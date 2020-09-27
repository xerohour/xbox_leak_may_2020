/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    psspnd.c

Abstract:

    This module implements NtSuspendThread and NtResumeThread

Author:

    Mark Lucovsky (markl) 25-May-1989

Revision History:

--*/

#include "psp.h"

NTSTATUS
NtSuspendThread(
    IN HANDLE ThreadHandle,
    OUT PULONG PreviousSuspendCount OPTIONAL
    )

/*++

Routine Description:

    This function suspends the target thread, and optionally
    returns the previous suspend count.

Arguments:

    ThreadHandle - Supplies a handle to the thread object to suspend.

    PreviousSuspendCount - An optional parameter, that if specified
        points to a variable that receives the thread's previous suspend
        count.

Return Value:

    Status of operation.

--*/

{
    PETHREAD Thread;
    NTSTATUS st;
    ULONG LocalPreviousSuspendCount;

    PAGED_CODE();

    st = ObReferenceObjectByHandle(
            ThreadHandle,
            &PsThreadObjectType,
            (PVOID *)&Thread
            );

    if ( !NT_SUCCESS(st) ) {
        return st;
    }

    try {

        if ( Thread != PsGetCurrentThread() ) {
            if ( Thread->Tcb.HasTerminated ) {
                ObDereferenceObject(Thread);
                return STATUS_THREAD_IS_TERMINATING;
            }

            LocalPreviousSuspendCount = (ULONG) KeSuspendThread(&Thread->Tcb);

        } else {
            LocalPreviousSuspendCount = (ULONG) KeSuspendThread(&Thread->Tcb);
        }

        ObDereferenceObject(Thread);

        if (ARGUMENT_PRESENT(PreviousSuspendCount))
            *PreviousSuspendCount = LocalPreviousSuspendCount;

    } except (EXCEPTION_EXECUTE_HANDLER) {

        st = GetExceptionCode();

        //
        // Either the suspend, or the store could cause an
        // exception. The store is a partial success, while the
        // suspend exception is an error
        //

        if ( st == STATUS_SUSPEND_COUNT_EXCEEDED ) {
            ObDereferenceObject(Thread);
        } else {
            st = STATUS_SUCCESS;
        }

        return st;
    }

    return STATUS_SUCCESS;

}

NTSTATUS
NtResumeThread(
    IN HANDLE ThreadHandle,
    OUT PULONG PreviousSuspendCount OPTIONAL
    )

/*++

Routine Description:

    This function resumes the target thread, and optionally
    returns the previous suspend count.

Arguments:

    ThreadHandle - Supplies a handle to the thread object to resume.

    PreviousSuspendCount - An optional parameter, that if specified
        points to a variable that receives the thread's previous suspend
        count.

Return Value:

    Status of operation.

--*/

{
    PETHREAD Thread;
    NTSTATUS st;
    ULONG LocalPreviousSuspendCount;

    PAGED_CODE();

    st = ObReferenceObjectByHandle(
            ThreadHandle,
            &PsThreadObjectType,
            (PVOID *)&Thread
            );

    if ( !NT_SUCCESS(st) ) {
        return st;
    }

    LocalPreviousSuspendCount = (ULONG) KeResumeThread(&Thread->Tcb);

    ObDereferenceObject(Thread);

    if (ARGUMENT_PRESENT(PreviousSuspendCount)) {
        *PreviousSuspendCount = LocalPreviousSuspendCount;
    }

    return STATUS_SUCCESS;
}
