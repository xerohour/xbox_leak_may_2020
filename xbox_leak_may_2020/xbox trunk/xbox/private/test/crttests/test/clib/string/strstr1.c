
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strstr1.c
                     strstr1
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
 [ 0] 20-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char string1a[50] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxY\0";
static char string1b[10] = "\0";


static char string2a[4]  = "eFf\0";
static char string2b[3]  = "tT\0";
static char string2c[10] = "\0";


int strstr1Entry(){
    char *pstr1;



    startest();




/*  Scenario 1: Verify that the function returns a pointer to the first
                occurence of string2 in string1.
*/

    pstr1 = strstr( string1a, string2a );

    checke( pstr1, (string1a + 10 - 1) );





/*  Scenario 2: Verify that when string2 is not found in string1, the
                function returns NULL.
*/
    pstr1 = strstr( string1a, string2b );
    checkNULL( pstr1 );





/*  Scenario 3: Verify that the function works correctly when string1
                is NULL.
*/
    pstr1 = strstr( string1b, string2b );
    checkNULL( pstr1 );





/*  Scenario 4: Verify that the function works correctly when string2
                is NULL.  This should return string1 because string2
                has length zero.
*/
    pstr1 = strstr( string1a, string2c );
    checke( pstr1, string1a );





/*  Scenario 5: Verify that the function works correctly when both string1
                and string2 are NULL.  This should return string1 because
                string2 has length zero.
*/
    pstr1 = strstr( string1b, string2c );
    checke( pstr1, string1b );





    finish();
}
