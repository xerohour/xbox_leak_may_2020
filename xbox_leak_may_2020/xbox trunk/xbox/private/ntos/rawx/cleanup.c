/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    cleanup.c

Abstract:

    This module implements routines related to handling IRP_MJ_CLEANUP.

--*/

#include "rawx.h"

NTSTATUS
RawxFsdCleanup(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_CLEANUP requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    PRAW_VOLUME_EXTENSION VolumeExtension;
    PFILE_OBJECT FileObject;

    VolumeExtension = (PRAW_VOLUME_EXTENSION)DeviceObject->DeviceExtension;

    RawxAcquireVolumeMutexExclusive(VolumeExtension);

    //
    // Cleanup the sharing mode associated with the file object.
    //

    FileObject = IoGetCurrentIrpStackLocation(Irp)->FileObject;
    IoRemoveShareAccess(FileObject, &VolumeExtension->ShareAccess);

    RawxReleaseVolumeMutex(VolumeExtension);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}
