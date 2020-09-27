/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    close.c

Abstract:

    This module implements routines related to handling IRP_MJ_CLOSE.

--*/

#include "gdfx.h"

NTSTATUS
GdfxFsdClose(
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
    PGDF_VOLUME_EXTENSION VolumeExtension;
    PIO_STACK_LOCATION IrpSp;
    PFILE_OBJECT FileObject;
    PDIRECTORY_ENUM_CONTEXT DirectoryEnumContext;
    PGDF_FCB Fcb;

    VolumeExtension = (PGDF_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    FileObject = IrpSp->FileObject;

    //
    // Synchronize access to the file control block lists by acquiring the
    // global mutex.
    //

    GdfxAcquireGlobalMutexExclusive();

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

    Fcb = (PGDF_FCB)FileObject->FsContext;
    FileObject->FsContext = NULL;
    GdfxDereferenceFcb(Fcb);

    //
    // Decrement the file object count for the volume and check if we're ready
    // to delete this device object.
    //

    VolumeExtension->FileObjectCount--;

    if ((VolumeExtension->FileObjectCount == 0) && VolumeExtension->Dismounted) {
        GdfxDeleteVolumeDevice(DeviceObject);
    }

    GdfxReleaseGlobalMutex();

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}
