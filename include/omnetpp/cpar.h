//==========================================================================
//   CPAR.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CPAR_H
#define __OMNETPP_CPAR_H

#include "cownedobject.h"
#include "cexpression.h"
#include "cexception.h"

namespace omnetpp {

class cParImpl;
class cExpression;
class cXMLElement;
class cProperties;
class cComponent;

/**
 * @brief Represents a module or channel parameter.
 *
 * When a module or channel is created, parameter objects are added
 * automatically, based on the NED declaration of the module/channel.
 * It is not possible to create further parameters (or to remove parameters)
 * at runtime. This is enforced by the cPar constructor being private.
 *
 * Parameters get their initial values automatically, from the NED
 * declarations and the configuration. It is possible to change the
 * parameter value during runtime (see various setter methods and
 * operator='s), but not the type of the parameter (see getType()).
 * The type correspond to NED types (bool, double, long, string, xml),
 * and cannot be changed at runtime.
 *
 * The module or channel object can get notified when a parameter is
 * changed; one has to override cComponent::handleParameterChange()
 * for that.
 *
 * <b>Note:</b> In earlier versions of \opp, cPar could be used as a
 * general value storage object, and attached to cMessages as well.
 * From the 4.0 version, simulation models should use cMsgPar for that.
 *
 * <b>Implementation note:</b> from the 4.0 version, almost all methods
 * of cPar delegates to an internal cParImpl object, which actually stores
 * the value, and generally does the real job. This was done to allow
 * sharing parameter objects which have the same name, same value, etc.
 * among module/channel instances. This significantly reduces memory
 * consumption of most simulation models. Because cPar is just a thin
 * wrapper around cParImpl, cPar is not meant for subclassing, and
 * none if its methods are virtual. cParImpl and subclasses should also
 * be regarded as internal data structures, and they should not be
 * directly accessed or manipulated from model code.
 *
 * @ingroup SimCore
 */
class SIM_API cPar : public cObject
{
    friend class cComponent;
  public:
    enum Type {
        BOOL = 'B',
        DOUBLE = 'D',
        LONG = 'L',
        STRING = 'S',
        XML = 'X'
    };

  private:
    cComponent *ownerComponent;
    cParImpl *p;
    cComponent *evalContext;

  private:
    // private constructor and destructor -- only cComponent is allowed to create parameters
    cPar() {ownerComponent = evalContext = nullptr; p = nullptr;}
    virtual ~cPar();
    // internal, called from cComponent
    void init(cComponent *ownercomponent, cParImpl *p);
    // internal
    void moveto(cPar& other);
    // internal: called each time before the value of this object changes.
    void beforeChange();
    // internal: called each time after the value of this object changes.
    void afterChange();

  public:
    // internal, used by cComponent::finalizeParameters()
    void read();
    // internal, used by cComponent::finalizeParameters()
    void finalize();
    // internal: applies the default value if there is one
    void acceptDefault();
    // internal
    void setImpl(cParImpl *p);
    // internal
    cParImpl *impl() const {return p;}
    // internal
    cParImpl *copyIfShared();

#ifdef SIMFRONTEND_SUPPORT
    // internal
    virtual bool hasChangedSince(int64_t lastRefreshSerial);
#endif

  public:
    /** @name Redefined cObject methods */
    //@{
    /**
     * Assignment operator.
     */
    void operator=(const cPar& other);

    /**
     * Returns the parameter name.
     */
    virtual const char *getName() const override;

    /**
     * Returns the value in text form.
     */
    virtual std::string str() const override;

    /**
     * Returns the component (module/channel) this parameter belongs to.
     * Note: return type is cObject only for technical reasons, it can be
     * safely cast to cComponent.
     */
    virtual cObject *getOwner() const override; // note: cannot return cComponent* (covariant return type) due to declaration order

    /**
     * Calls v->visit(this) for contained objects.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v) override;
    //@}

    /** @name Type, flags. */
    //@{
    /**
     * Returns the parameter type
     */
    Type getType() const;

    /**
     * Returns the given type as a string.
     */
    static const char *getTypeName(Type t);

    /**
     * Returns true if the stored value is of a numeric type.
     */
    bool isNumeric() const;

    /**
     * Returns true if this parameter is marked in the NED file as "volatile".
     * This flag affects the operation of setExpression().
     */
    bool isVolatile() const;

    /**
     * Returns false if the stored value is a constant, and true if it is
     * an expression. (It is not examined whether the expression yields
     * a constant value.)
     */
    bool isExpression() const;

    /**
     * Returns true if the parameter value expression is shared among several
     * modules to save memory. This flag is purely informational, and whether
     * a parameter is shared or not does not affect operation at all.
     */
    bool isShared() const;

    /**
     * Returns true if the parameter is assigned a value, and false otherwise.
     * Parameters of an already initialized module or channel are guaranteed to
     * assigned, so this method will return true for them.
     */
    bool isSet() const;

    /**
     * Returns true if the parameter is set (see isSet()) or contains a default
     * value, and false otherwise. Parameters of an already initialized module or
     * channel are guaranteed to be assigned, so this method will return true for them.
     */
    bool containsValue() const;

    /**
     * Return the properties for this parameter. Properties cannot be changed
     * at runtime.
     */
    cProperties *getProperties() const;
    //@}

    /** @name Setter functions. Note that overloaded assignment operators also exist. */
    //@{

    /**
     * Sets the value to the given bool value.
     */
    cPar& setBoolValue(bool b);

    /**
     * Sets the value to the given long value.
     */
    cPar& setLongValue(long l);

    /**
     * Sets the value to the given double value.
     */
    cPar& setDoubleValue(double d);

    /**
     * Sets the value to the given string value.
     * The cPar will make its own copy of the string. nullptr is also accepted
     * and treated as an empty string.
     */
    cPar& setStringValue(const char *s);

    /**
     * Sets the value to the given string value.
     */
    cPar& setStringValue(const std::string& s)  {setStringValue(s.c_str()); return *this;}

    /**
     * Sets the value to the given cXMLElement.
     */
    cPar& setXMLValue(cXMLElement *node);

    /**
     * Sets the value to the given expression. This object will assume
     * the responsibility to delete the expression object.
     *
     * The evalcontext parameter determines the module or channel in the
     * context of which the expression will be evaluated. If evalcontext
     * is nullptr, the owner of this parameter will be used.
     *
     * Note: if the parameter is marked as non-volatile (isVolatile()==false),
     * one should not set an expression as value. This is not enforced
     * by cPar though.
     *
     * @see getOwner(), getEvaluationContext(), setEvaluationContext()
     */
    cPar& setExpression(cExpression *e, cComponent *evalcontext=nullptr);

    /**
     * If the parameter contains an expression (see isExpression()), this method
     * sets the evaluation context for the expression.
     *
     * @see getEvaluationContext(), isExpression(), setExpression()
     */
    void setEvaluationContext(cComponent *ctx)  {evalContext = ctx;}
    //@}

    /** @name Getter functions. Note that overloaded conversion operators also exist. */
    //@{

    /**
     * Returns value as a boolean. The cPar type must be BOOL.
     */
    bool boolValue() const;

    /**
     * Returns value as intpar_t. The cPar type must be LONG or DOUBLE.
     */
    intpar_t longValue() const;

    /**
     * Returns value as double. The cPar type must be LONG or DOUBLE.
     */
    double doubleValue() const;

    /**
     * Returns the parameter's unit ("s", "mW", "Hz", "bps", etc),
     * as declared with the @unit property of the parameter in NED,
     * or nullptr if no unit was specified. Unit is only valid for LONG and DOUBLE
     * types.
     */
    const char *getUnit() const;

    /**
     * Returns value as const char *. The cPar type must be STRING.
     * This method may only be invoked when the parameter's value is a
     * string constant and not the result of expression evaluation, otherwise
     * an error is thrown. This practically means this method cannot be used
     * on parameters declared as "volatile string" in NED; they can only be
     * accessed using stdstringValue().
     */
    const char *stringValue() const;

    /**
     * Returns value as string. The cPar type must be STRING.
     */
    std::string stdstringValue() const;

    /**
     * Returns value as pointer to cXMLElement. The cPar type must be XML.
     *
     * The lifetime of the returned object tree is undefined, but it is
     * valid at least until the end of the current simulation event or
     * initialize() call. Modules are expected to process their XML
     * configurations at once (within one event or within initialize()),
     * and not hang on to pointers returned from this method. The reason
     * for the limited lifetime is that this method may return pointers to
     * objects stored in an internal XML document cache, and the simulation
     * kernel reserves the right to discard cached XML documents at any time
     * to free up memory, and re-load them on demand (i.e. when xmlValue() is
     * called again).
     */
    cXMLElement *xmlValue() const;

    /**
     * Returns pointer to the expression stored by the object, or nullptr.
     */
    cExpression *getExpression() const;

    /**
     * If the parameter contains an expression, this method returns the
     * module or channel in the context of which the expression will be
     * evaluated. (The context affects the resolution of parameter
     * references, and NED operators like <tt>index</tt> or <tt>sizeof()</tt>.)
     * If the parameter does not contain an expression, the return value is
     * undefined.
     *
     * @see isExpression(), setEvaluationContext()
     */
    cComponent *getEvaluationContext() const  {return evalContext;}
    //@}

    /** @name Miscellaneous utility functions. */
    //@{
    /**
     * For non-const values, replaces the stored expression with its
     * evaluation.
     */
    void convertToConst();

    /**
     * Converts the value from string, and stores the result.
     * If the text cannot be parsed, an exception is thrown, which
     * can be caught as std::runtime_error& if necessary.
     *
     * Note: this method understands expressions too, but does NOT handle
     * the special values "default" and "ask".
     */
    void parse(const char *text);
    //@}

    /** @name Overloaded assignment and conversion operators. */
    //@{

    /**
     * Equivalent to setBoolValue().
     */
    cPar& operator=(bool b)  {return setBoolValue(b);}

    /**
     * Delegates to setLongValue().
     */
    cPar& operator=(char c)  {return setLongValue(c);}

    /**
     * Delegates to setLongValue().
     */
    cPar& operator=(unsigned char c)  {return setLongValue(c);}

    /**
     * Delegates to setLongValue().
     */
    cPar& operator=(int i)  {return setLongValue(i);}

    /**
     * Delegates to setLongValue().
     */
    cPar& operator=(unsigned int i)  {return setLongValue(i);}

    /**
     * Delegates to setLongValue().
     */
    cPar& operator=(short i)  {return setLongValue(i);}

    /**
     * Delegates to setLongValue().
     */
    cPar& operator=(unsigned short i)  {return setLongValue(i);}

    /**
     * Equivalent to setLongValue().
     */
    cPar& operator=(long i)  {return setLongValue(i);}

    /**
     * Delegates to setLongValue().
     */
    cPar& operator=(unsigned long i) {return setLongValue(i);}

    /**
     * Delegates to setLongValue().
     */
    cPar& operator=(long long i)  {return setLongValue(i);}

    /**
     * Delegates to setLongValue().
     */
    cPar& operator=(unsigned long long i)  {return setLongValue(i);}

    /**
     * Equivalent to setDoubleValue().
     */
    cPar& operator=(double d)  {return setDoubleValue(d);}

    /**
     * Converts the argument to double, and calls setDoubleValue().
     */
    cPar& operator=(long double d)  {return setDoubleValue((double)d);}

    /**
     * Equivalent to setStringValue().
     */
    cPar& operator=(const char *s)  {return setStringValue(s);}

    /**
     * Equivalent to setStringValue().
     */
    cPar& operator=(const std::string& s)  {return setStringValue(s);}

    /**
     * Equivalent to setXMLValue().
     */
    cPar& operator=(cXMLElement *node)  {return setXMLValue(node);}

    /**
     * Equivalent to boolValue().
     */
    operator bool() const  {return boolValue();}

    /**
     * Calls longValue() and converts the result to char.
     */
    operator char() const  {return (char)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned char.
     */
    operator unsigned char() const  {return (unsigned char)longValue();}

    /**
     * Calls longValue() and converts the result to int.
     */
    operator int() const  {return (int)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned int.
     */
    operator unsigned int() const  {return (unsigned int)longValue();}

    /**
     * Calls longValue() and converts the result to short.
     */
    operator short() const  {return (short)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned short.
     */
    operator unsigned short() const  {return (unsigned short)longValue();}

    /**
     * Calls longValue() and converts the result to long.
     */
    operator long() const  {return longValue();}

    /**
     * Calls longValue() and converts the result to unsigned long.
     */
    operator unsigned long() const  {return longValue();}

    /**
     * Calls longValue() and converts the result to long long.
     */
    operator long long() const  {return longValue();}

    /**
     * Calls longValue() and converts the result to unsigned long long.
     */
    operator unsigned long long() const  {return longValue();}

    /**
     * Equivalent to doubleValue().
     */
    operator double() const  {return doubleValue();}

    /**
     * Calls doubleValue() and converts the result to long double.
     */
    operator long double() const  {return doubleValue();}

    /**
     * Equivalent to stringValue().
     */
    operator const char *() const  {return stringValue();}

    /**
     * Equivalent to stdstringValue().
     */
    operator std::string() const  {return stdstringValue();}

    /**
     * Equivalent to xmlValue(). NOTE: The lifetime of the returned object tree
     * is limited; see xmlValue() for details.
     */
    operator cXMLElement *() const  {return xmlValue();}
    //@}
};

}  // namespace omnetpp


#endif




