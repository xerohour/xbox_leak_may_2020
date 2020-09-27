/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  WSAStartup.c

Abstract:

  This modules tests WSAStartup

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _WSASTARTUP_TABLE {
    CHAR       szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL       bXnetInitialized;                        // bXnetInitialized indicates if the net subsystem is initialized
    WORD       wVersionRequested;                       // wVersionRequested is the requested version of Winsock
    BOOL       bWSAData;                                // bWSAData indicates if lpWSAData is non-NULL
    BOOL       bReinitialize;                           // bReinitialize indicates if Winsock should be reinitialized to version 2.2
    int        iReturnCode;                             // iReturnCode is the return value of WSAStartup
    BOOL       bRIP;                                    // Specifies a RIP test case
} WSASTARTUP_TABLE, *PWSASTARTUP_TABLE;

static WSASTARTUP_TABLE WSAStartupTable[] =
{
    { "6.1 Not Initialized",     FALSE, MAKEWORD(2, 2),   TRUE,  FALSE, 0,              FALSE },
    { "6.2 Ver 0.0",             TRUE,  MAKEWORD(0, 0),   TRUE,  FALSE, 0,              FALSE },
    { "6.3 Ver 1.0",             TRUE,  MAKEWORD(1, 0),   TRUE,  FALSE, 0,              FALSE },
    { "6.4 Ver 1.1",             TRUE,  MAKEWORD(1, 1),   TRUE,  FALSE, 0,              FALSE },
    { "6.5 Ver 1.2",             TRUE,  MAKEWORD(1, 2),   TRUE,  FALSE, 0,              FALSE },
    { "6.6 Ver 2.0",             TRUE,  MAKEWORD(2, 0),   TRUE,  FALSE, 0,              FALSE },
    { "6.7 Ver 2.1",             TRUE,  MAKEWORD(2, 1),   TRUE,  FALSE, 0,              FALSE },
    { "6.8 Ver 2.2",             TRUE,  MAKEWORD(2, 2),   TRUE,  FALSE, 0,              FALSE },
    { "6.9 Ver 2.3",             TRUE,  MAKEWORD(2, 3),   TRUE,  FALSE, 0,              FALSE },
    { "6.10 Ver 15.15",          TRUE,  MAKEWORD(15, 15), TRUE,  FALSE, 0,              FALSE },
    { "6.11 Ver 1.1 -> Ver 2.2", TRUE,  MAKEWORD(1, 1),   TRUE,  TRUE,  0,              FALSE },
    { "6.12 NULL lpWSAData",     TRUE,  MAKEWORD(2, 2),   FALSE, FALSE, WSAEFAULT,      TRUE  },
    { "6.13 Not Initialized",    FALSE, MAKEWORD(2, 2),   TRUE,  FALSE, 0,              FALSE }
};

#define WSAStartupTableCount (sizeof(WSAStartupTable) / sizeof(WSASTARTUP_TABLE))



VOID
WSAStartupTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests WSAStartup

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

    // bXnetInitialized indicates if the net subsystem is initialized
    BOOL     bXnetInitialized = FALSE;
    // WSAData is the details of the Winsock implementation
    WSADATA  WSAData;

    // sSocket1 is the first socket descriptor
    SOCKET   sSocket1;
    // sSocket2 is the second socket descriptor
    SOCKET   sSocket2;

    // bException indicates if an exception occurred
    BOOL     bException;
    // iReturnCode is the return code of the operation
    int      iReturnCode;
    // bTestPassed indicates if the test passed
    BOOL     bTestPassed;



    // Set the function name
    xSetFunctionName(hLog, "WSAStartup");

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_WSAStartup+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_WSAStartup-");

    for (dwTableIndex = 0; dwTableIndex < WSAStartupTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, WSAStartupTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, WSAStartupTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != WSAStartupTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, WSAStartupTable[dwTableIndex].szVariationName);

        // Check the state of the net subsystem
        if (bXnetInitialized != WSAStartupTable[dwTableIndex].bXnetInitialized) {
            // Initialize or terminate net subsystem as necessary
            if (TRUE == WSAStartupTable[dwTableIndex].bXnetInitialized) {
                XNetAddRef();
            }
            else {
                XNetRelease();
            }

            // Update the state of net subsystem
            bXnetInitialized = WSAStartupTable[dwTableIndex].bXnetInitialized;
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Initialize the WSAData
        ZeroMemory(&WSAData, sizeof(WSAData));

        __try {
            // Call WSAStartup
            iReturnCode = WSAStartup(WSAStartupTable[dwTableIndex].wVersionRequested, (TRUE == WSAStartupTable[dwTableIndex].bWSAData) ? &WSAData : NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == WSAStartupTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "WSAStartup RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "WSAStartup caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == WSAStartupTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "WSAStartup did not RIP");
            }

            if (iReturnCode != WSAStartupTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "iReturnCode - EXPECTED: %u; RECEIVED: %u", WSAStartupTable[dwTableIndex].iReturnCode, iReturnCode);
            }
            else {
                xLog(hLog, XLL_PASS, "iReturnCode - OUT: %u", iReturnCode);
            }

            if ((FALSE == WSAStartupTable[dwTableIndex].bRIP) && (0 == iReturnCode) && (0 == WSAStartupTable[dwTableIndex].iReturnCode)) {
                // Check wVersion
                if (WSAData.wVersion != WSAStartupTable[dwTableIndex].wVersionRequested) {
                    xLog(hLog, XLL_FAIL, "wVersion - EXPECTED: 0x%04x; RECEIVED: 0x%04x", WSAStartupTable[dwTableIndex].wVersionRequested, WSAData.wVersion);
                    bTestPassed = FALSE;
                }
                else {
                    xLog(hLog, XLL_PASS, "wVersion - OUT: 0x%04x", WSAData.wVersion);
                }

                // Check wHighVersion
                if (WSAData.wHighVersion != MAKEWORD(2, 2)) {
                    xLog(hLog, XLL_FAIL, "wHighVersion - EXPECTED: 0x%04x; RECEIVED: 0x%04x", MAKEWORD(2, 2), WSAData.wHighVersion);
                    bTestPassed = FALSE;
                }
                else {
                    xLog(hLog, XLL_PASS, "wHighVersion - OUT: 0x%04x", WSAData.wHighVersion);
                }

                // Check szDescription
                if (0 != strcmp(WSAData.szDescription, "")) {
                    xLog(hLog, XLL_FAIL, "szDescription - EXPECTED: %s; RECEIVED: %s", "", WSAData.szDescription);
                    bTestPassed = FALSE;
                }
                else {
                    xLog(hLog, XLL_PASS, "szDescription - OUT: %s", WSAData.szDescription);
                }

                // Check szSystemStatus
                if (0 != strcmp(WSAData.szSystemStatus, "")) {
                    xLog(hLog, XLL_FAIL, "szSystemStatus - EXPECTED: %s; RECEIVED: %s", "", WSAData.szSystemStatus);
                    bTestPassed = FALSE;
                }
                else {
                    xLog(hLog, XLL_PASS, "szSystemStatus - OUT: %s", WSAData.szSystemStatus);
                }

                // Check iMaxSockets
                if (WSAData.iMaxSockets != 0) {
                    xLog(hLog, XLL_FAIL, "iMaxSockets - EXPECTED: 0x%04x; RECEIVED: 0x%04x", 0, WSAData.iMaxSockets);
                    bTestPassed = FALSE;
                }
                else {
                    xLog(hLog, XLL_PASS, "iMaxSockets - OUT: 0x%04x", WSAData.iMaxSockets);
                }

                // Check iMaxUdpDg
                if (WSAData.iMaxUdpDg != 0) {
                    xLog(hLog, XLL_FAIL, "iMaxUdpDg - EXPECTED: 0x%04x; RECEIVED: 0x%04x", 0, WSAData.iMaxUdpDg);
                    bTestPassed = FALSE;
                }
                else {
                    xLog(hLog, XLL_PASS, "iMaxUdpDg - OUT: 0x%04x", WSAData.iMaxUdpDg);
                }

                // Check lpVendorInfo
                if (NULL != WSAData.lpVendorInfo) {
                    xLog(hLog, XLL_FAIL, "lpVendorInfo - EXPECTED: %s; RECEIVED: %s", NULL, WSAData.lpVendorInfo);
                    bTestPassed = FALSE;
                }
                else {
                    xLog(hLog, XLL_PASS, "lpVendorInfo - OUT: %s", WSAData.lpVendorInfo);
                }

                // Create a socket
                sSocket1 = INVALID_SOCKET;
                sSocket1 = socket(AF_INET, SOCK_DGRAM, 0);
                sSocket2 = INVALID_SOCKET;
                sSocket2 = socket(AF_INET, SOCK_STREAM, 0);

                if (INVALID_SOCKET == sSocket1) {
                    xLog(hLog, XLL_FAIL, "UDP socket failed - Call #1 - ec = %u", WSAGetLastError());
                    bTestPassed = FALSE;
                }
                else {
                    closesocket(sSocket1);
                }

                if (INVALID_SOCKET == sSocket2) {
                    xLog(hLog, XLL_FAIL, "TCP socket failed - Call #1 - ec = %u", WSAGetLastError());
                    bTestPassed = FALSE;
                }
                else {
                    closesocket(sSocket2);
                }

                if (TRUE == WSAStartupTable[dwTableIndex].bReinitialize) {
                    // Call WSAStartup
                    iReturnCode = WSAStartup(MAKEWORD(2, 2), &WSAData);

                    // Check iReturnCode
                    if (0 != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "iReturnCode - EXPECTED: %u; RECEIVED: %u", 0, iReturnCode);
                        bTestPassed = FALSE;
                    }
                    else {
                        xLog(hLog, XLL_PASS, "iReturnCode - OUT: %u", iReturnCode);
                    }

                    // Check wVersion
                    if (WSAData.wVersion != MAKEWORD(2, 2)) {
                        xLog(hLog, XLL_FAIL, "wVersion - EXPECTED: 0x%04x; RECEIVED: 0x%04x", MAKEWORD(2, 2), WSAData.wVersion);
                        bTestPassed = FALSE;
                    }
                    else {
                        xLog(hLog, XLL_PASS, "wVersion - OUT: 0x%04x", WSAData.wVersion);

                        // Check wHighVersion
                        if (WSAData.wHighVersion != MAKEWORD(2, 2)) {
                            xLog(hLog, XLL_FAIL, "wHighVersion - EXPECTED: 0x%04x; RECEIVED: 0x%04x", MAKEWORD(2, 2), WSAData.wHighVersion);
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "wHighVersion - OUT: 0x%04x", WSAData.wHighVersion);
                        }

                        // Check szDescription
                        if (0 != strcmp(WSAData.szDescription, "")) {
                            xLog(hLog, XLL_FAIL, "szDescription - EXPECTED: %s; RECEIVED: %s", "", WSAData.szDescription);
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "szDescription - OUT: %s", WSAData.szDescription);
                        }

                        // Check szSystemStatus
                        if (0 != strcmp(WSAData.szSystemStatus, "")) {
                            xLog(hLog, XLL_FAIL, "szSystemStatus - EXPECTED: %s; RECEIVED: %s", "", WSAData.szSystemStatus);
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "szSystemStatus - OUT: %s", WSAData.szSystemStatus);
                        }

                        // Check iMaxSockets
                        if (WSAData.iMaxSockets != 0) {
                            xLog(hLog, XLL_FAIL, "iMaxSockets - EXPECTED: 0x%04x; RECEIVED: 0x%04x", 0, WSAData.iMaxSockets);
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "iMaxSockets - OUT: 0x%04x", WSAData.iMaxSockets);
                        }

                        // Check iMaxUdpDg
                        if (WSAData.iMaxUdpDg != 0) {
                            xLog(hLog, XLL_FAIL, "iMaxUdpDg - EXPECTED: 0x%04x; RECEIVED: 0x%04x", 0, WSAData.iMaxUdpDg);
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "iMaxUdpDg - OUT: 0x%04x", WSAData.iMaxUdpDg);
                        }

                        // Check lpVendorInfo
                        if (NULL != WSAData.lpVendorInfo) {
                            xLog(hLog, XLL_FAIL, "lpVendorInfo - EXPECTED: %s; RECEIVED: %s", NULL, WSAData.lpVendorInfo);
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "lpVendorInfo - OUT: %s", WSAData.lpVendorInfo);
                        }

                        // Create a socket
                        sSocket1 = socket(AF_INET, SOCK_DGRAM, 0);
                        sSocket2 = socket(AF_INET, SOCK_STREAM, 0);

                        if (INVALID_SOCKET == sSocket1) {
                            xLog(hLog, XLL_FAIL, "UDP socket failed - Call #2 - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            closesocket(sSocket1);
                        }

                        if (INVALID_SOCKET == sSocket2) {
                            xLog(hLog, XLL_FAIL, "TCP socket failed - Call #2 - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            closesocket(sSocket2);
                        }

                        // Terminate Winsock
                        WSACleanup();

                        // Create a socket
                        sSocket1 = socket(AF_INET, SOCK_DGRAM, 0);
                        sSocket2 = socket(AF_INET, SOCK_STREAM, 0);

                        if (INVALID_SOCKET == sSocket1) {
                            xLog(hLog, XLL_FAIL, "UDP socket failed - Call #3 - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            closesocket(sSocket1);
                        }

                        if (INVALID_SOCKET == sSocket2) {
                            xLog(hLog, XLL_FAIL, "TCP socket failed - Call #3 - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            closesocket(sSocket2);
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "WSAStartup succeeded");
                }

                // Terminate Winsock
                WSACleanup();
            }
        }

        while (SOCKET_ERROR != WSACleanup());

        // End the variation
        xEndVariation(hLog);
    }

    // Terminate net subsystem if necessary
    if (TRUE == bXnetInitialized) {
        XNetRelease();
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
