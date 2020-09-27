/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  wsagetoverlappedresult.c

Abstract:

  This modules tests WSAGetOverlappedResult

Author:

  Steven Kehrli (steveke) 29-Jan-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// WSAGetOverlappedResult messages

#define WSAGETOVERLAPPEDRESULT_REQUEST_MSG   NETSYNC_MSG_USER + 240 + 1
#define WSAGETOVERLAPPEDRESULT_COMPLETE_MSG  NETSYNC_MSG_USER + 240 + 2

typedef struct _WSAGETOVERLAPPEDRESULT_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  Port;
    BOOL     bServerSend;
    BOOL     bServerRecv;
    int      nQueueLen;
    BOOL     bComplete;
} WSAGETOVERLAPPEDRESULT_REQUEST, *PWSAGETOVERLAPPEDRESULT_REQUEST;

typedef struct _WSAGETOVERLAPPEDRESULT_COMPLETE {
    DWORD    dwMessageId;
} WSAGETOVERLAPPEDRESULT_COMPLETE, *PWSAGETOVERLAPPEDRESULT_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

#define WSAOVERLAPPED_NULL           1
#define WSAOVERLAPPED_NULL_EVENT     2
#define WSAOVERLAPPED_VALID          3

#define WSA_CANCEL                   0x00000001
#define WSA_SECOND_CALL              0x00000002
#define WSASEND_FUNCTION             0x00000010
#define WSASENDTO_FUNCTION           0x00000020
#define WSARECV_FUNCTION             0x00000040
#define WSARECVFROM_FUNCTION         0x00000080

typedef struct WSAGETOVERLAPPEDRESULT_TABLE {
    CHAR   szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL   bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL   bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD  dwSocket;                                // dwSocket indicates the socket to be created
    DWORD  dwFunction;                              // dwFunction specifies the function to start the pending overlapped operation
    DWORD  dwOverlapped;                            // dwOverlapped specifies the overlapped structure
    BOOL   bPartial;                                // bPartial indicates if the MSG_PARTIAL flag should be present
    BOOL   bComplete;                               // bComplete indicates if the operation completes before the result call
    BOOL   bBytes;                                  // bBytes indicates pdwBytes is valid
    BOOL   bWait;                                   // bWait specifies if the function waits until the pending overlapped operation is complete
    BOOL   bFlags;                                  // bFlags indicates pdwFlags is valid
    BOOL   bReturnCode;                             // bReturnCode is the return code of WSAGetOverlappedResult
    int    iLastError;                              // iLastError is the error code if the operation failed
    BOOL   bRIP;                                    // Specifies a RIP test case
} WSAGETOVERLAPPEDRESULT_TABLE, *PWSAGETOVERLAPPEDRESULT_TABLE;

static WSAGETOVERLAPPEDRESULT_TABLE WSAGetOverlappedResultTable[] =
{
    { "24.1 Not Initialized",            FALSE, FALSE, SOCKET_INVALID_SOCKET,      0,                                                      WSAOVERLAPPED_NULL,       FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, WSANOTINITIALISED, FALSE },
    { "24.2 s = INT_MIN",                TRUE,  TRUE,  SOCKET_INT_MIN,             0,                                                      WSAOVERLAPPED_NULL,       FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, WSAENOTSOCK,       FALSE },
    { "24.3 s = -1",                     TRUE,  TRUE,  SOCKET_NEG_ONE,             0,                                                      WSAOVERLAPPED_NULL,       FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, WSAENOTSOCK,       FALSE },
    { "24.4 s = 0",                      TRUE,  TRUE,  SOCKET_ZERO,                0,                                                      WSAOVERLAPPED_NULL,       FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, WSAENOTSOCK,       FALSE },
    { "24.5 s = INT_MAX",                TRUE,  TRUE,  SOCKET_INT_MAX,             0,                                                      WSAOVERLAPPED_NULL,       FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, WSAENOTSOCK,       FALSE },
    { "24.6 s = INVALID_SOCKET",         TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      0,                                                      WSAOVERLAPPED_NULL,       FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, WSAENOTSOCK,       FALSE },
    { "24.7 TCP Send WDone",             TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.8 TCP Send WPend",             TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.9 TCP Send 2 WDone",           TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.10 TCP Send 2 WPend",          TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.11 TCP Send WCanc",            TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSA_CANCEL,                          WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.12 TCP Send Done",             TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.13 TCP Send Pend",             TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.14 TCP Send 2 Done",           TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.15 TCP Send 2 Pend",           TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.16 TCP Send Canc",             TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSA_CANCEL,                          WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.17 TCP Send NULL IO",          TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION,                                       WSAOVERLAPPED_NULL,       FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.18 TCP Send NULL Event",       TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION,                                       WSAOVERLAPPED_NULL_EVENT, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.19 TCP Send NULL Bytes",       TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.20 TCP Send NULL Flags",       TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, WSAEFAULT,         TRUE  },
    { "24.21 TCP SendTo WDone",          TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION,                                     WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.22 TCP SendTo WPend",          TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION,                                     WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.23 TCP SendTo 2 WDone",        TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSA_SECOND_CALL,                   WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.24 TCP SendTo 2 WPend",        TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSA_SECOND_CALL,                   WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.25 TCP SendTo WCanc",          TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSA_CANCEL,                        WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.26 TCP SendTo Done",           TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION,                                     WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.27 TCP SendTo Pend",           TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION,                                     WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.28 TCP SendTo 2 Done",         TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSA_SECOND_CALL,                   WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.29 TCP SendTo 2 Pend",         TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSA_SECOND_CALL,                   WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.30 TCP SendTo Canc",           TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSA_CANCEL,                        WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.31 TCP SendTo NULL IO",        TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION,                                     WSAOVERLAPPED_NULL,       FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.32 TCP SendTo NULL Event",     TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION,                                     WSAOVERLAPPED_NULL_EVENT, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.33 TCP SendTo NULL Bytes",     TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION,                                     WSAOVERLAPPED_VALID,      FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.34 TCP SendTo NULL Flags",     TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION,                                     WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, WSAEFAULT,         TRUE  },
    { "24.35 TCP Recv WDone",            TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.36 TCP Recv WPend",            TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.37 TCP Recv 2 WDone",          TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.38 TCP Recv 2 WPend",          TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.39 TCP Recv WCanc",            TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION | WSA_CANCEL,                          WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.40 TCP Recv Done",             TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.41 TCP Recv Pend",             TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.42 TCP Recv 2 Done",           TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.43 TCP Recv 2 Pend",           TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.44 TCP Recv Canc",             TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION | WSA_CANCEL,                          WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.45 TCP Recv NULL IO",          TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_NULL,       FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.46 TCP Recv NULL Event",       TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_NULL_EVENT, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.47 TCP Recv NULL Bytes",       TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.48 TCP Recv NULL Flags",       TRUE,  TRUE,  SOCKET_TCP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, WSAEFAULT,         TRUE  },
    { "24.49 TCP RecvFrom WDone",        TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION,                                   WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.50 TCP RecvFrom WPend",        TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION,                                   WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.51 TCP RecvFrom 2 WDone",      TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION | WSA_SECOND_CALL,                 WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.52 TCP RecvFrom 2 WPend",      TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION | WSA_SECOND_CALL,                 WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.53 TCP RecvFrom WCanc",        TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION | WSA_CANCEL,                      WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.54 TCP RecvFrom Done",         TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION,                                   WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.55 TCP RecvFrom Pend",         TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION,                                   WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.56 TCP RecvFrom 2 Done",       TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION | WSA_SECOND_CALL,                 WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.57 TCP RecvFrom 2 Pend",       TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION | WSA_SECOND_CALL,                 WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.58 TCP RecvFrom Canc",         TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION | WSA_CANCEL,                      WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.59 TCP RecvFrom NULL IO",      TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION,                                   WSAOVERLAPPED_NULL,       FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.60 TCP RecvFrom NULL Event",   TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION,                                   WSAOVERLAPPED_NULL_EVENT, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.61 TCP RecvFrom NULL Bytes",   TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION,                                   WSAOVERLAPPED_VALID,      FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.62 TCP RecvFrom NULL Flags",   TRUE,  TRUE,  SOCKET_TCP,                 WSARECVFROM_FUNCTION,                                   WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, WSAEFAULT,         TRUE  },
    { "24.63 TCP Send/Recv WDone",       TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECV_FUNCTION,                    WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.64 TCP Send/Recv WPend",       TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECV_FUNCTION,                    WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.65 TCP Send/Recv WCanc",       TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECV_FUNCTION | WSA_CANCEL,       WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.66 TCP Send/Recv Done",        TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECV_FUNCTION,                    WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.67 TCP Send/Recv Pend",        TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECV_FUNCTION,                    WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.68 TCP Send/Recv Canc",        TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECV_FUNCTION | WSA_CANCEL,       WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.69 TCP SendTo/Recv WDone",     TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECV_FUNCTION,                  WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.70 TCP SendTo/Recv WPend",     TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECV_FUNCTION,                  WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.71 TCP SendTo/Recv WCanc",     TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECV_FUNCTION | WSA_CANCEL,     WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.72 TCP SendTo/Recv Done",      TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECV_FUNCTION,                  WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.73 TCP SendTo/Recv Pend",      TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECV_FUNCTION,                  WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.74 TCP SendTo/Recv Canc",      TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECV_FUNCTION | WSA_CANCEL,     WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.75 TCP Send/RecvFrom WDone",   TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECVFROM_FUNCTION,                WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.76 TCP Send/RecvFrom WPend",   TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECVFROM_FUNCTION,                WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.77 TCP Send/RecvFrom WCanc",   TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECVFROM_FUNCTION | WSA_CANCEL,   WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.78 TCP Send/RecvFrom Done",    TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECVFROM_FUNCTION,                WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.79 TCP Send/RecvFrom Pend",    TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECVFROM_FUNCTION,                WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.80 TCP Send/RecvFrom Canc",    TRUE,  TRUE,  SOCKET_TCP,                 WSASEND_FUNCTION | WSARECVFROM_FUNCTION | WSA_CANCEL,   WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.81 TCP SendTo/RecvFrom WDone", TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECVFROM_FUNCTION,              WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.82 TCP SendTo/RecvFrom WPend", TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECVFROM_FUNCTION,              WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.83 TCP SendTo/RecvFrom WCanc", TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECVFROM_FUNCTION | WSA_CANCEL, WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.84 TCP SendTo/RecvFrom Done",  TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECVFROM_FUNCTION,              WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.85 TCP SendTo/RecvFrom Pend",  TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECVFROM_FUNCTION,              WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.86 TCP SendTo/RecvFrom Canc",  TRUE,  TRUE,  SOCKET_TCP,                 WSASENDTO_FUNCTION | WSARECVFROM_FUNCTION | WSA_CANCEL, WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.87 UDP Recv WDone",            TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.88 UDP Recv WPend",            TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.89 UDP Recv WPart",            TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEMSGSIZE,       FALSE },
    { "24.90 UDP Recv 2 WDone",          TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.91 UDP Recv 2 WPend",          TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.92 UDP Recv 2 WPart",          TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEMSGSIZE,       FALSE },
    { "24.93 UDP Recv WCanc",            TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_CANCEL,                          WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.94 UDP Recv Done",             TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.95 UDP Recv Pend",             TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.96 UDP Recv Part",             TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, WSAEMSGSIZE,       FALSE },
    { "24.97 UDP Recv 2 Done",           TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.98 UDP Recv 2 Pend",           TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.99 UDP Recv 2 Part",           TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, WSAEMSGSIZE,       FALSE },
    { "24.100 UDP Recv Canc",            TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_CANCEL,                          WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.101 UDP Recv NULL IO",         TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_NULL,       FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.102 UDP Recv NULL Event",      TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_NULL_EVENT, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.103 UDP Recv NULL Bytes",      TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.104 UDP Recv NULL Flags",      TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, WSAEFAULT,         TRUE  },
    { "24.105 UDP RecvFrom WDone",       TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.106 UDP RecvFrom WPend",       TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.107 UDP RecvFrom WPart",       TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEMSGSIZE,       FALSE },
    { "24.108 UDP RecvFrom 2 WDone",     TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.109 UDP RecvFrom 2 WPend",     TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "24.110 UDP RecvFrom 2 WPart",     TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEMSGSIZE,       FALSE },
    { "24.111 UDP RecvFrom WCanc",       TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_CANCEL,                          WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.112 UDP RecvFrom Done",        TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.113 UDP RecvFrom Pend",        TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.114 UDP RecvFrom Part",        TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, WSAEMSGSIZE,       FALSE },
    { "24.115 UDP RecvFrom 2 Done",      TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "24.116 UDP RecvFrom 2 Pend",      TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSA_IO_INCOMPLETE, FALSE },
    { "24.117 UDP RecvFrom 2 Part",      TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_SECOND_CALL,                     WSAOVERLAPPED_VALID,      TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, WSAEMSGSIZE,       FALSE },
    { "24.118 UDP RecvFrom Canc",        TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION | WSA_CANCEL,                          WSAOVERLAPPED_VALID,      FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, WSAECANCELLED,     FALSE },
    { "24.119 UDP RecvFrom NULL IO",     TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_NULL,       FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.120 UDP RecvFrom NULL Event",  TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_NULL_EVENT, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.121 UDP RecvFrom NULL Bytes",  TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, WSAEFAULT,         TRUE  },
    { "24.122 UDP RecvFrom NULL Flags",  TRUE,  TRUE,  SOCKET_UDP,                 WSARECV_FUNCTION,                                       WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, WSAEFAULT,         TRUE  },
    { "24.123 Closed Socket TCP",        TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, 0,                                                      WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAENOTSOCK,       FALSE },
    { "24.124 Closed Socket UDP",        TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, 0,                                                      WSAOVERLAPPED_VALID,      FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAENOTSOCK,       FALSE },
    { "24.125 Not Initialized",          FALSE, FALSE, SOCKET_INVALID_SOCKET,      0,                                                      WSAOVERLAPPED_NULL,       FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, WSANOTINITIALISED, FALSE }
};

#define WSAGetOverlappedResultTableCount (sizeof(WSAGetOverlappedResultTable) / sizeof(WSAGETOVERLAPPEDRESULT_TABLE))

NETSYNC_TYPE_THREAD  WSAGetOverlappedResultTestSessionNt =
{
    1,
    WSAGetOverlappedResultTableCount,
    L"xnetapi_nt.dll",
    "WSAGetOverlappedResultTestServer"
};

NETSYNC_TYPE_THREAD  WSAGetOverlappedResultTestSessionXbox =
{
    1,
    WSAGetOverlappedResultTableCount,
    L"xnetapi_xbox.dll",
    "WSAGetOverlappedResultTestServer"
};



VOID
WSAGetOverlappedResultTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests WSAGetOverlappedResult - Client side

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
    LPSTR                           lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR                           lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD                           dwTableIndex;

    // bWinsockInitialized indicates if Winsock is initialized
    BOOL                            bWinsockInitialized = FALSE;
    // WSAData is the details of the Winsock implementation
    WSADATA                         WSAData;

    // hNetsyncObject is a handle to the netsync object
    HANDLE                          hNetsyncObject = INVALID_HANDLE_VALUE;
    // NetsyncTypeSession is the session type descriptor
    NETSYNC_TYPE_THREAD             NetsyncTypeSession;
    // NetsyncInAddr is the address of the netsync server
    u_long                          NetsyncInAddr = 0;
    // LowPort is the low bound of the netsync port range
    u_short                         LowPort = 0;
    // HighPort is the high bound of the netsync port range
    u_short                         HighPort = 0;
    // CurrentPort is the current port in the netsync port range
    u_short                         CurrentPort = 0;
    // FromInAddr is the address of the netsync sender
    u_long                          FromInAddr;
    // dwMessageSize is the size of the received message
    DWORD                           dwMessageSize;
    // pMessage is a pointer to the received message
    char                            *pMessage;
    // WSAGetOverlappedResultRequest is the request sent to the server
    WSAGETOVERLAPPEDRESULT_REQUEST  WSAGetOverlappedResultRequest;
    
    // sSocket is the socket descriptor
    SOCKET                          sSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD                           dwBufferSize = BUFFER_10_LEN;
    // bNagle indicates if Nagle is enabled
    BOOL                            bNagle = FALSE;
    // Nonblock sets the socket to blocking or non-blocking mode
    u_long                          Nonblock;
    // bNonblocking indicates if the socket is in non-blocking mode
    BOOL                            bNonblocking = FALSE;

    // localname is the local address
    SOCKADDR_IN                     localname;
    // remotename1 is the first remote address
    SOCKADDR_IN                     remotename1;
    // remotename2 is the second remote address
    SOCKADDR_IN                     remotename2;
    // namelen is the size of the address buffer
    int                             namelen;

    // writefds is the set of sockets to check for a write condition
    fd_set                          writefds;

    // SendBuffer10 is the send buffer
    char                            SendBuffer10[BUFFER_10_LEN + 1];
    // SendBufferLarge is the large send buffer
    char                            SendBufferLarge[BUFFER_LARGE_LEN + 1];
    // RecvBuffer10 is the recv buffer
    char                            RecvBuffer10[BUFFER_10_LEN + 1];

    // dwFirstTime is the first tick count
    DWORD                           dwFirstTime;
    // dwSecondTime is the second tick count
    DWORD                           dwSecondTime;

    // WSASendBuf is the send WSABUF structure
    WSABUF                          WSASendBuf;
    // WSARecvBuf is the recv WSABUF structure
    WSABUF                          WSARecvBuf;

    // dwSendBytes1 is the number of bytes transferred for the function call
    DWORD                           dwSendBytes1;
    // dwSendBytes2 is the number of bytes transferred for the result call
    DWORD                           dwSendBytes2;
    // dwRecvBytes1 is the number of bytes transferred for the function call
    DWORD                           dwRecvBytes1;
    // dwRecvBytes2 is the number of bytes transferred for the result call
    DWORD                           dwRecvBytes2;

    // dwSendFlags2 is the transfer flags for the result call
    DWORD                           dwSendFlags2;
    // dwRecvFlags1 is the transfer flags for the function call
    DWORD                           dwRecvFlags1;
    // dwRecvFlags2 is the transfer flags for the result call
    DWORD                           dwRecvFlags2;

    // hSendEvent1 is a handle to the overlapped event
    HANDLE                          hSendEvent1;
    // hSendEvent2 is a handle to the overlapped event
    HANDLE                          hSendEvent2;
    // hRecvEvent1 is a handle to the overlapped event
    HANDLE                          hRecvEvent1;
    // hRecvEvent2 is a handle to the overlapped event
    HANDLE                          hRecvEvent2;

    // WSASendOverlapped1 is the overlapped structure
    WSAOVERLAPPED                   WSASendOverlapped1;
    // WSASendOverlapped2 is the overlapped structure
    WSAOVERLAPPED                   WSASendOverlapped2;
    // WSARecvOverlapped1 is the overlapped structure
    WSAOVERLAPPED                   WSARecvOverlapped1;
    // WSARecvOverlapped2 is the overlapped structure
    WSAOVERLAPPED                   WSARecvOverlapped2;

    // bSendReturnCode is the send return code
    BOOL                            bSendReturnCode;
    // bRecvReturnCode is the recv return code
    BOOL                            bRecvReturnCode;
    // bReturnCode is the return code
    BOOL                            bReturnCode;

    // bException indicates if an exception occurred
    BOOL                            bException;
    // iReturnCode is the return code of the operation
    int                             iReturnCode;
    // iLastError is the error code if the operation failed
    int                             iLastError;
    // bTestPassed indicates if the test passed
    BOOL                            bTestPassed;

    // szFunctionName is the function name
    CHAR                            szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    sprintf(szFunctionName, "WSAGetOverlapped... v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_WSAGetOverlappedResult+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_WSAGetOverlappedResult-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = WSAGetOverlappedResultTestSessionXbox;
    }
    else {
        NetsyncTypeSession = WSAGetOverlappedResultTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < WSAGetOverlappedResultTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, WSAGetOverlappedResultTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, WSAGetOverlappedResultTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != WSAGetOverlappedResultTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, WSAGetOverlappedResultTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == WSAGetOverlappedResultTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != WSAGetOverlappedResultTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = WSAGetOverlappedResultTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bNetsyncConnected)) {
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

        // Create the overlapped events
        hSendEvent1 = CreateEvent(NULL, FALSE, FALSE, NULL);
        hSendEvent2 = CreateEvent(NULL, FALSE, FALSE, NULL);
        hRecvEvent1 = CreateEvent(NULL, FALSE, FALSE, NULL);
        hRecvEvent2 = CreateEvent(NULL, FALSE, FALSE, NULL);

        // Setup the overlapped structures
        ZeroMemory(&WSASendOverlapped1, sizeof(WSASendOverlapped1));
        WSASendOverlapped1.hEvent = hSendEvent1;

        ZeroMemory(&WSASendOverlapped2, sizeof(WSASendOverlapped2));
        WSASendOverlapped2.hEvent = hSendEvent2;

        ZeroMemory(&WSARecvOverlapped1, sizeof(WSARecvOverlapped1));
        WSARecvOverlapped1.hEvent = hRecvEvent1;

        ZeroMemory(&WSARecvOverlapped2, sizeof(WSARecvOverlapped2));
        WSARecvOverlapped2.hEvent = hRecvEvent2;

        // Create the socket
        sSocket = INVALID_SOCKET;
        if (SOCKET_INT_MIN == WSAGetOverlappedResultTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == WSAGetOverlappedResultTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == WSAGetOverlappedResultTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == WSAGetOverlappedResultTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == WSAGetOverlappedResultTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & WSAGetOverlappedResultTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & WSAGetOverlappedResultTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & WSAGetOverlappedResultTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        if ((0 != (SOCKET_TCP & WSAGetOverlappedResultTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & WSAGetOverlappedResultTable[dwTableIndex].dwSocket))) {
            // Set the buffer size
            dwBufferSize = 10;
            setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

            dwBufferSize = 10;
            setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

            // Set the socket to non-blocking mode
            Nonblock = 1;
            ioctlsocket(sSocket, FIONBIO, &Nonblock);

            // Bind the socket
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        if (0 != WSAGetOverlappedResultTable[dwTableIndex].dwFunction) {
            // Initialize the WSAGetOverlappedResult request
            WSAGetOverlappedResultRequest.dwMessageId = WSAGETOVERLAPPEDRESULT_REQUEST_MSG;
            if (0 != (SOCKET_TCP & WSAGetOverlappedResultTable[dwTableIndex].dwSocket)) {
                WSAGetOverlappedResultRequest.nSocketType = SOCK_STREAM;
            }
            else {
                WSAGetOverlappedResultRequest.nSocketType = SOCK_DGRAM;
            }
            WSAGetOverlappedResultRequest.Port = CurrentPort;
            WSAGetOverlappedResultRequest.bServerSend = (0 != (WSARECV_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) || (0 != (WSARECVFROM_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction));
            WSAGetOverlappedResultRequest.bServerRecv = (0 != (WSASEND_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) || (0 != (WSASENDTO_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction));
            WSAGetOverlappedResultRequest.nQueueLen = 0;
            WSAGetOverlappedResultRequest.bComplete = WSAGetOverlappedResultTable[dwTableIndex].bComplete;

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSAGetOverlappedResultRequest), (char *) &WSAGetOverlappedResultRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            // Connect the socket
            ZeroMemory(&remotename1, sizeof(remotename1));
            remotename1.sin_family = AF_INET;
            remotename1.sin_addr.s_addr = FromInAddr;
            remotename1.sin_port = htons(CurrentPort);

            connect(sSocket, (SOCKADDR *) &remotename1, sizeof(remotename1));

            if (0 != (SOCKET_TCP & WSAGetOverlappedResultTable[dwTableIndex].dwSocket)) {
                FD_ZERO(&writefds);
                FD_SET(sSocket, &writefds);
                select(0, NULL, &writefds, NULL, NULL);
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSAGetOverlappedResultRequest), (char *) &WSAGetOverlappedResultRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);
        }

        // Setup the WSABuf structure
        ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
        WSASendBuf.len = BUFFER_TCPSEGMENT_LEN;
        WSASendBuf.buf = SendBufferLarge;

        ZeroMemory(RecvBuffer10, sizeof(RecvBuffer10));
        if (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bPartial) {
            WSARecvBuf.len = 10 / 2;
        }
        else {
            WSARecvBuf.len = 10;
        }
        WSARecvBuf.buf = RecvBuffer10;

        // Initialize dwBytes and dwFlags
        dwSendBytes1 = 0;
        dwSendBytes2 = 0;
        dwRecvBytes1 = 0;
        dwRecvBytes2 = 0;

        dwSendFlags2 = 0;
        dwRecvFlags1 = 0;
        dwRecvFlags2 = 0;

        // Start the pending overlapped operation
        if (0 != (WSASEND_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) {
            // Fill the queue
            ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
            while (SOCKET_ERROR != send(sSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0)) {
                WSAGetOverlappedResultRequest.nQueueLen++;
                Sleep(SLEEP_ZERO_TIME);
            }

            // Call WSASend
            iReturnCode = WSASend(sSocket, &WSASendBuf, 1, &dwSendBytes1, 0, &WSASendOverlapped1, NULL);

            if (SOCKET_ERROR != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSASend returned non-SOCKET_ERROR");
            }
            else if (WSA_IO_PENDING != WSAGetLastError()) {
                xLog(hLog, XLL_FAIL, "WSASend iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, WSAGetLastError());
            }
        }
        else if (0 != (WSASENDTO_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) {
            // Fill the queue
            ZeroMemory(SendBufferLarge, sizeof(SendBufferLarge));
            while (SOCKET_ERROR != send(sSocket, SendBufferLarge, BUFFER_TCPSEGMENT_LEN, 0)) {
                WSAGetOverlappedResultRequest.nQueueLen++;
                Sleep(SLEEP_ZERO_TIME);
            }

            // Call WSASendTo
            ZeroMemory(&remotename1, sizeof(remotename1));
            remotename1.sin_family = AF_INET;
            remotename1.sin_addr.s_addr = FromInAddr;
            remotename1.sin_port = htons(CurrentPort);

            iReturnCode = WSASendTo(sSocket, &WSASendBuf, 1, &dwSendBytes1, 0, (SOCKADDR *) &remotename1, sizeof(remotename1), &WSASendOverlapped1, NULL);

            if (SOCKET_ERROR != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSASendTo returned non-SOCKET_ERROR");
            }
            else if (WSA_IO_PENDING != WSAGetLastError()) {
                xLog(hLog, XLL_FAIL, "WSASendTo iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, WSAGetLastError());
            }
        }

        if (0 != (WSARECV_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) {
            // Call WSARecv
            iReturnCode = WSARecv(sSocket, &WSARecvBuf, 1, &dwRecvBytes1, &dwRecvFlags1, &WSARecvOverlapped1, NULL);

            if (SOCKET_ERROR != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSARecv returned non-SOCKET_ERROR");
            }
            else if (WSA_IO_PENDING != WSAGetLastError()) {
                xLog(hLog, XLL_FAIL, "WSARecv iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, WSAGetLastError());
            }
        }
        else if (0 != (WSARECVFROM_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) {
            // Call WSARecvFrom
            namelen = sizeof(remotename2);
            ZeroMemory(&remotename2, sizeof(remotename2));

            iReturnCode = WSARecvFrom(sSocket, &WSARecvBuf, 1, &dwRecvBytes1, &dwRecvFlags1, (SOCKADDR *) &remotename2, &namelen, &WSARecvOverlapped1, NULL);

            if (SOCKET_ERROR != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSARecvFrom returned non-SOCKET_ERROR");
            }
            else if (WSA_IO_PENDING != WSAGetLastError()) {
                xLog(hLog, XLL_FAIL, "WSARecvFrom iLastError - EXPECTED: %u; RECEIVED: %u", WSA_IO_PENDING, WSAGetLastError());
            }
        }

        // Start another pending overlapped operation
        if ((0 != (WSASEND_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) && (0 != (WSA_SECOND_CALL & WSAGetOverlappedResultTable[dwTableIndex].dwFunction))) {
            // Call WSASend
            iReturnCode = WSASend(sSocket, &WSASendBuf, 1, &dwSendBytes2, 0, &WSASendOverlapped2, NULL);

            if (SOCKET_ERROR != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSASend returned non-SOCKET_ERROR");
            }
            else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                xLog(hLog, XLL_FAIL, "WSASend iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
            }
        }
        else if ((0 != (WSASENDTO_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) && (0 != (WSA_SECOND_CALL & WSAGetOverlappedResultTable[dwTableIndex].dwFunction))) {
            // Call WSASendTo
            ZeroMemory(&remotename1, sizeof(remotename1));
            remotename1.sin_family = AF_INET;
            remotename1.sin_addr.s_addr = FromInAddr;
            remotename1.sin_port = htons(CurrentPort);

            iReturnCode = WSASendTo(sSocket, &WSASendBuf, 1, &dwSendBytes2, 0, (SOCKADDR *) &remotename1, sizeof(remotename1), &WSASendOverlapped2, NULL);

            if (SOCKET_ERROR != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSASendTo returned non-SOCKET_ERROR");
            }
            else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                xLog(hLog, XLL_FAIL, "WSASendTo iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
            }
        }

        if ((0 != (WSARECV_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) && (0 != (WSA_SECOND_CALL & WSAGetOverlappedResultTable[dwTableIndex].dwFunction))) {
            // Call WSARecv
            iReturnCode = WSARecv(sSocket, &WSARecvBuf, 1, &dwRecvBytes2, &dwRecvFlags2, &WSARecvOverlapped2, NULL);

            if (SOCKET_ERROR != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSARecv returned non-SOCKET_ERROR");
            }
            else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                xLog(hLog, XLL_FAIL, "WSARecv iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
            }
        }
        else if ((0 != (WSARECVFROM_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) && (0 != (WSA_SECOND_CALL & WSAGetOverlappedResultTable[dwTableIndex].dwFunction))) {
            // Call WSARecvFrom
            namelen = sizeof(remotename2);
            ZeroMemory(&remotename2, sizeof(remotename2));

            iReturnCode = WSARecvFrom(sSocket, &WSARecvBuf, 1, &dwRecvBytes2, &dwRecvFlags2, (SOCKADDR *) &remotename2, &namelen, &WSARecvOverlapped2, NULL);

            if (SOCKET_ERROR != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSARecvFrom returned non-SOCKET_ERROR");
            }
            else if (WSAEWOULDBLOCK != WSAGetLastError()) {
                xLog(hLog, XLL_FAIL, "WSARecvFrom iLastError - EXPECTED: %u; RECEIVED: %u", WSAEWOULDBLOCK, WSAGetLastError());
            }
        }

        // Cancel the pending overlapped operation
        if (0 != (WSA_CANCEL & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) {
            // Call WSACancelOverlappedIO
            iReturnCode = WSACancelOverlappedIO(sSocket);

            if (0 != iReturnCode) {
                xLog(hLog, XLL_FAIL, "WSACancelOverlappedIO returned SOCKET_ERROR - ec = %u", WSAGetLastError());
            }
        }

        if (WSAOVERLAPPED_NULL_EVENT == WSAGetOverlappedResultTable[dwTableIndex].dwOverlapped) {
            WSASendOverlapped1.hEvent = NULL;
            WSARecvOverlapped1.hEvent = NULL;
        }

        if (0 != WSAGetOverlappedResultTable[dwTableIndex].dwFunction) {
            // Send the function request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSAGetOverlappedResultRequest), (char *) &WSAGetOverlappedResultRequest);

            // Wait for the function complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bComplete) {
                if ((0 != (WSASEND_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) || (0 != (WSASENDTO_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction))) {
                    FD_ZERO(&writefds);
                    FD_SET(sSocket, &writefds);
                    select(0, NULL, &writefds, NULL, NULL);
                }
            }
        }

        bReturnCode = TRUE;
        bSendReturnCode = TRUE;
        bRecvReturnCode = TRUE;

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & WSAGetOverlappedResultTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        // Get the current tick count
        dwFirstTime = GetTickCount();

        __try {
            // Call WSAGetOverlappedResult
            if (0 == WSAGetOverlappedResultTable[dwTableIndex].dwFunction) {
                bReturnCode = WSAGetOverlappedResult(sSocket, (WSAOVERLAPPED_NULL != WSAGetOverlappedResultTable[dwTableIndex].dwOverlapped) ? &WSASendOverlapped1 : NULL, (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bBytes) ? &dwSendBytes2 : NULL, WSAGetOverlappedResultTable[dwTableIndex].bWait, (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bFlags) ? &dwSendFlags2 : NULL);
            }
            else {
                if ((0 != (WSASEND_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) || (0 != (WSASENDTO_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction))) {
                    bSendReturnCode = WSAGetOverlappedResult(sSocket, (WSAOVERLAPPED_NULL != WSAGetOverlappedResultTable[dwTableIndex].dwOverlapped) ? &WSASendOverlapped1 : NULL, (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bBytes) ? &dwSendBytes2 : NULL, WSAGetOverlappedResultTable[dwTableIndex].bWait, (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bFlags) ? &dwSendFlags2 : NULL);
                }

                if ((0 != (WSARECV_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) || (0 != (WSARECVFROM_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction))) {
                    bRecvReturnCode = WSAGetOverlappedResult(sSocket, (WSAOVERLAPPED_NULL != WSAGetOverlappedResultTable[dwTableIndex].dwOverlapped) ? &WSARecvOverlapped1 : NULL, (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bBytes) ? &dwRecvBytes2 : NULL, WSAGetOverlappedResultTable[dwTableIndex].bWait, (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bFlags) ? &dwRecvFlags2 : NULL);
                }

                bReturnCode = bSendReturnCode & bRecvReturnCode;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "WSAGetOverlappedResult RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "WSAGetOverlappedResult caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            // Get the current tick count
            dwSecondTime = GetTickCount();

            if (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult did not RIP");
            }

            if ((FALSE == bReturnCode) && (FALSE == WSAGetOverlappedResultTable[dwTableIndex].bReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != WSAGetOverlappedResultTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult iLastError - EXPECTED: %u; RECEIVED: %u", WSAGetOverlappedResultTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "WSAGetOverlappedResult iLastError - OUT: %u", iLastError);
                }
            }
            else if (FALSE == bReturnCode) {
                xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult returned FALSE - ec = %u", WSAGetLastError());
            }
            else if (FALSE == WSAGetOverlappedResultTable[dwTableIndex].bReturnCode) {
                xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult returned TRUE");
            }
            else {
                if (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bComplete) {
                    if ((dwSecondTime - dwFirstTime) > SLEEP_ZERO_TIME) {
                        xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_ZERO_TIME, dwSecondTime - dwFirstTime);
                        bTestPassed = FALSE;
                    }
                }
                else {
                    if (((dwSecondTime - dwFirstTime) < SLEEP_LOW_TIME) || ((dwSecondTime - dwFirstTime) > SLEEP_HIGH_TIME)) {
                        xLog(hLog, XLL_FAIL, "WSAGetOverlappedResult nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME, dwSecondTime - dwFirstTime);
                        bTestPassed = FALSE;
                    }
                }

                // Check return values
                if ((0 != (WSASEND_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) || (0 != (WSASENDTO_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction))) {
                    if (0 != dwSendBytes1) {
                        xLog(hLog, XLL_FAIL, "dwSendBytes1 - EXPECTED: %d; RECEIVED: %d", 0, dwSendBytes1);
                        bTestPassed = FALSE;
                    }

                    if (WSASendBuf.len != dwSendBytes2) {
                        xLog(hLog, XLL_FAIL, "dwSendBytes2 - EXPECTED: %d; RECEIVED: %d", WSASendBuf.len, dwSendBytes2);
                        bTestPassed = FALSE;
                    }

                    if (0 != dwSendFlags2) {
                        xLog(hLog, XLL_FAIL, "dwSendFlags2 - EXPECTED: %d; RECEIVED: %d", 0, dwSendFlags2);
                        bTestPassed = FALSE;
                    }
                }

                if ((0 != (WSARECV_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) || (0 != (WSARECVFROM_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction))) {
                    if (0 != dwRecvBytes1) {
                        xLog(hLog, XLL_FAIL, "dwRecvBytes1 - EXPECTED: %d; RECEIVED: %d", 0, dwRecvBytes1);
                        bTestPassed = FALSE;
                    }

                    if (WSARecvBuf.len != dwRecvBytes2) {
                        xLog(hLog, XLL_FAIL, "dwRecvBytes2 - EXPECTED: %d; RECEIVED: %d", WSARecvBuf.len, dwRecvBytes2);
                        bTestPassed = FALSE;
                    }

                    if (0 != dwRecvFlags1) {
                        xLog(hLog, XLL_FAIL, "dwRecvFlags1 - EXPECTED: %d; RECEIVED: %d", 0, dwRecvFlags1);
                        bTestPassed = FALSE;
                    }

                    if (TRUE == WSAGetOverlappedResultTable[dwTableIndex].bPartial) {
                        if (MSG_PARTIAL != dwRecvFlags2) {
                            xLog(hLog, XLL_FAIL, "dwRecvFlags2 - EXPECTED: %d; RECEIVED: %d", MSG_PARTIAL, dwRecvFlags2);
                            bTestPassed = FALSE;
                        }
                    }
                    else {
                        if (0 != dwRecvFlags2) {
                            xLog(hLog, XLL_FAIL, "dwRecvFlags2 - EXPECTED: %d; RECEIVED: %d", 0, dwRecvFlags2);
                            bTestPassed = FALSE;
                        }
                    }

                    if (0 != (WSARECVFROM_FUNCTION & WSAGetOverlappedResultTable[dwTableIndex].dwFunction)) {
                        if (sizeof(remotename2) != namelen) {
                            xLog(hLog, XLL_FAIL, "namelen - EXPECTED: %d; RECEIVED: %d", sizeof(remotename2), namelen);
                            bTestPassed = FALSE;
                        }

                        if (AF_INET != remotename2.sin_family) {
                            xLog(hLog, XLL_FAIL, "sin_family - EXPECTED: %d; RECEIVED: %d", AF_INET, remotename2.sin_family);
                            bTestPassed = FALSE;
                        }

                        if (NetsyncInAddr != remotename2.sin_addr.s_addr) {
                            xLog(hLog, XLL_FAIL, "sin_addr - EXPECTED: %u; RECEIVED: %u", NetsyncInAddr, remotename2.sin_addr.s_addr);
                            bTestPassed = FALSE;
                        }

                        if (htons(CurrentPort) != remotename2.sin_port) {
                            xLog(hLog, XLL_FAIL, "sin_port - EXPECTED: %d; RECEIVED: %d", htons(CurrentPort), remotename2.sin_port);
                            bTestPassed = FALSE;
                        }

                        sprintf(SendBuffer10, "%05d%05d", 1, 1);
                        if (0 != strncmp(SendBuffer10, WSARecvBuf.buf, WSARecvBuf.len)) {
                            xLog(hLog, XLL_FAIL, "Received Unexpected Buffer");
                            bTestPassed = FALSE;
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "WSAGetOverlappedResult succeeded");
                }
            }
        }

        if (0 != WSAGetOverlappedResultTable[dwTableIndex].dwFunction) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(WSAGetOverlappedResultRequest), (char *) &WSAGetOverlappedResultRequest);
        }

        // Close the socket
        if (0 == (SOCKET_CLOSED & WSAGetOverlappedResultTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & WSAGetOverlappedResultTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & WSAGetOverlappedResultTable[dwTableIndex].dwSocket))) {
                shutdown(sSocket, SD_BOTH);
                closesocket(sSocket);
            }
        }

        // Close the event
        CloseHandle(hRecvEvent2);
        CloseHandle(hRecvEvent1);
        CloseHandle(hSendEvent2);
        CloseHandle(hSendEvent1);

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
WSAGetOverlappedResultTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests WSAGetOverlappedResult - Server side

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
    u_long                           FromInAddr;
    // dwMessageType is the type of received message
    DWORD                            dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD                            dwMessageSize;
    // pMessage is a pointer to the received message
    char                             *pMessage;
    // WSAGetOverlappedResultRequest is the request
    WSAGETOVERLAPPEDRESULT_REQUEST   WSAGetOverlappedResultRequest;
    // WSAGetOverlappedResultComplete is the result
    WSAGETOVERLAPPEDRESULT_COMPLETE  WSAGetOverlappedResultComplete;

    // sSocket is the socket descriptor
    SOCKET                           sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                           nsSocket;

    // dwBufferSize is the send/receive buffer size
    DWORD                            dwBufferSize = 1;
    // bNagle indicates if Nagle is enabled
    BOOL                             bNagle = FALSE;
    // nQueueLen is the size of the queue
    int                              nQueueLen;

    // localname is the local address
    SOCKADDR_IN                      localname;
    // remotename is the remote address
    SOCKADDR_IN                      remotename;

    // SendBuffer10 is the send buffer
    char                             SendBuffer10[BUFFER_10_LEN + 1];
    // RecvBufferLarge is the large recv buffer
    char                             RecvBufferLarge[BUFFER_LARGE_LEN + 1];
    // dwFillBuffer is a counter to fill the buffers
    DWORD                            dwFillBuffer;
    // nBytes is the number of bytes sent/received
    int                              nBytes;
    // nSendCount is a counter to enumerate each send
    int                              nSendCount;



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
        CopyMemory(&WSAGetOverlappedResultRequest, pMessage, sizeof(WSAGetOverlappedResultRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, WSAGetOverlappedResultRequest.nSocketType, 0);

        if (SOCK_STREAM == WSAGetOverlappedResultRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Set the buffer size
        setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));
        setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwBufferSize, sizeof(dwBufferSize));

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(WSAGetOverlappedResultRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if (SOCK_STREAM == WSAGetOverlappedResultRequest.nSocketType) {
            // Place the socket in listening mode
            listen(sSocket, SOMAXCONN);
        }

        // Send the complete
        WSAGetOverlappedResultComplete.dwMessageId = WSAGETOVERLAPPEDRESULT_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSAGetOverlappedResultComplete), (char *) &WSAGetOverlappedResultComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if (SOCK_STREAM == WSAGetOverlappedResultRequest.nSocketType) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(WSAGetOverlappedResultRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        WSAGetOverlappedResultComplete.dwMessageId = WSAGETOVERLAPPEDRESULT_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSAGetOverlappedResultComplete), (char *) &WSAGetOverlappedResultComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        nQueueLen = ((PWSAGETOVERLAPPEDRESULT_REQUEST) pMessage)->nQueueLen;
        NetsyncFreeMessage(pMessage);

        if (FALSE == WSAGetOverlappedResultRequest.bComplete) {
            // Send the complete
            WSAGetOverlappedResultComplete.dwMessageId = WSAGETOVERLAPPEDRESULT_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSAGetOverlappedResultComplete), (char *) &WSAGetOverlappedResultComplete);

            // Sleep
            Sleep(SLEEP_MIDLOW_TIME);
        }

        // Complete the pending overlapped operation
        if (TRUE == WSAGetOverlappedResultRequest.bServerRecv) {
            ZeroMemory(&RecvBufferLarge, sizeof(RecvBufferLarge));
            for (nSendCount = 0; nSendCount < nQueueLen; nSendCount++) {
                recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, BUFFER_TCPSEGMENT_LEN, 0);
            }
        }

        if (TRUE == WSAGetOverlappedResultRequest.bServerSend) {
            // Call send
            sprintf(SendBuffer10, "%05d%05d", 1, 1);
            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBuffer10, 10, 0);
        }

        if (TRUE == WSAGetOverlappedResultRequest.bComplete) {
            // Send the complete
            WSAGetOverlappedResultComplete.dwMessageId = WSAGETOVERLAPPEDRESULT_COMPLETE_MSG;
            NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(WSAGetOverlappedResultComplete), (char *) &WSAGetOverlappedResultComplete);
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
