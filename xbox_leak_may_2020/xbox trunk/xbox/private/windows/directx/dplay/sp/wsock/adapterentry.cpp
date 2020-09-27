/*==========================================================================
 *
 *  Copyright (C) 2000-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       AdapterEntry.cpp
 *  Content:	Structure used in the list of active sockets
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	08/07/2000	jtk		Derived from IODAta.h
 ***************************************************************************/

#include "wsockspi.h"

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_WSOCK

//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

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
// ------------------------------
// CAdapterEntry::CAdapterEntry - constructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CAdapterEntry::CAdapterEntry"

CAdapterEntry::CAdapterEntry():
// BUGBUG: [mgere] [xbox] Temporarily remove RSIP support.
//	m_pRsip( NULL ),
	m_pOwningPool( NULL )
{
	m_AdapterListLinkage.Initialize();
	m_ActiveSocketPorts.Initialize();
	memset( &m_BaseSocketAddress, 0x00, sizeof( m_BaseSocketAddress ) );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CAdapterEntry::~CAdapterEntry - destructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CAdapterEntry::~CAdapterEntry"

CAdapterEntry::~CAdapterEntry()
{
// BUGBUG: [mgere] [xbox] RSIP Removed
//	DNASSERT( m_pRsip == NULL );
	DNASSERT( m_AdapterListLinkage.IsEmpty() != FALSE );
	DNASSERT( m_ActiveSocketPorts.IsEmpty() != FALSE );
	DNASSERT( m_pOwningPool == NULL );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CAdapterEntry::StartRsip - start Rsip for this adapter
//
// Entry:		Pointer to adapter address
//				Pointer to thread pool
//				Boolean indicating whether this is an Rsip server machine
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CAdapterEntry::StartRsip"

HRESULT	CAdapterEntry::StartRsip( const SOCKADDR *const pAdapterAddress,
								  CThreadPool *const pThreadPool,
								  const BOOL fIsRsipServer )
{
	HRESULT	hr;


	DNASSERT( pAdapterAddress != NULL );
/* BUGBUG: [mgere] [xbox] Removed RSIP support

	//
	// initialize
	//
	hr = DPN_OK;
	if ( RsipModule() == NULL )
	{
		m_pRsip = CreateRsip();
		if ( m_pRsip == NULL )
		{
			hr = DPNERR_OUTOFMEMORY;
			DPF( 0, "Failed to create Rsip object!" );
			goto Failure;
		}

		if ( m_pRsip->Initialize( pThreadPool, pAdapterAddress, fIsRsipServer ) == FALSE )
		{
			hr = DPNERR_NOCONNECTION;
			DPF( 0, "Failed Rsip init!" );
			goto Failure;
		}
	}

Exit:
	return	hr;

Failure:
	if ( m_pRsip != NULL )
	{
		m_pRsip->DecRef();
		m_pRsip = NULL;
	}

	goto Exit;	
*/ return E_FAIL;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CAdapterEntry::PoolAllocFunction - called when item is removed from pool
//
// Entry:		Nothing
//
// Exit:		Boolean indicating success
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CAdapterEntry::PoolAllocFunction"

BOOL	CAdapterEntry::PoolAllocFunction( void )
{
	BOOL	fReturn;


	//
	// initialie
	//
	fReturn = TRUE;

// BUGBUG: [mgere] [xbox] RSIP Removed
//	DNASSERT( RsipModule() == NULL );
	DNASSERT( m_AdapterListLinkage.IsEmpty() != FALSE );
	DNASSERT( m_ActiveSocketPorts.IsEmpty() != FALSE );

	return	fReturn;
}
//**********************************************************************
// ------------------------------
// CAdapterEntry::PoolInitFunction - called when item is removed from pool
//
// Entry:		Nothing
//
// Exit:		Boolean indicating success
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CAdapterEntry::PoolInitFunction"

BOOL	CAdapterEntry::PoolInitFunction( void )
{
	BOOL	fReturn;


	//
	// initialize
	//
	fReturn = TRUE;
	
// BUGBUG: [mgere] [xbox] RSIP Removed
//	DNASSERT( RsipModule() == NULL );
	DNASSERT( m_AdapterListLinkage.IsEmpty() != FALSE );
	DNASSERT( m_ActiveSocketPorts.IsEmpty() != FALSE );

	return	fReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CAdapterEntry::PoolReleaseFunction - called when item is returned to pool
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CAdapterEntry::PoolReleaseFunction"

void	CAdapterEntry::PoolReleaseFunction( void )
{
// BUGBUG: [mgere] [xbox] RSIP Removed
//	DNASSERT( RsipModule() == NULL );
	DNASSERT( m_AdapterListLinkage.IsEmpty() != FALSE );
	DNASSERT( m_ActiveSocketPorts.IsEmpty() != FALSE );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CAdapterEntry::PoolDeallocFunction - called when this item is freed from pool
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CAdapterEntry::PoolDeallocFunction"

void	CAdapterEntry::PoolDeallocFunction( void )
{
// BUGBUG: [mgere] [xbox] RSIP Removed
//	DNASSERT( RsipModule() == NULL );
	DNASSERT( m_AdapterListLinkage.IsEmpty() != FALSE );
	DNASSERT( m_ActiveSocketPorts.IsEmpty() != FALSE );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CAdapterEntry::ReturnSelfToPool - return this item to the pool
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CAdapterEntry::ReturnSelfToPool"

void	CAdapterEntry::ReturnSelfToPool( void )
{
/* BUGBUG: [mgere] [xbox] Remove RSIP support
	if ( m_pRsip != NULL )
	{
		m_pRsip->Deinitialize();
		m_pRsip->DecRef();
		m_pRsip = NULL;
	}
*/

	DNASSERT( m_pOwningPool != NULL );
	m_pOwningPool->Release( this );
}
//**********************************************************************

