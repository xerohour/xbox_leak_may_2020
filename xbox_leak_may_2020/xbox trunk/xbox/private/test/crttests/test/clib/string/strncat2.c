
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strncat2.c
                     strncat2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strncat() function.
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
 [ 0] 20-May-92    edv            created

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
#define DELTA        2000
#define ALMOST_MAX  (SHRT_MAX - DELTA)

#define TWO	2
#ifdef QA_SIMPLE_QALIB
#define MAX_STEPS TWO
#else
#define MAX_STEPS TWENTY
#endif


/*  Scenario 8: Verify that string1 and string2 can be almost
        as large as SHRT_MAX.
*/

static char string1[ALMOST_MAX] = "\0";
static char string2[ALMOST_MAX] = "\0";


int strncat2Entry(){
    int i, c, Null='\0';
    size_t count;
    char *pstr;



    startest();



/*  Scenario 9: Verify that count can be anywhere in the range
        0 to almost SHRT_MAX.
*/
    c = 'X';

    for (i = 0; i < (ALMOST_MAX - 2); i++)       /* Initialize string2 */
        string2[i] = c;

    string2[ALMOST_MAX - 1] = Null;

    for (count = TWO; count < TWENTY; count++)
    {
        string1[0] = Null;                       /* Set string1 to NULL */
        pstr = strncat( string1, string2, count );
        checke( pstr[0], c );
        checke( pstr[1], c );
        checke( pstr[count - 2], c );
        checke( pstr[count - 1], c );
        checke( pstr[count], Null );
    }



    for (count = HUNDRED; count < (HUNDRED + MAX_STEPS); count++)
    {
        string1[0] = Null;                       /* Set string1 to NULL */
        pstr = strncat( string1, string2, count );
        checke( string1[0], c );
        checke( string1[1], c );
        checke( string1[count - 50], c );
        checke( string1[count - 25], c );
        checke( string1[count - 2], c );
        checke( string1[count - 1], c );
        checke( string1[count], Null );
    }


    for (count = THOUSAND; count < (THOUSAND + MAX_STEPS); count++)
    {
        string1[0] = Null;                       /* Set string1 to NULL */
        pstr = strncat( string1, string2, count );
        checke( string1[0], c );
        checke( string1[1], c );
        checke( string1[count - 500], c );
        checke( string1[count - 250], c );
        checke( string1[count - 50], c );
        checke( string1[count - 25], c );
        checke( string1[count - 2], c );
        checke( string1[count - 1], c );
        checke( string1[count], Null );
    }


    for (count = TEN_THOU; count < (TEN_THOU + MAX_STEPS); count++)
    {
        string1[0] = Null;                       /* Set string1 to NULL */
        pstr = strncat( string1, string2, count );
        checke( string1[0], c );
        checke( string1[1], c );
        checke( string1[count - 5000], c );
        checke( string1[count - 2500], c );
        checke( string1[count - 500], c );
        checke( string1[count - 250], c );
        checke( string1[count - 50], c );
        checke( string1[count - 25], c );
        checke( string1[count - 2], c );
        checke( string1[count - 1], c );
        checke( string1[count], Null );
    }


    for (count = THIRTY_THOU; count < (THIRTY_THOU + MAX_STEPS); count++)
    {
        string1[0] = Null;                       /* Set string1 to Null */
        pstr = strncat( string1, string2, count );
        checke( string1[0], c );
        checke( string1[1], c );
        checke( string1[count - 25000], c );
        checke( string1[count - 5000], c );
        checke( string1[count - 2500], c );
        checke( string1[count - 500], c );
        checke( string1[count - 250], c );
        checke( string1[count - 50], c );
        checke( string1[count - 25], c );
        checke( string1[count - 2], c );
        checke( string1[count - 1], c );
        checke( string1[count], Null );
    }


    for (count = (ALMOST_MAX - MAX_STEPS); count < (ALMOST_MAX - 1); count++)
    {
        string1[0] = Null;                       /* Set string1 to Null */
        pstr = strncat( string1, string2, count );
        checke( string1[0], c );
        checke( string1[1], c );
        checke( string1[count - 25000], c );
        checke( string1[count - 5000], c );
        checke( string1[count - 2500], c );
        checke( string1[count - 500], c );
        checke( string1[count - 250], c );
        checke( string1[count - 50], c );
        checke( string1[count - 25], c );
        checke( string1[count - 2], c );
        checke( string1[count - 1], c );
        checke( string1[count], Null );
    }





    finish();
}
