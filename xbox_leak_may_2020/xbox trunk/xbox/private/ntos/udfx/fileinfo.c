/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fileinfo.c

Abstract:

    This module implements routines related to handling
    IRP_MJ_QUERY_INFORMATION and IRP_MJ_SET_INFORMATION.

--*/

#include "udfx.h"

VOID
UdfxQueryNetworkOpenInformation(
    IN PFILE_OBJECT FileObject,
    OUT PFILE_NETWORK_OPEN_INFORMATION NetworkOpenInformation
    )
/*++

Routine Description:

    This routine fills the information structure with attributes about the
    supplied file object.

Arguments:

    FileObject - Specifies the file object to obtain the information from.

    NetworkOpenInformation - Specifies the buffer to receive the file
        information.

Return Value:

    None.

--*/
{
    PUDF_FCB Fcb;

    Fcb = (PUDF_FCB)FileObject->FsContext;

    NetworkOpenInformation->CreationTime =
    NetworkOpenInformation->LastAccessTime =
    NetworkOpenInformation->LastWriteTime =
    NetworkOpenInformation->ChangeTime =
        UdfxUdfTimestampToTime(&Fcb->ModifyTime);

    if (UdfxIsFlagSet(Fcb->Flags, UDF_FCB_DIRECTORY)) {

        NetworkOpenInformation->FileAttributes = FILE_ATTRIBUTE_READONLY |
            FILE_ATTRIBUTE_DIRECTORY;
        NetworkOpenInformation->AllocationSize.QuadPart = 0;
        NetworkOpenInformation->EndOfFile.QuadPart = 0;

    } else {

        NetworkOpenInformation->FileAttributes = FILE_ATTRIBUTE_READONLY;
        NetworkOpenInformation->AllocationSize = Fcb->FileSize;
        NetworkOpenInformation->EndOfFile = Fcb->FileSize;
    }
}

NTSTATUS
UdfxFsdQueryInformation(
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
    PUDF_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    PFILE_OBJECT FileObject;
    PUDF_FCB Fcb;
    ULONG BytesWritten;
    PFILE_INTERNAL_INFORMATION InternalInformation;
    PFILE_POSITION_INFORMATION PositionInformation;

    VolumeExtension = (PUDF_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    FileObject = IrpSp->FileObject;
    Fcb = (PUDF_FCB)FileObject->FsContext;

    //
    // For volume file control blocks, the only thing that can be queried is the
    // current file position.
    //

    if (UdfxIsFlagSet(Fcb->Flags, UDF_FCB_VOLUME) &&
        (IrpSp->Parameters.SetFile.FileInformationClass != FilePositionInformation)) {
        status = STATUS_INVALID_PARAMETER;
        goto CleanupAndExit;
    }

    //
    // Clear the output buffer.
    //

    RtlZeroMemory(Irp->UserBuffer, IrpSp->Parameters.QueryFile.Length);

    //
    // Dispatch the information class function.
    //

    status = STATUS_SUCCESS;

    switch (IrpSp->Parameters.QueryFile.FileInformationClass) {

        case FileInternalInformation:
            InternalInformation = (PFILE_INTERNAL_INFORMATION)Irp->UserBuffer;
            InternalInformation->IndexNumber.HighPart = PtrToUlong(VolumeExtension);
            InternalInformation->IndexNumber.LowPart = PtrToUlong(Fcb);
            BytesWritten = sizeof(FILE_INTERNAL_INFORMATION);
            break;

        case FilePositionInformation:
            PositionInformation = (PFILE_POSITION_INFORMATION)Irp->UserBuffer;
            PositionInformation->CurrentByteOffset = FileObject->CurrentByteOffset;
            BytesWritten = sizeof(FILE_POSITION_INFORMATION);
            break;

        case FileNetworkOpenInformation:
            UdfxQueryNetworkOpenInformation(FileObject,
                (PFILE_NETWORK_OPEN_INFORMATION)Irp->UserBuffer);
            BytesWritten = sizeof(FILE_NETWORK_OPEN_INFORMATION);
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

    ASSERT(Irp->IoStatus.Information <= IrpSp->Parameters.QueryFile.Length);

CleanupAndExit:
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS
UdfxFsdSetInformation(
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
    // Note that we don't use UDF_SECTOR_CD_MASK here so that the generated code
    // ends up being identical to RawxFsdSetInformation.  The linker will take
    // care of collapsing the two functions.
    //

    if (UdfxIsFlagSet(FileObject->Flags, FO_NO_INTERMEDIATE_BUFFERING) &&
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
