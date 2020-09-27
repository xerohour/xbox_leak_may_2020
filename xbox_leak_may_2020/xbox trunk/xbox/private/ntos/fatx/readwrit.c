/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    readwrit.c

Abstract:

    This module implements routines related to handling IRP_MJ_READ and
    IRP_MJ_WRITE.

--*/

#include "fatx.h"

//
// Local support.
//

VOID
FatxStartNextAsynchronousIoEntry(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_ASYNC_IO_DESCRIPTOR AsyncIoDescriptor
    );

NTSTATUS
FatxVolumeIoCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:

    This routine is called to process the completion of a volume I/O transfer.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    Context - Specifies the context that was supplied to IoSetCompletionRoutine.

Return Value:

    Status of operation.

--*/
{
    PFAT_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    PFILE_OBJECT FileObject;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Propagate the pending flag up the IRP stack.
    //

    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }

    //
    // Check if this is a read or write completion.  This completion routine is
    // shared by other IRP dispatch routines.
    //

    if ((IrpSp->MajorFunction == IRP_MJ_READ) ||
        (IrpSp->MajorFunction == IRP_MJ_WRITE)) {

        if (NT_SUCCESS(Irp->IoStatus.Status)) {

            ASSERT(Irp->IoStatus.Information > 0);
            ASSERT(FatxIsSectorAligned(VolumeExtension, Irp->IoStatus.Information));

            //
            // If the file is open for synchronous I/O, then we need to update
            // the current file position.
            //

            FileObject = IrpSp->FileObject;

            if (FatxIsFlagSet(FileObject->Flags, FO_SYNCHRONOUS_IO)) {
                FileObject->CurrentByteOffset.QuadPart =
                    IrpSp->Parameters.Read.ByteOffset.QuadPart +
                    Irp->IoStatus.Information;
            }
        }
    }

    FatxDpcReleaseVolumeMutex(VolumeExtension);

    return STATUS_SUCCESS;
}

NTSTATUS
FatxSignalIoEventCompletion(
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
FatxSynchronousIoTail(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN UCHAR MajorFunction,
    IN PFILE_OBJECT FileObject,
    IN ULONG FileByteOffset,
    IN ULONG IoLength
    )
/*++

Routine Description:

    This routine is called at the tail of the non-cached and cached synchronous
    I/O routines to update the state in the file control block and file objects.

    This routine is only called on success from these other routines.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    MajorFunction - Specifies the function to be performed; either IRP_MJ_READ
        or IRP_MJ_WRITE.

    FileObject - Specifies the file object that the I/O request is for.

    FileByteOffset - Specifies the file byte offset where the transfer started
        from.

    IoLength - Specifies the number of bytes that were transferred.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PFAT_FCB FileFcb;
    ULONG EndingByteOffset;
    ULONG OriginalFileSize;

    FileFcb = (PFAT_FCB)FileObject->FsContext;

    EndingByteOffset = FileByteOffset + IoLength;

    //
    // If we've written to the file, then update the last write time.  The last
    // write time is lazily flushed to the directory entry.  We need to acquire
    // the volume mutex for exclusive access to synchronize with
    // FatxFsdSetInformation and to write out any directory changes below.
    //

    if (MajorFunction == IRP_MJ_WRITE) {

        FatxAcquireVolumeMutexExclusive(VolumeExtension);

        if (FatxIsFlagClear(FileFcb->Flags, FAT_FCB_DISABLE_LAST_WRITE_TIME)) {
            KeQuerySystemTime(&FileFcb->LastWriteTime);
            FileFcb->Flags |= FAT_FCB_UPDATE_DIRECTORY_ENTRY;
        }

        //
        // If the ending byte offset is beyond the current size of the file,
        // then we've extended the file.  Change the file size to the new size
        // and commit the directory change.
        //

        if (EndingByteOffset > FileFcb->FileSize) {

            OriginalFileSize = FileFcb->FileSize;
            FileFcb->FileSize = EndingByteOffset;

            status = FatxUpdateDirectoryEntry(VolumeExtension, Irp, FileFcb);

            if (!NT_SUCCESS(status)) {
                FileFcb->FileSize = OriginalFileSize;
                FatxReleaseVolumeMutex(VolumeExtension);
                return status;
            }
        }

        FatxReleaseVolumeMutex(VolumeExtension);
    }

    //
    // If the file is open for synchronous I/O, then we need to update the
    // current file position.
    //

    if (FatxIsFlagSet(FileObject->Flags, FO_SYNCHRONOUS_IO)) {
        FileObject->CurrentByteOffset.LowPart = EndingByteOffset;
        FileObject->CurrentByteOffset.HighPart = 0;
    }

    //
    // Fill in the number of bytes of transferred.  This number may be less than
    // the actual number of modified bytes in the buffer if we're at the end of
    // file.
    //

    Irp->IoStatus.Information = IoLength;

    return STATUS_SUCCESS;
}

NTSTATUS
FatxNonCachedSynchronousIo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN UCHAR MajorFunction,
    IN PFILE_OBJECT FileObject,
    IN ULONG FileByteOffset,
    IN ULONG BufferByteOffset,
    IN ULONG IoLength,
    IN BOOLEAN PartialTransfer
    )
/*++

Routine Description:

    This routine is called to synchronously read or write a sector aligned
    buffer from or to the target device.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    MajorFunction - Specifies the function to be performed; either IRP_MJ_READ
        or IRP_MJ_WRITE.

    FileObject - Specifies the file object that the I/O request is for.

    FileByteOffset - Specifies the file byte offset to start the transfer from.

    BufferByteOffset - Specifies the buffer byte offset ot start the transfer
        from.

    IoLength - Specifies the number of bytes to transfer.

    PartialTransfer - TRUE if called from FatxPartiallyCachedSynchronousIo, else
        FALSE.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PFAT_VOLUME_EXTENSION VolumeExtension;
    PFAT_FCB Fcb;
    PIO_STACK_LOCATION NextIrpSp;
    ULONG OriginalFileByteOffset;
    ULONG IoLengthRemaining;
    ULONGLONG PhysicalByteOffset;
    ULONG PhysicalRunLength;
    ULONGLONG StartingPhysicalByteOffset;
    ULONG PhysicalIoLength;
    ULONGLONG AdjacentPhysicalByteOffset;
    KEVENT IoEvent;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    Fcb = (PFAT_FCB)FileObject->FsContext;
    NextIrpSp = IoGetNextIrpStackLocation(Irp);
    OriginalFileByteOffset = FileByteOffset;
    IoLengthRemaining = FatxRoundToSectors(VolumeExtension, IoLength);

    ASSERT(IoLengthRemaining > 0);
    ASSERT(FatxIsSectorAligned(VolumeExtension, FileByteOffset));

    //
    // Get the physical byte offset corresponding to the starting byte offset.
    //

    status = FatxFileByteOffsetToPhysicalByteOffset(VolumeExtension, Irp, Fcb,
        FileByteOffset, TRUE, &PhysicalByteOffset, &PhysicalRunLength);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Do the transfer.
    //

    do {

        StartingPhysicalByteOffset = PhysicalByteOffset;

        //
        // Compute how many physical bytes we can process in this pass.  The
        // cluster cache doesn't follow adjacent clusters that cross cache pages
        // so we handle that logic here by increasing the physical I/O length
        // until we find a non-adjacent cluster run.  This non-adjacent cluster
        // run is the starting cluster for the next iteration of the outer loop.
        //

        PhysicalIoLength = 0;

        do {

            //
            // Limit the number of bytes in the physical run to the number of
            // bytes left to process.
            //

            if (PhysicalRunLength > IoLengthRemaining) {
                PhysicalRunLength = IoLengthRemaining;
                IoLengthRemaining = 0;
            } else {
                IoLengthRemaining -= PhysicalRunLength;
            }

            //
            // Update the number of physical bytes we can process and the offset
            // into the file.
            //

            PhysicalIoLength += PhysicalRunLength;
            FileByteOffset += PhysicalRunLength;

            if (IoLengthRemaining == 0) {
                break;
            }

            //
            // Compute the physical byte offset for the adjacent run.
            //

            AdjacentPhysicalByteOffset = PhysicalByteOffset + PhysicalRunLength;

            //
            // Obtain the next physical run.
            //

            status = FatxFileByteOffsetToPhysicalByteOffset(VolumeExtension,
                Irp, Fcb, FileByteOffset, TRUE, &PhysicalByteOffset,
                &PhysicalRunLength);

            if (!NT_SUCCESS(status)) {
                return status;
            }

            //
            // If the next physical byte offset isn't adjacent to the last
            // physical run, then we're done.
            //

        } while (PhysicalByteOffset == AdjacentPhysicalByteOffset);

        //
        // Invalidate any file system cache buffers for this byte range if this
        // is a write operation.
        //

        if (MajorFunction == IRP_MJ_WRITE) {
            FscInvalidateByteRange(&VolumeExtension->CacheExtension,
                StartingPhysicalByteOffset, PhysicalIoLength);
        }

        //
        // Initialize (or reinitialize) the synchronization event we'll use to
        // block for the port driver to handle the I/O.
        //

        KeInitializeEvent(&IoEvent, SynchronizationEvent, FALSE);

        //
        // Fill in the starting physical byte offset and the number of bytes to
        // transfer.
        //

        NextIrpSp->Parameters.Read.ByteOffset.QuadPart =
            StartingPhysicalByteOffset;
        NextIrpSp->Parameters.Read.Length = PhysicalIoLength;

        //
        // Fill in the offset into the buffer to start the transfer and update
        // that offset by the number of bytes we'll physically transfer.
        //

        NextIrpSp->Parameters.Read.BufferOffset = BufferByteOffset;
        BufferByteOffset += PhysicalIoLength;

        //
        // Fill in the header for the stack location.  This clears out the
        // MinorFunction, Control, and Flags field as well.
        //

        *((PULONG)&NextIrpSp->MajorFunction) = MajorFunction;

        //
        // Set the completion routine that will signal our synchronization
        // event.
        //

        IoSetCompletionRoutine(Irp, FatxSignalIoEventCompletion, &IoEvent, TRUE,
            TRUE, TRUE);

        //
        // Call down to the target device and block for the I/O to complete.
        //

        status = IoCallDriver(VolumeExtension->TargetDeviceObject, Irp);

        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&IoEvent, Executive, KernelMode, FALSE, NULL);
            status = Irp->IoStatus.Status;
        }

        if (!NT_SUCCESS(status)) {
            return status;
        }

        //
        // Assert that the device handled as many bytes as we programmed it to.
        //

        ASSERT(Irp->IoStatus.Information == PhysicalIoLength);

    } while (IoLengthRemaining > 0);

    if (!PartialTransfer) {
        status = FatxSynchronousIoTail(VolumeExtension, Irp, MajorFunction,
            FileObject, OriginalFileByteOffset, IoLength);
    } else {
        status = STATUS_SUCCESS;
    }

    return status;
}

NTSTATUS
FatxNonCachedAsynchronousIoCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:

    This routine is called to process the completion of a non-cached
    asynchronous I/O transfer.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    Context - Specifies the context that was supplied to IoSetCompletionRoutine.

Return Value:

    Status of operation.

--*/
{
    PFAT_VOLUME_EXTENSION VolumeExtension;
    PFAT_ASYNC_IO_DESCRIPTOR AsyncIoDescriptor;
    PFAT_FCB FileFcb;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    AsyncIoDescriptor = (PFAT_ASYNC_IO_DESCRIPTOR)Context;

    if (NT_SUCCESS(Irp->IoStatus.Status)) {

        //
        // If there are still more bytes to transfer, then start the entry and
        // bail out.
        //

        if (AsyncIoDescriptor->IoLengthRemaining > 0) {
            FatxStartNextAsynchronousIoEntry(VolumeExtension, Irp,
                AsyncIoDescriptor);
            return STATUS_MORE_PROCESSING_REQUIRED;
        }

        //
        // If this was a file write, then we need to update the last write time.
        //

        if (AsyncIoDescriptor->MajorFunction == IRP_MJ_WRITE) {

            FileFcb = AsyncIoDescriptor->FileFcb;

            if (FatxIsFlagClear(FileFcb->Flags, FAT_FCB_DISABLE_LAST_WRITE_TIME)) {
                KeQuerySystemTime(&FileFcb->LastWriteTime);
                FileFcb->Flags |= FAT_FCB_UPDATE_DIRECTORY_ENTRY;
            }
        }

        //
        // Fill in the number of bytes of transferred.  This number may be less
        // than the actual number of modified bytes in the buffer if we're at
        // the end of file.
        //

        Irp->IoStatus.Information = AsyncIoDescriptor->IoLength;
    }

    FatxDecrementDismountBlockCount(VolumeExtension);
    FatxDpcReleaseFileMutex(AsyncIoDescriptor->FileFcb);

    ExFreePool(AsyncIoDescriptor);

    return STATUS_SUCCESS;
}

VOID
FatxStartNextAsynchronousIoEntry(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_ASYNC_IO_DESCRIPTOR AsyncIoDescriptor
    )
/*++

Routine Description:

    This routine is called to start the next read or write from the supplied
    asynchronous I/O descriptor.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    AsyncIoDescriptor - Specifies the asynchronous I/O descriptor that indicates
        the next starting physical sector and transfer length.

Return Value:

    None.

--*/
{
    PIO_STACK_LOCATION NextIrpSp;
    PFAT_ASYNC_IO_ENTRY AsyncIoEntry;
    ULONG PhysicalIoLength;

    NextIrpSp = IoGetNextIrpStackLocation(Irp);
    AsyncIoEntry = &AsyncIoDescriptor->Entries[AsyncIoDescriptor->NextAsyncIoEntry++];
    PhysicalIoLength = AsyncIoEntry->PhysicalIoLength;

    //
    // Adjust the number of bytes remaining in the transfer.
    //

    AsyncIoDescriptor->IoLengthRemaining -= PhysicalIoLength;

    //
    // Fill in the starting physical byte offset and the number of bytes to
    // transfer.
    //

    NextIrpSp->Parameters.Read.ByteOffset.QuadPart =
        (ULONGLONG)AsyncIoEntry->PhysicalSector << VolumeExtension->SectorShift;
    NextIrpSp->Parameters.Read.Length = PhysicalIoLength;

    //
    // Fill in the offset into the buffer to start the transfer and update
    // that offset by the number of bytes we'll physically transfer.
    //

    NextIrpSp->Parameters.Read.BufferOffset = AsyncIoDescriptor->BufferOffset;
    AsyncIoDescriptor->BufferOffset += PhysicalIoLength;

    //
    // Fill in the header for the stack location.  This clears out the
    // MinorFunction, Control, and Flags field as well.
    //

    *((PULONG)&NextIrpSp->MajorFunction) = AsyncIoDescriptor->MajorFunction;

    //
    // Set the completion routine that will start the next entry or finish the
    // transfer.
    //

    IoSetCompletionRoutine(Irp, FatxNonCachedAsynchronousIoCompletion,
        AsyncIoDescriptor, TRUE, TRUE, TRUE);

    //
    // Call down to the target device.
    //

    IoCallDriver(VolumeExtension->TargetDeviceObject, Irp);
}

NTSTATUS
FatxNonCachedAsynchronousIo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN UCHAR MajorFunction,
    IN PFILE_OBJECT FileObject,
    IN ULONG FileByteOffset,
    IN ULONG IoLength
    )
/*++

Routine Description:

    This routine is called to asynchronously read or write a sector aligned
    buffer from or to the target device.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    MajorFunction - Specifies the function to be performed; either IRP_MJ_READ
        or IRP_MJ_WRITE.

    FileObject - Specifies the file object that the I/O request is for.

    FileByteOffset - Specifies the file byte offset to start the transfer from.

    IoLength - Specifies the number of bytes to transfer.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PFAT_VOLUME_EXTENSION VolumeExtension;
    PFAT_FCB Fcb;
    ULONG IoLengthRemaining;
    SIZE_T AsyncIoDescriptorSize;
    PFAT_ASYNC_IO_DESCRIPTOR AsyncIoDescriptor;
    ULONGLONG PhysicalByteOffset;
    ULONG PhysicalRunLength;
    PFAT_ASYNC_IO_ENTRY AsyncIoEntry;
    ULONGLONG StartingPhysicalByteOffset;
    ULONG PhysicalIoLength;
    ULONGLONG AdjacentPhysicalByteOffset;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    Fcb = (PFAT_FCB)FileObject->FsContext;
    IoLengthRemaining = FatxRoundToSectors(VolumeExtension, IoLength);

    ASSERT(IoLengthRemaining > 0);
    ASSERT(FatxIsSectorAligned(VolumeExtension, FileByteOffset));

    //
    // Always assume the worst case for the state of the file.  If the file is
    // highly fragmented, then we'll need one entry per cluster.  If the file
    // isn't fragmented, then we'll end up wasting memory, but the application
    // will always be charged a consistent number of bytes per transfer.
    //

    AsyncIoDescriptorSize = sizeof(FAT_ASYNC_IO_DESCRIPTOR) +
        ((IoLengthRemaining >> VolumeExtension->ClusterShift) + 1) *
        sizeof(FAT_ASYNC_IO_ENTRY);

    AsyncIoDescriptor = ExAllocatePoolWithTag(AsyncIoDescriptorSize, 'dAtF');

    if (AsyncIoDescriptor == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Initialize the asynchronous I/O descriptor.
    //

    AsyncIoDescriptor->MajorFunction = MajorFunction;
    AsyncIoDescriptor->IoLength = IoLength;
    AsyncIoDescriptor->IoLengthRemaining = IoLengthRemaining;
    AsyncIoDescriptor->BufferOffset = 0;
    AsyncIoDescriptor->NextAsyncIoEntry = 0;
    AsyncIoDescriptor->FileFcb = Fcb;

    //
    // Get the physical byte offset corresponding to the starting byte offset.
    //

    status = FatxFileByteOffsetToPhysicalByteOffset(VolumeExtension, Irp, Fcb,
        FileByteOffset, TRUE, &PhysicalByteOffset, &PhysicalRunLength);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Fill in the entries of the asynchronous I/O descriptor.
    //

    AsyncIoEntry = AsyncIoDescriptor->Entries;

    do {

        StartingPhysicalByteOffset = PhysicalByteOffset;

        //
        // Compute how many physical bytes we can process in this pass.  The
        // cluster cache doesn't follow adjacent clusters that cross cache pages
        // so we handle that logic here by increasing the physical I/O length
        // until we find a non-adjacent cluster run.  This non-adjacent cluster
        // run is the starting cluster for the next iteration of the outer loop.
        //

        PhysicalIoLength = 0;

        do {

            //
            // Limit the number of bytes in the physical run to the number of
            // bytes left to process.
            //

            if (PhysicalRunLength > IoLengthRemaining) {
                PhysicalRunLength = IoLengthRemaining;
                IoLengthRemaining = 0;
            } else {
                IoLengthRemaining -= PhysicalRunLength;
            }

            //
            // Update the number of physical bytes we can process and the offset
            // into the file.
            //

            PhysicalIoLength += PhysicalRunLength;
            FileByteOffset += PhysicalRunLength;

            if (IoLengthRemaining == 0) {
                break;
            }

            //
            // Compute the physical byte offset for the adjacent run.
            //

            AdjacentPhysicalByteOffset = PhysicalByteOffset + PhysicalRunLength;

            //
            // Obtain the next physical run.
            //

            status = FatxFileByteOffsetToPhysicalByteOffset(VolumeExtension,
                Irp, Fcb, FileByteOffset, TRUE, &PhysicalByteOffset,
                &PhysicalRunLength);

            if (!NT_SUCCESS(status)) {
                ExFreePool(AsyncIoDescriptor);
                return status;
            }

            //
            // If the next physical byte offset isn't adjacent to the last
            // physical run, then we're done.
            //

        } while (PhysicalByteOffset == AdjacentPhysicalByteOffset);

        ASSERT(FatxIsSectorAligned(VolumeExtension, PhysicalIoLength));

        //
        // Invalidate any file system cache buffers for this byte range if this
        // is a write operation.
        //

        if (MajorFunction == IRP_MJ_WRITE) {
            FscInvalidateByteRange(&VolumeExtension->CacheExtension,
                StartingPhysicalByteOffset, PhysicalIoLength);
        }

        //
        // Fill in the starting physical byte offset and the number of bytes to
        // transfer.
        //

        AsyncIoEntry->PhysicalSector = (ULONG)(StartingPhysicalByteOffset >>
            VolumeExtension->SectorShift);
        AsyncIoEntry->PhysicalIoLength = PhysicalIoLength;
        AsyncIoEntry++;

    } while (IoLengthRemaining > 0);

    //
    // Start transferring the first entry in the asynchronous I/O descriptor.
    //

    IoMarkIrpPending(Irp);

    FatxStartNextAsynchronousIoEntry(VolumeExtension, Irp,
        AsyncIoDescriptor);

    return STATUS_PENDING;
}

NTSTATUS
FatxFullyCachedSynchronousIo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN UCHAR MajorFunction,
    IN PFILE_OBJECT FileObject,
    IN ULONG FileByteOffset,
    IN ULONG BufferByteOffset,
    IN ULONG IoLength,
    IN BOOLEAN PartialTransfer
    )
/*++

Routine Description:

    This routine is called to synchronously read or write a buffer through the
    file system cache.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    MajorFunction - Specifies the function to be performed; either IRP_MJ_READ
        or IRP_MJ_WRITE.

    FileObject - Specifies the file object that the I/O request is for.

    FileByteOffset - Specifies the file byte offset to start the transfer from.

    BufferByteOffset - Specifies the buffer byte offset ot start the transfer
        from.

    IoLength - Specifies the number of bytes to transfer.

    PartialTransfer - TRUE if called from FatxPartiallyCachedSynchronousIo, else
        FALSE.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PFAT_VOLUME_EXTENSION VolumeExtension;
    PFAT_FCB Fcb;
    ULONG OriginalFileByteOffset;
    ULONG IoLengthRemaining;
    PVOID UserBuffer;
    ULONGLONG PhysicalByteOffset;
    ULONG PhysicalRunLength;
    ULONG PhysicalRunBytesRemaining;
    PVOID CacheBuffer;
    ULONG BytesToCopy;
    BOOLEAN MapEmptyBuffer;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    Fcb = (PFAT_FCB)FileObject->FsContext;
    OriginalFileByteOffset = FileByteOffset;
    IoLengthRemaining = IoLength;
    UserBuffer = (PUCHAR)Irp->UserBuffer + BufferByteOffset;

    ASSERT(IoLengthRemaining > 0);

    //
    // Get the physical byte offset corresponding to the starting byte offset.
    //

    status = FatxFileByteOffsetToPhysicalByteOffset(VolumeExtension, Irp, Fcb,
        FileByteOffset, TRUE, &PhysicalByteOffset, &PhysicalRunLength);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Do the transfer.
    //

    for (;;) {

        PhysicalRunBytesRemaining = PhysicalRunLength;

        do {

            //
            // Compute the number of bytes remaining on this cache buffer.
            //

            BytesToCopy = PAGE_SIZE - BYTE_OFFSET(PhysicalByteOffset);

            //
            // Adjust the number of bytes remaining in this physical run and the
            // next cache byte offset depending on whether we're near the end of
            // the run or not.
            //

            if (BytesToCopy < PhysicalRunBytesRemaining) {
                PhysicalRunBytesRemaining -= BytesToCopy;
            } else {
                BytesToCopy = PhysicalRunBytesRemaining;
                PhysicalRunBytesRemaining = 0;
            }

            //
            // Limit the number of bytes copied to the number of bytes we
            // actually need.
            //

            if (BytesToCopy > IoLengthRemaining) {
                BytesToCopy = IoLengthRemaining;
            }

            if (MajorFunction == IRP_MJ_WRITE) {

                //
                // If we're going to be writing out an entire page or if we're
                // going to be writing to the first byte of the last page of the
                // file, then we can map in an empty cache page.
                //

                if (BytesToCopy == PAGE_SIZE) {
                    MapEmptyBuffer = TRUE;
                } else if ((BYTE_OFFSET(FileByteOffset) == 0) &&
                    (FileByteOffset + IoLengthRemaining >= Fcb->FileSize)) {
                    MapEmptyBuffer = TRUE;
                } else {
                    MapEmptyBuffer = FALSE;
                }

                //
                // Map in the next page of the physical run.
                //

                if (MapEmptyBuffer) {
                    status = FscMapEmptyBuffer(&VolumeExtension->CacheExtension,
                        PhysicalByteOffset, &CacheBuffer);
                } else {
                    status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
                        PhysicalByteOffset, TRUE, &CacheBuffer);
                }

                if (!NT_SUCCESS(status)) {
                    return status;
                }

                //
                // Write to the cache buffer and unmap the cache buffer.
                //

                RtlCopyMemory(CacheBuffer, UserBuffer, BytesToCopy);

                status = FscWriteBuffer(&VolumeExtension->CacheExtension, Irp,
                    PhysicalByteOffset, BytesToCopy, CacheBuffer);

                if (!NT_SUCCESS(status)) {
                    return status;
                }

            } else {

                //
                // Map in the next page of the physical run.
                //

                status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
                    PhysicalByteOffset, FALSE, &CacheBuffer);

                if (!NT_SUCCESS(status)) {
                    return status;
                }

                //
                // Read from the cache buffer and unmap the cache buffer.
                //

                RtlCopyMemory(UserBuffer, CacheBuffer, BytesToCopy);
                FscUnmapBuffer(CacheBuffer);
            }

            //
            // Adjust the number of bytes remaining and check if we're through
            // with the transfer.
            //

            PhysicalByteOffset += BytesToCopy;
            IoLengthRemaining -= BytesToCopy;

            if (IoLengthRemaining == 0) {

                if (!PartialTransfer) {
                    status = FatxSynchronousIoTail(VolumeExtension, Irp,
                        MajorFunction, FileObject, OriginalFileByteOffset,
                        IoLength);
                } else {
                    status = STATUS_SUCCESS;
                }

                return status;
            }

            //
            // There's still more bytes to transfer.  Update the other loop
            // variables and continue transfering this physical run.
            //

            FileByteOffset += BytesToCopy;
            UserBuffer = (PUCHAR)UserBuffer + BytesToCopy;

        } while (PhysicalRunBytesRemaining > 0);

        //
        // Obtain the next physical run.
        //

        status = FatxFileByteOffsetToPhysicalByteOffset(VolumeExtension, Irp,
            Fcb, FileByteOffset, TRUE, &PhysicalByteOffset, &PhysicalRunLength);

        if (!NT_SUCCESS(status)) {
            return status;
        }
    }
}

NTSTATUS
FatxPartiallyCachedSynchronousIo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN UCHAR MajorFunction,
    IN PFILE_OBJECT FileObject,
    IN ULONG FileByteOffset,
    IN ULONG IoLength,
    IN BOOLEAN NonCachedEndOfFileTransfer
    )
/*++

Routine Description:

    This routine is called to synchronously read or write a buffer partially
    through the file system cache and partially through direct device I/O.  This
    routine is intended for large I/O transfers where we want to maximize use of
    direct device I/O in order to avoid PAGE_SIZE I/O transfers.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    MajorFunction - Specifies the function to be performed; either IRP_MJ_READ
        or IRP_MJ_WRITE.

    FileObject - Specifies the file object that the I/O request is for.

    FileByteOffset - Specifies the file byte offset to start the transfer from.

    IoLength - Specifies the number of bytes to transfer.

    NonCachedEndOfFileTransfer - Specifies TRUE if the read is to the end of
        file and the output buffer is large enough to hold the transfer length
        rounded up to a sector boundary.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PFAT_VOLUME_EXTENSION VolumeExtension;
    ULONG OriginalFileByteOffset;
    ULONG OriginalIoLength;
    ULONG PartialIoLength;
    ULONG BufferByteOffset;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    OriginalFileByteOffset = FileByteOffset;
    OriginalIoLength = IoLength;

    //
    // The below code assumes that the relative byte offsets into the file and
    // the file area have the same page alignment as their corresponding
    // physical byte offsets.
    //

    ASSERT(BYTE_OFFSET(VolumeExtension->FileAreaByteOffset) == 0);

    //
    // Transfer the head of the request if it's not page aligned.
    //

    if (BYTE_OFFSET(FileByteOffset) != 0) {

        PartialIoLength = PAGE_SIZE - BYTE_OFFSET(FileByteOffset);

        ASSERT(PartialIoLength < IoLength);

        status = FatxFullyCachedSynchronousIo(DeviceObject, Irp, MajorFunction,
            FileObject, FileByteOffset, 0, PartialIoLength, TRUE);

        if (!NT_SUCCESS(status)) {
            return status;
        }

        IoLength -= PartialIoLength;
        FileByteOffset += PartialIoLength;
        BufferByteOffset = PartialIoLength;

    } else {
        BufferByteOffset = 0;
    }

    //
    // If NonCachedEndOfFileTransfer is TRUE, then we're reading to the end of
    // the file and the transfer buffer is large enough to hold the transfer
    // length rounded up to a sector boundary.
    //

    if (NonCachedEndOfFileTransfer) {
        ASSERT(MajorFunction == IRP_MJ_READ);
        IoLength = FatxRoundToSectors(VolumeExtension, IoLength);
        PartialIoLength = IoLength;
    } else {
        ASSERT(IoLength >= PAGE_SIZE);
        PartialIoLength = (ULONG)PAGE_ALIGN(IoLength);
    }

    status = FatxNonCachedSynchronousIo(DeviceObject, Irp, MajorFunction,
        FileObject, FileByteOffset, BufferByteOffset, PartialIoLength, TRUE);

    //
    // Transfer the remaining non whole page of the request if necessary.
    //

    if (NT_SUCCESS(status)) {

        IoLength -= PartialIoLength;

        if (IoLength > 0) {

            FileByteOffset += PartialIoLength;
            BufferByteOffset += PartialIoLength;

            status = FatxFullyCachedSynchronousIo(DeviceObject, Irp,
                MajorFunction, FileObject, FileByteOffset, BufferByteOffset,
                IoLength, TRUE);
        }
    }

    if (NT_SUCCESS(status)) {
        status = FatxSynchronousIoTail(VolumeExtension, Irp, MajorFunction,
            FileObject, OriginalFileByteOffset, OriginalIoLength);
    }

    return status;
}

NTSTATUS
FatxFsdReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_READ and
    IRP_MJ_WRITE requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PFAT_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    PIO_STACK_LOCATION NextIrpSp;
    UCHAR MajorFunction;
    PFILE_OBJECT FileObject;
    PFAT_FCB Fcb;
    ULONG IoLength;
    BOOLEAN NonCachedEndOfFileTransfer;
    ULONGLONG PartitionBytesRemaining;
    ULONG FileBytesRemaining;
    BOOLEAN SynchronousIo;
    ULONG EndingByteOffset;
    ULONG ClusterNumber;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    NextIrpSp = IoGetNextIrpStackLocation(Irp);
    MajorFunction = IrpSp->MajorFunction;
    FileObject = IrpSp->FileObject;
    Fcb = (PFAT_FCB)FileObject->FsContext;
    IoLength = IrpSp->Parameters.Read.Length;
    NonCachedEndOfFileTransfer = FALSE;

    //
    // Ensure that the file object is not for a directory.
    //

    if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY)) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto CompleteAndExit;
    }

    //
    // If this is a zero length request, then we can complete the IRP now.
    //

    if (IoLength == 0) {
        Irp->IoStatus.Information = 0;
        status = STATUS_SUCCESS;
        goto CompleteAndExit;
    }

    //
    // Check if we're reading from or writing to the volume file control block.
    //

    if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_VOLUME)) {

        if (MajorFunction == IRP_MJ_READ) {
            FatxAcquireVolumeMutexShared(VolumeExtension);
        } else {
            FatxAcquireVolumeMutexExclusive(VolumeExtension);
        }

        //
        // Check if the volume has been dismounted.
        //

        if (FatxIsFlagSet(VolumeExtension->Flags, FAT_VOLUME_DISMOUNTED)) {
            status = STATUS_VOLUME_DISMOUNTED;
            goto UnlockVolumeAndExit;
        }

        //
        // Check if the starting offset is beyond the end of the partition.
        //

        if ((ULONGLONG)IrpSp->Parameters.Read.ByteOffset.QuadPart >=
            (ULONGLONG)VolumeExtension->PartitionLength.QuadPart) {
            status = STATUS_END_OF_FILE;
            goto UnlockVolumeAndExit;
        }

        //
        // If the number of bytes to transfer is greater than the number of
        // bytes remaining in the partition, then truncate the number of bytes
        // we'll actually transfer.
        //

        PartitionBytesRemaining =
            (ULONGLONG)VolumeExtension->PartitionLength.QuadPart -
            (ULONGLONG)IrpSp->Parameters.Read.ByteOffset.QuadPart;

        if ((ULONGLONG)IoLength >= PartitionBytesRemaining) {
            IoLength = (ULONG)PartitionBytesRemaining;
        }

        ASSERT(IoLength > 0);

        //
        // We'll leave it to the target device's driver to validate that the
        // user's buffer, the starting byte offset, and the transfer length are
        // all properly aligned.
        //

        IoCopyCurrentIrpStackLocationToNext(Irp);

        //
        // Lock the user's buffer into memory if necessary.
        //

        IoLockUserBuffer(Irp, IrpSp->Parameters.Read.Length);

        //
        // Invalidate any file system cache buffers for this byte range if this
        // is a write operation.
        //

        if (MajorFunction == IRP_MJ_WRITE) {
            FscInvalidateByteRange(&VolumeExtension->CacheExtension,
                IrpSp->Parameters.Read.ByteOffset.QuadPart, IoLength);
        }

        //
        // Limit the number of bytes physically read to the end of the volume.
        //

        NextIrpSp->Parameters.Read.Length = IoLength;

        //
        // Set a completion routine to unlock the volume mutex and update any
        // state in the file control block.
        //

        IoSetCompletionRoutine(Irp, FatxVolumeIoCompletion, NULL, TRUE, TRUE,
            TRUE);

        //
        // Call down to the target device.
        //

        status = IoCallDriver(VolumeExtension->TargetDeviceObject, Irp);

        //
        // Leave the critical region that we acquired when we took the volume
        // mutex.
        //

        KeLeaveCriticalRegion();

        return status;

UnlockVolumeAndExit:
        FatxReleaseVolumeMutex(VolumeExtension);
        goto CompleteAndExit;
    }

    //
    // Otherwise, we're reading from or writing to a standard file.
    //

    if (MajorFunction == IRP_MJ_READ) {
        FatxAcquireFileMutexShared(Fcb);
    } else {
        FatxAcquireFileMutexExclusive(Fcb);
    }

    //
    // Increment the dismount unblock count for the volume.  We won't be
    // holding the volume's mutex throughout the processing of this request,
    // so FatxDismountVolume needs to have some synchronization mechanism to
    // know when all pending read/write IRPs have completed.
    //
    // If a request enters the file system after a dismount has been
    // unblocked or completed, then the dismount flag will have been set for
    // the volume and the code below will fail the request.
    //
    // We don't need to do this for volume file control block because in
    // that code path, the volume's mutex is held for the entire operation.
    //

    FatxIncrementDismountBlockCount(VolumeExtension);

    //
    // Check if the volume has been dismounted.
    //

    if (FatxIsFlagSet(VolumeExtension->Flags, FAT_VOLUME_DISMOUNTED)) {
        status = STATUS_VOLUME_DISMOUNTED;
        goto UnlockFileAndExit;
    }

    //
    // Check if the file object has already been cleaned up.  We don't allow a
    // a file object to be modified after its handle has been closed.
    //

    if (FatxIsFlagSet(FileObject->Flags, FO_CLEANUP_COMPLETE)) {
        status = STATUS_FILE_CLOSED;
        goto UnlockFileAndExit;
    }

    if (MajorFunction == IRP_MJ_READ) {

        //
        // Check if the starting offset is beyond the end of file.
        //

        if ((IrpSp->Parameters.Read.ByteOffset.HighPart != 0) ||
            (IrpSp->Parameters.Read.ByteOffset.LowPart >= Fcb->FileSize)) {
            status = STATUS_END_OF_FILE;
            goto UnlockFileAndExit;
        }

        //
        // If the number of bytes to read is greater than the number of bytes
        // remaining in the file, then truncate the number of bytes we'll
        // actually read.
        //

        FileBytesRemaining = Fcb->FileSize -
            IrpSp->Parameters.Read.ByteOffset.LowPart;

        if (IoLength >= FileBytesRemaining) {

            //
            // If the user's buffer is large enough to hold the logical read
            // length rounded up to a sector boundary, then set a flag so that
            // the below code will potentially read this part of the file as
            // non cached.
            //

            if (IoLength >= (FatxRoundToSectors(VolumeExtension, Fcb->FileSize) -
                IrpSp->Parameters.Read.ByteOffset.LowPart)) {
                NonCachedEndOfFileTransfer = TRUE;
            }

            IoLength = FileBytesRemaining;
        }

        ASSERT(IoLength > 0);

        //
        // Check if we should do synchronous or asynchronous I/O depending on
        // how the file object was originally opened.
        //

        SynchronousIo = (BOOLEAN)FatxIsFlagSet(FileObject->Flags,
            FO_SYNCHRONOUS_IO);

    } else {

        //
        // If we're supposed to write to the end of the file, then change the
        // starting byte offset to the current end of file.
        //

        if ((IrpSp->Parameters.Read.ByteOffset.LowPart == FILE_WRITE_TO_END_OF_FILE) &&
            (IrpSp->Parameters.Read.ByteOffset.HighPart == -1)) {
            IrpSp->Parameters.Read.ByteOffset.LowPart = Fcb->FileSize;
            IrpSp->Parameters.Read.ByteOffset.HighPart = 0;
        }

        //
        // Verify that the starting or ending offset are only 32-bits.
        //

        EndingByteOffset = IrpSp->Parameters.Read.ByteOffset.LowPart + IoLength;

        if ((IrpSp->Parameters.Read.ByteOffset.HighPart != 0) ||
            (EndingByteOffset <= IrpSp->Parameters.Read.ByteOffset.LowPart)) {
            status = STATUS_DISK_FULL;
            goto UnlockFileAndExit;
        }

        //
        // Determine the number of bytes currently allocated to the file.
        //

        if (Fcb->AllocationSize == MAXULONG) {

            //
            // Attempt to find the cluster corresponding to the maximum byte
            // offset which will have the side effect of filling in the number
            // of bytes allocated.
            //

            FatxAcquireVolumeMutexShared(VolumeExtension);

            status = FatxFileByteOffsetToCluster(VolumeExtension, Irp, Fcb,
                MAXULONG, &ClusterNumber, NULL);

            FatxReleaseVolumeMutex(VolumeExtension);

            if (!NT_SUCCESS(status) && (status != STATUS_END_OF_FILE)) {
                goto UnlockFileAndExit;
            }
        }

        //
        // If the file size is greater than the number of bytes allocated to the
        // file, then the file is corrupt and we won't allow any writes to it.
        //

        if (Fcb->FileSize > Fcb->AllocationSize) {
            status = STATUS_FILE_CORRUPT_ERROR;
            goto UnlockFileAndExit;
        }

        //
        // If the ending byte offset is greater than the number of bytes
        // allocated to the file, then we need to grow the allocation size for
        // the file.  The volume mutex must be acquired exclusively so that the
        // volume can be modified.
        //

        if (EndingByteOffset > Fcb->AllocationSize) {

            FatxAcquireVolumeMutexExclusive(VolumeExtension);

            status = FatxExtendFileAllocation(VolumeExtension, Irp, Fcb,
                FatxRoundToClusters(VolumeExtension, EndingByteOffset));

            FatxReleaseVolumeMutex(VolumeExtension);

            if (!NT_SUCCESS(status)) {
                goto UnlockFileAndExit;
            }
        }

        //
        // Check if we should do synchronous or asynchronous I/O depending on
        // how the file object was originally opened.
        //
        // If this is an asynchronous I/O operation, verify that the ending byte
        // offset is before the end of file.  If it isn't, then we can't change
        // the directory entry's file size asynchronously, so switch this back
        // to a synchronous I/O operation.
        //

        SynchronousIo = (BOOLEAN)FatxIsFlagSet(FileObject->Flags,
            FO_SYNCHRONOUS_IO);

        if (!SynchronousIo && (EndingByteOffset > Fcb->FileSize)) {
            SynchronousIo = TRUE;
        }
    }

    //
    // The upper 32-bits of the byte offset should be zero at this point.  Code
    // below this point will only look at ByteOffset.LowPart.
    //

    ASSERT(IrpSp->Parameters.Read.ByteOffset.HighPart == 0);

    //
    // Check if we're supposed to bypass the file system cache.
    //

    if (FatxIsFlagSet(Irp->Flags, IRP_NOCACHE)) {

        //
        // Verify that the starting byte offset and transfer length are sector
        // aligned.  We'll leave it to the target device's driver to validate
        // that the user's buffer is properly aligned.
        //

        if (!FatxIsSectorAligned(VolumeExtension, IrpSp->Parameters.Read.ByteOffset.LowPart) ||
            !FatxIsSectorAligned(VolumeExtension, IrpSp->Parameters.Read.Length)) {
            status = STATUS_INVALID_PARAMETER;
            goto UnlockFileAndExit;
        }

        //
        // Lock the user's buffer into memory if necessary.
        //

        IoLockUserBuffer(Irp, IrpSp->Parameters.Read.Length);

        //
        // Do the transfer depending on whether we can block waiting for the
        // I/O to complete or not.
        //

        if (SynchronousIo) {

            status = FatxNonCachedSynchronousIo(DeviceObject, Irp, MajorFunction,
                FileObject, IrpSp->Parameters.Read.ByteOffset.LowPart, 0,
                IoLength, FALSE);

        } else {

            status = FatxNonCachedAsynchronousIo(DeviceObject, Irp, MajorFunction,
                FileObject, IrpSp->Parameters.Read.ByteOffset.LowPart, IoLength);

            if (status == STATUS_PENDING) {

                //
                // Leave the critical region that we acquired when we took the
                // file mutex.
                //

                KeLeaveCriticalRegion();

                return status;
            }
        }

    } else {

        //
        // Scatter/gather operations are always non-cached.
        //

        ASSERT(FatxIsFlagClear(Irp->Flags, IRP_SCATTER_GATHER_OPERATION));

        //
        // Otherwise, handle reading the data using the file cache.
        //
        // Check if this transfer should use the fully or partially cached path.
        //

        if (FscTestForFullyCachedIo(Irp, IrpSp->Parameters.Read.ByteOffset.LowPart,
            IoLength, NonCachedEndOfFileTransfer)) {

            status = FatxFullyCachedSynchronousIo(DeviceObject, Irp,
                MajorFunction, FileObject,
                IrpSp->Parameters.Read.ByteOffset.LowPart, 0, IoLength, FALSE);

        } else {

            //
            // Lock the user's buffer into memory if necessary.
            //

            IoLockUserBuffer(Irp, IrpSp->Parameters.Read.Length);

            status = FatxPartiallyCachedSynchronousIo(DeviceObject, Irp,
                MajorFunction, FileObject,
                IrpSp->Parameters.Read.ByteOffset.LowPart, IoLength,
                NonCachedEndOfFileTransfer);
        }
    }

    ASSERT(status != STATUS_PENDING);

UnlockFileAndExit:
    FatxDecrementDismountBlockCount(VolumeExtension);
    FatxReleaseFileMutex(Fcb);

CompleteAndExit:
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
