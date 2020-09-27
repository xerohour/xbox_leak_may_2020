/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    close.c

Abstract:

    This module implements routines related to handling IRP_MJ_CLOSE.

--*/

#include "rawx.h"

NTSTATUS
RawxFsdClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_CLOSE requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    PRAW_VOLUME_EXTENSION VolumeExtension;

    VolumeExtension = (PRAW_VOLUME_EXTENSION)DeviceObject->DeviceExtension;

    RawxAcquireVolumeMutexExclusive(VolumeExtension);

    //
    // Check if we're ready to delete this device object.
    //

    if ((VolumeExtension->ShareAccess.OpenCount == 0) &&
        VolumeExtension->Dismounted) {
        RawxReleaseVolumeMutex(VolumeExtension);
        RawxDeleteVolumeDevice(DeviceObject);
    } else {
        RawxReleaseVolumeMutex(VolumeExtension);
    }

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}
