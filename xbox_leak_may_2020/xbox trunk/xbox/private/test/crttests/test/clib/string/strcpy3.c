
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strcpy3.c
                     strcpy3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strcpy() function.
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


/*  Scenario 7: Verify that string1 and string2 can be almost
        as large as SHRT_MAX.
*/

static unsigned char string1[ALMOST_MAX];
static unsigned char string2[ALMOST_MAX];



int strcpy3Entry(){
    int i;
    char *pstr;



    startest();


    string1[0] = '\0';

    memset( string2, 'x', sizeof( string2 ) );
    string2[ALMOST_MAX - 1] = '\0';


    string2[10] = '&';
    string2[100] = '*';
    string2[1000] = '%';

    pstr = strcpy( string1, string2 );
    checke( pstr, string1 );

    for(i = 0; i < sizeof(string2); i++)
       checke( pstr[i], string1[i] );



    finish();
}
