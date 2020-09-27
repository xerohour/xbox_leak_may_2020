
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strnicm2.c
                     strnicm2
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


static char string1[16] = "ABCDEFGHIJKLMNO\0";
static char string2[16] = "ABCDEFGHIJKLMNO\0";


int strnicm2Entry(){
    int outcome;
    size_t count;


    startest();


    count = sizeof( string1 );


/*  Scenario 4: Verify that the function is, in fact, case insensitive
                by making the first letter in each buffer differ in the
                case (ie. 'abcd...' vs. 'Abcd...').
*/
    string1[0] = 'a';
    outcome = 999;
    outcome = _strnicmp( string1, string2, count);
    checke( (outcome == 0), 1 );
    string1[0] = 'A';



/*  Scenario 5: Verify that the function is, in fact, case insensitive
                by making the last letter in each buffer differ in the
                case (ie. 'abcD...' vs. 'abcd...').
*/
    string1[count - 2] = 'o';
    outcome = -999;
    outcome = _strnicmp( string1, string2, count);
    checke( (outcome == 0), 1 );


    finish();
}
