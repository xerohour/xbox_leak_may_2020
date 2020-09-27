
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strncmp3.c
                     strncmp3
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strncmp() function.
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
 [ 0] 11-May-92    edv            created

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


int strncmp3Entry(){
    int outcome;
    unsigned int str_size;
    unsigned char *str1, *str2;
    size_t count;


    startest();


/*  Scenario 6: Verify that str1 and str2 can be as large as
        LARGEST using any memory model.
*/

       str_size = LARGEST;

       str1 = malloc( str_size );
       if (str1 == NULL)
       {
          faill();
          exit( 1 );
       }

       str2 = malloc( str_size );
       if (str2 == NULL)
       {
          faill();
          exit( 1 );
       }

       memset( str1, 'a', str_size );
       memset( str2, 'a', str_size );
       count = str_size;

       outcome = strncmp( str1, str2, count );
       checke( (outcome == 0), 1 );


       str1[str_size - 1] = 'b';
       outcome = strncmp( str1, str2, count );
       checke( (outcome > 0), 1 );
       str1[str_size - 1] = 'a';


       str1[1] = 'X';
       outcome = strncmp( str1, str2, count );
       checke( (outcome < 0), 1 );




    free( str1 );
    free( str2 );


    finish();
}
