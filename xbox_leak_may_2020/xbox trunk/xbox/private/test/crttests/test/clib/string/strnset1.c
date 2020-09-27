
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strnset1.c
                     strnset1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strnset() function.
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
 [ 0] 14-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static unsigned char string1[21] = "AaBbCcDdEeFfGgHhIiJj\0";
static unsigned char string2[21] = "XXXXXXXXXXXXXXXXXXXX\0";

static unsigned char baseline1[21] = "%%%%%*DdEeFfGgHhIiJj\0";
static unsigned char baseline2[21] = "????????????????????\0";
static unsigned char baseline3[21] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
static unsigned char baseline4[21] = "  XXXXXXXXXXXXXXXXXX\0";


int strnset1Entry(){
    int c, i;
    size_t count;
    unsigned char *pstr;

    startest();


/*  Scenario 1: Verify that the function returns a pointer to string.
*/

    count = 5;
    string1[count] = '*';

    pstr = _strnset( string1, '%', count );

    checke( pstr, string1 );

    for (i = 0; i < sizeof( string1 ) - 1; i++)
        checke( string1[i], baseline1[i] );





/*  Scenario 2: Verify that if count is greater than the length of string,
                the length of string is used in place of count.
*/

    count = sizeof( string1 ) * 2;

    pstr = _strnset( string1, '?', count );

    checke( pstr, string1 );

    for (i = 0; i < sizeof( string1 ) - 1; i++)
        checke( string1[i], baseline2[i] );





/*  Scenario 3: Verify that c can be any value from 0 to UCHAR_MAX.
*/

    c = 0;
    count = 20;

    pstr = _strnset( string1, c, count );

    for (i = 0; i < sizeof( string1 ) - 1; i++)
        checke( pstr[i], baseline3[i] );



    count = 2;

    for (c = 1; c < UCHAR_MAX; c++)
    {
        baseline4[0] = c;
        baseline4[1] = c;

        pstr = _strnset( string2, c, count );

        for (i = 0; i < sizeof( string2 ) - 1; i++)
            checke( pstr[i], baseline4[i] );

    }




    finish();
}
