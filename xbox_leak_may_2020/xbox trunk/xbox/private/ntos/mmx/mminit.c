/*++

Copyright (c) 1989-2002  Microsoft Corporation

Module Name:

    mminit.c

Abstract:

    This module implements the initialization for the memory management system.

--*/

#include "mi.h"

//
// Export the data structures used by the memory manager implementation.
//
DECLSPEC_RDATA MMGLOBALDATA MmGlobalData = {
    &MmRetailPfnRegion,
    &MmSystemPteRange,
    &MmAvailablePages,
    MmAllocatedPagesByUsage,
    &MmAddressSpaceLock,
    &MmVadRoot,
    &MmVadHint,
    &MmVadFreeHint
};

//
// Stores the physical address of the top byte, not inclusive, of the kernel
// image.
//
DECLSPEC_STICKY ULONG_PTR MmTopOfKernelPhysicalAddress;

//
// Local support.
//

#ifdef DEVKIT
VOID
MiCopyShadowROMDataSection(
    VOID
    );
#endif

#ifdef ALLOC_PRAGMA
#ifdef DEVKIT
#pragma alloc_text(INIT, MiCopyShadowROMDataSection)
#endif
#endif

#ifdef DEVKIT

VOID
MiCopyShadowROMDataSection(
    VOID
    )
/*++

Routine Description:

    This routine takes a snapshot of the initialized .data section from the
    shadow ROM and fixes up the kernel's headers to point at this copy.

Arguments:

    None.

Return Value:

    None.

Environment:

    Kernel mode.

--*/
{
    PXDATA_SECTION_HEADER DataSectionHeader;
    PFN_COUNT DataSectionSizeInPages;
    PFN_COUNT PfnDatabaseSizeInPages;
    PUCHAR DataSectionCopy;

    //
    // Compute how many pages will be required to hold the copy of the
    // initialized data.
    //

    DataSectionHeader = (PXDATA_SECTION_HEADER)((PIMAGE_DOS_HEADER)PsNtosImageBase)->e_res2;
    DataSectionSizeInPages = BYTES_TO_PAGES(DataSectionHeader->SizeOfInitializedData);

    //
    // Compute how many pages will be required to hold the PFN database.
    //

    PfnDatabaseSizeInPages = BYTES_TO_PAGES((MM_HIGHEST_PHYSICAL_PAGE + 1) *
        sizeof(MMPFN));

#ifdef ARCADE
    //
    // Always place the copy of the initialized data below the PFN database and
    // NVIDIA NV2A instance memory.
    //

    DataSectionCopy = MI_CONVERT_PFN_TO_PHYSICAL(MM_DATABASE_PHYSICAL_PAGE -
        DataSectionSizeInPages);
#else
    //
    // Figure out where we're going to put the copy of the initialized data.  If
    // this is a machine with 64M of memory, then the copy is placed below the
    // PFN database and NVIDIA NV2A instance memory.  Otherwise, the copy is
    // placed after the PFN database in the upper 64M.
    //

    if (MM_DATABASE_PHYSICAL_PAGE + PfnDatabaseSizeInPages +
        DataSectionSizeInPages >= MM_HIGHEST_PHYSICAL_PAGE) {
        DataSectionCopy = MI_CONVERT_PFN_TO_PHYSICAL(MM_INSTANCE_PHYSICAL_PAGE -
            DataSectionSizeInPages);
    } else {
        DataSectionCopy = MI_CONVERT_PFN_TO_PHYSICAL(MM_DATABASE_PHYSICAL_PAGE +
            PfnDatabaseSizeInPages);
    }
#endif

    //
    // Copy the initialized data to the private copy.
    //

    RtlCopyMemory(DataSectionCopy, (PUCHAR)DataSectionHeader->PointerToRawData,
        DataSectionHeader->SizeOfInitializedData);

    //
    // Update the image header to point at the private copy of the data.
    //

    DataSectionHeader->PointerToRawData = (ULONG_PTR)DataSectionCopy;
}

#endif // DEVKIT

VOID
MmInitSystem(
    VOID
    )
/*++

Routine Description:

    This routine initializes the memory manager.

Arguments:

    None.

Return Value:

    None.

Environment:

    Kernel mode.

--*/
{
    MMPTE TempPte;
    PMMPTE PointerPde;
#ifndef ARCADE
    PCI_SLOT_NUMBER PCISlotNumber;
    UCHAR MemoryTop;
#endif
    PMMPTE EndingPointerPde;

    //
    // Verify that the page attribute table (PAT) is initialized correctly by
    // the boot loader.  Specifically, we want to be able to mark pages as USWC.
    //

    ASSERT(RDMSR(0x277) == 0x0007010600070106ui64);

    //
    // Unmap the lower two gigabytes of memory.
    //

    for (PointerPde = MiGetPdeAddress(0); PointerPde <
        MiGetPdeAddress(MM_PHYSICAL_MAP_BASE); PointerPde++) {
        MI_WRITE_ZERO_PTE(PointerPde);
    }

    //
    // Unmap the boot ROM from its identity mapping at the top of memory.
    //

    PointerPde = MiGetPdeAddress((PVOID)MAXULONG_PTR);
    MI_WRITE_ZERO_PTE(PointerPde);

    //
    // Map the MM_DEVICE_WC_BASE window.
    //

    TempPte.Long = MiGetValidKernelPteBits();
    TempPte.Hard.LargePage = 1;
    TempPte.Hard.PageFrameNumber = MM_DEVICE_WC_BASE >> PAGE_SHIFT;
    MI_SET_PTE_WRITE_COMBINE(TempPte);

    for (PointerPde = MiGetPdeAddress(MM_DEVICE_WC_BASE); PointerPde <=
        MiGetPdeAddress(MM_DEVICE_WC_END); PointerPde++) {
        MI_WRITE_PTE(PointerPde, TempPte);
        TempPte.Long += PAGE_SIZE_LARGE;
    }

    //
    // Map the MM_DEVICE_UC_BASE window.
    //

    TempPte.Hard.PageFrameNumber = MM_DEVICE_UC_BASE >> PAGE_SHIFT;
    MI_DISABLE_CACHING(TempPte);

    for (PointerPde = MiGetPdeAddress(MM_DEVICE_UC_BASE); PointerPde <=
        MiGetPdeAddress(MM_DEVICE_UC_END); PointerPde++) {
        MI_WRITE_PTE(PointerPde, TempPte);
        TempPte.Long += PAGE_SIZE_LARGE;
    }

#ifndef ARCADE
    //
    // Access the host bridge's configuration space.
    //

    PCISlotNumber.u.AsULONG = 0;
    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_HOSTBRIDGE_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_HOSTBRIDGE_FUNCTION_ID;

#ifdef DEVKIT
    //
    // Configure the memory manager to use the page count configured in the host
    // bridge.  Each increment of the memory top is equal to 16 megabytes, or
    // 4096 pages.
    //

    HalReadPCISpace(0, PCISlotNumber.u.AsULONG, CR_CPU_MEMTOP_LIMIT,
        &MemoryTop, sizeof(UCHAR));

    MmHighestPhysicalPage = (((ULONG)MemoryTop + 1) * 4096) - 1;

    //
    // If the system only has 64 megabytes of memory, then allow pages in the
    // developer kit range to use memory from standard system memory.
    //

    if (MM_HIGHEST_PHYSICAL_PAGE < MM_64M_PHYSICAL_PAGE) {
        MmDeveloperKitPteRange.AvailablePages = &MmAvailablePages;
        MmDeveloperKitPteRange.RemovePageRoutine = MiRemoveZeroPage;
    }
#else
    //
    // Configure the host bridge's top of memory to a hardwired 64 megabytes.
    //

    MemoryTop = CR_CPU_MEMTOP_LIMIT_64MB;

    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, CR_CPU_MEMTOP_LIMIT,
        &MemoryTop, sizeof(UCHAR));
#endif
#endif

#ifdef DEVKIT
    //
    // If this is the first boot and we booted from a ROM image loaded in RAM,
    // then make a copy of the initialized .data section so that we can throw
    // away the rest of the shadow ROM.
    //

    if (!KeHasQuickBooted && (XboxBootFlags & XBOX_BOOTFLAG_SHADOW)) {
        MiCopyShadowROMDataSection();
    }
#endif

    //
    // Initialize the page frame database.
    //

    if (!KeHasQuickBooted) {
        MiInitializePfnDatabase();
    } else {
        MiReinitializePfnDatabase();
    }

    //
    // Switch from the large pages used by the boot loader to the page tables
    // contained in the page frame database.
    //

    TempPte.Long = MiGetValidKernelPdeBits();
    TempPte.Hard.PageFrameNumber = MM_DATABASE_PHYSICAL_PAGE;

    PointerPde = MiGetPdeAddress(MM_SYSTEM_PHYSICAL_MAP);
    EndingPointerPde = MiGetPdeAddress(MI_CONVERT_PFN_TO_PHYSICAL(MM_HIGHEST_PHYSICAL_PAGE));

    while (PointerPde <= EndingPointerPde) {
        MI_WRITE_PTE(PointerPde, TempPte);
        TempPte.Hard.PageFrameNumber++;
        PointerPde++;
    }

    //
    // Unmap the rest of the system RAM physical window.
    //

    EndingPointerPde = MiGetPdeAddress(MM_SYSTEM_PHYSICAL_MAP +
        MM_BYTES_IN_PHYSICAL_MAP - 1);

    while (PointerPde <= EndingPointerPde) {
        MI_WRITE_ZERO_PTE(PointerPde);
        PointerPde++;
    }

    //
    // Flush the TLB now that the switch over is complete.
    //

    KeFlushCurrentTb();

    //
    // Initialize the pool support.
    //

    InitializePool();

    //
    // Allocate the "write 0xFF" PTEs for the file system cache.  We're called
    // early enough in the boot process that this shouldn't fail.
    //

    FscWriteFFsPtes = MiReserveSystemPtes(&MmSystemPteRange,
        FSCACHE_NUMBER_OF_WRITE_FF_PTES);

    ASSERT(FscWriteFFsPtes != NULL);

    //
    // Set the file system cache to the default boot size.
    //

    FscSetCacheSize(16);
}

VOID
MmDiscardInitSection(
    VOID
    )
/*++

Routine Description:

    This routine frees the INIT section of the kernel.

Arguments:

    None.

Return Value:

    None.

Environment:

    Kernel mode.

--*/
{
    PIMAGE_NT_HEADERS NtHeader;
    PIMAGE_SECTION_HEADER InitSectionHeader;
    PUCHAR VirtualAddress;
    PUCHAR EndingVirtualAddress;
    PFN_NUMBER PageFrameNumber;
    PFN_NUMBER EndingPageFrameNumberExclusive;
    KIRQL OldIrql;

    //
    // Find the NT headers for XBOXKRNL.EXE.
    //

    NtHeader = RtlImageNtHeader(PsNtosImageBase);

    ASSERT(NtHeader != NULL);
    ASSERT(NtHeader->FileHeader.NumberOfSections > 0);

    //
    // Find the last section of XBOXKRNL.EXE, which by convention should be the
    // discardable INIT section.
    //

    InitSectionHeader = IMAGE_FIRST_SECTION(NtHeader) +
        NtHeader->FileHeader.NumberOfSections - 1;

    ASSERT((*(PULONG)InitSectionHeader->Name) == 'TINI');
    ASSERT((InitSectionHeader->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) != 0);

    //
    // Insert the pages of the INIT section into the free lists.  The starting
    // page of the INIT section may be shared with other code or data but
    // everything on the ending page of the INIT section is discardable.
    //

    VirtualAddress = (PUCHAR)PsNtosImageBase + InitSectionHeader->VirtualAddress;
    EndingVirtualAddress = VirtualAddress + InitSectionHeader->SizeOfRawData;

#if DBG
    RtlFillMemory(VirtualAddress, InitSectionHeader->SizeOfRawData, 0xCC);
#endif

    VirtualAddress = (PUCHAR)PAGE_ALIGN((ULONG_PTR)VirtualAddress + PAGE_SIZE - 1);
    EndingVirtualAddress = (PUCHAR)PAGE_ALIGN((ULONG_PTR)EndingVirtualAddress + PAGE_SIZE - 1);

    PageFrameNumber = MI_CONVERT_PHYSICAL_TO_PFN(VirtualAddress);
    EndingPageFrameNumberExclusive = MI_CONVERT_PHYSICAL_TO_PFN(EndingVirtualAddress);

    MmTopOfKernelPhysicalAddress = MmGetPhysicalAddress(VirtualAddress);

    MI_LOCK_MM(&OldIrql);

    while (PageFrameNumber < EndingPageFrameNumberExclusive) {

        //
        // MmAllocateContiguousMemorySpecifyCache searches for free pages from
        // the highest page to the lowest page, so insert pages such that the
        // lowest pages are at the front of the free lists.  This avoids
        // unnecessary page relocation in order to satisfy a contiguous memory
        // allocation.
        //

        MmAllocatedPagesByUsage[MmContiguousUsage]--;

        MiInsertPageInFreeList(PageFrameNumber, FALSE);

        PageFrameNumber++;
    }

    MI_UNLOCK_MM(OldIrql);

    ASSERT(EndingVirtualAddress == (PUCHAR)PsNtosImageBase +
        ROUND_TO_PAGES(RtlImageNtHeader(PsNtosImageBase)->OptionalHeader.SizeOfImage));

    //
    // Flush the TLB now that we've cleared several page table entries.
    //

    KeFlushCurrentTb();
}

VOID
MmRelocatePersistentMemory(
    VOID
    )
/*++

Routine Description:

    This routine relocates the contiguous memory allocations used by the launch
    data page and the persisted frame buffer.  This is done in order to make the
    memory layout at entry to an Xbox executable consistent between various
    methods of invocation (cold boot, warm boot from title, warm boot from
    Dashboard, etc.).

Arguments:

    None.

Return Value:

    None.

Environment:

    Kernel mode.

--*/
{
    PLAUNCH_DATA_PAGE NewLaunchDataPage;
    PLAUNCH_DATA_PAGE OldLaunchDataPage;
    PVOID OldSurface;
    ULONG_PTR OldSurfacePhysicalAddress;
    ULONG NumberOfBytes;
    PVOID NewSurface;

    //
    // First, attempt to relocate the launch data page at the bottom of memory
    // in the region below the base of the kernel.  This region should be
    // available at this point unless we've collided with other persisted
    // contiguous memory.
    //

    OldLaunchDataPage = LaunchDataPage;

    if (OldLaunchDataPage != NULL) {

        NewLaunchDataPage = MmAllocateContiguousMemoryEx(PAGE_SIZE, 0,
            MmGetPhysicalAddress(PsNtosImageBase) - 1, 0, PAGE_READWRITE);

        if (NewLaunchDataPage != NULL) {

            RtlCopyMemory(NewLaunchDataPage, OldLaunchDataPage, PAGE_SIZE);
            MmPersistContiguousMemory(NewLaunchDataPage, PAGE_SIZE, TRUE);

            LaunchDataPage = NewLaunchDataPage;

            MmFreeContiguousMemory(OldLaunchDataPage);

        } else {
            MiDbgPrint(("MM: cannot relocate launch data page\n"));
        }
    }

    //
    // Second, attempt to relocate the frame buffer created by
    // D3DDevice::PersistDisplay at the location immediately above the kernel
    // image.  If no surface exists or the surface is already at the desired
    // address, then bail out now.
    //

    OldSurface = AvGetSavedDataAddress();

    if (OldSurface == NULL) {
        return;
    }

    OldSurfacePhysicalAddress = MmGetPhysicalAddress(OldSurface);

    if (OldSurfacePhysicalAddress == MmTopOfKernelPhysicalAddress) {
        return;
    }

    //
    // Check if the surface overlaps the target surface physical address.  If
    // so, then we'll try to first relocate the surface to a higher physical
    // address.  If this fails, we'll fall into the below code path which will
    // fail due to the existing allocation.
    //

    NumberOfBytes = MmQueryAllocationSize(OldSurface);

    if (OldSurfacePhysicalAddress < MmTopOfKernelPhysicalAddress + NumberOfBytes) {

        NewSurface = MmAllocateContiguousMemoryEx(NumberOfBytes,
            MmTopOfKernelPhysicalAddress, MAXULONG_PTR, 0, PAGE_READWRITE |
            PAGE_WRITECOMBINE);

        if (NewSurface != NULL) {
            AvRelocateSavedDataAddress(NewSurface, NumberOfBytes);
            OldSurface = NewSurface;
        }
    }

    //
    // Attempt to relocate the surface to the physical memory immediately above
    // the kernel image.
    //

    NewSurface = MmAllocateContiguousMemoryEx(NumberOfBytes,
        MmTopOfKernelPhysicalAddress, MmTopOfKernelPhysicalAddress +
        NumberOfBytes - 1, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);

    if (NewSurface != NULL) {
        AvRelocateSavedDataAddress(NewSurface, NumberOfBytes);
    } else {
        MiDbgPrint(("MM: cannot relocate persist display surface\n"));
    }
}

VOID
MmPrepareToQuickRebootSystem(
    VOID
    )
/*++

Routine Description:

    This routine is called by the HAL immediately before quick rebooting the
    system.

Arguments:

    None.

Return Value:

    None.

Environment:

    Kernel mode.

--*/
{
    ULONG PageFrameNumber;
    ULONG EndingPageFrameNumber;
    MMPFN TempPageFrame;
    MMPTE TempPte;
    PMMPTE PointerPde;

    //
    // Convert the page frames that hold NV2A instance memory to page table
    // entries.  When the memory manager reinitializes itself in the next
    // instance of the kernel, we want all of the pages that can be used for
    // instance memory to be reserved again until the D3D drivers calls
    // MmClaimGpuInstanceMemory.
    //

    PageFrameNumber = MM_INSTANCE_PHYSICAL_PAGE;
    EndingPageFrameNumber = MM_INSTANCE_PHYSICAL_PAGE + MM_INSTANCE_PAGE_COUNT;

    while (PageFrameNumber < EndingPageFrameNumber) {

        TempPageFrame.Pte.Long = MiGetValidKernelPteBits();
        MI_DISABLE_CACHING(TempPageFrame.Pte);
        TempPageFrame.Pte.Hard.PersistAllocation = 1;
        TempPageFrame.Pte.Hard.PageFrameNumber = PageFrameNumber;

        *MI_PFN_ELEMENT(PageFrameNumber) = TempPageFrame;

        PageFrameNumber++;
    }

#ifdef CONSOLE_DEVKIT
    //
    // On a development kit system, the NVIDIA NV2A instance memory must be
    // relative to the top of memory, so we also reserve the instance memory
    // pages in the upper half of memory.
    //

    if (MM_HIGHEST_PHYSICAL_PAGE != MM_64M_PHYSICAL_PAGE - 1) {

        PageFrameNumber = MM_INSTANCE_PHYSICAL_PAGE + MM_64M_PHYSICAL_PAGE;
        EndingPageFrameNumber = MM_DATABASE_PHYSICAL_PAGE + MM_64M_PHYSICAL_PAGE;

        while (PageFrameNumber < EndingPageFrameNumber) {

            TempPageFrame.Pte.Long = MiGetValidKernelPteBits();
            MI_DISABLE_CACHING(TempPageFrame.Pte);
            TempPageFrame.Pte.Hard.PersistAllocation = 1;
            TempPageFrame.Pte.Hard.PageFrameNumber = PageFrameNumber;

            *MI_PFN_ELEMENT(PageFrameNumber) = TempPageFrame;

            PageFrameNumber++;
        }
    }
#endif

    //
    // Unmap the lower two gigabytes of memory.
    //

    for (PointerPde = MiGetPdeAddress(0); PointerPde <
        MiGetPdeAddress(MM_PHYSICAL_MAP_BASE); PointerPde++) {
        MI_WRITE_ZERO_PTE(PointerPde);
    }

    //
    // Unmap the system PTE space.
    //

    for (PointerPde = MiGetPdeAddress(MM_SYSTEM_PTE_BASE); PointerPde <=
        MiGetPdeAddress(MM_SYSTEM_PTE_END); PointerPde++) {
        MI_WRITE_ZERO_PTE(PointerPde);
    }

    //
    // Switch back to large pages for the physical mapping window.  When the
    // memory manager reinitializes, the PFN database will be modified causing
    // the existing page tables to become invalid.  Also, this causes the ROM
    // shadow physical address to be visible for KeQuickRebootSystem.
    //

    TempPte.Long = MiGetValidKernelPteBits();
    TempPte.Hard.LargePage = 1;

    for (PointerPde = MiGetPdeAddress(MM_PHYSICAL_MAP_BASE); PointerPde <=
        MiGetPdeAddress(MM_PHYSICAL_MAP_END); PointerPde++) {
        MI_WRITE_PTE(PointerPde, TempPte);
        TempPte.Long += PAGE_SIZE_LARGE;
    }

    //
    // Map the boot ROM to its identity mapping at the top of memory.
    //

    TempPte.Long = MiGetValidKernelPteBits();
    TempPte.Hard.LargePage = 1;
    TempPte.Hard.PageFrameNumber = (ULONG_PTR)PAGE_ALIGN_LARGE(MAXULONG_PTR) >>
        PAGE_SHIFT;

    PointerPde = MiGetPdeAddress((PVOID)MAXULONG_PTR);
    MI_WRITE_PTE(PointerPde, TempPte);

    //
    // Flush the TLB now that the switch over is complete.
    //

    KeFlushCurrentTb();
}
