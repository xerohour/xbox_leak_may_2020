/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  wsacanceloverlappedio.c

Abstract:

  This modules tests WSACancelOverlappedIO

Author:

  Steven Kehrli (steveke) 6-Feb-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// WSACancelOverlappedIO messages

#define WSACANCELOVERLAPPEDIO_REQUEST_MSG   NETSYNC_MSG_USER + 250 + 1
#define WSACANCELOVERLAPPEDIO_COMPLETE_MSG  NETSYNC_MSG_USER + 250 + 2

typedef struct _WSACANCELOVERLAPPEDIO_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  Port;
    BOOL     bServerSend;
    BOOL     bServerReceive;
    int      nQueueLen;
} WSACANCELOVERLAPPEDIO_REQUEST, *PWSACANCELOVERLAPPEDIO_REQUEST;

typedef struct _WSACANCELOVERLAPPEDIO_COMPLETE {
    DWORD    dwMessageId;
} WSACANCELOVERLAPPEDIO_COMPLETE, *PWSACANCELOVERLAPPEDIO_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

#define WSASEND_FUNCTION             0x00000010
#define WSASENDTO_FUNCTION           0x00000020
#define WSARECV_FUNCTION             0x00000040
#define WSARECVFROM_FUNCTION         0x00000080

typedef struct WSACANCELOVERLAPPEDIO_TABLE {
    CHAR   szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL   bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL   bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD  dwSocket;                                // dwSocket indicates the socket to be created
    DWORD  dwFunction;                              // dwFunction specifies the function to start the pending overlapped operation
    int    iReturnCode;                             // iReturnCode is the return code of WSACancelOverlappedIO
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} WSACANCELOVERLAPPEDIO_TABLE, *PWSACANCELOVERLAPPEDIO_TABLE;

static WSACANCELOVERLAPPEDIO_TABLE WSACancelOverlappedIOTable[] =
{
    { "25.1 Not Initialized",      FALSE, FALSE, SOCKET_INVALID_SOCKET,      0,                                         SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "25.2 s = INT_MIN",          TRUE,  TRUE,  SOCKET_INT_MIN,             0,                                         SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "25.3 s = -1",               TRUE,  TRUE,  SOCKET_NEG_ONE,             0,                                         SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "25.4 s = 0",                TRUE,  TRUE,  SOCKET_ZERO,                0,                                         SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "25.5 s = INT_MAX",          TRUE,  TRUE,  SOCKET_INT_MAX,             0,                                         SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "25.6 s = INVALID_SOCKET",   TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      0,                                         SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "25.7 TCP None",             TRUE,  TRUE,  SOCKET_TCP,                 0,                                         0,            0,                 FALSE },
    { "25.8 TCP Send",             TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION,                          0,            0,                 FALSE },
    { "25.9 TCP SendTo",           TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION,                        0,            0,                 FALSE },
    { "25.10 TCP Recv",            TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION,                          0,            0,                 FALSE },
    { "25.11 TCP RecvFrom",        TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION,                      0,            0,                 FALSE },
    { "25.12 TCP Send/Recv",       TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECV_FUNCTION,       0,            0,                 FALSE },
    { "25.13 TCP Send/RecvFrom",   TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECVFROM_FUNCTION,   0,            0,                 FALSE },
    { "25.14 TCP SendTo/Recv",     TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECV_FUNCTION,     0,            0,                 FALSE },
    { "25.15 TCP SendTo/RecvFrom", TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECVFROM_FUNCTION, 0,            0,                 FALSE },
    { "25.16 UDP None",            TRUE,  TRUE,  SOCKET_UDP,                 0,                                         0,            0,                 FALSE },
    { "25.17 UDP Recv",            TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                          0,            0,                 FALSE },
    { "25.18 UDP RecvFrom",        TRUE,  TRUE,  SOCKET_UDP,                 WSARECVFROM_FUNCTION,                      0,            0,                 FALSE },
    { "25.19 Closed Socket TCP",   TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, 0,                                         SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "25.20 Closed Socket UDP",   TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, 0,                                         SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "25.21 Not Initialized",     FALSE, FALSE, SOCKET_INVALID_SOCKET,      0,                                         SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define WSACancelOverlappedIOTableCount (sizeof(WSACancelOverlappedIOTable) / sizeof(WSACANCELOVERLAPPEDIO_TABLE))

NETSYNC_TYPE_THREAD  WSACancelOverlappedIOTestSessionNt =
{
    1,
    WSACancelOverlappedIOTableCount,
    L"xnetapi_nt.dll",
    "WSACancelOverlappedIOTestServer"
};

NETSYNC_TYPE_THREAD  WSACancelOverlappedIOTestSessionXbox =
{
    1,
    WSACancelOverlappedIOTableCount,
    L"xnetapi_xbox.dll",
    "WSACancelOverlappedIOTestServer"
};



VOID
WSACancelOverlappedIOTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests WSACancelOverlappedIO - Client side

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
    LPSTR                          lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR                          lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD                          dwTableIndex;

    // bWinsockInitialized indicates if Winsock is initialized
    BOOL                           bWinsockInitialized = FALSE;
    // WSAData is the details of the Winsock implementation
    WSADATA                        WSAData;

    // hNetsyncObject is a handle to the netsync object
    HANDLE                         hNetsyncObject = INVALID_HANDLE_VALUE;
    // NetsyncTypeSession is the session type descriptor
    NETSYNC_TYPE_THREAD            NetsyncTypeSession;
    // NetsyncInAddr is the address of the netsync server
    u_long                         NetsyncInAddr = 0;
    // LowPort is the low bound of the netsync port range
    u_short                        LowPort = 0;
    // HighPort is the high bound of the netsync port range
    u_short                        HighPort = 0;
    // CurrentPort is the current port in the netsync port range
    u_short                        CurrentPort = 0;
    // FromInAddr is the address of the netsync sender
    u_long                         FromInAddr;
    // dwMessageSize is the size of the received message
    DWORD                          dwMessageSize;
    // pMessage is a pointer to the received message
    char                           *pMessage;
    // WSACancelOverlappedIORequest is the request sent to the server
    WSACANCELOVERLAPPEDIO_REQUEST  WSACancelOverlappedIORequest;
    
    // sSocket is the socket descriptor
    SOCKET                         sSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD                          dwBufferSize = BUFFER_10_LEN;
    // bNagle indicates if Nagle is enabled
    BOOL                           bNagle = FALSE;
    // Nonblock sets the socket to blocking or non-blocking mode
    u_long                         Nonblock;
    // bNonblocking indicates if the socket is in non-blocking mode
    BOOL                           bNonblocking = FALSE;

    // localname is the local address
    SOCKADDR_IN                    localname;
    // remotename1 is the first remote address
    SOCKADDR_IN                    remotename1;
    // remotename2 is the second remote address
    SOCKADDR_IN                    remotename2;
    // namelen is the size of the address buffer
    int                            namelen;

    // writefds is the set of sockets to check for a write condition
    fd_set                         writefds;

    // SendBuffer10 is the send buffer
    char                           SendBuffer10[BUFFER_10_LEN + 1];
    // SendBufferLarge is the large send buffer
    char                           SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // RecvBuffer10 is the recv buffer
    char                           RecvBuffer10[BUFFER_10_LEN + 1];

    // WSASendBuf is the send WSABUF structure
    WSABUF                         WSASendBuf;
    // WSARecvBuf is the recv WSABUF structure
    WSABUF                         WSARecvBuf;

    // dwSendBytes1 is the number of bytes transferred for the function call
    DWORD                          dwSendBytes1;
    // dwSendBytes2 is the number of bytes transferred for the result call
    DWORD                          dwSendBytes2;
    // dwRecvBytes1 is the number of bytes transferred for the function call
    DWORD                          dwRecvBytes1;
    // dwRecvBytes2 is the number of bytes transferred for the result call
    DWORD                          dwRecvBytes2;

    // dwSendFlags2 is the transfer flags for the result call
    DWORD                          dwSendFlags2;
    // dwRecvFlags1 is the transfer flags for the function call
    DWORD                          dwRecvFlags1;
    // dwRecvFlags2 is the transfer flags for the result call
    DWORD                          dwRecvFlags2;

    // hSendEvent is a handle to the overlapped event
    HANDLE                         hSendEvent;
    // hRecvEvent is a handle to the overlapped event
    HANDLE                         hRecvEvent;

    // WSASendOverlapped is the overlapped structure
    WSAOVERLAPPED                  WSASendOverlapped;
    // WSARecvOverlapped is the overlapped structure
    WSAOVERLAPPED                  WSARecvOverlapped;

    // bException indicates if an exception occurred
    BOOL                           bException;
    // iReturnCode is the return code of the operation
    int                            iReturnCode;
    // iLastError is the error code if the operation failed
    int                            iLastError;
    // bTestPassed indicates if the test passed
    BOOL                           bTestPassed;

    // szFunctionName is the function name
    CHAR                           szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    sprintf(szFunctionName, "WSACancelOver... v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_WSACancelOverlappedIO+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_WSACancelOverlappedIO-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = WSACancelOverlappedIOTestSessionXbox;
    }
    else {
        NetsyncTypeSession = WSACancelOverlappedIOTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < WSACancelOverlappedIOTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, WSACancelOverlappedIOTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, WSACancelOverlappedIOTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != WSACancelOverlappedIOTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, WSACancelOverlappedIOTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == WSACancelOverlappedIOTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != WSACancelOverlappedIOTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == WSACancelOverlappedIOTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = WSACancelOverlappedIOTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == WSACancelOverlappedIOTable[dwTableIndex].bNetsyncConnected)) {
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

        // Create the overlapped events
        hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        hRecvEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

        // Setup the overlapped structures
        ZeroMemory(&WSASendOverlapped, sizeof(WSASendOverlapped));
        WSASendOverlapped.hEvent = hSendEvent;

        ZeroMemory(&WSARecvOverlapped, sizeof(WSARecvOverlapped));
        WSARecvOverlapped.hEvent = hRecvEvent;

        // Create the socket
        sSocket = INVALID_SOCKET;
        if (SOCKET_INT_MIN == WSACancelOverlappedIOTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == WSACancelOverlappedIOTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == WSACancelOverlappedIOTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == WSACancelOverlappedIOTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == WSACancelOverlappedIOTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & WSACancelOverlappedIOTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & WSACancelOverlappedIOTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & WSACancelOverlappedIOTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        if ((0 != (SOCKET_TCP & WSACancelOverlappedIOTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & WSACancelOverlappedIOTable[dwTableIndex].dwSocket))) {
            // Set the buffer size
            dwBufferSize = 10;
            setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

            dwBufferSize = 10;
            setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

            // Set the socket to non-blocking mode
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);

            // Bind the socket
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        if (0 != WSACancelOverlappedIOTable[dwTableIndex].dwFunction) {
            // Initialize the WSACancelOverlappedIO request
            WSACancelOverlappedIORequest.dwMessageId = WSACANCELOVERLAPPEDIO_REQUEST_MSG;
            if (0 != (SOCKET_TCP & WSACancelOverlappedIOTable[dwTableIndex].dwSocket)) {
                WSACancelOverlappedIORequest.nSocketType = SOCK_STREAM;
            }
            else {
                WSACancelOverlappedIORequest.nSocketType = SOCK_DGRAM;
            }
            WSACancelOverlappedIORequest.Port = CurrentPort;
            WSACancelOverlappedIORequest.bServerSend = (0 != (WSARECV_FUNCTION & WSACancelOverlappedIOTable[dwTableIndex].dwFunction)) || (0 != (WSARECVFROM_FUNCTION & WSACancelOverlappedIOTable[dwTableIndex].dwFunction));
            WSACancelOverlappedIORequest.bServerReceive = (0 != (WSASEND_FUNCTION & WSACancelOverlappedIOTable[dwTableIndex].dwFunction)) || (0 != (WSASENDTO_FUNCTION & WSACancelOverlappedIOTable[dwTableIndex].dwFunction));
            WSACancelOverlappedIORequest.nQueueLen = 0;

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSACancelOverlappedIORequest), (char *) &WSACancelOverlappedIORequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            // Connect the socket
            ZeroMemory(&remotename1, sizeof(remotename1));
            remotename1.sin_family = AF_INET;
            remotename1.sin_addr.s_addr = FromInAddr;
            remotename1.sin_port = htons(CurrentPort);

            connect(sSocket, (SOCKADDR *) &remotename1, sizeof(remotename1));

            if (0 != (SOCKET_TCP & WSACancelOverlappedIOTable[dwTableIndex].dwSocket)) {
                FD_ZERO(&writefds);
                FD_SET(sSocket, &writefds);
                select(0, NULL, &writefds, NULL, NULL);
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSACancelOverlappedIORequest), (char *) &WSACancelOverlappedIORequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);
        }

        // Setup the WSABuf structures
        ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
        WSASendBuf.len = BUFFER_TCPSEGMENT_LEN;
        WSASendBuf.buf = SendBufferLarge;

        ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
        WSARecvBuf.len = 10;
        WSARecvBuf.buf = RecvBuffer10;

        // Initialize dwBytes and dwFlags
        dwSendBytes1 = 0;
        dwSendBytes2 = 0;
        dwRecvBytes1 = 0;
        dwRecvBytes2 = 0;

        dwSendFlags2 = 0;
        dwRecvFlags1 = 0;
        dwRecvFlags2 = 0;

        // Start the pending overlapped operation
        if (0 != (WSASEND_FUNCTION & WSACancelOverlappedIOTable[dwTableIndex].dwFunction)) {
            // Fill the queue
            ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
            while (SOCKET_ERROR != send(sSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0)) {
                WSACancelOverlappedIORequest.nQueueLen++;
                Sleep(SLEEP_ZERO_TIME);
            }

            // Call WSASend
            iReturnCode = WSASend(sSocket, &WSASendBuf, 1, &dwSendBytes1, 0, &WSASendOverlapped, NULL);

            if (SOCKET_ERROR != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSASend returned non-SOCKET_ERROR");
            }
            else if (WSA_IO_PENDING != WSAGetLastError()) {
                xLog(hLog, XLL_FAIL, "WSASend iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, WSAGetLastError());
            }
        }
        else if (0 != (WSASENDTO_FUNCTION & WSACancelOverlappedIOTable[dwTableIndex].dwFunction)) {
            // Fill the queue
            ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
            while (SOCKET_ERROR != send(sSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0)) {
                WSACancelOverlappedIORequest.nQueueLen++;
                Sleep(SLEEP_ZERO_TIME);
            }

            // Call WSASendTo
            ZeroMemory(&remotename1, sizeof(remotename1));
            remotename1.sin_family = AF_INET;
            remotename1.sin_addr.s_addr = FromInAddr;
            remotename1.sin_port = htons(CurrentPort);

            iReturnCode = WSASendTo(sSocket, &WSASendBuf, 1, &dwSendBytes1, 0, (SOCKADDR *) &remotename1, sizeof(remotename1), &WSASendOverlapped, NULL);

            if (SOCKET_ERROR != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSASendTo returned non-SOCKET_ERROR");
            }
            else if (WSA_IO_PENDING != WSAGetLastError()) {
                xLog(hLog, XLL_FAIL, "WSASendTo iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, WSAGetLastError());
            }
        }

        if (0 != (WSARECV_FUNCTION & WSACancelOverlappedIOTable[dwTableIndex].dwFunction)) {
            // Call WSARecv
            iReturnCode = WSARecv(sSocket, &WSARecvBuf, 1, &dwRecvBytes1, &dwRecvFlags1, &WSARecvOverlapped, NULL);

            if (SOCKET_ERROR != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSARecv returned non-SOCKET_ERROR");
            }
            else if (WSA_IO_PENDING != WSAGetLastError()) {
                xLog(hLog, XLL_FAIL, "WSARecv iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, WSAGetLastError());
            }
        }
        else if (0 != (WSARECVFROM_FUNCTION & WSACancelOverlappedIOTable[dwTableIndex].dwFunction)) {
            // Call WSARecvFrom
            namelen = sizeof(remotename2);
            ZeroMemory(&remotename2, sizeof(remotename2));

            iReturnCode = WSARecvFrom(sSocket, &WSARecvBuf, 1, &dwRecvBytes1, &dwRecvFlags1, (SOCKADDR *) &remotename2, &namelen, &WSARecvOverlapped, NULL);

            if (SOCKET_ERROR != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSARecvFrom returned non-SOCKET_ERROR");
            }
            else if (WSA_IO_PENDING != WSAGetLastError()) {
                xLog(hLog, XLL_FAIL, "WSARecvFrom iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, WSAGetLastError());
            }
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & WSACancelOverlappedIOTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        __try {
            // Call WSACancelOverlappedIO
            iReturnCode = WSACancelOverlappedIO(sSocket);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == WSACancelOverlappedIOTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "WSACancelOverlappedIO RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "WSACancelOverlappedIO caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (0 != WSACancelOverlappedIOTable[dwTableIndex].dwFunction) {
                // Send the function request
                NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSACancelOverlappedIORequest), (char *) &WSACancelOverlappedIORequest);

                // Wait for the function complete
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                NetsyncFreeMessage(pMessage);
            }

            if (TRUE == WSACancelOverlappedIOTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "WSACancelOverlappedIO did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == WSACancelOverlappedIOTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != WSACancelOverlappedIOTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "WSACancelOverlappedIO iLastError - EXPECTED: %u; RECEIVED: %u", WSACancelOverlappedIOTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "WSACancelOverlappedIO iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSACancelOverlappedIO returned SOCKET_ERROR - ec = %u", WSAGetLastError());
            }
            else if (SOCKET_ERROR == WSACancelOverlappedIOTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSACancelOverlappedIO returned non-SOCKET_ERROR");
            }
            else {
                if ((0 != (WSASEND_FUNCTION & WSACancelOverlappedIOTable[dwTableIndex].dwFunction)) || (0 != (WSASENDTO_FUNCTION & WSACancelOverlappedIOTable[dwTableIndex].dwFunction))) {
                    if (TRUE == WSAGetOverlappedResult(sSocket, &WSASendOverlapped, &dwSendBytes2, TRUE, &dwSendFlags2)) {
                        xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult send returned TRUE");
                        bTestPassed = FALSE;
                    }
                    else if (WSAECANCELLED != WSAGetLastError()) {
                        xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult send iLastError - EXPECTED: %u; RECEIVED: %u", WSAECANCELLED, WSAGetLastError());
                        bTestPassed = FALSE;
                    }
                    else {
                        FD_ZERO(&writefds);
                        FD_SET(sSocket, &writefds);
                        select(0, NULL, &writefds, NULL, NULL);

                        if (0 == FD_ISSET(sSocket, &writefds)) {
                            xLog(hLog, XLL_FAIL, "select writefds returned failure");
                            bTestPassed = FALSE;
                        }
                    }
                }

                if ((0 != (WSARECV_FUNCTION & WSACancelOverlappedIOTable[dwTableIndex].dwFunction)) || (0 != (WSARECVFROM_FUNCTION & WSACancelOverlappedIOTable[dwTableIndex].dwFunction))) {
                    if (TRUE == WSAGetOverlappedResult(sSocket, &WSARecvOverlapped, &dwRecvBytes2, TRUE, &dwRecvFlags2)) {
                        xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult send returned TRUE");
                        bTestPassed = FALSE;
                    }
                    else if (WSAECANCELLED != WSAGetLastError()) {
                        xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult send iLastError - EXPECTED: %u; RECEIVED: %u", WSAECANCELLED, WSAGetLastError());
                        bTestPassed = FALSE;
                    }
                    else if (SOCKET_ERROR == recv(sSocket, RecvBuffer10, sizeof(RecvBuffer10), 0)) {
                        xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "WSACancelOverlappedIO succeeded");
                }
            }
        }

        if (0 != WSACancelOverlappedIOTable[dwTableIndex].dwFunction) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSACancelOverlappedIORequest), (char *) &WSACancelOverlappedIORequest);
        }

        // Close the socket
        if (0 == (SOCKET_CLOSED & WSACancelOverlappedIOTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & WSACancelOverlappedIOTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & WSACancelOverlappedIOTable[dwTableIndex].dwSocket))) {
                shutdown(sSocket, SD_BOTH);
                closesocket(sSocket);
            }
        }

        // Close the event
        CloseHandle(hRecvEvent);
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
WSACancelOverlappedIOTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests WSACancelOverlappedIO - Server side

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
    u_long                          FromInAddr;
    // dwMessageType is the type of received message
    DWORD                           dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD                           dwMessageSize;
    // pMessage is a pointer to the received message
    char                            *pMessage;
    // WSACancelOverlappedIORequest is the request
    WSACANCELOVERLAPPEDIO_REQUEST   WSACancelOverlappedIORequest;
    // WSACancelOverlappedIOComplete is the result
    WSACANCELOVERLAPPEDIO_COMPLETE  WSACancelOverlappedIOComplete;

    // sSocket is the socket descriptor
    SOCKET                          sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                          nsSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD                           dwBufferSize = 1;
    // bNagle indicates if Nagle is enabled
    BOOL                            bNagle = FALSE;
    // nQueueLen is the size of the queue
    int                             nQueueLen;

    // localname is the local address
    SOCKADDR_IN                     localname;
    // remotename is the remote address
    SOCKADDR_IN                     remotename;

    // SendBuffer10 is the send buffer
    char                            SendBuffer10[BUFFER_10_LEN + 1];
    // SendBufferLarge is the large send buffer
    char                            SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // nSendCount is a counter to enumerate each send
    int                             nSendCount;



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
        CopyMemory(&WSACancelOverlappedIORequest, pMessage, sizeof(WSACancelOverlappedIORequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, WSACancelOverlappedIORequest.nSocketType, 0);

        if (SOCK_STREAM == WSACancelOverlappedIORequest.nSocketType) {
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
        localname.sin_port = htons(WSACancelOverlappedIORequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if (SOCK_STREAM == WSACancelOverlappedIORequest.nSocketType) {
            // Place the socket in listening mode
            listen(sSocket, SOMAXCONN);
        }

        // Send the complete
        WSACancelOverlappedIOComplete.dwMessageId = WSACANCELOVERLAPPEDIO_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSACancelOverlappedIOComplete), (char *) &WSACancelOverlappedIOComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if (SOCK_STREAM == WSACancelOverlappedIORequest.nSocketType) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(WSACancelOverlappedIORequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        WSACancelOverlappedIOComplete.dwMessageId = WSACANCELOVERLAPPEDIO_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSACancelOverlappedIOComplete), (char *) &WSACancelOverlappedIOComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        nQueueLen = ((PWSACANCELOVERLAPPEDIO_REQUEST) pMessage)->nQueueLen;
        NetsyncFreeMessage(pMessage);

        // Fulfill the send/recv operations
        if (TRUE == WSACancelOverlappedIORequest.bServerSend) {
            // Call send
            sprintf(SendBuffer10, "%05d%05d", 1, 1);
            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBuffer10, 10, 0);
        }

        if (TRUE == WSACancelOverlappedIORequest.bServerReceive) {
            ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
            for (nSendCount = 0; nSendCount < nQueueLen; nSendCount++) {
                recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0);
            }
        }

        // Send the complete
        WSACancelOverlappedIOComplete.dwMessageId = WSACANCELOVERLAPPEDIO_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSACancelOverlappedIOComplete), (char *) &WSACancelOverlappedIOComplete);

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
