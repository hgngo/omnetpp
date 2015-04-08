//==========================================================================
//  ONSTARTUP.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  supporting class for EXECUTE_ON_STARTUP macro
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "omnetpp/onstartup.h"

NAMESPACE_BEGIN

#if NDEBUG
extern "C" SIM_API bool __is_release_oppsim__;
bool __is_release_oppsim__ = true;
#endif

// Checks whether an other instance of the shared library has already been loaded.
// This can happen when some parts of the simulation are linked with debug
// while others are linked with the release version of the oppsim shared library.
// This is an error: we must tell the user about it, and abort.
class StartupChecker
{
    public:
        StartupChecker()
        {
            char *loaded = getenv("__OPPSIM_LOADED__");
            if (loaded != NULL && strcmp(loaded, "yes") == 0 ) {
                fprintf(stderr,
                       "\n<!> Error: Attempt to load the oppsim shared library more than once. "
                       "This usually happens when part of your simulation is using release libraries "
                       "while other parts are using the debug version. Make sure to rebuild all parts "
                       "of your model in either release or debug mode!\n");
                exit(1);
            }

            // we set the environment variable so a new instance will be able to
            // detect it (note: there is no setenv() on windows)
            putenv((char *)"__OPPSIM_LOADED__=yes");
        }
};

StartupChecker startupChecker;


CodeFragments *CodeFragments::head;


CodeFragments::CodeFragments(void (*code)(), Type type) : type(type), code(code)
{
    // add to list
    next = head;
    head = this;
}

CodeFragments::~CodeFragments()
{
}

void CodeFragments::executeAll(Type type)
{
    CodeFragments *p = CodeFragments::head;
    while (p)
    {
        if (p->type == type && p->code != NULL)
        {
            p->code();
            p->code = NULL;  // do it only once (executeAll() may be called multiple times, e.g. after dlopen() / LoadLibrary() calls)
        }
        p = p->next;
    }
}

NAMESPACE_END

