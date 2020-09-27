
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memchr1.c
                     memchr1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the memchr() function.
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
 [ 0] 01-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char buf[50] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYy";
/*                       1         2         3         4         5
*/


int memchr1Entry(){
    int c;
/*    size_t count; */
    char *pbuf;

    startest();


/*  Scenario 1: Verify that if successful, a pointer to the first location
                of c in buf is returned.
*/
    c = 'o';
    pbuf = memchr( buf, c, sizeof( buf ) );

/*  Subtract one because relative addresses start at plus 0
*/
    checke( pbuf, (buf + 30 - 1) );



/*  Scenario 2: Verify that if unsuccessful, a NULL pointer is returned.
*/
    c = 'z';
    pbuf = memchr( buf, c, sizeof( buf ) );
    checkNULL( pbuf );


/*  Scenario 3: Verify that the function works correctly when c occurs
                in the first byte of buf.
*/
    c = 'A';
    pbuf = memchr( buf, c, sizeof( buf ) );
    checke( pbuf, buf );



/*  Scenario 4: Verify that the function works correctly when c occurs
                in the last byte of buf.
*/
    c = 'y';
    pbuf = memchr( buf, c, sizeof( buf ) );

/*  Subtract one because relative addresses start at plus 0
*/
    checke( pbuf, (buf + 50 - 1) );



    finish();
}
