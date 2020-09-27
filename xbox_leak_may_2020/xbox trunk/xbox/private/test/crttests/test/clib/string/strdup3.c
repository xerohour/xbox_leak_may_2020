
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strdup3.c
                     strdup3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strdup() function.
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
 [ 0] 28-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


#define DELTA        2000
#define ALMOST_MAX  (SHRT_MAX - DELTA)


/*  Scenario 6: Verify that string can be almost as large as SHRT_MAX.
*/

static char string[ALMOST_MAX] = "JUNK";


int strdup3Entry(){
    int i;
    char *pstr;



    startest();



    for (i = 4; i < ALMOST_MAX - 1; i++)   /*   Initialize string   */
        string[i] = '*';

    string[i] = '\0';                      /* Null-terminate string */


    pstr = _strdup( string );

    checke( pstr[0], 'J' );
    checke( pstr[1], 'U' );
    checke( pstr[2], 'N' );
    checke( pstr[3], 'K' );
    checke( pstr[4], '*' );

    for (i = (ALMOST_MAX - 40); i < (ALMOST_MAX - 2); i++)
        checke( pstr[i], '*' );

    checke( pstr[ALMOST_MAX - 1], '\0' );





    finish();
}
