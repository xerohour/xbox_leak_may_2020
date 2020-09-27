/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  XNetInAddrToXnAddr.c

Abstract:

  This modules tests XNetInAddrToXnAddr

Author:

  Steven Kehrli (steveke) 23-Jul-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



#ifdef _XBOX

using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// XNetInAddrToXnAddr messages

#define XNETINADDRTOXNADDR_REQUEST_MSG   NETSYNC_MSG_USER + 380 + 1
#define XNETINADDRTOXNADDR_COMPLETE_MSG  NETSYNC_MSG_USER + 380 + 2

typedef struct _XNETINADDRTOXNADDR_REQUEST {
    DWORD    dwMessageId;
} XNETINADDRTOXNADDR_REQUEST, *PXNETINADDRTOXNADDR_REQUEST;

typedef struct _XNETINADDRTOXNADDR_COMPLETE {
    DWORD    dwMessageId;
    XNADDR   XnAddr;
} XNETINADDRTOXNADDR_COMPLETE, *PXNETINADDRTOXNADDR_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _XNETINADDRTOXNADDR_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL     bXnetInitialized;                        // bXnetInitialized indicates if the net subsystem is initialized
    BOOL     bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    BOOL     bRegistered;                             // bRegistered indicates if the security association is registered
    BOOL     bLocal;                                  // bLocal indicates if the local xnaddr is used
    BOOL     bInAddr;                                 // bInAddr indicates if the inaddr is valid
    BOOL     bXnAddr;                                 // bXnAddr indicates if the xnaddr is valid
    BOOL     bXnKid;                                  // bXnKid indicates if the xnkid is valid
    BOOL     bAgain;                                  // bAgain indicates if the process should be repeated
    DWORD    dwReturnCode;                            // dwReturnCode is the return code of the operation
    BOOL     bRIP;                                    // Specifies a RIP test case
} XNETINADDRTOXNADDR_TABLE, *PXNETINADDRTOXNADDR_TABLE;

static XNETINADDRTOXNADDR_TABLE XNetInAddrToXnAddrTable[] =
{
    { "38.1 Not Initialized", FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSANOTINITIALISED, FALSE },
    { "38.2 Not Registered",  TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSAEINVAL,         FALSE },
    { "38.3 Valid",           TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  FALSE, 0,                 FALSE },
    { "38.4 Again",           TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "38.5 Local",           TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, 0,                 FALSE },
    { "38.6 Zero inaddr",     TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, 0,                 TRUE  },
    { "38.7 NULL xnaddr",     TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,                 TRUE  },
    { "38.8 NULL xnkid",      TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,                 FALSE },
    { "38.9 Not Initialized", FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSANOTINITIALISED, FALSE }
};

#define XNetInAddrToXnAddrTableCount (sizeof(XNetInAddrToXnAddrTable) / sizeof(XNETINADDRTOXNADDR_TABLE))

NETSYNC_TYPE_THREAD  XNetInAddrToXnAddrTestSessionXbox =
{
    1,
    XNetInAddrToXnAddrTableCount,
    L"xnetapi_xbox.dll",
    "XNetInAddrToXnAddrTestServer"
};



VOID
XNetInAddrToXnAddrTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN LPSTR   lpszNetsyncRemote,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests XNetInAddrToXnAddr

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  lpszNetsyncRemote - pointer to the netsync remote address
  bRIPs - specifies RIP testing

------------------------------------------------------------------------------*/
{
    // lpszCaseTest is a pointer to the list of cases to test
    LPSTR                       lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR                       lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD                       dwTableIndex;

    // bXnetInitialized indicates if the net subsystem is initialized
    BOOL                        bXnetInitialized = FALSE;
    // WSAData is the details of the Winsock implementation
    WSADATA                     WSAData;

    // hNetsyncObject is a handle to the netsync object
    HANDLE                      hNetsyncObject = INVALID_HANDLE_VALUE;
    // NetsyncTypeSession is the session type descriptor
    NETSYNC_TYPE_THREAD         NetsyncTypeSession;
    // NetsyncInAddr is the address of the netsync server
    u_long                      NetsyncInAddr = 0;
    // LowPort is the low bound of the netsync port range
    u_short                     LowPort = 0;
    // HighPort is the high bound of the netsync port range
    u_short                     HighPort = 0;
    // CurrentPort is the current port in the netsync port range
    u_short                     CurrentPort = 0;
    // FromInAddr is the address of the netsync sender
    u_long                      FromInAddr;
    // dwMessageSize is the size of the received message
    DWORD                       dwMessageSize;
    // pMessage is a pointer to the received message
    char                        *pMessage;
    // XNetInAddrToXnAddrRequest is the request sent to the server
    XNETINADDRTOXNADDR_REQUEST  XNetInAddrToXnAddrRequest;

    // LocalXnAddr is the address of the client
    XNADDR                      LocalXnAddr;
    // ServerXnAddr is the address of the netsync server
    XNADDR                      ServerXnAddr;
    // ServerInAddr is the address of the netsync server
    IN_ADDR                     ServerInAddr;

    // XnKid1 is the first xnkid
    XNKID                       XnKid1;
    // XnKid2 is the second xnkid
    XNKID                       XnKid2;
    // XnKey is the xnkey
    XNKEY                       XnKey;
    // InAddr is the inaddr address
    IN_ADDR                     InAddr;
    // XnAddr1 is the first xnaddr address
    XNADDR                      XnAddr1;
    // XnAddr2 is the second xnaddr address
    XNADDR                      XnAddr2;

    // bException indicates if an exception occurred
    BOOL                        bException;
    // dwReturnCode is the return code of the operation
    int                         dwReturnCode;
    // bTestPassed indicates if the test passed
    BOOL                        bTestPassed;

    // szFunctionName is the function name
    CHAR                        szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    sprintf(szFunctionName, "XNetInAddrToXnAddr");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_XNetInAddrToXnAddr+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_XNetInAddrToXnAddr-");

    // Determine the remote netsync server type
    NetsyncTypeSession = XNetInAddrToXnAddrTestSessionXbox;

    for (dwTableIndex = 0; dwTableIndex < XNetInAddrToXnAddrTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, XNetInAddrToXnAddrTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, XNetInAddrToXnAddrTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != XNetInAddrToXnAddrTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, XNetInAddrToXnAddrTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == XNetInAddrToXnAddrTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of the net subsystem
        if (bXnetInitialized != XNetInAddrToXnAddrTable[dwTableIndex].bXnetInitialized) {
            // Initialize or terminate net subsystem as necessary
            if (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bXnetInitialized) {
                XNetAddRef();
            }
            else {
                XNetRelease();
            }

            // Update the state of net subsystem
            bXnetInitialized = XNetInAddrToXnAddrTable[dwTableIndex].bXnetInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bNetsyncConnected)) {
            // Connect to the session
            hNetsyncObject = NetsyncCreateClient((NULL != lpszNetsyncRemote) ? inet_addr(lpszNetsyncRemote) : 0, NETSYNC_SESSION_THREAD, &NetsyncTypeSession, &NetsyncInAddr, NULL, &LowPort, &HighPort);
            if (INVALID_HANDLE_VALUE == hNetsyncObject) {
                xLog(hLog, XLL_BLOCK, "Cannot create Netsync client - ec = %u", GetLastError());

                // End the variation
                xEndVariation(hLog);
                break;
            }

            CurrentPort = LowPort;

            // Get the server xnaddr
            XNetInAddrToXnAddrRequest.dwMessageId = XNETINADDRTOXNADDR_REQUEST_MSG;
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(XNetInAddrToXnAddrRequest), (char *) &XNetInAddrToXnAddrRequest);

            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            CopyMemory(&ServerXnAddr, &((XNETINADDRTOXNADDR_COMPLETE *) pMessage)->XnAddr, sizeof(ServerXnAddr));
            NetsyncFreeMessage(pMessage);
        }

        // Get the local xnaddr
        do {
            dwReturnCode = XNetGetTitleXnAddr(&LocalXnAddr);
            if (0 == dwReturnCode) {
                Sleep(SLEEP_ZERO_TIME);
            }
        } while (0 == dwReturnCode);

        // Clear the xnkid and xnkey
        ZeroMemory(&XnKid1, sizeof(XnKid1));
        ZeroMemory(&XnKid2, sizeof(XnKid2));
        ZeroMemory(&XnKey, sizeof(XnKey));

        // Register key pair
        XNetCreateKey(&XnKid1, &XnKey);
        XNetRegisterKey(&XnKid1, &XnKey);

        // Register security associations
        XNetXnAddrToInAddr(&ServerXnAddr, &XnKid1, &ServerInAddr);

        if (FALSE == XNetInAddrToXnAddrTable[dwTableIndex].bInAddr) {
            ZeroMemory(&InAddr, sizeof(InAddr));
        }
        else if (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bLocal) {
            InAddr.s_addr = htonl(INADDR_LOOPBACK);
        }
        else if (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bRegistered) {
            CopyMemory(&InAddr, &ServerInAddr, sizeof(InAddr));
        }
        else {
            InAddr.s_addr = INADDR_BROADCAST;
        }

        bTestPassed = TRUE;
        bException = FALSE;

        __try {
            // Call XNetInAddrToXnAddr
            dwReturnCode = XNetInAddrToXnAddr(InAddr, (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bXnAddr) ? &XnAddr1 : NULL, (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bXnKid) ? &XnKid2 : NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "XNetInAddrToXnAddr RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "XNetInAddrToXnAddr caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "XNetInAddrToXnAddr did not RIP");
            }

            if (dwReturnCode != XNetInAddrToXnAddrTable[dwTableIndex].dwReturnCode) {
                xLog(hLog, XLL_FAIL, "dwReturnCode - EXPECTED: %u; RECEIVED: %u", XNetInAddrToXnAddrTable[dwTableIndex].dwReturnCode, dwReturnCode);
            }
            else if (0 == dwReturnCode) {
                if (0 != memcmp((TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bLocal) ? &LocalXnAddr : &ServerXnAddr, &XnAddr1, sizeof(XnAddr1))) {
                    xLog(hLog, XLL_FAIL, "xnaddr difference");
                    bTestPassed = FALSE;
                }

                if ((FALSE == XNetInAddrToXnAddrTable[dwTableIndex].bLocal) && (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bXnKid)) {
                    if (0 != memcmp(&XnKid1, &XnKid2, sizeof(XnKid1))) {
                        xLog(hLog, XLL_FAIL, "xnkid difference");
                        bTestPassed = FALSE;
                    }
                }

                if (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bAgain) {
                    // Call XNetInAddrToXnAddr
                    dwReturnCode = XNetInAddrToXnAddr(InAddr, (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bXnAddr) ? &XnAddr2 : NULL, (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bXnKid) ? &XnKid2 : NULL);
                    if (0 != dwReturnCode) {
                        xLog(hLog, XLL_FAIL, "Second XNetInAddrToXnAddr - EXPECTED: %u; RECEIVED: %u", 0, dwReturnCode);
                        bTestPassed = FALSE;
                    }
                    else {
                        if (0 != memcmp(&XnAddr1, &XnAddr2, sizeof(XnAddr1))) {
                            xLog(hLog, XLL_FAIL, "Second xnaddr difference");
                            bTestPassed = FALSE;
                        }

                        if (TRUE == XNetInAddrToXnAddrTable[dwTableIndex].bXnKid) {
                            if (0 != memcmp(&XnKid1, &XnKid2, sizeof(XnKid1))) {
                                xLog(hLog, XLL_FAIL, "Second xnkid difference");
                                bTestPassed = FALSE;
                            }
                        }
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "XNetInAddrToXnAddr succeeded");
                }
            }
            else {
                xLog(hLog, XLL_PASS, "dwReturnCode - OUT: %u", dwReturnCode);
            }

        }

        // Unregister key pair
        XNetUnregisterKey(&XnKid1);

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
XNetInAddrToXnAddrTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests XNetInAddrToXnAddr - Server side

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
    u_long                       FromInAddr;
    // dwMessageType is the type of received message
    DWORD                        dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD                        dwMessageSize;
    // pMessage is a pointer to the received message
    char                         *pMessage;
    // XNetInAddrToXnAddrComplete is the result
    XNETINADDRTOXNADDR_COMPLETE  XNetInAddrToXnAddrComplete;

    // LocalXnAddr is the address of the local machine
    XNADDR                       LocalXnAddr;

    // dwReturnCode is the return code of the operation
    int                          dwReturnCode;



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
        if ((NETSYNC_MSGTYPE_SERVER == dwMessageType) && ((NETSYNC_MSG_STOPSESSION == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId) || (NETSYNC_MSG_DELETECLIENT == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId))) {
            NetsyncFreeMessage(pMessage);
            break;
        }

        // Send the server xnaddr
        XNetInAddrToXnAddrComplete.dwMessageId = XNETINADDRTOXNADDR_COMPLETE_MSG;
        CopyMemory(&XNetInAddrToXnAddrComplete.XnAddr, &LocalXnAddr, sizeof(XNetInAddrToXnAddrComplete.XnAddr));
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(XNetInAddrToXnAddrComplete), (char *) &XNetInAddrToXnAddrComplete);

        NetsyncFreeMessage(pMessage);
    }
}

#endif

#endif // _XBOX
