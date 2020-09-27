/*++ BUILD Version: 0014    // Increment this if a change has global effects

Copyright (c) 1989  Microsoft Corporation

Module Name:

    i386.h

Abstract:

    This module contains the i386 hardware specific header file.

Author:

    David N. Cutler (davec) 2-Aug-1989

Revision History:

    25-Jan-1990    shielint

                   Added definitions for 8259 ports and commands and
                   macros for 8259 irq# and system irql conversion.

--*/

#ifndef _i386_
#define _i386_

// begin_ntddk begin_wdm

#if defined(_X86_)

//
// Types to use to contain PFNs and their counts.
//

typedef ULONG PFN_COUNT;

typedef LONG SPFN_NUMBER, *PSPFN_NUMBER;
typedef ULONG PFN_NUMBER, *PPFN_NUMBER;

//
// Define maximum size of flush multiple TB request.
//

#define FLUSH_MULTIPLE_MAXIMUM 16

//
// Indicate that the i386 compiler supports the pragma textout construct.
//

#define ALLOC_PRAGMA 1

//
// Indicate that the i386 compiler supports the DATA_SEG("INIT") and
// DATA_SEG("PAGE") pragmas
//

#define ALLOC_DATA_PRAGMA 1

// end_ntddk end_wdm

//
// Interrupt controller register addresses.
//

#define PIC1_PORT0 0x20         // master PIC
#define PIC1_PORT1 0x21
#define PIC2_PORT0 0x0A0        // slave PIC
#define PIC2_PORT1 0x0A1

//
// Commands for Interrupt Controller
//

#define PIC1_EOI_MASK 0x60
#define PIC2_EOI      0x62
#define OCW2_NON_SPECIFIC_EOI 0x20
#define OCW3_READ_ISR 0xb
#define OCW3_READ_IRR 0xa

//
// Length on interrupt object dispatch code in longwords.
//

#define NORMAL_DISPATCH_LENGTH 22                   // ntddk wdm
#define DISPATCH_LENGTH NORMAL_DISPATCH_LENGTH      // ntddk wdm

//
// Define constants to access the bits in CR0.
//

#define CR0_PG  0x80000000          // paging
#define CR0_ET  0x00000010          // extension type (80387)
#define CR0_TS  0x00000008          // task switched
#define CR0_EM  0x00000004          // emulate math coprocessor
#define CR0_MP  0x00000002          // math present
#define CR0_PE  0x00000001          // protection enable

//
// More CR0 bits; these only apply to the 80486.
//

#define CR0_CD  0x40000000          // cache disable
#define CR0_NW  0x20000000          // not write-through
#define CR0_AM  0x00040000          // alignment mask
#define CR0_WP  0x00010000          // write protect
#define CR0_NE  0x00000020          // numeric error

//
// CR4 bits;  These only apply to Pentium
//

#define CR4_VME 0x00000001          // V86 mode extensions
#define CR4_PVI 0x00000002          // Protected mode virtual interrupts
#define CR4_TSD 0x00000004          // Time stamp disable
#define CR4_DE  0x00000008          // Debugging Extensions
#define CR4_PSE 0x00000010          // Page size extensions
#define CR4_PAE 0x00000020          // Physical address extensions
#define CR4_MCE 0x00000040          // Machine check enable
#define CR4_PGE 0x00000080          // Page global enable
#define CR4_FXSR 0x00000200         // FXSR used by OS
#define CR4_XMMEXCPT 0x00000400     // XMMI used by OS

//
// Define constants to access ThNpxState
//

#define NPX_STATE_NOT_LOADED    (CR0_TS | CR0_MP)
#define NPX_STATE_LOADED        0

//
// External references to the labels defined in int.asm
//

extern ULONG KiInterruptTemplate[NORMAL_DISPATCH_LENGTH];
extern PULONG KiInterruptTemplateObject;
extern PULONG KiInterruptTemplateDispatch;
extern PULONG KiInterruptTemplate2ndDispatch;

// begin_ntddk begin_wdm
//
// Interrupt Request Level definitions
//

#define PASSIVE_LEVEL 0             // Passive release level
#define LOW_LEVEL 0                 // Lowest interrupt level
#define APC_LEVEL 1                 // APC interrupt level
#define DISPATCH_LEVEL 2            // Dispatcher level

#define SMBUS_LEVEL 15              // System Management Bus interrupt level
#define PROFILE_LEVEL 26            // Profiler timer interrupt level
#define SCI_LEVEL 27                // System Control Interrupt (SCI) level
#define CLOCK_LEVEL 28              // Interval clock interrupt level
#define IPI_LEVEL 29                // Interprocessor interrupt level
#define POWER_LEVEL 30              // Power failure level
#define HIGH_LEVEL 31               // Highest interrupt level
#define SYNCH_LEVEL (IPI_LEVEL-1)   // Synchronization level
// end_ntddk end_wdm

//
// Define constants used in selector tests.
//
//  RPL_MASK is the real value for extracting RPL values.  IT IS THE WRONG
//  CONSTANT TO USE FOR MODE TESTING.
//
//  MODE_MASK is the value for deciding the current mode.
//  WARNING:    MODE_MASK assumes that all code runs at either ring-0
//              or ring-3.  Ring-1 or Ring-2 support will require changing
//              this value and all of the code that refers to it.

#define MODE_MASK    1
#define RPL_MASK     3

//
// SEGMENT_MASK is used to throw away trash part of segment.  Part always
// pushes or pops 32 bits to/from stack, but if it's a segment value,
// high order 16 bits are trash.
//

#define SEGMENT_MASK    0xffff

//
// Startup count value for KeStallExecution.  This value is used
// until KiInitializeStallExecution can compute the real one.
// Pick a value long enough for very fast processors.
//

#define INITIAL_STALL_COUNT 100

//
// Macro to extract the high word of a long offset
//

#define HIGHWORD(l) \
    ((USHORT)(((ULONG)(l)>>16) & 0xffff))

//
// Macro to extract the low word of a long offset
//

#define LOWWORD(l) \
    ((USHORT)((ULONG)l & 0x0000ffff))

//
// Macro to combine two USHORT offsets into a long offset
//

#if !defined(MAKEULONG)

#define MAKEULONG(x, y) \
    (((((ULONG)(x))<<16) & 0xffff0000) | \
    ((ULONG)(y) & 0xffff))

#endif

//
// Request a software interrupt.
//

#define KiRequestSoftwareInterrupt(RequestIrql) \
    HalRequestSoftwareInterrupt( RequestIrql )

// begin_ntddk begin_wdm

//
// I/O space read and write macros.
//
//  These have to be actual functions on the 386, because we need
//  to use assembler, but cannot return a value if we inline it.
//
//  The READ/WRITE_REGISTER_* calls manipulate I/O registers in MEMORY space.
//  (Use x86 move instructions, with LOCK prefix to force correct behavior
//   w.r.t. caches and write buffers.)
//
//  The READ/WRITE_PORT_* calls manipulate I/O registers in PORT space.
//  (Use x86 in/out instructions.)
//

NTHALAPI
UCHAR
READ_REGISTER_UCHAR(
    PUCHAR  Register
    );

NTHALAPI
USHORT
READ_REGISTER_USHORT(
    PUSHORT Register
    );

NTHALAPI
ULONG
READ_REGISTER_ULONG(
    PULONG  Register
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_REGISTER_UCHAR(
    PUCHAR  Register,
    UCHAR   Value
    );

NTHALAPI
VOID
WRITE_REGISTER_USHORT(
    PUSHORT Register,
    USHORT  Value
    );

NTHALAPI
VOID
WRITE_REGISTER_ULONG(
    PULONG  Register,
    ULONG   Value
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );

NTHALAPI
UCHAR
READ_PORT_UCHAR(
    PUCHAR  Port
    );

NTHALAPI
USHORT
READ_PORT_USHORT(
    PUSHORT Port
    );

NTHALAPI
ULONG
READ_PORT_ULONG(
    PULONG  Port
    );

NTHALAPI
VOID
READ_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_UCHAR(
    PUCHAR  Port,
    UCHAR   Value
    );

NTHALAPI
VOID
WRITE_PORT_USHORT(
    PUSHORT Port,
    USHORT  Value
    );

NTHALAPI
VOID
WRITE_PORT_ULONG(
    PULONG  Port,
    ULONG   Value
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

// end_ntddk end_wdm

//
// Define executive macros for acquiring and releasing executive spinlocks.
// These macros can ONLY be used by executive components and NOT by drivers.
// Drivers MUST use the kernel interfaces since they must be MP enabled on
// all systems.
//
// KeRaiseIrql is one instruction longer than KeAcquireSpinLock on x86 UP.
// KeLowerIrql and KeReleaseSpinLock are the same.
//

// begin_wdm begin_ntddk

#define ExAcquireSpinLock(Lock, OldIrql) KeAcquireSpinLock((Lock), (OldIrql))
#define ExReleaseSpinLock(Lock, OldIrql) KeReleaseSpinLock((Lock), (OldIrql))
#define ExAcquireSpinLockAtDpcLevel(Lock) KeAcquireSpinLockAtDpcLevel(Lock)
#define ExReleaseSpinLockFromDpcLevel(Lock) KeReleaseSpinLockFromDpcLevel(Lock)

// end_wdm end_ntddk

//
// The acquire and release fast lock macros disable and enable interrupts
// on UP nondebug systems. On MP or debug systems, the spinlock routines
// are used.
//
// N.B. Extreme caution should be observed when using these routines.
//

#if defined(_M_IX86) && !defined(USER_MODE_CODE)

#pragma warning(disable:4164)
#pragma intrinsic(_disable)
#pragma intrinsic(_enable)
#pragma warning(default:4164)

#endif

#if defined(NT_UP) && !DBG && !defined(USER_MODE_CODE)
#define ExAcquireFastLock(Lock, OldIrql) _disable()
#else
#define ExAcquireFastLock(Lock, OldIrql) \
    ExAcquireSpinLock(Lock, OldIrql)
#endif

#if defined(NT_UP) && !DBG && !defined(USER_MODE_CODE)
#define ExReleaseFastLock(Lock, OldIrql) _enable()
#else
#define ExReleaseFastLock(Lock, OldIrql) \
    ExReleaseSpinLock(Lock, OldIrql)
#endif

//
// The following function prototypes must be in this module so that the
// above macros can call them directly.
//

#if defined(NT_UP)

#define KiAcquireSpinLock(SpinLock)

#else

VOID
FASTCALL
KiAcquireSpinLock (
    IN PKSPIN_LOCK SpinLock
    );

#endif

#if defined(NT_UP)

#define KiReleaseSpinLock(SpinLock)

#else

VOID
FASTCALL
KiReleaseSpinLock (
    IN PKSPIN_LOCK SpinLock
    );

#endif

//
// Define query system time macro.
//

#define KiQuerySystemTime(CurrentTime) \
    while (TRUE) {                                                    \
        (CurrentTime)->HighPart = KeSystemTime.High1Time;             \
        (CurrentTime)->LowPart = KeSystemTime.LowPart;                \
        if ((CurrentTime)->HighPart == KeSystemTime.High2Time) break; \
        _asm { rep nop }                                              \
    }

//
// Define query interrupt time macro.
//

#define KiQueryInterruptTime(CurrentTime) \
    while (TRUE) {                                                       \
        (CurrentTime)->HighPart = KeInterruptTime.High1Time;             \
        (CurrentTime)->LowPart = KeInterruptTime.LowPart;                \
        if ((CurrentTime)->HighPart == KeInterruptTime.High2Time) break; \
        _asm { rep nop }                                                 \
    }

//
//
// 386 hardware structures
//

//
// A Page Table Entry on an Intel 386/486 has the following definition.
//

typedef struct _HARDWARE_PTE {
    ULONG Valid : 1;
    ULONG Write : 1;
    ULONG Owner : 1;
    ULONG WriteThrough : 1;
    ULONG CacheDisable : 1;
    ULONG Accessed : 1;
    ULONG Dirty : 1;
    ULONG LargePage : 1;
    ULONG Global : 1;
    ULONG GuardOrEndOfAllocation : 1;   // software field
    ULONG PersistAllocation : 1;        // software field
    ULONG reserved : 1;                 // software field
    ULONG PageFrameNumber : 20;
} HARDWARE_PTE, *PHARDWARE_PTE;

//
// GDT Entry
//

typedef struct _KGDTENTRY {
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
} KGDTENTRY, *PKGDTENTRY;

#define TYPE_CODE   0x10  // 11010 = Code, Readable, NOT Conforming, Accessed
#define TYPE_DATA   0x12  // 10010 = Data, ReadWrite, NOT Expanddown, Accessed
#define TYPE_TSS    0x01  // 01001 = NonBusy TSS
#define TYPE_LDT    0x02  // 00010 = LDT

#define DPL_USER    3
#define DPL_SYSTEM  0

#define GRAN_BYTE   0
#define GRAN_PAGE   1

#define SELECTOR_TABLE_INDEX 0x04

//
// Entry of Interrupt Descriptor Table (IDTENTRY)
//

typedef struct _KIDTENTRY {
   USHORT Offset;
   USHORT Selector;
   USHORT Access;
   USHORT ExtendedOffset;
} KIDTENTRY;

typedef KIDTENTRY *PKIDTENTRY;

extern KGDTENTRY KiGDT[];
extern KIDTENTRY KiIDT[];

//
// TSS (Task switch segment) NT only uses to control stack switches.
//
//  The only fields we actually care about are Esp0, Ss0, the IoMapBase
//  and the IoAccessMaps themselves.
//
//
//  N.B.    Size of TSS must be <= 0xDFFF
//

//
// The interrupt direction bitmap is used on Pentium to allow
// the processor to emulate V86 mode software interrupts for us.
// There is one for each IOPM.  It is located by subtracting
// 32 from the IOPM base in the Tss.
//
#define INT_DIRECTION_MAP_SIZE   32
typedef UCHAR   KINT_DIRECTION_MAP[INT_DIRECTION_MAP_SIZE];

#define IOPM_COUNT      1           // Number of i/o access maps that
                                    // exist (in addition to
                                    // IO_ACCESS_MAP_NONE)

#define IOPM_SIZE           8192    // Size of map callers can set.

#define PIOPM_SIZE          8196    // Size of structure we must allocate
                                    // to hold it.

typedef UCHAR   KIO_ACCESS_MAP[IOPM_SIZE];

typedef KIO_ACCESS_MAP *PKIO_ACCESS_MAP;

typedef struct _KiIoAccessMap {
    KINT_DIRECTION_MAP DirectionMap;
    UCHAR IoMap[PIOPM_SIZE];
} KIIO_ACCESS_MAP;

typedef struct _KTSS {

    USHORT  Backlink;
    USHORT  Reserved0;

    ULONG   Esp0;
    USHORT  Ss0;
    USHORT  Reserved1;

    ULONG   Esp1;
    USHORT  Ss1;
    USHORT  Reserved2;

    ULONG   Esp2;
    USHORT  Ss2;
    USHORT  Reserved3;

    ULONG   CR3;

    ULONG   Eip;
    ULONG   EFlags;
    ULONG   Eax;
    ULONG   Ecx;
    ULONG   Edx;
    ULONG   Ebx;
    ULONG   Esp;
    ULONG   Ebp;
    ULONG   Esi;
    ULONG   Edi;

    USHORT  Es;
    USHORT  Reserved4;

    USHORT  Cs;
    USHORT  Reserved5;

    USHORT  Ss;
    USHORT  Reserved6;

    USHORT  Ds;
    USHORT  Reserved7;

    USHORT  Fs;
    USHORT  Reserved8;

    USHORT  Gs;
    USHORT  Reserved9;

    USHORT  LDT;
    USHORT  Reserved10;

    USHORT  Flags;

    USHORT  IoMapBase;

    KIIO_ACCESS_MAP IoMaps[IOPM_COUNT];

    //
    // This is the Software interrupt direction bitmap associated with
    // IO_ACCESS_MAP_NONE
    //
    KINT_DIRECTION_MAP IntDirectionMap;
} KTSS, *PKTSS;

// begin_windbgkd

#ifdef _X86_
//
// Special Registers for i386
//

typedef struct _DESCRIPTOR {
    USHORT  Pad;
    USHORT  Limit;
    ULONG   Base;
} KDESCRIPTOR, *PKDESCRIPTOR;

typedef struct _KSPECIAL_REGISTERS {
    ULONG Cr0;
    ULONG Cr2;
    ULONG Cr3;
    ULONG Cr4;
    ULONG KernelDr0;
    ULONG KernelDr1;
    ULONG KernelDr2;
    ULONG KernelDr3;
    ULONG KernelDr6;
    ULONG KernelDr7;
    KDESCRIPTOR Gdtr;
    KDESCRIPTOR Idtr;
    USHORT Tr;
    USHORT Ldtr;
    ULONG Reserved[6];
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
// end_windbgkd

//
// Processor Control Block (PRCB)
//

typedef struct _KPRCB {

    struct _KTHREAD *CurrentThread;
    struct _KTHREAD *NextThread;
    struct _KTHREAD *IdleThread;
    struct _KTHREAD *NpxThread;

    ULONG   InterruptCount;             // per processor counts
    ULONG   DpcTime;
    ULONG   InterruptTime;
    ULONG   DebugDpcTime;               // per dpc tick count

//
// Kernel performance counters.
//

    ULONG KeContextSwitches;

//
// DPC interrupt requested.
//

    ULONG DpcInterruptRequested;

//
// DPC list head, spinlock, and count.
//

    LIST_ENTRY DpcListHead;
    ULONG DpcRoutineActive;
    PVOID DpcStack;

//
// QuantumEnd indicator
//

    ULONG QuantumEnd;

//
// Npx save area
//
    FX_SAVE_AREA    NpxSaveArea;

//
// network stack handshaking for debug monitor
//
    VOID* DmEnetFunc;

//
// devkit specific data
//
// debugger global data
    VOID* DebugMonitorData;

#ifdef DEVKIT
// function for debug synchronization
    VOID* DebugHaltThread;

// double-fault handler
    VOID* DebugDoubleFault;
#endif // DEVKIT

} KPRCB, *PKPRCB, *RESTRICTED_POINTER PRKPRCB;

// begin_ntddk

//
// Processor Control Region Structure Definition
//

typedef struct _KPCR {

    NT_TIB  NtTib;
    struct _KPCR *SelfPcr;              // flat address of this PCR
    struct _KPRCB *Prcb;                // pointer to Prcb
    KIRQL   Irql;

// end_ntddk

    struct _KPRCB PrcbData;

// begin_ntddk
} KPCR;
typedef KPCR *PKPCR;

// end_ntddk

//
// If kernel mode, then
//      let caller specify Carry, Parity, AuxCarry, Zero, Sign, Trap,
//      Direction, Overflow, Interrupt, AlignCheck.
//
// If user mode, then
//      let caller specify Carry, Parity, AuxCarry, Zero, Sign, Trap,
//      Direction, Overflow, AlignCheck.
//      force Interrupts on.
//

#define EFLAGS_DF_MASK        0x00000400L
#define EFLAGS_INTERRUPT_MASK 0x00000200L
#define EFLAGS_V86_MASK       0x00020000L
#define EFLAGS_ALIGN_CHECK    0x00040000L
#define EFLAGS_IOPL_MASK      0x00003000L
#define EFLAGS_VIF            0x00080000L
#define EFLAGS_VIP            0x00100000L
#define EFLAGS_USER_SANITIZE  0x003e0dd7L

#define SANITIZE_FLAGS(eFlags) ((eFlags) & 0x003e0fd7)

//
// Masks for Dr7 and sanitize macros for various Dr registers.
//

#define DR6_LEGAL   0x0000e00f

#define DR7_LEGAL   0xffff0155  // R/W, LEN for Dr0-Dr4,
                                // Local enable for Dr0-Dr4,
                                // Le for "perfect" trapping

#define DR7_ACTIVE  0x00000055  // If any of these bits are set, a Dr is active

#define SANITIZE_DR6(Dr6) (Dr6 & DR6_LEGAL)

#define SANITIZE_DR7(Dr7) (Dr7 & DR7_LEGAL)

//
// Define macro to clear reserved bits from MXCSR so that we don't
// GP fault when doing an FRSTOR
//
#define SANITIZE_MXCSR(_mxcsr_) ((_mxcsr_) & 0xFFBF)

//
//
// Trap frame
//
//  NOTE - We deal only with 32bit registers, so the assembler equivalents
//         are always the extended forms.
//
//  NOTE - Unless you want to run like slow molasses everywhere in the
//         the system, this structure must be of DWORD length, DWORD
//         aligned, and its elements must all be DWORD aligned.
//
//  NOTE WELL   -
//
//      The i386 does not build stack frames in a consistent format, the
//      frames vary depending on whether or not a privilege transition
//      was involved.
//
//      In order to make NtContinue work for both user mode and kernel
//      mode callers, we must force a canonical stack.
//
//      If we're called from kernel mode, this structure is 8 bytes longer
//      than the actual frame!
//
//  WARNING:
//
//      KTRAP_FRAME_LENGTH needs to be 16byte integral (at present.)
//

typedef struct _KTRAP_FRAME {

//
//  Following 4 values are only used and defined for DBG systems,
//  but are always allocated to make switching from DBG to non-DBG
//  and back quicker.  They are not DEVL because they have a non-0
//  performance impact.
//

    ULONG   DbgEbp;         // Copy of User EBP set up so KB will work.
    ULONG   DbgEip;         // EIP of caller to system call, again, for KB.
    ULONG   DbgArgMark;     // Marker to show no args here.
    ULONG   DbgArgPointer;  // Pointer to the actual args

//
//  Temporary values used when frames are edited.
//
//
//  NOTE:   Any code that want's ESP must materialize it, since it
//          is not stored in the frame for kernel mode callers.
//
//          And code that sets ESP in a KERNEL mode frame, must put
//          the new value in TempEsp, make sure that TempSegCs holds
//          the real SegCs value, and put a special marker value into SegCs.
//

    ULONG   TempSegCs;
    ULONG   TempEsp;

//
//  Volatile registers
//

    ULONG   Edx;
    ULONG   Ecx;
    ULONG   Eax;

//
//  Nesting state, not part of context record
//

    PEXCEPTION_REGISTRATION_RECORD ExceptionList;
                                            // Trash if caller was user mode.
                                            // Saved exception list if caller
                                            // was kernel mode or we're in
                                            // an interrupt.

//
//  Non-volatile registers
//

    ULONG   Edi;
    ULONG   Esi;
    ULONG   Ebx;
    ULONG   Ebp;

//
//  Control registers
//

    ULONG   ErrCode;
    ULONG   Eip;
    ULONG   SegCs;
    ULONG   EFlags;

    ULONG   HardwareEsp;    // WARNING - segSS:esp are only here for stacks
    ULONG   HardwareSegSs;  // that involve a ring transition.
} KTRAP_FRAME;


typedef KTRAP_FRAME *PKTRAP_FRAME;
typedef KTRAP_FRAME *PKEXCEPTION_FRAME;

#define KTRAP_FRAME_LENGTH  (sizeof(KTRAP_FRAME))
#define KTRAP_FRAME_ALIGN   (sizeof(ULONG))
#define KTRAP_FRAME_ROUND   (KTRAP_FRAME_ALIGN-1)

//
//  Bits forced to 0 in SegCs if Esp has been edited.
//

#define FRAME_EDITED        0xfff8

//
//  Switch Frame
//
//  386 doesn't have an "exception frame", and doesn't normally make
//  any use of nonvolatile context register structures.
//
//  However, swapcontext in ctxswap.c and KeInitializeThread in
//  thredini.c need to share common stack structure used at thread
//  startup and switch time.
//
//  This is that structure.
//

typedef struct _KSWITCHFRAME {
    ULONG   ExceptionList;
    ULONG   Eflags;
    ULONG   RetAddr;
} KSWITCHFRAME, *PKSWITCHFRAME;

//
// Various 387 defines
//

#define I386_80387_NP_VECTOR    0x07    // trap 7 when hardware not present

// begin_ntddk begin_wdm
//
// The non-volatile 387 state
//

typedef struct _KFLOATING_SAVE {
    ULONG   ControlWord;
    ULONG   StatusWord;
    ULONG   ErrorOffset;
    ULONG   ErrorSelector;
    ULONG   DataOffset;                 // Not used in wdm
    ULONG   DataSelector;
    ULONG   Cr0NpxState;
    ULONG   Spare1;                     // Not used in wdm
} KFLOATING_SAVE, *PKFLOATING_SAVE;

// end_ntddk end_wdm

//
// i386 Profile values
//

#define DEFAULT_PROFILE_INTERVAL    39063

//
// The minimum acceptable profiling interval is set to 1221 which is the
// fast RTC clock rate we can get.  If this
// value is too small, the system will run very slowly.
//

#define MINIMUM_PROFILE_INTERVAL    1221

//
// Define the value to program the 8254 timer with in order to fire an interrupt
// approximately every one millisecond.
//
// This is assuming a clock rate into the 8254 of 13.5 divided by 12.
//

#define CLOCK_ROLLOVER_COUNT        1125

//
// Define the number of 100ns units to add to the interrupt, system, and other
// timers on every clock interrupt.
//
// This is assuming a clock rate into the 8254 of 13.5 divided by 12.
//

#define CLOCK_TIME_INCREMENT        10000

// begin_ntddk begin_wdm
//
// i386 Specific portions of mm component
//

//
// Define the page size for the Intel 386 as 4096 (0x1000).
//

#define PAGE_SIZE 0x1000
#define PAGE_SIZE_LARGE 0x400000

//
// Define the number of trailing zeroes in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 12L
#define PAGE_SHIFT_LARGE 22L

// end_wdm

//
// Define the highest user address and user probe address.
//

// end_ntddk

#define MM_HIGHEST_USER_ADDRESS     (PVOID)0x7FFEFFFF
#define MM_SYSTEM_RANGE_START       (PVOID)KSEG0_BASE
#define MM_USER_PROBE_ADDRESS       (KSEG0_BASE - 0x10000)

// begin_ntddk
//
// The lowest user address reserves the low 64k.
//

#define MM_LOWEST_USER_ADDRESS (PVOID)0x10000

// end_ntddk

//
// Define the number of bits to shift to right justify the Page Directory Index
// field of a PTE.
//

#define PDI_SHIFT                   22

//
// Define the number of bits to shift to right justify the Page Table Index
// field of a PTE.
//

#define PTI_SHIFT                   12

//
// Define page directory and page base addresses.
//

#define PDE_BASE                    0xc0300000
#define PTE_BASE                    0xc0000000

//
// Define virtual base and alternate virtual base of kernel.
//

#define KSEG0_BASE                  0x80000000

//
// Define virtual base addresses for physical memory windows.
//

#define MM_BYTES_IN_PHYSICAL_MAP    (256 * 1024 * 1024)
#define MM_PAGES_IN_PHYSICAL_MAP    (MM_BYTES_IN_PHYSICAL_MAP >> PAGE_SHIFT)
#define MM_SYSTEM_PHYSICAL_MAP      KSEG0_BASE

// begin_ntddk

//
// Result type definition for i386.  (Machine specific enumerate type
// which is return type for portable exinterlockedincrement/decrement
// procedures.)  In general, you should use the enumerated type defined
// in ex.h instead of directly referencing these constants.
//

// Flags loaded into AH by LAHF instruction

#define EFLAG_SIGN      0x8000
#define EFLAG_ZERO      0x4000
#define EFLAG_SELECT    (EFLAG_SIGN | EFLAG_ZERO)

#define RESULT_NEGATIVE ((EFLAG_SIGN & ~EFLAG_ZERO) & EFLAG_SELECT)
#define RESULT_ZERO     ((~EFLAG_SIGN & EFLAG_ZERO) & EFLAG_SELECT)
#define RESULT_POSITIVE ((~EFLAG_SIGN & ~EFLAG_ZERO) & EFLAG_SELECT)

// begin_wdm

#define ExInterlockedInsertHeadList     ExfInterlockedInsertHeadList
#define ExInterlockedInsertTailList     ExfInterlockedInsertTailList
#define ExInterlockedRemoveHeadList     ExfInterlockedRemoveHeadList

// end_wdm

//
// Prototypes for architectural specific versions of Exi386 Api
//

NTKERNELAPI
LARGE_INTEGER
ExInterlockedExchangeAddLargeInteger (
    IN PLARGE_INTEGER Addend,
    IN LARGE_INTEGER Increment
    );

//
// Intrinsic interlocked functions
//

#if (defined(_NTDDK_) || defined(_NTIFS_) || defined(_NTHAL_) || defined(NO_INTERLOCKED_INTRINSICS)) && !defined(_WINBASE_)

// begin_wdm

NTKERNELAPI
LONG
FASTCALL
InterlockedIncrement(
    IN PLONG Addend
    );

NTKERNELAPI
LONG
FASTCALL
InterlockedDecrement(
    IN PLONG Addend
    );

NTKERNELAPI
LONG
FASTCALL
InterlockedExchange(
    IN OUT PLONG Target,
    IN LONG Value
    );

#define InterlockedExchangePointer(Target, Value) \
   (PVOID)InterlockedExchange((PLONG)(Target), (LONG)(Value))

NTKERNELAPI
LONG
FASTCALL
InterlockedExchangeAdd(
    IN OUT PLONG Addend,
    IN LONG Increment
    );

NTKERNELAPI
LONG
FASTCALL
InterlockedCompareExchange(
    IN OUT PLONG Destination,
    IN LONG ExChange,
    IN LONG Comperand
    );

#define InterlockedCompareExchangePointer(Destination, ExChange, Comperand) \
    (PVOID)InterlockedCompareExchange((PLONG)Destination, (LONG)ExChange, (LONG)Comperand)

// end_wdm

#endif

// end_ntddk

//
// UP/MP versions of interlocked intrinsics for use within ntoskrnl.exe.
//
// N.B. FASTCALL does NOT work with inline functions.
//

#if !defined(_NTDDK_) && !defined(_NTIFS_) && !defined(_NTHAL_) && !defined(_WINBASE_)
#if defined(_M_IX86) && !defined(_CROSS_PLATFORM_)

#pragma warning(disable:4035)               // wdm re-enable below

#if !defined(MIDL_PASS) // wdm

#if !defined(NO_INTERLOCKED_INTRINSICS)
#if defined(NT_UP)

FORCEINLINE
LONG
FASTCALL
InterlockedIncrement(
    IN PLONG Addend
    )
{
    __asm {
        mov     eax, 1
        mov     ecx, Addend
        xadd    [ecx], eax
        inc     eax
    }
}

FORCEINLINE
LONG
FASTCALL
InterlockedDecrement(
    IN PLONG Addend
    )
{
    __asm {
        mov     eax, -1
        mov     ecx, Addend
        xadd    [ecx], eax
        dec     eax
    }
}

FORCEINLINE
LONG
FASTCALL
InterlockedExchange(
    IN OUT PLONG Target,
    IN LONG Value
    )
{
    __asm {
        mov     edx, Value
        mov     ecx, Target
        mov     eax, [ecx]
ie:     cmpxchg [ecx], edx
        jnz     short ie
    }
}

#define InterlockedExchangePointer(Target, Value) \
   (PVOID)InterlockedExchange((PLONG)Target, (LONG)Value)

FORCEINLINE
LONG
FASTCALL
InterlockedExchangeAdd(
    IN OUT PLONG Addend,
    IN LONG Increment
    )
{
    __asm {
        mov     eax, Increment
        mov     ecx, Addend
        xadd    [ecx], eax
    }
}

FORCEINLINE
LONG
FASTCALL
InterlockedCompareExchange(
    IN OUT PLONG Destination,
    IN LONG Exchange,
    IN LONG Comperand
    )
{
    __asm {
        mov     eax, Comperand
        mov     ecx, Destination
        mov     edx, Exchange
        cmpxchg [ecx], edx
    }
}

#define InterlockedCompareExchangePointer(Destination, ExChange, Comperand) \
    (PVOID)InterlockedCompareExchange((PLONG)Destination, (LONG)ExChange, (LONG)Comperand)

#else

FORCEINLINE
LONG
FASTCALL
InterlockedIncrement(
    IN PLONG Addend
    )
{
    __asm {
        mov     eax, 1
        mov     ecx, Addend
   lock xadd    [ecx], eax
        inc     eax
    }
}

FORCEINLINE
LONG
FASTCALL
InterlockedDecrement(
    IN PLONG Addend
    )
{
    __asm {
        mov     eax, -1
        mov     ecx, Addend
   lock xadd    [ecx], eax
        dec     eax
    }
}

FORCEINLINE
LONG
FASTCALL
InterlockedExchange(
    IN OUT PLONG Target,
    IN LONG Value
    )
{
    __asm {
        mov     eax, Value
        mov     ecx, Target
        xchg    [ecx], eax
    }
}

#define InterlockedExchangePointer(Target, Value) \
   (PVOID)InterlockedExchange((PLONG)Target, (LONG)Value)

// begin_wdm

FORCEINLINE
LONG
FASTCALL
InterlockedExchangeAdd(
    IN OUT PLONG Addend,
    IN LONG Increment
    )
{
    __asm {
        mov     eax, Increment
        mov     ecx, Addend
   lock xadd    [ecx], eax
    }
}


// end_wdm

FORCEINLINE
LONG
FASTCALL
InterlockedCompareExchange(
    IN OUT PLONG Destination,
    IN LONG Exchange,
    IN LONG Comperand
    )
{
    __asm {
        mov     eax, Comperand
        mov     ecx, Destination
        mov     edx, Exchange
   lock cmpxchg [ecx], edx
    }
}

#define InterlockedCompareExchangePointer(Destination, ExChange, Comperand) \
    (PVOID)InterlockedCompareExchange((PLONG)Destination, (LONG)ExChange, (LONG)Comperand)

#endif      // wdm
#endif
#endif

#pragma warning(default:4035)   // wdm

#endif
#endif

// begin_ntddk begin_wdm

#if !defined(MIDL_PASS) && defined(_M_IX86)

//
// i386 function definitions
//

#pragma warning(disable:4035)               // re-enable below

// end_ntddk end_wdm

//
// Get address of current processor block.
//
// WARNING: This inline macro can only be used by the kernel or hal
//
FORCEINLINE PKPCR KeGetPcr(VOID)
{
#if defined(_NTSYSTEM_)
    extern KPCR KiPCR;
    return (PKPCR)&KiPCR;
#else
    __asm {  mov eax, fs:[0] KPCR.SelfPcr }
#endif
}

//
// Get address of current processor block.
//
// WARNING: This inline macro can only be used by the kernel or hal
//
FORCEINLINE PKPRCB KeGetCurrentPrcb (VOID)
{
#if defined(_NTSYSTEM_)
    return &KeGetPcr()->PrcbData;
#else
    __asm {  mov eax, fs:[0] KPCR.Prcb }
#endif
}

// begin_ntddk begin_wdm

//
// Get current IRQL.
//
// On x86 this function resides in the HAL
//

FORCEINLINE KIRQL KeGetCurrentIrql(VOID)
{
#if defined(_NTSYSTEM_)
    return KeGetPcr()->Irql;
#else
    __asm {  movzx eax, byte ptr fs:[0] KPCR.Irql }
#endif
}    


// end_wdm end_ntddk
//
// Get address of current kernel thread object.
//

FORCEINLINE struct _KTHREAD *KeGetCurrentThread(VOID)
{
#if defined(_NTSYSTEM_)
    return KeGetCurrentPrcb()->CurrentThread;
#else
    __asm {  mov eax, fs:[0] KPCR.PrcbData.CurrentThread }
#endif
}

//
// If processor executing DPC?
//

FORCEINLINE ULONG KeIsExecutingDpc(VOID)
{
#if defined(_NTSYSTEM_)
    return KeGetCurrentPrcb()->DpcRoutineActive;
#else
    __asm {  mov eax, fs:[0] KPCR.PrcbData.DpcRoutineActive }
#endif
}

FORCEINLINE PVOID KeGetCurrentTlsDataTop(VOID)
{
#if defined(_NTSYSTEM_)
    return KeGetPcr()->NtTib.StackBase;
#else
    __asm {  mov eax, fs:[0] NT_TIB.StackBase }
#endif
}

// begin_ntddk begin_wdm

#endif // !defined(MIDL_PASS) && defined(_M_IX86)

// end_ntddk end_wdm

//
//
// Macro to set address of a trap/interrupt handler to IDT
//
#define KiSetHandlerAddressToIDT(Vector, HandlerAddress) \
    KiIDT[Vector].ExtendedOffset = HIGHWORD(HandlerAddress); \
    KiIDT[Vector].Offset = LOWWORD(HandlerAddress);

//
// Macro to return address of a trap/interrupt handler in IDT
//
#define KiReturnHandlerAddressFromIDT(Vector) \
   MAKEULONG(KiIDT[Vector].ExtendedOffset, KiIDT[Vector].Offset)

#pragma warning(default:4035)

//++
//
// BOOLEAN
// KiIsThreadNumericStateSaved(
//     IN PKTHREAD Address
//     )
//
//--
#define KiIsThreadNumericStateSaved(a) \
    (a->NpxState != NPX_STATE_LOADED)

//++
//
// VOID
// KiRundownThread(
//     IN PKTHREAD Address
//     )
//
//--

#if defined(NT_UP)

//
// On UP x86 systems, FP state is lazy saved and loaded.  If this
// thread owns the current FP context, clear the ownership field
// so we will not try to save to this thread after it has been
// terminated.
//

#define KiRundownThread(a)                          \
    if (KeGetCurrentPrcb()->NpxThread == (a))   {   \
        KeGetCurrentPrcb()->NpxThread = NULL;       \
    }

#else

#define KiRundownThread(a)

#endif

//
// Procedures to support frame manipulation
//

ULONG
KiEspFromTrapFrame(
    IN PKTRAP_FRAME TrapFrame
    );

VOID
KiEspToTrapFrame(
    IN PKTRAP_FRAME TrapFrame,
    IN ULONG Esp
    );

ULONG
KiSegSsFromTrapFrame(
    IN PKTRAP_FRAME TrapFrame
    );

VOID
KiSegSsToTrapFrame(
    IN PKTRAP_FRAME TrapFrame,
    IN ULONG SegSs
    );

// begin_ntddk begin_wdm

NTKERNELAPI
NTSTATUS
NTAPI
KeSaveFloatingPointState (
    OUT PKFLOATING_SAVE     FloatSave
    );

NTKERNELAPI
NTSTATUS
NTAPI
KeRestoreFloatingPointState (
    IN PKFLOATING_SAVE      FloatSave
    );

// end_ntddk end_wdm

VOID
KeClockInterrupt(
    VOID
    );

// begin_ntddk begin_wdm

#endif // defined(_X86_)

// end_ntddk end_wdm

//
// x86 functions for special instructions
//

ULONGLONG
FASTCALL
RDMSR (
    IN ULONG MsrRegister
    );

VOID
WRMSR (
    IN ULONG MsrRegister,
    IN ULONGLONG MsrValue
    );

//
// Define the address of the page directory page.
//

#define PAGE_DIRECTORY_PHYSICAL_ADDRESS ((ULONG_PTR)0x0000F000)

//
// Define the address in RAM where a shadow copy of the ROM is stored.
//

#define ROM_SHADOW_PHYSICAL_ADDRESS ((ULONG_PTR)0x7F00000)
#define ROM_SHADOW_SIZE ((ULONG_PTR)1024*1024)

//
// Microcode update data block.
//

#include <pshpack1.h>

typedef struct _MICROCODE_UPDATE_DATA_BLOCK {
    ULONG HeaderVersion;
    ULONG UpdateRevision;
    USHORT Year;
    UCHAR Day;
    UCHAR Month;
    ULONG Processor;
    ULONG Checksum;
    ULONG LoaderRevision;
    ULONG ProcessorFlags;
    UCHAR Reserved[20];
    UCHAR UpdateData[2000];
} MICROCODE_UPDATE_DATA_BLOCK, *PMICROCODE_UPDATE_DATA_BLOCK;

//
// The kernel tick counter is precisely one millisecond, so the result of
// NtGetTickCount, which must be in units of one millisecond, is equivalent to
// KeQueryTickCount.
//

#define NtGetTickCount() KeQueryTickCount()

#include <poppack.h>

#endif // _i386_

