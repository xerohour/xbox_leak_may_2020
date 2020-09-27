
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strpbrk3.c
                     strpbrk3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strpbrk() function.
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
 [ 0] 21-May-92    edv            created

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



int strpbrk3Entry(){
    char *pstr1;


    startest();



/*  Scenario 9: Verify that the function works correctly when both string1
                and string2 are large and equal.
*/
    pstr1 = strpbrk( string1a, string2a );
    checke( pstr1, string1a );




/*  Scenario 10: Verify that the function works correctly when both string1
                 and string2 are large but mutually exclusive.
*/
    pstr1 = strpbrk( string1b, string2b );
    checkNULL( pstr1 );




/*  Scenario 11: Verify that the function works correctly when string2 is
                 a superset of string1.
*/
    pstr1 = strpbrk( string1c, string2c );
    checke( pstr1, string1c );




/*  Scenario 12: Verify that the function works correctly when a character
                 in string2 occurs in the last character of string1.
*/
    pstr1 = strpbrk( string1d, string2d );
    checke( pstr1, (string1d + 36 - 1) );




/*  Scenario 13: Verify that the function works correctly when a character
                 in string2 occurs in the middle of string1.
*/
    pstr1 = strpbrk( string1e, string2e );
    checke( pstr1, (string1e + 20 - 1) );




/*  Scenario 14: Verify that the function works correctly when string2
                 is larger than string1 but only because of a repetitve
                 character.
*/
    pstr1 = strpbrk( string1f, string2f );
    checke( pstr1, (string1f + 11 - 1) );





    finish();
}
