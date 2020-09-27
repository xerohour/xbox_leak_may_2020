
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memcmp1.c
                     memcmp1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the memcmp() function.
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
 [ 0] 01-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char bufval1[51] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYy";
static char bufval2[51] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxyz";
/*                                                                  ^
                                                         ___________|
*/

int memcmp1Entry(){
    int i, outcome;
    size_t count;
    char buf1[51], buf2[51];

    startest();


/*  Scenario 1: Verify that the function returns a negative value
                when buf1 < buf2.
*/
    for (i = 0; i < 51; i++ )
    {
        buf1[i] = bufval1[i];
        buf2[i] = bufval2[i];
    }
    count = 51;
    outcome = 999;
    outcome = memcmp( buf1, buf2, count);
    checke( (outcome < 0), 1 );



/*  Scenario 2: Verify that the function returns a positive value
                when buf1 > buf2.
*/
    for (i = 0; i < 51; i++ )
    {
        buf1[i] = bufval2[i];
        buf2[i] = bufval1[i];
    }
    count = 51;
    outcome = -999;
    outcome = memcmp( buf1, buf2, count);
    checke( (outcome > 0), 1 );



/*  Scenario 3: Verify that the function returns zero when buf1 = buf2.
*/
    for (i = 0; i < 51; i++ )
    {
        buf1[i] = bufval1[i];
        buf2[i] = bufval1[i];
    }
    count = 51;
    outcome = 999;
    outcome = memcmp( buf1, buf2, count);
    checke( (outcome == 0), 1 );


    finish();
}
