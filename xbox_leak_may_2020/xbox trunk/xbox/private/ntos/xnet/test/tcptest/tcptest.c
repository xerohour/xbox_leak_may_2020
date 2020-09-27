#include "precomp.h"
#include <stdlib.h>

#define MAXBUFSIZE      4096
#define MAXXFERCNT      0x100
#define TESTPORT        600
#define TESTADDR        "157.56.11.44"
//#define TESTADDR        "157.56.10.166"

CHAR buf[MAXBUFSIZE];
INT maxxfercnt = MAXXFERCNT;
INT fixxfersize = 0;
INT testFlag = 1;
INT err;
struct sockaddr_in sockname;


VOID
ServerTest()
{
    WSADATA wsadata;
    SOCKET sock, s;
    INT index, total;
    ULONG randSeed = GetTickCount();
    DWORD timer;

    err = WSAStartup(WINSOCK_VERSION, &wsadata);
    if (err != NO_ERROR) {
        WARNFAIL(WSAStartup); return;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        WARNFAIL(socket);
        WSACleanup();
        return;
    }

    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(TESTPORT);
    sockname.sin_addr.s_addr = INADDR_ANY;

    err = _bind(sock, &sockname);
    if (err != NO_ERROR) {
        WARNFAIL(bind); goto exit;
    }

    err = listen(sock, 1);
    if (err != NO_ERROR) {
        WARNFAIL(listen); goto exit;
    }

    err = _getsockname(sock, &sockname);
    if (err != NO_ERROR) {
        WARNFAIL(getsockname); goto exit;
    }

    DbgPrint("Listening on: %s:%d\n",
             inet_ntoa(sockname.sin_addr),
             ntohs(sockname.sin_port));

    s = _accept(sock, &sockname);
    if (s == INVALID_SOCKET) {
        WARNFAIL(accept); goto exit;
    }

    DbgPrint("Connected to: %s:%d\n",
             inet_ntoa(sockname.sin_addr),
             ntohs(sockname.sin_port));

    timer = GetTickCount();
    total = 0;
    for (index=0; index < maxxfercnt; index++) {
        INT count;

        if ((index & 0x3ff) == 0) DbgPrint(".");
        count = fixxfersize ? fixxfersize : RtlRandom(&randSeed) % MAXBUFSIZE + 1;

        count = send(s, buf, count, 0);
        if (count == SOCKET_ERROR) {
            WARNFAIL(send); break;
        }

        total += count;
    }

    closesocket(s);
    timer = GetTickCount() - timer;
    if (timer == 0) timer = 1;
    DbgPrint("Closing connection: %d total bytes transferred\n", total);
    DbgPrint("  time: %dmsec, rate: %d bytes/sec\n", timer, MulDiv(total, 1000, timer));
    Sleep(1000);

exit:
    closesocket(sock);
    WSACleanup();
}


VOID
ClientTest()
{
    WSADATA wsadata;
    SOCKET s;
    INT total, count;
    DWORD timer;

    err = WSAStartup(WINSOCK_VERSION, &wsadata);
    if (err != NO_ERROR) {
        WARNFAIL(WSAStartup); return;
    }

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        WARNFAIL(socket);
        WSACleanup();
        return;
    }

    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(TESTPORT);
    sockname.sin_addr.s_addr = inet_addr(TESTADDR);

    DbgPrint("Connecting to: %s:%d\n",
             inet_ntoa(sockname.sin_addr),
             ntohs(sockname.sin_port));

    timer = GetTickCount();
    err = _connect(s, &sockname);
    if (err != NO_ERROR) {
        WARNFAIL(connect); goto exit;
    }

    err = _getsockname(s, &sockname);
    if (err != NO_ERROR) {
        WARNFAIL(getsockname); goto exit;
    }

    DbgPrint("Local address: %s:%d\n",
             inet_ntoa(sockname.sin_addr),
             ntohs(sockname.sin_port));

    total = 0;
    do {
        count = recv(s, buf, MAXBUFSIZE, 0);
        if (count == SOCKET_ERROR) {
            WARNFAIL(recv); goto exit;
        }

        total += count;
    } while (count > 0);

    timer = GetTickCount() - timer;
    if (timer == 0) timer = 1;
    DbgPrint("Connection closed: %d total bytes transferred\n", total);
    DbgPrint("  time: %dmsec, rate: %d bytes/sec\n", timer, MulDiv(total, 1000, timer));

exit:
    closesocket(s);
    WSACleanup();
}


void __cdecl main()
{
    DbgPrint("Loading XBox network stack...\n");
    err = XnetInitialize(NULL, TRUE);
    if (err != NO_ERROR) {
        WARNFAIL(XnetInitialize); goto stop;
    }

    DbgPrint("*** Test started, press 'g' to continue...\n");

    while (TRUE) {
        DbgPrint("*** To quit, press CTRL-C and type: ed %x 0;g\n", &testFlag);
        BREAK_INTO_DEBUGGER

        if (testFlag <= 0) break;
        if (testFlag == 1) {
            ClientTest();
        } else {
            ServerTest();
        }
    }

    DbgPrint("Unloading XBox network stack...\n");
    XnetCleanup();

stop:
    if (testFlag == 0)
        HalReturnToFirmware(HalRebootRoutine);
    else
        Sleep(INFINITE);
}

