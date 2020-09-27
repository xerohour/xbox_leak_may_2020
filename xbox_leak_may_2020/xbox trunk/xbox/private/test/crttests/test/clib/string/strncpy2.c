
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strncpy2.c
                     strncpy2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strncpy() function.
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
 [ 0] 12-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

#define ZERO            0
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



/*  Scenario 6: Verify that string1 and string2 can be almost as
        large as SHRT_MAX when using SMALL model.
*/

static unsigned char string2[ALMOST_MAX];
static unsigned char string1[ALMOST_MAX] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static unsigned char ivals[10] = "!@#$%^&*()";


int strncpy2Entry(){
    unsigned int count, i, j;


    startest();


/*  Scenario 7: Verify that count can be anywhere in the range
        0 to aproximately SHRT_MAX when using SMALL model.
*/

    memset( string2, '+', ALMOST_MAX );

    for (count = ZERO, i = ZERO; count < TWENTY; count++, i++)
    {
        memset( string2, ivals[i], count );
        if (i == 9) i = 0;
        strncpy( string1, string2, count );
        for (j = 0; j < count; j++)
            checke( string1[j], string2[j] );
    }



    memset( string2, '-', ALMOST_MAX );
    i = 0;

    for (count = HUNDRED; count < (HUNDRED + MAX_STEPS); count++, i++)
    {
        memset( string2, ivals[i], count );
        if (i == 9) i = 0;
        strncpy( string1, string2, count );
        for (j = 0; j < count; j++)
            checke( string1[j], string2[j] );
    }



    memset( string2, '=', ALMOST_MAX );
    i = 0;

    for (count = THOUSAND; count < (THOUSAND + MAX_STEPS); count++, i++)
    {
        memset( string2, ivals[i], count );
        if (i == 9) i = 0;
        strncpy( string1, string2, count );
        for (j = 0; j < count; j++)
            checke( string1[j], string2[j] );
    }

#ifndef QA_SIMPLE_QALIB


    memset( string2, '{', ALMOST_MAX );
    i = 0;

    for (count = TEN_THOU; count < (TEN_THOU + MAX_STEPS); count++, i++)
    {
        memset( string2, ivals[i], count );
        if (i == 9) i = 0;
        strncpy( string1, string2, count );
        for (j = 0; j < count; j++)
            checke( string1[j], string2[j] );
    }




    memset( string2, '}', ALMOST_MAX );
    i = 0;

    for (count = THIRTY_THOU; count < (THIRTY_THOU + MAX_STEPS); count++, i++)
    {
        memset( string2, ivals[i], count );
        if (i == 9) i = 0;
        strncpy( string1, string2, count );
        for (j = 0; j < count; j++)
            checke( string1[j], string2[j] );
    }




    memset( string2, '|', ALMOST_MAX );
    i = 0;

    for (count = (ALMOST_MAX - MAX_STEPS); count < ALMOST_MAX; count++, i++)
    {
        memset( string2, ivals[i], count );
        if (i == 9) i = 0;
        strncpy( string1, string2, count );
        for (j = 0; j < count; j++)
            checke( string1[j], string2[j] );
    }

#endif

    finish();
}
