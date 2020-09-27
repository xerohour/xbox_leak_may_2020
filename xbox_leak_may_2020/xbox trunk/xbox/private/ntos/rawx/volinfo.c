/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    volinfo.c

Abstract:

    This module implements routines related to handling
    IRP_MJ_QUERY_VOLUME_INFORMATION.

--*/

#include "rawx.h"

NTSTATUS
RawxQueryVolumeSizeInformation(
    IN PRAW_VOLUME_EXTENSION VolumeExtension,
    OUT PFILE_FS_SIZE_INFORMATION SizeInformation
    )
/*++

Routine Description:

    This routine fills the information structure with the volume size
    information for the supplied volume extension.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    SizeInformation - Specifies the buffer to receive the volume size
        information.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    DISK_GEOMETRY DiskGeometry;
    PARTITION_INFORMATION PartitionInformation;

    //
    // Get the drive geometry from the device.  Note that this doesn't work for
    // non-disk devices, but that's the way the old RAW file system worked as
    // well.  If geometry information is required for non-disk devices, then
    // use IOCTLs to talk to the target device.
    //

    status = IoSynchronousDeviceIoControlRequest(IOCTL_DISK_GET_DRIVE_GEOMETRY,
        VolumeExtension->TargetDeviceObject, NULL, 0, &DiskGeometry,
        sizeof(DISK_GEOMETRY), NULL, FALSE);

    if (NT_SUCCESS(status)) {

        //
        // Get the partition information from the device.
        //

        status = IoSynchronousDeviceIoControlRequest(IOCTL_DISK_GET_PARTITION_INFO,
            VolumeExtension->TargetDeviceObject, NULL, 0,
            &PartitionInformation, sizeof(PARTITION_INFORMATION), NULL, FALSE);

        if (NT_SUCCESS(status)) {

            SizeInformation->TotalAllocationUnits.QuadPart =
                PartitionInformation.PartitionLength.QuadPart /
                DiskGeometry.BytesPerSector;
            SizeInformation->AvailableAllocationUnits =
                SizeInformation->TotalAllocationUnits;
            SizeInformation->BytesPerSector = DiskGeometry.BytesPerSector;
            SizeInformation->SectorsPerAllocationUnit = 1;
        }
    }

    return status;
}

NTSTATUS
RawxFsdQueryVolumeInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle
    IRP_MJ_QUERY_VOLUME_INFORMATION requests.

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
    ULONG BytesWritten;
    PFILE_FS_DEVICE_INFORMATION DeviceInformation;
    PFILE_FS_ATTRIBUTE_INFORMATION AttributeInformation;

    VolumeExtension = (PRAW_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    RawxAcquireVolumeMutexShared(VolumeExtension);

    //
    // Check if the volume has been dismounted.
    //

    if (VolumeExtension->Dismounted) {
        status = STATUS_VOLUME_DISMOUNTED;
        goto CleanupAndExit;
    }

    //
    // Clear the output buffer.
    //

    RtlZeroMemory(Irp->UserBuffer, IrpSp->Parameters.QueryVolume.Length);

    //
    // Dispatch the information class function.
    //

    status = STATUS_SUCCESS;

    switch (IrpSp->Parameters.QueryVolume.FsInformationClass) {

        case FileFsVolumeInformation:
            NOTHING;
            BytesWritten = FIELD_OFFSET(FILE_FS_VOLUME_INFORMATION, VolumeLabel);
            break;

        case FileFsSizeInformation:
            status = RawxQueryVolumeSizeInformation(VolumeExtension,
                (PFILE_FS_SIZE_INFORMATION)Irp->UserBuffer);
            BytesWritten = sizeof(FILE_FS_SIZE_INFORMATION);
            break;

        case FileFsDeviceInformation:
            DeviceInformation = (PFILE_FS_DEVICE_INFORMATION)Irp->UserBuffer;
            DeviceInformation->DeviceType =
                VolumeExtension->TargetDeviceObject->DeviceType;
            DeviceInformation->Characteristics = 0;
            BytesWritten = sizeof(FILE_FS_DEVICE_INFORMATION);
            break;

        case FileFsAttributeInformation:
            AttributeInformation =
                (PFILE_FS_ATTRIBUTE_INFORMATION)Irp->UserBuffer;
            AttributeInformation->FileSystemNameLength = sizeof(OCHAR) * 4;

            if (IrpSp->Parameters.QueryVolume.Length <
                FIELD_OFFSET(FILE_FS_ATTRIBUTE_INFORMATION, FileSystemName) +
                sizeof(OCHAR) * 4) {
                BytesWritten = FIELD_OFFSET(FILE_FS_ATTRIBUTE_INFORMATION,
                    FileSystemName);
                status = STATUS_BUFFER_OVERFLOW;
            } else {
                RtlCopyMemory(AttributeInformation->FileSystemName, OTEXT("RAWX"),
                    sizeof(OCHAR) * 4);
                BytesWritten = FIELD_OFFSET(FILE_FS_ATTRIBUTE_INFORMATION,
                    FileSystemName) + sizeof(OCHAR) * 4;
            }
            break;

        default:
            BytesWritten = 0;
            status = STATUS_INVALID_PARAMETER;
            break;
    }

    //
    // Fill in the number of bytes that we wrote to the user's buffer.
    //

    Irp->IoStatus.Information = BytesWritten;

    //
    // Check that we didn't overflow the user's buffer.  The I/O manager does
    // the initial check to make sure there's enough space for the static
    // structure for a given information class, but we might overflow the buffer
    // when copying in the variable length file name.
    //

    ASSERT(Irp->IoStatus.Information <= IrpSp->Parameters.QueryVolume.Length);

CleanupAndExit:
    RawxReleaseVolumeMutex(VolumeExtension);

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
