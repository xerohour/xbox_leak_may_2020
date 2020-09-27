/*++

Copyright (c) 1990-1999  Microsoft Corporation

Module Name:

    agp.c

Abstract:

    This is the agp portion of the video port driver.

Author:

    Erick Smith (ericks) Oct. 1997

Environment:

    kernel mode only

Revision History:

--*/


#include "videoprt.h"

PVOID
AddRangeToReservedRegion(
    IN PVIRTUAL_RESERVE_CONTEXT VirtualContext,
    IN ULONG Pages,
    IN ULONG Offset
    );

VOID
RemoveRangeFromReservedRegion(
    IN PVIRTUAL_RESERVE_CONTEXT VirtualContext,
    IN ULONG Pages,
    IN ULONG Offset
    );

#pragma alloc_text(PAGE,VpQueryAgpInterface)
#pragma alloc_text(PAGE,AgpReservePhysical)
#pragma alloc_text(PAGE,AgpReleasePhysical)
#pragma alloc_text(PAGE,AgpCommitPhysical)
#pragma alloc_text(PAGE,AgpFreePhysical)
#pragma alloc_text(PAGE,AgpReserveVirtual)
#pragma alloc_text(PAGE,AgpReleaseVirtual)
#pragma alloc_text(PAGE,AgpCommitVirtual)
#pragma alloc_text(PAGE,AgpFreeVirtual)
#pragma alloc_text(PAGE,VideoPortGetAgpServices)
#pragma alloc_text(PAGE,AddRangeToReservedRegion)
#pragma alloc_text(PAGE,RemoveRangeFromReservedRegion)
#pragma alloc_text(PAGE,CreateBitField)
#pragma alloc_text(PAGE,ModifyRegion)
#pragma alloc_text(PAGE,FindFirstRun)

VOID
DumpBitField(
    PREGION Region
    )
{
    ULONG i;
    ULONG Index = 0;
    ULONG Mask = 1;

    for (i=0; i<Region->Length; i++) {
        if (Mask & Region->BitField[Index]) {
            pVideoDebugPrint((1, "1"));
        } else {
            pVideoDebugPrint((1, "0"));
        }
        Mask <<= 1;
        if (Mask == 0) {
            Index++;
            Mask = 1;
        }
    }
    pVideoDebugPrint((1, "\n"));
}

BOOLEAN
CreateBitField(
    ULONG Length,
    PREGION *Region
    )

/*++

Routine Description:

    This routine creates and initializes a bitfield.

Arguments:

    Length - Number of items to track.

    Region - Location in which to store the pointer to the REGION handle.

Returns:

    TRUE - the the bitfield was created successfully, 
    FALSE - otherwise.

--*/

{
    ULONG NumDwords = (Length + 31) / 32;
    BOOLEAN bRet = FALSE;
    PREGION Buffer;

    Buffer = (PREGION) ExAllocatePoolWithTag(PagedPool, sizeof(REGION) + (NumDwords - 1) * sizeof(ULONG), POOL_TAG);

    if (Buffer) {

        Buffer->Length = Length;
        Buffer->NumDwords = NumDwords;
        memset(&Buffer->BitField[0], 0, NumDwords * 4);

        bRet = TRUE;
    }

    *Region = Buffer;
    return bRet;
}

VOID
ModifyRegion(
    PREGION Region,
    ULONG Offset,
    ULONG Length,
    BOOLEAN Set
    )

/*++

Routine Description:

    Sets 'Length' bits starting at position 'Offset' in the bitfield.

Arguments:

    Region - Pointer to the region to modify.

    Offset - Offset into the bitfield at which to start.

    Length - Number of bits to set.

    Set - TRUE if you want to set the region, FALSE to clear it.


--*/

{
    ULONG Index = Offset / 32;
    ULONG Count = ((Offset + Length - 1) / 32) - Index;
    ULONG lMask = ~((1 << (Offset & 31)) - 1);
    ULONG rMask = ((1 << ((Offset + Length - 1) & 31)) * 2) - 1;
    PULONG ptr = &Region->BitField[Index];

    ASSERT(Length != 0);

    if (Count == 0) {

        //
        // Only one DWORD is modified, so combine left and right masks.
        //

        lMask &= rMask;
    }

    if (Set) {

        *ptr++ |= lMask;

        while (Count > 1) {
            *ptr++ |= 0xFFFFFFFF;
            Count--;
        }

        if (Count) {
            *ptr |= rMask;
        }

    } else {

        *ptr++ &= ~lMask;

        while (Count > 1) {
            *ptr++ &= 0;
            Count--;
        }

        if (Count) {
            *ptr++ &= ~rMask;
        }
    }

#if DBG
    pVideoDebugPrint((1, "Current BitField for Region: 0x%x\n", Region));
    DumpBitField(Region);
#endif
}

BOOLEAN
FindFirstRun(
    PREGION Region,
    PULONG Offset,
    PULONG Length
    )

/*++

Routine Description:

    This routine finds the first run of bits in a bitfield.

Arguments:

    Region - Pointer to the region to operate on.

    Offset - Pointer to a ULONG to hold the offset of the run.

    Length - Pointer to a ULONG to hold the length of a run.

Returns:

    TRUE if a run was detected,
    FALSE otherwise.

--*/

{
    PULONG ptr = Region->BitField;
    ULONG Index = 0;
    ULONG BitMask;
    ULONG lsb;
    ULONG Count;
    ULONG ptrVal;

    while ((Index < Region->NumDwords) && (*ptr == 0)) {
        ptr++;
        Index++;
    }

    if (Index == Region->NumDwords) {
        return FALSE;
    }

    //
    // Find least significant bit
    //

    lsb = 0;
    ptrVal = *ptr;
    BitMask = 1;

    while ((ptrVal & BitMask) == 0) {
        BitMask <<= 1;
        lsb++;
    }

    *Offset = (Index * 32) + lsb;

    //
    // Determine the run length
    //

    Count = 0;

    while (Index < Region->NumDwords) {
        if (ptrVal & BitMask) {
            BitMask <<= 1;
            Count++;

            if (BitMask == 0) {
                BitMask = 0x1;
                Index++;
                ptrVal = *++ptr;
                while ((ptrVal == 0xFFFFFFFF) && (Index < Region->NumDwords)) {
                    Index++;
                    Count += 32;
                    ptrVal = *ptr++;
                }
            }

        } else {
            break;
        }
    }

    *Length = Count;
    return TRUE;
}

BOOLEAN
VpQueryAgpInterface(
    PFDO_EXTENSION FdoExtension
    )

/*++

Routine Description:

    Send a QueryInterface Irp to our parent (the PCI bus driver) to
    retrieve the AGP_BUS_INTERFACE.

Returns:

    NT_STATUS code

--*/

{
    KEVENT             Event;
    PIRP               QueryIrp = NULL;
    IO_STATUS_BLOCK    IoStatusBlock;
    PIO_STACK_LOCATION NextStack;
    NTSTATUS           Status;

    KeInitializeEvent(&Event, SynchronizationEvent, FALSE);

    QueryIrp = IoBuildSynchronousFsdRequest(IRP_MJ_FLUSH_BUFFERS,
                                            FdoExtension->AttachedDeviceObject,
                                            NULL,
                                            0,
                                            NULL,
                                            &Event,
                                            &IoStatusBlock);

    if (QueryIrp == NULL) {
        return FALSE;
    }

    NextStack = IoGetNextIrpStackLocation(QueryIrp);

    //
    // Set the default error code.
    //

    QueryIrp->IoStatus.Status = IoStatusBlock.Status = STATUS_NOT_SUPPORTED;

    //
    // Set up for a QueryInterface Irp.
    //

    NextStack->MajorFunction = IRP_MJ_PNP;
    NextStack->MinorFunction = IRP_MN_QUERY_INTERFACE;

    NextStack->Parameters.QueryInterface.InterfaceType = &GUID_AGP_BUS_INTERFACE_STANDARD;
    NextStack->Parameters.QueryInterface.Size = sizeof(AGP_BUS_INTERFACE_STANDARD);
    NextStack->Parameters.QueryInterface.Version = 1;
    NextStack->Parameters.QueryInterface.Interface = (PINTERFACE) &FdoExtension->AgpInterface;
    NextStack->Parameters.QueryInterface.InterfaceSpecificData = NULL;

    FdoExtension->AgpInterface.Size = sizeof(AGP_BUS_INTERFACE_STANDARD);
    FdoExtension->AgpInterface.Version = 1;

    Status = IoCallDriver(FdoExtension->AttachedDeviceObject, QueryIrp);

    if (Status == STATUS_PENDING) {
        KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
        Status = IoStatusBlock.Status;
    }

    return NT_SUCCESS(Status);
}

PHYSICAL_ADDRESS
AgpReservePhysical(
    IN PVOID Context,
    IN ULONG Pages,
    IN ULONG Caching,
    OUT PVOID *PhysicalReserveContext
    )

/*++

Routine Description:

    Reserves a range of physical addresses for AGP.

Arguments:

    Context - The Agp Context

    Pages - Number of pages to reserve

    Caching - Specifies the type of caching to use

    PhysicalReserveContext - Location to store our reservation context.

Returns:

    The base of the physical address range reserved.

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(Context);
    PHYSICAL_ADDRESS PhysicalAddress = {0,0};
    NTSTATUS status;
    PPHYSICAL_RESERVE_CONTEXT ReserveContext;
    PVOID MapHandle;

    pVideoDebugPrint((1, "AGP: Reserving 0x%x Pages of Address Space\n", Pages));

    ReserveContext = ExAllocatePoolWithTag(PagedPool,
                                           sizeof(PHYSICAL_RESERVE_CONTEXT),
                                           POOL_TAG);

    if (ReserveContext) {

        if (CreateBitField(Pages, &ReserveContext->Region)) {

            status = fdoExtension->AgpInterface.ReserveMemory(
                         fdoExtension->AgpInterface.AgpContext,
                         Pages,
                         Caching ? MmWriteCombined : MmNonCached,
                         &MapHandle,
                         &PhysicalAddress);

            if (NT_SUCCESS(status)) {

                ReserveContext->Pages = Pages;
                ReserveContext->Caching = Caching;
                ReserveContext->MapHandle = MapHandle;
                ReserveContext->PhysicalAddress = PhysicalAddress;

            } else {

                ExFreePool(ReserveContext->Region);
                goto FailureCase;
            }

        } else {

            goto FailureCase;
        }
    }

    *PhysicalReserveContext = ReserveContext;
    return PhysicalAddress;

FailureCase:

    PhysicalAddress.QuadPart = 0;
    ExFreePool(ReserveContext);
    *PhysicalReserveContext = NULL;

    return PhysicalAddress;
}

VOID
AgpReleasePhysical(
    PVOID Context,
    PVOID PhysicalReserveContext
    )

/*++

Routine Description:

    Reserves a range of reserved physical address.

Arguments:

    Context - The Agp Context

    PhysicalReserveContext - The reservation context.

Returns:

    none.

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(Context);
    PPHYSICAL_RESERVE_CONTEXT ReserveContext;
    ULONG Pages;
    ULONG Offset;

    ReserveContext = (PPHYSICAL_RESERVE_CONTEXT) PhysicalReserveContext;

    pVideoDebugPrint((1, "AGP: Releasing 0x%x Pages of Address Space\n", ReserveContext->Pages));

    //
    // Make sure all pages have been freed
    //

    while (FindFirstRun(ReserveContext->Region, &Offset, &Pages)) {
        AgpFreePhysical(Context, PhysicalReserveContext, Pages, Offset);
    }

    fdoExtension->AgpInterface.ReleaseMemory(fdoExtension->AgpInterface.AgpContext,
                                             ReserveContext->MapHandle);

    ExFreePool(ReserveContext->Region);
    ExFreePool(ReserveContext);
}

BOOLEAN
AgpCommitPhysical(
    PVOID Context,
    PVOID PhysicalReserveContext,
    ULONG Pages,
    ULONG Offset
    )

/*++

Routine Description:

    Reserves a range of physical addresses for AGP.

Arguments:

    Context - The Agp Context

    PhysicalReserveContext - The reservation context.

    Pages - Number of pages to commit.

    Offset - The offset into the reserved region at which to commit the pages.

Returns:

    TRUE if successful,
    FALSE otherwise.

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(Context);
    PHYSICAL_ADDRESS MemoryBase = {0,0};
    PPHYSICAL_RESERVE_CONTEXT ReserveContext;
    NTSTATUS status;
    PMDL Mdl;

    ReserveContext = (PPHYSICAL_RESERVE_CONTEXT) PhysicalReserveContext;

    status =
        fdoExtension->AgpInterface.CommitMemory(fdoExtension->AgpInterface.AgpContext,
                                                ReserveContext->MapHandle,
                                                Pages,
                                                Offset,
                                                NULL,
                                                &MemoryBase);

    if (NT_SUCCESS(status)) {

        ModifyRegion(ReserveContext->Region, Offset, Pages, TRUE);
        return TRUE;

    } else {

        return FALSE;
    }
}

VOID
AgpFreePhysical(
    IN PVOID Context,
    IN PVOID PhysicalReserveContext,
    IN ULONG Pages,
    IN ULONG Offset
    )

/*++

Routine Description:

    Reserves a range of physical addresses for AGP.

Arguments:

    Context - The Agp Context

    PhysicalReserveContext - The reservation context.

    Pages - Number of pages to release.

    Offset - The offset into the reserved region at which to release the pages.

Returns:

    none.

Notes:

    At the moment you must free the exact same offset/size as you commited.
    (ie. You can commit 10 pages at offset 2 and then free 5 pages at offset 4).

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(Context);
    PPHYSICAL_RESERVE_CONTEXT ReserveContext;
    PMDL Mdl;

    ReserveContext = (PPHYSICAL_RESERVE_CONTEXT) PhysicalReserveContext;

    fdoExtension->AgpInterface.FreeMemory(fdoExtension->AgpInterface.AgpContext,
                                          ReserveContext->MapHandle,
                                          Pages,
                                          Offset);

    ModifyRegion(ReserveContext->Region, Offset, Pages, FALSE);
}


PVOID
AgpReserveVirtual(
    IN PVOID Context,
    IN HANDLE ProcessHandle,
    IN PVOID PhysicalReserveContext,
    OUT PVOID *VirtualReserveContext
    )

/*++

Routine Description:

    Reserves a range of virtual addresses for AGP.

Arguments:

    Context - The Agp Context

    ProcessHandle - The handle of the process in which to reserve the
        virtual address range.

    PhysicalReserveContext - The physical reservation context to assoctiate
        with the given virtual reservation.

    VirtualReserveContext - The location in which to store the virtual
        reserve context.

Returns:

    The base of the virtual address range reserved.

Notes:

    You can't reserve a range of kernel address space, but if you want to
    commit into kernel space you still need a reservation handle.  Pass in
    NULL for the process handle in this case.

    For the moment, we'll commit the entire region when the do a reservation
    in kernel space.  Then Commit and Free will be no-ops.

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(Context);
    NTSTATUS status = STATUS_SUCCESS;
    ULONG Protect = PAGE_READWRITE;
    PVIRTUAL_RESERVE_CONTEXT ReserveContext;
    PPHYSICAL_RESERVE_CONTEXT PhysicalContext;
    SIZE_T Length;
    PVOID VirtualAddress = NULL;
    PEPROCESS Process = NULL;

    PhysicalContext = (PPHYSICAL_RESERVE_CONTEXT) PhysicalReserveContext;
    Length = PhysicalContext->Pages * PAGE_SIZE;

    ReserveContext = ExAllocatePoolWithTag(PagedPool,
                                           sizeof(VIRTUAL_RESERVE_CONTEXT),
                                           POOL_TAG);

    if (ReserveContext) {

        if (CreateBitField(PhysicalContext->Pages, &ReserveContext->Region)) {

            //
            // BUGBUG: Fix this routine for kernel mode reservation!
            //

            if (!PhysicalContext->Caching) {
                Protect |= PAGE_NOCACHE;
            }

            //
            // Make sure we have the real process handle.
            //

            if (ProcessHandle == NtCurrentProcess()) {
                Process = PsGetCurrentProcess();
            }

            ReserveContext->ProcessHandle = ProcessHandle;
            ReserveContext->Process = Process;
            ReserveContext->PhysicalReserveContext =
                (PPHYSICAL_RESERVE_CONTEXT) PhysicalReserveContext;
            ReserveContext->ReservationList = NULL;

            if (ProcessHandle) {

                VirtualAddress =
                    AddRangeToReservedRegion(ReserveContext,
                                             PhysicalContext->Pages,
                                             0);

                ASSERT(VirtualAddress != NULL);

            } else {

                //
                // BUGBUG: Find a better way to reserve in kernel space.
                //
                // For a kernel reservation, go ahead and commit the
                // entire range.
                //

                if (fdoExtension->AgpInterface.Capabilities &
                    AGP_CAPABILITIES_MAP_PHYSICAL)
                {
                    //
                    // CPU can access AGP memory through AGP aperature.
                    //

                    VirtualAddress =
                        MmMapIoSpace(PhysicalContext->PhysicalAddress,
                                     PhysicalContext->Pages * PAGE_SIZE,
                                     PhysicalContext->Caching ? MmFrameBufferCached : 0);
                    //
                    // Not all systems support USWC, so if we attempted to map USWC
                    // and failed, try again with just non-cached.
                    //
                    if ((VirtualAddress == NULL) &&
                        (PhysicalContext->Caching)) {
                        VirtualAddress = MmMapIoSpace(PhysicalContext->PhysicalAddress,
                                                      PhysicalContext->Pages * PAGE_SIZE,
                                                      MmNonCached);
                    }

                } else {

                    PMDL Mdl;

                    //
                    // Get the MDL for the range we are trying to map.
                    //

                    Mdl = MmCreateMdl(NULL, NULL, PhysicalContext->Pages * PAGE_SIZE);

                    if (Mdl) {

                        fdoExtension->AgpInterface.GetMappedPages(
                                         fdoExtension->AgpInterface.AgpContext,
                                         PhysicalContext->MapHandle,
                                         PhysicalContext->Pages,
                                         0,
                                         Mdl);

                        //
                        // We must use the CPU's virtual memory mechanism to
                        // make the non-contiguous MDL look contiguous.
                        //

                        VirtualAddress =
                            MmMapLockedPagesSpecifyCache(
                                Mdl,
                                (KPROCESSOR_MODE)KernelMode,
                                PhysicalContext->Caching ? MmCached : MmNonCached,
                                NULL,
                                TRUE,
                                HighPagePriority);

                        ExFreePool(Mdl);
                    }
                }
            }

            ReserveContext->VirtualAddress = VirtualAddress;

        } else {

            pVideoDebugPrint((1, "Couldn't create bit field.\n"));
            ExFreePool(ReserveContext);
            ReserveContext = NULL;
        }

    } else {

        pVideoDebugPrint((1, "AgpReserveVirtual: Out of memory.\n"));
    }

    *VirtualReserveContext = ReserveContext;
    return VirtualAddress;
}

VOID
AgpReleaseVirtual(
    IN PVOID Context,
    IN PVOID VirtualReserveContext
    )

/*++

Routine Description:

    Releases a range of reserved virtual addresses.

Arguments:

    Context - The Agp Context

    VirtualReserveContext - The reservation context.

Returns:

    none.

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(Context);
    SIZE_T Length;
    PVIRTUAL_RESERVE_CONTEXT VirtualContext;
    PPHYSICAL_RESERVE_CONTEXT PhysicalContext;
    ULONG Offset;
    ULONG Pages;

    VirtualContext = (PVIRTUAL_RESERVE_CONTEXT) VirtualReserveContext;
    PhysicalContext = VirtualContext->PhysicalReserveContext;

    if (VirtualContext->ProcessHandle) {

        //
        // Make sure all pages have been freed
        //

        while (FindFirstRun(VirtualContext->Region, &Offset, &Pages)) {
            AgpFreeVirtual(Context, VirtualReserveContext, Pages, Offset);
        }

        if (VirtualContext->VirtualAddress) {

            ASSERT(VirtualContext->ReservationList->Next == NULL);

            RemoveRangeFromReservedRegion(VirtualContext,
                                          VirtualContext->ReservationList->Pages,
                                          VirtualContext->ReservationList->Offset);

            if (VirtualContext->ReservationList) {

                ExFreePool(VirtualContext->ReservationList);
                VirtualContext->ReservationList = NULL;
            }
        }

        ExFreePool(VirtualContext->Region);
        ExFreePool(VirtualContext);

    } else {

        //
        // This was kernel virtual memory, so release the memory we
        // committed at reserve time.
        //

        if (fdoExtension->AgpInterface.Capabilities &
            AGP_CAPABILITIES_MAP_PHYSICAL)
        {
            MmUnmapIoSpace(VirtualContext->VirtualAddress,
                           PhysicalContext->Pages * PAGE_SIZE);

        } else {

            PMDL Mdl;

            //
            // Get the MDL for the range we are trying to map.
            //

            Mdl = MmCreateMdl(NULL, NULL, PhysicalContext->Pages * PAGE_SIZE);

            if (Mdl) {

                fdoExtension->AgpInterface.GetMappedPages(
                                fdoExtension->AgpInterface.AgpContext,
                                PhysicalContext->MapHandle,
                                PhysicalContext->Pages,
                                0,
                                Mdl);

                MmUnmapLockedPages(
                    VirtualContext->VirtualAddress,
                    Mdl);

                ExFreePool(Mdl);

            } else {

                //
                // We couldn't free the memory because we couldn't allocate
                // memory for the MDL.  We can free a small chunk at a time
                // by using a MDL on the stack.
                //

                ASSERT(FALSE);
            }
        }
    }
}

PVOID
AddRangeToReservedRegion(
    IN PVIRTUAL_RESERVE_CONTEXT VirtualContext,
    IN ULONG Pages,
    IN ULONG Offset
    )

/*++

Routine Description:

    Adds a range to a reserved region of virtual address space.

Arguments:

    VirtualContext - The context of the virtual reservation.

    Pages - Length of the region to release.

    Offset - Offset into region at which to start releasing pages.

Returns:

    none.

--*/

{
    PULONG VirtualAddress = NULL;
    SIZE_T Length;
    ULONG Protect = PAGE_READWRITE;
    NTSTATUS Status;
    PRESERVATION_LIST Curr, Prev = NULL;

    pVideoDebugPrint((1, "AddPagesToReservedRange\n"));
    pVideoDebugPrint((1, "  Pages = 0x%x\n  Offset = 0x%x\n", Pages, Offset));

    //
    // Adjust Pages and Offset such that will always deal with 64K chunks.
    //

    Pages = Pages + (Offset & 0xf); // grow pages due to decrease offset
    Offset = Offset & ~0xf;         // shrink offset to make it multiple of 16
    Pages = (Pages + 0xf) & ~0xf;   // grow pages to make it multiple of 16

    pVideoDebugPrint((1, "  Pages = 0x%x\n  Offset = 0x%x\n", Pages, Offset));

    if (VirtualContext->ReservationList) {

        //
        // We are making the assumption that if range is added, and we
        // already have a reservation list, that we had removed a region
        // because we mapped it, and now we are adding it back.  So we
        // should be able to scan for a region, and just set the
        // reserved bit to TRUE.  We can then concatinate with existing
        // right or left regions.
        //

        Curr = VirtualContext->ReservationList;

        while (Curr) {

            if (Curr->Offset == Offset)
                break;

            Prev = Curr;
            Curr = Curr->Next;
        }

        pVideoDebugPrint((1, "Region to Reserve:\n"
                             "  Offset: 0x%x\n"
                             "  Pages: 0x%x\n",
                             Curr->Offset,
                             Curr->Pages));

        //
        // If the region is already reserved, simply return.
        //

        if (Curr->Reserved == TRUE) {
            pVideoDebugPrint((1, "Region is already reserved!\n"));
            return NULL;
        }

        //
        // If there is still an allocation in this region, then return.
        //

        if (--Curr->RefCount) {
            pVideoDebugPrint((1, "Region is still in use.\n"));
            return NULL;
        }

        pVideoDebugPrint((1, "Marking region as reserved.\n"));

        Curr->Reserved = TRUE;

        //
        // See if we can concatinate with the left or right region.
        //

        if (Prev && (Prev->Reserved)) {

            //
            // Release the region which we are concatinating with.
            //

            VirtualAddress = (PULONG)((ULONG_PTR)VirtualContext->VirtualAddress + Prev->Offset * PAGE_SIZE);
            Length = Prev->Pages * PAGE_SIZE;

            if (!VirtualContext->PhysicalReserveContext->Caching) {
                Protect |= PAGE_NOCACHE;
            }

            pVideoDebugPrint((1, "Freeing reserved region of length 0x%x at address 0x%p\n",
                                 (ULONG)Length, VirtualAddress));

            if (!NT_SUCCESS(ZwFreeVirtualMemory(VirtualContext->ProcessHandle,
                                                &VirtualAddress,
                                                &Length,
                                                MEM_RELEASE)))
            {
                ASSERT(FALSE);
            }

            pVideoDebugPrint((1, "Concatenating with previous range.\n"));

            Prev->Pages += Curr->Pages;
            Prev->Next = Curr->Next;

            ExFreePool(Curr);

            Curr = Prev;
        }

        if (Curr->Next && (Curr->Next->Reserved)) {

            PRESERVATION_LIST Next = Curr->Next;

            //
            // Release the region which we are concatinating with.
            //

            VirtualAddress = (PULONG)((ULONG_PTR)VirtualContext->VirtualAddress + Next->Offset * PAGE_SIZE);
            Length = Next->Pages * PAGE_SIZE;

            if (!VirtualContext->PhysicalReserveContext->Caching) {
                Protect |= PAGE_NOCACHE;
            }

            pVideoDebugPrint((1, "Freeing reserved region of length 0x%x at address 0x%p\n",
                                 (ULONG)Length, VirtualAddress));

            if (!NT_SUCCESS(ZwFreeVirtualMemory(VirtualContext->ProcessHandle,
                                                &VirtualAddress,
                                                &Length,
                                                MEM_RELEASE)))
            {
                ASSERT(FALSE);
            }

            pVideoDebugPrint((1, "Concatenating with next range.\n"));

            Curr->Pages += Next->Pages;
            Curr->Next = Next->Next;

            ExFreePool(Next);
        }

        pVideoDebugPrint((1, "Expanded Region:\n"
                             "  Offset: 0x%x\n"
                             "  Pages: 0x%x\n",
                             Curr->Offset,
                             Curr->Pages));

        VirtualAddress = (PULONG)((ULONG_PTR)VirtualContext->VirtualAddress + Curr->Offset * PAGE_SIZE);

    } else {

        //
        // Indicate we don't care where we reserve from.
        //

        VirtualAddress = NULL;

        Curr = ExAllocatePoolWithTag(PagedPool,
                                     sizeof(RESERVATION_LIST),
                                     POOL_TAG);

        if (Curr) {

            Curr->Offset   = Offset;
            Curr->Pages    = Pages;
            Curr->RefCount = 0;
            Curr->Reserved = TRUE;
            Curr->Next     = NULL;

            VirtualContext->ReservationList = Curr;

        } else {

            return NULL;
        }
    }

    Length = Curr->Pages * PAGE_SIZE;

    if (!VirtualContext->PhysicalReserveContext->Caching) {
        Protect |= PAGE_NOCACHE;
    }

    pVideoDebugPrint((1, "Reserving 0x%x bytes at virtual address 0x%p\n",
                         (ULONG)Length, VirtualAddress));

    Status = ZwAllocateVirtualMemory(VirtualContext->ProcessHandle,
                                            &VirtualAddress,
                                            0,
                                            &Length,
                                            MEM_RESERVE,
                                            Protect);

    if (!NT_SUCCESS(Status)) {

        pVideoDebugPrint((1, "Status = 0x%x\n", Status));
        ASSERT(FALSE);
        return NULL;
    }

    return VirtualAddress;
}

VOID
RemoveRangeFromReservedRegion(
    IN PVIRTUAL_RESERVE_CONTEXT VirtualContext,
    IN ULONG Pages,
    IN ULONG Offset
    )

/*++

Routine Description:

    Release a sub range of a reserved region of virtual address space.

Arguments:

    VirtualContext - The context of the virtual reservation.

    Pages - Length of the region to release.

    Offset - Offset into region at which to start releasing pages.

Returns:

    none.

--*/

{
    PRESERVATION_LIST ReservationList, Curr, Prev = NULL;
    PULONG VirtualAddress = NULL;
    SIZE_T Length;
    ULONG Protect = PAGE_READWRITE;
    ULONG_PTR Address, ExcessPages;

    pVideoDebugPrint((1, "RemovePagesFromReservedRange\n"));
    pVideoDebugPrint((1, "  Pages = 0x%x\n  Offset = 0x%x\n", Pages, Offset));

    //
    // Adjust Pages and Offset such that will always deal with 64K chunks.
    //

    Pages = Pages + (Offset & 0xf); // grow pages due to decrease offset
    Offset = Offset & ~0xf;         // shrink offset to make it multiple of 16
    Pages = (Pages + 0xf) & ~0xf;   // grow pages to make it multiple of 16

    pVideoDebugPrint((1, "  Pages = 0x%x\n  Offset = 0x%x\n", Pages, Offset));

    ASSERT(VirtualContext->ReservationList != NULL);

    Curr = VirtualContext->ReservationList;

    while (Curr) {

        if ((Curr->Offset <= Offset) &&
            ((Curr->Offset + Curr->Pages) > Offset)) {

            break;
        }

        Prev = Curr;
        Curr = Curr->Next;
    }

    pVideoDebugPrint((1, "Range to fill into:\n"
                         "  Offset: 0x%x\n"
                         "  Pages: 0x%x\n",
                         Curr->Offset,
                         Curr->Pages));


    ASSERT(Curr != NULL);

    //
    // If the region is already free, simply return.
    //

    if (Curr->Reserved == FALSE) {
        Curr->RefCount++;
        return;
    }

    //
    // Release the region which we are mapping into.
    //

    VirtualAddress = (PULONG)((ULONG_PTR)VirtualContext->VirtualAddress + Curr->Offset * PAGE_SIZE);
    Length = Curr->Pages * PAGE_SIZE;

    if (!VirtualContext->PhysicalReserveContext->Caching) {
        Protect |= PAGE_NOCACHE;
    }

    pVideoDebugPrint((1, "Freeing reserved region of length 0x%x at address 0x%p\n",
                         (ULONG)Length, VirtualAddress));

    if (!NT_SUCCESS(ZwFreeVirtualMemory(VirtualContext->ProcessHandle,
                                        &VirtualAddress,
                                        &Length,
                                        MEM_RELEASE)))
    {
        ASSERT(FALSE);
    }

    //
    // Mark the region as not reserved.
    //

    Curr->Reserved = FALSE;
    Curr->RefCount++;

    //
    // Now see if we have sub regions to re-reserve.
    //

    if (Curr->Offset < Offset) {

        //
        // There will be a left over region prior to Curr.
        //

        ReservationList = ExAllocatePoolWithTag(PagedPool,
                                                sizeof(RESERVATION_LIST),
                                                POOL_TAG);

        if (ReservationList) {

            ReservationList->Offset = Curr->Offset;
            ReservationList->Pages = Offset - Curr->Offset;
            ReservationList->RefCount = 0;
            ReservationList->Next = Curr;
            ReservationList->Reserved = TRUE;

            Curr->Pages -= Offset - Curr->Offset;
            Curr->Offset = Offset;

        } else {

            return;
        }

        if (Prev) {
            Prev->Next = ReservationList;
        } else {
            VirtualContext->ReservationList = ReservationList;
        }

        pVideoDebugPrint((1, "Creating 'left' subregion\n"
                             "  Offset: 0x%x\n"
                             "  Pages:  0x%x\n",
                             ReservationList->Offset,
                             ReservationList->Pages));

        //
        // Calculate the virtual address and length of the memory to reserve.
        // Note that ZwAllocateVirtualMemory always rounds to a 64K boundary,
        // so we'll have to round up to prevent it from rounding down.
        //

        Address = ((ULONG_PTR)VirtualContext->VirtualAddress + ReservationList->Offset * PAGE_SIZE);
        VirtualAddress = (PULONG)Address;

        Length = ReservationList->Pages * PAGE_SIZE;

        pVideoDebugPrint((1, "Reserving memory range of length 0x%x at location 0x%p\n",
                             (ULONG)Length, VirtualAddress));

        if (!NT_SUCCESS(ZwAllocateVirtualMemory(VirtualContext->ProcessHandle,
                                                &VirtualAddress,
                                                0,
                                                &Length,
                                                MEM_RESERVE,
                                                Protect)))
        {
            ASSERT(FALSE);
        }
    }

    if (Curr->Pages > Pages) {

        //
        // There will be a left over region after Curr.
        //

        ReservationList = ExAllocatePoolWithTag(PagedPool,
                                                sizeof(RESERVATION_LIST),
                                                POOL_TAG);

        if (ReservationList) {

            ReservationList->Offset = Curr->Offset + Pages;
            ReservationList->Pages = Curr->Pages - Pages;
            ReservationList->RefCount = 0;
            ReservationList->Reserved = TRUE;

            Curr->Pages = Pages;

        } else {

            //
            // BUGBUG:
            //
            // If we return here then we have to adjacent blocks
            // which are both reserved.  This will not cause a
            // problem, but it will never be cleaned up.
            //

            return;
        }

        pVideoDebugPrint((1, "Creating 'right' subregion\n"
                             "  Offset: 0x%x\n"
                             "  Pages:  0x%x\n",
                             ReservationList->Offset,
                             ReservationList->Pages));


        ReservationList->Next = Curr->Next;
        Curr->Next = ReservationList;

        VirtualAddress = (PULONG)((ULONG_PTR)VirtualContext->VirtualAddress + ReservationList->Offset * PAGE_SIZE);
        Length = ReservationList->Pages * PAGE_SIZE;

        pVideoDebugPrint((1, "Reserving memory range of length 0x%x at location 0x%p\n",
                             (ULONG)Length, VirtualAddress));

        if (!NT_SUCCESS(ZwAllocateVirtualMemory(VirtualContext->ProcessHandle,
                                                &VirtualAddress,
                                                0,
                                                &Length,
                                                MEM_RESERVE,
                                                Protect)))
        {
            ASSERT(FALSE);
        }
    }

    return;
}

PVOID
AgpCommitVirtual(
    IN PVOID Context,
    IN PVOID VirtualReserveContext,
    IN ULONG Pages,
    IN ULONG Offset
    )

/*++

Routine Description:

    Reserves a range of physical addresses for AGP.

Arguments:

    Context - The Agp Context

    VirtualReserveContext - The reservation context.

    Pages - Number of pages to commit.

    Offset - The offset into the reserved region at which to commit the pages.

Returns:

    The virtual address for the base of the commited pages.

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(Context);
    PVIRTUAL_RESERVE_CONTEXT VirtualContext;
    PPHYSICAL_RESERVE_CONTEXT PhysicalContext;
    PHYSICAL_ADDRESS PhysicalAddress;
    PVOID VirtualAddress = NULL;
    HANDLE PhysicalMemoryHandle;
    NTSTATUS ntStatus;
    BOOLEAN bUserMode;

    VirtualContext = (PVIRTUAL_RESERVE_CONTEXT) VirtualReserveContext;
    PhysicalContext = VirtualContext->PhysicalReserveContext;

    PhysicalAddress = PhysicalContext->PhysicalAddress;
    PhysicalAddress.LowPart += Offset * PAGE_SIZE;

    bUserMode = VirtualContext->ProcessHandle != NULL;

    if (bUserMode) {

        //
        // First release the VA range we are going to map into.
        //

        RemoveRangeFromReservedRegion(VirtualContext,
                                      Pages,
                                      Offset);

        if (fdoExtension->AgpInterface.Capabilities &
            AGP_CAPABILITIES_MAP_PHYSICAL)
        {
            //
            // CPU can access AGP memory through AGP aperature.
            //

            //
            // Get a handle to the physical memory section using our pointer.
            // If this fails, return.
            //

            ntStatus =
                ObOpenObjectByPointer(
                    PhysicalMemorySection,
                    0L,
                    (PACCESS_STATE) NULL,
                    SECTION_ALL_ACCESS,
                    (POBJECT_TYPE) NULL,
                    KernelMode,
                    &PhysicalMemoryHandle);

            //
            // If successful, map the memory.
            //

            if (NT_SUCCESS(ntStatus)) {

                SIZE_T Length = Pages * PAGE_SIZE;
                ULONG Protect = PAGE_READWRITE;

                VirtualAddress = ((PUCHAR)VirtualContext->VirtualAddress + Offset * PAGE_SIZE);

                if (!PhysicalContext->Caching) {
                    Protect |= PAGE_NOCACHE;
                }

                ntStatus =
                    ZwMapViewOfSection(
                        PhysicalMemoryHandle,
                        VirtualContext->ProcessHandle,
                        &VirtualAddress,
                        0,
                        Pages * PAGE_SIZE,
                        &PhysicalAddress,
                        &Length,
                        ViewUnmap,
                        0,
                        Protect);

                pVideoDebugPrint((1, "ntStatus = 0x%x\n", ntStatus));

                ZwClose(PhysicalMemoryHandle);

            }

        } else {

            PMDL Mdl;

            //
            // Get the MDL for the range we are trying to map.
            //

            Mdl = MmCreateMdl(NULL, NULL, Pages * PAGE_SIZE);

            if (Mdl) {

                fdoExtension->AgpInterface.GetMappedPages(
                                 fdoExtension->AgpInterface.AgpContext,
                                 PhysicalContext->MapHandle,
                                 Pages,
                                 Offset,
                                 Mdl);

                //
                // We must use the CPU's virtual memory mechanism to
                // make the non-contiguous MDL look contiguous.
                //

                VirtualAddress =
                    MmMapLockedPagesSpecifyCache(
                        Mdl,
                        (KPROCESSOR_MODE)UserMode,
                        PhysicalContext->Caching ? MmCached : MmNonCached,
                        (PVOID)((ULONG_PTR)VirtualContext->VirtualAddress + Offset * PAGE_SIZE),
                        TRUE,
                        HighPagePriority);

                ExFreePool(Mdl);
            }
        }

        if (VirtualAddress) {
            ModifyRegion(VirtualContext->Region, Offset, Pages, TRUE);
        }

    } else {

        //
        // Kernel mode commit.  Do nothing.
        //

        VirtualAddress = ((PUCHAR)VirtualContext->VirtualAddress + Offset * PAGE_SIZE);
    }

    return VirtualAddress;
}

VOID
AgpFreeVirtual(
    IN PVOID Context,
    IN PVOID VirtualReserveContext,
    IN ULONG Pages,
    IN ULONG Offset
    )

/*++

Routine Description:

    Frees a range of virtual addresses.

Arguments:

    Context - The Agp Context

    VirtualReserveContext - The reservation context.

    Pages - Number of pages to release.

    Offset - The offset into the reserved region at which to release the pages.

Returns:

    none.

Notes:

    At the moment you must free the exact same offset/size as you commited.
    (ie. You can commit 10 pages at offset 2 and then free 5 pages at offset 4).

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(Context);
    PVIRTUAL_RESERVE_CONTEXT VirtualContext;
    PPHYSICAL_RESERVE_CONTEXT PhysicalContext;
    PVOID VirtualAddress;

    VirtualContext = (PVIRTUAL_RESERVE_CONTEXT) VirtualReserveContext;
    PhysicalContext = VirtualContext->PhysicalReserveContext;

    VirtualAddress = (PUCHAR)((ULONG_PTR)VirtualContext->VirtualAddress + Offset * PAGE_SIZE);

    //
    // Make sure we are in the correct process context.
    //

    if (VirtualContext->ProcessHandle != NULL) {

        if (fdoExtension->AgpInterface.Capabilities &
            AGP_CAPABILITIES_MAP_PHYSICAL)
        {
            if (VirtualAddress <= MM_HIGHEST_USER_ADDRESS) {

                ZwUnmapViewOfSection(
                    VirtualContext->ProcessHandle,
                    VirtualAddress);

            } else {

                MmUnmapIoSpace(VirtualAddress,
                               Pages * PAGE_SIZE);
            }

        } else {

            PMDL Mdl;

            //
            // Get the MDL for the range we are trying to map.
            //

            Mdl = MmCreateMdl(NULL, NULL, Pages * PAGE_SIZE);

            if (Mdl) {

                fdoExtension->AgpInterface.GetMappedPages(
                                fdoExtension->AgpInterface.AgpContext,
                                PhysicalContext->MapHandle,
                                Pages,
                                Offset,
                                Mdl);

                MmUnmapLockedPages(
                    VirtualAddress,
                    Mdl);

                ExFreePool(Mdl);

            } else {

                //
                // We couldn't free the memory because we couldn't allocate
                // memory for the MDL.  We can free a small chunk at a time
                // by using a MDL on the stack.
                //

                ASSERT(FALSE);
            }
        }

        if (VirtualAddress <= MM_HIGHEST_USER_ADDRESS) {

            AddRangeToReservedRegion(VirtualContext,
                                     Pages,
                                     Offset);
        }

        ModifyRegion(VirtualContext->Region, Offset, Pages, FALSE);

    } else {

        //
        // Kernel Space Free - do nothing.
        //
    }
}

BOOLEAN
VideoPortGetAgpServices(
    IN PVOID HwDeviceExtension,
    OUT PVIDEO_PORT_AGP_SERVICES AgpServices
    )

/*++

Routine Description:

    This routine returns a set of AGP services to the caller.

Arguments:

    HwDeviceExtension - Pointer to the miniports device extension

    AgpServices - A buffer in which to place the AGP services.

Returns:

    TRUE if successful,
    FALSE otherwise.

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);
    SYSTEM_BASIC_INFORMATION basicInfo ;
    NTSTATUS status;

    //
    // This entry point is only valid for PnP Drivers.
    //

    if ((fdoExtension->Flags & LEGACY_DRIVER) == 0) {

        if (VpQueryAgpInterface(fdoExtension)) {

            //
            // Fill in the list of function pointers.
            //

            AgpServices->AgpReservePhysical = AgpReservePhysical;
            AgpServices->AgpCommitPhysical  = AgpCommitPhysical;
            AgpServices->AgpFreePhysical    = AgpFreePhysical;
            AgpServices->AgpReleasePhysical = AgpReleasePhysical;

            AgpServices->AgpReserveVirtual  = AgpReserveVirtual;
            AgpServices->AgpCommitVirtual   = AgpCommitVirtual;
            AgpServices->AgpFreeVirtual     = AgpFreeVirtual;
            AgpServices->AgpReleaseVirtual  = AgpReleaseVirtual;

	    status = ZwQuerySystemInformation (SystemBasicInformation,
					       &basicInfo,
					       sizeof(basicInfo),
					       NULL) ;
	    if (!NT_SUCCESS(status)) {
		pVideoDebugPrint((0, "VIDEOPRT: Failed AGP system information.\n"));
		return FALSE ;
	    }

	    AgpServices->AllocationLimit = (basicInfo.NumberOfPhysicalPages *
					    basicInfo.PageSize) / 8 ;

            return TRUE;

        } else {

            return FALSE;
        }

    } else {

        pVideoDebugPrint((1, "VideoPortGetAgpServices - only valid on PnP drivers\n"));
        return FALSE;
    }
}
