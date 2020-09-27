/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  listen.c

Abstract:

  This modules tests listen

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// listen messages

#define LISTEN_REQUEST_MSG   NETSYNC_MSG_USER + 100 + 1
#define LISTEN_COMPLETE_MSG  NETSYNC_MSG_USER + 100 + 2

typedef struct _LISTEN_REQUEST {
    DWORD    dwMessageId;
    int      nNumConnections;
    u_short  Port;
} LISTEN_REQUEST, *PLISTEN_REQUEST;

typedef struct _LISTEN_COMPLETE {
    DWORD    dwMessageId;
} LISTEN_COMPLETE, *PLISTEN_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct LISTEN_TABLE {
    CHAR   szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL   bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL   bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD  dwSocket;                                // dwSocket indicates the socket to be created
    BOOL   bBind;                                   // bBind indicates if the socket is to be bound
    int    backlog1;                                // backlog1 indicates the maximum length of the queue of pending connections
    int    backlog2;                                // backlog2 indicates if the maximum length of the queue of pending connections should be updated
    BOOL   bQueueConnections;                       // bQueueConnections indicates if the connect should occur before the final listen
    BOOL   bConnected;                              // bConnected indicates if the accept should occur before the final listen
    int    nNumConnections;                         // nNumConnections indicates the number of connections that should occur
    int    iReturnCode;                             // iReturnCode is the return code of listen
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} LISTEN_TABLE, *PLISTEN_TABLE;

static LISTEN_TABLE listenTable[] =
{
    { "10.1 Not Initialized",    FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, SOMAXCONN, SOMAXCONN, FALSE, FALSE, 0,                   SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "10.2 s = INT_MIN",        TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, SOMAXCONN, SOMAXCONN, FALSE, FALSE, 0,                   SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "10.3 s = -1",             TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, SOMAXCONN, SOMAXCONN, FALSE, FALSE, 0,                   SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "10.4 s = 0",              TRUE,  TRUE,  SOCKET_ZERO,                FALSE, SOMAXCONN, SOMAXCONN, FALSE, FALSE, 0,                   SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "10.5 s = INT_MAX",        TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, SOMAXCONN, SOMAXCONN, FALSE, FALSE, 0,                   SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "10.6 s = INVALID_SOCKET", TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, SOMAXCONN, SOMAXCONN, FALSE, FALSE, 0,                   SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "10.7 Not Bound",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, SOMAXCONN, SOMAXCONN, FALSE, FALSE, 0,                   SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "10.8 Bound",              TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  SOMAXCONN, SOMAXCONN, FALSE, FALSE, 0,                   0,            0,                 FALSE },
    { "10.9 bl = INT_MIN",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  INT_MIN,   INT_MIN,   FALSE, FALSE, 1,                   0,            0,                 FALSE },
    { "10.10 bl = -1",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  -1,        -1,        FALSE, FALSE, 1,                   0,            0,                 FALSE },
    { "10.11 bl = 0",            TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  0,         0,         FALSE, FALSE, 1,                   0,            0,                 FALSE },
    { "10.12 bl = SOMAXCONN",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  SOMAXCONN, SOMAXCONN, FALSE, FALSE, MAX_CONNECTIONS + 1, 0,            0,                 FALSE },
    { "10.13 Increase bl",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  1,         SOMAXCONN, FALSE, FALSE, MAX_CONNECTIONS + 1, 0,            0,                 FALSE },
    { "10.14 Decrease bl",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  SOMAXCONN, 1,         FALSE, FALSE, 2,                   0,            0,                 FALSE },
    { "10.15 Drop Excess",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  SOMAXCONN, 1,         TRUE,  FALSE, 2,                   0,            0,                 FALSE },
    { "10.16 Connected",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  1,         SOMAXCONN, FALSE, TRUE,  1,                   SOCKET_ERROR, WSAEISCONN,        FALSE },
    { "10.17 Closed Socket",     TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, TRUE,  SOMAXCONN, SOMAXCONN, FALSE, FALSE, 0,                   SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "10.18 UDP",               TRUE,  TRUE,  SOCKET_UDP,                 FALSE, SOMAXCONN, SOMAXCONN, FALSE, FALSE, 0,                   SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "10.19 Not Initialized",   FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, SOMAXCONN, SOMAXCONN, FALSE, FALSE, 0,                   SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define listenTableCount (sizeof(listenTable) / sizeof(LISTEN_TABLE))

NETSYNC_TYPE_THREAD  listenTestSessionNt =
{
    1,
    listenTableCount,
    L"xnetapi_nt.dll",
    "listenTestServer"
};

NETSYNC_TYPE_THREAD  listenTestSessionXbox =
{
    1,
    listenTableCount,
    L"xnetapi_xbox.dll",
    "listenTestServer"
};



VOID
listenTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests listen - Client side

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
    // listenRequest is the request sent to the server
    LISTEN_REQUEST         listenRequest;

    // sSocket is the socket descriptor
    SOCKET                 sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                 nsSocket;
    // pnsSockets is an array of socket descriptors returned by accept
    SOCKET                 pnsSockets[MAX_CONNECTIONS + 1];

    // Nonblock sets the socket to blocking or non-blocking mode
    u_long                 Nonblock;

    // localname is the local address
    SOCKADDR_IN            localname;

    // bAcceptConn indicates if the socket is in listening mode
    BOOL                   bAcceptConn;
    // nOptionSize is the size of the SO_ACCEPTCONN socket option
    int                    nOptionSize;
    // nConnection is a counter to enumerate each connection
    int                    nConnection;

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
    sprintf(szFunctionName, "listen v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_listen+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_listen-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = listenTestSessionXbox;
    }
    else {
        NetsyncTypeSession = listenTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < listenTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, listenTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, listenTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != listenTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, listenTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == listenTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != listenTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == listenTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = listenTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == listenTable[dwTableIndex].bNetsyncConnected)) {
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
        if (SOCKET_INT_MIN == listenTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == listenTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == listenTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == listenTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == listenTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & listenTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & listenTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Bind the socket
        if (TRUE == listenTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
            CurrentPort++;
        }

        if (0 != (SOCKET_TCP & listenTable[dwTableIndex].dwSocket)) {
            // Check SO_ACCEPTCONN
            nOptionSize = (int) sizeof(bAcceptConn);
            getsockopt(sSocket, SOL_SOCKET, SO_ACCEPTCONN, (char *) &bAcceptConn, &nOptionSize);
            if (FALSE != bAcceptConn) {
                xLog(hLog, XLL_FAIL, "SO_ACCEPTCONN - EXPECTED: FALSE; RECEIVED: TRUE");
            }
        }

        // Initialize the backlog, if necessary
        if (listenTable[dwTableIndex].backlog1 != listenTable[dwTableIndex].backlog2) {
            listen(sSocket, listenTable[dwTableIndex].backlog1);
        }

        if (TRUE == listenTable[dwTableIndex].bQueueConnections) {
            // Initialize the connect request
            listenRequest.dwMessageId = LISTEN_REQUEST_MSG;
            listenRequest.nNumConnections = listenTable[dwTableIndex].nNumConnections;
            listenRequest.Port = ntohs(localname.sin_port);

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(listenRequest), (char *) &listenRequest);

            // Wait for the connect result
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);
        }

        if (TRUE == listenTable[dwTableIndex].bConnected) {
            // Initialize the connect request
            listenRequest.dwMessageId = LISTEN_REQUEST_MSG;
            listenRequest.nNumConnections = 1;
            listenRequest.Port = ntohs(localname.sin_port);

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(listenRequest), (char *) &listenRequest);

            // Wait for the connect result
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & listenTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        __try {
            // Call listen
            iReturnCode = listen((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, listenTable[dwTableIndex].backlog2);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == listenTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "listen RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "listen caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == listenTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "listen did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == listenTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != listenTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "iLastError - EXPECTED: %u; RECEIVED: %u", listenTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "listen returned SOCKET_ERROR - ec = %u", WSAGetLastError());
            }
            else if (SOCKET_ERROR == listenTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "listen returned non-SOCKET_ERROR");
            }
            else {
                // Check SO_ACCEPTCONN
                nOptionSize = (int) sizeof(bAcceptConn);
                getsockopt(sSocket, SOL_SOCKET, SO_ACCEPTCONN, (char *) &bAcceptConn, &nOptionSize);
                if (TRUE != bAcceptConn) {
                    xLog(hLog, XLL_FAIL, "SO_ACCEPTCONN - EXPECTED: TRUE; RECEIVED: FALSE");
                }
                else if (0 != listenTable[dwTableIndex].nNumConnections) {
                    // Set the socket to non-blocking mode
                    Nonblock = 1;
                    ioctlsocket(sSocket, FIONBIO, &Nonblock);

                    if (FALSE == listenTable[dwTableIndex].bQueueConnections) {
                        // Initialize the connect request
                        listenRequest.dwMessageId = LISTEN_REQUEST_MSG;
                        listenRequest.nNumConnections = listenTable[dwTableIndex].nNumConnections;
                        listenRequest.Port = ntohs(localname.sin_port);

                        // Send the connect request
                        NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(listenRequest), (char *) &listenRequest);

                        // Wait for the connect result
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);
                    }

                    // Accept the connections
                    for (nConnection = 0; nConnection < listenTable[dwTableIndex].nNumConnections; nConnection++) {
                        // Accept the connection
                        pnsSockets[nConnection] = INVALID_SOCKET;
                        pnsSockets[nConnection] = accept(sSocket, NULL, NULL);

                        if (INVALID_SOCKET != pnsSockets[nConnection]) {
                            // Set the socket to blocking mode
                            Nonblock = 0;
                            ioctlsocket(pnsSockets[nConnection], FIONBIO, &Nonblock);
                        }

                        if ((nConnection < ((0 >= listenTable[dwTableIndex].backlog2) ? 1 : listenTable[dwTableIndex].backlog2)) && (nConnection < MAX_CONNECTIONS)) {
                            // Accept should succeed

                            if (INVALID_SOCKET == pnsSockets[nConnection]) {
                                xLog(hLog, XLL_FAIL, "accept returned INVALID_SOCKET - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "accept succeeded");
                            }
                        }
                        else {
                            // Connection should fail

                            if (INVALID_SOCKET != pnsSockets[nConnection]) {
                                xLog(hLog, XLL_FAIL, "accept returned non-INVALID_SOCKET", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "accept failed");
                            }
                        }
                    }

                    // Close the connections
                    for (nConnection = 0; nConnection < listenTable[dwTableIndex].nNumConnections; nConnection++) {
                        if (INVALID_SOCKET != pnsSockets[nConnection]) {
                            closesocket(pnsSockets[nConnection]);
                        }
                    }

                    // Set the socket to blocking mode
                    Nonblock = 0;
                    ioctlsocket(sSocket, FIONBIO, &Nonblock);

                    // Send the ack
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(listenRequest), (char *) &listenRequest);

                    if (TRUE == bTestPassed) {
                        xLog(hLog, XLL_PASS, "listen succeeded");
                    }
                }
                else {
                    xLog(hLog, XLL_PASS, "listen succeeded");
                }
            }
        }

        // Close the socket
        if (INVALID_SOCKET != nsSocket) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(listenRequest), (char *) &listenRequest);

            closesocket(nsSocket);
        }

        // Close the socket
        if (0 == (SOCKET_CLOSED & listenTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & listenTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & listenTable[dwTableIndex].dwSocket))) {
                shutdown(sSocket, SD_BOTH);
                closesocket(sSocket);
            }
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
listenTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests listen - Server side

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
    u_long           FromInAddr;
    // dwMessageType is the type of received message
    DWORD            dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD            dwMessageSize;
    // pMessage is a pointer to the received message
    char             *pMessage;
    // listenRequest is the request
    LISTEN_REQUEST   listenRequest;
    // listenComplete is the result
    LISTEN_COMPLETE  listenComplete;

    // psSockets is an array of socket
    SOCKET           psSockets[MAX_CONNECTIONS + 1];
    // nConnection is a counter to enumerate each connection
    int              nConnection;

    // remotename is the remote address
    SOCKADDR_IN      remotename;



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
        CopyMemory(&listenRequest, pMessage, sizeof(listenRequest));
        NetsyncFreeMessage(pMessage);

        for (nConnection = 0; nConnection < listenRequest.nNumConnections; nConnection++) {
            // Create the socket
            psSockets[nConnection] = INVALID_SOCKET;
            psSockets[nConnection] = socket(AF_INET, SOCK_STREAM, 0);
        
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(listenRequest.Port);
            connect(psSockets[nConnection], (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        listenComplete.dwMessageId = LISTEN_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(listenComplete), (char *) &listenComplete);

        // Wait for the ack
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        // Close the array of sockets
        for (nConnection = 0; nConnection < listenRequest.nNumConnections; nConnection++) {
            closesocket(psSockets[nConnection]);
        }
    }
}

#endif
