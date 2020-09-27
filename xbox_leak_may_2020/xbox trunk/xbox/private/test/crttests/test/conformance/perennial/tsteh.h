/* File: tsteh.h    Version: 2.12    Date: 09/07/92
 * CVS-C++, C++ Validation Suite
 * Section: include
 * Copyright 1991-92, PERENNIAL, All Rights Reserved
 */
/*======================================================================
        =================== TESTPLAN SEGMENT ===================

>KEYS:	X3J16/91-0059, May 1991: ANSI C++ Working Paper,
	15 Exception Handling.
        15.6 Special Functions.
        15.6.1 The terminate() Function.
        15.6.2 The unexpected() Function.

>REL:   EH Phase I and later.

>WHAT:  Provide a standard trapping mechanism for wayward
	exceptions.  The inclusion of this file is not
	necessary for normal functioning of the testcases, 
	but may assist debugging.
	
	No assertions here.  
	This is an include file.

>HOW:   Write messages to the report file if exceptions
	are detected herein.
	
	Include this file from most EH testcases,
	and call eh_traps().
	
>NOTE:	None
======================================================================*/

typedef void (*PFV)();
PFV set_terminate(PFV);
PFV set_unexpected(PFV);

static void eh_unexpected()
{
	failure("tsteh.h: eh_unexpected(): Missing exception-specification ?");
	blexit();
	anyfail(block_number+1);
}

static void eh_terminate()
{
	failure("tsteh.h: eh_terminate(): EH termination trap");
	blexit();
	anyfail(block_number+1);
}

static void eh_traps()
{
	(void) set_terminate (&eh_terminate);
	(void) set_unexpected (&eh_unexpected);
}
