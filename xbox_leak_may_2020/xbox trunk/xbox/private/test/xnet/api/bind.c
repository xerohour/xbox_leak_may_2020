/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module bind:

  bind.c

Abstract:

  This modules tests bind

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

#define NAME_ANY       0
#define NAME_ANYADDR   1
#define NAME_ANYPORT   2
#define NAME_LOCAL     3
#define NAME_LOOPBACK  4
#define NAME_BROADCAST 5
#define NAME_MULTICAST 6
#define NAME_UNAVAIL   7
#define NAME_BAD       8
#define NAME_NULL      9



typedef struct _BIND_TABLE {
    CHAR         szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL         bWinsockInitialized;                     // bWinsockInitialized indicates if Winsock is initialized
    DWORD        dwSocket1;                               // dwSocket1 indicates the first socket to be created
    DWORD        dwSocket2;                               // dwSocket2 indicates the second socket to be created
    BOOL         bBind;                                   // bBind indicates if socket is bound
    BOOL         bAddrInUse;                              // bAddrInUse indicates if address is in use
    BOOL         bReuseAddr;                              // bReuseAddr indicates if address reuse is enabled
    DWORD        dwName;                                  // dwName indicates the address
    int          bindnamelen;                             // bindnamelen is the length of the bindname buffer
    int          iReturnCode;                             // iReturnCode is the return code of bind
    int          iLastError;                              // iLastError is the error code if the operation failed
    BOOL         bRIP;                                    // Specifies a RIP test case
} BIND_TABLE, *PBIND_TABLE;

static BIND_TABLE bindTable[] =
{
    { "9.1 Not Initialized",       FALSE, SOCKET_INVALID_SOCKET,      0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSANOTINITIALISED, FALSE },
    { "9.2 s = INT_MIN",           TRUE,  SOCKET_INT_MIN,             0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "9.3 s = -1",                TRUE,  SOCKET_NEG_ONE,             0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "9.4 s = 0",                 TRUE,  SOCKET_ZERO,                0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "9.5 s = INT_MAX",           TRUE,  SOCKET_INT_MAX,             0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "9.6 s = INVALID_SOCKET",    TRUE,  SOCKET_INVALID_SOCKET,      0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "9.7 Bcast Address TCP",     TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_BROADCAST, sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.8 Bcast Address UDP",     TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_BROADCAST, sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.9 Mcast Address TCP",     TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_MULTICAST, sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.10 Mcast Address UDP",    TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_MULTICAST, sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.11 Unavail Address TCP",  TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_UNAVAIL,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.12 Unavail Address UDP",  TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_UNAVAIL,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.13 Bad Address TCP",      TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_BAD,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.14 Bad Address UDP",      TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_BAD,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.15 NULL Address TCP",     TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_NULL,      sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.16 NULL Address UDP",     TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_NULL,      sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.17 Local Address TCP",    TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_LOCAL,     sizeof(SOCKADDR_IN),     0,            WSAEFAULT,         TRUE  },
    { "9.18 Local Address UDP",    TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_LOCAL,     sizeof(SOCKADDR_IN),     0,            WSAEFAULT,         TRUE  },
    { "9.19 Loopback Address TCP", TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            WSAEFAULT,         TRUE  },
    { "9.20 Loopback Address UDP", TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_LOOPBACK,  sizeof(SOCKADDR_IN),     0,            WSAEFAULT,         TRUE  },
    { "9.21 Any Address TCP",      TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "9.22 Any Address UDP",      TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "9.23 Any Port TCP",         TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_ANYPORT,   sizeof(SOCKADDR_IN),     0,            WSAEFAULT,         TRUE  },
    { "9.24 Any Port UDP",         TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_ANYPORT,   sizeof(SOCKADDR_IN),     0,            WSAEFAULT,         TRUE  },
    { "9.25 Any TCP",              TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "9.26 Any UDP",              TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "9.27 Bound Socket TCP",     TRUE,  SOCKET_TCP,                 0,          TRUE,  FALSE, FALSE, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "9.28 Bound Socket UDP",     TRUE,  SOCKET_UDP,                 0,          TRUE,  FALSE, FALSE, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEINVAL,         FALSE },
    { "9.29 Address In Use TCP",   TRUE,  SOCKET_TCP,                 SOCKET_TCP, FALSE, TRUE,  FALSE, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRINUSE,     FALSE },
    { "9.30 Address In Use UDP",   TRUE,  SOCKET_UDP,                 SOCKET_UDP, FALSE, TRUE,  FALSE, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAEADDRINUSE,     FALSE },
    { "9.31 Reuse Address TCP",    TRUE,  SOCKET_TCP,                 SOCKET_TCP, FALSE, TRUE,  TRUE,  NAME_ANYADDR,   sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "9.32 Reuse Address UDP",    TRUE,  SOCKET_UDP,                 SOCKET_UDP, FALSE, TRUE,  TRUE,  NAME_ANYADDR,   sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "9.33 TCP and UDP",          TRUE,  SOCKET_TCP,                 SOCKET_UDP, FALSE, FALSE, FALSE, NAME_ANYADDR,   sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "9.34 Large Length TCP",     TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "9.35 Large Length UDP",     TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN) + 1, 0,            0,                 FALSE },
    { "9.36 Exact Length TCP",     TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "9.37 Exact Length UDP",     TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     0,            0,                 FALSE },
    { "9.38 Small Length TCP",     TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN) - 1, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.39 Small Length UDP",     TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN) - 1, SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.40 Zero Length TCP",      TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_ANY,       0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.41 Zero Length UDP",      TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_ANY,       0,                       SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.42 Negative Length TCP",  TRUE,  SOCKET_TCP,                 0,          FALSE, FALSE, FALSE, NAME_ANY,       -1,                      SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.43 Negative Length UDP",  TRUE,  SOCKET_UDP,                 0,          FALSE, FALSE, FALSE, NAME_ANY,       -1,                      SOCKET_ERROR, WSAEFAULT,         TRUE  },
    { "9.44 Closed Socket TCP",    TRUE,  SOCKET_TCP | SOCKET_CLOSED, 0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "9.45 Closed Socket UDP",    TRUE,  SOCKET_UDP | SOCKET_CLOSED, 0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSAENOTSOCK,       FALSE },
    { "9.46 Not Initialized",      FALSE, SOCKET_INVALID_SOCKET,      0,          FALSE, FALSE, FALSE, NAME_ANY,       sizeof(SOCKADDR_IN),     SOCKET_ERROR, WSANOTINITIALISED, FALSE }
};

#define bindTableCount (sizeof(bindTable) / sizeof(BIND_TABLE))



VOID
bindTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN WORD    WinsockVersion,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests bind

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  WinsockVersion - requested version of Winsock
  bRIPs - specifies RIP testing

------------------------------------------------------------------------------*/
{
    // lpszCaseTest is a pointer to the list of cases to test
    LPSTR           lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR           lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD           dwTableIndex;

    // bWinsockInitialized indicates if Winsock is initialized
    BOOL            bWinsockInitialized = FALSE;
    // WSAData is the details of the Winsock implementation
    WSADATA         WSAData;

    // HostXnAddr is the host xnet address
    XNADDR          HostXnAddr;
    // hostaddr is the local host address
    u_long          hostaddr = 0;

    // sSocket1 is the first socket descriptor
    SOCKET          sSocket1;
    // sSocket2 is the second socket descriptor
    SOCKET          sSocket2;

    // bindport1 is the first bind port
    u_short         bindport1;
    // bindport2 is the second bind port
    u_short         bindport2;

    // localname is the local address
    SOCKADDR_IN     localname;

    // bException indicates if an exception occurred
    BOOL            bException;
    // iReturnCode is the return code of the operation
    int             iReturnCode;
    // dwReturnCode is the return code of the operation
    DWORD           dwReturnCode;
    // iLastError is the error code if the operation failed
    int             iLastError;

    // szFunctionName is the function name
    CHAR            szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    sprintf(szFunctionName, "bind v%04x", WinsockVersion);
    xSetFunctionName(hLog, szFunctionName);

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_bind+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_bind-");

    // Initialize the net subsystem
    XNetAddRef();

    // Get the local xnaddr
    do {
        dwReturnCode = XNetGetTitleXnAddr(&HostXnAddr);
        if (0 == dwReturnCode) {
            Sleep(SLEEP_ZERO_TIME);
        }
    } while (0 == dwReturnCode);
    hostaddr = HostXnAddr.ina.s_addr;

    // Get the bind port for testing
    bindport1 = 65533;
    bindport2 = 65532;

    for (dwTableIndex = 0; dwTableIndex < bindTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, bindTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, bindTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != bindTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, bindTable[dwTableIndex].szVariationName);

        // Check the state of Winsock
        if (bWinsockInitialized != bindTable[dwTableIndex].bWinsockInitialized) {
            // Initialize or terminate Winsock as necessary
            if (TRUE == bindTable[dwTableIndex].bWinsockInitialized) {
                WSAStartup(WinsockVersion, &WSAData);
            }
            else {
                WSACleanup();
            }

            // Update the state of Winsock
            bWinsockInitialized = bindTable[dwTableIndex].bWinsockInitialized;
        }

        // Create socket 2
        sSocket2 = INVALID_SOCKET;
        if (0 != (SOCKET_TCP & bindTable[dwTableIndex].dwSocket2)) {
            sSocket2 = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & bindTable[dwTableIndex].dwSocket2)) {
            sSocket2 = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Bind socket 2
        if (INVALID_SOCKET != sSocket2) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;

            if (TRUE == bindTable[dwTableIndex].bAddrInUse) {
                localname.sin_port = htons(bindport1);
            }
            else {
                localname.sin_port = htons(bindport2);
            }

            bind(sSocket2, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Create socket 1
        sSocket1 = INVALID_SOCKET;
        if (SOCKET_INT_MIN == bindTable[dwTableIndex].dwSocket1) {
            sSocket1 = INT_MIN;
        }
        else if (SOCKET_NEG_ONE == bindTable[dwTableIndex].dwSocket1) {
            sSocket1 = -1;
        }
        else if (SOCKET_ZERO == bindTable[dwTableIndex].dwSocket1) {
            sSocket1 = 0;
        }
        else if (SOCKET_INT_MAX == bindTable[dwTableIndex].dwSocket1) {
            sSocket1 = INT_MAX;
        }
        else if (SOCKET_INVALID_SOCKET == bindTable[dwTableIndex].dwSocket1) {
            sSocket1 = INVALID_SOCKET;
        }
        else if (0 != (SOCKET_TCP & bindTable[dwTableIndex].dwSocket1)) {
            sSocket1 = socket(AF_INET, SOCK_STREAM, 0);
        }
        else if (0 != (SOCKET_UDP & bindTable[dwTableIndex].dwSocket1)) {
            sSocket1 = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Bind socket 1
        if (TRUE == bindTable[dwTableIndex].bBind) {
            ZeroMemory(&localname, sizeof(localname));
            localname.sin_family = AF_INET;
            localname.sin_port = htons(bindport1);

            bind(sSocket1, (SOCKADDR *) &localname, sizeof(localname));
        }

        // Enable address reuse
        if (TRUE == bindTable[dwTableIndex].bReuseAddr) {
            setsockopt(sSocket1, SOL_SOCKET, SO_REUSEADDR, (char *) &bindTable[dwTableIndex].bReuseAddr, sizeof(bindTable[dwTableIndex].bReuseAddr));
        }

        // Set the bind name
        ZeroMemory(&localname, sizeof(localname));
        localname.sin_family = AF_INET;

        if (NAME_ANYADDR == bindTable[dwTableIndex].dwName) {
            localname.sin_port = htons(bindport1);
        }
        else if (NAME_ANYPORT == bindTable[dwTableIndex].dwName) {
            localname.sin_addr.s_addr = hostaddr;
        }
        else if (NAME_LOCAL == bindTable[dwTableIndex].dwName) {
            localname.sin_addr.s_addr = hostaddr;
            localname.sin_port = htons(bindport1);
        }
        else if (NAME_LOOPBACK == bindTable[dwTableIndex].dwName) {
            localname.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            localname.sin_port = htons(bindport1);
        }
        else if (NAME_BROADCAST == bindTable[dwTableIndex].dwName) {
            localname.sin_addr.s_addr = htonl(INADDR_BROADCAST);
            localname.sin_port = htons(bindport1);
        }
        else if (NAME_MULTICAST == bindTable[dwTableIndex].dwName) {
            localname.sin_addr.s_addr = inet_addr("224.0.0.0");
            localname.sin_port = htons(bindport1);
        }
        else if (NAME_UNAVAIL == bindTable[dwTableIndex].dwName) {
            localname.sin_addr.s_addr = inet_addr("127.0.0.0");
            localname.sin_port = htons(bindport1);
        }
        else if (NAME_BAD == bindTable[dwTableIndex].dwName) {
            localname.sin_family = AF_UNIX;
            localname.sin_addr.s_addr = hostaddr;
            localname.sin_port = htons(bindport1);
        }

        bException = FALSE;

        // Close socket 1
        if (0 != (SOCKET_CLOSED & bindTable[dwTableIndex].dwSocket1)) {
            closesocket(sSocket1);
        }

        __try {
            // Call bind
            iReturnCode = bind(sSocket1, (NAME_NULL != bindTable[dwTableIndex].dwName) ? (SOCKADDR *) &localname : NULL, bindTable[dwTableIndex].bindnamelen);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == bindTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "bind RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "bind caused an exception - ec = 0x%08x", GetExceptionCode());
            }

            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == bindTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "bind did not RIP");
            }

            if ((SOCKET_ERROR == iReturnCode) && (SOCKET_ERROR == bindTable[dwTableIndex].iReturnCode)) {
                // Get the last error code
                iLastError = WSAGetLastError();

                if (iLastError != bindTable[dwTableIndex].iLastError) {
                    xLog(hLog, XLL_FAIL, "iLastError - EXPECTED: %u; RECEIVED: %u", bindTable[dwTableIndex].iLastError, iLastError);
                }
                else {
                    xLog(hLog, XLL_PASS, "iLastError - OUT: %u", iLastError);
                }
            }
            else if (SOCKET_ERROR == iReturnCode) {
                xLog(hLog, XLL_FAIL, "bind returned SOCKET_ERROR - ec = %u", WSAGetLastError());
            }
            else if (SOCKET_ERROR == bindTable[dwTableIndex].iReturnCode) {
                xLog(hLog, XLL_FAIL, "bind returned non-SOCKET_ERROR");
            }
            else {
                xLog(hLog, XLL_PASS, "bind succeeded");
            }
        }

        // Close socket 2
        if (INVALID_SOCKET != sSocket2) {
            closesocket(sSocket2);
        }

        // Close socket 1
        if (0 == (SOCKET_CLOSED & bindTable[dwTableIndex].dwSocket1)) {
            if ((0 != (SOCKET_TCP & bindTable[dwTableIndex].dwSocket1)) || (0 != (SOCKET_UDP & bindTable[dwTableIndex].dwSocket1))) {
                closesocket(sSocket1);
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

#endif
