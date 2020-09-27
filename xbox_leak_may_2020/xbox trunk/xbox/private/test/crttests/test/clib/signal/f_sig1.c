
/*****************************************************************************
 *									     *
 * Test Name: sig1.c    Date: 1/29/91   Author: Microsoft                    *
 *===========================================================================*
 *									     *
 * Suite: signal                                                             *
 *===========================================================================*
 *									     *
 * Tested Op/Func: signal						     *
 *---------------------------------------------------------------------------*
 * PURPOSE: Basic sanity check of functionality of signal.  Will verify that *
 *	    a signal handler can be installed for each type of exception     *
 *	    supported, and that the handler will be called for each time     *
 *	    that signal is raised via the raise() function.  It will also    *
 *	    implicitly verify that execution resumes after the raise when    *
 *	    the handler returns.					     *
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

static void __cdecl myhandler(int sigid);
static int  find_sig_entry(int sigid);

static int perfect = 0;

static char test[] = "f_sig1";

int f_sig1Entry(void){
	int i, counter = 0;

	startest();

	/* Install the handler for each type of signal. */

	for ( i = 0; i < NUM_SIG_TYPES; i++ )
	    if ( signal(sigtypes[i], myhandler) == SIG_ERR )
		fail(10+i);

	/* Raise each type of signal. */

	for ( i = 0; i < NUM_SIG_TYPES; i++ ){
	    if ( raise(sigtypes[i]) != 0 )
		fail(20+i);
	    else
		counter++;
	}

	/* Verify recovery ocurred for each exception raised. */

	if ( counter != NUM_SIG_TYPES )
	    fail(2);

	finish();
}

static void __cdecl myhandler(int sigid)
{
	int entry;

	if ( (entry = find_sig_entry(sigid)) < 0 )
	    fail(1);

	handled[entry]++;
}

static int find_sig_entry(int sigid)
{
	int i;

	for ( i = 0; i < NUM_SIG_TYPES; i++ )
	    if ( sigtypes[i] == sigid )
		return(i);

	return(-1);
}
