/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    internal.c

Abstract:

    This module contains the internal subroutines used by the I/O system.

--*/

#include "iop.h"

VOID
IopUserRundown(
    IN PKAPC Apc
    );

NTSTATUS
IopAllocateIrpCleanup(
    IN PFILE_OBJECT FileObject,
    IN PKEVENT EventObject OPTIONAL
    )

/*++

Routine Description:

    This routine is invoked internally by those system services that attempt
    to allocate an IRP and fail.  This routine cleans up the file object
    and any event object that has been references and releases any locks
    that were taken out.

Arguments:

    FileObject - Pointer to the file object being worked on.

    EventObject - Optional pointer to a referenced event to be dereferenced.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES.

--*/

{
    PAGED_CODE();

    //
    // Begin by dereferencing the event, if one was specified.
    //

    if (ARGUMENT_PRESENT( EventObject )) {
        ObDereferenceObject( EventObject );
    }

    //
    // Release the synchronization semaphore if it is currently held and
    // dereference the file object.
    //

    if (FileObject->Flags & FO_SYNCHRONOUS_IO) {
        IopReleaseFileObjectLock( FileObject );
    }

    ObDereferenceObject( FileObject );

    return STATUS_INSUFFICIENT_RESOURCES;
}

PIRP
IopAllocateIrpMustSucceed(
    IN CCHAR StackSize
    )

/*++

Routine Description:

    This routine is invoked to allocate an IRP when there are no appropriate
    packets remaining on the look-aside list, and no memory was available
    from the general non-paged pool, and yet, the code path requiring the
    packet has no way of backing out and simply returning an error.  There-
    fore, it must allocate an IRP.  Hence, this routine is called to allocate
    that packet.

Arguments:

    StackSize - Supplies the number of IRP I/O stack locations that the
        packet must have when allocated.

Return Value:

    A pointer to the allocated I/O Request Packet.

--*/

{
    PIRP irp;
    LARGE_INTEGER interval;

    //
    // Keeping attemting to allocate the IRP normally.
    //

    irp = IoAllocateIrp(StackSize);
    while (!irp) {
        interval.QuadPart = -1000 * 1000 * 50; // 50 Msec.
        KeDelayExecutionThread(KernelMode, FALSE, &interval);
        irp = IoAllocateIrp(StackSize);
    }

    return irp;
}

VOID
IopCompleteUnloadOrDelete(
    IN PDEVICE_OBJECT DeviceObject,
    IN KIRQL Irql
    )

/*++

Routine Description:

    This routine is invoked when the reference count on a device object
    transitions to a zero and the driver is mark for unload or device has
    been marked for delete. This means that it may be possible to actually
    unload the driver or delete the device object.  If all
    of the devices have a reference count of zero, then the driver is
    actually unloaded.  Note that in order to ensure that this routine is
    not invoked twice, at the same time, on two different processors, the
    I/O database spin lock is still held at this point.

Arguments:

    DeviceObject - Supplies a pointer to one of the driver's device objects,
        namely the one whose reference count just went to zero.

    Irql - Specifies the IRQL of the processor at the time that the I/O
        database lock was acquired.

Return Value:

    None.

--*/

{
    if (DeviceObject->DeletePending) {

        KeLowerIrql(Irql);

        //
        // Remove this device object from the driver object's list.
        //

        if (DeviceObject->DriverObject->DriverDeleteDevice != NULL) {

            DeviceObject->DriverObject->DriverDeleteDevice( DeviceObject );

        } else {

            //
            // Finally, dereference the object so it is deleted.
            //

            ObDereferenceObject( DeviceObject );
        }
    }
}

VOID
IopCompleteRequest(
    IN PKAPC Apc,
    IN PKNORMAL_ROUTINE *NormalRoutine,
    IN PVOID *NormalContext,
    IN PVOID *SystemArgument1,
    IN PVOID *SystemArgument2
    )

/*++

Routine Description:

    This routine executes as a special kernel APC routine in the context of
    the thread which originally requested the I/O operation which is now
    being completed.

    This routine performs the following tasks:

        o   A check is made to determine whether the specified request ended
            with an error status.  If so, and the error code qualifies as one
            which should be reported to an error port, then an error port is
            looked for in the thread/process.   If one exists, then this routine
            will attempt to set up an LPC to it.  Otherwise, it will attempt to
            set up an LPC to the system error port.

        o   Copy I/O status.

        o   Set event, if any and dereference if appropriate.

        o   Dequeue the IRP from the thread queue as pending I/O request.

        o   Queue APC to thread, if any.

        o   If no APC is to be queued, then free the packet now.


Arguments:

    Apc - Supplies a pointer to kernel APC structure.

    NormalRoutine - Supplies a pointer to a pointer to the normal function
        that was specified when the APC was initialied.

    NormalContext - Supplies a pointer to a pointer to an arbitrary data
        structure that was specified when the APC was initialized.

    SystemArgument1 - Supplies a pointer to an argument that contains the
        address of the original file object for this I/O operation.

    SystemArgument2 - Supplies a pointer to an argument that contains an
        argument that is used by this routine only in the case of STATUS_REPARSE.

Return Value:

    None.

--*/
{
#define SynchronousIo( Irp, FileObject ) (  \
    (Irp->Flags & IRP_SYNCHRONOUS_API) ||   \
    (FileObject == NULL ? 0 : FileObject->Flags & FO_SYNCHRONOUS_IO) )

    PIRP irp;
    PETHREAD thread;
    PFILE_OBJECT fileObject;
    NTSTATUS    status;

    UNREFERENCED_PARAMETER( NormalRoutine );
    UNREFERENCED_PARAMETER( NormalContext );

    //
    // Begin by getting the address of the I/O Request Packet.  Also, get
    // the address of the current thread and the address of the original file
    // object for this I/O operation.
    //

    irp = CONTAINING_RECORD( Apc, IRP, Tail.Apc );
    thread = PsGetCurrentThread();
    fileObject = (PFILE_OBJECT) *SystemArgument1;

    //
    // Ensure that the packet is not being completed with a minus one.  This
    // is apparently a common problem in some drivers, and has no meaning
    // as a status code.
    //

    ASSERT( irp->IoStatus.Status != 0xffffffff );

    //
    // Check to see whether or not the I/O operation actually completed.  If
    // it did, then proceed normally.  Otherwise, cleanup everything and get
    // out of here.
    //

    if (!NT_ERROR( irp->IoStatus.Status ) ||
        (NT_ERROR( irp->IoStatus.Status ) &&
        irp->PendingReturned &&
        !SynchronousIo( irp, fileObject ))) {

        PVOID port = NULL;
        PVOID key;
        BOOLEAN createOperation = FALSE;

        //
        // If there is an I/O completion port object associated w/this request,
        // save it here so that the file object can be dereferenced.
        //

        if (fileObject && fileObject->CompletionContext) {
            port = fileObject->CompletionContext->Port;
            key = fileObject->CompletionContext->Key;
        }

        //
        // Copy the I/O status from the IRP into the caller's I/O status
        // block.
        //
        //
        // Since HasOverlappedIoCompleted and GetOverlappedResult only
        // look at the Status field of the UserIosb to determine if the
        // IRP has completed, the Information field must be written
        // before the Status field.
        //

        irp->UserIosb->Information = irp->IoStatus.Information;
        irp->UserIosb->Status = irp->IoStatus.Status;

        //
        // Determine whether the caller supplied an event that needs to be set
        // to the Signaled state.  If so, then set it; otherwise, set the event
        // in the file object to the Signaled state.
        //
        // It is possible for the event to have been specified as a PKEVENT if
        // this was an I/O operation hand-built for an FSP or an FSD, or
        // some other types of operations such as synchronous I/O APIs.  In
        // any of these cases, the event was not referenced since it is not an
        // object manager event, so it should not be dereferenced.
        //
        // Also, it is possible for there not to be a file object for this IRP.
        // This occurs when an FSP is doing I/O operations to a device driver on
        // behalf of a process doing I/O to a file.  The file object cannot be
        // dereferenced if this is the case.  If this operation was a create
        // operation then the object should not be dereferenced either.  This
        // is because the reference count must be one or it will go away for
        // the caller (not much point in making an object that just got created
        // go away).
        //

        if (irp->UserEvent) {
            (VOID) KeSetEvent( irp->UserEvent, 0, FALSE );
            if (fileObject) {
                if (!(irp->Flags & IRP_SYNCHRONOUS_API)) {
                    ObDereferenceObject( irp->UserEvent );
                }
                if (fileObject->Flags & FO_SYNCHRONOUS_IO && !(irp->Flags & IRP_OB_QUERY_NAME)) {
                    (VOID) KeSetEvent( &fileObject->Event, 0, FALSE );
                    fileObject->FinalStatus = irp->IoStatus.Status;
                }
                if (irp->Flags & IRP_CREATE_OPERATION) {
                    createOperation = TRUE;
                    irp->Overlay.AsynchronousParameters.UserApcRoutine = (PIO_APC_ROUTINE) NULL;
                }
            }
        } else if (fileObject) {
            (VOID) KeSetEvent( &fileObject->Event, 0, FALSE );
            fileObject->FinalStatus = irp->IoStatus.Status;
            if (irp->Flags & IRP_CREATE_OPERATION) {
                createOperation = TRUE;
                irp->Overlay.AsynchronousParameters.UserApcRoutine = (PIO_APC_ROUTINE) NULL;
            }
        }

        //
        // Dequeue the packet from the thread's pending I/O request list.
        //

        IopDequeueThreadIrp( irp );

        //
        // If the caller requested an APC, queue it to the thread.  If not, then
        // simply free the packet now.
        //

        if (irp->Overlay.AsynchronousParameters.UserApcRoutine) {
            KeInitializeApc( &irp->Tail.Apc,
                             &thread->Tcb,
                             IopUserCompletion,
                             (PKRUNDOWN_ROUTINE) IopUserRundown,
                             (PKNORMAL_ROUTINE) irp->Overlay.AsynchronousParameters.UserApcRoutine,
                             IopApcRoutineMode(irp->Overlay.AsynchronousParameters.UserApcRoutine),
                             irp->Overlay.AsynchronousParameters.UserApcContext );

            KeInsertQueueApc( &irp->Tail.Apc,
                              irp->UserIosb,
                              NULL,
                              2 );

        } else if (port && irp->Overlay.AsynchronousParameters.UserApcContext) {

            //
            // If there is a completion context associated w/this I/O operation,
            // send the message to the port. Tag completion packet as an Irp.
            //

            irp->Tail.CompletionKey = key;
            irp->Tail.Overlay.PacketType = IopCompletionPacketIrp;

            KeInsertQueue( (PKQUEUE) port,
                           &irp->Tail.Overlay.ListEntry );

        } else {

            //
            // Free the IRP now since it is no longer needed.
            //

            IoFreeIrp( irp );
        }

        if (fileObject && !createOperation) {

            //
            // Dereference the file object now.
            //

            ObDereferenceObject( fileObject );
        }

    } else {

        if (irp->PendingReturned && fileObject) {

            //
            // This is an I/O operation that completed as an error for
            // which a pending status was returned and the I/O operation
            // is synchronous.  For this case, the I/O system is waiting
            // on behalf of the caller.  If the reason that the I/O was
            // synchronous is that the file object was opened for synchronous
            // I/O, then the event associated with the file object is set
            // to the signaled state.  If the I/O operation was synchronous
            // because this is a synchronous API, then the event is set to
            // the signaled state.
            //
            // Note also that the status must be returned for both types
            // of synchronous I/O.  If this is a synchronous API, then the
            // I/O system supplies its own status block so it can simply
            // be written;  otherwise, the I/O system will obtain the final
            // status from the file object itself.
            //

            if (irp->Flags & IRP_SYNCHRONOUS_API) {
                *irp->UserIosb = irp->IoStatus;
                if (irp->UserEvent) {
                    (VOID) KeSetEvent( irp->UserEvent, 0, FALSE );
                } else {
                    (VOID) KeSetEvent( &fileObject->Event, 0, FALSE );
                }
            } else {
                fileObject->FinalStatus = irp->IoStatus.Status;
                (VOID) KeSetEvent( &fileObject->Event, 0, FALSE );
            }
        }

        //
        // The operation was incomplete.  Perform the general cleanup.  Note
        // that everything is basically dropped on the floor without doing
        // anything.  That is:
        //
        //     IoStatusBlock - Do nothing.
        //     Event - Dereference without setting to Signaled state.
        //     FileObject - Dereference without setting to Signaled state.
        //     ApcRoutine - Do nothing.
        //

        if (fileObject) {
            if (!(irp->Flags & IRP_CREATE_OPERATION)) {
                ObDereferenceObject( fileObject );
            }
        }

        if (irp->UserEvent &&
            fileObject &&
            !(irp->Flags & IRP_SYNCHRONOUS_API)) {
            ObDereferenceObject( irp->UserEvent );
        }

        IopDequeueThreadIrp( irp );
        IoFreeIrp( irp );
    }
}

VOID
IopDisassociateThreadIrp(
    VOID
    )

/*++

Routine Description:

    This routine is invoked when the I/O requests for a thread are being
    cancelled, but there is a packet at the end of the thread's queue that
    has not been completed for such a long period of time that it has timed
    out.  It is this routine's responsibility to try to disassociate that
    IRP with this thread.

Arguments:

    None.

Return Value:

    None.

--*/

{
    KIRQL irql;
    KIRQL spIrql;
    PIRP irp;
    PETHREAD thread;
    PLIST_ENTRY entry;
    PIO_STACK_LOCATION irpSp;
    PDEVICE_OBJECT deviceObject;

    //
    // Begin by ensuring that the packet has not already been removed from
    // the thread's queue.
    //

    KeRaiseIrql( APC_LEVEL, &irql );

    thread = PsGetCurrentThread();

    //
    // If there are no packets on the IRP list, then simply return now.
    // All of the packets have been fully completed, so the caller will also
    // simply return to its caller.
    //

    if (IsListEmpty( &thread->IrpList )) {
        KeLowerIrql( irql );
        return;
    }

    //
    // Get a pointer to the first packet on the queue, and begin examining
    // it.  Note that because the processor is at raised IRQL, and because
    // the packet can only be removed in the context of the currently
    // executing thread, that it is not possible for the packet to be removed
    // from the list.  On the other hand, it IS possible for the packet to
    // be queued to the thread's APC list at this point, and this must be
    // blocked/synchronized in order to examine the request.
    //
    // Begin, therefore, by acquiring the I/O completion spinlock, so that
    // the packet can be safely examined.
    //

    spIrql = IopAcquireCompletionLock();

    //
    // Check to see whether or not the packet has been completed (that is,
    // queued to the current thread).  If not, change threads.
    //

    entry = thread->IrpList.Flink;
    irp = CONTAINING_RECORD( entry, IRP, ThreadListEntry );

    if (irp->CurrentLocation == irp->StackCount + 2) {

        //
        // The request has just gone through enough of completion that
        // queueing it to the thread is inevitable.  Simply release the
        // lock and return.
        //

        IopReleaseCompletionLock(spIrql);
        KeLowerIrql( irql );
        return;
    }

    //
    // The packet has been located, and it is not going through completion
    // at this point.  Switch threads, so that it will not complete through
    // this thread, remove the request from this thread's queue, and release
    // the spinlock.  Final processing of the IRP will occur when I/O
    // completion notices that there is no thread associated with the
    // request.  It will essentially drop the I/O on the floor.
    //
    // Also, while the request is still held, attempt to determine on which
    // device object the operation is being performed.
    //

    irp->Tail.Overlay.Thread = (PETHREAD) NULL;
    entry = RemoveHeadList( &thread->IrpList );

    // Initialize the thread entry. Otherwise the assertion in IoFreeIrp
    // called via IopDeadIrp will fail.
    InitializeListHead (&(irp)->ThreadListEntry);

    irpSp = IoGetCurrentIrpStackLocation( irp );
    if (irp->CurrentLocation <= irp->StackCount) {
        deviceObject = irpSp->DeviceObject;
    } else {
        deviceObject = (PDEVICE_OBJECT) NULL;
    }
    IopReleaseCompletionLock(spIrql);
    KeLowerIrql( irql );

    return;
}

VOID
IopDropIrp(
    IN PIRP Irp,
    IN PFILE_OBJECT FileObject
    )

/*++

Routine Description:

    This routine attempts to drop everything about the specified IRP on the
    floor.

Arguments:

    Irp - Supplies the I/O Request Packet to be completed to the bit bucket.

    FileObject - Supplies the file object for which the I/O Request Packet was
        bound.

Return Value:

    None.

--*/

{
    //
    // Free the resources associated with the IRP.
    //

    if (Irp->UserEvent &&
        FileObject &&
        !(Irp->Flags & IRP_SYNCHRONOUS_API)) {
        ObDereferenceObject( Irp->UserEvent );
    }

    if (FileObject && !(Irp->Flags & IRP_CREATE_OPERATION)) {
        ObDereferenceObject( FileObject );
    }

    //
    // Finally, free the IRP itself.
    //

    IoFreeIrp( Irp );
}

VOID
IopExceptionCleanup(
    IN PFILE_OBJECT FileObject,
    IN PIRP Irp,
    IN PKEVENT EventObject OPTIONAL
    )

/*++

Routine Description:

    This routine performs generalized cleanup for the I/O system services when
    an exception occurs during caller parameter processing.  This routine
    performs the following steps:

        o   If a system buffer was allocated it is freed.

        o   If an MDL was allocated it is freed.

        o   The IRP is freed.

        o   If the file object is opened for synchronous I/O, the semaphore
            is released.

        o   If an event object was referenced it is dereferenced.

        o   The file object is dereferenced.

Arguments:

    FileObject - Pointer to the file object currently being worked on.

    Irp - Pointer to the IRP allocated to handle the I/O request.

    EventObject - Optional pointer to a referenced event object.

    KernelEvent - Optional pointer to an allocated kernel event.

Return Value:

    None.

--*/

{
    PAGED_CODE();

    //
    // Free the I/O Request Packet.
    //

    IoFreeIrp( Irp );

    //
    // Finally, release the synchronization semaphore if it is currently
    // held, dereference the event if one was specified, and dereference
    // the file object.
    //

    if (FileObject->Flags & FO_SYNCHRONOUS_IO) {
        IopReleaseFileObjectLock( FileObject );
    }

    if (ARGUMENT_PRESENT( EventObject )) {
        ObDereferenceObject( EventObject );
    }

    ObDereferenceObject( FileObject );

    return;
}

VOID
IopDecrementDeviceObjectRef(
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    The routine decrements the reference count on a device object.  If the
    reference count goes to zero and the device object is a candidate for deletion
    then IopCompleteUnloadOrDelete is called.  A device object is subject for
    deletion if the device object is pending deletion or the driver is pending
    unload.

Arguments:

    DeviceObject - Supplies the device object whose reference count is to be
                   decremented.

Return Value:

    None.

--*/
{
    KIRQL irql;

    //
    // Decrement the reference count on the device object.  If this is the last
    // last reason that this mini-file system recognizer needs to stay around,
    // then unload it.
    //

    irql = KeRaiseIrqlToDpcLevel();

    ASSERT( DeviceObject->ReferenceCount > 0 );

    DeviceObject->ReferenceCount--;

    if (!DeviceObject->ReferenceCount && DeviceObject->DeletePending) {
        IopCompleteUnloadOrDelete( DeviceObject, irql );
    } else {
        KeLowerIrql(irql);
    }
}

NTSTATUS
IopOpenRenameTarget(
    OUT PHANDLE TargetHandle,
    IN PIRP Irp,
    IN PFILE_RENAME_INFORMATION RenameBuffer,
    IN PFILE_OBJECT FileObject
    )

/*++

Routine Description:

    This routine is invoked by the rename code in the I/O system's
    NtSetInformationFile system service when the caller has specified a fully
    qualified file name as the target of a rename operation.  This routine
    attempts to open the parent of the specified file and checks the following:

        o   If the file itself exists, then the caller must have specified that
            the target is to be replaced, otherwise an error is returned.

        o   Ensures that the target file specification refers to the same volume
            upon which the source file exists.

Arguments:

    TargetHandle - Supplies the address of a variable to return the handle to
        the opened target file if no errors have occurred.

    Irp - Supplies a pointer to the IRP that represents the current rename
        request.

    RenameBuffer - Supplies a pointer to the system intermediate buffer that
        contains the caller's rename parameters.

    FileObject - Supplies a pointer to the file object representing the file
        being renamed.

Return Value:

    The function value is the final status of the operation.

Note:

    This function assumes that the layout of a rename, set link and set
    copy-on-write information structure are exactly the same.

--*/

{
    NTSTATUS status;
    IO_STATUS_BLOCK ioStatus;
    HANDLE handle;
    OBJECT_ATTRIBUTES objectAttributes;
    PIO_STACK_LOCATION irpSp;
    PFILE_OBJECT targetFileObject;

    PAGED_CODE();

    //
    // A fully qualified file name was specified.  Begin by attempting to open
    // the parent directory of the specified target file.
    //

    InitializeObjectAttributes( &objectAttributes,
                                &RenameBuffer->FileName,
                                OBJ_CASE_INSENSITIVE,
                                RenameBuffer->RootDirectory,
                                (PSECURITY_DESCRIPTOR) NULL );

    status = IoCreateFile( &handle,
                           FILE_WRITE_DATA | SYNCHRONIZE,
                           &objectAttributes,
                           &ioStatus,
                           (PLARGE_INTEGER) NULL,
                           0,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           FILE_OPEN,
                           FILE_OPEN_FOR_BACKUP_INTENT,
                           IO_NO_PARAMETER_CHECKING |
                           IO_OPEN_TARGET_DIRECTORY |
                           IO_FORCE_ACCESS_CHECK );
    if (NT_SUCCESS( status )) {

        //
        // Everything up to this point is fine, so dereference the handle
        // to a pointer to the file object and ensure that the two file
        // specifications refer to the same device.
        //

        status = ObReferenceObjectByHandle( handle,
                                          &IoFileObjectType,
                                          (PVOID *) &targetFileObject );
        if (NT_SUCCESS( status )) {

            ObDereferenceObject( targetFileObject );

            if (targetFileObject->DeviceObject != FileObject->DeviceObject) {

                //
                // The two files refer to different devices.  Clean everything
                // up and return an appropriate error.
                //

                NtClose( handle );
                status = STATUS_NOT_SAME_DEVICE;

            } else {

                //
                // Otherwise, everything worked, so allow the rename operation
                // to continue.
                //

                irpSp = IoGetNextIrpStackLocation( Irp );
                irpSp->Parameters.SetFile.FileObject = targetFileObject;
                *TargetHandle = handle;
                status = STATUS_SUCCESS;
            }

        } else {

            //
            // There was an error referencing the handle to what should
            // have been the target directory.  This generally means that
            // there was a resource problem or the handle was invalid, etc.
            // Simply attempt to close the handle and return the error.
            //

            NtClose( handle );
        }
    }

    //
    // Return the final status of the operation.
    //

    return status;
}

NTSTATUS
IopQueryXxxInformation(
    IN PFILE_OBJECT FileObject,
    IN ULONG InformationClass,
    IN ULONG Length,
    OUT PVOID Information,
    OUT PULONG ReturnedLength,
    IN BOOLEAN FileInformation
    )

/*++

Routine Description:

    This routine returns the requested information about a specified file
    or volume.  The information returned is determined by the class that
    is specified, and it is placed into the caller's output buffer.

Arguments:

    FileObject - Supplies a pointer to the file object about which the requested
        information is returned.

    FsInformationClass - Specifies the type of information which should be
        returned about the file/volume.

    Length - Supplies the length of the buffer in bytes.

    FsInformation - Supplies a buffer to receive the requested information
        returned about the file.  This buffer must not be pageable and must
        reside in system space.

    ReturnedLength - Supplies a variable that is to receive the length of the
        information written to the buffer.

    FileInformation - Boolean that indicates whether the information requested
        is for a file or a volume.

Return Value:

    The status returned is the final completion status of the operation.

--*/

{
    PIRP irp;
    NTSTATUS status;
    PDEVICE_OBJECT deviceObject;
    KEVENT event;
    PIO_STACK_LOCATION irpSp;
    IO_STATUS_BLOCK localIoStatus;
    BOOLEAN synchronousIo;

    PAGED_CODE();

    //
    // Reference the file object here so that no special checks need be made
    // in I/O completion to determine whether or not to dereference the file
    // object.
    //

    ObReferenceObject( FileObject );

    //
    // Make a special check here to determine whether this is a synchronous
    // I/O operation.  If it is, then wait here until the file is owned by
    // the current thread.  If this is not a (serialized) synchronous I/O
    // operation, then initialize the local event.
    //

    if (FileObject->Flags & FO_SYNCHRONOUS_IO) {
        IopAcquireFileObjectLock(FileObject);
        KeClearEvent( &FileObject->Event );
        synchronousIo = TRUE;
    } else {
        KeInitializeEvent( &event, SynchronizationEvent, FALSE );
        synchronousIo = FALSE;
    }

    //
    // Get the address of the target device object.
    //

    deviceObject = FileObject->DeviceObject;

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

        return IopAllocateIrpCleanup( FileObject, (PKEVENT) NULL );
    }
    irp->Tail.Overlay.OriginalFileObject = FileObject;
    irp->Tail.Overlay.Thread = PsGetCurrentThread();

    //
    // Fill in the service independent parameters in the IRP.
    //

    irp->UserBuffer = Information;
    if (!synchronousIo) {
        irp->UserEvent = &event;
        irp->Flags = IRP_SYNCHRONOUS_API;
    }
    irp->UserIosb = &localIoStatus;

    //
    // Get a pointer to the stack location for the first driver.  This will be
    // used to pass the original function codes and parameters.
    //

    irpSp = IoGetNextIrpStackLocation( irp );
    irpSp->MajorFunction = (UCHAR)(FileInformation ? IRP_MJ_QUERY_INFORMATION :
        IRP_MJ_QUERY_VOLUME_INFORMATION);
    irpSp->FileObject = FileObject;

    //
    // Copy the caller's parameters to the service-specific portion of the
    // IRP.
    //

    if (FileInformation) {
        irpSp->Parameters.QueryFile.Length = Length;
        irpSp->Parameters.QueryFile.FileInformationClass = InformationClass;
    } else {
        irpSp->Parameters.QueryVolume.Length = Length;
        irpSp->Parameters.QueryVolume.FsInformationClass = InformationClass;
    }

    //
    // Insert the packet at the head of the IRP list for the thread.
    //

    IopQueueThreadIrp( irp );

    //
    // Now simply invoke the driver at its dispatch entry with the IRP.
    //

    status = IoCallDriver( deviceObject, irp );

    //
    // If this operation was a synchronous I/O operation, check the return
    // status to determine whether or not to wait on the file object.  If
    // the file object is to be waited on, wait for the operation to complete
    // and obtain the final status from the file object itself.
    //

    if (synchronousIo) {
        if (status == STATUS_PENDING) {
            KeWaitForSingleObject( &FileObject->Event,
                                   Executive,
                                   KernelMode,
                                   FALSE,
                                   (PLARGE_INTEGER) NULL );
            status = FileObject->FinalStatus;
        }
        IopReleaseFileObjectLock( FileObject );

    } else {

        //
        // This is a normal synchronous I/O operation, as opposed to a
        // serialized synchronous I/O operation.  For this case, wait
        // for the local event and copy the final status information
        // back to the caller.
        //

        if (status == STATUS_PENDING) {
            (VOID) KeWaitForSingleObject( &event,
                                          Executive,
                                          KernelMode,
                                          FALSE,
                                          (PLARGE_INTEGER) NULL );
            status = localIoStatus.Status;
        }
    }

    *ReturnedLength = (ULONG) localIoStatus.Information;
    return status;
}

NTSTATUS
IopSynchronousApiServiceTail(
    IN NTSTATUS ReturnedStatus,
    IN PKEVENT Event,
    IN PIRP Irp,
    IN PIO_STATUS_BLOCK LocalIoStatus,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    )

/*++

Routine Description:

    This routine is invoked when a synchronous API is invoked for a file
    that has been opened for asynchronous I/O.  This function synchronizes
    the completion of the I/O operation on the file.

Arguments:

    ReturnedStatus - Supplies the status that was returned from the call to
        IoCallDriver.

    Event - Address of the stack allocated kernel event to be used for
        synchronization of the I/O operation.

    Irp - Address of the I/O Request Packet submitted to the driver.

    LocalIoStatus - Address of the I/O status block used to capture the final
        status by the service itself.

    IoStatusBlock - Address of the I/O status block supplied by the caller of
        the system service.

Return Value:

    The function value is the final status of the operation.


--*/

{
    NTSTATUS status;

    PAGED_CODE();

    //
    // This is a normal synchronous I/O operation, as opposed to a
    // serialized synchronous I/O operation.  For this case, wait for
    // the local event and copy the final status information back to
    // the caller.
    //

    status = ReturnedStatus;

    if (status == STATUS_PENDING) {

        KeWaitForSingleObject( Event,
                               Executive,
                               KernelMode,
                               FALSE,
                               (PLARGE_INTEGER) NULL );

        status = LocalIoStatus->Status;
    }

    *IoStatusBlock = *LocalIoStatus;

    return status;
}

NTSTATUS
IopSynchronousServiceTail(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN DeferredIoCompletion,
    IN BOOLEAN SynchronousIo
    )

/*++

Routine Description:

    This routine is invoked to complete the operation of a system service.
    It queues the IRP to the thread's queue, updates the transfer count,
    calls the driver, and finally synchronizes completion of the I/O.

Arguments:

    DeviceObject - Device on which the I/O is to occur.

    Irp - I/O Request Packet representing the I/O operation.

    FileObject - File object for this open instantiation.

    DeferredIoCompletion - Indicates whether deferred completion is possible.

    SynchronousIo - Indicates whether the operation is to be synchronous.

Return Value:

    The function value is the final status of the operation.

--*/

{
    NTSTATUS status;

    PAGED_CODE();

    //
    // Insert the packet at the head of the IRP list for the thread.
    //

    IopQueueThreadIrp( Irp );

    //
    // Now simply invoke the driver at its dispatch entry with the IRP.
    //

    status = IoCallDriver( DeviceObject, Irp );

    //
    // If deferred I/O completion is possible, check for pending returned
    // from the driver.  If the driver did not return pending, then the
    // packet has not actually been completed yet, so complete it here.
    //

    if (DeferredIoCompletion) {

        if (status != STATUS_PENDING) {

            //
            // The I/O operation was completed without returning a status of
            // pending.  This means that at this point, the IRP has not been
            // fully completed.  Complete it now.
            //

            PKNORMAL_ROUTINE normalRoutine;
            PVOID normalContext;
            KIRQL irql;

            ASSERT( !Irp->PendingReturned );

            KeRaiseIrql( APC_LEVEL, &irql );
            IopCompleteRequest( &Irp->Tail.Apc,
                                &normalRoutine,
                                &normalContext,
                                (PVOID *) &FileObject,
                                &normalContext );
            KeLowerIrql( irql );
        }
    }

    //
    // If this operation was a synchronous I/O operation, check the return
    // status to determine whether or not to wait on the file object.  If
    // the file object is to be waited on, wait for the operation to complete
    // and obtain the final status from the file object itself.
    //

    if (SynchronousIo) {

        if (status == STATUS_PENDING) {

            KeWaitForSingleObject( &FileObject->Event,
                                   Executive,
                                   KernelMode,
                                   FALSE,
                                   (PLARGE_INTEGER) NULL );

            status = FileObject->FinalStatus;
        }

        IopReleaseFileObjectLock( FileObject );
    }

    return status;
}

VOID
IopUserCompletion(
    IN PKAPC Apc,
    IN PKNORMAL_ROUTINE *NormalRoutine,
    IN PVOID *NormalContext,
    IN PVOID *SystemArgument1,
    IN PVOID *SystemArgument2
    )

/*++

Routine Description:

    This routine is invoked in the final processing of an IRP.  Everything has
    been completed except that the caller's APC routine must be invoked.  The
    system will do this as soon as this routine exits.  The only processing
    remaining to be completed by the I/O system is to free the I/O Request
    Packet itself.

Arguments:

    Apc - Supplies a pointer to kernel APC structure.

    NormalRoutine - Supplies a pointer to a pointer to the normal function
        that was specified when the APC was initialied.

    NormalContext - Supplies a pointer to a pointer to an arbitrary data
        structure that was specified when the APC was initialized.

    SystemArgument1, SystemArgument2 - Supplies a set of two pointers to
        two arguments that contain untyped data.

Return Value:

    None.

Note:

    If no other processing is ever needed, and the APC can be placed at the
    beginning of the IRP, then this routine could be replaced by simply
    specifying the address of the pool deallocation routine in the APC instead
    of the address of this routine.

Caution:

    This routine is also invoked as a general purpose rundown routine for APCs.
    Should this code ever need to directly access any of the other parameters
    other than Apc, this routine will need to be split into two separate
    routines.  The rundown routine should perform exactly the following code's
    functionality.

--*/

{
    UNREFERENCED_PARAMETER( NormalRoutine );
    UNREFERENCED_PARAMETER( NormalContext );
    UNREFERENCED_PARAMETER( SystemArgument1 );
    UNREFERENCED_PARAMETER( SystemArgument2 );

    PAGED_CODE();

    //
    // Free the packet.
    //

    IoFreeIrp( CONTAINING_RECORD( Apc, IRP, Tail.Apc ) );
}

VOID
IopUserRundown(
    IN PKAPC Apc
    )

/*++

Routine Description:

    This routine is invoked during thread termination as the rundown routine
    for it simply calls IopUserCompletion.

Arguments:

    Apc - Supplies a pointer to kernel APC structure.

Return Value:

    None.


--*/

{
    PAGED_CODE();

    //
    // Free the packet.
    //

    IoFreeIrp( CONTAINING_RECORD( Apc, IRP, Tail.Apc ) );
}

NTSTATUS
IopXxxControlFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG IoControlCode,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN BOOLEAN DeviceIoControl
    )

/*++

Routine Description:

    This service builds descriptors or MDLs for the supplied buffer(s) and
    passes the untyped data to the driver associated with the file handle.
    handle.  It is up to the driver to check the input data and function
    IoControlCode for validity, as well as to make the appropriate access
    checks.

Arguments:

    FileHandle - Supplies a handle to the file on which the service is being
        performed.

    Event - Supplies an optional event to be set to the Signaled state when
        the service is complete.

    ApcRoutine - Supplies an optional APC routine to be executed when the
        service is complete.

    ApcContext - Supplies a context parameter to be passed to the ApcRoutine,
        if an ApcRoutine was specified.

    IoStatusBlock - Address of the caller's I/O status block.

    IoControlCode - Subfunction code to determine exactly what operation is
        being performed.

    InputBuffer - Optionally supplies an input buffer to be passed to the
        driver.  Whether or not the buffer is actually optional is dependent
        on the IoControlCode.

    InputBufferLength - Length of the InputBuffer in bytes.

    OutputBuffer - Optionally supplies an output buffer to receive information
        from the driver.  Whether or not the buffer is actually optional is
        dependent on the IoControlCode.

    OutputBufferLength - Length of the OutputBuffer in bytes.

    DeviceIoControl - Determines whether this is a Device or File System
        Control function.

Return Value:

    The status returned is success if the control operation was properly
    queued to the I/O system.   Once the operation completes, the status
    can be determined by examining the Status field of the I/O status block.

--*/

{
    PIRP irp;
    NTSTATUS status;
    PFILE_OBJECT fileObject;
    PDEVICE_OBJECT deviceObject;
    PKEVENT eventObject = (PKEVENT) NULL;
    PIO_STACK_LOCATION irpSp;
    ULONG method;
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
    // Get the address of the target device object.
    //

    deviceObject = fileObject->DeviceObject;

    //
    // Set the file object to the Not-Signaled state.
    //

    KeClearEvent( &fileObject->Event );

    //
    // Allocate and initialize the I/O Request Packet (IRP) for this operation.

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

    irp->UserBuffer = OutputBuffer;
    irp->UserEvent = eventObject;
    irp->UserIosb = IoStatusBlock;
    irp->Overlay.AsynchronousParameters.UserApcRoutine = ApcRoutine;
    irp->Overlay.AsynchronousParameters.UserApcContext = ApcContext;

    //
    // Get a pointer to the stack location for the first driver.  This will be
    // used to pass the original function codes and parameters.
    //

    irpSp = IoGetNextIrpStackLocation( irp );
    irpSp->MajorFunction = (UCHAR)(DeviceIoControl ? IRP_MJ_DEVICE_CONTROL :
        IRP_MJ_FILE_SYSTEM_CONTROL);
    irpSp->FileObject = fileObject;

    //
    // Copy the caller's parameters to the service-specific portion of the
    // IRP for those parameters that are the same for all three methods.
    //

    irpSp->Parameters.DeviceIoControl.OutputBufferLength = OutputBufferLength;
    irpSp->Parameters.DeviceIoControl.InputBufferLength = InputBufferLength;
    irpSp->Parameters.DeviceIoControl.IoControlCode = IoControlCode;
    irpSp->Parameters.DeviceIoControl.InputBuffer = InputBuffer;

    //
    // Based on the method that the buffer are being passed, lock down the
    // output buffer.
    //

    if (OutputBufferLength != 0) {

        method = IoControlCode & 3;

        if ((method == METHOD_IN_DIRECT) || (method == METHOD_OUT_DIRECT)) {
            IoLockUserBuffer(irp, OutputBufferLength);
        }
    }

    //
    // Defer I/O completion for FSCTL requests, but not for IOCTL requests,
    // since file systems set pending properly but device driver do not.
    //

    if (!DeviceIoControl) {
        irp->Flags |= IRP_DEFER_IO_COMPLETION;
    }

    //
    // Queue the packet, call the driver, and synchronize appropriately with
    // I/O completion.
    //

    return IopSynchronousServiceTail( deviceObject,
                                      irp,
                                      fileObject,
                                      (BOOLEAN)!DeviceIoControl,
                                      synchronousIo );
}
