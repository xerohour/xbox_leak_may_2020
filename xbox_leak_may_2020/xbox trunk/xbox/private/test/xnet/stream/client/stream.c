/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  stream.c

Abstract:

  This module streams data between client and server - library

Author:

  Steven Kehrli (steveke) 13-Feb-2002

------------------------------------------------------------------------------*/

#include "precomp.h"



namespace XNetStreamNamespace {

#define TestBuffer "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

NETSYNC_TYPE_CALLBACK  StreamSessionNt =
{
    1,
    1,
    2,
    L"xboxstream_nt.dll",
    "StreamServerMain",
    "StreamServerStart",
    "StreamServerStop"
};

NETSYNC_TYPE_CALLBACK  StreamSessionXbox =
{
    1,
    1,
    2,
    L"xboxstream_xbox.dll",
    "StreamServerMain",
    "StreamServerStart",
    "StreamServerStop"
};



VOID
StreamTest(
    IN HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Send and receive data over the network

Arguments:

  hLog - handle to the xLog log object

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // uXNetRef is the reference count of xnet
    ULONG        uXNetRef = 0;
    // WSAData is the Winsock startup data
    WSADATA      WSAData;
    // OldTitleXnAddr is the old title XNADDR
    XNADDR       OldTitleXnAddr;
    // NewTitleXnAddr is the new title XNADDR
    XNADDR       NewTitleXnAddr;

    // hNetsyncObject is a handle to the netsync object
    HANDLE       hNetsyncObject = INVALID_HANDLE_VALUE;
    // NetsyncInAddr is the address of the netsync server
    u_long       NetsyncInAddr = 0;
    // LowPort is the low bound of the netsync port range
    u_short      LowPort = 0;
    // HighPort is the high bound of the netsync port range
    u_short      HighPort = 0;
    // FromInAddr is the address of the netsync sender
    u_long       FromInAddr;
    // dwMessageSize is the size of the received message
    DWORD        dwMessageSize;
    // pMessage is a pointer to the received message
    char         *pMessage;

    // sTCPSocket is a TCP socket descriptor
    SOCKET       sTCPSocket = INVALID_SOCKET;
    // sUDPSocket is a UDP socket descriptor
    SOCKET       sUDPSocket = INVALID_SOCKET;
    // iTimeout is the send and receive timeout value for the socket
    int          iTimeout;
    // portname is the local address associated with a socket
    SOCKADDR_IN  portname = { AF_INET, 0, 0, {'\0'} };
    // toname is the recipient address information structure
    SOCKADDR_IN  toname = { AF_INET, 0, 0, {'\0'} };
    // TCPRecvBuffer is the TCP receive buffer
    char         TCPRecvBuffer[sizeof(TestBuffer)];
    // UDPRecvBuffer is the UDP receive buffer
    char         UDPRecvBuffer[sizeof(TestBuffer)];
    // nBytes is the number of bytes sent or received on the socket
    int          nBytes;

    // dwResult is the result of a function call
    DWORD        dwResult;
    // iLastError is the last error code
    int          iLastError;



    // Initialize the net subsystem
    uXNetRef = XNetAddRef();
    if (0 == uXNetRef) {
        xLog(hLog, XLL_FAIL, "XNetAddRef failed - ec: %u", GetLastError());

        goto FunctionExit0;
    }

    while (0 == (dwResult = XNetGetTitleXnAddr(&OldTitleXnAddr))) {
        Sleep(1000);
    }

    // Initialize Winsock
    iLastError = WSAStartup(MAKEWORD(2, 2), &WSAData);
    if (ERROR_SUCCESS != iLastError) {
        xLog(hLog, XLL_FAIL, "WSAStartup failed - ec: %u", iLastError);

        goto FunctionExit1;
    }

    // Connect to the session
    hNetsyncObject = NetsyncCreateClient(0, NETSYNC_SESSION_CALLBACK, &StreamSessionNt, &NetsyncInAddr, NULL, &LowPort, &HighPort);
    if (INVALID_HANDLE_VALUE == hNetsyncObject) {
        xLog(hLog, XLL_BLOCK, "Cannot create Netsync client - ec = %u", GetLastError());

        goto FunctionExit2;
    }

    // Wait for session to initialize
    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
    NetsyncFreeMessage(pMessage);

    // Create a TCP socket
    sTCPSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == sTCPSocket) {
        xLog(hLog, XLL_FAIL, "Cannot create TCP socket - ec = %u", GetLastError());

        goto FunctionExit3;
    }

    // Create a UDP socket
    sUDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (INVALID_SOCKET == sUDPSocket) {
        xLog(hLog, XLL_FAIL, "Cannot create UDP socket - ec = %u", GetLastError());

        goto FunctionExit4;
    }

    // Set the receive timeout value to 5 sec
    iTimeout = 5000;
    if (SOCKET_ERROR == setsockopt(sTCPSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        xLog(hLog, XLL_FAIL, "Cannot set receive timeout value for TCP socket - ec = %u", GetLastError());

        goto FunctionExit5;
    }

    iTimeout = 5000;
    if (SOCKET_ERROR == setsockopt(sUDPSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        xLog(hLog, XLL_FAIL, "Cannot set receive timeout value for UDP socket - ec = %u", GetLastError());

        goto FunctionExit5;
    }

    // Set the send timeout value to 5 sec
    iTimeout = 5000;
    if (SOCKET_ERROR == setsockopt(sTCPSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        xLog(hLog, XLL_FAIL, "Cannot set send timeout value for TCP socket - ec = %u", GetLastError());

        goto FunctionExit5;
    }

    iTimeout = 5000;
    if (SOCKET_ERROR == setsockopt(sUDPSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        xLog(hLog, XLL_FAIL, "Cannot set send timeout value for UDP socket - ec = %u", GetLastError());

        goto FunctionExit5;
    }

    // Bind TCP socket
    portname.sin_port = htons(LowPort);
    if (SOCKET_ERROR == bind(sTCPSocket, (SOCKADDR *) &portname, sizeof(portname))) {
        xLog(hLog, XLL_FAIL, "Cannot bind TCP socket - ec = %u", GetLastError());

        goto FunctionExit5;
    }

    // Bind UDP socket
    portname.sin_port = htons(LowPort);
    if (SOCKET_ERROR == bind(sUDPSocket, (SOCKADDR *) &portname, sizeof(portname))) {
        xLog(hLog, XLL_FAIL, "Cannot bind UDP socket - ec = %u", GetLastError());

        goto FunctionExit5;
    }

    // Connect the TCP socket
    toname.sin_port = htons(LowPort);
    toname.sin_addr.s_addr = NetsyncInAddr;

    if (SOCKET_ERROR == connect(sTCPSocket, (SOCKADDR *) &toname, sizeof(toname))) {
        xLog(hLog, XLL_FAIL, "connect on TCP socket failed - ec = %u", GetLastError());

        goto FunctionExit5;
    }

    // Connect the UDP socket
    toname.sin_port = htons(LowPort);
    toname.sin_addr.s_addr = NetsyncInAddr;

    if (SOCKET_ERROR == connect(sUDPSocket, (SOCKADDR *) &toname, sizeof(toname))) {
        xLog(hLog, XLL_FAIL, "connect on UDP socket failed - ec = %u", GetLastError());

        goto FunctionExit5;
    }

    while (TRUE) {
        // Get XNADDR
        XNetGetTitleXnAddr(&NewTitleXnAddr);
        if (0 != memcmp(&OldTitleXnAddr, &NewTitleXnAddr, sizeof(NewTitleXnAddr))) {
            xLog(hLog, XLL_BREAK, "XNADDR changed");
            break;
        }

        // Pump Netsync
        NetsyncSendClientMessage(hNetsyncObject, 0, 0, NULL);

        // Send Data
        nBytes = send(sTCPSocket, TestBuffer, sizeof(TestBuffer), 0);
        if (SOCKET_ERROR == nBytes) {
            xLog(hLog, XLL_BREAK, "send on TCP socket failed - ec = %u", GetLastError());
        }

        nBytes = send(sUDPSocket, TestBuffer, sizeof(TestBuffer), 0);
        if (SOCKET_ERROR == nBytes) {
            xLog(hLog, XLL_BREAK, "send on UDP socket failed - ec = %u", GetLastError());
        }

        // Receive Data
        nBytes = recv(sTCPSocket, TCPRecvBuffer, sizeof(TCPRecvBuffer), 0);
        if (SOCKET_ERROR == nBytes) {
            xLog(hLog, XLL_BREAK, "recv on TCP socket failed - ec = %u", GetLastError());
        }

        nBytes = recv(sUDPSocket, UDPRecvBuffer, sizeof(UDPRecvBuffer), 0);
        if (SOCKET_ERROR == nBytes) {
            xLog(hLog, XLL_BREAK, "recv on UDP socket failed - ec = %u", GetLastError());
        }

        // Verify Data
        if (0 != memcmp(TestBuffer, TCPRecvBuffer, sizeof(TestBuffer))) {
            xLog(hLog, XLL_BREAK, "Buffer data on TCP socket does not match");
        }

        if (0 != memcmp(TestBuffer, UDPRecvBuffer, sizeof(TestBuffer))) {
            xLog(hLog, XLL_BREAK, "Buffer data on UDP socket does not match");
        }
    }

FunctionExit5:
    // Close the UDP socket
    if (0 != closesocket(sUDPSocket)) {
        xLog(hLog, XLL_FAIL, "Cannot close UDP socket - ec = %u", GetLastError());
    }

FunctionExit4:
    // Close the TCP socket
    if (0 != closesocket(sTCPSocket)) {
        xLog(hLog, XLL_FAIL, "Cannot close TCP socket - ec = %u", GetLastError());
    }

FunctionExit3:
    // Close the netsync client object
    NetsyncCloseClient(hNetsyncObject);

FunctionExit2:
    // Terminate Winsock
    if (0 != WSACleanup()) {
        xLog(hLog, XLL_FAIL, "WSACleanup failed - ec: %u", GetLastError());
    }

FunctionExit1:
    // Terminate the net subsystem
    if ((uXNetRef - 1) != XNetRelease()) {
        xLog(hLog, XLL_FAIL, "XNetRelease failed - ec: %u", GetLastError());
    }

FunctionExit0:
    return;
}

} // namespace XNetStreamNamespace
