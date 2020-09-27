/* File: A15050.C    Version: 1.2    Date: 08/21/92
 * CVS-C++, C++ Validation Suite
 * Section: 15  Type: A - Abort Expected Test
 * Copyright 1991-92, PERENNIAL, All Rights Reserved
 */
/*======================================================================
        =================== TESTPLAN SEGMENT ===================

>KEYS:  ISO/ANSI C++ draft, X3J16/92-0060.  15.6.1 The terminate() Function
        6.6.3 The return Statement
        ARM, Ellis & Stroustrup, May 91.  15.6.1 The terminate() Function

>REL:   EH Phase III and later.

>WHAT:  In 15.6.1:

        "Selecting a terminate function that does not in fact
        terminate but tries to return to its caller either with
        return or by throwing an exception is an error."
        
        and in 6.6.3:

        "Flowing off the end of a function is equivalent to a
        return with no value;"

        It is unclear that this error must be detected by the
        implementation.  The ARM states in 15.6.1:

        "Note that terminate() may enforce this rule by calling
        abort() should a user-supplied function return."

        Assertion:
        When the terminate function tries to return to its caller
        by flowing off the end of its definition, the function 
        abort() is called.

>HOW:   Call set_terminate() with the address of eh_term(), a
        function that flows off the end of its definition.
        Throw an exception with no handler to catch it.
        The driver should report termination with abort().

>NOTE:  This is an implementation dependent test. 
======================================================================*/
#include "tsthd.h"        /* Perennial test suite header file */
#include "tsteh.h"        /* Perennial exception handling header */

static char prgnam[] = "A15050.C";
/*--------------------------------------------------------------------*/

int pend(void);

struct S {
        int i;
        S() { i=0;}
};

static int xpath = 0;

static void eh_term(void)
{
        xpath |= 02;
        b4abort();
        CreateExitFile();
        // attempt to return to caller with an implicit return
}

/*--------------------------------------------------------------------*/
int a15050_main(void)
{
        S s;

        setup();
        eh_traps();             // set_terminate() and set_unexpected() 
        set_terminate (&eh_term);
/*--------------------------------------------------------------------*/
        blenter();      /* block 00 */

        fprintf(logfp, "REF: ISO/ANSI C++ draft, X3J16/92-0060. 15.6.1 The terminate() Function\n");
        fprintf(logfp, "The terminate function may not return to ");
        fprintf(logfp, "its caller by flowing\noff its end.\n");

        try {
                xpath |= 01;
                throw s;        // uncaught exception.
        }
        catch (char) {
                xpath = 040;    // Throw was caught by the wrong handler
        }
        failure ("eh_term() returned to its caller; xpath is 0%o", xpath);

        blexit();
/*--------------------------------------------------------------------*/
        anyfail(1);      
        return 0;
}

#include "scaffold.cxx"
