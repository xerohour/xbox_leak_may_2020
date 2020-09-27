/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  closesocket.c

Abstract:

  This modules tests closesocket

Author:

  Steven Kehrli (steveke) 2-Mar-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// closesocket messages

#define CLOSESOCKET_REQUEST_MSG   NETSYNC_MSG_USER + 290 + 1
#define CLOSESOCKET_COMPLETE_MSG  NETSYNC_MSG_USER + 290 + 2

typedef struct _CLOSESOCKET_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    BOOL     bNonblock;
    u_short  Port;
    BOOL     bServerAccept;
    BOOL     bFillSendQueue;
    BOOL     bFillReceiveQueue;
    BOOL     bClearQueue;
    INT      nSendQueueLen;
    int      linger;
    u_short  l_onoff;
    u_short  l_linger;
    BOOL     bServerClose;
} CLOSESOCKET_REQUEST, *PCLOSESOCKET_REQUEST;

typedef struct _CLOSESOCKET_COMPLETE {
    DWORD    dwMessageId;
    INT      nSendQueueLen;
    INT      nReceiveQueueLen;
} CLOSESOCKET_COMPLETE, *PCLOSESOCKET_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct CLOSESOCKET_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL     bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL     bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD    dwSocket;                                // dwSocket indicates the socket to be created
    BOOL     bNonblock;                               // bNonblock indicates if the socket is to be set as non-blocking
    BOOL     bBind;                                   // bBind indicates if the socket is to be bound
    BOOL     bListen;                                 // bListen indicates if the socket is to be placed in the listening state
    BOOL     bAccept;                                 // bAccept indicates if the socket is accepted
    BOOL     bConnect;                                // bConnect indicates if the socket is connected
    BOOL     bFillSendQueue;                          // bFillSendQueue specifies to fill the send queue
    BOOL     bFillReceiveQueue;                       // bFillReceiveQueue specifies to fill the receive queue
    BOOL     bClearQueue;                             // bClearQueue specifies the server to clear its queue via receive
    BOOL     bSendPending;                            // bSendPending specifies if a send operation is pending
    BOOL     bReceivePending;                         // bReceivePending specifies if a receive operation is pending
    int      linger;                                  // linger specifies the linger option
    u_short  l_onoff;                                 // l_onoff specifies if socket remains open to enable queued data to be sent
    u_short  l_linger;                                // l_linger specifies the time-out value for the socket
    BOOL     bServerClose;                            // bServerClose indicates if server closes connection
    int      iReturnCode;                             // iReturnCode is the return code of closesocket
    int      iLastError;                              // iLastError is the error code if the operation failed
    BOOL     bRIP;                                    // Specifies a RIP test case
} CLOSESOCKET_TABLE, *PCLOSESOCKET_TABLE;

static CLOSESOCKET_TABLE closesocketTable[] =
{
    { "29.1 Not Initialized",          FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "29.2 s = INT_MIN",              TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "29.3 s = -1",                   TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "29.4 s = 0",                    TRUE,  TRUE,  SOCKET_ZERO,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "29.5 s = INT_MAX",              TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "29.6 s = INVALID_SOCKET",       TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "29.7 TCP L NB DL",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.8 TCP L NB L0",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.9 TCP L NB L10",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.10 TCP L NB L15",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.11 TCP L B DL",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.12 TCP L B L0",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.13 TCP L B L10",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.14 TCP L B L15",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.15 TCP L L DL",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.16 TCP L L L0",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.17 TCP L L L10",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.18 TCP L L L15",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.19 TCP L A DL",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.20 TCP L A L0",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.21 TCP L A L10",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.22 TCP L A L15",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.23 TCP L C DL",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.24 TCP L C L0",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.25 TCP L C L10",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.26 TCP L C L15",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.27 TCP L C TFull DL",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.28 TCP L C TFull L0",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.29 TCP L C TFull L10",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.30 TCP L C TFull L15",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.31 TCP L C TFullC DL",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.32 TCP L C TFullC L0",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.33 TCP L C TFullC L10",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.34 TCP L C TFullC L15",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.35 TCP L C RFull DL",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.36 TCP L C RFull L0",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.37 TCP L C RFull L10",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.38 TCP L C RFull L15",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.39 TCP L C TRFull DL",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.40 TCP L C TRFull L0",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.41 TCP L C TRFull L10",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.42 TCP L C TRFull L15",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.43 TCP L C TRFullC DL",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.44 TCP L C TRFullC L0",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.45 TCP L C TRFullC L10",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.46 TCP L C TRFullC L15",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.47 TCP L C TPend DL",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.48 TCP L C TPend L0",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.49 TCP L C TPend L10",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.50 TCP L C TPend L15",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.51 TCP L C TPendC DL",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.52 TCP L C TPendC L0",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.53 TCP L C TPendC L10",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.54 TCP L C TPendC L15",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.55 TCP L C RPend DL",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.56 TCP L C RPend L0",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.57 TCP L C RPend L10",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.58 TCP L C RPend L15",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.59 TCP L C TRPend DL",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.60 TCP L C TRPend L0",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.61 TCP L C TRPend L10",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.62 TCP L C TRPend L15",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.63 TCP L C TRPendC DL",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.64 TCP L C TRPendC L0",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.65 TCP L C TRPendC L10",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.66 TCP L C TRPendC L15",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.67 TCP L NB DL NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.68 TCP L NB L0 NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.69 TCP L NB L10 NB",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.70 TCP L NB L15 NB",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.71 TCP L B DL NB",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.72 TCP L B L0 NB",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.73 TCP L B L10 NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.74 TCP L B L15 NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.75 TCP L L DL NB",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.76 TCP L L L0 NB",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.77 TCP L L L10 NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.78 TCP L L L15 NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.79 TCP L A DL NB",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.80 TCP L A L0 NB",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.81 TCP L A L10 NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.82 TCP L A L15 NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.83 TCP L C DL NB",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.84 TCP L C L0 NB",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.85 TCP L C L10 NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.86 TCP L C L15 NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.87 TCP L C TFull DL NB",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.88 TCP L C TFull L0 NB",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.89 TCP L C TFull L10 NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.90 TCP L C TFull L15 NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
// BUGBUG: Xbox always does asynchronous linger
//    { "29.90 TCP L C TFull L15 NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, SOCKET_ERROR, WSAEWOULDBLOCK    },
    { "29.91 TCP L C TFullC DL NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.92 TCP L C TFullC L0 NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.93 TCP L C TFullC L10 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.94 TCP L C TFullC L15 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
// BUGBUG: Xbox always does asynchronous linger
//    { "29.94 TCP L C TFullC L15 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, SO_LINGER,     1, 5, FALSE, SOCKET_ERROR, WSAEWOULDBLOCK    },
    { "29.95 TCP L C RFull DL NB",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.96 TCP L C RFull L0 NB",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.97 TCP L C RFull L10 NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.98 TCP L C RFull L15 NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.99 TCP L C TRFull DL NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.100 TCP L C TRFull L0 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.101 TCP L C TRFull L10 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.102 TCP L C TRFull L15 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
// BUGBUG: Xbox always does asynchronous linger
//    { "29.102 TCP L C TRFull L15 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, SOCKET_ERROR, WSAEWOULDBLOCK    },
    { "29.103 TCP L C TRFullC DL NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.104 TCP L C TRFullC L0 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.105 TCP L C TRFullC L10 NB", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.106 TCP L C TRFullC L15 NB", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
// BUGBUG: Xbox always does asynchronous linger
//    { "29.106 TCP L C TRFullC L15 NB", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, SO_LINGER,     1, 5, FALSE, SOCKET_ERROR, WSAEWOULDBLOCK    },
    { "29.107 TCP L C TPend DL NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.108 TCP L C TPend L0 NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.109 TCP L C TPend L10 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.110 TCP L C TPend L15 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
// BUGBUG: Xbox always does asynchronous linger
//    { "29.110 TCP L C TPend L15 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, SO_LINGER,     1, 5, FALSE, SOCKET_ERROR, WSAEWOULDBLOCK    },
    { "29.111 TCP L C TPendC DL NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.112 TCP L C TPendC L0 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.113 TCP L C TPendC L10 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.114 TCP L C TPendC L15 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
// BUGBUG: Xbox always does asynchronous linger
//    { "29.114 TCP L C TPendC L15 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, SO_LINGER,     1, 5, FALSE, SOCKET_ERROR, WSAEWOULDBLOCK    },
    { "29.115 TCP L C RPend DL NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.116 TCP L C RPend L0 NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.117 TCP L C RPend L10 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.118 TCP L C RPend L15 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.119 TCP L C TRPend DL NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.120 TCP L C TRPend L0 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.121 TCP L C TRPend L10 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.122 TCP L C TRPend L15 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
// BUGBUG: Xbox always does asynchronous linger
//    { "29.122 TCP L C TRPend L15 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  SO_LINGER,     1, 5, FALSE, SOCKET_ERROR, WSAEWOULDBLOCK    },
    { "29.123 TCP L C TRPendC DL NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.124 TCP L C TRPendC L0 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.125 TCP L C TRPendC L10 NB", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.126 TCP L C TRPendC L15 NB", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
// BUGBUG: Xbox always does asynchronous linger
//    { "29.126 TCP L C TRPendC L15 NB", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  SO_LINGER,     1, 5, FALSE, SOCKET_ERROR, WSAEWOULDBLOCK    },
    { "29.127 TCP R A DL NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, TRUE,  0,            0,                 FALSE },
    { "29.128 TCP R A L0 NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, TRUE,  0,            0,                 FALSE },
    { "29.129 TCP R A L10 NB",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, TRUE,  0,            0,                 FALSE },
    { "29.130 TCP R A L15 NB",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, TRUE,  0,            0,                 FALSE },
    { "29.131 TCP R C DL NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, TRUE,  0,            0,                 FALSE },
    { "29.132 TCP R C L0 NB",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, TRUE,  0,            0,                 FALSE },
    { "29.133 TCP R C L10 NB",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, TRUE,  0,            0,                 FALSE },
    { "29.134 TCP R C L15 NB",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, TRUE,  0,            0,                 FALSE },
    { "29.135 TCP R C TFull DL NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, TRUE,  0,            0,                 FALSE },
    { "29.136 TCP R C TFull L0 NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, TRUE,  0,            0,                 FALSE },
    { "29.137 TCP R C TFull L10 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, TRUE,  0,            0,                 FALSE },
    { "29.138 TCP R C RFull DL NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, TRUE,  0,            0,                 FALSE },
    { "29.139 TCP R C RFull L0 NB",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     0, 0, TRUE,  0,            0,                 FALSE },
    { "29.140 TCP R C RFull L10 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     1, 0, TRUE,  0,            0,                 FALSE },
    { "29.141 TCP R C RFullC DL NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE, SO_DONTLINGER, 0, 0, TRUE,  0,            0,                 FALSE },
    { "29.142 TCP R C RFullC L0 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE, SO_LINGER,     0, 0, TRUE,  0,            0,                 FALSE },
    { "29.143 TCP R C RFullC L10 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE, SO_LINGER,     1, 0, TRUE,  0,            0,                 FALSE },
    { "29.144 TCP R C TRFull DL NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, TRUE,  0,            0,                 FALSE },
    { "29.145 TCP R C TRFull L0 NB",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     0, 0, TRUE,  0,            0,                 FALSE },
    { "29.146 TCP R C TRFull L10 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, SO_LINGER,     1, 0, TRUE,  0,            0,                 FALSE },
    { "29.147 TCP R C TRFullC DL NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, SO_DONTLINGER, 0, 0, TRUE,  0,            0,                 FALSE },
    { "29.148 TCP R C TRFullC L0 NB",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, SO_LINGER,     0, 0, TRUE,  0,            0,                 FALSE },
    { "29.149 TCP R C TRFullC L10 NB", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, SO_LINGER,     1, 0, TRUE,  0,            0,                 FALSE },
    { "29.150 UDP L NB DL",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.151 UDP L NB L0",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.152 UDP L NB L10",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.153 UDP L NB L15",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.154 UDP L B DL",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.155 UDP L B L0",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.156 UDP L B L10",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.157 UDP L B L15",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.158 UDP L C DL",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.159 UDP L C L0",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.160 UDP L C L10",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.161 UDP L C L15",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.162 UDP L C RPend DL",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.163 UDP L C RPend L0",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.164 UDP L C RPend L10",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.165 UDP L C RPend L15",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.166 UDP L NB DL NB",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.167 UDP L NB L0 NB",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.168 UDP L NB L10 NB",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.169 UDP L NB L15 NB",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.170 UDP L B DL NB",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.171 UDP L B L0 NB",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.172 UDP L B L10 NB",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.173 UDP L B L15 NB",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.174 UDP L C DL NB",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.175 UDP L C L0 NB",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.176 UDP L C L10 NB",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.177 UDP L C L15 NB",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.178 UDP L C RPend DL NB",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_DONTLINGER, 0, 0, FALSE, 0,            0,                 FALSE },
    { "29.179 UDP L C RPend L0 NB",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_LINGER,     0, 0, FALSE, 0,            0,                 FALSE },
    { "29.180 UDP L C RPend L10 NB",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_LINGER,     1, 0, FALSE, 0,            0,                 FALSE },
    { "29.181 UDP L C RPend L15 NB",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  SO_LINGER,     1, 5, FALSE, 0,            0,                 FALSE },
    { "29.182 TCP Closed",             TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "29.183 UDP Closed",             TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "29.184 Not Initialized",        FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, SO_DONTLINGER, 0, 0, FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define closesocketTableCount (sizeof(closesocketTable) / sizeof(CLOSESOCKET_TABLE))

NETSYNC_TYPE_THREAD  closesocketTestSessionNt =
{
    1,
    closesocketTableCount,
    L"xnetapi_nt.dll",
    "closesocketTestServer"
};

NETSYNC_TYPE_THREAD  closesocketTestSessionXbox =
{
    1,
    closesocketTableCount,
    L"xnetapi_xbox.dll",
    "closesocketTestServer"
};



VOID
closesocketTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests closesocket - Client side

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
    LPSTR                  lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR                  lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD                  dwTableIndex;

    // bWinsockInitialized indicates if Winsock is initialized
    BOOL                   bWinsockInitialized = FALSE;
    // WSAData is the details of the Winsock implementation
    WSADATA                WSAData;

    // hNetsyncObject is a handle to the netsync object
    HANDLE                 hNetsyncObject = INVALID_HANDLE_VALUE;
    // NetsyncTypeSession is the session type descriptor
    NETSYNC_TYPE_THREAD    NetsyncTypeSession;
    // NetsyncInAddr is the address of the netsync server
    u_long                 NetsyncInAddr = 0;
    // LowPort is the low bound of the netsync port range
    u_short                LowPort = 0;
    // HighPort is the high bound of the netsync port range
    u_short                HighPort = 0;
    // CurrentPort is the current port in the netsync port range
    u_short                CurrentPort = 0;
    // FromInAddr is the address of the netsync sender
    u_long                 FromInAddr;
    // dwMessageSize is the size of the received message
    DWORD                  dwMessageSize;
    // pMessage is a pointer to the received message
    char                   *pMessage;
    // closesocketRequest is the request sent to the server
    CLOSESOCKET_REQUEST    closesocketRequest;
    
    // sSocket is the socket descriptor
    SOCKET                 sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                 nsSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD                  dwBufferSize = 1;
    // bNagle indicates if Nagle is enabled
    BOOL                   bNagle = FALSE;
    // Nonblock sets the socket to blocking or non-blocking mode
    u_long                 Nonblock;
    // bNonblocking indicates if the socket is in non-blocking mode
    BOOL                   bNonblocking = FALSE;

    // localname is the local address
    SOCKADDR_IN            localname;
    // remotename is the remote address
    SOCKADDR_IN            remotename;

    // readfds is the set of sockets to check for a read condition
    fd_set                 readfds;
    // writefds is the set of sockets to check for a write condition
    fd_set                 writefds;

    // SendBufferLarge is the large send buffer
    char                   SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // RecvBuffer10 is the recv buffer
    char                   RecvBuffer10[BUFFER_10_LEN + 1];
    // RecvBufferLarge is the large recv buffer
    char                   RecvBufferLarge[BUFFER_LARGE_LEN + 1];

    // dwFirstTime is the first tick count
    DWORD                  dwFirstTime;
    // dwSecondTime is the second tick count
    DWORD                  dwSecondTime;

    // bsOpened indicates the sSocket socket is opened
    BOOL                   bsOpened;
    // bnsOpened indicates the nsSocket socket is opened
    BOOL                   bnsOpened;
    // nSendQueueLen is the size of the send queue
    int                    nSendQueueLen = 0;
    // nReceiveQueueLen is the size of the receive queue
    int                    nReceiveQueueLen = 0;
    // nReceiveBuffer is a counter to empty the receive queue
    int                    nReceiveBuffer;
    // bDontLinger indicates if SO_DONTLINGER is set
    BOOL                   bDontLinger;
    // linger is the linger structure
    struct linger          linger;

    // hSendEvent is a handle to the overlapped send event
    HANDLE                 hSendEvent;
    // WSASendOverlapped is the overlapped send structure
    WSAOVERLAPPED          WSASendOverlapped;
    // WSASendBuf is the WSABUF structure
    WSABUF                 WSASendBuf;
    // dwSendBytes is the number of bytes transferred for the function call
    DWORD                  dwSendBytes;
    // dwSendFlags is the transfer flags for the function call
    DWORD                  dwSendFlags;

    // hReceiveEvent is a handle to the overlapped receive event
    HANDLE                 hReceiveEvent;
    // WSAReceiveOverlapped is the overlapped receive structure
    WSAOVERLAPPED          WSAReceiveOverlapped;
    // WSAReceiveBuf is the WSABUF structure
    WSABUF                 WSAReceiveBuf;
    // dwReceiveBytes is the number of bytes transferred for the function call
    DWORD                  dwReceiveBytes;
    // dwReceiveFlags is the transfer flags for the function call
    DWORD                  dwReceiveFlags;

    // dwReturnCode is the return code of WaitForSingleObject
    DWORD                  dwReturnCode;

    // bException indicates if an exception occurred
    BOOL                   bException;
    // iReturnCode is the return code of the operation
    int                    iReturnCode;
    // iLastError is the error code if the operation failed
    int                    iLastError;
    // bTestPassed indicates if the test passed
    BOOL                   bTestPassed;

    // szFunctionName is the function name
    CHAR                   szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    sprintf(szFunctionName, "closesocket v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_closesocket+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_closesocket-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = closesocketTestSessionXbox;
    }
    else {
        NetsyncTypeSession = closesocketTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < closesocketTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, closesocketTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, closesocketTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != closesocketTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, closesocketTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == closesocketTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != closesocketTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == closesocketTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = closesocketTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == closesocketTable[dwTableIndex].bNetsyncConnected)) {
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

        // Create the overlapped event
        hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        hReceiveEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

        // Setup the overlapped structure
        ZeroMemory(&WSASendOverlapped, sizeof(WSASendOverlapped));
        WSASendOverlapped.hEvent = hSendEvent;

        ZeroMemory(&WSAReceiveOverlapped, sizeof(WSAReceiveOverlapped));
        WSAReceiveOverlapped.hEvent = hReceiveEvent;

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        if (SOCKET_INT_MIN == closesocketTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == closesocketTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == closesocketTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == closesocketTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == closesocketTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & closesocketTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
            bsOpened = TRUE;
        }
        else if (0 != (SOCKET_UDP & closesocketTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
            bsOpened = TRUE;
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & closesocketTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the buffer size
        if ((0 != (SOCKET_TCP & closesocketTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & closesocketTable[dwTableIndex].dwSocket))) {
            dwBufferSize = 1;
            setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

            dwBufferSize = 1;
            setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
        }

        // Set the socket to non-blocking mode
        if (TRUE == closesocketTable[dwTableIndex].bNonblock) {
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);
            bNonblocking = TRUE;
        }
        else {
            bNonblocking = FALSE;
        }

        // Set the linger option
        if (SO_DONTLINGER == closesocketTable[dwTableIndex].linger) {
            bDontLinger = TRUE;
            setsockopt(sSocket, SOL_SOCKET, SO_DONTLINGER, (char *) &bDontLinger, sizeof(bDontLinger));
        }
        else {
            linger.l_onoff = closesocketTable[dwTableIndex].l_onoff;
            linger.l_linger = closesocketTable[dwTableIndex].l_linger;
            setsockopt(sSocket, SOL_SOCKET, SO_LINGER, (char *) &linger, sizeof(linger));
        }

        // Bind the socket
        if (TRUE == closesocketTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Place the socket in the listening state
        if (TRUE == closesocketTable[dwTableIndex].bListen) {
            listen(sSocket, SOMAXCONN);
        }

        if ((TRUE == closesocketTable[dwTableIndex].bAccept) || (TRUE == closesocketTable[dwTableIndex].bConnect)) {
            // Initialize the closesocket request
            closesocketRequest.dwMessageId = CLOSESOCKET_REQUEST_MSG;
            if (0 != (SOCKET_TCP & closesocketTable[dwTableIndex].dwSocket)) {
                closesocketRequest.nSocketType = SOCK_STREAM;
            }
            else {
                closesocketRequest.nSocketType = SOCK_DGRAM;
            }
            closesocketRequest.bNonblock = closesocketTable[dwTableIndex].bNonblock;
            closesocketRequest.Port = CurrentPort;
            closesocketRequest.bServerAccept = closesocketTable[dwTableIndex].bConnect;
            closesocketRequest.bFillSendQueue = closesocketTable[dwTableIndex].bFillSendQueue;
            closesocketRequest.bFillReceiveQueue = closesocketTable[dwTableIndex].bFillReceiveQueue;
            closesocketRequest.bClearQueue = closesocketTable[dwTableIndex].bClearQueue;
            closesocketRequest.nSendQueueLen = 0;
            closesocketRequest.linger = closesocketTable[dwTableIndex].linger;
            closesocketRequest.l_onoff = closesocketTable[dwTableIndex].l_onoff;
            closesocketRequest.l_linger = closesocketTable[dwTableIndex].l_linger;
            closesocketRequest.bServerClose = closesocketTable[dwTableIndex].bServerClose;

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(closesocketRequest), (char *) &closesocketRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == closesocketTable[dwTableIndex].bConnect) {
                // Connect the socket
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = NetsyncInAddr;
                remotename.sin_port = htons(CurrentPort);

                connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));

                if (0 != (SOCKET_TCP & closesocketTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&writefds);
                    FD_SET(sSocket, &writefds);
                    select(0, NULL, &writefds, NULL, NULL);
                }
            }
            else if (TRUE == closesocketTable[dwTableIndex].bAccept) {
                if (0 != (SOCKET_TCP & closesocketTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&readfds);
                    FD_SET(sSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);
                }

                // Accept the socket
                nsSocket = accept(sSocket, NULL, NULL);
                bnsOpened = TRUE;
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(closesocketRequest), (char *) &closesocketRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == closesocketTable[dwTableIndex].bFillSendQueue) {
                if (FALSE == bNonblocking) {
                    Nonblock = 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                }

                // Fill the queue
                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                while (SOCKET_ERROR != send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0)) {
                    closesocketRequest.nSendQueueLen++;
                    Sleep(SLEEP_ZERO_TIME);
                }

                if (FALSE == bNonblocking) {
                    Nonblock = 0;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                }
            }

            // Start the pending overlapped operation
            if (TRUE == closesocketTable[dwTableIndex].bSendPending) {
                // Call WSASend
                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                WSASendBuf.len = BUFFER_TCPSEGMENT_LEN;
                WSASendBuf.buf = SendBufferLarge;
                dwSendBytes = 0;

                iReturnCode = WSASend((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSASendBuf, 1, &dwSendBytes, 0, &WSASendOverlapped, NULL);

                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "WSASend returned non-SOCKET_ERROR");
                }
                else if (WSA_IO_PENDING != WSAGetLastError()) {
                    xLog(hLog, XLL_FAIL, "WSASend iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, WSAGetLastError());
                }
            }

            if (TRUE == closesocketTable[dwTableIndex].bReceivePending) {
                // Call WSARecv
                ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                WSAReceiveBuf.len = sizeof(RecvBuffer10);
                WSAReceiveBuf.buf = RecvBuffer10;
                dwReceiveBytes = 0;
                dwReceiveFlags = 0;

                iReturnCode = WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSAReceiveBuf, 1, &dwReceiveBytes, &dwReceiveFlags, &WSAReceiveOverlapped, NULL);

                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "WSARecv returned non-SOCKET_ERROR");
                }
                else if (WSA_IO_PENDING != WSAGetLastError()) {
                    xLog(hLog, XLL_FAIL, "WSARecv iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, WSAGetLastError());
                }
            }
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & closesocketTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
            bsOpened = FALSE;
        }

        __try {
            // Call closesocket
            iReturnCode = 0;
            if (FALSE == closesocketTable[dwTableIndex].bServerClose) {
                // Get the current tick count
                dwFirstTime = GetTickCount();

                iReturnCode = closesocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket);
                iLastError = WSAGetLastError();

                // Get the current tick count
                dwSecondTime = GetTickCount();

                if (0 == iReturnCode) {
                    if (INVALID_SOCKET == nsSocket) {
                        bsOpened = FALSE;
                    }
                    else {
                        bnsOpened = FALSE;
                    }
                }
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            xLog(hLog, XLL_EXCEPTION, "closesocket caused an exception - ec = 0x%08x", GetExceptionCode());
        }

        if (FALSE == bException) {
            if (TRUE == closesocketTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "closesocket did not RIP");
            }

            if ((TRUE == closesocketTable[dwTableIndex].bAccept) || (TRUE == closesocketTable[dwTableIndex].bConnect)) {
                // Send the connect request
                NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(closesocketRequest), (char *) &closesocketRequest);

                // Wait for the connect complete
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                nSendQueueLen = ((PCLOSESOCKET_COMPLETE) pMessage)->nSendQueueLen;
                nReceiveQueueLen = ((PCLOSESOCKET_COMPLETE) pMessage)->nReceiveQueueLen;
                NetsyncFreeMessage(pMessage);
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == closesocketTable[dwTableIndex].iReturnCode)) {
                if (iLastError != closesocketTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "closesocket iLastError - EXPECTED: %u; RECEIVED: %u", closesocketTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "closesocket iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "closesocket returned SOCKET_ERROR - ec = %u", iLastError);
            }
            else if (SOCKET_ERROR == closesocketTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "closesocket returned non-SOCKET_ERROR");
            }
            else {
                if (FALSE == closesocketTable[dwTableIndex].bServerClose) {
                    if ((dwSecondTime - dwFirstTime) > SLEEP_ZERO_TIME) {
                        xLog(hLog, XLL_FAIL, "closesocket nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_ZERO_TIME, dwSecondTime - dwFirstTime);
                        bTestPassed = FALSE;
                    }
                }

// BUGBUG: Xbox always does asynchronous linger
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                if (FALSE == closesocketTable[dwTableIndex].bServerClose) {
                    if ((5 == closesocketTable[dwTableIndex].l_linger) && (TRUE == closesocketTable[dwTableIndex].bFillSendQueue)) {
                        if (FALSE == closesocketTable[dwTableIndex].bClearQueue) {
                            if (((dwSecondTime - dwFirstTime) < (SLEEP_LOW_TIME * 2)) || ((dwSecondTime - dwFirstTime) > (SLEEP_HIGH_TIME * 2))) {
                                xLog(hLog, XLL_FAIL, "closesocket nTime - EXPECTED: %d; RECEIVED: %d", 5000, dwSecondTime - dwFirstTime);
                                bTestPassed = FALSE;
                            }
                        }
                        else {
                            if ((dwSecondTime - dwFirstTime) > (SLEEP_HIGH_TIME * 2)) {
                                xLog(hLog, XLL_FAIL, "closesocket nTime - EXPECTED: %d; RECEIVED: %d", 5000, dwSecondTime - dwFirstTime);
                                bTestPassed = FALSE;
                            }
                        }
                    }
                    else {
                        if ((dwSecondTime - dwFirstTime) > SLEEP_ZERO_TIME) {
                            xLog(hLog, XLL_FAIL, "closesocket nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_ZERO_TIME, dwSecondTime - dwFirstTime);
                            bTestPassed = FALSE;
                        }
                    }
                }
------------------------------------------------------------------------------*/

                if (TRUE == closesocketTable[dwTableIndex].bSendPending) {
                    dwReturnCode = WaitForSingleObject(hSendEvent, 0);
                    if (WAIT_TIMEOUT == dwReturnCode) {
                        xLog(hLog, XLL_FAIL, "WaitForSingleObject send returned WAIT_TIMEOUT");
                        bTestPassed = FALSE;
                    }
                }

                if (TRUE == closesocketTable[dwTableIndex].bReceivePending) {
                    dwReturnCode = WaitForSingleObject(hReceiveEvent, 0);
                    if (WAIT_TIMEOUT == dwReturnCode) {
                        xLog(hLog, XLL_FAIL, "WaitForSingleObject receive returned WAIT_TIMEOUT");
                        bTestPassed = FALSE;
                    }
                }

                if (((TRUE == closesocketTable[dwTableIndex].bAccept) || (TRUE == closesocketTable[dwTableIndex].bConnect)) && (TRUE == closesocketTable[dwTableIndex].bClearQueue) && (FALSE == closesocketTable[dwTableIndex].bServerClose)) {
                    if ((TRUE == closesocketTable[dwTableIndex].bFillReceiveQueue) || ((1 == closesocketTable[dwTableIndex].l_onoff) && (0 == closesocketTable[dwTableIndex].l_linger))) {
                        if (nSendQueueLen >= closesocketRequest.nSendQueueLen) {
                            xLog(hLog, XLL_FAIL, "nSendQueueLen - EXPECTED: <%d; RECEIVED: %d", closesocketRequest.nSendQueueLen, nSendQueueLen);
                            bTestPassed = FALSE;
                        }
                    }
                    else {
                        if (nSendQueueLen != closesocketRequest.nSendQueueLen) {
                            xLog(hLog, XLL_FAIL, "nSendQueueLen - EXPECTED: %d; RECEIVED: %d", closesocketRequest.nSendQueueLen, nSendQueueLen);
                            bTestPassed = FALSE;
                        }
                    }
                }

                if ((INVALID_SOCKET == nsSocket) ? (TRUE == bsOpened) : (TRUE == bnsOpened)) {
                    if ((TRUE == closesocketTable[dwTableIndex].bFillReceiveQueue) && (TRUE == closesocketRequest.bClearQueue)) {
                        if (FALSE == bNonblocking) {
                            Nonblock = 1;
                            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                        }

                        // Clear the queue
                        nReceiveBuffer = 0;
                        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
                        do {
                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, BUFFER_TCPSEGMENT_LEN, 0);
                            if ((0 != iReturnCode) && (SOCKET_ERROR != iReturnCode)) {
                                Sleep(SLEEP_ZERO_TIME);
                                nReceiveBuffer += iReturnCode;
                            }
                        } while ((0 != iReturnCode) && (SOCKET_ERROR != iReturnCode));
                        nReceiveBuffer = (nReceiveBuffer / BUFFER_TCPSEGMENT_LEN) + ((nReceiveBuffer % BUFFER_TCPSEGMENT_LEN) ? 1 : 0);

                        if (FALSE == bNonblocking) {
                            Nonblock = 0;
                            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                        }

                        if (nReceiveBuffer != nReceiveQueueLen) {
                            xLog(hLog, XLL_FAIL, "nReceiveQueueLen - EXPECTED: %d; RECEIVED: %d", nReceiveQueueLen, nReceiveBuffer);
                            bTestPassed = FALSE;
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "closesocket succeeded");
                }
            }
        }

        if ((TRUE == closesocketTable[dwTableIndex].bAccept) || (TRUE == closesocketTable[dwTableIndex].bConnect)) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(closesocketRequest), (char *) &closesocketRequest);
        }

        // Close the sockets
        if (TRUE == bnsOpened) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (TRUE == bsOpened) {
            shutdown(sSocket, SD_BOTH);
            closesocket(sSocket);
        }

        // Close the event
        CloseHandle(hReceiveEvent);
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
closesocketTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests closesocket - Server side

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
    u_long                FromInAddr;
    // dwMessageType is the type of received message
    DWORD                 dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD                 dwMessageSize;
    // pMessage is a pointer to the received message
    char                  *pMessage;
    // closesocketRequest is the request
    CLOSESOCKET_REQUEST   closesocketRequest;
    // closesocketComplete is the result
    CLOSESOCKET_COMPLETE  closesocketComplete;

    // sSocket is the socket descriptor
    SOCKET                sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                nsSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD                 dwBufferSize = 1;
    // bNagle indicates if Nagle is enabled
    BOOL                  bNagle = FALSE;
    // Nonblock sets the socket to blocking or non-blocking mode
    u_long                Nonblock;
    // bNonblocking indicates if the socket is in non-blocking mode
    BOOL                  bNonblocking = FALSE;

    // localname is the local address
    SOCKADDR_IN           localname;
    // remotename is the remote address
    SOCKADDR_IN           remotename;

    // SendBufferLarge is the large send buffer
    char                  SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // RecvBufferLarge is the large recv buffer
    char                  RecvBufferLarge[BUFFER_LARGE_LEN + 1];

    // bDontLinger indicates if SO_DONTLINGER is set
    BOOL                  bDontLinger;
    // linger is the linger structure
    struct linger         linger;

    // iReturnCode is the return code of the operation
    int                    iReturnCode;



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
        CopyMemory(&closesocketRequest, pMessage, sizeof(closesocketRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, closesocketRequest.nSocketType, 0);

        if (SOCK_STREAM == closesocketRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the buffer size
        dwBufferSize = 1;
        setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

        dwBufferSize = 1;
        setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

        // Set the socket to non-blocking mode
        if (TRUE == closesocketRequest.bNonblock) {
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);
            bNonblocking = TRUE;
        }
        else {
            bNonblocking = FALSE;
        }

        // Set the linger option
        if (SO_DONTLINGER == closesocketRequest.linger) {
            bDontLinger = TRUE;
            setsockopt(sSocket, SOL_SOCKET, SO_DONTLINGER, (char *) &bDontLinger, sizeof(bDontLinger));
        }
        else {
            linger.l_onoff = closesocketRequest.l_onoff;
            linger.l_linger = closesocketRequest.l_linger;
            setsockopt(sSocket, SOL_SOCKET, SO_LINGER, (char *) &linger, sizeof(linger));
        }

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(closesocketRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if ((SOCK_STREAM == closesocketRequest.nSocketType) && (TRUE == closesocketRequest.bServerAccept)) {
            // Place the socket in listening mode
            listen(sSocket, SOMAXCONN);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(closesocketRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        closesocketComplete.dwMessageId = CLOSESOCKET_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(closesocketComplete), (char *) &closesocketComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if ((SOCK_STREAM == closesocketRequest.nSocketType) && (TRUE == closesocketRequest.bServerAccept)) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        // Fill the receive queue
        closesocketComplete.nReceiveQueueLen = 0;
        if (TRUE == closesocketRequest.bFillReceiveQueue) {
            if (FALSE == bNonblocking) {
                Nonblock = 1;
                ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
            }

            ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
            while (SOCKET_ERROR != send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0)) {
                closesocketComplete.nReceiveQueueLen++;
                Sleep(SLEEP_ZERO_TIME);
            }

            if (FALSE == bNonblocking) {
                Nonblock = 0;
                ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
            }
        }

        // Send the complete
        closesocketComplete.dwMessageId = CLOSESOCKET_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(closesocketComplete), (char *) &closesocketComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        closesocketComplete.nSendQueueLen = 0;
        if (TRUE == closesocketRequest.bServerClose) {
            closesocket(sSocket);
        }
        else {
            if (TRUE == closesocketRequest.bClearQueue) {
                if (FALSE == bNonblocking) {
                    Nonblock = 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                }

                // Clear the queue
                ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
                do {
                    iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, BUFFER_TCPSEGMENT_LEN, 0);
                    if ((0 != iReturnCode) && (SOCKET_ERROR != iReturnCode)) {
                        Sleep(SLEEP_ZERO_TIME);
                        closesocketComplete.nSendQueueLen += iReturnCode;
                    }
                } while ((0 != iReturnCode) && (SOCKET_ERROR != iReturnCode));
                closesocketComplete.nSendQueueLen = (closesocketComplete.nSendQueueLen / BUFFER_TCPSEGMENT_LEN) + ((closesocketComplete.nSendQueueLen % BUFFER_TCPSEGMENT_LEN) ? 1 : 0);

                if (FALSE == bNonblocking) {
                    Nonblock = 0;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                }
            }
        }

        // Send the complete
        closesocketComplete.dwMessageId = CLOSESOCKET_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(closesocketComplete), (char *) &closesocketComplete);

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
