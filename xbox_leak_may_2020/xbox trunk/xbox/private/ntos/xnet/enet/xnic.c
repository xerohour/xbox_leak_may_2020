/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    nic.c

Abstract:

    XBox network controller "driver" code

Revision History:

    07/25/2000 davidx
        Created it.

--*/

#include "precomp.h"

#ifndef SILVER
#include <xboxp.h>

//
// NIC driver global variables
//
UINT NicXmitDescCount;
BYTE* NicPktPoolBase;
UINT_PTR NicPktPoolPhyAddrOffset;
KINTERRUPT NicIntrObject;
BOOL NicDontCopyReceivedPacket;

PRIVATE VOID NicInterruptDpc(PKDPC, PVOID, PVOID, PVOID);

// BUGBUG: temporary workaround for nv2a snooping bug
#ifdef DVTSNOOPBUG
BYTE NicTempRecvBuffer[DMAPKT_SIZE];
PRIVATE VOID NicTempRecvBufferPktCompletion(Packet* pkt, NTSTATUS status) {}
#endif

//
// NIC statistics
//
typedef struct _XNICSTATS {
    ULONG isrCount;
    ULONG dpcCount;
    ULONG txGoodFrames;
    ULONG rxGoodFrames;
    ULONG txStuckXmits;
    ULONG txUnderflowErrors;
    ULONG txLateCollisions;
    ULONG txLostCarriers;
    ULONG txDefers;
    ULONG txExcessiveDefers;
    ULONG txRetryErrors;
    ULONG rxFramingErrors;
    ULONG rxOverFlowErrors;
    ULONG rxCrcErrors;
    ULONG rxLengthErrors;
    ULONG rxMaxFrameErrors;
    ULONG rxLateCollisions;
    ULONG rxRunts;
    ULONG rxExtraByteErrors;
    ULONG rxMissedFrames;
    ULONG rxEndOfFrameErrors;
} XNICSTATS;
XNICSTATS NicStats;

//
// Lock and unlock the physical pages containing packet data
//
INLINE VOID NicLockPacketPages(Packet* pkt) {
    MmLockUnlockBufferPages(pkt->data, pkt->datalen, FALSE);
}

INLINE VOID NicUnlockPacketPages(Packet* pkt) {
    MmLockUnlockBufferPages(pkt->data, pkt->datalen, TRUE);
}

//
// Where the received Ethernet frame data starts
//
#define GetPktFrameData(pkt) (&(pkt)->buf[RECVPKT_OVERHEAD])

//
// Receive packet completion routine
//
PRIVATE VOID NicRecvBufferPktCompletion(Packet* pkt, NTSTATUS status) {
    pkt->nextpkt = NULL;
    pkt->data = GetPktFrameData(pkt);
    pkt->pktflags = PKTFLAG_DMA;
}


PRIVATE VOID
NicStopXmitRecv(
    IfEnet* nic,
    INT handleIntr
    )

/*++

Routine Description:

    Stop the controller from receiving and transmitting

Arguments:

    nic - Points to the NIC data structure
    handleIntr - Whether to handle the pending interrupts
        > 0 : disable and then handle interrupts
        = 0 : disable interrupts
        < 0 : leave interrupts alone

Return Value:

    NONE

--*/

{
    PNIC_CSR csr = nic->CSR;
    INT timeout;

    // Turn off the transmitter and receiver
    csr->rx_poll = 0;
    csr->rx_en = 0;
    csr->tx_en = 0;

    // Wait for a max of 5msec until both 
    // the transmitter and receiver are idle
    for (timeout=500; timeout--; ) {
        if (!(csr->rx_sta & RXSTA_BUSY) &&
            !(csr->tx_sta & TXSTA_BUSY))
            break;
        KeStallExecutionProcessor(10);
    }

    // Ensure there is no active DMA transfer in progress
    csr->mode = MODE_DISABLE_DMA;
    for (timeout=500; timeout--; ) {
        if (csr->mode & MODE_DMA_IDLE) break;
        KeStallExecutionProcessor(10);
    }
    csr->mode = 0;

    if (handleIntr >= 0) {
        // Disable interrupts and
        // handle any pending interrupts if requested
        NicDisableInterrupt();

        if (handleIntr > 0) {
            NicInterruptDpc(&nic->dpc, nic, nic, NULL);
        }
    }
}


//
// Start the transmitter and receiver
//
INLINE VOID NicStartXmitRecv(PNIC_CSR csr, DWORD rxpoll_freq) {
    csr->rx_poll = RXPOLL_EN | rxpoll_freq;
    csr->tx_en = TXEN_ENABLE;
    csr->rx_en = RXEN_ENABLE;
    csr->mode = MODE_TXDM;
}

PRIVATE VOID
NicProcessRecvInterrupt(
    IfEnet* nic
    )

/*++

Routine Description:

    Process receive interrupts

Arguments:

    nic - Points to the NIC data structure

Return Value:

    NONE

--*/

{
    NicBufDesc* bufdesc = nic->cmdbuf.rxptr;
    Packet* pkt;
    DWORD flags_count;

    // Looping until we ran into a receive descriptor
    // that's still owned by the controller
    while (!((flags_count = bufdesc->flags_count) & RXDESC_OWN)) {
        pkt = PktQRemoveHead(&nic->recvq);
        if (flags_count & RXDESC_REND) {
            //
            // NOTE: Workaround for hardware bug
            //  Framing errors are ignored.
            //
            if (!(flags_count & RXDESC_ERR) || (flags_count & RXDESC_FRAM)) {
                //
                // A packet was received successfully.
                // Pass it upstream for processing.
                //
                // Note: we expect the processing to be done
                // synchronously here and the upstream component
                // we'll call CompletePacket on our packet
                // inside the following call.
                //

                NicStats.rxGoodFrames++;
                pkt->datalen = flags_count & 0xffff;

                // NOTE: Workaround for hardware bug
                //  If extra byte flag is set, decrement the frame length by 1
                if (flags_count & RXDESC_FRAM) {
                    NicStats.rxFramingErrors++;
                    if (flags_count & RXDESC_EXTRA) {
                        NicStats.rxExtraByteErrors++;
                        pkt->datalen--;
                    }
                }

                // BUGBUG: workaround for nv2a hardware bug
                #ifndef DVTSNOOPBUG

                EnetReceiveFrame(nic, pkt);

                #else // DVTSNOOPBUG

                {
                    UINT len = pkt->datalen;
                    Packet* p = (Packet*) NicTempRecvBuffer;

                    p->nextpkt = NULL;
                    p->pktflags = p->iphdrOffset = 0;
                    p->data = p->buf;
                    p->datalen = len;
                    p->recvifp = (IfInfo*) nic;
                    p->completionCallback = NicTempRecvBufferPktCompletion;
                    memcpy(p->data, pkt->data, len);

                    NicRecvBufferPktCompletion(pkt, NETERR_OK);
                    EnetReceiveFrame(nic, p);
                }

                #endif // DVTSNOOPBUG
            } else {
                if (flags_count & RXDESC_OFOL) NicStats.rxOverFlowErrors++;
                if (flags_count & RXDESC_CRC ) NicStats.rxCrcErrors++;
                if (flags_count & RXDESC_LFER) NicStats.rxLengthErrors++;
                if (flags_count & RXDESC_MAX ) NicStats.rxMaxFrameErrors++;
                if (flags_count & RXDESC_LCOL) NicStats.rxLateCollisions++;
                if (flags_count & RXDESC_RUNT) NicStats.rxRunts++;
            }
        } else {
            NicStats.rxEndOfFrameErrors++;
        }

        // Give the ownership of the receive descriptor back to the NIC
        // And tell the receiver to check the receive descriptor ring
        ASSERT(bufdesc->phyaddr == MmGetPhysicalAddress(pkt->data));
        PktQInsertTail(&nic->recvq, pkt);
        bufdesc->flags_count = RXDESC_OWN | (DMAPKT_MAXDATA - 1);

        // Move on to the next receive descriptor
        if (++bufdesc == nic->cmdbuf.rxend)
            bufdesc = nic->cmdbuf.rxstart;
    }

    // Update the next receive descriptor pointer
    nic->cmdbuf.rxptr = bufdesc;
}


PRIVATE VOID
NicCheckMiiStatus(
    IfEnet* nic,
    DWORD mintr,
    BOOL init
    )

/*++

Routine Description:

    Check the PHY status

Arguments:

    nic - Points to the NIC data structure
    mintr - MII interrupt bits
    init - Whether this is the first call after reboot
        (Tx and Rx are currently stopped)

Return Value:

    NONE

--*/

{
    PNIC_CSR csr = nic->CSR;
    DWORD miics = csr->mii_cs;
    DWORD delta = nic->linkStatus ^ miics;

    if (init || ((mintr & MINTR_MAPI) && (delta & MIICS_LS) && (miics & MIICS_LS))) {
        DWORD linkState = PhyGetLinkState(!init);

        // BUGBUG: always dump out Ethernet link status for now
        #if DBG
        DbgPrint("Ethernet link status: %s %dMbps %s-duplex\n",
            (linkState & XNET_LINK_IS_UP) ? "up" : "down",
            (linkState & XNET_LINK_100MBPS) ? 100 :
                (linkState & XNET_LINK_10MBPS) ? 10 : 0,
            (linkState & XNET_LINK_FULL_DUPLEX) ? "full" :
                (linkState & XNET_LINK_HALF_DUPLEX) ? "half" : "?");
        #endif

        // NOTE: When the link was up before, we need to stop
        // both Tx and Rx and then set Rx polling frequency
        // and Tx duplex mode according to the link status.

        if (!init) {
            NicStopXmitRecv(nic, -1);
        }

        nic->rxpollFreq = (linkState & XNET_LINK_10MBPS) ?
                                RXPOLL_FREQ_10MPS :
                                RXPOLL_FREQ_100MPS;

        if (linkState & XNET_LINK_FULL_DUPLEX)
            csr->tx_cntl &= ~TXCNTL_HDEN;
        else
            csr->tx_cntl |= TXCNTL_HDEN;

        if (!init) {
            NicStartXmitRecv(csr, nic->rxpollFreq);
        }

        if (init && (linkState & XNET_LINK_IS_UP))
            nic->flags |= IFFLAG_CONNECTED_BOOT;
    }

    nic->linkStatus = miics;
}


//
// Check if transmit descriptor td2 is the last one used
// to transmit an Ethernet frame that started at td1.
//
INLINE BOOL IsEndXmitDesc(IfEnet* nic, NicBufDesc* td1, NicBufDesc* td2) {
    if (td1 == td2) return TRUE;
    if (td1->flags_count & TXDESC_TEND) return FALSE;
    if (++td1 == nic->cmdbuf.txend)
        td1 = nic->cmdbuf.txstart;
    return (td1 == td2);
}


PRIVATE BOOL
NicProcessXmitInterrupt(
    IfEnet* nic
    )

/*++

Routine Description:

    Process transmit interrupts

Arguments:

    nic - Points to the NIC data structure

Return Value:

    TRUE if we processed any transmit completion interrupts
    FALSE otherwise

--*/

{
    NicBufDesc* bufdesc;
    Packet* pkt;
    DWORD flags_count;
    NTSTATUS status;
    BOOL processed = FALSE;
    
    // Loop through uncompleted transmission queue
    while (pkt = nic->cmdq.head) {
        bufdesc = (NicBufDesc*) pkt->ifdata;
        if ((flags_count = bufdesc->flags_count) & TXDESC_OWN) break;

        processed = TRUE;
        PktQRemoveHead(&nic->cmdq);
        nic->cmdqCount--;
        nic->cmdqWatchdog = 0;
        NicUnlockPacketPages(pkt);

        ASSERT(flags_count & TXDESC_TEND);
        if (!(flags_count & TXDESC_ERR)) {
            NicStats.txGoodFrames++;
            status = NETERR_OK;
        } else {
            if (flags_count & TXDESC_UFLO) NicStats.txUnderflowErrors++;
            if (flags_count & TXDESC_LCOL) NicStats.txLateCollisions++;
            if (flags_count & TXDESC_LCAR) NicStats.txLostCarriers++;
            if (flags_count & TXDESC_DEF) NicStats.txDefers++;
            if (flags_count & TXDESC_EXDEF) NicStats.txExcessiveDefers++;
            if (flags_count & TXDESC_RTRY) NicStats.txRetryErrors++;

            status = NETERR_HARDWARE;
        }
        COMPLETEPACKET(pkt, status);

        // Update the uncompleted transmit descriptor pointer
        ASSERT(IsEndXmitDesc(nic, nic->cmdbuf.txtail, bufdesc));
        ASSERT((nic->cmdbuf.txtail->flags_count & TXDESC_OWN) == 0);
        bufdesc++;
        nic->cmdbuf.txtail = (bufdesc == nic->cmdbuf.txend) ? nic->cmdbuf.txstart : bufdesc;
    }

    return processed;
}


VOID
NicWaitForXmitQEmpty(
    IfEnet* nic
    )

/*++

Routine Description:

    Wait until the transmit queue is empty
    This is only called by the debug monitor when the regular net stack is unloaded.

Arguments:

    nic - Points to the NIC data structure

Return Value:

    NONE

--*/

{
    UINT timeout = 0;
    while (!PktQIsEmpty(&nic->cmdq)) {
        NicProcessXmitInterrupt(nic);
        KeStallExecutionProcessor(50);

        timeout++;
        ASSERT(timeout < 20000);
    }
}


PRIVATE VOID
NicInterruptDpc(
    PKDPC dpc,
    PVOID deferredContext,
    PVOID noReenable,
    PVOID arg2
    )

/*++

Routine Description:

    Ethernet interface interrupt service routine
    (runs at DISPATCH_LEVEL)

Arguments:

    dpc - Pointer to the DPC object
    deferredContext - Points to the NIC data structure
    noReenable - Leave interrupts disabled
    arg2 - Unused arguments

Return Value:

    NONE

--*/

{
    IfEnet* nic = (IfEnet*) deferredContext;
    PNIC_CSR csr = nic->CSR;

    NicStats.dpcCount++;
    while (TRUE) {
        DWORD mintr = csr->mintr;
        DWORD intr = csr->intr;

        // Acknowledge all pending interrupts
        //  note that we need to acknowledge MII interrupts first
        if (intr == 0) break;
        csr->mintr = mintr;
        csr->intr = intr;

        if (intr & INTR_MINT) {
            // Process MII interrupt
            NicCheckMiiStatus(nic, mintr, FALSE);
        }

        if (intr & (INTR_MISS|INTR_RCINT|INTR_REINT)) {
            // Process any received packets
            NicProcessRecvInterrupt(nic);
            if (intr & INTR_MISS) {
                csr->mode = MODE_RXDM;
                NicStats.rxMissedFrames++;
            }
        }

        if (intr & (INTR_TCINT|INTR_TEINT)) {
            // Process any completed transmissions
            NicProcessXmitInterrupt(nic);

            // If there is more room now in the command queue, we can
            // move some packets from the send queue to the command queue.
            if (!EnetIsSendQEmpty(nic) && nic->cmdqCount < cfgXmitQLength) {
                EnetStartOutput(nic);
            }
        }
    }

    // Reenable interrupts
    if (!noReenable) {
        NicEnableInterrupt();
    }
}


PRIVATE BOOLEAN
NicIsr(
    PKINTERRUPT interrupt,
    PVOID serviceContext
    )

/*++

Routine Description:

    Ethernet interrupt service routine (runs at DIRQL)

Arguments:

    interrupt - Interrupt object
    serviceContext - Points to the NIC data structure

Return Value:

    TRUE if the interrupt was handled
    FALSE if the interrupt wasn't generated by our device

--*/

{
    IfEnet* nic = (IfEnet*) serviceContext;

    NicStats.isrCount++;
    if ((nic->CSR->intr & nic->CSR->intr_mk) == 0)
        return FALSE;

    // Yep, this is ours:
    //  schedule the DPC routine for execution
    //  and disable further interrupts
    KeInsertQueueDpc(&nic->dpc, NULL, NULL);
    NicDisableInterrupt();
    return TRUE;
}


VOID
NicTimerProc(
    IfEnet* nic
    )

/*++

Routine Description:

    NIC timer procedure

Arguments:

    nic - Points to the NIC data structure

Return Value:

    NONE

--*/

{
    // If the transmitter is stuck, start it
    if (!PktQIsEmpty(&nic->cmdq) && nic->cmdqWatchdog++ > 2) {
        WARNING_("Transmitter is stuck.");
        nic->cmdqWatchdog = 0;
        NicStats.txStuckXmits++;

        NicProcessXmitInterrupt(nic);
        nic->CSR->mode = MODE_TXDM;
    }
}


VOID
NicTransmitFrame(
    IfEnet* nic,
    Packet* pkt
    )

/*++

Routine Description:

    Transmit an Ethernet frame to the NIC

Arguments:

    nic - Points to the NIC data structure
    pkt - Points to the frame to be transmitted
        We assume the Ethernet frame has been completed filled out.

Return Value:

    NONE

--*/

{
    UINT_PTR phyaddr0;
    UINT len, len0;
    NicBufDesc* bufdesc;

    // Make sure we have at least 2 transmit descriptors available
    len = (nic->cmdbuf.txtail <= nic->cmdbuf.txhead) ?
            (NicXmitDescCount - (nic->cmdbuf.txhead - nic->cmdbuf.txtail)) :
            (nic->cmdbuf.txtail - nic->cmdbuf.txhead);
    
    if (len <= 2) {
        WARNING_("Out of transmit descriptors.");
        COMPLETEPACKET(pkt, NETERR_MEMORY);
        return;
    }

    // Check the packet data size
    ASSERT(pkt->datalen != 0);
    if (pkt->datalen > ENETHDRLEN+ENET_MAXDATASIZE) {
        COMPLETEPACKET(pkt, NETERR_MSGSIZE);
        return;
    }

    NicLockPacketPages(pkt);

    // Since the total buffer size is <= 1500+14 bytes,
    // it can at most span two physical pages.
    len = pkt->datalen;
    phyaddr0 = MmGetPhysicalAddress(pkt->data);
    len0 = PAGE_SIZE - (phyaddr0 & (PAGE_SIZE-1));

    if (len <= len0) {
        // The buffer is in a single physical page.
        // We only need one transmit descriptor here.
        //
        // Notice the funny len-1 business below.
        // This is an odd requirement of the NIC.
        bufdesc = nic->cmdbuf.txhead;
        bufdesc->phyaddr = phyaddr0;
        bufdesc->flags_count = TXDESC_OWN | TXDESC_TEND | (len-1);
    } else {
        // The buffer straddles page boundary.
        // So we need two transmit descriptors.
        NicBufDesc* bufdesc0 = nic->cmdbuf.txhead;

        bufdesc = bufdesc0+1;
        if (bufdesc == nic->cmdbuf.txend)
            bufdesc = nic->cmdbuf.txstart;

        // NOTE: We're setting up the second transmit descriptor
        // before the first one. Otherwise, the NIC might use up
        // the first descriptor before we have the second one ready.
        bufdesc->phyaddr = MmGetPhysicalAddress(pkt->data + len0);
        bufdesc->flags_count = TXDESC_OWN | TXDESC_TEND | (len-len0-1);
        bufdesc0->phyaddr = phyaddr0;
        bufdesc0->flags_count = TXDESC_OWN | (len0-1);
    }

    pkt->ifdata = (UINT_PTR) bufdesc;
    PktQInsertTail(&nic->cmdq, pkt);
    nic->cmdqCount++;

    // Tell the transmitter to check the transmit descriptor ring
    nic->CSR->mode = MODE_TXDM;

    bufdesc++;
    nic->cmdbuf.txhead = (bufdesc == nic->cmdbuf.txend) ? nic->cmdbuf.txstart : bufdesc;
}


//
// Tell the NIC to not to receive any multicast frames
//
INLINE VOID NicRecvMcastNone(PNIC_CSR csr) {
    csr->mult_mk0 = 0;
    csr->mult_mk1 = 0;
    csr->mult0 = 1;
    csr->mult1 = 0;
}


NTSTATUS
NicSetMcastAddrs(
    IfEnet* nic,
    const BYTE* addrs,
    UINT count
    )

/*++

Routine Description:

    Send multicast addresses down to the NIC.

Arguments:

    nic - Points to the NIC data structure
    addrs - Points to an array of multicast addresses
    count - Specifies the number of multicast addresses

Return Value:

    Status code

--*/

// Converting Ethernet address from a sequence of bytes
// to two DWORDs that can be set into UNIn and MULTn registers
#define HWADDR0(_hwaddr) ((const DWORD*) (_hwaddr))[0]
#define HWADDR1(_hwaddr) ((const WORD*) (_hwaddr))[2]

{
    PNIC_CSR csr = nic->CSR;

    // Stop the controller from transmitting and receiving
    NicStopXmitRecv(nic, 1);

    if (count == 0) {
        // Don't receive any multicast frames
        NicRecvMcastNone(csr);
    } else {
        // Compute the multicast address filter values.
        // Notice that the filtering here is not perfect.
        // Exactly filtering is done inside IP receive function.

        DWORD andMask0 = -1;
        DWORD orMask0 = 0;
        WORD andMask1 = -1;
        WORD orMask1 = 0;

        while (count--) {
            andMask0 &= HWADDR0(addrs);
            orMask0  |= HWADDR0(addrs);
            andMask1 &= HWADDR1(addrs);
            orMask1  |= HWADDR1(addrs);
            addrs += ENETADDRLEN;
        }

        orMask0 = andMask0 | ~orMask0;
        orMask1 = andMask1 | ~orMask1;

        csr->mult0 = andMask0;
        csr->mult1 = andMask1;
        csr->mult_mk0 = orMask0;
        csr->mult_mk1 = orMask1;
    }

    // Restart transmitter and receiver
    NicStartXmitRecv(csr, nic->rxpollFreq);
    NicEnableInterrupt();
    return NETERR_OK;
}


VOID
NicReset(
    IfEnet* nic,
    BOOL disconnectIntr
    )

/*++

Routine Description:

    Reset the network controller:
        disable transmit and receive
        disable all interrupts
        clear any pending interrupt bits

Arguments:

    nic - Points to the NIC data structure
    disconnectIntr - Whether to disconnect interrupts

Return Value:

    NONE

--*/

{
    PNIC_CSR csr = nic->CSR;

    // Stop transmitter and receiver
    NicStopXmitRecv(nic, 0);
    
    // Reset buffer management
    csr->mode = MODE_RESET_BUFFERS;
    KeStallExecutionProcessor(10);
    csr->mode = 0;
    KeStallExecutionProcessor(10);

    csr->intr_mk = 0;
    csr->mintr_mk = 0;
    csr->pm_cntl = 0;
    csr->swtr_cntl = 0;
    csr->tx_poll = 0;
    csr->rx_poll = 0;

    csr->tx_sta = csr->tx_sta;
    csr->rx_sta = csr->rx_sta;
    csr->intr = csr->intr;
    csr->mintr = csr->mintr;

    if (disconnectIntr) {
        NicDisconnectInterrupt(nic);
    }
}


PRIVATE NTSTATUS
NicInitBuffers(
    IfEnet* nic
    )

/*++

Routine Description:

    Initialize transmit and receive buffer descriptors

Arguments:

    nic - Points to the NIC data structure

Return Value:

    Status code

--*/

{
    BYTE* buf;
    NicBufDesc* bufdesc;
    UINT index;
    Packet* pkt;
    NTSTATUS status;

    status = NicPktPoolInit(cfgRecvQLength);
    if (!NT_SUCCESS(status)) return status;
    
    // Allocate one contiguous physical page
    buf = (BYTE*) NicAllocSharedMem(PAGE_SIZE);
    if (!buf) return NETERR_MEMORY;
    
    ZeroMem(buf, PAGE_SIZE);
    nic->cmdbuf.phyaddrOffset = MmGetPhysicalAddress(buf) - (UINT_PTR) buf;

    // First half page is used for transmit buffer descriptors
    nic->cmdbuf.txstart = (NicBufDesc*) buf;
    nic->cmdbuf.txend = nic->cmdbuf.txstart + NicXmitDescCount;
    nic->cmdbuf.txhead = nic->cmdbuf.txtail = nic->cmdbuf.txstart;

    // Second half page is used for receive buffer descriptors
    nic->cmdbuf.rxstart = (NicBufDesc*) (buf + PAGE_SIZE / 2);
    nic->cmdbuf.rxend = nic->cmdbuf.rxstart + cfgRecvQLength;
    nic->cmdbuf.rxptr = nic->cmdbuf.rxstart;

    // Allocate receive buffers
    bufdesc = nic->cmdbuf.rxstart;
    for (index=0; index < cfgRecvQLength; index++) {
        pkt = NicPktAlloc(index);
        pkt->data = GetPktFrameData(pkt);
        pkt->recvifp = (IfInfo*) nic;
        XnetSetPacketCompletion(pkt, NicRecvBufferPktCompletion);
        PktQInsertTail(&nic->recvq, pkt);

        bufdesc->phyaddr = NicPktGetPhyAddr(pkt->data);
        bufdesc->flags_count = RXDESC_OWN | (DMAPKT_MAXDATA - 1);
        bufdesc++;
    }

    return NETERR_OK;
}


PRIVATE NTSTATUS
NicReadEnetAddr(
    IfEnet* nic
    )

/*++

Routine Description:

    Read permanent Ethernet address

Arguments:

    nic - Points to the NIC data structure

Return Value:

    Status code

--*/

{
    DWORD err;
    ULONG type, size, tick0;

    err = XQueryValue(XC_FACTORY_ETHERNET_ADDR, &type, nic->hwaddr, ENETADDRLEN, &size);
    if (err != ERROR_SUCCESS || size != ENETADDRLEN) {
        // NOTE:
        // If we failed to read Ethernet address from non-volatile memory,
        // pick a random address among the first 32 addresses of
        // the 00-50-f2 address block. This is so that we can at least boot
        // on the manufacturing line and start communicating with the test server.

        #ifdef DEVKIT
        DbgPrint("########## Invalid Ethernet address:\n");
        DbgPrint("  You must run the Recovery CD.\n");
        DbgPrint("  Defaulting to hardcoded Ethernet address...\n");
        #endif

        __asm {
            rdtsc
            mov tick0, eax
        }

        nic->hwaddr[0] = 0x00;
        nic->hwaddr[1] = 0x50;
        nic->hwaddr[2] = 0xf2;
        nic->hwaddr[3] = 0x00;
        nic->hwaddr[4] = 0x00;
        nic->hwaddr[5] = (BYTE) (tick0 & 0x1f);
    }

    nic->hwaddrlen = ENETADDRLEN;
    return NETERR_OK;
}


VOID
NicCleanup(
    IfEnet* nic
    )

/*++

Routine Description:

    Cleanup the NIC interface

Arguments:

    nic - Points to the NIC data structure

Return Value:

    NONE

--*/

{
    // Clean up the command queue and the shared command data buffer
    while (!PktQIsEmpty(&nic->cmdq)) {
        Packet* pkt = PktQRemoveHead(&nic->cmdq);
        NicUnlockPacketPages(pkt);
        COMPLETEPACKET(pkt, NETERR_CANCELLED);
    }

    // Clean up the receive buffer queue
    NicPktPoolCleanup();
    PktQInit(&nic->recvq);

    if (nic->cmdbuf.txstart) {
        NicFreeSharedMem(nic->cmdbuf.txstart);
        nic->cmdbuf.txstart = NULL;
    }

    NicDisconnectInterrupt(nic);
}


NTSTATUS
NicInitialize(
    IfEnet* nic
    )

/*++

Routine Description:

    Initialize the NIC "driver" code

Arguments:

    nic - Points to the NIC data structure

Return Value:

    Status code

--*/

{
    PNIC_CSR csr = NicCsr;
    NTSTATUS status = NETERR_HARDWARE;

    // NicXmitDescCount must be > 2*cfgXmitQLength and < PAGE_SIZE/(2*8)
    NicXmitDescCount = 2*cfgXmitQLength+1;
    if (NicXmitDescCount < 64) NicXmitDescCount = 64;
    ASSERT(NicXmitDescCount < PAGE_SIZE/(2*sizeof(NicBufDesc)));

    // Locate the NIC card and get the assigned resources
    nic->CSR = (PNIC_CSR) XPCICFG_NIC_MEMORY_REGISTER_BASE_0;
    nic->csrSize = XPCICFG_NIC_MEMORY_REGISTER_LENGTH_0;
    nic->intrVector = HalGetInterruptVector(XPCICFG_NIC_IRQ, &nic->intrIrql);

    KeInitializeDpc(&nic->dpc, NicInterruptDpc, nic);
    KeInitializeInterrupt(
        &NicIntrObject,
        NicIsr,
        nic,
        nic->intrVector,
        nic->intrIrql,
        LevelSensitive,
        TRUE);

    // Reset the NIC
    NicReset(nic, FALSE);

    // Initialize transmit and receive buffers
    status = NicInitBuffers(nic);
    if (!NT_SUCCESS(status)) goto err;

    // Read permanent Ethernet address
    status = NicReadEnetAddr(nic);
    if (!NT_SUCCESS(status)) goto err;

    nic->rxpollFreq = RXPOLL_FREQ_100MPS;
    csr->uni0 = HWADDR0(nic->hwaddr);
    csr->uni1 = HWADDR1(nic->hwaddr);
    
    // Disable multicast frame reception by default
    NicRecvMcastNone(csr);

    // Setup transmitter and receiver
    // NOTE: nVidia NIC somehow expects the maximum
    // receive buffer size is 1518 instead of 1514.
    ASSERT(DMAPKT_MAXDATA > 1518);
    csr->rx_cntl_1 = 1518;
    csr->rx_cntl_0 = RXCNTL_DEFAULT;
    csr->tx_cntl = TXCNTL_DEFAULT;

    csr->bkoff_cntl = BKOFFCNTL_DEFAULT;
    csr->tx_def = TXDEF_DEFAULT;
    csr->rx_def = RXDEF_DEFAULT;

    csr->tx_dadr = NicBufPhyAddr(nic, nic->cmdbuf.txhead);
    csr->rx_dadr = NicBufPhyAddr(nic, nic->cmdbuf.rxptr);
    csr->dlen = ((cfgRecvQLength-1) << 16) | (NicXmitDescCount-1);
    csr->rx_fifo_wm = RXFIFOWM_DEFAULT;
    csr->tx_fifo_wm = TXFIFOWM_DEFAULT;

    // Enable MII auto-polling interrupt
    csr->mii_cs = MIICS_DEFAULT;
    csr->mii_tm = MIITM_DEFAULT;
    csr->mintr_mk = MINTR_MAPI;

    // Initialize the PHY
    csr->mii_cs &= ~MIICS_APEN;
    status = PhyInitialize(FALSE, NULL);
    csr->mii_cs |= MIICS_APEN;
    if (!NT_SUCCESS(status)) goto err;

    NicCheckMiiStatus(nic, 0, TRUE);

    // Enable transmit and receive
    NicStartXmitRecv(csr, nic->rxpollFreq);

    // Connect the NIC interrupt
    if (KeConnectInterrupt(&NicIntrObject))
        return NETERR_OK;

    status = STATUS_BIOS_FAILED_TO_CONNECT_INTERRUPT;

err:
    NicReset(nic, FALSE);
    NicCleanup(nic);
    return status;
}

#endif // !SILVER

