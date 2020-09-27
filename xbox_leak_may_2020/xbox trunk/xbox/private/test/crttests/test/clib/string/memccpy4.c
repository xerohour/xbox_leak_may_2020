
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memccpy4.c
                     memccpy4
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _memccpy() function.
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
 [ 0] 16-Apr-92    edv            created


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
#define DELTA 2000
#define ALMOST_MAX  (SHRT_MAX - DELTA)


/*  Scenario 9: Verify that dest and src can be almost as large as
        SHRT_MAX when using SMALL model.
*/

static unsigned char src[ALMOST_MAX];
static unsigned char dest[ALMOST_MAX] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static unsigned char ivals[10] = "!@#$%^&*()";


int memccpy4Entry(){
    int c, i;
    unsigned int count, j;


    startest();


/*  Scenario 10: Verify that count can be anywhere in the range
         0 to aproximately SHRT_MAX when using SMALL model.
*/

    memset( src, '+', ALMOST_MAX );
    c = '1';

    for (count = ZERO, i = ZERO; count < TWENTY; count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        _memccpy( dest, src, c, count );
        for (j = 0; j < count; j++)
            checke( dest[j], src[j] );
    }



    memset( src, '-', ALMOST_MAX );
    c = '2';

    for (count = HUNDRED, i = ZERO; count < (HUNDRED + TWENTY); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        _memccpy( dest, src, c, count );
        for (j = 0; j < count; j++)
            checke( dest[j], src[j] );
    }



    memset( src, '=', ALMOST_MAX );
    c = '3';

    for (count = THOUSAND, i = ZERO; count < (THOUSAND + TWENTY); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        _memccpy( dest, src, c, count );
        for (j = 0; j < count; j++)
            checke( dest[j], src[j] );
    }


#ifndef QA_SIMPLE_QALIB

    memset( src, '{', ALMOST_MAX );
    c = '4';
    i = ZERO;

    for (count = TEN_THOU; count < (TEN_THOU + TWENTY); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        _memccpy( dest, src, c, count );
        for (j = 0; j < count; j++)
            checke( dest[j], src[j] );
    }




    memset( src, '}', ALMOST_MAX );
    c = '5';
    i = ZERO;

    for (count = THIRTY_THOU; count < (THIRTY_THOU + TWENTY); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        _memccpy( dest, src, c, count );
        for (j = 0; j < count; j++)
            checke( dest[j], src[j] );
    }




    memset( src, '|', ALMOST_MAX );
    c = '6';
    i = ZERO;

    for (count = (ALMOST_MAX - TWENTY); count < ALMOST_MAX; count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        _memccpy( dest, src, c, count );
        for (j = 0; j < count; j++)
            checke( dest[j], src[j] );
    }

#endif

    finish();
}
