/*
 *
 * api.c
 *
 * Implementation of the Win32 APIs needed for winsock, on top of ntoskrnl
 *
 */

#include "dmp.h"

void SetLastError(DWORD dwErrCode)
{
    DmGetCurrentDmtd()->LastError = dwErrCode;
}

DWORD GetLastError(void)
{
    return DmGetCurrentDmtd()->LastError;
}

#if 0
void DebugBreak(void)
{
    DbgBreakPoint();
}
#endif

HLOCAL LocalAlloc(UINT uFlags, SIZE_T cb)
{
    PVOID pv;

    if(uFlags & (LMEM_MOVEABLE | LMEM_DISCARDABLE))
        pv = NULL;
    else {
        pv = ExAllocatePool(PagedPool, cb + 8);
        if(pv) {
            *((DWORD *)pv)++ = 'kmem';
            *((DWORD *)pv)++ = 'dbgr';
            if(uFlags & LMEM_ZEROINIT)
                RtlZeroMemory(pv, cb);
            }
        }
    return (HLOCAL)pv;
}

HLOCAL LocalFree(HLOCAL h)
{
    HLOCAL hRet = h;
    DWORD *pdw = (DWORD *)h;
    try {
        if(h && pdw[-1] == 'dbgr' && pdw[-2] == 'kmem') {
            ExFreePool(pdw - 2);
            hRet = NULL;
        }
    } except (EXCEPTION_EXECUTE_HANDLER) {
    }
    return hRet;
}

void Sleep(DWORD dw)
{
    SleepEx(dw, FALSE);
}

DWORD SleepEx(DWORD dw, BOOL f)
{
    LARGE_INTEGER li;
    NTSTATUS st;

    if(dw == -1)
        {
        li.LowPart = 0;
        li.HighPart = 0x80000000;
        }
    else
        {
        li.QuadPart = UInt32x32To64(dw, 10000);
        li.QuadPart *= -1;
        }
    do
        st = KeDelayExecutionThread(UserMode, (BOOLEAN)f, &li);
    while(f && STATUS_ALERTED == st);
    return st == STATUS_USER_APC ? WAIT_IO_COMPLETION : 0;
}

HANDLE CreateEventW(LPSECURITY_ATTRIBUTES psa, BOOL fManualReset,
    BOOL fInitState, LPCWSTR lpwzName)
{
    NTSTATUS st;
    HANDLE h;
    OBJECT_ATTRIBUTES oa;

    ASSERT(NULL == lpwzName);

    InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);
    st = NtCreateEvent(&h, EVENT_ALL_ACCESS, &oa, fManualReset ?
        NotificationEvent : SynchronizationEvent, (BOOLEAN)fInitState);
    if(NT_SUCCESS(st))
        SetLastError(0);
    else
        {
        SetLastError(RtlNtStatusToDosError(st));
        h = NULL;
        }
    return h;
}

#if 0
ULONG _DmUnhandledException(PEXCEPTION_POINTERS pep)
{
    KeBugCheckEx(KMODE_EXCEPTION_NOT_HANDLED,
        (ULONG_PTR)pep->ExceptionRecord->ExceptionAddress, 0, 0, 0);
    return EXCEPTION_EXECUTE_HANDLER;
}

void _DmThreadStartup(PKSTART_ROUTINE StartRoutine, PVOID StartContext)
{
    DWORD dwExit;

    KeLowerIrql(0);
    if(FAllocDmThread()) {
        try {
            dwExit = (*(LPTHREAD_START_ROUTINE)StartRoutine)(StartContext);
        } except(_DmUnhandledException(GetExceptionInformation())) {
            KeBugCheck(KMODE_EXCEPTION_NOT_HANDLED);
        }
    } else
        dwExit = 0;
    
    PsTerminateSystemThread(dwExit);
}

HANDLE CreateThread(LPSECURITY_ATTRIBUTES psa, DWORD dwStack,
    LPTHREAD_START_ROUTINE lpfn, LPVOID lpv, DWORD dwFlags, LPDWORD lpdwId)
{
    NTSTATUS st;
    HANDLE h;

    st = PsCreateSystemThreadEx(&h, 0, dwStack, lpdwId, (PKSTART_ROUTINE)lpfn,
        lpv, (BOOLEAN)(dwFlags & CREATE_SUSPENDED ? TRUE : FALSE), NULL);
    
    if(!NT_SUCCESS(st))
        {
        SetLastError(RtlNtStatusToDosError(st));
        return NULL;
        }
    
    if(lpdwId)
        *lpdwId = HandleToUlong(clid.UniqueThread);
    
    return h;
}
#endif

BOOL CloseHandle(HANDLE h)
{
    NTSTATUS st = NtClose(h);
    if(NT_SUCCESS(st))
        return TRUE;
    SetLastError(RtlNtStatusToDosError(st));
    return FALSE;
}

BOOL SetEvent(HANDLE h)
{
    NTSTATUS st = NtSetEvent(h, NULL);
    if(NT_SUCCESS(st))
        return TRUE;
    SetLastError(RtlNtStatusToDosError(st));
    return FALSE;
}

BOOL ResetEvent(HANDLE h)
{
    NTSTATUS st = NtClearEvent(h);
    if(NT_SUCCESS(st))
        return TRUE;
    SetLastError(RtlNtStatusToDosError(st));
    return FALSE;
}

DWORD WaitForMultipleObjectsEx(DWORD nCount, const HANDLE *lph, BOOL fWaitAll,
    DWORD dwTimeOut, BOOL fAlertable)
{
    NTSTATUS st;
    LARGE_INTEGER li;
    PLARGE_INTEGER pli;
    HANDLE rgh[MAXIMUM_WAIT_OBJECTS];

    if(nCount > MAXIMUM_WAIT_OBJECTS)
    {
        SetLastError(RtlNtStatusToDosError(STATUS_INVALID_PARAMETER));
        return (DWORD)-1;
    }

    RtlCopyMemory(rgh, lph, nCount * sizeof(HANDLE));
    if(dwTimeOut == -1)
        pli = NULL;
    else
        {
        pli = &li;
        li.QuadPart = UInt32x32To64(dwTimeOut, 10000);
        li.QuadPart *= -1;
        }
    do
        st = NtWaitForMultipleObjectsEx((CHAR)nCount, rgh,
            fWaitAll ? WaitAll : WaitAny, UserMode, (BOOLEAN)fAlertable, pli);
    while(st == STATUS_ALERTED && fAlertable);
    if(!NT_SUCCESS(st)) {
        SetLastError(RtlNtStatusToDosError(st));
        st = (DWORD)-1;
    }
    return st;
}

DWORD WaitForSingleObject(HANDLE h, DWORD dwTimeOut)
{
    NTSTATUS st;
    LARGE_INTEGER li;
    PLARGE_INTEGER pli;

    if(dwTimeOut == -1)
        pli = NULL;
    else
        {
        pli = &li;
        li.QuadPart = UInt32x32To64(dwTimeOut, 10000);
        li.QuadPart *= -1;
        }
    st = NtWaitForSingleObjectEx(h, UserMode, FALSE, pli);
    if(!NT_SUCCESS(st)) {
        SetLastError(RtlNtStatusToDosError(st));
        st = (DWORD)-1;
    }
    return st;
}

#if 0
BOOL SetWaitableTimer(HANDLE hTimer, const LARGE_INTEGER *pliDue, LONG lPer,
    PTIMERAPCROUTINE pfn, LPVOID lpArg, BOOL fResume)
{
    NTSTATUS st;

    st = NtSetTimer(hTimer, (PLARGE_INTEGER)pliDue, (PTIMER_APC_ROUTINE)pfn,
        lpArg, (BOOLEAN)fResume, lPer, NULL);

    if(!NT_SUCCESS(st))
    {
        SetLastError(RtlNtStatusToDosError(st));
        return FALSE;
    }
    if(st == STATUS_TIMER_RESUME_IGNORED)
        SetLastError(ERROR_NOT_SUPPORTED);
    else
        SetLastError(0);
    return TRUE;
}

HANDLE CreateWaitableTimerW(LPSECURITY_ATTRIBUTES psa, BOOL fManualReset,
    LPCWSTR lpwzName)
{
    NTSTATUS st;
    HANDLE h;
    OBJECT_ATTRIBUTES oa;

    ASSERT(NULL == lpwzName);

    InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);
    st = NtCreateTimer(&h, TIMER_ALL_ACCESS, &oa, fManualReset ?
        NotificationTimer : SynchronizationTimer);
    if(NT_SUCCESS(st))
        SetLastError(0);
    else
        {
        SetLastError(RtlNtStatusToDosError(st));
        h = NULL;
        }
    return h;
}

BOOL CancelWaitableTimer(HANDLE h)
{
    NTSTATUS st = NtCancelTimer(h, NULL);
    if(NT_SUCCESS(st))
        return TRUE;
    SetLastError(RtlNtStatusToDosError(st));
    return FALSE;
}
#endif
