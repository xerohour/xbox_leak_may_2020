
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:          cl memset4.c
             memset4
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _fmemset() function.
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
 [ 0]  1-May-92    edv            created

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


int memset4Entry(){
    int c, d;
    unsigned int dest_size;
    unsigned char *pdest1, *pdest2;



    startest();



/*  Scenario 7: Verify that dest can be as large as LARGEST
                using any STRING model.
*/



    dest_size = LARGEST;

    pdest1 = malloc( dest_size + 1 );    /* add 1 to delimit memory */
    if (pdest1 == NULL)
    {
       faill();
       exit( 1 );
    }

    c = '+';
    d = '\0';
    pdest1[dest_size] = d;

    pdest2 = memset( pdest1, c, dest_size );

    checke( pdest2, pdest1 );
    checke( pdest1[0], c );
    checke( pdest1[dest_size - 1], c );
    checke( pdest1[dest_size], d );


    free( pdest1 );


    finish();
}
