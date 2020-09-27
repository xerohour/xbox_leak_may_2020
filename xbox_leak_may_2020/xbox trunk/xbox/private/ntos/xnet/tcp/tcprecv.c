/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tcprecv.c

Abstract:

    TCP input processing functions

Revision History:

    05/31/2000 davidx
        Created it.

--*/

#include "precomp.h"


//
// Update the receive window information after accepting
// the specified amount data from the connection peer.
//
INLINE VOID
TcbSlideRecvWindow(
    TCB* tcb,
    UINT datalen
    )
{
    UINT newwnd = tcb->maxRecvBufsize - tcb->recvbufSize;

    // Here datalen must be <= rcv_wnd.
    ASSERT(datalen <= tcb->rcv_wnd);
    tcb->rcv_nxt += datalen;

    if ((datalen + newwnd) - tcb->rcv_wnd >= tcb->rcv_swsthresh) {
        // Enough space has freed up. Move the right edge of the receive window.
        tcb->rcv_wnd = newwnd;
    } else {
        // Implement receive side silly window avoidance:
        //  keep the right edge of the receive window unchanged.
        tcb->rcv_wnd -= datalen;
    }
}


//
// Copy data from the TCB receive buffer to the user's receive buffer
//
PRIVATE UINT
TcbCopyRecvData(
    TCB* tcb,
    RECVREQ* recvreq
    )
{
    RECVBUF* recvbuf;
    UINT copycnt, total = 0;
    BYTE tcpflags = 0;

    do {
        recvbuf = (RECVBUF*) tcb->recvbuf.Flink;
        ASSERT(SEQ_LE(recvbuf->seqnext, tcb->rcv_nxt));

        copycnt = min(recvbuf->datalen, recvreq->buflen);
        CopyMem(recvreq->buf, (BYTE*) recvbuf + recvbuf->dataoffset, copycnt);
        total += copycnt;
        tcb->recvbufSize -= copycnt;
        recvreq->buf += copycnt;
        recvreq->buflen -= copycnt;

        tcpflags |= recvbuf->tcpflags;
        if (copycnt == recvbuf->datalen) {
            RemoveEntryList(&recvbuf->links);
            XnetFree(recvbuf);
        } else {
            recvbuf->dataoffset = (WORD) (recvbuf->dataoffset + copycnt);
            recvbuf->datalen -= copycnt;
        }
    } while (recvreq->buflen && !IsTcpRecvBufEmpty(tcb));

    if (tcpflags & (TCP_PSH|TCP_URG))
        recvreq->buflen = 0;

    return total;
}


NTSTATUS
TcbRecv(
    TCB* tcb,
    RECVREQ* recvreq
    )

/*++

Routine Description:

    Handle a user request to receive data from a TCP socket

Arguments:

    tcb - Points to the TCP control block
    recvreq - Points to the receive request information

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    UINT oldrcvwnd;
    KIRQL irql = RaiseToDpc();

    // Only segments from the connection peer are accepted
    SetRecvReqFromAddr(recvreq, tcb->dstaddr, tcb->dstport);
    *recvreq->bytesRecv = 0;

    if (!IsTcpRecvBufEmpty(tcb)) {

        // If the receive buffer is not empty, return as much data
        // as possible from the receive buffer.

        (*recvreq->bytesRecv) += TcbCopyRecvData(tcb, recvreq);
        status = NETERR_OK;

        // If the receive window was closed and there is
        // now enough free spce in the receive buffer,
        // then we'll send out a gratuitous ACK here.

        oldrcvwnd = tcb->rcv_wnd;
        TcbSlideRecvWindow(tcb, 0);
        if (oldrcvwnd < tcb->rcv_swsthresh &&
            tcb->rcv_wnd >= tcb->rcv_swsthresh) {
            TcbEmitACK(tcb);
        }
    } else if (IsFINReceived(tcb)) {
        // The connection has been gracefully closed
        status = NETERR_OK;
    } else {
        status = PcbGetErrStatus(tcb);
        if (NT_SUCCESS(status)) {
            status = PcbQueueOverlappedRecv((PCB*) tcb, recvreq);
        }
    }

    LowerFromDpc(irql);
    return status;
}


PRIVATE VOID
TcbUpdateRTO(
    TCB* tcb
    )

/*++

Routine Description:

    Update the round-trip time measurements for a TCP connection

Arguments:

    tcb - Points to the TCP control block

Return Value:

    NONE

--*/

{
    INT nticks, delta;

    // The computation is based on the following formula:
    //  delta = nticks - srtt
    //  srtt = srtt + delta / 8
    //  rttvar = rttvar + (|delta| - rttvar) / 4
    //  RTO = srtt + 4 * rttvar

    nticks = TcpTickCount - tcb->rtt_tick;
    tcb->rtt_tick = 0;

    if (tcb->srtt_8 != 0) {
        delta = nticks - (tcb->srtt_8 >> SRTT_SHIFT);
        if ((tcb->srtt_8 += delta) <= 0)
            tcb->srtt_8 = 1;

        if (delta < 0) delta = -delta;
        delta -= (tcb->rttvar_4 >> RTTVAR_SHIFT);
        if ((tcb->rttvar_4 += delta) <= 0)
            tcb->rttvar_4 = 1;
    } else {
        // The very first measurement - use the unsmoothed data
        if (nticks == 0) {
            tcb->srtt_8 = tcb->rttvar_4 = 1;
        } else {
            tcb->srtt_8 = nticks << SRTT_SHIFT;
            tcb->rttvar_4 = nticks << (RTTVAR_SHIFT-1);
        }
    }

    tcb->RTO = (tcb->srtt_8 >> SRTT_SHIFT) + tcb->rttvar_4;
    if (tcb->RTO < cfgMinRexmitTimeout)
        tcb->RTO = cfgMinRexmitTimeout;
}


PRIVATE BOOL
TcbUpdatePersistFlag(
    TCB* tcb
    )

/*++

Routine Description:

    This function is called after the send window information is updated.
    If we're currently sending out window probing segments, then
    we'll stop doing so if the send window has opened up.

Arguments:

    tcb - Points to the TCP control block

Return Value:

    FALSE if we got out of persist mode
    TRUE otherwise

--*/

{
    TCPSENDBUF* sendbuf = TcbFirstSendbuf(tcb);
    ASSERT(!IsPcbSendBufEmpty(tcb));

    if (sendbuf->datalen <= tcb->snd_wnd) {
        tcb->persistFlag = 0;
        if (sendbuf->retries) {
            TRACE_("Retransmit persisting segment...");
            sendbuf->retries = 0;
            TcbSendSegment(tcb, sendbuf);
        } else {
            tcb->xmitTimer = 0;
        }
        return FALSE;
    }
    return TRUE;
}


PRIVATE VOID
TcbUpdateSndUna(
    TCB* tcb,
    TCPSEQ ack
    )

/*++

Routine Description:

    Update the snd.una variable for a TCP connection

Arguments:

    tcb - Points to the TCP connection block
    ack - The acknowledged sequence number

Return Value:

    NONE

--*/

{
    TCPSENDBUF* sendbuf;
    UINT ackedSends = 0;

    if (SEQ_GT(ack, tcb->snd_una)) {
        tcb->snd_una = ack;

        // Update round-trip time measurements
        if (tcb->rtt_tick && SEQ_GT(ack, tcb->rtt_seq)) {
            TcbUpdateRTO(tcb);
        }

        // Complete fully acknowledged send user requests

        LOOP_THRU_TCB_SENDBUF(tcb, sendbuf)

            if (sendbuf->retries && SEQ_GE(ack, sendbuf->seqnext)) {
                RemoveEntryList(&sendbuf->links);
                tcb->sendbufSize -= sendbuf->datalen;
                ackedSends++;
                SendbufRelease(sendbuf);
            } else {
                break;
            }

        END_TCB_SENDBUF_LOOP
    }

    // Update congestion window
    if (tcb->fastRexmitFlag) {
        if (ackedSends) {
            //
            // Getting out of fast retransmit / fast recovery mode:
            //  "deflate" the congestion window
            //
            tcb->snd_cwnd = tcb->snd_ssthresh;
        } else {
            //
            // In fast recovery mode:
            //  increment the congestion window by SMSS
            //  for every duplicate ACK received.
            //
            TcbIncrementCwnd(tcb, tcb->snd_mss);
        }
    } else {
        if (tcb->snd_cwnd < tcb->snd_ssthresh) {
            //
            // Slow-start mode:
            //  increment the congestion window by SMSS
            //
            TcbIncrementCwnd(tcb, tcb->snd_mss);
        } else {
            //
            // Congestion avoidance mode:
            //  increment the congestion window by ~SMSS per RTT
            //
            // NOTE: Overflow is not possible here because snd_mss
            // and snd_cwnd are 16-bit numbers. Also snd_cwnd is never 0.
            //
            UINT inc = tcb->snd_mss * tcb->snd_mss / tcb->snd_cwnd;
            TcbIncrementCwnd(tcb, max(1, inc));
        }
    }

    if (ackedSends) {
        if (!IsPcbSendBufFull(tcb)) {
            if (HasOverlappedSend(tcb)) {
                SENDREQ* sendreq = PcbGetOverlappedSend((PCB*) tcb);
                NTSTATUS status = TcbSend(tcb, sendreq);

                sendreq->overlapped->_ioxfercnt = sendreq->sendtotal;
                PcbCompleteOverlappedSend(sendreq, status);
            } else {
                PcbSignalEvent(tcb, PCBEVENT_WRITE);
            }
        }
        tcb->persistFlag = 0;
        TcbStopFastRexmitMode(tcb);

        // Reset retransmission timer
        if (tcb->snd_una == tcb->snd_nxt)
            tcb->xmitTimer = 0;
        else {
            sendbuf = TcbFirstSendbuf(tcb);
            if (sendbuf->firstSendTime + tcb->RTO <= TcpTickCount) {
                TcbXmitTimeout(tcb);
            } else {
                tcb->xmitTimer = sendbuf->firstSendTime + tcb->RTO - TcpTickCount;
            }
        }

    } else if (tcb->persistFlag) {

        // If we're probing the send window and the ack was for
        // the probe segment, then make sure we continue to probe
        // without timing out.

        sendbuf = TcbFirstSendbuf(tcb);
        if (SEQ_GE(ack, sendbuf->seq) &&
            sendbuf->retries >= cfgMaxXmitRetries-1) {
            sendbuf->retries--;
        }

    } else {
        if (tcb->snd_una != tcb->snd_nxt &&
            !tcb->fastRexmitFlag &&
            ++tcb->dupacks >= 4) {
            //
            // Too many duplicate ACK received:
            //  do fast retransmit / recovery
            //
            TcbDoFastRexmit(tcb);
        }
    }

    // If the send window has opened up and
    // we have pending data to sent, try to do it now.
    if (!tcb->persistFlag || !TcbUpdatePersistFlag(tcb)) {
        while (TcbHasPendingSend(tcb) && TcbStartOutput(tcb))
            NULL;
    }
}


// 
// Check if an acknowledged sequence number is valid for a TCP connection
//
#define IsValidACK(_tcb, _ack) \
        (SEQ_GT((_ack), (_tcb)->snd_una) && \
         SEQ_LE((_ack), (_tcb)->snd_nxt))

//
// Indicate whether we should send out an ACK immediately
// in response to an incoming segment.
//
#define NeedSendACKNow(_tcb) ((_tcb)->delayedAcks += 0x20000)

//
// Save the send window information from the connection peer
//
#define TcbUpdateSndWnd(_tcb, _wnd, _seq, _ack) { \
            (_tcb)->snd_wnd = (_wnd); \
            (_tcb)->snd_wl1 = (_seq); \
            (_tcb)->snd_wl2 = (_ack); \
        }


PRIVATE BOOL
TcbValidateSeqs(
    TCB* tcb,
    TCPSEQ oldseq0,
    TCPSEQ oldseq1,
    TCPSEQ* newseq0,
    TCPSEQ* newseq1
    )

/*++

Routine Description:

    Check if the received sequence number is valid for a TCP connection

Arguments:

    tcb - Points to TCP control block
    oldseq0 - Starting sequence number from the received segment
    oldseq1 - Ending sequence number from the received segment (oldseq+seglen)
    newseq0 - Returns the effective starting sequence number inside the receive window
    newseq1 - Returns the effective ending sequence number

Return Value:

    FALSE if none of the received segment is inside the receive window,
    TRUE otherwise. In the latter case, seq0 and seq1 will return updated
    sequence numbers.

--*/

{
    TCPSEQ rcv_last;

    rcv_last = tcb->rcv_nxt + tcb->rcv_wnd;
    *newseq0 = SEQ_LT(oldseq0, tcb->rcv_nxt) ? tcb->rcv_nxt : oldseq0;
    *newseq1 = SEQ_GT(oldseq1, rcv_last) ? rcv_last : oldseq1;

    return SEQ_LT(*newseq0, *newseq1) ||
           (*newseq0 == *newseq1 && oldseq0 == oldseq1);
}


#if DBG

PRIVATE BOOL
TcbVerifyRecvBuf(
    TCB* tcb
    )

/*++

Routine Description:

    Verify the current receive buffers for a TCB is good

Arguments:

    tcb - Points to the TCP control block

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    RECVBUF* buf0 = (RECVBUF*) tcb->recvbuf.Flink;
    RECVBUF* buf1;

    if (buf0 == TcbRecvbufNil(tcb)) return TRUE;
    while (TRUE) {
        buf1 = TcbRecvbufFlink(buf0);
        if (buf1 == TcbRecvbufNil(tcb)) return TRUE;
        if (SEQ_GT(buf0->seqnext, buf1->seqnext-buf1->datalen)) return FALSE;
        buf0 = buf1;
    }
}

#endif // DBG


PRIVATE UINT
TcbCorrectMisorderedSegments(
    TCB* tcb,
    TCPSEQ seq,
    RECVBUF* recvbuf
    )

/*++

Routine Description:

    This function is called when we receive a segment
    whose starting sequence number matches rcv_nxt and
    there are buffered out-of-order segments after this one.

Arguments:

    tcb - Points to TCP control block
    seq - Specifies the starting sequence number for this segment
    recvbuf - Points to the received data buffer

Return Value:

    Number of continuous bytes that can be acknowledged

--*/

{
    RECVBUF* buf0 = TcbLastRecvbuf(tcb);
    RECVBUF* buf1;
    UINT count;

    do {
        buf0 = TcbRecvbufBlink(buf0);
    } while (buf0 != TcbRecvbufNil(tcb) && buf0->seqnext != seq);

    buf1 = TcbRecvbufFlink(buf0);
    if (SEQ_GT(recvbuf->seqnext, buf1->seqnext - buf1->datalen)) {
        // Weird case: data in this segment overlapps with
        // data in the buffered out-of-order segments.
        // Chop off the overlapped data in the current receive buffer.
        TRACE_("TcbCorrectMisorderedSegments: overlapping segment");

        count = recvbuf->seqnext - (buf1->seqnext - buf1->datalen);
        ASSERT(count < recvbuf->datalen);

        recvbuf->datalen -= count;
        recvbuf->seqnext -= count;
    }

    // Insert recvbuf after buf0 and before buf1
    buf0->links.Flink = buf1->links.Blink = (LIST_ENTRY*) recvbuf;
    recvbuf->links.Flink = (LIST_ENTRY*) buf1;
    recvbuf->links.Blink = (LIST_ENTRY*) buf0;

    count = recvbuf->datalen;
    buf0 = recvbuf;
    while (buf1 != TcbRecvbufNil(tcb) &&
           buf0->seqnext == buf1->seqnext - buf1->datalen) {
        count += buf1->datalen;
        buf0 = buf1;
        buf1 = TcbRecvbufFlink(buf1);
    }

    // If we filled up a hole, emit an ACK immediately
    if (count > recvbuf->datalen) {
        NeedSendACKNow(tcb);
    }

    // ASSERT(TcbVerifyRecvBuf(tcb));
    return count;
}


PRIVATE VOID
TcbBufferMisorderedSegment(
    TCB* tcb,
    UINT seq,
    RECVBUF* recvbuf
    )


/*++

Routine Description:

    This function is called when we receive an out-of-order segment.

Arguments:

    tcb - Points to TCP control block
    seq - Specifies the starting sequence number for this segment
    recvbuf - Points to the received data buffer

Return Value:

    NONE

--*/

{
    RECVBUF* buf0 = TcbLastRecvbuf(tcb);
    RECVBUF* buf1;
    UINT count;

    if (buf0 == TcbRecvbufNil(tcb) || SEQ_GE(seq, buf0->seqnext)) {
        // The sequence number for this segment is larger
        // than everything in the receive buffer.
        InsertTailList(&tcb->recvbuf, &recvbuf->links);
        return;
    }

    do {
        buf0 = TcbRecvbufBlink(buf0);
    } while (buf0 != TcbRecvbufNil(tcb) && SEQ_LT(seq, buf0->seqnext));

    // recvbuf should go in between buf0 and buf1

    buf1 = TcbRecvbufFlink(buf0);
    if (SEQ_GT(recvbuf->seqnext, buf1->seqnext - buf1->datalen)) {
        // Data in this segment overlapps with
        // data in the buffered out-of-order segments.
        TRACE_("TcbBufferMisorderedSegment: overlapping segment");
        
        count= recvbuf->seqnext - (buf1->seqnext - buf1->datalen);
        if (count >= recvbuf->datalen) {
            XnetFree(recvbuf);
            return;
        }

        recvbuf->datalen -= count;
        recvbuf->seqnext -= count;
    }

    buf0->links.Flink = buf1->links.Blink = (LIST_ENTRY*) recvbuf;
    recvbuf->links.Flink = (LIST_ENTRY*) buf1;
    recvbuf->links.Blink = (LIST_ENTRY*) buf0;

    // ASSERT(TcbVerifyRecvBuf(tcb));
}


PRIVATE UINT
TcbCompletePendingRecvReqFast(
    TCB* tcb,
    Packet* pkt,
    BYTE tcpflags
    )

/*++

Routine Description:

    This function is called when we received data from a TCP connection
    and there is a pending overlapped receive request
    and there is currently no data in the receive buffer.
    In this case we can satisfy the request right away without
    allocating memory to store data in the receive buffer.

Arguments:

    tcb - Points to the TCP control block
    pkt - Points to the received packet
    tcpflags - TCP segment flags

Return Value:

    Number of bytes used up in this call

--*/

{
    UINT copycnt;
    RECVREQ* recvreq = PcbGetOverlappedRecv(tcb);

    copycnt = min(pkt->datalen, recvreq->buflen);
    CopyMem(recvreq->buf, pkt->data, copycnt);
    recvreq->overlapped->_ioxfercnt += copycnt;
    recvreq->buf += copycnt;
    recvreq->buflen -= copycnt;

    if ((recvreq->buflen == 0) || (tcpflags & TCP_PSH|TCP_URG)) {
        PcbCompleteOverlappedRecv(recvreq, NETERR_OK);
    }

    TcbSlideRecvWindow(tcb, copycnt);
    return copycnt;
}


PRIVATE TCB*
TcbProcessDataAck(
    TCB* tcb,
    Packet* pkt,
    TcpHeader* tcphdr
    )

/*++

Routine Description:

    Process a received TCP data segment.
    And we assume the connection is in a synchronized state.

Arguments:

    tcb - Points to the TCP connection block
    pkt - Points to the received packet
    tcphdr - Points to the TCP segment header information

Return Value:

    NULL if the TCB was deleted as a result of the incoming segment;
    Otherwise, just return the input tcb parameter

--*/

{
    TCPSEQ seq, ack, seq0, seq1;
    RECVBUF* recvbuf;

    seq = SEG_SEQ(tcphdr);

    // Process ACK if it's present
    if (ISTCPSEG(tcphdr, ACK)) {
        ack = SEG_ACK(tcphdr);
        if (SEQ_LT(ack, tcb->snd_una)) {
            // ACK is an old duplicate, ignore it
        } else if (SEQ_GT(ack, tcb->snd_nxt)) {
            // ACKing something that hasn't been sent.
            // Emit an ACK, drop the segment
            NeedSendACKNow(tcb);
            return tcb;
        } else {
            // update the send window information
            if (SEQ_LT(tcb->snd_wl1, seq) ||
                tcb->snd_wl1 == seq && SEQ_LE(tcb->snd_wl2, ack)) {
                TcbUpdateSndWnd(tcb, SEG_WIN(tcphdr), seq, ack);
            }

            TcbUpdateSndUna(tcb, ack);

            // If we already sent FIN, check to see if FIN has been acknowledged
            if (IsFINSent(tcb) && IsPcbSendBufEmpty(tcb)) {
                switch (tcb->tcpstate) {
                case TCPST_FIN_WAIT_1:
                    TcbSetState(tcb, TCPST_FIN_WAIT_2, "FinAck");
                    break;

                case TCPST_CLOSING:
                    TcbSetState(tcb, TCPST_TIME_WAIT, "FinAck");
                    TcbSetTimeWaitTimer(tcb, 2*cfgMSL);
                    return NULL;

                case TCPST_LAST_ACK:
                    TcbDelete(tcb);
                    return NULL;
                }
            }
        }
    }

    // If we already receive FIN from the peer,
    // we'll just ignore incoming data.
    if (IsFINReceived(tcb) || pkt->datalen == 0) return tcb;

    tcb->delayedAcks += pkt->datalen;

    // Quick check to see if the data from incoming segment
    // fits entirely within the current receive window.
    if (seq != tcb->rcv_nxt || pkt->datalen > tcb->rcv_wnd) {
        // If the receive sequence number is outside of
        // the current receive window, send an ACK right away.
        if (!TcbValidateSeqs(tcb, seq, seq+pkt->datalen, &seq0, &seq1)) {
            TRACE_("Sequence number out-of-range: %u %u %d",
                   seq, tcb->rcv_nxt, tcb->rcv_wnd);
            NeedSendACKNow(tcb);
            return tcb;
        }

        pkt->data += (seq0 - seq);
        pkt->datalen = (seq1 - seq0);
        ASSERT((INT) pkt->datalen > 0);
        seq = seq0;
    }

    if (IsPcbRecvShutdown(tcb)) {
        TcbResetPeer(tcb);
        TcbReset(tcb, NETERR_CONNRESET);
        return NULL;
    }

    // Special fast path: there is a pending overlapped receive request
    // and there is no data in the receive buffer.
    if (HasOverlappedRecv(tcb) &&
        seq == tcb->rcv_nxt &&
        IsListEmpty(&tcb->recvbuf)) {
        UINT copied = TcbCompletePendingRecvReqFast(tcb, pkt, tcphdr->flags);
        if (copied == pkt->datalen) return tcb;

        pkt->data += copied;
        pkt->datalen -= copied;
        seq += copied;
    }

    // Make a copy of the incoming segment and buffer it up
    recvbuf = (RECVBUF*) XnetAlloc(sizeof(RECVBUF) + pkt->datalen, PTAG_RBUF);
    if (!recvbuf) {
        WARNING_("Received TCP data discarded because of no memory");
        WARNING_("  local = %d, remote = %s:%d",
            NTOHS(tcb->srcport),
            IPADDRSTR(tcb->dstaddr),
            NTOHS(tcb->dstport));
        return tcb;
    }

    recvbuf->seqnext = seq + pkt->datalen;
    recvbuf->dataoffset = sizeof(RECVBUF);
    recvbuf->tcpflags = tcphdr->flags;
    recvbuf->datalen = pkt->datalen;
    CopyMem(recvbuf+1, pkt->data, pkt->datalen);

    // Fast case: the receive segment is in sequence
    if (seq == tcb->rcv_nxt) {
        UINT datarun;
        RECVBUF* lastbuf = TcbLastRecvbuf(tcb);

        if (lastbuf == TcbRecvbufNil(tcb) || lastbuf->seqnext == seq) {
            // We haven't buffered any out-of-order segments behind this one.
            datarun = pkt->datalen;
            InsertTailList(&tcb->recvbuf, &recvbuf->links);
        } else {
            // Figure out if this segment fills out holes
            // in the receive buffer.
            ASSERT(SEQ_LT(seq, lastbuf->seqnext));
            datarun = TcbCorrectMisorderedSegments(tcb, seq, recvbuf);
        }
        tcb->recvbufSize += datarun;
        TcbSlideRecvWindow(tcb, datarun);

        if (HasOverlappedRecv(tcb)) {
            // If we have a pending overlapped receive request, satisfy it now
            RECVREQ* recvreq = PcbGetOverlappedRecv(tcb);

            recvreq->overlapped->_ioxfercnt += TcbCopyRecvData(tcb, recvreq);
            if (recvreq->buflen == 0) {
                PcbCompleteOverlappedRecv(recvreq, NETERR_OK);
            }

            TcbSlideRecvWindow(tcb, 0);
        } else {
            PcbSignalEvent(tcb, PCBEVENT_READ);
        }
    } else {
        // Slow case: a segment arrived out of order.
        // Buffer up this segment for later processing.
        TRACE_("Out-of-order segment: %u %u", SEG_SEQ(tcphdr), tcb->rcv_nxt);
        TcpStats.misordered++;

        NeedSendACKNow(tcb);
        TcbBufferMisorderedSegment(tcb, seq, recvbuf);
    }

    return tcb;
}


PRIVATE VOID
TcbProcessFIN(
    TCB* tcb,
    TcpHeader* tcphdr,
    TCPSEQ seqfin
    )

/*++

Routine Description:

    Process an incoming TCP FIN segment

Arguments:

    tcb - Points to the TCP control block
    tcphdr - Points to the TCP segment header
    seqfin - Sequence number for the FIN

Return Value:

    NONE

--*/

{
    if (seqfin != tcb->rcv_nxt) return;

    tcb->rcv_nxt = seqfin+1;
    NeedSendACKNow(tcb);
    PcbSignalEvent(tcb, PCBEVENT_CLOSE);

    switch (tcb->tcpstate) {
    case TCPST_SYN_RECEIVED:
    case TCPST_ESTABLISHED:
        PcbClearOverlappedRecvs(tcb, NETERR_OK);
        PcbSignalEvent(tcb, PCBEVENT_READ);
        TcbSetState(tcb, TCPST_CLOSE_WAIT, "FinRecv");
        break;

    case TCPST_FIN_WAIT_1:
        if (IsPcbSendBufEmpty(tcb)) {
            TcbSetState(tcb, TCPST_CLOSING, "FinRecv");
            break;
        }

        // If our FIN has been acknowledged,
        // fall through and change to TIME-WAIT state.

    case TCPST_FIN_WAIT_2:
        TcbSetState(tcb, TCPST_TIME_WAIT, "FinRecv");

        // Fall through

    case TCPST_TIME_WAIT:
        TcbSetTimeWaitTimer(tcb, 2*cfgMSL);
        break;
    }
}


//
// Structure for storing TCP option parameters
// from an incoming SYN segment
//
typedef struct _TcpOptions {
    UINT mss;
    // other TCP option parameters ...
} TcpOptions;

PRIVATE BOOL
TcpParseOptions(
    Packet* pkt,
    TcpHeader* tcphdr,
    TcpOptions* opts
    )

/*++

Routine Description:

    Parse the option information in an incoming TCP SYN segment

Arguments:

    pkt - Points to the received packet
    tcphdr - Points to the TCP segment header
    opts - Returns the parsed option information

Return Value:

    TRUE if successful, FALSE if the TCP options are not well-formed

--*/

#define TCP_MINIMUM_MSS (MAXIPHDRLEN+MAXTCPHDRLEN+8-IPHDRLEN-TCPHDRLEN)

{
    const BYTE* buf;
    UINT buflen;

    // We ignore any data that's sent in the initial SYN segment.
    // Not sure if this case actually happens in real-life.
    // In any case, the sender should retransmit the data.
    if (ISTCPSEG(tcphdr, SYN) && pkt->datalen > 0) {
        WARNING_("Ignoring data in SYN segment.");
    }

    // Use default values if no options are present
    opts->mss = TCP_DEFAULT_MSS;

    buflen = GETTCPHDRLEN(tcphdr) - TCPHDRLEN;
    buf = (const BYTE*) (tcphdr+1);

    while (buflen) {
        BYTE opt, optlen;

        if ((opt = *buf) == TCPOPT_EOL) break;
        if (opt == TCPOPT_NOP) {
            buf++; buflen--; 
            continue;
        }

        // Verify option length field
        if (buflen < 2 || (optlen = buf[1]) < 2 || optlen > buflen)
            return FALSE;

        switch (opt) {
        case TCPOPT_MAX_SEGSIZE:
            if (optlen != 4) return FALSE;
            opts->mss = ((UINT) buf[2] << 8) | buf[3];
            if (opts->mss < TCP_MINIMUM_MSS)
                opts->mss = TCP_DEFAULT_MSS;
            break;

        default:
            TRACE_("TCP option ignored: %d", opt);
            break;
        }

        buf += optlen;
        buflen -= optlen;
    }
    return TRUE;
}


//
// Save the relevant information in an incoming connection request
//
#define TcpSaveConnReqParams(_tcb, _tcphdr, _opts) { \
            (_tcb)->rcv_isn = SEG_SEQ(_tcphdr); \
            (_tcb)->rcv_nxt = (_tcb)->rcv_isn+1; \
            (_tcb)->snd_mss = (_opts)->mss; \
        }

PRIVATE BOOL
TcbAcceptConnReqPassive(
    TCB* tcb,
    Packet* pkt,
    TcpHeader* tcphdr
    )

/*++

Routine Description:

    Process an incoming TCP connection request
    that was made to a listening (passively opened) socket

Arguments:

    tcb - TCP control block
    pkt - Points to the incoming packet
    tcphdr - Points to the TCP segment header

Return Value:

    FALSE if the incoming connection request is bad and
    the caller should send out a RST in response; TRUE otherwise

--*/

{
    NTSTATUS status;
    TCB* tcbChild;
    TcpOptions opts;

    // Parse TCP options
    if (!TcpParseOptions(pkt, tcphdr, &opts)) return FALSE;

    tcbChild = TcbCloneChild(tcb);
    if (!tcbChild) return TRUE;

    TcbSetState(tcbChild, TCPST_SYN_RECEIVED, "Connection request");
    TcpSaveConnReqParams(tcbChild, tcphdr, &opts);

    status = TcbConnect(tcbChild, GETPKTIPHDR(pkt)->srcaddr, tcphdr->srcport, TRUE);
    if (!NT_SUCCESS(status)) {
        TcbDelete(tcbChild);
        return TRUE;
    }

    InsertHeadList(&PcbList, &tcbChild->links);
    return TRUE;
}


PRIVATE VOID
TcbConnectionEstablished(
    TCB* tcb,
    TcpHeader* tcphdr
    )

/*++

Routine Description:

    Change a TCP connection to established state

Arguments:

    tcb - Points to the TCP control block
    tcphdr - Points to the incoming SYN/ACK segment

Return Value:

    NONE

--*/

{
    TCPSEQ ack;
    TCPSENDBUF* sendbuf;

    TRACE_("Connection established: %s:%d ", IPADDRSTR(tcb->srcaddr), NTOHS(tcb->srcport));
    TRACE_("to %s:%d", IPADDRSTR(tcb->dstaddr), NTOHS(tcb->dstport));

    //
    // The incoming segment must acknowledge our SYN
    //
    sendbuf = TcbFirstSendbuf(tcb);
    ack = SEG_ACK(tcphdr);
    ASSERT(sendbuf->tcpflags & TCP_SYN);
    ASSERT(ISTCPSEG(tcphdr, ACK));
    ASSERT(ack == tcb->snd_nxt);

    tcb->snd_una = ack;
    TcbUpdateSndWnd(tcb, SEG_WIN(tcphdr), SEG_SEQ(tcphdr), ack);

    if (tcb->rtt_tick) {
        TcbUpdateRTO(tcb);
    }
    RemoveEntryList(&sendbuf->links);
    SendbufRelease(sendbuf);
    tcb->xmitTimer = tcb->synTimer = 0;

    // NOTE: Since we don't fragment outgoing IP datagrams, we need
    // to limit snd_mss to be less than the first-hop interface MTU
    // minus the TCP and IP headers.
    if (tcb->snd_mss > tcb->rcv_mss)
        tcb->snd_mss = tcb->rcv_mss;

    tcb->snd_cwnd = 2*tcb->snd_mss;
    tcb->snd_ssthresh = max(tcb->snd_wnd, tcb->snd_cwnd);
    tcb->rcv_swsthresh = min(tcb->maxRecvBufsize>>1, tcb->snd_mss);

    TcbSetState(tcb, TCPST_ESTABLISHED, "Connected");
    tcb->flags |= PCBFLAG_CONNECTED;
    PcbSignalEvent(tcb, PCBEVENT_CONNECT);

    if (IsPendingConnReqTcb(tcb)) {
        // Signal the connection request is ready for acceptance
        PcbSignalEvent(tcb->parent, PCBEVENT_ACCEPT);
    }
}


PRIVATE BOOL
TcbAcceptConnReqActive(
    TCB* tcb,
    Packet* pkt,
    TcpHeader* tcphdr
    )

/*++

Routine Description:

    Process an incoming TCP connection request
    that was made to an actively opened socket

Arguments:

    tcb - TCP control block
    pkt - Points to the incoming packet
    tcphdr - Points to the TCP segment header

Return Value:

    FALSE if the incoming connection request is bad and
    the caller should send out a RST in response; TRUE otherwise

--*/

{
    TcpOptions opts;
    NTSTATUS status;

    // Parse TCP options and save connection request information
    if (!TcpParseOptions(pkt, tcphdr, &opts)) return FALSE;
    TcpSaveConnReqParams(tcb, tcphdr, &opts);

    if (ISTCPSEG(tcphdr, ACK)) {
        TcbConnectionEstablished(tcb, tcphdr);
        NeedSendACKNow(tcb);
        status = NETERR_OK;
    } else {
        TcbSetState(tcb, TCPST_SYN_RECEIVED, "Simultaneous open");
        status = TcbEmitSYN(tcb, TRUE);
    }

    return NT_SUCCESS(status);
}


VOID
TcpReceivePacket(
    Packet* pkt
    )

/*++

Routine Description:

    Receive a TCP segment

Arguments:

    pkt - Points to the received TCP segment

Return Value:

    NONE

--*/

{
    IpHeader* iphdr;
    PseudoHeader pseudohdr;
    TcpHeader* tcphdr;
    UINT tcphdrlen;
    UINT checksum;
    TCB* tcb;
    BOOL isAck;
    TCPSEQ ack;

    // Verify TCP segment header
    if (pkt->datalen < TCPHDRLEN) goto discard;

    iphdr = GETPKTIPHDR(pkt);
    pseudohdr.srcaddr = iphdr->srcaddr;
    pseudohdr.dstaddr = iphdr->dstaddr;
    pseudohdr.zero = 0;
    pseudohdr.protocol = IPPROTOCOL_TCP;
    pseudohdr.length = (WORD) HTONS(pkt->datalen);

    tcphdr = GETPKTDATA(pkt, TcpHeader);
    tcphdrlen = GETTCPHDRLEN(tcphdr);
    if (tcphdrlen < TCPHDRLEN || tcphdrlen > pkt->datalen)
        goto discard;

    // Verify checksum
    checksum = tcpipxsum(0, &pseudohdr, sizeof(pseudohdr));
    if (tcpipxsum(checksum, tcphdr, pkt->datalen) != 0xffff)
        goto discard;

    pkt->data += tcphdrlen;
    pkt->datalen -= tcphdrlen;

    // Find the socket that the segment is addressed to
    tcb = (TCB*) PcbFindMatch(
                    pseudohdr.dstaddr,
                    tcphdr->dstport,
                    pseudohdr.srcaddr,
                    tcphdr->srcport,
                    SOCK_STREAM,
                    IPPROTOCOL_TCP);

    if (!tcb) goto sendrst;

    if (IsTcpSyncState(tcb) && (tcphdr->flags & TCP_CONTROLS) == 0) {
        // Quick check for special case:
        //  we're in a synchronized state and
        //  the segment has no control flags.
        tcb = TcbProcessDataAck(tcb, pkt, tcphdr);

rcvdone:
        // NOTE: We return the interface driver's buffer first
        // before trying to send out the ACK segment.

        XnetCompletePacket(pkt, NETERR_OK);
        if (tcb && tcb->delayedAcks >= (tcb->rcv_mss << 1)) {
            TcbEmitACK(tcb);
        }
        return;
    }
    
    // If the connection is closed, send RST
    if (tcb->tcpstate == TCPST_CLOSED) goto sendrst;

    // Discard packets with broadcast/multicast destination address.
    // We assume that packets with broadcast/multicast source address
    // are already discarded by the IP layer.
    if (IfBcastAddr(pkt->recvifp, pseudohdr.dstaddr) || 
        IS_MCAST_IPADDR(pseudohdr.dstaddr))
        goto discard;

    isAck = ISTCPSEG(tcphdr, ACK);

    switch (tcb->tcpstate) {
    case TCPST_LISTEN:

        // Ignore RST segment in listen state
        if (ISTCPSEG(tcphdr, RST)) goto discard;

        // If ACK is on, send RST
        if (isAck) goto sendrst;

        // If there is no SYN, discard the segment
        if (!ISTCPSEG(tcphdr, SYN)) goto discard;

        // Process an incoming connection request on a listening socket
        if (!TcbAcceptConnReqPassive(tcb, pkt, tcphdr)) goto sendrst;
        break;
    
    case TCPST_SYN_SENT:

        // If ACK is on and the acknowledgement number is bad, send RST
        if (isAck) {
            ack = SEG_ACK(tcphdr);
            if (!IsValidACK(tcb, ack)) goto sendrst;
        }

        if (ISTCPSEG(tcphdr, RST)) {
            // If RST is on and ACK is good, reset the connection.
            // Otherwise, discard the RST segment.
            if (isAck) { TcbReset(tcb, NETERR_CONNRESET); }
            goto discard;
        }

        // If there is no SYN, just discard the segment
        if (!ISTCPSEG(tcphdr, SYN)) goto discard;

        // Simultaneous active open
        if (!TcbAcceptConnReqActive(tcb, pkt, tcphdr)) goto sendrst;
        break;
    
    default: {
        TCPSEQ oldseq0 = SEG_SEQ(tcphdr);
        TCPSEQ oldseq1 = oldseq0 + SEG_LEN(tcphdr, pkt->datalen);
        TCPSEQ seq0, seq1;

        // Make sure the sequence number is correct.
        // If not and the incoming segment is not RST, we'll emit an ACK.
        if (!TcbValidateSeqs(tcb, oldseq0, oldseq1, &seq0, &seq1)) {
            if (!ISTCPSEG(tcphdr, RST)) {
                NeedSendACKNow(tcb);
            }
            break;
        }

        // If RST is on, then we'll reset the connection:
        //  - if the socket corresponds to a pending connection
        //    request, then we'll just close it and destroy the TCB.
        if (ISTCPSEG(tcphdr, RST)) {
            TcbReset(tcb, NETERR_CONNRESET);
            goto discard;
        }

        // If SYN is set, there is an error.
        // We send out a RST as well as reset the connection.
        if (ISTCPSEG(tcphdr, SYN)) {
            if (oldseq0 == seq0) {
                TcbReset(tcb, NETERR_CONNRESET);
                goto sendrst;
            }
            goto discard;
        }

        if (!ISTCPSEG(tcphdr, ACK)) goto discard;

        if (tcb->tcpstate == TCPST_SYN_RECEIVED) {
            ack = SEG_ACK(tcphdr);
            if (!IsValidACK(tcb, ack)) {
                // The acknowledgement number is bad, emit an RST
                goto sendrst;
            }

            // NOTE: The ACK for our SYN will be processed
            // inside the TcbProcessDataAck call below.
            TcbConnectionEstablished(tcb, tcphdr);
        }

        // Process the data and acknowledgement information
        // and continue to process the FIN flag if necessary
        tcb = TcbProcessDataAck(tcb, pkt, tcphdr);
        if (tcb && ISTCPSEG(tcphdr, FIN)) {
            TcbProcessFIN(tcb, tcphdr, seq1-1);
        }
        }
        break;
    }
    goto rcvdone;

sendrst:

    // If the received segment is a RST, do nothing
    if ((tcphdr->flags & TCP_RST) == 0) {
        TCPSEQ seq;
        BYTE flags;
        IpAddrPair addrpair;
        IfInfo* ifp;

        if (ISTCPSEG(tcphdr, ACK)) {
            ack = 0;
            seq = SEG_ACK(tcphdr);
            flags = 0;
        } else {
            ack = SEG_SEQ(tcphdr) + SEG_LEN(tcphdr, pkt->datalen);
            seq = 0;
            flags  = TCP_ACK;
        }

        addrpair.dstaddr = iphdr->srcaddr;
        addrpair.srcaddr = iphdr->dstaddr;
        addrpair.dstport = tcphdr->srcport;
        addrpair.srcport = tcphdr->dstport;
        ifp = pkt->recvifp;

        // NOTE: We return the interface driver's buffer first
        // before trying to send out the RST segment.

        XnetCompletePacket(pkt, NETERR_DISCARDED);
        TcbEmitRST(ifp, &addrpair, seq, ack, flags);
        return;
    }

discard:
    XnetCompletePacket(pkt, NETERR_DISCARDED);
}

