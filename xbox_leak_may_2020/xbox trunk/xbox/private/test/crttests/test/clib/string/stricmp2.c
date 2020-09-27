
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl stricmp2.c
                     stricmp2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _stricmp() function.
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
 [ 0] 11-May-92    edv            created

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


/*  Scenario 6: Verify that string1 and string2 can be almost as large as
        SHRT_MAX when using SMALL model.
*/

static unsigned char string1[ALMOST_MAX];
static unsigned char string2[ALMOST_MAX];



int stricmp2Entry(){
    int outcome;



    startest();


/*  Scenario 9: Verify that count can be anywhere in the range
        0 to aproximately SHRT_MAX when using SMALL model.
*/

    memset( string1, '*', sizeof( string1 ) );
    memset( string2, '*', sizeof( string2 ) );

    string1[sizeof(string1) - 1] = '\0';
    string2[sizeof(string2) - 1] = '\0';

    string1[sizeof(string1) - 2] = 'z';
    string2[sizeof(string2) - 2] = 'Z';

    string1[0] = 'A';
    string2[0] = 'a';

    string1[100] = '^';
    string2[100] = '^';

    outcome = -1;
    outcome = _stricmp( string1, string2 );
    checke( (outcome == 0), 1 );




    string1[ALMOST_MAX - 2] = '1';
    string2[ALMOST_MAX - 2] = '2';

    outcome = 1;
    outcome = _stricmp( string1, string2 );
    checke( (outcome < 0), 1 );





    string1[ALMOST_MAX - 2] = '3';

    outcome = -1;
    outcome = _stricmp( string1, string2 );
    checke( (outcome > 0), 1 );





    finish();
}
