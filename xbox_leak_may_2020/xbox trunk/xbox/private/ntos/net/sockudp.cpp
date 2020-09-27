// ---------------------------------------------------------------------------------------
// sockudp.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

// ---------------------------------------------------------------------------------------
// Trace Tags
// ---------------------------------------------------------------------------------------

DefineTag(udpWarn, TAG_ENABLE);

// ---------------------------------------------------------------------------------------
// UdpShutdown
// ---------------------------------------------------------------------------------------

NTSTATUS CXnSock::UdpShutdown(CSocket* pSocket, DWORD dwFlags)
{
    RaiseToDpc();

    if (dwFlags & SOCKF_NOMORE_RECV)
    {
        SockReqComplete(pSocket, pSocket->GetRecvReq(), NETERR(WSAESHUTDOWN));
        SockFlushRecvBuffers(pSocket);
    }

    if (dwFlags & SOCKF_NOMORE_XMIT)
    {
        SockReqComplete(pSocket, pSocket->GetSendReq(), NETERR(WSAESHUTDOWN));
    }

    pSocket->SetFlags(dwFlags);

    return(NETERR_OK);
}

NTSTATUS CXnSock::UdpConnect(CSocket* pSocket, CIpAddr dstaddr, CIpPort dstport)
{
    if (    dstaddr == 0 && dstport != 0
        ||  dstaddr != 0 && dstport == 0
        ||  dstaddr.IsMulticast()
        || (dstaddr.IsLoopback() && dstaddr != IPADDR_LOOPBACK))
    {
        return(NETERR(WSAEADDRNOTAVAIL));
    }

    // Destination hasn't changed, no need to do anything
    if (dstaddr == pSocket->_ipaDst && dstport == pSocket->_ipportDst)
    {
        return(NETERR_OK);
    }

    // Is this socket allowed to send broadcast
    // datagrams on this socket?
    if (dstaddr.IsBroadcast() && !pSocket->TestFlags(SOCKF_OPT_BROADCAST))
    {
        return(NETERR(WSAEACCES));
    }

    RaiseToDpc();

    // If the socket is currently connected,
    // we need to disconnect it first.
    if (pSocket->TestFlags(SOCKF_CONNECTED))
    {
        pSocket->_ipaDst = 0;
        pSocket->_ipportDst = 0;

        if (pSocket->_prte)
        {
            RouteRelease(pSocket->_prte);
            pSocket->_prte = NULL;
        }

        pSocket->ClearFlags(SOCKF_CONNECTED);
    }

    // Discard any received packets that have been
    // queued up but not yet processed
    SockFlushRecvBuffers(pSocket);

    if (dstaddr != 0)
    {
        // Bind to a local address if necessary
        if (!pSocket->TestFlags(SOCKF_BOUND))
        {
            NTSTATUS status = SockBind(pSocket, 0);
            if (!NT_SUCCESS(status))
                return(status);
        }

        pSocket->_ipaDst = dstaddr;
        pSocket->_ipportDst = dstport;
        pSocket->SetFlags(SOCKF_CONNECTED);
    }

    return(NETERR_OK);
}

// ---------------------------------------------------------------------------------------
// UdpRead
// ---------------------------------------------------------------------------------------

NTSTATUS CXnSock::UdpRead(CSocket * pSocket, CRecvReq * pRecvReq)
{
    CUdpRecvBuf *   pUdpRecvBuf;
    UINT            cbCopy;
    NTSTATUS        status;

    {
        RaiseToDpc();

        if (pSocket->IsUdpRecvBufEmpty())
        {
            return(RecvReqEnqueue(pSocket, pRecvReq));
        }

        //
        // If there is unprocessed datagram,
        // we must not have any pending recv requests.
        // So we can satisfy the request immediately.
        //
        pUdpRecvBuf = (CUdpRecvBuf *)pSocket->DequeueRecvBuf();
        pSocket->SetCbRecvBuf(pSocket->GetCbRecvBuf() - pUdpRecvBuf->GetCbBuf());
    }

    pRecvReq->SetFromAddrPort(pUdpRecvBuf->fromaddr, pUdpRecvBuf->fromport);
    SecRegSetOwned(pUdpRecvBuf->fromaddr);

    if (pUdpRecvBuf->GetCbBuf() <= pRecvReq->buflen)
    {
        cbCopy = pUdpRecvBuf->GetCbBuf();
        pRecvReq->flags = 0;
        status = NETERR_OK;
    }
    else
    {
        cbCopy = pRecvReq->buflen;
        pRecvReq->flags = MSG_PARTIAL;
        status = NETERR_MSGSIZE;
    }

    *pRecvReq->bytesRecv = cbCopy;
    memcpy(pRecvReq->buf, pUdpRecvBuf+1, cbCopy);
    PoolFree(pUdpRecvBuf);

    return(status);
}

// ---------------------------------------------------------------------------------------
// UdpRecv
// ---------------------------------------------------------------------------------------

void CXnSock::UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbData)
{
    TCHECK(SDPC);

    CIpAddr     ipaDst      = pIpHdr->_ipaDst;
    CIpAddr     ipaSrc      = pIpHdr->_ipaSrc;
    CIpPort     ipportDst   = pUdpHdr->_ipportDst;
    CIpPort     ipportSrc   = pUdpHdr->_ipportSrc;
    BYTE *      pbData      = (BYTE *)pUdpHdr + sizeof(CUdpHdr);
    BOOL        fDelivered  = FALSE;
    CSocket *   pSocket;

    Assert(ipportDst != 0);
    Assert(ipportSrc != 0);

    if (ipaDst.IsBroadcast())
    {
        // Insecure broadcast UDP packets in the secure online stack are discarded. 

    #if defined(XNET_FEATURE_ONLINE) && !defined(XNET_FEATURE_INSECURE)
        if (ppkt->IsEsp())
    #endif
        {
            for (pSocket = GetFirstSocket(); pSocket; pSocket = GetNextSocket(pSocket))
            {
                if (    pSocket->GetFlags(SOCKF_TCP|SOCKF_BOUND|SOCKF_NOMORE_RECV) == SOCKF_BOUND
                    &&  (pSocket->_ipportSrc == ipportDst)
                    &&  (pSocket->_ipaDst == ipaSrc || !pSocket->_ipaDst)
                    &&  (pSocket->_ipportDst == ipportSrc || !pSocket->_ipportDst))
                {
                    TraceUdpHdr(pktRecv, pSocket, pUdpHdr, cbData);
                    UdpRecvData(pSocket, ipaSrc, ipportSrc, pbData, cbData);
                    fDelivered = TRUE;
                }
            }
        }
    }
    else
    {
        pSocket = SockFindMatch(ipportDst, ipaSrc, ipportSrc, SOCK_DGRAM);

        if (pSocket && pSocket->TestFlags(SOCKF_BOUND) && !pSocket->TestFlags(SOCKF_NOMORE_RECV))
        {
            // An insecure, non-loopback UDP packet will only be sent to a socket in the
            // secure online stack if the socket has explicitly allowed insecure packets.

        #if defined(XNET_FEATURE_ONLINE) && !defined(XNET_FEATURE_INSECURE)
            if (ppkt->IsEsp() || ppkt->TestFlags(PKTF_RECV_LOOPBACK) || pSocket->TestFlags(SOCKF_INSECURE))
        #endif
            {
                TraceUdpHdr(pktRecv, pSocket, pUdpHdr, cbData);
                UdpRecvData(pSocket, ipaSrc, ipportSrc, pbData, cbData);
                fDelivered = TRUE;
            }
        }
    }

#ifdef XNET_FEATURE_TRACE
    if (!fDelivered)
    {
        if (!ppkt->IsEsp() || ipaDst.IsBroadcast())
            TraceSz4(pktRecv, "[DISCARD] No UDP socket listening on port %d from %s:%d%s",
                     NTOHS(ipportDst), ipaSrc.Str(), NTOHS(ipportSrc),
                     ipaDst.IsBroadcast() ? " (via broadcast)" : "");
        else
            TraceSz3(pktWarn, "[DISCARD] No UDP socket listening on port %d from %s:%d",
                     NTOHS(ipportDst), ipaSrc.Str(), NTOHS(ipportSrc));
    }
#endif
}

void CXnSock::UdpRecvData(CSocket* pSocket, CIpAddr fromaddr, CIpPort fromport, BYTE * pbData, UINT cbData)
{
    if (pSocket->HasRecvReq())
    {
        CRecvReq *      pRecvReq;
        WSAOVERLAPPED * pWsaOverlapped;
        UINT            cbCopy;
        NTSTATUS        status;

        // If there is a pending receive request, then there must not be any receive buffers
        // enqueued on this socket (because they would have been consumed by the receive
        // request already).

        Assert(pSocket->IsUdpRecvBufEmpty());

        pRecvReq        = pSocket->GetRecvReq();
        pWsaOverlapped  = pRecvReq->_pWsaOverlapped;

        pRecvReq->SetFromAddrPort(fromaddr, fromport);
        SecRegSetOwned(fromaddr);

        cbCopy = min(cbData, pRecvReq->GetCbBuf());

        memcpy(pRecvReq->GetPbBuf(), pbData, cbCopy);

        pWsaOverlapped->_ioxfercnt = cbCopy;

        if (cbCopy < cbData)
        {
            pWsaOverlapped->_ioflags = MSG_PARTIAL;
            status = NETERR_MSGSIZE;
            TraceSz3(udpWarn, "[%X] Copied %ld bytes (%ld lost) into overlapped request",
                     pSocket, cbCopy, cbData - cbCopy);
        }
        else
        {
            pWsaOverlapped->_ioflags = 0;
            status = NETERR_OK;
            TraceSz2(pktRecv, "[%X] Copied %ld bytes directly into overlapped request",
                     pSocket, cbCopy);
        }

        SockReqComplete(pSocket, pRecvReq, status);
        return;
    }

    if (pSocket->IsRecvBufFull())
    {
        TraceSz3(udpWarn, "[%X] Receive buffer is full (%ld bytes).  UDP packet plus %ld data bytes lost.",
                 pSocket, pSocket->GetCbRecvBuf(), cbData);
        return;
    }

    // Copy the data into a receive buffer for later reading.
    
    CUdpRecvBuf * pUdpRecvBuf = (CUdpRecvBuf *)PoolAlloc(sizeof(CUdpRecvBuf) + cbData, PTAG_CUdpRecvBuf);

    if (pUdpRecvBuf == NULL)
    {
        TraceSz2(udpWarn, "[%X] Out of memory allocating receive buffer.  Packet and %ld data bytes lost.",
                 pSocket, cbData);
        return;
    }

    pUdpRecvBuf->Init(this);

    memcpy(pUdpRecvBuf + 1, pbData, cbData);
    pUdpRecvBuf->SetCbBuf(cbData);
    pUdpRecvBuf->fromaddr = fromaddr;
    pUdpRecvBuf->fromport = fromport;

    pSocket->SetCbRecvBuf(pSocket->GetCbRecvBuf() + cbData);
    pSocket->EnqueueRecvBuf(pUdpRecvBuf);
    pSocket->SignalEvent(SOCKF_EVENT_READ);

    TraceSz3(pktRecv, "[%X.u] Buffered %ld bytes (%ld available)",
             pSocket, cbData, pSocket->GetCbRecvBuf());
}

// ---------------------------------------------------------------------------------------
// UdpSend
// ---------------------------------------------------------------------------------------

NTSTATUS CXnSock::UdpSend(CSocket* pSocket, CSendReq * pSendReq, UINT uiFlags)
{
    CSendBuf *      pSendBuf;
    CUdpHdr *       pUdpHdr;
    CIpAddr         ipaDst;
    CRouteEntry **  pprte;

    if (pSendReq->sendtotal > UDP_MAXIMUM_MSS)
    {
        TraceSz3(udpWarn, "[%X] Can't send %ld bytes.  Maximum is %ld bytes.",
                 pSocket, pSendReq->sendtotal, UDP_MAXIMUM_MSS);
        return(NETERR_MSGSIZE);
    }

    Assert((uiFlags & ~(PKTF_POOLALLOC)) == 0);

    uiFlags |= PKTF_TYPE_UDP;

    ipaDst = pSendReq->toaddr ? pSendReq->toaddr->sin_addr.s_addr : pSocket->_ipaDst;

    if (ipaDst.IsSecure())
    {
        uiFlags |= PKTF_TYPE_ESP|PKTF_CRYPT;
    }
    else if (ipaDst.IsBroadcast())
    {
        uiFlags |= PKTF_TYPE_ESP|PKTF_CRYPT;

    #ifdef XNET_FEATURE_INSECURE
        if (cfgFlags & XNET_STARTUP_BYPASS_SECURITY)
        {
            uiFlags &= ~(PKTF_TYPE_ESP|PKTF_CRYPT);
        }
    #endif
    }
    else
    {
    #ifdef XNET_FEATURE_ONLINE
        if (pSocket->TestFlags(SOCKF_INSECURE))
        {
            uiFlags |= PKTF_XMIT_INSECURE;
        }
    #endif
    }

    pSendBuf = (CSendBuf *)PacketAlloc(PTAG_CUdpPacket, uiFlags, pSendReq->sendtotal,
                                       sizeof(CSendBuf), (PFNPKTFREE)UdpPacketFree);

    if (pSendBuf == NULL)
    {
        TraceSz1(udpWarn, "[%X] Out of memory allocating UDP packet", pSocket);
        return(NETERR_MEMORY);
    }

    pSendBuf->Init(pSocket, pSendReq->sendtotal, 2);

    pUdpHdr = pSendBuf->GetUdpHdr();

    // Make a copy of the user data that's passed in

    if (pSendReq->bufcnt == 1)
    {
        memcpy(pUdpHdr + 1, pSendReq->bufs->buf, pSendReq->sendtotal);
    }
    else
    {
        BYTE *      p      = (BYTE *)(pUdpHdr + 1);
        WSABUF *    bufs   = pSendReq->bufs;
        UINT        bufcnt = pSendReq->bufcnt;

        while (bufcnt--)
        {
            memcpy(p, bufs->buf, bufs->len);
            p += bufs->len;
            bufs++;
        }
    }

    if (pSendReq->toaddr)
    {
        pUdpHdr->_ipportSrc = pSocket->_ipportSrc;
        pUdpHdr->_ipportDst = pSendReq->toaddr->sin_port;
        pprte               = NULL;
    }
    else
    {
        pUdpHdr->_ipportSrc = pSocket->_ipportSrc;
        pUdpHdr->_ipportDst = pSocket->_ipportDst;
        pprte               = &pSocket->_prte;
    }

    pUdpHdr->_wLen = HTONS(sizeof(CUdpHdr) + pSendReq->sendtotal);
    pUdpHdr->_wChecksum = 0;

    {
        RaiseToDpc();
        pSocket->IncCbSendBuf(pSendReq->sendtotal);
        pSocket->EnqueueSendBuf(pSendBuf);
        TraceUdpHdr(pktXmit, pSocket, pUdpHdr, pSendReq->sendtotal);
        IpFillAndXmit(pSendBuf, ipaDst, IPPROTOCOL_UDP, pprte);
    }

    return(NETERR_OK);
}

void CXnSock::UdpPacketFree(CPacket * ppkt)
{
    ICHECK(SOCK, UDPC|SDPC);

    CSendBuf * pSendBuf = (CSendBuf *)ppkt;

    if (pSendBuf->Release() > 0)
    {
        CSocket * pSocket = pSendBuf->GetSocket();
        pSocket->DequeueSendBuf(pSendBuf);
        pSocket->DecCbSendBuf(pSendBuf->GetCbBuf());

        if (!pSocket->IsSendBufFull())
        {
            if (pSocket->HasSendReq())
            {
                CSendReq * pSendReq = pSocket->GetSendReq();
                NTSTATUS status = UdpSend(pSocket, pSendReq, PKTF_POOLALLOC);
                pSendReq->_pWsaOverlapped->_ioxfercnt = pSendReq->sendtotal;
                SockReqComplete(pSocket, pSendReq, status);
            }
            else
            {
                pSocket->SignalEvent(SOCKF_EVENT_WRITE);
            }
        }
    }

    PacketFree(ppkt);
}
