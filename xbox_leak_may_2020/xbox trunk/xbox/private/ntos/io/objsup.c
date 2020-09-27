/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    objsup.c

Abstract:

    This module contains the object support routine for the NT I/O system.

--*/

#include "iop.h"

VOID
IopCloseFile(
    IN PVOID Object,
    IN ULONG SystemHandleCount
    )

/*++

Routine Description:

    This routine is invoked whenever a handle to a file is deleted.  If the
    handle being deleted is the last handle to the file (the ProcessHandleCount
    parameter is one), then all locks for the file owned by the specified
    process must be released.

    Likewise, if the SystemHandleCount is one then this is the last handle
    for this for file object across all processes.  For this case, the file
    system is notified so that it can perform any necessary cleanup on the
    file.

Arguments:

    Object - A pointer to the file object that the handle referenced.

    SystemHandleCount - Count of handles outstanding to the object for the
        entire system.  If the count is one then this is the last handle
        to this file in the system.

Return Value:

    None.

--*/

{
    PIRP irp;
    PIO_STACK_LOCATION irpSp;
    PDEVICE_OBJECT deviceObject;
    NTSTATUS status;
    KEVENT event;
    PFILE_OBJECT fileObject;
    KIRQL irql;

    PAGED_CODE();

    fileObject = (PFILE_OBJECT) Object;

    if (SystemHandleCount == 1) {

        //
        // The last handle to this file object for all of the processes in the
        // system has just been closed, so invoke the driver's "cleanup" handler
        // for this file.  This is the file system's opportunity to remove any
        // share access information for the file, to indicate that if the file
        // is opened for a caching operation and this is the last file object
        // to the file, then it can do whatever it needs with memory management
        // to cleanup any information.
        //
        // Begin by getting the address of the target device object.
        //

        deviceObject = fileObject->DeviceObject;

        //
        // Ensure that the I/O system believes that this file has a handle
        // associated with it in case it doesn't actually get one from the
        // Object Manager.  This is done because sometimes the Object Manager
        // actually creates a handle, but the I/O system never finds out
        // about it so it attempts to send two cleanups for the same file.
        //

        fileObject->Flags |= FO_HANDLE_CREATED;

        //
        // If this file is open for synchronous I/O, wait until this thread
        // owns it exclusively since there may still be a thread using it.
        // This occurs when a system service owns the file because it owns
        // the semaphore, but the I/O completion code has already dereferenced
        // the file object itself.  Without waiting here for the same semaphore
        // there would be a race condition in the service who owns it now. The
        // service needs to be able to access the object w/o it going away after
        // its wait for the file event is satisfied.
        //

        if (fileObject->Flags & FO_SYNCHRONOUS_IO) {
            IopAcquireFileObjectLock(fileObject);
        }

        //
        // Initialize the local event that will be used to synchronize access
        // to the driver completing this I/O operation.
        //

        KeInitializeEvent( &event, SynchronizationEvent, FALSE );

        //
        // Reset the event in the file object.
        //

        KeClearEvent( &fileObject->Event );

        //
        // Allocate and initialize the I/O Request Packet (IRP) for this
        // operation.
        //

        irp = IopAllocateIrpMustSucceed( deviceObject->StackSize );
        irp->Tail.Overlay.OriginalFileObject = fileObject;
        irp->Tail.Overlay.Thread = PsGetCurrentThread();

        //
        // Fill in the service independent parameters in the IRP.
        //

        irp->UserEvent = &event;
        irp->UserIosb = &irp->IoStatus;
        irp->Overlay.AsynchronousParameters.UserApcRoutine = (PIO_APC_ROUTINE) NULL;
        irp->Flags = IRP_SYNCHRONOUS_API | IRP_CLOSE_OPERATION;

        //
        // Get a pointer to the stack location for the first driver.  This will
        // be used to pass the original function codes and parameters.  No
        // function-specific parameters are required for this operation.
        //

        irpSp = IoGetNextIrpStackLocation( irp );
        irpSp->MajorFunction = IRP_MJ_CLEANUP;
        irpSp->FileObject = fileObject;

        //
        // Insert the packet at the head of the IRP list for the thread.
        //

        IopQueueThreadIrp( irp );

        //
        // Invoke the driver at its appropriate dispatch entry with the IRP.
        //

        status = IoCallDriver( deviceObject, irp );

        //
        // If no error was incurred, wait for the I/O operation to complete.
        //

        if (status == STATUS_PENDING) {
            (VOID) KeWaitForSingleObject( &event,
                                          UserRequest,
                                          KernelMode,
                                          FALSE,
                                          (PLARGE_INTEGER) NULL );
        }

        //
        // The following code tears down the IRP by hand since it may not
        // either be possible to it to be completed (because this code was
        // invoked as APC_LEVEL in the first place - or because the reference
        // count on the object cannot be incremented due to this routine
        // being invoked by the delete file procedure below).  Cleanup IRPs
        // therefore use close sematics (the close operation flag is set
        // in the IRP) so that the I/O complete request routine itself sets
        // the event to the Signaled state.
        //

        KeRaiseIrql( APC_LEVEL, &irql );
        IopDequeueThreadIrp( irp );
        KeLowerIrql( irql );

        //
        // Also, free the IRP.
        //

        IoFreeIrp( irp );

        //
        // If this operation was a synchronous I/O operation, release the
        // semaphore so that the file can be used by other threads.
        //

        if (fileObject->Flags & FO_SYNCHRONOUS_IO) {
            IopReleaseFileObjectLock( fileObject );
        }
    }

    return;
}

VOID
IopDeleteFile(
    IN PVOID Object
    )

/*++

Routine Description:

    This routine is invoked when the last handle to a specific file handle is
    being closed and the file object is going away.  It is the responsibility
    of this routine to perform the following functions:

        o  Notify the device driver that the file object is open on that the
           file is being closed.

        o  Dereference the user's error port for the file object, if there
           is one associated with the file object.

        o  Decrement the device object reference count.

Arguments:

    Object - Pointer to the file object being deleted.

Return Value:

    None.

--*/

{
    PIRP irp;
    PIO_STACK_LOCATION irpSp;
    PDEVICE_OBJECT deviceObject;
    IO_STATUS_BLOCK ioStatusBlock;
    KIRQL irql;
    NTSTATUS status;
    PFILE_OBJECT fileObject;
    KEVENT event;

    //
    // Obtain a pointer to the file object.
    //

    fileObject = (PFILE_OBJECT) Object;

    //
    // Get a pointer to the first device driver which should be notified that
    // this file is going away.  If the device driver field is NULL, then this
    // file is being shut down due to an error attempting to get it open in the
    // first place, so do not do any further processing.
    //

    if (fileObject->DeviceObject) {

        deviceObject = fileObject->DeviceObject;

        //
        // If this file has never had a file handle created for it, and yet
        // it exists, invoke the close file procedure so that the file system
        // gets the cleanup IRP it is expecting before sending the close IRP.
        //

        if (!(fileObject->Flags & FO_HANDLE_CREATED)) {
            IopCloseFile( Object, 1 );
        }

        //
        // If this file is open for synchronous I/O, wait until this thread
        // owns it exclusively since there may still be a thread using it.
        // This occurs when a system service owns the file because it owns
        // the semaphore, but the I/O completion code has already dereferenced
        // the file object itself.  Without waiting here for the same semaphore
        // there would be a race condition in the service who owns it now.  The
        // service needs to be able to access the object w/o it going away after
        // its wait for the file event is satisfied.
        //

        if (fileObject->Flags & FO_SYNCHRONOUS_IO) {
            IopAcquireFileObjectLock(fileObject);
        }

        //
        // Reset a local event that can be used to wait for the device driver
        // to close the file.
        //

        KeInitializeEvent( &event, SynchronizationEvent, FALSE );

        //
        // Reset the event in the file object.
        //

        KeClearEvent( &fileObject->Event );

        //
        // Allocate and initialize the I/O Request Packet (IRP) for this
        // operation.
        //

        irp = IopAllocateIrpMustSucceed( deviceObject->StackSize );

        //
        // Get a pointer to the stack location for the first driver.  This is
        // where the function codes and parameters are placed.
        //

        irpSp = IoGetNextIrpStackLocation( irp );

        //
        // Fill in the IRP, indicating that this file object is being deleted.
        //

        irpSp->MajorFunction = IRP_MJ_CLOSE;
        irpSp->FileObject = fileObject;
        irp->UserIosb = &ioStatusBlock;
        irp->UserEvent = &event;
        irp->Tail.Overlay.OriginalFileObject = fileObject;
        irp->Tail.Overlay.Thread = PsGetCurrentThread();
        irp->Flags = IRP_CLOSE_OPERATION | IRP_SYNCHRONOUS_API;

        //
        // Place this packet in the thread's I/O pending queue.
        //

        IopQueueThreadIrp( irp );

        //
        // Give the device driver the packet.  If this request does not work,
        // there is nothing that can be done about it.  This is unfortunate
        // because the driver may have had problems that it was about to
        // report about other operations (e.g., write behind failures, etc.)
        // that it can no longer report.  The reason is that this routine
        // is really initially invoked by NtClose, which has already closed
        // the caller's handle, and that's what the return status from close
        // indicates:  the handle has successfully been closed.
        //

        status = IoCallDriver( deviceObject, irp );

        if (status == STATUS_PENDING) {
            (VOID) KeWaitForSingleObject( &event,
                                          Executive,
                                          KernelMode,
                                          FALSE,
                                          (PLARGE_INTEGER) NULL );
        }

        //
        // Perform any completion operations that need to be performed on
        // the IRP that was used for this request.  This is done here as
        // as opposed to in normal completion code because there is a race
        // condition between when this routine executes if it was invoked
        // from a special kernel APC (e.g., some IRP was just completed and
        // dereferenced this file object for the last time), and when the
        // special kernel APC because of this packet's completion executing.
        //
        // This problem is solved by not having to queue a special kernel
        // APC routine for completion of this packet.  Rather, it is treated
        // much like a synchronous paging I/O operation, except that the
        // packet is not even freed during I/O completion.  This is because
        // the packet is still in this thread's queue, and there is no way
        // to get it out except at APC_LEVEL.  Unfortunately, the part of
        // I/O completion that needs to dequeue the packet is running at
        // DISPATCH_LEVEL.
        //
        // Hence, the packet must be removed from the queue (synchronized,
        // of course), and then it must be freed.
        //

        KeRaiseIrql( APC_LEVEL, &irql );
        IopDequeueThreadIrp( irp );
        KeLowerIrql( irql );

        IoFreeIrp( irp );

        //
        // If there was an completion port associated w/this file object, dereference
        // it now, and deallocate the completion context pool.
        //

        if (fileObject->CompletionContext) {
            ObDereferenceObject( fileObject->CompletionContext->Port );
            ExFreePool( fileObject->CompletionContext );
        }

        //
        // Decrement the reference count on the device object.  Note that
        // if the driver has been marked for an unload operation, and the
        // reference count goes to zero, then the driver may need to be
        // unloaded at this point.
        //

        IopDecrementDeviceObjectRef( deviceObject );
    }
}
