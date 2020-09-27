
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strrev3.c
                     strrev3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strrev() function.
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
 [ 0] 14-May-92    edv            created

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
#define LARGEST  (USHRT_MAX - DELTA)



static unsigned char strinit[65] = { "0123456789"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "!@#$%^&*()_+-={}|[]:;'/?,.<>\0"
                            };

static unsigned char baseline[65] = { "><.,?/';:][|}{=-+_)(*&^%$#@!"
                               "ZYXWVUTSRQPONMLKJIHGFEDCBA"
                               "9876543210\0"
                              };

int strrev3Entry(){

    int i;
    unsigned int str_size;
    unsigned char *pstr1, *pstr2;

    startest();


/*  Scenario 7: Verify that string can be almost as large as
        LARGEST using any memory model.
*/

       str_size = LARGEST;

       pstr1 = malloc( str_size );

       if (pstr1 == NULL)
       {
          faill();
          exit( 1 );
       }

       for (i = 0; i < sizeof( strinit ); i++)   /* Initialize string */
           pstr1[i] = strinit[i];

       pstr2 = _strrev( pstr1 );

       checke( pstr2, pstr1 );

       for (i = 0; i < sizeof( baseline ); i++)
           checke( pstr2[i], baseline[i] );



    free( pstr1 );


    finish();
}
