/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  netsync.c

Abstract:

  This module synchronizes test machines

Author:

  Steven Kehrli (steveke) 15-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace NetsyncNamespace;

namespace NetsyncNamespace {

#ifndef _XBOX

u_long
GetHostAddr(
    IN HANDLE  hMemObject
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Gets the host address

Arguments:

  hMemObject - Handle to the memory object

Return Value:

  u_long:
    If the function succeeds, the return value is the host address.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // hostname is the host name
    char            *hostname = NULL;
    // hostaddr is the host address
    u_long          hostaddr = 0;
    // hostnamelen is the length of the host name buffer
    int             hostnamelen = 0x10;
    // hostent is a pointer to the returned host information
    struct hostent  *hostent = NULL;
    // iResultCode is the result code from gethostname()
    int             iResultCode = 0;
    // iErrorCode is the last error code
    int             iErrorCode = ERROR_SUCCESS;



    do {
        if (NULL == hostname) {
            // Allocate the memory for the standard host name
            hostname = (char *) xMemAlloc(hMemObject, hostnamelen);
        }
        else {
            // Increase the memory for the standard host name and reallocate
            hostnamelen += 0x10;
            hostname = (char *) xMemReAlloc(hMemObject, hostname, hostnamelen);
        }

        if (NULL == hostname) {
            // Get the last error code
            iErrorCode = GetLastError();

            goto FunctionFailed0;
        }

        iResultCode = gethostname(hostname, hostnamelen);
    } while ((SOCKET_ERROR == iResultCode) && (WSAEFAULT == WSAGetLastError()));

    if (SOCKET_ERROR == iResultCode) {
        // Get the last error code
        iErrorCode = WSAGetLastError();

        goto FunctionFailed0;
    }

    // Get the host information for the local machine
    hostent = gethostbyname(hostname);
    if (NULL == hostent) {
        // Get the last error code
        iErrorCode = WSAGetLastError();

        goto FunctionFailed0;
    }

    // Check the type of the address
    if (AF_INET != hostent->h_addrtype) {
        // Set the last error code
        iErrorCode = WSAEFAULT;

        goto FunctionFailed0;
    }

    // Check the size of the address
    if (sizeof(hostaddr) != hostent->h_length) {
        // Set the last error code
        iErrorCode = WSAEFAULT;

        goto FunctionFailed0;
    }

    // Get the host address for the local machine
    CopyMemory(&hostaddr, hostent->h_addr, sizeof(hostaddr));

FunctionFailed0:
    if (NULL != hostname) {
        // Free the standard host name
        xMemFree(hMemObject, hostname);
    }

    if (ERROR_SUCCESS != iErrorCode) {
        // Set the last error code
        SetLastError(iErrorCode);
    }

    return hostaddr;
}

#endif



PNETSYNC_OBJECT
CreateNetsyncObject(
    IN HANDLE   hMemObject,
    IN u_long   ServerAddr,
    IN u_short  NetsyncPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Creates a NETSYNC_OBJECT

Arguments:

  hMemObject - Handle to the memory object
  ServerAddr - Specifies the server address
  NetsyncPort - Specifies the port used by the socket for netsync communication

Return Value:

  HANDLE:
    If the function succeeds, the return value is a pointer to the NETSYNC_OBJECT.
    If the function fails, the return value is NULL.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is the pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT     pNetsyncObject = NULL;
    // bBroadcast indicates the control socket is enabled to send broadcast data
    BOOL                bBroadcast = TRUE;
#ifdef _XBOX
    // nResult is the int status code returned by a function
    INT                 nResult = 0;
#else
    // RpcStatus is the rpc status code returned by a function
    RPC_STATUS          RpcStatus = RPC_S_OK;
#endif
    // dwResult is the result of the address acquisition
    DWORD               dwResult = 0;
    // localname is the local address
    struct sockaddr_in  localname = { AF_INET, 0, 0, 0 };
    // iErrorCode is the last error code
    int                 iErrorCode = ERROR_SUCCESS;



    // Increment the net ref count
    if (FALSE == AddRefNet()) {
        iErrorCode = GetLastError();

        goto FunctionFailed0;
    }

    // Allocate the memory for the NETSYNC_OBJECT
    pNetsyncObject = (PNETSYNC_OBJECT) xMemAlloc(hMemObject, sizeof(NETSYNC_OBJECT));
    if (NULL == pNetsyncObject) {
        iErrorCode = GetLastError();

        goto FunctionFailed1;
    }

    // Set the object type
    pNetsyncObject->dwObjectType = NETSYNC_OBJECT_TYPE;

    // Set the memory object
    pNetsyncObject->hMemObject = hMemObject;

#ifdef _XBOX
    // Create the UUID
    nResult = XNetRandom((BYTE *) &pNetsyncObject->ObjectUUID, sizeof(pNetsyncObject->ObjectUUID));
    if (0 != nResult) {
        // Get the last error code
        iErrorCode = nResult;

        goto FunctionFailed2;
    }
#else
    // Create the UUID
    RpcStatus = UuidCreate(&pNetsyncObject->ObjectUUID);
    if (RPC_S_OK != RpcStatus) {
        // Set the last error code
        iErrorCode = (DWORD) RpcStatus;

        goto FunctionFailed2;
    }
#endif

#ifdef _XBOX
    // Get the xnet address
    do {
        dwResult = XNetGetTitleXnAddr(&pNetsyncObject->XnAddr);
        if (0 == dwResult) {
            Sleep(1000);
        }
    } while (0 == dwResult);

    if (0 == (XNET_GET_XNADDR_ETHERNET & dwResult)) {
        // Get the local address
        pNetsyncObject->InAddr = pNetsyncObject->XnAddr.ina.s_addr;
    }
#else
    // Get the local address
    pNetsyncObject->InAddr = GetHostAddr(pNetsyncObject->hMemObject);
#endif

    // Set the server address
    if (0 == ServerAddr) {
        pNetsyncObject->ServerAddr = pNetsyncObject->InAddr;
    }
    else {
        pNetsyncObject->ServerAddr = ServerAddr;
    }

    // Create the session socket descriptor
    pNetsyncObject->sSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (INVALID_SOCKET == pNetsyncObject->sSocket) {
        iErrorCode = WSAGetLastError();

        goto FunctionFailed2;
    }

    // Enable the control socket to send broadcast data
    if (SOCKET_ERROR == setsockopt(pNetsyncObject->sSocket, SOL_SOCKET, SO_BROADCAST, (char *) &bBroadcast, sizeof(bBroadcast))) {
        iErrorCode = WSAGetLastError();

        goto FunctionFailed3;
    }

    // Setup the local address
    localname.sin_port = htons(NetsyncPort);

    // Bind the session socket
    if (SOCKET_ERROR == bind(pNetsyncObject->sSocket, (SOCKADDR *) &localname, sizeof(localname))) {
        iErrorCode = WSAGetLastError();

        goto FunctionFailed3;
    }

    // Copy the port
    pNetsyncObject->NetsyncPort = NetsyncPort;

    // Create the receive event
    pNetsyncObject->hReceiveEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == pNetsyncObject->hReceiveEvent) {
        iErrorCode = GetLastError();

        goto FunctionFailed3;
    }

    // Create the semaphores
    pNetsyncObject->hServerSemaphore = CreateSemaphore(NULL, 0, 0xFFFF, NULL);
    if (NULL == pNetsyncObject->hServerSemaphore) {
        iErrorCode = GetLastError();

        goto FunctionFailed4;
    }

    pNetsyncObject->hClientSemaphore = CreateSemaphore(NULL, 0, 0xFFFF, NULL);
    if (NULL == pNetsyncObject->hClientSemaphore) {
        iErrorCode = GetLastError();

        goto FunctionFailed5;
    }

    // Create the receive thread
    pNetsyncObject->hReceiveThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ReceiveThread, pNetsyncObject, CREATE_SUSPENDED, NULL);
    if (NULL == pNetsyncObject->hReceiveThread) {
        iErrorCode = GetLastError();

        goto FunctionFailed6;
    }

    // Initialize the critical sections
    InitializeCriticalSection(&pNetsyncObject->csClientList);
    InitializeCriticalSection(&pNetsyncObject->csSocket);
    InitializeCriticalSection(&pNetsyncObject->csServerQueue);
    InitializeCriticalSection(&pNetsyncObject->csClientQueue);

    // Initialize the client list
    pNetsyncObject->pClientList = NULL;

    // Initialize the message queues
    pNetsyncObject->pHeadServerQueue = NULL;
    pNetsyncObject->pTailServerQueue = NULL;
    pNetsyncObject->pHeadClientQueue = NULL;
    pNetsyncObject->pTailClientQueue = NULL;

    // Allow the receive thread to run now that the NETSYNC_OBJECT is initialized
    ResumeThread(pNetsyncObject->hReceiveThread);

    return pNetsyncObject;

FunctionFailed6:
    // Close the semaphore
    CloseHandle(pNetsyncObject->hClientSemaphore);

FunctionFailed5:
    // Close the semaphore
    CloseHandle(pNetsyncObject->hServerSemaphore);

FunctionFailed4:
    // Close the event
    CloseHandle(pNetsyncObject->hReceiveEvent);

FunctionFailed3:
    // Close the socket
    closesocket(pNetsyncObject->sSocket);

FunctionFailed2:
    // Free the NETSYNC_OBJECT
    xMemFree(hMemObject, pNetsyncObject);

FunctionFailed1:
    // Decrement the net ref count
    ReleaseNet();

FunctionFailed0:
    // Set the last error code
    SetLastError(iErrorCode);

    return NULL;
}



VOID
CloseNetsyncObject(
    IN PNETSYNC_OBJECT  pNetsyncObject
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Frees the NETSYNC_OBJECT

Arguments:

  pNetsyncObject - Pointer to the NETSYNC_OBJECT

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // GenericMessage is the end session message
    NETSYNC_GENERIC_MESSAGE  GenericMessage;
    // FromInAddr is the source ip address
    u_long                   FromInAddr;
    // FromXnAddr is the source xnet address
    XNADDR                   FromXnAddr;
    // dwMessageSize is the size of the receive message
    DWORD                    dwMessageSize;
    // pMessage is a pointer to a received message, either query or connect
    char                     *pMessage;
    // pNextClient is a pointer to the next client in the client list
    PNETSYNC_CLIENT_OBJECT   pNextClient = NULL;



    // Assert pNetsyncObject is valid
    ASSERT((INVALID_HANDLE_VALUE != pNetsyncObject) && (NULL != pNetsyncObject) && (NETSYNC_OBJECT_TYPE == pNetsyncObject->dwObjectType));

    // Send the shutdown complete message
    GenericMessage.dwMessageId = NETSYNC_MSG_STOPRECEIVE;
    NetsyncSendServerMessage(pNetsyncObject, htonl(INADDR_LOOPBACK), pNetsyncObject->NetsyncPort, TRUE, sizeof(GenericMessage), (char *) &GenericMessage);

    // Wait for the receive thread to exit
    WaitForSingleObject(pNetsyncObject->hReceiveThread, INFINITE);

    // Flush the client message queue
    while (WAIT_OBJECT_0 == NetsyncReceiveClientMessage(pNetsyncObject, 0, &FromInAddr, &FromXnAddr, &dwMessageSize, &pMessage)) {
        // Free the message
        NetsyncFreeMessage(pMessage);
    }

    // Flush the server message queue
    while (WAIT_OBJECT_0 == NetsyncReceiveServerMessage(pNetsyncObject, 0, &FromInAddr, &FromXnAddr, &dwMessageSize, &pMessage)) {
        // Free the message
        NetsyncFreeMessage(pMessage);
    }

#ifdef _XBOX
    if (TRUE == pNetsyncObject->bXnKey) {
        // Unregister the xnet key id and key
        XNetUnregisterKey(&pNetsyncObject->XnKid);
    }
#endif

    // Wait for access to the critical section
    EnterCriticalSection(&pNetsyncObject->csSocket);

    // Close the socket
    closesocket(pNetsyncObject->sSocket);

    // Release access to the critical section
    LeaveCriticalSection(&pNetsyncObject->csSocket);

    // Wait for access to the client list
    EnterCriticalSection(&pNetsyncObject->csClientList);

    while (NULL != pNetsyncObject->pClientList) {
        // Get the next client
        pNextClient = pNetsyncObject->pClientList->pNextClient;

        // Delete the critical sections
        DeleteCriticalSection(&pNetsyncObject->pClientList->csReceiveSN);
        DeleteCriticalSection(&pNetsyncObject->pClientList->csAckSN);
        DeleteCriticalSection(&pNetsyncObject->pClientList->csSendSN);

        // Free the current client
        xMemFree(pNetsyncObject->hMemObject, pNetsyncObject->pClientList);

        // Set the next client
        pNetsyncObject->pClientList = pNextClient;
    }

    // Release access to the client list
    LeaveCriticalSection(&pNetsyncObject->csClientList);

    // Delete the critical sections
    DeleteCriticalSection(&pNetsyncObject->csClientQueue);
    DeleteCriticalSection(&pNetsyncObject->csServerQueue);
    DeleteCriticalSection(&pNetsyncObject->csSocket);
    DeleteCriticalSection(&pNetsyncObject->csClientList);

    // Close the semaphores
    CloseHandle(pNetsyncObject->hClientSemaphore);
    CloseHandle(pNetsyncObject->hServerSemaphore);

    // Close the receive event
    CloseHandle(pNetsyncObject->hReceiveEvent);

    // Free the NETSYNC_OBJECT
    xMemFree(pNetsyncObject->hMemObject, pNetsyncObject);

    // Decrement the net ref count
    ReleaseNet();
}



PNETSYNC_CLIENT_OBJECT
FindClientObject(
    IN PNETSYNC_OBJECT  pNetsyncObject,
    IN u_long           ClientAddr
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Finds the specified client or creates the client object

Arguments:

  pNetsyncObject - Pointer to the NETSYNC_OBJECT
  ClientAddr - Client address

Return Value:

  PNETSYNC_CLIENT_OBJECT:
    If the function succeeds, the return value is a pointer to the NETSYNC_CLIENT_OBJECT.
    If the function fails, the return value is NULL.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pClient is a pointer to the client
    PNETSYNC_CLIENT_OBJECT  pClient = NULL;
    // iErrorCode is the last error code
    int                     iErrorCode = ERROR_SUCCESS;



    // Wait for access to the client list
    EnterCriticalSection(&pNetsyncObject->csClientList);

    // Get the next client
    pClient = pNetsyncObject->pClientList;

    while (NULL != pClient) {
        // Check the client address
        if (ClientAddr == pClient->ClientAddr) {
            goto ExitFunc;
        }

        // Get the next client
        pClient = pClient->pNextClient;
    }

    // Allocate the memory for the NETSYNC_CLIENT_OBJECT
    pClient = (PNETSYNC_CLIENT_OBJECT) xMemAlloc(pNetsyncObject->hMemObject, sizeof(NETSYNC_CLIENT_OBJECT));
    if (NULL == pClient) {
        iErrorCode = GetLastError();

        goto ExitFunc;
    }

    // Set the client address
    pClient->ClientAddr = ClientAddr;

    // Initialize the critical sections
    InitializeCriticalSection(&pClient->csSendSN);
    InitializeCriticalSection(&pClient->csAckSN);
    InitializeCriticalSection(&pClient->csReceiveSN);

    // Add the client object to the client list
    pClient->pNextClient = pNetsyncObject->pClientList;
    pNetsyncObject->pClientList = pClient;

ExitFunc:
    if (ERROR_SUCCESS != iErrorCode) {
        // Set the last error code
        SetLastError(iErrorCode);
    }

    // Release access to the client list
    LeaveCriticalSection(&pNetsyncObject->csClientList);

    return pClient;
}

} // namespace NetsyncNamespace
