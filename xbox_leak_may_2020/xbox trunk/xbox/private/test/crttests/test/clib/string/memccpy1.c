
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memccpy1.c
                     memccpy1
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
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char src[60] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYy";
/*                       1         2         3         4         5
*/


int memccpy1Entry(){
    int c;
    unsigned int count;
    char dest[60];
    char *pdest;

    startest();


/*  Scenario 1: Verify that if the char c is copied, a pointer to the byte
                in dest that immediately follows the char is returned.
*/
    c = 'j';
    count = 60;
    pdest = _memccpy( dest, src, c, count );
    checke( pdest, (dest + 20) );



/*  Scenario 2: Verify that if the char c is not copied, a NULL pointer
                is returned.
*/
    c = 'z';
    count = 60;
    pdest = _memccpy( dest, src, c, count );
    checkNULL( pdest );



/*  Scenario 3: Verify that the function works correctly when c occurs
                in the first byte of src.
*/
    c = 'A';
    count = 60;

/*  clear dest string
*/
    pdest = dest;
    *pdest = '\0';

    pdest = _memccpy( dest, src, c, count );
    checke( pdest, (dest + 1) );
    checke( dest[0], src[0] );



/*  Scenario 4: Verify that the function works correctly when c occurs
                in the first byte of src and count equals 1.
*/
    c = 'A';
    count = 1;

/*  clear dest string
*/
    pdest = dest;
    *pdest = '\0';

    pdest = _memccpy( dest, src, c, count );
    checke( pdest, (dest + 1) );
    checke( dest[0], src[0] );



/*  Scenario 5: Verify that the function works correctly when c occurs
                in the last byte of src.
*/
    c = 'y';
    count = 60;
    pdest = _memccpy( dest, src, c, count );
    checke( pdest, (dest + 50) );

/*  check that all characters got copied
*/
    for (c = 0; c < 50; c++)
        checke( dest[c], src[c] );


    finish();
}
