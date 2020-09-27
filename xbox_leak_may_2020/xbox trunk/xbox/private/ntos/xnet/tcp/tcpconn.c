/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tcpconn.c

Abstract:

    TCP connection management functions

Revision History:

    06/05/2000 davidx
        Created it.

--*/

#include "precomp.h"


//
// Maximum segment lifetime (30 seconds)
//
UINT cfgMSL = 30 * SLOW_TCP_TIMER_FREQ;

//
// Timeout period for active connection requests
// NOTE: Non-standard behavior!!!
//  Our connection timeout value of 20sec is much shorter
//  than the usual value of 75sec.
//
UINT cfgConnectTimeout = 20 * SLOW_TCP_TIMER_FREQ;

//
// Maximum backlogs for a listening socket
//
INT cfgMaxListenBacklog = 5;

//
// Default linger time when a connection is gracefully closed (2 minutes)
//
USHORT cfgDefaultLingerTime = 2*60;

//
// Default retransmission timeout (3 seconds)
//
UINT cfgDefaultRTO = 3*SLOW_TCP_TIMER_FREQ;

//
// List of TCP connections that has been reset but not yet closed
//
LIST_ENTRY DeadTcbList;


VOID
TcbInit(
    TCB* tcb
    )

/*++

Routine Description:

    Initialize a TCP control block

Arguments:

    tcb - Points to a newly allocated TCB structure

Return Value:

    NONE

Note:

    We assume all fields have been zero-initialized
    when this function is called.

--*/

{
    InitializeListHead(&tcb->listenq);
    tcb->linger.l_linger = cfgDefaultLingerTime;

    tcb->snd_mss = tcb->rcv_mss = TCP_DEFAULT_MSS;
    tcb->snd_cwnd = tcb->snd_ssthresh = MAX_TCP_WNDSIZE;
    tcb->srtt_8 = 0;
    tcb->rttvar_4 = tcb->RTO = cfgDefaultRTO;

    tcb->sendbufNext = TcbSendbufNil(tcb);
}


//
// Disassociate a child socket (i.e. pending connection
// request for a listening socket) from its parent
//
PRIVATE VOID TcbDisassociateChild(TCB* childTcb)
{
    RemoveEntryList(&childTcb->listenq);
    InitializeListHead(&childTcb->listenq);
    childTcb->parent->backlog--;
    childTcb->parent = NULL;
}


//
// Trim the backlog of pending connection requests
//
INLINE VOID TcbTrimConnReqBacklog(TCB* tcb, UINT maxBacklog) {
    while (tcb->backlog > maxBacklog) {
        TCB* childTcb = CONTAINING_RECORD(tcb->listenq.Blink, TCB, listenq);
        TcbDisassociateChild(childTcb);
        TcbDelete(childTcb);
    }
}


BOOL
TcbClose(
    TCB* tcb,
    BOOL forceful
    )

/*++

Routine Description:

    Close a TCP connection

Arguments:

    tcb - Points to the TCB structure
    forceful - Forceful close, always succeed and no wait

Return Value:

    TRUE if the TCB structure can be deleted immediately.
    FALSE if the connection is being gracefully closed
    and the TCB structure should be kept around until
    the connection is really closed.

--*/

{
    NTSTATUS status;

    //
    // Try to gracefully close the socket if requested by the caller
    //
    if (!forceful && (tcb->tcpstate >= TCPST_SYN_RECEIVED) &&
        (!tcb->linger.l_onoff || tcb->linger.l_linger > 0)) {
        status = TcbShutdown(tcb, PCBFLAG_BOTH_SHUTDOWN, FALSE);
        if (NT_SUCCESS(status)) {
            UINT linger = tcb->linger.l_onoff ?
                            tcb->linger.l_linger :
                            cfgDefaultLingerTime;

            // TIME-WAIT and linger timers are the same
            TcbSetTimeWaitTimer(tcb, linger * SLOW_TCP_TIMER_FREQ);
            return FALSE;
        }
    }

    if (tcb->parent) {
        // This was a pending connection request
        // for a listening socket.
        TcbDisassociateChild(tcb);
    } else {
        // Clear all pending connection requests
        TcbTrimConnReqBacklog(tcb, 0);
    }

    return TRUE;
}


NTSTATUS
TcbShutdown(
    TCB* tcb,
    BYTE flags,
    BOOL apicall
    )

/*++

Routine Description:

    Gracefully shutdown a TCP connection

Arguments:

    tcb - Points to the TCP control block
    flags - Shutdown flags: PCBFLAG_SEND_SHUTDOWN and/or PCBFLAG_RECV_SHUTDOWN
    apicall - Whether this is called by shutdown API

Return Value:

    Status code

--*/

{
    NTSTATUS status = NETERR_OK;
    KIRQL irql = RaiseToDpc();

    if ((flags & PCBFLAG_RECV_SHUTDOWN) && !IsPcbRecvShutdown(tcb)) {
        tcb->flags |= PCBFLAG_RECV_SHUTDOWN;
        if (!IsTcpRecvBufEmpty(tcb)) {
            //
            // If the receive buffer is not empty,
            // we'll send a RST to the connection peer
            // and reset the connection.
            //
            TcbResetPeer(tcb);
            status = apicall ? NETERR_OK : NETERR_CONNRESET;
            tcb = TcbReset(tcb, NETERR_CONNRESET);
            ASSERT(tcb != NULL);
        } else {
            // Cancel any pending overlapped receive requests
            PcbClearOverlappedRecvs(tcb, NETERR(WSAESHUTDOWN));
        }
    }

    if ((flags & PCBFLAG_SEND_SHUTDOWN) && !IsPcbSendShutdown(tcb)) {
        // Cancel any pending overlapped send requests
        PcbClearOverlappedSends(tcb, NETERR(WSAESHUTDOWN));

        switch (tcb->tcpstate) {
        case TCPST_SYN_RECEIVED:
        case TCPST_ESTABLISHED:
            status = TcbEmitFIN(tcb);
            if (NT_SUCCESS(status)) {
                TcbSetState(tcb, TCPST_FIN_WAIT_1, "Shutdown");
                tcb->flags |= PCBFLAG_SEND_SHUTDOWN;
            }
            break;

        case TCPST_CLOSE_WAIT:
            status = TcbEmitFIN(tcb);
            if (NT_SUCCESS(status)) {
                TcbSetState(tcb, TCPST_LAST_ACK, "Shutdown");
                tcb->flags |= PCBFLAG_SEND_SHUTDOWN;
            }
            break;

        default:
            ASSERT(tcb->tcpstate < TCPST_SYN_RECEIVED);
            status = NETERR(WSAENOTCONN);
            break;
        }
    }

    flags &= ~PCBFLAG_REVIVABLE;
    LowerFromDpc(irql);
    return status;
}


TCB*
TcbReset(
    TCB* tcb,
    NTSTATUS status
    )

/*++

Routine Description:

    Reset a TCP connection

Arguments:

    tcb - Points to the TCP control block
    status - Specifies the cause of the reset

Return Value:

    NULL if the specified TCB was deleted inside this call.
    Otherwise, just return the input tcb parameter.

--*/

{
    BOOL revivable;

    ASSERT(!IsTcpIdleState(tcb));
    
    if (IsPendingConnReqTcb(tcb) || TcbIsLingering(tcb)) {
        TcbDelete(tcb);
        return NULL;
    }

    // Remove the TCB from the active list
    // and move it to the zombie list.

    ASSERT(!IsListNull(&tcb->links));
    RemoveEntryList(&tcb->links);
    InsertHeadList(&DeadTcbList, &tcb->links);

    tcb->errStatus = (status != NETERR_CONNRESET || IsTcpSyncState(tcb)) ?
                            status :
                            NETERR(WSAECONNREFUSED);

    revivable = !IsTcpSyncState(tcb) &&
                (tcb->flags & PCBFLAG_BOTH_SHUTDOWN) == 0;

    tcb->flags |= PCBFLAG_BOTH_SHUTDOWN;
    TcbSetState(tcb, TCPST_CLOSED, "Reset");

    // Stop various timers
    // NOTE: This is a little hacky - we're clearing all the fields
    // from delayedAcks to rtt_tick in one shot. Be careful when you
    // add or remove fields from the TCB structure.

    ZeroMem(
        &tcb->delayedAcks,
        offsetof(TCB, rtt_tick) + sizeof(tcb->rtt_tick) - offsetof(TCB, delayedAcks));

    TcbInit(tcb);

    PcbSignalEvent(tcb, PCBEVENT_ALL);
    PcbCleanup((PCB*) tcb, revivable);
    return tcb;
}


TCB*
TcbCloneChild(
    TCB* tcb
    )

/*++

Routine Description:

    Add a pending connection request to a listening socket

Arguments:

    tcb - Points to the listening socket

Return Value:

    Pointer to the child socket
    NULL if there is an error

--*/

{
    TCB* childTcb;

    // Fail if the backlog is full
    ASSERT(tcb->tcpstate == TCPST_LISTEN);
    if (tcb->backlog >= tcb->maxBacklog)
        return NULL;
    
    // Allocate memory for the child socket
    childTcb = (TCB*) PcbCreate(SOCK_STREAM, IPPROTOCOL_TCP, PCBFLAG_NETPOOL);
    if (!childTcb) return NULL;

    // Associate the child socket with its parent
    childTcb->parent = tcb;
    tcb->backlog++;
    InsertTailList(&tcb->listenq, &childTcb->listenq);

    // Copy the local address bindings
    childTcb->flags |= PCBFLAG_BOUND;
    childTcb->bindSrcAddr = tcb->bindSrcAddr;
    childTcb->srcaddr = tcb->srcaddr;
    childTcb->srcport = tcb->srcport;
    if (tcb->bindIfp) {
        CACHE_IFP_REFERENCE(childTcb->bindIfp, tcb->bindIfp);
    }

    // Copy socket options
    childTcb->options = tcb->options;
    if (tcb->ipoptlen) {
        PcbSetIpOpts((PCB*) childTcb, tcb->ipopts, tcb->ipoptlen);
    } else {
        ASSERT(childTcb->ipopts == NULL);
    }

    return childTcb;
}


NTSTATUS
TcbListen(
    TCB* tcb,
    INT backlog
    )

/*++

Routine Description:

    Switch a TCP socket into listening state

Arguments:

    tcb - Points to the TCP control block
    backlog - Maximum number of pending connections

Return Value:

    Status code

--*/

{
    KIRQL irql;
    NTSTATUS status;

    if (!IsTcpIdleState(tcb))
        return NETERR(WSAEISCONN);

    irql = RaiseToDpc();

    status = PcbGetErrStatus(tcb);
    if (NT_SUCCESS(status)) {
        TcbSetState(tcb, TCPST_LISTEN, "Listen");
        tcb->maxBacklog = (backlog < 1) ? 1 :
                          (backlog > cfgMaxListenBacklog) ?
                            cfgMaxListenBacklog :
                            backlog;

        // Trim down the backlog of pending connections if needed
        TcbTrimConnReqBacklog(tcb, tcb->maxBacklog);
        status = NETERR_OK;
    }

    LowerFromDpc(irql);
    return status;
}


NTSTATUS
TcbAccept(
    TCB* tcb,
    TCB** newtcb
    )

/*++

Routine Description:

    Accept an incoming TCP connection request

Arguments:

    tcb - Points to the TCP control block
    newtcb - Returns a pointer to the newly accepted connection block

Return Value:

    Status code

--*/

{
    KIRQL irql;
    LIST_ENTRY* entry;
    NTSTATUS status = NETERR_CONNRESET;

    irql = RaiseToDpc();

    entry = tcb->listenq.Flink;
    while (entry != &tcb->listenq) {
        TCB* childTcb = CONTAINING_RECORD(entry, TCB, listenq);
        if (IsPcbConnected(childTcb)) {
            TcbDisassociateChild(childTcb);
            *newtcb = childTcb;
            status = NETERR_OK;
            break;
        }
        entry = entry->Flink;
    }

    LowerFromDpc(irql);
    return status;
}


BOOL
TcbHasConnectedPendingConnReq(
    TCB* tcb
    )

/*++

Routine Description:

    Check to see if a listening socket has pending connection
    requests that are in ESTABLISHED state.

Arguments:

    TCB - Points to the TCP control block

Return Value:

    TRUE if there is pending connections ready to be accepted
    FALSE otherwise

--*/

{
    KIRQL irql;
    LIST_ENTRY* entry;
    BOOL result = FALSE;

    irql = RaiseToDpc();

    entry = tcb->listenq.Flink;
    while (entry != &tcb->listenq) {
        TCB* childTcb = CONTAINING_RECORD(entry, TCB, listenq);
        if (IsPcbConnected(childTcb)) {
            result = TRUE;
            break;
        }
        entry = entry->Flink;
    }

    LowerFromDpc(irql);

    return result;
}


NTSTATUS
TcbConnect(
    TCB* tcb,
    IPADDR dstaddr,
    IPPORT dstport,
    BOOL synAck
    )

/*++

Routine Description:

    Make a TCP connection to the specified foreign address

Arguments:

    tcb - Points to the TCP control block
    dstaddr, dstport - Specifies the foreign address to connect to
    synAck - Whether we're acknowledging a received SYN request

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    KIRQL irql;
    RTE* rte;

    if (dstport == 0 ||
        dstaddr == 0 ||
        IS_BCAST_IPADDR(dstaddr) ||
        IS_MCAST_IPADDR(dstaddr))
        return NETERR(WSAEADDRNOTAVAIL);

    if (IsTcpListenState(tcb))
        return NETERR_PARAM;

    // Bind to a local address if necessary
    if (!IsPcbBound(tcb)) {
        status = PcbBind((PCB*) tcb, 0, 0);
        if (!NT_SUCCESS(status)) return status;
    }

    irql = RaiseToDpc();

    if (tcb->flags & PCBFLAG_REVIVABLE) {
        // Revive a TCB that has been reset
        tcb->connectSelected = 0;
        tcb->flags &= ~(PCBFLAG_REVIVABLE|PCBFLAG_BOTH_SHUTDOWN);
        tcb->eventFlags = 0;
        KeClearEvent(GetPcbWaitEvent(tcb));
        tcb->errStatus = NETERR_OK;

        ASSERT(IsListEmpty(&tcb->listenq));
        TcbInit(tcb);

        // Move the TCB structure from the dead list
        // back to the active list.
        RemoveEntryList(&tcb->links);
        InsertHeadList(&PcbList, &tcb->links);
    } else {
        status = PcbGetErrStatus(tcb);
        if (!NT_SUCCESS(status)) goto exit;
    }

    if (!IsTcpIdleState(tcb) && !synAck) {
        status = NETERR(WSAEALREADY);
        goto exit;
    }

    // Find a route to the connected destination.
    ASSERT(tcb->rte == NULL);
    rte = tcb->rte = IpFindRTE(dstaddr, NULL);
    if (!rte) {
        status = NETERR_UNREACHABLE;
        goto exit;
    }

    RteAddRef(rte);
    if (tcb->bindSrcAddr == 0) {
        tcb->srcaddr = PcbGetDefaultSrcAddr(rte);
    }

    // If this is an active open, go to SYN-SENT state.
    // Otherwise, we should already be in SYN-RECEIVED state.
    if (!synAck) {
        TcbSetState(tcb, TCPST_SYN_SENT, "Connect");
    }

    // Set up MSS to make sure we don't have to
    // fragment on the first hop. We assume the first hop
    // interface won't change during the life of the connection.
    tcb->rcv_mss = rte->ifp->mtu - IPHDRLEN - TCPHDRLEN;
    tcb->maxRecvBufsize = 
        ((tcb->maxRecvBufsize + tcb->rcv_mss - 1) / tcb->rcv_mss) * tcb->rcv_mss;
    if (tcb->maxRecvBufsize > cfgMaxSendRecvBufsize)
        tcb->maxRecvBufsize = cfgMaxSendRecvBufsize;

    tcb->dstaddr = dstaddr;
    tcb->dstport = dstport;

    tcb->snd_isn = TcbGetIsn();
    tcb->rcv_wnd = tcb->maxRecvBufsize;
    ASSERT(IsTcpRecvBufEmpty(tcb));

    // send out a SYN or SYN/ACK segment
    status = TcbEmitSYN(tcb, synAck);

exit:
    LowerFromDpc(irql);
    return status;
}


#if DBG

extern ULONG XDebugOutLevel;

VOID
TcbSetState(
    TCB* tcb,
    BYTE state,
    const CHAR* caller
    )

/*++

Routine Description:

    Change the TCP connection state

Arguments:

    tcb - Points to the TCP control block
    state - Specifies the new connectio state
    caller - Name of the calling function

Return Value:

    NONE

--*/

{
    static const CHAR* stateNames[] = {
        "CLOSED",
        "LISTEN",
        "SYN_SENT",
        "SYN_RECEIVED",
        "ESTABLISHED",
        "FIN_WAIT_1",
        "FIN_WAIT_2",
        "CLOSING",
        "TIME_WAIT",
        "CLOSE_WAIT",
        "LAST_ACK"
    };

    BYTE oldstate = tcb->tcpstate;
    tcb->tcpstate = state;

    // Debug spew...

    if (XDebugOutLevel < XDBG_TRACE) return;

    DbgPrint("%s ", caller);
    DbgPrint("%s:%d - ", IPADDRSTR(tcb->srcaddr), NTOHS(tcb->srcport));
    DbgPrint("%s:%d: ", IPADDRSTR(tcb->dstaddr), NTOHS(tcb->dstport));
    DbgPrint("%s => %s\n", stateNames[oldstate], stateNames[state]);
}

#endif // DBG

