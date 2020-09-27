
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strncpy1.c
                     strncpy1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strncpy() function.
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


static char string2[31]  = "A1B2C3D4E5F6G7H8I9J0KkLlMmNnOo\0";
static char string3[11]  = "1234567890\0";

static char bsl1[31] = "A1B2C3D4E5A1B2C3D4E5A1B2C3D4E5\0";
static char bsl2[31] = "F6G7H8I9J0KkLlMmNnOoKkLlMmNnOo\0";
static char bsl3[31] = "A1B2C3D4E5********************\0";
static char bsl4[31] = "1234567890\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";



int strncpy1Entry(){
    int i;
    unsigned int count;
    char string1[31];
    char *pstring1;



    startest();


/*  Scenario 1: Verify that the function returns a pointer to string1.
*/

    count = sizeof( string2 );
    pstring1 = strncpy( string1, string2, count );
    checke( pstring1, string1 );







/*  Scenario 2: Verify that the function works correctly when
                string1 overlaps string2.
*/

    count = sizeof( string1 ) - 1;
    for (i = 0; i < (int) count; i++)                /* Initialize string1 */
        string1[i] = string2[i];
    string1[count] = '\0';
    count = 20;

    pstring1 = strncpy( string1 + 10, string1, count );

    for (i = 0; i < sizeof( string1 ); i++)
        checke( string1[i], bsl1[i]);







/*  Scenario 3: Verify that the function works correctly when
                string2 overlaps string1.
*/

    count = sizeof( string1 ) - 1;
    for (i = 0; i < (int) count; i++)                /* Initialize string1 */
        string1[i] = string2[i];
    string1[count] = '\0';
    count = 20;

    pstring1 = strncpy( string1, string1 + 10, count );

    for (i = 0; i < sizeof( string1 ); i++)
        checke( string1[i], bsl2[i] );






/*  Scenario 4: Verify that when count is less than the length of
                string2, a null is not appended.
*/

    count = sizeof( string1 ) - 1;
    for (i = 0; i < (int) count; i++)                /* Initialize string1 */
        string1[i] = '*';
    string1[count] = '\0';
    count = 10;

    pstring1 = strncpy( string1, string2, count );

    for (i = 0; i < sizeof( string1 ); i++)
        checke( string1[i], bsl3[i] );






/*  Scenario 5: Verify that when count is greater than the length of
                string2, string1 gets padded with NULL.
*/

    count = sizeof( string1 ) - 1;
    for (i = 0; i < (int) count; i++)                /* Initialize string1 */
        string1[i] = '#';
    string1[count] = '\0';
    count = 30;

    pstring1 = strncpy( string1, string3, count );

    for (i = 0; i < sizeof( string1 ); i++)
        checke( string1[i], bsl4[i] );




    finish();
}
