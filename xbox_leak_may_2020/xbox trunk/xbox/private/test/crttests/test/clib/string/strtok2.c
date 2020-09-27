
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strtok2.c
                     strtok2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strtok() function.
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
 [ 0] 05-May-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

static char big_tokenstr[] = "Token01!Token02@Token03#Token04$Token05%Token06^"
                      "Token07&Token08*Token09(Token10)Token11-Token12_"
                      "Token13=Token14+Token15[Token16]Token17{Token18}"
                      "Token19|Token20:Token21;Token22<Token23>Token24,"
                      "Token25.Token26/Token27?Token28 Token29\tToken30\n";

static char separators[] =  "!@#$%^&*()-_=+[]{}|:;<>,./? \t\n";

static char tokenstring1[] = " * Token01 * Token02 * ";
static char tokenstring2[] = "         Token01           Token02             ";
static char separator[] = " ";
static char baseline[] = "Token01";
static char null_str[] = "";
static char *token;


static void check_token( char *str1, char *str2 )
{
    int i;

    for (i = 0; i < 8; i++)
       checke( str1[i], str2[i] );
}


int strtok2Entry( void ){

    int i;

    startest();


/*  Scenario 5: Verify that the function works correctly when
                string1 is NULL.
*/

    token = strtok( null_str, separators );
    checkNULL( token );




/*  Scenario 6: Verify that the function works correctly when
                string2 is NULL.
*/

    token = strtok( tokenstring1, "");

    check_token( token, " * Token01 * Token02 * " );




/*  Scenario 7: Verify that the function works correctly when
                string2 is 1 byte long.
*/

    token = strtok( tokenstring2, " " );
    check_token( token, "Token01" );


    token = strtok( NULL, " " );
    check_token( token, "Token02" );




/*  Scenario 8: Verify that the function works correctly when
                string1 is large.
*/

    token = strtok( big_tokenstr, separators );
    check_token( token, "Token01" );

    for ( i = 1; i < 6; i++ )
        token = strtok( NULL, separators );
    check_token( token, "Token06" );


    for ( i = 1; i < 7; i++ )
        token = strtok( NULL, separators );
    check_token( token, "Token12" );


    for ( i = 1; i < 7; i++ )
        token = strtok( NULL, separators );
    check_token( token, "Token18" );


    for ( i = 1; i < 7; i++ )
        token = strtok( NULL, separators );
    check_token( token, "Token24" );


    for ( i = 1; i < 7; i++ )
        token = strtok( NULL, separators );
    check_token( token, "Token30" );

    token = strtok( NULL, separators );
    checkNULL( token );


    finish();
}
