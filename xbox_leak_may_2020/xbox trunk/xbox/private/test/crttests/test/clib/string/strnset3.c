
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strnset3.c
                     strnset3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strnset() function.
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
 [ 1] 10-Jan-95    a-timke        Fixed memory overrun problem.

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

#define DELTA    1000
#define LARGEST  (USHRT_MAX - DELTA)


int strnset3Entry(){
    int i;
    unsigned char *pstr1, *pstr2;
    size_t str_size, count;



    startest();



/*  Scenario 6: Verify that str can be as large as LARGEST
                using any STRING model.
*/


       str_size = LARGEST;

       pstr1 = malloc( str_size );
       if (pstr1 == NULL)
       {
          faill();
          exit( 1 );
       }

       count = str_size-1;

       for (i = 0; i < (int) count; i++)      /* Initialize string */
           pstr1[i] = ' ';
       pstr1[count] = '\0';


       pstr2 = _strnset( pstr1, 'X', count );

       for (i = 0; i < (int) count; i++)
           checke( pstr2[i], 'X' );
       checke( pstr2[count], '\0' );


    free( pstr1 );


    finish();
}
