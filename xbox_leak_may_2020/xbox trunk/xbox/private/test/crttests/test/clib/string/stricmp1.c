
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl stricmp1.c
                     stricmp1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _stricmp() function.
OS Version:          REAL/PROT/ALL
CPU:                 8086
Dependencies:        NONE
Environment Vars:    NONE
Verification Method: COMPILE/EXECUTE/COMPARE
Notes:               Must include STRING.H
                     Tests decribed in the testplan STRING.DOC
Products:            C800
Test Type:       DEPTH

Revision History:
 Date              emailname      description
---------------------------------------------------------------------------
 [ 0] 11-Apr-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char strval1A[11] = "AaBbCcDdE1\0";
static char strval1B[11] = "AaBbCcDdEe\0";

static char strval2A[53] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz\0";
static char strval2B[53] = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ\0";


int stricmp1Entry(){
    int i, outcome;
    char string1[52], string2[52];

    startest();


/*  Scenario 1: Verify that the function returns a negative value
                when string1 < string2.
*/
    for (i = 0; i < (sizeof( strval1A ) - 1); i++ )
    {
        string1[i] = strval1A[i];
        string2[i] = strval1B[i];
    }

    string1[i] = '\0';
    string2[i] = '\0';

    outcome = 999;
    outcome = _stricmp( string1, string2 );
    checke( (outcome < 0), 1 );




/*  Scenario 2: Verify that the function returns a positive value
                when string1 > string2.
*/
    strval1A[sizeof( strval1A ) - 2] = 'e';
    for (i = 0; i < (sizeof( strval1A ) - 1); i++ )
    {
        string1[i] = strval1A[i];
        string2[i] = strval1B[i];
    }

    string1[i] = '\0';
    string2[i] = '\0';


    outcome = -999;
    outcome = _stricmp( string1, string2 );
    checke( (outcome == 0), 1 );



/*  Scenario 3: Verify that the function returns zero when string1 = string2.
*/
    strval1A[sizeof( strval1A ) - 2] = 'f';
    for (i = 0; i < (sizeof( strval1A ) - 1); i++ )
    {
        string1[i] = strval1A[i];
        string2[i] = strval1B[i];
    }

    string1[i] = '\0';
    string2[i] = '\0';


    outcome = 999;
    outcome = _stricmp( string1, string2 );
    checke( (outcome > 0), 1 );



/*  Scenario 4: Verify that the function is, in fact, case sensitive
                by making the first letter of each string differ only
                in the case (ie. 'abcd...' vs. 'Abcd...').
*/
    for (i = 0; i < (sizeof( strval2A ) - 2); i++ )
    {
        string1[i] = strval2A[i];
        string2[i] = strval2B[i];
    }

    string1[i] = '\0';
    string2[i] = '\0';


    outcome = 999;
    outcome = _stricmp( string1, string2 );
    checke( (outcome == 0), 1 );



/*  Scenario 5: Verify that the function is, in fact, case sensitive
                by making the last letter of each string differ only
                in the case (ie. '...abcd' vs. '...abcD').
*/
    for (i = 0; i < (sizeof( strval2A ) - 2); i++ )
    {
        string1[i] = strval2A[i];
        string2[i] = strval2B[i];
    }

    string1[i] = '\0';
    string2[i] = '\0';


    outcome = 999;
    outcome = _stricmp( string1, string2 );
    checke( (outcome == 0), 1 );


    finish();
}
