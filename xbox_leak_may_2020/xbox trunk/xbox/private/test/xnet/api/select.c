/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  select.c

Abstract:

  This modules tests select

Author:

  Steven Kehrli (steveke) 25-Feb-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// select messages

#define SELECT_REQUEST_MSG   NETSYNC_MSG_USER + 260 + 1
#define SELECT_COMPLETE_MSG  NETSYNC_MSG_USER + 260 + 2

typedef struct _SELECT_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    int      nSockets;
    u_short  Port;
    BOOL     bServerAccept;
    BOOL     bServerConnect;
    BOOL     bServerClose;
    BOOL     bServerSend;
    DWORD    dwWaitTime;
} SELECT_REQUEST, *PSELECT_REQUEST;

typedef struct _SELECT_COMPLETE {
    DWORD    dwMessageId;
} SELECT_COMPLETE, *PSELECT_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

#define FDSET_READ        0x00000001
#define FDSET_WRITE       0x00000002
#define FDSET_EXCEPT      0x00000004

#define FUNC_ACCEPT       0x00000001
#define FUNC_ACCEPT_FAIL  0x00000002
#define FUNC_READ         0x00000010
#define FUNC_READ_FAIL    0x00000020
#define FUNC_CLOSE        0x00000100
#define FUNC_CLOSE_FAIL   0x00000200
#define FUNC_CONNECT      0x00001000
#define FUNC_CONNECT_FAIL 0x00002000
#define FUNC_SEND         0x00010000
#define FUNC_SEND_FAIL    0x00020000

typedef struct SELECT_TABLE {
    CHAR   szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL   bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL   bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD  dwSocket1;                               // dwSocket1 indicates the first socket to be created
    DWORD  dwSocket2;                               // dwSocket2 indicates the second socket to be created
    DWORD  dwAddSets;                               // dwAddSets specifies the fd_set structures to add the socket to
    DWORD  dwCheckSets;                             // dwCheckSets specifies the fd_set structures to check
    DWORD  dwFunction;                              // dwFunction specifies the function for select
    BOOL   bTimeout;                                // bTimeout indicates if the timeout is used
    long   tv_usec;                                 // timeout value, in micro-seconds
    int    iReturnCode;                             // iReturnCode is the return code of select
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} SELECT_TABLE, *PSELECT_TABLE;

static SELECT_TABLE selectTable[] =
{
    { "26.1 Not Initialized",             FALSE, FALSE, SOCKET_INVALID_SOCKET,       0,          0,                                       0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "26.2 r s = INT_MIN",               TRUE,  TRUE,  SOCKET_INT_MIN,              0,          FDSET_READ,                              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.3 r s = -1",                    TRUE,  TRUE,  SOCKET_NEG_ONE,              0,          FDSET_READ,                              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.4 r s = 0",                     TRUE,  TRUE,  SOCKET_ZERO,                 0,          FDSET_READ,                              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.5 r s = INT_MAX",               TRUE,  TRUE,  SOCKET_INT_MAX,              0,          FDSET_READ,                              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.6 r s = INVALID_SOCKET",        TRUE,  TRUE,  SOCKET_INVALID_SOCKET,       0,          FDSET_READ,                              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.7 w s = INT_MIN",               TRUE,  TRUE,  SOCKET_INT_MIN,              0,          FDSET_WRITE,                             0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.8 w s = -1",                    TRUE,  TRUE,  SOCKET_NEG_ONE,              0,          FDSET_WRITE,                             0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.9 w s = 0",                     TRUE,  TRUE,  SOCKET_ZERO,                 0,          FDSET_WRITE,                             0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.10 w s = INT_MAX",              TRUE,  TRUE,  SOCKET_INT_MAX,              0,          FDSET_WRITE,                             0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.11 w s = INVALID_SOCKET",       TRUE,  TRUE,  SOCKET_INVALID_SOCKET,       0,          FDSET_WRITE,                             0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.12 e s = INT_MIN",              TRUE,  TRUE,  SOCKET_INT_MIN,              0,          FDSET_EXCEPT,                            0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.13 e s = -1",                   TRUE,  TRUE,  SOCKET_NEG_ONE,              0,          FDSET_EXCEPT,                            0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.14 e s = 0",                    TRUE,  TRUE,  SOCKET_ZERO,                 0,          FDSET_EXCEPT,                            0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.15 e s = INT_MAX",              TRUE,  TRUE,  SOCKET_INT_MAX,              0,          FDSET_EXCEPT,                            0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.16 e s = INVALID_SOCKET",       TRUE,  TRUE,  SOCKET_INVALID_SOCKET,       0,          FDSET_EXCEPT,                            0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.17 rw s = INT_MIN",             TRUE,  TRUE,  SOCKET_INT_MIN,              0,          FDSET_READ | FDSET_WRITE,                0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.18 rw s = -1",                  TRUE,  TRUE,  SOCKET_NEG_ONE,              0,          FDSET_READ | FDSET_WRITE,                0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.19 rw s = 0",                   TRUE,  TRUE,  SOCKET_ZERO,                 0,          FDSET_READ | FDSET_WRITE,                0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.20 rw s = INT_MAX",             TRUE,  TRUE,  SOCKET_INT_MAX,              0,          FDSET_READ | FDSET_WRITE,                0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.21 rw s = INVALID_SOCKET",      TRUE,  TRUE,  SOCKET_INVALID_SOCKET,       0,          FDSET_READ | FDSET_WRITE,                0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.22 re s = INT_MIN",             TRUE,  TRUE,  SOCKET_INT_MIN,              0,          FDSET_READ | FDSET_EXCEPT,               0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.23 re s = -1",                  TRUE,  TRUE,  SOCKET_NEG_ONE,              0,          FDSET_READ | FDSET_EXCEPT,               0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.24 re s = 0",                   TRUE,  TRUE,  SOCKET_ZERO,                 0,          FDSET_READ | FDSET_EXCEPT,               0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.25 re s = INT_MAX",             TRUE,  TRUE,  SOCKET_INT_MAX,              0,          FDSET_READ | FDSET_EXCEPT,               0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.26 re s = INVALID_SOCKET",      TRUE,  TRUE,  SOCKET_INVALID_SOCKET,       0,          FDSET_READ | FDSET_EXCEPT,               0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.27 we s = INT_MIN",             TRUE,  TRUE,  SOCKET_INT_MIN,              0,          FDSET_WRITE | FDSET_EXCEPT,              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.28 we s = -1",                  TRUE,  TRUE,  SOCKET_NEG_ONE,              0,          FDSET_WRITE | FDSET_EXCEPT,              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.29 we s = 0",                   TRUE,  TRUE,  SOCKET_ZERO,                 0,          FDSET_WRITE | FDSET_EXCEPT,              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.30 we s = INT_MAX",             TRUE,  TRUE,  SOCKET_INT_MAX,              0,          FDSET_WRITE | FDSET_EXCEPT,              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.31 we s = INVALID_SOCKET",      TRUE,  TRUE,  SOCKET_INVALID_SOCKET,       0,          FDSET_WRITE | FDSET_EXCEPT,              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.32 rwe s = INT_MIN",            TRUE,  TRUE,  SOCKET_INT_MIN,              0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.33 rwe s = -1",                 TRUE,  TRUE,  SOCKET_NEG_ONE,              0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.34 rwe s = 0",                  TRUE,  TRUE,  SOCKET_ZERO,                 0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.35 rwe s = INT_MAX",            TRUE,  TRUE,  SOCKET_INT_MAX,              0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.36 rwe s = INVALID_SOCKET",     TRUE,  TRUE,  SOCKET_INVALID_SOCKET,       0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.37 r0 none TCP",                TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              0,                                       0,                 TRUE,  0,                      0,            0,                 FALSE },
    { "26.38 rw0 none TCP",               TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                0,                                       0,                 TRUE,  0,                      0,            0,                 FALSE },
    { "26.39 re0 none TCP",               TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               0,                                       0,                 TRUE,  0,                      0,            0,                 FALSE },
    { "26.40 we0 none TCP",               TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_EXCEPT,              0,                                       0,                 TRUE,  0,                      0,            0,                 FALSE },
    { "26.41 rwe0 none TCP",              TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       0,                 TRUE,  0,                      0,            0,                 FALSE },
    { "26.42 r0 none UDP",                TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ,                              0,                                       0,                 TRUE,  0,                      0,            0,                 FALSE },
    { "26.43 rw0 none UDP",               TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ | FDSET_WRITE,                FDSET_WRITE,                             0,                 TRUE,  0,                      1,            0,                 FALSE },
    { "26.44 re0 none UDP",               TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ | FDSET_EXCEPT,               0,                                       0,                 TRUE,  0,                      0,            0,                 FALSE },
    { "26.45 we0 none UDP",               TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_WRITE | FDSET_EXCEPT,              FDSET_WRITE,                             0,                 TRUE,  0,                      1,            0,                 FALSE },
    { "26.46 rwe0 none UDP",              TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_WRITE,                             0,                 TRUE,  0,                      1,            0,                 FALSE },
    { "26.47 r0 accept pass TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              FDSET_READ,                              FUNC_ACCEPT,       TRUE,  0,                      1,            0,                 FALSE },
    { "26.48 r0 accept fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              0,                                       FUNC_ACCEPT_FAIL,  TRUE,  0,                      0,            0,                 FALSE },
    { "26.49 r5 accept pass TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              FDSET_READ,                              FUNC_ACCEPT,       TRUE,  SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.50 r5 accept fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              0,                                       FUNC_ACCEPT_FAIL,  TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.51 rNULL accept pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              FDSET_READ,                              FUNC_ACCEPT,       FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.52 rw0 accept pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                FDSET_READ,                              FUNC_ACCEPT,       TRUE,  0,                      1,            0,                 FALSE },
    { "26.53 rw0 accept fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                0,                                       FUNC_ACCEPT_FAIL,  TRUE,  0,                      0,            0,                 FALSE },
    { "26.54 rw5 accept pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                FDSET_READ,                              FUNC_ACCEPT,       TRUE,  SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.55 rw5 accept fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                0,                                       FUNC_ACCEPT_FAIL,  TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.56 rwNULL accept pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                FDSET_READ,                              FUNC_ACCEPT,       FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.57 re0 accept pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_ACCEPT,       TRUE,  0,                      1,            0,                 FALSE },
    { "26.58 re0 accept fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_ACCEPT_FAIL,  TRUE,  0,                      0,            0,                 FALSE },
    { "26.59 re5 accept pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_ACCEPT,       TRUE,  SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.60 re5 accept fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_ACCEPT_FAIL,  TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.61 reNULL accept pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_ACCEPT,       FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.62 rwe0 accept pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ,                              FUNC_ACCEPT,       TRUE,  0,                      1,            0,                 FALSE },
    { "26.63 rwe0 accept fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       FUNC_ACCEPT_FAIL,  TRUE,  0,                      0,            0,                 FALSE },
    { "26.64 rwe5 accept pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ,                              FUNC_ACCEPT,       TRUE,  SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.65 rwe5 accept fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       FUNC_ACCEPT_FAIL,  TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.66 rweNULL accept pass TCP",    TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ,                              FUNC_ACCEPT,       FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.67 r0 read pass TCP",           TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              FDSET_READ,                              FUNC_READ,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.68 r0 read fail TCP",           TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              0,                                       FUNC_READ_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.69 r5 read pass TCP",           TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              FDSET_READ,                              FUNC_READ,         TRUE,  SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.70 r5 read fail TCP",           TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              0,                                       FUNC_READ_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.71 rNULL read pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              FDSET_READ,                              FUNC_READ,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.72 rw0 read pass TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                FDSET_READ,                              FUNC_READ,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.73 rw0 read fail TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                0,                                       FUNC_READ_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.74 rw5 read pass TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                FDSET_READ,                              FUNC_READ,         TRUE,  SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.75 rw5 read fail TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                0,                                       FUNC_READ_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.76 rwNULL read pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                FDSET_READ,                              FUNC_READ,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.77 re0 read pass TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_READ,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.78 re0 read fail TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_READ_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.79 re5 read pass TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_READ,         TRUE,  SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.80 re5 read fail TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_READ_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.81 reNULL read pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_READ,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.82 rwe0 read pass TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ,                              FUNC_READ,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.83 rwe0 read fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       FUNC_READ_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.84 rwe5 read pass TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ,                              FUNC_READ,         TRUE,  SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.85 rwe5 read fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       FUNC_READ_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.86 rweNULL read pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ,                              FUNC_READ,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.87 r0 read pass UDP",           TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ,                              FDSET_READ,                              FUNC_READ,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.88 r0 read fail UDP",           TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ,                              0,                                       FUNC_READ_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.89 r5 read pass UDP",           TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ,                              FDSET_READ,                              FUNC_READ,         TRUE,  SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.90 r5 read fail UDP",           TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ,                              0,                                       FUNC_READ_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.91 rNULL read pass UDP",        TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ,                              FDSET_READ,                              FUNC_READ,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.92 re0 read pass UDP",          TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_READ,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.93 re0 read fail UDP",          TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_READ_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.94 re5 read pass UDP",          TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_READ,         TRUE,  SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.95 re5 read fail UDP",          TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_READ_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.96 reNULL read pass UDP",       TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_READ,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.97 r0 close pass TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              FDSET_READ,                              FUNC_CLOSE,        TRUE,  0,                      1,            0,                 FALSE },
    { "26.98 r0 close fail TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              0,                                       FUNC_CLOSE_FAIL,   TRUE,  0,                      0,            0,                 FALSE },
    { "26.99 r5 close pass TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              FDSET_READ,                              FUNC_CLOSE,        TRUE,  SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.100 r5 close fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              0,                                       FUNC_CLOSE_FAIL,   TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.101 rNULL close pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ,                              FDSET_READ,                              FUNC_CLOSE,        FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.102 rw0 close pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                FDSET_READ | FDSET_WRITE,                FUNC_CLOSE,        TRUE,  0,                      2,            0,                 FALSE },
    { "26.103 rw0 close fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                0,                                       FUNC_CLOSE_FAIL,   TRUE,  0,                      0,            0,                 FALSE },
    { "26.104 rw5 close pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                FDSET_READ | FDSET_WRITE,                FUNC_CLOSE,        TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.105 rw5 close fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                0,                                       FUNC_CLOSE_FAIL,   TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.106 rwNULL close pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE,                FDSET_READ | FDSET_WRITE,                FUNC_CLOSE,        FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.107 re0 close pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_CLOSE,        TRUE,  0,                      1,            0,                 FALSE },
    { "26.108 re0 close fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_CLOSE_FAIL,   TRUE,  0,                      0,            0,                 FALSE },
    { "26.109 re5 close pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_CLOSE,        TRUE,  SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.110 re5 close fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_CLOSE_FAIL,   TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.111 reNULL close pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_CLOSE,        FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.112 rwe0 close pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ | FDSET_WRITE,                FUNC_CLOSE,        TRUE,  0,                      2,            0,                 FALSE },
    { "26.113 rwe0 close fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       FUNC_CLOSE_FAIL,   TRUE,  0,                      0,            0,                 FALSE },
    { "26.114 rwe5 close pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ | FDSET_WRITE,                FUNC_CLOSE,        TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.115 rwe5 close fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       FUNC_CLOSE_FAIL,   TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.116 rweNULL close pass TCP",    TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ | FDSET_WRITE,                FUNC_CLOSE,        FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.117 wNULL conn pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE,                             FDSET_WRITE,                             FUNC_CONNECT,      FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.118 wrNULL conn pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_READ,                FDSET_WRITE,                             FUNC_CONNECT,      FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.119 weNULL conn pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_EXCEPT,              FDSET_WRITE,                             FUNC_CONNECT,      FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.120 wreNULL conn pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, FDSET_WRITE,                             FUNC_CONNECT,      FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.121 w0 send pass TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE,                             FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.122 w0 send fail TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE,                             0,                                       FUNC_SEND_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.123 w5 send fail TCP",          TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE,                             0,                                       FUNC_SEND_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.124 wNULL send pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE,                             FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.125 wr0 send pass TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_READ,                FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.126 wr0 send fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_READ,                0,                                       FUNC_SEND_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.127 wr5 send fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_READ,                0,                                       FUNC_SEND_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.128 wrNULL send pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_READ,                FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.129 we0 send pass TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_EXCEPT,              FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.130 we0 send fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_EXCEPT,              0,                                       FUNC_SEND_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.131 we5 send fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_EXCEPT,              0,                                       FUNC_SEND_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.132 weNULL send pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_EXCEPT,              FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.133 wre0 send pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.134 wre0 send fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, 0,                                       FUNC_SEND_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.135 wre5 send fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, 0,                                       FUNC_SEND_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.136 wreNULL send pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.137 w0 send pass UDP",          TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_WRITE,                             FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.138 wNULL send pass UDP",       TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_WRITE,                             FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.139 wr0 send pass UDP",         TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_WRITE | FDSET_READ,                FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.140 wrNULL send pass UDP",      TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_WRITE | FDSET_READ,                FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.141 we0 send pass UDP",         TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_WRITE | FDSET_EXCEPT,              FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.142 weNULL send pass UDP",      TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_WRITE | FDSET_EXCEPT,              FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.143 wre0 send pass UDP",        TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      1,            0,                 FALSE },
    { "26.144 wreNULL send pass UDP",     TRUE,  TRUE,  SOCKET_UDP,                  0,          FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.145 eNULL conn fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_EXCEPT,                            FDSET_EXCEPT,                            FUNC_CONNECT_FAIL, FALSE, SLEEP_MEAN_TIME * 1000, 1,            0,                 FALSE },
    { "26.146 erNULL conn fail TCP",      TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_EXCEPT | FDSET_READ,               FDSET_EXCEPT | FDSET_READ,               FUNC_CONNECT_FAIL, FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.147 ewNULL conn fail TCP",      TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_EXCEPT | FDSET_WRITE,              FDSET_EXCEPT | FDSET_WRITE,              FUNC_CONNECT_FAIL, FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.148 erwNULL conn fail TCP",     TRUE,  TRUE,  SOCKET_TCP,                  0,          FDSET_EXCEPT | FDSET_READ | FDSET_WRITE, FDSET_EXCEPT | FDSET_READ | FDSET_WRITE, FUNC_CONNECT_FAIL, FALSE, SLEEP_MEAN_TIME * 1000, 3,            0,                 FALSE },
    { "26.149 2r0 accept pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              FDSET_READ,                              FUNC_ACCEPT,       TRUE,  0,                      2,            0,                 FALSE },
    { "26.150 2r0 accept fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              0,                                       FUNC_ACCEPT_FAIL,  TRUE,  0,                      0,            0,                 FALSE },
    { "26.151 2r5 accept pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              FDSET_READ,                              FUNC_ACCEPT,       TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.152 2r5 accept fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              0,                                       FUNC_ACCEPT_FAIL,  TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.153 2rNULL accept pass TCP",    TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              FDSET_READ,                              FUNC_ACCEPT,       FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.154 2rw0 accept pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                FDSET_READ,                              FUNC_ACCEPT,       TRUE,  0,                      2,            0,                 FALSE },
    { "26.155 2rw0 accept fail TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                0,                                       FUNC_ACCEPT_FAIL,  TRUE,  0,                      0,            0,                 FALSE },
    { "26.156 2rw5 accept pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                FDSET_READ,                              FUNC_ACCEPT,       TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.157 2rw5 accept fail TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                0,                                       FUNC_ACCEPT_FAIL,  TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.158 2rwNULL accept pass TCP",   TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                FDSET_READ,                              FUNC_ACCEPT,       FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.159 2re0 accept pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_ACCEPT,       TRUE,  0,                      2,            0,                 FALSE },
    { "26.160 2re0 accept fail TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_ACCEPT_FAIL,  TRUE,  0,                      0,            0,                 FALSE },
    { "26.161 2re5 accept pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_ACCEPT,       TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.162 2re5 accept fail TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_ACCEPT_FAIL,  TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.163 2reNULL accept pass TCP",   TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_ACCEPT,       FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.164 2rwe0 accept pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ,                              FUNC_ACCEPT,       TRUE,  0,                      2,            0,                 FALSE },
    { "26.165 2rwe0 accept fail TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       FUNC_ACCEPT_FAIL,  TRUE,  0,                      0,            0,                 FALSE },
    { "26.166 2rwe5 accept pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ,                              FUNC_ACCEPT,       TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.167 2rwe5 accept fail TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       FUNC_ACCEPT_FAIL,  TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.168 2rweNULL accept pass TCP",  TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ,                              FUNC_ACCEPT,       FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.169 2r0 read pass TCP",         TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              FDSET_READ,                              FUNC_READ,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.170 2r0 read fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              0,                                       FUNC_READ_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.171 2r5 read pass TCP",         TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              FDSET_READ,                              FUNC_READ,         TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.172 2r5 read fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              0,                                       FUNC_READ_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.173 2rNULL read pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              FDSET_READ,                              FUNC_READ,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.174 2rw0 read pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                FDSET_READ,                              FUNC_READ,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.175 2rw0 read fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                0,                                       FUNC_READ_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.176 2rw5 read pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                FDSET_READ,                              FUNC_READ,         TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.177 2rw5 read fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                0,                                       FUNC_READ_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.178 2rwNULL read pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                FDSET_READ,                              FUNC_READ,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.179 2re0 read pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_READ,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.180 2re0 read fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_READ_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.181 2re5 read pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_READ,         TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.182 2re5 read fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_READ_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.183 2reNULL read pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_READ,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.184 2rwe0 read pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ,                              FUNC_READ,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.185 2rwe0 read fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       FUNC_READ_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.186 2rwe5 read pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ,                              FUNC_READ,         TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.187 2rwe5 read fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       FUNC_READ_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.188 2rweNULL read pass TCP",    TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ,                              FUNC_READ,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.189 2r0 read pass UDP",         TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_READ,                              FDSET_READ,                              FUNC_READ,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.190 2r0 read fail UDP",         TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_READ,                              0,                                       FUNC_READ_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.191 2r5 read pass UDP",         TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_READ,                              FDSET_READ,                              FUNC_READ,         TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.192 2r5 read fail UDP",         TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_READ,                              0,                                       FUNC_READ_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.193 2rNULL read pass UDP",      TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_READ,                              FDSET_READ,                              FUNC_READ,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.194 2re0 read pass UDP",        TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_READ,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.195 2re0 read fail UDP",        TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_READ_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.196 2re5 read pass UDP",        TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_READ,         TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.197 2re5 read fail UDP",        TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_READ_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.198 2reNULL read pass UDP",     TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_READ,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.199 2r0 close pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              FDSET_READ,                              FUNC_CLOSE,        TRUE,  0,                      2,            0,                 FALSE },
    { "26.200 2r0 close fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              0,                                       FUNC_CLOSE_FAIL,   TRUE,  0,                      0,            0,                 FALSE },
    { "26.201 2r5 close pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              FDSET_READ,                              FUNC_CLOSE,        TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.202 2r5 close fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              0,                                       FUNC_CLOSE_FAIL,   TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.203 2rNULL close pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ,                              FDSET_READ,                              FUNC_CLOSE,        FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.204 2rw0 close pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                FDSET_READ | FDSET_WRITE,                FUNC_CLOSE,        TRUE,  0,                      4,            0,                 FALSE },
    { "26.205 2rw0 close fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                0,                                       FUNC_CLOSE_FAIL,   TRUE,  0,                      0,            0,                 FALSE },
    { "26.206 2rw5 close pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                FDSET_READ | FDSET_WRITE,                FUNC_CLOSE,        TRUE,  SLEEP_MEAN_TIME * 1000, 4,            0,                 FALSE },
    { "26.207 2rw5 close fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                0,                                       FUNC_CLOSE_FAIL,   TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.208 2rwNULL close pass TCP",    TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE,                FDSET_READ | FDSET_WRITE,                FUNC_CLOSE,        FALSE, SLEEP_MEAN_TIME * 1000, 4,            0,                 FALSE },
    { "26.209 2re0 close pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_CLOSE,        TRUE,  0,                      2,            0,                 FALSE },
    { "26.210 2re0 close fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_CLOSE_FAIL,   TRUE,  0,                      0,            0,                 FALSE },
    { "26.211 2re5 close pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_CLOSE,        TRUE,  SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.212 2re5 close fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               0,                                       FUNC_CLOSE_FAIL,   TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.213 2reNULL close pass TCP",    TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_EXCEPT,               FDSET_READ,                              FUNC_CLOSE,        FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.214 2rwe0 close pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ | FDSET_WRITE,                FUNC_CLOSE,        TRUE,  0,                      4,            0,                 FALSE },
    { "26.215 2rwe0 close fail TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       FUNC_CLOSE_FAIL,   TRUE,  0,                      0,            0,                 FALSE },
    { "26.216 2rwe5 close pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ | FDSET_WRITE,                FUNC_CLOSE,        TRUE,  SLEEP_MEAN_TIME * 1000, 4,            0,                 FALSE },
    { "26.217 2rwe5 close fail TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       FUNC_CLOSE_FAIL,   TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.218 2rweNULL close pass TCP",   TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, FDSET_READ | FDSET_WRITE,                FUNC_CLOSE,        FALSE, SLEEP_MEAN_TIME * 1000, 4,            0,                 FALSE },
    { "26.219 2wNULL conn pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE,                             FDSET_WRITE,                             FUNC_CONNECT,      FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.220 2wrNULL conn pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_READ,                FDSET_WRITE,                             FUNC_CONNECT,      FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.221 2weNULL conn pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_EXCEPT,              FDSET_WRITE,                             FUNC_CONNECT,      FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.222 2wreNULL conn pass TCP",    TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, FDSET_WRITE,                             FUNC_CONNECT,      FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.223 2w0 send pass TCP",         TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE,                             FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.224 2w0 send fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE,                             0,                                       FUNC_SEND_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.225 2w5 send fail TCP",         TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE,                             0,                                       FUNC_SEND_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.226 2wNULL send pass TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE,                             FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.227 2wr0 send pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_READ,                FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.228 2wr0 send fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_READ,                0,                                       FUNC_SEND_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.229 2wr5 send fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_READ,                0,                                       FUNC_SEND_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.230 2wrNULL send pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_READ,                FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.231 2we0 send pass TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_EXCEPT,              FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.232 2we0 send fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_EXCEPT,              0,                                       FUNC_SEND_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.233 2we5 send fail TCP",        TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_EXCEPT,              0,                                       FUNC_SEND_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.234 2weNULL send pass TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_EXCEPT,              FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.235 2wre0 send pass TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.236 2wre0 send fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, 0,                                       FUNC_SEND_FAIL,    TRUE,  0,                      0,            0,                 FALSE },
    { "26.237 2wre5 send fail TCP",       TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, 0,                                       FUNC_SEND_FAIL,    TRUE,  SLEEP_MEAN_TIME * 1000, 0,            0,                 FALSE },
    { "26.238 2wreNULL send pass TCP",    TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.239 2w0 send pass UDP",         TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_WRITE,                             FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.240 2wNULL send pass UDP",      TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_WRITE,                             FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.241 2wr0 send pass UDP",        TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_WRITE | FDSET_READ,                FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.242 2wrNULL send pass UDP",     TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_WRITE | FDSET_READ,                FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.243 2we0 send pass UDP",        TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_WRITE | FDSET_EXCEPT,              FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.244 2weNULL send pass UDP",     TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_WRITE | FDSET_EXCEPT,              FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.245 2wre0 send pass UDP",       TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, FDSET_WRITE,                             FUNC_SEND,         TRUE,  0,                      2,            0,                 FALSE },
    { "26.246 2wreNULL send pass UDP",    TRUE,  TRUE,  SOCKET_UDP,                  SOCKET_UDP, FDSET_WRITE | FDSET_READ | FDSET_EXCEPT, FDSET_WRITE,                             FUNC_SEND,         FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.247 2eNULL conn fail TCP",      TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_EXCEPT,                            FDSET_EXCEPT,                            FUNC_CONNECT_FAIL, FALSE, SLEEP_MEAN_TIME * 1000, 2,            0,                 FALSE },
    { "26.248 2erNULL conn fail TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_EXCEPT | FDSET_READ,               FDSET_EXCEPT | FDSET_READ,               FUNC_CONNECT_FAIL, FALSE, SLEEP_MEAN_TIME * 1000, 4,            0,                 FALSE },
    { "26.249 2ewNULL conn fail TCP",     TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_EXCEPT | FDSET_WRITE,              FDSET_EXCEPT | FDSET_WRITE,              FUNC_CONNECT_FAIL, FALSE, SLEEP_MEAN_TIME * 1000, 4,            0,                 FALSE },
    { "26.250 2erwNULL conn fail TCP",    TRUE,  TRUE,  SOCKET_TCP,                  SOCKET_TCP, FDSET_EXCEPT | FDSET_READ | FDSET_WRITE, FDSET_EXCEPT | FDSET_READ | FDSET_WRITE, FUNC_CONNECT_FAIL, FALSE, SLEEP_MEAN_TIME * 1000, 6,            0,                 FALSE },
    { "26.251 r Empty",                   TRUE,  TRUE,  0,                           0,          FDSET_READ,                              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "26.252 w Empty",                   TRUE,  TRUE,  0,                           0,          FDSET_WRITE,                             0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "26.253 e Empty",                   TRUE,  TRUE,  0,                           0,          FDSET_EXCEPT,                            0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "26.254 rw Empty",                  TRUE,  TRUE,  0,                           0,          FDSET_READ | FDSET_WRITE,                0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "26.255 re Empty",                  TRUE,  TRUE,  0,                           0,          FDSET_READ | FDSET_EXCEPT,               0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "26.256 we Empty",                  TRUE,  TRUE,  0,                           0,          FDSET_WRITE | FDSET_EXCEPT,              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "26.257 rwe Empty",                 TRUE,  TRUE,  0,                           0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "26.258 NULL sets",                 TRUE,  TRUE,  0,                           0,          0,                                       0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "26.259 r Closed Socket TCP",       TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED,  0,          FDSET_READ,                              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.260 r Closed Socket UDP",       TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED,  0,          FDSET_READ,                              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.261 w Closed Socket TCP",       TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED,  0,          FDSET_WRITE,                             0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.262 w Closed Socket UDP",       TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED,  0,          FDSET_WRITE,                             0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.263 e Closed Socket TCP",       TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED,  0,          FDSET_EXCEPT,                            0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.264 e Closed Socket UDP",       TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED,  0,          FDSET_EXCEPT,                            0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.265 rw Closed Socket TCP",      TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED,  0,          FDSET_READ | FDSET_WRITE,                0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.266 rw Closed Socket UDP",      TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED,  0,          FDSET_READ | FDSET_WRITE,                0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.367 re Closed Socket TCP",      TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED,  0,          FDSET_READ | FDSET_EXCEPT,               0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.268 re Closed Socket UDP",      TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED,  0,          FDSET_READ | FDSET_EXCEPT,               0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.269 we Closed Socket TCP",      TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED,  0,          FDSET_WRITE | FDSET_EXCEPT,              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.270 we Closed Socket UDP",      TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED,  0,          FDSET_WRITE | FDSET_EXCEPT,              0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.271 rwe Closed Socket TCP",     TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED,  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.272 rwe Closed Socket UDP",     TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED,  0,          FDSET_READ | FDSET_WRITE | FDSET_EXCEPT, 0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "26.273 Not Initialized",           FALSE, FALSE, SOCKET_INVALID_SOCKET,       0,          0,                                       0,                                       0,                 FALSE, 0,                      SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define selectTableCount (sizeof(selectTable) / sizeof(SELECT_TABLE))

NETSYNC_TYPE_THREAD  selectTestSessionNt =
{
    1,
    selectTableCount * 2,
    L"xnetapi_nt.dll",
    "selectTestServer"
};

NETSYNC_TYPE_THREAD  selectTestSessionXbox =
{
    1,
    selectTableCount * 2,
    L"xnetapi_xbox.dll",
    "selectTestServer"
};



VOID
selectTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests select - Client side

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
    // selectRequest is the request sent to the server
    SELECT_REQUEST         selectRequest;
    
    // sSocket1 is the first socket descriptor
    SOCKET                 sSocket1;
    // sSocket2 is the second socket descriptor
    SOCKET                 sSocket2;
    // nsSocket1 is the first accepted socket descriptor
    SOCKET                 nsSocket1;
    // nsSocket2 is the second accepted socket descriptor
    SOCKET                 nsSocket2;

    // dwBufferSize is the send/receive buffer size
    DWORD                  dwBufferSize = 1;
    // bNagle indicates if Nagle is enabled
    BOOL                   bNagle = FALSE;
    // Nonblock sets the socket to blocking or non-blocking mode
    u_long                 Nonblock;
    // bNonblocking indicates if the socket is in non-blocking mode
    BOOL                   bNonblocking = FALSE;

    // localname1 is the first local address
    SOCKADDR_IN            localname1;
    // localname2 is the second local address
    SOCKADDR_IN            localname2;
    // remotename1 is the first remote address
    SOCKADDR_IN            remotename1;
    // remotename2 is the second remote address
    SOCKADDR_IN            remotename2;

    // readfds is the set of sockets to check for a read condition
    fd_set                 readfds;
    // writefds is the set of sockets to check for a write condition
    fd_set                 writefds;
    // exceptfds is the set of sockets to check for an except condition
    fd_set                 exceptfds;
    // timeout is the timeout for select
    timeval                fdstimeout = { 1, 0 };

    // SendBuffer10 is the send buffer
    char                   SendBuffer10[BUFFER_10_LEN + 1];
    // SendBufferLarge is the large send buffer
    char                   SendBufferLarge[BUFFER_LARGE_LEN + 1];
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
    sprintf(szFunctionName, "select v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_select+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_select-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = selectTestSessionXbox;
    }
    else {
        NetsyncTypeSession = selectTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < selectTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, selectTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, selectTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != selectTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, selectTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == selectTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != selectTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == selectTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = selectTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == selectTable[dwTableIndex].bNetsyncConnected)) {
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

        // Create the sockets
        sSocket1 = INVALID_SOCKET;
        sSocket2 = INVALID_SOCKET;
        nsSocket1 = INVALID_SOCKET;
        nsSocket2 = INVALID_SOCKET;
        if (SOCKET_INT_MIN == selectTable[dwTableIndex].dwSocket1) {
            sSocket1 = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == selectTable[dwTableIndex].dwSocket1) {
            sSocket1 = -1;
        }
        else if (SOCKET_ZERO == selectTable[dwTableIndex].dwSocket1) {
            sSocket1 = 0;
        }
        else if (SOCKET_INT_MAX == selectTable[dwTableIndex].dwSocket1) {
            sSocket1 = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == selectTable[dwTableIndex].dwSocket1) {
            sSocket1 = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & selectTable[dwTableIndex].dwSocket1)) {
            sSocket1 = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & selectTable[dwTableIndex].dwSocket1)) {
            sSocket1 = socket(AF_INET, SOCK_DGRAM, 0);
        }

        sSocket2 = INVALID_SOCKET;
        if (SOCKET_TCP == selectTable[dwTableIndex].dwSocket2) {
            sSocket2 = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (SOCKET_UDP == selectTable[dwTableIndex].dwSocket2) {
            sSocket2 = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & selectTable[dwTableIndex].dwSocket1)) {
            setsockopt(sSocket1, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
            if (INVALID_SOCKET != sSocket1) {
                setsockopt(sSocket1, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
            }
        }

        if ((0 != (SOCKET_TCP & selectTable[dwTableIndex].dwSocket1)) || (0 != (SOCKET_UDP & selectTable[dwTableIndex].dwSocket1))) {
            // Set the buffer size
            dwBufferSize = 1;
            setsockopt(sSocket1, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

            dwBufferSize = 1;
            setsockopt(sSocket1, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

            if (INVALID_SOCKET != sSocket2) {
                dwBufferSize = 1;
                setsockopt(sSocket2, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

                dwBufferSize = 1;
                setsockopt(sSocket2, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
            }

            // Set the socket to non-blocking mode
            Nonblock = 1;
            ioctlsocket(sSocket1, FIONBIO, &Nonblock);
            if (INVALID_SOCKET != sSocket2) {
                ioctlsocket(sSocket2, FIONBIO, &Nonblock);
            }

            // Bind the socket
            ZeroMemory(&localname1, sizeof(localname1));
            localname1.sin_family = AF_INET;
            localname1.sin_port = htons(CurrentPort);
            bind(sSocket1, (SOCKADDR *) &localname1, sizeof(localname1));

            if (INVALID_SOCKET != sSocket2) {
                ZeroMemory(&localname2, sizeof(localname2));
                localname2.sin_family = AF_INET;
                localname2.sin_port = htons(CurrentPort + 1);
                bind(sSocket2, (SOCKADDR *) &localname2, sizeof(localname2));
            }
        }

        // Listen
        if ((FUNC_ACCEPT == selectTable[dwTableIndex].dwFunction) || (FUNC_ACCEPT_FAIL == selectTable[dwTableIndex].dwFunction)) {
            listen(sSocket1, SOMAXCONN);

            if (INVALID_SOCKET != sSocket2) {
                listen(sSocket2, SOMAXCONN);
            }
        }

        // Setup the fdset structures
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);

        if (0 != selectTable[dwTableIndex].dwSocket1) {
            if (0 != (FDSET_READ & selectTable[dwTableIndex].dwAddSets)) {
                FD_SET(sSocket1, &readfds);

                if (INVALID_SOCKET != sSocket2) {
                    FD_SET(sSocket2, &readfds);
                }
            }

            if (0 != (FDSET_WRITE & selectTable[dwTableIndex].dwAddSets)) {
                FD_SET(sSocket1, &writefds);

                if (INVALID_SOCKET != sSocket2) {
                    FD_SET(sSocket2, &writefds);
                }
            }

            if (0 != (FDSET_EXCEPT & selectTable[dwTableIndex].dwAddSets)) {
                FD_SET(sSocket1, &exceptfds);

                if (INVALID_SOCKET != sSocket2) {
                    FD_SET(sSocket2, &exceptfds);
                }
            }
        }

        // Setup the timeout
        fdstimeout.tv_sec = 0;
        fdstimeout.tv_usec = selectTable[dwTableIndex].tv_usec;

        if (0 != selectTable[dwTableIndex].dwFunction) {
            // Initialize the select request
            selectRequest.dwMessageId = SELECT_REQUEST_MSG;
            if (0 != (SOCKET_TCP & selectTable[dwTableIndex].dwSocket1)) {
                selectRequest.nSocketType = SOCK_STREAM;
            }
            else {
                selectRequest.nSocketType = SOCK_DGRAM;
            }
            if (INVALID_SOCKET != sSocket2) {
                selectRequest.nSockets = 2;
            }
            else {
                selectRequest.nSockets = 1;
            }
            selectRequest.Port = CurrentPort;

            selectRequest.bServerAccept = FALSE;
            selectRequest.bServerConnect = FALSE;
            selectRequest.bServerClose = FALSE;
            selectRequest.bServerSend = FALSE;
            if (FUNC_ACCEPT == selectTable[dwTableIndex].dwFunction) {
                selectRequest.bServerConnect = TRUE;
            }
            else if (FUNC_READ == selectTable[dwTableIndex].dwFunction) {
                selectRequest.bServerAccept = TRUE;
                selectRequest.bServerSend = TRUE;
            }
            else if (FUNC_READ_FAIL == selectTable[dwTableIndex].dwFunction) {
                selectRequest.bServerAccept = TRUE;
            }
            else if (FUNC_CLOSE == selectTable[dwTableIndex].dwFunction) {
                selectRequest.bServerAccept = TRUE;
                selectRequest.bServerClose = TRUE;
            }
            else if (FUNC_CLOSE_FAIL == selectTable[dwTableIndex].dwFunction) {
                selectRequest.bServerAccept = TRUE;
            }
            else if (FUNC_CONNECT == selectTable[dwTableIndex].dwFunction) {
                selectRequest.bServerAccept = TRUE;
            }
            else if (FUNC_SEND == selectTable[dwTableIndex].dwFunction) {
                selectRequest.bServerAccept = TRUE;
            }
            else if (FUNC_SEND_FAIL == selectTable[dwTableIndex].dwFunction) {
                selectRequest.bServerAccept = TRUE;
            }

            selectRequest.dwWaitTime = selectTable[dwTableIndex].tv_usec;

            // Send the select request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(selectRequest), (char *) &selectRequest);

            // Wait for the select complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);
        }

        // Connect
        if ((0 != selectTable[dwTableIndex].dwFunction) && (FUNC_CONNECT != selectTable[dwTableIndex].dwFunction) && (FUNC_CONNECT_FAIL != selectTable[dwTableIndex].dwFunction) && (FUNC_ACCEPT != selectTable[dwTableIndex].dwFunction) && (FUNC_ACCEPT_FAIL != selectTable[dwTableIndex].dwFunction)) {
            ZeroMemory(&remotename1, sizeof(remotename1));
            remotename1.sin_family = AF_INET;
            remotename1.sin_addr.s_addr = NetsyncInAddr;
            remotename1.sin_port = htons(CurrentPort);

            connect(sSocket1, (SOCKADDR *) &remotename1, sizeof(remotename1));

            if (INVALID_SOCKET != sSocket2) {
                ZeroMemory(&remotename2, sizeof(remotename2));
                remotename2.sin_family = AF_INET;
                remotename2.sin_addr.s_addr = NetsyncInAddr;
                remotename2.sin_port = htons(CurrentPort + 1);

                connect(sSocket2, (SOCKADDR *) &remotename2, sizeof(remotename2));
            }
        }

        // Fill the queue, if necessary
        if ((0 != (SOCKET_TCP & selectTable[dwTableIndex].dwSocket1)) && (FUNC_CONNECT != selectTable[dwTableIndex].dwFunction) && (FUNC_CONNECT_FAIL != selectTable[dwTableIndex].dwFunction) && (FUNC_ACCEPT != selectTable[dwTableIndex].dwFunction) && (FUNC_ACCEPT_FAIL != selectTable[dwTableIndex].dwFunction) && (FUNC_SEND != selectTable[dwTableIndex].dwFunction) && (0 != (FDSET_WRITE & selectTable[dwTableIndex].dwAddSets))) {
            ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
            do {
                Sleep(SLEEP_ZERO_TIME);
            } while (SOCKET_ERROR != send(sSocket1, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0));

            if (INVALID_SOCKET != sSocket2) {
                ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
                do {
                    Sleep(SLEEP_ZERO_TIME);
                } while (SOCKET_ERROR != send(sSocket2, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0));
            }
        }

        if (0 != selectTable[dwTableIndex].dwFunction) {
            // Send the select request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(selectRequest), (char *) &selectRequest);

            // Wait for the select complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Connect
        if ((FUNC_CONNECT == selectTable[dwTableIndex].dwFunction) || (FUNC_CONNECT_FAIL == selectTable[dwTableIndex].dwFunction)) {
            ZeroMemory(&remotename1, sizeof(remotename1));
            remotename1.sin_family = AF_INET;
            remotename1.sin_addr.s_addr = NetsyncInAddr;
            remotename1.sin_port = htons(CurrentPort);

            connect(sSocket1, (SOCKADDR *) &remotename1, sizeof(remotename1));

            if (INVALID_SOCKET != sSocket2) {
                ZeroMemory(&remotename2, sizeof(remotename2));
                remotename2.sin_family = AF_INET;
                remotename2.sin_addr.s_addr = NetsyncInAddr;
                remotename2.sin_port = htons(CurrentPort + 1);

                connect(sSocket2, (SOCKADDR *) &remotename2, sizeof(remotename2));
            }
        }

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & selectTable[dwTableIndex].dwSocket1)) {
            closesocket(sSocket1);
        }

        // Get the current tick count
        dwFirstTime = GetTickCount();

        __try {
            // Call select
            iReturnCode = select(0, (0 != (FDSET_READ & selectTable[dwTableIndex].dwAddSets)) ? &readfds : NULL, (0 != (FDSET_WRITE & selectTable[dwTableIndex].dwAddSets)) ? &writefds: NULL, (0 != (FDSET_EXCEPT & selectTable[dwTableIndex].dwAddSets)) ? &exceptfds: NULL, (TRUE == selectTable[dwTableIndex].bTimeout) ? &fdstimeout : NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == selectTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "select RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "select caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            // Get the current tick count
            dwSecondTime = GetTickCount();

            if (TRUE == selectTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "select did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == selectTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != selectTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "select iLastError - EXPECTED: %u; RECEIVED: %u", selectTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "select iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "select returned SOCKET_ERROR - ec = %u", WSAGetLastError());
            }
            else if (SOCKET_ERROR == selectTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "select returned non-SOCKET_ERROR");
            }
            else {
                // Check elapsed time
                if (FUNC_SEND == selectTable[dwTableIndex].dwFunction) {
                    if ((dwSecondTime - dwFirstTime) > SLEEP_ZERO_TIME) {
                        xLog(hLog, XLL_FAIL, "select nTime - EXPECTED: %d; RECEIVED: %d", 0, dwSecondTime - dwFirstTime);
                        bTestPassed = FALSE;
                    }
                }
                else if (FALSE == selectTable[dwTableIndex].bTimeout) {
                    if ((FUNC_CONNECT != selectTable[dwTableIndex].dwFunction) && (FUNC_CONNECT_FAIL != selectTable[dwTableIndex].dwFunction)) {
                        if (((dwSecondTime - dwFirstTime) < (SLEEP_LOW_TIME + SLEEP_MEAN_TIME)) || ((dwSecondTime - dwFirstTime) > (SLEEP_HIGH_TIME + SLEEP_MEAN_TIME))) {
                            xLog(hLog, XLL_FAIL, "select nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME, dwSecondTime - dwFirstTime);
                            bTestPassed = FALSE;
                        }
                    }
                }
                else {
                    if (0 == selectTable[dwTableIndex].tv_usec) {
                        if ((dwSecondTime - dwFirstTime) > SLEEP_ZERO_TIME) {
                            xLog(hLog, XLL_FAIL, "select nTime - EXPECTED: %d; RECEIVED: %d", 0, dwSecondTime - dwFirstTime);
                            bTestPassed = FALSE;
                        }
                    }
                    else if ((FUNC_CONNECT != selectTable[dwTableIndex].dwFunction) && (FUNC_CONNECT_FAIL != selectTable[dwTableIndex].dwFunction)) {
                        if (((dwSecondTime - dwFirstTime) < SLEEP_LOW_TIME) || ((dwSecondTime - dwFirstTime) > SLEEP_HIGH_TIME)) {
                            xLog(hLog, XLL_FAIL, "select nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME, dwSecondTime - dwFirstTime);
                            bTestPassed = FALSE;
                        }
                    }
                }

                if ((0 != selectTable[dwTableIndex].dwCheckSets) && (0 != selectTable[dwTableIndex].dwFunction) && (FUNC_SEND != selectTable[dwTableIndex].dwFunction)) {
                    if (TRUE == selectTable[dwTableIndex].bTimeout) {
                        // Check fd sets
                        if (0 != FD_ISSET(sSocket1, &readfds)) {
                            xLog(hLog, XLL_FAIL, "select0: sSocket1 in readfds");
                            bTestPassed = FALSE;
                        }

                        if (0 != FD_ISSET(sSocket1, &writefds)) {
                            xLog(hLog, XLL_FAIL, "select0: sSocket1 in writefds");
                            bTestPassed = FALSE;
                        }

                        if (0 != FD_ISSET(sSocket1, &exceptfds)) {
                            xLog(hLog, XLL_FAIL, "select0: sSocket1 in exceptfds");
                            bTestPassed = FALSE;
                        }

                        if (0 != FD_ISSET(sSocket2, &readfds)) {
                            xLog(hLog, XLL_FAIL, "select0: sSocket2 in readfds");
                            bTestPassed = FALSE;
                        }

                        if (0 != FD_ISSET(sSocket2, &writefds)) {
                            xLog(hLog, XLL_FAIL, "select0: sSocket2 in writefds");
                            bTestPassed = FALSE;
                        }

                        if (0 != FD_ISSET(sSocket2, &exceptfds)) {
                            xLog(hLog, XLL_FAIL, "select0: sSocket2 in exceptfds");
                            bTestPassed = FALSE;
                        }
                    }

                    do {
                        // Setup the fdset structures
                        FD_ZERO(&readfds);
                        FD_ZERO(&writefds);
                        FD_ZERO(&exceptfds);

                        if (0 != selectTable[dwTableIndex].dwSocket1) {
                            if (0 != (FDSET_READ & selectTable[dwTableIndex].dwAddSets)) {
                                FD_SET(sSocket1, &readfds);

                                if (INVALID_SOCKET != sSocket2) {
                                    FD_SET(sSocket2, &readfds);
                                }
                            }

                            if (0 != (FDSET_WRITE & selectTable[dwTableIndex].dwAddSets)) {
                                FD_SET(sSocket1, &writefds);

                                if (INVALID_SOCKET != sSocket2) {
                                    FD_SET(sSocket2, &writefds);
                                }
                            }

                            if (0 != (FDSET_EXCEPT & selectTable[dwTableIndex].dwAddSets)) {
                                FD_SET(sSocket1, &exceptfds);

                                if (INVALID_SOCKET != sSocket2) {
                                    FD_SET(sSocket2, &exceptfds);
                                }
                            }
                        }

                        if (0 == selectTable[dwTableIndex].tv_usec) {
                            Sleep(SLEEP_ZERO_TIME);
                        }

                        iReturnCode = select(0, (0 != (FDSET_READ & selectTable[dwTableIndex].dwAddSets)) ? &readfds : NULL, (0 != (FDSET_WRITE & selectTable[dwTableIndex].dwAddSets)) ? &writefds: NULL, (0 != (FDSET_EXCEPT & selectTable[dwTableIndex].dwAddSets)) ? &exceptfds: NULL, (TRUE == selectTable[dwTableIndex].bTimeout) ? &fdstimeout : NULL);
                    } while (selectTable[dwTableIndex].iReturnCode > iReturnCode);
                }

                // Check return code
                if (selectTable[dwTableIndex].iReturnCode != iReturnCode) {
                    xLog(hLog, XLL_FAIL, "select1 iReturnCode - EXPECTED: %d; RECEIVED: %d", selectTable[dwTableIndex].iReturnCode, iReturnCode);
                    bTestPassed = FALSE;
                }

                // Check fd sets
                if (0 != (FDSET_READ & selectTable[dwTableIndex].dwCheckSets)) {
                    if (0 == FD_ISSET(sSocket1, &readfds)) {
                        xLog(hLog, XLL_FAIL, "select1: sSocket1 not in readfds");
                        bTestPassed = FALSE;
                    }
                }
                else {
                    if (0 != FD_ISSET(sSocket1, &readfds)) {
                        xLog(hLog, XLL_FAIL, "select1: sSocket1 in readfds");
                        bTestPassed = FALSE;
                    }
                }

                if (0 != (FDSET_WRITE & selectTable[dwTableIndex].dwCheckSets)) {
                    if (0 == FD_ISSET(sSocket1, &writefds)) {
                        xLog(hLog, XLL_FAIL, "select1: sSocket1 not in writefds");
                        bTestPassed = FALSE;
                    }
                }
                else {
                    if (0 != FD_ISSET(sSocket1, &writefds)) {
                        xLog(hLog, XLL_FAIL, "select1: sSocket1 in writefds");
                        bTestPassed = FALSE;
                    }
                }

                if (0 != (FDSET_EXCEPT & selectTable[dwTableIndex].dwCheckSets)) {
                    if (0 == FD_ISSET(sSocket1, &exceptfds)) {
                        xLog(hLog, XLL_FAIL, "select1: sSocket1 not in exceptfds");
                        bTestPassed = FALSE;
                    }
                }
                else {
                    if (0 != FD_ISSET(sSocket1, &exceptfds)) {
                        xLog(hLog, XLL_FAIL, "select1: sSocket1 in exceptfds");
                        bTestPassed = FALSE;
                    }
                }

                if (INVALID_SOCKET != sSocket2) {
                    // Check fd sets
                    if (0 != (FDSET_READ & selectTable[dwTableIndex].dwCheckSets)) {
                        if (0 == FD_ISSET(sSocket2, &readfds)) {
                            xLog(hLog, XLL_FAIL, "select1: sSocket2 not in readfds");
                            bTestPassed = FALSE;
                        }
                    }
                    else {
                        if (0 != FD_ISSET(sSocket2, &readfds)) {
                            xLog(hLog, XLL_FAIL, "select1: sSocket2 in readfds");
                            bTestPassed = FALSE;
                        }
                    }

                    if (0 != (FDSET_WRITE & selectTable[dwTableIndex].dwCheckSets)) {
                        if (0 == FD_ISSET(sSocket2, &writefds)) {
                            xLog(hLog, XLL_FAIL, "select1: sSocket2 not in writefds");
                            bTestPassed = FALSE;
                        }
                    }
                    else {
                        if (0 != FD_ISSET(sSocket2, &writefds)) {
                            xLog(hLog, XLL_FAIL, "select1: sSocket2 in writefds");
                            bTestPassed = FALSE;
                        }
                    }

                    if (0 != (FDSET_EXCEPT & selectTable[dwTableIndex].dwCheckSets)) {
                        if (0 == FD_ISSET(sSocket2, &exceptfds)) {
                            xLog(hLog, XLL_FAIL, "select1: sSocket2 not in exceptfds");
                            bTestPassed = FALSE;
                        }
                    }
                    else {
                        if (0 != FD_ISSET(sSocket2, &exceptfds)) {
                            xLog(hLog, XLL_FAIL, "select1: sSocket2 in exceptfds");
                            bTestPassed = FALSE;
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    if (FUNC_ACCEPT == selectTable[dwTableIndex].dwFunction) {
                        nsSocket1 = accept(sSocket1, NULL, NULL);
                        if (INVALID_SOCKET == nsSocket1) {
                            xLog(hLog, XLL_FAIL, "accept1 failed");
                            bTestPassed = FALSE;
                        }
                        else {
                            // Setup the fdset structures
                            FD_ZERO(&readfds);
                            FD_ZERO(&writefds);
                            FD_ZERO(&exceptfds);

                            FD_SET(nsSocket1, &readfds);
                            FD_SET(nsSocket1, &writefds);
                            FD_SET(nsSocket1, &exceptfds);

                            // Setup the timeout
                            fdstimeout.tv_sec = 0;
                            fdstimeout.tv_usec = 0;

                            iReturnCode = select(0, &readfds, &writefds, &exceptfds, &fdstimeout);
                            if (1 != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "select after accept1 - EXPECTED: %d; RECEIVED: %d", 1, iReturnCode);
                                bTestPassed = FALSE;
                            }

                            if (0 != FD_ISSET(nsSocket1, &readfds)) {
                                xLog(hLog, XLL_FAIL, "select after accept1 - readfds set");
                                bTestPassed = FALSE;
                            }

                            if (0 == FD_ISSET(nsSocket1, &writefds)) {
                                xLog(hLog, XLL_FAIL, "select after accept1 - writefds not set");
                                bTestPassed = FALSE;
                            }

                            if (0 != FD_ISSET(nsSocket1, &exceptfds)) {
                                xLog(hLog, XLL_FAIL, "select after accept1 - exceptfds set");
                                bTestPassed = FALSE;
                            }
                        }

                        if (INVALID_SOCKET != sSocket2) {
                            nsSocket2 = accept(sSocket2, NULL, NULL);
                            if (INVALID_SOCKET == nsSocket2) {
                                xLog(hLog, XLL_FAIL, "accept2 failed");
                                bTestPassed = FALSE;
                            }
                            else {
                                // Setup the fdset structures
                                FD_ZERO(&readfds);
                                FD_ZERO(&writefds);
                                FD_ZERO(&exceptfds);

                                FD_SET(nsSocket2, &readfds);
                                FD_SET(nsSocket2, &writefds);
                                FD_SET(nsSocket2, &exceptfds);

                                // Setup the timeout
                                fdstimeout.tv_sec = 0;
                                fdstimeout.tv_usec = 0;

                                iReturnCode = select(0, &readfds, &writefds, &exceptfds, &fdstimeout);
                                if (1 != iReturnCode) {
                                    xLog(hLog, XLL_FAIL, "select after accept2 - EXPECTED: %d; RECEIVED: %d", 1, iReturnCode);
                                    bTestPassed = FALSE;
                                }

                                if (0 != FD_ISSET(nsSocket2, &readfds)) {
                                    xLog(hLog, XLL_FAIL, "select after accept2 - readfds not set");
                                    bTestPassed = FALSE;
                                }

                                if (0 == FD_ISSET(nsSocket2, &writefds)) {
                                    xLog(hLog, XLL_FAIL, "select after accept2 - writefds not set");
                                    bTestPassed = FALSE;
                                }

                                if (0 != FD_ISSET(nsSocket2, &exceptfds)) {
                                    xLog(hLog, XLL_FAIL, "select after accept2 - exceptfds set");
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                    }

                    if (FUNC_ACCEPT_FAIL == selectTable[dwTableIndex].dwFunction) {
                        nsSocket1 = accept(sSocket1, NULL, NULL);
                        if (INVALID_SOCKET != nsSocket1) {
                            xLog(hLog, XLL_FAIL, "accept1 passed");
                            bTestPassed = FALSE;
                        }

                        if (INVALID_SOCKET != sSocket2) {
                            nsSocket2 = accept(sSocket2, NULL, NULL);
                            if (INVALID_SOCKET != nsSocket2) {
                                xLog(hLog, XLL_FAIL, "accept2 passed");
                                bTestPassed = FALSE;
                            }
                        }
                    }

                    if (FUNC_CONNECT == selectTable[dwTableIndex].dwFunction) {
                        // Setup the fdset structures
                        FD_ZERO(&readfds);
                        FD_ZERO(&writefds);
                        FD_ZERO(&exceptfds);

                        FD_SET(sSocket1, &readfds);
                        FD_SET(sSocket1, &writefds);
                        FD_SET(sSocket1, &exceptfds);

                        // Setup the timeout
                        fdstimeout.tv_sec = 0;
                        fdstimeout.tv_usec = 0;

                        iReturnCode = select(0, &readfds, &writefds, &exceptfds, &fdstimeout);
                        if (1 != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "select after connect1 - EXPECTED: %d; RECEIVED: %d", 1, iReturnCode);
                            bTestPassed = FALSE;
                        }

                        if (0 != FD_ISSET(sSocket1, &readfds)) {
                            xLog(hLog, XLL_FAIL, "select after connect1 - readfds set");
                            bTestPassed = FALSE;
                        }

                        if (0 == FD_ISSET(sSocket1, &writefds)) {
                            xLog(hLog, XLL_FAIL, "select after connect1 - writefds not set");
                            bTestPassed = FALSE;
                        }

                        if (0 != FD_ISSET(sSocket1, &exceptfds)) {
                            xLog(hLog, XLL_FAIL, "select after connect1 - exceptfds set");
                            bTestPassed = FALSE;
                        }

                        ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                        iReturnCode = send(sSocket1, SendBuffer10, 10, 0);
                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "connect1 failed");
                            bTestPassed = FALSE;
                        }

                        if (INVALID_SOCKET != sSocket2) {
                            // Setup the fdset structures
                            FD_ZERO(&readfds);
                            FD_ZERO(&writefds);
                            FD_ZERO(&exceptfds);

                            FD_SET(sSocket2, &readfds);
                            FD_SET(sSocket2, &writefds);
                            FD_SET(sSocket2, &exceptfds);

                            // Setup the timeout
                            fdstimeout.tv_sec = 0;
                            fdstimeout.tv_usec = 0;

                            iReturnCode = select(0, &readfds, &writefds, &exceptfds, &fdstimeout);
                            if (1 != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "select after connect2 - EXPECTED: %d; RECEIVED: %d", 1, iReturnCode);
                                bTestPassed = FALSE;
                            }

                            if (0 != FD_ISSET(sSocket2, &readfds)) {
                                xLog(hLog, XLL_FAIL, "select after connect2 - readfds set");
                                bTestPassed = FALSE;
                            }

                            if (0 == FD_ISSET(sSocket2, &writefds)) {
                                xLog(hLog, XLL_FAIL, "select after connect2 - writefds not set");
                                bTestPassed = FALSE;
                            }

                            if (0 != FD_ISSET(sSocket2, &exceptfds)) {
                                xLog(hLog, XLL_FAIL, "select after connect2 - exceptfds set");
                                bTestPassed = FALSE;
                            }

                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            iReturnCode = send(sSocket2, SendBuffer10, 10, 0);
                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "connect2 failed");
                                bTestPassed = FALSE;
                            }
                        }
                    }

                    if (FUNC_CONNECT_FAIL == selectTable[dwTableIndex].dwFunction) {
                        ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                        iReturnCode = send(sSocket1, SendBuffer10, 10, 0);
                        if (SOCKET_ERROR != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "connect1 passed");
                            bTestPassed = FALSE;
                        }

                        if (INVALID_SOCKET != sSocket2) {
                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            iReturnCode = send(sSocket2, SendBuffer10, 10, 0);
                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "connect2 passed");
                                bTestPassed = FALSE;
                            }
                        }
                    }

                    if (FUNC_CLOSE == selectTable[dwTableIndex].dwFunction) {
                        ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                        iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);
                        if (0 != (FDSET_WRITE & selectTable[dwTableIndex].dwCheckSets)) {
                            if (WSAECONNRESET != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "close1 failed");
                                bTestPassed = FALSE;
                            }
                        }
                        else {
                            if (0 != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "close1 failed");
                                bTestPassed = FALSE;
                            }
                        }

                        if (INVALID_SOCKET != sSocket2) {
                            ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                            iReturnCode = recv(sSocket2, RecvBuffer10, sizeof(RecvBuffer10), 0);
                            if (0 != (FDSET_WRITE & selectTable[dwTableIndex].dwCheckSets)) {
                                if (WSAECONNRESET != WSAGetLastError()) {
                                    xLog(hLog, XLL_FAIL, "close2 failed");
                                    bTestPassed = FALSE;
                                }
                            }
                            else {
                                if (0 != iReturnCode) {
                                    xLog(hLog, XLL_FAIL, "close2 failed");
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                    }

                    if (FUNC_CLOSE_FAIL == selectTable[dwTableIndex].dwFunction) {
                        ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                        iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);
                        if (WSAEWOULDBLOCK != WSAGetLastError()) {
                            xLog(hLog, XLL_FAIL, "close1 passed");
                            bTestPassed = FALSE;
                        }

                        if (INVALID_SOCKET != sSocket2) {
                            ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                            iReturnCode = recv(sSocket2, RecvBuffer10, sizeof(RecvBuffer10), 0);
                            if (WSAEWOULDBLOCK != WSAGetLastError()) {
                                xLog(hLog, XLL_FAIL, "close2 passed");
                                bTestPassed = FALSE;
                            }
                        }
                    }

                    if (FUNC_READ == selectTable[dwTableIndex].dwFunction) {
                        ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                        sprintf(SendBuffer10, "%05d%05d", 1, 1);
                        ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                        iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);
                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv1 failed");
                            bTestPassed = FALSE;
                        }
                        else if (0 != strncmp(SendBuffer10, RecvBuffer10, 10)) {
                            xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                            bTestPassed = FALSE;
                        }

                        if (INVALID_SOCKET != sSocket2) {
                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            sprintf(SendBuffer10, "%05d%05d", 2, 2);
                            ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                            iReturnCode = recv(sSocket2, RecvBuffer10, sizeof(RecvBuffer10), 0);
                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv2 failed");
                                bTestPassed = FALSE;
                            }
                            else {
                            }
                        }
                    }

                    if (FUNC_READ_FAIL == selectTable[dwTableIndex].dwFunction) {
                        ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                        iReturnCode = recv(sSocket1, RecvBuffer10, sizeof(RecvBuffer10), 0);
                        if (SOCKET_ERROR != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "recv1 passed");
                            bTestPassed = FALSE;
                        }

                        if (INVALID_SOCKET != sSocket2) {
                            ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
                            iReturnCode = recv(sSocket2, RecvBuffer10, sizeof(RecvBuffer10), 0);
                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "recv2 passed");
                                bTestPassed = FALSE;
                            }
                        }
                    }

                    if (FUNC_SEND == selectTable[dwTableIndex].dwFunction) {
                        ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                        iReturnCode = send(sSocket1, SendBuffer10, 10, 0);
                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "send1 failed");
                            bTestPassed = FALSE;
                        }

                        if (INVALID_SOCKET != sSocket2) {
                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            iReturnCode = send(sSocket2, SendBuffer10, 10, 0);
                            if (SOCKET_ERROR == iReturnCode) {
                                xLog(hLog, XLL_FAIL, "send2 failed");
                                bTestPassed = FALSE;
                            }
                        }
                    }

                    if (FUNC_SEND_FAIL == selectTable[dwTableIndex].dwFunction) {
                        ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                        iReturnCode = send(sSocket1, SendBuffer10, 10, 0);
                        if (SOCKET_ERROR != iReturnCode) {
                            xLog(hLog, XLL_FAIL, "send1 passed");
                            bTestPassed = FALSE;
                        }

                        if (INVALID_SOCKET != sSocket2) {
                            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                            iReturnCode = send(sSocket2, SendBuffer10, 10, 0);
                            if (SOCKET_ERROR != iReturnCode) {
                                xLog(hLog, XLL_FAIL, "send2 passed");
                                bTestPassed = FALSE;
                            }
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "select succeeded");
                }
            }
        }

        if (0 != selectTable[dwTableIndex].dwFunction) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(selectRequest), (char *) &selectRequest);
        }

        // Close the socket
        if (INVALID_SOCKET != nsSocket2) {
            shutdown(nsSocket2, SD_BOTH);
            closesocket(nsSocket2);
        }

        if (INVALID_SOCKET != nsSocket1) {
            shutdown(nsSocket1, SD_BOTH);
            closesocket(nsSocket1);
        }

        if ((SOCKET_TCP == selectTable[dwTableIndex].dwSocket2) || (SOCKET_UDP == selectTable[dwTableIndex].dwSocket2)) {
            shutdown(sSocket2, SD_BOTH);
            closesocket(sSocket2);
        }

        if (0 == (SOCKET_CLOSED & selectTable[dwTableIndex].dwSocket1)) {
            if ((0 != (SOCKET_TCP & selectTable[dwTableIndex].dwSocket1)) || (0 != (SOCKET_UDP & selectTable[dwTableIndex].dwSocket1))) {
                shutdown(sSocket1, SD_BOTH);
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
selectTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests select - Server side

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
    // selectRequest is the request
    SELECT_REQUEST   selectRequest;
    // selectComplete is the result
    SELECT_COMPLETE  selectComplete;

    // sSocket1 is the first socket descriptor
    SOCKET           sSocket1;
    // sSocket2 is the second socket descriptor
    SOCKET           sSocket2;
    // nsSocket1 is the first accepted socket descriptor
    SOCKET           nsSocket1;
    // nsSocket2 is the second accepted socket descriptor
    SOCKET           nsSocket2;

    // dwBufferSize is the send/receive buffer size
    DWORD            dwBufferSize = 1;
    // bNagle indicates if Nagle is enabled
    BOOL             bNagle = FALSE;

    // localname1 is the first local address
    SOCKADDR_IN      localname1;
    // localname2 is the second local address
    SOCKADDR_IN      localname2;
    // remotename1 is the first remote address
    SOCKADDR_IN      remotename1;
    // remotename2 is the second remote address
    SOCKADDR_IN      remotename2;

    // SendBuffer10 is the send buffer
    char             SendBuffer10[BUFFER_10_LEN + 1];



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
        CopyMemory(&selectRequest, pMessage, sizeof(selectRequest));
        NetsyncFreeMessage(pMessage);

        // Create the sockets
        sSocket1 = INVALID_SOCKET;
        sSocket2 = INVALID_SOCKET;
        nsSocket1 = INVALID_SOCKET;
        nsSocket2 = INVALID_SOCKET;
        sSocket1 = socket(AF_INET, selectRequest.nSocketType, 0);
        if (2 == selectRequest.nSockets) {
            sSocket2 = socket(AF_INET, selectRequest.nSocketType, 0);
        }

        if (SOCK_STREAM == selectRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket1, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
            if (INVALID_SOCKET != sSocket2) {
                setsockopt(sSocket2, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
            }
        }

        // Set the buffer size
        dwBufferSize = 1;
        setsockopt(sSocket1, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

        dwBufferSize = 1;
        setsockopt(sSocket1, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

        if (INVALID_SOCKET != sSocket2) {
            dwBufferSize = 1;
            setsockopt(sSocket2, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

            dwBufferSize = 1;
            setsockopt(sSocket2, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
        }

        // Bind the socket
        ZeroMemory(&localname1, sizeof(localname1));
        localname1.sin_family = AF_INET;
        localname1.sin_port = htons(selectRequest.Port);
        bind(sSocket1, (SOCKADDR *) &localname1, sizeof(localname1));

        if (INVALID_SOCKET != sSocket2) {
            ZeroMemory(&localname2, sizeof(localname2));
            localname2.sin_family = AF_INET;
            localname2.sin_port = htons(selectRequest.Port + 1);
            bind(sSocket2, (SOCKADDR *) &localname2, sizeof(localname2));
        }

        if ((SOCK_STREAM == selectRequest.nSocketType) && (TRUE == selectRequest.bServerAccept)) {
            // Place the socket in listening mode
            listen(sSocket1, SOMAXCONN);

            if (INVALID_SOCKET != sSocket2) {
                listen(sSocket2, SOMAXCONN);
            }
        }

        if (SOCK_DGRAM == selectRequest.nSocketType) {
            // Connect the socket
            ZeroMemory(&remotename1, sizeof(remotename1));
            remotename1.sin_family = AF_INET;
            remotename1.sin_addr.s_addr = FromInAddr;
            remotename1.sin_port = htons(selectRequest.Port);

            connect(sSocket1, (SOCKADDR *) &remotename1, sizeof(remotename1));

            if (INVALID_SOCKET != sSocket2) {
                ZeroMemory(&remotename2, sizeof(remotename2));
                remotename2.sin_family = AF_INET;
                remotename2.sin_addr.s_addr = FromInAddr;
                remotename2.sin_port = htons(selectRequest.Port + 1);

                connect(sSocket2, (SOCKADDR *) &remotename2, sizeof(remotename2));
            }
        }

        // Send the complete
        selectComplete.dwMessageId = SELECT_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(selectComplete), (char *) &selectComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        // Send the complete
        selectComplete.dwMessageId = SELECT_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(selectComplete), (char *) &selectComplete);

        if ((SOCK_STREAM == selectRequest.nSocketType) && (TRUE == selectRequest.bServerAccept)) {
            // Accept the connection
            nsSocket1 = accept(sSocket1, NULL, NULL);

            if (INVALID_SOCKET != sSocket2) {
                nsSocket2 = accept(sSocket2, NULL, NULL);
            }
        }

        // Sleep
        if (0 == selectRequest.dwWaitTime) {
            Sleep(SLEEP_MIDLOW_TIME);
        }
        else {
            Sleep(SLEEP_MIDLOW_TIME + SLEEP_MEAN_TIME);
        }

        if (TRUE == selectRequest.bServerConnect) {
            // Connect the socket
            ZeroMemory(&remotename1, sizeof(remotename1));
            remotename1.sin_family = AF_INET;
            remotename1.sin_addr.s_addr = FromInAddr;
            remotename1.sin_port = htons(selectRequest.Port);

            connect(sSocket1, (SOCKADDR *) &remotename1, sizeof(remotename1));

            if (INVALID_SOCKET != sSocket2) {
                ZeroMemory(&remotename1, sizeof(remotename1));
                remotename2.sin_family = AF_INET;
                remotename2.sin_addr.s_addr = FromInAddr;
                remotename2.sin_port = htons(selectRequest.Port + 1);

                connect(sSocket2, (SOCKADDR *) &remotename2, sizeof(remotename2));
            }
        }

        if (TRUE == selectRequest.bServerSend) {
            // Call send
            ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
            sprintf(SendBuffer10, "%05d%05d", 1, 1);

            send((INVALID_SOCKET == nsSocket1) ? sSocket1 : nsSocket1, SendBuffer10, 10, 0);

            if (INVALID_SOCKET != sSocket2) {
                ZeroMemory(SendBuffer10, sizeof(SendBuffer10));
                sprintf(SendBuffer10, "%05d%05d", 2, 2);

                send((INVALID_SOCKET == nsSocket2) ? sSocket2 : nsSocket2, SendBuffer10, 10, 0);
            }
        }

        if (TRUE == selectRequest.bServerClose) {
            // Close the sockets
            if (INVALID_SOCKET != nsSocket2) {
                shutdown(nsSocket2, SD_BOTH);
            }

            if (INVALID_SOCKET != nsSocket1) {
                shutdown(nsSocket1, SD_BOTH);
            }
        }

        // Wait for the ack
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        // Close the sockets
        if (INVALID_SOCKET != nsSocket2) {
            shutdown(nsSocket2, SD_BOTH);
            closesocket(nsSocket2);
        }

        if (INVALID_SOCKET != nsSocket1) {
            shutdown(nsSocket1, SD_BOTH);
            closesocket(nsSocket1);
        }

        if (INVALID_SOCKET != sSocket2) {
            shutdown(sSocket2, SD_BOTH);
            closesocket(sSocket2);
        }

        if (INVALID_SOCKET != sSocket1) {
            shutdown(sSocket1, SD_BOTH);
            closesocket(sSocket1);
        }
    }
}

#endif
