/* File: A15053.C    Version: 1.2    Date: 08/24/92
 * CVS-C++, C++ Validation Suite
 * Section: 15  Type: A - Abort Expected Test
 * Copyright 1991-92, PERENNIAL, All Rights Reserved
 */
/*======================================================================
        =================== TESTPLAN SEGMENT ===================

>KEYS:  ISO/ANSI C++ draft, X3J16/92-0060.  15.6.1 The terminate() Function
        ARM, Ellis & Stroustrup, May 91.  15.6.1 The terminate() Function

>REL:   EH Phase III and later.

>WHAT:  "Selecting a terminate function that does not in fact
        terminate but tries to return to its caller either with
        return or by throwing an exception is an error."
        
        The ARM states in 15.6.1:

        "Note that terminate() may enforce this rule by calling
        abort() should a user-supplied function return."

        Assertion:
        When a terminate function tries to return to its caller
        by indirectly throwing an exception, the function abort()
        is called.

>HOW:   Call set_terminate() with the address of eh_term(), a
        function that attempts to return to its caller by calling 
        a function that throws an exception.
        Throw an exception with no handler to catch it.
        The driver should report termination with abort().

>NOTE:  This is an implementation dependent test. 
======================================================================*/
#include "tsthd.h"        /* Perennial test suite header file */
#include "tsteh.h"        /* Perennial exception handling header */

static char prgnam[] = "A15053.C";
/*--------------------------------------------------------------------*/
struct S {
        int i;
        S() {i=1;}
};

static int xpath = 0;          // the bits of xpath indicate the execution path

static void f00(void)
{
        try {
                xpath |= 01;
                throw 'a';      // throw of uncaught exception
        }
        catch (float) {
                xpath |= 0200;  // throw of 'a' caught by wrong handler
        }
}

void g(void)
{
        S s;
        xpath |= 04;
        b4abort();
        throw s;        // throw is prohibited
}

static void eh_term(void)
{
        xpath |= 02;
        CreateExitFile();
        g();            // indirect throw
        failure ("Throw from eh_term() was ignored");
        blexit();
        anyfail(1);
}

/*--------------------------------------------------------------------*/
int a15053_main(void)
{
        setup();
        eh_traps();             // set_terminate() and set_unexpected() 
        set_terminate (&eh_term);
/*--------------------------------------------------------------------*/
        blenter();      /* block 00 */

        fprintf(logfp, 
          "REF: ISO/ANSI C++ draft, X3J16/92-0060.  15.6.1 The terminate() Function.\n");
        fprintf(logfp, 
          "A terminate function may not return to its caller ");
        fprintf(logfp, 
          "by throwing an exception.\n");

        f00();
        failure ("abort() was not called; xpath is 0%o", xpath);

        blexit();
/*--------------------------------------------------------------------*/
        anyfail(1);      
        return 0;
}

#include "scaffold.cxx"
