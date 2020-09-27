/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    ipsend.c

Abstract:

    IP datagram transmission related functions

Revision History:

    05/22/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Default type-of-service value for outgoing datagrams
//
BYTE cfgDefaultTos = 0;

//
// Default time-to-live value for outgoing datagrams
//
BYTE cfgDefaultTtl = 64;

//
// Identifier to be assigned to the next outgoing IP datagram
//
WORD IpNextDgramId;


NTSTATUS
IpSendPacket(
    Packet* pkt,
    IPADDR dstaddr,
    IfInfo* ifp,
    RTE** cachedRte
    )

/*++

Routine Description:

    Send out an IP datagram to the specified destination address

Arguments:

    pkt - Specifies the datagram to be sent
    dstaddr - Specifies the destination address
    ifp - Points to the outgoing interface, optional
    cachedRte - Address of a cached route entry pointer

Return Value:

    Status code

--*/

{
    RTE* rte;

    RUNS_AT_DISPATCH_LEVEL

    // The IP header must be completely filled out
    ASSERT(pkt->data == (BYTE*) GETPKTIPHDR(pkt));

    // Find a route and cache it
    if ((rte = *cachedRte) == NULL || IsRteOrphaned(rte)) {
        // release the orphaned route entry
        if (rte) { IpReleaseCachedRTE(rte); }

        *cachedRte = rte = IpFindRTE(dstaddr, ifp);
        if (!rte) {
            XnetCompletePacket(pkt, NETERR_UNREACHABLE);
            return NETERR_UNREACHABLE;
        }
        RteAddRef(rte);
    }

    pkt->nexthop = IsRteLocal(rte) ? dstaddr : rte->nexthop;
    ifp = rte->ifp;
    if (pkt->datalen > ifp->mtu) {
        XnetCompletePacket(pkt, NETERR_MSGSIZE);
        return NETERR_MSGSIZE;
    }

    //
    // Queue up the packet for transmission
    // We use a very crude priority mechanism here:
    // - normal packets are added to the end of the queue
    // - retransmitted packets are added to the front of the queue
    //
    if (pkt->pktflags & PKTFLAG_REXMIT) {
        PktQInsertHead(&ifp->sendq, pkt);
    } else {
        PktQInsertTail(&ifp->sendq, pkt);
    }
    ifp->StartOutput(ifp);

    return NETERR_OK;
}


VOID
IpSendPacketInternal(
    Packet* pkt,
    IPADDR srcaddr,
    IPADDR dstaddr,
    BYTE protocol,
    IfInfo* ifp
    )

/*++

Routine Description:

    Send an IP datagram out on the specified interface.
    This is called by internal clients such as ICMP and DHCP.

Arguments:

    pkt - Specifies the datagram to be sent
    srcaddr - Specifies the source address
    dstaddr - Specifies the destination address
    protocol - Specifies the protocol number
    ifp - Points to the outgoing interface

Return Value:

    NONE

Note:

    We assume the pkt->iphdr field points to where the IP header starts.

--*/

{
    RTE* rte = NULL;
    IpHeader* iphdr;
    UINT iphdrlen;
    KIRQL irql;

    //
    // Fix up the IP header
    //
    iphdr = GETPKTIPHDR(pkt);
    ASSERT((BYTE*) iphdr < pkt->data);

    iphdrlen = pkt->data - (BYTE*) iphdr;
    pkt->data -= iphdrlen;
    pkt->datalen += iphdrlen;

    FILL_IPHEADER(
        iphdr,
        iphdrlen,
        cfgDefaultTos,
        pkt->datalen,
        0,
        cfgDefaultTtl,
        protocol,
        srcaddr,
        dstaddr);

    irql = RaiseToDpc();
    IpSendPacket(pkt, dstaddr, ifp, &rte);
    if (rte) { IpReleaseCachedRTE(rte); }
    LowerFromDpc(irql);
}

