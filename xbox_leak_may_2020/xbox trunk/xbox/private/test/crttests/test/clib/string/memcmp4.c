
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:          cl memcmp4.c
             memcmp4
Switches:            NONE
Hard/Soft Needs:
Abstract:        This test verifies the memcmp() function.
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


int memcmp4Entry(){
    int outcome;
    unsigned int buf_size;
    unsigned char *buf1, *buf2;
    size_t count;


    startest();


/*  Scenario 8: Verify that buf1 and buf2 can be as large as
         LARGEST using any STRING model.
*/

    buf_size = LARGEST;

    buf1 = malloc( buf_size );
    if (buf1 == NULL)
    {
       faill();
       exit( 1 );
    }

    buf2 = malloc( buf_size );
    if (buf2 == NULL)
    {
       faill();
       exit( 1 );
    }

    memset( buf1, 'a', buf_size );
    memset( buf2, 'a', buf_size );
    count = buf_size;

    outcome = memcmp( buf1, buf2, count );
    checke( (outcome == 0), 1 );


    buf1[buf_size - 1] = 'b';
    outcome = memcmp( buf1, buf2, count );
    checke( (outcome > 0), 1 );
    buf1[buf_size - 1] = 'a';


    buf1[1] = 'X';
    outcome = memcmp( buf1, buf2, count );
    checke( (outcome < 0), 1 );



    free( buf1 );
    free( buf2 );


    finish();
}
