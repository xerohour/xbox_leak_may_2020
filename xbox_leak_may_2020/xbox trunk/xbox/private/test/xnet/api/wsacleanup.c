/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  WSACleanup.c

Abstract:

  This modules tests WSACleanup

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _SOCKET_TABLE {
    int  af;       // af is the address family of the socket
    int  type;     // type is the type of the socket
    int  protocol; // protocol is the protocol of the socket
} SOCKET_TABLE, *PSOCKET_TABLE;

static SOCKET_TABLE socketTable[] =
{
    { AF_INET, SOCK_STREAM, 0            },
    { AF_INET, SOCK_STREAM, IPPROTO_TCP  },
    { AF_INET, SOCK_DGRAM,  0            },
    { AF_INET, SOCK_DGRAM,  IPPROTO_UDP  }
};

#define socketTableCount (sizeof(socketTable) / sizeof(SOCKET_TABLE))



typedef struct _WSACLEANUP_TABLE {
    CHAR  szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL  bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL  bRecleanup;                              // bRecleanup indicates if Winsock should be terminated a second time
    BOOL  bOpenSockets;                            // bOpenSockets indicates if sockets should be created before cleanup
    int   iReturnCode;                             // iReturnCode is the return value of WSACleanup
    int   iLastError;                              // iLastError is the error code if the operation failed
    BOOL  bRIP;                                    // Specifies a RIP test case
} WSACLEANUP_TABLE, *PWSACLEANUP_TABLE;

static WSACLEANUP_TABLE WSACleanupTable[] =
{
    { "7.1 Not Initialized", FALSE, FALSE, FALSE, SOCKET_ERROR, WSANOTINITIALISED },
    { "7.2 Initialized",     TRUE,  FALSE, FALSE, 0,            0                 },
    { "7.3 Cleanup Twice",   TRUE,  TRUE,  FALSE, SOCKET_ERROR, WSANOTINITIALISED },
    { "7.4 Open Sockets",    TRUE,  FALSE, TRUE,  0,            0                 }
};

#define WSACleanupTableCount (sizeof(WSACleanupTable) / sizeof(WSACLEANUP_TABLE))



VOID
WSACleanupTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests WSACleanup

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  WinsockVersion - requested version of Winsock
  bRIPs - specifies RIP testing

------------------------------------------------------------------------------*/
{
    // lpszCaseTest is a pointer to the list of cases to test
    LPSTR    lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR    lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD    dwTableIndex;

    // WSAData is the details of the Winsock implementation
    WSADATA  WSAData;

    // sSockets is an array of socket descriptors
    SOCKET   sSockets[socketTableCount];
    // dwSocket is a counter to enumerate each entry in socketTable
    DWORD    dwSocket;

    // bException indicates if an exception occurred
    BOOL     bException;
    // iReturnCode is the return code of the operation
    int      iReturnCode;
    // iLastError is the error code if the operation failed
    int      iLastError;

    // szFunctionName is the function name
    CHAR     szFunctionName[FUNCTION_NAME_LENGTH];

    // Set the function name
    sprintf(szFunctionName, "WSACleanup v%04x", WinsockVersion);
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_WSACleanup+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_WSACleanup-");

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < WSACleanupTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, WSACleanupTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, WSACleanupTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != WSACleanupTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, WSACleanupTable[dwTableIndex].szVariationName);

        // Check the state of Winsock
        if (TRUE == WSACleanupTable[dwTableIndex].bWinsockInitialized) {
            // Initialize Winsock
            WSAStartup(WinsockVersion, &WSAData);
        }

        // Check the state of terminate twice
        if (TRUE == WSACleanupTable[dwTableIndex].bRecleanup) {
            WSACleanup();
        }

        // Check the state of open sockets
        if (TRUE == WSACleanupTable[dwTableIndex].bOpenSockets) {
            for (dwSocket = 0; dwSocket < socketTableCount; dwSocket++) {
                // Create the socket
                sSockets[dwSocket] = socket(socketTable[dwSocket].af, socketTable[dwSocket].type, socketTable[dwSocket].protocol);
            }
        }

        bException = FALSE;

        __try {
            // Call WSACleanup
            iReturnCode = WSACleanup();
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == WSACleanupTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "WSACleanup RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "WSACleanup caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == WSACleanupTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "WSACleanup did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == WSACleanupTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != WSACleanupTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "iLastError - EXPECTED: %u; RECEIVED: %u", WSACleanupTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSACleanup returned SOCKET_ERROR - ec: %u", WSAGetLastError());
            }
            else if (SOCKET_ERROR == WSACleanupTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSACleanup returned non-SOCKET_ERROR");
            }
            else {
                xLog(hLog, XLL_PASS, "iReturnCode - OUT: %u", iReturnCode);
            }
        }

        // End the variation
        xEndVariation(hLog);
    }

    // Terminate net subsystem
    XNetRelease();

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
