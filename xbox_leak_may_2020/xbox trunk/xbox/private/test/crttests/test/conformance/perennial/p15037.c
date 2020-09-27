/* File: P15037.C    Version: 2.7    Date: 09/08/92
 * CVS-C++, C++ Validation Suite
 * Section: 15  Type: P - Positive Execution Test
 * Copyright 1991-92, PERENNIAL, All Rights Reserved
 */
/*======================================================================
        =================== TESTPLAN SEGMENT ===================

>KEYS:  ISO/ANSI C++ draft, X3J16/92-0023.
        15 Exception Handling.
        15.6 Special Functions.
        15.6.1 The terminate() Function.

>REL:   EH Phase I and later.

>WHAT:  "... terminate() calls the last function given as an
        argument to set_terminate():
        
                typedef void (*PFV) ();
                PFV set_terminate (PFV); "

>HOW:   Pass the function foo() as the last argument to 
        set_terminate().  Cause a call to terminate()
        by throwing an exception, with no corresponding 
        handler.

>NOTE:  None.
======================================================================*/
#include "tsthd.h"        /* Perennial test suite header file */
#include "tsteh.h"        /* Perennial exception handling header */

static char prgnam[] = "P15037.C";

class C {};
static C c;

static void f00(void)
{
        try {
                throw c;
        }
        catch (float) {
                failure("Throw caught by wrong handler");
        }
}

void old_eh_term(void)
{
        failure("terminate() called old_eh_term() instead of eh_term()");
        blexit();
        anyfail(1);
        CreateExitFile();
}

static void eh_term(void)
{
        blexit();
        anyfail(1);
        CreateExitFile();
}

/*--------------------------------------------------------------------*/
int p15037_main(void)
{
        setup();
        eh_traps();
        set_terminate (&old_eh_term);
        set_terminate (&eh_term);
/*--------------------------------------------------------------------*/
        blenter();      /* block 00 */

        fprintf(logfp,
          "REF: ISO/ANSI C++ draft, X3J16/92-0023.\n");
        fprintf(logfp,
         "15 Exception Handling, 15.6.1 The terminate() Function\n");
        fprintf(logfp,
          "terminate() calls the last function given as an argument\n");
        fprintf(logfp,
          "to set_terminate()\n");

        f00();
        failure ("terminate() did not call any function");

        blexit();
/*--------------------------------------------------------------------*/
        anyfail(1);      
        return 0;
}

#include "scaffold.cxx"
