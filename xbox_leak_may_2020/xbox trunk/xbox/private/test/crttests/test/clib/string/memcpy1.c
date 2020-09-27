
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memcpy1.c
                     memcpy1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the memcpy() function.
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
#include <test.h>

static char test[] =__FILE__;
static int perfect;


static char src[30]  = "A1B2C3D4E5F6G7H8I9J0KkLlMmNnOo";

static char bsl1[30] = "A1B2C3D4E5A1B2C3D4E5A1B2C3D4E5";
static char bsl2[30] = "F6G7H8I9J0A1B2C3D4E5A1B2C3D4E5";



int memcpy1Entry(){
    unsigned int count;
    char dest[30];
    char *pdest;



    startest();


/*  Scenario 1: Verify that the function returns a pointer to dest.
*/

    count = sizeof( src );
    pdest = memcpy( dest, src, count );
    checke( pdest, dest );




/*  Scenario 2: Verify that the function works correctly when
                dest overlaps src.
*/

/*  Invalid scenario: undefined behaviour when overlap

    for (i = 0; i < sizeof( dest ); i++)
        dest[i] = src[i];
    count = 20;

    pdest = memcpy( dest + 10, dest, count );

    for (i = 0; i < sizeof( dest ); i++)
        checke( dest[i], bsl1[i]);
*/


/*  Scenario 3: Verify that the function works correctly when
                src overlaps dest.
*/

/*  Invalid scenario: undefined behaviour when overlap

    for (i = 0; i < count; i++)
        dest[i] = src[i];
    count = 20;

    pdest = memcpy( dest, dest + 10, count );

    for (i = 0; i < sizeof( dest ); i++)
        checke( dest[i], bsl2[i] );
*/



    finish();
}
