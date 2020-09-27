/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  XNetXnAddrToInAddr.c

Abstract:

  This modules tests XNetXnAddrToInAddr

Author:

  Steven Kehrli (steveke) 23-Jul-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



#ifdef _XBOX

using namespace XNetAPINamespace;

namespace XNetAPINamespace {

// XNetXnAddrToInAddr messages

#define XNETXNADDRTOINADDR_REQUEST_MSG   NETSYNC_MSG_USER + 370 + 1
#define XNETXNADDRTOINADDR_COMPLETE_MSG  NETSYNC_MSG_USER + 370 + 2

typedef struct _XNETXNADDRTOINADDR_REQUEST {
    DWORD    dwMessageId;
} XNETXNADDRTOINADDR_REQUEST, *PXNETXNADDRTOINADDR_REQUEST;

typedef struct _XNETXNADDRTOINADDR_COMPLETE {
    DWORD    dwMessageId;
    XNADDR   XnAddr;
} XNETXNADDRTOINADDR_COMPLETE, *PXNETXNADDRTOINADDR_COMPLETE;

} // namespace XNetAPINamespace



#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _XNETXNADDRTOINADDR_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL     bXnetInitialized;                        // bXnetInitialized indicates if the net subsystem is initialized
    BOOL     bNetsyncConnected;                       // bNetsyncConnected indicates if the netsync client is connected
    BOOL     bRegistered;                             // bRegistered indicates if the key pair is registered
    BOOL     bLocal;                                  // bLocal indicates if the local xnaddr is used
    BOOL     bMaxSecReg;                              // bMaxSecReg indicates if the maximum security associations are registered
    BOOL     bXnAddr;                                 // bXnAddr indicates if the xnaddr is valid
    BOOL     bXnKid;                                  // bXnKid indicates if the xnkid is valid
    BOOL     bInAddr;                                 // bInAddr indicates if the inaddr is valid
    BOOL     bAgain;                                  // bAgain indicates if the process should be repeated
    DWORD    dwReturnCode;                            // dwReturnCode is the return code of the operation
    BOOL     bRIP;                                    // Specifies a RIP test case
} XNETXNADDRTOINADDR_TABLE, *PXNETXNADDRTOINADDR_TABLE;

static XNETXNADDRTOINADDR_TABLE XNetXnAddrToInAddrTable[] =
{
    { "37.1 Not Initialized",  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSANOTINITIALISED, FALSE },
    { "37.2 Not Registered",   TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSAEINVAL,         FALSE },
    { "37.3 Valid",            TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, 0,                 FALSE },
    { "37.4 Again",            TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "37.5 Local",            TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  FALSE, 0,                 FALSE },
    { "37.6 Max Sec Assoc",    TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, WSAENOMORE,        FALSE },
    { "37.7 NULL xnaddr",      TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, 0,                 TRUE  },
    { "37.8 NULL xnkid",       TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, 0,                 TRUE  },
    { "37.9 NULL inaddr",      TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, 0,                 TRUE  },
    { "37.10 Not Initialized", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, WSANOTINITIALISED, FALSE }
};

#define XNetXnAddrToInAddrTableCount (sizeof(XNetXnAddrToInAddrTable) / sizeof(XNETXNADDRTOINADDR_TABLE))

NETSYNC_TYPE_THREAD  XNetXnAddrToInAddrTestSessionXbox =
{
    1,
    XNetXnAddrToInAddrTableCount,
    L"xnetapi_xbox.dll",
    "XNetXnAddrToInAddrTestServer"
};



VOID
XNetXnAddrToInAddrTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN LPSTR   lpszNetsyncRemote,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests XNetXnAddrToInAddr

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
    // XNetXnAddrToInAddrRequest is the request sent to the server
    XNETXNADDRTOINADDR_REQUEST  XNetXnAddrToInAddrRequest;

    // LocalXnAddr is the address of the client
    XNADDR                      LocalXnAddr;
    // ServerXnAddr is the address of the netsync server
    XNADDR                      ServerXnAddr;

    // XnKid is the xnkid
    XNKID                       XnKid;
    // XnKey is the xnkey
    XNKEY                       XnKey;
    // XnAddr is the xnaddr address
    XNADDR                      XnAddr;
    // InAddr1 is the first inaddr address
    IN_ADDR                     InAddr1;
    // InAddr2 is the second inaddr address
    IN_ADDR                     InAddr2;

    // bException indicates if an exception occurred
    BOOL                        bException;
    // dwReturnCode is the return code of the operation
    int                         dwReturnCode;
    // bTestPassed indicates if the test passed
    BOOL                        bTestPassed;

    // szFunctionName is the function name
    CHAR                        szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    sprintf(szFunctionName, "XNetXnAddrToInAddr");
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_XNetXnAddrToInAddr+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_XNetXnAddrToInAddr-");

    // Determine the remote netsync server type
    NetsyncTypeSession = XNetXnAddrToInAddrTestSessionXbox;

    for (dwTableIndex = 0; dwTableIndex < XNetXnAddrToInAddrTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, XNetXnAddrToInAddrTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, XNetXnAddrToInAddrTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != XNetXnAddrToInAddrTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, XNetXnAddrToInAddrTable[dwTableIndex].szVariationName);

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE != hNetsyncObject) && (FALSE == XNetXnAddrToInAddrTable[dwTableIndex].bNetsyncConnected)) {
            // Close the netsync client object
            NetsyncCloseClient(hNetsyncObject);
            hNetsyncObject = INVALID_HANDLE_VALUE;
        }

        // Check the state of the net subsystem
        if (bXnetInitialized != XNetXnAddrToInAddrTable[dwTableIndex].bXnetInitialized) {
            // Initialize or terminate net subsystem as necessary
            if (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bXnetInitialized) {
                XNetAddRef();
            }
            else {
                XNetRelease();
            }

            // Update the state of net subsystem
            bXnetInitialized = XNetXnAddrToInAddrTable[dwTableIndex].bXnetInitialized;
        }

        // Check the state of Netsync
        if ((INVALID_HANDLE_VALUE == hNetsyncObject) && (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bNetsyncConnected)) {
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
            XNetXnAddrToInAddrRequest.dwMessageId = XNETXNADDRTOINADDR_REQUEST_MSG;
            NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(XNetXnAddrToInAddrRequest), (char *) &XNetXnAddrToInAddrRequest);

            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &FromInAddr, NULL, &dwMessageSize, &pMessage);
            CopyMemory(&ServerXnAddr, &((XNETXNADDRTOINADDR_COMPLETE *) pMessage)->XnAddr, sizeof(ServerXnAddr));
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
        ZeroMemory(&XnKid, sizeof(XnKid));
        ZeroMemory(&XnKey, sizeof(XnKey));
        XNetCreateKey(&XnKid, &XnKey);

        // Register key pair
        if (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bRegistered) {
            XNetRegisterKey(&XnKid, &XnKey);
        }

        // Register security associations
        if (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bMaxSecReg) {
            ZeroMemory(&XnAddr, sizeof(XnAddr));
            do {
                // Generate random xnadrr
                XNetRandom(XnAddr.abEnet, sizeof(XnAddr.abEnet));

                // Call XNetXnAddrToInAddr
                dwReturnCode = XNetXnAddrToInAddr(&XnAddr, &XnKid, &InAddr1);
            } while (0 == dwReturnCode);
        }

        if (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bLocal) {
            CopyMemory(&XnAddr, &LocalXnAddr, sizeof(XnAddr));
        }
        else {
            CopyMemory(&XnAddr, &ServerXnAddr, sizeof(XnAddr));
        }

        bTestPassed = TRUE;
        bException = FALSE;

        __try {
            // Call XNetXnAddrToInAddr
            dwReturnCode = XNetXnAddrToInAddr((TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bXnAddr) ? &XnAddr : NULL, (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bXnKid) ? &XnKid : NULL, (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bInAddr) ? &InAddr1 : NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "XNetXnAddrToInAddr RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "XNetXnAddrToInAddr caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "XNetXnAddrToInAddr did not RIP");
            }

            if (dwReturnCode != XNetXnAddrToInAddrTable[dwTableIndex].dwReturnCode) {
                xLog(hLog, XLL_FAIL, "dwReturnCode - EXPECTED: %u; RECEIVED: %u", XNetXnAddrToInAddrTable[dwTableIndex].dwReturnCode, dwReturnCode);
            }
            else if (0 == dwReturnCode) {
                if (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bLocal) {
                    if (htonl(INADDR_LOOPBACK) != InAddr1.s_addr) {
                        xLog(hLog, XLL_FAIL, "in_addr - EXPECTED: 127.0.0.1; RECEIVED: %u.%u.%u.%u", InAddr1.S_un.S_un_b.s_b1, InAddr1.S_un.S_un_b.s_b2, InAddr1.S_un.S_un_b.s_b3, InAddr1.S_un.S_un_b.s_b4);
                        bTestPassed = FALSE;
                    }
                }
                else {
                    if (0 != InAddr1.S_un.S_un_b.s_b1) {
                        xLog(hLog, XLL_FAIL, "in_addr - EXPECTED: 0.b.c.d; RECEIVED: %u.%u.%u.%u", InAddr1.S_un.S_un_b.s_b1, InAddr1.S_un.S_un_b.s_b2, InAddr1.S_un.S_un_b.s_b3, InAddr1.S_un.S_un_b.s_b4);
                        bTestPassed = FALSE;
                    }
                }

                if (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bAgain) {
                    // Call XNetXnAddrToInAddr
                    dwReturnCode = XNetXnAddrToInAddr((TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bXnAddr) ? &XnAddr : NULL, (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bXnKid) ? &XnKid : NULL, (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bInAddr) ? &InAddr2 : NULL);
                    if (0 != dwReturnCode) {
                        xLog(hLog, XLL_FAIL, "Second XNetXnAddrToInAddr - EXPECTED: %u; RECEIVED: %u", 0, dwReturnCode);
                        bTestPassed = FALSE;
                    }
                    else if (0 != memcmp(&InAddr1, &InAddr2, sizeof(InAddr1))) {
                        xLog(hLog, XLL_FAIL, "Second in_addr - EXPECTED: %u.%u.%u.%u; RECEIVED: %u.%u.%u.%u", InAddr1.S_un.S_un_b.s_b1, InAddr1.S_un.S_un_b.s_b2, InAddr1.S_un.S_un_b.s_b3, InAddr1.S_un.S_un_b.s_b4, InAddr2.S_un.S_un_b.s_b1, InAddr2.S_un.S_un_b.s_b2, InAddr2.S_un.S_un_b.s_b3, InAddr2.S_un.S_un_b.s_b4);
                        bTestPassed = FALSE;
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "XNetXnAddrToInAddr succeeded");
                }
            }
            else {
                xLog(hLog, XLL_PASS, "dwReturnCode - OUT: %u", dwReturnCode);
            }

        }

        // Unregister key pair
        if (TRUE == XNetXnAddrToInAddrTable[dwTableIndex].bRegistered) {
            XNetUnregisterKey(&XnKid);
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
XNetXnAddrToInAddrTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests XNetXnAddrToInAddr - Server side

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
    // XNetXnAddrToInAddrComplete is the result
    XNETXNADDRTOINADDR_COMPLETE  XNetXnAddrToInAddrComplete;

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
        XNetXnAddrToInAddrComplete.dwMessageId = XNETXNADDRTOINADDR_COMPLETE_MSG;
        CopyMemory(&XNetXnAddrToInAddrComplete.XnAddr, &LocalXnAddr, sizeof(XNetXnAddrToInAddrComplete.XnAddr));
        NetsyncSendClientMessage(hNetsyncObject, FromInAddr, sizeof(XNetXnAddrToInAddrComplete), (char *) &XNetXnAddrToInAddrComplete);

        NetsyncFreeMessage(pMessage);
    }
}

#endif

#endif // _XBOX
