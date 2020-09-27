/* File: P15038.C    Version: 2.6    Date: 05/27/92
 * CVS-C++, C++ Validation Suite
 * Section: 15	Type: P - Positive Execution Test
 * Copyright 1991-92, PERENNIAL, All Rights Reserved
 */
/*======================================================================
        =================== TESTPLAN SEGMENT ===================

>KEYS:	ISO/ANSI C++ draft, X3J16/92-0023.
	15 Exception Handling.
	15.6 Special Functions.
	15.6.1 The terminate() Function.

>REL:	EH Phase I and later.

>WHAT:	"The previous function given to set_terminate() will be
	the return value;  this enables users to implement a stack
	strategy for using terminate()."

>HOW:	Call set_terminate() N+1 times, each with a unique
	function address, and verify that set_terminate()
	returns the previous address each time.

>NOTE:	None.
======================================================================*/
#include "tsthd.h"     	  /* Perennial test suite header file */
#include "tsteh.h"        /* Perennial exception handling header */

static char prgnam[] = "P15038.C";

static void eh_term0(void) { }
static void eh_term1(void) { }
static void eh_term2(void) { }
static void eh_term3(void) { }

/*--------------------------------------------------------------------*/
int p15038_main(void)
{
	PFV ret;

        setup();
        eh_traps();
/*--------------------------------------------------------------------*/
	blenter();	/* block 00 */

        fprintf(logfp,
	  "REF: ISO/ANSI C++ draft, X3J16/92-0023.\n");
        fprintf(logfp,
	  "15 Exception Handling, 15.6.1 The terminate() Function\n");
        fprintf(logfp,
	  "The previous function given to set_terminate() is returned\n");

	set_terminate (&eh_term0);

	ret = set_terminate (&eh_term1);
	if (ret != &eh_term0)
	    failure ("&eh_term0() not returned.");
	else
	{
	    ret = set_terminate (&eh_term2);
	    if (ret != &eh_term1)
	        failure ("&eh_term1() not returned.");
	    else
	    {
	        ret = set_terminate (&eh_term3);
	        if (ret != &eh_term2)
	            failure ("&eh_term2() not returned.");
	    }
	}

        blexit();
/*--------------------------------------------------------------------*/
	anyfail(1);      
        return 0;
}


#include "scaffold.cxx"
