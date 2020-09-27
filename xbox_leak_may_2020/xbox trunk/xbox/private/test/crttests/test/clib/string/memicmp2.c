
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memicmp2.c
                     memicmp2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _memicmp() function.
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
 [ 0] 01-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char buf1[15] = "ABCDEFGHIJKLMNO";
static char buf2[15] = "ABCDEFGHIJKLMNO";


int memicmp2Entry(){
    int outcome;
    size_t count;


    startest();


    count = sizeof( buf1 );


/*  Scenario 4: Verify that the function is, in fact, case insensitive
                by making the first letter in each buffer differ in the
                case (ie. 'abcd...' vs. 'Abcd...').
*/
    buf1[0] = 'a';
    outcome = 999;
    outcome = (int)_memicmp( buf1, buf2, count);
    checke( (outcome == 0), 1 );
    buf1[0] = 'A';



/*  Scenario 5: Verify that the function is, in fact, case insensitive
                by making the last letter in each buffer differ in the
                case (ie. 'abcD...' vs. 'abcd...').
*/
    buf1[count - 1] = 'o';
    outcome = -999;
    outcome = (int)_memicmp( buf1, buf2, count);
    checke( (outcome == 0), 1 );


    finish();
}
