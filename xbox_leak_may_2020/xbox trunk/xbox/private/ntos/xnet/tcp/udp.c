/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    udp.c

Abstract:

    UDP protocol related functions

Revision History:

    05/27/2000 davidx
        Created it.

--*/

#include "precomp.h"
#include "..\dhcp\dhcp.h"


PRIVATE VOID
PcbDeliverDgram(
    PCB* pcb,
    Packet* pkt,
    IPADDR fromaddr,
    IPPORT fromport
    )

/*++

Routine Description:

    Deliver a received datagram to the specified socket

Arguments:

    pcb - Points to the protcol control block
    pkt - Points to the received datagram
    fromaddr, fromport - The sender's address

Return Value:

    NONE

--*/

{
    RECVBUF* recvbuf;

    //
    // If there are datagrams queued up on this socket
    // that haven't been processed, then we must not
    // have any pending recv requests.
    //
    // Otherwise, check if there is a pending recv request
    // that can be satisfied with the incoming datagram.
    //
    if (HasOverlappedRecv(pcb)) {
        WSAOVERLAPPED* overlapped;
        RECVREQ* recvreq;
        UINT copycnt;
        NTSTATUS status;

        ASSERT(IsDgramRecvBufEmpty(pcb));
        recvreq = PcbGetOverlappedRecv(pcb);
        overlapped = recvreq->overlapped;

        SetRecvReqFromAddr(recvreq, fromaddr, fromport);

        copycnt = min(pkt->datalen, recvreq->buflen);
        CopyMem(recvreq->buf, pkt->data, copycnt);

        overlapped->_ioxfercnt = copycnt;
        if (copycnt < pkt->datalen) {
            overlapped->_ioflags = MSG_PARTIAL;
            status = NETERR_MSGSIZE;
        } else {
            overlapped->_ioflags = 0;
            status = NETERR_OK;
        }

        PcbCompleteOverlappedRecv(recvreq, status);
        return;
    }

    //
    // If the unprocessed receive queue is full,
    // we'll drop the incoming packet. The alternative
    // is to drop the oldest packet.
    //
    if (IsPcbRecvBufFull(pcb)) return;

    // Make a copy of the incoming datagram
    // NOTE: we're not copying protocol header data here.

    recvbuf = (RECVBUF*) XnetAlloc(sizeof(RECVBUF) + pkt->datalen, PTAG_RBUF);
    if (!recvbuf) {
        WARNING_("Received datagram discarded because of no memory");
        WARNING_("  local = %d, remote = %s:%d",
            NTOHS(pcb->srcport),
            IPADDRSTR(fromaddr),
            NTOHS(fromport));
        return;
    }

    CopyMem(recvbuf+1, pkt->data, pkt->datalen);
    recvbuf->datalen = pkt->datalen;
    pcb->recvbufSize += pkt->datalen;
    recvbuf->fromaddr = fromaddr;
    recvbuf->fromport = fromport;

    InsertTailList(&pcb->recvbuf, &recvbuf->links);
    PcbSignalEvent(pcb, PCBEVENT_READ);
}


UINT
PcbDeliverRecvDgramMulti(
    Packet* pkt,
    IPADDR toaddr,
    IPPORT toport,
    IPADDR fromaddr,
    IPPORT fromport,
    BYTE type,
    BYTE protocol
    )

/*++

Routine Description:

    Deliver a received datagram to all applicable socket(s)

Arguments:

    pkt - Points to the received datagram
    toaddr, toport - The destination address of the datagram
    fromaddr, fromport - The sender's address
    type - Specifies the socket type (SOCK_DGRAM or SOCK_RAW)
    protocol - Specifies the protocol number

Return Value:

    Number of sockets this datagram was delivered to

--*/

{
    PCB* pcb;
    UINT delivered = 0;

    LOOP_THRU_PCB_LIST(pcb)

        if (pcb->type != type ||
            pcb->protocol != protocol && pcb->protocol ||
            pcb->srcport != toport ||
            IS_MCAST_IPADDR(toaddr) && !PcbCheckMcastGroup(pcb, toaddr) ||
            pcb->dstaddr != fromaddr && pcb->dstaddr ||
            pcb->dstport != fromport && pcb->dstport ||
            !IsPcbBound(pcb) ||
            IsPcbRecvShutdown(pcb))
            continue;

        PcbDeliverDgram(pcb, pkt, fromaddr, fromport);
        delivered++;

    END_PCB_LIST_LOOP

    return delivered;
}


NTSTATUS
PcbRecvDgram(
    PCB* pcb,
    RECVREQ* recvreq
    )

/*++

Routine Description:

    Handle a user request to receive data from a datagram socket

Arguments:

    pcb - Points to the protocol control block
    recvreq - Points to the receive request information

Return Value:

    Status code

Note:

    This call always return immediately. For blocking calls,
    we assume the caller has done the necessary wait.

--*/

{
    NTSTATUS status;
    KIRQL irql = RaiseToDpc();

    if (!IsDgramRecvBufEmpty(pcb)) {
        //
        // If there is unprocessed datagram,
        // we must not have any pending recv requests.
        // So we can satisfy the request immediately.
        //
        RECVBUF* recvbuf;
        DWORD copycnt;

        recvbuf = (RECVBUF*) RemoveHeadList(&pcb->recvbuf);
        pcb->recvbufSize -= recvbuf->datalen;
        LowerFromDpc(irql);

        SetRecvReqFromAddr(recvreq, recvbuf->fromaddr, recvbuf->fromport);

        if (recvbuf->datalen <= recvreq->buflen) {
            copycnt = recvbuf->datalen;
            recvreq->flags = 0;
            status = NETERR_OK;
        } else {
            copycnt = recvreq->buflen;
            recvreq->flags = MSG_PARTIAL;
            status = NETERR_MSGSIZE;
        }

        *recvreq->bytesRecv = copycnt;
        CopyMem(recvreq->buf, recvbuf+1, copycnt);
        XnetFree(recvbuf);
        return status;
    }

    status = PcbQueueOverlappedRecv(pcb, recvreq);
    LowerFromDpc(irql);
    return status;
}


VOID
UdpReceivePacket(
    Packet* pkt
    )

/*++

Routine Description:

    Receive a UDP datagram

Arguments:

    pkt - Points to the received UDP datagram

Return Value:

    NONE

--*/

{
    IpHeader* iphdr;
    PseudoHeader pseudohdr;
    UdpHeader* udphdr;
    UINT udplen;
    INT delivered;
    IPADDR dstaddr;

    RUNS_AT_DISPATCH_LEVEL

    // Validate header length field
    if (pkt->datalen < UDPHDRLEN) goto discard;

    iphdr = GETPKTIPHDR(pkt);
    pseudohdr.srcaddr = iphdr->srcaddr;
    pseudohdr.dstaddr = dstaddr = iphdr->dstaddr;
    pseudohdr.zero = 0;
    pseudohdr.protocol = IPPROTOCOL_UDP;

    udphdr = GETPKTDATA(pkt, UdpHeader);
    udplen = NTOHS(udphdr->length);
    pseudohdr.length = udphdr->length;

    if (udplen < UDPHDRLEN || udplen > pkt->datalen)
        goto discard;

    // Validate checksum if it's present

    if (udphdr->checksum) {
        UINT checksum;
        checksum = tcpipxsum(0, &pseudohdr, sizeof(pseudohdr));
        if (tcpipxsum(checksum, udphdr, udplen) != 0xffff)
            goto discard;
    }

    pkt->data += UDPHDRLEN;
    pkt->datalen = udplen - UDPHDRLEN;

    if (udphdr->dstport == 0)
        goto discard;

    delivered = 0;

    //
    // Hardwired hooks to the DHCP client
    //
    if (udphdr->dstport == DHCP_CLIENT_PORT &&
        IfDhcpEnabled(pkt->recvifp)) {
        // NOTE: we retain ownership of the packet here,
        //  i.e. DhcpReceivePacket must not call XnetCompletePacket.
        DhcpReceivePacket(pkt->recvifp, pkt);
        delivered++;
    }

    //
    // If this is a unicast datagram, then we'll deliver it
    // to at most one matching socket. Broadcast or unicast
    // datagrams are delivered to all matching datagrams.
    //
    if (!IS_MCAST_IPADDR(dstaddr) && !IfBcastAddr(pkt->recvifp, dstaddr)) {
        PCB* pcb = PcbFindMatch(
                    dstaddr,
                    udphdr->dstport,
                    pseudohdr.srcaddr,
                    udphdr->srcport,
                    SOCK_DGRAM,
                    IPPROTOCOL_UDP);

        if (pcb && IsPcbBound(pcb) && !IsPcbRecvShutdown(pcb)) {
            PcbDeliverDgram(pcb, pkt, pseudohdr.srcaddr, udphdr->srcport);
            delivered++;
        }
    } else {
        delivered += PcbDeliverRecvDgramMulti(
                        pkt,
                        dstaddr,
                        udphdr->dstport,
                        pseudohdr.srcaddr,
                        udphdr->srcport,
                        SOCK_DGRAM,
                        IPPROTOCOL_UDP);
    }

    if (delivered) {
        XnetCompletePacket(pkt, NETERR_OK);
        return;
    }

    // If the datagram wasn't delivered,
    // send out a port-unreachable ICMP message.
    IcmpSendError(
        pkt,
        ICMPTYPE_DESTINATION_UNREACHABLE,
        ICMPCODE_PORT_UNREACHABLE);

discard:
    XnetCompletePacket(pkt, NETERR_DISCARDED);
}


VOID
RawReceivePacket(
    Packet* pkt
    )

/*++

Routine Description:

    Receive a raw IP datagram

Arguments:

    pkt - Points to the received datagram

Return Value:

    NONE

--*/

{
    UINT delivered;
    BYTE protocol;
    IpHeader* iphdr;

    RUNS_AT_DISPATCH_LEVEL

    // Since pkt->data points to after the IP header,
    // we need to fix it here.
    iphdr = GETPKTIPHDR(pkt);
    pkt->datalen += (pkt->data - (BYTE*) iphdr);
    pkt->data = (BYTE*) iphdr;

    if ((protocol = iphdr->protocol) != 0) {
        //
        // The same datagram could be delivered to
        // multiple sockets. This is to follow w2k behavior.
        //
        delivered = PcbDeliverRecvDgramMulti(
                        pkt,
                        iphdr->dstaddr,
                        0,
                        iphdr->srcaddr,
                        0,
                        SOCK_RAW,
                        protocol);
        
        if (delivered) {
            XnetCompletePacket(pkt, NETERR_OK);
            return;
        }

        if (protocol != IPPROTOCOL_IGMP &&
            protocol != IPPROTOCOL_ICMP) {
            IcmpSendError(
                pkt,
                ICMPTYPE_DESTINATION_UNREACHABLE,
                ICMPCODE_PROTOCOL_UNREACHABLE);
        }
    }

    XnetCompletePacket(pkt, NETERR_DISCARDED);
}


VOID
UdpSendPacketInternal(
    Packet* pkt,
    IpAddrPair* addrpair,
    IfInfo* ifp
    )

/*++

Routine Description:

    Send out a UDP datagram (used internally by DHCP client)

Arguments:

    pkt - Points to the packet to be sent
    addrpair - Specifies source & destination addresses and ports
    ifp - Specifies the outgoing interface

Return Value:

    NONE

--*/

{
    // Compose UDP header
    pkt->data -= UDPHDRLEN;
    pkt->datalen += UDPHDRLEN;
    FILL_UDPHEADER(pkt, addrpair);

    // Call the IP layer to send out the packet
    IpSendPacketInternal(pkt, addrpair->srcaddr, addrpair->dstaddr, IPPROTOCOL_UDP, ifp);
}


PRIVATE NTSTATUS
PcbPrepareSendto(
    PCB* pcb,
    IpAddrPair* addrpair,
    RTE** cachedRte
    )

/*++

Routine Description:

    Choose the proper source address for a sendto call

Arguments:

    pcb - Points to the protocol control block
    addrpair - Specifies source & destination addresses and ports
    cachedRte - Stored the cached route entry

Return Value:

    NONE

--*/

{
    NTSTATUS status = NETERR_OK;
    KIRQL irql = RaiseToDpc();

    if (IS_MCAST_IPADDR(addrpair->dstaddr)) {
        // Sendto is called with a multicast destination address.
        // We'll choose the outgoing interface and source address
        // based on the setting of IP_MULTICAST_IF option.
        IfInfo* ifp = PcbGetMcastIf(pcb);
        if (ifp != NULL)
            addrpair->srcaddr = ifp->ipaddr;
        else
            status = NETERR_UNREACHABLE;
    } else {
        // Trying to find a route to the sendto destination.
        // And use that interface's address as our source address.
        RTE* rte = IpFindRTE(addrpair->dstaddr, NULL);
        if (rte != NULL) {
            *cachedRte = rte;
            RteAddRef(rte);
            addrpair->srcaddr = PcbGetDefaultSrcAddr(rte);
        } else {
            status = NETERR_UNREACHABLE;
        }
    }
    
    LowerFromDpc(irql);
    return status;
}


//
// Completion routine that's called when an outgoing datagram is sent
// 
PRIVATE VOID DgramSendCompletionProc(Packet* pkt, NTSTATUS status) {
    SENDBUF* sendbuf = GETPKTBUF(pkt, SENDBUF);
    KIRQL irql = RaiseToDpc();
    LONG refcount;
    PCB* pcb;

    if ((refcount = SendbufDecRef(sendbuf)) > 0) {
        pcb = sendbuf->pcb;
        RemoveEntryList(&sendbuf->links);
        pcb->sendbufSize -= sendbuf->datalen;

        if (!IsPcbSendBufFull(pcb)) {
            if (HasOverlappedSend(pcb)) {
                SENDREQ* sendreq = PcbGetOverlappedSend(pcb);

                status = PcbSendDgram(pcb, sendreq);
                sendreq->overlapped->_ioxfercnt = sendreq->sendtotal;
                PcbCompleteOverlappedSend(sendreq, status);
            } else {
                PcbSignalEvent(pcb, PCBEVENT_WRITE);
            }
        }
    }

    LowerFromDpc(irql);
    XnetFreePacket(pkt);
}


NTSTATUS
PcbSendDgram(
    PCB* pcb,
    SENDREQ* sendreq
    )

/*++

Routine Description:

    Send out a datagram from a UDP or raw socket

Arguments:

    pcb - Points to the protocol control block
    sendreq - Points to send request information

Return Value:

    Status code

Note:

    This function always return immediately. For blocking operation,
    we assume the caller has taken care of any necessary wait.

    The winsock layer checks to see if the send buffer is full before
    calling this function. In the case that:
        old sendbuf size + the datagram size > max sendbuf size
    we'll temporarily go over the send buffer size limit.

--*/

{
    SENDBUF* sendbuf;
    Packet* pkt;
    UINT hdrsize;
    KIRQL irql;
    IpAddrPair* addrpair;
    IpAddrPair sendtoAddr;
    RTE** cachedRte;
    RTE* sendtoRte = NULL;
    NTSTATUS status;

    hdrsize = sizeof(SENDBUF) +
              MAXLINKHDRLEN +
              IPHDRLEN + pcb->ipoptlen +
              UDPHDRLEN;
                
    pkt = XnetAllocPacket(hdrsize + sendreq->sendtotal, 0);
    if (!pkt) return NETERR_MEMORY;
    pkt->data += hdrsize;
    pkt->datalen = sendreq->sendtotal;

    // Make a copy of the user data that's passed in
    if (sendreq->bufcnt == 1) {
        CopyMem(pkt->data, sendreq->bufs->buf, sendreq->sendtotal);
    } else {
        BYTE* p = pkt->data;
        WSABUF* bufs = sendreq->bufs;
        UINT bufcnt = sendreq->bufcnt;

        while (bufcnt--) {
            CopyMem(p, bufs->buf, bufs->len);
            p += bufs->len;
            bufs++;
        }
    }

    if (!sendreq->toaddr && !IS_MCAST_IPADDR(pcb->dstaddr)) {
        // Normal send request
        addrpair = &pcb->addrpair;
        cachedRte = &pcb->rte;
    } else {
        // This is a sendto request.
        addrpair = &sendtoAddr;
        cachedRte = &sendtoRte;
        if (sendreq->toaddr) {
            addrpair->dstport = sendreq->toaddr->sin_port;
            addrpair->dstaddr = sendreq->toaddr->sin_addr.s_addr;
        } else {
            addrpair->dstport = pcb->dstport;
            addrpair->dstaddr = pcb->dstaddr;
        }
        addrpair->srcport = pcb->srcport;
        addrpair->srcaddr = pcb->bindSrcAddr;

        if (addrpair->srcaddr == 0) {
            status = PcbPrepareSendto(pcb, addrpair, cachedRte);
            if (!NT_SUCCESS(status)) {
                XnetFreePacket(pkt);
                return status;
            }
        }
    }

    if (pcb->type == SOCK_DGRAM) {
        // Sending UDP datagram
        pkt->data -= UDPHDRLEN;
        pkt->datalen += UDPHDRLEN;
        FILL_UDPHEADER(pkt, addrpair);
        PcbSetupIpHeader(pcb, pkt, addrpair);
    } else {
        // Sending raw IP datagram
        if (pcb->ipHdrIncl) {
            if (pkt->datalen < IPHDRLEN) {
                XnetFreePacket(pkt);
                status = NETERR_PARAM;
                goto exit;
            }
            SETPKTIPHDR(pkt, pkt->data);
        } else {
            PcbSetupIpHeader(pcb, pkt, addrpair);
        }
    }

    // Set up the packet completion routine to
    // decrement the send buffer size when the packet is sent.

    sendbuf = GETPKTBUF(pkt, SENDBUF);
    sendbuf->refcount = 2;
    sendbuf->datalen = sendreq->sendtotal;
    sendbuf->pcb = pcb;
    XnetSetPacketCompletion(pkt, DgramSendCompletionProc);

    irql = RaiseToDpc();
    pcb->sendbufSize += sendreq->sendtotal;
    InsertTailList(&pcb->sendbuf, &sendbuf->links);

    if (IS_MCAST_IPADDR(addrpair->dstaddr)) {
        // Send a copy of the multicast datagram to ourselves if needed
        status = PcbSendMcastDgram(pcb, pkt, addrpair->dstaddr);
    } else {
        // If this is a broadcast datagram, send a copy to ourselves
        // NOTE: subnet directed broadcast isn't handled correctly.
        if (IS_BCAST_IPADDR(addrpair->dstaddr)) {
            IpQueueLoopbackPacket(pkt, TRUE);
        }

        status = IpSendPacket(pkt, addrpair->dstaddr, NULL, cachedRte);
    }
    LowerFromDpc(irql);

exit:
    if (sendtoRte) { IpReleaseCachedRTE(sendtoRte); }
    return status;
}


NTSTATUS
PcbShutdownDgram(
    PCB* pcb,
    BYTE flags
    )

/*++

Routine Description:

    Shutdown a datagram socket

Arguments:

    pcb - Points to the protocol control block
    flags - Shutdown flags: PCBFLAG_SEND_SHUTDOWN and/or PCBFLAG_RECV_SHUTDOWN

Return Value:

    Status code

--*/

{
    KIRQL irql = RaiseToDpc();

    if (flags & PCBFLAG_RECV_SHUTDOWN) {
        PcbClearOverlappedRecvs(pcb, NETERR(WSAESHUTDOWN));
        PcbFlushRecvBuffers(pcb);
    }

    if (flags & PCBFLAG_SEND_SHUTDOWN) {
        PcbClearOverlappedSends(pcb, NETERR(WSAESHUTDOWN));
    }

    pcb->flags |= flags;
    LowerFromDpc(irql);
    return NETERR_OK;
}

