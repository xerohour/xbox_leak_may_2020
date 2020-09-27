/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    qsfs.c

Abstract:

    This module contains the code to implement the NtQueryVolumeInformationFile
    and NtSetVolumeInformationFile system services for the NT I/O system.

--*/

#include "iop.h"

NTSTATUS
NtQueryVolumeInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FsInformation,
    IN ULONG Length,
    IN FS_INFORMATION_CLASS FsInformationClass
    )

/*++

Routine Description:

    This service returns information about the volume associated with the
    FileHandle parameter.  The information returned in the buffer is defined
    by the FsInformationClass parameter.  The legal values for this parameter
    are as follows:

        o  FileFsVolumeInformation

        o  FileFsSizeInformation

        o  FileFsDeviceInformation

        o  FileFsAttributeInformation

Arguments:

    FileHandle - Supplies a handle to an open volume, directory, or file
        for which information about the volume is returned.

    IoStatusBlock - Address of the caller's I/O status block.

    FsInformation - Supplies a buffer to receive the requested information
        returned about the volume.

    Length - Supplies the length, in bytes, of the FsInformation buffer.

    FsInformationClass - Specifies the type of information which should be
        returned about the volume.

Return Value:

    The status returned is the final completion status of the operation.

--*/

{
    PIRP irp;
    NTSTATUS status;
    PFILE_OBJECT fileObject;
    PDEVICE_OBJECT deviceObject;
    PIO_STACK_LOCATION irpSp;
    KEVENT localEvent;
    IO_STATUS_BLOCK localIoStatus;
    BOOLEAN synchronousIo;

    PAGED_CODE();

    //
    // Ensure that the FsInformationClass parameter is legal for querying
    // information about the volume.
    //

    if ((ULONG) FsInformationClass >= FileFsMaximumInformation ||
        IopQueryFsOperationLength[FsInformationClass] == 0) {
        return STATUS_INVALID_INFO_CLASS;
    }

    //
    // Finally, ensure that the supplied buffer is large enough to contain
    // the information associated with the specified query operation that
    // is to be performed.
    //

    if (Length < (ULONG) IopQueryFsOperationLength[FsInformationClass]) {
        return STATUS_INFO_LENGTH_MISMATCH;
    }

    //
    // There were no blatant errors so far, so reference the file object so
    // the target device object can be found.  Note that if the handle does
    // not refer to a file object, or if the caller does not have the required
    // access to the file, then it will fail.
    //

    status = ObReferenceObjectByHandle( FileHandle,
                                        &IoFileObjectType,
                                        (PVOID *) &fileObject);
    if (!NT_SUCCESS( status )) {
        return status;
    }

    if ((IopQueryFsOperationAccess[FsInformationClass] & FILE_READ_DATA) &&
        !fileObject->ReadAccess) {
        ObDereferenceObject( fileObject );
        return STATUS_ACCESS_DENIED;
    }

    //
    // Make a special check here to determine whether this is a synchronous
    // I/O operation.  If it is, then wait here until the file is owned by
    // the current thread.  If this is not a (serialized) synchronous I/O
    // operation, then allocate and initialize the local event.
    //

    if (fileObject->Flags & FO_SYNCHRONOUS_IO) {
        IopAcquireFileObjectLock(fileObject);
        synchronousIo = TRUE;
    } else {
        synchronousIo = FALSE;
    }

    //
    // Set the file object to the Not-Signaled state.
    //

    KeClearEvent( &fileObject->Event );

    //
    // Get a pointer to the device object for the target device.
    //

    deviceObject = fileObject->DeviceObject;

    //
    // If this I/O operation is not being performed as synchronous I/O,
    // then allocate an event that will be used to synchronize the
    // completion of this operation.  That is, this system service is
    // a synchronous API being invoked for a file that is opened for
    // asynchronous I/O.
    //

    if (!(fileObject->Flags & FO_SYNCHRONOUS_IO)) {
        KeInitializeEvent( &localEvent, SynchronizationEvent, FALSE );
    }

    //
    // Allocate and initialize the I/O Request Packet (IRP) for this
    // operation.  The allocation is performed with an exception handler
    // in case the caller does not have enough quota to allocate the packet.

    irp = IoAllocateIrp( deviceObject->StackSize );
    if (!irp) {

        //
        // An IRP could not be allocated.  Cleanup and return an
        // appropriate error status code.
        //

        return IopAllocateIrpCleanup( fileObject, (PKEVENT) NULL );
    }
    irp->Tail.Overlay.OriginalFileObject = fileObject;
    irp->Tail.Overlay.Thread = PsGetCurrentThread();

    //
    // Fill in the service independent parameters in the IRP.
    //

    irp->UserBuffer = FsInformation;
    if (synchronousIo) {
        irp->UserIosb = IoStatusBlock;
    } else {
        irp->UserEvent = &localEvent;
        irp->UserIosb = &localIoStatus;
        irp->Flags = IRP_SYNCHRONOUS_API;
    }
    irp->Flags |= IRP_DEFER_IO_COMPLETION;

    //
    // Get a pointer to the stack location for the first driver.  This will
    // be used to pass the original function codes and parameters.
    //

    irpSp = IoGetNextIrpStackLocation( irp );
    irpSp->MajorFunction = IRP_MJ_QUERY_VOLUME_INFORMATION;
    irpSp->FileObject = fileObject;

    //
    // Copy the caller's parameters to the service-specific portion of the
    // IRP.
    //

    irpSp->Parameters.QueryVolume.Length = Length;
    irpSp->Parameters.QueryVolume.FsInformationClass = FsInformationClass;

    //
    // Queue the packet, call the driver, and synchronize appopriately with
    // I/O completion.
    //

    status = IopSynchronousServiceTail( deviceObject,
                                        irp,
                                        fileObject,
                                        TRUE,
                                        synchronousIo );

    //
    // If the file for this operation was not opened for synchronous I/O, then
    // synchronization of completion of the I/O operation has not yet occurred
    // since the allocated event must be used for synchronous APIs on files
    // opened for asynchronous I/O.  Synchronize the completion of the I/O
    // operation now.
    //

    if (!synchronousIo) {

        status = IopSynchronousApiServiceTail( status,
                                               &localEvent,
                                               irp,
                                               &localIoStatus,
                                               IoStatusBlock );
    }

    return status;
}
