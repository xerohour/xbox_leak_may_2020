
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strcspn2.c
                     strcspn2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strcspn() function.
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
 [ 0] 22-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

static char string1a[2]   = "1\0";
static char string1b[64]  = "A1B2C3D4E5F6G7H8I9J0\0";
static char string1c[2]   = "7\0";

static char string2a[27]  = "AaBbCc0987654321\0";
static char string2b[2]   = "J\0";
static char string2c[2]   = "7\0";

#define RESULT1   0
#define RESULT2  18
#define RESULT3   0


int strcspn2Entry(){
    int index;



    startest();



/*  Scenario 6: Verify that the function works correctly when string1 only
                holds 1 character.
*/
    index = (int)strcspn( string1a, string2a );
    checke( index, RESULT1 );


/*  Scenario 7: Verify that the function works correctly when string2 only
                holds 1 character.
*/
    index = (int)strcspn( string1b, string2b );
    checke( index, RESULT2 );


/*  Scenario 8: Verify that the function works correctly when both string1
                and string2 only hold 1 character.
*/
    index = (int)strcspn( string1c, string2c );
    checke( index, RESULT3 );



    finish();
}
