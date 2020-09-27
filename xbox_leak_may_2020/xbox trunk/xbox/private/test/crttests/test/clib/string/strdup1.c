
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strdup1.c
                     strdup1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strdup() function.
OS Version           REAL/PROT/ALL
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
 [ 0] 28-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;



#define LEN1  11
#define LEN2   1
#define LEN3   2
#define LEN4   3

static char str1[11]  = "0123456789\0";
static char str2[1]   = "\0";
static char str3[2]   = "2\0";
static char str4[3]   = "23\0";



int strdup1Entry(){
    int i;
    char *pstr;


    startest();


/*  Scenario 1: Verify that the function returns a pointer to the
                duplicated string.
*/

    pstr = _strdup( str1 );

    for (i = 0; i < LEN1; i++)
        checke( pstr[i], str1[i] );




/*  Scenario 2: Verify that the function works correctly when string
                is NULL.
*/

    pstr = _strdup( str2 );

    checke( pstr[0], '\0' );



    pstr = _strdup("");

    checke( pstr[0], '\0' );




/*  Scenario 3: Verify that the function works correctly when string
                only holds one character.
*/

    pstr = _strdup( str3 );

    for (i = 0; i < LEN3; i++)
        checke( pstr[i], str3[i] );




/*  Scenario 4: Verify that the function works correctly when string
                only holds two characters.
*/

    pstr = _strdup( str4 );

    for (i = 0; i < LEN4; i++)
        checke( pstr[i], str4[i] );





    finish();
}
