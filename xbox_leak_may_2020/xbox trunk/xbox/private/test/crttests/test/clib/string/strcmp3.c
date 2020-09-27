
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:          cl strcmp3.c
             strcmp3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _fstrcmp() function.
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

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

#define DELTA    2000
#define LARGEST (SHRT_MAX - DELTA)


int strcmp3Entry(){
    int outcome;
    unsigned int string_size;
    unsigned char *string1, *string2;


    startest();


/*  Scenario 5: Verify that string1 and string2 can be
        as large as LARGEST.
*/

    string_size = LARGEST;

    string1 = malloc( string_size );
    if (string1 == NULL)
    {
       faill();
       exit( 1 );
    }

    string2 = malloc( string_size );
    if (string2 == NULL)
    {
       faill();
       exit( 1 );
    }

    memset( string1, 'a', string_size );
    string1[string_size - 1] = '\0';

    memset( string2, 'a', string_size );
    string2[string_size - 1] = '\0';



    outcome = -1;
    outcome = strcmp( string1, string2 );
    checke( (outcome == 0), 1 );



    string1[string_size - 2] = 'b';
    outcome = 0;
    outcome = strcmp( string1, string2 );
    checke( (outcome > 0), 1 );
    string1[string_size - 2] = 'a';


    string2[string_size - 2] = 'd';
    outcome = -1;
    outcome = strcmp( string1, string2 );
    checke( (outcome < 0), 1 );




    free( string1 );
    free( string2 );


    finish();
}
