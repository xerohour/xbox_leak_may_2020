
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memccpy2.c
                     memccpy2
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
#include <limits.h>
#include <test.h>

static char test[] =__FILE__;
static int perfect;


static unsigned char src[20] = "abcdefghijklmnopqrst";
static unsigned char baseline[20] = "abcdefghij";
static unsigned char dest[20];


int memccpy2Entry(){
    int c, i;
    unsigned int count = 20;

    startest();

/*  Scenario 6: Verify that c can be any value from 0 to UCHAR_MAX,
                and will be recognized as a delimiter of the string
                to be copied.
*/

    for (c = 0; c < 'a'; c++) {
        src[9] = baseline[9] = c;
        _memccpy( dest, src, c, count );
        for (i = 0; i < (int) count; i++)
	  {
	    if (dest[i] != baseline[i])
	      {
		checke( dest[i], baseline[i] );
		printf("error with delimiter %x\n", (unsigned int)c);
	      };
	  };
    }


/*  Change string in src and baseline because _memccpy will halt when
    the first 'c' is copied.
*/
    strcpy( src, "ABCDEFGHIJKLMNOPQRST" );
    strcpy( baseline, "ABCDEFGHIJ" );

    for (c = 'a'; c <= UCHAR_MAX; c++) {
        src[9] = baseline[9] = c;
        _memccpy( dest, src, c, count );
        for (i = 0; i < (int) count; i++)
	  {
	    if (dest[i] != baseline[i])
	      {
            checke( dest[i], baseline[i] );
	    printf("error with delimiter %x\n", (unsigned int)c);
	      };
	  };
    }


    finish();
}
