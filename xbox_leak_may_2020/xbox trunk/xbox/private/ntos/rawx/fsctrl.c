/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fsctrl.c

Abstract:

    This module implements routines related to handling
    IRP_MJ_FILE_SYSTEM_CONTROL.

--*/

#include "rawx.h"

NTSTATUS
RawxDismountVolume(
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
    PRAW_VOLUME_EXTENSION VolumeExtension;
    KIRQL OldIrql;
    ULONG FileObjectCount;

    VolumeExtension = (PRAW_VOLUME_EXTENSION)DeviceObject->DeviceExtension;

    RawxAcquireVolumeMutexExclusive(VolumeExtension);

    //
    // Check if the volume is already marked for dismount.  If not, mark it for
    // dismount.
    //

    if (VolumeExtension->Dismounted) {
        RawxReleaseVolumeMutex(VolumeExtension);
        return STATUS_VOLUME_DISMOUNTED;
    }

    VolumeExtension->Dismounted = TRUE;

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

    FileObjectCount = VolumeExtension->ShareAccess.OpenCount;

    RawxReleaseVolumeMutex(VolumeExtension);

    if (FileObjectCount == 0) {
        RawxDeleteVolumeDevice(DeviceObject);
    } else if (FileObjectCount > 2) {
        RawxDbgPrint(("RAWX: dismounting volume %p with %d open file handles\n",
            VolumeExtension, FileObjectCount));
    }

    return STATUS_SUCCESS;
}

NTSTATUS
RawxFsdFileSystemControl(
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
            //
            // Note that we call indirect through our driver object so that the
            // generated code ends up being identical between GDFX/RAWX/UDFX.
            //

            status = DeviceObject->DriverObject->DriverDismountVolume(DeviceObject);
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
