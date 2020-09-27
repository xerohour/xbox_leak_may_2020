/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    create.c

Abstract:

    Process and Thread Creation.

Author:

    Mark Lucovsky (markl) 20-Apr-1989

Revision History:

--*/

#include "psp.h"

ULONG
PspUnhandledExceptionInSystemThread(
    IN PEXCEPTION_POINTERS ExceptionPointers
    )
{
    KdPrint(("PS: Unhandled Kernel Mode Exception Pointers = 0x%p\n", ExceptionPointers));
    KdPrint(("Code %x Addr %p\nInfo0 %p Info1 %p Info2 %p Info3 %p\n",
        ExceptionPointers->ExceptionRecord->ExceptionCode,
        (ULONG_PTR)ExceptionPointers->ExceptionRecord->ExceptionAddress,
        ExceptionPointers->ExceptionRecord->ExceptionInformation[0],
        ExceptionPointers->ExceptionRecord->ExceptionInformation[1],
        ExceptionPointers->ExceptionRecord->ExceptionInformation[2],
        ExceptionPointers->ExceptionRecord->ExceptionInformation[3]
        ));

    //
    // Return EXCEPTION_CONTINUE_SEARCH and let the exception dispatcher notify
    // the debugger of the second chance exception or bugcheck the system.
    //

    return EXCEPTION_CONTINUE_SEARCH;
}

VOID
PspSystemThreadStartup(
    IN PKSTART_ROUTINE StartRoutine,
    IN PVOID StartContext
    )

/*++

Routine Description:

    This function is called by the kernel to start a system thread.

Arguments:

    StartRoutine - Supplies the address of the system threads entry point.

    StartContext - Supplies a context value for the system thread.

Return Value:

    None.

--*/

{
    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    try {
        (StartRoutine)(StartContext);
    } except (PspUnhandledExceptionInSystemThread(GetExceptionInformation())) {
    }

    PsTerminateSystemThread(STATUS_SUCCESS);
}

NTSTATUS
PsCreateSystemThread(
    OUT PHANDLE ThreadHandle,
    OUT PHANDLE ThreadId OPTIONAL,
    IN PKSTART_ROUTINE StartRoutine,
    IN PVOID StartContext,
    IN BOOLEAN DebuggerThread
    )

/*++

Routine Description:

    This routine creates and starts a system thread.

Arguments:

    ThreadHandle - Returns the handle for the new thread.

    ThreadId - Returns the thread ID of for the new thread.

    StartRoutine - Supplies the address of the system thread start routine.

    StartContext - Supplies context for a system thread start routine.

    DebuggerThread - Supplies a value that controls whether or not this is a
        thread owned by the debugger.

Return Value:

    Status of operation.

--*/

{
    return PsCreateSystemThreadEx(ThreadHandle,
                                  0,
                                  KERNEL_STACK_SIZE,
                                  0,
                                  ThreadId,
                                  StartRoutine,
                                  StartContext,
                                  FALSE,
                                  DebuggerThread,
                                  PspSystemThreadStartup);
}

NTSTATUS
PsCreateSystemThreadEx(
    OUT PHANDLE ThreadHandle,
    IN SIZE_T ThreadExtensionSize,
    IN SIZE_T KernelStackSize,
    IN SIZE_T TlsDataSize,
    OUT PHANDLE ThreadId OPTIONAL,
    IN PKSTART_ROUTINE StartRoutine,
    IN PVOID StartContext,
    IN BOOLEAN CreateSuspended,
    IN BOOLEAN DebuggerThread,
    IN PKSYSTEM_ROUTINE SystemRoutine OPTIONAL
    )

/*++

Routine Description:

    This routine creates and initializes a thread object. It implements the
    foundation for NtCreateThread and for PsCreateSystemThread.

Arguments:

    ThreadHandle - Returns the handle for the new thread.

    XapiThreadSize - Supplies the size of the XAPITHREAD to allocate as part of
        the thread's object body.

    ThreadExtensionSize - Supplies the number of bytes to allocate as part of
        the thread's object body.

    KernelStackSize - Supplies the size of the stack for the new thread.

    TlsDataSize - Supplies the number of bytes to reserve from the kernel stack
        for thread local storage.

    ThreadId - Returns the thread ID of for the new thread.

    StartRoutine - Supplies the address of the system thread start routine.

    StartContext - Supplies context for a system thread start routine.

    CreateSuspended - Supplies a value that controls whether or not a thread is
        created in a suspended state.

    DebuggerThread - Supplies a value that controls whether or not this is a
        thread owned by the debugger.

    SystemRoutine - Supplies the address of the routine that wraps the
        StartRoutine.

Return Value:

    Status of operation.

--*/

{
    NTSTATUS st;
    PETHREAD Thread;
    PVOID KernelStack;

    PAGED_CODE();

    Thread = NULL;

    //
    // Allocate thread object.
    //

    st = ObCreateObject(&PsThreadObjectType,
                        NULL,
                        sizeof(ETHREAD) + ThreadExtensionSize,
                        (PVOID *)&Thread);

    if (!NT_SUCCESS(st)) {
        return st;
    }

    //
    // Zero out the thread object.
    //

    RtlZeroMemory(Thread, sizeof(ETHREAD) + ThreadExtensionSize);

    //
    // Initialize Io.
    //

    InitializeListHead(&Thread->IrpList);

    //
    // Initialize Ex.
    //

    InitializeListHead(&Thread->ActiveTimerListHead);

    //
    // Allocate the stack.
    //

    KernelStackSize = ROUND_TO_PAGES(KernelStackSize);
    if (KernelStackSize < KERNEL_STACK_SIZE) {
        KernelStackSize = KERNEL_STACK_SIZE;
    }

    ASSERT(KernelStackSize > TlsDataSize);
    ASSERT((KernelStackSize - TlsDataSize) > KERNEL_USABLE_STACK_SIZE);

    KernelStack = MmCreateKernelStack(KernelStackSize, DebuggerThread);
    if (KernelStack == NULL) {
        ObDereferenceObject(Thread);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Initialize kernel thread object for kernel mode thread.
    //

    Thread->StartAddress = (PVOID)(ULONG_PTR)StartRoutine;

    KeInitializeThread(&Thread->Tcb,
                       KernelStack,
                       KernelStackSize,
                       TlsDataSize,
                       SystemRoutine,
                       StartRoutine,
                       StartContext,
                       &KiSystemProcess);

    KeEnableApcQueuingThread(&Thread->Tcb);

    if (CreateSuspended) {
        KeSuspendThread(&Thread->Tcb);
    }

    //
    // Failures that occur after this point cause the thread to go through
    // PsTerminateSystemThread.
    //

    //
    // Increment the reference count on the thread so that we can safely use the
    // object until the end of this routine.  The thread that we start could
    // terminate before we end up returning to our caller.
    //

    ObReferenceObject(Thread);

    //
    // Increment the reference count on the thread on behalf of the running
    // thread.  The thread reaper will release this reference.
    //

    ObReferenceObject(Thread);

    //
    // Insert the thread object into the handle table and use that handle as the
    // thread identifier.
    //

    st = ObInsertObject(Thread, NULL, 0, &Thread->UniqueThread);

    if (NT_SUCCESS(st)) {

        //
        // Notify registered callout routines of thread creation.
        //

        if (PspCreateThreadNotifyRoutineCount != 0) {
            ULONG i;

            for (i=0; i<PSP_MAX_CREATE_THREAD_NOTIFY; i++) {
                if (PspCreateThreadNotifyRoutine[i] != NULL) {
                    (*PspCreateThreadNotifyRoutine[i])(Thread,
                                                       Thread->UniqueThread,
                                                       TRUE);
                }
            }
        }

        //
        // Create another handle to pass back to the client.
        //

        st = ObOpenObjectByPointer(Thread, &PsThreadObjectType, ThreadHandle);
    }

    if (!NT_SUCCESS(st)) {

        //
        // The insert failed or the handle creation failed.  Terminate the
        // thread.
        //

        Thread->Tcb.HasTerminated = TRUE;

        if (CreateSuspended) {
            KeResumeThread(&Thread->Tcb);
        }

    } else {

        //
        // Return the thread id to the client if requested.
        //

        if (ARGUMENT_PRESENT(ThreadId)) {
            *ThreadId = Thread->UniqueThread;
        }
    }

    //
    // Set the create time for the thread and insert the thread into the ready
    // queue.
    //

    KeQuerySystemTime(&Thread->CreateTime);
    KeReadyThread(&Thread->Tcb);

    //
    // Release the reference to the thread that we made above.
    //

    ObDereferenceObject(Thread);

    return st;
}

NTSTATUS
PsSetCreateThreadNotifyRoutine(
    IN PCREATE_THREAD_NOTIFY_ROUTINE NotifyRoutine
    )

/*++

Routine Description:

    This function allows an installable file system to hook into thread
    creation and deletion to track those events against their own internal
    data structures.

Arguments:

    NotifyRoutine - Supplies the address of the routine which is called at
        thread creation and deletion. The routine is passed the unique Id
        of the created or deleted thread and the unique Id of the containing
        process. The third parameter passed to the notify routine is TRUE if
        the thread is being created and FALSE if it is being deleted.

Return Value:

    STATUS_SUCCESS if successful, and STATUS_INSUFFICIENT_RESOURCES if not.

--*/

{

    ULONG i;
    NTSTATUS Status;

    Status = STATUS_INSUFFICIENT_RESOURCES;
    for (i = 0; i < PSP_MAX_CREATE_THREAD_NOTIFY; i += 1) {
        if (PspCreateThreadNotifyRoutine[i] == NULL) {
            PspCreateThreadNotifyRoutine[i] = NotifyRoutine;
            PspCreateThreadNotifyRoutineCount += 1;
            Status = STATUS_SUCCESS;
            break;
        }
    }

    return Status;
}
