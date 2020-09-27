
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memset3.c
                     memset3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the memset() function.
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
#define TWO             2
#define TWENTY         20
#define HUNDRED       100
#define THOUSAND     1000
#define TEN_THOU    10000
#define THIRTY_THOU 30000
#define DELTA        1850
#define ALMOST_MAX  (SHRT_MAX - DELTA)


/*  Scenario 5: Verify that dest can be almost as large as
        SHRT_MAX when using SMALL model.
*/

static unsigned char dest[ALMOST_MAX] = "813748973498 JUNK 71239847981237481720177";


int memset3Entry(){
    int c, d;
    size_t count;
    unsigned char *pdest;



    startest();



/*  Scenario 6: Verify that count can be anywhere in the range of
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



    for (count = HUNDRED; count < (HUNDRED + TWENTY); count++)
    {
        dest[count] = d;                   /* NULL to terminate string */
        pdest = memset( dest, c, count );
        checke( dest[0], c );
        checke( dest[count - 1], c );
        checke( dest[count], d );
        dest[count] = c;
    }


    for (count = THOUSAND; count < (THOUSAND + TWENTY); count++)
    {
        dest[count] = d;                   /* NULL to terminate string */
        pdest = memset( dest, c, count );
        checke( dest[0], c );
        checke( dest[count - 1], c );
        checke( dest[count], d );
        dest[count] = c;
    }


    for (count = TEN_THOU; count < (TEN_THOU + TWENTY); count++)
    {
        dest[count] = d;                   /* NULL to terminate string */
        pdest = memset( dest, c, count );
        checke( dest[0], c );
        checke( dest[count - 1], c );
        checke( dest[count], d );
        dest[count] = c;
    }


    for (count = THIRTY_THOU; count < (THIRTY_THOU + TWENTY); count++)
    {
        dest[count] = d;                   /* NULL to terminate string */
        pdest = memset( dest, c, count );
        checke( dest[0], c );
        checke( dest[count - 1], c );
        checke( dest[count], d );
        dest[count] = c;
    }


    for (count = (ALMOST_MAX - TWENTY); count < ALMOST_MAX; count++)
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
