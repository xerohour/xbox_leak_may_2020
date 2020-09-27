/*==========================================================================
 *
 *  Copyright (C) 1999-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	   SPData.cpp
 *  Content:	Global variables for the DNSerial service provider in class
 *				format.
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	03/15/99	jtk		Dereived from Locals.cpp
 *  01/10/20000	rmt		Updated to build with Millenium build process
 *  03/22/20000	jtk		Updated with changes to interface names
 ***************************************************************************/

#include "wsockspi.h"


#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_WSOCK

//**********************************************************************
// Constant definitions
//**********************************************************************

// default number of command descriptors to create
#define	DEFAULT_COMMAND_POOL_SIZE	20
#define	COMMAND_POOL_GROW_SIZE		5

#define	PORT_STRING_LENGTH	20
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
// CSPData::CSPData - constructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::CSPData"

CSPData::CSPData():
	m_lRefCount( 0 ),
	m_lObjectRefCount( 0 ),
	m_hShutdownEvent( NULL ),
	m_SPState( SPSTATE_UNINITIALIZED ),
	m_fIsRsipServer( FALSE ),
	m_pBroadcastAddress( NULL ),
	m_pListenAddress( NULL ),
	m_pGenericAddress( NULL ),
	m_pThreadPool( NULL ),
	m_fHandleTableInitialized( FALSE ),
	m_fLockInitialized( FALSE ),
	m_fSocketPortDataLockInitialized( FALSE ),
	m_fSocketPortListInitialized( FALSE ),
	m_fInterfaceGlobalsInitialized( FALSE ),
	m_fDefaultAddressesBuilt( FALSE )
{
	memset( &m_InitData, 0x00, sizeof( m_InitData ) );
	memset( &m_COMInterface, 0x00, sizeof( m_COMInterface ) );
	DNInterlockedIncrement( &g_lOutstandingInterfaceCount );
	m_ActiveAdapterList.Initialize();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::~CSPData - destructor
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::~CSPData"

CSPData::~CSPData()
{
	DNASSERT( m_lRefCount == 0 );
	DNASSERT( m_lObjectRefCount == 0 );
	DNASSERT( m_hShutdownEvent == NULL );
	DNASSERT( m_SPState == SPSTATE_UNINITIALIZED );
	DNASSERT( m_fIsRsipServer == FALSE );
	DNASSERT( m_pThreadPool == NULL );
	DNASSERT( m_ActiveSocketPortList.IsEmpty() != FALSE );
	DNASSERT( m_fHandleTableInitialized == FALSE );
	DNASSERT( m_fLockInitialized == FALSE );
	DNASSERT( m_fSocketPortDataLockInitialized == FALSE );
	DNASSERT( m_fSocketPortListInitialized == FALSE );
	DNASSERT( m_fInterfaceGlobalsInitialized == FALSE );
	DNASSERT( m_fDefaultAddressesBuilt == FALSE );
	DNASSERT( m_ActiveAdapterList.IsEmpty() );
	DNInterlockedDecrement( &g_lOutstandingInterfaceCount );
	memset( &m_PublicSocketAddress, 0x00, sizeof( m_PublicSocketAddress ) );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::Initialize - intialize
//
// Entry:		Class ID
//				SP type
//				Pointer to SP COM vtable
//
// Exit:		Error code
//
// Note:	This function assumes that someone else preventing reentry!
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::Initialize"

HRESULT	CSPData::Initialize( const CLSID *const pClassID,
							 IDP8ServiceProviderVtbl *const pVtbl )
{
	HRESULT			hr;


	DNASSERT( pVtbl != NULL );

	//
	// initialize
	//
	hr = DPN_OK;
	DNASSERT( m_lRefCount == 1 );
	DNASSERT( m_lObjectRefCount == 0 );

	DNASSERT( m_COMInterface.m_pCOMVtbl == NULL );
	m_COMInterface.m_pCOMVtbl = pVtbl;

	//
	// attempt to initialize shutdown event
	//
	DNASSERT( m_hShutdownEvent == NULL );
	m_hShutdownEvent = CreateEvent( NULL,		// pointer to security (none)
									TRUE,		// manual reset
									TRUE,		// start signalled (so close can be called without any endpoints being created)
									NULL		// pointer to name (none)
									);
	if ( m_hShutdownEvent == NULL )
	{
		DWORD	dwError;


		dwError = GetLastError();
		DPF( 0, "Failed to create event for shutting down spdata!" );
		DisplayErrorCode( 0, dwError );
	}

	//
	// attempt to check for Rsip
	//
// BUGBUG: [mgere] [xbox] Removed RSIP
/*
	DNASSERT( m_fIsRsipServer == FALSE );
		CRsip	*pRsip;
		
	
		pRsip = CreateRsip();
		if ( pRsip != NULL )
		{
			m_fIsRsipServer = pRsip->RsipIsRunningOnThisMachine( &m_PublicSocketAddress );		
			pRsip->DecRef();
			pRsip = NULL;
		}
*/	
	
	DNASSERT( m_fLockInitialized == FALSE );
	DNASSERT( m_fSocketPortDataLockInitialized == FALSE );
	DNASSERT( m_fSocketPortListInitialized == FALSE );
	DNASSERT( m_fInterfaceGlobalsInitialized == FALSE );
	DNASSERT( m_fDefaultAddressesBuilt == FALSE );

	hr = m_HandleTables.Initialize();
	if ( hr != DPN_OK )
	{
		DPF( 0, "Failed to initialize handle table!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}
	m_fHandleTableInitialized = TRUE;
	
	//
	// initialize internal critical sections
	//
	if ( DNInitializeCriticalSection( &m_Lock ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Problem initializing main critical section!" );
		goto Failure;
	}
	DebugSetCriticalSectionRecursionCount( &m_Lock, 0 );
	m_fLockInitialized = TRUE;

	if ( DNInitializeCriticalSection( &m_SocketPortDataLock ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Problem initializing SocketPortDataLock critical section!" );
		goto Failure;
	}
	DebugSetCriticalSectionRecursionCount( &m_SocketPortDataLock, 0 );
	m_fSocketPortDataLockInitialized = TRUE;

	//
	// initialize hash table for socket ports with 64 etries and a multiplier of 32
	//
	if ( m_ActiveSocketPortList.Initialize( 6, 5 ) == FALSE )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Could not initialize socket port list!" );
		goto Failure;
	}
	m_fSocketPortListInitialized = TRUE;

	//
	// get a thread pool
	//
	DNASSERT( m_pThreadPool == NULL );
	hr = InitializeInterfaceGlobals( this );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Failed to create thread pool!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}
	m_fInterfaceGlobalsInitialized = TRUE;

	//
	// build default addresses
	//
	hr = BuildDefaultAddresses();
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem building default addresses!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}
	m_fDefaultAddressesBuilt = TRUE;

Exit:
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem with CSPData::Initialize" );
		DisplayDNError( 0, hr );
	}

	return	hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::Shutdown - shut down this set of SP data
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::Shutdown"

void	CSPData::Shutdown( void )
{
	BOOL	fLooping;


	//
	// Unbind this interface from the globals.  This will cause a closure of all
	// of the I/O which will release endpoints, socket ports and then this data.
	//
	if ( m_fInterfaceGlobalsInitialized != FALSE )
	{
		DeinitializeInterfaceGlobals( this );
		DNASSERT( GetThreadPool() != NULL );
		m_fInterfaceGlobalsInitialized = FALSE;
	}

	SetState( SPSTATE_CLOSING );
	
	DNASSERT( m_hShutdownEvent != NULL );
	
	fLooping = TRUE;
	while ( fLooping != FALSE )
	{
		switch ( WaitForSingleObjectEx( m_hShutdownEvent, INFINITE, TRUE ) )
		{
			case WAIT_OBJECT_0:
			{
				fLooping = FALSE;
				break;
			}

			case WAIT_IO_COMPLETION:
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

	if ( CloseHandle( m_hShutdownEvent ) == FALSE )
	{
		DWORD	dwError;


		dwError = GetLastError();
		DPF( 0, "Failed to close shutdown event!" );
		DisplayErrorCode( 0, dwError );
	}
	m_hShutdownEvent = NULL;

	if ( DP8SPCallbackInterface() != NULL)
	{
		IDP8SPCallback_Release( DP8SPCallbackInterface() );
		memset( &m_InitData, 0x00, sizeof( m_InitData ) );
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::Deinitialize - deinitialize
//
// Entry:		Nothing
//
// Exit:		Nothing
//
// Note:	This function assumes that someone else is preventing reentry.
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::Deinitialize"

void	CSPData::Deinitialize( void )
{
	CBilink	*pTempBilink;


	DPF( 9, "Entering CSPData::Deinitialize" );

	//
	// return any outstanding adatper entries
	//
	pTempBilink = m_ActiveAdapterList.GetNext();
	while ( pTempBilink != &m_ActiveAdapterList )
	{
		CAdapterEntry	*pAdapterEntry;


		pAdapterEntry = CAdapterEntry::AdapterEntryFromAdapterLinkage( pTempBilink );
		pTempBilink = pTempBilink->GetNext();
		
		pAdapterEntry->RemoveFromAdapterList();
		pAdapterEntry->DecRef();
	}

	if ( m_fDefaultAddressesBuilt != FALSE )
	{
		FreeDefaultAddresses();
		m_fDefaultAddressesBuilt = FALSE;
	}

	//
	// release our reference to the global SP objects
	//
	if ( m_fInterfaceGlobalsInitialized != FALSE )
	{
		DeinitializeInterfaceGlobals( this );
		DNASSERT( GetThreadPool() != NULL );
		m_fInterfaceGlobalsInitialized = FALSE;
	}

	//
	// clean up lists and pools
	//
	if ( m_fSocketPortListInitialized != FALSE )
	{
		if ( m_ActiveSocketPortList.IsEmpty() == FALSE )
		{
			CSocketPort				*pSocketPort;

			DPF( 1, "Attempt to close interface with active connections!" );

			//
			// All of the threads have stopped so we can force close the socket ports
			// by cancelling all IO requests and unbinding all endpoints.
			//
			while ( m_ActiveSocketPortList.RemoveLastEntry( &pSocketPort ) != FALSE )
			{
				DNASSERT( pSocketPort != NULL );

				//
				// disconnect all endpoints
				//
				pSocketPort->DisconnectAllEndpoints();
			}
		}
		
		m_ActiveSocketPortList.Deinitialize();
		m_fSocketPortListInitialized = FALSE;
	}

	if ( m_fSocketPortDataLockInitialized != FALSE )
	{
		DNDeleteCriticalSection( &m_SocketPortDataLock );
		m_fSocketPortDataLockInitialized = FALSE;
	}

	if ( m_fLockInitialized != FALSE )
	{
		DNDeleteCriticalSection( &m_Lock );
		m_fLockInitialized = FALSE;
	}

	if ( m_fHandleTableInitialized != FALSE )
	{
		m_HandleTables.Deinitialize();
		m_fHandleTableInitialized = FALSE;
	}

	SetState( SPSTATE_UNINITIALIZED );
	m_fIsRsipServer = FALSE;
	memset( &m_InitData, 0x00, sizeof( m_InitData ) );
	memset( &m_COMInterface, 0x00, sizeof( m_COMInterface ) );
	
	DPF( 9, "Leaving CSPData::Deinitialize" );

	if ( GetThreadPool() != NULL )
	{
		GetThreadPool()->DecRef();
		SetThreadPool( NULL );
	}

	if ( m_hShutdownEvent != NULL )
	{
		if ( CloseHandle( m_hShutdownEvent ) == FALSE )
		{
			DWORD	dwError;


			dwError = GetLastError();
			DPF( 0, "Failed to close shutdown handle!" );
			DisplayErrorCode( 0, dwError );
		}

		m_hShutdownEvent = NULL;
	}

	return;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::SetCallbackData - set data for SP callbacks to application
//
// Entry:		Pointer to initialization data
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::SetCallbackData"

void	CSPData::SetCallbackData( const SPINITIALIZEDATA *const pInitData )
{
	DNASSERT( pInitData != NULL );

	DNASSERT( pInitData->dwFlags == 0 );
	m_InitData.dwFlags = pInitData->dwFlags;

	DNASSERT( pInitData->pIDP != NULL );
	m_InitData.pIDP = pInitData->pIDP;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::BindEndpoint - unbind an endpoint to a socket port
//
// Entry:		Pointer to endpoint
//				Pointer to IDirectPlay8Address for socket port
//				Pointer to CSocketAddress for socket port
//				Gateway bind type
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::BindEndpoint"

HRESULT	CSPData::BindEndpoint( CEndpoint *const pEndpoint,
							   IDirectPlay8Address *const pDeviceAddress,
							   const CSocketAddress *const pSocketAddress,
							   const GATEWAY_BIND_TYPE GatewayBindType )
{
	HRESULT			hr;
	CSocketAddress	*pDeviceSocketAddress;
	CSocketPort		*pSocketPort;
	BOOL			fSocketCreated;
	BOOL			fExtraEndpointReferenceAdded;
	BOOL			fSocketPortDataLocked;
	BOOL			fAdapterEntryCreated;
	BOOL			fSocketPortInActiveList;
	BOOL			fBindReferenceAdded;
	CBilink			*pAdapterBilink;
	CAdapterEntry	*pAdapterEntry;


	DNASSERT( pEndpoint != NULL );
	DNASSERT( ( pDeviceAddress != NULL ) || ( pSocketAddress != NULL ) );

	//
	// initialize
	//
	hr = DPN_OK;
	pDeviceSocketAddress = NULL;	
	pSocketPort = NULL;
	fSocketCreated = FALSE;
	fExtraEndpointReferenceAdded = FALSE;
	fSocketPortDataLocked = FALSE;
	fAdapterEntryCreated = FALSE;
	fSocketPortInActiveList = FALSE;
	fBindReferenceAdded = FALSE;
	pAdapterEntry = NULL;
	
	//
	// create and initialize a device address to be used for this socket port
	//
	pDeviceSocketAddress = GetNewAddress();
	if ( pDeviceSocketAddress == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Failed to allocate address for new socket port!" );
		goto Failure;
	}

	//
	// Initialize the socket address with the provided base addresses.  If there
	// is a public address, override the base address.
	//
	if ( pDeviceAddress != NULL )
	{
		DNASSERT( pSocketAddress == NULL );
		hr = pDeviceSocketAddress->SocketAddressFromDP8Address( pDeviceAddress, SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT );
		if ( hr != DPN_OK )
		{
			DPF( 0, "Failed to parse device address!" );
			DisplayDNError( 0, hr );
			goto Failure;
		}
	}
	else
	{
		DNASSERT( pSocketAddress != NULL );
		pDeviceSocketAddress->CopyAddressSettings( *pSocketAddress );
	}

	LockSocketPortData();
	fSocketPortDataLocked = TRUE;	

	//
	// Find the base adapter entry for this network address.  If none is found,
	// create a new one.  If a new one cannot be created, fail.
	//
	pAdapterBilink = m_ActiveAdapterList.GetNext();
	while ( pAdapterBilink != &m_ActiveAdapterList )
	{
		CAdapterEntry	*pTempAdapterEntry;
	
		
		pTempAdapterEntry = CAdapterEntry::AdapterEntryFromAdapterLinkage( pAdapterBilink );
		if ( pDeviceSocketAddress->CompareToBaseAddress( pTempAdapterEntry->BaseAddress() ) == 0 )
		{
			DNASSERT( pAdapterEntry == NULL );
			pAdapterEntry = pTempAdapterEntry;
		}
	
		pAdapterBilink = pAdapterBilink->GetNext();
	}

	if ( pAdapterEntry == NULL )
	{
		pAdapterEntry = CreateAdapterEntry();
		if ( pAdapterEntry == NULL )
		{
			hr = DPNERR_OUTOFMEMORY;
			DPF( 0, "Failed to create a new adapter entry!" );
			goto Failure;
		}
		fAdapterEntryCreated = TRUE;
	
		pAdapterEntry->SetBaseAddress( pDeviceSocketAddress->GetAddress() );
		pAdapterEntry->AddToAdapterList( &m_ActiveAdapterList );
	}

	//
	// Start Rsip on IP interface if this is the first time this adapter is
	// being referenced.  If Rsip doesn't start it's not a major failure.
	//
//BUGBUG: [mgere] [xbox] Remove RSIP support.
/*
	if ( fAdapterEntryCreated != FALSE )
	{
		HRESULT	hTempResult;


		hTempResult = pAdapterEntry->StartRsip( pDeviceSocketAddress->GetAddress(), GetThreadPool(), IsRsipServer() );
		if ( hTempResult != DPN_OK )
		{
			DPF( 0, "Failed to start Rsip on adapter entry!" );
			DisplayDNError( 0, hTempResult );
		}
	}
*/

	//
	// if no port is specified, check the list of active adapters for a matching
	// base address and reuse that CSocketPort.
	//
	if ( pDeviceSocketAddress->GetPort() == ANY_PORT )
	{
		if ( pAdapterEntry->SocketPortList()->IsEmpty() == FALSE )
		{
			pSocketPort = CSocketPort::SocketPortFromBilink( pAdapterEntry->SocketPortList()->GetNext() );
			DNASSERT( pSocketPort != NULL );
		}
	}

	//
	// If a socket port has not been found, attempt to look it up by network
	// address.  If that fails, attempt to create a new socket port.
	//
	if ( pSocketPort == NULL )
	{
		if ( m_ActiveSocketPortList.Find( pDeviceSocketAddress, &pSocketPort ) == FALSE )
		{
			CSocketPort	*pDuplicateSocket;


			//
			// No socket port found.  Create a new one, initialize it and attempt
			// to add it to the list (may result in a duplicate).  Whatever happens
			// there will be a socket port to bind the endpoint to.  Save the
			// reference on the CSocketPort from the call to 'Create' until the
            // socket port is removed from the active list.
			//

			UnlockSocketPortData();
			fSocketPortDataLocked = FALSE;

            pDuplicateSocket = NULL;

    	    DNASSERT( pSocketPort == NULL );
    	    pSocketPort = CreateSocketPort();
    	    if ( pSocketPort == NULL )
    	    {
    	    	hr = DPNERR_OUTOFMEMORY;
    	    	DPF( 0, "Failed to create new socket port!" );
    	    	goto Failure;
    	    }
    	    fSocketCreated = TRUE;

			//
			// Everything up to this point has been working with the expected
			// network address for this socket.  If this is the Rsip host, the
			// network socket needs to be bound to 'all adapters', not the
			// expected address (which is currently the public adapter address).
			//
			if ( GetPublicSocketAddress() != NULL )
			{
				pDeviceSocketAddress->InitializeWithAnyAddress();
			}

    	    hr = pSocketPort->Initialize( this, pDeviceSocketAddress );
    	    if ( hr != DPN_OK )
    	    {
    	    	DPF( 0, "Failed to initialize new socket port!" );
    	    	DisplayDNError( 0, hr );
    	    }
    	    pDeviceSocketAddress = NULL;

			pSocketPort->SetAdapterEntry( pAdapterEntry );
    	
			hr = pSocketPort->BindToNetwork( GatewayBindType );
    	    if ( hr != DPN_OK )
    	    {
				pSocketPort->SetAdapterEntry( NULL );

    	    	DPF( 0, "Failed to bind new socket port to network!" );
    	    	DisplayDNError( 0, hr );
    	    	goto Failure;
    	    }
    	    pSocketPort->EndpointAddRef();
    	    fExtraEndpointReferenceAdded = TRUE;

    	    LockSocketPortData();
			fSocketPortDataLocked = TRUE;
			
			//
			// The only way to get here is to have the socket bound to the
			// network.  The socket can't be bound twice, if there was a
			// race to bind the socket, Winsock would have decided which
			// thread lost and failed 'BindToNetwork'.
			//
			DNASSERT( m_ActiveSocketPortList.Find( pSocketPort->GetNetworkAddress(), &pDuplicateSocket ) == FALSE );
   	    	if ( m_ActiveSocketPortList.Insert( pSocketPort->GetNetworkAddress(), pSocketPort ) == FALSE )
   	    	{
                   hr = DPNERR_OUTOFMEMORY;
   	    		DPF( 0, "Could not add new socket port to list!" );
   	    		goto Failure;
   	    	}

			pSocketPort->AddToActiveList( pAdapterEntry->SocketPortList() );
			fSocketPortInActiveList = TRUE;
		}
	}
	
	//
	// bind the endpoint to whatever socketport we have
	//
	DNASSERT( pSocketPort != NULL );
	pSocketPort->EndpointAddRef();
	fBindReferenceAdded = TRUE;

	switch ( pEndpoint->GetType() )
	{
		case ENDPOINT_TYPE_CONNECT:
		case ENDPOINT_TYPE_ENUM:
		{
			pSocketPort->MungeAddressWithPAST( pEndpoint->GetWritableRemoteAddressPointer() );
			break;
		}

		default:
		{
			break;
		}
	}
	
	hr = pSocketPort->BindEndpoint( pEndpoint );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Failed to bind endpoint!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}
	
Exit:
	if ( fExtraEndpointReferenceAdded != FALSE )
	{
		DNASSERT( pSocketPort != NULL );
		pSocketPort->EndpointDecRef();
		fExtraEndpointReferenceAdded = FALSE;
	}

	if ( fSocketPortDataLocked != FALSE )
	{
		UnlockSocketPortData();
		fSocketPortDataLocked = FALSE;
	}
	
	if ( pDeviceSocketAddress != NULL )
	{
		ReturnAddress( pDeviceSocketAddress );
		pDeviceSocketAddress = NULL;
	}
	
	return	hr;

Failure:
	//
	// If we're failing and cleanup will require removal of some resources.
	// This requires the socket port data lock.
	//
	if ( fSocketPortDataLocked == FALSE )
	{
		LockSocketPortData();
		fSocketPortDataLocked = TRUE;
	}

	if ( fSocketPortInActiveList != FALSE )
	{
		pSocketPort->RemoveFromActiveList();
		fSocketPortInActiveList = FALSE;
	}

	if ( pSocketPort != NULL )
	{
		if ( fBindReferenceAdded != FALSE )
		{
			pSocketPort->EndpointDecRef();
			fBindReferenceAdded = FALSE;
		}

		if ( fExtraEndpointReferenceAdded != FALSE )
		{
			pSocketPort->EndpointDecRef();
			fExtraEndpointReferenceAdded = FALSE;
		}
	
		if ( fSocketCreated != FALSE )
		{
			pSocketPort->DecRef();
			fSocketCreated = FALSE;
			pSocketPort = NULL;
		}
	}

	if ( fAdapterEntryCreated != FALSE )
	{
		if ( pAdapterEntry->SocketPortList()->IsEmpty() != FALSE )
		{
			pAdapterEntry->RemoveFromAdapterList();
		}
		
		pAdapterEntry->DecRef();
		pAdapterEntry = NULL;
		fAdapterEntryCreated = FALSE;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::UnbindEndpoint - unbind an endpoint from a socket port
//
// Entry:		Pointer to endpoint
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::UnbindEndpoint"

void	CSPData::UnbindEndpoint( CEndpoint *const pEndpoint )
{
	CSocketPort	*pSocketPort;
	BOOL		fCleanUpSocketPort;


	DNASSERT( pEndpoint != NULL );

	//
	// initialize
	//
	pSocketPort = pEndpoint->GetSocketPort();
	fCleanUpSocketPort = FALSE;

	LockSocketPortData();
	
	pSocketPort->UnbindEndpoint( pEndpoint );
	if ( pSocketPort->EndpointDecRef() == 0 )
	{
		m_ActiveSocketPortList.Remove( pSocketPort->GetNetworkAddress() );
		fCleanUpSocketPort = TRUE;
		pSocketPort->RemoveFromActiveList();
		pSocketPort->SetAdapterEntry( NULL );
	}

	UnlockSocketPortData();

	if ( fCleanUpSocketPort != FALSE )
	{
		pSocketPort->DecRef();
		fCleanUpSocketPort = FALSE;
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::GetNewEndpoint - get a new endpoint
//
// Entry:		Nothing
//
// Exit:		Pointer to new endpoint
//				NULL = out of memory
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::GetNewEndpoint"

CEndpoint	*CSPData::GetNewEndpoint( void )
{
	HRESULT		hTempResult;
	CEndpoint	*pEndpoint;
	HANDLE		hEndpoint;
	ENDPOINT_POOL_CONTEXT	PoolContext;

	
	//
	// initialize
	//
	pEndpoint = NULL;
	hEndpoint = INVALID_HANDLE_VALUE;
	memset( &PoolContext, 0x00, sizeof( PoolContext ) );

	PoolContext.pSPData = this;
			pEndpoint = CreateIPEndpoint( &PoolContext );
	
	if ( pEndpoint == NULL )
	{
		DPF( 0, "Failed to create endpoint!" );
		goto Failure;
	}
	
	m_HandleTables.Lock();
	hTempResult = m_HandleTables.CreateHandle( &hEndpoint, pEndpoint );
	m_HandleTables.Unlock();
	
	if ( hTempResult != DPN_OK )
	{
		DNASSERT( hEndpoint == INVALID_HANDLE_VALUE );
		DPF( 0, "Failed to create endpoint handle!" );
		DisplayDNError( 0, hTempResult );
		goto Failure;
	}

	pEndpoint->SetHandle( hEndpoint );
	pEndpoint->AddCommandRef();
	pEndpoint->DecRef();

Exit:
	return	pEndpoint;

Failure:
	if ( hEndpoint != INVALID_HANDLE_VALUE )
	{
		m_HandleTables.Lock();
		m_HandleTables.InvalidateHandle( hEndpoint );
		m_HandleTables.Unlock();
	
		hEndpoint = INVALID_HANDLE_VALUE;
	}

	if ( pEndpoint != NULL )
	{
		pEndpoint->DecRef();
		pEndpoint = NULL;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::EndpointFromHandle - get endpoint from handle
//
// Entry:		Handle
//
// Exit:		Pointer to endpoint
//				NULL = invalid handle
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::EndpointFromHandle"

CEndpoint	*CSPData::EndpointFromHandle( const HANDLE hEndpoint )
{
	CEndpoint	*pEndpoint;


	pEndpoint = NULL;
	m_HandleTables.Lock();
	
	pEndpoint = static_cast<CEndpoint*>( m_HandleTables.GetAssociatedData( hEndpoint ) );
	if ( pEndpoint != NULL )
	{
		pEndpoint->AddCommandRef();
	}
	
	m_HandleTables.Unlock();

	return	pEndpoint;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::CloseEndpointHandle - close endpoint handle
//
// Entry:		Poiner to endpoint
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::CloseEndpointHandle"

void	CSPData::CloseEndpointHandle( CEndpoint *const pEndpoint )
{
	HANDLE	Handle;
	BOOL	fCloseReturn;


	DNASSERT( pEndpoint != NULL );
	Handle = pEndpoint->GetHandle();
	
	m_HandleTables.Lock();
	fCloseReturn = m_HandleTables.InvalidateHandle( Handle );
	m_HandleTables.Unlock();

	if ( fCloseReturn != FALSE )
	{
		pEndpoint->DecCommandRef();
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::GetEndpointAndCloseHandle - get endpoint from handle and close the
//		handle
//
// Entry:		Handle
//
// Exit:		Pointer to endpoint (it needs a call to 'DecCommandRef' when done)
//				NULL = invalid handle
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::GetEndpointAndCloseHandle"

CEndpoint	*CSPData::GetEndpointAndCloseHandle( const HANDLE hEndpoint )
{
	CEndpoint	*pEndpoint;
	BOOL	fCloseReturn;


	//
	// initialize
	//
	pEndpoint = NULL;
	fCloseReturn = FALSE;
	m_HandleTables.Lock();
	
	pEndpoint = static_cast<CEndpoint*>( m_HandleTables.GetAssociatedData( hEndpoint ) );
	if ( pEndpoint != NULL )
	{
		pEndpoint->AddRef();
		pEndpoint->AddCommandRef();
		fCloseReturn = m_HandleTables.InvalidateHandle( hEndpoint );
		DNASSERT( fCloseReturn != FALSE );
	}
	
	m_HandleTables.Unlock();

	if ( pEndpoint != NULL )
	{
		pEndpoint->DecCommandRef();
	}
	
	return	pEndpoint;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::SetBufferSizeOnAllSockets - set buffer size on all sockets
//
// Entry:		New buffer size
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::SetWinsockBufferSizeOnAllSockets"
void	CSPData::SetWinsockBufferSizeOnAllSockets( const INT iBufferSize )
{
	CBilink		*pAdapterEntryLink;

	
	LockSocketPortData();
	
	pAdapterEntryLink = m_ActiveAdapterList.GetNext();
	while ( pAdapterEntryLink != &m_ActiveAdapterList )
	{
		CAdapterEntry	*pAdapterEntry;
		CBilink			*pSocketPortList;


		pAdapterEntry = CAdapterEntry::AdapterEntryFromAdapterLinkage( pAdapterEntryLink );
		pSocketPortList = pAdapterEntry->SocketPortList()->GetNext();
		while ( pSocketPortList != pAdapterEntry->SocketPortList() )
		{
			CSocketPort	*pSocketPort;


			pSocketPort = CSocketPort::SocketPortFromBilink( pSocketPortList );
			pSocketPort->SetWinsockBufferSize( iBufferSize );

			pSocketPortList = pSocketPortList->GetNext();
		}

		pAdapterEntryLink = pAdapterEntryLink->GetNext();
	}
	
	UnlockSocketPortData();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::BuildDefaultAddresses - construct default addresses
//
// Entry:		Nothing
//
// Exit:		Nothing
//
// Notes:	This function is initializing with default values that should always
//			work.  If this function asserts, fix it!
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::BuildDefaultAddresses"

HRESULT	CSPData::BuildDefaultAddresses( void )
{
	HRESULT			hr;
	CSocketAddress	*pSPAddress;


	//
	// initialize
	//
	hr = DPN_OK;

	//
	// create appropriate address
	//
	pSPAddress = GetNewAddress();
	if ( pSPAddress == NULL )
	{
		DPF( 0, "Failed to get address when building default addresses!" );
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	//
	// query for broadcast address
	//
	DNASSERT( m_pBroadcastAddress == NULL );
	m_pBroadcastAddress = pSPAddress->CreateBroadcastAddress();
	if ( m_pBroadcastAddress == NULL )
	{
		DPF( 0, "Failed to create template for broadcast address." );
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	//
	// query for listen address
	//
	DNASSERT( m_pListenAddress == NULL );
	m_pListenAddress = pSPAddress->CreateListenAddress();
	if ( m_pListenAddress == NULL )
	{
		DPF( 0, "Failed to create template for listen address." );
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	//
	// query for generic address
	//
	DNASSERT( m_pGenericAddress == NULL );
	m_pGenericAddress = pSPAddress->CreateGenericAddress();
	if ( m_pGenericAddress == NULL )
	{
		DPF( 0, "Failed to create template for generic address." );
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

Exit:
	if ( pSPAddress != NULL )
	{
		ReturnAddress( pSPAddress );
		pSPAddress = NULL;
	}

	return	hr;

Failure:
	if ( m_pGenericAddress != NULL )
	{
		IDirectPlay8Address_Release( m_pGenericAddress );
		m_pGenericAddress = NULL;
	}

	if ( m_pListenAddress != NULL )
	{
		IDirectPlay8Address_Release( m_pListenAddress );
		m_pListenAddress = NULL;
	}

	if ( m_pBroadcastAddress != NULL )
	{
		IDirectPlay8Address_Release( m_pBroadcastAddress );
		m_pBroadcastAddress = NULL;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::FreeDefaultAddresses - free default addresses
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::FreeDefaultAddresses"

void	CSPData::FreeDefaultAddresses( void )
{
	if ( m_pBroadcastAddress != NULL )
	{
		IDirectPlay8Address_Release( m_pBroadcastAddress );
		m_pBroadcastAddress = NULL;
	}

	if ( m_pListenAddress != NULL )
	{
		IDirectPlay8Address_Release( m_pListenAddress );
		m_pListenAddress = NULL;
	}

	if ( m_pGenericAddress != NULL )
	{
		IDirectPlay8Address_Release( m_pGenericAddress );
		m_pGenericAddress = NULL;
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::DestroyThisObject - destroy this object
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::DestroyThisObject"
void	CSPData::DestroyThisObject( void )
{
	Deinitialize();
	delete	this;		// maybe a little too extreme......
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::GetNewAddress - get a new address
//
// Entry:		Nothing
//
// Exit:		Pointer to new address
//				NULL = out of memory
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::GetNewAddress"

CSocketAddress	*CSPData::GetNewAddress( void )
{
	return CreateIPAddress();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CSPData::ReturnAddress - return address to list
//
// Entry:		Poiner to address
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CSPData::ReturnAddress"

void	CSPData::ReturnAddress( CSocketAddress *const pAddress )
{
	DNASSERT( pAddress != NULL );

	pAddress->Reset();

			ReturnIPAddress( static_cast<CIPAddress*>( pAddress ) );
}
//**********************************************************************

