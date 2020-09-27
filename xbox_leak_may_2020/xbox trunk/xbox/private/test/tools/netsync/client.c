/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  client.c

Abstract:

  This module handles the netsync client logic

Author:

  Steven Kehrli (steveke) 15-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace NetsyncNamespace;

namespace NetsyncNamespace {

// g_hNetsyncClientInfoMutex is the object to synchronize access to the netsync client info
HANDLE               g_hNetsyncClientInfoMutex;
// g_NetsyncClientInfo is the netsync client info
NETSYNC_CLIENT_INFO  g_NetsyncClientInfo = { INVALID_HANDLE_VALUE, 0 };



PNETSYNC_SERVER
AddNetsyncServer(
    IN HANDLE               hMemObject,
    IN PNETSYNC_SERVER      pServers,
    IN u_long               ServerInAddr,
    IN XNADDR               *ServerXnAddr,
    IN PNETSYNC_ENUM_REPLY  pEnumReply
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Adds a netsync server to the head of a list

Arguments:

  hMemObject - Handle to the memory object
  pServers - Pointer to the head of the netsync server list
  ServerInAddr - ip address of the netsync server
  ServerXnAddr - xnet address of the netsync server
  pEnumReply - Pointer to the netsync server info

Return Value:

  PNETSYNC_SERVER: Pointer to the head of the list of netsync servers

------------------------------------------------------------------------------*/
{
    // pCurrentServer is a pointer to the current server element
    PNETSYNC_SERVER  pCurrentServer = NULL;
    // pNewServer is a pointer to the new server element
    PNETSYNC_SERVER  pNewServer = NULL;



    // Set the current server to the head of the server list
    pCurrentServer = pServers;

    while (NULL != pCurrentServer) {
        if ((0 != ServerInAddr) && (pCurrentServer->InAddr == ServerInAddr)) {
            break;
        }
        else if ((0 == ServerInAddr) && (0 == memcmp(&pCurrentServer->XnAddr, ServerXnAddr, sizeof(pCurrentServer->XnAddr)))) {
            break;
        }

        // Set the current server to the next server
        pCurrentServer = pCurrentServer->pNextServer;
    }

    if (NULL == pCurrentServer) {
        // Allocate the memory for the new server element
        pNewServer = (PNETSYNC_SERVER) xMemAlloc(hMemObject, sizeof(NETSYNC_SERVER));
        if (NULL == pNewServer) {
            goto FunctionExit;
        }
    }
    else {
        pNewServer = pCurrentServer;
    }

    // Setup the new server element
    pNewServer->InAddr = ServerInAddr;
    CopyMemory(&pNewServer->XnAddr, ServerXnAddr, sizeof(pNewServer->XnAddr));
    CopyMemory(&pNewServer->XnKid, &pEnumReply->XnKid, sizeof(pNewServer->XnKid));
    CopyMemory(&pNewServer->XnKey, &pEnumReply->XnKey, sizeof(pNewServer->XnKey));

    pNewServer->bySessionClientCount = pEnumReply->bySessionClientCount;
    pNewServer->nServerClientCount = pEnumReply->nServerClientCount;
    pNewServer->bySessionCount = pEnumReply->bySessionCount;

    if (NULL == pCurrentServer) {
        pNewServer->pNextServer = pServers;
        return pNewServer;
    }

FunctionExit:
    return pServers;
}



PNETSYNC_SERVER
FindNetsyncServer(
    IN PNETSYNC_SERVER  pServers
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Finds a netsync server from the server list

Arguments:

  pServers - Pointer to the head of the netsync server list

Return Value:

  PNETSYNC_SERVER: Pointer to the netsync server

------------------------------------------------------------------------------*/
{
    // pSelectedServer is a pointer to the selected server
    PNETSYNC_SERVER  pSelectedServer = NULL;
    // pCurrentServer is a pointer to the current server
    PNETSYNC_SERVER  pCurrentServer = NULL;



    // Set the selected and current server to the head of the server list
    pCurrentServer = pServers;

    while (NULL != pCurrentServer) {
        if (NULL == pSelectedServer) {
            pSelectedServer = pCurrentServer;
        }
        else {
            if (pCurrentServer->bySessionClientCount == pSelectedServer->bySessionClientCount) {
                if (pCurrentServer->nServerClientCount == pSelectedServer->nServerClientCount) {
                    if (pCurrentServer->bySessionCount < pSelectedServer->bySessionCount) {
                        // Current server has fewer sessions running
                        pSelectedServer = pCurrentServer;
                    }
                }
                else if (pCurrentServer->nServerClientCount < pSelectedServer->nServerClientCount) {
                    // Current server has fewer clients connected
                    pSelectedServer = pCurrentServer;
                }
            }
            else if (pCurrentServer->bySessionClientCount > pSelectedServer->bySessionClientCount) {
                // Current session has more clients connected
                pSelectedServer = pCurrentServer;
            }
        }

        // Set pCurrentServer to the next server in the list
        pCurrentServer = pCurrentServer->pNextServer;
    }

    return pSelectedServer;
}

} // namespace NetsyncNamespace



HANDLE
WINAPI
NetsyncCreateClient(
    IN  u_long   RemoteInAddr,
    IN  DWORD    dwSessionType,
    IN  LPVOID   pSessionType,
    OUT u_long   *ServerInAddr,
    OUT XNADDR   *ServerXnAddr,
    OUT u_short  *LowPort,
    OUT u_short  *HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Creates a netsync client

Arguments:

  RemoteInAddr - Specifies the ip address of the remote server
  dwSessionType - Specifies the type of netsync session
  pSessionType - Pointer to the netsync session information
  ServerInAddr - Specifies the ip address of the netsync server
  ServerXnAddr - Specifies the xnet address of the netsync server
  LowPort - Specifies the low port allocated for the session
  HighPort - Specifies the high port allocated for the session

Return Value:

  HANDLE:
    If the function succeeds, the return value is a handle to the NETSYNC_OBJECT
    If the function fails, the return value is INVALID_HANDLE_VALUE

------------------------------------------------------------------------------*/
{
    // pNetsyncConnectObject is a pointer to the NETSYNC_OBJECT for connect
    PNETSYNC_OBJECT          pNetsyncConnectObject = NULL;
    // pNetsyncObject is a pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT          pNetsyncObject = NULL;
    // hNetsyncObject is the handle to the NETSYNC_OBJECT
    HANDLE                   hNetsyncObject = INVALID_HANDLE_VALUE;
    // SessionMessage is the session message buffer
    NETSYNC_SESSION_MESSAGE  SessionMessage;
    // pCurrentServer is a pointer to the current netsync server
    PNETSYNC_SERVER          pCurrentServer = NULL;
    // pServers is a pointer to the list of netsync servers
    PNETSYNC_SERVER          pServers = NULL;
    // sin_addr is the server in_addr
    IN_ADDR                  sin_addr;

    // ToInAddr is the destination ip address
    u_long                   ToInAddr;
    // FromInAddr is the source ip address
    u_long                   FromInAddr;
    // FromXnAddr is the source xnet address
    XNADDR                   FromXnAddr;
    // dwMessageSize is the size of the receive message
    DWORD                    dwMessageSize;
    // pMessage is a pointer to the received message
    char                     *pMessage = NULL;
    // pEnumSessionReply is a pointer to the enum session reply
    PNETSYNC_ENUM_REPLY      pEnumSessionReply;
    // pConnectSessionReply is a pointer to the connect session reply
    PNETSYNC_CONNECT_REPLY   pConnectSessionReply;

    // dwEnumStartTime is the start time of enumeration
    DWORD                    dwEnumStartTime;
    // dwLoopStartTime is the start time of an enumeration loop
    DWORD                    dwLoopStartTime;
    // dwWaitTime is the wait time of an enumeration loop
    DWORD                    dwWaitTime;
    // dwCurrentTime is the current time
    DWORD                    dwCurrentTime;

    // iErrorCode is the error code
    int                      iErrorCode = ERROR_SUCCESS;



    if (((NETSYNC_SESSION_THREAD != dwSessionType) && (NETSYNC_SESSION_CALLBACK != dwSessionType)) || (NULL == pSessionType)) {
        iErrorCode = ERROR_INVALID_PARAMETER;

        goto FunctionExit;
    }

    // Create the netsync client mutex
    g_hNetsyncClientInfoMutex = CreateMutexA(NULL, FALSE, "NetsyncClientMutex");

    // Wait for the netsync client mutex
    WaitForSingleObject(g_hNetsyncClientInfoMutex, INFINITE);

    if (0 == g_NetsyncClientInfo.dwRefCount) {
        // Create the memory object
        g_NetsyncClientInfo.hMemObject = xMemCreate();
        if (INVALID_HANDLE_VALUE == g_NetsyncClientInfo.hMemObject) {
            iErrorCode = GetLastError();

            goto FunctionExit;
        }
    }

    // Create the NETSYNC_OBJECT
    pNetsyncConnectObject = CreateNetsyncObject(g_NetsyncClientInfo.hMemObject, 0, NETSYNC_PORT);
    if (NULL == pNetsyncConnectObject) {
        iErrorCode = GetLastError();

        goto FunctionExit;
    }

    // Setup the destination addr
    ToInAddr = (0 == RemoteInAddr) ? htonl(INADDR_BROADCAST) : RemoteInAddr;

    // Setup the message buffer
    ZeroMemory(&SessionMessage, sizeof(SessionMessage));
    SessionMessage.dwMessageId = NETSYNC_MSG_ENUM;
    CopyMemory(&SessionMessage.FromObjectUUID, &pNetsyncConnectObject->ObjectUUID, sizeof(SessionMessage.FromObjectUUID));
    SessionMessage.dwSessionType = dwSessionType;

    if (NETSYNC_SESSION_THREAD == dwSessionType) {
        SessionMessage.byMinClientCount = ((PNETSYNC_TYPE_THREAD) pSessionType)->byClientCount;
        SessionMessage.byMaxClientCount = ((PNETSYNC_TYPE_THREAD) pSessionType)->byClientCount;

        SessionMessage.nMinPortCount = ((PNETSYNC_TYPE_THREAD) pSessionType)->nMinPortCount;

        // Copy the dll name
        lstrcpy(SessionMessage.szDllName_W, ((PNETSYNC_TYPE_THREAD) pSessionType)->lpszDllName_W);

        // Copy the main function name
        strcpy(SessionMessage.szMainFunction_A, ((PNETSYNC_TYPE_THREAD) pSessionType)->lpszMainFunction_A);
    }
    else {
        SessionMessage.byMinClientCount = ((PNETSYNC_TYPE_CALLBACK) pSessionType)->byMinClientCount;
        SessionMessage.byMaxClientCount = ((PNETSYNC_TYPE_CALLBACK) pSessionType)->byMaxClientCount;

        SessionMessage.nMinPortCount = ((PNETSYNC_TYPE_CALLBACK) pSessionType)->nMinPortCount;

        // Copy the dll name
        lstrcpy(SessionMessage.szDllName_W, ((PNETSYNC_TYPE_CALLBACK) pSessionType)->lpszDllName_W);

        // Copy the main, start and stop function names
        strcpy(SessionMessage.szMainFunction_A, ((PNETSYNC_TYPE_CALLBACK) pSessionType)->lpszMainFunction_A);
        strcpy(SessionMessage.szStartFunction_A, ((PNETSYNC_TYPE_CALLBACK) pSessionType)->lpszStartFunction_A);
        strcpy(SessionMessage.szStopFunction_A, ((PNETSYNC_TYPE_CALLBACK) pSessionType)->lpszStopFunction_A);
    }

    // Get the current time
    dwEnumStartTime = GetTickCount();

    do {
        // Get the current time
        dwLoopStartTime = GetTickCount();

        DebugMessage(NETSYNC_DEBUG_CLIENT, "NetsyncCreateClient(): Send Enum Message\n");

        // Send the query
        NetsyncSendServerMessage(pNetsyncConnectObject, ToInAddr, 0, FALSE, sizeof(SessionMessage), (char *) &SessionMessage);

        DebugMessage(NETSYNC_DEBUG_CLIENT, "NetsyncCreateClient(): Wait For Enum Reply\n");

        // Calculate the wait time
        dwCurrentTime = GetTickCount();
        dwWaitTime = ((dwCurrentTime - dwLoopStartTime) < 5000) ? 5000 - (dwCurrentTime - dwLoopStartTime) : 0;

        while (WAIT_OBJECT_0 == NetsyncReceiveServerMessage(pNetsyncConnectObject, dwWaitTime, &FromInAddr, &FromXnAddr, &dwMessageSize, &pMessage)) {
            pEnumSessionReply = (PNETSYNC_ENUM_REPLY) pMessage;
            if ((NETSYNC_REPLY_ENUM == pEnumSessionReply->dwMessageId) && (0 == memcmp(&pEnumSessionReply->FromObjectUUID, &pNetsyncConnectObject->ObjectUUID, sizeof(pEnumSessionReply->FromObjectUUID)))) {
                DebugMessage(NETSYNC_DEBUG_CLIENT, "NetsyncCreateClient(): Received Enum Reply\n");

                // Add the server to the server list
                pServers = AddNetsyncServer(pNetsyncConnectObject->hMemObject, pServers, (0 == pEnumSessionReply->InAddr) ? 0 : FromInAddr, &FromXnAddr, pEnumSessionReply);
            }

            // Free the message
            NetsyncFreeMessage(pMessage);
            pMessage = NULL;

            // Calculate the wait time
            dwCurrentTime = GetTickCount();
            dwWaitTime = ((dwCurrentTime - dwLoopStartTime) < 5000) ? 5000 - (dwCurrentTime - dwLoopStartTime) : 0;
        }
    } while ((NULL == pServers) && ((GetTickCount() - dwEnumStartTime) < 60000));

    // Find the netsync server
    pCurrentServer = FindNetsyncServer(pServers);
    if (NULL == pCurrentServer) {
        iErrorCode = GetLastError();

        goto FunctionExit;
    }

    if (NULL != ServerXnAddr) {
        ZeroMemory(ServerXnAddr, sizeof(*ServerXnAddr));
    }
#ifdef _XBOX
    if (0 == pCurrentServer->InAddr) {
        // Copy the xnet key id and key
        CopyMemory(&pNetsyncConnectObject->XnKid, &pCurrentServer->XnKid, sizeof(pNetsyncConnectObject->XnKid));
        CopyMemory(&pNetsyncConnectObject->XnKey, &pCurrentServer->XnKey, sizeof(pNetsyncConnectObject->XnKey));

        // Register the xnet key id and key
        XNetRegisterKey(&pCurrentServer->XnKid, &pCurrentServer->XnKey);
        pNetsyncConnectObject->bXnKey = TRUE;

        // Get the server address
        XNetXnAddrToInAddr(&pCurrentServer->XnAddr, &pCurrentServer->XnKid, &sin_addr);
        *ServerInAddr = sin_addr.s_addr;
        if (NULL != ServerXnAddr) {
            CopyMemory(ServerXnAddr, &pCurrentServer->XnAddr, sizeof(*ServerXnAddr));
        }
    }
    else {
        *ServerInAddr = pCurrentServer->InAddr;
    }
#else
    // Get the server address
    *ServerInAddr = pCurrentServer->InAddr;
#endif

    // Setup the message buffer
    SessionMessage.dwMessageId = NETSYNC_MSG_CONNECT;

    DebugMessage(NETSYNC_DEBUG_CLIENT, "NetsyncCreateClient(): Send Connect Message To 0x%08x\n", *ServerInAddr);

    // Send the message
    if (WAIT_OBJECT_0 != NetsyncSendServerMessage(pNetsyncConnectObject, *ServerInAddr, 0, TRUE, sizeof(SessionMessage), (char *) &SessionMessage)) {
        iErrorCode = GetLastError();

        DebugMessage(NETSYNC_DEBUG_CLIENT, "NetsyncCreateClient(): Connect Message Failed - ec: 0x%08x\n", iErrorCode);

        goto FunctionExit;
    }

    DebugMessage(NETSYNC_DEBUG_CLIENT, "NetsyncCreateClient(): Wait For Connect Reply\n");

    while (WAIT_OBJECT_0 == NetsyncReceiveServerMessage(pNetsyncConnectObject, INFINITE, &FromInAddr, &FromXnAddr, &dwMessageSize, &pMessage)) {
        pConnectSessionReply = (PNETSYNC_CONNECT_REPLY) pMessage;
        if ((FromInAddr == *ServerInAddr) && ((NETSYNC_MSG_STOPSESSION == pConnectSessionReply->dwMessageId) || (NETSYNC_REPLY_CONNECT == pConnectSessionReply->dwMessageId))) {
            break;
        }

        // Free the message
        NetsyncFreeMessage(pMessage);
        pMessage = NULL;
    }

    if (NULL == pMessage) {
        iErrorCode = GetLastError();

        goto FunctionExit;
    }

    if (NETSYNC_MSG_STOPSESSION == pConnectSessionReply->dwMessageId) {
        iErrorCode = ERROR_SERVER_DISABLED;

        goto FunctionExit;
    }

    DebugMessage(NETSYNC_DEBUG_CLIENT, "NetsyncCreateClient(): Received Connect Reply From 0x%08x - ec: 0x%08x\n", FromInAddr, pConnectSessionReply->dwErrorCode);

    if (ERROR_SUCCESS != pConnectSessionReply->dwErrorCode) {
        iErrorCode = pConnectSessionReply->dwErrorCode;

        goto FunctionExit;
    }

    // Create the NETSYNC_OBJECT
    pNetsyncObject = CreateNetsyncObject(g_NetsyncClientInfo.hMemObject, *ServerInAddr, pConnectSessionReply->LowPort - 1);
    if (NULL == pNetsyncObject) {
        iErrorCode = GetLastError();

        goto FunctionExit;
    }

#ifdef _XBOX
    if (0 == pCurrentServer->InAddr) {
        // Copy the xnet key id and key
        CopyMemory(&pNetsyncObject->XnKid, &pCurrentServer->XnKid, sizeof(pNetsyncObject->XnKid));
        CopyMemory(&pNetsyncObject->XnKey, &pCurrentServer->XnKey, sizeof(pNetsyncObject->XnKey));
        pNetsyncConnectObject->bXnKey = FALSE;
        pNetsyncObject->bXnKey = TRUE;
    }
#endif

    // Copy the low and high port
    if (NULL != LowPort) {
        *LowPort = pConnectSessionReply->LowPort;
    }
    if (NULL != HighPort) {
        *HighPort = pConnectSessionReply->HighPort;
    }

    // Set hNetsyncObject
    hNetsyncObject = (HANDLE) pNetsyncObject;

FunctionExit:
    if (NULL != pMessage) {
        // Free the message
        NetsyncFreeMessage(pMessage);
    }

    // Set the current server to the head of the server list
    pCurrentServer = pServers;

    while (NULL != pCurrentServer) {
        // Set the head of the server list to the next server in the list
        pServers = pServers->pNextServer;

        // Free the current server
        xMemFree(pNetsyncConnectObject->hMemObject, pCurrentServer);

        // Set the current server to the head of the server list
        pCurrentServer = pServers;
    }

    // Free the NETSYNC_OBJECT for connect, if necessary
    if (NULL != pNetsyncConnectObject) {
        CloseNetsyncObject(pNetsyncConnectObject);
    }

    if (INVALID_HANDLE_VALUE != hNetsyncObject) {
        // Increment the reference count
        g_NetsyncClientInfo.dwRefCount++;
    }
    else if (0 == g_NetsyncClientInfo.dwRefCount){
        // Close the memory object
        xMemClose(g_NetsyncClientInfo.hMemObject);
        g_NetsyncClientInfo.hMemObject = INVALID_HANDLE_VALUE;
    }

    // Release the netsync client mutex
    ReleaseMutex(g_hNetsyncClientInfoMutex);

    if (ERROR_SUCCESS != iErrorCode) {
        // Set the last error code
        SetLastError(iErrorCode);
    }

    return hNetsyncObject;
}




VOID
WINAPI
NetsyncCloseClient(
    IN HANDLE  hNetsyncObject
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Closes the netsync client

Arguments:

  hNetsyncObject - Handle to the NETSYNC_OBJECT

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is the pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT          pNetsyncObject = (PNETSYNC_OBJECT) hNetsyncObject;
    // GenericMessage is the delete client message
    NETSYNC_GENERIC_MESSAGE  GenericMessage;



    // Assert hNetsyncObject is valid
    ASSERT((INVALID_HANDLE_VALUE != hNetsyncObject) && (NULL != hNetsyncObject) && (NETSYNC_OBJECT_TYPE == pNetsyncObject->dwObjectType));

    // Setup the delete client message
    GenericMessage.dwMessageId = NETSYNC_MSG_DELETECLIENT;

    // Send the delete client message
    NetsyncSendServerMessage(hNetsyncObject, 0, pNetsyncObject->NetsyncPort, TRUE, sizeof(GenericMessage), (char *) &GenericMessage);

    // Free the NETSYNC_OBJECT
    CloseNetsyncObject(pNetsyncObject);

    // Create the netsync client mutex
    g_hNetsyncClientInfoMutex = CreateMutexA(NULL, FALSE, "NetsyncClientMutex");

    // Wait for the netsync client mutex
    WaitForSingleObject(g_hNetsyncClientInfoMutex, INFINITE);

    // Ref count must be at least 1
    ASSERT(0 < g_NetsyncClientInfo.dwRefCount);

    // Decrement the reference count
    g_NetsyncClientInfo.dwRefCount--;

    if (0 == g_NetsyncClientInfo.dwRefCount) {
        // Close the memory object
        xMemClose(g_NetsyncClientInfo.hMemObject);
        g_NetsyncClientInfo.hMemObject = INVALID_HANDLE_VALUE;
    }
}
