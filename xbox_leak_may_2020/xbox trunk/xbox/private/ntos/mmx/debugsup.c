/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    debugsup.c

Abstract:

    This module implements routines which provide support for the debugger.

--*/

#include "mi.h"

PVOID
MmDbgWriteCheck(
    IN PVOID VirtualAddress,
    IN PHARDWARE_PTE Opaque
    )
/*++

Routine Description:

    i386/486 implementation specific:

    This routine checks the specified virtual address and if it is
    valid and writable, it returns that virtual address, otherwise
    it returns NULL.

Arguments:

    VirtualAddress - Supplies the virtual address to check.

    Opaque - Supplies an opaque pointer.

Return Value:

    Returns NULL if the address is not valid or writable, otherwise
    returns the virtual address.

Environment:

    Kernel mode IRQL at DISPATCH_LEVEL or greater.

--*/
{
    PMMPTE InputPte;
    PMMPTE PointerPte;
    MMPTE TempPte;

    InputPte = (PMMPTE)Opaque;
    InputPte->Long = 0;

    if (!MmIsAddressValid(VirtualAddress)) {
        return NULL;
    }

    PointerPte = MiGetPdeAddress(VirtualAddress);
    if (PointerPte->Hard.LargePage == 0) {
        PointerPte = MiGetPteAddress(VirtualAddress);
    }

    if (PointerPte->Hard.Write == 0) {

        TempPte = *PointerPte;

        *InputPte = TempPte;

        TempPte.Hard.Write = 1;

        MI_WRITE_AND_FLUSH_PTE(PointerPte, TempPte);
    }

    return VirtualAddress;
}

VOID
MmDbgReleaseAddress(
    IN PVOID VirtualAddress,
    IN PHARDWARE_PTE Opaque
    )
/*++

Routine Description:

    i386/486 implementation specific:

    This routine resets the specified virtual address access permissions
    to its original state.

Arguments:

    VirtualAddress - Supplies the virtual address to check.

    Opaque - Supplies an opaque pointer.

Return Value:

    None.

Environment:

    Kernel mode IRQL at DISPATCH_LEVEL or greater.

--*/
{
    MMPTE TempPte;
    PMMPTE PointerPte;
    PMMPTE InputPte;

    InputPte = (PMMPTE)Opaque;

    ASSERT(MmIsAddressValid(VirtualAddress));

    if (InputPte->Long != 0) {

        PointerPte = MiGetPdeAddress(VirtualAddress);
        if (PointerPte->Hard.LargePage == 0) {
            PointerPte = MiGetPteAddress(VirtualAddress);
        }

        TempPte = *InputPte;

        MI_WRITE_AND_FLUSH_PTE(PointerPte, TempPte);
    }
}

PVOID64
MmDbgTranslatePhysicalAddress64(
    IN PHYSICAL_ADDRESS PhysicalAddress
    )
/*++

Routine Description:

    i386/486 implementation specific:

    This routine maps the specified physical address and returns
    the virtual address which maps the physical address.

    The next call to MmDbgTranslatePhysicalAddress removes the
    previous physical address translation, hence only a single
    physical address can be examined at a time (can't cross page
    boundaries).

Arguments:

    PhysicalAddress - Supplies the physical address to map and translate.

Return Value:

    The virtual address which corresponds to the physical address.

Environment:

    Kernel mode IRQL at DISPATCH_LEVEL or greater.

--*/
{
    PMMPTE DebugPde;
    MMPTE TempPte;

    DebugPde = MiGetPdeAddress(MM_DEBUG_VA);

    TempPte.Long = MiGetValidKernelPteBits();
    MI_DISABLE_CACHING(TempPte);
    TempPte.Long |= (PhysicalAddress.LowPart & (~((1 << PAGE_SHIFT_LARGE) - 1)));
    TempPte.Hard.LargePage = 1;

    MI_WRITE_PTE(DebugPde, TempPte);
    MI_FLUSH_VA((PVOID)MM_DEBUG_VA);

    return (PVOID64)((ULONG)MM_DEBUG_VA + BYTE_OFFSET_LARGE(PhysicalAddress.LowPart));
}

#ifdef DEVKIT

PVOID
MmDbgAllocateMemory(
    IN SIZE_T NumberOfBytes,
    IN ULONG Protect
    )
/*++

Routine Description:

    This routine allocates a range of memory in the debugger portion of the
    address space.  The pages are allocated from the extra memory pool available
    on development systems.

Arguments:

    NumberOfBytes - Supplies the number of bytes to allocate.

    Protect - Supplies the type of protection and cache mapping to use for the
              allocation.

Return Value:

    NON-NULL - Returns a pointer (virtual address in the nonpaged portion
               of the system) to the allocated physically contiguous
               memory.

    NULL - The specified request could not be satisfied.

Environment:

    Kernel mode, <= DISPATCH_LEVEL

--*/
{
    return MiAllocateMappedMemory(&MmDeveloperKitPteRange, MmDebuggerUsage,
        Protect, NumberOfBytes, MmDeveloperKitPteRange.RemovePageRoutine, FALSE);
}

ULONG
MmDbgFreeMemory(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes
    )
/*++

Routine Description:

    This routine deallocates a range of memory in the debugger portion of the
    address space.  This is designed to free pages acquired via
    MmDbgAllocateMemory only.

Arguments:

    BaseAddress - Supplies the base virtual address where the system memory
                  resides.

    NumberOfBytes - Supplies the number of bytes allocated to the request.
                    This number can be zero to free the original size passed to
                    MmDbgAllocateMemory.

Return Value:

    Returns the number of pages deallocated.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    return MiFreeMappedMemory(&MmDeveloperKitPteRange, BaseAddress, NumberOfBytes);
}

PFN_COUNT
MmDbgQueryAvailablePages(
    VOID
    )
/*++

Routine Description:

    This routine returns the number of available pages in the extra memory
    region available on development kits.

Arguments:

    None.

Return Value:

    Number of available pages.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    //
    // Instead of using MmDeveloperKitPfnRegion, indirect through
    // MmDeveloperKitPteRange so that we handle the case of the DVT with only
    // 64 megabytes of memory.
    //

    return *MmDeveloperKitPteRange.AvailablePages;
}

#endif
