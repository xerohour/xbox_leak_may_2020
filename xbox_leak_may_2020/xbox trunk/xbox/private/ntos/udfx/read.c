/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    read.c

Abstract:

    This module implements routines related to handling IRP_MJ_READ.

--*/

#include "udfx.h"

NTSTATUS
UdfxNonCachedReadCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:

    This routine is called to process the completion of a non-cached file read.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    Context - Specifies the context that was supplied to IoSetCompletionRoutine.

Return Value:

    Status of operation.

--*/
{
    PIO_STACK_LOCATION IrpSp;
    PFILE_OBJECT FileObject;
    ULONG ReadLength;

    //
    // This completion routine is set to be called only on IRP success.
    //

    ASSERT(NT_SUCCESS(Irp->IoStatus.Status));

    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    FileObject = IrpSp->FileObject;
    ReadLength = IrpSp->Parameters.Read.Length;

    //
    // Assert that the device handled as many bytes as we programmed it to.
    //

    ASSERT(Irp->IoStatus.Information == UDF_CD_SECTOR_ALIGN_UP(ReadLength));

    //
    // Propagate the pending flag up the IRP stack.
    //

    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }

    //
    // Fix the number of bytes read to the number that we computed inside
    // UdfxFsdRead.  This number may be less than the actual number of bytes
    // read from the device if we're at the end of file.
    //

    Irp->IoStatus.Information = ReadLength;

    //
    // If the file is open for synchronous I/O, then we need to update the
    // current file position.
    //

    if (UdfxIsFlagSet(FileObject->Flags, FO_SYNCHRONOUS_IO)) {
        FileObject->CurrentByteOffset.QuadPart =
            IrpSp->Parameters.Read.ByteOffset.QuadPart + ReadLength;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
UdfxFsdRead(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_READ requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PUDF_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    PIO_STACK_LOCATION NextIrpSp;
    PFILE_OBJECT FileObject;
    PUDF_FCB Fcb;
    ULONG ReadLength;
    BOOLEAN NonCachedEndOfFileTransfer;
    ULONGLONG FileBytesRemaining;
    ULONGLONG PhysicalByteOffset;

    VolumeExtension = (PUDF_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    NextIrpSp = IoGetNextIrpStackLocation(Irp);
    FileObject = IrpSp->FileObject;
    Fcb = (PUDF_FCB)FileObject->FsContext;
    ReadLength = IrpSp->Parameters.Read.Length;
    NonCachedEndOfFileTransfer = FALSE;

    //
    // Check if the volume has been dismounted.
    //

    if (VolumeExtension->Dismounted) {
        status = STATUS_VOLUME_DISMOUNTED;
        goto CleanupAndExit;
    }

    //
    // Ensure that the file object is not for a directory.
    //

    if (UdfxIsFlagSet(Fcb->Flags, UDF_FCB_DIRECTORY)) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto CleanupAndExit;
    }

    //
    // Disable support for reading from files with data embedded in the file
    // identifier descriptor.
    //

    if (UdfxIsFlagSet(Fcb->Flags, UDF_FCB_EMBEDDED_DATA)) {
        status = STATUS_ACCESS_DENIED;
        goto CleanupAndExit;
    }

    //
    // If this is a zero length request, then we can complete the IRP now.
    //

    if (ReadLength == 0) {
        Irp->IoStatus.Information = 0;
        status = STATUS_SUCCESS;
        goto CleanupAndExit;
    }

    //
    // Check if the starting offset is beyond the end of file.
    //

    if ((ULONGLONG)IrpSp->Parameters.Read.ByteOffset.QuadPart >=
        (ULONGLONG)Fcb->FileSize.QuadPart) {
        status = STATUS_END_OF_FILE;
        goto CleanupAndExit;
    }

    //
    // If the number of bytes to read is greater than the number of bytes
    // remaining in the file, then truncate the number of bytes we'll actually
    // read.
    //

    FileBytesRemaining = (ULONGLONG)Fcb->FileSize.QuadPart -
        (ULONGLONG)IrpSp->Parameters.Read.ByteOffset.QuadPart;

    if ((ULONGLONG)ReadLength >= FileBytesRemaining) {

        if (UdfxIsFlagClear(Fcb->Flags, UDF_FCB_VOLUME)) {

            //
            // If the user's buffer is large enough to hold the logical read
            // length rounded up to a sector boundary, then set a flag so that
            // the below code will potentially read this part of the file as
            // non cached.
            //

            if (ReadLength >= (UDF_CD_SECTOR_ALIGN_UP(Fcb->FileSize.LowPart) -
                IrpSp->Parameters.Read.ByteOffset.LowPart)) {
                NonCachedEndOfFileTransfer = TRUE;
            }
        }

        ReadLength = (ULONG)FileBytesRemaining;
    }

    ASSERT(ReadLength > 0);

    //
    // Check if we're supposed to bypass the file system cache.
    //
    // If this is a physical volume file control block, also bypass the cache.
    // This was the behavior of the old file systems and it also let's us avoid
    // having to deal with the volume case in the cached path.
    //

    if (UdfxIsFlagSet(Irp->Flags, IRP_NOCACHE) ||
        UdfxIsFlagSet(Fcb->Flags, UDF_FCB_VOLUME)) {

        IoCopyCurrentIrpStackLocationToNext(Irp);

        //
        // Lock the user's buffer into memory if necessary.
        //

        IoLockUserBuffer(Irp, IrpSp->Parameters.Read.Length);

        //
        // The file is contiguous on disk, so we can pass this IRP down to the
        // target device after adjusting the starting byte offset assuming this
        // isn't a volume file control block.
        //

        if (UdfxIsFlagClear(Fcb->Flags, UDF_FCB_VOLUME)) {
            NextIrpSp->Parameters.Read.ByteOffset.QuadPart +=
                ((ULONGLONG)(VolumeExtension->PartitionSectorStart +
                Fcb->AllocationSectorStart) << UDF_CD_SECTOR_SHIFT);
        }

        //
        // Limit the number of bytes physically read to the end of the file or
        // volume.
        //

        NextIrpSp->Parameters.Read.Length = UDF_CD_SECTOR_ALIGN_UP(ReadLength);

        //
        // If the actual number of bytes we're to read is less than the number
        // of bytes we'll read from the physical device, then set a completion
        // routine to fix IO_STATUS_BLOCK.Information to be the actual number
        // of bytes.  We'll store the actual number of bytes back in the IRP so
        // that we don't have to calculate it again.
        //
        // If this is a synchronous I/O operation, then set a completion routine
        // so that we can update the current file position.
        //
        // We don't have to do either of the above if the IRP completes with an
        // error.
        //

        if ((IrpSp->Parameters.Read.Length != ReadLength) ||
            UdfxIsFlagSet(FileObject->Flags, FO_SYNCHRONOUS_IO)) {

            IrpSp->Parameters.Read.Length = ReadLength;

            IoSetCompletionRoutine(Irp, UdfxNonCachedReadCompletion, NULL, TRUE,
                FALSE, FALSE);
        }

        //
        // Call down to the target device.
        //

        return IoCallDriver(VolumeExtension->TargetDeviceObject, Irp);
    }

    //
    // Scatter/gather operations are always non-cached.
    //

    ASSERT(UdfxIsFlagClear(Irp->Flags, IRP_SCATTER_GATHER_OPERATION));

    //
    // Otherwise, handle reading the data using the file cache.
    //

    PhysicalByteOffset = IrpSp->Parameters.Read.ByteOffset.QuadPart +
        ((ULONGLONG)(VolumeExtension->PartitionSectorStart +
        Fcb->AllocationSectorStart) << UDF_CD_SECTOR_SHIFT);

    status = FscCachedRead(&VolumeExtension->CacheExtension, Irp,
        PhysicalByteOffset, ReadLength, NonCachedEndOfFileTransfer);

CleanupAndExit:
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
