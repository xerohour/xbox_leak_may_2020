
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strpbrk2.c
                     strpbrk2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strpbrk() function.
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

static char string1a[2]   = "1\0";
static char string1b[40]  = "A1B2C3D4E5F6G7H8I9J0\0";
static char string1c[2]   = "7\0";

static char string2a[27]  = "AaBbCc0987654321\0";
static char string2b[2]   = "J\0";
static char string2c[2]   = "7\0";



int strpbrk2Entry(){
    char *pstr1;



    startest();



/*  Scenario 6: Verify that the function works correctly when string1 only
                holds 1 character.
*/
    pstr1 = strpbrk( string1a, string2a );
    checke( pstr1, string1a );




/*  Scenario 7: Verify that the function works correctly when string2 only
                holds 1 character.
*/
    pstr1 = strpbrk( string1b, string2b );
    checke( pstr1, string1b + 19 - 1);




/*  Scenario 8: Verify that the function works correctly when both string1
                and string2 only hold 1 character.
*/
    pstr1 = strpbrk( string1c, string2c );
    checke( pstr1, string1c );




    finish();
}
