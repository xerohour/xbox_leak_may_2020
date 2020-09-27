/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    obwait.c

Abstract:

    This module implements the generic wait system services.

--*/

#include "obp.h"

NTSTATUS
NtSignalAndWaitForSingleObjectEx (
    IN HANDLE SignalHandle,
    IN HANDLE WaitHandle,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    )

/*++

Routine Description:

    This function atomically signals the specified signal object and then
    waits until the specified wait object attains a state of Signaled.  An
    optional timeout can also be specified.  If a timeout is not specified,
    then the wait will not be satisfied until the wait object attains a
    state of Signaled.  If a timeout is specified, and the wait object has
    not attained a state of Signaled when the timeout expires, then the
    wait is automatically satisfied.  If an explicit timeout value of zero
    is specified, then no wait will occur if the wait cannot be satisfied
    immediately.  The wait can also be specified as alertable.

Arguments:

    SignalHandle - Supplies the handle of the signal object.

    WaitHandle  - Supplies the handle for the wait object.

    WaitMode  - Supplies the processor mode in which the wait is to occur.

    Alertable - Supplies a boolean value that specifies whether the wait
        is alertable.

    Timeout - Supplies an pointer to an absolute or relative time over
        which the wait is to occur.

Return Value:

    The wait completion status.  A value of STATUS_TIMEOUT is returned if a
    timeout occurred.  A value of STATUS_SUCCESS is returned if the specified
    object satisfied the wait.  A value of STATUS_ALERTED is returned if the
    wait was aborted to deliver an alert to the current thread.  A value of
    STATUS_USER_APC is returned if the wait was aborted to deliver a user
    APC to the current thread.

--*/

{
    PVOID RealObject;
    PVOID SignalObject;
    POBJECT_HEADER SignalObjectHeader;
    NTSTATUS Status;
    PVOID WaitObject;
    POBJECT_HEADER WaitObjectHeader;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    //
    //  Reference the signal object by handle.
    //

    Status = ObReferenceObjectByHandle( SignalHandle,
                                        NULL,
                                        &SignalObject );

    //
    //  If the reference was successful, then reference the wait object by
    //  handle.
    //

    if (NT_SUCCESS(Status)) {

        Status = ObReferenceObjectByHandle( WaitHandle,
                                            NULL,
                                            &WaitObject );

        //
        //  If the reference was successful, then determine the real wait
        //  object, check the signal object access, signal the signal object,
        //  and wait for the real wait object.
        //

        if (NT_SUCCESS(Status)) {

            WaitObjectHeader = OBJECT_TO_OBJECT_HEADER(WaitObject);
            RealObject = WaitObjectHeader->Type->DefaultObject;

            if ((LONG_PTR)RealObject >= 0) {

                RealObject = (PVOID)((PCHAR)WaitObject + (ULONG_PTR)RealObject);
            }

            //
            //  If the signal object is an event, then check for modify access
            //  and set the event.  Otherwise, if the signal object is a
            //  mutant, then attempt to release ownership of the mutant.
            //  Otherwise, if the object is a semaphore, then check for modify
            //  access and release the semaphore.  Otherwise, the signal objet
            //  is invalid.
            //

            SignalObjectHeader = OBJECT_TO_OBJECT_HEADER(SignalObject);
            Status = STATUS_ACCESS_DENIED;

            if (SignalObjectHeader->Type == &ExEventObjectType) {

                //
                //  Set the specified event and wait atomically.
                //

                KeSetEvent((PKEVENT)SignalObject, EVENT_INCREMENT, TRUE);

            } else if (SignalObjectHeader->Type == &ExMutantObjectType) {

                //
                //  Release the specified mutant and wait atomically.
                //
                //  N.B. The release will only be successful if the current
                //       thread is the owner of the mutant.
                //

                try {

                    KeReleaseMutant( (PKMUTANT)SignalObject,
                                     MUTANT_INCREMENT,
                                     FALSE,
                                     TRUE );

                } except(EXCEPTION_EXECUTE_HANDLER) {

                    Status = GetExceptionCode();

                    goto WaitExit;
                }

            } else if (SignalObjectHeader->Type == &ExSemaphoreObjectType) {

                //
                //  Release the specified semaphore and wait atomically.
                //

                try {

                    //
                    //  Release the specified semaphore and wait atomically.
                    //

                    KeReleaseSemaphore( (PKSEMAPHORE)SignalObject,
                                        SEMAPHORE_INCREMENT,
                                        1,
                                        TRUE );

                } except(EXCEPTION_EXECUTE_HANDLER) {

                    Status = GetExceptionCode();

                    goto WaitExit;
                }

            } else {

                Status = STATUS_OBJECT_TYPE_MISMATCH;

                goto WaitExit;
            }

            //
            //  Protect the wait call just in case KeWait decides to raise
            //  For example, a mutant level is exceeded
            //

            try {

                Status = KeWaitForSingleObject( RealObject,
                                                UserRequest,
                                                WaitMode,
                                                Alertable,
                                                Timeout );

            } except(EXCEPTION_EXECUTE_HANDLER) {

                Status = GetExceptionCode();
            }

WaitExit:

            ObDereferenceObject(WaitObject);
        }

        ObDereferenceObject(SignalObject);
    }

    return Status;
}

NTSTATUS
NtWaitForSingleObject (
    IN HANDLE Handle,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    )
{
    return NtWaitForSingleObjectEx (Handle, KernelMode, Alertable, Timeout);
}

NTSTATUS
NtWaitForSingleObjectEx (
    IN HANDLE Handle,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    )

/*++

Routine Description:

    This function waits until the specified object attains a state of
    Signaled.  An optional timeout can also be specified.  If a timeout
    is not specified, then the wait will not be satisfied until the
    object attains a state of Signaled.  If a timeout is specified, and
    the object has not attained a state of Signaled when the timeout
    expires, then the wait is automatically satisfied.  If an explicit
    timeout value of zero is specified, then no wait will occur if the
    wait cannot be satisfied immediately.  The wait can also be specified
    as alertable.

Arguments:

    Handle  - Supplies the handle for the wait object.

    WaitMode  - Supplies the processor mode in which the wait is to occur.

    Alertable - Supplies a boolean value that specifies whether the wait
        is alertable.

    Timeout - Supplies an pointer to an absolute or relative time over
        which the wait is to occur.

Return Value:

    The wait completion status. A value of STATUS_TIMEOUT is returned if a
    timeout occurred.  A value of STATUS_SUCCESS is returned if the specified
    object satisfied the wait.  A value of STATUS_ALERTED is returned if the
    wait was aborted to deliver an alert to the current thread. A value of
    STATUS_USER_APC is returned if the wait was aborted to deliver a user
    APC to the current thread.

--*/

{
    PVOID Object;
    POBJECT_HEADER ObjectHeader;
    NTSTATUS Status;
    PVOID WaitObject;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    //
    //  Get a referenced pointer to the specified object with synchronize
    //  access.
    //

    Status = ObReferenceObjectByHandle( Handle, NULL, &Object );

    //
    //  If access is granted, then check to determine if the specified object
    //  can be waited on.
    //

    if (NT_SUCCESS(Status)) {

        ObjectHeader = OBJECT_TO_OBJECT_HEADER( Object );
        WaitObject = ObjectHeader->Type->DefaultObject;

        if ((LONG_PTR)WaitObject >= 0) {

            WaitObject = (PVOID)((PCHAR)Object + (ULONG_PTR)WaitObject);
        }

        //
        //  Protect the wait call just in case KeWait decides to raise
        //  For example, a mutant level is exceeded
        //

        try {

            Status = KeWaitForSingleObject( WaitObject,
                                            UserRequest,
                                            WaitMode,
                                            Alertable,
                                            Timeout );

        } except(EXCEPTION_EXECUTE_HANDLER) {

            Status = GetExceptionCode();
        }

        ObDereferenceObject(Object);
    }

    return Status;
}

NTSTATUS
NtWaitForMultipleObjectsEx (
    IN ULONG Count,
    IN HANDLE Handles[],
    IN WAIT_TYPE WaitType,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    )

/*++

Routine Description:

    This function waits until the specified objects attain a state of
    Signaled.  The wait can be specified to wait until all of the objects
    attain a state of Signaled or until one of the objects attains a state
    of Signaled.  An optional timeout can also be specified.  If a timeout
    is not specified, then the wait will not be satisfied until the objects
    attain a state of Signaled.  If a timeout is specified, and the objects
    have not attained a state of Signaled when the timeout expires, then
    the wait is automatically satisfied.  If an explicit timeout value of
    zero is specified, then no wait will occur if the wait cannot be satisfied
    immediately.  The wait can also be specified as alertable.

Arguments:

    Count - Supplies a count of the number of objects that are to be waited
        on.

    Handles[] - Supplies an array of handles to wait objects.

    WaitType - Supplies the type of wait to perform (WaitAll, WaitAny).

    WaitMode  - Supplies the processor mode in which the wait is to occur.

    Alertable - Supplies a boolean value that specifies whether the wait is
        alertable.

    Timeout - Supplies a pointer to an optional absolute of relative time over
        which the wait is to occur.

Return Value:

    The wait completion status.  A value of STATUS_TIMEOUT is returned if a
    timeout occurred.  The index of the object (zero based) in the object
    pointer array is returned if an object satisfied the wait.  A value of
    STATUS_ALERTED is returned if the wait was aborted to deliver an alert
    to the current thread.  A value of STATUS_USER_APC is returned if the
    wait was aborted to deliver a user APC to the current thread.

--*/

{
    ULONG i;
    ULONG j;
    PVOID Object;
    POBJECT_HEADER ObjectHeader;
    PVOID Objects[MAXIMUM_WAIT_OBJECTS];
    ULONG RefCount;
    ULONG Size;
    NTSTATUS Status;
    PKWAIT_BLOCK WaitBlockArray;
    union {
        PVOID WaitObjects[MAXIMUM_WAIT_OBJECTS];
        struct {
            PVOID WaitObjectsSmall[OB_MAXIMUM_STACK_WAIT_BLOCKS];
            KWAIT_BLOCK WaitBlocks[OB_MAXIMUM_STACK_WAIT_BLOCKS];
        };
    } WaitState;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    //
    //  If the number of objects is zero or greater than the largest number
    //  that can be waited on, then return and invalid parameter status.
    //

    if ((Count == 0) || (Count > MAXIMUM_WAIT_OBJECTS)) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    //  If the wait type is not wait any or wait all, then return an invalid
    //  parameter status.
    //

    if ((WaitType != WaitAny) && (WaitType != WaitAll)) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    //  If the number of objects to be waited on is greater than the number
    //  of stack wait blocks, then allocate an array of wait blocks from
    //  nonpaged pool. If the wait block array cannot be allocated, then
    //  return insufficient resources.
    //

    if (Count > OB_MAXIMUM_STACK_WAIT_BLOCKS) {

        Size = Count * sizeof( KWAIT_BLOCK );
        WaitBlockArray = ExAllocatePoolWithTag(Size, 'tiaW');

        if (WaitBlockArray == NULL) {

            return STATUS_INSUFFICIENT_RESOURCES;
        }

    } else {

        WaitBlockArray = WaitState.WaitBlocks;
    }

    //
    //  Loop through the array of handles and get a referenced pointer to
    //  each object.
    //

    i = 0;
    RefCount = 0;

    Status = STATUS_SUCCESS;

    do {

        //
        //  Get the handle table entry
        //

        Object = ObpGetObjectHandleReference( Handles[i] );

        //
        //  Make sure the handle really did translate to a valid
        //  entry
        //

        if (Object != NULL) {

            RefCount += 1;
            Objects[i] = Object;

            //
            //  We have a object with proper access so get the header
            //  and if the default objects points to a real object
            //  then that is the one we're going to wait on.
            //  Otherwise we'll find the kernel wait object at an
            //  offset into the object body
            //

            ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);

            if ((LONG_PTR)ObjectHeader->Type->DefaultObject < 0) {

                WaitState.WaitObjects[i] = ObjectHeader->Type->DefaultObject;

            } else {

                //
                //  Compute the address of the kernel wait object.
                //

                WaitState.WaitObjects[i] = (PVOID)((PCHAR)&ObjectHeader->Body +
                    (ULONG_PTR)ObjectHeader->Type->DefaultObject);
            }

        } else {

            //
            //  The entry in the handle table isn't in use
            //

            Status = STATUS_INVALID_HANDLE;

            goto ServiceFailed;
        }

        i += 1;

    } while (i < Count);

    //
    //  At this point the WaitObjects[] is set to the kernel wait objects
    //
    //  Now Check to determine if any of the objects are specified more than
    //  once, but we only need to check this for wait all, with a wait any
    //  the user can specify the same object multiple times.
    //

    if (WaitType == WaitAll) {

        i = 0;

        do {

            for (j = i + 1; j < Count; j += 1) {

                if (WaitState.WaitObjects[i] == WaitState.WaitObjects[j]) {

                    Status = STATUS_INVALID_PARAMETER_MIX;

                    goto ServiceFailed;
                }
            }

            i += 1;

        } while (i < Count);
    }

    //
    //  Wait for the specified objects to attain a state of Signaled or a
    //  time out to occur.  Protect the wait call just in case KeWait decides
    //  to raise For example, a mutant level is exceeded
    //

    try {

        Status = KeWaitForMultipleObjects( Count,
                                           WaitState.WaitObjects,
                                           WaitType,
                                           UserRequest,
                                           WaitMode,
                                           Alertable,
                                           Timeout,
                                           WaitBlockArray );

    } except(EXCEPTION_EXECUTE_HANDLER) {

        Status = GetExceptionCode();
    }

    //
    //  If any objects were referenced, then deference them.
    //

ServiceFailed:

    while (RefCount > 0) {

        RefCount -= 1;
        ObDereferenceObject(Objects[RefCount]);
    }

    //
    //  If a wait block array was allocated, then deallocate it.
    //

    if (WaitBlockArray != WaitState.WaitBlocks) {

        ExFreePool(WaitBlockArray);
    }

    return Status;
}
