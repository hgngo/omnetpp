//==========================================================================
//   MACROS.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Definition of the following macros:
//    Define_Network, Define_Link
//    ModuleInterface..End
//    Define_Module
//    Module_Class_Members
//    Define_Function
//    Register_Class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MACROS_H
#define __MACROS_H

#include "ctypes.h"

//=========================================================================

/**
 * @name Declaration macros
 * @ingroup Macros
 */
//@{

/**
 * Network declaration macro. It can be found in code generated by the
 * NEDC compiler. The use of this macro allows the creation of a network
 * when only its name is available as a string. (Typically, the name of the
 * network to be simulated is read from the configuration file.)
 * The macro expands to the definition of a cNetworkType object.
 *
 * @hideinitializer
 */
#define Define_Network(NAME,SETUPFUNC) \
  void SETUPFUNC(); \
  static cNetworkType NAME##__network(#NAME,SETUPFUNC);

/**
 * Link type definition. The macro expands to the definition of a cLinkType object;
 * the last three arguments are pointers to functions which dynamically create cPar
 * objects an return their pointers.
 *
 * @hideinitializer
 */
#define Define_Link(NAME,DELAY,ERROR,DATARATE) \
  static cLinkType NAME##__link(#NAME, DELAY, ERROR, DATARATE);

/**
 * Registers a mathematical function that takes 0, 1, 2 or 3 double arguments
 * and returns a double. The use of this macro allows the function to be used
 * in expressions inside NED network descriptions.
 *
 * Commonly used <math.h> functions have Define_Function() lines in the OMNeT++
 * simulation kernel.
 *
 * @hideinitializer
 */
#define Define_Function(FUNCTION,ARGCOUNT) \
  cFunctionType FUNCTION##__functype(#FUNCTION,(MathFunc)FUNCTION,ARGCOUNT);

/**
 * Register class. This defines a factory object which makes it possible
 * to create an object by the passing class name to the createOne() function.
 *
 * @hideinitializer
 */
#define Register_Class(CLASSNAME) \
  cObject *CLASSNAME##__create() {return new CLASSNAME;} \
  cClassRegister CLASSNAME##__reg(#CLASSNAME,CLASSNAME##__create);

/**
 * Register inspector factory. Used internally by graphical interface
 * libraries like Tkenv.
 *
 * @hideinitializer
 */
#define Register_InspectorFactory(FACTORYNAME,FUNCTION) \
  TInspector *FUNCTION(cObject *, int, void *); \
  cInspectorFactory FACTORYNAME##__inspfct(#FACTORYNAME,FUNCTION);
//@}

//=========================================================================

/**
 * @name Module declaration macros
 * @ingroup Macros
 */
//@{

/**
 * Announces the class as a module to OMNeT++ and couples it with the
 * NED interface of the same name. The macro expands to the definition
 * a cModuleType object.
 *
 * The NEDC compiler generates Define_Module() lines for all compound modules.
 * However, it is the user's responsibility to put Define_Module() lines for
 * all simple module types into one of the C++ sources.
 *
 * @hideinitializer
 */
#define Define_Module(CLASSNAME) \
  static cModule *CLASSNAME##__create(const char *name, cModule *parentmod ) \
  { \
     return (cModule *) new CLASSNAME(name, parentmod); \
  } \
  cModuleType CLASSNAME##__type(#CLASSNAME,#CLASSNAME,(ModuleCreateFunc)CLASSNAME##__create);

/**
 * Similar to Define_Module(), except that it couples the class with the
 * NED interface of the given name. This macro is used in connection with
 * the 'like' phrase in NED.
 *
 * @hideinitializer
 */
#define Define_Module_Like(CLASSNAME,INTERFACE) \
  static cModule *CLASSNAME##__create(const char *name, cModule *parentmod ) \
  { \
     return (cModule *) new CLASSNAME(name, parentmod); \
  } \
  cModuleType CLASSNAME##__type(#CLASSNAME,#INTERFACE,(ModuleCreateFunc)CLASSNAME##__create);

/**
 * This macro facilitates the declaration of a simple module class.
 * The macro expands to the definition of member functions which the user does
 * not need to worry about: constructors, destructor, className() function etc.
 * The user can derive the new class from an existing simple module class
 * (not only cSimpleModule), add new data members and add/redefine member functions
 * as needed.
 *
 * The macro is used like this:
 *
 * <PRE>
 *  class CLASSNAME : public cSimpleModule
 *  {
 *     Module_Class_Members(CLASSNAME,cSimpleModule,8192)
 *     virtual void activity();
 *  };
 * </PRE>
 *
 * @hideinitializer
 */
#define Module_Class_Members(CLASSNAME,BASECLASS,STACK) \
    public: \
      CLASSNAME(const char *name, cModule *parentmod, unsigned stk=STACK) : \
           BASECLASS(name, parentmod, stk) {} \
      virtual const char *className() const {return #CLASSNAME;}
//@}

//=========================================================================

// internal: declaration of a module interface (module gates and params)
// example:
//    Interface(CLASSNAME)
//        Gate(NAME,TYPE)
//        Parameter(NAME,TYPES)
//        Machine(NAME)
//    EndInterface
//
#define Interface(CLASSNAME)    static sDescrItem CLASSNAME##__descr[] = {
#define Gate(NAME,TYPE)         {'G', #NAME, NULL,  TYPE},
#define Parameter(NAME,TYPES)   {'P', #NAME, TYPES, 0   },
#define Machine(NAME)           {'M', #NAME, NULL,  0   },
#define EndInterface            {'E', NULL,  NULL,  0   }};

// internal: registers a module interface specified with the Interface..EndInterface macros
#define Register_Interface(CLASSNAME) \
  static cModuleInterface CLASSNAME##__interface( #CLASSNAME, CLASSNAME##__descr);

// internal: gate types. To be used with module interface declarations.
#define GateDir_Input      'I'
#define GateDir_Output     'O'

// internal: parameter allowed types. To be used with module interface declarations.
#define ParType_Const      "#"
#define ParType_Any        "*"
#define ParType_Numeric    "LDXFT"
#define ParType_Bool       "B"
#define ParType_String     "S"

#endif

