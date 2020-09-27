/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  wsasendto.c

Abstract:

  This modules tests wsasendto

Author:

  Steven Kehrli (steveke) 1-Dec-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// WSASendTo messages

#define WSASENDTO_REQUEST_MSG   NETSYNC_MSG_USER + 160 + 1
#define WSASENDTO_CANCEL_MSG    NETSYNC_MSG_USER + 160 + 2
#define WSASENDTO_COMPLETE_MSG  NETSYNC_MSG_USER + 160 + 3

typedef struct _WSASENDTO_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  ReceivePort;
    u_short  SendPort;
    BOOL     bServerAccept;
    BOOL     bFillQueue;
    INT      nQueueLen;
    BOOL     bRemoteClose;
    int      nDataBuffers;
    int      nBufferlen;
} WSASENDTO_REQUEST, *PWSASENDTO_REQUEST;

typedef struct _WSASENDTO_COMPLETE {
    DWORD    dwMessageId;
} WSASENDTO_COMPLETE, *PWSASENDTO_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

#define NAME_ANY              0
#define NAME_ANYADDR          1
#define NAME_ANYPORT          2
#define NAME_LOCAL            3
#define NAME_LOOPBACK         4
#define NAME_BROADCAST        5
#define NAME_MULTICAST        6
#define NAME_UNAVAIL          7
#define NAME_BAD              8
#define NAME_NULL             9
#define NAME_REMOTE_1        10
#define NAME_REMOTE_2        11

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



typedef struct WSASENDTOBUFFER_TABLE {
    DWORD  dwBuffer;
} WSASENDTOBUFFER_TABLE, *PWSASENDTOBUFFER_TABLE;

WSASENDTOBUFFER_TABLE WSASendToBufferTcpTable[] = { { BUFFER_10        },
                                                    { BUFFER_NULLZERO  },
                                                    { BUFFER_TCPLARGE  },
                                                    { BUFFER_TCPMULTI  } };

#define WSASendToBufferTcpTableCount  (sizeof(WSASendToBufferTcpTable) / sizeof(WSASENDTOBUFFER_TABLE));

WSASENDTOBUFFER_TABLE WSASendToBufferUdpTable[] = { { BUFFER_10        },
                                                    { BUFFER_NULLZERO  },
                                                    { BUFFER_UDPLARGE  },
                                                    { BUFFER_UDPMULTI  } };
#define WSASendToBufferUdpTableCount  (sizeof(WSASendToBufferUdpTable) / sizeof(WSASENDTOBUFFER_TABLE));



typedef struct WSASENDTO_TABLE {
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
    int            nFlags;                                  // nFlags specifies the WSASendTo flags
    DWORD          dwName;                                  // dwName indicates the address
    int            namelen;                                 // namelen is the length of the address buffer
    int            nOverlapped;                             // nOverlapped specifies if overlapped I/O is used
    BOOL           bCompletionRoutine;                      // bCompletionRoutine specifies if completion routine is used
    int            iReturnCode;                             // iReturnCode is the return code of WSASendTo
    int            iLastError;                              // iLastError is the error code if the operation failed
    BOOL           bRIP;                                    // Specifies a RIP test case
} WSASENDTO_TABLE, *PWSASENDTO_TABLE;

static WSASENDTO_TABLE WSASendToTable[] =
{
    { "16.1 Not Initialized",            FALSE, FALSE, SOCKET_INVALID_SOCKET,         FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "16.2 s = INT_MIN",                TRUE,  TRUE,  SOCKET_INT_MIN,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "16.3 s = -1",                     TRUE,  TRUE,  SOCKET_NEG_ONE,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "16.4 s = 0",                      TRUE,  TRUE,  SOCKET_ZERO,                   FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "16.5 s = INT_MAX",                TRUE,  TRUE,  SOCKET_INT_MAX,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "16.6 s = INVALID_SOCKET",         TRUE,  TRUE,  SOCKET_INVALID_SOCKET,         FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "16.7 Not Bound TCP",              TRUE,  TRUE,  SOCKET_TCP,                    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "16.8 Listening TCP",              TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "16.9 Accepted TCP",               TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.10 Connected TCP",             TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.11 Fill Queue TCP",            TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.12 Fill Queue I/O TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       OVERLAPPED_IO_RESULT, FALSE, 0,            0,                 FALSE },
    { "16.13 Fill Queue Event TCP",      TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "16.14 Event Event TCP",           TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,          0, NAME_NULL,      0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "16.15 SD_RECEIVE Accept TCP",     TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.16 SD_SEND Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "16.17 SD_BOTH Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "16.18 SD_RECEIVE Connect TCP",    TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.19 SD_SEND Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "16.20 SD_BOTH Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "16.21 Close Accept TCP",          TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "16.22 Close Connect TCP",         TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "16.23 NULL Buffer TCP",           TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.24 NULL 0 Bufferlen TCP",      TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,    0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.25 0 Bufferlen TCP",           TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,        0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.26 Small Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,       0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.27 Exact Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.28 Large Buffer TCP",          TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_TCPLARGE,    0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.29 Multi Buffer TCP",          TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_TCPMULTI,    0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.30 Switch Send TCP",           TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.31 Iterative Send TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.32 NULL Buffer Ptr TCP",       TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.33 0 Buffer Count TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT,   0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.34 flags != 0 TCP",            TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          4, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.35 Same addr TCP",             TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.36 Different addr TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_2,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.37 Broadcast addr TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_BROADCAST, sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.38 Multicast addr TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_MULTICAST, sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.39 Bad addr TCP",              TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_BAD,       sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "16.40 Zero addr TCP",             TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_ANY,       sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.41 Any addr TCP",              TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.42 Zero port addr TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_ANYPORT,   sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.43 Large addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) + 1, 0,                    FALSE, 0,            0,                 FALSE },
    { "16.44 Exact addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.45 Small addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) - 1, 0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "16.46 Zero addrlen TCP",          TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  0,                       0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "16.47 Neg addrlen TCP",           TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  -1,                      0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "16.48 Callback != NULL TCP",      TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.49 Not Bound NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "16.50 Listening NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "16.51 Accepted NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.52 Connected NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.53 Fill Queue NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.54 Fill Queue I/O NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       OVERLAPPED_IO_RESULT, FALSE, 0,            0,                 FALSE },
    { "16.55 Fill Queue Event NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "16.56 Event Event NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,          0, NAME_NULL,      0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "16.57 SD_RECEIVE Accept NB TCP",  TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.58 SD_SEND Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "16.59 SD_BOTH Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "16.60 SD_RECEIVE Connect NB TCP", TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.61 SD_SEND Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "16.62 SD_BOTH Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "16.63 Close Accept NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "16.64 Close Connect NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "16.65 NULL Buffer NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.66 NULL 0 Bufferlen NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,    0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.67 0 Bufferlen NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,        0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.68 Small Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,       0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.69 Exact Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.70 Large Buffer NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_TCPLARGE,    0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.71 Multi Buffer NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_TCPMULTI,    0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.72 Switch Send NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.73 Iterative Send NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.74 NULL Buffer Ptr NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.75 0 Buffer Count NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT,   0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.76 flags != 0 NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          4, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.77 Same addr NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.78 Different addr NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_2,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.79 Broadcast addr NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_BROADCAST, sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.80 Multicast addr NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_MULTICAST, sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.81 Bad addr NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_BAD,       sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "16.82 Zero addr NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_ANY,       sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.83 Any addr NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.84 Zero port addr NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_ANYPORT,   sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.85 Large addrlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) + 1, 0,                    FALSE, 0,            0,                 FALSE },
    { "16.86 Exact addrlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.87 Small addrlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) - 1, 0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "16.88 Zero addrlen NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  0,                       0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "16.89 Neg addrlen NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  -1,                      0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "16.90 Callback != NULL NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.91 Not Bound UDP",             TRUE,  TRUE,  SOCKET_UDP,                    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "16.92 Not Connected UDP",         TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "16.93 Connected UDP",             TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.94 Event Event UDP",           TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,          0, NAME_NULL,      0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "16.95 SD_RECEIVE Conn UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.96 SD_SEND Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "16.97 SD_BOTH Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "16.98 NULL Buffer UDP",           TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.99 NULL 0 Bufferlen UDP",      TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,    0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.100 0 Bufferlen UDP",          TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,        0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.101 Small Bufferlen UDP",      TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,       0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.102 Exact Bufferlen UDP",      TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.103 Large Buffer UDP",         TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPLARGE,    0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.104 Too Large Buffer UDP",     TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPTOOLARGE, 0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "16.105 Multi Buffer UDP",         TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPMULTI,    0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.106 Switch Send UDP",          TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.107 Iterative Send UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.108 NULL Buffer Ptr UDP",      TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.109 0 Buffer Count UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT,   0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.110 flags != 0 UDP",           TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          4, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.111 Not Bound Addr UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.112 Bound Addr UDP",           TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.113 Same addr UDP",            TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.114 Different addr UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_2,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.115 Broadcast addr UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_BROADCAST, sizeof(SOCKADDR_IN),     0,                    FALSE, SOCKET_ERROR, WSAEACCES,         FALSE },
    { "16.116 Broadcast enabled UDP",    TRUE,  TRUE,  SOCKET_UDP | SOCKET_BROADCAST, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_BROADCAST, sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.117 Multicast addr UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_MULTICAST, sizeof(SOCKADDR_IN),     0,                    FALSE, SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "16.118 Bad addr UDP",             TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_BAD,       sizeof(SOCKADDR_IN),     0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.119 Zero addr UDP",            TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_ANY,       sizeof(SOCKADDR_IN),     0,                    FALSE, SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "16.120 Any addr UDP",             TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     0,                    FALSE, SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "16.121 Zero port addr UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_ANYPORT,   sizeof(SOCKADDR_IN),     0,                    FALSE, SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "16.122 Large addrlen UDP",        TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) + 1, 0,                    FALSE, 0,            0,                 FALSE },
    { "16.123 Exact addrlen UDP",        TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.124 Small addrlen UDP",        TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) - 1, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.125 Zero addrlen UDP",         TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.126 Neg addrlen UDP",          TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  -1,                      0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.127 Callback != NULL UDP",     TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.128 Not Bound NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "16.129 Not Connected NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "16.130 Connected NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.131 Event Event NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,          0, NAME_NULL,      0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "16.132 SD_RECEIVE Conn NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.133 SD_SEND Conn NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "16.134 SD_BOTH Conn NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "16.135 NULL Buffer NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.136 NULL 0 Bufferlen NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,    0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.137 0 Bufferlen NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,        0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.138 Small Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,       0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.139 Exact Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.140 Large Buffer NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPLARGE,    0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.141 Too Large Buffer NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPTOOLARGE, 0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "16.142 Multi Buffer NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPMULTI,    0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.143 Switch Send NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.144 Iterative Send NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,          0, NAME_NULL,      0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "16.145 NULL Buffer Ptr NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.146 0 Buffer Count NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT,   0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.147 flags != 0 NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          4, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.148 Not Bound Addr NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.149 Bound Addr NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.150 Same addr NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.151 Different addr NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_2,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.152 Broadcast addr NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_BROADCAST, sizeof(SOCKADDR_IN),     0,                    FALSE, SOCKET_ERROR, WSAEACCES,         FALSE },
    { "16.153 Broadcast enabled NB UDP", TRUE,  TRUE,  SOCKET_UDP | SOCKET_BROADCAST, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_BROADCAST, sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.154 Multicast addr NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_MULTICAST, sizeof(SOCKADDR_IN),     0,                    FALSE, SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "16.155 Bad addr NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_BAD,       sizeof(SOCKADDR_IN),     0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.156 Zero addr NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_ANY,       sizeof(SOCKADDR_IN),     0,                    FALSE, SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "16.157 Any addr NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     0,                    FALSE, SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "16.158 Zero port addr NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_ANYPORT,   sizeof(SOCKADDR_IN),     0,                    FALSE, SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "16.159 Large addrlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) + 1, 0,                    FALSE, 0,            0,                 FALSE },
    { "16.160 Exact addrlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "16.161 Small addrlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) - 1, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.162 Zero addrlen NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.163 Neg addrlen NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_REMOTE_1,  -1,                      0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.164 Callback != NULL NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,          0, NAME_NULL,      0,                       0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "16.165 Closed Socket TCP",        TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "16.166 Closed Socket UDP",        TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "16.167 Not Initialized",          FALSE, FALSE, SOCKET_INVALID_SOCKET,         FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,        0, NAME_NULL,      0,                       0,                    FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define WSASendToTableCount (sizeof(WSASendToTable) / sizeof(WSASENDTO_TABLE))

NETSYNC_TYPE_THREAD  WSASendToTestSessionNt =
{
    1,
    WSASendToTableCount * 2,
    L"xnetapi_nt.dll",
    "WSASendToTestServer"
};

NETSYNC_TYPE_THREAD  WSASendToTestSessionXbox =
{
    1,
    WSASendToTableCount * 2,
    L"xnetapi_xbox.dll",
    "WSASendToTestServer"
};



// Callback routine

VOID CALLBACK WSASendToCompletionRoutine(IN DWORD  dwError, IN DWORD  cbTransferred, IN LPWSAOVERLAPPED  lpOverlapped, IN DWORD  dwFlags) { return; }



VOID
WSASendToTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests WSASendTo - Client side

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
    // WSASendToRequest is the request sent to the server
    WSASENDTO_REQUEST      WSASendToRequest;

    // HostXnAddr is the host xnet address
    XNADDR                 HostXnAddr;
    // hostaddr is the local host address
    u_long                 hostaddr = 0;

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
    // bBroadcast indicates the socket is enabled to send broadcast data
    BOOL                   bBroadcast = TRUE;
    // Nonblock sets the socket to blocking or non-blocking mode
    u_long                 Nonblock;
    // bNonblocking indicates if the socket is in non-blocking mode
    BOOL                   bNonblocking = FALSE;

    // localname is the local address
    SOCKADDR_IN            localname;
    // remotename1 is the first remote address
    SOCKADDR_IN            remotename1;
    // remotename2 is the second remote address
    SOCKADDR_IN            remotename2;

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

    // WSASendToBufferTable is the test buffer table to use for iterative send
    PWSASENDTOBUFFER_TABLE WSASendToBufferTable;
    // WSASendToBufferTableCount is the number of elements within the test buffer table
    size_t                 WSASendToBufferTableCount;
    // WSAOverlapped is the overlapped I/O structure
    WSAOVERLAPPED          WSAOverlapped;
    // dwFlags is the flags
    DWORD                  dwFlags;

    // bException indicates if an exception occurred
    BOOL                   bException;
    // iReturnCode is the return code of the operation
    int                    iReturnCode;
    // dwReturnCode is the return code of the operation
    DWORD                  dwReturnCode;
    // iLastError is the error code if the operation failed
    int                    iLastError;
    // bTestPassed indicates if the test passed
    BOOL                   bTestPassed;

    // szFunctionName is the function name
    CHAR                   szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    sprintf(szFunctionName, "WSASendTo v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_WSASendTo+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_WSASendTo-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = WSASendToTestSessionXbox;
    }
    else {
        NetsyncTypeSession = WSASendToTestSessionNt;
    }

    // Initialize the overlapped I/O structure
    ZeroMemory(&WSAOverlapped, sizeof(WSAOverlapped));
    WSAOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    // Initialize the net subsystem
    XNetAddRef();

    // Get the local xnaddr
    do {
        dwReturnCode = XNetGetTitleXnAddr(&HostXnAddr);
        if (0 == dwReturnCode) {
            Sleep(SLEEP_ZERO_TIME);
        }
    } while (0 == dwReturnCode);
    hostaddr = HostXnAddr.ina.s_addr;

    for (dwTableIndex = 0; dwTableIndex < WSASendToTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, WSASendToTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, WSASendToTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != WSASendToTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, WSASendToTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == WSASendToTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != WSASendToTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == WSASendToTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = WSASendToTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == WSASendToTable[dwTableIndex].bNetsyncConnected)) {
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
        if (SOCKET_INT_MIN == WSASendToTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == WSASendToTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == WSASendToTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == WSASendToTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == WSASendToTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & WSASendToTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Set the socket for broadcast
        if (0 != (SOCKET_BROADCAST & WSASendToTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, SOL_SOCKET, SO_BROADCAST, (char *) &bBroadcast, sizeof(bBroadcast));
        }

        if (TRUE == WSASendToTable[dwTableIndex].bFillQueue) {
            // Set the buffer size
            setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
            setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
        }

        // Set the send and receive timeout values to 5 sec
        if ((0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & WSASendToTable[dwTableIndex].dwSocket))) {
            setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
            setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the socket to non-blocking mode
        if (TRUE == WSASendToTable[dwTableIndex].bNonblock) {
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);
            bNonblocking = TRUE;
        }
        else {
            bNonblocking = FALSE;
        }

        // Bind the socket
        if (TRUE == WSASendToTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Place the socket in the listening state
        if (TRUE == WSASendToTable[dwTableIndex].bListen) {
            listen(sSocket, SOMAXCONN);
        }

        if (0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) {
            WSASendToBufferTable = WSASendToBufferTcpTable;
            WSASendToBufferTableCount = WSASendToBufferTcpTableCount;
        }
        else {
            WSASendToBufferTable = WSASendToBufferUdpTable;
            WSASendToBufferTableCount = WSASendToBufferUdpTableCount;
        }

        if ((TRUE == WSASendToTable[dwTableIndex].bAccept) || (TRUE == WSASendToTable[dwTableIndex].bConnect) || (0 == WSASendToTable[dwTableIndex].iReturnCode)) {
            // Initialize the send request
            WSASendToRequest.dwMessageId = WSASENDTO_REQUEST_MSG;
            if (0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) {
                WSASendToRequest.nSocketType = SOCK_STREAM;
            }
            else {
                WSASendToRequest.nSocketType = SOCK_DGRAM;
            }
            WSASendToRequest.ReceivePort = (NAME_REMOTE_2 != WSASendToTable[dwTableIndex].dwName) || (0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) ? CurrentPort : CurrentPort + 1;
            WSASendToRequest.SendPort = (TRUE == WSASendToTable[dwTableIndex].bBind) ? CurrentPort : 0;
            WSASendToRequest.bServerAccept = WSASendToTable[dwTableIndex].bConnect;
            WSASendToRequest.bFillQueue = WSASendToTable[dwTableIndex].bFillQueue;
            WSASendToRequest.nQueueLen = 0;
            WSASendToRequest.bRemoteClose = WSASendToTable[dwTableIndex].bRemoteClose;
            if (100 == WSASendToTable[dwTableIndex].nDataBuffers) {
                WSASendToRequest.nDataBuffers = WSASendToTable[dwTableIndex].nDataBuffers * WSASendToBufferTableCount;
            }
            else {
                WSASendToRequest.nDataBuffers = (0 == WSASendToTable[dwTableIndex].iReturnCode) ? WSASendToTable[dwTableIndex].nDataBuffers : 0;
            }
            if (BUFFER_10 == WSASendToTable[dwTableIndex].dwBuffer) {
                WSASendToRequest.nBufferlen = BUFFER_10_LEN;
            }
            else if (BUFFER_SMALL == WSASendToTable[dwTableIndex].dwBuffer) {
                WSASendToRequest.nBufferlen = BUFFER_10_LEN - 1;
            }
            else if (BUFFER_TCPLARGE == WSASendToTable[dwTableIndex].dwBuffer) {
                WSASendToRequest.nBufferlen = BUFFER_TCPLARGE_LEN;
            }
            else if (BUFFER_UDPLARGE == WSASendToTable[dwTableIndex].dwBuffer) {
                WSASendToRequest.nBufferlen = BUFFER_UDPLARGE_LEN;
            }
            else if (BUFFER_UDPTOOLARGE == WSASendToTable[dwTableIndex].dwBuffer) {
                WSASendToRequest.nBufferlen = BUFFER_UDPLARGE_LEN + 1;
            }
            else if (BUFFER_TCPMULTI == WSASendToTable[dwTableIndex].dwBuffer) {
                WSASendToRequest.nBufferlen = BUFFER_TCPLARGE_LEN;
            }
            else if (BUFFER_UDPMULTI == WSASendToTable[dwTableIndex].dwBuffer) {
                WSASendToRequest.nBufferlen = BUFFER_UDPLARGE_LEN;
            }
            else {
                WSASendToRequest.nBufferlen = 0;
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendToRequest), (char *) &WSASendToRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == WSASendToTable[dwTableIndex].bConnect) {
                // Connect the socket
                ZeroMemory(&remotename1, sizeof(remotename1));
                remotename1.sin_family = AF_INET;
                remotename1.sin_addr.s_addr = NetsyncInAddr;
                remotename1.sin_port = htons(CurrentPort);

                connect(sSocket, (SOCKADDR *) &remotename1, sizeof(remotename1));

                if (0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&writefds);
                    FD_SET(sSocket, &writefds);
                    select(0, NULL, &writefds, NULL, NULL);
                }
            }
            else if (TRUE == WSASendToTable[dwTableIndex].bAccept) {
                if (0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&readfds);
                    FD_SET(sSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);
                }

                // Accept the socket
                nsSocket = accept(sSocket, NULL, NULL);
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendToRequest), (char *) &WSASendToRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == WSASendToTable[dwTableIndex].bFillQueue) {
                if (FALSE == bNonblocking) {
                    Nonblock = 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                }

                // Fill the queue
                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                while (SOCKET_ERROR != send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0)) {
                    WSASendToRequest.nQueueLen++;
                    Sleep(SLEEP_ZERO_TIME);
                }
            }

            if (TRUE == WSASendToTable[dwTableIndex].bRemoteClose) {
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
        if (TRUE == WSASendToTable[dwTableIndex].bShutdown) {
            shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSASendToTable[dwTableIndex].nShutdown);
        }

        // Set the remote name
        ZeroMemory(&remotename1, sizeof(remotename1));
        remotename1.sin_family = AF_INET;

        if (NAME_ANYADDR == WSASendToTable[dwTableIndex].dwName) {
            remotename1.sin_port = htons(CurrentPort);
        }
        else if (NAME_ANYPORT == WSASendToTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = NetsyncInAddr;
        }
        else if (NAME_LOCAL == WSASendToTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = hostaddr;
            remotename1.sin_port = htons(CurrentPort + 1);
        }
        else if (NAME_LOOPBACK == WSASendToTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            remotename1.sin_port = htons(CurrentPort + 1);
        }
        else if (NAME_BROADCAST == WSASendToTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = htonl(INADDR_BROADCAST);
            remotename1.sin_port = htons(CurrentPort);
        }
        else if (NAME_MULTICAST == WSASendToTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = inet_addr("224.0.0.0");
            remotename1.sin_port = htons(CurrentPort);
        }
        else if (NAME_UNAVAIL == WSASendToTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = inet_addr("127.0.0.0");
            remotename1.sin_port = htons(CurrentPort);
        }
        else if (NAME_BAD == WSASendToTable[dwTableIndex].dwName) {
            remotename1.sin_family = AF_UNIX;
            remotename1.sin_addr.s_addr = NetsyncInAddr;
            remotename1.sin_port = htons(CurrentPort);
        }
        else if (NAME_REMOTE_1 == WSASendToTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = NetsyncInAddr;
            remotename1.sin_port = htons(CurrentPort);
        }
        else if (NAME_REMOTE_2 == WSASendToTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = NetsyncInAddr;
            remotename1.sin_port = htons(CurrentPort + 1);
        }

        // Initialize the buffers
        sprintf(SendBuffer10, "%05d%05d", 1, 1);
        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
        }

        dwBufferCount = 1;
        if (BUFFER_10 == WSASendToTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = SendBuffer10;

            nBufferlen = BUFFER_10_LEN;
        }
        else if (BUFFER_NULL == WSASendToTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = NULL;

            nBufferlen = 0;
        }
        else if (BUFFER_NULLZERO == WSASendToTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = 0;
            WSABuf[0].buf = NULL;

            nBufferlen = 0;
        }
        else if (BUFFER_ZERO == WSASendToTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = 0;
            WSABuf[0].buf = SendBuffer10;

            nBufferlen = 0;
        }
        else if (BUFFER_ZEROCOUNT == WSASendToTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = 0;
            WSABuf[0].buf = SendBuffer10;

            dwBufferCount = 0;
            nBufferlen = 0;
        }
        else if (BUFFER_SMALL == WSASendToTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN - 1;
            WSABuf[0].buf = SendBuffer10;

            nBufferlen = BUFFER_10_LEN - 1;
        }
        else if (BUFFER_TCPLARGE == WSASendToTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_TCPLARGE_LEN;
            WSABuf[0].buf = SendBufferLarge;

            nBufferlen = BUFFER_TCPLARGE_LEN;
        }
        else if (BUFFER_UDPLARGE == WSASendToTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_UDPLARGE_LEN;
            WSABuf[0].buf = SendBufferLarge;

            nBufferlen = BUFFER_UDPLARGE_LEN;
        }
        else if (BUFFER_UDPTOOLARGE == WSASendToTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_UDPLARGE_LEN + 1;
            WSABuf[0].buf = SendBufferLarge;

            nBufferlen = 0;
        }
        else if (BUFFER_TCPMULTI == WSASendToTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = SendBufferLarge;

            WSABuf[1].len = 0;
            WSABuf[1].buf = NULL;

            WSABuf[2].len = BUFFER_TCPLARGE_LEN - BUFFER_10_LEN;
            WSABuf[2].buf = &SendBufferLarge[BUFFER_10_LEN];

            dwBufferCount = 3;
            nBufferlen = BUFFER_TCPLARGE_LEN;
        }
        else if (BUFFER_UDPMULTI == WSASendToTable[dwTableIndex].dwBuffer) {
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
        if (0 != (SOCKET_CLOSED & WSASendToTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        __try {
            // Call WSASendTo
            iReturnCode = WSASendTo((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (BUFFER_NONE != WSASendToTable[dwTableIndex].dwBuffer) ? WSABuf : NULL, (BUFFER_NONE != WSASendToTable[dwTableIndex].dwBuffer) ? dwBufferCount : 0, &dwBytes, WSASendToTable[dwTableIndex].nFlags, (NAME_NULL != WSASendToTable[dwTableIndex].dwName) ? (SOCKADDR *) &remotename1 : NULL, WSASendToTable[dwTableIndex].namelen, (0 == WSASendToTable[dwTableIndex].nOverlapped) ? NULL : &WSAOverlapped, (FALSE == WSASendToTable[dwTableIndex].bCompletionRoutine) ? NULL : WSASendToCompletionRoutine);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == WSASendToTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "WSASendTo RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "WSASendTo caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == WSASendToTable[dwTableIndex].bFillQueue) {
                if (0 != WSASendToTable[dwTableIndex].nOverlapped) {
                    if (SOCKET_ERROR != iReturnCode) {
                        bTestPassed = FALSE;
                        WSASendToRequest.dwMessageId = WSASENDTO_CANCEL_MSG;
                        WSASendToRequest.nDataBuffers = 0;
                    }
                    else {
                        // Get the last error code
                        iLastError = WSAGetLastError();

                        if (iLastError != WSA_IO_PENDING) {
                            xLog(hLog, XLL_FAIL, "WSASendTo iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, iLastError);
                            bTestPassed = FALSE;
                            WSASendToRequest.dwMessageId = WSASENDTO_CANCEL_MSG;
                        }
                    }

                    // Send the send cancel
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendToRequest), (char *) &WSASendToRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    if (FALSE != bTestPassed) {
                        if (OVERLAPPED_IO_EVENT == WSASendToTable[dwTableIndex].nOverlapped) {
                            // Wait on the overlapped I/O event
                            WaitForSingleObject(WSAOverlapped.hEvent, INFINITE);
                        }

                        // Get the overlapped I/O result
                        if (FALSE == WSAGetOverlappedResult((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSAOverlapped, &dwBytes, (OVERLAPPED_IO_RESULT == WSASendToTable[dwTableIndex].nOverlapped) ? TRUE : FALSE, &dwFlags)) {
                            xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        }
                        else {
                            iReturnCode = 0;
                        }
                    }
                }
                else {
                    if (SOCKET_ERROR != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "WSASendTo returned non-SOCKET_ERROR");
                    }
                    else {
                        // Get the last error code
                        iLastError = WSAGetLastError();

                        if (iLastError != WSAEWOULDBLOCK) {
                            xLog(hLog, XLL_FAIL, "WSASendTo iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                        }
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendToRequest), (char *) &WSASendToRequest);

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

                    // Call WSASendTo
                    iReturnCode = WSASendTo((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, dwBufferCount, &dwBytes, WSASendToTable[dwTableIndex].nFlags, (NAME_NULL != WSASendToTable[dwTableIndex].dwName) ? (SOCKADDR *) &remotename1 : NULL, WSASendToTable[dwTableIndex].namelen, NULL, NULL);
                }
            }

            if (-1 == WSASendToTable[dwTableIndex].nDataBuffers) {
                if (SOCKET_ERROR == iReturnCode) {
                    bTestPassed = FALSE;
                    WSASendToRequest.dwMessageId = WSASENDTO_CANCEL_MSG;
                    WSASendToRequest.nDataBuffers = 0;
                }
                else {
                    dwReturnCode = WaitForSingleObject(WSAOverlapped.hEvent, 0);
                    if (WAIT_OBJECT_0 != dwReturnCode) {
                        xLog(hLog, XLL_FAIL, "WaitForSingleObject - EXPECTED: %u; RECEIVED: %u", WAIT_OBJECT_0, dwReturnCode);
                    }

                    // Get the overlapped I/O result
                    if (FALSE == WSAGetOverlappedResult((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSAOverlapped, &dwBytes, (OVERLAPPED_IO_RESULT == WSASendToTable[dwTableIndex].nOverlapped) ? TRUE : FALSE, &dwFlags)) {
                        xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                    }
                    else {
                        iReturnCode = 0;
                    }
                }
            }

            if (TRUE == WSASendToTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "WSASendTo did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == WSASendToTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != WSASendToTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "WSASendTo iLastError - EXPECTED: %u; RECEIVED: %u", WSASendToTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "WSASendTo iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSASendTo returned SOCKET_ERROR - ec = %u", WSAGetLastError());

                if (0 != WSASendToRequest.nDataBuffers) {
                    // Send the send cancel
                    WSASendToRequest.dwMessageId = WSASENDTO_CANCEL_MSG;
                    WSASendToRequest.nDataBuffers = 0;
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendToRequest), (char *) &WSASendToRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);
                }
            }
            else if (SOCKET_ERROR == WSASendToTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSASendTo returned non-SOCKET_ERROR");
            }
            else {
                if (nBufferlen != (int) dwBytes) {
                    xLog(hLog, XLL_FAIL, "WSASendTo return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwBytes);
                    bTestPassed = FALSE;
                }

                if ((NAME_REMOTE_2 == WSASendToTable[dwTableIndex].dwName) && (0 != (SOCKET_UDP & WSASendToTable[dwTableIndex].dwSocket))) {
                    // Reset the default connection
                    ZeroMemory(&remotename2, sizeof(remotename2));
                    remotename2.sin_family = AF_INET;

                    connect((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (SOCKADDR *) &remotename2, sizeof(remotename2));
                }

                // Send the read request
                NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendToRequest), (char *) &WSASendToRequest);

                // Wait for the read complete
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                NetsyncFreeMessage(pMessage);

                if ((TRUE != WSASendToTable[dwTableIndex].bFillQueue) && (TRUE != WSASendToTable[dwTableIndex].bShutdown)) {
                    // Call recv/recvfrom
                    if (0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) {
                        if (0 != nBufferlen) {
                            ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
                            nBytes = 0;

                            do {
                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                if (1 == iReturnCode) {
                                    iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], 10, 0);
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

                        iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, sizeof(RecvBufferLarge), 0, NULL, NULL);
                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv/recvfrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else if (nBufferlen != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv/recvfrom return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, iReturnCode);
                            bTestPassed = FALSE;
                        }
                        else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                            xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                            bTestPassed = FALSE;
                        }
                    }
                }

                if (3 == WSASendToTable[dwTableIndex].nDataBuffers) {
                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                    }

                    // Switch the blocking mode
                    Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                    bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                    // Call WSASendTo
                    iReturnCode = WSASendTo((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, dwBufferCount, &dwBytes, WSASendToTable[dwTableIndex].nFlags, (NAME_NULL != WSASendToTable[dwTableIndex].dwName) ? (SOCKADDR *) &remotename1 : NULL, WSASendToTable[dwTableIndex].namelen, NULL, NULL);

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "WSASendTo returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                        WSASendToRequest.dwMessageId = WSASENDTO_CANCEL_MSG;
                    }
                    else {
                        if (nBufferlen != (int) dwBytes) {
                            xLog(hLog, XLL_FAIL, "WSASendTo return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwBytes);
                            bTestPassed = FALSE;
                        }

                        WSASendToRequest.dwMessageId = WSASENDTO_REQUEST_MSG;
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendToRequest), (char *) &WSASendToRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    if (SOCKET_ERROR != iReturnCode) {
                        // Call recv/recvfrom
                        if (0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) {
                            if (0 != nBufferlen) {
                                ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
                                nBytes = 0;

                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                    if (1 == iReturnCode) {
                                        iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], 10, 0);
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

                            iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, sizeof(RecvBufferLarge), 0, NULL, NULL);
                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv/recvfrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else if (nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv/recvfrom return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, iReturnCode);
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

                    // Call WSASendTo
                    iReturnCode = WSASendTo((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, dwBufferCount, &dwBytes, WSASendToTable[dwTableIndex].nFlags, (NAME_NULL != WSASendToTable[dwTableIndex].dwName) ? (SOCKADDR *) &remotename1 : NULL, WSASendToTable[dwTableIndex].namelen, NULL, NULL);

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "WSASendTo returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                        WSASendToRequest.dwMessageId = WSASENDTO_CANCEL_MSG;
                    }
                    else {
                        if (nBufferlen != (int) dwBytes) {
                            xLog(hLog, XLL_FAIL, "WSASendTo return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwBytes);
                            bTestPassed = FALSE;
                        }

                        WSASendToRequest.dwMessageId = WSASENDTO_REQUEST_MSG;
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendToRequest), (char *) &WSASendToRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    if (SOCKET_ERROR != iReturnCode) {
                        // Call recv/recvfrom
                        if (0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) {
                            if (0 != nBufferlen) {
                                ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
                                nBytes = 0;

                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                    if (1 == iReturnCode) {
                                        iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], 10, 0);
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

                            iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, sizeof(RecvBufferLarge), 0, NULL, NULL);
                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv/recvfrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else if (nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv/recvfrom return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                                xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                bTestPassed = FALSE;
                            }
                        }
                    }
                }
                else if (100 == WSASendToTable[dwTableIndex].nDataBuffers) {
                    for (nSendCount = 0; nSendCount < (int) (WSASendToTable[dwTableIndex].nDataBuffers * WSASendToBufferTableCount); nSendCount++) {
                        // Initialize the buffers
                        sprintf(SendBuffer10, "%05d%05d", 3, 3);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                        }

                        dwBufferCount = 1;
                        if (BUFFER_10 == WSASendToBufferTable[nSendCount % WSASendToBufferTableCount].dwBuffer) {
                            WSABuf[0].len = BUFFER_10_LEN;
                            WSABuf[0].buf = SendBuffer10;

                            nBufferlen = BUFFER_10_LEN;
                        }
                        else if (BUFFER_NULLZERO == WSASendToBufferTable[nSendCount % WSASendToBufferTableCount].dwBuffer) {
                            WSABuf[0].len = 0;
                            WSABuf[0].buf = NULL;

                            nBufferlen = 0;
                        }
                        else if (BUFFER_TCPLARGE == WSASendToBufferTable[nSendCount % WSASendToBufferTableCount].dwBuffer) {
                            WSABuf[0].len = BUFFER_TCPLARGE_LEN;
                            WSABuf[0].buf = SendBufferLarge;

                            nBufferlen = BUFFER_TCPLARGE_LEN;
                        }
                        else if (BUFFER_UDPLARGE == WSASendToBufferTable[nSendCount % WSASendToBufferTableCount].dwBuffer) {
                            WSABuf[0].len = BUFFER_UDPLARGE_LEN;
                            WSABuf[0].buf = SendBufferLarge;

                            nBufferlen = BUFFER_UDPLARGE_LEN;
                        }
                        else if (BUFFER_TCPMULTI == WSASendToBufferTable[nSendCount % WSASendToBufferTableCount].dwBuffer) {
                            WSABuf[0].len = BUFFER_10_LEN;
                            WSABuf[0].buf = SendBufferLarge;

                            WSABuf[1].len = 0;
                            WSABuf[1].buf = NULL;

                            WSABuf[2].len = BUFFER_TCPLARGE_LEN - BUFFER_10_LEN;
                            WSABuf[2].buf = &SendBufferLarge[BUFFER_10_LEN];

                            dwBufferCount = 3;
                            nBufferlen = BUFFER_TCPLARGE_LEN;
                        }
                        else if (BUFFER_UDPMULTI == WSASendToBufferTable[nSendCount % WSASendToBufferTableCount].dwBuffer) {
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

                        // Call WSASendTo
                        iReturnCode = WSASendTo((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, dwBufferCount, &dwBytes, 0, (NAME_NULL != WSASendToTable[dwTableIndex].dwName) ? (SOCKADDR *) &remotename1 : NULL, WSASendToTable[dwTableIndex].namelen, NULL, NULL);

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "WSASendTo returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                            WSASendToRequest.dwMessageId = WSASENDTO_CANCEL_MSG;
                        }
                        else {
                            if (nBufferlen != (int) dwBytes) {
                                xLog(hLog, XLL_FAIL, "WSASendTo return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwBytes);
                                bTestPassed = FALSE;
                            }

                            WSASendToRequest.dwMessageId = WSASENDTO_REQUEST_MSG;
                        }

                        // Send the read request
                        WSASendToRequest.nBufferlen = nBufferlen;
                        NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendToRequest), (char *) &WSASendToRequest);

                        // Wait for the read complete
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);

                        if (SOCKET_ERROR != iReturnCode) {
                            // Call recv/recvfrom
                            if (0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) {
                                if (0 != nBufferlen) {
                                    ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
                                    nBytes = 0;

                                    do {
                                        FD_ZERO(&readfds);
                                        FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                        iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                        if (1 == iReturnCode) {
                                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], 10, 0);
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

                                iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, sizeof(RecvBufferLarge), 0, NULL, NULL);
                                if (SOCKET_ERROR == iReturnCode) {
                                    xLog(hLog, XLL_FAIL, "recv/recvfrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                    bTestPassed = FALSE;
                                }
                                else if (nBufferlen != iReturnCode) {
                                    xLog(hLog, XLL_FAIL, "recv/recvfrom return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, iReturnCode);
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
                    xLog(hLog, XLL_PASS, "WSASendTo succeeded");
                }
            }
        }

        // Switch the blocking mode
        if (WSASendToTable[dwTableIndex].bNonblock != bNonblocking) {
            Nonblock = (TRUE == WSASendToTable[dwTableIndex].bNonblock) ? 1 : 0;
            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
            bNonblocking = WSASendToTable[dwTableIndex].bNonblock;
        }

        if ((TRUE == WSASendToTable[dwTableIndex].bAccept) || (TRUE == WSASendToTable[dwTableIndex].bConnect) || (0 == WSASendToTable[dwTableIndex].iReturnCode)) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSASendToRequest), (char *) &WSASendToRequest);
        }

        // Close the socket
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & WSASendToTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & WSASendToTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & WSASendToTable[dwTableIndex].dwSocket))) {
                shutdown(sSocket, SD_BOTH);
                closesocket(sSocket);
            }
        }

        // Increment CurrentPort
        if (0 != CurrentPort) {
            CurrentPort += 2;
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
WSASendToTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests WSASendTo - Server side

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
    u_long              FromInAddr;
    // dwMessageType is the type of received message
    DWORD               dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD               dwMessageSize;
    // pMessage is a pointer to the received message
    char                *pMessage;
    // WSASendToRequest is the request
    WSASENDTO_REQUEST   WSASendToRequest;
    // WSASendToComplete is the result
    WSASENDTO_COMPLETE  WSASendToComplete;

    // sSocket is the socket descriptor
    SOCKET              sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET              nsSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD               dwBufferSize = 1;
    // iTimeout is the send and receive timeout value for the socket
    int                 iTimeout = 5000;
    // bNagle indicates if Nagle is enabled
    BOOL                bNagle = FALSE;
    // nQueueLen is the size of the queue
    int                 nQueueLen;

    // localname is the local address
    SOCKADDR_IN         localname;
    // remotename is the remote address
    SOCKADDR_IN         remotename;
    // namelen is the size of namelen
    int                 namelen;

    // readfds is the set of sockets to check for a read condition
    fd_set              readfds;
    // timeout is the timeout for select
    timeval             fdstimeout = { 1, 0 };

    // SendBufferLarge is the large send buffer
    char                SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // nBytes is the number of bytes sent/received
    int                 nBytes;
    // nSendCount is a counter to enumerate each send
    int                 nSendCount;

    // iReturnCode is the return code of the operation
    int                 iReturnCode;



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
        CopyMemory(&WSASendToRequest, pMessage, sizeof(WSASendToRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, WSASendToRequest.nSocketType, 0);

        if (TRUE == WSASendToRequest.bFillQueue) {
            // Set the buffer size
            setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
            setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
        }

        // Set the send and receive timeout values to 5 sec
        setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));

        if (SOCK_STREAM == WSASendToRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(WSASendToRequest.ReceivePort);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if (SOCK_STREAM == WSASendToRequest.nSocketType) {
            if (TRUE == WSASendToRequest.bServerAccept) {
                // Place the socket in listening mode
                listen(sSocket, SOMAXCONN);
            }
            else {
                // Connect the socket
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = FromInAddr;
                remotename.sin_port = htons(WSASendToRequest.SendPort);

                connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
            }
        }

        // Send the complete
        WSASendToComplete.dwMessageId = WSASENDTO_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendToComplete), (char *) &WSASendToComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if ((SOCK_STREAM == WSASendToRequest.nSocketType) && (TRUE == WSASendToRequest.bServerAccept)) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        if (TRUE == WSASendToRequest.bRemoteClose) {
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
        WSASendToComplete.dwMessageId = WSASENDTO_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendToComplete), (char *) &WSASendToComplete);

        if (TRUE == WSASendToRequest.bFillQueue) {
            // Wait for the request
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            nQueueLen = ((PWSASENDTO_REQUEST) pMessage)->nQueueLen;
            CopyMemory(&WSASendToRequest, pMessage, sizeof(WSASendToRequest));
            NetsyncFreeMessage(pMessage);

            if (WSASENDTO_CANCEL_MSG != WSASendToRequest.dwMessageId) {
                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                for (nSendCount = 0; nSendCount < nQueueLen; nSendCount++) {
                    recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0);
                }
            }

            // Send the complete
            WSASendToComplete.dwMessageId = WSASENDTO_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendToComplete), (char *) &WSASendToComplete);
        }

        if ((0 < WSASendToRequest.nDataBuffers) || (-1 == WSASendToRequest.nDataBuffers)) {
            // Wait for the request
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            CopyMemory(&WSASendToRequest, pMessage, sizeof(WSASendToRequest));
            NetsyncFreeMessage(pMessage);

            if ((WSASENDTO_CANCEL_MSG != WSASendToRequest.dwMessageId) && ((0 != WSASendToRequest.nBufferlen) || (SOCK_STREAM != WSASendToRequest.nSocketType))) {
                if (SOCK_DGRAM == WSASendToRequest.nSocketType) {
                    ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                    ZeroMemory(&remotename, sizeof(remotename));
                    namelen = sizeof(remotename);

                    if (SOCKET_ERROR != recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, sizeof(SendBufferLarge), 0, (SOCKADDR *) &remotename, &namelen)) {
                        remotename.sin_addr.s_addr = FromInAddr;
                        sendto((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, strlen(SendBufferLarge), 0, (SOCKADDR *) &remotename, sizeof(remotename));
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
                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &SendBufferLarge[nBytes], 10, 0);
                        }

                        if (SOCKET_ERROR != iReturnCode) {
                            nBytes += iReturnCode;
                        }
                    } while ((SOCKET_ERROR != iReturnCode) && (nBytes < WSASendToRequest.nBufferlen));

                    if (nBytes == WSASendToRequest.nBufferlen) {
                        send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                    }
                }
            }

            // Send the complete
            WSASendToComplete.dwMessageId = WSASENDTO_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendToComplete), (char *) &WSASendToComplete);

            if (3 == WSASendToRequest.nDataBuffers) {
                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                if (WSASENDTO_CANCEL_MSG != ((PWSASENDTO_REQUEST) pMessage)->dwMessageId) {
                    if (SOCK_DGRAM == WSASendToRequest.nSocketType) {
                        ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                        ZeroMemory(&remotename, sizeof(remotename));
                        namelen = sizeof(remotename);

                        if (SOCKET_ERROR != recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, sizeof(SendBufferLarge), 0, (SOCKADDR *) &remotename, &namelen)) {
                            remotename.sin_addr.s_addr = FromInAddr;
                            sendto((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, strlen(SendBufferLarge), 0, (SOCKADDR *) &remotename, sizeof(remotename));
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
                                iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &SendBufferLarge[nBytes], 10, 0);
                            }

                            if (SOCKET_ERROR != iReturnCode) {
                                nBytes += iReturnCode;
                            }
                        } while ((SOCKET_ERROR != iReturnCode) && (nBytes < WSASendToRequest.nBufferlen));

                        if (nBytes == WSASendToRequest.nBufferlen) {
                            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                        }
                    }
                }
                NetsyncFreeMessage(pMessage);

                // Send the complete
                WSASendToComplete.dwMessageId = WSASENDTO_COMPLETE_MSG;
                NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendToComplete), (char *) &WSASendToComplete);

                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                if (WSASENDTO_CANCEL_MSG != ((PWSASENDTO_REQUEST) pMessage)->dwMessageId) {
                    if (SOCK_DGRAM == WSASendToRequest.nSocketType) {
                        ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                        ZeroMemory(&remotename, sizeof(remotename));
                        namelen = sizeof(remotename);

                        if (SOCKET_ERROR != recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, sizeof(SendBufferLarge), 0, (SOCKADDR *) &remotename, &namelen)) {
                            remotename.sin_addr.s_addr = FromInAddr;
                            sendto((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, strlen(SendBufferLarge), 0, (SOCKADDR *) &remotename, sizeof(remotename));
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
                                iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &SendBufferLarge[nBytes], 10, 0);
                            }

                            if (SOCKET_ERROR != iReturnCode) {
                                nBytes += iReturnCode;
                            }
                        } while ((SOCKET_ERROR != iReturnCode) && (nBytes < WSASendToRequest.nBufferlen));

                        if (nBytes == WSASendToRequest.nBufferlen) {
                            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                        }
                    }
                }
                NetsyncFreeMessage(pMessage);

                // Send the complete
                WSASendToComplete.dwMessageId = WSASENDTO_COMPLETE_MSG;
                NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendToComplete), (char *) &WSASendToComplete);
            }
            else if (100 <= WSASendToRequest.nDataBuffers) {
                for (nSendCount = 0; nSendCount < WSASendToRequest.nDataBuffers; nSendCount++) {
                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                    if ((0 != ((PWSASENDTO_REQUEST) pMessage)->nBufferlen) || (SOCK_STREAM != WSASendToRequest.nSocketType)) {
                        if (WSASENDTO_CANCEL_MSG != ((PWSASENDTO_REQUEST) pMessage)->dwMessageId) {
                            if (SOCK_DGRAM == WSASendToRequest.nSocketType) {
                                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                                ZeroMemory(&remotename, sizeof(remotename));
                                namelen = sizeof(remotename);

                                if (SOCKET_ERROR != recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, sizeof(SendBufferLarge), 0, (SOCKADDR *) &remotename, &namelen)) {
                                    remotename.sin_addr.s_addr = FromInAddr;
                                    sendto((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, strlen(SendBufferLarge), 0, (SOCKADDR *) &remotename, sizeof(remotename));
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
                                        iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &SendBufferLarge[nBytes], 10, 0);
                                    }

                                    if (SOCKET_ERROR != iReturnCode) {
                                        nBytes += iReturnCode;
                                    }
                                } while ((SOCKET_ERROR != iReturnCode) && (nBytes < ((PWSASENDTO_REQUEST) pMessage)->nBufferlen));

                                if (nBytes == ((PWSASENDTO_REQUEST) pMessage)->nBufferlen) {
                                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                                }
                            }
                        }
                    }
                    NetsyncFreeMessage(pMessage);

                    // Send the complete
                    WSASendToComplete.dwMessageId = WSASENDTO_COMPLETE_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSASendToComplete), (char *) &WSASendToComplete);
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
