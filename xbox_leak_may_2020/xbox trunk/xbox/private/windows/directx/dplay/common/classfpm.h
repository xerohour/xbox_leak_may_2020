 /*==========================================================================
 *
 *  Copyright (C) 1995 - 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ClassFPM.h
 *  Content:	fixed size pool manager for classes
 *
 *  History:
 *   Date		By		Reason
 *   ======		==		======
 *  12-18-97	aarono	Original
 *	11-06-98	ejs		Add custom handler for Release function
 *	04-12-99	jtk		Trimmed unused functions and parameters, added size assert
 *	01-31-2000	jtk		Added code to check for items already being in the pool on Release().
***************************************************************************/

#ifndef __CLASS_FPM_H__
#define __CLASS_FPM_H__

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_COMMON

//**********************************************************************
// Constant definitions
//**********************************************************************

#define	CLASSFPM_BLANK_NODE_VALUE	0x55AA817E

#define	CHECK_FOR_DUPLICATE_CLASSFPM_RELEASE

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
class	CPoolNode
{
	STDNEWDELETE

	public:
		CPoolNode() { m_pNext = NULL; }
		~CPoolNode() {};

		T			m_Item;
		CPoolNode	*m_pNext;

	protected:
	private:
};

// class to manage the pool
template< class T >
class	CFixedPool
{
	STDNEWDELETE

	public:
		CFixedPool();
		~CFixedPool();

		T		*Get( void );
		void	Release( T *const pItem );

	protected:

	private:
		CPoolNode< T >		*m_pPool;		// pointer to list of available elements
		DEBUG_ONLY( UINT_PTR	m_uOutstandingItemCount );
};

//**********************************************************************
// Class function definitions
//**********************************************************************


//**********************************************************************
// ------------------------------
// CFixedPool::Get - get an item from the pool
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CFixedPool::CFixedPool"

template< class T >
CFixedPool< T >::CFixedPool()
{
	m_pPool = NULL;
	DEBUG_ONLY( m_uOutstandingItemCount = 0 );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CFixedPool::~CFixedPool - destructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CFixedPool::~CFixedPool"

template< class T >
CFixedPool< T >::~CFixedPool()
{
	DEBUG_ONLY( DNASSERT( m_uOutstandingItemCount == 0 ) );
	while ( m_pPool != NULL )
	{
		CPoolNode< T >	*pTemp;

		pTemp = m_pPool;
		m_pPool = m_pPool->m_pNext;
		delete	pTemp;
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CFixedPool::Get - get an item from the pool
//
// Entry:		Nothing
//
// Exit:		Pointer to item
//				NULL = out of memory
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CFixedPool::Get"

template< class T >
T	*CFixedPool< T >::Get( void )
{
	CPoolNode< T >	*pNode;
	T	*pReturn;


	// initialize
	pReturn = NULL;

	// is the pool empty?
	if ( m_pPool == NULL )
	{
		// try to create a new entry
		pNode = new CPoolNode< T >;
	}
	else
	{
		// grab first item from the pool
		pNode = m_pPool;
		m_pPool = m_pPool->m_pNext;
	}

	if ( pNode != NULL )
	{
		DEBUG_ONLY( pNode->m_pNext = (CPoolNode<T>*) CLASSFPM_BLANK_NODE_VALUE );
		pReturn = &pNode->m_Item;
		DEBUG_ONLY( m_uOutstandingItemCount++ );
	}

	return	 pReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CFixedPool::Release - return item to pool
//
// Entry:		Pointer to item
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CFixedPool::Release"

template< class T >
void	CFixedPool< T >::Release( T *const pItem )
{
	CPoolNode< T >	*pNode;


	DNASSERT( pItem != NULL );
	DBG_CASSERT( sizeof( BYTE* ) == sizeof( pItem ) );
	DBG_CASSERT( sizeof( CPoolNode< T >* ) == sizeof( BYTE* ) );
	pNode = reinterpret_cast<CPoolNode< T >*>( &reinterpret_cast<BYTE*>( pItem )[ -OFFSETOF( CPoolNode< T >, m_Item ) ] );

#if defined(CHECK_FOR_DUPLICATE_CLASSFPM_RELEASE) && defined(DEBUG)
	{
		CPoolNode< T >	*pTemp;


		pTemp = m_pPool;
		while ( pTemp != NULL )
		{
			DNASSERT( pTemp != pNode );
			pTemp = pTemp->m_pNext;
		}
	}
#endif	// CHECK_FOR_DUPLICATE_CLASSFPM_RELEASE

	DEBUG_ONLY( DNASSERT( pNode->m_pNext == (CPoolNode< T >*)CLASSFPM_BLANK_NODE_VALUE ) );
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

#endif	// __CLASS_FPM_H__
