/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  accept.c

Abstract:

  This modules tests accept

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// accept messages

#define ACCEPT_REQUEST_MSG   NETSYNC_MSG_USER + 110 + 1
#define ACCEPT_COMPLETE_MSG  NETSYNC_MSG_USER + 110 + 2

typedef struct _ACCEPT_REQUEST {
    DWORD    dwMessageId;
    int      nNumConnections;
    u_short  Port;
    BOOL     bSleep;
    int      nDataBuffers;
} ACCEPT_REQUEST, *PACCEPT_REQUEST;

typedef struct _ACCEPT_COMPLETE {
    DWORD    dwMessageId;
} ACCEPT_COMPLETE, *PACCEPT_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct ACCEPT_TABLE {
    CHAR   szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL   bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL   bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD  dwSocket;                                // dwSocket indicates the socket to be created
    BOOL   bNonblockFirstAccept;                    // bNonblockFirstAccept indicates if the socket is to be set as non-blocking for the accept
    BOOL   bNonblockNextAccept;                     // bNonblockNextAccept indicates if the socket is to be set as non-blocking for the next accept
    BOOL   bNonblockRecv;                           // bNonblockRecv indicates if the socket is to be set as non-blocking for the recv
    BOOL   bBind;                                   // bBind indicates if the socket is to be bound
    BOOL   bListen;                                 // bListen indicates if the socket is to be placed in the listening state
    int    backlog;                                 // backlog indicates the maximum length of the queue of pending connections
    BOOL   bConnected;                              // bConnected indicates if the socket is connected
    int    nNumConnects;                            // nNumConnects indicates the number of connects that should occur
    int    nNumAccepts;                             // nNumAccepts indicates the number of accepts that should occur
    BOOL   bSleep;                                  // bSleep indicates if time should elapse before the connect
    int    nDataBuffers;                            // nDataBuffers indicates the number of data buffers to be sent after the connect
    BOOL   bname;                                   // bname indicates if the address buffer is non-NULL
    BOOL   bnamelen;                                // bnamelen indicates if the address buffer length is non-NULL
    int    namelen;                                 // namelenlen is the length of the address buffer
    int    iReturnCode;                             // iReturnCode is the return code of accept
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} ACCEPT_TABLE, *PACCEPT_TABLE;

static ACCEPT_TABLE acceptTable[] =
{
    { "11.1 Not Initialized",    FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSANOTINITIALISED, FALSE },
    { "11.2 s = INT_MIN",        TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, FALSE, FALSE, FALSE, FALSE, SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSAENOTSOCK,       FALSE },
    { "11.3 s = -1",             TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, FALSE, FALSE, FALSE, FALSE, SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSAENOTSOCK,       FALSE },
    { "11.4 s = 0",              TRUE,  TRUE,  SOCKET_ZERO,                FALSE, FALSE, FALSE, FALSE, FALSE, SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSAENOTSOCK,       FALSE },
    { "11.5 s = INT_MAX",        TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, FALSE, FALSE, FALSE, FALSE, SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSAENOTSOCK,       FALSE },
    { "11.6 s = INVALID_SOCKET", TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSAENOTSOCK,       FALSE },
    { "11.7 Not Bound",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSAEINVAL,         FALSE },
    { "11.8 Bound",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  FALSE, SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSAEINVAL,         FALSE },
    { "11.9 Pending",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               FALSE, 0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.10 Pending Data",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               FALSE, 1, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.11 Wait",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               TRUE,  0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.12 Wait Data",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               TRUE,  3, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.13 Switch Accept",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 2,               2,               TRUE,  0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.14 Switch Recv",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, TRUE,  TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               TRUE,  3, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.15 bl = INT_MIN",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  INT_MIN,   FALSE, 1,               1,               TRUE,  0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.16 bl = -1",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  -1,        FALSE, 1,               1,               TRUE,  0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.17 bl = 0",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  0,         FALSE, 1,               1,               TRUE,  0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.18 bl = SOMAXCONN",    TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, MAX_CONNECTIONS, MAX_CONNECTIONS, TRUE,  0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.19 Not Bound NB",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSAEINVAL,         FALSE },
    { "11.20 Bound NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSAEINVAL,         FALSE },
    { "11.21 Pending NB",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               FALSE, 0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.22 Pending Data NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               FALSE, 1, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.23 Wait NB",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               TRUE,  0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.24 Wait Data NB",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               TRUE,  3, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.25 Switch Accept NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  TRUE,  TRUE,  SOMAXCONN, FALSE, 2,               2,               TRUE,  0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.26 Switch Recv NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               TRUE,  3, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.27 bl = INT_MIN NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  INT_MIN,   FALSE, 1,               1,               TRUE,  0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.28 bl = -1 NB",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  -1,        FALSE, 1,               1,               TRUE,  0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.29 bl = 0 NB",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,         FALSE, 1,               1,               TRUE,  0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.30 bl = SOMAXCONN NB", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  SOMAXCONN, FALSE, MAX_CONNECTIONS, MAX_CONNECTIONS, TRUE,  0, FALSE, FALSE, 0,                       0,              0,                 FALSE },
    { "11.31 addr = NULL",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               FALSE, 0, FALSE, TRUE,  0,                       0,              0,                 FALSE },
    { "11.32 addrlen = NULL",    TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               FALSE, 0, TRUE,  FALSE, 0,                       0,              WSAEFAULT,         TRUE  },
    { "11.33 Large addrlen",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               FALSE, 0, TRUE,  TRUE,  sizeof(SOCKADDR_IN) + 1, 0,              0,                 FALSE },
    { "11.34 Exact addrlen",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               FALSE, 0, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,              0,                 FALSE },
    { "11.35 Small addrlen",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               FALSE, 0, TRUE,  TRUE,  sizeof(SOCKADDR_IN) - 1, INVALID_SOCKET, WSAEFAULT,         TRUE  },
    { "11.36 Zero addrlen",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               FALSE, 0, TRUE,  TRUE,  0,                       INVALID_SOCKET, WSAEFAULT,         TRUE  },
    { "11.37 Negative addrlen",  TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 1,               1,               FALSE, 0, TRUE,  TRUE,  -1,                      INVALID_SOCKET, WSAEFAULT,         TRUE  },
    { "11.38 Connected",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, TRUE,  0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSAEINVAL,         FALSE },
    { "11.39 Closed Socket",     TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, FALSE, FALSE, FALSE, TRUE,  TRUE,  SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSAENOTSOCK,       FALSE },
    { "11.40 UDP",               TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSAEFAULT,         TRUE  },
    { "11.41 Not Initialized",   FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, SOMAXCONN, FALSE, 0,               1,               FALSE, 0, FALSE, FALSE, 0,                       INVALID_SOCKET, WSANOTINITIALISED, FALSE }
};

#define acceptTableCount (sizeof(acceptTable) / sizeof(ACCEPT_TABLE))

NETSYNC_TYPE_THREAD  acceptTestSessionNt =
{
    1,
    acceptTableCount,
    L"xnetapi_nt.dll",
    "acceptTestServer"
};

NETSYNC_TYPE_THREAD  acceptTestSessionXbox =
{
    1,
    acceptTableCount,
    L"xnetapi_xbox.dll",
    "acceptTestServer"
};



VOID
acceptTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests accept - Client side

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
    // acceptRequest is the request sent to the server
    ACCEPT_REQUEST         acceptRequest;
    
    // sSocket is the socket descriptor
    SOCKET                 sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                 nsSocket;
    // pnsSockets is an array of socket descriptors returned by accept
    SOCKET                 pnsSockets[MAX_CONNECTIONS];

    // Nonblock sets the socket to blocking or non-blocking mode
    u_long                 Nonblock;
    // bNonblocking indicates if the socket is in non-blocking mode
    BOOL                   bNonblocking = FALSE;

    // localname is the local address
    SOCKADDR_IN            localname;
    // remotename is the remote address
    SOCKADDR_IN            remotename;

    // namelen is the length of the address buffer
    int                    namelen;
    // nConnection is a counter to enumerate each connection
    int                    nConnection;

    // readfds is the set of sockets to check for a read condition
    fd_set                 readfds;

    // SendBuffer10 is the send buffer
    char                   SendBuffer10[BUFFER_10_LEN + 1];
    // RecvBuffer10 is the recv buffer
    char                   RecvBuffer10[BUFFER_10_LEN + 1];

    // dwFirstTime is the first tick count
    DWORD                  dwFirstTime;
    // dwSecondTime is the second tick count
    DWORD                  dwSecondTime;

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
    sprintf(szFunctionName, "accept v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_accept+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_accept-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = acceptTestSessionXbox;
    }
    else {
        NetsyncTypeSession = acceptTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < acceptTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, acceptTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, acceptTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != acceptTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, acceptTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == acceptTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != acceptTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == acceptTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = acceptTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == acceptTable[dwTableIndex].bNetsyncConnected)) {
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
        if (SOCKET_INT_MIN == acceptTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == acceptTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == acceptTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == acceptTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == acceptTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & acceptTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & acceptTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Set the socket to non-blocking mode
        if (TRUE == acceptTable[dwTableIndex].bNonblockFirstAccept) {
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);
            bNonblocking = TRUE;
        }
        else {
            bNonblocking = FALSE;
        }

        // Bind the socket
        if (TRUE == acceptTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
            CurrentPort++;
        }

        // Place the socket in the listening state
        if (TRUE == acceptTable[dwTableIndex].bListen) {
            listen(sSocket, acceptTable[dwTableIndex].backlog);
        }

        if (TRUE == acceptTable[dwTableIndex].bConnected) {
            // Initialize the request
            acceptRequest.dwMessageId = ACCEPT_REQUEST_MSG;
            acceptRequest.nNumConnections = 1;
            acceptRequest.Port = ntohs(localname.sin_port);
            acceptRequest.bSleep = FALSE;
            acceptRequest.nDataBuffers = 0;

            // Send the request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(acceptRequest), (char *) &acceptRequest);

            // Wait for the complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        if (0 != acceptTable[dwTableIndex].nNumConnects) {
            // Initialize the request
            acceptRequest.dwMessageId = ACCEPT_REQUEST_MSG;
            acceptRequest.nNumConnections = acceptTable[dwTableIndex].nNumConnects;
            acceptRequest.Port = ntohs(localname.sin_port);
            acceptRequest.bSleep = acceptTable[dwTableIndex].bSleep;
            acceptRequest.nDataBuffers = acceptTable[dwTableIndex].nDataBuffers;

            // Send the request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(acceptRequest), (char *) &acceptRequest);
        }

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & acceptTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        for (nConnection = 0; nConnection < acceptTable[dwTableIndex].nNumAccepts; nConnection++) {
            bTestPassed = TRUE;
            bException = FALSE;

            if ((0 != acceptTable[dwTableIndex].nNumConnects) && (FALSE == acceptTable[dwTableIndex].bSleep)) {
                // Wait for the complete
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                NetsyncFreeMessage(pMessage);
            }

            // Get the current tick count
            dwFirstTime = GetTickCount();

            // Call accept
            pnsSockets[nConnection] = INVALID_SOCKET;
            namelen = acceptTable[dwTableIndex].namelen;

            __try {
                pnsSockets[nConnection] = accept((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (TRUE == acceptTable[dwTableIndex].bname) ? (SOCKADDR *) &remotename : NULL, (TRUE == acceptTable[dwTableIndex].bnamelen) ? &namelen : NULL);
                iReturnCode = pnsSockets[nConnection];
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {
                if (TRUE == acceptTable[dwTableIndex].bRIP) {
                    xLog(hLog, XLL_PASS, "accept RIP'ed");
                }
                else {
                    xLog(hLog, XLL_EXCEPTION, "accept caused an exception - ec = 0x%08x", GetExceptionCode());
                }
                bException = TRUE;
            }

            if (FALSE == bException) {
                if ((TRUE == bNonblocking) && (0 == acceptTable[dwTableIndex].iReturnCode) && (TRUE == acceptTable[dwTableIndex].bSleep)) {
                    // Non-blocking socket
                    if (SOCKET_ERROR != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "accept returned non-SOCKET_ERROR");
                    }
                    else {
                        // Get the last error code
                        iLastError = WSAGetLastError();

                        if (WSAEWOULDBLOCK != iLastError) {
                            xLog(hLog, XLL_FAIL, "accept iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                        }
                        else {
                            xLog(hLog, XLL_PASS, "accept iLastError - OUT: %u", iLastError);

                            FD_ZERO(&readfds);
                            FD_SET(sSocket, &readfds);

                            // Wait for connect to complete
                            select(0, &readfds, NULL, NULL, NULL);

                            // Call accept
                            pnsSockets[nConnection] = INVALID_SOCKET;
                            pnsSockets[nConnection] = accept(sSocket, (TRUE == acceptTable[dwTableIndex].bname) ? (SOCKADDR *) &remotename : NULL, (TRUE == acceptTable[dwTableIndex].bnamelen) ? &namelen : NULL);
                            iReturnCode = pnsSockets[nConnection];
                        }
                    }
                }

                if (TRUE == acceptTable[dwTableIndex].bRIP) {
                    xLog(hLog, XLL_FAIL, "accept did not RIP");
                }

                if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == acceptTable[dwTableIndex].iReturnCode)) {
                    // Get the last error code
                    iLastError = WSAGetLastError();

                    if (iLastError != acceptTable[dwTableIndex].iLastError) {
                        xLog(hLog, XLL_FAIL, "accept iLastError - EXPECTED: %u; RECEIVED: %u", acceptTable[dwTableIndex].iLastError, iLastError);
                    }
                    else {
                        xLog(hLog, XLL_PASS, "accept iLastError - OUT: %u", iLastError);
                    }
                }
                else if (SOCKET_ERROR == iReturnCode) {
                    xLog(hLog, XLL_FAIL, "accept returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                }
                else if (SOCKET_ERROR == acceptTable[dwTableIndex].iReturnCode) {
                    xLog(hLog, XLL_FAIL, "accept returned non-SOCKET_ERROR");
                }
                else {
                    if ((1 == acceptTable[dwTableIndex].nDataBuffers) || (3 == acceptTable[dwTableIndex].nDataBuffers)) {
                        // Receive data
                        ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                        iReturnCode = recv(pnsSockets[nConnection], RecvBuffer10, sizeof(RecvBuffer10), 0);

                        if ((TRUE == bNonblocking) && (SOCKET_ERROR == iReturnCode)) {
                            // Non-blocking socket
                            // Get the last error code
                            iLastError = WSAGetLastError();

                            if (WSAEWOULDBLOCK != iLastError) {
                                xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "recv iLastError - OUT: %u", iLastError);

                                FD_ZERO(&readfds);
                                FD_SET(pnsSockets[nConnection], &readfds);

                                // Wait for data to be queued
                                select(0, &readfds, NULL, NULL, NULL);

                                // Receive data
                                ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                                iReturnCode = recv(pnsSockets[nConnection], RecvBuffer10, sizeof(RecvBuffer10), 0);
                            }
                        }

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            sprintf(SendBuffer10, "%05d%05d", 1, 1);

                            if (0 != strncmp(SendBuffer10, RecvBuffer10, sizeof(RecvBuffer10))) {
                                xLog(hLog, XLL_FAIL, "recv buffer - EXPECTED: %s; RECEIVED: %s", SendBuffer10, RecvBuffer10);
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "recv succeeded");
                            }
                        }

                        // Get the current tick count
                        dwSecondTime = GetTickCount();
                        if (FALSE == acceptTable[dwTableIndex].bSleep) {
                            if ((dwSecondTime - dwFirstTime) > SLEEP_ZERO_TIME) {
                                xLog(hLog, XLL_FAIL, "accept and recv nTime - EXPECTED: %d; RECEIVED: %d", 0, dwSecondTime - dwFirstTime);
                                bTestPassed = FALSE;
                            }
                        }
                        else {
                            if (((dwSecondTime - dwFirstTime) < SLEEP_LOW_TIME) || ((dwSecondTime - dwFirstTime) > SLEEP_HIGH_TIME)) {
                                xLog(hLog, XLL_FAIL, "accept and recv nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME, dwSecondTime - dwFirstTime);
                                bTestPassed = FALSE;
                            }
                        }

                        if (3 == acceptTable[dwTableIndex].nDataBuffers) {
                            // Receive data
                            ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                            iReturnCode = recv(pnsSockets[nConnection], RecvBuffer10, sizeof(RecvBuffer10), 0);

                            if (TRUE == bNonblocking) {
                                // Non-blocking socket
                                if (SOCKET_ERROR != iReturnCode) {
                                    xLog(hLog, XLL_FAIL, "recv returned non-SOCKET_ERROR");
                                    bTestPassed = FALSE;
                                }
                                else {
                                    // Get the last error code
                                    iLastError = WSAGetLastError();

                                    if (WSAEWOULDBLOCK != iLastError) {
                                        xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                                        bTestPassed = FALSE;
                                    }
                                    else {
                                        xLog(hLog, XLL_PASS, "recv iLastError - OUT: %u", iLastError);

                                        FD_ZERO(&readfds);
                                        FD_SET(pnsSockets[nConnection], &readfds);

                                        // Wait for data to be queued
                                        select(0, &readfds, NULL, NULL, NULL);

                                        // Receive data
                                        iReturnCode = recv(pnsSockets[nConnection], RecvBuffer10, sizeof(RecvBuffer10), 0);
                                    }
                                }
                            }

                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else {
                                ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                                sprintf(SendBuffer10, "%05d%05d", 2, 2);

                                if (0 != strcmp(SendBuffer10, RecvBuffer10)) {
                                    xLog(hLog, XLL_FAIL, "recv buffer - EXPECTED: %s; RECEIVED: %s", SendBuffer10, RecvBuffer10);
                                    bTestPassed = FALSE;
                                }
                                else {
                                    xLog(hLog, XLL_PASS, "recv succeeded");
                                }
                            }

                            // Get the current tick count
                            dwSecondTime = GetTickCount();
                            if (FALSE == acceptTable[dwTableIndex].bSleep) {
                                if ((dwSecondTime - dwFirstTime) > (SLEEP_ZERO_TIME * 2)) {
                                    xLog(hLog, XLL_FAIL, "accept and recv nTime - EXPECTED: %d; RECEIVED: %d", 0, dwSecondTime - dwFirstTime);
                                    bTestPassed = FALSE;
                                }
                            }
                            else {
                                if (((dwSecondTime - dwFirstTime) < (SLEEP_LOW_TIME * 2)) || ((dwSecondTime - dwFirstTime) > (SLEEP_HIGH_TIME * 2))) {
                                    xLog(hLog, XLL_FAIL, "accept and recv nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME * 2, dwSecondTime - dwFirstTime);
                                    bTestPassed = FALSE;
                                }
                            }
                            
                            // Switch the blocking mode
                            if (acceptTable[dwTableIndex].bNonblockRecv != bNonblocking) {
                                Nonblock = (TRUE == acceptTable[dwTableIndex].bNonblockRecv) ? 1 : 0;
                                ioctlsocket(pnsSockets[nConnection], FIONBIO, &Nonblock);
                            }

                            // Receive data
                            ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                            iReturnCode = recv(pnsSockets[nConnection], RecvBuffer10, sizeof(RecvBuffer10), 0);

                            if (TRUE == acceptTable[dwTableIndex].bNonblockRecv) {
                                // Non-blocking socket
                                if (SOCKET_ERROR != iReturnCode) {
                                    xLog(hLog, XLL_FAIL, "recv returned non-SOCKET_ERROR");
                                }
                                else {
                                    // Get the last error code
                                    iLastError = WSAGetLastError();

                                    if (WSAEWOULDBLOCK != iLastError) {
                                        xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                                        bTestPassed = FALSE;
                                    }
                                    else {
                                        xLog(hLog, XLL_PASS, "recv iLastError - OUT: %u", iLastError);

                                        FD_ZERO(&readfds);
                                        FD_SET(pnsSockets[nConnection], &readfds);

                                        // Wait for data to be queued
                                        select(0, &readfds, NULL, NULL, NULL);

                                        // Receive data
                                        iReturnCode = recv(pnsSockets[nConnection], RecvBuffer10, sizeof(RecvBuffer10), 0);
                                    }
                                }
                            }

                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else {
                                ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                                sprintf(SendBuffer10, "%05d%05d", 3, 3);

                                if (0 != strcmp(SendBuffer10, RecvBuffer10)) {
                                    xLog(hLog, XLL_FAIL, "recv buffer - EXPECTED: %s; RECEIVED: %s", SendBuffer10, RecvBuffer10);
                                    bTestPassed = FALSE;
                                }
                                else {
                                    xLog(hLog, XLL_PASS, "recv succeeded");
                                }
                            }

                            // Get the current tick count
                            dwSecondTime = GetTickCount();
                            if (FALSE == acceptTable[dwTableIndex].bSleep) {
                                if ((dwSecondTime - dwFirstTime) > (SLEEP_ZERO_TIME * 3)) {
                                    xLog(hLog, XLL_FAIL, "accept and recv nTime - EXPECTED: %d; RECEIVED: %d", 0, dwSecondTime - dwFirstTime);
                                    bTestPassed = FALSE;
                                }
                            }
                            else {
                                if (((dwSecondTime - dwFirstTime) < (SLEEP_LOW_TIME * 3)) || ((dwSecondTime - dwFirstTime) > (SLEEP_HIGH_TIME * 3))) {
                                    xLog(hLog, XLL_FAIL, "accept and recv nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME * 3, dwSecondTime - dwFirstTime);
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                    }
                    else {
                        // Get the current tick count
                        dwSecondTime = GetTickCount();
                        if (FALSE == acceptTable[dwTableIndex].bSleep) {
                            if ((dwSecondTime - dwFirstTime) > SLEEP_ZERO_TIME) {
                                xLog(hLog, XLL_FAIL, "accept and recv nTime - EXPECTED: %d; RECEIVED: %d", 0, dwSecondTime - dwFirstTime);
                                bTestPassed = FALSE;
                            }
                        }
                        else {
                            if (((dwSecondTime - dwFirstTime) < SLEEP_LOW_TIME) || ((dwSecondTime - dwFirstTime) > SLEEP_HIGH_TIME)) {
                                xLog(hLog, XLL_FAIL, "accept and recv nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME, dwSecondTime - dwFirstTime);
                                bTestPassed = FALSE;
                            }
                        }
                    }

                    if ((TRUE == acceptTable[dwTableIndex].bname) && (TRUE == acceptTable[dwTableIndex].bnamelen)) {
                        // Check the address
                        if (AF_INET != remotename.sin_family) {
                            xLog(hLog, XLL_FAIL, "addr->af - EXPECTED: %d; RECEIVED: %d", AF_INET, remotename.sin_family);
                            bTestPassed = FALSE;
                        }

                        if (NetsyncInAddr != remotename.sin_addr.s_addr) {
                            xLog(hLog, XLL_FAIL, "addr->addr - EXPECTED: %u; RECEIVED: %u", NetsyncInAddr, remotename.sin_addr.s_addr);
                            bTestPassed = FALSE;
                        }

                        // Check the address length
                        if (sizeof(SOCKADDR_IN) != namelen) {
                            xLog(hLog, XLL_FAIL, "addrlen - EXPECTED: %d; RECEIVED: %d", sizeof(SOCKADDR_IN), namelen);
                            bTestPassed = FALSE;
                        }
                    }

                    if (TRUE == bTestPassed) {
                        xLog(hLog, XLL_PASS, "accept succeeded");
                    }
                }
            }
        
            // Switch the blocking mode
            if (acceptTable[dwTableIndex].bNonblockNextAccept != bNonblocking) {
                Nonblock = (TRUE == acceptTable[dwTableIndex].bNonblockNextAccept) ? 1 : 0;
                ioctlsocket(sSocket, FIONBIO, &Nonblock);
                bNonblocking = acceptTable[dwTableIndex].bNonblockNextAccept;
            }
        }

        if ((0 != acceptTable[dwTableIndex].nNumConnects) && (TRUE == acceptTable[dwTableIndex].bSleep)) {
            // Wait for the complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);
        }

        // Close the connections
        for (nConnection = 0; nConnection < acceptTable[dwTableIndex].nNumAccepts; nConnection++) {
            if (INVALID_SOCKET != pnsSockets[nConnection]) {
                closesocket(pnsSockets[nConnection]);
            }
        }

        if (0 != acceptTable[dwTableIndex].nNumConnects) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(acceptRequest), (char *) &acceptRequest);
        }

        // Close the socket
        if (INVALID_SOCKET != nsSocket) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(acceptRequest), (char *) &acceptRequest);

            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & acceptTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & acceptTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & acceptTable[dwTableIndex].dwSocket))) {
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
acceptTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests accept - Server side

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
    // acceptRequest is the request
    ACCEPT_REQUEST   acceptRequest;
    // acceptComplete is the result
    ACCEPT_COMPLETE  acceptComplete;

    // psSockets is an array of socket
    SOCKET           psSockets[MAX_CONNECTIONS + 1];
    // nConnection is a counter to enumerate each connection
    int              nConnection;

    // bNagle indicates if Nagle is enabled
    BOOL             bNagle = FALSE;

    // remotename is the remote address
    SOCKADDR_IN      remotename;

    // SendBuffer10 is the send buffer
    char             SendBuffer10[BUFFER_10_LEN + 1];



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
        CopyMemory(&acceptRequest, pMessage, sizeof(acceptRequest));
        NetsyncFreeMessage(pMessage);

        for (nConnection = 0; nConnection < acceptRequest.nNumConnections; nConnection++) {
            if (TRUE == acceptRequest.bSleep) {
                // Sleep
                Sleep(SLEEP_MIDLOW_TIME);
            }

            // Create the socket
            psSockets[nConnection] = socket(AF_INET, SOCK_STREAM, 0);
        
            // Disable Nagle
            setsockopt(psSockets[nConnection], IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));

            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(acceptRequest.Port);
            connect(psSockets[nConnection], (SOCKADDR *) &remotename, sizeof(remotename));

            if ((1 == acceptRequest.nDataBuffers) || (3 == acceptRequest.nDataBuffers)) {
                // Send data
                ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                sprintf(SendBuffer10, "%05d%05d", 1, 1);

                send(psSockets[nConnection], SendBuffer10, 10, 0);

                if (3 == acceptRequest.nDataBuffers) {
                    if (TRUE == acceptRequest.bSleep) {
                        // Sleep
                        Sleep(SLEEP_MIDLOW_TIME);
                    }

                    // Send data
                    ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);

                    send(psSockets[nConnection], SendBuffer10, 10, 0);

                    if (TRUE == acceptRequest.bSleep) {
                        // Sleep
                        Sleep(SLEEP_MIDLOW_TIME);
                    }

                    // Send data
                    ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                    sprintf(SendBuffer10, "%05d%05d", 3, 3);

                    send(psSockets[nConnection], SendBuffer10, 10, 0);
                }
            }
        }

        // Send the complete
        acceptComplete.dwMessageId = ACCEPT_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(acceptComplete), (char *) &acceptComplete);

        // Wait for the ack
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        // Close the array of sockets
        for (nConnection = 0; nConnection < acceptRequest.nNumConnections; nConnection++) {
            closesocket(psSockets[nConnection]);
        }
    }
}

#endif
