#include <xtl.h>
#include <xdbg.h>

#define TESTPORT 6000
#define SENDCOUNT 1024
#define BUFSIZE 4

#define SERVERADDR "157.56.11.44"
//#define SERVERADDR "157.56.10.166"

volatile INT testFlag = 1;
INT err;
SOCKET s;
static union {
    CHAR buf[BUFSIZE];
    UINT seqno;
};
static union {
    struct sockaddr sockname;
    struct sockaddr_in sockin;
};

VOID UdpSendTest()
{
    LARGE_INTEGER counter0, counter1, freq;
    BOOL ok;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(s != INVALID_SOCKET);

    memset(&sockname, 0, sizeof(sockname));
    sockin.sin_family = AF_INET;
    sockin.sin_port = htons(TESTPORT);
    err = bind(s, &sockname, sizeof(sockname));
    ASSERT(err == NO_ERROR);

    sockin.sin_addr.s_addr = inet_addr(SERVERADDR);
    err = connect(s, &sockname, sizeof(sockname));
    ASSERT(err == NO_ERROR);

    DWORD tick = GetTickCount();

    for (seqno=0; seqno < SENDCOUNT; seqno++) {
        err = send(s, buf, BUFSIZE, 0);
        ASSERT(err == BUFSIZE);
    }

    tick = GetTickCount() - tick;
    DbgPrint("%d packets sent in %d msecs\n", SENDCOUNT, tick);
    Sleep(2000);
    closesocket(s);
}

VOID UdpRecvTest()
{
    s = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(s != INVALID_SOCKET);

    memset(&sockname, 0, sizeof(sockname));
    sockin.sin_family = AF_INET;
    sockin.sin_port = htons(TESTPORT);
    err = bind(s, &sockname, sizeof(sockname));
    ASSERT(err == NO_ERROR);

    INT timeout = 2000;
    err = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (CHAR*) &timeout, sizeof(timeout));
    ASSERT(err == NO_ERROR);

    DbgPrint("Listening on UDP port %d...\n", TESTPORT);

    INT count = 0;
    while (TRUE) {
        err = recv(s, buf, BUFSIZE, 0);
        if (err == SOCKET_ERROR) {
            ASSERT(WSAGetLastError() == WSAETIMEDOUT);
            if (count) break;
        } else {
            count++;
        }
    }

    closesocket(s);
    DbgPrint("%d packets received\n", count);
}

VOID __cdecl main()
{
    DbgPrint("Loading XBox network stack...\n");
    err = XnetInitialize(NULL, TRUE);
    ASSERT(err == NO_ERROR);

    WSADATA wsadata;
    err = WSAStartup(WINSOCK_VERSION, &wsadata);
    ASSERT(err == NO_ERROR);

    while (TRUE) {
        DbgPrint("*** To quit, type: ed %x 0;g\n", &testFlag);
        __asm int 3
        if (testFlag == 0) break;
        if (testFlag == 1)
            UdpSendTest();
        else
            UdpRecvTest();
    }

    DbgPrint("Unloading XBox network stack...\n");
    WSACleanup();
    XnetCleanup();

    DbgPrint("Test finished.\n");
    Sleep(INFINITE);
}

