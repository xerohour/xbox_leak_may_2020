
/***************************************************************************

           Copyright (c) 1993, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: strtoul in a non-C locale.  
            Test that strtoul handles characters in a non-c locale
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
    03-Nov-93   patlo    Created by copying wcstoul2.c
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/

#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <test.h>


static char test[] = "strtoul2";
static int perfect = 0;

static void compare (unsigned long, unsigned long, int testno);

int strtoul2Entry(){
    unsigned long l_result;

    startest();

    if ( setlocale ( LC_NUMERIC, "French") == NULL)
    { 
        fail (10);
        finish();
    }

    l_result = strtoul ( "3,5§", NULL, 10);

    compare ( l_result, 3, 1 );

    finish();
}



static void compare (unsigned long f1, unsigned long f2, int testno)
{
    if (f1 != f2) 
    {
        printf("Was:       \"%lu\"\nShould be: \"%lu\"\n", f1, f2);
        fail( testno );
    }
}
