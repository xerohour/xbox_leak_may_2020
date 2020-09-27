
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strcat1.c
                     strcat1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strcat() function.
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
 [ 0] 05-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char nullstr1[20] = "\0";
static char nullstr2[20] = "\0";
static char nullstr3[20] = "\0";

static char string1[20] = "String 1";
static char string2[20] = "String 2";

static char baseln1[] = "String 1String 2";
#define length1 16

static char baseln2[] = "String 2";
#define length2 8



int strcat1Entry(){
    int i;
    char *pstr;


    startest();



/*  Scenario 1: Verify that a pointer to string1 is returned.
*/
    pstr = strcat( string1, string2 );
    checke( pstr, string1 );

    for (i = 0; i < length1; i++)
        checke( pstr[i], baseln1[i] );





/*  Scenario 2: Verify that the function works correctly when
                string1 is a null string.
*/
    pstr = strcat( nullstr1, string2 );

    for (i = 0; i < length2; i++)
        checke( pstr[i], baseln2[i] );





/*  Scenario 3: Verify that the function works correctly when
                string2 is a null string.
*/
    pstr = strcat( string2, nullstr2 );

    for (i = 0; i < length2; i++)
        checke( pstr[i], baseln2[i] );





/*  Scenario 4: Verify that the function works correctly when
                both strings are null strings.
*/
    pstr = strcat( nullstr2, nullstr3 );

    checke( pstr[0], '\0' );





/*  Scenario 5: Verify that the function works correctly when
                string2 is only one byte long.
*/
    pstr = strcat( nullstr2, "1" );

    checke( pstr[0], '1' );
    checke( pstr[1], '\0' );





    finish();
}
