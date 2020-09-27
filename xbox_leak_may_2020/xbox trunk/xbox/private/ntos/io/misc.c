/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    misc.c

Abstract:

    This module contains the code to implement the NtFlushBuffersFile,
    NtSetNewSizeFile, and NtCancelIoFile system services for the NT I/O system.

--*/

#include "iop.h"

//
// Local copies of the Win32 types that are defined in terms of the NT types.
// Needed for NtUserIoApcDispatcher.
//

typedef struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    ULONG   Offset;
    ULONG   OffsetHigh;
    HANDLE  hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef
VOID
(NTAPI *LPOVERLAPPED_COMPLETION_ROUTINE)(
    ULONG dwErrorCode,
    ULONG dwNumberOfBytesTransfered,
    LPOVERLAPPED lpOverlapped
    );

NTSTATUS
NtDeleteFile(
    IN POBJECT_ATTRIBUTES ObjectAttributes
    )

/*++

Routine Description:

    This service deletes the specified file.

Arguments:

    ObjectAttributes - Supplies the attributes to be used for file object (name,
        SECURITY_DESCRIPTOR, etc.)

Return Value:

    The status returned is the final completion status of the operation.

--*/

{
    NTSTATUS status;
    OPEN_PACKET openPacket;
    DUMMY_FILE_OBJECT localFileObject;
    HANDLE handle;

    PAGED_CODE();

    //
    // Build a parse open packet that tells the parse method to open the file
    // for open for delete access w/the delete bit set, and then close it.
    //

    RtlZeroMemory( &openPacket, sizeof( OPEN_PACKET ) );

    openPacket.Type = IO_TYPE_OPEN_PACKET;
    openPacket.Size = sizeof( OPEN_PACKET );
    openPacket.CreateOptions = FILE_DELETE_ON_CLOSE;
    openPacket.ShareAccess = (USHORT) FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    openPacket.Disposition = FILE_OPEN;
    openPacket.DeleteOnly = TRUE;
    openPacket.LocalFileObject = &localFileObject;
    openPacket.DesiredAccess = DELETE;

    //
    // Open the object by its name.  Because of the special DeleteOnly flag
    // set in the open packet, the parse routine will open the file, and
    // then realize that it is only deleting the file, and will therefore
    // immediately dereference the file.  This will cause the cleanup and
    // the close to be sent to the file system, thus causing the file to
    // be deleted.
    //

    status = ObOpenObjectByName( ObjectAttributes,
                                 &IoFileObjectType,
                                 &openPacket,
                                 &handle );

    //
    // The operation is successful if the parse check field of the open packet
    // indicates that the parse routine was actually invoked, and the final
    // status field of the packet is set to success.
    //

    if (openPacket.ParseCheck != OPEN_PACKET_PATTERN) {
        return status;
    } else {
        return openPacket.FinalStatus;
    }
}

NTSTATUS
NtFlushBuffersFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    )

/*++

Routine Description:

    This service causes all buffered data to the file to be written.

Arguments:

    FileHandle - Supplies a handle to the file whose buffers should be flushed.

    IoStatusBlock - Address of the caller's I/O status block.

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
        // An exception was incurred while attempting to allocate the IRP.
        // Cleanup and return an appropriate error status code.
        //

        return IopAllocateIrpCleanup( fileObject, (PKEVENT) NULL );
    }
    irp->Tail.Overlay.OriginalFileObject = fileObject;
    irp->Tail.Overlay.Thread = PsGetCurrentThread();

    //
    // Fill in the service independent parameters in the IRP.
    //

    if (synchronousIo) {
        irp->UserIosb = IoStatusBlock;
    } else {
        irp->UserEvent = &localEvent;
        irp->UserIosb = &localIoStatus;
        irp->Flags = IRP_SYNCHRONOUS_API;
    }

    //
    // Get a pointer to the stack location for the first driver.  This is used
    // to pass the original function codes and parameters.
    //

    irpSp = IoGetNextIrpStackLocation( irp );
    irpSp->MajorFunction = IRP_MJ_FLUSH_BUFFERS;
    irpSp->FileObject = fileObject;

    //
    // Queue the packet, call the driver, and synchronize appopriately with
    // I/O completion.
    //

    status = IopSynchronousServiceTail( deviceObject,
                                        irp,
                                        fileObject,
                                        FALSE,
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

NTSTATUS
NtQueryFullAttributesFile(
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PFILE_NETWORK_OPEN_INFORMATION FileInformation
    )

/*++

Routine Description:

    This service queries the network attributes information for a specified
    file.

Arguments:

    ObjectAttributes - Supplies the attributes to be used for file object (name,
        SECURITY_DESCRIPTOR, etc.)

    FileInformation - Supplies an output buffer to receive the returned file
        attributes information.

Return Value:

    The status returned is the final completion status of the operation.

--*/

{
    NTSTATUS status;
    OPEN_PACKET openPacket;
    DUMMY_FILE_OBJECT localFileObject;
    HANDLE handle;

    PAGED_CODE();

    //
    // Build a parse open packet that tells the parse method to open the file,
    // query the file's full attributes, and close the file.
    //

    RtlZeroMemory( &openPacket, sizeof( OPEN_PACKET ) );

    openPacket.Type = IO_TYPE_OPEN_PACKET;
    openPacket.Size = sizeof( OPEN_PACKET );
    openPacket.ShareAccess = (USHORT) FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    openPacket.Disposition = FILE_OPEN;
    openPacket.QueryOnly = TRUE;
    openPacket.LocalFileObject = &localFileObject;
    openPacket.NetworkInformation = FileInformation;
    openPacket.DesiredAccess = FILE_READ_ATTRIBUTES;

    //
    // Open the object by its name.  Because of the special QueryOnly flag set
    // in the open packet, the parse routine will open the file, and then
    // realize that it is only performing a query.  It will therefore perform
    // the query, and immediately close the file.
    //

    status = ObOpenObjectByName( ObjectAttributes,
                                 &IoFileObjectType,
                                 &openPacket,
                                 &handle );

    //
    // The operation is successful if the parse check field of the open packet
    // indicates that the parse routine was actually invoked, and the final
    // status field of the packet is set to success.
    //

    if (openPacket.ParseCheck != OPEN_PACKET_PATTERN) {
        return status;
    } else {
        status = openPacket.FinalStatus;
    }

    return status;
}

VOID
NtUserIoApcDispatcher(
    IN PVOID ApcContext,
    IN PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG Reserved
    )
/*++

Routine Description:

    This procedure is called to complete ReadFileEx and WriteFileEx
    asynchronous I/O. Its primary function is to extract the
    appropriate information from the passed IoStatusBlock and call the
    users completion routine.

    The users completion routine is called as:

        Routine Description:

            When an outstanding I/O completes with a callback, this
            function is called.  This function is only called while the
            thread is in an alertable wait (SleepEx,
            WaitForSingleObjectEx, or WaitForMultipleObjectsEx with the
            bAlertable flag set to TRUE).  Returning from this function
            allows another pendiong I/O completion callback to be
            processed.  If this is the case, this callback is entered
            before the termination of the thread's wait with a return
            code of WAIT_IO_COMPLETION.

            Note that each time your completion routine is called, the
            system uses some of your stack.  If you code your completion
            logic to do additional ReadFileEx's and WriteFileEx's within
            your completion routine, AND you do alertable waits in your
            completion routine, you may grow your stack without ever
            trimming it back.

        Arguments:

            dwErrorCode - Supplies the I/O completion status for the
                related I/O.  A value of 0 indicates that the I/O was
                successful.  Note that end of file is indicated by a
                non-zero dwErrorCode value of ERROR_HANDLE_EOF.

            dwNumberOfBytesTransfered - Supplies the number of bytes
                transfered during the associated I/O.  If an error
                occured, a value of 0 is supplied.

            lpOverlapped - Supplies the address of the OVERLAPPED
                structure used to initiate the associated I/O.  The
                hEvent field of this structure is not used by the system
                and may be used by the application to provide additional
                I/O context.  Once a completion routine is called, the
                system will not use the OVERLAPPED structure.  The
                completion routine is free to deallocate the overlapped
                structure.

Arguments:

    ApcContext - Supplies the users completion routine. The format of
        this routine is an LPOVERLAPPED_COMPLETION_ROUTINE.

    IoStatusBlock - Supplies the address of the IoStatusBlock that
        contains the I/O completion status. The IoStatusBlock is
        contained within the OVERLAPPED structure.

    Reserved - Not used; reserved for future use.

Return Value:

    None.

--*/
{
    LPOVERLAPPED_COMPLETION_ROUTINE CompletionRoutine;
    ULONG dwErrorCode;
    ULONG dwNumberOfBytesTransfered;
    LPOVERLAPPED lpOverlapped;

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    dwErrorCode = 0;

    if ( NT_ERROR(IoStatusBlock->Status) ) {
        dwErrorCode = RtlNtStatusToDosError(IoStatusBlock->Status);
        dwNumberOfBytesTransfered = 0;
        }
    else {
        dwErrorCode = 0;
        dwNumberOfBytesTransfered = (ULONG)IoStatusBlock->Information;
        }

    CompletionRoutine = (LPOVERLAPPED_COMPLETION_ROUTINE)(ULONG_PTR)ApcContext;
    lpOverlapped = (LPOVERLAPPED)CONTAINING_RECORD(IoStatusBlock,OVERLAPPED,Internal);

    (CompletionRoutine)(dwErrorCode,dwNumberOfBytesTransfered,lpOverlapped);

    UNREFERENCED_PARAMETER( Reserved );
}
