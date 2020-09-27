/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    mcast.c

Abstract:

    Implement multicast support on UDP and RAW sockets

Revision History:

    07/07/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Maximum of multicast groups that can be joined from a single socket
//
UINT cfgMaxSocketMcastGroups = 16;


VOID
PcbCleanupMcastData(
    PCB* pcb
    )

/*++

Routine Description:

    Cleanup multicast group membership data associated with the PCB

Arguments:

    pcb - Points to the protocol control block

Return Value:

    NONE

--*/

{
    PcbMcastData* pcbmcast;
    PcbMcastGroup* mcastgrp;
    UINT count;

    RUNS_AT_DISPATCH_LEVEL
    if ((pcbmcast = pcb->mcastData) == NULL) return;
    pcb->mcastData = NULL;

    // Drop all multicast groups that this socket belongs to
    mcastgrp = pcbmcast->mcastGroups;
    count = pcbmcast->groupCount;
    while (count--) {
        IfChangeMcastGroup(mcastgrp->ifp, mcastgrp->mcastaddr, FALSE);
        mcastgrp++;
    }

    SysFree(pcbmcast);
}


INLINE PcbMcastData*
PcbGetMcastData(
    PCB* pcb
    )

/*++

Routine Description:

    Allocate per-socket multicast group membership information

Arguments:

    pcb - Points to the protocol control block

Return Value:

    Points to the per-socket multicast information
    NULL if out of memory

--*/

{
    if (pcb->mcastData == NULL) {
        UINT size = offsetof(PcbMcastData, mcastGroups) +
                    sizeof(PcbMcastGroup) * cfgMaxSocketMcastGroups;
        pcb->mcastData = (PcbMcastData*) SysAlloc0(size, PTAG_MCAST);
    }

    return pcb->mcastData;
}


NTSTATUS
PcbSetMcastIf(
    PCB* pcb,
    IPADDR ifaddr
    )

/*++

Routine Description:

    Specifies the default interface for sending out multicast datagrams

Arguments:

    pcb - Points to the protocol control block
    ifaddr - Specifies the address of the interface designated
        for sending out multicast datagrams by default;
        use 0 to unset any existing multicast interface

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    KIRQL irql;
    IfInfo* ifp;
    PcbMcastData* pcbmcast;

    irql = RaiseToDpc();

    if ((pcbmcast = PcbGetMcastData(pcb)) != NULL) {
        if (ifaddr == 0) {
            RTE* rte = IpFindRTE(CLASSD_NETID, NULL);
            ifp = rte ? rte->ifp : NULL;
        } else {
            ifp = IfFindInterface(ifaddr);
        }

        if (ifp && IfMcastEnabled(ifp)) {
            CACHE_IFP_REFERENCE(pcbmcast->mcastIfp, ifp);
            pcbmcast->mcastIfAddr = ifaddr;
            status = NETERR_OK;
        } else {
            status = NETERR(WSAEADDRNOTAVAIL);
        }
    } else {
        status = NETERR_MEMORY;
    }

    LowerFromDpc(irql);
    return status;
}


IfInfo*
PcbGetMcastIf(
    PCB* pcb
    )

/*++

Routine Description:

    Return the default multicast interface for a socket

Arguments:

    pcb - Points to the protocol control block

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    if (!pcb->mcastData || !pcb->mcastData->mcastIfp) {
        ASSERT(!pcb->mcastData || pcb->mcastData->mcastIfAddr == 0);
        status = PcbSetMcastIf(pcb, 0);
        if (!NT_SUCCESS(status))
            return NULL;
    }

    return pcb->mcastData->mcastIfp;
}


NTSTATUS
PcbChangeMcastGroup(
    PCB* pcb,
    IPADDR mcastaddr,
    IPADDR ifaddr,
    BOOL add
    )

/*++

Routine Description:

    Join or leave a multicast group

Arguments:

    pcb - Points to the protocol control block
    mcastaddr - Specifies the multicast group address
    ifaddr - Specifies the interface address (0 means any)
    add - Whether to join or leave the group

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    PcbMcastData* pcbmcast;
    KIRQL irql = RaiseToDpc();

    pcbmcast = PcbGetMcastData(pcb);
    if (pcbmcast) {
        PcbMcastGroup* mcastgrp;
        UINT index;
        BOOL existing;

        // Check to see if the socket is already joined into
        // the specified group/interface combination.
        mcastgrp = pcbmcast->mcastGroups;
        for (index=0; index < pcbmcast->groupCount; index++, mcastgrp++) {
            if (mcastgrp->mcastaddr == mcastaddr &&
                mcastgrp->ifaddr == ifaddr)
                break;
        }

        existing = (index < pcbmcast->groupCount);
        if (add) {
            // Joining a group
            if (existing) {
                // The socket already belong to the specified group
                status = NETERR(WSAEADDRINUSE);
            } else if (pcbmcast->groupCount == cfgMaxSocketMcastGroups) {
                // Socket belongs to too many groups
                status = NETERR(WSAEADDRNOTAVAIL);
            } else {
                IfInfo* ifp;

                ifp = ifaddr ? IfFindInterface(ifaddr) : PcbGetMcastIf(pcb);
                if (ifp) {
                    status = IfChangeMcastGroup(ifp, mcastaddr, TRUE);
                    if (NT_SUCCESS(status)) {
                        mcastgrp->mcastaddr = mcastaddr;
                        mcastgrp->ifaddr = ifaddr;
                        CACHE_IFP_REFERENCE(mcastgrp->ifp, ifp);
                        pcbmcast->groupCount++;
                    }
                } else {
                    status = NETERR_UNREACHABLE;
                }
            }
        } else {
            // Leaving a group
            if (existing) {
                status = IfChangeMcastGroup(mcastgrp->ifp, mcastgrp->mcastaddr, FALSE);
                pcbmcast->groupCount--;
                while (index++ < pcbmcast->groupCount) {
                    *mcastgrp = *(mcastgrp+1);
                    mcastgrp++;
                }
            } else {
                status = NETERR_PARAM;
            }
        }
    } else {
        status = NETERR_MEMORY;
    }

    LowerFromDpc(irql);
    return status;
}


BOOL
PcbCheckMcastGroup(
    PCB* pcb,
    IPADDR mcastaddr
    )

/*++

Routine Description:

    Check if the socket is joined into the specified multicast group

Arguments:

    pcb - Points to the protocol control block
    mcastaddr - Specifies the multicast group address

Return Value:

    TRUE if the socket belongs to the specified multicast group
    FALSE otherwise

Note:

    We're not checking the receiving interface here.
    In theory, we could redundantly deliver a multicast
    datagram received from one interface to a socket that 
    belongs to the same multicast group on another interface.
    But such practice is discouraged. Besides, we'll only have
    one active interface other than the loopback interface.

--*/

{
    PcbMcastData* pcbmcast = pcb->mcastData;
    PcbMcastGroup* mcastgrp;
    UINT count;

    if (!pcbmcast) return FALSE;
    mcastgrp = pcbmcast->mcastGroups;
    count = pcbmcast->groupCount;
    while (count--) {
        if (mcastgrp->mcastaddr == mcastaddr) return TRUE;
        mcastgrp++;
    }

    return FALSE;
}


NTSTATUS
PcbSendMcastDgram(
    PCB* pcb,
    Packet* pkt,
    IPADDR dstaddr
    )

/*++

Routine Description:

    Send out a multicast datagram

Arguments:

    pcb - Points to the protocol control block
    pkt - Points to the packet to be sent
    dstaddr - Specifies the destination multicast address

Return Value:

    Status code

--*/

{
    IfInfo* ifp;

    RUNS_AT_DISPATCH_LEVEL

    // Send a copy of the multicast datagram to ourselves if needed
    if (!pcb->noMcastLoopback) {
        IpQueueLoopbackPacket(pkt, TRUE);
    }

    // Choose the outgoing interface if we have done so already
    if ((pcb->mcastData  == NULL || (ifp = pcb->mcastData->mcastIfp) == NULL) &&
        (ifp = PcbGetMcastIf(pcb)) == NULL) {
        XnetCompletePacket(pkt, NETERR_UNREACHABLE);
        return NETERR_UNREACHABLE;
    }

    // Check to see if the datagram size is too large
    if (pkt->datalen > ifp->mtu) {
        XnetCompletePacket(pkt, NETERR_MSGSIZE);
        return NETERR_MSGSIZE;
    }

    // Queue up the packet for transmission
    pkt->nexthop = dstaddr;
    IfEnqueuePacket(ifp, pkt);
    ifp->StartOutput(ifp);
    return NETERR_OK;
}

