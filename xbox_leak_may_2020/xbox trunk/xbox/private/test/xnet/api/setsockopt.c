/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  setsockopt.c

Abstract:

  This modules tests setsockopt

Author:

  Steven Kehrli (steveke) 21-Jun-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct SETSOCKOPT_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL     bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    DWORD    dwSocket;                                // dwSocket indicates the socket to be created
    int      level;                                   // level is the socket option level
    int      optname;                                 // optname is the socket option
    BOOL     boptval;                                 // boptval specifies if optval is valid
    int      optlen;                                  // optlen is the size of the socket option
    int      iReturnCode;                             // iReturnCode is the return code of setsockopt
    int      iLastError;                              // iLastError is the error code if the operation failed
    BOOL     bRIP;                                    // Specifies a RIP test case
} SETSOCKOPT_TABLE, *PSETSOCKOPT_TABLE;

static SETSOCKOPT_TABLE setsockoptTable[] =
{
    { "31.1 Not Initialized",          FALSE, SOCKET_INVALID_SOCKET,      0,           0,                   TRUE,  0,                     SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "31.2 s = INT_MIN",              TRUE,  SOCKET_INT_MIN,             0,           0,                   TRUE,  0,                     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "31.3 s = -1",                   TRUE,  SOCKET_NEG_ONE,             0,           0,                   TRUE,  0,                     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "31.4 s = 0",                    TRUE,  SOCKET_ZERO,                0,           0,                   TRUE,  0,                     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "31.5 s = INT_MAX",              TRUE,  SOCKET_INT_MAX,             0,           0,                   TRUE,  0,                     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "31.6 s = INVALID_SOCKET",       TRUE,  SOCKET_INVALID_SOCKET,      0,           0,                   TRUE,  0,                     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "31.7 TCP SO_ACCEPTCONN",        TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_ACCEPTCONN,       TRUE,  sizeof(BOOL),          SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "31.8 TCP SO_BROADCAST",         TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_BROADCAST,        TRUE,  sizeof(BOOL),          SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "31.9 TCP SO_REUSEADDR",         TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_REUSEADDR,        TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "31.10 TCP SO_EXCLUSIVEADDRUSE", TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_EXCLUSIVEADDRUSE, TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "31.11 TCP SO_LINGER",           TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_LINGER,           TRUE,  sizeof(struct linger), 0,            0,                 FALSE },
    { "31.12 TCP SO_DONTLINGER",       TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_DONTLINGER,       TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "31.13 TCP SO_SNDBUF",           TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_SNDBUF,           TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "31.14 TCP SO_RCVBUF",           TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_RCVBUF,           TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "31.15 TCP SO_SNDTIMEO",         TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_SNDTIMEO,         TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "31.16 TCP SO_RCVTIMEO",         TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_RCVTIMEO,         TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "31.17 TCP SO_TYPE",             TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_TYPE,             TRUE,  sizeof(int),           SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "31.18 TCP TCP_NODELAY",         TRUE,  SOCKET_TCP,                 IPPROTO_TCP, TCP_NODELAY,         TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "31.19 UDP SO_ACCEPTCONN",       TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_ACCEPTCONN,       TRUE,  sizeof(BOOL),          SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "31.20 UDP SO_BROADCAST",        TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_BROADCAST,        TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "31.21 UDP SO_REUSEADDR",        TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_REUSEADDR,        TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "31.22 UDP SO_EXCLUSIVEADDRUSE", TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_EXCLUSIVEADDRUSE, TRUE,  sizeof(BOOL),          0,            0,                 FALSE },
    { "31.23 UDP SO_LINGER",           TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_LINGER,           TRUE,  sizeof(struct linger), SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "31.24 UDP SO_DONTLINGER",       TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_DONTLINGER,       TRUE,  sizeof(BOOL),          SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "31.25 UDP SO_SNDBUF",           TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_SNDBUF,           TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "31.26 UDP SO_RCVBUF",           TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_RCVBUF,           TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "31.27 UDP SO_SNDTIMEO",         TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_SNDTIMEO,         TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "31.28 UDP SO_RCVTIMEO",         TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_RCVTIMEO,         TRUE,  sizeof(int),           0,            0,                 FALSE },
    { "31.29 UDP SO_TYPE",             TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_TYPE,             TRUE,  sizeof(int),           SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "31.30 UDP TCP_NODELAY",         TRUE,  SOCKET_UDP,                 IPPROTO_TCP, TCP_NODELAY,         TRUE,  sizeof(BOOL),          SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "31.31 TCP 0 level",             TRUE,  SOCKET_TCP,                 0,           SO_TYPE,             TRUE,  sizeof(int),           SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "31.32 TCP 0 optname",           TRUE,  SOCKET_TCP,                 SOL_SOCKET,  0,                   TRUE,  sizeof(int),           SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "31.33 TCP NULL optval",         TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_TYPE,             FALSE, sizeof(int),           0,            0,                 TRUE  },
    { "31.34 TCP 0 optlen",            TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_TYPE,             TRUE,  0,                     0,            0,                 TRUE  },
    { "31.35 TCP SO_LINGER optlen",    TRUE,  SOCKET_TCP,                 SOL_SOCKET,  SO_LINGER,           TRUE,  sizeof(u_short),       0,            0,                 TRUE  },
    { "31.36 UDP 0 level",             TRUE,  SOCKET_UDP,                 0,           SO_TYPE,             TRUE,  sizeof(int),           SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "31.37 UDP 0 optname",           TRUE,  SOCKET_UDP,                 SOL_SOCKET,  0,                   TRUE,  sizeof(int),           SOCKET_ERROR, WSAENOPROTOOPT,    FALSE },
    { "31.38 UDP NULL optval",         TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_TYPE,             FALSE, sizeof(int),           0,            0,                 TRUE  },
    { "31.39 UDP 0 optlen",            TRUE,  SOCKET_UDP,                 SOL_SOCKET,  SO_TYPE,             TRUE,  0,                     0,            0,                 TRUE  },
    { "31.40 TCP Closed",              TRUE,  SOCKET_TCP | SOCKET_CLOSED, SOL_SOCKET,  SO_TYPE,             TRUE,  sizeof(int),           SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "31.41 UDP Closed",              TRUE,  SOCKET_UDP | SOCKET_CLOSED, SOL_SOCKET,  SO_TYPE,             TRUE,  sizeof(int),           SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "31.42 Not Initialized",         FALSE, SOCKET_INVALID_SOCKET,      0,           0,                   TRUE,  0,                     SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define setsockoptTableCount (sizeof(setsockoptTable) / sizeof(SETSOCKOPT_TABLE))



VOID
setsockoptTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests setsockopt - Client side

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  WinsockVersion - requested version of Winsock
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

    // sSocket is the socket descriptor
    SOCKET                 sSocket;

    // localname is the local address
    SOCKADDR_IN            localname;

    // optval is the pointer to the socket option value
    char                   *optval;
    int                    defaultoptval;
    // optlen is the size of the socket option
    int                    optlen;

    // bAcceptConn is the value for SO_ACCEPTCONN
    BOOL                   bAcceptConn;
    // bBroadcast is the value for SO_BROADCAST
    BOOL                   bBroadcast;
    // bReuseAddr is the value for SO_REUSEADDR
    BOOL                   bReuseAddr;
    // bExclusiveAddrUse is the value for SO_EXCLUSIVEADDRUSE
    BOOL                   bExclusiveAddrUse;
    // linger is the value for SO_LINGER
    struct linger          linger;
    // bDontLinger is the value for SO_DONTLINGER
    BOOL                   bDontLinger;
    // nSndBuf is the value for SO_SNDBUF
    int                    nSndBuf;
    // nRcvBuf is the value for SO_RCVBUF
    int                    nRcvBuf;
    // nSndTimeO is the value for SO_SNDTIMEO
    int                    nSndTimeO;
    // nRcvTimeO is the value for SO_RCVTIMEO
    int                    nRcvTimeO;
    // nType is the value for SO_TYPE
    int                    nType;
    // bNoDelay is the value for TCP_NODELAY
    BOOL                   bNoDelay;

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
    sprintf(szFunctionName, "setsockopt v%04x", WinsockVersion);
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_setsockopt+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_setsockopt-");

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < setsockoptTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, setsockoptTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, setsockoptTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != setsockoptTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, setsockoptTable[dwTableIndex].szVariationName);

        // Check the state of Winsock
        if (bWinsockInitialized != setsockoptTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == setsockoptTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = setsockoptTable[dwTableIndex].bWinsockInitialized;
        }

        // Create the socket
        sSocket = INVALID_SOCKET;
        if (SOCKET_INT_MIN == setsockoptTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == setsockoptTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == setsockoptTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == setsockoptTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == setsockoptTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & setsockoptTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & setsockoptTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Bind the socket
        if ((0 != (SOCKET_TCP & setsockoptTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & setsockoptTable[dwTableIndex].dwSocket))) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Setup optval
        optval = (char *) &defaultoptval;
        optlen = setsockoptTable[dwTableIndex].optlen;
        switch (setsockoptTable[dwTableIndex].optname) {
            case SO_ACCEPTCONN:
                bAcceptConn = TRUE;
                optval = (char *) &bAcceptConn;
                break;

            case SO_BROADCAST:
                bBroadcast = TRUE;
                optval = (char *) &bBroadcast;
                break;

            case SO_REUSEADDR:
                bReuseAddr = TRUE;
                optval = (char *) &bReuseAddr;
                break;

            case SO_EXCLUSIVEADDRUSE:
                bExclusiveAddrUse = TRUE;
                optval = (char *) &bExclusiveAddrUse;
                break;

            case SO_LINGER:
                linger.l_onoff = 0;
                linger.l_linger = 5;
                optval = (char *) &linger;
                break;

            case SO_DONTLINGER:
                bDontLinger = FALSE;
                optval = (char *) &bDontLinger;
                break;

            case SO_SNDBUF:
                nSndBuf = 1;
                optval = (char *) &nSndBuf;
                break;

            case SO_RCVBUF:
                nRcvBuf = 1;
                optval = (char *) &nRcvBuf;
                break;

            case SO_SNDTIMEO:
                nSndTimeO = 5;
                optval = (char *) &nSndTimeO;
                break;

            case SO_RCVTIMEO:
                nRcvTimeO = 5;
                optval = (char *) &nRcvTimeO;
                break;

            case SO_TYPE:
                nType = 0;
                optval = (char *) &nType;
                break;

            case TCP_NODELAY:
                bNoDelay = TRUE;
                optval = (char *) &bNoDelay;
                break;
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & setsockoptTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        __try {
            // Call setsockopt
            iReturnCode = setsockopt(sSocket, setsockoptTable[dwTableIndex].level, setsockoptTable[dwTableIndex].optname, (TRUE == setsockoptTable[dwTableIndex].boptval) ? optval : NULL, setsockoptTable[dwTableIndex].optlen);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            xLog(hLog, XLL_EXCEPTION, "setsockopt caused an exception - ec = 0x%08x", GetExceptionCode());
        }

        if (FALSE == bException) {
            if (TRUE == setsockoptTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "setsockopt did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == setsockoptTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != setsockoptTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "setsockopt iLastError - EXPECTED: %u; RECEIVED: %u", setsockoptTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "setsockopt iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                // Get the last error code
                iLastError = WSAGetLastError();

                xLog(hLog, XLL_FAIL, "setsockopt returned SOCKET_ERROR - ec = %u", iLastError);
            }
            else if (SOCKET_ERROR == setsockoptTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "setsockopt returned non-SOCKET_ERROR");
            }
            else {
                // Call getsockopt
                getsockopt(sSocket, setsockoptTable[dwTableIndex].level, setsockoptTable[dwTableIndex].optname, optval, &optlen);

                // Check optlen
                if (setsockoptTable[dwTableIndex].optlen != optlen) {
                    xLog(hLog, XLL_FAIL, "optlen - EXPECTED: %d; RECEIVED: %d", setsockoptTable[dwTableIndex].optlen, optlen);
                    bTestPassed = FALSE;
                }

                // Check optval
                switch (setsockoptTable[dwTableIndex].optname) {
                    case SO_ACCEPTCONN:
                        if (TRUE != bAcceptConn) {
                            xLog(hLog, XLL_FAIL, "optval - EXPECTED: %d; RECEIVED: %d", TRUE, bAcceptConn);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_BROADCAST:
                        if (TRUE != bBroadcast) {
                            xLog(hLog, XLL_FAIL, "optval - EXPECTED: %d; RECEIVED: %d", TRUE, bBroadcast);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_REUSEADDR:
                        if (TRUE != bReuseAddr) {
                            xLog(hLog, XLL_FAIL, "optval - EXPECTED: %d; RECEIVED: %d", TRUE, bReuseAddr);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_EXCLUSIVEADDRUSE:
                        if (TRUE != bExclusiveAddrUse) {
                            xLog(hLog, XLL_FAIL, "optval - EXPECTED: %d; RECEIVED: %d", TRUE, bExclusiveAddrUse);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_LINGER:
                        if (0 != linger.l_onoff) {
                            xLog(hLog, XLL_FAIL, "l_onoff - EXPECTED: %d; RECEIVED: %d", 0, linger.l_onoff);
                            bTestPassed = FALSE;
                        }

                        if (5 != linger.l_linger) {
                            xLog(hLog, XLL_FAIL, "l_linger - EXPECTED: %d; RECEIVED: %d", 5, linger.l_linger);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_DONTLINGER:
                        if (FALSE != bDontLinger) {
                            xLog(hLog, XLL_FAIL, "optval - EXPECTED: %d; RECEIVED: %d", FALSE, bDontLinger);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_SNDBUF:
                        if (1 != nSndBuf) {
                            xLog(hLog, XLL_FAIL, "optval - EXPECTED: %d; RECEIVED: %d", 1, nSndBuf);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_RCVBUF:
                        if (1 != nRcvBuf) {
                            xLog(hLog, XLL_FAIL, "optval - EXPECTED: %d; RECEIVED: %d", 1, nRcvBuf);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_SNDTIMEO:
                        if (5 != nSndTimeO) {
                            xLog(hLog, XLL_FAIL, "optval - EXPECTED: %d; RECEIVED: %d", 5, nSndTimeO);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_RCVTIMEO:
                        if (5 != nRcvTimeO) {
                            xLog(hLog, XLL_FAIL, "optval - EXPECTED: %d; RECEIVED: %d", 5, nRcvTimeO);
                            bTestPassed = FALSE;
                        }

                        break;

                    case SO_TYPE:
                        if (0 != (SOCKET_TCP & setsockoptTable[dwTableIndex].dwSocket)) {
                            if (SOCK_STREAM != nType) {
                                xLog(hLog, XLL_FAIL, "optval - EXPECTED: %d; RECEIVED: %d", SOCK_STREAM, nType);
                                bTestPassed = FALSE;
                            }
                        }
                        else {
                            if (SOCK_DGRAM != nType) {
                                xLog(hLog, XLL_FAIL, "optval - EXPECTED: %d; RECEIVED: %d", SOCK_DGRAM, nType);
                                bTestPassed = FALSE;
                            }
                        }

                        break;

                    case TCP_NODELAY:
                        if (TRUE != bNoDelay) {
                            xLog(hLog, XLL_FAIL, "optval - EXPECTED: %d; RECEIVED: %d", TRUE, bNoDelay);
                            bTestPassed = FALSE;
                        }

                        break;
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "setsockopt succeeded");
                }
            }
        }

        if (0 == (SOCKET_CLOSED & setsockoptTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & setsockoptTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & setsockoptTable[dwTableIndex].dwSocket))) {
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
