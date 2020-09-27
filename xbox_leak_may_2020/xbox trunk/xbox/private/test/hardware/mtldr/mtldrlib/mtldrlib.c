/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    mtldrlib.c

Abstract:

    This module contains useful routines to access kernel APIs

--*/

#include "mtldrp.h"

__declspec(thread) DWORD ThreadLastError = 0;

//
// Special symbols to mark start and end of Thread Local Storage area.
//

ULONG _tls_index = 0;

#pragma data_seg(".tls")

char _tls_start = 0;

#pragma data_seg(".tls$ZZZ")

char _tls_end = 0;

//
// Start and end sections for Thread Local Storage CallBack Array.
// Actual array is constructed using .CRT$XLA, .CRT$XLC, .CRT$XLL, .CRT$XLU,
// .CRT$XLZ similar to the way global static initializers are done for C++.
//

#pragma data_seg(".CRT$XLA")

PIMAGE_TLS_CALLBACK __xl_a = 0;

#pragma data_seg(".CRT$XLZ")

PIMAGE_TLS_CALLBACK __xl_z = 0;

#pragma data_seg(".rdata$T")

IMAGE_TLS_DIRECTORY _tls_used = {
    (ULONG)(ULONG_PTR)&_tls_start,  // start of tls data
    (ULONG)(ULONG_PTR)&_tls_end,    // end of tls data
    (ULONG)(ULONG_PTR)&_tls_index,  // address of tls_index
    (ULONG)(ULONG_PTR)&__xl_a,      // pointer to call back array
    (ULONG)0,                       // size of tls zero fill
    (ULONG)0                        // characteristics
};

#pragma data_seg()

#pragma comment(linker, "/merge:.CRT=.data")

PLDR_CURRENT_IMAGE XeLdrImage = NULL;

PLARGE_INTEGER
LdrpFormatTimeOut(
    OUT PLARGE_INTEGER TimeOut,
    IN ULONG Milliseconds
    )
/*++

Routine Description:

    This function translates a Win32 style timeout to an NT relative
    timeout value.

Arguments:

    TimeOut - Returns an initialized NT timeout value that is equivalent
         to the Milliseconds parameter.

    Milliseconds - Supplies the timeout value in milliseconds.  A value
         of -1 indicates indefinite timeout.

Return Value:


    NULL - A value of null should be used to mimic the behavior of the
        specified Milliseconds parameter.

    NON-NULL - Returns the TimeOut value.  The structure is properly
        initialized by this function.

--*/
{
    if ( (LONG) Milliseconds == -1 ) {
        TimeOut->LowPart = 0x0;
        TimeOut->HighPart = 0x80000000;
    } else {
        TimeOut->QuadPart = UInt32x32To64( Milliseconds, 10000 );
        TimeOut->QuadPart *= -1;
    }

    return TimeOut;
}

VOID
NTAPI
SetLastError(
    DWORD dwErrCode
    )
/*++

Routine Description:

    This function set the most recent error code and error string in per
    thread storage.  Win32 API functions call this function whenever
    they return a failure indication (e.g.  FALSE, NULL or -1).

    This function is not called by Win32 API function calls that are
    successful, so that if three Win32 API function calls are made, and
    the first one fails and the second two succeed, the error code and
    string stored by the first one are still available after the second
    two succeed.

    Applications can retrieve the values saved by this function using
    GetLastError.  The use of this function is optional, as an
    application need only call if it is interested in knowing the
    specific reason for an API function failure.

    The last error code value is kept in thread local storage so that
    multiple threads do not overwrite each other's values.

Arguments:

    dwErrCode - Specifies the error code to store in per thread storage
        for the current thread.

Return Value:

    return-value - Description of conditions needed to return value. - or -
    None.

--*/
{
    ASSERT( KeGetCurrentThread()->TlsData );
    ThreadLastError = dwErrCode;
}

DWORD
NTAPI
GetLastError(
    VOID
    )
/*++

Routine Description:

    This function returns the most recent error code set by a Win32 API
    call.  Applications should call this function immediately after a
    Win32 API call returns a failure indications (e.g.  FALSE, NULL or
    -1) to determine the cause of the failure.

    The last error code value is a per thread field, so that multiple
    threads do not overwrite each other's last error code value.

Arguments:

    None.

Return Value:

    The return value is the most recent error code as set by a Win32 API
    call.

--*/
{
    ASSERT( KeGetCurrentThread()->TlsData );
    return ThreadLastError;
}

BOOL
NTAPI
CloseHandle(
    HANDLE hObject
    )
/*++

Routine Description:

    An open handle to any object can be closed using CloseHandle.

    This is a generic function and operates on the following object
    types:

        - Process Object

        - Thread Object

        - Mutex Object

        - Event Object

        - Semaphore Object

        - File Object

    Please note that Module Objects are not in this list.

    Closing an open handle to an object causes the handle to become
    invalid and the HandleCount of the associated object to be
    decremented and object retention checks to be performed.  Once the
    last open handle to an object is closed, the object is removed from
    the system.

Arguments:

    hObject - An open handle to an object.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    NTSTATUS Status;

    Status = NtClose(hObject);
    if ( NT_SUCCESS(Status) ) {
        return TRUE;
    } else {
        SetLastError( RtlNtStatusToDosError(Status) );
        return FALSE;
    }
}

HANDLE
NTAPI
CreateEvent(
    PVOID lpEventAttributes,
    BOOL bManualReset,
    BOOL bInitialState,
    PCOSTR lpName
    )
/*++

Routine Description:

    An event object is created and a handle opened for access to the
    object with the CreateEvent function.

    The CreateEvent function creates an event object with the specified
    initial state.  If an event is in the Signaled state (TRUE), a wait
    operation on the event does not block.  If the event is in the Not-
    Signaled state (FALSE), a wait operation on the event blocks until
    the specified event attains a state of Signaled, or the timeout
    value is exceeded.

    In addition to the STANDARD_RIGHTS_REQUIRED access flags, the following
    object type specific access flags are valid for event objects:

        - EVENT_MODIFY_STATE - Modify state access (set and reset) to
          the event is desired.

        - SYNCHRONIZE - Synchronization access (wait) to the event is
          desired.

        - EVENT_ALL_ACCESS - This set of access flags specifies all of
          the possible access flags for an event object.


Arguments:

    lpEventAttributes - Ignore

    bManualReset - Supplies a flag which if TRUE specifies that the
        event must be manually reset.  If the value is FALSE, then after
        releasing a single waiter, the system automaticaly resets the
        event.

    bInitialState - The initial state of the event object, one of TRUE
        or FALSE.  If the InitialState is specified as TRUE, the event's
        current state value is set to one, otherwise it is set to zero.

    lpName - Optional unicode name of event

Return Value:

    NON-NULL - Returns a handle to the new event.  The handle has full
        access to the new event and may be used in any API that requires
        a handle to an event object.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    POBJECT_ATTRIBUTES pObja;
    HANDLE Handle;
    OBJECT_STRING ObjectName;

    if ( ARGUMENT_PRESENT(lpName) ) {
        pObja = &Obja;
        RtlInitObjectString(&ObjectName, lpName);
        InitializeObjectAttributes(
            &Obja,
            &ObjectName,
            OBJ_OPENIF,
            NULL,
            NULL
            );
    } else {
        pObja = NULL;
    }

    Status = NtCreateEvent(
                &Handle,
                pObja,
                bManualReset ? NotificationEvent : SynchronizationEvent,
                (BOOLEAN)bInitialState
                );

    if ( NT_SUCCESS(Status) ) {
        if ( Status == STATUS_OBJECT_NAME_EXISTS ) {
            SetLastError(ERROR_ALREADY_EXISTS);
        } else {
            SetLastError(0);
        }
        return Handle;
    } else {
        SetLastError(RtlNtStatusToDosError(Status));
        return NULL;
    }
}

BOOL
SetEvent(
    HANDLE hEvent
    )
/*++

Routine Description:

    An event can be set to the signaled state (TRUE) with the SetEvent
    function.

    Setting the event causes the event to attain a state of Signaled,
    which releases all currently waiting threads (for manual reset
    events), or a single waiting thread (for automatic reset events).

Arguments:

    hEvent - Supplies an open handle to an event object.  The
        handle must have EVENT_MODIFY_STATE access to the event.

Return Value:

    TRUE - The operation was successful

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    NTSTATUS Status;

    Status = NtSetEvent(hEvent,NULL);
    if ( NT_SUCCESS(Status) ) {
        return TRUE;
    } else {
        SetLastError(RtlNtStatusToDosError(Status));
        return FALSE;
    }
}

BOOL
ResetEvent(
    HANDLE hEvent
    )
/*++

Routine Description:

    The state of an event is set to the Not-Signaled state (FALSE) using
    the ClearEvent function.

    Once the event attains a state of Not-Signaled, any threads which
    wait on the event block, awaiting the event to become Signaled.  The
    reset event service sets the event count to zero for the state of
    the event.

Arguments:

    hEvent - Supplies an open handle to an event object.  The
        handle must have EVENT_MODIFY_STATE access to the event.

Return Value:

    TRUE - The operation was successful

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    NTSTATUS Status;

    Status = NtClearEvent(hEvent);
    if ( NT_SUCCESS(Status) ) {
        return TRUE;
    } else {
        SetLastError(RtlNtStatusToDosError(Status));
        return FALSE;
    }
}

HLOCAL
WINAPI
LocalAlloc(
    UINT uFlags,
    SIZE_T uBytes
    )
/*++

Routine Description:

    The LocalAlloc function allocates the specified number of bytes from the heap.
    In the linear Win32 API environment, there is no difference between the
    local heap and the global heap.

    NOTE: Moveable memory is not implemented. LMEM_FIXED is always on.

Arguments:

    UINT uFlags - allocation attributes
    UINT uBytes - number of bytes to allocate

Return Value:

    Returns pointer to newly allocated memory or NULL on failure.

--*/
{
    PVOID p = NULL;
    SIZE_T RegionSize = uBytes;

    NTSTATUS Status = NtAllocateVirtualMemory( &p, 0, &RegionSize, 
        MEM_COMMIT | MEM_NOZERO, PAGE_EXECUTE_READWRITE );

    if ( NT_SUCCESS(Status) )  {
        if ( uFlags & LMEM_ZEROINIT ) {
            RtlZeroMemory( p, uBytes );
        }
    } else {
        p = NULL;
        SetLastError( RtlNtStatusToDosError(Status) );
    }

    return (HLOCAL)p;
}

HLOCAL
WINAPI
LocalFree(
    HLOCAL hMem
    )
/*++

Routine Description:

    The LocalFree function frees the specified local memory object and
    invalidates its handle. Also implements GlobalFree().

Arguments:

    hMem - handle of local memory object

Return Value:

    If the function succeeds, the return value is NULL. If the function fails,
    the return value is equal to the handle of the local memory object.

--*/
{
    SIZE_T s = 0;
    PVOID p = (PVOID)hMem;

    if ( !p ) {
        return NULL;
    }

    NtFreeVirtualMemory( &p, &s, MEM_RELEASE );
    return NULL;
}

DWORD
WaitForSingleObject(
    HANDLE hHandle,
    DWORD dwMilliseconds
    )

/*++

Routine Description:

    A wait operation on a waitable object is accomplished with the
    WaitForSingleObject function.

    Waiting on an object checks the current state of the object.  If the
    current state of the object allows continued execution, any
    adjustments to the object state are made (for example, decrementing
    the semaphore count for a semaphore object) and the thread continues
    execution.  If the current state of the object does not allow
    continued execution, the thread is placed into the wait state
    pending the change of the object's state or time-out.

Arguments:

    hHandle - An open handle to a waitable object. The handle must have
        SYNCHRONIZE access to the object.

    dwMilliseconds - A time-out value that specifies the relative time,
        in milliseconds, over which the wait is to be completed.  A
        timeout value of 0 specified that the wait is to timeout
        immediately.  This allows an application to test an object to
        determine if it is in the signaled state.  A timeout value of -1
        specifies an infinite timeout period.

Return Value:

    WAIT_TIME_OUT - Indicates that the wait was terminated due to the
        TimeOut conditions.

    0 - indicates the specified object attained a Signaled
        state thus completing the wait.

    WAIT_ABANDONED - indicates the specified object attained a Signaled
        state but was abandoned.

--*/

{
    return WaitForSingleObjectEx(hHandle,dwMilliseconds,FALSE);
}

DWORD
APIENTRY
WaitForSingleObjectEx(
    HANDLE hHandle,
    DWORD dwMilliseconds,
    BOOL bAlertable
    )

/*++

Routine Description:

    A wait operation on a waitable object is accomplished with the
    WaitForSingleObjectEx function.

    Waiting on an object checks the current state of the object.  If the
    current state of the object allows continued execution, any
    adjustments to the object state are made (for example, decrementing
    the semaphore count for a semaphore object) and the thread continues
    execution.  If the current state of the object does not allow
    continued execution, the thread is placed into the wait state
    pending the change of the object's state or time-out.

    If the bAlertable parameter is FALSE, the only way the wait
    terminates is because the specified timeout period expires, or
    because the specified object entered the signaled state.  If the
    bAlertable parameter is TRUE, then the wait can return due to any
    one of the above wait termination conditions, or because an I/O
    completion callback terminated the wait early (return value of
    WAIT_IO_COMPLETION).

Arguments:

    hHandle - An open handle to a waitable object. The handle must have
        SYNCHRONIZE access to the object.

    dwMilliseconds - A time-out value that specifies the relative time,
        in milliseconds, over which the wait is to be completed.  A
        timeout value of 0 specified that the wait is to timeout
        immediately.  This allows an application to test an object to
        determine if it is in the signaled state.  A timeout value of
        0xffffffff specifies an infinite timeout period.

    bAlertable - Supplies a flag that controls whether or not the
        wait may terminate early due to an I/O completion callback.
        A value of TRUE allows this API to complete early due to an I/O
        completion callback.  A value of FALSE will not allow I/O
        completion callbacks to terminate this call early.

Return Value:

    WAIT_TIME_OUT - Indicates that the wait was terminated due to the
        TimeOut conditions.

    0 - indicates the specified object attained a Signaled
        state thus completing the wait.

    0xffffffff - The wait terminated due to an error. GetLastError may be
        used to get additional error information.

    WAIT_ABANDONED - indicates the specified object attained a Signaled
        state but was abandoned.

    WAIT_IO_COMPLETION - The wait terminated due to one or more I/O
        completion callbacks.

--*/
{
    NTSTATUS Status;
    LARGE_INTEGER TimeOut;
    PLARGE_INTEGER pTimeOut;

    pTimeOut = LdrpFormatTimeOut(&TimeOut,dwMilliseconds);
rewait:
    Status = NtWaitForSingleObjectEx(hHandle,
                                     UserMode,
                                     (BOOLEAN)bAlertable,
                                     pTimeOut);
    if ( !NT_SUCCESS(Status) ) {
        SetLastError(RtlNtStatusToDosError(Status));
        Status = (NTSTATUS)0xffffffff;
        }
    else {
        if ( bAlertable && Status == STATUS_ALERTED ) {
            goto rewait;
            }
        }
    return (DWORD)Status;
}

DWORD
WINAPI
WaitForMultipleObjectsEx(
    DWORD nCount,
    CONST HANDLE *lpHandles,
    BOOL bWaitAll,
    DWORD dwMilliseconds,
    BOOL bAlertable
    )
/*++

Routine Description:

    A wait operation on multiple waitable objects (up to
    MAXIMUM_WAIT_OBJECTS) is accomplished with the
    WaitForMultipleObjects function.

    This API can be used to wait on any of the specified objects to
    enter the signaled state, or all of the objects to enter the
    signaled state.

    If the bAlertable parameter is FALSE, the only way the wait
    terminates is because the specified timeout period expires, or
    because the specified objects entered the signaled state.  If the
    bAlertable parameter is TRUE, then the wait can return due to any one of
    the above wait termination conditions, or because an I/O completion
    callback terminated the wait early (return value of
    WAIT_IO_COMPLETION).

Arguments:

    nCount - A count of the number of objects that are to be waited on.

    lpHandles - An array of object handles.  Each handle must have
        SYNCHRONIZE access to the associated object.

    bWaitAll - A flag that supplies the wait type.  A value of TRUE
        indicates a "wait all".  A value of false indicates a "wait
        any".

    dwMilliseconds - A time-out value that specifies the relative time,
        in milliseconds, over which the wait is to be completed.  A
        timeout value of 0 specified that the wait is to timeout
        immediately.  This allows an application to test an object to
        determine if it is in the signaled state.  A timeout value of
        0xffffffff specifies an infinite timeout period.

    bAlertable - Supplies a flag that controls whether or not the
        wait may terminate early due to an I/O completion callback.
        A value of TRUE allows this API to complete early due to an I/O
        completion callback.  A value of FALSE will not allow I/O
        completion callbacks to terminate this call early.

Return Value:

    WAIT_TIME_OUT - indicates that the wait was terminated due to the
        TimeOut conditions.

    0 to MAXIMUM_WAIT_OBJECTS-1, indicates, in the case of wait for any
        object, the object number which satisfied the wait.  In the case
        of wait for all objects, the value only indicates that the wait
        was completed successfully.

    0xffffffff - The wait terminated due to an error. GetLastError may be
        used to get additional error information.

    WAIT_ABANDONED_0 to (WAIT_ABANDONED_0)+(MAXIMUM_WAIT_OBJECTS - 1),
        indicates, in the case of wait for any object, the object number
        which satisfied the event, and that the object which satisfied
        the event was abandoned.  In the case of wait for all objects,
        the value indicates that the wait was completed successfully and
        at least one of the objects was abandoned.

    WAIT_IO_COMPLETION - The wait terminated due to one or more I/O
        completion callbacks.

--*/
{
    NTSTATUS Status;
    LARGE_INTEGER TimeOut;
    PLARGE_INTEGER pTimeOut;
    HANDLE HandleArray[MAXIMUM_WAIT_OBJECTS];

    if (nCount > MAXIMUM_WAIT_OBJECTS) {
        SetLastError(RtlNtStatusToDosError(STATUS_INVALID_PARAMETER));
        return (DWORD)0xffffffff;
    }

    RtlCopyMemory(HandleArray,(LPVOID)lpHandles,nCount*sizeof(HANDLE));

    pTimeOut = LdrpFormatTimeOut(&TimeOut,dwMilliseconds);
rewait:
    Status = NtWaitForMultipleObjectsEx(
                 (CHAR)nCount,
                 HandleArray,
                 bWaitAll ? WaitAll : WaitAny,
                 UserMode,
                 (BOOLEAN)bAlertable,
                 pTimeOut
                 );
    if ( !NT_SUCCESS(Status) ) {
        SetLastError(RtlNtStatusToDosError(Status));
        Status = (NTSTATUS)0xffffffff;
    } else {
        if ( bAlertable && Status == STATUS_ALERTED ) {
            goto rewait;
        }
    }

    return (DWORD)Status;
}

VOID
Sleep(
    DWORD dwMilliseconds
    )

/*++

Routine Description:

    The execution of the current thread can be delayed for a specified
    interval of time with the Sleep function.

    The Sleep function causes the current thread to enter a
    waiting state until the specified interval of time has passed.

Arguments:

    dwMilliseconds - A time-out value that specifies the relative time,
        in milliseconds, over which the wait is to be completed.  A
        timeout value of 0 specified that the wait is to timeout
        immediately.  This allows an application to test an object to
        determine if it is in the signaled state.  A timeout value of -1
        specifies an infinite timeout period.

Return Value:

    None.

--*/

{
    SleepEx(dwMilliseconds,FALSE);
}

DWORD
APIENTRY
SleepEx(
    DWORD dwMilliseconds,
    BOOL bAlertable
    )

/*++

Routine Description:

    The execution of the current thread can be delayed for a specified
    interval of time with the SleepEx function.

    The SleepEx function causes the current thread to enter a waiting
    state until the specified interval of time has passed.

    If the bAlertable parameter is FALSE, the only way the SleepEx
    returns is when the specified time interval has passed.  If the
    bAlertable parameter is TRUE, then the SleepEx can return due to the
    expiration of the time interval (return value of 0), or because an
    I/O completion callback terminated the SleepEx early (return value
    of WAIT_IO_COMPLETION).

Arguments:

    dwMilliseconds - A time-out value that specifies the relative time,
        in milliseconds, over which the wait is to be completed.  A
        timeout value of 0 specified that the wait is to timeout
        immediately.  A timeout value of -1 specifies an infinite
        timeout period.

    bAlertable - Supplies a flag that controls whether or not the
        SleepEx may terminate early due to an I/O completion callback.
        A value of TRUE allows this API to complete early due to an I/O
        completion callback.  A value of FALSE will not allow I/O
        completion callbacks to terminate this call early.

Return Value:

    0 - The SleepEx terminated due to expiration of the time interval.

    WAIT_IO_COMPLETION - The SleepEx terminated due to one or more I/O
        completion callbacks.

--*/
{
    LARGE_INTEGER TimeOut;
    PLARGE_INTEGER pTimeOut;
    NTSTATUS Status;

    pTimeOut = LdrpFormatTimeOut(&TimeOut,dwMilliseconds);
    if (pTimeOut == NULL) {
        //
        // If Sleep( -1 ) then delay for the longest possible integer
        // relative to now.
        //

        TimeOut.LowPart = 0x0;
        TimeOut.HighPart = 0x80000000;
        pTimeOut = &TimeOut;
        }

rewait:
    Status = KeDelayExecutionThread(
                UserMode,
                (BOOLEAN)bAlertable,
                pTimeOut
                );
    if ( bAlertable && Status == STATUS_ALERTED ) {
        goto rewait;
        }
    return Status == STATUS_USER_APC ? WAIT_IO_COMPLETION : 0;
}

VOID
MtLdrThreadStartup(
    IN PKSTART_ROUTINE StartRoutine,
    IN PVOID StartContext
    )
/*++

Routine Description:

    Called by the kernel on thread startup. We set up the default exception handler
    and a data structure to keep track of TLS data.

Arguments:

    StartRoutine - address the thread should be started at.

    StartContext - an LPVOID pointer passed in to CreateThread to pass on to the thread.

Return Value:

    None.

--*/
{
    DWORD dwExitCode = 0;
    PULONG TlsData;
    DWORD RawDataSize;

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    //
    // At the moment we don't wrap thread start routine in SEH.  We will let
    // the debugger catches second-chance exception (if any).  Debug will be
    // easier this way
    //
    // If you don't like this, remove the comment below
    //

    //try {

        //
        // Need to set up the TLS data
        //

        ASSERT(KeGetCurrentThread()->TlsData);

#if DBG
        //
        // First make sure the TLS data is where we think it should be
        //
        ASSERT((ULONG_PTR)KeGetCurrentThread()->TlsData -
            (ULONG_PTR)KeGetCurrentTlsDataTop() == *((PULONG)_tls_used.AddressOfIndex) * 4);
#endif

        //
        // Need to fill the first slot with a pointer to the remainder
        //
        TlsData = (PULONG)KeGetCurrentThread()->TlsData + 1;
        TlsData[-1] = (ULONG_PTR)TlsData;

        //
        // Ensure 16-byte alignment
        //
        ASSERT(((ULONG_PTR)TlsData & 15) == 0);

        RawDataSize = _tls_used.EndAddressOfRawData - _tls_used.StartAddressOfRawData;

        RtlCopyMemory(TlsData, (PVOID)_tls_used.StartAddressOfRawData,
            RawDataSize);

        if (_tls_used.SizeOfZeroFill != 0) {
            RtlZeroMemory((PBYTE)TlsData + RawDataSize, _tls_used.SizeOfZeroFill);
        }

        dwExitCode = (*(LPTHREAD_START_ROUTINE)StartRoutine)(StartContext);
    //} except (EXCEPTION_EXECUTE_HANDLER) {
        //ASSERT(FALSE);
    //}

    PsTerminateSystemThread(dwExitCode);
}

HANDLE
WINAPI
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
    SIZE_T TlsSize;

    if (dwStackSize == 0) {
        dwStackSize = XeLdrImage->ImageHeader->OptionalHeader.SizeOfStackCommit;
    }

    TlsSize = (_tls_used.EndAddressOfRawData - _tls_used.StartAddressOfRawData) \
        + _tls_used.SizeOfZeroFill;

    //
    // The base of the TLS section must be 16-byte aligned.  We know
    // that fs:StackBase is 16-byte aligned, ensuring that TlsSize is
    // 16-byte aligned is sufficient
    //

    TlsSize = (TlsSize + 15) & ~15;

    //
    // Add four bytes for the index pointer
    //

    TlsSize += 4;

    Status = PsCreateSystemThreadEx(
                &Handle,
                0,
                dwStackSize,
                TlsSize,
                (PHANDLE)lpThreadId,
                (PKSTART_ROUTINE)lpStartAddress,
                lpParameter,
                (BOOLEAN)((dwCreationFlags & CREATE_SUSPENDED) ? TRUE : FALSE),
                FALSE,
                (PKSYSTEM_ROUTINE)MtLdrThreadStartup
                );

    if (!NT_SUCCESS(Status)) {
        SetLastError( RtlNtStatusToDosError(Status) );
        return NULL;
    }

    return Handle;
}

VOID
APIENTRY
OutputDebugStringA(
    IN LPCSTR lpOutputString
    )
/*++

Routine Description:

    This function allows an application to send a string to its debugger
    for display.  If the application is not being debugged, but the
    system debugger is active, the system debugger displays the string.
    Otherwise, this function has no effect.

Arguments:

    lpOutputString - Supplies the address of the debug string to be sent
        to the debugger.

Return Value:

    None.

--*/
{
    DbgPrint( (PSTR)lpOutputString );
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

