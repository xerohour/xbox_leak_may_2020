/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  socket.c

Abstract:

  This modules tests socket

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

#define SOCK_RDM        4
#define SOCK_SEQPACKET  5



typedef struct _SOCKET_TABLE {
    CHAR    szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL    bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    int     af;                                      // af is the address family of the socket
    int     type;                                    // type is the type of the socket
    int     protocol;                                // protocol is the protocol of the socket
    SOCKET  sSocket;                                 // sSocket is the socket descriptor returned by socket
    int     iLastError;                              // iLastError is the error code if the operation failed
    BOOL    bRIP;                                    // Specifies a RIP test case
} SOCKET_TABLE, *PSOCKET_TABLE;

static SOCKET_TABLE socketTable[] =
{
    { "8.1 Not Initialized TCP",    FALSE, AF_INET, SOCK_STREAM,    0,            INVALID_SOCKET, WSANOTINITIALISED,  FALSE },
    { "8.2 Not Initialized UDP",    FALSE, AF_INET, SOCK_DGRAM,     0,            INVALID_SOCKET, WSANOTINITIALISED,  FALSE },
    { "8.3 af = INT_MIN TCP",       TRUE,  INT_MIN, SOCK_STREAM,    0,            INVALID_SOCKET, WSAEAFNOSUPPORT,    FALSE },
    { "8.4 af = INT_MIN UDP",       TRUE,  INT_MIN, SOCK_DGRAM,     0,            INVALID_SOCKET, WSAEAFNOSUPPORT,    FALSE },
    { "8.5 af = -1 TCP",            TRUE,  -1,      SOCK_STREAM,    0,            INVALID_SOCKET, WSAEAFNOSUPPORT,    FALSE },
    { "8.6 af = -1 UDP",            TRUE,  -1,      SOCK_DGRAM,     0,            INVALID_SOCKET, WSAEAFNOSUPPORT,    FALSE },
    { "8.7 af = 0 TCP",             TRUE,  0,       SOCK_STREAM,    0,            0,              0,                  FALSE },
    { "8.8 af = 0 UDP",             TRUE,  0,       SOCK_DGRAM,     0,            0,              0,                  FALSE },
    { "8.9 af = AF_UNIX TCP",       TRUE,  AF_UNIX, SOCK_STREAM,    0,            INVALID_SOCKET, WSAEAFNOSUPPORT,    FALSE },
    { "8.10 af = AF_UNIX UDP",      TRUE,  AF_UNIX, SOCK_DGRAM,     0,            INVALID_SOCKET, WSAEAFNOSUPPORT,    FALSE },
    { "8.11 af = AF_MAX TCP",       TRUE,  AF_MAX,  SOCK_STREAM,    0,            INVALID_SOCKET, WSAEAFNOSUPPORT,    FALSE },
    { "8.12 af = AF_MAX UDP",       TRUE,  AF_MAX,  SOCK_DGRAM,     0,            INVALID_SOCKET, WSAEAFNOSUPPORT,    FALSE },
    { "8.13 af = INT_MAX TCP",      TRUE,  INT_MAX, SOCK_STREAM,    0,            INVALID_SOCKET, WSAEAFNOSUPPORT,    FALSE },
    { "8.14 af = INT_MAX UDP",      TRUE,  INT_MAX, SOCK_DGRAM,     0,            INVALID_SOCKET, WSAEAFNOSUPPORT,    FALSE },
    { "8.15 type = SOCK_RDM",       TRUE,  AF_INET, SOCK_RDM,       0,            INVALID_SOCKET, WSAESOCKTNOSUPPORT, FALSE },
    { "8.16 type = SOCK_SEQPACKET", TRUE,  AF_INET, SOCK_SEQPACKET, 0,            INVALID_SOCKET, WSAESOCKTNOSUPPORT, FALSE },
    { "8.17 proto = INT_MIN TCP",   TRUE,  AF_INET, SOCK_STREAM,    INT_MIN,      INVALID_SOCKET, WSAEPROTONOSUPPORT, FALSE },
    { "8.18 proto = -1 TCP",        TRUE,  AF_INET, SOCK_STREAM,    -1,           INVALID_SOCKET, WSAEPROTONOSUPPORT, FALSE },
    { "8.19 proto = UDP TCP",       TRUE,  AF_INET, SOCK_STREAM,    IPPROTO_UDP,  INVALID_SOCKET, WSAEPROTONOSUPPORT, FALSE },
    { "8.20 proto = MAX TCP",       TRUE,  AF_INET, SOCK_STREAM,    IPPROTO_MAX,  INVALID_SOCKET, WSAEPROTONOSUPPORT, FALSE },
    { "8.21 proto = INT_MAX TCP",   TRUE,  AF_INET, SOCK_STREAM,    INT_MAX,      INVALID_SOCKET, WSAEPROTONOSUPPORT, FALSE },
    { "8.22 proto = INT_MIN UDP",   TRUE,  AF_INET, SOCK_DGRAM,     INT_MIN,      INVALID_SOCKET, WSAEPROTONOSUPPORT, FALSE },
    { "8.23 proto = -1 UDP",        TRUE,  AF_INET, SOCK_DGRAM,     -1,           INVALID_SOCKET, WSAEPROTONOSUPPORT, FALSE },
    { "8.24 proto = TCP UDP",       TRUE,  AF_INET, SOCK_DGRAM,     IPPROTO_TCP,  INVALID_SOCKET, WSAEPROTONOSUPPORT, FALSE },
    { "8.25 proto = MAX UDP",       TRUE,  AF_INET, SOCK_DGRAM,     IPPROTO_MAX,  INVALID_SOCKET, WSAEPROTONOSUPPORT, FALSE },
    { "8.26 proto = INT_MAX UDP",   TRUE,  AF_INET, SOCK_DGRAM,     INT_MAX,      INVALID_SOCKET, WSAEPROTONOSUPPORT, FALSE },
    { "8.27 STREAM, proto = 0",     TRUE,  AF_INET, SOCK_STREAM,    0,            0,              0,                  FALSE },
    { "8.28 STREAM, proto = TCP",   TRUE,  AF_INET, SOCK_STREAM,    IPPROTO_TCP,  0,              0,                  FALSE },
    { "8.29 STREAM, af = 0",        TRUE,  0,       SOCK_STREAM,    IPPROTO_TCP,  0,              0,                  FALSE },
    { "8.30 DGRAM, proto = 0",      TRUE,  AF_INET, SOCK_DGRAM,     0,            0,              0,                  FALSE },
    { "8.31 DGRAM, proto = UDP",    TRUE,  AF_INET, SOCK_DGRAM,     IPPROTO_UDP,  0,              0,                  FALSE },
    { "8.32 DGRAM, af = 0",         TRUE,  0,       SOCK_DGRAM,     IPPROTO_UDP,  0,              0,                  FALSE },
    { "8.33 Not Initialized TCP",   FALSE, AF_INET, SOCK_STREAM,    0,            INVALID_SOCKET, WSANOTINITIALISED,  FALSE },
    { "8.34 Not Initialized UDP",   FALSE, AF_INET, SOCK_DGRAM,     0,            INVALID_SOCKET, WSANOTINITIALISED,  FALSE },
};

#define socketTableCount (sizeof(socketTable) / sizeof(SOCKET_TABLE))



VOID
socketTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests socket

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

    // bWinsockInitialized indicates if Winsock is initialized
    BOOL     bWinsockInitialized = FALSE;
    // WSAData is the details of the Winsock implementation
    WSADATA  WSAData;

    // sSocket is the socket descriptor
    SOCKET   sSocket;

    // bException indicates if an exception occurred
    BOOL     bException;
    // iLastError is the error code if the operation failed
    int      iLastError;

    // szFunctionName is the function name
    CHAR     szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    sprintf(szFunctionName, "socket v%04x", WinsockVersion);
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_socket+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_socket-");

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < socketTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, socketTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, socketTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != socketTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, socketTable[dwTableIndex].szVariationName);

        // Check the state of Winsock
        if (bWinsockInitialized != socketTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == socketTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = socketTable[dwTableIndex].bWinsockInitialized;
        }

        bException = FALSE;

        __try {
            // Call socket
            sSocket = socket(socketTable[dwTableIndex].af, socketTable[dwTableIndex].type, socketTable[dwTableIndex].protocol);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == socketTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "socket RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "socket caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == socketTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "socket did not RIP");
            }

            if ((INVALID_SOCKET == sSocket) && (INVALID_SOCKET == socketTable[dwTableIndex].sSocket)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != socketTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "iLastError - EXPECTED: %u; RECEIVED: %u", socketTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "iLastError - OUT: %u", iLastError);
                }
            }
            else if (INVALID_SOCKET == sSocket) {
                xLog(hLog, XLL_FAIL, "socket returned INVALID_SOCKET - ec = %u", WSAGetLastError());
            }
            else if (INVALID_SOCKET == socketTable[dwTableIndex].sSocket) {
                xLog(hLog, XLL_FAIL, "socket returned non-INVALID_SOCKET");
            }
            else {
                xLog(hLog, XLL_PASS, "socket succeeded");
            }

            if (INVALID_SOCKET != sSocket) {
                closesocket(sSocket);
            }
        }

        // End the variation
        xEndVariation(hLog);
    }

    // Terminate Winsock if necessary
    if (TRUE == bWinsockInitialized) {
        WSACleanup();
        bWinsockInitialized = FALSE;
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
