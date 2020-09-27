/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    enet.c

Abstract:

    Handle Ethernet frame transmission and reception

Revision History:

    05/04/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Ethernet broadcast address
//
const BYTE EnetBroadcastAddr[ENETADDRLEN] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

//
// IEEE LLC and SNAP headers for an Ethernet frame
//
const BYTE IeeeEnetHeaders[] = {
    SNAP_DSAP,      // DSAP - 0xaa
    SNAP_SSAP,      // SSAP - 0xaa
    LLC_DGRAM,      // control - 3
    0, 0, 0         // org code
};

//
// Maximum length of the transmit and receive queue.
//
UINT cfgXmitQLength = 8;
UINT cfgRecvQLength = 10;

//
// Enet system shutdown notification routine
//
PRIVATE BOOL EnetShutdownNotifyRegistered;
PRIVATE HAL_SHUTDOWN_REGISTRATION EnetShutdownNotification;


PRIVATE VOID
EnetTransmitPacket(
    IfEnet* ifp,
    Packet* pkt,
    const BYTE* dsthwaddr
    )

/*++

Routine Description:

    Transmit an IP datagram to the Ethernet interface

Arguments:

    ifp - Points to the interface structure
    pkt - Points to the packet to be transmitted
    dsthwaddr - Specifies the destination Ethernet hardware address

Return Value:

    NONE

--*/

{
    EnetFrameHeader* enethdr;

    //
    // We assume that the outgoing packet has enough free
    // space before the data buffer to hold the Ethernet frame header.
    //
    ASSERT(PktSpaceBefore(pkt) >= ENETHDRLEN);

    pkt->data -= ENETHDRLEN;
    pkt->datalen += ENETHDRLEN;

    // Now slap on the Ethernet frame header and send it out
    enethdr = GETPKTDATA(pkt, EnetFrameHeader);
    CopyMem(enethdr->dstaddr, dsthwaddr, ENETADDRLEN);
    CopyMem(enethdr->srcaddr, ifp->hwaddr, ENETADDRLEN);
    
    enethdr->etherType = HTONS(ENETTYPE_IP);
    NicTransmitFrame(ifp, pkt);
}


VOID
EnetReceiveFrame(
    IfEnet* ifp,
    Packet* pkt
    )

/*++

Routine Description:

    Process a received Ethernet frame

Arguments:

    ifp - Points to the interface structure
    pkt - Points to the received packet

Return Value:

    NONE

--*/

{
    EnetFrameHeader* enethdr;
    WORD etherType;

    // We assume the whole frame fits inside a single Packet
    // and the packet data length is validated by the NIC functions.
    // We also assume the Ethernet frame header starts on
    // a WORD-aligned boundary.
    ASSERT(pkt->datalen >= ENETHDRLEN + ENET_MINDATASIZE &&
           pkt->datalen <= ENETHDRLEN + ENET_MAXDATASIZE);

    // Peel off the Ethernet frame header
    enethdr = GETPKTDATA(pkt, EnetFrameHeader);
    pkt->data += ENETHDRLEN;
    pkt->datalen -= ENETHDRLEN;
    etherType = NTOHS(enethdr->etherType);

    // Peel off the IEEE 802.3 LLC and SNAP headers if necessary
    if (IsIEEE802Frame(etherType)) {
        IEEE802Header* ieeehdr = GETPKTDATA(pkt, IEEE802Header);
        pkt->data += IEEE802HDRLEN;
        pkt->datalen -= IEEE802HDRLEN;

        // If the IEEE frame wasn't meant for us, discard it.
        if (!EqualMem(ieeehdr, IeeeEnetHeaders, IEEE802HDRLEN))
            goto discard;

        etherType = NTOHS(ieeehdr->etherType);
    }

    if (etherType == ENETTYPE_IP) {
        // Was the frame received as a broadcast or multicast?
        if (IsEnetAddrMcast(enethdr->dstaddr))
            pkt->pktflags |= PKTFLAG_MCAST;

        // Pass the received frame up to the IP layer and return
        IPRECEIVEPACKET(pkt);
        return;
    }
    
    if (etherType == ENETTYPE_ARP) {
        if (EqualMem(enethdr->srcaddr, ifp->hwaddr, ENETADDRLEN)) {
            // If an ARP packet was receive and the source address
            // was the same as ours, then we have an Ethernet
            // address conflict. We assume we don't receive
            // our own transmissions.
            WARNING_("Ethernet address conflict: db %x!", enethdr);
            ASSERT(FALSE);
        } else {
            // Process incoming ARP packets
            ArpReceivePacket(ifp, pkt);
            return;
        }
    }

discard:
    // Ethernet frame wasn't recognized; quietly discard it.
    COMPLETEPACKET(pkt, NETERR_DISCARDED);
}


VOID
EnetStartOutput(
    IfEnet* ifp
    )

/*++

Routine Description:

    Start output on the Ethernet interface

Arguments:

    ifp - Points to the interface structure

Return Value:

    NONE

--*/

{
    Packet* pkt;
    ArpCacheEntry* arpEntry;
    NTSTATUS status;
    IPADDR nexthop;

    // Must be called at DISPATCH_LEVEL.
    ASSERT_DISPATCH_LEVEL();

    //
    // Remove the packet at the head of the output queue
    // NOTE: We need to make sure we don't flood the NIC's command queue.
    // 
    while (!PktQIsEmpty(&ifp->sendq) && !NicIsXmitQFull(ifp)) {
        pkt = IfDequeuePacket(ifp);

        nexthop = pkt->nexthop;
        if (IfBcastAddr(ifp, nexthop)) {
            // Broadcast packet
            // NOTE: We assume broadcast loopback is handled in the upper layer.
            if (IfUp(ifp)) {
                EnetTransmitPacket(ifp, pkt, EnetBroadcastAddr);
                continue;
            }
            status = NETERR_NETDOWN;
        } else if (IS_MCAST_IPADDR(nexthop)) {
            // Multicast packet
            // NOTE: We assume multicast loopback is handled in the upper layer.
            if (IfUp(ifp)) {
                BYTE mcastaddr[ENETADDRLEN];
                EnetGenerateMcastAddr(nexthop, mcastaddr);
                EnetTransmitPacket(ifp, pkt, mcastaddr);
                continue;
            }
            status = NETERR_NETDOWN;
        } else {
            // Unicast packet
            // Otherwise, resolve the nexthop IP address to Ethernet address
            if (!IfRunning(ifp)) {
                // Can't send unicast message if the interface is inactive
                status = NETERR_NETDOWN;
            } else if ((arpEntry = ArpFindCacheEntry(ifp, nexthop, RESOLVE_SEND_REQUEST)) == NULL) {
                status = NETERR_MEMORY;
            } else if (IsArpEntryOk(arpEntry)) {
                // Found a valid existing entry in the ARP cache
                // for the next hop address
                EnetTransmitPacket(ifp, pkt, arpEntry->enetaddr);
                continue;
            } else if (IsArpEntryBad(arpEntry)) {
                // Found an existing entry for the next hop address
                // but the entry indicates the target is unreachable.
                status = NETERR_UNREACHABLE;
            } else {
                // Created a new entry for the next hop address
                // or found an existing entry that's currently being resolved
                ASSERT(IsArpEntryBusy(arpEntry));
                PktQInsertTail(&arpEntry->waitq, pkt);
                continue;
            }
        }

        // Complete the packet with error status and move on
        VERBOSE_("Failed to send packet: 0x%x", status);
        COMPLETEPACKET(pkt, status);
    }
}


PRIVATE VOID
EnetDelete(
    IfEnet* ifp
    )

/*++

Routine Description:

    Delete the Ethernet interface

Arguments:

    ifp - Points to the interface structure

Return Value:

    NONE

--*/

{
    RUNS_AT_DISPATCH_LEVEL

    IFENET_DELETE_CHECK(ifp);
    if (ifp->refcount) return;

    // Unregister shutdown notification routine
    if (EnetShutdownNotifyRegistered) {
        HalRegisterShutdownNotification(&EnetShutdownNotification, FALSE);
        EnetShutdownNotifyRegistered = FALSE;
    }

    // Make sure the NIC is stopped
    ifp->flags &= ~(IFFLAG_UP|IFFLAG_RUNNING);
    NicReset(ifp, TRUE);

    // Cleanup the send queue
    while (!PktQIsEmpty(&ifp->sendq)) {
        Packet* pkt = PktQRemoveHead(&ifp->sendq);
        COMPLETEPACKET(pkt, NETERR_CANCELLED);
    }

    ArpCleanup(ifp);
    NicCleanup(ifp);
    SysFree(ifp);
}


PRIVATE VOID
EnetShutdownNotifyProc(
    HAL_SHUTDOWN_REGISTRATION* param
    )

/*++

Routine Description:

    Notification routine that's called during system shutdown

Arguments:

    param - Shutdown notification parameters

Return Value:

    NONE

--*/

{
    KIRQL irql = RaiseToDpc();
    IfEnet* ifp = (IfEnet*) LanIfp;

    // Make sure the NIC is stopped
    ifp->flags &= ~(IFFLAG_UP|IFFLAG_RUNNING);
    NicReset(ifp, TRUE);

    LowerFromDpc(irql);
}


PRIVATE VOID
EnetTimer(
    IfEnet* ifp
    )

/*++

Routine Description:

    Ethernet interface timer routine (called once a second)

Arguments:

    ifp - Points to the interface structure

Return Value:

    NONE

--*/

{
    if (IfUp(ifp)) {
        ArpTimerProc(ifp);
        NicTimerProc(ifp);
    }
}


PRIVATE NTSTATUS
EnetSetMcastAddrs(
    IfEnet* ifp
    )

/*++

Routine Description:

    Update the multicast filter on the NIC hardware

Arguments:

    ifp - Points to the interface structure

Return Value:

    Status code

--*/

{
    BYTE* addrs;
    UINT count;
    NTSTATUS status;

    RUNS_AT_DISPATCH_LEVEL

    count = ifp->mcastData->groupCount;
    if (count) {
        addrs = SysAlloc(count * ENETADDRLEN, PTAG_MCAST);
        if (addrs) {
            BYTE* p = addrs;
            IfMcastGroup* mcastgrp = ifp->mcastData->mcastGroups;
            UINT i;

            for (i=0; i < count; i++) {
                EnetGenerateMcastAddr(mcastgrp->mcastaddr, p);
                mcastgrp++;
                p += ENETADDRLEN;
            }
        } else {
            return NETERR_MEMORY;
        }
    } else {
        addrs = NULL;
    }

    status = NicSetMcastAddrs(ifp, addrs, count);
    SysFree(addrs);
    return status;
}


PRIVATE NTSTATUS
EnetIoctl(
    IfEnet* ifp,
    INT ctlcode,
    VOID* inbuf,
    UINT inlen,
    VOID* outbuf,
    UINT* outlen
    )

/*++

Routine Description:

    Ethernet interface I/O control function

Arguments:

    ifp - Points to the interface structure
    ctlcode - Control code
    inbuf - Points to the input buffer
    inlen - Size of the input buffer
    outbuf - Points to the output buffer
    outlen - On entry, this contains the size of the output buffer
        On return, this is the actually number of output bytes

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    IPADDR ipaddr;
    KIRQL irql = RaiseToDpc();

    switch (ctlcode) {
    case IFCTL_SET_MCAST_ADDRS:
        //
        // Send multicast addresses down to the NIC
        //
        status = EnetSetMcastAddrs(ifp);
        break;

    case IFCTL_CHECK_ADDR_CONFLICT:
        //
        // Check for IP address conflict
        //
        ASSERT(inlen == IPADDRLEN);

        ipaddr = *((IPADDR*) inbuf);
        ASSERT(XnetIsValidUnicastAddr(ipaddr));
        
        ifp->checkConflictAddr = ipaddr;
        status = ipaddr ? ArpSendRequest(ifp, ipaddr, ipaddr) : NETERR_OK;
        break;

    default:
        status = NETERR_NOTIMPL;
        break;
    }

    LowerFromDpc(irql);
    return status;
}


NTSTATUS
EnetInitialize(
    IfInfo** newifp
    )

/*++

Routine Description:

    Initialize the Ethernet interface.

Arguments:

    NONE

Return Value:

    Status code

--*/

{
    IfEnet* ifp;
    NTSTATUS status;

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    // Check if the debug monitor has already initialized the enet interface
    #pragma warning(disable:4055)
    CHECK_DBGMON_ENETINFO();

    #ifdef DVTSNOOPBUG
    status = XnetUncachedPoolInit();
    if (!NT_SUCCESS(status)) return NETERR_MEMORY;
    #endif

    // Allocate memory to hold our interface structure

    ifp = SysAlloc0(sizeof(IfEnet), PTAG_ENET);
    if (!ifp) return NETERR_MEMORY;
    ifp->refcount = 1;
    ifp->ifname = "Ethernet";
    ifp->magicCookie = 'TENE';

    // Interface functions
    ifp->Delete = (IfDeleteProc) EnetDelete;
    ifp->StartOutput = (IfStartOutputProc) EnetStartOutput;
    ifp->Timer = (IfTimerProc) EnetTimer;
    ifp->Ioctl = (IfIoctlProc) EnetIoctl;

    ifp->iftype = IFTYPE_ETHERNET;
    ifp->framehdrlen = ENETHDRLEN;
    ifp->mtu = ENET_MAXDATASIZE;

    // Initialize the NIC interface
    status = NicInitialize(ifp);
    if (!NT_SUCCESS(status)) goto failed;

    // Now mark the interface as fully initialized
    // and start handling interrupts.

    ifp->flags |= IFFLAG_UP |
                  IFFLAG_DHCP_ENABLED |
                  IFFLAG_BCAST |
                  IFFLAG_MCAST;

    *newifp = (IfInfo*) ifp;
    IfSetIpAddr(*newifp, 0, 0);

    // Set the enet interface information in the process control block
    SET_DBGMON_ENETINFO(ifp);

    // Register shutdown notification routine
    EnetShutdownNotification.NotificationRoutine = EnetShutdownNotifyProc;
    HalRegisterShutdownNotification(&EnetShutdownNotification, TRUE);
    EnetShutdownNotifyRegistered = TRUE;

    NicEnableInterrupt();
    return NETERR_OK;

failed:
    WARNING_("EnetInitialize failed: 0x%x", status);
    EnetDelete(ifp);
    return status;
}

