
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strncpy3.c
                     strncpy3
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
#include <malloc.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

#define DELTA    2000
#define LARGEST (SHRT_MAX - DELTA)


int strncpy3Entry(){
    unsigned int count, i, buf_size;
    unsigned char *string2, *string1;

    startest();


/*  Scenario 8: Verify that string1 and string2 can be as large as
        LARGEST using any STRING model.
*/


       buf_size = LARGEST;
       count = buf_size;


       string2 = malloc( buf_size );
       if (string2 == NULL)
       {
          faill();
          exit( 1 );
       }

       string1 = malloc( buf_size );
       if (string1 == NULL)
       {
          faill();
          exit( 1 );
       }

       memset( string2, '+', buf_size - 1 );
       memset( string1, '-', buf_size - 1 );
       string1[buf_size - 1] = string2[buf_size - 1] = '\0';

       strncpy( string1, string2, count );

       for (i = 0; i < count; i++)
           checke( string1[i], string2[i] );


    free( string2 );
    free( string1 );



    finish();
}
