
/***************************************************************************

           Copyright (c) 1994 Microsoft Corporation


Test:           _ecvt

Abstract:       This test verifies the functionality of _ecvt().  Test
                ensures proper handling of overflow case.

===========================================================================
Syntax

===========================================================================
Priority:               1
Products:               WIN NT


Revision History:

    Date        emailname       description
----------------------------------------------------------------------------
    05-06-94    lhanson         created
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "test.h"

static char test[] = __FILE__;
static int perfect;

int _ecvtEntry(){
    double dbl;
    char *pchRet;
    int cch, iDec, fSign;
    char szExpected[25];


    startest();


    // Simple case

    dbl = .1;
    cch = 4;
    strcpy(szExpected, "1000");

    pchRet = _ecvt(dbl, cch, &iDec, &fSign);

    if(strcmp(pchRet, szExpected))
    {
        fail(10);
        printf("Expected == \"%s\"\n"
               "Actual   == \"%s\"\n", szExpected, pchRet);
    }
    checke(strlen(pchRet), cch);
    checke(iDec, 0);
    checke(fSign, 0);


    // Overflow case

    dbl = .15 - .05;  // Should be same value as simple case
    cch = 4;
    strcpy(szExpected, "1000");

    pchRet = _ecvt(dbl, cch, &iDec, &fSign);

    if(strcmp(pchRet, szExpected))
    {
        fail(20);
        printf("Expected == \"%s\"\n"
               "Actual   == \"%s\"\n", szExpected, pchRet);
    }
    checke(strlen(pchRet), cch);
    checke(iDec, 0);
    checke(fSign, 0);


    finish();
}
