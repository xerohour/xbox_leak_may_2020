/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  ntohs.c

Abstract:

  This modules tests ntohs

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _NTOHS_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    u_short  netshort;                                // u_short in network byte order
    u_short  hostshort;                               // u_short in host byte order
    BOOL     bRIP;                                    // Specifies a RIP test case
} NTOHS_TABLE, *PNTOHS_TABLE;

static NTOHS_TABLE ntohsTable[] =
{
    { "4.1 0x0000", 0x0000, 0x0000, FALSE },
    { "4.2 0x7FFF", 0x7FFF, 0xFF7F, FALSE },
    { "4.3 0x3040", 0x3040, 0x4030, FALSE },
    { "4.4 0xABCD", 0xABCD, 0xCDAB, FALSE },
    { "4.5 0xABCD", 0xABCD, 0xCDAB, FALSE },
    { "4.6 0xAAAA", 0xAAAA, 0xAAAA, FALSE },
    { "4.7 0x00FF", 0x00FF, 0xFF00, FALSE }
};

#define ntohsTableCount (sizeof(ntohsTable) / sizeof(NTOHS_TABLE))



VOID
ntohsTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests ntohs

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

    // hostshort is the returned value in host byte order
    u_short  hostshort;

    // bException indicates if an exception occurred
    BOOL     bException;



    // Set the function name
    xSetFunctionName(hLog, "ntohs");

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_ntohs+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_ntohs-");

    for (dwTableIndex = 0; dwTableIndex < ntohsTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, ntohsTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, ntohsTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != ntohsTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, ntohsTable[dwTableIndex].szVariationName);

        bException = FALSE;

        __try {
            // Call ntohs
            hostshort = ntohs(ntohsTable[dwTableIndex].netshort);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == ntohsTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "ntohs RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "ntohs caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == ntohsTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "ntohs did not RIP");
            }

            if (hostshort != ntohsTable[dwTableIndex].hostshort) {
                xLog(hLog, XLL_FAIL, "EXPECTED: 0x%04x; RECEIVED: 0x%04x", ntohsTable[dwTableIndex].hostshort, hostshort);
            }
            else {
                xLog(hLog, XLL_PASS, "OUT: 0x%04x", hostshort);
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
