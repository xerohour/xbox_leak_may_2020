
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strerro2.c
                     strerro2
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

static char text1[8] = "Testing\0";
static char maxtext[95] = { "1234567890123456789012345678901234567890"
                     "1234567890123456789012345678901234567890"
                     "12345678901234\0"
                   };

static char baseln1[27] = "Testing: Arg list too long\n";
static char baseln2[122] = { "1234567890123456789012345678901234567890"
                      "1234567890123456789012345678901234567890"
                      "12345678901234: No such file or directory\n"
                    };
static char baseln3[12] = "File exists\n";

#define ERRLEN1  27
#define ERRLEN2 122
#define ERRLEN3  12


int strerror2Entry(){
    int i, j;
    char *errorstr;


    startest();



/*  Scenario 1: Verify that the function returns a pointer to a string
                containing (in order) the string passed, a colon, a space,
                the system error message and a newline character.
*/

    errno = 7;
    errorstr = _strerror( text1 );

    for (i = 0; i < ERRLEN1; i++)
        checke( errorstr[i], baseln1[i] );




/*  Scenario 2: Verify that string can be up to 94 characters.
*/

    errno = 2;
    errorstr = _strerror( maxtext );

    for (i = 0; i < ERRLEN2; i++)
        checke( errorstr[i], baseln2[i] );




/*  Scenario 3: Verify that if string is NULL, the function returns a
                pointer to a string containing the system error message
                for the last library call that produced an error.
*/

    errno = 17;
    errorstr = _strerror( NULL );

    for (i = 0; i < ERRLEN3; i++)
        checke( errorstr[i], baseln3[i] );




/*  Scenario 4: Verify that the function returns the correct error string
                for each error in sys_errlist.
*/

    for (i = 0; i < sys_nerr; i++)
    {
        errno = i;
        errorstr = _strerror( NULL );
        for (j = 0; j < (int) strlen( sys_errlist[i] ); j++)
            checke( errorstr[j], sys_errlist[i][j] );
    };



    finish();
}
