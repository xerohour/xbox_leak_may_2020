/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    flush.c

Abstract:

    This module implements routines related to handling IRP_MJ_FLUSH_BUFFERS.

--*/

#include "fatx.h"

NTSTATUS
FatxFsdFlushBuffers(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_FLUSH_BUFFERS
    requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PFAT_VOLUME_EXTENSION VolumeExtension;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;

    FatxAcquireVolumeMutexShared(VolumeExtension);

    //
    // Check if the volume has been dismounted.
    //

    if (FatxIsFlagSet(VolumeExtension->Flags, FAT_VOLUME_DISMOUNTED)) {

        FatxReleaseVolumeMutex(VolumeExtension);

        Irp->IoStatus.Status = STATUS_VOLUME_DISMOUNTED;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_VOLUME_DISMOUNTED;
    }

    //
    // Initialize the next IRP stack location.
    //

    IoCopyCurrentIrpStackLocationToNext(Irp);

    //
    // Set a completion routine to unlock the volume mutex.
    //

    IoSetCompletionRoutine(Irp, FatxVolumeIoCompletion, NULL, TRUE, TRUE, TRUE);

    //
    // Call down to the target device.
    //

    status = IoCallDriver(VolumeExtension->TargetDeviceObject, Irp);

    //
    // Leave the critical region that we acquired when we took the volume mutex.
    //

    KeLeaveCriticalRegion();

    return status;
}
