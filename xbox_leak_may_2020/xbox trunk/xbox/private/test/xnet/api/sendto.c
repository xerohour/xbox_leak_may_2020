/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  sendto.c

Abstract:

  This modules tests sendto

Author:

  Steven Kehrli (steveke) 1-Dec-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// sendto messages

#define SENDTO_REQUEST_MSG   NETSYNC_MSG_USER + 150 + 1
#define SENDTO_CANCEL_MSG    NETSYNC_MSG_USER + 150 + 2
#define SENDTO_COMPLETE_MSG  NETSYNC_MSG_USER + 150 + 3

typedef struct _SENDTO_REQUEST {
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
} SENDTO_REQUEST, *PSENDTO_REQUEST;

typedef struct _SENDTO_COMPLETE {
    DWORD    dwMessageId;
} SENDTO_COMPLETE, *PSENDTO_COMPLETE;

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
#define BUFFER_SMALL          5
#define BUFFER_TCPLARGE       6
#define BUFFER_UDPLARGE       7
#define BUFFER_UDPTOOLARGE    8
#define BUFFER_TCPMULTI       9
#define BUFFER_UDPMULTI      10
#define BUFFER_NONE          11

#define OVERLAPPED_IO_RESULT  1
#define OVERLAPPED_IO_EVENT   2



typedef struct SENDTOBUFFER_TABLE {
    DWORD  dwBuffer;
    int    nBufferlen;
} SENDTOBUFFER_TABLE, *PSENDTOBUFFER_TABLE;

SENDTOBUFFER_TABLE SendToBufferTcpTable[] = { { BUFFER_10,    BUFFER_10_LEN       },
                                              { BUFFER_NULL,  0                   },
                                              { BUFFER_LARGE, BUFFER_TCPLARGE_LEN } };

#define SendToBufferTcpTableCount  (sizeof(SendToBufferTcpTable) / sizeof(SENDTOBUFFER_TABLE));

SENDTOBUFFER_TABLE SendToBufferUdpTable[] = { { BUFFER_10,    BUFFER_10_LEN       },
                                              { BUFFER_NULL,  0                   },
                                              { BUFFER_LARGE, BUFFER_UDPLARGE_LEN } };

#define SendToBufferUdpTableCount  (sizeof(SendToBufferUdpTable) / sizeof(SENDTOBUFFER_TABLE));



typedef struct SENDTO_TABLE {
    CHAR   szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL   bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL   bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD  dwSocket;                                // dwSocket indicates the socket to be created
    BOOL   bNonblock;                               // bNonblock indicates if the socket is to be set as non-blocking for the send
    BOOL   bBind;                                   // bBind indicates if the socket is to be bound
    BOOL   bListen;                                 // bListen indicates if the socket is to be placed in the listening state
    BOOL   bAccept;                                 // bAccept indicates if the socket is accepted
    BOOL   bConnect;                                // bConnect indicates if the socket is connected
    BOOL   bFillQueue;                              // bFillQueue indicates if the buffer should be filled
    BOOL   bRemoteClose;                            // bRemoteClose indicates if the remote closes the connection
    BOOL   bShutdown;                               // bShutdown indicates if the connection is shutdown
    int    nShutdown;                               // nShutdown indicates how the connection is shutdown
    int    nDataBuffers;                            // nDataBuffers indicates the number of data buffers to be sent
    DWORD  dwBuffer;                                // dwBuffer specifies the send buffer
    int    nBufferlen;                              // nBufferlen specifies the send data buffer length
    int    nFlags;                                  // nFlags specifies the sendto flags
    DWORD  dwName;                                  // dwName indicates the address
    int    namelen;                                 // namelen is the length of the address buffer
    int    iReturnCode;                             // iReturnCode is the return code of sendto
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} SENDTO_TABLE, *PSENDTO_TABLE;

static SENDTO_TABLE sendtoTable[] =
{
    { "15.1 Not Initialized",            FALSE, FALSE, SOCKET_INVALID_SOCKET,         FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "15.2 s = INT_MIN",                TRUE,  TRUE,  SOCKET_INT_MIN,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "15.3 s = -1",                     TRUE,  TRUE,  SOCKET_NEG_ONE,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "15.4 s = 0",                      TRUE,  TRUE,  SOCKET_ZERO,                   FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "15.5 s = INT_MAX",                TRUE,  TRUE,  SOCKET_INT_MAX,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "15.6 s = INVALID_SOCKET",         TRUE,  TRUE,  SOCKET_INVALID_SOCKET,         FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "15.7 Not Bound TCP",              TRUE,  TRUE,  SOCKET_TCP,                    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "15.8 Listening TCP",              TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "15.9 Accepted TCP",               TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.10 Connected TCP",             TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.11 Fill Queue TCP",            TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.12 SD_RECEIVE Accept TCP",     TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.13 SD_SEND Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "15.14 SD_BOTH Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "15.15 SD_RECEIVE Connect TCP",    TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.16 SD_SEND Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "15.17 SD_BOTH Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "15.18 Close Accept TCP",          TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "15.19 Close Connect TCP",         TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "15.20 NULL Buffer TCP",           TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.21 Neg Bufferlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    -1,                      0, NAME_NULL,      0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.22 NULL 0 Bufferlen TCP",      TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.23 0 Bufferlen TCP",           TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    0,                       0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.24 Small Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN - 1,       0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.25 Exact Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.26 Large Buffer TCP",          TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_TCPLARGE_LEN,     0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.27 Switch Send TCP",           TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.28 Iterative Send TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.29 flags != 0 TCP",            TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           4, NAME_NULL,      0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.30 Same addr TCP",             TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.31 Different addr TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.32 Broadcast addr TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_BROADCAST, sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.33 Multicast addr TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_MULTICAST, sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.34 Bad addr TCP",              TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_BAD,       sizeof(SOCKADDR_IN),     0,            WSAEFAULT,         TRUE  },
    { "15.35 Zero addr TCP",             TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_ANY,       sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.36 Any addr TCP",              TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.37 Zero port addr TCP",        TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_ANYPORT,   sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.38 Large addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "15.39 Exact addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.40 Small addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) - 1, 0,            WSAEFAULT,         TRUE  },
    { "15.41 Zero addrlen TCP",          TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  0,                       0,            WSAEFAULT,         TRUE  },
    { "15.42 Neg addrlen TCP",           TRUE,  TRUE,  SOCKET_TCP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  -1,                      0,            WSAEFAULT,         TRUE  },
    { "15.43 Not Bound NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "15.44 Listening NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "15.45 Accepted NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.46 Connected NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.47 Fill Queue NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.48 SD_RECEIVE Accept NB TCP",  TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.49 SD_SEND Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "15.50 SD_BOTH Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "15.51 SD_RECEIVE Connect NB TCP", TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.52 SD_SEND Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "15.53 SD_BOTH Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "15.54 Close Accept NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "15.55 Close Connect NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "15.56 NULL Buffer NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.57 Neg Bufferlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    -1,                      0, NAME_NULL,      0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.58 NULL 0 Bufferlen NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.59 0 Bufferlen NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    0,                       0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.60 Small Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN - 1,       0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.61 Exact Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.62 Large Buffer NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_TCPLARGE_LEN,     0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.63 Switch Send NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.64 Iterative Send NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.65 flags != 0 NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           4, NAME_NULL,      0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.66 Same addr NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.67 Different addr NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_2,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.68 Broadcast addr NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_BROADCAST, sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.69 Multicast addr NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_MULTICAST, sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.70 Bad addr NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_BAD,       sizeof(SOCKADDR_IN),     0,            WSAEFAULT,         TRUE  },
    { "15.71 Zero addr NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_ANY,       sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.72 Any addr NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.73 Zero port addr NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_ANYPORT,   sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.74 Large addrlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "15.75 Exact addrlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.76 Small addrlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) - 1, 0,            WSAEFAULT,         TRUE  },
    { "15.77 Zero addrlen NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  0,                       0,            WSAEFAULT,         TRUE  },
    { "15.78 Neg addrlen NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  -1,                      0,            WSAEFAULT,         TRUE  },
    { "15.79 Not Bound UDP",             TRUE,  TRUE,  SOCKET_UDP,                    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "15.80 Not Connected UDP",         TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "15.81 Connected UDP",             TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.82 SD_RECEIVE Conn UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.83 SD_SEND Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "15.84 SD_BOTH Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "15.85 NULL Buffer UDP",           TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.86 Neg Bufferlen UDP",         TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    -1,                      0, NAME_NULL,      0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.87 NULL 0 Bufferlen UDP",      TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.88 0 Bufferlen UDP",           TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    0,                       0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.89 Small Bufferlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN - 1,       0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.90 Exact Bufferlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.91 Large Buffer UDP",          TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_UDPLARGE_LEN,     0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.92 Too Large Buffer UDP",      TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_UDPLARGE_LEN + 1, 0, NAME_NULL,      0,                       SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "15.93 Switch Send UDP",           TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.94 Iterative Send UDP",        TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.95 flags != 0 UDP",            TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           4, NAME_NULL,      0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.96 Not Bound Addr UDP",        TRUE,  TRUE,  SOCKET_UDP,                    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.97 Bound Addr UDP",            TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.98 Same addr UDP",             TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.99 Different addr UDP",        TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.100 Broadcast addr UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_BROADCAST, sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEACCES,         FALSE },
    { "15.101 Broadcast enabled UDP",    TRUE,  TRUE,  SOCKET_UDP | SOCKET_BROADCAST, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_BROADCAST, sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.102 Multicast addr UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_MULTICAST, sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "15.103 Bad addr UDP",             TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_BAD,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.104 Zero addr UDP",            TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_ANY,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "15.105 Any addr UDP",             TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "15.106 Zero port addr UDP",       TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_ANYPORT,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "15.107 Large addrlen UDP",        TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "15.108 Exact addrlen UDP",        TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.109 Small addrlen UDP",        TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) - 1, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.110 Zero addrlen UDP",         TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.111 Neg addrlen UDP",          TRUE,  TRUE,  SOCKET_UDP,                    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  -1,                      SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.112 Not Bound NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "15.113 Not Connected NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "15.114 Connected NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.115 SD_RECEIVE Conn NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.116 SD_SEND Conn NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "15.117 SD_BOTH Conn NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "15.118 NULL Buffer NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  BUFFER_10_LEN,           0, NAME_NULL,      0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.119 Neg Bufferlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    -1,                      0, NAME_NULL,      0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.120 NULL 0 Bufferlen NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.121 0 Bufferlen NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    0,                       0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.122 Small Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN - 1,       0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.123 Exact Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.124 Large Buffer NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_UDPLARGE_LEN,     0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.125 Too Large Buffer NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_UDPLARGE_LEN + 1, 0, NAME_NULL,      0,                       SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "15.126 Switch Send NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.127 Iterative Send NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,    BUFFER_10_LEN,           0, NAME_NULL,      0,                       0,            0,                 FALSE },
    { "15.128 flags != 0 NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           4, NAME_NULL,      0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.129 Not Bound Addr NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.130 Bound Addr NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.131 Same addr NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.132 Different addr NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_2,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.133 Broadcast addr NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_BROADCAST, sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEACCES,         FALSE },
    { "15.134 Broadcast enabled NB UDP", TRUE,  TRUE,  SOCKET_UDP | SOCKET_BROADCAST, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_BROADCAST, sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.135 Multicast addr NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_MULTICAST, sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "15.136 Bad addr NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_BAD,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.137 Zero addr NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_ANY,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "15.138 Any addr NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "15.139 Zero port addr NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_ANYPORT,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "15.140 Large addrlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "15.141 Exact addrlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "15.142 Small addrlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  sizeof(SOCKADDR_IN) - 1, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.143 Zero addrlen NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  0,                       SOCKET_ERROR, WSAEFAULT,         TRUE },
    { "15.144 Neg addrlen NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, NAME_REMOTE_1,  -1,                      SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "15.145 Closed Socket TCP",        TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "15.146 Closed Socket UDP",        TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "15.147 Not Initialized",          FALSE, FALSE, SOCKET_INVALID_SOCKET,         FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, NAME_NULL,      0,                       SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define sendtoTableCount (sizeof(sendtoTable) / sizeof(SENDTO_TABLE))

NETSYNC_TYPE_THREAD  sendtoTestSessionNt =
{
    1,
    sendtoTableCount * 2,
    L"xnetapi_nt.dll",
    "sendtoTestServer"
};

NETSYNC_TYPE_THREAD  sendtoTestSessionXbox =
{
    1,
    sendtoTableCount * 2,
    L"xnetapi_xbox.dll",
    "sendtoTestServer"
};



VOID
sendtoTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests sendto - Client side

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
    // sendtoRequest is the request sent to the server
    SENDTO_REQUEST         sendtoRequest;

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
    // Buffer is a pointer to the buffer
    char                   *Buffer;
    // dwFillBuffer is a counter to fill the buffers
    DWORD                  dwFillBuffer;
    // nBytes is the number of bytes sent
    int                    nBytes;
    // nSendCount is a counter to enumerate each send/recv
    int                    nSendCount;

    // SendToBufferTable is the test buffer table to use for iterative send
    PSENDTOBUFFER_TABLE    SendToBufferTable;
    // SendToBufferTableCount is the number of elements within the test buffer table
    size_t                 SendToBufferTableCount;

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
    sprintf(szFunctionName, "sendto v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_sendto+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_sendto-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = sendtoTestSessionXbox;
    }
    else {
        NetsyncTypeSession = sendtoTestSessionNt;
    }

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

    for (dwTableIndex = 0; dwTableIndex < sendtoTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, sendtoTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, sendtoTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != sendtoTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, sendtoTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == sendtoTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != sendtoTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == sendtoTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = sendtoTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == sendtoTable[dwTableIndex].bNetsyncConnected)) {
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
        if (SOCKET_INT_MIN == sendtoTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == sendtoTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == sendtoTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == sendtoTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == sendtoTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & sendtoTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Set the socket for broadcast
        if (0 != (SOCKET_BROADCAST & sendtoTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, SOL_SOCKET, SO_BROADCAST, (char *) &bBroadcast, sizeof(bBroadcast));
        }

        if (TRUE == sendtoTable[dwTableIndex].bFillQueue) {
            // Set the buffer size
            setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
            setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
        }

        // Set the send and receive timeout values to 5 sec
        if ((0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & sendtoTable[dwTableIndex].dwSocket))) {
            setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
            setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the socket to non-blocking mode
        if (TRUE == sendtoTable[dwTableIndex].bNonblock) {
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);
            bNonblocking = TRUE;
        }
        else {
            bNonblocking = FALSE;
        }

        // Bind the socket
        if (TRUE == sendtoTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Place the socket in the listening state
        if (TRUE == sendtoTable[dwTableIndex].bListen) {
            listen(sSocket, SOMAXCONN);
        }

        if (0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) {
            SendToBufferTable = SendToBufferTcpTable;
            SendToBufferTableCount = SendToBufferTcpTableCount;
        }
        else {
            SendToBufferTable = SendToBufferUdpTable;
            SendToBufferTableCount = SendToBufferUdpTableCount;
        }

        if ((TRUE == sendtoTable[dwTableIndex].bAccept) || (TRUE == sendtoTable[dwTableIndex].bConnect) || (0 == sendtoTable[dwTableIndex].iReturnCode)) {
            // Initialize the sendto request
            sendtoRequest.dwMessageId = SENDTO_REQUEST_MSG;
            if (0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) {
                sendtoRequest.nSocketType = SOCK_STREAM;
            }
            else {
                sendtoRequest.nSocketType = SOCK_DGRAM;
            }
            sendtoRequest.ReceivePort = (NAME_REMOTE_2 != sendtoTable[dwTableIndex].dwName) || (0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) ? CurrentPort : CurrentPort + 1;
            sendtoRequest.SendPort = (TRUE == sendtoTable[dwTableIndex].bBind) ? CurrentPort : 0;
            sendtoRequest.bServerAccept = sendtoTable[dwTableIndex].bConnect;
            sendtoRequest.bFillQueue = sendtoTable[dwTableIndex].bFillQueue;
            sendtoRequest.nQueueLen = 0;
            sendtoRequest.bRemoteClose = sendtoTable[dwTableIndex].bRemoteClose;
            if (100 == sendtoTable[dwTableIndex].nDataBuffers) {
                sendtoRequest.nDataBuffers = sendtoTable[dwTableIndex].nDataBuffers * SendToBufferTableCount;
            }
            else {
                sendtoRequest.nDataBuffers = (0 == sendtoTable[dwTableIndex].iReturnCode) ? sendtoTable[dwTableIndex].nDataBuffers : 0;
            }
            sendtoRequest.nBufferlen = sendtoTable[dwTableIndex].nBufferlen;

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendtoRequest), (char *) &sendtoRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == sendtoTable[dwTableIndex].bConnect) {
                // Connect the socket
                ZeroMemory(&remotename1, sizeof(remotename1));
                remotename1.sin_family = AF_INET;
                remotename1.sin_addr.s_addr = NetsyncInAddr;
                remotename1.sin_port = htons(CurrentPort);

                connect(sSocket, (SOCKADDR *) &remotename1, sizeof(remotename1));

                if (0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&writefds);
                    FD_SET(sSocket, &writefds);
                    select(0, NULL, &writefds, NULL, NULL);
                }
            }
            else if (TRUE == sendtoTable[dwTableIndex].bAccept) {
                if (0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&readfds);
                    FD_SET(sSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);
                }

                // Accept the socket
                nsSocket = accept(sSocket, NULL, NULL);
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendtoRequest), (char *) &sendtoRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == sendtoTable[dwTableIndex].bFillQueue) {
                if (FALSE == bNonblocking) {
                    Nonblock = 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                }

                // Fill the queue
                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                while (SOCKET_ERROR != send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0)) {
                    sendtoRequest.nQueueLen++;
                    Sleep(SLEEP_ZERO_TIME);
                }
            }

            if (TRUE == sendtoTable[dwTableIndex].bRemoteClose) {
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
        if (TRUE == sendtoTable[dwTableIndex].bShutdown) {
            shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, sendtoTable[dwTableIndex].nShutdown);
        }

        // Set the remote name
        ZeroMemory(&remotename1, sizeof(remotename1));
        remotename1.sin_family = AF_INET;

        if (NAME_ANYADDR == sendtoTable[dwTableIndex].dwName) {
            remotename1.sin_port = htons(CurrentPort);
        }
        else if (NAME_ANYPORT == sendtoTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = NetsyncInAddr;
        }
        else if (NAME_LOCAL == sendtoTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = hostaddr;
            remotename1.sin_port = htons(CurrentPort + 1);
        }
        else if (NAME_LOOPBACK == sendtoTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            remotename1.sin_port = htons(CurrentPort + 1);
        }
        else if (NAME_BROADCAST == sendtoTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = htonl(INADDR_BROADCAST);
            remotename1.sin_port = htons(CurrentPort);
        }
        else if (NAME_MULTICAST == sendtoTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = inet_addr("224.0.0.0");
            remotename1.sin_port = htons(CurrentPort);
        }
        else if (NAME_UNAVAIL == sendtoTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = inet_addr("127.0.0.0");
            remotename1.sin_port = htons(CurrentPort);
        }
        else if (NAME_BAD == sendtoTable[dwTableIndex].dwName) {
            remotename1.sin_family = AF_UNIX;
            remotename1.sin_addr.s_addr = NetsyncInAddr;
            remotename1.sin_port = htons(CurrentPort);
        }
        else if (NAME_REMOTE_1 == sendtoTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = NetsyncInAddr;
            remotename1.sin_port = htons(CurrentPort);
        }
        else if (NAME_REMOTE_2 == sendtoTable[dwTableIndex].dwName) {
            remotename1.sin_addr.s_addr = NetsyncInAddr;
            remotename1.sin_port = htons(CurrentPort + 1);
        }

        // Initialize the buffers
        sprintf(SendBuffer10, "%05d%05d", 1, 1);
        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
        }
        if (BUFFER_10 == sendtoTable[dwTableIndex].dwBuffer) {
            Buffer = SendBuffer10;
        }
        else if (BUFFER_LARGE == sendtoTable[dwTableIndex].dwBuffer) {
            Buffer = SendBufferLarge;
        }
        else {
            Buffer = NULL;
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & sendtoTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        __try {
            // Call sendto
            iReturnCode = sendto((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, Buffer, sendtoTable[dwTableIndex].nBufferlen, sendtoTable[dwTableIndex].nFlags, (NAME_NULL != sendtoTable[dwTableIndex].dwName) ? (SOCKADDR *) &remotename1 : NULL, sendtoTable[dwTableIndex].namelen);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == sendtoTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "sendto RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "sendto caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == sendtoTable[dwTableIndex].bFillQueue) {
                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "sendto returned non-SOCKET_ERROR");
                }
                else {
                    // Get the last error code
                    iLastError = WSAGetLastError();

                    if (iLastError != WSAEWOULDBLOCK) {
                        xLog(hLog, XLL_FAIL, "sendto iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                    }
                }

                // Send the read request
                NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendtoRequest), (char *) &sendtoRequest);

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

                // Call sendto
                iReturnCode = sendto((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, Buffer, sendtoTable[dwTableIndex].nBufferlen, sendtoTable[dwTableIndex].nFlags, (NAME_NULL != sendtoTable[dwTableIndex].dwName) ? (SOCKADDR *) &remotename1 : NULL, sendtoTable[dwTableIndex].namelen);
            }

            if (TRUE == sendtoTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "sendto did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == sendtoTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != sendtoTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "sendto iLastError - EXPECTED: %u; RECEIVED: %u", sendtoTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "sendto iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "sendto returned SOCKET_ERROR - ec = %u", WSAGetLastError());

                if (0 != sendtoRequest.nDataBuffers) {
                    // Send the send cancel
                    sendtoRequest.dwMessageId = SENDTO_CANCEL_MSG;
                    sendtoRequest.nDataBuffers = 0;
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendtoRequest), (char *) &sendtoRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);
                }
            }
            else if (SOCKET_ERROR == sendtoTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "sendto returned non-SOCKET_ERROR");
            }
            else {
                if (sendtoTable[dwTableIndex].nBufferlen != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "sendto return value - EXPECTED: %d; RECEIVED: %d", sendtoTable[dwTableIndex].nBufferlen, iReturnCode);
                    bTestPassed = FALSE;
                }

                if ((NAME_REMOTE_2 == sendtoTable[dwTableIndex].dwName) && (0 != (SOCKET_UDP & sendtoTable[dwTableIndex].dwSocket))) {
                    // Reset the default connection
                    ZeroMemory(&remotename2, sizeof(remotename2));
                    remotename2.sin_family = AF_INET;

                    connect(sSocket, (SOCKADDR *) &remotename2, sizeof(remotename2));
                }

                // Send the read request
                NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendtoRequest), (char *) &sendtoRequest);

                // Wait for the read complete
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                NetsyncFreeMessage(pMessage);

                if ((TRUE != sendtoTable[dwTableIndex].bFillQueue) && (TRUE != sendtoTable[dwTableIndex].bShutdown)) {
                    // Call recv/recvfrom
                    if (0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) {
                        if (0 != sendtoTable[dwTableIndex].nBufferlen) {
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
                            else if (sendtoTable[dwTableIndex].nBufferlen != nBytes) {
                                xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", sendtoTable[dwTableIndex].nBufferlen, nBytes);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(RecvBufferLarge, Buffer, sendtoTable[dwTableIndex].nBufferlen)) {
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
                        else if (sendtoTable[dwTableIndex].nBufferlen != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv/recvfrom return value - EXPECTED: %d; RECEIVED: %d", sendtoTable[dwTableIndex].nBufferlen, iReturnCode);
                            bTestPassed = FALSE;
                        }
                        else if (0 != strncmp(RecvBufferLarge, Buffer, sendtoTable[dwTableIndex].nBufferlen)) {
                            xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                            bTestPassed = FALSE;
                        }
                    }
                }

                if (3 == sendtoTable[dwTableIndex].nDataBuffers) {
                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                    }
                    if (BUFFER_10 == sendtoTable[dwTableIndex].dwBuffer) {
                        Buffer = SendBuffer10;
                    }
                    else if (BUFFER_LARGE == sendtoTable[dwTableIndex].dwBuffer) {
                        Buffer = SendBufferLarge;
                    }
                    else {
                        Buffer = NULL;
                    }

                    // Switch the blocking mode
                    Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                    bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                    // Call sendto
                    iReturnCode = sendto((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, Buffer, sendtoTable[dwTableIndex].nBufferlen, sendtoTable[dwTableIndex].nFlags, (NAME_NULL != sendtoTable[dwTableIndex].dwName) ? (SOCKADDR *) &remotename1 : NULL, sendtoTable[dwTableIndex].namelen);

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "sendto returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                        sendtoRequest.dwMessageId = SENDTO_CANCEL_MSG;
                    }
                    else {
                        if (sendtoTable[dwTableIndex].nBufferlen != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "sendto return value - EXPECTED: %d; RECEIVED: %d", sendtoTable[dwTableIndex].nBufferlen, iReturnCode);
                            bTestPassed = FALSE;
                        }

                        sendtoRequest.dwMessageId = SENDTO_REQUEST_MSG;
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendtoRequest), (char *) &sendtoRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    if (SOCKET_ERROR != iReturnCode) {
                        // Call recv/recvfrom
                        if (0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) {
                            if (0 != sendtoTable[dwTableIndex].nBufferlen) {
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
                                else if (sendtoTable[dwTableIndex].nBufferlen != nBytes) {
                                    xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", sendtoTable[dwTableIndex].nBufferlen, nBytes);
                                    bTestPassed = FALSE;
                                }
                                else if (0 != strncmp(RecvBufferLarge, Buffer, sendtoTable[dwTableIndex].nBufferlen)) {
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
                            else if (sendtoTable[dwTableIndex].nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv/recvfrom return value - EXPECTED: %d; RECEIVED: %d", sendtoTable[dwTableIndex].nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(RecvBufferLarge, Buffer, sendtoTable[dwTableIndex].nBufferlen)) {
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
                    if (BUFFER_10 == sendtoTable[dwTableIndex].dwBuffer) {
                        Buffer = SendBuffer10;
                    }
                    else if (BUFFER_LARGE == sendtoTable[dwTableIndex].dwBuffer) {
                        Buffer = SendBufferLarge;
                    }
                    else {
                        Buffer = NULL;
                    }

                    // Switch the blocking mode
                    Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                    bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                    // Call sendto
                    iReturnCode = sendto((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, Buffer, sendtoTable[dwTableIndex].nBufferlen, sendtoTable[dwTableIndex].nFlags, (NAME_NULL != sendtoTable[dwTableIndex].dwName) ? (SOCKADDR *) &remotename1 : NULL, sendtoTable[dwTableIndex].namelen);

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "sendto returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                        sendtoRequest.dwMessageId = SENDTO_CANCEL_MSG;
                    }
                    else {
                        if (sendtoTable[dwTableIndex].nBufferlen != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "sendto return value - EXPECTED: %d; RECEIVED: %d", sendtoTable[dwTableIndex].nBufferlen, iReturnCode);
                            bTestPassed = FALSE;
                        }

                        sendtoRequest.dwMessageId = SENDTO_REQUEST_MSG;
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendtoRequest), (char *) &sendtoRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    if (SOCKET_ERROR != iReturnCode) {
                        // Call recv/recvfrom
                        if (0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) {
                            if (0 != sendtoTable[dwTableIndex].nBufferlen) {
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
                                else if (sendtoTable[dwTableIndex].nBufferlen != nBytes) {
                                    xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", sendtoTable[dwTableIndex].nBufferlen, nBytes);
                                    bTestPassed = FALSE;
                                }
                                else if (0 != strncmp(RecvBufferLarge, Buffer, sendtoTable[dwTableIndex].nBufferlen)) {
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
                            else if (sendtoTable[dwTableIndex].nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv/recvfrom return value - EXPECTED: %d; RECEIVED: %d", sendtoTable[dwTableIndex].nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(RecvBufferLarge, Buffer, sendtoTable[dwTableIndex].nBufferlen)) {
                                xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                bTestPassed = FALSE;
                            }
                        }
                    }
                }
                else if (100 == sendtoTable[dwTableIndex].nDataBuffers) {
                    for (nSendCount = 0; nSendCount < (int) (sendtoTable[dwTableIndex].nDataBuffers * SendToBufferTableCount); nSendCount++) {
                        // Initialize the buffers
                        sprintf(SendBuffer10, "%05d%05d", 2 + nSendCount, 2 + nSendCount);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                        }
                        if (BUFFER_LARGE == SendToBufferTable[nSendCount % SendToBufferTableCount].dwBuffer) {
                            Buffer = SendBufferLarge;
                        }
                        else if (BUFFER_10 == SendToBufferTable[nSendCount % SendToBufferTableCount].dwBuffer) {
                            Buffer = SendBuffer10;
                        }
                        else {
                            Buffer = NULL;
                        }

                        xLog(hLog, XLL_INFO, "Iteration %d", nSendCount);

                        // Call sendto
                        iReturnCode = sendto((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, Buffer, SendToBufferTable[nSendCount % SendToBufferTableCount].nBufferlen, 0, (NAME_NULL != sendtoTable[dwTableIndex].dwName) ? (SOCKADDR *) &remotename1 : NULL, sendtoTable[dwTableIndex].namelen);

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "sendto returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                            sendtoRequest.dwMessageId = SENDTO_CANCEL_MSG;
                        }
                        else {
                            if (SendToBufferTable[nSendCount % SendToBufferTableCount].nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "sendto return value - EXPECTED: %d; RECEIVED: %d", SendToBufferTable[nSendCount % SendToBufferTableCount].nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }

                            sendtoRequest.dwMessageId = SENDTO_REQUEST_MSG;
                        }

                        // Send the read request
                        sendtoRequest.nBufferlen = SendToBufferTable[nSendCount % SendToBufferTableCount].nBufferlen;
                        NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendtoRequest), (char *) &sendtoRequest);

                        // Wait for the read complete
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);

                        if (SOCKET_ERROR != iReturnCode) {
                            // Call recv/recvfrom
                            if (0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) {
                                if (0 != SendToBufferTable[nSendCount % SendToBufferTableCount].nBufferlen) {
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
                                    else if (SendToBufferTable[nSendCount % SendToBufferTableCount].nBufferlen != nBytes) {
                                        xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", SendToBufferTable[nSendCount % SendToBufferTableCount].nBufferlen, nBytes);
                                        bTestPassed = FALSE;
                                    }
                                    else if (0 != strncmp(RecvBufferLarge, Buffer, SendToBufferTable[nSendCount % SendToBufferTableCount].nBufferlen)) {
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
                                else if (SendToBufferTable[nSendCount % SendToBufferTableCount].nBufferlen != iReturnCode) {
                                    xLog(hLog, XLL_FAIL, "recv/recvfrom return value - EXPECTED: %d; RECEIVED: %d", SendToBufferTable[nSendCount % SendToBufferTableCount].nBufferlen, iReturnCode);
                                    bTestPassed = FALSE;
                                }
                                else if (0 != strncmp(RecvBufferLarge, Buffer, SendToBufferTable[nSendCount % SendToBufferTableCount].nBufferlen)) {
                                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "sendto succeeded");
                }
            }
        }

        // Switch the blocking mode
        if (sendtoTable[dwTableIndex].bNonblock != bNonblocking) {
            Nonblock = (TRUE == sendtoTable[dwTableIndex].bNonblock) ? 1 : 0;
            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
            bNonblocking = sendtoTable[dwTableIndex].bNonblock;
        }

        if ((TRUE == sendtoTable[dwTableIndex].bAccept) || (TRUE == sendtoTable[dwTableIndex].bConnect) || (0 == sendtoTable[dwTableIndex].iReturnCode)) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendtoRequest), (char *) &sendtoRequest);
        }

        // Close the socket
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & sendtoTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & sendtoTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & sendtoTable[dwTableIndex].dwSocket))) {
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
sendtoTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests sendto - Server side

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
    u_long           FromInAddr;
    // dwMessageType is the type of received message
    DWORD            dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD            dwMessageSize;
    // pMessage is a pointer to the received message
    char             *pMessage;
    // sendtoRequest is the request
    SENDTO_REQUEST   sendtoRequest;
    // sendtoComplete is the result
    SENDTO_COMPLETE  sendtoComplete;

    // sSocket is the socket descriptor
    SOCKET           sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET           nsSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD            dwBufferSize = 1;
    // iTimeout is the send and receive timeout value for the socket
    int              iTimeout = 5000;
    // bNagle indicates if Nagle is enabled
    BOOL             bNagle = FALSE;
    // nQueueLen is the size of the queue
    int              nQueueLen;

    // localname is the local address
    SOCKADDR_IN      localname;
    // remotename is the remote address
    SOCKADDR_IN      remotename;
    // namelen is the size of namelen
    int              namelen;

    // readfds is the set of sockets to check for a read condition
    fd_set           readfds;
    // timeout is the timeout for select
    timeval          fdstimeout = { 1, 0 };

    // SendBufferLarge is the large send buffer
    char             SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // nBytes is the number of bytes sent/received
    int              nBytes;
    // nSendCount is a counter to enumerate each send
    int              nSendCount;

    // iReturnCode is the return code of the operation
    int              iReturnCode;



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
        CopyMemory(&sendtoRequest, pMessage, sizeof(sendtoRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, sendtoRequest.nSocketType, 0);

        if (TRUE == sendtoRequest.bFillQueue) {
            // Set the buffer size
            setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
            setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
        }

        // Set the send and receive timeout values to 5 sec
        setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));

        if (SOCK_STREAM == sendtoRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(sendtoRequest.ReceivePort);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if (SOCK_STREAM == sendtoRequest.nSocketType) {
            if (TRUE == sendtoRequest.bServerAccept) {
                // Place the socket in listening mode
                listen(sSocket, SOMAXCONN);
            }
            else {
                // Connect the socket
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = FromInAddr;
                remotename.sin_port = htons(sendtoRequest.SendPort);

                connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
            }
        }

        // Send the complete
        sendtoComplete.dwMessageId = SENDTO_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendtoComplete), (char *) &sendtoComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if ((SOCK_STREAM == sendtoRequest.nSocketType) && (TRUE == sendtoRequest.bServerAccept)) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        if (TRUE == sendtoRequest.bRemoteClose) {
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
        sendtoComplete.dwMessageId = SENDTO_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendtoComplete), (char *) &sendtoComplete);

        if (TRUE == sendtoRequest.bFillQueue) {
            // Wait for the request
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            nQueueLen = ((PSENDTO_REQUEST) pMessage)->nQueueLen;
            CopyMemory(&sendtoRequest, pMessage, sizeof(sendtoRequest));
            NetsyncFreeMessage(pMessage);

            if (SENDTO_CANCEL_MSG != sendtoRequest.dwMessageId) {
                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                for (nSendCount = 0; nSendCount < nQueueLen; nSendCount++) {
                    recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0);
                }
            }

            // Send the complete
            sendtoComplete.dwMessageId = SENDTO_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendtoComplete), (char *) &sendtoComplete);
        }

        if (0 < sendtoRequest.nDataBuffers) {
            // Wait for the request
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            CopyMemory(&sendtoRequest, pMessage, sizeof(sendtoRequest));
            NetsyncFreeMessage(pMessage);

            if ((SENDTO_CANCEL_MSG != sendtoRequest.dwMessageId) && ((0 != sendtoRequest.nBufferlen) || (SOCK_STREAM != sendtoRequest.nSocketType))) {
                if (SOCK_DGRAM == sendtoRequest.nSocketType) {
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
                    } while ((SOCKET_ERROR != iReturnCode) && (nBytes < sendtoRequest.nBufferlen));

                    if (nBytes == sendtoRequest.nBufferlen) {
                        send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                    }
                }
            }

            // Send the complete
            sendtoComplete.dwMessageId = SENDTO_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendtoComplete), (char *) &sendtoComplete);

            if (3 == sendtoRequest.nDataBuffers) {
                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                if (SENDTO_CANCEL_MSG != ((PSENDTO_REQUEST) pMessage)->dwMessageId) {
                    if (SOCK_DGRAM == sendtoRequest.nSocketType) {
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
                        } while ((SOCKET_ERROR != iReturnCode) && (nBytes < sendtoRequest.nBufferlen));

                        if (nBytes == sendtoRequest.nBufferlen) {
                            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                        }
                    }
                }
                NetsyncFreeMessage(pMessage);

                // Send the complete
                sendtoComplete.dwMessageId = SENDTO_COMPLETE_MSG;
                NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendtoComplete), (char *) &sendtoComplete);

                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                if (SENDTO_CANCEL_MSG != ((PSENDTO_REQUEST) pMessage)->dwMessageId) {
                    if (SOCK_DGRAM == sendtoRequest.nSocketType) {
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
                        } while ((SOCKET_ERROR != iReturnCode) && (nBytes < sendtoRequest.nBufferlen));

                        if (nBytes == sendtoRequest.nBufferlen) {
                            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                        }
                    }
                }
                NetsyncFreeMessage(pMessage);

                // Send the complete
                sendtoComplete.dwMessageId = SENDTO_COMPLETE_MSG;
                NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendtoComplete), (char *) &sendtoComplete);
            }
            else if (100 <= sendtoRequest.nDataBuffers) {
                for (nSendCount = 0; nSendCount < sendtoRequest.nDataBuffers; nSendCount++) {
                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                    if ((0 != ((PSENDTO_REQUEST) pMessage)->nBufferlen) || (SOCK_STREAM != sendtoRequest.nSocketType)) {
                        if (SENDTO_CANCEL_MSG != ((PSENDTO_REQUEST) pMessage)->dwMessageId) {
                            if (SOCK_DGRAM == sendtoRequest.nSocketType) {
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
                                } while ((SOCKET_ERROR != iReturnCode) && (nBytes < ((PSENDTO_REQUEST) pMessage)->nBufferlen));

                                if (nBytes == ((PSENDTO_REQUEST) pMessage)->nBufferlen) {
                                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                                }
                            }
                        }
                    }
                    NetsyncFreeMessage(pMessage);

                    // Send the complete
                    sendtoComplete.dwMessageId = SENDTO_COMPLETE_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendtoComplete), (char *) &sendtoComplete);
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
