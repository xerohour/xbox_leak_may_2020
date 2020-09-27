/* File: P15036.C    Version: 1.3    Date: 05/27/92
 * CVS-C++, C++ Validation Suite
 * Section: 15  Type: P - Positive Execution Test
 * Copyright 1992, PERENNIAL, All Rights Reserved
 */
/*======================================================================
        =================== TESTPLAN SEGMENT ===================

>KEYS:  ISO/ANSI C++ draft, X3J16/92-0023.
        15 Exception Handling
        15.6 Special Functions
        15.6.1 The terminate() Function

>REL:   EH Phase II and later

>WHAT:  "Occasionally, exception handling must be abandoned for less
        subtle error handling techniques.  For example,
            - when the exception handling mechanism cannot find a
            handler for a thrown exception, 
            - when the exception handling mechanism finds the
            stack corrupted, [untestable] or
            - when a destructor called during stack unwinding caused by
            an exception tries to exit using an exception .

        In such cases,
            void terminate();
        is called; ... "

        This file only tests the case "when a destructor called during
        stack unwinding caused by an exception tries to exit using an
        exception."

>HOW:   Define a destructor for a thrown class that tries to exit
        using an exception.  Verify that terminate() gets invoked.

>NOTE:  None
======================================================================*/

#include "tsthd.h"        /* Perennial test suite header file */
#include "tsteh.h"        /* Perennial exception handling header */

static char prgnam[] = "P15036.C";

class C {
    public:
        C() { }
        ~C() { throw 'a'; }
};

static void eh_term(void)
{
        blexit();
        anyfail(1);
        CreateExitFile();
}

/*--------------------------------------------------------------------*/
int p15036_main(void)
{
        setup();
        eh_traps();             // set_terminate() and set_unexpected() 
        set_terminate(&eh_term);
/*--------------------------------------------------------------------*/
        blenter();      /* block 00 */

        fprintf(logfp,
          "REF: ISO/ANSI C++ draft, X3J16/92-0023.\n");
        fprintf(logfp, "15 Exception Handling, 15.6 Special Functions, ");
        fprintf(logfp, "15.6.1 The terminate() Function\n");
        fprintf(logfp, 
          "During unwinding, destructor cannot exit using an exception\n");

        try {
                C c;
                throw 1;
        }
        catch (int) {
                failure("Destructor was not invoked");
        }
        catch (char) {
                failure("Destructor exited using an exception");
        }
        catch (...) {
                failure("Throw caught by wrong handler");
        }
        failure("terminate() was not called");

        blexit();
/*--------------------------------------------------------------------*/
        anyfail(1);      
        return 0;
}

#include "scaffold.cxx"
