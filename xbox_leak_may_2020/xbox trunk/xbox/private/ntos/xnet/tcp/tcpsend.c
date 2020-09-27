/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tcpsend.c

Abstract:

    TCP data transmission related functions

Revision History:

    06/06/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Maximum number of transmission retries
//
UINT cfgMaxXmitRetries = 5;

//
// Maximum and minimum retransmission timeout (in ticks)
// NOTE: Non-standard behavior!!!
//  Our maximum timeout between retries is 8sec, much shorter
//  than the usual value of 60sec. This is ok for our first
//  release since we only support LAN play.
//
UINT cfgMinRexmitTimeout = SLOW_TCP_TIMER_FREQ;
UINT cfgMaxRexmitTimeout = 8 * SLOW_TCP_TIMER_FREQ;

//
// Minimum zero-window-probe timeout (in ticks)
//
UINT cfgMinPersistTimeout = 5*SLOW_TCP_TIMER_FREQ;

//
// TCP statistics
//
TCPSTATS TcpStats;


VOID
TcbSendSegment(
    TCB* tcb,
    TCPSENDBUF* sendbuf
    )

/*++

Routine Description:

    Send a TCP segment out

Arguments:

    tcb - Points to the TCP control block
    sendbuf - Points to the segment to be sent

Return Value:

    NONE

--*/

{
    Packet* pkt = GetSendbufPkt(sendbuf);

    if (!IsSendbufActive(sendbuf)) {
        tcb->delayedAcks = 0;
        SendbufIncRef(sendbuf);

        ASSERT(SEQ_LE(sendbuf->seq, TcbSndNxt(tcb)) || (sendbuf->tcpflags & TCP_FIN));

        FILL_TCPHEADER(
            pkt,
            &tcb->addrpair,
            sendbuf->seq,
            tcb->rcv_nxt,
            sendbuf->tcphdrlen,
            sendbuf->tcpflags,
            (WORD) tcb->rcv_wnd);

        if (sendbuf->retries)
            pkt->pktflags |= PKTFLAG_REXMIT;

        PcbSetupIpHeader((PCB*) tcb, pkt, &tcb->addrpair);
        IpSendPacket(pkt, tcb->dstaddr, NULL, &tcb->rte);
    }

    if (sendbuf == TcbFirstSendbuf(tcb)) {
        // Set up the retransmisssion or persist timer

        UINT minTimeout = tcb->persistFlag ?
                            cfgMinPersistTimeout :
                            cfgMinRexmitTimeout;

        tcb->xmitTimer = (tcb->RTO << sendbuf->retries);
        if (tcb->xmitTimer < minTimeout)
            tcb->xmitTimer = minTimeout;
        else if (tcb->xmitTimer > cfgMaxRexmitTimeout)
            tcb->xmitTimer = cfgMaxRexmitTimeout;
    }

    sendbuf->retries++;
}


BOOL
TcbStartOutput(
    TCB* tcb
    )

/*++

Routine Description:

    Called when we have buffered data to send on a TCP socket. 

Arguments:

    tcb - Points to the TCP control block
    sendbuf - Specifies the segment to be sent

Return Value:

    TRUE if a segment was sent; FALSE otherwise.

--*/

{
    INT sndwndAvail;
    TCPSENDBUF* sendbuf;
    TCPSEQ cwndmax, swndmax;

    RUNS_AT_DISPATCH_LEVEL

    sendbuf = tcb->sendbufNext;
    ASSERT(tcb->snd_nxt == sendbuf->seq);
    ASSERT(sendbuf->retries == 0);

    // We always send SYN or FIN segments without delay
    if (sendbuf->datalen == 0) goto sendnow;

    // NOTE: We're using signed integer here in case
    // the send window was shrunk by the receiver.
    swndmax = tcb->snd_wl2 + tcb->snd_wnd;
    cwndmax = tcb->snd_una + tcb->snd_cwnd;
    sndwndAvail = (SEQ_LE(swndmax, cwndmax) ? swndmax : cwndmax) - tcb->snd_nxt;

    if ((INT) sendbuf->datalen <= sndwndAvail) {
        // Send window is large enough. Perform send-side
        // SWS avoidance. Note that if the PSH flag is not set,
        // then we must be sending a full-sized data segment.
        // Check the logic in TcbSend().

        // BUGBUG: Since we don't yet combine data from
        // multiple send calls into a single segment, it's
        // meaningless to delay the send.
        goto sendnow;
    }

    // Cannot send due to window constraints
    //  set up persist timers
    if (tcb->snd_nxt == tcb->snd_una) {
        TRACE_("Setting persist timer...");
        TcpStats.persists++;
        tcb->persistFlag = 1;
        tcb->xmitTimer = max(tcb->RTO, cfgMinPersistTimeout);
    }
    return FALSE;

sendnow:

    // Use this segment to measure round-trip time if possible
    tcb->persistFlag = 0;
    if (tcb->rtt_tick == 0) {
        tcb->rtt_tick = TcpTickCount;
        tcb->rtt_seq = sendbuf->seq;
    }
    
    sendbuf->firstSendTime = TcpTickCount;
    TcbSendSegment(tcb, sendbuf);
    tcb->snd_nxt = sendbuf->seqnext;
    tcb->sendbufNext = TcbSendbufFlink(sendbuf);
    return TRUE;
}


TCB*
TcbXmitTimeout(
    TCB* tcb
    )

/*++

Routine Description:

    This function is called when the TCP retransmission timer expires.

Arguments:

    tcb - Points to the TCP control block

Return Value:

    NULL if the specified TCB was deleted inside this call.
    Otherwise, just return the input tcb parameter.

--*/

{
    TCPSENDBUF* sendbuf;

    if (IsPcbSendBufEmpty(tcb)) return tcb;
    sendbuf = TcbFirstSendbuf(tcb);

    if (sendbuf->retries >= cfgMaxXmitRetries) {
        TcbResetPeer(tcb);
        return TcbReset(tcb, NETERR_TIMEOUT);
    }

    // Stop round-trip time measurement if there is retransmission
    tcb->rtt_tick = 0;

    // Retransmit the first segment
    if (tcb->persistFlag) {
        if (sendbuf->retries == 0) {
            // Send out the window probe segment for the first time.
            // BUGBUG: we always send out the whole segment instead
            // of just a single byte of data.

            ASSERT(sendbuf == tcb->sendbufNext);
            TcbSendSegment(tcb, sendbuf);
            tcb->snd_nxt = sendbuf->seqnext;
            tcb->sendbufNext = TcbSendbufFlink(sendbuf);
            return tcb;
        }
    } else {
        TRACE_("Retransmission timeout: %u, %d", sendbuf->seq, sendbuf->retries);
        ASSERT(sendbuf->retries != 0);
        TcpStats.rexmitTimeouts++;

        // Congestion control:
        //  set the slow-start threshold to be half the flight size
        //  and set the congestion window to be 1 SMSS
        TcbResetSsthresh(tcb);
        tcb->snd_cwnd = tcb->snd_mss;
        TcbStopFastRexmitMode(tcb);
    }

    TcbSendSegment(tcb, sendbuf);
    return tcb;
}


VOID
TcpSourceQuench(
    const IpHeader* iphdr
    )

/*++

Routine Description:

    Process ICMP source quench error message
    for a TCP connection

Arguments:

    iphdr - Points to the original IP header + at least 8 bytes of data

Return Value:

    NONE

--*/

{
    const TcpHeader* tcphdr;
    TCB* tcb;

    tcphdr = (const TcpHeader*) ((const BYTE*) iphdr + GETIPHDRLEN(iphdr));
    tcb = (TCB*) PcbFindMatch(
                    iphdr->srcaddr,
                    tcphdr->srcport,
                    iphdr->dstaddr,
                    tcphdr->dstport,
                    SOCK_STREAM,
                    IPPROTOCOL_TCP);

    if (tcb && IsTcpSyncState(tcb)) {
        // Treat source quench the same way as a retransmission timeout:
        //  reset the slow-start threshold
        //  and set the cwnd to 1 SMSS
        TcbResetSsthresh(tcb);
        tcb->snd_cwnd = tcb->snd_mss;
        TcbStopFastRexmitMode(tcb);
    }
}


VOID
TcbDoFastRexmit(
    TCB* tcb
    )

/*++

Routine Description:

    Initiate the TCP fast retransmit / recovery procedure

Arguments:

    tcb - Points to the TCP control block

Return Value:

    NONE

--*/

{
    TCPSENDBUF* sendbuf;

    TRACE_("Fast rexmit mode: %u", tcb->snd_una);
    TcpStats.fastRexmits++;

    // Reset the slow-start threshold
    // and set the congestion window to be that plus 3*SMSS
    TcbResetSsthresh(tcb);
    tcb->snd_cwnd = tcb->snd_ssthresh + 3*tcb->snd_mss;
    tcb->fastRexmitFlag = 1;
    tcb->dupacks = 0;

    // Retransmit the "lost" segment immediately
    sendbuf = TcbFirstSendbuf(tcb);
    ASSERT(sendbuf != TcbSendbufNil(tcb) && sendbuf->retries);
    TcbSendSegment(tcb, sendbuf);
}


VOID
TcbQueueSegment(
    TCB* tcb,
    Packet* pkt,
    BYTE tcpflags
    )

/*++

Routine Description:

    Initialize the header information for an outgoing TCP packet

Arguments:

    tcb - Points to the TCP control block
    pkt - Points to the outgoing TCP packet
    tcpflags - Specifies the TCP segment flags

Return Value:

    NONE

--*/

// TCP option length in our SYN segment
#define SYNTCPOPTLEN 4

{
    TCPSENDBUF* sendbuf;

    RUNS_AT_DISPATCH_LEVEL

    sendbuf = GETPKTBUF(pkt, TCPSENDBUF);
    sendbuf->refcount = 1;
    sendbuf->retries = 0;
    sendbuf->tcpflags = tcpflags;
    sendbuf->tcphdrlen = (BYTE) ((tcpflags & TCP_SYN) ?
                                    TCPHDRLEN+SYNTCPOPTLEN :
                                    TCPHDRLEN);
    sendbuf->seq = tcb->snd_end;

    if (tcpflags & (TCP_SYN|TCP_FIN)) {
        sendbuf->datalen = 0;
        sendbuf->seqnext = sendbuf->seq + 1;
    } else {
        sendbuf->datalen = pkt->datalen - TCPHDRLEN;
        sendbuf->seqnext = sendbuf->seq + sendbuf->datalen;
    }
    tcb->snd_end = sendbuf->seqnext;

    tcb->sendbufSize += sendbuf->datalen;
    InsertTailList(&tcb->sendbuf, &sendbuf->links);

    if (!TcbHasPendingSend(tcb)) {
        tcb->sendbufNext = sendbuf;
        TcbStartOutput(tcb);
    }
}


//
// TCP segment packet completion routine
//
PRIVATE VOID TcbSendCompletionProc(Packet* pkt, NTSTATUS status) {
    TCPSENDBUF* sendbuf = GETPKTBUF(pkt, TCPSENDBUF);
    pkt->data = sendbuf->pktdata;
    pkt->datalen = sendbuf->pktdatalen;
    SendbufRelease(sendbuf);
}

//
// Allocate packet buffer for sending a TCP segment
// (that could be retransmitted)
//
PRIVATE Packet*
TcbAllocSendbuf(
    UINT datalen,
    UINT pktflags
    )
{
    UINT hdrlen;
    Packet* pkt;
    TCPSENDBUF* sendbuf;

    hdrlen = sizeof(TCPSENDBUF) + MAXLINKHDRLEN + MAXIPHDRLEN;
    datalen += TCPHDRLEN;
    pkt = XnetAllocPacket(hdrlen + datalen, pktflags);
    if (!pkt) return NULL;

    pkt->data += hdrlen;
    pkt->datalen = datalen;
    sendbuf = GETPKTBUF(pkt, TCPSENDBUF);
    sendbuf->pktdata = pkt->data;
    sendbuf->pktdatalen = pkt->datalen;

    XnetSetPacketCompletion(pkt, TcbSendCompletionProc);
    return pkt;
}


NTSTATUS
TcbSend(
    TCB* tcb,
    SENDREQ* sendreq
    )

/*++

Routine Description:

    Handle a user request to send data from a TCP socket

Arguments:

    tcb - Points to the TCP control block
    sendreq - Points to the send request information

Return Value:

    Status code

Note:

    This function always return immediately. For blocking operation,
    we assume the caller has taken care of any necessary wait.

--*/

{
    NTSTATUS status = NETERR_OK;
    Packet* pkt = NULL;
    WSABUF* bufs = sendreq->bufs;
    UINT bytesToSend = sendreq->sendtotal;
    UINT mss = tcb->snd_mss - tcb->ipoptlen;
    UINT n, datalen;
    BYTE* data;
    KIRQL irql;

    // Make sure we don't overflow the send buffer too much.
    // In the following case:
    //  old sendbuf size + the datagram size > max sendbuf size
    // we'll temporarily exceed the send buffer size limit a little bit.
    ASSERT(!IsPcbSendBufFull(tcb));
    n = tcb->maxSendBufsize - tcb->sendbufSize;
    if (bytesToSend > max(n, mss))
        sendreq->sendtotal = bytesToSend = mss;

    data = bufs->buf;
    datalen = bufs->len;

    while (bytesToSend > 0) {
        UINT segsize = min(mss, bytesToSend);
        BYTE* p;

        pkt = TcbAllocSendbuf(segsize, 0);
        if (!pkt) {
            if ((sendreq->sendtotal -= bytesToSend) == 0) {
                // If we've already send out some data,
                // then return success with partial byte count.
                // Otherwise, return error code.
                status = NETERR_MEMORY;
            }
            break;
        }

        bytesToSend -= segsize;
        p = pkt->data + TCPHDRLEN;
        if (segsize <= datalen) {
            // Fast case: all the data for this segment
            // comes from the same user buffer.
            CopyMem(p, data, segsize);
            data += segsize;
            datalen -= segsize;
        } else {
            // Slow case: need to gather data for this segment
            // from multiple user buffers.
            while (segsize) {
                while (datalen == 0) {
                    bufs++;
                    data = bufs->buf;
                    datalen = bufs->len;
                }
                n = min(segsize, datalen);
                CopyMem(p, data, n);
                p += n;
                data += n;
                datalen -= n;
                segsize -= n;
            }
        }

        irql = RaiseToDpc();
        status = PcbGetErrStatus(tcb);
        if (NT_SUCCESS(status)) {
            TcbQueueSegment(tcb, pkt, (BYTE) (bytesToSend ? TCP_ACK : (TCP_ACK|TCP_PSH)));
            LowerFromDpc(irql);
        } else {
            XnetFreePacket(pkt);
            LowerFromDpc(irql);
            break;
        }
    }

    // If the app calls connect() on a non-blocking socket, and then calls send()
    // without calling select(), then we need to reset the connectSelected flag
    // so future select() calls will ignore PCBEVENT_CONNECT.
    tcb->connectSelected = 1;

    return status;
}


VOID
TcbEmitRST(
    IfInfo* ifp,
    IpAddrPair* addrpair,
    TCPSEQ seq,
    TCPSEQ ack,
    BYTE flags
    )

/*++

Routine Description:

    Send out a TCP RST segment in response to a received packet

Arguments:

    ifp - Points the interface involved
    addrpair - Specifies the source and destination
    seq, ack - Specifies the sequence and acknowledgement number
    flag - Specifies the TCP segment flags

Return Value:

    NONE

--*/

{
    Packet* pkt;

    pkt = XnetAllocIpPacket(0, TCPHDRLEN);
    if (!pkt) return;

    FILL_TCPHEADER(
        pkt,
        addrpair,
        seq,
        ack,
        TCPHDRLEN,
        (BYTE) (TCP_RST|flags),
        0);

    IpSendPacketInternal(pkt, addrpair->srcaddr, addrpair->dstaddr, IPPROTOCOL_TCP, ifp);
}


NTSTATUS
TcbEmitSYN(
    TCB* tcb,
    BOOL synAck
    )

/*++

Routine Description:

    Emit a TCP SYN segment

Arguments:

    tcb - Points to the TCP control block
    synAck - Whether to send ACK as well

Return Value:

    Status code

--*/

{
    Packet* pkt;
    BYTE* opt;
    TCPSENDBUF* sendbuf;

    // Check if we're already trying to send out a SYN segment
    if (!IsPcbSendBufEmpty(tcb)) {
        sendbuf = (TCPSENDBUF*) RemoveHeadList(&tcb->sendbuf);
        SendbufRelease(sendbuf);
        ASSERT(IsPcbSendBufEmpty(tcb));
        ASSERT(!TcbHasPendingSend(tcb));
    }

    tcb->snd_una = tcb->snd_nxt = tcb->snd_end = tcb->snd_isn;

    // Allocate a TCP send buffer for possible retransmission
    pkt = TcbAllocSendbuf(SYNTCPOPTLEN, synAck ? PKTFLAG_NETPOOL : 0);
    if (!pkt) {
        // This is very bad - we failed to allocate memory
        // for the SYN segment. We'll mark the TCB as dead
        // and return an error code.
        TcbReset(tcb, NETERR_MEMORY);
        return NETERR_MEMORY;
    }

    TcbSetSynTimer(tcb, cfgConnectTimeout);

    opt = pkt->data + TCPHDRLEN;
    opt[0] = TCPOPT_MAX_SEGSIZE;
    opt[1] = SYNTCPOPTLEN;
    opt[2] = (BYTE) (tcb->rcv_mss >> 8);
    opt[3] = (BYTE) tcb->rcv_mss;

    TcbQueueSegment(tcb, pkt, (BYTE) (synAck ? TCP_SYN|TCP_ACK : TCP_SYN));
    return NETERR_OK;
}


VOID
TcbEmitACK(
    TCB* tcb
    )

/*++

Routine Description:

    Send out an ACK segment on a TCP connection

Arguments:

    tcb - Points to the TCP control block

Return Value:

    NONE

--*/

{
    TCPSEQ seq;
    Packet* pkt = XnetAllocPacket(MAXLINKHDRLEN + MAXIPHDRLEN + TCPHDRLEN, PKTFLAG_NETPOOL);
    if (!pkt) return;

    pkt->data += MAXLINKHDRLEN + MAXIPHDRLEN;
    pkt->datalen = TCPHDRLEN;

    seq = TcbSndNxt(tcb);
    FILL_TCPHEADER(
        pkt,
        &tcb->addrpair,
        seq,
        tcb->rcv_nxt,
        TCPHDRLEN,
        TCP_ACK,
        (WORD) tcb->rcv_wnd);

    tcb->delayedAcks = 0;
    PcbSetupIpHeader((PCB*) tcb, pkt, &tcb->addrpair);
    IpSendPacket(pkt, tcb->dstaddr, NULL, &tcb->rte);
}


NTSTATUS
TcbEmitFIN(
    TCB* tcb
    )

/*++

Routine Description:

    Send out a FIN segment on a TCP connection

Arguments:

    tcb - Points to the TCP control block

Return Value:

    Status code

--*/

{
    Packet* pkt = TcbAllocSendbuf(0, 0);
    if (!pkt) return NETERR_MEMORY;
    TcbQueueSegment(tcb, pkt, TCP_ACK|TCP_FIN);
    return NETERR_OK;
}

