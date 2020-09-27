/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    virtual.c

Abstract:

    This module implements the kernel virtual memory management services.

--*/

#include "mi.h"

//
// Guards multiple threads attempting to commit, reserve, or change the page
// protections for the application virtual memory space.
//
INITIALIZED_CRITICAL_SECTION(MmAddressSpaceLock);

//
// Root pointer and hints into the virtual address descriptor tree.
//
PMMADDRESS_NODE MmVadRoot;
PMMADDRESS_NODE MmVadHint;
PMMADDRESS_NODE MmVadFreeHint;

//
// Number of bytes that have been reserved in the application virtual memory
// space.
//
SIZE_T MmVirtualMemoryBytesReserved;

NTSTATUS
NtAllocateVirtualMemory(
    IN OUT PVOID *BaseAddress,
    IN ULONG_PTR ZeroBits,
    IN OUT PSIZE_T RegionSize,
    IN ULONG AllocationType,
    IN ULONG Protect
    )
/*++

Routine Description:

    This function creates a region of pages within the virtual address
    space of a subject process.

Arguments:

    BaseAddress - Supplies a pointer to a variable that will receive
                  the base address of the allocated region of pages.
                  If the initial value of this argument is not null,
                  then the region will be allocated starting at the
                  specified virtual address rounded down to the next
                  host page size address boundary. If the initial
                  value of this argument is null, then the operating
                  system will determine where to allocate the region.

    ZeroBits - Supplies the number of high order address bits that
               must be zero in the base address of the section view. The
               value of this argument must be less than or equal to the
               maximum number of zero bits and is only used when memory
               management determines where to allocate the view (i.e. when
               BaseAddress is null).

               If ZeroBits is zero, then no zero bit constraints are applied.

               If ZeroBits is greater than 0 and less than 32, then it is
               the number of leading zero bits from bit 31.  Bits 63:32 are
               also required to be zero.  This retains compatibility
               with 32-bit systems.

               If ZeroBits is greater than 32, then it is considered as
               a mask and then number of leading zero are counted out
               in the mask.  This then becomes the zero bits argument.

    RegionSize - Supplies a pointer to a variable that will receive
                 the actual size in bytes of the allocated region
                 of pages. The initial value of this argument
                 specifies the size in bytes of the region and is
                 rounded up to the next host page size boundary.

    AllocationType - Supplies a set of flags that describe the type
                     of allocation that is to be performed for the
                     specified region of pages. Flags are:

         MEM_COMMIT - The specified region of pages is to be committed.

         MEM_RESERVE - The specified region of pages is to be reserved.

         MEM_TOP_DOWN - The specified region should be created at the
                        highest virtual address possible based on ZeroBits.

         MEM_RESET - Reset the state of the specified region so
                     that if the pages are in page paging file, they
                     are discarded and pages of zeroes are brought in.
                     If the pages are in memory and modified, they are marked
                     as not modified so they will not be written out to
                     the paging file.  The contents are NOT zeroed.

                     The Protect argument is ignored, but a valid protection
                     must be specified.

         MEM_PHYSICAL - The specified region of pages will map physical memory
                        directly via the AWE APIs.

         MEM_WRITE_WATCH - The specified private region is to be used for
                           write-watch purposes.

         MEM_NOZERO - The specified memory pages are not zero filled.

    Protect - Supplies the protection desired for the committed region of pages.

         PAGE_NOACCESS - No access to the committed region
                         of pages is allowed. An attempt to read,
                         write, or execute the committed region
                         results in an access violation.

         PAGE_EXECUTE - Execute access to the committed
                        region of pages is allowed. An attempt to
                        read or write the committed region results in
                        an access violation.

         PAGE_READONLY - Read only and execute access to the
                         committed region of pages is allowed. An
                         attempt to write the committed region results
                         in an access violation.

         PAGE_READWRITE - Read, write, and execute access to
                          the committed region of pages is allowed. If
                          write access to the underlying section is
                          allowed, then a single copy of the pages are
                          shared. Otherwise the pages are shared read
                          only/copy on write.

         PAGE_NOCACHE - The region of pages should be allocated
                        as non-cachable.

Return Value:

    Status of operation.

Environment:

    Kernel mode, PASSIVE_LEVEL.

--*/
{
    NTSTATUS status;
    ULONG PteProtectionMask;
    PVOID CapturedBase;
    SIZE_T CapturedRegionSize;
    PMMVAD Vad;
    BOOLEAN DeleteVadOnFailure;
    PCHAR TopAddress;
    PCHAR EndingAddress;
    PCHAR StartingAddress;
    PMMPTE StartingPte;
    PMMPTE EndingPte;
    PMMPTE PointerPte;
    PFN_COUNT PagesToCommit;
    PMMPFN PdePageFrame;
    PMMPTE PointerPde;
    PMMPTE NextPointerPte;
    KIRQL OldIrql;
    PMMREMOVE_PAGE_ROUTINE RemovePageRoutine;
    MMPFN_BUSY_TYPE BusyType;
    BOOLEAN ChangeProtection;
    MMPTE TempPte;
    ULONG OldProtect;

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    //
    // Check the zero bits argument for correctness.
    //

    if (ZeroBits > MM_MAXIMUM_ZERO_BITS) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Check the AllocationType for correctness.
    //

    if ((AllocationType & ~(MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN |
                            MEM_RESET | MEM_NOZERO)) != 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // One of MEM_COMMIT, MEM_RESET or MEM_RESERVE must be set.
    //

    if ((AllocationType & (MEM_COMMIT | MEM_RESERVE | MEM_RESET)) == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    if ((AllocationType & MEM_RESET) && (AllocationType != MEM_RESET)) {

        //
        // MEM_RESET may not be used with any other flag.
        //

        return STATUS_INVALID_PARAMETER;
    }

    //
    // Check the protection field.
    //

    if (!MiMakePteProtectionMask(Protect, &PteProtectionMask)) {
        return STATUS_INVALID_PAGE_PROTECTION;
    }

    //
    // Capture the base address.
    //

    CapturedBase = *BaseAddress;

    //
    // Capture the region size.
    //

    CapturedRegionSize = *RegionSize;

    //
    // Make sure the specified starting and ending addresses are within the user
    // part of the virtual address space.
    //

    if (CapturedBase > MM_HIGHEST_VAD_ADDRESS) {

        //
        // Invalid base address.
        //

        return STATUS_INVALID_PARAMETER;
    }

    if ((((ULONG_PTR)MM_HIGHEST_VAD_ADDRESS + 1) - (ULONG_PTR)CapturedBase) <
        CapturedRegionSize) {

        //
        // Invalid region size.
        //

        return STATUS_INVALID_PARAMETER;
    }

    if (CapturedRegionSize == 0) {

        //
        // Region size cannot be 0.
        //

        return STATUS_INVALID_PARAMETER;
    }

    MI_LOCK_ADDRESS_SPACE();

    //
    // Handle the case of reserving an address range.
    //

    Vad = NULL;
    DeleteVadOnFailure = FALSE;

    if ((CapturedBase == NULL) || (AllocationType & MEM_RESERVE)) {

        //
        // Reserve the address space.
        //

        if (CapturedBase == NULL) {

            //
            // No base address was specified.  This MUST be a reserve or
            // reserve and commit.
            //

            CapturedRegionSize = ROUND_TO_PAGES(CapturedRegionSize);

            //
            // If the number of zero bits is greater than zero, then calculate
            // the highest address.
            //

            if (ZeroBits != 0) {
                TopAddress = (PCHAR)(((ULONG_PTR)MM_USER_ADDRESS_RANGE_LIMIT) >> ZeroBits);

                //
                // Keep the top address below the highest user vad address
                // regardless.
                //

                if (TopAddress > (PCHAR)MM_HIGHEST_VAD_ADDRESS) {
                    status = STATUS_INVALID_PARAMETER;
                    goto ErrorReturn;
                }

            } else {
                TopAddress = MM_HIGHEST_VAD_ADDRESS;
            }

            //
            // Establish exception handler as MiFindEmptyAddressRange will raise
            // an exception if it fails.
            //

            try {

                if (AllocationType & MEM_TOP_DOWN) {

                    //
                    // Start from the top of memory downward.
                    //

                    StartingAddress = MiFindEmptyAddressRangeDown(
                        CapturedRegionSize, TopAddress, X64K);

                } else {

                    StartingAddress = MiFindEmptyAddressRange(
                        CapturedRegionSize, X64K, (ULONG)ZeroBits);
                }

            } except (EXCEPTION_EXECUTE_HANDLER) {
                status = GetExceptionCode();
                goto ErrorReturn;
            }

            //
            // Calculate the ending address based on the top address.
            //

            EndingAddress = (PVOID)(((ULONG_PTR)StartingAddress +
                CapturedRegionSize - 1L) | (PAGE_SIZE - 1L));

            if (EndingAddress > TopAddress) {

                //
                // The allocation does not honor the zero bits argument.
                //

                status = STATUS_NO_MEMORY;
                goto ErrorReturn;
            }

        } else {

            //
            // A non-NULL base address was specified. Check to make sure the
            // specified base address to ending address is currently unused.
            //

            EndingAddress = (PVOID)(((ULONG_PTR)CapturedBase +
                CapturedRegionSize - 1L) | (PAGE_SIZE - 1L));

            //
            // Align the starting address on a 64k boundary.
            //

            StartingAddress = (PVOID)MI_ALIGN_TO_SIZE(CapturedBase, X64K);

            //
            // See if a VAD overlaps with this starting/ending address pair.
            //

            if (MiCheckForConflictingVad(StartingAddress, EndingAddress) != NULL) {
                status = STATUS_CONFLICTING_ADDRESSES;
                goto ErrorReturn;
            }
        }

        Vad = ExAllocatePoolWithTag(sizeof(MMVAD), 'SdaV');

        if (Vad == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto ErrorReturn;
        }

        Vad->StartingVpn = MI_VA_TO_VPN(StartingAddress);
        Vad->EndingVpn = MI_VA_TO_VPN(EndingAddress);
        Vad->AllocationProtect = Protect;

        MiInsertVad(Vad);

        CapturedBase = StartingAddress;
        CapturedRegionSize = EndingAddress - StartingAddress + 1;

        MmVirtualMemoryBytesReserved += CapturedRegionSize;

        if ((AllocationType & MEM_COMMIT) == 0) {

            //
            // Don't commit the pages, so bail out now.
            //

            MI_UNLOCK_ADDRESS_SPACE();

            *RegionSize = CapturedRegionSize;
            *BaseAddress = CapturedBase;

            return STATUS_SUCCESS;
        }

        //
        // Fall into the commit path.
        //

        DeleteVadOnFailure = TRUE;
    }

    //
    // Handle the case of committing or resetting the pages contained in an
    // address range.
    //

    EndingAddress = (PCHAR)(((ULONG_PTR)CapturedBase + CapturedRegionSize - 1) |
        (PAGE_SIZE - 1));
    StartingAddress = (PCHAR)PAGE_ALIGN(CapturedBase);

    CapturedRegionSize = EndingAddress - StartingAddress + 1;

    //
    // Locate the virtual address descriptor for the specified addresses.
    //

    Vad = MiCheckForConflictingVad(StartingAddress, EndingAddress);

    if (Vad == NULL) {

        //
        // No virtual address is reserved at the specified base address, return
        // an error.
        //

        status = STATUS_CONFLICTING_ADDRESSES;
        goto ErrorReturn;
    }

    //
    // Ensure that the starting and ending addresses are all within the same
    // virtual address descriptor.
    //

    if ((MI_VA_TO_VPN(StartingAddress) < Vad->StartingVpn) ||
        (MI_VA_TO_VPN(EndingAddress) > Vad->EndingVpn)) {

        //
        // Not within the section virtual address descriptor,
        // return an error.
        //

        status = STATUS_CONFLICTING_ADDRESSES;
        goto ErrorReturn;
    }

    //
    // Handle the case of resetting the pages contained in an address range.
    // We wait until this point to ensure that a virtual address descriptor
    // really exists for the input arguments.
    //

    if (AllocationType == MEM_RESET) {

        //
        // There's no pagefile support, so this flag is a no-op.
        //

        status = STATUS_SUCCESS;
        goto ErrorReturn;
    }

    //
    // Compute the starting and ending PTE addresses.
    //

    StartingPte = MiGetPteAddress(StartingAddress);
    EndingPte = MiGetPteAddress(EndingAddress);

    //
    // Make a pass through the PTEs to determine the number of pages that we'll
    // need to commit.
    //

    PagesToCommit = 0;
    PointerPte = StartingPte;

    while (PointerPte <= EndingPte) {

        //
        // If this is the first pass through the loop or the current PTE pointer
        // is on a PDE boundary, then ensure that the page table is committed.
        //

        if ((PointerPte == StartingPte) || MiIsPteOnPdeBoundary(PointerPte)) {

            PointerPde = MiGetPteAddress(PointerPte);

            if (PointerPde->Hard.Valid == 0) {

                //
                // The PDE is not committed.  Add in one page for the page table
                // and all of the pages that need to be committed for this PDE.
                //

                NextPointerPte = MiGetVirtualAddressMappedByPte(PointerPde + 1);

                PagesToCommit++;

                if (NextPointerPte > EndingPte) {
                    PagesToCommit += (EndingPte - PointerPte + 1);
                } else {
                    PagesToCommit += (NextPointerPte - PointerPte);
                }

                PointerPte = NextPointerPte;

                continue;
            }
        }

        //
        // Add in one page if the PTE is zero.  Note that we don't just check
        // the Valid bit because the page may be a PAGE_NOACCESS or PAGE_GUARD
        // protected page.
        //

        if (PointerPte->Long == 0) {
            PagesToCommit++;
        }

        PointerPte++;
    }

    //
    // Now ensure that we can allocate the required number of pages.
    //

    MI_LOCK_MM(&OldIrql);

    if (MmAvailablePages < PagesToCommit) {
        MI_UNLOCK_MM(OldIrql);
        status = STATUS_NO_MEMORY;
        goto ErrorReturn;
    }

    //
    // Make another pass through the PTEs to actually commit the pages.
    //

    RemovePageRoutine = (AllocationType & MEM_NOZERO) ? MiRemoveAnyPage :
        MiRemoveZeroPage;
    BusyType = (Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ |
        PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) ? MmImageUsage :
        MmVirtualMemoryUsage;
    ChangeProtection = FALSE;
    PointerPte = StartingPte;
    PdePageFrame = NULL;

    while (PointerPte <= EndingPte) {

        //
        // If this is the first pass through the loop or the current PTE pointer
        // is on a PDE boundary, then ensure that the page table is committed.
        //

        if ((PointerPte == StartingPte) || MiIsPteOnPdeBoundary(PointerPte)) {

            PointerPde = MiGetPteAddress(PointerPte);

            if (PointerPde->Hard.Valid == 0) {

                PagesToCommit--;

                TempPte.Long = MiGetValidKernelPdeBits();
                TempPte.Hard.PageFrameNumber =
                    MiRemoveZeroPage(MmVirtualPageTableUsage, PointerPde);

                MI_WRITE_PTE(PointerPde, TempPte);
            }

            PdePageFrame = MI_PFN_ELEMENT(PointerPde->Hard.PageFrameNumber);

            ASSERT(PdePageFrame->Busy.Busy == 1);
            ASSERT(PdePageFrame->Busy.BusyType == MmVirtualPageTableUsage);
        }

        if (PointerPte->Long == 0) {

            PagesToCommit--;

            TempPte.Long = PteProtectionMask;
            TempPte.Hard.PageFrameNumber = RemovePageRoutine(BusyType, PointerPte);

            MI_WRITE_PTE(PointerPte, TempPte);

            PdePageFrame->Directory.NumberOfUsedPtes++;

            ASSERT(PdePageFrame->Directory.NumberOfUsedPtes <= PTE_PER_PAGE);

        } else if ((PointerPte->Long & MM_PTE_PROTECTION_MASK) != PteProtectionMask) {

            ChangeProtection = TRUE;
        }

        PointerPte++;
    }

    ASSERT(PagesToCommit == 0);
    ASSERT(PdePageFrame != NULL && PdePageFrame->Directory.NumberOfUsedPtes > 0);

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

    MI_UNLOCK_ADDRESS_SPACE();

    *RegionSize = CapturedRegionSize;
    *BaseAddress = StartingAddress;

    //
    // If we found a page that was already committed but had the wrong page
    // protection, then make a call to NtProtectVirtualMemory to make all of
    // the pages have the right attributes.
    //

    if (ChangeProtection) {
        NtProtectVirtualMemory(&StartingAddress, &CapturedRegionSize, Protect,
            &OldProtect);
    }

    return STATUS_SUCCESS;

ErrorReturn:
    if (!NT_SUCCESS(status) && DeleteVadOnFailure) {

        MmVirtualMemoryBytesReserved -= CapturedRegionSize;

        MiRemoveVad(Vad);
        ExFreePool(Vad);
    }

    MI_UNLOCK_ADDRESS_SPACE();
    return status;
}

NTSTATUS
NtFreeVirtualMemory(
    IN OUT PVOID *BaseAddress,
    IN OUT PSIZE_T RegionSize,
    IN ULONG FreeType
    )
/*++

Routine Description:

    This function deletes a region of pages within the virtual address
    space of a subject process.

Arguments:

    BaseAddress - The base address of the region of pages
                  to be freed. This value is rounded down to the
                  next host page address boundary.

    RegionSize - A pointer to a variable that will receive
                 the actual size in bytes of the freed region of
                 pages. The initial value of this argument is
                 rounded up to the next host page size boundary.

    FreeType - A set of flags that describe the type of
               free that is to be performed for the specified
               region of pages.

       FreeType Flags

        MEM_DECOMMIT - The specified region of pages is to
             be decommitted.

        MEM_RELEASE - The specified region of pages is to
             be released.

Return Value:

    Status of operation.

Environment:

    Kernel mode, PASSIVE_LEVEL.

--*/
{
    NTSTATUS status;
    PVOID CapturedBase;
    SIZE_T CapturedRegionSize;
    PCHAR EndingAddress;
    PCHAR StartingAddress;
    PMMVAD Vad;
    PMMVAD NewVad;
    KIRQL OldIrql;
    PMMPTE StartingPte;
    PMMPTE EndingPte;
    MMPTE TempPte;
    PMMPTE PointerPte;
    PMMPFN PdePageFrame;
    PMMPTE PointerPde;
    PFN_NUMBER PageFrameNumber;
    BOOLEAN FullTlbFlush;

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    //
    // Check to make sure FreeType is good.
    //

    if ((FreeType & ~(MEM_DECOMMIT | MEM_RELEASE)) != 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // One of MEM_DECOMMIT or MEM_RELEASE must be specified, but not both.
    //

    if (((FreeType & (MEM_DECOMMIT | MEM_RELEASE)) == 0) ||
        ((FreeType & (MEM_DECOMMIT | MEM_RELEASE)) ==
                            (MEM_DECOMMIT | MEM_RELEASE))) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Capture the base address.
    //

    CapturedBase = *BaseAddress;

    //
    // Capture the region size.
    //

    CapturedRegionSize = *RegionSize;

    //
    // Make sure the specified starting and ending addresses are within the user
    // part of the virtual address space.
    //

    if (CapturedBase > MM_HIGHEST_USER_ADDRESS) {

        //
        // Invalid base address.
        //

        return STATUS_INVALID_PARAMETER;
    }

    if ((ULONG_PTR)MM_HIGHEST_USER_ADDRESS - (ULONG_PTR)CapturedBase <
        CapturedRegionSize) {

        //
        // Invalid region size.
        //

        return STATUS_INVALID_PARAMETER;
    }

    EndingAddress = (PCHAR)(((ULONG_PTR)CapturedBase + CapturedRegionSize - 1) |
        (PAGE_SIZE - 1));
    StartingAddress = (PCHAR)PAGE_ALIGN(CapturedBase);

    MI_LOCK_ADDRESS_SPACE();

    Vad = (PMMVAD)MiLocateAddress(StartingAddress);

    if (Vad == NULL) {

        //
        // No virtual address descriptor located for base address.
        //

        status = STATUS_MEMORY_NOT_ALLOCATED;
        goto ErrorReturn;
    }

    //
    // Found the associated virtual address descriptor.
    //

    if (Vad->EndingVpn < MI_VA_TO_VPN(EndingAddress)) {

        //
        // The entire range to delete is not contained within a single
        // virtual address descriptor.  Return an error.
        //

        status = STATUS_UNABLE_TO_FREE_VM;
        goto ErrorReturn;
    }

    //
    // Handle the case of releasing an address range.
    //

    if (FreeType & MEM_RELEASE) {

        //
        // If the region size is zero, remove the whole VAD.
        //

        if (CapturedRegionSize == 0) {

            //
            // If the region size is specified as 0, the base address
            // must be the starting address for the region.
            //

            if (MI_VA_TO_VPN(CapturedBase) != Vad->StartingVpn) {
                status = STATUS_FREE_VM_NOT_AT_BASE;
                goto ErrorReturn;
            }

            //
            // Delete the VAD.
            //

            StartingAddress = MI_VPN_TO_VA(Vad->StartingVpn);
            EndingAddress = MI_VPN_TO_VA_ENDING(Vad->EndingVpn);

            MiRemoveVad(Vad);
            ExFreePool(Vad);

        } else {

            //
            // Region's size was not specified as zero, delete the whole VAD or
            // split the VAD.
            //

            if (MI_VA_TO_VPN(StartingAddress) == Vad->StartingVpn) {

                if (MI_VA_TO_VPN(EndingAddress) == Vad->EndingVpn) {

                    //
                    // Delete the VAD.
                    //

                    MiRemoveVad(Vad);
                    ExFreePool(Vad);

                } else {

                    //
                    // Change the starting address of the VAD.
                    //

                    Vad->StartingVpn = MI_VA_TO_VPN(EndingAddress + 1);
                }

            } else {

                //
                // Starting address is greater than start of VAD.
                //

                if (MI_VA_TO_VPN(EndingAddress) == Vad->EndingVpn) {

                    //
                    // Change the ending address of the VAD.
                    //

                    Vad->EndingVpn = MI_VA_TO_VPN(StartingAddress - 1);

                } else {

                    //
                    // Split this VAD as the address range is within the VAD.
                    //

                    NewVad = ExAllocatePoolWithTag(sizeof(MMVAD), 'SdaV');

                    if (NewVad == NULL) {
                        status = STATUS_INSUFFICIENT_RESOURCES;
                        goto ErrorReturn;
                    }

                    *NewVad = *Vad;

                    Vad->EndingVpn = MI_VA_TO_VPN(StartingAddress - 1);
                    NewVad->StartingVpn = MI_VA_TO_VPN(EndingAddress + 1);

                    MiInsertVad(NewVad);
                }
            }
        }

        MmVirtualMemoryBytesReserved -= (1 + EndingAddress - StartingAddress);

        //
        // Fall into the decommit path.
        //
    }

    //
    // Handle the case of decomitting pages in an address range.
    //

    MI_LOCK_MM(&OldIrql);

    //
    // Compute the starting and ending PTE addresses.
    //

    StartingPte = MiGetPteAddress(StartingAddress);
    EndingPte = MiGetPteAddress(EndingAddress);

    //
    // Make a pass through the PTEs to decommit pages.
    //

    TempPte.Long = 0;
    PointerPte = StartingPte;
    PdePageFrame = NULL;
    FullTlbFlush = FALSE;

    while (PointerPte <= EndingPte) {

        //
        // If this is the first pass through the loop or the current PTE pointer
        // is on a PDE boundary, then ensure that the page table is committed.
        //

        if ((PointerPte == StartingPte) || MiIsPteOnPdeBoundary(PointerPte)) {

            PointerPde = MiGetPteAddress(PointerPte);

            if (PointerPde->Hard.Valid == 0) {
                PointerPte = MiGetVirtualAddressMappedByPte(PointerPde + 1);
                continue;
            }

            PdePageFrame = MI_PFN_ELEMENT(PointerPde->Hard.PageFrameNumber);
        }

        //
        // Check if the PTE is marked committed.  Note that we don't just check
        // the Valid bit because the page may be a PAGE_NOACCESS or PAGE_GUARD
        // protected page.
        //

        if (PointerPte->Long != 0) {

            PageFrameNumber = PointerPte->Hard.PageFrameNumber;

            MI_WRITE_AND_FLUSH_PTE(PointerPte, TempPte);

            MiReleasePageOwnership(PageFrameNumber);

            //
            // Decrement the number of PTEs allocated to this page directory and
            // free the page directory if the count is zero.
            //

            PdePageFrame->Directory.NumberOfUsedPtes--;

            if (PdePageFrame->Directory.NumberOfUsedPtes == 0) {

                PointerPde = MiGetPteAddress(PointerPte);
                PageFrameNumber = PointerPde->Hard.PageFrameNumber;

                MI_WRITE_PTE(PointerPde, TempPte);

                FullTlbFlush = TRUE;

                MiReleasePageOwnership(PageFrameNumber);

                PointerPte = MiGetVirtualAddressMappedByPte(PointerPde + 1);
                continue;
            }
        }

        PointerPte++;
    }

    //
    // If any page table pages were discarded above, then do a full flush of the
    // TLB.
    //

    if (FullTlbFlush) {
        KeFlushCurrentTb();
    }

    MI_UNLOCK_MM(OldIrql);

    MI_UNLOCK_ADDRESS_SPACE();

    *RegionSize = 1 + EndingAddress - StartingAddress;
    *BaseAddress = StartingAddress;

    return STATUS_SUCCESS;

ErrorReturn:
    MI_UNLOCK_ADDRESS_SPACE();
    return status;
}

NTSTATUS
NtQueryVirtualMemory(
    IN PVOID BaseAddress,
    OUT PMEMORY_BASIC_INFORMATION MemoryInformation
    )
/*++

Routine Description:

    This function provides the capability to determine the state,
    protection, and type of a region of pages within the virtual address
    space of the subject process.

    The state of the first page within the region is determined and then
    subsequent entries in the process address map are scanned from the
    base address upward until either the entire range of pages has been
    scanned or until a page with a nonmatching set of attributes is
    encountered. The region attributes, the length of the region of pages
    with matching attributes, and an appropriate status value are
    returned.

    If the entire region of pages does not have a matching set of
    attributes, then the returned length parameter value can be used to
    calculate the address and length of the region of pages that was not
    scanned.

Arguments:

    BaseAddress - The base address of the region of pages to be
        queried. This value is rounded down to the next host-page-
        address boundary.

    MemoryInformation - A pointer to a buffer that receives the
        specified information.  The format and content of the buffer
        depend on the specified information class.

        MemoryBasicInformation - Data type is PMEMORY_BASIC_INFORMATION.

            MEMORY_BASIC_INFORMATION Structure

            ULONG RegionSize - The size of the region in bytes
                beginning at the base address in which all pages have
                identical attributes.

            ULONG State - The state of the pages within the region.

                State Values

                MEM_COMMIT - The state of the pages within the region
                    is committed.

                MEM_FREE - The state of the pages within the region
                    is free.

                MEM_RESERVE - The state of the pages within the
                    region is reserved.

            ULONG Protect - The protection of the pages within the
                region.

                Protect Values

                PAGE_NOACCESS - No access to the region of pages is
                    allowed. An attempt to read, write, or execute
                    within the region results in an access violation
                    (i.e., a GP fault).

                PAGE_EXECUTE - Execute access to the region of pages
                    is allowed. An attempt to read or write within
                    the region results in an access violation.

                PAGE_READONLY - Read-only and execute access to the
                    region of pages is allowed. An attempt to write
                    within the region results in an access violation.

                PAGE_READWRITE - Read, write, and execute access to
                    the region of pages is allowed. If write access
                    to the underlying section is allowed, then a
                    single copy of the pages are shared. Otherwise,
                    the pages are shared read-only/copy-on-write.

                PAGE_GUARD - Read, write, and execute access to the
                    region of pages is allowed; however, access to
                    the region causes a "guard region entered"
                    condition to be raised in the subject process.

                PAGE_NOCACHE - Disable the placement of committed
                    pages into the data cache.

            ULONG Type - The type of pages within the region.

                Type Values

                MEM_PRIVATE - The pages within the region are
                    private.

                MEM_MAPPED - The pages within the region are mapped
                    into the view of a section.

                MEM_IMAGE - The pages within the region are mapped
                    into the view of an image section.

Return Value:

    Status of operation.

Environment:

    Kernel mode, PASSIVE_LEVEL.

--*/
{
    PMMVAD Vad;
    ULONG_PTR BaseVpn;
    BOOLEAN Found;
    SIZE_T RegionSize;
    PMMPTE StartingPte;
    PMMPTE EndingPte;
    PMMPTE PointerPte;
    PMMPTE PointerPde;
    ULONG AllocationProtect;
    ULONG Protect;
    ULONG PteProtectionMask;
    ULONG State;

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    if (BaseAddress > MM_HIGHEST_VAD_ADDRESS) {

        //
        // Invalid base address.
        //

        return STATUS_INVALID_PARAMETER;
    }

    MI_LOCK_ADDRESS_SPACE();

    //
    // Locate the VAD that contains the base address or the VAD which follows
    // the base address.
    //

    Vad = (PMMVAD)MmVadRoot;
    BaseVpn = MI_VA_TO_VPN(BaseAddress);
    Found = FALSE;

    for (;;) {

        if (Vad == NULL) {
            break;
        }

        if ((BaseVpn >= Vad->StartingVpn) && (BaseVpn <= Vad->EndingVpn)) {
            Found = TRUE;
            break;
        }

        if (BaseVpn < Vad->StartingVpn) {
            if (Vad->LeftChild == NULL) {
                break;
            }
            Vad = (PMMVAD)Vad->LeftChild;

        } else {
            if (BaseVpn < Vad->EndingVpn) {
                break;
            }
            if (Vad->RightChild == NULL) {
                break;
            }
            Vad = (PMMVAD)Vad->RightChild;
        }
    }

    if (!Found) {

        //
        // There is no virtual address allocated at the base address.  Return
        // the size of the hole starting at the base address.
        //

        if (Vad == NULL) {
            RegionSize = ((PCHAR)MM_HIGHEST_VAD_ADDRESS + 1) -
                (PCHAR)PAGE_ALIGN(BaseAddress);
        } else {
            if (Vad->StartingVpn < BaseVpn) {

                //
                // We are looking at the Vad which occupies the range just
                // before the desired range.  Get the next Vad.
                //

                Vad = MiGetNextVad(Vad);
                if (Vad == NULL) {
                    RegionSize = ((PCHAR)MM_HIGHEST_VAD_ADDRESS + 1) -
                        (PCHAR)PAGE_ALIGN(BaseAddress);
                } else {
                    RegionSize = (PCHAR)MI_VPN_TO_VA (Vad->StartingVpn) -
                        (PCHAR)PAGE_ALIGN(BaseAddress);
                }
            } else {
                RegionSize = (PCHAR)MI_VPN_TO_VA (Vad->StartingVpn) -
                    (PCHAR)PAGE_ALIGN(BaseAddress);
            }
        }

        MI_UNLOCK_ADDRESS_SPACE();

        MemoryInformation->AllocationBase = NULL;
        MemoryInformation->AllocationProtect = 0;
        MemoryInformation->BaseAddress = PAGE_ALIGN(BaseAddress);
        MemoryInformation->RegionSize = RegionSize;
        MemoryInformation->State = MEM_FREE;
        MemoryInformation->Protect = PAGE_NOACCESS;
        MemoryInformation->Type = 0;

        return STATUS_SUCCESS;
    }

    //
    // There is a virtual address allocated at the base address.
    //

    StartingPte = MiGetPteAddress(BaseAddress);
    EndingPte = MiGetPteAddress(MI_VPN_TO_VA(Vad->EndingVpn));

    AllocationProtect = Vad->AllocationProtect;

    //
    // Determine the state and protection attributes of the base address.
    //

    Protect = 0;
    PteProtectionMask = 0;
    PointerPte = StartingPte;
    PointerPde = MiGetPteAddress(PointerPte);

    if (PointerPde->Hard.Valid != 0) {

        //
        // Check if the PTE is marked committed.  Note that we don't just check
        // the Valid bit because the page may be a PAGE_NOACCESS or PAGE_GUARD
        // protected page.
        //

        if (PointerPte->Long == 0) {
            State = MEM_RESERVE;
        } else {
            State = MEM_COMMIT;
            PteProtectionMask = (PointerPte->Long & MM_PTE_PROTECTION_MASK);
            Protect = MiDecodePteProtectionMask(PteProtectionMask);
        }

    } else {

        State = MEM_RESERVE;
    }

    //
    // Make a pass through the PTEs to compute the number of pages with the same
    // state and protection attributes as the base address.
    //

    while (PointerPte <= EndingPte) {

        //
        // If this is the first pass through the loop or the current PTE pointer
        // is on a PDE boundary, then ensure that the page table is committed.
        //

        if ((PointerPte == StartingPte) || MiIsPteOnPdeBoundary(PointerPte)) {

            PointerPde = MiGetPteAddress(PointerPte);

            if (PointerPde->Hard.Valid == 0) {

                if (State == MEM_COMMIT) {
                    break;
                }

                PointerPte = MiGetVirtualAddressMappedByPte(PointerPde + 1);
                continue;
            }
        }

        if (PointerPte->Long != 0) {

            if (State == MEM_RESERVE) {
                break;
            }

            if ((PointerPte->Long & MM_PTE_PROTECTION_MASK) != PteProtectionMask) {
                break;
            }

        } else {

            if (State == MEM_COMMIT) {
                break;
            }
        }

        PointerPte++;
    }

    if (PointerPte > EndingPte) {
        PointerPte = EndingPte + 1;
    }

    RegionSize = (PCHAR)MiGetVirtualAddressMappedByPte(PointerPte) -
        (PCHAR)PAGE_ALIGN(BaseAddress);

    //
    // We should have found at least one valid page worth of data.
    //

    ASSERT(RegionSize > 0);

    MI_UNLOCK_ADDRESS_SPACE();

    MemoryInformation->AllocationBase = MI_VPN_TO_VA(Vad->StartingVpn);
    MemoryInformation->AllocationProtect = AllocationProtect;
    MemoryInformation->BaseAddress = PAGE_ALIGN(BaseAddress);
    MemoryInformation->RegionSize = RegionSize;
    MemoryInformation->State = State;
    MemoryInformation->Protect = Protect;
    MemoryInformation->Type = MEM_PRIVATE;

    return STATUS_SUCCESS;
}

NTSTATUS
NtProtectVirtualMemory(
    IN OUT PVOID *BaseAddress,
    IN OUT PSIZE_T RegionSize,
    IN ULONG NewProtect,
    OUT PULONG OldProtect
    )
/*++

Routine Description:

    This routine changes the protection on a region of committed pages
    within the virtual address space of the subject process.  Setting
    the protection on a range of pages causes the old protection to be
    replaced by the specified protection value.

Arguments:

    BaseAddress - The base address of the region of pages
          whose protection is to be changed. This value is
          rounded down to the next host page address
          boundary.

    RegionSize - A pointer to a variable that will receive
          the actual size in bytes of the protected region
          of pages. The initial value of this argument is
          rounded up to the next host page size boundary.

    NewProtect - The new protection desired for the
          specified region of pages.

     Protect Values

          PAGE_NOACCESS - No access to the specified region
               of pages is allowed. An attempt to read,
               write, or execute the specified region
               results in an access violation (i.e. a GP
               fault).

          PAGE_EXECUTE - Execute access to the specified
               region of pages is allowed. An attempt to
               read or write the specified region results in
               an access violation.

          PAGE_READONLY - Read only and execute access to the
               specified region of pages is allowed. An
               attempt to write the specified region results
               in an access violation.

          PAGE_READWRITE - Read, write, and execute access to
               the specified region of pages is allowed. If
               write access to the underlying section is
               allowed, then a single copy of the pages are
               shared. Otherwise the pages are shared read
               only/copy on write.

          PAGE_GUARD - Read, write, and execute access to the
               specified region of pages is allowed,
               however, access to the region causes a "guard
               region entered" condition to be raised in the
               subject process. If write access to the
               underlying section is allowed, then a single
               copy of the pages are shared. Otherwise the
               pages are shared read only/copy on write.

          PAGE_NOCACHE - The page should be treated as uncached.
               This is only valid for non-shared pages.

     OldProtect - A pointer to a variable that will receive
          the old protection of the first page within the
          specified region of pages.

Return Value:

    Status of operation.

Environment:

    Kernel mode, PASSIVE_LEVEL.

--*/
{
    NTSTATUS status;
    ULONG PteProtectionMask;
    PVOID CapturedBase;
    SIZE_T CapturedRegionSize;
    PCHAR EndingAddress;
    PCHAR StartingAddress;
    PMMVAD Vad;
    PMMPTE StartingPte;
    PMMPTE EndingPte;
    PMMPTE PointerPte;
    PMMPTE PointerPde;
    KIRQL OldIrql;
    ULONG OldPteProtectionMask;
    MMPTE TempPte;

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    //
    // Check the protection field.
    //

    if (!MiMakePteProtectionMask(NewProtect, &PteProtectionMask)) {
        return STATUS_INVALID_PAGE_PROTECTION;
    }

    //
    // Capture the base address.
    //

    CapturedBase = *BaseAddress;

    //
    // Capture the region size.
    //

    CapturedRegionSize = *RegionSize;

    //
    // Make sure the specified starting and ending addresses are within the user
    // part of the virtual address space.
    //

    if (CapturedBase > MM_HIGHEST_USER_ADDRESS) {

        //
        // Invalid base address.
        //

        return STATUS_INVALID_PARAMETER;
    }

    if ((ULONG_PTR)MM_HIGHEST_USER_ADDRESS - (ULONG_PTR)CapturedBase <
        CapturedRegionSize) {

        //
        // Invalid region size.
        //

        return STATUS_INVALID_PARAMETER;
    }

    if (CapturedRegionSize == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    MI_LOCK_ADDRESS_SPACE();

    EndingAddress = (PCHAR)(((ULONG_PTR)CapturedBase + CapturedRegionSize - 1) |
        (PAGE_SIZE - 1));
    StartingAddress = (PCHAR)PAGE_ALIGN(CapturedBase);

    //
    // Locate the virtual address descriptor for the specified addresses.
    //

    Vad = MiCheckForConflictingVad(StartingAddress, EndingAddress);

    if (Vad == NULL) {

        //
        // No virtual address is reserved at the specified base address,
        // return an error.
        //

        status = STATUS_CONFLICTING_ADDRESSES;
        goto ErrorReturn;
    }

    //
    // Ensure that the starting and ending addresses are all within the same
    // virtual address descriptor.
    //

    if ((MI_VA_TO_VPN(StartingAddress) < Vad->StartingVpn) ||
        (MI_VA_TO_VPN(EndingAddress) > Vad->EndingVpn)) {

        //
        // Not within the section virtual address descriptor,
        // return an error.
        //

        status = STATUS_CONFLICTING_ADDRESSES;
        goto ErrorReturn;
    }

    //
    // Compute the starting and ending PTE addresses.
    //

    StartingPte = MiGetPteAddress(StartingAddress);
    EndingPte = MiGetPteAddress(EndingAddress);

    //
    // Make a pass through the PTEs to verify that all of the pages are
    // committed.
    //

    PointerPte = StartingPte;

    while (PointerPte <= EndingPte) {

        //
        // If this is the first pass through the loop or the current PTE pointer
        // is on a PDE boundary, then ensure that the page table is committed.
        //

        if ((PointerPte == StartingPte) || MiIsPteOnPdeBoundary(PointerPte)) {

            PointerPde = MiGetPteAddress(PointerPte);

            if (PointerPde->Hard.Valid == 0) {
                status = STATUS_NOT_COMMITTED;
                goto ErrorReturn;
            }
        }

        //
        // Check if the PTE is marked committed.  Note that we don't just check
        // the Valid bit because the page may be a PAGE_NOACCESS or PAGE_GUARD
        // protected page.
        //

        if (PointerPte->Long == 0) {
            status = STATUS_NOT_COMMITTED;
            goto ErrorReturn;
        }

        PointerPte++;
    }

    //
    // Make another pass through the PTEs to change the page permissions.
    //

    MI_LOCK_MM(&OldIrql);

    OldPteProtectionMask = PteProtectionMask;
    PointerPte = StartingPte;

    while (PointerPte <= EndingPte) {

        if (PointerPte->Long != 0) {

            TempPte = *PointerPte;

            if ((TempPte.Long & MM_PTE_PROTECTION_MASK) != PteProtectionMask) {

                //
                // Save off the attributes of the first non-matching page that
                // we find so that we can return the old protection code to the
                // caller.
                //

                if (OldPteProtectionMask == PteProtectionMask) {
                    OldPteProtectionMask = (TempPte.Long & MM_PTE_PROTECTION_MASK);
                }

                TempPte.Long = ((TempPte.Long & ~MM_PTE_PROTECTION_MASK) |
                    PteProtectionMask);

                MI_WRITE_AND_FLUSH_PTE(PointerPte, TempPte);
            }
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

    MI_UNLOCK_ADDRESS_SPACE();

    *RegionSize = 1 + EndingAddress - StartingAddress;
    *BaseAddress = StartingAddress;
    *OldProtect = MiDecodePteProtectionMask(OldPteProtectionMask);

    return STATUS_SUCCESS;

ErrorReturn:
    MI_UNLOCK_ADDRESS_SPACE();
    return status;
}
