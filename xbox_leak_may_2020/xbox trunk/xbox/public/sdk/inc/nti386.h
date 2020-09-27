/*++ BUILD Version: 0002    // Increment this if a change has global effects

Copyright (c) 1989-1999  Microsoft Corporation

Module Name:

    nti386.h

Abstract:

    User-mode visible i386 specific i386 structures and constants

Author:

    Mark Lucovsky (markl) 30-Nov-1989

Revision History:

    Bryan Willman (bryanwi) 8-Jan-90

        port to the 386

--*/

#ifndef _NTI386_
#define _NTI386_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

// begin_ntddk begin_wdm begin_nthal begin_ntifs

#if defined(_X86_)

//
// Define system time structure.
//

typedef struct _KSYSTEM_TIME {
    ULONG LowPart;
    LONG High1Time;
    LONG High2Time;
} KSYSTEM_TIME, *PKSYSTEM_TIME;

#endif

// end_ntddk end_wdm end_nthal end_ntifs

// begin_windbgkd

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
    ULONG   Dr6;
    ULONG   Dr7;
    USHORT  InstructionCount;
    USHORT  ReportFlags;
    UCHAR   InstructionStream[DBGKD_MAXSTREAM];
    USHORT  SegCs;
    USHORT  SegDs;
    USHORT  SegEs;
    USHORT  SegFs;
    ULONG   EFlags;
} DBGKD_CONTROL_REPORT, *PDBGKD_CONTROL_REPORT;

#define REPORT_INCLUDES_SEGS    0x0001  // this is for backward compatibility

//
// DBGKD_CONTROL_SET
//
// This structure control value the debugger wants to set on every
// continue, and thus sets here to avoid packet traffic.
//

typedef struct _DBGKD_CONTROL_SET {
    ULONG   TraceFlag;                  // WARNING: This must NOT be a BOOLEAN,
                                        //     or host and target will end
                                        //     up with different alignments!
    ULONG   Dr7;
    ULONG   CurrentSymbolStart;         // Range in which to trace locally
    ULONG   CurrentSymbolEnd;
} DBGKD_CONTROL_SET, *PDBGKD_CONTROL_SET;

#endif //_X86_

// end_windbgkd


// begin_ntddk begin_wdm begin_nthal begin_winnt begin_ntminiport begin_wx86

#ifdef _X86_

//
// Disable these two pramas that evaluate to "sti" "cli" on x86 so that driver
// writers to not leave them inadvertantly in their code.
//

#if !defined(MIDL_PASS)
#if !defined(RC_INVOKED)

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4164)   // disable C4164 warning so that apps that
                                // build with /Od don't get weird errors !
#ifdef _M_IX86
#pragma function(_enable)
#pragma function(_disable)
#endif

#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4164)   // reenable C4164 warning
#endif

#endif
#endif

// end_ntddk end_wdm end_nthal end_winnt end_ntminiport end_wx86

//
//  Values put in ExceptionRecord.ExceptionInformation[0]
//  First parameter is always in ExceptionInformation[1],
//  Second parameter is always in ExceptionInformation[2]
//

#define BREAKPOINT_BREAK            0
#define BREAKPOINT_PRINT            1
#define BREAKPOINT_PROMPT           2
#define BREAKPOINT_LOAD_SYMBOLS     3
#define BREAKPOINT_UNLOAD_SYMBOLS   4
#define BREAKPOINT_RTLASSERT        5
#define BREAKPOINT_RIP              6
#define BREAKPOINT_LOAD_XESECTION   7
#define BREAKPOINT_UNLOAD_XESECTION 8
#define BREAKPOINT_CREATE_FIBER     9
#define BREAKPOINT_DELETE_FIBER     10
#define BREAKPOINT_KDPRINT          11


//
// Define Address of User Shared Data
//

#if defined(MIDL_PASS) || !defined(_M_IX86)
struct _TEB *
NTAPI
NtCurrentTeb( void );
#else
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning (disable:4035)        // disable 4035 (function must return something)
#define PcTeb 0x18
FORCEINLINE struct _TEB * NtCurrentTeb( void ) { __asm mov eax, fs:[PcTeb] }
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning (default:4035)        // reenable it
#endif
#endif // defined(MIDL_PASS) || defined(__cplusplus) || !defined(_M_IX86)

// begin_ntddk begin_nthal
//
// Size of kernel mode stack.
//

#define KERNEL_STACK_SIZE 12288

//
// Size of kernel mode stack minus the overhead associated with the FPU save
// area and thread local storage area.  A thread's stack should have at least
// this many bytes available for actual stack usage.
//

#define KERNEL_USABLE_STACK_SIZE (KERNEL_STACK_SIZE - sizeof(FX_SAVE_AREA) - 512)

// end_ntddk end_nthal

#define DOUBLE_FAULT_STACK_SIZE 4096

//
// Call frame record definition.
//
// There is no standard call frame for NT/386, but there is a linked
// list structure used to register exception handlers, this is it.
//

// begin_nthal
//
// Exception Registration structure
//

typedef struct _EXCEPTION_REGISTRATION_RECORD {
    struct _EXCEPTION_REGISTRATION_RECORD *Next;
    PEXCEPTION_ROUTINE Handler;
} EXCEPTION_REGISTRATION_RECORD;

typedef EXCEPTION_REGISTRATION_RECORD *PEXCEPTION_REGISTRATION_RECORD;

//
// Define constants for system IDTs
//

#define MAXIMUM_IDTVECTOR 0xff
#define MAXIMUM_PRIMARY_VECTOR 0xff
#define PRIMARY_VECTOR_BASE 0x30        // 0-2f are x86 trap vectors

// begin_ntddk
#ifdef _X86_
// end_ntddk

// begin_winnt
// begin_ntddk begin_wx86

//
//  Define the size of the 80387 save area, which is in the context frame.
//

#define SIZE_OF_80387_REGISTERS      80

//
// Define the size of FP registers in the FXSAVE format.
//

#define SIZE_OF_FX_REGISTERS        128

//
// The following flags control the contents of the CONTEXT structure.
//

#if !defined(RC_INVOKED)

#define CONTEXT_i386    0x00010000    // this assumes that i386 and
#define CONTEXT_i486    0x00010000    // i486 have identical context records

// end_wx86

#define CONTEXT_CONTROL         (CONTEXT_i386 | 0x00000001L) // SS:SP, CS:IP, FLAGS, BP
#define CONTEXT_INTEGER         (CONTEXT_i386 | 0x00000002L) // AX, BX, CX, DX, SI, DI
#define CONTEXT_SEGMENTS        (CONTEXT_i386 | 0x00000004L) // DS, ES, FS, GS
#define CONTEXT_FLOATING_POINT  (CONTEXT_i386 | 0x00000008L) // 387 state
#define CONTEXT_DEBUG_REGISTERS (CONTEXT_i386 | 0x00000010L) // DB 0-3,6,7
#define CONTEXT_EXTENDED_REGISTERS  (CONTEXT_i386 | 0x00000020L) // cpu specific extensions

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_INTEGER |\
                      CONTEXT_SEGMENTS)

// begin_wx86

#endif

#define MAXIMUM_SUPPORTED_EXTENSION     512

#include <pshpack1.h>
typedef struct _FLOATING_SAVE_AREA {
    USHORT  ControlWord;
    USHORT  StatusWord;
    USHORT  TagWord;
    USHORT  ErrorOpcode;
    ULONG   ErrorOffset;
    ULONG   ErrorSelector;
    ULONG   DataOffset;
    ULONG   DataSelector;
    ULONG   MXCsr;
    ULONG   Reserved2;
    UCHAR   RegisterArea[SIZE_OF_FX_REGISTERS];
    UCHAR   XmmRegisterArea[SIZE_OF_FX_REGISTERS];
    UCHAR   Reserved4[224];
    ULONG   Cr0NpxState;
} FLOATING_SAVE_AREA;
#include <poppack.h>

typedef FLOATING_SAVE_AREA *PFLOATING_SAVE_AREA;

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) is is used to constuct a call frame for APC delivery,
//  and 3) it is used in the user level thread creation routines.
//
//  The layout of the record conforms to a standard call frame.
//

typedef struct _CONTEXT {

    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a threads context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    ULONG ContextFlags;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
    //

    FLOATING_SAVE_AREA FloatSave;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_INTEGER.
    //

    ULONG   Edi;
    ULONG   Esi;
    ULONG   Ebx;
    ULONG   Edx;
    ULONG   Ecx;
    ULONG   Eax;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_CONTROL.
    //

    ULONG   Ebp;
    ULONG   Eip;
    ULONG   SegCs;              // MUST BE SANITIZED
    ULONG   EFlags;             // MUST BE SANITIZED
    ULONG   Esp;
    ULONG   SegSs;

} CONTEXT;

typedef CONTEXT *PCONTEXT;

// begin_ntminiport

#endif //_X86_

// end_ntddk end_nthal end_winnt end_ntminiport end_wx86

typedef struct _FX_SAVE_AREA {
    FLOATING_SAVE_AREA FloatSave;
    ULONG Align16Byte[3];
} FX_SAVE_AREA, *PFX_SAVE_AREA;

#define CONTEXT_TO_PROGRAM_COUNTER(Context) ((Context)->Eip)

#define CONTEXT_LENGTH  (sizeof(CONTEXT))
#define CONTEXT_ALIGN   (sizeof(ULONG))
#define CONTEXT_ROUND   (CONTEXT_ALIGN - 1)

// begin_wx86
//
//  GDT selectors - These defines are R0 selector numbers, which means
//                  they happen to match the byte offset relative to
//                  the base of the GDT.
//

#define KGDT_NULL       0
#define KGDT_R0_CODE    8
#define KGDT_R0_DATA    16
#define KGDT_TSS        24
#define KGDT_R0_PCR     32
#define KGDT_DF_TSS     40
#define KGDT_NMI_TSS    48

// end_wx86

//
//  LDT descriptor entry
//

// begin_winnt begin_wx86

#ifndef _LDT_ENTRY_DEFINED
#define _LDT_ENTRY_DEFINED

typedef struct _LDT_ENTRY {
    USHORT  LimitLow;
    USHORT  BaseLow;
    union {
        struct {
            UCHAR   BaseMid;
            UCHAR   Flags1;     // Declare as bytes to avoid alignment
            UCHAR   Flags2;     // Problems.
            UCHAR   BaseHi;
        } Bytes;
        struct {
            ULONG   BaseMid : 8;
            ULONG   Type : 5;
            ULONG   Dpl : 2;
            ULONG   Pres : 1;
            ULONG   LimitHi : 4;
            ULONG   Sys : 1;
            ULONG   Reserved_0 : 1;
            ULONG   Default_Big : 1;
            ULONG   Granularity : 1;
            ULONG   BaseHi : 8;
        } Bits;
    } HighWord;
} LDT_ENTRY, *PLDT_ENTRY;

#endif

// end_winnt end_wx86

//
// Process Ldt Information
//  NtQueryInformationProcess using ProcessLdtInformation
//

typedef struct _LDT_INFORMATION {
    ULONG Start;
    ULONG Length;
    LDT_ENTRY LdtEntries[1];
} PROCESS_LDT_INFORMATION, *PPROCESS_LDT_INFORMATION;

//
// Process Ldt Size
//  NtSetInformationProcess using ProcessLdtSize
//

typedef struct _LDT_SIZE {
    ULONG Length;
} PROCESS_LDT_SIZE, *PPROCESS_LDT_SIZE;

//
// Thread Descriptor Table Entry
//  NtQueryInformationThread using ThreadDescriptorTableEntry
//

// begin_windbgkd

#ifndef _DESCRIPTOR_TABLE_ENTRY_DEFINED
#define _DESCRIPTOR_TABLE_ENTRY_DEFINED

typedef struct _DESCRIPTOR_TABLE_ENTRY {
    ULONG Selector;
    LDT_ENTRY Descriptor;
} DESCRIPTOR_TABLE_ENTRY, *PDESCRIPTOR_TABLE_ENTRY;

#endif // _DESCRIPTOR_TABLE_ENTRY_DEFINED

// end_windbgkd

// begin_ntddk begin_wdm begin_nthal
#endif // _X86_
// end_ntddk end_wdm end_nthal

//
// Library function prototypes.
//

VOID
RtlCaptureContext (
    OUT PCONTEXT ContextRecord
    );

//
// Additional information supplied in QuerySectionInformation for images.
//

#define SECTION_ADDITIONAL_INFO_USED 0

#ifdef __cplusplus
}
#endif

#endif // _NTI386_
