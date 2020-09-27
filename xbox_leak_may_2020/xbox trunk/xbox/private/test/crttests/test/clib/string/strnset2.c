
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strnset2.c
                     strnset2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strnset() function.
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
 [ 0] 14-May-92    edv            created

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
#define DELTA        4000
#define ALMOST_MAX  (USHRT_MAX - DELTA)


#define TWO	2
#ifdef QA_SIMPLE_QALIB
#define MAX_STEPS TWO
#else
#define MAX_STEPS TWENTY
#endif

/*  Scenario 4: Verify that string can be almost as large as USHRT_MAX.
*/

static unsigned char string[ALMOST_MAX] = "JUNK\0";


int strnset2Entry(){
    int c, d, i;
    size_t count;
    unsigned char *pstr;



    startest();



/*  Scenario 5: Verify that count can be anywhere in the range
        0 to almost SHRT_MAX.
*/
    c = 'A';

    count = 0;
    pstr = _strnset( string, c, count );
    checke( pstr, string );

    checke( string[0], 'J' );                 /*  Still JUNK because  */
    checke( string[1], 'U' );                 /*   count is zero.     */
    checke( string[2], 'N' );
    checke( string[3], 'K' );
    checke( string[4], '\0' );



    for (i = 0; i < (ALMOST_MAX - 2); i++)  /* Prepare string for test */
        string[i] = 'X';

    d = '\0';                               /* Set d to NULL */
    string[ALMOST_MAX - 1] = d;

    for (count = TWO; count < TWENTY; count++)
    {
        string[count] = d;                   /* NULL to terminate string */
        pstr = _strnset( string, c, count );
        checke( string[0], c );
        checke( string[1], c );
        checke( string[count - 2], c );
        checke( string[count - 1], c );
        checke( string[count], d );
        string[count] = c;
    }



    for (count = HUNDRED; count < (HUNDRED + MAX_STEPS ); count++)
    {
        string[count] = d;                   /* NULL to terminate string */
        pstr = _strnset( string, c, count );
        checke( string[0], c );
        checke( string[1], c );
        checke( string[count - 50], c );
        checke( string[count - 25], c );
        checke( string[count - 2], c );
        checke( string[count - 1], c );
        checke( string[count], d );
        string[count] = c;
    }


    for (count = THOUSAND; count < (THOUSAND + MAX_STEPS ); count++)
    {
        string[count] = d;                   /* NULL to terminate string */
        pstr = _strnset( string, c, count );
        checke( string[0], c );
        checke( string[1], c );
        checke( string[count - 500], c );
        checke( string[count - 250], c );
        checke( string[count - 50], c );
        checke( string[count - 25], c );
        checke( string[count - 2], c );
        checke( string[count - 1], c );
        checke( string[count], d );
        string[count] = c;
    }


    for (count = TEN_THOU; count < (TEN_THOU + MAX_STEPS ); count++)
    {
        string[count] = d;                   /* NULL to terminate string */
        pstr = _strnset( string, c, count );
        checke( string[0], c );
        checke( string[1], c );
        checke( string[count - 5000], c );
        checke( string[count - 2500], c );
        checke( string[count - 500], c );
        checke( string[count - 500], c );
        checke( string[count - 250], c );
        checke( string[count - 50], c );
        checke( string[count - 25], c );
        checke( string[count - 2], c );
        checke( string[count - 1], c );
        checke( string[count], d );
        string[count] = c;
    }


    for (count = THIRTY_THOU; count < (THIRTY_THOU + MAX_STEPS ); count++)
    {
        string[count] = d;                   /* NULL to terminate string */
        pstr = _strnset( string, c, count );
        checke( string[0], c );
        checke( string[1], c );
        checke( string[count - 25000], c );
        checke( string[count - 5000], c );
        checke( string[count - 2500], c );
        checke( string[count - 500], c );
        checke( string[count - 500], c );
        checke( string[count - 250], c );
        checke( string[count - 50], c );
        checke( string[count - 25], c );
        checke( string[count - 2], c );
        checke( string[count - 1], c );
        checke( string[count], d );
        string[count] = c;
    }


    for (count = (ALMOST_MAX - MAX_STEPS ); count < ALMOST_MAX; count++)
    {
        string[count] = d;                   /* NULL to terminate string */
        pstr = _strnset( string, c, count );
        checke( string[0], c );
        checke( string[1], c );
        checke( string[count - 25000], c );
        checke( string[count - 5000], c );
        checke( string[count - 2500], c );
        checke( string[count - 500], c );
        checke( string[count - 500], c );
        checke( string[count - 250], c );
        checke( string[count - 50], c );
        checke( string[count - 25], c );
        checke( string[count - 2], c );
        checke( string[count - 1], c );
        checke( string[count], d );
        string[count] = c;
    }





    finish();
}
