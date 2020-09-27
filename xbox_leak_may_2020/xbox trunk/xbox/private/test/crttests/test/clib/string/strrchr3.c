
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strrchr3.c
                     strrchr3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strrchr() function.
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
 [ 0] 13-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

#define DELTA        1850
#define ALMOST_MAX  (SHRT_MAX - DELTA)


/*  Scenario 6: Verify that string can be almost as large as SHRT_MAX.
*/

static unsigned char string[ALMOST_MAX];



int strrchr3Entry(){
    int c;
    unsigned char *pstr;


    startest();



    c = '^';
    memset( string, c, sizeof(string) );  /* Fill string with '^'        */
    string[ALMOST_MAX - 1] = '\0';        /* NULL terminate the string   */



    pstr = strrchr( string, '#' );
    checkNULL( pstr );                    /* NULL because no # in string */



    string[ALMOST_MAX - 2000] = 'X';      /* Prepare string for the test */
    string[ALMOST_MAX - 1500] = 'X';
    string[ALMOST_MAX - 1000] = 'X';
    string[ALMOST_MAX - 500]  = 'X';
    string[ALMOST_MAX - 200]  = 'X';
    string[ALMOST_MAX - 170]  = 'X';
    string[ALMOST_MAX - 150]  = 'X';
    string[ALMOST_MAX - 130]  = 'X';
    string[ALMOST_MAX - 100]  = 'X';
    string[ALMOST_MAX - 50]   = 'X';
    string[ALMOST_MAX - 25]   = 'X';
    string[ALMOST_MAX - 2]    = 'X';


    pstr  = strrchr( string, 'X' );

    checke( pstr, string + ALMOST_MAX - 2 );




    finish();
}
