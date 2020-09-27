/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    i82558.h

Abstract:

    Intel 82558 NIC hardware related declarations

Revision History:

    05/07/2000 davidx
        Created it.

--*/

#ifndef _82558_H
#define _82558_H

//
// Vendor and device IDs for EPRO100+
//
#define NIC_VENDORID 0x8086
#define NIC_DEVICEID 0x1229

//------------------------------------------------------------------------
// I82558 command/status registers
//------------------------------------------------------------------------

typedef struct _NIC_CSR {
    BYTE cusRus;
        // CU and RU status
        // Bit 7-6: CUS - CU status field
        //  00 - idle
        //  01 - suspended
        //  10 - active
        //  11 - * (reserved)
        // Bit 5-2: RUS - RU status field
        //  0000 - idle
        //  0001 - suspended
        //  0010 - no resources
        //  0011 - *
        //  0100 - ready
        //  0101 - *
        //  0110 - *
        //  0111 - *
        //  1000 - *
        //  1001 - suspended with no more RBDs
        //  1010 - No resources due to no more RBDs
        //  1011 - *
        //  1100 - ready with no RBDs present
        //  1101 - *
        //  1110 - *
        //  1111 - *
        // Bit 1-0: reserved

    #define SCB_STAT_CUS(x) (((x) >> 6) & 3)
    #define   CUS_IDLE          0
    #define   CUS_SUSPENDED     1
    #define   CUS_ACTIVE        2
    #define SCB_STAT_RUS(x) (((x) >> 2) & 0xf)
    #define   RUS_IDLE          0
    #define   RUS_SUSPENDED     1
    #define   RUS_NORES         2
    #define   RUS_READY         4
    #define   RUS_SUSPENDED_RBD 9
    #define   RUS_NORES_RBD     10
    #define   RUS_READY_RBD     12

    BYTE statAck;
        // Interrupt status and acknowledgement bits
        // Bit 7: CX - CU finished executing a command with I-bit set
        // Bit 6: FR - RU finished receiving a frame
        // Bit 5: CNA - CU went from active to idle or suspended state
        // Bit 4: RNR - RU left ready state
        // Bit 3: MDI - MDI read or write cycle is done
        // Bit 2: SWI - software generated an interrupt
        // Bit 1: reserved
        // Bit 0: FCP - flow control pause interrupt bit

    #define SCB_STAT_CX     BIT(7)
    #define SCB_STAT_FR     BIT(6)
    #define SCB_STAT_CNA    BIT(5)
    #define SCB_STAT_RNR    BIT(4)
    #define SCB_STAT_MDI    BIT(3)
    #define SCB_STAT_SWI    BIT(2)
    #define SCB_STAT_FCP    BIT(0)

    BYTE cucRuc;
        // Bit 7-4: CUC - CU command field
        //  0000 - NOP
        //  0001 - CU start
        //  0010 - CU resume
        //  0011 - *
        //  0100 - load dump counters address
        //  0101 - dump statistical counters
        //  0110 - load CU base
        //  0111 - dump and reset statistical counters
        //  1000 - *
        //  1001 - *
        //  1010 - static resume
        //  1011 - *
        //  ...
        //  1111 - *
        // Bit 3: reserved
        // Bit 2-0: RU command field
        //  000 - NOP
        //  001 - RU start
        //  010 - RU resume
        //  011 - RU DMA redirect
        //  100 - RU abort
        //  101 - load header data size
        //  110 - load RU base
        //  111 - RBD resume

    #define CUC_NOP             (0 << 4)
    #define CUC_START           (1 << 4)
    #define CUC_RESUME          (2 << 4)
    #define CUC_LOAD_DUMP_ADDR  (4 << 4)
    #define CUC_DUMP_STAT       (5 << 4)
    #define CUC_LOAD_CU_BASE    (6 << 4)
    #define CUC_DUMP_RESET_STAT (7 << 4)
    #define CUC_STATIC_RESUME   (10 << 4)

    #define RUC_NOP             0
    #define RUC_START           1
    #define RUC_RESUME          2
    #define RUC_RCVDMA_REDIRECT 3
    #define RUC_ABORT           4
    #define RUC_LOAD_HDS        5
    #define RUC_LOAD_RU_BASE    6
    #define RUC_RBD_RESUME      7

    BYTE intrMask;
        // Bit 7: CX interrupt mask bit
        // Bit 6: FR interrupt mask bit
        // Bit 5: CNA interrupt mask bit
        // Bit 4: RNR interrupt mask bit
        // Bit 3: ER interrupt mask bit
        // Bit 2: FCP interrupt mask bit
        // Bit 1: SI - software interrupt request
        // Bit 0: M - global interrupt mask

    #define CX_INTR_BIT     BIT(7)
    #define FR_INTR_BIT     BIT(6)
    #define CNA_INTR_BIT    BIT(5)
    #define RNR_INTR_BIT    BIT(4)
    #define ER_INTR_BIT     BIT(3)
    #define FCP_INTR_BIT    BIT(2)
    #define SW_INTR_BIT     BIT(1)
    #define INTR_MASK_BIT   BIT(0)

    DWORD scbGeneralPtr;
        // SCB general pointer

    DWORD port;
        // PORT interface
        // Bit 31-4: address bits
        // Bit 3-0: port function selection
        //  0000 - software reset
        //  0001 - self-test
        //  0010 - selective reset
        //  0011 - dump
        //  0100 - *
        //  ...
        //  1111 - *

    #define PORTCMD_SOFTWARE_RESET  0
    #define PORTCMD_SELF_TEST       1
    #define PORTCMD_SELECTIVE_RESET 2
    #define PORTCMD_DUMP            3

    WORD flashCtrl;
        // Flash control register

    WORD eepromCtrl;
        // EEPROM control register
        // Bit 7-4: reserved
        // Bit 3: EEDO - data out
        // Bit 2: EEDI - data in
        // Bit 1: EECS - chip select
        // Bit 0: EESK - serial clock

    #define EEPROM_DO   BIT(3)
    #define EEPROM_DI   BIT(2)
    #define EEPROM_CS   BIT(1)
    #define EEPROM_SK   BIT(0)

    #define EEPROMOP_WRITE  1
    #define EEPROMOP_READ   2

    DWORD mdiCtrl;
        // MDI control register
        // Bit 31-30: reserved
        // Bit 29: interrupt enable
        // Bit 28: ready
        // Bit 27-26: opcode
        //  00 - *
        //  01 - MDI write
        //  10 - MDI read
        //  11 - *
        // Bit 25-21: PHY address
        // Bit 20-16: PHY register address
        // Bit 15-0: data field

    #define MDI_INTR_ENABLE     BIT(29)
    #define MDI_READY           BIT(28)
    #define MDIOP_WRITE         (1 << 26)
    #define MDIOP_READ          (2 << 26)
    #define MDI_PHY_ADDR(x)     ((x) << 21)
    #define MDI_PHY_REG_ADDR(x) ((x) << 16)

    DWORD reserved1;
    DWORD reserved2;
        // Bit 31-24: PMDR - power management driver register
        // Bit 23-8: flow control register
        // Bit 7-0: reserved
} volatile *PNIC_CSR;

extern PNIC_CSR NicCsr;

//------------------------------------------------------------------------
// 82558 general action command format
// NOTE: We store this information in the Packet.ifdata field
// of an outgoing packet. And it's tail-aligned so that it's
// contiguous with the packet data field.
//------------------------------------------------------------------------

typedef struct _ActionCmdBlock {
    DWORD cmdstatus;
        // Command and status:
        //  bit 31 (EL) - end of command block list
        //  bit 30 (S) - suspend after command completion
        //  bit 29 (I) - interrupt after command completion
        //  bit 28-19 - varies with different commands
        //  bit 18-16 (CMD) - command code
        //  bit 15 (C) - command completed
        //  bit 14 - reserved
        //  bit 13 - command executed ok
        //  bit 12-0 - varies with different commands

    #define CMDFLAG_EL          BIT(31)
    #define CMDFLAG_S           BIT(30)
    #define CMDFLAG_I           BIT(29)

    #define ACTIONCMD_NOP       (0 << 16)
    #define ACTIONCMD_IA_SETUP  (1 << 16)
    #define ACTIONCMD_CONFIGURE (2 << 16)
    #define ACTIONCMD_MC_SETUP  (3 << 16)
    #define ACTIONCMD_TRANSMIT  (4 << 16)
    #define ACTIONCMD_DUMP      (6 << 16)
    #define ACTIONCMD_DIAGNOSE  (7 << 16)

    #define CMDSTATUS_C         BIT(15)
    #define CMDSTATUS_OK        BIT(13)

    DWORD link;
        // link to the next command block in the list
        //  this is a physical address
        //  use 0xffffffff at the end of the list

    #define LINK_OFFSET_NULL 0xffffffff

} ActionCmdBlock;

//------------------------------------------------------------------------
// Transmit command block (TxCB)
//------------------------------------------------------------------------

typedef struct _TransmitCmdBlock {
    ActionCmdBlock;
        // General action command data:
        //  command / status
        //  link
        // Additional command and status flags:
        //  bit 20 (NC) - no CRC and source address insertion
        //  bit 19 (SF) - simple or flexible mode
        //  bit 12 (U) - transmit underrun encountered

    #define TxCBFLAG_NC     BIT(20)
    #define TxCBFLAG_SF     BIT(19)

    #define TxCBSTATUS_U    BIT(12)

    DWORD tbdArray;
        // in flexible mode, physical address of the TBD array

    WORD byteCount;
        // bit 15 (EOF) - whole frame is kept inside the command block itself
        // bit 14 - reserved
        // bit 13-0: amount of data in the transmit command block itself

    #define TxCB_EOF        BIT(15)

    BYTE txThreshold;
        // number of bytes that should be present in the Tx FIFO
        // before it starts transmitting the frame

    BYTE tbdNumber;
        // in flexible mode, represent the number of transmit buffers
        // in the TBD array
} TransmitCmdBlock;

//
// Early transmit threshold
//  This number is multiplied by 8 to get the actual byte count.
//  BOGUS: Not sure why w2k driver uses such a high value which
//  basically disables the early transmit feature.
//
#define DEFAULT_Tx_THRESHOLD 200

//
// Maximum number of TBD's for transmitting a single packet.
// This is not a hardware limit. Rather it's an arbitrary software
// limit we set. When there are two many fragments for a single
// packet, something upstream must be pretty bad and it's better
// to address the real issues there.
//
#define MAX_TBD_PER_XMIT 4

//
// TBD (Transmit Buffer Descriptor) entry
//
typedef struct _TBDEntry {
    DWORD bufaddr;
        // physical address of the buffer data

    DWORD bufsize;
        // bit 31-16: reserved
        // bit 15 (EL): end of the TBD array
        // bit 14: reserved
        // bit 13-0: buffer size (in bytes)
} TBDEntry;

//------------------------------------------------------------------------
// Multicast setup command block
//------------------------------------------------------------------------

typedef struct _McastSetupCmdBlock {
    ActionCmdBlock;
        // General action command data:
        //  command / status
        //  link
    
    WORD mcastCount;
        // Number of multicast addresses to follow
    
    BYTE mcastAddrs[ENETADDRLEN];
        // variable number of Ethernet multicast addresses
} McastSetupCmdBlock;

//------------------------------------------------------------------------
// Individual address setup command block
//------------------------------------------------------------------------

typedef struct _IASetupCmdBlock {
    ActionCmdBlock;
        // General action command data:
        //  command / status
        //  link

    BYTE hwaddr[ENETADDRLEN];
        // Individual address
} IASetupCmdBlock;

//------------------------------------------------------------------------
// Configure command block
//------------------------------------------------------------------------

typedef struct _ConfigCmdBlock {
    ActionCmdBlock;
        // General action command data:
        //  command / status
        //  link

    // Maximum number of configuration parameters
    #define CONFIG_PARAM_COUNT 22

    BYTE params[CONFIG_PARAM_COUNT];
        // Configuration parameters
} ConfigCmdBlock;

//------------------------------------------------------------------------
// Receive frame descriptor (RFD)
//------------------------------------------------------------------------

typedef struct _RecvFrameDesc {
    ActionCmdBlock;
        // General action command data:
        //  command / status
        //  link
        // Additional command and status flags:
        //  bit 20 (H) - indicate this RFD is a Header RFC
        //  bit 19 (SF) - simple or flexible mode
        //
        //  bit 11 - CRC error in an aligned frame
        //  bit 10 - alignment error (number of bits is not 8x)
        //  bit 9 - ran out of buffer space - no resources
        //  bit 8 - DMA overrun - failed to acquire the system bus
        //  bit 7 - frame too short
        //  bit 5 - type/length: 1 for Ethernet frame, 0 for 802.3 frame
        //  bit 4 - receive error
        //  bit 2 - no address match
        //  bit 1 - IA match bit
        //  bit 0 - receive collision
    
    #define RFDFLAG_H   BIT(20)
    #define RFDFLAG_SF  BIT(19)

    #define RFDSTATUS_CRC_ERROR     BIT(11)
    #define RFDSTATUS_ALIGN_ERROR   BIT(10)
    #define RFDSTATUS_NO_BUFFER     BIT(9)
    #define RFDSTATUS_DMA_OVERRUN   BIT(8)
    #define RFDSTATUS_SHORT_FRAME   BIT(7)
    #define RFDSTATUS_ENET_FRAME    BIT(5)
    #define RFDSTATUS_RECV_ERROR    BIT(4)
    #define RFDSTATUS_NOADDR_MATCH  BIT(2)
    #define RFDSTATUS_IA_MATCH      BIT(1)
    #define RFDSTATUS_COLLISION     BIT(0)

    DWORD rbdAddr;
        // only used in flexible mode
        // address of RBD (receive buffer descriptor)
    
    WORD actualCount;
        // bit 15 (EOF): set by 82558 when it writes data into the data area
        // bit 14 (F): set by 82558 when it updates the actualCount field
        // bit 13-0: number of bytes written into the data area

    #define RFD_EOF     BIT(15)
    #define RFD_F       BIT(14)
    #define RFD_CNTMASK 0x3fff

    WORD size;
        // bit 15-14: reserved
        // bit 13-0: size of the data buffer
} RecvFrameDesc;

//------------------------------------------------------------------------
// 82558 Statistical Counters
//------------------------------------------------------------------------

typedef struct _NicStatCounters {
    DWORD xmitGoodFrames;
    DWORD xmitMaxCollisionsErrors;
    DWORD xmitLateCollisionsErrors;
    DWORD xmitUnderrunErrors;
    DWORD xmitLostCarrierSense;
    DWORD xmitDeferred;
    DWORD xmitSingleCollisions;
    DWORD xmitMultipleCollisions;
    DWORD xmitTotalCollisions;
    DWORD recvGoodFrames;
    DWORD recvCrcErrors;
    DWORD recvAlignmentErrors;
    DWORD recvResourceErrors;
    DWORD recvOverrunErrors;
    DWORD recvCollisionDetectErrors;
    DWORD recvShortFrameErrors;
    DWORD dumpCompletionStatus;
} NicStatCounters;


//
// We allocate a page of physically contiguous memory that's shared by
// the CPU and the NIC. This buffer is used for storing action command
// data (e.g. transmit command block and transmit buffer descriptor)
// that's passed to the NIC.
//
//  start  head        tail         last
//   |      |           |           |
//   v      v           v           v
//  |       xxxxxxxxxxxx           |
//
// Note that we always leave at least one unit empty in the buffer.
// If head = tail, that means the whole buffer is empty.
//
typedef struct _NicCmdBuffer {
    BYTE* start;        // buffer base virtual address
    BYTE* last;         // points to after the buffer
    BYTE* head;         // points to the first busy entry
    BYTE* tail;         // points to the first free entry
    UINT phyaddrOffset; // offset between virtual and physical address
} NicCmdBuffer;

// The smallest allocation unit is 8 bytes.
#define CMDBUF_ALLOC_UNIT 8

// Map virtual command buffer address to physical address
#define CmdBufferGetPhyAddr(nic, p) \
        ((UINT_PTR) (p) + (nic)->cmdbuf.phyaddrOffset)

//
// Extra header information at the beginning of the DMA packet
// that's used by the NIC for receiving frames:
//  receive frame descriptor
//
#define RECVPKT_OVERHEAD sizeof(RecvFrameDesc)

//
// Allocate shared memory buffers for the NIC
//
#define NicAllocSharedMem(size) MmAllocateContiguousMemory(size)
#define NicFreeSharedMem(p) MmFreeContiguousMemory(p)

//
// Disable and enable NIC interrupts
//
INLINE VOID NicDisableInterrupt() {
    NicCsr->intrMask = INTR_MASK_BIT;
}

INLINE VOID NicEnableInterrupt() {
    NicCsr->intrMask = 0;
}

#endif // !_82558_H

