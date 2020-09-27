/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    icmp.c

Abstract:

    Implementation of ICMP related functions

Revision History:

    05/22/2000 davidx
        Created it.

--*/

#include "precomp.h"


PRIVATE IPADDR
IcmpShouldSendResponse(
    Packet* recvpkt,
    BOOL error
    )

/*++

Routine Description:

    Determine if we should send an ICMP message in response
    to a received datagram

Arguments:

    recvpkt - Specifies the received datagram
    error - Whether we're trying to send an error response

Return Value:

    IP address of the host we should send the ICMP message to;
    0 if we should not send a response

--*/

{
    IPADDR fromaddr, toaddr;
    IfInfo* ifp = recvpkt->recvifp;
    IpHeader* iphdr = GETPKTIPHDR(recvpkt);

    fromaddr = iphdr->srcaddr;
    toaddr = iphdr->dstaddr;

    // We don't respond to ICMP Echo Requests
    // sent to broadcast or multicast addresses
    if (IfBcastAddr(ifp, toaddr) || IS_MCAST_IPADDR(toaddr))
        return 0;

    // Source IP address cannot be one of the following:
    //  0, broadcast or multicast addresses
    if (fromaddr == 0 ||
        IfBcastAddr(ifp, fromaddr) ||
        IS_MCAST_IPADDR(fromaddr))
        return 0;

    if (error) {
        // 1. We don't send ICMP error message to *any* ICMP message
        //    instead of just ICMP error messages.
        // 2. Only send ICMP error for the very first segment.
        // 3. Don't respond if link-layer broadcast or multicast.
        // 4. Don't respond if source is loopback address.
        if ((iphdr->protocol == IPPROTOCOL_ICMP) ||
            (iphdr->fragoffset & HTONS(FRAGOFFSET_MASK)) ||
            (recvpkt->pktflags & PKTFLAG_MCAST) ||
            IS_LOOPBACK_IPADDR(fromaddr))
            return 0;
    }

    return fromaddr;
}


VOID
IcmpSendError(
    Packet* origpkt,
    UINT type,
    UINT code
    )

/*++

Routine Description:

    Send out an ICMP error message in response to a received datagram

Arguments:

    origpkt - The received datagram
    type, code - Specifies the ICMP message type and code

Return Value:

    NONE

--*/

{
    Packet* sendpkt;
    UINT icmpdatalen;
    IcmpMessage* icmpmsg;
    IPADDR fromaddr;
    IpHeader* origiphdr;
    IfInfo* ifp = origpkt->recvifp;

    // make sure we're allowed to send an ICMP message
    fromaddr = IcmpShouldSendResponse(origpkt, TRUE);
    if (fromaddr == 0) return;
    
    // allocate packet buffer for composing the ICMP message
    origiphdr = GETPKTIPHDR(origpkt);
    icmpdatalen = min(GETIPHDRLEN(origiphdr) + 8, GETIPLEN(origiphdr));

    sendpkt = XnetAllocIpPacket(0, ICMPHDRLEN+icmpdatalen);
    if (!sendpkt) return;

    icmpmsg = GETPKTDATA(sendpkt, IcmpMessage);
    ZeroMem(icmpmsg, ICMPHDRLEN);
    icmpmsg->type = (BYTE) type;
    icmpmsg->code = (BYTE) code;
    CopyMem(icmpmsg->origdata, origiphdr, icmpdatalen);

    COMPUTE_CHECKSUM(icmpmsg->checksum, icmpmsg, ICMPHDRLEN+icmpdatalen);

    IpSendPacketInternal(sendpkt, ifp->ipaddr, fromaddr, IPPROTOCOL_ICMP, ifp);
}


PRIVATE VOID
IcmpSendEchoReply(
    Packet* reqpkt
    )

/*++

Routine Description:

    Respond to an ICMP Echo request message by sending
    out an ICMP Echo Reply message

Arguments:

    reqpkt - Points to the received packet

Return Value:

    NONE

--*/

{
    IPADDR fromaddr;
    Packet* replypkt;
    IfInfo* ifp;
    IcmpMessage* icmpreq;
    IcmpMessage* icmpreply;
    IpHeader* iphdrreq;
    IpHeader* iphdrreply;
    UINT iphdrlen, ipdatalen;

    // make sure it's ok to reply to the request
    fromaddr = IcmpShouldSendResponse(reqpkt, FALSE);
    if (!fromaddr) return;

    // calculate the reply message length
    //  (truncate data if we need to fragment)
    ifp = reqpkt->recvifp;
    iphdrreq = GETPKTIPHDR(reqpkt);
    iphdrlen = GETIPHDRLEN(iphdrreq);
    ipdatalen = GETIPLEN(iphdrreq) - iphdrlen;
    if (iphdrlen+ipdatalen > ifp->mtu)
        ipdatalen = ifp->mtu - iphdrlen;

    // allocate packet buffer and compose
    // the ICMP reply message

    replypkt = XnetAllocIpPacket(iphdrlen-IPHDRLEN, ipdatalen);
    if (!replypkt) return;

    iphdrreply = GETPKTIPHDR(replypkt);
    CopyMem(iphdrreply, iphdrreq, iphdrlen+ipdatalen);

    icmpreq = (IcmpMessage*) ((BYTE*) iphdrreq + iphdrlen);
    icmpreply = (IcmpMessage*) ((BYTE*) iphdrreply + iphdrlen);
    icmpreply->type = ICMPTYPE_ECHO_REPLY;
    icmpreply->code = icmpreq->code;
    icmpreply->id = icmpreq->id;
    icmpreply->seqno = icmpreq->seqno;

    COMPUTE_CHECKSUM(icmpreply->checksum, icmpreply, ipdatalen);

    // reflect option data in the IP header
    fromaddr = IpReflectIpOptions(ifp, iphdrreply, fromaddr);

    IpSendPacketInternal(replypkt, ifp->ipaddr, fromaddr, IPPROTOCOL_ICMP, ifp);
}


PRIVATE BOOL
IcmpCheckEmbeddedIpHeader(
    const IpHeader* iphdr,
    UINT buflen
    )

/*++

Routine Description:

    Validate the embedded IP header information inside an ICMP message

Arguments:

    iphdr - Points to the beginning of the embedded ICMP data
    buflen - Length of the data buffer

Return Value:

    TRUE if the embedded IP header information is valid
    FALSE otherwise

--*/

{
    UINT iphdrlen, iplen;

    if (buflen < IPHDRLEN) return FALSE;

    iphdrlen = VERIFY_IPVER_HDRLEN(iphdr->ver_hdrlen);
    iplen = NTOHS(iphdr->length);

    // must include the IP header plus at least 8 bytes of data
    return (iphdrlen >= IPHDRLEN &&
            iphdrlen <= iplen &&
            iphdrlen+8 <= buflen);
}


VOID
IcmpReceivePacket(
    Packet* pkt
    )

/*++

Routine Description:

    Handle the reception of an ICMP message

Arguments:

    pkt - Points to the received packet

Return Value:

    NONE

--*/

{
    IcmpMessage* icmpmsg;
    IpHeader* origiphdr;

    // verify the ICMP message length and checksum
    if (pkt->datalen < ICMPHDRLEN) goto discard;
    if (tcpipxsum(0, pkt->data, pkt->datalen) != 0xffff)
        goto discard;

    icmpmsg = GETPKTDATA(pkt, IcmpMessage);
    origiphdr = (IpHeader*) icmpmsg->origdata;

    switch (icmpmsg->type) {
    case ICMPTYPE_DESTINATION_UNREACHABLE:
    case ICMPTYPE_TIME_EXECEEDED:
    case ICMPTYPE_PARAMETER_PROBLEM:
    case ICMPTYPE_SOURCE_QUENCH:
    case ICMPTYPE_REDIRECT:
        // verify embedded IP header inside the ICMP message
        if (!IcmpCheckEmbeddedIpHeader(origiphdr, pkt->datalen-ICMPHDRLEN))
            goto discard;

        // handle ICMP redirect message
        //  we treat host and network redirects the same way
        if ((icmpmsg->type == ICMPTYPE_REDIRECT) &&
            (icmpmsg->code == ICMPCODE_REDIRECT_NET ||
             icmpmsg->code == ICMPCODE_REDIRECT_HOST ||
             icmpmsg->code == ICMPCODE_REDIRECT_NET_TOS ||
             icmpmsg->code == ICMPCODE_REDIRECT_HOST_TOS)) {
            IpRedirectHostRoute(
                pkt->recvifp,
                origiphdr->dstaddr,
                GETPKTIPHDR(pkt)->srcaddr,
                icmpmsg->gwaddr);
        }

        // Pass ICMP source quench error message to TCP layer
        if (icmpmsg->type == ICMPTYPE_SOURCE_QUENCH &&
            origiphdr->protocol == IPPROTOCOL_TCP) {
            TcpSourceQuench(origiphdr);
        }
        break;

    case ICMPTYPE_ECHO_REQUEST:
        IcmpSendEchoReply(pkt);
        break;
    }

    RawReceivePacket(pkt);
    return;

discard:
    XnetCompletePacket(pkt, NETERR_DISCARDED);
}

