/*++

Copyright (c) 1997  Microsoft Corporation

Module Name:

    intrface.c

Abstract:

    Routines for implementing the AGP_BUS_INTERFACE_STANDARD interface

Author:

    John Vert (jvert) 10/26/1997

Revision History:

   Elliot Shmukler (elliots) 3/24/1999 - Added support for "favored" memory
                                          ranges for AGP physical memory allocation,
                                          fixed some bugs.

--*/

#if 0 // NUGOOP

#define INITGUID 1
#include "videoprt.h"

ULONG AgpStopLevel;
ULONG AgpLogLevel;
PAGP_FLUSH_PAGES AgpFlushPages = NULL;  // not implemented

VOID
AgpLibFlushDcacheMdl(
    PMDL Mdl
    );

VOID
AgpInterfaceReference(
    IN PVIDEO_DEVICE_EXTENSION Extension
    )
/*++

Routine Description:

    References an interface. Currently a NOP.

Arguments:

    Extension - Supplies the device extension

Return Value:

    None

--*/

{
}


VOID
AgpInterfaceDereference(
    IN PVIDEO_DEVICE_EXTENSION Extension
    )
/*++

Routine Description:

    Dereferences an interface. Currently a NOP.

Arguments:

    Extension - Supplies the device extension

Return Value:

    None

--*/

{
}


NTSTATUS
AgpInterfaceReserveMemory(
    IN PVIDEO_DEVICE_EXTENSION Extension,
    IN ULONG NumberOfPages,
    IN MEMORY_CACHING_TYPE MemoryType,
    OUT PVOID *MapHandle,
    OUT OPTIONAL PHYSICAL_ADDRESS *PhysicalAddress
    )
/*++

Routine Description:

    Reserves memory in the specified aperture

Arguments:

    Extension - Supplies the device extension where physical address space should be reserved.

    NumberOfPages - Supplies the number of pages to reserve.

    MemoryType - Supplies the memory caching type.

    MapHandle - Returns the mapping handle to be used on subsequent calls.

    PhysicalAddress - If present, returns the physical address in the aperture of the reserved 
            space

Return Value:

    NTSTATUS

--*/

{
    PVOID AgpContext;
    NTSTATUS Status;
    PAGP_RANGE Range;

    PAGED_CODE();                              

    AgpContext = GET_AGP_CONTEXT_FROM_MASTER(Extension);

    Range = ExAllocatePoolWithTag(PagedPool,
                                  sizeof(AGP_RANGE),
                                  'RpgA');
    if (Range == NULL) {
        return(STATUS_INSUFFICIENT_RESOURCES);
    }
    Range->CommittedPages = 0;
    Range->NumberOfPages = NumberOfPages;
    Range->Type = MemoryType;

    LOCK_MASTER(Extension);
    Status = AgpReserveMemory(AgpContext,
                              Range);
    UNLOCK_MASTER(Extension);
    if (!NT_SUCCESS(Status)) {
        AGPLOG(AGP_CRITICAL,
               ("AgpInterfaceReserveMemory - reservation of %x pages of type %d failed %08lx\n",
                NumberOfPages,
                MemoryType,
                Status));
    } else {
        AGPLOG(AGP_NOISE,
               ("AgpInterfaceReserveMemory - reserved %x pages of type %d at %I64X\n",
                NumberOfPages,
                MemoryType,
                Range->MemoryBase.QuadPart));
    }

    *MapHandle = Range;
    if (ARGUMENT_PRESENT(PhysicalAddress)) {
        *PhysicalAddress = Range->MemoryBase;
    }
    return(Status);
}


NTSTATUS
AgpInterfaceReleaseMemory(
    IN PVIDEO_DEVICE_EXTENSION Extension,
    IN PVOID MapHandle
    )
/*++

Routine Description:

    Releases memory in the specified aperture that was previously reserved by
    AgpInterfaceReserveMemory

Arguments:

    Extension - Supplies the device extension where physical address space should be reserved.

    MapHandle - Supplies the mapping handle returned from AgpInterfaceReserveMemory

Return Value:

    NTSTATUS

--*/

{
    PAGP_RANGE Range;
    PVOID AgpContext;
    NTSTATUS Status;

    PAGED_CODE();

    AgpContext = GET_AGP_CONTEXT_FROM_MASTER(Extension);
    Range = (PAGP_RANGE)MapHandle;

    LOCK_MASTER(Extension);
    //
    // Make sure the range is empty
    //
    ASSERT(Range->CommittedPages == 0);
    if (Range->CommittedPages != 0) {
        AGPLOG(AGP_CRITICAL,
               ("AgpInterfaceReleaseMemory - Invalid attempt to release non-empty range %08lx\n",
                Range));
        UNLOCK_MASTER(Extension);
        return(STATUS_INVALID_PARAMETER);
    }

    AGPLOG(AGP_NOISE,
           ("AgpInterfaceReleaseMemory - releasing range %08lx, %lx pages at %08lx\n",
            Range,
            Range->NumberOfPages,
            Range->MemoryBase.QuadPart));
    Status = AgpReleaseMemory(AgpContext,
                              Range);
    if (!NT_SUCCESS(Status)) {
        AGPLOG(AGP_CRITICAL,
               ("AgpInterfaceReleaseMemory - release failed %08lx\n",
                Status));
    }
    UNLOCK_MASTER(Extension);
    ExFreePool(Range);
    return(Status);
}


NTSTATUS
AgpInterfaceCommitMemory(
    IN PVIDEO_DEVICE_EXTENSION Extension,
    IN PVOID MapHandle,
    IN ULONG NumberOfPages,
    IN ULONG OffsetInPages,
    IN OUT PMDL Mdl OPTIONAL,
    OUT PHYSICAL_ADDRESS *MemoryBase
    )
/*++

Routine Description:

    Commits memory into the specified aperture that was previously reserved by
    AgpInterfaceReserveMemory

Arguments:

    Extension - Supplies the device extension where physical address space should
        be committed.

    MapHandle - Supplies the mapping handle returned from AgpInterfaceReserveMemory

    NumberOfPages - Supplies the number of pages to be committed.

    OffsetInPages - Supplies the offset, in pages, into the aperture reserved by
        AgpInterfaceReserveMemory

    Mdl - Returns the MDL describing the pages of memory committed.

    MemoryBase - Returns the physical memory address of the committed memory.

Return Value:

    NTSTATUS

--*/

{
    PAGP_RANGE Range = (PAGP_RANGE)MapHandle;
    PMDL NewMdl;
    PVOID AgpContext;
    NTSTATUS Status = STATUS_SUCCESS;
    ULONG RunLength, RunOffset;
    ULONG CurrentLength, CurrentOffset;
    PMDL FirstMdl=NULL;

    PAGED_CODE();

    AgpContext = GET_AGP_CONTEXT_FROM_MASTER(Extension);

    ASSERT(NumberOfPages <= Range->NumberOfPages);
    ASSERT(NumberOfPages > 0);
    ASSERT((Mdl == NULL) || (Mdl->ByteCount == PAGE_SIZE * NumberOfPages));

    CurrentLength = NumberOfPages;
    CurrentOffset = OffsetInPages;

    LOCK_MASTER(Extension);
    do {

        //
        // Find the first free run in the supplied range.
        //
        AgpFindFreeRun(AgpContext,
                       Range,
                       CurrentLength,
                       CurrentOffset,
                       &RunLength,
                       &RunOffset);

        if (RunLength > 0) {
            ASSERT(RunLength <= CurrentLength);
            ASSERT(RunOffset >= CurrentOffset);
            ASSERT(RunOffset < CurrentOffset + CurrentLength);
            ASSERT(RunOffset + RunLength <= CurrentOffset + CurrentLength);

            //
            // Compute the next offset and length
            //
            CurrentLength -= (RunOffset - CurrentOffset) + RunLength;
            CurrentOffset = RunOffset + RunLength;

            //
            // Get an MDL from memory management big enough to map the 
            // requested range.
            //

            NewMdl = AgpLibAllocatePhysicalMemory(AgpContext, RunLength * PAGE_SIZE);

            //
            // This can fail in two ways, either no memory is available at all (NewMdl == NULL)
            // or some pages were available, but not enough. (NewMdl->ByteCount < Length)
            //
            if (NewMdl == NULL) {
                AGPLOG(AGP_CRITICAL,
                       ("AgpInterfaceReserveMemory - Couldn't allocate pages for %lx bytes\n",
                        RunLength));
                Status = STATUS_INSUFFICIENT_RESOURCES;
                break;
            } else if (BYTES_TO_PAGES(NewMdl->ByteCount) < RunLength) {
                AGPLOG(AGP_CRITICAL,
                       ("AgpInterfaceCommitMemory - Only allocated enough pages for %lx of %lx bytes\n",
                        NewMdl->ByteCount,
                        RunLength));
                Status = STATUS_INSUFFICIENT_RESOURCES;
                MmFreePagesFromMdl(NewMdl);
                break;
            }

            //
            // Now that we have our MDL, we can map this into the specified
            // range.
            //
            if (AgpFlushPages != NULL) {
                (AgpFlushPages)(AgpContext, NewMdl);
            } else {
                AgpLibFlushDcacheMdl(NewMdl);
            }
            Status = AgpMapMemory(AgpContext,
                                  Range,
                                  NewMdl,
                                  RunOffset,
                                  MemoryBase);
            if (!NT_SUCCESS(Status)) {
                AGPLOG(AGP_CRITICAL,
                       ("AgpInterfaceCommitMemory - AgpMapMemory for Mdl %08lx in range %08lx failed %08lx\n",
                        NewMdl,
                        Range,
                        Status));
                MmFreePagesFromMdl(NewMdl);
                break;
            }
            Range->CommittedPages += RunLength;

            //
            // Add this MDL to our list of allocated MDLs for cleanup
            // If we need to cleanup, we will also need to know the page offset
            // so that we can unmap the memory. Stash that value in the ByteOffset
            // field of the MDL (ByteOffset is always 0 for our MDLs)
            //
            NewMdl->ByteOffset = RunOffset;
            NewMdl->Next = FirstMdl;
            FirstMdl = NewMdl;
        }

    } while (RunLength > 0);

    //
    // Cleanup the MDLs. If the allocation failed, we need to
    // unmap them and free the pages and the MDL itself. If the
    // operation completed successfully, we just need to free the
    // MDL.
    //
    while (FirstMdl) {
        NewMdl = FirstMdl;
        FirstMdl = NewMdl->Next;
        if (!NT_SUCCESS(Status)) {

            //
            // Unmap the memory that was mapped. The ByteOffset field
            // of the MDL is overloaded here to store the offset in pages
            // into the range.
            //
            AgpUnMapMemory(AgpContext,
                           Range,
                           NewMdl->ByteCount / PAGE_SIZE,
                           NewMdl->ByteOffset);
            NewMdl->ByteOffset = 0;
            Range->CommittedPages -= NewMdl->ByteCount / PAGE_SIZE;
            MmFreePagesFromMdl(NewMdl);
        }
        ExFreePool(NewMdl);
    }

    if (NT_SUCCESS(Status)) {

        if (Mdl) {
            //
            // Get the MDL that describes the entire mapped range.
            //
            AgpGetMappedPages(AgpContext,
                              Range,
                              NumberOfPages,
                              OffsetInPages,
                              Mdl);
        }

        MemoryBase->QuadPart = Range->MemoryBase.QuadPart + OffsetInPages * PAGE_SIZE;
    }

    UNLOCK_MASTER(Extension);
    return(Status);
}


NTSTATUS
AgpInterfaceFreeMemory(
    IN PVIDEO_DEVICE_EXTENSION Extension,
    IN PVOID MapHandle,
    IN ULONG NumberOfPages,
    IN ULONG OffsetInPages
    )
/*++

Routine Description:

    Frees memory previously committed by AgpInterfaceCommitMemory

Arguments:

    Extension - Supplies the device extension where physical address space should
        be freed.

    MapHandle - Supplies the mapping handle returned from AgpInterfaceReserveMemory

    NumberOfPages - Supplies the number of pages to be freed.

    OffsetInPages - Supplies the start of the range to be freed.

Return Value:

    NTSTATUS

--*/

{
    PAGP_RANGE Range = (PAGP_RANGE)MapHandle;
    PVOID AgpContext;
    NTSTATUS Status;
    PMDL FreeMdl;

    PAGED_CODE();

    AgpContext = GET_AGP_CONTEXT_FROM_MASTER(Extension);

    ASSERT(OffsetInPages < Range->NumberOfPages);
    ASSERT(OffsetInPages + NumberOfPages <= Range->NumberOfPages);
    //
    // Make sure the supplied address is within the reserved range
    //
    if ((OffsetInPages >= Range->NumberOfPages) ||
        (OffsetInPages + NumberOfPages > Range->NumberOfPages)) {
        AGPLOG(AGP_WARNING,
               ("AgpInterfaceFreeMemory - Invalid free of %x pages at offset %x from range %I64X (%x pages)\n",
                NumberOfPages,
                OffsetInPages,
                Range->MemoryBase.QuadPart,
                Range->NumberOfPages));
        return(STATUS_INVALID_PARAMETER);
    }

    //
    // Allocate an MDL big enough to contain the pages to be unmapped.
    //
    FreeMdl = MmCreateMdl(NULL, 0, NumberOfPages * PAGE_SIZE);
    if (FreeMdl == NULL) {

// @@BEGIN_DDKSPLIT
        //
        // This is kind of a sticky situation. We can't allocate the memory that we need to free up
        // some memory! I guess we could have a small MDL on our stack and free things that way.
        // BUGBUG John Vert (jvert) 11/11/1997
        //   implement this
        //
// @@END_DDKSPLIT

        ASSERT(FreeMdl != NULL);
        return(STATUS_INSUFFICIENT_RESOURCES);
    }
    LOCK_MASTER(Extension);

    //
    // Get the MDL that describes the entire mapped range
    //
    AgpGetMappedPages(AgpContext, 
                      Range,
                      NumberOfPages,
                      OffsetInPages,
                      FreeMdl);
    //
    // Unmap the memory
    //
    Status = AgpUnMapMemory(AgpContext,
                            Range,
                            NumberOfPages,
                            OffsetInPages);
    UNLOCK_MASTER(Extension);
    if (!NT_SUCCESS(Status)) {
        AGPLOG(AGP_CRITICAL,
               ("AgpInterfaceFreeMemory - UnMapMemory for %x pages at %I64X failed %08lx\n",
                NumberOfPages,
                Range->MemoryBase.QuadPart + OffsetInPages * PAGE_SIZE,
                Status));
    } else {
        //
        // Free the pages
        //
        MmFreePagesFromMdl(FreeMdl);
        ASSERT(Range->CommittedPages >= NumberOfPages);
        Range->CommittedPages -= NumberOfPages;
    }

    //
    // Free the MDL we allocated.
    //
    ExFreePool(FreeMdl);
    return(Status);
}

NTSTATUS
AgpInterfaceGetMappedPages(
    IN PVIDEO_DEVICE_EXTENSION Extension,
    IN PVOID MapHandle,
    IN ULONG NumberOfPages,
    IN ULONG OffsetInPages,
    OUT PMDL Mdl
    )
/*++

Routine Description:

    Returns the list of physical pages mapped backing the specified range.

Arguments:

    Extension - Supplies the device extension where physical address space should
        be freed.

    MapHandle - Supplies the mapping handle returned from AgpInterfaceReserveMemory

    NumberOfPages - Supplies the number of pages to be returned

    OffsetInPages - Supplies the start of the rangion

Return Value:

    NTSTATUS

--*/

{
    PAGP_RANGE Range = (PAGP_RANGE)MapHandle;
    PVOID AgpContext;

    PAGED_CODE();

    AgpContext = GET_AGP_CONTEXT_FROM_MASTER(Extension);

    ASSERT(NumberOfPages <= Range->NumberOfPages);
    ASSERT(NumberOfPages > 0);
    ASSERT(OffsetInPages < Range->NumberOfPages);
    ASSERT(OffsetInPages + NumberOfPages <= Range->NumberOfPages);
    ASSERT(Mdl->ByteCount == PAGE_SIZE * NumberOfPages);

    //
    // Make sure the supplied address is within the reserved range
    //
    if ((OffsetInPages >= Range->NumberOfPages) ||
        (OffsetInPages + NumberOfPages > Range->NumberOfPages)) {
        AGPLOG(AGP_WARNING,
               ("AgpInterfaceGetMappedPages - Invalid 'get' of %x pages at offset %x from range %I64X (%x pages)\n",
                NumberOfPages,
                OffsetInPages,
                Range->MemoryBase.QuadPart,
                Range->NumberOfPages));
        return(STATUS_INVALID_PARAMETER);
    }

    //
    // Get the MDL that describes the entire mapped range
    //
    LOCK_MASTER(Extension);

    AgpGetMappedPages(AgpContext, 
                      Range,
                      NumberOfPages,
                      OffsetInPages,
                      Mdl);

    UNLOCK_MASTER(Extension);
    return(STATUS_SUCCESS);
}


PMDL
AgpLibAllocatePhysicalMemory(IN PVOID AgpContext, IN ULONG TotalBytes)
/*++

Routine Description:

    Allocates a set of physical memory pages for use by the AGP driver.
    
    This routine uses MmAllocatePagesForMdl to attempt to allocate
    as many of the pages as possible within favored AGP memory
    ranges (if any).

Arguments:

    AgpContext   - The AgpContext

    TotalBytes   - The total amount of bytes to allocate.

Return Value:

    An MDL that describes the allocated physical pages or NULL
    if this function is unsuccessful.

    NOTE: Just like MmAllocatePagesForMdl, this function can return
    an MDL that describes an allocation smaller than TotalBytes in size.

--*/
{
   PAGED_CODE();

   AGPLOG(AGP_NOISE, ("AGPLIB: Attempting to allocate memory = %u pages.\n",
            BYTES_TO_PAGES(TotalBytes)));

   return MmAllocatePagesForMdl(0,
                                MAXULONG_PTR,
                                TotalBytes);
}


PVOID
AgpLibAllocateMappedPhysicalMemory(IN PVOID AgpContext, IN ULONG TotalBytes)
/*++

Routine Description:

    Same as AgpLibAllocatePhysicalMemory, except this function will
    also map the allocated memory to a virtual address.

Arguments:

    Same as AgpLibAllocatePhysicalMemory.

Return Value:

    A virtual address of the allocated memory or NULL if unsuccessful.

--*/
{
   PMDL Mdl;
   PVOID Ret;

   PAGED_CODE();
   
   AGPLOG(AGP_NOISE, 
          ("AGPLIB: Attempting to allocate mapped memory = %u.\n", TotalBytes));

   //
   // Call the real memory allocator.
   //

   Mdl = AgpLibAllocatePhysicalMemory(AgpContext, TotalBytes);

   // Two possible failures

   // 1. MDL is NULL. No memory could be allocated.

   if (Mdl == NULL) {

      AGPLOG(AGP_WARNING, ("AGPMAP: Could not allocate anything.\n"));

      return NULL;
   }

   // 2. MDL has some pages allocated but not enough.

   if (Mdl->ByteCount < TotalBytes) {

      AGPLOG(AGP_WARNING, ("AGPMAP: Could not allocate enough.\n"));

      MmFreePagesFromMdl(Mdl);
      ExFreePool(Mdl);
      return NULL;
   }

   // Ok. Our allocation succeeded. Map it to a virtual address.

   // Step 1: Map the locked Pages. (will return NULL if failed)

   Mdl->MdlFlags |= MDL_PAGES_LOCKED;
   Ret = MmMapLockedPagesSpecifyCache (Mdl, MmNonCached);

   // Don't need the Mdl anymore, whether we succeeded or failed.

   ExFreePool(Mdl);

   if (Ret == NULL) {
      AGPLOG(AGP_WARNING, ("AGPMAP: Could not map.\n"));
   }

   return Ret;
}

#if defined (_X86_)
#define FLUSH_DCACHE(Mdl) __asm{ wbinvd }
#else
#define FLUSH_DCACHE(Mdl)   \
            AGPLOG(AGP_CRITICAL,    \
                   ("AgpLibFlushDcacheMdl - NEED TO IMPLEMENT DCACHE FLUSH FOR THIS ARCHITECTURE!!\n"))
#endif


VOID
AgpLibFlushDcacheMdl(
    PMDL Mdl
    )
/*++

Routine Description:

    Flushes the specified MDL from the D-caches of all processors
    in the system.

    Current algorithm is to set the current thread's affinity to each
    processor in turn and flush the dcache. This could be made a lot
    more efficient if this turns out to be a hot codepath

Arguments:

    Mdl - Supplies the MDL to be flushed.

Return Value:

    None.

--*/

{
    FLUSH_DCACHE(Mdl);
}

#endif 0
