/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		ThreadPool.cpp
 *  Content:	main job thread pool
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	11/25/98	jtk		Created
 ***************************************************************************/

#include "wsockspi.h"

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_WSOCK

//**********************************************************************
// Constant definitions
//**********************************************************************

//
// events for threads
//
enum
{
	EVENT_INDEX_STOP_ALL_THREADS = 0,
	EVENT_INDEX_PENDING_JOB = 1,
	EVENT_INDEX_WAKE_NT_TIMER_THREAD = 1,
	EVENT_INDEX_WINSOCK_2_SEND_COMPLETE = 2,
	EVENT_INDEX_WINSOCK_2_RECEIVE_COMPLETE = 3,

	EVENT_INDEX_MAX
};

//
// times to wait in milliseconds when polling for work thread shutdown
//
#define	WORK_THREAD_CLOSE_SLEEP_TIME	100

//
// select polling period for writes (milliseconds)
//
static const DWORD	g_dwSelectTimeSlice = 2;

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

typedef struct	_TIMER_OPERATION_ENTRY
{
	CBilink		Linkage;			// list links
	void		*pContext;			// user context passed back in timer events

	//
	// timer information
	//
	UINT_PTR	uRetryCount;		// number of times to retry this event
	BOOL		fRetryForever;		// Boolean for retrying forever
	DN_TIME		RetryInterval;		// time between enums (milliseconds)
	DN_TIME		IdleTimeout;		// time at which the command sits idle after all retrys are complete
	BOOL		fIdleWaitForever;	// Boolean for waiting forever in idle state
	DN_TIME		NextRetryTime;		// time at which this event will fire next (milliseconds)

	TIMER_EVENT_CALLBACK	*pTimerCallback;	// callback for when this event fires
	TIMER_EVENT_COMPLETE	*pTimerComplete;	// callback for when this event is complete

	#undef DPF_MODNAME
	#define	DPF_MODNAME	"_TIMER_OPERATION_ENTRY::TimerOperationFromLinkage"
	static TIMER_OPERATION_ENTRY	*TimerOperationFromLinkage( CBilink *const pLinkage )
	{
		DNASSERT( pLinkage != NULL );
		DBG_CASSERT( OFFSETOF( _TIMER_OPERATION_ENTRY, Linkage ) == 0 );
		return	reinterpret_cast<_TIMER_OPERATION_ENTRY*>( pLinkage );
	}

} TIMER_OPERATION_ENTRY;

//
// structure for common data in Win9x thread
//
typedef	struct	_WIN9X_CORE_DATA
{
	DN_TIME		NextTimerJobTime;					// time when the next timer job needs service
	HANDLE		hWaitHandles[ EVENT_INDEX_MAX ];	// handles for waiting on
	DWORD		dwTimeToNextJob;					// time to next job
	BOOL		fTimerJobsActive;					// Boolean indicating that there are active jobs
	BOOL		fLooping;							// Boolean indicating that this thread is still running

} WIN9X_CORE_DATA;

//
// information passed to the Win9x workhorse thread
//
typedef struct	_WIN9X_THREAD_DATA
{
	CThreadPool		*pThisThreadPool;	// pointer to this object
} WIN9X_THREAD_DATA;


//**********************************************************************
// Variable definitions
//**********************************************************************

extern CRITICAL_SECTION g_csSocketAccessLock;
extern CThreadPool *g_pThreadPool2;

//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::CThreadPool - constructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::CThreadPool"

CThreadPool::CThreadPool():
		m_iRefCount( 0 ),
		m_hStopAllThreads( NULL ),
		m_hWinsock2SendComplete( NULL ),
		m_hWinsock2ReceiveComplete( NULL ),
                m_hThreadCloseEvent( NULL ),
		m_uReservedSocketCount( 0 )
{
	memset( &m_SocketSet, 0x00, sizeof( m_SocketSet ) );
	m_OutstandingReadList.Initialize();
	m_OutstandingWriteList.Initialize();
	m_TimerJobList.Initialize();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::~CThreadPool - destructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::~CThreadPool"

CThreadPool::~CThreadPool()
{
	DNASSERT( m_iRefCount == 0 );
	DNASSERT( m_hStopAllThreads == NULL );
	DNASSERT( m_hWinsock2SendComplete == NULL );
	DNASSERT( m_hWinsock2ReceiveComplete == NULL );

	DNASSERT( m_uReservedSocketCount == 0 );
	DNASSERT( m_OutstandingReadList.IsEmpty() != FALSE );
	DNASSERT( m_OutstandingReadList.IsEmpty() != FALSE );
	DNASSERT( m_TimerJobList.IsEmpty() != FALSE );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::Initialize - initialize work threads
//
// Entry:		Nothing
//
// Exit:		Error Code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::Initialize"

HRESULT	CThreadPool::Initialize( void )
{
	HRESULT			hr;


	//
	// initialize
	//
	hr = DPN_OK;

	//
	// initialize critical sections
	//
	if ( DNInitializeCriticalSection( &m_Lock ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}
	DebugSetCriticalSectionRecursionCount( &m_Lock, 0 );


	if ( DNInitializeCriticalSection( &m_ReadDataLock ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	//
	// Win9x has poor APC support and as part of the workaround, the read data
	// lock needs to be taken twice.  Adjust the recursion counts accordingly.
	//
	DEBUG_ONLY( 
						DebugSetCriticalSectionRecursionCount( &m_ReadDataLock, 1 );
	)


	if ( DNInitializeCriticalSection( &m_WriteDataLock ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	//
	// Win9x has poor APC support and as part of the workaround, the write data
	// lock needs to be taken twice.  Adjust the recursion counts accordingly.
	//
	DEBUG_ONLY(
						DebugSetCriticalSectionRecursionCount( &m_WriteDataLock, 1 );
	)


	if ( DNInitializeCriticalSection( &m_JobDataLock ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}
	DebugSetCriticalSectionRecursionCount( &m_JobDataLock, 0 );

	if ( DNInitializeCriticalSection( &m_TimerDataLock ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}
	DebugSetCriticalSectionRecursionCount( &m_TimerDataLock, 1 );

	//
	// initialize job queue
	//
	if ( m_JobQueue.Initialize() == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	//
	// initialize pools
	//

	// pool of IP read requests
	m_IPReadIODataPool.Initialize( CReadIOData::ReadIOData_Alloc,
								   CReadIOData::ReadIOData_Get,
								   CReadIOData::ReadIOData_Release,
								   CReadIOData::ReadIOData_Dealloc
								   );

	// pool of write requests
	m_WriteIODataPool.Initialize( CWriteIOData::WriteIOData_Alloc,
								  CWriteIOData::WriteIOData_Get,
								  CWriteIOData::WriteIOData_Release,
								  CWriteIOData::WriteIOData_Dealloc
								  );

	// job pool
	if ( FPM_Initialize( &m_JobPool,					// pointer to pool
						 sizeof( THREAD_POOL_JOB ),		// size of pool entry
						 WorkThreadJob_Alloc,			// function called on pool entry initial allocation
						 WorkThreadJob_Get,				// function called on entry extraction from pool
						 WorkThreadJob_Release,			// function called on entry return to pool
						 WorkThreadJob_Dealloc			// function called on entry free
						 ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	// enum entry pool
	if ( FPM_Initialize( &m_TimerEntryPool,					// pointer to pool
						 sizeof( TIMER_OPERATION_ENTRY ),	// size of pool entry
						 TimerEntry_Alloc,					// function called on pool entry initial allocation
						 TimerEntry_Get,					// function called on entry extraction from pool
						 TimerEntry_Release,				// function called on entry return to pool
						 TimerEntry_Dealloc					// function called on entry free
						 ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	//
	// Create event to stop all threads.  Win9x needs this to stop processing
	// and the NT enum thread uses this to stop processing
	//
	DNASSERT( m_hStopAllThreads == NULL );
	m_hStopAllThreads = CreateEvent( NULL,		// pointer to security (none)
									 TRUE,		// manual reset
									 FALSE,		// start unsignalled
									 NULL );	// pointer to name (none)
	if ( m_hStopAllThreads == NULL )
	{
		DWORD	dwError;


		dwError = GetLastError();
		DPF( 0, "Failed to create event to stop all threads!" );
		DisplayErrorCode( 0, dwError );
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

			hr = Win9xInit();
			if ( hr != DPN_OK )
			{
				goto Failure;
			}

Exit:
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem with CreateWorkThreads" );
		DisplayDNError( 0, hr );
	}

	return	hr;

Failure:
	StopAllThreads();
	Deinitialize();

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::Win9xInit - initialize Win9x components
//
// Entry:		Nothing
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::Win9xInit"

HRESULT	CThreadPool::Win9xInit( void )
{
	HRESULT				hr;
#ifndef DPLAY_DOWORK
	HANDLE				hPrimaryThread;
	DWORD				dwPrimaryThreadID;
	WIN9X_THREAD_DATA	*pPrimaryThreadInput;
	hPrimaryThread = NULL;
	pPrimaryThreadInput = NULL;
#endif

	//
	// initialize
	//
	hr = DPN_OK;

	//
	// Win9x requires completion events for Winsock2.  Always allocate the
	// events even though the they might not be used.
	//
	DNASSERT( m_hWinsock2SendComplete == NULL );
	m_hWinsock2SendComplete = CreateEvent( NULL,	// pointer to security (none)
										   TRUE,	// manual reset
										   FALSE,	// start unsignalled
										   NULL		// pointer to name (none)
										   );
	if ( m_hWinsock2SendComplete == NULL )
	{
		DWORD	dwError;


		dwError = GetLastError();
		DPF( 0, "Failed to create event for Winsock2Send!" );
		DisplayErrorCode( 0, dwError );
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	DNASSERT( m_hWinsock2ReceiveComplete == NULL );
	m_hWinsock2ReceiveComplete = CreateEvent( NULL,		// pointer to security (none)
											  TRUE,		// manual reset
											  FALSE,	// start unsignalled
											  NULL		// pointer to name (none)
											  );
	if ( m_hWinsock2ReceiveComplete == NULL )
	{
		DWORD	dwError;


		dwError = GetLastError();
		DPF( 0, "Failed to create event for Winsock2Receive!" );
		DisplayErrorCode( 0, dwError );
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	m_hThreadCloseEvent = CreateEvent( NULL,		// pointer to security (none)
											  TRUE,		// manual reset
											  FALSE,	// start unsignalled
											  NULL		// pointer to name (none)
											  );
	if ( m_hThreadCloseEvent == NULL )
	{
		DWORD	dwError;


		dwError = GetLastError();
		DPF( 0, "Failed to create event for ThreadClose!" );
		DisplayErrorCode( 0, dwError );
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	ResetEvent(m_hThreadCloseEvent);


#ifndef DPLAY_DOWORK
	//
	// create parameters to worker threads
	//
	pPrimaryThreadInput = static_cast<WIN9X_THREAD_DATA*>( DNMalloc( sizeof( *pPrimaryThreadInput ) ) );
	if ( pPrimaryThreadInput == NULL )
	{
		DPF( 0, "Problem allocating memory for primary Win9x thread!" );
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	memset( pPrimaryThreadInput, 0x00, sizeof( *pPrimaryThreadInput ) );
	pPrimaryThreadInput->pThisThreadPool = this;
	
	//
	// Create one worker thread and boost its priority.  If the primary thread
	// can be created and boosted, create a secondary thread.  Do not create a
	// secondary thread if the primary could not be boosted because the system
	// is probably low on resources.
	//
	hPrimaryThread = CreateThread( NULL,					// pointer to security attributes (none)
								   0,						// stack size (default)
								   PrimaryWin9xThread,		// pointer to thread function
								   pPrimaryThreadInput,		// pointer to input parameter
								   0,						// let it run
								   &dwPrimaryThreadID		// pointer to destination of thread ID
								   );
	if ( hPrimaryThread == NULL )
	{
		DWORD	dwError;


		dwError = GetLastError();
		DPF( 0, "Problem creating Win9x thread!" );
		DisplayErrorCode( 0, dwError );
		hr = DPNERR_OUTOFMEMORY;

		goto Failure;
	}
	pPrimaryThreadInput = NULL;

	DPF( 8, "Created primary Win9x thread: 0x%x\tTotal Thread Count: 1", dwPrimaryThreadID);
	DNASSERT( hPrimaryThread != NULL );
	if ( SetThreadPriority( hPrimaryThread, THREAD_PRIORITY_ABOVE_NORMAL ) == FALSE )
	{
		DWORD	dwError;


		dwError = GetLastError();
		DPF( 0, "Failed to boost priority of primary Win9x read thread!  Not starting secondary thread" );
		DisplayErrorCode( 0, dwError );
	}
#endif

Exit:
#ifndef DPLAY_DOWORK
	if ( pPrimaryThreadInput != NULL )
	{
		DNFree( pPrimaryThreadInput );
		pPrimaryThreadInput = NULL;
	}

	if ( hPrimaryThread != NULL )
	{
		if ( CloseHandle( hPrimaryThread ) == FALSE )
		{
			DWORD	dwError;


			dwError = GetLastError();
			DPF( 0, "Problem closing Win9x thread hanle!" );
			DisplayErrorCode( 0, dwError );
		}

		hPrimaryThread = NULL;
	}
#endif

	return	hr;

Failure:
	SetEvent(m_hThreadCloseEvent);

	DPF( 0, "Failed Win9x Initialization!" );
	DisplayDNError( 0, hr );

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::StopAllThreads - stop all work threads
//
// Entry:		Nothing
//
// Exit:		Nothing
//
// Note:		This function blocks until all threads complete!
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::StopAllThreads"

void	CThreadPool::StopAllThreads( void )
{
	//
	// stop all non-I/O completion threads
	//
	if ( m_hStopAllThreads != NULL )
	{
		if ( SetEvent( m_hStopAllThreads ) == FALSE )
		{
			DWORD	dwError;


			dwError = GetLastError();
			DPF( 0, "Failed to set event to stop all threads!" );
			DisplayErrorCode( 0, dwError );
		}
	}
#ifdef DPLAY_DOWORK
	//
	// check for outstanding threads (no need to lock thread pool count)
	//
	while (1)
	{
		switch (WaitForSingleObject(m_hThreadCloseEvent,0))
		{
			case WAIT_TIMEOUT:
			{
            	if (g_pThreadPool2 != NULL)
	            {
		            g_pThreadPool2->PrimaryWin9xThread();
	            }
				break;
			}

            case WAIT_OBJECT_0:
			{
				return;
			}
		}
	}
#else
	WaitForSingleObject(m_hThreadCloseEvent,INFINITE);
#endif
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::CancelOutstandingJobs - cancel outstanding jobs
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::CancelOutstandingJobs"

void	CThreadPool::CancelOutstandingJobs( void )
{
	m_JobQueue.Lock();

	while ( m_JobQueue.IsEmpty() == FALSE )
	{
		THREAD_POOL_JOB	*pJob;


		pJob = m_JobQueue.DequeueJob();
		DNASSERT( pJob != NULL );
		DNASSERT( pJob->pCancelFunction != NULL );
		pJob->pCancelFunction( pJob );
		pJob->JobType = JOB_UNINITIALIZED;
		m_JobPool.Release( &m_JobPool, pJob );
	};

	m_JobQueue.Unlock();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::CancelOutstandingIO - cancel outstanding IO
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::CancelOutstandingIO"

void	CThreadPool::CancelOutstandingIO( void )
{
	CBilink	*pTemp;

	//
	// stop any receives with the notification that Winsock is shutting down.
	//
	pTemp = m_OutstandingReadList.GetNext();
	while ( pTemp != &m_OutstandingReadList )
	{
		CReadIOData	*pReadData;


		pReadData = CReadIOData::ReadDataFromBilink( pTemp );
		pTemp = pTemp->GetNext();
		pReadData->m_OutstandingReadListLinkage.RemoveFromList();
		
		DEBUG_ONLY( if ( pReadData->m_fRetainedByHigherLayer != FALSE )
					{
						INT3;
					}
				  );		
		DNASSERT( pReadData->Win9xOperationPending() != FALSE );
		pReadData->SetWin9xOperationPending( FALSE );
		pReadData->m_Win9xReceiveWSAReturn = WSAESHUTDOWN;
		pReadData->m_dwOverlappedBytesReceived = 0;
		pReadData->SocketPort()->CancelReceive( pReadData );
	}

	//
	// stop any pending writes with the notification that the user cancelled it.
	//
	pTemp = m_OutstandingWriteList.GetNext();
	while ( pTemp != &m_OutstandingWriteList )
	{
		CWriteIOData	*pWriteData;
		CSocketPort		*pSocketPort;


		pWriteData = CWriteIOData::WriteDataFromBilink( pTemp );
		pTemp = pTemp->GetNext();
		pWriteData->m_OutstandingWriteListLinkage.RemoveFromList();

		DNASSERT( pWriteData->Win9xOperationPending() != FALSE );
		pWriteData->SetWin9xOperationPending( FALSE );
		pSocketPort = pWriteData->SocketPort();
		pSocketPort->SendComplete( pWriteData, DPNERR_USERCANCEL );
		pSocketPort->DecRef();
	}

	while ( m_JobQueue.IsEmpty() == FALSE )
	{
		THREAD_POOL_JOB	*pJob;


		pJob = m_JobQueue.DequeueJob();
		DNASSERT( pJob != NULL );
		DNASSERT( pJob->pCancelFunction != NULL );
		pJob->pCancelFunction( pJob );
		pJob->JobType = JOB_UNINITIALIZED;
		m_JobPool.Release( &m_JobPool, pJob );
	};
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::ReturnSelfToPool - return this object to the pool
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::ReturnSelfToPool"

void	CThreadPool::ReturnSelfToPool( void )
{
	Deinitialize();
	ReturnThreadPool( this );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::Deinitialize - destroy work threads
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::Deinitialize"

void	CThreadPool::Deinitialize( void )
{
	g_pThreadPool2 = NULL;

	DNASSERT( m_JobQueue.IsEmpty() != FALSE );

	//
	// close StopAllThreads handle
	//
	if ( m_hStopAllThreads != NULL )
	{
		if ( CloseHandle( m_hStopAllThreads ) == FALSE )
		{
			DWORD	dwError;


			dwError = GetLastError();
			DPF( 0, "Failed to close StopAllThreads handle!" );
			DisplayErrorCode( 0, dwError );
		}

		m_hStopAllThreads = NULL;
	}

	//
	// close handles for I/O events
	//
	if ( m_hWinsock2SendComplete != NULL )
	{
		if ( CloseHandle( m_hWinsock2SendComplete ) == FALSE )
		{
			DWORD	dwError;


			dwError = GetLastError();
			DPF( 0, "Problem closing Winsock2SendComplete handle!" );
			DisplayErrorCode( 0, dwError );
		}

		m_hWinsock2SendComplete = NULL;
	}

	if ( m_hWinsock2ReceiveComplete != NULL )
	{
		if ( CloseHandle( m_hWinsock2ReceiveComplete ) == FALSE )
		{
			DWORD	dwError;


			dwError = GetLastError();
			DPF( 0, "Problem closing Winsock2ReceiveComplete handle!" );
			DisplayErrorCode( 0, dwError );
		}

		m_hWinsock2ReceiveComplete = NULL;
	}

//	//
//	// Now that all of the threads are stopped, clean up any outstanding I/O.
//	// this can be done without taking any locks
//	//
//	CancelOutstandingIO();	

	//
	// deinitialize pools
	//

	// timer entry pool
	FPM_Deinitialize( &m_TimerEntryPool );

	// job pool
	FPM_Deinitialize( &m_JobPool );

	// write data pool
	DNASSERT( m_OutstandingWriteList.IsEmpty() != FALSE );
	m_WriteIODataPool.Deinitialize();

	// read data pool
	DNASSERT( m_OutstandingReadList.IsEmpty() != FALSE );
	m_IPReadIODataPool.Deinitialize();

	//
	// deinitialize job queue
	//
	m_JobQueue.Deinitialize();

	DNDeleteCriticalSection( &m_TimerDataLock );
	DNDeleteCriticalSection( &m_JobDataLock );
	DNDeleteCriticalSection( &m_WriteDataLock );
	DNDeleteCriticalSection( &m_ReadDataLock );
	DNDeleteCriticalSection( &m_Lock );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::StopAllIO - stop all IO from the thread pool
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::StopAllIO"

void	CThreadPool::StopAllIO( void )
{

	StopAllThreads();

	while ( m_JobQueue.IsEmpty() == FALSE )
	{
		THREAD_POOL_JOB	*pThreadPoolJob;


		pThreadPoolJob = GetWorkItem();
		DNASSERT( pThreadPoolJob != NULL );
		pThreadPoolJob->pCancelFunction( pThreadPoolJob );
		pThreadPoolJob->JobType = JOB_UNINITIALIZED;

		m_JobPool.Release( &m_JobPool, pThreadPoolJob );
	}

	//
	// Now that all of the threads are stopped, clean up any outstanding I/O.
	// this can be done without taking any locks
	//
	CancelOutstandingIO();	
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::GetNewReadIOData - get new read request from pool
//
// Entry:		Pointer to context
//
// Exit:		Pointer to new read request
//				NULL = out of memory
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::GetNewReadIOData"

CReadIOData	*CThreadPool::GetNewReadIOData( READ_IO_DATA_POOL_CONTEXT *const pContext )
{
	CReadIOData		*pTempReadData;


	DNASSERT( pContext != NULL );

	//
	// initialize
	//
	pTempReadData = NULL;
	pContext->pThreadPool = this;

	LockReadData();

			pTempReadData = m_IPReadIODataPool.Get( pContext );
	
	if ( pTempReadData == NULL )
	{
		DPF( 0, "Failed to get new ReadIOData from pool!" );
		goto Failure;
	}

	//
	// we have data, immediately add a reference to it
	//
	pTempReadData->AddRef();

	DNASSERT( pTempReadData->m_pSourceSocketAddress != NULL );

	//
	// now that the read data is initialized, add it to the
	// unbound list
	//
	pTempReadData->m_OutstandingReadListLinkage.InsertBefore( &m_OutstandingReadList );

Exit:
	UnlockReadData();

	return pTempReadData;

Failure:
	if ( pTempReadData != NULL )
	{
		pTempReadData->DecRef();
		pTempReadData = NULL;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::ReturnReadIOData - return read data item to pool
//
// Entry:		Pointer to read data
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::ReturnReadIOData"

void	CThreadPool::ReturnReadIOData( CReadIOData *const pReadData )
{
	DNASSERT( pReadData != NULL );
	DNASSERT( pReadData->m_pSourceSocketAddress != NULL );

	//
	// remove this item from the unbound list and return it to the pool
	//
	LockReadData();

	pReadData->m_OutstandingReadListLinkage.RemoveFromList();
	DNASSERT( pReadData->m_OutstandingReadListLinkage.IsEmpty() != FALSE );
	
	switch ( pReadData->m_pSourceSocketAddress->GetFamily() )
	{
		//
		// IP
		//
		case AF_INET:
		{
			m_IPReadIODataPool.Release( pReadData );
			break;
		}

		//
		// unknown type (shouldn't be here!)
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}

	UnlockReadData();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::ProcessTimerJobs - process timed jobs
//
// Entry:		Pointer to job list
//				Pointer to destination for time of next job
//
// Exit:		Boolean indicating active timer jobs exist
//				TRUE = there are active jobs
//				FALSE = there are no active jobs
//
// Notes:	The input job queue is expected to be locked for the duration
//			of this function call!
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::ProcessTimerJobs"

BOOL	CThreadPool::ProcessTimerJobs( const CBilink *const pJobList, DN_TIME *const pNextJobTime )
{
	BOOL		fReturn;
	CBilink		*pWorkingEntry;
	INT_PTR		iActiveTimerJobCount;
	DN_TIME		CurrentTime;


	DNASSERT( pJobList != NULL );
	DNASSERT( pNextJobTime != NULL );

	//
	// Initialize.  Set the next job time to be infinitely far in the future
	// so this thread will wake up for any jobs that need to completed before
	// then.
	//
	fReturn = FALSE;
	DBG_CASSERT( OFFSETOF( TIMER_OPERATION_ENTRY, Linkage ) == 0 );
	pWorkingEntry = pJobList->GetNext();
	iActiveTimerJobCount = 0;
	memset( pNextJobTime, 0xFF, sizeof( *pNextJobTime ) );
	DNTimeGet( &CurrentTime );

	//
	// loop through all timer items
	//
	while ( pWorkingEntry != pJobList )
	{
		TIMER_OPERATION_ENTRY	*pTimerEntry;
		BOOL	fJobActive;


		pTimerEntry = TIMER_OPERATION_ENTRY::TimerOperationFromLinkage( pWorkingEntry );
		pWorkingEntry = pWorkingEntry->GetNext();
		
		fJobActive = ProcessTimedOperation( pTimerEntry, &CurrentTime, pNextJobTime );
		DNASSERT( ( fJobActive == FALSE ) || ( fJobActive == TRUE ) );
		
		fReturn |= fJobActive;

		if ( fJobActive == FALSE )
		{
			RemoveTimerOperationEntry( pTimerEntry, DPN_OK );
		}
	}

	DNASSERT( ( fReturn == FALSE ) || ( fReturn == TRUE ) );

	return	fReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::ProcessTimedOperation - process a timed operation
//
// Entry:		Pointer to job information
//				Pointer to current time
//				Pointer to time to be updated
//
// Exit:		Boolean indicating that the job is still active
//				TRUE = operation active
//				FALSE = operation not active
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::ProcessTimedOperation"

BOOL	CThreadPool::ProcessTimedOperation( TIMER_OPERATION_ENTRY *const pTimedJob,
											const DN_TIME *const pCurrentTime,
											DN_TIME *const pNextJobTime )
{
	BOOL	fEnumActive;


	DNASSERT( pTimedJob != NULL );
	DNASSERT( pCurrentTime != NULL );
	DNASSERT( pNextJobTime != NULL );

	//
	// Assume that this enum will remain active.  If we retire this enum, this
	// value will be reset.
	//
	fEnumActive = TRUE;

	//
	// If this enum has completed sending enums and is waiting only
	// for responses, decrement the wait time (assuming it's not infinite)
	// and remove the enum if the we've exceed its wait time.
	//
	if ( pTimedJob->uRetryCount == 0 )
	{
		if ( DNTimeCompare( &pTimedJob->IdleTimeout, pCurrentTime ) <= 0 )
		{
			fEnumActive = FALSE;
		}
		else
		{
			//
			// This enum isn't complete, check to see if it's the next enum
			// to need service.
			//
			if ( DNTimeCompare( &pTimedJob->IdleTimeout, pNextJobTime ) < 0 )
			{
				DBG_CASSERT( sizeof( *pNextJobTime ) == sizeof( pTimedJob->IdleTimeout ) );
				memcpy( pNextJobTime, &pTimedJob->IdleTimeout, sizeof( *pNextJobTime ) );
			}
		}
	}
	else
	{
		//
		// This enum is still sending.  Determine if it's time to send a new enum
		// and adjust the wakeup time if appropriate.
		//
		if ( DNTimeCompare( &pTimedJob->NextRetryTime, pCurrentTime ) <= 0 )
		{
			HRESULT	hTempResult;


			//
			// Timeout, execute this timed item
			//
			pTimedJob->pTimerCallback( pTimedJob->pContext );

			//
			// If this job isn't running forever, decrement the retry count.
			// If there are no more retries, set up wait time.  If the job
			// is waiting forever, set max wait timeout.
			//
			if ( pTimedJob->fRetryForever == FALSE )
			{
				pTimedJob->uRetryCount--;
				if ( pTimedJob->uRetryCount == 0 )
				{
					if ( pTimedJob->fIdleWaitForever == FALSE )
					{
						//
						// Compute stopping time for this job's 'Timeout' phase and
						// see if this will be the next job to need service.  ASSERT
						// if the math wraps.
						//
						DNTimeAdd( &pTimedJob->IdleTimeout, pCurrentTime, &pTimedJob->IdleTimeout );
						DNASSERT( pTimedJob->IdleTimeout.Time32.TimeHigh >= pCurrentTime->Time32.TimeHigh );
						if ( DNTimeCompare( &pTimedJob->IdleTimeout, pNextJobTime ) < 0 )
						{
							DBG_CASSERT( sizeof( *pNextJobTime ) == sizeof( pTimedJob->IdleTimeout ) );
							memcpy( pNextJobTime, &pTimedJob->IdleTimeout, sizeof( *pNextJobTime ) );
						}
					}
					else
					{
						//
						// We're waiting forever for enum returns.  ASSERT that we
						// have the maximum timeout and don't bother checking to see
						// if this will be the next enum to need service (it'll never
						// need service).  The following code needs to be revisited for
						// 64-bits.
						//
						DNASSERT( pTimedJob->IdleTimeout.Time32.TimeLow == -1 );
						DNASSERT( pTimedJob->IdleTimeout.Time32.TimeHigh == -1 );
					}

					goto SkipNextRetryTimeComputation;
				}
			}

			DNTimeAdd( pCurrentTime, &pTimedJob->RetryInterval, &pTimedJob->NextRetryTime );
		}

		//
		// is this the next enum to fire?
		//
		if ( DNTimeCompare( &pTimedJob->NextRetryTime, pNextJobTime ) < 0 )
		{
			DBG_CASSERT( sizeof( *pNextJobTime ) == sizeof( pTimedJob->NextRetryTime ) );
			memcpy( pNextJobTime, &pTimedJob->NextRetryTime, sizeof( *pNextJobTime ) );
		}

SkipNextRetryTimeComputation:
		//
		// the following blank line is there to shut up the compiler
		//
		;
	}

	return	fEnumActive;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::SubmitWorkItem - submit a work item for processing and inform
//		work thread that another job is available
//
// Entry:		Pointer to job information
//
// Exit:		Error code
//
// Note:	This function assumes that the job data is locked.
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::SubmitWorkItem"

HRESULT	CThreadPool::SubmitWorkItem( THREAD_POOL_JOB *const pJobInfo )
{
	HRESULT	hr;


	DNASSERT( pJobInfo != NULL );
	AssertCriticalSectionIsTakenByThisThread( &m_JobDataLock, TRUE );
	DNASSERT( pJobInfo->pCancelFunction != NULL );


	//
	// initialize
	//
	hr = DPN_OK;

	//
	// add job to queue and tell someone that there's a job available
	//
	m_JobQueue.Lock();
	m_JobQueue.EnqueueJob( pJobInfo );
	m_JobQueue.Unlock();

		//
		// Win9x, set event that the work thread will listen for
		//
			DNASSERT( m_JobQueue.GetPendingJobHandle() != NULL );
			if ( m_JobQueue.SignalPendingJob() == FALSE )
			{
				hr = DPNERR_OUTOFMEMORY;
				DPF( 0, "Failed to signal pending job!" );
				goto Failure;
			}

Exit:
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem with SubmitWorkItem!" );
		DisplayDNError( 0, hr );
	}

	return	hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::GetWorkItem - get a work item from the job queue
//
// Entry:		Nothing
//
// Exit:		Pointer to job information (may be NULL)
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::GetWorkItem"

THREAD_POOL_JOB	*CThreadPool::GetWorkItem( void )
{
	THREAD_POOL_JOB	*pReturn;


	//
	// initialize
	//
	pReturn = NULL;

	m_JobQueue.Lock();
	pReturn = m_JobQueue.DequeueJob();

	//
	// if we're under Win9x (we have a 'pending job' handle),
	// see if the handle needs to be reset
	//
	if ( m_JobQueue.IsEmpty() != FALSE )
	{
		DNASSERT( m_JobQueue.GetPendingJobHandle() != NULL );
		if ( ResetEvent( m_JobQueue.GetPendingJobHandle() ) == FALSE )
		{
			DWORD	dwError;


			dwError = GetLastError();
			DPF( 0, "Problem resetting event for pending Win9x jobs!" );
			DisplayErrorCode( 0, dwError );
		}
	}

	m_JobQueue.Unlock();

	return	pReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::SubmitTimerJob - add a timer job to the timer list
//
// Entry:		Retry count
//				Boolean indicating that we retry forever
//				Retry interval
//				Boolean indicating that we wait forever
//				Idle wait interval
//				Pointer to callback when event fires
//				Pointer to callback when event complete
//				User context
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::SubmitTimerJob"

HRESULT	CThreadPool::SubmitTimerJob( const UINT_PTR uRetryCount,
									 const BOOL fRetryForever,
									 const DN_TIME RetryInterval,
									 const BOOL fIdleWaitForever,
									 const DN_TIME IdleTimeout,
									 TIMER_EVENT_CALLBACK *const pTimerCallbackFunction,
									 TIMER_EVENT_COMPLETE *const pTimerCompleteFunction,
									 void *const pContext )
{
	HRESULT					hr;
	TIMER_OPERATION_ENTRY	*pEntry;
	THREAD_POOL_JOB			*pJob;
	BOOL					fTimerJobListLocked;


	DNASSERT( uRetryCount != 0 );
	DNASSERT( pTimerCallbackFunction != NULL );
	DNASSERT( pTimerCompleteFunction != NULL );
	DNASSERT( pContext != NULL );				// must be non-NULL because it's the lookup key to remove job

	//
	// initialize
	//
	hr = DPN_OK;
	pEntry = NULL;
	pJob = NULL;
	fTimerJobListLocked = FALSE;

	LockJobData();

	//
	// allocate new enum entry
	//
	pEntry = static_cast<TIMER_OPERATION_ENTRY*>( m_TimerEntryPool.Get( &m_TimerEntryPool ) );
	if ( pEntry == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Cannot allocate memory to add to timer list!" );
		goto Failure;
	}
	DNASSERT( pEntry->pContext == NULL );

	//
	// build timer entry block
	//
	pEntry->pContext = pContext;
	pEntry->uRetryCount = uRetryCount;
	pEntry->fRetryForever = fRetryForever;
	pEntry->RetryInterval = RetryInterval;
	pEntry->IdleTimeout = IdleTimeout;
	pEntry->fIdleWaitForever = fIdleWaitForever;
	pEntry->pTimerCallback = pTimerCallbackFunction;
	pEntry->pTimerComplete = pTimerCompleteFunction;

	//
	// set this enum to fire as soon as it gets a chance
	//
	memset( &pEntry->NextRetryTime, 0x00, sizeof( pEntry->NextRetryTime ) );

	pJob = static_cast<THREAD_POOL_JOB*>( m_JobPool.Get( &m_JobPool ) );
	if ( pJob == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Cannot allocate memory for enum job!" );
		goto Failure;
	}

	//
	// Create job for work thread.
	//
	pJob->pCancelFunction = CancelRefreshTimerJobs;
	pJob->JobType = JOB_REFRESH_TIMER_JOBS;

	// set our dummy paramter to simulate passing data
	DEBUG_ONLY( pJob->JobData.JobRefreshTimedJobs.uDummy = 0 );

	LockTimerData();
	fTimerJobListLocked = TRUE;

	//
	// we can submit the 'ENUM_REFRESH' job before inserting the enum entry
	// into the active enum list because nobody will be able to pull the
	// 'ENUM_REFRESH' job from the queue since we have the queue locked
	//
	hr = SubmitWorkItem( pJob );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem submitting enum work item" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

	//
	// debug block to check for duplicate contexts
	//
	DEBUG_ONLY(
				{
					CBilink	*pTempLink;


					pTempLink = m_TimerJobList.GetNext();
					while ( pTempLink != &m_TimerJobList )
					{
						TIMER_OPERATION_ENTRY	*pTempTimerEntry;
					
						
						pTempTimerEntry = TIMER_OPERATION_ENTRY::TimerOperationFromLinkage( pTempLink );
						DNASSERT( pTempTimerEntry->pContext != pContext );
						pTempLink = pTempLink->GetNext();
					}
				}
			);

	//
	// link to rest of list
	//
	pEntry->Linkage.InsertAfter( &m_TimerJobList );

Exit:
	if ( fTimerJobListLocked != FALSE )
	{
		UnlockTimerData();
		fTimerJobListLocked = FALSE;
	}

	UnlockJobData();

	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem with SubmitEnumJob" );
		DisplayDNError( 0, hr );
	}

	return	hr;

Failure:
	if ( pEntry != NULL )
	{
		m_TimerEntryPool.Release( &m_TimerEntryPool, pEntry );
		DEBUG_ONLY( pEntry = NULL );
	}

	if ( pJob != NULL )
	{
		m_JobPool.Release( &m_JobPool, pJob );
		DEBUG_ONLY( pJob = NULL );
	}

	//
	// It's possible that the enum thread has been started for this enum.
	// Since there's no way to stop it without completing the enums or
	// closing the SP, leave it running.
	//

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::CancelRefreshTimerJobs - cancel job to refresh timer jobs
//
// Entry:		Job data
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::CancelRefreshTimerJobs"

void	CThreadPool::CancelRefreshTimerJobs( THREAD_POOL_JOB *const pJob )
{
	DNASSERT( pJob != NULL );

	//
	// this function doesn't need to do anything
	//
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::StopTimerJob - remove timer job from list
//
// Entry:		Pointer to job context (these MUST be uniquie for jobs)
//				Command result
//
// Exit:		Nothing
//
// Note:	This function is for the forced removal of a job from the timed job
//			list.  It is assumed that the caller of this function will clean
//			up any messes.
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::StopTimerJob"

void	CThreadPool::StopTimerJob( void *const pContext, const HRESULT hCommandResult )
{
	CBilink	*pTempEntry;


	DNASSERT( pContext != NULL );

	//
	// initialize
	//
	LockTimerData();

	pTempEntry = m_TimerJobList.GetNext();
	while ( pTempEntry != &m_TimerJobList )
	{
		TIMER_OPERATION_ENTRY	*pTimerEntry;


		pTimerEntry = TIMER_OPERATION_ENTRY::TimerOperationFromLinkage( pTempEntry );
		if ( pTimerEntry->pContext == pContext )
		{
			RemoveTimerOperationEntry( pTimerEntry, hCommandResult );

			//
			// terminate loop
			//
			pTempEntry = &m_TimerJobList;
		}
		else
		{
			pTempEntry = pTempEntry->GetNext();
		}
	}

	UnlockTimerData();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::SubmitDelayedCommand - submit request to enum query to remote session
//
// Entry:		Pointer to callback function
//				Pointer to cancel function
//				Pointer to callback context
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::SubmitDelayedCommand"

HRESULT	CThreadPool::SubmitDelayedCommand( JOB_FUNCTION *const pFunction, JOB_FUNCTION *const pCancelFunction, void *const pContext )
{
	HRESULT			hr;
	THREAD_POOL_JOB	*pJob;
	BOOL			fJobDataLocked;


	DNASSERT( pFunction != NULL );
	DNASSERT( pCancelFunction != NULL );

	//
	// initialize
	//
	hr = DPN_OK;
	pJob = NULL;
	fJobDataLocked = FALSE;

	pJob = static_cast<THREAD_POOL_JOB*>( m_JobPool.Get( &m_JobPool ) );
	if ( pJob == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Cannot allocate job for DelayedCommand!" );
		goto Failure;
	}

	pJob->JobType = JOB_DELAYED_COMMAND;
	pJob->pCancelFunction = pCancelFunction;
	pJob->JobData.JobDelayedCommand.pCommandFunction = pFunction;
	pJob->JobData.JobDelayedCommand.pContext = pContext;

	LockJobData();
	fJobDataLocked = TRUE;

	hr = SubmitWorkItem( pJob );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem submitting DelayedCommand job!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

Exit:
	if ( fJobDataLocked != FALSE )
	{
		UnlockJobData();
		fJobDataLocked = FALSE;
	}

	return	hr;

Failure:
	if ( pJob != NULL )
	{
		m_JobPool.Release( &m_JobPool, pJob );
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::AddSocketPort - add a socket to the Win9x watch list
//
// Entry:		Pointer to SocketPort
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::AddSocketPort"

HRESULT	CThreadPool::AddSocketPort( CSocketPort *const pSocketPort )
{
	HRESULT	hr;
	BOOL	fSocketAdded;

	
	DNASSERT( pSocketPort != NULL );

	//
	// initialize
	//
	hr = DPN_OK;
	fSocketAdded = FALSE;

	Lock();

	//
	// We're capped by the number of sockets we can use for Winsock1.  Make
	// sure we don't allocate too many sockets.
	//
	if ( m_uReservedSocketCount == FD_SETSIZE )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "There are too many sockets allocated on Winsock1!" );
		goto Failure;
	}

	m_uReservedSocketCount++;
	
	DNASSERT( m_SocketSet.fd_count < FD_SETSIZE );
	m_pSocketPorts[ m_SocketSet.fd_count ] = pSocketPort;
	m_SocketSet.fd_array[ m_SocketSet.fd_count ] = pSocketPort->GetSocket();
	m_SocketSet.fd_count++;
	fSocketAdded = TRUE;

	//
	// add a reference to note that this socket port is being used by the thread
	// pool
	//
	pSocketPort->AddRef();

	if ( m_JobQueue.SignalPendingJob() == FALSE )
	{
		DPF( 0, "Failed to signal pending job when adding socket port to active list!" );
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

Exit:
	Unlock();
	
	return	hr;

Failure:
	if ( fSocketAdded != FALSE )
	{
		AssertCriticalSectionIsTakenByThisThread( &m_Lock, TRUE );
		m_SocketSet.fd_count--;
		m_pSocketPorts[ m_SocketSet.fd_count ] = NULL;
		m_SocketSet.fd_array[ m_SocketSet.fd_count ] = NULL;
		fSocketAdded = FALSE;
	}

	m_uReservedSocketCount--;

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::RemoveSocketPort - remove a socket from the Win9x watch list
//
// Entry:		Pointer to socket port to remove
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::RemoveSocketPort"

void	CThreadPool::RemoveSocketPort( CSocketPort *const pSocketPort )
{
	UINT_PTR	uIndex;


	DNASSERT( pSocketPort != NULL );
	
	Lock();

	uIndex = m_SocketSet.fd_count;
	DNASSERT( uIndex != 0 );
	while ( uIndex != 0 )
	{
		uIndex--;

		if ( m_pSocketPorts[ uIndex ] == pSocketPort )
		{
			m_uReservedSocketCount--;
			m_SocketSet.fd_count--;

			memmove( &m_pSocketPorts[ uIndex ],
					 &m_pSocketPorts[ uIndex + 1 ],
					 ( sizeof( m_pSocketPorts[ uIndex ] ) * ( m_SocketSet.fd_count - uIndex ) ) );

			memmove( &m_SocketSet.fd_array[ uIndex ],
					 &m_SocketSet.fd_array[ uIndex + 1 ],
					 ( sizeof( m_SocketSet.fd_array[ uIndex ] ) * ( m_SocketSet.fd_count - uIndex ) ) );

			//
			// clear last entry which is now unused
			//
			memset( &m_pSocketPorts[ m_SocketSet.fd_count ], 0x00, sizeof( m_pSocketPorts[ m_SocketSet.fd_count ] ) );
			memset( &m_SocketSet.fd_array[ m_SocketSet.fd_count ], 0x00, sizeof( m_SocketSet.fd_array[ m_SocketSet.fd_count ] ) );

			//
			// end the loop
			//
			uIndex = 0;
		}
	}

	Unlock();
	
	pSocketPort->DecRef();

	//
	// It's really not necessary to signal a new job here because there were
	// active sockets on the last iteration of the Win9x thread.  That means the
	// Win9x thread was in a polling mode to check for sockets and the next time
	// through it will notice that there is a missing socket.  By signalling the
	// job event we reduce the time needed for the thread to figure out that the
	// socket is gone.
	//
	if ( m_JobQueue.SignalPendingJob() == FALSE )
	{
		DPF( 0, "Failed to signal pending job when removeing socket port to active list!" );
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::RemoveTimerOperationEntry - remove timer operation job	from list
//
// Entry:		Pointer to timer operation
//				Result code to return
//
// Exit:		Nothing
//
// Note:	This function assumes that the list is appropriately locked
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::RemoveTimerOperationEntry"

void	CThreadPool::RemoveTimerOperationEntry( TIMER_OPERATION_ENTRY *const pTimerEntry, const HRESULT hJobResult )
{
	DNASSERT( pTimerEntry != NULL );
	AssertCriticalSectionIsTakenByThisThread( &m_TimerDataLock, TRUE );

	//
	// remove this link from the list, tell owner that the job is complete and
	// return the job to the pool
	//
	pTimerEntry->Linkage.RemoveFromList();
	pTimerEntry->pTimerComplete( hJobResult, pTimerEntry->pContext );
	m_TimerEntryPool.Release( &m_TimerEntryPool, pTimerEntry );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::CompleteOutstandingSends - check for completed sends and
//		indicate send completion for them.
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::CompleteOutstandingSends"

void	CThreadPool::CompleteOutstandingSends( void )
{
	CBilink		*pCurrentOutstandingWrite;
	CBilink		WritesToBeProcessed;


	WritesToBeProcessed.Initialize();
	LockWriteData();

	//
	// Loop through the list out outstanding sends.  Any completed sends are
	// removed from the list and processed after we release the write data lock.
	//
	pCurrentOutstandingWrite = m_OutstandingWriteList.GetNext();
	while ( pCurrentOutstandingWrite != &m_OutstandingWriteList )
	{
		CSocketPort		*pSocketPort;
		CWriteIOData	*pWriteIOData;
		DWORD			dwFlags;
		BOOL	fGetOverlappedResult;


		//
		// note this send and advance pointer to the next pending send
		//
		pWriteIOData = pWriteIOData->WriteDataFromBilink( pCurrentOutstandingWrite );
		pCurrentOutstandingWrite = pCurrentOutstandingWrite->GetNext();

		if ( pWriteIOData->Win9xOperationPending() != FALSE )
		{
			DNEnterCriticalSection(&g_csSocketAccessLock);
			fGetOverlappedResult = p_WSAGetOverlappedResult( pWriteIOData->SocketPort()->GetSocket(),
										   pWriteIOData->Overlap(),
										   &pWriteIOData->m_dwOverlappedBytesSent,
										   FALSE,
										   &dwFlags
										   );
			DNLeaveCriticalSection(&g_csSocketAccessLock);

			if ( fGetOverlappedResult != FALSE )
			{
				//
				// Overlapped results will complete with success and zero bytes
				// transferred when the overlapped structure is checked BEFORE
				// the operation has really been subnitted.  This is a possibility
				// with the current code.  To combat this, check the sent bytes
				// for zero (we'll never send zero bytes).
				//
				if ( pWriteIOData->m_dwOverlappedBytesSent == 0 )
				{
					goto SkipSendCompletion;
				}

				pWriteIOData->m_Win9xSendHResult = DPN_OK;
				pWriteIOData->m_dwOverlappedBytesSent = 0;
			}
			else
			{
				DWORD	dwWSAError;


				dwWSAError = p_WSAGetLastError();
				switch( dwWSAError )
				{
					//
					// this I/O operation is incomplete, don't send notification to the user
					//
					case ERROR_IO_PENDING:
					case WSA_IO_INCOMPLETE:
					{
						goto SkipSendCompletion;
						break;
					}

                    case WSAEINPROGRESS:		// BUGBUG: [mgere] [xbox] Added handler for new Winsock error message.  Check to see if this is correct.
					{
						goto SkipSendCompletion;
						break;
					}

					//
					// WSAENOTSOCK = the socket has been closed, most likely
					// as a result of a command completing or being cancelled.
					//
					case WSAENOTSOCK:
					{
						pWriteIOData->m_Win9xSendHResult = DPNERR_USERCANCEL;
						break;
					}

					//
					// other error, stop and look
					//
					default:
					{
						INT3;
						pWriteIOData->m_Win9xSendHResult = DPNERR_GENERIC;
						DisplayWinsockError( 0, dwWSAError );

						break;
					}
				}
			}

			DNASSERT( pWriteIOData->Win9xOperationPending() != FALSE );
			pWriteIOData->SetWin9xOperationPending( FALSE );

			pWriteIOData->m_OutstandingWriteListLinkage.RemoveFromList();
			pWriteIOData->m_OutstandingWriteListLinkage.InsertBefore( &WritesToBeProcessed );
		}

SkipSendCompletion:
		//
		// the following line is present to prevent the compiler from whining
		// about a blank line
		//
		;
	}

	UnlockWriteData();

	//
	// process all writes that have been pulled to the side.
	//
	while (  WritesToBeProcessed.GetNext() != &WritesToBeProcessed )
	{
		BOOL			fIOServiced;
		CWriteIOData	*pTempWrite;
		CSocketPort		*pSocketPort;


		pTempWrite = CWriteIOData::WriteDataFromBilink( WritesToBeProcessed.GetNext() );
		pTempWrite->m_OutstandingWriteListLinkage.RemoveFromList();
		pSocketPort = pTempWrite->SocketPort();
		DNASSERT( pSocketPort != NULL );

		fIOServiced = pSocketPort->SendFromWriteQueue();
		pSocketPort->SendComplete( pTempWrite, pTempWrite->m_Win9xSendHResult );
		pSocketPort->DecRef();
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::CompleteOutstandingReceives - check for completed receives and
//		indicate completion for them.
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::CompleteOutstandingReceives"

void	CThreadPool::CompleteOutstandingReceives( void )
{
	CBilink		*pCurrentOutstandingRead;
	CBilink		ReadsToBeProcessed;


	ReadsToBeProcessed.Initialize();
	LockReadData();

	//
	// Loop through the list of outstanding reads and pull out the ones that need
	// to be serviced.  We don't want to service them while the read data lock
	// is taken.
	//
	pCurrentOutstandingRead = m_OutstandingReadList.GetNext();
	while ( pCurrentOutstandingRead != &m_OutstandingReadList )
	{
		CSocketPort		*pSocketPort;
		CReadIOData		*pReadIOData;
		DWORD			dwFlags;
		BOOL		fGetOverlappedResult;


		pReadIOData = pReadIOData->ReadDataFromBilink( pCurrentOutstandingRead );
		pCurrentOutstandingRead = pCurrentOutstandingRead->GetNext();

		//
		// Make sure this operation is really pending before attempting to check
		// for completion.  It's possible that the read was added to the list, but
		// we haven't actually called Winsock yet.
		//
		if ( pReadIOData->Win9xOperationPending() != FALSE )
		{
			if (pReadIOData->SocketPort()->GetSocket() != INVALID_SOCKET)
			{

			DNEnterCriticalSection(&g_csSocketAccessLock);
			fGetOverlappedResult = p_WSAGetOverlappedResult( pReadIOData->SocketPort()->GetSocket(),
										   pReadIOData->Overlap(),
										   &pReadIOData->m_dwOverlappedBytesReceived,
										   FALSE,
										   &dwFlags
										   );
			DNLeaveCriticalSection(&g_csSocketAccessLock);

			if ( fGetOverlappedResult != FALSE )
			{
				//
				// Overlapped results will complete with success and zero bytes
				// transferred when the overlapped structure is checked BEFORE
				// the operation has really been subnitted.  This is a possibility
				// with the current code.  To combat this, check the received bytes
				// for zero (the return when the overlapped request was checked before
				// it was sent) and check the return address (it's possible that someone
				// really sent zero bytes).
				//
				DBG_CASSERT( ERROR_SUCCESS == 0 );
				if ( ( pReadIOData->m_dwOverlappedBytesReceived != 0 ) &&
					 ( pReadIOData->m_pSourceSocketAddress->IsUndefinedHostAddress() == FALSE ) )
				{
					pReadIOData->m_Win9xReceiveWSAReturn = ERROR_SUCCESS;
				}
				else
				{
					goto SkipReceiveCompletion;
				}
			}
			else
			{
				pReadIOData->m_Win9xReceiveWSAReturn = p_WSAGetLastError();
				switch( pReadIOData->m_Win9xReceiveWSAReturn )
				{
					//
					// If this I/O operation is incomplete, don't send notification to the user.
					//
					case WSA_IO_INCOMPLETE:
					{
						goto SkipReceiveCompletion;
						break;
					}

						case WSAEINPROGRESS:		// BUGBUG: [mgere] [xbox] Added handler for new Winsock error message.  Check to see if this is correct.
					{
					goto SkipReceiveCompletion;
						break;
					}

					//
					// socket was closed with an outstanding read, no problem
					// Win9x reports 'WSAENOTSOCK'
					// WinNT reports 'ERROR_OPERATION_ABORTED'
					//
					// If this is an indication that the connection was reset,
					// pass it on to the socket port so it can issue another
					// read
					//
					case ERROR_OPERATION_ABORTED:
					case WSAENOTSOCK:
					case WSAECONNRESET:
					{
						break;
					}

					default:
					{
						DisplayWinsockError( 0, pReadIOData->m_Win9xReceiveWSAReturn );
							// debug me!
						DNASSERT( FALSE );
							break;
					}
				}
			}

			}

			DNASSERT( pReadIOData->Win9xOperationPending() != FALSE );
			pReadIOData->SetWin9xOperationPending( FALSE );

			pReadIOData->m_OutstandingReadListLinkage.RemoveFromList();
			pReadIOData->m_OutstandingReadListLinkage.InsertBefore( &ReadsToBeProcessed );
		}

SkipReceiveCompletion:
		//
		// the following line is present to prevent the compiler from whining
		// about a blank line
		//
		;
	}

	UnlockReadData();

	//
	// loop through the list of reads that have completed and dispatch them
	//
	while ( ReadsToBeProcessed.GetNext() != &ReadsToBeProcessed )
	{
		CReadIOData		*pTempRead;
		CSocketPort		*pSocketPort;


		pTempRead = CReadIOData::ReadDataFromBilink( ReadsToBeProcessed.GetNext() );
		pTempRead->m_OutstandingReadListLinkage.RemoveFromList();

		pSocketPort = pTempRead->SocketPort();
		DNASSERT( pSocketPort != NULL );
		pSocketPort->Winsock2ReceiveComplete( pTempRead );
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::PrimaryWin9xThread - main thread to do everything that the SP is
//		supposed to do under Win9x.
//
// Entry:		Pointer to startup parameter
//
// Exit:		Error Code
//
// Note:	The startup parameter is allocated for this thread and must be
//			deallocated by this thread when it exits
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::PrimaryWin9xThread"
#ifdef DPLAY_DOWORK
DWORD CThreadPool::PrimaryWin9xThread( void )
{
	static WIN9X_CORE_DATA		CoreData;
	static DN_TIME				CurrentTime;
	static DWORD				dwMaxWaitTime;
	static DN_TIME				DeltaT;
	static BOOL fFirstRun = TRUE;
	DWORD	dwWaitReturn;
	DWORD dwNoMoreWork = 0;

	if (fFirstRun == TRUE)
	{
		fFirstRun = FALSE;

		//
		// initialize
		//
		memset( &CoreData, 0x00, sizeof CoreData );

		//
		// Clear socket data.  Since we need to correlate a CSocketPort with a SOCKET,
		// we're going to manage the FD_SET ourselves.  See Winsock.h for the FD_SET
		// structure definition.
		//
		DBG_CASSERT( OFFSETOF( FD_SET, fd_count ) == 0 );
		DNASSERT( CoreData.fTimerJobsActive == FALSE );

		//
		// set enums to happen infinitely in the future
		//
		memset( &CoreData.NextTimerJobTime, 0xFF, sizeof( CoreData.NextTimerJobTime ) );

		//
		// set wait handles
		//
		CoreData.hWaitHandles[ EVENT_INDEX_STOP_ALL_THREADS ] = m_hStopAllThreads;
		CoreData.hWaitHandles[ EVENT_INDEX_PENDING_JOB ] = m_JobQueue.GetPendingJobHandle();
		CoreData.hWaitHandles[ EVENT_INDEX_WINSOCK_2_SEND_COMPLETE ] = GetWinsock2SendCompleteEvent();
		CoreData.hWaitHandles[ EVENT_INDEX_WINSOCK_2_RECEIVE_COMPLETE ] = GetWinsock2ReceiveCompleteEvent();
	
		DNASSERT( CoreData.hWaitHandles[ EVENT_INDEX_STOP_ALL_THREADS ] != NULL );
		DNASSERT( CoreData.hWaitHandles[ EVENT_INDEX_PENDING_JOB ] != NULL );
		DNASSERT( CoreData.hWaitHandles[ EVENT_INDEX_WINSOCK_2_SEND_COMPLETE ] != NULL );
		DNASSERT( CoreData.hWaitHandles[ EVENT_INDEX_WINSOCK_2_RECEIVE_COMPLETE ] != NULL );

		CoreData.fLooping = TRUE;
	}

	//
	// Update the job time so we know how long to wait.  We can
	// only get here if a socket was just added to the socket list, or
	// we've been servicing sockets.
	//
	DNTimeGet( &CurrentTime );
	if ( DNTimeCompare( &CurrentTime, &CoreData.NextTimerJobTime ) >= 0 )
	{
		LockTimerData();
		CoreData.fTimerJobsActive = ProcessTimerJobs( &m_TimerJobList,
								&CoreData.NextTimerJobTime );
		if ( CoreData.fTimerJobsActive != FALSE )
		{
			DPF( 8, "There are active jobs left with Winsock1 sockets active!" );
		}
		UnlockTimerData();
	}
	DNTimeSubtract( &CoreData.NextTimerJobTime, &CurrentTime, &DeltaT );
	dwMaxWaitTime = static_cast<DWORD>( SaturatedWaitTime( DeltaT ) );

	//
	// go until we're told to stop
	//
	if ( CoreData.fLooping == FALSE )
	{
		SetEvent(m_hThreadCloseEvent);
		fFirstRun = TRUE;
		return 1;
	}

	//
	// Check Winsock2 sockets.
	//
	dwWaitReturn = WaitForMultipleObjectsEx( LENGTHOF( CoreData.hWaitHandles ),		// count of handles
								 CoreData.hWaitHandles,					// handles to wait on
								 FALSE,									// don't wait for all to be signalled
								 0,							// wait timeout
//								 0,							// wait timeout
								 TRUE									// we're alertable for APCs
								 );
	switch ( dwWaitReturn )
	{
		//
		// timeout, don't do anything, we'll probably process timer jobs on
		// the next loop
		//
		case WAIT_TIMEOUT:
		{
			dwNoMoreWork = 1;
			break;
		}
			
		case ( WAIT_OBJECT_0 + EVENT_INDEX_PENDING_JOB ):
		case ( WAIT_OBJECT_0 + EVENT_INDEX_STOP_ALL_THREADS ):
		case ( WAIT_OBJECT_0 + EVENT_INDEX_WINSOCK_2_SEND_COMPLETE ):
		case ( WAIT_OBJECT_0 + EVENT_INDEX_WINSOCK_2_RECEIVE_COMPLETE ):
		{
			ProcessWin9xEvents( &CoreData );
			break;
		}

		//
		// wait failed
		//
		case WAIT_FAILED:
		{
			DWORD	dwError;

			dwNoMoreWork = 1;
			dwError = GetLastError();
			DPF( 0, "Primary Win9x thread wait failed!" );
			DisplayDNError( 0, dwError );
			break;
		}

		//
		// problem
		//
		default:
		{
			DWORD	dwError;

			dwNoMoreWork = 1;
			dwError = GetLastError();
			DPF( 0, "Primary Win9x thread unknown problem in wait!" );
			DisplayDNError( 0, dwError );
			DNASSERT( FALSE );
			break;
		}
	}

	return	dwNoMoreWork;
}
#else
DWORD	WINAPI	CThreadPool::PrimaryWin9xThread( void *pParam )
{
	WIN9X_CORE_DATA		CoreData;
	DN_TIME				CurrentTime;
	DWORD				dwMaxWaitTime;
	DN_TIME				DeltaT;
	BOOL				fComInitialized;

	CThreadPool		*const pThisThreadPool = static_cast<WIN9X_THREAD_DATA *>( pParam )->pThisThreadPool;
	FD_SET 			*const pSocketSet = &pThisThreadPool->m_SocketSet;

	
	DNASSERT( pParam != NULL );
	DNASSERT( pThisThreadPool != NULL );
	DNASSERT( pSocketSet != NULL );

	//
	// initialize
	//
	memset( &CoreData, 0x00, sizeof CoreData );
	fComInitialized = FALSE;

	//
	// before we do anything we need to make sure COM is happy
	//
	switch ( COM_CoInitialize( NULL ) )
	{
		//
		// no problem
		//
		case S_OK:
		{
			fComInitialized = TRUE;
			break;
		}

		//
		// COM already initialized, huh?
		//
		case S_FALSE:
		{
			DNASSERT( FALSE );
			break;
		}

		//
		// COM init failed!
		//
		default:
		{
			DPF( 0, "Primary Win9x thread failed to initialize COM!" );
			DNASSERT( FALSE );
			break;
		}
	}

	//
	// Clear socket data.  Since we need to correlate a CSocketPort with a SOCKET,
	// we're going to manage the FD_SET ourselves.  See Winsock.h for the FD_SET
	// structure definition.
	//
	DBG_CASSERT( OFFSETOF( FD_SET, fd_count ) == 0 );
//#ifdef	_WIN32
//	DBG_CASSERT( OFFSETOF( FD_SET, fd_array ) == sizeof( pSocketSet->fd_count ) );
//#endif	// _WIN32
//	DNASSERT( pThisThreadPool->m_uReservedSocketCount == 0 );
//	DNASSERT( pSocketSet->fd_count == 0 );
	DNASSERT( CoreData.fTimerJobsActive == FALSE );

	//
	// set enums to happen infinitely in the future
	//
	memset( &CoreData.NextTimerJobTime, 0xFF, sizeof( CoreData.NextTimerJobTime ) );

	//
	// set wait handles
	//
	CoreData.hWaitHandles[ EVENT_INDEX_STOP_ALL_THREADS ] = pThisThreadPool->m_hStopAllThreads;
	CoreData.hWaitHandles[ EVENT_INDEX_PENDING_JOB ] = pThisThreadPool->m_JobQueue.GetPendingJobHandle();
	CoreData.hWaitHandles[ EVENT_INDEX_WINSOCK_2_SEND_COMPLETE ] = pThisThreadPool->GetWinsock2SendCompleteEvent();
	CoreData.hWaitHandles[ EVENT_INDEX_WINSOCK_2_RECEIVE_COMPLETE ] = pThisThreadPool->GetWinsock2ReceiveCompleteEvent();
	
	DNASSERT( CoreData.hWaitHandles[ EVENT_INDEX_STOP_ALL_THREADS ] != NULL );
	DNASSERT( CoreData.hWaitHandles[ EVENT_INDEX_PENDING_JOB ] != NULL );
	DNASSERT( CoreData.hWaitHandles[ EVENT_INDEX_WINSOCK_2_SEND_COMPLETE ] != NULL );
	DNASSERT( CoreData.hWaitHandles[ EVENT_INDEX_WINSOCK_2_RECEIVE_COMPLETE ] != NULL );
	
	//
	// go until we're told to stop
	//
	CoreData.fLooping = TRUE;
	while ( CoreData.fLooping != FALSE )
	{
		DWORD	dwWaitReturn;

		
		//
		// Update the job time so we know how long to wait.  We can
		// only get here if a socket was just added to the socket list, or
		// we've been servicing sockets.
		//
		DNTimeGet( &CurrentTime );
		if ( DNTimeCompare( &CurrentTime, &CoreData.NextTimerJobTime ) >= 0 )
		{
			pThisThreadPool->LockTimerData();
			CoreData.fTimerJobsActive = pThisThreadPool->ProcessTimerJobs( &pThisThreadPool->m_TimerJobList,
																		   &CoreData.NextTimerJobTime );
			if ( CoreData.fTimerJobsActive != FALSE )
			{
				DPF( 8, "There are active jobs left with Winsock1 sockets active!" );
			}
			pThisThreadPool->UnlockTimerData();
		}

		DNTimeSubtract( &CoreData.NextTimerJobTime, &CurrentTime, &DeltaT );
#pragma	BUGBUG( johnkan, "Busted Win64!" )
		dwMaxWaitTime = static_cast<DWORD>( SaturatedWaitTime( DeltaT ) );


		//
		// Check Winsock2 sockets.
		//
		dwWaitReturn = WaitForMultipleObjectsEx( LENGTHOF( CoreData.hWaitHandles ),		// count of handles
												 CoreData.hWaitHandles,					// handles to wait on
												 FALSE,									// don't wait for all to be signalled
												 dwMaxWaitTime,							// wait timeout
												 TRUE									// we're alertable for APCs
												 );
		switch ( dwWaitReturn )
		{
			//
			// timeout, don't do anything, we'll probably process timer jobs on
			// the next loop
			//
			case WAIT_TIMEOUT:
			{
				break;
			}
			
			case ( WAIT_OBJECT_0 + EVENT_INDEX_PENDING_JOB ):
			case ( WAIT_OBJECT_0 + EVENT_INDEX_STOP_ALL_THREADS ):
			case ( WAIT_OBJECT_0 + EVENT_INDEX_WINSOCK_2_SEND_COMPLETE ):
			case ( WAIT_OBJECT_0 + EVENT_INDEX_WINSOCK_2_RECEIVE_COMPLETE ):
			{
				pThisThreadPool->ProcessWin9xEvents( &CoreData );
				break;
			}

			//
			// There are I/O completion routines scheduled on this thread.
			// This is not a good thing!
			//
			case WAIT_IO_COMPLETION:
			{
				DPF( 1, "WARNING: APC was serviced on the primary Win9x IO service thread!  What is the application doing??" );
				break;
			}

			//
			// wait failed
			//
			case WAIT_FAILED:
			{
				DWORD	dwError;


				dwError = GetLastError();
				DPF( 0, "Primary Win9x thread wait failed!" );
				DisplayDNError( 0, dwError );
				break;
			}

			//
			// problem
			//
			default:
			{
				DWORD	dwError;


				dwError = GetLastError();
				DPF( 0, "Primary Win9x thread unknown problem in wait!" );
				DisplayDNError( 0, dwError );
				DNASSERT( FALSE );
				break;
			}
		}
	}

	DNFree( pParam );

	SetEvent(pThisThreadPool->m_hThreadCloseEvent);

	if ( fComInitialized != FALSE )
	{
		COM_CoUninitialize();
		fComInitialized = FALSE;
	}

	return	0;
}
#endif
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::ProcessWin9xEvents - process Win9x events
//
// Entry:		Pointer to core data
//				Thread type
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::ProcessWin9xEvents"

void	CThreadPool::ProcessWin9xEvents( WIN9X_CORE_DATA *const pCoreData )
{
	DNASSERT( pCoreData != NULL );

	//
	// If delayed jobs are to be processed, process one.  Otherwise sleep and
	// let another thread pick up the jobs.
	//
	switch ( WaitForSingleObject( pCoreData->hWaitHandles[ EVENT_INDEX_PENDING_JOB ], 0 ) )
	{
		case WAIT_TIMEOUT:
		{
			break;
		}

		case WAIT_OBJECT_0:
		{
					DPF( 8, "Primary Win9x thread has a pending job!" );
					ProcessWin9xJob( pCoreData );

			break;
		}

		default:
		{
			INT3;
			break;
		}
	}

	//
	// send complete
	//
	switch ( WaitForSingleObject( pCoreData->hWaitHandles[ EVENT_INDEX_WINSOCK_2_SEND_COMPLETE ], 0 ) )
	{
		case WAIT_OBJECT_0:
		{
			//
			// reset the event so it will be signalled again if anything
			// completes while we're scanning the pending write list
			//
			if ( ResetEvent( pCoreData->hWaitHandles[ EVENT_INDEX_WINSOCK_2_SEND_COMPLETE ] ) == FALSE )
			{
				DWORD	dwError;


				dwError = GetLastError();
				DPF( 0, "Failed to reset Winsock2 send event!" );
				DisplayErrorCode( 0, dwError );
			}

			CompleteOutstandingSends();
			
			break;
		}

		case WAIT_TIMEOUT:
		{
			break;
		}

		default:
		{
			INT3;
			break;
		}
	}

	//
	// receive complete
	//
	switch ( WaitForSingleObject( pCoreData->hWaitHandles[ EVENT_INDEX_WINSOCK_2_RECEIVE_COMPLETE ], 0 ) )
	{
		case WAIT_OBJECT_0:
		{
			//
			// reset the event so it will be signalled again if anything
			// completes while we're scanning the pending read list
			//
			if ( ResetEvent( pCoreData->hWaitHandles[ EVENT_INDEX_WINSOCK_2_RECEIVE_COMPLETE ] ) == FALSE )
			{
				DWORD	dwError;


				dwError = GetLastError();
				DPF( 0, "Failed to reset Winsock2 receive event!" );
				DisplayErrorCode( 0, dwError );
			}

			CompleteOutstandingReceives();
			
			break;
		}

		case WAIT_TIMEOUT:
		{
			break;
		}

		default:
		{
			INT3;
			break;
		}
	}

	//
	// stop all threads
	//
	switch ( WaitForSingleObject( pCoreData->hWaitHandles[ EVENT_INDEX_STOP_ALL_THREADS ], 0 ) )
	{
		case WAIT_OBJECT_0:
		{
			DPF( 8, "Win9x thread exit because SP closing!" );
			pCoreData->fLooping = FALSE;
			break;
		}
	
		case WAIT_TIMEOUT:
		{
			break;
		}

		default:
		{
			INT3;
			break;
		}
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::ProcessWin9xJob - process a Win9x job
//
// Entry:		Pointer core data
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::ProcessWin9xJob"

void	CThreadPool::ProcessWin9xJob( WIN9X_CORE_DATA *const pCoreData )
{
	THREAD_POOL_JOB	*pJobInfo;


	//
	// Remove and process a single job from the list.  If there is no job, skip
	// to the end of the function.
	//
	pJobInfo = GetWorkItem();

	if ( pJobInfo == NULL )
	{
		goto Exit;
	}

	switch ( pJobInfo->JobType )
	{
		//
		// enum refresh
		//
		case JOB_REFRESH_TIMER_JOBS:
		{
			DPF( 8, "WorkThread job REFRESH_ENUM" );
			DNASSERT( pJobInfo->JobData.JobRefreshTimedJobs.uDummy == 0 );
			LockTimerData();
			pCoreData->fTimerJobsActive = ProcessTimerJobs( &m_TimerJobList, &pCoreData->NextTimerJobTime );
			UnlockTimerData();

			if ( pCoreData->fTimerJobsActive != FALSE )
			{
				DPF( 8, "There are active timer jobs left after processing a Win9x REFRESH_TIMER_JOBS" );
			}

			break;
		}

		//
		// issue callback for this job
		//
		case JOB_DELAYED_COMMAND:
		{
			DPF( 8, "WorkThread job DELAYED_COMMAND" );
			DNASSERT( pJobInfo->JobData.JobDelayedCommand.pCommandFunction != NULL );
			pJobInfo->JobData.JobDelayedCommand.pCommandFunction( pJobInfo );
			break;
		}

		//
		// other job
		//
		default:
		{
			DPF( 0, "WorkThread Win9x job unknown!" );
			DNASSERT( FALSE );
			break;
		}
	}

	DEBUG_ONLY( pJobInfo->JobType = JOB_UNINITIALIZED );
	m_JobPool.Release( &m_JobPool, pJobInfo );

Exit:
	return;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::WorkThreadJob_Alloc - allocate a new job
//
// Entry:		Pointer to new entry
//
// Exit:		Boolean indicating success
//				TRUE = initialization successful
//				FALSE = initialization failed
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::WorkThreadJob_Alloc"

BOOL	CThreadPool::WorkThreadJob_Alloc( void *pItem )
{
	BOOL			fReturn;
	THREAD_POOL_JOB	*pJob;


	//
	// initialize
	//
	fReturn = TRUE;
	pJob = static_cast<THREAD_POOL_JOB*>( pItem );

	DEBUG_ONLY( memset( pJob, 0x00, sizeof( *pJob ) ) );

	return	fReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::WorkThreadJob_Get - a job is being removed from the pool
//
// Entry:		Pointer to job
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::WorkThreadJob_Get"

void	CThreadPool::WorkThreadJob_Get( void *pItem )
{
	THREAD_POOL_JOB	*pJob;


	//
	// initialize
	//
	pJob = static_cast<THREAD_POOL_JOB*>( pItem );
	DNASSERT( pJob->JobType == JOB_UNINITIALIZED );

	//
	// cannot ASSERT the the following because the pool manager uses that memory
	//
//	DNASSERT( pJob->pNext == NULL );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::WorkThreadJob_Release - a job is being returned to the pool
//
// Entry:		Pointer to job
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::WorkThreadJob_Release"

void	CThreadPool::WorkThreadJob_Release( void *pItem )
{
	THREAD_POOL_JOB	*pJob;


	DNASSERT( pItem != NULL );
	pJob = static_cast<THREAD_POOL_JOB*>( pItem );

	DNASSERT( pJob->JobType == JOB_UNINITIALIZED );
	pJob->pNext = NULL;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::WorkThreadJob_Dealloc - return job to memory manager
//
// Entry:		Pointer to job
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::WorkThreadJob_Dealloc"

void	CThreadPool::WorkThreadJob_Dealloc( void *pItem )
{
	// don't do anything
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::TimerEntry_Alloc - allocate a new timer job entry
//
// Entry:		Pointer to new entry
//
// Exit:		Boolean indicating success
//				TRUE = initialization successful
//				FALSE = initialization failed
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::TimerEntry_Alloc"

BOOL	CThreadPool::TimerEntry_Alloc( void *pItem )
{
	BOOL					fReturn;
	TIMER_OPERATION_ENTRY	*pTimerEntry;


	DNASSERT( pItem != NULL );

	//
	// initialize
	//
	fReturn = TRUE;
	pTimerEntry = static_cast<TIMER_OPERATION_ENTRY*>( pItem );
	DEBUG_ONLY( memset( pTimerEntry, 0x00, sizeof( *pTimerEntry ) ) );
	pTimerEntry->pContext = NULL;
	pTimerEntry->Linkage.Initialize();

	return	fReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::TimerEntry_Get - get new timer job entry from pool
//
// Entry:		Pointer to new entry
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::TimerEntry_Get"

void	CThreadPool::TimerEntry_Get( void *pItem )
{
	TIMER_OPERATION_ENTRY	*pTimerEntry;


	DNASSERT( pItem != NULL );

	pTimerEntry = static_cast<TIMER_OPERATION_ENTRY*>( pItem );

	pTimerEntry->Linkage.Initialize();
	DNASSERT( pTimerEntry->pContext == NULL );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::TimerEntry_Release - return timer job entry to pool
//
// Entry:		Pointer to entry
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::TimerEntry_Release"

void	CThreadPool::TimerEntry_Release( void *pItem )
{
	TIMER_OPERATION_ENTRY	*pTimerEntry;


	DNASSERT( pItem != NULL );

	pTimerEntry = static_cast<TIMER_OPERATION_ENTRY*>( pItem );
	pTimerEntry->pContext= NULL;

	DNASSERT( pTimerEntry->Linkage.IsEmpty() != FALSE );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CThreadPool::TimerEntry_Dealloc - deallocate a timer job entry
//
// Entry:		Pointer to entry
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME	"CThreadPool::TimerEntry_Dealloc"

void	CThreadPool::TimerEntry_Dealloc( void *pItem )
{
	TIMER_OPERATION_ENTRY	*pTimerEntry;


	DNASSERT( pItem != NULL );

	//
	// initialize
	//
	pTimerEntry = static_cast<TIMER_OPERATION_ENTRY*>( pItem );

	//
	// return associated poiner to write data
	//
// can't DNASSERT on Linkage because pool manager stomped on it
//	DNASSERT( pEnumEntry->Linkage.IsEmpty() != FALSE );
	DNASSERT( pTimerEntry->pContext == NULL );
}
//**********************************************************************



