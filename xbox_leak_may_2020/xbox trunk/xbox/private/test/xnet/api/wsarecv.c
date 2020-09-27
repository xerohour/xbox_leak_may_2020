/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  wsarecv.c

Abstract:

  This modules tests wsarecv

Author:

  Steven Kehrli (steveke) 13-Dev-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// WSARecv messages

#define WSARECV_REQUEST_MSG   NETSYNC_MSG_USER + 180 + 1
#define WSARECV_CANCEL_MSG    NETSYNC_MSG_USER + 180 + 2
#define WSARECV_COMPLETE_MSG  NETSYNC_MSG_USER + 180 + 3

typedef struct _WSARECV_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  Port;
    BOOL     bServerAccept;
    BOOL     bRemoteShutdown;
    BOOL     bRemoteClose;
    int      nDataBuffers;
    int      nBufferlen;
} WSARECV_REQUEST, *PWSARECV_REQUEST;

typedef struct _WSARECV_COMPLETE {
    DWORD    dwMessageId;
} WSARECV_COMPLETE, *PWSARECV_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

#define BUFFER_10             0
#define BUFFER_LARGE          1
#define BUFFER_NULL           2
#define BUFFER_NULLZERO       3
#define BUFFER_ZERO           4
#define BUFFER_ZEROCOUNT      5
#define BUFFER_SMALL          6
#define BUFFER_TCPLARGE       7
#define BUFFER_UDPLARGE       8
#define BUFFER_UDPTOOLARGE    9
#define BUFFER_MULTI         10
#define BUFFER_SECOND        11
#define BUFFER_NONE          12

#define OVERLAPPED_IO_RESULT  1
#define OVERLAPPED_IO_EVENT   2



typedef struct WSARECVBUFFER_TABLE {
    DWORD  dwBuffer;
} WSARECVBUFFER_TABLE, *PWSARECVBUFFER_TABLE;

WSARECVBUFFER_TABLE WSARecvBufferTcpTable[] = { { BUFFER_10        },
                                                { BUFFER_NULLZERO  },
                                                { BUFFER_TCPLARGE  } };

#define WSARecvBufferTcpTableCount  (sizeof(WSARecvBufferTcpTable) / sizeof(WSARECVBUFFER_TABLE));

WSARECVBUFFER_TABLE WSARecvBufferUdpTable[] = { { BUFFER_10        },
                                                { BUFFER_NULLZERO  },
                                                { BUFFER_UDPLARGE  } };

#define WSARecvBufferUdpTableCount  (sizeof(WSARecvBufferUdpTable) / sizeof(WSARECVBUFFER_TABLE));



typedef struct WSARECV_TABLE {
    CHAR           szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL           bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL           bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD          dwSocket;                                // dwSocket indicates the socket to be created
    BOOL           bNonblock;                               // bNonblock indicates if the socket is to be set as non-blocking for the recv
    BOOL           bBind;                                   // bBind indicates if the socket is to be bound
    BOOL           bListen;                                 // bListen indicates if the socket is to be placed in the listening state
    BOOL           bAccept;                                 // bAccept indicates if the socket is accepted
    BOOL           bConnect;                                // bConnect indicates if the socket is connected
    BOOL           bRemoteShutdown;                         // bRemoteShutdown indicates if the remote shutsdown the connection
    BOOL           bRemoteClose;                            // bRemoteClose indicates if the remote closes the connection
    BOOL           bShutdown;                               // bShutdown indicates if the connection is shutdown
    int            nShutdown;                               // nShutdown indicates how the connection is shutdown
    int            nDataBuffers;                            // nDataBuffers indicates the number of data buffers to be received
    DWORD          dwBuffer;                                // dwBuffer specifies the recv data structure
    BOOL           bBytesRecv;                              // bFlags indicates if dwBytes is used
    BOOL           bFlags;                                  // bFlags indicates if dwFlags is used
    DWORD          dwFlags;                                 // dwFlags specifies the recv flags
    BOOL           bSecondRecv;                             // bSecondRecv indicates if a second recv should occur
    int            nOverlapped;                             // nOverlapped specifies if overlapped I/O is used
    BOOL           bCompletionRoutine;                      // bCompletionRoutine specifies if completion routine is used
    int            iReturnCode;                             // iReturnCode is the return code of recv
    int            iLastError;                              // iLastError is the error code if the operation failed
    BOOL           bRIP;                                    // Specifies a RIP test case
} WSARECV_TABLE, *PWSARECV_TABLE;

static WSARECV_TABLE WSARecvTable[] =
{
    { "18.1 Not Initialized",            FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "18.2 s = INT_MIN",                TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "18.3 s = -1",                     TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "18.4 s = 0",                      TRUE,  TRUE,  SOCKET_ZERO,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "18.5 s = INT_MAX",                TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "18.6 s = INVALID_SOCKET",         TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "18.7 Not Bound TCP",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "18.8 Listening TCP",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "18.9 Accepted TCP",               TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.10 Connected TCP",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.11 Second Receive TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SECOND,    TRUE,  TRUE,  0, TRUE,  0,                    FALSE, 0,            0,                 FALSE },
    { "18.12 I/O TCP",                   TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, OVERLAPPED_IO_RESULT, FALSE, 0,            0,                 FALSE },
    { "18.13 Event TCP",                 TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "18.14 Event Event TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,        TRUE,  TRUE,  0, FALSE, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "18.15 SD_RECEIVE Accept TCP",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "18.16 SD_SEND Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.17 SD_BOTH Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "18.18 SD_RECEIVE Connect TCP",    TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "18.19 SD_SEND Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.20 SD_BOTH Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "18.21 Shutdown Accept TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.22 Shutdown Connect TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.23 Close Accept TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "18.24 Close Connect TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "18.25 NULL Buffer TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.26 NULL 0 Bufferlen TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,  TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.27 0 Bufferlen TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.28 Small Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,     TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.29 Exact Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.30 Large Buffer TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_TCPLARGE,  TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.31 Multi Buffer TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_MULTI,     TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.32 Switch Recv TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.33 Iterative Recv TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.34 NULL Buffer Ptr TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.35 0 Buffer Count TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT, TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.36 NULL dwBytes TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        FALSE, TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.37 NULL flags TCP",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  FALSE, 0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.38 flags != 0 TCP",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  4, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.39 Callback != NULL TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.40 Not Bound NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "18.41 Listening NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "18.42 Accepted NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.43 Connected NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.44 Second Receive NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SECOND,    TRUE,  TRUE,  0, TRUE,  0,                    FALSE, 0,            0,                 FALSE },
    { "18.45 I/O NB TCP",                TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, OVERLAPPED_IO_RESULT, FALSE, 0,            0,                 FALSE },
    { "18.46 Event NB TCP",              TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "18.47 Event Event NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,        TRUE,  TRUE,  0, FALSE, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "18.48 SD_RECEIVE Accept NB TCP",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "18.49 SD_SEND Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.50 SD_BOTH Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "18.51 SD_RECEIVE Connect NB TCP", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "18.52 SD_SEND Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.53 SD_BOTH Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "18.54 Shutdown Accept NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.55 Shutdown Connect NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.56 Close Accept NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "18.57 Close Connect NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "18.58 NULL Buffer NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.59 NULL 0 Bufferlen NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,  TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.60 0 Bufferlen NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.61 Small Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,     TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.62 Exact Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.63 Large Buffer NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_TCPLARGE,  TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.64 Multi Buffer NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_MULTI,     TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.65 Switch Recv NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.66 Iterative Recv NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.67 NULL Buffer Ptr NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.68 0 Buffer Count NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT, TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.69 NULL dwBytes NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        FALSE, TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.70 NULL flags NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  FALSE, 0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.71 flags != 0 NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  4, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.72 Callback != NULL NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.73 Not Bound UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "18.74 Not Connected UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.75 Connected UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.76 Second Receive UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SECOND,    TRUE,  TRUE,  0, TRUE,  0,                    FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "18.77 SD_RECEIVE Conn UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "18.78 SD_SEND Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.79 SD_BOTH Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "18.80 I/O UDP",                   TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, OVERLAPPED_IO_RESULT, FALSE, 0,            0,                 FALSE },
    { "18.81 Event UDP",                 TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "18.82 Event Event UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,        TRUE,  TRUE,  0, FALSE, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "18.83 NULL Buffer UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.84 NULL 0 Bufferlen UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,  TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.85 0 Bufferlen UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.86 Small Bufferlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,     TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "18.87 Exact Bufferlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.88 Large Buffer UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPLARGE,  TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.89 Multi Buffer UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_MULTI,     TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.90 Switch Recv UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.91 Iterative Recv UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.92 Null Buffer Ptr UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.93 0 Buffer Count UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT, TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.94 NULL dwBytes UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        FALSE, TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.95 NULL flags UDP",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  FALSE, 0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.96 flags != 0 UDP",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  4, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.97 Callback != NULL UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.98 Not Bound NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "18.99 Not Connected NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.100 Connected NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.101 Second Receive NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SECOND,    TRUE,  TRUE,  0, TRUE,  0,                    FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "18.102 SD_RECEIVE Conn NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "18.103 SD_SEND Conn NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.104 SD_BOTH Conn NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "18.105 I/O NB UDP",               TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, OVERLAPPED_IO_RESULT, FALSE, 0,            0,                 FALSE },
    { "18.106 Event NB UDP",             TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "18.107 Event Event NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,        TRUE,  TRUE,  0, FALSE, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "18.108 NULL Buffer NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.109 NULL 0 Bufferlen NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,  TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.110 0 Bufferlen NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.111 Small Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,     TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "18.112 Exact Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.113 Large Buffer NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPLARGE,  TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.114 Multi Buffer NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_MULTI,     TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.115 Switch Recv NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.116 Iterative Recv NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    FALSE, 0,            0,                 FALSE },
    { "18.117 Null Buffer Ptr NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.118 0 Buffer Count NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT, TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.119 NULL dwBytes NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        FALSE, TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.120 NULL flags NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  FALSE, 0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.121 flags != 0 NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  4, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.122 Callback != NULL NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, 0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "18.123 Closed Socket TCP",        TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "18.124 Closed Socket UDP",        TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "18.125 Not Initialized",          FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, 0,                    FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define WSARecvTableCount (sizeof(WSARecvTable) / sizeof(WSARECV_TABLE))

NETSYNC_TYPE_THREAD  WSARecvTestSessionNt =
{
    1,
    WSARecvTableCount,
    L"xnetapi_nt.dll",
    "WSARecvTestServer"
};

NETSYNC_TYPE_THREAD  WSARecvTestSessionXbox =
{
    1,
    WSARecvTableCount,
    L"xnetapi_xbox.dll",
    "WSARecvTestServer"
};



// Callback routine

VOID CALLBACK WSARecvCompletionRoutine(IN DWORD  dwError, IN DWORD  cbTransferred, IN LPWSAOVERLAPPED  lpOverlapped, IN DWORD  dwFlags) { return; }



VOID
WSARecvTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests WSARecv - Client side

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
    // WSARecvRequest is the request sent to the server
    WSARECV_REQUEST        WSARecvRequest;

    // sSocket is the socket descriptor
    SOCKET                 sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                 nsSocket;

    // iTimeout is the send and receive timeout value for the socket
    int                    iTimeout = 5000;
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
    // timeout is the timeout for select
    timeval                fdstimeout = { 1, 0 };

    // SendBuffer10 is the send buffer
    char                   SendBuffer10[BUFFER_10_LEN + 1];
    // SendBufferLarge is the large send buffer
    char                   SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // RecvBuffer10 is the recv buffer
    char                   RecvBuffer10[BUFFER_10_LEN + 1];
    // RecvBufferLarge is the large recv buffer
    char                   RecvBufferLarge[BUFFER_LARGE_LEN + 1];
    // WSABuf is an array of send data structures
    WSABUF                 WSABuf[3];
    // dwBufferCount is the number of send data structures
    DWORD                  dwBufferCount;
    // nBufferlen is the total size of the data
    int                    nBufferlen;
    // dwFillBuffer is a counter to fill the buffers
    DWORD                  dwFillBuffer;
    // dwBytes is the number of bytes received
    DWORD                  dwBytes;
    // nRecvCount is a counter to enumerate each recv
    int                    nRecvCount;

    // dwFirstTime is the first tick count
    DWORD                  dwFirstTime;
    // dwSecondTime is the second tick count
    DWORD                  dwSecondTime;

    // WSARecvBufferTable is the test buffer table to use for iterative send
    PWSARECVBUFFER_TABLE   WSARecvBufferTable;
    // WSARecvBufferTableCount is the number of elements within the test buffer table
    size_t                 WSARecvBufferTableCount;
    // WSAOverlapped is the overlapped I/O structure
    WSAOVERLAPPED          WSAOverlapped;
    // dwFlags specifies the receive flags
    DWORD                  dwFlags;
    // dwTotalBytes specifies the total bytes
    DWORD                  dwTotalBytes;

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
    sprintf(szFunctionName, "WSARecv v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_WSARecv+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_WSARecv-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = WSARecvTestSessionXbox;
    }
    else {
        NetsyncTypeSession = WSARecvTestSessionNt;
    }

    // Initialize the overlapped I/O structure
    ZeroMemory(&WSAOverlapped, sizeof(WSAOverlapped));
    WSAOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < WSARecvTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, WSARecvTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, WSARecvTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != WSARecvTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, WSARecvTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == WSARecvTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != WSARecvTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == WSARecvTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = WSARecvTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == WSARecvTable[dwTableIndex].bNetsyncConnected)) {
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

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        if (SOCKET_INT_MIN == WSARecvTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == WSARecvTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == WSARecvTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == WSARecvTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == WSARecvTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & WSARecvTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Set the send and receive timeout values to 5 sec
        if ((0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP && WSARecvTable[dwTableIndex].dwSocket))) {
            setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
            setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the socket to non-blocking mode
        if (TRUE == WSARecvTable[dwTableIndex].bNonblock) {
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);
            bNonblocking = TRUE;
        }
        else {
            bNonblocking = FALSE;
        }

        // Bind the socket
        if (TRUE == WSARecvTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Place the socket in the listening state
        if (TRUE == WSARecvTable[dwTableIndex].bListen) {
            listen(sSocket, SOMAXCONN);
        }

        if (0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) {
            WSARecvBufferTable = WSARecvBufferTcpTable;
            WSARecvBufferTableCount = WSARecvBufferTcpTableCount;
        }
        else {
            WSARecvBufferTable = WSARecvBufferUdpTable;
            WSARecvBufferTableCount = WSARecvBufferUdpTableCount;
        }

        if ((TRUE == WSARecvTable[dwTableIndex].bAccept) || (TRUE == WSARecvTable[dwTableIndex].bConnect) || (0 == WSARecvTable[dwTableIndex].iReturnCode)) {
            // Initialize the recv request
            WSARecvRequest.dwMessageId = WSARECV_REQUEST_MSG;
            if (0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) {
                WSARecvRequest.nSocketType = SOCK_STREAM;
            }
            else {
                WSARecvRequest.nSocketType = SOCK_DGRAM;
            }
            WSARecvRequest.Port = CurrentPort;
            WSARecvRequest.bServerAccept = WSARecvTable[dwTableIndex].bConnect;
            WSARecvRequest.bRemoteShutdown = WSARecvTable[dwTableIndex].bRemoteShutdown;
            WSARecvRequest.bRemoteClose = WSARecvTable[dwTableIndex].bRemoteClose;
            if (100 == WSARecvTable[dwTableIndex].nDataBuffers) {
                WSARecvRequest.nDataBuffers = WSARecvTable[dwTableIndex].nDataBuffers * WSARecvBufferTableCount;
            }
            else {
                WSARecvRequest.nDataBuffers = ((0 == WSARecvTable[dwTableIndex].iReturnCode) || (WSAEMSGSIZE == WSARecvTable[dwTableIndex].iLastError)) ? WSARecvTable[dwTableIndex].nDataBuffers : 0;
            }

            WSARecvRequest.nBufferlen = BUFFER_10_LEN;
            if (BUFFER_TCPLARGE == WSARecvTable[dwTableIndex].dwBuffer) {
                WSARecvRequest.nBufferlen = BUFFER_TCPLARGE_LEN;
            }
            else if (BUFFER_UDPLARGE == WSARecvTable[dwTableIndex].dwBuffer) {
                WSARecvRequest.nBufferlen = BUFFER_UDPLARGE_LEN;
            }
            else if ((BUFFER_NULL == WSARecvTable[dwTableIndex].dwBuffer) || ((0 != (SOCKET_UDP & WSARecvTable[dwTableIndex].dwSocket)) && ((BUFFER_NULLZERO == WSARecvTable[dwTableIndex].dwBuffer) || (BUFFER_ZERO == WSARecvTable[dwTableIndex].dwBuffer)))) {
                WSARecvRequest.nBufferlen = 0;
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvRequest), (char *) &WSARecvRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == WSARecvTable[dwTableIndex].bConnect) {
                // Connect the socket
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = NetsyncInAddr;
                remotename.sin_port = htons(CurrentPort);

                connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));

                if (0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&writefds);
                    FD_SET(sSocket, &writefds);
                    select(0, NULL, &writefds, NULL, NULL);
                }
            }
            else if (TRUE == WSARecvTable[dwTableIndex].bAccept) {
                if (0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&readfds);
                    FD_SET(sSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);
                }

                // Accept the socket
                nsSocket = accept(sSocket, NULL, NULL);
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvRequest), (char *) &WSARecvRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == WSARecvTable[dwTableIndex].bRemoteShutdown) {
                if (TRUE == bNonblocking) {
                    Nonblock = 0;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                }

                ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                do {
                    Sleep(SLEEP_ZERO_TIME);
                } while (0 != recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBuffer10, sizeof(RecvBuffer10), 0));

                if (TRUE == bNonblocking) {
                    Nonblock = 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                }
            }

            if (TRUE == WSARecvTable[dwTableIndex].bRemoteClose) {
                if (TRUE == bNonblocking) {
                    Nonblock = 0;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                }

                ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                do {
                    Sleep(SLEEP_ZERO_TIME);
                } while (SOCKET_ERROR != send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBuffer10, 10, 0));

                if (TRUE == bNonblocking) {
                    Nonblock = 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                }
            }
        }

        // Shutdown the connection
        if (TRUE == WSARecvTable[dwTableIndex].bShutdown) {
            shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSARecvTable[dwTableIndex].nShutdown);
        }

        // Zero the receive buffer
        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

        // Initialize the buffers
        sprintf(SendBuffer10, "%05d%05d", 1, 1);
        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
        }

        dwBufferCount = 1;
        if (BUFFER_10 == WSARecvTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = BUFFER_10_LEN;
        }
        else if (BUFFER_NULL == WSARecvTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = NULL;

            nBufferlen = 0;
        }
        else if (BUFFER_NULLZERO == WSARecvTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = 0;
            WSABuf[0].buf = NULL;

            nBufferlen = 0;
        }
        else if (BUFFER_ZERO == WSARecvTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = 0;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = 0;
        }
        else if (BUFFER_ZEROCOUNT == WSARecvTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = 0;
            WSABuf[0].buf = RecvBufferLarge;

            dwBufferCount = 0;
            nBufferlen = 0;
        }
        else if (BUFFER_SMALL == WSARecvTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN - 1;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = BUFFER_10_LEN - 1;
        }
        else if (BUFFER_TCPLARGE == WSARecvTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_TCPLARGE_LEN;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = BUFFER_TCPLARGE_LEN;
        }
        else if (BUFFER_UDPLARGE == WSARecvTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_UDPLARGE_LEN;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = BUFFER_UDPLARGE_LEN;
        }
        else if (BUFFER_UDPTOOLARGE == WSARecvTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_UDPLARGE_LEN + 1;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = 0;
        }
        else if (BUFFER_MULTI == WSARecvTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = RecvBufferLarge;

            WSABuf[1].len = BUFFER_10_LEN;
            WSABuf[1].buf = &RecvBufferLarge[BUFFER_10_LEN];

            WSABuf[2].len = BUFFER_10_LEN;
            WSABuf[2].buf = &RecvBufferLarge[BUFFER_10_LEN + BUFFER_10_LEN];

            dwBufferCount = 3;
            nBufferlen = BUFFER_10_LEN + BUFFER_10_LEN + BUFFER_10_LEN;
        }
        else if (BUFFER_SECOND == WSARecvTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN / 2;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = BUFFER_10_LEN;
        }

        // Initialize the flags
        dwFlags = WSARecvTable[dwTableIndex].dwFlags;

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & WSARecvTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        // Get the current tick count
        dwFirstTime = GetTickCount();

        __try {
            // Call WSARecv
            iReturnCode = WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (BUFFER_NONE != WSARecvTable[dwTableIndex].dwBuffer) ? WSABuf : NULL, (BUFFER_NONE != WSARecvTable[dwTableIndex].dwBuffer) ? dwBufferCount : 0, (TRUE == WSARecvTable[dwTableIndex].bBytesRecv) ? &dwBytes : NULL, (TRUE == WSARecvTable[dwTableIndex].bFlags) ? &dwFlags : NULL, (0 == WSARecvTable[dwTableIndex].nOverlapped) ? NULL : &WSAOverlapped, (FALSE == WSARecvTable[dwTableIndex].bCompletionRoutine) ? NULL : WSARecvCompletionRoutine);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == WSARecvTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "WSARecv RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "WSARecv caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (0 != WSARecvTable[dwTableIndex].nOverlapped) {
                if (-1 != WSARecvTable[dwTableIndex].nDataBuffers) {
                    if (SOCKET_ERROR != iReturnCode) {
                        bTestPassed = FALSE;
                        WSARecvRequest.dwMessageId = WSARECV_CANCEL_MSG;
                        WSARecvRequest.nDataBuffers = 0;
                    }
                    else {
                        // Get the last error code
                        iLastError = WSAGetLastError();

                        if (iLastError != WSA_IO_PENDING) {
                            xLog(hLog, XLL_FAIL, "WSARecv iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, iLastError);
                            bTestPassed = FALSE;
                        }
                        else {
                            if (OVERLAPPED_IO_EVENT == WSARecvTable[dwTableIndex].nOverlapped) {
                                // Wait on the overlapped I/O event
                                WaitForSingleObject(WSAOverlapped.hEvent, INFINITE);
                            }
                            // Get the overlapped I/O result
                            if (FALSE == WSAGetOverlappedResult((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSAOverlapped, &dwBytes, (OVERLAPPED_IO_RESULT == WSARecvTable[dwTableIndex].nOverlapped) ? TRUE : FALSE, &dwFlags)) {
                                xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            }
                            else {
                                iReturnCode = 0;
                            }
                        }
                    }
                }
                else {
                    if (SOCKET_ERROR == iReturnCode) {
                        bTestPassed = FALSE;
                        WSARecvRequest.dwMessageId = WSARECV_CANCEL_MSG;
                        WSARecvRequest.nDataBuffers = 0;
                    }
                    else {
                        dwReturnCode = WaitForSingleObject(WSAOverlapped.hEvent, 0);
                        if (WAIT_OBJECT_0 != dwReturnCode) {
                            xLog(hLog, XLL_FAIL, "WaitForSingleObject - EXPECTED: %u; RECEIVED: %u", WAIT_OBJECT_0, dwReturnCode);
                        }

                        // Get the overlapped I/O result
                        if (FALSE == WSAGetOverlappedResult((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSAOverlapped, &dwBytes, (OVERLAPPED_IO_RESULT == WSARecvTable[dwTableIndex].nOverlapped) ? TRUE : FALSE, &dwFlags)) {
                            xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        }
                        else {
                            iReturnCode = 0;
                        }

                        // Spoof the sleep time
                        Sleep(SLEEP_MEAN_TIME);
                    }
                }
            }
            else if ((TRUE == bNonblocking) && (FALSE == WSARecvTable[dwTableIndex].bRemoteShutdown) && ((0 == WSARecvTable[dwTableIndex].iReturnCode) || (WSAEMSGSIZE == WSARecvTable[dwTableIndex].iLastError))) {
                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "WSARecv returned non-SOCKET_ERROR");
                    bTestPassed = FALSE;
                }
                else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                    xLog(hLog, XLL_FAIL, "WSARecv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
                    bTestPassed = FALSE;
                }
                else {
                    xLog(hLog, XLL_PASS, "WSARecv iLastError - OUT: %u", WSAGetLastError());

                    FD_ZERO(&readfds);
                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);

                    // Call WSARecv
                    iReturnCode = WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL);
                }
            }
            
            // Get the current tick count
            dwSecondTime = GetTickCount();

            if (TRUE == WSARecvTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "WSARecv did not RIP");
            }

            if (TRUE == WSARecvTable[dwTableIndex].bRemoteShutdown) {
                if (SOCKET_ERROR == iReturnCode) {
                    xLog(hLog, XLL_FAIL, "WSARecv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                }
                else if (0 != dwBytes) {
                    xLog(hLog, XLL_FAIL, "WSARecv dwBytes - EXPECTED: %u; RECEIVED: %u", 0, dwBytes);
                }
                else {
                    xLog(hLog, XLL_PASS, "WSARecv succeeded");
                }
            }
            else if (WSAEMSGSIZE == WSARecvTable[dwTableIndex].iLastError) {
                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "WSARecv returned non-SOCKET_ERROR");
                }
                else if (WSAEMSGSIZE != WSAGetLastError()) {
                    xLog(hLog, XLL_FAIL, "WSARecv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEMSGSIZE, WSAGetLastError());
                }
                else {
                    if (MSG_PARTIAL != dwFlags) {
                        xLog(hLog, XLL_FAIL, "flags != MSG_PARTIAL");
                        bTestPassed = FALSE;
                    }

                    dwFlags = 0;
                    iReturnCode = WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL);

                    if (SOCKET_ERROR != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "WSARecv returned non-SOCKET_ERROR");
                        bTestPassed = FALSE;
                    }
                    else if (((TRUE == bNonblocking) && (WSAEWOULDBLOCK != WSAGetLastError())) || ((FALSE == bNonblocking) && (WSAETIMEDOUT != WSAGetLastError()))) {
                        xLog(hLog, XLL_FAIL, "WSARecv iLastError - EXPECTED: %u; RECEIVED: %u", (TRUE == bNonblocking) ? WSAEWOULDBLOCK : WSAETIMEDOUT, WSAGetLastError());
                        bTestPassed = FALSE;
                    }

                    if (TRUE == bTestPassed) {
                        xLog(hLog, XLL_PASS, "WSARecv succeeded");
                    }
                }
            }
            else if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == WSARecvTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != WSARecvTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "WSARecv iLastError - EXPECTED: %u; RECEIVED: %u", WSARecvTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "WSARecv iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSARecv returned SOCKET_ERROR - ec = %u", WSAGetLastError());

                if (1 < WSARecvRequest.nDataBuffers) {
                    // Send the WSARecv cancel
                    WSARecvRequest.dwMessageId = WSARECV_CANCEL_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvRequest), (char *) &WSARecvRequest);
                }
            }
            else if (SOCKET_ERROR == WSARecvTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSARecv returned non-SOCKET_ERROR");
            }
            else {
                dwTotalBytes = dwBytes;

                if (((dwSecondTime - dwFirstTime) < SLEEP_LOW_TIME) || ((dwSecondTime - dwFirstTime) > SLEEP_HIGH_TIME)) {
                    xLog(hLog, XLL_FAIL, "WSARecv nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME, dwSecondTime - dwFirstTime);
                    bTestPassed = FALSE;
                }

                if (0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) {
                    if (TRUE == WSARecvTable[dwTableIndex].bSecondRecv) {
                        if (WSABuf[0].len != dwBytes) {
                            xLog(hLog, XLL_FAIL, "WSARecv return value - EXPECTED: %d; RECEIVED: %d", WSABuf[0].len, dwBytes);
                            bTestPassed = FALSE;
                        }
                        else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, WSABuf[0].len)) {
                            xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                            bTestPassed = FALSE;
                        }

                        WSABuf[0].buf = &RecvBufferLarge[dwTotalBytes];
                        WSABuf[0].len = nBufferlen - dwTotalBytes;
                        iReturnCode = WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL);
                        WSABuf[0].buf = RecvBufferLarge;
                        WSABuf[0].len = BUFFER_10_LEN / 2;

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "WSARecv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            if (WSABuf[0].len != dwBytes) {
                                xLog(hLog, XLL_FAIL, "WSARecv return value - EXPECTED: %d; RECEIVED: %d", WSABuf[0].len, dwBytes);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(&SendBufferLarge[dwTotalBytes], &RecvBufferLarge[dwTotalBytes], WSABuf[0].len)) {
                                xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                bTestPassed = FALSE;
                            }

                            dwTotalBytes += dwBytes;
                        }
                    }

                    if (BUFFER_10_LEN <= nBufferlen) {
                        do {
                            FD_ZERO(&readfds);
                            FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                            iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                            if (1 == iReturnCode) {
                                WSABuf[0].buf = &RecvBufferLarge[dwTotalBytes];
                                WSABuf[0].len = nBufferlen - dwTotalBytes;
                                if (SOCKET_ERROR != WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL)) {
                                    dwTotalBytes += dwBytes;
                                }
                                WSABuf[0].buf = RecvBufferLarge;
                                WSABuf[0].len = nBufferlen;
                            }
                        } while (0 != iReturnCode);
                    }
                }

                if (nBufferlen != dwTotalBytes) {
                    xLog(hLog, XLL_FAIL, "WSARecv return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwTotalBytes);
                    bTestPassed = FALSE;
                }
                else {
                    if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                        xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                        bTestPassed = FALSE;
                    }
                }

                if (3 == WSARecvTable[dwTableIndex].nDataBuffers) {
                    // Zero the receive buffer
                    ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvRequest), (char *) &WSARecvRequest);

                    if (WSARECV_CANCEL_MSG != WSARecvRequest.dwMessageId) {
                        // Wait for the read complete
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);

                        // Switch the blocking mode
                        Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                        ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                        bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                        iReturnCode = WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL);

                        if (TRUE == bNonblocking) {
                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "WSARecv returned non-SOCKET_ERROR");
                                bTestPassed = FALSE;
                            }
                            else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "WSARecv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "WSARecv iLastError - OUT: %u", WSAGetLastError());

                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                select(0, &readfds, NULL, NULL, NULL);

                                // Call WSARecv
                                iReturnCode = WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL);
                            }
                        }

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                            WSARecvRequest.dwMessageId = WSARECV_CANCEL_MSG;
                        }
                        else {
                            dwTotalBytes = dwBytes;

                            if (0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) {
                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                    if (1 == iReturnCode) {
                                        WSABuf[0].buf = &RecvBufferLarge[dwTotalBytes];
                                        WSABuf[0].len = nBufferlen - dwTotalBytes;
                                        if (SOCKET_ERROR != WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL)) {
                                            dwTotalBytes += dwBytes;
                                        }
                                        WSABuf[0].buf = RecvBufferLarge;
                                        WSABuf[0].len = nBufferlen;
                                    }
                                } while (0 != iReturnCode);
                            }

                            if (nBufferlen != dwTotalBytes) {
                                xLog(hLog, XLL_FAIL, "WSARecv return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwTotalBytes);
                                bTestPassed = FALSE;
                            }
                            else {
                                if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                    bTestPassed = FALSE;
                                }
                            }

                            WSARecvRequest.dwMessageId = WSARECV_REQUEST_MSG;
                        }

                        // Zero the receive buffer
                        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                        // Initialize the buffers
                        sprintf(SendBuffer10, "%05d%05d", 3, 3);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                        }

                        // Send the read request
                        NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvRequest), (char *) &WSARecvRequest);

                        if (WSARECV_CANCEL_MSG != WSARecvRequest.dwMessageId) {
                            // Wait for the read complete
                            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                            NetsyncFreeMessage(pMessage);

                            // Switch the blocking mode
                            Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                            bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                            // Call WSARecv
                            iReturnCode = WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL);

                            if (TRUE == bNonblocking) {
                                if (SOCKET_ERROR != iReturnCode) {
                                    xLog(hLog, XLL_FAIL, "WSARecv returned non-SOCKET_ERROR");
                                    bTestPassed = FALSE;
                                }
                                else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                                    xLog(hLog, XLL_FAIL, "WSARecv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
                                    bTestPassed = FALSE;
                                }
                                else {
                                    xLog(hLog, XLL_PASS, "WSARecv iLastError - OUT: %u", WSAGetLastError());

                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                    select(0, &readfds, NULL, NULL, NULL);

                                    // Call WSARecv
                                    iReturnCode = WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL);
                                }
                            }

                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                                WSARecvRequest.dwMessageId = WSARECV_CANCEL_MSG;
                            }
                            else {
                                dwTotalBytes = dwBytes;

                                if (0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) {
                                    do {
                                        FD_ZERO(&readfds);
                                        FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                        iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                        if (1 == iReturnCode) {
                                            WSABuf[0].buf = &RecvBufferLarge[dwTotalBytes];
                                            WSABuf[0].len = nBufferlen - dwTotalBytes;
                                            if (SOCKET_ERROR != WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL)) {
                                                dwTotalBytes += dwBytes;
                                            }
                                            WSABuf[0].buf = RecvBufferLarge;
                                            WSABuf[0].len = nBufferlen;
                                        }
                                    } while (0 != iReturnCode);
                                }

                                if (nBufferlen != dwTotalBytes) {
                                    xLog(hLog, XLL_FAIL, "WSARecv return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwTotalBytes);
                                    bTestPassed = FALSE;
                                }
                                else {
                                    if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                                        xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                        bTestPassed = FALSE;
                                    }
                                }
                            }
                        }
                    }
                }
                else if (100 == WSARecvTable[dwTableIndex].nDataBuffers) {
                    for (nRecvCount = 0; nRecvCount < (int) (WSARecvTable[dwTableIndex].nDataBuffers * WSARecvBufferTableCount); nRecvCount++) {
                        // Zero the receive buffer
                        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                        // Initialize the buffers
                        sprintf(SendBuffer10, "%05d%05d", 2 + nRecvCount, 2 + nRecvCount);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                        }

                        xLog(hLog, XLL_INFO, "Iteration %d", nRecvCount);

                        dwBufferCount = 1;
                        if (BUFFER_10 == WSARecvTable[dwTableIndex].dwBuffer) {
                            WSABuf[0].len = BUFFER_10_LEN;
                            WSABuf[0].buf = RecvBufferLarge;

                            nBufferlen = BUFFER_10_LEN;
                        }
                        else if (BUFFER_NULLZERO == WSARecvTable[dwTableIndex].dwBuffer) {
                            WSABuf[0].len = 0;
                            WSABuf[0].buf = NULL;

                            nBufferlen = 0;
                        }
                        else if (BUFFER_TCPLARGE == WSARecvTable[dwTableIndex].dwBuffer) {
                            WSABuf[0].len = BUFFER_TCPLARGE_LEN;
                            WSABuf[0].buf = RecvBufferLarge;

                            nBufferlen = BUFFER_TCPLARGE_LEN;
                        }
                        else if (BUFFER_UDPLARGE == WSARecvTable[dwTableIndex].dwBuffer) {
                            WSABuf[0].len = BUFFER_UDPLARGE_LEN;
                            WSABuf[0].buf = RecvBufferLarge;

                            nBufferlen = BUFFER_UDPLARGE_LEN;
                        }

                        // Send the WSARecv request
                        WSARecvRequest.nBufferlen = nBufferlen;
                        NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvRequest), (char *) &WSARecvRequest);

                        if (WSARECV_CANCEL_MSG == WSARecvRequest.dwMessageId) {
                            break;
                        }

                        // Wait for the read complete
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);

                        if ((0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) && (0 == nBufferlen)) {
                            iReturnCode = 0;
                            dwBytes = 0;
                        }
                        else {
                            if (TRUE == bNonblocking) {
                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                select(0, &readfds, NULL, NULL, NULL);
                            }

                            // Call WSARecv
                            iReturnCode = WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL);
                        }

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "WSARecv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            WSARecvRequest.dwMessageId = WSARECV_CANCEL_MSG;
                            bTestPassed = FALSE;
                        }
                        else {
                            dwTotalBytes = dwBytes;

                            if (0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) {
                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                    if (1 == iReturnCode) {
                                        WSABuf[0].buf = &RecvBufferLarge[dwTotalBytes];
                                        WSABuf[0].len = nBufferlen - dwTotalBytes;
                                        if (SOCKET_ERROR != WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL)) {
                                            dwTotalBytes += dwBytes;
                                        }
                                        WSABuf[0].buf = RecvBufferLarge;
                                        WSABuf[0].len = nBufferlen;
                                    }
                                } while (0 != iReturnCode);
                            }

                            if (nBufferlen != dwTotalBytes) {
                                xLog(hLog, XLL_FAIL, "WSARecv return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwTotalBytes);
                                bTestPassed = FALSE;
                            }
                            else {
                                if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "WSARecv succeeded");
                }
            }
        }

        // Switch the blocking mode
        if (WSARecvTable[dwTableIndex].bNonblock != bNonblocking) {
            Nonblock = (TRUE == WSARecvTable[dwTableIndex].bNonblock) ? 1 : 0;
            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
            bNonblocking = WSARecvTable[dwTableIndex].bNonblock;
        }

        if ((TRUE == WSARecvTable[dwTableIndex].bAccept) || (TRUE == WSARecvTable[dwTableIndex].bConnect) || (0 == WSARecvTable[dwTableIndex].iReturnCode)) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvRequest), (char *) &WSARecvRequest);
        }

        // Close the sockets
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & WSARecvTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & WSARecvTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & WSARecvTable[dwTableIndex].dwSocket))) {
                shutdown(sSocket, SD_BOTH);
                closesocket(sSocket);
            }
        }

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

    CloseHandle(WSAOverlapped.hEvent);

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
WSARecvTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests recv - Server side

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
    u_long            FromInAddr;
    // dwMessageType is the type of received message
    DWORD             dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD             dwMessageSize;
    // pMessage is a pointer to the received message
    char              *pMessage;
    // WSARecvRequest is the request
    WSARECV_REQUEST   WSARecvRequest;
    // WSARecvComplete is the result
    WSARECV_COMPLETE  WSARecvComplete;

    // sSocket is the socket descriptor
    SOCKET            sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET            nsSocket;

    // iTimeout is the send and receive timeout value for the socket
    int               iTimeout = 5000;
    // bNagle indicates if Nagle is enabled
    BOOL              bNagle = FALSE;

    // localname is the local address
    SOCKADDR_IN       localname;
    // remotename is the remote address
    SOCKADDR_IN       remotename;

    // SendBuffer10 is the send buffer
    char              SendBuffer10[BUFFER_10_LEN + 1];
    // SendBufferLarge is the large send buffer
    char              SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // dwFillBuffer is a counter to fill the buffers
    DWORD             dwFillBuffer;
    // nSendCount is a counter to enumerate each send
    int               nSendCount;



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
        CopyMemory(&WSARecvRequest, pMessage, sizeof(WSARecvRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, WSARecvRequest.nSocketType, 0);

        // Set the send and receive timeout values to 5 sec
        setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));

        if (SOCK_STREAM == WSARecvRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(WSARecvRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if ((SOCK_STREAM == WSARecvRequest.nSocketType) && (TRUE == WSARecvRequest.bServerAccept)) {
            // Place the socket in listening mode
            listen(sSocket, SOMAXCONN);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(WSARecvRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        WSARecvComplete.dwMessageId = WSARECV_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSARecvComplete), (char *) &WSARecvComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if ((SOCK_STREAM == WSARecvRequest.nSocketType) && (TRUE == WSARecvRequest.bServerAccept)) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        if (TRUE == WSARecvRequest.bRemoteShutdown) {
            // Shutdown the sockets
            if (INVALID_SOCKET != nsSocket) {
                shutdown(nsSocket, SD_BOTH);
            }

            if (INVALID_SOCKET != sSocket) {
                shutdown(sSocket, SD_BOTH);
            }
        }

        if (TRUE == WSARecvRequest.bRemoteClose) {
            // Close the sockets
            if (INVALID_SOCKET != nsSocket) {
                shutdown(nsSocket, SD_BOTH);
                closesocket(nsSocket);
                nsSocket = INVALID_SOCKET;
            }

            if (INVALID_SOCKET != sSocket) {
                shutdown(sSocket, SD_BOTH);
                closesocket(sSocket);
                sSocket = INVALID_SOCKET;
            }
        }

        if (-1 == WSARecvRequest.nDataBuffers) {
            // Initialize the buffer
            sprintf(SendBuffer10, "%05d%05d", 1, 1);
            for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
            }

            // Send the buffer
            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, WSARecvRequest.nBufferlen, 0);
        }

        // Send the complete
        WSARecvComplete.dwMessageId = WSARECV_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSARecvComplete), (char *) &WSARecvComplete);

        if (0 < WSARecvRequest.nDataBuffers) {
            // Sleep
            Sleep(SLEEP_MIDLOW_TIME);

            // Initialize the buffer
            sprintf(SendBuffer10, "%05d%05d", 1, 1);
            for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
            }

            // Send the buffer
            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, WSARecvRequest.nBufferlen, 0);

            if (3 == WSARecvRequest.nDataBuffers) {
                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                
                if (WSARECV_CANCEL_MSG != ((PWSARECV_REQUEST) pMessage)->dwMessageId) {
                    // Send the complete
                    WSARecvComplete.dwMessageId = WSARECV_COMPLETE_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSARecvComplete), (char *) &WSARecvComplete);

                    // Sleep
                    Sleep(SLEEP_MIDLOW_TIME);

                    // Initialize the buffer
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                    }

                    // Send the buffer
                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, WSARecvRequest.nBufferlen, 0);

                    NetsyncFreeMessage(pMessage);

                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                    if (WSARECV_CANCEL_MSG != ((PWSARECV_REQUEST) pMessage)->dwMessageId) {
                        // Send the complete
                        WSARecvComplete.dwMessageId = WSARECV_COMPLETE_MSG;
                        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSARecvComplete), (char *) &WSARecvComplete);

                        // Sleep
                        Sleep(SLEEP_MIDLOW_TIME);

                        // Initialize the buffer
                        sprintf(SendBuffer10, "%05d%05d", 3, 3);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                        }

                        // Send the buffer
                        send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, WSARecvRequest.nBufferlen, 0);
                    }
                }

                NetsyncFreeMessage(pMessage);
            }
            else if (100 <= WSARecvRequest.nDataBuffers) {
                for (nSendCount = 0; nSendCount < WSARecvRequest.nDataBuffers; nSendCount++) {
                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                    if (WSARECV_CANCEL_MSG == ((PWSARECV_REQUEST) pMessage)->dwMessageId) {
                        NetsyncFreeMessage(pMessage);
                        break;
                    }

                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 2 + nSendCount, 2 + nSendCount);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                    }

                    // Send the buffer
                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, ((PWSARECV_REQUEST) pMessage)->nBufferlen, 0);

                    NetsyncFreeMessage(pMessage);

                    // Send the complete
                    WSARecvComplete.dwMessageId = WSARECV_COMPLETE_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSARecvComplete), (char *) &WSARecvComplete);
                }
            }
        }

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
