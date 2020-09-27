
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl /AL memchr3.c
                     memchr3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the memchr() function.
OS Version:          REAL/PROT/ALL
CPU:                 8086
Dependencies:        Must be compiled using LARGE model
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

#define TWO            2
#define TWENTY        20
#define HUNDRED      100
#define THOUSAND    1000
#define TEN_THOU   10000
#define FORTY_THOU 40000


/*  Scenario 6: Verify tat buf can be as large as USHRT_MAX
                when using LARGE model.
*/

static unsigned char buf[USHRT_MAX];


int memchr3Entry(){
    int c, d=0x01;
    size_t count;
    unsigned char *pbuf;

    startest();


/*  Scenario 7: Verify that count can be anywhere in the range of
        0 to USHRT_MAX when using the LARGE model.
*/

    c = '^';
    memset( buf, d, sizeof( buf ) );

    count = 0;
    pbuf = memchr( buf, c, count );
    checkNULL( pbuf );


    for (count = TWO; count < TWENTY; count++)
    {
        buf[count - TWO] = c;
        pbuf = memchr( buf, c, count );
        checke( pbuf, buf + count - TWO );
        buf[count - TWO] = d;
    }



    for (count = HUNDRED; count < (HUNDRED + TWENTY); count++)
    {
        buf[count - TWO] = c;
        pbuf = memchr( buf, c, count );
        checke( pbuf, buf + count - TWO );
        buf[count - TWO] = d;
    }


    for (count = THOUSAND; count < (THOUSAND + TWENTY); count++)
    {
        buf[count - TWO] = c;
        pbuf = memchr( buf, c, count );
        checke( pbuf, buf + count - TWO );
        buf[count - TWO] = d;
    }


    for (count = TEN_THOU; count < (TEN_THOU + TWENTY); count++)
    {
        buf[count - TWO] = c;
        pbuf = memchr( buf, c, count );
        checke( pbuf, buf + count - TWO );
        buf[count - TWO] = d;
    }


    for (count = FORTY_THOU; count < (FORTY_THOU + TWENTY); count++)
    {
        buf[count - TWO] = c;
        pbuf = memchr( buf, c, count );
        checke( pbuf, buf + count - TWO );
        buf[count - TWO] = d;
    }


    for (count = (USHRT_MAX - TWENTY); count < USHRT_MAX; count++)
    {
        buf[count - TWO] = c;
        pbuf = memchr( buf, c, count );
        checke( pbuf, buf + count - TWO );
        buf[count - TWO] = d;
    }


    finish();
}
