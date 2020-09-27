
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strlen2.c
                     strlen2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strlen() function.
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
 [ 0] 22-May-92    edv            created

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
#define FIFTY_THOU  50000
#define DELTA        4000
#define ALMOST_MAX  (USHRT_MAX - DELTA)

#define TWO	2
#ifdef QA_SIMPLE_QALIB
#define MAX_STEPS TWO
#else
#define MAX_STEPS TWENTY
#endif


/*  Scenario 7: Verify that string can be almost as large as USHRT_MAX.
*/

static unsigned char string[ALMOST_MAX];



int strlen2Entry(){
    unsigned int i;
    size_t length;


    startest();


    for (i = ZERO; i < ALMOST_MAX; i++)
        string[i] = 'X';



/*  Scenario 8: Verify that the return value can be anywhere in the range
        of 0 to almost USHRT_MAX.
*/

    for (i = ZERO; i < TWENTY; i++)
    {
        string[i] = '\0';
        length = strlen( string );
        checke( length, (size_t) i );
        string[i] = 'X';
    }


    for (i = HUNDRED; i < (HUNDRED + MAX_STEPS); i++)
    {
        string[i] = '\0';
        length = strlen( string );
        checke( length, (size_t) i );
        string[i] = 'X';
    }


    for (i = THOUSAND; i < (THOUSAND + MAX_STEPS); i++)
    {
        string[i] = '\0';
        length = strlen( string );
        checke( length, (size_t) i );
        string[i] = 'X';
    }


    for (i = TEN_THOU; i < (TEN_THOU + MAX_STEPS); i++)
    {
        string[i] = '\0';
        length = strlen( string );
        checke( length, (size_t) i );
        string[i] = 'X';
    }


    for (i = THIRTY_THOU; i < (THIRTY_THOU + MAX_STEPS); i++)
    {
        string[i] = '\0';
        length = strlen( string );
        checke( length, (size_t) i );
        string[i] = 'X';
    }


    for (i = FIFTY_THOU; i < (FIFTY_THOU + MAX_STEPS); i++)
    {
        string[i] = '\0';
        length = strlen( string );
        checke( length, (size_t) i );
        string[i] = 'X';
    }


    for (i = (ALMOST_MAX - MAX_STEPS); i < ALMOST_MAX; i++)
    {
        string[i] = '\0';
        length = strlen( string );
        checke( length, (size_t) i );
        string[i] = 'X';
    }




    finish();
}
