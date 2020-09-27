/*++ BUILD Version: 0063    // Increment this if a change has global effects

Copyright (c) 1990-1997  Microsoft Corporation

Module Name:

    windbgkd.h

Abstract:

    This module defines the 32-Bit Windows Kernel Debugger interface.

Revision History:

--*/

#ifndef _WINDBGKD_
#define _WINDBGKD_

typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS; 

#ifdef _X86_

//
// DBGKD_CONTROL_REPORT
//
// This structure contains machine specific data passed to the debugger
// when a Wait_State_Change message is sent.  Idea is to allow debugger
// to do what it needes without reading any more packets.
// Structure is filled in by KdpSetControlReport
//

#define DBGKD_MAXSTREAM 16

typedef struct _DBGKD_CONTROL_REPORT {
    DWORD   Dr6;
    DWORD   Dr7;
    WORD    InstructionCount;
    WORD    ReportFlags;
    BYTE    InstructionStream[DBGKD_MAXSTREAM];
    WORD    SegCs;
    WORD    SegDs;
    WORD    SegEs;
    WORD    SegFs;
    DWORD   EFlags;
} DBGKD_CONTROL_REPORT, *PDBGKD_CONTROL_REPORT;

#define REPORT_INCLUDES_SEGS    0x0001  // this is for backward compatibility

//
// DBGKD_CONTROL_SET
//
// This structure control value the debugger wants to set on every
// continue, and thus sets here to avoid packet traffic.
//

typedef struct _DBGKD_CONTROL_SET {
    DWORD   TraceFlag;                  // WARNING: This must NOT be a BOOLEAN,
                                        //     or host and target will end
                                        //     up with different alignments!
    DWORD   Dr7;
    DWORD   CurrentSymbolStart;         // Range in which to trace locally
    DWORD   CurrentSymbolEnd;
} DBGKD_CONTROL_SET, *PDBGKD_CONTROL_SET;

#endif //_X86_


#ifndef _DESCRIPTOR_TABLE_ENTRY_DEFINED
#define _DESCRIPTOR_TABLE_ENTRY_DEFINED

typedef struct _DESCRIPTOR_TABLE_ENTRY {
    DWORD Selector;
    LDT_ENTRY Descriptor;
} DESCRIPTOR_TABLE_ENTRY, *PDESCRIPTOR_TABLE_ENTRY;

#endif // _DESCRIPTOR_TABLE_ENTRY_DEFINED


#if defined(_MIPS_)

// end_ntddk end_wdm end_nthal

//
// Define MIPS specific kernel debugger information.
//
// The following structure contains machine specific data passed to
// the host system kernel debugger in a wait state change message.
//

#define DBGKD_MAXSTREAM 16

typedef struct _DBGKD_CONTROL_REPORT {
    DWORD InstructionCount;
    BYTE  InstructionStream[DBGKD_MAXSTREAM];
} DBGKD_CONTROL_REPORT, *PDBGKD_CONTROL_REPORT;

//
// The following structure contains information that the host system
// kernel debugger wants to set on every continue operation and avoids
// the need to send extra packets of information.
//

typedef DWORD DBGKD_CONTROL_SET, *PDBGKD_CONTROL_SET;

#endif                          // ntddk wdm nthal

#ifdef _ALPHA_

//
// Define Alpha specific kernel debugger information.
//
// The following structure contains machine specific data passed to
// the host system kernel debugger in a wait state change message.
//

#define DBGKD_MAXSTREAM 16

typedef struct _DBGKD_CONTROL_REPORT {
    DWORD InstructionCount;
    BYTE  InstructionStream[DBGKD_MAXSTREAM];
} DBGKD_CONTROL_REPORT, *PDBGKD_CONTROL_REPORT;

//
// The following structure contains information that the host system
// kernel debugger wants to set on every continue operation and avoids
// the need to send extra packets of information.
//

typedef DWORD DBGKD_CONTROL_SET, *PDBGKD_CONTROL_SET;

#endif // _ALPHA_

#if defined(_PPC_)


//
// Define PowerPC specific kernel debugger information.
//
// The following structure contains machine specific data passed to
// the host system kernel debugger in a wait state change message.
//
// **FINISH**  Copied without change from MIPS; may need modification

#define DBGKD_MAXSTREAM 16

typedef struct _DBGKD_CONTROL_REPORT {
    DWORD InstructionCount;
    BYTE  InstructionStream[DBGKD_MAXSTREAM];
} DBGKD_CONTROL_REPORT, *PDBGKD_CONTROL_REPORT;

//
// The following structure contains information that the host system
// kernel debugger wants to set on every continue operation and avoids
// the need to send extra packets of information.
//

typedef DWORD DBGKD_CONTROL_SET, *PDBGKD_CONTROL_SET;

#endif // defined(_PPC_)

#ifdef _IA64_

//
// Define IA64 specific kernel debugger information.
//
// The following structure contains machine specific data passed to
// the host system kernel debugger in a wait state change message.
//

#define DBGKD_MAXSTREAM 16

typedef struct _DBGKD_CONTROL_REPORT {
    DWORD InstructionCount;
    BYTE  InstructionStream[DBGKD_MAXSTREAM];
} DBGKD_CONTROL_REPORT, *PDBGKD_CONTROL_REPORT;

//
// The following structure contains information that the host system
// kernel debugger wants to set on every continue operation and avoids
// the need to send extra packets of information.
//

typedef DWORD DBGKD_CONTROL_SET, *PDBGKD_CONTROL_SET;

#endif // _IA64_


#ifndef _DESCRIPTOR_TABLE_ENTRY_DEFINED
#define _DESCRIPTOR_TABLE_ENTRY_DEFINED

typedef struct _DESCRIPTOR_TABLE_ENTRY {
    DWORD Selector;
    LDT_ENTRY Descriptor;
} DESCRIPTOR_TABLE_ENTRY, *PDESCRIPTOR_TABLE_ENTRY;

#endif // _DESCRIPTOR_TABLE_ENTRY_DEFINED


#ifdef _PPC_
//
// Special Registers for PowerPC
//

typedef struct _KSPECIAL_REGISTERS {
    DWORD  KernelDr0;
    DWORD  KernelDr1;
    DWORD  KernelDr2;
    DWORD  KernelDr3;
    DWORD  KernelDr4;
    DWORD  KernelDr5;
    DWORD  KernelDr6;
    DWORD  KernelDr7;
    DWORD  Sprg0;
    DWORD  Sprg1;
    DWORD  Sr0;
    DWORD  Sr1;
    DWORD  Sr2;
    DWORD  Sr3;
    DWORD  Sr4;
    DWORD  Sr5;
    DWORD  Sr6;
    DWORD  Sr7;
    DWORD  Sr8;
    DWORD  Sr9;
    DWORD  Sr10;
    DWORD  Sr11;
    DWORD  Sr12;
    DWORD  Sr13;
    DWORD  Sr14;
    DWORD  Sr15;
    DWORD  DBAT0L;
    DWORD  DBAT0U;
    DWORD  DBAT1L;
    DWORD  DBAT1U;
    DWORD  DBAT2L;
    DWORD  DBAT2U;
    DWORD  DBAT3L;
    DWORD  DBAT3U;
    DWORD  IBAT0L;
    DWORD  IBAT0U;
    DWORD  IBAT1L;
    DWORD  IBAT1U;
    DWORD  IBAT2L;
    DWORD  IBAT2U;
    DWORD  IBAT3L;
    DWORD  IBAT3U;
    DWORD  Sdr1;
    DWORD  Reserved[9];
} KSPECIAL_REGISTERS, *PKSPECIAL_REGISTERS;

//
// Processor State structure.
//

typedef struct _KPROCESSOR_STATE {
    struct _CONTEXT ContextFrame;
    struct _KSPECIAL_REGISTERS SpecialRegisters;
} KPROCESSOR_STATE, *PKPROCESSOR_STATE;

#endif // _PPC_

#ifdef _X86_
//
// Special Registers for i386
//

typedef struct _DESCRIPTOR {
    WORD    Pad;
    WORD    Limit;
    DWORD   Base;
} KDESCRIPTOR, *PKDESCRIPTOR;

typedef struct _KSPECIAL_REGISTERS {
    DWORD Cr0;
    DWORD Cr2;
    DWORD Cr3;
    DWORD Cr4;
    DWORD KernelDr0;
    DWORD KernelDr1;
    DWORD KernelDr2;
    DWORD KernelDr3;
    DWORD KernelDr6;
    DWORD KernelDr7;
    KDESCRIPTOR Gdtr;
    KDESCRIPTOR Idtr;
    WORD   Tr;
    WORD   Ldtr;
    DWORD Reserved[6];
} KSPECIAL_REGISTERS, *PKSPECIAL_REGISTERS;

//
// Processor State frame: Before a processor freezes itself, it
// dumps the processor state to the processor state frame for
// debugger to examine.
//

typedef struct _KPROCESSOR_STATE {
    struct _CONTEXT ContextFrame;
    struct _KSPECIAL_REGISTERS SpecialRegisters;
} KPROCESSOR_STATE, *PKPROCESSOR_STATE;
#endif // _X86_

#ifdef _IA64_

// begin_nthal

//
// Stack Registers for IA64
//

typedef struct _STACK_REGISTERS {

    DWORDLONG IntR32;
    DWORDLONG IntR33;
    DWORDLONG IntR34;
    DWORDLONG IntR35;
    DWORDLONG IntR36;
    DWORDLONG IntR37;
    DWORDLONG IntR38;
    DWORDLONG IntR39;

    DWORDLONG IntR40;
    DWORDLONG IntR41;
    DWORDLONG IntR42;
    DWORDLONG IntR43;
    DWORDLONG IntR44;
    DWORDLONG IntR45;
    DWORDLONG IntR46;
    DWORDLONG IntR47;
    DWORDLONG IntR48;
    DWORDLONG IntR49;

    DWORDLONG IntR50;
    DWORDLONG IntR51;
    DWORDLONG IntR52;
    DWORDLONG IntR53;
    DWORDLONG IntR54;
    DWORDLONG IntR55;
    DWORDLONG IntR56;
    DWORDLONG IntR57;
    DWORDLONG IntR58;
    DWORDLONG IntR59;

    DWORDLONG IntR60;
    DWORDLONG IntR61;
    DWORDLONG IntR62;
    DWORDLONG IntR63;
    DWORDLONG IntR64;
    DWORDLONG IntR65;
    DWORDLONG IntR66;
    DWORDLONG IntR67;
    DWORDLONG IntR68;
    DWORDLONG IntR69;

    DWORDLONG IntR70;
    DWORDLONG IntR71;
    DWORDLONG IntR72;
    DWORDLONG IntR73;
    DWORDLONG IntR74;
    DWORDLONG IntR75;
    DWORDLONG IntR76;
    DWORDLONG IntR77;
    DWORDLONG IntR78;
    DWORDLONG IntR79;

    DWORDLONG IntR80;
    DWORDLONG IntR81;
    DWORDLONG IntR82;
    DWORDLONG IntR83;
    DWORDLONG IntR84;
    DWORDLONG IntR85;
    DWORDLONG IntR86;
    DWORDLONG IntR87;
    DWORDLONG IntR88;
    DWORDLONG IntR89;

    DWORDLONG IntR90;
    DWORDLONG IntR91;
    DWORDLONG IntR92;
    DWORDLONG IntR93;
    DWORDLONG IntR94;
    DWORDLONG IntR95;
    DWORDLONG IntR96;
    DWORDLONG IntR97;
    DWORDLONG IntR98;
    DWORDLONG IntR99;

    DWORDLONG IntR100;
    DWORDLONG IntR101;
    DWORDLONG IntR102;
    DWORDLONG IntR103;
    DWORDLONG IntR104;
    DWORDLONG IntR105;
    DWORDLONG IntR106;
    DWORDLONG IntR107;
    DWORDLONG IntR108;
    DWORDLONG IntR109;

    DWORDLONG IntR110;
    DWORDLONG IntR111;
    DWORDLONG IntR112;
    DWORDLONG IntR113;
    DWORDLONG IntR114;
    DWORDLONG IntR115;
    DWORDLONG IntR116;
    DWORDLONG IntR117;
    DWORDLONG IntR118;
    DWORDLONG IntR119;

    DWORDLONG IntR120;
    DWORDLONG IntR121;
    DWORDLONG IntR122;
    DWORDLONG IntR123;
    DWORDLONG IntR124;
    DWORDLONG IntR125;
    DWORDLONG IntR126;
    DWORDLONG IntR127;
                                 // Nat bits for stack registers
    DWORDLONG IntNats2;          // r32-r95 in bit positions 1 to 63
    DWORDLONG IntNats3;          // r96-r127 in bit position 1 to 31

} STACK_REGISTERS, *PSTACK_REGISTERS;


//
// Special Registers for IA64
//

typedef struct _KSPECIAL_REGISTERS {

    // Kernel debug breakpoint registers

    DWORDLONG KernelDbI0;         // Instruction debug registers
    DWORDLONG KernelDbI1;
    DWORDLONG KernelDbI2;
    DWORDLONG KernelDbI3;
    DWORDLONG KernelDbI4;
    DWORDLONG KernelDbI5;
    DWORDLONG KernelDbI6;
    DWORDLONG KernelDbI7;

    DWORDLONG KernelDbD0;         // Data debug registers
    DWORDLONG KernelDbD1;
    DWORDLONG KernelDbD2;
    DWORDLONG KernelDbD3;
    DWORDLONG KernelDbD4;
    DWORDLONG KernelDbD5;
    DWORDLONG KernelDbD6;
    DWORDLONG KernelDbD7;

    // Kernel performance monitor registers

    DWORDLONG KernelPfC0;         // Performance configuration registers
    DWORDLONG KernelPfC1;
    DWORDLONG KernelPfC2;
    DWORDLONG KernelPfC3;
    DWORDLONG KernelPfC4;
    DWORDLONG KernelPfC5;
    DWORDLONG KernelPfC6;
    DWORDLONG KernelPfC7;

    DWORDLONG KernelPfD0;         // Performance data registers
    DWORDLONG KernelPfD1;
    DWORDLONG KernelPfD2;
    DWORDLONG KernelPfD3;
    DWORDLONG KernelPfD4;
    DWORDLONG KernelPfD5;
    DWORDLONG KernelPfD6;
    DWORDLONG KernelPfD7;

    // kernel bank shadow (hidden) registers

    DWORDLONG IntH16;
    DWORDLONG IntH17;
    DWORDLONG IntH18;
    DWORDLONG IntH19;
    DWORDLONG IntH20;
    DWORDLONG IntH21;
    DWORDLONG IntH22;
    DWORDLONG IntH23;
    DWORDLONG IntH24;
    DWORDLONG IntH25;
    DWORDLONG IntH26;
    DWORDLONG IntH27;
    DWORDLONG IntH28;
    DWORDLONG IntH29;
    DWORDLONG IntH30;
    DWORDLONG IntH31;

    // Application Registers

    //       - CPUID Registers - AR
    DWORDLONG ApCPUID0; // Cpuid Register 0
    DWORDLONG ApCPUID1; // Cpuid Register 1
    DWORDLONG ApCPUID2; // Cpuid Register 2
    DWORDLONG ApCPUID3; // Cpuid Register 3
    DWORDLONG ApCPUID4; // Cpuid Register 4
    DWORDLONG ApCPUID5; // Cpuid Register 5
    DWORDLONG ApCPUID6; // Cpuid Register 6
    DWORDLONG ApCPUID7; // Cpuid Register 7

    //       - Kernel Registers - AR
    DWORDLONG ApKR0;    // Kernel Register 0 (User RO)
    DWORDLONG ApKR1;    // Kernel Register 1 (User RO)
    DWORDLONG ApKR2;    // Kernel Register 2 (User RO)
    DWORDLONG ApKR3;    // Kernel Register 3 (User RO)
    DWORDLONG ApKR4;    // Kernel Register 4
    DWORDLONG ApKR5;    // Kernel Register 5
    DWORDLONG ApKR6;    // Kernel Register 6
    DWORDLONG ApKR7;    // Kernel Register 7

    DWORDLONG ApITC;    // Interval Timer Counter

    // Global control registers

    DWORDLONG ApITM;    // Interval Timer Match register
    DWORDLONG ApIVA;    // Interrupt Vector Address
    DWORDLONG ApPTA;    // Page Table Address
    DWORDLONG ApGPTA;   // ia32 Page Table Address

    //       - Trap Status Information
    DWORDLONG StISR;    // Interrupt status
    DWORDLONG StIFA;    // Interruption Faulting Address
    DWORDLONG StITIR;   // Interruption TLB Insertion Register
    DWORDLONG StIIPA;   // Interruption Instruction Previous Address (RO)
    DWORDLONG StIIM;    // Interruption Immediate register (RO)
    DWORDLONG StIHA;    // Interruption Hash Address (RO)

    //       - External Interrupt control registers (SAPIC)
    DWORDLONG SaLID;    // Local SAPIC ID
    DWORDLONG SaIVR;    // Interrupt Vector Register (RO)
    DWORDLONG SaTPR;    // Task Priority Register
    DWORDLONG SaEOI;    // End Of Interrupt
    DWORDLONG SaIRR0;   // Interrupt Request Register 0 (RO)
    DWORDLONG SaIRR1;   // Interrupt Request Register 1 (RO)
    DWORDLONG SaIRR2;   // Interrupt Request Register 2 (RO)
    DWORDLONG SaIRR3;   // Interrupt Request Register 3 (RO)
    DWORDLONG SaITV;    // Interrupt Timer Vector
    DWORDLONG SaPMV;    // Performance Monitor Vector
    DWORDLONG SaCMCV;   // Corrected Machine Check Vector
    DWORDLONG SaLRR0;   // Local Interrupt Redirection Vector 0
    DWORDLONG SaLRR1;   // Local Interrupt Redirection Vector 1

    // System Registers
    //       - Region registers
    DWORDLONG Rr0;  // Region register 0
    DWORDLONG Rr1;  // Region register 1
    DWORDLONG Rr2;  // Region register 2
    DWORDLONG Rr3;  // Region register 3
    DWORDLONG Rr4;  // Region register 4
    DWORDLONG Rr5;  // Region register 5
    DWORDLONG Rr6;  // Region register 6
    DWORDLONG Rr7;  // Region register 7

    //      - Protection Key registers
    DWORDLONG Pkr0;     // Protection Key register 0
    DWORDLONG Pkr1;     // Protection Key register 1
    DWORDLONG Pkr2;     // Protection Key register 2
    DWORDLONG Pkr3;     // Protection Key register 3
    DWORDLONG Pkr4;     // Protection Key register 4
    DWORDLONG Pkr5;     // Protection Key register 5
    DWORDLONG Pkr6;     // Protection Key register 6
    DWORDLONG Pkr7;     // Protection Key register 7
    DWORDLONG Pkr8;     // Protection Key register 8
    DWORDLONG Pkr9;     // Protection Key register 9
    DWORDLONG Pkr10;    // Protection Key register 10
    DWORDLONG Pkr11;    // Protection Key register 11
    DWORDLONG Pkr12;    // Protection Key register 12
    DWORDLONG Pkr13;    // Protection Key register 13
    DWORDLONG Pkr14;    // Protection Key register 14
    DWORDLONG Pkr15;    // Protection Key register 15

    //      -  Translation Lookaside buffers
    DWORDLONG TrI0;     // Instruction Translation Register 0
    DWORDLONG TrI1;     // Instruction Translation Register 1
    DWORDLONG TrI2;     // Instruction Translation Register 2
    DWORDLONG TrI3;     // Instruction Translation Register 3
    DWORDLONG TrI4;     // Instruction Translation Register 4
    DWORDLONG TrI5;     // Instruction Translation Register 5
    DWORDLONG TrI6;     // Instruction Translation Register 6
    DWORDLONG TrI7;     // Instruction Translation Register 7

    DWORDLONG TrD0;     // Data Translation Register 0
    DWORDLONG TrD1;     // Data Translation Register 1
    DWORDLONG TrD2;     // Data Translation Register 2
    DWORDLONG TrD3;     // Data Translation Register 3
    DWORDLONG TrD4;     // Data Translation Register 4
    DWORDLONG TrD5;     // Data Translation Register 5
    DWORDLONG TrD6;     // Data Translation Register 6
    DWORDLONG TrD7;     // Data Translation Register 7

    //      -  Machine Specific Registers
    DWORDLONG SrMSR0;   // Machine Specific Register 0
    DWORDLONG SrMSR1;   // Machine Specific Register 1
    DWORDLONG SrMSR2;   // Machine Specific Register 2
    DWORDLONG SrMSR3;   // Machine Specific Register 3
    DWORDLONG SrMSR4;   // Machine Specific Register 4
    DWORDLONG SrMSR5;   // Machine Specific Register 5
    DWORDLONG SrMSR6;   // Machine Specific Register 6
    DWORDLONG SrMSR7;   // Machine Specific Register 7

} KSPECIAL_REGISTERS, *PKSPECIAL_REGISTERS;


//
// Processor State structure.
//

typedef struct _KPROCESSOR_STATE {
    struct _CONTEXT ContextFrame;
    struct _KSPECIAL_REGISTERS SpecialRegisters;
} KPROCESSOR_STATE, *PKPROCESSOR_STATE;

// end_nthal

#endif // _IA64_


typedef struct _DBGKM_EXCEPTION {
    EXCEPTION_RECORD ExceptionRecord;
    DWORD FirstChance;
} DBGKM_EXCEPTION, *PDBGKM_EXCEPTION;


//
// DbgKd APIs are for the portable kernel debugger
//

//
// KD_PACKETS are the low level data format used in KD. All packets
// begin with a packet leader, byte count, packet type. The sequence
// for accepting a packet is:
//
//  - read 4 bytes to get packet leader.  If read times out (10 seconds)
//    with a short read, or if packet leader is incorrect, then retry
//    the read.
//
//  - next read 2 byte packet type.  If read times out (10 seconds) with
//    a short read, or if packet type is bad, then start again looking
//    for a packet leader.
//
//  - next read 4 byte packet Id.  If read times out (10 seconds)
//    with a short read, or if packet Id is not what we expect, then
//    ask for resend and restart again looking for a packet leader.
//
//  - next read 2 byte byte count.  If read times out (10 seconds) with
//    a short read, or if byte count is greater than PACKET_MAX_SIZE,
//    then start again looking for a packet leader.
//
//  - next read 4 byte packet data checksum.
//
//  - The packet data immediately follows the packet.  There should be
//    ByteCount bytes following the packet header.  Read the packet
//    data, if read times out (10 seconds) then start again looking for
//    a packet leader.
//


typedef struct _KD_PACKET {
    DWORD PacketLeader;
    WORD   PacketType;
    WORD   ByteCount;
    DWORD PacketId;
    DWORD Checksum;
} KD_PACKET, *PKD_PACKET;


#define PACKET_MAX_SIZE 4000
#define INITIAL_PACKET_ID 0x80800000    // DON't use 0
#define SYNC_PACKET_ID    0x00000800    // Or in with INITIAL_PACKET_ID
                                        // to force a packet ID reset.

//
// BreakIn packet
//

#define BREAKIN_PACKET                  0x62626262
#define BREAKIN_PACKET_BYTE             0x62

//
// Packet lead in sequence
//

#define PACKET_LEADER                   0x30303030 //0x77000077
#define PACKET_LEADER_BYTE              0x30

#define CONTROL_PACKET_LEADER           0x69696969
#define CONTROL_PACKET_LEADER_BYTE      0x69

//
// Packet Trailing Byte
//

#define PACKET_TRAILING_BYTE            0xAA

//
// Packet Types
//

#define PACKET_TYPE_UNUSED              0
#define PACKET_TYPE_KD_STATE_CHANGE     1
#define PACKET_TYPE_KD_STATE_MANIPULATE 2
#define PACKET_TYPE_KD_DEBUG_IO         3
#define PACKET_TYPE_KD_ACKNOWLEDGE      4       // Packet-control type
#define PACKET_TYPE_KD_RESEND           5       // Packet-control type
#define PACKET_TYPE_KD_RESET            6       // Packet-control type
#define PACKET_TYPE_MAX                 7

//
// If the packet type is PACKET_TYPE_KD_STATE_CHANGE, then
// the format of the packet data is as follows:
//

#define DbgKdExceptionStateChange   0x00003030L
#define DbgKdLoadSymbolsStateChange 0x00003031L

//
// Pathname Data follows directly
//

typedef struct _DBGKD_LOAD_SYMBOLS {
    DWORD PathNameLength;
    PVOID BaseOfDll;
    DWORD ProcessId;
    DWORD CheckSum;
    DWORD SizeOfImage;
    BOOLEAN UnloadSymbols;
} DBGKD_LOAD_SYMBOLS, *PDBGKD_LOAD_SYMBOLS;

typedef struct _DBGKD_WAIT_STATE_CHANGE {
    DWORD NewState;
    WORD   ProcessorLevel;
    WORD   Processor;
    DWORD NumberProcessors;
    PVOID Thread;
    PVOID ProgramCounter;
    union {
        DBGKM_EXCEPTION Exception;
        DBGKD_LOAD_SYMBOLS LoadSymbols;
    } u;
    DBGKD_CONTROL_REPORT ControlReport;
    CONTEXT Context;
} DBGKD_WAIT_STATE_CHANGE, *PDBGKD_WAIT_STATE_CHANGE;

//
// If the packet type is PACKET_TYPE_KD_STATE_MANIPULATE, then
// the format of the packet data is as follows:
//
// Api Numbers for state manipulation
//

#define DbgKdReadVirtualMemoryApi           0x00003130L
#define DbgKdWriteVirtualMemoryApi          0x00003131L
#define DbgKdGetContextApi                  0x00003132L
#define DbgKdSetContextApi                  0x00003133L
#define DbgKdWriteBreakPointApi             0x00003134L
#define DbgKdRestoreBreakPointApi           0x00003135L
#define DbgKdContinueApi                    0x00003136L
#define DbgKdReadControlSpaceApi            0x00003137L
#define DbgKdWriteControlSpaceApi           0x00003138L
#define DbgKdReadIoSpaceApi                 0x00003139L
#define DbgKdWriteIoSpaceApi                0x0000313AL
#define DbgKdRebootApi                      0x0000313BL
#define DbgKdContinueApi2                   0x0000313CL
#define DbgKdReadPhysicalMemoryApi          0x0000313DL
#define DbgKdWritePhysicalMemoryApi         0x0000313EL
#define DbgKdQuerySpecialCallsApi           0x0000313FL
#define DbgKdSetSpecialCallApi              0x00003140L
#define DbgKdClearSpecialCallsApi           0x00003141L
#define DbgKdSetInternalBreakPointApi       0x00003142L
#define DbgKdGetInternalBreakPointApi       0x00003143L
#define DbgKdReadIoSpaceExtendedApi         0x00003144L
#define DbgKdWriteIoSpaceExtendedApi        0x00003145L
#define DbgKdGetVersionApi                  0x00003146L
#define DbgKdWriteBreakPointExApi           0x00003147L
#define DbgKdRestoreBreakPointExApi         0x00003148L
#define DbgKdCauseBugCheckApi               0x00003149L
#define DbgKdSwitchProcessor                0x00003150L
#define DbgKdPageInApi                      0x00003151L
#define DbgKdReadMachineSpecificRegister    0x00003152L
#define DbgKdWriteMachineSpecificRegister   0x00003153L
#define DbgKdReadVirtualMemory64Api         0x00003154L
#define DbgKdWriteVirtualMemory64Api        0x00003155L

//
// Response is a read memory message with data following
//

typedef struct _DBGKD_READ_MEMORY {
    PVOID TargetBaseAddress;
    DWORD TransferCount;
    DWORD ActualBytesRead;
} DBGKD_READ_MEMORY, *PDBGKD_READ_MEMORY;

#include "pshpack4.h"
typedef struct _DBGKD_READ_MEMORY64 {
    DWORDLONG TargetBaseAddress;
    DWORD TransferCount;
    DWORD ActualBytesRead;
} DBGKD_READ_MEMORY64, *PDBGKD_READ_MEMORY64;
#include "poppack.h"

//
// Data follows directly
//

typedef struct _DBGKD_WRITE_MEMORY {
    PVOID TargetBaseAddress;
    DWORD TransferCount;
    DWORD ActualBytesWritten;
} DBGKD_WRITE_MEMORY, *PDBGKD_WRITE_MEMORY;

#include "pshpack4.h"
typedef struct _DBGKD_WRITE_MEMORY64 {
    DWORDLONG TargetBaseAddress;
    DWORD TransferCount;
    DWORD ActualBytesWritten;
} DBGKD_WRITE_MEMORY64, *PDBGKD_WRITE_MEMORY64;
#include "poppack.h"

//
// Response is a get context message with a full context record following
//

typedef struct _DBGKD_GET_CONTEXT {
    DWORD ContextFlags;
} DBGKD_GET_CONTEXT, *PDBGKD_GET_CONTEXT;

//
// Full Context record follows
//

typedef struct _DBGKD_SET_CONTEXT {
    DWORD ContextFlags;
} DBGKD_SET_CONTEXT, *PDBGKD_SET_CONTEXT;

#define BREAKPOINT_TABLE_SIZE   32      // max number supported by kernel

typedef struct _DBGKD_WRITE_BREAKPOINT {
    PVOID BreakPointAddress;
    DWORD BreakPointHandle;
} DBGKD_WRITE_BREAKPOINT, *PDBGKD_WRITE_BREAKPOINT;

typedef struct _DBGKD_RESTORE_BREAKPOINT {
    DWORD BreakPointHandle;
} DBGKD_RESTORE_BREAKPOINT, *PDBGKD_RESTORE_BREAKPOINT;

typedef struct _DBGKD_BREAKPOINTEX {
    DWORD     BreakPointCount;
    DWORD     ContinueStatus;
} DBGKD_BREAKPOINTEX, *PDBGKD_BREAKPOINTEX;

typedef struct _DBGKD_CONTINUE {
    DWORD    ContinueStatus;
} DBGKD_CONTINUE, *PDBGKD_CONTINUE;

typedef struct _DBGKD_CONTINUE2 {
    DWORD    ContinueStatus;
    DBGKD_CONTROL_SET ControlSet;
} DBGKD_CONTINUE2, *PDBGKD_CONTINUE2;

typedef struct _DBGKD_READ_WRITE_IO {
    DWORD DataSize;                     // 1, 2, 4
    PVOID IoAddress;
    DWORD DataValue;
} DBGKD_READ_WRITE_IO, *PDBGKD_READ_WRITE_IO;

typedef struct _DBGKD_READ_WRITE_IO_EXTENDED {
    DWORD DataSize;                     // 1, 2, 4
    DWORD InterfaceType;
    DWORD BusNumber;
    DWORD AddressSpace;
    PVOID IoAddress;
    DWORD DataValue;
} DBGKD_READ_WRITE_IO_EXTENDED, *PDBGKD_READ_WRITE_IO_EXTENDED;

typedef struct _DBGKD_READ_WRITE_MSR {
    DWORD Msr;
    DWORD DataValueLow;
    DWORD DataValueHigh;
} DBGKD_READ_WRITE_MSR, *PDBGKD_READ_WRITE_MSR;


typedef struct _DBGKD_QUERY_SPECIAL_CALLS {
    DWORD NumberOfSpecialCalls;
    // DWORD SpecialCalls[];
} DBGKD_QUERY_SPECIAL_CALLS, *PDBGKD_QUERY_SPECIAL_CALLS;

typedef struct _DBGKD_SET_SPECIAL_CALL {
    DWORD SpecialCall;
} DBGKD_SET_SPECIAL_CALL, *PDBGKD_SET_SPECIAL_CALL;

typedef struct _DBGKD_SET_INTERNAL_BREAKPOINT {
    UINT_PTR BreakpointAddress;
    DWORD Flags;
} DBGKD_SET_INTERNAL_BREAKPOINT, *PDBGKD_SET_INTERNAL_BREAKPOINT;

typedef struct _DBGKD_GET_INTERNAL_BREAKPOINT {
    UINT_PTR BreakpointAddress;
    DWORD Flags;
    DWORD Calls;
    DWORD MaxCallsPerPeriod;
    DWORD MinInstructions;
    DWORD MaxInstructions;
    DWORD TotalInstructions;
} DBGKD_GET_INTERNAL_BREAKPOINT, *PDBGKD_GET_INTERNAL_BREAKPOINT;

#define DBGKD_INTERNAL_BP_FLAG_COUNTONLY 0x00000001 // don't count instructions
#define DBGKD_INTERNAL_BP_FLAG_INVALID   0x00000002 // disabled BP
#define DBGKD_INTERNAL_BP_FLAG_SUSPENDED 0x00000004 // temporarily suspended
#define DBGKD_INTERNAL_BP_FLAG_DYING     0x00000008 // kill on exit

typedef struct _DBGKD_GET_VERSION {
    WORD    MajorVersion;
    WORD    MinorVersion;
    WORD    ProtocolVersion;
    WORD    Flags;
    UINT_PTR   KernBase;
    UINT_PTR   PsLoadedModuleList;
    WORD    MachineType;

    //
    // help for walking stacks with user callbacks:
    //

    //
    // The address of the thread structure is provided in the
    // WAIT_STATE_CHANGE packet.  This is the offset from the base of
    // the thread structure to the pointer to the kernel stack frame
    // for the currently active usermode callback.
    //

    WORD    ThCallbackStack;            // offset in thread data

    //
    // these values are offsets into that frame:
    //

    WORD    NextCallback;               // saved pointer to next callback frame
    WORD    FramePointer;               // saved frame pointer

    //
    // Address of the kernel callout routine.
    //

    UINT_PTR   KiCallUserMode;             // kernel routine

    //
    // Address of the usermode entry point for callbacks.
    //

    UINT_PTR   KeUserCallbackDispatcher;   // address in ntdll

    //
    // DbgBreakPointWithStatus is a function which takes a DWORD argument
    // and hits a breakpoint.  This field contains the address of the
    // breakpoint instruction.  When the debugger sees a breakpoint
    // at this address, it may retrieve the argument from the first
    // argument register, or on x86 the eax register.
    //

    UINT_PTR   BreakpointWithStatus;       // address of breakpoint

    //
    // Components may register a debug data block for use by
    // debugger extensions.  This is the address of the list head.
    //

    UINT_PTR   DebuggerDataList;

} DBGKD_GET_VERSION, *PDBGKD_GET_VERSION;

#define DBGKD_VERS_FLAG_MP      0x0001      // kernel is MP built
#define DBGKD_VERS_FLAG_DATA    0x0002      // DebuggerDataList is valid

//
// If DBGKD_VERS_FLAG_DATA is set in Flags, info should be retrieved from
// the KDDEBUGGER_DATA block rather than from the DBGKD_GET_VERSION
// packet.  The data will remain in the version packet for a while to
// reduce compatibility problems.
//

typedef struct _DBGKD_DEBUG_DATA_HEADER {

    //
    // Link to other blocks
    //

    LIST_ENTRY      List;

    //
    // This is a unique tag to identify the owner of the block.
    // If your component only uses one pool tag, use it for this, too.
    //

    DWORD           OwnerTag;

    //
    // This must be initialized to the size of the data block,
    // including this structure.
    //

    DWORD           Size;

} DBGKD_DEBUG_DATA_HEADER, *PDBGKD_DEBUG_DATA_HEADER;


#define KDBG_TAG    'GBDK'

typedef struct _KDDEBUGGER_DATA {

    DBGKD_DEBUG_DATA_HEADER Header;

    //
    // Base address of kernel image
    //

    UINT_PTR   KernBase;

    //
    // DbgBreakPointWithStatus is a function which takes an argument
    // and hits a breakpoint.  This field contains the address of the
    // breakpoint instruction.  When the debugger sees a breakpoint
    // at this address, it may retrieve the argument from the first
    // argument register, or on x86 the eax register.
    //

    UINT_PTR   BreakpointWithStatus;       // address of breakpoint

    //
    // Address of the saved context record during a bugcheck
    //
    // N.B. This is an automatic in KeBugcheckEx's frame, and
    // is only valid after a bugcheck.
    //

    UINT_PTR   SavedContext;

    //
    // help for walking stacks with user callbacks:
    //

    //
    // The address of the thread structure is provided in the
    // WAIT_STATE_CHANGE packet.  This is the offset from the base of
    // the thread structure to the pointer to the kernel stack frame
    // for the currently active usermode callback.
    //

    WORD    ThCallbackStack;            // offset in thread data

    //
    // these values are offsets into that frame:
    //

    WORD    NextCallback;               // saved pointer to next callback frame
    WORD    FramePointer;               // saved frame pointer

    WORD    Unused1;
    //
    // Address of the kernel callout routine.
    //

    UINT_PTR   KiCallUserMode;             // kernel routine

    //
    // Address of the usermode entry point for callbacks.
    //

    UINT_PTR   KeUserCallbackDispatcher;   // address in ntdll


    //
    // Addresses of various kernel data structures and lists
    // that are of interest to the kernel debugger.
    //

    UINT_PTR   PsLoadedModuleList;
    UINT_PTR   PsActiveProcessHead;
    UINT_PTR   PspCidTable;

    UINT_PTR   ExpSystemResourcesList;
    UINT_PTR   ExpPagedPoolDescriptor;
    UINT_PTR   ExpNumberOfPagedPools;

    UINT_PTR   KeTimeIncrement;
    UINT_PTR   KeBugCheckCallbackListHead;
    UINT_PTR   KiBugcheckData;

    UINT_PTR   IopErrorLogListHead;

    UINT_PTR   ObpRootDirectoryObject;
    UINT_PTR   ObpTypeObjectType;

    UINT_PTR   MmSystemCacheStart;
    UINT_PTR   MmSystemCacheEnd;
    UINT_PTR   MmSystemCacheWs;

    UINT_PTR   MmPfnDatabase;
    UINT_PTR   MmSystemPtesStart;
    UINT_PTR   MmSystemPtesEnd;
    UINT_PTR   MmSubsectionBase;
    UINT_PTR   MmNumberOfPagingFiles;

    UINT_PTR   MmLowestPhysicalPage;
    UINT_PTR   MmHighestPhysicalPage;
    UINT_PTR   MmNumberOfPhysicalPages;

    UINT_PTR   MmMaximumNonPagedPoolInBytes;
    UINT_PTR   MmNonPagedSystemStart;
    UINT_PTR   MmNonPagedPoolStart;
    UINT_PTR   MmNonPagedPoolEnd;

    UINT_PTR   MmPagedPoolStart;
    UINT_PTR   MmPagedPoolEnd;
    UINT_PTR   MmPagedPoolAllocationMap;
    UINT_PTR   MmEndOfPagedPoolBitmap;
    UINT_PTR   MmSizeOfPagedPoolInBytes;

    UINT_PTR   MmTotalCommitLimit;
    UINT_PTR   MmTotalCommittedPages;
    UINT_PTR   MmSharedCommit;
    UINT_PTR   MmDriverCommit;
    UINT_PTR   MmProcessCommit;
    UINT_PTR   MmPagedPoolCommit;
    UINT_PTR   MmExtendedCommit;

    UINT_PTR   MmZeroedPageListHead;
    UINT_PTR   MmFreePageListHead;
    UINT_PTR   MmStandbyPageListHead;
    UINT_PTR   MmModifiedPageListHead;
    UINT_PTR   MmModifiedNoWritePageListHead;
    UINT_PTR   MmAvailablePages;
    UINT_PTR   MmResidentAvailablePages;

    UINT_PTR   PoolTrackTable;
    UINT_PTR   NonPagedPoolDescriptor;

    UINT_PTR   MmHighestUserAddress;
    UINT_PTR   MmSystemRangeStart;
    UINT_PTR   MmUserProbeAddress;

    UINT_PTR   KdPrintCircularBuffer;
    UINT_PTR   KdPrintCircularBufferEnd;
    UINT_PTR   KdPrintWritePointer;
    UINT_PTR   KdPrintRolloverCount;

    UINT_PTR   MmLoadedUserImageList;
} KDDEBUGGER_DATA, *PKDDEBUGGER_DATA;

typedef struct _DBGKD_PAGEIN {
    UINT_PTR   Address;
    DWORD   ContinueStatus;
} DBGKD_PAGEIN, *PDBGKD_PAGEIN;

typedef struct _DBGKD_MANIPULATE_STATE {
    DWORD ApiNumber;
    WORD   ProcessorLevel;
    WORD   Processor;
    DWORD    ReturnStatus;
    union {
        DBGKD_READ_MEMORY ReadMemory;
        DBGKD_WRITE_MEMORY WriteMemory;
        DBGKD_READ_MEMORY64 ReadMemory64;
        DBGKD_WRITE_MEMORY64 WriteMemory64;
        DBGKD_GET_CONTEXT GetContext;
        DBGKD_SET_CONTEXT SetContext;
        DBGKD_WRITE_BREAKPOINT WriteBreakPoint;
        DBGKD_RESTORE_BREAKPOINT RestoreBreakPoint;
        DBGKD_CONTINUE Continue;
        DBGKD_CONTINUE2 Continue2;
        DBGKD_READ_WRITE_IO ReadWriteIo;
        DBGKD_READ_WRITE_IO_EXTENDED ReadWriteIoExtended;
        DBGKD_QUERY_SPECIAL_CALLS QuerySpecialCalls;
        DBGKD_SET_SPECIAL_CALL SetSpecialCall;
        DBGKD_SET_INTERNAL_BREAKPOINT SetInternalBreakpoint;
        DBGKD_GET_INTERNAL_BREAKPOINT GetInternalBreakpoint;
        DBGKD_GET_VERSION GetVersion;
        DBGKD_BREAKPOINTEX BreakPointEx;
        DBGKD_PAGEIN PageIn;
        DBGKD_READ_WRITE_MSR ReadWriteMsr;
    } u;
} DBGKD_MANIPULATE_STATE, *PDBGKD_MANIPULATE_STATE;

//
// This is the format for the trace data passed back from the kernel to
// the debugger to describe multiple calls that have returned since the
// last trip back.  The basic format is that there are a bunch of these
// (4 byte) unions stuck together.  Each union is of one of two types: a
// 4 byte unsigned long interger, or a three field struct, describing a
// call (where "call" is delimited by returning or exiting the symbol
// scope).  If the number of instructions executed is too big to fit
// into a WORD   -1, then the Instructions field has
// TRACE_DATA_INSTRUCTIONS_BIG and the next union is a LongNumber
// containing the real number of instructions executed.
//
// The very first union returned in each callback is a LongNumber
// containing the number of unions returned (including the "size"
// record, os it's always at least 1 even if there's no data to return).
//
// This is all returned to the debugger when one of two things
// happens:
//
//   1) The pc moves out of all defined symbol ranges
//   2) The buffer of trace data entries is filled.
//
// The "trace done" case is hacked around on the debugger side.  It
// guarantees that the pc address that indicates a trace exit never
// winds up in a defined symbol range.
//
// The only other complexity in this system is handling the SymbolNumber
// table.  This table is kept in parallel by the kernel and the
// debugger.  When the PC exits a known symbol range, the Begin and End
// symbol ranges are set by the debugger and are allocated to the next
// symbol slot upon return.  "The next symbol slot" means the numerical
// next slot number, unless we've filled all slots, in which case it is
// #0.  (ie., allocation is cyclic and not LRU or something).  The
// SymbolNumber table is flushed when a SpecialCalls call is made (ie.,
// at the beginning of the WatchTrace).
//

typedef union _DBGKD_TRACE_DATA {
    struct {
        BYTE  SymbolNumber;
        CHAR LevelChange;
        WORD   Instructions;
    } s;
    DWORD LongNumber;
} DBGKD_TRACE_DATA, *PDBGKD_TRACE_DATA;

#define TRACE_DATA_INSTRUCTIONS_BIG 0xffff

#define TRACE_DATA_BUFFER_MAX_SIZE 40

//
// If the packet type is PACKET_TYPE_KD_DEBUG_IO, then
// the format of the packet data is as follows:
//

#define DbgKdPrintStringApi     0x00003230L
#define DbgKdGetStringApi       0x00003231L

//
// For print string, the Null terminated string to print
// immediately follows the message
//
typedef struct _DBGKD_PRINT_STRING {
    DWORD LengthOfString;
} DBGKD_PRINT_STRING, *PDBGKD_PRINT_STRING;

//
// For get string, the Null terminated promt string
// immediately follows the message. The LengthOfStringRead
// field initially contains the maximum number of characters
// to read. Upon reply, this contains the number of bytes actually
// read. The data read immediately follows the message.
//
//
typedef struct _DBGKD_GET_STRING {
    DWORD LengthOfPromptString;
    DWORD LengthOfStringRead;
} DBGKD_GET_STRING, *PDBGKD_GET_STRING;

typedef struct _DBGKD_DEBUG_IO {
    DWORD ApiNumber;
    WORD   ProcessorLevel;
    WORD   Processor;
    union {
        DBGKD_PRINT_STRING PrintString;
        DBGKD_GET_STRING GetString;
    } u;
} DBGKD_DEBUG_IO, *PDBGKD_DEBUG_IO;


VOID
NTAPI
DbgKdSendBreakIn(
    VOID
    );

PBYTE 
NTAPI
DbgKdGets(
    PBYTE  Buffer,
    WORD   Length
    );

DWORD   
NTAPI
DbgKdWaitStateChange(
    OUT PDBGKD_WAIT_STATE_CHANGE StateChange,
    OUT PVOID Buffer,
    IN DWORD BufferLength
    );

DWORD   
NTAPI
DbgKdContinue (
    IN DWORD    ContinueStatus
    );

DWORD   
NTAPI
DbgKdContinue2 (
    IN DWORD    ContinueStatus,
    IN DBGKD_CONTROL_SET ControlSet
    );

DWORD   
NTAPI
DbgKdReadVirtualMemory(
    IN PVOID TargetBaseAddress,
    OUT PVOID UserInterfaceBuffer,
    IN DWORD TransferCount,
    OUT PDWORD ActualBytesRead OPTIONAL
    );

DWORD   
NTAPI
DbgKdWriteVirtualMemory(
    IN PVOID TargetBaseAddress,
    OUT PVOID UserInterfaceBuffer,
    IN DWORD TransferCount,
    OUT PDWORD ActualBytesWritten OPTIONAL
    );

DWORD   
NTAPI
DbgKdReadPhysicalMemory(
    IN PHYSICAL_ADDRESS TargetBaseAddress,
    OUT PVOID UserInterfaceBuffer,
    IN DWORD TransferCount,
    OUT PDWORD ActualBytesRead OPTIONAL
    );

DWORD   
NTAPI
DbgKdWritePhysicalMemory(
    IN PHYSICAL_ADDRESS TargetBaseAddress,
    OUT PVOID UserInterfaceBuffer,
    IN DWORD TransferCount,
    OUT PDWORD ActualBytesWritten OPTIONAL
    );

DWORD   
NTAPI
DbgKdReadControlSpace(
    IN WORD   Processor,
    IN PVOID TargetBaseAddress,
    OUT PVOID UserInterfaceBuffer,
    IN DWORD TransferCount,
    OUT PDWORD ActualBytesRead OPTIONAL
    );

DWORD   
NTAPI
DbgKdWriteControlSpace(
    IN WORD   Processor,
    IN PVOID TargetBaseAddress,
    OUT PVOID UserInterfaceBuffer,
    IN DWORD TransferCount,
    OUT PDWORD ActualBytesWritten OPTIONAL
    );

DWORD   
NTAPI
DbgKdReadIoSpace(
    IN PVOID IoAddress,
    OUT PVOID ReturnedData,
    IN DWORD DataSize
    );

DWORD   
NTAPI
DbgKdWriteIoSpace(
    IN PVOID IoAddress,
    IN DWORD DataValue,
    IN DWORD DataSize
    );

DWORD   
NTAPI
DbgKdReadMsr(
    IN DWORD MsrReg,
    OUT PDWORDLONG MsrValue
    );

DWORD   
NTAPI
DbgKdWriteMsr(
    IN DWORD MsrReg,
    IN DWORDLONG MsrValue
    );


DWORD   
NTAPI
DbgKdGetContext(
    IN WORD   Processor,
    IN OUT PCONTEXT Context
    );

DWORD   
NTAPI
DbgKdSetContext(
    IN WORD   Processor,
    IN PCONTEXT Context
    );

DWORD   
NTAPI
DbgKdWriteBreakPoint(
    IN PVOID BreakPointAddress,
    OUT PDWORD BreakPointHandle
    );

DWORD   
NTAPI
DbgKdRestoreBreakPoint(
    IN DWORD BreakPointHandle
    );

DWORD   
NTAPI
DbgKdReboot(
    VOID
    );

#ifdef _X86_
DWORD   
NTAPI
DbgKdLookupSelector(
    IN WORD   Processor,
    IN OUT PDESCRIPTOR_TABLE_ENTRY pDescriptorTableEntry
    );
#endif

#define DBG_EXCEPTION_HANDLED           ((DWORD   )0x00010001L) 

#endif // _WINDBGKD_

