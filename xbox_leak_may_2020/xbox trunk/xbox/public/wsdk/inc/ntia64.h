
/*++ BUILD Version: 0011    // Increment this if a change has global effects

//#include intelcopy.h

Module Name:

    ntia64.h

Abstract:

    User-mode visible IA64 specific structures and constants

Author:

    Bernard Lint     21-jun-95

Revision History:

--*/

#ifndef _NTIA64H_
#define _NTIA64H_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "ia64inst.h"


// begin_ntddk begin_wdm begin_nthal
#ifdef _IA64_

//
// Define system time structure (use alpha approach).
//

typedef ULONGLONG KSYSTEM_TIME;
typedef KSYSTEM_TIME *PKSYSTEM_TIME;

#endif // _IA64_
// end_ntddk end_wdm end_nthal

// begin_windbgkd

#ifdef _IA64_

//
// Define IA64 specific kernel debugger information.
//
// The following structure contains machine specific data passed to
// the host system kernel debugger in a wait state change message.
//

#define DBGKD_MAXSTREAM 16

typedef struct _DBGKD_CONTROL_REPORT {
    ULONG InstructionCount;
    UCHAR InstructionStream[DBGKD_MAXSTREAM];
} DBGKD_CONTROL_REPORT, *PDBGKD_CONTROL_REPORT;

//
// The following structure contains information that the host system
// kernel debugger wants to set on every continue operation and avoids
// the need to send extra packets of information.
//

typedef ULONG DBGKD_CONTROL_SET, *PDBGKD_CONTROL_SET;

#endif // _IA64_

// end_windbgkd

// begin_ntddk begin_wdm begin_nthal

#ifdef _IA64_

// end_ntddk end_wdm end_nthal

//
// Define breakpoint codes.
//

//
// Define BREAK immediate usage
//      IA64 conventions for 21 bit break immediate:
//        all zeroes : reserved for break.b instruction
//        000xxxx... : system software interrupts (overflow, divide by zero...)
//        001xxxx... : application software interrupts (reserved for user code)
//        01xxxxx... : debug breakpoints
//        10xxxxx... : system level debug (reserved for subsystems)
//        110xxxx... : normal system calls
//        111xxxx... : fast path system calls for event pair
//

#define BREAK_SSI_BASE      0x000000
#define BREAK_APP_BASE      0x040000
#define BREAK_APP_SUBSYSTEM_OFFSET 0x008000 // used to debug subsystem
#define BREAK_DEBUG_BASE    0x080000
#define BREAK_SYSCALL_BASE  0x180000
#define BREAK_FASTSYS_BASE  0x1C0000

//
// Define system break values
//

#define BREAKB_BREAKPOINT               (BREAK_SSI_BASE | 0) // reserved for break.b - do not use
#define KERNEL_BREAKPOINT               (BREAK_SSI_BASE | 1) // kernel breakpoint
#define USER_BREAKPOINT                 (BREAK_SSI_BASE | 2) // user breakpoint
#define INTEGER_OVERFLOW_BREAKPOINT     (BREAK_SSI_BASE | 5) // integer overflow breakpoint
#define DIVIDE_OVERFLOW_BREAKPOINT      (BREAK_SSI_BASE | 6) // divide overflow breakpoint
#define DIVIDE_BY_ZERO_BREAKPOINT       (BREAK_SSI_BASE | 7) // divide by zero breakpoint
#define RANGE_CHECK_BREAKPOINT          (BREAK_SSI_BASE | 8) // range check breakpoint
#define STACK_OVERFLOW_BREAKPOINT       (BREAK_SSI_BASE | 9) // stack overflow
#define MULTIPLY_OVERFLOW_BREAKPOINT    (BREAK_SSI_BASE |10) // multiply overflow breakpoint

//
// Define debug related break values
// N.B. KdpTrap() checks for break value >= DEBUG_PRINT_BREAKPOINT
//

#define DEBUG_PRINT_BREAKPOINT          (BREAK_DEBUG_BASE | 20) // debug print breakpoint
#define DEBUG_PROMPT_BREAKPOINT         (BREAK_DEBUG_BASE | 21) // debug prompt breakpoint
#define DEBUG_STOP_BREAKPOINT           (BREAK_DEBUG_BASE | 22) // debug stop breakpoint
#define DEBUG_LOAD_SYMBOLS_BREAKPOINT   (BREAK_DEBUG_BASE | 23) // load symbols breakpoint
#define DEBUG_UNLOAD_SYMBOLS_BREAKPOINT (BREAK_DEBUG_BASE | 24) // unload symbols breakpoint
#define BREAKIN_BREAKPOINT              (BREAK_DEBUG_BASE | 25) // break into kernel debugger

//
// Define IA64 specific read control space commands for the
// Kernel Debugger.
//

#define DEBUG_CONTROL_SPACE_PCR       1
#define DEBUG_CONTROL_SPACE_PRCB      2
#define DEBUG_CONTROL_SPACE_KSPECIAL  3
#define DEBUG_CONTROL_SPACE_THREAD    4

//
// System call break
//

#define BREAK_SYSCALL   BREAK_SYSCALL_BASE

//
// Define special fast path even pair client/server system service codes.
//
// N.B. These codes are VERY special. The high three bits signifies a fast path
//      event pair service and the low bit signifies what type.
//

#define BREAK_SET_LOW_WAIT_HIGH (BREAK_FASTSYS_BASE|0x20) // fast path event pair service
#define BREAK_SET_HIGH_WAIT_LOW (BREAK_FASTSYS_BASE|0x10) // fast path event pair service

//
// Special subsystem break codes: 0x048xxx (from application software interrupt space)
//

#define BREAK_SUBSYSTEM_BASE  0x048000

// begin_ntddk begin_nthal
// Please contact INTEL to get IA64-specific information
//
// Define size of kernel mode stack.
//

#define KERNEL_STACK_SIZE 0x8000

//
// Define size of large kernel mode stack for callbacks.
//

#define KERNEL_LARGE_STACK_SIZE 0xC000

//
// Define number of pages to initialize in a large kernel stack.
//

#define KERNEL_LARGE_STACK_COMMIT 0x8000

//

//

#define KERNEL_BSTORE_SIZE 0x4000

//

//

#define KERNEL_LARGE_BSTORE_SIZE 0x8000

//

//

#define KERNEL_LARGE_BSTORE_COMMIT 0x4000

//
// Define base address for kernel and user space
//

#define UREGION_INDEX 0

#define KREGION_INDEX 7

#define UADDRESS_BASE ((ULONGLONG)UREGION_INDEX << 61)


#define KADDRESS_BASE ((ULONGLONG)KREGION_INDEX << 61)

// end_ntddk end_nthal


//
// Define address of data shared between user and kernel mode.
// Alas, the MM_SHARED_USER_DATA_VA needs to be below 2G for
// compatibility reasons
//

#define MM_SHARED_USER_DATA_VA   (UADDRESS_BASE + 0x7FFE0000)

#define USER_SHARED_DATA ((KUSER_SHARED_DATA * const)MM_SHARED_USER_DATA_VA)

//
// Define address of the wow64 reserved compatibility area.
// The offset needs to be large enough that the CSRSS can fit it's data
// See csr\srvinit.c and the refrence the SharedSection key in the registry
//
#define WOW64_COMPATIBILITY_AREA_ADDRESS  (MM_SHARED_USER_DATA_VA - 0x1000000)

//
// Define address of the system-wide csrss shared section.
//
#define CSR_SYSTEM_SHARED_ADDRESS (WOW64_COMPATIBILITY_AREA_ADDRESS)

//
// Define address for epc system calls
//

// Please contact INTEL to get IA64-specific information
// @@BEGIN_DDKSPLIT
#define MM_EPC_VA (KADDRESS_BASE + 0xFFA00000)
// @@END_DDKSPLIT

//
// Call frame record definition.
//
// There is no standard call frame for NT/386, but there is a linked
// list structure used to register exception handlers, this is it.
//

//
// begin_nthal
// Exception Registration structure
//

typedef struct _EXCEPTION_REGISTRATION_RECORD {
    struct _EXCEPTION_REGISTRATION_RECORD *Next;
    PEXCEPTION_ROUTINE Handler;
} EXCEPTION_REGISTRATION_RECORD;

// end_nthal

typedef EXCEPTION_REGISTRATION_RECORD *PEXCEPTION_REGISTRATION_RECORD;

//
// Define function to return the current Thread Environment Block
//

// stub this out for midl compiler.
// idl files incude this header file for the typedefs and #defines etc.
// midl never generates stubs for the functions declared here.
//

//
// Don't define for GENIA64 since GENIA64.C is built with 386 compiler.
//

// begin_winnt

#if !defined(__midl) && !defined(GENUTIL) && !defined(_GENIA64_) && defined(_IA64_)

// Please contact INTEL to get IA64-specific information
// @@BEGIN_DDKSPLIT
void * _cdecl _rdteb(void);
#if defined(_M_IA64)                    // winnt
#pragma intrinsic(_rdteb)               // winnt
#endif                                  // winnt
#define NtCurrentTeb()      ((struct _TEB *)_rdteb())
// @@END_DDKSPLIT

//
// Define functions to get the address of the current fiber and the
// current fiber data.
//

#define GetCurrentFiber() (((PNT_TIB)NtCurrentTeb())->FiberData)
#define GetFiberData() (*(PVOID *)(GetCurrentFiber()))

#endif  // !defined(__midl) && !defined(GENUTIL) && !defined(_GENIA64_) && defined(_M_IA64)

#ifdef _IA64_

// begin_ntddk begin_nthal

//
// The following flags control the contents of the CONTEXT structure.
//

#if !defined(RC_INVOKED)

// Please contact INTEL to get IA64-specific information
// @@BEGIN_DDKSPLIT










// @@END_DDKSPLIT

#endif // !defined(RC_INVOKED)

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) it is used to construct a call frame for APC delivery,
//  3) it is used to construct a call frame for exception dispatching
//  in user mode, 4) it is used in the user level thread creation
//  routines, and 5) it is used to to pass thread state to debuggers.
//
//  N.B. Because this record is used as a call frame, it must be EXACTLY
//  a multiple of 16 bytes in length and aligned on a 16-byte boundary.
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
    // is being used to modify a thread's context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    ULONG ContextFlags;
    ULONG Fill1[3];         // for alignment of following on 16-byte boundary

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_DEBUG.
    //
    // N.B. CONTEXT_DEBUG is *not* part of CONTEXT_FULL.
    //

// Please contact INTEL to get IA64-specific information
// @@BEGIN_DDKSPLIT


















    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_LOWER_FLOATING_POINT.
    //
















    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_HIGHER_FLOATING_POINT.
    //




























































































































    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_LOWER_FLOATING_POINT | CONTEXT_HIGHER_FLOATING_POINT | CONTEXT_CONTROL.
    //



    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_INTEGER.
    //
    // N.B. The registers gp, sp, rp are part of the control context
    //














































    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_CONTROL.
    //

    // Other application registers






    // Register stack info






    // Trap Status Information




    // iA32 related control registers










// @@END_DDKSPLIT

} CONTEXT, *PCONTEXT;

//
// Followings are the data structures that required to support Legacy x86
//


//
//  Define the size of the 80387 save area, which is in the context frame.
//

#define SIZE_OF_80387_REGISTERS      80

//
// The following flags control the contents of the CONTEXT structure.
//

#if !defined(RC_INVOKED)

#define CONTEXT_i386    0x00010000    // this assumes that i386 and
#define CONTEXT_i486    0x00010000    // i486 have identical context records
#define CONTEXT_X86     0x00010000    // X86 have identical context records

#define CONTEXT86_CONTROL         (CONTEXT_i386 | 0x00000001L) // SS:SP, CS:IP, FLAGS, BP
#define CONTEXT86_INTEGER         (CONTEXT_i386 | 0x00000002L) // AX, BX, CX, DX, SI, DI
#define CONTEXT86_SEGMENTS        (CONTEXT_i386 | 0x00000004L) // DS, ES, FS, GS
#define CONTEXT86_FLOATING_POINT  (CONTEXT_i386 | 0x00000008L) // 387 state
#define CONTEXT86_DEBUG_REGISTERS (CONTEXT_i386 | 0x00000010L) // DB 0-3,6,7

#define CONTEXT86_FULL (CONTEXT86_CONTROL | CONTEXT86_INTEGER |\
                      CONTEXT86_SEGMENTS)    // context corresponding to set flags will be returned.

#endif // !defined(RC_INVOKED)

typedef struct _FLOATING_SAVE_AREA {
    ULONG   ControlWord;
    ULONG   StatusWord;
    ULONG   TagWord;
    ULONG   ErrorOffset;
    ULONG   ErrorSelector;
    ULONG   DataOffset;
    ULONG   DataSelector;
    UCHAR   RegisterArea[SIZE_OF_80387_REGISTERS];
    ULONG   Cr0NpxState;
} FLOATING_SAVE_AREA;

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

typedef struct _CONTEXT86 {

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
    // This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
    // set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
    // included in CONTEXT_FULL.
    //

    ULONG   Dr0;
    ULONG   Dr1;
    ULONG   Dr2;
    ULONG   Dr3;
    ULONG   Dr6;
    ULONG   Dr7;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
    //

    FLOATING_SAVE_AREA FloatSave;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_SEGMENTS.
    //

    ULONG   SegGs;
    ULONG   SegFs;
    ULONG   SegEs;
    ULONG   SegDs;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_INTEGER.
    //
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

} CONTEXT86;

// end_winnt

typedef CONTEXT86 *PCONTEXT86;

#define CONTEXT86_TO_PROGRAM_COUNTER(Context) (PVOID)((Context)->Eip)

#define CONTEXT86_LENGTH  (sizeof(CONTEXT86))
#define CONTEXT86_ALIGN   (sizeof(ULONG))
#define CONTEXT86_ROUND   (CONTEXT86_ALIGN - 1)

//
//  GDT selectors - These defines are R0 selector numbers, which means
//                  they happen to match the byte offset relative to
//                  the base of the GDT.
//

#define KGDT_NULL       0
#define KGDT_R0_CODE    8
#define KGDT_R0_DATA    16
#define KGDT_R3_CODE    24
#define KGDT_R3_DATA    32
#define KGDT_TSS        40
#define KGDT_R0_PCR     48
#define KGDT_R3_TEB     56
#define KGDT_VDM_TILE   64
#define KGDT_LDT        72
#define KGDT_DF_TSS     80
#define KGDT_NMI_TSS    88

//
//  GDT selectors - These defines are R0 selector numbers, which means
//                  they happen to match the byte offset relative to
//                  the base of the GDT.
//

#define KGDT_NULL       0
#define KGDT_R3_CODE    24
#define KGDT_R3_DATA    32
#define KGDT_R3_TEB     56
#define KGDT_VDM_TILE   64

//
//  GDT table is now reside in TEB structure, and the entries can only
//  accomerdate 24 entries.  We purposely take away KGDT_NUMBER and use
//  GDT_TABLE_SIZE
//

#define GDT_ENTRIES 24
#define GDT_TABLE_SIZE  (GDT_ENTRIES<<3)


// begin_winnt

#ifndef _LDT_ENTRY_DEFINED
#define _LDT_ENTRY_DEFINED

//
//  LDT descriptor entry
//

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

#endif // _LDT_ENTRY_DEFINED

// end_winnt

#define LDT_TABLE_SIZE  ( 8 * 1024 * sizeof(LDT_ENTRY) )

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

// begin_winnt

//
// Plabel descriptor structure definition
//

// Please contact INTEL to get IA64-specific information
// @@BEGIN_DDKSPLIT




// @@END_DDKSPLIT

// end_winnt

// end_ntddk end_nthal


// begin_winnt

#endif // _IA64_

// end_winnt

// Please contact INTEL to get IA64-specific information
// @@BEGIN_DDKSPLIT






// @@END_DDKSPLIT

//
// Nonvolatile context pointer record.
//
// See i386.h, this structure follows the 386 implementation. The
// architecture currently doesn't have any nonvolatile kernel context
// as we capture everything in either the trap or exception frames on
// transition from user to kernel mode. We allocate a single bogus
// pointer field as usually this structure is made up of pointers to
// places in the kernel stack where the various nonvolatile items were
// pushed on to the kernel stack.
//
// TBD *** Need to fill in this structure with the relavent fields
//         when we start storing the nonvolatile information only when
//         necessary.
//

typedef struct _KNONVOLATILE_CONTEXT_POINTERS {
// Please contact INTEL to get IA64-specific information
// @@BEGIN_DDKSPLIT



















































// @@END_DDKSPLIT

} KNONVOLATILE_CONTEXT_POINTERS, *PKNONVOLATILE_CONTEXT_POINTERS;

// begin_nthal
//
// Define address space layout as defined by IA64 memory management.
//
// The names come from MIPS hardwired virtual to first 512MB real.
// We use these values to define the size of the PowerPC kernel BAT.
// Must coordinate with values in ../private/mm/ia64/miia64.h.
//
//

#define KUSEG_BASE (UADDRESS_BASE + 0x0)                  // base of user segment
#define KSEG0_BASE (KADDRESS_BASE + 0x80000000)           // base of kernel
#define KSEG2_BASE (KADDRESS_BASE + 0xA0000000)           // end of kernel

// IA64 Register Definitions

//
// Since Apps don't need to know HARDWARE_PTE, the definition moves to
// NTOS\INC\IA64.H as Alpha and i386 port.
//
// Please contact INTEL to get IA64-specific information
// @@BEGIN_DDKSPLIT

#if !(defined(MIDL_PASS) || defined(__midl))
// Processor Status Register (PSR) structure


// User/System mask







// System mask only
















// Neither



















//
// Define hardware Floating Point Status Register.
//

// Floating Point Status Register (FPSR) structure






































































//

//
























//

//

















//

//


























#endif // MIDL_PASS

//

//


























//


//

//

//





//


//

//
//





//


//

//
//




//

//
















//

//






//


















//

//






































// @@END_DDKSPLIT

// end_nthal

// Please contact INTEL to get IA64-specific information
// @@BEGIN_DDKSPLIT
//
// Define IA64 exception handling structures and function prototypes.
// **** TBD ****

//
// Unwind information structure definition.
//
// N.B. If the EHANDLER flag is set, personality routine should be calle
//      during search for an exception handler.  If the UHANDLER flag is
//      set, the personality routine should be called during the second
//      unwind.
//



// @@END_DDKSPLIT

#define GetLanguageSpecificData(f, base)                                     \
    ((PVOID)(base + f->UnwindInfoAddress + sizeof(UNWIND_INFO) +             \
        ((PUNWIND_INFO)(base+f->UnwindInfoAddress))->DataLength*sizeof(ULONGLONG) + sizeof(ULONGLONG)))

// Please contact INTEL to get IA64-specific information
// @@BEGIN_DDKSPLIT






//
// Function table entry structure definition.
//






// @@END_DDKSPLIT

//
// Scope table structure definition - for electron C compiler.
//
// One table entry is created by the acc C compiler for each try-except or
// try-finally scope. Nested scopes are ordered from inner to outer scope.
// Current scope is passively maintained by PC-mapping (function tables).
//

typedef struct _SCOPE_TABLE {
    ULONG Count;
    struct
    {
        ULONG BeginAddress;
        ULONG EndAddress;
        ULONG HandlerAddress;                  // filter/termination handler
        ULONG JumpTarget;                      // continuation address
                                               // e.g. exception handler
    } ScopeRecord[1];
} SCOPE_TABLE, *PSCOPE_TABLE;


//
//
// Runtime Library function prototypes.
//

VOID
RtlCaptureContext (
    OUT PCONTEXT ContextRecord
    );

// Please contact INTEL to get IA64-specific information
// @@BEGIN_DDKSPLIT
































//
// Bit position in IP for low order bit of slot number
//































/*++








Routine Description:





Arguments:







Return Value:

    None



--*/
























typedef struct _DYNAMIC_FUNCTION_TABLE {
    LIST_ENTRY          Links;
    PRUNTIME_FUNCTION   FunctionTable;
    ULONG               EntryCount;
    LARGE_INTEGER       TimeStamp;
    ULONG_PTR           MinimumAddress;
    ULONG_PTR           MaximumAddress;
    BOOLEAN             Sorted;
    ULONGLONG           BaseAddress;
    ULONGLONG           TargetGp;
} DYNAMIC_FUNCTION_TABLE, *PDYNAMIC_FUNCTION_TABLE;

#define RF_BEGIN_ADDRESS(Base,RF)      (( (SIZE_T) Base + (RF)->BeginAddress) & (0xFFFFFFFFFFFFFFF0)) // Instruction Size 16 bytes
#define RF_END_ADDRESS(Base, RF)        (((SIZE_T) Base + (RF)->EndAddress+15) & (0xFFFFFFFFFFFFFFF0))   // Instruction Size 16 bytes

PLIST_ENTRY
RtlGetFunctionTableListHead (
    VOID
    );

BOOLEAN
RtlAddFunctionTable(
    IN PRUNTIME_FUNCTION FunctionTable,
    IN ULONG             EntryCount,
    IN ULONGLONG         BaseAddress,
    IN ULONGLONG         TargetGp
    );

BOOLEAN
RtlDeleteFunctionTable (
    IN PRUNTIME_FUNCTION FunctionTable
    );


















//
// Define C structured exception handing function prototypes.
//









struct _EXCEPTION_POINTERS;

















// @@END_DDKSPLIT

// begin_winnt

#ifdef _IA64_

// Please contact INTEL to get IA64-specific information
// @@BEGIN_DDKSPLIT






// @@END_DDKSPLIT

#endif // _IA64_

// end_winnt

// begin_ntddk begin_wdm begin_nthal
#endif // _IA64_
// end_ntddk end_wdm end_nthal

#ifdef __cplusplus
}
#endif

#endif // _NTIA64H_
