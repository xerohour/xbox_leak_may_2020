/*
 *
 * api.c
 *
 * Implementation of the Win32 APIs needed for winsock, on top of ntoskrnl
 *
 */

#include "precomp.h"
#include "dm.h"

void SetLastError(DWORD dwErrCode)
{
    DmGetCurrentDmtd()->LastError = dwErrCode;
}

DWORD GetLastError(void)
{
    return DmGetCurrentDmtd()->LastError;
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

HANDLE CreateEventA(LPSECURITY_ATTRIBUTES psa, BOOL fManualReset,
    BOOL fInitState, LPCSTR lpwzName)
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

void _ThreadStartup(PKSTART_ROUTINE StartRoutine, PVOID StartContext)
{
    DmGetCurrentDmtd()->DebugFlags |= DMFLAG_DEBUGTHREAD;
    StartRoutine(StartContext);
    PsTerminateSystemThread(STATUS_SUCCESS);
}

HANDLE CreateThread(LPSECURITY_ATTRIBUTES psa, DWORD dwStack,
    LPTHREAD_START_ROUTINE lpfn, LPVOID lpv, DWORD dwFlags, LPDWORD lpdwId)
{
    NTSTATUS st;
    HANDLE h;
    CLIENT_ID clid;

    st = PsCreateSystemThreadEx(&h, 0, dwStack, (PHANDLE)lpdwId,
		(PKSTART_ROUTINE)lpfn, lpv,
		(BOOLEAN)(dwFlags & CREATE_SUSPENDED ? TRUE : FALSE), _ThreadStartup);
    
    if(!NT_SUCCESS(st))
        {
        SetLastError(RtlNtStatusToDosError(st));
        return NULL;
        }
    
    return h;
}

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

#if 0
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
#endif

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
