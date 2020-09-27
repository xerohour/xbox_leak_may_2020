/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    event.c

Abstract:

    This module implements the executive event object. Functions are provided
    to create, open, set, reset, pulse, and query event objects.

--*/

#include "exp.h"

//
// Event object type information.
//

DECLSPEC_RDATA OBJECT_TYPE ExEventObjectType = {
    ExAllocatePoolWithTag,
    ExFreePool,
    NULL,
    NULL,
    NULL,
    (PVOID)FIELD_OFFSET(KEVENT, Header),
    'vevE'
};

NTSTATUS
NtClearEvent (
    IN HANDLE EventHandle
    )

/*++

Routine Description:

    This function sets an event object to a Not-Signaled state.

Arguments:

    EventHandle - Supplies a handle to an event object.

Return Value:

    TBS

--*/

{

    PVOID Event;
    NTSTATUS Status;

    //
    // Reference event object by handle.
    //

    Status = ObReferenceObjectByHandle(EventHandle,
                                       &ExEventObjectType,
                                       &Event);

    //
    // If the reference was successful, then set the state of the event
    // object to Not-Signaled and dereference event object.
    //

    if (NT_SUCCESS(Status)) {
        KeClearEvent((PKEVENT)Event);
        ObDereferenceObject(Event);
    }

    //
    // Return service status.
    //

    return Status;
}

NTSTATUS
NtCreateEvent (
    OUT PHANDLE EventHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN EVENT_TYPE EventType,
    IN BOOLEAN InitialState
    )

/*++

Routine Description:

    This function creates an event object, sets it initial state to the
    specified value, and opens a handle to the object with the specified
    desired access.

Arguments:

    EventHandle - Supplies a pointer to a variable that will receive the
        event object handle.

    ObjectAttributes - Supplies a pointer to an object attributes structure.

    EventType - Supplies the type of the event (autoclearing or notification).

    InitialState - Supplies the initial state of the event object.

Return Value:

    TBS

--*/

{

    PVOID Event;
    NTSTATUS Status;

    //
    // Check argument validity.
    //

    if ((EventType != NotificationEvent) && (EventType != SynchronizationEvent)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Allocate event object.
    //

    Status = ObCreateObject(&ExEventObjectType,
                            ObjectAttributes,
                            sizeof(KEVENT),
                            (PVOID *)&Event);

    //
    // If the event object was successfully allocated, then initialize the
    // event object and attempt to insert the event object in the current
    // process' handle table.
    //

    if (NT_SUCCESS(Status)) {
        KeInitializeEvent((PKEVENT)Event, EventType, InitialState);
        Status = ObInsertObject(Event,
                                ObjectAttributes,
                                0,
                                EventHandle);
    }

    //
    // Return service status.
    //

    return Status;
}

NTSTATUS
NtPulseEvent (
    IN HANDLE EventHandle,
    OUT PLONG PreviousState OPTIONAL
    )

/*++

Routine Description:

    This function sets an event object to a Signaled state, attempts to
    satisfy as many waits as possible, and then resets the state of the
    event object to Not-Signaled.

Arguments:

    EventHandle - Supplies a handle to an event object.

    PreviousState - Supplies an optional pointer to a variable that will
        receive the previous state of the event object.

Return Value:

    TBS

--*/

{

    PVOID Event;
    LONG State;
    NTSTATUS Status;

    //
    // Reference event object by handle.
    //

    Status = ObReferenceObjectByHandle(EventHandle,
                                       &ExEventObjectType,
                                       &Event);

    //
    // If the reference was successful, then pulse the event object,
    // dereference event object, and write the previous state value if
    // specified. If the write of the previous state fails, then do not
    // report an error. When the caller attempts to access the previous
    // state value, an access violation will occur.
    //

    if (NT_SUCCESS(Status)) {
        State = KePulseEvent((PKEVENT)Event, EVENT_INCREMENT, FALSE);
        ObDereferenceObject(Event);
        if (ARGUMENT_PRESENT(PreviousState)) {
            *PreviousState = State;
        }
    }

    //
    // Return service status.
    //

    return Status;
}

NTSTATUS
NtQueryEvent (
    IN HANDLE EventHandle,
    OUT PEVENT_BASIC_INFORMATION EventInformation
    )

/*++

Routine Description:

    This function queries the state of an event object and returns the
    requested information in the specified record structure.

Arguments:

    EventHandle - Supplies a handle to an event object.

    EventInformation - Supplies a pointer to a record that is to receive the
        requested information.

Return Value:

    TBS

--*/

{

    PKEVENT Event;
    LONG State;
    NTSTATUS Status;
    EVENT_TYPE EventType;

    //
    // Reference event object by handle.
    //

    Status = ObReferenceObjectByHandle(EventHandle,
                                       &ExEventObjectType,
                                       (PVOID *)&Event);

    //
    // If the reference was successful, then read the current state of
    // the event object, deference event object, fill in the information
    // structure, and return the length of the information structure if
    // specified. If the write of the event information or the return
    // length fails, then do not report an error. When the caller accesses
    // the information structure or length an access violation will occur.
    //

    if (NT_SUCCESS(Status)) {
        State = KeReadStateEvent(Event);
        EventType = Event->Header.Type;
        ObDereferenceObject(Event);

        EventInformation->EventType = EventType;
        EventInformation->EventState = State;
    }

    //
    // Return service status.
    //

    return Status;
}

NTSTATUS
NtResetEvent (
    IN HANDLE EventHandle,
    OUT PLONG PreviousState OPTIONAL
    )

/*++

Routine Description:

    This function sets an event object to a Not-Signaled state.

Arguments:

    EventHandle - Supplies a handle to an event object.

    PreviousState - Supplies an optional pointer to a variable that will
        receive the previous state of the event object.

Return Value:

    TBS

--*/

{

    PVOID Event;
    LONG State;
    NTSTATUS Status;

    //
    // Reference event object by handle.
    //

    Status = ObReferenceObjectByHandle(EventHandle,
                                       &ExEventObjectType,
                                       &Event);

    //
    // If the reference was successful, then set the state of the event
    // object to Not-Signaled, dereference event object, and write the
    // previous state value if specified. If the write of the previous
    // state fails, then do not report an error. When the caller attempts
    // to access the previous state value, an access violation will occur.
    //

    if (NT_SUCCESS(Status)) {
        State = KeResetEvent((PKEVENT)Event);
        ObDereferenceObject(Event);
        if (ARGUMENT_PRESENT(PreviousState)) {
            *PreviousState = State;
        }
    }

    //
    // Return service status.
    //

    return Status;
}

NTSTATUS
NtSetEvent (
    IN HANDLE EventHandle,
    OUT PLONG PreviousState OPTIONAL
    )

/*++

Routine Description:

    This function sets an event object to a Signaled state and attempts to
    satisfy as many waits as possible.

Arguments:

    EventHandle - Supplies a handle to an event object.

    PreviousState - Supplies an optional pointer to a variable that will
        receive the previous state of the event object.

Return Value:

    TBS

--*/

{

    PVOID Event;
    LONG State;
    NTSTATUS Status;

    //
    // Reference event object by handle.
    //

    Status = ObReferenceObjectByHandle(EventHandle,
                                       &ExEventObjectType,
                                       &Event);

    //
    // If the reference was successful, then set the event object to the
    // Signaled state, dereference event object, and write the previous
    // state value if specified. If the write of the previous state fails,
    // then do not report an error. When the caller attempts to access the
    // previous state value, an access violation will occur.
    //

    if (NT_SUCCESS(Status)) {
        State = KeSetEvent((PKEVENT)Event, EVENT_INCREMENT, FALSE);
        ObDereferenceObject(Event);
        if (ARGUMENT_PRESENT(PreviousState)) {
            *PreviousState = State;
        }
    }

    //
    // Return service status.
    //

    return Status;
}
