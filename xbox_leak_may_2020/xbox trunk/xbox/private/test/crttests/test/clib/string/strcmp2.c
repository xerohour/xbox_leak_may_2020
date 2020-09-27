
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strcmp2.c
                     strcmp2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strcmp() function.
OS Version:          REAL/PROT/ALL
CPU:                 8086
Dependencies:        NONE
Environment Vars:    NONE
Verification Method: COMPILE/EXECUTE/COMPARE
Notes:               Must include STRING.H
                     Tests decribed in the testplan STRING.DOC
Products:            C800
Test Type:       DEPTH

Revision History:
 Date              emailname      description
---------------------------------------------------------------------------
 [ 0] 08-May-92    edv            created

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


/*  Scenario 6: Verify that string1 and string2 can be almost
        as large as SHRT_MAX.
*/

static unsigned char string1[ALMOST_MAX];
static unsigned char string2[ALMOST_MAX];



int strcmp2Entry(){
    int outcome;



    startest();



    memset( string1, 'x', sizeof( string1 ) );
    memset( string2, 'x', sizeof( string2 ) );
    string1[ALMOST_MAX - 1] = '\0';
    string2[ALMOST_MAX - 1] = '\0';


    outcome = 1;
    outcome = strcmp( string1, string2 );
    checke( (outcome == 0), 1 );



    string1[ALMOST_MAX - 2] = 'W';
    string2[ALMOST_MAX - 2] = 'w';

    outcome = 1;
    outcome = strcmp( string1, string2 );
    checke( (outcome < 0), 1 );



    string1[ALMOST_MAX - 3] = 'y';

    outcome = -1;
    outcome = strcmp( string1, string2 );
    checke( (outcome > 0), 1 );




    finish();
}
