/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  ioctlsocket.c

Abstract:

  This modules tests ioctlsocket

Author:

  Steven Kehrli (steveke) 26-Jan-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// ioctlsocket messages

#define IOCTLSOCKET_REQUEST_MSG   NETSYNC_MSG_USER + 230 + 1
#define IOCTLSOCKET_COMPLETE_MSG  NETSYNC_MSG_USER + 230 + 2

typedef struct _IOCTLSOCKET_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  Port;
    BOOL     bServerAccept;
    BOOL     bServerSendData;
    DWORD    dwDataSize1;
    DWORD    dwDataSize2;
} IOCTLSOCKET_REQUEST, *PIOCTLSOCKET_REQUEST;

typedef struct _IOCTLSOCKET_COMPLETE {
    DWORD    dwMessageId;
} IOCTLSOCKET_COMPLETE, *PIOCTLSOCKET_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

#define SIOCATMARK  _IOR('s',  7, u_long)



typedef struct IOCTLSOCKET_TABLE {
    CHAR    szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL    bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    BOOL    bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD   dwSocket;                                // dwSocket indicates the socket to be created
    BOOL    bBind;                                   // bBind indicates if the socket is to be bound
    BOOL    bListen;                                 // bListen indicates if the socket is to be placed in the listening state
    BOOL    bAccept;                                 // bAccept indicates if the socket is accepted
    BOOL    bConnect;                                // bConnect indicates if the socket is connected
    BOOL    bReceiveData;                            // bReceiveData specifies if any receive data should be queued
    DWORD   dwDataSize1;                             // dwDataSize1 specifies the amount of receive data that should be queued first
    DWORD   dwDataSize2;                             // dwDataSize2 specifies the amount of receive data that should be queued second
    long    Command;                                 // ioctlsocket command
    u_long  argp_in;                                 // ioctlsocket argp in
    BOOL    bargp_out;                               // argp_out specifies if argp is used
    int     iReturnCode;                             // iReturnCode is the return code of ioctlsocket
    int     iLastError;                              // iLastError is the error code if the operation failed
    BOOL    bRIP;                                    // Specifies a RIP test case
} IOCTLSOCKET_TABLE, *PIOCTLSOCKET_TABLE;

static IOCTLSOCKET_TABLE ioctlsocketTable[] =
{
    { "23.1 Not Initialized",            FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  0,          0, FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "23.2 s = INT_MIN",                TRUE,  TRUE,  SOCKET_INT_MIN,             FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  0,          0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "23.3 s = -1",                     TRUE,  TRUE,  SOCKET_NEG_ONE,             FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  0,          0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "23.4 s = 0",                      TRUE,  TRUE,  SOCKET_ZERO,                FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  0,          0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "23.5 s = INT_MAX",                TRUE,  TRUE,  SOCKET_INT_MAX,             FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  0,          0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "23.6 s = INVALID_SOCKET",         TRUE,  TRUE,  SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  0,          0, FALSE, SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "23.7 FIONBIO 0 Not Bound TCP",    TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  FIONBIO,    0, TRUE,  0,            0,                 FALSE },
    { "23.8 FIONBIO 0 Bound TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, 0,  0,  FIONBIO,    0, TRUE,  0,            0,                 FALSE },
    { "23.9 FIONBIO 0 Listening TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, 0,  0,  FIONBIO,    0, TRUE,  0,            0,                 FALSE },
    { "23.10 FIONBIO 0 Accepted TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, 0,  0,  FIONBIO,    0, TRUE,  0,            0,                 FALSE },
    { "23.11 FIONBIO 0 Connected TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONBIO,    0, TRUE,  0,            0,                 FALSE },
    { "23.12 FIONBIO 0 NULL TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONBIO,    0, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "23.13 FIONBIO 1 Not Bound TCP",   TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  FIONBIO,    1, TRUE,  0,            0,                 FALSE },
    { "23.14 FIONBIO 1 Bound TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, 0,  0,  FIONBIO,    1, TRUE,  0,            0,                 FALSE },
    { "23.15 FIONBIO 1 Listening TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, 0,  0,  FIONBIO,    1, TRUE,  0,            0,                 FALSE },
    { "23.16 FIONBIO 1 Accepted TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, 0,  0,  FIONBIO,    1, TRUE,  0,            0,                 FALSE },
    { "23.17 FIONBIO 1 Connected TCP",   TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONBIO,    1, TRUE,  0,            0,                 FALSE },
    { "23.18 FIONBIO 1 NULL TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONBIO,    1, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "23.19 FIONBIO 0 Not Bound UDP",   TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  FIONBIO,    0, TRUE,  0,            0,                 FALSE },
    { "23.20 FIONBIO 0 Bound UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, 0,  0,  FIONBIO,    0, TRUE,  0,            0,                 FALSE },
    { "23.21 FIONBIO 0 Connected UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONBIO,    0, TRUE,  0,            0,                 FALSE },
    { "23.22 FIONBIO 0 NULL UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONBIO,    0, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "23.23 FIONBIO 1 Not Bound UDP",   TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  FIONBIO,    1, TRUE,  0,            0,                 FALSE },
    { "23.24 FIONBIO 1 Bound UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, 0,  0,  FIONBIO,    1, TRUE,  0,            0,                 FALSE },
    { "23.25 FIONBIO 1 Connected UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONBIO,    1, TRUE,  0,            0,                 FALSE },
    { "23.26 FIONBIO 1 NULL UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONBIO,    1, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "23.27 FIONREAD Not Bound TCP",    TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.28 FIONREAD Bound TCP",        TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, 0,  0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.29 FIONREAD Listening TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  FALSE, FALSE, FALSE, 0,  0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.30 FIONREAD Accepted TCP",     TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  TRUE,  TRUE,  FALSE, FALSE, 0,  0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.31 FIONREAD Connected TCP",    TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.32 FIONREAD NULL TCP",         TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONREAD,   0, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "23.33 FIONREAD No Data TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.34 FIONREAD 0 Data TCP",       TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, TRUE,  TRUE,  0,  0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.35 FIONREAD 10 Data TCP",      TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, TRUE,  TRUE,  10, 0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.36 FIONREAD 10 & 5 Data TCP",  TRUE,  TRUE,  SOCKET_TCP,                 TRUE,  FALSE, FALSE, TRUE,  TRUE,  10, 5,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.37 FIONREAD Not Bound UDP",    TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.38 FIONREAD Bound UDP",        TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, FALSE, FALSE, 0,  0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.39 FIONREAD Connected UDP",    TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.40 FIONREAD NULL UDP",         TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONREAD,   0, FALSE, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "23.41 FIONREAD No Data UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, TRUE,  FALSE, 0,  0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.42 FIONREAD 0 Data UDP",       TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, TRUE,  TRUE,  0,  0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.43 FIONREAD 10 Data UDP",      TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, TRUE,  TRUE,  10, 0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.44 FIONREAD 10 & 5 Data UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, TRUE,  TRUE,  10, 5,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.45 FIONREAD 10 & 0 Data UDP",  TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, TRUE,  TRUE,  10, 0,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.46 FIONREAD 0 & 5 Data UDP",   TRUE,  TRUE,  SOCKET_UDP,                 TRUE,  FALSE, FALSE, TRUE,  TRUE,  0,  5,  FIONREAD,   0, TRUE,  0,            0,                 FALSE },
    { "23.47 SIOCATMARK TCP",            TRUE,  TRUE,  SOCKET_TCP,                 FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  SIOCATMARK, 0, TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "23.48 SIOCATMARK UDP",            TRUE,  TRUE,  SOCKET_UDP,                 FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  SIOCATMARK, 0, TRUE,  SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "23.49 Closed Socket TCP",         TRUE,  TRUE,  SOCKET_TCP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  0,          0, TRUE,  SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "23.50 Closed Socket UDP",         TRUE,  TRUE,  SOCKET_UDP | SOCKET_CLOSED, FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  0,          0, TRUE,  SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "23.51 Not Initialized",           FALSE, FALSE, SOCKET_INVALID_SOCKET,      FALSE, FALSE, FALSE, FALSE, FALSE, 0,  0,  0,          0, FALSE, SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define ioctlsocketTableCount (sizeof(ioctlsocketTable) / sizeof(IOCTLSOCKET_TABLE))

NETSYNC_TYPE_THREAD  ioctlsocketTestSessionNt =
{
    1,
    ioctlsocketTableCount,
    L"xnetapi_nt.dll",
    "ioctlsocketTestServer"
};

NETSYNC_TYPE_THREAD  ioctlsocketTestSessionXbox =
{
    1,
    ioctlsocketTableCount,
    L"xnetapi_xbox.dll",
    "ioctlsocketTestServer"
};



VOID
ioctlsocketTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN WORD    NetsyncRemoteType,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests ioctlsocket - Client side

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
    // ioctlsocketRequest is the request sent to the server
    IOCTLSOCKET_REQUEST    ioctlsocketRequest;
    
    // sSocket is the socket descriptor
    SOCKET                 sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                 nsSocket;

    // localname is the local address
    SOCKADDR_IN            localname;
    // remotename is the remote address
    SOCKADDR_IN            remotename;

    // iTimeout is the send and receive timeout value for the socket
    int                    iTimeout = SLEEP_LOW_TIME;
    // bNagle indicates if Nagle is enabled
    BOOL                   bNagle = FALSE;

    // RecvBufferLarge is the large recv buffer
    char                   RecvBufferLarge[BUFFER_LARGE_LEN + 1];
    // argp is the ioctlsocket command parameter
    u_long                 argp;

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
    sprintf(szFunctionName, "ioctlsocket v%04x vs %s", WinsockVersion, (VS_XBOX == NetsyncRemoteType) ? "Xbox" : "Nt");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_ioctlsocket+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_ioctlsocket-");

    // Determine the remote netsync server type
    if (VS_XBOX == NetsyncRemoteType) {
        NetsyncTypeSession = ioctlsocketTestSessionXbox;
    }
    else {
        NetsyncTypeSession = ioctlsocketTestSessionNt;
    }

    // Initialize the net subsystem
    XNetAddRef();

    for (dwTableIndex = 0; dwTableIndex < ioctlsocketTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, ioctlsocketTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, ioctlsocketTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != ioctlsocketTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, ioctlsocketTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == ioctlsocketTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of Winsock
        if (bWinsockInitialized != ioctlsocketTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == ioctlsocketTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = ioctlsocketTable[dwTableIndex].bWinsockInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == ioctlsocketTable[dwTableIndex].bNetsyncConnected)) {
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
        if (SOCKET_INT_MIN == ioctlsocketTable[dwTableIndex].dwSocket) {
            sSocket = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == ioctlsocketTable[dwTableIndex].dwSocket) {
            sSocket = -1;
        }
        else if (SOCKET_ZERO == ioctlsocketTable[dwTableIndex].dwSocket) {
            sSocket = 0;
        }
        else if (SOCKET_INT_MAX == ioctlsocketTable[dwTableIndex].dwSocket) {
            sSocket = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == ioctlsocketTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & ioctlsocketTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & ioctlsocketTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Set the send and receive timeout values
        if ((0 != (SOCKET_TCP  & ioctlsocketTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & ioctlsocketTable[dwTableIndex].dwSocket))) {
            setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
            setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        }

        // Disable Nagle
        if (0 != (SOCKET_TCP & ioctlsocketTable[dwTableIndex].dwSocket)) {
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Bind the socket
        if (TRUE == ioctlsocketTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Place the socket in the listening state
        if (TRUE == ioctlsocketTable[dwTableIndex].bListen) {
            listen(sSocket, SOMAXCONN);
        }

        if ((TRUE == ioctlsocketTable[dwTableIndex].bAccept) || (TRUE == ioctlsocketTable[dwTableIndex].bConnect)) {
            // Initialize the ioctlsocket request
            ioctlsocketRequest.dwMessageId = IOCTLSOCKET_REQUEST_MSG;
            if (0 != (SOCKET_TCP & ioctlsocketTable[dwTableIndex].dwSocket)) {
                ioctlsocketRequest.nSocketType = SOCK_STREAM;
            }
            else {
                ioctlsocketRequest.nSocketType = SOCK_DGRAM;
            }
            ioctlsocketRequest.Port = CurrentPort;
            ioctlsocketRequest.bServerAccept = ioctlsocketTable[dwTableIndex].bConnect;
            ioctlsocketRequest.bServerSendData = ioctlsocketTable[dwTableIndex].bReceiveData;
            ioctlsocketRequest.dwDataSize1 = ioctlsocketTable[dwTableIndex].dwDataSize1;
            ioctlsocketRequest.dwDataSize2 = ioctlsocketTable[dwTableIndex].dwDataSize2;

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(ioctlsocketRequest), (char *) &ioctlsocketRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);

            if (TRUE == ioctlsocketTable[dwTableIndex].bConnect) {
                // Connect the socket
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = NetsyncInAddr;
                remotename.sin_port = htons(CurrentPort);

                connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
            }
            else if (TRUE == ioctlsocketTable[dwTableIndex].bAccept) {
                // Accept the socket
                nsSocket = accept(sSocket, NULL, NULL);
            }

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(ioctlsocketRequest), (char *) &ioctlsocketRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);
        }

        // Initialize the socket result
        argp = ioctlsocketTable[dwTableIndex].argp_in;

        if ((0 != (SOCKET_TCP & ioctlsocketTable[dwTableIndex].dwSocket)) && (10 == ioctlsocketTable[dwTableIndex].dwDataSize1) && (5 == ioctlsocketTable[dwTableIndex].dwDataSize2)) {
            do {
                Sleep(SLEEP_LOW_TIME);

                ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket: nsSocket, FIONREAD, &argp);
            } while (15 != argp);
        }

        bTestPassed = TRUE;
        bException = FALSE;

        // Close the socket, if necessary
        if (0 != (SOCKET_CLOSED & ioctlsocketTable[dwTableIndex].dwSocket)) {
            closesocket(sSocket);
        }

        __try {
            // Call ioctlsocket
            iReturnCode = ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket: nsSocket, ioctlsocketTable[dwTableIndex].Command, (TRUE == ioctlsocketTable[dwTableIndex].bargp_out) ? &argp : NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == ioctlsocketTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "ioctlsocket RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "ioctlsocket caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == ioctlsocketTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "ioctlsocket did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == ioctlsocketTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != ioctlsocketTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "ioctlsocket iLastError - EXPECTED: %u; RECEIVED: %u", ioctlsocketTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "ioctlsocket iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "ioctlsocket returned SOCKET_ERROR - ec = %u", WSAGetLastError());
            }
            else if (SOCKET_ERROR == ioctlsocketTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "ioctlsocket returned non-SOCKET_ERROR");
            }
            else {
                if (FIONBIO == ioctlsocketTable[dwTableIndex].Command) {
                    if ((TRUE == ioctlsocketTable[dwTableIndex].bAccept) || (TRUE == ioctlsocketTable[dwTableIndex].bConnect)) {
                        // Get the first tick count
                        dwFirstTime = GetTickCount();

                        // Call receive
                        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
                        recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, sizeof(RecvBufferLarge), 0);

                        // Get the second tick count
                        dwSecondTime = GetTickCount();

                        if (0 == ioctlsocketTable[dwTableIndex].argp_in) {
                            // Blocking operation
                            if (((dwSecondTime - dwFirstTime) < SLEEP_LOW_TIME) || ((dwSecondTime - dwFirstTime) > SLEEP_HIGH_TIME)) {
                                xLog(hLog, XLL_FAIL, "FIONBIO nTime - EXPECTED: %d; RECEIVED: %d", SLEEP_MEAN_TIME, dwSecondTime - dwFirstTime);
                                bTestPassed = FALSE;
                            }
                        }
                        else {
                            // Nonblocking operation
                            if ((dwSecondTime - dwFirstTime) > SLEEP_ZERO_TIME) {
                                xLog(hLog, XLL_FAIL, "FIONBIO nTime - EXPECTED: %d; RECEIVED: %d", 0, dwSecondTime - dwFirstTime);
                                bTestPassed = FALSE;
                            }
                        }
                    }
                }
                else if (FIONREAD == ioctlsocketTable[dwTableIndex].Command) {
                    if (0 != (SOCKET_TCP & ioctlsocketTable[dwTableIndex].dwSocket)) {
                        if (argp != (ioctlsocketTable[dwTableIndex].dwDataSize1 + ioctlsocketTable[dwTableIndex].dwDataSize2)) {
                            xLog(hLog, XLL_FAIL, "FIONREAD argp1 - EXPECTED: %d; RECEIVED: %d", ioctlsocketTable[dwTableIndex].dwDataSize1 + ioctlsocketTable[dwTableIndex].dwDataSize2, argp);
                            bTestPassed = FALSE;
                        }
                    }
                    else {
                        if (TRUE == ioctlsocketTable[dwTableIndex].bReceiveData) {
                            if (argp != ioctlsocketTable[dwTableIndex].dwDataSize1 + (0 == ioctlsocketTable[dwTableIndex].dwDataSize1) ? 1 : 0) {
                                xLog(hLog, XLL_FAIL, "FIONREAD argp1 - EXPECTED: %d; RECEIVED: %d", ioctlsocketTable[dwTableIndex].dwDataSize1 + (0 == ioctlsocketTable[dwTableIndex].dwDataSize1) ? 1 : 0, argp);
                                bTestPassed = FALSE;
                            }
                        }
                        else {
                            if (argp != 0) {
                                xLog(hLog, XLL_FAIL, "FIONREAD argp1 - EXPECTED: %d; RECEIVED: %d", 0, argp);
                                bTestPassed = FALSE;
                            }
                        }
                    }

                    if ((0 != ioctlsocketTable[dwTableIndex].dwDataSize1) || (0 != ioctlsocketTable[dwTableIndex].dwDataSize2)) {
                        // Call receive
                        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
                        recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, ioctlsocketTable[dwTableIndex].dwDataSize1, 0);
                    
                        argp = 0;
                        iReturnCode = ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONREAD, &argp);

                        if (SOCKET_ERROR == iReturnCode) {
                            xLog(hLog, XLL_FAIL, "ioctlsocket returned SOCKET_ERROR - ec = %u", WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                        else {
                            if (0 != (SOCKET_TCP & ioctlsocketTable[dwTableIndex].dwSocket)) {
                                if (argp != ioctlsocketTable[dwTableIndex].dwDataSize2) {
                                    xLog(hLog, XLL_FAIL, "FIONREAD argp2 - EXPECTED: %d; RECEIVED: %d", ioctlsocketTable[dwTableIndex].dwDataSize2, argp);
                                    bTestPassed = FALSE;
                                }
                            }
                            else {
                                if (argp != ioctlsocketTable[dwTableIndex].dwDataSize2 + (0 == ioctlsocketTable[dwTableIndex].dwDataSize2) ? 1 : 0) {
                                    xLog(hLog, XLL_FAIL, "FIONREAD argp2 - EXPECTED: %d; RECEIVED: %d", ioctlsocketTable[dwTableIndex].dwDataSize2 + (0 == ioctlsocketTable[dwTableIndex].dwDataSize2) ? 1 : 0, argp);
                                    bTestPassed = FALSE;
                                }
                            }
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "ioctlsocket succeeded");
                }
            }
        }

        if ((TRUE == ioctlsocketTable[dwTableIndex].bAccept) || (TRUE == ioctlsocketTable[dwTableIndex].bConnect)) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(ioctlsocketRequest), (char *) &ioctlsocketRequest);
        }

        // Close the sockets
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }

        if (0 == (SOCKET_CLOSED & ioctlsocketTable[dwTableIndex].dwSocket)) {
            if ((0 != (SOCKET_TCP & ioctlsocketTable[dwTableIndex].dwSocket)) || (0 != (SOCKET_UDP & ioctlsocketTable[dwTableIndex].dwSocket))) {
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
ioctlsocketTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests ioctlsocket - Server side

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
    // ioctlsocketRequest is the request
    IOCTLSOCKET_REQUEST   ioctlsocketRequest;
    // ioctlsocketComplete is the result
    IOCTLSOCKET_COMPLETE  ioctlsocketComplete;

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
        CopyMemory(&ioctlsocketRequest, pMessage, sizeof(ioctlsocketRequest));
        NetsyncFreeMessage(pMessage);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, ioctlsocketRequest.nSocketType, 0);

        // Set the send and receive timeout values to 5 sec
        setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));

        if (SOCK_STREAM == ioctlsocketRequest.nSocketType) {
            // Disable Nagle
            setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));
        }

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(ioctlsocketRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if ((SOCK_STREAM == ioctlsocketRequest.nSocketType) && (TRUE == ioctlsocketRequest.bServerAccept)) {
            // Place the socket in listening mode
            listen(sSocket, SOMAXCONN);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = FromInAddr;
            remotename.sin_port = htons(ioctlsocketRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        ioctlsocketComplete.dwMessageId = IOCTLSOCKET_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(ioctlsocketComplete), (char *) &ioctlsocketComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if ((SOCK_STREAM == ioctlsocketRequest.nSocketType) && (TRUE == ioctlsocketRequest.bServerAccept)) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        if (TRUE == ioctlsocketRequest.bServerSendData) {
            if (0 == ioctlsocketRequest.dwDataSize1) {
                send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, NULL, 0, 0);
            }
            else {
                // Initialize the buffers
                sprintf(SendBuffer10, "%05d%05d", 1, 1);
                for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += 10) {
                    CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > 10 ? 10 : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                }

                send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, ioctlsocketRequest.dwDataSize1, 0);
            }

            if (0 == ioctlsocketRequest.dwDataSize2) {
                send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, NULL, 0, 0);
            }
            else {
                // Initialize the buffers
                sprintf(SendBuffer10, "%05d%05d", 2, 2);
                for (dwFillBuffer = 0; dwFillBuffer < BUFFER_TCPLARGE_LEN; dwFillBuffer += 10) {
                    CopyMemory(&SendBufferLarge[dwFillBuffer], SendBuffer10, BUFFER_TCPLARGE_LEN - dwFillBuffer > 10 ? 10 : BUFFER_TCPLARGE_LEN - dwFillBuffer);
                }

                send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBufferLarge, ioctlsocketRequest.dwDataSize2, 0);
            }
        }

        // Send the complete
        ioctlsocketComplete.dwMessageId = IOCTLSOCKET_COMPLETE_MSG;
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(ioctlsocketComplete), (char *) &ioctlsocketComplete);

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
