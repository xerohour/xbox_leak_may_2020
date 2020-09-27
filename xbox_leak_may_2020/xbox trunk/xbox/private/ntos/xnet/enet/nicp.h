/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    nicp.h

Abstract:

    Private header file for the NIC "driver" code

Revision History:

    01/24/2001 davidx
        Created it.

--*/

#ifndef _NICP_H
#define _NICP_H

//
// NIC data structure
//
typedef struct _NicInfo {
    PNIC_CSR CSR;               // points to NIC CSR structure
    ULONG csrSize;              // size of mapped CSR structure
    PacketQueue recvq;          // receive packet queue
    PacketQueue cmdq;           // NIC command queue
    UINT cmdqCount;             // # of entries in the command queue
    UINT cmdqWatchdog;          // command queue watchdog timer
    NicCmdBuffer cmdbuf;        // NIC command buffer
    KDPC dpc;                   // DPC object
    ULONG intrVector;           // interrupt vector
    KIRQL intrIrql;             // interrupt IRQ level
    DWORD rxpollFreq;           // XNIC Rx polling frequency
    DWORD linkStatus;           // last known link status
} NicInfo;

//
// Functions for managing DMA packet pool
//
// NOTE: The DMA packet size actually only needs to be as big as:
//      PKTHDRLEN + RECVPKT_OVERHEAD + ENETHDRLEN + ENET_MAXDATASIZE = 1546
// But we're have to use 2KB here to workaround an nVidia NIC hardware problem
// whether a receive buffer cannot cross physical page boundary.
//
#define DMAPKT_SIZE 2048
#define DMAPKT_MAXDATA (DMAPKT_SIZE - PKTHDRLEN - RECVPKT_OVERHEAD)

// Virtual and physical offset of the DMA packet pool
extern BYTE* NicPktPoolBase;
extern UINT_PTR NicPktPoolPhyAddrOffset;

INLINE Packet* NicPktAlloc(UINT index) {
    Packet* pkt = (Packet*) (NicPktPoolBase + index*DMAPKT_SIZE);
    ZeroMem(pkt, PKTHDRLEN);
    pkt->pktflags = PKTFLAG_DMA;
    pkt->data = pkt->buf;
    return pkt;
}

INLINE UINT_PTR NicPktGetPhyAddr(VOID* data) {
    return (UINT_PTR) data + NicPktPoolPhyAddrOffset;
}

INLINE NTSTATUS NicPktPoolInit(UINT count) {
    NicPktPoolBase = (BYTE*) NicAllocSharedMem(count * DMAPKT_SIZE);
    if (!NicPktPoolBase) return NETERR_MEMORY;
    NicPktPoolPhyAddrOffset = MmGetPhysicalAddress(NicPktPoolBase) - (UINT_PTR) NicPktPoolBase;
    return NETERR_OK;
}

INLINE VOID NicPktPoolCleanup() {
    if (NicPktPoolBase) {
        NicFreeSharedMem(NicPktPoolBase);
        NicPktPoolBase = NULL;
        NicPktPoolPhyAddrOffset = 0;
    }
}

#endif // !_NICP_H
