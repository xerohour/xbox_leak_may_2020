/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  getsockname.c

Abstract:

  This modules tests getsockname

Author:

  Steven Kehrli (steveke) 8-Jan-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// getsockname messages

#define GETSOCKNAME_REQUEST_MSG   NETSYNC_MSG_USER + 210 + 1
#define GETSOCKNAME_COMPLETE_MSG  NETSYNC_MSG_USER + 210 + 2

typedef struct _GETSOCKNAME_REQUEST {
    DWORD    dwMessageId;
    BOOL     bServerAccept;
    u_short  Port;
} GETSOCKNAME_REQUEST, *PGETSOCKNAME_REQUEST;

typedef struct _GETSOCKNAME_COMPLETE {
    DWORD    dwMessageId;
} GETSOCKNAME_COMPLETE, *PGETSOCKNAME_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct GETSOCKNAME_TABLE {
    CHAR   szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL   bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL   bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD  dwSocket;                                // dwSocket indicates the socket to be created
    BOOL   bBind;                                   // bBind indicates if the socket is to be bound
    BOOL   bAnyPort;                                // bAnyPort indicates if any port is used for the bind call
    BOOL   bAccept;                                 // bAccept indicates if the socket is accepted
    BOOL   bConnect;                                // bConnect indicates if the socket is connected
    BOOL   bname;                                   // bname indicates if the address buffer is used
    BOOL   bnamelen;                                // bnamelen indicates if socknamelen is used
    int    namelen;                                 // namelen is the length of the address buffer
    int    iReturnCode;                             // iReturnCode is the return code of getsockname
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} GETSOCKNAME_TABLE, *PGETSOCKNAME_TABLE;

static GETSOCKNAME_TABLE getsocknameTable[] =
{
    { "21.1 Not Initialized",     FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "21.2 s = INT_MIN",         TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "21.3 s = -1",              TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "21.4 s = 0",               TRUE,  TRUE,  SOCKET_ZERO,                FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "21.5 s = INT_MAX",         TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "21.6 s = INVALID_SOCKET",  TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "21.7 Not Bound TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "21.8 Bound TCP",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.9 Bound Any Port TCP",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.10 Bound Any TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.11 Connected TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.12 Accepted TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.13 NULL addr TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "21.14 addr TCP",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.15 Large addrlen TCP",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "21.16 Exact addrlen TCP",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.17 Small addrlen TCP",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) - 1, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "21.18 Zero addrlen TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "21.19 Neg addrlen TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  -1,                      SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "21.20 NULL addrlen TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "21.21 Not Bound UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "21.22 Bound UDP",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.23 Bound Any Port UDP", TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.24 Bound Any UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.25 Connected UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.26 NULL addr UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "21.27 addr UDP",           TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.28 Large addrlen UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "21.29 Exact addrlen UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "21.30 Small addrlen UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) - 1, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "21.31 Zero addrlen UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "21.32 Neg addrlen UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  -1,                      SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "21.33 NULL addrlen UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "21.34 Closed Socket TCP",  TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "21.35 Closed Socket UDP",  TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "21.36 Not Initialized",    FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define getsocknameTableCount (sizeof(getsocknameTable) / sizeof(GETSOCKNAME_TABLE))

NETSYNC_TYPE_THREAD  getsocknameTestSessionNt =
{
    1,
    getsocknameTableCount,
    L"xnetapi_nt.dll",
    "getsocknameTestServer"
};

NETSYNC_TYPE_THREAD  getsocknameTestSessionXbox =
{
    1,
    getsocknameTableCount,
    L"xnetapi_xbox.dll",
    "getsocknameTestServer"
};



VOID
getsocknameTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests getsockname - Client side

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  WinsockVersion - requested version of Winsock
  lpszNetsyncRemote - pointer to the netsync remote address
  NetsyncRemoteType - remote netsync server type
  bRIPs - specifies RIP testing

------------------------------------------------------------------------------*/
{
    // lpszCaseTest is a pointer to the list of cases to test
    LPSTR                  lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR                  lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD                  dwTableIndex;

    // bWinsockInitialized indicates if Winsock is initialized
    BOOL                   bWinsockInitialized = FALSE;
    // WSAData is the details of the Winsock implementation
    WSADATA                WSAData;

    // hNetsyncObject is a handle to the netsync object
    HANDLE                 hNetsyncObject = INVALID_HANDLE_VALUE;
    // NetsyncTypeSession is the session type descriptor
    NETSYNC_TYPE_THREAD    NetsyncTypeSession;
    // NetsyncInAddr is the address of the netsync server
    u_long                 NetsyncInAddr = 0;
    // LowPort is the low bound of the netsync port range
    u_short                LowPort = 0;
    // HighPort is the high bound of the netsync port range
    u_short                HighPort = 0;
    // CurrentPort is the current port in the netsync port range
    u_short                CurrentPort = 0;
    // FromInAddr is the address of the netsync sender
    u_long                 FromInAddr;
    // dwMessageSize is the size of the received message
    DWORD                  dwMessageSize;
    // pMessage is a pointer to the received message
    char                   *pMessage;
    // getsocknameRequest is the request sent to the server
    GETSOCKNAME_REQUEST    getsocknameRequest;
    
    // sSocket is the socket descriptor
    SOCKET                 sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                 nsSocket;

    // localname is the local address
    SOCKADDR_IN            localname;
    // remotename is the remote address
    SOCKADDR_IN            remotename;
    // namelen is the size of the address information buffer
    int                    namelen;

    // bException indicates if an exception occurred
    BOOL                   bException;
    // iReturnCode is the return code of the operation
    int                    iReturnCode;
    // iLastError is the error code if the operation failed
    int                    iLastError;
    // bTestPassed indicates if the test passed
    BOOL                   bTestPassed;

    // szFunctionName is the function name
    CHAR                   szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    sprintf(szFunctionName, "getsockname v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_getsockname+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_getsockname-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = getsocknameTestSessionXbox;
    }
    else {
        NetsyncTypeSession = getsocknameTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < getsocknameTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, getsocknameTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, getsocknameTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != getsocknameTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, getsocknameTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == getsocknameTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != getsocknameTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == getsocknameTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = getsocknameTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == getsocknameTable[dwTableIndex].bNetsyncConnected)) {
            // Connect to the session
            hNetsyncObject = NetsyncCreateClient((NULL != lpszNetsyncRemote) ? inet_addr(lpszNetsyncRemote) : 0, NETSYNC_SESSION_THREAD, &NetsyncTypeSession, &NetsyncInAddr, NULL, &LowPort, &HighPort);
            if (INVALID_HANDLE_VALUE == hNetsyncObject) {
                xLog(hLog, XLL_BLOCK, "Cannot create Netsync client - ec = %u", GetLastError());

                // End the variation
                xEndVariation(hLog);
                break;
            }

            CurrentPort = LowPort;
        }

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        if (SOCKET_INT_MIN == getsocknameTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == getsocknameTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == getsocknameTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == getsocknameTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == getsocknameTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & getsocknameTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & getsocknameTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Bind the socket
        if (TRUE == getsocknameTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;

            if (TRUE == getsocknameTable[dwTableIndex].bAnyPort) {
                localname.sin_port = 0;
            }
            else {
                localname.sin_port = htons(CurrentPort);
            }

            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        if ((0 != (SOCKET_TCP & getsocknameTable[dwTableIndex].dwSocket)) && ((TRUE == getsocknameTable[dwTableIndex].bAccept) || (TRUE == getsocknameTable[dwTableIndex].bConnect))) {
            if (TRUE == getsocknameTable[dwTableIndex].bAccept) {
                // Listen for connection
                listen(sSocket, SOMAXCONN);
            }

            // Initialize the connect request
            getsocknameRequest.dwMessageId = GETSOCKNAME_REQUEST_MSG;
            getsocknameRequest.bServerAccept = getsocknameTable[dwTableIndex].bConnect;
            getsocknameRequest.Port = CurrentPort;

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(getsocknameRequest), (char *) &getsocknameRequest);

            // Wait for the connect result
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == getsocknameTable[dwTableIndex].bAccept) {
                // Accept the connection
                nsSocket = accept(sSocket, NULL, NULL);
            }
        }

        if (TRUE == getsocknameTable[dwTableIndex].bConnect) {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(CurrentPort);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Initialize the local name
        namelen = getsocknameTable[dwTableIndex].namelen;
        ZeroMemory(&localname, sizeof(localname));

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & getsocknameTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        __try {
            // Call getsockname
            iReturnCode = getsockname((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (TRUE == getsocknameTable[dwTableIndex].bname) ? (SOCKADDR *) &localname : NULL, (TRUE == getsocknameTable[dwTableIndex].bnamelen) ? &namelen : NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == getsocknameTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "getsockname RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "getsockname caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == getsocknameTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "getsockname did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == getsocknameTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != getsocknameTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "iLastError - EXPECTED: %u; RECEIVED: %u", getsocknameTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "getsockname returned SOCKET_ERROR - ec = %u", WSAGetLastError());
            }
            else if (SOCKET_ERROR == getsocknameTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "getsockname returned non-SOCKET_ERROR");
            }
            else {
                if (sizeof(localname) != namelen) {
                    xLog(hLog, XLL_FAIL, "namelen - EXPECTED: %d; RECEIVED: %d", sizeof(localname), namelen);
                    bTestPassed = FALSE;
                }

                if (AF_INET != localname.sin_family) {
                    xLog(hLog, XLL_FAIL, "sin_family - EXPECTED: %d; RECEIVED: %d", AF_INET, localname.sin_family);
                    bTestPassed = FALSE;
                }

                if (0 != localname.sin_addr.s_addr) {
                    xLog(hLog, XLL_FAIL, "sin_addr - EXPECTED: %u; RECEIVED: %u", 0, localname.sin_addr.s_addr);
                    bTestPassed = FALSE;
                }

                if ((TRUE == getsocknameTable[dwTableIndex].bBind) && (FALSE == getsocknameTable[dwTableIndex].bAnyPort) && (FALSE == getsocknameTable[dwTableIndex].bAccept)) {
                    if (htons(CurrentPort) != localname.sin_port) {
                        xLog(hLog, XLL_FAIL, "sin_port - EXPECTED: %d; RECEIVED: %d", htons(CurrentPort), localname.sin_port);
                        bTestPassed = FALSE;
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "getsockname succeeded");
                }
            }
        }

        if ((0 != (SOCKET_TCP & getsocknameTable[dwTableIndex].dwSocket)) && ((TRUE == getsocknameTable[dwTableIndex].bAccept) || (TRUE == getsocknameTable[dwTableIndex].bConnect))) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(getsocknameRequest), (char *) &getsocknameRequest);
        }

        // Close the sockets
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & getsocknameTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & getsocknameTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & getsocknameTable[dwTableIndex].dwSocket))) {
                shutdown(sSocket, SD_BOTH);
                closesocket(sSocket);
            }
        }

        // Increment CurrentPort
        if (0 != CurrentPort) {
            CurrentPort++;
        }

        // End the variation
        xEndVariation(hLog);
    }

    // Terminate Netsync if necesssary
    if (INVALID_HANDLE_VALUE != hNetsyncObject) {
        // Close the netsync client object
        NetsyncCloseClient(hNetsyncObject);
        hNetsyncObject = INVALID_HANDLE_VALUE;
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



#else


VOID
WINAPI
getsocknameTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests getsockname - Server side

Arguments:

  hNetsyncObject - Handle to the netsync object
  byClientCount - Specifies the number of clients in the session
  ClientAddrs - Pointer to an array of client addresses
  LowPort - Specifies the low bound of the netsync port range
  HighPort - Specifies the high bound of the netsync port range

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // FromInAddr is the address of the netsync sender
    u_long                FromInAddr;
    // dwMessageType is the type of received message
    DWORD                 dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD                 dwMessageSize;
    // pMessage is a pointer to the received message
    char                  *pMessage;
    // getsocknameRequest is the request
    GETSOCKNAME_REQUEST   getsocknameRequest;
    // getsocknameComplete is the result
    GETSOCKNAME_COMPLETE  getsocknameComplete;

    // sSocket is the socket descriptor
    SOCKET                sSocket;

    // localname is the local address
    SOCKADDR_IN           localname;
    // remotename is the remote address
    SOCKADDR_IN           remotename;



    while (TRUE) {
        // Receive a message
        NetsyncReceiveMessage(hNetsyncObject, INFINITE, &dwMessageType, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        if (NETSYNC_MSGTYPE_SERVER == dwMessageType) {
            if ((NETSYNC_MSG_STOPSESSION == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId) || (NETSYNC_MSG_DELETECLIENT == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId)) {
                NetsyncFreeMessage(pMessage);
                break;
            }

            NetsyncFreeMessage(pMessage);
            continue;
        }
        CopyMemory(&getsocknameRequest, pMessage, sizeof(getsocknameRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, SOCK_STREAM, 0);

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(getsocknameRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if (TRUE == getsocknameRequest.bServerAccept) {
            // Listen for connection
            listen(sSocket, SOMAXCONN);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(getsocknameRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        getsocknameComplete.dwMessageId = GETSOCKNAME_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(getsocknameComplete), (char *) &getsocknameComplete);

        // Wait for the ack
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        // Close the socket
        shutdown(sSocket, SD_BOTH);
        closesocket(sSocket);
    }
}

#endif
