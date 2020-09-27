/*++

Copyright (c) 1997  Microsoft Corporation

Module Name:

    gart.c

Abstract:

    Routines for querying and setting the Intel 440xx GART aperture

Author:

    John Vert (jvert) 10/30/1997

Revision History:

--*/

#include "videoprt.h"

#if 0 // NUGOOP

//
// Local function prototypes
//
NTSTATUS
Agp440CreateGart(
    IN PAGP440_EXTENSION AgpContext,
    IN ULONG MinimumPages
    );

PGART_PTE
Agp440FindRangeInGart(
    IN PGART_PTE StartPte,
    IN PGART_PTE EndPte,
    IN ULONG Length,
    IN BOOLEAN SearchBackward,
    IN ULONG SearchState
    );

VOID
Agp440SetGTLB_Enable(
    IN PAGP440_EXTENSION AgpContext,
    IN BOOLEAN Enable
    );

#define Agp440EnableTB(_x_) Agp440SetGTLB_Enable((_x_), TRUE)
#define Agp440DisableTB(_x_) Agp440SetGTLB_Enable((_x_), FALSE)


NTSTATUS
AgpQueryAperture(
    IN PAGP440_EXTENSION AgpContext,
    OUT PHYSICAL_ADDRESS *CurrentBase,
    OUT ULONG *CurrentSizeInPages,
    OUT OPTIONAL PIO_RESOURCE_LIST *pApertureRequirements
    )
/*++

Routine Description:

    Queries the current size of the GART aperture. Optionally returns
    the possible GART settings.

Arguments:

    AgpContext - Supplies the AGP context.

    CurrentBase - Returns the current physical address of the GART.

    CurrentSizeInPages - Returns the current GART size.

    ApertureRequirements - if present, returns the possible GART settings

Return Value:

    NTSTATUS

--*/

{
    ULONG ApBase;
    UCHAR ApSize;
    PIO_RESOURCE_LIST Requirements;
    ULONG i;
    ULONG Length;

    PAGED_CODE();
    //
    // Get the current APBASE and APSIZE settings
    //
    Read440Config(&ApBase, APBASE_OFFSET, sizeof(ApBase));
    Read440Config(&ApSize, APSIZE_OFFSET, sizeof(ApSize));

    ASSERT(ApBase != 0);
    CurrentBase->QuadPart = ApBase & PCI_ADDRESS_MEMORY_ADDRESS_MASK;

    //
    // Convert APSIZE into the actual size of the aperture
    //
    switch (ApSize) {
        case AP_SIZE_4MB:
            *CurrentSizeInPages = 4 * (1024*1024 / PAGE_SIZE);
            break;
        case AP_SIZE_8MB:
            *CurrentSizeInPages = 8 * (1024*1024 / PAGE_SIZE);
            break;
        case AP_SIZE_16MB:
            *CurrentSizeInPages = 16 * (1024*1024 / PAGE_SIZE);
            break;
        case AP_SIZE_32MB:
            *CurrentSizeInPages = 32 * (1024*1024 / PAGE_SIZE);
            break;
        case AP_SIZE_64MB:
            *CurrentSizeInPages = 64 * (1024*1024 / PAGE_SIZE);
            break;
        case AP_SIZE_128MB:
            *CurrentSizeInPages = 128 * (1024*1024 / PAGE_SIZE);
            break;
        case AP_SIZE_256MB:
            *CurrentSizeInPages = 256 * (1024*1024 / PAGE_SIZE);
            break;

        default:
            AGPLOG(AGP_CRITICAL,
                   ("AGP440 - AgpQueryAperture - Unexpected value %x for ApSize!\n",
                    ApSize));
            ASSERT(FALSE);
            AgpContext->ApertureStart.QuadPart = 0;
            AgpContext->ApertureLength = 0;
            return(STATUS_UNSUCCESSFUL);
    }

    //
    // Remember the current aperture settings
    //
    AgpContext->ApertureStart.QuadPart = CurrentBase->QuadPart;
    AgpContext->ApertureLength = *CurrentSizeInPages * PAGE_SIZE;

    if (pApertureRequirements != NULL) {
        //
        // 440 supports 7 different aperture sizes, all must be 
        // naturally aligned. Start with the largest aperture and 
        // work downwards so that we get the biggest possible aperture.
        //
        Requirements = ExAllocatePoolWithTag(PagedPool,
                                             sizeof(IO_RESOURCE_LIST) + (AP_SIZE_COUNT-1)*sizeof(IO_RESOURCE_DESCRIPTOR),
                                             'RpgA');
        if (Requirements == NULL) {
            return(STATUS_INSUFFICIENT_RESOURCES);
        }
        Requirements->Version = Requirements->Revision = 1;
        Requirements->Count = AP_SIZE_COUNT;
        Length = AP_MAX_SIZE;
        for (i=0; i<AP_SIZE_COUNT; i++) {
            Requirements->Descriptors[i].Option = IO_RESOURCE_ALTERNATIVE;
            Requirements->Descriptors[i].Type = CmResourceTypeMemory;
            Requirements->Descriptors[i].ShareDisposition = CmResourceShareDeviceExclusive;
            Requirements->Descriptors[i].Flags = CM_RESOURCE_MEMORY_READ_WRITE | CM_RESOURCE_MEMORY_PREFETCHABLE;

            Requirements->Descriptors[i].u.Memory.Length = Length;
            Requirements->Descriptors[i].u.Memory.Alignment = Length;
            Requirements->Descriptors[i].u.Memory.MinimumAddress.QuadPart = 0;
            Requirements->Descriptors[i].u.Memory.MaximumAddress.QuadPart = (ULONG)-1;

            Length = Length/2;
        }
        *pApertureRequirements = Requirements;


    }
    return(STATUS_SUCCESS);
}


NTSTATUS
AgpSetAperture(
    IN PAGP440_EXTENSION AgpContext,
    IN PHYSICAL_ADDRESS NewBase,
    IN ULONG NewSizeInPages
    )
/*++

Routine Description:

    Sets the GART aperture to the supplied settings

Arguments:

    AgpContext - Supplies the AGP context

    NewBase - Supplies the new physical memory base for the GART.

    NewSizeInPages - Supplies the new size for the GART

Return Value:

    NTSTATUS

--*/

{
    PACCFG PACConfig;
    UCHAR ApSize;
    ULONG ApBase;

    //
    // Figure out the new APSIZE setting, make sure it is valid.
    //
    switch (NewSizeInPages) {
        case 4 * 1024 * 1024 / PAGE_SIZE:
            ApSize = AP_SIZE_4MB;
            break;
        case 8 * 1024 * 1024 / PAGE_SIZE:
            ApSize = AP_SIZE_8MB;
            break;
        case 16 * 1024 * 1024 / PAGE_SIZE:
            ApSize = AP_SIZE_16MB;
            break;
        case 32 * 1024 * 1024 / PAGE_SIZE:
            ApSize = AP_SIZE_32MB;
            break;
        case 64 * 1024 * 1024 / PAGE_SIZE:
            ApSize = AP_SIZE_64MB;
            break;
        case 128 * 1024 * 1024 / PAGE_SIZE:
            ApSize = AP_SIZE_128MB;
            break;
        case 256 * 1024 * 1024 / PAGE_SIZE:
            ApSize = AP_SIZE_256MB;
            break;
        default:
            AGPLOG(AGP_CRITICAL,
                   ("AgpSetAperture - invalid GART size of %lx pages specified, aperture at %I64X.\n",
                    NewSizeInPages,
                    NewBase.QuadPart));
            ASSERT(FALSE);
            return(STATUS_INVALID_PARAMETER);
    }

    //
    // Make sure the supplied size is aligned on the appropriate boundary.
    //
    ASSERT(NewBase.HighPart == 0);
    ASSERT((NewBase.QuadPart & ((NewSizeInPages * PAGE_SIZE) - 1)) == 0);
    if ((NewBase.QuadPart & ((NewSizeInPages * PAGE_SIZE) - 1)) != 0 ) {
        AGPLOG(AGP_CRITICAL,
               ("AgpSetAperture - invalid base %I64X specified for GART aperture of %lx pages\n",
               NewBase.QuadPart,
               NewSizeInPages));
        return(STATUS_INVALID_PARAMETER);
    }

    //
    // Need to reset the hardware to match the supplied settings
    //
    // If the aperture is enabled, disable it, write the new settings, then reenable the aperture
    //
    Read440Config(&PACConfig, PACCFG_OFFSET, sizeof(PACConfig));
    PACConfig.GlobalEnable = 0;
    Write440Config(&PACConfig, PACCFG_OFFSET, sizeof(PACConfig));

    //
    // Write APSIZE first, as this will enable the correct bits in APBASE that need to
    // be written next.
    //
    Write440Config(&ApSize, APSIZE_OFFSET, sizeof(ApSize));

    //
    // Now we can update APBASE
    //
    ApBase = NewBase.LowPart & PCI_ADDRESS_MEMORY_ADDRESS_MASK;
    Write440Config(&ApBase, APBASE_OFFSET, sizeof(ApBase));

#if DBG
    //
    // Read back what we wrote, make sure it worked
    //
    {
        ULONG DbgBase;
        UCHAR DbgSize;

        Read440Config(&DbgSize, APSIZE_OFFSET, sizeof(ApSize));
        Read440Config(&DbgBase, APBASE_OFFSET, sizeof(ApBase));
        ASSERT(DbgSize == ApSize);
        ASSERT((DbgBase & PCI_ADDRESS_MEMORY_ADDRESS_MASK) == ApBase);
    }
#endif

    //
    // Now enable the aperture if it was enabled before
    //
    if (AgpContext->GlobalEnable) {
        Read440Config(&PACConfig, PACCFG_OFFSET, sizeof(PACConfig));
        ASSERT(PACConfig.GlobalEnable == 0);
        PACConfig.GlobalEnable = 1;
        Write440Config(&PACConfig, PACCFG_OFFSET, sizeof(PACConfig));
    }

    //
    // Update our extension to reflect the new GART setting
    //
    AgpContext->ApertureStart = NewBase;
    AgpContext->ApertureLength = NewSizeInPages * PAGE_SIZE;

    //
    // Enable the TB in case we are resuming from S3 or S4
    //
    Agp440EnableTB(AgpContext);

    //
    // If the GART has been allocated, rewrite the ATTBASE
    //
    if (AgpContext->Gart != NULL) {
        Write440Config(&AgpContext->GartPhysical.LowPart,
                       ATTBASE_OFFSET,
                       sizeof(AgpContext->GartPhysical.LowPart));
    }

    return(STATUS_SUCCESS);
}


NTSTATUS
AgpReserveMemory(
    IN PAGP440_EXTENSION AgpContext,
    IN OUT AGP_RANGE *Range
    )
/*++

Routine Description:

    Reserves a range of memory in the GART.

Arguments:

    AgpContext - Supplies the AGP Context

    Range - Supplies the AGP_RANGE structure. AGPLIB
        will have filled in NumberOfPages and Type. This
        routine will fill in MemoryBase and Context.

Return Value:

    NTSTATUS

--*/

{
    ULONG Index;
    ULONG NewState;
    NTSTATUS Status;
    PGART_PTE FoundRange;
    BOOLEAN Backwards;

    PAGED_CODE();

    ASSERT((Range->Type == MmNonCached) || (Range->Type == MmWriteCombined));
    ASSERT(Range->NumberOfPages <= (AgpContext->ApertureLength / PAGE_SIZE));

    //
    // If we have not allocated our GART yet, now is the time to do so
    //
    if (AgpContext->Gart == NULL) {
        ASSERT(AgpContext->GartLength == 0);
        Status = Agp440CreateGart(AgpContext,Range->NumberOfPages);
        if (!NT_SUCCESS(Status)) {
            AGPLOG(AGP_CRITICAL,
                   ("Agp440CreateGart failed %08lx to create GART of size %lx\n",
                    Status,
                    AgpContext->ApertureLength));
            return(Status);
        }
    }
    ASSERT(AgpContext->GartLength != 0);

    //
    // Now that we have a GART, try and find enough contiguous entries to satisfy
    // the request. Requests for uncached memory will scan from high addresses to
    // low addresses. Requests for write-combined memory will scan from low addresses
    // to high addresses. We will use a first-fit algorithm to try and keep the allocations
    // packed and contiguous.
    //
    Backwards = (BOOLEAN)((Range->Type == MmNonCached) ? TRUE : FALSE);
    FoundRange = Agp440FindRangeInGart(&AgpContext->Gart[0],
                                       &AgpContext->Gart[(AgpContext->GartLength / sizeof(GART_PTE)) - 1],
                                       Range->NumberOfPages,
                                       Backwards,
                                       GART_ENTRY_FREE);

    if (FoundRange == NULL) {
        //
        // A big enough chunk was not found.
        //
        AGPLOG(AGP_CRITICAL,
               ("AgpReserveMemory - Could not find %d contiguous free pages of type %d in GART at %08lx\n",
                Range->NumberOfPages,
                Range->Type,
                AgpContext->Gart));

// @@BEGIN_DDKSPLIT
        //
        // BUGBUG John Vert (jvert) 11/4/1997
        //  This is the point where we should try and grow the GART
        //
// @@END_DDKSPLIT

        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    AGPLOG(AGP_NOISE,
           ("AgpReserveMemory - reserved %d pages at GART PTE %08lx\n",
            Range->NumberOfPages,
            FoundRange));

    //
    // Set these pages to reserved
    //
    if (Range->Type == MmNonCached) {
        NewState = GART_ENTRY_RESERVED_UC;
    } else {
        NewState = GART_ENTRY_RESERVED_WC;
    }

    for (Index = 0;Index < Range->NumberOfPages; Index++) {
        ASSERT(FoundRange[Index].Soft.State == GART_ENTRY_FREE);
        FoundRange[Index].AsUlong = 0;
        FoundRange[Index].Soft.State = NewState;
    }

    Range->MemoryBase.QuadPart = AgpContext->ApertureStart.QuadPart + (FoundRange - &AgpContext->Gart[0]) * PAGE_SIZE;
    Range->Context = FoundRange;

    ASSERT(Range->MemoryBase.HighPart == 0);
    AGPLOG(AGP_NOISE,
           ("AgpReserveMemory - reserved memory handle %lx at PA %08lx\n",
            FoundRange,
            Range->MemoryBase.LowPart));

    return(STATUS_SUCCESS);
}


NTSTATUS
AgpReleaseMemory(
    IN PAGP440_EXTENSION AgpContext,
    IN PAGP_RANGE Range
    )
/*++

Routine Description:

    Releases memory previously reserved with AgpReserveMemory

Arguments:

    AgpContext - Supplies the AGP context

    AgpRange - Supplies the range to be released.

Return Value:

    NTSTATUS

--*/

{
    PGART_PTE Pte;

    PAGED_CODE();

    //
    // Go through and free all the PTEs. None of these should still
    // be valid at this point.
    //
    for (Pte = Range->Context;
         Pte < (PGART_PTE)Range->Context + Range->NumberOfPages;
         Pte++) {
        if (Range->Type == MmNonCached) {
            ASSERT(Pte->Soft.State == GART_ENTRY_RESERVED_UC);
        } else {
            ASSERT(Pte->Soft.State == GART_ENTRY_RESERVED_WC);
        }
        Pte->Soft.State = GART_ENTRY_FREE;
    }

    Range->MemoryBase.QuadPart = 0;
    return(STATUS_SUCCESS);
}


NTSTATUS
Agp440CreateGart(
    IN PAGP440_EXTENSION AgpContext,
    IN ULONG MinimumPages
    )
/*++

Routine Description:

    Allocates and initializes an empty GART. The current implementation
    attempts to allocate the entire GART on the first reserve.

Arguments:

    AgpContext - Supplies the AGP context

    MinimumPages - Supplies the minimum size (in pages) of the GART to be
        created.

Return Value:

    NTSTATUS

--*/

{
    PGART_PTE GartCached;
    PGART_PTE Gart;
    ULONG GartLength;
    PHYSICAL_ADDRESS HighestAcceptable;
    PHYSICAL_ADDRESS GartPhysical;
    ULONG i;

    PAGED_CODE();

    //
    // Try and get a chunk of contiguous memory big enough to map the
    // entire aperture.
    //
    GartLength = BYTES_TO_PAGES(AgpContext->ApertureLength) * sizeof(GART_PTE);

// @@BEGIN_DDKSPLIT
    //
    // BUGBUG John Vert (jvert) 11/5/1997
    //   This is a crock. We really need to use AllocateCommonBuffer here to
    //   get a non-cached contiguous range. Unfortunately, AllocateCommonBuffer
    //   does not actually allocate the memory non-cached. Until this is fixed,
    //   we have this hack-o-rama:
    //      MmAllocateContiguousMemory
    //      MmGetPhysicalAddress
    //      MmMapIoSpace
    //
// @@END_DDKSPLIT

    GartCached = MmAllocateContiguousMemory(GartLength);
    if (GartCached == NULL) {
        AGPLOG(AGP_CRITICAL,
               ("Agp440CreateGart - MmAllocateContiguousMemory %lx failed\n",
                GartLength));
        return(STATUS_INSUFFICIENT_RESOURCES);
    }
    //
    // We successfully allocated a contiguous chunk of memory.
    // It should be page aligned already.
    //
    ASSERT(((ULONG_PTR)GartCached & (PAGE_SIZE-1)) == 0);

    //
    // Get the physical address.
    //
    GartPhysical.QuadPart = MmGetPhysicalAddress(GartCached);
    AGPLOG(AGP_NOISE,
           ("Agp440CreateGart - GART of length %lx created at VA %08lx, PA %08lx\n",
            GartLength,
            GartCached,
            GartPhysical.LowPart));
    ASSERT(GartPhysical.HighPart == 0);
    ASSERT((GartPhysical.LowPart & (PAGE_SIZE-1)) == 0);

    //
    // Now we need to map this again to get a noncached pointer
    //
    Gart = MmMapIoSpace(GartPhysical.LowPart,
                        GartLength,
                        MmNonCached);
    if (Gart == NULL) {
        AGPLOG(AGP_CRITICAL,
               ("Agp440CreateGart - couldn't map GART at %08lx non-cached\n",
                GartCached));
    }

    //
    // Initialize all the PTEs to free
    //
    for (i=0; i<GartLength/sizeof(GART_PTE); i++) {
        Gart[i].Soft.State = GART_ENTRY_FREE;
    }


    Write440Config(&GartPhysical.LowPart, ATTBASE_OFFSET, sizeof(GartPhysical.LowPart));

    //
    // Update our extension to reflect the current state.
    //
    AgpContext->GartCached = GartCached;
    AgpContext->Gart = Gart;
    AgpContext->GartLength = GartLength;
    AgpContext->GartPhysical = GartPhysical;

    return(STATUS_SUCCESS);
}


NTSTATUS
AgpMapMemory(
    IN PAGP440_EXTENSION AgpContext,
    IN PAGP_RANGE Range,
    IN PMDL Mdl,
    IN ULONG OffsetInPages,
    OUT PHYSICAL_ADDRESS *MemoryBase
    )
/*++

Routine Description:

    Maps physical memory into the GART somewhere in the specified range.

Arguments:

    AgpContext - Supplies the AGP context

    Range - Supplies the AGP range that the memory should be mapped into

    Mdl - Supplies the MDL describing the physical pages to be mapped

    OffsetInPages - Supplies the offset into the reserved range where the 
        mapping should begin.

    MemoryBase - Returns the physical memory in the aperture where the pages
        were mapped.

Return Value:

    NTSTATUS

--*/

{
    ULONG PageCount;
    PGART_PTE Pte;
    PGART_PTE StartPte;
    ULONG Index;
    ULONG TargetState;
    PPFN_NUMBER Page;
    GART_PTE NewPte;
    PACCFG PACConfig;

    PAGED_CODE();

    ASSERT(Mdl->Next == NULL);

    StartPte = Range->Context;
    PageCount = BYTES_TO_PAGES(Mdl->ByteCount);
    ASSERT(PageCount <= Range->NumberOfPages);
    ASSERT(OffsetInPages <= Range->NumberOfPages);
    ASSERT(PageCount + OffsetInPages <= Range->NumberOfPages);
    ASSERT(PageCount > 0);

    TargetState = (Range->Type == MmNonCached) ? GART_ENTRY_RESERVED_UC : GART_ENTRY_RESERVED_WC;

    Pte = StartPte + OffsetInPages;

    //
    // We have a suitable range, now fill it in with the supplied MDL.
    //
    ASSERT(Pte >= StartPte);
    ASSERT(Pte + PageCount <= StartPte + Range->NumberOfPages);
    NewPte.AsUlong = 0;
    NewPte.Soft.State = (Range->Type == MmNonCached) ? GART_ENTRY_VALID_UC :
                                                       GART_ENTRY_VALID_WC;
    Page = (PPFN_NUMBER)(Mdl + 1);

    //
    // Disable the TB as per the 440 spec. This is probably unnecessary
    // as there should be no valid entries in this range, and there should
    // be no invalid entries still in the TB. So flushing the TB seems
    // a little gratuitous but that's what the 440 spec says to do.
    //
    Agp440DisableTB(AgpContext);

    for (Index = 0; Index < PageCount; Index++) {
        ASSERT(Pte[Index].Soft.State == TargetState);

        NewPte.Hard.Page = (ULONG)(*Page++);
        Pte[Index].AsUlong = NewPte.AsUlong;
        ASSERT(Pte[Index].Hard.Valid == 1);
    }

    //
    // We have filled in all the PTEs. Read back the last one we wrote
    // in order to flush the write buffers.
    //
    NewPte.AsUlong = *(volatile ULONG *)&Pte[PageCount-1].AsUlong;

    //
    // Re-enable the TB
    //
    Agp440EnableTB(AgpContext);

    //
    // If we have not yet gotten around to enabling the GART aperture, do it now.
    //
    if (!AgpContext->GlobalEnable) {
        AGPLOG(AGP_NOISE,
               ("AgpMapMemory - Enabling global aperture access\n"));

        Read440Config(&PACConfig, PACCFG_OFFSET, sizeof(PACConfig));
        PACConfig.GlobalEnable = 1;
        Write440Config(&PACConfig, PACCFG_OFFSET, sizeof(PACConfig));

        AgpContext->GlobalEnable = TRUE;
    }

    MemoryBase->QuadPart = Range->MemoryBase.QuadPart + (Pte - StartPte) * PAGE_SIZE;

    return(STATUS_SUCCESS);
}


NTSTATUS
AgpUnMapMemory(
    IN PAGP440_EXTENSION AgpContext,
    IN PAGP_RANGE AgpRange,
    IN ULONG NumberOfPages,
    IN ULONG OffsetInPages
    )
/*++

Routine Description:

    Unmaps previously mapped memory in the GART.

Arguments:

    AgpContext - Supplies the AGP context

    AgpRange - Supplies the AGP range that the memory should be freed from

    NumberOfPages - Supplies the number of pages in the range to be freed.

    OffsetInPages - Supplies the offset into the range where the freeing should begin.

Return Value:

    NTSTATUS

--*/

{
    ULONG i;
    PGART_PTE Pte;
    PGART_PTE LastChanged=NULL;
    PGART_PTE StartPte;
    ULONG NewState;

    PAGED_CODE();

    ASSERT(OffsetInPages + NumberOfPages <= AgpRange->NumberOfPages);

    StartPte = AgpRange->Context;
    Pte = &StartPte[OffsetInPages];

    if (AgpRange->Type == MmNonCached) {
        NewState = GART_ENTRY_RESERVED_UC;
    } else {
        NewState = GART_ENTRY_RESERVED_WC;
    }

    //
    // Disable the TB to flush it
    //
    Agp440DisableTB(AgpContext);
    for (i=0; i<NumberOfPages; i++) {
        if (Pte[i].Hard.Valid) {
            Pte[i].Soft.State = NewState;
            LastChanged = Pte;
        } else {
            //
            // This page is not mapped, just skip it.
            //
            AGPLOG(AGP_NOISE,
                   ("AgpUnMapMemory - PTE %08lx (%08lx) at offset %d not mapped\n",
                    &Pte[i],
                    Pte[i].AsUlong,
                    i));
            ASSERT(Pte[i].Soft.State == NewState);
        }
    }

    //
    // We have invalidated all the PTEs. Read back the last one we wrote
    // in order to flush the write buffers.
    //
    if (LastChanged != NULL) {
        ULONG Temp;
        Temp = *(volatile ULONG *)(&LastChanged->AsUlong);
    }

    //
    // Reenable the TB
    //
    Agp440EnableTB(AgpContext);

    return(STATUS_SUCCESS);
}


PGART_PTE
Agp440FindRangeInGart(
    IN PGART_PTE StartPte,
    IN PGART_PTE EndPte,
    IN ULONG Length,
    IN BOOLEAN SearchBackward,
    IN ULONG SearchState
    )
/*++

Routine Description:

    Finds a contiguous range in the GART. This routine can
    search either from the beginning of the GART forwards or
    the end of the GART backwards.

Arguments:

    StartIndex - Supplies the first GART pte to search

    EndPte - Supplies the last GART to search (inclusive)

    Length - Supplies the number of contiguous free entries
        to search for.

    SearchBackward - TRUE indicates that the search should begin
        at EndPte and search backwards. FALSE indicates that the
        search should begin at StartPte and search forwards

    SearchState - Supplies the PTE state to look for.

Return Value:

    Pointer to the first PTE in the GART if a suitable range
    is found.

    NULL if no suitable range exists.

--*/

{
    PGART_PTE Current;
    PGART_PTE Last;
    LONG Delta;
    ULONG Found;

    PAGED_CODE();

    ASSERT(EndPte >= StartPte);
    ASSERT(Length <= (ULONG)(EndPte - StartPte + 1));
    ASSERT(Length != 0);

    if (SearchBackward) {
        Current = EndPte;
        Last = StartPte-1;
        Delta = -1;
    } else {
        Current = StartPte;
        Last = EndPte+1;
        Delta = 1;
    }

    Found = 0;
    while (Current != Last) {
        if (Current->Soft.State == SearchState) {
            if (++Found == Length) {
                //
                // A suitable range was found, return it
                //
                if (SearchBackward) {
                    return(Current);
                } else {
                    return(Current - Length + 1);
                }
            }
        } else {
            Found = 0;
        }
        Current += Delta;
    }

    //
    // A suitable range was not found.
    //
    return(NULL);
}


VOID
Agp440SetGTLB_Enable(
    IN PAGP440_EXTENSION AgpContext,
    IN BOOLEAN Enable
    )
/*++

Routine Description:

    Enables or disables the GTLB by setting or clearing the GTLB_Enable bit
    in the AGPCTRL register

Arguments:

    AgpContext - Supplies the AGP context

    Enable - TRUE, GTLB_Enable is set to 1
             FALSE, GTLB_Enable is set to 0

Return Value:

    None

--*/

{
    AGPCTRL AgpCtrl;

    Read440Config(&AgpCtrl, AGPCTRL_OFFSET, sizeof(AgpCtrl));

    if (Enable) {
        AgpCtrl.GTLB_Enable = 1;
    } else {
        AgpCtrl.GTLB_Enable = 0;
    }
    Write440Config(&AgpCtrl, AGPCTRL_OFFSET, sizeof(AgpCtrl));
}


VOID
AgpFindFreeRun(
    IN PVOID AgpContext,
    IN PAGP_RANGE AgpRange,
    IN ULONG NumberOfPages,
    IN ULONG OffsetInPages,
    OUT ULONG *FreePages,
    OUT ULONG *FreeOffset
    )
/*++

Routine Description:

    Finds the first contiguous run of free pages in the specified
    part of the reserved range.

Arguments:

    AgpContext - Supplies the AGP context

    AgpRange - Supplies the AGP range

    NumberOfPages - Supplies the size of the region to be searched for free pages

    OffsetInPages - Supplies the start of the region to be searched for free pages

    FreePages - Returns the length of the first contiguous run of free pages

    FreeOffset - Returns the start of the first contiguous run of free pages

Return Value:

    None. FreePages == 0 if there are no free pages in the specified range.

--*/

{
    PGART_PTE Pte;
    ULONG i;
    
    Pte = (PGART_PTE)(AgpRange->Context) + OffsetInPages;

    //
    // Find the first free PTE
    //
    for (i=0; i<NumberOfPages; i++) {
        if (Pte[i].Hard.Valid == 0) {
            //
            // Found a free PTE, count the contiguous ones.
            //
            *FreeOffset = i + OffsetInPages;
            *FreePages = 0;
            while ((i<NumberOfPages) && (Pte[i].Hard.Valid == 0)) {
                *FreePages += 1;
                ++i;
            }
            return;
        }
    }

    //
    // No free PTEs in the specified range
    //
    *FreePages = 0;
    return;

}


VOID
AgpGetMappedPages(
    IN PVOID AgpContext,
    IN PAGP_RANGE AgpRange,
    IN ULONG NumberOfPages,
    IN ULONG OffsetInPages,
    OUT PMDL Mdl
    )
/*++

Routine Description:

    Returns the list of physical pages mapped into the specified 
    range in the GART.

Arguments:

    AgpContext - Supplies the AGP context

    AgpRange - Supplies the AGP range

    NumberOfPages - Supplies the number of pages to be returned

    OffsetInPages - Supplies the start of the region 

    Mdl - Returns the list of physical pages mapped in the specified range.

Return Value:

    None

--*/

{
    PGART_PTE Pte;
    ULONG i;
    PULONG Pages;

    ASSERT(NumberOfPages * PAGE_SIZE == Mdl->ByteCount);

    Pages = (PULONG)(Mdl + 1);
    Pte = (PGART_PTE)(AgpRange->Context) + OffsetInPages;

    for (i=0; i<NumberOfPages; i++) {
        ASSERT(Pte[i].Hard.Valid == 1);
        Pages[i] = Pte[i].Hard.Page;
    }
    return;
}

typedef struct _BUS_SLOT_ID {
    ULONG BusId;
    ULONG SlotId;
} BUS_SLOT_ID, *PBUS_SLOT_ID;

typedef
NTSTATUS
(*PAGP_GETSET_CONFIG_SPACE)(
    IN PVOID Context,
    IN BOOLEAN Read,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
    );


NTSTATUS
ApGetSetDeviceBusData(
    IN PVOID Context,
    IN BOOLEAN Read,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
    )
/*++

Routine Description:

    Reads or writes PCI config space for the specified device.

Arguments:

    Read - if TRUE, this is a READ IRP
           if FALSE, this is a WRITE IRP

    Buffer - Returns the PCI config data

    Offset - Supplies the offset into the PCI config data where the read should begin

    Length - Supplies the number of bytes to be read

Return Value:

    NTSTATUS

--*/

{
    PCI_SLOT_NUMBER SlotNumber;
    ULONG Transferred;

    SlotNumber.u.AsULONG = 0;
    SlotNumber.u.bits.DeviceNumber = AGP440_TARGET_DEVICE_NUMBER;
    SlotNumber.u.bits.FunctionNumber = AGP440_TARGET_FUNCTION_NUMBER;

    if (Read) {
        Transferred = HalGetBusDataByOffset(PCIConfiguration,
                                            1,
                                            SlotNumber.u.AsULONG,
                                            Buffer,
                                            Offset,
                                            Length);
    } else {
        Transferred = HalSetBusDataByOffset(PCIConfiguration,
                                            1,
                                            SlotNumber.u.AsULONG,
                                            Buffer,
                                            Offset,
                                            Length);

    }

    if (Transferred == Length) {
        return(STATUS_SUCCESS);
    } else {
        return(STATUS_UNSUCCESSFUL);
    }
}


NTSTATUS
ApGetSetBusData(
    IN PBUS_SLOT_ID BusSlotId,
    IN BOOLEAN Read,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
    )
/*++

Routine Description:

    Calls HalGet/SetBusData for the specified PCI bus/slot ID.

Arguments:

    BusSlotId - Supplies the bus and slot ID.

    Read - if TRUE, this is a GetBusData
           if FALSE, this is a SetBusData

    Buffer - Returns the PCI config data

    Offset - Supplies the offset into the PCI config data where the read should begin

    Length - Supplies the number of bytes to be read

Return Value:

    NTSTATUS

--*/

{
    ULONG Transferred;

    if (Read) {
        Transferred = HalGetBusDataByOffset(PCIConfiguration,
                                            BusSlotId->BusId,
                                            BusSlotId->SlotId,
                                            Buffer,
                                            Offset,
                                            Length);
    } else {
        Transferred = HalSetBusDataByOffset(PCIConfiguration,
                                            BusSlotId->BusId,
                                            BusSlotId->SlotId,
                                            Buffer,
                                            Offset,
                                            Length);

    }
    if (Transferred == Length) {
        return(STATUS_SUCCESS);
    } else {
        return(STATUS_UNSUCCESSFUL);
    }
}


NTSTATUS
ApFindAgpCapability(
    IN PAGP_GETSET_CONFIG_SPACE pConfigFn,
    IN PVOID Context,
    OUT PPCI_AGP_CAPABILITY Capability,
    OUT UCHAR *pOffset
    )
/*++

Routine Description:

    Finds the capability offset for the specified device and
    reads in the header.

Arguments:

    pConfigFn - Supplies the function to call for accessing config space
        on the appropriate device.

    Context - Supplies the context to pass to pConfigFn

    Capabilities - Returns the AGP Capabilities common header

    pOffset - Returns the offset into config space.

Return Value:

    NTSTATUS

--*/

{
    PCI_COMMON_HEADER Header;
    PPCI_COMMON_CONFIG PciConfig = (PPCI_COMMON_CONFIG)&Header;
    NTSTATUS Status;
    UCHAR CapabilityOffset;

    //
    // Read the PCI common header to get the capabilities pointer
    //
    Status = (pConfigFn)(Context,
                         TRUE,
                         PciConfig,
                         0,
                         sizeof(PCI_COMMON_HEADER));
    if (!NT_SUCCESS(Status)) {
        AGPLOG(AGP_CRITICAL,
               ("AgpLibGetAgpCapability - read PCI Config space for Context %08lx failed %08lx\n",
                Context,
                Status));
        return(Status);
    }

    //
    // Check the Status register to see if this device supports capability lists.
    // If not, it is not an AGP-compliant device.
    //
    if ((PciConfig->Status & PCI_STATUS_CAPABILITIES_LIST) == 0) {
        AGPLOG(AGP_CRITICAL,
               ("AgpLibGetAgpCapability - Context %08lx does not support Capabilities list, not an AGP device\n",
                Context));
        return(STATUS_NOT_IMPLEMENTED);
    }

    //
    // The device supports capability lists, find the AGP capabilities
    //
    if ((PciConfig->HeaderType & (~PCI_MULTIFUNCTION)) == PCI_BRIDGE_TYPE) {
        CapabilityOffset = PciConfig->u.type1.CapabilitiesPtr;
    } else {
        ASSERT((PciConfig->HeaderType & (~PCI_MULTIFUNCTION)) == PCI_DEVICE_TYPE);
        CapabilityOffset = PciConfig->u.type0.CapabilitiesPtr;
    }
    while (CapabilityOffset != 0) {

        //
        // Read the Capability at this offset
        //
        Status = (pConfigFn)(Context,
                             TRUE,
                             Capability,
                             CapabilityOffset,
                             sizeof(PCI_CAPABILITIES_HEADER));
        if (!NT_SUCCESS(Status)) {
            AGPLOG(AGP_CRITICAL,
                   ("AgpLibGetAgpCapability - read PCI Capability at offset %x for Context %08lx failed %08lx\n",
                    CapabilityOffset,
                    Context,
                    Status));
            return(Status);
        }
        if (Capability->Header.CapabilityID == PCI_CAPABILITY_ID_AGP) {
            //
            // Found the AGP Capability
            //
            break;
        } else {
            //
            // This is some other Capability, keep looking for the AGP Capability
            //
            CapabilityOffset = Capability->Header.Next;
        }
    }
    if (CapabilityOffset == 0) {
        //
        // No AGP capability was found
        //
        AGPLOG(AGP_CRITICAL,
               ("AgpLibGetAgpCapability - Context %08lx does have an AGP Capability entry, not an AGP device\n",
                Context));
        return(STATUS_NOT_IMPLEMENTED);
    }

    AGPLOG(AGP_NOISE,
           ("AgpLibGetAgpCapability - Context %08lx has AGP Capability at offset %x\n",
            Context,
            CapabilityOffset));

    *pOffset = CapabilityOffset;
    return(STATUS_SUCCESS);
}


NTSTATUS
AgpLibGetAgpCapability(
    IN PAGP_GETSET_CONFIG_SPACE pConfigFn,
    IN PVOID Context,
    OUT PPCI_AGP_CAPABILITY Capability
    )
/*++

Routine Description:

    This routine finds and retrieves the AGP capabilities in the
    PCI config space of the AGP master (graphics card).

Arguments:

    pConfigFn - Supplies the function to call for accessing config space
        on the appropriate device.

    Context - Supplies the context to pass to pConfigFn

    Capabilities - Returns the current AGP Capabilities

Return Value:

    NTSTATUS

--*/

{
    NTSTATUS Status;
    UCHAR CapabilityOffset;

    Status = ApFindAgpCapability(pConfigFn,
                                 Context,
                                 Capability,
                                 &CapabilityOffset);
    if (!NT_SUCCESS(Status)) {
        return(Status);
    }

    //
    // Read the rest of the AGP capability
    //
    Status = (pConfigFn)(Context,
                         TRUE,
                         &Capability->Header + 1,
                         CapabilityOffset + sizeof(PCI_CAPABILITIES_HEADER),
                         sizeof(PCI_AGP_CAPABILITY) - sizeof(PCI_CAPABILITIES_HEADER));
    if (!NT_SUCCESS(Status)) {
        AGPLOG(AGP_CRITICAL,
               ("AgpLibGetAgpCapability - read AGP Capability at offset %x for Context %08lx failed %08lx\n",
                CapabilityOffset,
                Context,
                Status));
        return(Status);
    }

    AGPLOG(AGP_NOISE,
           ("AGP CAPABILITY: version %d.%d\n",Capability->Major, Capability->Minor));
    AGPLOG(AGP_NOISE,
           ("\tSTATUS  - Rate: %x  SBA: %x  RQ: %02x\n",
           Capability->AGPStatus.Rate,
           Capability->AGPStatus.SideBandAddressing,
           Capability->AGPStatus.RequestQueueDepthMaximum));
    AGPLOG(AGP_NOISE,
           ("\tCOMMAND - Rate: %x  SBA: %x  RQ: %02x  AGPEnable: %x\n",
           Capability->AGPCommand.Rate,
           Capability->AGPCommand.SBAEnable,
           Capability->AGPCommand.RequestQueueDepth,
           Capability->AGPCommand.AGPEnable));

    return(STATUS_SUCCESS);
}


NTSTATUS
AgpLibGetMasterCapability(
    IN PVOID AgpExtension,
    OUT PPCI_AGP_CAPABILITY Capability
    )
/*++

Routine Description:

    Retrieves the AGP capability for the AGP master (graphics card)

Arguments:

    AgpExtension - Supplies the AGP extension

    Capability - Returns the AGP capability

Return Value:

    NTSTATUS

--*/

{
    return(AgpLibGetAgpCapability(ApGetSetDeviceBusData,
                                  NULL,
                                  Capability));
}


NTSTATUS
AgpLibGetPciDeviceCapability(
    IN ULONG BusId,
    IN ULONG SlotId,
    OUT PPCI_AGP_CAPABILITY Capability
    )
/*++

Routine Description:

    Retrieves the AGP capability for the specified PCI slot.

    Caller is responsible for figuring out what the correct
    Bus/Slot ID is. These are just passed right to HalGetBusData.

Arguments:

    BusId - supplies the bus id

    SlotId - Supplies the slot id

    Capability - Returns the AGP capability

Return Value:

    NTSTATUS

--*/

{
    BUS_SLOT_ID BusSlotId;

    BusSlotId.BusId = BusId;
    BusSlotId.SlotId = SlotId;

    return(AgpLibGetAgpCapability(ApGetSetBusData,
                                  &BusSlotId,
                                  Capability));
}


NTSTATUS
AgpLibSetAgpCapability(
    IN PAGP_GETSET_CONFIG_SPACE pConfigFn,
    IN PVOID Context,
    OUT PPCI_AGP_CAPABILITY Capability
    )
/*++

Routine Description:

    This routine finds and retrieves the AGP capabilities in the
    PCI config space of the AGP master (graphics card).

Arguments:

    pConfigFn - Supplies the function to call for accessing config space
        on the appropriate device.

    Context - Supplies the context to pass to pConfigFn

    Capabilities - Returns the current AGP Capabilities

Return Value:

    NTSTATUS

--*/

{
    NTSTATUS Status;
    UCHAR CapabilityOffset;

    Status = ApFindAgpCapability(pConfigFn,
                                 Context,
                                 Capability,
                                 &CapabilityOffset);
    if (!NT_SUCCESS(Status)) {
        return(Status);
    }

    //
    // Now that we know the offset, write the supplied command register
    //
    Status = (pConfigFn)(Context,
                         FALSE,
                         &Capability->AGPCommand,
                         CapabilityOffset + FIELD_OFFSET(PCI_AGP_CAPABILITY, AGPCommand),
                         sizeof(Capability->AGPCommand));
    if (!NT_SUCCESS(Status)) {
        AGPLOG(AGP_CRITICAL,
               ("AgpLibSetAgpCapability - Set AGP command at offset %x for Context %08lx failed %08lx\n",
                CapabilityOffset,
                Context,
                Status));
        return(Status);
    }

    return(STATUS_SUCCESS);
}


NTSTATUS
AgpLibSetMasterCapability(
    IN PVOID AgpExtension,
    OUT PPCI_AGP_CAPABILITY Capability
    )
/*++

Routine Description:

    Sets the AGP capability for the AGP master (graphics card)

Arguments:

    AgpExtension - Supplies the AGP extension

    Capability - Returns the AGP capability

Return Value:

    NTSTATUS

--*/

{
    return(AgpLibSetAgpCapability(ApGetSetDeviceBusData,
                                  NULL,
                                  Capability));
}


NTSTATUS
AgpLibSetPciDeviceCapability(
    IN ULONG BusId,
    IN ULONG SlotId,
    OUT PPCI_AGP_CAPABILITY Capability
    )
/*++

Routine Description:

    Sets the AGP capability for the specified PCI slot.

    Caller is responsible for figuring out what the correct
    Bus/Slot ID is. These are just passed right to HalSetBusData.

Arguments:

    BusId - supplies the bus id

    SlotId - Supplies the slot id

    Capability - Returns the AGP capability

Return Value:

    NTSTATUS

--*/

{
    BUS_SLOT_ID BusSlotId;

    BusSlotId.BusId = BusId;
    BusSlotId.SlotId = SlotId;

    return(AgpLibSetAgpCapability(ApGetSetBusData,
                                  &BusSlotId,
                                  Capability));
}


NTSTATUS
AgpInitializeTarget(
    IN PVOID AgpExtension
    )
/*++

Routine Description:

    Entrypoint for target initialization. This is called first.

Arguments:

    AgpExtension - Supplies the AGP extension

Return Value:

    NTSTATUS

--*/

{
    PAGP440_EXTENSION Extension = AgpExtension;

    //
    // Initialize our chipset-specific extension
    //
    Extension->ApertureStart.QuadPart = 0;
    Extension->ApertureLength = 0;
    Extension->Gart = NULL;
    Extension->GartLength = 0;
    Extension->GlobalEnable = FALSE;
    Extension->PCIEnable = FALSE;
    Extension->GartPhysical.QuadPart = 0;

    return(STATUS_SUCCESS);
}


NTSTATUS
AgpInitializeMaster(
    IN  PVOID AgpExtension,
    OUT ULONG *AgpCapabilities
    )
/*++

Routine Description:

    Entrypoint for master initialization. This is called after target initialization
    and should be used to initialize the AGP capabilities of both master and target.

    This is also called when the master transitions into the D0 state.

Arguments:

    AgpExtension - Supplies the AGP extension

    AgpCapabilities - Returns the capabilities of this AGP device.

Return Value:

    STATUS_SUCCESS

--*/

{
    NTSTATUS Status;
    PCI_AGP_CAPABILITY MasterCap;
    PCI_AGP_CAPABILITY TargetCap;
    ULONG SBAEnable;
    ULONG DataRate;
    ULONG FastWrite;
#if DBG
    PCI_AGP_CAPABILITY CurrentCap;
#endif

    //
    // Indicate that we can map memory through the GART aperture
    //
    *AgpCapabilities = AGP_CAPABILITIES_MAP_PHYSICAL;

    //
    // Get the master and target AGP capabilities
    //
    Status = AgpLibGetMasterCapability(AgpExtension, &MasterCap);
    if (!NT_SUCCESS(Status)) {
        AGPLOG(AGP_CRITICAL,
               ("AGP440InitializeDevice - AgpLibGetMasterCapability failed %08lx\n"));
        return(Status);
    }

    //
    // Some broken cards (Matrox Millenium II "AGP") report no valid
    // supported transfer rates. These are not really AGP cards. They
    // have an AGP Capabilities structure that reports no capabilities.
    //
    if (MasterCap.AGPStatus.Rate == 0) {
        AGPLOG(AGP_CRITICAL,
               ("AGP440InitializeDevice - AgpLibGetMasterCapability returned no valid transfer rate\n"));
        return(STATUS_INVALID_DEVICE_REQUEST);
    }

    Status = AgpLibGetPciDeviceCapability(0,0,&TargetCap);
    if (!NT_SUCCESS(Status)) {
        AGPLOG(AGP_CRITICAL,
               ("AGP440InitializeDevice - AgpLibGetPciDeviceCapability failed %08lx\n"));
        return(Status);
    }

    //
    // Determine the greatest common denominator for data rate.
    //
    DataRate = TargetCap.AGPStatus.Rate & MasterCap.AGPStatus.Rate;

    ASSERT(DataRate != 0);

    //
    // Select the highest common rate.
    //
    if (DataRate & PCI_AGP_RATE_4X) {
        DataRate = PCI_AGP_RATE_4X;
    } else if (DataRate & PCI_AGP_RATE_2X) {
        DataRate = PCI_AGP_RATE_2X;
    } else if (DataRate & PCI_AGP_RATE_1X) {
        DataRate = PCI_AGP_RATE_1X;
    }

    //
    // Enable SBA if both master and target support it.
    //
    SBAEnable = (TargetCap.AGPStatus.SideBandAddressing & MasterCap.AGPStatus.SideBandAddressing);

    //
    // Enable FastWrite if both master and target support it.
    //
    FastWrite = (TargetCap.AGPStatus.FastWrite & MasterCap.AGPStatus.FastWrite);

    //
    // Enable the Target first.
    //
    TargetCap.AGPCommand.Rate = DataRate;
    TargetCap.AGPCommand.AGPEnable = 1;
    TargetCap.AGPCommand.SBAEnable = SBAEnable;
    TargetCap.AGPCommand.FastWriteEnable = FastWrite;
    TargetCap.AGPCommand.FourGBEnable = 0;  
    Status = AgpLibSetPciDeviceCapability(0, 0, &TargetCap);
    if (!NT_SUCCESS(Status)) {
        AGPLOG(AGP_CRITICAL,
               ("AGP440InitializeDevice - AgpLibSetPciDeviceCapability %08lx for target failed %08lx\n",
                &TargetCap,
                Status));
        return(Status);
    }

    //
    // Now enable the Master
    //
    MasterCap.AGPCommand.Rate = DataRate;
    MasterCap.AGPCommand.AGPEnable = 1;
    MasterCap.AGPCommand.SBAEnable = SBAEnable;
    MasterCap.AGPCommand.FastWriteEnable = FastWrite;
    MasterCap.AGPCommand.FourGBEnable = 0;  
    MasterCap.AGPCommand.RequestQueueDepth = TargetCap.AGPStatus.RequestQueueDepthMaximum;
    Status = AgpLibSetMasterCapability(AgpExtension, &MasterCap);
    if (!NT_SUCCESS(Status)) {
        AGPLOG(AGP_CRITICAL,
               ("AGP440InitializeDevice - AgpLibSetMasterCapability %08lx failed %08lx\n",
                &MasterCap,
                Status));
    }

#if DBG
    //
    // Read them back, see if it worked
    //
    Status = AgpLibGetMasterCapability(AgpExtension, &CurrentCap);
    ASSERT(NT_SUCCESS(Status));

    //
    // If the target request queue depth is greater than the master will
    // allow, it will be trimmed.   Loosen the assert to not require an
    // exact match.
    //
    ASSERT(CurrentCap.AGPCommand.RequestQueueDepth <= MasterCap.AGPCommand.RequestQueueDepth);
    CurrentCap.AGPCommand.RequestQueueDepth = MasterCap.AGPCommand.RequestQueueDepth;
    ASSERT(RtlEqualMemory(&CurrentCap.AGPCommand, &MasterCap.AGPCommand, sizeof(CurrentCap.AGPCommand)));

    Status = AgpLibGetPciDeviceCapability(0,0,&CurrentCap);
    ASSERT(NT_SUCCESS(Status));
    ASSERT(RtlEqualMemory(&CurrentCap.AGPCommand, &TargetCap.AGPCommand, sizeof(CurrentCap.AGPCommand)));

#endif

    return(Status);
}

#endif

