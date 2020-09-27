/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    dbgmon.c

Abstract:

    Functions that are specific to the debug monitor stack

Revision History:

    08/04/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Debug monitor related global variables
//
PRIVATE IPRECEIVEPROC IpReceivePacketProc;
PRIVATE FREEPKTPROC FreePacketProc;
PRIVATE IfEnet *IfpEnetShared;

VOID
DmExFreePool(
    IN PVOID P
    );

//
// Pass the received frame up to the IP layer
//
VOID IPRECEIVEPACKET(Packet* pkt) {
    // If two stacks are present, we need to figure out
    // who's supposed to get this packet.
    if (IpReceivePacketProc) {
        IpHeader* iphdr;
        TcpHeader* tcphdr;
        UdpHeader* udphdr;
        UINT iphdrlen;

        // We assume the packet data length >= IPHDRLEN
        iphdr = GETPKTDATA(pkt, IpHeader);
        iphdrlen = GETIPHDRLEN(iphdr);
        tcphdr = (TcpHeader*) ((BYTE*) iphdr + iphdrlen);
        udphdr = (UdpHeader*) ((BYTE*) iphdr + iphdrlen);

        if (iphdrlen && 
            (iphdr->protocol == IPPROTOCOL_TCP &&
            iphdrlen+TCPHDRLEN <= pkt->datalen &&
            tcphdr->dstport == HTONS(DEBUGGER_PORT)) ||
            (iphdr->protocol == IPPROTOCOL_UDP &&
            iphdrlen+UDPHDRLEN <= pkt->datalen &&
            udphdr->dstport == HTONS(DEBUGGER_PORT))) {
            IpReceivePacket(pkt);
        } else {
            IpReceivePacketProc(pkt);
        }
    } else {
        IpReceivePacket(pkt);
    }
}


//
// Dispose of a packet after transmission
//
VOID COMPLETEPACKET(Packet* pkt, NTSTATUS status) {
    if (pkt->pktflags & PKTFLAG_DBGMON) {
        XnetCompletePacket(pkt, status);
    } else {
        // This packet came from the regular net stack.
        // We must use the callback function to dispose of it.
        if (pkt->completionCallback) {
            pkt->completionCallback(pkt, status);
        #ifdef DVTSNOOPBUG
        } else if (pkt->pktflags & PKTFLAG_UNCACHED) {
            XnetUncachedFreeProc(pkt);
        #endif
        } else if (!(pkt->pktflags & PKTFLAG_NETPOOL)) {
            // The packet memory came from the system pool
            DmExFreePool(pkt);
        } else if (FreePacketProc) {
            // The packet memory came from the private pool
            FreePacketProc(pkt);
        }
    }
}


//
// Cancel any pending transmissions that
// originated from the regular stack.
//
VOID DmCancelPendingPackets(PacketQueue* pktq) {
    PacketQueue tmpq;
    Packet* pkt;
    INT dropped = 0;
    
    PktQInit(&tmpq);
    while (!PktQIsEmpty(pktq)) {
        pkt = PktQRemoveHead(pktq);
        if (pkt->pktflags & PKTFLAG_DBGMON) {
            PktQInsertTail(&tmpq, pkt);
        } else {
            COMPLETEPACKET(pkt, NETERR_CANCELLED);
            dropped++;
        }
    }

    *pktq = tmpq;
    if (dropped) {
        WARNING_("Pending transmissions cancelled: %d", dropped);
    }
}


//
// Check to see if we should delete the enet interface
//
VOID IFENET_DELETE_CHECK(IfEnet* ifp) {
    ASSERT(ifp->refcount > 0);
    if (--ifp->refcount > 0) {
        //
        // Cancel any pending transmissions that
        // originated from the regular stack.
        //
        INT index;
        DmCancelPendingPackets(&ifp->sendq);
        NicWaitForXmitQEmpty(ifp);
        for (index=0; index < ARP_CACHE_SIZE; index++) {
            DmCancelPendingPackets(&ifp->arpCache[index].waitq);
        }

        IpReceivePacketProc = NULL;
        FreePacketProc = NULL;
    }
}


//
// Notify the debug monitor that regular netstack is starting up
//
PRIVATE IfEnet* EnetStartTitleStack(ENETINITPARAMS* initParams) {
    // NOTE: Wait until we acquired an address 
    // before starting the title stack.
    DhcpWaitForAddress((IfInfo*) IfpEnetShared);

    IpReceivePacketProc = initParams->IpReceiveProc;
    FreePacketProc = initParams->FreePktProc;
    initParams->UncachedAllocProc = XnetUncachedAllocProc;
    initParams->UncachedFreeProc = XnetUncachedFreeProc;
    IfpEnetShared->refcount++;
    return IfpEnetShared;
}



//
// Set the enet interface information in the process control block
//
VOID SET_DBGMON_ENETINFO(IfEnet* ifp) {
    IfpEnetShared = ifp;
    ASSERT(KeGetCurrentPrcb()->DmEnetFunc == 0);

    #pragma warning(disable:4054)
    KeGetCurrentPrcb()->DmEnetFunc = (VOID*) EnetStartTitleStack;
}


//
// Return the IP address of the Ethernet interface to the debugger
//
DWORD DmGetIpAddress(void) {
    return IfpEnetShared ? HTONL(IfpEnetShared->ipaddr) : 0;
}

