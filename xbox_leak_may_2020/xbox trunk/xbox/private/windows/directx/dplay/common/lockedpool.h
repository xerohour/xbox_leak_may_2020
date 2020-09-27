 /*==========================================================================
 *
 *  Copyright (C) 1995 - 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       LockedPool.h
 *  Content:	Pool manager for classes
 *
 *  History:
 *   Date		By			Reason
 *   ======		==			======
 *  12-18-97	aarono		Original
 *	11-06-98	ejs			Add custom handler for Release function
 *	04-12-99	johnkan		Trimmed unused functions and parameters, added size assert
 *	01-31-2000	johnkan		Added code to check for items already being in the pool on Release().
 *	02-08-2000	johnkan		Derived from ClassFPM.h
 *	03-26-2000	johnkan		Renamed to avoid collisions with other classes
 *	04-06-2000	johnkan		Modified to have a base class to derive pool items from
***************************************************************************/

#ifndef __LOCKED_POOL__
#define __LOCKED_POOL__

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_COMMON


//**********************************************************************
// Constant definitions
//**********************************************************************

#define	CHECK_FOR_DUPLICATE_LOCKED_POOL_RELEASE

//**********************************************************************
// Macro definitions
//**********************************************************************

#ifndef	OFFSETOF
// Macro to compute the offset of an element inside of a larger structure (copied from MSDEV's STDLIB.H)
#define OFFSETOF(s,m)	( (INT_PTR) &(((s *)0)->m) )
#define	__LOCAL_OFFSETOF_DEFINED__
#endif	// OFFSETOF

//**********************************************************************
// Structure definitions
//**********************************************************************

//
// forward reference
//
class	CLockedPoolItem;
template< class T > class	CLockedPool;

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Class definitions
//**********************************************************************

//
// class to act as a link in the pool
//
class	CLockedPoolItem
{
	STDNEWDELETE

	public:
		#undef DPF_MODNAME
		#define DPF_MODNAME "CLockedPoolItem::CLockedPoolItem"
		CLockedPoolItem()
		{
			m_iRefCount = 0;
			m_pNext = NULL;
		}
		
		#undef DPF_MODNAME
		#define DPF_MODNAME "CLockedPoolItem::~CLockedPoolItem"
		virtual	~CLockedPoolItem() { DNASSERT( m_iRefCount == 0 ); }

		#undef DPF_MODNAME
		#define DPF_MODNAME "CLockedPoolItem::AddRef"
		void	AddRef( void )
		{
			DNASSERT( m_iRefCount != -1 );
			InterlockedIncrement( const_cast<LONG*>( &m_iRefCount ) );
		}
		
		#undef DPF_MODNAME
		#define DPF_MODNAME "CLockedPoolItem::DecRef"
		void	DecRef( void )
		{
			DNASSERT( m_iRefCount != 0 );
			if ( InterlockedDecrement( const_cast<LONG*>( &m_iRefCount ) ) == 0 )
			{
				ReturnSelfToPool();
			}
		}

		CLockedPoolItem	*GetNext( void ) const { return m_pNext; }
		void	InvalidateNext( void ) { m_pNext = NULL; }

		#undef DPF_MODNAME
		#define DPF_MODNAME "CLockedPoolItem::LinkToPool"
		void	LinkToPool( CLockedPoolItem *volatile *const ppPoolItems )
		{
			DNASSERT( ppPoolItems != NULL );
			m_pNext = *ppPoolItems;
			*ppPoolItems = this;
		}

		//
		// Default initialization and deinitialization functions.  These can
		// be overridden by the derived classes.
		//
		virtual	BOOL	PoolAllocFunction( void ){ return TRUE; }
		virtual	BOOL	PoolInitFunction( void ){ return TRUE; }
		virtual void	PoolReleaseFunction( void ){};
		virtual void	PoolDeallocFunction( void ){};

	protected:
	
	private:
		//
		// reference count used to return this item to the pool
		//
		volatile LONG	m_iRefCount;	

		//
		// pointer used to link this item to the rest of the pool
		//
		CLockedPoolItem		*m_pNext;

		virtual void	ReturnSelfToPool( void ) = 0;

		//
		// prevent unwarranted copies
		//
		CLockedPoolItem( const CLockedPoolItem & );
		CLockedPoolItem& operator=( const CLockedPoolItem & );
};


//
// class to manage the pool
//
template< class T >
class	CLockedPool
{
	STDNEWDELETE

	public:
		CLockedPool();
		~CLockedPool();

		void	Lock( void ) { DNEnterCriticalSection( &m_Lock ); }
		void	Unlock( void ) { DNLeaveCriticalSection( &m_Lock ); }

		BOOL	Initialize( void );
		void	Deinitialize( void );

		T		*Get( void );
		void	Release( T *const pItem );

	protected:

	private:
		DNCRITICAL_SECTION	m_Lock;

		CLockedPoolItem	*volatile m_pPool;		// pointer to list of available elements

		DEBUG_ONLY( BOOL			m_fInitialized );
		DEBUG_ONLY( volatile LONG	m_lOutstandingItemCount );

		T	*RemoveNode( void )
		{
			T	*pReturn;


			if ( m_pPool != NULL )
			{
				pReturn = static_cast<T*>( m_pPool );
				m_pPool = m_pPool->GetNext();
				DEBUG_ONLY( pReturn->InvalidateNext() );
			}
			else
			{
				pReturn = NULL;
			}

			return	pReturn;
		}
		
		//
		// prevent unwarranted copies
		//
		CLockedPool< T >( const CLockedPool< T > & );
		CLockedPool< T >& operator=( const CLockedPool< T > & );
};


//**********************************************************************
// Class function definitions
//**********************************************************************


//**********************************************************************
// ------------------------------
// CLockedPool::CLockedPool - constructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CLockedPool::CLockedPool"

template< class T >
CLockedPool< T >::CLockedPool():
	m_pPool( NULL )
{
	DEBUG_ONLY( m_fInitialized = FALSE );
	DEBUG_ONLY( m_lOutstandingItemCount = 0 );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CLockedPool::~CLockedPool - destructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CLockedPool::~CLockedPool"

template< class T >
CLockedPool< T >::~CLockedPool()
{
	DEBUG_ONLY( DNASSERT( m_lOutstandingItemCount == 0 ) );
	DEBUG_ONLY( DNASSERT( m_fInitialized == FALSE ) );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CLockedPool::Initialize - initialize pool
//
// Entry:		Nothing
//				Pointer to function to call when a new entry is removed from the pool
//				Pointer to function to call when an entry is returned to the pool
//				Pointer to function to call when an entry is deallocated
//
// Exit:		Boolean indicating success
//				TRUE = initialization succeeded
//				FALSE = initialization failed
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CLockedPool::Initialize"

template< class T >
BOOL	CLockedPool< T >::Initialize( void )
{
	BOOL	fReturn;

	
	DEBUG_ONLY( DNASSERT( m_fInitialized == FALSE ) );

	fReturn = TRUE;

	if ( DNInitializeCriticalSection( &m_Lock ) == FALSE )
	{
		fReturn = FALSE;
		goto Exit;
	}
	DebugSetCriticalSectionRecursionCount( &m_Lock, 0 );

	DEBUG_ONLY( m_fInitialized = TRUE );

Exit:
	return	fReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CLockedPool::Deinitialize - deinitialize pool
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CLockedPool::Deinitialize"

template< class T >
void	CLockedPool< T >::Deinitialize( void )
{
	DEBUG_ONLY( DNASSERT( m_fInitialized != FALSE ) );

	Lock();

	DEBUG_ONLY( DNASSERT( m_lOutstandingItemCount == 0 ) );
	while ( m_pPool != NULL )
	{
		CLockedPoolItem	*pNode;

		
		pNode = RemoveNode();
		pNode->PoolDeallocFunction();
		delete	pNode;
	}
	
	Unlock();

	DNDeleteCriticalSection( &m_Lock );

	DEBUG_ONLY( m_fInitialized = FALSE );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CLockedPool::Get - get an item from the pool
//
// Entry:		Nothing
//
// Exit:		Pointer to item
//				NULL = out of memory
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CLockedPool::Get"

template< class T >
T	*CLockedPool< T >::Get( void )
{
	T	*pReturn;


	DEBUG_ONLY( DNASSERT( m_fInitialized != FALSE ) );

	//
	// initialize
	//
	pReturn = NULL;

	Lock();

	//
	// if the pool is empty, try to allocate a new entry, otherwise grab
	// the first item from the pool
	//
	if ( m_pPool == NULL )
	{
		Unlock();
		
		pReturn = new T;
		if ( pReturn != NULL )
		{
			if ( pReturn->PoolAllocFunction() == FALSE )
			{
				delete pReturn;
				pReturn = NULL;
			}
		}
	}
	else
	{
		pReturn = RemoveNode();
		Unlock();
	}


	//
	// if we have an entry (it was freshly created, or removed from the pool),
	// attempt to initialize it before passing it to the user
	//
	if ( pReturn != NULL )
	{
		if ( pReturn->PoolInitFunction() == FALSE )
		{
			Lock();
			
			pReturn->LinkToPool( &m_pPool );
			
			Unlock();
			
			pReturn = NULL;
		}
		else
		{
			pReturn->SetOwningPool( this );
			pReturn->AddRef();
			DEBUG_ONLY( InterlockedIncrement( const_cast<LONG*>( &m_lOutstandingItemCount ) ) );
		}
	}

	return	 pReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CLockedPool::Release - return item to pool
//
// Entry:		Pointer to item
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CLockedPool::Release"

template< class T >
void	CLockedPool< T >::Release( T *const pItem )
{
	DEBUG_ONLY( DNASSERT( m_fInitialized != FALSE ) );
	DNASSERT( pItem != NULL );

	
	DEBUG_ONLY( DNASSERT( pItem->GetNext() == NULL ) );
	pItem->PoolReleaseFunction();
	
	Lock();
	
#if defined(CHECK_FOR_DUPLICATE_LOCKED_POOL_RELEASE) && defined(DEBUG)
	{
		CLockedPoolItem	*pTemp;


		pTemp = m_pPool;
		while ( pTemp != NULL )
		{
			DNASSERT( pTemp != pItem );
			pTemp = pTemp->GetNext();
		}
	}
#endif	// CHECK_FOR_DUPLICATE_LOCKED_POOL_RELEASE

	DEBUG_ONLY( pItem->SetOwningPool( NULL ) );
	pItem->LinkToPool( &m_pPool );
	Unlock();
	
	DEBUG_ONLY( InterlockedDecrement( const_cast<LONG*>( &m_lOutstandingItemCount ) ) );
}
//**********************************************************************

#ifdef	__LOCAL_OFFSETOF_DEFINED__
#undef	__LOCAL_OFFSETOF_DEFINED__
#undef	OFFSETOF
#endif	// __LOCAL_OFFSETOF_DEFINED__


#undef DPF_SUBCOMP
#undef DPF_MODNAME

#endif	// __LOCKED_POOL__
