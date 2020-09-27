/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    close.c

Abstract:

    This module implements routines related to handling IRP_MJ_CLOSE.

--*/

#include "fatx.h"

NTSTATUS
FatxFsdClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_CLOSE requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    PFAT_VOLUME_EXTENSION VolumeExtension;
    PFILE_OBJECT FileObject;
    PDIRECTORY_ENUM_CONTEXT DirectoryEnumContext;
    PFAT_FCB Fcb;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;

    //
    // Always grab the exclusive lock because we could end up writing to the
    // disk and we also use this lock to guard access to the file control block
    // and file control block list.
    //

    FatxAcquireVolumeMutexExclusive(VolumeExtension);

    FileObject = IoGetCurrentIrpStackLocation(Irp)->FileObject;

    //
    // Delete the directory enumeration context if one exists.
    //

    DirectoryEnumContext = (PDIRECTORY_ENUM_CONTEXT)FileObject->FsContext2;

    if (DirectoryEnumContext != NULL) {
        FileObject->FsContext2 = NULL;
        ExFreePool(DirectoryEnumContext);
    }

    //
    // Dereference the file control block associated with the file object.
    //

    Fcb = (PFAT_FCB)FileObject->FsContext;
    FileObject->FsContext = NULL;
    FatxDereferenceFcb(Fcb);

    //
    // Decrement the file object count for the volume and check if we're ready
    // to delete this device object.
    //

    VolumeExtension->FileObjectCount--;

    if ((VolumeExtension->FileObjectCount == 0) &&
        FatxIsFlagSet(VolumeExtension->Flags, FAT_VOLUME_DISMOUNTED)) {
        FatxReleaseVolumeMutex(VolumeExtension);
        FatxDeleteVolumeDevice(DeviceObject);
    } else {
        FatxReleaseVolumeMutex(VolumeExtension);
    }

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}
