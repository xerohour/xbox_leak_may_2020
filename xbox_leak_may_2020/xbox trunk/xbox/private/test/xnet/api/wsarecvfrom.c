/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  wsarecvfrom.c

Abstract:

  This modules tests wsarecvfrom

Author:

  Steven Kehrli (steveke) 13-Dev-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// WSARecvFrom messages

#define WSARECVFROM_REQUEST_MSG   NETSYNC_MSG_USER + 200 + 1
#define WSARECVFROM_CANCEL_MSG    NETSYNC_MSG_USER + 200 + 2
#define WSARECVFROM_COMPLETE_MSG  NETSYNC_MSG_USER + 200 + 3

typedef struct _WSARECVFROM_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  Port;
    BOOL     bServerAccept;
    BOOL     bRemoteShutdown;
    BOOL     bRemoteClose;
    int      nDataBuffers;
    int      nBufferlen;
} WSARECVFROM_REQUEST, *PWSARECVFROM_REQUEST;

typedef struct _WSARECVFROM_COMPLETE {
    DWORD    dwMessageId;
} WSARECVFROM_COMPLETE, *PWSARECVFROM_COMPLETE;

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



typedef struct WSARECVFROMBUFFER_TABLE {
    DWORD  dwBuffer;
} WSARECVFROMBUFFER_TABLE, *PWSARECVFROMBUFFER_TABLE;

WSARECVFROMBUFFER_TABLE WSARecvFromBufferTcpTable[] = { { BUFFER_10        },
                                                        { BUFFER_NULLZERO  },
                                                        { BUFFER_TCPLARGE  } };

#define WSARecvFromBufferTcpTableCount  (sizeof(WSARecvFromBufferTcpTable) / sizeof(WSARECVFROMBUFFER_TABLE));

WSARECVFROMBUFFER_TABLE WSARecvFromBufferUdpTable[] = { { BUFFER_10        },
                                                        { BUFFER_NULLZERO  },
                                                        { BUFFER_UDPLARGE  } };

#define WSARecvFromBufferUdpTableCount  (sizeof(WSARecvFromBufferUdpTable) / sizeof(WSARECVFROMBUFFER_TABLE));



typedef struct WSARECVFROM_TABLE {
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
    BOOL           bname;                                   // bname indicates if the address buffer is used
    BOOL           bnamelen;                                // bnamelen indicates if namelen is used
    int            namelen;                                 // namelen is the length of the address buffer
    int            nOverlapped;                             // nOverlapped specifies if overlapped I/O is used
    BOOL           bCompletionRoutine;                      // bCompletionRoutine specifies if completion routine is used
    int            iReturnCode;                             // iReturnCode is the return code of WSARecvFrom
    int            iLastError;                              // iLastError is the error code if the operation failed
    BOOL           bRIP;                                    // Specifies a RIP test case
} WSARECVFROM_TABLE, *PWSARECVFROM_TABLE;

static WSARECVFROM_TABLE WSARecvFromTable[] =
{
    { "20.1 Not Initialized",            FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "20.2 s = INT_MIN",                TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "20.3 s = -1",                     TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "20.4 s = 0",                      TRUE,  TRUE,  SOCKET_ZERO,                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "20.5 s = INT_MAX",                TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "20.6 s = INVALID_SOCKET",         TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "20.7 Not Bound TCP",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "20.8 Listening TCP",              TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "20.9 Accepted TCP",               TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.10 Connected TCP",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.11 Second Receive TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SECOND,    TRUE,  TRUE,  0, TRUE,  FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.12 I/O TCP",                   TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       OVERLAPPED_IO_RESULT, FALSE, 0,            0,                 FALSE },
    { "20.13 Event TCP",                 TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "20.14 Event Event TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "20.15 SD_RECEIVE Accept TCP",     TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "20.16 SD_SEND Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.17 SD_BOTH Accept TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "20.18 SD_RECEIVE Connect TCP",    TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "20.19 SD_SEND Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.20 SD_BOTH Connect TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "20.21 Shutdown Accept TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.22 Shutdown Connect TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.23 Close Accept TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "20.24 Close Connect TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "20.25 NULL Buffer TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.26 NULL 0 Bufferlen TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,  TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.27 0 Bufferlen TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.28 Small Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,     TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.29 Exact Bufferlen TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.30 Large Buffer TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_TCPLARGE,  TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.31 Multi Buffer TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_MULTI,     TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.32 Switch Recv TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.33 Iterative Recv TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.34 NULL Buffer Ptr TCP",       TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.35 0 Buffer Count TCP",        TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT, TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.36 NULL dwBytes TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        FALSE, TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.37 NULL flags TCP",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  FALSE, 0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.38 flags != 0 TCP",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  4, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.39 Callback != NULL TCP",      TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.40 NULL addr TCP",             TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.41 addr TCP",                  TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.42 Large addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) + 1, 0,                    FALSE, 0,            0,                 FALSE },
    { "20.43 Exact addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.44 Small addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) - 1, 0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "20.45 Zero addrlen TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  0,                       0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "20.46 Neg addrlen TCP",           TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  -1,                      0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "20.47 NULL addrlen TCP",          TRUE,  TRUE,  SOCKET_TCP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  FALSE, 0,                       0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "20.48 Not Bound NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "20.49 Listening NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTCONN,       FALSE },
    { "20.50 Accepted NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.51 Connected NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.52 Second Receive NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SECOND,    TRUE,  TRUE,  0, TRUE,  FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.53 I/O NB TCP",                TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       OVERLAPPED_IO_RESULT, FALSE, 0,            0,                 FALSE },
    { "20.54 Event NB TCP",              TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "20.55 Event Event NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "20.56 SD_RECEIVE Accept NB TCP",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "20.57 SD_SEND Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.58 SD_BOTH Accept NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "20.59 SD_RECEIVE Connect NB TCP", TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "20.60 SD_SEND Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.61 SD_BOTH Connect NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "20.62 Shutdown Accept NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.63 Shutdown Connect NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.64 Close Accept NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "20.65 Close Connect NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAECONNRESET,     FALSE },
    { "20.66 NULL Buffer NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.67 NULL 0 Bufferlen NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,  TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.68 0 Bufferlen NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.69 Small Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,     TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.70 Exact Bufferlen NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.71 Large Buffer NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_TCPLARGE,  TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.72 Multi Buffer NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_MULTI,     TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.73 Switch Recv NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.74 Iterative Recv NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.75 NULL Buffer Ptr NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.76 0 Buffer Count NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT, TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.77 NULL dwBytes NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        FALSE, TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.78 NULL flags NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  FALSE, 0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.79 flags != 0 NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  4, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.80 Callback != NULL NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.81 NULL addr NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.82 addr NB TCP",               TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.83 Large addrlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) + 1, 0,                    FALSE, 0,            0,                 FALSE },
    { "20.84 Exact addrlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.85 Small addrlen NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) - 1, 0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "20.86 Zero addrlen NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  0,                       0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "20.87 Neg addrlen NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  -1,                      0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "20.88 NULL addrlen NB TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  FALSE, 0,                       0,                    FALSE, 0,            WSAEFAULT,         TRUE  },
    { "20.89 Not Bound UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "20.90 Not Connected UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.91 Connected UDP",             TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.92 Second Receive UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SECOND,    TRUE,  TRUE,  0, TRUE,  FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "20.93 SD_RECEIVE Conn UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "20.94 SD_SEND Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.95 SD_BOTH Conn UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "20.96 I/O UDP",                   TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       OVERLAPPED_IO_RESULT, FALSE, 0,            0,                 FALSE },
    { "20.97 Event UDP",                 TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "20.98 Event Event UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "20.99 NULL Buffer UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.100 NULL 0 Bufferlen UDP",     TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,  TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.101 0 Bufferlen UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.102 Small Bufferlen UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,     TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "20.103 Exact Bufferlen UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.104 Large Buffer UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPLARGE,  TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.105 Multi Buffer UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_MULTI,     TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.106 Switch Recv UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.107 Iterative Recv UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.108 Null Buffer Ptr UDP",      TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.109 0 Buffer Count UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT, TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.110 NULL dwBytes UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        FALSE, TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.111 NULL flags UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  FALSE, 0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.112 flags != 0 UDP",           TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  4, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.113 Callback != NULL UDP",     TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.114 NULL addr UDP",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.115 Not Connect addr UDP",     TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.116 Connected addr UDP",       TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.117 Large addrlen UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) + 1, 0,                    FALSE, 0,            0,                 FALSE },
    { "20.118 Exact addrlen UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.119 Small addrlen UDP",        TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) - 1, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.120 Zero addrlen UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.121 Neg addrlen UDP",          TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  -1,                      0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.122 NULL addrlen UDP",         TRUE,  TRUE,  SOCKET_UDP,                 FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.123 Not Bound NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "20.124 Not Connected NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.125 Connected NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.126 Second Receive NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SECOND,    TRUE,  TRUE,  0, TRUE,  FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "20.127 SD_RECEIVE Conn NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_RECEIVE, 1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "20.128 SD_SEND Conn NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_SEND,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.129 SD_BOTH Conn NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  SD_BOTH,    1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAESHUTDOWN,      FALSE },
    { "20.130 I/O NB UDP",               TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       OVERLAPPED_IO_RESULT, FALSE, 0,            0,                 FALSE },
    { "20.131 Event NB UDP",             TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "20.132 Event Event NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          -1,  BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       OVERLAPPED_IO_EVENT,  FALSE, 0,            0,                 FALSE },
    { "20.133 NULL Buffer NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULL,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.134 NULL 0 Bufferlen NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NULLZERO,  TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.135 0 Bufferlen NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZERO,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.136 Small Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_SMALL,     TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEMSGSIZE,       FALSE },
    { "20.137 Exact Bufferlen NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.138 Large Buffer NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_UDPLARGE,  TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.139 Multi Buffer NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_MULTI,     TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.140 Switch Recv NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          3,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.141 Iterative Recv NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          100, BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, 0,            0,                 FALSE },
    { "20.142 Null Buffer Ptr NB UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.143 0 Buffer Count NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_ZEROCOUNT, TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.144 NULL dwBytes NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        FALSE, TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.145 NULL flags NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  FALSE, 0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.146 flags != 0 NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  4, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.147 Callback != NULL NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.148 NULL addr NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, FALSE, TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.149 Not Connect addr NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.150 Connected addr NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.151 Large addrlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) + 1, 0,                    FALSE, 0,            0,                 FALSE },
    { "20.152 Exact addrlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN),     0,                    FALSE, 0,            0,                 FALSE },
    { "20.153 Small addrlen NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  sizeof(SOCKADDR_IN) - 1, 0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.154 Zero addrlen NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.155 Neg addrlen NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  TRUE,  -1,                      0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.156 NULL addrlen NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0,          1,   BUFFER_10,        TRUE,  TRUE,  0, FALSE, TRUE,  FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "20.157 Closed Socket TCP",        TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "20.158 Closed Socket UDP",        TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "20.159 Not Initialized",          FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0,          1,   BUFFER_NONE,      TRUE,  TRUE,  0, FALSE, FALSE, FALSE, 0,                       0,                    FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define WSARecvFromTableCount (sizeof(WSARecvFromTable) / sizeof(WSARECVFROM_TABLE))

NETSYNC_TYPE_THREAD  WSARecvFromTestSessionNt =
{
    1,
    WSARecvFromTableCount,
    L"xnetapi_nt.dll",
    "WSARecvFromTestServer"
};

NETSYNC_TYPE_THREAD  WSARecvFromTestSessionXbox =
{
    1,
    WSARecvFromTableCount,
    L"xnetapi_xbox.dll",
    "WSARecvFromTestServer"
};



// Callback routine

VOID CALLBACK WSARecvFromCompletionRoutine(IN DWORD  dwError, IN DWORD  cbTransferred, IN LPWSAOVERLAPPED  lpOverlapped, IN DWORD  dwFlags) { return; }



VOID
WSARecvFromTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests WSARecvFrom - Client side

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
    LPSTR                     lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR                     lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD                     dwTableIndex;

    // bWinsockInitialized indicates if Winsock is initialized
    BOOL                      bWinsockInitialized = FALSE;
    // WSAData is the details of the Winsock implementation
    WSADATA                   WSAData;

    // hNetsyncObject is a handle to the netsync object
    HANDLE                    hNetsyncObject = INVALID_HANDLE_VALUE;
    // NetsyncTypeSession is the session type descriptor
    NETSYNC_TYPE_THREAD       NetsyncTypeSession;
    // NetsyncInAddr is the address of the netsync server
    u_long                    NetsyncInAddr = 0;
    // LowPort is the low bound of the netsync port range
    u_short                   LowPort = 0;
    // HighPort is the high bound of the netsync port range
    u_short                   HighPort = 0;
    // CurrentPort is the current port in the netsync port range
    u_short                   CurrentPort = 0;
    // FromInAddr is the address of the netsync sender
    u_long                    FromInAddr;
    // dwMessageSize is the size of the received message
    DWORD                     dwMessageSize;
    // pMessage is a pointer to the received message
    char                      *pMessage;
    // WSARecvFromRequest is the request sent to the server
    WSARECVFROM_REQUEST       WSARecvFromRequest;
    
    // sSocket is the socket descriptor
    SOCKET                    sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                    nsSocket;

    // iTimeout is the send and receive timeout value for the socket
    int                       iTimeout = 5000;
    // bNagle indicates if Nagle is enabled
    BOOL                      bNagle = FALSE;
    // Nonblock sets the socket to blocking or non-blocking mode
    u_long                    Nonblock;
    // bNonblocking indicates if the socket is in non-blocking mode
    BOOL                      bNonblocking = FALSE;

    // localname is the local address
    SOCKADDR_IN               localname;
    // remotename is the remote address
    SOCKADDR_IN               remotename;

    // readfds is the set of sockets to check for a read condition
    fd_set                    readfds;
    // writefds is the set of sockets to check for a write condition
    fd_set                    writefds;
    // timeout is the timeout for select
    timeval                   fdstimeout = { 1, 0 };

    // SendBuffer10 is the send buffer
    char                      SendBuffer10[BUFFER_10_LEN + 1];
    // SendBufferLarge is the large send buffer
    char                      SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // RecvBuffer10 is the recv buffer
    char                      RecvBuffer10[BUFFER_10_LEN + 1];
    // RecvBufferLarge is the large recv buffer
    char                      RecvBufferLarge[BUFFER_LARGE_LEN + 1];
    // WSABuf is an array of send data structures
    WSABUF                    WSABuf[3];
    // dwBufferCount is the number of send data structures
    DWORD                     dwBufferCount;
    // nBufferlen is the total size of the data
    int                       nBufferlen;
    // dwFillBuffer is a counter to fill the buffers
    DWORD                     dwFillBuffer;
    // dwBytes is the number of bytes received
    DWORD                     dwBytes;
    // nRecvCount is a counter to enumerate each recv
    int                       nRecvCount;

    // dwFirstTime is the first tick count
    DWORD                     dwFirstTime;
    // dwSecondTime is the second tick count
    DWORD                     dwSecondTime;

    // WSARecvFromBufferTable is the test buffer table to use for iterative send
    PWSARECVFROMBUFFER_TABLE  WSARecvFromBufferTable;
    // WSARecvFromBufferTableCount is the number of elements within the test buffer table
    size_t                    WSARecvFromBufferTableCount;
    // WSAOverlapped is the overlapped I/O structure
    WSAOVERLAPPED             WSAOverlapped;
    // dwFlags specifies the receive flags
    DWORD                     dwFlags;
    // dwTotalBytes specifies the total bytes
    DWORD                     dwTotalBytes;
    // namelen is the size of the address buffer
    int                       namelen;

    // dwReturnCode is the return code of WaitForSingleObject
    DWORD                     dwReturnCode;

    // bException indicates if an exception occurred
    BOOL                      bException;
    // iReturnCode is the return code of the operation
    int                       iReturnCode;
    // iLastError is the error code if the operation failed
    int                       iLastError;
    // bTestPassed indicates if the test passed
    BOOL                      bTestPassed;

    // szFunctionName is the function name
    CHAR                      szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    sprintf(szFunctionName, "WSARecvFrom v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_WSARecvFrom+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_WSARecvFrom-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = WSARecvFromTestSessionXbox;
    }
    else {
        NetsyncTypeSession = WSARecvFromTestSessionNt;
    }

    // Initialize the overlapped I/O structure
    ZeroMemory(&WSAOverlapped, sizeof(WSAOverlapped));
    WSAOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < WSARecvFromTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, WSARecvFromTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, WSARecvFromTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != WSARecvFromTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, WSARecvFromTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == WSARecvFromTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != WSARecvFromTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == WSARecvFromTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = WSARecvFromTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == WSARecvFromTable[dwTableIndex].bNetsyncConnected)) {
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
        if (SOCKET_INT_MIN == WSARecvFromTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == WSARecvFromTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == WSARecvFromTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == WSARecvFromTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == WSARecvFromTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & WSARecvFromTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Set the send and receive timeout values to 5 sec
        if ((0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & WSARecvFromTable[dwTableIndex].dwSocket))) {
            setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
            setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the socket to non-blocking mode
        if (TRUE == WSARecvFromTable[dwTableIndex].bNonblock) {
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);
            bNonblocking = TRUE;
        }
        else {
            bNonblocking = FALSE;
        }

        // Bind the socket
        if (TRUE == WSARecvFromTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Place the socket in the listening state
        if (TRUE == WSARecvFromTable[dwTableIndex].bListen) {
            listen(sSocket, SOMAXCONN);
        }

        if (0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) {
            WSARecvFromBufferTable = WSARecvFromBufferTcpTable;
            WSARecvFromBufferTableCount = WSARecvFromBufferTcpTableCount;
        }
        else {
            WSARecvFromBufferTable = WSARecvFromBufferUdpTable;
            WSARecvFromBufferTableCount = WSARecvFromBufferUdpTableCount;
        }

        if ((TRUE == WSARecvFromTable[dwTableIndex].bAccept) || (TRUE == WSARecvFromTable[dwTableIndex].bConnect) || (0 == WSARecvFromTable[dwTableIndex].iReturnCode)) {
            // Initialize the recv request
            WSARecvFromRequest.dwMessageId = WSARECVFROM_REQUEST_MSG;
            if (0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) {
                WSARecvFromRequest.nSocketType = SOCK_STREAM;
            }
            else {
                WSARecvFromRequest.nSocketType = SOCK_DGRAM;
            }
            WSARecvFromRequest.Port = CurrentPort;
            WSARecvFromRequest.bServerAccept = WSARecvFromTable[dwTableIndex].bConnect;
            WSARecvFromRequest.bRemoteShutdown = WSARecvFromTable[dwTableIndex].bRemoteShutdown;
            WSARecvFromRequest.bRemoteClose = WSARecvFromTable[dwTableIndex].bRemoteClose;
            if (100 == WSARecvFromTable[dwTableIndex].nDataBuffers) {
                WSARecvFromRequest.nDataBuffers = WSARecvFromTable[dwTableIndex].nDataBuffers * WSARecvFromBufferTableCount;
            }
            else {
                WSARecvFromRequest.nDataBuffers = ((0 == WSARecvFromTable[dwTableIndex].iReturnCode) || (WSAEMSGSIZE == WSARecvFromTable[dwTableIndex].iLastError)) ? WSARecvFromTable[dwTableIndex].nDataBuffers : 0;
            }

            WSARecvFromRequest.nBufferlen = BUFFER_10_LEN;
            if (BUFFER_TCPLARGE == WSARecvFromTable[dwTableIndex].dwBuffer) {
                WSARecvFromRequest.nBufferlen = BUFFER_TCPLARGE_LEN;
            }
            else if (BUFFER_UDPLARGE == WSARecvFromTable[dwTableIndex].dwBuffer) {
                WSARecvFromRequest.nBufferlen = BUFFER_UDPLARGE_LEN;
            }
            else if ((BUFFER_NULL == WSARecvFromTable[dwTableIndex].dwBuffer) || ((0 != (SOCKET_UDP & WSARecvFromTable[dwTableIndex].dwSocket)) && ((BUFFER_NULLZERO == WSARecvFromTable[dwTableIndex].dwBuffer) || (BUFFER_ZERO == WSARecvFromTable[dwTableIndex].dwBuffer)))) {
                WSARecvFromRequest.nBufferlen = 0;
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvFromRequest), (char *) &WSARecvFromRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == WSARecvFromTable[dwTableIndex].bConnect) {
                // Connect the socket
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = NetsyncInAddr;
                remotename.sin_port = htons(CurrentPort);

                connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));

                if (0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&writefds);
                    FD_SET(sSocket, &writefds);
                    select(0, NULL, &writefds, NULL, NULL);
                }
            }
            else if (TRUE == WSARecvFromTable[dwTableIndex].bAccept) {
                if (0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&readfds);
                    FD_SET(sSocket, &writefds);
                    select(0, &readfds, NULL, NULL, NULL);
                }

                // Accept the socket
                nsSocket = accept(sSocket, NULL, NULL);
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvFromRequest), (char *) &WSARecvFromRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == WSARecvFromTable[dwTableIndex].bRemoteShutdown) {
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

            if (TRUE == WSARecvFromTable[dwTableIndex].bRemoteClose) {
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
        if (TRUE == WSARecvFromTable[dwTableIndex].bShutdown) {
            shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSARecvFromTable[dwTableIndex].nShutdown);
        }

        // Zero the receive buffer
        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

        // Initialize the buffers
        sprintf(SendBuffer10, "%05d%05d", 1, 1);
        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
        }

        dwBufferCount = 1;
        if (BUFFER_10 == WSARecvFromTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = BUFFER_10_LEN;
        }
        else if (BUFFER_NULL == WSARecvFromTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = NULL;

            nBufferlen = 0;
        }
        else if (BUFFER_NULLZERO == WSARecvFromTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = 0;
            WSABuf[0].buf = NULL;

            nBufferlen = 0;
        }
        else if (BUFFER_ZERO == WSARecvFromTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = 0;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = 0;
        }
        else if (BUFFER_ZEROCOUNT == WSARecvFromTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = 0;
            WSABuf[0].buf = RecvBufferLarge;

            dwBufferCount = 0;
            nBufferlen = 0;
        }
        else if (BUFFER_SMALL == WSARecvFromTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN - 1;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = BUFFER_10_LEN - 1;
        }
        else if (BUFFER_TCPLARGE == WSARecvFromTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_TCPLARGE_LEN;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = BUFFER_TCPLARGE_LEN;
        }
        else if (BUFFER_UDPLARGE == WSARecvFromTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_UDPLARGE_LEN;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = BUFFER_UDPLARGE_LEN;
        }
        else if (BUFFER_UDPTOOLARGE == WSARecvFromTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_UDPLARGE_LEN + 1;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = 0;
        }
        else if (BUFFER_MULTI == WSARecvFromTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN;
            WSABuf[0].buf = RecvBufferLarge;

            WSABuf[1].len = BUFFER_10_LEN;
            WSABuf[1].buf = &RecvBufferLarge[BUFFER_10_LEN];

            WSABuf[2].len = BUFFER_10_LEN;
            WSABuf[2].buf = &RecvBufferLarge[BUFFER_10_LEN + BUFFER_10_LEN];

            dwBufferCount = 3;
            nBufferlen = BUFFER_10_LEN + BUFFER_10_LEN + BUFFER_10_LEN;
        }
        else if (BUFFER_SECOND == WSARecvFromTable[dwTableIndex].dwBuffer) {
            WSABuf[0].len = BUFFER_10_LEN / 2;
            WSABuf[0].buf = RecvBufferLarge;

            nBufferlen = BUFFER_10_LEN;
        }

        // Initialize the remote name
        namelen = WSARecvFromTable[dwTableIndex].namelen;
        ZeroMemory(&remotename, sizeof(remotename));

        // Initialize the flags
        dwFlags = WSARecvFromTable[dwTableIndex].dwFlags;

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & WSARecvFromTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        // Get the current tick count
        dwFirstTime = GetTickCount();

        __try {
            // Call WSARecvFrom
            iReturnCode = WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, (BUFFER_NONE != WSARecvFromTable[dwTableIndex].dwBuffer) ? WSABuf : NULL, (BUFFER_NONE != WSARecvFromTable[dwTableIndex].dwBuffer) ? dwBufferCount : 0, (TRUE == WSARecvFromTable[dwTableIndex].bBytesRecv) ? &dwBytes : NULL, (TRUE == WSARecvFromTable[dwTableIndex].bFlags) ? &dwFlags : NULL, (TRUE == WSARecvFromTable[dwTableIndex].bname) ? (SOCKADDR *) &remotename : NULL, (TRUE == WSARecvFromTable[dwTableIndex].bnamelen) ? &namelen : NULL, (0 == WSARecvFromTable[dwTableIndex].nOverlapped) ? NULL : &WSAOverlapped, (FALSE == WSARecvFromTable[dwTableIndex].bCompletionRoutine) ? NULL : WSARecvFromCompletionRoutine);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == WSARecvFromTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "WSARecvFrom RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "WSARecvFrom caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (0 != WSARecvFromTable[dwTableIndex].nOverlapped) {
                if (-1 != WSARecvFromTable[dwTableIndex].nDataBuffers) {
                    if (SOCKET_ERROR != iReturnCode) {
                        bTestPassed = FALSE;
                        WSARecvFromRequest.dwMessageId = WSARECVFROM_CANCEL_MSG;
                        WSARecvFromRequest.nDataBuffers = 0;
                    }
                    else {
                        // Get the last error code
                        iLastError = WSAGetLastError();

                        if (iLastError != WSA_IO_PENDING) {
                            xLog(hLog, XLL_FAIL, "WSARecvFrom iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, iLastError);
                            bTestPassed = FALSE;
                        }
                        else {
                            if (OVERLAPPED_IO_EVENT == WSARecvFromTable[dwTableIndex].nOverlapped) {
                                // Wait on the overlapped I/O event
                                WaitForSingleObject(WSAOverlapped.hEvent, INFINITE);
                            }
                            // Get the overlapped I/O result
                            if (FALSE == WSAGetOverlappedResult((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSAOverlapped, &dwBytes, (OVERLAPPED_IO_RESULT == WSARecvFromTable[dwTableIndex].nOverlapped) ? TRUE : FALSE, &dwFlags)) {
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
                        WSARecvFromRequest.dwMessageId = WSARECVFROM_CANCEL_MSG;
                        WSARecvFromRequest.nDataBuffers = 0;
                    }
                    else {
                        dwReturnCode = WaitForSingleObject(WSAOverlapped.hEvent, 0);
                        if (WAIT_OBJECT_0 != dwReturnCode) {
                            xLog(hLog, XLL_FAIL, "WaitForSingleObject - EXPECTED: %u; RECEIVED: %u", WAIT_OBJECT_0, dwReturnCode);
                        }

                        // Get the overlapped I/O result
                        if (FALSE == WSAGetOverlappedResult((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSAOverlapped, &dwBytes, (OVERLAPPED_IO_RESULT == WSARecvFromTable[dwTableIndex].nOverlapped) ? TRUE : FALSE, &dwFlags)) {
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
            else if ((TRUE == bNonblocking) && (FALSE == WSARecvFromTable[dwTableIndex].bRemoteShutdown) && ((0 == WSARecvFromTable[dwTableIndex].iReturnCode) || (WSAEMSGSIZE == WSARecvFromTable[dwTableIndex].iLastError))) {
                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "WSARecvFrom returned non-SOCKET_ERROR");
                    bTestPassed = FALSE;
                }
                else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                    xLog(hLog, XLL_FAIL, "WSARecvFrom iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
                    bTestPassed = FALSE;
                }
                else {
                    xLog(hLog, XLL_PASS, "WSARecvFrom iLastError - OUT: %u", WSAGetLastError());

                    FD_ZERO(&readfds);
                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);

                    // Call WSARecvFrom
                    iReturnCode = WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, (TRUE == WSARecvFromTable[dwTableIndex].bname) ? (SOCKADDR *) &remotename : NULL, (TRUE == WSARecvFromTable[dwTableIndex].bnamelen) ? &namelen : NULL, NULL, NULL);
                }
            }
            
            // Get the current tick count
            dwSecondTime = GetTickCount();

            if (TRUE == WSARecvFromTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "WSARecvFrom did not RIP");
            }

            if (TRUE == WSARecvFromTable[dwTableIndex].bRemoteShutdown) {
                if (SOCKET_ERROR == iReturnCode) {
                    xLog(hLog, XLL_FAIL, "WSARecvFrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                }
                else if (0 != dwBytes) {
                    xLog(hLog, XLL_FAIL, "WSARecvFrom dwBytes - EXPECTED: %u; RECEIVED: %u", 0, dwBytes);
                }
                else {
                    xLog(hLog, XLL_PASS, "WSARecvFrom succeeded");
                }
            }
            else if (WSAEMSGSIZE == WSARecvFromTable[dwTableIndex].iLastError) {
                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "WSARecvFrom returned non-SOCKET_ERROR");
                }
                else if (WSAEMSGSIZE != WSAGetLastError()) {
                    xLog(hLog, XLL_FAIL, "WSARecvFrom iLastError - EXPECTED: %u; RECEIVED: %u", WSAEMSGSIZE, WSAGetLastError());
                }
                else {
                    if (MSG_PARTIAL != dwFlags) {
                        xLog(hLog, XLL_FAIL, "flags != MSG_PARTIAL");
                        bTestPassed = FALSE;
                    }

                    dwFlags = 0;
                    iReturnCode = WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL, NULL, NULL);

                    if (SOCKET_ERROR != iReturnCode) {
                        xLog(hLog, XLL_FAIL, "WSARecvFrom returned non-SOCKET_ERROR");
                        bTestPassed = FALSE;
                    }
                    else if (((TRUE == bNonblocking) && (WSAEWOULDBLOCK != WSAGetLastError())) || ((FALSE == bNonblocking) && (WSAETIMEDOUT != WSAGetLastError()))) {
                        xLog(hLog, XLL_FAIL, "WSARecvFrom iLastError - EXPECTED: %u; RECEIVED: %u", (TRUE == bNonblocking) ? WSAEWOULDBLOCK : WSAETIMEDOUT, WSAGetLastError());
                        bTestPassed = FALSE;
                    }

                    if (TRUE == bTestPassed) {
                        xLog(hLog, XLL_PASS, "WSARecv succeeded");
                    }
                }
            }
            else if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == WSARecvFromTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != WSARecvFromTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "WSARecvFrom iLastError - EXPECTED: %u; RECEIVED: %u", WSARecvFromTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "WSARecvFrom iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSARecvFrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());

                if (1 < WSARecvFromRequest.nDataBuffers) {
                    // Send the WSARecvFrom cancel
                    WSARecvFromRequest.dwMessageId = WSARECVFROM_CANCEL_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvFromRequest), (char *) &WSARecvFromRequest);
                }
            }
            else if (SOCKET_ERROR == WSARecvFromTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSARecvFrom returned non-SOCKET_ERROR");
            }
            else {
                dwTotalBytes = dwBytes;

                if (((dwSecondTime - dwFirstTime) < SLEEP_LOW_TIME) || ((dwSecondTime - dwFirstTime) > SLEEP_HIGH_TIME)) {
                    xLog(hLog, XLL_FAIL, "WSARecvFrom nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME, dwSecondTime - dwFirstTime);
                    bTestPassed = FALSE;
                }

                if (TRUE == WSARecvFromTable[dwTableIndex].bname) {
                    if (AF_INET != remotename.sin_family) {
                        xLog(hLog, XLL_FAIL, "WSARecvFrom sin_family - EXPECTED: %d; RECEIVED: %d", AF_INET, remotename.sin_family);
                        bTestPassed = FALSE;
                    }

                    if (htons(CurrentPort) != remotename.sin_port) {
                        xLog(hLog, XLL_FAIL, "WSARecvFrom sin_port - EXPECTED: %d; RECEIVED: %d", htons(CurrentPort), remotename.sin_port);
                        bTestPassed = FALSE;
                    }

                    if (NetsyncInAddr != remotename.sin_addr.s_addr) {
                        xLog(hLog, XLL_FAIL, "WSARecvFrom sin_addr - EXPECTED: %u; RECEIVED: %u", NetsyncInAddr, remotename.sin_addr.s_addr);
                        bTestPassed = FALSE;
                    }
                }

                if (0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) {
                    if (TRUE == WSARecvFromTable[dwTableIndex].bSecondRecv) {
                        if (WSABuf[0].len != dwBytes) {
                            xLog(hLog, XLL_FAIL, "WSARecvFrom return value - EXPECTED: %d; RECEIVED: %d", WSABuf[0].len, dwBytes);
                            bTestPassed = FALSE;
                        }
                        else if (0 != strncmp(SendBufferLarge, RecvBufferLarge, WSABuf[0].len)) {
                            xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                            bTestPassed = FALSE;
                        }

                        WSABuf[0].buf = &RecvBufferLarge[dwTotalBytes];
                        WSABuf[0].len = nBufferlen - dwTotalBytes;
                        iReturnCode = WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL, NULL, NULL);
                        WSABuf[0].buf = RecvBufferLarge;
                        WSABuf[0].len = nBufferlen / 2;

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "WSARecvFrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            if (WSABuf[0].len != dwBytes) {
                                xLog(hLog, XLL_FAIL, "WSARecvFrom return value - EXPECTED: %d; RECEIVED: %d", WSABuf[0].len, dwBytes);
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
                                if (SOCKET_ERROR != WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL, NULL, NULL)) {
                                    dwTotalBytes += dwBytes;
                                }
                                WSABuf[0].buf = RecvBufferLarge;
                                WSABuf[0].len = nBufferlen;
                            }
                        } while (0 != iReturnCode);
                    }
                }

                if (nBufferlen != dwTotalBytes) {
                    xLog(hLog, XLL_FAIL, "WSARecvFrom return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwTotalBytes);
                    bTestPassed = FALSE;
                }
                else {
                    if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                        xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                        bTestPassed = FALSE;
                    }
                }

                if (3 == WSARecvFromTable[dwTableIndex].nDataBuffers) {
                    // Zero the receive buffer
                    ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                    }

                    // Send the read request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvFromRequest), (char *) &WSARecvFromRequest);

                    if (WSARECVFROM_CANCEL_MSG != WSARecvFromRequest.dwMessageId) {
                        // Wait for the read complete
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);

                        // Switch the blocking mode
                        Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                        ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                        bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                        iReturnCode = WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL, NULL, NULL);

                        if (TRUE == bNonblocking) {
                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "WSARecvFrom returned non-SOCKET_ERROR");
                                bTestPassed = FALSE;
                            }
                            else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "WSARecvFrom iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "WSARecvFrom iLastError - OUT: %u", WSAGetLastError());

                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                select(0, &readfds, NULL, NULL, NULL);

                                // Call WSARecvFrom
                                iReturnCode = WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL, NULL, NULL);
                            }
                        }

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "WSARecvFrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                            WSARecvFromRequest.dwMessageId = WSARECVFROM_CANCEL_MSG;
                        }
                        else {
                            dwTotalBytes = dwBytes;

                            if (0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) {
                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                    if (1 == iReturnCode) {
                                        WSABuf[0].buf = &RecvBufferLarge[dwTotalBytes];
                                        WSABuf[0].len = nBufferlen - dwTotalBytes;
                                        if (SOCKET_ERROR != WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL, NULL, NULL)) {
                                            dwTotalBytes += dwBytes;
                                        }
                                        WSABuf[0].buf = RecvBufferLarge;
                                        WSABuf[0].len = nBufferlen;
                                    }
                                } while (0 != iReturnCode);
                            }

                            if (nBufferlen != dwTotalBytes) {
                                xLog(hLog, XLL_FAIL, "WSARecvFrom return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwTotalBytes);
                                bTestPassed = FALSE;
                            }
                            else {
                                if (0 != strncmp(SendBufferLarge, RecvBufferLarge, nBufferlen)) {
                                    xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                                    bTestPassed = FALSE;
                                }
                            }

                            WSARecvFromRequest.dwMessageId = WSARECVFROM_REQUEST_MSG;
                        }

                        // Zero the receive buffer
                        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                        // Initialize the buffers
                        sprintf(SendBuffer10, "%05d%05d", 3, 3);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                        }

                        // Send the read request
                        NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvFromRequest), (char *) &WSARecvFromRequest);

                        if (WSARECVFROM_CANCEL_MSG != WSARecvFromRequest.dwMessageId) {
                            // Wait for the read complete
                            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                            NetsyncFreeMessage(pMessage);

                            // Switch the blocking mode
                            Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
                            bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                            // Call WSARecvFrom
                            iReturnCode = WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL, NULL, NULL);

                            if (TRUE == bNonblocking) {
                                if (SOCKET_ERROR != iReturnCode) {
                                    xLog(hLog, XLL_FAIL, "WSARecvFrom returned non-SOCKET_ERROR");
                                    bTestPassed = FALSE;
                                }
                                else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                                    xLog(hLog, XLL_FAIL, "WSARecvFrom iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
                                    bTestPassed = FALSE;
                                }
                                else {
                                    xLog(hLog, XLL_PASS, "WSARecvFrom iLastError - OUT: %u", WSAGetLastError());

                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                    select(0, &readfds, NULL, NULL, NULL);

                                    // Call WSARecvFrom
                                    iReturnCode = WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL, NULL, NULL);
                                }
                            }

                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "WSARecvFrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                                bTestPassed = FALSE;
                                WSARecvFromRequest.dwMessageId = WSARECVFROM_CANCEL_MSG;
                            }
                            else {
                                dwTotalBytes = dwBytes;

                                if (0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) {
                                    do {
                                        FD_ZERO(&readfds);
                                        FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                        iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                        if (1 == iReturnCode) {
                                            WSABuf[0].buf = &RecvBufferLarge[dwTotalBytes];
                                            WSABuf[0].len = nBufferlen - dwTotalBytes;
                                            if (SOCKET_ERROR != WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL, NULL, NULL)) {
                                                dwTotalBytes += dwBytes;
                                            }
                                            WSABuf[0].buf = RecvBufferLarge;
                                            WSABuf[0].len = nBufferlen;
                                        }
                                    } while (0 != iReturnCode);
                                }

                                if (nBufferlen != dwTotalBytes) {
                                    xLog(hLog, XLL_FAIL, "WSARecvFrom return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwTotalBytes);
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
                else if (100 == WSARecvFromTable[dwTableIndex].nDataBuffers) {
                    for (nRecvCount = 0; nRecvCount < (int) (WSARecvFromTable[dwTableIndex].nDataBuffers * WSARecvFromBufferTableCount); nRecvCount++) {
                        // Zero the receive buffer
                        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));

                        // Initialize the buffers
                        sprintf(SendBuffer10, "%05d%05d", 2 + nRecvCount, 2 + nRecvCount);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_LARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_LARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_LARGE_LEN - dwFillBuffer);
                        }

                        xLog(hLog, XLL_INFO, "Iteration %d", nRecvCount);

                        dwBufferCount = 1;
                        if (BUFFER_10 == WSARecvFromTable[dwTableIndex].dwBuffer) {
                            WSABuf[0].len = BUFFER_10_LEN;
                            WSABuf[0].buf = RecvBufferLarge;

                            nBufferlen = BUFFER_10_LEN;
                        }
                        else if (BUFFER_NULLZERO == WSARecvFromTable[dwTableIndex].dwBuffer) {
                            WSABuf[0].len = 0;
                            WSABuf[0].buf = NULL;

                            nBufferlen = 0;
                        }
                        else if (BUFFER_TCPLARGE == WSARecvFromTable[dwTableIndex].dwBuffer) {
                            WSABuf[0].len = BUFFER_TCPLARGE_LEN;
                            WSABuf[0].buf = RecvBufferLarge;

                            nBufferlen = BUFFER_TCPLARGE_LEN;
                        }
                        else if (BUFFER_UDPLARGE == WSARecvFromTable[dwTableIndex].dwBuffer) {
                            WSABuf[0].len = BUFFER_UDPLARGE_LEN;
                            WSABuf[0].buf = RecvBufferLarge;

                            nBufferlen = BUFFER_UDPLARGE_LEN;
                        }

                        // Send the WSARecv request
                        WSARecvFromRequest.nBufferlen = nBufferlen;
                        NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvFromRequest), (char *) &WSARecvFromRequest);

                        if (WSARECVFROM_CANCEL_MSG == WSARecvFromRequest.dwMessageId) {
                            break;
                        }

                        // Wait for the read complete
                        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                        NetsyncFreeMessage(pMessage);

                        if ((0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) && (0 == nBufferlen)) {
                            iReturnCode = 0;
                            dwBytes = 0;
                        }
                        else {
                            if (TRUE == bNonblocking) {
                                FD_ZERO(&readfds);
                                FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);
                                select(0, &readfds, NULL, NULL, NULL);
                            }

                            // Call WSARecvFrom
                            iReturnCode = WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL, NULL, NULL);
                        }

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "WSARecvFrom returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            WSARecvFromRequest.dwMessageId = WSARECVFROM_CANCEL_MSG;
                            bTestPassed = FALSE;
                        }
                        else {
                            dwTotalBytes = dwBytes;

                            if (0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) {
                                do {
                                    FD_ZERO(&readfds);
                                    FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

                                    iReturnCode = select(0, &readfds, NULL, NULL, &fdstimeout);
                                    if (1 == iReturnCode) {
                                        WSABuf[0].buf = &RecvBufferLarge[dwTotalBytes];
                                        WSABuf[0].len = nBufferlen - dwTotalBytes;
                                        if (SOCKET_ERROR != WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, WSABuf, 1, &dwBytes, &dwFlags, NULL, NULL, NULL, NULL)) {
                                            dwTotalBytes += dwBytes;
                                        }
                                        WSABuf[0].buf = RecvBufferLarge;
                                        WSABuf[0].len = nBufferlen;
                                    }
                                } while (0 != iReturnCode);
                            }

                            if (nBufferlen != dwTotalBytes) {
                                xLog(hLog, XLL_FAIL, "WSARecvFrom return value - EXPECTED: %d; RECEIVED: %d", nBufferlen, dwTotalBytes);
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
                    xLog(hLog, XLL_PASS, "WSARecvFrom succeeded");
                }
            }
        }

        // Switch the blocking mode
        if (WSARecvFromTable[dwTableIndex].bNonblock != bNonblocking) {
            Nonblock = (TRUE == WSARecvFromTable[dwTableIndex].bNonblock) ? 1 : 0;
            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONBIO, &Nonblock);
            bNonblocking = WSARecvFromTable[dwTableIndex].bNonblock;
        }

        if ((TRUE == WSARecvFromTable[dwTableIndex].bAccept) || (TRUE == WSARecvFromTable[dwTableIndex].bConnect) || (0 == WSARecvFromTable[dwTableIndex].iReturnCode)) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSARecvFromRequest), (char *) &WSARecvFromRequest);
        }

        // Close the sockets
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & WSARecvFromTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & WSARecvFromTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & WSARecvFromTable[dwTableIndex].dwSocket))) {
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
WSARecvFromTestServer(
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
    u_long                FromInAddr;
    // dwMessageType is the type of received message
    DWORD                 dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD                 dwMessageSize;
    // pMessage is a pointer to the received message
    char                  *pMessage;
    // WSARecvFromRequest is the request
    WSARECVFROM_REQUEST   WSARecvFromRequest;
    // WSARecvComplete is the result
    WSARECVFROM_COMPLETE  WSARecvComplete;

    // sSocket is the socket descriptor
    SOCKET                sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                nsSocket;

    // iTimeout is the send and receive timeout value for the socket
    int                   iTimeout = 5000;
    // bNagle indicates if Nagle is enabled
    BOOL                  bNagle = FALSE;

    // localname is the local address
    SOCKADDR_IN           localname;
    // remotename is the remote address
    SOCKADDR_IN           remotename;

    // SendBuffer10 is the send buffer
    char                  SendBuffer10[BUFFER_10_LEN + 1];
    // SendBufferLarge is the large send buffer
    char                  SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // dwFillBuffer is a counter to fill the buffers
    DWORD                 dwFillBuffer;
    // nSendCount is a counter to enumerate each send
    int                   nSendCount;



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
        CopyMemory(&WSARecvFromRequest, pMessage, sizeof(WSARecvFromRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, WSARecvFromRequest.nSocketType, 0);

        // Set the send and receive timeout values to 5 sec
        setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));

        if (SOCK_STREAM == WSARecvFromRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(WSARecvFromRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if ((SOCK_STREAM == WSARecvFromRequest.nSocketType) && (TRUE == WSARecvFromRequest.bServerAccept)) {
            // Place the socket in listening mode
            listen(sSocket, SOMAXCONN);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(WSARecvFromRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        WSARecvComplete.dwMessageId = WSARECVFROM_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSARecvComplete), (char *) &WSARecvComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if ((SOCK_STREAM == WSARecvFromRequest.nSocketType) && (TRUE == WSARecvFromRequest.bServerAccept)) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        if (TRUE == WSARecvFromRequest.bRemoteShutdown) {
            // Shutdown the sockets
            if (INVALID_SOCKET != nsSocket) {
                shutdown(nsSocket, SD_BOTH);
            }

            if (INVALID_SOCKET != sSocket) {
                shutdown(sSocket, SD_BOTH);
            }
        }

        if (TRUE == WSARecvFromRequest.bRemoteClose) {
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

        if (-1 == WSARecvFromRequest.nDataBuffers) {
            // Initialize the buffer
            sprintf(SendBuffer10, "%05d%05d", 1, 1);
            for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
            }

            // Send the buffer
            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, WSARecvFromRequest.nBufferlen, 0);
        }

        // Send the complete
        WSARecvComplete.dwMessageId = WSARECVFROM_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSARecvComplete), (char *) &WSARecvComplete);

        if (0 < WSARecvFromRequest.nDataBuffers) {
            // Sleep
            Sleep(SLEEP_MIDLOW_TIME);

            // Initialize the buffer
            sprintf(SendBuffer10, "%05d%05d", 1, 1);
            for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
            }

            // Send the buffer
            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, WSARecvFromRequest.nBufferlen, 0);

            if (3 == WSARecvFromRequest.nDataBuffers) {
                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                
                if (WSARECVFROM_CANCEL_MSG != ((PWSARECVFROM_REQUEST) pMessage)->dwMessageId) {
                    // Send the complete
                    WSARecvComplete.dwMessageId = WSARECVFROM_COMPLETE_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSARecvComplete), (char *) &WSARecvComplete);

                    // Sleep
                    Sleep(SLEEP_MIDLOW_TIME);

                    // Initialize the buffer
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                    }

                    // Send the buffer
                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, WSARecvFromRequest.nBufferlen, 0);

                    NetsyncFreeMessage(pMessage);

                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                    if (WSARECVFROM_CANCEL_MSG != ((PWSARECVFROM_REQUEST) pMessage)->dwMessageId) {
                        // Send the complete
                        WSARecvComplete.dwMessageId = WSARECVFROM_COMPLETE_MSG;
                        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSARecvComplete), (char *) &WSARecvComplete);

                        // Sleep
                        Sleep(SLEEP_MIDLOW_TIME);

                        // Initialize the buffer
                        sprintf(SendBuffer10, "%05d%05d", 3, 3);
                        for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                            CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                        }

                        // Send the buffer
                        send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, WSARecvFromRequest.nBufferlen, 0);
                    }
                }

                NetsyncFreeMessage(pMessage);
            }
            else if (100 <= WSARecvFromRequest.nDataBuffers) {
                for (nSendCount = 0; nSendCount < WSARecvFromRequest.nDataBuffers; nSendCount++) {
                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);

                    if (WSARECVFROM_CANCEL_MSG == ((PWSARECVFROM_REQUEST) pMessage)->dwMessageId) {
                        NetsyncFreeMessage(pMessage);
                        break;
                    }

                    // Initialize the buffers
                    sprintf(SendBuffer10, "%05d%05d", 2 + nSendCount, 2 + nSendCount);
                    for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += BUFFER_10_LEN) {
                        CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > BUFFER_10_LEN ? BUFFER_10_LEN : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                    }

                    // Send the buffer
                    send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, ((PWSARECVFROM_REQUEST) pMessage)->nBufferlen, 0);

                    NetsyncFreeMessage(pMessage);

                    // Send the complete
                    WSARecvComplete.dwMessageId = WSARECVFROM_COMPLETE_MSG;
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
