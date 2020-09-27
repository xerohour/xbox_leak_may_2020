
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strchr3.c
                     strchr3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strchr() function.
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
 [ 0] 08-May-92    edv            created
 [ 1] 28-Jan-98    a-hemalk       64bit warning (C4311) fix.

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

#define DELTA        4000
#define ALMOST_MAX  (USHRT_MAX - DELTA)


/*  Scenario 6: Verify that string can be almost as large as USHRT_MAX.
*/

static unsigned char string[ALMOST_MAX];



int strchr3Entry(){
    int c;
    unsigned char *pstr;


    startest();



    c = '^';
    memset( string, c, sizeof(string) );  /* Fill string with '^'        */
    string[ALMOST_MAX - 1] = '\0';        /* NULL terminate the string   */




    pstr = strchr( string, '#' );
    checkNULL( pstr );                    /* NULL because no # in string */





    string[ALMOST_MAX - 2] = 'X';
    pstr  = strchr( string, 'X' );
    checknNULL(pstr);





    string[100] = 'X';
    pstr  = strchr( string, 'X' );
    checknNULL(pstr);   /* Ensure that we did find occurence - and not a null */



    finish();
}
