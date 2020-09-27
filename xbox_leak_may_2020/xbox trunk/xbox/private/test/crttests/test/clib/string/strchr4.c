
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:          cl strchr4.c
             strchr4
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _fstrchr() function.
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
 [ 1] 28-Jan-98    a-hemalk       64bit warning (C4311) fix


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


int strchr4Entry(){
    int c;
    unsigned int str_size;
    unsigned char *pstr1, *pstr2;

    startest();


/*  Scenario 7: Verify that str can be as large as LARGEST
                using any memory model.
*/

    str_size = LARGEST;

    pstr1 = malloc( str_size );
    if (pstr1 == NULL)
    {
       faill();
       exit( 1 );
    }
    memset( pstr1, '+', str_size );
    pstr1[str_size - 1] = '\0';

    pstr1[str_size - 100] = c = 'X';
    pstr2 = strchr( pstr1, c );

    checknNULL(pstr2);

    free( pstr1 );


    finish();
}
