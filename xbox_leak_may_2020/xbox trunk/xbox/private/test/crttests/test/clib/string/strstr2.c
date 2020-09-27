
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strstr2.c
                     strstr2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strstr() function.
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

static char string1a[50] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxY\0";
static char string1b[40] = "A1B2C3D4E5F6G7H8I9J0\0";


static char string2a[7]  = "AaBbCc\0";
static char string2b[11] = "tUuVvWwXxY\0";
static char string2c[25] = "A1B2C3D4E5F6G7H8I9J0\0";
static char string2d[30] = "1B2C3D4E5F6G7H8I9J0\0";


int strstr2Entry(){
    char *pstr1;


    startest();



/*  Scenario 6: Verify that the function works correctly when string2 occurs
                in the first byte of string1.
*/
    pstr1 = strstr( string1a, string2a );
    checke( pstr1, string1a );




/*  Scenario 7: Verify that the function works correctly when string2 occurs
                in the last byte of string1.
*/
    pstr1 = strstr( string1a, string2b );

    checke( pstr1, (string1a + 40 - 1) );



/*  Scenario 8: Verify that the function works correctly when both string1
                and string2 are equal.
*/
    pstr1 = strstr( string1b, string2c );

    checke( pstr1, string1b );



/*  Scenario 9: Verify that the function works correctly when string2 is
                a substring equal to string1 minus string1[0].
*/
    pstr1 = strstr( string1b, string2d );

    checke( pstr1, string1b + 1 );




    finish();
}
