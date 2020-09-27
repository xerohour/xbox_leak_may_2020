/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    qsinfo.c

Abstract:

    This module contains the code to implement the NtQueryInformationFile and
    NtSetInformationFile system services for the NT I/O system.

--*/

#include "iop.h"

//
// Create local definitions for long flag names to make code slightly more
// readable.
//

#define FSIO_A  FILE_SYNCHRONOUS_IO_ALERT
#define FSIO_NA FILE_SYNCHRONOUS_IO_NONALERT

ULONG
IopGetModeInformation(
    IN PFILE_OBJECT FileObject
    )

/*++

Routine Description:

    This encapsulates extracting and translating the mode bits from
    the passed file object, to be returned from a query information call.

Arguments:

    FileObject - Specifies the file object for which to return Mode info.

Return Value:

    The translated mode information is returned.

--*/

{
    ULONG mode = 0;

    if (FileObject->Flags & FO_SEQUENTIAL_ONLY) {
        mode |= FILE_SEQUENTIAL_ONLY;
    }
    if (FileObject->Flags & FO_NO_INTERMEDIATE_BUFFERING) {
        mode |= FILE_NO_INTERMEDIATE_BUFFERING;
    }
    if (FileObject->Flags & FO_SYNCHRONOUS_IO) {
        if (FileObject->Flags & FO_ALERTABLE_IO) {
            mode |= FILE_SYNCHRONOUS_IO_ALERT;
        } else {
            mode |= FILE_SYNCHRONOUS_IO_NONALERT;
        }
    }
    return mode;
}

NTSTATUS
NtQueryInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass
    )

/*++

Routine Description:

    This service returns the requested information about a specified file.
    The information returned is determined by the FileInformationClass that
    is specified, and it is placed into the caller's FileInformation buffer.

Arguments:

    FileHandle - Supplies a handle to the file about which the requested
        information should be returned.

    IoStatusBlock - Address of the caller's I/O status block.

    FileInformation - Supplies a buffer to receive the requested information
        returned about the file.

    Length - Supplies the length, in bytes, of the FileInformation buffer.

    FileInformationClass - Specifies the type of information which should be
        returned about the file.

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
    BOOLEAN skipDriver;

    PAGED_CODE();

    //
    // The caller's mode is kernel.  Ensure that at least the information
    // class and lengths are appropriate.
    //

    if ((ULONG) FileInformationClass >= FileMaximumInformation ||
        !IopQueryOperationLength[FileInformationClass]) {
        return STATUS_INVALID_INFO_CLASS;
    }

    if (Length < (ULONG) IopQueryOperationLength[FileInformationClass]) {
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
                                        (PVOID *) &fileObject );

    if (!NT_SUCCESS( status )) {
        return status;
    }

    if ((IopQueryOperationAccess[FileInformationClass] & FILE_READ_DATA) &&
        !fileObject->ReadAccess) {
        ObDereferenceObject( fileObject );
        return STATUS_ACCESS_DENIED;
    }

    //
    // Get the address of the target device object.
    //

    deviceObject = fileObject->DeviceObject;

    //
    // Make a special check here to determine whether this is a synchronous
    // I/O operation.  If it is, then wait here until the file is owned by
    // the current thread.  If this is not a (serialized) synchronous I/O
    // operation, then allocate and initialize the local event.
    //

    if (fileObject->Flags & FO_SYNCHRONOUS_IO) {

        IopAcquireFileObjectLock(fileObject);

        //
        // Make a special check here to determine whether or not the caller
        // is attempting to query the file position pointer.  If so, then
        // return it immediately and get out.
        //

        if (FileInformationClass == FilePositionInformation) {

            //
            // The caller has requested the current file position context
            // information.  This is a relatively frequent call, so it is
            // optimized here to cut through the normal IRP path.
            //

            PFILE_POSITION_INFORMATION fileInformation = FileInformation;

            //
            // Return the current position information.
            //

            fileInformation->CurrentByteOffset = fileObject->CurrentByteOffset;

            //
            // Write the I/O status block.
            //

            IoStatusBlock->Status = STATUS_SUCCESS;
            IoStatusBlock->Information = sizeof( FILE_POSITION_INFORMATION );

            //
            // Note that the state of the event in the file object has not yet
            // been reset, so it need not be set either.  Therefore, simply
            // cleanup and return.
            //

            IopReleaseFileObjectLock( fileObject );
            ObDereferenceObject( fileObject );
            return status;
        }
        synchronousIo = TRUE;
    } else {

        //
        // This is a synchronous API being invoked for a file that is opened
        // for asynchronous I/O.  This means that this system service is
        // to synchronize the completion of the operation before returning
        // to the caller.  A local event is used to do this.
        //

        KeInitializeEvent( &localEvent, SynchronizationEvent, FALSE );
        synchronousIo = FALSE;
    }

    //
    // Set the file object to the Not-Signaled state.
    //

    KeClearEvent( &fileObject->Event );

    //
    // Allocate and initialize the I/O Request Packet (IRP) for this operation.
    // The allocation is performed with an exception handler in case the
    // caller does not have enough quota to allocate the packet.
    //

    irp = IoAllocateIrp( deviceObject->StackSize );
    if (!irp) {

        //
        // An IRP could not be allocated.  Cleanup and return an appropriate
        // error status code.
        //

        return IopAllocateIrpCleanup( fileObject, (PKEVENT) NULL );
    }
    irp->Tail.Overlay.OriginalFileObject = fileObject;
    irp->Tail.Overlay.Thread = PsGetCurrentThread();

    //
    // Fill in the service independent parameters in the IRP.
    //

    irp->UserBuffer = FileInformation;
    if (synchronousIo) {
        irp->UserIosb = IoStatusBlock;
    } else {
        irp->UserEvent = &localEvent;
        irp->UserIosb = &localIoStatus;
        irp->Flags = IRP_SYNCHRONOUS_API;
    }
    irp->Flags |= IRP_DEFER_IO_COMPLETION;

    //
    // Get a pointer to the stack location for the first driver.  This will be
    // used to pass the original function codes and parameters.
    //

    irpSp = IoGetNextIrpStackLocation( irp );
    irpSp->MajorFunction = IRP_MJ_QUERY_INFORMATION;
    irpSp->FileObject = fileObject;

    //
    // Copy the caller's parameters to the service-specific portion of the
    // IRP.
    //

    irpSp->Parameters.QueryFile.Length = Length;
    irpSp->Parameters.QueryFile.FileInformationClass = FileInformationClass;

    //
    // Insert the packet at the head of the IRP list for the thread.
    //

    IopQueueThreadIrp( irp );

    //
    // Everything is now set to invoke the device driver with this request.
    // However, it is possible that the information that the caller wants
    // is device independent.  If this is the case, then the request can
    // be satisfied here without having to have all of the drivers implement
    // the same code.  Note that having the IRP is still necessary since
    // the I/O completion code requires it.
    //

    skipDriver = FALSE;

    if (FileInformationClass == FileModeInformation) {

        PFILE_MODE_INFORMATION modeBuffer = FileInformation;

        //
        // Return the mode information for this file.
        //

        modeBuffer->Mode = IopGetModeInformation( fileObject );

        //
        // Complete the I/O operation.
        //

        irp->IoStatus.Information = sizeof( FILE_MODE_INFORMATION );
        skipDriver = TRUE;

    } else if (FileInformationClass == FileAlignmentInformation) {

        PFILE_ALIGNMENT_INFORMATION alignmentInformation = FileInformation;

        //
        // Return the alignment information for this file.
        //

        alignmentInformation->AlignmentRequirement = deviceObject->AlignmentRequirement;

        //
        // Complete the I/O operation.
        //

        irp->IoStatus.Information = sizeof( FILE_ALIGNMENT_INFORMATION );
        skipDriver = TRUE;
    }

    if (skipDriver) {

        //
        // The requested operation has already been performed.  Simply
        // set the final status in the packet and the return state.
        //

        status = STATUS_SUCCESS;
        irp->IoStatus.Status = STATUS_SUCCESS;

    } else {

        //
        // This is not a request that can be [completely] performed here, so
        // invoke the driver at its appropriate dispatch entry with the IRP.
        //

        status = IoCallDriver( deviceObject, irp );
    }

    //
    // If this operation was a synchronous I/O operation, check the return
    // status to determine whether or not to wait on the file object.  If
    // the file object is to be waited on, wait for the operation to complete
    // and obtain the final status from the file object itself.
    //

    if (status == STATUS_PENDING) {

        if (synchronousIo) {

            KeWaitForSingleObject( &fileObject->Event,
                                   Executive,
                                   KernelMode,
                                   FALSE,
                                   (PLARGE_INTEGER) NULL );

            status = fileObject->FinalStatus;

            IopReleaseFileObjectLock( fileObject );

        } else {

            //
            // This is a normal synchronous I/O operation, as opposed to a
            // serialized synchronous I/O operation.  For this case, wait for
            // the local event and copy the final status information back to
            // the caller.
            //

            KeWaitForSingleObject( &localEvent,
                                   Executive,
                                   KernelMode,
                                   FALSE,
                                   (PLARGE_INTEGER) NULL );

            status = localIoStatus.Status;

            *IoStatusBlock = localIoStatus;
        }

    } else {

        //
        // The I/O operation finished without return a status of pending.
        // This means that the operation has not been through I/O completion,
        // so it must be done here.
        //

        PKNORMAL_ROUTINE normalRoutine;
        PVOID normalContext;
        KIRQL irql;

        if (!synchronousIo) {

            //
            // This is not a synchronous I/O operation, it is a synchronous
            // I/O API to a file opened for asynchronous I/O.  Since this
            // code path need never wait on the allocated and supplied event,
            // get rid of it so that it doesn't have to be set to the
            // Signaled state by the I/O completion code.
            //

            irp->UserEvent = (PKEVENT) NULL;
        }

        irp->UserIosb = IoStatusBlock;
        KeRaiseIrql( APC_LEVEL, &irql );
        IopCompleteRequest( &irp->Tail.Apc,
                            &normalRoutine,
                            &normalContext,
                            (PVOID *) &fileObject,
                            &normalContext );
        KeLowerIrql( irql );

        if (synchronousIo) {
            IopReleaseFileObjectLock( fileObject );
        }
    }

    return status;
}

NTSTATUS
NtSetInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass
    )

/*++

Routine Description:

    This service changes the provided information about a specified file.  The
    information that is changed is determined by the FileInformationClass that
    is specified.  The new information is taken from the FileInformation buffer.

Arguments:

    FileHandle - Supplies a handle to the file whose information should be
        changed.

    IoStatusBlock - Address of the caller's I/O status block.

    FileInformation - Supplies a buffer containing the information which should
        be changed on the file.

    Length - Supplies the length, in bytes, of the FileInformation buffer.

    FileInformationClass - Specifies the type of information which should be
        changed about the file.

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
    HANDLE targetHandle = (HANDLE) NULL;
    BOOLEAN synchronousIo;

    PAGED_CODE();

    //
    // The caller's mode is kernel.  Ensure that at least the information
    // class and lengths are appropriate.
    //

    if ((ULONG) FileInformationClass >= FileMaximumInformation ||
        FileInformationClass == FileTrackingInformation ||
        !IopSetOperationLength[FileInformationClass]) {
        return STATUS_INVALID_INFO_CLASS;
    }

    if (Length < (ULONG) IopSetOperationLength[FileInformationClass]) {
        return STATUS_INFO_LENGTH_MISMATCH;
    }

    //
    // Negative file offsets are illegal.
    //

    ASSERT((FIELD_OFFSET(FILE_END_OF_FILE_INFORMATION, EndOfFile) |
            FIELD_OFFSET(FILE_ALLOCATION_INFORMATION, AllocationSize) |
            FIELD_OFFSET(FILE_POSITION_INFORMATION, CurrentByteOffset)) == 0);

    if (((FileInformationClass == FileEndOfFileInformation) ||
         (FileInformationClass == FileAllocationInformation) ||
         (FileInformationClass == FilePositionInformation)) &&
        (((PFILE_POSITION_INFORMATION)FileInformation)->CurrentByteOffset.HighPart < 0)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // There were no blatant errors so far, so reference the file object so
    // the target device object can be found.  Note that if the handle does
    // not refer to a file object, or if the caller does not have the required
    // access to the file, then it will fail.
    //

    status = ObReferenceObjectByHandle( FileHandle,
                                        &IoFileObjectType,
                                        (PVOID *) &fileObject );
    if (!NT_SUCCESS( status )) {
        return status;
    }

    if ((IopSetOperationAccess[FileInformationClass] & FILE_WRITE_DATA) &&
        !fileObject->WriteAccess) {
        ObDereferenceObject( fileObject );
        return STATUS_ACCESS_DENIED;
    }

    //
    // Get the address of the target device object.
    //

    deviceObject = fileObject->DeviceObject;

    //
    // Make a special check here to determine whether this is a synchronous
    // I/O operation.  If it is, then wait here until the file is owned by
    // the current thread.  If this is not a (serialized) synchronous I/O
    // operation, then allocate and initialize the local event.
    //

    if (fileObject->Flags & FO_SYNCHRONOUS_IO) {

        IopAcquireFileObjectLock(fileObject);

        //
        // Make a special check here to determine whether or not the caller
        // is attempting to set the file position pointer information.  If so,
        // then set it immediately and get out.
        //

        if (FileInformationClass == FilePositionInformation) {

            //
            // The caller has requested setting the current file position
            // context information.  This is a relatively frequent call, so
            // it is optimized here to cut through the normal IRP path.
            //
            // Begin by checking to see whether the file was opened with no
            // intermediate buffering.  If so, then the file pointer must be
            // set in a manner consistent with the alignment requirement of
            // read and write operations to a non-buffered file.
            //

            PFILE_POSITION_INFORMATION fileInformation = FileInformation;
            LARGE_INTEGER currentByteOffset;

            //
            // Attempt to read the position information from the buffer.
            //

            currentByteOffset.QuadPart = fileInformation->CurrentByteOffset.QuadPart;

            if ((fileObject->Flags & FO_NO_INTERMEDIATE_BUFFERING &&
                 (deviceObject->SectorSize &&
                 (currentByteOffset.LowPart &
                 (deviceObject->SectorSize - 1)))) ||
                 currentByteOffset.HighPart < 0) {

                    status = STATUS_INVALID_PARAMETER;

            } else {

                //
                // Set the current file position information.
                //

                fileObject->CurrentByteOffset.QuadPart = currentByteOffset.QuadPart;

                //
                // Write the I/O status block.
                //

                IoStatusBlock->Status = STATUS_SUCCESS;
                IoStatusBlock->Information = 0;
            }

            //
            // Note that the file object's event has not yet been reset,
            // so it is not necessary to set it to the Signaled state, since
            // that is it's state at this point by definition.  Therefore,
            // simply cleanup and return.
            //

            IopReleaseFileObjectLock( fileObject );
            ObDereferenceObject( fileObject );
            return status;
        }
        synchronousIo = TRUE;
    } else {

        //
        // This is a synchronous API being invoked for a file that is opened
        // for asynchronous I/O.  This means that this system service is
        // to synchronize the completion of the operation before returning
        // to the caller.  A local event is used to do this.
        //

        KeInitializeEvent( &localEvent, SynchronizationEvent, FALSE );
        synchronousIo = FALSE;
    }

    //
    // Set the file object to the Not-Signaled state.
    //

    KeClearEvent( &fileObject->Event );

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

        return IopAllocateIrpCleanup( fileObject, (PKEVENT) NULL );
    }
    irp->Tail.Overlay.OriginalFileObject = fileObject;
    irp->Tail.Overlay.Thread = PsGetCurrentThread();

    //
    // Fill in the service independent parameters in the IRP.
    //

    irp->UserBuffer = FileInformation;
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
    irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
    irpSp->FileObject = fileObject;

    //
    // Copy the caller's parameters to the service-specific portion of the
    // IRP.
    //

    irpSp->Parameters.SetFile.Length = Length;
    irpSp->Parameters.SetFile.FileInformationClass = FileInformationClass;

    //
    // Insert the packet at the head of the IRP list for the thread.
    //

    IopQueueThreadIrp( irp );

    //
    // Everything is now set to invoke the device driver with this request.
    // However, it is possible that the information that the caller wants
    // to set is device independent.  If this is the case, then the request
    // can be satisfied here without having to have all of the drivers
    // implement the same code.  Note that having the IRP is still necessary
    // since the I/O completion code requires it.
    //

    if (FileInformationClass == FileModeInformation) {

        PFILE_MODE_INFORMATION modeBuffer = FileInformation;

        //
        // Set the various flags in the mode field for the file object, if
        // they are reasonable.  There are 4 different invalid combinations
        // that the caller may not specify:
        //
        //     1)  An invalid flag was set in the mode field.  Not all Create/
        //         Open options may be changed.
        //
        //     2)  The caller set one of the synchronous I/O flags (alert or
        //         nonalert), but the file is not opened for synchronous I/O.
        //
        //     3)  The file is opened for synchronous I/O but the caller did
        //         not set either of the synchronous I/O flags (alert or non-
        //         alert).
        //
        //     4)  The caller set both of the synchronous I/O flags (alert and
        //         nonalert).
        //

        if ((modeBuffer->Mode & ~FILE_VALID_SET_FLAGS) ||
            ((modeBuffer->Mode & (FSIO_A | FSIO_NA)) && (!(fileObject->Flags & FO_SYNCHRONOUS_IO))) ||
            ((!(modeBuffer->Mode & (FSIO_A | FSIO_NA))) && (fileObject->Flags & FO_SYNCHRONOUS_IO)) ||
            (((modeBuffer->Mode & FSIO_A) && (modeBuffer->Mode & FSIO_NA) ))) {
            status = STATUS_INVALID_PARAMETER;

        } else {

            //
            // Set or clear the appropriate flags in the file object.
            //

            if (modeBuffer->Mode & FILE_SEQUENTIAL_ONLY) {
                fileObject->Flags |= FO_SEQUENTIAL_ONLY;
            } else {
                fileObject->Flags &= ~FO_SEQUENTIAL_ONLY;
            }

            if (fileObject->Flags & FO_SYNCHRONOUS_IO) {
                if (modeBuffer->Mode & FSIO_A) {
                    fileObject->Flags |= FO_ALERTABLE_IO;
                } else {
                    fileObject->Flags &= ~FO_ALERTABLE_IO;
                }
            }

            status = STATUS_SUCCESS;
        }

        //
        // Complete the I/O operation.
        //

        irp->IoStatus.Status = status;
        irp->IoStatus.Information = 0L;

    } else if (FileInformationClass == FileRenameInformation) {

        PFILE_RENAME_INFORMATION renameBuffer = FileInformation;

        //
        // Check to see whether or not a fully qualified pathname was
        // supplied.  If so, then more processing is required.
        //

        if ((renameBuffer->FileName.Length > 0 &&
            renameBuffer->FileName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR) ||
            renameBuffer->RootDirectory != NULL) {

            //
            // A fully qualified file name was specified as the target of
            // the rename operation.  Attempt to open the target file and
            // ensure that the replacement policy for the file is consistent
            // with the caller's request, and ensure that the file is on the
            // same volume.
            //

            status = IopOpenRenameTarget( &targetHandle,
                                          irp,
                                          renameBuffer,
                                          fileObject );
            if (!NT_SUCCESS( status )) {
                irp->IoStatus.Status = status;

            } else {

                //
                // The fully qualified file name specifies a file on the
                // same volume and if it exists, then the caller specified
                // that it should be replaced.
                //

                status = IoCallDriver( deviceObject, irp );
            }

        } else {

            //
            // This is a simple rename operation, so call the driver and
            // let it perform the rename operation within the same directory
            // as the source file.
            //

            status = IoCallDriver( deviceObject, irp );
        }

    } else if (FileInformationClass == FileCompletionInformation) {

        PFILE_COMPLETION_INFORMATION completion = FileInformation;
        PIO_COMPLETION_CONTEXT context;
        PVOID portObject;

        //
        // It is an error if this file object already has an LPC port associated
        // with it.
        //

        if (fileObject->CompletionContext || fileObject->Flags & FO_SYNCHRONOUS_IO) {

            status = STATUS_INVALID_PARAMETER;

        } else {

            //
            // Attempt to reference the port object by its handle and convert it
            // into a pointer to the port object itself.
            //

            status = ObReferenceObjectByHandle( completion->Port,
                                                &IoCompletionObjectType,
                                                (PVOID *) &portObject );
            if (NT_SUCCESS( status )) {

                //
                // Allocate the memory to be associated w/this file object
                //

                context = ExAllocatePoolWithTag( sizeof( IO_COMPLETION_CONTEXT ),
                                                 'cCoI' );
                if (!context) {

                    ObDereferenceObject( portObject );
                    status = STATUS_INSUFFICIENT_RESOURCES;

                } else {

                    //
                    // Everything was successful.  Capture the completion port
                    // and the key.
                    //

                    context->Port = portObject;
                    context->Key = completion->Key;

                    if (!InterlockedCompareExchangePointer( &fileObject->CompletionContext, context, NULL )) {

                        status = STATUS_SUCCESS;

                    } else {

                        //
                        // Someone set the completion context after the check.
                        // Simply drop everything on the floor and return an
                        // error.
                        //

                        ExFreePool( context );
                        ObDereferenceObject( portObject );
                        status = STATUS_INVALID_PARAMETER;
                    }
                }
            }
        }

        //
        // Complete the I/O operation.
        //

        irp->IoStatus.Status = status;
        irp->IoStatus.Information = 0;

    } else {

        //
        // This is not a request that can be performed here, so invoke the
        // driver at its appropriate dispatch entry with the IRP.
        //

        status = IoCallDriver( deviceObject, irp );
    }

    //
    // If this operation was a synchronous I/O operation, check the return
    // status to determine whether or not to wait on the file object.  If
    // the file object is to be waited on, wait for the operation to complete
    // and obtain the final status from the file object itself.
    //

    if (status == STATUS_PENDING) {

        if (synchronousIo) {

            KeWaitForSingleObject( &fileObject->Event,
                                   Executive,
                                   KernelMode,
                                   FALSE,
                                   (PLARGE_INTEGER) NULL );

            status = fileObject->FinalStatus;

            IopReleaseFileObjectLock( fileObject );

        } else {

            //
            // This is a normal synchronous I/O operation, as opposed to a
            // serialized synchronous I/O operation.  For this case, wait for
            // the local event and copy the final status information back to
            // the caller.
            //

            status = KeWaitForSingleObject( &localEvent,
                                            Executive,
                                            KernelMode,
                                            FALSE,
                                            (PLARGE_INTEGER) NULL );

            status = localIoStatus.Status;

            *IoStatusBlock = localIoStatus;
        }

    } else {

        //
        // The I/O operation finished without return a status of pending.
        // This means that the operation has not been through I/O completion,
        // so it must be done here.
        //

        PKNORMAL_ROUTINE normalRoutine;
        PVOID normalContext;
        KIRQL irql;

        if (!synchronousIo) {

            //
            // This is not a synchronous I/O operation, it is a synchronous
            // I/O API to a file opened for asynchronous I/O.  Since this
            // code path need never wait on the allocated and supplied event,
            // get rid of it so that it doesn't have to be set to the
            // Signaled state by the I/O completion code.
            //

            irp->UserEvent = (PKEVENT) NULL;
        }

        irp->UserIosb = IoStatusBlock;
        KeRaiseIrql( APC_LEVEL, &irql );
        IopCompleteRequest( &irp->Tail.Apc,
                            &normalRoutine,
                            &normalContext,
                            (PVOID *) &fileObject,
                            &normalContext );
        KeLowerIrql( irql );

        if (synchronousIo) {
            IopReleaseFileObjectLock( fileObject );
        }
    }

    //
    // If there was a target handle generated because of a rename operation,
    // close it now.
    //

    if (targetHandle) {
        NtClose( targetHandle );
    }

    return status;
}
