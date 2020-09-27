
/***************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strchr2.c
                     strchr2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strchr() function.
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
 [ 0] 06-May-92    edv            created
 [ 1] 28-Jan-98    a-hemalk       fixed 64bit warning (C4311) - fixed

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

static unsigned char string[20] = "abcdefghijklmnopqrst";


int strchr2Entry(){
    int c;
    unsigned char *pstr;

    startest();


/*  Scenario 5: Verify that c can be any value from 0 to UCHAR_MAX,
                and will be recognized as the character to be found.
*/

    for (c = 0; c < 'a'; c++) {
        string[9] = c;
        pstr = strchr( string, c );
        checknNULL(pstr);
    }


/*  Change string in string because first occurence will be found.
*/

    strcpy( string, "ABCDEFGHIJKLMNOPQRST" );

    for (c = 'a'; c <= UCHAR_MAX; c++) {
        string[9] = c;
        pstr = strchr( string, c );
        checknNULL(pstr);
    }



    finish();
}
