/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Endpoint.cpp
 *  Content:	Winsock endpoint base class
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	01/20/99	jtk		Created
 *	05/12/99	jtk		Derived from modem endpoint class
 *  01/10/20000	rmt		Updated to build with Millenium build process
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
// CEndpoint::CEndpoint - constructor
//
// Entry:		Nothing
//
// Exit:		Nothing
//
// Notes:	Do not allocate anything in a constructor
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::CEndpoint"

CEndpoint::CEndpoint():
	m_State( ENDPOINT_STATE_UNINITIALIZED ),
	m_fConnectSignalled( FALSE ),
	m_EndpointType( ENDPOINT_TYPE_UNKNOWN ),
	m_pRemoteMachineAddress( NULL ),
	m_pSPData( NULL ),
	m_pSocketPort( NULL ),
	m_pUserEndpointContext( NULL ),
	m_fListenStatusNeedsToBeIndicated( FALSE ),
	m_Handle( INVALID_HANDLE_VALUE ),
	m_lCommandRefCount( 0 ),
	m_fCommandPending( FALSE ),
	m_hDisconnectIndicationHandle( INVALID_HANDLE_VALUE ),
	m_pActiveCommandData( NULL ),
	m_dwEnumSendIndex( 0 )
{
	DEBUG_ONLY( m_fInitialized = FALSE );
	DEBUG_ONLY( m_fEndpointOpen = FALSE );
	memset( &m_PendingCommandData, 0x00, sizeof( m_PendingCommandData ) );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::~CEndpoint - destructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::~CEndpoint"

CEndpoint::~CEndpoint()
{
	DNASSERT( m_State == ENDPOINT_STATE_UNINITIALIZED );
	DNASSERT( m_fConnectSignalled == FALSE );
	DNASSERT( m_EndpointType == ENDPOINT_TYPE_UNKNOWN );
	DNASSERT( m_pRemoteMachineAddress == NULL );
	DNASSERT( m_pSPData == NULL );
	DNASSERT( m_pSocketPort == NULL );
	DNASSERT( m_pUserEndpointContext == NULL );
	DNASSERT( m_fListenStatusNeedsToBeIndicated == FALSE );
	DNASSERT( m_Handle == INVALID_HANDLE_VALUE );
	DNASSERT( m_lCommandRefCount == 0 );
	DNASSERT( m_fCommandPending == FALSE );
	DNASSERT( GetDisconnectIndicationHandle() == INVALID_HANDLE_VALUE );
	DNASSERT( m_pActiveCommandData == NULL );
	DNASSERT( m_dwEnumSendIndex == 0 );

	DNASSERT( m_EnumKey.GetKey() == INVALID_ENUM_KEY );
	DNASSERT( m_fEndpointOpen == FALSE );
	DNASSERT( m_fInitialized == FALSE );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::Initialize - initialize an endpoint
//
// Entry:		Nothing
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::Initialize"

HRESULT	CEndpoint::Initialize( void )
{
	HRESULT	hr;


	DNASSERT( m_pSPData == NULL );
	DNASSERT( m_fInitialized == FALSE );

	//
	// initialize
	//
	hr = DPN_OK;

	//
	// attempt to initialize the internal critical section
	//
	if ( DNInitializeCriticalSection( &m_Lock )	== FALSE )
	{
		DPF( 0, "Problem initializing critical section for this endpoint!" );
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}
	DebugSetCriticalSectionRecursionCount( &m_Lock, 0 );

Exit:
	return	hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::Deinitilize - deinitialize an endpoint
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::Deinitialize"

void	CEndpoint::Deinitialize( void )
{
	DNDeleteCriticalSection( &m_Lock );
	DNASSERT( m_pSPData == NULL );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::Open - open endpoint for use
//
// Entry:		Type of endpoint
//				Pointer to address to of remote machine
//				Pointer to socket address of remote machine
//
// Exit:		Nothing
//
// Note:	Any call to Open() will require an associated call to Close().
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::Open"

HRESULT	CEndpoint::Open( const ENDPOINT_TYPE EndpointType,
						 IDirectPlay8Address *const pDP8Address,
						 const CSocketAddress *const pSocketAddress
						 )
{
	HRESULT	hr;


//	DNASSERT( pSocketPort != NULL );
	DNASSERT( ( pDP8Address != NULL ) ||
			  ( pSocketAddress != NULL ) ||
			  ( ( EndpointType == ENDPOINT_TYPE_LISTEN ) ||
				( EndpointType == ENDPOINT_TYPE_MULTICAST ) )
			  );
	DNASSERT( m_fInitialized != FALSE );
	DNASSERT( m_fEndpointOpen == FALSE );

	//
	// initialize
	//
	hr = DPN_OK;
	DEBUG_ONLY( m_fEndpointOpen = TRUE );

//	DNASSERT( m_pSocketPort == NULL );
//	m_pSocketPort = pSocketPort;
//	pSocketPort->EndpointAddRef();

	DNASSERT( m_EndpointType == ENDPOINT_TYPE_UNKNOWN );
	m_EndpointType = EndpointType;

	//
	// determine the endpoint type so we know how to handle the input paramters
	//
	switch ( EndpointType )
	{
		case ENDPOINT_TYPE_ENUM:
		{
//			m_EnumKey.SetKey( pSocketPort->GetEnumKey() );
			//
			// NOTE: THIS CASE FALLS THROUGH!!
			//
		}

		//
		// standard endpoint creation, attempt to parse the input address
		//
		case ENDPOINT_TYPE_CONNECT:
		{
			DNASSERT( pSocketAddress == NULL );
			DNASSERT( pDP8Address != NULL );
			DNASSERT( m_pRemoteMachineAddress != NULL );
			hr = m_pRemoteMachineAddress->SocketAddressFromDP8Address( pDP8Address, SP_ADDRESS_TYPE_HOST );
			if ( hr != DPN_OK )
			{
				DPF( 0, "Problem converting DNAddress to IP address in Initialize!" );
				DisplayDNError( 0, hr );
				goto Failure;
			}

			break;
		}

		//
		// listen, there should be no input DNAddress
		//
		case ENDPOINT_TYPE_LISTEN:
		{
			DNASSERT( pSocketAddress == NULL );
			DNASSERT( pDP8Address == NULL );
			DNASSERT( m_pRemoteMachineAddress != NULL );

			break;
		}

		//
		// new endpoint spawned from a listen, copy the input address and
		// note that this endpoint is really just a connection
		//
		case ENDPOINT_TYPE_CONNECT_ON_LISTEN:
		{
			DNASSERT( pSocketAddress != NULL );
			DNASSERT( pDP8Address == NULL );
			DNASSERT( m_pRemoteMachineAddress != NULL );
			m_pRemoteMachineAddress->CopyAddressSettings( *pSocketAddress );
			m_EndpointType = ENDPOINT_TYPE_CONNECT;
			m_State = ENDPOINT_STATE_ATTEMPTING_CONNECT;

			break;
		}

		//
		// endpoint is being created for multicasts
		//
		case ENDPOINT_TYPE_MULTICAST:
		{
			DNASSERT( pSocketAddress != NULL );
			DNASSERT( pDP8Address == NULL );
			DNASSERT( m_pRemoteMachineAddress != NULL );
			m_pRemoteMachineAddress->CopyAddressSettings( *pSocketAddress );
			m_EndpointType = ENDPOINT_TYPE_MULTICAST;
			m_State = ENDPOINT_STATE_MULTICAST;

			break;
		}

		//
		// unknown type
		//
		default:
		{
			DNASSERT( FALSE );
			break;

		}
	}

Exit:
	return hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::Close - close an endpoint
//
// Entry:		Error code for active command
//
// Exit:		Error code
//
// Note:	This code does not disconnect an endpoint from its associated
//			socket port.  That is the responsibility of the code that is
//			calling this function.  This function assumes that this endpoint
//			is locked.
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::Close"

void	CEndpoint::Close( const HRESULT hActiveCommandResult )
{
	AssertCriticalSectionIsTakenByThisThread( &m_Lock, FALSE );

	DNASSERT( m_fEndpointOpen != FALSE );
	DNASSERT( m_fInitialized != FALSE );

	//
	// is there an active command?
	//
	if ( CommandPending() != FALSE )
	{
		m_hPendingCommandResult = hActiveCommandResult;
	}

	DEBUG_ONLY( m_fEndpointOpen = FALSE );

	return;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::ChangeLoopbackAlias - change the loopback alias to a real address
//
// Entry:		Pointer to real address to use
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::ChangeLoopbackAlias"

void	CEndpoint::ChangeLoopbackAlias( const CSocketAddress *const pSocketAddress ) const
{
	DNASSERT( m_pRemoteMachineAddress != NULL );
	m_pRemoteMachineAddress->ChangeLoopBackToLocalAddress( pSocketAddress );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::CopyConnectData - copy data for connect command
//
// Entry:		Pointer to job information
//				Pointer to device address
//
// Exit:		Nothing
//
// Note:	Since we've already initialized the local adapter, and we've either
//			completely parsed the host address (or are about to display a dialog
//			asking for more information), the address information doesn't need
//			to be copied.
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::CopyConnectData"

void	CEndpoint::CopyConnectData( const SPCONNECTDATA *const pConnectData, IDirectPlay8Address *const pDeviceAddress )
{
	DNASSERT( pConnectData != NULL );
	DNASSERT( pDeviceAddress != NULL );

	DNASSERT( pConnectData->hCommand != NULL );
	DNASSERT( pConnectData->dwCommandDescriptor != NULL_DESCRIPTOR );
	DNASSERT( m_fCommandPending == FALSE );
	DNASSERT( m_pActiveCommandData == FALSE );

	DBG_CASSERT( sizeof( m_PendingCommandData.ConnectData ) == sizeof( *pConnectData ) );
	memcpy( &m_PendingCommandData, pConnectData, sizeof( m_PendingCommandData.ConnectData ) );
	m_PendingCommandData.ConnectData.pAddressHost = NULL;
	m_PendingCommandData.ConnectData.pAddressDeviceInfo = pDeviceAddress;
	IDirectPlay8Address_AddRef( pDeviceAddress );

	m_fCommandPending = TRUE;
	m_pActiveCommandData = static_cast<CCommandData*>( m_PendingCommandData.ConnectData.hCommand );
	m_pActiveCommandData->SetUserContext( pConnectData->pvContext );
	m_State = ENDPOINT_STATE_ATTEMPTING_CONNECT;
};
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::ConnectJobCallback - asynchronous callback wrapper from work thread
//
// Entry:		Pointer to job information
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::ConnectJobCallback"

void	CEndpoint::ConnectJobCallback( THREAD_POOL_JOB *const pDelayedCommand )
{
	HRESULT		hr;
	CEndpoint	*pThisEndpoint;


	DNASSERT( pDelayedCommand != NULL );

	// initialize
	pThisEndpoint = static_cast<CEndpoint*>( pDelayedCommand->JobData.JobDelayedCommand.pContext );

	DNASSERT( pThisEndpoint->m_fCommandPending != FALSE );
	DNASSERT( pThisEndpoint->m_pActiveCommandData != NULL );
	DNASSERT( pThisEndpoint->m_PendingCommandData.ConnectData.hCommand == pThisEndpoint->m_pActiveCommandData );
	DNASSERT( pThisEndpoint->m_PendingCommandData.ConnectData.dwCommandDescriptor != NULL_DESCRIPTOR );
	DNASSERT( pThisEndpoint->m_PendingCommandData.ConnectData.pAddressDeviceInfo != NULL );

	hr = pThisEndpoint->CompleteConnect();
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem completing connect in job callback!" );
		DisplayDNError( 0, hr );
		goto Exit;
	}

	//
	// Don't do anything here because it's possible that this object was returned
	// to the pool!!!
	//

Exit:
	pThisEndpoint->DecRef();
	return;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::CancelConnectJobCallback - cancel for connect job
//
// Entry:		Pointer to job information
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::CancelConnectJobCallback"

void	CEndpoint::CancelConnectJobCallback( THREAD_POOL_JOB *const pDelayedCommand )
{
	CEndpoint	*pThisEndpoint;


	DNASSERT( pDelayedCommand != NULL );

	//
	// initialize
	//
	pThisEndpoint = static_cast<CEndpoint*>( pDelayedCommand->JobData.JobDelayedCommand.pContext );
	DNASSERT( pThisEndpoint != NULL );
	DNASSERT( pThisEndpoint->m_State == ENDPOINT_STATE_ATTEMPTING_CONNECT );

	//
	// we're cancelling this command, set the command state to 'cancel'
	//
	DNASSERT( pThisEndpoint->m_pActiveCommandData != NULL );
	pThisEndpoint->m_pActiveCommandData->Lock();
	DNASSERT( ( pThisEndpoint->m_pActiveCommandData->GetState() == COMMAND_STATE_PENDING ) ||
			  ( pThisEndpoint->m_pActiveCommandData->GetState() == COMMAND_STATE_CANCELLING ) );
	pThisEndpoint->m_pActiveCommandData->SetState( COMMAND_STATE_CANCELLING );
	pThisEndpoint->m_pActiveCommandData->Unlock();
	
	//
	// clean up
	//
	DNASSERT( pThisEndpoint->m_PendingCommandData.ConnectData.pAddressDeviceInfo != NULL );
	IDirectPlay8Address_Release( pThisEndpoint->m_PendingCommandData.ConnectData.pAddressDeviceInfo );

	pThisEndpoint->Close( DPNERR_USERCANCEL );
	pThisEndpoint->m_pSPData->CloseEndpointHandle( pThisEndpoint );
	pThisEndpoint->DecRef();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::CompleteConnect - complete connection
//
// Entry:		Nothing
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::CompleteConnect"

HRESULT	CEndpoint::CompleteConnect( void )
{
	HRESULT			hr;
	HRESULT			hTempResult;
	SPIE_CONNECT	ConnectIndicationData;
	BOOL			fEndpointBound;
	SPIE_CONNECTADDRESSINFO	ConnectAddressInfo;
	IDirectPlay8Address	*pDeviceAddress;


	//
	// initialize
	//
	hr = DPN_OK;
	fEndpointBound = FALSE;
	memset( &ConnectAddressInfo, 0x00, sizeof( ConnectAddressInfo ) );
	pDeviceAddress = m_PendingCommandData.ConnectData.pAddressDeviceInfo;
	DNASSERT( pDeviceAddress != NULL );

	DNASSERT( m_State == ENDPOINT_STATE_ATTEMPTING_CONNECT );
	DNASSERT( m_fCommandPending != FALSE );
	DNASSERT( m_pActiveCommandData != NULL );
	DNASSERT( m_PendingCommandData.ConnectData.hCommand == m_pActiveCommandData );
	DNASSERT( m_PendingCommandData.ConnectData.dwCommandDescriptor != NULL_DESCRIPTOR );
	

	//
	// check for user cancelling command
	//
	m_pActiveCommandData->Lock();

	DNASSERT( m_pActiveCommandData->GetType() == COMMAND_TYPE_CONNECT );
	switch ( m_pActiveCommandData->GetState() )
	{
		//
		// command is still pending, mark it as in-progress and uninterruptable
		//
		case COMMAND_STATE_INPROGRESS:
		case COMMAND_STATE_PENDING:
		{
			m_pActiveCommandData->SetState( COMMAND_STATE_INPROGRESS_CANNOT_CANCEL );
			DNASSERT( hr == DPN_OK );

			break;
		}

		//
		// command has been cancelled
		//
		case COMMAND_STATE_CANCELLING:
		{
			hr = DPNERR_USERCANCEL;
			DPF( 0, "User cancelled connect!" );

			break;
		}

		//
		// other state
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}
	m_pActiveCommandData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Failure;
	}

	hr = m_pSPData->BindEndpoint( this, pDeviceAddress, NULL, GATEWAY_BIND_TYPE_DEFAULT );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Failed to bind endpoint!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}
	
	//
	// attempt to indicate addressing to a higher layer
	//
	ConnectAddressInfo.pDeviceAddress = GetSocketPort()->GetDP8BoundNetworkAddress( SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT );
	ConnectAddressInfo.pHostAddress = GetRemoteHostDP8Address();
	ConnectAddressInfo.hCommandStatus = DPN_OK;
	ConnectAddressInfo.pCommandContext = m_pActiveCommandData->GetUserContext();

	if ( ( ConnectAddressInfo.pHostAddress == NULL ) ||
		 ( ConnectAddressInfo.pDeviceAddress == NULL ) )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	hTempResult = IDP8SPCallback_IndicateEvent( m_pSPData->DP8SPCallbackInterface(),	// interface
												SPEV_CONNECTADDRESSINFO,				// event type
												&ConnectAddressInfo						// pointer to data
												);
	DNASSERT( hTempResult == DPN_OK );
	
	//
	// Inform user of connection.  Assume that the user will accept and everything
	// will succeed so we can set the user context for the endpoint.  If the
	// connection fails, clear the user endpoint context.
	//
	memset( &ConnectIndicationData, 0x00, sizeof( ConnectIndicationData ) );
	DBG_CASSERT( sizeof( ConnectIndicationData.hEndpoint ) == sizeof( this ) );
	ConnectIndicationData.hEndpoint = GetHandle();
	ConnectIndicationData.pCommandContext = m_PendingCommandData.ConnectData.pvContext;
	SetUserEndpointContext( ConnectIndicationData.pEndpointContext );
	hr = SignalConnect( &ConnectIndicationData );
	if ( hr != DPN_OK )
	{
		DNASSERT( hr == DPNERR_ABORTED );
		DPF( 0, "User refused connect in CompleteConnect!" );
		DisplayDNError( 0, hr );
		SetUserEndpointContext( NULL );
		goto Failure;
	}

	//
	// we're done and everyone's happy, complete the command, this
	// will clear all of our internal command data
	//
	CompletePendingCommand( hr );
	DNASSERT( m_fCommandPending == FALSE );
	DNASSERT( m_pActiveCommandData == NULL );

Exit:
	if ( ConnectAddressInfo.pHostAddress != NULL )
	{
		IDirectPlay8Address_Release( ConnectAddressInfo.pHostAddress );
		ConnectAddressInfo.pHostAddress = NULL;
	}

	if ( ConnectAddressInfo.pDeviceAddress != NULL )
	{
		IDirectPlay8Address_Release( ConnectAddressInfo.pDeviceAddress );
		ConnectAddressInfo.pDeviceAddress = NULL;
	}

	DNASSERT( pDeviceAddress != NULL );
	IDirectPlay8Address_Release( pDeviceAddress );	
	
	return	hr;

Failure:
	//
	// we've failed to complete the connect, clean up and return this endpoint
	// to the pool
	//
	Close( hr );
	m_pSPData->CloseEndpointHandle( this );

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::Disconnect - disconnect an endpoint
//
// Entry:		Old endpoint handle
//
// Exit:		Error code
//
// Notes:	This function assumes that the endpoint is locked.  If this
//			function completes successfully (returns DPN_OK), the endpoint
//			is no longer locked (it was returned to the pool).
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::Disconnect"

HRESULT	CEndpoint::Disconnect( const HANDLE hOldEndpointHandle )
{
	HRESULT	hr;


	DNASSERT( hOldEndpointHandle != INVALID_HANDLE_VALUE );
	AssertCriticalSectionIsTakenByThisThread( &m_Lock, FALSE );

	//
	// initialize
	//
	hr = DPNERR_PENDING;

	Lock();
	switch ( GetState() )
	{
		//
		// connected endpoint
		//
		case ENDPOINT_STATE_CONNECT_CONNECTED:
		{
			HRESULT	hTempResult;


			DNASSERT( m_fCommandPending == FALSE );
			DNASSERT( m_pActiveCommandData == NULL );

			SetState( ENDPOINT_STATE_DISCONNECTING );
			AddRef();

			//
			// Unlock this endpoint before calling to a higher level.  The endpoint
			// has already been labeled as DISCONNECTING so nothing will happen to it.
			//
			Unlock();

			//
			// Note the old endpoint handle so it can be used in the disconnect
			// indication that will be given just before this endpoint is returned
			// to the pool.  Need to release the reference that was added for the
			// connection at this point or the endpoint will never be returned to
			// the pool.
			//
			SetDisconnectIndicationHandle( hOldEndpointHandle );
			DecRef();

			//
			// release reference from just after setting state
			//
			DecCommandRef();
			Close( DPN_OK );
			DecRef();

			break;
		}

		//
		// some other endpoint state
		//
		default:
		{
			hr = DPNERR_INVALIDENDPOINT;
			DPF( 0, "Attempted to disconnect endpoint that's not connected!" );
			switch ( m_State )
			{
				case ENDPOINT_STATE_UNINITIALIZED:
				{
					DPF( 0, "ENDPOINT_STATE_UNINITIALIZED" );
					break;
				}

				case ENDPOINT_STATE_ATTEMPTING_CONNECT:
				{
					DPF( 0, "ENDPOINT_STATE_ATTEMPTING_CONNECT" );
					break;
				}

				case ENDPOINT_STATE_ATTEMPTING_LISTEN:
				{
					DPF( 0, "ENDPOINT_STATE_ATTEMPTING_LISTEN" );
					break;
				}

				case ENDPOINT_STATE_ENUM:
				{
					DPF( 0, "ENDPOINT_STATE_ENUM" );
					break;
				}

				case ENDPOINT_STATE_DISCONNECTING:
				{
					DPF( 0, "ENDPOINT_STATE_DISCONNECTING" );
					break;
				}

				default:
				{
					DNASSERT( FALSE );
					break;
				}
			}

			Unlock();
			DNASSERT( FALSE );
			goto Failure;

			break;
		}
	}

Exit:
	return	hr;

Failure:
	// nothing to do
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::StopEnumCommand - stop a running enum command
//
// Entry:		Command result
//
// Exit:		Nothing
//
// Notes:	This function assumes that the endpoint is locked.  If this
//			function completes successfully (returns DPN_OK), the endpoint
//			is no longer locked (it was returned to the pool).
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::StopEnumCommand"

void	CEndpoint::StopEnumCommand( const HRESULT hCommandResult )
{
	Lock();
	DNASSERT( GetState() == ENDPOINT_STATE_DISCONNECTING );
		//
		// Don't hold the lock when cancelling a timer job because the
		// job might be in progress and attempting to use this endpoint!
		//
		Unlock();
		m_pSPData->GetThreadPool()->StopTimerJob( m_pActiveCommandData, hCommandResult );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::CopyListenData - copy data for listen command
//
// Entry:		Pointer to job information
//				Pointer to device address
//
// Exit:		Nothing
//
// Note:	Since we've already initialized the local adapter, and we've either
//			completely parsed the host address (or are about to display a dialog
//			asking for more information), the address information doesn't need
//			to be copied.
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::CopyListenData"

void	CEndpoint::CopyListenData( const SPLISTENDATA *const pListenData, IDirectPlay8Address *const pDeviceAddress )
{
	DNASSERT( pListenData != NULL );
	DNASSERT( pDeviceAddress != NULL );
	
	DNASSERT( pListenData->hCommand != NULL );
	DNASSERT( pListenData->dwCommandDescriptor != NULL_DESCRIPTOR );
	DNASSERT( m_fCommandPending == FALSE );
	DNASSERT( m_pActiveCommandData == NULL );
	DNASSERT( m_fListenStatusNeedsToBeIndicated == FALSE );

	DBG_CASSERT( sizeof( m_PendingCommandData.ListenData ) == sizeof( *pListenData ) );
	memcpy( &m_PendingCommandData.ListenData, pListenData, sizeof( m_PendingCommandData.ListenData ) );
	m_PendingCommandData.ListenData.pAddressDeviceInfo = pDeviceAddress;
	IDirectPlay8Address_AddRef( pDeviceAddress );

	m_fCommandPending = TRUE;
	m_fListenStatusNeedsToBeIndicated = TRUE;
	m_pActiveCommandData = static_cast<CCommandData*>( m_PendingCommandData.ListenData.hCommand );
	m_pActiveCommandData->SetUserContext( pListenData->pvContext );
	m_State = ENDPOINT_STATE_ATTEMPTING_LISTEN;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::ListenJobCallback - asynchronous callback wrapper for work thread
//
// Entry:		Pointer to job information
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::ListenJobCallback"

void	CEndpoint::ListenJobCallback( THREAD_POOL_JOB *const pDelayedCommand )
{
	HRESULT		hr;
	CEndpoint	*pThisEndpoint;


	DNASSERT( pDelayedCommand != NULL );

	//
	// initialize
	//
	pThisEndpoint = static_cast<CEndpoint*>( pDelayedCommand->JobData.JobDelayedCommand.pContext );

	DNASSERT( pThisEndpoint->m_fCommandPending != NULL );
	DNASSERT( pThisEndpoint->m_pActiveCommandData != NULL );
	DNASSERT( pThisEndpoint->m_PendingCommandData.ListenData.hCommand == pThisEndpoint->m_pActiveCommandData );
	DNASSERT( pThisEndpoint->m_PendingCommandData.ListenData.dwCommandDescriptor != NULL_DESCRIPTOR );
	DNASSERT( pThisEndpoint->m_PendingCommandData.ListenData.pAddressDeviceInfo != NULL );

	hr = pThisEndpoint->CompleteListen();
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem completing listen in job callback!" );
		DisplayDNError( 0, hr );
		goto Exit;
	}

Exit:
	pThisEndpoint->DecRef();

	return;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::CancelListenJobCallback - cancel for listen job
//
// Entry:		Pointer to job information
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::CancelListenJobCallback"

void	CEndpoint::CancelListenJobCallback( THREAD_POOL_JOB *const pDelayedCommand )
{
	CEndpoint	*pThisEndpoint;


	DNASSERT( pDelayedCommand != NULL );

	//
	// initialize
	//
	pThisEndpoint = static_cast<CEndpoint*>( pDelayedCommand->JobData.JobDelayedCommand.pContext );
	DNASSERT( pThisEndpoint != NULL );
	DNASSERT( pThisEndpoint->m_State == ENDPOINT_STATE_ATTEMPTING_LISTEN );

	//
	// we're cancelling this command, set the command state to 'cancel'
	//
	DNASSERT( pThisEndpoint->m_pActiveCommandData != NULL );
	pThisEndpoint->m_pActiveCommandData->Lock();
	DNASSERT( ( pThisEndpoint->m_pActiveCommandData->GetState() == COMMAND_STATE_PENDING ) ||
			  ( pThisEndpoint->m_pActiveCommandData->GetState() == COMMAND_STATE_CANCELLING ) );
	pThisEndpoint->m_pActiveCommandData->SetState( COMMAND_STATE_CANCELLING );
	pThisEndpoint->m_pActiveCommandData->Unlock();
	
	//
	// clean up
	//
	DNASSERT( pThisEndpoint->m_PendingCommandData.ListenData.pAddressDeviceInfo != NULL );
	IDirectPlay8Address_Release( pThisEndpoint->m_PendingCommandData.ListenData.pAddressDeviceInfo );

	pThisEndpoint->Close( DPNERR_USERCANCEL );
	pThisEndpoint->m_pSPData->CloseEndpointHandle( pThisEndpoint );
	pThisEndpoint->DecRef();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::CompleteListen - complete listen process
//
// Entry:		Nothing
//
// Exit:		Error code
//
// Note:	Calling this function may result in the deletion of 'this', don't
//			do anything else with this object after calling!!!!
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::CompleteListen"

HRESULT	CEndpoint::CompleteListen( void )
{
	HRESULT					hr;
	HRESULT					hTempResult;
	SPIE_LISTENSTATUS		ListenStatus;
	BOOL					fEndpointLocked;
	SPIE_LISTENADDRESSINFO	ListenAddressInfo;
	IDirectPlay8Address		*pDeviceAddress;
	GATEWAY_BIND_TYPE		GatewayBindType;


	//
	// initialize
	//
	hr = DPN_OK;
	fEndpointLocked = FALSE;
	memset( &ListenStatus, 0x00, sizeof( ListenStatus ) );
	memset( &ListenAddressInfo, 0x00, sizeof( ListenAddressInfo ) );
	pDeviceAddress = m_PendingCommandData.ListenData.pAddressDeviceInfo;
	DNASSERT( pDeviceAddress != NULL );

	if ( ( m_PendingCommandData.ListenData.dwFlags & DPNSPF_BINDLISTENTOGATEWAY ) != 0 )
	{
		GatewayBindType = GATEWAY_BIND_TYPE_SPECIFIC;
	}
	else
	{
		GatewayBindType = GATEWAY_BIND_TYPE_DEFAULT;
	}

	DNASSERT( m_State == ENDPOINT_STATE_ATTEMPTING_LISTEN );
	DNASSERT( m_fCommandPending != FALSE );
	DNASSERT( m_pActiveCommandData != NULL );
	DNASSERT( m_PendingCommandData.ListenData.hCommand == m_pActiveCommandData );
	DNASSERT( m_PendingCommandData.ListenData.dwCommandDescriptor != NULL_DESCRIPTOR );

	//
	// check for user cancelling command
	//
	m_pActiveCommandData->Lock();

	DNASSERT( m_pActiveCommandData->GetType() == COMMAND_TYPE_LISTEN );
	switch ( m_pActiveCommandData->GetState() )
	{
		//
		// command is pending, mark as in-progress
		//
		case COMMAND_STATE_PENDING:
		{
			m_pActiveCommandData->SetState( COMMAND_STATE_INPROGRESS );
			
			Lock();
			fEndpointLocked = TRUE;
			
			DNASSERT( hr == DPN_OK );

			break;
		}

		//
		// command has been cancelled
		//
		case COMMAND_STATE_CANCELLING:
		{
			hr = DPNERR_USERCANCEL;
			DPF( 0, "User cancelled listen!" );

			break;
		}

		//
		// other state
		//
		default:
		{
			break;
		}
	}
	m_pActiveCommandData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Failure;
	}

	//
	// note that this endpoint is officially listening before adding it to the
	// socket port because it may get used immediately
	//
	m_State = ENDPOINT_STATE_LISTEN;

	hr = m_pSPData->BindEndpoint( this, pDeviceAddress, NULL, GatewayBindType );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Failed to bind endpont!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

	//
	// attempt to indicate addressing to a higher layer
	//
	ListenAddressInfo.pDeviceAddress = GetSocketPort()->GetDP8BoundNetworkAddress( SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT );
	ListenAddressInfo.hCommandStatus = DPN_OK;
	ListenAddressInfo.pCommandContext = m_pActiveCommandData->GetUserContext();

	if ( ListenAddressInfo.pDeviceAddress == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	hTempResult = IDP8SPCallback_IndicateEvent( m_pSPData->DP8SPCallbackInterface(),	// interface
												SPEV_LISTENADDRESSINFO,					// event type
												&ListenAddressInfo						// pointer to data
												);
	DNASSERT( hTempResult == DPN_OK );
	
	if ( fEndpointLocked != FALSE )
	{
		Unlock();
		fEndpointLocked = FALSE;
	}

Exit:
	//
	// report the listen status
	//
	if ( m_fListenStatusNeedsToBeIndicated != FALSE )
	{
		m_fListenStatusNeedsToBeIndicated = FALSE;
		ListenStatus.hResult = hr;
		DNASSERT( m_pActiveCommandData == m_PendingCommandData.ListenData.hCommand );
		ListenStatus.hCommand = m_PendingCommandData.ListenData.hCommand;
		ListenStatus.pUserContext = m_PendingCommandData.ListenData.pvContext;
		ListenStatus.hEndpoint = GetHandle();

		//
		// if the listen binding failed, there's no socket port to dereference so
		// return GUID_NULL as set by the memset.
		//
		if ( GetSocketPort() != NULL )
		{
			GetSocketPort()->GetNetworkAddress()->GuidFromInternalAddressWithoutPort( ListenStatus.ListenAdapter );
		}

		//
		// it's possible that this endpoint was cleaned up so its internal pointers to the
		// COM and data interfaces may have been wiped, use the cached pointer
		//
		hTempResult = IDP8SPCallback_IndicateEvent( m_pSPData->DP8SPCallbackInterface(),	// pointer to DPlay callback interface
													SPEV_LISTENSTATUS,						// data type
													&ListenStatus							// pointer to data
													);
		DNASSERT( hTempResult == DPN_OK );
	}

	if ( ListenAddressInfo.pDeviceAddress != NULL )
	{
		IDirectPlay8Address_Release( ListenAddressInfo.pDeviceAddress );
		ListenAddressInfo.pDeviceAddress = NULL;
	}
	
	DNASSERT( pDeviceAddress != NULL );
	IDirectPlay8Address_Release( pDeviceAddress );
	
	return	hr;

Failure:
	//
	// we've failed to complete the listen, clean up and return this
	// endpoint to the pool
	//
	if ( fEndpointLocked != FALSE )
	{
		Unlock();
		fEndpointLocked = FALSE;
	}

	Close( hr );
	m_pSPData->CloseEndpointHandle( this );

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::CopyEnumQueryData - copy data for enum query command
//
// Entry:		Pointer to command data
//				Pointer to device address
//
// Exit:		Nothing
//
// Note:	Since we've already initialized the local adapter, and we've either
//			completely parsed the host address (or are about to display a dialog
//			asking for more information), the address information doesn't need
//			to be copied.
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::CopyEnumQueryData"

void	CEndpoint::CopyEnumQueryData( const SPENUMQUERYDATA *const pEnumQueryData, IDirectPlay8Address *const pDeviceAddress )
{
	DNASSERT( pEnumQueryData != NULL );
	DNASSERT( pDeviceAddress != NULL );

	DNASSERT( pEnumQueryData->hCommand != NULL );
	DNASSERT( pEnumQueryData->dwCommandDescriptor != NULL_DESCRIPTOR );
	DNASSERT( m_fCommandPending == FALSE );
	DNASSERT( m_pActiveCommandData == NULL );

	DBG_CASSERT( sizeof( m_PendingCommandData.EnumQueryData ) == sizeof( *pEnumQueryData ) );
	memcpy( &m_PendingCommandData.EnumQueryData, pEnumQueryData, sizeof( m_PendingCommandData.EnumQueryData ) );
	m_PendingCommandData.EnumQueryData.pAddressHost = NULL;
	m_PendingCommandData.EnumQueryData.pAddressDeviceInfo = pDeviceAddress;
	IDirectPlay8Address_AddRef( pDeviceAddress );

	m_fCommandPending = TRUE;
	m_pActiveCommandData = static_cast<CCommandData*>( m_PendingCommandData.EnumQueryData.hCommand );
	m_pActiveCommandData->SetUserContext( pEnumQueryData->pvContext );
	m_State = ENDPOINT_STATE_ATTEMPTING_ENUM;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::EnumQueryJobCallback - asynchronous callback wrapper for work thread
//
// Entry:		Pointer to job information
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::EnumQueryJobCallback"

void	CEndpoint::EnumQueryJobCallback( THREAD_POOL_JOB *const pDelayedCommand )
{
	HRESULT		hr;
	CEndpoint	*pThisEndpoint;


	DNASSERT( pDelayedCommand != NULL );

	// initialize
	pThisEndpoint = static_cast<CEndpoint*>( pDelayedCommand->JobData.JobDelayedCommand.pContext );

	DNASSERT( pThisEndpoint->m_fCommandPending != FALSE );
	DNASSERT( pThisEndpoint->m_pActiveCommandData != NULL );
	DNASSERT( pThisEndpoint->m_PendingCommandData.EnumQueryData.hCommand == pThisEndpoint->m_pActiveCommandData );
	DNASSERT( pThisEndpoint->m_PendingCommandData.EnumQueryData.dwCommandDescriptor != NULL_DESCRIPTOR );

	hr = pThisEndpoint->CompleteEnumQuery();
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem completing enum query in job callback!" );
		DisplayDNError( 0, hr );
		goto Exit;
	}

	//
	// Don't do anything here because it's possible that this object was returned to the pool!!!!
	//
Exit:
	pThisEndpoint->DecRef();

	return;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::CancelEnumQueryJobCallback - cancel for enum query job
//
// Entry:		Pointer to job information
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::CancelEnumQueryJobCallback"

void	CEndpoint::CancelEnumQueryJobCallback( THREAD_POOL_JOB *const pDelayedCommand )
{
	CEndpoint	*pThisEndpoint;


	DNASSERT( pDelayedCommand != NULL );

	//
	// initialize
	//
	pThisEndpoint = static_cast<CEndpoint*>( pDelayedCommand->JobData.JobDelayedCommand.pContext );
	DNASSERT( pThisEndpoint != NULL );
	DNASSERT( pThisEndpoint->m_State == ENDPOINT_STATE_ATTEMPTING_ENUM );

	//
	// we're cancelling this command, set the command state to 'cancel'
	//
	DNASSERT( pThisEndpoint->m_pActiveCommandData != NULL );
	pThisEndpoint->m_pActiveCommandData->Lock();
	DNASSERT( ( pThisEndpoint->m_pActiveCommandData->GetState() == COMMAND_STATE_INPROGRESS ) ||
			  ( pThisEndpoint->m_pActiveCommandData->GetState() == COMMAND_STATE_CANCELLING ) );
	pThisEndpoint->m_pActiveCommandData->SetState( COMMAND_STATE_CANCELLING );
	pThisEndpoint->m_pActiveCommandData->Unlock();
	
	//
	// clean up
	//
	DNASSERT( pThisEndpoint->m_PendingCommandData.EnumQueryData.pAddressDeviceInfo != NULL );
	IDirectPlay8Address_Release( pThisEndpoint->m_PendingCommandData.EnumQueryData.pAddressDeviceInfo );

	pThisEndpoint->Close( DPNERR_USERCANCEL );
	pThisEndpoint->m_pSPData->CloseEndpointHandle( pThisEndpoint );
	pThisEndpoint->DecRef();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::CompleteEnumQuery - complete enum query process
//
// Entry:		Nothing
//
// Exit:		Error code
//
// Note:	Calling this function may result in the deletion of 'this', don't
//			do anything else with this object after calling!!!!
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::CompleteEnumQuery"

HRESULT	CEndpoint::CompleteEnumQuery( void )
{
	HRESULT		hr;
	HRESULT		hTempResult;
	BOOL		fEndpointLocked;
	UINT_PTR	uRetryCount;
	BOOL		fRetryForever;
	DN_TIME		RetryInterval;
	BOOL		fWaitForever;
	DN_TIME		IdleTimeout;
	SPIE_ENUMADDRESSINFO	EnumAddressInfo;
	IDirectPlay8Address		*pDeviceAddress;


	//
	// initialize
	//
	hr = DPN_OK;
	fEndpointLocked = FALSE;
	IdleTimeout.Time32.TimeHigh = 0;
	IdleTimeout.Time32.TimeLow = 0;
	memset( &EnumAddressInfo, 0x00, sizeof( EnumAddressInfo ) );
	pDeviceAddress = m_PendingCommandData.EnumQueryData.pAddressDeviceInfo;
	DNASSERT( pDeviceAddress != NULL );

	DNASSERT( m_pSPData != NULL );

	DNASSERT( m_State == ENDPOINT_STATE_ATTEMPTING_ENUM );
	DNASSERT( m_fCommandPending != FALSE );
	DNASSERT( m_pActiveCommandData != NULL );
	DNASSERT( m_PendingCommandData.EnumQueryData.hCommand == m_pActiveCommandData );
	DNASSERT( m_PendingCommandData.EnumQueryData.dwCommandDescriptor != NULL_DESCRIPTOR );
	DNASSERT( m_PendingCommandData.EnumQueryData.pAddressDeviceInfo != NULL );

	//
	// Since this endpoint will be passed off to the timer thread, add a reference
	// for the thread.  If the handoff fails, DecRef()
	//
	AddRef();
	
	//
	// check for user cancelling command
	//
	m_pActiveCommandData->Lock();

	DNASSERT( m_pActiveCommandData->GetType() == COMMAND_TYPE_ENUM_QUERY );
	switch ( m_pActiveCommandData->GetState() )
	{
		//
		// command is still pending, mark it as in-progress
		//
		case COMMAND_STATE_PENDING:
		{
			m_pActiveCommandData->SetState( COMMAND_STATE_INPROGRESS );
			
			Lock();
			fEndpointLocked = TRUE;
			DNASSERT( hr == DPN_OK );

			break;
		}

		//
		// command has been cancelled
		//
		case COMMAND_STATE_CANCELLING:
		{
			hr = DPNERR_USERCANCEL;
			DPF( 0, "User cancelled enum query!" );

			break;
		}
	
		//
		// command is in progress (probably came here from a dialog)
		//
		case COMMAND_STATE_INPROGRESS:
		{
			Lock();
			fEndpointLocked = TRUE;
			DNASSERT( hr == DPN_OK );
			break;
		}

		//
		// other state
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}
	m_pActiveCommandData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Failure;
	}

	//
	// Submit job to the work thread.  Mark the endpoint as enuming in case the
	// enum thread takes off immediately.  If the endpoint fails to submit the enum
	// job it will be closed so changing the state was just a waste of a clock cycle.
	//
	m_State = ENDPOINT_STATE_ENUM;
	
	hr = m_pSPData->BindEndpoint( this, pDeviceAddress, NULL, GATEWAY_BIND_TYPE_DEFAULT );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Failed to bind endpoint!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

	//
	// Submit job to the work thread.  Mark the endpoint as enuming in case the
	// enum thread takes off immediately.  If the endpoint fails to submit the enum
	// job it will be closed so changing the state was just a waste of a clock cycle.
	//
	m_State = ENDPOINT_STATE_ENUM;
	
	EnumAddressInfo.pDeviceAddress = GetSocketPort()->GetDP8BoundNetworkAddress( SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT );
	EnumAddressInfo.pHostAddress = GetRemoteHostDP8Address();
	EnumAddressInfo.hCommandStatus = DPN_OK;
	EnumAddressInfo.pCommandContext = m_pActiveCommandData->GetUserContext();

	if ( ( EnumAddressInfo.pHostAddress == NULL ) ||
		 ( EnumAddressInfo.pDeviceAddress == NULL ) )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	hTempResult = IDP8SPCallback_IndicateEvent( m_pSPData->DP8SPCallbackInterface(),	// interface
												SPEV_ENUMADDRESSINFO,					// event type
												&EnumAddressInfo						// pointer to data
												);
	DNASSERT( hTempResult == DPN_OK );
	
	//
	// check retry count to determine if we're enumerating forever
	//
	switch ( m_PendingCommandData.EnumQueryData.dwRetryCount )
	{
		//
		// let SP determine retry count
		//
		case 0:
		{
			uRetryCount = DEFAULT_ENUM_RETRY_COUNT;
			fRetryForever = FALSE;
			break;
		}

		//
		// retry forever
		//
		case INFINITE:
		{
			uRetryCount = 1;
			fRetryForever = TRUE;
			break;
		}

		//
		// other
		//
		default:
		{
			uRetryCount = m_PendingCommandData.EnumQueryData.dwRetryCount;
			fRetryForever = FALSE;
			break;
		}
	}
	
	//
	// check interval for default
	//
	RetryInterval.Time32.TimeHigh = 0;
	if ( m_PendingCommandData.EnumQueryData.dwRetryInterval == 0 )
	{
		RetryInterval.Time32.TimeLow = DEFAULT_ENUM_RETRY_INTERVAL;
	}
	else
	{
		RetryInterval.Time32.TimeLow = m_PendingCommandData.EnumQueryData.dwRetryInterval;
	}

	//
	// check timeout to see if we're enumerating forever
	//
	switch ( m_PendingCommandData.EnumQueryData.dwTimeout )
	{
		//
		// wait forever
		//
		case INFINITE:
		{
			fWaitForever = TRUE;
			IdleTimeout.Time32.TimeHigh = -1;
			IdleTimeout.Time32.TimeLow = -1;
			break;
		}

		//
		// possible default
		//
		case 0:
		{
			fWaitForever = FALSE;
			IdleTimeout.Time32.TimeHigh = 0;
			IdleTimeout.Time32.TimeLow = DEFAULT_ENUM_TIMEOUT;	
			break;
		}

		//
		// other
		//
		default:
		{
			fWaitForever = FALSE;
			IdleTimeout.Time32.TimeHigh = 0;
			IdleTimeout.Time32.TimeLow = m_PendingCommandData.EnumQueryData.dwTimeout;
			break;
		}
	}

	memset( m_dwEnumSendTimes, 0x00, sizeof( m_dwEnumSendTimes ) );
	m_dwEnumSendIndex = 0;

	hr = m_pSPData->GetThreadPool()->SubmitTimerJob( uRetryCount,						// number of times to retry command
													 fRetryForever,						// retry forever
													 RetryInterval,						// retry interval
													 fWaitForever,						// wait forever after all enums sent
													 IdleTimeout,						// timeout to wait after command complete
													 CEndpoint::EnumTimerCallback,		// function called when timer event fires
													 CEndpoint::EnumCompleteWrapper,	// function called when timer event expires
													 m_pActiveCommandData );			// context
	if ( hr != DPN_OK )
	{
		DPF( 0, "Failed to spool enum job on work thread!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

	DNASSERT( m_fCommandPending != FALSE );
	DNASSERT( m_pActiveCommandData != NULL );
	
	if ( fEndpointLocked != FALSE )
	{
		Unlock();
		fEndpointLocked = FALSE;
	}

Exit:
	if ( EnumAddressInfo.pHostAddress != NULL )
	{
		IDirectPlay8Address_Release( EnumAddressInfo.pHostAddress );
		EnumAddressInfo.pHostAddress = NULL;
	}

	if ( EnumAddressInfo.pDeviceAddress != NULL )
	{
		IDirectPlay8Address_Release( EnumAddressInfo.pDeviceAddress );
		EnumAddressInfo.pDeviceAddress = NULL;
	}
	
	DNASSERT( pDeviceAddress != NULL );
	IDirectPlay8Address_Release( pDeviceAddress );
	
	return	hr;

Failure:
	//
	// we've failed to complete the enum query, clean up and return this
	// endpoint to the pool
	//
	if ( fEndpointLocked != FALSE )
	{
		Unlock();
		fEndpointLocked = FALSE;
	}

	Close( hr );
	m_pSPData->CloseEndpointHandle( this );

	//
	// remove timer thread reference
	//
	DecRef();

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::EnumCompleteWrapper - wrapper when enum has completed
//
// Entry:		Error code from enum command
//				Pointer to context	
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::EnumCompleteWrapper"

void	CEndpoint::EnumCompleteWrapper( const HRESULT hResult, void *const pContext )
{
	CCommandData	*pCommandData;


	DNASSERT( pContext != NULL );
	pCommandData = static_cast<CCommandData*>( pContext );
	pCommandData->GetEndpoint()->EnumComplete( hResult );
	pCommandData->GetEndpoint()->DecRef();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::EnumComplete - enum has completed
//
// Entry:		Error code from enum command
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::EnumComplete"

void	CEndpoint::EnumComplete( const HRESULT hResult )
{
	Lock();
	switch ( m_State )
	{
		//
		// enumerating, note that this endpoint is disconnecting
		//
		case ENDPOINT_STATE_ENUM:
		{
			SetState( ENDPOINT_STATE_DISCONNECTING );
			break;
		}

		//
		// disconnecting (command was probably cancelled)
		//
		case ENDPOINT_STATE_DISCONNECTING:
		{
			break;
		}

		//
		// there's a problem
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}
	Unlock();

	m_dwEnumSendIndex = 0;
	m_pSPData->CloseEndpointHandle( this );
	Close( hResult );

	return;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::CleanUpCommand - clean up this endpoint and unbind from CSocketPort
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::CleanupCommand"

void	CEndpoint::CleanUpCommand( void )
{
	//
	// There is an 'EndpointRef' that the endpoint holds against the
	// socket port since it was created and always must be released.
	// If the endpoint was bound it needs to be unbound.
	//
	if ( GetSocketPort() != NULL )
	{
		DNASSERT( m_pSPData != NULL );
		m_pSPData->UnbindEndpoint( this );
	}
	
	//
	// If we're bailing here it's because the UI didn't complete.  There is no
	// adapter guid to return because one may have not been specified.  Return
	// a bogus endpoint handle so it can't be queried for addressing data.
	//
	if ( m_fListenStatusNeedsToBeIndicated != FALSE )
	{
		HRESULT				hTempResult;
		SPIE_LISTENSTATUS	ListenStatus;
		

		m_fListenStatusNeedsToBeIndicated = FALSE;
		memset( &ListenStatus, 0x00, sizeof( ListenStatus ) );
		ListenStatus.hCommand = m_pActiveCommandData;
		ListenStatus.hEndpoint = INVALID_HANDLE_VALUE;
		ListenStatus.hResult = m_hPendingCommandResult;
		memset( &ListenStatus.ListenAdapter, 0x00, sizeof( ListenStatus.ListenAdapter ) );
		ListenStatus.pUserContext = m_pActiveCommandData->GetUserContext();

		hTempResult = IDP8SPCallback_IndicateEvent( m_pSPData->DP8SPCallbackInterface(),	// pointer to DPlay callbacks
													SPEV_LISTENSTATUS,						// data type
													&ListenStatus							// pointer to data
													);
		DNASSERT( hTempResult == DPN_OK );
	}
	
	m_State = ENDPOINT_STATE_UNINITIALIZED;
	SetHandle( INVALID_HANDLE_VALUE );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::ProcessEnumData - process received enum data
//
// Entry:		Pointer to received buffer
//				Associated enum key
//				Pointer to return address
//
// Exit:		Nothing
//
// Note:	This function assumes that the endpoint has been locked.
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::ProcessEnumData"

void	CEndpoint::ProcessEnumData( SPRECEIVEDBUFFER *const pBuffer, const DWORD dwEnumKey, const CSocketAddress *const pReturnSocketAddress )
{
	DNASSERT( pBuffer != NULL );
	DNASSERT( pReturnSocketAddress != NULL );
	AssertCriticalSectionIsTakenByThisThread( &m_Lock, FALSE );

	//
	// find out what state the endpoint is in before processing data
	//
	switch ( m_State )
	{
		//
		// we're listening, this is the only way to detect enums
		//
		case ENDPOINT_STATE_LISTEN:
		{
			ENDPOINT_ENUM_QUERY_CONTEXT	QueryContext;
			HRESULT		hr;


			//
			// initialize
			//
			DNASSERT( m_pActiveCommandData != NULL );
			DEBUG_ONLY( memset( &QueryContext, 0x00, sizeof( QueryContext ) ) );

			//
			// set callback data
			//
			QueryContext.hEndpoint = GetHandle();
			QueryContext.dwEnumKey = dwEnumKey;
			QueryContext.pReturnAddress = pReturnSocketAddress;
			
			QueryContext.EnumQueryData.pReceivedData = pBuffer;
			QueryContext.EnumQueryData.pUserContext = m_pActiveCommandData->GetUserContext();

			//
			// attempt to build a DNAddress for the user, if we can't allocate
			// the memory ignore this enum
			//
			QueryContext.EnumQueryData.pAddressSender = pReturnSocketAddress->DP8AddressFromSocketAddress();
			QueryContext.EnumQueryData.pAddressDevice = GetSocketPort()->GetDP8BoundNetworkAddress( SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT );

			if ( ( QueryContext.EnumQueryData.pAddressSender != NULL ) &&
				 ( QueryContext.EnumQueryData.pAddressDevice != NULL ) )
			{
				hr = IDP8SPCallback_IndicateEvent( m_pSPData->DP8SPCallbackInterface(),		// pointer to DirectNet interface
												   SPEV_ENUMQUERY,							// data type
												   &QueryContext.EnumQueryData				// pointer to data
												   );
				if ( hr != DPN_OK )
				{
					DPF( 0, "User returned unexpected error from enum query indication!" );
					DisplayDNError( 0, hr );
					DNASSERT( FALSE );
				}
			}

			if ( QueryContext.EnumQueryData.pAddressSender != NULL )
			{
				IDirectPlay8Address_Release( QueryContext.EnumQueryData.pAddressSender );
				QueryContext.EnumQueryData.pAddressSender = NULL;
 			}
			
			if ( QueryContext.EnumQueryData.pAddressDevice != NULL )
			{
				IDirectPlay8Address_Release( QueryContext.EnumQueryData.pAddressDevice );
				QueryContext.EnumQueryData.pAddressDevice = NULL;
 			}

			break;
		}

		//
		// we're disconnecting, ignore this message
		//
		case ENDPOINT_STATE_DISCONNECTING:
		{
			break;
		}

		//
		// other state
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::ProcessEnumResponseData - process received enum response data
//
// Entry:		Pointer to received data
//				Pointer to address of sender
//
// Exit:		Nothing
//
// Note:	This function assumes that the endpoint has been locked.
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::ProcessEnumResponseData"

void	CEndpoint::ProcessEnumResponseData( SPRECEIVEDBUFFER *const pBuffer,
											const CSocketAddress *const pReturnSocketAddress,
											const UINT_PTR uRTTIndex )
{
	DNASSERT( pBuffer != NULL );
	DNASSERT( pReturnSocketAddress != NULL );
	AssertCriticalSectionIsTakenByThisThread( &m_Lock, FALSE );

	//
	// find out what state the endpoint is in before processing data
	//
	switch ( m_State )
	{
		//
		// endpoint is enuming, it can handle enum responses
		//
		case ENDPOINT_STATE_ENUM:
		{
			SPIE_QUERYRESPONSE	QueryResponseData;
			HRESULT	hr;


			//
			// initialize
			//
			DNASSERT( m_pActiveCommandData != NULL );
			DEBUG_ONLY( memset( &QueryResponseData, 0x00, sizeof( QueryResponseData ) ) );

			//
			// set message data
			//
			QueryResponseData.pAddressSender = NULL;
			QueryResponseData.pReceivedData = pBuffer;
			QueryResponseData.dwRoundTripTime = timeGetTime() - m_dwEnumSendTimes[ uRTTIndex ];
			QueryResponseData.pUserContext = m_pActiveCommandData->GetUserContext();

			//
			// attempt to build a DPlay8Addresses for the user, if we can't allocate
			// the memory ignore this enum
			//
			QueryResponseData.pAddressSender = pReturnSocketAddress->DP8AddressFromSocketAddress();
			QueryResponseData.pAddressDevice = GetSocketPort()->GetDP8BoundNetworkAddress( SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT );
			
			if ( ( QueryResponseData.pAddressSender != NULL ) &&
				 ( QueryResponseData.pAddressDevice != NULL ) )
			{
				hr = IDP8SPCallback_IndicateEvent( m_pSPData->DP8SPCallbackInterface(),		// pointer to DirectNet interface
												   SPEV_QUERYRESPONSE,						// data type
												   &QueryResponseData						// pointer to data
												   );
				if ( hr != DPN_OK )
				{
					DPF( 0, "User returned unknown error when indicating query response!" );
					DisplayDNError( 0, hr );
					DNASSERT( FALSE );
				}
			}

			if ( QueryResponseData.pAddressSender != NULL )
			{
				IDirectPlay8Address_Release( QueryResponseData.pAddressSender );
				QueryResponseData.pAddressSender = NULL;
			}

			if ( QueryResponseData.pAddressDevice != NULL )
			{
				IDirectPlay8Address_Release( QueryResponseData.pAddressDevice );
				QueryResponseData.pAddressDevice = NULL;
			}

			break;
		}

		//
		// endpoint is disconnecting, ignore data
		//
		case ENDPOINT_STATE_DISCONNECTING:
		{
			break;
		}

		//
		// other state
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::ProcessUserData - process received user data
//
// Entry:		Pointer to received data
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::ProcessUserData"

void	CEndpoint::ProcessUserData( CReadIOData *const pReadData )
{
	DNASSERT( pReadData != NULL );

	switch ( m_State )
	{
		//
		// endpoint is connected
		//
		case ENDPOINT_STATE_CONNECT_CONNECTED:
		{
			HRESULT		hr;
			SPIE_DATA	UserData;


			//
			// it's possible that the user wants to keep the data, add a
			// reference to keep it from going away
			//
			pReadData->AddRef();
			DEBUG_ONLY( DNASSERT( pReadData->m_fRetainedByHigherLayer == FALSE ) );
			DEBUG_ONLY( pReadData->m_fRetainedByHigherLayer = TRUE );

			//
			// we're connected report the user data
			//
			DEBUG_ONLY( memset( &UserData, 0x00, sizeof( UserData ) ) );
			UserData.hEndpoint = GetHandle();
			UserData.pEndpointContext = GetUserEndpointContext();
			UserData.pReceivedData = pReadData->ReceivedBuffer();

			hr = IDP8SPCallback_IndicateEvent( m_pSPData->DP8SPCallbackInterface(),		// pointer to interface
											   SPEV_DATA,								// user data was received
											   &UserData								// pointer to data
											   );
			switch ( hr )
			{
				//
				// user didn't keep the data, remove the reference added above
				//
				case DPN_OK:
				{
					DNASSERT( pReadData != NULL );
					DEBUG_ONLY( pReadData->m_fRetainedByHigherLayer = FALSE );
					pReadData->DecRef();
					break;
				}

				//
				// The user kept the data buffer, they will return it later.
				// Leave the reference to prevent this buffer from being returned
				// to the pool.
				//
				case DPNERR_PENDING:
				{
					break;
				}


				//
				// Unknown return.  Remove the reference added above.
				//
				default:
				{
					DNASSERT( pReadData != NULL );
					DEBUG_ONLY( pReadData->m_fRetainedByHigherLayer = FALSE );
					pReadData->DecRef();

					DPF( 0, "User returned unknown error when indicating user data!" );
					DisplayDNError( 0, hr );
					DNASSERT( FALSE );

					break;
				}
			}

			break;
		}

		//
		// Endpoint disconnecting, or we haven't finished acknowledging a connect,
		// ignore data.
		//
		case ENDPOINT_STATE_ATTEMPTING_CONNECT:
		case ENDPOINT_STATE_DISCONNECTING:
		{
			break;
		}

		//
		// other state
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}

	return;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::ProcessUserDataOnListen - process received user data on a listen
//		port that may result in a new connection
//
// Entry:		Pointer to received data
//				Pointer to socket address that data was received from
//
// Exit:		Nothing
//
// Note:	This function assumes that this endpoint has been locked.
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::ProcessUserDataOnListen"

void	CEndpoint::ProcessUserDataOnListen( CReadIOData *const pReadData, const CSocketAddress *const pSocketAddress )
{
	HRESULT			hr;
	CEndpoint		*pNewEndpoint;
	SPIE_CONNECT	ConnectData;


	DNASSERT( pReadData != NULL );
	DNASSERT( pSocketAddress != NULL );
	AssertCriticalSectionIsTakenByThisThread( &m_Lock, FALSE );

	DPF( 8, "Reporting connect on a listen!" );

	//
	// initialize
	//
	pNewEndpoint = NULL;

	switch ( m_State )
	{
		//
		// this endpoint is still listening
		//
		case ENDPOINT_STATE_LISTEN:
		{
			break;
		}

		//
		// we're unable to process this user data, exti
		//
		case ENDPOINT_STATE_DISCONNECTING:
		{
			goto Exit;

			break;
		}

		//
		// other state
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}

	//
	// get a new endpoint from the pool
	//
	pNewEndpoint = m_pSPData->GetNewEndpoint();
	if ( pNewEndpoint == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Could not create new endpoint for new connection on listen!" );
		goto Failure;
	}

	//
	// open this endpoint as a new connection, since the new endpoint
	// is related to 'this' endpoint, copy local information
	//
	hr = pNewEndpoint->Open( ENDPOINT_TYPE_CONNECT_ON_LISTEN,
							 NULL,
							 pSocketAddress
							 );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem initializing new endpoint when indicating connect on listen!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

	hr = m_pSPData->BindEndpoint( pNewEndpoint, NULL, GetSocketPort()->GetNetworkAddress(), GATEWAY_BIND_TYPE_DEFAULT );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Failed to bind new endpoint for connect on listen!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

	//
	// Indicate connect on this endpoint.
	//
	DEBUG_ONLY( memset( &ConnectData, 0x00, sizeof( ConnectData ) ) );
	DBG_CASSERT( sizeof( ConnectData.hEndpoint ) == sizeof( pNewEndpoint ) );
	ConnectData.hEndpoint = pNewEndpoint->GetHandle();

	DNASSERT( m_fCommandPending != FALSE );
	DNASSERT( m_pActiveCommandData != NULL );
	ConnectData.pCommandContext = m_PendingCommandData.ListenData.pvContext;

	DNASSERT( pNewEndpoint->GetUserEndpointContext() == NULL );
	hr = pNewEndpoint->SignalConnect( &ConnectData );
	switch ( hr )
	{
		//
		// user accepted new connection
		//
		case DPN_OK:
		{
			//
			// fall through to code below
			//

			break;
		}

		//
		// user refused new connection
		//
		case DPNERR_ABORTED:
		{
			DNASSERT( pNewEndpoint->GetUserEndpointContext() == NULL );
			DPF( 8, "User refused new connection!" );
			goto Failure;

			break;
		}

		//
		// other
		//
		default:
		{
			DPF( 0, "Unknown return when indicating connect event on new connect from listen!" );
			DisplayDNError( 0, hr );
			DNASSERT( FALSE );

			break;
		}
	}

	//
	// note that a connection has been establised and send the data received
	// through this new endpoint
	//
	pNewEndpoint->ProcessUserData( pReadData );

Exit:
	return;

Failure:
	if ( pNewEndpoint != NULL )
	{
		//
		// closing endpoint decrements reference count and may return it to the pool
		//
		pNewEndpoint->Close( hr );
		m_pSPData->CloseEndpointHandle( pNewEndpoint );
		pNewEndpoint = NULL;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::EnumTimerCallback - timed callback to send enum data
//
// Entry:		Pointer to context
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::EnumTimerCallback"

void	CEndpoint::EnumTimerCallback( void *const pContext )
{
	CCommandData	*pCommandData;
	CEndpoint		*pThisObject;
	WRITE_IO_DATA_POOL_CONTEXT	PoolContext;
	CWriteIOData	*pWriteData;


	DNASSERT( pContext != NULL );

	//
	// initialize
	//
	pCommandData = static_cast<CCommandData*>( pContext );
	pThisObject = pCommandData->GetEndpoint();
	pWriteData = NULL;

	pThisObject->Lock();

	switch ( pThisObject->m_State )
	{
		//
		// we're enumerating (as expected)
		//
		case ENDPOINT_STATE_ENUM:
		{
			break;
		}

		//
		// this endpoint is disconnecting, bail!
		//
		case ENDPOINT_STATE_DISCONNECTING:
		{
			pThisObject->Unlock();
			goto Exit;

			break;
		}

		//
		// there's a problem
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}
	pThisObject->Unlock();

	//
	// attempt to get a new IO buffer for this endpoint
	//
	pWriteData = pThisObject->m_pSPData->GetThreadPool()->GetNewWriteIOData( &PoolContext );
	if ( pWriteData == NULL )
	{
		DPF( 0, "Failed to get write data for an enum!" );
		goto Failure;
	}

	//
	// Set all data for the write.  Since this is an enum and we
	// don't care about the outgoing data so don't send an indication
	// when it completes.
	//
	DNASSERT( pThisObject->m_fCommandPending != FALSE );
	DNASSERT( pThisObject->m_pActiveCommandData != NULL );
	DNASSERT( pThisObject->GetState() == ENDPOINT_STATE_ENUM );
	pWriteData->m_pBuffers = pThisObject->m_PendingCommandData.EnumQueryData.pBuffers;
	pWriteData->m_uBufferCount = pThisObject->m_PendingCommandData.EnumQueryData.dwBufferCount;
	pWriteData->m_pDestinationSocketAddress = pThisObject->GetRemoteAddressPointer();
	pWriteData->m_SendCompleteAction = SEND_COMPLETE_ACTION_NONE;

	DNASSERT( pWriteData->m_pCommand != NULL );
	DNASSERT( pWriteData->m_pCommand->GetUserContext() == NULL );
	pWriteData->m_pCommand->SetState( COMMAND_STATE_PENDING );

	DNASSERT( pThisObject->GetSocketPort() != NULL );
	pThisObject->m_dwEnumSendIndex++;
	pThisObject->m_dwEnumSendTimes[ ( pThisObject->m_dwEnumSendIndex & ENUM_RTT_MASK ) ] = timeGetTime();
	pThisObject->GetSocketPort()->SendEnumQueryData( pWriteData,
													 ( pThisObject->GetEnumKey()->GetKey() | ( pThisObject->m_dwEnumSendIndex & ENUM_RTT_MASK ) ) );

Exit:
	return;

Failure:
	// nothing to clean up at this time

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::SignalConnect - note connection
//
// Entry:		Pointer to connect data
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::SignalConnect"

HRESULT	CEndpoint::SignalConnect( SPIE_CONNECT *const pConnectData )
{
	HRESULT	hr;


	DNASSERT( pConnectData != NULL );
	DNASSERT( pConnectData->hEndpoint == GetHandle() );
	AssertCriticalSectionIsTakenByThisThread( &m_Lock, FALSE );

	//
	// initialize
	//
	hr = DPN_OK;

	switch ( m_State )
	{
		//
		// disconnecting, nothing to do
		//
		case ENDPOINT_STATE_DISCONNECTING:
		{
			goto Exit;
			break;
		}

		//
		// we're attempting to connect
		//
		case ENDPOINT_STATE_ATTEMPTING_CONNECT:
		{
			DNASSERT( m_fConnectSignalled == FALSE );
			hr = IDP8SPCallback_IndicateEvent( m_pSPData->DP8SPCallbackInterface(),		// interface
											   SPEV_CONNECT,							// event type
											   pConnectData								// pointer to data
											   );
			switch ( hr )
			{
				//
				// connection accepted
				//
				case DPN_OK:
				{
					//
					// note that we're connected
					//
					SetUserEndpointContext( pConnectData->pEndpointContext );
					m_fConnectSignalled = TRUE;
					m_State = ENDPOINT_STATE_CONNECT_CONNECTED;
					AddRef();

					break;
				}

				//
				// user aborted connection attempt, nothing to do, just pass
				// the result on
				//
				case DPNERR_ABORTED:
				{
					DNASSERT( GetUserEndpointContext() == NULL );
					break;
				}

				default:
				{
					DNASSERT( FALSE );
					break;
				}
			}

			break;
		}

		//
		// states where we shouldn't be getting called
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}

Exit:
	return	hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::SignalDisconnect - note disconnection
//
// Entry:		Old endpoint handle
//
// Exit:		Nothing
//
// Note:	This function assumes that this endpoint's data is locked!
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::SignalDisconnect"

void	CEndpoint::SignalDisconnect( const HANDLE hOldEndpointHandle )
{
	HRESULT	hr;
	SPIE_DISCONNECT	DisconnectData;


	// tell user that we're disconnecting
	DNASSERT( m_fConnectSignalled != FALSE );
	DBG_CASSERT( sizeof( DisconnectData.hEndpoint ) == sizeof( this ) );
	DisconnectData.hEndpoint = hOldEndpointHandle;
	DisconnectData.pEndpointContext = GetUserEndpointContext();
	m_fConnectSignalled = FALSE;
	hr = IDP8SPCallback_IndicateEvent( m_pSPData->DP8SPCallbackInterface(),		// interface
									   SPEV_DISCONNECT,							// event type
									   &DisconnectData							// pointer to data
									   );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem with SignalDisconnect!" );
		DisplayDNError( 0, hr );
		DNASSERT( FALSE );
	}

	SetDisconnectIndicationHandle( INVALID_HANDLE_VALUE );

	return;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CEndpoint::CompletePendingCommand - complete a pending command
//
// Entry:		Error code returned for command
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CEndpoint::CompletePendingCommand"

void	CEndpoint::CompletePendingCommand( const HRESULT hCommandResult )
{
	DNASSERT( m_fCommandPending != FALSE );
	DNASSERT( m_pActiveCommandData != NULL );

	IDP8SPCallback_CommandComplete( m_pSPData->DP8SPCallbackInterface(),		// pointer to DirectNet
									m_pActiveCommandData,						// command handle
									hCommandResult,								// return
									m_pActiveCommandData->GetUserContext()		// user cookie
									);

	memset( &m_PendingCommandData, 0x00, sizeof( m_PendingCommandData ) );
	m_fCommandPending = FALSE;
	m_pActiveCommandData->DecRef();
	m_pActiveCommandData = NULL;

	//
	// Now that the command is done, release the interface reference we were
	// holding.
	//
	IDP8ServiceProvider_Release( m_pSPData->COMInterface() );
}
//**********************************************************************

