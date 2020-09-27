
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl /AL memmove2.c
                     memmove2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the memmove() function.
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
 [ 0] 06-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
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


/*  Scenario 4: Verify that dest and src can be as large as
        USHRT_MAX when using LARGE model.

*/

static unsigned char src[USHRT_MAX];
static unsigned char dest[USHRT_MAX] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static unsigned char ivals[10] = "!@#$%^&*()";


int memmove2Entry(){
    int i;
    unsigned int count, j;

    startest();


/*  Scenario 5: Verify that count can be anywhere in the range
        0 to USHRT_MAX when using LARGE model.

*/

    memset( src, '+', USHRT_MAX );


    for (count = ZERO, i = ZERO; count < TWENTY; count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        memmove( dest, src, count );
        for (j = 0; j < count; j++)
            checke32( dest[j], src[j] );
    }




    memset( src, '-', USHRT_MAX );


    for (count = HUNDRED, i = ZERO; count < (HUNDRED + MAX_STEPS); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        memmove( dest, src, count );
        for (j = 0; j < count; j++)
            checke32( dest[j], src[j] );
    }




    memset( src, '=', USHRT_MAX );


    for (count = THOUSAND, i = ZERO; count < (THOUSAND + MAX_STEPS); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        memmove( dest, src, count );
        for (j = 0; j < count; j++)
            checke32( dest[j], src[j] );
    }



#ifndef QA_SIMPLE_QALIB

    memset( src, '{', USHRT_MAX );


    for (count = TEN_THOU, i = 0; count < (TEN_THOU + MAX_STEPS); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        memmove( dest, src, count );
        for (j = 0; j < count; j++)
            checke32( dest[j], src[j] );
    }





    memset( src, '}', USHRT_MAX );


    for (count = FORTY_THOU, i = 0; count < (FORTY_THOU + MAX_STEPS); count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        memmove( dest, src, count );
        for (j = 0; j < count; j++)
            checke32( dest[j], src[j] );
    }



    memset( src, '|', USHRT_MAX );



    for (count = (USHRT_MAX - MAX_STEPS), i = 0; count < USHRT_MAX; count++, i++)
    {
        memset( src, ivals[i], count );
        if (i == 9) i = 0;
        memmove( dest, src, count );
        for (j = 0; j < count; j++)
            checke32( dest[j], src[j] );
    }

#endif

    finish();
}
