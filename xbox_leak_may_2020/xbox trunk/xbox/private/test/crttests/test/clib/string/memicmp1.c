
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memicmp1.c
                     memicmp1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _memicmp() function.
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


static char buf1[51] = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyY";
static char buf2[51] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYy";


int memicmp1Entry(){
    int outcome;
    size_t count;


    startest();


    count = sizeof( buf1 );


/*  Scenario 1: Verify that the function returns a negative value
                when buf1 < buf2.
*/
    buf1[49] = 'X';
    outcome = 0;
    outcome = (int)_memicmp( buf1, buf2, count);
    checke( (outcome < 0), 1 );
    buf1[49] = 'Y';



/*  Scenario 2: Verify that the function returns a positive value
                when buf1 > buf2.
*/
    buf1[49] = '{';
    outcome = -999;
    outcome = (int)_memicmp( buf1, buf2, count);
    checke( (outcome > 0), 1 );
    buf1[49] = 'Y';



/*  Scenario 3: Verify that the function returns zero when buf1 = buf2.
*/
    outcome = 999;
    outcome = (int)_memicmp( buf1, buf2, count);
    checke( (outcome == 0), 1 );


    finish();
}
