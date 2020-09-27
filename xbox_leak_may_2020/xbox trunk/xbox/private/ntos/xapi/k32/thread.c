/*++

Copyright (c) 1990-2001  Microsoft Corporation

Module Name:

    thread.c

Abstract:

    This module implements Win32 Thread Object APIs

--*/

#include "basedll.h"
#pragma hdrstop

//
// Number of bytes that need to be allocated for thread local storage.
//
ULONG XapiTlsSize;

//
// General critical section used to guard XAPI data structures.
//
INITIALIZED_CRITICAL_SECTION(XapiProcessLock);

//
// List of routines to be notified at thread creation and deletion.  Access is
// guarded by the XapiProcessLock.
//
INITIALIZED_LIST_ENTRY(XapiThreadNotifyRoutineList);

//
// Top level Win32 exception filter implemented by the title.
//
LPTOP_LEVEL_EXCEPTION_FILTER XapiCurrentTopLevelFilter;

VOID
XapiCallThreadNotifyRoutines(
    BOOL Create
    )

/*++

Routine Description:

    Calls the registered thread notification routines when the active thread is
    created or deleted.

Arguments:

    Create - TRUE if the active thread has been created, else FALSE if the
        active thread is to be deleted.

Return Value:

    None.

--*/

{
    PLIST_ENTRY NextListEntry;
    PXTHREAD_NOTIFICATION ThreadNotification;

    XapiAcquireProcessLock();

    NextListEntry = XapiThreadNotifyRoutineList.Flink;

    while (NextListEntry != &XapiThreadNotifyRoutineList) {

        ThreadNotification = CONTAINING_RECORD(NextListEntry,
            XTHREAD_NOTIFICATION, ListEntry);
        NextListEntry = ThreadNotification->ListEntry.Flink;

        ThreadNotification->pfnNotifyRoutine(Create);
    }

    XapiReleaseProcessLock();
}

LONG
WINAPI
UnhandledExceptionFilter(
    IN PEXCEPTION_POINTERS ExceptionPointers
    )

/*++

Routine Description:

    Exception filter for exceptions that don't get handled before the
    top-level handler. This filter always spews debug info and passes
    on the exception to the exception handler.

Arguments:

    ExceptionPointers - Exception information

Return Value:

    EXCEPTION_EXECUTE_HANDLER - passes on control to the handler

--*/

{
#if DBG
    DbgPrint("Unhandled XAPI Exception. Exception Pointers = 0x%p\n", ExceptionPointers);
    DbgPrint("Code %x Addr %p\nInfo0 %p Info1 %p Info2 %p Info3 %p\n",
        ExceptionPointers->ExceptionRecord->ExceptionCode,
        (ULONG_PTR)ExceptionPointers->ExceptionRecord->ExceptionAddress,
        ExceptionPointers->ExceptionRecord->ExceptionInformation[0],
        ExceptionPointers->ExceptionRecord->ExceptionInformation[1],
        ExceptionPointers->ExceptionRecord->ExceptionInformation[2],
        ExceptionPointers->ExceptionRecord->ExceptionInformation[3]
        );
#endif // DBG

    if ( XapiCurrentTopLevelFilter )
    {
        LONG FilterReturn = (XapiCurrentTopLevelFilter)(ExceptionPointers);
        if ( FilterReturn == EXCEPTION_CONTINUE_EXECUTION )
        {
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

VOID
XapiThreadStartup(
    IN PKSTART_ROUTINE StartRoutine,
    IN PVOID StartContext
    )

/*++

Routine Description:

    Called by the kernel on thread startup. We set up the default exception handler
    and a data structure to keep track of Xapi data.

Arguments:

    StartRoutine - address the thread should be started at.

    StartContext - an LPVOID pointer passed in to CreateThread to pass on to the thread.

Return Value:

    None.

--*/

{
    DWORD dwExitCode;

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    try
    {
        /* Need to set up the TLS data */
        PULONG TlsData;
        DWORD RawDataSize;

        ASSERT(KeGetCurrentThread()->TlsData);

#if DBG
        /* First make sure the TLS data is where we think it should be */
        ASSERT((ULONG_PTR)KeGetCurrentThread()->TlsData -
            (ULONG_PTR)KeGetCurrentTlsDataTop() == *((PULONG)_tls_used.AddressOfIndex) * 4);
#endif

        /* Need to fill the first slot with a pointer to the remainder */
        TlsData = (PULONG)KeGetCurrentThread()->TlsData + 1;
        TlsData[-1] = (ULONG_PTR)TlsData;

        /* Ensure 16-byte alignment */
        ASSERT(((ULONG_PTR)TlsData & 15) == 0);

        RawDataSize = _tls_used.EndAddressOfRawData - _tls_used.StartAddressOfRawData;

        RtlCopyMemory(TlsData, (PVOID)_tls_used.StartAddressOfRawData,
            RawDataSize);

        if (_tls_used.SizeOfZeroFill != 0) {
            RtlZeroMemory((PBYTE)TlsData + RawDataSize, _tls_used.SizeOfZeroFill);
        }

        XapiCallThreadNotifyRoutines(TRUE);

        dwExitCode = (*(LPTHREAD_START_ROUTINE)StartRoutine)(StartContext);

        XapiCallThreadNotifyRoutines(FALSE);
    }
    except (UnhandledExceptionFilter(GetExceptionInformation()))
    {
        //
        // UnhandledExceptionFilter will return either EXCEPTION_CONTINUE_SEARCH,
        // in which case, the exception search will stop since we're the top of
        // the exception stack, or it will return EXCEPTION_CONTINUE_EXECUTION.
        // We'll never execute this handler.
        //

        ASSERT(FALSE);
    }

    PsTerminateSystemThread(dwExitCode);
}

HANDLE
APIENTRY
CreateThread(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    DWORD dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
    )

/*++

Routine Description:

    A thread object can be created to execute within the address space of the
    calling process using CreateThread.

    Creating a thread causes a new thread of execution to begin in the address
    space of the current process. The thread has access to all objects opened
    by the process.

    The thread begins executing at the address specified by the StartAddress
    parameter. If the thread returns from this procedure, the results are
    un-specified.

    The thread remains in the system until it has terminated and
    all handles to the thread
    have been closed through a call to CloseHandle.

    When a thread terminates, it attains a state of signaled satisfying all
    waits on the object.

    In addition to the STANDARD_RIGHTS_REQUIRED access flags, the following
    object type specific access flags are valid for thread objects:

        - THREAD_QUERY_INFORMATION - This access is required to read
          certain information from the thread object.

        - SYNCHRONIZE - This access is required to wait on a thread
          object.

        - THREAD_GET_CONTEXT - This access is required to read the
          context of a thread using GetThreadContext.

        - THREAD_SET_CONTEXT - This access is required to write the
          context of a thread using SetThreadContext.

        - THREAD_SUSPEND_RESUME - This access is required to suspend or
          resume a thread using SuspendThread or ResumeThread.

        - THREAD_ALL_ACCESS - This set of access flags specifies all of
          the possible access flags for a thread object.

Arguments:

    hProcess - Supplies the handle to the process in which the thread is
        to be create in.

    lpThreadAttributes - An optional parameter that may be used to specify
        the attributes of the new thread.  If the parameter is not
        specified, then the thread is created without a security
        descriptor, and the resulting handle is not inherited on process
        creation.

    dwStackSize - Supplies the size in bytes of the stack for the new thread.
        A value of zero specifies that the thread's stack size should be
        the same size as the stack size of the first thread in the process.
        This size is specified in the application's executable file.

    lpStartAddress - Supplies the starting address of the new thread.  The
        address is logically a procedure that never returns and that
        accepts a single 32-bit pointer argument.

    lpParameter - Supplies a single parameter value passed to the thread.

    dwCreationFlags - Supplies additional flags that control the creation
        of the thread.

        dwCreationFlags Flags:

        CREATE_SUSPENDED - The thread is created in a suspended state.
            The creator can resume this thread using ResumeThread.
            Until this is done, the thread will not begin execution.

    lpThreadId - Returns the thread identifier of the thread.  The
        thread ID is valid until the thread terminates.

Return Value:

    NON-NULL - Returns a handle to the new thread.  The handle has full
        access to the new thread and may be used in any API that
        requires a handle to a thread object.

    NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    HANDLE Handle;

    if (dwStackSize == 0) {
        dwStackSize = XeImageHeader()->SizeOfStackCommit;
    }

    Status = PsCreateSystemThreadEx(
                &Handle,
                0,
                dwStackSize,
                XapiTlsSize,
                (PHANDLE)lpThreadId,
                (PKSTART_ROUTINE)lpStartAddress,
                lpParameter,
                (BOOLEAN)((dwCreationFlags & CREATE_SUSPENDED) ? TRUE : FALSE),
                FALSE,
                (PKSYSTEM_ROUTINE)XapiThreadStartup
                );

    if (!NT_SUCCESS(Status) && XapiIsXapiThread()) {
        XapiSetLastNTError(Status);
        return NULL;
    }

    return Handle;
}

BOOL
APIENTRY
SetThreadPriority(
    HANDLE hThread,
    int nPriority
    )

/*++

Routine Description:

    The specified thread's priority can be set using SetThreadPriority.

    A thread's priority may be set using SetThreadPriority.  This call
    allows the thread's relative execution importance to be communicated
    to the system.  The system normally schedules threads according to
    their priority.  The system is free to temporarily boost the
    priority of a thread when signifigant events occur (e.g.  keyboard
    or mouse input...).  Similarly, as a thread runs without blocking,
    the system will decay its priority.  The system will never decay the
    priority below the value set by this call.

    In the absence of system originated priority boosts, threads will be
    scheduled in a round-robin fashion at each priority level from
    THREAD_PRIORITY_TIME_CRITICAL to THREAD_PRIORITY_IDLE.  Only when there
    are no runnable threads at a higher level, will scheduling of
    threads at a lower level take place.

    All threads initially start at THREAD_PRIORITY_NORMAL.

    If for some reason the thread needs more priority, it can be
    switched to THREAD_PRIORITY_ABOVE_NORMAL or THREAD_PRIORITY_HIGHEST.
    Switching to THREAD_PRIORITY_TIME_CRITICAL should only be done in extreme
    situations.  Since these threads are given the highes priority, they
    should only run in short bursts.  Running for long durations will
    soak up the systems processing bandwidth starving threads at lower
    levels.

    If a thread needs to do low priority work, or should only run there
    is nothing else to do, its priority should be set to
    THREAD_PRIORITY_BELOW_NORMAL or THREAD_PRIORITY_LOWEST.  For extreme
    cases, THREAD_PRIORITY_IDLE can be used.

    Care must be taken when manipulating priorites.  If priorities are
    used carelessly (every thread is set to THREAD_PRIORITY_TIME_CRITICAL),
    the effects of priority modifications can produce undesireable
    effects (e.g.  starvation, no effect...).

Arguments:

    hThread - Supplies a handle to the thread whose priority is to be
        set.  The handle must have been created with
        THREAD_SET_INFORMATION access.

    nPriority - Supplies the priority value for the thread.  The
        following five priority values (ordered from lowest priority to
        highest priority) are allowed.

        nPriority Values:

        THREAD_PRIORITY_IDLE - The thread's priority should be set to
            the lowest possible settable priority.

        THREAD_PRIORITY_LOWEST - The thread's priority should be set to
            the next lowest possible settable priority.

        THREAD_PRIORITY_BELOW_NORMAL - The thread's priority should be
            set to just below normal.

        THREAD_PRIORITY_NORMAL - The thread's priority should be set to
            the normal priority value.  This is the value that all
            threads begin execution at.

        THREAD_PRIORITY_ABOVE_NORMAL - The thread's priority should be
            set to just above normal priority.

        THREAD_PRIORITY_HIGHEST - The thread's priority should be set to
            the next highest possible settable priority.

        THREAD_PRIORITY_TIME_CRITICAL - The thread's priority should be set
            to the highest possible settable priority.  This priority is
            very likely to interfere with normal operation of the
            system.

Return Value:

    TRUE - The operation was successful

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.
--*/

{
    NTSTATUS Status;
    LONG BasePriority;
    PETHREAD Thread;

    Status = ObReferenceObjectByHandle(hThread,
                                       PsThreadObjectType,
                                       (PVOID *)&Thread);

    if (NT_SUCCESS(Status)) {

        BasePriority = (LONG)nPriority;
        if (BasePriority == THREAD_PRIORITY_TIME_CRITICAL) {
            BasePriority = ((HIGH_PRIORITY + 1) / 2);
        } else if (BasePriority == THREAD_PRIORITY_IDLE) {
            BasePriority = -((HIGH_PRIORITY + 1) / 2);
        }

        KeSetBasePriorityThread(&Thread->Tcb, BasePriority);
        ObDereferenceObject(Thread);
        return TRUE;
    } else {
        XapiSetLastNTError(Status);
        return FALSE;
    }
}

int
APIENTRY
GetThreadPriority(
    HANDLE hThread
    )

/*++

Routine Description:

    The specified thread's priority can be read using GetThreadPriority.

Arguments:

    hThread - Supplies a handle to the thread whose priority is to be
        set.  The handle must have been created with
        THREAD_QUERY_INFORMATION access.

Return Value:

    The value of the thread's current priority is returned.  If an error
    occured, the value THREAD_PRIORITY_ERROR_RETURN is returned.
    Extended error status is available using GetLastError.

--*/

{
    NTSTATUS Status;
    PETHREAD Thread;
    int returnvalue;

    Status = ObReferenceObjectByHandle(hThread,
                                       PsThreadObjectType,
                                       (PVOID *)&Thread);

    if (NT_SUCCESS(Status)) {

        returnvalue = (int)KeQueryBasePriorityThread(&Thread->Tcb);
        if (returnvalue == ((HIGH_PRIORITY + 1) / 2)) {
            returnvalue = THREAD_PRIORITY_TIME_CRITICAL;
        } else if (returnvalue == -((HIGH_PRIORITY + 1) / 2)) {
            returnvalue = THREAD_PRIORITY_IDLE;
        }

        ObDereferenceObject(Thread);
        return returnvalue;
    } else {
        XapiSetLastNTError(Status);
        return (int)THREAD_PRIORITY_ERROR_RETURN;
    }
}

BOOL
WINAPI
SetThreadPriorityBoost(
    HANDLE hThread,
    BOOL bDisablePriorityBoost
    )
{
    NTSTATUS Status;
    PETHREAD Thread;

    Status = ObReferenceObjectByHandle(hThread,
                                       PsThreadObjectType,
                                       (PVOID *)&Thread);

    if (NT_SUCCESS(Status)) {
        KeSetDisableBoostThread(&Thread->Tcb, bDisablePriorityBoost ? TRUE : FALSE);
        ObDereferenceObject(Thread);
        return TRUE;
    } else {
        XapiSetLastNTError(Status);
        return FALSE;
    }
}

BOOL
WINAPI
GetThreadPriorityBoost(
    HANDLE hThread,
    PBOOL pDisablePriorityBoost
    )
{
    NTSTATUS Status;
    PETHREAD Thread;

    Status = ObReferenceObjectByHandle(hThread,
                                       PsThreadObjectType,
                                       (PVOID *)&Thread);

    if (NT_SUCCESS(Status)) {
        *pDisablePriorityBoost = Thread->Tcb.DisableBoost ? TRUE : FALSE;
        ObDereferenceObject(Thread);
        return TRUE;
    } else {
        XapiSetLastNTError(Status);
        return FALSE;
    }
}

DWORD
APIENTRY
GetCurrentThreadId(
    VOID
    )

/*++

Routine Description:

    The thread ID of the current thread may be retrieved using
    GetCurrentThreadId.

Arguments:

    None.

Return Value:

    Returns a unique value representing the thread ID of the currently
    executing thread.  The return value may be used to identify a thread
    in the system.

--*/

{
    return HandleToUlong(PsGetCurrentThreadId());
}

DWORD
APIENTRY
SuspendThread(
    HANDLE hThread
    )

/*++

Routine Description:

    A thread can be suspended using SuspendThread.

    Suspending a thread causes the thread to stop executing user-mode
    (or application) code.  Each thread has a suspend count (with a
    maximum value of MAXIMUM_SUSPEND_COUNT).  If the suspend count is
    greater than zero, the thread is suspended; otherwise, the thread is
    not suspended and is eligible for execution.

    Calling SuspendThread causes the target thread's suspend count to
    increment.  Attempting to increment past the maximum suspend count
    causes an error without incrementing the count.

Arguments:

    hThread - Supplies a handle to the thread that is to be suspended.
        The handle must have been created with THREAD_SUSPEND_RESUME
        access to the thread.

Return Value:

    -1 - The operation failed.  Extended error status is available using
         GetLastError.

    Other - The target thread was suspended. The return value is the thread's
        previous suspend count.

--*/

{
    NTSTATUS Status;
    DWORD PreviousSuspendCount;

    Status = NtSuspendThread(hThread,&PreviousSuspendCount);

    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return (DWORD)-1;
        }
    else {
        return PreviousSuspendCount;
        }
}

DWORD
APIENTRY
ResumeThread(
    IN HANDLE hThread
    )

/*++

Routine Description:

    A thread can be resumed using ResumeThread.

    Resuming a thread object checks the suspend count of the subject
    thread.  If the suspend count is zero, then the thread is not
    currently suspended and no operation is performed.  Otherwise, the
    subject thread's suspend count is decremented.  If the resultant
    value is zero , then the execution of the subject thread is resumed.

    The previous suspend count is returned as the function value.  If
    the return value is zero, then the subject thread was not previously
    suspended.  If the return value is one, then the subject thread's
    the subject thread is still suspended and must be resumed the number
    of times specified by the return value minus one before it will
    actually resume execution.

    Note that while reporting debug events, all threads withing the
    reporting process are frozen.  This has nothing to do with
    SuspendThread or ResumeThread.  Debuggers are expected to use
    SuspendThread and ResumeThread to limit the set of threads that can
    execute within a process.  By suspending all threads in a process
    except for the one reporting a debug event, it is possible to
    "single step" a single thread.  The other threads will not be
    released by a continue if they are suspended.

Arguments:

    hThread - Supplies a handle to the thread that is to be resumed.
        The handle must have been created with THREAD_SUSPEND_RESUME
        access to the thread.

Return Value:

    -1 - The operation failed.  Extended error status is available using
        GetLastError.

    Other - The target thread was resumed (or was not previously
        suspended).  The return value is the thread's previous suspend
        count.

--*/

{
    NTSTATUS Status;
    DWORD PreviousSuspendCount;

    Status = NtResumeThread(hThread,&PreviousSuspendCount);

    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return (DWORD)-1;
        }
    else {
        return PreviousSuspendCount;
        }
}

VOID
APIENTRY
RaiseException(
    DWORD dwExceptionCode,
    DWORD dwExceptionFlags,
    DWORD nNumberOfArguments,
    CONST ULONG_PTR *lpArguments
    )

/*++

Routine Description:

    Raising an exception causes the exception dispatcher to go through
    its search for an exception handler.  This includes debugger
    notification, frame based handler searching, and system default
    actions.

Arguments:

    dwExceptionCode - Supplies the exception code of the exception being
        raised.  This value may be obtained in exception filters and in
        exception handlers by calling GetExceptionCode.

    dwExceptionFlags - Supplies a set of flags associated with the exception.

    dwExceptionFlags Flags:

        EXCEPTION_NONCONTINUABLE - The exception is non-continuable.
            Returning EXCEPTION_CONTINUE_EXECUTION from an exception
            marked in this way causes the
            STATUS_NONCONTINUABLE_EXCEPTION exception.

    nNumberOfArguments - Supplies the number of arguments associated
        with the exception.  This value may not exceed
        EXCEPTION_MAXIMUM_PARAMETERS.  This parameter is ignored if
        lpArguments is NULL.

    lpArguments - An optional parameter, that if present supplies the
        arguments for the exception.

Return Value:

    None.

--*/

{
    EXCEPTION_RECORD ExceptionRecord;
    ULONG n;
    PULONG_PTR s,d;
    ExceptionRecord.ExceptionCode = (DWORD)dwExceptionCode;
    ExceptionRecord.ExceptionFlags = dwExceptionFlags & EXCEPTION_NONCONTINUABLE;
    ExceptionRecord.ExceptionRecord = NULL;
    ExceptionRecord.ExceptionAddress = (PVOID)RaiseException;
    if ( ARGUMENT_PRESENT(lpArguments) ) {
        n =  nNumberOfArguments;
        if ( n > EXCEPTION_MAXIMUM_PARAMETERS ) {
            n = EXCEPTION_MAXIMUM_PARAMETERS;
            }
        ExceptionRecord.NumberParameters = n;
        s = (PULONG_PTR)lpArguments;
        d = ExceptionRecord.ExceptionInformation;
        while(n--){
            *d++ = *s++;
            }
        }
    else {
        ExceptionRecord.NumberParameters = 0;
        }
    RtlRaiseException(&ExceptionRecord);
}

VOID
XapiDispatchAPC(
    LPVOID lpApcArgument1,
    LPVOID lpApcArgument2,
    LPVOID lpApcArgument3
    )
{
    PAPCFUNC pfnAPC;
    ULONG_PTR dwData;

    pfnAPC = (PAPCFUNC)lpApcArgument1;
    dwData = (ULONG_PTR)lpApcArgument2;
    (pfnAPC)(dwData);
}


WINBASEAPI
DWORD
WINAPI
QueueUserAPC(
    PAPCFUNC pfnAPC,
    HANDLE hThread,
    ULONG_PTR dwData
    )
/*++

Routine Description:

    This function is used to queue a user-mode APC to the specified thread. The APC
    will fire when the specified thread does an alertable wait.

Arguments:

    pfnAPC - Supplies the address of the APC routine to execute when the
        APC fires.

    hHandle - Supplies a handle to a thread object.  The caller
        must have THREAD_SET_CONTEXT access to the thread.

    dwData - Supplies a DWORD passed to the APC

Return Value:

    TRUE - The operations was successful

    FALSE - The operation failed. GetLastError() is not defined.

--*/

{
    NTSTATUS Status;

    Status = NtQueueApcThread(
                hThread,
                (PPS_APC_ROUTINE)XapiDispatchAPC,
                (PVOID)pfnAPC,
                (PVOID)dwData,
                NULL
                );

    if ( !NT_SUCCESS(Status) ) {
        return 0;
        }
    return 1;
}

BOOL
WINAPI
SwitchToThread(
    VOID
    )

/*++

Routine Description:

    This function causes a yield from the running thread to any other
    thread that is ready and can run on the current processor.  The
    yield will be effective for up to one quantum and then the yielding
    thread will be scheduled again according to its priority and
    whatever other threads may also be avaliable to run.  The thread
    that yields will not bounce to another processor even it another
    processor is idle or running a lower priority thread.

Arguments:

    None

Return Value:

    TRUE - Calling this function caused a switch to another thread to occur
    FALSE - There were no other ready threads, so no context switch occured

--*/

{

    if ( NtYieldExecution() == STATUS_NO_YIELD_PERFORMED ) {
        return FALSE;
        }
    else {
        return TRUE;
        }
}

VOID
APIENTRY
ExitThread(
    DWORD dwExitCode
    )

/*++

Routine Description:

    The current thread can exit using ExitThread.

    ExitThread is the prefered method of exiting a thread.  When this
    API is called (either explicitly or by returning from a thread
    procedure), The current thread's stack is deallocated and the thread
    terminates.  If the thread is the last thread in the process when
    this API is called, the behavior of this API does not change.  DLLs
    are not notified as a result of a call to ExitThread.

Arguments:

    dwExitCode - Supplies the termination status for the thread.

Return Value:

    None.

--*/

{
    XapiCallThreadNotifyRoutines(FALSE);

    PsTerminateSystemThread(dwExitCode);
}

BOOL
APIENTRY
GetExitCodeThread(
    HANDLE hThread,
    LPDWORD lpExitCode
    )

/*++

Routine Description:

    The termination status of a thread can be read using
    GetExitCodeThread.

    If a Thread is in the signaled state, calling this function returns
    the termination status of the thread.  If the thread is not yet
    signaled, the termination status returned is STILL_ACTIVE.

Arguments:

    hThread - Supplies a handle to the thread whose termination status is
        to be read.  The handle must have been created with
        THREAD_QUERY_INFORMATION access.

    lpExitCode - Returns the current termination status of the
        thread.

Return Value:

    TRUE - The operation was successful

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    PETHREAD Thread;

    Status = ObReferenceObjectByHandle(hThread,
                                       PsThreadObjectType,
                                       (PVOID *)&Thread);

    if (NT_SUCCESS(Status)) {
        *lpExitCode = KeReadStateThread(&Thread->Tcb) ? Thread->ExitStatus : STATUS_PENDING;
        ObDereferenceObject(Thread);
        return TRUE;
    } else {
        XapiSetLastNTError(Status);
        return FALSE;
    }
}

LPTOP_LEVEL_EXCEPTION_FILTER
WINAPI
SetUnhandledExceptionFilter(
    LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
    )

/*++

Routine Description:

    This function allows an application to supersede the top level
    exception handler that Win32 places at the top of each thread and
    process.

    If an exception occurs, and it makes it to the Win32 unhandled
    exception filter, and the process is not being debugged, the Win32
    filter will call the unhandled exception filter specified by
    lpTopLevelExceptionFilter.

    This filter may return:

        EXCEPTION_EXECUTE_HANDLER - Return from the Win32
            UnhandledExceptionFilter and execute the associated
            exception handler.  This will usually result in process
            termination

        EXCEPTION_CONTINUE_EXECUTION - Return from the Win32
            UnhandledExceptionFilter and continue execution from the
            point of the exception.  The filter is of course free to
            modify the continuation state my modifying the passed
            exception information.

        EXCEPTION_CONTINUE_SEARCH - Proceed with normal execution of the
            Win32 UnhandledExceptionFilter.  e.g.  obey the SetErrorMode
            flags, or invoke the Application Error popup.

    This function is not a general vectored exception handling
    mechanism.  It is intended to be used to establish a per-process
    exception filter that can monitor unhandled exceptions at the
    process level and respond to these exceptions appropriately.

Arguments:

    lpTopLevelExceptionFilter - Supplies the address of a top level
        filter function that will be called whenever the Win32
        UnhandledExceptionFilter gets control, and the process is NOT
        being debugged.  A value of NULL specifies default handling
        within the Win32 UnhandledExceptionFilter.

Return Value:

    This function returns the address of the previous exception filter
    established with this API.  A value of NULL means that there is no
    current top level handler.

--*/

{
    LPTOP_LEVEL_EXCEPTION_FILTER PreviousTopLevelFilter;

    PreviousTopLevelFilter = XapiCurrentTopLevelFilter;
    XapiCurrentTopLevelFilter = lpTopLevelExceptionFilter;

    return PreviousTopLevelFilter;
}

BOOL
WINAPI
GetThreadTimes(
    HANDLE hThread,
    LPFILETIME lpCreationTime,
    LPFILETIME lpExitTime,
    LPFILETIME lpKernelTime,
    LPFILETIME lpUserTime
    )

/*++

Routine Description:

    This function is used to return various timing information about the
    thread specified by hThread.

    All times are in units of 100ns increments. For lpCreationTime and lpExitTime,
    the times are in terms of the SYSTEM time or GMT time.

Arguments:

    hThread - Supplies an open handle to the specified thread.  The
        handle must have been created with THREAD_QUERY_INFORMATION
        access.

    lpCreationTime - Returns a creation time of the thread.

    lpExitTime - Returns the exit time of a thread.  If the thread has
        not exited, this value is not defined.

    lpKernelTime - Returns the amount of time that this thread has
        executed in kernel-mode.

    lpUserTime - Returns the amount of time that this thread has
        executed in user-mode.


Return Value:

    TRUE - The API was successful

    FALSE - The operation failed.  Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    PETHREAD Thread;

    Status = ObReferenceObjectByHandle(hThread,
                                       PsThreadObjectType,
                                       (PVOID *)&Thread);

    if (NT_SUCCESS(Status)) {

        *((PLARGE_INTEGER)lpCreationTime) = Thread->CreateTime;

        if (KeReadStateThread(&Thread->Tcb)) {
            *((PLARGE_INTEGER)lpExitTime) = Thread->ExitTime;
        } else {
            ((PLARGE_INTEGER)lpExitTime)->QuadPart = 0;
        }

        ((PLARGE_INTEGER)lpKernelTime)->QuadPart = UInt32x32To64(Thread->Tcb.KernelTime,
            *KeTimeIncrement);
        ((PLARGE_INTEGER)lpUserTime)->QuadPart = 0;

        ObDereferenceObject(Thread);
        return TRUE;
    } else {
        XapiSetLastNTError(Status);
        return FALSE;
    }

    return TRUE;
}

XBOXAPI
VOID
WINAPI
XRegisterThreadNotifyRoutine(
    PXTHREAD_NOTIFICATION pThreadNotification,
    BOOL fRegister
    )

/*++

Routine Description:

    Register or deregister a notification routine
    which will be called when a thread is created or deleted.

Arguments:

    pThreadNotification - Points to a XTHREAD_NOTIFICATION structure
        NOTE: The XTHREAD_NOTIFICATION structure must remain
        valid until the thread notification routine is deregistered.
        For example, you can use a global variable for this.
        But you should NOT use a local variable inside a function.

    fRegister - TRUE to register a new thread notification routine
        FALSE to deregister a previously registered notification routine

Return Value:

    None.

--*/

{
    XapiAcquireProcessLock();

    if (fRegister) {
        InsertTailList(&XapiThreadNotifyRoutineList,
            &pThreadNotification->ListEntry);
    } else {
        RemoveEntryList(&pThreadNotification->ListEntry);
    }

    XapiReleaseProcessLock();
}

XBOXAPI
VOID
XSetProcessQuantumLength(
    IN DWORD dwMilliseconds
    )

/*++

Routine Description:

    This routine changes the quantum length for the current process.  Changing
    the quantum length does not affect the quantum of the current thread.

    The quantum length determines the number of milliseconds that a thread is
    executed before the scheduler selects the next ready thread to execute.

Arguments:

    dwMilliseconds - Supplies the number of milliseconds for the process quantum
        length.

Return Value:

    None.

--*/

{
    LONG ThreadQuantum;

    if (dwMilliseconds == 0 || dwMilliseconds > 1000) {
        RIP("XSetProcessQuantumSize() invalid parameter (dwMilliseconds)");
    }

    ThreadQuantum = dwMilliseconds * CLOCK_QUANTUM_DECREMENT;
    KeGetCurrentThread()->ApcState.Process->ThreadQuantum = ThreadQuantum;
}

XBOXAPI
DWORD
XGetProcessQuantumLength(
    VOID
    )

/*++

Routine Description:

    This routine returns the quantum length for the current process.

    The quantum length determines the number of milliseconds that a thread is
    executed before the scheduler selects the next ready thread to execute.

Arguments:

    None.

Return Value:

    Returns the number of milliseconds for the process quantum length.

--*/

{
    LONG ThreadQuantum;

    ThreadQuantum = KeGetCurrentThread()->ApcState.Process->ThreadQuantum;

    return ThreadQuantum / CLOCK_QUANTUM_DECREMENT;
}
