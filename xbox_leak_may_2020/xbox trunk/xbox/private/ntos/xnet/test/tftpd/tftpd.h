/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tftpd.h

Abstract:

    Simple TFTP server implementation for XBox

Revision History:

    04/19/2000 davidx
        Created it.

--*/

#ifndef _TFTPD_H
#define _TFTPD_H

//
// TFTP server UDP port number
//

#define TFTP_SERVER_PORT 69

//
// TFTP opcodes
//

#define TFTPOP_RRQ      1
#define TFTPOP_WRQ      2
#define TFTPOP_DATA     3
#define TFTPOP_ACK      4
#define TFTPOP_ERROR    5
#define TFTPOP_OACK     6

//
// Max TFTP packet size: 4 bytes header + 512 bytes data
//

#define TFTP_DATAPACKET_HEADERSIZE  4
#define TFTP_DATAPACKET_BLOCKSIZE   512
#define TFTP_PACKET_BUFSIZE         (4+512)

//
// Error codes
//

#define TFTPERR_UNKNOWN         0
#define TFTPERR_FILE_NOT_FOUND  1
#define TFTPERR_ACCESS_DENIED   2
#define TFTPERR_DISK_FULL       3
#define TFTPERR_INVALID_OPCODE  4
#define TFTPERR_INVALID_ID      5
#define TFTPERR_FILE_EXISTS     6
#define TFTPERR_INVALID_USER    7
#define TFTPERR_INVALID_OACK    8

//
// Timeout and retry values
//
#define TFTP_MAX_RETRIES        3
#define TFTP_RECV_TIMEOUT       2000

//
// Extract a u_short field out of an incoming TFTP message
// NOTE:
//  we assume we're on a little-endian machine and
//  take care of byte swapping here.
//
#define GETUSHORTFIELD(_var) do { \
            if (buflen < 2) goto failed; \
            _var = ((UINT) ((BYTE*) buf)[0] << 8 | \
                    (UINT) ((BYTE*) buf)[1]); \
            buf += 2; \
            buflen -= 2; \
        } while (0)

//
// Extract a string field out of an incoming TFTP message
//
#define GETSTRINGFIELD(_var) do { \
            CHAR* _p = buf; \
            CHAR* _q = _p + buflen; \
            INT _n; \
            while (_p < _q && *_p) _p++; \
            if (_p == _q) goto failed; \
            _var = buf; \
            _n = (_p - buf) + 1; \
            buf += _n; \
            buflen -= _n; \
        } while (0)

//
// Insert a u_short field into an outgoing TFTP message
// NOTE:
//  we assume we're on a little-endian machine and
//  take care of byte swapping here.
//
#define ADDUSHORTFIELD(_var) do { \
            buf[0] = (CHAR) ((_var) >> 8); \
            buf[1] = (CHAR) (_var); \
            buf += 2; \
        } while (0)

//
// Insert a string field into an outgoing TFTP message
//
#define ADDSTRINGFIELD(_var) do { \
            INT _n = strlen(_var) + 1; \
            memcpy(buf, _var, _n); \
            buf += _n; \
        } while (0)

//
// Function prototypes
//

DWORD WINAPI
TftpStartService();

DWORD WINAPI
TftpStopService();

VOID
TftpCleanup();

BOOL
TftpSetDefaultDirectory();

BOOL
TftpOpenServerSocket();

BOOL
TftpHandleRequest(
    CHAR* buf,
    DWORD buflen,
    struct sockaddr_in* from
    );

BOOL
TftpSendAckPacket(
    SOCKET s,
    UINT blocknum
    );

BOOL
TftpVerifyPacketHeader(
    CHAR* buf,
    DWORD buflen,
    UINT opcode,
    UINT blocknum
    );

BOOL
TftpSendErrorPacket(
    SOCKET s,
    DWORD err
    );

BOOL
TftpReadFile(
    SOCKET s,
    const CHAR* filename
    );

BOOL
TftpWriteFile(
    SOCKET s,
    const CHAR* filename
    );

#if DBG

VOID
TftpDumpPacket(
    CHAR* buf,
    DWORD buflen
    );

#else

#define TftpDumpPacket(buf, buflen)

#endif

#endif // !_TFTPD_H

