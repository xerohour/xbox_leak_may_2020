
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strrchr1.c
                     strrchr1
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
 [ 0] 12-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char string[51] = "AaBbCcDdE1FfGgHhIiJ1KkLlMmNnO1PpQqRrSsT1UuVvWwXxY1\0";
/*                          1         2         3         4         5
*/


int strrchr1Entry(){
    int c;
    char *pstr;

    startest();


/*  Scenario 1: Verify that if successful, a pointer to the first location
                of c in string is returned.
*/
    c = '1';
    pstr = strrchr( string, c );

/*  Subtract one because relative addresses start at plus 0
*/
    checke( pstr, (string + 50 - 1) );



/*  Scenario 2: Verify that if unsuccessful, a NULL pointer is returned.
*/
    c = 'z';
    pstr = strrchr( string, c );
    checkNULL( pstr );



/*  Scenario 3: Verify that the function works correctly when c occurs
                in the first byte of string.
*/
    c = 'A';
    pstr = strrchr( string, c );
    checke( pstr, string );



/*  Scenario 4: Verify that the function works correctly when c occurs
                in the last byte of string.
*/
    c = '1';
    pstr = strrchr( string, c );

/*  Subtract one because relative addresses start at plus 0
*/
    checke( pstr, (string + 50 - 1) );



    finish();
}
