/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    CWorkerThread.cpp

Abstract:

    Base class for handling worker threads

Author:

    Josh Poley (jpoley)

*****************************************************************************/
#include "..\inc\CWorkerThread.h"


/*****************************************************************************

Routine Description:

    LocalThreadFunct

Arguments:

    LPVOID lpParameter  - a valid CWorkerThread class

Return Value:

    0 on error, ~0 on execption, or the thread's exit value

Notes:


*****************************************************************************/
static DWORD WINAPI LocalThreadFunct(LPVOID lpParameter)
    {
    DWORD returnCode = 0;
    CWorkerThread *localPointer = (CWorkerThread*)lpParameter;

    if(!localPointer)
        {
        // a valid CWorkerThread class must be supplied via the lpParameter!
        //_ASSERT(localPointer);
        return 0;
        }

    // turn off the exit flag and set notification values
    localPointer->Lock();
    localPointer->bExit = FALSE;
    localPointer->Unlock();

    // set the thread id
    localPointer->SetThreadID(GetCurrentThreadId());

    // run the main thread function (this is the custom function)
    returnCode = localPointer->ThreadFunct();

    localPointer->SetThreadHandle((DWORD)~0x0L);

    // store the return code from the main thread function
    // invalidate the thread handle & id
    localPointer->Lock();
#ifndef _XBOX // dont overwrite the handle if XBOX build
    localPointer->dwThreadHandle = (DWORD)~0x0L;
#endif
    localPointer->dwThreadID = (DWORD)~0x0L;
    localPointer->dwThreadExitCode = returnCode;
    localPointer->Unlock();

    // exit thread
    return returnCode;
    }

/*****************************************************************************

Routine Description:

    CWorkerThread::Run
    start up a worker thread

Arguments:

    none

Return Value:

    none

Notes:

*****************************************************************************/
void CWorkerThread::Run(void)
    {
#ifdef _XBOX

    DWORD handle = (DWORD)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LocalThreadFunct, this, 0, 0);
    SetThreadHandle(handle);

#else

    DWORD handle = _beginthread((void (__cdecl *)(void *))LocalThreadFunct, 0, this);
    SetThreadHandle(handle);

#endif
    }

/*****************************************************************************

Routine Description:

    CWorkerThread::CWorkerThread
    worker thread data constructor

Arguments:

    none

Return Value:

    none

Notes:

*****************************************************************************/
CWorkerThread::CWorkerThread()
    {
    InitializeCriticalSection(&criter);
    SetThreadHandle((DWORD)~0x0L);
    SetThreadID((DWORD)~0x0L);

    Lock();

    dwLocalThreadID = 0;

    bExit = FALSE;
    dwThreadExitCode = 0;

    Unlock();
    }

/*****************************************************************************

Routine Description:

    CWorkerThread::CWorkerThread
    worker thread data constructor

Arguments:

    DWORD localID

Return Value:

    none

Notes:

*****************************************************************************/
CWorkerThread::CWorkerThread(DWORD localID)
    {
    InitializeCriticalSection(&criter);
    SetThreadHandle((DWORD)~0x0L);
    SetThreadID((DWORD)~0x0L);

    Lock();

    dwLocalThreadID = localID;

    bExit = FALSE;
    dwThreadExitCode = 0;

    Unlock();
    }

/*****************************************************************************

Routine Description:

    CWorkerThread::~CWorkerThread
    worker thread data destructor

Arguments:

    none

Return Value:

    none

Notes:

    If the thread does not exit, this function will be caught in an infinate 
        loop.

*****************************************************************************/
CWorkerThread::~CWorkerThread()
    {
    if(GetThreadHandle() != (DWORD)~0x0L)
        {
        DWORD timeout = 1000; // start with 1 sec

        // if your code infinite loops here, you have a blocking problem
        while(!SoftBreak(timeout))
            {
            timeout *= 2;
            if(timeout == 64000) OutputDebugStringA("CWorkerThread::~CWorkerThread(): Possible deadlock\n");
            }
        }

#ifdef _XBOX
    DWORD h = GetThreadHandle();
    if(h != (DWORD)~0x0L)
        {
        CloseHandle((HANDLE)h);
        SetThreadHandle((DWORD)~0x0L);
        }
#endif

    DeleteCriticalSection(&criter);
    }

/*****************************************************************************

Routine Description:

    CWorkerThread::SoftBreak

Arguments:

    DWORD dwTimeOut - timeout value in ms

Return Value:

    TRUE - the thread exited, FALSE - timedout before the thread exited

Notes:

*****************************************************************************/
BOOL CWorkerThread::SoftBreak(DWORD dwTimeOut)
    {
    Lock();
    bExit = TRUE;
    Unlock();

    return WaitForExit(dwTimeOut);
    }

/*****************************************************************************

Routine Description:

    CWorkerThread::HardBreak

Arguments:

    none

Return Value:

    TRUE - operation succedded, FALSE - the operation failed.

Notes:

    This function does not perform any locking or unlocking.

    WARNING: Do not use this function execpt in DIRE situations. This WILL 
        cause leaks, and perhaps blocking or crashes if critical sections 
        are not released!

*****************************************************************************/
BOOL CWorkerThread::HardBreak(void)
    {
    DWORD dwHandle = dwThreadHandle;
    bExit = TRUE;
    dwThreadHandle = (DWORD)~0x0L;
    dwThreadExitCode = (DWORD)~0x0L;
    dwThreadID = (DWORD)~0x0L;

    if(dwHandle != (DWORD)~0x0L)
        {
#ifdef _XBOX
        OutputDebugStringA("CWorkerThead::HardBreak() called!\n");
        return FALSE;
#else
        OutputDebugStringA("CWorkerThead::HardBreak() called!\n");
        return TerminateThread((void*)dwHandle, (DWORD)~0x0L);
#endif
        }

    return FALSE;
    }

/*****************************************************************************

Routine Description:

    CWorkerThread::WaitForExit

Arguments:

    DWORD dwTimeOut - timeout value in ms

Return Value:

    TRUE - the thread exited, FALSE - timeout was reached

Notes:

*****************************************************************************/
BOOL CWorkerThread::WaitForExit(DWORD dwTimeOut)
    {
    DWORD dwStartTime = GetTickCount();
    DWORD dwCurTime;

    while(GetThreadID() != (DWORD)~0x0L)
        {
        Sleep(20);
        dwCurTime = GetTickCount();
        if(dwCurTime - dwStartTime >= dwTimeOut)
            {
            return FALSE;
            }
        }

#ifdef _XBOX
    DWORD h = GetThreadHandle();
    if(h != (DWORD)~0x0L)
        {
        CloseHandle((HANDLE)h);
        SetThreadHandle((DWORD)~0x0L);
        }
#endif

    return TRUE;
    }


/*****************************************************************************

Routine Description:

    CWorkerThread::Lock

Arguments:

    DWORD dwTimeOut - timeout value in ms
   
Return Value:

    TRUE - critical section entered, FALSE - timeout

Notes:

*****************************************************************************/
#ifdef _WIN32_WINNT // TryEnterCriticalSection only available on NT
#include "winbase.h"
BOOL CWorkerThread::Lock(DWORD dwTimeOut)
    {
    DWORD dwStartTime = GetTickCount();
    DWORD dwCurTime;

    while(!TryEnterCriticalSection(&criter))
        {
        Sleep(20);
        dwCurTime = GetTickCount();
        if(dwCurTime - dwStartTime >= dwTimeOut)
            {
            return FALSE;
            }
        }

    return TRUE;
    }
#endif


/*****************************************************************************

Routine Description:

    CWorkerThread::SuspendThread

Arguments:

    none

Return Value:

    DWORD - the thread's suspend count, 0xFFFFFFFF on error.

Notes:

    This function just calls the WIN API SuspendThread

*****************************************************************************/
DWORD CWorkerThread::SuspendThread(void)
    {
    DWORD dwHandle = GetThreadHandle();

    if(dwHandle != (DWORD)~0x0L)
        {
        return ::SuspendThread((void*)dwHandle);
        }

    return 0xFFFFFFFF;
    }

/*****************************************************************************

Routine Description:

    CWorkerThread::ResumeThread

Arguments:

    none

Return Value:

    DWORD - the thread's suspend count, 0xFFFFFFFF on error.

Notes:

    This function just calls the WIN API ResumeThread

*****************************************************************************/
DWORD CWorkerThread::ResumeThread(void)
    {
    DWORD dwHandle = GetThreadHandle();

    if(dwHandle != (DWORD)~0x0L)
        {
        return ::ResumeThread((void*)dwHandle);
        }

    return 0xFFFFFFFF;
    }
