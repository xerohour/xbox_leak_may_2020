
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strncat1.c
                     strncat1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strncat() function.
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
 [ 0] 19-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

static char string1[20] = "String 1\0";
static char string2[20] = "String 2\0";

static char string3[20] = "String 3\0";
static char string4[20] = "String 4\0";

static char string5[20] = "String 5\0";
static char string6[20] = "String 6\0";

static char string7[20] = "String 7\0";
static char string8[20] = "String 8\0";

static char string9[20]  = "\0";
static char string10[20] = "String 10\0";

static char string11[20] = "String 11\0";
static char string12[20] = "\0";

static char string13[20] = "\0";
static char string14[20] = "\0";


static char baseln1[17] = "String 1String 2\0";
#define length1 17

static char baseln2[9] = "String 3\0";
#define length2 9

static char baseln3[10] = "String 5S\0";
#define length3 10

static char baseln4[17] = "String 7String 8\0";
#define length4 17

static char baseln5[10] = "String 10\0";
#define length5 10

static char baseln6[10] = "String 11\0";
#define length6 10

static char baseln7[10] = "\0";
#define length7 1

#define lengthX 10


int strncat1Entry(){
    int i;
    char *pstr;


    startest();



/*  Scenario 1: Verify that the function returns a pointer
                to the concatenated string.
*/

    pstr = strncat( string1, string2, length1 );
    checke( pstr, string1 );

    for (i = 0; i < length1; i++)
        checke( pstr[i], baseln1[i] );





/*  Scenario 2: Verify that the function works correctly when
                count is zero.
*/

    pstr = strncat( string3, string4, 0 );

    for (i = 0; i < length2; i++)
        checke( pstr[i], baseln2[i] );





/*  Scenario 3: Verify that the function works correctly when
                count is 1.
*/

    pstr = strncat( string5, string6, 1 );

    for (i = 0; i < length3; i++)
        checke( pstr[i], baseln3[i] );





/*  Scenario 4: Verify that the length of string2 is used in place of
                count when count is greater than the length of string2.
*/
    pstr = strncat( string7, string8, 100 );

    for (i = 0; i < length4; i++)
        checke( pstr[i], baseln4[i] );





/*  Scenario 5: Verify that the function works correctly when string1
                is NULL.
*/
    pstr = strncat( string9, string10, length5 );

    for (i = 0; i < length5; i++)
        checke( pstr[i], baseln5[i] );




/*  Scenario 6: Verify that the function works correctly when string2
                is NULL.
*/
    pstr = strncat( string11, string12, length6 );

    for (i = 0; i < length6; i++)
        checke( pstr[i], baseln6[i] );




/*  Scenario 7: Verify that the function works correctly when both
                string1 and string2 are NULL.
*/
    pstr = strncat( string13, string14, length7 );

    for (i = 0; i < length7; i++)
        checke( pstr[i], baseln7[i] );





    finish();
}
