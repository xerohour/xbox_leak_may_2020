
/***************************************************************************

           Copyright (c) 1993, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: wcstol in a non-C locale.  
            Test that wcstol handles characters in a non-c locale
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
    03-Nov-93   patlo    Created by copying wcstod1.c
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/

#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <float.h>
#include "test.h"


static char test[] = "wcstol2";
static int perfect = 0;

static void compare (long, long, int testno);

int wcstol3Entry(){
    long l_result;

    startest();

    if ( setlocale ( LC_NUMERIC, "French") == NULL)
    { 
        fail (10);
        finish();
    }

    l_result = wcstol ( L"3,5§", NULL, 10);

    compare ( l_result, 3, 1 );

    finish();
}



void compare (long f1, long f2, int testno)
{
    if (f1 != f2) 
    {
        DbgPrint("Was:       \"%l\"\nShould be: \"%l\"\n", f1, f2);
        fail( testno );
    }
}
