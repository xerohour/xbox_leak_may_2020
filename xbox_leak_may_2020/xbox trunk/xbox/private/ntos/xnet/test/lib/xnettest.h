/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    xnettest.h

Abstract:

    XBox network test shared header file

Revision History:

    08/07/2000 davidx
        Created it.

--*/

#ifndef _XNETTEST_H
#define _XNETTEST_H

//
// Wrap function for Winsock API
//
INLINE INT _send(SOCKET s, CHAR* buf, DWORD* buflen) {
    WSABUF wsabuf = { *buflen, buf };
    return WSASend(s, &wsabuf, 1, buflen, 0, 0, 0);
}

INLINE INT _sendto(SOCKET s, CHAR* buf, DWORD* buflen, struct sockaddr_in* to) {
    WSABUF wsabuf = { *buflen, buf };
    return WSASendTo(s, &wsabuf, 1, buflen, 0, (struct sockaddr*) to, sizeof(*to), 0, 0);
}

INLINE INT _recv(SOCKET s, CHAR* buf, DWORD* buflen) {
    WSABUF wsabuf = { *buflen, buf };
    DWORD flags = 0;
    return WSARecv(s, &wsabuf, 1, buflen, &flags, 0, 0);
}

INLINE INT _recvfrom(SOCKET s, CHAR* buf, DWORD* buflen, struct sockaddr_in* from) {
    WSABUF wsabuf = { *buflen, buf };
    INT fromlen = sizeof(*from);
    DWORD flags = 0;
    return WSARecvFrom(s, &wsabuf, 1, buflen, &flags, (struct sockaddr*) from, &fromlen, 0, 0);
}

INLINE INT _bind(SOCKET s, struct sockaddr_in* addr) {
    return bind(s, (struct sockaddr*) addr, sizeof(*addr));
}

INLINE INT _getsockname(SOCKET s, struct sockaddr_in* addr) {
    INT addrlen = sizeof(*addr);
    return getsockname(s, (struct sockaddr*) addr, &addrlen);
}

INLINE _connect(SOCKET s, struct sockaddr_in* addr) {
    return connect(s, (struct sockaddr*) addr, sizeof(*addr));
}

INLINE INT _accept(SOCKET s, struct sockaddr_in* addr) {
    INT addrlen = sizeof(*addr);
    return accept(s, (struct sockaddr*) addr, &addrlen);
}

INLINE INT _setrcvtimeout(SOCKET s, INT timeout) {
    return setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (CHAR*) &timeout, sizeof(timeout));
}

//
// Sequential file I/O functions
//
typedef struct _SeqFile SeqFile;
extern UINT cfgSeqFileBufSize;

SeqFile* _CreateFile(const CHAR* filename, DWORD accessMode);
VOID _CloseFile(SeqFile* file);
BOOL _ReadFile(SeqFile* file, CHAR** buf, UINT* buflen);
BOOL _WriteFile(SeqFile* file, const VOID* buf, UINT buflen);

//
// Debugging macros
//
#define BREAK_INTO_DEBUGGER __asm int 3
#define WARNFAIL(_apiname) \
        DbgPrint(#_apiname " failed: %d %d\n", err, GetLastError())

#endif // !_XNETTEST_H

