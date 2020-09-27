/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tftp.c

Abstract:

    XBox TFTP client program

Revision History:

    06/30/2000 davidx
        Created it.

--*/

#include "precomp.h"
#include "tftp.h"

CHAR TftpRecvBuf[TFTP_PACKET_BUFSIZE];
CHAR TftpSendBuf[TFTP_PACKET_BUFSIZE];
DWORD TftpSendBufsize;
DWORD TftpRecvBufsize;

CHAR* TftpMode = "octet";
CHAR* TftpServerAddr = "157.56.10.165";
CHAR* SrcFilename = "c:\\temp\\data2";
CHAR* DstFilename = "c:\\temp\\test";
INT testFlag = 1;
INT err;
struct sockaddr_in sockname;

#define VERIFY(cond, _apiname) \
        if (!(cond)) { \
            DbgPrint(#_apiname " failed: %d %d\n", err, GetLastError()); \
            goto failed; \
        }

SOCKET
ConnectTftpServer()
{
    SOCKET sock = INVALID_SOCKET;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    VERIFY(sock != INVALID_SOCKET, socket);

    ZeroMem(&sockname, sizeof(sockname));
    sockname.sin_family = AF_INET;
    err = _bind(sock, &sockname);
    VERIFY(err == NO_ERROR, bind);

    err = _setrcvtimeout(sock, TFTP_RECV_TIMEOUT);
    VERIFY(err == NO_ERROR, setrcvtimeout);

    return sock;

failed:
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
    }
    return INVALID_SOCKET;
}


//
// Send initial request to the server and wait for response
//
BOOL
SendRequestAndWait(
    SOCKET sock,
    UINT opcode,
    UINT blocknum
    )
{
    INT retries = TFTP_MAX_RETRIES;
    UINT buflen;

    ZeroMem(&sockname, sizeof(sockname));
    sockname.sin_family = AF_INET;
    
    while (TRUE) {
        if (retries-- == 0) {
            WARNING_("Server doesn't appear to be responding...");
            goto failed;
        }

        sockname.sin_port = htons(TFTP_SERVER_PORT);
        sockname.sin_addr.s_addr = inet_addr(TftpServerAddr);

        DbgPrint("Sending request to TFTP server: %s:%d\n",
                 inet_ntoa(sockname.sin_addr),
                 ntohs(sockname.sin_port));

        buflen = TftpSendBufsize;
        err = _sendto(sock, TftpSendBuf, &buflen, &sockname);
        VERIFY(err == NO_ERROR, sendto);

        TftpRecvBufsize = sizeof(TftpRecvBuf);
        if (_recvfrom(sock, TftpRecvBuf, &TftpRecvBufsize, &sockname) == NO_ERROR) {
            UINT op, blk, buflen = TftpRecvBufsize;
            CHAR* buf = TftpRecvBuf;

            GETUSHORTFIELD(op);
            GETUSHORTFIELD(blk);
            if (op == opcode && blocknum == blk) break;

            if (op == TFTPOP_ERROR) {
                WARNING_("Received ERROR packet: %d", blk);
                return FALSE;
            }
        }
    }

    DbgPrint("Connecting to TFTP server: %s:%d\n",
             inet_ntoa(sockname.sin_addr),
             ntohs(sockname.sin_port));

    err = _connect(sock, &sockname);
    VERIFY(err == NO_ERROR, connect);

    err = _getsockname(sock, &sockname);
    VERIFY(err == NO_ERROR, getsockname);

    DbgPrint("Local address: %s:%d\n",
             inet_ntoa(sockname.sin_addr),
             ntohs(sockname.sin_port));

    return TRUE;

failed:
    return FALSE;
}


//
// Send data to the server and wait for response
//
BOOL
SendAndWait(
    SOCKET sock,
    UINT opcode,
    UINT blocknum
    )
{
    INT retries = TFTP_MAX_RETRIES;
    UINT buflen;

    while (retries--) {
        buflen = TftpSendBufsize;
        err = _send(sock, TftpSendBuf, &buflen);
        VERIFY(err == NO_ERROR, send);

        TftpRecvBufsize = sizeof(TftpRecvBuf);
        if (_recv(sock, TftpRecvBuf, &TftpRecvBufsize) == NO_ERROR) {
            UINT op, blk, buflen = TftpRecvBufsize;
            CHAR* buf = TftpRecvBuf;

            GETUSHORTFIELD(op);
            GETUSHORTFIELD(blk);
            if (op == opcode && blocknum == blk) return TRUE;

            if (op == TFTPOP_ERROR) {
                WARNING_("Received ERROR packet: %d", blk);
                return FALSE;
            }
        }
    }
    SetLastError(ERROR_TIMEOUT);

failed:
    return FALSE;
}

//
// Send file to the TFTP server
//
VOID
PutTest()
{
    SeqFile* file;
    SOCKET sock = INVALID_SOCKET;
    CHAR* buf;
    DWORD buflen, timer, total = 0;
    UINT blocknum = 0;

    DbgPrint("Opening local file %s for reading...\n", SrcFilename);
    file = _CreateFile(SrcFilename, GENERIC_READ);
    VERIFY(file != NULL, CreateFile);

    timer = GetTickCount();

    sock = ConnectTftpServer();
    VERIFY(sock != INVALID_SOCKET, ConnectTftpServer);

    DbgPrint("Writing remote file %s...\n", DstFilename);

    buf = TftpSendBuf;
    ADDUSHORTFIELD(TFTPOP_WRQ);
    ADDSTRINGFIELD(DstFilename);
    ADDSTRINGFIELD(TftpMode);

    TftpSendBufsize = buf-TftpSendBuf;
    VERIFY(SendRequestAndWait(sock, TFTPOP_ACK, blocknum), WaitWRQAck);

    ASSERT(cfgSeqFileBufSize % TFTP_DATAPACKET_BLOCKSIZE == 0);

    do {
        //
        // Read a block of data from the file
        //
        buflen = TFTP_DATAPACKET_BLOCKSIZE;
        VERIFY(_ReadFile(file, &buf, &buflen), ReadFile);
        CopyMem(&TftpSendBuf[TFTP_DATAPACKET_HEADERSIZE], buf, buflen);

        //
        // Munge data packet header
        //
        blocknum++;
        buf = TftpSendBuf;
        ADDUSHORTFIELD(TFTPOP_DATA);
        ADDUSHORTFIELD(blocknum);

        TftpSendBufsize = TFTP_DATAPACKET_HEADERSIZE+buflen;
        VERIFY(SendAndWait(sock, TFTPOP_ACK, blocknum), WaitDataAck);

        total += buflen;
    } while (buflen == TFTP_DATAPACKET_BLOCKSIZE);

    timer = GetTickCount() - timer;
    if (timer == 0) timer = 1;
    DbgPrint("Total transfer: %d bytes in %d msecs\n", total, timer);
    DbgPrint("Transfer speed: %d bytes /sec\n", MulDiv(total, 1000, timer));

failed:
    _CloseFile(file);
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
    }
}


//
// Retrieve file from the TFTP server
//
VOID
GetTest()
{
    SeqFile* file;
    SOCKET sock = INVALID_SOCKET;
    CHAR* buf;
    DWORD buflen, timer, total = 0;
    UINT blocknum = 1;

    DbgPrint("Opening local file %s for writing...\n", DstFilename);
    file = _CreateFile(DstFilename, GENERIC_WRITE);
    VERIFY(file != NULL, CreateFile);

    timer = GetTickCount();

    sock = ConnectTftpServer();
    VERIFY(sock != INVALID_SOCKET, ConnectTftpServer);

    DbgPrint("Reading remote file %s...\n", SrcFilename);

    buf = TftpSendBuf;
    ADDUSHORTFIELD(TFTPOP_RRQ);
    ADDSTRINGFIELD(SrcFilename);
    ADDSTRINGFIELD(TftpMode);

    TftpSendBufsize = buf-TftpSendBuf;
    VERIFY(SendRequestAndWait(sock, TFTPOP_DATA, blocknum), WaitRRQAck);

    ASSERT(cfgSeqFileBufSize % TFTP_DATAPACKET_BLOCKSIZE == 0);

    while (TRUE) {
        buf = &TftpRecvBuf[TFTP_DATAPACKET_HEADERSIZE];
        buflen = TftpRecvBufsize - TFTP_DATAPACKET_HEADERSIZE;
        if (buflen > 0) {
            total += buflen;
            VERIFY(_WriteFile(file, buf, buflen), WriteFile);
        }

        // Prepare ACK packet
        buf = TftpSendBuf;
        ADDUSHORTFIELD(TFTPOP_ACK);
        ADDUSHORTFIELD(blocknum);
        TftpSendBufsize = buf-TftpSendBuf;

        if (buflen != TFTP_DATAPACKET_BLOCKSIZE) {
            VERIFY(_send(sock, TftpSendBuf, &TftpSendBufsize) == NO_ERROR, send);
            break;
        }

        blocknum++;
        VERIFY(SendAndWait(sock, TFTPOP_DATA, blocknum), WaitData);
    }

    timer = GetTickCount() - timer;
    if (timer == 0) timer = 1;
    DbgPrint("Total transfer: %d bytes in %d msecs\n", total, timer);
    DbgPrint("Transfer speed: %d bytes /sec\n", MulDiv(total, 1000, timer));

failed:
    _CloseFile(file);
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
    }
}


#define BREAK_INTO_DEBUGGER __asm int 3

void __cdecl main()
{
    WSADATA wsadata;

    DbgPrint("Loading XBox network stack...\n");
    err = XnetInitialize(NULL, TRUE);
    if (err != NO_ERROR) {
        WARNFAIL(XnetInitialize); goto stop;
    }

    err = WSAStartup(WINSOCK_VERSION, &wsadata);
    if (err != NO_ERROR) {
        WARNFAIL(WSAStartup); goto unload;
    }

    DbgPrint("*** Test started, press 'g' to continue...\n");

    while (TRUE) {
        DbgPrint("*** To quit, press CTRL-C and type: ed %x 0;g\n", &testFlag);
        BREAK_INTO_DEBUGGER

        if (testFlag <= 0) break;
        if (testFlag == 1) {
            PutTest();
        } else {
            GetTest();
        }
    }
    WSACleanup();

unload:
    DbgPrint("Unloading XBox network stack...\n");
    XnetCleanup();

stop:
    if (testFlag == 0)
        HalReturnToFirmware(HalRebootRoutine);
    else
        Sleep(INFINITE);
}

