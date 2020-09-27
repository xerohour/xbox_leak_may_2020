/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  server.c

Abstract:

  This module streams data between client and server

Author:

  Steven Kehrli (steveke) 13-Feb-2002

------------------------------------------------------------------------------*/

#include "precomp.h"



namespace XNetStreamNamespace {

typedef struct _TEST_INFO {
    HANDLE  hMemObject;
    SOCKET  sTCPSocket;
    SOCKET  nsTCPSocket;
    SOCKET  sUDPSocket;
} TEST_INFO, *PTEST_INFO;

} // namespace XNetStreamNamespace



using namespace XNetStreamNamespace;

LPVOID
WINAPI
StreamServerStart(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientInAddrs,
    IN XNADDR   *ClientXnAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Start Function - Send and receive data over the network

Arguments:

  hSessionObject - Handle to the session object
  hNetsyncObject - Handle to the netsync object
  byClientCount - Specifies the number of clients in the session
  ClientAddrs - Pointer to an array of client addresses
  ClientXnAddrs - Pointer to an array of client addresses
  LowPort - Specifies the low bound of the netsync port range
  HighPort - Specifies the high bound of the netsync port range

Return Value:

  LPVOID - Pointer to the context

------------------------------------------------------------------------------*/
{
    // hMemObject is the handle to the memory object
    HANDLE       hMemObject = INVALID_HANDLE_VALUE;
    // PTEST_INFO is a pointer to the test info
    PTEST_INFO   pTestInfo = NULL;
    // iTimeout is the send and receive timeout value for the socket
    int          iTimeout;
    // portname is the local address associated with a socket
    SOCKADDR_IN  portname = { AF_INET, 0, 0, {'\0'} };
    // toname is the recipient address information structure
    SOCKADDR_IN  toname = { AF_INET, 0, 0, {'\0'} };



    // Create the memory object
    hMemObject = xMemCreate();
    if (INVALID_HANDLE_VALUE == hMemObject) {
        return NULL;
    }

    // Create the test info structure
    pTestInfo = (PTEST_INFO) xMemAlloc(hMemObject, sizeof(TEST_INFO));
    if (NULL == pTestInfo) {
        // Close the memory object
        xMemClose(hMemObject);

        return NULL;
    }

    // Set the test info structure
    pTestInfo->hMemObject = hMemObject;
    pTestInfo->nsTCPSocket = INVALID_SOCKET;

    // Create a TCP socket
    pTestInfo->sTCPSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Create a UDP socket
    pTestInfo->sUDPSocket = socket(AF_INET, SOCK_DGRAM, 0);

    // Set the receive timeout value to 5 sec
    iTimeout = 5000;
    setsockopt(pTestInfo->sTCPSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));

    iTimeout = 5000;
    setsockopt(pTestInfo->sUDPSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));

    // Set the send timeout value to 5 sec
    iTimeout = 5000;
    setsockopt(pTestInfo->sTCPSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));

    iTimeout = 5000;
    setsockopt(pTestInfo->sUDPSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));

    // Bind TCP socket
    portname.sin_port = htons(LowPort);
    bind(pTestInfo->sTCPSocket, (SOCKADDR *) &portname, sizeof(portname));

    // Bind UDP socket
    portname.sin_port = htons(LowPort);
    bind(pTestInfo->sUDPSocket, (SOCKADDR *) &portname, sizeof(portname));

    // Listen on TCP socket
    listen(pTestInfo->sTCPSocket, 1);

    // Send the session initialized message
    NetsyncSendClientMessage(hNetsyncObject, ClientInAddrs[0], 0, NULL);

    return pTestInfo;
}



BOOL
WINAPI
StreamServerMain(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN u_long   FromInAddr,
    IN XNADDR   *FromXnAddr,
    IN DWORD    dwMessageType,
    IN DWORD    dwMessageSize,
    IN char     *pMessage,
    IN LPVOID   lpContext
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Main Function - Send and receive data over the network

Arguments:

  hSessionObject - Handle to the session object
  hNetsyncObject - Handle to the netsync object
  FromInAddr - Sender address
  FromXnAddr - Sender address
  dwMessageType - Specifies the message type
  dwMessageSize - Specifies the message size
  pMessage - Pointer to the message
  lpContext - Pointer to the context

Return Value:

  BOOL - FALSE on exit

------------------------------------------------------------------------------*/
{
    // PTEST_INFO is a pointer to the test info
    PTEST_INFO   pTestInfo = (PTEST_INFO) lpContext;
    // fromname is the sender/recipient address information structure
    SOCKADDR_IN  fromname = { AF_INET, 0, 0, {'\0'} };
    // fromnamelen is the size of fromname
    int          fromnamelen;
    // RecvBuffer is the receive buffer
    char         RecvBuffer[1024];
    // nBytes is the number of bytes sent or received on the socket
    int          nBytes;



    if (NULL == pTestInfo) {
        return FALSE;
    }

    if (NETSYNC_MSGTYPE_SERVER == dwMessageType) {
        return FALSE;
    }

    if (INVALID_SOCKET == pTestInfo->nsTCPSocket) {
        pTestInfo->nsTCPSocket = accept(pTestInfo->sTCPSocket, NULL, NULL);
    }

    // Receive and send data
    nBytes = recv(pTestInfo->nsTCPSocket, RecvBuffer, sizeof(RecvBuffer), 0);
    send(pTestInfo->nsTCPSocket, RecvBuffer, nBytes, 0);

    fromnamelen = sizeof(fromname);
    nBytes = recvfrom(pTestInfo->sUDPSocket, RecvBuffer, sizeof(RecvBuffer), 0, (SOCKADDR *) &fromname, &fromnamelen);
    sendto(pTestInfo->sUDPSocket, RecvBuffer, nBytes, 0, (SOCKADDR *) &fromname, fromnamelen);

    return TRUE;
}



VOID
WINAPI
StreamServerStop(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN LPVOID   lpContext
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Main Function - Send and receive data over the network

Arguments:

  hSessionObject - Handle to the session object
  hNetsyncObject - Handle to the netsync object
  lpContext - Pointer to the context

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // hMemObject is the handle to the memory object
    HANDLE       hMemObject = INVALID_HANDLE_VALUE;
    // PTEST_INFO is a pointer to the test info
    PTEST_INFO   pTestInfo = (PTEST_INFO) lpContext;



    if (NULL != pTestInfo) {
        // Get the memory object
        hMemObject = pTestInfo->hMemObject;

        // Close the sockets
        shutdown(pTestInfo->sTCPSocket, SD_BOTH);
        shutdown(pTestInfo->nsTCPSocket, SD_BOTH);
        shutdown(pTestInfo->sUDPSocket, SD_BOTH);

        closesocket(pTestInfo->sTCPSocket);
        closesocket(pTestInfo->nsTCPSocket);
        closesocket(pTestInfo->sUDPSocket);

        // Free the test info
        xMemFree(hMemObject, pTestInfo);

        // Close the memory object
        xMemClose(hMemObject);
    }
}
