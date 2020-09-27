
/***************************************************************************

           Copyright (c) 1993, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: wcstod in a non-C locale.  
            Test that wcstod handles characters in a non-c locale
            by setting LC_NUMERIC and checking the radix character.

OS Version:
CPU:
Dependencies:       NONE
Environment Vars:   NONE
Verification Method:    EXECUTE
Priority:       1/2/3/4
Notes:          NONE
Products:


Revision History:

    Date    emailname   description
----------------------------------------------------------------------------
    02-Nov-93   patlo    Created by copying wcstod.c
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/

#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <math.h>
#include <float.h>
#include "test.h"


static char test[] = "wcstod1";
static int perfect = 0;

static void compare (double, double, int testno);


int wcstod2Entry(){
    double d_result;

    startest();

    if ( setlocale ( LC_NUMERIC, "French") == NULL)
    { 
        fail (10);
        finish();
    }

    d_result = wcstod ( L"3,5", NULL);
/*  
    Note, if wcstod doesn't respond to a non-c locale, it's likely that
    d_result will contain 3 instead of 3.5
*/
    compare ( d_result, 3.5, 1 );

    finish();
}



void compare (double f1, double f2, int testno)
{
    if (fabs(f1-f2)>=0.01) 
    {
        printf("Was:       \"%lf\"\nShould be: \"%lf\"\n", f1, f2);
        fail( testno );
    }
}
