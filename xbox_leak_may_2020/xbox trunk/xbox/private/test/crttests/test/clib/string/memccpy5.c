
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memccpy5.c
                     memccpy5
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _memccpy() function.
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
 [ 0] 01-May-92    edv            created


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


int memccpy5Entry(){
    int c;
    unsigned int count, i, buf_size;
    unsigned char *src, *dest;
    /* size_t contig; */

    startest();


/*  Scenario 11: Verify that dest and src can be as large as
         LARGEST using any string model.
*/

       buf_size = LARGEST;

       src = malloc( buf_size );
       if (src == NULL)
       {
          faill();
          exit( 1 );
       }

       dest = malloc( buf_size );
       if (dest == NULL)
       {
          faill();
          exit( 1 );
       }

       memset( src, '+', buf_size );
       memset( dest, '-', buf_size );
       count = buf_size;
       c = 'C';
       _memccpy( dest, src, c, count );

       for (i = 0; i < count; i++)
           checke( dest[i], src[i] );



    free( src );
    free( dest );


    finish();
}
