
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strtok1.c
                     strtok1
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

#define TOK_LEN 7    /* length of tokens */
#define TOK_ID  5    /* position in baseline identifying which token */

static char tokenstring[] = ";Token1,;Token2 Token3\t, Token4\nToken5 \t\n;,";
static char separators[] = "\t\n ,;";
static char baseline[] = "Token1";
static char *token;


int strtok1Entry( void ){

    int i;

    startest();


/*  Scenario 1: Verify that the function returns a pointer to string1.
*/

    token = strtok( tokenstring, separators );

    for (i = 0; i < TOK_LEN; i++)
       checke( token[i], baseline[i] );




/*  Scenario 2: Verify that the function inserts a NULL to end string1.
*/

    checke( token[TOK_LEN - 1], NULL );




/*  Scenario 3: Verify that if string1 is NULL, the function returns the
                next token in the previous token string.
*/

    baseline[TOK_ID] = '2';

    token = strtok( NULL, separators );

    for (i = 0; i < TOK_LEN; i++)
       checke( token[i], baseline[i] );


    baseline[TOK_ID] = '3';

    token = strtok( NULL, separators );

    for (i = 0; i < TOK_LEN; i++)
       checke( token[i], baseline[i] );


    baseline[TOK_ID] = '4';

    token = strtok( NULL, separators );

    for (i = 0; i < TOK_LEN; i++)
       checke( token[i], baseline[i] );


    baseline[TOK_ID] = '5';

    token = strtok( NULL, separators );

    for (i = 0; i < TOK_LEN; i++)
       checke( token[i], baseline[i] );




/*  Scenario 4: Verify that if string1 is NULL, the function returns the
                next token in the previous token string.
*/


    token = strtok( NULL, separators );

    checke( token, NULL );


    finish();
}
