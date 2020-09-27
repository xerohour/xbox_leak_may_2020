/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    driver.c

Abstract:

    This module implements routines that apply to the driver object including
    initialization and IRP dispatch routines.

--*/

#include "rawx.h"

//
// Driver object for the RAW file system.
//
DECLSPEC_RDATA DRIVER_OBJECT RawxDriverObject = {
    NULL,                               // DriverStartIo
    NULL,                               // DriverDeleteDevice
    RawxDismountVolume,                 // DriverDismountVolume
    {
        RawxFsdCreate,                  // IRP_MJ_CREATE
        RawxFsdClose,                   // IRP_MJ_CLOSE
        RawxPassIrpDownToTargetDevice,  // IRP_MJ_READ
        RawxPassIrpDownToTargetDevice,  // IRP_MJ_WRITE
        RawxFsdQueryInformation,        // IRP_MJ_QUERY_INFORMATION
        RawxFsdSetInformation,          // IRP_MJ_SET_INFORMATION
        RawxPassIrpDownToTargetDevice,  // IRP_MJ_FLUSH_BUFFERS
        RawxFsdQueryVolumeInformation,  // IRP_MJ_QUERY_VOLUME_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_DIRECTORY_CONTROL
        RawxFsdFileSystemControl,       // IRP_MJ_FILE_SYSTEM_CONTROL
        RawxPassIrpDownToTargetDevice,  // IRP_MJ_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_INTERNAL_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_SHUTDOWN
        RawxFsdCleanup,                 // IRP_MJ_CLEANUP
    }
};

NTSTATUS
RawxVolumeIoCompletion(
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
    PRAW_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    PFILE_OBJECT FileObject;

    VolumeExtension = (PRAW_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
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

            //
            // If the file is open for synchronous I/O, then we need to update
            // the current file position.
            //

            FileObject = IrpSp->FileObject;

            if (RawxIsFlagSet(FileObject->Flags, FO_SYNCHRONOUS_IO)) {
                FileObject->CurrentByteOffset.QuadPart =
                    IrpSp->Parameters.Read.ByteOffset.QuadPart +
                    Irp->IoStatus.Information;
            }
        }
    }

    RawxDpcReleaseVolumeMutex(VolumeExtension);

    return STATUS_SUCCESS;
}

NTSTATUS
RawxPassIrpDownToTargetDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine passes the current request down to the target device.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PRAW_VOLUME_EXTENSION VolumeExtension;

    VolumeExtension = (PRAW_VOLUME_EXTENSION)DeviceObject->DeviceExtension;

    RawxAcquireVolumeMutexShared(VolumeExtension);

    //
    // Check if the volume has been dismounted.
    //

    if (VolumeExtension->Dismounted) {

        RawxReleaseVolumeMutex(VolumeExtension);

        Irp->IoStatus.Status = STATUS_VOLUME_DISMOUNTED;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_VOLUME_DISMOUNTED;
    }

    //
    // Initialize the next IRP stack location.
    //

    IoCopyCurrentIrpStackLocationToNext(Irp);

    //
    // Set a completion routine to unlock the volume mutex and update any state
    // in the file control block.
    //

    IoSetCompletionRoutine(Irp, RawxVolumeIoCompletion, NULL, TRUE, TRUE, TRUE);

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
