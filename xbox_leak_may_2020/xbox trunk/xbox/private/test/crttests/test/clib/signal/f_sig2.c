
/*****************************************************************************
 *									     *
 * Test Name: sig2.c    Date: 1/29/91   Author: Microsoft                    *
 *===========================================================================*
 *									     *
 * Suite: signal                                                             *
 *===========================================================================*
 *									     *
 * Tested Op/Func: signal						     *
 *---------------------------------------------------------------------------*
 * PURPOSE: This tests that each signal that can be raised by the operating  *
 *	    system, will result in a call to installed signal handler.       *
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
 *  6/23/92    xiangjun 2.00            add longjmp and setjmp to make       *
 *                                      for SIGSEGV work                     *
 *									     *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <test.h>
#include <setjmp.h>

static void __cdecl myhandler(int sigid);
static void __cdecl myhandler_SIGSEGV(int sigid);

static int perfect = 0;

static char test[] = "f_sig2";

static double T(z)  = PDV(0.0);
static char T(*ptr) = PDV(0);
static char valid_address[128];
static int sig_err;
static jmp_buf mark;

int f_sig2Entry(void){
	int counter = 0;
	double x, y = 1.0;

        int jmpret;

	startest();

	/* Install the handler for each type. */

	if ( signal(SIGFPE, myhandler) == SIG_ERR )
	    fail(10);

	if ( signal(SIGILL, myhandler) == SIG_ERR )
	    fail(11);

	if ( signal(SIGSEGV, myhandler_SIGSEGV) == SIG_ERR )
	    fail(12);

	/* Cause a floating-point exception to ocurr. */

	x = y / z;

	counter++;

	/* Raise a SIGILL.  It would be much better if we would at least
	   call the OS API to raise an illegal instruction exception.
	   In NT, the API is RaiseException, I have no idea of what to
	   pass it though. */

	raise(SIGILL);

	counter++;

	/* Cause a segmentation violation. */

        jmpret = setjmp( mark );
        if( jmpret == 0 ) {

    	   *V(ptr) = '\0';
           printf( "Error: program should have not reached here\n" );
           }
        else {
           if( sig_err == SIGSEGV )
           counter++;
           else
              fail(15);
           } 

	/* Be sure signal was raised, and recovered 3 times. */

	if ( counter != 3 )
	    fail(20);

	finish();
}

static void __cdecl myhandler(int sigid)
{
	switch(sigid){

	    /* Clear up the divide by zero problem. */

	    case SIGFPE:

		V(z) = 0.5;

	    break;

	    /* Clear up the NULL pointer problem. */

	    case SIGSEGV:

		V(ptr) = valid_address;

	    break;

	    /* Nothing to clear up here, don't have a method of causing
	       the OS to raise this. */

	    case SIGILL:

	    break;

	    default:

		fail(1);
	}
}

static void __cdecl myhandler_SIGSEGV(int sigid) {
    sig_err = sigid;
    longjmp( mark, -1 );
} 
