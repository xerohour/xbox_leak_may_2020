/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  htonl.c

Abstract:

  This modules tests htonl

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _HTONL_TABLE {
    CHAR    szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    u_long  hostlong;                                // u_long in host byte order
    u_long  netlong;                                 // u_long in network byte order
    BOOL    bRIP;                                    // Specifies a RIP test case
} HTONL_TABLE, *PHTONL_TABLE;

static HTONL_TABLE htonlTable[] =
{
    { "1.1 0x00000000", 0x00000000, 0x00000000, FALSE },
    { "1.2 0x7FFFFFFF", 0x7FFFFFFF, 0xFFFFFF7F, FALSE },
    { "1.3 0x10203040", 0x10203040, 0x40302010, FALSE },
    { "1.4 0x0000ABCD", 0x0000ABCD, 0xCDAB0000, FALSE },
    { "1.5 0x0000ABCD", 0x0000ABCD, 0xCDAB0000, FALSE },
    { "1.6 0xAAAAAAAA", 0xAAAAAAAA, 0xAAAAAAAA, FALSE },
    { "1.7 0x0000FFFF", 0x0000FFFF, 0xFFFF0000, FALSE }
};

#define htonlTableCount (sizeof(htonlTable) / sizeof(HTONL_TABLE))



VOID
htonlTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests htonl

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  bRIPs - specifies RIP testing

------------------------------------------------------------------------------*/
{
    // lpszCaseTest is a pointer to the list of cases to test
    LPSTR   lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR   lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD   dwTableIndex;

    // netlong is the returned value in network byte order
    u_long  netlong;

    // bException indicates if an exception occurred
    BOOL    bException;



    // Set the function name
    xSetFunctionName(hLog, "htonl");

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_htonl+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_htonl-");

    for (dwTableIndex = 0; dwTableIndex < htonlTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, htonlTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, htonlTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != htonlTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, htonlTable[dwTableIndex].szVariationName);

        bException = FALSE;

        __try {
            // Call htonl
            netlong = htonl(htonlTable[dwTableIndex].hostlong);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == htonlTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "htonl RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "htonl caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == htonlTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "htonl did not RIP");
            }

            if (netlong != htonlTable[dwTableIndex].netlong) {
                xLog(hLog, XLL_FAIL, "EXPECTED: 0x%08x; RECEIVED: 0x%08x", htonlTable[dwTableIndex].netlong, netlong);
            }
            else {
                xLog(hLog, XLL_PASS, "OUT: 0x%08x", netlong);
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
