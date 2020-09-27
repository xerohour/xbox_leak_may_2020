/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    sockinit.c

Abstract:

    Winsock initialization and cleanup related functions

Revision History:

    05/24/2000 davidx
        Created it.

--*/

#include "precomp.h"
#include "phy.h"
#include <xboxverp.h>

#pragma comment(linker, "/include:_XnetBuildNumber")
#pragma data_seg(".XBLD$V")

#if DBG
unsigned short XnetBuildNumber[8] =
        { 'X' | ('N' << 8), 'E' | ('T' << 8), 'D', 0, VER_PRODUCTVERSION | 0x8000 };
#else
unsigned short XnetBuildNumber[8] =
        { 'X' | ('N' << 8), 'E' | ('T' << 8),   0, 0, VER_PRODUCTVERSION };
#endif

#pragma data_seg()

//
// Linker directive to merge XNET_RD section into XNET section
//
#pragma comment(linker, "/merge:XNET_RD=XNET")

//
// Global variable definitions
//
LONG XnetInitCount = 0;
LONG WsaStartupCount = 0;
WORD WinsockVersion = WINSOCK_VERSION;
LONG XnetInitState = XNETSTATE_INVALID;
INITIALIZED_CRITICAL_SECTION(WinsockInitLock);
UINT cfgXnetConfigFlags;

// BUGBUG: temporary workaround for nv2a snooping bug
VOID* (*XnetUncachedAllocProc)(SIZE_T, ULONG);
VOID (*XnetUncachedFreeProc)(VOID*);

#if DBG
INT _winsockTraceFlag = 0;
#endif

#ifdef BUILD_FOR_DEBUGGER
UINT defaultPacketAllocFlag = PKTFLAG_DBGMON;
#else
UINT defaultPacketAllocFlag = 0;
__declspec(thread) WinsockTlsData* WinsockTlsDataPtr;
#endif

//
// Stuff we need to cleanup our per-thread data
// when a thread is deleted.
//
VOID WinsockThreadDetach();

#ifndef BUILD_FOR_DEBUGGER
PRIVATE XTHREAD_NOTIFICATION WinsockThreadNotification;
PRIVATE BOOL WinsockThreadNotifyRegistered;
PRIVATE VOID WINAPI WinsockThreadNotifyProc(BOOL Create) {
    if (!Create) WinsockThreadDetach();
}
#endif

//
// Acquire and release global initialization lock
//
INLINE LONG AcquireWinsockInitLock() {
    EnterCriticalSection(&WinsockInitLock);
    return XnetInitState;
}

INLINE VOID ReleaseWinsockInitLock(LONG state) {
    XnetInitState = state;
    LeaveCriticalSection(&WinsockInitLock);
}


PRIVATE VOID
XnetCleanupInternal()

/*++

Routine Description:

    Internal function to unload the XBox network stack

Arguments:

    NONE

Return Value:

    NONE

--*/

{
    KIRQL irql;

    DnsCleanup();

    // Forcefully close all open sockets
    PcbCloseAll();

    // Clean up the TCP/IP stack
    irql = RaiseToDpc();
    TcpCleanup();
    XnetPoolCleanup();
    #ifdef DVTSNOOPBUG
    XnetUncachedPoolCleanup();
    #endif
    LowerFromDpc(irql);

    WinsockThreadDetach();

    #ifndef BUILD_FOR_DEBUGGER

    if (WinsockThreadNotifyRegistered) {
        XRegisterThreadNotifyRoutine(&WinsockThreadNotification, FALSE);
        WinsockThreadNotifyRegistered = FALSE;
    }

    #endif // !BUILD_FOR_DEBUGGER
}


INT WSAAPI
XnetInitialize(
    const XnetInitParams* initParams,
    BOOL wait
    )

/*++

Routine Description:

    Load XBox network stack

Arguments:

    initParams - Optional initialization parameters
    wait - Whether to wait for the initialization to complete

Return Value:

    Winsock error code

--*/

{
    NTSTATUS status;
    LONG xnetstate;

    xnetstate = AcquireWinsockInitLock();

    // If this is the first XnetInitialize call,
    // initialize the network stack now.

    if (xnetstate == XNETSTATE_INVALID) {
        ASSERT(XnetInitCount == 0);

        #ifndef BUILD_FOR_DEBUGGER

        WinsockThreadNotification.pfnNotifyRoutine = WinsockThreadNotifyProc;
        XRegisterThreadNotifyRoutine(&WinsockThreadNotification, TRUE);
        WinsockThreadNotifyRegistered = TRUE;

        #endif // !BUILD_FOR_DEBUGGER

        // Use the specified configuration parameters
        if (initParams && initParams->structSize == sizeof(XnetInitParams)) {
            // private pool size
            if (initParams->privatePoolSize)
                cfgXnetPoolSize = initParams->privatePoolSize;

            // Ethernet receive queue length
            if (initParams->enetRecvQLength)
                cfgRecvQLength = initParams->enetRecvQLength;

            // max IP reassembly size
            if (initParams->maxIPReassemblySize &&
                initParams->maxIPReassemblySize <= (MAXIPLEN-MAXIPHDRLEN))
                cfgMaxReassemblySize = initParams->maxIPReassemblySize;

            // max IP reassembly datagrams
            if (initParams->maxIPReassemblyDgrams)
                cfgMaxReassemblyDgrams = initParams->maxIPReassemblyDgrams;

            // default send/recv buffer sizes
            if (initParams->defaultSocketRecvBufSize > 0 &&
                initParams->defaultSocketRecvBufSize <= cfgMaxSendRecvBufsize)
                cfgDefaultRecvBufsize = initParams->defaultSocketRecvBufSize;

            if (initParams->defaultSocketSendBufSize > 0 &&
                initParams->defaultSocketSendBufSize <= cfgMaxSendRecvBufsize)
                cfgDefaultSendBufsize = initParams->defaultSocketSendBufSize;

            // default IP datagram TTL and TOS
            cfgDefaultTtl = initParams->defaultIpTTL;
            cfgDefaultTos = initParams->defaultIpTOS;

            // max number of sockets
            if (initParams->maxSockets)
                cfgMaxSockets = initParams->maxSockets;

            // configuration flags
            if (initParams->configFlags)
                cfgXnetConfigFlags = initParams->configFlags;
        }

        XnetInitRandSeed();

        // Initialize our private pool
        status = XnetPoolInit();
        if (!NT_SUCCESS(status)) goto failed;

        // Initialize the DNS client
        status = DnsInitialize();
        if (!NT_SUCCESS(status)) goto failed;

        // Initialize the TCP/IP stack
        status = TcpInitialize();
        if (!NT_SUCCESS(status)) goto failed;

        xnetstate = XNETSTATE_READY;
    }

    XnetInitCount++;
    ReleaseWinsockInitLock(xnetstate);

    // Wait for the initialization to complete if necessary
    if (wait) {
        IfInfo* ifp;
        status = NETERR(WSASYSNOTREADY);
        LOOP_THRU_INTERFACE_LIST(ifp)
            if (IfDhcpEnabled(ifp)) {
                status = DhcpWaitForAddress(ifp);
                break;
            }
        END_INTERFACE_LIST_LOOP()

        if (!NT_SUCCESS(status)) {
            XnetCleanup();
            return RtlNtStatusToDosError(status);
        }
    }

    return NO_ERROR;

failed:
    // Fail to initialize the network stack.
    // Cleanup before returning error.

    XnetCleanupInternal();
    ReleaseWinsockInitLock(xnetstate);
    return RtlNtStatusToDosError(status);
}


INT WSAAPI
XnetCleanup()

/*++

Routine Description:

    Public API function to unload the XBox network stack

Arguments:

    NONE

Return Value:

    Winsock error code

--*/

{
    INT err;
    LONG xnetstate;

    xnetstate = AcquireWinsockInitLock();

    if (xnetstate == XNETSTATE_INVALID) {
        ASSERT(XnetInitCount == 0);
        err = WSASYSNOTREADY;
    } else {
        if (XnetInitCount <= 0) {
            err = WSANOTINITIALISED;
        } else {
            if (--XnetInitCount == 0) {
                ASSERT(WsaStartupCount == 0);
                XnetCleanupInternal();
                xnetstate = XNETSTATE_INVALID;
            }
            err = NO_ERROR;
        }
    }

    ReleaseWinsockInitLock(xnetstate);
    return err;
}


VOID
WinsockThreadDetach()

/*++

Routine Description:

    This is called when a thread dies and we use the chance
    to clean up any per-thread Winsock data we may have allocated.

Arguments:

    NONE

Return Value:

    NONE

--*/

{
    WinsockTlsData* tlsData;
    if ((tlsData = SockGetThreadData()) != NULL) {
        SockSetThreadData(NULL);
        SockFreeThreadData(tlsData);
    }
}


int WSAAPI
WSAStartup(
    IN WORD wVersionRequired,
    OUT LPWSADATA lpWSAData
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    BYTE majorVersion = LOBYTE(wVersionRequired);
    BYTE minorVersion = HIBYTE(wVersionRequired);
    WinsockTlsData* tlsData;
    LONG xnetstate;
    INT err;

    WinsockApiPrologLight_(WSAStartup);
    WinsockApiParamCheck_(lpWSAData != NULL);

    xnetstate = AcquireWinsockInitLock();
    if (xnetstate == XNETSTATE_INVALID) {
        WinsockApiGotoExit_(WSASYSNOTREADY);
    }

    //
    // Version checks
    //
    switch (majorVersion) {
    case 0:
        WinsockApiGotoExit_(WSAVERNOTSUPPORTED);
        break;

    case 1:
        if (minorVersion > 1) minorVersion = 1;
        break;

    case 2:
        if (minorVersion > 2) minorVersion = 2;
        break;

    default:
        majorVersion = minorVersion = 2;
        break;
    }

    WinsockVersion = MAKEWORD(majorVersion, minorVersion);
    TRACE_("WSAStartup: version = %d.%d", majorVersion, minorVersion);

    //
    // Fill in the user structure
    //
    ZeroMem(lpWSAData, sizeof(WSADATA));
    lpWSAData->wVersion = WinsockVersion;
    lpWSAData->wHighVersion = WINSOCK_VERSION;

    if (majorVersion == 1) {
        // Hack for Winsock 1.1
        lpWSAData->iMaxSockets = 0x7fff;
        lpWSAData->iMaxUdpDg = MAXIPLEN-MAXIPHDRLEN-UDPHDRLEN;
    }

    strcpy(lpWSAData->szDescription, "WinSock 2.0");
    strcpy(lpWSAData->szSystemStatus, "Running");

    //
    // Initialize per-thread information
    //
    if ((tlsData = SockGetThreadData()) == NULL &&
        (tlsData = SockAllocThreadData()) == NULL) {
        WinsockApiGotoExit_(WSAENOBUFS);
    }

    SockSetThreadData(tlsData);
    WsaStartupCount++;
    err =  NO_ERROR;

exit:
    ReleaseWinsockInitLock(xnetstate);
    return err;
}


int WSAAPI
WSACleanup()

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    LONG xnetstate;
    INT err;

    xnetstate = AcquireWinsockInitLock();

    if (xnetstate == XNETSTATE_INVALID) {
        err = WSASYSNOTREADY;
    } else {
        if (WsaStartupCount <= 0) {
            err = WSANOTINITIALISED;
        } else {
            if (--WsaStartupCount == 0) {
                // Last call to WSACleanup
                //  perform any necessary cleanup work
                PcbCloseAll();
                WinsockThreadDetach();
            }
            err = NO_ERROR;
        }
    }

    ReleaseWinsockInitLock(xnetstate);

    if (err != NO_ERROR) {
        WARNING_("WSACleanup failed: %d", err);
        SetLastError(err);
        err = SOCKET_ERROR;
    }
    return err;
}


INT
SockEnterApiSlow(
    OUT WinsockTlsData** tlsData
    )

/*++

Routine Description:

    Initialize per-thread Winsock data

Arguments:

    tlsData - Returns a pointer to the per-thread Winsock data

Return Value:

    Error code

--*/

{
    WinsockTlsData* data;

    if (WsaStartupCount <= 0)
        return WSANOTINITIALISED;

    //
    // If this thread has not been initialized, do it now.
    //
    if ((data = SockAllocThreadData()) == NULL)
        return WSAENOBUFS;

    SockSetThreadData(data);
    *tlsData = data;
    return NO_ERROR;
}


WinsockTlsData*
SockAllocThreadData()

/*++

Routine Description:

    Allocate memory for per-thread Winsock data

Arguments:

    NONE

Return Value:

    Pointer to newly created per-thread data structure
    NULL if there is an error

--*/

{
    WinsockTlsData* tlsData;

    tlsData = MAlloc0(sizeof(*tlsData));
    if (!tlsData) return NULL;

    // Perform additional initialization here
    return tlsData;
}


VOID
SockFreeThreadData(
    WinsockTlsData* tlsData
    )

/*++

Routine Description:

    Dispose of per-thread Winsock data

Arguments:

    tlsData - Points to the per-thread data structure to be disposed

Return Value:

    NONE

--*/

{
    if (tlsData) {
        SockSetThreadData(NULL);
        Free(tlsData->strbuf.data);
        Free(tlsData->hostentbuf.data);
        Free(tlsData->serventbuf.data);
        Free(tlsData->protoentbuf.data);
        Free(tlsData);
    }
}


VOID*
SockAllocThreadbuf(
    WinsockThreadbuf* buf,
    UINT size
    )

/*++

Routine Description:

    Allocate (or grow) a temporary per-thread memory buffer

Arguments:

    buf - Points to the buffer structure
    size - Specifies the desired size of the buffer

Return Value:

    Pointer to the buffer data if successful
    NULL if there is an error

--*/

{
    VOID* data;

    // Check if the existing buffer is already ok
    if (size <= buf->size) return buf->data;

    data = MAlloc(size);
    if (data) {
        Free(buf->data);
        buf->data = (CHAR*) data;
        buf->size = size;
    }
    return data;
}


//
// Debug spew functions
//
#if DBG

VOID XnetDbgWarn(CHAR* format, ...) {
    va_list args;
    va_start(args, format);
    vXDebugPrint(XDBG_WARNING, "XNET", format, args);
    va_end(args);
}

VOID XnetDbgVerbose(CHAR* format, ...) {
    va_list args;
    va_start(args, format);
    vXDebugPrint(XDBG_TRACE, "XNET", format, args);
    va_end(args);
}

VOID XnetDbgTrace(CHAR* format, ...) {
    va_list args;
    va_start(args, format);
    vXDebugPrint(XDBG_ENTRY, "XNET", format, args);
    va_end(args);
}

#endif // DBG


//
// NOTE: XnetGetEthernetLinkStatus is placed in the default .text code section.
// This is so that it can be called before XNET code section is loaded.
//
#pragma code_seg(".text")

DWORD WSAAPI
XnetGetEthernetLinkStatus()

/*++

Routine Description:

    Get Ethernet link status

Arguments:

    NONE

Return Value:

    TRUE if Ethernet port is connected to the network
    FALSE otherwise

--*/

{
    // If Xnet is not yet initialized, we may need to
    // initialize the PHY first.
    #ifndef SILVER
    if (XnetInitCount == 0) {
        PhyInitialize(FALSE, NULL);
    }
    #endif

    return PhyGetLinkState(FALSE);
}

#pragma code_seg()

