/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  getsockopt.c

Abstract:

  This modules tests getsockopt

Author:

  Steven Kehrli (steveke) 21-Jun-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct GETSOCKOPT_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL     bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    DWORD    dwSocket;                                // dwSocket indicates the socket to be created
    int      level;                                   // level is the socket option level
    int      optname;                                 // optname is the socket option
    BOOL     boptval;                                 // boptval specifies if optval is valid
    BOOL     boptlen;                                 // boptlen specifies if optlen is valid
    int      optlen;                                  // optlen is the size of the socket option
    int      iReturnCode;                             // iReturnCode is the return code of getsockopt
    int      iLastError;                              // iLastError is the error code if the operation failed
    BOOL     bRIP;                                    // Specifies a RIP test case
} GETSOCKOPT_TABLE, *PGETSOCKOPT_TABLE;

static GETSOCKOPT_TABLE getsockoptTable[] =
{
    { "30.1 Not Initialized",          FALSE, SOCKET_INVALID_SOCKET,      0,           0,                   TRUE,  TRUE,  0,                     SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "30.2 s = INT_MIN",              TRUE,  SOCKET_INT_MIN,             0,           0,                   TRUE,  TRUE,  0,                     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "30.3 s = -1",                   TRUE,  SOCKET_NEG_ONE,             0,           0,                   TRUE,  TRUE,  0,                     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "30.4 s = 0",                    TRUE,  SOCKET_ZERO,                0,           0,                   TRUE,  TRUE,  0,                     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "30.5 s = INT_MAX",              TRUE,  SOCKET_INT_MAX,             0,           0,                   TRUE,  TRUE,  0,                     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "30.6 s = INVALID_SOCKET",       TRUE,  SOCKET_INVALID_SOCKET,      0,           0,                   TRUE,  TRUE,  0,                     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "30.7 TCP SO_ACCEPTCONN",        TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_ACCEPTCONN,       TRUE,  TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "30.8 TCP SO_BROADCAST",         TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_BROADCAST,        TRUE,  TRUE,  sizeof(BOOL),          SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "30.9 TCP SO_REUSEADDR",         TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_REUSEADDR,        TRUE,  TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "30.10 TCP SO_EXCLUSIVEADDRUSE", TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_EXCLUSIVEADDRUSE, TRUE,  TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "30.11 TCP SO_LINGER",           TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_LINGER,           TRUE,  TRUE,  sizeof(struct linger), 0,            0,                 FALSE },
    { "30.12 TCP SO_DONTLINGER",       TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_DONTLINGER,       TRUE,  TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "30.13 TCP SO_SNDBUF",           TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_SNDBUF,           TRUE,  TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "30.14 TCP SO_RCVBUF",           TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_RCVBUF,           TRUE,  TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "30.15 TCP SO_SNDTIMEO",         TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_SNDTIMEO,         TRUE,  TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "30.16 TCP SO_RCVTIMEO",         TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_RCVTIMEO,         TRUE,  TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "30.17 TCP SO_TYPE",             TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_TYPE,             TRUE,  TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "30.18 TCP TCP_NODELAY",         TRUE,  SOCKET_TCP,                 IPPROTO_TCP, TCP_NODELAY,         TRUE,  TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "30.19 UDP SO_ACCEPTCONN",       TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_ACCEPTCONN,       TRUE,  TRUE,  sizeof(BOOL),          SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "30.20 UDP SO_BROADCAST",        TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_BROADCAST,        TRUE,  TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "30.21 UDP SO_REUSEADDR",        TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_REUSEADDR,        TRUE,  TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "30.22 UDP SO_EXCLUSIVEADDRUSE", TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_EXCLUSIVEADDRUSE, TRUE,  TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "30.23 UDP SO_LINGER",           TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_LINGER,           TRUE,  TRUE,  sizeof(struct linger), SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "30.24 UDP SO_DONTLINGER",       TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_DONTLINGER,       TRUE,  TRUE,  sizeof(BOOL),          SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "30.25 UDP SO_SNDBUF",           TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_SNDBUF,           TRUE,  TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "30.26 UDP SO_RCVBUF",           TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_RCVBUF,           TRUE,  TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "30.27 UDP SO_SNDTIMEO",         TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_SNDTIMEO,         TRUE,  TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "30.28 UDP SO_RCVTIMEO",         TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_RCVTIMEO,         TRUE,  TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "30.29 UDP SO_TYPE",             TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_TYPE,             TRUE,  TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "30.30 UDP TCP_NODELAY",         TRUE,  SOCKET_UDP,                 IPPROTO_TCP, TCP_NODELAY,         TRUE,  TRUE,  sizeof(BOOL),          SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "30.31 TCP 0 level",             TRUE,  SOCKET_TCP,                 0,           SO_TYPE,             TRUE,  TRUE,  sizeof(int),           SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "30.32 TCP 0 optname",           TRUE,  SOCKET_TCP,                 SOL_SOCKET,  0,                   TRUE,  TRUE,  sizeof(int),           SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "30.33 TCP NULL optval",         TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_TYPE,             FALSE, TRUE,  sizeof(int),           0,            0,                 TRUE  },
    { "30.34 TCP NULL optlen",         TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_TYPE,             TRUE,  FALSE, sizeof(int),           0,            0,                 TRUE  },
    { "30.35 TCP 0 optlen",            TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_TYPE,             TRUE,  FALSE, 0,                     0,            0,                 TRUE  },
    { "30.36 TCP SO_LINGER optlen",    TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_LINGER,           TRUE,  TRUE,  sizeof(u_short),       0,            0,                 TRUE  },
    { "30.37 UDP 0 level",             TRUE,  SOCKET_UDP,                 0,           SO_TYPE,             TRUE,  TRUE,  sizeof(int),           SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "30.38 UDP 0 optname",           TRUE,  SOCKET_UDP,                 SOL_SOCKET,  0,                   TRUE,  TRUE,  sizeof(int),           SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "30.39 UDP NULL optval",         TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_TYPE,             FALSE, TRUE,  sizeof(int),           0,            0,                 TRUE  },
    { "30.40 UDP NULL optlen",         TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_TYPE,             TRUE,  FALSE, sizeof(int),           0,            0,                 TRUE  },
    { "30.41 UDP 0 optlen",            TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_TYPE,             TRUE,  FALSE, 0,                     0,            0,                 TRUE  },
    { "30.42 TCP Closed",              TRUE,  SOCKET_TCP | SOCKET_CLOSED, SOL_SOCKET,  SO_TYPE,             TRUE,  TRUE,  sizeof(int),           SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "30.43 UDP Closed",              TRUE,  SOCKET_UDP | SOCKET_CLOSED, SOL_SOCKET,  SO_TYPE,             TRUE,  TRUE,  sizeof(int),           SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "30.44 Not Initialized",         FALSE, SOCKET_INVALID_SOCKET,      0,           0,                   TRUE,  TRUE,  0,                     SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define getsockoptTableCount (sizeof(getsockoptTable) / sizeof(GETSOCKOPT_TABLE))



VOID
getsockoptTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests getsockopt - Client side

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  WinsockVersion - requested version of Winsock
  bRIPs - specifies RIP testing

------------------------------------------------------------------------------*/
{
    // lpszCaseTest is a pointer to the list of cases to test
    LPSTR          lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR          lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD          dwTableIndex;

    // bWinsockInitialized indicates if Winsock is initialized
    BOOL           bWinsockInitialized = FALSE;
    // WSAData is the details of the Winsock implementation
    WSADATA        WSAData;

    // sSocket is the socket descriptor
    SOCKET         sSocket;

    // localname is the local address
    SOCKADDR_IN    localname;

    // optval is the pointer to the socket option value
    char           *optval;
    int            defaultoptval;
    // optlen is the size of the socket option
    int            optlen;

    // bAcceptConn is the value for SO_ACCEPTCONN
    BOOL           bAcceptConn;
    // bBroadcast is the value for SO_BROADCAST
    BOOL           bBroadcast;
    // bReuseAddr is the value for SO_REUSEADDR
    BOOL           bReuseAddr;
    // bExclusiveAddrUse is the value for SO_EXCLUSIVEADDRUSE
    BOOL           bExclusiveAddrUse;
    // linger is the value for SO_LINGER
    struct linger  linger;
    // bDontLinger is the value for SO_DONTLINGER
    BOOL           bDontLinger;
    // nSndBuf is the value for SO_SNDBUF
    int            nSndBuf;
    // nRcvBuf is the value for SO_RCVBUF
    int            nRcvBuf;
    // nSndTimeO is the value for SO_SNDTIMEO
    int            nSndTimeO;
    // nRcvTimeO is the value for SO_RCVTIMEO
    int            nRcvTimeO;
    // nType is the value for SO_TYPE
    int            nType;
    // bNoDelay is the value for TCP_NODELAY
    BOOL           bNoDelay;

    // bException indicates if an exception occurred
    BOOL           bException;
    // iReturnCode is the return code of the operation
    int            iReturnCode;
    // iLastError is the error code if the operation failed
    int            iLastError;
    // bTestPassed indicates if the test passed
    BOOL           bTestPassed;

    // szFunctionName is the function name
    CHAR           szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    sprintf(szFunctionName, "getsockopt v%04x", WinsockVersion);
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_getsockopt+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_getsockopt-");

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < getsockoptTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, getsockoptTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, getsockoptTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != getsockoptTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, getsockoptTable[dwTableIndex].szVariationName);

        // Check the state of Winsock
        if (bWinsockInitialized != getsockoptTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == getsockoptTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = getsockoptTable[dwTableIndex].bWinsockInitialized;
        }

        // Create the socket
        sSocket = INVALID_SOCKET;
        if (SOCKET_INT_MIN == getsockoptTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == getsockoptTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == getsockoptTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == getsockoptTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == getsockoptTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & getsockoptTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & getsockoptTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Bind the socket
        if ((0 != (SOCKET_TCP & getsockoptTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & getsockoptTable[dwTableIndex].dwSocket))) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Setup optval
        optval = (char *) &defaultoptval;
        optlen = getsockoptTable[dwTableIndex].optlen;
        switch (getsockoptTable[dwTableIndex].optname) {
            case SO_ACCEPTCONN:
                optval = (char *) &bAcceptConn;
                break;

            case SO_BROADCAST:
                optval = (char *) &bBroadcast;
                break;

            case SO_REUSEADDR:
                optval = (char *) &bReuseAddr;
                break;

            case SO_EXCLUSIVEADDRUSE:
                optval = (char *) &bExclusiveAddrUse;
                break;

            case SO_LINGER:
                optval = (char *) &linger;
                break;

            case SO_DONTLINGER:
                optval = (char *) &bDontLinger;
                break;

            case SO_SNDBUF:
                optval = (char *) &nSndBuf;
                break;

            case SO_RCVBUF:
                optval = (char *) &nRcvBuf;
                break;

            case SO_SNDTIMEO:
                optval = (char *) &nSndTimeO;
                break;

            case SO_RCVTIMEO:
                optval = (char *) &nRcvTimeO;
                break;

            case SO_TYPE:
                optval = (char *) &nType;
                break;

            case TCP_NODELAY:
                optval = (char *) &bNoDelay;
                break;
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & getsockoptTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        __try {
            // Call getsockopt
            iReturnCode = getsockopt(sSocket, getsockoptTable[dwTableIndex].level, getsockoptTable[dwTableIndex].optname, (TRUE == getsockoptTable[dwTableIndex].boptval) ? optval : NULL, (TRUE == getsockoptTable[dwTableIndex].boptlen) ? &optlen : NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            xLog(hLog, XLL_EXCEPTION, "getsockopt caused an exception - ec = 0x%08x", GetExceptionCode());
        }

        if (FALSE == bException) {
            if (TRUE == getsockoptTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "getsockopt did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == getsockoptTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != getsockoptTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "getsockopt iLastError - EXPECTED: %u; RECEIVED: %u", getsockoptTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "getsockopt iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                // Get the last error code
                iLastError = WSAGetLastError();

                xLog(hLog, XLL_FAIL, "getsockopt returned SOCKET_ERROR - ec = %u", iLastError);
            }
            else if (SOCKET_ERROR == getsockoptTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "getsockopt returned non-SOCKET_ERROR");
            }
            else {
                // Check optlen
                if (getsockoptTable[dwTableIndex].optlen != optlen) {
                    xLog(hLog, XLL_FAIL, "optlen - EXPECTED: %d; RECEIVED: %d", getsockoptTable[dwTableIndex].optlen, optlen);
                    bTestPassed = FALSE;
                }

                // Check optval
                switch (getsockoptTable[dwTableIndex].optname) {
                    case SO_ACCEPTCONN:
                        if (FALSE != bAcceptConn) {
                            xLog(hLog, XLL_FAIL, "optval1 - EXPECTED: %d; RECEIVED: %d", FALSE, bAcceptConn);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_BROADCAST:
                        if (FALSE != bBroadcast) {
                            xLog(hLog, XLL_FAIL, "optval1 - EXPECTED: %d; RECEIVED: %d", FALSE, bBroadcast);
                            bTestPassed = FALSE;
                        }

                        bBroadcast = TRUE;

                        break;

                    case SO_REUSEADDR:
                        if (FALSE != bReuseAddr) {
                            xLog(hLog, XLL_FAIL, "optval1 - EXPECTED: %d; RECEIVED: %d", FALSE, bReuseAddr);
                            bTestPassed = FALSE;
                        }

                        bReuseAddr = TRUE;

                        break;

                    case SO_EXCLUSIVEADDRUSE:
                        if (FALSE != bExclusiveAddrUse) {
                            xLog(hLog, XLL_FAIL, "optval1 - EXPECTED: %d; RECEIVED: %d", FALSE, bExclusiveAddrUse);
                            bTestPassed = FALSE;
                        }

                        bExclusiveAddrUse = TRUE;

                        break;

                    case SO_LINGER:
                        if (0 != linger.l_onoff) {
                            xLog(hLog, XLL_FAIL, "l_onoff1 - EXPECTED: %d; RECEIVED: %d", 0, linger.l_onoff);
                            bTestPassed = FALSE;
                        }

                        if (120 != linger.l_linger) {
                            xLog(hLog, XLL_FAIL, "l_linger1 - EXPECTED: %d; RECEIVED: %d", 120, linger.l_linger);
                            bTestPassed = FALSE;
                        }

                        linger.l_onoff = 1;
                        linger.l_linger = 5;

                        break;

                    case SO_DONTLINGER:
                        if (TRUE != bDontLinger) {
                            xLog(hLog, XLL_FAIL, "optval1 - EXPECTED: %d; RECEIVED: %d", TRUE, bDontLinger);
                            bTestPassed = FALSE;
                        }

                        bDontLinger = FALSE;

                        break;

                    case SO_SNDBUF:
                        if (16384 != nSndBuf) {
                            xLog(hLog, XLL_FAIL, "optval1 - EXPECTED: %d; RECEIVED: %d", 16384, nSndBuf);
                            bTestPassed = FALSE;
                        }

                        nSndBuf = 1;

                        break;

                    case SO_RCVBUF:
                        if (16384 != nRcvBuf) {
                            xLog(hLog, XLL_FAIL, "optval1 - EXPECTED: %d; RECEIVED: %d", 16384, nRcvBuf);
                            bTestPassed = FALSE;
                        }

                        nRcvBuf = 1;

                        break;

                    case SO_SNDTIMEO:
                        if (0 != nSndTimeO) {
                            xLog(hLog, XLL_FAIL, "optval1 - EXPECTED: %d; RECEIVED: %d", 0, nSndTimeO);
                            bTestPassed = FALSE;
                        }

                        nSndTimeO = 5;

                        break;

                    case SO_RCVTIMEO:
                        if (0 != nRcvTimeO) {
                            xLog(hLog, XLL_FAIL, "optval1 - EXPECTED: %d; RECEIVED: %d", 0, nRcvTimeO);
                            bTestPassed = FALSE;
                        }

                        nRcvTimeO = 5;

                        break;

                    case SO_TYPE:
                        if (0 != (SOCKET_TCP & getsockoptTable[dwTableIndex].dwSocket)) {
                            if (SOCK_STREAM != nType) {
                                xLog(hLog, XLL_FAIL, "optval1 - EXPECTED: %d; RECEIVED: %d", SOCK_STREAM, nType);
                                bTestPassed = FALSE;
                            }
                        }
                        else {
                            if (SOCK_DGRAM != nType) {
                                xLog(hLog, XLL_FAIL, "optval1 - EXPECTED: %d; RECEIVED: %d", SOCK_DGRAM, nType);
                                bTestPassed = FALSE;
                            }
                        }

                        break;

                    case TCP_NODELAY:
                        if (FALSE != bNoDelay) {
                            xLog(hLog, XLL_FAIL, "optval1 - EXPECTED: %d; RECEIVED: %d", FALSE, bNoDelay);
                            bTestPassed = FALSE;
                        }

                        bNoDelay = TRUE;

                        break;
                }

                // Call setsockopt
                switch (getsockoptTable[dwTableIndex].optname) {
                    case SO_ACCEPTCONN:
                        listen(sSocket, SOMAXCONN);

                        break;

                    case SO_BROADCAST:
                    case SO_REUSEADDR:
                    case SO_EXCLUSIVEADDRUSE:
                    case SO_LINGER:
                    case SO_DONTLINGER:
                    case SO_SNDBUF:
                    case SO_RCVBUF:
                    case SO_SNDTIMEO:
                    case SO_RCVTIMEO:
                    case TCP_NODELAY:
                        setsockopt(sSocket, getsockoptTable[dwTableIndex].level, getsockoptTable[dwTableIndex].optname, optval, optlen);

                        break;
                }

                // Call getsockopt
                getsockopt(sSocket, getsockoptTable[dwTableIndex].level, getsockoptTable[dwTableIndex].optname, optval, &optlen);

                // Check optlen
                if (getsockoptTable[dwTableIndex].optlen != optlen) {
                    xLog(hLog, XLL_FAIL, "optlen - EXPECTED: %d; RECEIVED: %d", getsockoptTable[dwTableIndex].optlen, optlen);
                    bTestPassed = FALSE;
                }

                // Check optval
                switch (getsockoptTable[dwTableIndex].optname) {
                    case SO_ACCEPTCONN:
                        if (TRUE != bAcceptConn) {
                            xLog(hLog, XLL_FAIL, "optval2 - EXPECTED: %d; RECEIVED: %d", TRUE, bAcceptConn);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_BROADCAST:
                        if (TRUE != bBroadcast) {
                            xLog(hLog, XLL_FAIL, "optval2 - EXPECTED: %d; RECEIVED: %d", TRUE, bBroadcast);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_REUSEADDR:
                        if (TRUE != bReuseAddr) {
                            xLog(hLog, XLL_FAIL, "optval2 - EXPECTED: %d; RECEIVED: %d", TRUE, bReuseAddr);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_EXCLUSIVEADDRUSE:
                        if (TRUE != bExclusiveAddrUse) {
                            xLog(hLog, XLL_FAIL, "optval2 - EXPECTED: %d; RECEIVED: %d", TRUE, bExclusiveAddrUse);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_LINGER:
                        if (1 != linger.l_onoff) {
                            xLog(hLog, XLL_FAIL, "l_onoff2 - EXPECTED: %d; RECEIVED: %d", 1, linger.l_onoff);
                            bTestPassed = FALSE;
                        }

                        if (5 != linger.l_linger) {
                            xLog(hLog, XLL_FAIL, "l_linger2 - EXPECTED: %d; RECEIVED: %d", 5, linger.l_linger);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_DONTLINGER:
                        if (FALSE != bDontLinger) {
                            xLog(hLog, XLL_FAIL, "optval2 - EXPECTED: %d; RECEIVED: %d", FALSE, bDontLinger);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_SNDBUF:
                        if (1 != nSndBuf) {
                            xLog(hLog, XLL_FAIL, "optval2 - EXPECTED: %d; RECEIVED: %d", 1, nSndBuf);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_RCVBUF:
                        if (1 != nRcvBuf) {
                            xLog(hLog, XLL_FAIL, "optval2 - EXPECTED: %d; RECEIVED: %d", 1, nRcvBuf);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_SNDTIMEO:
                        if (5 != nSndTimeO) {
                            xLog(hLog, XLL_FAIL, "optval2 - EXPECTED: %d; RECEIVED: %d", 5, nSndTimeO);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_RCVTIMEO:
                        if (5 != nRcvTimeO) {
                            xLog(hLog, XLL_FAIL, "optval2 - EXPECTED: %d; RECEIVED: %d", 5, nRcvTimeO);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_TYPE:
                        if (0 != (SOCKET_TCP & getsockoptTable[dwTableIndex].dwSocket)) {
                            if (SOCK_STREAM != nType) {
                                xLog(hLog, XLL_FAIL, "optval2 - EXPECTED: %d; RECEIVED: %d", SOCK_STREAM, nType);
                                bTestPassed = FALSE;
                            }
                        }
                        else {
                            if (SOCK_DGRAM != nType) {
                                xLog(hLog, XLL_FAIL, "optval2 - EXPECTED: %d; RECEIVED: %d", SOCK_DGRAM, nType);
                                bTestPassed = FALSE;
                            }
                        }

                        break;

                    case TCP_NODELAY:
                        if (TRUE != bNoDelay) {
                            xLog(hLog, XLL_FAIL, "optval2 - EXPECTED: %d; RECEIVED: %d", TRUE, bNoDelay);
                            bTestPassed = FALSE;
                        }

                        break;
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "getsockopt succeeded");
                }
            }
        }

        if (0 == (SOCKET_CLOSED & getsockoptTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & getsockoptTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & getsockoptTable[dwTableIndex].dwSocket))) {
                shutdown(sSocket, SD_BOTH);
                closesocket(sSocket);
            }
        }

        // End the variation
        xEndVariation(hLog);
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

#endif
