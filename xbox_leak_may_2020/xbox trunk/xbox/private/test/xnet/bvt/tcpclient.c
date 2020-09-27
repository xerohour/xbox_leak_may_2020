/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  tcpclient.c

Abstract:

  This modules tests TCP

Author:

  Steven Kehrli (steveke) 18-Oct-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetBVTNamespace;
 
namespace XNetBVTNamespace {

// TCP messages

typedef struct _BVT_TCP_MESSAGE {
    DWORD    dwMessageId;
    u_short  Port;
    int      iLastError;
} BVT_TCP_MESSAGE, *PBVT_TCP_MESSAGE;

#define BVT_TCP_SEND_MSG  NETSYNC_MSG_USER + 1
#define BVT_TCP_RECV_MSG  NETSYNC_MSG_USER + 2



NETSYNC_TYPE_THREAD  BVT_TCPSessionNt =
{
    1,
    4,
    L"xnetbvt_nt.dll",
    "BVT_TCPTestServer"
};

NETSYNC_TYPE_THREAD  BVT_TCPSessionXbox =
{
    1,
    4,
    L"xnetbvt_xbox.dll",
    "BVT_TCPTestServer"
};

} // namespace XNetBVTNamespace



#ifdef XNETBVT_CLIENT

namespace XNetBVTNamespace {

BOOL
BVT_TCPTest(
    IN HANDLE  hLog,
    IN LPSTR   pNetsyncRemote
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests TCP - Client side

Arguments:

  hLog - handle to the xLog log object
  pNetsyncRemote - pointer to the netsync remote name

Return Value:

  TRUE on success

------------------------------------------------------------------------------*/
{
    // iLastError is the last error code
    int                  iLastError = ERROR_SUCCESS;
    // iResult is the result of a function
    int                  iResult = 0;
    // uXNetRef is the reference count of xnet
    ULONG                uXNetRef = 0;
    // XnAddr is the title xnet address
    XNADDR               XnAddr;
    // dwResult is the result of XNetGetTitleXnAddr
    DWORD                dwResult = 0;
    
    // WSAData is the details of the Winsock implementation
    WSADATA              WSAData;
    // hNetsyncObject is a handle to the netsync object
    HANDLE               hNetsyncObject = INVALID_HANDLE_VALUE;
    // NetsyncTypeSession is the session type descriptor
    NETSYNC_TYPE_THREAD  NetsyncTypeSession;
    // NetsyncInAddr is the address of the netsync server
    u_long               NetsyncInAddr = 0;
    // LowPort is the low bound of the netsync port range
    u_short              LowPort = 0;
    // HighPort is the high bound of the netsync port range
    u_short              HighPort = 0;

    // sSocket1 is a socket descriptor
    SOCKET               sSocket1 = INVALID_SOCKET;
    // sSocket2 is a socket descriptor
    SOCKET               sSocket2 = INVALID_SOCKET;
    // sSocket3 is a socket descriptor
    SOCKET               sSocket3 = INVALID_SOCKET;
    // nSocket3 is a connected socket descriptor
    SOCKET               nsSocket3 = INVALID_SOCKET;
    // sSocket4 is a socket descriptor
    SOCKET               sSocket4 = INVALID_SOCKET;
    // nSocket4 is a connected socket descriptor
    SOCKET               nsSocket4 = INVALID_SOCKET;
    // Port1 is the port number of sSocket1
    u_short              Port1 = 0;
    // Port2 is the port number of sSocket2
    u_short              Port2 = 0;
    // Port3 is the port number of sSocket3
    u_short              Port3 = 0;
    // Port4 is the port number of sSocket4
    u_short              Port4 = 0;

    // iTimeout is the send and receive timeout value for the socket
    int                  iTimeout;
    // portname is the local address associated with a socket
    SOCKADDR_IN          portname = { AF_INET, 0, 0, {'\0'} };
    // toname is the recipient address information structure
    SOCKADDR_IN          toname = { AF_INET, 0, 0, {'\0'} };
    // fromname is the sender address information structure
    SOCKADDR_IN          fromname = { AF_INET, 0, 0, {'\0'} };
    // fromnamelen is the sizeof of the fromname buffer
    int                  fromnamelen = sizeof(fromname);

    // readfds is the set of sockets to check for a pending connection
    fd_set               readfds;
    // timeout is the timeout of select
    timeval              timeout;
    // ReceiveBuffer is the receive buffer
    char                 ReceiveBuffer[sizeof(BVTBuffer)];
    // nBytes is the number of bytes sent or received on the socket
    int                  nBytes;

    // FromInAddr is the address of the netsync sender
    u_long               FromInAddr;
    // dwMessageSize is the size of the received message
    DWORD                dwMessageSize;
    // pMessage is a pointer to the received message
    char                 *pMessage;
    // BVT_TCPMessage is the test message sent to the netsync server
    BVT_TCP_MESSAGE      BVT_TCPMessage;



    // Set the function name
    xSetFunctionName(hLog, "BVT TCP");

    // Start the variation
    xStartVariation(hLog, "BVT TCP Test");



    // Initialize the net subsystem
    uXNetRef = XNetAddRef();
    if (0 == uXNetRef) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "XNetAddRef failed - ec: %u", iLastError);

        goto FunctionExit0;
    }

    while (0 == (dwResult = XNetGetTitleXnAddr(&XnAddr))) {
        Sleep(1000);
    }

    // Determine the remote netsync server type
    if (0 != (XNET_GET_XNADDR_ETHERNET & dwResult)) {
        NetsyncTypeSession = BVT_TCPSessionXbox;
    }
    else {
        NetsyncTypeSession = BVT_TCPSessionNt;
    }

    // Initialize Winsock
    iLastError = WSAStartup(MAKEWORD(2, 2), &WSAData);
    if (ERROR_SUCCESS != iLastError) {
        xLog(hLog, XLL_FAIL, "WSAStartup failed - ec: %u", iLastError);

        goto FunctionExit1;
    }



    // Connect to the session
    hNetsyncObject = NetsyncCreateClient((NULL != pNetsyncRemote) ? inet_addr(pNetsyncRemote) : 0, NETSYNC_SESSION_THREAD, &NetsyncTypeSession, &NetsyncInAddr, NULL, &LowPort, &HighPort);
    if (INVALID_HANDLE_VALUE == hNetsyncObject) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_BLOCK, "Cannot create Netsync client - ec = %u", iLastError);
    }
    else {
        Port1 = LowPort;
        Port2 = LowPort + 1;
        Port3 = LowPort + 2;
        Port4 = LowPort + 3;
    }

    if (INVALID_HANDLE_VALUE == hNetsyncObject) {
        goto FunctionExit2;
    }



    // Create socket
    sSocket1 = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == sSocket1) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot create socket 1 - ec = %u", iLastError);

        goto FunctionExit3;
    }

    sSocket2 = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == sSocket2) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot create socket 2 - ec = %u", iLastError);

        goto FunctionExit4;
    }

    sSocket3 = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == sSocket3) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot create socket 3 - ec = %u", iLastError);

        goto FunctionExit5;
    }

    sSocket4 = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == sSocket4) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot create socket 4 - ec = %u", iLastError);

        goto FunctionExit6;
    }



    // Set the receive timeout value to 5 sec
    iTimeout = 5000;
    if (SOCKET_ERROR == setsockopt(sSocket1, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot set receive timeout value for socket 1 - ec = %u", iLastError);

        goto FunctionExit7;
    }

    iTimeout = 5000;
    if (SOCKET_ERROR == setsockopt(sSocket2, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot set receive timeout value for socket 2 - ec = %u", iLastError);

        goto FunctionExit7;
    }

    iTimeout = 5000;
    if (SOCKET_ERROR == setsockopt(sSocket3, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot set receive timeout value for socket 3 - ec = %u", iLastError);

        goto FunctionExit7;
    }

    iTimeout = 5000;
    if (SOCKET_ERROR == setsockopt(sSocket4, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot set receive timeout value for socket 4 - ec = %u", iLastError);

        goto FunctionExit7;
    }



    // Set the send timeout value to 5 sec
    iTimeout = 5000;
    if (SOCKET_ERROR == setsockopt(sSocket1, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot set send timeout value for socket 1 - ec = %u", iLastError);

        goto FunctionExit7;
    }

    iTimeout = 5000;
    if (SOCKET_ERROR == setsockopt(sSocket2, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot set send timeout value for socket 2 - ec = %u", iLastError);

        goto FunctionExit7;
    }

    iTimeout = 5000;
    if (SOCKET_ERROR == setsockopt(sSocket3, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot set send timeout value for socket 3 - ec = %u", iLastError);

        goto FunctionExit7;
    }

    iTimeout = 5000;
    if (SOCKET_ERROR == setsockopt(sSocket4, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot set send timeout value for socket 4 - ec = %u", iLastError);

        goto FunctionExit7;
    }



    // Bind socket
    portname.sin_port = htons(Port1);
    if (SOCKET_ERROR == bind(sSocket1, (SOCKADDR *) &portname, sizeof(portname))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot bind socket 1 - ec = %u", iLastError);

        goto FunctionExit7;
    }

    portname.sin_port = htons(Port2);
    if (SOCKET_ERROR == bind(sSocket2, (SOCKADDR *) &portname, sizeof(portname))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot bind socket 2 - ec = %u", iLastError);

        goto FunctionExit7;
    }

    portname.sin_port = htons(Port3);
    if (SOCKET_ERROR == bind(sSocket3, (SOCKADDR *) &portname, sizeof(portname))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot bind socket 3 - ec = %u", iLastError);

        goto FunctionExit7;
    }

    portname.sin_port = htons(Port4);
    if (SOCKET_ERROR == bind(sSocket4, (SOCKADDR *) &portname, sizeof(portname))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot bind socket 4 - ec = %u", iLastError);

        goto FunctionExit7;
    }



    // Listen on socket
    if (SOCKET_ERROR == listen(sSocket3, 1)) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot listen on socket 3 - ec = %u", iLastError);

        goto FunctionExit7;
    }

    if (SOCKET_ERROR == listen(sSocket4, 1)) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot listen on socket 4 - ec = %u", iLastError);

        goto FunctionExit7;
    }



    // Start the variation
    xStartVariation(hLog, "Send Test 1");

    // Start the send test
    BVT_TCPMessage.dwMessageId = BVT_TCP_SEND_MSG;
    BVT_TCPMessage.Port = Port1;
    BVT_TCPMessage.iLastError = ERROR_SUCCESS;
    if (WAIT_OBJECT_0 != NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(BVT_TCPMessage), (char *) &BVT_TCPMessage)) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "NetsyncSendClientMessage for socket 1 failed - ec = %u", iLastError);

        goto FunctionExit7;
    }

    // Wait for the ack
    if (WAIT_OBJECT_0 != NetsyncReceiveClientMessage(hNetsyncObject, 15000, &FromInAddr, NULL, &dwMessageSize, &pMessage)) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "NetsyncReceiveClientMessage for socket 1 failed - ec = %u", iLastError);

        goto FunctionExit7;
    }
    NetsyncFreeMessage(pMessage);

    if (ERROR_SUCCESS != BVT_TCPMessage.iLastError) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "Netsync Server failed - ec = %u", iLastError);

        goto FunctionExit7;
    }

    // Initialize connect
    toname.sin_port = htons(Port1);
    toname.sin_addr.s_addr = NetsyncInAddr;

    // Connect on socket 1
    if (SOCKET_ERROR == connect(sSocket1, (SOCKADDR *) &toname, sizeof(toname))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "connect on socket 1 failed - ec = %u", iLastError);
    }
    else {
        // Send first on socket 1
        nBytes = send(sSocket1, BVTBuffer, sizeof(BVTBuffer), 0);
        if (SOCKET_ERROR == nBytes) {
            // Get the last error code
            iLastError = WSAGetLastError();

            xLog(hLog, XLL_FAIL, "send on socket 1 failed - ec = %u", iLastError);
        }
        else {
            if (sizeof(BVTBuffer) != nBytes) {
                xLog(hLog, XLL_FAIL, "send return value on socket 1 - EXPECTED: %d, RECEIVED: %d", sizeof(BVTBuffer), nBytes);
            }

            // Initialize receive
            ZeroMemory(ReceiveBuffer, sizeof(ReceiveBuffer));

            // Receive on socket 1
            nBytes = recv(sSocket1, ReceiveBuffer, sizeof(ReceiveBuffer), 0);
            if (SOCKET_ERROR == nBytes) {
                // Get the last error code
                iLastError = WSAGetLastError();

                xLog(hLog, XLL_FAIL, "recv on socket 1 failed - ec = %u", iLastError);
            }
            else {
                if (sizeof(BVTBuffer) != nBytes) {
                    xLog(hLog, XLL_FAIL, "recv return value on socket 1 - EXPECTED: %d; RECEIVED: %d", sizeof(BVTBuffer), nBytes);
                }
                else if (0 != strcmp(BVTBuffer, ReceiveBuffer)) {
                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer on socket 1 - %s", ReceiveBuffer);
                }
                else {
                    xLog(hLog, XLL_PASS, "send and recv on socket 1 passed");
                }
            }
        }
    }

    // End the variation
    xEndVariation(hLog);



    // Start the variation
    xStartVariation(hLog, "Send Test 2");

    // Start the send test
    BVT_TCPMessage.dwMessageId = BVT_TCP_SEND_MSG;
    BVT_TCPMessage.Port = Port2;
    BVT_TCPMessage.iLastError = ERROR_SUCCESS;
    if (WAIT_OBJECT_0 != NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(BVT_TCPMessage), (char *) &BVT_TCPMessage)) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "NetsyncSendClientMessage for socket 2 failed - ec = %u", iLastError);

        goto FunctionExit7;
    }

    // Wait for the ack
    if (WAIT_OBJECT_0 != NetsyncReceiveClientMessage(hNetsyncObject, 15000, &FromInAddr, NULL, &dwMessageSize, &pMessage)) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "NetsyncReceiveClientMessage for socket 2 failed - ec = %u", iLastError);

        goto FunctionExit7;
    }
    NetsyncFreeMessage(pMessage);

    if (ERROR_SUCCESS != BVT_TCPMessage.iLastError) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "Netsync Server failed - ec = %u", iLastError);

        goto FunctionExit7;
    }

    // Initialize connect
    toname.sin_port = htons(Port2);
    toname.sin_addr.s_addr = NetsyncInAddr;

    // Connect on socket 2
    if (SOCKET_ERROR == connect(sSocket2, (SOCKADDR *) &toname, sizeof(toname))) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "connect on socket 2 failed - ec = %u", iLastError);
    }
    else {
        // Send first on socket 2
        nBytes = send(sSocket2, BVTBuffer, sizeof(BVTBuffer), 0);
        if (SOCKET_ERROR == nBytes) {
            // Get the last error code
            iLastError = WSAGetLastError();

            xLog(hLog, XLL_FAIL, "send on socket 2 failed - ec = %u", iLastError);
        }
        else {
            if (sizeof(BVTBuffer) != nBytes) {
                xLog(hLog, XLL_FAIL, "send return value on socket 2 - EXPECTED: %d, RECEIVED: %d", sizeof(BVTBuffer), nBytes);
            }

            // Initialize receive
            ZeroMemory(ReceiveBuffer, sizeof(ReceiveBuffer));

            // Receive on socket 2
            nBytes = recv(sSocket2, ReceiveBuffer, sizeof(ReceiveBuffer), 0);
            if (SOCKET_ERROR == nBytes) {
                // Get the last error code
                iLastError = WSAGetLastError();

                xLog(hLog, XLL_FAIL, "recv on socket 2 failed - ec = %u", iLastError);
            }
            else {
                if (sizeof(BVTBuffer) != nBytes) {
                    xLog(hLog, XLL_FAIL, "recv return value on socket 2 - EXPECTED: %d; RECEIVED: %d", sizeof(BVTBuffer), nBytes);
                }
                else if (0 != strcmp(BVTBuffer, ReceiveBuffer)) {
                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer on socket 2 - %s", ReceiveBuffer);
                }
                else {
                    xLog(hLog, XLL_PASS, "send and recv on socket 2 passed");
                }
            }
        }
    }

    // End the variation
    xEndVariation(hLog);



    // Start the variation
    xStartVariation(hLog, "Receive Test 1");

    // Start the receive test
    BVT_TCPMessage.dwMessageId = BVT_TCP_RECV_MSG;
    BVT_TCPMessage.Port = Port3;
    BVT_TCPMessage.iLastError = ERROR_SUCCESS;
    if (WAIT_OBJECT_0 != NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(BVT_TCPMessage), (char *) &BVT_TCPMessage)) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "NetsyncSendClientMessage for socket 3 failed - ec = %u", iLastError);

        goto FunctionExit7;
    }

    // Wait for the ack
    if (WAIT_OBJECT_0 != NetsyncReceiveClientMessage(hNetsyncObject, 15000, &FromInAddr, NULL, &dwMessageSize, &pMessage)) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "NetsyncReceiveClientMessage for socket 3 failed - ec = %u", iLastError);

        goto FunctionExit7;
    }
    NetsyncFreeMessage(pMessage);

    if (ERROR_SUCCESS != BVT_TCPMessage.iLastError) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "Netsync Server failed - ec = %u", iLastError);

        goto FunctionExit7;
    }

    // Initialize readfds
    FD_ZERO(&readfds);
    FD_SET(sSocket3, &readfds);

    // Initialize timeout
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // Select on socket 3
    iResult = select(0, &readfds, NULL, NULL, &timeout);
    if (SOCKET_ERROR == iResult) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "select on socket 3 failed - ec = %u", iLastError);
    }
    else if (0 == iResult) {
        xLog(hLog, XLL_FAIL, "select timed out on socket 3");
    }
    else if (0 == FD_ISSET(sSocket3, &readfds)) {
        xLog(hLog, XLL_FAIL, "socket 3 not in readfds");
    }
    else {
        // Accept on socket 3
        fromnamelen = sizeof(fromname);
        nsSocket3 = accept(sSocket3, (SOCKADDR *) &fromname, &fromnamelen);
        if (INVALID_SOCKET == nsSocket3) {
            // Get the last error code
            iLastError = WSAGetLastError();

            xLog(hLog, XLL_FAIL, "accept on socket 3 failed - ec = %u", iLastError);
        }
        else {
            // Initialize receive
            ZeroMemory(ReceiveBuffer, sizeof(ReceiveBuffer));

            // Receive first on socket 3
            nBytes = recv(nsSocket3, ReceiveBuffer, sizeof(ReceiveBuffer), 0);
            if (SOCKET_ERROR == nBytes) {
                // Get the last error code
                iLastError = WSAGetLastError();

                xLog(hLog, XLL_FAIL, "recv on socket 3 failed - ec = %u", iLastError);
            }
            else {
                if (sizeof(BVTBuffer) != nBytes) {
                    xLog(hLog, XLL_FAIL, "recv return value on socket 3 - EXPECTED: %d; RECEIVED: %d", sizeof(BVTBuffer), nBytes);
                }
                else if (0 != strcmp(BVTBuffer, ReceiveBuffer)) {
                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer on socket 3 - %s", ReceiveBuffer);
                }

                // Send on socket 3
                nBytes = send(nsSocket3, ReceiveBuffer, strlen(ReceiveBuffer) + 1, 0);
                if (SOCKET_ERROR == nBytes) {
                    // Get the last error code
                    iLastError = WSAGetLastError();

                    xLog(hLog, XLL_FAIL, "send on socket 3 failed - ec = %u", iLastError);
                }
                else {
                    if ((strlen(ReceiveBuffer) + 1) != (size_t) nBytes) {
                        xLog(hLog, XLL_FAIL, "send return value on socket 3 - EXPECTED: %d: RECEIVED: %d", strlen(ReceiveBuffer) + 1, nBytes);
                    }
                    else {
                        xLog(hLog, XLL_PASS, "recv and send on socket 3 passed");
                    }
                }
            }

            // Close the connected socket
            closesocket(nsSocket3);
            nsSocket3 = INVALID_SOCKET;
        }
    }

    // End the variation
    xEndVariation(hLog);



    // Start the variation
    xStartVariation(hLog, "Receive Test 2");

    // Start the receive test
    BVT_TCPMessage.dwMessageId = BVT_TCP_RECV_MSG;
    BVT_TCPMessage.Port = Port4;
    BVT_TCPMessage.iLastError = ERROR_SUCCESS;
    if (WAIT_OBJECT_0 != NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(BVT_TCPMessage), (char *) &BVT_TCPMessage)) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "NetsyncSendClientMessage for socket 4 failed - ec = %u", iLastError);

        goto FunctionExit7;
    }

    // Wait for the ack
    if (WAIT_OBJECT_0 != NetsyncReceiveClientMessage(hNetsyncObject, 15000, &FromInAddr, NULL, &dwMessageSize, &pMessage)) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "NetsyncReceiveClientMessage for socket 4 failed - ec = %u", iLastError);

        goto FunctionExit7;
    }
    NetsyncFreeMessage(pMessage);

    if (ERROR_SUCCESS != BVT_TCPMessage.iLastError) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "Netsync Server failed - ec = %u", iLastError);

        goto FunctionExit7;
    }

    // Initialize readfds
    FD_ZERO(&readfds);
    FD_SET(sSocket4, &readfds);

    // Initialize timeout
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // Select on socket 4
    iResult = select(0, &readfds, NULL, NULL, &timeout);
    if (SOCKET_ERROR == iResult) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "select on socket 4 failed - ec = %u", iLastError);
    }
    else if (0 == iResult) {
        xLog(hLog, XLL_FAIL, "select timed out on socket 4");
    }
    else if (0 == FD_ISSET(sSocket4, &readfds)) {
        xLog(hLog, XLL_FAIL, "socket 4 not in readfds");
    }
    else {
        // Accept on socket 4
        fromnamelen = sizeof(fromname);
        nsSocket4 = accept(sSocket4, (SOCKADDR *) &fromname, &fromnamelen);
        if (INVALID_SOCKET == nsSocket4) {
            // Get the last error code
            iLastError = WSAGetLastError();

            xLog(hLog, XLL_FAIL, "accept on socket 4 failed - ec = %u", iLastError);
        }
        else {
            // Initialize receive
            ZeroMemory(ReceiveBuffer, sizeof(ReceiveBuffer));

            // Receive first on socket 4
            nBytes = recv(nsSocket4, ReceiveBuffer, sizeof(ReceiveBuffer), 0);
            if (SOCKET_ERROR == nBytes) {
                // Get the last error code
                iLastError = WSAGetLastError();

                xLog(hLog, XLL_FAIL, "recv on socket 4 failed - ec = %u", iLastError);
            }
            else {
                if (sizeof(BVTBuffer) != nBytes) {
                    xLog(hLog, XLL_FAIL, "recv return value on socket 4 - EXPECTED: %d; RECEIVED: %d", sizeof(BVTBuffer), nBytes);
                }
                else if (0 != strcmp(BVTBuffer, ReceiveBuffer)) {
                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer on socket 4 - %s", ReceiveBuffer);
                }

                // Send on socket 4
                nBytes = send(nsSocket4, ReceiveBuffer, strlen(ReceiveBuffer) + 1, 0);
                if (SOCKET_ERROR == nBytes) {
                    // Get the last error code
                    iLastError = WSAGetLastError();

                    xLog(hLog, XLL_FAIL, "send on socket 4 failed - ec = %u", iLastError);
                }
                else {
                    if ((strlen(ReceiveBuffer) + 1) != (size_t) nBytes) {
                        xLog(hLog, XLL_FAIL, "send return value on socket 4 - EXPECTED: %d: RECEIVED: %d", strlen(ReceiveBuffer) + 1, nBytes);
                    }
                    else {
                        xLog(hLog, XLL_PASS, "recv and send on socket 4 passed");
                    }
                }
            }

            // Close the connected socket
            closesocket(nsSocket4);
            nsSocket4 = INVALID_SOCKET;
        }
    }

    // End the variation
    xEndVariation(hLog);



FunctionExit7:
    // Close the socket
    if (0 != closesocket(sSocket4)) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot close socket 4 - ec = %u", iLastError);
    }

FunctionExit6:
    // Close the socket
    if (0 != closesocket(sSocket3)) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot close socket 3 - ec = %u", iLastError);
    }

FunctionExit5:
    // Close the socket
    if (0 != closesocket(sSocket2)) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot close socket 2 - ec = %u", iLastError);
    }

FunctionExit4:
    // Close the socket
    if (0 != closesocket(sSocket1)) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "Cannot close socket 1 - ec = %u", iLastError);
    }

FunctionExit3:
    // Close the netsync client object
    NetsyncCloseClient(hNetsyncObject);

FunctionExit2:
    // Terminate Winsock
    if (0 != WSACleanup()) {
        // Get the last error code
        iLastError = WSAGetLastError();

        xLog(hLog, XLL_FAIL, "WSACleanup failed - ec: %u", iLastError);
    }

FunctionExit1:
    // Terminate the net subsystem
    if ((uXNetRef - 1) != XNetRelease()) {
        // Get the last error code
        iLastError = GetLastError();

        xLog(hLog, XLL_FAIL, "XNetRelease failed - ec: %u", iLastError);
    }

FunctionExit0:
    if (ERROR_SUCCESS == iLastError) {
        xLog(hLog, XLL_PASS, "Test passed");
    }

    // End the variation
    xEndVariation(hLog);

    return (ERROR_SUCCESS == iLastError);
}

} // namespace XNetBVTNamespace



#else



VOID
WINAPI
BVT_TCPTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests TCP - Server side

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
    // sSocket is a socket descriptor
    SOCKET           sSocket = INVALID_SOCKET;
    // nsSocket is a connected socket descriptor
    SOCKET           nsSocket = INVALID_SOCKET;
    // iTimeout is the send and receive timeout value for the socket
    int              iTimeout;
    // portname is the local address associated with a socket
    SOCKADDR_IN      portname = { AF_INET, 0, 0, {'\0'} };
    // toname is the recipient address information structure
    SOCKADDR_IN      toname = { AF_INET, 0, 0, {'\0'} };
    // fromname is the sender address information structure
    SOCKADDR_IN      fromname = { AF_INET, 0, 0, {'\0'} };
    // fromnamelen is the sizeof of the fromname buffer
    int              fromnamelen = sizeof(fromname);

    // readfds is the set of sockets to check for a pending connection
    fd_set           readfds;
    // timeout is the timeout of select
    timeval          timeout;
    // iResult is the result of select
    int              iResult;
    // ReceiveBuffer is the receive buffer
    char             ReceiveBuffer[sizeof(BVTBuffer)];
    // nBytes is the number of bytes sent or received on the socket
    int              nBytes;

    // FromInAddr is the address of the netsync sender
    u_long           FromInAddr;
    // dwMessageType is the type of received message
    DWORD            dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD            dwMessageSize;
    // pMessage is a pointer to the received message
    char             *pMessage;
    // BVT_TCPMessage is the test message sent to the netsync server
    BVT_TCP_MESSAGE  BVT_TCPMessage;



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
        CopyMemory(&BVT_TCPMessage, pMessage, sizeof(BVT_TCPMessage));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (INVALID_SOCKET == sSocket) {
            // Get the last error code
            BVT_TCPMessage.iLastError = WSAGetLastError();

            goto SendAck;
        }
        
        // Bind the socket
        portname.sin_port = htons(BVT_TCPMessage.Port);
        if (SOCKET_ERROR == bind(sSocket, (SOCKADDR *) &portname, sizeof(portname))) {
            // Get the last error code
            BVT_TCPMessage.iLastError = WSAGetLastError();

            goto SendAck;
        }
        
        if (BVT_TCP_SEND_MSG == BVT_TCPMessage.dwMessageId) {
            // Listen on the socket
            if (SOCKET_ERROR == listen(sSocket, 1)) {
                // Get the last error code
                BVT_TCPMessage.iLastError = WSAGetLastError();

                goto SendAck;
            }
        }
        
        // Set the receive timeout value to 5 sec
        iTimeout = 5000;
        if (SOCKET_ERROR == setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
            // Get the last error code
            BVT_TCPMessage.iLastError = WSAGetLastError();

            goto SendAck;
        }

        // Set the send timeout value to 5 sec
        iTimeout = 5000;
        if (SOCKET_ERROR == setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
            // Get the last error code
            BVT_TCPMessage.iLastError = WSAGetLastError();

            goto SendAck;
        }

SendAck:
        // Send the ack
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, dwMessageSize, (char *) &BVT_TCPMessage);

        if (ERROR_SUCCESS == BVT_TCPMessage.iLastError) {
            if (BVT_TCP_SEND_MSG == BVT_TCPMessage.dwMessageId) {
                // Initialize readfds
                FD_ZERO(&readfds);
                FD_SET(sSocket, &readfds);

                // Initialize timeout
                timeout.tv_sec = 5;
                timeout.tv_usec = 0;

                // Select on socket
                iResult = select(0, &readfds, NULL, NULL, &timeout);
                if ((SOCKET_ERROR != iResult) && (0 != iResult) && (0 != FD_ISSET(sSocket, &readfds))) {
                    // Accept on the socket
                    fromnamelen = sizeof(fromname);
                    nsSocket = accept(sSocket, (SOCKADDR *) &fromname, &fromnamelen);
                    if (INVALID_SOCKET != nsSocket) {
                        // Receive the buffer
                        recv(nsSocket, ReceiveBuffer, sizeof(ReceiveBuffer), 0);

                        // Send the buffer
                        send(nsSocket, ReceiveBuffer, strlen(ReceiveBuffer) + 1, 0);

                        // Close the connected socket
                        closesocket(nsSocket);
                        nsSocket = INVALID_SOCKET;
                    }
                }
            }
            else if (BVT_TCP_RECV_MSG == BVT_TCPMessage.dwMessageId) {
                // Connect the socket
                toname.sin_port = htons(BVT_TCPMessage.Port);
                toname.sin_addr.s_addr = FromInAddr;
                if (SOCKET_ERROR != connect(sSocket, (SOCKADDR *) &toname, sizeof(toname))) {
                    // Send the buffer
                    send(sSocket, BVTBuffer, sizeof(BVTBuffer), 0);

                    // Receive the buffer
                    recv(sSocket, ReceiveBuffer, sizeof(ReceiveBuffer), 0);
                }
            }
        }

        if (INVALID_SOCKET != sSocket) {
            // Close the socket
            closesocket(sSocket);
            sSocket = INVALID_SOCKET;
        }
    }
}

#endif // XNETBVT_CLIENT
