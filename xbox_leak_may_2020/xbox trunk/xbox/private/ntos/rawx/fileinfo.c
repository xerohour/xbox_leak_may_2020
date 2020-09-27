/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fileinfo.c

Abstract:

    This module implements routines related to handling
    IRP_MJ_QUERY_INFORMATION and IRP_MJ_SET_INFORMATION.

--*/

#include "rawx.h"

NTSTATUS
RawxFsdQueryInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_QUERY_INFORMATION
    requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PIO_STACK_LOCATION IrpSp;
    PFILE_POSITION_INFORMATION PositionInformation;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // The only thing that can be queried is the current file position.
    //

    if (IrpSp->Parameters.QueryFile.FileInformationClass != FilePositionInformation) {
        status = STATUS_INVALID_PARAMETER;
        goto CleanupAndExit;
    }

    PositionInformation = (PFILE_POSITION_INFORMATION)Irp->UserBuffer;

    //
    // Return the current file position in the user's buffer.
    //

    PositionInformation->CurrentByteOffset = IrpSp->FileObject->CurrentByteOffset;

    //
    // Fill in the number of bytes that we wrote to the user's buffer.
    //

    Irp->IoStatus.Information = sizeof(FILE_POSITION_INFORMATION);
    status = STATUS_SUCCESS;

CleanupAndExit:
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS
RawxFsdSetInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_SET_INFORMATION
    requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PIO_STACK_LOCATION IrpSp;
    PFILE_OBJECT FileObject;
    PFILE_POSITION_INFORMATION PositionInformation;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    FileObject = IrpSp->FileObject;

    //
    // The only thing that can be set is the current file position.
    //

    if (IrpSp->Parameters.SetFile.FileInformationClass != FilePositionInformation) {
        status = STATUS_INVALID_PARAMETER;
        goto CleanupAndExit;
    }

    PositionInformation = (PFILE_POSITION_INFORMATION)Irp->UserBuffer;

    //
    // If the file was opened without intermediate buffering, then the byte
    // offset must be sector aligned.
    //
    // Note that we still do the check for FO_NO_INTERMEDIATE_BUFFERING even
    // though this is always set for RAWX file objects; this keeps the code
    // identical to GdfxFsdSetInformation and allows the functions to be
    // collpased by the linker.
    //

    if (RawxIsFlagSet(FileObject->Flags, FO_NO_INTERMEDIATE_BUFFERING) &&
        (PositionInformation->CurrentByteOffset.LowPart &
        (DeviceObject->SectorSize - 1)) != 0) {
        status = STATUS_INVALID_PARAMETER;
        goto CleanupAndExit;
    }

    //
    // Update the current file position.
    //

    FileObject->CurrentByteOffset = PositionInformation->CurrentByteOffset;
    status = STATUS_SUCCESS;

CleanupAndExit:
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}
