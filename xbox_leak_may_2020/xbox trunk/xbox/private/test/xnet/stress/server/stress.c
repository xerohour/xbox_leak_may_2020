/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright Microsoft Corporation

Module Name:

  stress.c

Abstract:

  This modules tests XNet stress

Author:

  Steven Kehrli (steveke) 17-Apr-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetStressNamespace;

namespace XNetStressNamespace {

typedef struct _STRESS_CLIENT_LIST {
    BYTE                    byClientCount;
    STRESS_CLIENT           StressClients[NUM_STRESS_CLIENTS];
} STRESS_CLIENT_LIST, *PSTRESS_CLIENT_LIST;

typedef struct _STRESS_SESSION {
    HANDLE                  hMemObject;
    CRITICAL_SECTION        cs;
    HANDLE                  hThread;
    HANDLE                  hSessionObject;
    HANDLE                  hNetsyncObject;
    BOOL                    bExit;
    STRESS_CLIENT_LIST      StressClientList;
    struct _STRESS_SESSION  *pNextStressSession;
} STRESS_SESSION, *PSTRESS_SESSION;

HANDLE            g_hStressSessionsMutex = NULL;
DWORD             g_dwNumStressSessions = 0;
PSTRESS_SESSION   g_pStressSessions = NULL;



DWORD
XNetStressKeepAlive(
    IN LPVOID  lpv
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Thread to handle the server keep-alive logic

Arguments:

  lpv - Pointer to the stress session structure

------------------------------------------------------------------------------*/
{
    // pStressSession is a pointer to the stress session
    PSTRESS_SESSION         pStressSession = (PSTRESS_SESSION) lpv;
    // StressSessionMessage is a message sent to the stress session
    STRESS_SESSION_MESSAGE  StressSessionMessage;
    // NetsyncRemoteInAddr is the netsync address of the partner stress client
    u_long                  NetsyncRemoteInAddr;
    // byIndex is a counter to enumerate each stress client
    BYTE                    byIndex;



    do {
        Sleep(60000);

        // Wait for the critical section
        EnterCriticalSection(&pStressSession->cs);

        for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
            if ((0 != pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) && (600000 < (GetTickCount() - pStressSession->StressClientList.StressClients[byIndex].dwLastKeepAlive))) {
                // Client failed to send keep-alive so remove client from session
                NetsyncRemoveClientFromSession(pStressSession->hSessionObject, pStressSession->hNetsyncObject, pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr);

                // And remove client from list
                pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr = 0;
                ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].ClientXnAddr, sizeof(pStressSession->StressClientList.StressClients[byIndex].ClientXnAddr));
                pStressSession->StressClientList.StressClients[byIndex].dwLastKeepAlive = 0;
                ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].XnKid, sizeof(pStressSession->StressClientList.StressClients[byIndex].XnKid));
                ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].XnKey, sizeof(pStressSession->StressClientList.StressClients[byIndex].XnKey));
                NetsyncRemoteInAddr = pStressSession->StressClientList.StressClients[byIndex].NetsyncRemoteInAddr;
                pStressSession->StressClientList.StressClients[byIndex].NetsyncRemoteInAddr = 0;
                ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr, sizeof(pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr));
                pStressSession->StressClientList.StressClients[byIndex].dwStressType = 0;
                pStressSession->StressClientList.byClientCount--;

                // And shutdown partner
                ZeroMemory(&StressSessionMessage, sizeof(StressSessionMessage));
                StressSessionMessage.dwMessageId = SERVER_STOP_MSG;
                NetsyncSendClientMessage(pStressSession->hNetsyncObject, NetsyncRemoteInAddr, sizeof(StressSessionMessage), (char *) &StressSessionMessage);
            }
        }

        // Release critical section
        LeaveCriticalSection(&pStressSession->cs);
    } while (FALSE == pStressSession->bExit);

    return 0;
}

} // namespace XNetStressNamespace



BOOL
WINAPI
XNetStressMain(
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

  Callback for the stress session

Arguments:

  hSessionObject - Handle to the session object
  hNetsyncObject - Handle to the netsync object
  FromAddr - Specifies the address of the netsync sender
  dwMessageType - Specifies the type of received message
  dwMessageSize - Specifies the size of the received message
  pMessage - Pointer to the received message
  lpContext - Pointer to the context structure

Return Value:

  BOOL:
    If the session should continue, the return value is TRUE.
    If the session should exit, the return value is FALSE.

------------------------------------------------------------------------------*/
{
    // pStressSession is a pointer to the stress session
    PSTRESS_SESSION         pStressSession = (PSTRESS_SESSION) lpContext;
    // StressSessionMessage is a message sent to the stress session
    STRESS_SESSION_MESSAGE  StressSessionMessage;
    // NetsyncRemoteInAddr is the netsync address of the partner stress client
    u_long                  NetsyncRemoteInAddr;
    // RemoteXnAddr is the xnet address of the partner stress client
    XNADDR                  RemoteXnAddr;
    // byIndex is a counter to enumerate each stress client
    BYTE                    byIndex;



    if (NULL == lpContext) {
        return FALSE;
    }

    // Wait for the critical section
    EnterCriticalSection(&pStressSession->cs);

    if (NETSYNC_MSGTYPE_SERVER == dwMessageType) {
        if (NETSYNC_MSG_STOPSESSION == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId) {
            // Shutdown all clients
            for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
                if (0 != pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) {
                    ZeroMemory(&StressSessionMessage, sizeof(StressSessionMessage));
                    StressSessionMessage.dwMessageId = SERVER_STOP_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr, sizeof(StressSessionMessage), (char *) &StressSessionMessage);

                    pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr = 0;
                    ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].ClientXnAddr, sizeof(pStressSession->StressClientList.StressClients[byIndex].ClientXnAddr));
                    pStressSession->StressClientList.StressClients[byIndex].dwLastKeepAlive = 0;
                    ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].XnKid, sizeof(pStressSession->StressClientList.StressClients[byIndex].XnKid));
                    ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].XnKey, sizeof(pStressSession->StressClientList.StressClients[byIndex].XnKey));
                    pStressSession->StressClientList.StressClients[byIndex].NetsyncRemoteInAddr = 0;
                    ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr, sizeof(pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr));
                    pStressSession->StressClientList.StressClients[byIndex].dwStressType = 0;
                    pStressSession->StressClientList.StressClients[byIndex].dwStatus = 0;
                    pStressSession->StressClientList.byClientCount--;
                }
            }
        }
        else if (NETSYNC_MSG_DUPLICATECLIENT == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId) {
            // Search for duplicate client
            for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
                if (FromInAddr == pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) {
                    if (0 != pStressSession->StressClientList.StressClients[byIndex].dwStatus) {
                        // Flag the status
                        pStressSession->StressClientList.StressClients[byIndex].dwStatus = 0;

                        // Send the message
                        ZeroMemory(&StressSessionMessage, sizeof(StressSessionMessage));
                        StressSessionMessage.dwMessageId = SERVER_REGISTER_MSG;
                        CopyMemory(&StressSessionMessage.XnKid, &pStressSession->StressClientList.StressClients[byIndex].XnKid, sizeof(StressSessionMessage.XnKid));
                        CopyMemory(&StressSessionMessage.XnKey, &pStressSession->StressClientList.StressClients[byIndex].XnKey, sizeof(StressSessionMessage.XnKey));
                        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(StressSessionMessage), (char *) &StressSessionMessage);
                    }

                    break;
                }
            }
        }
        else if (NETSYNC_MSG_ADDCLIENT == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId) {
            // Initiate partner address
            NetsyncRemoteInAddr = 0;
            ZeroMemory(&RemoteXnAddr, sizeof(RemoteXnAddr));

            // Find a partner for the stress client
            for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
                if ((0 != pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) && (0 == pStressSession->StressClientList.StressClients[byIndex].NetsyncRemoteInAddr)) {
                    NetsyncRemoteInAddr = pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr;
                    pStressSession->StressClientList.StressClients[byIndex].NetsyncRemoteInAddr = FromInAddr;
                    CopyMemory(&RemoteXnAddr, &pStressSession->StressClientList.StressClients[byIndex].ClientXnAddr, sizeof(RemoteXnAddr));
                    CopyMemory(&pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr, FromXnAddr, sizeof(pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr));
                    pStressSession->StressClientList.StressClients[byIndex].dwStressType = STRESS_TYPE_1;

                    break;
                }
            }

            // Add the new client to the list of stress clients
            for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
                if (0 == pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) {
                    pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr = FromInAddr;
                    CopyMemory(&pStressSession->StressClientList.StressClients[byIndex].ClientXnAddr, FromXnAddr, sizeof(pStressSession->StressClientList.StressClients[byIndex].ClientXnAddr));
                    pStressSession->StressClientList.StressClients[byIndex].dwLastKeepAlive = GetTickCount();
                    pStressSession->StressClientList.StressClients[byIndex].NetsyncRemoteInAddr = NetsyncRemoteInAddr;
                    CopyMemory(&pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr, &RemoteXnAddr, sizeof(pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr));

                    break;
                }
            }

            pStressSession->StressClientList.byClientCount++;

            if (0 != NetsyncRemoteInAddr) {
                // Send key-pair message
                ZeroMemory(&StressSessionMessage, sizeof(StressSessionMessage));
                StressSessionMessage.dwMessageId = SERVER_KEYPAIR_MSG;
                NetsyncSendClientMessage(hNetsyncObject, NetsyncRemoteInAddr, sizeof(StressSessionMessage), (char *) &StressSessionMessage);
            }
        }
        else if (NETSYNC_MSG_DELETECLIENT == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId) {
            // Initiate partner address
            NetsyncRemoteInAddr = 0;

            // Find the old client
            for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
                if (FromInAddr == pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) {
                    // Remove the old client from the list of stress clients
                    pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr = 0;
                    ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].ClientXnAddr, sizeof(pStressSession->StressClientList.StressClients[byIndex].ClientXnAddr));
                    pStressSession->StressClientList.StressClients[byIndex].dwLastKeepAlive = 0;
                    ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].XnKid, sizeof(pStressSession->StressClientList.StressClients[byIndex].XnKid));
                    ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].XnKey, sizeof(pStressSession->StressClientList.StressClients[byIndex].XnKey));
                    NetsyncRemoteInAddr = pStressSession->StressClientList.StressClients[byIndex].NetsyncRemoteInAddr;
                    pStressSession->StressClientList.StressClients[byIndex].NetsyncRemoteInAddr = 0;
                    ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr, sizeof(pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr));
                    pStressSession->StressClientList.StressClients[byIndex].dwStressType = 0;
                    pStressSession->StressClientList.byClientCount--;

                    break;
                }
            }

            if (0 != NetsyncRemoteInAddr) {
                // Shutdown partner
                for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
                    if (NetsyncRemoteInAddr == pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) {
                        ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].XnKid, sizeof(pStressSession->StressClientList.StressClients[byIndex].XnKid));
                        ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].XnKey, sizeof(pStressSession->StressClientList.StressClients[byIndex].XnKey));
                        pStressSession->StressClientList.StressClients[byIndex].NetsyncRemoteInAddr = 0;
                        ZeroMemory(&pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr, sizeof(pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr));
                        pStressSession->StressClientList.StressClients[byIndex].dwStressType = 0;
                        pStressSession->StressClientList.StressClients[byIndex].dwStatus = 0;

                        ZeroMemory(&StressSessionMessage, sizeof(StressSessionMessage));
                        StressSessionMessage.dwMessageId = SERVER_STOP_MSG;
                        NetsyncSendClientMessage(hNetsyncObject, NetsyncRemoteInAddr, sizeof(StressSessionMessage), (char *) &StressSessionMessage);

                        break;
                    }
                }
            }
        }
    }
    else if (NETSYNC_MSGTYPE_CLIENT == dwMessageType) {
        if (CLIENT_KEYPAIR_MSG == ((PSTRESS_SESSION_MESSAGE) pMessage)->dwMessageId) {
            // Initiate partner address
            NetsyncRemoteInAddr = 0;

            // Find the old client
            for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
                if (FromInAddr == pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) {
                    // Copy the key-pair
                    CopyMemory(&pStressSession->StressClientList.StressClients[byIndex].XnKid, &((PSTRESS_SESSION_MESSAGE) pMessage)->XnKid, sizeof(pStressSession->StressClientList.StressClients[byIndex].XnKid));
                    CopyMemory(&pStressSession->StressClientList.StressClients[byIndex].XnKey, &((PSTRESS_SESSION_MESSAGE) pMessage)->XnKey, sizeof(pStressSession->StressClientList.StressClients[byIndex].XnKey));
                    NetsyncRemoteInAddr = pStressSession->StressClientList.StressClients[byIndex].NetsyncRemoteInAddr;
                    pStressSession->StressClientList.StressClients[byIndex].dwStatus = STATUS_REGISTERED;

                    break;
                }
            }

            if (0 != NetsyncRemoteInAddr) {
                // Find the partner
                for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
                    if (NetsyncRemoteInAddr == pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) {
                        // Copy the key-pair
                        CopyMemory(&pStressSession->StressClientList.StressClients[byIndex].XnKid, &((PSTRESS_SESSION_MESSAGE) pMessage)->XnKid, sizeof(pStressSession->StressClientList.StressClients[byIndex].XnKid));
                        CopyMemory(&pStressSession->StressClientList.StressClients[byIndex].XnKey, &((PSTRESS_SESSION_MESSAGE) pMessage)->XnKey, sizeof(pStressSession->StressClientList.StressClients[byIndex].XnKey));

                        // Send register message
                        ZeroMemory(&StressSessionMessage, sizeof(StressSessionMessage));
                        StressSessionMessage.dwMessageId = SERVER_REGISTER_MSG;
                        CopyMemory(&StressSessionMessage.XnKid, &pStressSession->StressClientList.StressClients[byIndex].XnKid, sizeof(StressSessionMessage.XnKid));
                        CopyMemory(&StressSessionMessage.XnKey, &pStressSession->StressClientList.StressClients[byIndex].XnKey, sizeof(StressSessionMessage.XnKey));
                        NetsyncSendClientMessage(hNetsyncObject, NetsyncRemoteInAddr, sizeof(StressSessionMessage), (char *) &StressSessionMessage);

                        break;
                    }
                }
            }
        }
        else if (CLIENT_REGISTER_MSG == ((PSTRESS_SESSION_MESSAGE) pMessage)->dwMessageId) {
            // Initiate partner address
            NetsyncRemoteInAddr = 0;

            // Find the old client
            for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
                if (FromInAddr == pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) {
                    // Update the status
                    NetsyncRemoteInAddr = pStressSession->StressClientList.StressClients[byIndex].NetsyncRemoteInAddr;
                    pStressSession->StressClientList.StressClients[byIndex].dwStatus = STATUS_STARTED;

                    // Send start message
                    ZeroMemory(&StressSessionMessage, sizeof(StressSessionMessage));
                    StressSessionMessage.dwMessageId = SERVER_START_MSG;
                    StressSessionMessage.dwStressType = pStressSession->StressClientList.StressClients[byIndex].dwStressType;
                    CopyMemory(&StressSessionMessage.XnAddr, &pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr, sizeof(StressSessionMessage.XnAddr));
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(StressSessionMessage), (char *) &StressSessionMessage);

                    break;
                }
            }

            if (0 != NetsyncRemoteInAddr) {
                // Find the partner
                for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
                    if (NetsyncRemoteInAddr == pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) {
                        if (STATUS_STARTED != pStressSession->StressClientList.StressClients[byIndex].dwStatus) {
                            // Update the status
                            pStressSession->StressClientList.StressClients[byIndex].dwStatus = STATUS_STARTED;

                            // Send start message
                            ZeroMemory(&StressSessionMessage, sizeof(StressSessionMessage));
                            StressSessionMessage.dwMessageId = SERVER_START_MSG;
                            StressSessionMessage.dwStressType = pStressSession->StressClientList.StressClients[byIndex].dwStressType;
                            CopyMemory(&StressSessionMessage.XnAddr, &pStressSession->StressClientList.StressClients[byIndex].RemoteXnAddr, sizeof(StressSessionMessage.XnAddr));
                            NetsyncSendClientMessage(hNetsyncObject, NetsyncRemoteInAddr, sizeof(StressSessionMessage), (char *) &StressSessionMessage);
                        }

                        break;
                    }
                }
            }
        }
        else if (CLIENT_KEEPALIVE_MSG == ((PSTRESS_SESSION_MESSAGE) pMessage)->dwMessageId) {
            for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
                // Update the keep-alive for the client
                if (FromInAddr == pStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) {
                    pStressSession->StressClientList.StressClients[byIndex].dwLastKeepAlive = GetTickCount();

                    break;
                }
            }
        }
    }

    // Release critical section
    LeaveCriticalSection(&pStressSession->cs);

    return (0 != pStressSession->StressClientList.byClientCount);
}



LPVOID
WINAPI
XNetStressStart(
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

  Start the stress session

Arguments:

  hSessionObject - Handle to the session object
  hNetsyncObject - Handle to the netsync object
  byClientCount - Specifies the number of clients in the session
  ClientInAddrs - Pointer to an array of client ip addresses
  ClientXnAddrs - Pointer to an array of client xnet addresses
  LowPort - Specifies the low bound of the netsync port range
  HighPort - Specifies the high bound of the netsync port range

Return Value:

  LPVOID - Pointer to a context structure

------------------------------------------------------------------------------*/
{
    // hMemObject is a handle to the memory object
    HANDLE                   hMemObject = INVALID_HANDLE_VALUE;
    // pStressSession is a pointer to the stress session
    PSTRESS_SESSION          pStressSession = NULL;
    // StressSessionMessage is a message sent to the stress session
    STRESS_SESSION_MESSAGE   StressSessionMessage;
    // GenericMessage is a message sent to the main function
    NETSYNC_GENERIC_MESSAGE  GenericMessage;
    // byCurrentClient is a counter to enumerate the client list
    BYTE                     byCurrentClient;



    // Create the memory object
    hMemObject = xMemCreate();
    if (INVALID_HANDLE_VALUE == hMemObject) {
        return NULL;
    }

    // Create the stress session structure
    pStressSession = (PSTRESS_SESSION) xMemAlloc(hMemObject, sizeof(STRESS_SESSION));
    if (NULL == pStressSession) {
        // Close the memory object
        xMemClose(hMemObject);

        return NULL;
    }

    // Initialize the critical section
    InitializeCriticalSection(&pStressSession->cs);

    // Set the netsync object
    pStressSession->hSessionObject = hSessionObject;
    pStressSession->hNetsyncObject = hNetsyncObject;
    pStressSession->bExit = FALSE;

    // Create the server keep-alive thread
    pStressSession->hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) XNetStressKeepAlive, pStressSession, 0, NULL);
    if (NULL == pStressSession->hThread) {
        // Free the stress session structure
        xMemFree(hMemObject, pStressSession);

        // Close the memory object
        xMemClose(hMemObject);

        return NULL;
    }

    // Create the stress sessions mutex
    g_hStressSessionsMutex = CreateMutexA(NULL, FALSE, "XNetStressMutex");

    // Wait for access to the stress sessions
    WaitForSingleObject(g_hStressSessionsMutex, INFINITE);

    // Increment the count of stress sessions
    g_dwNumStressSessions++;

    // Add the stress session to the list
    pStressSession->pNextStressSession = g_pStressSessions;
    g_pStressSessions = pStressSession;

    // Release the stress sessions mutex
    ReleaseMutex(g_hStressSessionsMutex);

    // Send add client messages to the main function
    ZeroMemory(&GenericMessage, sizeof(GenericMessage));
    GenericMessage.dwMessageId = NETSYNC_MSG_ADDCLIENT;
    for (byCurrentClient = 0; byCurrentClient < byClientCount; byCurrentClient++) {
        XNetStressMain(hSessionObject, hNetsyncObject, ClientInAddrs[byCurrentClient], &ClientXnAddrs[byCurrentClient], NETSYNC_MSGTYPE_SERVER, sizeof(NETSYNC_GENERIC_MESSAGE), (char *) &GenericMessage, pStressSession);
    }

    return pStressSession;
}



VOID
WINAPI
XNetStressStop(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN LPVOID   lpContext
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Stops the stress session

Arguments:

  hSessionObject - Handle to the session object
  hNetsyncObject - Handle to the netsync object
  lpContext - Pointer to the context structure

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // hMemObject is a handle to the memory object
    HANDLE           hMemObject = INVALID_HANDLE_VALUE;

    // pStressSession is a pointer to the stress session
    PSTRESS_SESSION  pStressSession = (PSTRESS_SESSION) lpContext;
    // pPrevStressSession is a pointer to the previous stress session in the list
    PSTRESS_SESSION  pPrevStressSession = NULL;
    // pCurStressSession is a pointer to the current stress session in the list
    PSTRESS_SESSION  pCurStressSession = NULL;



    if (NULL != lpContext) {
        // Exit the keep-alive thread
        pStressSession->bExit = TRUE;
        WaitForSingleObject(pStressSession->hThread, INFINITE);
        CloseHandle(pStressSession->hThread);

        // Delete the critical section
        DeleteCriticalSection(&pStressSession->cs);

        // Create the stress sessions mutex
        g_hStressSessionsMutex = CreateMutexA(NULL, FALSE, "XNetStressMutex");

        // Wait for access to the stress sessions
        WaitForSingleObject(g_hStressSessionsMutex, INFINITE);

        // Set pCurStressSession to the head of the list
        pCurStressSession = g_pStressSessions;

        // Search the list for the current stress session
        while ((NULL != pCurStressSession) && (pCurStressSession != pStressSession)) {
            pPrevStressSession = pCurStressSession;
            pCurStressSession = pCurStressSession->pNextStressSession;
        }

        // Remove stress session from the list
        if ((NULL == pCurStressSession->pNextStressSession) && (NULL == pPrevStressSession)) {
            // Only stress session in the list
            g_pStressSessions = NULL;
        }
        else if (NULL == pCurStressSession->pNextStressSession) {
            // Stress session is tail of the list
            pPrevStressSession->pNextStressSession = NULL;
        }
        else if (NULL == pPrevStressSession) {
            // Stress session is head of the list
            g_pStressSessions = pCurStressSession->pNextStressSession;
        }
        else {
            // Stress session is middle of the list
            pPrevStressSession->pNextStressSession = pCurStressSession->pNextStressSession;
        }

        // Decrement the count of stress sessions
        g_dwNumStressSessions--;

        // Release the stress sessions mutex
        ReleaseMutex(g_hStressSessionsMutex);

        // Free the stress session structure
        hMemObject = pStressSession->hMemObject;
        xMemFree(hMemObject, pStressSession);

        // Close the memory object
        xMemClose(hMemObject);
    }
}



BOOL
WINAPI
XNetStressStatusMain(
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

  Gets the status of the stress sessions

Routine Description:

  Callback for the stress session

Arguments:

  hSessionObject - Handle to the session object
  hNetsyncObject - Handle to the netsync object
  FromAddr - Specifies the address of the netsync sender
  dwMessageType - Specifies the type of received message
  dwMessageSize - Specifies the size of the received message
  pMessage - Pointer to the received message
  lpContext - Pointer to the context structure

Return Value:

  BOOL:
    If the session should continue, the return value is TRUE.
    If the session should exit, the return value is FALSE.

------------------------------------------------------------------------------*/
{
    // StressStatusReplyMessage is the stress status reply message
    STRESS_STATUS_REPLY_MESSAGE    StressStatusReplyMessage;
    // StressStatusSessionMessage is the stress status session message
    STRESS_STATUS_SESSION_MESSAGE  StressStatusSessionMessage;
    // StressStatusClientMessage is the stress status client message
    STRESS_STATUS_CLIENT_MESSAGE   StressStatusClientMessage;
    // pCurStressSession is a pointer to the current stress session in the list
    PSTRESS_SESSION                pCurStressSession = NULL;
    // dwCurStressSession is the current stress session in the list
    DWORD                          dwCurStressSession = 0;
    // byIndex is a counter to enumerate each stress client
    BYTE                           byIndex;



    // Check if netsync server is stopping
    if ((NETSYNC_MSGTYPE_SERVER == dwMessageType) && (NETSYNC_MSG_STOPSESSION == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId)) {
        return FALSE;
    }

    if ((NETSYNC_MSGTYPE_CLIENT == dwMessageType) && (STRESS_STATUS_REQUEST_MSG == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId)) {
        // Create the stress sessions mutex
        g_hStressSessionsMutex = CreateMutexA(NULL, FALSE, "XNetStressMutex");

        // Wait for access to the stress sessions
        WaitForSingleObject(g_hStressSessionsMutex, INFINITE);

        // Set pCurStressSession to the head of the list
        pCurStressSession = g_pStressSessions;

        // Send the number of stress sessions
        StressStatusReplyMessage.dwMessageId = STRESS_STATUS_REPLY_MSG;
        StressStatusReplyMessage.dwNumStressSessions = g_dwNumStressSessions;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(StressStatusReplyMessage), (char *) &StressStatusReplyMessage);

        // Search the list for the current stress session
        while (NULL != pCurStressSession) {
            dwCurStressSession++;

            // Wait for the critical section
            EnterCriticalSection(&pCurStressSession->cs);

            StressStatusSessionMessage.dwMessageId = STRESS_STATUS_SESSION_MSG;
            StressStatusSessionMessage.dwNumStressSessions = g_dwNumStressSessions;
            StressStatusSessionMessage.dwCurStressSession = dwCurStressSession;
            StressStatusSessionMessage.dwNumStressClients = pCurStressSession->StressClientList.byClientCount;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(StressStatusSessionMessage), (char *) &StressStatusSessionMessage);

            StressStatusClientMessage.dwMessageId = STRESS_STATUS_CLIENT_MSG;
            StressStatusClientMessage.dwNumStressSessions = g_dwNumStressSessions;
            StressStatusClientMessage.dwCurStressSession = dwCurStressSession;
            StressStatusClientMessage.dwNumStressClients = pCurStressSession->StressClientList.byClientCount;

            // Enumerate the stress clients
            for (byIndex = 0; byIndex < NUM_STRESS_CLIENTS; byIndex++) {
                if (0 != pCurStressSession->StressClientList.StressClients[byIndex].NetsyncClientInAddr) {
                    StressStatusClientMessage.dwCurStressClient = byIndex + 1;

                    CopyMemory(&StressStatusClientMessage.StressClient, &pCurStressSession->StressClientList.StressClients[byIndex], sizeof(StressStatusClientMessage.StressClient));
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(StressStatusClientMessage), (char *) &StressStatusClientMessage);
                }
            }

            // Release critical section
            LeaveCriticalSection(&pCurStressSession->cs);

            // Get the next stress session
            pCurStressSession = pCurStressSession->pNextStressSession;
        }

        // Release the stress sessions mutex
        ReleaseMutex(g_hStressSessionsMutex);
    }

    return TRUE;
}



LPVOID
WINAPI
XNetStressStatusStart(
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

  Start the stress status session

Arguments:

  hSessionObject - Handle to the session object
  hNetsyncObject - Handle to the netsync object
  byClientCount - Specifies the number of clients in the session
  ClientInAddrs - Pointer to an array of client ip addresses
  ClientXnAddrs - Pointer to an array of client xnet addresses
  LowPort - Specifies the low bound of the netsync port range
  HighPort - Specifies the high bound of the netsync port range

Return Value:

  LPVOID - Pointer to a context structure

------------------------------------------------------------------------------*/
{
    return NULL;
}



VOID
WINAPI
XNetStressStatusStop(
    IN HANDLE   hSessionObject,
    IN HANDLE   hNetsyncObject,
    IN LPVOID   lpContext
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Stops the stress status session

Arguments:

  hSessionObject - Handle to the session object
  hNetsyncObject - Handle to the netsync object
  lpContext - Pointer to the context structure

Return Value:

  None

------------------------------------------------------------------------------*/
{
}
