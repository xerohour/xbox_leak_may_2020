/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  message.c

Abstract:

  This module handles the netsync message logic

Author:

  Steven Kehrli (steveke) 15-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace NetsyncNamespace;

namespace NetsyncNamespace {

DWORD
NetsyncSendMessageToPort(
    IN HANDLE   hNetsyncObject,
    IN u_long   ToAddr,
    IN u_short  ToPort,
    IN BOOL     bGuarantee,
    IN DWORD    dwMessageId,
    IN DWORD    dwMessageSize,
    IN char     *pMessage
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Sends a netsync message

Arguments:

  hNetsyncObject - Handle to the NETSYNC_OBJECT
  ToAddr - Destination address
  ToPort - Destination port
  bGuarantee - Specifies if the message is guaranteed
  dwMessageId - Specifies the id of the message
  dwMessageSize - Size of the message buffer, in bytes
  pMessage - Pointer to the message buffer

Return Value:

  DWORD:
    If the function succeeds, the return value is WAIT_OBJECT_0.
    If the function times out, the return value is WAIT_TIMEOUT.
    If the function fails, the return value is WAIT_FAILED.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is the pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT          pNetsyncObject = (PNETSYNC_OBJECT) hNetsyncObject;
    // toname is the destination address
    struct sockaddr_in       toname = { AF_INET, 0, 0, 0 };
    // pClient is a pointer to the client object
    PNETSYNC_CLIENT_OBJECT   pClient = NULL;
    // FileTime is the current file time
    FILETIME                 FileTime;
    // SendSN is the new send sequence number
    ULONGLONG                SendSN = 0;
    // hACKEvent is a handle to the ACK event
    HANDLE                   hACKEvent = NULL;
    // pSendMessage is a pointer to the send message buffer
    char                     *pSendMessage = NULL;
    // pNetsyncMessage is the pointer to the netsync message header in the send message buffer
    PNETSYNC_MESSAGE_HEADER  pNetsyncMessage = NULL;
    // dwSendMessageSize is the size of the send message
    DWORD                    dwSendMessageSize = 0;
    // dwBytes is the number of bytes sent on the socket
    DWORD                    dwBytes = 0;
    // dwAttempt is a counter to enumerate each attempt
    DWORD                    dwAttempt = 0;
    // dwNumAttempts is the number of attempts
    DWORD                    dwNumAttempts = 5;
    // dwTimeout specifies the timeout to wait for an ACK
    DWORD                    dwTimeout = 1000;
    // dwWaitResult is the result of the wait on the overlapped I/O event
    DWORD                    dwWaitResult = WAIT_FAILED;
    // iErrorCode is the last error code
    int                      iErrorCode = ERROR_SUCCESS;



    // Assert hNetsyncObject is valid
    ASSERT((INVALID_HANDLE_VALUE != hNetsyncObject) && (NULL != hNetsyncObject) && (NETSYNC_OBJECT_TYPE == pNetsyncObject->dwObjectType));

    // Setup the destination address
    if (0 == ToAddr) {
        toname.sin_addr.s_addr = pNetsyncObject->ServerAddr;
    }
    else {
        toname.sin_addr.s_addr = ToAddr;
    }
    toname.sin_port = htons(ToPort);

    if (TRUE == bGuarantee) {
        // Find the client object
        pClient = FindClientObject(pNetsyncObject, toname.sin_addr.s_addr);
        if (NULL == pClient) {
            iErrorCode = GetLastError();

            goto ExitFunc;
        }

        // Create the ACK event
        hACKEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (NULL == hACKEvent) {
            iErrorCode = GetLastError();

            goto ExitFunc;
        }
    }

    // Get the message size
    dwSendMessageSize = sizeof(NETSYNC_MESSAGE_HEADER) + __min(dwMessageSize, NETSYNC_MESSAGE_SIZE);

    // Allocate the memory for the message
    pSendMessage = (char *) xMemAlloc(pNetsyncObject->hMemObject, dwSendMessageSize);
    if (NULL == pSendMessage) {
        iErrorCode = GetLastError();

        goto ExitFunc;
    }

    if (NULL != pClient) {
        // Wait for access to the send sequence number
        EnterCriticalSection(&pClient->csSendSN);

        // Get the new send sequence number
        ZeroMemory(&FileTime, sizeof(FileTime));
        GetSystemTimeAsFileTime(&FileTime);
        CopyMemory(&SendSN, &FileTime, sizeof(SendSN));

        if (SendSN <= pClient->LowSendSN) {
            pClient->HighSendSN++;
        }
        pClient->LowSendSN = SendSN;
    }

    // Set the netsync message header
    pNetsyncMessage = (PNETSYNC_MESSAGE_HEADER) pSendMessage;

    // Format the netsync message header
    pNetsyncMessage->dwMessageId = dwMessageId;
    pNetsyncMessage->dwMessageSize = dwMessageSize;
    CopyMemory(&pNetsyncMessage->FromXnAddr, &pNetsyncObject->XnAddr, sizeof(pNetsyncMessage->FromXnAddr));
    pNetsyncMessage->LowSN = (NULL == pClient) ? 0 : pClient->LowSendSN;
    pNetsyncMessage->HighSN = (NULL == pClient) ? 0 : pClient->HighSendSN;
    pNetsyncMessage->hACKEvent = hACKEvent;

    // Copy the message
    CopyMemory(pSendMessage + sizeof(NETSYNC_MESSAGE_HEADER), pMessage, __min(dwMessageSize, NETSYNC_MESSAGE_SIZE));

    // Wait for access to the critical section
    EnterCriticalSection(&pNetsyncObject->csSocket);

    for (dwAttempt = 0; dwAttempt < dwNumAttempts; dwAttempt++) {
        DebugMessage(NETSYNC_DEBUG_SEND, "NetsyncSendMessage(): Attempt %d - Send Message 0x%016I64x 0x%016I64x To 0x%08x On Port %u\n", (dwAttempt + 1), pNetsyncMessage->HighSN, pNetsyncMessage->LowSN, toname.sin_addr, ToPort);

        // Send the message
        dwBytes = sendto(pNetsyncObject->sSocket, pSendMessage, dwSendMessageSize, 0, (SOCKADDR *) &toname, sizeof(SOCKADDR_IN));
        if (SOCKET_ERROR == dwBytes) {
            iErrorCode = WSAGetLastError();

            DebugMessage(NETSYNC_DEBUG_SEND, "NetsyncSendMessage(): Attempt %d - Failed Send Message 0x%016I64x 0x%016I64x To 0x%08x On Port %u - ec: 0x%08x\n", (dwAttempt + 1), pNetsyncMessage->HighSN, pNetsyncMessage->LowSN, toname.sin_addr, ToPort, iErrorCode);

            break;
        }

        if (NULL == hACKEvent) {
            dwWaitResult = WAIT_OBJECT_0;

            break;
        }

        // Release access to the critical section
        LeaveCriticalSection(&pNetsyncObject->csSocket);

        // Wait for the ACK
        dwWaitResult = WaitForSingleObject(hACKEvent, dwTimeout);

        // Wait for access to the critical section
        EnterCriticalSection(&pNetsyncObject->csSocket);

        if (WAIT_OBJECT_0 == dwWaitResult) {
            DebugMessage(NETSYNC_DEBUG_SEND, "NetsyncSendMessage(): Attempt %d - Received ACK Send Message 0x%016I64x 0x%016I64x To 0x%08x On Port %u\n", (dwAttempt + 1), pNetsyncMessage->HighSN, pNetsyncMessage->LowSN, toname.sin_addr, ToPort);

            break;
        }
        else if (WAIT_TIMEOUT == dwWaitResult) {
            iErrorCode = WAIT_TIMEOUT;

            DebugMessage(NETSYNC_DEBUG_SEND, "NetsyncSendMessage(): Attempt %d - Wait Timed Out Send Message 0x%016I64x 0x%016I64x To 0x%08x On Port %u\n", (dwAttempt + 1), pNetsyncMessage->HighSN, pNetsyncMessage->LowSN, toname.sin_addr, ToPort);
        }
        else {
            iErrorCode = GetLastError();

            DebugMessage(NETSYNC_DEBUG_SEND, "NetsyncSendMessage(): Attempt %d - Wait Failed Send Message 0x%016I64x 0x%016I64x To 0x%08x On Port %u - ec: 0x%08x\n", (dwAttempt + 1), pNetsyncMessage->HighSN, pNetsyncMessage->LowSN, toname.sin_addr, ToPort, iErrorCode);

            break;
        }

        // Increase timeout
        dwTimeout = dwTimeout * 2;
    }

    // Release access to the critical section
    LeaveCriticalSection(&pNetsyncObject->csSocket);

    if (NULL != pClient) {
        // Release access to the send sequence number
        LeaveCriticalSection(&pClient->csSendSN);
    }

ExitFunc:
    if (NULL != pSendMessage) {
        xMemFree(pNetsyncObject->hMemObject, pSendMessage);
    }

    if (NULL != hACKEvent) {
        CloseHandle(hACKEvent);
    }

    if (ERROR_SUCCESS != iErrorCode) {
        // Set the last error code
        SetLastError(iErrorCode);
    }

    return dwWaitResult;
}



DWORD
NetsyncSendServerMessage(
    IN HANDLE   hNetsyncObject,
    IN u_long   ToAddr,
    IN u_short  ToPort,
    IN BOOL     bGuarantee,
    IN DWORD    dwMessageSize,
    IN char     *pMessage
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Sends a netsync message

Arguments:

  hNetsyncObject - Handle to the NETSYNC_OBJECT
  ToAddr - Destination address
  ToPort - Destination port
  bGuarantee - Specifies if the message is guaranteed
  dwMessageSize - Size of the message buffer, in bytes
  pMessage - Pointer to the message buffer

Return Value:

  DWORD:
    If the function succeeds, the return value is WAIT_OBJECT_0.
    If the function times out, the return value is WAIT_TIMEOUT.
    If the function fails, the return value is WAIT_FAILED.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is the pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT  pNetsyncObject = (PNETSYNC_OBJECT) hNetsyncObject;



    // Assert hNetsyncObject is valid
    ASSERT((INVALID_HANDLE_VALUE != hNetsyncObject) && (NULL != hNetsyncObject) && (NETSYNC_OBJECT_TYPE == pNetsyncObject->dwObjectType));

    if (0 == ToPort) {
        ToPort = NETSYNC_PORT;
    }

    return NetsyncSendMessageToPort(hNetsyncObject, ToAddr, ToPort, bGuarantee, NETSYNC_MSGTYPE_SERVER, dwMessageSize, pMessage);
}



DWORD
NetsyncDequeueMessage(
    IN     HANDLE  hNetsyncObject,
    IN     DWORD   dwTimeout,
    IN OUT DWORD   *dwMessageType,
    OUT    u_long  *FromInAddr,
    OUT    XNADDR  *FromXnAddr,
    OUT    DWORD   *dwMessageSize,
    OUT    char *  *pMessage
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Removes a message from a message queue

Arguments:

  hNetsyncObject - Handle to the NETSYNC_OBJECT
  dwTimeout - Specifies the timeout interval, in milliseconds
  dwMessageType - Specifies the message type
  FromInAddr - Source ip address
  FromXnAddr - Source xnet address
  dwMessageSize - Size of the message buffer, in bytes
  pMessage - Address of the pointer to the message buffer

Return Value:

  DWORD:
    If the function succeeds, the return value is WAIT_OBJECT_0.
    If the function times out, the return value is WAIT_TIMEOUT.
    If the function fails, the return value is WAIT_FAILED.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is the pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT         pNetsyncObject = (PNETSYNC_OBJECT) hNetsyncObject;

    // dwSemaphores is the count of semaphores
    DWORD                   dwSemaphores = 0;
    // hSemaphore is an array of semaphore
    HANDLE                  hSemaphore[2] = {NULL, NULL};
    // pcs is an array of critical sections
    CRITICAL_SECTION        *pcs[2] = {NULL, NULL};
    // pHeadQueue is an array of queue heads
    PNETSYNC_MESSAGE_QUEUE  *pHeadQueue[2] = {NULL, NULL};
    // pTailQueue is an array of queue tails
    PNETSYNC_MESSAGE_QUEUE  *pTailQueue[2] = {NULL, NULL};
    
    // pOldQueuedMessage is a pointer to the removed message
    PNETSYNC_MESSAGE_QUEUE  pOldQueuedMessage = NULL;
    // DebugAddr is the address for the debug message
    struct in_addr          DebugAddr;
    // dwWaitResult is the result of the wait on the overlapped I/O event
    DWORD                   dwWaitResult = WAIT_OBJECT_0;
    // iErrorCode is the last error code
    int                     iErrorCode = ERROR_SUCCESS;



    // Setup the arrays
    if (0 != (NETSYNC_MSGTYPE_SERVER & *dwMessageType)) {
        hSemaphore[dwSemaphores] = pNetsyncObject->hServerSemaphore;
        pcs[dwSemaphores] = &pNetsyncObject->csServerQueue;
        pHeadQueue[dwSemaphores] = &pNetsyncObject->pHeadServerQueue;
        pTailQueue[dwSemaphores] = &pNetsyncObject->pTailServerQueue;
        dwSemaphores++;
    }

    if (0 != (NETSYNC_MSGTYPE_CLIENT & *dwMessageType)) {
        hSemaphore[dwSemaphores] = pNetsyncObject->hClientSemaphore;
        pcs[dwSemaphores] = &pNetsyncObject->csClientQueue;
        pHeadQueue[dwSemaphores] = &pNetsyncObject->pHeadClientQueue;
        pTailQueue[dwSemaphores] = &pNetsyncObject->pTailClientQueue;
        dwSemaphores++;
    }

    // Wait for access to a semaphore
    dwWaitResult = WaitForMultipleObjects(dwSemaphores, hSemaphore, FALSE, dwTimeout);

    if (WAIT_TIMEOUT == dwWaitResult) {
        iErrorCode = WAIT_TIMEOUT;
    }
    else if (WAIT_FAILED == dwWaitResult) {
        iErrorCode = GetLastError();
    }
    else {
        // Get the message type
        if (0 == (dwWaitResult - WAIT_OBJECT_0)) {
            if (0 != (NETSYNC_MSGTYPE_SERVER & *dwMessageType)) {
                *dwMessageType = NETSYNC_MSGTYPE_SERVER;
            }
            else {
                *dwMessageType = NETSYNC_MSGTYPE_CLIENT;
            }
        }
        else {
            *dwMessageType = NETSYNC_MSGTYPE_CLIENT;
        }

        // Wait for access to the critical section
        EnterCriticalSection(pcs[dwWaitResult - WAIT_OBJECT_0]);

        // Get the message
        pOldQueuedMessage = *pHeadQueue[dwWaitResult - WAIT_OBJECT_0];

        DebugAddr.s_addr = pOldQueuedMessage->FromInAddr;
        DebugMessage(NETSYNC_DEBUG_QUEUE, "NetsyncDequeueMessage(): Dequeued Message 0x%016I64x 0x%016I64x From 0x%08x On Port %u\n", pOldQueuedMessage->HighSN, pOldQueuedMessage->LowSN, DebugAddr, pNetsyncObject->NetsyncPort);

        // Get the message information
        *dwMessageSize = pOldQueuedMessage->dwMessageSize;
        *FromInAddr = pOldQueuedMessage->FromInAddr;
        if (NULL != FromXnAddr) {
            CopyMemory(FromXnAddr, &pOldQueuedMessage->FromXnAddr, sizeof(*FromXnAddr));
        }
        *pMessage = (char *) ((UINT_PTR) pOldQueuedMessage->pReceivedMessage + sizeof(HANDLE));

        // Update the pointer to the head of the queue
        *pHeadQueue[dwWaitResult - WAIT_OBJECT_0] = pOldQueuedMessage->pNextQueuedMessage;

        if (NULL == *pHeadQueue[dwWaitResult - WAIT_OBJECT_0]) {
            *pTailQueue[dwWaitResult - WAIT_OBJECT_0] = NULL;
        }

        // Free the removed message
        xMemFree(pNetsyncObject->hMemObject, pOldQueuedMessage);

        // Release access to the critical section
        LeaveCriticalSection(pcs[dwWaitResult - WAIT_OBJECT_0]);
    }

    if (ERROR_SUCCESS != iErrorCode) {
        // Set the last error code
        SetLastError(iErrorCode);
    }
    else {
        dwWaitResult = WAIT_OBJECT_0;
    }

    return dwWaitResult;
}



VOID
NetsyncQueueMessage(
    IN     CRITICAL_SECTION        *pcs,
    IN     HANDLE                  hSemaphore,
    IN OUT PNETSYNC_MESSAGE_QUEUE  *pHeadQueue,
    IN OUT PNETSYNC_MESSAGE_QUEUE  *pTailQueue,
    IN     PNETSYNC_MESSAGE_QUEUE  pNewQueuedMessage
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Adds a message from a message queue

Arguments:

  cs - Pointer to the object to synchronize access to the queue
  hSemaphore - Handle to the object to synchronize access to the queue
  pHeadQueue - Address of the pointer to the head of the queue
  pTailQueue - Address of the pointer to the tail of the queue
  pNewQueuedMessage - Pointer to the message buffer

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // Wait for access to the critical section
    EnterCriticalSection(pcs);

    if (NULL != *pTailQueue) {
        (*pTailQueue)->pNextQueuedMessage = pNewQueuedMessage;
    }
    *pTailQueue = pNewQueuedMessage;

    if (NULL == *pHeadQueue) {
        *pHeadQueue = pNewQueuedMessage;
    }

    ReleaseSemaphore(hSemaphore, 1, NULL);

    // Release access to the critical section
    LeaveCriticalSection(pcs);
}



DWORD
ReceiveThread(
    IN LPVOID  lpv
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Thread to handle the receive logic

Arguments:

  lpv - Pointer to the NETSYNC_OBJECT

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is the pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT           pNetsyncObject = (PNETSYNC_OBJECT) lpv;

    // pClient is a pointer to the client object
    PNETSYNC_CLIENT_OBJECT    pClient;

    // ReceiveMessage is the receive message buffer
    char                      ReceiveMessage[sizeof(NETSYNC_MESSAGE_HEADER) + NETSYNC_MESSAGE_SIZE];
    // pGenericMessage is the pointer to the generic message
    PNETSYNC_GENERIC_MESSAGE  pGenericMessage = (PNETSYNC_GENERIC_MESSAGE) (ReceiveMessage + sizeof(NETSYNC_MESSAGE_HEADER));
    // pNetsyncMessage is the pointer to the netsync message header in the receive message buffer
    PNETSYNC_MESSAGE_HEADER   pNetsyncMessage = (PNETSYNC_MESSAGE_HEADER) ReceiveMessage;
    // pNewQueuedMessage is a pointer to the new queued message
    PNETSYNC_MESSAGE_QUEUE    pNewQueuedMessage;
    // dwMessageId is a copy of the message id
    DWORD                     dwMessageId;
    // DebugAddr is the address for the debug message
    struct in_addr            DebugAddr;

    // WSAOverlapped is the overlapped I/O structure
    WSAOVERLAPPED             WSAOverlapped;
    // WSABuffer is the WSABUF structures
    WSABUF                    WSABuffer;
    // fromname is the source address
    struct sockaddr_in        fromname;
    // fromnamelen is the size of the source address
    int                       fromnamelen;
    // dwBytes is the number of bytes received on the socket
    DWORD                     dwBytes;
    // dwFlags is a bitmask of MSG_ values
    DWORD                     dwFlags;
    // dwWaitResult is the result of the wait on the overlapped I/O event
    DWORD                     dwWaitResult;
    // iErrorCode is the last error code
    int                       iErrorCode;



    // Assert pNetsyncObject is valid
    ASSERT((INVALID_HANDLE_VALUE != pNetsyncObject) && (NULL != pNetsyncObject) && (NETSYNC_OBJECT_TYPE == pNetsyncObject->dwObjectType));

    do {
        // Setup the WSABUF structure
        ZeroMemory(ReceiveMessage, sizeof(ReceiveMessage));
        WSABuffer.len = sizeof(ReceiveMessage);
        WSABuffer.buf = ReceiveMessage;

        // Setup the flags
        dwFlags = 0;

        // Setup the address
        ZeroMemory(&fromname, sizeof(fromname));
        fromnamelen = sizeof(fromname);

        // Setup the overlapped I/O
        ZeroMemory(&WSAOverlapped, sizeof(WSAOverlapped));
        WSAOverlapped.hEvent = pNetsyncObject->hReceiveEvent;

        // Setup the result
        dwWaitResult = WAIT_FAILED;
        iErrorCode = ERROR_SUCCESS;

        // Wait for access to the critical section
        EnterCriticalSection(&pNetsyncObject->csSocket);

        // Post the WSARecv request
        if (SOCKET_ERROR == WSARecvFrom(pNetsyncObject->sSocket, &WSABuffer, 1, &dwBytes, &dwFlags, (SOCKADDR *) &fromname, &fromnamelen, &WSAOverlapped, NULL)) {
            if (WSA_IO_PENDING != WSAGetLastError()) {
                iErrorCode = WSAGetLastError();

                // Release access to the critical section
                LeaveCriticalSection(&pNetsyncObject->csSocket);

                continue;
            }

            // Release access to the critical section
            LeaveCriticalSection(&pNetsyncObject->csSocket);

            // Wait for the overlapped I/O request to complete
            dwWaitResult = WaitForSingleObject(pNetsyncObject->hReceiveEvent, INFINITE);

            // Wait for access to the critical section
            EnterCriticalSection(&pNetsyncObject->csSocket);

            if (WAIT_OBJECT_0 == dwWaitResult) {
                // Get the status of the overlapped I/O request
                if (FALSE == WSAGetOverlappedResult(pNetsyncObject->sSocket, &WSAOverlapped, &dwBytes, FALSE, &dwFlags)) {
                    iErrorCode = WSAGetLastError();

                    dwWaitResult = WAIT_FAILED;
                }
            }
            else {
                iErrorCode = GetLastError();
            }

            if (ERROR_SUCCESS != iErrorCode) {
                // Cancel the pending IO request
#ifdef _XBOX
                WSACancelOverlappedIO(pNetsyncObject->sSocket);
#else
                CancelIo((HANDLE) pNetsyncObject->sSocket);
#endif
            }
        }
        else {
            // WSARecv request completed immediately
            dwWaitResult = WAIT_OBJECT_0;

            // Reset the receive event
            ResetEvent(pNetsyncObject->hReceiveEvent);
        }

        // Release access to the critical section
        LeaveCriticalSection(&pNetsyncObject->csSocket);

        if (WAIT_FAILED == dwWaitResult) {
            DebugMessage(NETSYNC_DEBUG_RECEIVE, "ReceiveThread(): WAIT_FAILED - ec = 0x%08x\n", iErrorCode);
            continue;
        }

        // Find the client object
        pClient = FindClientObject(pNetsyncObject, fromname.sin_addr.s_addr);
        if (NULL == pClient) {
            continue;
        }

        // Set the address
        DebugAddr.s_addr = fromname.sin_addr.s_addr;

        if (NETSYNC_MSG_ACK != pNetsyncMessage->dwMessageId) {
            // Initialize the queued message
            pNewQueuedMessage = NULL;

            // Wait for access to the receive sequence number
            EnterCriticalSection(&pClient->csReceiveSN);

            if (((0 == pNetsyncMessage->LowSN) && (0 == pNetsyncMessage->HighSN)) || (pClient->LowReceiveSN < pNetsyncMessage->LowSN) || (pClient->HighReceiveSN < pNetsyncMessage->HighSN)) {
                // Allocate the memory for the queued message
                pNewQueuedMessage = (PNETSYNC_MESSAGE_QUEUE) xMemAlloc(pNetsyncObject->hMemObject, sizeof(NETSYNC_MESSAGE_QUEUE));
                if (NULL == pNewQueuedMessage) {
                    // Release access to the receive sequence number
                    LeaveCriticalSection(&pClient->csReceiveSN);

                    continue;
                }

                // Allocate the memory for the message
                pNewQueuedMessage->pReceivedMessage = (char *) xMemAlloc(pNetsyncObject->hMemObject, pNetsyncMessage->dwMessageSize + sizeof(HANDLE));
                if (NULL == pNewQueuedMessage->pReceivedMessage) {
                    // Free the queued message
                    xMemFree(pNetsyncObject->hMemObject, pNewQueuedMessage);

                    // Release access to the receive sequence number
                    LeaveCriticalSection(&pClient->csReceiveSN);

                    continue;
                }

                // Copy the queue message
                pNewQueuedMessage->dwMessageSize = pNetsyncMessage->dwMessageSize;

                pNewQueuedMessage->FromInAddr = fromname.sin_addr.s_addr;
                CopyMemory(&pNewQueuedMessage->FromXnAddr, &pNetsyncMessage->FromXnAddr, sizeof(pNewQueuedMessage->FromXnAddr));

                pNewQueuedMessage->LowSN = pNetsyncMessage->LowSN;
                pNewQueuedMessage->HighSN = pNetsyncMessage->HighSN;

                CopyMemory(pNewQueuedMessage->pReceivedMessage, &pNetsyncObject->hMemObject, sizeof(HANDLE));
                CopyMemory((char *) (UINT_PTR) pNewQueuedMessage->pReceivedMessage + sizeof(HANDLE), pGenericMessage, pNetsyncMessage->dwMessageSize);
            }

            if (NULL != pNetsyncMessage->hACKEvent) {
                // ACK the message
                dwMessageId = pNetsyncMessage->dwMessageId;
                pNetsyncMessage->dwMessageId = NETSYNC_MSG_ACK;

                if (NETSYNC_MSG_STOPRECEIVE == pGenericMessage->dwMessageId) {
                    DebugMessage(NETSYNC_DEBUG_RECEIVE, "ReceiveThread(): NETSYNC_MSG_STOPRECEIVE\n");
                    SetEvent(pNetsyncMessage->hACKEvent);
                }
                else {
                    DebugMessage(NETSYNC_DEBUG_RECEIVE, "ReceiveThread(): Send ACK Message 0x%016I64x 0x%016I64x From 0x%08x On Port %u\n", pNetsyncMessage->HighSN, pNetsyncMessage->LowSN, DebugAddr, ntohs(fromname.sin_port));
                    sendto(pNetsyncObject->sSocket, (char *) pNetsyncMessage, sizeof(NETSYNC_MESSAGE_HEADER), 0, (SOCKADDR *) &fromname, sizeof(SOCKADDR_IN));
                }

                pNetsyncMessage->dwMessageId = dwMessageId;
            }

            if (NULL != pNewQueuedMessage) {
                // Queue the message
                DebugMessage(NETSYNC_DEBUG_RECEIVE, "ReceiveThread(): Queue Message 0x%016I64x 0x%016I64x From 0x%08x On Port %u\n", pNetsyncMessage->HighSN, pNetsyncMessage->LowSN, DebugAddr, ntohs(fromname.sin_port));

                if (NETSYNC_MSGTYPE_SERVER == pNetsyncMessage->dwMessageId) {
                    // Server message
                    NetsyncQueueMessage(&pNetsyncObject->csServerQueue, pNetsyncObject->hServerSemaphore, &pNetsyncObject->pHeadServerQueue, &pNetsyncObject->pTailServerQueue, pNewQueuedMessage);
                }
                else {
                    // Client message
                    NetsyncQueueMessage(&pNetsyncObject->csClientQueue, pNetsyncObject->hClientSemaphore, &pNetsyncObject->pHeadClientQueue, &pNetsyncObject->pTailClientQueue, pNewQueuedMessage);
                }

                if ((0 != pNetsyncMessage->LowSN) || (0 != pNetsyncMessage->HighSN)) {
                    // Update the receive sequence number
                    pClient->LowReceiveSN = pNetsyncMessage->LowSN;
                    pClient->HighReceiveSN = pNetsyncMessage->HighSN;
                }
            }

            // Release access to the receive sequence number
            LeaveCriticalSection(&pClient->csReceiveSN);
        }
        else {
            // Wait for access to the ack sequence number
            EnterCriticalSection(&pClient->csAckSN);

            if ((pClient->LowAckSN < pNetsyncMessage->LowSN) || (pClient->HighAckSN < pNetsyncMessage->HighSN)) {
                DebugMessage(NETSYNC_DEBUG_RECEIVE, "ReceiveThread(): Receive ACK Message 0x%016I64x 0x%016I64x From 0x%08x On Port %u\n", pNetsyncMessage->HighSN, pNetsyncMessage->LowSN, DebugAddr, ntohs(fromname.sin_port));

                // ACK the message
                SetEvent(pNetsyncMessage->hACKEvent);

                // Update the ack sequence number
                pClient->LowAckSN = pNetsyncMessage->LowSN;
                pClient->HighAckSN = pNetsyncMessage->HighSN;
            }

            // Release access to the ack sequence number
            LeaveCriticalSection(&pClient->csAckSN);
        }
    } while (NETSYNC_MSG_STOPRECEIVE != pGenericMessage->dwMessageId);

    return 0;
}

} // namespace NetsyncNamespace



DWORD
WINAPI
NetsyncSendClientMessage(
    IN HANDLE  hNetsyncObject,
    IN u_long  ToAddr,
    IN DWORD   dwMessageSize,
    IN char    *pMessage
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Sends a netsync message

Arguments:

  hNetsyncObject - Handle to the NETSYNC_OBJECT
  ToAddr - Destination address
  dwMessageSize - Size of the message buffer, in bytes
  pMessage - Pointer to the message buffer

Return Value:

  DWORD:
    If the function succeeds, the return value is WAIT_OBJECT_0.
    If the function times out, the return value is WAIT_TIMEOUT.
    If the function fails, the return value is WAIT_FAILED.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is the pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT  pNetsyncObject = (PNETSYNC_OBJECT) hNetsyncObject;



    // Assert hNetsyncObject is valid
    ASSERT((INVALID_HANDLE_VALUE != hNetsyncObject) && (NULL != hNetsyncObject) && (NETSYNC_OBJECT_TYPE == pNetsyncObject->dwObjectType));

    return NetsyncSendMessageToPort(hNetsyncObject, ToAddr, pNetsyncObject->NetsyncPort, TRUE, NETSYNC_MSGTYPE_CLIENT, dwMessageSize, pMessage);
}



DWORD
WINAPI
NetsyncReceiveServerMessage(
    IN  HANDLE  hNetsyncObject,
    IN  DWORD   dwTimeout,
    OUT u_long  *FromInAddr,
    OUT XNADDR  *FromXnAddr,
    OUT DWORD   *dwMessageSize,
    OUT char *  *pMessage
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Receives a netsync server message

Arguments:

  hNetsyncObject - Handle to the NETSYNC_OBJECT
  dwTimeout - Specifies the timeout interval, in milliseconds
  FromInAddr - Source ip address
  FromXnAddr - Source xnet address
  dwMessageSize - Size of the message buffer, in bytes
  pMessage - Address of the pointer to the message buffer

Return Value:

  DWORD:
    If the function succeeds, the return value is WAIT_OBJECT_0.
    If the function times out, the return value is WAIT_TIMEOUT.
    If the function fails, the return value is WAIT_FAILED.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is the pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT  pNetsyncObject = (PNETSYNC_OBJECT) hNetsyncObject;
    // dwMessageType is the message type
    DWORD            dwMessageType = NETSYNC_MSGTYPE_SERVER;



    // Assert hNetsyncObject is valid
    ASSERT((INVALID_HANDLE_VALUE != hNetsyncObject) && (NULL != hNetsyncObject) && (NETSYNC_OBJECT_TYPE == pNetsyncObject->dwObjectType));

    return NetsyncDequeueMessage(hNetsyncObject, dwTimeout, &dwMessageType, FromInAddr, FromXnAddr, dwMessageSize, pMessage);
}



DWORD
WINAPI
NetsyncReceiveClientMessage(
    IN  HANDLE  hNetsyncObject,
    IN  DWORD   dwTimeout,
    OUT u_long  *FromInAddr,
    OUT XNADDR  *FromXnAddr,
    OUT DWORD   *dwMessageSize,
    OUT char *  *pMessage
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Receives a netsync message

Arguments:

  hNetsyncObject - Handle to the NETSYNC_OBJECT
  dwTimeout - Specifies the timeout interval, in milliseconds
  FromInAddr - Source ip address
  FromXnAddr - Source xnet address
  dwMessageSize - Size of the message buffer, in bytes
  pMessage - Address of the pointer to the message buffer

Return Value:

  DWORD:
    If the function succeeds, the return value is WAIT_OBJECT_0.
    If the function times out, the return value is WAIT_TIMEOUT.
    If the function fails, the return value is WAIT_FAILED.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is the pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT  pNetsyncObject = (PNETSYNC_OBJECT) hNetsyncObject;
    // dwMessageType is the message type
    DWORD            dwMessageType = NETSYNC_MSGTYPE_CLIENT;



    // Assert hNetsyncObject is valid
    ASSERT((INVALID_HANDLE_VALUE != hNetsyncObject) && (NULL != hNetsyncObject) && (NETSYNC_OBJECT_TYPE == pNetsyncObject->dwObjectType));

    return NetsyncDequeueMessage(hNetsyncObject, dwTimeout, &dwMessageType, FromInAddr, FromXnAddr, dwMessageSize, pMessage);
}



DWORD
WINAPI
NetsyncReceiveMessage(
    IN  HANDLE  hNetsyncObject,
    IN  DWORD   dwTimeout,
    OUT DWORD   *dwMessageType,
    OUT u_long  *FromInAddr,
    OUT XNADDR  *FromXnAddr,
    OUT DWORD   *dwMessageSize,
    OUT char *  *pMessage
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Receives a netsync message

Arguments:

  hNetsyncObject - Handle to the NETSYNC_OBJECT
  dwTimeout - Specifies the timeout interval, in milliseconds
  dwMessageType - Specifies the type of message
  FromInAddr - Source ip address
  FromXnAddr - Source xnet address
  dwMessageSize - Size of the message buffer, in bytes
  pMessage - Address of the pointer to the message buffer

Return Value:

  DWORD:
    If the function succeeds, the return value is WAIT_OBJECT_0.
    If the function times out, the return value is WAIT_TIMEOUT.
    If the function fails, the return value is WAIT_FAILED.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is the pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT  pNetsyncObject = (PNETSYNC_OBJECT) hNetsyncObject;



    // Assert hNetsyncObject is valid
    ASSERT((INVALID_HANDLE_VALUE != hNetsyncObject) && (NULL != hNetsyncObject) && (NETSYNC_OBJECT_TYPE == pNetsyncObject->dwObjectType));

    *dwMessageType = NETSYNC_MSGTYPE_SERVER | NETSYNC_MSGTYPE_CLIENT;

    return NetsyncDequeueMessage(hNetsyncObject, dwTimeout, dwMessageType, FromInAddr, FromXnAddr, dwMessageSize, pMessage);
}



BOOL
WINAPI
NetsyncFreeMessage(
    IN  char  *pMessage
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Frees a block of memory allocated from the private heap for a message

Arguments:

  pMessage - Pointer to the message buffer

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pReceivedMessage is a pointer to the entire message
    char    *pReceivedMessage = NULL;
    // hMemObject is a handle to the memory object
    HANDLE  *hMemObject = NULL;



    // Get the message
    pReceivedMessage = (char *) ((UINT_PTR) pMessage - sizeof(HANDLE));

    // Get the memory object
    hMemObject = (HANDLE *) pReceivedMessage;

    return xMemFree(*hMemObject, pReceivedMessage);
}
