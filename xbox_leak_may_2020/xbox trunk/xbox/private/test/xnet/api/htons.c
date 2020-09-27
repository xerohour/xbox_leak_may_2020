/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  htons.c

Abstract:

  This modules tests htons

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _HTONS_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    u_short  hostshort;                               // u_short in host byte order
    u_short  netshort;                                // u_short in network byte order
    BOOL     bRIP;                                    // Specifies a RIP test case
} HTONS_TABLE, *PHTONS_TABLE;

static HTONS_TABLE htonsTable[] =
{
    { "2.1 0x0000", 0x0000, 0x0000, FALSE },
    { "2.2 0x7FFF", 0x7FFF, 0xFF7F, FALSE },
    { "2.3 0x3040", 0x3040, 0x4030, FALSE },
    { "2.4 0xABCD", 0xABCD, 0xCDAB, FALSE },
    { "2.5 0xABCD", 0xABCD, 0xCDAB, FALSE },
    { "2.6 0xAAAA", 0xAAAA, 0xAAAA, FALSE },
    { "2.7 0x00FF", 0x00FF, 0xFF00, FALSE }
};

#define htonsTableCount (sizeof(htonsTable) / sizeof(HTONS_TABLE))



VOID
htonsTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests htons

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  bRIPs - specifies RIP testing

------------------------------------------------------------------------------*/
{
    // lpszCaseTest is a pointer to the list of cases to test
    LPSTR    lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR    lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD    dwTableIndex;

    // netshort is the returned value in network byte order
    u_short  netshort;

    // bException indicates if an exception occurred
    BOOL     bException;



    // Set the function name
    xSetFunctionName(hLog, "htons");

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_htons+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_htons-");

    for (dwTableIndex = 0; dwTableIndex < htonsTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, htonsTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, htonsTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != htonsTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, htonsTable[dwTableIndex].szVariationName);

        bException = FALSE;

        __try {
            // Call htons
            netshort = htons(htonsTable[dwTableIndex].hostshort);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == htonsTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "htons did not RIP");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "htons caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == htonsTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "htons did not RIP");
            }

            if (netshort != htonsTable[dwTableIndex].netshort) {
                xLog(hLog, XLL_FAIL, "EXPECTED: 0x%04x; RECEIVED: 0x%04x", htonsTable[dwTableIndex].netshort, netshort);
            }
            else {
                xLog(hLog, XLL_PASS, "OUT: 0x%04x", netshort);
            }
        }

        // End the variation
        xEndVariation(hLog);
    }

    // Free the list of test cases
    if (NULL != lpszCaseSkip) {
        xMemFree(hMemObject, lpszCaseSkip);
    }

    if (NULL != lpszCaseTest) {
        xMemFree(hMemObject, lpszCaseTest);
    }
}

} // namespace XNetAPINamespace

#endif
