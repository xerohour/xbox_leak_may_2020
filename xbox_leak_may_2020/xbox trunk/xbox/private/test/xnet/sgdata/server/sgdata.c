/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  sgdata.c

Abstract:

  This module validates the data through a security gateway

Author:

  Steven Kehrli (steveke) 24-Jan-2002

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace SGDataNamespace;

namespace SGDataNamespace {

SOCKET
CreateSocket(
    IN int     type,
    IN u_short sin_port
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Creates and binds a socket

Arguments:

  type - Specifies the type of socket (SOCK_DGRAM or SOCK_STREAM)
  sin_port - Specifies the bind port of socket

Return Value:

  SOCKET:
    If the function succeeds, the return value is a socket descriptor
    If the function fails, the return value is INVALID_SOCKET.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // sSocket is the new socket
    SOCKET       sSocket = INVALID_SOCKET;
    // iTimeout is the timeout value of the socket
    int          iTimeout;
    // localname is the local address
    SOCKADDR_IN  localname;
    // dwErrorCode is the last error code
    DWORD        dwErrorCode = ERROR_SUCCESS;



    // Create the socket
    sSocket = socket(AF_INET, type, 0);
    if (INVALID_SOCKET == sSocket) {
        // Get the last error code
        dwErrorCode = WSAGetLastError();

        goto FunctionExit;
    }

    // Set the socket timeouts
    iTimeout = 15000;
    if (SOCKET_ERROR == setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

    iTimeout = 15000;
    if (SOCKET_ERROR == setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

    // Setup the local address
    ZeroMemory(&localname, sizeof(localname));
    localname.sin_family = AF_INET;
    localname.sin_port = htons(sin_port);

    // Bind the socket
    if (SOCKET_ERROR == bind(sSocket, (SOCKADDR *) &localname, sizeof(localname))) {
        // Get the last error code
        dwErrorCode = WSAGetLastError();

        goto FunctionExit;
    }

FunctionExit:
    if (ERROR_SUCCESS != dwErrorCode) {
        if (INVALID_SOCKET != sSocket) {
            closesocket(sSocket);
            sSocket = INVALID_SOCKET;
        }

        SetLastError(dwErrorCode);
    }

    return sSocket;
}



DWORD
TestThread(
    IN LPVOID  lpv
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Thread to handle the test logic

Arguments:

  lpv - Pointer to the TEST_OBJECT

------------------------------------------------------------------------------*/
{
    // pTestObject is a pointer to the test object
    PTEST_OBJECT   pTestObject = (PTEST_OBJECT) lpv;
    // pUpdateTestObject is a pointer to the update test object
    PTEST_OBJECT   pUpdateTestObject;
    // SystemTime is the system time
    SYSTEMTIME     SystemTime;

    // hEvents is an array of events
    HANDLE         hEvents[2] = { pTestObject->hStopEvent, pTestObject->hRecvEvent };

    // RecvMessage is the recv message
    char           RecvMessage[1500];
    // MaskMessage is the mask message
    char           MaskMessage[1500];

    // WSAOverlapped is the overlapped I/O structure
    WSAOVERLAPPED  WSAOverlapped;
    // WSABuffer is the WSABUF structures
    WSABUF         WSABuffer;
    // fromname is the source address
    SOCKADDR_IN    fromname;
    // fromnamelen is the size of the source address
    int            fromnamelen;
    // dwBytes is the number of bytes received on the socket
    DWORD          dwBytes;
    // dwFlags is a bitmask of MSG_ values
    DWORD          dwFlags;
    // dwWaitResult is the result of the wait on the overlapped I/O event
    DWORD          dwWaitResult;
    // dwErrorCode is the last error code
    DWORD          dwErrorCode;



    do {
        // Setup the WSABUF structure
        ZeroMemory(RecvMessage, sizeof(RecvMessage));
        ZeroMemory(MaskMessage, sizeof(MaskMessage));
        WSABuffer.len = sizeof(RecvMessage);
        WSABuffer.buf = RecvMessage;

        // Setup the flags
        dwBytes = 0;
        dwFlags = 0;

        // Setup the address
        ZeroMemory(&fromname, sizeof(fromname));
        fromnamelen = sizeof(fromname);

        // Setup the overlapped I/O
        ZeroMemory(&WSAOverlapped, sizeof(WSAOverlapped));
        WSAOverlapped.hEvent = pTestObject->hRecvEvent;

        // Setup the result
        dwWaitResult = WAIT_FAILED;
        dwErrorCode = ERROR_SUCCESS;

        // Post the WSARecv request
        if (SOCKET_ERROR == WSARecvFrom(pTestObject->sSocket, &WSABuffer, 1, &dwBytes, &dwFlags, (SOCKADDR *) &fromname, &fromnamelen, &WSAOverlapped, NULL)) {
            if (WSA_IO_PENDING != WSAGetLastError()) {
                dwErrorCode = WSAGetLastError();

                continue;
            }

            // Wait for the overlapped I/O request to complete
            dwWaitResult = WaitForMultipleObjects(sizeof(hEvents) / sizeof(hEvents[0]), hEvents, FALSE, INFINITE);

            if ((WAIT_OBJECT_0 + 1) == dwWaitResult) {
                // Get the status of the overlapped I/O request
                if (FALSE == WSAGetOverlappedResult(pTestObject->sSocket, &WSAOverlapped, &dwBytes, FALSE, &dwFlags)) {
                    dwErrorCode = WSAGetLastError();

                    dwWaitResult = WAIT_FAILED;
                }
            }
            else if (WAIT_OBJECT_0 != dwWaitResult) {
                dwErrorCode = GetLastError();
            }

            if (ERROR_SUCCESS != dwErrorCode) {
                // Cancel the pending IO request
                CancelIo((HANDLE) pTestObject->sSocket);
            }
        }
        else {
            // WSARecv request completed immediately
            dwWaitResult = (WAIT_OBJECT_0 + 1);
        }

        if ((WAIT_OBJECT_0 + 1) == dwWaitResult) {
            // Update the test object
            EnterCriticalSection(pTestObject->pcs);

            if (SOCK_STREAM != pTestObject->type) {
                // Find the test object
                pUpdateTestObject = pTestObject->pPrevTestObject;

                while ((NULL != pUpdateTestObject) && (0 != memcmp(&pUpdateTestObject->Test_remotename, &fromname, sizeof(pUpdateTestObject->Test_remotename)))) {
                    pUpdateTestObject = pUpdateTestObject->pPrevTestObject;
                }

                if (NULL != pUpdateTestObject) {
                    // Update the byte count
                    pUpdateTestObject->dwUDPBytes += dwBytes;
                }
            }
            else {
                // Find the test object
                pUpdateTestObject = pTestObject;

                // Update the byte count
                pUpdateTestObject->dwTCPBytes += dwBytes;
            }

            if (NULL != pUpdateTestObject) {
                // Update the time
                GetSystemTime(&SystemTime);
                SystemTimeToTzSpecificLocalTime(NULL, &SystemTime, &pUpdateTestObject->LastActivityTime);

                // Update the test object
                SendMessage(pTestObject->hWnd, UM_UPDATE_TEST, 0, (LPARAM) pUpdateTestObject);

                // Write the activity to the log file
                LogActivity(pTestObject->hFile, &pUpdateTestObject->LastActivityTime, pUpdateTestObject->RemoteName, (SOCK_DGRAM == pTestObject->type) ? LOG_UDP_DATA : LOG_TCP_DATA, dwBytes);
            }

            LeaveCriticalSection(pTestObject->pcs);

            // Copy the message to the mask buffer
            CopyMemory(MaskMessage, RecvMessage, dwBytes);

            // Mask the message
            DataMask((BYTE *) MaskMessage, dwBytes);

            // Echo the message
            sendto(pTestObject->sSocket, RecvMessage, dwBytes, 0, (SOCKADDR *) &fromname, fromnamelen);
            sendto(pTestObject->sSocket, MaskMessage, dwBytes, 0, (SOCKADDR *) &fromname, fromnamelen);
        }
    } while (WAIT_OBJECT_0 != dwWaitResult);

    return 0;
}



VOID
CloseTest(
    IN PTEST_OBJECT  pTestObject
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Closes a test object

Arguments:

  pTestObject - Pointer to the test object

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // hMemObject is a handle to the memory object
    HANDLE  hMemObject = pTestObject->hMemObject;



    // Signal the stop event
    if (NULL != pTestObject->hStopEvent) {
        SetEvent(pTestObject->hStopEvent);
    }

    if (NULL != pTestObject->hTestThread) {
        // Wait for the test thread to exit
        WaitForSingleObject(pTestObject->hTestThread, INFINITE);

        // Close the thread handle
        CloseHandle(pTestObject->hTestThread);
    }

    // Close the recv event
    if (NULL != pTestObject->hRecvEvent) {
        CloseHandle(pTestObject->hRecvEvent);
    }

    // Close the stop event
    if (NULL != pTestObject->hStopEvent) {
        CloseHandle(pTestObject->hStopEvent);
    }

    // Close the socket
    if ((SOCK_STREAM == pTestObject->type) && (INVALID_SOCKET != pTestObject->sSocket)) {
        shutdown(pTestObject->sSocket, SD_BOTH);
        closesocket(pTestObject->sSocket);
    }

    // Free the test object
    xMemFree(hMemObject, pTestObject);
}



PTEST_OBJECT
CreateTest(
    IN CRITICAL_SECTION  *pcs,
    IN HWND              hWnd,
    IN HANDLE            hMemObject,
    IN HANDLE            hFile,
    IN SOCKADDR_IN       *remotename,
    IN int               type,
    IN SOCKET            sSocket,
    IN u_short           sin_port
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Create a session

Arguments:

  pcs - Pointer to the object to synchronize access to the test objects
  hWnd - Handle to the parent window
  hMemObject - Handle to the memory object
  hFile - Handle to the log file
  remotename - Specifies the session remote name
  type - Specifies the type of socket (SOCK_DGRAM or SOCK_STREAM)
  sSocket - Specifies the socket descriptor
  sin_port - Specifies the port

Return Value:

  PTEST_OBJECT:
    If the function succeeds, the return value is a pointer to the test object.
    If the function fails, the return value is NULL.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pTestObject is a pointer to the test object
    PTEST_OBJECT  pTestObject = NULL;
    // readfds is the read socket set
    fd_set        readfds;
    // timeout it the select timeout
    timeval       timeout = { 0, 0 };
    // remotenamelen is the remote name len
    int           remotenamelen;
    // SystemTime is the current system time
    SYSTEMTIME    SystemTime;
    // dwErrorCode is the last error code
    DWORD         dwErrorCode = ERROR_SUCCESS;



    // Create the test object
    pTestObject = (PTEST_OBJECT) xMemAlloc(hMemObject, sizeof(TEST_OBJECT));
    if (NULL == pTestObject) {
        // Get the last error
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

    // Set the session object
    pTestObject->pcs = pcs;
    pTestObject->hWnd = hWnd;
    pTestObject->hMemObject = hMemObject;
    pTestObject->hFile = hFile;
    pTestObject->type = type;
    pTestObject->sSocket = INVALID_SOCKET;
    pTestObject->sin_port = sin_port;

    // Get the session remote name
    if (NULL != remotename) {
        CopyMemory(&pTestObject->Session_remotename, remotename, sizeof(pTestObject->Session_remotename));
    }

    // Get the socket
    if (SOCK_STREAM == type) {
        // Check for pending connection
        FD_ZERO(&readfds);
        FD_SET(sSocket, &readfds);

        select(0, &readfds, NULL, NULL, &timeout);

        if (0 == FD_ISSET(sSocket, &readfds)) {
            // Get the last error
            dwErrorCode = WSAEWOULDBLOCK;

            goto FunctionExit;
        }

        // Accept pending connection
        remotenamelen = sizeof(pTestObject->Test_remotename);
        pTestObject->sSocket = accept(sSocket, (SOCKADDR *) &pTestObject->Test_remotename, &remotenamelen);
        if (INVALID_SOCKET == pTestObject->sSocket) {
            // Get the last error
            dwErrorCode = WSAGetLastError();

            goto FunctionExit;
        }

        wsprintf(pTestObject->RemoteName, L"%d.%d.%d.%d:%u", pTestObject->Test_remotename.sin_addr.S_un.S_un_b.s_b1, pTestObject->Test_remotename.sin_addr.S_un.S_un_b.s_b2, pTestObject->Test_remotename.sin_addr.S_un.S_un_b.s_b3, pTestObject->Test_remotename.sin_addr.S_un.S_un_b.s_b4, ntohs(pTestObject->Test_remotename.sin_port));
    }
    else {
        pTestObject->sSocket = sSocket;
    }

    // Create the stop event
    pTestObject->hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == pTestObject->hStopEvent) {
        // Get the last error
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

    // Create the recv event
    pTestObject->hRecvEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == pTestObject->hRecvEvent) {
        // Get the last error
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

    // Set the times
    GetSystemTime(&SystemTime);
    SystemTimeToTzSpecificLocalTime(NULL, &SystemTime, &pTestObject->FirstActivityTime);
    CopyMemory(&pTestObject->LastActivityTime, &pTestObject->FirstActivityTime, sizeof(pTestObject->LastActivityTime));

    // Create the test thread
    pTestObject->hTestThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) TestThread, pTestObject, 0, NULL);
    if (NULL == pTestObject->hTestThread) {
        // Get the last error
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

FunctionExit:
    if (ERROR_SUCCESS != dwErrorCode) {
        if (NULL != pTestObject) {
            // Close the test object
            CloseTest(pTestObject);
            pTestObject = NULL;
        }

        // Set the last error code
        SetLastError(dwErrorCode);
    }

    return pTestObject;
}



DWORD
SessionThread(
    IN LPVOID  lpv
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Thread to handle the session logic

Arguments:

  lpv - Pointer to the SESSION_OBJECT

------------------------------------------------------------------------------*/
{
    // pSessionObject is a pointer to the session object
    PSESSION_OBJECT  pSessionObject = (PSESSION_OBJECT) lpv;
    // pTestObject is a pointer to the test object
    PTEST_OBJECT     pTestObject;

    // hEvents is an array of events
    HANDLE           hEvents[2] = { pSessionObject->hStopEvent, pSessionObject->hRecvEvent };

    // SGDataMessage is the session message
    SGDATA_MESSAGE   SGDataMessage;

    // WSAOverlapped is the overlapped I/O structure
    WSAOVERLAPPED    WSAOverlapped;
    // WSABuffer is the WSABUF structures
    WSABUF           WSABuffer;
    // fromname is the source address
    SOCKADDR_IN      fromname;
    // fromnamelen is the size of the source address
    int              fromnamelen;
    // dwBytes is the number of bytes received on the socket
    DWORD            dwBytes;
    // dwFlags is a bitmask of MSG_ values
    DWORD            dwFlags;
    // dwWaitResult is the result of the wait on the overlapped I/O event
    DWORD            dwWaitResult;
    // dwErrorCode is the last error code
    DWORD            dwErrorCode;



    do {
        // Setup the WSABUF structure
        ZeroMemory(&SGDataMessage, sizeof(SGDataMessage));
        WSABuffer.len = sizeof(SGDataMessage);
        WSABuffer.buf = (char *) &SGDataMessage;

        // Setup the flags
        dwBytes = 0;
        dwFlags = 0;

        // Setup the address
        ZeroMemory(&fromname, sizeof(fromname));
        fromnamelen = sizeof(fromname);

        // Setup the overlapped I/O
        ZeroMemory(&WSAOverlapped, sizeof(WSAOverlapped));
        WSAOverlapped.hEvent = pSessionObject->hRecvEvent;

        // Setup the result
        dwWaitResult = WAIT_FAILED;
        dwErrorCode = ERROR_SUCCESS;

        // Post the WSARecv request
        if (SOCKET_ERROR == WSARecvFrom(pSessionObject->sSessionSocket, &WSABuffer, 1, &dwBytes, &dwFlags, (SOCKADDR *) &fromname, &fromnamelen, &WSAOverlapped, NULL)) {
            if (WSA_IO_PENDING != WSAGetLastError()) {
                dwErrorCode = WSAGetLastError();

                continue;
            }

            // Wait for the overlapped I/O request to complete
            dwWaitResult = WaitForMultipleObjects(sizeof(hEvents) / sizeof(hEvents[0]), hEvents, FALSE, INFINITE);

            if ((WAIT_OBJECT_0 + 1) == dwWaitResult) {
                // Get the status of the overlapped I/O request
                if (FALSE == WSAGetOverlappedResult(pSessionObject->sSessionSocket, &WSAOverlapped, &dwBytes, FALSE, &dwFlags)) {
                    dwErrorCode = WSAGetLastError();

                    dwWaitResult = WAIT_FAILED;
                }
            }
            else if (WAIT_OBJECT_0 != dwWaitResult) {
                dwErrorCode = GetLastError();
            }

            if (ERROR_SUCCESS != dwErrorCode) {
                // Cancel the pending IO request
                CancelIo((HANDLE) pSessionObject->sSessionSocket);
            }
        }
        else {
            // WSARecv request completed immediately
            dwWaitResult = (WAIT_OBJECT_0 + 1);
        }

        if ((WAIT_OBJECT_0 + 1) == dwWaitResult) {
            if (SGDATA_ENUM_MSG == SGDataMessage.dwMessageId) {
                // Send the reply
                ZeroMemory(&SGDataMessage, sizeof(SGDataMessage));
                SGDataMessage.dwMessageId = SGDATA_ENUM_REPLY;
                sendto(pSessionObject->sSessionSocket, (char *) &SGDataMessage, sizeof(SGDataMessage), 0, (SOCKADDR *) &fromname, fromnamelen);
            }
            else if (SGDATA_CONNECT_MSG == SGDataMessage.dwMessageId) {
                // Create the test object
                EnterCriticalSection(pSessionObject->pcs);

                ZeroMemory(&SGDataMessage, sizeof(SGDataMessage));

                pTestObject = CreateTest(pSessionObject->pcs, pSessionObject->hWnd, pSessionObject->hMemObject, pSessionObject->hFile, &fromname, SOCK_STREAM, pSessionObject->sTCPSocket, pSessionObject->sin_TCPPort);

                // Add the test object to the list of test objects
                if (NULL != pTestObject) {
                    pTestObject->pNextTestObject = pSessionObject->pTestObject;
                    pSessionObject->pTestObject->pPrevTestObject = pTestObject;
                    pSessionObject->pTestObject = pTestObject;

                    // Write the activity to the log file
                    LogActivity(pTestObject->hFile, &pTestObject->LastActivityTime, pTestObject->RemoteName, LOG_CONNECTED, 0);

                    // Update the test object
                    SendMessage(pSessionObject->hWnd, UM_UPDATE_TEST, 0, (LPARAM) pTestObject);

                    SGDataMessage.dwMessageId = SGDATA_CONNECT_SUCCESS;
                    SGDataMessage.dwErrorCode = ERROR_SUCCESS;
                }
                else {
                    SGDataMessage.dwMessageId = SGDATA_CONNECT_FAILED;
                    SGDataMessage.dwErrorCode = GetLastError();
                }

                LeaveCriticalSection(pSessionObject->pcs);

                // Send the reply
                sendto(pSessionObject->sSessionSocket, (char *) &SGDataMessage, sizeof(SGDataMessage), 0, (SOCKADDR *) &fromname, fromnamelen);
            }
            else if (SGDATA_DISCONNECT_MSG == SGDataMessage.dwMessageId) {
                // Find the test object
                EnterCriticalSection(pSessionObject->pcs);

                pTestObject = pSessionObject->pTestObject;

                while ((NULL != pTestObject) && (0 != memcmp(&pTestObject->Session_remotename, &fromname, sizeof(pTestObject->Session_remotename)))) {
                    pTestObject = pTestObject->pNextTestObject;
                }

                // Close the test object
                if (NULL != pTestObject) {
                    // Write the activity to the log file
                    LogActivity(pTestObject->hFile, &pTestObject->LastActivityTime, pTestObject->RemoteName, LOG_DISCONNECTED, 0);

                    SendMessage(pSessionObject->hWnd, UM_DELETE_TEST, 0, (LPARAM) pTestObject);
                }

                LeaveCriticalSection(pSessionObject->pcs);

                // Send the reply
                ZeroMemory(&SGDataMessage, sizeof(SGDataMessage));
                SGDataMessage.dwMessageId = SGDATA_DISCONNECT_REPLY;
                sendto(pSessionObject->sSessionSocket, (char *) &SGDataMessage, sizeof(SGDataMessage), 0, (SOCKADDR *) &fromname, fromnamelen);
            }
        }
    } while (WAIT_OBJECT_0 != dwWaitResult);

    return 0;
}



VOID
CloseSession(
    IN PSESSION_OBJECT  pSessionObject
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Closes a session object

Arguments:

  pSessionObject - Pointer to the session object

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // hMemObject is a handle to the memory object
    HANDLE        hMemObject = pSessionObject->hMemObject;
    // pTestObject is a pointer to the test object
    PTEST_OBJECT  pTestObject = NULL;



    // Signal the stop event
    if (NULL != pSessionObject->hStopEvent) {
        SetEvent(pSessionObject->hStopEvent);
    }

    if (NULL != pSessionObject->hSessionThread) {
        // Wait for the session thread to exit
        WaitForSingleObject(pSessionObject->hSessionThread, INFINITE);

        // Close the thread handle
        CloseHandle(pSessionObject->hSessionThread);
    }

    // Close the test objects
    EnterCriticalSection(pSessionObject->pcs);

    pTestObject = pSessionObject->pTestObject;
    while (NULL != pTestObject) {
        // Get the next test object
        pSessionObject->pTestObject = pSessionObject->pTestObject->pNextTestObject;

        // Close the test object
        CloseTest(pTestObject);

        // Set the next test object
        pTestObject = pSessionObject->pTestObject;
    }

    LeaveCriticalSection(pSessionObject->pcs);

    // Close the TCP socket
    if (INVALID_SOCKET != pSessionObject->sTCPSocket) {
        shutdown(pSessionObject->sTCPSocket, SD_BOTH);
        closesocket(pSessionObject->sTCPSocket);
    }

    // Close the UDP socket
    if (INVALID_SOCKET != pSessionObject->sUDPSocket) {
        shutdown(pSessionObject->sUDPSocket, SD_BOTH);
        closesocket(pSessionObject->sUDPSocket);
    }

    // Close the session socket
    if (INVALID_SOCKET != pSessionObject->sSessionSocket) {
        shutdown(pSessionObject->sSessionSocket, SD_BOTH);
        closesocket(pSessionObject->sSessionSocket);
    }

    // Close the recv event
    if (NULL != pSessionObject->hRecvEvent) {
        CloseHandle(pSessionObject->hRecvEvent);
    }

    // Close the stop event
    if (NULL != pSessionObject->hStopEvent) {
        CloseHandle(pSessionObject->hStopEvent);
    }

    // Free the session object
    xMemFree(hMemObject, pSessionObject);
}



PSESSION_OBJECT
CreateSession(
    IN CRITICAL_SECTION  *pcs,
    IN HWND              hWnd,
    IN HANDLE            hMemObject,
    IN HANDLE            hFile
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Create a session

Arguments:

  pcs - Pointer to the object to synchronize access to the test objects
  hWnd - Handle to the parent window
  hMemObject - Handle to the memory object
  hFile - Handle to the log file

Return Value:

  PSESSION_OBJECT:
    If the function succeeds, the return value is a pointer to the session object.
    If the function fails, the return value is NULL.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pSessionObject is a pointer to the session object
    PSESSION_OBJECT  pSessionObject = NULL;
    // dwErrorCode is the last error code
    DWORD            dwErrorCode = ERROR_SUCCESS;



    // Create the session object
    pSessionObject = (PSESSION_OBJECT) xMemAlloc(hMemObject, sizeof(SESSION_OBJECT));
    if (NULL == pSessionObject) {
        // Get the last error
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

    // Set the session object
    pSessionObject->pcs = pcs;
    pSessionObject->hWnd = hWnd;
    pSessionObject->hMemObject = hMemObject;
    pSessionObject->hFile = hFile;
    pSessionObject->sSessionSocket = INVALID_SOCKET;
    pSessionObject->sin_SessionPort = PORT_SESSION;
    pSessionObject->sSessionSocket = INVALID_SOCKET;
    pSessionObject->sin_UDPPort = PORT_UDP;
    pSessionObject->sSessionSocket = INVALID_SOCKET;
    pSessionObject->sin_TCPPort = PORT_TCP;

    // Create the stop event
    pSessionObject->hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == pSessionObject->hStopEvent) {
        // Get the last error
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

    // Create the recv event
    pSessionObject->hRecvEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == pSessionObject->hRecvEvent) {
        // Get the last error
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

    // Create the session socket
    pSessionObject->sSessionSocket = CreateSocket(SOCK_DGRAM, pSessionObject->sin_SessionPort);
    if (INVALID_SOCKET == pSessionObject->sSessionSocket) {
        // Get the last error
        dwErrorCode = WSAGetLastError();

        goto FunctionExit;
    }

    // Create the UDP socket
    pSessionObject->sUDPSocket = CreateSocket(SOCK_DGRAM, pSessionObject->sin_UDPPort);
    if (INVALID_SOCKET == pSessionObject->sUDPSocket) {
        // Get the last error
        dwErrorCode = WSAGetLastError();

        goto FunctionExit;
    }

    // Create the TCP socket
    pSessionObject->sTCPSocket = CreateSocket(SOCK_STREAM, pSessionObject->sin_TCPPort);
    if (INVALID_SOCKET == pSessionObject->sTCPSocket) {
        // Get the last error
        dwErrorCode = WSAGetLastError();

        goto FunctionExit;
    }

    // Listen on the TCP socket
    if (SOCKET_ERROR == listen(pSessionObject->sTCPSocket, 0)) {
        // Get the last error
        dwErrorCode = WSAGetLastError();

        goto FunctionExit;
    }

    // Enter the critical section
    EnterCriticalSection(pSessionObject->pcs);

    // Create the test object
    pSessionObject->pTestObject = CreateTest(pSessionObject->pcs, pSessionObject->hWnd, pSessionObject->hMemObject, pSessionObject->hFile, NULL, SOCK_DGRAM, pSessionObject->sUDPSocket, pSessionObject->sin_UDPPort);
    if (NULL == pSessionObject->pTestObject) {
        // Get the last error
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

    // Leave the critical section
    LeaveCriticalSection(pSessionObject->pcs);

    // Create the session thread
    pSessionObject->hSessionThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SessionThread, pSessionObject, 0, NULL);
    if (NULL == pSessionObject->hSessionThread) {
        // Get the last error
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

FunctionExit:
    if (ERROR_SUCCESS != dwErrorCode) {
        if (NULL != pSessionObject) {
            // Close the session object
            CloseSession(pSessionObject);
            pSessionObject = NULL;
        }

        // Set the last error code
        SetLastError(dwErrorCode);
    }

    return pSessionObject;
}



int CALLBACK
SortTestObjects(
    LPARAM  lParam1,
    LPARAM  lParam2,
    LPARAM  lParamSort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Sorts the test objects in the status list

Arguments:

  lParam1 - Specifies the first lParam (pointer to the first test object)
  lParam2 - Specifies the second lParam (pointer to the second test object)
  lParamSort - Specifies the sort method

Return Value:

  int:
    If the first item should precede the second, negative value (-1)
    If the second item should precede the first, positive value (+1)
    If the two items are equivalent, 0

------------------------------------------------------------------------------*/
{
    // ActivityTime1 is the time of activity for the first test object
    ULARGE_INTEGER  ActivityTime1;
    // LastActivityTime2 is the time of activity for the second test object
    ULARGE_INTEGER  ActivityTime2;



    switch (lParamSort) {
        case eSortRemoteNameAscending:
            return lstrcmp(((PTEST_OBJECT) lParam1)->RemoteName, ((PTEST_OBJECT) lParam2)->RemoteName);
            break;

        case eSortRemoteNameDescending:
            return lstrcmp(((PTEST_OBJECT) lParam2)->RemoteName, ((PTEST_OBJECT) lParam1)->RemoteName);
            break;

        case eSortUDPBytesAscending:
            if (((PTEST_OBJECT) lParam1)->dwUDPBytes < ((PTEST_OBJECT) lParam2)->dwUDPBytes) {
                return -1;
            }
            else if (((PTEST_OBJECT) lParam1)->dwUDPBytes > ((PTEST_OBJECT) lParam2)->dwUDPBytes) {
                return 1;
            }

            break;

        case eSortUDPBytesDescending:
            if (((PTEST_OBJECT) lParam2)->dwUDPBytes < ((PTEST_OBJECT) lParam1)->dwUDPBytes) {
                return -1;
            }
            else if (((PTEST_OBJECT) lParam2)->dwUDPBytes > ((PTEST_OBJECT) lParam1)->dwUDPBytes) {
                return 1;
            }

            break;


        case eSortTCPBytesAscending:
            if (((PTEST_OBJECT) lParam1)->dwTCPBytes < ((PTEST_OBJECT) lParam2)->dwTCPBytes) {
                return -1;
            }
            else if (((PTEST_OBJECT) lParam1)->dwTCPBytes > ((PTEST_OBJECT) lParam2)->dwTCPBytes) {
                return 1;
            }

            break;


        case eSortTCPBytesDescending:
            if (((PTEST_OBJECT) lParam2)->dwTCPBytes < ((PTEST_OBJECT) lParam1)->dwTCPBytes) {
                return -1;
            }
            else if (((PTEST_OBJECT) lParam2)->dwTCPBytes > ((PTEST_OBJECT) lParam1)->dwTCPBytes) {
                return 1;
            }

            break;

        case eSortFirstActivityTimeAscending:
            // Get the time of activity
            SystemTimeToFileTime(&((PTEST_OBJECT) lParam1)->FirstActivityTime, (FILETIME *) &ActivityTime1);
            SystemTimeToFileTime(&((PTEST_OBJECT) lParam2)->FirstActivityTime, (FILETIME *) &ActivityTime2);

            if (ActivityTime1.QuadPart < ActivityTime2.QuadPart) {
                return -1;
            }
            else if (ActivityTime1.QuadPart > ActivityTime2.QuadPart) {
                return 1;
            }

            break;

        case eSortFirstActivityTimeDescending:
            // Get the time of activity
            SystemTimeToFileTime(&((PTEST_OBJECT) lParam1)->FirstActivityTime, (FILETIME *) &ActivityTime1);
            SystemTimeToFileTime(&((PTEST_OBJECT) lParam2)->FirstActivityTime, (FILETIME *) &ActivityTime2);

            if (ActivityTime2.QuadPart < ActivityTime1.QuadPart) {
                return -1;
            }
            else if (ActivityTime2.QuadPart > ActivityTime1.QuadPart) {
                return 1;
            }

            break;

        case eSortLastActivityTimeAscending:
            // Get the time of last activity
            SystemTimeToFileTime(&((PTEST_OBJECT) lParam1)->LastActivityTime, (FILETIME *) &ActivityTime1);
            SystemTimeToFileTime(&((PTEST_OBJECT) lParam2)->LastActivityTime, (FILETIME *) &ActivityTime2);

            if (ActivityTime1.QuadPart < ActivityTime2.QuadPart) {
                return -1;
            }
            else if (ActivityTime1.QuadPart > ActivityTime2.QuadPart) {
                return 1;
            }

            break;

        case eSortLastActivityTimeDescending:
            // Get the time of last activity
            SystemTimeToFileTime(&((PTEST_OBJECT) lParam1)->LastActivityTime, (FILETIME *) &ActivityTime1);
            SystemTimeToFileTime(&((PTEST_OBJECT) lParam2)->LastActivityTime, (FILETIME *) &ActivityTime2);

            if (ActivityTime2.QuadPart < ActivityTime1.QuadPart) {
                return -1;
            }
            else if (ActivityTime2.QuadPart > ActivityTime1.QuadPart) {
                return 1;
            }

            break;

        default:
            break;
    }

    return 0;
}



LRESULT CALLBACK DialogProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Dialog procedure for the main window

-----------------------------------------------------------------------------*/
{
    // cs is the object to synchronize access to the test objects
    static CRITICAL_SECTION  cs;
    // bcsInitialized indicates the cs is initialized
    static BOOL              bcsInitialized = FALSE;
    // hMemObject is the handle to the memory object
    static HANDLE            hMemObject = INVALID_HANDLE_VALUE;
    // hFile is the handle to the log file
    static HANDLE            hFile = INVALID_HANDLE_VALUE;
    // WSAData is the details of the Winsock implementation
    WSADATA                  WSAData;
    // bWinsockInitialized indicates Winsock is initialized
    static BOOL              bWinsockInitialized = FALSE;
    // pSessionObject is the pointer to the session object
    static PSESSION_OBJECT   pSessionObject = NULL;
    // bTimerInitialized indicates the timer is initialized
    static BOOL              bTimerInitialized = FALSE;
    // bDlgInitialized indicates the dialog is initialized
    static BOOL              bDlgInitialized = TRUE;

    // pTestObject is a pointer to the test object
    PTEST_OBJECT             pTestObject;
    // pNextTestObject is a pointer to the next test object
    PTEST_OBJECT             pNextTestObject;
    // SystemTime is the system time
    SYSTEMTIME               SystemTime;
    // CurrentTime is the local time
    SYSTEMTIME               CurrentTime;
    // CurrentActivityTime is the current time of activity
    ULARGE_INTEGER           CurrentActivityTime;
    // LastActivityTime is the last time of activity
    ULARGE_INTEGER           LastActivityTime;

    // hWndList is a handle to the status list
    static HWND              hWndList = NULL;
    // eCurrentSortMethod is the current sort method of the status list
    static eSortMethod       eCurrentSortMethod = eSortRemoteNameAscending;
    // rcClient is the WindowRect of the status list
    RECT                     rcClient;
    // lvc specifies the attributes of a particular column of the status list
    LV_COLUMN                lvc;
    // lvi specifies the attributes of a particular item of the status list
    LV_ITEM                  lvi;
    // lvfi specifies the attributes of a particular item to find in the status list
    LV_FINDINFO              lvfi;
    // dwListIndex is the index of a particular item in the status list
    DWORD                    dwListIndex;
    // pnmhdr is a pointer to the WM_NOTIFY message header
    LPNMHDR                  pnmhdr;
    // pnmlistview is a pointer to the LVN_COLUMNCLICK message
    LPNMLISTVIEW             pnmlistview;
    // bSort indicates status list needs to be sorted
    BOOL                     bSort;

    // szText is the text string buffer
    WCHAR                    szText[256];
    // dwTextLen is the length of the text string buffer
    DWORD                    dwTextLen;
    // szError is the error string buffer
    WCHAR                    szError[256];
    // dwErrorCode is the last error code
    DWORD                    dwErrorCode = ERROR_SUCCESS;



    switch (iMsg) {

        case UM_DIALOG_INITIALIZE:
            // Get the handle to the status list
            hWndList = GetDlgItem(hDlg, IDC_STATUS_LIST);

            // Set the status list to select the full row
            ListView_SetExtendedListViewStyle(hWndList, LVS_EX_FULLROWSELECT);

            // Get the Rect of the Status List
            GetWindowRect(hWndList, &rcClient);

            // Set common attributes for each column
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            lvc.fmt = LVCFMT_LEFT;

            // Remote Name Column
            // Load the text
            GetResourceString(IDS_REMOTENAME_LABEL, szText, sizeof(szText) / sizeof(WCHAR));
            // Set the column text
            lvc.pszText = szText;
            // Set the width of the column to be about 1 / 7 of the width of the status list, allowing for the width of the borders and scroll bar
            lvc.cx = (rcClient.right - rcClient.left - 4 * GetSystemMetrics(SM_CXBORDER) - GetSystemMetrics(SM_CXVSCROLL)) / 7;
            // Indicate this is the first column
            lvc.iSubItem = eColumnRemoteName;
            // Insert column
            ListView_InsertColumn(hWndList, lvc.iSubItem, &lvc);

            // Set common attributes for each column
            lvc.fmt = LVCFMT_RIGHT;

            // UDP Byte Count Column
            // Load the text
            GetResourceString(IDS_UDPBYTECOUNT_LABEL, szText, sizeof(szText) / sizeof(WCHAR));
            // Set the column text
            lvc.pszText = szText;
            // Indicate this is the second column
            lvc.iSubItem = eColumnUDPBytes;
            // Insert column
            ListView_InsertColumn(hWndList, lvc.iSubItem, &lvc);

            // TCP Byte Count Column
            // Load the text
            GetResourceString(IDS_TCPBYTECOUNT_LABEL, szText, sizeof(szText) / sizeof(WCHAR));
            // Set the column text
            lvc.pszText = szText;
            // Indicate this is the third column
            lvc.iSubItem = eColumnTCPBytes;
            // Insert column
            ListView_InsertColumn(hWndList, lvc.iSubItem, &lvc);

            // First Activity Time Column
            // Load the text
            GetResourceString(IDS_FIRSTACTIVITYTIME_LABEL, szText, sizeof(szText) / sizeof(WCHAR));
            // Set the column text
            lvc.pszText = szText;
            // Set the width of the column to be about 2 / 7 of the width of the status list, allowing for the width of the borders and scroll bar
            lvc.cx *= 2;
            // Indicate this is the fourth column
            lvc.iSubItem = eColumnFirstActivityTime;
            // Insert column
            ListView_InsertColumn(hWndList, lvc.iSubItem, &lvc);

            // Last Activity Time Column
            // Load the text
            GetResourceString(IDS_LASTACTIVITYTIME_LABEL, szText, sizeof(szText) / sizeof(WCHAR));
            // Set the column text
            lvc.pszText = szText;
            // Indicate this is the fourth column
            lvc.iSubItem = eColumnLastActivityTime;
            // Insert column
            ListView_InsertColumn(hWndList, lvc.iSubItem, &lvc);

            // Initialize the critical section
            InitializeCriticalSection(&cs);
            bcsInitialized = TRUE;

            // Set the caption
            GetResourceString(IDS_SGDATA_CAPTION, szText, sizeof(szText) / sizeof(WCHAR));
            SetWindowText(hDlg, szText);

            // Create the memory object
            hMemObject = xMemCreate();
            if (INVALID_HANDLE_VALUE == hMemObject) {
                // Get the last error
                dwErrorCode = GetLastError();

                // Get the resource string buffer
                GetResourceString(IDS_MEMORY_INIT_FAILED, szError, sizeof(szError) / sizeof(WCHAR), dwErrorCode);

                goto UM_DIALOG_INITIALIZE_Exit;
            }

            // Create the log file
            hFile = CreateFile(SGDATA_LOGFILE_NAME, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (INVALID_HANDLE_VALUE == hFile) {
                // Get the last error
                dwErrorCode = GetLastError();

                // Get the resource string buffer
                GetResourceString(IDS_LOGFILE_INIT_FAILED, szError, sizeof(szError) / sizeof(WCHAR), dwErrorCode);

                goto UM_DIALOG_INITIALIZE_Exit;
            }

            // Write the log file header
            LogActivityHeader(hFile);

            // Initialize Winsock
            ZeroMemory(&WSAData, sizeof(WSAData));
            dwErrorCode = WSAStartup(MAKEWORD(2, 2), &WSAData);
            if (ERROR_SUCCESS != dwErrorCode) {
                // Get the resource string buffer
                GetResourceString(IDS_WINSOCK_INIT_FAILED, szError, sizeof(szError) / sizeof(WCHAR), dwErrorCode);

                goto UM_DIALOG_INITIALIZE_Exit;
            }
            else {
                bWinsockInitialized = TRUE;
            }

            // Create the session
            pSessionObject = CreateSession(&cs, hDlg, hMemObject, hFile);
            if (NULL == pSessionObject) {
                // Get the last error
                dwErrorCode = GetLastError();

                // Get the resource string buffer
                GetResourceString(IDS_SESSION_INIT_FAILED, szError, sizeof(szError) / sizeof(WCHAR), dwErrorCode);

                goto UM_DIALOG_INITIALIZE_Exit;
            }

            // Create the timer
            SetTimer(hDlg, 0, TIMER_TIMEOUT, NULL);
            bTimerInitialized = TRUE;

UM_DIALOG_INITIALIZE_Exit:
            if (ERROR_SUCCESS != dwErrorCode) {
                // Display the error message
                GetResourceString(IDS_SGDATA_CAPTION, szText, sizeof(szText) / sizeof(WCHAR));
                MessageBox(NULL, szError, szText, MB_OK | MB_ICONERROR);

                // End the dialog
                bDlgInitialized = FALSE;
                DestroyWindow(hDlg);
                PostQuitMessage(dwErrorCode);
            }


            break;

        case UM_UPDATE_TEST:
            // Get the test object
            pTestObject = (PTEST_OBJECT) lParam;

            // Search the lParam
            lvfi.flags = LVFI_PARAM;

            // Set the search criteria
            lvfi.lParam = (long) pTestObject;

            // Find the item in the status list
            dwListIndex = ListView_FindItem(hWndList, -1, &lvfi);
            if (-1 == dwListIndex) {
                // Insert the item

                // Indicate pszText and lParam are valid
                lvi.mask = LVIF_TEXT | LVIF_PARAM;

                // Set the text
                lvi.pszText = pTestObject->RemoteName;
                // Set the lParam
                lvi.lParam = (long) pTestObject;
                // Set the item number
                lvi.iItem = ListView_GetItemCount(hWndList);
                // Indicate this is the first column
                lvi.iSubItem = eColumnRemoteName;
                // Insert item
                ListView_InsertItem(hWndList, &lvi);

                bSort = TRUE;
            }
            else {
                // Set the item number
                lvi.iItem = dwListIndex;

                bSort = FALSE;
            }

            // Indicate only pszText is valid
            lvi.mask = LVIF_TEXT;

            // Set the text
            wsprintf(szText, L"%u", pTestObject->dwUDPBytes);
            lvi.pszText = szText;
            // Indicate this is the second column
            lvi.iSubItem = eColumnUDPBytes;
            // Set item
            ListView_SetItem(hWndList, &lvi);

            // Set the text
            wsprintf(szText, L"%u", pTestObject->dwTCPBytes);
            lvi.pszText = szText;
            // Indicate this is the third column
            lvi.iSubItem = eColumnTCPBytes;
            // Set item
            ListView_SetItem(hWndList, &lvi);

            // Get the date and time string
            GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &pTestObject->FirstActivityTime, NULL, szText, (sizeof(szText) / sizeof(WCHAR)) - 2);
            lstrcat(szText, TEXT(" "));
            dwTextLen = lstrlen(szText);
            GetTimeFormat(LOCALE_USER_DEFAULT, 0, &pTestObject->FirstActivityTime, NULL, &szText[dwTextLen], (sizeof(szText) / sizeof(WCHAR)) - dwTextLen - 1);

            // Set the text
            lvi.pszText = szText;
            // Indicate this is the fourth column
            lvi.iSubItem = eColumnFirstActivityTime;
            // Set item
            ListView_SetItem(hWndList, &lvi);

            // Get the date and time string
            GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &pTestObject->LastActivityTime, NULL, szText, sizeof(szText) / sizeof(WCHAR) - 2);
            lstrcat(szText, TEXT(" "));
            dwTextLen = lstrlen(szText);
            GetTimeFormat(LOCALE_USER_DEFAULT, 0, &pTestObject->LastActivityTime, NULL, &szText[dwTextLen], (sizeof(szText) / sizeof(WCHAR)) - dwTextLen - 1);

            // Set the text
            lvi.pszText = szText;
            // Indicate this is the fifth column
            lvi.iSubItem = eColumnLastActivityTime;
            // Set item
            ListView_SetItem(hWndList, &lvi);

            // Sort the status list
            if (TRUE == bSort) {
                ListView_SortItems(hWndList, SortTestObjects, eCurrentSortMethod);
            }

            break;

        case UM_DELETE_TEST:
            // Get the test object
            pTestObject = (PTEST_OBJECT) lParam;

            // Search the lParam
            lvfi.flags = LVFI_PARAM;

            // Set the search criteria
            lvfi.lParam = (long) pTestObject;

            // Find the item in the status list
            dwListIndex = ListView_FindItem(hWndList, -1, &lvfi);
            if (-1 != dwListIndex) {
                // Delete the item
                ListView_DeleteItem(hWndList, dwListIndex);
            }

            // Close the test object
            if (NULL != pTestObject->pPrevTestObject) {
                pTestObject->pPrevTestObject->pNextTestObject = pTestObject->pNextTestObject;
            }
            else {
                pSessionObject->pTestObject = pTestObject->pNextTestObject;
            }

            if (NULL != pTestObject->pNextTestObject) {
                pTestObject->pNextTestObject->pPrevTestObject = pTestObject->pPrevTestObject;
            }

            CloseTest(pTestObject);

            break;

        case WM_TIMER:
            EnterCriticalSection(pSessionObject->pcs);

            // Get the current time
            GetSystemTime(&SystemTime);
            SystemTimeToTzSpecificLocalTime(NULL, &SystemTime, &CurrentTime);
            SystemTimeToFileTime(&CurrentTime, (FILETIME *) &CurrentActivityTime);

            pTestObject = pSessionObject->pTestObject;
            while (NULL != pTestObject) {
                // Get the next test object
                pNextTestObject = pTestObject->pNextTestObject;

                if (SOCK_STREAM == pTestObject->type) {
                    // Convert the last activity time
                    SystemTimeToFileTime(&pTestObject->LastActivityTime, (FILETIME *) &LastActivityTime);

                    if (ACTIVITY_TIMEOUT < (CurrentActivityTime.QuadPart - LastActivityTime.QuadPart)) {
                        // Write the activity to the log file
                        LogActivity(pTestObject->hFile, &CurrentTime, pTestObject->RemoteName, LOG_TIMEOUT, 0);

                        SendMessage(hDlg, UM_DELETE_TEST, 0, (LPARAM) pTestObject);
                    }
                }

                // Get the next test object
                pTestObject = pNextTestObject;
            }

            LeaveCriticalSection(pSessionObject->pcs);

            break;

        case WM_NOTIFY:
            // Get the WM_NOTIFY message header
            pnmhdr = (LPNMHDR) lParam;

            if ((hWndList == pnmhdr->hwndFrom) && (LVN_COLUMNCLICK == pnmhdr->code)) {
                // Get the LVN_COLUMNCLICK message
                pnmlistview = (LPNMLISTVIEW) lParam;

                // Update the sort method
                switch (pnmlistview->iSubItem) {
                    case eColumnRemoteName:
                        if (eSortRemoteNameAscending != eCurrentSortMethod) {
                            eCurrentSortMethod = eSortRemoteNameAscending;
                        }
                        else {
                            eCurrentSortMethod = eSortRemoteNameDescending;
                        }

                        break;

                    case eColumnUDPBytes:
                        if (eSortUDPBytesAscending != eCurrentSortMethod) {
                            eCurrentSortMethod = eSortUDPBytesAscending;
                        }
                        else {
                            eCurrentSortMethod = eSortUDPBytesDescending;
                        }

                        break;

                    case eColumnTCPBytes:
                        if (eSortTCPBytesAscending != eCurrentSortMethod) {
                            eCurrentSortMethod = eSortTCPBytesAscending;
                        }
                        else {
                            eCurrentSortMethod = eSortTCPBytesDescending;
                        }

                        break;

                    case eColumnFirstActivityTime:
                        if (eSortFirstActivityTimeAscending != eCurrentSortMethod) {
                            eCurrentSortMethod = eSortFirstActivityTimeAscending;
                        }
                        else {
                            eCurrentSortMethod = eSortFirstActivityTimeDescending;
                        }

                        break;

                    case eColumnLastActivityTime:
                        if (eSortLastActivityTimeAscending != eCurrentSortMethod) {
                            eCurrentSortMethod = eSortLastActivityTimeAscending;
                        }
                        else {
                            eCurrentSortMethod = eSortLastActivityTimeDescending;
                        }

                        break;

                    default:
                        break;
                }

                // Sort the status list
                ListView_SortItems(hWndList, SortTestObjects, eCurrentSortMethod);
            }

            break;

        case WM_QUERYENDSESSION:
        case WM_ENDSESSION:
        case WM_CLOSE:
        case WM_DESTROY:
            // Destroy the timer
            if (TRUE == bTimerInitialized) {
                KillTimer(hDlg, 0);
                bTimerInitialized = FALSE;
            }

            // Close the session
            if (NULL != pSessionObject) {
                CloseSession(pSessionObject);
                pSessionObject = NULL;
            }

            // Terminate Winsock
            if (TRUE == bWinsockInitialized) {
                WSACleanup();
                bWinsockInitialized = FALSE;
            }

            // Close the log file
            if (INVALID_HANDLE_VALUE != hFile) {
                CloseHandle(hFile);
                hFile = INVALID_HANDLE_VALUE;
            }

            // Free the memory object
            if (INVALID_HANDLE_VALUE != hMemObject) {
                xMemClose(hMemObject);
                hMemObject = INVALID_HANDLE_VALUE;
            }

            // Delete the critical section
            if (TRUE == bcsInitialized) {
                DeleteCriticalSection(&cs);
                bcsInitialized = FALSE;
            }

            // End the dialog
            if (TRUE == bDlgInitialized) {
                bDlgInitialized = FALSE;
                DestroyWindow(hDlg);
                PostQuitMessage(ERROR_SUCCESS);
            }

            break;

    }

    return DefDlgProc(hDlg, iMsg, wParam, lParam);
}

} // namespace SGDataNamespace



int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Window procedure for the main window

-----------------------------------------------------------------------------*/
{
    // hDlg is the handle to the dialog
    HWND        hDlg = NULL;
    // wndclass is the window class of the dialog
    WNDCLASSEX  wndclass;
    // msg is the window message
    MSG         msg;
    // dwErrorCode is the error code
    DWORD       dwErrorCode = ERROR_SUCCESS;



    // Initialize the common controls
    InitCommonControls();

    // Initialize the window class
    ZeroMemory(&wndclass, sizeof(wndclass));
    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = DialogProc;
    wndclass.cbWndExtra = DLGWINDOWEXTRA;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SGDATA_ICON));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) (COLOR_INACTIVEBORDER + 1);
    wndclass.lpszClassName = SGDATA_CLASS_NAME;
    wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SGDATA_ICON));

    // Register the class
    RegisterClassEx(&wndclass);

    // Create the dialog
    hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, NULL);

    // Initialize the dialog
    SendMessage(hDlg, UM_DIALOG_INITIALIZE, 0, 0);

    // Show the dialog
    ShowWindow(hDlg, iCmdShow);
    UpdateWindow(hDlg);

    // Pump the message handler
    while (0 != GetMessage(&msg, NULL, 0, 0)) {
        if (FALSE == IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Get the error code
    dwErrorCode = msg.wParam;

    return dwErrorCode;
}
