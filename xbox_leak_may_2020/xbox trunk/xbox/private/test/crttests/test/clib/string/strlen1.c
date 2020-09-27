
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strlen1.c
                     strlen1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strlen() function.
OS Version:          REAL/PROT/ALL
CPU:                 8086
Dependencies:        NONE
Environment Vars:    NONE
Verification Method: COMPILE/EXECUTE/COMPARE
Notes:               Must include STRING.H
                     Tests described in the testplan STRING.DOC
Products:            C800
Test Type:       DEPTH

Revision History:
 Date              emailname      description
---------------------------------------------------------------------------
 [ 0] 21-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char string0[20]    = "\0";
static char string1[35]    = "1\0";
static char string2[40]    = "12\0";
static char string33[75]   = "123456789012345678901234567890123\0";
static char string50[95]   = "12345678901234567890123456789012345678901234567890\0";
static char string101[195] = {
                       "123456789012345678901234567890123456789012345678901"
                       "12345678901234567890123456789012345678901234567890\0"
                      };



int strlen1Entry(){
    size_t length=999;



    startest();



/*  Scenario 1: Verify that the function works correctly when string
                is NULL.
*/
    length = strlen( string0 );
    checke( length, 0 );




/*  Scenario 2: Verify that the function works correctly when string
                contains only 1 character.
*/
    length = strlen( string1 );
    checke( length, 1 );




/*  Scenario 3: Verify that the function works correctly when string
                contains only 2 characters.
*/
    length = strlen( string2 );
    checke( length, 2 );




/*  Scenario 4: Verify that the function works correctly when string
                contains 33 characters.
*/
    length = strlen( string33 );
    checke( length, 33 );




/*  Scenario 5: Verify that the function works correctly when string
                contains 50 characters.
*/
    length = strlen( string50 );
    checke( length, 50 );




/*  Scenario 6: Verify that the function works correctly when string
                contains 101 characters.
*/
    length = strlen( string101 );
    checke( length, 101 );




    finish();
}
