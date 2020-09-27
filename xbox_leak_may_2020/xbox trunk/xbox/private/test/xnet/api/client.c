/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  client.c

Abstract:

  Entry points for xnetapi.dll

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

VOID
WINAPI
StartTest(
    IN HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Test entry point

Arguments:

  hLog - handle to the xLog log object

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // hMemObject is the mem object
    HANDLE  hMemObject = INVALID_HANDLE_VALUE;

    // lpszNetsyncRemoteNt is a pointer to the remote nt netsync server
    LPSTR   lpszNetsyncRemoteNt = NULL;
    // lpszNetsyncRemoteXbox is a pointer to the remote xbox netsync server
    LPSTR   lpszNetsyncRemoteXbox = NULL;

    // lpszAPITest is a pointer to the list of APIs to test
    LPSTR   lpszAPITest = NULL;
    // lpszAPISkip is a pointer to the list of APIs to skip
    LPSTR   lpszAPISkip = NULL;
    // lpszVersions is a pointer to the list of versions to test
    LPSTR   lpszVersions = NULL;
    // lpszRemotes is a pointer to the list of remotes to test vs.
    LPSTR   lpszRemotes = NULL;

    // bVersion11 is a flag to specify testing of Winsock 1.1
    BOOL    bVersion11 = TRUE;
    // bVersion22 is a flag to specify testing of Winsock 2.2
    BOOL    bVersion22 = TRUE;

    // bRemoteNt is a flag to specify remote testing vs. nt
    BOOL    bRemoteNt = TRUE;
    // bRemoteXbox is a flag to specify remote testing vs. xbox
    BOOL    bRemoteXbox = TRUE;

    // bRIPs is a flag to specify RIP testing
    BOOL    bRIPs = FALSE;



    // Create the private heap
    hMemObject = xMemCreate();
    if (INVALID_HANDLE_VALUE == hMemObject) {
        return;
    }

    // Set the component and subcomponent
    xSetComponent(hLog, "Network", "XNet");

    // Get the remote netsync addr
    lpszNetsyncRemoteNt = GetIniString(hMemObject, "xnetapi", "NetsyncRemoteNt");
    lpszNetsyncRemoteXbox = GetIniString(hMemObject, "xnetapi", "NetsyncRemoteXbox");

    // Get the APIs
    lpszAPITest = GetIniSection(hMemObject, "xnetapi_apis+");
    lpszAPISkip = GetIniSection(hMemObject, "xnetapi_apis-");

    // Get the versions
    lpszVersions = GetIniSection(hMemObject, "xnetapi_versions");
    if (NULL != lpszVersions) {
        // Check for version 1.1
        bVersion11 = FindString(lpszVersions, "1.1");

        // Check for version 2.2
        bVersion22 = FindString(lpszVersions, "2.2");
    }

    // Get the remotes
    lpszRemotes = GetIniSection(hMemObject, "xnetapi_remotes");
    if (NULL != lpszRemotes) {
        // Check for remote nt
        bRemoteNt = FindString(lpszRemotes, "nt");

        // Check for remote xbox
        bRemoteXbox = FindString(lpszRemotes, "xbox");
    }

    // Get the RIPs
    bRIPs = (BOOL) GetProfileIntA("xnetapi", "RIPs", 0);

    // Test htonl()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "htonl"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "htonl")))) {
        htonlTest(hLog, hMemObject, bRIPs);
    }

    // Test htons()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "htons"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "htons")))) {
        htonsTest(hLog, hMemObject, bRIPs);
    }

    // Test ntohl()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "ntohl"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "ntohl")))) {
        ntohlTest(hLog, hMemObject, bRIPs);
    }

    // Test ntohs()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "ntohs"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "ntohs")))) {
        ntohsTest(hLog, hMemObject, bRIPs);
    }

    // Test inet_addr()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "inet_addr"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "inet_addr")))) {
        inet_addrTest(hLog, hMemObject, bRIPs);
    }

    // Test WSAStartup()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "WSAStartup"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "WSAStartup")))) {
        WSAStartupTest(hLog, hMemObject, bRIPs);
    }

    // Test WSACleanup()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "WSACleanup"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "WSACleanup")))) {
        if (TRUE == bVersion11) {
            WSACleanupTest(hLog, hMemObject, MAKEWORD(1, 1), bRIPs);
        }

        if (TRUE == bVersion22) {
            WSACleanupTest(hLog, hMemObject, MAKEWORD(2, 2), bRIPs);
        }
    }

    // Test socket()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "socket"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "socket")))) {
        if (TRUE == bVersion11) {
            socketTest(hLog, hMemObject, MAKEWORD(1, 1), bRIPs);
        }

        if (TRUE == bVersion22) {
            socketTest(hLog, hMemObject, MAKEWORD(2, 2), bRIPs);
        }
    }

    // Test bind()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "bind"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "bind")))) {
        if (TRUE == bVersion11) {
            bindTest(hLog, hMemObject, MAKEWORD(1, 1), bRIPs);
        }

        if (TRUE == bVersion22) {
            bindTest(hLog, hMemObject, MAKEWORD(2, 2), bRIPs);
        }
    }

    // Test listen()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "listen"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "listen")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                listenTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                listenTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                listenTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                listenTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test accept()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "accept"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "accept")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                acceptTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                acceptTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                acceptTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                acceptTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test connect()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "connect"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "connect")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                connectTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                connectTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                connectTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                connectTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test send()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "send"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "send")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                sendTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                sendTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                sendTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                sendTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test WSASend()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "WSASend"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "WSASend")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                WSASendTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                WSASendTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                WSASendTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                WSASendTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test sendto()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "sendto"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "sendto")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                sendtoTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                sendtoTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                sendtoTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                sendtoTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test WSASendTo()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "WSASendTo"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "WSASendTo")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                WSASendToTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                WSASendToTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                WSASendToTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                WSASendToTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test recv()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "recv"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "recv")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                recvTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                recvTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                recvTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                recvTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test WSARecv()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "WSARecv"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "WSARecv")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                WSARecvTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                WSARecvTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                WSARecvTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                WSARecvTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test recvfrom()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "recvfrom"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "recvfrom")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                recvfromTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                recvfromTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                recvfromTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                recvfromTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test WSARecvFrom()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "WSARecvFrom"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "WSARecvFrom")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                WSARecvFromTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                WSARecvFromTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                WSARecvFromTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                WSARecvFromTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test getsockname()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "getsockname"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "getsockname")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                getsocknameTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                getsocknameTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                getsocknameTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                getsocknameTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test getpeername()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "getpeername"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "getpeername")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                getpeernameTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                getpeernameTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                getpeernameTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                getpeernameTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test ioctlsocket()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "ioctlsocket"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "ioctlsocket")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                ioctlsocketTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                ioctlsocketTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                ioctlsocketTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                ioctlsocketTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test WSAGetOverlappedResult()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "WSAGetOverlappedResult"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "WSAGetOverlappedResult")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                WSAGetOverlappedResultTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                WSAGetOverlappedResultTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                WSAGetOverlappedResultTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                WSAGetOverlappedResultTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test WSACancelOverlappedIO()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "WSACancelOverlappedIO"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "WSACancelOverlappedIO")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                WSACancelOverlappedIOTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                WSACancelOverlappedIOTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                WSACancelOverlappedIOTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                WSACancelOverlappedIOTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test select()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "select"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "select")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                selectTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                selectTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                selectTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                selectTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test fds macros
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "fdsmacros"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "fdsmacros")))) {
        fdsmacrosTest(hLog, hMemObject, bRIPs);
    }

    // Test shutdown()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "shutdown"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "shutdown")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                shutdownTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                shutdownTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                shutdownTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                shutdownTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test closesocket()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "closesocket"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "closesocket")))) {
        if (TRUE == bRemoteNt) {
            if (TRUE == bVersion11) {
                closesocketTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }

            if (TRUE == bVersion22) {
                closesocketTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteNt, VS_NT, bRIPs);
            }
        }

        if (TRUE == bRemoteXbox) {
            if (TRUE == bVersion11) {
                closesocketTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }

            if (TRUE == bVersion22) {
                closesocketTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, VS_XBOX, bRIPs);
            }
        }
    }

    // Test getsockopt()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "getsockopt"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "getsockopt")))) {
        if (TRUE == bVersion11) {
            getsockoptTest(hLog, hMemObject, MAKEWORD(1, 1), bRIPs);
        }

        if (TRUE == bVersion22) {
            getsockoptTest(hLog, hMemObject, MAKEWORD(2, 2), bRIPs);
        }
    }

    // Test setsockopt()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "setsockopt"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "setsockopt")))) {
        if (TRUE == bVersion11) {
            setsockoptTest(hLog, hMemObject, MAKEWORD(1, 1), bRIPs);
        }

        if (TRUE == bVersion22) {
            setsockoptTest(hLog, hMemObject, MAKEWORD(2, 2), bRIPs);
        }
    }

    // Test XNetInAddrToString()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "XNetInAddrToString"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "XNetInAddrToString")))) {
        XNetInAddrToStringTest(hLog, hMemObject, bRIPs);
    }

    // Test XNetRandom()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "XNetRandom"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "XNetRandom")))) {
        XNetRandomTest(hLog, hMemObject, bRIPs);
    }

    // Test XNetCreateKey()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "XNetCreateKey"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "XNetCreateKey")))) {
        XNetCreateKeyTest(hLog, hMemObject, bRIPs);
    }

    if (TRUE == bRemoteXbox) {
        // Test XNetRegisterKey()
        if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "XNetRegisterKey"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "XNetRegisterKey")))) {
            if (TRUE == bVersion11) {
                XNetRegisterKeyTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, bRIPs);
            }

            if (TRUE == bVersion22) {
                XNetRegisterKeyTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, bRIPs);
            }
        }

        // Test XNetUnregisterKey()
        if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "XNetUnregisterKey"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "XNetUnregisterKey")))) {
            if (TRUE == bVersion11) {
                XNetUnregisterKeyTest(hLog, hMemObject, MAKEWORD(1, 1), lpszNetsyncRemoteXbox, bRIPs);
            }

            if (TRUE == bVersion22) {
                XNetUnregisterKeyTest(hLog, hMemObject, MAKEWORD(2, 2), lpszNetsyncRemoteXbox, bRIPs);
            }
        }

        // Test XNetXnAddrToInAddr()
        if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "XNetXnAddrToInAddr"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "XNetXnAddrToInAddr")))) {
            XNetXnAddrToInAddrTest(hLog, hMemObject, lpszNetsyncRemoteXbox, bRIPs);
        }

        // Test XNetInAddrToXnAddr()
        if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "XNetInAddrToXnAddr"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "XNetInAddrToXnAddr")))) {
            XNetInAddrToXnAddrTest(hLog, hMemObject, lpszNetsyncRemoteXbox, bRIPs);
        }
    }

    // Test XNetDnsLookup()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "XNetDnsLookup"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "XNetDnsLookup")))) {
        XNetDnsLookupTest(hLog, hMemObject, bRIPs);
    }

    // Test XNetDnsRelease()
    if (((NULL == lpszAPISkip) || (FALSE == FindString(lpszAPISkip, "XNetDnsRelease"))) && ((NULL == lpszAPITest) || (TRUE == FindString(lpszAPITest, "XNetDnsRelease")))) {
        XNetDnsReleaseTest(hLog, hMemObject, bRIPs);
    }

    if (NULL != lpszRemotes) {
        xMemFree(hMemObject, lpszRemotes);
    }

    if (NULL != lpszVersions) {
        xMemFree(hMemObject, lpszVersions);
    }

    if (NULL != lpszAPISkip) {
        xMemFree(hMemObject, lpszAPISkip);
    }

    if (NULL != lpszAPITest) {
        xMemFree(hMemObject, lpszAPITest);
    }

    if (NULL != lpszNetsyncRemoteXbox) {
        xMemFree(hMemObject, lpszNetsyncRemoteXbox);
    }

    if (NULL != lpszNetsyncRemoteNt) {
        xMemFree(hMemObject, lpszNetsyncRemoteNt);
    }

    // Free the private heap
    xMemClose(hMemObject);
}



VOID
WINAPI
EndTest(
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Test exit point

Return Value:

  None

------------------------------------------------------------------------------*/
{
}

} // namespace XNetAPINamespace



#ifdef __cplusplus
extern "C" {
#endif

// Export table for harness

#pragma data_seg(EXPORT_SECTION_NAME)
DECLARE_EXPORT_DIRECTORY(xnetapi)
#pragma data_seg()

BEGIN_EXPORT_TABLE(xnetapi)
    EXPORT_TABLE_ENTRY("StartTest", StartTest)
    EXPORT_TABLE_ENTRY("EndTest", EndTest)
END_EXPORT_TABLE(xnetapi)

#ifdef __cplusplus
}
#endif
