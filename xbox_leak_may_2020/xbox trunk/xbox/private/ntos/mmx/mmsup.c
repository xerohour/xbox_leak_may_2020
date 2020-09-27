/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    mmsup.c

Abstract:

    This module implements the various routines for miscellaneous support
    operations for memory management.

--*/

#include "mi.h"

//
// Table to convert external protection codes to internal memory manager codes.
//

const CCHAR MmProtectToPteProtectionMask[16] = {
    -1,
    MM_PTE_READONLY,
    MM_PTE_READONLY,
    -1,
    MM_PTE_READWRITE,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1
};

BOOLEAN
MmIsAddressValid(
    IN PVOID VirtualAddress
    )
/*++

Routine Description:

    For a given virtual address this function returns TRUE if no page fault
    will occur for a read operation on the address, FALSE otherwise.

    Note that after this routine was called, if appropriate locks are not
    held, a non-faulting address could fault.

Arguments:

    VirtualAddress - Supplies the virtual address to check.

Return Value:

    TRUE if a no page fault would be generated reading the virtual address,
    FALSE otherwise.

Environment:

    Kernel mode.

--*/
{
    PMMPTE PointerPte;

    PointerPte = MiGetPdeAddress (VirtualAddress);
    if (PointerPte->Hard.Valid == 0) {
        return FALSE;
    }

    if (PointerPte->Hard.LargePage != 0) {
        return TRUE;
    }

    PointerPte = MiGetPteAddress (VirtualAddress);
    if (PointerPte->Hard.Valid == 0) {
        return FALSE;
    }

    //
    // Make sure we're not treating a page directory as a page table here for
    // the case where the page directory is mapping a large page.  This is
    // because the large page bit is valid in PDE formats, but reserved in
    // PTE formats and will cause a trap.  A virtual address like c0200000
    // triggers this case.  It's not enough to just check the large page bit
    // in the PTE below because of course that bit's been reused by other
    // steppings of the processor so we have to look at the address too.
    //
    if (PointerPte->Hard.LargePage != 0) {
        PVOID Va;

        Va = MiGetVirtualAddressMappedByPde (PointerPte);
        if (MI_IS_PHYSICAL_ADDRESS(Va)) {
            return FALSE;
        }
    }

    return TRUE;
}

PVOID
MiAllocateMappedMemory(
    IN PMMPTERANGE PteRange,
    IN MMPFN_BUSY_TYPE BusyType,
    IN ULONG Protect,
    IN SIZE_T NumberOfBytes,
    IN PMMREMOVE_PAGE_ROUTINE RemovePageRoutine,
    IN BOOLEAN AddBarrierPage
    )
/*++

Routine Description:

    This routine allocates a number of pages in the system part of the address
    space.  The page frames are marked with the supplied busy type code and the
    PTEs are configured with the supplied cache type.  The RemovePageRoutine
    parameter is used to specify the source of the pages (non-zeroed, zeroed,
    etc.).

Arguments:

    PteRange - Supplies the PTE range to allocate the pages from.

    BusyType - Usage code to place in allocated page frame.

    Protect - Supplies the type of protection and cache mapping to use for the
              allocation.

    NumberOfBytes - Supplies the number of byte to allocate.

    RemovePageRoutine - Supplies the allocator routine to use grab pages.

    AddBarrierPage - Supplies whether or not to add an extra barrier page.

Return Value:

    Returns the virtual address of the allocated memory.  When AddBarrierPage is
    TRUE, the returned address points at the barrier page.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    MMPTE TempPte;
    KIRQL OldIrql;
    PFN_COUNT NumberOfPages;
    PFN_COUNT NumberOfPtes;
    PMMPTE StartingPte;
    PMMPTE PointerPte;
    PMMPTE EndingPte;

    //
    // Convert the protect code to a PTE mask.
    //

    if (!MiMakeSystemPteProtectionMask(Protect, &TempPte)) {
        return NULL;
    }

    MI_LOCK_MM(&OldIrql);

    //
    // Compute the number of pages to allocate.
    //

    NumberOfPages = BYTES_TO_PAGES(NumberOfBytes);
    ASSERT(NumberOfPages != 0);

    //
    // Verify that there are enough pages available to satisfy the request.
    // This check doesn't take into account the pages that may be taken when we
    // reserve system PTEs below.
    //

    if (*PteRange->AvailablePages < NumberOfPages) {
        MI_UNLOCK_MM(OldIrql);
        return NULL;
    }

    //
    // Compute the number of PTEs that will be required to map the allocated
    // memory.
    //

    NumberOfPtes = NumberOfPages;

    if (AddBarrierPage) {
        NumberOfPtes++;
    }

    //
    // Reserve the PTEs for the allocated memory.
    //

    StartingPte = MiReserveSystemPtes(PteRange, NumberOfPtes);

    if (StartingPte == NULL) {
        MI_UNLOCK_MM(OldIrql);
        return NULL;
    }

    PointerPte = StartingPte;

    //
    // Verify that there are enough pages available to satisfy the request.  We
    // have to check again after reserving the system PTEs because that may have
    // consumed the pages that we thought we had available above.
    //

    if (*PteRange->AvailablePages < NumberOfPages) {
        MiReleaseSystemPtes(PteRange, StartingPte, NumberOfPtes);
        MI_UNLOCK_MM(OldIrql);
        return NULL;
    }

    if (AddBarrierPage) {

        //
        // Fill in the barrier page to protect against stack overflow errors.
        //

        MI_WRITE_ZERO_PTE(PointerPte);
        PointerPte++;
    }

    //
    // Allocate the pages.
    //

    EndingPte = PointerPte + NumberOfPages - 1;

    while (PointerPte <= EndingPte) {
        TempPte.Hard.PageFrameNumber = RemovePageRoutine(BusyType, PointerPte);
        MI_WRITE_PTE(PointerPte, TempPte);
        PointerPte++;
    }

    //
    // Mark the last page of the allocation so that we can find it later if we
    // need to free the buffer without knowing the original size.
    //

    EndingPte->Hard.GuardOrEndOfAllocation = 1;

    //
    // Write combined accesses may not check the processor's cache, so force a
    // flush of the TLB and cache now to ensure coherency.
    //
    // Flush the cache for uncached allocations so that all cache lines from the
    // page are out of the processor's caches.  The pages are likely to be
    // shared with an external device and the external device may not snoop the
    // cache lines.
    //

    if (Protect & (PAGE_WRITECOMBINE | PAGE_NOCACHE)) {
        KeFlushCurrentTbAndInvalidateAllCaches();
    }

    MI_UNLOCK_MM(OldIrql);

    return MiGetVirtualAddressMappedByPte(StartingPte);
}

PFN_COUNT
MiFreeMappedMemory(
    IN PMMPTERANGE PteRange,
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes OPTIONAL
    )
/*++

Routine Description:

    This routine deallocates a number of pages in the system part of the address
    space.  This is designed to free pages acquired via MiAllocateMappedMemory.

    For stack allocations, the BaseAddress parameter should be pointing at the
    barrier page and NumberOfBytes should include the size of the barrier page.

Arguments:

    PteRange - Supplies the PTE range to free the pages to.

    BaseAddress - Supplies the virtual address of the buffer.

    NumberOfBytes - Supplies the number of bytes in the buffer.  If the count is
                    zero, then the routine attempts to determine the size of the
                    buffer by finding the page frame with the "end of
                    allocation" flag set.

Return Value:

    Returns the number of pages deallocated.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    KIRQL OldIrql;
    PMMPTE StartingPte;
    PMMPTE PointerPte;
    MMPTE TempPte;
    PMMPTE EndingPte;
    PFN_NUMBER PageFrameNumber;
    ULONG EndOfAllocation;
    PFN_COUNT NumberOfPages;

    ASSERT(MiGetPteAddress(BaseAddress) >= PteRange->FirstCommittedPte);
    ASSERT(MiGetPteAddress(BaseAddress) < PteRange->LastCommittedPte);
    ASSERT(((ULONG_PTR)BaseAddress & (PAGE_SIZE - 1)) == 0);

    //
    // Free the pages.
    //

    MI_LOCK_MM(&OldIrql);

    StartingPte = MiGetPteAddress(BaseAddress);
    PointerPte = StartingPte;
    TempPte.Long = 0;

    if (NumberOfBytes != 0) {

        EndingPte = StartingPte + BYTES_TO_PAGES(NumberOfBytes) - 1;

        while (PointerPte <= EndingPte) {

            //
            // If we know the size of the original allocation, then the caller
            // is allowed to free any pages in the original allocation.  For
            // example, the loader may have freed pages used at initialization
            // time only.
            //

            if (PointerPte->Hard.Valid != 0) {
                PageFrameNumber = PointerPte->Hard.PageFrameNumber;
                MI_WRITE_AND_FLUSH_PTE(PointerPte, TempPte);
                MiReleasePageOwnership(PageFrameNumber);
            }

            PointerPte++;
        }

    } else {

        do {

            //
            // If we don't know the size of the original allocation, then the
            // caller must not free any of the pages since the page frame is
            // used to store the "end of allocation" flag.
            //

            ASSERT(PointerPte->Hard.Valid != 0);

            PageFrameNumber = PointerPte->Hard.PageFrameNumber;
            EndOfAllocation = PointerPte->Hard.GuardOrEndOfAllocation;
            MI_WRITE_AND_FLUSH_PTE(PointerPte, TempPte);
            MiReleasePageOwnership(PageFrameNumber);

            PointerPte++;

        } while (!EndOfAllocation);
    }

    //
    // Release the PTEs for the allocated memory.
    //

    NumberOfPages = PointerPte - StartingPte;
    MiReleaseSystemPtes(PteRange, StartingPte, NumberOfPages);

    MI_UNLOCK_MM(OldIrql);

    return NumberOfPages;
}

BOOLEAN
FASTCALL
MiMakePteProtectionMask(
    IN ULONG Protect,
    OUT PULONG PteProtectionMask
    )
/*++

Routine Description:

    This routine translates the access protection code used by external APIs
    to the PTE bit mask that implements that policy.

Arguments:

    Protect - Supplies the protection code (e.g., PAGE_READWRITE).

    PteProtectionMask - Supplies a pointer to the variable that will receive
                        the PTE protection mask (e.g., MM_PTE_READWRITE).

Return Value:

    TRUE if the protection code was successfully decoded, else FALSE.

Environment:

    Kernel mode.

--*/
{
    ULONG Field1;
    ULONG Field2;
    ULONG Mask;

    //
    // Check for unknown protection bits.
    //

    if (Protect & ~(PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE | 0xFF)) {
        return FALSE;
    }

    //
    // None of the protection attributes are valid for "no access" pages.
    //

    if (Protect & PAGE_NOACCESS) {

        if (Protect & (PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE)) {
            return FALSE;
        }
    }

    //
    // Only one of the cache protection attributes may be specified.
    //

    if ((Protect & PAGE_NOCACHE) && (Protect & PAGE_WRITECOMBINE)) {
        return FALSE;
    }

    //
    // One of the nibbles must be non-zero and the other must be zero.
    //

    Field1 = Protect & 0xF;
    Field2 = (Protect >> 4) & 0xF;

    if ((Field1 == 0 && Field2 == 0) || (Field1 != 0 && Field2 != 0)) {
        return FALSE;
    }

    //
    // Check if the table knows how to translate the protection code.
    //

    Mask = (ULONG)(LONG)(CCHAR)MmProtectToPteProtectionMask[Field1 | Field2];

    if (Mask == (ULONG)-1) {
        return FALSE;
    }

    //
    // Apply the rest of the protection attributes to the PTE mask.
    //

    if ((Protect & (PAGE_NOACCESS | PAGE_GUARD)) == 0) {
        Mask |= MM_PTE_VALID_MASK;
    } else if (Protect & PAGE_GUARD) {
        Mask |= MM_PTE_GUARD;
    }

    if (Protect & PAGE_NOCACHE) {
        Mask |= MM_PTE_CACHE_DISABLE_MASK;
    } else if (Protect & PAGE_WRITECOMBINE) {
        Mask |= MM_PTE_WRITE_THROUGH_MASK;
    }

    ASSERT((Mask & ~(MM_PTE_PROTECTION_MASK)) == 0);

    *PteProtectionMask = Mask;

    return TRUE;
}

BOOLEAN
FASTCALL
MiMakeSystemPteProtectionMask(
    IN ULONG Protect,
    OUT PMMPTE ProtoPte
    )
/*++

Routine Description:

    This routine translates the access protection code used by external APIs
    to the PTE bit mask that implements that policy.

Arguments:

    Protect - Supplies the protection code (e.g., PAGE_READWRITE).

    ProtoPte - Supplies a pointer to the variable that will receive
               the PTE protection mask (e.g., MM_PTE_READWRITE).

Return Value:

    TRUE if the protection code was successfully decoded, else FALSE.

Environment:

    Kernel mode.

--*/
{
    ULONG Mask;

    Mask = 0;

    //
    // Check for unknown protection bits.
    //

    if (Protect & ~(PAGE_NOCACHE | PAGE_WRITECOMBINE | PAGE_READWRITE |
        PAGE_READONLY)) {
        return FALSE;
    }

    //
    // Only one of the page protection attributes may be specified.
    //

    switch (Protect & (PAGE_READONLY | PAGE_READWRITE)) {

        case PAGE_READONLY:
            Mask = (MM_PTE_VALID_MASK | MM_PTE_DIRTY_MASK | MM_PTE_ACCESS_MASK);
            break;

        case PAGE_READWRITE:
            Mask = (MM_PTE_VALID_MASK | MM_PTE_WRITE_MASK | MM_PTE_DIRTY_MASK |
                MM_PTE_ACCESS_MASK);
            break;

        default:
            return FALSE;
    }

    //
    // Only one of the cache protection attributes may be specified.
    //

    switch (Protect & (PAGE_NOCACHE | PAGE_WRITECOMBINE)) {

        case 0:
            break;

        case PAGE_NOCACHE:
            Mask |= MM_PTE_CACHE_DISABLE_MASK;
            break;

        case PAGE_WRITECOMBINE:
            Mask |= MM_PTE_WRITE_THROUGH_MASK;
            break;

        default:
            return FALSE;
    }

    ProtoPte->Long = Mask;

    return TRUE;
}

ULONG
FASTCALL
MiDecodePteProtectionMask(
    IN ULONG PteProtectionMask
    )
/*++

Routine Description:

    This routine translate a PTE protection bit mask into the external access
    protection code used by external APIs.

Arguments:

    PteProtectionMask - Supplies the PTE protection bit mask.

Return Value:

    The protection code equivalent (e.g., PAGE_READWRITE).

Environment:

    Kernel mode.

--*/
{
    ULONG Protect;

    if (PteProtectionMask & MM_PTE_READWRITE) {
        Protect = PAGE_READWRITE;
    } else {
        Protect = PAGE_READONLY;
    }

    if ((PteProtectionMask & MM_PTE_VALID_MASK) == 0) {

        if (PteProtectionMask & MM_PTE_GUARD) {
            Protect |= PAGE_GUARD;
        } else {
            Protect = PAGE_NOACCESS;
        }
    }

    if (PteProtectionMask & MM_PTE_CACHE_DISABLE_MASK) {
        Protect |= PAGE_NOCACHE;
    } else if (PteProtectionMask & MM_PTE_WRITE_THROUGH_MASK) {
        Protect |= PAGE_WRITECOMBINE;
    }

    return Protect;
}

ULONG
MmQueryAddressProtect(
    IN PVOID VirtualAddress
    )
/*++

Routine Description:

    This routine queries the protection code associated with the specified base
    address.

Arguments:

    VirtualAddress - Supplies the virtual address to return the protection code
                     for.

Return Value:

    Returns the page protection code.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    ULONG Protect;
    KIRQL OldIrql;
    PMMPTE PointerPte;
    MMPTE TempPte;

    MI_LOCK_MM(&OldIrql);

    PointerPte = MiGetPdeAddress(VirtualAddress);
    TempPte = *PointerPte;

    if (TempPte.Hard.Valid != 0) {

        if (TempPte.Hard.LargePage == 0) {

            PointerPte = MiGetPteAddress(VirtualAddress);
            TempPte = *PointerPte;

            //
            // Addresses in system space must be marked as valid.  Addresses in
            // user space may be marked invalid if they are PAGE_NOACCESS or
            // PAGE_GUARD pages.
            //

            if ((TempPte.Hard.Valid != 0) ||
                ((TempPte.Long != 0) && (VirtualAddress <= MM_HIGHEST_USER_ADDRESS))) {
                Protect = MiDecodePteProtectionMask(TempPte.Long);
            } else {
                Protect = 0;
            }

        } else {
            Protect = MiDecodePteProtectionMask(TempPte.Long);
        }

    } else {
        Protect = 0;
    }

    MI_UNLOCK_MM(OldIrql);

    return Protect;
}

VOID
MmSetAddressProtect(
    IN PVOID BaseAddress,
    IN ULONG NumberOfBytes,
    IN ULONG NewProtect
    )
/*++

Routine Description:

    This routine applies the protection code to the specified address range.

Arguments:

    BaseAddress - Supplies the starting virtual address to change the protection
                  code for.

    NumberOfBytes - Supplies the number of bytes to change.

    NewProtect - Supplies the type of protection and cache mapping to use for
                 the allocation.

Return Value:

    None.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    MMPTE ProtoPte;
    KIRQL OldIrql;
    PMMPTE PointerPte;
    PMMPTE EndingPte;
    MMPTE TempPte;

    ASSERT(MI_IS_PHYSICAL_ADDRESS(BaseAddress) || MI_IS_SYSTEM_PTE_ADDRESS(BaseAddress));
    ASSERT(NumberOfBytes != 0);

    //
    // Convert the protect code to a PTE mask.
    //

    if (!MiMakeSystemPteProtectionMask(NewProtect, &ProtoPte)) {
        return;
    }

    MI_LOCK_MM(&OldIrql);

    PointerPte = MiGetPteAddress(BaseAddress);
    EndingPte = MiGetPteAddress((PUCHAR)BaseAddress + NumberOfBytes - 1);

    while (PointerPte <= EndingPte) {

        TempPte = *PointerPte;

        ASSERT(TempPte.Hard.Valid != 0);

        if ((TempPte.Long & MM_SYSTEM_PTE_PROTECTION_MASK) != ProtoPte.Long) {

            TempPte.Long = ((TempPte.Long & ~MM_SYSTEM_PTE_PROTECTION_MASK) |
                ProtoPte.Long);

            MI_WRITE_AND_FLUSH_PTE(PointerPte, TempPte);
        }

        PointerPte++;
    }

    //
    // Write combined accesses may not check the processor's cache, so force a
    // flush of the TLB and cache now to ensure coherency.
    //
    // Flush the cache for uncached allocations so that all cache lines from the
    // page are out of the processor's caches.  The pages are likely to be
    // shared with an external device and the external device may not snoop the
    // cache lines.
    //

    if (NewProtect & (PAGE_WRITECOMBINE | PAGE_NOCACHE)) {
        KeFlushCurrentTbAndInvalidateAllCaches();
    }

    MI_UNLOCK_MM(OldIrql);
}

NTSTATUS
MmQueryStatistics(
    IN OUT PMM_STATISTICS MemoryStatistics
    )
/*++

Routine Description:

    This routine returns various statistics about the state of the memory
    manager subsystem.

Arguments:

    MemoryStatistics - Supplies the buffer to fill with the memory manager's
        statistics.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    KIRQL OldIrql;

    if (MemoryStatistics->Length == sizeof(MM_STATISTICS)) {

        MI_LOCK_MM(&OldIrql);

        MemoryStatistics->TotalPhysicalPages = MmNumberOfPhysicalPages;
        MemoryStatistics->AvailablePages = MmAvailablePages;
        MemoryStatistics->VirtualMemoryBytesCommitted =
            ((MmAllocatedPagesByUsage[MmVirtualMemoryUsage] +
            MmAllocatedPagesByUsage[MmImageUsage]) << PAGE_SHIFT);
        MemoryStatistics->VirtualMemoryBytesReserved = MmVirtualMemoryBytesReserved;
        MemoryStatistics->CachePagesCommitted = MmAllocatedPagesByUsage[MmFsCacheUsage];
        MemoryStatistics->PoolPagesCommitted = MmAllocatedPagesByUsage[MmPoolUsage];
        MemoryStatistics->StackPagesCommitted = MmAllocatedPagesByUsage[MmStackUsage];
        MemoryStatistics->ImagePagesCommitted = MmAllocatedPagesByUsage[MmImageUsage];

        MI_UNLOCK_MM(OldIrql);

        status = STATUS_SUCCESS;

    } else {
        status = STATUS_INVALID_PARAMETER;
    }

    return status;
}

PVOID
MmAllocateSystemMemory(
    IN SIZE_T NumberOfBytes,
    IN ULONG Protect
    )
/*++

Routine Description:

    This function allocates a range of memory in the non-paged portion of the
    system address space.

    This routine is designed to be used by a driver's initialization
    routine to allocate a noncached block of virtual memory for various device
    specific buffers.

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
    return MiAllocateMappedMemory(&MmSystemPteRange, MmSystemMemoryUsage,
        Protect, NumberOfBytes, MiRemoveAnyPage, FALSE);
}

ULONG
MmFreeSystemMemory(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes
    )
/*++

Routine Description:

    This function deallocates a range of memory in the non-paged portion of the
    system address space.  This is designed to free pages acquired via
    MmAllocateSystemMemory only.

Arguments:

    BaseAddress - Supplies the base virtual address where the system memory
                  resides.

    NumberOfBytes - Supplies the number of bytes allocated to the request.
                    This must be the same number that was obtained with
                    the MmAllocateSystemMemory call.

Return Value:

    Returns the number of pages deallocated.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    return MiFreeMappedMemory(&MmSystemPteRange, BaseAddress, NumberOfBytes);
}

PVOID
MmAllocatePoolPages(
    IN SIZE_T NumberOfBytes
    )
/*++

Routine Description:

    This function allocates a set of pages from the specified pool
    and returns the starting virtual address to the caller.

Arguments:

    NumberOfBytes - Supplies the size of the request in bytes.  The actual
                    size returned is rounded up to a page boundary.

Return Value:

    Returns a pointer to the allocated pool, or NULL if no more pool is
    available.

Environment:

    These functions are used by the general pool allocation routines
    and should not be called directly.

    Mutexes guarding the pool databases must be held when calling
    these functions.

    Kernel mode, IRQL at DISPATCH_LEVEL.

--*/
{
    PVOID StartingAddress;

    StartingAddress = MiAllocateMappedMemory(&MmSystemPteRange, MmPoolUsage,
        PAGE_READWRITE, NumberOfBytes, MiRemoveAnyPage, FALSE);

#if DBG
    if (StartingAddress != NULL) {
        RtlFillMemoryUlong(StartingAddress, NumberOfBytes, 'looP');
    }
#endif

    return StartingAddress;
}

ULONG
MmFreePoolPages(
    IN PVOID StartingAddress
    )
/*++

Routine Description:

    This function returns a set of pages back to the pool from
    which they were obtained.  Once the pages have been deallocated
    the region provided by the allocation becomes available for
    allocation to other callers, i.e. any data in the region is now
    trashed and cannot be referenced.

Arguments:

    StartingAddress - Supplies the starting address which was returned
                      in a previous call to MmAllocatePoolPages.

Return Value:

    Returns the number of pages deallocated.

Environment:

    These functions are used by the general pool allocation routines
    and should not be called directly.

    Mutexes guarding the pool databases must be held when calling
    these functions.

--*/
{
#if DBG
    //
    // The number of pages may exceed one for a large pool allocation, but we'll
    // do the simple thing and just assume a single page here.
    //

    RtlFillMemoryUlong(StartingAddress, PAGE_SIZE, 'daeD');
#endif

    return MiFreeMappedMemory(&MmSystemPteRange, StartingAddress, 0);
}

PVOID
MmCreateKernelStack(
    IN SIZE_T NumberOfBytes,
    IN BOOLEAN DebuggerThread
    )
/*++

Routine Description:

    This routine allocates a kernel stack and a no-access page within
    the non-pagable portion of the system address space.

Arguments:

    NumberOfBytes - Supplies the size of the request in bytes.  The actual
                    size returned is rounded up to a page boundary.

    DebuggerThread - Supplies TRUE if this thread's stack should be allocated
                     from the extra memory available on a development kit or
                     from the retail memory region.

Return Value:

    Returns a pointer to the base of the kernel stack.  Note, that the
    base address points to the guard page, so space must be allocated
    on the stack before accessing the stack.

    If a kernel stack cannot be created, the value NULL is returned.

Environment:

    Kernel mode.  APCs Disabled.

--*/
{
    PMMPTERANGE PteRange;
    PMMREMOVE_PAGE_ROUTINE RemovePageRoutine;
    MMPFN_BUSY_TYPE BusyType;
    PVOID KernelStackBottom;
    SIZE_T ActualNumberOfBytes;

    ASSERT(NumberOfBytes != 0);
    ASSERT((NumberOfBytes & (PAGE_SIZE - 1)) == 0);

    //
    // Determine which PTE address range to allocate the stack from.
    //

    PteRange = &MmSystemPteRange;
    RemovePageRoutine = MiRemoveAnyPage;
    BusyType = MmStackUsage;

#ifdef DEVKIT
    if (DebuggerThread) {
        PteRange = &MmDeveloperKitPteRange;
        RemovePageRoutine = MmDeveloperKitPteRange.RemovePageRoutine;
        BusyType = MmDebuggerUsage;
    }
#endif

    //
    // Allocate the stack.  Note that an extra barrier page will be added to the
    // allocation.
    //

    KernelStackBottom = MiAllocateMappedMemory(PteRange, BusyType,
        PAGE_READWRITE, NumberOfBytes, RemovePageRoutine, TRUE);
    ActualNumberOfBytes = NumberOfBytes + PAGE_SIZE;

    if (KernelStackBottom == NULL) {
        return NULL;
    }

#if DBG
    RtlFillMemoryUlong((PCHAR)KernelStackBottom + PAGE_SIZE, NumberOfBytes,
        'katS');
#endif

    return (PCHAR)KernelStackBottom + ActualNumberOfBytes;
}

VOID
MmDeleteKernelStack(
    IN PVOID KernelStackBase,
    IN PVOID KernelStackLimit
    )
/*++

Routine Description:

    This routine deletes a kernel stack and the no-access page within
    the non-pagable portion of the system address space.

Arguments:

    KernelStackBase - Supplies a pointer to the base of the kernel stack.

    KernelStackLimit - Supplies a pointer to the limit of the kernel stack.

Return Value:

    None.

Environment:

    Kernel mode.  APCs Disabled.

--*/
{
    PMMPTERANGE PteRange;
    SIZE_T ActualNumberOfBytes;
    PVOID KernelStackBottom;

    //
    // Determine which PTE range allocated the stack.
    //

    PteRange = &MmSystemPteRange;

#ifdef DEVKIT
    if (!MI_IS_SYSTEM_PTE_ADDRESS(KernelStackLimit)) {
        PteRange = &MmDeveloperKitPteRange;
    }
#endif

    //
    // Free the stack.  Note that an extra barrier page had been added to the
    // allocation.
    //

    ActualNumberOfBytes = ((PCHAR)KernelStackBase - (PCHAR)KernelStackLimit) +
        PAGE_SIZE;

    ASSERT((ActualNumberOfBytes & (PAGE_SIZE - 1)) == 0);

    KernelStackBottom = (PCHAR)KernelStackBase - ActualNumberOfBytes;

    MiFreeMappedMemory(PteRange, KernelStackBottom, ActualNumberOfBytes);
}
