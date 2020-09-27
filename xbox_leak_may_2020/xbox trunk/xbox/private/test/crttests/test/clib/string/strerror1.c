
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strerro1.c
                     strerro1
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the strerror() function.
OS Version:          REAL/PROT/ALL
CPU:                 8086
Dependencies:        NONE
Environment Vars:    NONE
Verification Method: COMPILE/EXECUTE/COMPARE
Notes:               Must include STRING.H and ERRNO.H
                     Tests decribed in the testplan STRING.DOC
Products:            C800
Test Type:       DEPTH

Revision History:
 Date              emailname      description
---------------------------------------------------------------------------
 [ 0] 27-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <errno.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

static char error0[9] = "No error";
#define ERRLEN1 9

static char errorx[14] = "Unknown error";
#define ERRLEN2 14

int strerror1Entry(){
    int i, j;
    char *errorstr;


    startest();

/*  Scenario 1: Verify that the function returns a pointer to the string
                containing the error message.
*/

    errorstr = strerror(0);

    for (i = 0; i < ERRLEN1; i++)
        checke( errorstr[i], error0[i] );



/*  Scenario 2: Verify that the function returns the correct string for each
                error number.
*/

    for (i = 0; i < sys_nerr; i++)
    {
        errorstr = strerror( i );
        for (j = 0; j < (int) strlen( sys_errlist[i] ); j++)
            checke( errorstr[j], sys_errlist[i][j] );
    }



/*  Scenario 3: Verify that the function works correctly when illegal error
                numbers are passed.
*/

    for (i = 90; i < 110; i++)
    {
        errorstr = strerror( i );
        for (j = 0; j < ERRLEN2; j++)
            checke( errorstr[j], errorx[j] );
    }



    finish();
}
