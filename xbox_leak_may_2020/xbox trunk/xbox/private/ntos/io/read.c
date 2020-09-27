/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    read.c

Abstract:

    This module contains the code to implement the NtReadFile system service.

--*/

#include "iop.h"

NTSTATUS
NtReadFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL
    )

/*++

Routine Description:

    This service reads Length bytes of data from the file associated with
    FileHandle starting at ByteOffset and puts the data into the caller's
    Buffer.  If the end of the file is reached before Length bytes have
    been read, then the operation will terminate.  The actual length of
    the data read from the file will be returned in the second longword
    of the IoStatusBlock.

Arguments:

    FileHandle - Supplies a handle to the file to be read.

    Event - Optionally supplies an event to be signaled when the read operation
        is complete.

    ApcRoutine - Optionally supplies an APC routine to be executed when the read
        operation is complete.

    ApcContext - Supplies a context parameter to be passed to the ApcRoutine, if
        an ApcRoutine was specified.

    IoStatusBlock - Address of the caller's I/O status block.

    Buffer - Address of buffer to receive the data read from the file.

    Length - Supplies the length, in bytes, of the data to read from the file.

    ByteOffset - Optionally specifies the starting byte offset within the file
        to begin the read operation.  If not specified and the file is open
        for synchronous I/O, then the current file position is used.  If the
        file is not opened for synchronous I/O and the parameter is not
        specified, then it is an error.

Return Value:

    The status returned is success if the read operation was properly queued
    to the I/O system.  Once the read completes the status of the operation
    can be determined by examining the Status field of the I/O status block.

--*/

{
    PIRP irp;
    NTSTATUS status;
    PFILE_OBJECT fileObject;
    PDEVICE_OBJECT deviceObject;
    PIO_STACK_LOCATION irpSp;
    BOOLEAN synchronousIo;
    PKEVENT eventObject = (PKEVENT) NULL;
    LARGE_INTEGER fileOffset = {0,0};

    PAGED_CODE();

    //
    // Reference the file object so the target device can be found.  Note
    // that if the caller does not have read access to the file, the operation
    // will fail.
    //

    status = ObReferenceObjectByHandle( FileHandle,
                                        &IoFileObjectType,
                                        (PVOID *) &fileObject );
    if (!NT_SUCCESS( status )) {
        return status;
    }

    if (!fileObject->ReadAccess) {
        ObDereferenceObject( fileObject );
        return STATUS_ACCESS_DENIED;
    }

    //
    // Get the address of the target device object.
    //

    deviceObject = fileObject->DeviceObject;

    //
    // If this file has an I/O completion port associated w/it, then
    // ensure that the caller did not supply an APC routine, as the
    // two are mutually exclusive methods for I/O completion
    // notification.
    //

    if (fileObject->CompletionContext && IopApcRoutinePresent( ApcRoutine )) {
        ObDereferenceObject( fileObject );
        return STATUS_INVALID_PARAMETER;
    }

    if (ARGUMENT_PRESENT( ByteOffset )) {
        fileOffset = *ByteOffset;
    }

    //
    // Get the address of the event object and set the event to the Not-
    // Signaled state, if an one was specified.  Note here too, that if
    // the handle does not refer to an event, then the reference will fail.
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

        if (!ARGUMENT_PRESENT( ByteOffset ) ||
            (fileOffset.LowPart == FILE_USE_FILE_POINTER_POSITION &&
            fileOffset.HighPart == -1)) {
            fileOffset = fileObject->CurrentByteOffset;
        }

        synchronousIo = TRUE;

    } else if (!ARGUMENT_PRESENT( ByteOffset )) {

        //
        // The file is not open for synchronous I/O operations, but the
        // caller did not specify a ByteOffset parameter.
        //

        if (eventObject) {
            ObDereferenceObject( eventObject );
        }
        ObDereferenceObject( fileObject );
        return STATUS_INVALID_PARAMETER;
    } else {
        synchronousIo = FALSE;
    }

    //
    //  Negative file offsets are illegal.
    //

    if (fileOffset.HighPart < 0) {
        if (eventObject) {
            ObDereferenceObject( eventObject );
        }
        if (synchronousIo) {
            IopReleaseFileObjectLock( fileObject );
        }
        ObDereferenceObject( fileObject );
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Set the file object to the Not-Signaled state.
    //

    KeClearEvent( &fileObject->Event );

    //
    // Allocate and initialize the I/O Request Packet (IRP) for this operation.
    //

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

    irp->UserBuffer = Buffer;
    irp->UserEvent = eventObject;
    irp->UserIosb = IoStatusBlock;
    irp->Overlay.AsynchronousParameters.UserApcRoutine = ApcRoutine;
    irp->Overlay.AsynchronousParameters.UserApcContext = ApcContext;

    //
    // Get a pointer to the stack location for the first driver.  This will be
    // used to pass the original function codes and parameters.
    //

    irpSp = IoGetNextIrpStackLocation( irp );
    irpSp->MajorFunction = IRP_MJ_READ;
    irpSp->FileObject = fileObject;

    //
    // Now determine whether or not we should automatically lock the user's
    // buffer for direct I/O.
    //

    if ((deviceObject->Flags & DO_DIRECT_IO) && (Length != 0)) {
        IoLockUserBuffer(irp, Length);
    }

    //
    // If this read operation is supposed to be performed with caching disabled
    // set the disable flag in the IRP so no caching is performed.
    //

    if (fileObject->Flags & FO_NO_INTERMEDIATE_BUFFERING) {
        irp->Flags |= IRP_NOCACHE | IRP_READ_OPERATION | IRP_DEFER_IO_COMPLETION;
    } else {
        irp->Flags |= IRP_READ_OPERATION | IRP_DEFER_IO_COMPLETION;
    }

    //
    // Copy the caller's parameters to the service-specific portion of the
    // IRP.
    //

    irpSp->Parameters.Read.Length = Length;
    irpSp->Parameters.Read.BufferOffset = 0;
    irpSp->Parameters.Read.ByteOffset = fileOffset;

    //
    // Queue the packet, call the driver, and synchronize appopriately with
    // I/O completion.
    //

    status =  IopSynchronousServiceTail( deviceObject,
                                         irp,
                                         fileObject,
                                         TRUE,
                                         synchronousIo );

    return status;
}

NTSTATUS
NtReadFileScatter(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PFILE_SEGMENT_ELEMENT SegmentArray,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL
    )

/*++

Routine Description:

    This service reads Length bytes of data from the file associated with
    FileHandle starting at ByteOffset and puts the data into the caller's
    buffer segments.  The buffer segments are not virtually contiguous,
    but are 8 KB in length and alignment. If the end of the file is reached
    before Length bytes have been read, then the operation will terminate.
    The actual length of the data read from the file will be returned in
    the second longword of the IoStatusBlock.

Arguments:

    FileHandle - Supplies a handle to the file to be read.

    Event - Optionally supplies an event to be signaled when the read operation
        is complete.

    ApcRoutine - Optionally supplies an APC routine to be executed when the read
        operation is complete.

    ApcContext - Supplies a context parameter to be passed to the ApcRoutine, if
        an ApcRoutine was specified.

    IoStatusBlock - Address of the caller's I/O status block.

    SegmentArray - An array of buffer segment pointers that specify
        where the data should be placed.

    Length - Supplies the length, in bytes, of the data to read from the file.

    ByteOffset - Optionally specifies the starting byte offset within the file
        to begin the read operation.  If not specified and the file is open
        for synchronous I/O, then the current file position is used.  If the
        file is not opened for synchronous I/O and the parameter is not
        specified, then it is an error.

Return Value:

    The status returned is success if the read operation was properly queued
    to the I/O system.  Once the read completes the status of the operation
    can be determined by examining the Status field of the I/O status block.

Notes:

    This interface is only supported for no buffering and asynchronous I/O.

--*/

{
    PIRP irp;
    NTSTATUS status;
    PFILE_OBJECT fileObject;
    PDEVICE_OBJECT deviceObject;
    PIO_STACK_LOCATION irpSp;
    PKEVENT eventObject = (PKEVENT) NULL;
    ULONG elementCount;
    LARGE_INTEGER fileOffset = {0,0};
    BOOLEAN synchronousIo;

    PAGED_CODE();

    //
    // Reference the file object so the target device can be found.  Note
    // that if the caller does not have read access to the file, the operation
    // will fail.
    //

    status = ObReferenceObjectByHandle( FileHandle,
                                        &IoFileObjectType,
                                        (PVOID *) &fileObject );
    if (!NT_SUCCESS( status )) {
        return status;
    }

    if (!fileObject->ReadAccess) {
        ObDereferenceObject( fileObject );
        return STATUS_ACCESS_DENIED;
    }

    //
    // Get the address of the target device object.
    //

    deviceObject = fileObject->DeviceObject;

    //
    // Verify this is a valid scatter read request.  In particular it must be
    // non-cached and and directed at a file system device.
    //

    if (!(fileObject->Flags & FO_NO_INTERMEDIATE_BUFFERING) ||
        (deviceObject->DeviceType != FILE_DEVICE_DISK_FILE_SYSTEM &&
         deviceObject->DeviceType != FILE_DEVICE_CD_ROM_FILE_SYSTEM)) {

        ObDereferenceObject( fileObject );
        return STATUS_INVALID_PARAMETER;
    }

    elementCount = BYTES_TO_PAGES(Length);

    //
    // If this file has an I/O completion port associated w/it, then
    // ensure that the caller did not supply an APC routine, as the
    // two are mutually exclusive methods for I/O completion
    // notification.
    //

    if (fileObject->CompletionContext && IopApcRoutinePresent( ApcRoutine )) {
        ObDereferenceObject( fileObject );
        return STATUS_INVALID_PARAMETER;
    }

    if (ARGUMENT_PRESENT( ByteOffset )) {
        fileOffset = *ByteOffset;
    }

    //
    // Get the address of the event object and set the event to the Not-
    // Signaled state, if an one was specified.  Note here too, that if
    // the handle does not refer to an event, then the reference will fail.
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

        if (!ARGUMENT_PRESENT( ByteOffset ) ||
            (fileOffset.LowPart == FILE_USE_FILE_POINTER_POSITION &&
            fileOffset.HighPart == -1)) {
            fileOffset = fileObject->CurrentByteOffset;
        }

        synchronousIo = TRUE;

    } else if (!ARGUMENT_PRESENT( ByteOffset )) {

        //
        // The file is not open for synchronous I/O operations, but the
        // caller did not specify a ByteOffset parameter.
        //

        if (eventObject) {
            ObDereferenceObject( eventObject );
        }
        ObDereferenceObject( fileObject );
        return STATUS_INVALID_PARAMETER;
    } else {
        synchronousIo = FALSE;
    }

    //
    //  Negative file offsets are illegal.
    //

    if (fileOffset.HighPart < 0) {
        if (eventObject) {
            ObDereferenceObject( eventObject );
        }
        if (synchronousIo) {
            IopReleaseFileObjectLock( fileObject );
        }
        ObDereferenceObject( fileObject );
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Set the file object to the Not-Signaled state.
    //

    KeClearEvent( &fileObject->Event );

    //
    // Allocate and initialize the I/O Request Packet (IRP) for this operation.
    //

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
    irp->Flags |= IRP_NOCACHE | IRP_READ_OPERATION | IRP_DEFER_IO_COMPLETION |
        IRP_SCATTER_GATHER_OPERATION;

    //
    // Get a pointer to the stack location for the first driver.  This will be
    // used to pass the original function codes and parameters.
    //

    irpSp = IoGetNextIrpStackLocation( irp );
    irpSp->MajorFunction = IRP_MJ_READ;
    irpSp->FileObject = fileObject;

    //
    // This is a direct I/O operation.  Lock down and clone the pages in the
    // segment array.  If the target device doesn't support scatter/gather I/O,
    // then also map a virtual buffer that describes the pages.
    //

    if (Length != 0) {

        status = MmLockSelectedIoPages(SegmentArray, Length, irp,
            (BOOLEAN)((deviceObject->Flags & DO_SCATTER_GATHER_IO) == 0));

        if (!NT_SUCCESS(status)) {
            IopExceptionCleanup(fileObject, irp, eventObject);
            return status;
        }
    }

    //
    // Copy the caller's parameters to the service-specific portion of the
    // IRP.
    //

    irpSp->Parameters.Read.Length = Length;
    irpSp->Parameters.Read.BufferOffset = 0;
    irpSp->Parameters.Read.ByteOffset = fileOffset;

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
