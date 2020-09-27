/* File: A15040.C    Version: 2.9    Date: 08/21/92
 * CVS-C++, C++ Validation Suite
 * Section: 15  Type: A - Abort Expected Test
 * Copyright 1991-92, PERENNIAL, All Rights Reserved
 */
/*======================================================================
        =================== TESTPLAN SEGMENT ===================

>KEYS:  ISO/ANSI C++ draft, X3J16/92-0060.  15.6.1 The terminate() Function
        ARM, Ellis & Stroustrup, May 91.  15.6.1 The terminate() Function

>REL:   EH Phase I and later.

>WHAT:  "Selecting a terminate function that does not in fact
        terminate but tries to return to its caller is an error."
        
        It is unclear that this error must be detected by the
        implementation.  From the ARM 15.6.1:

        "Note that terminate() may enforce this rule by calling
        abort() should a user-supplied function return."

>HOW:   Call set_terminate() with the address of eh_term(), a
        function that attempts to return. 
        Throw a handler-less exception.  Verify that 
        eh_term() gets invoked, yet does not return since
        there must be no return from terminate().
        The driver should report termination with abort().

>NOTE:  This is an implementation dependent test. 
======================================================================*/
#include "tsthd.h"        /* Perennial test suite header file */
#include "tsteh.h"        /* Perennial exception handling header */

static char prgnam[] = "A15040.C";

class C {};
static C c;

int global_x = 0;

int f00(void)
{
        int r = 0;
        try {
                throw c;        // uncaught exception.
        }
        catch (float) {
                r = 99;
        }
        return r;
}

static void eh_term(void)
{
        global_x = 1;
        b4abort();
        CreateExitFile();
        return;                 // return not permitted
}

/*--------------------------------------------------------------------*/
int a15040_main(void)
{
        int ret;

        setup();
        eh_traps();
        set_terminate (&eh_term);
/*--------------------------------------------------------------------*/
        blenter();      /* block 00 */

        fprintf(logfp,
          "REF: ISO/ANSI C++ draft, X3J16/92-0060.  15.6.1 The terminate() Function\n");
        fprintf(logfp,
          "Can an argument of set_terminate() return to its caller?\n");

        if (global_x != 0)
            failure ("eh_term() already invoked.");
        else 
        {
            ret = f00();
            if (global_x != 1)
                failure ("eh_term() not invoked.");
            else if (ret == 99)
                failure ("Wrong catch().");
            failure ("Execution should have aborted.");
        }

        blexit();
/*--------------------------------------------------------------------*/
        anyfail(1);      
        return 0;
}

#include "scaffold.cxx"
