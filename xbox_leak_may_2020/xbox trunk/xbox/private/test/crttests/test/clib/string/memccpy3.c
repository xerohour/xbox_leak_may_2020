
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl /AL memccpy3.c
                     memccpy3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _memccpy() function.
OS Version:          REAL/PROT/ALL
CPU:                 8086
Dependencies:        Must be compiled with LARGE memory model.
Environment Vars:    NONE
Verification Method: COMPILE/EXECUTE/COMPARE
Notes:               Must include STRING.H
                     Tests described in the testplan STRING.DOC
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

static char test[] =__FILE__;
static int perfect;

#define ZERO           0
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


/*  Scenario 7: Verify that dest and src can be as large as
        USHRT_MAX when using LARGE model.

*/

static unsigned char src[USHRT_MAX];
static unsigned char dest[USHRT_MAX] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static unsigned char ivals[10] = "!@#$%^&*()";


int memccpy3Entry(){
    int c, i;
    unsigned int count, j;

    startest();


/*  Scenario 8: Verify that count can be anywhere in the range
        0 to USHRT_MAX when using LARGE model.

*/

    memset( src, '+', USHRT_MAX );
    c = '1';

    for (count = ZERO, i = ZERO; count < TWENTY; count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        _memccpy( dest, src, c, count );
        for (j = 0; j < count; j++)
            checke32( dest[j], src[j] );
    }



    memset( src, '-', USHRT_MAX );
    c = '2';

    for (count = HUNDRED, i = ZERO; count < (HUNDRED + MAX_STEPS); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        _memccpy( dest, src, c, count );
        for (j = 0; j < count; j++)
            checke32( dest[j], src[j] );
    }



    memset( src, '=', USHRT_MAX );
    c = '3';

    for (count = THOUSAND, i = ZERO; count < (THOUSAND + MAX_STEPS); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        _memccpy( dest, src, c, count );
        for (j = 0; j < count; j++)
            checke32( dest[j], src[j] );
    }



    memset( src, '{', USHRT_MAX );
    c = '4';
    i = ZERO;

    for (count = TEN_THOU; count < (TEN_THOU + MAX_STEPS); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        _memccpy( dest, src, c, count );
        for (j = 0; j < count; j++)
            checke32( dest[j], src[j] );
    }




    memset( src, '}', USHRT_MAX );
    c = '5';
    i = ZERO;

    for (count = FORTY_THOU; count < (FORTY_THOU + MAX_STEPS); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        _memccpy( dest, src, c, count );
        for (j = 0; j < count; j++)
            checke32( dest[j], src[j] );
    }

#ifndef QA_SIMPLE_QALIB

    memset( src, '|', USHRT_MAX );
    c = '6';
    i = ZERO;

    for (count = (USHRT_MAX - TWENTY); count < USHRT_MAX; count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        _memccpy( dest, src, c, count );
        for (j = 0; j < count; j++)
            checke32( dest[j], src[j] );
    }
#endif


    finish();
}
