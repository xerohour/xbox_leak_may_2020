/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  send.c

Abstract:

  This modules tests send

Author:

  Steven Kehrli (steveke) 28-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// send messages

#define SEND_REQUEST_MSG   NETSYNC_MSG_USER + 130 + 1
#define SEND_CANCEL_MSG    NETSYNC_MSG_USER + 130 + 2
#define SEND_COMPLETE_MSG  NETSYNC_MSG_USER + 130 + 3

typedef struct _SEND_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  Port;
    BOOL     bServerAccept;
    BOOL     bFillQueue;
    INT      nQueueLen;
    BOOL     bRemoteClose;
    int      nDataBuffers;
    int      nBufferlen;
} SEND_REQUEST, *PSEND_REQUEST;

typedef struct _SEND_COMPLETE {
    DWORD    dwMessageId;
} SEND_COMPLETE, *PSEND_COMPLETE;

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
#define BUFFER_TCPMULTI       9
#define BUFFER_UDPMULTI      10
#define BUFFER_NONE          11

#define OVERLAPPED_IO_RESULT  1
#define OVERLAPPED_IO_EVENT   2



typedef struct SENDBUFFER_TABLE {
    DWORD  dwBuffer;
    int    nBufferlen;
} SENDBUFFER_TABLE, *PSENDBUFFER_TABLE;

SENDBUFFER_TABLE SendBufferTcpTable[] = { { BUFFER_10,    BUFFER_10_LEN       },
                                          { BUFFER_NULL,  0                   },
                                          { BUFFER_LARGE, BUFFER_TCPLARGE_LEN } };

#define SendBufferTcpTableCount  (sizeof(SendBufferTcpTable) / sizeof(SENDBUFFER_TABLE));

SENDBUFFER_TABLE SendBufferUdpTable[] = { { BUFFER_10,    BUFFER_10_LEN       },
                                          { BUFFER_NULL,  0                   },
                                          { BUFFER_LARGE, BUFFER_UDPLARGE_LEN } };

#define SendBufferUdpTableCount  (sizeof(SendBufferUdpTable) / sizeof(SENDBUFFER_TABLE));



typedef struct SEND_TABLE {
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
    int    nFlags;                                  // nFlags specifies the send flags
    int    iReturnCode;                             // iReturnCode is the return code of send
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} SEND_TABLE, *PSEND_TABLE;

static SEND_TABLE sendTable[] =
{
    { "13.1 Not Initialized",            FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "13.2 s = INT_MIN",                TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "13.3 s = -1",                     TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "13.4 s = 0",                      TRUE,  TRUE,  SOCKET_ZERO,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "13.5 s = INT_MAX",                TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "13.6 s = INVALID_SOCKET",         TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "13.7 Not Bound TCP",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "13.8 Listening TCP",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "13.9 Accepted TCP",               TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.10 Connected TCP",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.11 Fill Queue TCP",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.12 SD_RECEIVE Accept TCP",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.13 SD_SEND Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "13.14 SD_BOTH Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "13.15 SD_RECEIVE Connect TCP",    TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.16 SD_SEND Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "13.17 SD_BOTH Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "13.18 Close Accept TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "13.19 Close Connect TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "13.20 NULL Buffer TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  BUFFER_10_LEN,           0, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "13.21 Neg Bufferlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    -1,                      0, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "13.22 NULL 0 Bufferlen TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, 0,            0,                 FALSE },
    { "13.23 0 Bufferlen TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    0,                       0, 0,            0,                 FALSE },
    { "13.24 Small Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN - 1,       0, 0,            0,                 FALSE },
    { "13.25 Exact Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.26 Large Buffer TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_TCPLARGE_LEN,     0, 0,            0,                 FALSE },
    { "13.27 Switch Send TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.28 Iterative Send TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.29 flags != 0 TCP",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           4, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "13.30 Not Bound NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "13.31 Listening NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "13.32 Accepted NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.33 Connected NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.34 Fill Queue NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.35 SD_RECEIVE Accept NB TCP",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.36 SD_SEND Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "13.37 SD_BOTH Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "13.38 SD_RECEIVE Connect NB TCP", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.39 SD_SEND Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "13.40 SD_BOTH Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "13.41 Close Accept NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "13.42 Close Connect NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "13.43 NULL Buffer NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  BUFFER_10_LEN,           0, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "13.44 Neg Bufferlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    -1,                      0, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "13.45 NULL 0 Bufferlen NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, 0,            0,                 FALSE },
    { "13.46 0 Bufferlen NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    0,                       0, 0,            0,                 FALSE },
    { "13.47 Small Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN - 1,       0, 0,            0,                 FALSE },
    { "13.48 Exact Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.49 Large Buffer NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_TCPLARGE_LEN,     0, 0,            0,                 FALSE },
    { "13.50 Switch Send NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.51 Iterative Send NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.52 flags != 0 NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           4, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "13.53 Not Bound UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "13.54 Not Connected UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "13.55 Connected UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.56 SD_RECEIVE Conn UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.57 SD_SEND Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "13.58 SD_BOTH Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "13.59 NULL Buffer UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  BUFFER_10_LEN,           0, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "13.60 Neg Bufferlen UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    -1,                      0, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "13.61 NULL 0 Bufferlen UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, 0,            0,                 FALSE },
    { "13.62 0 Bufferlen UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    0,                       0, 0,            0,                 FALSE },
    { "13.63 Small Bufferlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN - 1,       0, 0,            0,                 FALSE },
    { "13.64 Exact Bufferlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.65 Large Buffer UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_UDPLARGE_LEN,     0, 0,            0,                 FALSE },
    { "13.66 Too Large Buffer UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_UDPLARGE_LEN + 1, 0, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "13.67 Switch Send UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.68 Iterative Send UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.69 flags != 0 UDP",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           4, SOCKET_ERROR, WSAEFAULT,         TRUE },
    { "13.70 Not Bound NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "13.71 Not Connected NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "13.72 Connected NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.73 SD_RECEIVE Conn NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.74 SD_SEND Conn NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "13.75 SD_BOTH Conn NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,    BUFFER_10_LEN,           0, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "13.76 NULL Buffer NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  BUFFER_10_LEN,           0, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "13.77 Neg Bufferlen NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    -1,                      0, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "13.78 NULL 0 Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, 0,            0,                 FALSE },
    { "13.79 0 Bufferlen NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    0,                       0, 0,            0,                 FALSE },
    { "13.80 Small Bufferlen NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN - 1,       0, 0,            0,                 FALSE },
    { "13.81 Exact Bufferlen NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.82 Large Buffer NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_UDPLARGE_LEN,     0, 0,            0,                 FALSE },
    { "13.83 Too Large Buffer NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_LARGE, BUFFER_UDPLARGE_LEN + 1, 0, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "13.84 Switch Send NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.85 Iterative Send NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,    BUFFER_10_LEN,           0, 0,            0,                 FALSE },
    { "13.86 flags != 0 NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,    BUFFER_10_LEN,           4, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "13.87 Closed Socket TCP",         TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "13.88 Closed Socket UDP",         TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "13.89 Not Initialized",           FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,  0,                       0, SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define sendTableCount (sizeof(sendTable) / sizeof(SEND_TABLE))

NETSYNC_TYPE_THREAD  sendTestSessionNt =
{
    1,
    sendTableCount,
    L"xnetapi_nt.dll",
    "sendTestServer"
};

NETSYNC_TYPE_THREAD  sendTestSessionXbox =
{
    1,
    sendTableCount,
    L"xnetapi_xbox.dll",
    "sendTestServer"
};



VOID
sendTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests send - Client side

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
    // sendRequest is the request sent to the server
    SEND_REQUEST           sendRequest;

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
    // Buffer is a pointer to the buffer
    char                   *Buffer;
    // dwFillBuffer is a counter to fill the buffers
    DWORD                  dwFillBuffer;
    // nBytes is the number of bytes sent
    int                    nBytes;
    // nSendCount is a counter to enumerate each send/recv
    int                    nSendCount;

    // SendBufferTable is the test buffer table to use for iterative send
    PSENDBUFFER_TABLE      SendBufferTable;
    // SendBufferTableCount is the number of elements within the test buffer table
    size_t                 SendBufferTableCount;

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
    sprintf(szFunctionName, "send v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_send+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_send-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = sendTestSessionXbox;
    }
    else {
        NetsyncTypeSession = sendTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < sendTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, sendTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, sendTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != sendTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, sendTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == sendTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != sendTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == sendTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = sendTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == sendTable[dwTableIndex].bNetsyncConnected)) {
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
        if (SOCKET_INT_MIN == sendTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == sendTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == sendTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == sendTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == sendTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & sendTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & sendTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        if (TRUE == sendTable[dwTableIndex].bFillQueue) {
            // Set the buffer size
            setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
            setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
        }

        // Set the send and receive timeout values to 5 sec
        if ((0 != (SOCKET_TCP & sendTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & sendTable[dwTableIndex].dwSocket))) {
            setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
            setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & sendTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the socket to non-blocking mode
        if (TRUE == sendTable[dwTableIndex].bNonblock) {
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);
            bNonblocking = TRUE;
        }
        else {
            bNonblocking = FALSE;
        }

        // Bind the socket
        if (TRUE == sendTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Place the socket in the listening state
        if (TRUE == sendTable[dwTableIndex].bListen) {
            listen(sSocket, SOMAXCONN);
        }

        if (0 != (SOCKET_TCP & sendTable[dwTableIndex].dwSocket)) {
            SendBufferTable = SendBufferTcpTable;
            SendBufferTableCount = SendBufferTcpTableCount;
        }
        else {
            SendBufferTable = SendBufferUdpTable;
            SendBufferTableCount = SendBufferUdpTableCount;
        }

        if ((TRUE == sendTable[dwTableIndex].bAccept) || (TRUE == sendTable[dwTableIndex].bConnect)) {
            // Initialize the send request
            sendRequest.dwMessageId = SEND_REQUEST_MSG;
            if (0 != (SOCKET_TCP & sendTable[dwTableIndex].dwSocket)) {
                sendRequest.nSocketType = SOCK_STREAM;
            }
            else {
                sendRequest.nSocketType = SOCK_DGRAM;
            }
            sendRequest.Port = CurrentPort;
            sendRequest.bServerAccept = sendTable[dwTableIndex].bConnect;
            sendRequest.bFillQueue = sendTable[dwTableIndex].bFillQueue;
            sendRequest.nQueueLen = 0;
            sendRequest.bRemoteClose = sendTable[dwTableIndex].bRemoteClose;
            if (100 == sendTable[dwTableIndex].nDataBuffers) {
                sendRequest.nDataBuffers = sendTable[dwTableIndex].nDataBuffers * SendBufferTableCount;
            }
            else {
                sendRequest.nDataBuffers = (0 == sendTable[dwTableIndex].iReturnCode) ? sendTable[dwTableIndex].nDataBuffers : 0;
            }
            sendRequest.nBufferlen = sendTable[dwTableIndex].nBufferlen;

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendRequest), (char *) &sendRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == sendTable[dwTableIndex].bConnect) {
                // Connect the socket
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = NetsyncInAddr;
                remotename.sin_port = htons(CurrentPort);

                connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));

                if (0 != (SOCKET_TCP & sendTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&writefds);
                    FD_SET(sSocket, &writefds);
                    select(0, NULL, &writefds, NULL, NULL);
                }
            }
            else {
                if (0 != (SOCKET_TCP & sendTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&readfds);
                    FD_SET(sSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);
                }

                // Accept the socket
                nsSocket = accept(sSocket, NULL, NULL);
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendRequest), (char *) &sendRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == sendTable[dwTableIndex].bFillQueue) {
                if (FALSE == bNonblocking) {
                    Nonblock = 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                }

                // Fill the queue
                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                while (SOCKET_ERROR != send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0)) {
                    sendRequest.nQueueLen++;
                    Sleep(SLEEP_ZERO_TIME);
                }
            }

            if (TRUE == sendTable[dwTableIndex].bRemoteClose) {
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
        if (TRUE == sendTable[dwTableIndex].bShutdown) {
            shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, sendTable[dwTableIndex].nShutdown);
        }

        // Initialize the buffers
        sprintf(SendBuffer10, "%05d%05d", 1, 1);
        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
        }
        if (BUFFER_10 == sendTable[dwTableIndex].dwBuffer) {
            Buffer = SendBuffer10;
        }
        else if (BUFFER_LARGE == sendTable[dwTableIndex].dwBuffer) {
            Buffer = SendBufferLarge;
        }
        else {
            Buffer = NULL;
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & sendTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        __try {
            // Call send
            iReturnCode = send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, Buffer, sendTable[dwTableIndex].nBufferlen, sendTable[dwTableIndex].nFlags);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == sendTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "send RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "send caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == sendTable[dwTableIndex].bFillQueue) {
                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "send returned non-SOCKET_ERROR");
                }
                else {
                    // Get the last error code
                    iLastError = WSAGetLastError();

                    if (iLastError != WSAEWOULDBLOCK) {
                        xLog(hLog, XLL_FAIL, "send iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                    }
                }

                // Send the read request
                NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendRequest), (char *) &sendRequest);

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

                // Call send
                iReturnCode = send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, Buffer, sendTable[dwTableIndex].nBufferlen, sendTable[dwTableIndex].nFlags);
            }

            if (TRUE == sendTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "send did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == sendTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != sendTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "send iLastError - EXPECTED: %u; RECEIVED: %u", sendTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "send iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "send returned SOCKET_ERROR - ec = %u", WSAGetLastError());

                if (0 != sendRequest.nDataBuffers) {
                    // Send the send cancel
                    sendRequest.dwMessageId = SEND_CANCEL_MSG;
                    sendRequest.nDataBuffers = 0;
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendRequest), (char *) &sendRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);
                }
            }
            else if (SOCKET_ERROR == sendTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "send returned non-SOCKET_ERROR");
            }
            else {
                if (sendTable[dwTableIndex].nBufferlen != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "send return value - EXPECTED: %d; RECEIVED: %d", sendTable[dwTableIndex].nBufferlen, iReturnCode);
                    bTestPassed = FALSE;
                }

                // Send the read request
                NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendRequest), (char *) &sendRequest);

                // Wait for the read complete
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                NetsyncFreeMessage(pMessage);

                if ((TRUE != sendTable[dwTableIndex].bFillQueue) && (TRUE != sendTable[dwTableIndex].bShutdown)) {
                    // Call recv
                    if (0 != (SOCKET_TCP & sendTable[dwTableIndex].dwSocket)) {
                        if (0 != sendTable[dwTableIndex].nBufferlen) {
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
                            else if (sendTable[dwTableIndex].nBufferlen != nBytes) {
                                xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", sendTable[dwTableIndex].nBufferlen, nBytes);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, sendTable[dwTableIndex].nBufferlen)) {
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
                        else if (sendTable[dwTableIndex].nBufferlen != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", sendTable[dwTableIndex].nBufferlen, iReturnCode);
                            bTestPassed = FALSE;
                        }
                        else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, sendTable[dwTableIndex].nBufferlen)) {
                            xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                            bTestPassed = FALSE;
                        }
                    }
                }

                if (3 == sendTable[dwTableIndex].nDataBuffers) {
                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                    }
                    if (BUFFER_10 == sendTable[dwTableIndex].dwBuffer) {
                        Buffer = SendBuffer10;
                    }
                    else if (BUFFER_LARGE == sendTable[dwTableIndex].dwBuffer) {
                        Buffer = SendBufferLarge;
                    }
                    else {
                        Buffer = NULL;
                    }

                    // Switch the blocking mode
                    Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                    bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                    // Call send
                    iReturnCode = send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, Buffer, sendTable[dwTableIndex].nBufferlen, sendTable[dwTableIndex].nFlags);

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "send returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                        sendRequest.dwMessageId = SEND_CANCEL_MSG;
                    }
                    else {
                        if (sendTable[dwTableIndex].nBufferlen != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "send return value - EXPECTED: %d; RECEIVED: %d", sendTable[dwTableIndex].nBufferlen, iReturnCode);
                            bTestPassed = FALSE;
                        }

                        sendRequest.dwMessageId = SEND_REQUEST_MSG;
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendRequest), (char *) &sendRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    if (SOCKET_ERROR != iReturnCode) {
                        // Call recv
                        if (0 != (SOCKET_TCP & sendTable[dwTableIndex].dwSocket)) {
                            if (0 != sendTable[dwTableIndex].nBufferlen) {
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
                                else if (sendTable[dwTableIndex].nBufferlen != nBytes) {
                                    xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", sendTable[dwTableIndex].nBufferlen, nBytes);
                                    bTestPassed = FALSE;
                                }
                                else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, sendTable[dwTableIndex].nBufferlen)) {
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
                            else if (sendTable[dwTableIndex].nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", sendTable[dwTableIndex].nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, sendTable[dwTableIndex].nBufferlen)) {
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
                    if (BUFFER_10 == sendTable[dwTableIndex].dwBuffer) {
                        Buffer = SendBuffer10;
                    }
                    else if (BUFFER_LARGE == sendTable[dwTableIndex].dwBuffer) {
                        Buffer = SendBufferLarge;
                    }
                    else {
                        Buffer = NULL;
                    }

                    // Switch the blocking mode
                    Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                    ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                    bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                    // Call send
                    iReturnCode = send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, Buffer, sendTable[dwTableIndex].nBufferlen, sendTable[dwTableIndex].nFlags);

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "send returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                        sendRequest.dwMessageId = SEND_CANCEL_MSG;
                    }
                    else {
                        if (sendTable[dwTableIndex].nBufferlen != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "send return value - EXPECTED: %d; RECEIVED: %d", sendTable[dwTableIndex].nBufferlen, iReturnCode);
                            bTestPassed = FALSE;
                        }

                        sendRequest.dwMessageId = SEND_REQUEST_MSG;
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendRequest), (char *) &sendRequest);

                    // Wait for the read complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    if (SOCKET_ERROR != iReturnCode) {
                        // Call recv
                        if (0 != (SOCKET_TCP & sendTable[dwTableIndex].dwSocket)) {
                            if (0 != sendTable[dwTableIndex].nBufferlen) {
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
                                else if (sendTable[dwTableIndex].nBufferlen != nBytes) {
                                    xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", sendTable[dwTableIndex].nBufferlen, nBytes);
                                    bTestPassed = FALSE;
                                }
                                else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, sendTable[dwTableIndex].nBufferlen)) {
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
                            else if (sendTable[dwTableIndex].nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", sendTable[dwTableIndex].nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }
                            else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, sendTable[dwTableIndex].nBufferlen)) {
                                xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                bTestPassed = FALSE;
                            }
                        }
                    }
                }
                else if (100 == sendTable[dwTableIndex].nDataBuffers) {
                    for (nSendCount = 0; nSendCount < (int) (sendTable[dwTableIndex].nDataBuffers * SendBufferTableCount); nSendCount++) {
                        // Initialize the buffers
                        sprintf(SendBuffer10, "%05d%05d", 2 + nSendCount, 2 + nSendCount);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                        }
                        if (BUFFER_LARGE == SendBufferTable[nSendCount % SendBufferTableCount].dwBuffer) {
                            Buffer = SendBufferLarge;
                        }
                        else if (BUFFER_10 == SendBufferTable[nSendCount % SendBufferTableCount].dwBuffer) {
                            Buffer = SendBuffer10;
                        }
                        else {
                            Buffer = NULL;
                        }

                        xLog(hLog, XLL_INFO, "Iteration %d", nSendCount);

                        // Call send
                        iReturnCode = send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, Buffer, SendBufferTable[nSendCount % SendBufferTableCount].nBufferlen, 0);

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "send returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                            sendRequest.dwMessageId = SEND_CANCEL_MSG;
                        }
                        else {
                            if (SendBufferTable[nSendCount % SendBufferTableCount].nBufferlen != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "send return value - EXPECTED: %d; RECEIVED: %d", SendBufferTable[nSendCount % SendBufferTableCount].nBufferlen, iReturnCode);
                                bTestPassed = FALSE;
                            }

                            sendRequest.dwMessageId = SEND_REQUEST_MSG;
                        }

                        // Send the read request
                        sendRequest.nBufferlen = SendBufferTable[nSendCount % SendBufferTableCount].nBufferlen;
                        NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendRequest), (char *) &sendRequest);

                        // Wait for the read complete
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);

                        if (SOCKET_ERROR != iReturnCode) {
                            // Call recv
                            if (0 != (SOCKET_TCP & sendTable[dwTableIndex].dwSocket)) {
                                if (0 != SendBufferTable[nSendCount % SendBufferTableCount].nBufferlen) {
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
                                    else if (SendBufferTable[nSendCount % SendBufferTableCount].nBufferlen != nBytes) {
                                        xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", SendBufferTable[nSendCount % SendBufferTableCount].nBufferlen, nBytes);
                                        bTestPassed = FALSE;
                                    }
                                    else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, SendBufferTable[nSendCount % SendBufferTableCount].nBufferlen)) {
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
                                else if (SendBufferTable[nSendCount % SendBufferTableCount].nBufferlen != iReturnCode) {
                                    xLog(hLog, XLL_FAIL, "recv return value - EXPECTED: %d; RECEIVED: %d", SendBufferTable[nSendCount % SendBufferTableCount].nBufferlen, iReturnCode);
                                    bTestPassed = FALSE;
                                }
                                else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, SendBufferTable[nSendCount % SendBufferTableCount].nBufferlen)) {
                                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "send succeeded");
                }
            }
        }

        // Switch the blocking mode
        if (sendTable[dwTableIndex].bNonblock != bNonblocking) {
            Nonblock = (TRUE == sendTable[dwTableIndex].bNonblock) ? 1 : 0;
            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
            bNonblocking = sendTable[dwTableIndex].bNonblock;
        }

        if ((TRUE == sendTable[dwTableIndex].bAccept) || (TRUE == sendTable[dwTableIndex].bConnect)) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(sendRequest), (char *) &sendRequest);
        }

        // Close the sockets
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & sendTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & sendTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & sendTable[dwTableIndex].dwSocket))) {
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
sendTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests send - Server side

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
    // sendRequest is the request
    SEND_REQUEST   sendRequest;
    // sendComplete is the result
    SEND_COMPLETE  sendComplete;

    // sSocket is the socket descriptor
    SOCKET         sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET         nsSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD          dwBufferSize = 1;
    // iTimeout is the send and receive timeout value for the socket
    int            iTimeout = 5000;
    // bNagle indicates if Nagle is enabled
    BOOL           bNagle = FALSE;
    // nQueueLen is the size of the queue
    int            nQueueLen;

    // localname is the local address
    SOCKADDR_IN    localname;
    // remotename is the remote address
    SOCKADDR_IN    remotename;

    // readfds is the set of sockets to check for a read condition
    fd_set         readfds;
    // timeout is the timeout for select
    timeval        fdstimeout = { 1, 0 };

    // SendBufferLarge is the large send buffer
    char           SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // nBytes is the number of bytes sent/received
    int            nBytes;
    // nSendCount is a counter to enumerate each send
    int            nSendCount;

    // iReturnCode is the return code of the operation
    int            iReturnCode;



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
        CopyMemory(&sendRequest, pMessage, sizeof(sendRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, sendRequest.nSocketType, 0);

        if (TRUE == sendRequest.bFillQueue) {
            // Set the buffer size
            setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
            setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
        }

        // Set the send and receive timeout values to 5 sec
        setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));

        if (SOCK_STREAM == sendRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(sendRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if ((SOCK_STREAM == sendRequest.nSocketType) && (TRUE == sendRequest.bServerAccept)) {
            // Place the socket in listening mode
            listen(sSocket, SOMAXCONN);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(sendRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        sendComplete.dwMessageId = SEND_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendComplete), (char *) &sendComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if ((SOCK_STREAM == sendRequest.nSocketType) && (TRUE == sendRequest.bServerAccept)) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        if (TRUE == sendRequest.bRemoteClose) {
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
        sendComplete.dwMessageId = SEND_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendComplete), (char *) &sendComplete);

        if (TRUE == sendRequest.bFillQueue) {
            // Wait for the request
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            nQueueLen = ((PSEND_REQUEST) pMessage)->nQueueLen;
            CopyMemory(&sendRequest, pMessage, sizeof(sendRequest));
            NetsyncFreeMessage(pMessage);

            if (SEND_CANCEL_MSG != sendRequest.dwMessageId) {
                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                for (nSendCount = 0; nSendCount < nQueueLen; nSendCount++) {
                    recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0);
                }
            }

            // Send the complete
            sendComplete.dwMessageId = SEND_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendComplete), (char *) &sendComplete);
        }

        if (0 < sendRequest.nDataBuffers) {
            // Wait for the request
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            CopyMemory(&sendRequest, pMessage, sizeof(sendRequest));
            NetsyncFreeMessage(pMessage);

            if ((SEND_CANCEL_MSG != sendRequest.dwMessageId) && ((0 != sendRequest.nBufferlen) || (SOCK_STREAM != sendRequest.nSocketType))) {
                if (SOCK_DGRAM == sendRequest.nSocketType) {
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
                    } while ((SOCKET_ERROR != iReturnCode) && (nBytes < sendRequest.nBufferlen));

                    if (nBytes == sendRequest.nBufferlen) {
                        send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                    }
                }
            }

            // Send the complete
            sendComplete.dwMessageId = SEND_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendComplete), (char *) &sendComplete);

            if (3 == sendRequest.nDataBuffers) {
                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                if (SEND_CANCEL_MSG != ((PSEND_REQUEST) pMessage)->dwMessageId) {
                    if (SOCK_DGRAM == sendRequest.nSocketType) {
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
                        } while ((SOCKET_ERROR != iReturnCode) && (nBytes < sendRequest.nBufferlen));

                        if (nBytes == sendRequest.nBufferlen) {
                            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                        }
                    }
                }
                NetsyncFreeMessage(pMessage);

                // Send the complete
                sendComplete.dwMessageId = SEND_COMPLETE_MSG;
                NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendComplete), (char *) &sendComplete);

                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                if (SEND_CANCEL_MSG != ((PSEND_REQUEST) pMessage)->dwMessageId) {
                    if (SOCK_DGRAM == sendRequest.nSocketType) {
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
                        } while ((SOCKET_ERROR != iReturnCode) && (nBytes < sendRequest.nBufferlen));

                        if (nBytes == sendRequest.nBufferlen) {
                            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                        }
                    }
                }
                NetsyncFreeMessage(pMessage);

                // Send the complete
                sendComplete.dwMessageId = SEND_COMPLETE_MSG;
                NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendComplete), (char *) &sendComplete);
            }
            else if (100 <= sendRequest.nDataBuffers) {
                for (nSendCount = 0; nSendCount < sendRequest.nDataBuffers; nSendCount++) {
                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                    if ((0 != ((PSEND_REQUEST) pMessage)->nBufferlen) || (SOCK_STREAM != sendRequest.nSocketType)) {
                        if (SEND_CANCEL_MSG != ((PSEND_REQUEST) pMessage)->dwMessageId) {
                            if (SOCK_DGRAM == sendRequest.nSocketType) {
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
                                } while ((SOCKET_ERROR != iReturnCode) && (nBytes < ((PSEND_REQUEST) pMessage)->nBufferlen));

                                if (nBytes == ((PSEND_REQUEST) pMessage)->nBufferlen) {
                                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, nBytes, 0);
                                }
                            }
                        }
                    }
                    NetsyncFreeMessage(pMessage);

                    // Send the complete
                    sendComplete.dwMessageId = SEND_COMPLETE_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(sendComplete), (char *) &sendComplete);
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
