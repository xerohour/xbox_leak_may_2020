/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  main.c

Abstract:

  This module tests XNet performance

Author:

  Steven Kehrli (steveke) 17-Nov-2001

Environment:

  XBox

Revision History:

------------------------------------------------------------------------------*/

#include "precomp.h"



#define PORT_SESSION              65475
#define PORT_UDP                  65476
#define PORT_TCP                  65476



typedef struct _SETUP_TABLE {
    LPSTR  lpszSecurityName;
    BYTE   cfgFlags;
    BOOL   bSecure;
    INT    nTCPOverhead;
    INT    nUDPOverhead;
} SETUP_TABLE, *PSETUP_TABLE;

static SETUP_TABLE SetupTable[] = {
    { "None", XNET_STARTUP_BYPASS_SECURITY,   FALSE, 66,  54 },
    { "Full", 0,                              TRUE,  104, 92 }
};

#define SetupTableCount (sizeof(SetupTable) / sizeof(SETUP_TABLE))



#define ITERATIONS_DEFAULT        10

#define BANDWIDTH_MIN             32
#define BANDWIDTH_DEFAULT         64
#define BANDWIDTH_MAX             51208



#define SESSION_ID_LEN            8

#define SESSION_ENUM_MSG          1
#define SESSION_ENUM_REPLY        2
#define SESSION_CONNECT_MSG       3
#define SESSION_CONNECT_REPLY     4
#define SESSION_READY_MSG         5
#define SESSION_READY_REPLY       6



typedef struct _SESSION_OBJECT {
    HANDLE                hMemObject;                  // Handle to the memory object
    XNADDR                LocalXnAddr;                 // Specifies the local xnet addr
    SOCKET                sSessionSocket;              // Specifies the session socket descriptor
    const static u_short  SessionPort = PORT_SESSION;  // Specifies the session port
    SOCKET                sUDPSocket;                  // Specifies the UDP socket descriptor
    const static u_short  UDPPort = PORT_UDP;          // Specifies the UDP port
    SOCKET                sTCPConnectSocket;           // Specifies the TCP socket descriptor for connect
    SOCKET                sTCPAcceptSocket;            // Specifies the TCP socket descriptor for accept
    const static u_short  TCPPort = PORT_TCP;          // Specifies the TCP port
    XNKID                 XnKid;                       // Specifies the xnet key id
    XNKEY                 XnKey;                       // Specifies the xnet key
    BOOL                  bXnKey;                      // Specifies if an xnet key id and key are registered
    BYTE                  SessionId[SESSION_ID_LEN];   // Specifies the session id
    u_long                RemoteAddr;                  // Specifies the remote address
    INT                   nBandwidth;                  // Specifies the bandwidth usage
    INT                   nTCPOverhead;                // Specifies the TCP overhead
    INT                   nUDPOverhead;                // Specifies the UDP overhead
} SESSION_OBJECT, *PSESSION_OBJECT;

typedef struct _SESSION_MESSAGE {
    DWORD                 dwMessageId;                 // Specifies the message id
    INT                   nSetup;                      // Specifies the current test setup
    BYTE                  SessionId[SESSION_ID_LEN];   // Specifies the session id
    XNADDR                FromXnAddr;                  // Specifies the xnet addr of the sender
    XNKID                 XnKid;                       // Specifies the xnet key id
    XNKEY                 XnKey;                       // Specifies the xnet key
    INT                   nBandwidth;                  // Specifies the bandwidth usage
} SESSION_MESSAGE, *PSESSION_MESSAGE;



#define SEND_TIME                 60
#define BUFFER_KBYTES_ACK_SIZE    63
#define BUFFER_BYTES_ACK_SIZE     1024 * BUFFER_KBYTES_ACK_SIZE



typedef struct _TEST_TABLE {
    LPSTR                 lpszTestName;                // Specifies the test name
    int                   type;                        // Specifies the type of socket
    int                   len;                         // Specifies the length of the buffer
} TEST_TABLE, *PTEST_TABLE;

static TEST_TABLE TestTable[] = {
    { "TCP 38 Bytes",   SOCK_STREAM, 38   },
    { "TCP 70 Bytes",   SOCK_STREAM, 70   },
    { "TCP 134 Bytes",  SOCK_STREAM, 134  },
    { "TCP 262 Bytes",  SOCK_STREAM, 262  },
    { "TCP 390 Bytes",  SOCK_STREAM, 390  },
    { "TCP 518 Bytes",  SOCK_STREAM, 518  },
    { "TCP 646 Bytes",  SOCK_STREAM, 646  },
    { "TCP 774 Bytes",  SOCK_STREAM, 774  },
    { "TCP 902 Bytes",  SOCK_STREAM, 902  },
    { "TCP 1030 Bytes", SOCK_STREAM, 1030 },
    { "UDP 38 Bytes",   SOCK_DGRAM,  38   },
    { "UDP 70 Bytes",   SOCK_DGRAM,  70   },
    { "UDP 134 Bytes",  SOCK_DGRAM,  134  },
    { "UDP 262 Bytes",  SOCK_DGRAM,  262  },
    { "UDP 390 Bytes",  SOCK_DGRAM,  390  },
    { "UDP 518 Bytes",  SOCK_DGRAM,  518  },
    { "UDP 646 Bytes",  SOCK_DGRAM,  646  },
    { "UDP 774 Bytes",  SOCK_DGRAM,  774  },
    { "UDP 902 Bytes",  SOCK_DGRAM,  902  },
    { "UDP 1030 Bytes", SOCK_DGRAM,  1030 },
};

#define TestTableCount (sizeof(TestTable) / sizeof(TEST_TABLE))



INT
CalculatePayloadTotal(
    IN INT  nBandwidth,
    IN INT  nPayload,
    IN INT  nOverhead
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Calculates the total payload

Arguments:

  nBandwidth - Specifies the bandwidth usage
  nPayload - Specifies the payload size
  nOverhead - Specifies the ethernet overhead size

Return Value:

  INT:
    If the function succeeds, the return value is the total payload
    If the function fails, the return value is 0.

------------------------------------------------------------------------------*/
{
    if (BANDWIDTH_MAX == nBandwidth) {
        nBandwidth = 102400;
    }

    return (((nBandwidth / 8) * 1024 * SEND_TIME) / (nPayload + nOverhead)) * nPayload;
}



INT
CalculateEthernetTotal(
    IN INT  nPayloadTotal,
    IN INT  nPayload,
    IN INT  nOverhead
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Calculates the total ethernet

Arguments:

  nPayloadTotal - Specifies the total payload
  nPayload - Specifies the payload size
  nOverhead - Specifies the ethernet overhead size

Return Value:

  INT:
    If the function succeeds, the return value is the total ethernet
    If the function fails, the return value is 0.

------------------------------------------------------------------------------*/
{
    return (nPayloadTotal / nPayload) * (nPayload + nOverhead);
}



DOUBLE
CalculateSendRate(
    IN INT  nBandwidth,
    IN INT  nPayload,
    IN INT  nOverhead
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Calculates the send rate

Arguments:

  nBandwidth - Specifies the bandwidth usage
  nPayload - Specifies the payload size
  nOverhead - Specifies the ethernet overhead size

Return Value:

  INT:
    If the function succeeds, the return value is the send rate
    If the function fails, the return value is 0.

------------------------------------------------------------------------------*/
{
    if (BANDWIDTH_MAX == nBandwidth) {
        return 0;
    }

    return (((DOUBLE) nBandwidth / 8) * 1024) / ((DOUBLE) nPayload + (DOUBLE) nOverhead);
}



SOCKET
CreateSocket(
    IN int     type,
    IN u_short sin_port,
    IN BOOL    bBroadcast
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Creates and binds a socket

Arguments:

  type - Specifies the type of socket (SOCK_DGRAM or SOCK_STREAM)
  sin_port - Specifies the bind port of socket
  bBroadcast - Specifies the broadcast mode of socket

Return Value:

  SOCKET:
    If the function succeeds, the return value is a socket descriptor
    If the function fails, the return value is INVALID_SOCKET.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // sSocket is the new socket
    SOCKET       sSocket = INVALID_SOCKET;
    // nTimeout is the timeout, in ms
    int          nTimeout = 15000;
    // localname is the local address
    SOCKADDR_IN  localname;
    // iErrorCode is the last error code
    int          iErrorCode = ERROR_SUCCESS;



    // Create the session socket
    sSocket = socket(AF_INET, type, 0);
    if (INVALID_SOCKET == sSocket) {
        // Get the last error code
        iErrorCode = WSAGetLastError();

        goto FunctionExit;
    }

    if ((TRUE == bBroadcast) && (SOCK_DGRAM == type)) {
        // Enable the control socket to send broadcast data
        if (SOCKET_ERROR == setsockopt(sSocket, SOL_SOCKET, SO_BROADCAST, (char *) &bBroadcast, sizeof(bBroadcast))) {
            // Get the last error code
            iErrorCode = WSAGetLastError();

            goto FunctionExit;
        }
    }

    // Set the timeout
    setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &nTimeout, sizeof(nTimeout));
    setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &nTimeout, sizeof(nTimeout));

    // Setup the local address
    ZeroMemory(&localname, sizeof(localname));
    localname.sin_family = AF_INET;
    localname.sin_port = htons(sin_port);

    // Bind the session socket
    if (SOCKET_ERROR == bind(sSocket, (SOCKADDR *) &localname, sizeof(localname))) {
        // Get the last error code
        iErrorCode = WSAGetLastError();

        goto FunctionExit;
    }

FunctionExit:
    if (ERROR_SUCCESS != iErrorCode) {
        if (INVALID_SOCKET != sSocket) {
            closesocket(sSocket);
            sSocket = INVALID_SOCKET;
        }

        SetLastError(iErrorCode);
    }

    return sSocket;
}



VOID
CloseSession(
    IN PSESSION_OBJECT  pSessionObject
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Closes a session

Arguments:

  pSessionObject - Pointer to the session object

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // hMemObject is a handle to the memory object
    HANDLE  hMemObject = pSessionObject->hMemObject;



    // Unregister the xnet key id and key
    if (TRUE == pSessionObject->bXnKey) {
        XNetUnregisterKey(&pSessionObject->XnKid);
    }

    // Close the TCP socket for accept
    if (INVALID_SOCKET != pSessionObject->sTCPAcceptSocket) {
        shutdown(pSessionObject->sTCPAcceptSocket, SD_BOTH);
        closesocket(pSessionObject->sTCPAcceptSocket);
    }

    // Close the TCP socket for connect
    if (INVALID_SOCKET != pSessionObject->sTCPConnectSocket) {
        shutdown(pSessionObject->sTCPConnectSocket, SD_BOTH);
        closesocket(pSessionObject->sTCPConnectSocket);
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

    // Free the session object
    xMemFree(hMemObject, pSessionObject);

    // Close the memory object
    xMemClose(hMemObject);
}



PSESSION_OBJECT
CreateSession(
    IN INT  nBandwidth,
    IN INT  nTCPOverhead,
    IN INT  nUDPOverhead
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Create a session

Arguments:

  nBandwidth - Specifies the bandwidth usage
  nTCPOverhead - Specifies the TCP overhead
  nUDPOverhead - Specifies the UDP overhead

Return Value:

  PSESSION_OBJECT:
    If the function succeeds, the return value is a pointer to the session object.
    If the function fails, the return value is NULL.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pSessionObject is a pointer to the session object
    PSESSION_OBJECT  pSessionObject = NULL;
    // hMemObject is a handle to the memory object
    HANDLE           hMemObject = INVALID_HANDLE_VALUE;
    // dwLastError is the last error code
    DWORD            dwLastError = ERROR_SUCCESS;



    // Create the memory object
    hMemObject = xMemCreate();
    if (INVALID_HANDLE_VALUE == hMemObject) {
        // Get the last error
        dwLastError = GetLastError();

        goto FunctionExit;
    }

    // Create the session object
    pSessionObject = (PSESSION_OBJECT) xMemAlloc(hMemObject, sizeof(SESSION_OBJECT));
    if (NULL == pSessionObject) {
        // Get the last error
        dwLastError = GetLastError();

        goto FunctionExit;
    }

    // Set the session object
    pSessionObject->hMemObject = hMemObject;
    pSessionObject->sSessionSocket = INVALID_SOCKET;
    pSessionObject->sUDPSocket = INVALID_SOCKET;
    pSessionObject->sTCPConnectSocket = INVALID_SOCKET;
    pSessionObject->sTCPAcceptSocket = INVALID_SOCKET;
    pSessionObject->RemoteAddr = INADDR_NONE;
    pSessionObject->nBandwidth = nBandwidth;
    pSessionObject->nTCPOverhead = nTCPOverhead;
    pSessionObject->nUDPOverhead = nUDPOverhead;

    // Get the local xnet addr
    while (0 == XNetGetTitleXnAddr(&pSessionObject->LocalXnAddr)) {
        Sleep(250);
    }

    // Create the session socket
    pSessionObject->sSessionSocket = CreateSocket(SOCK_DGRAM, pSessionObject->SessionPort, TRUE);
    if (INVALID_SOCKET == pSessionObject->sSessionSocket) {
        // Get the last error
        dwLastError = WSAGetLastError();

        goto FunctionExit;
    }

    // Create the UDP socket
    pSessionObject->sUDPSocket = CreateSocket(SOCK_DGRAM, pSessionObject->UDPPort, FALSE);
    if (INVALID_SOCKET == pSessionObject->sUDPSocket) {
        // Get the last error
        dwLastError = WSAGetLastError();

        goto FunctionExit;
    }

    // Create the TCP socket for connect
    pSessionObject->sTCPConnectSocket = CreateSocket(SOCK_STREAM, pSessionObject->TCPPort, FALSE);
    if (INVALID_SOCKET == pSessionObject->sTCPConnectSocket) {
        // Get the last error
        dwLastError = WSAGetLastError();

        goto FunctionExit;
    }

FunctionExit:
    if (ERROR_SUCCESS != dwLastError) {
        if (NULL != pSessionObject) {
            // Close the session object
            CloseSession(pSessionObject);
            pSessionObject = NULL;
        }

        // Set the last error code
        SetLastError(dwLastError);
    }

    return pSessionObject;
}



INT
HostSession(
    IN PSESSION_OBJECT  pSessionObject,
    IN INT              nSetup,
    IN BOOL             bSecure
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Hosts a session

Arguments:

  pSessionObject - Pointer to the session object
  nSetup - Specifies the current test setup
  bSecure - Specifies if session is secure

Return Value:

  INT:
    If the function succeeds, the return value is the bandwidth usage.

------------------------------------------------------------------------------*/
{
    // SessionMessageIn is a session message
    SESSION_MESSAGE  SessionMessageIn;
    // SessionMessageOut is a session message
    SESSION_MESSAGE  SessionMessageOut;
    // remotename is the remote address
    SOCKADDR_IN      remotename;
    // RemoteInAddr is the remote address
    IN_ADDR          RemoteInAddr;



    // Create and register the xnet key id and key
    XNetCreateKey(&pSessionObject->XnKid, &pSessionObject->XnKey);
    XNetRegisterKey(&pSessionObject->XnKid, &pSessionObject->XnKey);
    pSessionObject->bXnKey = TRUE;

    // Listen on TCP socket
    listen(pSessionObject->sTCPConnectSocket, SOMAXCONN);

    // Setup the remote address
    ZeroMemory(&remotename, sizeof(remotename));
    remotename.sin_family = AF_INET;
    remotename.sin_addr.s_addr = INADDR_BROADCAST;
    remotename.sin_port = htons(pSessionObject->SessionPort);

    do {
        // Receive enum message
        ZeroMemory(&SessionMessageIn, sizeof(SessionMessageIn));
        recvfrom(pSessionObject->sSessionSocket, (char *) &SessionMessageIn, sizeof(SessionMessageIn), 0, NULL, NULL);

        if ((SESSION_ENUM_MSG == SessionMessageIn.dwMessageId) && (nSetup == SessionMessageIn.nSetup)) {
            // Set the session message
            ZeroMemory(&SessionMessageOut, sizeof(SessionMessageOut));

            SessionMessageOut.dwMessageId = SESSION_ENUM_REPLY;
            SessionMessageOut.nSetup = nSetup;
            CopyMemory(SessionMessageOut.SessionId, SessionMessageIn.SessionId, sizeof(SessionMessageOut.SessionId));
            CopyMemory(&SessionMessageOut.FromXnAddr, &pSessionObject->LocalXnAddr, sizeof(SessionMessageOut.FromXnAddr));
            CopyMemory(&SessionMessageOut.XnKid, &pSessionObject->XnKid, sizeof(SessionMessageOut.XnKid));
            CopyMemory(&SessionMessageOut.XnKey, &pSessionObject->XnKey, sizeof(SessionMessageOut.XnKey));
            SessionMessageOut.nBandwidth = SessionMessageIn.nBandwidth;

            // Send the enum reply
            sendto(pSessionObject->sSessionSocket, (char *) &SessionMessageOut, sizeof(SessionMessageOut), 0, (SOCKADDR *) &remotename, sizeof(remotename));
        }
    } while (SESSION_CONNECT_MSG != SessionMessageIn.dwMessageId);

    // Resolve the remote name
    if (FALSE == bSecure) {
        RemoteInAddr = SessionMessageIn.FromXnAddr.ina;
    }
    else {
        XNetXnAddrToInAddr(&SessionMessageIn.FromXnAddr, &pSessionObject->XnKid, &RemoteInAddr);
    }

    // Setup the remote address
    ZeroMemory(&remotename, sizeof(remotename));
    remotename.sin_family = AF_INET;
    remotename.sin_addr = RemoteInAddr;
    remotename.sin_port = htons(pSessionObject->SessionPort);

    // Connect the session socket
    connect(pSessionObject->sSessionSocket, (SOCKADDR *) &remotename, sizeof(remotename));

    // Setup the remote address
    ZeroMemory(&remotename, sizeof(remotename));
    remotename.sin_family = AF_INET;
    remotename.sin_addr = RemoteInAddr;
    remotename.sin_port = htons(pSessionObject->UDPPort);

    // Connect the UDP connection
    connect(pSessionObject->sUDPSocket, (SOCKADDR *) &remotename, sizeof(remotename));

    // Accept the TCP connection
    pSessionObject->sTCPAcceptSocket = accept(pSessionObject->sTCPConnectSocket, NULL, NULL);

    // Set the bandwidth usage
    pSessionObject->nBandwidth = SessionMessageIn.nBandwidth;

    // Set the session message
    ZeroMemory(&SessionMessageOut, sizeof(SessionMessageOut));

    SessionMessageOut.dwMessageId = SESSION_CONNECT_REPLY;
    SessionMessageOut.nSetup = nSetup;
    CopyMemory(SessionMessageOut.SessionId, SessionMessageIn.SessionId, sizeof(SessionMessageOut.SessionId));
    CopyMemory(&SessionMessageOut.FromXnAddr, &pSessionObject->LocalXnAddr, sizeof(SessionMessageOut.FromXnAddr));

    // Send the connect reply
    send(pSessionObject->sSessionSocket, (char *) &SessionMessageOut, sizeof(SessionMessageOut), 0);

    // Return the remote addr
    pSessionObject->RemoteAddr = RemoteInAddr.s_addr;

    return pSessionObject->nBandwidth;
}



VOID
JoinSession(
    IN PSESSION_OBJECT  pSessionObject,
    IN INT              nSetup,
    IN BOOL             bSecure
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Joins a session

Arguments:

  pSessionObject - Pointer to the session object
  nSetup - Specifies the current test setup
  bSecure - Specifies if session is secure

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // SessionMessageIn is a session message
    SESSION_MESSAGE  SessionMessageIn;
    // SessionMessageOut is a session message
    SESSION_MESSAGE  SessionMessageOut;
    // remotename is the remote address
    SOCKADDR_IN      remotename;
    // RemoteInAddr is the remote address
    IN_ADDR          RemoteInAddr;

    // readfds is the read socket set
    fd_set           readfds;
    // timeout it the select timeout
    timeval          timeout = { 1, 0 };
    // bContinue specifies to continue join
    BOOL             bContinue = FALSE;



    // Generate the session id
    XNetRandom(pSessionObject->SessionId, sizeof(pSessionObject->SessionId));

    // Set the session message
    ZeroMemory(&SessionMessageOut, sizeof(SessionMessageOut));

    SessionMessageOut.dwMessageId = SESSION_ENUM_MSG;
    SessionMessageOut.nSetup = nSetup;
    CopyMemory(SessionMessageOut.SessionId, pSessionObject->SessionId, sizeof(SessionMessageOut.SessionId));
    CopyMemory(&SessionMessageOut.FromXnAddr, &pSessionObject->LocalXnAddr, sizeof(SessionMessageOut.FromXnAddr));
    SessionMessageOut.nBandwidth = pSessionObject->nBandwidth;

    // Setup the remote address
    ZeroMemory(&remotename, sizeof(remotename));
    remotename.sin_family = AF_INET;
    remotename.sin_addr.s_addr = INADDR_BROADCAST;
    remotename.sin_port = htons(pSessionObject->SessionPort);

    do {
        // Send the enum message
        sendto(pSessionObject->sSessionSocket, (char *) &SessionMessageOut, sizeof(SessionMessageOut), 0, (SOCKADDR *) &remotename, sizeof(remotename));

        FD_ZERO(&readfds);
        FD_SET(pSessionObject->sSessionSocket, &readfds);

        while (1 == select(0, &readfds, NULL, NULL, &timeout)) {
            // Receive enum reply
            ZeroMemory(&SessionMessageIn, sizeof(SessionMessageIn));
            recvfrom(pSessionObject->sSessionSocket, (char *) &SessionMessageIn, sizeof(SessionMessageIn), 0, NULL, NULL);

            if ((0 == memcmp(SessionMessageOut.SessionId, SessionMessageIn.SessionId, sizeof(SessionMessageOut.SessionId))) && (SESSION_ENUM_REPLY == SessionMessageIn.dwMessageId) && (nSetup == SessionMessageIn.nSetup)) {
                bContinue = TRUE;
                break;
            }

            FD_ZERO(&readfds);
            FD_SET(pSessionObject->sSessionSocket, &readfds);
        }
    } while (FALSE == bContinue);

    // Copy the key id and key
    CopyMemory(&pSessionObject->XnKid, &SessionMessageIn.XnKid, sizeof(pSessionObject->XnKid));
    CopyMemory(&pSessionObject->XnKey, &SessionMessageIn.XnKey, sizeof(pSessionObject->XnKey));

    // Register the key
    XNetRegisterKey(&pSessionObject->XnKid, &pSessionObject->XnKey);
    pSessionObject->bXnKey = TRUE;

    // Resolve the remote name
    if (FALSE == bSecure) {
        RemoteInAddr = SessionMessageIn.FromXnAddr.ina;
    }
    else {
        XNetXnAddrToInAddr(&SessionMessageIn.FromXnAddr, &pSessionObject->XnKid, &RemoteInAddr);
    }

    // Setup the remote address
    ZeroMemory(&remotename, sizeof(remotename));
    remotename.sin_family = AF_INET;
    remotename.sin_addr = RemoteInAddr;
    remotename.sin_port = htons(pSessionObject->SessionPort);

    // Connect the session socket
    connect(pSessionObject->sSessionSocket, (SOCKADDR *) &remotename, sizeof(remotename));

    // Setup the remote address
    ZeroMemory(&remotename, sizeof(remotename));
    remotename.sin_family = AF_INET;
    remotename.sin_addr = RemoteInAddr;
    remotename.sin_port = htons(pSessionObject->UDPPort);

    // Connect the UDP connection
    connect(pSessionObject->sUDPSocket, (SOCKADDR *) &remotename, sizeof(remotename));

    // Setup the remote address
    ZeroMemory(&remotename, sizeof(remotename));
    remotename.sin_family = AF_INET;
    remotename.sin_addr = RemoteInAddr;
    remotename.sin_port = htons(pSessionObject->TCPPort);

    // Connect the TCP connection
    connect(pSessionObject->sTCPConnectSocket, (SOCKADDR *) &remotename, sizeof(remotename));

    // Set the session message
    ZeroMemory(&SessionMessageOut, sizeof(SessionMessageOut));

    SessionMessageOut.dwMessageId = SESSION_CONNECT_MSG;
    SessionMessageOut.nSetup = nSetup;
    CopyMemory(SessionMessageOut.SessionId, pSessionObject->SessionId, sizeof(SessionMessageOut.SessionId));
    CopyMemory(&SessionMessageOut.FromXnAddr, &pSessionObject->LocalXnAddr, sizeof(SessionMessageOut.FromXnAddr));
    SessionMessageOut.nBandwidth = pSessionObject->nBandwidth;

    // Send the connect message
    send(pSessionObject->sSessionSocket, (char *) &SessionMessageOut, sizeof(SessionMessageOut), 0);

    // Wait for the connect reply
    ZeroMemory(&SessionMessageIn, sizeof(SessionMessageIn));
    recv(pSessionObject->sSessionSocket, (char *) &SessionMessageIn, sizeof(SessionMessageIn), 0);

    // Return the remote addr
    pSessionObject->RemoteAddr = RemoteInAddr.s_addr;
}



VOID
SendSession(
    IN SOCKET  sSocket,
    IN char    *Buffer,
    IN int     len,
    IN int     totallen,
    IN DOUBLE  SleepTime
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Runs a send session

Arguments:

  sSocket - Specifies the socket descriptor
  Buffer - Pointer to the buffer
  len - Specifies the length of the buffer
  totallen - Specifies the total length of the buffer
  SleepTime - Specifies the sleep time

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // nIndex is an integer index into the buffer
    int     *nIndex = (int *) Buffer;
    // nTotal is a counter to enumerate the total buffer
    int     nTotal = 0;
    // nAck is a counter to enumerate the ack
    int     nAck = 0;
    // SleepUntilTime is the sleep until time
    DOUBLE  SleepUntilTime;



    if (0 != SleepTime) {
        // Get the initial time
        SleepUntilTime = (DOUBLE) GetTickCount();
        SleepUntilTime += SleepTime;
    }

    for (nTotal = 0; nTotal < totallen; nTotal += nAck) {
        for (nAck = 0; (nAck < BUFFER_BYTES_ACK_SIZE) && ((nTotal + nAck) < totallen); nAck += len) {
            nIndex[0] = nTotal;
            nIndex[1] = nAck;

            // Send
            if (SOCKET_ERROR == send(sSocket, Buffer, len, 0)) {
                DbgPrint("send [%u][%u] failed\n  sSocket = 0x%08x\n  ec = %u\n", nTotal, nAck, sSocket, WSAGetLastError());
                __asm int 3;
            }

            if (0 != SleepTime) {
                if (SleepUntilTime > GetTickCount()) {
                    Sleep((DWORD) SleepUntilTime - GetTickCount());
                }
                SleepUntilTime += SleepTime;
            }
        }

        // Recv
        if (SOCKET_ERROR == recv(sSocket, Buffer, len, 0)) {
            DbgPrint("recv [%u][%u] failed\n  sSocket = 0x%08x\n  ec = %u\n", nTotal, nAck, sSocket, WSAGetLastError());
            __asm int 3;
        }
    }
}



VOID
RecvSession(
    IN SOCKET  sSocket,
    IN char    *Buffer,
    IN int     len,
    IN int     totallen
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Runs a recv session

Arguments:

  sSocket - Specifies the socket descriptor
  Buffer - Pointer to the buffer
  len - Specifies the length of the buffer
  totallen - Specifies the total length of the buffer

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // nTotal is a counter to enumerate the total buffer
    int  nTotal = 0;
    // nAck is a counter to enumerate the ack
    int  nAck = 0;



    for (nTotal = 0; nTotal < totallen; nTotal += nAck) {
        for (nAck = 0; (nAck < BUFFER_BYTES_ACK_SIZE) && ((nTotal + nAck) < totallen); nAck += len) {
            // Recv
            if (SOCKET_ERROR == recv(sSocket, Buffer, len, 0)) {
                DbgPrint("recv [%u][%u] failed\n  sSocket = 0x%08x\n  ec = %u\n", nTotal, nAck, sSocket, WSAGetLastError());
                __asm int 3;
            }
        }

        // Send
        if (SOCKET_ERROR == send(sSocket, Buffer, len, 0)) {
            DbgPrint("send [%u][%u] failed\n  sSocket = 0x%08x\n  ec = %u\n", nTotal, nAck, sSocket, WSAGetLastError());
            __asm int 3;
        }
    }
}



VOID
TestSession(
    IN PSESSION_OBJECT  pSessionObject,
    IN BOOL             bSend,
    IN HANDLE           hLogAll,
    IN HANDLE           hLogSummary,
    IN HANDLE           hLogCsv
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Runs a test session

Arguments:

  pSessionObject - Pointer to the session object
  bSend - Specifies send or receive
  hLogAll - Handle to the log file
  hLogSummary - Handle to the summary log file
  hLogCsv - Handle to the csv log file

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // dwTestIndex is a counter to enumerate each test
    DWORD            dwTestIndex;
    // nIteration is a counter to enumerate each iteration
    INT              nIteration;

    // nPayloadTotal is the total payload
    INT              nPayloadTotal;
    // nEthernetTotal is the total ethernet
    INT              nEthernetTotal;
    // SendRate is the send rate
    DOUBLE           SendRate;

    // NetworkBuffer is the network buffer
    char             NetworkBuffer[1024];

    // SessionMessageIn is a session message
    SESSION_MESSAGE  SessionMessageIn;
    // SessionMessageOut is a session message
    SESSION_MESSAGE  SessionMessageOut;

    // KernelTimeInitial is the initial kernel time
    ULONG            KernelTimeInitial;
    // KernelTimeFinal is the final kernel time
    ULONG            KernelTimeFinal;
    // KernelTimeTotal is the final kernel time
    ULONG            KernelTimeTotal;
    // InterruptTimeInitial is the initial interrupt time
    ULONG            InterruptTimeInitial;
    // InterruptTimeFinal is the final interrupt time
    ULONG            InterruptTimeFinal;
    // InterruptTimeTotal is the total interrupt time
    ULONG            InterruptTimeTotal;
    // DpcTimeInitial is the initial dpc time
    ULONG            DpcTimeInitial;
    // DpcTimeFinal is the final dpc time
    ULONG            DpcTimeFinal;
    // DpcTimeTotal is the total dpc time
    ULONG            DpcTimeTotal;
    // IdleTimeInitial is the initial idle time
    ULONG            IdleTimeInitial;
    // IdleTimeFinal is the final idle time
    ULONG            IdleTimeFinal;
    // IdleTimeTotal is the total idle time
    ULONG            IdleTimeTotal;



    // Initialize the buffer
    XNetRandom((BYTE *) NetworkBuffer, sizeof(NetworkBuffer));

    for (dwTestIndex = 0; dwTestIndex < TestTableCount; dwTestIndex++) {
        // Initialize the time
        ZeroMemory(&KernelTimeTotal, sizeof(KernelTimeTotal));
        ZeroMemory(&InterruptTimeTotal, sizeof(InterruptTimeTotal));
        ZeroMemory(&DpcTimeTotal, sizeof(DpcTimeTotal));
        ZeroMemory(&IdleTimeTotal, sizeof(IdleTimeTotal));

        for (nIteration = 0; nIteration < ITERATIONS_DEFAULT; nIteration++) {
            // Initialize the CPU times
            ZeroMemory(&KernelTimeInitial, sizeof(KernelTimeInitial));
            ZeroMemory(&KernelTimeFinal, sizeof(KernelTimeFinal));
            ZeroMemory(&InterruptTimeInitial, sizeof(InterruptTimeInitial));
            ZeroMemory(&InterruptTimeFinal, sizeof(InterruptTimeFinal));
            ZeroMemory(&DpcTimeInitial, sizeof(DpcTimeInitial));
            ZeroMemory(&DpcTimeFinal, sizeof(DpcTimeFinal));
            ZeroMemory(&IdleTimeInitial, sizeof(IdleTimeInitial));
            ZeroMemory(&IdleTimeFinal, sizeof(IdleTimeFinal));

            // Calculate the send info
            if (SOCK_STREAM == TestTable[dwTestIndex].type) {
                nPayloadTotal = CalculatePayloadTotal(pSessionObject->nBandwidth, TestTable[dwTestIndex].len, pSessionObject->nTCPOverhead);
                nEthernetTotal = CalculateEthernetTotal(nPayloadTotal, TestTable[dwTestIndex].len, pSessionObject->nTCPOverhead);
                SendRate = CalculateSendRate(pSessionObject->nBandwidth, TestTable[dwTestIndex].len, pSessionObject->nTCPOverhead);
            }
            else {
                nPayloadTotal = CalculatePayloadTotal(pSessionObject->nBandwidth, TestTable[dwTestIndex].len, pSessionObject->nUDPOverhead);
                nEthernetTotal = CalculateEthernetTotal(nPayloadTotal, TestTable[dwTestIndex].len, pSessionObject->nUDPOverhead);
                SendRate = CalculateSendRate(pSessionObject->nBandwidth, TestTable[dwTestIndex].len, pSessionObject->nUDPOverhead);
            }

            // Set the session message
            ZeroMemory(&SessionMessageIn, sizeof(SessionMessageIn));
            ZeroMemory(&SessionMessageOut, sizeof(SessionMessageOut));

            if (TRUE == bSend) {
                // Send the ready message
                SessionMessageOut.dwMessageId = SESSION_READY_MSG;
                send(pSessionObject->sSessionSocket, (char *) &SessionMessageOut, sizeof(SessionMessageOut), 0);

                // Receive the ready reply
                recv(pSessionObject->sSessionSocket, (char *) &SessionMessageIn, sizeof(SessionMessageIn), 0);
            }
            else {
                // Receive the ready message
                recv(pSessionObject->sSessionSocket, (char *) &SessionMessageIn, sizeof(SessionMessageIn), 0);

                // Send the ready reply
                SessionMessageOut.dwMessageId = SESSION_READY_REPLY;
                send(pSessionObject->sSessionSocket, (char *) &SessionMessageOut, sizeof(SessionMessageOut), 0);
            }

            // Query the CPU times
            GetCpuTimes(&KernelTimeInitial, &InterruptTimeInitial, &DpcTimeInitial, &IdleTimeInitial);

            // Run the test
            if (SOCK_STREAM == TestTable[dwTestIndex].type) {
                if (TRUE == bSend) {
                    SendSession((INVALID_SOCKET != pSessionObject->sTCPAcceptSocket) ? pSessionObject->sTCPAcceptSocket : pSessionObject->sTCPConnectSocket, NetworkBuffer, TestTable[dwTestIndex].len, nPayloadTotal, (0 != SendRate) ? (1000 / SendRate) : 0);
                }
                else {
                    RecvSession((INVALID_SOCKET != pSessionObject->sTCPAcceptSocket) ? pSessionObject->sTCPAcceptSocket : pSessionObject->sTCPConnectSocket, NetworkBuffer, TestTable[dwTestIndex].len, nPayloadTotal);
                }
            }
            else {
                if (TRUE == bSend) {
                    SendSession(pSessionObject->sUDPSocket, NetworkBuffer, TestTable[dwTestIndex].len, nPayloadTotal, (0 != SendRate) ? (1000 / SendRate) : 0);
                }
                else {
                    RecvSession(pSessionObject->sUDPSocket, NetworkBuffer, TestTable[dwTestIndex].len, nPayloadTotal);
                }
            }

            // Query the CPU times
            GetCpuTimes(&KernelTimeFinal, &InterruptTimeFinal, &DpcTimeFinal, &IdleTimeFinal);

            // Log the stats
            LogStats(hLogAll, TestTable[dwTestIndex].lpszTestName, nIteration, TestTable[dwTestIndex].len, nPayloadTotal, (SOCK_STREAM == TestTable[dwTestIndex].type) ? pSessionObject->nTCPOverhead : pSessionObject->nUDPOverhead, nEthernetTotal, KernelTimeInitial, KernelTimeFinal, InterruptTimeInitial, InterruptTimeFinal, DpcTimeInitial, DpcTimeFinal, IdleTimeInitial, IdleTimeFinal);
            CalculateAndAddStatsDiff(KernelTimeInitial, KernelTimeFinal, InterruptTimeInitial, InterruptTimeFinal, DpcTimeInitial, DpcTimeFinal, IdleTimeInitial, IdleTimeFinal, &KernelTimeTotal, &InterruptTimeTotal, &DpcTimeTotal, &IdleTimeTotal);
        }

        // Log the stats
        LogStatsSummary(hLogSummary, TestTable[dwTestIndex].lpszTestName, TestTable[dwTestIndex].len, nPayloadTotal, (SOCK_STREAM == TestTable[dwTestIndex].type) ? pSessionObject->nTCPOverhead : pSessionObject->nUDPOverhead, nEthernetTotal, KernelTimeTotal, InterruptTimeTotal, DpcTimeTotal, IdleTimeTotal, ITERATIONS_DEFAULT);
        LogStatsCsv(hLogCsv, (SOCK_STREAM == TestTable[dwTestIndex].type) ? "TCP" : "UDP", TestTable[dwTestIndex].len, nPayloadTotal, (SOCK_STREAM == TestTable[dwTestIndex].type) ? pSessionObject->nTCPOverhead : pSessionObject->nUDPOverhead, nEthernetTotal, KernelTimeTotal, InterruptTimeTotal, DpcTimeTotal, IdleTimeTotal, ITERATIONS_DEFAULT);
    }
}



void __cdecl main()
{
    // dwLinkStatus is the Ethernet link status
    DWORD              dwLinkStatus = 0;

    // hDevices is an array of handles to the devices
    HANDLE             hDevices[4];
    // dwCurrentDevice is a counter to enumerate each device
    DWORD              dwCurrentDevice = 0;
    // dwInsertions is a bit mask of device insertions
    DWORD              dwInsertions;
    // dwRemovals is a bit mask of device removals
    DWORD              dwRemovals;
    // XInputState is the current device state
    XINPUT_STATE       XInputState;
    // dwUpButton specifies if up is pressed
    DWORD              dwUpButtonFirst[4] = { 0, 0, 0, 0 };
    DWORD              dwUpButtonLast[4] = { 0, 0, 0, 0 };
    // dwDownButton specifies if down is pressed
    DWORD              dwDownButtonFirst[4] = { 0, 0, 0, 0 };
    DWORD              dwDownButtonLast[4] = { 0, 0, 0, 0 };
    // bStartButton specifies if start is pressed
    BOOL               bStartButton[4] = { FALSE, FALSE, FALSE, FALSE };

    // bClient specifies the client session
    BOOL               bClient = FALSE;
    // nBandwidth specifies the bandwidth usage
    INT                nBandwidth = BANDWIDTH_DEFAULT;
    // nIncreaseIncrement specifies the increase increment
    INT                nIncreaseIncrement = 0;
    // nDecreaseIncrement specifies the decrease increment
    INT                nDecreaseIncrement = 0;
    // bContinue specifies if continue
    BOOL               bContinue = FALSE;

    // szLogFileName is the name of the log file
    CHAR               szLogFileName[MAX_PATH];
    // hLogAll is a handle to the log file
    HANDLE             hLogAll = INVALID_HANDLE_VALUE;
    // hLogSummary is a handle to the summary log file
    HANDLE             hLogSummary = INVALID_HANDLE_VALUE;
    // hLogCsv is a handle to the csv log file
    HANDLE             hLogCsv = INVALID_HANDLE_VALUE;

    // nSetup is a counter to enumerate each test setup
    INT                nSetup = 0;

    // XNetParams is the xnet startup params
    XNetStartupParams  XNetParams;
    // WSAData is the details of the Winsock implementation
    WSADATA            WSAData;

    // pSessionObject is the pointer to session object
    PSESSION_OBJECT    pSessionObject = NULL;



    // Get the Ethernet link status
    dwLinkStatus = XNetGetEthernetLinkStatus();
    DbgPrint("Ethernet Link Status:\n");

    if (0 != (XNET_ETHERNET_LINK_ACTIVE & dwLinkStatus)) {
        DbgPrint("  Connected\n");
    }
    else {
        DbgPrint("  Disconnected\n");
    }

    if (0 != (XNET_ETHERNET_LINK_100MBPS & dwLinkStatus)) {
        DbgPrint("  Speed: 100Mbps\n");
    }
    else if (0 != (XNET_ETHERNET_LINK_10MBPS & dwLinkStatus)) {
        DbgPrint("  Speed: 10Mbps\n");
    }
    else {
        DbgPrint("  Speed: Unknown\n");
    }

    if (0 != (XNET_ETHERNET_LINK_FULL_DUPLEX & dwLinkStatus)) {
        DbgPrint("  Mode: Full Duplex\n");
    }
    else if (0 != (XNET_ETHERNET_LINK_HALF_DUPLEX & dwLinkStatus)) {
        DbgPrint("  Mode: Half Duplex\n");
    }
    else {
        DbgPrint("  Mode: Unknown\n");
    }

    // Initialize the USB stack
    XInitDevices(0, NULL);

    // Initialize the array of device handles
    ZeroMemory(hDevices, sizeof(HANDLE) * XGetPortCount());

    bContinue = FALSE;
    DbgPrint("Press 'Up' or 'Down' to change session.\n");
    DbgPrint("  Session: %s\n", (TRUE == bClient) ? "Client" : "Server");

    do {
        // Get for hotplug insertions or removals
        if (FALSE == XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals)) {
            dwInsertions = 0;
            dwRemovals = 0;
        }

        for (dwCurrentDevice = 0; dwCurrentDevice < XGetPortCount(); dwCurrentDevice++) {
            // Close the device if it has been removed
            if ((1 << dwCurrentDevice) & dwRemovals) {
                XInputClose(hDevices[dwCurrentDevice]);
                hDevices[dwCurrentDevice] = NULL;
            }

            // Open the device if it has been inserted
            if ((1 << dwCurrentDevice) & dwInsertions) {
                hDevices[dwCurrentDevice] = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0 + dwCurrentDevice, XDEVICE_NO_SLOT, NULL);
            }

            // Get the current device state
            if (NULL != hDevices[dwCurrentDevice]) {
                XInputGetState(hDevices[dwCurrentDevice], &XInputState);

                if ((XINPUT_GAMEPAD_DPAD_UP & XInputState.Gamepad.wButtons) || ((SHRT_MAX / 5) < XInputState.Gamepad.sThumbLY)) {
                    if (0 == dwUpButtonFirst[dwCurrentDevice]) {
                        if (TRUE == bClient) {
                            bClient = FALSE;

                            DbgPrint("  Session: Server\n");
                        }
                    }

                    dwUpButtonFirst[dwCurrentDevice] = GetTickCount();
                }
                else {
                    dwUpButtonFirst[dwCurrentDevice] = 0;
                    dwUpButtonLast[dwCurrentDevice] = 0;
                }

                if ((XINPUT_GAMEPAD_DPAD_DOWN & XInputState.Gamepad.wButtons) || ((SHRT_MIN / 5) > XInputState.Gamepad.sThumbLY)) {
                    if (0 == dwDownButtonFirst[dwCurrentDevice]) {
                        if (FALSE == bClient) {
                            bClient = TRUE;

                            DbgPrint("  Session: Client\n");
                        }
                    }

                    dwDownButtonFirst[dwCurrentDevice] = GetTickCount();
                }
                else {
                    dwDownButtonFirst[dwCurrentDevice] = 0;
                    dwDownButtonLast[dwCurrentDevice] = 0;
                }

                if ((XINPUT_GAMEPAD_START & XInputState.Gamepad.wButtons) || (XINPUT_GAMEPAD_MAX_CROSSTALK < XInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A])) {
                    if (FALSE == bStartButton[dwCurrentDevice]) {
                        bContinue = TRUE;
                    }

                    bStartButton[dwCurrentDevice] = TRUE;
                }
                else {
                    bStartButton[dwCurrentDevice] = FALSE;
                }
            }
        }
    } while (FALSE == bContinue);

    if (TRUE == bClient) {
        bContinue = FALSE;
        nIncreaseIncrement = 0;
        nDecreaseIncrement = 0;
        DbgPrint("Press 'Up' to increase bandwidth usage.\n");
        DbgPrint("Press 'Down' to decrease bandwidth usage.\n");
        DbgPrint("  Bandwidth Usage: %d kbps\n", nBandwidth);

        do {
            // Get for hotplug insertions or removals
            if (FALSE == XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals)) {
                dwInsertions = 0;
                dwRemovals = 0;
            }

            for (dwCurrentDevice = 0; dwCurrentDevice < XGetPortCount(); dwCurrentDevice++) {
                // Close the device if it has been removed
                if ((1 << dwCurrentDevice) & dwRemovals) {
                    XInputClose(hDevices[dwCurrentDevice]);
                    hDevices[dwCurrentDevice] = NULL;
                }

                // Open the device if it has been inserted
                if ((1 << dwCurrentDevice) & dwInsertions) {
                    hDevices[dwCurrentDevice] = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0 + dwCurrentDevice, XDEVICE_NO_SLOT, NULL);
                }

                // Get the current device state
                if (NULL != hDevices[dwCurrentDevice]) {
                    XInputGetState(hDevices[dwCurrentDevice], &XInputState);

                    if ((XINPUT_GAMEPAD_DPAD_UP & XInputState.Gamepad.wButtons) || ((SHRT_MAX / 5) < XInputState.Gamepad.sThumbLY)) {
                        if (0 == dwUpButtonFirst[dwCurrentDevice]) {
                            dwUpButtonFirst[dwCurrentDevice] = GetTickCount();
                        }

                        if (0 != nIncreaseIncrement) {
                            if (250 < (GetTickCount() - dwUpButtonLast[dwCurrentDevice])) {
                                if (BANDWIDTH_MAX > nBandwidth) {
                                    nBandwidth += nIncreaseIncrement;

                                    if (BANDWIDTH_MAX < nBandwidth) {
                                        nBandwidth = BANDWIDTH_MAX;
                                    }

                                    if (BANDWIDTH_MAX == nBandwidth) {
                                        DbgPrint("  Bandwidth Usage: Max\n");
                                    }
                                    else {
                                        DbgPrint("  Bandwidth Usage: %d kbps\n", nBandwidth);
                                    }
                                }

                                dwUpButtonLast[dwCurrentDevice] = GetTickCount();
                            }

                            if (6000 < (GetTickCount() - dwUpButtonFirst[dwCurrentDevice])) {
                                nIncreaseIncrement = 512;
                            }
                            else if (4000 < (GetTickCount() - dwUpButtonFirst[dwCurrentDevice])) {
                                nIncreaseIncrement = 128;
                            }
                            else if (2000 < (GetTickCount() - dwUpButtonFirst[dwCurrentDevice])) {
                                nIncreaseIncrement = 32;
                            }
                        }
                    }
                    else {
                        dwUpButtonFirst[dwCurrentDevice] = 0;
                        dwUpButtonLast[dwCurrentDevice] = 0;
                        nIncreaseIncrement = 8;
                    }

                    if ((XINPUT_GAMEPAD_DPAD_DOWN & XInputState.Gamepad.wButtons) || ((SHRT_MIN / 5) > XInputState.Gamepad.sThumbLY)) {
                        if (0 == dwDownButtonFirst[dwCurrentDevice]) {
                            dwDownButtonFirst[dwCurrentDevice] = GetTickCount();
                        }

                        if (0 != nDecreaseIncrement) {
                            if (250 < (GetTickCount() - dwDownButtonLast[dwCurrentDevice])) {
                                if (BANDWIDTH_MIN < nBandwidth) {
                                    nBandwidth -= nDecreaseIncrement;

                                    if (BANDWIDTH_MIN > nBandwidth) {
                                        nBandwidth = BANDWIDTH_MIN;
                                    }

                                    if (BANDWIDTH_MAX == nBandwidth) {
                                        DbgPrint("  Bandwidth Usage: Max\n");
                                    }
                                    else {
                                        DbgPrint("  Bandwidth Usage: %d kbps\n", nBandwidth);
                                    }
                                }

                                dwDownButtonLast[dwCurrentDevice] = GetTickCount();
                            }

                            if (6000 < (GetTickCount() - dwDownButtonFirst[dwCurrentDevice])) {
                                nDecreaseIncrement = 512;
                            }
                            else if (4000 < (GetTickCount() - dwDownButtonFirst[dwCurrentDevice])) {
                                nDecreaseIncrement = 128;
                            }
                            else if (2000 < (GetTickCount() - dwDownButtonFirst[dwCurrentDevice])) {
                                nDecreaseIncrement = 32;
                            }
                        }
                    }
                    else {
                        dwDownButtonFirst[dwCurrentDevice] = 0;
                        dwDownButtonLast[dwCurrentDevice] = 0;
                        nDecreaseIncrement = 8;
                    }

                    if ((XINPUT_GAMEPAD_START & XInputState.Gamepad.wButtons) || (XINPUT_GAMEPAD_MAX_CROSSTALK < XInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A])) {
                        if (FALSE == bStartButton[dwCurrentDevice]) {
                            bContinue = TRUE;
                        }

                        bStartButton[dwCurrentDevice] = TRUE;
                    }
                    else {
                        bStartButton[dwCurrentDevice] = FALSE;
                    }
                }
            }
        } while (FALSE == bContinue);
    }

    if (TRUE == bClient) {
        DbgPrint("\n");
        if (BANDWIDTH_MAX == nBandwidth) {
            DbgPrint("Client Started - Bandwidth Usage: Max\n\n");
        }
        else {
            DbgPrint("Client Started - Bandwidth Usage: %d kbps\n\n", nBandwidth);
        }
    }
    else {
        DbgPrint("\n");
        DbgPrint("Server Started\n\n");
    }

    // Close any open devices
    for (dwCurrentDevice = 0; dwCurrentDevice < XGetPortCount(); dwCurrentDevice++) {
        if (NULL != hDevices[dwCurrentDevice]) {
            XInputClose(hDevices[dwCurrentDevice]);
        }
    }

    for (nSetup = 0; nSetup < SetupTableCount; nSetup++) {
        // Initialize XNet and Winsock
        ZeroMemory(&XNetParams, sizeof(XNetParams));
        XNetParams.cfgSizeOfStruct = sizeof(XNetParams);
        XNetParams.cfgFlags = SetupTable[nSetup].cfgFlags;
        XNetParams.cfgPrivatePoolSizeInPages = 255;
        XNetParams.cfgEnetReceiveQueueLength = 255;
        XNetParams.cfgSockDefaultRecvBufsizeInK = BUFFER_KBYTES_ACK_SIZE;

        XNetStartup(&XNetParams);
    
        WSAStartup(MAKEWORD(2, 2), &WSAData);



        // Create the session
        pSessionObject = CreateSession((TRUE == bClient) ? nBandwidth : 0, SetupTable[nSetup].nTCPOverhead, SetupTable[nSetup].nUDPOverhead);

        // Join or host a session
        if (TRUE == bClient) {
            // Join the session
            JoinSession(pSessionObject, nSetup, SetupTable[nSetup].bSecure);
        }
        else {
            // Host the session
            nBandwidth = HostSession(pSessionObject, nSetup, SetupTable[nSetup].bSecure);
        }



        // Open the log file
        CreateDirectory("T:\\XNetPerf", NULL);
        if (BANDWIDTH_MAX == nBandwidth) {
            sprintf(szLogFileName, "T:\\XNetPerf\\Max %s %s All.log", SetupTable[nSetup].lpszSecurityName, (TRUE == bClient) ? "Send" : "Receive");
        }
        else {
            sprintf(szLogFileName, "T:\\XNetPerf\\%dkbps %s %s All.log", nBandwidth, SetupTable[nSetup].lpszSecurityName, (TRUE == bClient) ? "Send" : "Receive");
        }
        hLogAll = CreateFile(szLogFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (BANDWIDTH_MAX == nBandwidth) {
            sprintf(szLogFileName, "T:\\XNetPerf\\Max %s %s Summary.log", SetupTable[nSetup].lpszSecurityName, (TRUE == bClient) ? "Send" : "Receive");
        }
        else {
            sprintf(szLogFileName, "T:\\XNetPerf\\%dkbps %s %s Summary.log", nBandwidth, SetupTable[nSetup].lpszSecurityName, (TRUE == bClient) ? "Send" : "Receive");
        }
        hLogSummary = CreateFile(szLogFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (BANDWIDTH_MAX == nBandwidth) {
            sprintf(szLogFileName, "T:\\XNetPerf\\Max %s %s.csv", SetupTable[nSetup].lpszSecurityName, (TRUE == bClient) ? "Send" : "Receive");
        }
        else {
            sprintf(szLogFileName, "T:\\XNetPerf\\%dkbps %s %s.csv", nBandwidth, SetupTable[nSetup].lpszSecurityName, (TRUE == bClient) ? "Send" : "Receive");
        }
        hLogCsv = CreateFile(szLogFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        LogCsvHeader(hLogCsv);

        // Flush the file buffers to prevent corruption
        FlushFileBuffers(hLogAll);
        FlushFileBuffers(hLogSummary);
        FlushFileBuffers(hLogCsv);



        // Run the test session
        TestSession(pSessionObject, bClient, hLogAll, hLogSummary, hLogCsv);



        // Close the session
        CloseSession(pSessionObject);

        // Flush the file buffers to prevent corruption
        // Close the log file
        FlushFileBuffers(hLogAll);
        CloseHandle(hLogAll);

        FlushFileBuffers(hLogSummary);
        CloseHandle(hLogSummary);

        FlushFileBuffers(hLogCsv);
        CloseHandle(hLogCsv);

        // Cleanup Winsock
        WSACleanup();

        // Cleanup XNet
        XNetCleanup();
    }

    // Launch the dashboard
    XLaunchNewImage(NULL, NULL);
}
