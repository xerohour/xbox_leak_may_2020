// File transfer program using a TCP connection

#include "precomp.h"

#define TESTPORT 600
#define RCVTIMEOUT 15000
#define RCVBUFSIZE 4096

INT testFlag = 1;
INT err;
ULONG total;
CHAR filename[MAX_PATH];
CHAR recvbuf[RCVBUFSIZE];
BOOL noWrite;


VOID
DoGetFile(
    SOCKET s,
    CHAR* filename
    )
{
    SeqFile* file;
    DWORD timer = GetTickCount();
    DWORD buflen, count;

    file = _CreateFile(filename, GENERIC_READ);
    if (!file) {
        WARNFAIL(CreateFile);
        return;
    }

    total = 0;
    do {
        CHAR* p;
        count = cfgSeqFileBufSize;
        if (!_ReadFile(file, &p, &count)) {
            WARNFAIL(ReadFile); goto exit;
        }

        total += count;
        buflen = count;
        while (buflen) {
            DWORD n = buflen;
            err = _send(s, p, &n);
            if (err != NO_ERROR) {
                WARNFAIL(send); goto exit;
            }

            p += n;
            buflen -= n;
        }
    } while (count == cfgSeqFileBufSize);

    timer = GetTickCount() - timer;
    if (timer == 0) timer = 1;
    DbgPrint("  total bytes transferred: %d\n", total);
    DbgPrint("  time: %dmsec, rate: %d bytes/sec\n", timer, MulDiv(total, 1000, timer));

exit:
    _CloseFile(file);
}


VOID
DoPutFile(
    SOCKET s,
    CHAR* filename
    )
{
    SeqFile* file;
    DWORD timer = GetTickCount();
    DWORD count;

    file = _CreateFile(filename, GENERIC_WRITE);
    if (!file) {
        WARNFAIL(CreateFile);
        return;
    }

    total = 0;
    while (TRUE) {
        count = RCVBUFSIZE;
        err = _recv(s, recvbuf, &count);
        if (err != NO_ERROR) {
            WARNFAIL(recv); goto exit;
        }

        if (count == 0) break;
        total += count;
        if (count && !noWrite) {
            if (!_WriteFile(file, recvbuf, count)) {
                WARNFAIL(WriteFile); goto exit;
            }
        }
    }

    timer = GetTickCount() - timer;
    if (timer == 0) timer = 1;
    DbgPrint("  total bytes transferred: %d\n", total);
    DbgPrint("  time: %dmsec, rate: %d bytes/sec\n", timer, MulDiv(total, 1000, timer));

exit:
    _CloseFile(file);
}


BOOL
GetXferRequest(
    IN SOCKET s,
    OUT BOOL* getFile,
    OUT CHAR* filename
    )
{
    DWORD buflen = RCVBUFSIZE;

    err = _recv(s, recvbuf, &buflen);
    if (err != NO_ERROR) {
        WARNFAIL(recv);
        return FALSE;
    }

    if (buflen <= 4 || buflen >= 4+MAX_PATH) goto badreq;

    if (strncmp(recvbuf, "GET ", 4) == 0)
        *getFile = TRUE;
    else if (strncmp(recvbuf, "PUT ", 4) == 0)
        *getFile = FALSE;
    else
        goto badreq;
    
    CopyMem(filename, &recvbuf[4], buflen-4);
    filename[buflen-4] = 0;

    DbgPrint("%s %s\n", *getFile ? "GET" : "PUT", filename);
    return TRUE;

badreq:
    DbgPrint("Bad file transfer request\n");
    return FALSE;
}


VOID
FileXfer()
{
    SOCKET sock, s;
    struct sockaddr_in sockname;
    INT total;
    DWORD timer;
    BOOL getFile;

    s = INVALID_SOCKET;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        WARNFAIL(socket); goto exit;
    }

    ZeroMem(&sockname, sizeof(sockname));
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(TESTPORT);
    sockname.sin_addr.s_addr = INADDR_ANY;

    err = _bind(sock, &sockname);
    if (err != NO_ERROR) {
        WARNFAIL(bind); goto exit;
    }

    err = listen(sock, 3);
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

    err = _setrcvtimeout(s, RCVTIMEOUT);
    if (err != NO_ERROR) {
        WARNFAIL(setrcvtimeout); goto exit;
    }

    DbgPrint("Connected to: %s:%d\n",
             inet_ntoa(sockname.sin_addr),
             ntohs(sockname.sin_port));

    if (GetXferRequest(s, &getFile, filename)) {
        if (getFile) {
            DoGetFile(s, filename);
        } else {
            DoPutFile(s, filename);
        }
    }

exit:
    if (s != INVALID_SOCKET) { closesocket(s); }
    if (sock != INVALID_SOCKET) { closesocket(sock); }
}


void __cdecl main()
{
    WSADATA wsadata;

    DbgPrint("Loading XBox network stack...\n");
    err = XnetInitialize(NULL, TRUE);
    if (err != NO_ERROR) {
        WARNFAIL(XnetInitialize); goto stop;
    }

    DbgPrint("*** Test started, press 'g' to continue...\n");

    while (TRUE) {
        DbgPrint("*** To quit, press CTRL-C and type: ed %x 0;g\n", &testFlag);
        Sleep(1000);
        BREAK_INTO_DEBUGGER

        if (testFlag <= 0) break;
        err = WSAStartup(WINSOCK_VERSION, &wsadata);
        if (err != NO_ERROR) {
            WARNFAIL(WSAStartup); goto unload;
        }

        if (testFlag == 1) {
            FileXfer();
        }
        WSACleanup();
    }

unload:
    DbgPrint("Unloading XBox network stack...\n");
    XnetCleanup();

stop:
    if (testFlag == 0)
        HalReturnToFirmware(HalRebootRoutine);
    else
        Sleep(INFINITE);
}

