/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    timer.c

Abstract:

    This module implements the executive timer object. Functions are provided
    to create, open, cancel, set, and query timer objects.

--*/

#include "exp.h"

//
// Define private function prototypes.
//

VOID
ExpDeleteTimer (
    IN PVOID Object
    );

//
// Timer object type information.
//

DECLSPEC_RDATA OBJECT_TYPE ExTimerObjectType = {
    ExAllocatePoolWithTag,
    ExFreePool,
    NULL,
    ExpDeleteTimer,
    NULL,
    (PVOID)FIELD_OFFSET(KTIMER, Header),
    'emiT'
};

//
// Executive timer object structure definition.
//

typedef struct _ETIMER {
    KTIMER KeTimer;
    KAPC TimerApc;
    KDPC TimerDpc;
    LIST_ENTRY ActiveTimerListEntry;
    LONG Period;
    BOOLEAN ApcAssociated;
} ETIMER, *PETIMER;

//
// Define macros to lock and unlock the various timer lists.
//

#define ExpAcquireTimerLock(TIMER, OldIrql) \
    *(OldIrql) = KeRaiseIrqlToDpcLevel()

#define ExpReleaseTimerLock(TIMER, OldIrql) \
    KeLowerIrql(OldIrql)

#define ExpAcquireThreadActiveTimerListLock(THREAD, OldIrql) \
    *(OldIrql) = KeRaiseIrqlToDpcLevel()

#define ExpReleaseThreadActiveTimerListLock(THREAD, OldIrql) \
    KeLowerIrql(OldIrql)

#define ExpAcquireThreadActiveTimerListLockAtDpcLevel(THREAD) \
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL)

#define ExpReleaseThreadActiveTimerListLockFromDpcLevel(THREAD)

VOID
ExpTimerApcRoutine (
    IN PKAPC Apc,
    IN PKNORMAL_ROUTINE *NormalRoutine,
    IN PVOID *NormalContext,
    IN PVOID *SystemArgument1,
    IN PVOID *SystemArgument2
    )

/*++

Routine Description:

    This function is the special APC routine that is called to remove
    a timer from the current thread's active timer list.

Arguments:

    Apc - Supplies a pointer to the APC object used to invoke this routine.

    NormalRoutine - Supplies a pointer to a pointer to the normal routine
        function that was specified when the APC was initialized.

    NormalContext - Supplies a pointer to a pointer to an arbitrary data
        structure that was specified when the APC was initialized.

    SystemArgument1, SystemArgument2 - Supplies a set of two pointers to
        two arguments that contain untyped data.

Return Value:

    None.

--*/

{

    BOOLEAN Dereference;
    PETHREAD ExThread;
    PETIMER ExTimer;
    KIRQL OldIrql;

    //
    // Get address of executive timer object and the current thread object.
    //

    ExThread = PsGetCurrentThread();
    ExTimer = CONTAINING_RECORD(Apc, ETIMER, TimerApc);

    //
    // If the timer is still in the current thread's active timer list, then
    // remove it if it is not a periodic timer and set APC associated FALSE.
    // It is possible for the timer not to be in the current thread's active
    // timer list since the APC could have been delivered, and then another
    // thread could have set the timer again with another APC. This would
    // have caused the timer to be removed from the current thread's active
    // timer list.
    //
    // N. B. The spin locks for the timer and the active timer list must be
    //  acquired in the order: 1) timer lock, 2) thread list lock.
    //

    Dereference = FALSE;
    ExpAcquireTimerLock(ExTimer, &OldIrql);
    ExpAcquireThreadActiveTimerListLockAtDpcLevel(ExThread);
    if ((ExTimer->ApcAssociated) && (&ExThread->Tcb == ExTimer->TimerApc.Thread)) {
        if (ExTimer->Period == 0) {
            RemoveEntryList(&ExTimer->ActiveTimerListEntry);
            ExTimer->ApcAssociated = FALSE;
            Dereference = TRUE;
        }

    } else {
        *NormalRoutine = (PKNORMAL_ROUTINE)NULL;
    }

    ExpReleaseThreadActiveTimerListLockFromDpcLevel(ExThread);
    ExpReleaseTimerLock(ExTimer, OldIrql);
    if (Dereference) {
        ObDereferenceObject((PVOID)ExTimer);
    }

    return;
}

VOID
ExpTimerDpcRoutine (
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )

/*++

Routine Description:

    This function is the DPC routine that is called when a timer expires that
    has an associated APC routine. Its function is to insert the associated
    APC into the target thread's APC queue.

Arguments:

    Dpc - Supplies a pointer to a control object of type DPC.

    DeferredContext - Supplies a pointer to the executive timer that contains
        the DPC that caused this routine to be executed.

    SystemArgument1, SystemArgument2 - Supplies values that are not used by
        this routine.

Return Value:

    None.

--*/

{

    PETIMER ExTimer;
    PKTIMER KeTimer;
    KIRQL OldIrql;

    //
    // Get address of executive and kernel timer objects.
    //

    ExTimer = (PETIMER)DeferredContext;
    KeTimer = &ExTimer->KeTimer;

    //
    // If there is still an APC associated with the timer, then insert the APC
    // in target thread's APC queue. It is possible that the timer does not
    // have an associated APC. This can happen when the timer is set to expire
    // by a thread running on another processor just after the DPC has been
    // removed from the DPC queue, but before it has acquired the timer related
    // spin lock.
    //

    ExpAcquireTimerLock(ExTimer, &OldIrql);
    if (ExTimer->ApcAssociated) {
        KeInsertQueueApc(&ExTimer->TimerApc,
                         SystemArgument1,
                         SystemArgument2,
                         TIMER_APC_INCREMENT);
    }

    ExpReleaseTimerLock(ExTimer, OldIrql);
    return;
}

VOID
ExpDeleteTimer (
    IN PVOID Object
    )

/*++

Routine Description:

    This function is the delete routine for timer objects. Its function is
    to cancel the timer and free the spin lock associated with a timer.

Arguments:

    Object - Supplies a pointer to an executive timer object.

Return Value:

    None.

--*/

{
    PETIMER     ExTimer;

    ExTimer = (PETIMER) Object;

    //
    // Cancel the timer and free the spin lock associated with the timer.
    //

    KeCancelTimer(&ExTimer->KeTimer);
    return;
}

VOID
ExTimerRundown (
    )

/*++

Routine Description:

    This function is called when a thread is about to be terminated to
    process the active timer list. It is assumed that APC's have been
    disabled for the subject thread, thus this code cannot be interrupted
    to execute an APC for the current thread.

Arguments:

    None.

Return Value:

    None.

--*/

{

    BOOLEAN Dereference;
    PETHREAD ExThread;
    PETIMER ExTimer;
    PLIST_ENTRY NextEntry;
    KIRQL OldIrql;

    //
    // Process each entry in the active timer list.
    //

    ExThread = PsGetCurrentThread();
    ExpAcquireThreadActiveTimerListLock(ExThread, &OldIrql);
    NextEntry = ExThread->ActiveTimerListHead.Flink;
    while (NextEntry != &ExThread->ActiveTimerListHead) {
        ExTimer = CONTAINING_RECORD(NextEntry, ETIMER, ActiveTimerListEntry);

        //
        // Increment the reference count on the object so that it cannot be
        // deleted, and then drop the active timer list lock.
        //
        // N. B. The object reference cannot fail and will acquire no mutexes.
        //

        ObReferenceObject(ExTimer);
        ExpReleaseThreadActiveTimerListLock(ExThread, OldIrql);

        //
        // Acquire the timer spin lock and reacquire the active time list spin
        // lock. If the timer is still in the current thread's active timer
        // list, then cancel the timer, remove the timer's DPC from the DPC
        // queue, remove the timer's APC from the APC queue, remove the timer
        // from the thread's active timer list, and set the associate APC
        // flag FALSE.
        //
        // N. B. The spin locks for the timer and the active timer list must be
        //  acquired in the order: 1) timer lock, 2) thread list lock.
        //

        ExpAcquireTimerLock(ExTimer, &OldIrql);
        ExpAcquireThreadActiveTimerListLockAtDpcLevel(ExThread);
        if ((ExTimer->ApcAssociated) && (&ExThread->Tcb == ExTimer->TimerApc.Thread)) {
            RemoveEntryList(&ExTimer->ActiveTimerListEntry);
            ExTimer->ApcAssociated = FALSE;
            KeCancelTimer(&ExTimer->KeTimer);
            KeRemoveQueueDpc(&ExTimer->TimerDpc);
            KeRemoveQueueApc(&ExTimer->TimerApc);
            Dereference = TRUE;

        } else {
            Dereference = FALSE;
        }

        ExpReleaseThreadActiveTimerListLockFromDpcLevel(ExThread);
        ExpReleaseTimerLock(ExTimer, OldIrql);
        if (Dereference) {
            ObDereferenceObject((PVOID)ExTimer);
        }

        ObDereferenceObject((PVOID)ExTimer);

        //
        // Raise IRQL to DISPATCH_LEVEL and reacquire active timer list
        // spin lock.
        //

        ExpAcquireThreadActiveTimerListLock(ExThread, &OldIrql);
        NextEntry = ExThread->ActiveTimerListHead.Flink;
    }

    ExpReleaseThreadActiveTimerListLock(ExThread, OldIrql);
    return;
}

NTSTATUS
NtCreateTimer (
    OUT PHANDLE TimerHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN TIMER_TYPE TimerType
    )

/*++

Routine Description:

    This function creates an timer object and opens a handle to the object with
    the specified desired access.

Arguments:

    TimerHandle - Supplies a pointer to a variable that will receive the
        timer object handle.

    ObjectAttributes - Supplies a pointer to an object attributes structure.

    TimerType - Supplies the type of the timer (autoclearing or notification).

Return Value:

    TBS

--*/

{

    PETIMER ExTimer;
    NTSTATUS Status;

    //
    // Check argument validity.
    //

    if ((TimerType != NotificationTimer) &&
        (TimerType != SynchronizationTimer)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Allocate timer object.
    //

    Status = ObCreateObject(&ExTimerObjectType,
                            ObjectAttributes,
                            sizeof(ETIMER),
                            (PVOID *)&ExTimer);

    //
    // If the timer object was successfully allocated, then initialize the
    // timer object and attempt to insert the time object in the current
    // process' handle table.
    //

    if (NT_SUCCESS(Status)) {
        KeInitializeDpc(&ExTimer->TimerDpc,
                        ExpTimerDpcRoutine,
                        (PVOID)ExTimer);

        KeInitializeTimerEx(&ExTimer->KeTimer, TimerType);
        ExTimer->ApcAssociated = FALSE;
        Status = ObInsertObject((PVOID)ExTimer,
                                ObjectAttributes,
                                0,
                                TimerHandle);
    }

    //
    // Return service status.
    //

    return Status;
}

NTSTATUS
NtCancelTimer (
    IN HANDLE TimerHandle,
    OUT PBOOLEAN CurrentState OPTIONAL
    )

/*++

Routine Description:

    This function cancels a timer object.

Arguments:

    TimerHandle - Supplies a handle to an timer object.

    CurrentState - Supplies an optional pointer to a variable that will
        receive the current state of the timer object.

Return Value:

    TBS

--*/

{

    BOOLEAN Dereference;
    PETHREAD ExThread;
    PETIMER ExTimer;
    KIRQL OldIrql;
    BOOLEAN State;
    NTSTATUS Status;

    //
    // Reference timer object by handle.
    //

    Status = ObReferenceObjectByHandle(TimerHandle,
                                       &ExTimerObjectType,
                                       (PVOID *)&ExTimer);

    //
    // If the reference was successful, then cancel the timer object,
    // dereference the timer object, and write the current state value
    // if specified. If the write attempt fails, then do not report an
    // error. When the caller attempts to access the current state value,
    // an access violation will occur.
    //

    if (NT_SUCCESS(Status)) {
        ExpAcquireTimerLock(ExTimer, &OldIrql);
        if (ExTimer->ApcAssociated) {
            ExThread = CONTAINING_RECORD(ExTimer->TimerApc.Thread, ETHREAD, Tcb);
            ExpAcquireThreadActiveTimerListLockAtDpcLevel(ExThread);
            RemoveEntryList(&ExTimer->ActiveTimerListEntry);
            ExTimer->ApcAssociated = FALSE;
            ExpReleaseThreadActiveTimerListLockFromDpcLevel(ExThread);
            KeCancelTimer(&ExTimer->KeTimer);
            KeRemoveQueueDpc(&ExTimer->TimerDpc);
            KeRemoveQueueApc(&ExTimer->TimerApc);
            Dereference = TRUE;

        } else {
            KeCancelTimer(&ExTimer->KeTimer);
            Dereference = FALSE;
        }

        ExpReleaseTimerLock(ExTimer, OldIrql);
        if (Dereference) {
            ObDereferenceObject((PVOID)ExTimer);
        }

        //
        // Read current state of timer, dereference timer object, and set
        // current state.
        //

        State = KeReadStateTimer(&ExTimer->KeTimer);
        ObDereferenceObject(ExTimer);
        if (ARGUMENT_PRESENT(CurrentState)) {
            *CurrentState = State;
        }
    }

    //
    // Return service status.
    //

    return Status;
}

NTSTATUS
NtQueryTimer (
    IN HANDLE TimerHandle,
    OUT PTIMER_BASIC_INFORMATION TimerInformation
    )

/*++

Routine Description:

    This function queries the state of an timer object and returns the
    requested information in the specified record structure.

Arguments:

    TimerHandle - Supplies a handle to an timer object.

    TimerInformation - Supplies a pointer to a record that is to receive the
        requested information.

Return Value:

    TBS

--*/

{

    PETIMER ExTimer;
    PKTIMER KeTimer;
    BOOLEAN State;
    NTSTATUS Status;
    LARGE_INTEGER TimeToGo;

    //
    // Reference timer object by handle.
    //

    Status = ObReferenceObjectByHandle(TimerHandle,
                                       &ExTimerObjectType,
                                       (PVOID *)&ExTimer);

    //
    // If the reference was successful, then read the current state,
    // compute the time remaining, dereference the timer object, fill in
    // the information structure, and return the length of the information
    // structure if specified. If the write of the time information or the
    // return length fails, then do not report an error. When the caller
    // accesses the information structure or the length, an violation will
    // occur.
    //

    if (NT_SUCCESS(Status)) {
        KeTimer = &ExTimer->KeTimer;
        State = KeReadStateTimer(KeTimer);
        KiQueryInterruptTime(&TimeToGo);
        TimeToGo.QuadPart = KeTimer->DueTime.QuadPart - TimeToGo.QuadPart;
        ObDereferenceObject(ExTimer);

        TimerInformation->TimerState = State;
        TimerInformation->RemainingTime = TimeToGo;
    }

    //
    // Return service status.
    //

    return Status;
}

NTSTATUS
NtSetTimerEx (
    IN HANDLE TimerHandle,
    IN PLARGE_INTEGER DueTime,
    IN PTIMER_APC_ROUTINE TimerApcRoutine OPTIONAL,
    IN KPROCESSOR_MODE ApcMode,
    IN PVOID TimerContext OPTIONAL,
    IN BOOLEAN WakeTimer,
    IN LONG Period OPTIONAL,
    OUT PBOOLEAN PreviousState OPTIONAL
    )

/*++

Routine Description:

    This function sets an timer object to a Not-Signaled state and sets the timer
    to expire at the specified time.

Arguments:

    TimerHandle - Supplies a handle to an timer object.

    DueTime - Supplies a pointer to absolute of relative time at which the
        timer is to expire.

    TimerApcRoutine - Supplies an optional pointer to a function which is to
        be executed when the timer expires. If this parameter is not specified,
        then the TimerContext parameter is ignored.

    TimerContext - Supplies an optional pointer to an arbitrary data structure
        that will be passed to the function specified by the TimerApcRoutine
        parameter. This parameter is ignored if the TimerApcRoutine parameter
        is not specified.

    WakeTimer - Supplies a boolean value that specifies whether the timer
        wakes computer operation if sleeping

    Period - Supplies an optional repetitive period for the timer.

    PreviousState - Supplies an optional pointer to a variable that will
        receive the previous state of the timer object.

Return Value:

    TBS

--*/

{

    BOOLEAN AssociatedApc;
    BOOLEAN Dereference;
    PETHREAD ExThread;
    PETIMER ExTimer;
    LARGE_INTEGER ExpirationTime;
    KIRQL OldIrql;
    BOOLEAN State;
    NTSTATUS Status;

    //
    // Check argument validity.
    //

    if (Period < 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Capture the expiration time.
    //

    ExpirationTime = *DueTime;

    //
    // Reference timer object by handle.
    //

    Status = ObReferenceObjectByHandle(TimerHandle,
                                       &ExTimerObjectType,
                                       (PVOID *)&ExTimer);

    //
    // If this WakeTimer flag is set, return the appropiate informational
    // success status code.
    //

    if (NT_SUCCESS(Status) && WakeTimer) {
        Status = STATUS_TIMER_RESUME_IGNORED;
    }

    //
    // If the reference was successful, then cancel the timer object, set
    // the timer object, dereference time object, and write the previous
    // state value if specified. If the write of the previous state value
    // fails, then do not report an error. When the caller attempts to
    // access the previous state value, an access violation will occur.
    //

    if (NT_SUCCESS(Status)) {
        ExpAcquireTimerLock(ExTimer, &OldIrql);

        if (ExTimer->ApcAssociated) {
            ExThread = CONTAINING_RECORD(ExTimer->TimerApc.Thread, ETHREAD, Tcb);
            ExpAcquireThreadActiveTimerListLockAtDpcLevel(ExThread);
            RemoveEntryList(&ExTimer->ActiveTimerListEntry);
            ExTimer->ApcAssociated = FALSE;
            ExpReleaseThreadActiveTimerListLockFromDpcLevel(ExThread);
            KeCancelTimer(&ExTimer->KeTimer);
            KeRemoveQueueDpc(&ExTimer->TimerDpc);
            KeRemoveQueueApc(&ExTimer->TimerApc);
            Dereference = TRUE;

        } else {
            KeCancelTimer(&ExTimer->KeTimer);
            Dereference = FALSE;
        }

        //
        // Read the current state of the timer.
        //

        State = KeReadStateTimer(&ExTimer->KeTimer);

        //
        // If an APC routine is specified, then initialize the APC, acquire the
        // thread's active time list lock, insert the timer in the thread's
        // active timer list, set the timer with an associated DPC, and set the
        // associated APC flag TRUE. Otherwise set the timer without an associated
        // DPC, and set the associated APC flag FALSE.
        //

        ExTimer->Period = Period;
        if (ARGUMENT_PRESENT(TimerApcRoutine)) {
            ExThread = PsGetCurrentThread();
            KeInitializeApc(&ExTimer->TimerApc,
                            &ExThread->Tcb,
                            ExpTimerApcRoutine,
                            (PKRUNDOWN_ROUTINE)NULL,
                            (PKNORMAL_ROUTINE)TimerApcRoutine,
                            ApcMode,
                            TimerContext);

            ExpAcquireThreadActiveTimerListLockAtDpcLevel(ExThread);
            InsertTailList(&ExThread->ActiveTimerListHead,
                           &ExTimer->ActiveTimerListEntry);

            ExTimer->ApcAssociated = TRUE;
            ExpReleaseThreadActiveTimerListLockFromDpcLevel(ExThread);
            KeSetTimerEx(&ExTimer->KeTimer,
                         ExpirationTime,
                         Period,
                         &ExTimer->TimerDpc);

            AssociatedApc = TRUE;

        } else {
            KeSetTimerEx(&ExTimer->KeTimer,
                         ExpirationTime,
                         Period,
                         NULL);

            AssociatedApc = FALSE;
        }

        ExpReleaseTimerLock(ExTimer, OldIrql);

        //
        // Dereference the object as appropriate.
        //

        if (Dereference) {
            ObDereferenceObject((PVOID)ExTimer);
        }

        if (AssociatedApc == FALSE) {
            ObDereferenceObject((PVOID)ExTimer);
        }

        if (ARGUMENT_PRESENT(PreviousState)) {
            *PreviousState = State;
        }
    }

    //
    // Return service status.
    //

    return Status;
}
