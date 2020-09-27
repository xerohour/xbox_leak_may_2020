
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strnicm4.c
                     strnicm4
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strnicmp() function.
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


int strnicm4Entry(){
    int outcome;
    unsigned int str_size;
    unsigned char *string1, *string2;
    size_t count;


    startest();



/*  Scenario 10: Verify that string1 and string2 can be as large as
         LARGEST using any memory model.
*/

       str_size = LARGEST;

       string1 = malloc( str_size );
       if (string1 == NULL)
       {
          faill();
          exit( 1 );
       }

       string2 = malloc( str_size );
       if (string2 == NULL)
       {
          faill();
          exit( 1 );
       }

       memset( string1, 'a', str_size );
       memset( string2, 'a', str_size );
       string1[str_size - 1] = string2[str_size - 1] = '\0';
       count = str_size;

       outcome = _strnicmp( string1, string2, count );
       checke( (outcome == 0), 1 );


       string1[str_size - 2] = 'b';
       outcome = _strnicmp( string1, string2, count );
       checke( (outcome > 0), 1 );
       string1[str_size - 2] = 'a';


       string1[1] = '1';
       outcome = _strnicmp( string1, string2, count );
       checke( (outcome < 0), 1 );



    free( string1 );
    free( string2 );


    finish();
}
