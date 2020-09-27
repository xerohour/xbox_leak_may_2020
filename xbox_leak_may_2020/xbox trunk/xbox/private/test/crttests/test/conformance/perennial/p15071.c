/* File: P15071.C    Version: 1.1    Date: 08/18/92
 * CVS-C++, C++ Validation Suite
 * Section: 15  Type: P - Positive Execution Test
 * Copyright 1992, PERENNIAL, All Rights Reserved
 */
/*======================================================================
        =================== TESTPLAN SEGMENT ===================

>KEYS:  ISO/ANSI C++ draft, X3J16/92-0060.  15.6.1 The terminate() Function

>REL:   EH Phase III and later

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

        This file only tests the case "when the exception handling
        mechanism cannot find a handler for a thrown exception".

        Assertion:
        When the exception handling mechanism cannot find a handler
        for a thrown exception, the function terminate() is called.

>HOW:   Throw an exception in a try-block for which no handler is
        provided.  Verify that terminate() gets invoked.

>NOTE:  None
======================================================================*/

#include "tsthd.h"        /* Perennial test suite header file */
#include "tsteh.h"        /* Perennial exception handling header */

static char prgnam[] = "P15071.C";

static void eh_term(void)
{
        blexit();
        anyfail(1);
        CreateExitFile();
}

/*--------------------------------------------------------------------*/
int p15071_main(void)
{
        setup();
        eh_traps();             // set_terminate() and set_unexpected() 
        set_terminate(&eh_term);
/*--------------------------------------------------------------------*/
        blenter();      /* block 00 */

        fprintf(logfp, "REF: ISO/ANSI C++ draft, X3J16/92-0060. 15.6.1 The terminate() Function\n");
        fprintf(logfp, "terminate() is called if no handler can be found.\n");

        try {
                throw 1;
        }
        catch (char) {
                failure("control passed to wrong handler");
        }
        failure("terminate() was not called");

        blexit();
/*--------------------------------------------------------------------*/
        anyfail(1);      
        return 0;
}

#include "scaffold.cxx"
