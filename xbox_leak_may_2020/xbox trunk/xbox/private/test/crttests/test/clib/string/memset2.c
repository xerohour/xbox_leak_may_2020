
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl /AL memset2.c
                     memset2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the memset() function.
OS Version:          REAL/PROT/ALL
CPU:                 8086
Dependencies:        Must be compiled using LARGE model
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

#define TWO            2
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

/*  Scenario 3: Verify that buf can be as large as USHRT_MAX
                when using LARGE model.
*/

static unsigned char dest[USHRT_MAX] = "813748973498 JUNK 71239847981237481720177";


int memset2Entry(){
    int c, d;
    size_t count;
    unsigned char *pdest;



    startest();



/*  Scenario 4: Verify that count can be anywhere in the range of
        0 to USHRT_MAX when using the LARGE model.
*/
    c = 'A';

    count = 0;
    pdest = memset( dest, c, count );
    checke( pdest, dest );
    checke( dest[0], '8' );            /*  still 8 because count = 0  */



    d = 0x00;

    for (count = TWO; count < TWENTY; count++)
    {
        dest[count] = d;                   /* NULL to terminate string */
        pdest = memset( dest, c, count );
        checke( dest[0], c );
        checke( dest[count - 1], c );
        checke( dest[count], d );
        dest[count] = c;
    }



    for (count = HUNDRED; count < (HUNDRED + MAX_STEPS); count++)
    {
        dest[count] = d;                   /* NULL to terminate string */
        pdest = memset( dest, c, count );
        checke( dest[0], c );
        checke( dest[count - 1], c );
        checke( dest[count], d );
        dest[count] = c;
    }


    for (count = THOUSAND; count < (THOUSAND + MAX_STEPS); count++)
    {
        dest[count] = d;                   /* NULL to terminate string */
        pdest = memset( dest, c, count );
        checke( dest[0], c );
        checke( dest[count - 1], c );
        checke( dest[count], d );
        dest[count] = c;
    }


    for (count = TEN_THOU; count < (TEN_THOU + MAX_STEPS); count++)
    {
        dest[count] = d;                   /* NULL to terminate string */
        pdest = memset( dest, c, count );
        checke( dest[0], c );
        checke( dest[count - 1], c );
        checke( dest[count], d );
        dest[count] = c;
    }


    for (count = FORTY_THOU; count < (FORTY_THOU + MAX_STEPS); count++)
    {
        dest[count] = d;                   /* NULL to terminate string */
        pdest = memset( dest, c, count );
        checke( dest[0], c );
        checke( dest[count - 1], c );
        checke( dest[count], d );
        dest[count] = c;
    }


    for (count = (USHRT_MAX - MAX_STEPS); count < USHRT_MAX; count++)
    {
        dest[count] = d;                   /* NULL to terminate string */
        pdest = memset( dest, c, count );
        checke( dest[0], c );
        checke( dest[count - 1], c );
        checke( dest[count], d );
        dest[count] = c;
    }


    finish();
}
