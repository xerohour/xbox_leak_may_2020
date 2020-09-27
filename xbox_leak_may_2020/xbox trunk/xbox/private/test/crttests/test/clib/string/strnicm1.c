
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strnicm1.c
                     strnicm1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strnicmp() function.
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
 [ 0] 12-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char string1[51] = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyY\0";
static char string2[51] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYy\0";

int strnicm1Entry(){
    int outcome;
    size_t count;


    startest();


    count = sizeof( string1 );



/*  Scenario 1: Verify that the function returns a negative value
                when string1 < string2.
*/
    string1[49] = 'X';
    outcome = 0;
    outcome = _strnicmp( string1, string2, count);
    checke( (outcome < 0), 1 );
    string1[49] = 'Y';




/*  Scenario 2: Verify that the function returns a positive value
                when string1 > string2.
*/
    string1[49] = '{';
    outcome = -999;
    outcome = _strnicmp( string1, string2, count);
    checke( (outcome > 0), 1 );
    string1[49] = 'Y';




/*  Scenario 3: Verify that the function returns zero
                when string1 = string2.
*/
    outcome = 999;
    outcome = _strnicmp( string1, string2, count);
    checke( (outcome == 0), 1 );


    finish();
}
