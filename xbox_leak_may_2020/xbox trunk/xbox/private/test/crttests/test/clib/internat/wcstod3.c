
/***************************************************************************

           Copyright (c) 1993, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: wcstod in a non-C locale.  
            Test that wcstod handles characters > 127.

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
    03-Nov-93   patlo    Created by copying wcstod1.c
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/

#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <math.h>
#include <float.h>
#include "test.h"


static char test[] = "wcstod2";
static int perfect = 0;



int wcstod3Entry(){
    double d_result;

    startest();

    if ( setlocale ( LC_ALL, "French") == NULL)
    { 
        fail (10);
        finish();
    }

    d_result = wcstod ( L"3,5§", NULL);

    finish();
}



void compare (double f1, double f2, int testno)
{
    if (fabs(f1-f2)>=0.01) 
    {
        DbgPrint("Was:       \"%lf\"\nShould be: \"%lf\"\n", f1, f2);
        fail( testno );
    }
}
