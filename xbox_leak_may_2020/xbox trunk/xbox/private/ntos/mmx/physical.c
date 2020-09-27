/*++

Copyright (c) 1989-2002  Microsoft Corporation

Module Name:

    physical.c

Abstract:

    This module implements the kernel physical memory management services.

--*/

#include "mi.h"

//
// Number of bytes that have been reserved for instance memory allocation.
//
ULONG MmNumberOfInstanceMemoryBytes = MM_INSTANCE_PAGE_COUNT << PAGE_SHIFT;

ULONG_PTR
MmGetPhysicalAddress(
    IN PVOID BaseAddress
    )
/*++

Routine Description:

    This function returns the corresponding physical address for a
    valid virtual address.

Arguments:

    BaseAddress - Supplies the virtual address for which to return the
                  physical address.

Return Value:

    Returns the corresponding physical address.

Environment:

    Kernel mode.  Any IRQL level.

--*/
{
    ULONG_PTR PhysicalAddress;
    PMMPTE PointerPte;
#if DBG
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;
#endif

    PointerPte = MiGetPdeAddress(BaseAddress);
    if (PointerPte->Hard.Valid == 0) {
        goto InvalidAddress;
    }

    if (PointerPte->Hard.LargePage == 0) {

        PointerPte = MiGetPteAddress(BaseAddress);
        if (PointerPte->Hard.Valid == 0) {
            goto InvalidAddress;
        }

        PhysicalAddress = BYTE_OFFSET(BaseAddress);

    } else {

        PhysicalAddress = BYTE_OFFSET_LARGE(BaseAddress);
    }

    PhysicalAddress += (PointerPte->Hard.PageFrameNumber << PAGE_SHIFT);

#if DBG
    //
    // Verify that the base address is either a physically mapped page (either a
    // contiguous memory allocation or part of XBOXKRNL.EXE) or that it's I/O
    // lock count is non-zero (a page that's been liked with a service like
    // MmLockUnlockBufferPages).
    //

    PageFrameNumber = PhysicalAddress >> PAGE_SHIFT;

    if (PageFrameNumber <= MM_HIGHEST_PHYSICAL_PAGE) {

        PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

        if (PageFrame->Pte.Hard.Valid == 0) {
            ASSERT(PageFrame->Busy.LockCount != 0);
        }
    }
#endif

    return PhysicalAddress;

InvalidAddress:
    MiDbgPrint(("MmGetPhysicalAddress failed, base address was %p", BaseAddress));
    return 0;
}

PVOID
MmAllocateContiguousMemory(
    IN SIZE_T NumberOfBytes
    )
/*++

Routine Description:

    This function allocates a range of physically contiguous non-paged pool.

    This routine is designed to be used by a driver's initialization
    routine to allocate a contiguous block of physical memory for
    issuing DMA requests from.

Arguments:

    NumberOfBytes - Supplies the number of bytes to allocate.

Return Value:

    NULL - a contiguous range could not be found to satisfy the request.

    NON-NULL - Returns a pointer (virtual address in the nonpaged portion
               of the system) to the allocated physically contiguous
               memory.

Environment:

    Kernel mode, IRQL of APC_LEVEL or below.

--*/
{
    return MmAllocateContiguousMemoryEx(NumberOfBytes, 0, MAXULONG_PTR, 0,
        PAGE_READWRITE);
}

PVOID
MmAllocateContiguousMemoryEx(
    IN SIZE_T NumberOfBytes,
    IN ULONG_PTR LowestAcceptableAddress,
    IN ULONG_PTR HighestAcceptableAddress,
    IN ULONG_PTR Alignment,
    IN ULONG Protect
    )
/*++

Routine Description:

    This function allocates a range of physically contiguous non-cached,
    non-paged memory.  This is accomplished by using MmAllocateContiguousMemory
    which uses nonpaged pool virtual addresses to map the found memory chunk.

    Then this function establishes another map to the same physical addresses,
    but this alternate map is initialized as non-cached.  All references by
    our caller will be done through this alternate map.

    This routine is designed to be used by a driver's initialization
    routine to allocate a contiguous block of noncached physical memory for
    things like the AGP GART.

Arguments:

    NumberOfBytes - Supplies the number of bytes to allocate.

    LowestAcceptableAddress - Supplies the lowest physical address
                              which is valid for the allocation.  For
                              example, if the device can only reference
                              physical memory in the 8M to 16MB range, this
                              value would be set to 0x800000 (8Mb).

    HighestAcceptableAddress - Supplies the highest physical address
                               which is valid for the allocation.  For
                               example, if the device can only reference
                               physical memory below 16MB, this
                               value would be set to 0xFFFFFF (16Mb - 1).

    Alignment - Supplies the desired page alignment for the allocation.  The
                alignment is treated as a power of two.  The minimum alignment
                is PAGE_SIZE.

    Protect - Supplies the type of protection and cache mapping to use for the
              allocation.

Return Value:

    NULL - a contiguous range could not be found to satisfy the request.

    NON-NULL - Returns a pointer (virtual address in the nonpaged portion
               of the system) to the allocated physically contiguous
               memory.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    ULONG PhysicalAperture;
    MMPTE TempPte;
    PFN_NUMBER LowestAcceptablePageFrameNumber;
    PFN_NUMBER HighestAcceptablePageFrameNumber;
    PFN_COUNT PfnAlignment;
    PFN_COUNT PfnAlignmentMask;
    PFN_COUNT NumberOfPages;
    PFN_COUNT PfnAlignmentSubtraction;
    KIRQL OldIrql;
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;
    PFN_COUNT ContiguousCandidatePagesFound;
    PFN_NUMBER EndingPageFrameNumber;
    PFN_NUMBER PageFrameNumberToGrab;

    ASSERT(NumberOfBytes != 0);

    //
    // Determine which system memory aperature to use.  If this is a video
    // memory request, use the write-combined memory aperture, otherwise use the
    // standard memory aperture.
    //

    if (Protect & PAGE_OLD_VIDEO) {
        PhysicalAperture = MM_WRITE_COMBINE_APERTURE;
        Protect = (Protect & ~PAGE_OLD_VIDEO);
    } else {
        PhysicalAperture = 0;
    }

    //
    // Convert the protect code to a PTE mask.
    //

    if (!MiMakeSystemPteProtectionMask(Protect, &TempPte)) {
        return NULL;
    }

    //
    // Convert the supplied physical addresses into page frame numbers.
    //

    LowestAcceptablePageFrameNumber =
        (PFN_NUMBER)(LowestAcceptableAddress >> PAGE_SHIFT);
    HighestAcceptablePageFrameNumber =
        (PFN_NUMBER)(HighestAcceptableAddress >> PAGE_SHIFT);

    if (HighestAcceptablePageFrameNumber > MM_CONTIGUOUS_MEMORY_LIMIT) {
        HighestAcceptablePageFrameNumber = MM_CONTIGUOUS_MEMORY_LIMIT;
    }

    if (LowestAcceptablePageFrameNumber > HighestAcceptablePageFrameNumber) {
        LowestAcceptablePageFrameNumber = HighestAcceptablePageFrameNumber;
    }

    //
    // Compute the alignment of the allocation in terms of pages.  The alignment
    // should be a power of two.
    //

    ASSERT((Alignment & (Alignment - 1)) == 0);

    PfnAlignment = (PFN_COUNT)(Alignment >> PAGE_SHIFT);

    if (PfnAlignment == 0) {
        PfnAlignment = 1;
    }

    //
    // Compute the alignment mask to round a page frame number down to the
    // nearest alignment boundary.
    //

    PfnAlignmentMask = ~(PfnAlignment - 1);

    //
    // Compute the number of pages to allocate.
    //

    NumberOfPages = BYTES_TO_PAGES(NumberOfBytes);

    //
    // Compute the number of pages to subtract from an aligned page frame number
    // to get to the prior candidate ending page frame number.
    //

    PfnAlignmentSubtraction = ((NumberOfPages + PfnAlignment - 1) &
        PfnAlignmentMask) - NumberOfPages + 1;

    //
    // Now ensure that we can allocate the required number of pages.
    //

    MI_LOCK_MM(&OldIrql);

    if (MmAvailablePages < NumberOfPages) {
        MI_UNLOCK_MM(OldIrql);
        return NULL;
    }

    //
    // Search the page frame database for a range that satisfies the size and
    // alignment requirements.
    //

    PageFrameNumber = HighestAcceptablePageFrameNumber + 1;

InvalidCandidatePageFound:
    PageFrameNumber = (PageFrameNumber & PfnAlignmentMask) -
        PfnAlignmentSubtraction;
    ContiguousCandidatePagesFound = 0;

    while ((SPFN_NUMBER)PageFrameNumber >= (SPFN_NUMBER)LowestAcceptablePageFrameNumber) {

        PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

        //
        // If we have a page frame that's already being used for a physical
        // mapping, then this is an invalid candidate page.
        //

        if (PageFrame->Pte.Hard.Valid != 0) {
            goto InvalidCandidatePageFound;
        }

        //
        // If we have a page frame that's busy and is locked for I/O, then we
        // can't relocate the page, so this is an invalid candidate page.
        //

        if ((PageFrame->Busy.Busy != 0) && (PageFrame->Busy.LockCount != 0)) {
            goto InvalidCandidatePageFound;
        }

        //
        // This page can be used to help satisfy the request.  If we haven't
        // found the required number of physical pages yet, then continue the
        // search.
        //

        ContiguousCandidatePagesFound++;

        if (ContiguousCandidatePagesFound < NumberOfPages) {
            PageFrameNumber--;
            continue;
        }

        //
        // Verify that the starting page frame number is correctly aligned.
        //

        ASSERT((PageFrameNumber & (PfnAlignment - 1)) == 0);

        //
        // We found a range of physical pages of the requested size.
        //

        EndingPageFrameNumber = PageFrameNumber + NumberOfPages - 1;

        //
        // First, allocate all of the free pages in the range so that any
        // relocations we do won't go into our target range.
        //

        for (PageFrameNumberToGrab = PageFrameNumber;
            PageFrameNumberToGrab <= EndingPageFrameNumber;
            PageFrameNumberToGrab++) {

            PageFrame = MI_PFN_ELEMENT(PageFrameNumberToGrab);

            if (PageFrame->Busy.Busy == 0) {

                //
                // Detach the page from the free list.
                //

                MiRemovePageFromFreeList(PageFrameNumberToGrab);

                //
                // Convert the page frame to a physically mapped page.
                //

                TempPte.Hard.PageFrameNumber = PageFrameNumberToGrab +
                    PhysicalAperture;
                MI_WRITE_PTE(&PageFrame->Pte, TempPte);

                //
                // Increment the number of physically mapped pages.
                //

                MmAllocatedPagesByUsage[MmContiguousUsage]++;
            }
        }

        //
        // Second, relocate any non-pinned pages in the range.  The above loop
        // will allocate physically mapped pages and there won't be any pinned
        // pages already existing in the range due to the above candidate page
        // checks.
        //

        for (PageFrameNumberToGrab = PageFrameNumber;
            PageFrameNumberToGrab <= EndingPageFrameNumber;
            PageFrameNumberToGrab++) {

            PageFrame = MI_PFN_ELEMENT(PageFrameNumberToGrab);

            if (PageFrame->Pte.Hard.Valid == 0) {

                //
                // Relocate the page.
                //

                MiRelocateBusyPage(PageFrameNumberToGrab);

                //
                // Convert the page frame to a physically mapped page.
                //

                TempPte.Hard.PageFrameNumber = PageFrameNumberToGrab +
                    PhysicalAperture;
                MI_WRITE_PTE(&PageFrame->Pte, TempPte);

                //
                // Increment the number of physically mapped pages.
                //

                MmAllocatedPagesByUsage[MmContiguousUsage]++;
            }
        }

        //
        // Mark the last page of the allocation with a flag so that we
        // can later determine the size of this allocation.
        //

        MI_PFN_ELEMENT(EndingPageFrameNumber)->Pte.Hard.GuardOrEndOfAllocation = 1;

        //
        // Write combined accesses may not check the processor's cache, so force
        // a flush of the TLB and cache now to ensure coherency.
        //
        // Flush the cache for uncached allocations so that all cache lines from
        // the page are out of the processor's caches.  The pages are likely to
        // be shared with an external device and the external device may not
        // snoop cache lines.
        //

        if (Protect & (PAGE_WRITECOMBINE | PAGE_NOCACHE)) {
            KeFlushCurrentTbAndInvalidateAllCaches();
        }

        MI_UNLOCK_MM(OldIrql);

        return MI_CONVERT_PFN_TO_PHYSICAL(PageFrameNumber);
    }

    MI_UNLOCK_MM(OldIrql);

    return NULL;
}

VOID
MmFreeContiguousMemory(
    IN PVOID BaseAddress
    )
/*++

Routine Description:

    This function deallocates a range of physically contiguous non-paged
    pool which was allocated with the MmAllocateContiguousMemory function.

Arguments:

    BaseAddress - Supplies the base virtual address where the physical
                  address was previously mapped.

Return Value:

    None.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    KIRQL OldIrql;
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;
    ULONG EndOfAllocation;

    ASSERT(MI_IS_PHYSICAL_ADDRESS(BaseAddress));

    //
    // Release each page in the buffer while looking for the end of allocation
    // marker bit in the PTE.
    //

    MI_LOCK_MM(&OldIrql);

    PageFrameNumber = MI_CONVERT_PHYSICAL_TO_PFN(BaseAddress);

    do {

        PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

        ASSERT(PageFrame->Pte.Hard.Valid != 0);

        EndOfAllocation = MI_PFN_ELEMENT(PageFrameNumber)->Pte.Hard.GuardOrEndOfAllocation;

        //
        // Insert the page at the tail of the free lists so that we keep this
        // range of contiguous pages free as long as possible in order to
        // satisfy another contiguous memory allocation request.
        //

        MiInsertPageInFreeList(PageFrameNumber, FALSE);

        //
        // Flush the system RAM physical mapping.
        //

        MI_FLUSH_VA(MI_CONVERT_PFN_TO_PHYSICAL(PageFrameNumber));

        //
        // Decrement the number of physically mapped pages.
        //

        MmAllocatedPagesByUsage[MmContiguousUsage]--;

        PageFrameNumber++;

    } while (!EndOfAllocation);

    MI_UNLOCK_MM(OldIrql);
}

SIZE_T
MmQueryAllocationSize(
    IN PVOID BaseAddress
    )
/*++

Routine Description:

    This function attempts to query the number of bytes allocated to a range of
    memory.  This is designed to work with memory allocated by
    MmAllocateContiguousMemory or MmAllocateSystemMemory.

Arguments:

    BaseAddress - Supplies the base virtual address where the system memory
                  resides.

Return Value:

    Number of bytes allocated to the base address.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    KIRQL OldIrql;
    PFN_COUNT NumberOfPages;
    PMMPTE PointerPte;

    MI_LOCK_MM(&OldIrql);

    //
    // Start with one page for the page of the base address.  Loop over the PTEs
    // of the buffer looking for the end of allocation marker.
    //

    NumberOfPages = 1;
    PointerPte = MiGetPteAddress(BaseAddress);

    while (PointerPte->Hard.GuardOrEndOfAllocation == 0) {

        ASSERT(PointerPte->Hard.Valid != 0);

        NumberOfPages++;
        PointerPte++;
    }

    MI_UNLOCK_MM(OldIrql);

    return NumberOfPages << PAGE_SHIFT;
}

VOID
MmPersistContiguousMemory(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes,
    IN BOOLEAN Persist
    )
/*++

Routine Description:

    This function marks a contiguous memory allocation as needing to be
    preserved across a quick reboot.

Arguments:

    BaseAddress - Supplies the base virtual address where the system memory
                  resides.

    NumberOfBytes - Supplies the number of bytes to be preserved.

    Persist - Supplies TRUE if the memory should be persisted, else FALSE.

Return Value:

    None.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    KIRQL OldIrql;
    PMMPTE PointerPte;
    PMMPTE EndingPte;

    ASSERT(MI_IS_PHYSICAL_ADDRESS(BaseAddress));
    ASSERT(NumberOfBytes != 0);

    MI_LOCK_MM(&OldIrql);

    PointerPte = MiGetPteAddress(BaseAddress);
    EndingPte = MiGetPteAddress((PCHAR)BaseAddress + NumberOfBytes - 1);
    Persist = Persist ? TRUE : FALSE;

    while (PointerPte <= EndingPte) {

        ASSERT(PointerPte->Hard.Valid != 0);

        PointerPte->Hard.PersistAllocation = Persist;
        PointerPte++;
    }

    MI_UNLOCK_MM(OldIrql);
}

PVOID
MmMapIoSpace(
    IN ULONG_PTR PhysicalAddress,
    IN SIZE_T NumberOfBytes,
    IN ULONG Protect
    )
/*++

Routine Description:

    This function maps the specified physical address into the non-pagable
    portion of the system address space.

Arguments:

    PhysicalAddress - Supplies the starting physical address to map.

    NumberOfBytes - Supplies the number of bytes to map.

    Protect - Supplies the type of protection and cache mapping to use for the
              allocation.

Return Value:

    Returns the virtual address which maps the specified physical addresses.
    The value NULL is returned if sufficient virtual address space for
    the mapping could not be found.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    MMPTE TempPte;
    PFN_COUNT NumberOfPtes;
    PMMPTE StartingPte;
    PMMPTE PointerPte;
    PFN_NUMBER PageFrameNumber;
    PMMPTE EndingPte;
    PVOID BaseAddress;

    ASSERT (NumberOfBytes != 0);

    //
    // Convert the protect code to a PTE mask.
    //

    if (!MiMakeSystemPteProtectionMask(Protect, &TempPte)) {
        return NULL;
    }

    //
    // If the mapping is for device memory, then use one of the physical device
    // mappings if we recoginze the cache type.  These device mappings are
    // identity mapped, so the base address is simply the physical address.
    //

    if ((Protect == (PAGE_READWRITE | PAGE_NOCACHE)) &&
        (PhysicalAddress >= MM_DEVICE_UC_BASE) &&
        (PhysicalAddress + NumberOfBytes <= MM_DEVICE_UC_END)) {
        BaseAddress = (PVOID)PhysicalAddress;
        return BaseAddress;
    }

    if ((Protect == (PAGE_READWRITE | PAGE_WRITECOMBINE)) &&
        (PhysicalAddress >= MM_DEVICE_WC_BASE) &&
        (PhysicalAddress + NumberOfBytes <= MM_DEVICE_WC_END)) {
        BaseAddress = (PVOID)PhysicalAddress;
        goto ReturnWriteCombinedAddress;
    }

    //
    // Reserve the PTEs for the mapped memory.
    //

    NumberOfPtes = COMPUTE_PAGES_SPANNED(PhysicalAddress, NumberOfBytes);

    StartingPte = MiReserveSystemPtes(&MmSystemPteRange, NumberOfPtes);
    if (StartingPte == NULL) {
        return NULL;
    }

    //
    // Fill in the PTEs.
    //

    PageFrameNumber = (PFN_NUMBER)(PhysicalAddress >> PAGE_SHIFT);
    EndingPte = StartingPte + NumberOfPtes;

    for (PointerPte = StartingPte; PointerPte < EndingPte; PointerPte++,
        PageFrameNumber++) {
        TempPte.Hard.PageFrameNumber = PageFrameNumber;
        MI_WRITE_PTE(PointerPte, TempPte);
    }

    //
    // Compute the offset into the mapped region for the requested address.
    //

    BaseAddress = (PCHAR)MiGetVirtualAddressMappedByPte(StartingPte) +
        BYTE_OFFSET(PhysicalAddress);

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
ReturnWriteCombinedAddress:
        KeFlushCurrentTbAndInvalidateAllCaches();
    }

    return BaseAddress;
}

VOID
MmUnmapIoSpace(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes
    )
/*++

Routine Description:

    This function unmaps a range of physical address which were previously
    mapped via an MmMapIoSpace function call.

Arguments:

    BaseAddress - Supplies the base virtual address where the physical
                  address was previously mapped.

    NumberOfBytes - Supplies the number of bytes which were mapped.

Return Value:

    None.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    PFN_COUNT NumberOfPtes;
    PMMPTE StartingPte;

    ASSERT(NumberOfBytes != 0);

    if (MI_IS_SYSTEM_PTE_ADDRESS(BaseAddress)) {

        //
        // The physical address was not mapped into a physical mapping window.
        // Free the PTEs that were allocated to map the memory.
        //

        NumberOfPtes = COMPUTE_PAGES_SPANNED(BaseAddress, NumberOfBytes);
        StartingPte = MiGetPteAddress(BaseAddress);

        MiZeroAndFlushPtes(StartingPte, NumberOfPtes);
        MiReleaseSystemPtes(&MmSystemPteRange, StartingPte, NumberOfPtes);
    }
}

VOID
MmLockUnlockBufferPages(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes,
    IN BOOLEAN UnlockPages
    )
/*++

Routine Description:

    This function locks or unlocks the physical pages backing the specified
    buffer.  The routine may be called multiple times for a given buffer and
    buffer ranges may overlap.  Every lock call must be matched by an unlock
    call.

Arguments:

    BaseAddress - Supplies the base virtual address to lock or unlock.

    NumberOfBytes - Supplies the number of bytes to lock or unlock.

    UnlockPages - If TRUE, the pages are unlocked, else the pages are locked.

Return Value:

    None.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    KIRQL OldIrql;
    ULONG LockCountAdjustment;
    PFN_NUMBER PageFrameNumber;
    PMMPTE PointerPte;
    PMMPTE EndingPte;
    PMMPFN PageFrame;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);
    ASSERT(NumberOfBytes != 0);

    MI_LOCK_MM(&OldIrql);

    ASSERT(MiGetPdeAddress(BaseAddress)->Hard.Valid != 0);

    //
    // If this is a physically mapped address, then there's nothing to lock or
    // unlock.  Locking is used to prevent virtually mapped pages from being
    // relocated while a device is accessing the page.
    //
    // If this is an address backed by a large page, then there's nothing to
    // lock or unlock.  Large pages are used for memory mapped I/O.
    //

    if (!MI_IS_PHYSICAL_ADDRESS(BaseAddress) &&
        (MiGetPdeAddress(BaseAddress)->Hard.LargePage == 0)) {

        LockCountAdjustment = UnlockPages ? (-MI_LOCK_COUNT_UNIT) :
            MI_LOCK_COUNT_UNIT;

        PointerPte = MiGetPteAddress(BaseAddress);
        EndingPte = MiGetPteAddress((PCHAR)BaseAddress + NumberOfBytes - 1);

        do {

            ASSERT(PointerPte->Hard.Valid != 0);

            PageFrameNumber = PointerPte->Hard.PageFrameNumber;

            if (PageFrameNumber <= MM_HIGHEST_PHYSICAL_PAGE) {

                PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

                ASSERT(PageFrame->Pte.Hard.Valid == 0);
                ASSERT(PageFrame->Busy.Busy != 0);
                ASSERT(PageFrame->Busy.BusyType != MmUnknownUsage);
                ASSERT(PageFrame->Busy.BusyType < MmMaximumUsage);

                if (UnlockPages) {
                    ASSERT(PageFrame->Busy.LockCount != 0);
                } else {
                    ASSERT(PageFrame->Busy.LockCount != MI_LOCK_COUNT_MAXIMUM);
                }

                PageFrame->Busy.LockCount += LockCountAdjustment;
            }

            PointerPte++;

        } while (PointerPte <= EndingPte);
    }

    MI_UNLOCK_MM(OldIrql);
}

VOID
MmLockUnlockPhysicalPage(
    IN ULONG_PTR PhysicalAddress,
    IN BOOLEAN UnlockPage
    )
/*++

Routine Description:

    This function locks or unlocks the specified physical page.

Arguments:

    BaseAddress - Supplies the physical page to lock or unlock.

    UnlockPage - If TRUE, the page is unlocked, else the page is locked.

Return Value:

    None.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    KIRQL OldIrql;
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    MI_LOCK_MM(&OldIrql);

    PageFrameNumber = PhysicalAddress >> PAGE_SHIFT;

    //
    // If this is a physically mapped page or a memory mapped I/O page, then
    // there's nothing to lock or unlock.  Physically mapped pages are by
    // definition always locked and memory mapped I/O addresses are not managed
    // by this code.
    //

    if ((MI_PFN_ELEMENT(PageFrameNumber)->Pte.Hard.Valid == 0) &&
        (PageFrameNumber <= MM_HIGHEST_PHYSICAL_PAGE)) {

        PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

        ASSERT(PageFrame->Pte.Hard.Valid == 0);
        ASSERT(PageFrame->Busy.Busy != 0);
        ASSERT(PageFrame->Busy.BusyType != MmUnknownUsage);
        ASSERT(PageFrame->Busy.BusyType < MmMaximumUsage);

        if (UnlockPage) {
            ASSERT(PageFrame->Busy.LockCount != 0);
            PageFrame->Busy.LockCount -= MI_LOCK_COUNT_UNIT;
        } else {
            ASSERT(PageFrame->Busy.LockCount != MI_LOCK_COUNT_MAXIMUM);
            PageFrame->Busy.LockCount += MI_LOCK_COUNT_UNIT;
        }
    }

    MI_UNLOCK_MM(OldIrql);
}

NTSTATUS
MmLockSelectedIoPages(
    IN PFILE_SEGMENT_ELEMENT SegmentArray,
    IN ULONG Length,
    IN OUT PIRP Irp,
    IN BOOLEAN MapUserBuffer
    )
/*++

Routine Description:

    This function locks the pages described by the supplied file segment element
    array.  A copy of the array is placed in the supplied I/O request packet.
    Optionally, the file segment element array is mapped to a virtual buffer in
    system space for target devices that do not support scatter/gather I/O.

Arguments:

    SegmentArray - Supplies the list of file segments to be locked.

    Length - Supplies the number of bytes contained in the segment array.

    Irp - Supplies a pointer to the I/O request packet to receive a copy of the
        segment array as well as the optional virtual mapping.

    MapUserBuffer - Supplies whether or not to map a virtual user buffer for the
        segment array.

Return Value:

    Status of operation.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    PFN_COUNT NumberOfPages;
    PFILE_SEGMENT_ELEMENT SegmentArrayClone;
    PMMPTE MappingPte;
    KIRQL OldIrql;
    PFILE_SEGMENT_ELEMENT EndingSegmentArray;
    PVOID BufferAddress;
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;
    MMPTE TempPte;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);
    ASSERT(Length != 0);

    NumberOfPages = ROUND_TO_PAGES(Length) >> PAGE_SHIFT;

    //
    // Allocate the array to hold the copy of the segment array and perform the
    // copy.
    //

    SegmentArrayClone = (PFILE_SEGMENT_ELEMENT)ExAllocatePoolWithTag(
        sizeof(FILE_SEGMENT_ELEMENT) * NumberOfPages, 'aSmM');

    if (SegmentArrayClone == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(SegmentArrayClone, SegmentArray,
        sizeof(FILE_SEGMENT_ELEMENT) * NumberOfPages);

    //
    // If we need to map a virtual buffer for the segment array, then reserve
    // the PTEs now.
    //

    if (MapUserBuffer) {

        MappingPte = MiReserveSystemPtes(&MmSystemPteRange, NumberOfPages);

        if (MappingPte == NULL) {
            ExFreePool(SegmentArrayClone);
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        //
        // Fill the I/O request packet with the mapped user buffer and set a
        // flag so that MmUnlockSelectedIoPages knows to release the mapping.
        //

        Irp->UserBuffer = MiGetVirtualAddressMappedByPte(MappingPte);
        Irp->Flags |= IRP_UNMAP_SEGMENT_ARRAY;

    } else {
        MappingPte = NULL;
    }

    //
    // Loop over the segment array and lock down each of the page buffers.
    //

    MI_LOCK_MM(&OldIrql);

    EndingSegmentArray = SegmentArray + NumberOfPages;

    while (SegmentArray < EndingSegmentArray) {

        BufferAddress = SegmentArray->Buffer;

        ASSERT(MiGetPdeAddress(BufferAddress)->Hard.Valid != 0);
        ASSERT(MiGetPdeAddress(BufferAddress)->Hard.LargePage == 0);

        if (!MI_IS_PHYSICAL_ADDRESS(BufferAddress)) {

            ASSERT(MiGetPteAddress(BufferAddress)->Hard.Valid != 0);

            PageFrameNumber = MiGetPteAddress(BufferAddress)->Hard.PageFrameNumber;

            if (PageFrameNumber <= MM_HIGHEST_PHYSICAL_PAGE) {

                PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

                ASSERT(PageFrame->Pte.Hard.Valid == 0);
                ASSERT(PageFrame->Busy.Busy != 0);
                ASSERT(PageFrame->Busy.BusyType != MmUnknownUsage);
                ASSERT(PageFrame->Busy.BusyType < MmMaximumUsage);
                ASSERT(PageFrame->Busy.LockCount != MI_LOCK_COUNT_MAXIMUM);

                PageFrame->Busy.LockCount += MI_LOCK_COUNT_UNIT;
            }

        } else {
            PageFrameNumber = MI_CONVERT_PHYSICAL_TO_PFN(BufferAddress);
        }

        //
        // If we need to map a virtual buffer for the segment array, then fill
        // in the next mapping PTE.
        //

        if (MappingPte != NULL) {
            TempPte.Long = MiGetValidKernelPteBits();
            TempPte.Hard.PageFrameNumber = PageFrameNumber;
            MI_WRITE_PTE(MappingPte, TempPte);
            MappingPte++;
        }

        SegmentArray++;
    }

    MI_UNLOCK_MM(OldIrql);

    //
    // Fill the I/O request packet with the copy of the segment array and the
    // number of bytes that are locked down.  These are used by
    // MmUnlockSelectedIoPages.
    //

    Irp->SegmentArray = SegmentArrayClone;
    Irp->LockedBufferLength = Length;

    return STATUS_SUCCESS;
}

VOID
MmUnlockSelectedIoPages(
    IN PIRP Irp
    )
/*++

Routine Description:

    This function unlocks the pages described by the file segment element
    pointed to by the supplied I/O request packet.  Optionally, the virtual
    mapping created by MmLockSelectedPages is unmapped.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    None.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    PFN_COUNT NumberOfPages;
    PMMPTE MappingPte;
    KIRQL OldIrql;
    PFILE_SEGMENT_ELEMENT SegmentArray;
    PFILE_SEGMENT_ELEMENT EndingSegmentArray;
    PVOID BufferAddress;
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);
    ASSERT(Irp->SegmentArray != NULL);
    ASSERT(Irp->LockedBufferLength != 0);

    NumberOfPages = ROUND_TO_PAGES(Irp->LockedBufferLength) >> PAGE_SHIFT;

    //
    // If we mapped a virtual buffer for the segment array, then release the
    // PTEs now.
    //

    if (Irp->Flags & IRP_UNMAP_SEGMENT_ARRAY) {

        ASSERT(Irp->UserBuffer != NULL);

        MappingPte = MiGetPteAddress(Irp->UserBuffer);

        MiZeroAndFlushPtes(MappingPte, NumberOfPages);
        MiReleaseSystemPtes(&MmSystemPteRange, MappingPte, NumberOfPages);

        Irp->UserBuffer = NULL;
        Irp->Flags &= ~IRP_UNMAP_SEGMENT_ARRAY;
    }

    //
    // Loop over the segment array and unlock each of the page buffers.
    //

    MI_LOCK_MM(&OldIrql);

    SegmentArray = Irp->SegmentArray;
    EndingSegmentArray = SegmentArray + NumberOfPages;

    while (SegmentArray < EndingSegmentArray) {

        BufferAddress = SegmentArray->Buffer;

        ASSERT(MiGetPdeAddress(BufferAddress)->Hard.Valid != 0);
        ASSERT(MiGetPdeAddress(BufferAddress)->Hard.LargePage == 0);

        if (!MI_IS_PHYSICAL_ADDRESS(BufferAddress)) {

            ASSERT(MiGetPteAddress(BufferAddress)->Hard.Valid != 0);

            PageFrameNumber = MiGetPteAddress(BufferAddress)->Hard.PageFrameNumber;

            if (PageFrameNumber <= MM_HIGHEST_PHYSICAL_PAGE) {

                PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

                ASSERT(PageFrame->Pte.Hard.Valid == 0);
                ASSERT(PageFrame->Busy.Busy != 0);
                ASSERT(PageFrame->Busy.BusyType != MmUnknownUsage);
                ASSERT(PageFrame->Busy.BusyType < MmMaximumUsage);
                ASSERT(PageFrame->Busy.LockCount != 0);

                PageFrame->Busy.LockCount -= MI_LOCK_COUNT_UNIT;
            }
        }

        SegmentArray++;
    }

    MI_UNLOCK_MM(OldIrql);

    //
    // Free the memory for the copied segment array.
    //

    ExFreePool(Irp->SegmentArray);

    Irp->SegmentArray = NULL;
}

PVOID
MmClaimGpuInstanceMemory(
    IN SIZE_T NumberOfBytes,
    OUT SIZE_T *NumberOfPaddingBytes
    )
/*++

Routine Description:

    This function returns the ending physical address and number of instance
    padding bytes for instance memory used by the NVIDIA NV2A.

Arguments:

    NumberOfBytes - Specifies the number of bytes to leave allocated for
        instance memory, or MAXULONG_PTR if no change in instance memory
        allocation should be made (used by the startup animation).

    NumberOfPaddingBytes - Specifies the location to receive the number of bytes
        that are reserved from the top of memory for other system use.

Return Value:

    Returns the virtual address of the pointer to the last byte, not inclusive,
    of the instance memory.

Environment:

    Kernel mode, <= DISPATCH_LEVEL.

--*/
{
    KIRQL OldIrql;
    PFN_NUMBER PageFrameNumber;
    PFN_NUMBER EndingPageFrameNumber;

    //
    // The number of padding bytes indicates the number of bytes that must be
    // skipped over in order to reach the first byte of the instance memory.
    //
    // For a retail console configuration, the top 64K of memory is used to
    // store the PFN database.  For a 128M configuration, we'll also reserve the
    // top 64K of memory in order to make the code simpler.
    //

#ifdef ARCADE
    *NumberOfPaddingBytes = 0;
#else
    *NumberOfPaddingBytes = MI_CONVERT_PFN_TO_PHYSICAL(MM_64M_PHYSICAL_PAGE) -
        MI_CONVERT_PFN_TO_PHYSICAL(MM_INSTANCE_PHYSICAL_PAGE + MM_INSTANCE_PAGE_COUNT);
#endif

    //
    // The startup animation code will call us with a number of bytes set to
    // MAXULONG_PTR.  At this point, don't free any of the instance memory
    // pages.
    //

    if (NumberOfBytes != MAXULONG_PTR) {

        MI_LOCK_MM(&OldIrql);

        NumberOfBytes = ROUND_TO_PAGES(NumberOfBytes);

        //
        // Verify that the requested allocation size is equal to or less than
        // the number of bytes already set aside for instance memory.
        //

        ASSERT(NumberOfBytes <= MmNumberOfInstanceMemoryBytes);

        //
        // Compute the number of pages that can be freed as instance memory and
        // made available for other use.
        //

        PageFrameNumber = MM_INSTANCE_PHYSICAL_PAGE + MM_INSTANCE_PAGE_COUNT -
            BYTES_TO_PAGES(MmNumberOfInstanceMemoryBytes);
        EndingPageFrameNumber = MM_INSTANCE_PHYSICAL_PAGE +
            MM_INSTANCE_PAGE_COUNT - BYTES_TO_PAGES(NumberOfBytes);

        while (PageFrameNumber < EndingPageFrameNumber) {

            ASSERT(MI_PFN_ELEMENT(PageFrameNumber)->Pte.Hard.Valid != 0);

            //
            // Insert the page at the head of the free lists.  We don't allow
            // contiguous memory allocations to cross into the instance memory
            // region, so these pages might as well be allocated ahead of other
            // pages that could be used for contiguous memory allocations.
            //

            MiInsertPageInFreeList(PageFrameNumber, TRUE);

            MI_FLUSH_VA(MI_CONVERT_PFN_TO_PHYSICAL(PageFrameNumber));

            MmAllocatedPagesByUsage[MmContiguousUsage]--;

#ifdef CONSOLE_DEVKIT
            //
            // On a development kit system, the NVIDIA NV2A instance memory must
            // be relative to the top of memory, so we also reserve the instance
            // memory pages in the upper half of memory.
            //

            if (MM_HIGHEST_PHYSICAL_PAGE != MM_64M_PHYSICAL_PAGE - 1) {

                ASSERT(MI_PFN_ELEMENT(MM_64M_PHYSICAL_PAGE + PageFrameNumber)->Pte.Hard.Valid != 0);

                MiInsertPageInFreeList(MM_64M_PHYSICAL_PAGE + PageFrameNumber,
                    TRUE);

                MI_FLUSH_VA(MI_CONVERT_PFN_TO_PHYSICAL(MM_64M_PHYSICAL_PAGE +
                    PageFrameNumber));

                MmAllocatedPagesByUsage[MmContiguousUsage]--;
            }
#endif

            PageFrameNumber++;
        }

        MmNumberOfInstanceMemoryBytes = NumberOfBytes;

        MI_UNLOCK_MM(OldIrql);
    }

    //
    // Return the virtual address of the non-inclusive ending byte of the
    // instance memory allocation.  This pointer is always relative to the top
    // of memory.
    //

    return (PUCHAR)MI_CONVERT_PFN_TO_PHYSICAL(MM_HIGHEST_PHYSICAL_PAGE + 1) -
        *NumberOfPaddingBytes;
}
