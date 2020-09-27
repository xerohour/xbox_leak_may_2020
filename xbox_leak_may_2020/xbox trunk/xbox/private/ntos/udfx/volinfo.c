/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    volinfo.c

Abstract:

    This module implements routines related to handling
    IRP_MJ_QUERY_VOLUME_INFORMATION.

--*/

#include "udfx.h"

NTSTATUS
UdfxFsdQueryVolumeInformation(
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
    PUDF_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    ULONG BytesWritten;
    PFILE_FS_SIZE_INFORMATION SizeInformation;
    PFILE_FS_DEVICE_INFORMATION DeviceInformation;
    PFILE_FS_ATTRIBUTE_INFORMATION AttributeInformation;

    VolumeExtension = (PUDF_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

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
            SizeInformation = (PFILE_FS_SIZE_INFORMATION)Irp->UserBuffer;
            SizeInformation->TotalAllocationUnits.QuadPart =
                VolumeExtension->PartitionLength.QuadPart >> UDF_CD_SECTOR_SHIFT;
            SizeInformation->SectorsPerAllocationUnit = 1;
            SizeInformation->BytesPerSector = UDF_CD_SECTOR_SIZE;
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
            AttributeInformation->MaximumComponentNameLength = 255;
            AttributeInformation->FileSystemNameLength = sizeof(OCHAR) * 4;

            if (IrpSp->Parameters.QueryVolume.Length <
                FIELD_OFFSET(FILE_FS_ATTRIBUTE_INFORMATION, FileSystemName) +
                sizeof(OCHAR) * 4) {
                BytesWritten = FIELD_OFFSET(FILE_FS_ATTRIBUTE_INFORMATION,
                    FileSystemName);
                status = STATUS_BUFFER_OVERFLOW;
            } else {
                RtlCopyMemory(AttributeInformation->FileSystemName, OTEXT("UDFX"),
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

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
