
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strupr1.c
                     strupr1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strupr() function.
OS Version:          REAL/PROT/ALL
CPU:                 8086
Dependencies:        NONE
Environment Vars:    NONE
Verification Method: COMPILE/EXECUTE/COMPARE
Notes:               must include STRING.H
                     Tests described in the testplan STRING.DOC
Products:            C800
Test Type:       DEPTH

Revision History:
 Date              emailname      description
---------------------------------------------------------------------------
 [ 0] 19-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

static char string1[30] = "this string is all same case\0";
static char string2[30] = "THIS STRING IS ALL SAME CASE\0";

int strupr1Entry(){
    char *p, c[2], x[2], z;
    int i;


    startest();



/*  Scenario 1: Verify that the function returns a pointer to the
                converted string.
*/

    p = _strupr( string1 );
    checke( p, string1 );

    for (i = 0; i < sizeof( string1 ); i++)
        checke( string1[i], string2[i] );





/*  Scenario 2: Loop through all the lowercase letters and verify
                that they get converted to uppercase.
*/

    c[1] = x[1] = '\0';          /*  NULL terminate the strings  */


    for (i = 'a', x[0] = 'A'; i <= 'z'; i++, x[0]++)
    {
        c[0] = i;
        _strupr( c );
        checke( c[0], x[0] );
    }




/*  Scenario 3: Loop through the remainder of the ASCII characters
                and verify that the characters remain unchanged.
*/

    for (i = 0X00; i <= 0X60; i++)
    {
        c[0] = z = i;
        _strupr( c );
        checke( c[0], z );
    }


    for (i = 0X7B; i <= 0X7F; i++)
    {
        c[0] = z = i;
        _strupr( c );
        checke( c[0], z );
    }




    finish();
}
