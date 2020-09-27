
#include "precomp.h"
#pragma hdrstop



typedef struct _THREAD_CONTROL {
    HANDLE          hThread;
    HANDLE          hEventStartThread;
    HANDLE          hEventThreadReady;
    THREAD_FUNCTION pUserFunction;
    LPVOID          pUserParam;
    } THREAD_CONTROL, *PTHREAD_CONTROL;


PTHREAD_CONTROL ThreadArray;
PHANDLE         WaitArray;
UINT            nThreads;


DWORD WINAPI ThreadPool( LPVOID lpvParam ) {

    PTHREAD_CONTROL pThreadControl = lpvParam;

    for (;;) {
        SetEvent( pThreadControl->hEventThreadReady );
        WaitForSingleObject( pThreadControl->hEventStartThread, INFINITE );
        pThreadControl->pUserFunction( pThreadControl->pUserParam );
        }

    }


void InitializeThreadPool( UINT nMaxThreads ) {

    DWORD dwTid;
    UINT i;

    ASSERT( nMaxThreads > 0 );

    nThreads    = nMaxThreads;
    ThreadArray = MyAllocNeverFreeLocked( nThreads * sizeof( THREAD_CONTROL ));
    WaitArray   = MyAllocNeverFreeLocked( nThreads * sizeof( HANDLE ));

    for ( i = 0; i < nThreads; i++ ) {
        ThreadArray[ i ].hEventStartThread = AllocateAutoEvent();
        ThreadArray[ i ].hEventThreadReady = AllocateAutoEvent();
        WaitArray[ i ] = ThreadArray[ i ].hEventThreadReady;
        ThreadArray[ i ].hThread = CreateThread( NULL, 0, ThreadPool, &ThreadArray[ i ], 0, &dwTid );
        if ( ThreadArray[ i ].hThread == NULL ) {
            ErrorExit( GETLASTERROR, "CreateThread failed\r\n" );
            }
        }
    }


VOID StartThread( THREAD_FUNCTION pFunction,
                  PVOID           pParam,
                  INT             nPriority ) {

    DWORD dwIndex;

    dwIndex = WaitForMultipleObjects( nThreads, WaitArray, FALSE, INFINITE )
              - WAIT_OBJECT_0;

    if ( dwIndex >= nThreads ) {
        ErrorExit( GETLASTERROR, "WaitForMultipleObjects failed\r\n" );
        }

    ThreadArray[ dwIndex ].pUserFunction = pFunction;
    ThreadArray[ dwIndex ].pUserParam    = pParam;
    SetThreadPriority( ThreadArray[ dwIndex ].hThread, nPriority );
    SetEvent( ThreadArray[ dwIndex ].hEventStartThread );

    }


#ifdef DONTCOMPILE  // let process termination clean up the thread pool

void CleanUpThreadPool( BOOL bWaitForThreads ) {

    UINT i;

    if ( bWaitForThreads ) {
        WaitForMultipleObjects( nThreads, WaitArray, TRUE, INFINITE );
        }

    for ( i = 0; i < nThreads; i++ ) {
        TerminateThread(  ThreadArray[ i ].hThread, 0 );
        CloseHandle(      ThreadArray[ i ].hThread );
        RecycleAutoEvent( ThreadArray[ i ].hEventStartThread );
        RecycleAutoEvent( ThreadArray[ i ].hEventThreadReady );
        }

    // MyHeapFree( ThreadArray );
    // MyHeapFree( WaitArray );

    nThreads = 0;
    }

#endif // DONTCOMPILE

HANDLE MyCreateEvent( BOOL bManualReset, BOOL bInitialState ) {

    //
    //  Create an unnamed event object with no security descriptor.
    //  Exit app if fails.
    //

    HANDLE hEvent = CreateEvent( NULL, bManualReset, bInitialState, NULL );

    if ( hEvent == NULL ) {
        ErrorExit( GETLASTERROR, "CreateEvent failed\r\n" );
        }

    return hEvent;
    }


HANDLE MyCreateSemaphore( LONG InitialCount ) {

    HANDLE hSem = CreateSemaphore( NULL, InitialCount, 0x7FFFFFFF, NULL );

    if ( hSem == NULL ) {
        ErrorExit( GETLASTERROR, "CreateSemaphore failed\r\n" );
        }

    return hSem;
    }
