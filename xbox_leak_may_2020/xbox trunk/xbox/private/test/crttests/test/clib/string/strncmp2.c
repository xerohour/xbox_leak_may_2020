
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strncmp2.c
                     strncmp2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strncmp() function.
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
 [ 0] 11-May-92    edv            created

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

#define TWO	2
#ifdef QA_SIMPLE_QALIB
#define MAX_STEPS TWO
#else
#define MAX_STEPS TWENTY
#endif


/*  Scenario 6: Verify that str1 and str2 can be almost as large as
        SHRT_MAX when using SMALL model.
*/

static unsigned char str1[ALMOST_MAX];
static unsigned char str2[ALMOST_MAX];



int strncmp2Entry(){
    int c, d, outcome;
    size_t count;


    startest();


/*  Scenario 4: Verify that count can be anywhere in the range
        0 to aproximately SHRT_MAX.
*/

    c = 'x';
    memset( str1, c, sizeof( str1 ) );
    memset( str2, c, sizeof( str2 ) );



    d = 'w';
    str1[0] = d;

/* Note that str1 is less than str2, but count is zero,
   so it should return zero.
*/
    count = 0;
    outcome = 1;
    outcome = strncmp( str1, str2, count );
    checke( (outcome == 0), 1 );

    str1[0] = c;



    d = 'y';
    str1[(ALMOST_MAX - TEN_THOU)] = d;

    count = (ALMOST_MAX - TEN_THOU + 1);
    outcome = - 1;
    outcome = strncmp( str1, str2, count );
    checke( (outcome > 0), 1 );

    str1[(ALMOST_MAX - TEN_THOU)] = c;



    for (count = ONE; count < TWENTY; count++)
    {
        outcome = -1;
        outcome = strncmp( str1, str2, count );
        checke( (outcome == 0), 1 );
    }



    d = 'd';
    for (count = HUNDRED; count < (HUNDRED + MAX_STEPS); count++)
    {
        str1[count - 1] = d;
        outcome = 0;
        outcome = strncmp( str1, str2, count );
        checke( (outcome < 0), 1 );
        str1[count - 1] = c;
    }



    d = 'y';
    for (count = THOUSAND; count < (THOUSAND + MAX_STEPS); count++)
    {
        str1[count - 1] = d;
        outcome = 0;
        outcome = strncmp( str1, str2, count );
        checke( (outcome > 0), 1 );
        str1[count - 1] = c;
    }



    for (count = TEN_THOU; count < (TEN_THOU + MAX_STEPS); count++)
    {
        outcome = -1;
        outcome = strncmp( str1, str2, count );
        checke( (outcome == 0), 1 );
    }



    d = 'X';
    for (count = THIRTY_THOU; count < (THIRTY_THOU + MAX_STEPS); count++)
    {
        str1[count - 1] = d;
        outcome = 1;
        outcome = strncmp( str1, str2, count );
        checke( (outcome < 0), 1 );
        str1[count - 1] = c;
    }



    d = '{';
    for (count = (ALMOST_MAX - MAX_STEPS); count < ALMOST_MAX; count++)
    {
        str1[count - 1] = d;
        outcome = 0;
        outcome = strncmp( str1, str2, count );
        checke( (outcome > 0), 1 );
        str1[count - 1] = c;
    }




    finish();
}
