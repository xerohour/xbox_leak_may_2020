
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strset1.c
                     strset1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strset() function.
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


static unsigned char string1[6] = "ABCDE\0";
static unsigned char string2[5] = "\0\0\0\0\0";
static unsigned char string3[2] = "1\0";
static unsigned char string4[3] = "22\0";


int strset1Entry(){
    int i;
    unsigned char *pstr;

    startest();



/*  Scenario 1: Verify that the function returns a pointer to the
                altered string.
*/

    pstr = _strset( string1, '%' );

    checke( pstr, string1 );

    for (i = 0; i < (sizeof( string1 ) - 1); i++)
        checke( string1[i], '%' );

    checke( string1[i], '\0' );




/*  Scenario 2: Verify that the function works correctly when string
                is NULL.
*/

    pstr = _strset( string2, 'X' );      /* String begins with NULL,  */
                                         /*  so string is unalterred. */
    checke( pstr, string2 );

    for (i = 0; i < (sizeof( string2 ) ); i++)
        checke( string2[i], '\0' );




/*  Scenario 3: Verify that the function works correctly when string
                can hold only 1 character.
*/

    pstr = _strset( string3, '*' );

    checke( pstr, string3 );

    checke( pstr[0], '*' );

    checke( pstr[1], '\0' );




/*  Scenario 4: Verify that the function works correctly when string
                can hold only 2 characters.
*/

    pstr = _strset( string4, '$' );

    checke( pstr, string4 );

    checke( pstr[0], '$' );

    checke( pstr[1], '$' );

    checke( pstr[2], '\0' );




    finish();
}
