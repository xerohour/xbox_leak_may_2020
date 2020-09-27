/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  recv.c

Abstract:

  This modules tests recv

Author:

  Steven Kehrli (steveke) 13-Dev-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// recv messages

#define RECV_REQUEST_MSG   NETSYNC_MSG_USER + 170 + 1
#define RECV_CANCEL_MSG    NETSYNC_MSG_USER + 170 + 2
#define RECV_COMPLETE_MSG  NETSYNC_MSG_USER + 170 + 3

typedef struct _RECV_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  Port;
    BOOL     bServerAccept;
    BOOL     bRemoteShutdown;
    BOOL     bRemoteClose;
    int      nDataBuffers;
    int      nBufferlen;
} RECV_REQUEST, *PRECV_REQUEST;

typedef struct _RECV_COMPLETE {
    DWORD    dwMessageId;
} RECV_COMPLETE, *PRECV_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

#define BUFFER_10             0
#define BUFFER_LARGE          1
#define BUFFER_NULL           2
#define BUFFER_NULLZERO       3
#define BUFFER_ZERO           4
#define BUFFER_SMALL          5
#define BUFFER_TCPLARGE       6
#define BUFFER_UDPLARGE       7
#define BUFFER_UDPTOOLARGE    8
#define BUFFER_MULTI          9
#define BUFFER_SECOND        10
#define BUFFER_NONE          11

#define OVERLAPPED_IO_RESULT  1
#define OVERLAPPED_IO_EVENT   2



typedef struct RECVBUFFER_TABLE {
    int   nBufferlen;
} RECVBUFFER_TABLE, *PRECVBUFFER_TABLE;

RECVBUFFER_TABLE RecvBufferTcpTable[] = { { BUFFER_10_LEN       },
                                          { 0                   },
                                          { BUFFER_TCPLARGE_LEN } };
#define RecvBufferTcpTableCount  (sizeof(RecvBufferTcpTable) / sizeof(RECVBUFFER_TABLE));

RECVBUFFER_TABLE RecvBufferUdpTable[] = { { BUFFER_10_LEN       },
                                          { 0                   },
                                          { BUFFER_UDPLARGE_LEN } };
#define RecvBufferUdpTableCount  (sizeof(RecvBufferUdpTable) / sizeof(RECVBUFFER_TABLE));



typedef struct RECV_TABLE {
    CHAR   szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL   bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL   bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD  dwSocket;                                // dwSocket indicates the socket to be created
    BOOL   bNonblock;                               // bNonblock indicates if the socket is to be set as non-blocking for the recv
    BOOL   bBind;                                   // bBind indicates if the socket is to be bound
    BOOL   bListen;                                 // bListen indicates if the socket is to be placed in the listening state
    BOOL   bAccept;                                 // bAccept indicates if the socket is accepted
    BOOL   bConnect;                                // bConnect indicates if the socket is connected
    BOOL   bRemoteShutdown;                         // bRemoteShutdown indicates if the remote shutsdown the connection
    BOOL   bRemoteClose;                            // bRemoteClose indicates if the remote closes the connection
    BOOL   bShutdown;                               // bShutdown indicates if the connection is shutdown
    int    nShutdown;                               // nShutdown indicates how the connection is shutdown
    int    nDataBuffers;                            // nDataBuffers indicates the number of data buffers to be received
    DWORD  dwBuffer;                                // dwBuffer specifies the recv buffer
    int    nBufferlen;                              // nBufferlen specifies the recv data buffer length
    int    nFlags;                                  // nFlags specifies the recv flags
    BOOL   bSecondRecv;                             // bSecondRecv indicates if a second recv should occur
    int    iReturnCode;                             // iReturnCode is the return code of recv
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} RECV_TABLE, *PRECV_TABLE;

static RECV_TABLE recvTable[] =
{
    { "17.1 Not Initialized",            FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "17.2 s = INT_MIN",                TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "17.3 s = -1",                     TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "17.4 s = 0",                      TRUE,  TRUE,  SOCKET_ZERO,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "17.5 s = INT_MAX",                TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "17.6 s = INVALID_SOCKET",         TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "17.7 Not Bound TCP",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "17.8 Listening TCP",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "17.9 Accepted TCP",               TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.10 Connected TCP",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.11 Second Receive TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN / 2,   0, TRUE,  0,            0,                 FALSE },
    { "17.12 SD_RECEIVE Accept TCP",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "17.13 SD_SEND Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.14 SD_BOTH Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "17.15 SD_RECEIVE Connect TCP",    TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "17.16 SD_SEND Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.17 SD_BOTH Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "17.18 Shutdown Accept TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.19 Shutdown Connect TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.20 Close Accept TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "17.21 Close Connect TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "17.22 NULL Buffer TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "17.23 Neg Bufferlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, -1,                  0, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "17.24 NULL 0 Bufferlen TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, 0,            0,                 FALSE },
    { "17.25 0 Bufferlen TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, 0,                   0, FALSE, 0,            0,                 FALSE },
    { "17.26 Small Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN - 1,   0, FALSE, 0,            0,                 FALSE },
    { "17.27 Exact Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.28 Large Buffer TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_TCPLARGE_LEN, 0, FALSE, 0,            0,                 FALSE },
    { "17.29 Switch Recv TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.30 Iterative Recv TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.31 flags != 0 TCP",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       4, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "17.32 Not Bound NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "17.33 Listening NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "17.34 Accepted NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.35 Connected NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.36 Second Receive NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, TRUE,  0,            0,                 FALSE },
    { "17.37 SD_RECEIVE Accept NB TCP",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "17.38 SD_SEND Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.39 SD_BOTH Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "17.40 SD_RECEIVE Connect NB TCP", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "17.41 SD_SEND Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.42 SD_BOTH Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "17.43 Shutdown Accept NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.44 Shutdown Connect NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.45 Close Accept NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "17.46 Close Connect NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "17.47 NULL Buffer NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "17.48 Neg Bufferlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, -1,                  0, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "17.49 NULL 0 Bufferlen NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, 0,            0,                 FALSE },
    { "17.50 0 Bufferlen NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, 0,                   0, FALSE, 0,            0,                 FALSE },
    { "17.51 Small Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN - 1,   0, FALSE, 0,            0,                 FALSE },
    { "17.52 Exact Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.53 Large Buffer NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_TCPLARGE_LEN, 0, FALSE, 0,            0,                 FALSE },
    { "17.54 Switch Recv NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.55 Iterative Recv NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.56 flags != 0 NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       4, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "17.57 Not Bound UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "17.58 Not Connected UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.59 Connected UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.60 SD_RECEIVE Conn UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "17.61 SD_SEND Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.62 SD_BOTH Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "17.63 Second Receive UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN / 2,   0, TRUE,  SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "17.64 NULL Buffer UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "17.65 Neg Bufferlen UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, -1,                  0, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "17.66 NULL 0 Bufferlen UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, 0,            0,                 FALSE },
    { "17.67 0 Bufferlen UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, 0,                   0, FALSE, 0,            0,                 FALSE },
    { "17.68 Small Bufferlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN - 1,   0, FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "17.69 Exact Bufferlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.70 Large Buffer UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_UDPLARGE_LEN, 0, FALSE, 0,            0,                 FALSE },
    { "17.71 Switch Recv UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.72 Iterative Recv UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.73 flags != 0 UDP",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       4, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "17.74 Not Bound NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "17.75 Not Connected NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.76 Connected NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.77 SD_RECEIVE Conn NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "17.78 SD_SEND Conn NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.79 SD_BOTH Conn NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "17.80 Second Receive NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN / 2,   0, TRUE,  SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "17.81 NULL Buffer NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  BUFFER_10_LEN,       0, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "17.82 Neg Bufferlen NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, -1,                  0, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "17.83 NULL 0 Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, 0,            0,                 FALSE },
    { "17.84 0 Bufferlen NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, 0,                   0, FALSE, 0,            0,                 FALSE },
    { "17.85 Small Bufferlen NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN - 1,   0, FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "17.86 Exact Bufferlen NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.87 Large Buffer NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_UDPLARGE_LEN, 0, FALSE, 0,            0,                 FALSE },
    { "17.88 Switch Recv NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.89 Iterative Recv NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, 0,            0,                 FALSE },
    { "17.90 flags != 0 NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       4, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "17.91 Closed Socket TCP",         TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "17.92 Closed Socket UDP",         TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "17.93 Not Initialized",           FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define recvTableCount (sizeof(recvTable) / sizeof(RECV_TABLE))

NETSYNC_TYPE_THREAD  recvTestSessionNt =
{
    1,
    recvTableCount,
    L"xnetapi_nt.dll",
    "recvTestServer"
};

NETSYNC_TYPE_THREAD  recvTestSessionXbox =
{
    1,
    recvTableCount,
    L"xnetapi_xbox.dll",
    "recvTestServer"
};



VOID
recvTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests recv - Client side

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
    // recvRequest is the request sent to the server
    RECV_REQUEST           recvRequest;
    
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
    // dwFillBuffer is a counter to fill the buffers
    DWORD                  dwFillBuffer;
    // nBytes is the number of bytes sent
    int                    nBytes;
    // nRecvCount is a counter to enumerate each recv
    int                    nRecvCount;

    // dwFirstTime is the first tick count
    DWORD                  dwFirstTime;
    // dwSecondTime is the second tick count
    DWORD                  dwSecondTime;

    // RecvBufferTable is the test buffer table to use for iterative recv
    PRECVBUFFER_TABLE      RecvBufferTable;
    // RecvBufferTableCount is the number of elements within the test buffer table
    size_t                 RecvBufferTableCount;

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
    sprintf(szFunctionName, "recv v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_recv+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_recv-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = recvTestSessionXbox;
    }
    else {
        NetsyncTypeSession = recvTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < recvTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, recvTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, recvTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != recvTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, recvTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == recvTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != recvTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == recvTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = recvTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == recvTable[dwTableIndex].bNetsyncConnected)) {
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
        if (SOCKET_INT_MIN == recvTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == recvTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == recvTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == recvTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == recvTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & recvTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Set the send and receive timeout values to 5 sec
        if ((0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & recvTable[dwTableIndex].dwSocket))) {
            setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
            setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the socket to non-blocking mode
        if (TRUE == recvTable[dwTableIndex].bNonblock) {
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);
            bNonblocking = TRUE;
        }
        else {
            bNonblocking = FALSE;
        }

        // Bind the socket
        if (TRUE == recvTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Place the socket in the listening state
        if (TRUE == recvTable[dwTableIndex].bListen) {
            listen(sSocket, SOMAXCONN);
        }

        if (0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) {
            RecvBufferTable = RecvBufferTcpTable;
            RecvBufferTableCount = RecvBufferTcpTableCount;
        }
        else {
            RecvBufferTable = RecvBufferUdpTable;
            RecvBufferTableCount = RecvBufferUdpTableCount;
        }

        if ((TRUE == recvTable[dwTableIndex].bAccept) || (TRUE == recvTable[dwTableIndex].bConnect) || (0 == recvTable[dwTableIndex].iReturnCode)) {
            // Initialize the recv request
            recvRequest.dwMessageId = RECV_REQUEST_MSG;
            if (0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) {
                recvRequest.nSocketType = SOCK_STREAM;
            }
            else {
                recvRequest.nSocketType = SOCK_DGRAM;
            }
            recvRequest.Port = CurrentPort;
            recvRequest.bServerAccept = recvTable[dwTableIndex].bConnect;
            recvRequest.bRemoteShutdown = recvTable[dwTableIndex].bRemoteShutdown;
            recvRequest.bRemoteClose = recvTable[dwTableIndex].bRemoteClose;
            if (100 == recvTable[dwTableIndex].nDataBuffers) {
                recvRequest.nDataBuffers = recvTable[dwTableIndex].nDataBuffers * RecvBufferTableCount;
            }
            else {
                recvRequest.nDataBuffers = ((0 == recvTable[dwTableIndex].iReturnCode) || (WSAEMSGSIZE == recvTable[dwTableIndex].iLastError)) ? recvTable[dwTableIndex].nDataBuffers : 0;
            }
            if (TRUE == recvTable[dwTableIndex].bSecondRecv) {
                recvRequest.nBufferlen = recvTable[dwTableIndex].nBufferlen * 2;
            }
            else if (((0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) || (WSAEMSGSIZE == recvTable[dwTableIndex].iLastError)) && (BUFFER_10_LEN > recvTable[dwTableIndex].nBufferlen)) {
                recvRequest.nBufferlen = BUFFER_10_LEN;
            }
            else {
                recvRequest.nBufferlen = recvTable[dwTableIndex].nBufferlen;
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvRequest), (char *) &recvRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == recvTable[dwTableIndex].bConnect) {
                // Connect the socket
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = NetsyncInAddr;
                remotename.sin_port = htons(CurrentPort);

                connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));

                if (0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&writefds);
                    FD_SET(sSocket, &writefds);
                    select(0, NULL, &writefds, NULL, NULL);
                }
            }
            else if (TRUE == recvTable[dwTableIndex].bAccept) {
                if (0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&readfds);
                    FD_SET(sSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);
                }

                // Accept the socket
                nsSocket = accept(sSocket, NULL, NULL);
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvRequest), (char *) &recvRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == recvTable[dwTableIndex].bRemoteShutdown) {
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

            if (TRUE == recvTable[dwTableIndex].bRemoteClose) {
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
        if (TRUE == recvTable[dwTableIndex].bShutdown) {
            shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, recvTable[dwTableIndex].nShutdown);
        }

        // Zero the receive buffer
        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

        // Initialize the buffers
        sprintf(SendBuffer10, "%05d%05d", 1, 1);
        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & recvTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        // Get the current tick count
        dwFirstTime = GetTickCount();

        __try {
            // Call recv
            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (BUFFER_NONE != recvTable[dwTableIndex].dwBuffer) ? RecvBufferLarge : NULL, recvTable[dwTableIndex].nBufferlen, recvTable[dwTableIndex].nFlags);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == recvTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "recv RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "recv caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if ((TRUE == bNonblocking) && (FALSE == recvTable[dwTableIndex].bRemoteShutdown) && ((0 == recvTable[dwTableIndex].iReturnCode) || (WSAEMSGSIZE == recvTable[dwTableIndex].iLastError))) {
                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "recv returned non-SOCKET_ERROR");
                    bTestPassed = FALSE;
                }
                else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                    xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
                    bTestPassed = FALSE;
                }
                else {
                    xLog(hLog, XLL_PASS, "recv iLastError - OUT: %u", WSAGetLastError());

                    FD_ZERO(&readfds);
                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);

                    // Call recv
                    iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (BUFFER_NONE != recvTable[dwTableIndex].dwBuffer) ? RecvBufferLarge : NULL, recvTable[dwTableIndex].nBufferlen, recvTable[dwTableIndex].nFlags);
                }
            }

            // Get the current tick count
            dwSecondTime = GetTickCount();

            if (TRUE == recvTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "recv did not RIP");
            }

            if (TRUE == recvTable[dwTableIndex].bRemoteShutdown) {
                if (SOCKET_ERROR == iReturnCode) {
                    xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                }
                else if (0 != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "recv iReturnCode - EXPECTED: %u; RECEIVED: %u", 0, iReturnCode);
                }
                else {
                    xLog(hLog, XLL_PASS, "recv succeeded");
                }
            }
            else if (WSAEMSGSIZE == recvTable[dwTableIndex].iLastError) {
                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "recv returned non-SOCKET_ERROR");
                }
                else if (WSAEMSGSIZE != WSAGetLastError()) {
                    xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEMSGSIZE, WSAGetLastError());
                }
                else {
                    iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (BUFFER_NONE != recvTable[dwTableIndex].dwBuffer) ? RecvBufferLarge : NULL, recvTable[dwTableIndex].nBufferlen, recvTable[dwTableIndex].nFlags);

                    if (SOCKET_ERROR != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "recv returned non-SOCKET_ERROR");
                    }
                    else if (((TRUE == bNonblocking) && (WSAEWOULDBLOCK != WSAGetLastError())) || ((FALSE == bNonblocking) && (WSAETIMEDOUT != WSAGetLastError()))) {
                        xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", (TRUE == bNonblocking) ? WSAEWOULDBLOCK : WSAETIMEDOUT, WSAGetLastError());
                    }
                    else {
                        xLog(hLog, XLL_PASS, "recv succeeded");
                    }
                }
            }
            else if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == recvTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != recvTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", recvTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "recv iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());

                if (1 < recvRequest.nDataBuffers) {
                    // Send the recv cancel
                    recvRequest.dwMessageId = RECV_CANCEL_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvRequest), (char *) &recvRequest);
                }
            }
            else if (SOCKET_ERROR == recvTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "recv returned non-SOCKET_ERROR");
            }
            else {
                if (((dwSecondTime - dwFirstTime) < SLEEP_LOW_TIME) || ((dwSecondTime - dwFirstTime) > SLEEP_HIGH_TIME)) {
                    xLog(hLog, XLL_FAIL, "recv nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME, dwSecondTime - dwFirstTime);
                    bTestPassed = FALSE;
                }

                if (0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) {
                    nBytes = iReturnCode;

                    if (TRUE == recvTable[dwTableIndex].bSecondRecv) {
                        if (recvTable[dwTableIndex].nBufferlen != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", recvTable[dwTableIndex].nBufferlen, iReturnCode);
                            bTestPassed = FALSE;
                        }
                        else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, recvTable[dwTableIndex].nBufferlen)) {
                            xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                            bTestPassed = FALSE;
                        }

                        iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], recvTable[dwTableIndex].nBufferlen, recvTable[dwTableIndex].nFlags);

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            if (recvTable[dwTableIndex].nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", recvTable[dwTableIndex].nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(&SendBufferLarge[nBytes], &RecvBufferLarge[nBytes], recvTable[dwTableIndex].nBufferlen)) {
                                xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                bTestPassed = FALSE;
                            }

                            nBytes += iReturnCode;
                        }
                    }

                    if (BUFFER_10_LEN <= recvTable[dwTableIndex].nBufferlen) {
                        do {
                            FD_ZERO(&readfds);
                            FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                            iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                            if (1 == iReturnCode) {
                                iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], recvTable[dwTableIndex].nBufferlen, 0);

                                if (SOCKET_ERROR != iReturnCode) {
                                    nBytes += iReturnCode;
                                }
                            }
                        } while (0 != iReturnCode);
                    }

                    iReturnCode = nBytes;
                }

                if ((recvTable[dwTableIndex].nBufferlen * ((TRUE == recvTable[dwTableIndex].bSecondRecv) ? 2 : 1)) != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", (recvTable[dwTableIndex].nBufferlen * ((TRUE == recvTable[dwTableIndex].bSecondRecv) ? 2 : 1)), iReturnCode);
                    bTestPassed = FALSE;
                }
                else {
                    if (0 != strncmp(SendBufferLarge, RecvBufferLarge, recvTable[dwTableIndex].nBufferlen * ((TRUE == recvTable[dwTableIndex].bSecondRecv) ? 2 : 1))) {
                        xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                        bTestPassed = FALSE;
                    }
                }

                if (3 == recvTable[dwTableIndex].nDataBuffers) {
                    // Zero the receive buffer
                    ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvRequest), (char *) &recvRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    // Switch the blocking mode
                    Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                    bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                    // Call recv
                    iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, recvTable[dwTableIndex].nBufferlen, recvTable[dwTableIndex].nFlags);

                    if (TRUE == bNonblocking) {
                        if (SOCKET_ERROR != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv returned non-SOCKET_ERROR");
                            bTestPassed = FALSE;
                        }
                        else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                            xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "recv iLastError - OUT: %u", WSAGetLastError());

                            FD_ZERO(&readfds);
                            FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                            select(0, &readfds, NULL, NULL, NULL);

                            // Call recv
                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, recvTable[dwTableIndex].nBufferlen, recvTable[dwTableIndex].nFlags);
                        }
                    }

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                        recvRequest.dwMessageId = RECV_CANCEL_MSG;
                    }
                    else {
                        if (0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) {
                            nBytes = iReturnCode;

                            do {
                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                if (1 == iReturnCode) {
                                    iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], recvTable[dwTableIndex].nBufferlen, 0);

                                    if (SOCKET_ERROR != iReturnCode) {
                                        nBytes += iReturnCode;
                                    }
                                }
                            } while (0 != iReturnCode);

                            iReturnCode = nBytes;
                        }

                        if (recvTable[dwTableIndex].nBufferlen != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", recvTable[dwTableIndex].nBufferlen, iReturnCode);
                            bTestPassed = FALSE;
                        }
                        else {
                            if (0 != strncmp(SendBufferLarge, RecvBufferLarge, recvTable[dwTableIndex].nBufferlen)) {
                                xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                bTestPassed = FALSE;
                            }
                        }

                        recvRequest.dwMessageId = RECV_REQUEST_MSG;
                    }

                    // Zero the receive buffer
                    ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 3, 3);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvRequest), (char *) &recvRequest);

                    if (RECV_CANCEL_MSG != recvRequest.dwMessageId) {
                        // Wait for the read complete
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);

                        // Switch the blocking mode
                        Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                        ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                        bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                        // Call recv
                        iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, recvTable[dwTableIndex].nBufferlen, recvTable[dwTableIndex].nFlags);

                        if (TRUE == bNonblocking) {
                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv returned non-SOCKET_ERROR");
                                bTestPassed = FALSE;
                            }
                            else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "recv iLastError - OUT: %u", WSAGetLastError());

                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                select(0, &readfds, NULL, NULL, NULL);

                                // Call recv
                                iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, recvTable[dwTableIndex].nBufferlen, recvTable[dwTableIndex].nFlags);
                            }
                        }

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            if (0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) {
                                nBytes = iReturnCode;

                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                    if (1 == iReturnCode) {
                                        iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], recvTable[dwTableIndex].nBufferlen, 0);

                                        if (SOCKET_ERROR != iReturnCode) {
                                            nBytes += iReturnCode;
                                        }
                                    }
                                } while (0 != iReturnCode);

                                iReturnCode = nBytes;
                            }

                            if (recvTable[dwTableIndex].nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", recvTable[dwTableIndex].nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else {
                                if (0 != strncmp(SendBufferLarge, RecvBufferLarge, recvTable[dwTableIndex].nBufferlen)) {
                                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                    }
                }
                else if (100 == recvTable[dwTableIndex].nDataBuffers) {
                    for (nRecvCount = 0; nRecvCount < (int) (recvTable[dwTableIndex].nDataBuffers * RecvBufferTableCount); nRecvCount++) {
                        // Zero the receive buffer
                        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                        // Initialize the buffers
                        sprintf(SendBuffer10, "%05d%05d", 2 + nRecvCount, 2 + nRecvCount);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                        }

                        xLog(hLog, XLL_INFO, "Iteration %d", nRecvCount);

                        // Send the recv request
                        recvRequest.nBufferlen = RecvBufferTable[nRecvCount % RecvBufferTableCount].nBufferlen;
                        NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvRequest), (char *) &recvRequest);

                        if (RECV_CANCEL_MSG == recvRequest.dwMessageId) {
                            break;
                        }

                        // Wait for the read complete
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);

                        if ((0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) && (0 == RecvBufferTable[nRecvCount % RecvBufferTableCount].nBufferlen)) {
                            iReturnCode = 0;
                        }
                        else {
                            if (TRUE == bNonblocking) {
                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                select(0, &readfds, NULL, NULL, NULL);
                            }

                            // Call recv
                            iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, RecvBufferTable[nRecvCount % RecvBufferTableCount].nBufferlen, 0);
                        }

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            recvRequest.dwMessageId = RECV_CANCEL_MSG;
                            bTestPassed = FALSE;
                        }
                        else {
                            if (0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) {
                                nBytes = iReturnCode;

                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                    if (1 == iReturnCode) {
                                        iReturnCode = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], RecvBufferTable[nRecvCount % RecvBufferTableCount].nBufferlen, 0);

                                        if (SOCKET_ERROR != iReturnCode) {
                                            nBytes += iReturnCode;
                                        }
                                    }
                                } while (0 != iReturnCode);

                                iReturnCode = nBytes;
                            }

                            if (RecvBufferTable[nRecvCount % RecvBufferTableCount].nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", RecvBufferTable[nRecvCount % RecvBufferTableCount].nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else {
                                if (0 != strncmp(SendBufferLarge, RecvBufferLarge, RecvBufferTable[nRecvCount % RecvBufferTableCount].nBufferlen)) {
                                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "recv succeeded");
                }
            }
        }

        // Switch the blocking mode
        if (recvTable[dwTableIndex].bNonblock != bNonblocking) {
            Nonblock = (TRUE == recvTable[dwTableIndex].bNonblock) ? 1 : 0;
            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
            bNonblocking = recvTable[dwTableIndex].bNonblock;
        }

        if ((TRUE == recvTable[dwTableIndex].bAccept) || (TRUE == recvTable[dwTableIndex].bConnect) || (0 == recvTable[dwTableIndex].iReturnCode)) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvRequest), (char *) &recvRequest);
        }

        // Close the sockets
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & recvTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & recvTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & recvTable[dwTableIndex].dwSocket))) {
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
recvTestServer(
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
    u_long         FromInAddr;
    // dwMessageType is the type of received message
    DWORD          dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD          dwMessageSize;
    // pMessage is a pointer to the received message
    char           *pMessage;
    // recvRequest is the request
    RECV_REQUEST   recvRequest;
    // recvComplete is the result
    RECV_COMPLETE  recvComplete;

    // sSocket is the socket descriptor
    SOCKET         sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET         nsSocket;

    // iTimeout is the send and receive timeout value for the socket
    int            iTimeout = 5000;
    // bNagle indicates if Nagle is enabled
    BOOL           bNagle = FALSE;

    // localname is the local address
    SOCKADDR_IN    localname;
    // remotename is the remote address
    SOCKADDR_IN    remotename;

    // SendBuffer10 is the send buffer
    char           SendBuffer10[BUFFER_10_LEN + 1];
    // SendBufferLarge is the large send buffer
    char           SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // dwFillBuffer is a counter to fill the buffers
    DWORD          dwFillBuffer;
    // nSendCount is a counter to enumerate each send
    int            nSendCount;



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
        CopyMemory(&recvRequest, pMessage, sizeof(recvRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, recvRequest.nSocketType, 0);

        // Set the send and receive timeout values to 5 sec
        setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));

        if (SOCK_STREAM == recvRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(recvRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if ((SOCK_STREAM == recvRequest.nSocketType) && (TRUE == recvRequest.bServerAccept)) {
            // Place the socket in listening mode
            listen(sSocket, SOMAXCONN);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(recvRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        recvComplete.dwMessageId = RECV_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(recvComplete), (char *) &recvComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if ((SOCK_STREAM == recvRequest.nSocketType) && (TRUE == recvRequest.bServerAccept)) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        if (TRUE == recvRequest.bRemoteShutdown) {
            // Shutdown the sockets
            if (INVALID_SOCKET != nsSocket) {
                shutdown(nsSocket, SD_BOTH);
            }

            if (INVALID_SOCKET != sSocket) {
                shutdown(sSocket, SD_BOTH);
            }
        }

        if (TRUE == recvRequest.bRemoteClose) {
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
        recvComplete.dwMessageId = RECV_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(recvComplete), (char *) &recvComplete);

        if (0 < recvRequest.nDataBuffers) {
            // Sleep
            Sleep(SLEEP_MIDLOW_TIME);

            // Initialize the buffer
            sprintf(SendBuffer10, "%05d%05d", 1, 1);
            for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
            }

            // Send the buffer
            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, recvRequest.nBufferlen, 0);

            if (3 == recvRequest.nDataBuffers) {
                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                
                if (RECV_CANCEL_MSG != ((PRECV_REQUEST) pMessage)->dwMessageId) {
                    // Send the complete
                    recvComplete.dwMessageId = RECV_COMPLETE_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(recvComplete), (char *) &recvComplete);

                    // Sleep
                    Sleep(SLEEP_MIDLOW_TIME);

                    // Initialize the buffer
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                    }

                    // Send the buffer
                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, recvRequest.nBufferlen, 0);

                    NetsyncFreeMessage(pMessage);

                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                    if (RECV_CANCEL_MSG != ((PRECV_REQUEST) pMessage)->dwMessageId) {
                        // Send the complete
                        recvComplete.dwMessageId = RECV_COMPLETE_MSG;
                        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(recvComplete), (char *) &recvComplete);

                        // Sleep
                        Sleep(SLEEP_MIDLOW_TIME);

                        // Initialize the buffer
                        sprintf(SendBuffer10, "%05d%05d", 3, 3);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                        }

                        // Send the buffer
                        send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, recvRequest.nBufferlen, 0);
                    }
                }

                NetsyncFreeMessage(pMessage);
            }
            else if (100 <= recvRequest.nDataBuffers) {
                for (nSendCount = 0; nSendCount < recvRequest.nDataBuffers; nSendCount++) {
                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                    if (RECV_CANCEL_MSG == ((PRECV_REQUEST) pMessage)->dwMessageId) {
                        NetsyncFreeMessage(pMessage);
                        break;
                    }

                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 2 + nSendCount, 2 + nSendCount);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                    }

                    // Send the buffer
                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, ((PRECV_REQUEST) pMessage)->nBufferlen, 0);

                    NetsyncFreeMessage(pMessage);

                    // Send the complete
                    recvComplete.dwMessageId = RECV_COMPLETE_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(recvComplete), (char *) &recvComplete);
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
