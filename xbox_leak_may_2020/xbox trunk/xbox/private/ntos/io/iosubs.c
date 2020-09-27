/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    iosubs.c

Abstract:

    This module contains the subroutines for the I/O system.

--*/

#include "iop.h"

PIRP
IoAllocateIrp(
    IN CCHAR StackSize
    )

/*++

Routine Description:

    This routine allocates an I/O Request Packet from the system nonpaged pool.
    The packet will be allocated to contain StackSize stack locations.  The IRP
    will also be initialized.

Arguments:

    StackSize - Specifies the maximum number of stack locations required.

Return Value:

    The function value is the address of the allocated/initialized IRP,
    or NULL if one could not be allocated.

--*/

{
    USHORT packetSize;
    PIRP irp;

    packetSize = IoSizeOfIrp(StackSize);

    //
    // Attempt to allocate the IRP from non-paged pool.
    //

    irp = ExAllocatePoolWithTag(packetSize, ' prI');

    if (!irp) {
        return NULL;
    }

    //
    // Initialize the packet.
    //

    IopInitializeIrp(irp, packetSize, StackSize);

    return irp;
}

PIRP
IoBuildAsynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL,
    IN PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
    )

/*++

Routine Description:

    This routine builds an I/O Request Packet (IRP) suitable for a File System
    Driver (FSD) to use in requesting an I/O operation from a device driver.
    The request must be one of the following request codes:

        IRP_MJ_READ
        IRP_MJ_WRITE
        IRP_MJ_FLUSH_BUFFERS
        IRP_MJ_SHUTDOWN

    This routine provides a simple, fast interface to the device driver w/o
    having to put the knowledge of how to build an IRP into all of the FSDs
    (and device drivers) in the system.

Arguments:

    MajorFunction - Function to be performed;  see previous list.

    DeviceObject - Pointer to device object on which the I/O will be performed.

    Buffer - Pointer to buffer to get data from or write data into.  This
        parameter is required for read/write, but not for flush or shutdown
        functions.

    Length - Length of buffer in bytes.  This parameter is required for
        read/write, but not for flush or shutdown functions.

    StartingOffset - Pointer to the offset on the disk to read/write from/to.
        This parameter is required for read/write, but not for flush or
        shutdown functions.

    IoStatusBlock - Pointer to the I/O status block for completion status
        information.  This parameter is optional since most asynchronous FSD
        requests will be synchronized by using completion routines, and so the
        I/O status block will not be written.

Return Value:

    The function value is a pointer to the IRP representing the specified
    request.

--*/

{
    PIRP irp;
    PIO_STACK_LOCATION irpSp;
    NTSTATUS status;

    //
    // Begin by allocating the IRP for this request.
    //

    irp = IoAllocateIrp( DeviceObject->StackSize );
    if (!irp) {
        return irp;
    }
    irp->Tail.Overlay.Thread = PsGetCurrentThread();

    //
    // Get a pointer to the stack location of the first driver which will be
    // invoked.  This is where the function codes and the parameters are set.
    //

    irpSp = IoGetNextIrpStackLocation( irp );

    //
    // Set the major function code.
    //

    irpSp->MajorFunction = (UCHAR) MajorFunction;

    if (MajorFunction != IRP_MJ_FLUSH_BUFFERS &&
        MajorFunction != IRP_MJ_SHUTDOWN) {

        irp->UserBuffer = Buffer;

        //
        // Now determine whether or not we should automatically lock the user's
        // buffer for direct I/O.
        //

        if ((DeviceObject->Flags & DO_DIRECT_IO) && (Length != 0)) {
            IoLockUserBuffer(irp, Length);
        }

        //
        // Set the parameters according to whether this is a read or a write
        // operation.  Notice that these parameters must be set even if the
        // driver has not specified buffered or direct I/O.
        //

        if (MajorFunction == IRP_MJ_WRITE) {
            irpSp->Parameters.Write.Length = Length;
            irpSp->Parameters.Write.ByteOffset = *StartingOffset;
        } else {
            irpSp->Parameters.Read.Length = Length;
            irpSp->Parameters.Read.ByteOffset = *StartingOffset;
        }
    }

    //
    // Finally, set the address of the I/O status block and return a pointer
    // to the IRP.
    //

    irp->UserIosb = IoStatusBlock;
    return irp;
}

PIRP
IoBuildDeviceIoControlRequest(
    IN ULONG IoControlCode,
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN BOOLEAN InternalDeviceIoControl,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    )

/*++

Routine Description:

    This routine builds an I/O Request Packet (IRP) that can be used to
    perform a synchronous internal or normal device I/O control function.

Arguments:

    IoControlCode - Specifies the device I/O control code that is to be
        performed by the target device driver.

    DeviceObject - Specifies the target device on which the I/O control
        function is to be performed.

    InputBuffer - Optional pointer to an input buffer that is to be passed
        to the device driver.

    InputBufferLength - Length of the InputBuffer in bytes.  If the Input-
        Buffer parameter is not passed, this parameter must be zero.

    OutputBuffer - Optional pointer to an output buffer that is to be passed
        to the device driver.

    OutputBufferLength - Length of the OutputBuffer in bytes.  If the
        OutputBuffer parameter is not passed, this parameter must be zero.

    InternalDeviceIoControl - A BOOLEAN parameter that specifies whether
        the packet that gets generated should have a major function code
        of IRP_MJ_INTERNAL_DEVICE_CONTROL (the parameter is TRUE), or
        IRP_MJ_DEVICE_CONTROL (the parameter is FALSE).

    Event - Supplies a pointer to a kernel event that is to be set to the
        Signaled state when the I/O operation is complete.  Note that the
        Event must already be set to the Not-Signaled state.

    IoStatusBlock - Supplies a pointer to an I/O status block that is to
        be filled in with the final status of the operation once it
        completes.

Return Value:

    The function value is a pointer to the generated IRP suitable for calling
    the target device driver.

--*/

{
    PIRP irp;
    PIO_STACK_LOCATION irpSp;
    ULONG method;
    NTSTATUS status;

    //
    // Begin by allocating the IRP for this request.
    //

    irp = IoAllocateIrp( DeviceObject->StackSize );
    if (!irp) {
        return irp;
    }
    irp->UserBuffer = OutputBuffer;

    //
    // Get a pointer to the stack location of the first driver which will be
    // invoked.  This is where the function codes and the parameters are set.
    //

    irpSp = IoGetNextIrpStackLocation( irp );

    //
    // Set the major function code based on the type of device I/O control
    // function the caller has specified.
    //

    if (InternalDeviceIoControl) {
        irpSp->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
    } else {
        irpSp->MajorFunction = IRP_MJ_DEVICE_CONTROL;
    }

    //
    // Copy the caller's parameters to the service-specific portion of the
    // IRP for those parameters that are the same for all four methods.
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
    // Finally, set the address of the I/O status block and the address of
    // the kernel event object.  Note that I/O completion will not attempt
    // to dereference the event since there is no file object associated
    // with this operation.
    //

    irp->UserIosb = IoStatusBlock;
    irp->UserEvent = Event;

    //
    // Also set the address of the current thread in the packet so the
    // completion code will have a context to execute in.  The IRP also
    // needs to be queued to the thread since the caller is going to set
    // the file object pointer.
    //

    irp->Tail.Overlay.Thread = PsGetCurrentThread();
    IopQueueThreadIrp( irp );

    //
    // Simply return a pointer to the packet.
    //

    return irp;
}

PIRP
IoBuildSynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    )

/*++

Routine Description:

    This routine builds an I/O Request Packet (IRP) suitable for a File System
    Driver (FSD) to use in requesting an I/O operation from a device driver.
    The request must be one of the following request codes:

        IRP_MJ_READ
        IRP_MJ_WRITE
        IRP_MJ_FLUSH_BUFFERS
        IRP_MJ_SHUTDOWN

    This routine provides a simple, fast interface to the device driver w/o
    having to put the knowledge of how to build an IRP into all of the FSDs
    (and device drivers) in the system.

    The IRP created by this function causes the I/O system to complete the
    request by setting the specified event to the Signaled state.

Arguments:

    MajorFunction - Function to be performed;  see previous list.

    DeviceObject - Pointer to device object on which the I/O will be performed.

    Buffer - Pointer to buffer to get data from or write data into.  This
        parameter is required for read/write, but not for flush or shutdown
        functions.

    Length - Length of buffer in bytes.  This parameter is required for
        read/write, but not for flush or shutdown functions.

    StartingOffset - Pointer to the offset on the disk to read/write from/to.
        This parameter is required for read/write, but not for flush or
        shutdown functions.

    Event - Pointer to a kernel event structure for synchronization.  The event
        will be set to the Signaled state when the I/O has completed.

    IoStatusBlock - Pointer to I/O status block for completion status info.

Return Value:

    The function value is a pointer to the IRP representing the specified
    request.

--*/

{
    PIRP irp;

    //
    // Do all of the real work in real IRP build routine.
    //

    irp = IoBuildAsynchronousFsdRequest( MajorFunction,
                                         DeviceObject,
                                         Buffer,
                                         Length,
                                         StartingOffset,
                                         IoStatusBlock );
    if (irp == NULL) {
        return irp;
    }

    //
    // Now fill in the event to the completion code will do the right thing.
    // Notice that because there is no FileObject, the I/O completion code
    // will not attempt to dereference the event.
    //

    irp->UserEvent = Event;

    //
    // There will be a file object associated w/this packet, so it must be
    // queued to the thread.
    //

    IopQueueThreadIrp( irp );
    return irp;
}

NTSTATUS
FASTCALL
IofCallDriver(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )

/*++

Routine Description:

    This routine is invoked to pass an I/O Request Packet (IRP) to another
    driver at its dispatch routine.

Arguments:

    DeviceObject - Pointer to device object to which the IRP should be passed.

    Irp - Pointer to IRP for request.

Return Value:

    Return status from driver's dispatch routine.

--*/

{
    PIO_STACK_LOCATION irpSp;
    PDRIVER_OBJECT driverObject;
    NTSTATUS status;

    //
    // Ensure that this is really an I/O Request Packet.
    //

    ASSERT( Irp->Type == IO_TYPE_IRP );

    //
    // Update the IRP stack to point to the next location.
    //
    Irp->CurrentLocation--;

    if (Irp->CurrentLocation <= 0) {
        KeBugCheckEx( NO_MORE_IRP_STACK_LOCATIONS, (ULONG_PTR) Irp, 0, 0, 0 );
    }

    irpSp = IoGetNextIrpStackLocation( Irp );
    Irp->Tail.Overlay.CurrentStackLocation = irpSp;

    //
    // Save a pointer to the device object for this request so that it can
    // be used later in completion.
    //

    irpSp->DeviceObject = DeviceObject;

    //
    // Invoke the driver at its dispatch routine entry point.
    //

    driverObject = DeviceObject->DriverObject;

    status = driverObject->MajorFunction[irpSp->MajorFunction]( DeviceObject,
                                                              Irp );

    return status;
}

VOID
IoCancelThreadIo(
    IN PETHREAD Thread
    )

/*++

Routine Description:

    This routine cancels all of the I/O operations for the specified thread.
    This is accomplished by walking the list of IRPs in the thread IRP list
    and canceling each one individually.  No other I/O operations can be
    started for the thread since this routine has control of the thread itself.

Arguments:

    Tcb - Pointer to the Thread Control Block for the thread.

Return Value:

    None.

--*/

{
    PLIST_ENTRY header;
    PLIST_ENTRY entry;
    KIRQL irql;
    PIRP irp;
    ULONG count;
    LARGE_INTEGER interval;

    PAGED_CODE();

    //
    // Raise the IRQL so that the IrpList cannot be modified by a completion
    // APC and so that we can mark all of the IRPs as canceled without being
    // interrupted by IofCompleteRequest.
    //

    irql = KeRaiseIrqlToDpcLevel();

    header = &Thread->IrpList;
    entry = Thread->IrpList.Flink;

    //
    // Walk the list of pending IRPs, canceling each of them.
    //

    while (header != entry) {
        irp = CONTAINING_RECORD( entry, IRP, ThreadListEntry );
        irp->Cancel = TRUE;
        entry = entry->Flink;
    }

    //
    // Wait for the requests to complete.  Note that waiting may eventually
    // timeout, in which case more work must be done.
    //

    count = 0;
    interval.QuadPart = -10 * 1000 * 100;

    while (!IsListEmpty( &Thread->IrpList )) {

        //
        // Lower the IRQL so that the thread APC can fire which will complete
        // the requests.  Delay execution for a time and let the request
        // finish.  The delay time is 100ms.
        //

        KeLowerIrql( irql );
        KeDelayExecutionThread( KernelMode, FALSE, &interval );

        if (count++ > 3000) {

            //
            // This I/O request has timed out, as it has not been completed
            // for a full 5 minutes. Attempt to remove the packet's association
            // with this thread.  Note that by not resetting the count, the
            // next time through the loop the next packet, if there is one,
            // which has also timed out, will be dealt with, although it
            // will be given another 100ms to complete.
            //

            IopDisassociateThreadIrp();
        }

        KeRaiseIrql( APC_LEVEL, &irql );
    }

    KeLowerIrql( irql );
}

NTSTATUS
IoCheckShareAccess(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG DesiredShareAccess,
    IN OUT PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess,
    IN BOOLEAN Update
    )

/*++

Routine Description:

    This routine is invoked to determine whether or not a new accessor to
    a file actually has shared access to it.  The check is made according
    to:

        1)  How the file is currently opened.

        2)  What types of shared accesses are currently specified.

        3)  The desired and shared accesses that the new open is requesting.

    If the open should succeed, then the access information about how the
    file is currently opened is updated, according to the Update parameter.

Arguments:

    DesiredAccess - Desired access of current open request.

    DesiredShareAccess - Shared access requested by current open request.

    FileObject - Pointer to the file object of the current open request.

    ShareAccess - Pointer to the share access structure that describes how
        the file is currently being accessed.

    Update - Specifies whether or not the share access information for the
        file is to be updated.

Return Value:

    The final status of the access check is the function value.  If the
    accessor has access to the file, STATUS_SUCCESS is returned.  Otherwise,
    STATUS_SHARING_VIOLATION is returned.

Note:

    Note that the ShareAccess parameter must be locked against other accesses
    from other threads while this routine is executing.  Otherwise the counts
    will be out-of-synch.

--*/

{
    UCHAR ocount;

    PAGED_CODE();

    //
    // Set the access type in the file object for the current accessor.
    // Note that reading and writing attributes are not included in the
    // access check.
    //

    FileObject->ReadAccess = (BOOLEAN) ((DesiredAccess & (FILE_EXECUTE
        | FILE_READ_DATA)) != 0);
    FileObject->WriteAccess = (BOOLEAN) ((DesiredAccess & (FILE_WRITE_DATA
        | FILE_APPEND_DATA)) != 0);
    FileObject->DeleteAccess = (BOOLEAN) ((DesiredAccess & DELETE) != 0);

    //
    // There is no more work to do unless the user specified one of the
    // sharing modes above.
    //

    if (FileObject->ReadAccess ||
        FileObject->WriteAccess ||
        FileObject->DeleteAccess) {

        FileObject->SharedRead = (BOOLEAN) ((DesiredShareAccess & FILE_SHARE_READ) != 0);
        FileObject->SharedWrite = (BOOLEAN) ((DesiredShareAccess & FILE_SHARE_WRITE) != 0);
        FileObject->SharedDelete = (BOOLEAN) ((DesiredShareAccess & FILE_SHARE_DELETE) != 0);

        //
        // Now check to see whether or not the desired accesses are compatible
        // with the way that the file is currently open.
        //

        ocount = ShareAccess->OpenCount;

        if ( (ocount == MAXUCHAR)
             ||
             (FileObject->ReadAccess && (ShareAccess->SharedRead < ocount))
             ||
             (FileObject->WriteAccess && (ShareAccess->SharedWrite < ocount))
             ||
             (FileObject->DeleteAccess && (ShareAccess->SharedDelete < ocount))
             ||
             ((ShareAccess->Readers != 0) && !FileObject->SharedRead)
             ||
             ((ShareAccess->Writers != 0) && !FileObject->SharedWrite)
             ||
             ((ShareAccess->Deleters != 0) && !FileObject->SharedDelete)
           ) {

            //
            // The check failed.  Simply return to the caller indicating that the
            // current open cannot access the file.
            //

            return STATUS_SHARING_VIOLATION;

        //
        // The check was successful.  Update the counter information in the
        // shared access structure for this open request if the caller
        // specified that it should be updated.
        //

        } else if (Update) {

            ShareAccess->OpenCount++;

            ShareAccess->Readers += FileObject->ReadAccess;
            ShareAccess->Writers += FileObject->WriteAccess;
            ShareAccess->Deleters += FileObject->DeleteAccess;

            ShareAccess->SharedRead += FileObject->SharedRead;
            ShareAccess->SharedWrite += FileObject->SharedWrite;
            ShareAccess->SharedDelete += FileObject->SharedDelete;
        }
    }
    return STATUS_SUCCESS;
}

VOID
FASTCALL
IofCompleteRequest(
    IN PIRP Irp,
    IN CCHAR PriorityBoost
    )

/*++

Routine Description:

    This routine is invoked to complete an I/O request.  It is invoked by the
    driver in its DPC routine to perform the final completion of the IRP.  The
    functions performed by this routine are as follows.

        1.  A check is made to determine whether the packet's stack locations
            have been exhausted.  If not, then the stack location pointer is set
            to the next location and if there is a routine to be invoked, then
            it will be invoked.  This continues until there are either no more
            routines which are interested or the packet runs out of stack.

            If a routine is invoked to complete the packet for a specific driver
            which needs to perform work a lot of work or the work needs to be
            performed in the context of another process, then the routine will
            return an alternate success code of STATUS_MORE_PROCESSING_REQUIRED.
            This indicates that this completion routine should simply return to
            its caller because the operation will be "completed" by this routine
            again sometime in the future.

        2.  A check is made to determine whether this IRP is an associated IRP.
            If it is, then the count on the master IRP is decremented.  If the
            count for the master becomes zero, then the master IRP will be
            completed according to the steps below taken for a normal IRP being
            completed.  If the count is still non-zero, then this IRP (the one
            being completed) will simply be deallocated.

        3.  If this is paging I/O or a close operation, then simply write the
            I/O status block and set the event to the signaled state, and
            dereference the event.  If this is paging I/O, deallocate the IRP
            as well.

        4.  Unlock the pages, if any, specified by the MDL by calling
            MmUnlockPages.

        5.  A check is made to determine whether or not completion of the
            request can be deferred until later.  If it can be, then this
            routine simply exits and leaves it up to the originator of the
            request to fully complete the IRP.  By not initializing and queueing
            the special kernel APC to the calling thread (which is the current
            thread by definition), a lot of interrupt and queueing processing
            can be avoided.

        6.  The final rundown routine is invoked to queue the request packet to
            the target (requesting) thread as a special kernel mode APC.

Arguments:

    Irp - Pointer to the I/O Request Packet to complete.

    PriorityBoost - Supplies the amount of priority boost that is to be given
        to the target thread when the special kernel APC is queued.

Return Value:

    None.

--*/

#define ZeroIrpStackLocation( IrpSp ) {         \
    (IrpSp)->MinorFunction = 0;                 \
    (IrpSp)->Flags = 0;                         \
    (IrpSp)->Control = 0 ;                      \
    (IrpSp)->Parameters.Others.Argument1 = 0;   \
    (IrpSp)->Parameters.Others.Argument2 = 0;   \
    (IrpSp)->Parameters.Others.Argument3 = 0;   \
    (IrpSp)->Parameters.Others.Argument4 = 0;   \
    (IrpSp)->FileObject = (PFILE_OBJECT) NULL; }

{
    NTSTATUS status;
    PIO_STACK_LOCATION stackPointer;
    PETHREAD thread;
    PFILE_OBJECT fileObject;
    KIRQL irql;

    //
    // Begin by ensuring that this packet has not already been completed
    // by someone.
    //

    if (Irp->CurrentLocation > (CCHAR) (Irp->StackCount + 1) ||
        Irp->Type != IO_TYPE_IRP) {
        KeBugCheckEx( MULTIPLE_IRP_COMPLETE_REQUESTS, (ULONG_PTR) Irp, __LINE__, 0, 0 );
    }

    //
    // Ensure that the packet being completed really is still an IRP.
    //

    ASSERT( Irp->Type == IO_TYPE_IRP );

    //
    // Ensure that the packet is not being completed with a thoroughly
    // confusing status code.  Actually completing a packet with a pending
    // status probably means that someone forgot to set the real status in
    // the packet.
    //

    ASSERT( Irp->IoStatus.Status != STATUS_PENDING );

    //
    // Ensure that the packet is not being completed with a minus one.  This
    // is apparently a common problem in some drivers, and has no meaning
    // as a status code.
    //

    ASSERT( Irp->IoStatus.Status != 0xffffffff );

    //
    // Now check to see whether this is the last driver that needs to be
    // invoked for this packet.  If not, then bump the stack and check to
    // see whether the driver wishes to see the completion.  As each stack
    // location is examined, invoke any routine which needs to be invoked.
    // If the routine returns STATUS_MORE_PROCESSING_REQUIRED, then stop the
    // processing of this packet.
    //

    for (stackPointer = IoGetCurrentIrpStackLocation( Irp ),
         Irp->CurrentLocation++,
         Irp->Tail.Overlay.CurrentStackLocation++;
         Irp->CurrentLocation <= (CCHAR) (Irp->StackCount + 1);
         stackPointer++,
         Irp->CurrentLocation++,
         Irp->Tail.Overlay.CurrentStackLocation++) {

        //
        // Check if the stack location is marked as a must complete packet.  If
        // so, decrement the number of pending must complete packets.
        //

        if (stackPointer->Control & SL_MUST_COMPLETE) {

            ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

            if ((--IoPendingMustCompletePackets == 0) &&
                HalIsResetOrShutdownPending()) {
                HalMustCompletePacketsFinished();
            }
        }

        //
        // A stack location was located.  Check to see whether or not it
        // has a completion routine and if so, whether or not it should be
        // invoked.
        //
        // Begin by saving the pending returned flag in the current stack
        // location in the fixed part of the IRP.
        //

        Irp->PendingReturned = (BOOLEAN)(stackPointer->Control & SL_PENDING_RETURNED);

        if ( (NT_SUCCESS( Irp->IoStatus.Status ) &&
             stackPointer->Control & SL_INVOKE_ON_SUCCESS) ||
             (!NT_SUCCESS( Irp->IoStatus.Status ) &&
             stackPointer->Control & SL_INVOKE_ON_ERROR)
           ) {

            //
            // This driver has specified a completion routine.  Invoke the
            // routine passing it a pointer to its device object and the
            // IRP that is being completed.
            //

            ZeroIrpStackLocation( stackPointer );

            status = stackPointer->CompletionRoutine( (PDEVICE_OBJECT) (Irp->CurrentLocation == (CCHAR) (Irp->StackCount + 1) ?
                                                      (PDEVICE_OBJECT) NULL :
                                                      IoGetCurrentIrpStackLocation( Irp )->DeviceObject),
                                                      Irp,
                                                      stackPointer->Context );

            if (status == STATUS_MORE_PROCESSING_REQUIRED) {

                //
                // Note:  Notice that if the driver has returned the above
                //        status value, it may have already DEALLOCATED the
                //        packet!  Therefore, do NOT touch any part of the
                //        IRP in the following code.
                //

                return;
            }

        } else {
            if (Irp->PendingReturned && Irp->CurrentLocation <= Irp->StackCount) {
                IoMarkIrpPending( Irp );
            }
            ZeroIrpStackLocation( stackPointer );
        }
    }

    //
    // Check to see if this is paging I/O or a close operation.  If either,
    // then special processing must be performed.  The reasons that special
    // processing must be performed is different based on the type of
    // operation being performed.  The biggest reasons for special processing
    // on paging operations are that using a special kernel APC for an in-
    // page operation cannot work since the special kernel APC can incur
    // another pagefault.  Likewise, all paging I/O uses MDLs that belong
    // to the memory manager, not the I/O system.
    //
    // Close operations are special because the close may have been invoked
    // because of a special kernel APC (some IRP was completed which caused
    // the reference count on the object to become zero while in the I/O
    // system's special kernel APC routine).  Therefore, a special kernel APC
    // cannot be used since it cannot execute until the close APC finishes.
    //
    // The special steps are as follows for a synchronous paging operation
    // and close are:
    //
    //     1.  Copy the I/O status block (it is in SVAS, nonpaged).
    //     2.  Signal the event
    //     3.  If paging I/O, deallocate the IRP
    //
    // The special steps taken for asynchronous paging operations (out-pages)
    // are as follows:
    //
    //     1.  Initialize a special kernel APC just for page writes.
    //     1.  Queue the special kernel APC.
    //
    // It should also be noted that the logic for completing a Mount request
    // operation is exactly the same as a Page Read.  No assumptions should be
    // made here about this being a Page Read operation w/o carefully checking
    // to ensure that they are also true for a Mount.  That is:
    //
    //     IRP_PAGING_IO  and  IRP_MOUNT_COMPLETION
    //
    // are the same flag in the IRP.
    //
    // Also note that the last time the IRP is touched for a close operation
    // must be just before the event is set to the signaled state.  Once this
    // occurs, the IRP can be deallocated by the thread waiting for the event.
    //

    if (Irp->Flags & (IRP_MOUNT_COMPLETION | IRP_CLOSE_OPERATION)) {
        ULONG flags;
        flags = Irp->Flags & IRP_MOUNT_COMPLETION;
        *Irp->UserIosb = Irp->IoStatus;
        KeSetEvent( Irp->UserEvent, PriorityBoost, FALSE );
        if (flags) {
            IoFreeIrp( Irp );
        }
        return;
    }

    //
    // Check to see whether any pages need to be unlocked.
    //

    if ((Irp->Flags & IRP_UNLOCK_USER_BUFFER) != 0) {
        MmLockUnlockBufferPages(Irp->UserBuffer, Irp->LockedBufferLength, TRUE);
        Irp->Flags &= ~IRP_UNLOCK_USER_BUFFER;
    } else if (Irp->SegmentArray != NULL) {
        MmUnlockSelectedIoPages(Irp);
    }

    //
    // Make a final check here to determine whether or not this is a
    // synchronous I/O operation that is being completed in the context
    // of the original requestor.  If so, then an optimal path through
    // I/O completion can be taken.
    //

    if (Irp->Flags & IRP_DEFER_IO_COMPLETION && !Irp->PendingReturned) {
        return;
    }

    //
    // Finally, initialize the IRP as an APC structure and queue the special
    // kernel APC to the target thread.
    //

    thread = Irp->Tail.Overlay.Thread;
    fileObject = Irp->Tail.Overlay.OriginalFileObject;

    if (!Irp->Cancel) {

        KeInitializeApc( &Irp->Tail.Apc,
                         &thread->Tcb,
                         IopCompleteRequest,
                         (PKRUNDOWN_ROUTINE) NULL,
                         (PKNORMAL_ROUTINE) NULL,
                         KernelMode,
                         (PVOID) NULL );

        (VOID) KeInsertQueueApc( &Irp->Tail.Apc,
                                 fileObject,
                                 (PVOID) NULL,
                                 PriorityBoost );
    } else {

        //
        // This request has been cancelled.  Ensure that access to the thread
        // is synchronized, otherwise it may go away while attempting to get
        // through the remainder of completion for this request.  This happens
        // when the thread times out waiting for the request to be completed
        // once it has been cancelled.
        //
        // Note that it is safe to capture the thread pointer above, w/o having
        // the lock because the cancel flag was not set at that point, and
        // the code that disassociates IRPs must set the flag before looking to
        // see whether or not the packet has been completed, and this packet
        // will appear to be completed because it no longer belongs to a driver.
        //

        irql = IopAcquireCompletionLock();

        thread = Irp->Tail.Overlay.Thread;

        if (thread) {

            KeInitializeApc( &Irp->Tail.Apc,
                             &thread->Tcb,
                             IopCompleteRequest,
                             (PKRUNDOWN_ROUTINE) NULL,
                             (PKNORMAL_ROUTINE) NULL,
                             KernelMode,
                             (PVOID) NULL );

            (VOID) KeInsertQueueApc( &Irp->Tail.Apc,
                                     fileObject,
                                     (PVOID) NULL,
                                     PriorityBoost );

            IopReleaseCompletionLock(irql);

        } else {

            //
            // This request has been aborted from completing in the caller's
            // thread.  This can only occur if the packet was cancelled, and
            // the driver did not complete the request, so it was timed out.
            // Attempt to drop things on the floor, since the originating thread
            // has probably exited at this point.
            //

            IopReleaseCompletionLock(irql);

            ASSERT( Irp->Cancel );

            //
            // Drop the IRP on the floor.
            //

            IopDropIrp( Irp, fileObject );
        }
    }
}

NTSTATUS
IoCreateDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN ULONG DeviceExtensionSize,
    IN POBJECT_STRING DeviceName OPTIONAL,
    IN DEVICE_TYPE DeviceType,
    IN BOOLEAN Exclusive,
    OUT PDEVICE_OBJECT *DeviceObject
    )

/*++

Routine Description:

    This routine creates a device object and links it into the I/O database.

Arguments:

    DriverObject - A pointer to the driver object for this device.

    DeviceExtensionSize - Size, in bytes, of extension to device object;
        i.e., the size of the driver-specific data for this device object.

    DeviceName - The name that should be associated with this device.

    DeviceType - The type of device that the device object should represent.

    Exclusive - Indicates that the device object should be created with using
        the exclusive object attribute.

        NOTE: This flag should not be used for WDM drivers.  Since only the
        PDO is named, it is the only device object in a devnode attachment
        stack that is openable.  However, since this device object is created
        by the underlying bus driver (which has no knowledge about what type
        of device this is), there is no way to know whether this flag should
        be set.  Therefore, this parameter should always be FALSE for WDM
        drivers.  Drivers attached to the PDO (e.g., the function driver) must
        enforce any exclusivity rules.

    DeviceObject - Pointer to the device object pointer we will return.

Return Value:

    The function value is the final status of the operation.


--*/

{
    OBJECT_ATTRIBUTES objectAttributes;
    PDEVICE_OBJECT deviceObject;
    HANDLE handle;
    BOOLEAN deviceHasName;
    ULONG RoundedSize;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Remember whether or not this device was created with a name so that
    // it can be deallocated later.
    //

    deviceHasName = (BOOLEAN) (ARGUMENT_PRESENT( DeviceName ) ? TRUE : FALSE);

    //
    // Initialize the object attributes structure in preparation for creating
    // device object.  Note that the device may be created as an exclusive
    // device so that only one open can be done to it at a time.  This saves
    // single user devices from having drivers that implement special code to
    // make sure that only one connection is ever valid at any given time.
    //

    InitializeObjectAttributes( &objectAttributes,
                                DeviceName,
                                0,
                                (HANDLE) NULL,
                                (PSECURITY_DESCRIPTOR) NULL );


    if (Exclusive) {
        objectAttributes.Attributes |= OBJ_EXCLUSIVE;
    } else {
        objectAttributes.Attributes |= 0;
    }

    if (deviceHasName) {
        objectAttributes.Attributes |= OBJ_PERMANENT;
    }

    //
    // Create the device object itself.
    //

    RoundedSize = (sizeof( DEVICE_OBJECT ) + DeviceExtensionSize)
                   % sizeof (LONGLONG);
    if (RoundedSize) {
        RoundedSize = sizeof (LONGLONG) - RoundedSize;
    }

    RoundedSize += DeviceExtensionSize;

    status = ObCreateObject( &IoDeviceObjectType,
                             &objectAttributes,
                             (ULONG) sizeof( DEVICE_OBJECT ) +
                                     RoundedSize,
                             (PVOID *) &deviceObject );

    if (!NT_SUCCESS( status )) {

        //
        // Creating the device object was not successful.  Clean everything
        // up and indicate that the object was not created.
        //

        deviceObject = (PDEVICE_OBJECT) NULL;

    } else {

        //
        // The device was successfully created.  Initialize the object so
        // that it can be inserted into the object table.  Begin by zeroing
        // the memory for the device object.
        //

        RtlZeroMemory( deviceObject,
                       sizeof( DEVICE_OBJECT ) + RoundedSize );

        //
        // Set the type and size of this device object.
        //

        deviceObject->Type = IO_TYPE_DEVICE;
        deviceObject->Size = (USHORT) (sizeof( DEVICE_OBJECT ) + DeviceExtensionSize);

        //
        // The device type field in the device object was shrunk to a single
        // byte.  All of the valid types defined in devioctl.h fall well below
        // this limit.
        //

        ASSERT(DeviceType == (DEVICE_TYPE)(UCHAR)DeviceType);

        //
        // Set the device type field in the object so that later code can
        // check the type.  Likewise, set the device characteristics.
        //

        deviceObject->DeviceType = (UCHAR)DeviceType;

        //
        // If this device is a storage device, set the MountedOrSelfDevice field
        // to NULL so that IoParseDevice forces a mount for the device.  All
        // other devices do not require a mount, so set the MountedOrSelfDevice
        // field to the device itself.
        //

        if ((DeviceType == FILE_DEVICE_DISK) ||
            (DeviceType == FILE_DEVICE_MEMORY_UNIT) ||
            (DeviceType == FILE_DEVICE_CD_ROM) ||
            (DeviceType == FILE_DEVICE_MEDIA_BOARD)) {

            KeInitializeEvent( &deviceObject->DeviceLock,
                               SynchronizationEvent,
                               TRUE );

            deviceObject->MountedOrSelfDevice = NULL;

        } else {

            deviceObject->MountedOrSelfDevice = deviceObject;
        }

        //
        // Initialize the remainder of the device object.
        //
        deviceObject->AlignmentRequirement = 0;
        deviceObject->Flags = DO_DEVICE_INITIALIZING;

        if (Exclusive) {
            deviceObject->Flags |= DO_EXCLUSIVE;
        }
        if (deviceHasName) {
            deviceObject->Flags |= DO_DEVICE_HAS_NAME;
        }

        if(DeviceExtensionSize) {
            deviceObject->DeviceExtension = deviceObject + 1;
        } else {
            deviceObject->DeviceExtension = NULL;
        }

        deviceObject->StackSize = 1;

        //
        // This is a real device of some sort.  Allocate a spin lock
        // and initialize the device queue object in the device object.
        //

        KeInitializeDeviceQueue( &deviceObject->DeviceQueue );

        //
        // Insert the device object into the table.
        //

        status = ObInsertObject( deviceObject,
                                 &objectAttributes,
                                 1,
                                 &handle );

        if (NT_SUCCESS( status )) {

            //
            // The insert completed successfully.  Link the device object
            // and driver objects together.  Close the handle so that if
            // the driver is unloaded, the device object can go away.
            //

            deviceObject->DriverObject = DriverObject;

            NtClose( handle );

        } else {

            //
            // The insert operation failed.  Fortunately it dropped the
            // reference count on the device - since that was the last one
            // all the cleanup should be done for us.
            //

            //
            // indicate that no device object was created.
            //

            deviceObject = (PDEVICE_OBJECT) NULL;
        }
    }

    *DeviceObject = deviceObject;
    return status;
}

NTSTATUS
IoCreateFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG Disposition,
    IN ULONG CreateOptions,
    IN ULONG Options
    )

/*++

Routine Description:

    This is the common routine for both NtCreateFile and NtOpenFile to allow
    a user to create or open a file.  This procedure is also used internally
    by kernel mode components, such as the network server, to perform the
    same type of operation, but allows kernel mode code to force checking
    arguments and access to the file, rather than bypassing these checks
    because the code is running in kernel mode.

Arguments:

    FileHandle - A pointer to a variable to receive the handle to the open
        file.

    DesiredAccess - Supplies the types of access that the caller would like
        to the file.

    ObjectAttributes - Supplies the attributes to be used for the file object
        (name, SECURITY_DESCRIPTOR, etc.)

    IoStatusBlock - Specifies the address of the caller's I/O status block.

    AllocationSize - Initial size that should be allocated to the file.
        This parameter only has an affect if the file is created.  Further,
        if not specified, then it is taken to mean zero.

    FileAttributes - Specifies the attributes that should be set on the file,
        if it is created.

    ShareAccess - Supplies the types of share access that the caller would
        like to the file.

    Disposition - Supplies the method for handling the create/open.

    CreateOptions - Caller options for how to perform the create/open.

    Options - Specifies the options that are to be used during generation
        of the create IRP.

Return Value:

    The function value is the final status of the create/open operation.

--*/

{
    NTSTATUS status;
    HANDLE handle;
    OPEN_PACKET openPacket;
    BOOLEAN SuccessfulIoParse;
    LARGE_INTEGER initialAllocationSize;

    PAGED_CODE();

    if (Options & IO_CHECK_CREATE_PARAMETERS) {

        //
        // Check for any invalid parameters.
        //

        if (

            //
            // Check that no invalid file attributes flags were specified.
            //

//          (FileAttributes & ~FILE_ATTRIBUTE_VALID_SET_FLAGS)
            (FileAttributes & ~FILE_ATTRIBUTE_VALID_FLAGS)

            ||

            //
            // Check that no invalid share access flags were specified.
            //

            (ShareAccess & ~FILE_SHARE_VALID_FLAGS)

            ||

            //
            // Ensure that the disposition value is in range.
            //

            (Disposition > FILE_MAXIMUM_DISPOSITION)

            ||

            //
            // Check that no invalid create options were specified.
            //

            (CreateOptions & ~FILE_VALID_OPTION_FLAGS)

            ||

            //
            // If the caller specified synchronous I/O, then ensure that
            // (s)he also asked for synchronize desired access to the
            // file.
            //

            (CreateOptions & (FILE_SYNCHRONOUS_IO_ALERT | FILE_SYNCHRONOUS_IO_NONALERT) &&
            (!(DesiredAccess & SYNCHRONIZE)))

            ||

            //
            // Also, if the caller specified that the file is to be deleted
            // on close, then ensure that delete is specified as one of the
            // desired accesses requested.
            //

            (CreateOptions & FILE_DELETE_ON_CLOSE &&
            (!(DesiredAccess & DELETE)))

            ||

            //
            // Likewise, ensure that if one of the synchronous I/O modes
            // is specified that the other one is not specified as well.
            //

            ((CreateOptions & (FILE_SYNCHRONOUS_IO_ALERT | FILE_SYNCHRONOUS_IO_NONALERT)) ==
                              (FILE_SYNCHRONOUS_IO_ALERT | FILE_SYNCHRONOUS_IO_NONALERT))

            ||

            //
            // If this create or open is for a directory operation, check
            // that all of the other flags, dispositions, and desired
            // access parameters were also specified correctly.
            //
            // These are as follows:
            //
            //     o  No other flags other than the synchronous I/O flags,
            //        write-through, or open by file ID are set.
            //
            //     o  The disposition value is one of create, open, or
            //        open-if.
            //
            //     o  No non-directory accesses have been specified.
            //

            ((CreateOptions & FILE_DIRECTORY_FILE)
             && !(CreateOptions & FILE_NON_DIRECTORY_FILE)
             && ((CreateOptions & ~(FILE_DIRECTORY_FILE |
                                    FILE_SYNCHRONOUS_IO_ALERT |
                                    FILE_SYNCHRONOUS_IO_NONALERT |
                                    FILE_WRITE_THROUGH |
                                    FILE_COMPLETE_IF_OPLOCKED |
                                    FILE_OPEN_FOR_BACKUP_INTENT |
                                    FILE_DELETE_ON_CLOSE |
                                    FILE_OPEN_FOR_FREE_SPACE_QUERY |
                                    FILE_OPEN_BY_FILE_ID |
                                    FILE_OPEN_REPARSE_POINT))
                 || ((Disposition != FILE_CREATE)
                     && (Disposition != FILE_OPEN)
                     && (Disposition != FILE_OPEN_IF))
                )
            )

            ||

            //
            //  FILE_COMPLETE_IF_OPLOCK and FILE_RESERVE_OPFILTER are
            //  incompatible options.
            //

            ((CreateOptions & FILE_COMPLETE_IF_OPLOCKED) &&
             (CreateOptions & FILE_RESERVE_OPFILTER))

            ||

            //
            // Finally, if the no intermediate buffering option was
            // specified, ensure that the caller did not also request
            // append access to the file.
            //

            (CreateOptions & FILE_NO_INTERMEDIATE_BUFFERING &&
            (DesiredAccess & FILE_APPEND_DATA)) ) {

            return STATUS_INVALID_PARAMETER;
        }
    }

    //
    // The caller's mode is kernel.  Copy the input parameters to their
    // expected locations for later use.
    //

    if (ARGUMENT_PRESENT( AllocationSize )) {
        initialAllocationSize = *AllocationSize;
    } else {
        initialAllocationSize.QuadPart = 0;
    }

    //
    // Now fill in an Open Packet (OP) to be used in calling the device object
    // parse routine.  This packet will allow information to be passed between
    // this routine and the parse routine so that a common context may be kept.
    // For most services this would be done with an I/O Request Packet (IRP),
    // but this cannot be done here because the number of stack entries which
    // need to be allocated in the IRP is not yet known.
    //

    openPacket.Type = IO_TYPE_OPEN_PACKET;
    openPacket.Size = sizeof( OPEN_PACKET );
    openPacket.ParseCheck = 0L;
    openPacket.AllocationSize = initialAllocationSize;
    openPacket.CreateOptions = CreateOptions;
    openPacket.FileAttributes = (USHORT) FileAttributes;
    openPacket.ShareAccess = (USHORT) ShareAccess;
    openPacket.Disposition = Disposition;
    openPacket.QueryOnly = FALSE;
    openPacket.DeleteOnly = FALSE;
    openPacket.Options = Options;
    openPacket.RelatedFileObject = (PFILE_OBJECT) NULL;
    openPacket.DesiredAccess = DesiredAccess;

    //
    // Assume that the operation is going to be successful.
    //

    openPacket.FinalStatus = STATUS_SUCCESS;

    //
    // Zero the file object field in the OP so the parse routine knows that
    // this is the first time through.  For reparse operations it will continue
    // to use the same file object that it allocated the first time.
    //

    openPacket.FileObject = (PFILE_OBJECT) NULL;

    //
    // Attempt to open the file object by name.  This will yield the handle
    // that the user is to use as his handle to the file in all subsequent
    // calls, if it works.
    //
    // This call performs a whole lot of the work for actually getting every-
    // thing set up for the I/O system.  The object manager will take the name
    // of the file and will translate it until it reaches a device object (or
    // it fails).  If the former, then it will invoke the parse routine set up
    // by the I/O system for device objects.  This routine will actually end
    // up creating the file object, allocating an IRP, filling it in, and then
    // invoking the driver's dispatch routine with the packet.
    //

    status = ObOpenObjectByName( ObjectAttributes,
                                 (POBJECT_TYPE) NULL,
                                 &openPacket,
                                 &handle );

    //
    // Check the status of the open.  If it was not successful, cleanup and
    // get out.  Notice that it is also possible, because this code does not
    // explicitly request that a particular type of object (because the Object
    // Manager does not check when a parse routine is present and because the
    // name first refers to a device object and then a file object), a check
    // must be made here to ensure that what was returned was really a file
    // object.  The check is to see whether the device object parse routine
    // believes that it successfully returned a pointer to a file object.  If
    // it does, then OK;  otherwise, something went wrong somewhere.
    //

    SuccessfulIoParse = (BOOLEAN) (openPacket.ParseCheck == OPEN_PACKET_PATTERN);

    if (!NT_SUCCESS( status ) || !SuccessfulIoParse) {

        if (NT_SUCCESS( status )) {

            //
            // The operation was successful as far as the object system is
            // concerned, but the I/O system device parse routine was never
            // successfully completed so this operation has actually completed
            // with an error because of an object mismatch.  Therefore, this is
            // the wrong type of object so dereference whatever was actually
            // referenced by closing the handle that was created for it.
            // We have to do a NtClose as this handle can be a kernel handle if
            // IoCreateFile was called by a driver.
            //

            NtClose( handle );
            status = STATUS_OBJECT_TYPE_MISMATCH;
        }

        //
        // If the final status according to the device parse routine
        // indicates that the operation was not successful, then use that
        // routine's final status because it is more descriptive than the
        // status which was returned by the object manager.
        //

        if (!NT_SUCCESS( openPacket.FinalStatus )) {
            status = openPacket.FinalStatus;

            if (NT_WARNING( status )) {

                IoStatusBlock->Status = openPacket.FinalStatus;
                IoStatusBlock->Information = openPacket.Information;
            }

        } else if (openPacket.FileObject != NULL && !SuccessfulIoParse) {

            //
            // Otherwise, one of two things occurred:
            //
            //     1)  The parse routine was invoked at least once and a
            //         reparse was performed but the parse routine did not
            //         actually complete.
            //
            //     2)  The parse routine was successful so everything worked
            //         but the object manager incurred an error after the
            //         parse routine completed.
            //
            // For case #1, there is an outstanding file object that still
            // exists.  This must be cleaned up.
            //
            // For case #2, nothing must be done as the object manager has
            // already dereferenced the file object.  Note that this code is
            // not invoked if the parse routine completed with a successful
            // status return (SuccessfulIoParse is TRUE).
            //

            openPacket.FileObject->DeviceObject = (PDEVICE_OBJECT) NULL;
            ObDereferenceObject( openPacket.FileObject );
        }

    } else {

        //
        // At this point, the open/create operation has been successfully
        // completed.  There is a handle to the file object, which has been
        // created, and the file object has been signaled.
        //
        // The remaining work to be done is to complete the operation.  This is
        // performed as follows:
        //
        //    1.  The file object has been signaled, so no work needs to be done
        //        for it.
        //
        //    2.  The file handle is returned to the user.
        //
        //    3.  The I/O status block is written with the final status.
        //

        openPacket.FileObject->Flags |= FO_HANDLE_CREATED;

        ASSERT( openPacket.FileObject->Type == IO_TYPE_FILE );

        //
        // Return the file handle.
        //

        *FileHandle = handle;

        //
        // Write the I/O status into the caller's buffer.
        //

        IoStatusBlock->Information = openPacket.Information;
        IoStatusBlock->Status = openPacket.FinalStatus;
        status = openPacket.FinalStatus;
    }

    //
    // If the parse routine successfully created a file object then
    // derefence it here.
    //

    if (SuccessfulIoParse && openPacket.FileObject != NULL) {
        ObDereferenceObject( openPacket.FileObject );
    }

    return status;
}

NTSTATUS
IoCreateSymbolicLink(
    IN POBJECT_STRING SymbolicLinkName,
    IN POBJECT_STRING DeviceName
    )

/*++

Routine Description:

    This routine is invoked to assign a symbolic link name to a device.

Arguments:

    SymbolicLinkName - Supplies the symbolic link name as a Unicode string.

    DeviceName - Supplies the name to which the symbolic link name refers.

Return Value:

    The function value is the final status of the operation.

--*/

{
    OBJECT_ATTRIBUTES objectAttributes;
    HANDLE linkHandle;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Begin by initializing the object attributes for the symbolic link.
    //

    InitializeObjectAttributes( &objectAttributes,
                                SymbolicLinkName,
                                OBJ_PERMANENT | OBJ_CASE_INSENSITIVE,
                                (HANDLE) NULL,
                                (PSECURITY_DESCRIPTOR) NULL );

    //
    // Note that the following assignment can fail (because it is not system
    // initialization time and therefore the \ARCname directory does not
    // exist - if this is really a call to IoAssignArcName), but that is fine.
    //

    status = NtCreateSymbolicLinkObject( &linkHandle,
                                         &objectAttributes,
                                         DeviceName );
    if (NT_SUCCESS( status )) {
        NtClose( linkHandle );
    }

    return status;
}

VOID
IoDeleteDevice(
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine deletes the specified device object from the system so that
    it may no longer be referenced.  It is invoked when either the device
    driver is being unloaded from the system, or the driver's initialization
    routine failed to properly initialize the device or a fatal driver
    initialization error was encountered, or when the device is being removed
    from the system.

Arguments:

    DeviceObject - Pointer to the device object that is to be deleted.

Return Value:

    None.

--*/

{
    KIRQL irql;

    //
    // If this device has a name, then mark the
    // object as temporary so that when it is dereferenced it will be
    // deleted.
    //

    if (DeviceObject->Flags & DO_DEVICE_HAS_NAME) {
        ObMakeTemporaryObject( DeviceObject );
    }

    //
    // Mark the device object as deleted.
    //

    irql = KeRaiseIrqlToDpcLevel();

    DeviceObject->DeletePending = TRUE;

    if (!DeviceObject->ReferenceCount) {
        IopCompleteUnloadOrDelete( DeviceObject, irql );
    } else {
        KeLowerIrql(irql);
    }
}

NTSTATUS
IoDeleteSymbolicLink(
    IN POBJECT_STRING SymbolicLinkName
    )

/*++

Routine Description:

    This routine is invoked to remove a symbolic link from the system.  This
    generally occurs whenever a driver that has assigned a symbolic link needs
    to exit.  It can also be used when a driver no longer needs to redirect
    a name.

Arguments:

    SymbolicLinkName - Provides the Unicode name string to be deassigned.

Return Values:

    Status of operation.

--*/

{
    OBJECT_ATTRIBUTES objectAttributes;
    HANDLE linkHandle;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Begin by initializing the object attributes for the symbolic link.
    //

    InitializeObjectAttributes( &objectAttributes,
                                SymbolicLinkName,
                                OBJ_CASE_INSENSITIVE,
                                (HANDLE) NULL,
                                (PSECURITY_DESCRIPTOR) NULL );

    //
    // Open the symbolic link itself so that it can be marked temporary and
    // closed.
    //

    status = NtOpenSymbolicLinkObject( &linkHandle,
                                       &objectAttributes );
    if (NT_SUCCESS( status )) {

        //
        // The symbolic link was successfully opened.  Attempt to make it a
        // temporary object, and then close the handle.  This will cause the
        // object to go away.
        //

        status = NtMakeTemporaryObject( linkHandle );
        if (NT_SUCCESS( status )) {
            NtClose( linkHandle );
        }
    }


    return status;
}

NTSTATUS
IoDismountVolume(
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine dismounts the file system volume attached to the supplied
    device.

Arguments:

    DeviceObject - Specifies the device to be dismounted.

Return Value:

    Status of operation.

--*/

{
    NTSTATUS status;
    KIRQL OldIrql;
    PDEVICE_OBJECT MountedDeviceObject;

    //
    // Synchronize access to MountedOrSelfDevice with IoParseDevice by raising
    // to DISPATCH_LEVEL.
    //

    OldIrql = KeRaiseIrqlToDpcLevel();

    MountedDeviceObject = DeviceObject->MountedOrSelfDevice;

    if (MountedDeviceObject != NULL) {

        if (MountedDeviceObject->DriverObject->DriverDismountVolume != NULL) {
            MountedDeviceObject->ReferenceCount++;
            status = STATUS_SUCCESS;
        } else {
            MountedDeviceObject = NULL;
            status = STATUS_INVALID_DEVICE_REQUEST;
        }

    } else {
        status = STATUS_VOLUME_DISMOUNTED;
    }

    KeLowerIrql(OldIrql);

    //
    // If the supplied device has been mounted by a device with a dismount
    // volume routine, then invoke that routine.
    //

    if (MountedDeviceObject != NULL) {
        status = MountedDeviceObject->DriverObject->DriverDismountVolume(MountedDeviceObject);
        IopDecrementDeviceObjectRef(MountedDeviceObject);
    }

    return status;
}

NTSTATUS
IoDismountVolumeByName(
    IN POBJECT_STRING DeviceName
    )

/*++

Routine Description:

    This routine dismounts the file system volume attached to the device with
    supplied name.

Arguments:

    DeviceName - Specifies the name of the device to be dismounted.

Return Value:

    Status of operation.

--*/

{
    NTSTATUS status;
    PVOID DeviceObject;

    status = ObReferenceObjectByName(DeviceName, 0, &IoDeviceObjectType, NULL,
        &DeviceObject);

    if (NT_SUCCESS(status)) {
        status = IoDismountVolume((PDEVICE_OBJECT)DeviceObject);
        ObDereferenceObject(DeviceObject);
    }

    return status;
}

VOID
IoFreeIrp(
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine deallocates the specified I/O Request Packet.

Arguments:

    Irp - I/O Request Packet to deallocate.

Return Value:

    None.

--*/

{
    //
    // Ensure that the data structure being freed is really an IRP.
    //

    ASSERT( Irp->Type == IO_TYPE_IRP );

    if (Irp->Type != IO_TYPE_IRP) {
        KeBugCheckEx( MULTIPLE_IRP_COMPLETE_REQUESTS, (ULONG_PTR) Irp, __LINE__, 0, 0 );
    }

    ASSERT(IsListEmpty(&(Irp)->ThreadListEntry));
    Irp->Type = 0;

    //
    // Ensure that all of the owners of the IRP have at least been notified
    // that the request is going away.
    //

    ASSERT( Irp->CurrentLocation >= Irp->StackCount );

    //
    // Deallocate the IRP.
    //

    ExFreePool( Irp );
}

VOID
IoInitializeIrp(
    IN OUT PIRP Irp,
    IN USHORT PacketSize,
    IN CCHAR StackSize
    )

/*++

Routine Description:

    Initializes an IRP.

Arguments:

    Irp - a pointer to the IRP to initialize.

    PacketSize - length, in bytes, of the IRP.

    StackSize - Number of stack locations in the IRP.

Return Value:

    None.

--*/

{
    IopInitializeIrp(Irp, PacketSize, StackSize);
}

NTSTATUS
IoInvalidDeviceRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This function is the default dispatch routine for all driver entries
    not implemented by drivers that have been loaded into the system.  Its
    responsibility is simply to set the status in the packet to indicate
    that the operation requested is invalid for this device type, and then
    complete the packet.

Arguments:

    DeviceObject - Specifies the device object for which this request is
        bound.  Ignored by this routine.

    Irp - Specifies the address of the I/O Request Packet (IRP) for this
        request.

Return Value:

    The final status is always STATUS_INVALID_DEVICE_REQUEST.


--*/

{
    UNREFERENCED_PARAMETER( DeviceObject );

    //
    // Simply store the appropriate status, complete the request, and return
    // the same status stored in the packet.
    //

    Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    return STATUS_INVALID_DEVICE_REQUEST;
}

VOID
IoLockUserBuffer(
    IN OUT PIRP Irp,
    IN ULONG Length
    )

/*++

Routine Description:

    This routine locks Irp->UserBuffer using MmLockUnlockBufferPages and marks
    the IRP so that on packet cleanup, the buffer is unlocked.

Arguments:

    Irp - Pointer to IRP that describes the buffer to be locked.

    Length - Length, in bytes, of the buffer to be locked.

Return Value:

    None.

--*/

{
    ASSERT(Length != 0);

    //
    // If this is a scatter/gather operation or if the user buffer has already
    // been locked down, then we don't need to do anything.
    //

    if ((Irp->Flags & (IRP_SCATTER_GATHER_OPERATION | IRP_UNLOCK_USER_BUFFER)) == 0) {

        //
        // This isn't a scatter/gather operation, so the user buffer should have
        // been set to a valid pointer.
        //

        ASSERT(Irp->UserBuffer != NULL);

        MmLockUnlockBufferPages(Irp->UserBuffer, Length, FALSE);

        Irp->LockedBufferLength = Length;
        Irp->Flags |= IRP_UNLOCK_USER_BUFFER;

    } else {

        ASSERT(Irp->LockedBufferLength == Length);
    }
}

VOID
IoMarkIrpMustComplete(
    IN OUT PIRP Irp
    )

/*++

Routine Description:

    This routine marks the IRP as a must complete packet.  Must complete packets
    delay a system reset or shutdown until they are complete.

    This routine can only be called as DISPATCH_LEVEL.  The HAL code that relies
    on this variable only runs as a DPC.

Arguments:

    Irp - Pointer to IRP.

Return Value:

    None.

--*/

{
    PIO_STACK_LOCATION IrpSp;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // Only mark the current stack location once.
    //

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    if ((IrpSp->Control & SL_MUST_COMPLETE) == 0) {

        //
        // Mark the IRP so that we know that on completion of this IRP that the
        // must complete packet should be decremented.
        //

        IrpSp->Control |= SL_MUST_COMPLETE;

        //
        // Increment the number of must complete packets.
        //

        IoPendingMustCompletePackets++;
    }
}

NTSTATUS
IoQueryFileInformation(
    IN PFILE_OBJECT FileObject,
    IN FILE_INFORMATION_CLASS FileInformationClass,
    IN ULONG Length,
    OUT PVOID FileInformation,
    OUT PULONG ReturnedLength
    )

/*++

Routine Description:

    This routine returns the requested information about a specified file.
    The information returned is determined by the FileInformationClass that
    is specified, and it is placed into the caller's FileInformation buffer.

Arguments:

    FileObject - Supplies a pointer to the file object about which the requested
        information is returned.

    FileInformationClass - Specifies the type of information which should be
        returned about the file.

    Length - Supplies the length, in bytes, of the FileInformation buffer.

    FileInformation - Supplies a buffer to receive the requested information
        returned about the file.  This buffer must not be pageable and must
        reside in system space.

    ReturnedLength - Supplies a variable that is to receive the length of the
        information written to the FileInformation buffer.

Return Value:

    The status returned is the final completion status of the operation.

--*/

{
    PAGED_CODE();

    //
    // Simply invoke the common routine to perform the query operation.
    //

    return IopQueryXxxInformation( FileObject,
                                   FileInformationClass,
                                   Length,
                                   FileInformation,
                                   ReturnedLength,
                                   TRUE );
}

NTSTATUS
IoQueryVolumeInformation(
    IN PFILE_OBJECT FileObject,
    IN FS_INFORMATION_CLASS FsInformationClass,
    IN ULONG Length,
    OUT PVOID FsInformation,
    OUT PULONG ReturnedLength
    )

/*++

Routine Description:

    This routine returns the requested information about a specified volume.
    The information returned is determined by the FsInformationClass that
    is specified, and it is placed into the caller's FsInformation buffer.

Arguments:

    FileObject - Supplies a pointer to the file object about which the requested
        information is returned.

    FsInformationClass - Specifies the type of information which should be
        returned about the volume.

    Length - Supplies the length of the FsInformation buffer in bytes.

    FsInformation - Supplies a buffer to receive the requested information
        returned about the file.  This buffer must not be pageable and must
        reside in system space.

    ReturnedLength - Supplies a variable that is to receive the length of the
        information written to the FsInformation buffer.

Return Value:

    The status returned is the final completion status of the operation.

--*/

{
    PAGED_CODE();

    //
    // Simply invoke the common routine to perform the query operation.
    //

    return IopQueryXxxInformation( FileObject,
                                   FsInformationClass,
                                   Length,
                                   FsInformation,
                                   ReturnedLength,
                                   FALSE );
}

VOID
IoQueueThreadIrp(
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine queues the specified I/O Request Packet (IRP) to the current
    thread's IRP pending queue.  This queue locates all of the outstanding
    I/O requests for the thread.

Arguments:

    Irp - Pointer to the I/O Request Packet (IRP) to be queued.

Return Value:

    None.

--*/

{
    //
    // Simply queue the packet using the internal queueing routine.
    //

    IopQueueThreadIrp( Irp );
}

VOID
IoRemoveShareAccess(
    IN PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess
    )

/*++

Routine Description:

    This routine is invoked to remove the access and share access information
    in a file system Share Access structure for a given open instance.

Arguments:

    FileObject - Pointer to the file object of the current access being closed.

    ShareAccess - Pointer to the share access structure that describes
         how the file is currently being accessed.

Return Value:

    None.

--*/

{
    PAGED_CODE();

    //
    // If this accessor wanted some type of access other than READ_ or
    // WRITE_ATTRIBUTES, then account for the fact that he has closed the
    // file.  Otherwise, he hasn't been accounted for in the first place
    // so don't do anything.
    //

    if (FileObject->ReadAccess ||
        FileObject->WriteAccess ||
        FileObject->DeleteAccess) {

        //
        // Decrement the number of opens in the Share Access structure.
        //

        ShareAccess->OpenCount--;

        //
        // For each access type, decrement the appropriate count in the Share
        // Access structure.
        //

        if (FileObject->ReadAccess) {
            ShareAccess->Readers--;
        }

        if (FileObject->WriteAccess) {
            ShareAccess->Writers--;
        }

        if (FileObject->DeleteAccess) {
            ShareAccess->Deleters--;
        }

        //
        // For each shared access type, decrement the appropriate count in the
        // Share Access structure.
        //

        if (FileObject->SharedRead) {
            ShareAccess->SharedRead--;
        }

        if (FileObject->SharedWrite) {
            ShareAccess->SharedWrite--;
        }

        if (FileObject->SharedDelete) {
            ShareAccess->SharedDelete--;
        }
    }
}

VOID
IoSetShareAccess(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG DesiredShareAccess,
    IN OUT PFILE_OBJECT FileObject,
    OUT PSHARE_ACCESS ShareAccess
    )

/*++

Routine Description:

    This routine is invoked to set the access and share access information
    in a file system Share Access structure for the first open.

Arguments:

    DesiredAccess - Desired access of current open request.

    DesiredShareAccess - Shared access requested by current open request.

    FileObject - Pointer to the file object of the current open request.

    ShareAccess - Pointer to the share access structure that describes
         how the file is currently being accessed.

Return Value:

    None.

--*/

{
    PAGED_CODE();

    //
    // Set the access type in the file object for the current accessor.
    //

    FileObject->ReadAccess = (BOOLEAN) ((DesiredAccess & (FILE_EXECUTE
        | FILE_READ_DATA)) != 0);
    FileObject->WriteAccess = (BOOLEAN) ((DesiredAccess & (FILE_WRITE_DATA
        | FILE_APPEND_DATA)) != 0);
    FileObject->DeleteAccess = (BOOLEAN) ((DesiredAccess & DELETE) != 0);

    //
    // Check to see whether the current file opener would like to read,
    // write, or delete the file.  If so, account for it in the share access
    // structure; otherwise, skip it.
    //

    if (FileObject->ReadAccess ||
        FileObject->WriteAccess ||
        FileObject->DeleteAccess) {

        //
        // Only update the share modes if the user wants to read, write or
        // delete the file.
        //

        FileObject->SharedRead = (BOOLEAN) ((DesiredShareAccess & FILE_SHARE_READ) != 0);
        FileObject->SharedWrite = (BOOLEAN) ((DesiredShareAccess & FILE_SHARE_WRITE) != 0);
        FileObject->SharedDelete = (BOOLEAN) ((DesiredShareAccess & FILE_SHARE_DELETE) != 0);

        //
        // Set the Share Access structure open count.
        //

        ShareAccess->OpenCount = 1;

        //
        // Set the number of readers, writers, and deleters in the Share Access
        // structure.
        //

        ShareAccess->Readers = FileObject->ReadAccess;
        ShareAccess->Writers = FileObject->WriteAccess;
        ShareAccess->Deleters = FileObject->DeleteAccess;

        //
        // Set the number of shared readers, writers, and deleters in the Share
        // Access structure.
        //

        ShareAccess->SharedRead = FileObject->SharedRead;
        ShareAccess->SharedWrite = FileObject->SharedWrite;
        ShareAccess->SharedDelete = FileObject->SharedDelete;

    } else {

        //
        // No read, write, or delete access has been requested.  Simply zero
        // the appropriate fields in the structure so that the next accessor
        // sees a consistent state.
        //

        ShareAccess->OpenCount = 0;
        ShareAccess->Readers = 0;
        ShareAccess->Writers = 0;
        ShareAccess->Deleters = 0;
        ShareAccess->SharedRead = 0;
        ShareAccess->SharedWrite = 0;
        ShareAccess->SharedDelete = 0;
    }
}

VOID
IopStartNextPacket(
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is invoked to dequeue the next packet (IRP) from the
    specified device work queue and invoke the device driver's start I/O
    routine for it.  If the Cancelable paramter is TRUE, then the update of
    current IRP is synchronized using the cancel spinlock.

Arguments:

    DeviceObject - Pointer to device object itself.

    Cancelable - Indicates that IRPs in the device queue may be cancelable.

Return Value:

    None.

--*/

{
    PIRP irp;
    PKDEVICE_QUEUE_ENTRY packet;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // Clear the current IRP field before starting another request.
    //

    DeviceObject->CurrentIrp = (PIRP) NULL;

    //
    // Remove the next packet from the head of the queue.  If a packet was
    // found, then process it.
    //

    packet = KeRemoveDeviceQueue( &DeviceObject->DeviceQueue );

    if (packet) {
        irp = CONTAINING_RECORD( packet, IRP, Tail.Overlay.DeviceQueueEntry );

        //
        // A packet was located so make it the current packet for this
        // device.
        //

        DeviceObject->CurrentIrp = irp;

        //
        // Mark the device object as busy so that nested calls to
        // IoStartNextPacket(ByKey) cause the requested to be deferred until
        // we're out of this StartIo call.
        //

        DeviceObject->StartIoFlags |= DO_STARTIO_BUSY;

        //
        // Invoke the driver's start I/O routine for this packet.
        //

        DeviceObject->DriverObject->DriverStartIo( DeviceObject, irp );

        //
        // The device object's start I/O routine is no longer busy.
        //

        DeviceObject->StartIoFlags &= ~DO_STARTIO_BUSY;
    }
}

VOID
IopStartNextPacketByKey(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG Key
    )

/*++

Routine Description:

    This routine is invoked to dequeue the next packet (IRP) from the
    specified device work queue by key and invoke the device driver's start
    I/O routine for it.

Arguments:

    DeviceObject - Pointer to device object itself.

    Key - Specifics the Key used to remove the entry from the queue.

Return Value:

    None.

--*/

{
    PIRP                 irp;
    PKDEVICE_QUEUE_ENTRY packet;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // Clear the current IRP field before starting another request.
    //

    DeviceObject->CurrentIrp = (PIRP) NULL;

    //
    // Attempt to remove the indicated packet according to the key from the
    // device queue.  If one is found, then process it.
    //

    packet = KeRemoveByKeyDeviceQueue( &DeviceObject->DeviceQueue, Key );

    if (packet) {
        irp = CONTAINING_RECORD( packet, IRP, Tail.Overlay.DeviceQueueEntry );

        //
        // A packet was successfully located.  Make it the current packet
        // and invoke the driver's start I/O routine for it.
        //

        DeviceObject->CurrentIrp = irp;

        //
        // Mark the device object as busy so that nested calls to
        // IoStartNextPacket(ByKey) cause the requested to be deferred until
        // we're out of this StartIo call.
        //

        DeviceObject->StartIoFlags |= DO_STARTIO_BUSY;

        //
        // Invoke the driver's start I/O routine for this packet.
        //

        DeviceObject->DriverObject->DriverStartIo( DeviceObject, irp );

        //
        // The device object's start I/O routine is no longer busy.
        //

        DeviceObject->StartIoFlags &= ~DO_STARTIO_BUSY;
    }
}

VOID
IopStartNextPacketDeferred(
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is invoked to dequeue the next packet (IRP) from the
    specified device work queue and invoke the device driver's start I/O
    routine for it.

    This routine is invoked for deferred requests.  Deferred StartIo requests
    are used to prevent stack overflows that can occur when a StartIo routine
    calls IoStartNextPacket(ByKey) which recursively calls the StartIo routine.
    The nested IoStartNextPacket(ByKey) call is deferred until the topmost
    IoStartPacket or IoStartNextPacket(ByKey) is finished.

Arguments:

    DeviceObject - Pointer to device object itself.

Return Value:

    None.

--*/

{
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    ASSERT(!(DeviceObject->StartIoFlags & DO_STARTIO_BUSY));
    ASSERT(DeviceObject->StartIoFlags & DO_STARTIO_REQUESTED);

    do {

        //
        // Clear the request for a deferred StartIo call.
        //

        DeviceObject->StartIoFlags &= ~DO_STARTIO_REQUESTED;

        //
        // Make the deferred StartIo call.
        //

        if (DeviceObject->StartIoFlags & DO_STARTIO_REQUESTED_BYKEY) {
            IopStartNextPacketByKey(DeviceObject, DeviceObject->StartIoKey);
        } else {
            IopStartNextPacket(DeviceObject);
        }

    } while (DeviceObject->StartIoFlags & DO_STARTIO_REQUESTED);
}

VOID
IoStartNextPacket(
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is invoked to dequeue the next packet (IRP) from the
    specified device work queue and invoke the device driver's start I/O
    routine for it.

Arguments:

    DeviceObject - Pointer to device object itself.

Return Value:

    None.

--*/

{
    //
    // Check if we're nested inside a IoStartPacket or IoStartNextPacket(ByKey)
    // call.
    //

    if (!(DeviceObject->StartIoFlags & DO_STARTIO_BUSY)) {

        //
        // Any requests for a StartIo call should have already been handled in
        // the exit path for IoStartPacket and IoStartNextPacket(ByKey).
        //

        ASSERT(!(DeviceObject->StartIoFlags & DO_STARTIO_REQUESTED));

        //
        // We're not nested.  Start the next packet now.
        //

        IopStartNextPacket(DeviceObject);

        //
        // If IoStartNextPacket(ByKey) was called while we were in the above
        // call, then process the deferred StartIo request.
        //

        if (DeviceObject->StartIoFlags & DO_STARTIO_REQUESTED) {
            IopStartNextPacketDeferred(DeviceObject);
        }

    } else {

        //
        // A device can only call IoStartNextPacket(ByKey) once per StartIo
        // call (there can only be one current IRP).
        //

        ASSERT(!(DeviceObject->StartIoFlags & DO_STARTIO_REQUESTED));

        //
        // Set up the StartIo flags for a deferred call.
        //

        DeviceObject->StartIoFlags |= DO_STARTIO_REQUESTED;
        DeviceObject->StartIoFlags &= ~DO_STARTIO_REQUESTED_BYKEY;
    }
}

VOID
IoStartNextPacketByKey(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG Key
    )

/*++

Routine Description:

    This routine is invoked to dequeue the next packet (IRP) from the
    specified device work queue by key and invoke the device driver's start
    I/O routine for it.

Arguments:

    DeviceObject - Pointer to device object itself.

    Key - Specifics the Key used to remove the entry from the queue.

Return Value:

    None.

--*/

{
    //
    // Check if we're nested inside a IoStartPacket or IoStartNextPacket(ByKey)
    // call.
    //

    if (!(DeviceObject->StartIoFlags & DO_STARTIO_BUSY)) {

        //
        // Any requests for a StartIo call should have already been handled in
        // the exit path for IoStartPacket and IoStartNextPacket(ByKey).
        //

        ASSERT(!(DeviceObject->StartIoFlags & DO_STARTIO_REQUESTED));

        //
        // We're not nested.  Start the next packet now.
        //

        IopStartNextPacketByKey(DeviceObject, Key);

        //
        // If IoStartNextPacket(ByKey) was called while we were in the above
        // call, then process the deferred StartIo request.
        //

        if (DeviceObject->StartIoFlags & DO_STARTIO_REQUESTED) {
            IopStartNextPacketDeferred(DeviceObject);
        }

    } else {

        //
        // A device can only call IoStartNextPacket(ByKey) once per StartIo
        // call (there can only be one current IRP).
        //

        ASSERT(!(DeviceObject->StartIoFlags & DO_STARTIO_REQUESTED));

        //
        // Set up the StartIo flags for a deferred call.
        //

        DeviceObject->StartIoFlags |= (DO_STARTIO_REQUESTED |
            DO_STARTIO_REQUESTED_BYKEY);

        DeviceObject->StartIoKey = Key;
    }
}

VOID
IoStartPacket(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PULONG Key OPTIONAL
    )

/*++

Routine Description:

    This routine attempts to start the specified packet request (IRP) on the
    specified device.  If the device is already busy, then the packet is
    simply queued to the device queue.

Arguments:

    DeviceObject - Pointer to device object itself.

    Irp - I/O Request Packet which should be started on the device.

    Key - Key to be used in inserting packet into device queue;  optional
        (if not specified, then packet is inserted at the tail).

Return Value:

    None.

--*/

{
    KIRQL oldIrql;
    BOOLEAN i;

    //
    // Raise the IRQL of the processor to dispatch level for synchronization.
    //

    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    // If a key parameter was specified, then insert the request into the
    // work queue according to the key;  otherwise, simply insert it at the
    // tail.
    //

    if (Key) {
        i = KeInsertByKeyDeviceQueue( &DeviceObject->DeviceQueue,
                                      &Irp->Tail.Overlay.DeviceQueueEntry,
                                      *Key );
    } else {
        i = KeInsertDeviceQueue( &DeviceObject->DeviceQueue,
                                 &Irp->Tail.Overlay.DeviceQueueEntry );
    }

    //
    // If the packet was not inserted into the queue, then this request is
    // now the current packet for this device.  Indicate so by storing its
    // address in the current IRP field, and begin processing the request.
    //

    if (!i) {

        DeviceObject->CurrentIrp = Irp;

        //
        // Mark the device object as busy so that nested calls to
        // IoStartNextPacket(Ex) cause the requested to be deferred until we're
        // out of this StartIo call.
        //

        DeviceObject->StartIoFlags |= DO_STARTIO_BUSY;

        //
        // Invoke the driver's start I/O routine to get the request going on the device.
        //

        DeviceObject->DriverObject->DriverStartIo( DeviceObject, Irp );

        //
        // The device object's start I/O routine is no longer busy.
        //

        DeviceObject->StartIoFlags &= ~DO_STARTIO_BUSY;

        //
        // If IoStartNextPacket(ByKey) was called while we were in the above
        // call, then process the deferred StartIo request.
        //

        if (DeviceObject->StartIoFlags & DO_STARTIO_REQUESTED) {
            IopStartNextPacketDeferred(DeviceObject);
        }
    }

    //
    // Restore the IRQL back to its value upon entry to this function before
    // returning to the caller.
    //

    KeLowerIrql( oldIrql );
}

NTSTATUS
IoSynchronousDeviceIoControlRequest(
    IN ULONG IoControlCode,
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    OUT PULONG ReturnedOutputBufferLength OPTIONAL,
    IN BOOLEAN InternalDeviceIoControl
    )
/*++

Routine Description:

    This routine builds an IRP for a device I/O control function and sends it
    to the supplied device object.

Arguments:

    IoControlCode - Specifies the device I/O control code that is to be
        performed by the target device driver.

    DeviceObject - Specifies the target device on which the I/O control
        function is to be performed.

    InputBuffer - Optional pointer to an input buffer that is to be passed
        to the device driver.

    InputBufferLength - Length of the InputBuffer in bytes.  If the Input-
        Buffer parameter is not passed, this parameter must be zero.

    OutputBuffer - Optional pointer to an output buffer that is to be passed
        to the device driver.

    OutputBufferLength - Length of the OutputBuffer in bytes.  If the
        OutputBuffer parameter is not passed, this parameter must be zero.

    InternalDeviceIoControl - A BOOLEAN parameter that specifies whether
        the packet that gets generated should have a major function code
        of IRP_MJ_INTERNAL_DEVICE_CONTROL (the parameter is TRUE), or
        IRP_MJ_DEVICE_CONTROL (the parameter is FALSE).

Return Value:

    The function value is the final status of the operation.

--*/
{
    NTSTATUS status;
    KEVENT Event;
    IO_STATUS_BLOCK IoStatusBlock;
    PIRP Irp;

    KeInitializeEvent(&Event, NotificationEvent, FALSE);

    Irp = IoBuildDeviceIoControlRequest(IoControlCode, DeviceObject, InputBuffer,
        InputBufferLength, OutputBuffer, OutputBufferLength,
        InternalDeviceIoControl, &Event, &IoStatusBlock);

    if (Irp != NULL) {

        status = IoCallDriver(DeviceObject, Irp);

        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
            status = IoStatusBlock.Status;
        }

    } else {

        status = STATUS_INSUFFICIENT_RESOURCES;
    }

    if (ReturnedOutputBufferLength != NULL) {
        *ReturnedOutputBufferLength = IoStatusBlock.Information;
    }

    return status;
}

NTSTATUS
IoSynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL
    )
/*++

Routine Description:

    This routine builds an I/O Request Packet (IRP) suitable for a File System
    Driver (FSD) to use in requesting an I/O operation from a device driver
    and sends it to the supplied device object.

    The request must be one of the following request codes:

        IRP_MJ_READ
        IRP_MJ_WRITE
        IRP_MJ_FLUSH_BUFFERS
        IRP_MJ_SHUTDOWN

Arguments:

    MajorFunction - Function to be performed;  see previous list.

    DeviceObject - Pointer to device object on which the I/O will be performed.

    Buffer - Pointer to buffer to get data from or write data into.  This
        parameter is required for read/write, but not for flush or shutdown
        functions.

    Length - Length of buffer in bytes.  This parameter is required for
        read/write, but not for flush or shutdown functions.

    StartingOffset - Pointer to the offset on the disk to read/write from/to.
        This parameter is required for read/write, but not for flush or
        shutdown functions.

    OverrideVerifyVolume - A BOOLEAN parmeter that specifies whether the packet
        that gets generated should have the SL_OVERRIDE_VERIFY_VOLUME flag set.

Return Value:

    The function value is the final status of the operation.

--*/
{
    NTSTATUS status;
    KEVENT Event;
    IO_STATUS_BLOCK IoStatusBlock;
    PIRP Irp;

    KeInitializeEvent(&Event, NotificationEvent, FALSE);

    Irp = IoBuildSynchronousFsdRequest(MajorFunction, DeviceObject, Buffer,
        Length, StartingOffset, &Event, &IoStatusBlock);

    if (Irp != NULL) {

        status = IoCallDriver(DeviceObject, Irp);

        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
            status = IoStatusBlock.Status;
        }

    } else {

        status = STATUS_INSUFFICIENT_RESOURCES;
    }

    return status;
}
