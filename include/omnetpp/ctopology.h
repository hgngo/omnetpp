//==========================================================================
//  CTOPOLOGY.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//     cTopology : network topology to find shortest paths etc.
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CTOPOLOGY_H
#define __OMNETPP_CTOPOLOGY_H

#include <string>
#include <vector>
#include "cownedobject.h"
#include "csimulation.h"
#include "cmodule.h"
#include "cgate.h"

NAMESPACE_BEGIN

class cPar;

// not all compilers define INFINITY (gcc does)
#ifndef INFINITY
#define INFINITY HUGE_VAL
#endif

/**
 * Routing support. The cTopology class was designed primarily to support
 * routing in telecommunication or multiprocessor networks.
 *
 * A cTopology object stores an abstract representation of the network
 * in graph form:
 * <ul>
 *   <li> each cTopology node corresponds to a module (simple or compound), and
 *   <li> each cTopology edge corresponds to a link or series of connecting links.
 * </ul>
 *
 * You can specify which modules (either simple or compound) you want to
 * include in the graph. The graph will include all connections among the
 * selected modules. In the graph, all nodes are at the same level, there is
 * no submodule nesting. Connections which span across compound module
 * boundaries are also represented as one graph edge. Graph edges are directed,
 * just as module gates are.
 *
 * @ingroup SimSupport
 * @see cTopology::Node, cTopology::Link, cTopology::LinkIn, cTopology::LinkOut
 */
class SIM_API cTopology : public cOwnedObject
{
  public:
    class Link;
    class LinkIn;
    class LinkOut;

    /**
     * Supporting class for cTopology, represents a node in the graph.
     */
    class SIM_API Node
    {
        friend class cTopology;

      protected:
        int moduleId;
        double weight;
        bool enabled;
        std::vector<Link*> inLinks;
        std::vector<Link*> outLinks;

        // variables used by the shortest-path algorithms
        double dist;
        Link *outPath;

      public:
        /**
         * Constructor
         */
        Node(int moduleId=-1) {this->moduleId=moduleId; weight=0; enabled=true; dist=INFINITY; outPath=NULL;}
        virtual ~Node() {}

        /** @name Node attributes: weight, enabled state, correspondence to modules. */
        //@{

        /**
         * Returns the ID of the network module to which this node corresponds.
         */
        int getModuleId() const  {return moduleId;}

        /**
         * Returns the pointer to the network module to which this node corresponds.
         */
        cModule *getModule() const  {return simulation.getModule(moduleId);}

        /**
         * Returns the weight of this node. Weight is used with the
         * weighted shortest path finder methods of cTopology.
         */
        double getWeight() const  {return weight;}

        /**
         * Sets the weight of this node. Weight is used with the
         * weighted shortest path finder methods of cTopology.
         */
        void setWeight(double d)  {weight=d;}

        /**
         * Returns true of this node is enabled. This has significance
         * with the shortest path finder methods of cTopology.
         */
        bool isEnabled() const  {return enabled;}

        /**
         * Enable this node. This has significance with the shortest path
         * finder methods of cTopology.
         */
        void enable()  {enabled=true;}

        /**
         * Disable this node. This has significance with the shortest path
         * finder methods of cTopology.
         */
        void disable()  {enabled=false;}
        //@}

        /** @name Node connectivity. */
        //@{

        /**
         * Returns the number of incoming links to this graph node.
         */
        int getNumInLinks() const  {return inLinks.size();}

        /**
         * Returns ith incoming link of graph node.
         */
        LinkIn *getLinkIn(int i);

        /**
         * Returns the number of outgoing links from this graph node.
         */
        int getNumOutLinks() const  {return outLinks.size();}

        /**
         * Returns ith outgoing link of graph node.
         */
        LinkOut *getLinkOut(int i);
        //@}

        /** @name Result of shortest path extraction. */
        //@{

        /**
         * Returns the distance of this node to the target node.
         */
        double getDistanceToTarget() const  {return dist;}

        /**
         * Returns the number of shortest paths towards the target node.
         * (There may be several paths with the same length.)
         */
        int getNumPaths() const  {return outPath?1:0;}

        /**
         * Returns the next link in the ith shortest paths towards the
         * target node. (There may be several paths with the same
         * length.)
         */
        LinkOut *getPath(int) const  {return (LinkOut *)outPath;}
        //@}
    };


    /**
     * Supporting class for cTopology, represents a link in the graph.
     */
    class SIM_API Link
    {
        friend class cTopology;

      protected:
        Node *srcNode;
        int srcGateId;
        Node *destNode;
        int destGateId;
        double weight;
        bool enabled;

      public:
        /**
         * Constructor.
         */
        Link(double weight=1) {srcNode=destNode=NULL; srcGateId=destGateId=-1; this->weight=weight; enabled=true;}
        virtual ~Link() {}

        /**
         * Returns the weight of this link. Weight is used with the
         * weighted shortest path finder methods of cTopology.
         */
        double getWeight() const  {return weight;}

        /**
         * Sets the weight of this link. Weight is used with the
         * weighted shortest path finder methods of cTopology.
         */
        void setWeight(double d)  {weight=d;}

        /**
         * Returns true of this link is enabled. This has significance
         * with the shortest path finder methods of cTopology.
         */
        bool isEnabled() const  {return enabled;}

        /**
         * Enables this link. This has significance with the shortest path
         * finder methods of cTopology.
         */
        void enable()  {enabled=true;}

        /**
         * Disables this link. This has significance with the shortest path
         * finder methods of cTopology.
         */
        void disable()  {enabled=false;}
    };


    /**
     * Supporting class for cTopology.
     *
     * While navigating the graph stored in a cTopology, Node's methods return
     * LinkIn and LinkOut objects, which are 'aliases' to Link objects.
     * LinkIn and LinkOut provide convenience functions that return the
     * 'local' and 'remote' end of the connection when traversing the topology.
     */
    class SIM_API LinkIn : public Link
    {
      public:
        /**
         * Returns the node at the remote end of this connection.
         */
        Node *getRemoteNode() const  {return srcNode;}

        /**
         * Returns the node at the local end of this connection.
         */
        Node *getLocalNode() const  {return destNode;}

        /**
         * Returns the gate ID at the remote end of this connection.
         */
        int getRemoteGateId() const  {return srcGateId;}

        /**
         * Returns the gate ID at the local end of this connection.
         */
        int getLocalGateId() const  {return destGateId;}

        /**
         * Returns the gate at the remote end of this connection.
         */
        cGate *getRemoteGate() const  {return srcNode->getModule()->gate(srcGateId);}

        /**
         * Returns the gate at the local end of this connection.
         */
        cGate *getLocalGate() const  {return destNode->getModule()->gate(destGateId);}
    };


    /**
     * Supporting class for cTopology.
     *
     * While navigating the graph stored in a cTopology, Node's methods return
     * LinkIn and LinkOut objects, which are 'aliases' to Link objects.
     * LinkIn and LinkOut provide convenience functions that return the
     * 'local' and 'remote' end of the connection when traversing the topology.
     */
    class SIM_API LinkOut : public Link
    {
      public:
        /**
         * Returns the node at the remote end of this connection.
         */
        Node *getRemoteNode() const  {return destNode;}

        /**
         * Returns the node at the local end of this connection.
         */
        Node *getLocalNode() const  {return srcNode;}

        /**
         * Returns the gate ID at the remote end of this connection.
         */
        int getRemoteGateId() const  {return destGateId;}

        /**
         * Returns the gate ID at the local end of this connection.
         */
        int getLocalGateId() const  {return srcGateId;}

        /**
         * Returns the gate at the remote end of this connection.
         */
        cGate *getRemoteGate() const  {return destNode->getModule()->gate(destGateId);}

        /**
         * Returns the gate at the local end of this connection.
         */
        cGate *getLocalGate() const  {return srcNode->getModule()->gate(srcGateId);}
    };

    /**
     * Base class for selector objects used in extract...() methods of cTopology.
     * Redefine the matches() method to return whether the given module
     * should be included in the extracted topology or not.
     */
    class SIM_API Predicate
    {
      public:
        virtual ~Predicate() {}
        virtual bool matches(cModule *module) = 0;
    };

  protected:
    std::vector<Node*> nodes;
    Node *target;

    // note: the purpose of the (unsigned int) cast is that nodes with moduleId==-1 are inserted at the end of the vector
    static bool lessByModuleId(Node *a, Node *b) { return (unsigned int)a->moduleId < (unsigned int)b->moduleId; }
    static bool isModuleIdLess(Node *a, int moduleId) { return (unsigned int)a->moduleId < (unsigned int)moduleId; }

    void unlinkFromSourceNode(Link *link);
    void unlinkFromDestNode(Link *link);

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    explicit cTopology(const char *name=NULL);

    /**
     * Copy constructor.
     */
    cTopology(const cTopology& topo);

    /**
     * Destructor.
     */
    virtual ~cTopology();

    /**
     * Assignment operator. The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cTopology& operator=(const cTopology& topo);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cTopology *dup() const  {return new cTopology(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer) const;

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer);
    //@}

    /** @name Extracting the topology from a network.
     *
     * extract...() functions build topology from the model.
     * User can select which modules to include. All connections
     * between those modules will be in the topology. Connections can
     * cross compound module boundaries.
     */
    //@{

    /**
     * Extracts model topology by a user-defined criteria. Includes into the graph
     * modules for which the passed selfunc() returns nonzero. The userdata
     * parameter may take any value you like, and it is passed back to selfunc()
     * in its second argument.
     */
    void extractFromNetwork(bool (*selfunc)(cModule *,void *), void *userdata=NULL);

    /**
     * The type safe, object-oriented equivalent of extractFromNetwork(selfunc, userdata).
     */
    void extractFromNetwork(Predicate *predicate);

    /**
     * Extracts model topology by module full path. All modules whole getFullPath()
     * matches one of the patterns in given string vector will get included.
     * The patterns may contain wilcards in the same syntax as in ini files.
     *
     * An example:
     *
     * <tt>topo.extractByModulePath(cStringTokenizer("**.host[*] **.router*").asVector());</tt>
     */
    void extractByModulePath(const std::vector<std::string>& fullPathPatterns);

    /**
     * Extracts model topology by the fully qualified NED type name of the
     * modules. All modules whose getNedTypeName() is listed in the given string
     * vector will get included.
     *
     * Note: If you have all class names as a single, space-separated string,
     * you can use cStringTokenizer to turn it into a string vector:
     *
     * <tt>topo.extractByNedTypeName(cStringTokenizer("some.package.Host other.package.Router").asVector());</tt>
     */
    void extractByNedTypeName(const std::vector<std::string>& nedTypeNames);

    /**
     * Extracts model topology by a module property. All modules get included
     * that have a property with the given name and the given value
     * (more precisely, the first value of its default key being the specified
     * value). If value is NULL, the property's value may be anything except
     * "false" (i.e. the first value of the default key may not be "false").
     *
     * For example, <tt>topo.extractByProperty("node");</tt> would extract
     * all modules that contain the <tt>\@node</tt> property, like the following
     * one:
     *
     * <pre>
     * module X {
     *     \@node;
     * }
     * </pre>
     *
     */
    void extractByProperty(const char *propertyName, const char *value=NULL);

    /**
     * Extracts model topology by a module parameter. All modules get included
     * that have a parameter with the given name, and the parameter's str()
     * method returns the paramValue string. If paramValue is NULL, only the
     * parameter's existence is checked but not its value.
     */
    void extractByParameter(const char *paramName, const char *paramValue=NULL);

    /**
     * Deletes the topology stored in the object.
     */
    void clear();
    //@}

    /** @name Manipulating the graph.
     */
    //@{
    /**
     * Adds the given node to the graph. Returns the index of the new graph node
     * (see getNode(int)). Indices of existing graph nodes may change.
     */
    int addNode(Node *node);

    /**
     * Removes the given node from the graph, together with all of its links.
     * Indices of existing graph nodes may change.
     */
    void deleteNode(Node *node);

    /**
     * Adds the given link to the graph, between the specified nodes.
     * If the link is already part of the graph it is removed first,
     * i.e. this method also serves as reconnectLink().
     */
    void addLink(Link *link, Node *srcNode, Node *destNode);

    /**
     * Adds the given link to the graph, between the nodes that correspond
     * to the specified gates. It is an error if the modules that contain
     * the gates are not parts of the graph. If the link is already part
     * of the graph it is removed first, i.e. this method also serves as
     * reconnectLink().
     */
    void addLink(Link *link, cGate *srcGate, cGate *destGate);

    /**
     * Removes the given link from the graph. Indices of existing links in the
     * source and destination nodes may change.
     */
    void deleteLink(Link *link);
    //@}


    /** @name Functions to examine topology by hand.
     *
     * Users also need to rely on Node and Link member functions
     * to explore the graph stored in the object.
     */
    //@{

    /**
     * Returns the number of nodes in the graph.
     */
    int getNumNodes() const  {return nodes.size();}

    /**
     * Returns pointer to the ith node in the graph. Node's methods
     * can be used to further examine the node's connectivity, etc.
     */
    Node *getNode(int i);

    /**
     * Returns the graph node which corresponds to the given module in the
     * network. If no graph node corresponds to the module, the method returns
     * NULL. This method assumes that the topology corresponds to the
     * network, that is, it was probably created with one of the
     * extract...() functions.
     */
    Node *getNodeFor(cModule *mod);
    //@}

    /** @name Algorithms to find shortest paths. */
    /*
     * To be implemented:
     *    -  void unweightedMultiShortestPathsTo(Node *target);
     *    -  void weightedMultiShortestPathsTo(Node *target);
     */
    //@{

    /**
     * Apply the Dijkstra algorithm to find all shortest paths to the given
     * graph node. The paths found can be extracted via Node's methods.
     */
    void calculateUnweightedSingleShortestPathsTo(Node *target);

    /**
     * Apply the Dijkstra algorithm to find all shortest paths to the given
     * graph node. The paths found can be extracted via Node's methods.
     * Uses weights in nodes and links.
     */
    void calculateWeightedSingleShortestPathsTo(Node *target);

    /**
     * Returns the node that was passed to the most recently called
     * shortest path finding function.
     */
    Node *getTargetNode() const {return target;}
    //@}

  protected:
    /**
     * Node factory.
     */
    virtual Node *createNode(cModule *module) { return new Node(module->getId()); }

    /**
     * Link factory.
     */
    virtual Link *createLink() { return new Link(); }
};

NAMESPACE_END

#endif