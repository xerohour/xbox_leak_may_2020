/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fscache.c

Abstract:

    This module implements the file system cache services.

--*/

#include "mi.h"

//
// Macro to convert from a physical byte offset to a physical block number.
//

#define FscByteOffsetToBlockNumber(byteoffset) \
    ((ULONG)((ULONGLONG)(byteoffset) >> PAGE_SHIFT))

//
// Macro to round a size in bytes up to a multiple of the sector size.
//

#define FscRoundToSectors(volume, size) \
    (((ULONG)(size) + ((volume)->SectorSize - 1)) & \
        ~((volume)->SectorSize - 1))

//
// Array of cache elements.
//
PFSCACHE_ELEMENT FscElementArray;

//
// Number of elements allocated to the array of cache elements.
//
ULONG FscNumberOfCachePages;

//
// Linked list of cache elements ordered by least recently used.
//
LIST_ENTRY FscLruList;

//
// Page table entries used by FscWriteFFs to bulk write a range of 0xFF bytes
// using a single page.
//
PMMPTE FscWriteFFsPtes;

//
// Synchronization event to guard against multiple threads executing
// FscSetCacheSize at the same time.
//
INITIALIZED_KEVENT(FscSetCacheSizeEvent, SynchronizationEvent, TRUE);

//
// Notification event for when a read for a cache buffer has completed and there
// are threads waiting for the I/O completion.
//
INITIALIZED_KEVENT(FscCollidedMapBufferEvent, NotificationEvent, FALSE);

//
// Synchronization event for when the number of free elements is non-zero.
//
INITIALIZED_KEVENT(FscWaitingForElementEvent, SynchronizationEvent, FALSE);

//
// Guards multiple threads attempting to enter FscWriteFFs and access
// FscWriteFFsPtes.
//
INITIALIZED_CRITICAL_SECTION(FscWriteFFsLock);

VOID
FscBuildElementLruList(
    VOID
    )
/*++

Routine Description:

    This routine rebuilds the global element lists after the number of pages
    allocated to the cache has changed.  No assumptions are made about the order
    that pages should be placed in the LRU list.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PFSCACHE_ELEMENT ElementArray;
    ULONG NumberOfCachePages;
    ULONG Index;

    MI_ASSERT_LOCK_MM();

    //
    // Initialize the LRU list head.
    //

    InitializeListHead(&FscLruList);

    //
    // Move these globals into locals to keep the compiler happy.
    //

    ElementArray = FscElementArray;
    NumberOfCachePages = FscNumberOfCachePages;

    //
    // Loop over all of the elements and insert them into the appropriate lists.
    //

    for (Index = 0; Index < NumberOfCachePages; Index++) {

        if (ElementArray[Index].CacheExtension == NULL) {

            //
            // The element doesn't reference valid data, so place it at the
            // front of the LRU list so that we grab this type of page first.
            //

            InsertHeadList(&FscLruList, &ElementArray[Index].ListEntry);

        } else {

            //
            // The element references valid data, so place it at the tail of the
            // LRU list so that we don't start reclaiming any elements with
            // valid data until all "free" elements from above are allocated.
            //

            InsertTailList(&FscLruList, &ElementArray[Index].ListEntry);
        }
    }
}

NTSTATUS
FscGrowCacheSize(
    IN PFN_COUNT NumberOfCachePages
    )
/*++

Routine Description:

    This routine grows the number of pages allocated to the file system cache.

Arguments:

    NumberOfCachePages - Specifies the number of pages that should be allocated
        to the file system cache.

Return Value:

    Status of operation.

--*/
{
    PFSCACHE_ELEMENT NewElementArray;
    PCHAR CacheBuffer;
    ULONG Index;
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;

    MI_ASSERT_LOCK_MM();

    ASSERT(NumberOfCachePages > FscNumberOfCachePages);

    //
    // Allocate the new array of cache elements.
    //

    NewElementArray = (PFSCACHE_ELEMENT)ExAllocatePoolWithTag(
        sizeof(FSCACHE_ELEMENT) * NumberOfCachePages, 'AcsF');

    if (NewElementArray == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

#if DBG
    RtlFillMemoryUlong(NewElementArray, sizeof(FSCACHE_ELEMENT) *
        NumberOfCachePages, 'looP');
#endif

    //
    // Allocate the additional cache pages as one virtually contiguous chunk.
    //

    CacheBuffer = (PCHAR)MiAllocateMappedMemory(&MmSystemPteRange,
        MmFsCacheUsage, PAGE_READONLY,
        (NumberOfCachePages - FscNumberOfCachePages) << PAGE_SHIFT,
        MiRemoveAnyPage, FALSE);

    if (CacheBuffer == NULL) {
        ExFreePool(NewElementArray);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Copy the old array of cache elements to the new array.
    //

    RtlCopyMemory(NewElementArray, FscElementArray, sizeof(FSCACHE_ELEMENT) *
        FscNumberOfCachePages);

    //
    // Fill in the new cache elements.
    //

    for (Index = FscNumberOfCachePages; Index < NumberOfCachePages; Index++) {

        NewElementArray[Index].CacheExtension = NULL;
        NewElementArray[Index].CacheBuffer = CacheBuffer;

        //
        // Override the PTE index that's currently in the page frame with the
        // buffer's index into the FscElementArray so that we can quickly map
        // from a buffer's virtual address to it's cache element.
        //

        PageFrameNumber = MiGetPteAddress(CacheBuffer)->Hard.PageFrameNumber;
        PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

        ASSERT(PageFrame->Busy.Busy != 0);
        ASSERT(PageFrame->Busy.BusyType == MmFsCacheUsage);

        PageFrame->FsCache.ElementIndex = Index;

        CacheBuffer += PAGE_SIZE;
    }

    //
    // Switch to the new array of cache elements.
    //

    if (FscElementArray != NULL) {
        ExFreePool(FscElementArray);
    }

    FscElementArray = NewElementArray;
    FscNumberOfCachePages = NumberOfCachePages;

    //
    // Relink the LRU list of cache elements.
    //

    FscBuildElementLruList();

    return STATUS_SUCCESS;
}

NTSTATUS
FscShrinkCacheSize(
    IN PFN_COUNT NumberOfCachePages
    )
/*++

Routine Description:

    This routine shrinks the number of pages allocated to the file system cache.

Arguments:

    NumberOfCachePages - Specifies the number of pages that should be allocated
        to the file system cache.

Return Value:

    Status of operation.

--*/
{
    PFSCACHE_ELEMENT NewElementArray;
    ULONG PagesToRelease;
    ULONG Index;
    PFSCACHE_ELEMENT Element;
    KIRQL OldIrql;
    ULONG NewIndex;
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;

    MI_ASSERT_LOCK_MM();

    ASSERT(NumberOfCachePages < FscNumberOfCachePages);

    //
    // Allocate the new array of cache elements.
    //

    if (NumberOfCachePages != 0) {

        NewElementArray = (PFSCACHE_ELEMENT)ExAllocatePoolWithTag(
            sizeof(FSCACHE_ELEMENT) * NumberOfCachePages, 'AcsF');

        if (NewElementArray == NULL) {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

#if DBG
        RtlFillMemoryUlong(NewElementArray, sizeof(FSCACHE_ELEMENT) *
            NumberOfCachePages, 'looP');
#endif

    } else {
        NewElementArray = NULL;
    }

    //
    // Compute the number of pages that we need to grab from the existing cache.
    //

    PagesToRelease = FscNumberOfCachePages - NumberOfCachePages;

    //
    // Loop over the file system cache and grab as many free pages as we can.
    //

ReleaseMoreElements:
    for (Index = FscNumberOfCachePages; Index > 0; Index--) {

        Element = &FscElementArray[Index - 1];

        //
        // Check if this element is free and not already marked for deletion.
        //

        if (Element->UsageCount == 0 && !Element->DeletePending) {

            //
            // Mark the element as pending deletion.
            //

            Element->DeletePending = TRUE;
            Element->CacheExtension = NULL;

            //
            // Remove the entry from the LRU list so that other routines no
            // longer can access the element.
            //

            RemoveEntryList(&Element->ListEntry);
            InitializeListHead(&Element->ListEntry);

            //
            // Decrement the number of pages that we've released and check if
            // we've discovered enough free pages.
            //

            PagesToRelease--;

            if (PagesToRelease == 0) {
                break;
            }
        }
    }

    //
    // If we still need to release more pages, then block until more elements
    // become available.  We own FscSetCacheSizeEvent, so we don't have to worry
    // about another thread growing or shrinking the cache out from under us.
    //

    if (PagesToRelease != 0) {

        MI_UNLOCK_MM_AND_THEN_WAIT(PASSIVE_LEVEL);

        KeWaitForSingleObject(&FscWaitingForElementEvent, Executive, KernelMode,
            FALSE, NULL);

        MI_LOCK_MM(&OldIrql);

        goto ReleaseMoreElements;
    }

    //
    // We've now marked the necessary number of pages to delete.  Walk through
    // the array and free up the entries marked for deletion and copy the
    // surviving elements to the new array of cache elements.
    //

    NewIndex = 0;

    for (Index = 0; Index < FscNumberOfCachePages; Index++) {

        Element = &FscElementArray[Index];

        if (Element->DeletePending) {

            //
            // Free the cache buffer.
            //

            MiFreeMappedMemory(&MmSystemPteRange, PAGE_ALIGN(Element->CacheBuffer),
                PAGE_SIZE);

        } else {

            //
            // Transfer the cache buffer to the new array of cache elements.
            //

            NewElementArray[NewIndex] = *Element;

            //
            // Change the index stored in the page frame to be the new array's
            // index.
            //

            PageFrameNumber =
                MiGetPteAddress(Element->CacheBuffer)->Hard.PageFrameNumber;
            PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

            ASSERT(PageFrame->Busy.Busy != 0);
            ASSERT(PageFrame->Busy.BusyType == MmFsCacheUsage);

            PageFrame->FsCache.ElementIndex = NewIndex;

            NewIndex++;
        }
    }

    ASSERT(NewIndex == NumberOfCachePages);

    //
    // Switch to the new array of cache elements.
    //

    ExFreePool(FscElementArray);

    FscElementArray = NewElementArray;
    FscNumberOfCachePages = NumberOfCachePages;

    //
    // Relink the LRU list of cache elements.
    //

    FscBuildElementLruList();

    //
    // Notify as many waiters for free elements as we can so that other threads
    // try to grab free elements given the new element count, which could be
    // zero now.
    //

    while (!IsListEmpty(&FscWaitingForElementEvent.Header.WaitListHead)) {
        KeSetEvent(&FscWaitingForElementEvent, 0, FALSE);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
FscSetCacheSize(
    IN PFN_COUNT NumberOfCachePages
    )
/*++

Routine Description:

    This routine changes the number of pages allocated to the file system cache.

Arguments:

    NumberOfCachePages - Specifies the number of pages that should be allocated
        to the file system cache.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    KIRQL OldIrql;

    KeWaitForSingleObject(&FscSetCacheSizeEvent, Executive, KernelMode, FALSE,
        NULL);

    MI_LOCK_MM(&OldIrql);

    if (NumberOfCachePages > FSCACHE_MAXIMUM_NUMBER_OF_CACHE_PAGES) {
        status = STATUS_INVALID_PARAMETER;
    } else if (NumberOfCachePages > FscNumberOfCachePages) {
        status = FscGrowCacheSize(NumberOfCachePages);
    } else if (NumberOfCachePages < FscNumberOfCachePages) {
        status = FscShrinkCacheSize(NumberOfCachePages);
    } else {
        status = STATUS_SUCCESS;
    }

    MI_UNLOCK_MM(OldIrql);

    KeSetEvent(&FscSetCacheSizeEvent, 0, FALSE);

    return status;
}

PFN_COUNT
FscGetCacheSize(
    VOID
    )
/*++

Routine Description:

    This routine returns the number of pages allocated to the file system cache.

Arguments:

    None.

Return Value:

    Number of pages currently allocated to the file system cache.

--*/
{
    return FscNumberOfCachePages;
}

PFSCACHE_ELEMENT
FscLookupElement(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN ULONG BlockNumber
    )
/*++

Routine Description:

    This routine searches the file system cache for an element that maps the
    block with the supplied block number from the supplied target device.

Arguments:

    CacheExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    The pointer to the element if found, else NULL.

--*/
{
    PLIST_ENTRY NextListEntry;
    PFSCACHE_ELEMENT Element;

    NextListEntry = FscLruList.Blink;

    while (NextListEntry != &FscLruList) {

        Element = CONTAINING_RECORD(NextListEntry, FSCACHE_ELEMENT, ListEntry);

        //
        // Check if this element satisfies the search criteria.
        //

        if ((Element->BlockNumber == BlockNumber) &&
            (Element->CacheExtension == CacheExtension)) {

            //
            // Move the element to the end of the LRU list.
            //

            RemoveEntryList(&Element->ListEntry);
            InsertTailList(&FscLruList, &Element->ListEntry);

            return Element;
        }

        //
        // If we find an element with an empty cache extension, then the element
        // doesn't reference valid data.  All free elements are at the front of
        // the LRU list, so we know that we can stop the search now.
        //

        if (Element->CacheExtension == NULL) {
            break;
        }

        NextListEntry = Element->ListEntry.Blink;
    }

    return NULL;
}

PFSCACHE_ELEMENT
FscAllocateElement(
    VOID
    )
/*++

Routine Description:

    This routine allocates an element from the file system cache.  If no
    elements are currently available, it will block until elements do become
    available.

Arguments:

    None.

Return Value:

    The pointer to the allocated element.

--*/
{
    PLIST_ENTRY NextListEntry;
    PFSCACHE_ELEMENT Element;
    KIRQL OldIrql;

    MI_ASSERT_LOCK_MM();

RetryOperation:
    //
    // If the cache doesn't have any pages allocated to it, then we're never
    // going to be able to allocate an element, so bail out now.
    //

    if (FscNumberOfCachePages == 0) {
        MiDbgPrint(("FSCACHE: cache page required by file system, but cache size is zero.\n"));
        return NULL;
    }

    //
    // Search the LRU list for an unreferenced element.
    //

    NextListEntry = FscLruList.Flink;

    while (NextListEntry != &FscLruList) {

        Element = CONTAINING_RECORD(NextListEntry, FSCACHE_ELEMENT, ListEntry);

        //
        // Check if this element is free.  If so, detach the element from the
        // LRU list and give it back to the caller.
        //

        if (Element->UsageCount == 0) {
            RemoveEntryList(&Element->ListEntry);
            ASSERT(MiGetPteAddress(Element->CacheBuffer)->Hard.Write == 0);
            return Element;
        }

        NextListEntry = Element->ListEntry.Flink;
    }

    //
    // There aren't any unused pages in the LRU list.  We'll have to block until
    // a page becomes available.
    //

    MI_UNLOCK_MM_AND_THEN_WAIT(APC_LEVEL);

    KeWaitForSingleObject(&FscWaitingForElementEvent, Executive, KernelMode,
        FALSE, NULL);

    MI_LOCK_MM(&OldIrql);

    goto RetryOperation;
}

PFSCACHE_ELEMENT
FscElementFromCacheBuffer(
    PVOID CacheBuffer
    )
/*++

Routine Description:

    This routine returns the element corresponding to the supplied cache buffer
    pointer.

Arguments:

    CacheBuffer - Supplies the pointer to the cache buffer.

Return Value:

    The pointer to the corresponding element.

--*/
{
    PFSCACHE_ELEMENT Element;
    PFN_NUMBER PageFrameNumber;
    PMMPFN PageFrame;

    MI_ASSERT_LOCK_MM();

    //
    // From any cache virtual address, we can get back to its cache element by
    // going through the page frame database.  We have to acquire the memory
    // manager lock here to make sure the page doesn't move while we're doing
    // the conversion.
    //

    PageFrameNumber = MiGetPteAddress(CacheBuffer)->Hard.PageFrameNumber;
    PageFrame = MI_PFN_ELEMENT(PageFrameNumber);

    ASSERT(PageFrame->Busy.Busy != 0);
    ASSERT(PageFrame->Busy.BusyType == MmFsCacheUsage);

    Element = &FscElementArray[PageFrame->FsCache.ElementIndex];

    ASSERT(PAGE_ALIGN(Element->CacheBuffer) == PAGE_ALIGN(CacheBuffer));

    return Element;
}

VOID
FASTCALL
FscSetCacheBufferProtection(
    IN PVOID CacheBuffer,
    IN BOOLEAN MapAsReadWrite
    )
/*++

Routine Description:

    This routine is called to change the page protection of the supplied cache
    buffer.

Arguments:

    CacheBuffer - Specifies the cache buffer to change the page protection for.

    MapAsReadWrite - Specifies whether the page should be mapped as read-only or
        read/write.

Return Value:

    None.

--*/
{
    PMMPTE PointerPte;
    MMPTE TempPte;

    MI_ASSERT_LOCK_MM();

    PointerPte = MiGetPteAddress(CacheBuffer);

    TempPte = *PointerPte;

    TempPte.Hard.Write = MapAsReadWrite ? 1 : 0;

    MI_WRITE_AND_FLUSH_PTE(PointerPte, TempPte);
}

NTSTATUS
FscSignalIoEventCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:

    This routine is called to signal an event when a thread is blocked on an
    I/O operation inside an IRP dispatch routine.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    Context - Specifies the context that was supplied to IoSetCompletionRoutine.

Return Value:

    Status of operation.

--*/
{
    KeSetEvent((PKEVENT)Context, IO_DISK_INCREMENT, FALSE);

    return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS
FscMapBuffer(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN PIRP Irp,
    IN ULONGLONG ByteOffset,
    IN BOOLEAN MapAsReadWrite,
    OUT PVOID *ReturnedCacheBuffer
    )
/*++

Routine Description:

    This routine maps the page buffer at the supplied byte offset into the file
    system cache.

    The returned cache buffer will have the same page byte offset as the
    supplied byte offset.

Arguments:

    CacheExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    ByteOffset - Specifies the byte offset to map into the cache.

    MapAsReadWrite - Specifies whether the page should be mapped as read-only or
        read/write.

    CacheBuffer - Specifies the buffer to receive the pointer to the cache
        buffer.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    KIRQL OldIrql;
    ULONG BlockNumber;
    PFSCACHE_ELEMENT Element;
    PVOID CacheBuffer;
    KEVENT IoEvent;
    PIO_STACK_LOCATION IrpSp;
    ULONG EndingBlockNumber;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);
    ASSERT(Irp->CurrentLocation > CacheExtension->TargetDeviceObject->StackSize);
    ASSERT(ByteOffset < (ULONGLONG)CacheExtension->PartitionLength.QuadPart);

RetryOperation:
    MI_LOCK_MM(&OldIrql);

    //
    // Attempt to find the requested block number in the file system cache.
    //

    BlockNumber = FscByteOffsetToBlockNumber(ByteOffset);
    Element = FscLookupElement(CacheExtension, BlockNumber);

    if (Element != NULL) {

        //
        // The element was found.  If there's I/O in progress for the element,
        // then we need to block until the I/O is complete.  Most likely,
        // another thread is in the process of reading the block we need.
        //

        if (!Element->ReadInProgress) {

            Element->UsageCount++;

            CacheBuffer = PAGE_ALIGN(Element->CacheBuffer);

            if (MapAsReadWrite) {
                FscSetCacheBufferProtection(CacheBuffer, TRUE);
            } else {
                ASSERT(MiGetPteAddress(CacheBuffer)->Hard.Write == 0);
            }

            *ReturnedCacheBuffer = (PCHAR)CacheBuffer + BYTE_OFFSET(ByteOffset);

            MI_UNLOCK_MM(OldIrql);

            return STATUS_SUCCESS;
        }

        //
        // Mark the element so that the original thread knows that somebody is
        // waiting for a read operation to complete.
        //

        Element->ReadWaiters = TRUE;

        //
        // Contention should happen fairly infrequently since file system access
        // will likely happen on a small number of threads.  So we use one event
        // for all collided calls to FscMapBuffer.
        //

        MI_UNLOCK_MM_AND_THEN_WAIT(OldIrql);

        KeWaitForSingleObject(&FscCollidedMapBufferEvent, Executive, KernelMode,
            FALSE, NULL);

        goto RetryOperation;
    }

    //
    // Allocate an element to receive the page.  We could get a NULL back from
    // this routine if the cache has been scaled back to zero pages.
    //

    Element = FscAllocateElement();

    if (Element == NULL) {
        MI_UNLOCK_MM(OldIrql);
        return STATUS_NO_MEMORY;
    }

    //
    // Initialize the element with our block number and cache extension and
    // insert the element at the tail of the LRU list.  The element is marked so
    // that other threads know that we're currently reading the cache buffer
    // into memory.
    //

    Element->BlockNumber = BlockNumber;
    Element->CacheExtension = CacheExtension;
    Element->UsageCount = 1;
    Element->ReadInProgress = TRUE;
    ASSERT(!Element->ReadWaiters);

    InsertTailList(&FscLruList, &Element->ListEntry);

    //
    // Lock down the cache buffer.
    //

    CacheBuffer = PAGE_ALIGN(Element->CacheBuffer);
    MmLockUnlockBufferPages(CacheBuffer, PAGE_SIZE, FALSE);

    //
    // Leave the memory manager lock so that we can block for the I/O operation.
    //

    MI_UNLOCK_MM(OldIrql);

    //
    // Initialize the event used to block for completion of the I/O operation
    // and set the completion routine that will signal this event.
    //

    KeInitializeEvent(&IoEvent, NotificationEvent, FALSE);

    IoSetCompletionRoutine(Irp, FscSignalIoEventCompletion, &IoEvent, TRUE,
        TRUE, TRUE);

    //
    // Initialize the next IRP stack location for the I/O operation.
    //

    IrpSp = IoGetNextIrpStackLocation(Irp);

    IrpSp->MajorFunction = IRP_MJ_READ;
    IrpSp->Flags = SL_FSCACHE_REQUEST;
    IrpSp->Parameters.Read.ByteOffset.QuadPart = (ULONGLONG)BlockNumber <<
        PAGE_SHIFT;
    IrpSp->Parameters.Read.CacheBuffer = CacheBuffer;

    //
    // Compute the number of bytes that we're going to read.  The length of the
    // partition is not necessarily page aligned, so we have to be careful to
    // request a legal number of bytes.
    //

    EndingBlockNumber =
        FscByteOffsetToBlockNumber(CacheExtension->PartitionLength.QuadPart);

    if (BlockNumber < EndingBlockNumber) {
        IrpSp->Parameters.Read.Length = PAGE_SIZE;
    } else {
        IrpSp->Parameters.Read.Length =
            BYTE_OFFSET(CacheExtension->PartitionLength.LowPart);
        ASSERT(BlockNumber == EndingBlockNumber);
        ASSERT(IrpSp->Parameters.Read.Length != 0);
    }

    //
    // Submit the request to the target device and wait for the I/O operation to
    // complete.
    //

    status = IoCallDriver(CacheExtension->TargetDeviceObject, Irp);

    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(&IoEvent, WrFsCacheIn, KernelMode, FALSE, NULL);
        status = Irp->IoStatus.Status;
    }

    MI_LOCK_MM(&OldIrql);

    //
    // Unlock the cache buffer.
    //

    MmLockUnlockBufferPages(CacheBuffer, PAGE_SIZE, TRUE);

    //
    // Recompute the pointer to the cache element to guard against somebody
    // changing the size of the cache out from under us and causing the old
    // cache element pointer to become invalid.
    //

    Element = FscElementFromCacheBuffer(CacheBuffer);

    ASSERT(Element->BlockNumber == BlockNumber);
    ASSERT(Element->UsageCount == 1);
    ASSERT(Element->ReadInProgress);

    //
    // Clear the read in progress flag from the cache element.  If anybody is
    // waiting for this buffer to be mapped, then signal the collision event so
    // that everybody goes and checks if their page is available.  If an error
    // occurred reading in the page, the other threads won't see that error but
    // will have to call down to the lower driver themselves.  That's equivalent
    // to the same thread attempting to read in the same page multiple times and
    // failing.
    //

    Element->ReadInProgress = FALSE;

    if (Element->ReadWaiters) {
        KePulseEvent(&FscCollidedMapBufferEvent, IO_DISK_INCREMENT, FALSE);
        Element->ReadWaiters = FALSE;
    }

    //
    // If we successfully read the page in, then return the address of the cache
    // buffer to the caller.  Otherwise, discard the cache buffer.
    //

    if (NT_SUCCESS(status)) {

        if (MapAsReadWrite) {
            FscSetCacheBufferProtection(CacheBuffer, TRUE);
        } else {
            ASSERT(MiGetPteAddress(CacheBuffer)->Hard.Write == 0);
        }

        *ReturnedCacheBuffer = (PCHAR)CacheBuffer + BYTE_OFFSET(ByteOffset);

    } else {
        FscDiscardBuffer(CacheBuffer);
    }

    MI_UNLOCK_MM(OldIrql);

    return status;
}

NTSTATUS
FscMapEmptyBuffer(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN ULONGLONG ByteOffset,
    OUT PVOID *CacheBuffer
    )
/*++

Routine Description:

    This routine maps the page buffer at the supplied byte offset into the file
    system cache if the buffer is already present in the cache.  If the buffer
    is not present in the cache, then an uninitialized page is returned instead.
    This routine is intended for writes of page sized buffers through the cache.

    The supplied byte offset must be page aligned.

Arguments:

    CacheExtension - Specifies the extension that the I/O request is for.

    ByteOffset - Specifies the byte offset to map into the cache.

    CacheBuffer - Specifies the buffer to receive the pointer to the cache
        buffer.

Return Value:

    Status of operation.

--*/
{
    KIRQL OldIrql;
    ULONG BlockNumber;
    PFSCACHE_ELEMENT Element;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);
    ASSERT(ByteOffset < (ULONGLONG)CacheExtension->PartitionLength.QuadPart);
    ASSERT(BYTE_OFFSET(ByteOffset) == 0);

    MI_LOCK_MM(&OldIrql);

    //
    // Attempt to find the requested block number in the file system cache.
    //

    BlockNumber = FscByteOffsetToBlockNumber(ByteOffset);
    Element = FscLookupElement(CacheExtension, BlockNumber);

    if (Element != NULL) {

        ASSERT(!Element->ReadInProgress);

        Element->UsageCount++;

        //
        // Make the cache buffer writable.
        //

        FscSetCacheBufferProtection(Element->CacheBuffer, TRUE);

        //
        // Fill the cache buffer with 0xFFs to reduce signal noise when writing
        // to a memory unit (MU) device.
        //

        RtlFillMemory(PAGE_ALIGN(Element->CacheBuffer), PAGE_SIZE, (ULONG)-1);

        *CacheBuffer = PAGE_ALIGN(Element->CacheBuffer);

        MI_UNLOCK_MM(OldIrql);

        return STATUS_SUCCESS;
    }

    //
    // Allocate an element to receive the page.  We could get a NULL back from
    // this routine if the cache has been scaled back to zero pages.
    //

    Element = FscAllocateElement();

    if (Element == NULL) {
        MI_UNLOCK_MM(OldIrql);
        return STATUS_NO_MEMORY;
    }

    //
    // All we want is an empty page, so we just need to initialize the element
    // with our block number and cache extension and insert the element at the
    // tail of the LRU list.
    //

    Element->BlockNumber = BlockNumber;
    Element->CacheExtension = CacheExtension;
    Element->UsageCount = 1;
    ASSERT(!Element->ReadInProgress);
    ASSERT(!Element->ReadWaiters);

    InsertTailList(&FscLruList, &Element->ListEntry);

    //
    // Make the cache buffer writable.
    //

    FscSetCacheBufferProtection(Element->CacheBuffer, TRUE);

    //
    // Fill the cache buffer with 0xFFs to reduce signal noise when writing to a
    // memory unit (MU) device.
    //

    RtlFillMemory(PAGE_ALIGN(Element->CacheBuffer), PAGE_SIZE, (ULONG)-1);

    *CacheBuffer = PAGE_ALIGN(Element->CacheBuffer);

    MI_UNLOCK_MM(OldIrql);

    return STATUS_SUCCESS;
}

NTSTATUS
FscWriteBuffer(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN PIRP Irp,
    IN ULONGLONG ByteOffset,
    IN ULONG WriteLength,
    IN PVOID CacheBuffer
    )
/*++

Routine Description:

    This routine writes out the contents of the supplied cache buffer to the
    target device.

    Although the cache buffer is PAGE_SIZE bytes in length, this routine may
    only write enough sectors to contain the starting byte offset and the
    specified number of bytes.

Arguments:

    CacheExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    ByteOffset - Specifies the byte offset to start writing to.

    WriteLength - Specifies the number of bytes that have been dirtied in the
        cache.

    CacheBuffer - Specifies the address of the cache buffer to start writing
        from.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONG SectorMask;
    ULONGLONG EndingByteOffset;
    KEVENT IoEvent;
    PIO_STACK_LOCATION IrpSp;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);
    ASSERT(Irp->CurrentLocation > CacheExtension->TargetDeviceObject->StackSize);
    ASSERT(ByteOffset < (ULONGLONG)CacheExtension->PartitionLength.QuadPart);
    ASSERT((ByteOffset + WriteLength) <= (ULONGLONG)CacheExtension->PartitionLength.QuadPart);
    ASSERT((BYTE_OFFSET(ByteOffset) + WriteLength) <= PAGE_SIZE);
    ASSERT(MiGetPteAddress(CacheBuffer)->Hard.Write != 0);

    //
    // Align the byte offset and length to sector boundaries.
    //

    SectorMask = CacheExtension->SectorSize - 1;
    EndingByteOffset = (ByteOffset + WriteLength + SectorMask) & ~((ULONGLONG)SectorMask);
    ByteOffset = ByteOffset & ~((ULONGLONG)SectorMask);
    WriteLength = (ULONG)(EndingByteOffset - ByteOffset);
    CacheBuffer = (PCHAR)PAGE_ALIGN(CacheBuffer) + BYTE_OFFSET(ByteOffset);

    ASSERT(EndingByteOffset <= (ULONGLONG)CacheExtension->PartitionLength.QuadPart);
    ASSERT((BYTE_OFFSET(ByteOffset) + WriteLength) <= PAGE_SIZE);

    //
    // Lock down the cache buffer.
    //

    MmLockUnlockBufferPages(CacheBuffer, WriteLength, FALSE);

    //
    // Initialize the event used to block for completion of the I/O operation
    // and set the completion routine that will signal this event.
    //

    KeInitializeEvent(&IoEvent, NotificationEvent, FALSE);

    IoSetCompletionRoutine(Irp, FscSignalIoEventCompletion, &IoEvent, TRUE,
        TRUE, TRUE);

    //
    // Initialize the next IRP stack location for the I/O operation.
    //

    IrpSp = IoGetNextIrpStackLocation(Irp);

    IrpSp->MajorFunction = IRP_MJ_WRITE;
    IrpSp->Flags = SL_FSCACHE_REQUEST;
    IrpSp->Parameters.Write.ByteOffset.QuadPart = ByteOffset;
    IrpSp->Parameters.Write.Length = WriteLength;
    IrpSp->Parameters.Write.CacheBuffer = CacheBuffer;

    //
    // Submit the request to the target device and wait for the I/O operation to
    // complete.
    //

    status = IoCallDriver(CacheExtension->TargetDeviceObject, Irp);

    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(&IoEvent, WrFsCacheOut, KernelMode, FALSE, NULL);
        status = Irp->IoStatus.Status;
    }

    //
    // Unlock the cache buffer.
    //

    MmLockUnlockBufferPages(CacheBuffer, WriteLength, TRUE);

    //
    // Unmap or discard the buffer.
    //

    if (NT_SUCCESS(status)) {
        FscUnmapBuffer(CacheBuffer);
    } else {
        FscDiscardBuffer(CacheBuffer);
    }

    return status;
}

VOID
FscUnmapBuffer(
    PVOID CacheBuffer
    )
/*++

Routine Description:

    This routine unmaps the supplied page buffer that was obtained from a call
    to FscMapBuffer.  The buffer remains in the file system cache until another
    request reclaims the memory.

Arguments:

    CacheBuffer - Specifies the cache buffer to unmap.

Return Value:

    None.

--*/
{
    KIRQL OldIrql;
    PFSCACHE_ELEMENT Element;

    MI_LOCK_MM(&OldIrql);

    //
    // Change the protection of the cache buffer back to read-only if necessary.
    //

    if (MiGetPteAddress(CacheBuffer)->Hard.Write != 0) {
        FscSetCacheBufferProtection(CacheBuffer, FALSE);
    }

    //
    // Obtain the element corresponding to the supplied cache buffer.
    //

    Element = FscElementFromCacheBuffer(CacheBuffer);

    ASSERT(Element->UsageCount > 0);

    //
    // Decrement the reference count for the element and signal anybody waiting
    // for a cache page.
    //
    // Note that setting the cache buffer back to its page aligned address
    // zeroes out the usage count and various flags.
    //

    Element->UsageCount--;

    if (Element->UsageCount == 0) {

        Element->CacheBuffer = PAGE_ALIGN(Element->CacheBuffer);

        if (!IsListEmpty(&FscWaitingForElementEvent.Header.WaitListHead)) {
            KeSetEvent(&FscWaitingForElementEvent, 0, FALSE);
        }
    }

    MI_UNLOCK_MM(OldIrql);
}

VOID
FscDiscardBuffer(
    PVOID CacheBuffer
    )
/*++

Routine Description:

    This routine unmaps and discards the supplied page buffer that was obtained
    from a call to FscMapBuffer or FscMapEmptyBuffer.  The buffer is marked as
    being free in the system cache.  This is intended to be called after failing
    to write out a dirty cache buffer so that a subsequent mapping obtains the
    original buffer.

    Note that this routine is also called in the error path of FscMapBuffer.

Arguments:

    CacheBuffer - Specifies the cache buffer to discard.

Return Value:

    None.

--*/
{
    KIRQL OldIrql;
    PFSCACHE_ELEMENT Element;

    MI_LOCK_MM(&OldIrql);

    //
    // Change the protection of the cache buffer back to read-only.
    //

    FscSetCacheBufferProtection(CacheBuffer, FALSE);

    //
    // Obtain the element corresponding to the supplied cache buffer.
    //

    Element = FscElementFromCacheBuffer(CacheBuffer);

    ASSERT(!Element->DeletePending);
    ASSERT(Element->UsageCount == 1);

    //
    // Release the reference on the page, null out the cache extension so that
    // we don't try to map the page again, and insert it at the front of the LRU
    // list.
    //
    // Note that setting the cache buffer back to its page aligned address
    // zeroes out the usage count and various flags.
    //

    Element->CacheBuffer = PAGE_ALIGN(Element->CacheBuffer);
    Element->CacheExtension = NULL;

    ASSERT(Element->UsageCount == 0);

    RemoveEntryList(&Element->ListEntry);
    InsertHeadList(&FscLruList, &Element->ListEntry);

    if (!IsListEmpty(&FscWaitingForElementEvent.Header.WaitListHead)) {
        KeSetEvent(&FscWaitingForElementEvent, 0, FALSE);
    }

    MI_UNLOCK_MM(OldIrql);
}

NTSTATUS
FscWriteFFs(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN PIRP Irp,
    IN ULONGLONG ByteOffset,
    IN ULONG WriteLength
    )
/*++

Routine Description:

    This routine writes out the supplied number of 0xFF bytes to the target
    device.  Only a single cache buffer at the supplied byte offset is dirtied
    for the I/O operation.

Arguments:

    CacheExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    ByteOffset - Specifies the byte offset to start writing to.

    WriteLength - Specifies the number of 0xFF bytes to write out; this number
        should be a multiple of the sector size.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PVOID CacheBuffer;
    MMPTE TempPte;
    PMMPTE PointerPte;
    ULONG Index;
    ULONG WriteBytesRemaining;
    KEVENT IoEvent;
    PIO_STACK_LOCATION IrpSp;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);
    ASSERT(ByteOffset < (ULONGLONG)CacheExtension->PartitionLength.QuadPart);
    ASSERT((ByteOffset + WriteLength) <= (ULONGLONG)CacheExtension->PartitionLength.QuadPart);
    ASSERT(BYTE_OFFSET(ByteOffset) == 0);
    ASSERT(WriteLength > 0);
    ASSERT((WriteLength & (CacheExtension->SectorSize - 1)) == 0);

    //
    // Synchronize access to FscWriteFFsPtes.
    //

    RtlEnterCriticalSection(&FscWriteFFsLock);

    //
    // Allocate a cache buffer for the operation.
    //

    status = FscMapEmptyBuffer(CacheExtension, ByteOffset, &CacheBuffer);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // If we're writing out more than a page worth of data, then invalidate any
    // other pages from the dirty byte range.
    //

    if (WriteLength > PAGE_SIZE) {
        FscInvalidateByteRange(CacheExtension, ByteOffset + PAGE_SIZE,
            WriteLength - PAGE_SIZE);
    }

    //
    // Initialize the contents of this cache buffer.
    //

    RtlFillMemory(CacheBuffer, PAGE_SIZE, (ULONG)-1);

    //
    // Lock down the cache buffer.
    //

    MmLockUnlockBufferPages(CacheBuffer, PAGE_SIZE, FALSE);

    //
    // Copy the page table entry for the cache buffer to the "write 0xFF" PTEs.
    // The cache buffer is locked down, so we can safely map the page multiple
    // times without fear that the page will be relocated out from under us.
    //

    TempPte = *MiGetPteAddress(CacheBuffer);
    PointerPte = FscWriteFFsPtes;

    for (Index = 0; Index < FSCACHE_NUMBER_OF_WRITE_FF_PTES; Index++) {
        MI_WRITE_PTE(PointerPte, TempPte);
        PointerPte++;
    }

    //
    // Write out chunks of pages until we've exhausted the requested write
    // length.
    //

    WriteBytesRemaining = WriteLength;

    do {

        //
        // Compute the number of bytes to write out in this iteration.
        //

        WriteLength = PAGE_SIZE * FSCACHE_NUMBER_OF_WRITE_FF_PTES;

        if (WriteLength > WriteBytesRemaining) {
            WriteLength = WriteBytesRemaining;
        }

        //
        // Initialize the event used to block for completion of the I/O operation
        // and set the completion routine that will signal this event.
        //

        KeInitializeEvent(&IoEvent, NotificationEvent, FALSE);

        IoSetCompletionRoutine(Irp, FscSignalIoEventCompletion, &IoEvent, TRUE,
            TRUE, TRUE);

        //
        // Initialize the next IRP stack location for the I/O operation.
        //

        IrpSp = IoGetNextIrpStackLocation(Irp);

        IrpSp->MajorFunction = IRP_MJ_WRITE;
        IrpSp->Flags = SL_FSCACHE_REQUEST;
        IrpSp->Parameters.Write.ByteOffset.QuadPart = ByteOffset;
        IrpSp->Parameters.Write.Length = WriteLength;
        IrpSp->Parameters.Write.CacheBuffer =
            MiGetVirtualAddressMappedByPte(FscWriteFFsPtes);

        //
        // Submit the request to the target device and wait for the I/O operation to
        // complete.
        //

        status = IoCallDriver(CacheExtension->TargetDeviceObject, Irp);

        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&IoEvent, WrFsCacheOut, KernelMode, FALSE, NULL);
            status = Irp->IoStatus.Status;
        }

        if (!NT_SUCCESS(status)) {
            break;
        }

        ByteOffset += WriteLength;
        WriteBytesRemaining -= WriteLength;

    } while (WriteBytesRemaining > 0);

    //
    // Clear the double mapping from the "write 0xFF" PTEs.
    //

    MiZeroAndFlushPtes(FscWriteFFsPtes, FSCACHE_NUMBER_OF_WRITE_FF_PTES);

    //
    // Unlock the cache buffer.
    //

    MmLockUnlockBufferPages(CacheBuffer, PAGE_SIZE, TRUE);

    //
    // Unmap or discard the buffer.
    //

    if (NT_SUCCESS(status)) {
        FscUnmapBuffer(CacheBuffer);
    } else {
        FscDiscardBuffer(CacheBuffer);
    }

CleanupAndExit:
    RtlLeaveCriticalSection(&FscWriteFFsLock);

    return status;
}

BOOLEAN
FscTestForFullyCachedIo(
    IN PIRP Irp,
    IN ULONG ByteOffsetLowPart,
    IN ULONG IoLength,
    IN BOOLEAN NonCachedEndOfFileTransfer
    )
/*++

Routine Description:

    This routine tests the supplied transfer parameters to see if the transfer
    should be handled through a fully cached path, where all bytes are
    transferred using the file system cache, or a partially cached path, where
    whole pages are transferred using non-cached I/O requests and partial pages
    are transferered using the file system cache.

Arguments:

    Irp - Specifies the packet that describes the I/O request.

    ByteOffsetLowPart - Specifies the lower 32-bits of the transfer byte offset.

    IoLength - Specifies the number of bytes to transfer.

    NonCachedEndOfFileTransfer - Specifies TRUE if the read is to the end of
        file and the output buffer is large enough to hold the transfer length
        rounded up to a sector boundary.

Return Value:

    Returns TRUE if the transfer should go through a fully cached path, else
    FALSE.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    ULONG AlignmentRequirement;
    ULONG EndingByteOffsetLowPart;

    ASSERT(IoLength != 0);

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Check if the file object was opened for random access.  If so, then we'll
    // copy through the cache so that the pages stay resident in the cache as
    // long as possible.
    //

    if (MiIsFlagClear(IrpSp->FileObject->Flags, FO_RANDOM_ACCESS)) {

        //
        // Check if the starting file byte offset or user buffer are misaligned.
        // If so, then we need to go through the fully cached path since the
        // hardware won't be able to directly deal with these buffers.
        //

        AlignmentRequirement = IrpSp->DeviceObject->AlignmentRequirement;

        if (((IrpSp->Parameters.Read.ByteOffset.LowPart & AlignmentRequirement) == 0) &&
            ((BYTE_OFFSET(Irp->UserBuffer) & AlignmentRequirement) == 0)) {

            if (NonCachedEndOfFileTransfer) {

                //
                // Compute the ending byte offset truncated down to the nearest
                // page and the starting byte offset truncated down to the
                // nearest page.  If these aren't the same byte offsets, then
                // there's some number of bytes on a different cache page than
                // the starting byte offset, so use the partially cached paths
                // to avoid using the cache for any middle pages or the partial
                // tail page.
                //
                // If we've reached this code, then we already know that this is
                // a read request and that the transfer buffer is large enough
                // to handle a partial sector I/O transfer.
                //

                if (BYTE_OFFSET(ByteOffsetLowPart) == 0) {
                    return FALSE;
                }

                EndingByteOffsetLowPart = ByteOffsetLowPart + IoLength;

                if ((BYTE_OFFSET(EndingByteOffsetLowPart) != 0) &&
                    (PAGE_ALIGN(ByteOffsetLowPart) != PAGE_ALIGN(EndingByteOffsetLowPart))) {
                    return FALSE;
                }

            } else if (IoLength >= PAGE_SIZE) {

                //
                // Compute the ending byte offset truncated down to the nearest
                // page and the starting byte offset rounded up to the nearest
                // page.  If these aren't the same byte offsets, then the
                // transfer contains at least one whole page, so use the
                // partially cached paths to avoid using the cache for these
                // middle pages.
                //
                // We only go through this code for the case where the transfer
                // length is greater than a page to handle the case of the
                // starting and ending byte offset residing on the same cache
                // page.
                //

                EndingByteOffsetLowPart = (ULONG)PAGE_ALIGN(ByteOffsetLowPart + IoLength);
                ByteOffsetLowPart = ROUND_TO_PAGES(ByteOffsetLowPart);

                if (ByteOffsetLowPart != EndingByteOffsetLowPart) {
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

NTSTATUS
FscPartiallyCachedRead(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN PIRP Irp,
    IN ULONGLONG ByteOffset,
    IN ULONG ReadLength,
    IN ULONG BufferByteOffset,
    IN BOOLEAN NonCachedEndOfFileTransfer
    )
/*++

Routine Description:

    This routine reads the supplied number of bytes from the supplied starting
    byte offset to IRP.UserBuffer plus the supplied buffer byte offset.  This
    routine reads whole pages using direct I/O into the read buffer and,
    if necessary, reads the tail page using a file system cache buffer.

Arguments:

    CacheExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    ByteOffset - Specifies the byte offset to start reading from.  The byte
        offset must be page aligned.

    ReadLength - Specifies the number of bytes to read.  The number of bytes
        must be greater than or equal to the page size.

    BufferByteOffset - Specifies the offset into IRP.UserBuffer to start reading
        the data.

    NonCachedEndOfFileTransfer - Specifies TRUE if the read is to the end of
        file and the output buffer is large enough to hold the transfer length
        rounded up to a sector boundary.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONG PartialReadLength;
    KEVENT IoEvent;
    PIO_STACK_LOCATION IrpSp;
    PVOID CacheBuffer;

    ASSERT(BYTE_OFFSET(ByteOffset) == 0);

    //
    // If NonCachedEndOfFileTransfer is TRUE, then we're reading to the end of
    // the file and the transfer buffer is large enough to hold the transfer
    // length rounded up to a sector boundary.
    //

    if (NonCachedEndOfFileTransfer) {
        ReadLength = FscRoundToSectors(CacheExtension, ReadLength);
        PartialReadLength = ReadLength;
    } else {
        ASSERT(ReadLength >= PAGE_SIZE);
        PartialReadLength = (ULONG)PAGE_ALIGN(ReadLength);
    }

    //
    // Initialize the event used to block for completion of the I/O operation
    // and set the completion routine that will signal this event.
    //

    KeInitializeEvent(&IoEvent, NotificationEvent, FALSE);

    IoSetCompletionRoutine(Irp, FscSignalIoEventCompletion, &IoEvent, TRUE,
        TRUE, TRUE);

    //
    // Initialize the next IRP stack location for the I/O operation.
    //

    IrpSp = IoGetNextIrpStackLocation(Irp);

    IrpSp->MajorFunction = IRP_MJ_READ;
    IrpSp->Parameters.Read.ByteOffset.QuadPart = ByteOffset;
    IrpSp->Parameters.Read.Length = PartialReadLength;
    IrpSp->Parameters.Read.BufferOffset = BufferByteOffset;

    //
    // Submit the request to the target device and wait for the I/O operation to
    // complete.
    //

    status = IoCallDriver(CacheExtension->TargetDeviceObject, Irp);

    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(&IoEvent, WrFsCacheIn, KernelMode, FALSE, NULL);
        status = Irp->IoStatus.Status;
    }

    //
    // Transfer the remaining non whole page of the request if necessary.
    //

    if (NT_SUCCESS(status)) {

        ReadLength -= PartialReadLength;

        if (ReadLength > 0) {

            ByteOffset += PartialReadLength;
            BufferByteOffset += PartialReadLength;

            status = FscMapBuffer(CacheExtension, Irp, ByteOffset, FALSE,
                &CacheBuffer);

            if (NT_SUCCESS(status)) {
                RtlCopyMemory((PUCHAR)Irp->UserBuffer + BufferByteOffset,
                    CacheBuffer, ReadLength);
                FscUnmapBuffer(CacheBuffer);
            }
        }
    }

    return status;
}

NTSTATUS
FscCachedRead(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN PIRP Irp,
    IN ULONGLONG ByteOffset,
    IN ULONG ReadLength,
    IN BOOLEAN NonCachedEndOfFileTransfer
    )
/*++

Routine Description:

    This routine reads the supplied number of bytes from the supplied starting
    byte offset to IRP.UserBuffer.

Arguments:

    CacheExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    ByteOffset - Specifies the byte offset to start reading from.

    ReadLength - Specifies the number of bytes to read.

    NonCachedEndOfFileTransfer - Specifies TRUE if the read is to the end of
        file and the output buffer is large enough to hold the transfer length
        rounded up to a sector boundary.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PIO_STACK_LOCATION IrpSp;
    PVOID UserBuffer;
    ULONG BytesRemaining;
    PVOID CacheBuffer;
    ULONG BytesToCopy;
    ULONG BufferByteOffset;
    ULONG PartialIoLength;
    PFILE_OBJECT FileObject;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    UserBuffer = Irp->UserBuffer;
    BytesRemaining = ReadLength;

    //
    // Check if this transfer should use the fully or partially cached path.
    //

    if (FscTestForFullyCachedIo(Irp, (ULONG)ByteOffset, ReadLength,
        NonCachedEndOfFileTransfer)) {

        //
        // Map each page into the cache and copy the contents into the user's
        // buffer.
        //

        do {

            //
            // Map the physical byte offset into the cache.
            //

            status = FscMapBuffer(CacheExtension, Irp, ByteOffset, FALSE,
                &CacheBuffer);

            if (!NT_SUCCESS(status)) {
                return status;
            }

            //
            // Compute the number of bytes to copy from this cache page.
            //

            BytesToCopy = PAGE_SIZE - BYTE_OFFSET(ByteOffset);

            if (BytesToCopy > BytesRemaining) {
                BytesToCopy = BytesRemaining;
            }

            //
            // Copy the bytes from the cache page and unmap the cache buffer.
            //

            RtlCopyMemory(UserBuffer, CacheBuffer, BytesToCopy);
            FscUnmapBuffer(CacheBuffer);

            //
            // Advance to the next physical byte offset, which now will be page
            // aligned, and adjust our other loop variables.
            //

            ByteOffset = (ByteOffset + PAGE_SIZE) & ~(PAGE_SIZE - 1);
            UserBuffer = (PUCHAR)UserBuffer + BytesToCopy;
            BytesRemaining -= BytesToCopy;

        } while (BytesRemaining > 0);

    } else {

        //
        // Lock the user's buffer into memory if necessary.
        //

        IoLockUserBuffer(Irp,
            IoGetCurrentIrpStackLocation(Irp)->Parameters.Read.Length);

        //
        // Transfer the head of the request if it's not page aligned.
        //

        if (BYTE_OFFSET(ByteOffset) != 0) {

            PartialIoLength = PAGE_SIZE - BYTE_OFFSET(ByteOffset);

            ASSERT(PartialIoLength < BytesRemaining);

            //
            // Map the physical byte offset into the cache.
            //

            status = FscMapBuffer(CacheExtension, Irp, ByteOffset, FALSE,
                &CacheBuffer);

            if (!NT_SUCCESS(status)) {
                return status;
            }

            //
            // Copy the bytes from the cache page and unmap the cache buffer.
            //

            RtlCopyMemory(UserBuffer, CacheBuffer, PartialIoLength);
            FscUnmapBuffer(CacheBuffer);

            //
            // Advance to the next physical byte offset, which now will be page
            // aligned.
            //

            BytesRemaining -= PartialIoLength;
            ByteOffset += PartialIoLength;
            BufferByteOffset = PartialIoLength;

        } else {
            BufferByteOffset = 0;
        }

        ASSERT(BYTE_OFFSET(ByteOffset) == 0);

        //
        // Transfer the body and tail of the request using the file system
        // cache to handle the tail page.
        //

        status = FscPartiallyCachedRead(CacheExtension, Irp, ByteOffset,
            BytesRemaining, BufferByteOffset, NonCachedEndOfFileTransfer);

        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    //
    // If the file is open for synchronous I/O, then we need to update the
    // current file position.
    //

    FileObject = IrpSp->FileObject;

    if (MiIsFlagSet(FileObject->Flags, FO_SYNCHRONOUS_IO)) {
        FileObject->CurrentByteOffset.QuadPart =
            IrpSp->Parameters.Read.ByteOffset.QuadPart + ReadLength;
    }

    //
    // Fill in the number of bytes actually read.
    //

    Irp->IoStatus.Information = ReadLength;

    return STATUS_SUCCESS;
}

VOID
FscInvalidateBlocks(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN ULONG BlockNumber,
    IN ULONG EndingBlockNumber
    )
/*++

Routine Description:

    This routine invalidates any buffers in the supplied byte range for the
    supplied cache extension.

Arguments:

    CacheExtension - Specifies the cache extension to invalidate.

    BlockNumber - Specifies the block number to start invalidating buffers from.

    EndingBlockNumber - Specifies the block number, inclusive, to stop
        invalidating buffers from.

Return Value:

    None.

--*/
{
    KIRQL OldIrql;
    PLIST_ENTRY NextListEntry;
    PFSCACHE_ELEMENT Element;

    MI_LOCK_MM(&OldIrql);

    NextListEntry = FscLruList.Blink;

    while (NextListEntry != &FscLruList) {

        Element = CONTAINING_RECORD(NextListEntry, FSCACHE_ELEMENT, ListEntry);
        NextListEntry = Element->ListEntry.Blink;

        //
        // If we find an element with an empty cache extension, then the element
        // doesn't reference valid data.  All free elements are at the front of
        // the LRU list, so we know that we can stop the search now.
        //

        if (Element->CacheExtension == NULL) {
            break;
        }

        //
        // Does this element hold data from the given cache extension and block
        // number range and is the element idle?
        //

        if ((Element->CacheExtension == CacheExtension) &&
            (Element->BlockNumber >= BlockNumber) &&
            (Element->BlockNumber <= EndingBlockNumber) &&
            (Element->UsageCount == 0)) {

            ASSERT(!Element->DeletePending);
            ASSERT(Element->UsageCount == 0);

            //
            // We found a cache element with the desired attributes.  Mark the
            // element as free and move it to the beginning of the LRU list.
            //

            Element->CacheExtension = NULL;

            RemoveEntryList(&Element->ListEntry);
            InsertHeadList(&FscLruList, &Element->ListEntry);

            //
            // Release another waiter for a free element if necessary.
            //

            if (!IsListEmpty(&FscWaitingForElementEvent.Header.WaitListHead)) {
                KeSetEvent(&FscWaitingForElementEvent, 0, FALSE);
            }
        }
    }

    MI_UNLOCK_MM(OldIrql);
}

VOID
FscInvalidateDevice(
    IN PFSCACHE_EXTENSION CacheExtension
    )
/*++

Routine Description:

    This routine invalidates any buffers for the supplied cache extension.

Arguments:

    CacheExtension - Specifies the cache extension to invalidate.

Return Value:

    None.

--*/
{
    //
    // Invalidate all blocks for the supplied cache extension.
    //

    FscInvalidateBlocks(CacheExtension, 0, MAXULONG);
}

VOID
FscInvalidateByteRange(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN ULONGLONG ByteOffset,
    IN ULONG Length
    )
/*++

Routine Description:

    This routine invalidates any buffers in the supplied byte range for the
    supplied cache extension.

    This routine assumes that it is called from within an exclusive writer lock
    for the supplied cache extension.  No buffers from the supplied cache
    extension map be actively mapped.

Arguments:

    CacheExtension - Specifies the cache extension to invalidate.

    ByteOffset - Specifies the byte offset to start invalidating buffers from.

    Length - Specifies the number of bytes to invalidate.

Return Value:

    None.

--*/
{
    ULONG BlockNumber;
    ULONG EndingBlockNumber;

    ASSERT(Length != 0);

    BlockNumber = FscByteOffsetToBlockNumber(ByteOffset);
    EndingBlockNumber = FscByteOffsetToBlockNumber(ByteOffset + Length - 1);

    FscInvalidateBlocks(CacheExtension, BlockNumber, EndingBlockNumber);
}

VOID
FscInvalidateIdleBlocks(
    VOID
    )
/*++

Routine Description:

    This routine invalidates any buffers that are not currently marked as busy.

Arguments:

    None.

Return Value:

    None.

--*/
{
    KIRQL OldIrql;
    PLIST_ENTRY NextListEntry;
    PFSCACHE_ELEMENT Element;

    MI_LOCK_MM(&OldIrql);

    NextListEntry = FscLruList.Blink;

    while (NextListEntry != &FscLruList) {

        Element = CONTAINING_RECORD(NextListEntry, FSCACHE_ELEMENT, ListEntry);
        NextListEntry = Element->ListEntry.Blink;

        //
        // If we find an element with an empty cache extension, then the element
        // doesn't reference valid data.  All free elements are at the front of
        // the LRU list, so we know that we can stop the search now.
        //

        if (Element->CacheExtension == NULL) {
            break;
        }

        //
        // Is the element idle?
        //

        if (Element->UsageCount == 0) {

            ASSERT(!Element->DeletePending);

            //
            // We found a cache element with the desired attributes.  Mark the
            // element as free and move it to the beginning of the LRU list.
            //

            Element->CacheExtension = NULL;

            RemoveEntryList(&Element->ListEntry);
            InsertHeadList(&FscLruList, &Element->ListEntry);

            //
            // Release another waiter for a free element if necessary.
            //

            if (!IsListEmpty(&FscWaitingForElementEvent.Header.WaitListHead)) {
                KeSetEvent(&FscWaitingForElementEvent, 0, FALSE);
            }
        }
    }

    MI_UNLOCK_MM(OldIrql);
}
