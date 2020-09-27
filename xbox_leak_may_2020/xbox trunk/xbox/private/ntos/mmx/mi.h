/*++

Copyright (c) 1989-2002  Microsoft Corporation

Module Name:

    mi.h

Abstract:

    This module contains the private data structures and procedure
    prototypes for the memory management system.

--*/

#ifndef _MI_
#define _MI_

#include <ntos.h>
#include <bldr.h>
#include <ki.h>
#include <pool.h>
#include <pci.h>
#include <av.h>
#include <xlaunch.h>

//
// Break DEVKIT functionality into CONSOLE versus ARCADE features with the
// following macro.
//

#if defined(DEVKIT) && !defined(ARCADE)
#define CONSOLE_DEVKIT
#endif

//
// DBG sensitive DbgPrint wrapper.
//

#if DBG
#define MiDbgPrint(x)               DbgPrint x
#else
#define MiDbgPrint(x)
#endif

//
// Bit flag macros.
//

#define MiIsFlagSet(flagset, flag)              (((flagset) & (flag)) != 0)
#define MiIsFlagClear(flagset, flag)            (((flagset) & (flag)) == 0)

//
// Returns the index of the supplied element relative to the supplied base
// address.
//

#define ARRAY_ELEMENT_NUMBER(base, type, element)                             \
    ((ULONG)(((type *)(element)) - (base)))

//
// Returns the number of elements in the supplied array.
//

#define ARRAY_ELEMENTS(array)                                                 \
    (sizeof((array)) / sizeof((array)[0]))

//
// Shortcuts to common multiplication factors.
//

#define X64K                        ((ULONG)64*1024)
#define X1024K                      ((ULONG)1024*1024)

//
// Define the NVIDIA NV2A constants for controlling the last accessible byte of
// memory.  These are accessed from the host bridge device.
//

#define CR_CPU_MEMTOP_LIMIT         0x87
#define CR_CPU_MEMTOP_LIMIT_64MB    0x03
#define CR_CPU_MEMTOP_LIMIT_128MB   0x07

//
// Define the lowest physical page available in the system.
//

#define MM_LOWEST_PHYSICAL_PAGE     0x00000

//
// Define the highest physical page available in the system.
//

#if defined(ARCADE)
#define MM_HIGHEST_PHYSICAL_PAGE    0x07FFF
#elif defined(DEVKIT)
#define MM_HIGHEST_PHYSICAL_PAGE    MmHighestPhysicalPage
#else
#define MM_HIGHEST_PHYSICAL_PAGE    0x03FFF
#endif

//
// Define the physical page that's reserved for use by D3D.  D3D needs the first
// ULONG in the system in order to initialize the push buffer, so we reserve the
// entire page for D3D's use.
//

#define MM_D3D_PHYSICAL_PAGE        0x00000

//
// Define the highest physical page that's allowed for a contiguous memory
// allocation.  All contiguous memory allocations are constrained to the memory
// available on a retail system.
//
// Additionally, the NVIDIA NV2A can use up to 128K of instance memory that must
// be allocated from the top of memory.  In order to leave the maximum amount of
// instance memory available from quick boot to quick boot, we don't allow
// anybody to allocate contiguous pages from the upper 128K (64K of which is
// already consumed by the PFN database).  This prevents somebody from
// persisting a contiguous allocation in this range across a quick reboot and
// screwing up the next title.
//
// For ARCADE, we require a 128K PFN database to describe the 128M retail
// system.  With 64K reserved for NVIDIA NV2A instance memory, this pushes the
// contiguous memory limit to 196K from the top of memory.
//

#ifdef ARCADE
#define MM_CONTIGUOUS_MEMORY_LIMIT  0x07FCF
#else
#define MM_CONTIGUOUS_MEMORY_LIMIT  0x03FDF
#endif

//
// Define the first physical page that's used to hold the PFN database.  The
// page number is selected such that the first 64M's half of the database sits
// at the end of the first 64M and the second 64M's half of the database sits
// at the start of the second 64M.
//
// For ARCADE, the PFN database is always 128K.  NVIDIA NV2A instance memory
// must be located in the top 128K of memory, but we only reserve 64K of
// instance memory.  The PFN database is instead located below the reserved
// instance memory.
//

#ifdef ARCADE
#define MM_DATABASE_PHYSICAL_PAGE   0x07FD0
#else
#define MM_DATABASE_PHYSICAL_PAGE   0x03FF0
#endif

//
// Define the bottom of the region that is reserved at system startup for NVIDIA
// NV2A instance memory.  These pages may be made available for other use when
// the D3D library calls MmClaimGpuInstanceMemory.
//
// For ARCADE, the PFN database is always 128K.  This would consume all of the
// usable pages for NVIDIA NV2A instance memory, so instead this instance memory
// is reserved at the top of memory and the PFN database is located below this
// instance memory.
//

#ifdef ARCADE
#define MM_INSTANCE_PHYSICAL_PAGE   0x07FF0
#else
#define MM_INSTANCE_PHYSICAL_PAGE   0x03FE0
#endif

#define MM_INSTANCE_PAGE_COUNT      16

//
// Define the first physical page that's available only on a development kit
// system.
//

#define MM_64M_PHYSICAL_PAGE        0x04000

//
// Define the write combine system memory aperture that's exposed by the NVIDIA
// NV2A.  PAGE_VIDEO allocations are mapped through this aperture instead of the
// standard system memory aperture starting at physical page zero.
//

#define MM_WRITE_COMBINE_APERTURE   0x40000

//
// Fixed addresses for memory manager data structures.
//

#define MM_PHYSICAL_MAP_BASE        ((ULONG)0x80000000)
#define MM_PHYSICAL_MAP_END         ((ULONG)0x8FFFFFFF)

#define MM_DEVKIT_PTE_BASE          ((ULONG)0xB0000000)
#define MM_DEVKIT_PTE_END           ((ULONG)0xBFFFFFFF)

#define MM_PAGE_TABLES_BASE         ((ULONG)0xC0000000)
#define MM_PAGE_TABLES_END          ((ULONG)0xC03FFFFF)

#define MM_DEBUG_VA                 ((ULONG)0xC0800000)

#define MM_SYSTEM_PTE_BASE          ((ULONG)0xD0000000)
#define MM_SYSTEM_PTE_END           ((ULONG)0xEFFFFFFF)

#define MM_DEVICE_WC_BASE           ((ULONG)0xF0000000)
#define MM_DEVICE_WC_END            ((ULONG)0xF7FFFFFF)
#define MM_DEVICE_UC_BASE           ((ULONG)0xF8000000)
#define MM_DEVICE_UC_END            ((ULONG)0xFFBFFFFF)

#define MM_HIGHEST_VAD_ADDRESS      ((PVOID)((ULONG_PTR)MM_HIGHEST_USER_ADDRESS - X64K))
#define MM_USER_ADDRESS_RANGE_LIMIT 0xFFFFFFFF
#define MM_MAXIMUM_ZERO_BITS        21

//
// Page color support.
//

#define MM_NUMBER_OF_COLORS_BITS    5
#define MM_NUMBER_OF_COLORS         (1 << MM_NUMBER_OF_COLORS_BITS)
#define MM_NUMBER_OF_COLORS_MASK    (MM_NUMBER_OF_COLORS - 1)

typedef ULONG MMCOLOR;

//
// Macros to lock and unlock the memory manager data structures.
//

#define MI_LOCK_MM(OldIrql)         (*OldIrql) = KeRaiseIrqlToDpcLevel()
#define MI_UNLOCK_MM(OldIrql)       KeLowerIrql(OldIrql)
#define MI_ASSERT_LOCK_MM()         ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL)

#define MI_UNLOCK_MM_AND_THEN_WAIT(OldIrql) {                                 \
    PKTHREAD Thread = KeGetCurrentThread();                                   \
    MI_ASSERT_LOCK_MM();                                                      \
    ASSERT((OldIrql) <= APC_LEVEL);                                           \
    Thread->WaitIrql = (OldIrql);                                             \
    Thread->WaitNext = TRUE;                                                  \
}

//
// Page table entry data structure as overloaded by the memory manager.
//

typedef struct _MMPTE {
    union {
        ULONG Long;
        HARDWARE_PTE Hard;
        struct {
            ULONG Valid : 1;
            ULONG OneEntry : 1;
            ULONG NextEntry : 30;
        } List;
    };
} MMPTE, *PMMPTE;

#define PTE_PER_PAGE                (PAGE_SIZE / sizeof(MMPTE))
#define PDE_PER_PAGE                (PAGE_SIZE / sizeof(MMPTE))

//
// Define masks for fields within the PTE.
//

#define MM_PTE_VALID_MASK           0x00000001
#define MM_PTE_WRITE_MASK           0x00000002
#define MM_PTE_OWNER_MASK           0x00000004
#define MM_PTE_WRITE_THROUGH_MASK   0x00000008
#define MM_PTE_CACHE_DISABLE_MASK   0x00000010
#define MM_PTE_ACCESS_MASK          0x00000020
#define MM_PTE_DIRTY_MASK           0x00000040
#define MM_PTE_LARGE_PAGE_MASK      0x00000080
#define MM_PTE_GLOBAL_MASK          0x00000100
#define MM_PTE_GUARD_MASK           0x00000200
#define MM_PTE_NEXT_ENTRY_MASK      0xFFFFFFFC

//
// Bit fields to or into PTE to make a PTE valid based on the protection field
// of the invalid PTE.
//
// Note that for guard pages, we overload the supervisor/user PTE flag to mean
// that the page is a guard page.  Because we never go to user mode, this flag
// is effectively unused by hardware.
//

#define MM_PTE_NOACCESS             0x000   // not expressable on i386
#define MM_PTE_READONLY             0x000
#define MM_PTE_READWRITE            MM_PTE_WRITE_MASK
#define MM_PTE_NOCACHE              MM_PTE_CACHE_DISABLE_MASK
#define MM_PTE_GUARD                MM_PTE_GUARD_MASK
#define MM_PTE_CACHE                0x000

//
// Define the set of bits that MiMakePteProtectionMask will return.  This mask
// can be used to check if an existing PTE has compatible attributes.
//

#define MM_PTE_PROTECTION_MASK      0x0000021B

//
// Define the set of bits that MiMakeSystemPteProtectionMask will return.  This
// mask can be used to check if an existing PTE has compatible attributes.
//

#define MM_SYSTEM_PTE_PROTECTION_MASK 0x0000001B

//
// Define the end of list marker for the linked list contained in a PTE.
//

#define MM_EMPTY_PTE_LIST           ((ULONG)0x3FFFFFFF)

//
// Enumeration to identify what a busy page frame is being used for.
//

typedef enum _MMPFN_BUSY_TYPE {
    MmUnknownUsage,
    MmStackUsage,
    MmVirtualPageTableUsage,
    MmSystemPageTableUsage,
    MmPoolUsage,
    MmVirtualMemoryUsage,
    MmSystemMemoryUsage,
    MmImageUsage,
    MmFsCacheUsage,
    MmContiguousUsage,
    MmDebuggerUsage,
    MmMaximumUsage
} MMPFN_BUSY_TYPE;

//
// PFN free page element.
//
// The low bit of PackedPfnFlink must be clear so that the overloaded
// MMPFN.Pte.Valid is clear.
//
// The low bit of PackedPfnBlink must be clear so that the overloaded
// MMPFN.Busy.Busy is clear.
//

typedef struct _MMPFNFREE {
    USHORT PackedPfnFlink;              // low bit must be clear
    USHORT PackedPfnBlink;              // low bit must be clear
} MMPFNFREE, *PMMPFNFREE;

//
// PFN database element.
//

typedef struct _MMPFN {
    union {
        ULONG Long;
        MMPTE Pte;
        MMPFNFREE Free;
        struct {
            ULONG LockCount : 16;       // low bit must be clear
            ULONG Busy : 1;
            ULONG Reserved : 1;
            ULONG PteIndex : 10;
            ULONG BusyType : 4;
        } Busy;
        struct {
            ULONG LockCount : 16;       // low bit must be clear
            ULONG Busy : 1;
            ULONG ElementIndex : 11;
            ULONG BusyType : 4;
        } FsCache;
        struct {
            ULONG LockCount : 16;       // low bit must be clear
            ULONG Busy : 1;
            ULONG NumberOfUsedPtes : 11;
            ULONG BusyType : 4;
        } Directory;
    };
} MMPFN, *PMMPFN;

#define MM_PFN_NULL                 ((PFN_NUMBER)-1)
#define MM_PACKED_PFN_NULL          ((USHORT)0xFFFE)

#define MM_PFN_DATABASE             ((PMMPFN)MI_CONVERT_PFN_TO_PHYSICAL(MM_DATABASE_PHYSICAL_PAGE))

#define MI_PFN_ELEMENT(pfn)         (&MM_PFN_DATABASE[pfn])
#define MI_PFN_NUMBER(pmmpfn)       ((PFN_NUMBER)ARRAY_ELEMENT_NUMBER(MM_PFN_DATABASE, MMPFN, pmmpfn))

//
// Define the basic unit for the MMPFN.Busy.LockCount field.  The low bit must
// always be clear in order for the entry to not be viewed by the processor as a
// valid PTE, so we always increment or decrement the LockCount field in units
// of two.
//

#define MI_LOCK_COUNT_UNIT          2
#define MI_LOCK_COUNT_MAXIMUM       0xFFFE

//
// PFN region descriptor.
//

typedef struct _MMPFNREGION {
    MMPFNFREE FreePagesByColor[MM_NUMBER_OF_COLORS];
    PFN_COUNT AvailablePages;
} MMPFNREGION, *PMMPFNREGION;

#ifdef CONSOLE_DEVKIT
#define MI_PFN_REGION_SHIFT         14
#define MI_PAGES_IN_PFN_REGION      (1 << MI_PFN_REGION_SHIFT)
#define MI_BYTES_IN_PFN_REGION      (MI_PAGES_IN_PFN_REGION << PAGE_SHIFT)
#define MI_NUMBER_OF_REGIONS        (MM_PAGES_IN_PHYSICAL_MAP / MI_PAGES_IN_PFN_REGION)
#define MI_PFN_REGION(pfn)          (MmPfnRegions[((pfn) >> MI_PFN_REGION_SHIFT)])
#else
#define MI_PAGES_IN_PFN_REGION      (MM_PAGES_IN_PHYSICAL_MAP)
#define MI_BYTES_IN_PFN_REGION      (MI_PAGES_IN_PFN_REGION << PAGE_SHIFT)
#define MI_PFN_REGION(pfn)          &MmRetailPfnRegion
#endif

//
// Define the function signature for a routine that removes a page with the
// supplied busy type code and target page table entry address.
//

typedef
PFN_NUMBER
(FASTCALL *PMMREMOVE_PAGE_ROUTINE)(
    IN MMPFN_BUSY_TYPE BusyType,
    IN PMMPTE TargetPte
    );

//
// Page table entry range structure.
//

typedef struct _MMPTERANGE {
    MMPTE HeadPte;
    PMMPTE FirstCommittedPte;
    PMMPTE LastCommittedPte;
    PMMPTE LastReservedPte;
    PFN_COUNT *AvailablePages;
    PMMREMOVE_PAGE_ROUTINE RemovePageRoutine;
} MMPTERANGE, *PMMPTERANGE;

//
// Address node.
//

typedef struct _MMADDRESS_NODE {
    ULONG_PTR StartingVpn;
    ULONG_PTR EndingVpn;
    struct _MMADDRESS_NODE *Parent;
    struct _MMADDRESS_NODE *LeftChild;
    struct _MMADDRESS_NODE *RightChild;
} MMADDRESS_NODE, *PMMADDRESS_NODE;

//
// Virtual address descriptor.
//

#ifdef __cplusplus
typedef struct _MMVAD : public MMADDRESS_NODE {
#else
typedef struct _MMVAD {
    MMADDRESS_NODE;
#endif
    ULONG AllocationProtect;
} MMVAD, *PMMVAD;

//
// Macros to guard access to the virtual memory space.
//

#define MI_LOCK_ADDRESS_SPACE()     RtlEnterCriticalSectionAndRegion(&MmAddressSpaceLock)
#define MI_UNLOCK_ADDRESS_SPACE()   RtlLeaveCriticalSectionAndRegion(&MmAddressSpaceLock)

//
// Macros to convert between virtual address and virtual page numbers.
//

#define MI_VA_TO_PAGE(va)           ((ULONG_PTR)(va) >> PAGE_SHIFT)
#define MI_VA_TO_VPN(va)            ((ULONG_PTR)(va) >> PAGE_SHIFT)
#define MI_VPN_TO_VA(vpn)           (PVOID)((vpn) << PAGE_SHIFT)
#define MI_VPN_TO_VA_ENDING(vpn)    (PVOID)(((vpn) << PAGE_SHIFT) | (PAGE_SIZE - 1))

//++
//ULONG
//MI_ROUND_TO_SIZE (
//    IN ULONG LENGTH,
//    IN ULONG ALIGNMENT
//    )
//
// Routine Description:
//
//    The ROUND_TO_SIZE macro takes a LENGTH in bytes and rounds it up to a
//    multiple of the alignment.
//
// Arguments:
//
//    LENGTH - LENGTH in bytes to round up to.
//
//    ALIGNMENT - alignment to round to, must be a power of 2, e.g, 2**n.
//
// Return Value:
//
//    Returns the LENGTH rounded up to a multiple of the alignment.
//
//--

#define MI_ROUND_TO_SIZE(LENGTH,ALIGNMENT)                                    \
    (((LENGTH) + ((ALIGNMENT) - 1)) & ~((ALIGNMENT) - 1))

//++
//PVOID
//MI_ALIGN_TO_SIZE (
//    IN PVOID VA
//    IN ULONG ALIGNMENT
//    );
//
// Routine Description:
//
//    The MI_ALIGN_TO_SIZE macro takes a virtual address and returns a
//    virtual address for that page with the specified alignment.
//
// Arguments:
//
//    VA - Virtual address.
//
//    ALIGNMENT - alignment to round to, must be a power of 2, e.g, 2**n.
//
// Return Value:
//
//    Returns the aligned virtual address.
//
//--

#define MI_ALIGN_TO_SIZE(VA,ALIGNMENT)                                        \
    ((PVOID)((ULONG_PTR)(VA) & ~((ULONG_PTR) ALIGNMENT - 1)))

//++
//VOID
//MI_WRITE_PTE (
//    IN PMMPTE PointerPte,
//    IN MMPTE PteContents
//    );
//
// Routine Description:
//
//    MI_WRITE_PTE fills in the specified PTE with the specified contents.
//
// Arguments
//
//    PointerPte - Supplies a PTE to fill.
//
//    PteContents - Supplies the contents to put in the PTE.
//
// Return Value:
//
//    None.
//
//--

#define MI_WRITE_PTE(_PointerPte, _PteContents)                               \
    (*(_PointerPte) = (_PteContents))

//++
//VOID
//MI_WRITE_ZERO_PTE (
//    IN PMMPTE PointerPte
//    );
//
// Routine Description:
//
//    MI_WRITE_PTE fills in the specified PTE with zero.
//
// Arguments
//
//    PointerPte - Supplies a PTE to fill.
//
// Return Value:
//
//    None.
//
//--

#define MI_WRITE_ZERO_PTE(_PointerPte)                                        \
    ((_PointerPte)->Long = 0)

//++
//VOID
//MI_WRITE_AND_FLUSH_PTE(
//    IN PMMPTE PointerPte,
//    IN MMPTE PteContents
//    );
//
// Routine Description:
//
//    MI_WRITE_AND_FLUSH_PTE fills in the specified PTE with the specified
//    contents and invalidates the TLB line associated with the page.
//
// Arguments:
//
//    PointerPte - Supplies a PTE to fill.
//
//    PteContents - Supplies the contents to put in the PTE.
//
// Return Value:
//
//    None.
//
//--

__inline
VOID
MI_WRITE_AND_FLUSH_PTE(
    PMMPTE PointerPte,
    MMPTE PteContents
    )
{
    MI_WRITE_PTE(PointerPte, PteContents);

    __asm {
        mov     eax, PointerPte
        shl     eax, 10             ; eax = MiGetVirtualAddressMappedByPte(eax)
        invlpg  [eax]
    }
}

//++
//VOID
//MI_FLUSH_VA(
//    IN PVOID VirtualAddress
//    );
//
// Routine Description:
//
//    MI_FLUSH_VA invalidates the TLB line associated with the page.
//
// Arguments:
//
//    VirtualAddress - Supplies the virtual address to flush.
//
// Return Value:
//
//    None.
//
//--

__inline
VOID
MI_FLUSH_VA(
    PVOID VirtualAddress
    )
{
    __asm {
        mov     ecx, VirtualAddress
        invlpg  [ecx]
    }
}

//++
//VOID
//MI_DISABLE_CACHING (
//    IN MMPTE PTE
//    );
//
// Routine Description:
//
//    This macro takes a valid PTE and sets the caching state to be
//    disabled.  This is performed by setting the PCD and PWT bits in the PTE.
//
//    Semantics of the overlap between PCD, PWT, and the
//    USWC memory type in the MTRR are:
//
//    PCD   PWT   Mtrr Mem Type      Effective Memory Type
//     1     0    USWC               USWC
//     1     1    USWC               UC
//
//    Since an effective memory type of UC is desired here,
//    the WT bit is set.
//
// Arguments
//
//    PTE - Supplies a pointer to the valid PTE.
//
// Return Value:
//
//    None.
//
//--

#define MI_DISABLE_CACHING(PTE) {                                             \
    ((PTE).Hard.CacheDisable = 1);                                            \
    ((PTE).Hard.WriteThrough = 1);                                            \
}

//++
//VOID
//MI_SET_PTE_WRITE_COMBINE (
//    IN MMPTE PTE
//    );
//
// Routine Description:
//
//    This macro takes a valid PTE and enables WriteCombining as the
//    caching state.  Note that the PTE bits may only be set this way
//    if the Page Attribute Table is present and the PAT has been
//    initialized to provide Write Combining.
//
//    If either of the above conditions is not satisfied, then
//    the macro enables WEAK UC (PCD = 1, PWT = 0) in the PTE.
//
// Arguments
//
//    PTE - Supplies a valid PTE.
//
// Return Value:
//
//    None.
//
//--

#define MI_SET_PTE_WRITE_COMBINE(PTE) {                                       \
    ((PTE).Hard.CacheDisable = 0);                                            \
    ((PTE).Hard.WriteThrough = 1);                                            \
}

//++
//PMMPTE
//MiGetPdeAddress (
//    IN PVOID va
//    );
//
// Routine Description:
//
//    MiGetPdeAddress returns the address of the PDE which maps the
//    given virtual address.
//
// Arguments
//
//    Va - Supplies the virtual address to locate the PDE for.
//
// Return Value:
//
//    The address of the PDE.
//
//--

#define MiGetPdeAddress(va) ((PMMPTE)(((((ULONG)(va)) >> 22) << 2) + PDE_BASE))

//++
//PMMPTE
//MiGetPteAddress (
//    IN PVOID va
//    );
//
// Routine Description:
//
//    MiGetPteAddress returns the address of the PTE which maps the
//    given virtual address.
//
// Arguments
//
//    Va - Supplies the virtual address to locate the PTE for.
//
// Return Value:
//
//    The address of the PTE.
//
//--

#define MiGetPteAddress(va) ((PMMPTE)(((((ULONG)(va)) >> 12) << 2) + PTE_BASE))

//++
//ULONG
//MiGetPdeOffset (
//    IN PVOID va
//    );
//
// Routine Description:
//
//    MiGetPdeOffset returns the offset into a page directory
//    for a given virtual address.
//
// Arguments
//
//    Va - Supplies the virtual address to locate the offset for.
//
// Return Value:
//
//    The offset into the page directory table the corresponding PDE is at.
//
//--

#define MiGetPdeOffset(va) (((ULONG)(va)) >> 22)

//++
//ULONG
//MiGetPteOffset (
//    IN PVOID va
//    );
//
// Routine Description:
//
//    MiGetPteOffset returns the offset into a page table page
//    for a given virtual address.
//
// Arguments
//
//    Va - Supplies the virtual address to locate the offset for.
//
// Return Value:
//
//    The offset into the page table page table the corresponding PTE is at.
//
//--

#define MiGetPteOffset(va) ((((ULONG)(va)) << 10) >> 22)

//++
//PVOID
//MiGetVirtualAddressMappedByPde (
//    IN PMMPTE PTE
//    );
//
// Routine Description:
//
//    MiGetVirtualAddressMappedByPde returns the virtual address
//    which is mapped by a given PDE address.
//
// Arguments
//
//    PDE - Supplies the PDE to get the virtual address for.
//
// Return Value:
//
//    Virtual address mapped by the PDE.
//
//--

#define MiGetVirtualAddressMappedByPde(PDE) ((PVOID)((ULONG)(PDE) << 20))

//++
//PVOID
//MiGetVirtualAddressMappedByPte (
//    IN PMMPTE PTE
//    );
//
// Routine Description:
//
//    MiGetVirtualAddressMappedByPte returns the virtual address
//    which is mapped by a given PTE address.
//
// Arguments
//
//    PTE - Supplies the PTE to get the virtual address for.
//
// Return Value:
//
//    Virtual address mapped by the PTE.
//
//--

#define MiGetVirtualAddressMappedByPte(PTE) ((PVOID)((ULONG)(PTE) << 10))

//++
//LOGICAL
//MiIsPteOnPdeBoundary (
//    IN PVOID PTE
//    );
//
// Routine Description:
//
//    MiIsPteOnPdeBoundary returns TRUE if the PTE is
//    on a page directory entry boundary.
//
// Arguments
//
//    PTE - Supplies the PTE to check.
//
// Return Value:
//
//    TRUE if on a 4MB PDE boundary, FALSE if not.
//
//--

#define MiIsPteOnPdeBoundary(PTE) (((ULONG_PTR)(PTE) & (PAGE_SIZE - 1)) == 0)

//++
//MMPTE
//MiGetValidKernelPdeBits (
//    VOID
//    );
//
// Routine Description:
//
//    MiGetValidKernelPde returns the basic bits for a valid kernel PDE.
//
// Return Value:
//
//    The bits for the PDE.
//
//--

#define MiGetValidKernelPdeBits() \
    (MM_PTE_VALID_MASK | MM_PTE_WRITE_MASK | MM_PTE_OWNER_MASK | MM_PTE_DIRTY_MASK | MM_PTE_ACCESS_MASK)

//++
//MMPTE
//MiGetValidKernelPteBits (
//    VOID
//    );
//
// Routine Description:
//
//    MiGetValidKernelPde returns the basic bits for a valid kernel PTE.
//
// Return Value:
//
//    The bits for the PTE.
//
//--

#define MiGetValidKernelPteBits() \
    (MM_PTE_VALID_MASK | MM_PTE_WRITE_MASK | MM_PTE_DIRTY_MASK | MM_PTE_ACCESS_MASK)

//++
//MMPTE
//MiGetValidCachePteBits (
//    VOID
//    );
//
// Routine Description:
//
//    MiGetValidKernelPde returns the basic bits for a valid cache PTE.
//
// Return Value:
//
//    The bits for the PTE.
//
//--

#define MiGetValidCachePteBits() \
    (MM_PTE_VALID_MASK | MM_PTE_WRITE_MASK | MM_PTE_ACCESS_MASK)

//++
//BOOLEAN
//MI_IS_PHYSICAL_ADDRESS (
//    IN PVOID VA
//    );
//
// Routine Description:
//
//    This macro determines if a given virtual address is really a
//    physical address.
//
// Arguments
//
//    VA - Supplies the virtual address.
//
// Return Value:
//
//    FALSE if it is not a physical address, TRUE if it is.
//
//--

#define MI_IS_PHYSICAL_ADDRESS(Va) \
    (((ULONG)(Va) - MM_PHYSICAL_MAP_BASE) <= (MM_PHYSICAL_MAP_END - MM_PHYSICAL_MAP_BASE))

//++
//ULONG
//MI_CONVERT_PHYSICAL_TO_PFN (
//    IN PVOID VA
//    );
//
// Routine Description:
//
//    This macro converts a physical address (see MI_IS_PHYSICAL_ADDRESS)
//    to its corresponding physical frame number.
//
// Arguments
//
//    VA - Supplies a pointer to the physical address.
//
// Return Value:
//
//    Returns the PFN for the page.
//
//--

#define MI_CONVERT_PHYSICAL_TO_PFN(Va)                                        \
    (((ULONG)(Va) & (MM_BYTES_IN_PHYSICAL_MAP - 1)) >> PAGE_SHIFT)

//++
//PCHAR
//MI_CONVERT_PFN_TO_PHYSICAL (
//    IN PAGE_FRAME_NUMBER Pfn
//    );
//
// Routine Description:
//
//    This macro converts a physical frame number to its corresponding
//    physical address.
//
// Arguments
//
//    Pfn - Supplies the physical frame number.
//
// Return Value:
//
//    Returns the physical address for the page number.
//
//--

#define MI_CONVERT_PFN_TO_PHYSICAL(Pfn)                                       \
    ((PCHAR)MM_SYSTEM_PHYSICAL_MAP + ((ULONG)(Pfn) << PAGE_SHIFT))

//++
//BOOLEAN
//MI_IS_SYSTEM_PTE_ADDRESS (
//    IN PVOID VA
//    );
//
// Routine Description:
//
//    This macro takes a virtual address and determines if
//    it is an address in the system PTE space.
//
// Arguments
//
//    VA - Supplies a virtual address.
//
// Return Value:
//
//    TRUE if the address is in the system PTE space, FALSE if not.
//
//--

#define MI_IS_SYSTEM_PTE_ADDRESS(Va)                                          \
    (((ULONG)(Va) - MM_SYSTEM_PTE_BASE) <= (MM_SYSTEM_PTE_END - MM_SYSTEM_PTE_BASE))

//++
//BOOLEAN
//MI_SIZE_OF_MDL (
//    IN PVOID BASE,
//    IN SIZE_T LENGTH
//    );
//
// Routine Description:
//
//    This function returns the number of bytes required for an MDL for a
//    given buffer and size.
//
// Arguments:
//
//    Base - Supplies the base virtual address for the buffer.
//
//    Length - Supplies the size of the buffer in bytes.
//
// Return Value:
//
//    Returns the number of bytes required to contain the MDL.
//
//--

#define MI_SIZE_OF_MDL(BASE,LENGTH)                                           \
    (sizeof(MDL) + (ADDRESS_AND_SIZE_TO_SPAN_PAGES((BASE), (LENGTH)) * sizeof(PFN_NUMBER)))

//++
//USHORT
//MiPackFreePfn(
//    IN PFN_NUMBER PFN
//    );
//
// Routine Description:
//
//    This function packs a page frame number for storage in the MMPFN free
//    link fields.  The returned number must have the low bit clear in order
//    to make the entry appear as non-busy.
//
// Arguments:
//
//    PFN - Supplies the page frame number.
//
// Return Value:
//
//    Returns the packed form of the page frame number.
//
//--

#define MiPackFreePfn(PFN)                                                \
    ((USHORT)((((PFN_NUMBER)(PFN)) >> MM_NUMBER_OF_COLORS_BITS) << 1))

//++
//PFN_NUMBER
//MiUnpackFreePfn(
//    IN USHORT CPFN,
//    IN ULONG COLOR
//    );
//
// Routine Description:
//
//    This function unpacks a MMPFN free link field to obtain the original
//    page frame number.  The low bit of the packed page frame number will
//    have the low bit clear.
//
// Arguments:
//
//    CPFN - Supplies the packed page frame number.
//
//    COLOR - Supplies the color of the original page frame number.
//
// Return Value:
//
//    Returns the unpacked page frame number.
//
//--

#define MiUnpackFreePfn(CPFN,COLOR)                                           \
    ((((ULONG)(CPFN)) << (MM_NUMBER_OF_COLORS_BITS - 1)) + (ULONG)(COLOR))

//++
//PMMPFN
//MiUnpackFreePfnElement(
//    IN USHORT CPFN,
//    IN ULONG COLOR
//    );
//
// Routine Description:
//
//    This function unpacks a MMPFN free link field to obtain the original
//    page frame element.  The low bit of the packed page frame number will
//    have the low bit clear.
//
// Arguments:
//
//    CPFN - Supplies the packed page frame number.
//
//    COLOR - Supplies the color of the original page frame number.
//
// Return Value:
//
//    Returns the unpacked page frame element.
//
//--

#define MiUnpackFreePfnElement(CPFN,COLOR)                                    \
    MI_PFN_ELEMENT(MiUnpackFreePfn((CPFN),(COLOR)))

//++
//MMCOLOR
//MiGetPfnColor(
//    IN ULONG PFN
//    );
//
// Routine Description:
//
//    This function computes the color of the supplied page frame number.
//
// Arguments:
//
//    PFN - Supplies the page frame number.
//
// Return Value:
//
//    Returns the color of the page frame number.
//
//--

#define MiGetPfnColor(PFN)                                                    \
    ((PFN_NUMBER)(PFN) & (MM_NUMBER_OF_COLORS - 1))

// PVOID
// MiFindEmptyAddressRangeDown (
//    IN ULONG_PTR SizeOfRange,
//    IN PVOID HighestAddressToEndAt,
//    IN ULONG_PTR Alignment
//    )
//
// Routine Description:
//
//    The function examines the virtual address descriptors to locate
//    an unused range of the specified size and returns the starting
//    address of the range.  This routine looks from the top down.
//
// Arguments:
//
//    SizeOfRange - Supplies the size in bytes of the range to locate.
//
//    HighestAddressToEndAt - Supplies the virtual address to begin looking
//                            at.
//
//    Alignment - Supplies the alignment for the address.  Must be
//                 a power of 2 and greater than the page_size.
//
//Return Value:
//
//    Returns the starting address of a suitable range.
//

#define MiFindEmptyAddressRangeDown(SizeOfRange,HighestAddressToEndAt,Alignment) \
               (MiFindEmptyAddressRangeDownTree(                             \
                    (SizeOfRange),                                           \
                    (HighestAddressToEndAt),                                 \
                    (Alignment),                                             \
                    MmVadRoot))

// PMMVAD
// MiGetPreviousVad (
//     IN PMMVAD Vad
//     )
//
// Routine Description:
//
//     This function locates the virtual address descriptor which contains
//     the address range which logically precedes the specified virtual
//     address descriptor.
//
// Arguments:
//
//     Vad - Supplies a pointer to a virtual address descriptor.
//
// Return Value:
//
//     Returns a pointer to the virtual address descriptor containing the
//     next address range, NULL if none.
//
//

#define MiGetPreviousVad(VAD) ((PMMVAD)MiGetPreviousNode((PMMADDRESS_NODE)(VAD)))

// PMMVAD
// MiGetNextVad (
//     IN PMMVAD Vad
//     )
//
// Routine Description:
//
//     This function locates the virtual address descriptor which contains
//     the address range which logically follows the specified address range.
//
// Arguments:
//
//     VAD - Supplies a pointer to a virtual address descriptor.
//
// Return Value:
//
//     Returns a pointer to the virtual address descriptor containing the
//     next address range, NULL if none.
//

#define MiGetNextVad(VAD) ((PMMVAD)MiGetNextNode((PMMADDRESS_NODE)(VAD)))

// PMMVAD
// MiCheckForConflictingVad (
//     IN PVOID StartingAddress,
//     IN PVOID EndingAddress
//     )
//
// Routine Description:
//
//     The function determines if any addresses between a given starting and
//     ending address is contained within a virtual address descriptor.
//
// Arguments:
//
//     StartingAddress - Supplies the virtual address to locate a containing
//                       descriptor.
//
//     EndingAddress - Supplies the virtual address to locate a containing
//                       descriptor.
//
// Return Value:
//
//     Returns a pointer to the first conflicting virtual address descriptor
//     if one is found, otherwise a NULL value is returned.
//

#define MiCheckForConflictingVad(StartingAddress,EndingAddress)           \
    ((PMMVAD)MiCheckForConflictingNode(                                   \
                    MI_VA_TO_VPN(StartingAddress),                        \
                    MI_VA_TO_VPN(EndingAddress),                          \
                    MmVadRoot))

//++
//LOGICAL
//MiIsRetryIoStatus(
//    IN NTSTATUS S
//    );
//
// Routine Description:
//
//    This function tests the supplied status code to see if the error might
//    have been caused by a verifier induced error or by temporarily being out
//    of system resources.
//
// Arguments:
//
//    S - Supplies the status code to test.
//
// Return Value:
//
//    Returns TRUE if the I/O operation should be retried, else FALSE.
//
//--

#define MiIsRetryIoStatus(S)                                                  \
    (((S) == STATUS_INSUFFICIENT_RESOURCES) || ((S) == STATUS_NO_MEMORY))

//
// Routines which operate on the page frame database.
//

VOID
MiInitializePfnDatabase(
    VOID
    );

VOID
MiReinitializePfnDatabase(
    VOID
    );

VOID
FASTCALL
MiInsertPageInFreeList(
    IN PFN_NUMBER PageFrameNumber,
    IN BOOLEAN InsertAtHeadList
    );

VOID
MiInsertPhysicalMemoryInFreeList(
    IN PFN_NUMBER PageFrameNumber,
    IN PFN_NUMBER EndingPageFrameNumberExclusive
    );

VOID
FASTCALL
MiRemovePageFromFreeList(
    IN PFN_NUMBER PageFrameNumber
    );

PFN_NUMBER
FASTCALL
MiRemoveAnyPage(
    IN MMPFN_BUSY_TYPE BusyType,
    IN PMMPTE TargetPte
    );

PFN_NUMBER
FASTCALL
MiRemoveZeroPage(
    IN MMPFN_BUSY_TYPE BusyType,
    IN PMMPTE TargetPte
    );

PFN_NUMBER
FASTCALL
MiRemoveDebuggerPage(
    IN MMPFN_BUSY_TYPE BusyType,
    IN PMMPTE TargetPte
    );

VOID
FASTCALL
MiRelocateBusyPage(
    IN PFN_NUMBER PageFrameNumber
    );

VOID
FASTCALL
MiReleasePageOwnership(
    IN PFN_NUMBER PageFrameNumber
    );

//
// Routines to obtain and release system PTEs.
//

PMMPTE
MiReserveSystemPtes(
    IN PMMPTERANGE PteRange,
    IN PFN_COUNT NumberOfPtes
    );

VOID
MiReleaseSystemPtes(
    IN PMMPTERANGE PteRange,
    IN PMMPTE StartingPte,
    IN PFN_COUNT NumberOfPtes
    );

VOID
FASTCALL
MiZeroAndFlushPtes(
    IN PMMPTE StartingPte,
    IN PFN_COUNT NumberOfPtes
    );

//
// Routines which operate on an address tree.
//

PMMADDRESS_NODE
FASTCALL
MiGetNextNode(
   IN PMMADDRESS_NODE Node
   );

PMMADDRESS_NODE
FASTCALL
MiGetPreviousNode(
   IN PMMADDRESS_NODE Node
   );

VOID
FASTCALL
MiInsertNode(
   IN PMMADDRESS_NODE Node,
   IN OUT PMMADDRESS_NODE *Root
   );

VOID
FASTCALL
MiRemoveNode(
   IN PMMADDRESS_NODE Node,
   IN OUT PMMADDRESS_NODE *Root
   );

PMMADDRESS_NODE
FASTCALL
MiLocateAddressInTree(
   IN ULONG_PTR Vpn,
   IN PMMADDRESS_NODE *Root
   );

PMMADDRESS_NODE
MiCheckForConflictingNode(
   IN ULONG_PTR StartVpn,
   IN ULONG_PTR EndVpn,
   IN PMMADDRESS_NODE Root
   );

PVOID
MiFindEmptyAddressRangeInTree(
   IN SIZE_T SizeOfRange,
   IN ULONG_PTR Alignment,
   IN PMMADDRESS_NODE Root,
   OUT PMMADDRESS_NODE *PreviousVad
   );

PVOID
MiFindEmptyAddressRangeDownTree(
   IN SIZE_T SizeOfRange,
   IN PVOID HighestAddressToEndAt,
   IN ULONG_PTR Alignment,
   IN PMMADDRESS_NODE Root
   );

//
// Routines which operate on the tree of virtual address descriptors.
//

VOID
MiInsertVad(
    IN PMMVAD Vad
    );

VOID
MiRemoveVad(
    IN PMMVAD Vad
    );

PMMVAD
FASTCALL
MiLocateAddress(
    IN PVOID Vad
    );

PVOID
MiFindEmptyAddressRange(
    IN SIZE_T SizeOfRange,
    IN ULONG_PTR Alignment,
    IN ULONG QuickCheck
    );

//
// Miscellaneous routines.
//

PVOID
MiAllocateMappedMemory(
    IN PMMPTERANGE PteRange,
    IN MMPFN_BUSY_TYPE BusyType,
    IN ULONG Protect,
    IN SIZE_T NumberOfBytes,
    IN PMMREMOVE_PAGE_ROUTINE RemovePageRoutine,
    IN BOOLEAN AddBarrierPage
    );

PFN_COUNT
MiFreeMappedMemory(
    IN PMMPTERANGE PteRange,
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes OPTIONAL
    );

BOOLEAN
FASTCALL
MiMakePteProtectionMask(
    IN ULONG Protect,
    OUT PULONG PteProtectionMask
    );

BOOLEAN
FASTCALL
MiMakeSystemPteProtectionMask(
    IN ULONG Protect,
    OUT PMMPTE ProtoPte
    );

ULONG
FASTCALL
MiDecodePteProtectionMask(
    IN ULONG PteProtectionMask
    );

//
// Global data structure.
//

typedef struct _MMGLOBALDATA {
    PMMPFNREGION RetailPfnRegion;
    PMMPTERANGE SystemPteRange;
    PULONG AvailablePages;
    PFN_COUNT *AllocatedPagesByUsage;
    PRTL_CRITICAL_SECTION AddressSpaceLock;
    PMMADDRESS_NODE *VadRoot;
    PMMADDRESS_NODE *VadHint;
    PMMADDRESS_NODE *VadFreeHint;
} MMGLOBALDATA, *PMMGLOBALDATA;

//
// External symbols.
//

extern PFN_COUNT MmNumberOfPhysicalPages;
extern PMMPFNREGION MmPfnRegions[];
extern MMPFNREGION MmRetailPfnRegion;
extern MMPFNREGION MmDeveloperKitPfnRegion;
extern PFN_COUNT MmAvailablePages;
extern PFN_COUNT MmAllocatedPagesByUsage[MmMaximumUsage];
extern RTL_CRITICAL_SECTION MmAddressSpaceLock;
extern SIZE_T MmVirtualMemoryBytesReserved;
extern PMMADDRESS_NODE MmVadRoot;
extern PMMADDRESS_NODE MmVadHint;
extern PMMADDRESS_NODE MmVadFreeHint;
extern MMPTERANGE MmSystemPteRange;
extern MMPTERANGE MmDeveloperKitPteRange;
extern PFSCACHE_ELEMENT FscElementArray;
extern ULONG FscNumberOfCachePages;
extern PMMPTE FscWriteFFsPtes;

#endif  // MI
