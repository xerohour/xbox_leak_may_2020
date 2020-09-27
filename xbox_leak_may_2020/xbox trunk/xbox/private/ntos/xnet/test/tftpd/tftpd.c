/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tftpd.c

Abstract:

    Simple TFTP server implementation for XBox

Revision History:

    04/19/2000 davidx
        Created it.

Notes:

    Please refer to RFC1350 and RFC2347.

--*/

#include "precomp.h"
#include "tftpd.h"

//
// Global variable definitions
//
BOOL WinsockStarted;
SOCKET TftpSock = INVALID_SOCKET;
BOOL TftpServiceRunning;
INT err;
CHAR TftpRecvBuf[TFTP_PACKET_BUFSIZE];
CHAR TftpSendBuf[TFTP_PACKET_BUFSIZE];
INT tftpdTraceFlag;


BOOL
TftpSendErrorPacket(
    SOCKET s,
    DWORD err
    )

/*++

Routine Description:

    Send a TFTP error packet

Arguments:

    s - Specifies the socket to send to
    err - Specifies the TFTP error code

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

#define ARRAYCOUNT(a) (sizeof(a) / sizeof(a[0]))

{
    //
    // TFTP error message strings
    //
    static const PCSTR errorMessages[] = {
        "Undefined error code.",
        "File not found.",
        "Access violation.",
        "Disk full or allocation exceeded.",
        "Illegal TFTP operation.",
        "Unknown transfer ID.",
        "File already exists.",
        "No such user.",
        "Illegal OACK packet.\n"
    };

    CHAR* buf = TftpSendBuf;
    PCSTR str;
    UINT buflen;

    ADDUSHORTFIELD(TFTPOP_ERROR);
    ADDUSHORTFIELD(err);

    if (err >= ARRAYCOUNT(errorMessages)) err = 0;
    str = errorMessages[err];
    ADDSTRINGFIELD(str);

    buflen = buf - TftpSendBuf;
    return _send(s, TftpSendBuf, &buflen) == NO_ERROR;
}


BOOL
TftpSendAckPacket(
    SOCKET s,
    UINT blocknum
    )

/*++

Routine Description:

    Send a TFTP ACK packet

Arguments:

    s - Specifies the socket handle
    blocknum - Specifies the acknowledged block number

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    CHAR* buf = TftpSendBuf;
    UINT buflen;

    ADDUSHORTFIELD(TFTPOP_ACK);
    ADDUSHORTFIELD(blocknum);

    buflen = buf - TftpSendBuf;
    return _send(s, TftpSendBuf, &buflen) == NO_ERROR;
}


INLINE BOOL
TftpVerifyPacketHeader(
    CHAR* buf,
    DWORD buflen,
    UINT opcode,
    UINT blocknum
    )

/*++

Routine Description:

    Verify a received packet matches what we're expecting
    (the packet can be either ACK or DATA packets)

Arguments:

    buf - Points to the data buffer
    buflen - Length of the data buffer
    opcode - Expected opcode
    blocknum - Expected block number

Return Value:

    TRUE if the received packet is as expected
    FALSE otherwise

--*/

{
    UINT op, blk;

    GETUSHORTFIELD(op);
    GETUSHORTFIELD(blk);

    TRACE_("Received: opcode %d, block %d", op, blk);

    if (opcode == op && blocknum == blk)
        return TRUE;

failed:
    WARNING_("Invalid packet received: expecting %d", blocknum);
    return FALSE;
}


BOOL
TftpIsErrorPacket(
    CHAR* buf,
    DWORD buflen
    )

/*++

Routine Description:

    Determine if a received packet is an ERROR packet

Arguments:

    buf - Points to the data buffer
    buflen - Data buffer length

Return Value:

    TRUE if the packet is a well-formed ERROR packet
    FALSE otherwise

--*/

{
    UINT opcode, errcode;
    CHAR* errmsg;

    GETUSHORTFIELD(opcode);
    GETUSHORTFIELD(errcode);
    GETSTRINGFIELD(errmsg);

    if (opcode == TFTPOP_ERROR) {
        WARNING_("Received ERROR packet: %d, %s", errcode, errmsg);
        return TRUE;
    }

failed:
    return FALSE;
}


BOOL
TftpReadFile(
    SOCKET s,
    const CHAR* filename
    )

/*++

Routine Description:

    Read a file from the disk and send it to a client

Arguments:

    s - Specifies the socket handle
    filename - Specifies the filename

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    UINT blocknum = 1;
    CHAR* buf;
    DWORD buflen, total = 0;
    INT retries;
    SeqFile* file;
    BOOL retval = FALSE;

    VERBOSE_("Read file: %s", filename);

    //
    // Open the file for reading
    //
    file = _CreateFile(filename, GENERIC_READ);
    if (!file) {
        WARNFAIL(CreateFile); return FALSE;
    }
    ASSERT(cfgSeqFileBufSize % TFTP_DATAPACKET_BLOCKSIZE == 0);

    do {
        //
        // Read a block of data from the file
        //
        buflen = TFTP_DATAPACKET_BLOCKSIZE;
        if (!_ReadFile(file, &buf, &buflen)) {
            WARNFAIL(ReadFile); goto failed;
        }
        CopyMem(&TftpSendBuf[TFTP_DATAPACKET_HEADERSIZE], buf, buflen);

        // NOTE: If EOF is reached and buflen is 0,
        // we'll send an empty data packet.

        //
        // Munge data packet header
        //
        buf = TftpSendBuf;
        ADDUSHORTFIELD(TFTPOP_DATA);
        ADDUSHORTFIELD(blocknum);

        TRACE_("Send DATA packet: block %d", blocknum);

        for (retries=0; retries < TFTP_MAX_RETRIES; retries++) {
            //
            // Send the data packet to the client
            //
            DWORD count = TFTP_DATAPACKET_HEADERSIZE+buflen;

            err = _send(s, TftpSendBuf, &count);
            if (err != NO_ERROR) {
                WARNFAIL(send); goto failed;
            }
            
            //
            // Now wait to receive the ACK from the client or
            // until the timeout expires
            //
            count = sizeof(TftpRecvBuf);
            if (_recv(s, TftpRecvBuf, &count) == NO_ERROR) {
                //
                // Did we receive an ACK of correct block number?
                // If so, move on to the next block.
                //
                if (TftpVerifyPacketHeader(TftpRecvBuf, count, TFTPOP_ACK, blocknum)) {
                    total += buflen;
                    blocknum++;
                    break;
                }

                //
                // Did we get an error packet?
                // If so, abort. Otherwise, continue retry.
                //
                if (TftpIsErrorPacket(TftpRecvBuf, count)) goto failed;
            } else {
                //
                // Is the service shutting down?
                //
                if (!TftpServiceRunning) goto failed;
            }
        }

        //
        // Did we exceed our retry count? Give up if we did.
        //
        if (retries == TFTP_MAX_RETRIES) {
            WARNING_("Timed out while waiting for ACK");
            SetLastError(ERROR_TIMEOUT);
            goto failed;
        }

    } while (buflen == TFTP_DATAPACKET_BLOCKSIZE);

    VERBOSE_("Total number of bytes transferred: %d", total);
    retval = TRUE;

failed:
    _CloseFile(file);
    return retval;
}


BOOL
TftpWriteFile(
    SOCKET s,
    const CHAR* filename
    )

/*++

Routine Description:

    Receive a file from a client and write it to the disk

Arguments:

    s - Specifies the socket handle
    filename - Specifies the filename

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    UINT blocknum = 0;
    SeqFile* file;
    INT retries;
    CHAR* buf;
    DWORD buflen, total = 0;
    BOOL retval = FALSE;

    VERBOSE_("Write file: %s", filename);

    //
    // Open the file for writing
    //
    file = _CreateFile(filename, GENERIC_WRITE);
    if (!file) {
        WARNFAIL(CreateFile); return FALSE;
    }
    ASSERT(cfgSeqFileBufSize % TFTP_DATAPACKET_BLOCKSIZE == 0);

    do {
        for (retries=0; retries < TFTP_MAX_RETRIES; retries++) {
            //
            // Send the current ACK packet
            //
            TRACE_("Send ACK packet: block %d", blocknum);
            if (!TftpSendAckPacket(s, blocknum)) goto failed;

            //
            // Wait for the next data packet
            //
            buflen = sizeof(TftpRecvBuf);
            if (_recv(s, TftpRecvBuf, &buflen) == NO_ERROR) {
                //
                // Got a data packet: verify header information
                //
                if (TftpVerifyPacketHeader(TftpRecvBuf, buflen, TFTPOP_DATA, blocknum+1))
                    break;

                //
                // Did we get an error packet?
                // If so, abort. Otherwise, continue retry.
                //
                if (TftpIsErrorPacket(TftpRecvBuf, buflen)) goto failed;
            } else {
                //
                // We failed to receive a data packet.
                // If the service is shutting down, give up.
                // Otherwise, resend the ACK and wait again.
                //
                if (!TftpServiceRunning) goto failed;
            }
        }

        if (retries == TFTP_MAX_RETRIES) {
            WARNING_("Timed out waiting for data packet");
            SetLastError(ERROR_TIMEOUT);
            goto failed;
        }

        //
        // Write the received data to file
        //
        blocknum++;
        buf = &TftpRecvBuf[TFTP_DATAPACKET_HEADERSIZE];
        buflen -= TFTP_DATAPACKET_HEADERSIZE;
        total += buflen;

        if (buflen > 0) {
            if (!_WriteFile(file, buf, buflen)) {
                WARNFAIL(WriteFile); goto failed;
            }
        }
    } while (buflen == TFTP_DATAPACKET_BLOCKSIZE);

    //
    // Send the final ACK packet, ignore error
    //
    TftpSendAckPacket(s, blocknum);
    
    VERBOSE_("Total number of bytes transferred: %d", total);
    retval = TRUE;

failed:
    _CloseFile(file);
    return retval;
}


BOOL
TftpHandleRequest(
    CHAR* buf,
    DWORD buflen,
    struct sockaddr_in* from
    )

/*++

Routine Description:

    Handle a TFTP request packet

Arguments:

    buf - Points to the received data buffer
    buflen - Buffer length
    from - Specifies the requester's address and port

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

#define WAITREAD_TIMEOUT 2000

{
    UINT opcode;
    CHAR* filename;
    CHAR* mode;
    SOCKET s;
    struct sockaddr_in addr;
    INT err = TFTPERR_INVALID_OPCODE;

    TRACE_("Received request: %s %d", inet_ntoa(from->sin_addr), ntohs(from->sin_port));
    TftpDumpPacket(buf, buflen);

    //
    // Open a new socket to handle the request
    //
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) goto sockfailed;

    //
    // Bind to local port and connect to remote client
    //
    ZeroMem(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;

    if (_bind(s, &addr) != 0 ||
        _connect(s, from) != 0 ||
        _setrcvtimeout(s, WAITREAD_TIMEOUT) != 0)
        goto sockfailed;

    if (_getsockname(s, &addr) == NO_ERROR) {
        TRACE_("Local socket: %s %d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }

    //
    // Extract the opcode
    //
    GETUSHORTFIELD(opcode);
    if (opcode != TFTPOP_RRQ && opcode != TFTPOP_WRQ) goto failed;
    
    //
    // Extract the filename and transfer mode field
    // NOTE: we treat octet and netascii mode as the same
    //
    GETSTRINGFIELD(filename);
    GETSTRINGFIELD(mode);

    if (_stricmp(mode, "octet") != 0 && _stricmp(mode, "netascii") != 0) {
        WARNING_("Only octet and netascii mode are supported");
        goto failed;
    }

    // NOTE: Options are ignored

    //
    // Execute the read or write request
    //
    if (((opcode == TFTPOP_RRQ) ? TftpReadFile : TftpWriteFile)(s, filename)) {
        closesocket(s);
        return TRUE;
    }
    
    //
    // Map Win32 error code to TFTP error code
    //
    switch (GetLastError()) {

    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
        err = TFTPERR_FILE_NOT_FOUND;
        break;
    
    case ERROR_ACCESS_DENIED:
        err = TFTPERR_ACCESS_DENIED;
        break;

    case ERROR_HANDLE_DISK_FULL:
        err = TFTPERR_DISK_FULL;
        break;

    case ERROR_FILE_EXISTS:
        err = TFTPERR_FILE_EXISTS;
        break;
    
    case ERROR_NO_SUCH_USER:
        err = TFTPERR_INVALID_USER;
        break;

    default:
        err = TFTPERR_UNKNOWN;
        break;
    }

failed:
    //
    // Failed to handle the TFTP request
    //
    WARNING_("TftpHandleRequest failed: %d", err);

    TftpSendErrorPacket(s, err);
    closesocket(s);
    return FALSE;

sockfailed:
    //
    // Failed to open a socket to handle the request
    //
    WARNING_("Failed to open response socket: %d", GetLastError());
    if (s != INVALID_SOCKET) closesocket(s);
    return FALSE;

}


VOID
TftpCleanup()

/*++

Routine Description:

    Cleanup the resources used by the TFTP service 

Arguments:

    NONE

Return Value:

    NONE

--*/

{
    if (TftpServiceRunning) {
        //
        // The TFTP service is still running when cleanup is called
        //
        WARNING_("TftpCleanup: the service thread is still running");
        TftpStopService();
        return;
    }

    //
    // Close the server socket
    //
    if (TftpSock != INVALID_SOCKET) {
        closesocket(TftpSock);
        TftpSock = INVALID_SOCKET;
    }

    //
    // Uninitialize Winsock
    //
    if (WinsockStarted) {
        WSACleanup();
        WinsockStarted = FALSE;
    }
}


BOOL
TftpSetDefaultDirectory()

/*++

Routine Description:

    Set the default directory for the TFTP service

Arguments:

    NONE

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    // _XBOX_BUGBUG: Do nothing for now
    return TRUE;
}


BOOL
TftpOpenServerSocket()

/*++

Routine Description:

    Open the TFTP server socket

Arguments:

    NONE

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

#define WAITREQ_TIMEOUT 2000

{
    struct sockaddr_in addr;

    //
    // Open the socket
    //
    TftpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (TftpSock == INVALID_SOCKET) {
        WARNING_("socket failed: %d\n", GetLastError());
        return FALSE;
    }

    //
    // Bind the socket to the TFTP server port
    // and set the receive timeout option
    //
    ZeroMemory(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(TFTP_SERVER_PORT);

    if (_bind(TftpSock, &addr) != 0 ||
        _setrcvtimeout(TftpSock, WAITREQ_TIMEOUT) != 0) {
        WARNING_("bind failed: %d", GetLastError());
        return FALSE;
    }

    return TRUE;
}


DWORD
TftpServiceThread(
    VOID* param
    )
{
    DWORD byteCount;
    struct sockaddr_in from;

    VERBOSE_("Waiting for the next request...");
    while (TftpServiceRunning) {
        //
        // Wait to receive the next request and then handle it
        //
        byteCount = sizeof(TftpRecvBuf);
        if (_recvfrom(TftpSock, TftpRecvBuf, &byteCount, &from) == NO_ERROR) {
            TftpHandleRequest(TftpRecvBuf, byteCount, &from);
            VERBOSE_("Waiting for the next request...");
        }
    }

    TftpCleanup();
    TRACE_("TFTP service stopped");
    return 0;
}


DWORD WINAPI
TftpStartService()

/*++

Routine Description:

    Start the TFTP service

Arguments:

    NONE

Return Value:

    Win32 error code

--*/

{
    HANDLE threadHandle;
    DWORD threadId;
    WSADATA wsadata;
    DWORD err;

    //
    // Initialize Winsock
    //
    if (WSAStartup(WINSOCK_VERSION, &wsadata) != 0) goto failed;
    WinsockStarted = TRUE;

    //
    // Set default directory
    //
    if (!TftpSetDefaultDirectory()) goto failed;

    //
    // Open TFTP server socket
    //
    if (!TftpOpenServerSocket()) goto failed;

    //
    // Create the TFTP server thread
    //
    TftpServiceRunning = TRUE;
    threadHandle = CreateThread(NULL, 0, TftpServiceThread, NULL, 0, &threadId);
    if (!threadHandle) goto failed;

    TRACE_("TFTP service started");
    return NO_ERROR;

failed:

    //
    // Cleanup and return error
    //
    TftpServiceRunning = FALSE;
    TftpCleanup();

    err = GetLastError();
    WARNING_("TftpStartService failed: %d", err);
    return err;
}


DWORD WINAPI
TftpStopService()
{
    if (TftpServiceRunning) {
        //
        // Signal the TFTP service thread to stop
        //
        TftpServiceRunning = FALSE;
        while (WinsockStarted) {
            Sleep(500);
        }
    } else {
        WARNING_("TFTP service already stopped");
    }

    return NO_ERROR;
}


#if DBG

VOID
TftpDumpPacket(
    CHAR* buf,
    DWORD buflen
    )

/*++

Routine Description:

    Dump a TFTP packet

Arguments:

    buf - Points to the packet data
    buflen - Specifies the packet length

Return Value:

    NONE

--*/

{
    //
    // Opcode strings
    //
    static const PCSTR opcodeStrings[] = {
        "RRQ",
        "WRQ",
        "DATA",
        "ACK",
        "ERROR",
        "OACK"
    };

    DWORD status;
    UINT opcode, blocknum, errcode;
    CHAR* s;

    if (!tftpdTraceFlag) return;

    GETUSHORTFIELD(opcode);

    DbgPrint("TFTP packet type: ");
    if (opcode > 0 && opcode <= ARRAYCOUNT(opcodeStrings))
        DbgPrint("%s\n", opcodeStrings[opcode-1]);
    else
        DbgPrint("unknown (%d)\n", opcode);

    switch (opcode) {
    case TFTPOP_RRQ:
    case TFTPOP_WRQ:
    case TFTPOP_OACK:
        while (buflen) {
            GETSTRINGFIELD(s);
            DbgPrint("  %s\n", s);
        }
        break;
    
    case TFTPOP_DATA:
        GETUSHORTFIELD(blocknum);
        DbgPrint("  block %d, %d bytes\n", blocknum, buflen);
        break;
        
    case TFTPOP_ACK:
        GETUSHORTFIELD(blocknum);
        DbgPrint("  block %d\n", blocknum);
        break;

    case TFTPOP_ERROR:
        GETUSHORTFIELD(errcode);
        GETSTRINGFIELD(s);
        DbgPrint("  code %d, %s\n", errcode, s);
        break;
    }
    
    return;

failed:
    DbgPrint("Ill-formed TFTP packet\n");
}

#endif // DBG


#ifndef BUILD_DLL

volatile INT testFlag = 1;

void __cdecl main()
{
    DbgPrint("Loading XBox network stack...\n");
    err = XnetInitialize(NULL, TRUE);
    if (err != NO_ERROR) {
        WARNFAIL(XnetInitialize); goto stop;
    }

    DbgPrint("Starting TFTP service...\n");
    err = TftpStartService();
    if (err != NO_ERROR) {
        WARNFAIL(TftpStartService); goto unload;
    }

    DbgPrint("*** Test started, press 'g' to continue...\n");
    DbgPrint("*** To quit, press CTRL-C and type: ed %x 0;g\n", &testFlag);
    BREAK_INTO_DEBUGGER

    while (testFlag > 0) {
        Sleep(1000);
    }

    DbgPrint("Stopping TFTP service...\n");
    TftpStopService();

unload:
    DbgPrint("Unloading XBox network stack...\n");
    XnetCleanup();

stop:
    if (testFlag == 0)
        HalReturnToFirmware(HalRebootRoutine);
    else
        Sleep(INFINITE);
}

#else // BUILD_DLL

BOOL WINAPI
DllMain(
    HINSTANCE hInstance,
    DWORD fdwReason,
    LPVOID lpReserved
    )
{
    UNREFERENCED_PARAMETER( hInstance );
    UNREFERENCED_PARAMETER( fdwReason );
    UNREFERENCED_PARAMETER( lpReserved );
    return TRUE;
}

#endif // BUILD_DLL
