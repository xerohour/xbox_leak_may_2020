/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  ntohl.c

Abstract:

  This modules tests ntohl

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _NTOHL_TABLE {
    CHAR    szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    u_long  netlong;                                 // u_long in network byte order
    u_long  hostlong;                                // u_long in host byte order
    BOOL    bRIP;                                    // Specifies a RIP test case
} NTOHL_TABLE, *PNTOHL_TABLE;

static NTOHL_TABLE ntohlTable[] =
{
    { "3.1 0x00000000", 0x00000000, 0x00000000, FALSE },
    { "3.2 0x7FFFFFFF", 0x7FFFFFFF, 0xFFFFFF7F, FALSE },
    { "3.3 0x10203040", 0x10203040, 0x40302010, FALSE },
    { "3.4 0x0000ABCD", 0x0000ABCD, 0xCDAB0000, FALSE },
    { "3.5 0x0000ABCD", 0x0000ABCD, 0xCDAB0000, FALSE },
    { "3.6 0xAAAAAAAA", 0xAAAAAAAA, 0xAAAAAAAA, FALSE },
    { "3.7 0x0000FFFF", 0x0000FFFF, 0xFFFF0000, FALSE }
};

#define ntohlTableCount (sizeof(ntohlTable) / sizeof(NTOHL_TABLE))



VOID
ntohlTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests ntohl

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

    // hostlong is the returned value in host byte order
    u_long   hostlong;

    // bException indicates if an exception occurred
    BOOL     bException;



    // Set the function name
    xSetFunctionName(hLog, "ntohl");

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_ntohl+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_ntohl-");

    for (dwTableIndex = 0; dwTableIndex < ntohlTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, ntohlTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, ntohlTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != ntohlTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, ntohlTable[dwTableIndex].szVariationName);

        bException = FALSE;

        __try {
            // Call ntohl
            hostlong = ntohl(ntohlTable[dwTableIndex].netlong);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == ntohlTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "ntohl RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "ntohl caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == ntohlTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "ntohl did not RIP");
            }

            if (hostlong != ntohlTable[dwTableIndex].hostlong) {
                xLog(hLog, XLL_FAIL, "EXPECTED: 0x%08x; RECEIVED: 0x%08x", ntohlTable[dwTableIndex].hostlong, hostlong);
            }
            else {
                xLog(hLog, XLL_PASS, "OUT: 0x%08x", hostlong);
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
