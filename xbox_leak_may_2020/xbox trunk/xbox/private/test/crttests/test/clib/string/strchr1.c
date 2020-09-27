
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strchr1.c
                     strchr1
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
 [ 0] 06-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char string[50] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYy";
/*                       1         2         3         4         5
*/


int strchr1Entry(){
    int c;
    char *pstr;

    startest();


/*  Scenario 1: Verify that if successful, a pointer to the first location
                of c in string is returned.
*/
    c = 'o';
    pstr = strchr( string, c );

/*  Subtract one because relative addresses start at plus 0
*/
    checke( pstr, (string + 30 - 1) );



/*  Scenario 2: Verify that if unsuccessful, a NULL pointer is returned.
*/
    c = 'z';
    pstr = strchr( string, c );
    checkNULL( pstr );



/*  Scenario 3: Verify that the function works correctly when c occurs
                in the first byte of string.
*/
    c = 'A';
    pstr = strchr( string, c );
    checke( pstr, string );



/*  Scenario 4: Verify that the function works correctly when c occurs
                in the last byte of string.
*/
    c = 'y';
    pstr = strchr( string, c );

/*  Subtract one because relative addresses start at plus 0
*/
    checke( pstr, (string + 50 - 1) );



    finish();
}
