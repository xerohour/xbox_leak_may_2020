
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strpbrk1.c
                     strpbrk1
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


static char string1a[50] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxY\0";
static char string1b[30] = "\0";


static char string2a[15]  = "1234567890aA\0";
static char string2b[35]  = "yZz1234567890)(*&^%$#@!~\0";
static char string2c[15] = "\0";


int strpbrk1Entry(){
    char *pstr1;



    startest();




/*  Scenario 1: Verify that the function returns a pointer to the first
                occurence of a character of string2 in string1.
*/

    pstr1 = strpbrk( string1a, string2a );

    checke( pstr1, string1a );





/*  Scenario 2: Verify that the function returns NULL when the strings have
                no characters in common.
*/
    pstr1 = strpbrk( string1a, string2b );
    checkNULL( pstr1 );





/*  Scenario 3: Verify that the function works correctly when string1
                is NULL.
*/
    pstr1 = strpbrk( string1b, string2b );
    checkNULL( pstr1 );





/*  Scenario 4: Verify that the function works correctly when string2
                is NULL.
*/
    pstr1 = strpbrk( string1a, string2c );
    checkNULL( pstr1 );





/*  Scenario 5: Verify that the function works correctly when both string1
                and string2 are NULL.
*/
    pstr1 = strpbrk( string1b, string2c );
    checkNULL( pstr1 );





    finish();
}
