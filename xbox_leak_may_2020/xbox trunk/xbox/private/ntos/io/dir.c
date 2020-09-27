/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    dir.c

Abstract:

    This module contains the code to implement the NtQueryDirectoryFile
    system service for the NT I/O system.

--*/

#include "iop.h"

NTSTATUS
NtQueryDirectoryFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass,
    IN POBJECT_STRING FileName OPTIONAL,
    IN BOOLEAN RestartScan
    )

/*++

Routine Description:

    This service operates on a directory file specified by the FileHandle
    parameter.  The service returns information about files in the directory
    specified by the file handle.  The ReturnSingleEntry parameter specifies
    that only a single entry should be returned rather than filling the buffer.
    The actual number of files whose information is returned, is the smallest
    of the following:

        o  One entry, if the ReturnSingleEntry parameter is TRUE.

        o  The number of files whose information fits into the specified
           buffer.

        o  The number of files that exist.

        o  One entry if the optional FileName parameter is specified.

    If the optional FileName parameter is specified, then the only information
    that is returned is for that single file, if it exists.  Note that the
    file name may not specify any wildcard characters according to the naming
    conventions of the target file system.  The ReturnSingleEntry parameter is
    simply ignored.

    The information that is obtained about the files in the directory is based
    on the FileInformationClass parameter.  The legal values are as follows:

        o  FileNamesInformation

        o  FileDirectoryInformation

        o  FileFullDirectoryInformation

Arguments:

    FileHandle - Supplies a handle to the directory file for which information
        should be returned.

    Event - Supplies an optional event to be set to the Signaled state when
        the query is complete.

    ApcRoutine - Supplies an optional APC routine to be executed when the
        query is complete.

    ApcContext - Supplies a context parameter to be passed to the ApcRoutine,
        if an ApcRoutine was specified.

    IoStatusBlock - Address of the caller's I/O status block.

    FileInformation - Supplies a buffer to receive the requested information
        returned about the contents of the directory.

    Length - Supplies the length, in bytes, of the FileInformation buffer.

    FileInformationClass - Specfies the type of information that is to be
        returned about the files in the specified directory.

    ReturnSingleEntry - Supplies a BOOLEAN value that, if TRUE, indicates that
        only a single entry should be returned.

    FileName - Optionally supplies a file name within the specified directory.

    RestartScan - Supplies a BOOLEAN value that, if TRUE, indicates that the
        scan should be restarted from the beginning.  This parameter must be
        set to TRUE by the caller the first time the service is invoked.

Return Value:

    The status returned is success if the query operation was properly queued
    to the I/O system.  Once the operation completes, the status of the query
    can be determined by examining the Status field of the I/O status block.

--*/

{
    NTSTATUS status;
    BOOLEAN synchronousIo;
    PDEVICE_OBJECT deviceObject;
    PFILE_OBJECT fileObject;
    PIRP irp;
    PKEVENT eventObject = (PKEVENT) NULL;
    PIO_STACK_LOCATION irpSp;

    PAGED_CODE();

    //
    // Reference the file object so the target device can be found and the
    // access rights mask can be used in the following checks for callers in
    // user mode.  Note that if the handle does not refer to a file object,
    // then it will fail.
    //

    status = ObReferenceObjectByHandle( FileHandle,
                                        &IoFileObjectType,
                                        (PVOID *) &fileObject );
    if (!NT_SUCCESS( status )) {
        return status;
    }

    //
    // If this file has an I/O completion port associated w/it, then ensure
    // that the caller did not supply an APC routine, as the two are mutually
    // exclusive methods for I/O completion notification.
    //

    if (fileObject->CompletionContext && IopApcRoutinePresent( ApcRoutine )) {
        ObDereferenceObject( fileObject );
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Get the address of the event object and set the event to the Not-
    // Signaled state, if an event was specified.  Note here, too, that if
    // the handle does not refer to an event, or if the event cannot be
    // written, then the reference will fail.
    //

    if (ARGUMENT_PRESENT( Event )) {
        status = ObReferenceObjectByHandle( Event,
                                            &ExEventObjectType,
                                            (PVOID *) &eventObject );
        if (!NT_SUCCESS( status )) {
            ObDereferenceObject( fileObject );
            return status;
        } else {
            KeClearEvent( eventObject );
        }
    }

    //
    // Make a special check here to determine whether this is a synchronous
    // I/O operation.  If it is, then wait here until the file is owned by
    // the current thread.
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
    // Get the address of the target device object.
    //

    deviceObject = fileObject->DeviceObject;

    //
    // Allocate and initialize the I/O Request Packet (IRP) for this operation.
    // The allocation is performed with an exception handler in case the
    // caller does not have enough quota to allocate the packet.

    irp = IoAllocateIrp( deviceObject->StackSize );
    if (!irp) {

        //
        // An IRP could not be allocated.  Cleanup and return an appropriate
        // error status code.
        //

        return IopAllocateIrpCleanup( fileObject, eventObject );
    }
    irp->Tail.Overlay.OriginalFileObject = fileObject;
    irp->Tail.Overlay.Thread = PsGetCurrentThread();

    //
    // Fill in the service independent parameters in the IRP.
    //

    irp->UserEvent = eventObject;
    irp->UserIosb = IoStatusBlock;
    irp->Overlay.AsynchronousParameters.UserApcRoutine = ApcRoutine;
    irp->Overlay.AsynchronousParameters.UserApcContext = ApcContext;
    irp->Flags |= IRP_DEFER_IO_COMPLETION;

    //
    // Get a pointer to the stack location for the first driver.  This will be
    // used to pass the original function codes and parameters.
    //

    irpSp = IoGetNextIrpStackLocation( irp );
    irpSp->MajorFunction = IRP_MJ_DIRECTORY_CONTROL;
    irpSp->FileObject = fileObject;

    //
    // Pass the address of the user's buffer so the driver has access to
    // it.  It is now the driver's responsibility to do everything.
    //

    irp->UserBuffer = FileInformation;

    //
    // Copy the caller's parameters to the service-specific portion of the
    // IRP.
    //

    irpSp->Parameters.QueryDirectory.Length = Length;
    irpSp->Parameters.QueryDirectory.FileInformationClass = FileInformationClass;
    irpSp->Parameters.QueryDirectory.FileName = FileName;
    if (RestartScan) {
        irpSp->Flags = SL_RESTART_SCAN;
    }

    //
    // Queue the packet, call the driver, and synchronize appopriately with
    // I/O completion.
    //

    status = IopSynchronousServiceTail( deviceObject,
                                        irp,
                                        fileObject,
                                        TRUE,
                                        synchronousIo );

    return status;
}
