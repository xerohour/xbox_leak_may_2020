/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    iprecv.c

Abstract:

    Handles IP datagram reception

Revision History:

    05/17/2000 davidx
        Created it.

--*/

#include "precomp.h"


//
// A singly-linked list of datagrams that're currently
// being reassembled. We're using a list rather than a
// hash table here for simplicity. We assume that fragmentation
// occurs very infrequently.
//
PRIVATE Packet* IpReassemblyPktList;
PRIVATE UINT IpReassemblyPktCount;

// Maximum number of actively reassembled datagrams
UINT cfgMaxReassemblyDgrams = 4;

// Maximum size for a reassembled datagram
UINT cfgMaxReassemblySize = 2048;

// Reassembly timeout period = 60sec
UINT cfgReassemblyTimeout = 60;

//
// Packet containing a datagram that's being reassembled:
//  packet header
//  reassembly information
//  space to hold maximum IP header
//  actual datagram data
//
typedef struct _ReassemblyHeader {
    IPADDR srcaddr;         // source IP address
    IPADDR dstaddr;         // destination IP address
    UINT proto_id;          // datagram ID and protocol
    UINT timer;             // reassembly timer
    UINT origdatalen;       // total length of the original IP datagram
    DWORD bitFlags[1];      // which fragments have arrived?
} ReassemblyHeader;


VOID
IpCleanupReassemblyPkts(
    IfInfo* ifp
    )

/*++

Routine Description:

    Clean up all partially reassembly datagrams

Arguments:

    ifp - Optional parameter: when present we only clean up
        the partial datagrams from the specified interface;
        otherwise, we clean up all partial datagrams.

Return Value:

    NONE

--*/

{
    Packet** link = &IpReassemblyPktList;
    Packet* pkt;

    while ((pkt = *link) != NULL) {
        if (ifp == pkt->recvifp || ifp == NULL) {
            *link = pkt->nextpkt;
            IpReassemblyPktCount--;
            XnetCompletePacket(pkt, NETERR_CANCELLED);
        } else {
            link = &pkt->nextpkt;
        }
    }
}


VOID
IpReassemblyTimerProc()

/*++

Routine Description:

    IP datagram reassembly timer routine

Arguments:

    NONE

Return Value:

    NONE

--*/

{
    Packet** link = &IpReassemblyPktList;
    Packet* pkt;
    ReassemblyHeader* rahdr;

    while ((pkt = *link) != NULL) {
        rahdr = GETPKTBUF(pkt, ReassemblyHeader);
        if (rahdr->timer-- == 0) {
            TRACE_("Reassembly timeout: src = %s", IPADDRSTR(rahdr->srcaddr));
            *link = pkt->nextpkt;
            IpReassemblyPktCount--;

            // Send out an ICMP error message if fragment 0 has been received
            if (pkt->iphdrOffset) {
                pkt->data = (BYTE*) GETPKTIPHDR(pkt);
                IcmpSendError(
                    pkt,
                    ICMPTYPE_TIME_EXECEEDED,
                    ICMPCODE_REASSEMBLY_TIMEOUT);
            }

            XnetCompletePacket(pkt, NETERR_REASSEMBLY);
        } else
            link = &pkt->nextpkt;
    }
}


PRIVATE Packet**
IpFindReassemblyPkt(
    IPADDR srcaddr,
    IPADDR dstaddr,
    UINT proto_id
    )

/*++

Routine Description:

    Check if we already have a partially assembled datagram
    matching the specified identification information.

Arguments:

    srcaddr - Specifies the source IP address
    dstaddr - Specifies the destination IP address
    proto_id - IP protocol number and datagram ID

Return Value:

    Address of the link pointer to the specified datagram

--*/

{
    Packet** link = &IpReassemblyPktList;
    Packet* pkt;
    ReassemblyHeader* rahdr;

    while ((pkt = *link) != NULL) {
        rahdr = GETPKTBUF(pkt, ReassemblyHeader);
        if (rahdr->srcaddr == srcaddr &&
            rahdr->dstaddr == dstaddr &&
            rahdr->proto_id == proto_id)
            break;

        link = &pkt->nextpkt;
    }

    return link;
}


PRIVATE Packet*
IpAllocReassemblyPkt(
    IPADDR srcaddr,
    IPADDR dstaddr,
    UINT proto_id,
    IfInfo* ifp
    )

/*++

Routine Description:

    Allocate buffer for assembling a datagram

Arguments:

    srcaddr - Specifies the source IP address
    dstaddr - Specifies the destination IP address
    proto_id - IP protocol number and datagram ID
    ifp - Points to the receiving interface

Return Value:

    Pointer to the allocated packet
    NULL if there is an error

--*/

{
    Packet* pkt;
    UINT hdrsize;
    ReassemblyHeader* rahdr;

    //
    // We reserve some space in the head of the packet buffer
    // for storing reassembly information.
    //

    hdrsize = offsetof(ReassemblyHeader, bitFlags) +
              (((cfgMaxReassemblySize / 8) + 31) / 32) * 4 +
              MAXIPHDRLEN;

    pkt = XnetAllocPacket(hdrsize + cfgMaxReassemblySize, PKTFLAG_NETPOOL);
    if (!pkt) return NULL;
    pkt->iphdrOffset = 0;

    rahdr = GETPKTBUF(pkt, ReassemblyHeader);
    ZeroMem(rahdr, hdrsize);
    rahdr->timer = cfgReassemblyTimeout;
    rahdr->srcaddr = srcaddr;
    rahdr->dstaddr = dstaddr;
    rahdr->proto_id = proto_id;

    pkt->data += hdrsize;
    CACHE_IFP_REFERENCE(pkt->recvifp,ifp);
    return pkt;
}


PRIVATE BOOL
IpUpdateFragmentFlags(
    Packet* pkt,
    UINT fragOffset,
    UINT fragCount
    )

/*++

Routine Description:

    Update the bit flags to indicate which fragments have already arrived

Arguments:

    pkt - Points to the packet contain partially assembled datagram
    fragOffset - Starting chunk offset for the current fragment
    fragCount - Number of 8 byte chunks in the current fragment

Return Value:

    TRUE if the entire datagram has been completed
    FALSE otherwise

--*/

{
    UINT chunks, i;
    ReassemblyHeader* rahdr = GETPKTBUF(pkt, ReassemblyHeader);

    // Update flag bits
    chunks = fragOffset % 32;
    i = fragOffset / 32;

    if (chunks) {
        UINT bits = min(fragCount, 32-chunks);
        rahdr->bitFlags[i++] |= (((1 << bits) - 1) << chunks);
        fragCount -= bits;
    }

    while (fragCount >= 32) {
        rahdr->bitFlags[i++] = 0xffffffff;
        fragCount -= 32;
    }

    if (fragCount)
        rahdr->bitFlags[i] |= (1 << fragCount) - 1;

    // We don't know the total datagram length yet,
    // which must mean we haven't finished reassembly.
    if (rahdr->origdatalen == 0) return FALSE;

    // Check if all bit flags are set
    i = 0;
    chunks = (rahdr->origdatalen + 7) / 8;
    while (chunks >= 32) {
        if (rahdr->bitFlags[i++] != 0xffffffff) return FALSE;
        chunks -= 32;
    }

    return (chunks == 0 ||
            rahdr->bitFlags[i] == (1u << chunks) - 1);
}


PRIVATE VOID
IpReassemblePkt(
    Packet* pkt
    )

/*++

Routine Description:

    Handle IP datagram reassembly:
    called when a datagram fragment is received

Arguments:

    pkt - Points to the received packet (datagram fragment)

Return Value:

    NONE

NOTE:

    We assume that fragmented datagrams are rare. Consequently,
    our priority here is implementation simplicity (over efficiency).

--*/

{
    IpHeader* iphdr = GETPKTIPHDR(pkt);
    UINT iphdrlen = GETIPHDRLEN(iphdr);
    UINT iplen = GETIPLEN(iphdr);
    UINT proto_id, fragOffset, lastbyte;
    BOOL moreFlag;
    Packet** link;
    Packet* rapkt;
    ReassemblyHeader* rahdr;

    // Extract datagram header information
    fragOffset = NTOHS(iphdr->fragoffset);
    moreFlag = (fragOffset & MORE_FRAGMENTS) != 0;
    fragOffset &= FRAGOFFSET_MASK;

    proto_id = NTOHS(iphdr->id);
    proto_id = (proto_id << 16) | iphdr->protocol;
    link = IpFindReassemblyPkt(iphdr->srcaddr, iphdr->dstaddr, proto_id);
    
    // Check if we already have fragments from the same datagram.
    // If no existing fragments are found, then we need to
    // allocate a new reassembly buffer
    if ((rapkt = *link) == NULL) {
        if (IpReassemblyPktCount >= cfgMaxReassemblyDgrams) {
            WARNING_("Too many fragmented IP datagrams");
            goto discard_fragment;
        }

        rapkt = IpAllocReassemblyPkt(iphdr->srcaddr, iphdr->dstaddr, proto_id, pkt->recvifp);
        if (!rapkt) goto discard_fragment;
        *link = rapkt;
        IpReassemblyPktCount++;
    }

    if ((iplen -= iphdrlen) == 0) goto reassembly_failed;
    lastbyte = fragOffset*8 + iplen;
    if (lastbyte > cfgMaxReassemblySize) {
        WARNING_("Exceeded max reassembly size");
        goto reassembly_failed;
    }

    rahdr = GETPKTBUF(rapkt, ReassemblyHeader);
    if (moreFlag) {
        // Only the last fragment can have size that's not a multiple of 8.
        if (iplen % 8 != 0) goto reassembly_failed;
    } else {
        // Update the total length field for the entire datagram
        if (rahdr->origdatalen && rahdr->origdatalen != lastbyte)
            goto reassembly_failed;

        rahdr->origdatalen = lastbyte;
    }

    // Check if we got the first fragment.
    // Remember the original IP datagram header if we did.
    if (fragOffset == 0 && rapkt->iphdrOffset == 0) {
        BYTE* hdr = rapkt->data - iphdrlen;
        SETPKTIPHDR(rapkt, hdr);
        CopyMem(hdr, pkt->data, iphdrlen);
        rapkt->pktflags |= (pkt->pktflags & PKTFLAG_MCAST);
    }

    // Copy the fragment data into the reassembly buffer
    CopyMem(rapkt->data + fragOffset*8, pkt->data + iphdrlen, iplen);
    XnetCompletePacket(pkt, NETERR_OK);

    // Update the flags to indicate which fragments have already arrived.
    // If the datagram is complete, pass it upstream for processing.
    if (IpUpdateFragmentFlags(rapkt, fragOffset, (iplen+7)/8)) {
        *link = rapkt->nextpkt;
        IpReassemblyPktCount--;
        rapkt->nextpkt = NULL;

        iphdr = GETPKTIPHDR(rapkt);
        iphdrlen = GETIPHDRLEN(iphdr);
        rapkt->data = (BYTE*) iphdr;
        rapkt->datalen = iphdrlen + rahdr->origdatalen;
        iphdr->length = (WORD) HTONS(rapkt->datalen);
        iphdr->fragoffset &= ~HTONS(MORE_FRAGMENTS|FRAGOFFSET_MASK);
        COMPUTE_CHECKSUM(iphdr->hdrxsum, iphdr, iphdrlen);
        IpReceivePacket(rapkt);
    }
    return;

reassembly_failed:
    // We detected an error during the reassembly process.
    // Flush all associated fragments.
    *link = rapkt->nextpkt;
    IpReassemblyPktCount--;

    TRACE_("Datagram reassembly failed");
    XnetCompletePacket(rapkt, NETERR_REASSEMBLY);

discard_fragment:
    // Discard this fragment and return
    XnetCompletePacket(pkt, NETERR_DISCARDED);
}



PRIVATE BOOL
IpProcessRecvOptions(
    BYTE* buf,
    UINT buflen,
    IfInfo* ifp
    )

/*++

Routine Description:

    Process options in a received IP datagram

Arguments:

    buf - Points to the option data buffer
    buflen - Size of the buffer, in bytes
    ifp - The interface that received the datagram

Return Value:

    TRUE if the option data is valid
    FALSE if there is an error and the datagram should be discarded

--*/

{
    BYTE opt, optlen, ptr;
    IPADDR* paddr;

    while (buflen) {
        // reached the end of option list?
        if ((opt = *buf) == IPOPT_EOL) break;

        // skip over the NOP option
        if (opt == IPOPT_NOP) {
            buf++; buflen--;
            continue;
        }

        // check the option length field
        if (buflen < 2 || (optlen = buf[1]) < 2 || optlen > buflen)
            return FALSE;

        switch (opt) {
        case IPOPT_LOOSE_SRCROUTE:
        case IPOPT_STRICT_SRCROUTE:
            // loose or strict source and record route options
            if (optlen % 4 != 3 || (ptr = buf[2]) < 4 || ptr % 4 != 0)
                return FALSE;
            
            // reached the end of the IP address list?
            if (ptr > optlen) break;

            // only one address left and it's ours
            //  this shouldn't happen but we handle it anyway
            paddr = (IPADDR*) (buf+ptr-1);
            if (optlen - ptr == 3 && ifp->ipaddr && ifp->ipaddr == *paddr) {
                *paddr = 0;
                break;
            }

            // otherwise, discard the datagram since we don't
            // participate in source routing.
            return FALSE;

        case IPOPT_RECORD_ROUTE:
            // record route
            if (optlen % 4 != 3 || (ptr = buf[2]) < 4 || ptr % 4 != 0)
                return FALSE;
            break;

        case IPOPT_TIMESTAMP:
            // internet timestamp
            if (optlen < 4 || optlen % 4 != 0 ||
                (ptr = buf[2]) < 5 || ptr % 4 != 1)
                return FALSE;
            break;
        }

        // move on to the next option
        buf += optlen;
        buflen -= optlen;
    }

    return TRUE;
}


IPADDR
IpReflectIpOptions(
    IfInfo* ifp,
    IpHeader* iphdr,
    IPADDR dstaddr
    )

/*++

Routine Description:

    Reflect any IP options in a received datagram

Arguments:

    ifp - Specifies the outgoing interface
    iphdr - Points to the received datagram header
    dstaddr - Specifies the final destination address

Return Value:

    New destination address

Note:

    We assume the option data in the received datagram
    has already been validated.

--*/

{
    BYTE* buf;
    BYTE opt, optlen, ptr;
    UINT buflen;
    IPADDR* paddr;

    buf = (BYTE*) iphdr + IPHDRLEN;
    buflen = GETIPHDRLEN(iphdr) - IPHDRLEN;

    while (buflen) {
        if ((opt = *buf) == IPOPT_EOL) break;
        if (opt == IPOPT_NOP) {
            buf++; buflen--;
            continue;
        }

        optlen = buf[1];

        switch (opt) {
        case IPOPT_LOOSE_SRCROUTE:
        case IPOPT_STRICT_SRCROUTE:
            //
            // We assume the record route option contains the following:
            //  G1 G2 ... Gn
            // and we'll modify it to:
            //  Gn-1 ... G2 G1 D
            // and return Gn as the new destination address
            //
            if ((ptr = buf[2]) > 4) {
                IPADDR addr;
                IPADDR* pend;

                ASSERT(optlen % 4 == 3 && ptr % 4 == 0);
                if (ptr > optlen) ptr = (BYTE) (optlen+1);

                paddr = (IPADDR*) (buf+3);
                pend = (IPADDR*) (buf+ptr-5);

                // swap Gn and D
                addr = *pend;
                
                // simple sanity check
                //  if the gateway address is bad, we'll just
                //  leave the source route option alone
                if (!XnetIsValidUnicastAddr(addr)) break;

                *pend = dstaddr;
                dstaddr = addr;
                pend--;

                // flip G1 ... Gn-1
                while (pend > paddr) {
                    addr = *pend;
                    *pend = *paddr;
                    *paddr = addr;
                    paddr++;
                    pend--;
                }
                
                // reset ptr field
                buf[2] = 4;
            }
            break;

        case IPOPT_RECORD_ROUTE:
            //
            // If there is room left, record our own IP address
            //
            if ((ptr = buf[2]) > optlen) break;
            ASSERT(optlen % 4 == 3 && ptr >= 4 && ptr % 4 == 0);

            paddr = (IPADDR*) (buf+ptr-1);
            *paddr = ifp->ipaddr;
            buf[2] += 4;
            break;
        
        case IPOPT_TIMESTAMP:
            if ((ptr = buf[2]) > optlen) {
                // increment the overflow count if we don't have room
                if ((buf[3] & 0xf0) < 0xf0) buf[3] += 0x10;
            } else {
                UINT timestamp;
                DWORD* p;

                ASSERT(optlen % 4 == 0 && optlen > 4 &&
                       ptr >= 5 && ptr % 4 == 1);

                p = (DWORD*) (buf+ptr-1);
                timestamp = IpGetMsecsSinceMidnightGMT();

                switch (buf[3] & 0xf) {
                case 3:
                    if (*p != ifp->ipaddr) break;
                    // fall through

                case 1:
                    if (optlen-ptr < 7) break;
                    *p++ = ifp->ipaddr;
                    buf[2] += 4;
                    ptr += 4;
                    // fall through

                case 0:
                    if (optlen-ptr < 3) break;
                    *p = HTONL(timestamp);
                    buf[2] += 4;
                    break;
                }
            }
            break;
        }

        buf += optlen;
        buflen -= optlen;
    }

    return dstaddr;
}



VOID
IpReceivePacket(
    Packet* pkt
    )

/*++

Routine Description:

    Receive an IP datagram
    (called by the NIC interface driver)

Arguments:

    pkt - Points to the received packet

Return Value:

    NONE

--*/

{
    IpHeader* iphdr;
    IPADDR dstaddr, srcaddr;
    UINT iphdrlen, iplen;
    IfInfo* ifp;

    ASSERT_DISPATCH_LEVEL();

    //
    // NOTE:
    // 1. We assume the received packet is at least large enough
    //    to hold the default IP header.
    // 2. It's more efficient if the interface driver can manage
    //    to place the IP header at DWORD-aligned boundary.
    //    This is NOT currently the case for EPRO100 interface driver:
    //    the Ethernet frame header starts at DWORD-aligned address;
    //    which causes the IP header to start at odd-WORD boundary.
    //
    ASSERT(pkt->datalen >= IPHDRLEN);

    // verify header and packet length
    iphdr = (IpHeader*) pkt->data;
    SETPKTIPHDR(pkt, iphdr);
    iphdrlen = VERIFY_IPVER_HDRLEN(iphdr->ver_hdrlen);
    iplen = GETIPLEN(iphdr);

    if (iphdrlen < IPHDRLEN || iphdrlen > iplen || iplen > pkt->datalen)
        goto discard;

    // if the received packet length is larger than the IP length,
    // ignore the data at the end
    pkt->datalen = iplen;

    // verify header checksum
    if (tcpipxsum(0, pkt->data, iphdrlen) != 0xffff)
        goto discard;

    //
    // Determine if the packet is addressed to us.
    //
    // NOTE:
    //
    // 1. We reject a packet if the destination address doesn't
    //    match the address of the interface it's received on
    //    (i.e. we implement the strong ES model).
    //
    // 2. We don't check if a packet with an IP broadcast/multicast
    //    destination address was received as unicast on the link-layer
    //    interface. Converse, we don't check if a packet with a unicast
    //    IP destination address was received as broadcast/multicast
    //    on the link-layer interface.
    //
    // 3. We'll receive multicast packets even before the interface
    //    has a valid IP address.
    //
    // 4. We only support the all-1's form of broadcast address.
    //    Also, we only support limited broadcasts and subnet
    //    directed broadcasts (i.e. no special case for directed
    //    broadcasts and all-subnets directed broadcasts).
    //
    // 5. We discard all datagrams whose source address is
    //    a broadcast or multicast address.
    //
    ifp = pkt->recvifp;
    dstaddr = iphdr->dstaddr;
    
    if (dstaddr == ifp->ipaddr && ifp->ipaddr != 0 ||
        IS_MCAST_IPADDR(dstaddr) && IfFindMcastGroup(ifp, dstaddr) ||
        IfBcastAddr(ifp, dstaddr) ||
        IfLoopback(ifp))
    {
        srcaddr = iphdr->srcaddr;
        if (IfBcastAddr(ifp, srcaddr) ||
            IS_MCAST_IPADDR(srcaddr) ||
            IS_LOOPBACK_IPADDR(srcaddr) && !IfLoopback(ifp))
            goto discard;

        // Check if there is fragmentation involved
        if (iphdr->fragoffset & HTONS(MORE_FRAGMENTS|FRAGOFFSET_MASK)) {
            IpReassemblePkt(pkt);
            return;
        }

        // Process options if needed
        if (iphdrlen > IPHDRLEN &&
            !IpProcessRecvOptions(pkt->data+IPHDRLEN, iphdrlen-IPHDRLEN, ifp)) {
            // NOTE: we could send a parameter problem ICMP packet here.
            TRACE_("IP datagram option error");
            goto discard;
        }

        pkt->data += iphdrlen;
        pkt->datalen -= iphdrlen;

        switch (iphdr->protocol) {
        case IPPROTOCOL_TCP:
            TcpReceivePacket(pkt);
            break;

        case IPPROTOCOL_UDP:
            UdpReceivePacket(pkt);
            break;

        case IPPROTOCOL_ICMP:
            IcmpReceivePacket(pkt);
            break;

        case IPPROTOCOL_IGMP:
            IgmpReceivePacket(pkt);
            break;

        default:
            RawReceivePacket(pkt);
            break;
        }
        return;
    }
    
discard:
    // Discard the packet and return
    XnetCompletePacket(pkt, NETERR_DISCARDED);
}

