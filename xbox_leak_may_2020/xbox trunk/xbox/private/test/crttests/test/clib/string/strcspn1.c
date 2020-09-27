
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strcspn1.c
                     strcspn1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strcspn() function.
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
 [ 0] 22-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char string1a[128] = "12345678901234567890123456789012\0";
static char string1b[32]  = "AabCcDdEeFfghI\0";
static char string1c[16]  = "\0";
static char string1d[64]  = "String2d is null - 21\0";
static char string1e[8]   = "\0";


static char string2a[55] = "Mutually Exclusive String\0";
static char string2b[75] = "Last Char Starts string1b xxxxx12345xxxxxxA\0";
static char string2c[27] = "String1c is null\0";
static char string2d[15] = "\0";
static char string2e[9]  = "\0";


#define RESULT1   32
#define RESULT2    0
#define RESULT3    0
#define RESULT4   21
#define RESULT5    0


int strcspn1Entry(){
    int  index;



    startest();



/*  Scenario 1: Verify that the function works correctly when string1
                and string2 are mutually exclusive.  The return value
                should be equal to the length of string1.
*/

    index = (int)strcspn( string1a, string2a );
    checke( index, RESULT1 );



/*  Scenario 2: Verify that the function returns zero when string1
                begins with a characters in string2.
*/
    index = (int)strcspn( string1b, string2b );
    checke( index, RESULT2 );



/*  Scenario 3: Verify that the function works correctly when string1
                is NULL.  Should return zero.
*/
    index = (int)strcspn( string1c, string2c );
    checke( index, RESULT3 );



/*  Scenario 4: Verify that the function works correctly when string2
                is NULL.  Should return length of string1.
*/
    index = (int)strcspn( string1d, string2d );
    checke( index, RESULT4 );



/*  Scenario 5: Verify that the function works correctly when both string1
                and string2 are NULL.
*/
    index = (int)strcspn( string1e, string2e );
    checke( index, 0 );




    finish();
}
