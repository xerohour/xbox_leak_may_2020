
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strset2.c
                     strset2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strset() function.
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
 [ 0] 13-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


#define DELTA       100
#define ALMOST_MAX (SHRT_MAX - DELTA)

/*  Scenario 5: Verify that string can be almost as large as SHRT_MAX.
*/

static unsigned char string1[ALMOST_MAX];

static unsigned char string2[11] = "0123456789\0";



int strset2Entry(){
    int c, i;
    unsigned char *pstr;

    startest();


    c = ' ';
    memset( string1, c, ALMOST_MAX);
    string1[ALMOST_MAX - 1] = '\0';

    c = 'X';
    pstr = _strset( string1, c );

    checke( pstr, string1 );

    for (i = 0; i < ALMOST_MAX - 1; i++)
        checke( pstr[i], c );

    checke( pstr[i], '\0' );





/*  Scenario 6: Verify that c can be any value from 1 to UCHAR_MAX.
*/

    for (c = 1; c < UCHAR_MAX; c++)
    {
        pstr = _strset( string2, c );

        for (i = 0; i < (sizeof( string2 ) - 1); i++)
            checke( pstr[i], c );

        checke( pstr[i], '\0' );

    }




    finish();
}
