#include <xtl.h>

// Hack! DbgPrint is much nicer than OutputDebugString so I exported it from xapi.lib
// (as DebugPrint) but the prototype isn't in the public headers...
ULONG
DebugPrint(
    PCHAR Format,
    ...
    );

BOOL
WINAPI
DllEntryPoint(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved
    )
{
    DebugPrint("testdll: DLLEntryPoint reason=%d\n", fdwReason);

    return TRUE;
}


#define MAX_BLOCKS 10
#define MAX_THREADS 10


DWORD
WINAPI
TestThreadStart(
    LPVOID lp
    );

VOID
WINAPI
IRandSeed(
    ULONG ulSeed
    );

ULONG
WINAPI
IRand( VOID );

VOID
CallMe( VOID )
{
    DWORD thid;
    int i;
    LPVOID pMem[MAX_BLOCKS];
    HLOCAL hStatus;
    LPVOID lpT;
    int nSize;
    HANDLE hThreads[MAX_THREADS];
    DWORD dwWaitStatus;
    DWORD dwExitCode;
    DWORD dwThreadId;


    OutputDebugString(L"TestDLL!CallMe entered\n");

    // Add the date and time in here, this would be way better than GetTickCount because
    // GetTickCount restarts at boot, so we probably get the same seed almost every time!
    IRandSeed(GetTickCount());

#if 0
    TestDirectDraw();
#endif

    // Allocate some memory blocks in varying sizes
    for (i = 0 ; i < MAX_BLOCKS ; i++)
    {
        nSize = IRand() % 5000;
        pMem[i] = (LPVOID)LocalAlloc(LPTR, nSize);
        DebugPrint("#%d: Allocated %d bytes as %08x\n", i, nSize, pMem[i]);
    }

    // Reallocate some memory blocks to random sizes
    for (i = 0 ; i < MAX_BLOCKS ; i++)
    {
        nSize = IRand() % 5000;
        lpT = pMem[i];
        pMem[i] = (LPVOID)LocalReAlloc((HLOCAL)lpT, nSize, LMEM_MOVEABLE);
        DebugPrint("#%d: ReAllocated pointer %08x as %08x as %d bytes\n", i, lpT, pMem[i], nSize);
    }
    
    // Free the blocks
    for (i = 0 ; i < MAX_BLOCKS ; i++)
    {
        hStatus = LocalFree((HLOCAL)pMem[i]);
        
        DebugPrint("#%d: %08x freed %s\n", i, pMem[i],
            hStatus == NULL ? "successfully" : "unsucessfully");
    }

    // Create some threads that will occasionally fault
    for (i = 0 ; i < MAX_THREADS ; i++)
    {
        hThreads[i] = CreateThread(
                                    NULL,
                                    0,
                                    (LPTHREAD_START_ROUTINE)TestThreadStart,
                                    (LPVOID)i,
                                    0,
                                    &dwThreadId
                                   );
    }

    // Wait until all the threads exit
    dwWaitStatus = WaitForMultipleObjects(
                                            MAX_THREADS,
                                            hThreads,
                                            TRUE,
                                            60 * 1000
                                         );
    if (dwWaitStatus == WAIT_TIMEOUT)
        DebugPrint("Timed out waiting for threads to exit (one minute)\n");
    else
        DebugPrint("All threads exited\n");

    // Now display all the exit codes
    for (i = 0 ; i < MAX_THREADS ; i++)
    {
        GetExitCodeThread(hThreads[i], &dwExitCode);
        DebugPrint("Thread #%d, exit code was %d\n", i, dwExitCode);
        CloseHandle(hThreads[i]);
    }
    
    OutputDebugString(L"TestDLL!CallMe exiting\n");
}

// Thread startup routine

DWORD
WINAPI
MyFilter(
    DWORD dwException
    )
{
    DebugPrint("Exception filter caught exception %08x\n", dwException);
    return EXCEPTION_EXECUTE_HANDLER;
//    return EXCEPTION_CONTINUE_SEARCH;
//    return EXCEPTION_CONTINUE_EXECUTION;
}
         

DWORD
WINAPI
TestThreadStart(
    LPVOID lp
    )
{
    int nThread = (int)lp;
    int nExitCode = IRand() % 100;
    int nSleepTime = IRand() % 10000;

    try
    {
        DebugPrint("Thread #%d sleeping %d milliseconds\n", nThread, nSleepTime);

        Sleep(nSleepTime);
        
        if (IRand() % 10 == 7) // lucky 7
        {
            DebugPrint("Thread #%d is going to fault\n", nThread);
            
//            RaiseException(0, /* EXCEPTION_NONCONTINUABLE */ 0, 0, NULL);

//            DebugPrint("We continued after the exception! Woo hoo!\n");

            _asm mov ecx, 0x30000000    // page fault
            _asm mov [ecx], eax
        }
    }
    except(MyFilter(GetExceptionCode()))
    {
        DebugPrint("We faulted in thread #%d\n", nThread);
        ExitThread(0);
    }

    DebugPrint("Thread #%d exiting with exit code of %d\n", nThread, nExitCode);
    return nExitCode;
}


// Quick and dirty random number generator

    ULONG g_ulSeed;

VOID
WINAPI
IRandSeed(
    ULONG ulSeed
    )
{
    g_ulSeed = ulSeed;
}

ULONG
WINAPI
IRand( VOID )
{
    g_ulSeed =  1664525 * g_ulSeed + 1013904223;

    return g_ulSeed;
}


