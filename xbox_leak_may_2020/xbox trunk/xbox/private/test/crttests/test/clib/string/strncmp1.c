
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strncmp1.c
                     strncmp1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strncmp() function.
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
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char strval1[51] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYy";
static char strval2[51] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxyz";
/*                                                                  ^
                                                         ___________|
*/

int strncmp1Entry(){
    int i, outcome;
    size_t count;
    char str1[51], str2[51];

    startest();


/*  Scenario 1: Verify that the function returns a negative value
                when string1 < string2.
*/
    for (i = 0; i < 51; i++ )
    {
        str1[i] = strval1[i];
        str2[i] = strval2[i];
    }
    count = 51;
    outcome = 999;
    outcome = strncmp( str1, str2, count);
    checke( (outcome < 0), 1 );



/*  Scenario 2: Verify that the function returns a positive value
                when string1 > string2.
*/
    for (i = 0; i < 51; i++ )
    {
        str1[i] = strval2[i];             /*  Just reverse values   */
        str2[i] = strval1[i];
    }
    count = 51;
    outcome = -999;
    outcome = strncmp( str1, str2, count);
    checke( (outcome > 0), 1 );



/*  Scenario 3: Verify that the function returns zero
                when string1 = string2.
*/
    for (i = 0; i < 51; i++ )
    {
        str1[i] = strval1[i];             /*  Just repeat values   */
        str2[i] = strval1[i];
    }
    count = 51;
    outcome = 999;
    outcome = strncmp( str1, str2, count);
    checke( (outcome == 0), 1 );


    finish();
}
