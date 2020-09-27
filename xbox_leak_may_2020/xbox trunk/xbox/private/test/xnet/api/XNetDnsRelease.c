/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  XNetDnsRelease.c

Abstract:

  This modules tests XNetDnsRelease

Author:

  Steven Kehrli (steveke) 14-Nov-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _XNETDNSRELEASE_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL     bXnetInitialized;                        // bXnetInitialized indicates if the net subsystem is initialized
    BOOL     bKnownHost;                              // bKnownHost indicates if the hostname is known
    BOOL     bWait;                                   // bWait indicates if dns resolution completes
    BOOL     bXnDNS;                                  // bXnDNS indicates if the xndns pointer is non-NULL
    DWORD    dwReturnCode;                            // dwReturnCode is the return code of the operation
    BOOL     bRIP;                                    // Specifies a RIP test case
} XNETDNSRELEASE_TABLE, *PXNETDNSRELEASE_TABLE;

static XNETDNSRELEASE_TABLE XNetDnsReleaseInsecureTable[] =
{
    { "40.1 Not Initialized",      FALSE, TRUE,  TRUE,  TRUE,  WSANOTINITIALISED, FALSE },
    { "40.2 Known Host Wait",      TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "40.3 Unknown Host Wait",    TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "40.4 Known Host No Wait",   TRUE,  TRUE,  FALSE, TRUE,  0,                 FALSE },
    { "40.5 Unknown Host No Wait", TRUE,  FALSE, FALSE, TRUE,  0,                 FALSE },
    { "40.6 NULL XNDNS",           TRUE,  TRUE,  TRUE,  FALSE, 0,                 TRUE  },
    { "40.7 Not Initialized",      FALSE, TRUE,  TRUE,  TRUE,  WSANOTINITIALISED, FALSE }
};

#define XNetDnsReleaseInsecureTableCount (sizeof(XNetDnsReleaseInsecureTable) / sizeof(XNETDNSRELEASE_TABLE))

static XNETDNSRELEASE_TABLE XNetDnsReleaseSecureTable[] =
{
    { "40.1 Not Initialized",      FALSE, TRUE,  TRUE,  TRUE,  WSANOTINITIALISED, FALSE },
    { "40.2 WSAEACCES",            TRUE,  TRUE,  FALSE, TRUE,  WSAEACCES,         FALSE },
    { "40.3 Not Initialized",      FALSE, TRUE,  TRUE,  TRUE,  WSANOTINITIALISED, FALSE }
};

#define XNetDnsReleaseSecureTableCount (sizeof(XNetDnsReleaseSecureTable) / sizeof(XNETDNSRELEASE_TABLE))



VOID
XNetDnsReleaseTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests XNetDnsRelease

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  bRIPs - specifies RIP testing

------------------------------------------------------------------------------*/
{
    // lpszCaseTest is a pointer to the list of cases to test
    LPSTR                 lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR                 lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD                 dwTableIndex;

    // XnAddr is the title xnet address
    XNADDR                XnAddr;
    // dwResult is the result of XNetGetTitleXnAddr
    DWORD                 dwResult = 0;
    
    // XNetDnsReleaseTable is the test table
    XNETDNSRELEASE_TABLE  *XNetDnsReleaseTable;
    // XNetDnsReleaseTableCount is the test table count
    DWORD                 XNetDnsReleaseTableCount;

    // bXnetInitialized indicates if the net subsystem is initialized
    BOOL                  bXnetInitialized = FALSE;
    // LocalXnAddr is the address of the client
    XNADDR                LocalXnAddr;

    // lpszKnownHosts is a pointer to the list of known hosts
    LPSTR                 lpszKnownHosts = NULL;
    // lpszUnknownHosts is a pointer to the list of unknown hosts
    LPSTR                 lpszUnknownHosts = NULL;
    // lpszHostName is a pointer to a host name within the list
    LPSTR                 lpszHostName = NULL;

    // hEvent is a handle to the event
    HANDLE                hEvent = NULL;
    // pXnDNS is a pointer to the DNS entry
    XNDNS                 *pXnDNS = NULL;

    // bException indicates if an exception occurred
    BOOL                  bException;
    // dwReturnCode is the return code of the operation
    int                   dwReturnCode;
    // bTestPassed indicates if the test passed
    BOOL                  bTestPassed;



    // Set the function name
    xSetFunctionName(hLog, "XNetDnsRelease");

    // Initialize the net subsystem
    XNetAddRef();

    while (0 == (dwResult = XNetGetTitleXnAddr(&XnAddr))) {
        Sleep(1000);
    }

    // Determine the remote netsync server type
    if (0 != (XNET_GET_XNADDR_ETHERNET & dwResult)) {
        XNetDnsReleaseTable = XNetDnsReleaseSecureTable;
        XNetDnsReleaseTableCount = XNetDnsReleaseSecureTableCount;
    }
    else {
        XNetDnsReleaseTable = XNetDnsReleaseInsecureTable;
        XNetDnsReleaseTableCount = XNetDnsReleaseInsecureTableCount;
    }

    // Terminate the net subsystem
    XNetRelease();

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_XNetDnsRelease+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_XNetDnsRelease-");

    // Get the known hosts
    lpszKnownHosts = GetIniSection(hMemObject, "xnetapi_KnownHosts");

    // Enumerate the host
    if (NULL != lpszKnownHosts) {
        lpszHostName = strchr(lpszKnownHosts, '=');
        if (NULL != lpszHostName) {
            *lpszHostName = '\0';
        }
    }

    if (NULL == lpszKnownHosts) {
        xLog(hLog, XLL_BLOCK, "No Known Hosts for XNetDnsRelease");
        goto ExitTest;
    }

    // Get the unknown hosts
    lpszUnknownHosts = GetIniSection(hMemObject, "xnetapi_UnknownHosts");

    if (NULL == lpszUnknownHosts) {
        xLog(hLog, XLL_BLOCK, "No Unknown Hosts for XNetDnsRelease");
        goto ExitTest;
    }

    // Create the event
    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    for (dwTableIndex = 0; dwTableIndex < XNetDnsReleaseTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, XNetDnsReleaseTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, XNetDnsReleaseTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != XNetDnsReleaseTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, XNetDnsReleaseTable[dwTableIndex].szVariationName);

        // Check the state of the net subsystem
        if (bXnetInitialized != XNetDnsReleaseTable[dwTableIndex].bXnetInitialized) {
            // Initialize or terminate net subsystem as necessary
            if (TRUE == XNetDnsReleaseTable[dwTableIndex].bXnetInitialized) {
                XNetAddRef();

                // Get the local xnaddr
                do {
                    dwReturnCode = XNetGetTitleXnAddr(&LocalXnAddr);
                    if (0 == dwReturnCode) {
                        Sleep(SLEEP_ZERO_TIME);
                    }
                } while (0 == dwReturnCode);
            }
            else {
                XNetRelease();
            }

            // Update the state of net subsystem
            bXnetInitialized = XNetDnsReleaseTable[dwTableIndex].bXnetInitialized;
        }

        // Set the host name
        lpszHostName = NULL;
        if (TRUE == XNetDnsReleaseTable[dwTableIndex].bKnownHost) {
            lpszHostName = lpszKnownHosts;
        }
        else {
            lpszHostName = lpszUnknownHosts;
        }

        bTestPassed = TRUE;
        bException = FALSE;

        if ((TRUE == XNetDnsReleaseTable[dwTableIndex].bXnetInitialized) && (TRUE == XNetDnsReleaseTable[dwTableIndex].bXnDNS)) {
            // Call XNetDnsLookup
            dwReturnCode = XNetDnsLookup(lpszHostName, hEvent, &pXnDNS);

            if ((0 == dwReturnCode) && (TRUE == XNetDnsReleaseTable[dwTableIndex].bWait)) {
                WaitForSingleObject(hEvent, INFINITE);
            }
        }

        __try {
            // Call XNetDnsRelease
            dwReturnCode = XNetDnsRelease((TRUE == XNetDnsReleaseTable[dwTableIndex].bXnDNS) ? pXnDNS : NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == XNetDnsReleaseTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "XNetDnsRelease RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "XNetDnsRelease caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == XNetDnsReleaseTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "XNetDnsRelease did not RIP");
            }

            if (dwReturnCode != XNetDnsReleaseTable[dwTableIndex].dwReturnCode) {
                xLog(hLog, XLL_FAIL, "dwReturnCode - EXPECTED: %u; RECEIVED: %u", XNetDnsReleaseTable[dwTableIndex].dwReturnCode, dwReturnCode);
            }
            else if (0 == dwReturnCode) {
                // Call XNetDnsRelease again
                dwReturnCode = XNetDnsRelease(pXnDNS);

                if (WSAEINVAL != dwReturnCode) {
                    xLog(hLog, XLL_FAIL, "Second XNetDnsRelease - EXPECTED: %u; RECEIVED: %u", WSAEINVAL, dwReturnCode);
                    bTestPassed = FALSE;
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "XNetDnsRelease succeeded");
                }

            }
            else {
                xLog(hLog, XLL_PASS, "dwReturnCode - OUT: %u", dwReturnCode);
            }
        }

        // End the variation
        xEndVariation(hLog);
    }

    // Terminate net subsystem if necessary
    if (TRUE == bXnetInitialized) {
        XNetRelease();
    }

    // Close the event
    CloseHandle(hEvent);

ExitTest:
    // Free the list of unknown hosts
    if (NULL != lpszUnknownHosts) {
        xMemFree(hMemObject, lpszUnknownHosts);
    }

    // Free the list of known hosts
    if (NULL != lpszKnownHosts) {
        xMemFree(hMemObject, lpszKnownHosts);
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
