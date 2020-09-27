/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    parse.c

Abstract:

    This module contains the code to implement the device object parse routine.

--*/

#include "iop.h"

//
// Define external procedures not in common header files
//

NTSTATUS
RawxMountVolume(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
FatxMountVolume(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
GdfxMountVolume(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
UdfxMountVolume(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IopMountVolume(
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN AllowRawMount
    )

/*++

Routine Description:

    This routine is used to mount a volume on the specified device.

Arguments:

    DeviceObject - Pointer to device object on which the volume is to be
        mounted.

    AllowRawMount - This parameter tells us if we should continue our
        filesystem search to include the Raw file system.  This flag will
        only be passed in as TRUE as a result of a DASD open.

Return Value:

    The function value is a successful status code if a volume was successfully
    mounted on the device.  Otherwise, an error code is returned.

--*/

{
    NTSTATUS status;

    //
    // Obtain the lock for the device to be mounted.  This guarantees that
    // only one thread is attempting to mount this particular device at a time.
    //

    status = KeWaitForSingleObject( &DeviceObject->DeviceLock,
                                    Executive,
                                    KernelMode,
                                    FALSE,
                                    (PLARGE_INTEGER) NULL );

    //
    // Check if the device is still in the unmounted state.
    //

    if (DeviceObject->MountedOrSelfDevice == NULL) {

        //
        // Check if the device supports high level file systems and if so,
        // attempt to mount an appropriate file system.
        //

        if ((DeviceObject->Flags & DO_RAW_MOUNT_ONLY) == 0) {

            if (DeviceObject->DeviceType == FILE_DEVICE_CD_ROM) {

                status = GdfxMountVolume(DeviceObject);

                if (status == STATUS_UNRECOGNIZED_VOLUME) {
                    status = UdfxMountVolume(DeviceObject);
                }

            } else {
                status = FatxMountVolume(DeviceObject);
            }

        } else {
            status = STATUS_UNRECOGNIZED_VOLUME;
        }

        //
        // If the volume hasn't been recognized yet and we're allowed to mount
        // the volume as a RAW device, then do so.
        //

        if (!NT_SUCCESS(status) && AllowRawMount) {
            status = RawxMountVolume(DeviceObject);
        }

    } else {

        //
        // The volume for this device has already been mounted.  Return a
        // success code.
        //

        status = STATUS_SUCCESS;
    }

    //
    // Release the synchronization event for the device.
    //

    KeSetEvent( &DeviceObject->DeviceLock, 0, FALSE );

    return status;
}

NTSTATUS
IoParseDevice(
    IN PVOID ParseObject,
    IN POBJECT_TYPE ObjectType,
    IN ULONG Attributes,
    IN OUT POBJECT_STRING CompleteName,
    IN OUT POBJECT_STRING RemainingName,
    IN OUT PVOID Context OPTIONAL,
    OUT PVOID *Object
    )

/*++

Routine Description:

    This routine interfaces to the NT Object Manager.  It is invoked when
    the object system is given the name of an entity to create or open and the
    name translates to a device object.  This routine is specified as the parse
    routine for all device objects.

    In the normal case of an NtCreateFile, the user specifies either the name
    of a device or of a file.  In the former situation, this routine is invoked
    with a pointer to the device and a null ("") string.  For this case, the
    routine simply allocates an IRP, fills it in, and passes it to the driver
    for the device.  The driver will then perform whatever rudimentary functions
    are necessary and will return a status code indicating whether an error was
    incurred.  This status code is remembered in the Open Packet (OP).

    In the latter situation, the name string to be opened/created is non-null.
    That is, it contains the remainder of the pathname to the file that is to
    be opened or created.  For this case, the routine allocates an IRP, fills
    it in, and passes it to the driver for the device.  The driver may then
    need to take further action or it may complete the request immediately.  If
    it needs to perform some work asynchronously, then it can queue the request
    and return a status of STATUS_PENDING.  This allows this routine and its
    caller to return to the user so that he can continue.  Otherwise, the open/
    create is basically finished.

    If the driver supports symbolic links, then it is also possible for the
    driver to return a new name.  This name will be returned to the Object
    Manager as a new name to look up.  The parsing will then begin again from
    the start.

    It is also the responsibility of this routine to create a file object for
    the file, if the name specifies a file.  The file object's address is
    returned to the NtCreateFile service through the OP.

Arguments:

    ParseObject - Pointer to the device object the name translated into.

    ObjectType - Type of the object being opened.

    AccessState - Running security access state information for operation.

    Attributes - Attributes to be applied to the object.

    CompleteName - Complete name of the object.

    RemainingName - Remaining name of the object.

    Context - Pointer to an Open Packet (OP) from NtCreateFile service.

    Object - The address of a variable to receive the created file object, if
        any.

Return Value:

    The function return value is one of the following:

        a)  Success - This indicates that the function succeeded and the object
            parameter contains the address of the created file object.

        b)  Error - This indicates that the file was not found or created and
            no file object was created.

        c)  Reparse - This indicates that the remaining name string has been
            replaced by a new name that is to be parsed.

--*/

{

    PIRP irp;
    PIO_STACK_LOCATION irpSp;
    POPEN_PACKET op;
    PFILE_OBJECT fileObject;
    NTSTATUS status;
    IO_STATUS_BLOCK ioStatus;
    PDEVICE_OBJECT deviceObject;
    PDEVICE_OBJECT parseDeviceObject;
    PDUMMY_FILE_OBJECT localFileObject;
    LOGICAL realFileObjectRequired;
    KIRQL OldIrql;

    PAGED_CODE();

    //
    // Assume failure by setting the returned object pointer to NULL.
    //

    *Object = (PVOID) NULL;

    //
    // Get the address of the Open Packet (OP).
    //

    op = Context;

    //
    // If no open packet was specified and there's no more name to parse, then
    // return the device object itself.
    //
    // We hit this path if somebody calls ObReferenceObjectByName (or
    // equivalent).  This allows the client to access a device object without
    // going through a file system.
    //

    if (op == NULL && RemainingName->Length == 0) {
        ObReferenceObject(ParseObject);
        *Object = ParseObject;
        return STATUS_SUCCESS;
    }

    //
    // Ensure that this routine is actually being invoked because someone is
    // attempting to open a device or a file through NtCreateFile.  This code
    // must be invoked from there (as opposed to some other random object
    // create or open routine).
    //

    if (op == NULL ||
        op->Type != IO_TYPE_OPEN_PACKET ||
        op->Size != sizeof( OPEN_PACKET )) {

        return STATUS_OBJECT_TYPE_MISMATCH;
    }

    //
    // Obtain a pointer to the parse object as a device object, which is the
    // actual type of the object anyway.
    //

    parseDeviceObject = (PDEVICE_OBJECT) ParseObject;

    //
    // If this is a relative open, then get the device on which the file
    // is really being opened from the related file object and use that for
    // the remainder of this function and for all operations performed on
    // the file object that is about to be created.
    //

    if (op->RelatedFileObject) {
        parseDeviceObject = op->RelatedFileObject->DeviceObject;
    }

    //
    // Make sure that the device and its driver are really there.
    //
    // Note that it is possible to "open" an exclusive device more than once
    // provided that the caller is performing a relative open.  This feature
    // is how users "allocate" a device, and then use it to perform operations.
    //

    OldIrql = KeRaiseIrqlToDpcLevel();

    if (parseDeviceObject->DeletePending ||
        parseDeviceObject->Flags & DO_DEVICE_INITIALIZING) {

        status = STATUS_NO_SUCH_DEVICE;

    } else if (parseDeviceObject->Flags & DO_EXCLUSIVE &&
               parseDeviceObject->ReferenceCount != 0 &&
               op->RelatedFileObject == NULL) {

        status = STATUS_ACCESS_DENIED;

    } else {

        status = STATUS_SUCCESS;
    }

    if (!NT_SUCCESS(status)) {
        KeLowerIrql(OldIrql);
        return op->FinalStatus = status;
    }

    //
    // Get the address of the mounted or self device object.  If the pointer
    // is null, then the device has not been mounted yet, so do it now.
    //

    while ((deviceObject = parseDeviceObject->MountedOrSelfDevice) == NULL) {

        KeLowerIrql(OldIrql);

        status = IopMountVolume( parseDeviceObject,
                                 (BOOLEAN) (!RemainingName->Length && !op->RelatedFileObject));

        if (!NT_SUCCESS(status)) {
            return op->FinalStatus = status;
        }

        OldIrql = KeRaiseIrqlToDpcLevel();
    }

    deviceObject->ReferenceCount++;
    KeLowerIrql(OldIrql);

    //
    // After this point, we shouldn't touch parseDeviceObject.  The device that
    // we're going to send the IRP_MJ_CREATE to is deviceObject.  That may be a
    // file system device.  A file system device could be unmounted by another
    // thread while we're trying to create a file.  We have a stable reference
    // to the file system device, but the link between the original
    // parseDeviceObject and deviceObject may no longer exist.
    //

    parseDeviceObject = NULL;

    //
    // Since ObOpenObjectByName is called without being passed
    // any object type information, we need to map the generic
    // bits in the DesiredAccess mask here.
    //

    RtlMapGenericMask( &op->DesiredAccess, &IopFileMapping );

    //
    // Allocate and fill in the I/O Request Packet (IRP) to use in interfacing
    // to the driver.
    //

    irp = IoAllocateIrp( deviceObject->StackSize );
    if (!irp) {
        IopDecrementDeviceObjectRef(deviceObject);
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    irp->Tail.Overlay.Thread = PsGetCurrentThread();
    irp->Flags = IRP_CREATE_OPERATION | IRP_SYNCHRONOUS_API | IRP_DEFER_IO_COMPLETION;

    //
    // Get a pointer to the stack location for the first driver.  This is where
    // the original function codes and parameters are passed.
    //

    irpSp = IoGetNextIrpStackLocation( irp );

    //
    // This is a normal file open or create function.
    //

    irpSp->MajorFunction = IRP_MJ_CREATE;
    irpSp->Flags = (UCHAR) op->Options;
    if (!(Attributes & OBJ_CASE_INSENSITIVE)) {
        irpSp->Flags |= SL_CASE_SENSITIVE;
    }

    //
    // Also fill in the NtCreateFile service's caller's parameters.
    //

    irp->Overlay.AllocationSize = op->AllocationSize;
    irpSp->Parameters.Create.Options = (op->Disposition << 24) | (op->CreateOptions & 0x00ffffff);
    irpSp->Parameters.Create.FileAttributes = op->FileAttributes;
    irpSp->Parameters.Create.ShareAccess = op->ShareAccess;
    irpSp->Parameters.Create.DesiredAccess = op->DesiredAccess;
    irpSp->Parameters.Create.RemainingName = RemainingName;

    //
    // Fill in local parameters so this routine can determine when the I/O is
    // finished, and the normal I/O completion code will not get any errors.
    //

    irp->UserIosb = &ioStatus;

    //
    // Allocate and initialize the file object that will be used in dealing
    // with the device for the remainder of this session with the user.  How
    // the file object is allocated is based on whether or not a real file
    // object is actually required.  It is not required for the query and
    // delete only operations.
    //

    realFileObjectRequired = !(op->QueryOnly || op->DeleteOnly);

    if (realFileObjectRequired) {

        OBJECT_ATTRIBUTES objectAttributes;

        //
        // A real, full-blown file object is actually required.
        //

        InitializeObjectAttributes( &objectAttributes,
                                    (POBJECT_STRING) NULL,
                                    Attributes,
                                    (HANDLE) NULL,
                                    (PSECURITY_DESCRIPTOR) NULL
                                  );

        status = ObCreateObject( &IoFileObjectType,
                                 &objectAttributes,
                                 (ULONG) sizeof( FILE_OBJECT ),
                                 (PVOID *) &fileObject );

        if (!NT_SUCCESS( status )) {
            IoFreeIrp( irp );
            IopDecrementDeviceObjectRef(deviceObject);
            return op->FinalStatus = status;
        }

        RtlZeroMemory( fileObject, sizeof( FILE_OBJECT ) );
        fileObject->Type = IO_TYPE_FILE;
        fileObject->RelatedFileObject = op->RelatedFileObject;
        if (op->CreateOptions & (FILE_SYNCHRONOUS_IO_ALERT | FILE_SYNCHRONOUS_IO_NONALERT)) {
            fileObject->Flags = FO_SYNCHRONOUS_IO;
            if (op->CreateOptions & FILE_SYNCHRONOUS_IO_ALERT) {
                fileObject->Flags |= FO_ALERTABLE_IO;
            }
        }

        //
        // Now fill in the file object as best is possible at this point and set
        // a pointer to it in the IRP so everyone else can find it.
        //

        if (fileObject->Flags & FO_SYNCHRONOUS_IO) {
            fileObject->LockCount = -1;
            KeInitializeEvent( &fileObject->Lock, SynchronizationEvent, FALSE );
        }
        if (op->CreateOptions & FILE_NO_INTERMEDIATE_BUFFERING) {
            fileObject->Flags |= FO_NO_INTERMEDIATE_BUFFERING;
        }
        if (op->CreateOptions & FILE_SEQUENTIAL_ONLY) {
            fileObject->Flags |= FO_SEQUENTIAL_ONLY;
        }
        if (op->CreateOptions & FILE_RANDOM_ACCESS) {
            fileObject->Flags |= FO_RANDOM_ACCESS;
        }

    } else {

        //
        // This is either a quick delete or query operation.  For these cases,
        // it is possible to optimize the Object Manager out of the picture by
        // simply putting together something that "looks" like a file object,
        // and then operating on it.
        //

        localFileObject = op->LocalFileObject;
        RtlZeroMemory( localFileObject, sizeof( DUMMY_FILE_OBJECT ) );
        fileObject = (PFILE_OBJECT) &localFileObject->ObjectHeader.Body;
        localFileObject->ObjectHeader.Type = &IoFileObjectType;
        localFileObject->ObjectHeader.PointerCount = 1;
    }

    fileObject->Type = IO_TYPE_FILE;
    fileObject->RelatedFileObject = op->RelatedFileObject;
    fileObject->DeviceObject = deviceObject;

    irp->Tail.Overlay.OriginalFileObject = fileObject;
    irpSp->FileObject = fileObject;

    //
    // Finally, initialize the file object's event to the Not Signaled state
    // and remember that a file object was created.
    //

    KeInitializeEvent( &fileObject->Event, NotificationEvent, FALSE );
    op->FileObject = fileObject;

    //
    // Insert the packet at the head of the IRP list for the thread.
    //

    IopQueueThreadIrp( irp );

    //
    // Now invoke the driver itself to open the file.
    //

    status = IoCallDriver( deviceObject, irp );

    //
    // One of three things may have happened when the driver was invoked:
    //
    //    1.  The I/O operation is pending (Status == STATUS_PENDING).  This can
    //        occur on devices which need to perform some sort of device
    //        manipulation (such as opening a file for a file system).
    //
    //    2.  The driver returned an error (Status < 0). This occurs when either
    //        a supplied parameter was in error, or the device or file system
    //        incurred or discovered an error.
    //
    //    3.  The operation is complete and was successful (Status ==
    //        STATUS_SUCCESS).  Note that for this case the only action is to
    //        return a pointer to the file object.
    //

    if (status == STATUS_PENDING) {

        KeWaitForSingleObject( &fileObject->Event,
                               Executive,
                               KernelMode,
                               FALSE,
                               (PLARGE_INTEGER) NULL );
        status = ioStatus.Status;

    } else {

        //
        // The I/O operation was completed without returning a status of
        // pending.  This means that at this point, the IRP has not been
        // fully completed.  Complete it now.
        //

        KIRQL irql;

        ASSERT( !irp->PendingReturned );

        KeRaiseIrql( APC_LEVEL, &irql );

        //
        // Note that normally the system would simply call IopCompleteRequest
        // here to complete the packet.  However, because this is a create
        // operation, several assumptions can be made that make it much faster
        // to perform the couple of operations that completing the request
        // would perform.  These include:  copying the I/O status block,
        // dequeueing the IRP and freeing it, and setting the file object's
        // event to the signalled state.  The latter is done here by hand,
        // since it is known that it is not possible for any thread to be
        // waiting on the event.
        //

        ioStatus = irp->IoStatus;
        status = ioStatus.Status;

        fileObject->Event.Header.SignalState = 1;

        IopDequeueThreadIrp( irp );

        IoFreeIrp( irp );

        KeLowerIrql( irql );
    }

    ASSERT(status != STATUS_REPARSE);

    //
    // Copy the information field of the I/O status block back to the
    // original caller in case it is required.
    //

    op->Information = ioStatus.Information;

    if (!NT_SUCCESS( status )) {

        //
        // The operation ended in an error.  Kill the file object, dereference
        // the device object, and return a null pointer.
        //

        fileObject->DeviceObject = (PDEVICE_OBJECT) NULL;

        if (realFileObjectRequired) {
            ObDereferenceObject( fileObject );
        }
        op->FileObject = (PFILE_OBJECT) NULL;

        IopDecrementDeviceObjectRef(deviceObject);

        return op->FinalStatus = status;
    }

    //
    // The operation was successful. If this is not a query or a delete,
    // but rather a normal open/create, return the address of the
    // FileObject to the caller and set the information returned in the
    // original requestor's I/O status block. Also set the value of the
    // parse check field in the open packet to a value which will let
    // the caller know that this routine was successful in creating the
    // file object. Finally, return the status of the operation to the
    // caller.
    //

    if (realFileObjectRequired) {

        *Object = fileObject;
        op->ParseCheck = OPEN_PACKET_PATTERN;

        //
        // Add a reference so the file object cannot go away before
        // the create routine gets chance to flag the object for handle
        // create.
        //

        ObReferenceObject( fileObject );

        return op->FinalStatus = ioStatus.Status;

    } else {

        //
        // This is either a quick query or delete operation.  Determine
        // which it is and quickly perform the operation.
        //

        if (op->QueryOnly) {

            ULONG returnedLength;

            //
            // This is a full attribute query.  Attempt to obtain the
            // full network attributes for the file.  This includes
            // both the basic and standard information about the
            // file.
            //

            status = IoQueryFileInformation(
                        fileObject,
                        FileNetworkOpenInformation,
                        sizeof( FILE_NETWORK_OPEN_INFORMATION ),
                        op->NetworkInformation,
                        &returnedLength
                        );

        } else {

            //
            // There is nothing to do for a quick delete since the caller
            // set the FILE_DELETE_ON_CLOSE CreateOption so it is already
            // set in the file system.
            //

            NOTHING;
        }

        op->ParseCheck = OPEN_PACKET_PATTERN;
        IopDeleteFile( fileObject );
        op->FileObject = (PFILE_OBJECT) NULL;
        op->FinalStatus = status;

        //
        // Return an error code here so that the object manager doesn't
        // think that we actually created an object.  The query and delete
        // code end up looking at the open packet to determine whether or
        // not the operation actually worked.
        //

        return STATUS_OBJECT_NAME_NOT_FOUND;
    }
}

NTSTATUS
IopParseFile(
    IN PVOID ParseObject,
    IN POBJECT_TYPE ObjectType,
    IN ULONG Attributes,
    IN OUT POBJECT_STRING CompleteName,
    IN OUT POBJECT_STRING RemainingName,
    IN OUT PVOID Context OPTIONAL,
    OUT PVOID *Object
    )

/*++

Routine Description:

    This routine interfaces to the NT Object Manager.  It is invoked when
    the object system is given the name of an entity to create or open and is
    also given a handle to a directory file object that the operation is to be
    performed relative to.  This routine is specified as the parse routine for
    all file objects.

    This routine simply invokes the parse routine for the appropriate device
    that is associated with the file object.  It is the responsibility of that
    routine to perform the operation.

Arguments:

    ParseObject - Pointer to the file object that the name is to be opened or
        created relative to.

    ObjectType - Type of the object being opened.

    Attributes - Attributes to be applied to the object.

    CompleteName - Complete name of the object.

    RemainingName - Remaining name of the object.

    Context - Pointer to an Open Packet (OP) from NtCreateFile service.

    Object - The address of a variable to receive the created file object, if
        any.

Return Value:

    The function return value is one of the following:

        a)  Success - This indicates that the function succeeded and the object
            parameter contains the address of the created file object.

        b)  Error - This indicates that the file was not found or created and
            no file object was created.

        c)  Reparse - This indicates that the remaining name string has been
            replaced by a new name that is to be parsed.

--*/

{
    PFILE_OBJECT fileObject;
    POPEN_PACKET op;

    PAGED_CODE();

    //
    // Get the address of the Open Packet (OP).
    //

    op = (POPEN_PACKET) Context;

    //
    // Ensure that this routine is actually being invoked because someone is
    // attempting to open a device or a file through NtCreateFile.  This code
    // must be invoked from there (as opposed to some other random object
    // create or open routine).
    //

    if (op == NULL ||
        op->Type != IO_TYPE_OPEN_PACKET ||
        op->Size != sizeof( OPEN_PACKET )) {
        return STATUS_OBJECT_TYPE_MISMATCH;
    }

    fileObject = (PFILE_OBJECT) ParseObject;

    //
    // Pass the related file object to the device object parse routine.
    //

    op->RelatedFileObject = fileObject;

    //
    // Open or create the specified file.
    //

    return IoParseDevice( fileObject->DeviceObject,
                          ObjectType,
                          Attributes,
                          CompleteName,
                          RemainingName,
                          Context,
                          Object );
}
