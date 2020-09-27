
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl /AL memicmp3.c
                     memicmp3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _memicmp() function.
OS Version:          REAL/PROT/ALL
CPU:                 8086
Dependencies:        Must be compiled with LARGE STRING model.
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


#define ONE            1
#define TWENTY        20
#define HUNDRED      100
#define THOUSAND    1000
#define TEN_THOU   10000
#define FORTY_THOU 40000

#define TWO	2
#ifdef QA_SIMPLE_QALIB
#define MAX_STEPS TWO
#else
#define MAX_STEPS TWENTY
#endif


/*  Scenario 6: Verify that buf1 and buf2 can be as large as
        USHRT_MAX when using LARGE model.
*/

static unsigned char buf1[USHRT_MAX];
static unsigned char buf2[USHRT_MAX];



int memicmp3Entry(){
    int c, d, outcome;
    size_t count;


    startest();


/*  Scenario 7: Verify that count can be anywhere in the range
        0 to USHRT_MAX when using the LARGE model.
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



    d = 'z';
    buf1[(USHRT_MAX - TEN_THOU)] = d;
    count = (USHRT_MAX - TEN_THOU + 1);
    outcome = - 1;

    outcome = (int)_memicmp( buf1, buf2, count );
    checke( (outcome > 0), 1 );

    buf1[(USHRT_MAX - TEN_THOU)] = c;



    for (count = ONE; count < TWENTY; count++)
    {
        outcome = -1;
        outcome = (int)_memicmp( buf1, buf2, count );
        checke( (outcome == 0), 1 );
    }



    d = 'd';
    for (count = HUNDRED; count < (HUNDRED + MAX_STEPS ); count++)
    {
        buf1[count - 1] = d;
        outcome = 0;
        outcome = (int)_memicmp( buf1, buf2, count );
        checke( (outcome < 0), 1 );
        buf1[count - 1] = c;
    }



    d = 'y';
    for (count = THOUSAND; count < (THOUSAND + MAX_STEPS ); count++)
    {
        buf1[count - 1] = d;
        outcome = 0;
        outcome = (int)_memicmp( buf1, buf2, count );
        checke( (outcome > 0), 1 );
        buf1[count - 1] = c;
    }

#ifndef QA_SIMPLE_QALIB

    for (count = TEN_THOU; count < (TEN_THOU + MAX_STEPS ); count++)
    {
        outcome = -1;
        outcome = (int)_memicmp( buf1, buf2, count );
        checke( (outcome == 0), 1 );
    }



    d = 'w';
    for (count = FORTY_THOU; count < (FORTY_THOU + MAX_STEPS ); count++)
    {
        buf1[count - 1] = d;
        outcome = 1;
        outcome = (int)_memicmp( buf1, buf2, count );
        checke( (outcome < 0), 1 );
        buf1[count - 1] = c;
    }


    d = '{';
    for (count = (USHRT_MAX - MAX_STEPS ); count < USHRT_MAX; count++)
    {
        buf1[count - 1] = d;
        outcome = 0;
        outcome = (int)_memicmp( buf1, buf2, count );
        checke( (outcome > 0), 1 );
        buf1[count - 1] = c;
    }

#endif

    finish();
}
