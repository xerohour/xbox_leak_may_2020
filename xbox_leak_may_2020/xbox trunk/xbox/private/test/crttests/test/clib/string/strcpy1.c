
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strcpy1.c
                     strcpy1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strcpy() function.
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
 [ 0] 08-May-92    edv            created
 [ 1] 28-Jan-98    a-hemalk       64bit warning (C4311) fix


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char string1[10] = "abc";
static char string2[10] = "def";

static char bsl1[4] = "ABC\0";
static char bsl2[2] = "X\0";



int strcpy1Entry(){
    int i;
    char *pstr;



    startest();


/*  Scenario 1: Verify that the function returns a pointer to string1.
*/

    pstr = strcpy( string1, string2 );
    checke( pstr, string1 );




/*  Scenario 2: Verify that the function works correctly when
                string1 is NULL.
*/

    string1[0] = '\0';
    pstr = strcpy( string1, string2 );

    checke( pstr, string1 );
    
    for (i = 0; i < sizeof( string2 ); i++)    /* Initialize string1 */
        checke( pstr[i], string2[i]);




/*  Scenario 3: Verify that the function works correctly when
                string2 is NULL.
*/

    string2[0] = '\0';

    string1[0] = 'A';
    string1[1] = 'B';
    string1[2] = 'C';
    string1[3] = '\0';

    pstr = strcpy( string1, string2 );

    //checke( pstr[0], '\0' );
    checkNULL(pstr[0]);




/*  Scenario 4: Verify that the function works correctly when both
                string1 and string2 are NULL.
*/

    string1[0] = '\0';
    string2[0] = '\0';

    pstr = strcpy( string1, string2 );

    //checke( pstr[0], '\0' );
    checkNULL(pstr[0]);




/*  Scenario 5: Verify that the function works correctly when
                string2 is one byte.
*/


    string1[0] = 'A';
    string1[1] = 'B';
    string1[2] = 'C';
    string1[3] = 'D';
    string1[4] = 'E';
    string1[5] = '\0';

    string2[0] = 'X';
    string2[1] = '\0';

    pstr = strcpy( string1, string2 );

    for (i = 0; i < 2; i++)
        checke( pstr[i], bsl2[i] );



    finish();
}
