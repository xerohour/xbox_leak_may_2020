/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  recvfrom.c

Abstract:

  This modules tests recvfrom

Author:

  Steven Kehrli (steveke) 13-Dev-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// recv messages

#define RECVFROM_REQUEST_MSG   NETSYNC_MSG_USER + 190 + 1
#define RECVFROM_CANCEL_MSG    NETSYNC_MSG_USER + 190 + 2
#define RECVFROM_COMPLETE_MSG  NETSYNC_MSG_USER + 190 + 3

typedef struct _RECVFROM_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  Port;
    BOOL     bServerAccept;
    BOOL     bRemoteShutdown;
    BOOL     bRemoteClose;
    int      nDataBuffers;
    int      nBufferlen;
} RECVFROM_REQUEST, *PRECVFROM_REQUEST;

typedef struct _RECVFROM_COMPLETE {
    DWORD    dwMessageId;
} RECVFROM_COMPLETE, *PRECVFROM_COMPLETE;

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



typedef struct RECVFROMBUFFER_TABLE {
    int   nBufferlen;
} RECVFROMBUFFER_TABLE, *PRECVFROMBUFFER_TABLE;

RECVFROMBUFFER_TABLE RecvFromBufferTcpTable[] = { { BUFFER_10_LEN       },
                                                  { 0                   },
                                                  { BUFFER_TCPLARGE_LEN } };
#define RecvFromBufferTcpTableCount  (sizeof(RecvFromBufferTcpTable) / sizeof(RECVFROMBUFFER_TABLE));

RECVFROMBUFFER_TABLE RecvFromBufferUdpTable[] = { { BUFFER_10_LEN       },
                                                  { 0                   },
                                                  { BUFFER_UDPLARGE_LEN } };
#define RecvFromBufferUdpTableCount  (sizeof(RecvFromBufferUdpTable) / sizeof(RECVFROMBUFFER_TABLE));



typedef struct RECVFROM_TABLE {
    CHAR   szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL   bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL   bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD  dwSocket;                                // dwSocket indicates the socket to be created
    BOOL   bNonblock;                               // bNonblock indicates if the socket is to be set as non-blocking for the recvfrom
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
    int    nBufferlen;                              // nBufferlen specifies the recvfrom data buffer length
    int    nFlags;                                  // nFlags specifies the recvfrom flags
    BOOL   bSecondRecv;                             // bSecondRecv indicates if a second recvfrom should occur
    BOOL   bname;                                   // bname indicates if the address buffer is used
    BOOL   bnamelen;                                // bnamelen indicates if namelen is used
    int    namelen;                                 // namelen is the length of the address buffer
    int    iReturnCode;                             // iReturnCode is the return code of recvfrom
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} RECVFROM_TABLE, *PRECVFROM_TABLE;

static RECVFROM_TABLE recvfromTable[] =
{
    { "19.1 Not Initialized",            FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "19.2 s = INT_MIN",                TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "19.3 s = -1",                     TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "19.4 s = 0",                      TRUE,  TRUE,  SOCKET_ZERO,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "19.5 s = INT_MAX",                TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "19.6 s = INVALID_SOCKET",         TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "19.7 Not Bound TCP",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "19.8 Listening TCP",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "19.9 Accepted TCP",               TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.10 Connected TCP",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.11 Second Receive TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN / 2,   0, TRUE,  FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.12 SD_RECEIVE Accept TCP",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "19.13 SD_SEND Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.14 SD_BOTH Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "19.15 SD_RECEIVE Connect TCP",    TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "19.16 SD_SEND Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.17 SD_BOTH Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "19.18 Shutdown Accept TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "19.19 Shutdown Connect TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "19.20 Close Accept TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "19.21 Close Connect TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "19.22 NULL Buffer TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.23 Neg Bufferlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, -1,                  0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.24 NULL 0 Bufferlen TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.25 0 Bufferlen TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, 0,                   0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.26 Small Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN - 1,   0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.27 Exact Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.28 Large Buffer TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_TCPLARGE_LEN, 0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.29 Switch Recv TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.30 Iterative Recv TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.31 flags != 0 TCP",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       4, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.32 NULL addr TCP",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.33 addr TCP",                  TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.34 Large addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "19.35 Exact addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.36 Small addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN) - 1, 0,            WSAEFAULT,         TRUE  },
    { "19.37 Zero addrlen TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  0,                       0,            WSAEFAULT,         TRUE  },
    { "19.38 Neg addrlen TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  -1,                      0,            WSAEFAULT,         TRUE  },
    { "19.39 NULL addrlen TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   FALSE, 0,                       0,            WSAEFAULT,         TRUE  },
    { "19.40 Not Bound NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "19.41 Listening NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "19.42 Accepted NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.43 Connected NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.44 Second Receive NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, TRUE,  FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.45 SD_RECEIVE Accept NB TCP",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "19.46 SD_SEND Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.47 SD_BOTH Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "19.48 SD_RECEIVE Connect NB TCP", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "19.49 SD_SEND Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.50 SD_BOTH Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "19.51 Shutdown Accept NB TCP",    FALSE, TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "19.52 Shutdown Connect NB TCP",   FALSE, TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "19.53 Close Accept NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "19.54 Close Connect NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "19.55 NULL Buffer NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.56 Neg Bufferlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, -1,                  0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.57 NULL 0 Bufferlen NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.58 0 Bufferlen NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, 0,                   0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.59 Small Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN - 1,   0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.60 Exact Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.61 Large Buffer NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_TCPLARGE_LEN, 0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.62 Switch Recv NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.63 Iterative Recv NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.64 flags != 0 NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       4, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.65 NULL addr NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.66 addr NB TCP",               TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.67 Large addrlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "19.68 Exact addrlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.69 Small addrlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN) - 1, 0,            WSAEFAULT,         TRUE  },
    { "19.70 Zero addrlen NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  0,                       0,            WSAEFAULT,         TRUE  },
    { "19.71 Neg addrlen NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  -1,                      0,            WSAEFAULT,         TRUE  },
    { "19.72 NULL addrlen NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   FALSE, 0,                       0,            WSAEFAULT,         TRUE  },
    { "19.73 Not Bound UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "19.74 Not Connected UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.75 Connected UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.76 SD_RECEIVE Conn UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "19.77 SD_SEND Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.78 SD_BOTH Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "19.79 Second Receive UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN / 2,   0, TRUE,  FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "19.80 NULL Buffer UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.81 Neg Bufferlen UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, -1,                  0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.82 NULL 0 Bufferlen UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.83 0 Bufferlen UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, 0,                   0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.84 Small Bufferlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN - 1,   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "19.85 Exact Bufferlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.86 Large Buffer UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_UDPLARGE_LEN, 0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.87 Switch Recv UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.88 Iterative Recv UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.89 flags != 0 UDP",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       4, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.90 NULL addr UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.91 Not Connect addr UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.92 Connected addr UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.93 Large addrlen UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "19.94 Exact addrlen UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.95 Small addrlen UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN) - 1, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.96 Zero addrlen UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.97 Neg addrlen UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  -1,                      SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.98 NULL addrlen UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.99 Not Bound NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "19.100 Not Connected NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.101 Connected NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.102 SD_RECEIVE Conn NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "19.103 SD_SEND Conn NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.104 SD_BOTH Conn NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "19.105 Second Receive NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN / 2,   0, TRUE,  FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "19.106 NULL Buffer NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.107 Neg Bufferlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, -1,                  0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.108 NULL 0 Bufferlen NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.109 0 Bufferlen NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, 0,                   0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.110 Small Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN - 1,   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "19.111 Exact Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.112 Large Buffer NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_UDPLARGE_LEN, 0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.113 Switch Recv NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.114 Iterative Recv NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  FALSE, 0,                       0,            0,                 FALSE },
    { "19.115 flags != 0 NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       4, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.116 NULL addr NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, FALSE,  TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.117 Not Connect addr NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.118 Connected addr NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.119 Large addrlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "19.120 Exact addrlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "19.121 Small addrlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  sizeof(SOCKADDR_IN) - 1, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.122 Zero addrlen NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.123 Neg addrlen NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   TRUE,  -1,                      SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.124 NULL addrlen NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_10_LEN,       0, FALSE, TRUE,   FALSE, 0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "19.125 Closed Socket TCP",        TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "19.126 Closed Socket UDP",        TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "19.127 Not Initialized",          FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,  0,                   0, FALSE, FALSE,  FALSE, 0,                       SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define recvfromTableCount (sizeof(recvfromTable) / sizeof(RECVFROM_TABLE))

NETSYNC_TYPE_THREAD  recvfromTestSessionNt =
{
    1,
    recvfromTableCount,
    L"xnetapi_nt.dll",
    "recvfromTestServer"
};

NETSYNC_TYPE_THREAD  recvfromTestSessionXbox =
{
    1,
    recvfromTableCount,
    L"xnetapi_xbox.dll",
    "recvfromTestServer"
};



VOID
recvfromTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests recvfrom - Client side

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
    // recvfromRequest is the request sent to the server
    RECVFROM_REQUEST       recvfromRequest;

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

    // RecvFromBufferTable is the test buffer table to use for iterative recvfrom
    PRECVFROMBUFFER_TABLE  RecvFromBufferTable;
    // RecvFromBufferTableCount is the number of elements within the test buffer table
    size_t                 RecvFromBufferTableCount;
    // namelen is the size of the address buffer
    int                    namelen;

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
    sprintf(szFunctionName, "recvfrom v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_recvfrom+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_recvfrom-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = recvfromTestSessionXbox;
    }
    else {
        NetsyncTypeSession = recvfromTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < recvfromTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, recvfromTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, recvfromTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != recvfromTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, recvfromTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == recvfromTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != recvfromTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == recvfromTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = recvfromTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == recvfromTable[dwTableIndex].bNetsyncConnected)) {
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
        if (SOCKET_INT_MIN == recvfromTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == recvfromTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == recvfromTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == recvfromTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == recvfromTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & recvfromTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Set the send and receive timeout values to 5 sec
        if ((0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & recvfromTable[dwTableIndex].dwSocket))) {
            setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
            setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the socket to non-blocking mode
        if (TRUE == recvfromTable[dwTableIndex].bNonblock) {
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);
            bNonblocking = TRUE;
        }
        else {
            bNonblocking = FALSE;
        }

        // Bind the socket
        if (TRUE == recvfromTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Place the socket in the listening state
        if (TRUE == recvfromTable[dwTableIndex].bListen) {
            listen(sSocket, SOMAXCONN);
        }

        if (0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) {
            RecvFromBufferTable = RecvFromBufferTcpTable;
            RecvFromBufferTableCount = RecvFromBufferTcpTableCount;
        }
        else {
            RecvFromBufferTable = RecvFromBufferUdpTable;
            RecvFromBufferTableCount = RecvFromBufferUdpTableCount;
        }

        if ((TRUE == recvfromTable[dwTableIndex].bAccept) || (TRUE == recvfromTable[dwTableIndex].bConnect) || (0 == recvfromTable[dwTableIndex].iReturnCode)) {
            // Initialize the recvfrom request
            recvfromRequest.dwMessageId = RECVFROM_REQUEST_MSG;
            if (0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) {
                recvfromRequest.nSocketType = SOCK_STREAM;
            }
            else {
                recvfromRequest.nSocketType = SOCK_DGRAM;
            }
            recvfromRequest.Port = CurrentPort;
            recvfromRequest.bServerAccept = recvfromTable[dwTableIndex].bConnect;
            recvfromRequest.bRemoteShutdown = recvfromTable[dwTableIndex].bRemoteShutdown;
            recvfromRequest.bRemoteClose = recvfromTable[dwTableIndex].bRemoteClose;
            if (100 == recvfromTable[dwTableIndex].nDataBuffers) {
                recvfromRequest.nDataBuffers = recvfromTable[dwTableIndex].nDataBuffers * RecvFromBufferTableCount;
            }
            else {
                recvfromRequest.nDataBuffers = ((0 == recvfromTable[dwTableIndex].iReturnCode) || (WSAEMSGSIZE == recvfromTable[dwTableIndex].iLastError)) ? recvfromTable[dwTableIndex].nDataBuffers : 0;
            }
            if (TRUE == recvfromTable[dwTableIndex].bSecondRecv) {
                recvfromRequest.nBufferlen = recvfromTable[dwTableIndex].nBufferlen * 2;
            }
            else if (((0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) || (WSAEMSGSIZE == recvfromTable[dwTableIndex].iLastError)) && (BUFFER_10_LEN > recvfromTable[dwTableIndex].nBufferlen)) {
                recvfromRequest.nBufferlen = BUFFER_10_LEN;
            }
            else {
                recvfromRequest.nBufferlen = recvfromTable[dwTableIndex].nBufferlen;
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvfromRequest), (char *) &recvfromRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == recvfromTable[dwTableIndex].bConnect) {
                // Connect the socket
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = NetsyncInAddr;
                remotename.sin_port = htons(CurrentPort);

                connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));

                if (0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&writefds);
                    FD_SET(sSocket, &writefds);
                    select(0, NULL, &writefds, NULL, NULL);
                }
            }
            else if (TRUE == recvfromTable[dwTableIndex].bAccept) {
                if (0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&readfds);
                    FD_SET(sSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);
                }

                // Accept the socket
                nsSocket = accept(sSocket, NULL, NULL);
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvfromRequest), (char *) &recvfromRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == recvfromTable[dwTableIndex].bRemoteShutdown) {
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

            if (TRUE == recvfromTable[dwTableIndex].bRemoteClose) {
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
        if (TRUE == recvfromTable[dwTableIndex].bShutdown) {
            shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, recvfromTable[dwTableIndex].nShutdown);
        }

        // Zero the receive buffer
        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

        // Initialize the buffers
        sprintf(SendBuffer10, "%05d%05d", 1, 1);
        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
        }

        // Initialize the remote name
        namelen = recvfromTable[dwTableIndex].namelen;
        ZeroMemory(&remotename, sizeof(remotename));

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & recvfromTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        // Get the current tick count
        dwFirstTime = GetTickCount();

        __try {
            // Call recvfrom
            iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (BUFFER_NONE != recvfromTable[dwTableIndex].dwBuffer) ? RecvBufferLarge : NULL, recvfromTable[dwTableIndex].nBufferlen, recvfromTable[dwTableIndex].nFlags, (TRUE == recvfromTable[dwTableIndex].bname) ? (SOCKADDR *) &remotename : NULL, (TRUE == recvfromTable[dwTableIndex].bnamelen) ? &namelen : NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == recvfromTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "recvfrom RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "recvfrom caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if ((TRUE == bNonblocking) && (FALSE == recvfromTable[dwTableIndex].bRemoteShutdown) && ((0 == recvfromTable[dwTableIndex].iReturnCode) || (WSAEMSGSIZE == recvfromTable[dwTableIndex].iLastError))) {
                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "recvfrom returned non-SOCKET_ERROR");
                    bTestPassed = FALSE;
                }
                else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                    xLog(hLog, XLL_FAIL, "recvfrom iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
                    bTestPassed = FALSE;
                }
                else {
                    xLog(hLog, XLL_PASS, "recvfrom iLastError - OUT: %u", WSAGetLastError());

                    FD_ZERO(&readfds);
                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);

                    // Call recvfrom
                    iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (BUFFER_NONE != recvfromTable[dwTableIndex].dwBuffer) ? RecvBufferLarge : NULL, recvfromTable[dwTableIndex].nBufferlen, recvfromTable[dwTableIndex].nFlags, (TRUE == recvfromTable[dwTableIndex].bname) ? (SOCKADDR *) &remotename : NULL, (TRUE == recvfromTable[dwTableIndex].bnamelen) ? &namelen : NULL);
                }
            }

            // Get the current tick count
            dwSecondTime = GetTickCount();

            if (TRUE == recvfromTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "recvfrom did not RIP");
            }

            if (TRUE == recvfromTable[dwTableIndex].bRemoteShutdown) {
                if (SOCKET_ERROR == iReturnCode) {
                    xLog(hLog, XLL_FAIL, "recvfrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                }
                else if (0 != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "recvfrom iReturnCode - EXPECTED: %u; RECEIVED: %u", 0, iReturnCode);
                }
                else {
                    xLog(hLog, XLL_PASS, "recvfrom succeeded");
                }
            }
            else if (WSAEMSGSIZE == recvfromTable[dwTableIndex].iLastError) {
                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "recvfrom returned non-SOCKET_ERROR");
                }
                else if (WSAEMSGSIZE != WSAGetLastError()) {
                    xLog(hLog, XLL_FAIL, "recvfrom iLastError - EXPECTED: %u; RECEIVED: %u", WSAEMSGSIZE, WSAGetLastError());
                }
                else {
                    iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (BUFFER_NONE != recvfromTable[dwTableIndex].dwBuffer) ? RecvBufferLarge : NULL, recvfromTable[dwTableIndex].nBufferlen, recvfromTable[dwTableIndex].nFlags, NULL, NULL);

                    if (SOCKET_ERROR != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "recvfrom returned non-SOCKET_ERROR");
                    }
                    else if (((TRUE == bNonblocking) && (WSAEWOULDBLOCK != WSAGetLastError())) || ((FALSE == bNonblocking) && (WSAETIMEDOUT != WSAGetLastError()))) {
                        xLog(hLog, XLL_FAIL, "recvfrom iLastError - EXPECTED: %u; RECEIVED: %u", (TRUE == bNonblocking) ? WSAEWOULDBLOCK : WSAETIMEDOUT, WSAGetLastError());
                    }
                    else {
                        xLog(hLog, XLL_PASS, "recvfrom succeeded");
                    }
                }
            }
            else if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == recvfromTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != recvfromTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "recvfrom iLastError - EXPECTED: %u; RECEIVED: %u", recvfromTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "recvfrom iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "recvfrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());

                if (1 < recvfromRequest.nDataBuffers) {
                    // Send the recvfrom cancel
                    recvfromRequest.dwMessageId = RECVFROM_CANCEL_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvfromRequest), (char *) &recvfromRequest);
                }
            }
            else if (SOCKET_ERROR == recvfromTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "recvfrom returned non-SOCKET_ERROR");
            }
            else {
                if (((dwSecondTime - dwFirstTime) < SLEEP_LOW_TIME) || ((dwSecondTime - dwFirstTime) > SLEEP_HIGH_TIME)) {
                    xLog(hLog, XLL_FAIL, "recvfrom nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME, dwSecondTime - dwFirstTime);
                    bTestPassed = FALSE;
                }

                if (TRUE == recvfromTable[dwTableIndex].bname) {
                    if (AF_INET != remotename.sin_family) {
                        xLog(hLog, XLL_FAIL, "recvfrom sin_family - EXPECTED: %d; RECEIVED: %d", AF_INET, remotename.sin_family);
                        bTestPassed = FALSE;
                    }

                    if (htons(CurrentPort) != remotename.sin_port) {
                        xLog(hLog, XLL_FAIL, "recvfrom sin_port - EXPECTED: %d; RECEIVED: %d", htons(CurrentPort), remotename.sin_port);
                        bTestPassed = FALSE;
                    }

                    if (NetsyncInAddr != remotename.sin_addr.s_addr) {
                        xLog(hLog, XLL_FAIL, "recvfrom sin_addr - EXPECTED: %u; RECEIVED: %u", NetsyncInAddr, remotename.sin_addr.s_addr);
                        bTestPassed = FALSE;
                    }
                }

                if (0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) {
                    nBytes = iReturnCode;

                    if (TRUE == recvfromTable[dwTableIndex].bSecondRecv) {
                        if (recvfromTable[dwTableIndex].nBufferlen != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recvfrom return value - EXPECTED: %d; RECEIVED: %d", recvfromTable[dwTableIndex].nBufferlen, iReturnCode);
                            bTestPassed = FALSE;
                        }
                        else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, recvfromTable[dwTableIndex].nBufferlen)) {
                            xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                            bTestPassed = FALSE;
                        }

                        iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], recvfromTable[dwTableIndex].nBufferlen, recvfromTable[dwTableIndex].nFlags, NULL, NULL);

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recvfrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            if (recvfromTable[dwTableIndex].nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recvfrom return value - EXPECTED: %d; RECEIVED: %d", recvfromTable[dwTableIndex].nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(&SendBufferLarge[nBytes], &RecvBufferLarge[nBytes], recvfromTable[dwTableIndex].nBufferlen)) {
                                xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                bTestPassed = FALSE;
                            }

                            nBytes += iReturnCode;
                        }
                    }

                    if (BUFFER_10_LEN <= recvfromTable[dwTableIndex].nBufferlen) {
                        do {
                            FD_ZERO(&readfds);
                            FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                            iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                            if (1 == iReturnCode) {
                                iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], recvfromTable[dwTableIndex].nBufferlen, 0, NULL, NULL);

                                if (SOCKET_ERROR != iReturnCode) {
                                    nBytes += iReturnCode;
                                }
                            }
                        } while (0 != iReturnCode);
                    }

                    iReturnCode = nBytes;
                }

                if ((recvfromTable[dwTableIndex].nBufferlen * ((TRUE == recvfromTable[dwTableIndex].bSecondRecv) ? 2 : 1)) != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "recvfrom return value - EXPECTED: %d; RECEIVED: %d", (recvfromTable[dwTableIndex].nBufferlen * ((TRUE == recvfromTable[dwTableIndex].bSecondRecv) ? 2 : 1)), iReturnCode);
                    bTestPassed = FALSE;
                }
                else {
                    if (0 != strncmp(SendBufferLarge, RecvBufferLarge, recvfromTable[dwTableIndex].nBufferlen * ((TRUE == recvfromTable[dwTableIndex].bSecondRecv) ? 2 : 1))) {
                        xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                        bTestPassed = FALSE;
                    }
                }

                if (3 == recvfromTable[dwTableIndex].nDataBuffers) {
                    // Zero the receive buffer
                    ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvfromRequest), (char *) &recvfromRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    // Switch the blocking mode
                    Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                    bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                    // Call recvfrom
                    iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, recvfromTable[dwTableIndex].nBufferlen, recvfromTable[dwTableIndex].nFlags, NULL, NULL);

                    if (TRUE == bNonblocking) {
                        if (SOCKET_ERROR != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recvfrom returned non-SOCKET_ERROR");
                            bTestPassed = FALSE;
                        }
                        else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                            xLog(hLog, XLL_FAIL, "recvfrom iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "recvfrom iLastError - OUT: %u", WSAGetLastError());

                            FD_ZERO(&readfds);
                            FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                            select(0, &readfds, NULL, NULL, NULL);

                            // Call recvfrom
                            iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, recvfromTable[dwTableIndex].nBufferlen, recvfromTable[dwTableIndex].nFlags, NULL, NULL);
                        }
                    }

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "recvfrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                        recvfromRequest.dwMessageId = RECVFROM_CANCEL_MSG;
                    }
                    else {
                        if (0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) {
                            nBytes = iReturnCode;

                            do {
                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                if (1 == iReturnCode) {
                                    iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], recvfromTable[dwTableIndex].nBufferlen, 0, NULL, NULL);

                                    if (SOCKET_ERROR != iReturnCode) {
                                        nBytes += iReturnCode;
                                    }
                                }
                            } while (0 != iReturnCode);

                            iReturnCode = nBytes;
                        }

                        if (recvfromTable[dwTableIndex].nBufferlen != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recvfrom return value - EXPECTED: %d; RECEIVED: %d", recvfromTable[dwTableIndex].nBufferlen, iReturnCode);
                            bTestPassed = FALSE;
                        }
                        else {
                            if (0 != strncmp(SendBufferLarge, RecvBufferLarge, recvfromTable[dwTableIndex].nBufferlen)) {
                                xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                bTestPassed = FALSE;
                            }
                        }

                        recvfromRequest.dwMessageId = RECVFROM_REQUEST_MSG;
                    }

                    // Zero the receive buffer
                    ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 3, 3);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvfromRequest), (char *) &recvfromRequest);

                    if (RECVFROM_CANCEL_MSG != recvfromRequest.dwMessageId) {
                        // Wait for the read complete
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);

                        // Switch the blocking mode
                        Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                        ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                        bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                        // Call recvfrom
                        iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, recvfromTable[dwTableIndex].nBufferlen, recvfromTable[dwTableIndex].nFlags, NULL, NULL);

                        if (TRUE == bNonblocking) {
                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recvfrom returned non-SOCKET_ERROR");
                                bTestPassed = FALSE;
                            }
                            else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "recvfrom iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "recvfrom iLastError - OUT: %u", WSAGetLastError());

                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                select(0, &readfds, NULL, NULL, NULL);

                                // Call recvfrom
                                iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, recvfromTable[dwTableIndex].nBufferlen, recvfromTable[dwTableIndex].nFlags, NULL, NULL);
                            }
                        }

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recvfrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            if (0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) {
                                nBytes = iReturnCode;

                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                    if (1 == iReturnCode) {
                                        iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], recvfromTable[dwTableIndex].nBufferlen, 0, NULL, NULL);

                                        if (SOCKET_ERROR != iReturnCode) {
                                            nBytes += iReturnCode;
                                        }
                                    }
                                } while (0 != iReturnCode);

                                iReturnCode = nBytes;
                            }

                            if (recvfromTable[dwTableIndex].nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recvfrom return value - EXPECTED: %d; RECEIVED: %d", recvfromTable[dwTableIndex].nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else {
                                if (0 != strncmp(SendBufferLarge, RecvBufferLarge, recvfromTable[dwTableIndex].nBufferlen)) {
                                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                    }
                }
                else if (100 == recvfromTable[dwTableIndex].nDataBuffers) {
                    for (nRecvCount = 0; nRecvCount < (int) (recvfromTable[dwTableIndex].nDataBuffers * RecvFromBufferTableCount); nRecvCount++) {
                        // Zero the receive buffer
                        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                        // Initialize the buffers
                        sprintf(SendBuffer10, "%05d%05d", 2 + nRecvCount, 2 + nRecvCount);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                        }

                        xLog(hLog, XLL_INFO, "Iteration %d", nRecvCount);

                        // Send the recvfrom request
                        recvfromRequest.nBufferlen = RecvFromBufferTable[nRecvCount % RecvFromBufferTableCount].nBufferlen;
                        NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvfromRequest), (char *) &recvfromRequest);

                        if (RECVFROM_CANCEL_MSG == recvfromRequest.dwMessageId) {
                            break;
                        }

                        // Wait for the read complete
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);

                        if ((0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) && (0 == RecvFromBufferTable[nRecvCount % RecvFromBufferTableCount].nBufferlen)) {
                            iReturnCode = 0;
                        }
                        else {
                            if (TRUE == bNonblocking) {
                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                select(0, &readfds, NULL, NULL, NULL);
                            }

                            // Call recvfrom
                            iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, RecvFromBufferTable[nRecvCount % RecvFromBufferTableCount].nBufferlen, 0, NULL, NULL);
                        }

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recvfrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            recvfromRequest.dwMessageId = RECVFROM_CANCEL_MSG;
                            bTestPassed = FALSE;
                        }
                        else {
                            if (0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) {
                                nBytes = iReturnCode;

                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                    if (1 == iReturnCode) {
                                        iReturnCode = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &RecvBufferLarge[nBytes], RecvFromBufferTable[nRecvCount % RecvFromBufferTableCount].nBufferlen, 0, NULL, NULL);

                                        if (SOCKET_ERROR != iReturnCode) {
                                            nBytes += iReturnCode;
                                        }
                                    }
                                } while (0 != iReturnCode);

                                iReturnCode = nBytes;
                            }

                            if (RecvFromBufferTable[nRecvCount % RecvFromBufferTableCount].nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recvfrom return value - EXPECTED: %d; RECEIVED: %d", RecvFromBufferTable[nRecvCount % RecvFromBufferTableCount].nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else {
                                if (0 != strncmp(SendBufferLarge, RecvBufferLarge, RecvFromBufferTable[nRecvCount % RecvFromBufferTableCount].nBufferlen)) {
                                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "recvfrom succeeded");
                }
            }
        }

        // Switch the blocking mode
        if (recvfromTable[dwTableIndex].bNonblock != bNonblocking) {
            Nonblock = (TRUE == recvfromTable[dwTableIndex].bNonblock) ? 1 : 0;
            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
            bNonblocking = recvfromTable[dwTableIndex].bNonblock;
        }

        if ((TRUE == recvfromTable[dwTableIndex].bAccept) || (TRUE == recvfromTable[dwTableIndex].bConnect) || (0 == recvfromTable[dwTableIndex].iReturnCode)) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(recvfromRequest), (char *) &recvfromRequest);
        }

        // Close the sockets
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & recvfromTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & recvfromTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & recvfromTable[dwTableIndex].dwSocket))) {
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
recvfromTestServer(
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
    u_long             FromInAddr;
    // dwMessageType is the type of received message
    DWORD              dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD              dwMessageSize;
    // pMessage is a pointer to the received message
    char               *pMessage;
    // recvfromRequest is the request
    RECVFROM_REQUEST   recvfromRequest;
    // recvfromComplete is the result
    RECVFROM_COMPLETE  recvfromComplete;

    // sSocket is the socket descriptor
    SOCKET             sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET             nsSocket;

    // iTimeout is the send and receive timeout value for the socket
    int                iTimeout = 5000;
    // bNagle indicates if Nagle is enabled
    BOOL               bNagle = FALSE;

    // localname is the local address
    SOCKADDR_IN        localname;
    // remotename is the remote address
    SOCKADDR_IN        remotename;

    // SendBuffer10 is the send buffer
    char               SendBuffer10[BUFFER_10_LEN + 1];
    // SendBufferLarge is the large send buffer
    char               SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // dwFillBuffer is a counter to fill the buffers
    DWORD              dwFillBuffer;
    // nSendCount is a counter to enumerate each send
    int                nSendCount;



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
        CopyMemory(&recvfromRequest, pMessage, sizeof(recvfromRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, recvfromRequest.nSocketType, 0);

        // Set the send and receive timeout values to 5 sec
        setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));

        if (SOCK_STREAM == recvfromRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(recvfromRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if ((SOCK_STREAM == recvfromRequest.nSocketType) && (TRUE == recvfromRequest.bServerAccept)) {
            // Place the socket in listening mode
            listen(sSocket, SOMAXCONN);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(recvfromRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        recvfromComplete.dwMessageId = RECVFROM_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(recvfromComplete), (char *) &recvfromComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if ((SOCK_STREAM == recvfromRequest.nSocketType) && (TRUE == recvfromRequest.bServerAccept)) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        if (TRUE == recvfromRequest.bRemoteShutdown) {
            // Shutdown the sockets
            if (INVALID_SOCKET != nsSocket) {
                shutdown(nsSocket, SD_BOTH);
            }

            if (INVALID_SOCKET != sSocket) {
                shutdown(sSocket, SD_BOTH);
            }
        }

        if (TRUE == recvfromRequest.bRemoteClose) {
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
        recvfromComplete.dwMessageId = RECVFROM_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(recvfromComplete), (char *) &recvfromComplete);

        if (0 < recvfromRequest.nDataBuffers) {
            // Sleep
            Sleep(SLEEP_MIDLOW_TIME);

            // Initialize the buffer
            sprintf(SendBuffer10, "%05d%05d", 1, 1);
            for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
            }

            // Send the buffer
            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, recvfromRequest.nBufferlen, 0);

            if (3 == recvfromRequest.nDataBuffers) {
                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                
                if (RECVFROM_CANCEL_MSG != ((PRECVFROM_REQUEST) pMessage)->dwMessageId) {
                    // Send the complete
                    recvfromComplete.dwMessageId = RECVFROM_COMPLETE_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(recvfromComplete), (char *) &recvfromComplete);

                    // Sleep
                    Sleep(SLEEP_MIDLOW_TIME);

                    // Initialize the buffer
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                    }

                    // Send the buffer
                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, recvfromRequest.nBufferlen, 0);

                    NetsyncFreeMessage(pMessage);

                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                    if (RECVFROM_CANCEL_MSG != ((PRECVFROM_REQUEST) pMessage)->dwMessageId) {
                        // Send the complete
                        recvfromComplete.dwMessageId = RECVFROM_COMPLETE_MSG;
                        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(recvfromComplete), (char *) &recvfromComplete);

                        // Sleep
                        Sleep(SLEEP_MIDLOW_TIME);

                        // Initialize the buffer
                        sprintf(SendBuffer10, "%05d%05d", 3, 3);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                        }

                        // Send the buffer
                        send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, recvfromRequest.nBufferlen, 0);
                    }
                }

                NetsyncFreeMessage(pMessage);
            }
            else if (100 <= recvfromRequest.nDataBuffers) {
                for (nSendCount = 0; nSendCount < recvfromRequest.nDataBuffers; nSendCount++) {
                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                    if (RECVFROM_CANCEL_MSG == ((PRECVFROM_REQUEST) pMessage)->dwMessageId) {
                        NetsyncFreeMessage(pMessage);
                        break;
                    }

                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 2 + nSendCount, 2 + nSendCount);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                    }

                    // Send the buffer
                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, ((PRECVFROM_REQUEST) pMessage)->nBufferlen, 0);

                    NetsyncFreeMessage(pMessage);

                    // Send the complete
                    recvfromComplete.dwMessageId = RECVFROM_COMPLETE_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(recvfromComplete), (char *) &recvfromComplete);
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
