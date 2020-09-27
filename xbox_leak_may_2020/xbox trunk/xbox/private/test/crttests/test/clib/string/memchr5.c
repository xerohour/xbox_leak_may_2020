
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:          cl memchr5.c
             memchr5
Switches:            NONE
Hard/Soft Needs:
Abstract:        This test verifies the memchr() function.
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


int memchr5Entry(){
    int c;
    unsigned int buf_size;
    unsigned char *pbuf1, *pbuf2;

    startest();


/*  Scenario 10: Verify that buf can be as large as LARGEST
                 using any STRING model.
*/

    buf_size = LARGEST;

    pbuf1 = malloc( buf_size );
    if (pbuf1 == NULL)
    {
      faill();
      exit( 1 );
    }
    memset( pbuf1, '+', buf_size );
    pbuf1[5] = c = 'X';
    pbuf2 = memchr( pbuf1, c, buf_size );
    checke( pbuf2, pbuf1 + 5 );


    free( pbuf1 );


    finish();
}
