/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ThreadPool.h
 *  Content:	Functions to manage a thread pool
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	03/01/99	jtk		Derived from Utils.h
 ***************************************************************************/

#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_WSOCK

//**********************************************************************
// Constant definitions
//**********************************************************************

//
// max handles that can be waited on for Win9x
//
#define	MAX_WIN9X_HANDLE_COUNT	64

//
// job definitions
//
typedef enum	_JOB_TYPE
{
	JOB_UNINITIALIZED,			// uninitialized value
	JOB_DELAYED_COMMAND,		// callback provided
	JOB_REFRESH_TIMER_JOBS		// revisit timer jobs
} JOB_TYPE;

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

//
// forward class and structure references
//
class	CSocketPort;
class	CThreadPool;
typedef struct	_THREAD_POOL_JOB		THREAD_POOL_JOB;
typedef	struct	_TIMER_OPERATION_ENTRY	TIMER_OPERATION_ENTRY;
typedef	struct	_WIN9X_CORE_DATA		WIN9X_CORE_DATA;

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

typedef	BOOL	(CSocketPort::*PSOCKET_SERVICE_FUNCTION)( void );
typedef	void	JOB_FUNCTION( THREAD_POOL_JOB *const pJobInfo );
typedef	void	TIMER_EVENT_CALLBACK( void *const pContext );
typedef	void	TIMER_EVENT_COMPLETE( const HRESULT hCompletionCode, void *const pContext );

//**********************************************************************
// Class prototypes
//**********************************************************************

//
// class for thread pool
//
class	CThreadPool
{
	STDNEWDELETE

	public:
		CThreadPool();
		~CThreadPool();

		void	Lock( void ) { DNEnterCriticalSection( &m_Lock ); }
		void	Unlock( void ) { DNLeaveCriticalSection( &m_Lock ); }
		void	LockReadData( void ) { DNEnterCriticalSection( &m_ReadDataLock ); }
		void	UnlockReadData( void ) { DNLeaveCriticalSection( &m_ReadDataLock ); }
		void	LockWriteData( void ) { DNEnterCriticalSection( &m_WriteDataLock ); }
		void	UnlockWriteData( void ) { DNLeaveCriticalSection( &m_WriteDataLock ); }


		void	AddRef( void ) { DNInterlockedIncrement( &m_iRefCount ); }
		void	DecRef( void )
		{
			if ( DNInterlockedDecrement( &m_iRefCount ) == 0 )
			{
				ReturnSelfToPool();
			}
		}

		HRESULT	Initialize( void );
		void	Deinitialize( void );
		void	StopAllIO( void );

		CReadIOData	*GetNewReadIOData( READ_IO_DATA_POOL_CONTEXT *const pContext );
		void	ReturnReadIOData( CReadIOData *const pReadIOData );

		#undef DPF_MODNAME
		#define DPF_MODNAME "CThreadPool::GetNewWriteIOData"
		CWriteIOData	*GetNewWriteIOData( WRITE_IO_DATA_POOL_CONTEXT *const pContext )
		{
			CWriteIOData   *pTemp;


			DNASSERT( pContext != NULL );
			pContext->hOverlapEvent = GetWinsock2SendCompleteEvent();

			LockWriteData();

			//
			// attempt to get an entry from the pool, if one is gotten, put into
			// the pending write list
			//
			pTemp = m_WriteIODataPool.Get( pContext );
			if ( pTemp != NULL )
			{
				pTemp->m_OutstandingWriteListLinkage.InsertBefore( &m_OutstandingWriteList );
			}

			UnlockWriteData();

			return pTemp;
		}

		#undef DPF_MODNAME
		#define DPF_MODNAME "CThreadPool::ReturnWriteIOData"
		void	ReturnWriteIOData( CWriteIOData *const pWriteData )
		{
			DNASSERT( pWriteData != NULL );

			LockWriteData();

			pWriteData->m_OutstandingWriteListLinkage.RemoveFromList();
			DNASSERT( pWriteData->m_OutstandingWriteListLinkage.IsEmpty() != FALSE );
			m_WriteIODataPool.Release( pWriteData );

			UnlockWriteData();
		}

		#undef DPF_MODNAME
		#define DPF_MODNAME "CThreadPool::GetWinsock2SendCompleteEvent"
		HANDLE	GetWinsock2SendCompleteEvent( void ) const
		{
			DNASSERT( m_hWinsock2SendComplete != NULL );
			return m_hWinsock2SendComplete;
		}

		#undef DPF_MODNAME
		#define DPF_MODNAME "CThreadPool::GetWinsock2ReceiveCompleteEvent"
		HANDLE	GetWinsock2ReceiveCompleteEvent( void ) const
		{
			DNASSERT( m_hWinsock2ReceiveComplete != NULL );
			return m_hWinsock2ReceiveComplete;
		}


		HRESULT	SubmitDelayedCommand( JOB_FUNCTION *const pFunction,
									  JOB_FUNCTION *const pCancelFunction,
									  void *const pContext );

		HRESULT	AddSocketPort( CSocketPort *const pSocketPort );
		void	RemoveSocketPort( CSocketPort *const pSocketPort );

		HRESULT	SubmitTimerJob( const UINT_PTR uRetryCount,
								const BOOL fRetryForever,
								const DN_TIME RetryInterval,
								const BOOL fIdleWaitForever,
								const DN_TIME IdleTimeout,
								TIMER_EVENT_CALLBACK *const pTimerCallbackFunction,
								TIMER_EVENT_COMPLETE *const pTimerCompleteFunction,
								void *const pContext );
		
		void	StopTimerJob( void *const pContext, const HRESULT hCommandResult );

#ifdef DPLAY_DOWORK
		DWORD PrimaryWin9xThread( void );
#endif

	protected:

	private:
		DNCRITICAL_SECTION	m_Lock;				// local lock

		volatile LONG	m_iRefCount;					// reference count

		HANDLE	m_hStopAllThreads;				// handle used to stop all non-I/O completion threads
		HANDLE	m_hWinsock2SendComplete;		// send complete on Winsock2
		HANDLE	m_hWinsock2ReceiveComplete;		// receive complete on Winsock2
		HANDLE	m_hThreadCloseEvent;		// Signal that the thread has been closed


		//
		// list of pending network operations, it doesn't really matter if they're
		// reads or writes, they're just pending.
		//
		DNCRITICAL_SECTION	m_ReadDataLock;		// lock for all read data
		CContextFixedPool< CReadIOData, READ_IO_DATA_POOL_CONTEXT >	m_IPReadIODataPool;		// pool for IP read data
		CBilink		m_OutstandingReadList;		// list of outstanding reads

		DNCRITICAL_SECTION	m_WriteDataLock;	// lock for all write data
		CContextFixedPool< CWriteIOData, WRITE_IO_DATA_POOL_CONTEXT >	m_WriteIODataPool;	// pool for write data
		CBilink		m_OutstandingWriteList;		// list of outstanding writes

		//
		// The Job data lock covers all items through and including m_pSocketPorts
		//
		DNCRITICAL_SECTION	m_JobDataLock;		// lock for job queue/pool

		FPOOL		m_TimerEntryPool;			// pool for timed entries
		FPOOL		m_JobPool;					// pool of jobs for work threads

		CJobQueue	m_JobQueue;					// job queue

		//
		// Data used by the the timer thread.  This data is protected by m_TimerDataLock.
		// This data is cleaned by the timer thread.  Since only one timer thread
		// is allowed to run at any given time, the status of the NT timer thread
		// can be determined by m_fNTEnumThreadRunning.  Win9x doesn't have a timer
		// thread, the main thread loop is timed.
		//
		DNCRITICAL_SECTION	m_TimerDataLock;
		CBilink				m_TimerJobList;

		UINT_PTR		m_uReservedSocketCount;			// count of sockets that are 'reserved' for use
		FD_SET			m_SocketSet;					// set of all sockets in use
		CSocketPort 	*m_pSocketPorts[ FD_SETSIZE ];	// set of corresponding socket ports

		void	LockJobData( void ) { DNEnterCriticalSection( &m_JobDataLock ); }
		void	UnlockJobData( void ) { DNLeaveCriticalSection( &m_JobDataLock ); }

		void	LockTimerData( void ) { DNEnterCriticalSection( &m_TimerDataLock ); }
		void	UnlockTimerData( void ) { DNLeaveCriticalSection( &m_TimerDataLock ); }

		static	UINT_PTR	SaturatedWaitTime( const DN_TIME &Time )
		{
				UINT_PTR	uReturn;

				if ( Time.Time32.TimeHigh != 0 )
				{
					uReturn = -1;
				}
				else
				{
					uReturn = Time.Time32.TimeLow;
				}

				return	uReturn;
		}

		HRESULT	Win9xInit( void );

		BOOL	ProcessTimerJobs( const CBilink *const pJobList, DN_TIME *const pNextJobTime);

		BOOL	ProcessTimedOperation( TIMER_OPERATION_ENTRY *const pJob,
									   const DN_TIME *const pCurrentTime,
									   DN_TIME *const pNextJobTime );

		void	RemoveTimerOperationEntry( TIMER_OPERATION_ENTRY *const pTimerOperationData, const HRESULT hReturnCode );
		void	CompleteOutstandingSends( void );
		void	CompleteOutstandingReceives( void );

#ifndef DPLAY_DOWORK
		static	DWORD WINAPI	PrimaryWin9xThread( void *pParam );
#endif

		HRESULT	SubmitWorkItem( THREAD_POOL_JOB *const pJob );
		THREAD_POOL_JOB	*GetWorkItem( void );

		static	void	CancelRefreshTimerJobs( THREAD_POOL_JOB *const pJobData );
		void	ProcessWin9xEvents( WIN9X_CORE_DATA *const pCoreData );
		void	ProcessWin9xJob( WIN9X_CORE_DATA *const pCoreData );

		void	StopAllThreads( void );
		void	CancelOutstandingJobs( void );
		void	CancelOutstandingIO( void );
		void	ReturnSelfToPool( void );

		//
		// functions for managing the job pool
		//
		static	BOOL	WorkThreadJob_Alloc( void *pItem );
		static	void	WorkThreadJob_Get( void *pItem );
		static	void	WorkThreadJob_Release( void *pItem );
		static	void	WorkThreadJob_Dealloc( void *pItem );

		//
		// functions for managing the timer data pool
		//
		static	BOOL	TimerEntry_Alloc( void *pItem );
		static	void	TimerEntry_Get( void *pItem );
		static	void	TimerEntry_Release( void *pItem );
		static	void	TimerEntry_Dealloc( void *pItem );

		//
		// prevent unwarranted copies
		//
		CThreadPool( const CThreadPool & );
		CThreadPool& operator=( const CThreadPool & );
};

#undef DPF_MODNAME

#endif	// __THREAD_POOL_H__
