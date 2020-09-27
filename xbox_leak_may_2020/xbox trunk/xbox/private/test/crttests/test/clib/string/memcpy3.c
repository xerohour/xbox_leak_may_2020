
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memcpy3.c
                     memcpy3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the memcpy() function.
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

#define ZERO            0
#define TWENTY         20
#define HUNDRED       100
#define THOUSAND     1000
#define TEN_THOU    10000
#define THIRTY_THOU 30000
#define DELTA        2000
#define ALMOST_MAX  (SHRT_MAX - DELTA)


/*  Scenario 6 Verify that dest and src can be almost as large as
        SHRT_MAX when using SMALL model.
*/

static unsigned char src[ALMOST_MAX];
static unsigned char dest[ALMOST_MAX] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static unsigned char ivals[10] = "!@#$%^&*()";


int memcpy3Entry(){
    unsigned int count, i, j;


    startest();


/*  Scenario 7: Verify that count can be anywhere in the range
         0 to aproximately SHRT_MAX when using SMALL model.
*/

    memset( src, '+', ALMOST_MAX );

    for (count = ZERO, i = ZERO; count < TWENTY; count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        memcpy( dest, src, count );
        for (j = 0; j < count; j++)
            checke( dest[j], src[j] );
    }



    memset( src, '-', ALMOST_MAX );
    i = 0;

    for (count = HUNDRED; count < (HUNDRED + TWENTY); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        memcpy( dest, src, count );
        for (j = 0; j < count; j++)
            checke( dest[j], src[j] );
    }



    memset( src, '=', ALMOST_MAX );
    i = 0;

    for (count = THOUSAND; count < (THOUSAND + TWENTY); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        memcpy( dest, src, count );
        for (j = 0; j < count; j++)
            checke( dest[j], src[j] );
    }

#ifndef QA_SIMPLE_QALIB

    memset( src, '{', ALMOST_MAX );
    i = 0;

    for (count = TEN_THOU; count < (TEN_THOU + TWENTY); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        memcpy( dest, src, count );
        for (j = 0; j < count; j++)
            checke( dest[j], src[j] );
    }




    memset( src, '}', ALMOST_MAX );
    i = 0;

    for (count = THIRTY_THOU; count < (THIRTY_THOU + TWENTY); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        memcpy( dest, src, count );
        for (j = 0; j < count; j++)
            checke( dest[j], src[j] );
    }




    memset( src, '|', ALMOST_MAX );
    i = 0;

    for (count = (ALMOST_MAX - TWENTY); count < ALMOST_MAX; count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        memcpy( dest, src, count );
        for (j = 0; j < count; j++)
            checke( dest[j], src[j] );
    }

#endif

    finish();
}
