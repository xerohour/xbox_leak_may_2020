/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fsctrl.c

Abstract:

    This module implements routines related to handling
    IRP_MJ_FILE_SYSTEM_CONTROL.

--*/

#include "fatx.h"

VOID
FatxSignalDismountUnblockEvent(
    IN PFAT_VOLUME_EXTENSION VolumeExtension
    )
/*++

Routine Description:

    This routine signals the volume's dismount unblock event when the volume's
    dismount block count is negative.

Arguments:

    VolumeExtension - Specifies the volume to signal the dismount unblock event
        for.

Return Value:

    None.

--*/
{
    KIRQL OldIrql;

    OldIrql = KeRaiseIrqlToDpcLevel();

    if (VolumeExtension->DismountUnblockEvent != NULL) {
        KeSetEvent(VolumeExtension->DismountUnblockEvent, 0, FALSE);
    }

    KeLowerIrql(OldIrql);
}

NTSTATUS
FatxDismountVolume(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    This routine is indirectly called by the I/O manager to handle
    FSCTL_DISMOUNT_VOLUME requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

Return Value:

    Status of operation.

--*/
{
    PFAT_VOLUME_EXTENSION VolumeExtension;
    KEVENT DismountUnblockEvent;
    KIRQL OldIrql;
    ULONG FileObjectCount;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;

    FatxAcquireVolumeMutexExclusive(VolumeExtension);

    //
    // Check if the volume is already marked for dismount.  If not, mark it for
    // dismount.
    //

    if (FatxIsFlagSet(VolumeExtension->Flags, FAT_VOLUME_DISMOUNTED)) {
        FatxReleaseVolumeMutex(VolumeExtension);
        return STATUS_VOLUME_DISMOUNTED;
    }

    VolumeExtension->Flags |= FAT_VOLUME_DISMOUNTED;

    //
    // Synchronize the dismount operation with any read and write operations
    // already in progress.  When all read and write operations have completed,
    // the DismountBlockCount will be zero.  If the DismountBlockCount is not
    // zero at this point, then we need to block.  Whenever a read or write
    // operation completes, the DismountBlockCount is decremented and if the
    // count is negative, then that indicates that this routine is blocked
    // waiting for the operation to complete.
    //
    // A read or write operation can complete from a DPC, so we need to
    // synchronize access to the DismountBlockCount and DismountUnblockEvent at
    // DISPATCH_LEVEL.
    //

    KeInitializeEvent(&DismountUnblockEvent, SynchronizationEvent, FALSE);

    OldIrql = KeRaiseIrqlToDpcLevel();

    ASSERT(VolumeExtension->DismountUnblockEvent == NULL);
    VolumeExtension->DismountUnblockEvent = &DismountUnblockEvent;

    if (VolumeExtension->DismountBlockCount > 0) {

        //
        // Decrement the dismount count so that the lowest possible state is
        // negative one, which FatxDecrementDismountBlockCount uses as the
        // trigger to know that we need to be signaled.
        //

        VolumeExtension->DismountBlockCount--;

        //
        // Wait for the unblock event to be signaled outside of the raised IRQL
        // and ownership of the volume's mutex.  We don't have to worry about
        // another thread attempting to dismount this volume because the above
        // code has already marked the volume as dismounted, so another dismount
        // request will fail.
        //

        KeLowerIrql(OldIrql);
        FatxReleaseVolumeMutex(VolumeExtension);

        KeWaitForSingleObject(&DismountUnblockEvent, Executive, KernelMode,
            FALSE, NULL);

        FatxAcquireVolumeMutexExclusive(VolumeExtension);
        OldIrql = KeRaiseIrqlToDpcLevel();

        //
        // Boost the lowest possible state back up to zero so that read and
        // write operations to the dismounted volume don't keep calling
        // FatxSignalDismountUnblockEvent.
        //

        VolumeExtension->DismountBlockCount++;
    }

    ASSERT(VolumeExtension->DismountUnblockEvent == &DismountUnblockEvent);
    VolumeExtension->DismountUnblockEvent = NULL;

    KeLowerIrql(OldIrql);

    //
    // Invalidate any file system cache buffers for this device.
    // FatxDeleteVolumeDevice will also invalidate the cache, but we might as
    // well release as many cache pages as we can now.
    //

    FscInvalidateDevice(&VolumeExtension->CacheExtension);

    //
    // Synchronize access to the MountedOrSelfDevice with the I/O manager by
    // raising to DISPATCH_LEVEL.
    //
    // Clear out the target device object's MountedOrSelfDevice field.  That
    // will cause future accesses to the target device object to mount a new
    // file system device object.
    //

    OldIrql = KeRaiseIrqlToDpcLevel();

    VolumeExtension->TargetDeviceObject->MountedOrSelfDevice = NULL;

    KeLowerIrql(OldIrql);

    //
    // Release the reference on the target device object.
    //

    ObDereferenceObject(VolumeExtension->TargetDeviceObject);
    VolumeExtension->TargetDeviceObject = NULL;

    //
    // Take a snapshot of the file object count, release the volume mutex, and
    // delete the volume device if the file object count is zero.  This won't
    // normally happen since there's probably an open handle in order to make
    // this dismount call, but it could happen if an IRP was submitted directly
    // to the device object.
    //
    // We'll print out a debug message if the file object count is greater than
    // two.  XUnmountMU dismounts a volume with two file objects outstanding, so
    // anything beyond that represents some user file that hasn't been closed.
    //

    FileObjectCount = VolumeExtension->FileObjectCount;

    FatxReleaseVolumeMutex(VolumeExtension);

    if (FileObjectCount == 0) {
        FatxDeleteVolumeDevice(DeviceObject);
    } else if (FileObjectCount > 2) {
        FatxDbgPrint(("FATX: dismounting volume %p with %d open file handles\n",
            VolumeExtension, FileObjectCount));
    }

    return STATUS_SUCCESS;
}

NTSTATUS
FatxReadWriteVolumeMetadata(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is indirectly called by the I/O manager to handle
    FSCTL_READ_VOLUME_METADATA and FSCTL_WRITE_VOLUME_METADATA requests.

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
    PFSCTL_VOLUME_METADATA FsctlVolumeMetadata;
    BOOLEAN WritingMetadata;
    PVOID CacheBuffer;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    FatxAcquireVolumeMutexExclusive(VolumeExtension);

    //
    // Verify that the input buffer is large enough.
    //

    if (IrpSp->Parameters.FileSystemControl.InputBufferLength <
        sizeof(FSCTL_VOLUME_METADATA)) {
        status = STATUS_INVALID_PARAMETER;
        goto CleanupAndExit;
    }

    //
    // Verify that the starting byte offset and transfer length are valid.  The
    // volume metadata sector is at least 4096 bytes in length.
    //

    FsctlVolumeMetadata =
        (PFSCTL_VOLUME_METADATA)IrpSp->Parameters.FileSystemControl.InputBuffer;

    if ((FsctlVolumeMetadata->ByteOffset >= PAGE_SIZE) ||
        (FsctlVolumeMetadata->TransferLength > PAGE_SIZE) ||
        ((FsctlVolumeMetadata->ByteOffset +
            FsctlVolumeMetadata->TransferLength) > PAGE_SIZE)) {
        status = STATUS_INVALID_PARAMETER;
        goto CleanupAndExit;
    }

    //
    // Map the volume metadata block into the file system cache.
    //

    WritingMetadata = (BOOLEAN)(IrpSp->Parameters.FileSystemControl.FsControlCode ==
        FSCTL_WRITE_VOLUME_METADATA);

    status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp, 0,
        WritingMetadata, &CacheBuffer);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    if (WritingMetadata) {

        //
        // Copy from the transfer buffer to the volume metadata block and write
        // out the changes.
        //

        RtlCopyMemory((PUCHAR)CacheBuffer + FsctlVolumeMetadata->ByteOffset,
            FsctlVolumeMetadata->TransferBuffer, FsctlVolumeMetadata->TransferLength);

        status = FscWriteBuffer(&VolumeExtension->CacheExtension, Irp, 0,
            PAGE_SIZE, CacheBuffer);

    } else {

        //
        // Copy from the volume metadata block to the transfer buffer.
        //

        RtlCopyMemory(FsctlVolumeMetadata->TransferBuffer, (PUCHAR)CacheBuffer +
            FsctlVolumeMetadata->ByteOffset, FsctlVolumeMetadata->TransferLength);

        FscUnmapBuffer(CacheBuffer);

        status = STATUS_SUCCESS;
    }

CleanupAndExit:
    FatxReleaseVolumeMutex(VolumeExtension);

    return status;
}

NTSTATUS
FatxFsdFileSystemControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle
    IRP_MJ_FILE_SYSTEM_CONTROL requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PIO_STACK_LOCATION IrpSp;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    switch (IrpSp->Parameters.FileSystemControl.FsControlCode) {

        case FSCTL_DISMOUNT_VOLUME:
            status = FatxDismountVolume(DeviceObject);
            break;

        case FSCTL_READ_VOLUME_METADATA:
        case FSCTL_WRITE_VOLUME_METADATA:
            status = FatxReadWriteVolumeMetadata(DeviceObject, Irp);
            break;

        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
