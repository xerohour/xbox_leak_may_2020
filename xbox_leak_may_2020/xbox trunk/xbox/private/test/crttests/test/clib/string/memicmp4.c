
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memicmp4.c
                     memicmp4
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _memicmp() function.
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
 [ 0] 01-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


#define ONE             1
#define TWO             2
#define TWENTY         20
#define HUNDRED       100
#define THOUSAND     1000
#define TEN_THOU    10000
#define THIRTY_THOU 30000
#define DELTA        2000
#define ALMOST_MAX  (SHRT_MAX - DELTA)


/*  Scenario 8: Verify that buf1 and buf2 can be almost as large as
        SHRT_MAX when using SMALL model.
*/

static unsigned char buf1[ALMOST_MAX];
static unsigned char buf2[ALMOST_MAX];



int memicmp4Entry(){
    int c, d, outcome;
    size_t count;


    startest();


/*  Scenario 9: Verify that count can be anywhere in the range
        0 to aproximately SHRT_MAX when using SMALL model.
*/

    c = 'x';
    memset( buf1, c, sizeof( buf1 ) );
    memset( buf2, c, sizeof( buf2 ) );



    d = 'a';
    buf1[0] = d;

/* Note that buf1 is less than buf2, but count is zero,
   so it should return zero.
*/
    count = 0;
    outcome = 1;
    outcome = (int)_memicmp( buf1, buf2, count );
    checke( (outcome == 0), 1 );

    buf1[0] = c;



    d = 'y';
    buf1[(ALMOST_MAX - TEN_THOU)] = d;

    count = (ALMOST_MAX - TEN_THOU + 1);
    outcome = - 1;
    outcome = (int)_memicmp( buf1, buf2, count );
    checke( (outcome > 0), 1 );

    buf1[(ALMOST_MAX - TEN_THOU)] = c;



    for (count = ONE; count < TWENTY; count++)
    {
        outcome = -1;
        outcome = (int)_memicmp( buf1, buf2, count );
        checke( (outcome == 0), 1 );
    }



    d = 'd';
    for (count = HUNDRED; count < (HUNDRED + TWENTY); count++)
    {
        buf1[count - 1] = d;
        outcome = 0;
        outcome = (int)_memicmp( buf1, buf2, count );
        checke( (outcome < 0), 1 );
        buf1[count - 1] = c;
    }



    d = 'y';
    for (count = THOUSAND; count < (THOUSAND + TWENTY); count++)
    {
        buf1[count - 1] = d;
        outcome = 0;
        outcome = (int)_memicmp( buf1, buf2, count );
        checke( (outcome > 0), 1 );
        buf1[count - 1] = c;
    }



    for (count = TEN_THOU; count < (TEN_THOU + TWENTY); count++)
    {
        outcome = -1;
        outcome = (int)_memicmp( buf1, buf2, count );
        checke( (outcome == 0), 1 );
    }



    d = 'W';
    for (count = THIRTY_THOU; count < (THIRTY_THOU + TWENTY); count++)
    {
        buf1[count - 1] = d;
        outcome = 1;
        outcome = (int)_memicmp( buf1, buf2, count );
        checke( (outcome < 0), 1 );
        buf1[count - 1] = c;
    }



    d = '{';
    for (count = (ALMOST_MAX - TWENTY); count < ALMOST_MAX; count++)
    {
        buf1[count - 1] = d;
        outcome = 0;
        outcome = (int)_memicmp( buf1, buf2, count );
        checke( (outcome > 0), 1 );
        buf1[count - 1] = c;
    }




    finish();
}
