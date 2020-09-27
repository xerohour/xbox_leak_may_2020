/* File: A15039.C    Version: 2.10    Date: 08/21/92
 * CVS-C++, C++ Validation Suite
 * Section: 15  Type: A - Abort Expected Test
 * Copyright 1991-92, PERENNIAL, All Rights Reserved
 */
/*======================================================================
        =================== TESTPLAN SEGMENT ===================

>KEYS:  ISO/ANSI C++ draft, X3J16/92-0060. 15.6.1 The terminate() Function.

>REL:   EH Phase I and later.

>WHAT:  "The default function called by terminate() is abort()."

>HOW:   Do not call set_terminate().
        Throw a handler-less exception.  Verify that 
        there is no return from terminate().
        The driver should report termination with abort().

>NOTE:  None.
======================================================================*/
#include "tsthd.h"        /* Perennial test suite header file */
#include "tsteh.h"        /* Perennial exception handling header */
#include <stdlib.h>

static char prgnam[] = "A15039.C";

static void f00(void)
{
        try {
                b4abort();
                throw 'a';      // throw should cause a call to abort()
        }
        catch (float) {
                failure("Throw caught by the wrong handler");
        }
}

/*--------------------------------------------------------------------*/
int a15039_main(void)
{
        setup();
/*--------------------------------------------------------------------*/
        blenter();      /* block 00 */

        fprintf(logfp, 
          "REF: ISO/ANSI C++ draft, X3J16/92-0060.  15.6.1 The terminate() Function\n");
        fprintf(logfp, 
          "The default function called by terminate() is abort().\n");

        f00();
        failure ("Execution should have aborted.");

        blexit();
/*--------------------------------------------------------------------*/
        anyfail(1);      
        return 0;
}

#include "scaffold.cxx"
