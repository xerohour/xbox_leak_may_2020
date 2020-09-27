
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl memmove1.c
                     memmove1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the memmove() function.
OS Version:          REAL/PROT/ALL
CPU:                 8086
Dependencies:        NONE
Environment Vars:    NONE
Verification Method: COMPILE/EXECUTE/COMPARE
Notes:               Must include MEMORY.H
                     Tests described in the testplan MEMORY.DOC
Products:            C800
Test Type:       DEPTH

Revision History:
 Date              emailname      description
---------------------------------------------------------------------------
 [ 0] 22-Apr-92    edv            created


---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


static char src[45]  = "A1 B2 C3 D4 E5 F6 G7 H8 I9 J0 Kk Ll Mm Nn Oo ";

static char bsl1[45] = "F6 G7 H8 I9 J0 Kk Ll Mm Nn Oo Kk Ll Mm Nn Oo ";
static char bsl2[45] = "A1 B2 C3 D4 E5 A1 B2 C3 D4 E5 F6 G7 H8 I9 J0 ";


int memmove1Entry(){
    int i;
    size_t count;
    char dest[45];
    char *pdest;



    startest();


/*  Scenario 1: Verify that the function returns a pointer to dest.
*/

    count = sizeof( src );
    pdest = memmove( dest, src, count );
    checke( pdest, dest );



/*  Scenario 2: Verify that if src overlaps dest, the original source bytes
                in the overlapping region are copied before being overwritten.
*/

    for (i = 0; i < (int) count; i++)             /* Initialize dest */
        dest[i] = src[i];

    count = 30;

    pdest = memmove( dest, dest + 15, count );

    for (i = 0; i < sizeof( dest ); i++)
        checke( dest[i], bsl1[i] );



/*  Scenario 3: Verify that if dest overlaps src, the original source bytes
                in the overlapping region are copied before being overwritten.
*/

    for (i = 0; i < sizeof( dest ); i++)    /* Initialize dest */
        dest[i] = src[i];

    count = 30;

    pdest = memmove( dest + 15, dest, count );

    for (i = 0; i < sizeof( dest ); i++)
        checke( dest[i], bsl2[i]);



    finish();
}
