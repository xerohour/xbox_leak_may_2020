/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    create.c

Abstract:

    This module implements routines related to handling IRP_MJ_CREATE.

--*/

#include "rawx.h"

NTSTATUS
RawxFsdCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_CREATE requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PRAW_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    ACCESS_MASK DesiredAccess;
    USHORT ShareAccess;
    ULONG CreateOptions;
    PFILE_OBJECT FileObject;
    ULONG CreateDisposition;

    VolumeExtension = (PRAW_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    DesiredAccess = IrpSp->Parameters.Create.DesiredAccess;
    ShareAccess = IrpSp->Parameters.Create.ShareAccess;
    CreateOptions = IrpSp->Parameters.Create.Options;
    FileObject = IrpSp->FileObject;

    RawxAcquireVolumeMutexExclusive(VolumeExtension);

    //
    // Check if the volume has been dismounted.
    //

    if (VolumeExtension->Dismounted) {
        status = STATUS_VOLUME_DISMOUNTED;
        goto CleanupAndExit;
    }

    //
    // Don't allow a relative path to be opened.
    //

    if (IrpSp->FileObject->RelatedFileObject != NULL) {
        status = STATUS_INVALID_PARAMETER;
        goto CleanupAndExit;
    } else if (IrpSp->Parameters.Create.RemainingName->Length != 0) {
        status = STATUS_OBJECT_PATH_NOT_FOUND;
        goto CleanupAndExit;
    }

    //
    // Verify that the create disposition is for open access only.
    //

    CreateDisposition = (CreateOptions >> 24) & 0xFF;

    if ((CreateDisposition != FILE_OPEN) && (CreateDisposition != FILE_OPEN_IF)) {
        status = STATUS_ACCESS_DENIED;
        goto CleanupAndExit;
    }

    //
    // The caller shouldn't be expecting to see a directory file.
    //

    if (RawxIsFlagSet(CreateOptions, FILE_DIRECTORY_FILE)) {
        status = STATUS_NOT_A_DIRECTORY;
        goto CleanupAndExit;
    }

    //
    // If this is the first open handle to the file, then initialize the sharing
    // mode.  Otherwise, verify that the requested sharing mode is compatible
    // with the current sharing mode.
    //

    if (VolumeExtension->ShareAccess.OpenCount == 0) {
        IoSetShareAccess(DesiredAccess, ShareAccess, FileObject,
            &VolumeExtension->ShareAccess);
        status = STATUS_SUCCESS;
    } else {
        status = IoCheckShareAccess(DesiredAccess, ShareAccess, FileObject,
            &VolumeExtension->ShareAccess, TRUE);
    }

    if (NT_SUCCESS(status)) {

        //
        // Physical volume access is always non-cached.  Mark the file object as
        // non-cached so that the I/O manager enforces alignment requirements.
        //

        FileObject->Flags |= FO_NO_INTERMEDIATE_BUFFERING;

        //
        // Indicate to the caller that we opened the file as opposed to creating
        // or overwriting the file.
        //

        Irp->IoStatus.Information = FILE_OPENED;

        status = STATUS_SUCCESS;
    }

CleanupAndExit:
    RawxReleaseVolumeMutex(VolumeExtension);

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}
