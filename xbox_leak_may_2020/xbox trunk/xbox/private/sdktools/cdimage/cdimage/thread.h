
typedef VOID ( *THREAD_FUNCTION )( PVOID );

void InitializeThreadPool( UINT nMaxThreads );

VOID StartThread( THREAD_FUNCTION pFunction,
                  PVOID           pParam,
                  INT             nPriority );

void CleanUpThreadPool( BOOL bWaitForThreads );

HANDLE MyCreateEvent( BOOL bManualReset, BOOL bInitialState );

HANDLE MyCreateSemaphore( LONG InitialCount );
