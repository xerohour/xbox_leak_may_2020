
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strrev2.c
                     strrev2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strrev() function.
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


#define DELTA  4000
#define ALMOST_MAX (USHRT_MAX - DELTA)


/*  Scenario 6: Verify that string can be almost as large as USHRT_MAX.
*/

static unsigned char string[ALMOST_MAX] = { "0123456789"
                                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "!@#$%^&*()_+-={}|[]:;'/?,.<>\0"
                                   };

static unsigned char baseline[65] = { "><.,?/';:][|}{=-+_)(*&^%$#@!"
                               "ZYXWVUTSRQPONMLKJIHGFEDCBA"
                               "9876543210\0"
                              };

int strrev2Entry(){
    int i;
    unsigned char *pstr;

    startest();



    pstr = _strrev( string );

    checke( pstr, string );

    for (i = 0; i < sizeof( baseline ); i++)
        checke( pstr[i], baseline[i] );

    checke( pstr[i], '\0' );




    finish();
}
