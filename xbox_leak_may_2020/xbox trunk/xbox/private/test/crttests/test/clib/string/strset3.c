
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strset3.c
                     strset3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strset() function.
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
 [ 0] 13-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

#define DELTA    100
#define LARGEST  (USHRT_MAX - DELTA)


int strset3Entry(){
    int c, d, i;
    unsigned int str_size;
    unsigned char *pstr1, *pstr2;



    startest();



/*  Scenario 7: Verify that string1 can be as large as LARGEST
                using any memory model.
*/


       str_size = LARGEST;

       pstr1 = malloc( str_size );

       if (pstr1 == NULL)
       {
          faill();
          exit( 1 );
       }

       memset( pstr1, ' ', str_size );       /* Clear the string.      */

       d = '\0';
       pstr1[str_size - 1] = d;              /* Terminate with NULL.   */

       c = '+';
       pstr2 = _strset( pstr1, c );
                                             /* Check the pointers.    */
       checke( pstr2, pstr1 );

       for (i = 0; i < (int) (str_size - 1); i++)  /* Check each char.       */
             checke( pstr2[i], c );

       checke( pstr2[i], d );                /* Check NULL terminator. */


    free( pstr1 );


    finish();
}
