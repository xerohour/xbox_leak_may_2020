/*-- BUILD Version: 0005    // Increment this if a change has global effects

Copyright (c) 1989  Microsoft Corporation

Module Name:

    mm.h

Abstract:

    This module contains the public data structures and procedure
    prototypes for the memory management system.

Author:

    Lou Perazzoli (loup) 20-Mar-1989

Revision History:

--*/

#ifndef _MM_
#define _MM_

//
// The allocation granularity is 64k.
//

#define MM_ALLOCATION_GRANULARITY ((ULONG)0x10000)

// begin_ntddk begin_wdm begin_nthal begin_ntifs

//++
//
// ULONG_PTR
// ROUND_TO_PAGES(
//     IN ULONG_PTR Size
//     )
//
// Routine Description:
//
//     The ROUND_TO_PAGES macro takes a size in bytes and rounds it up to a
//     multiple of the page size.
//
//     NOTE: This macro fails for values 0xFFFFFFFF - (PAGE_SIZE - 1).
//
// Arguments:
//
//     Size - Size in bytes to round up to a page multiple.
//
// Return Value:
//
//     Returns the size rounded up to a multiple of the page size.
//
//--

#define ROUND_TO_PAGES(Size)  (((ULONG_PTR)(Size) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

//++
//
// ULONG
// BYTES_TO_PAGES(
//     IN ULONG Size
//     )
//
// Routine Description:
//
//     The BYTES_TO_PAGES macro takes the size in bytes and calculates the
//     number of pages required to contain the bytes.
//
// Arguments:
//
//     Size - Size in bytes.
//
// Return Value:
//
//     Returns the number of pages required to contain the specified size.
//
//--

#define BYTES_TO_PAGES(Size)  ((ULONG)((ULONG_PTR)(Size) >> PAGE_SHIFT) + \
                               (((ULONG)(Size) & (PAGE_SIZE - 1)) != 0))

//++
//
// ULONG
// BYTE_OFFSET(
//     IN PVOID Va
//     )
//
// Routine Description:
//
//     The BYTE_OFFSET macro takes a virtual address and returns the byte offset
//     of that address within the page.
//
// Arguments:
//
//     Va - Virtual address.
//
// Return Value:
//
//     Returns the byte offset portion of the virtual address.
//
//--

#define BYTE_OFFSET(Va) ((ULONG)((LONG_PTR)(Va) & (PAGE_SIZE - 1)))

//++
//
// ULONG
// BYTE_OFFSET_LARGE(
//     IN PVOID Va
//     )
//
// Routine Description:
//
//     The BYTE_OFFSET macro takes a virtual address and returns the byte offset
//     of that address within the large page.
//
// Arguments:
//
//     Va - Virtual address.
//
// Return Value:
//
//     Returns the byte offset portion of the virtual address.
//
//--

#define BYTE_OFFSET_LARGE(Va) ((ULONG)((LONG_PTR)(Va) & (PAGE_SIZE_LARGE - 1)))

//++
//
// PVOID
// PAGE_ALIGN(
//     IN PVOID Va
//     )
//
// Routine Description:
//
//     The PAGE_ALIGN macro takes a virtual address and returns a page-aligned
//     virtual address for that page.
//
// Arguments:
//
//     Va - Virtual address.
//
// Return Value:
//
//     Returns the page aligned virtual address.
//
//--

#define PAGE_ALIGN(Va) ((PVOID)((ULONG_PTR)(Va) & ~(PAGE_SIZE - 1)))

//++
//
// PVOID
// PAGE_ALIGN_LARGE(
//     IN PVOID Va
//     )
//
// Routine Description:
//
//     The PAGE_ALIGN macro takes a virtual address and returns a page-aligned
//     virtual address for that page.
//
// Arguments:
//
//     Va - Virtual address.
//
// Return Value:
//
//     Returns the page aligned virtual address.
//
//--

#define PAGE_ALIGN_LARGE(Va) ((PVOID)((ULONG_PTR)(Va) & ~(PAGE_SIZE_LARGE - 1)))

//++
//
// ULONG
// ADDRESS_AND_SIZE_TO_SPAN_PAGES(
//     IN PVOID Va,
//     IN ULONG Size
//     )
//
// Routine Description:
//
//     The ADDRESS_AND_SIZE_TO_SPAN_PAGES macro takes a virtual address and
//     size and returns the number of pages spanned by the size.
//
// Arguments:
//
//     Va - Virtual address.
//
//     Size - Size in bytes.
//
// Return Value:
//
//     Returns the number of pages spanned by the size.
//
//--

#define ADDRESS_AND_SIZE_TO_SPAN_PAGES(Va,Size) \
   (((((Size) - 1) >> PAGE_SHIFT) + \
   (((((ULONG)(Size-1)&(PAGE_SIZE-1)) + (PtrToUlong(Va) & (PAGE_SIZE -1)))) >> PAGE_SHIFT)) + 1L)

#define COMPUTE_PAGES_SPANNED(Va, Size) \
    ((ULONG)((((ULONG_PTR)(Va) & (PAGE_SIZE -1)) + (Size) + (PAGE_SIZE - 1)) >> PAGE_SHIFT))

// end_ntddk end_wdm end_nthal end_ntifs

//++
//
// BOOLEAN
// IS_SYSTEM_ADDRESS
//     IN PVOID Va,
//     )
//
// Routine Description:
//
//     This macro takes a virtual address and returns TRUE if the virtual address
//     is within system space, FALSE otherwise.
//
// Arguments:
//
//     Va - Virtual address.
//
// Return Value:
//
//     Returns TRUE is the address is in system space.
//
//--

#define IS_SYSTEM_ADDRESS(VA) ((VA) >= MM_SYSTEM_RANGE_START)

//
// Number of physical pages.
//

extern PFN_COUNT MmNumberOfPhysicalPages;

//
// Page frame number of the lowest physically addressable RAM page.
//

extern PFN_NUMBER MmLowestPhysicalPage;

//
// Page frame number of the highest physically addressable RAM page.
//

extern PFN_NUMBER MmHighestPhysicalPage;

//
// Memory management initialization routine.
//

VOID
MmInitSystem(
    VOID
    );

VOID
MmDiscardInitSection(
    VOID
    );

VOID
MmRelocatePersistentMemory(
    VOID
    );

VOID
MmPrepareToQuickRebootSystem(
    VOID
    );

#ifdef DEVKIT

VOID
MmReleaseDeveloperKitMemory(
    VOID
    );

#endif

//
// Shutdown routine - flushes dirty pages, etc for system shutdown.
//

BOOLEAN
MmShutdownSystem(
    VOID
    );

//
// Pool support routines to allocate complete pages, not for
// general consumption, these are only used by the executive pool allocator.
//

PVOID
MmAllocatePoolPages(
    IN SIZE_T NumberOfBytes
    );

ULONG
MmFreePoolPages(
    IN PVOID StartingAddress
    );

//
// First level fault routine.
//

NTSTATUS
MmAccessFault(
    IN BOOLEAN StoreInstruction,
    IN PVOID VirtualAddress,
    IN PVOID TrapInformation
    );

//
// Process Support Routines.
//

NTKERNELAPI
PVOID
MmCreateKernelStack(
    IN SIZE_T NumberOfBytes,
    IN BOOLEAN DebuggerThread
    );

NTKERNELAPI
VOID
MmDeleteKernelStack(
    IN PVOID KernelStackBase,
    IN PVOID KernelStackLimit
    );

//
// Debugger support routines.
//

#define MmDbgReadCheck(VirtualAddress) (MmIsAddressValid(VirtualAddress) ? VirtualAddress : NULL)

NTKERNELAPI
PVOID
MmDbgWriteCheck(
    IN PVOID VirtualAddress,
    IN PHARDWARE_PTE Opaque
    );

NTKERNELAPI
VOID
MmDbgReleaseAddress(
    IN PVOID VirtualAddress,
    IN PHARDWARE_PTE Opaque
    );

NTKERNELAPI
PVOID64
MmDbgTranslatePhysicalAddress64(
    IN PHYSICAL_ADDRESS PhysicalAddress
    );

NTKERNELAPI
PVOID
MmDbgAllocateMemory(
    IN SIZE_T NumberOfBytes,
    IN ULONG Protect
    );

NTKERNELAPI
ULONG
MmDbgFreeMemory(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes
    );

NTKERNELAPI
PFN_COUNT
MmDbgQueryAvailablePages(
    VOID
    );

VOID
MmDbgAllocateDebugMonitorBase(
    PFN_COUNT NumberOfPages
    );

// begin_ntddk begin_ntifs begin_nthal

NTKERNELAPI
VOID
MmLockUnlockBufferPages(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes,
    IN BOOLEAN UnlockPages
    );

NTKERNELAPI
VOID
MmLockUnlockPhysicalPage(
    IN ULONG_PTR PhysicalAddress,
    IN BOOLEAN UnlockPage
    );

// begin_wdm

NTKERNELAPI
PVOID
MmMapIoSpace(
    IN ULONG_PTR PhysicalAddress,
    IN SIZE_T NumberOfBytes,
    IN ULONG Protect
    );

NTKERNELAPI
VOID
MmUnmapIoSpace(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes
    );

// end_wdm end_ntddk end_ntifs

NTSTATUS
MmLockSelectedIoPages(
    IN PFILE_SEGMENT_ELEMENT SegmentArray,
    IN ULONG Length,
    IN OUT PIRP Irp,
    IN BOOLEAN MapUserBuffer
    );

VOID
MmUnlockSelectedIoPages(
    IN PIRP Irp
    );

// begin_ntddk begin_ntifs

NTKERNELAPI
ULONG_PTR
MmGetPhysicalAddress(
    IN PVOID BaseAddress
    );

NTKERNELAPI
PVOID
MmAllocateContiguousMemory(
    IN SIZE_T NumberOfBytes
    );

NTKERNELAPI
PVOID
MmAllocateContiguousMemoryEx(
    IN SIZE_T NumberOfBytes,
    IN ULONG_PTR LowestAcceptableAddress,
    IN ULONG_PTR HighestAcceptableAddress,
    IN ULONG_PTR Alignment,
    IN ULONG Protect
    );

NTKERNELAPI
VOID
MmFreeContiguousMemory(
    IN PVOID BaseAddress
    );

NTKERNELAPI
VOID
MmPersistContiguousMemory(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes,
    IN BOOLEAN Persist
    );

NTKERNELAPI
PVOID
MmAllocateSystemMemory(
    IN SIZE_T NumberOfBytes,
    IN ULONG Protect
    );

NTKERNELAPI
ULONG
MmFreeSystemMemory(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes
    );

NTKERNELAPI
SIZE_T
MmQueryAllocationSize(
    IN PVOID BaseAddress
    );

NTKERNELAPI
ULONG
MmQueryAddressProtect(
    IN PVOID VirtualAddress
    );

NTKERNELAPI
VOID
MmSetAddressProtect(
    IN PVOID BaseAddress,
    IN ULONG NumberOfBytes,
    IN ULONG NewProtect
    );

NTKERNELAPI
BOOLEAN
MmIsAddressValid(
    IN PVOID VirtualAddress
    );

// end_ntddk end_nthal end_ntifs

//
// Memory manager statistics.
//

typedef struct _MM_STATISTICS {
    ULONG Length;
    ULONG TotalPhysicalPages;
    ULONG AvailablePages;
    ULONG VirtualMemoryBytesCommitted;
    ULONG VirtualMemoryBytesReserved;
    ULONG CachePagesCommitted;
    ULONG PoolPagesCommitted;
    ULONG StackPagesCommitted;
    ULONG ImagePagesCommitted;
} MM_STATISTICS, *PMM_STATISTICS;

NTKERNELAPI
NTSTATUS
MmQueryStatistics(
    IN OUT PMM_STATISTICS MemoryStatistics
    );

NTKERNELAPI
PVOID
MmClaimGpuInstanceMemory(
    IN SIZE_T NumberOfBytes,
    OUT SIZE_T *NumberOfPaddingBytes
    );

#endif  // MM
