
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strnicm3.c
                     strnicm3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strnicmp() function.
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
 [ 0] 12-May-92    edv            created

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


/*  Scenario 6: Verify that string1 and string2 can be almost
        as large as SHRT_MAX.
*/

static unsigned char string1[ALMOST_MAX];
static unsigned char string2[ALMOST_MAX];



int strnicm3Entry(){
    int c, d, outcome;
    size_t count;


    startest();


/*  Scenario 7: Verify that count can be anywhere in the
        range 0 to aproximately SHRT_MAX.
*/

    c = 'x';
    memset( string1, c, sizeof( string1 ) );
    memset( string2, c, sizeof( string2 ) );
    string1[sizeof( string1 ) -1] = string2[sizeof( string2 ) -1] = '\0';


    d = 'a';
    string1[0] = d;

/* Note that string1 is less than string2, but count is zero,
   so it should return zero.
*/
    count = 0;
    outcome = 1;
    outcome = _strnicmp( string1, string2, count );
    checke( (outcome == 0), 1 );

    string1[0] = c;



    d = 'y';
    string1[(ALMOST_MAX - TEN_THOU)] = d;

    count = (ALMOST_MAX - TEN_THOU + 1);
    outcome = - 1;
    outcome = _strnicmp( string1, string2, count );
    checke( (outcome > 0), 1 );

    string1[(ALMOST_MAX - TEN_THOU)] = c;



    for (count = ONE; count < TWENTY; count++)
    {
        outcome = -1;
        outcome = _strnicmp( string1, string2, count );
        checke( (outcome == 0), 1 );
    }



    d = 'd';
    for (count = HUNDRED; count < (HUNDRED + MAX_STEPS); count++)
    {
        string1[count - 1] = d;
        outcome = 0;
        outcome = _strnicmp( string1, string2, count );
        checke( (outcome < 0), 1 );
        string1[count - 1] = c;
    }



    d = 'y';
    for (count = THOUSAND; count < (THOUSAND + MAX_STEPS); count++)
    {
        string1[count - 1] = d;
        outcome = 0;
        outcome = _strnicmp( string1, string2, count );
        checke( (outcome > 0), 1 );
        string1[count - 1] = c;
    }


#ifndef QA_SIMPLE_QALIB

    for (count = TEN_THOU; count < (TEN_THOU + MAX_STEPS); count++)
    {
        outcome = -1;
        outcome = _strnicmp( string1, string2, count );
        checke( (outcome == 0), 1 );
    }



    d = 'W';
    for (count = THIRTY_THOU; count < (THIRTY_THOU + MAX_STEPS); count++)
    {
        string1[count - 1] = d;
        outcome = 1;
        outcome = _strnicmp( string1, string2, count );
        checke( (outcome < 0), 1 );
        string1[count - 1] = c;
    }



    d = '{';
    for (count = (ALMOST_MAX - MAX_STEPS); count < ALMOST_MAX; count++)
    {
        string1[count - 1] = d;
        outcome = 0;
        outcome = _strnicmp( string1, string2, count );
        checke( (outcome > 0), 1 );
        string1[count - 1] = c;
    }

#endif


    finish();
}
