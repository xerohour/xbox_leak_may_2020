
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strcat2.c
                     strcat2
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
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


#define DELTA 2000
#define ALMOST_MAX  (SHRT_MAX - DELTA)

static unsigned char max_str1[ALMOST_MAX] = "\0";
static unsigned char max_str2[ALMOST_MAX];

static unsigned char string1[10] = "\0";
static unsigned char string2[10] = "A1B2C3D4 ";



int strcat2Entry(){
    int c, i;
    unsigned char *pstr;

    startest();




/*  Scenario 6: Verify that string2 can contain any value from
                0 to UCHAR_MAX.
*/

    for (c = 0; c <= UCHAR_MAX; c++)
    {
        string2[8] = c;
        pstr = strcat( string1, string2 );

        for (i = 0; i < 10; i++)
            checke( pstr[i], string2[i]);

        string1[0] = '\0';
    }





/*  Scenario 7: Verify that string1 and string2 can be almost as
        large as SHRT_MAX.
*/

    memset( max_str2, '*', sizeof( max_str2 ) );
    max_str2[ALMOST_MAX - 2] = 'X';
    max_str2[ALMOST_MAX - 1] = '\0';

    pstr = strcat( max_str1, max_str2);

    checke( pstr[0], max_str2[0] );
    checke( pstr[ALMOST_MAX / 2], max_str2[ALMOST_MAX / 2] );
    checke( pstr[ALMOST_MAX - 2], 'X' );
    checke( pstr[ALMOST_MAX - 1], '\0' );

    finish();
}
