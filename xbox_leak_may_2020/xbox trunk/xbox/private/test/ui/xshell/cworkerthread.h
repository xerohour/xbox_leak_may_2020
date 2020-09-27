/*****************************************************************************

Copyright (C) 1999-2001 Microsoft Corporation

Module Name:

    CWorkerThread.h

Abstract:

    Base class for handling worker threads

Author:

    Josh Poley (jpoley)

Notes:

    If you get the following error:
        error C2065: '_beginthread' : undeclared identifier
    Then you are building your project in single threaded mode, switch it 
    to multi-threaded.

*****************************************************************************/

#ifndef _CWORKERTHRAD_H_
#define _CWORKERTHRAD_H_

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

// #include <winsock2.h>
// #include <crtdbg.h>
#include <process.h>    //_beginthread, _endthread

static DWORD WINAPI LocalThreadFunct(LPVOID lpParameter);

/*****************************************************************************

Class Description:

    Worker thread class

Methods:



Data:



NOTE: 
    
    Remember to nest any member read/write with Lock and Unlock 

    In the implementation of the ThreadFunct function, the user must check 
        the bExit flag and exit gracefully if set.

*****************************************************************************/
class __declspec(dllexport) CWorkerThread
    {
    private:
        CRITICAL_SECTION criter;
        DWORD dwThreadHandle;
        DWORD dwThreadID;
        DWORD dwThreadExitCode;

    protected:
        BOOL bExit;

        DWORD dwLocalThreadID;

    // constructors / destructors
    public:
        CWorkerThread();
        CWorkerThread(DWORD localID);
        virtual ~CWorkerThread();

    // public access and information
    public:
        void Lock(void) { EnterCriticalSection(&criter); }
        void Unlock(void) { LeaveCriticalSection(&criter); }

        #ifdef _WIN32_WINNT
        BOOL Lock(DWORD dwTimeOut);
        #endif

        DWORD GetThreadHandle(void) { Lock(); DWORD h = dwThreadHandle; Unlock(); return h; }
        DWORD GetThreadID(void) { Lock(); DWORD id = dwThreadID; Unlock(); return id; }
        DWORD GetLocalThreadID(void) { Lock(); DWORD id = dwLocalThreadID; Unlock(); return id; }
        void  SetLocalThreadID(DWORD id) { Lock(); dwLocalThreadID = id; Unlock(); }
        DWORD GetThreadExitCode(void) { Lock(); DWORD e = dwThreadExitCode; Unlock(); return e; }
        BOOL  GetExitFlag(void) { Lock(); BOOL e = bExit; Unlock(); return e; }

        void Run(void);
        virtual BOOL SoftBreak(DWORD dwTimeOut);

        BOOL WaitForExit(DWORD dwTimeOut);

        // thread manipulation
        DWORD SuspendThread(void);
        DWORD ResumeThread(void);

    // internal functions
    private:
        void SetThreadHandle(DWORD h) { Lock(); dwThreadHandle = h; Unlock(); }
        void SetThreadID(DWORD id) { Lock(); dwThreadID = id; Unlock(); }
        void SetThreadExitCode(DWORD c) { Lock(); dwThreadExitCode = c; Unlock(); }

        friend DWORD WINAPI LocalThreadFunct(LPVOID lpParameter);

    // overrideable functions
    public:
        /*****************************************************************************
        NOTE: in the implementation of this function, the user must check 
               the bExit flag and exit gracefully if set:
        while(1)
            {
            if(GetExitFlag()) break;
            }
        *****************************************************************************/
        virtual DWORD ThreadFunct(void) { return 0; }
    };

#endif // _CWORKERTHRAD_H_
