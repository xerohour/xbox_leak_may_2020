
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strcpy2.c
                     strcpy2
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
 [ 0] 11-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char string1[100];

static char str2A[60]  = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char str2B[100] = "-:-{}-[]-()-<>-:-{}-[]-()-<>-:-{}-[]-()-<>-:-{}-[]-+";
static char str2C[32] = "123456789012345678901234567890";

static char bsl1[52] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char bsl2[52] = "-:-{}-[]-()-<>-:-{}-[]-()-<>-:-{}-[]-()-<>-:-{}-[]-+";
static char bsl3[30] = "123456789012345678901234567890";



int strcpy2Entry(){
    int i;
    char *pstr;



    startest();



/*  Scenario 6: Verify that the function works correctly with
                larger strings.
*/

    string1[0] = '\0';
    pstr = strcpy( string1, str2A );

    checke( pstr, string1 );

    for (i = 0; i < sizeof( bsl1 ); i++)
        checke( pstr[i], bsl1[i] );





    string1[0] = '\0';
    pstr = strcpy( string1, str2B );

    checke( pstr, string1 );

    for (i = 0; i < sizeof( bsl2 ); i++)
        checke( pstr[i], bsl2[i] );





    string1[0] = '\0';
    pstr = strcpy( string1, str2C );

    checke( pstr, string1 );

    for (i = 0; i < sizeof( bsl3 ); i++)
        checke( pstr[i], bsl3[i] );






    string1[0] = '\0';
    memset( str2B, '*', sizeof( str2B ) );
    str2B[sizeof( str2B ) - 1] = '\0';

    pstr = strcpy( string1, str2B );

    checke( pstr, string1 );

    for (i = 0; i < sizeof( str2B ); i++)
        checke( pstr[i], str2B[i] );



    finish();

}
