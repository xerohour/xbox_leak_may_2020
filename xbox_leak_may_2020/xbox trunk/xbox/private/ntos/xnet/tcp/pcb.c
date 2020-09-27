/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    pcb.c

Abstract:

    Functions for dealing with PCBs (protocol control blocks)

Revision History:

    06/01/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Global PCB lists
//
LIST_ENTRY PcbList;
ULONG PcbCount;

//
// Default and max send and receive buffer sizes
// Since we don't do TCP window scaling option, the max
// receive buffer size cannot exceed 16-bit (65535).
//
UINT cfgDefaultSendBufsize = 16*1024;
UINT cfgDefaultRecvBufsize = 16*1024;
UINT cfgMinSendRecvBufsize = 1;
UINT cfgMaxSendRecvBufsize = 0xffff;

//
// Maximum number of sockets
//
UINT cfgMaxSockets = 64;


PCB*
PcbCreate(
    INT type,
    INT protocol,
    BYTE allocFlag
    )

/*++

Routine Description:

    Create a new PCB for the specified socket type and protocol

Arguments:

    type - Socket type: SOCK_STREAM, SOCK_DGRAM, or SOCK_RAW
    protocol - Protocol number
    allocFlag - Whether to allocate memory from our private pool
        PCBFLAG_NETPOOL or 0

Return Value:

    Pointer to the newly created PCB structure
    NULL if there is an error

--*/

{
    PCB* pcb;

    if (PcbCount >= cfgMaxSockets) {
        WARNING_("Too many sockets");
        return NULL;
    }

    //
    // Allocate memory out of the system pool or
    // our private pool based on the caller specified flag
    //
    if (allocFlag & PCBFLAG_NETPOOL) {
        pcb = (PCB*) ((type == SOCK_STREAM) ?
                        XnetAlloc0(sizeof(TCB), PTAG_TCB) :
                        XnetAlloc0(sizeof(PCB), PTAG_PCB));
    } else {
        pcb = (PCB*) ((type == SOCK_STREAM) ?
                        SysAlloc0(sizeof(TCB), PTAG_TCB) :
                        SysAlloc0(sizeof(PCB), PTAG_PCB));
    }

    if (!pcb) return NULL;

    pcb->magicCookie = ACTIVE_PCB_COOKIE;
    pcb->flags = allocFlag;
    pcb->type = (BYTE) type;
    pcb->protocol = (BYTE) protocol;
    pcb->maxSendBufsize = cfgDefaultSendBufsize;
    pcb->maxRecvBufsize = cfgDefaultRecvBufsize;
    pcb->ipTtl = cfgDefaultTtl;
    pcb->ipTos = cfgDefaultTos;
    pcb->mcastTtl = 1;

    InitializeListHead(&pcb->recvbuf);
    InitializeListHead(&pcb->sendbuf);
    KeInitializeEvent(GetPcbWaitEvent(pcb), NotificationEvent, FALSE);

    if (type == SOCK_STREAM) {
        TcbInit((TCB*) pcb);
    }

    InterlockedIncrement(&PcbCount);
    return pcb;
}


NTSTATUS
PcbClose(
    PCB* pcb,
    BOOL force
    )

/*++

Routine Description:

    Close a PCB structure

Arguments:

    pcb - Points to the PCB structure to be disposed of
    force - Forceful close, always succeed

Return Value:

    Status code

--*/

{
    KIRQL irql = RaiseToDpc();

    // If a TCP is being gracefully closed, then we'll
    // leave the the PCB structure in the global list
    // until the connection is really gone.
    if (IsTcb(pcb) && !TcbClose((TCB*) pcb, force)) {
        pcb->magicCookie = CLOSED_PCB_COOKIE;
        pcb->flags |= PCBFLAG_BOTH_SHUTDOWN;
        LowerFromDpc(irql);
        return NETERR_OK;
    }

    // Mark the PCB structure as invalid for simple protection
    // against bad apps trying to access closed socket handles.
    pcb->magicCookie = CLOSED_PCB_COOKIE;

    if (!IsListNull(&pcb->links)) {
        RemoveEntryList(&pcb->links);
    }

    // Clean up the information associated with the PCB
    PcbCleanup(pcb, FALSE);

    LowerFromDpc(irql);

    if (pcb->flags & PCBFLAG_NETPOOL) {
        XnetFree(pcb);
    } else {
        SysFree(pcb);
    }

    InterlockedDecrement(&PcbCount);
    return NETERR_OK;
}


VOID
PcbCleanup(
    PCB* pcb,
    BOOL revivable
    )

/*++

Routine Description:

    Clean up the information associated with a PCB

Arguments:

    pcb - Points to the protocol control block
    revivable - Only significant for a TCP socket;
        if TRUE we leave the socket in a revivable state

Return Value:

    NONE

--*/

{
    NTSTATUS status;
    RUNS_AT_DISPATCH_LEVEL

    status = NT_SUCCESS(pcb->errStatus) ? NETERR_CANCELLED : pcb->errStatus;
    PcbClearOverlappedRecvs(pcb, status);
    PcbClearOverlappedSends(pcb, status);

    // Flush send and receive buffers
    PcbFlushRecvBuffers(pcb);

    while (!IsPcbSendBufEmpty(pcb)) {
        SENDBUF* sendbuf = (SENDBUF*) RemoveHeadList(&pcb->sendbuf);
        SendbufRelease(sendbuf);
    }
    pcb->sendbufSize = 0;

    if (pcb->rte) {
        IpReleaseCachedRTE(pcb->rte);
        pcb->rte = NULL;
    }

    if (revivable) {
        pcb->flags |= PCBFLAG_REVIVABLE;
    } else {
        if (pcb->bindIfp)
            pcb->bindIfp = NULL;

        PcbCleanupMcastData(pcb);
        PcbFreeIpOpts(pcb);
    }
}


VOID
PcbCloseAll()

/*++

Routine Description:

    Forcefully close all sockets

Arguments:

    NONE

Return Value:

    NONE

--*/

{
    PCB* pcb;

    if (IsListNull(&PcbList)) return;
    while (!IsPcbListEmpty()) {
        pcb = (PCB*) PcbList.Flink;
        PcbDelete(pcb);
    }

    if (IsListNull(&DeadTcbList)) return;
    while (!IsListEmpty(&DeadTcbList)) {
        pcb = (PCB*) DeadTcbList.Flink;
        PcbDelete(pcb);
    }
}


PCB*
PcbFindMatch(
    IPADDR toaddr,
    IPPORT toport,
    IPADDR fromaddr,
    IPPORT fromport,
    BYTE type,
    BYTE protocol
    )

/*++

Routine Description:

    Find the matching socket for a received datagarm

Arguments:

    toaddr, toport - The destination address of the datagram
    fromaddr, fromport - The sender's address
    type - Specifies the socket type (SOCK_DGRAM or SOCK_RAW)
    protocol - Specifies the protocol number

Return Value:

    Points to the best socket to receive the datagram
    NULL if no matching socket is found

--*/

{
    PCB* pcb;
    PCB* found;
    UINT wildcard, minWildcard;

    RUNS_AT_DISPATCH_LEVEL
    found = NULL;
    minWildcard = 4;

    LOOP_THRU_PCB_LIST(pcb)

        if (pcb->type != type ||
            pcb->protocol != protocol && pcb->protocol != 0 ||
            pcb->srcport != toport)
            continue;

        wildcard = 0;
        if (pcb->srcaddr != toaddr) {
            if (pcb->srcaddr) continue;
            wildcard++;
        }

        if (pcb->dstaddr != fromaddr) {
            if (pcb->dstaddr) continue;
            wildcard++;
        }

        if (pcb->dstport != fromport) {
            if (pcb->dstport) continue;
            wildcard++;
        }

        if (wildcard == 0) return pcb;
        if (wildcard < minWildcard) {
            found = pcb;
            minWildcard = wildcard;
        }

    END_PCB_LIST_LOOP
    return found;
}


NTSTATUS
PcbUpdateBufferSize(
    PCB* pcb,
    INT sendBufsize,
    INT recvBufsize
    )

/*++

Routine Description:

    Update the send and receive buffer sizes

Arguments:

    pcb - Points to the PCB structure
    sendBufsize, recvBufsize - 
        Specifies the new send and receive buffer sizes

Return Value:

    Status code

--*/

{
    KIRQL irql;
    NTSTATUS status;

    if (sendBufsize > (INT) cfgMaxSendRecvBufsize)
        sendBufsize = (INT) cfgMaxSendRecvBufsize;
    else if (sendBufsize <= 0) {
        // NOTE: we never set actual send buffer size to 0
        // because we don't support the no-buffering option.
        sendBufsize = 1;
    }

    if (recvBufsize > (INT) cfgMaxSendRecvBufsize)
        recvBufsize = (INT) cfgMaxSendRecvBufsize;
    else if (recvBufsize <= 0)
        recvBufsize = 0;

    status = NETERR_OK;
    irql = RaiseToDpc();

    if (IsDgramPcb(pcb)) {
        // For datagram sockets, we'll just update the send and
        // receive buffers sizes. If the current buffers are bigger
        // than the specified limits, we'll leave the current data alone.
        pcb->maxSendBufsize = sendBufsize;
        pcb->maxRecvBufsize = recvBufsize;
    } else {
        TCB* tcb = (TCB*) pcb;

        // Set the send buffer size. If the current send buffer size
        // is larger than the specified limit, leave the current data untouched.
        tcb->maxSendBufsize = sendBufsize;
        
        // If the TCP socket is already connected,
        // don't allow the app to reduce the receive buffer size.
        if (!IsTcpIdleState(tcb) && recvBufsize < (INT) tcb->maxRecvBufsize) {
            status = NETERR_PARAM;
        } else {
            // NOTE: we don't update receive window to the connection peer
            // right away. The new window information will be sent
            // in the next outgoing ACK segment.
            tcb->maxRecvBufsize = recvBufsize;
        }
    }

    // NOTE: If the send buffer has just opened up,
    // we don't check to see if there is any pending
    // overlapped send request that can be started.
    // Rather the overlapped send request will be started
    // by the normal process.

    LowerFromDpc(irql);
    return status;
}


//
// Temporary port numbers are between 1024 and 4999 (inclusive)
// We're not using port number 5000 just as a safety precaution.
//
#define MIN_TEMP_PORT 1024
#define MAX_TEMP_PORT 4999
#define TEMP_PORT_COUNT (MAX_TEMP_PORT-MIN_TEMP_PORT+1)

INLINE IPPORT GenerateTempPort() {
    static USHORT nextTempPort = MAX_TEMP_PORT;
    if (++nextTempPort > MAX_TEMP_PORT)
        nextTempPort = MIN_TEMP_PORT;
    return HTONS(nextTempPort);
}


NTSTATUS
PcbBind(
    PCB* pcb,
    IPADDR srcaddr,
    IPPORT srcport
    )

/*++

Routine Description:

    Bind a socket to the specified local address

Arguments:

    pcb - Points to the protocol control block
    srcaddr, srcport - Specifies the local socket address

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    INT retries = 1;
    KIRQL irql = RaiseToDpc();

    ASSERT(!IsPcbBound(pcb));

    if (pcb->type == SOCK_RAW) {
        // Port number is meaningless for raw sockets
        // and we always set it to 0.
        srcport = 0;
    } else if (srcport == 0) {
        // We're picking a randomly-generated port number.
        retries = min(cfgMaxSockets, TEMP_PORT_COUNT);
        srcport = GenerateTempPort();
    }

    while (TRUE) {
        PCB* cur;

        status = NETERR_OK;
        LOOP_THRU_PCB_LIST(cur)

            if (cur == pcb || !IsPcbBound(cur)) continue;

            // NOTE: Different raw sockets can bind to
            // the same address/protocol pair. This is
            // to follow win2k behavior.

            if ((pcb->type != SOCK_RAW) &&
                (cur->protocol == pcb->protocol) &&
                (cur->srcport == srcport) &&    // conflicting port number?
                (cur->bindSrcAddr == srcaddr || // conflicting address?
                 cur->bindSrcAddr == 0 ||
                 srcaddr == 0) &&
                (cur->exclusiveAddr ||          // address reuse disallowed?
                 pcb->exclusiveAddr ||
                 !pcb->reuseAddr)) {
                status = NETERR_ADDRINUSE;
                break;
            }

        END_PCB_LIST_LOOP

        if (status == NETERR_OK) {
            // If we're binding to a specific IP address,
            // loop through all the interfaces and
            // select the one with the matching address.
            if (srcaddr != 0) {
                IfInfo* ifp;

                ifp = IfFindInterface(srcaddr);
                if (ifp == NULL) {
                    status = NETERR(WSAEADDRNOTAVAIL);
                    break;
                }

                ASSERT(pcb->bindIfp == NULL);
                CACHE_IFP_REFERENCE(pcb->bindIfp, ifp);
            }
    
            pcb->srcaddr = pcb->bindSrcAddr = srcaddr;
            pcb->srcport = srcport;
            pcb->flags |= PCBFLAG_BOUND;
        } else {
            // The selected port is not available.
            // Check to see if we should try another one.
            if (--retries) {
                srcport = GenerateTempPort();
                continue;
            }
        }

        break;
    }

    LowerFromDpc(irql);
    return status;
}


NTSTATUS
PcbConnectDgram(
    PCB* pcb,
    IPADDR dstaddr,
    IPPORT dstport
    )

/*++

Routine Description:

    Connect a datagram socket to the specified foreign address

Arguments:

    pcb - Points to the protocol control block
    dstaddr, dstport - Specifies the foreign socket address

Return Value:

    Status code

--*/

{
    KIRQL irql;
    NTSTATUS status;

    if (pcb->type == SOCK_RAW) {
        // Port number for raw sockets is meaningless
        dstport = 0;
    } else {
        if (dstaddr == 0 && dstport != 0 ||
            dstaddr != 0 && dstport == 0)
            return NETERR(WSAEADDRNOTAVAIL);
    }

    // Destination hasn't changed, no need to do anything
    if (dstaddr == pcb->dstaddr && dstport == pcb->dstport)
        return NETERR_OK;

    // Is this socket allowed to send broadcast
    // datagrams on this socket?
    if (IS_BCAST_IPADDR(dstaddr) && !pcb->broadcast)
        return NETERR(WSAEACCES);

    irql = RaiseToDpc();

    // If the socket is currently connected,
    // we need to disconnect it first.
    if (IsPcbConnected(pcb)) {
        pcb->flags &= ~PCBFLAG_CONNECTED;
        pcb->dstaddr = 0;
        pcb->dstport = 0;
        pcb->srcaddr = pcb->bindSrcAddr;
        if (pcb->rte) {
            IpReleaseCachedRTE(pcb->rte);
            pcb->rte = NULL;
        }
    }

    // Discard any received packets that have been
    // queued up but not yet processed
    PcbFlushRecvBuffers(pcb);

    if (dstaddr != 0) {
        // Bind to a local address if necessary
        if (!IsPcbBound(pcb)) {
            status = PcbBind(pcb, 0, 0);
            if (!NT_SUCCESS(status)) {
                LowerFromDpc(irql);
                return status;
            }
        }
        
        if (!IS_MCAST_IPADDR(dstaddr)) {
            // Find a route to the connected destination.
            // And if we're bound to a wildcard source address,
            // then we'll pick a specific source address here
            // based on the route.

            ASSERT(pcb->rte == NULL);
            pcb->rte = IpFindRTE(dstaddr, NULL);
            if (!pcb->rte) {
                LowerFromDpc(irql);
                return NETERR_UNREACHABLE;
            }

            RteAddRef(pcb->rte);
            if (pcb->bindSrcAddr == 0) {
                pcb->srcaddr = PcbGetDefaultSrcAddr(pcb->rte);
            }
        }

        pcb->flags |= PCBFLAG_CONNECTED;
        pcb->dstaddr = dstaddr;
        pcb->dstport = dstport;
    }

    LowerFromDpc(irql);
    return NETERR_OK;
}


VOID
PcbSetupIpHeader(
    PCB* pcb,
    Packet* pkt,
    IpAddrPair* addrpair
    )

/*++

Routine Description:

    Prepare the IP header information in an outgoing packet
    using the connection information from the PCB.

Arguments:

    pcb - Points to a connected PCB
    pkt - Points to the outgoing packet
    addrpair - Specifies the source and destination address pair

Return Value:

    NONE

Note:

    This function really belong to the ..\ip\ipsend.c.
    We duplicate the code here to save a function call (with
    large number of parameters).

--*/

{
    UINT iphdrlen;
    IpHeader* iphdr;

    iphdrlen = IPHDRLEN + ROUNDUP4(pcb->ipoptlen);
    pkt->data -= iphdrlen;
    iphdr = (IpHeader*) pkt->data;
    SETPKTIPHDR(pkt, iphdr);
    pkt->datalen += iphdrlen;

    if (pcb->ipoptlen) {
        ZeroMem(iphdr+1, iphdrlen-IPHDRLEN);
        CopyMem(iphdr+1, pcb->ipopts, pcb->ipoptlen);
    }

    FILL_IPHEADER(
        iphdr,
        iphdrlen,
        pcb->ipTos,
        pkt->datalen,
        pcb->ipDontFrag ? HTONS(DONT_FRAGMENT) : 0,
        IS_BCAST_IPADDR(addrpair->dstaddr) ? 1 :
            IS_MCAST_IPADDR(addrpair->dstaddr) ? pcb->mcastTtl : pcb->ipTtl,
        pcb->protocol,
        addrpair->srcaddr,
        addrpair->dstaddr);
}


NTSTATUS
PcbWaitForEvent(
    PCB* pcb,
    INT eventMask,
    UINT timeout
    )

/*++

Routine Description:

    Block the current thread until the specified PCB event is signalled

Arguments:

    pcb - Points to the protocol control block
    eventMask - Flag bit to indicate which event to block on
    timeout - Specifies the wait timeout (in milliseconds, 0 means forever)

Return Value:

    Status code

--*/

{
    INT readyMask;
    NTSTATUS status = NETERR_OK;

    //
    // Check if the specified event is already available
    // or if the socket connection has been reset.
    //
    readyMask = PcbCheckSelectEvents(pcb, eventMask, 1);
    if (readyMask & PCBEVENT_CONNRESET) goto exit;
    if (readyMask != 0) return NETERR_OK;

    status = WaitKernelEventObject(GetPcbWaitEvent(pcb), timeout);
    PcbClearSelectEvents(pcb);

exit:
    return NT_SUCCESS(status) ? PcbGetErrStatus(pcb) : status;
}


INT
PcbCheckSelectEvents(
    PCB* pcb,
    INT eventMasks,
    INT setwait
    )

/*++

Routine Description:

    Check if the specified socket events are available
    and optionally set up the socket to wait for them

Arguments:

    pcb - Points to the protocol control block
    eventMasks - Specifies the socket events the caller is interested in
    setwait - Whether to set up the sockets to wait if 
        none of the specified events are avaiable

Return Value:

    Set of event flags that are already available

--*/

{
    NTSTATUS status;
    INT readyMasks;
    KIRQL irql = RaiseToDpc();

    // Check to see if the specified event is already available
    // Since our checks are trivial, it's faster to check
    // everything instead of trying to check selectively
    // based on the flags specified by the caller.
    
    if (IsTcb(pcb)) {
        // If the connection was reset, return reset status
        status = PcbGetErrStatus(pcb);
        if (!NT_SUCCESS(status)) {
            // Note: If we already told the app that the socket was connected
            // and then the socket got resetted, we don't need to set the socket
            // in the exceptfds again to tell the app the connection has failed.
            if (eventMasks != PCBEVENT_CONNRESET || !pcb->connectSelected) {
                LowerFromDpc(irql);
                return PCBEVENT_CONNRESET;
            }
        }

        readyMasks = (IsTcpRecvBufEmpty(pcb) ? 0 : PCBEVENT_READ) |
                     (TcbHasPendingConnReq((TCB*) pcb) ? PCBEVENT_ACCEPT : 0) |
                     (IsFINReceived(pcb) ? PCBEVENT_READ|PCBEVENT_CLOSE : 0);

        if (IsPcbConnected(pcb)) {
            if (!IsPcbSendBufFull(pcb)) readyMasks |= PCBEVENT_WRITE;

            // NOTE: we only signal the connect event exactly once
            if ((eventMasks & PCBEVENT_CONNECT) && !pcb->connectSelected) {
                readyMasks |= PCBEVENT_CONNECT;
                if (setwait >= 0) pcb->connectSelected = 1;
            }
        }
    } else {
        readyMasks = (IsDgramRecvBufEmpty(pcb) ? 0 : PCBEVENT_READ) |
                     (IsPcbSendBufFull(pcb) ? 0 : PCBEVENT_WRITE);
    }

    if ((readyMasks &= eventMasks) == 0 && setwait) {
        // Indicate that we're interested in the specified event
        // and prepare to wait
        pcb->eventFlags = eventMasks;
        KeClearEvent(GetPcbWaitEvent(pcb));
    }

    LowerFromDpc(irql);
    return readyMasks;
}


VOID
PcbCompleteOverlappedSendRecv(
    PcbOverlappedReq* req,
    NTSTATUS status
    )

/*++

Routine Description:

    Complete an overlapped send/receive request

Arguments:

    req - Points to the overlapped send/receive request
    status - Specifies the completion status

Return Value:

    NONE

--*/

{
    PCB* pcb = req->pcb;

    // NOTE: we can have at most 1 overlapped send/receive request
    if ((RECVREQ*) req == pcb->overlappedRecvs) {
        pcb->overlappedRecvs = NULL;
    } else if ((SENDREQ*) req == pcb->overlappedSends) {
        pcb->overlappedSends = NULL;
    }

    req->overlapped->_iostatus = status;
    SetKernelEvent(req->overlappedEvent);

    // If the wait event is our internal per-PCB event,
    // then this is a special case for implementing the
    // blocking recv call. See comments in PcbQueueOverlappedRecv.

    if (req->overlappedEvent != GetPcbWaitEvent(req->pcb)) {
        ObDereferenceObject(req->overlappedEvent);
        SysFree(req);
    }
}


NTSTATUS
PcbQueueOverlappedRecv(
    PCB* pcb,
    RECVREQ* recvreq
    )

/*++

Routine Description:

    Queue up an overlapped receive request

Arguments:

    pcb - Points to the protocol control block
    recvreq - Points to the receive request

Return Value:

    Status code

--*/

{
    RECVREQ* newreq;

    // Queue up an overlapped receive request.
    // We only support 1 outstanding overlapped receive request.
    if (HasOverlappedRecv(pcb) || !recvreq->overlappedEvent)
        return NETERR_WOULDBLOCK;

    if (recvreq->overlappedEvent == GetPcbWaitEvent(pcb)) {
        // If the wait event is our internal per-PCB event,
        // then this is a special case for implementing the
        // blocking recv call.
        //
        // In this case, we avoid an allocation by directly
        // queuing up the RECVREQ structure that was passed
        // in from the caller (winsock layer). This works because
        // the caller will wait for the recv to complete after
        // this function returns.

        newreq = recvreq;
    } else {
        newreq = (RECVREQ*) SysAlloc(sizeof(RECVREQ), PTAG_RREQ);
        if (!newreq) return NETERR_MEMORY;
        *newreq = *recvreq;
    }

    pcb->overlappedRecvs = newreq;
    newreq->pcb = pcb;
    newreq->overlapped->_ioxfercnt = 0;
    newreq->overlapped->_ioflags = 0;
    newreq->overlapped->_ioreq = (UINT_PTR) newreq;
    newreq->overlapped->_iostatus = (DWORD) NETERR_PENDING;
    return NETERR_PENDING;
}


NTSTATUS
PcbQueueOverlappedSend(
    PCB* pcb,
    SENDREQ* sendreq
    )

/*++

Routine Description:

    Queue up an overlapped send request

Arguments:

    pcb - Points to the protocol control block
    sendreq - Points to the overlapped send request

Return Value:

    Status code:
        NETERR_PENDING - the request was successfully queued up
        NETERR_OK - if the send buffer has opened up and
            there is no need to queue up the request
        otherwise - the request was not queued up due to an error

--*/

{
    NTSTATUS status;
    KIRQL irql = RaiseToDpc();

    if (HasOverlappedSend(pcb)) {
        // We only support 1 outstanding overlapped send request.
        status = NETERR_WOULDBLOCK;
    } else if (!IsPcbSendBufFull(pcb)) {
        // The send buffer opened up just as
        // we were raising to DPC level
        status = NETERR_OK;
    } else {
        SENDREQ* newreq;
        UINT size = sizeof(SENDREQ) +
                    (sendreq->toaddr ? sizeof(*sendreq->toaddr) : 0) +
                    sizeof(WSABUF) * sendreq->bufcnt;

        newreq = (SENDREQ*) SysAlloc(size, PTAG_RREQ);
        if (!newreq) {
            // Out of memory
            status = NETERR_MEMORY;
        } else {
            VOID* bufs;

            pcb->overlappedSends = newreq;
            *newreq = *sendreq;
            bufs = newreq+1;
            if (sendreq->toaddr) {
                newreq->toaddr = (struct sockaddr_in*) bufs;
                *newreq->toaddr = *sendreq->toaddr;
                bufs = newreq->toaddr + 1;
            }

            newreq->bufs = (WSABUF*) bufs;
            CopyMem(bufs, sendreq->bufs, sizeof(WSABUF) * sendreq->bufcnt);

            newreq->pcb = pcb;
            newreq->overlapped->_ioxfercnt = 0;
            newreq->overlapped->_ioflags = 0;
            newreq->overlapped->_ioreq = (UINT_PTR) newreq;
            newreq->overlapped->_iostatus = status = NETERR_PENDING;
        }
    }

    LowerFromDpc(irql);
    return status;
}

