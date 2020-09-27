/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       SocketPort.cpp
 *  Content:	Winsock socket port that manages data flow on a given adapter,
 *				address and port.
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

#define	INVALID_PAST_BIND_ID	0

#define	SOCKET_RECEIVE_BUFFER_SIZE		( 128 * 1024 )

//
// DPlay port limits (inclusive) scanned to find an available port.
// Exclude 2300 and 2301 because there are network broadcasts on 2301
// that we may receive.
//
static const WORD	g_wBaseDPlayPort = 2302;
static const WORD	g_wMaxDPlayPort = 2400;

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

//**********************************************************************
// Variable definitions
//**********************************************************************

extern CRITICAL_SECTION g_csSocketAccessLock;

//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::CSocketPort - constructor
//
// Entry:		Nothing
//
// Exit:		Nothing
//
// Notes:	Do not allocate anything in a constructor
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::CSocketPort"

CSocketPort::CSocketPort():
	m_pSPData( NULL ),
	m_pThreadPool( NULL ),
	m_iRefCount( 0 ),
	m_iEndpointRefCount( 0 ),
	m_State( SOCKET_PORT_STATE_UNKNOWN ),
	m_pNetworkSocketAddress( NULL ),
	m_pPASTSocketAddress( NULL ),
	m_pAdapterEntry( NULL ),
	m_dwPASTBindID( INVALID_PAST_BIND_ID ),
	m_Socket( INVALID_SOCKET ),
	m_hListenEndpoint( INVALID_HANDLE_VALUE ),
	m_iEnumKey( INVALID_ENUM_KEY ),
	m_pRemoveSocketPortData( NULL ),
	m_pSendFunction( NULL )
{
	DEBUG_ONLY( m_fInitialized = FALSE );
	m_ActiveListLinkage.Initialize();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::~CSocketPort - destructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::~CSocketPort"

CSocketPort::~CSocketPort()
{
	//
	// m_pThis needs to be around for the life of the endpoint
	// it should be part of the constructor, but can't be since we're using
	// a pool manager
	//
	DNASSERT( m_pSPData == NULL );
	DNASSERT( m_fInitialized == FALSE );

	DNASSERT( m_iRefCount == 0 );
	DNASSERT( m_iEndpointRefCount == 0 );
	DNASSERT( m_State == SOCKET_PORT_STATE_UNKNOWN );
	DNASSERT( GetSocket() == INVALID_SOCKET );
	DNASSERT( m_pNetworkSocketAddress == NULL );
	DNASSERT( m_pPASTSocketAddress == NULL );
	DNASSERT( m_pAdapterEntry == NULL );
	DNASSERT( m_dwPASTBindID == INVALID_PAST_BIND_ID );
	DNASSERT( m_ActiveListLinkage.IsEmpty() != FALSE );
	DNASSERT( m_hListenEndpoint == INVALID_HANDLE_VALUE );
	DNASSERT( m_iEnumKey == INVALID_ENUM_KEY );
	DNASSERT( m_pRemoveSocketPortData == NULL );
	DNASSERT( m_pSendFunction == NULL );
	DNASSERT( m_pThreadPool == NULL );
	DNASSERT( m_pSPData == NULL );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::Initialize - initialize this socket port
//
// Entry:		Pointer to CSPData
//				Pointer to address to bind to
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::Initialize"

HRESULT	CSocketPort::Initialize( CSPData *const pSPData, CSocketAddress *const pAddress )
{
	HRESULT	hr;
	HRESULT	hTempResult;


	DNASSERT( pSPData != NULL );
	DNASSERT( pAddress != NULL );

	//
	// initialize
	//
	hr = DPN_OK;
	m_pSPData = pSPData;
	m_pSPData->ObjectAddRef();
	m_pThreadPool = m_pSPData->GetThreadPool();

	DEBUG_ONLY( m_fInitialized = TRUE );
	DNASSERT( m_State != SOCKET_PORT_STATE_INITIALIZED );
	m_State = SOCKET_PORT_STATE_INITIALIZED;

	//
	// attempt to initialize the internal critical sections
	//
	if ( DNInitializeCriticalSection( &m_Lock ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Failed to initialize critical section for socket port!" );
		goto Failure;
	}
	DebugSetCriticalSectionRecursionCount( &m_Lock, 0 );

	if ( DNInitializeCriticalSection( &m_EndpointDataLock ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Failed to initialize EndpointDataLock critical section!" );
		goto Failure;
	}
	DebugSetCriticalSectionRecursionCount( &m_EndpointDataLock, 0 );

	//
	// attempt to initialize the contained send queue
	//
	hr = m_SendQueue.Initialize();
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem initializing send queue!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

	//
	// initialize the endpoint list with 64 entries and grow by a factor of 16
	//
	DNASSERT( hr == DPN_OK );
	if ( m_ConnectEndpointList.Initialize( 6, 4 ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Could not initialize the connect endpoint list!" );
		goto Failure;
	}

	//
	// initialize enum list with 16 entries and grow by a factor of 4
	//
	DNASSERT( hr == DPN_OK );
	if ( m_EnumEndpointList.Initialize( 4, 2 ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Could not initialize the enum endpoint list!" );
		goto Failure;
	}

	//
	// allocate addresses:
	//		local address this socket is binding to
	//		address of received messages
	//
	DNASSERT( m_pNetworkSocketAddress == NULL );
	DNASSERT( m_pPASTSocketAddress == NULL );
	m_pNetworkSocketAddress = pAddress;

	DNASSERT( m_pSendFunction == NULL );
		m_pSendFunction = Winsock2Send;

Exit:
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem in CSocketPort::Initialize()" );
		DisplayDNError( 0, hr );
	}

	return hr;

Failure:
	DEBUG_ONLY( m_fInitialized = FALSE );

	hTempResult = Deinitialize();
	if ( hTempResult != DPN_OK )
	{
		DPF( 0, "Problem deinitializing CSocketPort on failed Initialize!" );
		DisplayDNError( 0, hTempResult );
	}

	m_pNetworkSocketAddress = NULL;

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::Deinitialize - deinitialize this socket port
//
// Entry:		Nothing
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::Deinitialize"

HRESULT	CSocketPort::Deinitialize( void )
{
	HRESULT	hr;


	//
	// initialize
	//
	hr = DPN_OK;

	Lock();
	DNASSERT( ( m_State == SOCKET_PORT_STATE_INITIALIZED ) ||
			  ( m_State == SOCKET_PORT_STATE_UNBOUND ) );
	DEBUG_ONLY( m_fInitialized = FALSE );

	DNASSERT( m_iEndpointRefCount == 0 );
	DNASSERT( m_iRefCount == 0 );

	//
	// return base network socket addresses
	//
	if ( m_pNetworkSocketAddress != NULL )
	{
		m_pSPData->ReturnAddress( m_pNetworkSocketAddress );
		m_pNetworkSocketAddress = NULL;
	}

	//
	// if we've been involved with PAST, return the network address and bind ID
	//
	if ( m_pPASTSocketAddress != NULL )
	{
		m_pSPData->ReturnAddress( m_pPASTSocketAddress );
		m_pPASTSocketAddress = NULL;
	}
	DNASSERT( m_dwPASTBindID == INVALID_PAST_BIND_ID );

	m_pSendFunction = NULL;
	m_iEnumKey = INVALID_ENUM_KEY;

	m_SendQueue.Deinitialize();

	m_EnumEndpointList.Deinitialize();
	m_ConnectEndpointList.Deinitialize();

	Unlock();

	DNDeleteCriticalSection( &m_EndpointDataLock );
	DNDeleteCriticalSection( &m_Lock );

	DNASSERT( m_pSPData != NULL );
	m_pSPData->ObjectDecRef();
	m_pSPData = NULL;
	m_pThreadPool = NULL;

	return	hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::EndpointAddRef - increment endpoint reference count
//
// Entry:		Nothing
//
// Exit:		Nothing
//
// Notes:	This function should not be called without having the SP's SocketPort
//			data locked to make sure we're the only ones playing with socket ports!
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::EndpointAddRef"

void	CSocketPort::EndpointAddRef( void )
{
	Lock();

	//
	// add a global reference and then add an endpoint reference
	//
	DNASSERT( m_iEndpointRefCount != -1 );
	m_iEndpointRefCount++;
	AddRef();

	Unlock();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::EndpointDecRef - decrement endpoint reference count
//
// Entry:		Nothing
//
// Exit:		Endpoint reference count
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::EndpointDecRef"

DWORD	CSocketPort::EndpointDecRef( void )
{
	DWORD	dwReturn;


	Lock();

	DNASSERT( m_iEndpointRefCount != 0 );

	m_iEndpointRefCount--;
	dwReturn = m_iEndpointRefCount;
	if ( m_iEndpointRefCount == 0 )
	{
		HRESULT	hr;


		//
		// No more endpoints are referencing this item, unbind this socket port
		// from the network and then remove it from the active socket port list.
		// If we're on Winsock1, tell the other thread that this socket needs to
		// be removed so we can get rid of our outstanding I/O reference.
		//
		hr = UnbindFromNetwork();
		if ( hr != DPN_OK )
		{
			DPF( 0, "Problem unbinding from network when final endpoint has disconnected" );
			DisplayDNError( 0, hr );
		}

		m_State = SOCKET_PORT_STATE_UNBOUND;
		DNASSERT( m_pNetworkSocketAddress != NULL );
	}

	Unlock();

	//
	// Decrement global reference count.  This may result in this socketport
	// being returned to the pool!
	//
	DecRef();

	return	dwReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::BindEndpoint - add an endpoint to this SP's list
//
// Entry:		Pointer to endpoint
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::BindEndpoint"

HRESULT	CSocketPort::BindEndpoint( CEndpoint *const pEndpoint )
{
	HRESULT	hr;


	//
	// initialize
	//
	hr = DPN_OK;

	DNASSERT( m_iRefCount != 0 );

//	EndpointAddRef();

	pEndpoint->ChangeLoopbackAlias( GetNetworkAddress() );

	LockEndpointData();

	switch ( pEndpoint->GetType() )
	{
		//
		// Treat 'connect' and 'multicast' endpoints as the same type (multicast is just
		// a custom subset of addresses).  We don't care how many connections are made
		// through this socket port, just make sure we're not connecting to the same
		// person more than once
		//
		case ENDPOINT_TYPE_MULTICAST:
		case ENDPOINT_TYPE_CONNECT:
		{
			HANDLE	hExistingEndpoint;


			if ( m_ConnectEndpointList.Find( pEndpoint->GetRemoteAddressPointer() , &hExistingEndpoint ) != FALSE )
			{
				hr = DPNERR_ALREADYINITIALIZED;
				DPF( 0, "Attempted to connect twice to the same endpoint!" );
				goto Failure;
			}

			DNASSERT( hr == DPN_OK );
			if ( m_ConnectEndpointList.Insert( pEndpoint->GetRemoteAddressPointer(), pEndpoint->GetHandle() ) == FALSE )
			{
				hr = DPNERR_OUTOFMEMORY;
				DPF( 0, "Problem adding endpoint to socket port list!" );
				goto Failure;
			}

			pEndpoint->SetSocketPort( this );
			pEndpoint->AddRef();

			break;
		}

		//
		// we only allow one listen endpoint on a socket port
		//
		case ENDPOINT_TYPE_LISTEN:
		{
			if ( m_hListenEndpoint != INVALID_HANDLE_VALUE )
			{
				hr = DPNERR_ALREADYINITIALIZED;
				DPF( 0, "Attempted to listen more than once on a given SocketPort!" );
				goto Failure;
			}

			m_hListenEndpoint = pEndpoint->GetHandle();
			pEndpoint->SetSocketPort( this );
			pEndpoint->AddRef();

			break;
		}

		//
		// we don't allow duplicate enum endpoints
		//
		case ENDPOINT_TYPE_ENUM:
		{
			HANDLE	hExistingEndpoint;


			pEndpoint->SetEnumKey( GetEnumKey() );
			if ( m_EnumEndpointList.Find( pEndpoint->GetEnumKey(), &hExistingEndpoint ) != FALSE )
			{
				hr = DPNERR_ALREADYINITIALIZED;
				DPF( 0, "Attempted to enum twice to the same endpoint!" );
				goto Failure;
			}

			DNASSERT( hr == DPN_OK );
			if ( m_EnumEndpointList.Insert( pEndpoint->GetEnumKey(), pEndpoint->GetHandle() ) == FALSE )
			{
				hr = DPNERR_OUTOFMEMORY;
				DPF( 0, "Problem adding endpoint to socket port list!" );
				goto Failure;
			}

			pEndpoint->SetSocketPort( this );
			pEndpoint->AddRef();

			break;
		}

		//
		// unknown endpoint type
		//
		default:
		{
			INT3;
			hr = DPNERR_GENERIC;
			goto Failure;
			break;
		}
	}
	
	UnlockEndpointData();

Exit:
	return	hr;

Failure:
	UnlockEndpointData();
	
//	EndpointDecRef();
	
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::UnbindEndpoint - remove an endpoint from the SP's list
//
// Entry:		Pointer to endpoint
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::UnbindEndpoint"

void	CSocketPort::UnbindEndpoint( CEndpoint *const pEndpoint )
{
	DEBUG_ONLY( HANDLE	hFindTemp );


	LockEndpointData();

	//
	// adjust any special pointers before removing endpoint
	//
	switch ( pEndpoint->GetType() )
	{
		//
		// Multicast and connect endpoints are the same (multicast is a reserved subset
		// of the connect address space).  Remove endpoint from connect list.
		//
		case ENDPOINT_TYPE_MULTICAST:
		case ENDPOINT_TYPE_CONNECT:
		{
			DEBUG_ONLY( DNASSERT( m_ConnectEndpointList.Find( pEndpoint->GetRemoteAddressPointer(), &hFindTemp ) != FALSE ) );
			m_ConnectEndpointList.Remove( pEndpoint->GetRemoteAddressPointer() );
			pEndpoint->SetSocketPort( NULL );
			pEndpoint->DecRef();
			break;
		}

		//
		// make sure this is really the active listen and then remove it
		//
		case ENDPOINT_TYPE_LISTEN:
		{
			DNASSERT( m_hListenEndpoint != INVALID_HANDLE_VALUE );
			m_hListenEndpoint = INVALID_HANDLE_VALUE;
			pEndpoint->SetSocketPort( NULL );
			pEndpoint->DecRef();
			break;
		}

		//
		// remove endpoint from enum list
		//
		case ENDPOINT_TYPE_ENUM:
		{
			DEBUG_ONLY( DNASSERT( m_EnumEndpointList.Find( pEndpoint->GetEnumKey(), &hFindTemp ) != FALSE ) );
			m_EnumEndpointList.Remove( pEndpoint->GetEnumKey() );
			pEndpoint->SetSocketPort( NULL );
			pEndpoint->DecRef();
			break;
		}

		default:
		{
			INT3;
			break;
		}
	}

	UnlockEndpointData();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::DisconnectAllEndpoints - disconnect all endpoints bound to this
//		socket port
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::DisconnectAllEndpoints"

void	CSocketPort::DisconnectAllEndpoints( void )
{
	INT3;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::ReturnSelfToPool - return this object to the pool
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::ReturnSelfToPool"

void	CSocketPort::ReturnSelfToPool( void )
{
	m_State = SOCKET_PORT_STATE_UNKNOWN;
	DNASSERT( m_pSPData == NULL );
	ReturnSocketPort( this );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::SendEnumQueryData - send data for an enum query
//
// Entry:		Pointer to write data
//				Enum key
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::SendEnumQueryData"

void	CSocketPort::SendEnumQueryData( CWriteIOData *const pWriteData, const UINT_PTR uEnumKey )
{
	pWriteData->m_pBuffers = &pWriteData->m_pBuffers[ -1 ];
	pWriteData->m_uBufferCount++;
	DBG_CASSERT( sizeof( &pWriteData->m_PrependBuffer.EnumDataHeader ) == sizeof( BYTE* ) );
	pWriteData->m_pBuffers[ 0 ].pBufferData = reinterpret_cast<BYTE*>( &pWriteData->m_PrependBuffer.EnumDataHeader );
	pWriteData->m_pBuffers[ 0 ].dwBufferSize = sizeof( pWriteData->m_PrependBuffer.EnumDataHeader );
	
	DNASSERT( pWriteData->m_PrependBuffer.EnumResponseDataHeader.bSPLeadByte == SP_HEADER_LEAD_BYTE);
	pWriteData->m_PrependBuffer.EnumResponseDataHeader.bSPCommandByte = ENUM_DATA_KIND;

	DNASSERT( uEnumKey <= WORD_MAX );
	pWriteData->m_PrependBuffer.EnumResponseDataHeader.wEnumResponsePayload = static_cast<WORD>( uEnumKey );

	SendData( pWriteData );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::SendData - send data
//
// Entry:		Pointer to write data
//				Pointer to return address (real address the message should be returned to)
//				Enum key
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::SendProxiedEnumData"

void	CSocketPort::SendProxiedEnumData( CWriteIOData *const pWriteData, const CSocketAddress *const pReturnAddress, const UINT_PTR uOldEnumKey )
{
	DNASSERT( pWriteData != NULL );
	DNASSERT( pReturnAddress != NULL );

	pWriteData->m_pBuffers = &pWriteData->m_pBuffers[ -1 ];
	pWriteData->m_uBufferCount++;

	//
	// We could save 2 bytes on IPX by only passing 14 bytes for the
	// SOCKADDR structure but it's not worth it, especially since it's
	// looping back in the local network stack.  SOCKADDR structures are also
	// 16 bytes so reducing the data passed to 14 bytes would destroy alignment.
	//
	DBG_CASSERT( sizeof( pWriteData->m_PrependBuffer.ProxiedEnumDataHeader.ReturnAddress ) == 16 );
	DBG_CASSERT( sizeof( &pWriteData->m_PrependBuffer.ProxiedEnumDataHeader ) == sizeof( BYTE* ) );
	pWriteData->m_pBuffers[ 0 ].pBufferData = reinterpret_cast<BYTE*>( &pWriteData->m_PrependBuffer.ProxiedEnumDataHeader );
	pWriteData->m_pBuffers[ 0 ].dwBufferSize = sizeof( pWriteData->m_PrependBuffer.ProxiedEnumDataHeader );
	
	DNASSERT( pWriteData->m_PrependBuffer.ProxiedEnumDataHeader.bSPLeadByte == SP_HEADER_LEAD_BYTE );
	pWriteData->m_PrependBuffer.ProxiedEnumDataHeader.bSPCommandByte = PROXIED_ENUM_DATA_KIND;
	
	DNASSERT( uOldEnumKey <= WORD_MAX );
	pWriteData->m_PrependBuffer.ProxiedEnumDataHeader.wEnumKey = static_cast<WORD>( uOldEnumKey );

	DBG_CASSERT( sizeof( pWriteData->m_PrependBuffer.ProxiedEnumDataHeader.ReturnAddress ) == sizeof( *pReturnAddress->GetAddress() ) );
	memcpy( &pWriteData->m_PrependBuffer.ProxiedEnumDataHeader.ReturnAddress,
			pReturnAddress->GetAddress(),
			sizeof( pWriteData->m_PrependBuffer.ProxiedEnumDataHeader.ReturnAddress ) );

	SendData( pWriteData );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::SendData - send data
//
// Entry:		Pointer to write data
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::SendData"

void	CSocketPort::SendData( CWriteIOData *const pWriteData )
{
	DNASSERT( pWriteData != NULL );
	DNASSERT( pWriteData->SocketPort() == NULL );

	pWriteData->SetSocketPort( this );

	m_SendQueue.Lock();

	//
	// If the send queue is not empty, add this item to the end of the queue and
	// then attempt to send as much as possible.  Otherwise attempt to send this
	// data immediatly.
	//
	if ( m_SendQueue.IsEmpty() == FALSE )
	{
		BOOL	fIOServiced;


		m_SendQueue.Enqueue( pWriteData );
		m_SendQueue.Unlock();
		fIOServiced = SendFromWriteQueue();
	}
	else
	{
		SEND_COMPLETION_CODE	SendCompletionCode;


		//
		// there are no items in the queue, attempt to send
		//
		m_SendQueue.Unlock();

		DPF(8, "WriteData %x", pWriteData);
		
		pWriteData->m_pCommand->SetState( COMMAND_STATE_INPROGRESS_CANNOT_CANCEL );

		SendCompletionCode = (this->*m_pSendFunction)( pWriteData );
		switch ( SendCompletionCode )
		{
			//
			// Send has been spooled to Winsock, nothing to do
			//
			case SEND_IN_PROGRESS:
			{
				DPF( 8, "SendInProgress, will complete later" );
				break;
			}

			//
			// Send can't be submitted, spool it so it will go out the next
			// time someone tries to send.  Reset the command state to allow the
			// user to cancel the command.
			//
			case SEND_WINSOCK_BUSY:
			{
				DPF( 8, "Winsock Busy - Requeueing Send" );
				pWriteData->m_pCommand->SetState( COMMAND_STATE_PENDING );
				m_SendQueue.Lock();
				m_SendQueue.AddToFront( pWriteData );
				m_SendQueue.Unlock();
				break;
			}

			//
			// something went wrong, tell the user that their send barfed and
			// that the connection is probably going bye-bye
			//
			case SEND_FAILED:
			{
				SendComplete( pWriteData, DPNERR_CONNECTIONLOST );
				break;
			}

			//
			// invalid return
			//
			default:
			{
				DNASSERT( FALSE );
				break;
			}
		}
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::Winsock2Send - send data in a Winsock 2.0 fashion
//
// Entry:		Pointer to write data
//
// Exit:		Send completion code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::Winsock2Send"

SEND_COMPLETION_CODE	CSocketPort::Winsock2Send( CWriteIOData *const pWriteData )
{
	SEND_COMPLETION_CODE	SendCompletionCode;
	INT			iWSAReturn;


	DNASSERT( pWriteData != NULL );

	//
	// initialize
	//
	SendCompletionCode = SEND_IN_PROGRESS;

	//
	// note an I/O reference before submitting command
	//
	AddRef();

	DBG_CASSERT( sizeof( pWriteData->m_pBuffers ) == sizeof( WSABUF* ) );
	DBG_CASSERT( sizeof( *pWriteData->m_pBuffers ) == sizeof( WSABUF ) );
	DNASSERT( pWriteData->OverlapEvent() != NULL );
	DNASSERT( pWriteData->m_pDestinationSocketAddress != NULL );

	DPF( 8, "Winsock2 sending to socket:" );
	DumpSocketAddress( 8, pWriteData->m_pDestinationSocketAddress->GetAddress(), pWriteData->m_pDestinationSocketAddress->GetFamily() );

	//
	// lock the 'pending operation' list over the call to Winsock to prevent the
	// operation from being completed while it's being set up.
	//
		m_pSPData->GetThreadPool()->LockWriteData();
	
	//
	// Note that this operation is now in a 'pending' status.  It really
	// isn't yet, but Windows should alert us to that if we attempt to query
	// for I/O completion before the operation has been submitted.  Only assert
	// the 'pending' flag on Win9x.
	//
	DNASSERT( pWriteData->m_dwOverlappedBytesSent == 0 );
	DNASSERT( pWriteData->Win9xOperationPending() == FALSE );
	pWriteData->SetWin9xOperationPending( TRUE );

	DNASSERT( pWriteData->m_uBufferCount <= UINT32_MAX );

	DNEnterCriticalSection(&g_csSocketAccessLock);
	iWSAReturn = p_WSASendTo( GetSocket(),													// socket
							  reinterpret_cast<WSABUF*>( pWriteData->m_pBuffers ),			// buffers
							  static_cast<DWORD>( pWriteData->m_uBufferCount ),				// count of buffers
							  &pWriteData->m_dwBytesSent,									// pointer to number of bytes sent
							  0,															// send flags
							  pWriteData->m_pDestinationSocketAddress->GetAddress(),		// pointer to destination address
							  pWriteData->m_pDestinationSocketAddress->GetAddressSize(),	// size of destination address
							  pWriteData->Overlap(),										// pointer to overlap structure
							  NULL															// APC callback (unused)
							  );
	DNLeaveCriticalSection(&g_csSocketAccessLock);

		m_pSPData->GetThreadPool()->UnlockWriteData();

	if ( iWSAReturn == SOCKET_ERROR )
	{
		DWORD	dwWSAError;


		dwWSAError = p_WSAGetLastError();
		switch ( dwWSAError )
		{
			//
			// I/O is pending, note that the command cannot be cancelled,
			// wait for completion
			//
			case WSA_IO_PENDING:
			{
				DNASSERT( SendCompletionCode == SEND_IN_PROGRESS );
				break;
			}

			//
			// could not submit another overlapped I/O request, indicate that
			// the send was busy so someone above us spools the send for later
			//
			case WSAEWOULDBLOCK:
			{
				DPF( 8 , "Got WSAEWOULDBLOCK on Send " );
				pWriteData->SetWin9xOperationPending( FALSE );
				SendCompletionCode = SEND_WINSOCK_BUSY;
				DecRef();
				break;
			}

			//
			// socket was closed on us
			//
			case WSAENOTSOCK:
			default:
			{
				SendCompletionCode = SEND_FAILED;

				DPF( 8 , "Failing Send " );

				//
				// the operation was assumed to be pending and it's definitely
				// not going to be sent now
				//
				DNASSERT( pWriteData->Win9xOperationPending() != FALSE );
				pWriteData->SetWin9xOperationPending( FALSE );

				switch ( dwWSAError )
				{
					//
					// WSAENOTSOCK: another thread closed the socket
					// WSAENOBUFS: machine out of memory
					//
					case WSAENOTSOCK:
					case WSAENOBUFS:
					{
						break;
					}

					default:
					{
						//
						// something bad happened, stop and take a look
						//
						DisplayWinsockError( 0, dwWSAError );
						DNASSERT( FALSE );
						break;
					}
				}

				DecRef();
				break;
			}
		}
	}
	else
	{
		//
		// Send completed immediately.  There's nothing to do because the
		// delayed I/O completion notification will still be given and will do
		// final processing at that time.
		//
		DNASSERT( SendCompletionCode == SEND_IN_PROGRESS );
	}

	return	SendCompletionCode;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::Winsock2Receive - receive data in a Winsock 2.0 fashion
//
// Entry:		Nothing
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::Winsock2Receive"

HRESULT	CSocketPort::Winsock2Receive( void )
{
	HRESULT			hr;
	INT				iWSAReturn;
	READ_IO_DATA_POOL_CONTEXT	PoolContext;
	CReadIOData		*pReadData;


	//
	// initialize
	//
	hr = DPN_OK;

	pReadData = m_pThreadPool->GetNewReadIOData( &PoolContext );
	if ( pReadData == NULL )
	{
#pragma	BUGBUG( johnkan, "Handle this!" )
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Out of memory attempting Winsock2 read!" );
		INT3;
		goto Exit;
	}

	//
	// note the IO reference before attempting the read
	//
	AddRef();

	DNASSERT( pReadData->m_pSourceSocketAddress != NULL );
	DNASSERT( pReadData->SocketPort() == NULL );

	DBG_CASSERT( sizeof( pReadData->ReceivedBuffer()->BufferDesc ) == sizeof( WSABUF ) );
	DBG_CASSERT( OFFSETOF( BUFFERDESC, dwBufferSize ) == OFFSETOF( WSABUF, len ) );
	DBG_CASSERT( OFFSETOF( BUFFERDESC, pBufferData ) == OFFSETOF( WSABUF, buf ) );

	DNASSERT( pReadData->OverlapEvent() != NULL );

	pReadData->m_dwReadFlags = 0;
	pReadData->m_iSocketAddressSize = pReadData->m_pSourceSocketAddress->GetAddressSize();
	pReadData->SetSocketPort( this );

	DPF( 8, "Reading from socket.  Current read buffer:" );
	DumpSocketAddress( 8, pReadData->m_pSourceSocketAddress->GetAddress(), pReadData->m_pSourceSocketAddress->GetFamily() );

	//
	// lock the 'pending operation' list over the call to Winsock to prevent the
	// operation from being completed while it's being set up.
	//
		m_pSPData->GetThreadPool()->LockReadData();

	//
	// Note that this operation is 'pending'.  It really isn't, but Windows
	// should let us know if we query for completion status and this operation
	// isn't complete.  Only assert state on Win9x because NT doesn't use the
	// 'pending' field.
	//
	DNASSERT( pReadData->m_dwOverlappedBytesReceived == 0 );
	DNASSERT( pReadData->Win9xOperationPending() == FALSE );
	pReadData->SetWin9xOperationPending( TRUE );

Reread:
	if (GetSocket() == INVALID_SOCKET)
	{
					m_pSPData->GetThreadPool()->UnlockReadData();

				hr = DPNERR_GENERIC;

				DNASSERT( pReadData != NULL );

				DNASSERT( pReadData->Win9xOperationPending() != FALSE );
				pReadData->SetWin9xOperationPending( FALSE );
				pReadData->DecRef();
				pReadData = NULL;

				//
				// the following DecRef may result in this object being returned to the
				// pool, make sure we don't access member variables after this point!
				//
				DecRef();

				goto Exit;
    }

	DNEnterCriticalSection(&g_csSocketAccessLock);
	iWSAReturn = p_WSARecvFrom( GetSocket(),							// socket
								reinterpret_cast<WSABUF*>( &pReadData->ReceivedBuffer()->BufferDesc ),	// pointer to receive buffers
								1,										// number of receive buffers
								&pReadData->m_dwBytesRead,				// pointer to bytes received (if command completes immediately)
								&pReadData->m_dwReadFlags,				// flags (none)
								pReadData->m_pSourceSocketAddress->GetWritableAddress(),	// address of sending socket
								&pReadData->m_iSocketAddressSize,		// size of address of sending socket
								pReadData->Overlap(),					// pointer to overlapped structure
								NULL									// APC callback (unused)
								);	
	DNLeaveCriticalSection(&g_csSocketAccessLock);

	if ( iWSAReturn == 0 )
	{
			//
			// function completed immediately, do nothing, wait for IOCompetion
			// notification	to be processed
			//
				m_pSPData->GetThreadPool()->UnlockReadData();
	}
	else
	{
		DWORD	dwWSAReceiveError;


		//
		// failure, check for pending operation
		//
		dwWSAReceiveError = p_WSAGetLastError();
		switch ( dwWSAReceiveError )
		{
			//
			// the send is pending, nothing to do
			//
			case WSA_IO_PENDING:
			{
					m_pSPData->GetThreadPool()->UnlockReadData();

				break;
			}

            case WSAEINPROGRESS:		// BUGBUG: [mgere] [xbox] Added this to handle overlapped Recv error.  Make sure this is the proper place for it.
			{
					m_pSPData->GetThreadPool()->UnlockReadData();

				break;
			}

			//
			// Since this is a UDP socket, this is an indication
			// that a previous send failed.  Ignore it and move
			// on.
			//
			case WSAECONNRESET:
			{
				DPF( 8, "WSARecvFrom issued a WSACONNRESET!" );
				goto Reread;
				break;
			}

			case WSAENOTSOCK:
			{
					m_pSPData->GetThreadPool()->UnlockReadData();

				hr = DPNERR_GENERIC;

				DNASSERT( pReadData != NULL );

				DNASSERT( pReadData->Win9xOperationPending() != FALSE );
				pReadData->SetWin9xOperationPending( FALSE );
				pReadData->DecRef();
				pReadData = NULL;

				//
				// the following DecRef may result in this object being returned to the
				// pool, make sure we don't access member variables after this point!
				//
				DecRef();

				goto Exit;
			}

			//
			// there was a problem, no completion notification will
			// be given, decrement our IO reference count
			//
			default:
			{
					m_pSPData->GetThreadPool()->UnlockReadData();

				hr = DPNERR_GENERIC;
				
				//
				// 'Known Errors' that we don't want to ASSERT on.
				//
				// WSAENOTSOCKET: the socket has been closed
				// WSAESHUTDOWN: the socket has been shut down and is about to be closed
				//
				switch ( dwWSAReceiveError )
				{
					case WSAENOTSOCK:
					case WSAESHUTDOWN:
					{
						break;
					}

					default:
					{
						DPF( 0, "Problem with initial read in SocketPort::StartReceiving" );
						DisplayWinsockError( 0, dwWSAReceiveError );
						DNASSERT( FALSE );
					}
				}

				DNASSERT( pReadData != NULL );

				DNASSERT( pReadData->Win9xOperationPending() != FALSE );
				pReadData->SetWin9xOperationPending( FALSE );
				pReadData->DecRef();
				pReadData = NULL;

				//
				// the following DecRef may result in this object being returned to the
				// pool, make sure we don't access member variables after this point!
				//
				DecRef();

				goto Exit;
			}
		}
	}

Exit:
	return	hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::SendComplete - a Wisock send is complete, clean up and
//			notify user
//
// Entry:		Pointer to write data
//				Error code for this operation
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::SendComplete"

void	CSocketPort::SendComplete( CWriteIOData *const pWriteData, const HRESULT hResult )
{
	DNASSERT( pWriteData != NULL );
	DNASSERT( pWriteData->Win9xOperationPending() == FALSE );

#pragma	BUGBUG( johnkan, "Inline this function (except that everyone will need to know about the thread pool)!!!" )

	//
	// only signal user if requested
	//
	switch ( pWriteData->m_SendCompleteAction )
	{
		//
		// send command completion to user (most common case)
		//
		case SEND_COMPLETE_ACTION_COMPLETE_COMMAND:
		{
			DPF(8,"Completing Send Command hr %x Context %x",hResult,pWriteData->m_pCommand->GetUserContext() );
			IDP8SPCallback_CommandComplete( m_pSPData->DP8SPCallbackInterface(),		// pointer to DirectNet
											pWriteData->m_pCommand,						// command handle
											hResult,									// error code
											pWriteData->m_pCommand->GetUserContext()	// user cookie
											);
			break;
		}

		//
		// no action
		//
		case SEND_COMPLETE_ACTION_NONE:
		{
			if(pWriteData->m_pCommand){
				DPF(8,"Not Completing Send Command hr %x Context %x",hResult,pWriteData->m_pCommand->GetUserContext() );
			} else {
				DPF(8,"Not Completing Send Command hr %x Context NULL",hResult );
			}
			break;
		}

		//
		// Clean up after proxied enum.  The proxied enum will remove the
		// reference on the leave the receive buffer that came in on the enum.
		// The destination address that was supplied needs to be released
		// because it was allocated specifically for this task.
		//
		case SEND_COMPLETE_ACTION_PROXIED_ENUM_CLEANUP:
		{
			DNASSERT( pWriteData->m_pProxiedEnumReceiveBuffer != NULL );
			DNASSERT( pWriteData->m_pDestinationSocketAddress != NULL );

			pWriteData->m_pProxiedEnumReceiveBuffer->DecRef();
			pWriteData->m_pProxiedEnumReceiveBuffer = NULL;

			//
			// We know that this address was allocated specifically for this
			// proxied enum and we need to free it.
			//
			m_pSPData->ReturnAddress( const_cast<CSocketAddress*>( pWriteData->m_pDestinationSocketAddress ) );
			pWriteData->m_pDestinationSocketAddress = NULL;

			break;
		}

		//
		// unknown case
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}

	m_pThreadPool->ReturnWriteIOData( pWriteData );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::SetWinsockBufferSize -  set the buffer size used by Winsock for
//			this socket.
//
// Entry:		Buffer size
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::SetWinsockBufferSize"

void	CSocketPort::SetWinsockBufferSize( const INT iBufferSize ) const
{
	INT	iReturnValue;


	DPF( 3, "Setting socket receive buffer size to: %d", g_iWinsockReceiveBufferSize );

	iReturnValue = p_setsockopt( GetSocket(),
								 SOL_SOCKET,
								 SO_RCVBUF,
								 reinterpret_cast<char*>( &g_iWinsockReceiveBufferSize ),
								 sizeof( g_iWinsockReceiveBufferSize )
								 );
	if ( iReturnValue == SOCKET_ERROR )
	{
		DWORD	dwErrorCode;


		dwErrorCode = p_WSAGetLastError();
		DPF( 0, "Failed to set the socket buffer receive size!" );
		DisplayWinsockError( 0, dwErrorCode );
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::BindToNetwork - bind this socket port to the network
//
// Entry:		Handle of I/O completion port (NT only)
//				Type of socket
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::BindToNetwork"

HRESULT	CSocketPort::BindToNetwork( const GATEWAY_BIND_TYPE GatewayBindType )
{
	HRESULT			hr;
	INT				iReturnValue;
	BOOL			fBroadcastFlag;
	BOOL			fBoundToNetwork;
	INT				iSendBufferSize;
	DWORD			dwBlockValue;
	CSocketAddress	*pBoundSocketAddress;


	//
	// initialize
	//
	hr = DPN_OK;
	fBoundToNetwork = FALSE;
	pBoundSocketAddress = NULL;

	DNASSERT( m_fInitialized != FALSE );
	DNASSERT( m_State == SOCKET_PORT_STATE_INITIALIZED );

	//
	// get a socket for this socket port
	//
	DNASSERT( GetSocket() == INVALID_SOCKET );
	m_Socket = p_socket( m_pNetworkSocketAddress->GetFamily(),		// address family
						  SOCK_DGRAM,								// datagram (connectionless) socket
						  m_pNetworkSocketAddress->GetProtocol()	// protocol
						  );
	if ( GetSocket() == INVALID_SOCKET )
	{
		hr = DPNERR_NOCONNECTION;
		DPF( 0, "Failed to bind to socket!" );
		goto Failure;
	}

	//
	// set socket to allow broadcasts
	//
	fBroadcastFlag = TRUE;
	DBG_CASSERT( sizeof( &fBroadcastFlag ) == sizeof( char * ) );
	iReturnValue = p_setsockopt( GetSocket(),		// socket
	    						 SOL_SOCKET,		// level (set socket options)
	    						 SO_BROADCAST,		// set broadcast option
	    						 reinterpret_cast<char *>( &fBroadcastFlag ),	// allow broadcast
	    						 sizeof( fBroadcastFlag )	// size of parameter
	    						 );
	if ( iReturnValue == SOCKET_ERROR )
	{
		DWORD	dwErrorCode;


		dwErrorCode = p_WSAGetLastError();
	    DPF( 0, "Unable to set socket options!" );
	    DisplayWinsockError( 0, dwErrorCode );
	    hr = DPNERR_GENERIC;
	    goto Failure;
	}

	//
	// set socket receive buffer space if the user overrode it
	// Failing this is a preformance hit so ignore and errors.
	//
	if ( g_fWinsockReceiveBufferSizeOverridden != FALSE )
	{
		SetWinsockBufferSize( g_iWinsockReceiveBufferSize) ;
	}
	
	//
	// set socket send buffer space to 0 (we will supply all buffers).
	// Failing this is a preformance hit so ignore and errors.
	//
	iSendBufferSize = 0;
	iReturnValue = p_setsockopt( GetSocket(),
								 SOL_SOCKET,
								 SO_SNDBUF,
								 reinterpret_cast<char*>( &iSendBufferSize ),
								 sizeof( iSendBufferSize )
								 );
	if ( iReturnValue == SOCKET_ERROR )
	{
		DWORD	dwErrorCode;


		dwErrorCode = p_WSAGetLastError();
		DPF( 0, "Failed to set the socket buffer send size!" );
		DisplayWinsockError( 0, dwErrorCode );
	}

	//
	// put socket into non-block mode
	//
	dwBlockValue = 1;
	iReturnValue = p_ioctlsocket( GetSocket(),		// socket
	    						  FIONBIO,			// I/O option to set (blocking mode)
	    						  &dwBlockValue		// I/O option value (non-zero puts socked into non-block mode)
	    						  );
	if ( iReturnValue == SOCKET_ERROR )
	{
		DWORD	dwErrorCode;


		dwErrorCode = p_WSAGetLastError();
	    DPF( 0, "Could not set socket into non-blocking mode!" );
	    DisplayWinsockError( 0, dwErrorCode );
	    hr = DPNERR_GENERIC;
	    goto Failure;
	}

	//
	// bind socket
	//
	DPF( 1, "Binding to socket addess:" );
	DumpSocketAddress( 1, m_pNetworkSocketAddress->GetAddress(), m_pNetworkSocketAddress->GetFamily() );
	
	DNASSERT( GetSocket() != INVALID_SOCKET );
	hr = BindToNextAvailablePort( m_pNetworkSocketAddress );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Failed to bind to network!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}
	fBoundToNetwork = TRUE;

	//
	// Find out what address we really bound to.  This information is needed to
	// talk to the PAST server and will be needed when someone above querys for
	// what the local network address is.  If we are on the PAST server, munge
	// the address to be the 'public' address, but keep the port.
	//
	pBoundSocketAddress = GetBoundNetworkAddress( SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT );
	if ( pBoundSocketAddress == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Failed to get bound adapter address!" );
		goto Failure;
	}
	DPF( 1, "Socket we really bound to:" );
	DumpSocketAddress( 1, pBoundSocketAddress->GetAddress(), pBoundSocketAddress->GetFamily() );
	
	if ( m_pSPData->IsRsipServer() != FALSE )
	{
	    DNASSERT( m_pNetworkSocketAddress != NULL );
	    DNASSERT( m_pSPData->GetPublicSocketAddress() != NULL );
	    m_pNetworkSocketAddress->SetAddressFromSOCKADDR( *m_pSPData->GetPublicSocketAddress(),
	    												 sizeof( *( m_pSPData->GetPublicSocketAddress() ) ) );
	    m_pNetworkSocketAddress->SetPort( pBoundSocketAddress->GetPort() );
	}
	else
	{
		m_pSPData->ReturnAddress( m_pNetworkSocketAddress );
		m_pNetworkSocketAddress = pBoundSocketAddress;
		pBoundSocketAddress = NULL;
	}

	//
	// Attempt to bind to the PAST server.  If this fails, assume no PAST server
	// is present and fall back to plain NetworkSocketAddress.
	//
	DNASSERT( m_pNetworkSocketAddress != NULL );
	DNASSERT( m_pPASTSocketAddress == NULL );
	m_pPASTSocketAddress = m_pSPData->GetNewAddress();
	if ( m_pPASTSocketAddress == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Failed to allocate address for PAST binding!" );
		goto Failure;
	}
	m_pPASTSocketAddress->SetAddressType( SP_ADDRESS_TYPE_PUBLIC_HOST_ADDRESS );

	if ( BindToPASTServer( m_pNetworkSocketAddress, m_pPASTSocketAddress, GatewayBindType ) != DPN_OK )
	{
		m_pSPData->ReturnAddress( m_pPASTSocketAddress );
		m_pPASTSocketAddress = NULL;
		DNASSERT( m_dwPASTBindID == INVALID_PAST_BIND_ID );
	}

	//
	// start processing input messages
	// It's possible that messages will arrive before an endpoint is officially
	// bound to this socket port, but that's not a problem, the contents will
	// be lost
	//
	hr = StartReceiving();
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem starting endpoint receiving!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

Exit:
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem in CSocketPort::Initialize()" );
		DisplayDNError( 0, hr );
	}

	if ( pBoundSocketAddress != NULL )
	{
		m_pSPData->ReturnAddress( pBoundSocketAddress );
		pBoundSocketAddress = NULL;
	}

	return hr;

Failure:
	DEBUG_ONLY( m_fInitialized = FALSE );
	if ( fBoundToNetwork != FALSE )
	{
		UnbindFromNetwork();
	}
	else
	{
		//
		// If we were bound to network, m_Socket will be reset to INVALID_SOCKET.
		// Otherwise, we will take care of this ourselves (!)
		//
		iReturnValue = p_closesocket( m_Socket );
		if ( iReturnValue == SOCKET_ERROR )
		{
			DWORD	dwErrorCode;


			dwErrorCode = p_WSAGetLastError();
			DPF( 0, "Problem closing socket!" );
			DisplayWinsockError( 0, dwErrorCode );
		}
		m_Socket = INVALID_SOCKET;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::UnbindFromNetwork - unbind this socket port from the network
//
// Entry:		Nothing
//
// Exit:		Error code
//
// Note:	It is assumed that this socket port's information is locked!
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::UnbindFromNetwork"

HRESULT	CSocketPort::UnbindFromNetwork( void )
{
	HRESULT			hr;
	INT				iWSAReturn;
	SOCKET			TempSocket;


	//
	// initialize
	//
	hr = DPN_OK;

	TempSocket = GetSocket();
	m_Socket = INVALID_SOCKET;
	DNASSERT( TempSocket != INVALID_SOCKET );

	iWSAReturn = p_shutdown( TempSocket, SD_BOTH );
	if ( iWSAReturn == SOCKET_ERROR )
	{
		DWORD	dwErrorCode;


		dwErrorCode = p_WSAGetLastError();
		DPF( 0, "Problem shutting down socket!" );
		DisplayWinsockError( 0, dwErrorCode );
	}

	iWSAReturn = p_closesocket( TempSocket );
	if ( iWSAReturn == SOCKET_ERROR )
	{
		DWORD	dwErrorCode;


		dwErrorCode = p_WSAGetLastError();
		DPF( 0, "Problem closing socket!" );
		DisplayWinsockError( 0, dwErrorCode );
	}

	if ( m_dwPASTBindID != INVALID_PAST_BIND_ID )
	{
		DNASSERT( m_pAdapterEntry != NULL );
// BUGBUG: [mgere] [xbox] Remove RSIP
//		DNASSERT( m_pAdapterEntry->RsipModule() != NULL );
//		m_pAdapterEntry->RsipModule()->FreePort( m_dwPASTBindID );
		m_dwPASTBindID = INVALID_PAST_BIND_ID;
	}
	
	return	hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::BindToNextAvailablePort - bind to next available port
//
// Entry:		Pointer adapter address to bind to
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::BindToNextAvailablePort"

HRESULT	CSocketPort::BindToNextAvailablePort( const CSocketAddress *const pNetworkAddress ) const
{
	HRESULT	hr;
	CSocketAddress	*pDuplicateNetworkAddress;

	
	DNASSERT( pNetworkAddress != NULL );

	//
	// initialize
	//
	hr = DPN_OK;
	pDuplicateNetworkAddress = NULL;

	//
	// If a port was specified, try to bind to that port.  If no port was
	// specified, start walking the reserved DPlay port range looking for an
	// available port.  If none is found, let Winsock choose the port.
	//
	if ( pNetworkAddress->GetPort() != ANY_PORT )
	{
		INT	iSocketReturn;

		
		iSocketReturn = p_bind( GetSocket(),
								pNetworkAddress->GetAddress(),
								pNetworkAddress->GetAddressSize()
								);
		if ( iSocketReturn == SOCKET_ERROR )
		{
			DWORD	dwErrorCode;


			hr = DPNERR_ALREADYINITIALIZED;
			dwErrorCode = p_WSAGetLastError();
			DPF( 0, "Failed to bind socket!" );
			DisplayWinsockError( 0, dwErrorCode );
			goto Failure;
		}
	}
	else
	{
		WORD	wPort;
		INT		iSocketReturn;
		BOOL	fBound;


		fBound = FALSE;
		DNASSERT( pDuplicateNetworkAddress == NULL );
		pDuplicateNetworkAddress = m_pSPData->GetNewAddress();
		if ( pDuplicateNetworkAddress == NULL )
		{
			hr = DPNERR_OUTOFMEMORY;
			DPF( 0, "Failed to get address for walking reserved DPlay ports!" );
			goto Failure;
		}
	
		pDuplicateNetworkAddress->CopyAddressSettings( *pNetworkAddress );

		wPort = g_wBaseDPlayPort;
		while ( ( wPort <= g_wMaxDPlayPort ) && ( fBound == FALSE ) )
		{
			pDuplicateNetworkAddress->SetPort( p_htons( wPort ) );
			iSocketReturn = p_bind( GetSocket(),
									pDuplicateNetworkAddress->GetAddress(),
									pDuplicateNetworkAddress->GetAddressSize()
									);
			if ( iSocketReturn == SOCKET_ERROR )
			{
				DWORD	dwErrorCode;


				dwErrorCode = p_WSAGetLastError();
				switch ( dwErrorCode )
				{
					case WSAEADDRINUSE:
					{
						DPF( 8, "Port in use, skipping to next port!" );
						break;
					}

					default:
					{
						hr = DPNERR_NOCONNECTION;
						DPF( 0, "Failed to bind socket!" );
						DisplayWinsockError( 0, dwErrorCode );
						goto Failure;
						
						break;
					}
				}
			}
			else
			{
				DNASSERT( hr == DPN_OK );
				fBound = TRUE;
			}

			wPort++;
		}
	
		//
		// For some reason, all of the default DPlay ports were in use, let
		// Winsock choose.  We can use the network address passed because it
		// has 'ANY_PORT'.
		//
		if ( fBound == FALSE )
		{
			INT	iSocketReturn;


			DNASSERT( pNetworkAddress->GetPort() == ANY_PORT );
			iSocketReturn = p_bind( GetSocket(),
									pNetworkAddress->GetAddress(),
									pNetworkAddress->GetAddressSize()
									);
			if ( iSocketReturn == SOCKET_ERROR )
			{
				DWORD	dwErrorCode;


				hr = DPNERR_NOCONNECTION;
				dwErrorCode = p_WSAGetLastError();
				DPF( 0, "Failed to bind socket!" );
				DisplayWinsockError( 0, dwErrorCode );
				goto Failure;
			}
		}
	}

Exit:
	if ( pDuplicateNetworkAddress != NULL )
	{
		m_pSPData->ReturnAddress( pDuplicateNetworkAddress );
		pDuplicateNetworkAddress = NULL;
	}

	return	hr;

Failure:
	
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::BindToPASTServer - bind to PAST server
//
// Entry:		Pointer to SocketAddress we bound to
//				Pointer to PAST network address to be filled in
//				Gateway bind type
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::BindToPASTServer"

HRESULT	CSocketPort::BindToPASTServer( const CSocketAddress *const pBoundSocketAddress,
									   CSocketAddress *const pPASTSocketAddress,
									   const GATEWAY_BIND_TYPE GatewayBindType )
{
// BUGBUG: [mgere] [xbox] Removed RSIP
/*
	HRESULT	hr;


	DNASSERT( pBoundSocketAddress != NULL );
	DNASSERT( pPASTSocketAddress != NULL );

	hr = DPNERR_UNSUPPORTED;
	DNASSERT( GetAdapterEntry() != NULL );
	if ( GetAdapterEntry()->RsipModule() != NULL )
	{
		switch ( GatewayBindType )
		{
			//
			// ask the PAST server to listen for us (DPNSVR)
			//
			case GATEWAY_BIND_TYPE_SPECIFIC:
			{
				DPF( 0, "PAST: binding listen on server!" );
				DNASSERT( m_dwPASTBindID == INVALID_PAST_BIND_ID );
				hr = m_pAdapterEntry->RsipModule()->ListenPort( FALSE,
																pBoundSocketAddress->GetPort(),
																pPASTSocketAddress->GetWritableAddress(),
																&m_dwPASTBindID );
				if ( hr != DPN_OK )
				{
					DPF( 0, "PAST: Failed to bind listen!" );
					DumpSocketAddress( 0, m_pNetworkSocketAddress->GetAddress(), m_pNetworkSocketAddress->GetFamily() );
					DisplayDNError( 0, hr );
					goto Failure;
				}
				
				DPF( 0, "Bound SPECIFIC to PAST:" );
				DumpSocketAddress( 0, m_pNetworkSocketAddress->GetAddress(), m_pNetworkSocketAddress->GetFamily() );
				DumpSocketAddress( 0, pPASTSocketAddress->GetWritableAddress(), pPASTSocketAddress->GetFamily() );

				break;
			}

			//
			// just ask the server to open a generic socket for us (connect, listen, enum)
			//
			case GATEWAY_BIND_TYPE_DEFAULT:
			{
				DPF( 0, "PAST: binding to server!" );
				DNASSERT( m_dwPASTBindID == INVALID_PAST_BIND_ID );
				hr = m_pAdapterEntry->RsipModule()->AssignPort( FALSE,
																pBoundSocketAddress->GetPort(),
																pPASTSocketAddress->GetWritableAddress(),
																&m_dwPASTBindID );
				if ( hr != DPN_OK )
				{
					DPF( 0, "PAST: Failed to acquire assigned port!" );
					DumpSocketAddress( 0, m_pNetworkSocketAddress->GetAddress(), m_pNetworkSocketAddress->GetFamily() );
					DisplayDNError( 0, hr );
					goto Failure;
				}
				
				DPF( 0, "Bound DEFAULT to PAST:" );
				DumpSocketAddress( 0, m_pNetworkSocketAddress->GetAddress(), m_pNetworkSocketAddress->GetFamily() );
				DumpSocketAddress( 0, pPASTSocketAddress->GetWritableAddress(), pPASTSocketAddress->GetFamily() );

				break;
			}


			//
			// no binding
			//
			case GATEWAY_BIND_TYPE_NONE:
			{
				DNASSERT( hr == DPNERR_UNSUPPORTED );
				break;
			}

			//
			// unknown condition, someone broke the code!
			//
			default:
			{
				INT3;
				break;
			}
		}
	}

Exit:
	return	hr;

Failure:
	DNASSERT( m_dwPASTBindID == INVALID_PAST_BIND_ID );
	goto Exit;
*/
	return E_FAIL;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::StartReceiving - start receiving data on this socket port
//
// Entry:		Handle of I/O completion port to bind to (used on NT only)
//
// Exit:		Error code
//
// Notes:	There is no 'Failure' label in this function because failures need
//			to be cleaned up for each OS variant.
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::StartReceiving"

HRESULT	CSocketPort::StartReceiving( void )
{
	HRESULT	hr;


	//
	// initialize
	//
	hr = DPN_OK;

				//
				// we're using Winsock2, call for two outstanding reads per socket.
				//
				hr = Winsock2Receive();
				if ( hr != DPN_OK )
				{
					DPF( 0, "Problem issuing Win9x read in StartReceiving!" );
					DisplayDNError( 0, hr );
					INT3;
				}
// BUGBUG: [mgere] [xbox] Calling twice doesn't work on Xbox
//
/*
				hr = Winsock2Receive();
				if ( hr != DPN_OK )
				{
					DPF( 0, "Problem issuing Win9x read #2 in StartReceiving!" );
					DisplayDNError( 0, hr );
					INT3;
				}
*/

	return	hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::MungeAddressWithPAST - munge a socket address with PAST to turn
//		it into a local address.
//
// Entry:		Pointer to address
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::MungeAddressWithPAST"

void	CSocketPort::MungeAddressWithPAST( CSocketAddress *const pSocketAddress ) const
{
	DNASSERT( pSocketAddress != NULL );

	DNASSERT( m_pAdapterEntry != NULL );
// BUGBUG: [mgere] [xbox] Removed RSIP
/*
	if ( m_pAdapterEntry->RsipModule() != NULL )
	{
		HRESULT		hr;
		SOCKADDR	SocketAddress;


		DBG_CASSERT( sizeof( SocketAddress ) == sizeof( *pSocketAddress->GetAddress() ) );
		memcpy( &SocketAddress, pSocketAddress->GetAddress(), sizeof( SocketAddress ) );
		hr = m_pAdapterEntry->RsipModule()->QueryLocalAddress( FALSE,
															   pSocketAddress->GetAddress(),
															   &SocketAddress );
		if ( hr == DPN_OK )
		{
			pSocketAddress->SetAddressFromSOCKADDR( SocketAddress, sizeof( SocketAddress ) );
		}
	}
*/
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::SendFromWriteQueue - send as many items as possible from the
//		write queue
//
// Entry:		Nothing
//
// Exit:		Boolean indicating that something was sent
//				TRUE = data was sent
//				FALSE = no data was sent
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::SendFromWriteQueue"

BOOL	CSocketPort::SendFromWriteQueue( void )
{
	BOOL					fDataSent;
	CWriteIOData			*pWriteData;
	SEND_COMPLETION_CODE	TempCompletionCode;	


	DPF(8,"SendFromWriteQueue");

	//
	// initialize
	//
	fDataSent = FALSE;
	TempCompletionCode = SEND_IN_PROGRESS;

	m_SendQueue.Lock();
	pWriteData = m_SendQueue.Dequeue();
	DPF(8,"WriteData %p",pWriteData);
	m_SendQueue.Unlock();

	while ( ( pWriteData != NULL ) && ( TempCompletionCode == SEND_IN_PROGRESS ) )
	{
		pWriteData->m_pCommand->Lock();
		switch ( pWriteData->m_pCommand->GetState() )
		{
			//
			// command is still pending, attempt to send the data
			//
			case COMMAND_STATE_PENDING:
			{
				DPF(8,"COMMAND_STATE_PENDING");
				TempCompletionCode = (this->*m_pSendFunction)( pWriteData );
				DPF(8,"TempCompletionCode %x",TempCompletionCode);
				switch ( TempCompletionCode )
				{
					//
					// We managed to get this send going.  It's already been removed
					// from the queue.  Get the next item from the queue and restart
					// the loop.
					//
					case SEND_IN_PROGRESS:
					{
						DPF(8,"Send In Progress, going ok, so getting another");
						pWriteData->m_pCommand->SetState( COMMAND_STATE_INPROGRESS_CANNOT_CANCEL );
						pWriteData->m_pCommand->Unlock();
						
						m_SendQueue.Lock();
						pWriteData = m_SendQueue.Dequeue();
						DPF(8,"(SIP) WriteData %p",pWriteData);
						m_SendQueue.Unlock();

						fDataSent = TRUE;
						break;
					}

					//
					// Winsock is still busy, put this item back at the front of
					// the queue.  Clear 'pWriteData' to stop the loop.
					//
					case SEND_WINSOCK_BUSY:
					{
						DPF(8,"Winsock Busy, requeuing for later");
						DNASSERT( pWriteData->m_pCommand->GetState() == COMMAND_STATE_PENDING );
						m_SendQueue.Lock();
						m_SendQueue.AddToFront( pWriteData );
						m_SendQueue.Unlock();
						pWriteData->m_pCommand->Unlock();
						pWriteData = NULL;

						break;
					}

					//
					// send failed, try sending the next item
					//
					case SEND_FAILED:
					{
						DPF(8,"Send Failed");
						pWriteData->m_pCommand->SetState( COMMAND_STATE_INPROGRESS_CANNOT_CANCEL );
						pWriteData->m_pCommand->Unlock();

						SendComplete( pWriteData, DPNERR_GENERIC );

						m_SendQueue.Lock();
						pWriteData = m_SendQueue.Dequeue();
						m_SendQueue.Unlock();

						break;
					}

					//
					// invalid return
					//
					default:
					{
						DNASSERT( FALSE );
						break;
					}
				}

				break;
			}

			//
			// This command is to be cancelled, remove it from the queue
			// and issue completion notification to caller.  Then we can
			// look at the next item.
			//
			case COMMAND_STATE_CANCELLING:
			{
				pWriteData->m_pCommand->SetState( COMMAND_STATE_INPROGRESS_CANNOT_CANCEL );
				pWriteData->m_pCommand->Unlock();

				SendComplete( pWriteData, DPNERR_USERCANCEL );

				m_SendQueue.Lock();
				pWriteData = m_SendQueue.Dequeue();
				m_SendQueue.Unlock();

				break;
			}

			//
			// invalid command state
			//
			default:
			{
				DNASSERT( FALSE );
				break;
			}
		}
	}

	return	fDataSent;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::AddToMulticastGroup - add a socket to a multicast group
//
// Entry:		Pointer to multicast group data
//				Size of multicast group data
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::AddToMulticastGroup"

HRESULT	CSocketPort::AddToMulticastGroup( const void *const pGroupData, const UINT_PTR uGroupDataSize )
{
	HRESULT			hr;


	DNASSERT( pGroupData != NULL );
	DNASSERT( uGroupDataSize != 0 );

	DNASSERT( uGroupDataSize <= UINT32_MAX );
	hr = m_pNetworkSocketAddress->AddToMulticastGroup( pGroupData,
													   static_cast<DWORD>( uGroupDataSize ),
													   GetSocket() );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem adding socket port to multicast group!" );
		DisplayDNError( 0, hr );
	}

	return	hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::GetBoundNetworkAddress - get the full network address that
//		this socket port was really bound to
//
// Entry:		Address type for bound address
//
// Exit:		Pointer to network address
//
// Note:	Since this function creates a local address to derive the network
//			address from, it needs to know what kind of address to derive.  This
//			address type is supplied as the function parameter.
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::GetBoundNetworkAddress"

CSocketAddress	*CSocketPort::GetBoundNetworkAddress( const SP_ADDRESS_TYPE AddressType ) const
{
	HRESULT			hr;
	CSocketAddress	*pTempSocketAddress;
	SOCKADDR		BoundSocketAddress;
	INT_PTR			iReturnValue;
	INT				iBoundSocketAddressSize;


#pragma	BUGBUG( johnkan, "Speed this up!" )

	//
	// initialize
	//
	pTempSocketAddress = NULL;

	//
	// create addresses
	//
	pTempSocketAddress = m_pSPData->GetNewAddress();
	if ( pTempSocketAddress == NULL )
	{
		DPF( 0, "GetBoundNetworkAddress: Failed to create socket address!" );
		goto Failure;
	}

	//
	// find out what address we really bound to and reset the information for
	// this socket port
	//
	iBoundSocketAddressSize = pTempSocketAddress->GetAddressSize();
	iReturnValue = p_getsockname( GetSocket(), &BoundSocketAddress, &iBoundSocketAddressSize );
	if ( iReturnValue == SOCKET_ERROR )
	{
		DWORD	dwErrorCode;


		dwErrorCode = p_WSAGetLastError();
		hr = DPNERR_GENERIC;
		DPF( 0, "GetBoundNetworkAddress: Failed to get local socket name after bind!" );
		DisplayWinsockError( 0, dwErrorCode );
		goto Failure;
	}
	pTempSocketAddress->SetAddressFromSOCKADDR( BoundSocketAddress, iBoundSocketAddressSize );
	DNASSERT( iBoundSocketAddressSize == pTempSocketAddress->GetAddressSize() );

	//
	// Since this address was created locally, we need to tell it what type of
	// address to export according to the input.
	//
	pTempSocketAddress->SetAddressType( AddressType );

	switch ( AddressType )
	{
		//
		//  known types
		//
		case SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT:
		case SP_ADDRESS_TYPE_DEVICE_PROXIED_ENUM_TARGET:
		case SP_ADDRESS_TYPE_HOST:
		case SP_ADDRESS_TYPE_READ_HOST:
		{
			break;
		}

		//
		// if we're looking for a public address, we need to make sure that this
		// is not an undefined address.  If it is, don't return an address.
		// Otherwise, remap the address type to a 'host' address.
		//
		case SP_ADDRESS_TYPE_PUBLIC_HOST_ADDRESS:
		{
			if ( pTempSocketAddress->IsUndefinedHostAddress() != FALSE )
			{
				m_pSPData->ReturnAddress( pTempSocketAddress );
				pTempSocketAddress = NULL;
			}
			else
			{
				pTempSocketAddress->SetAddressType( SP_ADDRESS_TYPE_HOST );
			}

			break;
		}

		//
		// unknown address type, fix the code!
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}

	}

Exit:
	return	pTempSocketAddress;

Failure:

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::GetDP8BoundNetworkAddress - get the network address this machine
//		is bound to according to the input paramter.  If the requested address
//		is a public address and PAST is available, use the PAST address.  If a
//		public address is requested and PAST is unavailable, fall back to the
//		bound network address.
//
// Entry:		Type of address to get (local adapter vs. host)
//
// Exit:		Pointer to network address
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::GetDP8BoundNetworkAddress"

IDirectPlay8Address *CSocketPort::GetDP8BoundNetworkAddress( const SP_ADDRESS_TYPE AddressType ) const
{
	IDirectPlay8Address	*pAddress;


	//
	// initialize
	//
	pAddress = NULL;
	
	if ( ( m_pPASTSocketAddress != NULL ) &&
		 ( ( AddressType == SP_ADDRESS_TYPE_PUBLIC_HOST_ADDRESS ) ||
		   ( AddressType == SP_ADDRESS_TYPE_HOST ) ) )		
	{
		pAddress = m_pPASTSocketAddress->DP8AddressFromSocketAddress();
	}
	else
	{
		DNASSERT( m_pNetworkSocketAddress != NULL );
		switch ( AddressType )
		{
			case SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT:
			{
				pAddress = m_pNetworkSocketAddress->DP8AddressFromSocketAddress();
				break;
			}

			case SP_ADDRESS_TYPE_HOST:
			case SP_ADDRESS_TYPE_PUBLIC_HOST_ADDRESS:
			{
				CSocketAddress	*pTempAddress;


				pTempAddress = m_pSPData->GetNewAddress();
				if ( pTempAddress != NULL )
				{
					pTempAddress->CopyAddressSettings( *m_pNetworkSocketAddress );
					pTempAddress->SetAddressType( SP_ADDRESS_TYPE_HOST );
					pAddress = pTempAddress->DP8AddressFromSocketAddress();
				
					m_pSPData->ReturnAddress( pTempAddress );
				}
				else
				{
					DNASSERT( pAddress == NULL );
				}

				break;
			}

			//
			// shouldn't be here
			//
			default:
			{
				INT3;
				break;
			}
		}
	}

	return	pAddress;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::RemoveFromMulticastGroup - remove socket from multicast group
//
// Entry:		Pointer to group data
//				Size of group data
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::RemoveFromMulticastGroup"

HRESULT	CSocketPort::RemoveFromMulticastGroup( const void *const pGroupData, const UINT_PTR uGroupDataSize )
{
	HRESULT	hr;


	DNASSERT( pGroupData != NULL );
	DNASSERT( uGroupDataSize != 0 );

	//
	// initialize
	//
	hr = DPN_OK;

	DNASSERT( uGroupDataSize <= UINT32_MAX );
	hr = m_pNetworkSocketAddress->RemoveFromMulticastGroup( pGroupData,
															static_cast<DWORD>( uGroupDataSize ),
															GetSocket() );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem removing socket port from multicast group!" );
		DisplayDNError( 0, hr );
	}

	return hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::Winsock2ReceiveComplete - a Winsock2 socket receive completed
//
// Entry:		Poiner to read data
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::Winsock2ReceiveComplete"

void	CSocketPort::Winsock2ReceiveComplete( CReadIOData *const pReadData )
{
	DNASSERT( pReadData != NULL );

	//
	// initialize
	//
	DNASSERT( pReadData->Win9xOperationPending() == FALSE );

	//
	// figure out what's happening with this socket port
	//
	switch ( m_State )
	{
		//
		// we're unbound, discard this message and don't ask for any more
		//
		case SOCKET_PORT_STATE_UNBOUND:
		{
			goto Exit;
			break;
		}

		//
		// we're initialized, process input data and submit a new receive if
		// applicable
		//
		case SOCKET_PORT_STATE_INITIALIZED:
		{
			switch ( pReadData->m_Win9xReceiveWSAReturn )
			{
				//
				// the socket was closed on an outstanding read, stop
				// receiving
				//
				case WSAENOTSOCK:					// WinNT return for closed socket
				case ERROR_OPERATION_ABORTED:		// Win9x return for closed socket
				{
					break;
				}

				//
				// other error, perform another receive and process data if
				// applicable
				//
				default:
                {
					//
					// stop if the error isn't 'expected'
					//
					switch ( pReadData->m_Win9xReceiveWSAReturn )
					{
						// BUGBUG: [mgere] [xbox] WSAEINPROGRESS indicates a second overlapped receive was not allowed.
						// Ignore this but don't start another receive.  Make sure this works.
						case WSAEINPROGRESS:
						{
							break;
						}

						//
						// ERROR_SUCCESS = no problem (process received data)
						// WSAECONNRESET = previous send failed
						// ERROR_PORT_UNREACHABLE = previous send failed
						// ERROR_MORE_DATA = datagram was sent that was too large
						// ERROR_FILE_NOT_FOUND = socket was closed or previous send failed
						//
						case WSAECONNRESET:
						case ERROR_SUCCESS:
						case ERROR_PORT_UNREACHABLE:
						case ERROR_MORE_DATA:
						case ERROR_FILE_NOT_FOUND:
						{
							Winsock2Receive();
							break;
						}

						default:
						{
							DPF( 0, "Unexpected return from WSARecvFrom()" );
							DisplayErrorCode( 0, pReadData->m_Win9xReceiveWSAReturn );
							INT3;
							break;
						}
					}

					if ( pReadData->m_Win9xReceiveWSAReturn == ERROR_SUCCESS )
                    {
						pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize = pReadData->m_dwOverlappedBytesReceived;
						ProcessReceivedData( pReadData );
					}

					break;
				}
			}

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

Exit:
	//
	// Return the current data to the pool and note that this I/O operation is
	// complete.  Clear the overlapped bytes received so they aren't misinterpreted
	// if this item is reused from the pool.
	//
	DNASSERT( pReadData != NULL );
	pReadData->m_dwOverlappedBytesReceived = 0;	
	pReadData->DecRef();	
	DecRef();

	return;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::CancelReceive - cancel a pending receive
//
// Entry:		Poiner to read data
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::CancelReceive"

void	CSocketPort::CancelReceive( CReadIOData *const pRead )
{
	DNASSERT( pRead != NULL );

	pRead->DecRef();
	DecRef();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSocketPort::ProcessReceivedData - process received data
//
// Entry:		Pointer to CReadIOData
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSocketPort::ProcessReceivedData"

void	CSocketPort::ProcessReceivedData( CReadIOData *const pReadData )
{
	PREPEND_BUFFER	*pPrependBuffer;
	HANDLE		hEndpoint;
	CEndpoint	*pEndpoint;


	DNASSERT( pReadData != NULL );

//	DPF( 0, "Processing data from:" );
//	DumpSocketAddress( 0, pReadData->m_pSourceSocketAddress->GetAddress(), pReadData->m_pSourceSocketAddress->GetFamily() );
	
	DBG_CASSERT( sizeof( pReadData->ReceivedBuffer()->BufferDesc.pBufferData ) == sizeof( PREPEND_BUFFER* ) );
	pPrependBuffer = reinterpret_cast<PREPEND_BUFFER*>( pReadData->ReceivedBuffer()->BufferDesc.pBufferData );

	//
	// Check data for integrity and decide what to do with it.  If there is
	// enough data to determine an SP command type, try that.  If there isn't
	// enough data, and it looks spoofed, reject it.
	//
	if ( pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize >= sizeof( pPrependBuffer->GenericHeader ) )
	{
		if ( pPrependBuffer->GenericHeader.bSPLeadByte != SP_HEADER_LEAD_BYTE )
		{
			goto ProcessUserData;
		}
		else
		{
			switch ( pPrependBuffer->GenericHeader.bSPCommandByte )
			{
				//
				// Normal data, the user's first byte matched the service
				// provider tag byte.  Check for a non-zero payload and skip the
				// header that was added.
				//
				case ESCAPED_USER_DATA_KIND:
				{
					if ( ( pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize > sizeof( pPrependBuffer->EscapedUserDataHeader ) ) &&
						 ( pPrependBuffer->EscapedUserDataHeader.wPad == ESCAPED_USER_DATA_PAD_VALUE ) )
					{
						pReadData->ReceivedBuffer()->BufferDesc.pBufferData = &pReadData->ReceivedBuffer()->BufferDesc.pBufferData[ sizeof( pPrependBuffer->EscapedUserDataHeader ) ];
						DNASSERT( pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize > sizeof( pPrependBuffer->EscapedUserDataHeader ) );
						pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize -= sizeof( pPrependBuffer->EscapedUserDataHeader );
						goto ProcessUserData;
					}

					break;
				}

				//
				// Enum data, send it to the active listen (if there's one).  Allow users to send
				// enum requests that contain no data.
				//
				case ENUM_DATA_KIND:
				{
					if ( pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize >= sizeof( pPrependBuffer->EnumDataHeader ) )
					{
						LockEndpointData();
						if ( m_hListenEndpoint != INVALID_HANDLE_VALUE )
						{
							CEndpoint	*pEndpoint;


							//
							// add a reference to this endpoint so it doesn't go away while we're processing
							// this data
							//
							pEndpoint = m_pSPData->EndpointFromHandle( m_hListenEndpoint );
							UnlockEndpointData();
							
							if ( pEndpoint != NULL )
							{
								//
								// skip prepended enum header
								//
								pReadData->ReceivedBuffer()->BufferDesc.pBufferData = &pReadData->ReceivedBuffer()->BufferDesc.pBufferData[ sizeof( pPrependBuffer->EnumDataHeader ) ];
								DNASSERT( pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize >= sizeof( pPrependBuffer->EnumDataHeader ) );
								pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize -= sizeof( pPrependBuffer->EnumDataHeader );

								//
								// process data
								//
								pEndpoint->ProcessEnumData( pReadData->ReceivedBuffer(),
															pPrependBuffer->EnumDataHeader.wEnumPayload,
															pReadData->m_pSourceSocketAddress );
								pEndpoint->DecCommandRef();
							}
						}
						else
						{
							//
							// there's no listen active, return the receive buffer to the pool
							//
							UnlockEndpointData();
						}
					}

					break;
				}

				//
				// Enum response data, find the appropriate enum and pass it on.  Allow users to send
				// enum responses that contain no data.
				//
				case ENUM_RESPONSE_DATA_KIND:
				{
					if ( pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize >= sizeof( pPrependBuffer->EnumResponseDataHeader ) )
					{
						CEndpointEnumKey	Key;
						HANDLE				hEndpoint;


						Key.SetKey( pPrependBuffer->EnumResponseDataHeader.wEnumResponsePayload );
						LockEndpointData();
						if ( m_EnumEndpointList.Find( &Key, &hEndpoint ) != FALSE )
						{
							CEndpoint	*pEndpoint;


							UnlockEndpointData();

							DNASSERT( hEndpoint != INVALID_HANDLE_VALUE );
							pEndpoint = m_pSPData->EndpointFromHandle( hEndpoint );
							if ( pEndpoint != NULL )
							{
								pReadData->ReceivedBuffer()->BufferDesc.pBufferData = &pReadData->ReceivedBuffer()->BufferDesc.pBufferData[ sizeof( pPrependBuffer->EnumResponseDataHeader ) ];
								DNASSERT( pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize > sizeof( pPrependBuffer->EnumResponseDataHeader ) );
								pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize -= sizeof( pPrependBuffer->EnumResponseDataHeader );

								pEndpoint->ProcessEnumResponseData( pReadData->ReceivedBuffer(),
																	pReadData->m_pSourceSocketAddress,
																	( pPrependBuffer->EnumResponseDataHeader.wEnumResponsePayload & ENUM_RTT_MASK ) );
								pEndpoint->DecCommandRef();
							}
						}
						else
						{
							//
							// the associated ENUM doesn't exist, return the receive buffer
							//
							UnlockEndpointData();
						}
					}
	
					break;
				}

				//
				// proxied query data, this data was forwarded from another port.  Munge
				// the return address, modify the buffer pointer and then send it up
				// through the normal enum data processing pipeline.
				//
				case PROXIED_ENUM_DATA_KIND:
				{
					if ( pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize >= sizeof( pPrependBuffer->ProxiedEnumDataHeader ) )
					{
						LockEndpointData();
						if ( m_hListenEndpoint != INVALID_HANDLE_VALUE )
						{
							CEndpoint	*pEndpoint;


							//
							// add a reference to this endpoint so it doesn't go
							// away while we're processing this data
							//
							pEndpoint = m_pSPData->EndpointFromHandle( m_hListenEndpoint );
							UnlockEndpointData();
	
							if ( pEndpoint != NULL )
							{
								pReadData->m_pSourceSocketAddress->SetAddressFromSOCKADDR( pPrependBuffer->ProxiedEnumDataHeader.ReturnAddress,
																						   pReadData->m_pSourceSocketAddress->GetAddressSize() );

								pReadData->ReceivedBuffer()->BufferDesc.pBufferData = &pReadData->ReceivedBuffer()->BufferDesc.pBufferData[ sizeof( pPrependBuffer->ProxiedEnumDataHeader ) ];

								DNASSERT( pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize > sizeof( pPrependBuffer->ProxiedEnumDataHeader ) );
								pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize -= sizeof( pPrependBuffer->ProxiedEnumDataHeader );

								pEndpoint->ProcessEnumData( pReadData->ReceivedBuffer(),
															pPrependBuffer->ProxiedEnumDataHeader.wEnumKey,
															pReadData->m_pSourceSocketAddress );
								pEndpoint->DecCommandRef();
							}
						}
						else
						{
							//
							// there's no listen active, return the receive buffer to the pool
							//
							UnlockEndpointData();
						}
					}
		
					break;
				}
			}
		}
	}
	else	// pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize >= sizeof( pPrependBuffer->GenericHeader
	{
		if ( ( pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize == 1 ) &&
			 ( pPrependBuffer->GenericHeader.bSPLeadByte != SP_HEADER_LEAD_BYTE ) )
		{
			goto ProcessUserData;
		}
	}

Exit:
	return;

ProcessUserData:
	//	
	// If there's an active connection, send it to the connection.  If there's
	// no active connection, send it to an available 'listen' to indicate a
	// potential new connection.	
	//
	LockEndpointData();
	DNASSERT( pReadData->ReceivedBuffer()->BufferDesc.dwBufferSize != 0 );
	
	if ( m_ConnectEndpointList.Find( pReadData->m_pSourceSocketAddress, &hEndpoint ) != FALSE )
	{
		UnlockEndpointData();
		
		DNASSERT( hEndpoint != INVALID_HANDLE_VALUE );
		pEndpoint = m_pSPData->EndpointFromHandle( hEndpoint );

		if ( pEndpoint != NULL )
		{
			pEndpoint->ProcessUserData( pReadData );
			pEndpoint->DecCommandRef();
		}
	}
	else
	{
		if ( m_hListenEndpoint != INVALID_HANDLE_VALUE )
		{
			CEndpoint	*pEndpoint;


			pEndpoint = m_pSPData->EndpointFromHandle( m_hListenEndpoint );
			UnlockEndpointData();

			if ( pEndpoint != NULL )
			{
				pEndpoint->ProcessUserDataOnListen( pReadData, pReadData->m_pSourceSocketAddress );
				pEndpoint->DecCommandRef();
			}
		}
		else
		{
			//
			// nobody claimed this data, return the receive buffer to the
			// pool before exiting because we're supposed to.
			//
			UnlockEndpointData();
		}
	}

	goto Exit;
}
//**********************************************************************

