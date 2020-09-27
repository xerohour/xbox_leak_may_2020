/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    psctx.c

Abstract:

    This procedure implements Get/Set Context Thread

Author:

    Mark Lucovsky (markl) 25-May-1989

Revision History:

--*/

#include "psp.h"

VOID
PspQueueApcSpecialApc(
    IN PKAPC Apc,
    IN PKNORMAL_ROUTINE *NormalRoutine,
    IN PVOID *NormalContext,
    IN PVOID *SystemArgument1,
    IN PVOID *SystemArgument2
    )
{
    PAGED_CODE();

    ExFreePool(Apc);
}

NTSYSAPI
NTSTATUS
NTAPI
NtQueueApcThread(
    IN HANDLE ThreadHandle,
    IN PPS_APC_ROUTINE ApcRoutine,
    IN PVOID ApcArgument1,
    IN PVOID ApcArgument2,
    IN PVOID ApcArgument3
    )

/*++

Routine Description:

    This function is used to queue a user-mode APC to the specified thread. The APC
    will fire when the specified thread does an alertable wait

Arguments:

    ThreadHandle - Supplies a handle to a thread object.  The caller
        must have THREAD_SET_CONTEXT access to the thread.

    ApcRoutine - Supplies the address of the APC routine to execute when the
        APC fires.

    ApcArgument1 - Supplies the first PVOID passed to the APC

    ApcArgument2 - Supplies the second PVOID passed to the APC

    ApcArgument3 - Supplies the third PVOID passed to the APC

Return Value:

    Returns an NT Status code indicating success or failure of the API

--*/

{
    PETHREAD Thread;
    NTSTATUS Status;
    PKAPC Apc;

    PAGED_CODE();

    Status = ObReferenceObjectByHandle(ThreadHandle,
                                       &PsThreadObjectType,
                                       (PVOID *)&Thread);

    if (NT_SUCCESS(Status)) {

        Apc = ExAllocatePoolWithTag(sizeof(*Apc), 'pasP');

        if (Apc != NULL) {

            KeInitializeApc(Apc,
                            &Thread->Tcb,
                            PspQueueApcSpecialApc,
                            NULL,
                            (PKNORMAL_ROUTINE)ApcRoutine,
                            UserMode,
                            ApcArgument1);

            if (!KeInsertQueueApc(Apc, ApcArgument2, ApcArgument3, 0)) {
                ExFreePool(Apc);
                Status = STATUS_UNSUCCESSFUL;
            }

        } else {
            Status = STATUS_NO_MEMORY;
        }

        ObDereferenceObject(Thread);
    }

    return Status;
}
