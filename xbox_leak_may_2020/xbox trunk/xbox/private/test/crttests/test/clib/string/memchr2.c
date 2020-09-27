
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memchr2.c
                     memchr2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the memchr() function.
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
 [ 0]  1-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

static unsigned char buf[20] = "abcdefghijklmnopqrst";


int memchr2Entry(){
    int c;
    size_t count;
    unsigned char *pbuf;

    startest();


/*  Scenario 5: Verify that c can be any value from 0 to UCHAR_MAX,
                and will be recognized as the character to be found.
*/

    count = sizeof( buf );

    for (c = 0; c < 'a'; c++) {
        buf[9] = c;
        pbuf = memchr( buf, c, count );
        checke( pbuf, buf + 9 );
    }


/*  Change string in buf because first occurence will be found.
*/

    strcpy( buf, "ABCDEFGHIJKLMNOPQRST" );

    for (c = 'a'; c <= UCHAR_MAX; c++) {
        buf[9] = c;
        pbuf = memchr( buf, c, count );
        checke( pbuf, buf + 9 );
    }



    finish();
}
