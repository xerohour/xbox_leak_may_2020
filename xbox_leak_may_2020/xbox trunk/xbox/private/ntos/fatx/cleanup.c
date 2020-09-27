/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    cleanup.c

Abstract:

    This module implements routines related to handling IRP_MJ_CLEANUP.

--*/

#include "fatx.h"

NTSTATUS
FatxFsdCleanup(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_CLEANUP requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PFAT_VOLUME_EXTENSION VolumeExtension;
    PFILE_OBJECT FileObject;
    PFAT_FCB Fcb;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)DeviceObject->DeviceExtension;
    FileObject = IoGetCurrentIrpStackLocation(Irp)->FileObject;
    Fcb = (PFAT_FCB)FileObject->FsContext;

    //
    // Synchronize with pending asynchronous I/O by acquiring the file's mutex.
    //

    if (FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY)) {
        FatxAcquireFileMutexExclusive(Fcb);
    }

    //
    // Always grab the exclusive lock to synchronize access to the share access
    // data structure.  Multiple threads closing distinct file objects pointing
    // at the same file control block could execute this routine at the same
    // time.
    //

    FatxAcquireVolumeMutexExclusive(VolumeExtension);

    //
    // Cleanup the sharing mode associated with the file object.
    //

    IoRemoveShareAccess(FileObject, &Fcb->ShareAccess);

    //
    // If this was the last open handle to the file, then check if we need to
    // perform any cleanup actions for the file.  Abandon the actions if the
    // volume has been dismounted.
    //

    if ((Fcb->ShareAccess.OpenCount == 0) &&
        FatxIsFlagClear(VolumeExtension->Flags, FAT_VOLUME_DISMOUNTED)) {

        if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_DELETE_ON_CLOSE)) {

            //
            // The file or directory is flagged for deletion.  Mark the
            // directory entry as deleted.
            //

            ASSERT(FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME | FAT_FCB_ROOT_DIRECTORY));

            status = FatxMarkDirectoryEntryDeleted(VolumeExtension, Irp, Fcb);

            if (NT_SUCCESS(status)) {

                //
                // The directory entry was succesfully marked as deleted, so
                // free the clusters allocated to the file.  Note that
                // FatxFreeClusters checks if the first cluster number is valid
                // for the volume, so we don't have to do any validation here.
                //

                FatxFreeClusters(VolumeExtension, Irp, Fcb->FirstCluster, FALSE);

                //
                // Remove this file control block from the list of siblings so
                // and dereference the parent file control block.
                //

                if (Fcb->ParentFcb != NULL) {

                    RemoveEntryList(&Fcb->SiblingFcbLink);
                    FatxDereferenceFcb(Fcb->ParentFcb);

                    Fcb->ParentFcb = NULL;
                }
            }

        } else if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_UPDATE_DIRECTORY_ENTRY)) {

            //
            // If there are any pending changes to flush out to the directory
            // entry (last write time), then do that now.
            //

            FatxUpdateDirectoryEntry(VolumeExtension, Irp, Fcb);
        }
    }

    //
    // Mark the file object as having been cleaned up.
    //

    FileObject->Flags |= FO_CLEANUP_COMPLETE;

    //
    // Release the mutexes that we acquired above.
    //

    FatxReleaseVolumeMutex(VolumeExtension);

    if (FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY)) {
        FatxReleaseFileMutex(Fcb);
    }

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}
