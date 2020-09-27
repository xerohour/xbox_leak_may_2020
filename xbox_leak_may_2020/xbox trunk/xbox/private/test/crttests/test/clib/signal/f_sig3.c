
/*****************************************************************************
 *									     *
 * Test Name: sig3.c    Date: 1/29/91   Author: Microsoft                    *
 *===========================================================================*
 *									     *
 * Suite: signal                                                             *
 *===========================================================================*
 *									     *
 * Tested Op/Func: signal						     *
 *---------------------------------------------------------------------------*
 * PURPOSE: Tests that signal will properly return the address of the        *
 *	    previously installed handler.				     *
 *									     *
 * METHOD:                                                                   *
 *                                                                           *
 * MEMORY MODEL(S):                                                          *
 *      32-bit								     *
 * COMPILE/LINK OPTIONS:                                                     *
 *      No restrictions.                                                     *
 * OPERATING SYSTEM(S):                                                      *
 *      NT.  								     *
 * HARDWARE:                                                                 *	
 *      No restrictions                                                      *
 * SOFTWARE:                                                                 *
 *      No restrictions                                                      *
 * CONFIG.SYS:                                                               *
 *      Should contain what is required for compiler/linker                  *
 * ENVIRONMENT VARIABLES:                                                    *
 *      Should contain what is required for compiler/linker                  *
 *---------------------------------------------------------------------------*
 * TEST-RESULT OUTPUT: The outputs for this test shall be as follows:        *
 *      Normal: ***** PASSED *****                                           *
 *      Error:  ----- FAILED -----                                           *
 *---------------------------------------------------------------------------*
 * MODIFICATION HISTORY:                                                     *
 *                                                                           *
 *  Date       Initials Version PTR #   Description                          *
 *  ---------  -------- ------- ------- -------------------------------------*
 *  1/29/91    SDK      1.00                                                 *
 *									     *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <test.h>

static int sigtypes[] = { SIGABRT, SIGBREAK, SIGFPE, SIGILL,
	           SIGINT,  SIGSEGV,  SIGTERM };

#define NUM_SIG_TYPES ( sizeof(sigtypes) / sizeof(int) )

static int handled[NUM_SIG_TYPES] = { 0 };

static void __cdecl myhandler1(int sigid);
static void __cdecl myhandler2(int sigid);

static int  find_sig_entry(int sigid);

static int perfect = 0;

static char test[] = "f_sig1";

int f_sig3Entry(void){
	int i, counter = 0;

	startest();

	/* Install the handler1 for each type of signal. */

	for ( i = 0; i < NUM_SIG_TYPES; i++ )
	    if ( signal(sigtypes[i], myhandler1) == SIG_ERR )
		fail(10+i);

	/* Install handler 2 - and be sure the returned value is handler1. */

	for ( i = 0; i < NUM_SIG_TYPES; i++ )
	    if ( signal(sigtypes[i], myhandler2) != myhandler1 )
		fail(20+i);

	finish();
}

static void __cdecl myhandler1(int sigid)
{
}

static void __cdecl myhandler2(int sigid)
{
}
