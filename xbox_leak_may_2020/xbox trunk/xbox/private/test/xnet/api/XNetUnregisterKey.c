/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  XNetUnregisterKey.c

Abstract:

  This modules tests XNetUnregisterKey

Author:

  Steven Kehrli (steveke) 19-Jul-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



#ifdef _XBOX

using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// XNetUnregisterKey messages

#define XNETUNREGISTERKEY_REQUEST_MSG   NETSYNC_MSG_USER + 360 + 1
#define XNETUNREGISTERKEY_COMPLETE_MSG  NETSYNC_MSG_USER + 360 + 2

typedef struct _XNETUNREGISTERKEY_REQUEST {
    DWORD    dwMessageId;
    int      nSocketType;
    u_short  Port;
    BOOL     bServerAccept;
    XNADDR   XnAddr;
    XNKID    XnKid;
    XNKEY    XnKey;
} XNETUNREGISTERKEY_REQUEST, *PXNETUNREGISTERKEY_REQUEST;

typedef struct _XNETUNREGISTERKEY_COMPLETE {
    DWORD    dwMessageId;
    XNADDR   XnAddr;
} XNETUNREGISTERKEY_COMPLETE, *PXNETUNREGISTERKEY_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

using namespace XNetAPINamespace;

namespace XNetAPINamespace {

typedef struct _XNETUNREGISTERKEY_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL     bXnetInitialized;                        // bXnetInitialized indicates if the net subsystem is initialized
    BOOL     bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    DWORD    dwSocket;                                // dwSocket indicates the socket to be created
    BOOL     bConnect;                                // bConnect indicates if the socket is connected
    BOOL     bXnKid;                                  // bXnKid indicates if the xnkid is valid
    BOOL     bRegister;                               // bRegister indicates if the xnkid is registered
    DWORD    dwReturnCode;                            // dwReturnCode is the return code of the operation
    BOOL     bRIP;                                    // Specifies a RIP test case
} XNETUNREGISTERKEY_TABLE, *PXNETUNREGISTERKEY_TABLE;

static XNETUNREGISTERKEY_TABLE XNetUnregisterKeyTable[] =
{
    { "36.1 Not Initialized", FALSE, FALSE, SOCKET_INVALID_SOCKET, FALSE, TRUE,  TRUE,  WSANOTINITIALISED, FALSE },
    { "36.2 TCP A",           TRUE,  TRUE,  SOCKET_TCP,            FALSE, TRUE,  TRUE,  0,                 FALSE },
    { "36.3 TCP C",           TRUE,  TRUE,  SOCKET_TCP,            TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "36.4 UDP",             TRUE,  TRUE,  SOCKET_UDP,            TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "36.5 NULL xnkid",      TRUE,  TRUE,  SOCKET_INVALID_SOCKET, FALSE, FALSE, TRUE,  0,                 TRUE  },
    { "36.6 Invalid xnkid",   TRUE,  TRUE,  SOCKET_INVALID_SOCKET, FALSE, TRUE,  FALSE, WSAEINVAL,         FALSE },
    { "36.7 Not Initialized", FALSE, FALSE, SOCKET_INVALID_SOCKET, FALSE, TRUE,  TRUE,  WSANOTINITIALISED, FALSE }
};

#define XNetUnregisterKeyTableCount (sizeof(XNetUnregisterKeyTable) / sizeof(XNETUNREGISTERKEY_TABLE))

NETSYNC_TYPE_THREAD  XNetUnregisterKeyTestSessionXbox =
{
    1,
    XNetUnregisterKeyTableCount,
    L"xnetapi_xbox.dll",
    "XNetUnregisterKeyTestServer"
};



VOID
XNetUnregisterKeyTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN LPSTR   lpszNetsyncRemote,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests XNetUnregisterKey

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  WinsockVersion - requested version of Winsock
  lpszNetsyncRemote - pointer to the netsync remote address
  bRIPs - specifies RIP testing

------------------------------------------------------------------------------*/
{
    // lpszCaseTest is a pointer to the list of cases to test
    LPSTR                      lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR                      lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD                      dwTableIndex;

    // bXnetInitialized indicates if the net subsystem is initialized
    BOOL                       bXnetInitialized = FALSE;
    // WSAData is the details of the Winsock implementation
    WSADATA                    WSAData;

    // hNetsyncObject is a handle to the netsync object
    HANDLE                     hNetsyncObject = INVALID_HANDLE_VALUE;
    // NetsyncTypeSession is the session type descriptor
    NETSYNC_TYPE_THREAD        NetsyncTypeSession;
    // NetsyncInAddr is the address of the netsync server
    u_long                     NetsyncInAddr = 0;
    // LowPort is the low bound of the netsync port range
    u_short                    LowPort = 0;
    // HighPort is the high bound of the netsync port range
    u_short                    HighPort = 0;
    // CurrentPort is the current port in the netsync port range
    u_short                    CurrentPort = 0;
    // FromInAddr is the address of the netsync sender
    u_long                     FromInAddr;
    // dwMessageSize is the size of the received message
    DWORD                      dwMessageSize;
    // pMessage is a pointer to the received message
    char                       *pMessage;
    // XNetUnregisterKeyRequest is the request sent to the server
    XNETUNREGISTERKEY_REQUEST  XNetUnregisterKeyRequest;

    // ClientXnAddr is the address of the client
    XNADDR                     ClientXnAddr;

    // pXnKid is a pointer to the xnkid
    XNKID                      *pXnKid;
    // XnKid1 is the first xnkid
    XNKID                      XnKid1;
    // XnKid2 is the second xnkid
    XNKID                      XnKid2;
    // XnKey1 is the first xnkey
    XNKEY                      XnKey1;
    // XnKey2 is the second xnkey
    XNKEY                      XnKey2;
    // LocalXnAddr is the address of the local machine
    XNADDR                     LocalXnAddr;
    // ServerXnAddr is the address of the netsync server
    XNADDR                     ServerXnAddr;
    // ServerInAddr is the address of the netsync server
    u_long                     ServerInAddr;

    // sSocket is the socket descriptor
    SOCKET                     sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                     nsSocket;

    // iTimeout is the send and receive timeout value for the socket
    int                        iTimeout = 5000;

    // localname is the local address
    SOCKADDR_IN                localname;
    // remotename is the remote address
    SOCKADDR_IN                remotename;

    // readfds is the set of sockets to check for a read condition
    fd_set                     readfds;
    // writefds is the set of sockets to check for a write condition
    fd_set                     writefds;

    // SendBuffer10 is the send buffer
    char                       SendBuffer10[BUFFER_10_LEN + 1];
    // RecvBufferLarge is the large recv buffer
    char                       RecvBufferLarge[BUFFER_LARGE_LEN + 1];

    // bException indicates if an exception occurred
    BOOL                       bException;
    // dwReturnCode is the return code of the operation
    int                        dwReturnCode;
    // bTestPassed indicates if the test passed
    BOOL                       bTestPassed;

    // szFunctionName is the function name
    CHAR                       szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    sprintf(szFunctionName, "XNetUnregisterKey v%04x", WinsockVersion);
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_XNetUnregisterKey+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_XNetUnregisterKey-");

    // Determine the remote netsync server type
    NetsyncTypeSession = XNetUnregisterKeyTestSessionXbox;

    for (dwTableIndex = 0; dwTableIndex < XNetUnregisterKeyTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, XNetUnregisterKeyTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, XNetUnregisterKeyTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != XNetUnregisterKeyTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, XNetUnregisterKeyTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == XNetUnregisterKeyTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of the net subsystem
        if (bXnetInitialized != XNetUnregisterKeyTable[dwTableIndex].bXnetInitialized) {
            // Initialize or terminate net subsystem as necessary
            if (TRUE == XNetUnregisterKeyTable[dwTableIndex].bXnetInitialized) {
                XNetAddRef();
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
                XNetRelease();
            }

            // Update the state of net subsystem
            bXnetInitialized = XNetUnregisterKeyTable[dwTableIndex].bXnetInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == XNetUnregisterKeyTable[dwTableIndex].bNetsyncConnected)) {
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

        // Get the local xnaddr
        do {
            dwReturnCode = XNetGetTitleXnAddr(&LocalXnAddr);
            if (0 == dwReturnCode) {
                Sleep(SLEEP_ZERO_TIME);
            }
        } while (0 == dwReturnCode);

        // Clear the xnaddr, xnkid and xnkey
        ZeroMemory(&ServerXnAddr, sizeof(ServerXnAddr));
        ZeroMemory(&XnKid1, sizeof(XnKid1));
        ZeroMemory(&XnKid2, sizeof(XnKid2));
        ZeroMemory(&XnKey1, sizeof(XnKey1));
        ZeroMemory(&XnKey2, sizeof(XnKey2));
        XNetCreateKey(&XnKid1, &XnKey1);
        XNetCreateKey(&XnKid2, &XnKey2);

        // Register the key
        XNetRegisterKey(&XnKid1, &XnKey1);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        if (SOCKET_INVALID_SOCKET == XNetUnregisterKeyTable[dwTableIndex].dwSocket) {
            sSocket = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & XNetUnregisterKeyTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & XNetUnregisterKeyTable[dwTableIndex].dwSocket)) {
            sSocket = socket(AF_INET, SOCK_DGRAM, 0);
        }

        if (INVALID_SOCKET != sSocket) {
            // Set the send and receive timeout values to 5 sec
            setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
            setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        }

        // Bind the socket
        if (INVALID_SOCKET != sSocket) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(CurrentPort);
            bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Place the socket in the listening state
        if ((0 != (SOCKET_TCP & XNetUnregisterKeyTable[dwTableIndex].dwSocket)) && (FALSE == XNetUnregisterKeyTable[dwTableIndex].bConnect)) {
            listen(sSocket, SOMAXCONN);
        }

        if (SOCKET_INVALID_SOCKET != XNetUnregisterKeyTable[dwTableIndex].dwSocket) {
            // Initialize the connect request
            XNetUnregisterKeyRequest.dwMessageId = XNETUNREGISTERKEY_REQUEST_MSG;
            if (0 != (SOCKET_TCP & XNetUnregisterKeyTable[dwTableIndex].dwSocket)) {
                XNetUnregisterKeyRequest.nSocketType = SOCK_STREAM;
            }
            else {
                XNetUnregisterKeyRequest.nSocketType = SOCK_DGRAM;
            }
            XNetUnregisterKeyRequest.Port = CurrentPort;
            XNetUnregisterKeyRequest.bServerAccept = (XNetUnregisterKeyTable[dwTableIndex].bConnect) && (0 != (SOCKET_TCP & XNetUnregisterKeyTable[dwTableIndex].dwSocket));
            CopyMemory(&XNetUnregisterKeyRequest.XnAddr, &LocalXnAddr, sizeof(XNetUnregisterKeyRequest.XnAddr));
            CopyMemory(&XNetUnregisterKeyRequest.XnKid, &XnKid1, sizeof(XnKid1));
            CopyMemory(&XNetUnregisterKeyRequest.XnKey, &XnKey1, sizeof(XnKey1));

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(XNetUnregisterKeyRequest), (char *) &XNetUnregisterKeyRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            CopyMemory(&ServerXnAddr, &((XNETUNREGISTERKEY_COMPLETE *) pMessage)->XnAddr, sizeof(ServerXnAddr));
            NetsyncFreeMessage(pMessage);

            // Resolve the address
            XNetXnAddrToInAddr(&ServerXnAddr, &XnKid1, (IN_ADDR *) &ServerInAddr);

            if (TRUE == XNetUnregisterKeyTable[dwTableIndex].bConnect) {
                // Connect the socket
                ZeroMemory(&remotename, sizeof(remotename));
                remotename.sin_family = AF_INET;
                remotename.sin_addr.s_addr = ServerInAddr;
                remotename.sin_port = htons(CurrentPort);

                connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));

                if (0 != (SOCKET_TCP & XNetUnregisterKeyTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&writefds);
                    FD_SET(sSocket, &writefds);
                    select(0, NULL, &writefds, NULL, NULL);
                }
            }
            else {
                if (0 != (SOCKET_TCP & XNetUnregisterKeyTable[dwTableIndex].dwSocket)) {
                    FD_ZERO(&readfds);
                    FD_SET(sSocket, &readfds);
                    select(0, &readfds, NULL, NULL, NULL);
                }

                // Accept the socket
                nsSocket = accept(sSocket, NULL, NULL);
            }

            // Send data
            sprintf(SendBuffer10, "%05d%05d", 1, 1);
            send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SendBuffer10, 10, 0);

            // Send the connect request
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(XNetUnregisterKeyRequest), (char *) &XNetUnregisterKeyRequest);

            // Wait for the connect complete
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            NetsyncFreeMessage(pMessage);
        }

        if (FALSE == XNetUnregisterKeyTable[dwTableIndex].bXnKid) {
            pXnKid = NULL;
        }
        else {
            if (TRUE == XNetUnregisterKeyTable[dwTableIndex].bRegister) {
                pXnKid = &XnKid1;
            }
            else {
                pXnKid = &XnKid2;
            }
        }

        bTestPassed = TRUE;
        bException = FALSE;

        __try {
            // Call XNetUnregisterKey
            dwReturnCode = XNetUnregisterKey(pXnKid);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == XNetUnregisterKeyTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "XNetUnregisterKey RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "XNetUnregisterKey caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == XNetUnregisterKeyTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "XNetUnregisterKey did not RIP");
            }

            if (dwReturnCode != XNetUnregisterKeyTable[dwTableIndex].dwReturnCode) {
                xLog(hLog, XLL_FAIL, "dwReturnCode - EXPECTED: %u; RECEIVED: %u", XNetUnregisterKeyTable[dwTableIndex].dwReturnCode, dwReturnCode);
            }
            else if (0 == dwReturnCode) {
                // Send the send request
                NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(XNetUnregisterKeyRequest), (char *) &XNetUnregisterKeyRequest);

                // Wait for the send complete
                NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
                NetsyncFreeMessage(pMessage);

                if (SOCKET_ERROR != recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, sizeof(RecvBufferLarge), 0)) {
                    xLog(hLog, XLL_FAIL, "recv passed");
                    bTestPassed = FALSE;
                }
                else {
                    if (0 != (SOCKET_TCP & XNetUnregisterKeyTable[dwTableIndex].dwSocket)) {
                        if (WSAECONNRESET != WSAGetLastError()) {
                            xLog(hLog, XLL_FAIL, "recv error code - EXPECTED: %u; RECEIVED: %u", WSAECONNRESET, WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                    }
                    else {
                        if (WSAETIMEDOUT != WSAGetLastError()) {
                            xLog(hLog, XLL_FAIL, "recv error code - EXPECTED: %u; RECEIVED: %u", WSAETIMEDOUT, WSAGetLastError());
                            bTestPassed = FALSE;
                        }
                    }
                }

                // Call XNetUnregisterKey again
                dwReturnCode = XNetUnregisterKey(pXnKid);
                if (WSAEINVAL != dwReturnCode) {
                    xLog(hLog, XLL_FAIL, "Second XNetUnregisterKey - EXPECTED: %u; RECEIVED: %u", WSAEINVAL, dwReturnCode);
                    bTestPassed = FALSE;
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "XNetUnregisterKey succeeded");
                }
            }
            else {
                xLog(hLog, XLL_PASS, "dwReturnCode - OUT: %u", dwReturnCode);
            }

        }

        if (&XnKid2 == pXnKid) {
            XNetUnregisterKey(&XnKid1);
        }

        if (SOCKET_INVALID_SOCKET != XNetUnregisterKeyTable[dwTableIndex].dwSocket) {
            // Send the ack
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(XNetUnregisterKeyRequest), (char *) &XNetUnregisterKeyRequest);
        }

        if (SOCKET_INVALID_SOCKET != XNetUnregisterKeyTable[dwTableIndex].dwSocket) {
            shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SD_BOTH);
            if (INVALID_SOCKET != nsSocket) {
                closesocket(nsSocket);
            }
            closesocket(sSocket);
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

    // Terminate net subsystem if necessary
    if (TRUE == bXnetInitialized) {
        WSACleanup();
        XNetRelease();
    }

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
XNetUnregisterKeyTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests XNetUnregisterKey - Server side

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
    u_long                      FromInAddr;
    // dwMessageType is the type of received message
    DWORD                       dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD                       dwMessageSize;
    // pMessage is a pointer to the received message
    char                        *pMessage;
    // XNetUnregisterKeyRequest is the request
    XNETUNREGISTERKEY_REQUEST   XNetUnregisterKeyRequest;
    // XNetUnregisterKeyComplete is the result
    XNETUNREGISTERKEY_COMPLETE  XNetUnregisterKeyComplete;

    // LocalXnAddr is the address of the local machine
    XNADDR                      LocalXnAddr;
    // ClientXnAddr is the address of the netsync client
    XNADDR                      ClientXnAddr;
    // ClientInAddr is the address of the netsync client
    u_long                      ClientInAddr;

    // sSocket is the socket descriptor
    SOCKET                      sSocket;
    // nsSocket is the accepted socket descriptor
    SOCKET                      nsSocket;

    // iTimeout is the send and receive timeout value for the socket
    int                         iTimeout = 5000;

    // localname is the local address
    SOCKADDR_IN                 localname;
    // remotename is the remote address
    SOCKADDR_IN                 remotename;

    // RecvBufferLarge is the large recv buffer
    char                        RecvBufferLarge[BUFFER_LARGE_LEN + 1];
    // nBytes is the number of bytes sent/received
    int                         nBytes;

    // dwReturnCode is the return code of the operation
    DWORD                       dwReturnCode;



    // Get the local xnaddr
    do {
        dwReturnCode = XNetGetTitleXnAddr(&LocalXnAddr);
        if (0 == dwReturnCode) {
            Sleep(SLEEP_ZERO_TIME);
        }
    } while (0 == dwReturnCode);

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
        CopyMemory(&XNetUnregisterKeyRequest, pMessage, sizeof(XNetUnregisterKeyRequest));
        NetsyncFreeMessage(pMessage);

        CopyMemory(&ClientXnAddr, &XNetUnregisterKeyRequest.XnAddr, sizeof(ClientXnAddr));

        // Register the key
        XNetRegisterKey(&XNetUnregisterKeyRequest.XnKid, &XNetUnregisterKeyRequest.XnKey);

        // Resolve the address
        XNetXnAddrToInAddr(&ClientXnAddr, &XNetUnregisterKeyRequest.XnKid, (IN_ADDR *) &ClientInAddr);

        // Create the socket
        sSocket = INVALID_SOCKET;
        nsSocket = INVALID_SOCKET;
        sSocket = socket(AF_INET, XNetUnregisterKeyRequest.nSocketType, 0);

        // Set the send and receive timeout values to 5 sec
        setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
        setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));

        // Bind the socket
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;
        localname.sin_port = htons(XNetUnregisterKeyRequest.Port);
        bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

        if (TRUE == XNetUnregisterKeyRequest.bServerAccept) {
            // Place the socket in listening mode
            listen(sSocket, SOMAXCONN);
        }
        else {
            // Connect the socket
            ZeroMemory(&remotename, sizeof(remotename));
            remotename.sin_family = AF_INET;
            remotename.sin_addr.s_addr = ClientInAddr;
            remotename.sin_port = htons(XNetUnregisterKeyRequest.Port);

            connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));
        }

        // Send the complete
        XNetUnregisterKeyComplete.dwMessageId = XNETUNREGISTERKEY_COMPLETE_MSG;
        CopyMemory(&XNetUnregisterKeyComplete.XnAddr, &LocalXnAddr, sizeof(XNetUnregisterKeyComplete.XnAddr));
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(XNetUnregisterKeyComplete), (char *) &XNetUnregisterKeyComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        if (TRUE == XNetUnregisterKeyRequest.bServerAccept) {
            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }

        ZeroMemory(RecvBufferLarge, sizeof(RecvBufferLarge));
        nBytes = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, sizeof(RecvBufferLarge), 0);

        // Send the complete
        XNetUnregisterKeyComplete.dwMessageId = XNETUNREGISTERKEY_COMPLETE_MSG;
        CopyMemory(&XNetUnregisterKeyComplete.XnAddr, &LocalXnAddr, sizeof(XNetUnregisterKeyComplete.XnAddr));
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(XNetUnregisterKeyComplete), (char *) &XNetUnregisterKeyComplete);

        // Wait for the request
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
        NetsyncFreeMessage(pMessage);

        // Send data
        send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, RecvBufferLarge, (SOCKET_ERROR == nBytes) ? 10 : nBytes, 0);

        // Send the complete
        XNetUnregisterKeyComplete.dwMessageId = XNETUNREGISTERKEY_COMPLETE_MSG;
        CopyMemory(&XNetUnregisterKeyComplete.XnAddr, &LocalXnAddr, sizeof(XNetUnregisterKeyComplete.XnAddr));
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(XNetUnregisterKeyComplete), (char *) &XNetUnregisterKeyComplete);

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

        // Unregister the key
        XNetUnregisterKey(&XNetUnregisterKeyRequest.XnKid);
    }
}

#endif

#endif // _XBOX
