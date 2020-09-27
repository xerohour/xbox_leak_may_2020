// ---------------------------------------------------------------------------------------
// socktcp.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

// ---------------------------------------------------------------------------------------
// Trace Tags
// ---------------------------------------------------------------------------------------

DefineTag(tcpStat,  0);
DefineTag(tcpWarn,  TAG_ENABLE);

// ---------------------------------------------------------------------------------------
// TcpTimer
// ---------------------------------------------------------------------------------------

// TCP option length in our SYN segment
#define SYNTCPOPTLEN 4

void CXnSock::TcpTimer(CTimer * pt)
{
    CTcpSocket * pTcpSocket = CTcpSocket::TimerToSocket(pt);
    DWORD dwTickNow  = TimerTick();
    DWORD dwTickNext;

    if (pTcpSocket->delayedAcks)
    {
        TcpEmitACK(pTcpSocket);
    }

    if (pTcpSocket->dwTickSyn && dwTickNow >= pTcpSocket->dwTickSyn)
    {
        pTcpSocket->dwTickSyn = 0;
        if (TcpReset(pTcpSocket, NETERR_TIMEOUT) == NULL)
            return;
    }

    if (pTcpSocket->dwTickXmit && dwTickNow >= pTcpSocket->dwTickXmit)
    {
        pTcpSocket->dwTickXmit = 0;
        if (TcpXmitTimeout(pTcpSocket) == NULL)
            return;
    }

    if (pTcpSocket->dwTickWait && dwTickNow >= pTcpSocket->dwTickWait)
    {
        pTcpSocket->dwTickWait = 0;
        Assert(pTcpSocket->IsLingering());
        SockClose(pTcpSocket, TRUE);
        return;
    }

    dwTickNext = pTcpSocket->delayedAcks ? dwTickNow : TIMER_INFINITE;

    if (pTcpSocket->dwTickSyn > 0 && pTcpSocket->dwTickSyn < dwTickNext)
        dwTickNext = pTcpSocket->dwTickSyn;
    if (pTcpSocket->dwTickXmit > 0 && pTcpSocket->dwTickXmit < dwTickNext)
        dwTickNext = pTcpSocket->dwTickXmit;
    if (pTcpSocket->dwTickWait > 0 && pTcpSocket->dwTickWait < dwTickNext)
        dwTickNext = pTcpSocket->dwTickWait;

    TimerSet(pTcpSocket->GetTimer(), dwTickNext);
}

CTcpSocket * CXnSock::TcpXmitTimeout(CTcpSocket* pTcpSocket)

/*++

Routine Description:

    This function is called when the TCP retransmission timer expires.

Arguments:

    pTcpSocket - Points to the TCP control block

Return Value:

    NULL if the specified CTcpSocket was deleted inside this call.
    Otherwise, just return the input pTcpSocket parameter.

--*/

{
    if (pTcpSocket->IsSendBufEmpty())
        return(pTcpSocket);

    CTcpSendBuf* pTcpSendBuf = (CTcpSendBuf *)pTcpSocket->GetSendBufFirst();

    if (pTcpSendBuf->retries >= cfgSockRexmitRetries)
    {
        return(TcpReset(pTcpSocket, NETERR_TIMEOUT));
    }

    // Stop round-trip time measurement if there is retransmission
    pTcpSocket->rtt_tick = 0;

    // Retransmit the first segment
    if (pTcpSocket->TestFlags(SOCKF_PERSIST))
    {
        if (pTcpSendBuf->retries == 0)
        {
            // Send out the window probe segment for the first time.
            // BUGBUG: we always send out the whole segment instead
            // of just a single byte of data.

            Assert(pTcpSendBuf == pTcpSocket->GetSendBufPtr());
            TcpSendSegment(pTcpSocket, pTcpSendBuf, FALSE);
            pTcpSocket->snd_nxt = pTcpSendBuf->seqnext;
            pTcpSocket->SetSendBufPtr((CTcpSendBuf *)pTcpSocket->GetSendBufNext(pTcpSendBuf));
            return(pTcpSocket);
        }
    }
    else
    {
        TraceSz3(tcpStat, "[%X.t] Retransmission timeout seq %08lX (%d retries)",
                 pTcpSocket, pTcpSendBuf->seq, pTcpSendBuf->retries);
        Assert(pTcpSendBuf->retries != 0);
        // Congestion control:
        //  set the slow-start threshold to be half the flight size
        //  and set the congestion window to be 1 SMSS
        pTcpSocket->ResetSlowStartThresh();
        pTcpSocket->snd_cwnd = pTcpSocket->snd_mss;
        TcpStopFastRexmitMode(pTcpSocket);
    }

    TcpSendSegment(pTcpSocket, pTcpSendBuf, pTcpSendBuf->retries != 0);
    return(pTcpSocket);
}

CTcpSocket * CXnSock::TcpReset(CTcpSocket* pTcpSocket, NTSTATUS status)

/*++

Routine Description:

    Reset a TCP connection

Arguments:

    pTcpSocket - Points to the TCP control block
    status - Specifies the cause of the reset

Return Value:

    NULL if the specified CTcpSocket was deleted inside this call.
    Otherwise, just return the input pTcpSocket parameter.

--*/

{
    BOOL fRevivable;

    Assert(!pTcpSocket->IsIdleState());
    
    if (pTcpSocket->IsChild() || pTcpSocket->IsLingering())
    {
        SockClose(pTcpSocket, TRUE);
        return NULL;
    }

    // Remove the CTcpSocket from the active list
    // and move it to the zombie list.

    pTcpSocket->Dequeue();
    pTcpSocket->Enqueue(&_leDeadSockets);

    pTcpSocket->SetStatus((status != NETERR_CONNRESET || pTcpSocket->IsSyncState()) ? status : NETERR(WSAECONNREFUSED));

    fRevivable = !pTcpSocket->IsSyncState() && !pTcpSocket->TestFlags(SOCKF_NOMORE_XMIT|SOCKF_NOMORE_RECV);
    pTcpSocket->SetFlags(SOCKF_NOMORE_XMIT|SOCKF_NOMORE_RECV);
    TcpSetState(pTcpSocket, TCPST_CLOSED, "Reset");

    // Stop various timers
    // NOTE: This is a little hacky - we're clearing all the fields
    // from delayedAcks to rtt_tick in one shot. Be careful when you
    // add or remove fields from the CTcpSocket structure.

    memset(&pTcpSocket->delayedAcks, 0,
           offsetof(CTcpSocket, rtt_tick) + sizeof(pTcpSocket->rtt_tick) - offsetof(CTcpSocket, delayedAcks));

    TimerSet(pTcpSocket->GetTimer(), TIMER_INFINITE);
    pTcpSocket->TcpInit(this);
    pTcpSocket->ClearFlags(SOCKF_PERSIST|SOCKF_FAST_RETRANSMIT);
    pTcpSocket->SetDupAcks(0);
    pTcpSocket->SignalEvent(SOCKF_EVENT_MASK);

    SockCleanup(pTcpSocket);

    if (fRevivable)
    {
        pTcpSocket->SetFlags(SOCKF_REVIVABLE);
    }

    return(pTcpSocket);
}

CTcpSocket * CXnSock::TcpCloneChild(CTcpSocket * pTcpSocket)

/*++

Routine Description:

    Add a pending connection request to a listening socket

Arguments:

    pTcpSocket - Points to the listening socket

Return Value:

    Pointer to the child socket
    NULL if there is an error

--*/

{
    // Fail if the backlog is full
    Assert(pTcpSocket->IsListenState());

    if (pTcpSocket->GetBacklog() >= pTcpSocket->GetBacklogMax())
    {
        TraceSz2(Warning, "[%X.t] Reached backlog limit for listening socket (%d)",
                 pTcpSocket, pTcpSocket->GetBacklogMax());
        return(NULL);
    }
    
    // Allocate memory for the child socket
    CTcpSocket * pTcpSocketChild = (CTcpSocket *)SockAlloc(TRUE, TRUE);

    if (!pTcpSocketChild)
    {
        TraceSz1(Warning, "[%X.t] Out of memory allocating child of listening socket",
                 pTcpSocket);
        return(NULL);
    }

    pTcpSocket->EnqueueChild(pTcpSocketChild);

    return(pTcpSocketChild);
}
            
void CXnSock::TcpEmitRST(CTcpSocket * pTcpSocket, CIpAddr ipaDst, CIpPort ipportDst,
                         CIpPort ipportSrc, TCPSEQ seq, TCPSEQ ack, BYTE flags)
{
    CPacket * pkt = TcpPacketAlloc(pTcpSocket, ipaDst, PKTF_POOLALLOC);
    if (!pkt) return;

    CTcpHdr * pTcpHdr   = pkt->GetTcpHdr();
    pTcpHdr->SetHdrLen(sizeof(CTcpHdr));
    pTcpHdr->_ipportSrc = ipportSrc;
    pTcpHdr->_ipportDst = ipportDst;
    pTcpHdr->_dwSeqNum  = HTONL(seq);
    pTcpHdr->_dwAckNum  = HTONL(ack);
    pTcpHdr->_bFlags    = (BYTE)(TCP_RST | flags);
    pTcpHdr->_wWindow   = 0;
    pTcpHdr->_wUrgent   = 0;
    pTcpHdr->_wChecksum = 0;

    TraceTcpHdr(pktXmit, pTcpSocket, pTcpHdr, 0, 0);
    IpFillAndXmit(pkt, ipaDst, IPPROTOCOL_TCP);
}


// Return the effective snd_nxt value for a TCP connection
// NOTE: we cannot use CTcpSocket.snd_nxt field directly here
// because the way we're doing persisting causes snd_nxt
// to go past the send window.
TCPSEQ CXnSock::TcpSndNxt(CTcpSocket* pTcpSocket)
{
    TCPSEQ sndmax = pTcpSocket->snd_wl2 + pTcpSocket->snd_wnd;
    return (pTcpSocket->IsSyncState() && SEQ_GT(pTcpSocket->snd_nxt, sndmax)) ?
                sndmax :
                pTcpSocket->snd_nxt;
}

// Send a RST segment to the TCP connection peer
VOID CXnSock::TcpResetPeer(CTcpSocket* pTcpSocket)
{
    TcpEmitRST(pTcpSocket, pTcpSocket->_ipaDst, pTcpSocket->_ipportDst, pTcpSocket->_ipportSrc,
               TcpSndNxt(pTcpSocket), pTcpSocket->rcv_nxt, TCP_ACK);
}

NTSTATUS CXnSock::TcpEmitSYN(CTcpSocket* pTcpSocket, BOOL synAck)

/*++

Routine Description:

    Emit a TCP SYN segment

Arguments:

    pTcpSocket - Points to the TCP control block
    synAck - Whether to send ACK as well

Return Value:

    Status code

--*/

{
    CPacket* pkt;
    BYTE* opt;
    CTcpSendBuf* pTcpSendBuf;

    // Check if we're already trying to send out a SYN segment
    if (!pTcpSocket->IsSendBufEmpty())
    {
        pTcpSendBuf = (CTcpSendBuf *)pTcpSocket->DequeueSendBuf();
        SockReleaseSendBuf(pTcpSendBuf);
        Assert(pTcpSocket->IsSendBufEmpty());
        Assert(pTcpSocket->GetSendBufPtr() == NULL);
    }

    pTcpSocket->snd_una = pTcpSocket->snd_nxt = pTcpSocket->snd_end = pTcpSocket->snd_isn;

    // Allocate a TCP send buffer for possible retransmission
    pkt = TcpPacketAlloc(pTcpSocket, pTcpSocket->_ipaDst, synAck ? PKTF_POOLALLOC : 0, SYNTCPOPTLEN,
                         sizeof(CTcpSendBuf), (PFNPKTFREE)TcpPacketFree);
    if (!pkt)
    {
        // This is very bad - we failed to allocate memory
        // for the SYN segment. We'll mark the CTcpSocket as dead
        // and return an error code.
        TcpReset(pTcpSocket, NETERR_MEMORY);
        return NETERR_MEMORY;
    }

    pTcpSocket->dwTickSyn = TimerSetRelative(pTcpSocket->GetTimer(), cfgSockConnectTimeoutInSeconds * TICKS_PER_SECOND);

    opt = (BYTE *)pkt->GetTcpHdr() + sizeof(CTcpHdr);
    opt[0] = TCPOPT_MAX_SEGSIZE;
    opt[1] = SYNTCPOPTLEN;
    opt[2] = (BYTE) (pTcpSocket->rcv_mss >> 8);
    opt[3] = (BYTE) pTcpSocket->rcv_mss;

    TcpQueueSegment(pTcpSocket, pkt, (BYTE) (synAck ? TCP_SYN|TCP_ACK : TCP_SYN), 0);
    return NETERR_OK;
}


void CXnSock::TcpEmitACK(CTcpSocket* pTcpSocket)

/*++

Routine Description:

    Send out an ACK segment on a TCP connection

Arguments:

    pTcpSocket - Points to the TCP control block

Return Value:

    NONE

--*/

{
    CPacket * ppkt = TcpPacketAlloc(pTcpSocket, pTcpSocket->_ipaDst, PKTF_POOLALLOC);
    if (!ppkt)
        return;

    TCPSEQ seq = TcpSndNxt(pTcpSocket);
    pTcpSocket->delayedAcks = 0;

    CTcpHdr * pTcpHdr   = ppkt->GetTcpHdr();
    pTcpHdr->SetHdrLen(sizeof(CTcpHdr));
    pTcpHdr->_ipportSrc = pTcpSocket->_ipportSrc;
    pTcpHdr->_ipportDst = pTcpSocket->_ipportDst;
    pTcpHdr->_dwSeqNum  = HTONL(seq);
    pTcpHdr->_dwAckNum  = HTONL(pTcpSocket->rcv_nxt);
    pTcpHdr->_bFlags    = TCP_ACK;
    pTcpHdr->_wWindow   = HTONS((WORD)pTcpSocket->rcv_wnd);
    pTcpHdr->_wUrgent   = 0;
    pTcpHdr->_wChecksum = 0;

    TraceTcpHdr(pktXmit, pTcpSocket, pTcpHdr, 0, 0);
    IpFillAndXmit(ppkt, pTcpSocket->_ipaDst, IPPROTOCOL_TCP, &pTcpSocket->_prte);
}


NTSTATUS CXnSock::TcpEmitFIN(CTcpSocket * pTcpSocket)

/*++

Routine Description:

    Send out a FIN segment on a TCP connection

Arguments:

    pTcpSocket - Points to the TCP control block

Return Value:

    Status code

--*/

{
    ICHECK(SOCK, UDPC);
    CPacket * ppkt = TcpPacketAlloc(pTcpSocket, pTcpSocket->_ipaDst, 0, 0, sizeof(CTcpSendBuf), (PFNPKTFREE)TcpPacketFree);
    if (!ppkt) return NETERR_MEMORY;
    TcpQueueSegment(pTcpSocket, ppkt, TCP_ACK|TCP_FIN, 0);
    return(NETERR_OK);
}

//
// Update the receive window information after accepting
// the specified amount data from the connection peer.
//
void CXnSock::TcpSlideRecvWindow(CTcpSocket* pTcpSocket, UINT datalen)
{
    Assert(pTcpSocket->_cbMaxRecvBuf >= pTcpSocket->GetCbRecvBuf());

    UINT newwnd = pTcpSocket->_cbMaxRecvBuf - pTcpSocket->GetCbRecvBuf();

    // Here datalen must be <= rcv_wnd.
    Assert(datalen <= pTcpSocket->rcv_wnd);
    pTcpSocket->rcv_nxt += datalen;

    if ((datalen + newwnd) - pTcpSocket->rcv_wnd >= pTcpSocket->rcv_swsthresh) {
        // Enough space has freed up. Move the right edge of the receive window.
        pTcpSocket->rcv_wnd = newwnd;
    } else {
        // Implement receive side silly window avoidance:
        //  keep the right edge of the receive window unchanged.
        pTcpSocket->rcv_wnd -= datalen;
    }
}

void CXnSock::TcpUpdateRTO(CTcpSocket* pTcpSocket)

/*++

Routine Description:

    Update the round-trip time measurements for a TCP connection

Arguments:

    pTcpSocket - Points to the TCP control block

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

    nticks = TimerTick() - pTcpSocket->rtt_tick;
    pTcpSocket->rtt_tick = 0;

    if (pTcpSocket->srtt_8 != 0) {
        delta = nticks - (pTcpSocket->srtt_8 >> SRTT_SHIFT);
        if ((pTcpSocket->srtt_8 += delta) <= 0)
            pTcpSocket->srtt_8 = 1;

        if (delta < 0) delta = -delta;
        delta -= (pTcpSocket->rttvar_4 >> RTTVAR_SHIFT);
        if ((pTcpSocket->rttvar_4 += delta) <= 0)
            pTcpSocket->rttvar_4 = 1;
    } else {
        // The very first measurement - use the unsmoothed data
        if (nticks == 0) {
            pTcpSocket->srtt_8 = pTcpSocket->rttvar_4 = 1;
        } else {
            pTcpSocket->srtt_8 = nticks << SRTT_SHIFT;
            pTcpSocket->rttvar_4 = nticks << (RTTVAR_SHIFT-1);
        }
    }

    pTcpSocket->RTO = (pTcpSocket->srtt_8 >> SRTT_SHIFT) + pTcpSocket->rttvar_4;
    if (pTcpSocket->RTO < (UINT)cfgSockRexmitMinTimeoutInSeconds * TICKS_PER_SECOND)
        pTcpSocket->RTO = (UINT)cfgSockRexmitMinTimeoutInSeconds * TICKS_PER_SECOND;
}


BOOL CXnSock::TcpUpdatePersistFlag(CTcpSocket* pTcpSocket)

/*++

Routine Description:

    This function is called after the send window information is updated.
    If we're currently sending out window probing segments, then
    we'll stop doing so if the send window has opened up.

Arguments:

    pTcpSocket - Points to the TCP control block

Return Value:

    FALSE if we got out of persist mode
    TRUE otherwise

--*/

{
    Assert(!pTcpSocket->IsSendBufEmpty());
    CTcpSendBuf* pTcpSendBuf = (CTcpSendBuf *)pTcpSocket->GetSendBufFirst();

    if (pTcpSendBuf->GetCbBuf() <= pTcpSocket->snd_wnd)
    {
        pTcpSocket->ClearFlags(SOCKF_PERSIST);

        if (pTcpSendBuf->retries)
        {
            TraceSz1(tcpStat, "[%X.t] Retransmit persisting segment...", pTcpSocket);
            pTcpSendBuf->retries = 0;
            TcpSendSegment(pTcpSocket, pTcpSendBuf, TRUE);
        }
        else
        {
            pTcpSocket->dwTickXmit = 0;
        }

        return(FALSE);
    }

    return(TRUE);
}


void CXnSock::TcpUpdateSndUna(CTcpSocket* pTcpSocket, TCPSEQ ack)

/*++

Routine Description:

    Update the snd.una variable for a TCP connection

Arguments:

    pTcpSocket - Points to the TCP connection block
    ack - The acknowledged sequence number

Return Value:

    NONE

--*/

{
    CTcpSendBuf *   pTcpSendBuf;
    CTcpSendBuf *   pTcpSendBufNext;
    UINT            ackedSends = 0;

    if (SEQ_GT(ack, pTcpSocket->snd_una))
    {
        pTcpSocket->snd_una = ack;

        // Update round-trip time measurements
        if (pTcpSocket->rtt_tick && SEQ_GT(ack, pTcpSocket->rtt_seq))
        {
            TcpUpdateRTO(pTcpSocket);
        }

        // Complete fully acknowledged send user requests

        pTcpSendBuf = (CTcpSendBuf *)pTcpSocket->GetSendBufFirst();

        while (pTcpSendBuf)
        {
            if (pTcpSendBuf->retries && SEQ_GE(ack, pTcpSendBuf->seqnext))
            {
                pTcpSendBufNext = (CTcpSendBuf *)pTcpSocket->GetSendBufNext(pTcpSendBuf);
                pTcpSocket->DequeueSendBuf();
                pTcpSocket->DecCbSendBuf(pTcpSendBuf->GetCbBuf());
                SockReleaseSendBuf(pTcpSendBuf);
                ackedSends++;
                pTcpSendBuf = pTcpSendBufNext;
            }
            else
            {
                break;
            }
        }
    }

    // Update congestion window
    if (pTcpSocket->TestFlags(SOCKF_FAST_RETRANSMIT))
    {
        if (ackedSends)
        {
            //
            // Getting out of fast retransmit / fast recovery mode:
            //  "deflate" the congestion window
            //
            pTcpSocket->snd_cwnd = pTcpSocket->snd_ssthresh;
        }
        else
        {
            //
            // In fast recovery mode:
            //  increment the congestion window by SMSS
            //  for every duplicate ACK received.
            //
            pTcpSocket->IncrementCongestionWindow(pTcpSocket->snd_mss);
        }
    }
    else
    {
        if (pTcpSocket->snd_cwnd < pTcpSocket->snd_ssthresh)
        {
            //
            // Slow-start mode:
            //  increment the congestion window by SMSS
            //
            pTcpSocket->IncrementCongestionWindow(pTcpSocket->snd_mss);
        }
        else
        {
            //
            // Congestion avoidance mode:
            //  increment the congestion window by ~SMSS per RTT
            //
            // NOTE: Overflow is not possible here because snd_mss
            // and snd_cwnd are 16-bit numbers. Also snd_cwnd is never 0.
            //
            UINT inc = pTcpSocket->snd_mss * pTcpSocket->snd_mss / pTcpSocket->snd_cwnd;
            pTcpSocket->IncrementCongestionWindow(max(1, inc));
        }
    }

    if (ackedSends)
    {
        pTcpSocket->ClearFlags(SOCKF_PERSIST);
        TcpStopFastRexmitMode(pTcpSocket);

        // Reset retransmission timer
        pTcpSocket->dwTickXmit = 0;

        if (pTcpSocket->snd_una != pTcpSocket->snd_nxt)
        {
            pTcpSendBuf = (CTcpSendBuf *)pTcpSocket->GetSendBufFirst();

            if (pTcpSendBuf->firstSendTime + pTcpSocket->RTO <= TimerTick())
            {
                TcpXmitTimeout(pTcpSocket);
            }
            else
            {
                pTcpSocket->dwTickXmit = TimerSetRelative(pTcpSocket->GetTimer(), pTcpSendBuf->firstSendTime + pTcpSocket->RTO - TimerTick());
            }
        }

        if (!pTcpSocket->IsSendBufFull())
        {
            if (pTcpSocket->HasSendReq())
            {
                CSendReq * pSendReq = pTcpSocket->GetSendReq();
                NTSTATUS status = TcpSend(pTcpSocket, pSendReq, PKTF_POOLALLOC);
                pSendReq->_pWsaOverlapped->_ioxfercnt = pSendReq->sendtotal;
                SockReqComplete(pTcpSocket, pSendReq, status);
            }
            else
            {
                pTcpSocket->SignalEvent(SOCKF_EVENT_WRITE);
            }
        }
    }
    else if (pTcpSocket->TestFlags(SOCKF_PERSIST))
    {
        // If we're probing the send window and the ack was for
        // the probe segment, then make sure we continue to probe
        // without timing out.

        pTcpSendBuf = (CTcpSendBuf *)pTcpSocket->GetSendBufFirst();

        if (SEQ_GE(ack, pTcpSendBuf->seq) &&
            pTcpSendBuf->retries >= cfgSockRexmitRetries-1)
        {
            pTcpSendBuf->retries--;
        }
    }
    else
    {
        if (pTcpSocket->snd_una != pTcpSocket->snd_nxt &&
            !pTcpSocket->TestFlags(SOCKF_FAST_RETRANSMIT) &&
            pTcpSocket->IncDupAcks() >= 4)
        {
            //
            // Too many duplicate ACK received:
            //  do fast retransmit / recovery
            //
            TcpDoFastRexmit(pTcpSocket);
        }
    }

    // If the send window has opened up and
    // we have pending data to send, try to do it now.
    if (!pTcpSocket->TestFlags(SOCKF_PERSIST) || !TcpUpdatePersistFlag(pTcpSocket))
    {
        while (pTcpSocket->GetSendBufPtr() && TcpStartOutput(pTcpSocket))
            NULL;
    }
}

void CXnSock::TcpDoFastRexmit(CTcpSocket* pTcpSocket)

/*++

Routine Description:

    Initiate the TCP fast retransmit / recovery procedure

Arguments:

    pTcpSocket - Points to the TCP control block

Return Value:

    NONE

--*/

{
    CTcpSendBuf* pTcpSendBuf;

    TraceSz2(tcpStat, "[%X.t] Fast rexmit mode: %08lX", pTcpSocket, pTcpSocket->snd_una);

    // Reset the slow-start threshold
    // and set the congestion window to be that plus 3*SMSS
    pTcpSocket->ResetSlowStartThresh();
    pTcpSocket->snd_cwnd = pTcpSocket->snd_ssthresh + 3*pTcpSocket->snd_mss;
    pTcpSocket->SetFlags(SOCKF_FAST_RETRANSMIT);
    pTcpSocket->SetDupAcks(0);

    // Retransmit the "lost" segment immediately
    Assert(pTcpSocket->HasSendBuf());
    pTcpSendBuf = (CTcpSendBuf *)pTcpSocket->GetSendBufFirst();
    Assert(pTcpSendBuf->retries);
    TcpSendSegment(pTcpSocket, pTcpSendBuf, TRUE);
}


// 
// Check if an acknowledged sequence number is valid for a TCP connection
//
#define IsValidACK(_pTcpSocket, _ack) \
        (SEQ_GT((_ack), (_pTcpSocket)->snd_una) && \
         SEQ_LE((_ack), (_pTcpSocket)->snd_nxt))

//
// Indicate whether we should send out an ACK immediately
// in response to an incoming segment.
//
//#define NeedSendACKNow(_pTcpSocket) ((_pTcpSocket)->delayedAcks += 0x20000)

void CXnSock::NeedSendACKNow(CTcpSocket * pTcpSocket)
{
    pTcpSocket->delayedAcks += 0x20000;
}

//
// Save the send window information from the connection peer
//
#define TcpUpdateSndWnd(_pTcpSocket, _wnd, _seq, _ack) { \
            (_pTcpSocket)->snd_wnd = (_wnd); \
            (_pTcpSocket)->snd_wl1 = (_seq); \
            (_pTcpSocket)->snd_wl2 = (_ack); \
        }


BOOL CXnSock::TcpValidateSeqs(CTcpSocket* pTcpSocket, TCPSEQ oldseq0, TCPSEQ oldseq1, TCPSEQ* newseq0, TCPSEQ* newseq1)

/*++

Routine Description:

    Check if the received sequence number is valid for a TCP connection

Arguments:

    pTcpSocket - Points to TCP control block
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

    rcv_last = pTcpSocket->rcv_nxt + pTcpSocket->rcv_wnd;
    *newseq0 = SEQ_LT(oldseq0, pTcpSocket->rcv_nxt) ? pTcpSocket->rcv_nxt : oldseq0;
    *newseq1 = SEQ_GT(oldseq1, rcv_last) ? rcv_last : oldseq1;

    return SEQ_LT(*newseq0, *newseq1) ||
           (*newseq0 == *newseq1 && oldseq0 == oldseq1);
}


#if DBG

BOOL CXnSock::TcpVerifyRecvBuf(CTcpSocket* pTcpSocket)

/*++

Routine Description:

    Verify the current receive buffers for a CTcpSocket is good

Arguments:

    pTcpSocket - Points to the TCP control block

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    if (!pTcpSocket->HasRecvBuf())
        return(TRUE);

    CTcpRecvBuf * pTcpRecvBuf0 = (CTcpRecvBuf *)pTcpSocket->GetRecvBufFirst();
    CTcpRecvBuf * pTcpRecvBuf1;

    while (1)
    {
        pTcpRecvBuf1 = (CTcpRecvBuf *)pTcpSocket->GetRecvBufNext(pTcpRecvBuf0);

        if (pTcpRecvBuf1 == NULL)
            return(TRUE);

        if (SEQ_GT(pTcpRecvBuf0->seqnext, pTcpRecvBuf1->seqnext - pTcpRecvBuf1->GetCbBuf()))
            return(FALSE);

        pTcpRecvBuf0 = pTcpRecvBuf1;
    }
}

#endif // DBG


UINT CXnSock::TcpCorrectMisorderedSegments(CTcpSocket* pTcpSocket, TCPSEQ seq, CTcpRecvBuf* recvbuf)

/*++

Routine Description:

    This function is called when we receive a segment
    whose starting sequence number matches rcv_nxt and
    there are buffered out-of-order segments after this one.

Arguments:

    pTcpSocket - Points to TCP control block
    seq - Specifies the starting sequence number for this segment
    recvbuf - Points to the received data buffer

Return Value:

    Number of continuous bytes that can be acknowledged

--*/

{
    CTcpRecvBuf* buf0 = (CTcpRecvBuf *)pTcpSocket->GetRecvBufLast();
    CTcpRecvBuf* buf1;
    UINT count;

    do
    {
        buf0 = (CTcpRecvBuf *)pTcpSocket->GetRecvBufPrev(buf0);
    }
    while (buf0 && buf0->seqnext != seq);

    buf1 = (CTcpRecvBuf *)pTcpSocket->GetRecvBufNext(buf0);

    if (SEQ_GT(recvbuf->seqnext, buf1->seqnext - buf1->GetCbBuf())) {
        // Weird case: data in this segment overlapps with
        // data in the buffered out-of-order segments.
        // Chop off the overlapped data in the current receive buffer.
        TraceSz1(tcpStat, "[%X.t] TcpCorrectMisorderedSegments: overlapping segment", pTcpSocket);

        count = recvbuf->seqnext - (buf1->seqnext - buf1->GetCbBuf());
        Assert(count < recvbuf->GetCbBuf());

        recvbuf->DecCbBuf(count);
        recvbuf->seqnext -= count;
    }

    // Insert recvbuf after buf0 and before buf1
    pTcpSocket->InsertRecvBuf(recvbuf, buf0);

    count = recvbuf->GetCbBuf();
    buf0 = recvbuf;
    while (buf1 && buf0->seqnext == buf1->seqnext - buf1->GetCbBuf())
    {
        count += buf1->GetCbBuf();
        buf0 = buf1;
        buf1 = (CTcpRecvBuf *)pTcpSocket->GetRecvBufNext(buf1);
    }

    // If we filled up a hole, emit an ACK immediately
    if (count > recvbuf->GetCbBuf())
    {
        NeedSendACKNow(pTcpSocket);
    }

    // Assert(TcpVerifyRecvBuf(pTcpSocket));
    return count;
}


void CXnSock::TcpBufferMisorderedSegment(CTcpSocket* pTcpSocket, UINT seq, CTcpRecvBuf* recvbuf)


/*++

Routine Description:

    This function is called when we receive an out-of-order segment.

Arguments:

    pTcpSocket - Points to TCP control block
    seq - Specifies the starting sequence number for this segment
    recvbuf - Points to the received data buffer

Return Value:

    NONE

--*/

{
    CTcpRecvBuf* buf0 = (CTcpRecvBuf *)pTcpSocket->GetRecvBufLast();
    CTcpRecvBuf* buf1;
    UINT count;

    if (buf0 == NULL || SEQ_GE(seq, buf0->seqnext))
    {
        // The sequence number for this segment is larger
        // than everything in the receive buffer.
        pTcpSocket->EnqueueRecvBuf(recvbuf);
        return;
    }

    do
    {
        buf0 = (CTcpRecvBuf *)pTcpSocket->GetRecvBufPrev(buf0);
    }
    while (buf0 != NULL && SEQ_LT(seq, buf0->seqnext));

    // recvbuf should go in between buf0 and buf1

    buf1 = (CTcpRecvBuf *)pTcpSocket->GetRecvBufNext(buf0);

    if (SEQ_GT(recvbuf->seqnext, buf1->seqnext - buf1->GetCbBuf()))
    {
        // Data in this segment overlapps with
        // data in the buffered out-of-order segments.
        TraceSz1(tcpStat, "[%X.t] TcpBufferMisorderedSegment: overlapping segment", pTcpSocket);
        
        count= recvbuf->seqnext - (buf1->seqnext - buf1->GetCbBuf());

        if (count >= recvbuf->GetCbBuf())
        {
            PoolFree(recvbuf);
            return;
        }

        recvbuf->DecCbBuf(count);
        recvbuf->seqnext -= count;
    }

    pTcpSocket->InsertRecvBuf(recvbuf, buf0);

    // Assert(TcpVerifyRecvBuf(pTcpSocket));
}


UINT CXnSock::TcpCompletePendingRecvReqFast(CTcpSocket* pTcpSocket, BYTE tcpflags, BYTE * pbData, UINT cbData)

/*++

Routine Description:

    This function is called when we received data from a TCP connection
    and there is a pending overlapped receive request
    and there is currently no data in the receive buffer.
    In this case we can satisfy the request right away without
    allocating memory to store data in the receive buffer.

Arguments:

    pTcpSocket - Points to the TCP control block
    tcpflags - TCP segment flags

Return Value:

    Number of bytes used up in this call

--*/

{
    UINT copycnt;
    CRecvReq* pRecvReq = pTcpSocket->GetRecvReq();

    copycnt = min(cbData, pRecvReq->buflen);
    memcpy(pRecvReq->buf, pbData, copycnt);
    pRecvReq->_pWsaOverlapped->_ioxfercnt += copycnt;
    pRecvReq->buf += copycnt;
    pRecvReq->buflen -= copycnt;

    if ((pRecvReq->buflen == 0) || (tcpflags & TCP_PSH|TCP_URG)) {
        SockReqComplete(pTcpSocket, pRecvReq, NETERR_OK);
    }

    TcpSlideRecvWindow(pTcpSocket, copycnt);
    return copycnt;
}


CTcpSocket * CXnSock::TcpRecvData(CTcpSocket* pTcpSocket, CTcpHdr* tcphdr, BYTE * pbData, UINT cbData)

/*++

Routine Description:

    Process a received TCP data segment.
    And we assume the connection is in a synchronized state.

Arguments:

    pTcpSocket - Points to the TCP connection block
    tcphdr - Points to the TCP segment header information

Return Value:

    NULL if the CTcpSocket was deleted as a result of the incoming segment;
    Otherwise, just return the input pTcpSocket parameter

--*/

{
    TCPSEQ seq, ack, seq0, seq1;
    CTcpRecvBuf* recvbuf;
    DWORD dwTickWait;

    seq = SEG_SEQ(tcphdr);

    // Process ACK if it's present
    if (ISTCPSEG(tcphdr, ACK))
    {
        ack = SEG_ACK(tcphdr);
        if (SEQ_LT(ack, pTcpSocket->snd_una))
        {
            // ACK is an old duplicate, ignore it
        }
        else if (SEQ_GT(ack, pTcpSocket->snd_nxt))
        {
            // ACKing something that hasn't been sent.
            // Emit an ACK, drop the segment
            NeedSendACKNow(pTcpSocket);
            return pTcpSocket;
        }
        else
        {
            // update the send window information
            if (SEQ_LT(pTcpSocket->snd_wl1, seq) ||
                pTcpSocket->snd_wl1 == seq && SEQ_LE(pTcpSocket->snd_wl2, ack))
            {
                TcpUpdateSndWnd(pTcpSocket, SEG_WIN(tcphdr), seq, ack);
            }

            TcpUpdateSndUna(pTcpSocket, ack);

            // If we already sent FIN, check to see if FIN has been acknowledged
            if (pTcpSocket->IsFinSent() && pTcpSocket->IsSendBufEmpty())
            {
                switch (pTcpSocket->GetState())
                {
                case TCPST_FIN_WAIT_1:
                    TcpSetState(pTcpSocket, TCPST_FIN_WAIT_2, "FinAck");
                    break;

                case TCPST_CLOSING:
                    TcpSetState(pTcpSocket, TCPST_TIME_WAIT, "FinAck");

                    dwTickWait = TimerTick() + 2*cfgSockSegmentLifetimeInSeconds*TICKS_PER_SECOND;

                    if (pTcpSocket->dwTickWait == 0 || dwTickWait < pTcpSocket->dwTickWait)
                    {
                        pTcpSocket->dwTickWait = TimerSetRelative(pTcpSocket->GetTimer(), dwTickWait - TimerTick());
                    }
                    return NULL;

                case TCPST_LAST_ACK:
                    SockClose(pTcpSocket, TRUE);
                    return NULL;
                }
            }
        }
    }

    // If we already receive FIN from the peer,
    // we'll just ignore incoming data.
    if (pTcpSocket->IsFinReceived() || cbData == 0)
        return(pTcpSocket);

    pTcpSocket->delayedAcks += cbData;

    // Quick check to see if the data from incoming segment
    // fits entirely within the current receive window.
    if (seq != pTcpSocket->rcv_nxt || cbData > pTcpSocket->rcv_wnd)
    {
        // If the receive sequence number is outside of
        // the current receive window, send an ACK right away.
        if (!TcpValidateSeqs(pTcpSocket, seq, seq+cbData, &seq0, &seq1))
        {
            TraceSz4(tcpStat, "[%X.t] Sequence number out-of-range: %08lX %08lX %d",
                     pTcpSocket, seq, pTcpSocket->rcv_nxt, pTcpSocket->rcv_wnd);
            NeedSendACKNow(pTcpSocket);
            return pTcpSocket;
        }

        pbData += (seq0 - seq);
        cbData = (seq1 - seq0);
        Assert((INT)cbData > 0);
        seq = seq0;
    }

    if (pTcpSocket->TestFlags(SOCKF_NOMORE_RECV))
    {
        TcpResetPeer(pTcpSocket);
        TcpReset(pTcpSocket, NETERR_CONNRESET);
        return NULL;
    }

    // Special fast path: there is a pending overlapped receive request
    // and there is no data in the receive buffer.
    if (    pTcpSocket->HasRecvReq()
        &&  seq == pTcpSocket->rcv_nxt
        &&  !pTcpSocket->HasRecvBuf())
    {
        UINT copied = TcpCompletePendingRecvReqFast(pTcpSocket, tcphdr->_bFlags, pbData, cbData);
        if (copied == cbData) return pTcpSocket;

        pbData += copied;
        cbData -= copied;
        seq += copied;
    }

    // Make a copy of the incoming segment and buffer it up
    recvbuf = (CTcpRecvBuf*) PoolAlloc(sizeof(CTcpRecvBuf) + cbData, PTAG_CTcpRecvBuf);

    if (!recvbuf)
    {
        TraceSz2(tcpWarn, "[%X.t] Out of memory allocating receive buffer.  %ld bytes lost.",
                 pTcpSocket, cbData);
        return(pTcpSocket);
    }

    recvbuf->Init(this);

    recvbuf->seqnext = seq + cbData;
    recvbuf->dataoffset = sizeof(CTcpRecvBuf);
    recvbuf->tcpflags = tcphdr->_bFlags;
    recvbuf->SetCbBuf(cbData);
    memcpy(recvbuf+1, pbData, cbData);

    // Fast case: the receive segment is in sequence
    if (seq == pTcpSocket->rcv_nxt)
    {
        CTcpRecvBuf* lastbuf = (CTcpRecvBuf *)pTcpSocket->GetRecvBufLast();
        UINT datarun;

        if (lastbuf == NULL || lastbuf->seqnext == seq)
        {
            // We haven't buffered any out-of-order segments behind this one.
            datarun = cbData;
            pTcpSocket->EnqueueRecvBuf(recvbuf);
        }
        else
        {
            // Figure out if this segment fills out holes
            // in the receive buffer.
            Assert(SEQ_LT(seq, lastbuf->seqnext));
            datarun = TcpCorrectMisorderedSegments(pTcpSocket, seq, recvbuf);
        }

        pTcpSocket->SetCbRecvBuf(pTcpSocket->GetCbRecvBuf() + datarun);
        TcpSlideRecvWindow(pTcpSocket, datarun);

        if (pTcpSocket->HasRecvReq())
        {
            // If we have a pending overlapped receive request, satisfy it now
            CRecvReq* pRecvReq = pTcpSocket->GetRecvReq();
            pRecvReq->_pWsaOverlapped->_ioxfercnt += TcpCopyRecvData(pTcpSocket, pRecvReq);

            if (pRecvReq->buflen == 0)
            {
                SockReqComplete(pTcpSocket, pRecvReq, NETERR_OK);
            }

            TcpSlideRecvWindow(pTcpSocket, 0);
        }
        else
        {
            pTcpSocket->SignalEvent(SOCKF_EVENT_READ);
        }
    }
    else
    {
        // Slow case: a segment arrived out of order.
        // Buffer up this segment for later processing.
        TraceSz3(tcpStat, "[%X.t] Out-of-order segment: %08lX %08lX", pTcpSocket, SEG_SEQ(tcphdr), pTcpSocket->rcv_nxt);
        NeedSendACKNow(pTcpSocket);
        TcpBufferMisorderedSegment(pTcpSocket, seq, recvbuf);
    }

    return(pTcpSocket);
}


void CXnSock::TcpProcessFIN(CTcpSocket* pTcpSocket, CTcpHdr* tcphdr, TCPSEQ seqfin)

/*++

Routine Description:

    Process an incoming TCP FIN segment

Arguments:

    pTcpSocket - Points to the TCP control block
    tcphdr - Points to the TCP segment header
    seqfin - Sequence number for the FIN

Return Value:

    NONE

--*/

{
    DWORD dwTickWait;

    if (seqfin != pTcpSocket->rcv_nxt)
        return;

    pTcpSocket->rcv_nxt = seqfin+1;
    NeedSendACKNow(pTcpSocket);
    pTcpSocket->SignalEvent(SOCKF_EVENT_CLOSE);

    switch (pTcpSocket->GetState()) {
    case TCPST_SYN_RECEIVED:
    case TCPST_ESTABLISHED:
        SockReqComplete(pTcpSocket, pTcpSocket->GetRecvReq(), NETERR_OK);
        pTcpSocket->SignalEvent(SOCKF_EVENT_READ);
        TcpSetState(pTcpSocket, TCPST_CLOSE_WAIT, "FinRecv");
        break;

    case TCPST_FIN_WAIT_1:
        if (pTcpSocket->IsSendBufEmpty()) {
            TcpSetState(pTcpSocket, TCPST_CLOSING, "FinRecv");
            break;
        }

        // If our FIN has been acknowledged,
        // fall through and change to TIME-WAIT state.

    case TCPST_FIN_WAIT_2:
        TcpSetState(pTcpSocket, TCPST_TIME_WAIT, "FinRecv");

        // Fall through

    case TCPST_TIME_WAIT:
        dwTickWait = TimerTick() + 2*cfgSockSegmentLifetimeInSeconds*TICKS_PER_SECOND;

        if (pTcpSocket->dwTickWait == 0  || dwTickWait < pTcpSocket->dwTickWait)
        {
            pTcpSocket->dwTickWait = TimerSetRelative(pTcpSocket->GetTimer(), dwTickWait - TimerTick());
        }
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

BOOL CXnSock::TcpParseOptions(CTcpSocket * pTcpSocket, CTcpHdr * tcphdr, struct _TcpOptions * opts)

/*++

Routine Description:

    Parse the option information in an incoming TCP SYN segment

Arguments:

    tcphdr - Points to the TCP segment header
    opts - Returns the parsed option information

Return Value:

    TRUE if successful, FALSE if the TCP options are not well-formed

--*/

{
    const BYTE* buf;
    UINT buflen;

    // We ignore any data that's sent in the initial SYN segment.
    // Not sure if this case actually happens in real-life.
    // In any case, the sender should retransmit the data.
#if 0
    if (ISTCPSEG(tcphdr, SYN) && cbData > 0) {
        TraceSz(Warning, "Ignoring data in SYN segment.");
    }
#endif

    // Use default values if no options are present
    opts->mss = TCP_DEFAULT_MSS;

    buflen = tcphdr->GetHdrLen() - sizeof(CTcpHdr);
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
            TraceSz1(tcpStat, "TCP option ignored: %d", opt);
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
#define TcpSaveConnReqParams(_pTcpSocket, _tcphdr, _opts) { \
            (_pTcpSocket)->rcv_isn = SEG_SEQ(_tcphdr); \
            (_pTcpSocket)->rcv_nxt = (_pTcpSocket)->rcv_isn+1; \
            (_pTcpSocket)->snd_mss = (_opts)->mss; \
        }

BOOL CXnSock::TcpAcceptConnReqPassive(CTcpSocket* pTcpSocket, CTcpHdr* tcphdr, CIpAddr ipaSrc)

/*++

Routine Description:

    Process an incoming TCP connection request
    that was made to a listening (passively opened) socket

Arguments:

    pTcpSocket - TCP control block
    tcphdr - Points to the TCP segment header

Return Value:

    FALSE if the incoming connection request is bad and
    the caller should send out a RST in response; TRUE otherwise

--*/

{
    NTSTATUS status;
    CTcpSocket* pTcpSocketChild;
    TcpOptions opts;

    // Parse TCP options
    if (!TcpParseOptions(pTcpSocket, tcphdr, &opts)) return FALSE;

    pTcpSocketChild = TcpCloneChild(pTcpSocket);
    if (!pTcpSocketChild) return TRUE;

    TcpSetState(pTcpSocketChild, TCPST_SYN_RECEIVED, "Connection request");
    TcpSaveConnReqParams(pTcpSocketChild, tcphdr, &opts);

    status = TcpConnect(pTcpSocketChild, ipaSrc, tcphdr->_ipportSrc, TRUE);
    if (!NT_SUCCESS(status)) {
        SockClose(pTcpSocketChild, TRUE);
        return TRUE;
    }

    pTcpSocketChild->Enqueue(&_leSockets);

    return(TRUE);
}


void CXnSock::TcpConnectionEstablished(CTcpSocket* pTcpSocket, CTcpHdr* tcphdr)

/*++

Routine Description:

    Change a TCP connection to established state

Arguments:

    pTcpSocket - Points to the TCP control block
    tcphdr - Points to the incoming SYN/ACK segment

Return Value:

    NONE

--*/

{
    TCPSEQ ack;
    CTcpSendBuf* pTcpSendBuf;

    TraceSz4(tcpStat, "[%X.t] Connection established on %d to %s:%d ", pTcpSocket,
             NTOHS(pTcpSocket->_ipportSrc), pTcpSocket->_ipaDst.Str(),
             NTOHS(pTcpSocket->_ipportDst));

    //
    // The incoming segment must acknowledge our SYN
    //
    pTcpSendBuf = (CTcpSendBuf *)pTcpSocket->GetSendBufFirst();
    ack = SEG_ACK(tcphdr);
    Assert(pTcpSendBuf->tcpflags & TCP_SYN);
    Assert(ISTCPSEG(tcphdr, ACK));
    Assert(ack == pTcpSocket->snd_nxt);

    pTcpSocket->snd_una = ack;
    TcpUpdateSndWnd(pTcpSocket, SEG_WIN(tcphdr), SEG_SEQ(tcphdr), ack);

    if (pTcpSocket->rtt_tick)
    {
        TcpUpdateRTO(pTcpSocket);
    }

    pTcpSocket->DequeueSendBuf(pTcpSendBuf);
    SockReleaseSendBuf(pTcpSendBuf);

    pTcpSocket->dwTickXmit = pTcpSocket->dwTickSyn = 0;

    // NOTE: Since we don't fragment outgoing IP datagrams, we need
    // to limit snd_mss to be less than the first-hop interface MTU
    // minus the TCP and IP headers.
    if (pTcpSocket->snd_mss > pTcpSocket->rcv_mss)
        pTcpSocket->snd_mss = pTcpSocket->rcv_mss;

    pTcpSocket->snd_cwnd = 2*pTcpSocket->snd_mss;
    pTcpSocket->snd_ssthresh = max(pTcpSocket->snd_wnd, pTcpSocket->snd_cwnd);
    pTcpSocket->rcv_swsthresh = min(pTcpSocket->_cbMaxSendBuf>>1, pTcpSocket->snd_mss);
    if (pTcpSocket->rcv_swsthresh < 1)
        pTcpSocket->rcv_swsthresh = 1;

    TcpSetState(pTcpSocket, TCPST_ESTABLISHED, "Connected");
    pTcpSocket->SetFlags(SOCKF_CONNECTED);
    pTcpSocket->SignalEvent(SOCKF_EVENT_CONNECT);

    if (pTcpSocket->IsChild())
    {
        // Signal the connection request is ready for acceptance
        pTcpSocket->GetParent()->SignalEvent(SOCKF_EVENT_ACCEPT);
    }
}


BOOL CXnSock::TcpAcceptConnReqActive(CTcpSocket* pTcpSocket, CTcpHdr* tcphdr)

/*++

Routine Description:

    Process an incoming TCP connection request
    that was made to an actively opened socket

Arguments:

    pTcpSocket - TCP control block
    tcphdr - Points to the TCP segment header

Return Value:

    FALSE if the incoming connection request is bad and
    the caller should send out a RST in response; TRUE otherwise

--*/

{
    TcpOptions opts;
    NTSTATUS status;

    // Parse TCP options and save connection request information
    if (!TcpParseOptions(pTcpSocket, tcphdr, &opts)) return FALSE;
    TcpSaveConnReqParams(pTcpSocket, tcphdr, &opts);

    if (ISTCPSEG(tcphdr, ACK)) {
        TcpConnectionEstablished(pTcpSocket, tcphdr);
        NeedSendACKNow(pTcpSocket);
        status = NETERR_OK;
    } else {
        TcpSetState(pTcpSocket, TCPST_SYN_RECEIVED, "Simultaneous open");
        status = TcpEmitSYN(pTcpSocket, TRUE);
    }

    return NT_SUCCESS(status);
}


void CXnSock::TcpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CTcpHdr * pTcpHdr, UINT cbHdrLen, UINT cbData)
{
    CTcpSocket *    pTcpSocket;
    BYTE *          pbData = (BYTE *)pTcpHdr + cbHdrLen;
    BOOL            isAck;
    TCPSEQ          ack;

    pTcpSocket = (CTcpSocket *)SockFindMatch(pTcpHdr->_ipportDst, pIpHdr->_ipaSrc,
                                             pTcpHdr->_ipportSrc, SOCK_STREAM);

#if defined(XNET_FEATURE_ONLINE) && !defined(XNET_FEATURE_INSECURE)
    if (!ppkt->IsEsp() && !ppkt->TestFlags(PKTF_RECV_LOOPBACK) && !pTcpSocket->TestFlags(SOCKF_INSECURE))
    {
        // An insecure, non-loopback TCP packet will only be sent to a socket in the
        // secure online stack if the socket has explicitly allowed insecure packets.

        pTcpSocket = NULL;
    }
#endif

    if (!pTcpSocket)
    {
#ifdef XNET_FEATURE_TRACE
        if (ppkt->IsEsp())
            TraceSz3(pktWarn, "[DISCARD] No TCP socket listening on port %d from %s:%d",
                     NTOHS(pTcpHdr->_ipportDst), pIpHdr->_ipaSrc.Str(), NTOHS(pTcpHdr->_ipportSrc));
        else
            TraceSz3(pktRecv, "[DISCARD] No TCP socket listening on port %d from %s:%d",
                     NTOHS(pTcpHdr->_ipportDst), pIpHdr->_ipaSrc.Str(), NTOHS(pTcpHdr->_ipportSrc));
#endif

#if defined(XNET_FEATURE_ONLINE) && !defined(XNET_FEATURE_INSECURE)
        if (!ppkt->IsEsp())
        {
            // An insecure TCP packet will not be sent a RST because we want to be "stealthy" and
            // not let a PC get any sort of response from us by sending random TCP packets.

            return;
        }
#endif

        goto sendrst;
    }

    TraceTcpHdr(pktRecv, pTcpSocket, pTcpHdr, cbHdrLen - sizeof(CTcpHdr), cbData);

    if (pTcpSocket->IsSyncState() && (pTcpHdr->_bFlags & TCP_CONTROLS) == 0)
    {
        // Common case of receiving data in a synchronized state

        pTcpSocket = TcpRecvData(pTcpSocket, pTcpHdr, pbData, cbData);

        goto rcvdone;
    }

    if (pTcpSocket->GetState() == TCPST_CLOSED)
    {
        TraceSz1(tcpWarn, "[%X.t] Received packet on closed socket.  Sending RST.", pTcpSocket);
        goto sendrst;
    }

    Assert(!pIpHdr->_ipaDst.IsBroadcast());
    Assert(!pIpHdr->_ipaSrc.IsBroadcast());

    isAck = ISTCPSEG(pTcpHdr, ACK);

    switch (pTcpSocket->GetState())
    {
        case TCPST_LISTEN:

            if (ISTCPSEG(pTcpHdr, RST))
            {
                TraceSz1(pktRecv, "[%X.t] Received RST while in LISTEN state.  Packet discarded.",
                         pTcpSocket);
                return;
            }

            if (isAck)
            {
                TraceSz1(tcpWarn, "[%X.t] Received ACK while in LISTEN state.  Sending RST.",
                         pTcpSocket);
                goto sendrst;
            }

            if (!ISTCPSEG(pTcpHdr, SYN))
            {
                TraceSz1(tcpWarn, "[%X.t] Received non-SYN while in LISTEN state.  Packet discarded.",
                         pTcpSocket);
                return;
            }

            // Process an incoming connection request on a listening socket
            if (!TcpAcceptConnReqPassive(pTcpSocket, pTcpHdr, pIpHdr->_ipaSrc))
            {
                TraceSz1(tcpWarn, "[%X.t] Failed to accept connection on listening socket", pTcpSocket);
                goto sendrst;
            }
            break;

        case TCPST_SYN_SENT:

            if (isAck)
            {
                ack = SEG_ACK(pTcpHdr);

                if (!IsValidACK(pTcpSocket, ack))
                {
                    TraceSz1(tcpWarn, "[%X.t] Received bad ACK seq while in SYN_SENT state.  Sending RST.",
                             pTcpSocket);
                    goto sendrst;
                }
            }

            if (ISTCPSEG(pTcpHdr, RST))
            {
                if (isAck)
                {
                    TraceSz1(tcpWarn, "[%X.t] Received RST while in SYN_SENT state.  Resetting connection.",
                             pTcpSocket);
                    TcpReset(pTcpSocket, NETERR_CONNRESET);
                }
                else
                {
                    TraceSz1(tcpWarn, "[%X.t] Ignoring RST without ACK in SYN_SENT state.",
                             pTcpSocket);
                }
                return;
            }

            if (!ISTCPSEG(pTcpHdr, SYN))
            {
                TraceSz1(tcpWarn, "[%X.t] Ignoring non-SYN while in SYN_SENT state.",
                         pTcpSocket);
                return;
            }

            // Simultaneous active open
            if (!TcpAcceptConnReqActive(pTcpSocket, pTcpHdr))
            {
                TraceSz1(tcpWarn, "[%X.t] Failed to accept simultaneous connection", pTcpSocket);
                goto sendrst;
            }
            break;

        default:
        {
            TCPSEQ oldseq0 = SEG_SEQ(pTcpHdr);
            TCPSEQ oldseq1 = oldseq0 + SEG_LEN(pTcpHdr, cbData);
            TCPSEQ seq0, seq1;

            // Make sure the sequence number is correct.
            // If not and the incoming segment is not RST, we'll emit an ACK.
            if (!TcpValidateSeqs(pTcpSocket, oldseq0, oldseq1, &seq0, &seq1))
            {
                if (!ISTCPSEG(pTcpHdr, RST))
                {
                    NeedSendACKNow(pTcpSocket);
                }
                break;
            }

            // If RST is on, then we'll reset the connection:
            //  - if the socket corresponds to a pending connection
            //    request, then we'll just close it and destroy the CTcpSocket.
            if (ISTCPSEG(pTcpHdr, RST))
            {
                TcpReset(pTcpSocket, NETERR_CONNRESET);
                goto discard;
            }

            // If SYN is set, there is an error.
            // We send out a RST as well as reset the connection.
            if (ISTCPSEG(pTcpHdr, SYN))
            {
                if (oldseq0 == seq0)
                {
                    TcpReset(pTcpSocket, NETERR_CONNRESET);
                    goto sendrst;
                }

                goto discard;
            }

            if (!ISTCPSEG(pTcpHdr, ACK))
            {
                goto discard;
            }

            if (pTcpSocket->GetState() == TCPST_SYN_RECEIVED)
            {
                ack = SEG_ACK(pTcpHdr);

                if (!IsValidACK(pTcpSocket, ack))
                {
                    // The acknowledgement number is bad, emit an RST
                    goto sendrst;
                }

                // NOTE: The ACK for our SYN will be processed
                // inside the TcpProcessDataAck call below.
                TcpConnectionEstablished(pTcpSocket, pTcpHdr);
            }

            // Process the data and acknowledgement information
            // and continue to process the FIN flag if necessary
            pTcpSocket = TcpRecvData(pTcpSocket, pTcpHdr, pbData, cbData);

            if (pTcpSocket && ISTCPSEG(pTcpHdr, FIN))
            {
                TcpProcessFIN(pTcpSocket, pTcpHdr, seq1-1);
            }
        }
        break;
    }

rcvdone:

    if (pTcpSocket && pTcpSocket->delayedAcks > 0)
    {
        // If we've delayed more than two maximum packets worth of data,
        // of if the receive window just closed, send an ACK now.

        if (    pTcpSocket->delayedAcks >= (pTcpSocket->rcv_mss << 1)
            ||  pTcpSocket->rcv_wnd == 0)
        {
            TcpEmitACK(pTcpSocket);
        }
        else
        {
            // If we aren't forced to send an ACK now, just queue up the timer and
            // wait for the very next tick.  This will come back anywhere from zero to
            // 200ms from now, depending on how far into a "tick" we happen to be.

            TimerSet(pTcpSocket->GetTimer(), TimerTick());
        }
    }
    return;

sendrst:

    // If the received segment is a RST, do nothing
    if ((pTcpHdr->_bFlags & TCP_RST) == 0) {
        TCPSEQ seq;
        BYTE flags;

        if (ISTCPSEG(pTcpHdr, ACK)) {
            ack = 0;
            seq = SEG_ACK(pTcpHdr);
            flags = 0;
        } else {
            ack = SEG_SEQ(pTcpHdr) + SEG_LEN(pTcpHdr, cbData);
            seq = 0;
            flags  = TCP_ACK;
        }

        TcpEmitRST(pTcpSocket, pIpHdr->_ipaSrc, pTcpHdr->_ipportSrc, pTcpHdr->_ipportDst,
                   seq, ack, flags);
        return;
    }

discard:
    return;
}

#ifdef XNET_FEATURE_TRACE

VOID CXnSock::TcpSetState(CTcpSocket* pTcpSocket, BYTE state, const CHAR* caller)
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

    UINT oldstate = pTcpSocket->GetState();

    pTcpSocket->SetState(state);

    TraceSz7(tcpStat, "[%X.t] %s %d - %s:%d: %s => %s",
             pTcpSocket, caller, NTOHS(pTcpSocket->_ipportSrc),
             pTcpSocket->_ipaDst.Str(), NTOHS(pTcpSocket->_ipportDst),
             stateNames[oldstate], stateNames[state]);
}

#endif

BOOL CXnSock::TcpClose(CTcpSocket* pTcpSocket, BOOL forceful)

/*++

Routine Description:

    Close a TCP connection

Arguments:

    pTcpSocket - Points to the CTcpSocket structure
    forceful - Forceful close, always succeed and no wait

Return Value:

    TRUE if the CTcpSocket structure can be deleted immediately.
    FALSE if the connection is being gracefully closed
    and the CTcpSocket structure should be kept around until
    the connection is really closed.

--*/

{
    ICHECK(SOCK, UDPC|SDPC);

    //
    // Try to gracefully close the socket if requested by the caller
    //
    if (    !forceful
        &&  pTcpSocket->GetState() >= TCPST_SYN_RECEIVED
        &&  (!pTcpSocket->GetLingerOnOff() || pTcpSocket->GetLingerTimeout() > 0))
    {
        NTSTATUS status = TcpShutdown(pTcpSocket, SOCKF_NOMORE_XMIT|SOCKF_NOMORE_RECV, FALSE);

        if (NT_SUCCESS(status))
        {
            DWORD dwLingerSecs = pTcpSocket->GetLingerOnOff() ?
                                   pTcpSocket->GetLingerTimeout() :
                                   cfgSockLingerTimeoutInSeconds;

            // TIME-WAIT and linger timers are the same
            pTcpSocket->dwTickWait = TimerSetRelative(pTcpSocket->GetTimer(),
                dwLingerSecs * TICKS_PER_SECOND);

            return(FALSE);
        }
    }

    if (pTcpSocket->HasParent())
    {
        pTcpSocket->GetParent()->DequeueChild(pTcpSocket);
    }
    else
    {
        TcpTrimBacklog(pTcpSocket, 0);
    }

    TimerSet(pTcpSocket->GetTimer(), TIMER_INFINITE);

    return(TRUE);
}

VOID CXnSock::TcpTrimBacklog(CTcpSocket * pTcpSocket, UINT cBacklogMax)
{
    while (pTcpSocket->GetBacklog() > cBacklogMax)
    {
        CTcpSocket * pTcpSocketChild = pTcpSocket->GetLastChild();
        Assert(pTcpSocketChild != NULL);

        pTcpSocket->DequeueChild(pTcpSocketChild);
        SockClose(pTcpSocketChild, TRUE);
    }
}

NTSTATUS CXnSock::TcpShutdown(CTcpSocket* pTcpSocket, DWORD dwFlags, BOOL apicall)

/*++

Routine Description:

    Gracefully shutdown a TCP connection

Arguments:

    pTcpSocket - Points to the TCP control block
    flags - Shutdown flags: PCBFLAG_SEND_SHUTDOWN and/or PCBFLAG_RECV_SHUTDOWN
    apicall - Whether this is called by shutdown API

Return Value:

    Status code

--*/

{
    NTSTATUS status = NETERR_OK;

    RaiseToDpc();

    if ((dwFlags & SOCKF_NOMORE_RECV) && !pTcpSocket->TestFlags(SOCKF_NOMORE_RECV))
    {
        pTcpSocket->SetFlags(SOCKF_NOMORE_RECV);

        if (!pTcpSocket->IsTcpRecvBufEmpty())
        {
            //
            // If the receive buffer is not empty,
            // we'll send a RST to the connection peer
            // and reset the connection.
            //
            TcpResetPeer(pTcpSocket);
            status = apicall ? NETERR_OK : NETERR_CONNRESET;
            pTcpSocket = TcpReset(pTcpSocket, NETERR_CONNRESET);
            Assert(pTcpSocket != NULL);
        }
        else if (pTcpSocket->HasRecvReq())
        {
            // Cancel any pending overlapped receive requests
            TraceSz1(tcpStat, "[%X.t] TcpShutdown: Cancelling pending recv request", pTcpSocket);
            SockReqComplete(pTcpSocket, pTcpSocket->GetRecvReq(), NETERR(WSAESHUTDOWN));
        }
    }

    if ((dwFlags & SOCKF_NOMORE_XMIT) && !pTcpSocket->TestFlags(SOCKF_NOMORE_XMIT))
    {
        if (pTcpSocket->HasSendReq())
        {
            // Cancel any pending overlapped send requests
            TraceSz1(tcpStat, "[%X.t] TcpShutdown: Cancelling pending send request", pTcpSocket);
            SockReqComplete(pTcpSocket, pTcpSocket->GetSendReq(), NETERR(WSAESHUTDOWN));
        }

        switch (pTcpSocket->GetState()) {
        case TCPST_SYN_RECEIVED:
        case TCPST_ESTABLISHED:
            status = TcpEmitFIN(pTcpSocket);
            if (NT_SUCCESS(status)) {
                TcpSetState(pTcpSocket, TCPST_FIN_WAIT_1, "Shutdown");
                pTcpSocket->SetFlags(SOCKF_NOMORE_XMIT);
            }
            break;

        case TCPST_CLOSE_WAIT:
            status = TcpEmitFIN(pTcpSocket);
            if (NT_SUCCESS(status)) {
                TcpSetState(pTcpSocket, TCPST_LAST_ACK, "Shutdown");
                pTcpSocket->SetFlags(SOCKF_NOMORE_XMIT);
            }
            break;

        default:
            Assert(pTcpSocket->GetState() < TCPST_SYN_RECEIVED);
            status = NETERR(WSAENOTCONN);
            break;
        }
    }

    pTcpSocket->ClearFlags(SOCKF_REVIVABLE);

    return(status);
}

NTSTATUS CXnSock::TcpConnect(CTcpSocket* pTcpSocket, CIpAddr dstaddr, CIpPort dstport, BOOL synAck)

/*++

Routine Description:

    Make a TCP connection to the specified foreign address

Arguments:

    pTcpSocket - Points to the TCP control block
    dstaddr, dstport - Specifies the foreign address to connect to
    synAck - Whether we're acknowledging a received SYN request

Return Value:

    Status code

--*/

{
    NTSTATUS status;

    if (    dstport == 0
        ||  dstaddr == 0
        ||  dstaddr.IsBroadcast()
        ||  dstaddr.IsMulticast()
        || (dstaddr.IsLoopback() && dstaddr != IPADDR_LOOPBACK))
    {
        return(NETERR(WSAEADDRNOTAVAIL));
    }

    if (pTcpSocket->IsListenState())
        return NETERR_PARAM;

    // Bind to a local address if necessary
    if (!pTcpSocket->TestFlags(SOCKF_BOUND))
    {
        status = SockBind(pTcpSocket, 0);
        if (!NT_SUCCESS(status))
            return status;
    }

    RaiseToDpc();

    if (pTcpSocket->TestFlags(SOCKF_REVIVABLE))
    {
        // Revive a CTcpSocket that has been reset
        pTcpSocket->Dequeue();
        pTcpSocket->TcpInit(this);
        pTcpSocket->ClearFlags(SOCKF_CONNECT_SELECTED|SOCKF_REVIVABLE|SOCKF_NOMORE_RECV|SOCKF_NOMORE_XMIT|SOCKF_EVENT_MASK);
        EvtClear(pTcpSocket->GetEvent());
        pTcpSocket->SetStatus(NETERR_OK);
        pTcpSocket->Enqueue(&_leSockets);
    }
    else
    {
        status = pTcpSocket->GetStatus();
        if (!NT_SUCCESS(status))
            return(status);
    }

    if (!pTcpSocket->IsIdleState() && !synAck)
    {
        return(NETERR(WSAEALREADY));
    }

    // If this is an active open, go to SYN-SENT state.
    // Otherwise, we should already be in SYN-RECEIVED state.
    if (!synAck)
    {
        TcpSetState(pTcpSocket, TCPST_SYN_SENT, "Connect");
    }

    // Set up MSS to make sure we don't have to
    // fragment on the first hop. We assume the first hop
    // interface won't change during the life of the connection.
    pTcpSocket->rcv_mss = TCP_MAXIMUM_MSS;
    pTcpSocket->_cbMaxRecvBuf = 
        ((pTcpSocket->_cbMaxRecvBuf + pTcpSocket->rcv_mss - 1) / pTcpSocket->rcv_mss) * pTcpSocket->rcv_mss;
    if (pTcpSocket->_cbMaxRecvBuf > (UINT)(cfgSockMaxRecvBufsizeInK * 1024))
        pTcpSocket->_cbMaxRecvBuf = (UINT)(cfgSockMaxRecvBufsizeInK * 1024);

    pTcpSocket->_ipaDst = dstaddr;
    pTcpSocket->_ipportDst = dstport;

    // Get the initial sequence number for a new TCP connection.

    LARGE_INTEGER time;
    HalQueryTsc(&time);
    pTcpSocket->snd_isn = (TCPSEQ)time.QuadPart;

    pTcpSocket->rcv_wnd = pTcpSocket->_cbMaxRecvBuf;
    Assert(pTcpSocket->IsTcpRecvBufEmpty());

    // send out a SYN or SYN/ACK segment
    return(TcpEmitSYN(pTcpSocket, synAck));
}

NTSTATUS CXnSock::TcpListen(CTcpSocket* pTcpSocket, INT cBacklog)

/*++

Routine Description:

    Switch a TCP socket into listening state

Arguments:

    pTcpSocket - Points to the TCP control block
    backlog - Maximum number of pending connections

Return Value:

    Status code

--*/

{
    if (!pTcpSocket->IsIdleState())
    {
        return(NETERR(WSAEISCONN));
    }
    
    RaiseToDpc();

    NTSTATUS status = pTcpSocket->GetStatus();
    if (!NT_SUCCESS(status))
        return(status);

    TcpSetState(pTcpSocket, TCPST_LISTEN, "Listen");

    if (cBacklog < 1)
        cBacklog = 1;
    else if (cBacklog > cfgSockMaxListenBacklog)
        cBacklog = cfgSockMaxListenBacklog;

    pTcpSocket->SetBacklogMax(cBacklog);

    TcpTrimBacklog(pTcpSocket, cBacklog);

    return(NETERR_OK);
}

// ---------------------------------------------------------------------------------------
// TcpRead
// ---------------------------------------------------------------------------------------

NTSTATUS CXnSock::TcpRead(CTcpSocket* pTcpSocket, CRecvReq* pRecvReq)

/*++

Routine Description:

    Handle a user request to receive data from a TCP socket

Arguments:

    pTcpSocket - Points to the TCP control block
    pRecvReq - Points to the receive request information

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    UINT oldrcvwnd;

    RaiseToDpc();

    // Only segments from the connection peer are accepted
    pRecvReq->SetFromAddrPort(pTcpSocket->_ipaDst, pTcpSocket->_ipportDst);
    SecRegSetOwned(pTcpSocket->_ipaDst);
    *pRecvReq->bytesRecv = 0;

    if (!pTcpSocket->IsTcpRecvBufEmpty()) {

        // If the receive buffer is not empty, return as much data
        // as possible from the receive buffer.

        (*pRecvReq->bytesRecv) += TcpCopyRecvData(pTcpSocket, pRecvReq);
        status = NETERR_OK;

        // If the receive window was closed and there is
        // now enough free spce in the receive buffer,
        // then we'll send out a gratuitous ACK here.

        oldrcvwnd = pTcpSocket->rcv_wnd;
        TcpSlideRecvWindow(pTcpSocket, 0);
        if (oldrcvwnd < pTcpSocket->rcv_swsthresh &&
            pTcpSocket->rcv_wnd >= pTcpSocket->rcv_swsthresh) {
            TcpEmitACK(pTcpSocket);
        }
    } else if (pTcpSocket->IsFinReceived()) {
        // The connection has been gracefully closed
        status = NETERR_OK;
    } else {
        status = pTcpSocket->GetStatus();
        if (NT_SUCCESS(status)) {
            status = RecvReqEnqueue(pTcpSocket, pRecvReq);
        }
    }

    return(status);
}

//
// Copy data from the CTcpSocket receive buffer to the user's receive buffer
//
UINT CXnSock::TcpCopyRecvData(CTcpSocket* pTcpSocket, CRecvReq* pRecvReq)
{
    CTcpRecvBuf* recvbuf;
    UINT copycnt, total = 0;
    BYTE tcpflags = 0;

    do {
        recvbuf = (CTcpRecvBuf *) pTcpSocket->GetRecvBufFirst();
        Assert(SEQ_LE(recvbuf->seqnext, pTcpSocket->rcv_nxt));

        copycnt = min(recvbuf->GetCbBuf(), pRecvReq->buflen);
        memcpy(pRecvReq->buf, (BYTE*) recvbuf + recvbuf->dataoffset, copycnt);
        total += copycnt;
        pTcpSocket->SetCbRecvBuf(pTcpSocket->GetCbRecvBuf() - copycnt);
        pRecvReq->buf += copycnt;
        pRecvReq->buflen -= copycnt;

        tcpflags |= recvbuf->tcpflags;
        if (copycnt == recvbuf->GetCbBuf())
        {
            pTcpSocket->DequeueRecvBuf(recvbuf);
            PoolFree(recvbuf);
        }
        else
        {
            recvbuf->dataoffset = (WORD) (recvbuf->dataoffset + copycnt);
            recvbuf->DecCbBuf(copycnt);
        }
    } while (pRecvReq->buflen && !pTcpSocket->IsTcpRecvBufEmpty());

    if (tcpflags & (TCP_PSH|TCP_URG))
        pRecvReq->buflen = 0;

    return total;
}

// ---------------------------------------------------------------------------------------
// TcpSend
// ---------------------------------------------------------------------------------------

NTSTATUS CXnSock::TcpSend(CTcpSocket* pTcpSocket, CSendReq * pSendReq, UINT uiFlags)

/*++

Routine Description:

    Handle a user request to send data from a TCP socket

Arguments:

    pTcpSocket - Points to the TCP control block
    pSendReq - Points to the send request information

Return Value:

    Status code

Note:

    This function always return immediately. For blocking operation,
    we assume the caller has taken care of any necessary wait.

--*/

{
    NTSTATUS status = NETERR_OK;
    CPacket* pkt = NULL;
    WSABUF* bufs = pSendReq->bufs;
    UINT bytesToSend = pSendReq->sendtotal;
    UINT mss = pTcpSocket->snd_mss;
    UINT n, datalen;
    BYTE* data;

    // Make sure we don't overflow the send buffer too much.
    // In the following case:
    //  old pTcpSendBuf size + the datagram size > max pTcpSendBuf size
    // we'll temporarily exceed the send buffer size limit a little bit.
    Assert(!pTcpSocket->IsSendBufFull());
    Assert(pTcpSocket->_cbMaxSendBuf >= pTcpSocket->GetCbSendBuf());
    n = pTcpSocket->_cbMaxSendBuf - pTcpSocket->GetCbSendBuf();
    if (bytesToSend > max(n, mss))
        pSendReq->sendtotal = bytesToSend = mss;

    data = (BYTE *)bufs->buf;
    datalen = bufs->len;

    while (bytesToSend > 0)
    {
        UINT segsize = min(mss, bytesToSend);
        BYTE* p;

        pkt = TcpPacketAlloc(pTcpSocket, pTcpSocket->_ipaDst, uiFlags, segsize, sizeof(CTcpSendBuf), (PFNPKTFREE)TcpPacketFree);

        if (!pkt)
        {
            if ((pSendReq->sendtotal -= bytesToSend) == 0)
            {
                // If we've already send out some data,
                // then return success with partial byte count.
                // Otherwise, return error code.
                status = NETERR_MEMORY;
            }
            break;
        }

        bytesToSend -= segsize;
        p = (BYTE *)pkt->GetTcpHdr() + sizeof(CTcpHdr);
        if (segsize <= datalen)
        {
            // Fast case: all the data for this segment
            // comes from the same user buffer.
            memcpy(p, data, segsize);
            data += segsize;
            datalen -= segsize;
        }
        else
        {
            // Slow case: need to gather data for this segment
            // from multiple user buffers.
            
            UINT segremain = segsize;

            while (segremain)
            {
                while (datalen == 0)
                {
                    bufs++;
                    data = (BYTE *)bufs->buf;
                    datalen = bufs->len;
                }
                n = min(segremain, datalen);
                memcpy(p, data, n);
                p += n;
                data += n;
                datalen -= n;
                segremain -= n;
            }
        }

        {
            RaiseToDpc();

            status = pTcpSocket->GetStatus();

            if (NT_SUCCESS(status))
            {
                TcpQueueSegment(pTcpSocket, pkt, (BYTE)(bytesToSend ? TCP_ACK : (TCP_ACK|TCP_PSH)),
                                segsize);
            }
            else
            {
                PacketFree(pkt);
                break;
            }
        }
    }

    // If the app calls connect() on a non-blocking socket, and then calls send()
    // without calling select(), then we need to reset the connectSelected flag
    // so future select() calls will ignore SOCKF_EVENT_CONNECT.

    pTcpSocket->SetFlags(SOCKF_CONNECT_SELECTED);

    return(status);
}

CPacket * CXnSock::TcpPacketAlloc(CTcpSocket * pTcpSocket, CIpAddr ipaDst, UINT uiFlags, UINT cbDat, UINT cbPkt, PFNPKTFREE pfn)
{
    Assert((uiFlags & ~PKTF_POOLALLOC) == 0);

    uiFlags |= PKTF_TYPE_TCP;

    if (ipaDst.IsSecure())
    {
        uiFlags |= PKTF_TYPE_ESP|PKTF_CRYPT;
    }
    else
    {
    #ifdef XNET_FEATURE_ONLINE
        if (pTcpSocket && pTcpSocket->TestFlags(SOCKF_INSECURE))
        {
            uiFlags |= PKTF_XMIT_INSECURE;
        }
    #endif
    }

    return(PacketAlloc(PTAG_CTcpPacket, uiFlags, cbDat, cbPkt, pfn));
}

void CXnSock::TcpPacketFree(CPacket * ppkt)
{
    ICHECK(SOCK, UDPC|SDPC);

    if (((CTcpSendBuf *)ppkt)->Release() == 0)
    {
        PacketFree(ppkt);
    }
}

void CXnSock::TcpQueueSegment(CTcpSocket* pTcpSocket, CPacket* pkt, BYTE tcpflags, UINT datalen)

/*++

Routine Description:

    Initialize the header information for an outgoing TCP packet

Arguments:

    pTcpSocket - Points to the TCP control block
    pkt - Points to the outgoing TCP packet
    tcpflags - Specifies the TCP segment flags

Return Value:

    NONE

--*/

{
    TCHECK(UDPC|SDPC);

    CTcpSendBuf* pTcpSendBuf = (CTcpSendBuf *)pkt;
    pTcpSendBuf->Init(pTcpSocket, 0, 1);
    pTcpSendBuf->retries = 0;
    pTcpSendBuf->tcpflags = tcpflags;
    pTcpSendBuf->tcphdrlen = (BYTE) ((tcpflags & TCP_SYN) ?
                                    sizeof(CTcpHdr)+SYNTCPOPTLEN :
                                    sizeof(CTcpHdr));
    pTcpSendBuf->seq = pTcpSocket->snd_end;

    if (tcpflags & (TCP_SYN|TCP_FIN))
    {
        Assert(datalen == 0);
        pTcpSendBuf->seqnext = pTcpSendBuf->seq + 1;
    }
    else
    {
        pTcpSendBuf->SetCbBuf(datalen);
        pTcpSendBuf->seqnext = pTcpSendBuf->seq + datalen;
    }

    pTcpSocket->snd_end = pTcpSendBuf->seqnext;

    pTcpSocket->IncCbSendBuf(pTcpSendBuf->GetCbBuf());
    pTcpSocket->EnqueueSendBuf(pTcpSendBuf);

    if (pTcpSocket->GetSendBufPtr() == NULL)
    {
        pTcpSocket->SetSendBufPtr(pTcpSendBuf);
        TcpStartOutput(pTcpSocket);
    }
}

BOOL CXnSock::TcpStartOutput(CTcpSocket* pTcpSocket)

/*++

Routine Description:

    Called when we have buffered data to send on a TCP socket. 

Arguments:

    pTcpSocket - Points to the TCP control block
    pTcpSendBuf - Specifies the segment to be sent

Return Value:

    TRUE if a segment was sent; FALSE otherwise.

--*/

{
    TCHECK(UDPC|SDPC);

    INT sndwndAvail;
    CTcpSendBuf* pTcpSendBuf;
    TCPSEQ cwndmax, swndmax;

    pTcpSendBuf = pTcpSocket->GetSendBufPtr();
    Assert(pTcpSocket->snd_nxt == pTcpSendBuf->seq);
    Assert(pTcpSendBuf->retries == 0);

    // We always send SYN or FIN segments without delay
    if (pTcpSendBuf->GetCbBuf() == 0)
        goto sendnow;

    // NOTE: We're using signed integer here in case
    // the send window was shrunk by the receiver.
    swndmax = pTcpSocket->snd_wl2 + pTcpSocket->snd_wnd;
    cwndmax = pTcpSocket->snd_una + pTcpSocket->snd_cwnd;
    sndwndAvail = (SEQ_LE(swndmax, cwndmax) ? swndmax : cwndmax) - pTcpSocket->snd_nxt;

    if ((INT) pTcpSendBuf->GetCbBuf() <= sndwndAvail) {
        // Send window is large enough. Perform send-side
        // SWS avoidance. Note that if the PSH flag is not set,
        // then we must be sending a full-sized data segment.
        // Check the logic in TcpSend().

        // BUGBUG: Since we don't yet combine data from
        // multiple send calls into a single segment, it's
        // meaningless to delay the send.
        goto sendnow;
    }

    // Cannot send due to window constraints
    //  set up persist timers
    if (pTcpSocket->snd_nxt == pTcpSocket->snd_una)
    {
        TraceSz1(tcpStat, "[%X.t] Setting persist timer...", pTcpSocket);
        pTcpSocket->SetFlags(SOCKF_PERSIST);
        pTcpSocket->dwTickXmit = TimerSetRelative(pTcpSocket->GetTimer(), max(pTcpSocket->RTO, (UINT)(cfgSockPersistTimeoutInSeconds * TICKS_PER_SECOND)));
    }
    return FALSE;

sendnow:

    // Use this segment to measure round-trip time if possible
    pTcpSocket->ClearFlags(SOCKF_PERSIST);
    if (pTcpSocket->rtt_tick == 0) {
        pTcpSocket->rtt_tick = TimerTick();
        pTcpSocket->rtt_seq = pTcpSendBuf->seq;
    }
    
    pTcpSendBuf->firstSendTime = TimerTick();
    TcpSendSegment(pTcpSocket, pTcpSendBuf, pTcpSendBuf->retries != 0);
    pTcpSocket->snd_nxt = pTcpSendBuf->seqnext;
    pTcpSocket->SetSendBufPtr((CTcpSendBuf *)pTcpSocket->GetSendBufNext(pTcpSendBuf));
    return(TRUE);
}

void CXnSock::TcpSendSegment(CTcpSocket* pTcpSocket, CTcpSendBuf* pTcpSendBuf, BOOL fRetransmit)
{
    if (!pTcpSendBuf->IsActive())
    {
        CPacket * ppkt = pTcpSendBuf;

        pTcpSocket->delayedAcks = 0;

        Assert(     (pTcpSendBuf->tcpflags & TCP_FIN)
               ||   pTcpSocket->TestFlags(SOCKF_PERSIST)
               ||   SEQ_LE(pTcpSendBuf->seq, TcpSndNxt(pTcpSocket)));

        // If this packet is encrypted, we need to make sure it decrypts properly before
        // attempting to retransmit.  If it fails for any reason, just continue without
        // retransmitting and allow the timeouts to reset the socket.

        if (    !fRetransmit
            ||  !ppkt->IsEsp()
            ||  !ppkt->IsCrypt()
            ||  IpDecrypt(ppkt, pTcpSocket->_ipaDst))
        {
            CTcpHdr * pTcphdr   = ppkt->GetTcpHdr();
            pTcphdr->SetHdrLen(pTcpSendBuf->tcphdrlen);
            pTcphdr->_ipportSrc = pTcpSocket->_ipportSrc;
            pTcphdr->_ipportDst = pTcpSocket->_ipportDst;
            pTcphdr->_dwSeqNum  = HTONL(pTcpSendBuf->seq),
            pTcphdr->_dwAckNum  = HTONL(pTcpSocket->rcv_nxt);
            pTcphdr->_bFlags    = (BYTE)pTcpSendBuf->tcpflags;
            pTcphdr->_wWindow   = HTONS((WORD)pTcpSocket->rcv_wnd);
            pTcphdr->_wUrgent   = 0;
            pTcphdr->_wChecksum = 0;

            pTcpSendBuf->AddRef();

            TraceTcpHdr(pktXmit, pTcpSocket, pTcphdr, pTcpSendBuf->tcphdrlen - sizeof(CTcpHdr), pTcpSendBuf->GetCbBuf());
            IpFillAndXmit(ppkt, pTcpSocket->_ipaDst, IPPROTOCOL_TCP, &pTcpSocket->_prte);
        }
    }

    if (pTcpSendBuf == pTcpSocket->GetSendBufFirst())
    {
        // Set up the retransmisssion or persist timer

        UINT minTimeout = pTcpSocket->TestFlags(SOCKF_PERSIST) ?
                            cfgSockPersistTimeoutInSeconds :
                            cfgSockRexmitMinTimeoutInSeconds;

        minTimeout *= TICKS_PER_SECOND;

        pTcpSocket->dwTickXmit = (pTcpSocket->RTO << pTcpSendBuf->retries);
        if (pTcpSocket->dwTickXmit < minTimeout)
            pTcpSocket->dwTickXmit = minTimeout;
        if (pTcpSocket->dwTickXmit > (UINT)cfgSockRexmitMaxTimeoutInSeconds * TICKS_PER_SECOND)
            pTcpSocket->dwTickXmit = (UINT)cfgSockRexmitMaxTimeoutInSeconds * TICKS_PER_SECOND;

        pTcpSocket->dwTickXmit = TimerSetRelative(pTcpSocket->GetTimer(), pTcpSocket->dwTickXmit);
    }

    pTcpSendBuf->retries++;
}

// ---------------------------------------------------------------------------------------
// CTcpSocket
// ---------------------------------------------------------------------------------------

void CTcpSocket::EnqueueChild(CTcpSocket * pTcpSocketChild)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(_pTcpSocketParent == NULL);
    Assert(pTcpSocketChild->_pTcpSocketParent == NULL);

    _bBacklog += 1;
    pTcpSocketChild->_pTcpSocketParent = this;

    AssertList(&_le);
    InsertTailList(&_le, &pTcpSocketChild->_le);

    // Copy the local address bindings
    pTcpSocketChild->SetFlags(GetFlags() | SOCKF_BOUND, SOCKF_OPT_MASK | SOCKF_BOUND);
    pTcpSocketChild->_ipportSrc      = _ipportSrc;
    pTcpSocketChild->_uiSendTimeout  = _uiSendTimeout;
    pTcpSocketChild->_uiRecvTimeout  = _uiRecvTimeout;
    pTcpSocketChild->_cbMaxSendBuf   = _cbMaxSendBuf;
    pTcpSocketChild->_cbMaxRecvBuf   = _cbMaxRecvBuf;
    pTcpSocketChild->_Linger         = _Linger;
}

void CTcpSocket::DequeueChild(CTcpSocket * pTcpSocketChild)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);

    Assert(_pTcpSocketParent == NULL);
    Assert(pTcpSocketChild->_pTcpSocketParent == this);

    AssertListEntry(&_le, &pTcpSocketChild->_le);
    RemoveEntryList(&pTcpSocketChild->_le);
    InitializeListHead(&pTcpSocketChild->_le);
    pTcpSocketChild->_pTcpSocketParent = NULL;

    Assert(_bBacklog > 0);
    _bBacklog -= 1;
}

CTcpSocket * CTcpSocket::GetConnectedChild()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    
    Assert(_pTcpSocketParent == NULL);

    CTcpSocket * pTcpSocketChild = GetFirstChild();
    while (pTcpSocketChild && !pTcpSocketChild->TestFlags(SOCKF_CONNECTED))
        pTcpSocketChild = GetNextChild(pTcpSocketChild);

    return(pTcpSocketChild);
}

CTcpSocket * CTcpSocket::DequeueConnectedChild()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);

    CTcpSocket * pTcpSocketChild = GetConnectedChild();

    if (pTcpSocketChild)
    {
        DequeueChild(pTcpSocketChild);
    }

    return(pTcpSocketChild);
}

void CSocket::InsertRecvBuf(CRecvBuf * pRecvBuf, CRecvBuf * pRecvBufPrev)
{
    ICHECK_(GetXnBase(), SOCK, SDPC);
    Assert(offsetof(CRecvBuf, _le) == 0);
    Assert(pRecvBuf != NULL);

    AssertList(&_leRecvBufs);

    if (pRecvBufPrev == NULL)
    {
        InsertHeadList(&_leRecvBufs, &pRecvBuf->_le);
    }
    else
    {
        pRecvBuf->_le.Flink = pRecvBufPrev->_le.Flink;
        pRecvBuf->_le.Blink = &pRecvBufPrev->_le;
        pRecvBufPrev->_le.Flink->Blink = &pRecvBuf->_le;
        pRecvBufPrev->_le.Flink = &pRecvBuf->_le;
        AssertList(&_leRecvBufs);
    }
}

