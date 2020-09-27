/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  getpeername.c

Abstract:

  This modules tests getpeername

Author:

  Steven Kehrli (steveke) 8-Jan-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// getpeername messages

#define GETPEERNAME_REQUEST_MSG   NETSYNC_MSG_USER + 220 + 1
#define GETPEERNAME_COMPLETE_MSG  NETSYNC_MSG_USER + 220 + 2

typedef struct _GETPEERNAME_REQUEST {
    DWORD    dwMessageId;
    BOOL     bServerAccept;
    BOOL     bRemoteClose;
    u_short  Port;
} GETPEERNAME_REQUEST, *PGETPEERNAME_REQUEST;

typedef struct _GETPEERNAME_COMPLETE {
    DWORD    dwMessageId;
} GETPEERNAME_COMPLETE, *PGETPEERNAME_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct GETPEERNAME_TABLE {
    CHAR   szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL   bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL   bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD  dwSocket;                                // dwSocket indicates the socket to be created
    BOOL   bBind;                                   // bBind indicates if the socket is to be bound
    BOOL   bAccept;                                 // bAccept indicates if the socket is accepted
    BOOL   bConnect;                                // bConnect indicates if the socket is connected
    BOOL   bRemoteClose;                            // bRemoteClose indicates if the remote closes the connection
    BOOL   bname;                                   // bname indicates if the address buffer is used
    BOOL   bnamelen;                                // bnamelen indicates if socknamelen is used
    int    namelen;                                 // namelen is the length of the address buffer
    int    iReturnCode;                             // iReturnCode is the return code of getpeername
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} GETPEERNAME_TABLE, *PGETPEERNAME_TABLE;

static GETPEERNAME_TABLE getpeernameTable[] =
{
    { "22.1 Not Initialized",      FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "22.2 s = INT_MIN",          TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "22.3 s = -1",               TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "22.4 s = 0",                TRUE,  TRUE,  SOCKET_ZERO,                FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "22.5 s = INT_MAX",          TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "22.6 s = INVALID_SOCKET",   TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "22.7 Not Bound TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "22.8 Bound TCP",            TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "22.9 Connected TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "22.10 Accepted TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "22.11 Connected Close TCP", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "22.12 Accepted Close TCP",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "22.13 NULL addr TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "22.14 addr TCP",            TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "22.15 Large addrlen TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "22.16 Exact addrlen TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "22.17 Small addrlen TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) - 1, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "22.18 Zero addrlen TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "22.19 Neg addrlen TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  -1,                      SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "22.20 NULL addrlen TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "22.21 Not Bound UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "22.22 Bound UDP",           TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "22.23 Connected UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "22.24 NULL addr UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "22.25 addr UDP",            TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "22.26 Large addrlen UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "22.27 Exact addrlen UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "22.28 Small addrlen UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) - 1, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "22.29 Zero addrlen UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "22.30 Neg addrlen UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  -1,                      SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "22.31 NULL addrlen UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "22.43 Closed Socket TCP",   TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "22.44 Closed Socket UDP",   TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "22.46 Not Initialized",     FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define getpeernameTableCount (sizeof(getpeernameTable) / sizeof(GETPEERNAME_TABLE))

NETSYNC_TYPE_THREAD  getpeernameTestSessionNt =
{
    1,
    getpeernameTableCount,
    L"xnetapi_nt.dll",
    "getpeernameTestServer"
};

NETSYNC_TYPE_THREAD  getpeernameTestSessionXbox =
{
    1,
    getpeernameTableCount,
    L"xnetapi_xbox.dll",
    "getpeernameTestServer"
};



VOID
getpeernameTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests getpeername - Client side

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
    // getpeernameRequest is the request sent to the server
    GETPEERNAME_REQUEST    getpeernameRequest;
    
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
    sprintf(szFunctionName, "getpeername v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_getpeername+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_getpeername-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = getpeernameTestSessionXbox;
    }
    else {
        NetsyncTypeSession = getpeernameTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < getpeernameTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, getpeernameTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, getpeernameTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != getpeernameTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, getpeernameTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == getpeernameTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != getpeernameTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == getpeernameTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = getpeernameTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == getpeernameTable[dwTableIndex].bNetsyncConnected)) {
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
        if (SOCKET_INT_MIN == getpeernameTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == getpeernameTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == getpeernameTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == getpeernameTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == getpeernameTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & getpeernameTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & getpeernameTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Bind the socket
        if (TRUE == getpeernameTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);

            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        if ((0 != (SOCKET_TCP & getpeernameTable[dwTableIndex].dwSocket)) && ((TRUE == getpeernameTable[dwTableIndex].bAccept) || (TRUE == getpeernameTable[dwTableIndex].bConnect))) {
            if (TRUE == getpeernameTable[dwTableIndex].bAccept) {
                // Listen for connection
                listen(sSocket, SOMAXCONN);
            }

            // Initialize the connect request
            getpeernameRequest.dwMessageId = GETPEERNAME_REQUEST_MSG;
            getpeernameRequest.bServerAccept = getpeernameTable[dwTableIndex].bConnect;
            getpeernameRequest.bRemoteClose = getpeernameTable[dwTableIndex].bRemoteClose;
            getpeernameRequest.Port = CurrentPort;

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(getpeernameRequest), (char *) &getpeernameRequest);

            // Wait for the connect result
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == getpeernameTable[dwTableIndex].bAccept) {
                // Accept the connection
                nsSocket = accept(sSocket, NULL, NULL);
            }
        }

        if (TRUE == getpeernameTable[dwTableIndex].bConnect) {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(CurrentPort);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        if (TRUE == getpeernameTable[dwTableIndex].bRemoteClose) {
            // Send the remote close request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(getpeernameRequest), (char *) &getpeernameRequest);

            // Wait for the remote close result
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);
        }

        // Initialize the remotename name
        namelen = getpeernameTable[dwTableIndex].namelen;
        ZeroMemory(&remotename, sizeof(remotename));

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & getpeernameTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        __try {
            // Call getpeername
            iReturnCode = getpeername((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (TRUE == getpeernameTable[dwTableIndex].bname) ? (SOCKADDR *) &remotename : NULL, (TRUE == getpeernameTable[dwTableIndex].bnamelen) ? &namelen : NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == getpeernameTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "getpeername RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "getpeername caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == getpeernameTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "getpeername did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == getpeernameTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != getpeernameTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "iLastError - EXPECTED: %u; RECEIVED: %u", getpeernameTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "getpeername returned SOCKET_ERROR - ec = %u", WSAGetLastError());
            }
            else if (SOCKET_ERROR == getpeernameTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "getpeername returned non-SOCKET_ERROR");
            }
            else {
                if (sizeof(remotename) != namelen) {
                    xLog(hLog, XLL_FAIL, "namelen - EXPECTED: %d; RECEIVED: %d", sizeof(remotename), namelen);
                    bTestPassed = FALSE;
                }

                if (AF_INET != remotename.sin_family) {
                    xLog(hLog, XLL_FAIL, "sin_family - EXPECTED: %d; RECEIVED: %d", AF_INET, remotename.sin_family);
                    bTestPassed = FALSE;
                }

                if (NetsyncInAddr != remotename.sin_addr.s_addr) {
                    xLog(hLog, XLL_FAIL, "sin_addr - EXPECTED: %u; RECEIVED: %u", NetsyncInAddr, remotename.sin_addr.s_addr);
                    bTestPassed = FALSE;
                }

                if (htons(CurrentPort) != remotename.sin_port) {
                    xLog(hLog, XLL_FAIL, "sin_port - EXPECTED: %d; RECEIVED: %d", htons(CurrentPort), remotename.sin_port);
                    bTestPassed = FALSE;
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "getpeername succeeded");
                }
            }
        }

        if ((0 != (SOCKET_TCP & getpeernameTable[dwTableIndex].dwSocket)) && ((TRUE == getpeernameTable[dwTableIndex].bAccept) || (TRUE == getpeernameTable[dwTableIndex].bConnect))) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(getpeernameRequest), (char *) &getpeernameRequest);
        }

        // Close the sockets
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & getpeernameTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & getpeernameTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & getpeernameTable[dwTableIndex].dwSocket))) {
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
getpeernameTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests getpeername - Server side

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
    // getpeernameRequest is the request
    GETPEERNAME_REQUEST   getpeernameRequest;
    // getpeernameComplete is the result
    GETPEERNAME_COMPLETE  getpeernameComplete;

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
        CopyMemory(&getpeernameRequest, pMessage, sizeof(getpeernameRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, SOCK_STREAM, 0);

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(getpeernameRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if (TRUE == getpeernameRequest.bServerAccept) {
            // Listen for connection
            listen(sSocket, SOMAXCONN);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(getpeernameRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        getpeernameComplete.dwMessageId = GETPEERNAME_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(getpeernameComplete), (char *) &getpeernameComplete);

        if (TRUE == getpeernameRequest.bRemoteClose) {
            // Wait for the remote close request
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            // Close the socket
            shutdown(sSocket, SD_BOTH);
            closesocket(sSocket);

            // Send the complete
            getpeernameComplete.dwMessageId = GETPEERNAME_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(getpeernameComplete), (char *) &getpeernameComplete);
        }

        // Wait for the ack
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if (FALSE == getpeernameRequest.bRemoteClose) {
            // Close the socket
            shutdown(sSocket, SD_BOTH);
            closesocket(sSocket);
        }
    }
}

#endif
