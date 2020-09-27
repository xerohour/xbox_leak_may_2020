/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  connect.c

Abstract:

  This modules tests connect

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// connect messages

#define CONNECT_REQUEST_MSG   NETSYNC_MSG_USER + 120 + 1
#define CONNECT_CANCEL_MSG    NETSYNC_MSG_USER + 120 + 2
#define CONNECT_COMPLETE_MSG  NETSYNC_MSG_USER + 120 + 3

typedef struct _CONNECT_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  Port;
    BOOL     bClientAccept;
    BOOL     bServerAccept;
    int      nDataBuffers;
} CONNECT_REQUEST, *PCONNECT_REQUEST;

typedef struct _CONNECT_COMPLETE {
    DWORD    dwMessageId;
} CONNECT_COMPLETE, *PCONNECT_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

#define NAME_ANY       0
#define NAME_ANYADDR   1
#define NAME_ANYPORT   2
#define NAME_LOOPBACK  3
#define NAME_BROADCAST 4
#define NAME_MULTICAST 5
#define NAME_UNAVAIL   6
#define NAME_BAD       7
#define NAME_NULL      8
#define NAME_REMOTE    9



typedef struct CONNECT_TABLE {
    CHAR   szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL   bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL   bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD  dwSocket1;                               // dwSocket1 indicates the first socket to be created
    DWORD  dwSocket2;                               // dwSocket2 indicates the second socket to be created
    BOOL   bNonblock;                               // bNonblock indicates if the socket is to be set as non-blocking for the connect
    BOOL   bBind;                                   // bBind indicates if the socket is to be bound
    BOOL   bListen;                                 // bListen indicates if the socket is to be placed in the listening state
    BOOL   bAccept;                                 // bAccept indicates if the socket is accepted
    BOOL   bPreConnect;                             // bPreConnect indicates if a connection attempt is made before it could succeed
    BOOL   bFirstConnect;                           // bFirstConnect indicates if the socket is connected
    BOOL   bSecondConnect;                          // bSecondConnect indicates if the socket will be connected
    BOOL   bFirstSend;                              // bFirstSend indicates if data will be sent on the socket after the connect but before the accept
    BOOL   bSecondSend;                             // bSecondSend indicates if data will be sent on the socket after the connect and accept
    int    nDataBuffers;                            // nDataBuffers indicates the number of data buffers to be received after the connect
    DWORD  dwFirstName;                             // dwFirstName indicates the first connect address
    DWORD  dwSecondName;                            // dwSecondName indicates the second connect address
    int    namelen;                                 // namelen is a pointer to the length of the address buffer
    int    iReturnCode;                             // iReturnCode is the return code of connect
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} CONNECT_TABLE, *PCONNECT_TABLE;

static CONNECT_TABLE connectTable[] =
{
    { "12.1 Not Initialized",           FALSE, FALSE, SOCKET_INVALID_SOCKET,         0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "12.2 s = INT_MIN",               TRUE,  TRUE,  SOCKET_INT_MIN,                0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "12.3 s = -1",                    TRUE,  TRUE,  SOCKET_NEG_ONE,                0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "12.4 s = 0",                     TRUE,  TRUE,  SOCKET_ZERO,                   0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "12.5 s = INT_MAX",               TRUE,  TRUE,  SOCKET_INT_MAX,                0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "12.6 s = INVALID_SOCKET",        TRUE,  TRUE,  SOCKET_INVALID_SOCKET,         0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "12.7 Not Bound TCP",             TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.8 Not Bound 1 Send TCP",      TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.9 Not Bound 2 Send TCP",      TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.10 Not Bound Recv TCP",       TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 1, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.11 Not Bound Switch TCP",     TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 3, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.12 Bound TCP",                TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.13 Bound 1 Send TCP",         TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.14 Bound 2 Send TCP",         TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.15 Bound Recv TCP",           TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 1, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.16 Bound Switch TCP",         TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 3, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.17 Listening TCP",            TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "12.18 Accepted TCP",             TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEISCONN,        FALSE },
    { "12.19 Connected TCP",            TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0, NAME_REMOTE,   NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEISCONN,        FALSE },
    { "12.20 Loopback TCP",             TRUE,  TRUE,  SOCKET_TCP,                    SOCKET_TCP, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.21 Loopback 1 Send TCP",      TRUE,  TRUE,  SOCKET_TCP,                    SOCKET_TCP, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  0, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.22 Loopback 2 Send TCP",      TRUE,  TRUE,  SOCKET_TCP,                    SOCKET_TCP, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  0, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.23 Loopback Recv TCP",        TRUE,  TRUE,  SOCKET_TCP,                    SOCKET_TCP, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 1, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.24 Refused TCP",              TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAECONNREFUSED,   FALSE },
    { "12.25 Retry TCP",                TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.26 Not Bound NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.27 Not Bound 1 Send NB TCP",  TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.28 Not Bound 2 Send NB TCP",  TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.29 Not Bound Recv NB TCP",    TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 1, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.30 Not Bound Switch NB TCP",  TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 3, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.31 Bound NB TCP",             TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.32 Bound 1 Send NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.33 Bound 2 Send NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.34 Bound Recv NB TCP",        TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 1, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.35 Bound Switch NB TCP",      TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 3, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.36 Listening NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "12.37 Accepted NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEISCONN,        FALSE },
    { "12.38 Connected NB TCP",         TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, 0, NAME_REMOTE,   NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEISCONN,        FALSE },
    { "12.39 Loopback NB TCP",          TRUE,  TRUE,  SOCKET_TCP,                    SOCKET_TCP, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.40 Loopback 1 Send NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                    SOCKET_TCP, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  0, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.41 Loopback 2 Send NB TCP",   TRUE,  TRUE,  SOCKET_TCP,                    SOCKET_TCP, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  0, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.42 Loopback Recv NB TCP",     TRUE,  TRUE,  SOCKET_TCP,                    SOCKET_TCP, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 1, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.43 Refused NB TCP",           TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAECONNREFUSED,   FALSE },
    { "12.44 Retry NB TCP",             TRUE,  TRUE,  SOCKET_TCP,                    0,          TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.45 Not Bound UDP",            TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.46 Not Bound Send UDP",       TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.47 Bound UDP",                TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.48 Bound Send UDP",           TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.49 Bound Recv UDP",           TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 1, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.50 Bound Switch UDP",         TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 3, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.51 Connected UDP",            TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0, NAME_REMOTE,   NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.52 Connected Send UDP",       TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  0, NAME_REMOTE,   NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.53 Connected Recv UDP",       TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 1, NAME_REMOTE,   NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.54 Connected Switch UDP",     TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 3, NAME_REMOTE,   NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.55 Change UDP",               TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0, NAME_LOOPBACK, NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.56 Change Send UDP",          TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  0, NAME_LOOPBACK, NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.57 Change Recv UDP",          TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 1, NAME_LOOPBACK, NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.58 Change Switch UDP",        TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 3, NAME_LOOPBACK, NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.59 Reject Recv UDP",          TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 2, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.60 Flush Recv UDP",           TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 4, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.61 Reset Send UDP",           TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  0, NAME_REMOTE,   NAME_ANY,       sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.62 Reset Recv UDP",           TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 5, NAME_REMOTE,   NAME_ANY,       sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.63 Loopback UDP",             TRUE,  TRUE,  SOCKET_UDP,                    SOCKET_UDP, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.64 Loopback Send UDP",        TRUE,  TRUE,  SOCKET_UDP,                    SOCKET_UDP, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  0, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.65 Loopback Recv UDP",        TRUE,  TRUE,  SOCKET_UDP,                    SOCKET_UDP, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 1, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.66 Not Bound NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.67 Not Bound Send NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.68 Bound NB UDP",             TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.69 Bound Send NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.70 Bound Recv NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 1, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.71 Bound Switch NB UDP",      TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 3, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.72 Connected NB UDP",         TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0, NAME_REMOTE,   NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.73 Connected Send NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  0, NAME_REMOTE,   NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.74 Connected Recv NB UDP",    TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 1, NAME_REMOTE,   NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.75 Connected Switch NB UDP",  TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 3, NAME_REMOTE,   NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.76 Change NB UDP",            TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0, NAME_LOOPBACK, NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.77 Change Send NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  0, NAME_LOOPBACK, NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.78 Change Recv NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 1, NAME_LOOPBACK, NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.79 Change Switch NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 3, NAME_LOOPBACK, NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.80 Reject Recv NB UDP",       TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 2, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.81 Flush Recv NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 4, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.82 Reset Send NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  0, NAME_REMOTE,   NAME_ANY,       sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.83 Reset Recv NB UDP",        TRUE,  TRUE,  SOCKET_UDP,                    0,          TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 5, NAME_REMOTE,   NAME_ANY,       sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.84 Loopback NB UDP",          TRUE,  TRUE,  SOCKET_UDP,                    SOCKET_UDP, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.85 Loopback Send NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                    SOCKET_UDP, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  0, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.86 Loopback Recv NB UDP",     TRUE,  TRUE,  SOCKET_UDP,                    SOCKET_UDP, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 1, NAME_NULL,     NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.87 addr = NULL TCP",          TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_NULL,      sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "12.88 Broadcast TCP",            TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_BROADCAST, sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "12.89 Multicast TCP",            TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_MULTICAST, sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "12.90 Unavail addr TCP",         TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_UNAVAIL,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "12.91 Bad addr TCP",             TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_BAD,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "12.92 Zero addr TCP",            TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_ANY,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "12.93 Any addr TCP",             TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_ANYADDR,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "12.94 Zero port addr TCP",       TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_ANYPORT,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "12.95 Large addrlen TCP",        TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "12.96 Exact addrlen TCP",        TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.97 Small addrlen TCP",        TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN) - 1, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "12.98 Zero addrlen TCP",         TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "12.99 Neg addrlen TCP",          TRUE,  TRUE,  SOCKET_TCP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    -1,                      SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "12.100 addr = NULL UDP",         TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_NULL,      sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "12.101 Broadcast UDP",           TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_BROADCAST, sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEACCES,         FALSE },
    { "12.102 Broadcast enabled UDP",   TRUE,  TRUE,  SOCKET_UDP | SOCKET_BROADCAST, 0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_BROADCAST, sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.103 Multicast UDP",           TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_MULTICAST, sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "12.104 Unavail addr UDP",        TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_UNAVAIL,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "12.105 Bad addr UDP",            TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_BAD,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "12.106 Zero addr UDP",           TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_ANY,       sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.107 Any addr UDP",            TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_ANYADDR,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "12.108 Zero port addr UDP",      TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_ANYPORT,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRNOTAVAIL,  FALSE },
    { "12.109 Large addrlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "12.110 Exact addrlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "12.111 Small addrlen UDP",       TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN) - 1, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "12.112 Zero addrlen UDP",        TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "12.113 Neg addrlen UDP",         TRUE,  TRUE,  SOCKET_UDP,                    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    -1,                      SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "12.114 Closed Socket TCP",       TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED,    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "12.115 Closed Socket UDP",       TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED,    0,          FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "12.116 Not Initialized",         FALSE, FALSE, SOCKET_INVALID_SOCKET,         0,          FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, NAME_NULL,     NAME_REMOTE,    sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define connectTableCount (sizeof(connectTable) / sizeof(CONNECT_TABLE))

NETSYNC_TYPE_THREAD  connectTestSessionNt =
{
    1,
    connectTableCount * 2,
    L"xnetapi_nt.dll",
    "connectTestServer"
};

NETSYNC_TYPE_THREAD  connectTestSessionXbox =
{
    1,
    connectTableCount * 2,
    L"xnetapi_xbox.dll",
    "connectTestServer"
};



VOID
connectTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests connect - Client side

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
    // connectRequest is the request sent to the server
    CONNECT_REQUEST        connectRequest;

    // sSocket1 is the first socket descriptor
    SOCKET                 sSocket1;
    // sSocket2 is the second socket descriptor
    SOCKET                 sSocket2;
    // nsSocket1 is the first accepted socket descriptor
    SOCKET                 nsSocket1;
    // nsSocket2 is the second accepted socket descriptor
    SOCKET                 nsSocket2;

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
    // remotename is the remote address
    SOCKADDR_IN            remotename;

    // readfds is the set of sockets to check for a read condition
    fd_set                 readfds;
    // writefds is the set of sockets to check for a write condition
    fd_set                 writefds;
    // exceptfds is the set of sockets to check for an except condition
    fd_set                 exceptfds;

    // SendBuffer10 is the send buffer
    char                   SendBuffer10[BUFFER_10_LEN + 1];
    // RecvBuffer10 is the recv buffer
    char                   RecvBuffer10[BUFFER_10_LEN + 1];

    // dwFirstTime is the first tick count
    DWORD                  dwFirstTime;
    // dwSecondTime is the second tick count
    DWORD                  dwSecondTime;

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
    sprintf(szFunctionName, "connect v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_connect+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_connect-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = connectTestSessionXbox;
    }
    else {
        NetsyncTypeSession = connectTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < connectTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, connectTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, connectTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != connectTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, connectTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == connectTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != connectTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == connectTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = connectTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == connectTable[dwTableIndex].bNetsyncConnected)) {
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
        sSocket1 = INVALID_SOCKET;
        nsSocket1 = INVALID_SOCKET;
        if (SOCKET_INT_MIN == connectTable[dwTableIndex].dwSocket1) {
            sSocket1 = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == connectTable[dwTableIndex].dwSocket1) {
            sSocket1 = -1;
        }
        else if (SOCKET_ZERO == connectTable[dwTableIndex].dwSocket1) {
            sSocket1 = 0;
        }
        else if (SOCKET_INT_MAX == connectTable[dwTableIndex].dwSocket1) {
            sSocket1 = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == connectTable[dwTableIndex].dwSocket1) {
            sSocket1 = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & connectTable[dwTableIndex].dwSocket1)) {
            sSocket1 = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & connectTable[dwTableIndex].dwSocket1)) {
            sSocket1 = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Create the socket
        sSocket2 = INVALID_SOCKET;
        nsSocket2 = INVALID_SOCKET;
        if (0 != (SOCKET_TCP & connectTable[dwTableIndex].dwSocket2)) {
            sSocket2 = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & connectTable[dwTableIndex].dwSocket2)) {
            sSocket2 = socket(AF_INET, SOCK_DGRAM, 0);
        }

        if (0 != (SOCKET_BROADCAST & connectTable[dwTableIndex].dwSocket1)) {
            setsockopt(sSocket1, SOL_SOCKET, SO_BROADCAST, (char *) &bBroadcast, sizeof(bBroadcast));
        }

        // Set the send and receive timeout values to 5 sec
        if ((0 != (SOCKET_TCP & connectTable[dwTableIndex].dwSocket1)) || (0 != (SOCKET_UDP & connectTable[dwTableIndex].dwSocket1))) {
            setsockopt(sSocket1, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
            setsockopt(sSocket1, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & connectTable[dwTableIndex].dwSocket1)) {
            setsockopt(sSocket1, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the socket to non-blocking mode
        if (TRUE == connectTable[dwTableIndex].bNonblock) {
            Nonblock = 1;
            ioctlsocket(sSocket1, FIONBIO, &Nonblock);
            bNonblocking = TRUE;

            if (INVALID_SOCKET != sSocket2) {
                Nonblock = 1;
                ioctlsocket(sSocket2, FIONBIO, &Nonblock);
                bNonblocking = TRUE;
            }
        }
        else {
            bNonblocking = FALSE;
        }

        if (INVALID_SOCKET != sSocket2) {
            // Bind the socket
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort + 1);
            bind(sSocket2, (SOCKADDR *) &localname, sizeof(localname));

            if (0 != (SOCKET_TCP & connectTable[dwTableIndex].dwSocket2)) {
                // Place the socket in the listening state
                listen(sSocket2, SOMAXCONN);
            }
        }

        // Bind the socket
        if (TRUE == connectTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket1, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Place the socket in the listening state
        if (TRUE == connectTable[dwTableIndex].bListen) {
            listen(sSocket1, SOMAXCONN);
        }

        if (TRUE == connectTable[dwTableIndex].bAccept) {
            // Initialize the connect request
            connectRequest.dwMessageId = CONNECT_REQUEST_MSG;
            if (0 != (SOCKET_TCP & connectTable[dwTableIndex].dwSocket1)) {
                connectRequest.nSocketType = SOCK_STREAM;
            }
            else if (0 != (SOCKET_UDP & connectTable[dwTableIndex].dwSocket1)) {
                connectRequest.nSocketType = SOCK_DGRAM;
            }
            connectRequest.Port = CurrentPort;
            connectRequest.bClientAccept = TRUE;
            connectRequest.bServerAccept = FALSE;
            connectRequest.nDataBuffers = 0;

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(connectRequest), (char *) &connectRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            // Accept the socket
            nsSocket1 = accept(sSocket1, NULL, NULL);
        }
        else {
            if (TRUE == connectTable[dwTableIndex].bPreConnect) {
                // Connect the socket
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = NetsyncInAddr;
                remotename.sin_port = htons(CurrentPort);

                connect(sSocket1, (SOCKADDR *) &remotename, sizeof(remotename));

                if (TRUE == bNonblocking) {
                    FD_ZERO(&writefds);
                    FD_SET(sSocket1, &writefds);

                    // Wait for connect to complete
                    select(0, NULL, &writefds, NULL, NULL);
                }
            }

            if ((TRUE == connectTable[dwTableIndex].bFirstConnect) || (TRUE == connectTable[dwTableIndex].bSecondConnect)) {
                // Initialize the connect request
                connectRequest.dwMessageId = CONNECT_REQUEST_MSG;
                if (0 != (SOCKET_TCP & connectTable[dwTableIndex].dwSocket1)) {
                    connectRequest.nSocketType = SOCK_STREAM;
                }
                else if (0 != (SOCKET_UDP & connectTable[dwTableIndex].dwSocket1)) {
                    connectRequest.nSocketType = SOCK_DGRAM;
                }
                connectRequest.Port = CurrentPort;
                connectRequest.bClientAccept = FALSE;
                connectRequest.bServerAccept = connectTable[dwTableIndex].bSecondConnect;
                connectRequest.nDataBuffers = connectTable[dwTableIndex].nDataBuffers;

                // Send the connect request
                NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(connectRequest), (char *) &connectRequest);

                // Wait for the connect complete
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                NetsyncFreeMessage(pMessage);

                if (TRUE == connectTable[dwTableIndex].bFirstConnect) {
                    // Connect the socket
                    ZeroMemory(&remotename, sizeof(remotename));
                    remotename.sin_family = AF_INET;

                    if (NAME_REMOTE == connectTable[dwTableIndex].dwSecondName) {
                        remotename.sin_addr.s_addr = NetsyncInAddr;
                        remotename.sin_port = htons(CurrentPort);
                    }

                    connect(sSocket1, (SOCKADDR *) &remotename, sizeof(remotename));

                    if ((TRUE == bNonblocking) && (0 != (SOCKET_TCP & connectTable[dwTableIndex].dwSocket1))) {
                        FD_ZERO(&writefds);
                        FD_SET(sSocket1, &writefds);

                        // Wait for connect to complete
                        select(0, NULL, &writefds, NULL, NULL);
                    }
                }
            }
        }

        // Set the remote name
        ZeroMemory(&remotename, sizeof(remotename));
        remotename.sin_family = AF_INET;

        if (NAME_ANYADDR == connectTable[dwTableIndex].dwSecondName) {
            remotename.sin_port = htons(CurrentPort);
        }
        else if (NAME_ANYPORT == connectTable[dwTableIndex].dwSecondName) {
            remotename.sin_addr.s_addr = NetsyncInAddr;
        }
        else if (NAME_LOOPBACK == connectTable[dwTableIndex].dwSecondName) {
            remotename.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            remotename.sin_port = htons(CurrentPort + 1);
        }
        else if (NAME_BROADCAST == connectTable[dwTableIndex].dwSecondName) {
            remotename.sin_addr.s_addr = htonl(INADDR_BROADCAST);
            remotename.sin_port = htons(CurrentPort);
        }
        else if (NAME_MULTICAST == connectTable[dwTableIndex].dwSecondName) {
            remotename.sin_addr.s_addr = inet_addr("224.0.0.0");
            remotename.sin_port = htons(CurrentPort);
        }
        else if (NAME_UNAVAIL == connectTable[dwTableIndex].dwSecondName) {
            remotename.sin_addr.s_addr = inet_addr("127.0.0.0");
            remotename.sin_port = htons(CurrentPort);
        }
        else if (NAME_BAD == connectTable[dwTableIndex].dwSecondName) {
            remotename.sin_family = AF_UNIX;
            remotename.sin_addr.s_addr = NetsyncInAddr;
            remotename.sin_port = htons(CurrentPort);
        }
        else if (NAME_REMOTE == connectTable[dwTableIndex].dwSecondName) {
            remotename.sin_addr.s_addr = NetsyncInAddr;
            remotename.sin_port = htons(CurrentPort);
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & connectTable[dwTableIndex].dwSocket1)) {
            closesocket(sSocket1);
        }

        __try {
            // Call connect
            iReturnCode = connect((INVALID_SOCKET == nsSocket1) ? sSocket1 : nsSocket1, (NAME_NULL != connectTable[dwTableIndex].dwSecondName) ? (SOCKADDR *) &remotename : NULL, connectTable[dwTableIndex].namelen);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == connectTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "connect RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "connect caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if ((TRUE == bNonblocking) && (0 != (SOCKET_TCP & connectTable[dwTableIndex].dwSocket1)) && ((0 == connectTable[dwTableIndex].iReturnCode) || (WSAECONNREFUSED == connectTable[dwTableIndex].iLastError))) {
                // Non-blocking socket
                if (SOCKET_ERROR != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "connect returned non-SOCKET_ERROR");
                }
                else {
                    // Get the last error code
                    iLastError = WSAGetLastError();

                    if (iLastError != WSAEWOULDBLOCK) {
                        xLog(hLog, XLL_FAIL, "connect iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                    }
                    else {
                        xLog(hLog, XLL_PASS, "connect iLastError - OUT: %u", iLastError);

                        FD_ZERO(&writefds);
                        FD_ZERO(&exceptfds);
                        FD_SET(sSocket1, &writefds);
                        FD_SET(sSocket1, &exceptfds);

                        // Wait for connect to complete
                        select(0, NULL, &writefds, &exceptfds, NULL);

                        if (0 == connectTable[dwTableIndex].iReturnCode) {
                            if ((0 != FD_ISSET(sSocket1, &writefds)) && (0 == FD_ISSET(sSocket1, &exceptfds))) {
                                iReturnCode = 0;
                            }
                            else {
                                iReturnCode = SOCKET_ERROR;
                                WSASetLastError(WSAEFAULT);
                            }
                        }
                        else {
                            iReturnCode = SOCKET_ERROR;
                            if (0 != FD_ISSET(sSocket1, &exceptfds)) {
                                WSASetLastError(WSAECONNREFUSED);
                            }
                            else {
                                WSASetLastError(WSAEFAULT);
                            }
                        }
                    }
                }
            }

            if (TRUE == connectTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "connect did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == connectTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != connectTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "connect iLastError - EXPECTED: %u; RECEIVED: %u", connectTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "connect iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "connect returned SOCKET_ERROR - ec = %u", WSAGetLastError());

                if (TRUE == connectTable[dwTableIndex].bSecondConnect) {
                    // Send the accept cancel
                    connectRequest.dwMessageId = CONNECT_CANCEL_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(connectRequest), (char *) &connectRequest);

                    // Wait for the accept complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);
                }
            }
            else if (SOCKET_ERROR == connectTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "connect returned non-SOCKET_ERROR");
            }
            else {
                if (TRUE == connectTable[dwTableIndex].bFirstSend) {
                    // Send data
                    ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                    sprintf(SendBuffer10, "%05d%05d", 1, 1);

                    iReturnCode = send(sSocket1, SendBuffer10, 10, 0);

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "send returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                    }
                    else {
                        xLog(hLog, XLL_PASS, "send succeeded");
                    }
                }

                if (TRUE == connectTable[dwTableIndex].bSecondConnect) {
                    // Send the accept request
                    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(connectRequest), (char *) &connectRequest);

                    // Wait for the accept complete
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);
                }

                if (TRUE == connectTable[dwTableIndex].bSecondSend) {
                    // Send data
                    ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                    sprintf(SendBuffer10, "%05d%05d", 2, 2);

                    iReturnCode = send(sSocket1, SendBuffer10, 10, 0);

                    if (NAME_ANY == connectTable[dwTableIndex].dwSecondName) {
                        // Send should fail
                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_PASS, "send failed");
                        }
                        else {
                            xLog(hLog, XLL_FAIL, "send succeeded");
                            bTestPassed = FALSE;
                        }
                    }
                    else {
                        // Send should succeed
                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "send failed");
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "send succeeded");
                        }
                    }
                }

                if (5 == connectTable[dwTableIndex].nDataBuffers) {
                    FD_ZERO(&readfds);
                    FD_SET(sSocket1, &readfds);

                    // Wait for data to be queued
                    select(0, &readfds, NULL, NULL, NULL);

                    // Receive data
                    ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                    iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);

                    if (NAME_ANY == connectTable[dwTableIndex].dwSecondName) {
                        // Recv should succeed
                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv failed");
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "recv succeeded");
                        }
                    }
                }
                else if ((2 == connectTable[dwTableIndex].nDataBuffers) || (4 == connectTable[dwTableIndex].nDataBuffers)) {
                    // Get the current tick count
                    dwFirstTime = GetTickCount();

                    // Receive data
                    ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                    iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);

                    if ((TRUE == bNonblocking) && (SOCKET_ERROR == iReturnCode)) {
                        // Get the last error code
                        iLastError = WSAGetLastError();

                        if (iLastError != WSAEWOULDBLOCK) {
                            xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "recv iLastError - OUT: %u", iLastError);

                            FD_ZERO(&readfds);
                            FD_SET(sSocket1, &readfds);

                            // Wait for data to be queued
                            select(0, &readfds, NULL, NULL, NULL);

                            // Receive data
                            iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);
                        }
                    }

                    if (SOCKET_ERROR == iReturnCode) {
                        xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                        bTestPassed = FALSE;
                    }
                    else {
                        ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                        sprintf(SendBuffer10, "%05d%05d", 1, 1);

                        if (0 != strcmp(SendBuffer10, RecvBuffer10)) {
                            xLog(hLog, XLL_FAIL, "recv buffer - EXPECTED: %s; RECEIVED: %s", SendBuffer10, RecvBuffer10);
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "recv succeeded");
                        }
                    }

                    // Get the current tick count
                    dwSecondTime = GetTickCount();
                    if (((dwSecondTime - dwFirstTime) < SLEEP_LOW_TIME) || ((dwSecondTime - dwFirstTime) > SLEEP_HIGH_TIME)) {
                        xLog(hLog, XLL_FAIL, "recv nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME, dwSecondTime - dwFirstTime);
                        bTestPassed = FALSE;
                    }

                    if (4 == connectTable[dwTableIndex].nDataBuffers) {
                        // Reset the connection
                        ZeroMemory(&remotename, sizeof(remotename));
                        remotename.sin_family = AF_INET;
                        iReturnCode = connect(sSocket1, (SOCKADDR *) &remotename, sizeof(remotename));

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "connect returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                    }

                    if (NAME_NULL == connectTable[dwTableIndex].dwFirstName) {
                        // Check for pending data
                        ioctlsocket(sSocket1, FIONREAD, (u_long *) &iReturnCode);

                        if (0 != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "Pending data %d bytes", iReturnCode);
                            bTestPassed = FALSE;
                        }
                        else {
                            xLog(hLog, XLL_PASS, "No pending data");
                        }
                    }
                    else {
                        // Receive data
                        ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                        iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            sprintf(SendBuffer10, "%05d%05d", 2, 2);

                            if (0 != strcmp(SendBuffer10, RecvBuffer10)) {
                                xLog(hLog, XLL_FAIL, "recv buffer - EXPECTED: %s; RECEIVED: %s", SendBuffer10, RecvBuffer10);
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "recv succeeded");
                            }
                        }
                    }
                }
                else if ((1 == connectTable[dwTableIndex].nDataBuffers) || (3 == connectTable[dwTableIndex].nDataBuffers)) {
                    if (INVALID_SOCKET != sSocket2) {
                        if (0 != (SOCKET_TCP & connectTable[dwTableIndex].dwSocket2)) {
                            // Accept the connection
                            nsSocket2 = accept(sSocket2, NULL, NULL);

                            // Send data
                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            sprintf(SendBuffer10, "%05d%05d", 1, 1);

                            send(nsSocket2, SendBuffer10, 10, 0);
                        }
                        else {
                            // Initialize localsendname
                            ZeroMemory(&remotename, sizeof(remotename));
                            remotename.sin_family = AF_INET;
                            remotename.sin_port = htons(CurrentPort);
                            remotename.sin_addr.s_addr = inet_addr("127.0.0.1");

                            // Call connect
                            connect(sSocket2, (SOCKADDR *) &remotename, sizeof(remotename));

                            // Send data
                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            sprintf(SendBuffer10, "%05d%05d", 1, 1);

                            send(sSocket2, SendBuffer10, 10, 0);
                        }

                        // Receive data
                        ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                        iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);

                        if ((TRUE == bNonblocking) && (SOCKET_ERROR == iReturnCode)) {
                            // Get the last error code
                            iLastError = WSAGetLastError();

                            if (iLastError != WSAEWOULDBLOCK) {
                                xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "recv iLastError - OUT: %u", iLastError);

                                FD_ZERO(&readfds);
                                FD_SET(sSocket1, &readfds);

                                // Wait for data to be queued
                                select(0, &readfds, NULL, NULL, NULL);

                                // Receive data
                                iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);
                            }
                        }
                    
                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            sprintf(SendBuffer10, "%05d%05d", 1, 1);

                            if (0 != strcmp(SendBuffer10, RecvBuffer10)) {
                                xLog(hLog, XLL_FAIL, "recv buffer - EXPECTED: %s; RECEIVED: %s", SendBuffer10, RecvBuffer10);
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "recv succeeded");
                            }
                        }
                    }
                    else {
                        // Get the current tick count
                        dwFirstTime = GetTickCount();

                        // Receive data
                        ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                        iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);

                        if (TRUE == bNonblocking) {
                            // Non-blocking socket
                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv returned non-SOCKET_ERROR");
                                bTestPassed = FALSE;
                            }
                            else {
                                // Get the last error code
                                iLastError = WSAGetLastError();

                                if (iLastError != WSAEWOULDBLOCK) {
                                    xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                                    bTestPassed = FALSE;
                                }
                                else {
                                    xLog(hLog, XLL_PASS, "recv iLastError - OUT: %u", iLastError);

                                    FD_ZERO(&readfds);
                                    FD_SET(sSocket1, &readfds);

                                    // Wait for data to be queued
                                    select(0, &readfds, NULL, NULL, NULL);

                                    // Receive data
                                    iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);
                                }
                            }
                        }

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            sprintf(SendBuffer10, "%05d%05d", 1, 1);

                            if (0 != strcmp(SendBuffer10, RecvBuffer10)) {
                                xLog(hLog, XLL_FAIL, "recv buffer - EXPECTED: %s; RECEIVED: %s", SendBuffer10, RecvBuffer10);
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "recv succeeded");
                            }
                        }

                        // Get the current tick count
                        dwSecondTime = GetTickCount();
                        if (((dwSecondTime - dwFirstTime) < SLEEP_LOW_TIME) || ((dwSecondTime - dwFirstTime) > SLEEP_HIGH_TIME)) {
                            xLog(hLog, XLL_FAIL, "recv nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME, dwSecondTime - dwFirstTime);
                            bTestPassed = FALSE;
                        }
                    }

                    if (3 == connectTable[dwTableIndex].nDataBuffers) {
                        // Switch the blocking mode
                        Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                        ioctlsocket(sSocket1, FIONBIO, &Nonblock);
                        bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                        // Receive data
                        ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                        iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);

                        if (TRUE == bNonblocking) {
                            // Non-blocking socket
                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv returned non-SOCKET_ERROR");
                                bTestPassed = FALSE;
                            }
                            else {
                                // Get the last error code
                                iLastError = WSAGetLastError();

                                if (iLastError != WSAEWOULDBLOCK) {
                                    xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                                    bTestPassed = FALSE;
                                }
                                else {
                                    xLog(hLog, XLL_PASS, "recv iLastError - OUT: %u", iLastError);

                                    FD_ZERO(&readfds);
                                    FD_SET(sSocket1, &readfds);

                                    // Wait for data to be queued
                                    select(0, &readfds, NULL, NULL, NULL);

                                    // Receive data
                                    iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);
                                }
                            }
                        }

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            sprintf(SendBuffer10, "%05d%05d", 2, 2);

                            if (0 != strcmp(SendBuffer10, RecvBuffer10)) {
                                xLog(hLog, XLL_FAIL, "recv buffer - EXPECTED: %s; RECEIVED: %s", SendBuffer10, RecvBuffer10);
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "recv succeeded");
                            }
                        }

                        // Get the current tick count
                        dwSecondTime = GetTickCount();
                        if (((dwSecondTime - dwFirstTime) < (SLEEP_LOW_TIME * 2)) || ((dwSecondTime - dwFirstTime) > (SLEEP_HIGH_TIME * 2))) {
                            xLog(hLog, XLL_FAIL, "recv nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME * 2, dwSecondTime - dwFirstTime);
                            bTestPassed = FALSE;
                        }

                        // Switch the blocking mode
                        Nonblock = (TRUE == bNonblocking) ? 0 : 1;
                        ioctlsocket(sSocket1, FIONBIO, &Nonblock);
                        bNonblocking = (1 == Nonblock) ? TRUE : FALSE;

                        // Receive data
                        ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                        iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);

                        if (TRUE == bNonblocking) {
                            // Non-blocking socket
                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv returned non-SOCKET_ERROR");
                                bTestPassed = FALSE;
                            }
                            else {
                                // Get the last error code
                                iLastError = WSAGetLastError();

                                if (iLastError != WSAEWOULDBLOCK) {
                                    xLog(hLog, XLL_FAIL, "recv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, iLastError);
                                    bTestPassed = FALSE;
                                }
                                else {
                                    xLog(hLog, XLL_PASS, "recv iLastError - OUT: %u", iLastError);

                                    FD_ZERO(&readfds);
                                    FD_SET(sSocket1, &readfds);

                                    // Wait for data to be queued
                                    select(0, &readfds, NULL, NULL, NULL);

                                    // Receive data
                                    iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);
                                }
                            }
                        }

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            sprintf(SendBuffer10, "%05d%05d", 3, 3);

                            if (0 != strcmp(SendBuffer10, RecvBuffer10)) {
                                xLog(hLog, XLL_FAIL, "recv buffer - EXPECTED: %s; RECEIVED: %s", SendBuffer10, RecvBuffer10);
                                bTestPassed = FALSE;
                            }
                            else {
                                xLog(hLog, XLL_PASS, "recv succeeded");
                            }
                        }

                        // Get the current tick count
                        dwSecondTime = GetTickCount();
                        if (((dwSecondTime - dwFirstTime) < (SLEEP_LOW_TIME * 3)) || ((dwSecondTime - dwFirstTime) > (SLEEP_HIGH_TIME * 3))) {
                            xLog(hLog, XLL_FAIL, "recv nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME * 3, dwSecondTime - dwFirstTime);
                            bTestPassed = FALSE;
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "connect succeeded");
                }
            }
        }

        // Switch the blocking mode
        if (connectTable[dwTableIndex].bNonblock != bNonblocking) {
            Nonblock = (TRUE == connectTable[dwTableIndex].bNonblock) ? 1 : 0;
            ioctlsocket(sSocket1, FIONBIO, &Nonblock);
            bNonblocking = connectTable[dwTableIndex].bNonblock;
        }

        if ((TRUE == connectTable[dwTableIndex].bAccept) || (TRUE == connectTable[dwTableIndex].bFirstConnect) || (TRUE == connectTable[dwTableIndex].bSecondConnect)) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(connectRequest), (char *) &connectRequest);
        }

        // Close the sockets
        if (INVALID_SOCKET != nsSocket2) {
            closesocket(nsSocket2);
        }

        if (INVALID_SOCKET != nsSocket1) {
            closesocket(nsSocket1);
        }

        if (INVALID_SOCKET != sSocket2) {
            closesocket(sSocket2);
        }

        if (0 == (SOCKET_CLOSED & connectTable[dwTableIndex].dwSocket1)) {
            if ((0 != (SOCKET_TCP & connectTable[dwTableIndex].dwSocket1)) || (0 != (SOCKET_UDP & connectTable[dwTableIndex].dwSocket1))) {
                closesocket(sSocket1);
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

    // Terminate net subsystem
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
connectTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests connect - Server side

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
    // connectRequest is the request
    CONNECT_REQUEST   connectRequest;
    // connectComplete is the result
    CONNECT_COMPLETE  connectComplete;

    // sSocket1 is the first socket descriptor
    SOCKET            sSocket1;
    // sSocket2 is the second socket descriptor
    SOCKET            sSocket2;
    // nsSocket1 is the first accepted socket descriptor
    SOCKET            nsSocket1;

    // bNagle indicates if Nagle is enabled
    BOOL              bNagle = FALSE;

    // localname is the local address
    SOCKADDR_IN       localname;
    // remotename is the remote address
    SOCKADDR_IN       remotename;

    // SendBuffer10 is the send buffer
    char              SendBuffer10[BUFFER_10_LEN + 1];
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
        CopyMemory(&connectRequest, pMessage, sizeof(connectRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        nsSocket1 = INVALID_SOCKET;
        sSocket1 = socket(AF_INET, connectRequest.nSocketType, 0);

        sSocket2 = INVALID_SOCKET;
        if (2 == connectRequest.nDataBuffers) {
            sSocket2 = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Disable Nagle
        if (SOCK_STREAM == connectRequest.nSocketType) {
            setsockopt(sSocket1, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Initialize localname
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(connectRequest.Port);

        // Bind the socket
        ZeroMemory(&remotename, sizeof(remotename));
        remotename.sin_family = AF_INET;
        remotename.sin_addr.s_addr = FromInAddr;
        remotename.sin_port = htons(connectRequest.Port);
        bind(sSocket1, (SOCKADDR *) &localname, sizeof(localname));

        if (SOCK_STREAM == connectRequest.nSocketType) {
            if (TRUE == connectRequest.bClientAccept) {
                // Connect the socket
                connect(sSocket1, (SOCKADDR *) &remotename, sizeof(remotename));

                // Send the result
                connectComplete.dwMessageId = CONNECT_COMPLETE_MSG;
                NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(connectComplete), (char *) &connectComplete);
            }
            else {
                // Place the socket in listening mode
                listen(sSocket1, SOMAXCONN);

                // Send the complete
                connectComplete.dwMessageId = CONNECT_COMPLETE_MSG;
                NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(connectComplete), (char *) &connectComplete);

                if (TRUE == connectRequest.bServerAccept) {
                    // Wait for the request
                    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                    NetsyncFreeMessage(pMessage);

                    if (CONNECT_CANCEL_MSG != connectRequest.dwMessageId) {
                        // Accept the connection
                        nsSocket1 = accept(sSocket1, NULL, NULL);
                    }

                    // Send the complete
                    connectComplete.dwMessageId = CONNECT_COMPLETE_MSG;
                    NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(connectComplete), (char *) &connectComplete);
                }
            }
        }
        else {
            // Connect the socket
            connect(sSocket1, (SOCKADDR *) &remotename, sizeof(remotename));

            // Send the complete
            connectComplete.dwMessageId = CONNECT_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(connectComplete), (char *) &connectComplete);

            if (TRUE == connectRequest.bServerAccept) {
                // Wait for the request
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                NetsyncFreeMessage(pMessage);

                // Send the complete
                connectComplete.dwMessageId = CONNECT_COMPLETE_MSG;
                NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(connectComplete), (char *) &connectComplete);
            }
        }

        if (2 == connectRequest.nDataBuffers) {
            // Sleep
            Sleep(SLEEP_MIDLOW_TIME);

            // Send data
            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
            sprintf(SendBuffer10, "%05d%05d", 1, 1);

            send(sSocket1, SendBuffer10, 10, 0);
        
            // Bind the socket
            localname.sin_port = htons(connectRequest.Port + 1);
            bind(sSocket2, (SOCKADDR *) &localname, sizeof(localname));

            // Connect the socket
            connect(sSocket2, (SOCKADDR *) &remotename, sizeof(remotename));

            // Send data
            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
            sprintf(SendBuffer10, "%05d%05d", 2, 2);

            send(sSocket2, SendBuffer10, 10, 0);
        }
        else {
            for (nSendCount = 0; nSendCount < connectRequest.nDataBuffers; nSendCount++) {
                if ((4 != connectRequest.nDataBuffers) || (0 == nSendCount)) {
                    // Sleep
                    Sleep(SLEEP_MIDLOW_TIME);
                }

                // Send data
                ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                sprintf(SendBuffer10, "%05d%05d", (nSendCount + 1), (nSendCount + 1));

                send((INVALID_SOCKET == nsSocket1) ? sSocket1 : nsSocket1, SendBuffer10, 10, 0);
            }
        }

        // Wait for the ack
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        // Close the sockets
        if (INVALID_SOCKET != nsSocket1) {
            closesocket(nsSocket1);
        }

        if (INVALID_SOCKET != sSocket2) {
            closesocket(sSocket2);
        }

        if (INVALID_SOCKET != sSocket1) {
            closesocket(sSocket1);
        }
    }
}

#endif
