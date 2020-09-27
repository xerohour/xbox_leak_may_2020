/*++

Copyright (c) 1989-2002  Microsoft Corporation

Module Name:

    pfnsup.c

Abstract:

    This module implements routines which provide support for managing the page
    frame database.

--*/

#include "mi.h"

//
// Define a macro to statically initialize a PFN region.
//
#define MI_PFN_REGION_INITIALIZER() {                                         \
    {                                                                         \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
        { MM_PACKED_PFN_NULL, MM_PACKED_PFN_NULL },                           \
    },                                                                        \
    0,                                                                        \
}

//
// Total number of physical pages available on the system.
//
DECLSPEC_STICKY PFN_COUNT MmNumberOfPhysicalPages;

//
// Highest physical page number in the system.
//
#ifdef CONSOLE_DEVKIT
PFN_NUMBER MmHighestPhysicalPage;
#else
DECLSPEC_RDATA PFN_NUMBER MmHighestPhysicalPage = MM_HIGHEST_PHYSICAL_PAGE;
#endif

//
// Database that contains the use of each allocatable physical page in the
// system.  Exposed here for the kernel debugger.
//
DECLSPEC_SELECTANY PMMPFN MmPfnDatabase = MM_PFN_DATABASE;

//
// Region for general use pages, such as code, pool, and stacks.
//
MMPFNREGION MmRetailPfnRegion = MI_PFN_REGION_INITIALIZER();

#ifdef CONSOLE_DEVKIT
//
// Region for the extra memory available in the development kits.
//
MMPFNREGION MmDeveloperKitPfnRegion = MI_PFN_REGION_INITIALIZER();

//
// Mapping from MI_PFN_REGION to the corresponding PMMPFNREGION.
//
PMMPFNREGION MmPfnRegions[MI_NUMBER_OF_REGIONS] = {
    &MmRetailPfnRegion,
    &MmDeveloperKitPfnRegion,
    &MmDeveloperKitPfnRegion,
    &MmDeveloperKitPfnRegion,
};

//
// Set if MmReleaseDeveloperKitMemory has been called to place all of the extra
// pages available on a developer kit in the general free lists.
//
BOOLEAN MmDeveloperKitMemoryReleased;
#endif

//
// Page color to use for the next allocation without a specified page color.
//
MMCOLOR MmNextDefaultPageColor;

//
// Number of pages that can be allocated for general use.
//
PFN_COUNT MmAvailablePages;

//
// Number of pages that have been allocated for the various usage types.
//
PFN_COUNT MmAllocatedPagesByUsage[MmMaximumUsage];

//
// Local support.
//

VOID
MiRelocateBusyPageHelper(
    IN PVOID OldPhysicalAddress,
    IN PVOID NewPhysicalAddress,
    IN PMMPTE PointerPte,
    IN MMPTE NewPteContents,
    IN BOOLEAN FullTlbFlush
    );

#if DBG
VOID
MiAssertFreeListsValid(
    VOID
    );
#define MI_ASSERT_FREE_LISTS_VALID()    MiAssertFreeListsValid()
#else
#define MI_ASSERT_FREE_LISTS_VALID()
#endif

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, MiInitializePfnDatabase)
#endif

VOID
MiInitializePfnDatabase(
    VOID
    )
/*++

Routine Description:

    This routine initializes the database used to track busy and free pages.

Arguments:

    None.

Return Value:

    None.

Environment:

    Kernel mode.

--*/
{
    KIRQL OldIrql;
    PIMAGE_NT_HEADERS NtHeader;
    ULONG NtosImageSize;
    PFN_COUNT PfnDatabaseSizeInPages;
    MMPFN TempPageFrame;
    ULONG PageFrameNumber;
    ULONG EndingPageFrameNumber;
#ifdef DEVKIT
    PXDATA_SECTION_HEADER DataSectionHeader;
#endif
    PIMAGE_SECTION_HEADER TextSectionHeader;

    MI_LOCK_MM(&OldIrql);

    //
    // Verify that the highest physical page doesn't exceed the limits of the
    // physical mapping window.
    //

    ASSERT(MM_HIGHEST_PHYSICAL_PAGE < MM_PAGES_IN_PHYSICAL_MAP);

    //
    // Find the NT headers for XBOXKRNL.EXE.
    //

    NtHeader = RtlImageNtHeader(PsNtosImageBase);

    ASSERT(NtHeader != NULL);
    ASSERT(NtHeader->FileHeader.NumberOfSections > 0);

    //
    // Get the size of the XBOXKRNL.EXE image.
    //

    NtosImageSize = ROUND_TO_PAGES(NtHeader->OptionalHeader.SizeOfImage);

    //
    // Compute how many pages will be required to hold the PFN database.
    //

    PfnDatabaseSizeInPages = BYTES_TO_PAGES((MM_HIGHEST_PHYSICAL_PAGE + 1) *
        sizeof(MMPFN));

    //
    // Initialize all entries in the PFN database as unknown usage with a
    // maximum reference count so that MmAllocateContiguousMemoryEx doesn't
    // consider the page when finding a candidate range.
    //

    TempPageFrame.Long = 0;
    TempPageFrame.Busy.Busy = 1;
    TempPageFrame.Busy.LockCount = MI_LOCK_COUNT_MAXIMUM;
    TempPageFrame.Busy.BusyType = MmUnknownUsage;

    RtlFillMemoryUlong(MM_PFN_DATABASE, PfnDatabaseSizeInPages * PAGE_SIZE,
        TempPageFrame.Long);

    //
    // Insert all of the pages from the bottom of memory to the base of
    // XBOXKRNL.EXE into the free lists.
    //

    MiInsertPhysicalMemoryInFreeList(MM_LOWEST_PHYSICAL_PAGE,
        MI_CONVERT_PHYSICAL_TO_PFN(PsNtosImageBase));

#ifdef ARCADE
    //
    // Insert all of the pages from the end of XBOXKRNL.EXE to the base of the
    // PFN database into the free lists.
    //

    MiInsertPhysicalMemoryInFreeList(
        MI_CONVERT_PHYSICAL_TO_PFN((PUCHAR)PsNtosImageBase + NtosImageSize),
        MM_DATABASE_PHYSICAL_PAGE);
#else
    //
    // Insert all of the pages from the end of XBOXKRNL.EXE to the base of the
    // NVIDIA NV2A instance memory into the free lists.
    //

    MiInsertPhysicalMemoryInFreeList(
        MI_CONVERT_PHYSICAL_TO_PFN((PUCHAR)PsNtosImageBase + NtosImageSize),
        MM_INSTANCE_PHYSICAL_PAGE);
#endif

#ifdef CONSOLE_DEVKIT
    //
    // Insert all of the pages from the end of the PFN database to the top of
    // memory into the free lists.
    //

    MiInsertPhysicalMemoryInFreeList(MM_DATABASE_PHYSICAL_PAGE +
        PfnDatabaseSizeInPages, MM_HIGHEST_PHYSICAL_PAGE + 1);
#endif

#ifdef DEVKIT
    //
    // If we booted from a shadow copy of the ROM, then we need to remove the
    // pages that were used to make a copy of the kernel's initialized data.
    //

    if (XboxBootFlags & XBOX_BOOTFLAG_SHADOW) {

        DataSectionHeader = (PXDATA_SECTION_HEADER)((PIMAGE_DOS_HEADER)PsNtosImageBase)->e_res2;

        PageFrameNumber = MI_CONVERT_PHYSICAL_TO_PFN(DataSectionHeader->PointerToRawData);
        EndingPageFrameNumber = PageFrameNumber +
            BYTES_TO_PAGES(DataSectionHeader->SizeOfInitializedData);

        while (PageFrameNumber < EndingPageFrameNumber) {

            MiRemovePageFromFreeList(PageFrameNumber);

            TempPageFrame.Pte.Long = MiGetValidKernelPteBits();
            TempPageFrame.Pte.Hard.Write = 0;
            TempPageFrame.Pte.Hard.PersistAllocation = 1;
            TempPageFrame.Pte.Hard.PageFrameNumber = PageFrameNumber;

            *MI_PFN_ELEMENT(PageFrameNumber) = TempPageFrame;

            MmAllocatedPagesByUsage[MmContiguousUsage]++;

            PageFrameNumber++;
        }
    }
#endif

    //
    // Convert the page frame that holds the page directory to a page table
    // entry.
    //

    PageFrameNumber = MI_CONVERT_PHYSICAL_TO_PFN(PAGE_DIRECTORY_PHYSICAL_ADDRESS);

    MiRemovePageFromFreeList(PageFrameNumber);

    TempPageFrame.Pte.Long = MiGetValidKernelPteBits();
    TempPageFrame.Pte.Hard.PersistAllocation = 1;
    TempPageFrame.Pte.Hard.GuardOrEndOfAllocation = 1;
    TempPageFrame.Pte.Hard.PageFrameNumber = PageFrameNumber;;

    *MI_PFN_ELEMENT(PageFrameNumber) = TempPageFrame;

    MmAllocatedPagesByUsage[MmContiguousUsage]++;

    //
    // Convert the page frames that hold XBOXKRNL.EXE to page table entries.
    //

    PageFrameNumber = MI_CONVERT_PHYSICAL_TO_PFN(PsNtosImageBase);
    EndingPageFrameNumber = MI_CONVERT_PHYSICAL_TO_PFN((PUCHAR)PsNtosImageBase +
        NtosImageSize);

    while (PageFrameNumber < EndingPageFrameNumber) {

        TempPageFrame.Pte.Long = MiGetValidKernelPteBits();
        TempPageFrame.Pte.Hard.PersistAllocation = 1;
        TempPageFrame.Pte.Hard.PageFrameNumber = PageFrameNumber;

        *MI_PFN_ELEMENT(PageFrameNumber) = TempPageFrame;

        MmNumberOfPhysicalPages++;
        MmAllocatedPagesByUsage[MmContiguousUsage]++;

        PageFrameNumber++;
    }

    //
    // Find the first section of XBOXKRNL.EXE, which by convention should be the
    // text section.
    //

    TextSectionHeader = IMAGE_FIRST_SECTION(NtHeader);

    ASSERT((*(PULONG)TextSectionHeader->Name) == 'xet.');
    ASSERT((TextSectionHeader->Characteristics & IMAGE_SCN_CNT_CODE) != 0);

    //
    // Convert the image headers and .text section of XBOXKRNL.EXE to read-only.
    //

    PageFrameNumber = MI_CONVERT_PHYSICAL_TO_PFN(PsNtosImageBase);
    EndingPageFrameNumber = MI_CONVERT_PHYSICAL_TO_PFN((PUCHAR)PsNtosImageBase +
        TextSectionHeader->Misc.VirtualSize);

    while (PageFrameNumber < EndingPageFrameNumber) {
        MI_PFN_ELEMENT(PageFrameNumber)->Pte.Hard.Write = 0;
        PageFrameNumber++;
    }

    //
    // Convert the page frames that hold the PFN database to page table entries.
    // The pages persist across a quick reboot.
    //

    PageFrameNumber = MM_DATABASE_PHYSICAL_PAGE;
    EndingPageFrameNumber = MM_DATABASE_PHYSICAL_PAGE + PfnDatabaseSizeInPages;

    while (PageFrameNumber < EndingPageFrameNumber) {

        TempPageFrame.Pte.Long = MiGetValidKernelPteBits();
        TempPageFrame.Pte.Hard.PersistAllocation = 1;
        TempPageFrame.Pte.Hard.PageFrameNumber = PageFrameNumber;

        *MI_PFN_ELEMENT(PageFrameNumber) = TempPageFrame;

        MmNumberOfPhysicalPages++;
        MmAllocatedPagesByUsage[MmContiguousUsage]++;

        PageFrameNumber++;
    }

    //
    // Convert the page frames that hold NV2A instance memory to page table
    // entries.
    //

    PageFrameNumber = MM_INSTANCE_PHYSICAL_PAGE;
    EndingPageFrameNumber = MM_INSTANCE_PHYSICAL_PAGE + MM_INSTANCE_PAGE_COUNT;

    while (PageFrameNumber < EndingPageFrameNumber) {

        TempPageFrame.Pte.Long = MiGetValidKernelPteBits();
        MI_DISABLE_CACHING(TempPageFrame.Pte);
        TempPageFrame.Pte.Hard.PageFrameNumber = PageFrameNumber;

        *MI_PFN_ELEMENT(PageFrameNumber) = TempPageFrame;

        MmNumberOfPhysicalPages++;
        MmAllocatedPagesByUsage[MmContiguousUsage]++;

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
        EndingPageFrameNumber = MM_INSTANCE_PHYSICAL_PAGE +
            MM_INSTANCE_PAGE_COUNT + MM_64M_PHYSICAL_PAGE;

        while (PageFrameNumber < EndingPageFrameNumber) {

            MiRemovePageFromFreeList(PageFrameNumber);

            TempPageFrame.Pte.Long = MiGetValidKernelPteBits();
            MI_DISABLE_CACHING(TempPageFrame.Pte);
            TempPageFrame.Pte.Hard.PageFrameNumber = PageFrameNumber;

            *MI_PFN_ELEMENT(PageFrameNumber) = TempPageFrame;

            MmAllocatedPagesByUsage[MmContiguousUsage]++;

            PageFrameNumber++;
        }
    }
#endif

    //
    // Reserve the first physical page in the system for D3D's use.
    //

    MiRemovePageFromFreeList(MM_D3D_PHYSICAL_PAGE);

    TempPageFrame.Pte.Long = MiGetValidKernelPteBits();
    TempPageFrame.Pte.Hard.PersistAllocation = 1;
    TempPageFrame.Pte.Hard.GuardOrEndOfAllocation = 1;
    TempPageFrame.Pte.Hard.PageFrameNumber = MM_D3D_PHYSICAL_PAGE;

    *MI_PFN_ELEMENT(MM_D3D_PHYSICAL_PAGE) = TempPageFrame;

    MmAllocatedPagesByUsage[MmContiguousUsage]++;

    MI_UNLOCK_MM(OldIrql);
}

VOID
MiReinitializePfnDatabase(
    VOID
    )
/*++

Routine Description:

    This routine reinitializes the database used to track busy and free pages
    after a quick reboot has occurred.

Arguments:

    None.

Return Value:

    None.

Environment:

    Kernel mode.

--*/
{
    KIRQL OldIrql;
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;

    MI_LOCK_MM(&OldIrql);

    //
    // Loop over the page frame database and either keep persistent pages or
    // insert pages into the free lists.
    //

    for (PageFrameNumber = MM_LOWEST_PHYSICAL_PAGE; PageFrameNumber <=
        MM_HIGHEST_PHYSICAL_PAGE; PageFrameNumber++) {

        PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

        if (PageFrame->Pte.Hard.Valid != 0) {

            //
            // The page was a contiguous memory allocation in the previous
            // instance of the kernel.  If the page is marked as being
            // peristent, then keep the allocation alive.
            //

            if (PageFrame->Pte.Hard.PersistAllocation != 0) {
                MmAllocatedPagesByUsage[MmContiguousUsage]++;
                continue;
            }

        } else if (PageFrame->Busy.Busy != 0) {

            //
            // The page was marked as an unknown page type in the previous
            // instance of the kernel.  Keep the page in this state since we
            // can't manage the page.
            //

            if (PageFrame->Busy.BusyType == MmUnknownUsage) {
                continue;
            }

        } else {

            //
            // The page was free in the previous instance of the kernel.
            //
        }

        //
        // MmAllocateContiguousMemorySpecifyCache searches for free pages from
        // the highest page to the lowest page, so insert pages such that the
        // lowest pages are at the front of the free lists.  This avoids
        // unnecessary page relocation in order to satisfy a contiguous memory
        // allocation.
        //

        MiInsertPageInFreeList(PageFrameNumber, FALSE);
    }

    MI_UNLOCK_MM(OldIrql);
}

VOID
FASTCALL
MiInsertPageInFreeList(
    IN PFN_NUMBER PageFrameNumber,
    IN BOOLEAN InsertAtHeadList
    )
/*++

Routine Description:

    This routine inserts the supplied physical page into the appropriate free
    list.  No assumptions are made about the current status of the page.

Arguments:

    PageFrameNumber - Supplies the physical page frame to insert.

    InsertAtHeadList - If TRUE, insert the page at the front of the free lists,
        else insert the page at the end of the free lists.

Return Value:

    None.

Environment:

    Kernel mode, DISPATCH_LEVEL.

--*/
{
    MMCOLOR Color;
    USHORT PackedPfn;
    PMMPFN PageFrame;
    PMMPFNREGION PfnRegion;
    PMMPFN OldLinkPageFrame;

    MI_ASSERT_LOCK_MM();

    ASSERT(PageFrameNumber <= MM_HIGHEST_PHYSICAL_PAGE);

    Color = MiGetPfnColor(PageFrameNumber);
    PackedPfn = MiPackFreePfn(PageFrameNumber);

    //
    // Verify that the caller is not attempting to free the page frame number
    // used as the end of list marker.
    //

    ASSERT(PackedPfn != MM_PACKED_PFN_NULL);

    //
    // Copy the list head to the freed page frame.  We'll clear out the forward
    // or backward link below depending on the list insertion direction.
    //

    PageFrame = MI_PFN_ELEMENT(PageFrameNumber);
    PfnRegion = MI_PFN_REGION(PageFrameNumber);
    PageFrame->Free = PfnRegion->FreePagesByColor[Color];

    if (PfnRegion->FreePagesByColor[Color].PackedPfnFlink != MM_PACKED_PFN_NULL) {

        //
        // The page color list was not empty, so link the new page list head or
        // tail to the old page list head or tail.
        //

        if (InsertAtHeadList) {

            PageFrame->Free.PackedPfnBlink = MM_PACKED_PFN_NULL;
            PfnRegion->FreePagesByColor[Color].PackedPfnFlink = PackedPfn;

            OldLinkPageFrame = MiUnpackFreePfnElement(PageFrame->Free.PackedPfnFlink,
                Color);

            ASSERT(OldLinkPageFrame->Free.PackedPfnBlink == MM_PACKED_PFN_NULL);
            OldLinkPageFrame->Free.PackedPfnBlink = PackedPfn;

        } else {

            PageFrame->Free.PackedPfnFlink = MM_PACKED_PFN_NULL;
            PfnRegion->FreePagesByColor[Color].PackedPfnBlink = PackedPfn;

            OldLinkPageFrame = MiUnpackFreePfnElement(PageFrame->Free.PackedPfnBlink,
                Color);

            ASSERT(OldLinkPageFrame->Free.PackedPfnFlink == MM_PACKED_PFN_NULL);
            OldLinkPageFrame->Free.PackedPfnFlink = PackedPfn;
        }

        ASSERT(OldLinkPageFrame->Busy.Busy == 0);

    } else {

        //
        // The page color list was empty, so make both the head and tail
        // pointer reference this page.
        //

        ASSERT(PageFrame->Free.PackedPfnFlink == MM_PACKED_PFN_NULL);
        ASSERT(PageFrame->Free.PackedPfnBlink == MM_PACKED_PFN_NULL);

        PfnRegion->FreePagesByColor[Color].PackedPfnBlink = PackedPfn;
        PfnRegion->FreePagesByColor[Color].PackedPfnFlink = PackedPfn;
    }

    //
    // Verify that the packed page frame numbers are structurally correct-- the
    // low bits must be clear to mark the page as free.
    //

    ASSERT(PageFrame->Pte.Hard.Valid == 0);
    ASSERT(PageFrame->Busy.Busy == 0);

    //
    // Update the number of available pages in the region.
    //

    PfnRegion->AvailablePages++;

#ifdef CONSOLE_DEVKIT
    //
    // Update the number of available pages in the system if the page is from
    // the retail PFN region or if the extra memory on a development kit is
    // available for general use.
    //

    if (PfnRegion == &MmRetailPfnRegion || MmDeveloperKitMemoryReleased) {
        MmAvailablePages++;
    }
#else
    //
    // Update the number of available pages in the system.
    //

    MmAvailablePages++;
#endif
}

VOID
MiInsertPhysicalMemoryInFreeList(
    IN PFN_NUMBER PageFrameNumber,
    IN PFN_NUMBER EndingPageFrameNumberExclusive
    )
/*++

Routine Description:

    This routine inserts a range of physical pages into the appropriate free
    lists.  No assumptions are made about the current status of the pages.

Arguments:

    PageFrameNumber - Supplies the first physical page frame to insert.

    EndingPageFrameNumberExclusive - Supplies the last physical page frame to
                                     insert, not including the page itself.

Return Value:

    None.

Environment:

    Kernel mode.

--*/
{
    MI_ASSERT_LOCK_MM();

    while (PageFrameNumber < EndingPageFrameNumberExclusive) {

        //
        // MmAllocateContiguousMemorySpecifyCache searches for free pages from
        // the highest page to the lowest page, so insert pages such that the
        // lowest pages are at the front of the free lists.  This avoids
        // unnecessary page relocation in order to satisfy a contiguous memory
        // allocation.
        //

        MiInsertPageInFreeList(PageFrameNumber, FALSE);

        MmNumberOfPhysicalPages++;

        PageFrameNumber++;
    }
}

VOID
FASTCALL
MiRemovePageFromFreeList(
    IN PFN_NUMBER PageFrameNumber
    )
/*++

Routine Description:

    This routine removes the supplied physical page from the free page list
    that it is currently attached to.

    The returned page frame is not initialized.

Arguments:

    PageFrameNumber - Supplies the physical page frame to remove.

Return Value:

    None.

Environment:

    Kernel mode, DISPATCH_LEVEL.

--*/
{
    MMCOLOR Color;
    USHORT PackedPfn;
    PMMPFN PageFrame;
    PMMPFNREGION PfnRegion;
    PMMPFN LinkPageFrame;

    MI_ASSERT_LOCK_MM();

    ASSERT(PageFrameNumber <= MM_HIGHEST_PHYSICAL_PAGE);

    Color = MiGetPfnColor(PageFrameNumber);
    PackedPfn = MiPackFreePfn(PageFrameNumber);

    PageFrame = MI_PFN_ELEMENT(PageFrameNumber);
    PfnRegion = MI_PFN_REGION(PageFrameNumber);

    ASSERT(PageFrame->Pte.Hard.Valid == 0);
    ASSERT(PageFrame->Busy.Busy == 0);

    //
    // Detach the page from its backward link.
    //

    if (PageFrame->Free.PackedPfnBlink != MM_PACKED_PFN_NULL) {

        LinkPageFrame = MiUnpackFreePfnElement(PageFrame->Free.PackedPfnBlink,
            Color);

        ASSERT(LinkPageFrame->Pte.Hard.Valid == 0);
        ASSERT(LinkPageFrame->Busy.Busy == 0);
        ASSERT(LinkPageFrame->Free.PackedPfnFlink == PackedPfn);

        LinkPageFrame->Free.PackedPfnFlink = PageFrame->Free.PackedPfnFlink;

        ASSERT(LinkPageFrame->Busy.Busy == 0);

    } else {

        //
        // The page is the head of the list.
        //

        ASSERT(PfnRegion->FreePagesByColor[Color].PackedPfnFlink == PackedPfn);
        PfnRegion->FreePagesByColor[Color].PackedPfnFlink =
            PageFrame->Free.PackedPfnFlink;
    }

    //
    // Detach the page from its forward link.
    //

    if (PageFrame->Free.PackedPfnFlink != MM_PACKED_PFN_NULL) {

        LinkPageFrame = MiUnpackFreePfnElement(PageFrame->Free.PackedPfnFlink,
            Color);

        ASSERT(LinkPageFrame->Pte.Hard.Valid == 0);
        ASSERT(LinkPageFrame->Busy.Busy == 0);
        ASSERT(LinkPageFrame->Free.PackedPfnBlink == PackedPfn);

        LinkPageFrame->Free.PackedPfnBlink = PageFrame->Free.PackedPfnBlink;

        ASSERT(LinkPageFrame->Busy.Busy == 0);

    } else {

        //
        // The page is the tail of the list.
        //

        ASSERT(PfnRegion->FreePagesByColor[Color].PackedPfnBlink == PackedPfn);
        PfnRegion->FreePagesByColor[Color].PackedPfnBlink =
            PageFrame->Free.PackedPfnBlink;
    }

    //
    // Update the number of available pages in the region.
    //

    PfnRegion->AvailablePages--;

#ifdef CONSOLE_DEVKIT
    //
    // Update the number of available pages in the system if the page is from
    // the retail PFN region or if the extra memory on a development kit is
    // available for general use.
    //

    if (PfnRegion == &MmRetailPfnRegion || MmDeveloperKitMemoryReleased) {
        MmAvailablePages--;
    }
#else
    //
    // Update the number of available pages in the system.
    //

    MmAvailablePages--;
#endif
}

PFN_NUMBER
FASTCALL
MiRemoveAnyPageFromFreeList(
    VOID
    )
/*++

Routine Description:

    This routine removes any physical page from the free page lists.

    The returned page frame is not initialized.

Arguments:

    None.

Return Value:

    The allocated physical page frame number.

Environment:

    Kernel mode, DISPATCH_LEVEL.

--*/
{
    PMMPFNREGION PfnRegion;
    MMCOLOR Color;
    USHORT PackedPfn;
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;
    PMMPFN NewLinkPageFrame;

    MI_ASSERT_LOCK_MM();

    //
    // Verify that there are pages available.  The caller is responsible for
    // first checking that a page can be allocated.
    //

    ASSERT(MmAvailablePages > 0);

    //
    // Look for the first region with a free page.
    //

    PfnRegion = &MmRetailPfnRegion;

#ifdef CONSOLE_DEVKIT
    if (PfnRegion->AvailablePages == 0) {
        PfnRegion = &MmDeveloperKitPfnRegion;
        ASSERT(MmDeveloperKitMemoryReleased);
    }
#endif

    ASSERT(PfnRegion->AvailablePages > 0);

    //
    // Look for the first colored free list with a free page.
    //

    ASSERT(MmNextDefaultPageColor < MM_NUMBER_OF_COLORS);

    for (Color = MmNextDefaultPageColor; NOTHING;
        Color = ((Color + 1) & MM_NUMBER_OF_COLORS_MASK)) {

        if (PfnRegion->FreePagesByColor[Color].PackedPfnFlink !=
            MM_PACKED_PFN_NULL) {

            PackedPfn = PfnRegion->FreePagesByColor[Color].PackedPfnFlink;
            PageFrameNumber = MiUnpackFreePfn(PackedPfn, Color);

            ASSERT(PageFrameNumber <= MM_HIGHEST_PHYSICAL_PAGE);
            ASSERT(MI_PFN_REGION(PageFrameNumber) == PfnRegion);

            PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

            ASSERT(PageFrame->Pte.Hard.Valid == 0);
            ASSERT(PageFrame->Busy.Busy == 0);

            //
            // Unlink the page from the free list.
            //

            if (PageFrame->Free.PackedPfnFlink != MM_PACKED_PFN_NULL) {

                NewLinkPageFrame = MiUnpackFreePfnElement(PageFrame->Free.PackedPfnFlink,
                    Color);
                ASSERT(NewLinkPageFrame->Busy.Busy == 0);

                ASSERT(NewLinkPageFrame->Free.PackedPfnBlink ==
                    PfnRegion->FreePagesByColor[Color].PackedPfnFlink);
                NewLinkPageFrame->Free.PackedPfnBlink = MM_PACKED_PFN_NULL;

            } else {

                //
                // The page is the tail of the list.
                //

                ASSERT(PfnRegion->FreePagesByColor[Color].PackedPfnBlink == PackedPfn);
                PfnRegion->FreePagesByColor[Color].PackedPfnBlink = MM_PACKED_PFN_NULL;
            }

            ASSERT(PageFrame->Free.PackedPfnBlink == MM_PACKED_PFN_NULL);
            PfnRegion->FreePagesByColor[Color].PackedPfnFlink =
                PageFrame->Free.PackedPfnFlink;

            break;
        }
    }

    MmNextDefaultPageColor = ((Color + 1) & MM_NUMBER_OF_COLORS_MASK);

    //
    // Update the number of available pages in the region.
    //

    PfnRegion->AvailablePages--;

#ifdef CONSOLE_DEVKIT
    //
    // Update the number of available pages in the system if the page is from
    // the retail PFN region or if the extra memory on a development kit is
    // available for general use.
    //

    if (PfnRegion == &MmRetailPfnRegion || MmDeveloperKitMemoryReleased) {
        MmAvailablePages--;
    }
#else
    //
    // Update the number of available pages in the system.
    //

    MmAvailablePages--;
#endif

    return PageFrameNumber;
}

PFN_NUMBER
FASTCALL
MiRemoveAnyPage(
    IN MMPFN_BUSY_TYPE BusyType,
    IN PMMPTE TargetPte
    )
/*++

Routine Description:

    This routine removes one page from the free list and initializes it as a
    busy page with the supplied busy type.

    The contents of the page are uninitialized.

Arguments:

    BusyType - Usage code to place in allocated page frame.

    TargetPte - Supplies the address of the PDE/PTE that will receive the page
        frame.

Return Value:

    The allocated physical page frame number.

Environment:

    Kernel mode, DISPATCH_LEVEL.

--*/
{
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;

    ASSERT(BusyType < MmMaximumUsage);

    MI_ASSERT_LOCK_MM();

    //
    // Allocate any page from a free list.
    //

    PageFrameNumber = MiRemoveAnyPageFromFreeList();
    PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

    //
    // Initialize the page frame as busy.  Page tables created for the lower
    // half of the address space (user space) are always allocated via
    // MiRemoveZeroPage, so we don't need to handle that special case here.
    //

    ASSERT(BusyType != MmVirtualPageTableUsage);

    PageFrame->Long = 0;                // Also zeroes LockCount
    PageFrame->Busy.Busy = 1;
    PageFrame->Busy.BusyType = BusyType;

    PageFrame->Busy.PteIndex = MiGetPteOffset(MiGetVirtualAddressMappedByPte(TargetPte));

    //
    // Increment the number of pages of this type.
    //

    MmAllocatedPagesByUsage[BusyType]++;

    return PageFrameNumber;
}

PFN_NUMBER
FASTCALL
MiRemoveZeroPage(
    IN MMPFN_BUSY_TYPE BusyType,
    IN PMMPTE TargetPte
    )
/*++

Routine Description:

    This routine removes one page from the free list and initializes it as a
    busy page with the supplied busy type.

    The contents of the page are zeroed.

Arguments:

    BusyType - Usage code to place in allocated page frame.

    TargetPte - Supplies the address of the PDE/PTE that will receive the page
        frame.

Return Value:

    The allocated physical page frame number.

Environment:

    Kernel mode, DISPATCH_LEVEL.

--*/
{
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;
    MMPTE TempPte;

    ASSERT(BusyType < MmMaximumUsage);

    MI_ASSERT_LOCK_MM();

    //
    // Allocate any page from a free list.
    //

    PageFrameNumber = MiRemoveAnyPageFromFreeList();
    PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

    //
    // Zero the page's contents by temporarily exposing it through the system
    // RAM physical mapping.
    //
    // The below "nop" serves as a fake write barrier to ensure that
    // MI_WRITE_PTE is not optimized away because we end up changing PageFrame
    // below.
    //

    TempPte.Long = MiGetValidKernelPteBits();
    TempPte.Hard.PageFrameNumber = PageFrameNumber;

    MI_WRITE_PTE(&((volatile MMPFN*)PageFrame)->Pte, TempPte);

    __asm nop;

    RtlZeroMemory(MI_CONVERT_PFN_TO_PHYSICAL(PageFrameNumber), PAGE_SIZE);

    //
    // Initialize the page frame as busy.
    //

    PageFrame->Long = 0;                // Also zeroes LockCount
    PageFrame->Busy.Busy = 1;
    PageFrame->Busy.BusyType = BusyType;

    if (BusyType != MmVirtualPageTableUsage) {
        PageFrame->Busy.PteIndex =
            MiGetPteOffset(MiGetVirtualAddressMappedByPte(TargetPte));
    }

    //
    // Increment the number of pages of this type.
    //

    MmAllocatedPagesByUsage[BusyType]++;

    //
    // Flush the system RAM physical mapping created above.
    //

    MI_FLUSH_VA(MI_CONVERT_PFN_TO_PHYSICAL(PageFrameNumber));

    return PageFrameNumber;
}

VOID
FASTCALL
MiRelocateBusyPage(
    IN PFN_NUMBER PageFrameNumber
    )
/*++

Routine Description:

    This routine relocates the contents of the supplied physical page to another
    physical page.

    The original page frame is left in an uninitialized state.

Arguments:

    PageFrameNumber - Supplies the physical page frame to relocate.

Return Value:

    None.

Environment:

    Kernel mode, DISPATCH_LEVEL.

--*/
{
    PMMPFN PageFrame;
    BOOLEAN FullTlbFlush;
    PMMPTE PointerPte;
    PMMPTE PointerPde;
    PVOID CacheBuffer;
    PFN_NUMBER NewPageFrameNumber;
    PMMPFN NewPageFrame;
    MMPTE TempPte;

    MI_ASSERT_LOCK_MM();

    PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

    ASSERT(PageFrame->Pte.Hard.Valid == 0);
    ASSERT(PageFrame->Busy.Busy != 0);
    ASSERT(PageFrame->Busy.BusyType != MmUnknownUsage);
    ASSERT(PageFrame->Busy.LockCount == 0);

    //
    // Locate the pointer to the PTE for the page frame.
    //

    FullTlbFlush = FALSE;

    switch (PageFrame->Busy.BusyType) {

        case MmVirtualPageTableUsage:
            //
            // Busy.PteIndex isn't valid for a MmVirtualPageTableUsage page.
            // Instead, scan through the PDE for the page table.
            //

            PointerPde = MiGetPdeAddress(MM_LOWEST_USER_ADDRESS);

            for (;;) {

                if ((PointerPde->Hard.Valid != 0) &&
                    (PointerPde->Hard.PageFrameNumber == PageFrameNumber)) {
                    PointerPte = PointerPde;
                    break;
                }

                PointerPde++;

                //
                // Verify that we haven't run off the end of the page directory.
                //

                ASSERT(PointerPde <= MiGetPdeAddress(MM_HIGHEST_USER_ADDRESS));
            }

            FullTlbFlush = TRUE;

            break;

        case MmSystemPageTableUsage:
            //
            // Busy.PteIndex is valid for a MmSystemPageTableUsage page, but
            // it's relative to the page directory, not a page table.
            //

            PointerPte = (PMMPTE)PDE_BASE + PageFrame->Busy.PteIndex;

            ASSERT(PointerPte->Hard.Valid != 0);
            ASSERT(PointerPte->Hard.PageFrameNumber == PageFrameNumber);

            FullTlbFlush = TRUE;

            break;

        case MmFsCacheUsage:
            //
            // FsCache.ElementIndex is the index into the FscElementArray which
            // can be used to find the virtual address of the cache page.
            //

            ASSERT(PageFrame->FsCache.ElementIndex < FscNumberOfCachePages);
            CacheBuffer = PAGE_ALIGN(FscElementArray[PageFrame->FsCache.ElementIndex].CacheBuffer);
            PointerPte = MiGetPteAddress(CacheBuffer);
            break;

        case MmVirtualMemoryUsage:
        case MmImageUsage:
            //
            // Busy.PteIndex is valid for a MmVirtualMemoryUsage page.  Scan
            // through each page table in the application virtual memory space.
            //

            PointerPde = MiGetPdeAddress(MM_LOWEST_USER_ADDRESS);

            for (;;) {

                if (PointerPde->Hard.Valid != 0) {

                    ASSERT(PointerPde->Hard.LargePage == 0);

                    PointerPte = (PMMPTE)MiGetVirtualAddressMappedByPte(PointerPde) +
                        PageFrame->Busy.PteIndex;

                    //
                    // The page may be protected as PAGE_NOACCESS or PAGE_GUARD,
                    // so we can't just check the Valid bit here.
                    //

                    if ((PointerPte->Long != 0) &&
                        (PointerPte->Hard.PageFrameNumber == PageFrameNumber)) {
                        break;
                    }
                }

                PointerPde++;

                //
                // Verify that we haven't run off the end of the page directory.
                // If we hit this, then that means that a page either has an
                // invalid PTE index.
                //

                ASSERT(PointerPde <= MiGetPdeAddress(MM_HIGHEST_USER_ADDRESS));
            }
            break;

#ifdef DEVKIT
        case MmDebuggerUsage:
#ifdef CONSOLE_DEVKIT
            //
            // Relocatable pages only exist in the first 64 megabytes of memory
            // and debugger pages can only be allocated in this memory if the
            // system only has 64 megabytes of memory.
            //

            ASSERT(MM_HIGHEST_PHYSICAL_PAGE < MM_64M_PHYSICAL_PAGE);
#endif

            //
            // Busy.PteIndex is valid for a MmDebuggerUsage page.  Scan through
            // each page table in the development kit PTE space.
            //

            PointerPde = MiGetPdeAddress(MM_DEVKIT_PTE_BASE);

            for (;;) {

                if (PointerPde->Hard.Valid != 0) {

                    ASSERT(PointerPde->Hard.LargePage == 0);

                    PointerPte = (PMMPTE)MiGetVirtualAddressMappedByPte(PointerPde) +
                        PageFrame->Busy.PteIndex;

                    if ((PointerPte->Hard.Valid != 0) &&
                        (PointerPte->Hard.PageFrameNumber == PageFrameNumber)) {
                        break;
                    }
                }

                PointerPde++;

                //
                // Verify that we haven't run off the end of the page directory.
                // If we hit this, then that means that a page either has an
                // invalid PTE index.
                //

                ASSERT(PointerPde <= MiGetPdeAddress(MM_DEVKIT_PTE_END));
            }
            break;
#endif

        default:
#ifdef CONSOLE_DEVKIT
            //
            // For non-DEVKIT builds or builds with more than 64 megabytes of
            // memory, make sure that we don't hit a debugger page.
            //

            ASSERT(PageFrame->Busy.BusyType != MmDebuggerUsage);
#endif

            //
            // Busy.PteIndex is valid for all other types.  Scan through each
            // page table in the system PTE space.
            //

            PointerPde = MiGetPdeAddress(MM_SYSTEM_PTE_BASE);

            for (;;) {

                if (PointerPde->Hard.Valid != 0) {

                    ASSERT(PointerPde->Hard.LargePage == 0);

                    PointerPte = (PMMPTE)MiGetVirtualAddressMappedByPte(PointerPde) +
                        PageFrame->Busy.PteIndex;

                    if ((PointerPte->Hard.Valid != 0) &&
                        (PointerPte->Hard.PageFrameNumber == PageFrameNumber)) {
                        break;
                    }
                }

                PointerPde++;

                //
                // Verify that we haven't run off the end of the page directory.
                // If we hit this, then that means that a page either has an
                // invalid PTE index.
                //

                ASSERT(PointerPde <= MiGetPdeAddress(MM_SYSTEM_PTE_END));
            }
            break;
    }

    //
    // Allocate another page to hold the contents of the page.  Attempt to
    // obtain the same color page so that virtual buffers are still spaced
    // nicely in the cache.
    //

    MmNextDefaultPageColor = MiGetPfnColor(PageFrameNumber);
    NewPageFrameNumber = MiRemoveAnyPageFromFreeList();
    NewPageFrame = MI_PFN_ELEMENT(NewPageFrameNumber);

    //
    // Temporarily expose the new page through the system RAM physical mapping
    // so that we can copy the page from the old address to the new address.
    //

    TempPte.Long = MiGetValidKernelPteBits();
    TempPte.Hard.PageFrameNumber = NewPageFrameNumber;

    MI_WRITE_PTE(&((volatile MMPFN*)NewPageFrame)->Pte, TempPte);

    //
    // Build the PTE contents for the virtual address to point at the new page.
    //

    TempPte = *PointerPte;
    ASSERT(PointerPte->Hard.PageFrameNumber == PageFrameNumber);
    TempPte.Hard.PageFrameNumber = NewPageFrameNumber;

    //
    // Invoke a helper to copy the contents of the page from the old page to the
    // new page, to update the page tables, and to flush the appropriate TLB
    // entries.  This is done in an assembly routine in order to ensure the
    // stack is updated properly if we're relocating the current stack's pages.
    //

    MiRelocateBusyPageHelper(MiGetVirtualAddressMappedByPte(PointerPte),
        MI_CONVERT_PFN_TO_PHYSICAL(NewPageFrameNumber), PointerPte, TempPte,
        FullTlbFlush);

    //
    // Copy the page frame attributes from the old page frame to the new.
    //

    NewPageFrame->Long = PageFrame->Long;

    //
    // Flush the system RAM physical mapping created above.
    //

    MI_FLUSH_VA(MI_CONVERT_PFN_TO_PHYSICAL(NewPageFrameNumber));

    //
    // If caching is disabled or is write combined for the page's old virtual
    // address, then force a flush of the TLB and cache now to ensure coherency.
    //

    if (TempPte.Hard.WriteThrough) {
        KeFlushCurrentTbAndInvalidateAllCaches();
    }
}

VOID
FASTCALL
MiReleasePageOwnership(
    IN PFN_NUMBER PageFrameNumber
    )
/*++

Routine Description:

    This routine is called when the original use of an allocated page is
    complete.

Arguments:

    PageFrameNumber - Supplies the physical page frame to release.

Return Value:

    None.

Environment:

    Kernel mode, DISPATCH_LEVEL.

--*/
{
    PMMPFN PageFrame;

    MI_ASSERT_LOCK_MM();

    ASSERT(PageFrameNumber <= MM_HIGHEST_PHYSICAL_PAGE);

    PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

    ASSERT(PageFrame->Pte.Hard.Valid == 0);
    ASSERT(PageFrame->Busy.Busy != 0);
    ASSERT(PageFrame->Busy.BusyType != MmUnknownUsage);
    ASSERT(PageFrame->Busy.BusyType < MmMaximumUsage);

    //
    // If the page is still locked for I/O, then this is a bug in the client
    // code.  Pages should never be released with pending I/O.
    //

    ASSERT(PageFrame->Busy.LockCount == 0);

    //
    // Decrement the number of pages of this type.
    //

    MmAllocatedPagesByUsage[PageFrame->Busy.BusyType]--;

    //
    // Insert the page at the head of the free lists so that this page is
    // more likely to be allocated before a previously pinned page.
    //

    MiInsertPageInFreeList(PageFrameNumber, TRUE);
}

#ifdef CONSOLE_DEVKIT

VOID
MmReleaseDeveloperKitMemory(
    VOID
    )
/*++

Routine Description:

    This routine places all of the available pages in the extra memory PFN
    region back in the general available pages pool.  All of the other virtual
    memory manager services can start using these pages.

Arguments:

    None.

Return Value:

    None.

Environment:

    Kernel mode.

--*/
{
    KIRQL OldIrql;

    MI_LOCK_MM(&OldIrql);

    MmAvailablePages += MmDeveloperKitPfnRegion.AvailablePages;

    MmDeveloperKitMemoryReleased = TRUE;

    MI_UNLOCK_MM(OldIrql);
}

PFN_NUMBER
FASTCALL
MiRemoveDebuggerPage(
    IN MMPFN_BUSY_TYPE BusyType,
    IN PMMPTE TargetPte
    )
/*++

Routine Description:

    This routine removes one page from the free list and initializes it as a
    busy page with the supplied busy type.

    The contents of the page are zeroed.

Arguments:

    BusyType - Ignored.  MmDebuggerUsage is used for all pages allocated by
        this routine.

    TargetPte - Supplies the address of the PDE/PTE that will receive the page
        frame.

Return Value:

    The allocated physical page frame number.

Environment:

    Kernel mode, DISPATCH_LEVEL.

--*/
{
    MMCOLOR Color;
    USHORT PackedPfn;
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;
    MMPTE TempPte;

    MI_ASSERT_LOCK_MM();

    //
    // Verify that there are pages available.  The caller is responsible for
    // first checking that a page can be allocated.
    //

    ASSERT(MmDeveloperKitPfnRegion.AvailablePages > 0);

    //
    // Look for the first colored free list with a free page.
    //

    ASSERT(MmNextDefaultPageColor < MM_NUMBER_OF_COLORS);

    for (Color = MmNextDefaultPageColor; NOTHING;
        Color = ((Color + 1) & MM_NUMBER_OF_COLORS_MASK)) {

        if (MmDeveloperKitPfnRegion.FreePagesByColor[Color].PackedPfnFlink !=
            MM_PACKED_PFN_NULL) {

            PackedPfn = MmDeveloperKitPfnRegion.FreePagesByColor[Color].PackedPfnFlink;
            PageFrameNumber = MiUnpackFreePfn(PackedPfn, Color);

            //
            // MiRemoveAnyPage is optimized for speed and detaches the page from
            // the free list itself knowing that the page is at the head of the
            // list.  Here, we optimize for size and delegate the list
            // detachment to a common routine.
            //

            MiRemovePageFromFreeList(PageFrameNumber);

            break;
        }
    }

    MmNextDefaultPageColor = ((Color + 1) & MM_NUMBER_OF_COLORS_MASK);

    PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

    //
    // Zero the page's contents by temporarily exposing it through the system
    // RAM physical mapping.
    //
    // The below "nop" serves as a fake write barrier to ensure that
    // MI_WRITE_PTE is not optimized away because we end up changing PageFrame
    // below.
    //

    TempPte.Long = MiGetValidKernelPteBits();
    TempPte.Hard.PageFrameNumber = PageFrameNumber;

    MI_WRITE_PTE(&((volatile MMPFN*)PageFrame)->Pte, TempPte);

    __asm nop;

    RtlZeroMemory(MI_CONVERT_PFN_TO_PHYSICAL(PageFrameNumber), PAGE_SIZE);

    //
    // Initialize the page frame as busy.
    //

    PageFrame->Long = 0;                // Also zeroes LockCount
    PageFrame->Busy.Busy = 1;
    PageFrame->Busy.BusyType = MmDebuggerUsage;
    PageFrame->Busy.PteIndex =
        MiGetPteOffset(MiGetVirtualAddressMappedByPte(TargetPte));

    //
    // Increment the number of pages of this type.
    //

    MmAllocatedPagesByUsage[MmDebuggerUsage]++;

    //
    // Flush the system RAM physical mapping created above.
    //

    MI_FLUSH_VA(MI_CONVERT_PFN_TO_PHYSICAL(PageFrameNumber));

    return PageFrameNumber;
}

#endif

#if DBG

VOID
MiAssertPfnRegionFreeListsValid(
    IN PMMPFNREGION PfnRegion
    )
/*++

Routine Description:

    This routine analyzes the free lists of a PFN region to verify that they are
    structurally correct.

Arguments:

    PfnRegion - Supplies the PFN region to verify.

Return Value:

    None.

Environment:

    Kernel mode, DISPATCH_LEVEL.

--*/
{
    PFN_COUNT FreePagesFound;
    MMCOLOR Color;
    USHORT PackedPfnBlink;
    USHORT PackedPfn;
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;

    MI_ASSERT_LOCK_MM();

    FreePagesFound = 0;

    for (Color = 0; Color < MM_NUMBER_OF_COLORS; Color++) {

        PackedPfnBlink = MM_PACKED_PFN_NULL;
        PackedPfn = PfnRegion->FreePagesByColor[Color].PackedPfnFlink;

        while (PackedPfn != MM_PACKED_PFN_NULL) {

            ASSERT((PackedPfn & 1) == 0);

            PageFrameNumber = MiUnpackFreePfn(PackedPfn, Color);

            ASSERT(PageFrameNumber <= MM_HIGHEST_PHYSICAL_PAGE);
            ASSERT(MI_PFN_REGION(PageFrameNumber) == PfnRegion);

            PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

            ASSERT(PageFrame->Pte.Hard.Valid == 0);
            ASSERT(PageFrame->Busy.Busy == 0);
            ASSERT(PageFrame->Free.PackedPfnBlink == PackedPfnBlink);

            FreePagesFound++;

            PackedPfnBlink = PackedPfn;
            PackedPfn = PageFrame->Free.PackedPfnFlink;

            ASSERT(FreePagesFound <= MmAvailablePages);
        }

        ASSERT(PfnRegion->FreePagesByColor[Color].PackedPfnBlink == PackedPfnBlink);
    }

    ASSERT(PfnRegion->AvailablePages == FreePagesFound);
}

VOID
MiAssertFreeListsValid(
    VOID
    )
/*++

Routine Description:

    This routine analyzes the free lists to verify that they are structurally
    correct.

Arguments:

    None.

Return Value:

    None.

Environment:

    Kernel mode, DISPATCH_LEVEL.

--*/
{
    MiAssertPfnRegionFreeListsValid(&MmRetailPfnRegion);
#ifdef CONSOLE_DEVKIT
    MiAssertPfnRegionFreeListsValid(&MmDeveloperKitPfnRegion);
#endif
}

#endif // DBG
