/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    volinfo.c

Abstract:

    This module implements routines related to handling
    IRP_MJ_QUERY_VOLUME_INFORMATION.

--*/

#include "fatx.h"

NTSTATUS
FatxFsdQueryVolumeInformation(
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
    PFAT_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    ULONG BytesWritten;
    PFILE_FS_VOLUME_INFORMATION VolumeInformation;
    PFILE_FS_SIZE_INFORMATION SizeInformation;
    PFILE_FS_DEVICE_INFORMATION DeviceInformation;
    PFILE_FS_ATTRIBUTE_INFORMATION AttributeInformation;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    FatxAcquireVolumeMutexShared(VolumeExtension);

    //
    // Check if the volume has been dismounted.
    //

    if (FatxIsFlagSet(VolumeExtension->Flags, FAT_VOLUME_DISMOUNTED)) {
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
            VolumeInformation = (PFILE_FS_VOLUME_INFORMATION)Irp->UserBuffer;
            VolumeInformation->VolumeSerialNumber = VolumeExtension->SerialNumber;
            BytesWritten = FIELD_OFFSET(FILE_FS_VOLUME_INFORMATION, VolumeLabel);
            break;

        case FileFsSizeInformation:
            SizeInformation = (PFILE_FS_SIZE_INFORMATION)Irp->UserBuffer;
            SizeInformation->TotalAllocationUnits.QuadPart =
                VolumeExtension->NumberOfClusters;
            SizeInformation->AvailableAllocationUnits.QuadPart =
                VolumeExtension->NumberOfClustersAvailable;
            SizeInformation->SectorsPerAllocationUnit =
                VolumeExtension->BytesPerCluster >> VolumeExtension->SectorShift;
            SizeInformation->BytesPerSector = VolumeExtension->SectorSize;
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
            AttributeInformation->FileSystemAttributes = 0;
            AttributeInformation->MaximumComponentNameLength =
                FAT_FILE_NAME_LENGTH;
            AttributeInformation->FileSystemNameLength = sizeof(OCHAR) * 4;

            if (IrpSp->Parameters.QueryVolume.Length <
                FIELD_OFFSET(FILE_FS_ATTRIBUTE_INFORMATION, FileSystemName) +
                sizeof(OCHAR) * 4) {
                BytesWritten = FIELD_OFFSET(FILE_FS_ATTRIBUTE_INFORMATION,
                    FileSystemName);
                status = STATUS_BUFFER_OVERFLOW;
            } else {
                RtlCopyMemory(AttributeInformation->FileSystemName, OTEXT("FATX"),
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
    FatxReleaseVolumeMutex(VolumeExtension);

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
