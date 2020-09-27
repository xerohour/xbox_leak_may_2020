/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  XNetDnsLookup.c

Abstract:

  This modules tests XNetDnsLookup

Author:

  Steven Kehrli (steveke) 12-Nov-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _XNETDNSLOOKUP_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL     bXnetInitialized;                        // bXnetInitialized indicates if the net subsystem is initialized
    BOOL     bHostName;                               // bHostName indicates if the hostname is non-NULL
    BOOL     bKnownHost;                              // bKnownHost indicates if the hostname is known
    BOOL     bEvent;                                  // bEvent indicates if the event is non-NULL
    BOOL     bXnDNS;                                  // bXnDNS indicates if the xndns pointer is non-NULL
    DWORD    dwReturnCode;                            // dwReturnCode is the return code of the operation
    INT      iStatus;                                 // iStatus is the status code of the operation
    BOOL     bRIP;                                    // Specifies a RIP test case
} XNETDNSLOOKUP_TABLE, *PXNETDNSLOOKUP_TABLE;

static XNETDNSLOOKUP_TABLE XNetDnsLookupInsecureTable[] =
{
    { "39.1 Not Initialized",       FALSE, TRUE,  TRUE,  FALSE, TRUE,  WSANOTINITIALISED, 0,                 FALSE },
    { "39.2 Known Host Event",      TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 0,                 FALSE },
    { "39.3 Known Host No Event",   TRUE,  TRUE,  TRUE,  FALSE, TRUE,  0,                 0,                 FALSE },
    { "39.4 Unknown Host Event",    TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 WSAHOST_NOT_FOUND, FALSE },
    { "39.5 Unknown Host No Event", TRUE,  TRUE,  FALSE, FALSE, TRUE,  0,                 WSAHOST_NOT_FOUND, FALSE },
    { "39.6 NULL Host",             TRUE,  FALSE, TRUE,  FALSE, FALSE, 0,                 0,                 TRUE  },
    { "39.7 NULL XNDNS",            TRUE,  TRUE,  TRUE,  FALSE, FALSE, 0,                 0,                 TRUE  },
    { "39.8 Not Initialized",       FALSE, TRUE,  TRUE,  TRUE,  TRUE,  WSANOTINITIALISED, 0,                 FALSE }
};

#define XNetDnsLookupInsecureTableCount (sizeof(XNetDnsLookupInsecureTable) / sizeof(XNETDNSLOOKUP_TABLE))

static XNETDNSLOOKUP_TABLE XNetDnsLookupSecureTable[] =
{
    { "39.1 Not Initialized",       FALSE, TRUE,  TRUE,  FALSE, TRUE,  WSANOTINITIALISED, 0,                 FALSE },
    { "39.2 WSAEACCES",             TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  WSAEACCES,         0,                 FALSE },
    { "39.3 Not Initialized",       FALSE, TRUE,  TRUE,  TRUE,  TRUE,  WSANOTINITIALISED, 0,                 FALSE }
};

#define XNetDnsLookupSecureTableCount (sizeof(XNetDnsLookupSecureTable) / sizeof(XNETDNSLOOKUP_TABLE))

typedef struct _HOSTADDR_ELEMENT {
    u_long                    HostAddr;
    BOOL                      bFound;
    struct _HOSTADDR_ELEMENT  *pNextHostAddr;
} HOSTADDR_ELEMENT, *PHOSTADDR_ELEMENT;

typedef struct _HOSTNAME_ELEMENT {
    LPSTR                     lpszHostName;
    UINT                      nHostAddrs;
    PHOSTADDR_ELEMENT         pHostAddr;
    struct _HOSTNAME_ELEMENT  *pNextHostName;
} HOSTNAME_ELEMENT, *PHOSTNAME_ELEMENT;



VOID
XNetDnsLookupTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests XNetDnsLookup

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
    
    // XNetDnsLookupTable is the test table
    XNETDNSLOOKUP_TABLE  *XNetDnsLookupTable;
    // XNetDnsReleaseTableCount is the test table count
    DWORD                 XNetDnsLookupTableCount;

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
    // lpszNextHostName is a pointer to the next host name within the list
    LPSTR                 lpszNextHostName = NULL;
    // lpszHostNameIp is a pointer to a host addr within the list
    LPSTR                 lpszHostAddr = NULL;
    // lpszNextHostNameIp is a pointer to the next host addr within the list
    LPSTR                 lpszNextHostAddr = NULL;

    // pKnownHostNameList is a pointer to the list of known hosts
    PHOSTNAME_ELEMENT     pKnownHostNameList = NULL;
    // pUnknownHostNameList is a pointer to the list of unknown hosts
    PHOSTNAME_ELEMENT     pUnknownHostNameList = NULL;
    // pHostNameElement is a pointer to the new host name element
    PHOSTNAME_ELEMENT     pHostNameElement = NULL;
    // pHostAddrElement is a pointer to the new host addr element
    PHOSTADDR_ELEMENT     pHostAddrElement = NULL;
    // nHostAddr is a counter to enumerate each host addr
    UINT                  nHostAddr = 0;

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
    // bHostPassed indicates if the host passed
    BOOL                  bHostPassed;



    // Set the function name
    xSetFunctionName(hLog, "XNetDnsLookup");

    // Initialize the net subsystem
    XNetAddRef();

    while (0 == (dwResult = XNetGetTitleXnAddr(&XnAddr))) {
        Sleep(1000);
    }

    // Determine the remote netsync server type
    if (0 != (XNET_GET_XNADDR_ETHERNET & dwResult)) {
        XNetDnsLookupTable = XNetDnsLookupSecureTable;
        XNetDnsLookupTableCount = XNetDnsLookupSecureTableCount;
    }
    else {
        XNetDnsLookupTable = XNetDnsLookupInsecureTable;
        XNetDnsLookupTableCount = XNetDnsLookupInsecureTableCount;
    }

    // Terminate the net subsystem
    XNetRelease();

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_XNetDnsLookup+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_XNetDnsLookup-");

    // Get the known hosts
    lpszKnownHosts = GetIniSection(hMemObject, "xnetapi_KnownHosts");

    // Enumerate each host
    if (NULL != lpszKnownHosts) {
        lpszHostName = lpszKnownHosts;
        while ('\0' != *lpszHostName) {
            lpszNextHostName = lpszHostName + strlen(lpszHostName) + 1;

            // Find the first host addr
            lpszHostAddr = strchr(lpszHostName, '=');
            if (NULL != lpszHostAddr) {
                *lpszHostAddr = '\0';
                lpszHostAddr++;
            }

            // Create the new host name element
            pHostNameElement = (PHOSTNAME_ELEMENT) xMemAlloc(hMemObject, sizeof(HOSTNAME_ELEMENT) + strlen(lpszHostName) + 1);
            if (NULL != pHostNameElement) {
                // Copy the host name
                pHostNameElement->lpszHostName = (LPSTR) ((UINT_PTR) pHostNameElement + sizeof(HOSTNAME_ELEMENT));
                strcpy(pHostNameElement->lpszHostName, lpszHostName);

                // Add the host name element to the list
                pHostNameElement->pNextHostName = pKnownHostNameList;
                pKnownHostNameList = pHostNameElement;

                while (NULL != lpszHostAddr) {
                    lpszNextHostAddr = strchr(lpszHostAddr, ';');
                    if (NULL != lpszNextHostAddr) {
                        *lpszNextHostAddr = '\0';
                        lpszNextHostAddr++;
                    }

                    // Create the new host addr element
                    pHostAddrElement = (PHOSTADDR_ELEMENT) xMemAlloc(hMemObject, sizeof(HOSTADDR_ELEMENT));
                    if (NULL != pHostAddrElement) {
                        // Copy the host addr
                        pHostAddrElement->HostAddr = inet_addr(lpszHostAddr);

                        // Add the host addr element to the list
                        pHostAddrElement->pNextHostAddr = pHostNameElement->pHostAddr;
                        pHostNameElement->pHostAddr = pHostAddrElement;

                        // Increment the number of host addrs
                        pHostNameElement->nHostAddrs++;
                    }

                    lpszHostAddr = lpszNextHostAddr;
                }
            }

            lpszHostName = lpszNextHostName;
        }
    }

    if (NULL == pKnownHostNameList) {
        xLog(hLog, XLL_BLOCK, "No Known Hosts for XNetDnsLookup");
        goto ExitTest;
    }

    // Get the unknown hosts
    lpszUnknownHosts = GetIniSection(hMemObject, "xnetapi_UnknownHosts");

    // Enumerate each host
    if (NULL != lpszUnknownHosts) {
        lpszHostName = lpszUnknownHosts;
        while ('\0' != *lpszHostName) {
            lpszNextHostName = lpszHostName + strlen(lpszHostName) + 1;

            // Create the new host name element
            pHostNameElement = (PHOSTNAME_ELEMENT) xMemAlloc(hMemObject, sizeof(HOSTNAME_ELEMENT) + strlen(lpszHostName) + 1);
            if (NULL != pHostNameElement) {
                // Copy the host name
                pHostNameElement->lpszHostName = (LPSTR) ((UINT_PTR) pHostNameElement + sizeof(HOSTNAME_ELEMENT));
                strcpy(pHostNameElement->lpszHostName, lpszHostName);

                // Add the host name element to the list
                pHostNameElement->pNextHostName = pUnknownHostNameList;
                pUnknownHostNameList = pHostNameElement;
            }

            lpszHostName = lpszNextHostName;
        }
    }

    if (NULL == pUnknownHostNameList) {
        xLog(hLog, XLL_BLOCK, "No Unknown Hosts for XNetDnsLookup");
        goto ExitTest;
    }

    // Create the event
    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    for (dwTableIndex = 0; dwTableIndex < XNetDnsLookupTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, XNetDnsLookupTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, XNetDnsLookupTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != XNetDnsLookupTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, XNetDnsLookupTable[dwTableIndex].szVariationName);

        // Check the state of the net subsystem
        if (bXnetInitialized != XNetDnsLookupTable[dwTableIndex].bXnetInitialized) {
            // Initialize or terminate net subsystem as necessary
            if (TRUE == XNetDnsLookupTable[dwTableIndex].bXnetInitialized) {
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
            bXnetInitialized = XNetDnsLookupTable[dwTableIndex].bXnetInitialized;
        }

        // Set the host name
        lpszHostName = NULL;
        if (TRUE == XNetDnsLookupTable[dwTableIndex].bKnownHost) {
            pHostNameElement = pKnownHostNameList;
            lpszHostName = pHostNameElement->lpszHostName;
        }
        else {
            pHostNameElement = pUnknownHostNameList;
            lpszHostName = pHostNameElement->lpszHostName;
        }

        bTestPassed = TRUE;
        bException = FALSE;

        do {
            bHostPassed = TRUE;

            __try {
                // Call XNetDnsLookup
                dwReturnCode = XNetDnsLookup(lpszHostName, (TRUE == XNetDnsLookupTable[dwTableIndex].bEvent) ? hEvent : NULL, (TRUE == XNetDnsLookupTable[dwTableIndex].bXnDNS) ? &pXnDNS : NULL);
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {
                if (TRUE == XNetDnsLookupTable[dwTableIndex].bRIP) {
                    xLog(hLog, XLL_PASS, "XNetDnsLookup for %s RIP'ed", lpszHostName);
                }
                else {
                    xLog(hLog, XLL_EXCEPTION, "XNetDnsLookup for %s caused an exception - ec = 0x%08x", lpszHostName, GetExceptionCode());
                    bTestPassed = FALSE;
                }
                bException = TRUE;
            }

            if (FALSE == bException) {
                if (TRUE == XNetDnsLookupTable[dwTableIndex].bRIP) {
                    xLog(hLog, XLL_FAIL, "XNetDnsLookup for %s did not RIP", lpszHostName);
                    bTestPassed = FALSE;
                }

                if (dwReturnCode != XNetDnsLookupTable[dwTableIndex].dwReturnCode) {
                    xLog(hLog, XLL_FAIL, "dwReturnCode for %s - EXPECTED: %u; RECEIVED: %u", lpszHostName, XNetDnsLookupTable[dwTableIndex].dwReturnCode, dwReturnCode);
                    bTestPassed = FALSE;
                }
                else if (0 == dwReturnCode) {
                    // Wait for the DNS resolution
                    if (TRUE == XNetDnsLookupTable[dwTableIndex].bEvent) {
                        WaitForSingleObject(hEvent, INFINITE);
                    }
                    else {
                        while (WSAEINPROGRESS == pXnDNS->iStatus) {
                            Sleep(SLEEP_ZERO_TIME);
                        }
                    }

                    // Check the status
                    if (pXnDNS->iStatus != XNetDnsLookupTable[dwTableIndex].iStatus) {
                        xLog(hLog, XLL_FAIL, "iStatus for %s - EXPECTED: %u; RECEIVED: %u", lpszHostName, XNetDnsLookupTable[dwTableIndex].iStatus, pXnDNS->iStatus);
                        bTestPassed = FALSE;
                        bHostPassed = FALSE;
                    }
                    else {
                        xLog(hLog, XLL_INFO, "iStatus for %s - OUT: %u", lpszHostName, pXnDNS->iStatus);
                    }

                    if (NULL != pHostNameElement) {
                        // Check the number of addrs
                        if (pXnDNS->cina != pHostNameElement->nHostAddrs) {
                            xLog(hLog, XLL_FAIL, "cina for %s - EXPECTED: %u; RECEIVED: %u", lpszHostName, pHostNameElement->nHostAddrs, pXnDNS->cina);
                            bTestPassed = FALSE;
                            bHostPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_INFO, "cina for %s - OUT: %u", lpszHostName, pXnDNS->cina);
                        }

                        // Flag the addrs
                        for (nHostAddr = 0; nHostAddr < pXnDNS->cina; nHostAddr++) {
                            for (pHostAddrElement = pHostNameElement->pHostAddr; NULL != pHostAddrElement; pHostAddrElement = pHostAddrElement->pNextHostAddr) {
                                if (pXnDNS->aina[nHostAddr].s_addr == pHostAddrElement->HostAddr) {
                                    if (TRUE == pHostAddrElement->bFound) {
                                        xLog(hLog, XLL_FAIL, "Duplicate Ip for %s Found: 0x%08x", lpszHostName, pHostAddrElement->HostAddr);
                                        bTestPassed = FALSE;
                                        bHostPassed = FALSE;
                                    }

                                    pHostAddrElement->bFound = TRUE;
                                    pXnDNS->aina[nHostAddr].s_addr = 0;
                                    break;
                                }
                            }
                        }

                        // Check the addrs
                        for (pHostAddrElement = pHostNameElement->pHostAddr; NULL != pHostAddrElement; pHostAddrElement = pHostAddrElement->pNextHostAddr) {
                            if (FALSE == pHostAddrElement->bFound) {
                                xLog(hLog, XLL_FAIL, "Ip for %s Not Found: 0x%08x", lpszHostName, pHostAddrElement->HostAddr);
                                bTestPassed = FALSE;
                                bHostPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_INFO, "Ip for %s Found: 0x%08x", lpszHostName, pHostAddrElement->HostAddr);
                                pHostAddrElement->bFound = FALSE;
                            }
                        }

                        // Check the addrs
                        for (nHostAddr = 0; nHostAddr < pXnDNS->cina; nHostAddr++) {
                            if (0 != pXnDNS->aina[nHostAddr].s_addr) {
                                xLog(hLog, XLL_FAIL, "Extraneous Ip for %s Found: 0x%08x", lpszHostName, pXnDNS->aina[nHostAddr].s_addr);
                                bTestPassed = FALSE;
                                bHostPassed = FALSE;
                            }
                        }
                    }

                    XNetDnsRelease(pXnDNS);

                    if (TRUE == bHostPassed) {
                        xLog(hLog, XLL_PASS, "XNetDnsLookup for %s succeeded", lpszHostName);
                    }
                }
                else {
                    xLog(hLog, XLL_PASS, "dwReturnCode for %s - OUT: %u", lpszHostName, dwReturnCode);
                }
            }

            if ((0 == XNetDnsLookupTable[dwTableIndex].dwReturnCode) && (NULL != pHostNameElement)) {
                pHostNameElement = pHostNameElement->pNextHostName;
            }
            else {
                pHostNameElement = NULL;
            }

            if (NULL != pHostNameElement) {
                lpszHostName = pHostNameElement->lpszHostName;
            }
            else {
                lpszHostName = NULL;
            }
        } while (NULL != lpszHostName);

        if (TRUE == bHostPassed) {
            xLog(hLog, XLL_PASS, "XNetDnsLookup succeeded");
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
    // Free the list of unknown host names
    while (NULL != pUnknownHostNameList) {
        pHostNameElement = pUnknownHostNameList;
        pUnknownHostNameList = pUnknownHostNameList->pNextHostName;

        while (NULL != pHostNameElement->pHostAddr) {
            pHostAddrElement = pHostNameElement->pHostAddr;
            pHostNameElement->pHostAddr = pHostNameElement->pHostAddr->pNextHostAddr;

            xMemFree(hMemObject, pHostAddrElement);
        }

        xMemFree(hMemObject, pHostNameElement);
    }

    // Free the list of known host names
    while (NULL != pKnownHostNameList) {
        pHostNameElement = pKnownHostNameList;
        pKnownHostNameList = pKnownHostNameList->pNextHostName;

        while (NULL != pHostNameElement->pHostAddr) {
            pHostAddrElement = pHostNameElement->pHostAddr;
            pHostNameElement->pHostAddr = pHostNameElement->pHostAddr->pNextHostAddr;

            xMemFree(hMemObject, pHostAddrElement);
        }

        xMemFree(hMemObject, pHostNameElement);
    }

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
