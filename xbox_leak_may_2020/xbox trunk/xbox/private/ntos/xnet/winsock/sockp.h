/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    sockp.h

Abstract:

    Declarations internal to Winsock implementation

Revision History:

    05/30/2000 davidx
        Created it.

--*/

#ifndef _SOCKP_H
#define _SOCKP_H

//------------------------------------------------------------------------
// Per-thread Winsock data
//------------------------------------------------------------------------

typedef struct _WinsockThreadbuf {
    CHAR* data;
    UINT size;
} WinsockThreadbuf;

typedef struct _WinsockTlsData {
    WinsockThreadbuf strbuf;
    WinsockThreadbuf hostentbuf;
    WinsockThreadbuf serventbuf;
    WinsockThreadbuf protoentbuf;
} WinsockTlsData;

WinsockTlsData* SockAllocThreadData();
VOID SockFreeThreadData(WinsockTlsData* tlsData);

#if defined(BUILD_FOR_DEBUGGER)

INLINE WinsockTlsData* SockGetThreadData() {
    return (WinsockTlsData*) DmGetCurrentDmtd()->WinSockData;
}

INLINE VOID SockSetThreadData(WinsockTlsData* tlsData) {
    DmGetCurrentDmtd()->WinSockData = tlsData;
}

#else // !BUILD_FOR_DEBUGGER

extern __declspec(thread) WinsockTlsData* WinsockTlsDataPtr;

INLINE WinsockTlsData* SockGetThreadData() {
    return WinsockTlsDataPtr;
}

INLINE VOID SockSetThreadData(WinsockTlsData* tlsData) {
    WinsockTlsDataPtr = tlsData;
}

#endif // !BUILD_FOR_DEBUGGER

// Allocate per-thread temporary buffer
VOID* SockAllocThreadbuf(WinsockThreadbuf* buf, UINT size);

//------------------------------------------------------------------------
// Map an API-level socket handle to
// a pointer to our internal socket data structure
//------------------------------------------------------------------------

INLINE PCB* SockLock(SOCKET s) {
    PCB* pcb;
    INT err;
    LONG cookie;

    // NOTE: We don't allow multiple threads to access the same
    // socket simultaneously. If the app tries to do that, the first
    // thread will succeed and other threads will get an error return.

    if (s == 0 || s == INVALID_SOCKET) {
        err = WSAENOTSOCK;
    } else {
        pcb = (PCB*) s;
        cookie = InterlockedCompareExchange(
                    &pcb->magicCookie,
                    BUSY_PCB_COOKIE,
                    ACTIVE_PCB_COOKIE);

        if (cookie == ACTIVE_PCB_COOKIE) return pcb;
        err = (cookie == BUSY_PCB_COOKIE) ? WSAEINPROGRESS : WSAENOTSOCK;
    }

    #if DBG
    #if !defined(BUILD_FOR_DEBUGGER)
    if (err == WSAEINPROGRESS) {
        WARNING_("Thread contention on socket handle: tid = %x, socket = %x",
                 GetCurrentThreadId(), s);
    } else
    #endif
    {
        WARNING_("Invalid socket handle: socket = %x, error = %d", s, err);
    }
    #endif

    SetLastError(err);
    return NULL;
}

INLINE VOID SockUnlock(PCB* pcb) {
    pcb->magicCookie = ACTIVE_PCB_COOKIE;
}

//
// Socket address length
//
#define SOCKADDRLEN ((INT) sizeof(SOCKADDR_IN))

//
// Network stack initialization state
//
extern LONG XnetInitState;

#define XNETSTATE_INVALID   0   // not initialized
#define XNETSTATE_READY     1   // successfully initialized

//
// Find a supported protocol that matches the specified parameters
//
INT
MatchTcpipProtocol(
    IN INT af,
    IN INT type,
    IN INT protocol,
    OUT const WSAPROTOCOL_INFOW** protoinfo
    );

//------------------------------------------------------------------------
// Prolog and epilog code for Winsock API functions
//------------------------------------------------------------------------

INT SockEnterApiSlow(WinsockTlsData** tlsData);

#if DBG

#define SOCKTRACE_WSAStartup                BIT(0)
#define SOCKTRACE_WSACleanup                BIT(0)
#define SOCKTRACE_socket                    BIT(1)
#define SOCKTRACE_closesocket               BIT(1)
#define SOCKTRACE_shutdown                  BIT(1)
#define SOCKTRACE_setsockopt                BIT(2)
#define SOCKTRACE_getsockopt                BIT(2)
#define SOCKTRACE_ioctlsocket               BIT(2)
#define SOCKTRACE_WSARecv                   BIT(3)
#define SOCKTRACE_WSARecvFrom               BIT(3)
#define SOCKTRACE_recv                      BIT(3)
#define SOCKTRACE_recvfrom                  BIT(3)
#define SOCKTRACE_WSASend                   BIT(4)
#define SOCKTRACE_WSASendTo                 BIT(4)
#define SOCKTRACE_send                      BIT(4)
#define SOCKTRACE_sendto                    BIT(4)
#define SOCKTRACE_bind                      BIT(5)
#define SOCKTRACE_connect                   BIT(5)
#define SOCKTRACE_listen                    BIT(5)
#define SOCKTRACE_accept                    BIT(5)
#define SOCKTRACE_getsockname               BIT(5)
#define SOCKTRACE_getpeername               BIT(5)
#define SOCKTRACE_select                    BIT(6)
#define SOCKTRACE_WSAGetOverlappedResult    BIT(6)
#define SOCKTRACE_WSACancelOverlappedIO     BIT(6)
#define SOCKTRACE_inet_addr                 BIT(7)
#define SOCKTRACE_inet_ntoa                 BIT(7)
#define SOCKTRACE_gethostbyaddr             BIT(7)
#define SOCKTRACE_gethostbyname             BIT(7)
#define SOCKTRACE_gethostname               BIT(7)
#define SOCKTRACE_getprotoby_               BIT(7)
#define SOCKTRACE_getprotobyname            BIT(7)
#define SOCKTRACE_getprotobynumber          BIT(7)
#define SOCKTRACE_getservby_                BIT(7)
#define SOCKTRACE_getservbyname             BIT(7)
#define SOCKTRACE_getservbyport             BIT(7)
#define SOCKTRACE_WSAEnumProtocols          BIT(7)
#define SOCKTRACE_XnetInitialize            BIT(8)
#define SOCKTRACE_XnetCleanup               BIT(8)
#define SOCKTRACE_XnetGetIpAddress          BIT(8)

extern INT _winsockTraceFlag;

#define WinsockApiProlog_(_apiname, _result) \
        static const CHAR* fname_ = #_apiname; \
        WinsockTlsData* tlsData; \
        INT err; \
        if (_winsockTraceFlag & SOCKTRACE_##_apiname) { \
            DbgPrint("Entering %s...\n", fname_); \
        } \
        err = (XnetInitState == XNETSTATE_INVALID) ? WSASYSNOTREADY : \
              ((tlsData = SockGetThreadData()) != NULL) ? NO_ERROR : \
              SockEnterApiSlow(&tlsData); \
        WinsockApiCheckError_(_result)

#define WinsockApiPrologLight_(_apiname) \
        static const CHAR* fname_ = #_apiname; \
        if (_winsockTraceFlag & SOCKTRACE_##_apiname) { \
            DbgPrint("Entering %s...\n", fname_); \
        }

#define WinsockApiWarnError_(_err) { \
            if ((_err) != WSA_IO_PENDING && \
                (_err) != WSA_IO_INCOMPLETE && \
                (_err) != WSAETIMEDOUT && \
                (_err) != WSAEWOULDBLOCK) { \
                WARNING_("%s failed: %d", fname_, _err); \
            } \
        }

#define WinsockApiParamCheck_(_exp) { \
            if (!(_exp)) { \
                DbgPrint("%s: %s\n", fname_, #_exp); \
                RIP("bad winsock parameters\n"); \
            } \
        }

#else // !DBG

#define WinsockApiProlog_(_apiname, _result) \
        WinsockTlsData* tlsData; \
        INT err; \
        err = (XnetInitState == XNETSTATE_INVALID) ? WSASYSNOTREADY : \
              ((tlsData = SockGetThreadData()) != NULL) ? NO_ERROR : \
              SockEnterApiSlow(&tlsData); \
        WinsockApiCheckError_(_result)

#define WinsockApiPrologLight_(_apiname)
#define WinsockApiWarnError_(_err)
#define WinsockApiParamCheck_(_exp)

#endif // !DBG

#define WinsockApiReturnError_(_err, _result) { \
            WinsockApiWarnError_(_err); \
            SetLastError(_err); \
            return (_result); \
        }

#define WinsockApiCheckError_(_result) { \
            if (err != NO_ERROR) { \
                WinsockApiReturnError_(err, _result); \
            } \
        }

#define WinsockApiPrologSockLock_(_apiname, _result) \
        PCB* pcb; \
        WinsockApiProlog_(_apiname, _result); \
        pcb = SockLock(s); \
        if (!pcb) return (_result)

#define WinsockApiExitSockUnlock_(_resultOk, _resultErr) \
        exit: SockUnlock(pcb); \
        WinsockApiCheckError_(_resultErr); \
        return (_resultOk)

#define WinsockApiGotoExit_(_err) \
        { err = (_err); goto exit; }

#define MapNtStatusToWinsockError_(_err) \
        ((_err) = NT_SUCCESS(_err) ? NO_ERROR : RtlNtStatusToDosError(_err))

#endif // !_SOCKP_H

