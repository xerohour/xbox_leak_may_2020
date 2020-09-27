 /*==========================================================================
 *
 *  Copyright (C) 1995 - 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ContextCFPM.h
 *  Content:	fixed pool manager for classes that takes into account contexts
 *
 *  History:
 *   Date		By		Reason
 *   ======		==		======
 *  12-18-97	aarono	Original
 *	11-06-98	ejs		Add custom handler for Release function
 *	04-12-99	jtk		Trimmed unused functions and parameters, added size assert
 *	01-31-2000	jtk		Added code to check for items already being in the pool on Release().
 *	02-08-2000	jtk		Derived from ClassFPM.h
***************************************************************************/

#ifndef __CONTEXT_CLASS_FPM_H__
#define __CONTEXT_CLASS_FPM_H__

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_COMMON


//**********************************************************************
// Constant definitions
//**********************************************************************

#define	CONTEXTCFPM_BLANK_NODE_VALUE	0x5AA5817E

#define	CHECK_FOR_DUPLICATE_CONTEXTCFPM_RELEASE

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

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Class definitions
//**********************************************************************

// class to act as a link in the pool
template< class T >
class	CContextClassFPMPoolNode
{
	STDNEWDELETE

	public:
		CContextClassFPMPoolNode() { m_pNext = NULL; }
		~CContextClassFPMPoolNode() {};

		CContextClassFPMPoolNode	*m_pNext;
		T		m_Item;

	protected:
	private:
};

// class to manage the pool
template< class T, class S >
class	CContextFixedPool
{
	STDNEWDELETE

	public:
		CContextFixedPool();
		~CContextFixedPool();


typedef BOOL (T::*PBOOLCALLBACK)( S *const pContext );
typedef	void (T::*PVOIDCONTEXTCALLBACK)( S *const pContext );
typedef void (T::*PVOIDCALLBACK)( void );

		BOOL	Initialize( PBOOLCALLBACK pAllocFunction, PVOIDCONTEXTCALLBACK pInitFunction, PVOIDCALLBACK pReleaseFunction, PVOIDCALLBACK pDeallocFunction );
		void	Deinitialize( void ) { DEBUG_ONLY( m_fInitialized = FALSE ); }

		T		*Get( S *const pContext );
		void	Release( T *const pItem );

	protected:

	private:
		PBOOLCALLBACK			m_pAllocFunction;
		PVOIDCONTEXTCALLBACK	m_pInitFunction;
		PVOIDCALLBACK			m_pReleaseFunction;
		PVOIDCALLBACK			m_pDeallocFunction;

		CContextClassFPMPoolNode< T >	*m_pPool;		// pointer to list of available elements
		DEBUG_ONLY( BOOL		m_fInitialized );
		DEBUG_ONLY( UINT_PTR	m_uOutstandingItemCount );
};

//**********************************************************************
// Class function definitions
//**********************************************************************


//**********************************************************************
// ------------------------------
// CContextFixedPool::CContextFixedPool - constructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CContextFixedPool::CContextFixedPool"

template< class T, class S >
CContextFixedPool< T, S >::CContextFixedPool():
	m_pAllocFunction( NULL ),
	m_pInitFunction( NULL ),
	m_pReleaseFunction( NULL ),
	m_pDeallocFunction( NULL ),
	m_pPool( NULL )
{
	DEBUG_ONLY( m_uOutstandingItemCount = 0 );
	DEBUG_ONLY( m_fInitialized = FALSE );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CContextFixedPool::~CContextFixedPool - destructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CContextFixedPool::~CContextFixedPool"

template< class T, class S >
CContextFixedPool< T, S >::~CContextFixedPool()
{
	DEBUG_ONLY( DNASSERT( m_uOutstandingItemCount == 0 ) );
	while ( m_pPool != NULL )
	{
		CContextClassFPMPoolNode< T >	*pNode;
		T*	pItem;


		DEBUG_ONLY( DNASSERT( m_fInitialized == FALSE ) );
		pNode = m_pPool;
		m_pPool = m_pPool->m_pNext;
		(pNode->m_Item.*this->m_pDeallocFunction)();
		delete	pNode;
	}

	DEBUG_ONLY( DNASSERT( m_fInitialized == FALSE ) );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CContextFixedPool::Initialize - initialize pool
//
// Entry:		Pointer to function to call when a new entry is allocated
//				Pointer to function to call when a new entry is removed from the pool
//				Pointer to function to call when an entry is returned to the pool
//				Pointer to function to call when an entry is deallocated
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CContextFixedPool::Initialize"

template< class T, class S >
BOOL	CContextFixedPool< T, S >::Initialize( PBOOLCALLBACK pAllocFunction, PVOIDCONTEXTCALLBACK pInitFunction, PVOIDCALLBACK pReleaseFunction, PVOIDCALLBACK pDeallocFunction )
{
	BOOL	fReturn;


	DNASSERT( pAllocFunction != NULL );
	DNASSERT( pInitFunction != NULL );
	DNASSERT( pReleaseFunction != NULL );
	DNASSERT( pDeallocFunction != NULL );

	fReturn = TRUE;
	m_pAllocFunction = pAllocFunction;
	m_pInitFunction = pInitFunction;
	m_pReleaseFunction = pReleaseFunction;
	m_pDeallocFunction = pDeallocFunction;

	DEBUG_ONLY( DNASSERT( m_fInitialized == FALSE ) );
	DEBUG_ONLY( m_fInitialized = TRUE );

	return	fReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CContextFixedPool::Get - get an item from the pool
//
// Entry:		Pointer to user context
//
// Exit:		Pointer to item
//				NULL = out of memory
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CContextFixedPool::Get"

template< class T, class S >
T	*CContextFixedPool< T, S >::Get( S *const pContext )
{
	CContextClassFPMPoolNode< T >	*pNode;
	T	*pReturn;


	DEBUG_ONLY( DNASSERT( m_fInitialized != FALSE ) );

	//
	// initialize
	//
	pReturn = NULL;

	//
	// if the pool is empty, try to allocate a new entry, otherwise grab
	// the first item from the pool
	//
	if ( m_pPool == NULL )
	{
		pNode = new CContextClassFPMPoolNode< T >;
		if ( pNode != NULL )
		{
			if ( (pNode->m_Item.*this->m_pAllocFunction)( pContext ) == FALSE )
			{
				delete pNode;
				pNode = NULL;
			}
		}
	}
	else
	{
		pNode = m_pPool;
		m_pPool = m_pPool->m_pNext;
	}

	//
	// if we have an entry (it was freshly created, or removed from the pool),
	// attempt to initialize it before passing it to the user
	//
	if ( pNode != NULL )
	{
		(pNode->m_Item.*this->m_pInitFunction)( pContext );

		pReturn = &pNode->m_Item;
		DEBUG_ONLY( pNode->m_pNext = (CContextClassFPMPoolNode<T>*) CONTEXTCFPM_BLANK_NODE_VALUE );
		DEBUG_ONLY( m_uOutstandingItemCount++ );
	}

	return	 pReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CContextFixedPool::Release - return item to pool
//
// Entry:		Pointer to item
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CContextFixedPool::Release"

template< class T, class S >
void	CContextFixedPool< T, S >::Release( T *const pItem )
{
	CContextClassFPMPoolNode< T >	*pNode;


	DEBUG_ONLY( DNASSERT( m_fInitialized != FALSE ) );
	DNASSERT( pItem != NULL );
	DBG_CASSERT( sizeof( BYTE* ) == sizeof( pItem ) );
	DBG_CASSERT( sizeof( CContextClassFPMPoolNode< T >* ) == sizeof( BYTE* ) );
	pNode = reinterpret_cast<CContextClassFPMPoolNode< T >*>( &reinterpret_cast<BYTE*>( pItem )[ -OFFSETOF( CContextClassFPMPoolNode< T >, m_Item ) ] );

#if defined(CHECK_FOR_DUPLICATE_CONTEXTCFPM_RELEASE) && defined(DEBUG)
	{
		CContextClassFPMPoolNode< T >	*pTemp;


		pTemp = m_pPool;
		while ( pTemp != NULL )
		{
			DNASSERT( pTemp != pNode );
			pTemp = pTemp->m_pNext;
		}
	}
#endif	// CHECK_FOR_DUPLICATE_CONTEXTCFPM_RELEASE

	DEBUG_ONLY( DNASSERT( pNode->m_pNext == (CContextClassFPMPoolNode< T >*)CONTEXTCFPM_BLANK_NODE_VALUE ) );
	(pNode->m_Item.*this->m_pReleaseFunction)();
	pNode->m_pNext = m_pPool;
	m_pPool = pNode;
	DEBUG_ONLY( m_uOutstandingItemCount-- );
}
//**********************************************************************

#ifdef	__LOCAL_OFFSETOF_DEFINED__
#undef	__LOCAL_OFFSETOF_DEFINED__
#undef	OFFSETOF
#endif	// __LOCAL_OFFSETOF_DEFINED__

#undef DPF_SUBCOMP
#undef DPF_MODNAME

#endif	// __CONTEXT_CLASS_FPM_H__
