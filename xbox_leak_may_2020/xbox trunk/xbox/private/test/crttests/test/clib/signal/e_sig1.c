
/*****************************************************************************
 *									     *
 * Test Name: e_sig1.c    Date: 1/29/91   Author: Microsoft                  *
 *===========================================================================*
 *									     *
 * Suite: signal                                                             *
 *===========================================================================*
 *									     *
 * Tested Op/Func: signal						     *
 *---------------------------------------------------------------------------*
 * PURPOSE: Tests that signal returns SIG_ERR when given an invalid value.   *
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

static int  find_sig_entry(int sigid);

static int T(perfect) = PDV(0);

static char test[] = "e_sig1";

static void __cdecl myhandler(int sigid);

int e_sig1Entry(void){
	int i, counter = 0;

	startest();

	/* Try some positive numbers. */

	for ( i = 0; i < 100; i++ ){

	    /* skip if valid. */

	    if ( find_sig_entry(i) >= 0 )
		continue;

	    /* try invalid value. */

	    if ( signal(i, myhandler) != SIG_ERR )
		fail(i);
	}

	/* Try some negative numbers. */

	for ( i = -100; i < 0; i++ ){

	    /* skip if valid. */

	    if ( find_sig_entry(i) >= 0 )
		continue;

	    /* try invalid value. */

	    if ( signal(i, myhandler) != SIG_ERR )
		fail(200+i);
	}
	
	/* Try very big and very small. */

	if ( signal(0x7fffffff, myhandler) != SIG_ERR )
	    fail(300);

	if ( signal(0x80000000, myhandler) != SIG_ERR )
	    fail(301);
	
	finish();
}

static int find_sig_entry(int sigid)
{
	int i;

	for ( i = 0; i < NUM_SIG_TYPES; i++ )
	    if ( sigtypes[i] == sigid )
		return(i);

	return(-1);
}

static void __cdecl myhandler(int sigid)
{
}
