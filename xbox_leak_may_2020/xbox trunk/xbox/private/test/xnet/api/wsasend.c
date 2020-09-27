/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  wsasend.c

Abstract:

  This modules tests wsasend

Author:

  Steven Kehrli (steveke) 28-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// WSASend messages

#define WSASEND_REQUEST_MSG   NETSYNC_MSG_USER + 140 + 1
#define WSASEND_CANCEL_MSG    NETSYNC_MSG_USER + 140 + 2
#define WSASEND_COMPLETE_MSG  NETSYNC_MSG_USER + 140 + 3

typedef struct _WSASEND_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  Port;
    BOOL     bServerAccept;
    BOOL     bFillQueue;
    INT      nQueueLen;
    BOOL     bRemoteClose;
    int      nDataBuffers;
    int      nBufferlen;
} WSASEND_REQUEST, *PWSASEND_REQUEST;

typedef struct _WSASEND_COMPLETE {
    DWORD    dwMessageId;
} WSASEND_COMPLETE, *PWSASEND_COMPLETE;

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
#define BUFFER_TCPMULTI      10
#define BUFFER_UDPMULTI      11
#define BUFFER_NONE          12

#define OVERLAPPED_IO_RESULT  1
#define OVERLAPPED_IO_EVENT   2



typedef struct WSASENDBUFFER_TABLE {
    DWORD  dwBuffer;
} WSASENDBUFFER_TABLE, *PWSASENDBUFFER_TABLE;

WSASENDBUFFER_TABLE WSASendBufferTcpTable[] = { { BUFFER_10        },
                                                { BUFFER_NULLZERO  },
                                                { BUFFER_TCPLARGE  },
                                                { BUFFER_TCPMULTI  } };

#define WSASendBufferTcpTableCount  (sizeof(WSASendBufferTcpTable) / sizeof(WSASENDBUFFER_TABLE));

WSASENDBUFFER_TABLE WSASendBufferUdpTable[] = { { BUFFER_10        },
                                                { BUFFER_NULLZERO  },
                                                { BUFFER_UDPLARGE  },
                                                { BUFFER_UDPMULTI  } };

#define WSASendBufferUdpTableCount  (sizeof(WSASendBufferUdpTable) / sizeof(WSASENDBUFFER_TABLE));



typedef struct WSASEND_TABLE {
    CHAR           szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL           bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL           bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD          dwSocket;                                // dwSocket indicates the socket to be created
    BOOL           bNonblock;                               // bNonblock indicates if the socket is to be set as non-blocking for the send
    BOOL           bBind;                                   // bBind indicates if the socket is to be bound
    BOOL           bListen;                                 // bListen indicates if the socket is to be placed in the listening state
    BOOL           bAccept;                                 // bAccept indicates if the socket is accepted
    BOOL           bConnect;                                // bConnect indicates if the socket is connected
    BOOL           bFillQueue;                              // bFillQueue indicates if the buffer should be filled
    BOOL           bRemoteClose;                            // bRemoteClose indicates if the remote closes the connection
    BOOL           bShutdown;                               // bShutdown indicates if the connection is shutdown
    int            nShutdown;                               // nShutdown indicates how the connection is shutdown
    int            nDataBuffers;                            // nDataBuffers indicates the number of data buffers to be sent
    DWORD          dwBuffer;                                // dwBuffer specifies the send data structure
    int            nFlags;                                  // nFlags specifies the WSASend flags
    int            nOverlapped;                             // nOverlapped specifies if overlapped I/O is used
    BOOL           bCompletionRoutine;                      // bCompletionRoutine specifies if completion routine is used
    int            iReturnCode;                             // iReturnCode is the return code of WSASend
    int            iLastError;                              // iLastError is the error code if the operation failed
    BOOL           bRIP;                                    // Specifies a RIP test case
} WSASEND_TABLE, *PWSASEND_TABLE;

static WSASEND_TABLE WSASendTable[] =
{
    { "14.1 Not Initialized",            FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "14.2 s = INT_MIN",                TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "14.3 s = -1",                     TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "14.4 s = 0",                      TRUE,  TRUE,  SOCKET_ZERO,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "14.5 s = INT_MAX",                TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "14.6 s = INVALID_SOCKET",         TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "14.7 Not Bound TCP",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "14.8 Listening TCP",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "14.9 Accepted TCP",               TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.10 Connected TCP",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.11 Fill Queue TCP",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.12 Fill Queue I/O TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,          0, OVERLAPPED_IO_RESULT, FALSE, 0,            0,                 FALSE },
    { "14.13 Fill Queue Event TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,          0, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "14.14 Event Event TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,          0, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "14.15 SD_RECEIVE Accept TCP",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.16 SD_SEND Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "14.17 SD_BOTH Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "14.18 SD_RECEIVE Connect TCP",    TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.19 SD_SEND Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "14.20 SD_BOTH Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "14.21 Close Accept TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "14.22 Close Connect TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "14.23 NULL Buffer TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,        0, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.24 NULL 0 Bufferlen TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,    0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.25 0 Bufferlen TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,        0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.26 Small Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,       0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.27 Exact Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.28 Large Buffer TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_TCPLARGE,    0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.29 Multi Buffer TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_TCPMULTI,    0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.30 Switch Send TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.31 Iterative Send TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.32 NULL Buffer Ptr TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.33 0 Buffer Count TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT,   0, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.34 flags != 0 TCP",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          4, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.35 Callback != NULL TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.36 Not Bound NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "14.37 Listening NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "14.38 Accepted NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.39 Connected NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.40 Fill Queue NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.41 Fill Queue I/O NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,          0, OVERLAPPED_IO_RESULT, FALSE, 0,            0,                 FALSE },
    { "14.42 Fill Queue Event NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,          0, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "14.43 Event Event NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,          0, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "14.44 SD_RECEIVE Accept NB TCP",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.45 SD_SEND Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "14.46 SD_BOTH Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "14.47 SD_RECEIVE Connect NB TCP", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.48 SD_SEND Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "14.49 SD_BOTH Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "14.50 Close Accept NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "14.51 Close Connect NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "14.52 NULL Buffer NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,        0, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.53 NULL 0 Bufferlen NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,    0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.54 0 Bufferlen NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,        0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.55 Small Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,       0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.56 Exact Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.57 Large Buffer NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_TCPLARGE,    0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.58 Multi Buffer NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_TCPMULTI,    0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.59 Switch Send NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.60 Iterative Send NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.61 NULL Buffer Ptr NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.62 0 Buffer Count NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT,   0, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.63 flags != 0 NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          4, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.64 Callback != NULL NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.65 Not Bound UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "14.66 Not Connected UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "14.67 Connected UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.68 Event Event UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,          0, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "14.69 SD_RECEIVE Conn UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.70 SD_SEND Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "14.71 SD_BOTH Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "14.72 NULL Buffer UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,        0, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.73 NULL 0 Bufferlen UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,    0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.74 0 Bufferlen UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,        0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.75 Small Bufferlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,       0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.76 Exact Bufferlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.77 Large Buffer UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPLARGE,    0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.78 Too Large Buffer UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPTOOLARGE, 0, 0,                    FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "14.79 Multi Buffer UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPMULTI,    0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.80 Switch Send UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.81 Iterative Send UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.82 NULL Buffer Ptr UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.83 0 Buffer Count UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT,   0, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.84 flags != 0 UDP",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          4, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.85 Callback != NULL UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.86 Not Bound NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "14.87 Not Connected NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "14.88 Connected NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.89 Event Event NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,          0, OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "14.90 SD_RECEIVE Conn NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.91 SD_SEND Conn NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "14.92 SD_BOTH Conn NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,          0, 0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "14.93 NULL Buffer NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,        0, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.94 NULL 0 Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,    0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.95 0 Bufferlen NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,        0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.96 Small Bufferlen NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,       0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.97 Exact Bufferlen NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.98 Large Buffer NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPLARGE,    0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.99 Too Large Buffer NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPTOOLARGE, 0, 0,                    FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "14.100 Multi Buffer NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPMULTI,    0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.101 Switch Send NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.102 Iterative Send NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,          0, 0,                    FALSE, 0,            0,                 FALSE },
    { "14.103 NULL Buffer Ptr NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.104 0 Buffer Count NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT,   0, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.105 flags != 0 NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          4, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.106 Callback != NULL NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, 0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "14.107 Closed Socket TCP",        TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "14.108 Closed Socket UDP",        TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "14.109 Not Initialized",          FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, 0,                    FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define WSASendTableCount (sizeof(WSASendTable) / sizeof(WSASEND_TABLE))

NETSYNC_TYPE_THREAD  WSASendTestSessionNt =
{
    1,
    WSASendTableCount,
    L"xnetapi_nt.dll",
    "WSASendTestServer"
};

NETSYNC_TYPE_THREAD  WSASendTestSessionXbox =
{
    1,
    WSASendTableCount,
    L"xnetapi_xbox.dll",
    "WSASendTestServer"
};



// Callback routine

VOID CALLBACK WSASendCompletionRoutine(IN DWORD  dwError, IN DWORD  cbTransferred, IN LPWSAOVERLAPPED  lpOverlapped, IN DWORD  dwFlags) { return; }



VOID
WSASendTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests WSASend - Client side

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
    // WSASendRequest is the request sent to the server
    WSASEND_REQUEST        WSASendRequest;
    
    // sSocket is the socket descriptor
    SOCKET                 sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                 nsSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD                  dwBufferSize = 1;
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
    // dwBytes is the number of bytes sent
    DWORD                  dwBytes;
    // nBytes is the number of bytes received
    int                    nBytes;
    // nSendCount is a counter to enumerate each send/recv
    int                    nSendCount;

    // WSASendBufferTable is the test buffer table to use for iterative send
    PWSASENDBUFFER_TABLE   WSASendBufferTable;
    // WSASendBufferTableCount is the number of elements within the test buffer table
    size_t                 WSASendBufferTableCount;
    // WSAOverlapped is the overlapped I/O structure
    WSAOVERLAPPED          WSAOverlapped;
    // dwFlags is the flags
    DWORD                  dwFlags;

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
    sprintf(szFunctionName, "WSASend v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_WSASend+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_WSASend-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = WSASendTestSessionXbox;
    }
    else {
        NetsyncTypeSession = WSASendTestSessionNt;
    }

    // Initialize the overlapped I/O structure
    ZeroMemory(&WSAOverlapped, sizeof(WSAOverlapped));
    WSAOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < WSASendTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, WSASendTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, WSASendTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != WSASendTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, WSASendTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == WSASendTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != WSASendTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == WSASendTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = WSASendTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == WSASendTable[dwTableIndex].bNetsyncConnected)) {
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
        if (SOCKET_INT_MIN == WSASendTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == WSASendTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == WSASendTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == WSASendTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == WSASendTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & WSASendTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & WSASendTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        if (TRUE == WSASendTable[dwTableIndex].bFillQueue) {
            // Set the buffer size
            setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
            setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
        }

        // Set the send and receive timeout values to 5 sec
        if ((0 != (SOCKET_TCP & WSASendTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & WSASendTable[dwTableIndex].dwSocket))) {
            setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
            setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & WSASendTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the socket to non-blocking mode
        if (TRUE == WSASendTable[dwTableIndex].bNonblock) {
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);
            bNonblocking = TRUE;
        }
        else {
            bNonblocking = FALSE;
        }

        // Bind the socket
        if (TRUE == WSASendTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Place the socket in the listening state
        if (TRUE == WSASendTable[dwTableIndex].bListen) {
            listen(sSocket, SOMAXCONN);
        }

        if (0 != (SOCKET_TCP & WSASendTable[dwTableIndex].dwSocket)) {
            WSASendBufferTable = WSASendBufferTcpTable;
            WSASendBufferTableCount = WSASendBufferTcpTableCount;
        }
        else {
            WSASendBufferTable = WSASendBufferUdpTable;
            WSASendBufferTableCount = WSASendBufferUdpTableCount;
        }

        if ((TRUE == WSASendTable[dwTableIndex].bAccept) || (TRUE == WSASendTable[dwTableIndex].bConnect)) {
            // Initialize the send request
            WSASendRequest.dwMessageId = WSASEND_REQUEST_MSG;
            if (0 != (SOCKET_TCP & WSASendTable[dwTableIndex].dwSocket)) {
                WSASendRequest.nSocketType = SOCK_STREAM;
            }
            else {
                WSASendRequest.nSocketType = SOCK_DGRAM;
            }
            WSASendRequest.Port = CurrentPort;
            WSASendRequest.bServerAccept = WSASendTable[dwTableIndex].bConnect;
            WSASendRequest.bFillQueue = WSASendTable[dwTableIndex].bFillQueue;
            WSASendRequest.nQueueLen = 0;
            WSASendRequest.bRemoteClose = WSASendTable[dwTableIndex].bRemoteClose;
            if (100 == WSASendTable[dwTableIndex].nDataBuffers) {
                WSASendRequest.nDataBuffers = WSASendTable[dwTableIndex].nDataBuffers * WSASendBufferTableCount;
            }
            else {
                WSASendRequest.nDataBuffers = (0 == WSASendTable[dwTableIndex].iReturnCode) ? WSASendTable[dwTableIndex].nDataBuffers : 0;
            }
            if (BUFFER_10 == WSASendTable[dwTableIndex].dwBuffer) {
                WSASendRequest.nBufferlen = BUFFER_10_LEN;
            }
            else if (BUFFER_SMALL == WSASendTable[dwTableIndex].dwBuffer) {
                WSASendRequest.nBufferlen = BUFFER_10_LEN - 1;
            }
            else if (BUFFER_TCPLARGE == WSASendTable[dwTableIndex].dwBuffer) {
                WSASendRequest.nBufferlen = BUFFER_TCPLARGE_LEN;
            }
            else if (BUFFER_UDPLARGE == WSASendTable[dwTableIndex].dwBuffer) {
                WSASendRequest.nBufferlen = BUFFER_UDPLARGE_LEN;
            }
            else if (BUFFER_UDPTOOLARGE == WSASendTable[dwTableIndex].dwBuffer) {
                WSASendRequest.nBufferlen = BUFFER_UDPLARGE_LEN + 1;
            }
            else if (BUFFER_TCPMULTI == WSASendTable[dwTableIndex].dwBuffer) {
                WSASendRequest.nBufferlen = BUFFER_TCPLARGE_LEN;
            }
            else if (BUFFER_UDPMULTI == WSASendTable[dwTableIndex].dwBuffer) {
                WSASendRequest.nBufferlen = BUFFER_UDPLARGE_LEN;
            }
            else {
                WSASendRequest.nBufferlen = 0;
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendRequest), (char *) &WSASendRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == WSASendTable[dwTableIndex].bConnect) {
                // Connect the socket
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = NetsyncInAddr;
                remotename.sin_port = htons(CurrentPort);

                connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));

                if (0 != (SOCKET_TCP & WSASendTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&writefds);
                    FD_SET(sSocket, &writefds);
                    select(0, NULL, &writefds, NULL, NULL);
                }
            }
            else {
                if (0 != (SOCKET_TCP & WSASendTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&readfds);
                    FD_SET(sSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);
                }

                // Accept the socket
                nsSocket = accept(sSocket, NULL, NULL);
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendRequest), (char *) &WSASendRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == WSASendTable[dwTableIndex].bFillQueue) {
                if (FALSE == bNonblocking) {
                    Nonblock = 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                }

                // Fill the queue
                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                while (SOCKET_ERROR != send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0)) {
                    WSASendRequest.nQueueLen++;
                    Sleep(SLEEP_ZERO_TIME);
                }
            }

            if (TRUE == WSASendTable[dwTableIndex].bRemoteClose) {
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
        if (TRUE == WSASendTable[dwTableIndex].bShutdown) {
            shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSASendTable[dwTableIndex].nShutdown);
        }

        // Initialize the buffers
        sprintf(SendBuffer10, "%05d%05d", 1, 1);
        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
        }

        dwBufferCount = 1;
        if (BUFFER_10 == WSASendTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = SendBuffer10;

            nBufferlen = BUFFER_10_LEN;
        }
        else if (BUFFER_NULL == WSASendTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = NULL;

            nBufferlen = 0;
        }
        else if (BUFFER_NULLZERO == WSASendTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = 0;
            WSABuf[0].buf = NULL;

            nBufferlen = 0;
        }
        else if (BUFFER_ZERO == WSASendTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = 0;
            WSABuf[0].buf = SendBuffer10;

            nBufferlen = 0;
        }
        else if (BUFFER_ZEROCOUNT == WSASendTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = 0;
            WSABuf[0].buf = SendBuffer10;

            dwBufferCount = 0;
            nBufferlen = 0;
        }
        else if (BUFFER_SMALL == WSASendTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN - 1;
            WSABuf[0].buf = SendBuffer10;

            nBufferlen = BUFFER_10_LEN - 1;
        }
        else if (BUFFER_TCPLARGE == WSASendTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_TCPLARGE_LEN;
            WSABuf[0].buf = SendBufferLarge;

            nBufferlen = BUFFER_TCPLARGE_LEN;
        }
        else if (BUFFER_UDPLARGE == WSASendTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_UDPLARGE_LEN;
            WSABuf[0].buf = SendBufferLarge;

            nBufferlen = BUFFER_UDPLARGE_LEN;
        }
        else if (BUFFER_UDPTOOLARGE == WSASendTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_UDPLARGE_LEN + 1;
            WSABuf[0].buf = SendBufferLarge;

            nBufferlen = 0;
        }
        else if (BUFFER_TCPMULTI == WSASendTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = SendBufferLarge;

            WSABuf[1].len = 0;
            WSABuf[1].buf = NULL;

            WSABuf[2].len = BUFFER_TCPLARGE_LEN - BUFFER_10_LEN;
            WSABuf[2].buf = &SendBufferLarge[BUFFER_10_LEN];

            dwBufferCount = 3;
            nBufferlen = BUFFER_TCPLARGE_LEN;
        }
        else if (BUFFER_UDPMULTI == WSASendTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = SendBufferLarge;

            WSABuf[1].len = 0;
            WSABuf[1].buf = NULL;

            WSABuf[2].len = BUFFER_UDPLARGE_LEN - BUFFER_10_LEN;
            WSABuf[2].buf = &SendBufferLarge[BUFFER_10_LEN];

            dwBufferCount = 3;
            nBufferlen = BUFFER_UDPLARGE_LEN;
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & WSASendTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        __try {
            // Call WSASend
            iReturnCode = WSASend((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (BUFFER_NONE != WSASendTable[dwTableIndex].dwBuffer) ? WSABuf : NULL, (BUFFER_NONE != WSASendTable[dwTableIndex].dwBuffer) ? dwBufferCount : 0, &dwBytes, WSASendTable[dwTableIndex].nFlags, (0 == WSASendTable[dwTableIndex].nOverlapped) ? NULL : &WSAOverlapped, (FALSE == WSASendTable[dwTableIndex].bCompletionRoutine) ? NULL : WSASendCompletionRoutine);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == WSASendTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "WSASend RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "WSASend caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == WSASendTable[dwTableIndex].bFillQueue) {
                if (0 != WSASendTable[dwTableIndex].nOverlapped) {
                    if (SOCKET_ERROR != iReturnCode) {
                        bTestPassed = FALSE;
                        WSASendRequest.dwMessageId = WSASEND_CANCEL_MSG;
                        WSASendRequest.nDataBuffers = 0;
                    }
                    else {
                        // Get the last error code
                        iLastError = WSAGetLastError();

                        if (iLastError != WSA_IO_PENDING) {
                            xLog(hLog, XLL_FAIL, "WSASend iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, iLastError);
                            bTestPassed = FALSE;
                            WSASendRequest.dwMessageId = WSASEND_CANCEL_MSG;
                        }
                    }

                    // Send the send cancel
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendRequest), (char *) &WSASendRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    if (FALSE != bTestPassed) {
                        if (OVERLAPPED_IO_EVENT == WSASendTable[dwTableIndex].nOverlapped) {
                            // Wait on the overlapped I/O event
                            WaitForSingleObject(WSAOverlapped.hEvent, INFINITE);
                        }

                        // Get the overlapped I/O result
                        if (FALSE == WSAGetOverlappedResult((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSAOverlapped, &dwBytes, (OVERLAPPED_IO_RESULT == WSASendTable[dwTableIndex].nOverlapped) ? TRUE : FALSE, &dwFlags)) {
                            xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult returned FALSE - ec = %u", WSAGetLastError());
                        }
                        else {
                            iReturnCode = 0;
                        }
                    }
                }
                else {
                    if (SOCKET_ERROR != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "WSASend returned non-SOCKET_ERROR");
                    }
                    else {
                        // Get the last error code
                        iLastError = WSAGetLastError();

                        if (iLastError != WSAEWOULDBLOCK) {
                            xLog(hLog, XLL_FAIL, "WSASend iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                        }
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendRequest), (char *) &WSASendRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    if (FALSE == bNonblocking) {
                        Nonblock = 0;
                        ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                    }
                    else {
                        FD_ZERO(&writefds);
                        FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &writefds);
                        select(0, NULL, &writefds, NULL, NULL);
                    }

                    // Call WSASend
                    iReturnCode = WSASend((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, dwBufferCount, &dwBytes, 0, NULL, NULL);
                }
            }

            if (-1 == WSASendTable[dwTableIndex].nDataBuffers) {
                if (SOCKET_ERROR == iReturnCode) {
                    bTestPassed = FALSE;
                    WSASendRequest.dwMessageId = WSASEND_CANCEL_MSG;
                    WSASendRequest.nDataBuffers = 0;
                }
                else {
                    dwReturnCode = WaitForSingleObject(WSAOverlapped.hEvent, 0);
                    if (WAIT_OBJECT_0 != dwReturnCode) {
                        xLog(hLog, XLL_FAIL, "WaitForSingleObject - EXPECTED: %u; RECEIVED: %u", WAIT_OBJECT_0, dwReturnCode);
                    }

                    // Get the overlapped I/O result
                    if (FALSE == WSAGetOverlappedResult((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSAOverlapped, &dwBytes, (OVERLAPPED_IO_RESULT == WSASendTable[dwTableIndex].nOverlapped) ? TRUE : FALSE, &dwFlags)) {
                        xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                    }
                    else {
                        iReturnCode = 0;
                    }
                }
            }

            if (TRUE == WSASendTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "WSASend did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == WSASendTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != WSASendTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "WSASend iLastError - EXPECTED: %u; RECEIVED: %u", WSASendTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "WSASend iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSASend returned SOCKET_ERROR - ec = %u", WSAGetLastError());

                if (0 != WSASendRequest.nDataBuffers) {
                    // Send the send cancel
                    WSASendRequest.dwMessageId = WSASEND_CANCEL_MSG;
                    WSASendRequest.nDataBuffers = 0;
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendRequest), (char *) &WSASendRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);
                }
            }
            else if (SOCKET_ERROR == WSASendTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSASend returned non-SOCKET_ERROR");
            }
            else {
                if (nBufferlen != (int) dwBytes) {
                    xLog(hLog, XLL_FAIL, "WSASend return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwBytes);
                    bTestPassed = FALSE;
                }

                // Send the read request
                NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendRequest), (char *) &WSASendRequest);

                // Wait for the read complete
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                NetsyncFreeMessage(pMessage);

                if ((TRUE != WSASendTable[dwTableIndex].bFillQueue) && (TRUE != WSASendTable[dwTableIndex].bShutdown)) {
                    // Call recv
                    if (0 != (SOCKET_TCP & WSASendTable[dwTableIndex].dwSocket)) {
                        if (0 != nBufferlen) {
                            ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
                            nBytes = 0;

                            do {
                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                if (1 == iReturnCode) {
                                    iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], BUFFER_10_LEN, 0);
                                }
                                else if (0 == iReturnCode) {
                                    iReturnCode = SOCKET_ERROR;
                                    SetLastError(WSAETIMEDOUT);
                                }

                                if (SOCKET_ERROR != iReturnCode) {
                                    nBytes += iReturnCode;
                                }
                            } while (SOCKET_ERROR != iReturnCode);

                            if (WSAETIMEDOUT != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else if (nBufferlen != nBytes) {
                                xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, nBytes);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                                xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                bTestPassed = FALSE;
                            }
                        }
                    }
                    else {
                        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                        if (TRUE == bNonblocking) {
                            FD_ZERO(&readfds);
                            FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                            select(0, &readfds, NULL, NULL, NULL);
                        }

                        iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, sizeof(RecvBufferLarge), 0);
                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else if (nBufferlen != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, iReturnCode);
                            bTestPassed = FALSE;
                        }
                        else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                            xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                            bTestPassed = FALSE;
                        }
                    }
                }

                if (3 == WSASendTable[dwTableIndex].nDataBuffers) {
                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                    }

                    // Switch the blocking mode
                    Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                    bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                    // Call WSASend
                    iReturnCode = WSASend((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, dwBufferCount, &dwBytes, WSASendTable[dwTableIndex].nFlags, NULL, NULL);

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "WSASend returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                        WSASendRequest.dwMessageId = WSASEND_CANCEL_MSG;
                    }
                    else {
                        if (nBufferlen != (int) dwBytes) {
                            xLog(hLog, XLL_FAIL, "WSASend return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwBytes);
                            bTestPassed = FALSE;
                        }

                        WSASendRequest.dwMessageId = WSASEND_REQUEST_MSG;
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendRequest), (char *) &WSASendRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    if (SOCKET_ERROR != iReturnCode) {
                        // Call recv
                        if (0 != (SOCKET_TCP & WSASendTable[dwTableIndex].dwSocket)) {
                            if (0 != nBufferlen) {
                                ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
                                nBytes = 0;

                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                    if (1 == iReturnCode) {
                                        iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], BUFFER_10_LEN, 0);
                                    }
                                    else if (0 == iReturnCode) {
                                        iReturnCode = SOCKET_ERROR;
                                        SetLastError(WSAETIMEDOUT);
                                    }

                                    if (SOCKET_ERROR != iReturnCode) {
                                        nBytes += iReturnCode;
                                    }
                                } while (SOCKET_ERROR != iReturnCode);

                                if (WSAETIMEDOUT != WSAGetLastError()) {
                                    xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                    bTestPassed = FALSE;
                                }
                                else if (nBufferlen != nBytes) {
                                    xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, nBytes);
                                    bTestPassed = FALSE;
                                }
                                else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                        else {
                            ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                            if (TRUE == bNonblocking) {
                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                select(0, &readfds, NULL, NULL, NULL);
                            }

                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, sizeof(RecvBufferLarge), 0);
                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else if (nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                                xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                bTestPassed = FALSE;
                            }
                        }
                    }

                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 3, 3);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                    }

                    // Switch the blocking mode
                    Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                    bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                    // Call WSASend
                    iReturnCode = WSASend((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, dwBufferCount, &dwBytes, WSASendTable[dwTableIndex].nFlags, NULL, NULL);

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "WSASend returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                        WSASendRequest.dwMessageId = WSASEND_CANCEL_MSG;
                    }
                    else {
                        if (nBufferlen != (int) dwBytes) {
                            xLog(hLog, XLL_FAIL, "WSASend return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwBytes);
                            bTestPassed = FALSE;
                        }

                        WSASendRequest.dwMessageId = WSASEND_REQUEST_MSG;
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendRequest), (char *) &WSASendRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    if (SOCKET_ERROR != iReturnCode) {
                        // Call recv
                        if (0 != (SOCKET_TCP & WSASendTable[dwTableIndex].dwSocket)) {
                            if (0 != nBufferlen) {
                                ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
                                nBytes = 0;

                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                    if (1 == iReturnCode) {
                                        iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], BUFFER_10_LEN, 0);
                                    }
                                    else if (0 == iReturnCode) {
                                        iReturnCode = SOCKET_ERROR;
                                        SetLastError(WSAETIMEDOUT);
                                    }

                                    if (SOCKET_ERROR != iReturnCode) {
                                        nBytes += iReturnCode;
                                    }
                                } while (SOCKET_ERROR != iReturnCode);

                                if (WSAETIMEDOUT != WSAGetLastError()) {
                                    xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                    bTestPassed = FALSE;
                                }
                                else if (nBufferlen != nBytes) {
                                    xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, nBytes);
                                    bTestPassed = FALSE;
                                }
                                else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                        else {
                            ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                            if (TRUE == bNonblocking) {
                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                select(0, &readfds, NULL, NULL, NULL);
                            }

                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, sizeof(RecvBufferLarge), 0);
                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else if (nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                                xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                bTestPassed = FALSE;
                            }
                        }
                    }
                }
                else if (100 == WSASendTable[dwTableIndex].nDataBuffers) {
                    for (nSendCount = 0; nSendCount < (int) (WSASendTable[dwTableIndex].nDataBuffers * WSASendBufferTableCount); nSendCount++) {
                        // Initialize the buffers
                        sprintf(SendBuffer10, "%05d%05d", 2 + nSendCount, 2 + nSendCount);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                        }

                        dwBufferCount = 1;
                        if (BUFFER_10 == WSASendBufferTable[nSendCount % WSASendBufferTableCount].dwBuffer) {
                            WSABuf[0].len = BUFFER_10_LEN;
                            WSABuf[0].buf = SendBuffer10;

                            nBufferlen = BUFFER_10_LEN;
                        }
                        else if (BUFFER_NULLZERO == WSASendBufferTable[nSendCount % WSASendBufferTableCount].dwBuffer) {
                            WSABuf[0].len = 0;
                            WSABuf[0].buf = NULL;

                            nBufferlen = 0;
                        }
                        else if (BUFFER_TCPLARGE == WSASendBufferTable[nSendCount % WSASendBufferTableCount].dwBuffer) {
                            WSABuf[0].len = BUFFER_TCPLARGE_LEN;
                            WSABuf[0].buf = SendBufferLarge;

                            nBufferlen = BUFFER_TCPLARGE_LEN;
                        }
                        else if (BUFFER_UDPLARGE == WSASendBufferTable[nSendCount % WSASendBufferTableCount].dwBuffer) {
                            WSABuf[0].len = BUFFER_UDPLARGE_LEN;
                            WSABuf[0].buf = SendBufferLarge;

                            nBufferlen = BUFFER_UDPLARGE_LEN;
                        }
                        else if (BUFFER_TCPMULTI == WSASendBufferTable[nSendCount % WSASendBufferTableCount].dwBuffer) {
                            WSABuf[0].len = BUFFER_10_LEN;
                            WSABuf[0].buf = SendBufferLarge;

                            WSABuf[1].len = 0;
                            WSABuf[1].buf = NULL;

                            WSABuf[2].len = BUFFER_TCPLARGE_LEN - BUFFER_10_LEN;
                            WSABuf[2].buf = &SendBufferLarge[BUFFER_10_LEN];

                            dwBufferCount = 3;
                            nBufferlen = BUFFER_TCPLARGE_LEN;
                        }
                        else if (BUFFER_UDPMULTI == WSASendBufferTable[nSendCount % WSASendBufferTableCount].dwBuffer) {
                            WSABuf[0].len = BUFFER_10_LEN;
                            WSABuf[0].buf = SendBufferLarge;

                            WSABuf[1].len = 0;
                            WSABuf[1].buf = NULL;

                            WSABuf[2].len = BUFFER_UDPLARGE_LEN - BUFFER_10_LEN;
                            WSABuf[2].buf = &SendBufferLarge[BUFFER_10_LEN];

                            dwBufferCount = 3;
                            nBufferlen = BUFFER_UDPLARGE_LEN;
                        }

                        xLog(hLog, XLL_INFO, "Iteration %d", nSendCount);

                        // Call WSASend
                        iReturnCode = WSASend((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, dwBufferCount, &dwBytes, 0, NULL, NULL);

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "WSASend returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                            WSASendRequest.dwMessageId = WSASEND_CANCEL_MSG;
                        }
                        else {
                            if (nBufferlen != (int) dwBytes) {
                                xLog(hLog, XLL_FAIL, "WSASend return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwBytes);
                                bTestPassed = FALSE;
                            }

                            WSASendRequest.dwMessageId = WSASEND_REQUEST_MSG;
                        }

                        // Send the read request
                        WSASendRequest.nBufferlen = nBufferlen;
                        NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendRequest), (char *) &WSASendRequest);

                        // Wait for the read complete
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);

                        if (SOCKET_ERROR != iReturnCode) {
                            // Call recv
                            if (0 != (SOCKET_TCP & WSASendTable[dwTableIndex].dwSocket)) {
                                if (0 != nBufferlen) {
                                    ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
                                    nBytes = 0;

                                    do {
                                        FD_ZERO(&readfds);
                                        FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                        iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                        if (1 == iReturnCode) {
                                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], BUFFER_10_LEN, 0);
                                        }
                                        else if (0 == iReturnCode) {
                                            iReturnCode = SOCKET_ERROR;
                                            SetLastError(WSAETIMEDOUT);
                                        }

                                        if (SOCKET_ERROR != iReturnCode) {
                                            nBytes += iReturnCode;
                                        }
                                    } while (SOCKET_ERROR != iReturnCode);

                                    if (WSAETIMEDOUT != WSAGetLastError()) {
                                        xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                        bTestPassed = FALSE;
                                    }
                                    else if (nBufferlen != nBytes) {
                                        xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, nBytes);
                                        bTestPassed = FALSE;
                                    }
                                    else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                                        xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                        bTestPassed = FALSE;
                                    }
                                }
                            }
                            else {
                                ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                                if (TRUE == bNonblocking) {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                    select(0, &readfds, NULL, NULL, NULL);
                                }

                                iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, sizeof(RecvBufferLarge), 0);
                                if (SOCKET_ERROR == iReturnCode) {
                                    xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                    bTestPassed = FALSE;
                                }
                                else if (nBufferlen != iReturnCode) {
                                    xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, iReturnCode);
                                    bTestPassed = FALSE;
                                }
                                else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "WSASend succeeded");
                }
            }
        }

        // Switch the blocking mode
        if (WSASendTable[dwTableIndex].bNonblock != bNonblocking) {
            Nonblock = (TRUE == WSASendTable[dwTableIndex].bNonblock) ? 1 : 0;
            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
            bNonblocking = WSASendTable[dwTableIndex].bNonblock;
        }

        if ((TRUE == WSASendTable[dwTableIndex].bAccept) || (TRUE == WSASendTable[dwTableIndex].bConnect)) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendRequest), (char *) &WSASendRequest);
        }

        // Close the socket
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & WSASendTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & WSASendTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & WSASendTable[dwTableIndex].dwSocket))) {
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
WSASendTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests WSASend - Server side

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
    // WSASendRequest is the request
    WSASEND_REQUEST   WSASendRequest;
    // WSASendComplete is the result
    WSASEND_COMPLETE  WSASendComplete;

    // sSocket is the socket descriptor
    SOCKET            sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET            nsSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD             dwBufferSize = 1;
    // iTimeout is the send and receive timeout value for the socket
    int               iTimeout = 5000;
    // bNagle indicates if Nagle is enabled
    BOOL              bNagle = FALSE;
    // nQueueLen is the size of the queue
    int               nQueueLen;

    // localname is the local address
    SOCKADDR_IN       localname;
    // remotename is the remote address
    SOCKADDR_IN       remotename;

    // readfds is the set of sockets to check for a read condition
    fd_set            readfds;
    // timeout is the timeout for select
    timeval           fdstimeout = { 1, 0 };

    // SendBufferLarge is the large send buffer
    char              SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // nBytes is the number of bytes sent/received
    int               nBytes;
    // nSendCount is a counter to enumerate each send
    int               nSendCount;

    // iReturnCode is the return code of the operation
    int               iReturnCode;



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
        CopyMemory(&WSASendRequest, pMessage, sizeof(WSASendRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, WSASendRequest.nSocketType, 0);

        if (TRUE == WSASendRequest.bFillQueue) {
            // Set the buffer size
            setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
            setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
        }

        // Set the send and receive timeout values to 5 sec
        setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));

        if (SOCK_STREAM == WSASendRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(WSASendRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if ((SOCK_STREAM == WSASendRequest.nSocketType) && (TRUE == WSASendRequest.bServerAccept)) {
            // Place the socket in listening mode
            listen(sSocket, SOMAXCONN);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(WSASendRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        WSASendComplete.dwMessageId = WSASEND_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendComplete), (char *) &WSASendComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if ((SOCK_STREAM == WSASendRequest.nSocketType) && (TRUE == WSASendRequest.bServerAccept)) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        if (TRUE == WSASendRequest.bRemoteClose) {
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

        // Send the complete
        WSASendComplete.dwMessageId = WSASEND_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendComplete), (char *) &WSASendComplete);

        if (TRUE == WSASendRequest.bFillQueue) {
            // Wait for the request
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            nQueueLen = ((PWSASEND_REQUEST) pMessage)->nQueueLen;
            CopyMemory(&WSASendRequest, pMessage, sizeof(WSASendRequest));
            NetsyncFreeMessage(pMessage);

            if (WSASEND_CANCEL_MSG != WSASendRequest.dwMessageId) {
                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                for (nSendCount = 0; nSendCount < nQueueLen; nSendCount++) {
                    recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0);
                }
            }

            // Send the complete
            WSASendComplete.dwMessageId = WSASEND_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendComplete), (char *) &WSASendComplete);
        }

        if ((0 < WSASendRequest.nDataBuffers) || (-1 == WSASendRequest.nDataBuffers)) {
            // Wait for the request
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            CopyMemory(&WSASendRequest, pMessage, sizeof(WSASendRequest));
            NetsyncFreeMessage(pMessage);

            if ((WSASEND_CANCEL_MSG != WSASendRequest.dwMessageId) && ((0 != WSASendRequest.nBufferlen) || (SOCK_STREAM != WSASendRequest.nSocketType))) {
                if (SOCK_DGRAM == WSASendRequest.nSocketType) {
                    ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));

                    if (SOCKET_ERROR != recv(sSocket, SendBufferLarge, sizeof(SendBufferLarge), 0)) {
                        send(sSocket, SendBufferLarge, strlen(SendBufferLarge), 0);
                    }
                }
                else {
                    ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                    nBytes = 0;

                    do {
                        FD_ZERO(&readfds);
                        FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                        iReturnCode = SOCKET_ERROR;
                        if (1 == select(0, &readfds, NULL, NULL, &fdstimeout)) {
                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &SendBufferLarge[nBytes], BUFFER_10_LEN, 0);
                        }

                        if (SOCKET_ERROR != iReturnCode) {
                            nBytes += iReturnCode;
                        }
                    } while ((SOCKET_ERROR != iReturnCode) && (nBytes < WSASendRequest.nBufferlen));

                    if (nBytes == WSASendRequest.nBufferlen) {
                        send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                    }
                }
            }

            // Send the complete
            WSASendComplete.dwMessageId = WSASEND_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendComplete), (char *) &WSASendComplete);

            if (3 == WSASendRequest.nDataBuffers) {
                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                if (WSASEND_CANCEL_MSG != ((PWSASEND_REQUEST) pMessage)->dwMessageId) {
                    if (SOCK_DGRAM == WSASendRequest.nSocketType) {
                        ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));

                        if (SOCKET_ERROR != recv(sSocket, SendBufferLarge, sizeof(SendBufferLarge), 0)) {
                            send(sSocket, SendBufferLarge, strlen(SendBufferLarge), 0);
                        }
                    }
                    else {
                        ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                        nBytes = 0;

                        do {
                            FD_ZERO(&readfds);
                            FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                            iReturnCode = SOCKET_ERROR;
                            if (1 == select(0, &readfds, NULL, NULL, &fdstimeout)) {
                                iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &SendBufferLarge[nBytes], BUFFER_10_LEN, 0);
                            }

                            if (SOCKET_ERROR != iReturnCode) {
                                nBytes += iReturnCode;
                            }
                        } while ((SOCKET_ERROR != iReturnCode) && (nBytes < WSASendRequest.nBufferlen));

                        if (nBytes == WSASendRequest.nBufferlen) {
                            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                        }
                    }
                }
                NetsyncFreeMessage(pMessage);

                // Send the complete
                WSASendComplete.dwMessageId = WSASEND_COMPLETE_MSG;
                NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendComplete), (char *) &WSASendComplete);

                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                if (WSASEND_CANCEL_MSG != ((PWSASEND_REQUEST) pMessage)->dwMessageId) {
                    if (SOCK_DGRAM == WSASendRequest.nSocketType) {
                        ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));

                        if (SOCKET_ERROR != recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, sizeof(SendBufferLarge), 0)) {
                            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, strlen(SendBufferLarge), 0);
                        }
                    }
                    else {
                        ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                        nBytes = 0;

                        do {
                            FD_ZERO(&readfds);
                            FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                            iReturnCode = SOCKET_ERROR;
                            if (1 == select(0, &readfds, NULL, NULL, &fdstimeout)) {
                                iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &SendBufferLarge[nBytes], BUFFER_10_LEN, 0);
                            }

                            if (SOCKET_ERROR != iReturnCode) {
                                nBytes += iReturnCode;
                            }
                        } while ((SOCKET_ERROR != iReturnCode) && (nBytes < WSASendRequest.nBufferlen));

                        if (nBytes == WSASendRequest.nBufferlen) {
                            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                        }
                    }
                }
                NetsyncFreeMessage(pMessage);

                // Send the complete
                WSASendComplete.dwMessageId = WSASEND_COMPLETE_MSG;
                NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendComplete), (char *) &WSASendComplete);
            }
            else if (100 <= WSASendRequest.nDataBuffers) {
                for (nSendCount = 0; nSendCount < WSASendRequest.nDataBuffers; nSendCount++) {
                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                    if ((0 != ((PWSASEND_REQUEST) pMessage)->nBufferlen) || (SOCK_STREAM != WSASendRequest.nSocketType)) {
                        if (WSASEND_CANCEL_MSG != ((PWSASEND_REQUEST) pMessage)->dwMessageId) {
                            if (SOCK_DGRAM == WSASendRequest.nSocketType) {
                                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));

                                if (SOCKET_ERROR != recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, sizeof(SendBufferLarge), 0)) {
                                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, strlen(SendBufferLarge), 0);
                                }
                            }
                            else {
                                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                                nBytes = 0;

                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = SOCKET_ERROR;
                                    if (1 == select(0, &readfds, NULL, NULL, &fdstimeout)) {
                                        iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &SendBufferLarge[nBytes], BUFFER_10_LEN, 0);
                                    }

                                    if (SOCKET_ERROR != iReturnCode) {
                                        nBytes += iReturnCode;
                                    }
                                } while ((SOCKET_ERROR != iReturnCode) && (nBytes < ((PWSASEND_REQUEST) pMessage)->nBufferlen));

                                if (nBytes == ((PWSASEND_REQUEST) pMessage)->nBufferlen) {
                                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                                }
                            }
                        }
                    }
                    NetsyncFreeMessage(pMessage);

                    // Send the complete
                    WSASendComplete.dwMessageId = WSASEND_COMPLETE_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendComplete), (char *) &WSASendComplete);
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
