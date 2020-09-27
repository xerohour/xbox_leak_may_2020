
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strcspn3.c
                     strcspn3
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

static char string1a[200] = { "`1234567890-=~!@#$%^&*()_+"
                       "qwertyuiop[]QWERTYUIOP{}|\\"
                       "asdfghjkl;'ASDFGHJKL:\""
                       "zxcvbnm,./ZXCVBNM<>? \0" };
static char string1b[250] =  "`1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./ \0";
static char string1c[300] =  "1234567890abcdefgHIJKLMNOPqrstuvWXYZ\0";
static char string1d[100] =  "abcdefgHIJKLMNOPqrstuvWXYZ0123456789\0";
static char string1e[120] =  "abcdefgHIJKLMNOPqrstuvWXYZ0123456789./?\0";
static char string1f[210] =  "1234567890ABCDEFGHI";



static char string2a[155] = { "QWERTYUIOP{}|qwertyuiop[]"
                       "ASDFGHJKL:\"asdfghjkl;'"
                       "ZXCVBNM<>? zxcvbnm,./"
                       "~!@#$%^&*()_+`1234567890-=\0" };
static char string2b[555] =  "~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?\\\0";
static char string2c[111] =  "abcdefgHIJKLMNOPqrstuvWXYZ!@#$%^&*()_+-=1234567890\0";
static char string2d[333] =  "!@#$%^&*()_+-=9\0";
static char string2e[151] =  "&*<t>_+-=9\0";
static char string2f[351] =  "JKAAAAAAAAALMNOAAAAAAAAAPQRAAAAAAAASTUAAAAVWXYZAAA\0";


#define RESULT1    0
#define RESULT2   47
#define RESULT3    0
#define RESULT4   35
#define RESULT5   19
#define RESULT6   10


int strcspn3Entry(){
    int index;


    startest();



/*  Scenario 9: Verify that the function works correctly when both string1
                and string2 are large and equal.
*/
    index = (int)strcspn( string1a, string2a );
    checke( index, RESULT1 );



/*  Scenario 10: Verify that the function works correctly when both string1
                 and string2 are large but mutually exclusive.
*/
    index = (int)strcspn( string1b, string2b );
    checke( index, RESULT2 );



/*  Scenario 11: Verify that the function works correctly when string2 is
                 a superset of string1.
*/
    index = (int)strcspn( string1c, string2c );
    checke( index, RESULT3 );



/*  Scenario 12: Verify that the function works correctly when a character
                 in string2 occurs in the last character of string1.
*/
    index = (int)strcspn( string1d, string2d );
    checke( index, RESULT4 );



/*  Scenario 13: Verify that the function works correctly when a character
                 in string2 occurs in the middle of string1.
*/
    index = (int)strcspn( string1e, string2e );
    checke( index, RESULT5 );



/*  Scenario 14: Verify that the function works correctly when string2
                 is larger than string1 but only because of a repetitve
                 character.
*/
    index = (int)strcspn( string1f, string2f );
    checke( index, RESULT6 );




    finish();
}
