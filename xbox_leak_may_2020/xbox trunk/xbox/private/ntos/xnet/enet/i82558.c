/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    82558.c

Abstract:

    Intel 82558 NIC related functions

Revision History:

    05/09/2000 davidx
        Created it.

    01/24/2000 davidx
        Moved into the kernel.

Note:

    We assume every NIC function here runs at DISPATCH_LEVEL
    unless noted otherwise.

--*/

#include "precomp.h"

#ifdef SILVER
#include <pci.h>


//
// NIC driver global variables
//
PNIC_CSR NicCsr;
BYTE* NicPktPoolBase;
UINT_PTR NicPktPoolPhyAddrOffset;
KINTERRUPT NicIntrObject;

// Forward declaration of local functions
PRIVATE VOID NicProcessRecvInterrupt(IfEnet* nic);
PRIVATE BOOL NicProcessXmitInterrupt(IfEnet* nic, BYTE statAck);

//
// Interlocked bit-wise AND and OR operations
//
INLINE __declspec(naked) VOID __fastcall InterlockedAND(DWORD* p, DWORD mask) {
    __asm and DWORD PTR [ecx], edx
    __asm ret
}

INLINE __declspec(naked) VOID __fastcall InterlockedOR(DWORD* p, DWORD mask) {
    __asm or DWORD PTR [ecx], edx
    __asm ret
}

//
// For an outgoing packet, we store the command block information
// associated with the packet in the Packet.ifdata field.
//  high word: offset to where the command block starts
//  low word: command block size
//
#define SetPktCmdBlk(nic, pkt, cmdblk, cmdsize) \
        (pkt)->ifdata = ((cmdsize) | (((BYTE*) (cmdblk) - (nic)->cmdbuf.start) << 16))

#define GetPktCmdBlk(nic, pkt) \
        ((ActionCmdBlock*) ((nic)->cmdbuf.start + ((pkt)->ifdata >> 16)))

#define GetPktCmdBlkSize(nic, pkt) ((pkt)->ifdata & 0xffff)

//
// For a received packet, the packet content after the the packet header is:
//  RecvFrameDesc - receive frame descriptor
//  Ethernet frame header
//  frame data
//
#define GetPktRFD(pkt) GETPKTBUF(pkt, RecvFrameDesc)
#define GetPktRFDPhy(pkt) NicPktGetPhyAddr((pkt)->buf)

//
// Lock and unlock the physical pages containing packet data
//
INLINE VOID NicLockPacketPages(Packet* pkt) {
    if (pkt->datalen) {
        MmLockUnlockBufferPages(pkt->data, pkt->datalen, FALSE);
    }
}

INLINE VOID NicUnlockPacketPages(Packet* pkt) {
    if (pkt->datalen) {
        MmLockUnlockBufferPages(pkt->data, pkt->datalen, TRUE);
    }
}


PRIVATE NTSTATUS
NicWaitScb(
    PNIC_CSR csr
    )

/*++

Routine Description:

    Wait for the low SCB command byte to be clear
    (before we can issue CU or RU commands)

Arguments:

    csr - Points to the 82558 CSR registers

Return Value:

    Status code

--*/

{
    // BOGUS: not sure where w2k driver got this 600ms number
    //  which seems an awfully long time to wait
    UINT timeout = 60000;

    while (timeout--) {
        if (csr->cucRuc == 0) return NETERR_OK;
        KeStallExecutionProcessor(10);
    }

    WARNING_("NicWaitScb failed!");
    return NETERR_HARDWARE;
}


PRIVATE NTSTATUS
NicExecuteActionCmdAndWait(
    IfEnet* nic,
    ActionCmdBlock* cmdblk,
    INT opcode
    )

/*++

Routine Description:

    Execute an action command and wait for its completion

Arguments:

    nic - Points to the NIC data structure
    cmdblk - Points to the action command data block
    opcode - Specifies the command opcode

Return Value:

    Status code

Note:

    This function should only be called when the command unit is idle
    and interrupts are disabled. It issues the command and busy-wait
    for the command to complete. It's intended to be called during
    initialization.

--*/

{
    NTSTATUS status;
    PNIC_CSR csr = nic->CSR;
    UINT timeout;

    // Wait until the command unit is ready to accept
    // a new command and then issue the command.
    cmdblk->cmdstatus = opcode | CMDFLAG_EL;
    cmdblk->link = LINK_OFFSET_NULL;
    status = NicWaitScb(csr);
    if (!NT_SUCCESS(status)) return status;

    csr->scbGeneralPtr = CmdBufferGetPhyAddr(nic, cmdblk);
    csr->cucRuc = CUC_START;

    // Now wait for command completion
    //  BOGUS: seems to be an extremely long timeout period (3sec)
    timeout = 300000;
    while (!(cmdblk->cmdstatus & CMDSTATUS_C) && timeout--) {
        KeStallExecutionProcessor(10);
    }

    // Acknowledge any pending interrupts
    csr->statAck = csr->statAck;

    if (cmdblk->cmdstatus & CMDSTATUS_OK) return NETERR_OK;

    WARNING_("NicExecuteActionCmdAndWait failed!");
    return NETERR_HARDWARE;
}


PRIVATE NTSTATUS
NicDoIASetupCmd(
    IfEnet* nic
    )

/*++

Routine Description:

    Configure the individual address

Arguments:

    nic - Points to the NIC data structure

Return Value:

    Status code

--*/

{
    // We assume the CU is idle and we can use
    // the entire shared command data buffer.
    IASetupCmdBlock* cmdblk = (IASetupCmdBlock*) nic->cmdbuf.start;

    CopyMem(cmdblk->hwaddr, nic->hwaddr, ENETADDRLEN);
    return NicExecuteActionCmdAndWait(nic, (ActionCmdBlock*) cmdblk, ACTIONCMD_IA_SETUP);
}


PRIVATE NTSTATUS
NicDoConfigureCmd(
    IfEnet* nic
    )

/*++

Routine Description:

    Configure the default 82558 operating parameters

Arguments:

    nic - Points to the NIC data structure

Return Value:

    Status code

--*/

{
    // These values are copied directly from w2k driver.
    static const BYTE defaultParams[CONFIG_PARAM_COUNT] = {
        CONFIG_PARAM_COUNT,
        0x88, // 1: Tx and Rx FIFO limits
        0x00, // 2: no adaptive IFS
        0x01, // 3: MWI enable
        0x00, // 4: Rx DMA max count
        0x00, // 5: Tx DMA max count
        0x32, // 6: discard bad frames, extended stats, CNA intr, non-direct recv DMA
        0x03, // 7: 1 underrun retry, discard short frames
        0x01, // 8:
        0x00, // 9:
        0x2e, // 10: no loopback, 7 bytes preamble, no SA insertion
        0x00, // 11:
        0x60, // 12: inter-frame spacing (IFS) = 96 bit time
        0x00, // 13:
        0xf2, // 14:
        0xc8, // 15:
        0x00, // 16: 
        0x40, // 17:
        0xf2, // 18: padding enabled
        0x80, // 19: auto full-duplex
        0x3f, // 20:
        0x05  // 21:
    };

    // We assume the CU is idle and we can use
    // the entire shared command data buffer.
    ConfigCmdBlock* cmdblk = (ConfigCmdBlock*) nic->cmdbuf.start;

    CopyMem(cmdblk->params, defaultParams, CONFIG_PARAM_COUNT);
    return NicExecuteActionCmdAndWait(nic, (ActionCmdBlock*) cmdblk, ACTIONCMD_CONFIGURE);
}


PRIVATE NTSTATUS
NicCmdBufferInit(
    IfEnet* nic
    )

/*++

Routine Description:

    Initialize the shared command data buffer

Arguments:

    nic - Points to the NIC data structure

Return Value:

    Status code

--*/

{
    BYTE* buf;

    // Allocate a page and get its physical address
    buf = (BYTE*) NicAllocSharedMem(PAGE_SIZE);
    if (!buf) return NETERR_MEMORY;

    nic->cmdbuf.head = nic->cmdbuf.tail = nic->cmdbuf.start = buf;
    nic->cmdbuf.last = buf + PAGE_SIZE;
    nic->cmdbuf.phyaddrOffset = MmGetPhysicalAddress(buf) - (UINT_PTR) buf;

    return NETERR_OK;
}


PRIVATE NTSTATUS
NicStartCommandUnit(
    IfEnet* nic,
    ActionCmdBlock* cmdblk
    )

/*++

Routine Description:

    Start the 82558 CU (command unit) if it's idle

Arguments:

    nic - Points to the NIC data structure
    cmdblk - Points to the action command data block

Return Value:

    Status code

--*/

{
    PNIC_CSR csr = nic->CSR;
    INT cus = SCB_STAT_CUS(csr->cusRus);
    NTSTATUS status;

    // CU is active, do nothing
    if (cus != CUS_IDLE || (cmdblk->cmdstatus & CMDSTATUS_C))
        return NETERR_OK;

    // Wait for SCB and then issue CUC_START command
    status = NicWaitScb(csr);
    if (NT_SUCCESS(status)) {
        csr->scbGeneralPtr = CmdBufferGetPhyAddr(nic, cmdblk);
        csr->cucRuc = CUC_START;
    }
    return status;
}


PRIVATE VOID
NicEnqueueRecvBuffer(
    IfEnet* nic,
    Packet* pkt
    )

/*++

Routine Description:

    Queue up a packet in the receive queue

Arguments:

    nic - Points to the NIC data structure
    pkt - Points to the packet to be queue

Return Value:

    NONE

Notes:

    We maintain a queue of buffers to try to always keep
    the 82558 receive unit (RU) ready. The buffers are
    allocated for DMA transfer.

    After a packet is received, we pass it up to upper
    layer protocols. Those protocols are expected to
    process the packet as soon as possible and return
    the packet back to us with a CompletePacket call.

    When a packet is returned to us by the upper layer
    protocols, we'll put the packet back to the receive
    queue (unless we already have enough packets in the queue).

--*/

{
    Packet* q;
    RecvFrameDesc* rfd;

    //
    // Initialize the packet header fields
    //
    rfd = GetPktRFD(pkt);
    pkt->nextpkt = NULL;
    pkt->pktflags = PKTFLAG_DMA;
    pkt->data = (BYTE*) (rfd+1);
    pkt->datalen = 0;

    //
    // Initialize the RFD structure, which is stored in the first
    // 16 bytes of the packet buffer is used to store the RFD structure.
    // Note that we always use the simple mode.
    //
    rfd->cmdstatus = CMDFLAG_EL;
    rfd->rbdAddr = rfd->link = LINK_OFFSET_NULL;
    rfd->size = ENETHDRLEN + ENET_MAXDATASIZE;
    rfd->actualCount = 0;

    // Append the packet to the receive queue
    q = nic->recvq.tail;
    PktQInsertTail(&nic->recvq, pkt);

    if (q) {
        // Chain the previous RFD to the current one
        // and clear its EOL flag bit.
        rfd = GetPktRFD(q);
        rfd->link = GetPktRFDPhy(pkt);
        InterlockedAND(&rfd->cmdstatus, ~CMDFLAG_EL);
    }
}


PRIVATE VOID
NicRecvBufferPktCompletion(
    Packet* pkt,
    NTSTATUS status
    )

/*++

Routine Description:

    Receive packet completion routine

Arguments:

    pkt - Points to the packet being completed
    status - Completion status

Return Value:

    NONE

--*/

{
    // Insert the packet back into the receive queue
    NicEnqueueRecvBuffer((IfEnet*) pkt->recvifp, pkt);
}

    
PRIVATE NTSTATUS
NicRecvBufferInit(
    IfEnet* nic
    )

/*++

Routine Description:

    Initialize the receive packet queue

Arguments:

    nic - Points to the NIC data structure

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    UINT index;
    Packet* pkt;
    
    status = NicPktPoolInit(cfgRecvQLength);
    if (!NT_SUCCESS(status)) {
        WARNING_("Couldn't allocate DMA receive buffers.");
        return status;
    }

    for (index=0; index < cfgRecvQLength; index++) {
        pkt = NicPktAlloc(index);
        pkt->recvifp = (IfInfo*) nic;
        XnetSetPacketCompletion(pkt, NicRecvBufferPktCompletion);
        NicEnqueueRecvBuffer(nic, pkt);
    }

    return NETERR_OK;
}


PRIVATE NTSTATUS
NicStartReceiveUnit(
    IfEnet* nic
    )

/*++

Routine Description:

    Start the 82558 RU (receive unit). We assume the RU is
    not in the ready state when this function is called.

Arguments:

    nic - Points to the NIC data structure

Return Value:

    Status code

--*/

{
    PNIC_CSR csr = nic->CSR;
    NTSTATUS status = NETERR_OK;

    while (SCB_STAT_RUS(csr->cusRus) != RUS_READY) {
        Packet* pkt = nic->recvq.head;

        if (GetPktRFD(pkt)->cmdstatus & CMDSTATUS_C) {
            // If the head of the receive buffer queue is completed
            // but not yet processed, process it first.
            NicProcessRecvInterrupt(nic);
        } else {
            UINT timeout;

            // Wait for SCB and then issue the RUC_START command
            status = NicWaitScb(csr);
            if (NT_SUCCESS(status)) {
                ASSERT(SCB_STAT_RUS(csr->cusRus) != RUS_READY);
                csr->scbGeneralPtr = GetPktRFDPhy(pkt);
                csr->cucRuc = RUC_START;

                // Wait until the RU to accept the command
                // BOGUS: the 80000 number came from w2k driver
                NicWaitScb(csr);
                timeout = 80000;
                while (timeout && SCB_STAT_RUS(csr->cusRus) != RUS_READY) {
                    timeout--;
                    KeStallExecutionProcessor(10);
                }

                if (timeout == 0)
                    status = NETERR_HARDWARE;
            }

            if (!NT_SUCCESS(status)) {
                WARNING_("Failed to start receiveer.");
            }

            break;
        }
    }

    return status;
}


PRIVATE WORD
NicReadEEPROM(
    volatile WORD* eepromCtrl,
    INT addr
    )

/*++

Routine Description:

    Read a WORD out of the 82558 EEPROM

Arguments:

    eepromCtrl - Address of the EEPROM control register
    addr - EEPROM location to be read

Return Value:

    WORD value read out of the EEPROM

--*/

// Wait for about 100us after toggling signals to the EEPROM
//  BOGUS: not sure why w2k driver picked this 100us number
#define WaitEEPROM() KeStallExecutionProcessor(100)

// Toggle EEPROM SK bit
#define RaiseEEPROMClock() \
        reg |= EEPROM_SK; \
        *eepromCtrl = reg; \
        WaitEEPROM()

#define LowerEEPROMClock() \
        reg &= ~EEPROM_SK; \
        *eepromCtrl = reg; \
        WaitEEPROM()

{
    INT bit;
    WORD reg, result;

    // start bit + opcode + address
    addr = (1 << 8) | (EEPROMOP_READ << 6) | (addr & 0x3f);

    //
    // Set EEPROM CS bit
    //
    reg = *eepromCtrl;
    reg &= ~(EEPROM_SK | EEPROM_DI | EEPROM_DO);
    reg |= EEPROM_CS;
    *eepromCtrl = reg;

    //
    // Shift out 9 bits - from MSB to LSB
    //
    for (bit=8; bit >= 0; bit--) {
        // Write EEPROM DI bit
        if (addr & (1 << bit))
            reg |= EEPROM_DI;
        else
            reg &= ~EEPROM_DI;
        *eepromCtrl = reg;
        WaitEEPROM();

        // Toggle EEPROM SK bit
        RaiseEEPROMClock();
        LowerEEPROMClock();
    }

    // EEPROM DO bit must be 0 at this point

    //
    // Read out 16 data bits
    //
    result = 0;
    reg &= ~EEPROM_DI;

    for (bit=0; bit < 16; bit++) {
        // Raise EEPROM SK bit
        // then read EEPROM DO bit
        // finally lower EEPROM SK bit

        RaiseEEPROMClock();

        result = (WORD) (result << 1);
        if (*eepromCtrl & EEPROM_DO)
            result |= 1;

        LowerEEPROMClock();
    }

    //
    // Deassert EEPROM CS bit
    //  BOGUS: not sure why we need to toggle the clock bit here
    //
    reg &= ~EEPROM_CS;
    *eepromCtrl = reg;
    RaiseEEPROMClock();
    LowerEEPROMClock();

    return result;
}


PRIVATE NTSTATUS
NicReadEnetAddr(
    IfEnet* nic
    )

/*++

Routine Description:

    Read permanent Ethernet address from the EEPROM

Arguments:

    nic - Points to the NIC data structure

Return Value:

    Status code

--*/

{
    volatile WORD* eepromCtrl;
    WORD* addr;

    eepromCtrl = &nic->CSR->eepromCtrl;
    addr = (WORD*) nic->hwaddr;

    addr[0] = NicReadEEPROM(eepromCtrl, 0);
    addr[1] = NicReadEEPROM(eepromCtrl, 1);
    addr[2] = NicReadEEPROM(eepromCtrl, 2);

    nic->hwaddrlen = ENETADDRLEN;
    return NETERR_OK;
}


VOID
NicReset(
    IfEnet* nic,
    BOOL disconnectIntr
    )

/*++

Routine Description:

    Reset the NIC (stop both command and receive units)
    Also disconnects the NIC interrupts.

Arguments:

    nic - Points to the NIC data structure
    disconnectIntr - Whether to disconnect NIC interrupts

Return Value:

    NONE

--*/

{
    if (!nic->CSR) return;

    nic->CSR->port = PORTCMD_SELECTIVE_RESET;
    KeStallExecutionProcessor(20);
    NicDisableInterrupt();

    // Clear pending interrupts
    nic->CSR->statAck = nic->CSR->statAck;

    if (disconnectIntr) {
        NicDisconnectInterrupt(nic);
    }
}


VOID
NicTimerProc(
    IfEnet* nic
    )

/*++

Routine Description:

    NIC interface timer routine

Arguments:

    nic - Points to the NIC data structure

Return Value:

    NONE

--*/

{
    BYTE cusRus = nic->CSR->cusRus;

    // NOTE:
    //  We shouldn't have to do any of the following
    //  under normal operations. Just being paranoid here.

    // If RU is idle, start it
    if (SCB_STAT_RUS(cusRus) != RUS_READY) {
        NicProcessRecvInterrupt(nic);
        NicStartReceiveUnit(nic);
    }

    // If CU is stuck, start or resume it
    if (!PktQIsEmpty(&nic->cmdq) && nic->cmdqWatchdog++ > 2) {
        Packet* pkt = nic->cmdq.head;
        ActionCmdBlock* cmdblk = GetPktCmdBlk(nic, pkt);

        WARNING_("Transmitter is stuck.");
        nic->cmdqWatchdog = 0;

        if ((cmdblk->cmdstatus & CMDSTATUS_C) || SCB_STAT_CUS(cusRus) != CUS_IDLE) {
            nic->CSR->cucRuc = CUC_RESUME;
        } else {
            NicStartCommandUnit(nic, cmdblk);
        }
    }
}


PRIVATE VOID*
NicCmdBufferReserve(
    IfEnet* nic,
    UINT count
    )

/*++

Routine Description:

    Reserve the requested number of bytes in the shared command buffer

Arguments:

    nic - Points to the NIC data structure
    count - Specifies the desired number of bytes

Return Value:

    Pointer to the first reserved byte in the command buffer
    NULL if the request cannot be satisfied (no space left)

--*/

{
    BYTE* addr;
    UINT freeCount;
    INT reclaim = 0;

    // We always allocate in 8-byte chunks
    ASSERT(count % CMDBUF_ALLOC_UNIT == 0);

retry: 

    if (nic->cmdbuf.tail >= nic->cmdbuf.head) {
        // we have the following case:
        //       head        tail
        //  |    xxxxxxxxxxxxx   |

        // first check to see if we have enough room at the end of the buffer
        if (count <= (freeCount = nic->cmdbuf.last - nic->cmdbuf.tail)) {
            // yes, reserve space at the end
            if (nic->cmdbuf.head > nic->cmdbuf.start) freeCount++;
            addr = nic->cmdbuf.tail;
        } else {
            freeCount = nic->cmdbuf.head - nic->cmdbuf.start;
            addr = nic->cmdbuf.start;
        }
    } else {
        // we have the following case:
        //      tail       head
        //  |xxxx           xxxxx|

        freeCount = nic->cmdbuf.head - nic->cmdbuf.tail;
        addr = nic->cmdbuf.tail;
    }

    //
    // If there is enough room, then return success.
    // Otherwise, try to process any pending CU interrupts
    // and reclaim the shared command buffer space.
    //
    if (count < freeCount) return addr;
    if (reclaim++ == 0 && NicProcessXmitInterrupt(nic, 0)) goto retry;

    WARNING_("Failed to allocate command buffer space.");
    return NULL;
}

//
// Commit the command buffer space previously reserved by NicCmdBufferReserve.
// The count parameter must be equal to or less than the original
// value passed to NicCmdBufferReserve.
//
INLINE VOID NicCmdBufferCommit(IfEnet* nic, VOID* addr, UINT count) {
    BYTE* tail = (BYTE*) addr + count;
    nic->cmdbuf.tail = (tail < nic->cmdbuf.last) ? tail : (tail-PAGE_SIZE);
}


PRIVATE NTSTATUS
NicEnqueueCmdBlock(
    IfEnet* nic,
    Packet* pkt,
    ActionCmdBlock* cmdblk,
    UINT cmdsize
    )

/*++

Routine Description:

    Queue up a command description block for execution

Arguments:

    nic - Points to the NIC data structure
    pkt - Points to the packet to be queued
    cmdblk - Points to the data for this command in the shared buffer
    cmdsize - Number of bytes taken by this command

Return Value:

    Status code

--*/

{
    Packet* q;

    // Keep a pointer to the command block in the packet
    cmdblk->link = LINK_OFFSET_NULL;
    SetPktCmdBlk(nic, pkt, cmdblk, cmdsize);

    // If the 82558 command unit is idle or suspended
    // and the command queue was empty, start or resume it here.
    if ((q = nic->cmdq.tail) == NULL) {
        NTSTATUS status = NicStartCommandUnit(nic, cmdblk);
        if (!NT_SUCCESS(status)) return status;
    }

    // Append this entry to the command queue
    NicCmdBufferCommit(nic, cmdblk, cmdsize);
    PktQInsertTail(&nic->cmdq, pkt);
    nic->cmdqCount++;

    // And modify the last command's link field
    // and clear its EOL flag.
    if (q != NULL) {
        ActionCmdBlock* lastcmd = GetPktCmdBlk(nic, q);
        lastcmd->link = CmdBufferGetPhyAddr(nic, cmdblk);
        InterlockedAND(&lastcmd->cmdstatus, ~CMDFLAG_EL);
    }
    return NETERR_OK;
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
        NicProcessXmitInterrupt(nic, nic->CSR->statAck);
        KeStallExecutionProcessor(50);

        timeout++;
        ASSERT(timeout < 20000);
    }
}


PRIVATE BOOL
NicProcessXmitInterrupt(
    IfEnet* nic,
    BYTE statAck
    )

/*++

Routine Description:

    Process command completion interrupts

Arguments:

    nic - Points to the NIC data structure
    statAck - NIC interrupt status bits

Return Value:

    TRUE if any command was completed
    FALSE otherwise

--*/

{
    Packet* pkt;
    ActionCmdBlock* cmdblk;
    NTSTATUS status;
    BOOL processed = FALSE;

    // Loop through our command queue
    while ((pkt = nic->cmdq.head) != NULL) {
        // Stop when we encounter a command which hasn't completed yet
        cmdblk = GetPktCmdBlk(nic, pkt);
        if (!(cmdblk->cmdstatus & CMDSTATUS_C)) {
            if (processed || (statAck & SCB_STAT_CNA)) {
                //
                // HACK to take care of the following scenario:
                //  We appended an entry to the command queue and
                //  modified the link and EOL info in the previous
                //  command (inside function NicEnqueueCmdBlock). But
                //  CU could be completing the previous command just
                //  as we were modifying it. So it could end up in
                //  the idle state instead of starting to execute
                //  the new command.
                // 
                NicStartCommandUnit(nic, cmdblk);
            }
            break;
        }

        // Remove the completed command from the queue
        // and do any necessary post-processing.
        processed = TRUE;
        PktQRemoveHead(&nic->cmdq);
        nic->cmdqCount--;
        nic->cmdqWatchdog = 0;
        status = (cmdblk->cmdstatus & CMDSTATUS_OK) ?
                    NETERR_OK :
                    NETERR_HARDWARE;

        // Free up the space taken in the shared command data buffer
        if (PktQIsEmpty(&nic->cmdq)) {
            //
            // no more outstanding commands
            //  reset the command buffer to its default state
            //
            nic->cmdbuf.head =
            nic->cmdbuf.tail = nic->cmdbuf.start;
        } else {
            //
            // Notice that if there is a gap at the end of
            // the command data buffer after , the head pointer
            // won't be correctly updated here. But that's ok
            // because it'll be correct again after the next
            // command is completed.
            //
            nic->cmdbuf.head = (BYTE*) cmdblk + GetPktCmdBlkSize(nic, pkt);
            if (nic->cmdbuf.head >= nic->cmdbuf.last)
                nic->cmdbuf.head -= PAGE_SIZE;
        }
        NicUnlockPacketPages(pkt);
        COMPLETEPACKET(pkt, status);
    }

    return processed;
}


PRIVATE VOID
NicProcessRecvInterrupt(
    IfEnet* nic
    )

/*++

Routine Description:

    Process packet reception interrupts

Arguments:

    nic - Points to the NIC data structure

Return Value:

    NONE

--*/

{
    while (TRUE) {
        Packet* pkt = nic->recvq.head;
        RecvFrameDesc* rfd = GetPktRFD(pkt);
        UINT pktlen;
        
        // No more received packets
        if (!(rfd->cmdstatus & CMDSTATUS_C)) return;
        
        // Take the packet off the receive buffer queue
        PktQRemoveHead(&nic->recvq);

        if (rfd->cmdstatus & CMDSTATUS_OK) {
            //
            // If the packet was received without error, process it.
            //
            ASSERT(rfd->actualCount & RFD_EOF);
            ASSERT(rfd->actualCount & RFD_F);

            pkt->datalen = pktlen = (rfd->actualCount & RFD_CNTMASK);
            ASSERT(pktlen >= ENETHDRLEN + ENET_MINDATASIZE);

            EnetReceiveFrame(nic, pkt);
        } else {
            //
            // Bad reception: just recycle the packet back into
            // the receive buffer queue
            //
            NicEnqueueRecvBuffer(nic, pkt);
        }
    }
}


PRIVATE VOID
NicInterruptDpc(
    PKDPC dpc,
    IfEnet* nic,
    VOID* arg1,
    VOID* arg2
    )

/*++

Routine Description:

    Ethernet interface interrupt service routine
    (runs at DISPATCH_LEVEL)

Arguments:

    dpc - Pointer to the DPC object
    nic - Points to the NIC data structure
    arg1, arg2 - Unused arguments

Return Value:

    NONE

--*/

{
    PNIC_CSR csr = nic->CSR;
    BYTE statAck = csr->statAck;

    // Acknowledge pending interrupts
    csr->statAck = statAck;

    // Process any received packets
    NicProcessRecvInterrupt(nic);

    // Process any completed action commands
    if (!PktQIsEmpty(&nic->cmdq)) {
        NicProcessXmitInterrupt(nic, statAck);
    }

    // Start the receive unit if it has been stopped
    NicStartReceiveUnit(nic);

    // If there is more room now in the command queue, we can
    // move some packets from the send queue to the command queue.
    if (!EnetIsSendQEmpty(nic) && nic->cmdqCount < cfgXmitQLength) {
        EnetStartOutput(nic);
    }

    // Reenable interrupts
    NicEnableInterrupt();
}


PRIVATE BOOLEAN
NicIsr(
    PKINTERRUPT interrupt,
    IfEnet* nic
    )

/*++

Routine Description:

    Ethernet interrupt service routine (runs at DIRQL)

Arguments:

    interrupt - Interrupt object
    nic - Points to the NIC data structure

Return Value:

    TRUE if the interrupt was handled
    FALSE if the interrupt wasn't generated by our device

--*/

{
    if ((nic->CSR->intrMask & INTR_MASK_BIT) || !nic->CSR->statAck)
        return FALSE;

    // Yep, this is ours:
    //  schedule the DPC routine for execution
    //  and disable further interrupts
    KeInsertQueueDpc(&nic->dpc, NULL, NULL);
    NicDisableInterrupt();
    return TRUE;
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
        We assume the Ethernet frame has been completed filled out
        and all the data are inside a single Packet structure.

Return Value:

    NONE

--*/

{
    NTSTATUS status;
    TransmitCmdBlock* txcb;
    TBDEntry* tbdArray;
    UINT tbdCount, cmdsize, len;
    UINT_PTR phyaddr0, phyaddr1;

    //
    // Lock the physical pages contain packet data
    //
    NicLockPacketPages(pkt);

    //
    // Allocate space in the shared command buffer:
    //  transmit command block followed by
    //  max number of transmission block descriptors
    //
    cmdsize = sizeof(TransmitCmdBlock) + sizeof(TBDEntry) * MAX_TBD_PER_XMIT;
    txcb = (TransmitCmdBlock*) NicCmdBufferReserve(nic, cmdsize);
    if (!txcb) {
        status = NETERR_MEMORY;
        goto exit;
    }

    // Make sure the packet is not too big
    ASSERT(pkt->datalen != 0);
    if (pkt->datalen > ENETHDRLEN+ENET_MAXDATASIZE) {
        status = NETERR_MSGSIZE;
        goto exit;
    }

    // Since the total buffer size is <= 1500+14 bytes,
    // it can at most span two physical pages.
    len = pkt->datalen;
    phyaddr0 = MmGetPhysicalAddress(pkt->data);
    phyaddr1 = MmGetPhysicalAddress(pkt->data + (len-1));

    tbdArray = (TBDEntry*) (txcb+1);
    tbdCount = 0;
    tbdArray[tbdCount].bufaddr = phyaddr0;

    if ((phyaddr1 - phyaddr0) == len-1) {
        // The buffer is physically contiguous.
        // We only need one TBD here.
        tbdArray[tbdCount++].bufsize = len;
    } else {
        // The buffer is not physically contiguous.
        // So we need two TBDs.
        UINT len0 = PAGE_SIZE - (phyaddr0 & (PAGE_SIZE-1));
        tbdArray[tbdCount++].bufsize = len0;
        tbdArray[tbdCount].bufaddr = phyaddr1 & ~(PAGE_SIZE-1);
        tbdArray[tbdCount++].bufsize = len - len0;
    }

    cmdsize = sizeof(TransmitCmdBlock) + sizeof(TBDEntry) * tbdCount;

    //
    // Fill out the TxCB (transmit command block) itself
    //
    txcb->cmdstatus =
        ACTIONCMD_TRANSMIT |    // transmit command
        CMDFLAG_EL |            // end-of-list
        CMDFLAG_I |             // interrupt after completion
        TxCBFLAG_SF;            // flexible mode

    txcb->tbdArray = CmdBufferGetPhyAddr(nic, tbdArray);
    txcb->byteCount = 0;        // no data bytes in the TxCB
    txcb->txThreshold = DEFAULT_Tx_THRESHOLD;
    txcb->tbdNumber = (BYTE) tbdCount;

    // Queue up the transmit command for execution
    status = NicEnqueueCmdBlock(nic, pkt, (ActionCmdBlock*) txcb, cmdsize);

exit:
    //
    // If we failed to insert the packet into the NIC's command queue,
    // complete the packet here with error status. This is because we
    // won't get a command-completion interrupt and do it then.
    //
    if (!NT_SUCCESS(status)) {
        WARNING_("NicTransmitFrame failed: 0x%x", status);
        NicUnlockPacketPages(pkt);
        COMPLETEPACKET(pkt, status);
    }
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
    We assume the new list will replace the existing one.

Arguments:

    nic - Points to the NIC data structure
    addrs - Points to an array of multicast addresses
    count - Specifies the number of multicast addresses

Return Value:

    Status code

--*/

{
    McastSetupCmdBlock* mcastcmd;
    UINT cmdsize;
    Packet* pkt;
    NTSTATUS status;

    //
    // Reserve space in the shared command data buffer
    //
    cmdsize = offsetof(McastSetupCmdBlock, mcastAddrs) + count * ENETADDRLEN;
    cmdsize = ROUNDUP8(cmdsize);
    mcastcmd = (McastSetupCmdBlock*) NicCmdBufferReserve(nic, cmdsize);
    if (!mcastcmd) return NETERR_MEMORY;

    // Allocate a dummy packet for the command queue
    //  HACK: When the command finishes, CompletePacket is called.
    //  That ends up in the up-level Ethernet interface code.
    //  It'll just free the memory back to the system pool.
    pkt = SysAlloc0(PKTHDRLEN, PTAG_NIC);
    if (!pkt) return NETERR_MEMORY;

    pkt->data = pkt->buf;
    pkt->pktflags = (WORD)defaultPacketAllocFlag;
    NicLockPacketPages(pkt);

    //
    // Fill out the multicast setup command block
    //
    mcastcmd->cmdstatus =
        ACTIONCMD_MC_SETUP |
        CMDFLAG_EL |
        CMDFLAG_I;

    mcastcmd->mcastCount = (WORD) (count * ENETADDRLEN);
    if (count) {
        CopyMem(mcastcmd->mcastAddrs, addrs, count * ENETADDRLEN);
    }

    // Queue up the multicast setup command for execution
    status = NicEnqueueCmdBlock(nic, pkt, (ActionCmdBlock*) mcastcmd, cmdsize);
    if (!NT_SUCCESS(status)) {
        NicUnlockPacketPages(pkt);
        SysFree(pkt);
    }
    return status;
}


PRIVATE NTSTATUS
NicClaimHardware(
    IfEnet* nic
    )

/*++

Routine Description:

    Locate and claim the NIC adapter and
    map the NIC's CSR register into memory space

Arguments:

    nic - Points to the NIC data structure

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    ULONG deviceCount = 1;
    PCI_DEVICE_DESCRIPTOR hwres;

    ZeroMem(&hwres, sizeof(hwres));
    hwres.Bus =
    hwres.Slot = 0xffffffff;
    hwres.VendorID = NIC_VENDORID;
    hwres.DeviceID = NIC_DEVICEID;
    hwres.BaseClass = PCI_CLASS_NETWORK_CTLR;
    hwres.SubClass = PCI_SUBCLASS_NET_ETHERNET_CTLR;
    hwres.ProgIf = 0xff;

    status = HalSetupPciDevice(&hwres, &deviceCount);
    if (!NT_SUCCESS(status)) {
        WARNING_("Couldn't find NIC adapter: 0x%x!", status);
        return NETERR_HARDWARE;
    }

    ASSERT(hwres.ResourceData.Address[0].Type == CmResourceTypeMemory);
    ASSERT(hwres.ResourceData.Address[0].u.Memory.Length >= sizeof(struct _NIC_CSR));

    NicCsr = nic->CSR = (PNIC_CSR) hwres.ResourceData.Address[0].u.Memory.TranslatedAddress;
    nic->csrSize = hwres.ResourceData.Address[0].u.Memory.Length;
    nic->intrVector = hwres.ResourceData.Interrupt.Vector;
    nic->intrIrql = hwres.ResourceData.Interrupt.Irql;

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

    if (nic->cmdbuf.start) {
        NicFreeSharedMem(nic->cmdbuf.start);
        nic->cmdbuf.start = NULL;
    }

    // Clean up the receive buffer queue
    NicPktPoolCleanup();
    PktQInit(&nic->recvq);

    // Acknowledge pending interrupts
    if (NicCsr) {
        NicCsr->statAck = NicCsr->statAck;
        NicDisableInterrupt();
        NicDisconnectInterrupt(nic);

        // Unmap CSR from the virtual address space.
        MmUnmapIoSpace((VOID*) NicCsr, nic->csrSize);
        NicCsr = NULL;
    }
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
    PNIC_CSR csr;
    NTSTATUS status = NETERR_HARDWARE;
    DWORD linkState;

    // Locate the NIC card and get the assigned resources
    status = NicClaimHardware(nic);
    if (!NT_SUCCESS(status)) goto err;
    csr = NicCsr;

    KeInitializeDpc(&nic->dpc, (PKDEFERRED_ROUTINE) NicInterruptDpc, nic);
    KeInitializeInterrupt(
        &NicIntrObject,
        (PKSERVICE_ROUTINE) NicIsr,
        nic,
        nic->intrVector,
        nic->intrIrql,
        LevelSensitive,
        TRUE);


    // Per 82558 manual, we should do a selective reset first
    // before doing a full software reset.
    csr->port = PORTCMD_SELECTIVE_RESET;
    KeStallExecutionProcessor(20);
    csr->port = PORTCMD_SOFTWARE_RESET;
    KeStallExecutionProcessor(20);

    // Interrupts are enabled after a reset.
    // So disable them here.
    NicDisableInterrupt();

    // Load CU and RU base to 0 (linear flat addressing)
    status = NicWaitScb(csr);
    if (!NT_SUCCESS(status)) goto err;
    csr->scbGeneralPtr = 0;
    csr->cucRuc = CUC_LOAD_CU_BASE;

    status = NicWaitScb(csr);
    if (!NT_SUCCESS(status)) goto err;
    csr->scbGeneralPtr = 0;
    csr->cucRuc = RUC_LOAD_RU_BASE;

    // Initialize the shared command data buffer
    // and the receive buffer queue
    status = NicCmdBufferInit(nic);
    if (!NT_SUCCESS(status)) goto err;
    status = NicRecvBufferInit(nic);
    if (!NT_SUCCESS(status)) goto err;

    // Read permanent Ethernet address from EEPROM
    status = NicReadEnetAddr(nic);
    if (!NT_SUCCESS(status)) goto err;

    // Configure the NIC parameters
    status = NicDoConfigureCmd(nic);
    if (!NT_SUCCESS(status)) goto err;

    // BOGUS: why in the world do we need to wait 500msec here?
    // The number is from win2k driver. But the NIC seems to work fine
    // if we take out the wait. The wait has also been taken out of the
    // current driver in Whistler.
    // KeStallExecutionProcessor(500000);

    // Set up the individual address
    status = NicDoIASetupCmd(nic);
    if (!NT_SUCCESS(status)) goto err;

    // Start the RU and enable interrupts
    status = NicStartReceiveUnit(nic);
    if (!NT_SUCCESS(status)) goto err;

    // Check if the Ethernet link is up
    PhyInitialize(FALSE, (VOID*) nic->CSR);
    linkState = PhyGetLinkState(FALSE);

    VERBOSE_("Ethernet link status: %s %dMbps %s-duplex",
        (linkState & XNET_LINK_IS_UP) ? "up" : "down",
        (linkState & XNET_LINK_100MBPS) ? 100 :
            (linkState & XNET_LINK_10MBPS) ? 10 : 0,
        (linkState & XNET_LINK_FULL_DUPLEX) ? "full" :
            (linkState & XNET_LINK_HALF_DUPLEX) ? "half" : "?");

    if (linkState & XNET_LINK_IS_UP)
        nic->flags |= IFFLAG_CONNECTED_BOOT;

    // Connect the NIC interrupt
    if (KeConnectInterrupt(&NicIntrObject))
        return NETERR_OK;

    status = STATUS_BIOS_FAILED_TO_CONNECT_INTERRUPT;

err:
    NicReset(nic, FALSE);
    NicCleanup(nic);
    return status;
}

#endif // SILVER

