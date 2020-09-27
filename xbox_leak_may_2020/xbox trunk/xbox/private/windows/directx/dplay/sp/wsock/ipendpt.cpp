/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       IPEndpoint.cpp
 *  Content:	IP endpoint class
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	01/20/99	jtk		Created
 *	05/12/99	jtk		Derived from modem endpoint class
 *  03/22/20000	jtk		Updated with changes to interface names
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
// Function definitions
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPEndpoint::CIPEndpoint - constructor
//
// Entry:		Nothing
//
// Exit:		Nothing
//
// Notes:	Do not allocate anything in a constructor
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CIPEndpoint::CIPEndpoint"

CIPEndpoint::CIPEndpoint():
	m_pOwningPool( NULL )
{
	m_pRemoteMachineAddress = &m_IPAddress;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPEndpoint::~CIPEndpoint - destructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CIPEndpoint::~CIPEndpoint"

CIPEndpoint::~CIPEndpoint()
{
	DEBUG_ONLY( m_pRemoteMachineAddress = NULL );
	DNASSERT( m_pOwningPool == NULL );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPEndpoint::PoolAllocFunction - function called when item is created in pool
//
// Entry:		Pointer to context
//
// Exit:		Boolean indicating success
//				TRUE = success
//				FALSE = failure
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CIPEndpoint::PoolAllocFunction"

BOOL	CIPEndpoint::PoolAllocFunction( ENDPOINT_POOL_CONTEXT *pContext )
{
	BOOL	fReturn;
	HRESULT	hr;


	DNASSERT( pContext != NULL );
	
	//
	// initialize
	//
	fReturn = TRUE;
	DNASSERT( m_fListenStatusNeedsToBeIndicated == FALSE );
	DNASSERT( GetDisconnectIndicationHandle() == INVALID_HANDLE_VALUE );
	
	hr = Initialize();
	if ( hr != DPN_OK )
	{
		fReturn = FALSE;
		DPF( 0, "Failed to initialize base endpoint!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

Exit:
	return	fReturn;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPEndpoint::PoolInitFunction - function called when item is removed from pool
//
// Entry:		Pointer to context
//
// Exit:		Boolean indicating success
//				TRUE = success
//				FALSE = failure
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CIPEndpoint::PoolInitFunction"

BOOL	CIPEndpoint::PoolInitFunction( ENDPOINT_POOL_CONTEXT *pContext )
{
	BOOL	fReturn;


	DNASSERT( pContext != NULL );
	DNASSERT( pContext->pSPData != NULL );

	//
	// initialize
	//
	fReturn = TRUE;

	DNASSERT( m_pSPData == NULL );
	m_pSPData = pContext->pSPData;
	m_pSPData->ObjectAddRef();

	DNASSERT( m_fListenStatusNeedsToBeIndicated == FALSE );
	DNASSERT( GetDisconnectIndicationHandle() == INVALID_HANDLE_VALUE );
	
	DEBUG_ONLY( m_fInitialized = TRUE );

	return	fReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPEndpoint::PoolReleaseFunction - function called when item is returning
//		to the pool
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CIPEndpoint::PoolReleaseFunction"

void	CIPEndpoint::PoolReleaseFunction( void )
{
	DEBUG_ONLY( DNASSERT( m_fInitialized != FALSE ) );
	DEBUG_ONLY( DNASSERT( m_fEndpointOpen == FALSE ) );

	m_EndpointType = ENDPOINT_TYPE_UNKNOWN;
	m_EnumKey.SetKey( INVALID_ENUM_KEY );

	DNASSERT( m_fConnectSignalled == FALSE );
	DNASSERT( m_State == ENDPOINT_STATE_UNINITIALIZED );
	DNASSERT( m_EndpointType == ENDPOINT_TYPE_UNKNOWN );
	DNASSERT( m_pRemoteMachineAddress != NULL );

	DNASSERT( m_pSPData != NULL );
	m_pSPData->ObjectDecRef();
	m_pSPData = NULL;

	m_pRemoteMachineAddress->Reset();

	DNASSERT( GetSocketPort() == NULL );
	DNASSERT( m_pUserEndpointContext == NULL );
	DNASSERT( GetDisconnectIndicationHandle() == INVALID_HANDLE_VALUE );

	DNASSERT( m_fListenStatusNeedsToBeIndicated == FALSE );
	DEBUG_ONLY( m_fInitialized = FALSE );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPEndpoint::PoolDeallocFunction - function called when item is deallocated
//		from the pool
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CIPEndpoint::PoolDeallocFunction"

void	CIPEndpoint::PoolDeallocFunction( void )
{
	DNASSERT( m_fListenStatusNeedsToBeIndicated == FALSE );
	DNASSERT( GetDisconnectIndicationHandle() == INVALID_HANDLE_VALUE );
	Deinitialize();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPEndpoint::ReturnSelfToPool - return this endpoint to the pool
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CIPEndpoint::ReturnSelfToPool"

void	CIPEndpoint::ReturnSelfToPool( void )
{
	if ( CommandPending() != FALSE )
	{
		CompletePendingCommand( PendingCommandResult() );
	}

	if ( ConnectHasBeenSignalled() != FALSE )
	{
		SignalDisconnect( GetDisconnectIndicationHandle() );
	}
	
	DNASSERT( ConnectHasBeenSignalled() == FALSE );

	SetUserEndpointContext( NULL );
	m_pOwningPool->Release( this );
}
//**********************************************************************

