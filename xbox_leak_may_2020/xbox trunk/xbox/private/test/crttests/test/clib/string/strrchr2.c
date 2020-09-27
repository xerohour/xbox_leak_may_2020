
/***************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strrchr2.c
                     strrchr2
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

static unsigned char string[21] = "$$$$$$$$$$$$$$$$$$$$\0";


int strrchr2Entry(){
    int c;
    unsigned char *pstr;

    startest();


/*  Scenario 5: Verify that c can be any value from 0 to UCHAR_MAX,
                and will be recognized as the character to be found.
*/

    pstr = strrchr( string, '\0' );
    checke( pstr, string + 20);



    for (c = 1; c < '$'; c++)
    {
        string[1] = c;
        string[3] = c;
        string[5] = c;
        string[7] = c;
        string[9] = c;
        string[15] = c;
        pstr = strrchr( string, c );
        checke( pstr, string + 15 );
    }




/*  Change values in string to avoid finding the wrong dollar sign.
*/

    strcpy( string, "####################\0" );

    for (c = '$'; c <= UCHAR_MAX; c++)
    {
        string[1] = c;
        string[3] = c;
        string[5] = c;
        string[7] = c;
        string[9] = c;
        string[15] = c;
        pstr = strrchr( string, c );
        checke( pstr, string + 15 );
    }



    finish();
}
