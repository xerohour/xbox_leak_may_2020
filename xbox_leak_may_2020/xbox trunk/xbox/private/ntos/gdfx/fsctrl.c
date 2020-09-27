/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fsctrl.c

Abstract:

    This module implements routines related to handling
    IRP_MJ_FILE_SYSTEM_CONTROL.

--*/

#include "gdfx.h"

NTSTATUS
GdfxDismountVolume(
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
    NTSTATUS status;
    PGDF_VOLUME_EXTENSION VolumeExtension;
    KIRQL OldIrql;

    GdfxAcquireGlobalMutexExclusive();

    VolumeExtension = (PGDF_VOLUME_EXTENSION)DeviceObject->DeviceExtension;

    //
    // Check if the volume is already marked for dismount.  If not, mark it for
    // dismount.
    //

    if (VolumeExtension->Dismounted) {
        status = STATUS_VOLUME_DISMOUNTED;
        goto CleanupAndExit;
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
    // Delete the volume device if the file object count is zero.
    //

    if (VolumeExtension->FileObjectCount == 0) {
        GdfxDeleteVolumeDevice(DeviceObject);
    } else if (VolumeExtension->FileObjectCount > 1) {
        GdfxDbgPrint(("GDFX: dismounting volume %p with %d open file handles\n",
            VolumeExtension, VolumeExtension->FileObjectCount));
    }

    status = STATUS_SUCCESS;

CleanupAndExit:
    GdfxReleaseGlobalMutex();

    return status;
}

NTSTATUS
GdfxFsdFileSystemControl(
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
