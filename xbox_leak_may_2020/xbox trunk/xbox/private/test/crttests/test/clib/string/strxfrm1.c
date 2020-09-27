
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strxfrm1.c
                     strxfrm1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strxfrm() function.
OS Version:          REAL/PROT/ALL
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
 [ 0] 22-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <locale.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char string1a[52] = "AaBbCc\0";
static char string2a[39] = "0123456789012345678901234567890123\0";

static char string1b[69] = "\0";
static char string2b[75] = "Copy 30 characters to string1b\0";

static char string1c[69] = "\0";
static char string2c[75] = "Copy only 35 characters to string1c\0";

static char string1d[20] = "0123456789\0";
static char string2d[51] = "This length is 17\0";

static char string1e[69] = "\0";
static char string2e[75] = "abcdefg\0";

static char string1f[23] = "1234567890\0";
static char string2f[32] = "\0";

static char string1g[47] = "\0";
static char string2g[64] = "\0";

#define LEN1 34
#define LEN2 30
#define LEN3 35
#define LEN4 17
#define LEN5  7
#define LEN6  0
#define LEN7  0

int strxfrm1Entry(){
    int i, count;
    size_t retvalue;


    startest();



    setlocale( LC_COLLATE, "C" );




/*  Scenario 1: Verify that the function returns the length of the
                transformed string (should = len( string2 )).
*/

    retvalue = 0;
    count = LEN1 + 1;
    retvalue = strxfrm( string1a, string2a, count );

    checke( retvalue, LEN1 );




/*  Scenario 2: Verify that count characters are copied from string2
                to string1.
*/

    retvalue = 0;
    count = LEN2 + 1;

    retvalue = strxfrm( string1b, string2b, count );

    checke( retvalue, LEN2 );

    for (i = 0; i < count; i++ )
        checke( string1b[i], string2b[i] );

    checke( string1b[i], '\0' );




/*  Scenario 3: Verify that when count is greater than the length of string2,
                only length(string2) + 1 characters are copied from string2
                to string1.
*/

    retvalue = 0;
    count = 2 * LEN3;

    retvalue = strxfrm( string1c, string2c, count );

    checke( retvalue, LEN3 );

    for (i = 0; i < LEN3; i++ )
        checke( string1c[i], string2c[i] );

    checke( string1c[i], '\0' );




/*  Scenario 4: Verify that when count equals zero the function returns
                the length of string2 and string1 is unaltered.

*/

    retvalue = 0;
    count = 0;

    retvalue = strxfrm( string1d, string2d, count );

    checke( retvalue, LEN4 );

    for (i = 0; i < 10; i++ )
        checke( string1d[i],  i + '0' );

    checke( string1d[i], '\0' );




/*  Scenario 5: Verify that the function works correctly when string1 is
                NULL.

*/

    retvalue = 0;
    count = LEN5 + 1;

    retvalue = strxfrm( string1e, string2e, count );

    checke( retvalue, LEN5 );

    for (i = 0; i <= LEN5; i++ )
        checke( string1e[i],  string2e[i] );




/*  Scenario 6: Verify that the function works correctly when string2 is
                NULL.

*/

    retvalue = 0;
    count = LEN6 + 1;

    retvalue = strxfrm( string1f, string2f, count );

    checke( retvalue, LEN6 );

    checke( string1f[0], '\0' );




/*  Scenario 7: Verify that the function works correctly when both string1
                and string2 are NULL.

*/

    retvalue = 0;
    count = LEN7 + 1;

    retvalue = strxfrm( string1g, string2g, count );

    checke( retvalue, LEN7 );

    checke( string1g[0], '\0' );




    finish();
}
