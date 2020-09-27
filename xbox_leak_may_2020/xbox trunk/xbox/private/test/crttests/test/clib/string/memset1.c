
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memset1.c
                     memset1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the memset() function.
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


static unsigned char buf[20] = "AaBbCcDdEeFfGgHhIiJj";


int memset1Entry(){
    int c, i;
    size_t count;
    unsigned char *pbuf;

    startest();


/*  Scenario 1: Verify that the function returns a pointer to dest.
*/

    count = 5;
    buf[count] = '*';

    pbuf = memset( buf, '%', count );

    checke( pbuf, buf );

    for (i = 0; i < (int) count; i++)
        checke( buf[i], '%' );

    checke( buf[count], '*' );





/*  Scenario 2: Verify that c can be any value from 0 to UCHAR_MAX.
*/

    count = 10;

    for (c = 0; c < UCHAR_MAX; c++)
    {
        buf[count] = '*';

        pbuf = memset( buf, c, count );

        for (i = 0; i < (int) count; i++)
            checke( buf[i], c );

        checke( buf[count], '*' );

    }




    finish();
}
