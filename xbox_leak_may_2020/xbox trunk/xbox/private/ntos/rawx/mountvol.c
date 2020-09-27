/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    mountvol.c

Abstract:

    This module implements routines related to the mounting of a RAW volume.

--*/

#include "rawx.h"

VOID
RawxDeleteVolumeDevice(
    IN PDEVICE_OBJECT VolumeDeviceObject
    )
/*++

Routine Description:

    This routine deletes an instance of a volume device object.

Arguments:

    VolumeDeviceObject - Specifies the volume device object to delete.

Return Value:

    Status of operation.

--*/
{
    PRAW_VOLUME_EXTENSION VolumeExtension;

    VolumeExtension = (PRAW_VOLUME_EXTENSION)VolumeDeviceObject->DeviceExtension;

    //
    // There shouldn't be any open files for this volume.
    //

    ASSERT(VolumeExtension->ShareAccess.OpenCount == 0);

    //
    // Release the reference on the target device object.
    //

    if (VolumeExtension->TargetDeviceObject != NULL) {
        ObDereferenceObject(VolumeExtension->TargetDeviceObject);
    }

    //
    // Delete the device object.
    //

    IoDeleteDevice(VolumeDeviceObject);
}

NTSTATUS
RawxMountVolume(
    IN PDEVICE_OBJECT TargetDeviceObject
    )
/*++

Routine Description:

    This routine is called by the I/O manager to attempt to mount this file
    system.

Arguments:

    TargetDeviceObject - Specifies the device object to attempt to mount.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONG IoControlCode;
    DISK_GEOMETRY DiskGeometry;
    PDEVICE_OBJECT VolumeDeviceObject;
    PRAW_VOLUME_EXTENSION VolumeExtension;

    VolumeDeviceObject = NULL;

    RawxAcquireGlobalMutexExclusive();

    //
    // Get the drive geometry from the device.  We don't care about the result,
    // but this gives the device an opportunity to reset any state, such as the
    // number of accessible bytes.
    //

    IoControlCode = (TargetDeviceObject->DeviceType == FILE_DEVICE_CD_ROM) ?
        IOCTL_CDROM_GET_DRIVE_GEOMETRY : IOCTL_DISK_GET_DRIVE_GEOMETRY;

    IoSynchronousDeviceIoControlRequest(IoControlCode, TargetDeviceObject, NULL,
        0, &DiskGeometry, sizeof(DISK_GEOMETRY), NULL, FALSE);

    //
    // Create the volume's device object.
    //

    status = IoCreateDevice(&RawxDriverObject, sizeof(RAW_VOLUME_EXTENSION),
        NULL, FILE_DEVICE_DISK_FILE_SYSTEM, FALSE, &VolumeDeviceObject);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Increase the volume's IRP stack size for requests that we pass down to
    // the target device object.
    //

    VolumeDeviceObject->StackSize = (UCHAR)(VolumeDeviceObject->StackSize +
        TargetDeviceObject->StackSize);

    //
    // Ensure that the alignment for this device matches the requirements of the
    // target device.
    //

    if (VolumeDeviceObject->AlignmentRequirement < TargetDeviceObject->AlignmentRequirement) {
        VolumeDeviceObject->AlignmentRequirement = TargetDeviceObject->AlignmentRequirement;
    }

    //
    // If the target device object supports direct I/O, then indicate that we
    // can as well.
    //

    if (RawxIsFlagSet(TargetDeviceObject->Flags, DO_DIRECT_IO)) {
        VolumeDeviceObject->Flags |= DO_DIRECT_IO;
    }

    //
    // If the target device object supports scatter gather I/O, then indicate
    // that we can as well.
    //

    if (RawxIsFlagSet(TargetDeviceObject->Flags, DO_SCATTER_GATHER_IO)) {
        VolumeDeviceObject->Flags |= DO_SCATTER_GATHER_IO;
    }

    //
    // Set the size of a sector for this device.
    //

    VolumeDeviceObject->SectorSize = TargetDeviceObject->SectorSize;

    //
    // Increment the reference count on the target device object since we'll be
    // holding a pointer to it.
    //

    ObReferenceObject(TargetDeviceObject);

    //
    // Initialize the volume's device extension data.
    //

    VolumeExtension = (PRAW_VOLUME_EXTENSION)VolumeDeviceObject->DeviceExtension;

    VolumeExtension->TargetDeviceObject = TargetDeviceObject;

    //
    // Initialize the volume's instance lock.
    //

    ExInitializeReadWriteLock(&VolumeExtension->VolumeMutex);

    //
    // The device has finished initializing and is ready to accept requests.
    //

    VolumeDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    //
    // Connect the volume device object to the target device object and return
    // success.
    //

    TargetDeviceObject->MountedOrSelfDevice = VolumeDeviceObject;

    status = STATUS_SUCCESS;

CleanupAndExit:
    RawxReleaseGlobalMutex();

    return status;
}
