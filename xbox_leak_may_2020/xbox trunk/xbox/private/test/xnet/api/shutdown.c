/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  shutdown.c

Abstract:

  This modules tests shutdown

Author:

  Steven Kehrli (steveke) 8-Mar-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// shutdown messages

#define SHUTDOWN_REQUEST_MSG   NETSYNC_MSG_USER + 280 + 1
#define SHUTDOWN_COMPLETE_MSG  NETSYNC_MSG_USER + 280 + 2

typedef struct _SHUTDOWN_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  Port;
    BOOL     bServerAccept;
    BOOL     bServerConnect;
    BOOL     bServerFirstSend;
    BOOL     bServerSecondSend;
} SHUTDOWN_REQUEST, *PSHUTDOWN_REQUEST;

typedef struct _SHUTDOWN_COMPLETE {
    DWORD    dwMessageId;
} SHUTDOWN_COMPLETE, *PSHUTDOWN_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

#define SHUTDOWN_SEND     0x01
#define SHUTDOWN_RECEIVE  0x02
#define SHUTDOWN_BOTH     0x03

#define SHUTDOWN_INT_MIN  0x10
#define SHUTDOWN_NEG_ONE  0x20
#define SHUTDOWN_INT_MAX  0x30
#define SHUTDOWN_THREE    0x40



typedef struct SHUTDOWN_TABLE {
    CHAR   szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL   bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL   bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD  dwSocket;                                // dwSocket indicates the socket to be created
    BOOL   bServerAccept;                           // bServerAccept indicates if server accepts
    BOOL   bServerConnect;                          // bServerConnect indicates if server connects
    BOOL   bServerFirstSend;                        // bServerFirstSend indicates if server sends data
    BOOL   bServerSecondSend;                       // bServerSecondSend indicates if server sends data
    DWORD  dwHow1;                                  // dwHow1 specifies how to shutdown the socket
    DWORD  dwHow2;                                  // dwHow2 specifies how to shutdown the socket
    DWORD  dwHow3;                                  // dwHow3 specifies how to shutdown the socket
    int    iReturnCode;                             // iReturnCode is the return code of shutdown
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} SHUTDOWN_TABLE, *PSHUTDOWN_TABLE;

static SHUTDOWN_TABLE shutdownTable[] =
{
    { "28.1 Not Initialized",         FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "28.2 s = INT_MIN",             TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "28.3 s = -1",                  TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "28.4 s = 0",                   TRUE,  TRUE,  SOCKET_ZERO,                FALSE, FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "28.5 s = INT_MAX",             TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "28.6 s = INVALID_SOCKET",      TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "28.7 Not Connected Send",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, SHUTDOWN_SEND,    0,                0,                SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "28.8 Not Connected Receive",   TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, SHUTDOWN_RECEIVE, 0,                0,                SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "28.9 Not Connected Both",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "28.10 Connected Send",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_SEND,    0,                0,                0,            0,                 FALSE },
    { "28.11 Connected Send Data",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, SHUTDOWN_SEND,    0,                0,                0,            0,                 FALSE },
    { "28.12 Connected Receive",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_RECEIVE, 0,                0,                0,            0,                 FALSE },
    { "28.13 Connected Receive Data", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, SHUTDOWN_RECEIVE, 0,                0,                0,            0,                 FALSE },
    { "28.14 Connected Receive 2",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, TRUE,  SHUTDOWN_RECEIVE, 0,                0,                0,            0,                 FALSE },
    { "28.15 Connected Both",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                0,            0,                 FALSE },
    { "28.16 Connected Both Data",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, TRUE,  FALSE, SHUTDOWN_BOTH,    0,                0,                0,            0,                 FALSE },
    { "28.17 Connected Both 2",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, TRUE,  SHUTDOWN_BOTH,    0,                0,                0,            0,                 FALSE },
    { "28.18 Accepted Send",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, SHUTDOWN_SEND,    0,                0,                0,            0,                 FALSE },
    { "28.19 Accepted Send Data",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, SHUTDOWN_SEND,    0,                0,                0,            0,                 FALSE },
    { "28.20 Accepted Receive",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, SHUTDOWN_RECEIVE, 0,                0,                0,            0,                 FALSE },
    { "28.21 Accepted Receive Data",  TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, SHUTDOWN_RECEIVE, 0,                0,                0,            0,                 FALSE },
    { "28.22 Accepted Receive 2",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, TRUE,  SHUTDOWN_RECEIVE, 0,                0,                0,            0,                 FALSE },
    { "28.23 Accepted Both",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                0,            0,                 FALSE },
    { "28.24 Accepted Both Data",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, SHUTDOWN_BOTH,    0,                0,                0,            0,                 FALSE },
    { "28.25 Accepted Both 2",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, TRUE,  SHUTDOWN_BOTH,    0,                0,                0,            0,                 FALSE },
    { "28.26 Connected SRB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_SEND,    SHUTDOWN_RECEIVE, SHUTDOWN_BOTH,    0,            0,                 FALSE },
    { "28.27 Accepted SRB",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, SHUTDOWN_SEND,    SHUTDOWN_RECEIVE, SHUTDOWN_BOTH,    0,            0,                 FALSE },
    { "28.28 Connected SBR",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_SEND,    SHUTDOWN_BOTH,    SHUTDOWN_RECEIVE, 0,            0,                 FALSE },
    { "28.29 Accepted SBR",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, SHUTDOWN_SEND,    SHUTDOWN_BOTH,    SHUTDOWN_RECEIVE, 0,            0,                 FALSE },
    { "28.30 Connected RSB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_RECEIVE, SHUTDOWN_SEND,    SHUTDOWN_BOTH,    0,            0,                 FALSE },
    { "28.31 Accepted RSB",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, SHUTDOWN_RECEIVE, SHUTDOWN_SEND,    SHUTDOWN_BOTH,    0,            0,                 FALSE },
    { "28.32 Connected RBS",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_RECEIVE, SHUTDOWN_BOTH,    SHUTDOWN_SEND,    0,            0,                 FALSE },
    { "28.33 Accepted RBS",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, SHUTDOWN_RECEIVE, SHUTDOWN_BOTH,    SHUTDOWN_SEND,    0,            0,                 FALSE },
    { "28.34 Connected BSR",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    SHUTDOWN_SEND,    SHUTDOWN_RECEIVE, 0,            0,                 FALSE },
    { "28.35 Accepted BSR",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, SHUTDOWN_BOTH,    SHUTDOWN_SEND,    SHUTDOWN_RECEIVE, 0,            0,                 FALSE },
    { "28.36 Connected BRS",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    SHUTDOWN_RECEIVE, SHUTDOWN_SEND,    0,            0,                 FALSE },
    { "28.37 Accepted BRS",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, SHUTDOWN_BOTH,    SHUTDOWN_RECEIVE, SHUTDOWN_SEND,    0,            0,                 FALSE },
    { "28.38 UDP Send",               TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_SEND,    0,                0,                0,            0,                 FALSE },
    { "28.39 UDP Send Data",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  TRUE,  FALSE, SHUTDOWN_SEND,    0,                0,                0,            0,                 FALSE },
    { "28.40 UDP Receive",            TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_RECEIVE, 0,                0,                0,            0,                 FALSE },
    { "28.41 UDP Receive Data",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  TRUE,  FALSE, SHUTDOWN_RECEIVE, 0,                0,                0,            0,                 FALSE },
    { "28.42 UDP Both",               TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                0,            0,                 FALSE },
    { "28.43 UDP Both Data",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  TRUE,  FALSE, SHUTDOWN_BOTH,    0,                0,                0,            0,                 FALSE },
    { "28.44 UDP SRB",                TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_SEND,    SHUTDOWN_RECEIVE, SHUTDOWN_BOTH,    0,            0,                 FALSE },
    { "28.45 UDP SBR",                TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_SEND,    SHUTDOWN_BOTH,    SHUTDOWN_RECEIVE, 0,            0,                 FALSE },
    { "28.46 UDP RSB",                TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_RECEIVE, SHUTDOWN_SEND,    SHUTDOWN_BOTH,    0,            0,                 FALSE },
    { "28.47 UDP RBS",                TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_RECEIVE, SHUTDOWN_BOTH,    SHUTDOWN_SEND,    0,            0,                 FALSE },
    { "28.48 UDP BSR",                TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_BOTH,    SHUTDOWN_SEND,    SHUTDOWN_RECEIVE, 0,            0,                 FALSE },
    { "28.49 UDP BRS",                TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_BOTH,    SHUTDOWN_RECEIVE, SHUTDOWN_SEND,    0,            0,                 FALSE },
    { "28.50 how = INT_MIN TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_INT_MIN, 0,                0,                SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "28.51 how = -1 TCP",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_NEG_ONE, 0,                0,                SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "28.52 how = INT_MAX TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_INT_MAX, 0,                0,                SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "28.53 how = 3 TCP",            TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, SHUTDOWN_THREE,   0,                0,                SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "28.54 how = INT_MIN UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_INT_MIN, 0,                0,                SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "28.55 how = -1 UDP",           TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_NEG_ONE, 0,                0,                SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "28.56 how = INT_MAX UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_INT_MAX, 0,                0,                SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "28.57 how = 3 UDP",            TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, SHUTDOWN_THREE,   0,                0,                SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "28.58 Closed Socket TCP",      TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "28.59 Closed Socket UDP",      TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "28.60 Not Initialized",        FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, SHUTDOWN_BOTH,    0,                0,                SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define shutdownTableCount (sizeof(shutdownTable) / sizeof(SHUTDOWN_TABLE))

NETSYNC_TYPE_THREAD  shutdownTestSessionNt =
{
    1,
    shutdownTableCount,
    L"xnetapi_nt.dll",
    "shutdownTestServer"
};

NETSYNC_TYPE_THREAD  shutdownTestSessionXbox =
{
    1,
    shutdownTableCount,
    L"xnetapi_xbox.dll",
    "shutdownTestServer"
};



VOID
shutdownTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests shutdown - Client side

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
    // shutdownRequest is the request sent to the server
    SHUTDOWN_REQUEST       shutdownRequest;
    
    // sSocket is the socket descriptor
    SOCKET                 sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                 nsSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD                  dwBufferSize = BUFFER_10_LEN;
    // bNagle indicates if Nagle is enabled
    BOOL                   bNagle = FALSE;
    // Nonblock sets the socket to blocking or non-blocking mode
    u_long                 Nonblock;
    // bNonblocking indicates if the socket is in non-blocking mode
    BOOL                   bNonblocking = FALSE;

    // localname is the local address
    SOCKADDR_IN            localname;
    // remotename is the remote address
    SOCKADDR_IN            remotename;

    // readfds is the set of sockets to check for a read condition
    fd_set                 readfds;

    // SendBuffer10 is the send buffer
    char                   SendBuffer10[BUFFER_10_LEN + 1];
    // SendBufferLarge is the large send buffer
    char                   SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // RecvBuffer10 is the recv buffer
    char                   RecvBuffer10[BUFFER_10_LEN + 1];
    // dwHow specifies how the socket is shutdown
    DWORD                  dwHow;

    // hSendEvent is a handle to the overlapped send event
    HANDLE                 hSendEvent;
    // WSASendOverlapped is the overlapped send structure
    WSAOVERLAPPED          WSASendOverlapped;
    // WSASendBuf is the WSABUF structure
    WSABUF                 WSASendBuf;
    // dwSendBytes is the number of bytes transferred for the function call
    DWORD                  dwSendBytes;
    // dwSendFlags is the transfer flags for the function call
    DWORD                  dwSendFlags;

    // hReceiveEvent is a handle to the overlapped receive event
    HANDLE                 hReceiveEvent;
    // WSAReceiveOverlapped is the overlapped receive structure
    WSAOVERLAPPED          WSAReceiveOverlapped;
    // WSAReceiveBuf is the WSABUF structure
    WSABUF                 WSAReceiveBuf;
    // dwReceiveBytes is the number of bytes transferred for the function call
    DWORD                  dwReceiveBytes;
    // dwReceiveFlags is the transfer flags for the function call
    DWORD                  dwReceiveFlags;

    // bReturnCode is the return code of WSAGetOverlappedResult
    BOOL                   bReturnCode;

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
    sprintf(szFunctionName, "shutdown v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_shutdown+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_shutdown-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = shutdownTestSessionXbox;
    }
    else {
        NetsyncTypeSession = shutdownTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < shutdownTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, shutdownTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, shutdownTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != shutdownTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, shutdownTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == shutdownTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != shutdownTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == shutdownTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = shutdownTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == shutdownTable[dwTableIndex].bNetsyncConnected)) {
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

        // Create the overlapped event
        hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        hReceiveEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

        // Setup the overlapped structure
        ZeroMemory(&WSASendOverlapped, sizeof(WSASendOverlapped));
        WSASendOverlapped.hEvent = hSendEvent;

        ZeroMemory(&WSAReceiveOverlapped, sizeof(WSAReceiveOverlapped));
        WSAReceiveOverlapped.hEvent = hReceiveEvent;

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        if (SOCKET_INT_MIN == shutdownTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == shutdownTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == shutdownTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == shutdownTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == shutdownTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & shutdownTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & shutdownTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & shutdownTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        if ((0 != (SOCKET_TCP & shutdownTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & shutdownTable[dwTableIndex].dwSocket))) {
            // Set the buffer size
            dwBufferSize = 10;
            setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

            dwBufferSize = 10;
            setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

            // Bind the socket
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

            if ((FALSE == shutdownTable[dwTableIndex].bServerAccept) && (TRUE == shutdownTable[dwTableIndex].bServerConnect)) {
                listen(sSocket, SOMAXCONN);
            }
        }

        if ((TRUE == shutdownTable[dwTableIndex].bServerConnect) || (TRUE == shutdownTable[dwTableIndex].bServerAccept)) {
            // Initialize the shutdown request
            shutdownRequest.dwMessageId = SHUTDOWN_REQUEST_MSG;
            if (0 != (SOCKET_TCP & shutdownTable[dwTableIndex].dwSocket)) {
                shutdownRequest.nSocketType = SOCK_STREAM;
            }
            else {
                shutdownRequest.nSocketType = SOCK_DGRAM;
            }
            shutdownRequest.Port = CurrentPort;
            shutdownRequest.bServerConnect = shutdownTable[dwTableIndex].bServerConnect;
            shutdownRequest.bServerFirstSend = shutdownTable[dwTableIndex].bServerFirstSend;
            shutdownRequest.bServerSecondSend = shutdownTable[dwTableIndex].bServerSecondSend;

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(shutdownRequest), (char *) &shutdownRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            // Connect the socket
            if (TRUE == shutdownTable[dwTableIndex].bServerAccept) {
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = NetsyncInAddr;
                remotename.sin_port = htons(CurrentPort);

                connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
            }
            else {
                nsSocket = accept(sSocket, NULL, NULL);
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(shutdownRequest), (char *) &shutdownRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            // Start the pending overlapped operation
            if ((0 != (SOCKET_TCP & shutdownTable[dwTableIndex].dwSocket)) && (0 != (SHUTDOWN_SEND & shutdownTable[dwTableIndex].dwHow1))) {
                // Fill the queue
                Nonblock = 1;
                ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);

                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                while (SOCKET_ERROR != send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0)) {
                    Sleep(SLEEP_ZERO_TIME);
                }

                Nonblock = 0;
                ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);

                // Call WSASend
                ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                WSASendBuf.len = 10;
                WSASendBuf.buf = SendBuffer10;
                dwSendBytes = 0;

                iReturnCode = WSASend((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSASendBuf, 1, &dwSendBytes, 0, &WSASendOverlapped, NULL);

                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "WSASend returned non-SOCKET_ERROR");
                }
                else if (WSA_IO_PENDING != WSAGetLastError()) {
                    xLog(hLog, XLL_FAIL, "WSASend iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, WSAGetLastError());
                }
            }

            if ((FALSE == shutdownTable[dwTableIndex].bServerFirstSend) && (0 != (SHUTDOWN_RECEIVE & shutdownTable[dwTableIndex].dwHow1))) {
                // Call WSARecv
                ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                WSAReceiveBuf.len = sizeof(RecvBuffer10);
                WSAReceiveBuf.buf = RecvBuffer10;
                dwReceiveBytes = 0;
                dwReceiveFlags = 0;

                iReturnCode = WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSAReceiveBuf, 1, &dwReceiveBytes, &dwReceiveFlags, &WSAReceiveOverlapped, NULL);

                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "WSARecv returned non-SOCKET_ERROR");
                }
                else if (WSA_IO_PENDING != WSAGetLastError()) {
                    xLog(hLog, XLL_FAIL, "WSARecv iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, WSAGetLastError());
                }
            }

            if (TRUE == shutdownTable[dwTableIndex].bServerFirstSend) {
                ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBuffer10, sizeof(RecvBuffer10), 0);

                FD_ZERO(&readfds);
                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                select(0, &readfds, NULL, NULL, NULL);
            }
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & shutdownTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        __try {
            if (SHUTDOWN_SEND == shutdownTable[dwTableIndex].dwHow1) {
                iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SD_SEND);
            }
            else if (SHUTDOWN_RECEIVE == shutdownTable[dwTableIndex].dwHow1) {
                iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SD_RECEIVE);
            }
            else if (SHUTDOWN_BOTH == shutdownTable[dwTableIndex].dwHow1) {
                iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SD_BOTH);
            }
            else if (SHUTDOWN_INT_MIN == shutdownTable[dwTableIndex].dwHow1) {
                iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, INT_MIN);
            }
            else if (SHUTDOWN_NEG_ONE == shutdownTable[dwTableIndex].dwHow1) {
                iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, -1);
            }
            else if (SHUTDOWN_INT_MAX == shutdownTable[dwTableIndex].dwHow1) {
                iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, INT_MAX);
            }
            else if (SHUTDOWN_THREE == shutdownTable[dwTableIndex].dwHow1) {
                iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, 3);
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == shutdownTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "shutdown RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "shutdown caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == shutdownTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "shutdown did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == shutdownTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != shutdownTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "shutdown1 iLastError - EXPECTED: %u; RECEIVED: %u", shutdownTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "shutdown1 iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "shutdown1 returned SOCKET_ERROR - ec = %u", WSAGetLastError());
            }
            else if (SOCKET_ERROR == shutdownTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "shutdown1 returned non-SOCKET_ERROR");
            }
            else {
                dwHow = shutdownTable[dwTableIndex].dwHow1;

                if (0 != (SHUTDOWN_RECEIVE & dwHow)) {
                    if (FALSE == shutdownTable[dwTableIndex].bServerFirstSend) {
                        bReturnCode = WSAGetOverlappedResult((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSAReceiveOverlapped, &dwReceiveBytes, TRUE, &dwReceiveFlags);

                        if (FALSE != bReturnCode) {
                            xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult receive returned TRUE");
                            bTestPassed = FALSE;
                        }
                        else if (WSAESHUTDOWN != WSAGetLastError()) {
                            xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult receive iLastError - EXPECTED: %u; RECEIVED: %u", WSAESHUTDOWN, WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                    }

                    ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                    iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBuffer10, sizeof(RecvBuffer10), 0);

                    if (SOCKET_ERROR != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "recv1a returned non-SOCKET_ERROR");
                        bTestPassed = FALSE;
                    }
                    else if ((0 != (SOCKET_TCP & shutdownTable[dwTableIndex].dwSocket)) && (TRUE == shutdownTable[dwTableIndex].bServerFirstSend)) {
                        if (WSAECONNRESET != WSAGetLastError()) {
                            xLog(hLog, XLL_FAIL, "recv1a iLastError - EXPECTED: %u; RECEIVED: %u", WSAECONNRESET, WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                    }
                    else {
                        if (WSAESHUTDOWN != WSAGetLastError()) {
                            xLog(hLog, XLL_FAIL, "recv1a iLastError - EXPECTED: %u; RECEIVED: %u", WSAESHUTDOWN, WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                    }
                }
                else if (TRUE == shutdownTable[dwTableIndex].bServerFirstSend) {
                    ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                    iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBuffer10, sizeof(RecvBuffer10), 0);

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "recv1a returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                    }
                }

                if (0 != (SHUTDOWN_SEND & dwHow)) {
                    if (0 != (SOCKET_TCP & shutdownTable[dwTableIndex].dwSocket)) {
                        bReturnCode = WSAGetOverlappedResult((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSASendOverlapped, &dwSendBytes, TRUE, &dwSendFlags);

                        if (FALSE != bReturnCode) {
                            xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult send returned TRUE");
                            bTestPassed = FALSE;
                        }
                        else if ((SHUTDOWN_BOTH == dwHow) && (TRUE == shutdownTable[dwTableIndex].bServerFirstSend)) {
                            if (WSAECONNRESET != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult send iLastError - EXPECTED: %u; RECEIVED: %u", WSAECONNRESET, WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                        }
                        else {
                            if (WSAESHUTDOWN != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult send iLastError - EXPECTED: %u; RECEIVED: %u", WSAESHUTDOWN, WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                        }
                    }

                    ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                    iReturnCode = send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBuffer10, 10, 0);
                    if (SOCKET_ERROR != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "send1a returned non-SOCKET_ERROR");
                        bTestPassed = FALSE;
                    }
                    else if ((0 != (SOCKET_TCP & shutdownTable[dwTableIndex].dwSocket)) && (SHUTDOWN_BOTH == dwHow) && (TRUE == shutdownTable[dwTableIndex].bServerFirstSend)) {
                        if (WSAECONNRESET != WSAGetLastError()) {
                            xLog(hLog, XLL_FAIL, "send1a iLastError - EXPECTED: %u; RECEIVED: %u", WSAECONNRESET, WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                    }
                    else {
                        if (WSAESHUTDOWN != WSAGetLastError()) {
                            xLog(hLog, XLL_FAIL, "send1a iLastError - EXPECTED: %u; RECEIVED: %u", WSAESHUTDOWN, WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                    }
                }
                else if ((0 != (SOCKET_TCP & shutdownTable[dwTableIndex].dwSocket)) && (TRUE == shutdownTable[dwTableIndex].bServerFirstSend) && (0 != (SHUTDOWN_RECEIVE & dwHow))) {
                    ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                    iReturnCode = send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBuffer10, 10, 0);
                    if (SOCKET_ERROR != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "send1a returned non-SOCKET_ERROR");
                        bTestPassed = FALSE;
                    }
                    else if (WSAECONNRESET != WSAGetLastError()) {
                        xLog(hLog, XLL_FAIL, "send1a iLastError - EXPECTED: %u; RECEIVED: %u", WSAECONNRESET, WSAGetLastError());
                        bTestPassed = FALSE;
                    }
                }
                else {
                    ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                    iReturnCode = send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBuffer10, 10, 0);
                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "send1a returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                    }
                }

                if ((0 != (SHUTDOWN_RECEIVE & dwHow)) && (TRUE == shutdownTable[dwTableIndex].bServerSecondSend)) {
                    // Send the second send request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(shutdownRequest), (char *) &shutdownRequest);

                    // Wait for the second send complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                    iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBuffer10, sizeof(RecvBuffer10), 0);

                    if (SOCKET_ERROR != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "recv1b returned non-SOCKET_ERROR");
                        bTestPassed = FALSE;
                    }
                    else if (WSAECONNRESET != WSAGetLastError()) {
                        xLog(hLog, XLL_FAIL, "recv1b iLastError - EXPECTED: %u; RECEIVED: %u", WSAECONNRESET, WSAGetLastError());
                        bTestPassed = FALSE;
                    }

                    ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                    iReturnCode = send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBuffer10, 10, 0);
                    if (SOCKET_ERROR != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "send1b returned non-SOCKET_ERROR");
                        bTestPassed = FALSE;
                    }
                    else if (WSAECONNRESET != WSAGetLastError()) {
                        xLog(hLog, XLL_FAIL, "send1b iLastError - EXPECTED: %u; RECEIVED: %u", WSAECONNRESET, WSAGetLastError());
                        bTestPassed = FALSE;
                    }
                }

                if (0 != shutdownTable[dwTableIndex].dwHow2) {
                    if (SHUTDOWN_SEND == shutdownTable[dwTableIndex].dwHow2) {
                        iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SD_SEND);
                    }
                    else if (SHUTDOWN_RECEIVE == shutdownTable[dwTableIndex].dwHow2) {
                        iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SD_RECEIVE);
                    }
                    else if (SHUTDOWN_BOTH == shutdownTable[dwTableIndex].dwHow2) {
                        iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SD_BOTH);
                    }

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "shutdown2 returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                    }
                    else {
                        dwHow |= shutdownTable[dwTableIndex].dwHow2;

                        if (0 != (SHUTDOWN_RECEIVE & dwHow)) {
                            ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBuffer10, sizeof(RecvBuffer10), 0);

                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv2 returned non-SOCKET_ERROR");
                                bTestPassed = FALSE;
                            }
                            else if (WSAESHUTDOWN != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "recv2 iLastError - EXPECTED: %u; RECEIVED: %u", WSAESHUTDOWN, WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                        }
                        else if (TRUE == shutdownTable[dwTableIndex].bServerFirstSend) {
                            ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBuffer10, sizeof(RecvBuffer10), 0);

                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv2 returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                        }

                        ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                        iReturnCode = send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBuffer10, 10, 0);
                        if (0 != (SHUTDOWN_SEND & dwHow)) {
                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "send2 returned non-SOCKET_ERROR");
                                bTestPassed = FALSE;
                            }
                            else if (WSAESHUTDOWN != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "send2 iLastError - EXPECTED: %u; RECEIVED: %u", WSAESHUTDOWN, WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                        }
                        else {
                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "send2 returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                        }
                    }
                }

                if (0 != shutdownTable[dwTableIndex].dwHow3) {
                    if (SHUTDOWN_SEND == shutdownTable[dwTableIndex].dwHow3) {
                        iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SD_SEND);
                    }
                    else if (SHUTDOWN_RECEIVE == shutdownTable[dwTableIndex].dwHow3) {
                        iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SD_RECEIVE);
                    }
                    else if (SHUTDOWN_BOTH == shutdownTable[dwTableIndex].dwHow3) {
                        iReturnCode = shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SD_BOTH);
                    }

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "shutdown3 returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                    }
                    else {
                        dwHow |= shutdownTable[dwTableIndex].dwHow3;

                        if (0 != (SHUTDOWN_RECEIVE & dwHow)) {
                            ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBuffer10, sizeof(RecvBuffer10), 0);

                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv3 returned non-SOCKET_ERROR");
                                bTestPassed = FALSE;
                            }
                            else if (WSAESHUTDOWN != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "recv3 iLastError - EXPECTED: %u; RECEIVED: %u", WSAESHUTDOWN, WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                        }
                        else if (TRUE == shutdownTable[dwTableIndex].bServerFirstSend) {
                            ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBuffer10, sizeof(RecvBuffer10), 0);

                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv3 returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                        }

                        ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                        iReturnCode = send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBuffer10, 10, 0);
                        if (0 != (SHUTDOWN_SEND & dwHow)) {
                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "send3 returned non-SOCKET_ERROR");
                                bTestPassed = FALSE;
                            }
                            else if (WSAESHUTDOWN != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "send3 iLastError - EXPECTED: %u; RECEIVED: %u", WSAESHUTDOWN, WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                        }
                        else {
                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "send3 returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "shutdown succeeded");
                }
            }
        }

        if ((TRUE == shutdownTable[dwTableIndex].bServerConnect) || (TRUE == shutdownTable[dwTableIndex].bServerAccept)) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(shutdownRequest), (char *) &shutdownRequest);
        }

        // Close the sockets
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & shutdownTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & shutdownTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & shutdownTable[dwTableIndex].dwSocket))) {
                shutdown(sSocket, SD_BOTH);
                closesocket(sSocket);
            }
        }

        // Close the event
        CloseHandle(hReceiveEvent);
        CloseHandle(hSendEvent);

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

    // Terminate net subsystem if necessary
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
shutdownTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests shutdown - Server side

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
    u_long             FromInAddr;
    // dwMessageType is the type of received message
    DWORD              dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD              dwMessageSize;
    // pMessage is a pointer to the received message
    char               *pMessage;
    // shutdownRequest is the request
    SHUTDOWN_REQUEST   shutdownRequest;
    // shutdownComplete is the result
    SHUTDOWN_COMPLETE  shutdownComplete;

    // sSocket is the socket descriptor
    SOCKET             sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET             nsSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD              dwBufferSize = BUFFER_10_LEN;
    // bNagle indicates if Nagle is enabled
    BOOL               bNagle = FALSE;

    // localname is the local address
    SOCKADDR_IN        localname;
    // remotename is the remote address
    SOCKADDR_IN        remotename;

    // SendBuffer10 is the send buffer
    char               SendBuffer10[BUFFER_10_LEN + 1];
    // nSendCount is a counter to enumerate each send
    int                nSendCount;



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
        CopyMemory(&shutdownRequest, pMessage, sizeof(shutdownRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, shutdownRequest.nSocketType, 0);

        if (SOCK_STREAM == shutdownRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the buffer size
        dwBufferSize = 1;
        setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

        dwBufferSize = 1;
        setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(shutdownRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if ((SOCK_STREAM == shutdownRequest.nSocketType) && (FALSE == shutdownRequest.bServerConnect)) {
            // Place the socket in listening mode
            listen(sSocket, SOMAXCONN);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(shutdownRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        shutdownComplete.dwMessageId = SHUTDOWN_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(shutdownComplete), (char *) &shutdownComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if ((SOCK_STREAM == shutdownRequest.nSocketType) && (FALSE == shutdownRequest.bServerConnect)) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        // Send the complete
        shutdownComplete.dwMessageId = SHUTDOWN_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(shutdownComplete), (char *) &shutdownComplete);

        // Fulfill the send operations
        if (TRUE == shutdownRequest.bServerFirstSend) {
            for (nSendCount = 0; nSendCount < 10; nSendCount++) {
                // Call send
                ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBuffer10, 10, 0);
            }
        }

        if (TRUE == shutdownRequest.bServerSecondSend) {
            // Wait for the request
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
            while (SOCKET_ERROR != send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBuffer10, 10, 0)) {
                Sleep(SLEEP_ZERO_TIME);
            }

            // Send the complete
            shutdownComplete.dwMessageId = SHUTDOWN_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(shutdownComplete), (char *) &shutdownComplete);
        }

        // Wait for the ack
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        // Close the sockets
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (INVALID_SOCKET != sSocket) {
            shutdown(sSocket, SD_BOTH);
            closesocket(sSocket);
        }
    }
}

#endif
