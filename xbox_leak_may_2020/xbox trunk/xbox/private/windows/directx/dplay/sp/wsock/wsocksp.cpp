/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       WSockSP.cpp
 *  Content:	Protocol-independent APIs for the DN Winsock SP
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	10/26/98	jwo		Created it.
 *	11/1/98		jwo		Un-subclassed everything (moved it to this generic
 *						file from IP and IPX specific ones
 *  03/22/20000	jtk		Updated with changes to interface names
 *	04/22/00	mjn		Allow all flags in DNSP_GetAddressInfo()
 *  08/06/00    RichGr  IA64: Use %p format specifier in DPFs for 32/64-bit pointers and handles.
 ***************************************************************************/

#include "wsockspi.h"

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_WSOCK

//**********************************************************************
// Constant definitions
//**********************************************************************

//
// maximum bandwidth in bits per second
//
#define	UNKNOWN_BANDWIDTH	0

#define WAIT_FOR_CLOSE_TIMEOUT 30000		// milliseconds

#define	ADDRESS_ENCODE_KEY	0

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
/*
 *
 *	DNSP_Initialize initializes the instance of the SP.  It must be called
 *		at least once before using any other functions.  Further attempts
 *		to initialize the SP are ignored.
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_Initialize"

STDMETHODIMP DNSP_Initialize( IDP8ServiceProvider *pThis, SPINITIALIZEDATA *pData )
{
	HRESULT			hr;
	CSPData			*pSPData;
	SOCKET			TestSocket;

	DPF(9, "->DNSP_Initialize (0x%p, 0x%p)\n", pThis, pData);
	DNASSERT( pThis != NULL );
	DNASSERT( pData != NULL );

	//
	// initialize
	//
	hr = DPN_OK;
	TestSocket = INVALID_SOCKET;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	//
	// prevent anyone else from messing with this interface
	//
	pSPData->Lock();

	//
	// check interface state
	//
	switch ( pSPData->GetState() )
	{
		//
		// uninitialized interface, we can initialize it
		//
		case SPSTATE_UNINITIALIZED:
		{
			break;
		}

		//
		// other state
		//
		case SPSTATE_INITIALIZED:
		case SPSTATE_CLOSING:
		default:
		{
			hr = DPNERR_ALREADYINITIALIZED;
			DPF( 0, "Attempted to reinitialize interface!" );
			DNASSERT( FALSE );

			goto Exit;
		}
	}


	//
	// Before we get too far, check for the existance of this protocol by
	// attempting to create a socket.
	//
			TestSocket = p_socket( AF_INET, SOCK_DGRAM, IPPROTO_IP );
	
	if ( TestSocket == INVALID_SOCKET )
	{
		DPF( 0, "Creating socket failed, is the protocol installed?" );
		hr = DPNERR_UNSUPPORTED;
		goto Failure;
	}
	
	//
	// remember our init data
	//
	pSPData->SetCallbackData( pData );

	//
	// Success from here on in
	//
	IDP8SPCallback_AddRef( pSPData->DP8SPCallbackInterface() );
	pSPData->SetState( SPSTATE_INITIALIZED );

	pSPData->Unlock();

	IDP8ServiceProvider_AddRef( pThis );

Exit:
	if ( TestSocket != INVALID_SOCKET )
	{
		p_closesocket( TestSocket );
		TestSocket = INVALID_SOCKET;
	}

	DPF(9, "<-DNSP_Initialize(0x%x)", hr);
	return hr;

Failure:
	pSPData->Unlock();
	goto Exit;
}
//**********************************************************************


//**********************************************************************
/*
 *
 *	DNSP_Close is the opposite of Initialize.  Call it when you're done
 *		using the SP
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_Close"

STDMETHODIMP DNSP_Close( IDP8ServiceProvider *pThis )
{
	HRESULT		hr;
	CSPData		*pSPData;
	
	DNASSERT( pThis != NULL );

	//
	// initialize
	//
	hr = DPN_OK;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

			//
			// release our ref to the DPLAY callbacks
			//
			pSPData->Shutdown();
			

			IDP8ServiceProvider_Release( pThis );

	return hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNSP_AddRef - increment reference count
//
// Entry:		Pointer to interface
//
// Exit:		New reference count
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_AddRef"

STDMETHODIMP_(ULONG) DNSP_AddRef( IDP8ServiceProvider *pThis )
{	
	CSPData	*pSPData;


	DNASSERT( pThis != NULL );
	pSPData = CSPData::SPDataFromCOMInterface( pThis );
	return	pSPData->AddRef();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNSP_Release - decrement reference count
//
// Entry:		Pointer to interface
//
// Exit:		New reference count
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_Release"

STDMETHODIMP_(ULONG) DNSP_Release( IDP8ServiceProvider *pThis )
{
	CSPData	*pSPData;

	
	DNASSERT( pThis != NULL );
	pSPData = CSPData::SPDataFromCOMInterface( pThis );
	return	pSPData->DecRef();
}
//**********************************************************************

//**********************************************************************
/*
 *
 *	DNSP_EnumQuery  sends out the
 *		specified data to the specified address.  If the SP is unable to
 *		determine the address based on the input params, it checks to see
 *		if it's allowed to put up a dialog querying the user for address
 *		info.  If it is, it queries the user for address info.
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_EnumQuery"

STDMETHODIMP DNSP_EnumQuery( IDP8ServiceProvider *pThis, SPENUMQUERYDATA *pEnumQueryData)
{
	HRESULT					hr;
	HRESULT					hTempResult;
	CEndpoint				*pEndpoint;
	CCommandData			*pCommand;
	CSocketAddress			*pLocalAddress;
//	CSocketPort				*pSocketPort;
	BOOL					fInterfaceReferenceAdded;
	BOOL					fEndpointOpen;
	IDirectPlay8Address		*pDeviceAddress;
	IDirectPlay8Address		*pHostAddress;
	IDirectPlay8Address		*pSPOnlyAddress;
	CSPData					*pSPData;


	DPF(9, "\nEnter:\tDNSP_EnumQuery (0x%p, 0x%p)", pThis, pEnumQueryData);

	DNASSERT( pThis != NULL );
	DNASSERT( pEnumQueryData != NULL );
	DNASSERT( ( pEnumQueryData->dwFlags & ~( DPNSPF_OKTOSHOWSTATUS | DPNSPF_NOBROADCASTFALLBACK ) ) == 0 );

	DBG_CASSERT( sizeof( pEnumQueryData->dwRetryInterval ) == sizeof( DWORD ) );

	//
	// initialize
	//
	hr = DPNERR_PENDING;
	pEndpoint = NULL;
	pCommand = NULL;
//	pLocalAddress = NULL;
//	pSocketPort = NULL;
	fInterfaceReferenceAdded = FALSE;
	fEndpointOpen = FALSE;
	pDeviceAddress = NULL;
	pHostAddress = NULL;
	pSPOnlyAddress = NULL;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );
	DNASSERT( pSPData != NULL );

	pEnumQueryData->hCommand = NULL;
	pEnumQueryData->dwCommandDescriptor = NULL_DESCRIPTOR;

	DumpAddress( 8, "Enuming on device:", pEnumQueryData->pAddressDeviceInfo );
	DumpAddress( 8, "Enum destination:", pEnumQueryData->pAddressHost );

	//
	// duplicate device address
	//
	hTempResult = IDirectPlay8Address_Duplicate( pEnumQueryData->pAddressDeviceInfo, &pDeviceAddress );
	if ( hTempResult != DPN_OK )
	{
		hr = hTempResult;
		DPF( 0, "Failed to duplicate device address!" );
		goto Failure;
	}

//	//
//	// take the device address and fill in any defaults
//	//
//	hTempResult = ApplyDeviceAddressDefaults( pEnumQueryData->pAddressDeviceInfo,
//	    									  pSPData->GetGenericDeviceAddress(),
//	    									  &pDeviceAddress );
//	if ( hTempResult != DPN_OK )
//	{
//	    hr = hTempResult;
//	    goto Failure;
//	}

	//
	// Use the host address supplied by the user.  If no address was supplied,
	// use the broadcast address only if we're not allowed to display a dialog
	// on TCP, or if we're using IPX.  If no address was supplied and we're
	// allowed to display a dialog build a base address with just an SP guid.
	//
	if ( pEnumQueryData->pAddressHost != NULL )
	{
		pHostAddress = pEnumQueryData->pAddressHost;
	}
	else
	{
			DPF( 8, "No host address supplied for enum, using broadcast." );
			pHostAddress = pSPData->GetBroadcastHostAddress();
	}

	//
	// check SP state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		// provider is initialized
		case SPSTATE_INITIALIZED:
		{
			//
			// no problem
			//
			DNASSERT( hr == DPNERR_PENDING );
			IDP8ServiceProvider_AddRef( pThis );
			fInterfaceReferenceAdded = TRUE;

			break;
		}

		// provider is uninitialized
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "EnumQuery called on uninitialized SP!" );
			goto Failure;

			break;
		}

		// provider is closing
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "EnumQuery called while SP closing!" );
			goto Failure;

			break;
		}

		// unknown
		default:
		{
			DNASSERT( FALSE );
			hr = DPNERR_GENERIC;
			goto Failure;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPNERR_PENDING )
	{
		goto Failure;
	}

//    //
//    // determine local adapter address
//    //
//    pLocalAddress = pSPData->GetNewAddress();
//    if ( pLocalAddress == NULL )
//    {
//    	hr = DPNERR_OUTOFMEMORY;
//    	DPF( 0, "Out of memory when attempting to get local address in DNSP_EnumQuery!" );
//    	goto Failure;
//    }
//
//    hr = pLocalAddress->SocketAddressFromDP8Address( pDeviceAddress, SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT );
//    if ( hr != DPN_OK )
//    {
//        DPF( 0, "Problem parsing adapter address in DNSP_EnumQuery!" );
//        DisplayDNError( 0, hr );
//        goto Failure;
//    }
//
//	//
//	// locate the SocketPort, create one if none are found
//	//
//	hr = pSPData->FindOrCreateSocketPort( pLocalAddress, &pSocketPort, SOCKET_TYPE_ENUM_OR_CONNECT );
//	if ( hr != DPN_OK )
//	{
//		DPF( 0, "Problem attempting to get socket port in DNSP_EnumQuery!" );
//		DisplayDNError( 0, hr );
//		goto Failure;
//	}
//	DNASSERT( pSocketPort != NULL );

	//
	// create and new endpoint
	//
	pEndpoint = pSPData->GetNewEndpoint();
	if ( pEndpoint == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Cannot create new endpoint in DNSP_EnumQuery!" );
		goto Failure;
	}

	//
	// get new command and initialize it
	//
	pCommand = CreateCommand();
	if ( pCommand == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Cannot get command handle for DNSP_EnumQuery!" );
		goto Failure;
	}

	pEnumQueryData->hCommand = pCommand;
	pEnumQueryData->dwCommandDescriptor = pCommand->GetDescriptor();
	pCommand->SetType( COMMAND_TYPE_ENUM_QUERY );
	pCommand->SetState( COMMAND_STATE_PENDING );
	pCommand->SetEndpoint( pEndpoint );

	//
	// open endpoint with outgoing address
	//
	fEndpointOpen = TRUE;
	hr = pEndpoint->Open( ENDPOINT_TYPE_ENUM, pHostAddress, NULL );
	switch ( hr )
	{
		//
		// Incomplete address passed in, query user for more information if
		// we're allowed.  If we're on IPX (no dialog available), don't attempt
		// to display the dialog, skip to checking for broadcast fallback.
		// Since we don't have a complete address at this time,
		// don't bind this endpoint to the socket port!
		//
		case DPNERR_INCOMPLETEADDRESS:
		{
				if ( ( pEnumQueryData->dwFlags & DPNSPF_NOBROADCASTFALLBACK ) == 0 )
				{
					//
					// we're OK, reset the destination address and reset the
					// function return to 'pending'
					//
					pEndpoint->ReinitializeWithBroadcast();
					hr = DPNERR_PENDING;
					goto SubmitDelayedCommand;
				}
				else
				{
					goto Failure;
				}
			break;
		}

		//
		// address conversion was fine, copy connect data and finish connection
		// on background thread.
		//
		case DPN_OK:
		{
SubmitDelayedCommand:
			//
			// Copy enum data and submit job to finish off enum.
			//
			DNASSERT( pSPData != NULL );
			pEndpoint->CopyEnumQueryData( pEnumQueryData, pDeviceAddress );
			pEndpoint->AddRef();

			hr = pSPData->GetThreadPool()->SubmitDelayedCommand( pEndpoint->EnumQueryJobCallback,
																 pEndpoint->CancelEnumQueryJobCallback,
																 pEndpoint );
			if ( hr != DPN_OK )
			{
				pEndpoint->DecRef();
				DPF( 0, "Failed to set delayed enum query!" );
				DisplayDNError( 0, hr );
				goto Failure;
			}

			//
			// this endpoint has been handed off, remove our reference
			//
			pEndpoint = NULL;
			hr = DPNERR_PENDING;
			goto Exit;

			break;
		}

		default:
		{
			//
			// this endpoint is screwed
			//
			DPF( 0, "Problem initializing endpoint in DNSP_EnumQuery!" );
			DisplayDNError( 0, hr );
			goto Failure;

			break;
		}
	}

Exit:
//	//
//	// if there was a socket port created, release our reference to it
//	//
//	if ( pSocketPort != NULL )
//	{
//	    pSocketPort->DecRef();
//	    pSocketPort = NULL;
//	}

//	//
//	// is there a temporary address to free?
//	//
//	if ( pLocalAddress != NULL )
//	{
//	    pSPData->ReturnAddress( pLocalAddress );
//	    pLocalAddress = NULL;
//	}

	if ( pDeviceAddress != NULL )
	{
		IDirectPlay8Address_Release( pDeviceAddress );
		pDeviceAddress = NULL;
	}
	
	DNASSERT( pEndpoint == NULL );

	if ( pSPOnlyAddress != NULL )
	{
		IDirectPlay8Address_Release( pSPOnlyAddress );
		pSPOnlyAddress = NULL;
	}

	if ( hr != DPNERR_PENDING )
	{
		// this command cannot complete synchronously!
		DNASSERT( hr != DPN_OK );

		DPF( 0, "Problem with DNSP_EnumQuery()" );
		DisplayDNError( 0, hr );
	}

	DPF(9, "\nLeave:\tDNSP_EnumQuery\n" );

	return hr;

Failure:
	//
	// if there's an allocated command, clean up and then
	// return the command
	//
	if ( pCommand != NULL )
	{
		pCommand->DecRef();
		pCommand = NULL;

		pEnumQueryData->hCommand = NULL;
		pEnumQueryData->dwCommandDescriptor = NULL_DESCRIPTOR;
	}

	//
	// is there an endpoint to free?
	//
	if ( pEndpoint != NULL )
	{
		if ( fEndpointOpen != FALSE )
		{
			pEndpoint->Close( hr );
			fEndpointOpen = FALSE;
		}
		
		pSPData->CloseEndpointHandle( pEndpoint );
		pEndpoint = NULL;
	}

	if ( fInterfaceReferenceAdded != FALSE )
	{
		DNASSERT( pSPData != NULL );
		IDP8ServiceProvider_Release( pThis );
		fInterfaceReferenceAdded = FALSE;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
/*
 *
 *	DNSP_EnumRespond  sends a response to an enum request by
 *		sending the specified data to the address provided (on
 *		unreliable transport).
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_EnumRespond"

STDMETHODIMP DNSP_EnumRespond( IDP8ServiceProvider *pThis, SPENUMRESPONDDATA *pEnumRespondData )
{
	HRESULT			hr;
	CEndpoint		*pEndpoint;
	WRITE_IO_DATA_POOL_CONTEXT	PoolContext;
	CWriteIOData	*pWriteData;
	CSPData			*pSPData;
	const ENDPOINT_ENUM_QUERY_CONTEXT	*pEnumQueryContext;


	DNASSERT( pThis != NULL );
	DNASSERT( pEnumRespondData != NULL );
	DNASSERT( pEnumRespondData->dwFlags == 0 );

	//
	// initialize
	//
	hr = DPNERR_PENDING;
	DBG_CASSERT( OFFSETOF( ENDPOINT_ENUM_QUERY_CONTEXT, EnumQueryData ) == 0 );
	pEnumQueryContext = reinterpret_cast<ENDPOINT_ENUM_QUERY_CONTEXT*>( pEnumRespondData->pQuery );
	pEndpoint = NULL;
	pWriteData = NULL;
	pEnumRespondData->hCommand = NULL;
	pEnumRespondData->dwCommandDescriptor = NULL_DESCRIPTOR;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );
	DNASSERT( pSPData->GetState() == SPSTATE_INITIALIZED );
	IDP8ServiceProvider_AddRef( pThis );
	
	//
	// check for valid endpoint
	//
	pEndpoint = pSPData->EndpointFromHandle( pEnumQueryContext->hEndpoint );
	if ( pEndpoint == NULL )
	{
		hr = DPNERR_INVALIDENDPOINT;
		DPF( 8, "Invalid endpoint handle in DNSP_EnumRespond" );
		goto Failure;
	}

	//
	// no need to poke at the thread pool here to lock down threads because we
	// can only really be here if there's an enum and that enum locked down the
	// thread pool.
	//

	pWriteData = pSPData->GetThreadPool()->GetNewWriteIOData( &PoolContext );
	if ( pWriteData == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Cannot get new WRITE_IO_DATA for enum response!" );
		goto Failure;
	}

	pWriteData->m_pCommand->SetType( COMMAND_TYPE_SEND );
	pWriteData->m_pCommand->SetState( COMMAND_STATE_PENDING );
	pWriteData->m_pCommand->SetEndpoint( pEndpoint );
	pWriteData->m_pCommand->SetUserContext( pEnumRespondData->pvContext );
	DNASSERT( pWriteData->m_SendCompleteAction == SEND_COMPLETE_ACTION_UNKNOWN );
	pWriteData->m_SendCompleteAction = SEND_COMPLETE_ACTION_COMPLETE_COMMAND;

	pWriteData->m_uBufferCount = pEnumRespondData->dwBufferCount;
	pWriteData->m_pBuffers = pEnumRespondData->pBuffers;
	pWriteData->m_pDestinationSocketAddress = pEnumQueryContext->pReturnAddress;

	pEnumRespondData->hCommand = pWriteData->m_pCommand;
	pEnumRespondData->dwCommandDescriptor = pWriteData->m_pCommand->GetDescriptor();

	//
	// send data
	//
	pEndpoint->SendEnumResponseData( pWriteData, pEnumQueryContext->dwEnumKey );

Exit:
	if ( pEndpoint != NULL )
	{
		pEndpoint->DecCommandRef();
		pEndpoint = NULL;
	}

	IDP8ServiceProvider_Release( pThis );

	return hr;

Failure:
	if ( pWriteData != NULL )
	{
		DNASSERT( pSPData != NULL );
		pSPData->GetThreadPool()->ReturnWriteIOData( pWriteData );

		pEnumRespondData->hCommand = NULL;
		pEnumRespondData->dwCommandDescriptor = NULL_DESCRIPTOR;

		pWriteData = NULL;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
/*
 *
 *	DNSP_Connect "connects" to the specified address.  This doesn't
 *		necessarily mean a real (TCP) connection is made.  It could
 *		just be a virtual UDP connection
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_Connect"

STDMETHODIMP DNSP_Connect( IDP8ServiceProvider *pThis, SPCONNECTDATA *pConnectData )
{
	HRESULT					hr;
	HRESULT					hTempResult;
	CEndpoint				*pEndpoint;
	CCommandData			*pCommand;
	IDirectPlay8Address		*pDeviceAddress;
	BOOL					fInterfaceReferenceAdded;
	BOOL					fEndpointOpen;
	CSPData					*pSPData;


	DPF(9, "\nEnter:\tDNSP_Connect (0x%p, 0x%p)", pThis, pConnectData);

	DNASSERT( pThis != NULL );
	DNASSERT( pConnectData != NULL );
	DNASSERT( pConnectData->pAddressHost != NULL );
	DNASSERT( ( pConnectData->dwFlags & ~( DPNSPF_OKTOSHOWSTATUS | DPNSPF_RELIABLE | DPNSPF_UNRELIABLE ) ) == 0 );

	//
	// initialize
	//
	hr = DPNERR_PENDING;
	pEndpoint = NULL;
	pCommand = NULL;
//	pLocalAddress = NULL;
//	pSocketPort = NULL;
	pDeviceAddress = NULL;
	fInterfaceReferenceAdded = FALSE;
	fEndpointOpen = FALSE;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	pConnectData->hCommand = NULL;
	pConnectData->dwCommandDescriptor = NULL_DESCRIPTOR;

	DumpAddress( 8, "Connecting on device:", pConnectData->pAddressDeviceInfo );
	DumpAddress( 8, "Connect destination:", pConnectData->pAddressHost );
	
	hTempResult = IDirectPlay8Address_Duplicate( pConnectData->pAddressDeviceInfo, &pDeviceAddress );
	if ( hTempResult != DPN_OK )
	{
		hr = hTempResult;
		DPF( 0, "Failed to duplicate device address!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

	//
	// check SP state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		// provider is initialized
		case SPSTATE_INITIALIZED:
		{
			//
			// no problem, add a reference and proceed
			//
			IDP8ServiceProvider_AddRef( pThis );
			fInterfaceReferenceAdded = TRUE;
			DNASSERT( hr == DPNERR_PENDING );
			break;
		}

		// provider is uninitialized
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "Connect called on uninitialized SP!" );

			break;
		}

		// provider is closing
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "Connect called while SP closing!" );

			break;
		}

		// unknown
		default:
		{
			DNASSERT( FALSE );
			hr = DPNERR_GENERIC;
			goto Failure;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPNERR_PENDING )
	{
		DNASSERT( hr != DPN_OK );
		goto Failure;
	}


	//
	// create and new endpoint
	//
	pEndpoint = pSPData->GetNewEndpoint();
	if ( pEndpoint == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Cannot create new endpoint in DNSP_Connect!" );
		goto Failure;
	}

	//
	// get new command and initialize it
	//
	pCommand = CreateCommand();
	if ( pCommand == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Cannot get command handle for DNSP_Connect!" );
		goto Failure;
	}

	pConnectData->hCommand = pCommand;
	pConnectData->dwCommandDescriptor = pCommand->GetDescriptor();
	pCommand->SetType( COMMAND_TYPE_CONNECT );
	pCommand->SetState( COMMAND_STATE_PENDING );
	pCommand->SetEndpoint( pEndpoint );

	//
	// open endpoint with outgoing address
	//
	fEndpointOpen = TRUE;
	hr = pEndpoint->Open( ENDPOINT_TYPE_CONNECT,
						  pConnectData->pAddressHost,
						  NULL );
	switch ( hr )
	{
		//
		// address conversion was fine, copy connect data and finish connection
		// on background thread.
		//
		case DPN_OK:
		{
			//
			// Copy connection data and submit job to finish off connection.
			// Since we're going to hand off this endpoint, add it to the
			// unbound list so we don't lose track of it.
			//
			DNASSERT( pSPData != NULL );

			pEndpoint->CopyConnectData( pConnectData, pDeviceAddress );
			pEndpoint->AddRef();

			hr = pSPData->GetThreadPool()->SubmitDelayedCommand( pEndpoint->ConnectJobCallback,
																 pEndpoint->CancelConnectJobCallback,
																 pEndpoint );
			if ( hr != DPN_OK )
			{
				pEndpoint->DecRef();
				DPF( 0, "Failed to set delayed connect!" );
				DisplayDNError( 0, hr );
				goto Failure;
			}

			//
			// this endpoint has been handed off, remove our reference to it
			//
			pEndpoint = NULL;
			hr = DPNERR_PENDING;
			goto Exit;

			break;
		}

		//
		// Incomplete address passed in, query user for more information if
		// we're allowed.  Since we don't have a complete address at this time,
		// don't bind this endpoint to the socket port!
		//
		case DPNERR_INCOMPLETEADDRESS:
		{
				goto Failure;

			break;
		}

		default:
		{
			DPF( 0, "Problem initializing endpoint in DNSP_Connect!" );
			DisplayDNError( 0, hr );
			goto Failure;

			break;
		}
	}

Exit:
	if ( pDeviceAddress != NULL )
	{
		IDirectPlay8Address_Release( pDeviceAddress );
		pDeviceAddress = NULL;
	}
	
	DNASSERT( pEndpoint == NULL );

	if ( hr != DPNERR_PENDING )
	{
		// this command cannot complete synchronously!
		DNASSERT( hr != DPN_OK );

		DPF( 0, "Problem with DNSP_Connect()" );
		DisplayDNError( 0, hr );
	}

	DPF(9, "\nLeave:\tDNSP_Connect\n" );

	return hr;

Failure:
	//
	// if there's an allocated command, clean up and then
	// return the command
	//
	if ( pCommand != NULL )
	{
		pCommand->DecRef();
		pCommand = NULL;

		pConnectData->hCommand = NULL;
		pConnectData->dwCommandDescriptor = NULL_DESCRIPTOR;
	}

	//
	// is there an endpoint to free?
	//
	if ( pEndpoint != NULL )
	{
		if ( fEndpointOpen != FALSE )
		{
			pEndpoint->Close( hr );
			fEndpointOpen = FALSE;
		}

		pSPData->CloseEndpointHandle( pEndpoint );
		pEndpoint = NULL;
	}

	if ( fInterfaceReferenceAdded != FALSE )
	{
		IDP8ServiceProvider_Release( pThis );
		fInterfaceReferenceAdded = FALSE;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
/*
 *
 *	DNSP_Disconnect disconnects an active connection
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_Disconnect"

STDMETHODIMP DNSP_Disconnect( IDP8ServiceProvider *pThis, SPDISCONNECTDATA *pDisconnectData )
{
	HRESULT		hr;
	HRESULT		hTempResult;
	CEndpoint	*pEndpoint;
	CSPData		*pSPData;

	DPF(9, "->DNSP_Disconnect (0x%p, 0x%p)\n", pThis, pDisconnectData);


	DNASSERT( pThis != NULL );
	DNASSERT( pDisconnectData != NULL );
	DNASSERT( pDisconnectData->dwFlags == 0 );
	DNASSERT( pDisconnectData->hEndpoint != INVALID_HANDLE_VALUE );
	DNASSERT( pDisconnectData->dwFlags == 0 );

	//
	// initialize
	//
	hr = DPN_OK;
	pEndpoint = NULL;
	pDisconnectData->hCommand = NULL;
	pDisconnectData->dwCommandDescriptor = NULL_DESCRIPTOR;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	//
	// no need to poke at the thread pool here because there was already a connect
	// issued and that connect should have locked down the thread pool.
	//

	//
	// check service provider state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		//
		// provider is initialized
		//
		case SPSTATE_INITIALIZED:
		{
			DNASSERT( hr == DPN_OK );
			break;
		}

		//
		// provider is uninitialized
		//
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "Disconnect called on uninitialized SP!" );
			goto Failure;

			break;
		}

		//
		// provider is closing
		//
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "Disconnect called on closing SP!" );
			goto Failure;

			break;
		}

		//
		// unknown
		//
		default:
		{
			hr = DPNERR_GENERIC;
			DNASSERT( FALSE );
			goto Failure;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Failure;
	}

	//
	// look up the endpoint and if it's found, close its handle
	//
	pEndpoint = pSPData->GetEndpointAndCloseHandle( pDisconnectData->hEndpoint );
	if ( pEndpoint == NULL )
	{
		hr = DPNERR_INVALIDENDPOINT;
		goto Failure;
	}
	
	hTempResult = pEndpoint->Disconnect( pDisconnectData->hEndpoint );
	switch ( hTempResult )
	{
		//
		// endpoint disconnected immediately
		//
		case DPNERR_PENDING:
		case DPN_OK:
		{
			break;
		}

		//
		// Other return.  Since the disconnect didn't complete, we need
		// to unlock the endpoint.
		//
		default:
		{
			DPF( 0, "Error reported when attempting to disconnect endpoint in DNSP_Disconnect!" );
			DisplayDNError( 0, hTempResult );
			DNASSERT( FALSE );

			break;
		}
	}

Exit:
	//
	// remove outstanding reference from GetEndpointHandleAndClose()
	//
	if ( pEndpoint != NULL )
	{
		pEndpoint->DecRef();
		pEndpoint = NULL;
	}

	DPF(9, "<-DNSP_Disconnect(0x%x)", hr);

	return hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
/*
 *
 *	DNSP_Listen "listens" on the specified address/port.  This doesn't
 *		necessarily mean that a true TCP socket is used.  It could just
 *		be a UDP port that's opened for receiving packets
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_Listen"

STDMETHODIMP DNSP_Listen( IDP8ServiceProvider *pThis, SPLISTENDATA *pListenData)
{
	HRESULT					hr;
	HRESULT					hTempResult;
	CEndpoint				*pEndpoint;
	CCommandData			*pCommand;
	IDirectPlay8Address		*pDeviceAddress;
	BOOL					fInterfaceReferenceAdded;
	BOOL					fEndpointOpen;
	CSPData					*pSPData;


	DPF(9, "->DNSP_Listen (0x%p, 0x%p)\n", pThis, pListenData);

	DNASSERT( pThis != NULL );
	DNASSERT( pListenData != NULL );
	DNASSERT( ( pListenData->dwFlags & ~( DPNSPF_OKTOSHOWSTATUS |
										  DPNSPF_RELIABLE |
										  DPNSPF_UNRELIABLE |
										  DPNSPF_BINDLISTENTOGATEWAY ) ) == 0 );

	//
	// initialize
	//
	hr = DPNERR_PENDING;
	pEndpoint = NULL;
	pCommand = NULL;
//	pLocalAddress = NULL;
//	pSocketPort = NULL;
	pDeviceAddress = NULL;
	fInterfaceReferenceAdded = FALSE;
	fEndpointOpen = FALSE;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	pListenData->hCommand = NULL;
	pListenData->dwCommandDescriptor = NULL_DESCRIPTOR;

	DumpAddress( 8, "Listening on device:", pListenData->pAddressDeviceInfo );

	//
	// take the device address and fill in any defaults
	//
	hTempResult = IDirectPlay8Address_Duplicate( pListenData->pAddressDeviceInfo, &pDeviceAddress );
	if ( hTempResult != DPN_OK )
	{
		hr = hTempResult;
		DPF( 0, "Failed to duplicate device address!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

	//
	// check service provider state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		//
		// provider is initialized
		//
		case SPSTATE_INITIALIZED:
		{
			IDP8ServiceProvider_AddRef( pThis );
			fInterfaceReferenceAdded = TRUE;
			DNASSERT( hr == DPNERR_PENDING );
			break;
		}

		//
		// provider is uninitialized
		//
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "Disconnect called on uninitialized SP!" );
			DNASSERT( FALSE );
			goto Failure;

			break;
		}

		//
		// provider is closing
		//
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "Disconnect called on closing SP!" );
			DNASSERT( FALSE );
			goto Failure;

			break;
		}

		//
		// unknown
		//
		default:
		{
			hr = DPNERR_GENERIC;
			DNASSERT( FALSE );
			goto Failure;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPNERR_PENDING )
	{
		goto Failure;
	}

	//
	// create and new endpoint
	//
	pEndpoint = pSPData->GetNewEndpoint();
	if ( pEndpoint == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Cannot create new endpoint in DNSP_Listen!" );
		goto Failure;
	}

	//
	// get new command and initialize it
	//
	pCommand = CreateCommand();
	if ( pCommand == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Cannot get command handle for DNSP_Listen!" );
		goto Failure;
	}

	pListenData->hCommand = pCommand;
	pListenData->dwCommandDescriptor = pCommand->GetDescriptor();
	pCommand->SetType( COMMAND_TYPE_LISTEN );
	pCommand->SetState( COMMAND_STATE_PENDING );
	pCommand->SetEndpoint( pEndpoint );

	//
	// open endpoint with outgoing address
	//
	fEndpointOpen = TRUE;
	hr = pEndpoint->Open( ENDPOINT_TYPE_LISTEN, NULL, NULL );
	switch ( hr )
	{
		//
		// address conversion was fine, copy connect data and finish connection
		// on background thread.
		//
		case DPN_OK:
		{
			//
			// Copy listen data and submit job to finish off listen.
			//
			DNASSERT( pSPData != NULL );

			pEndpoint->CopyListenData( pListenData, pDeviceAddress );
			pEndpoint->AddRef();

			hr = pSPData->GetThreadPool()->SubmitDelayedCommand( pEndpoint->ListenJobCallback,
																 pEndpoint->CancelListenJobCallback,
																 pEndpoint );
			if ( hr != DPN_OK )
			{
				pEndpoint->DecRef();
				DPF( 0, "Failed to set delayed listen!" );
				DisplayDNError( 0, hr );
				goto Failure;
			}

			//
			// this endpoint has been handed off, remove our reference to it
			//
			pEndpoint = NULL;
			hr = DPNERR_PENDING;

			break;
		}

		//
		// Incomplete address passed in, query user for more information if
		// we're allowed.  Since we don't have a complete address at this time,
		// don't bind this endpoint to the socket port!
		//
		case DPNERR_INCOMPLETEADDRESS:
		{
				goto Failure;

			break;
		}

		default:
		{
			DPF( 0, "Problem initializing endpoint in DNSP_Listen!" );
			DisplayDNError( 0, hr );
			goto Failure;

			break;
		}
	}

Exit:
	if ( pDeviceAddress != NULL )
	{
		IDirectPlay8Address_Release( pDeviceAddress );
		pDeviceAddress = NULL;
	}

	DNASSERT( pEndpoint == NULL );

	if ( hr != DPNERR_PENDING )
	{
		// this command cannot complete synchronously!
		DNASSERT( hr != DPN_OK );

		DPF( 0, "Problem with DNSP_Listen()" );
		DisplayDNError( 0, hr );
	}

	DPF(9, "\nLeave:\tDNSP_Listen\n" );

	return hr;

Failure:
	//
	// if there's an allocated command, clean up and then
	// return the command
	//
	if ( pCommand != NULL )
	{
		pCommand->DecRef();
		pCommand = NULL;

		pListenData->hCommand = NULL;
		pListenData->dwCommandDescriptor = NULL_DESCRIPTOR;
	}

	//
	// is there an endpoint to free?
	//
	if ( pEndpoint != NULL )
	{
		if ( fEndpointOpen != FALSE )
		{
			pEndpoint->Close( hr );
			fEndpointOpen = FALSE;
		}

		pSPData->CloseEndpointHandle( pEndpoint );
		pEndpoint = NULL;
	}

	//
	// clean up any outstanding references
	//

	if ( fInterfaceReferenceAdded != FALSE )
	{
		IDP8ServiceProvider_Release( pThis );
		fInterfaceReferenceAdded = FALSE;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
/*
 *
 *	DNSP_SendData sends data to the specified "player"
 *
 *	This call MUST BE HIGHLY OPTIMIZED
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_SendData"

STDMETHODIMP DNSP_SendData( IDP8ServiceProvider *pThis, SPSENDDATA *pSendData )
{
	HRESULT			hr;
	CEndpoint		*pEndpoint;
	WRITE_IO_DATA_POOL_CONTEXT	PoolContext;
	CWriteIOData	*pWriteData;
	CSPData			*pSPData;


	DPF(9, "->DNSP_SendData (0x%p, 0x%p)\n", pThis, pSendData);

	DNASSERT( pThis != NULL );
	DNASSERT( pSendData != NULL );
	DNASSERT( pSendData->pBuffers != NULL );
	DNASSERT( pSendData->dwBufferCount != 0 );
	DNASSERT( pSendData->hEndpoint != INVALID_HANDLE_VALUE );
	DNASSERT( ( pSendData->dwFlags & ~( DPNSPF_RELIABLE | DPNSPF_UNRELIABLE ) ) == 0 );

	//
	// initialize
	//
	hr = DPNERR_PENDING;
	pEndpoint = NULL;
	pSendData->hCommand = NULL;
	pSendData->dwCommandDescriptor = NULL_DESCRIPTOR;
	pWriteData = NULL;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	DNASSERT( pSPData->GetState() == SPSTATE_INITIALIZED );

	//
	// No need to lock down the thread counts here because the user already has
	// a connect or something running or they wouldn't be calling this function.
	// That outstanding connect would have locked down the thread pool.
	//

	//
	// Attempt to grab the endpoint from the handle.  If this succeeds, the
	// endpoint can send.
	//
	pEndpoint = pSPData->EndpointFromHandle( pSendData->hEndpoint );
	if ( pEndpoint == NULL )
	{
		hr = DPNERR_INVALIDHANDLE;
		DPF( 0, "Invalid endpoint handle on send!" );
		goto Failure;
	}
	
	//
	// send data from pool
	//
	pWriteData = pSPData->GetThreadPool()->GetNewWriteIOData( &PoolContext );
	if ( pWriteData == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Cannot get new write data from pool in SendData!" );
		goto Failure;
	}
	DNASSERT( pWriteData->m_pCommand != NULL );
	DNASSERT( pWriteData->SocketPort() == NULL );

	//
	// set the command state and fill in the message information
	//
	pWriteData->m_pCommand->SetType( COMMAND_TYPE_SEND );
	pWriteData->m_pCommand->SetState( COMMAND_STATE_PENDING );
	pWriteData->m_pCommand->SetEndpoint( pEndpoint );
	pWriteData->m_pCommand->SetUserContext( pSendData->pvContext );
	DNASSERT( pWriteData->m_SendCompleteAction == SEND_COMPLETE_ACTION_UNKNOWN );
	pWriteData->m_SendCompleteAction = SEND_COMPLETE_ACTION_COMPLETE_COMMAND;

	DNASSERT( pSendData->dwBufferCount != 0 );
	pWriteData->m_uBufferCount = pSendData->dwBufferCount;
	pWriteData->m_pBuffers = pSendData->pBuffers;
	pWriteData->m_pDestinationSocketAddress = pEndpoint->GetRemoteAddressPointer();

	pSendData->hCommand = pWriteData->m_pCommand;
	pSendData->dwCommandDescriptor = pWriteData->m_pCommand->GetDescriptor();

	//
	// send data through the endpoint
	//
	pEndpoint->SendUserData( pWriteData );

Exit:
	if ( pEndpoint != NULL )
	{
		pEndpoint->DecCommandRef();
		pEndpoint = NULL;
	}

	return hr;

Failure:
	if ( pWriteData != NULL )
	{
		pSPData->GetThreadPool()->ReturnWriteIOData( pWriteData );
		DEBUG_ONLY( pWriteData = NULL );
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
/*
 *
 *	DNSP_CancelCommand cancels a command in progress
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_CancelCommand"

STDMETHODIMP DNSP_CancelCommand( IDP8ServiceProvider *pThis, HANDLE hCommand, DWORD dwCommandDescriptor )
{
	HRESULT hr;
	CCommandData	*pCommandData;
	BOOL			fCommandLocked;
	BOOL			fReferenceAdded;
	CSPData			*pSPData;


	DNASSERT( pThis != NULL );
	DNASSERT( hCommand != NULL );
	DNASSERT( dwCommandDescriptor != NULL_DESCRIPTOR );

	//
	// initialize
	//
	hr = DPN_OK;
	fCommandLocked = FALSE;
	fReferenceAdded = FALSE;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	//
	// No need to lock the thread pool counts because there's already some outstanding
	// connect or listen running that has done so.
	//

	//
	// check SP state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		//
		// provider is initialized
		//
		case SPSTATE_INITIALIZED:
		{
			IDP8ServiceProvider_AddRef( pThis );
			fReferenceAdded = TRUE;
			break;
		}

		//
		// provider is uninitialized
		//
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "CancelCommand called on uninitialized SP!" );
			DNASSERT( FALSE );
			goto Exit;

			break;
		}

		//
		// provider is closing
		//
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "CancelCommand called on closing SP!" );
			DNASSERT( FALSE );
			goto Exit;

			break;
		}

		//
		// unknown
		//
		default:
		{
			hr = DPNERR_GENERIC;
			DNASSERT( FALSE );
			goto Exit;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Exit;
	}

	pCommandData = static_cast<CCommandData*>( hCommand );
	pCommandData->Lock();
	fCommandLocked = TRUE;

	//
	// this should never happen
	//
	if ( pCommandData->GetDescriptor() != dwCommandDescriptor )
	{
		hr = DPNERR_INVALIDCOMMAND;
		DPF( 0, "Attempt to cancel command with mismatched command descriptor!" );
		goto Exit;
	}

	switch ( pCommandData->GetState() )
	{
		//
		// unknown command state
		//
		case COMMAND_STATE_UNKNOWN:
		{
			hr = DPNERR_INVALIDCOMMAND;
			DNASSERT( FALSE );
			break;
		}

		//
		// command is waiting to be processed, set command state to be cancelling
		// and wait for someone to pick it up
		//
		case COMMAND_STATE_PENDING:
		{
			pCommandData->SetState( COMMAND_STATE_CANCELLING );
			break;
		}

		//
		// command in progress, and can't be cancelled
		//
		case COMMAND_STATE_INPROGRESS_CANNOT_CANCEL:
		{
			hr = DPNERR_CANNOTCANCEL;
			break;
		}

		//
		// Command is already being cancelled.  This is not a problem, but shouldn't
		// be happening.
		//
		case COMMAND_STATE_CANCELLING:
		{
			DNASSERT( hr == DPN_OK );
			DNASSERT( FALSE );
			break;
		}

		//
		// command is in progress, find out what type of command it is
		//
		case COMMAND_STATE_INPROGRESS:
		{
			switch ( pCommandData->GetType() )
			{
				case COMMAND_TYPE_UNKNOWN:
				{
					// we should never be here
					INT3;
					break;
				}

				case COMMAND_TYPE_CONNECT:
				{
					// we should never be in this state!
					INT3;
					break;
				}

				case COMMAND_TYPE_LISTEN:
				{
					HRESULT		hTempResult;
					CEndpoint	*pEndpoint;


					//
					// Set this command to the cancel state before we shut down
					// this endpoint.  Make sure a reference is added to the
					// endpoint so it stays around for the cancel.
					//
					pCommandData->SetState( COMMAND_STATE_CANCELLING );
					pEndpoint = pCommandData->GetEndpoint();
					pEndpoint->AddRef();
					pCommandData->Unlock();
					fCommandLocked = FALSE;

					pEndpoint->Lock();
					switch ( pEndpoint->GetState() )
					{
						//
						// endpoint is already disconnecting, no action needs to be taken
						//
						case ENDPOINT_STATE_DISCONNECTING:
						{
							pEndpoint->Unlock();
							pEndpoint->DecRef();
							goto Exit;
							break;
						}

						//
						// Endpoint is listening.  Flag it as Disconnecting and
						// add a reference so it doesn't disappear on us
						//
						case ENDPOINT_STATE_LISTEN:
						{
							pEndpoint->SetState( ENDPOINT_STATE_DISCONNECTING );
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

					pEndpoint->Unlock();

					pSPData->CloseEndpointHandle( pEndpoint );
					pEndpoint->Close( DPNERR_USERCANCEL );
					pEndpoint->DecRef();

					break;
				}

				case COMMAND_TYPE_ENUM_QUERY:
				{
					CEndpoint	 *pEndpoint;


					pEndpoint = pCommandData->GetEndpoint();
					DNASSERT( pEndpoint != NULL );

					pEndpoint->AddRef();
					pEndpoint->Lock();
					pEndpoint->SetState( ENDPOINT_STATE_DISCONNECTING );
					pEndpoint->Unlock();

					pCommandData->SetState( COMMAND_STATE_CANCELLING );
					pCommandData->Unlock();
					fCommandLocked = FALSE;
						
					pEndpoint->StopEnumCommand( DPNERR_USERCANCEL );
					pEndpoint->DecRef();

					break;
				}

				case COMMAND_TYPE_SEND:
				{
					// we should never be here
					INT3;
					break;
				}

				default:
				{
					INT3;
					break;
				}
			}

			break;
		}

		//
		// other command state
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}

Exit:
	if ( fCommandLocked != FALSE  )
	{
		DNASSERT( pCommandData != NULL );
		pCommandData->Unlock();
		fCommandLocked = FALSE;
	}

	if ( fReferenceAdded != FALSE )
	{
		IDP8ServiceProvider_Release( pThis );
		fReferenceAdded = FALSE;
	}

	return hr;
}
//**********************************************************************


//**********************************************************************
/*
 *
 *	DNSP_CreateGroup creates a new group (duh)
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_CreateGroup"

STDMETHODIMP DNSP_CreateGroup( IDP8ServiceProvider *pThis, SPCREATEGROUPDATA *pCreateGroupData)
{
	HRESULT 		hr;
	CSocketAddress	*pLocalSocketAddress;
	CSocketAddress	*pMulticastGroupAddress;
	BOOL			fInterfaceReferenceAdded;
	CSPData			*pSPData;


	DNASSERT( pThis != NULL );
	DNASSERT( pCreateGroupData != NULL );
	DNASSERT( pCreateGroupData->dwFlags == 0 );
	DNASSERT( pCreateGroupData->pAdapterGuid != NULL );
	DNASSERT( ( pCreateGroupData->pGroupData != NULL ) || ( pCreateGroupData->dwGroupDataSize == 0 ) );

	//
	// initialize
	//
	hr = DPN_OK;
	pLocalSocketAddress = NULL;
	pMulticastGroupAddress = NULL;
	fInterfaceReferenceAdded = FALSE;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	//
	// validate SP state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		// provider is initialized
		case SPSTATE_INITIALIZED:
		{
			//
			// no problem, add a reference and proceed
			//
			IDP8ServiceProvider_AddRef( pThis );
			fInterfaceReferenceAdded = TRUE;
			DNASSERT( hr == DPN_OK );
			break;
		}

		// provider is uninitialized
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "CreateGroup called on uninitialized SP!" );

			break;
		}

		// provider is closing
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "CreateGroup called while SP closing!" );

			break;
		}

		// unknown
		default:
		{
			DNASSERT( FALSE );
			hr = DPNERR_GENERIC;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Failure;
	}

	//
	// get an address from the pool
	//
	pLocalSocketAddress = pSPData->GetNewAddress();
	if ( pLocalSocketAddress == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Failed to get new address for local socket when creating multicast group!" );
		goto Failure;
	}

	//
	// check for user specified multicast address and parse, if applicable
	//
	if ( pCreateGroupData->pGroupAddress != NULL )
	{
		pMulticastGroupAddress = pSPData->GetNewAddress();
		if ( pMulticastGroupAddress == NULL )
		{
			hr = DPNERR_OUTOFMEMORY;
			DPF( 0, "Failed to get new addres for multicast address!" );
			goto Failure;
		}

		hr = pMulticastGroupAddress->SocketAddressFromDP8Address( pCreateGroupData->pGroupAddress, SP_ADDRESS_TYPE_HOST );
		if ( hr != DPN_OK )
		{
			DPF( 0, "Problem parsing user-specified group address in DNSP_CreateGroup!" );
			DisplayDNError( 0, hr );
			goto Failure;
		}
	}

	hr = pLocalSocketAddress->CreateMulticastGroup( *pCreateGroupData->pAdapterGuid,
													pMulticastGroupAddress,
													pCreateGroupData->pGroupData,
													&pCreateGroupData->dwGroupDataSize
													);
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem creating multicast group on socket port!" );
		DisplayDNError( 0, hr );
	}

Exit:
	if ( pLocalSocketAddress != NULL )
	{
		pSPData->ReturnAddress( pLocalSocketAddress );
		pLocalSocketAddress = NULL;
	}

	if ( pMulticastGroupAddress != NULL )
	{
		pSPData->ReturnAddress( pMulticastGroupAddress );
		pMulticastGroupAddress = NULL;
	}

	if ( fInterfaceReferenceAdded != FALSE )
	{
		IDP8ServiceProvider_Release( pThis );
		fInterfaceReferenceAdded = FALSE;
	}

	return hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
/*
 *
 *	DNSP_DeleteGroup deletes a specified group (duh)
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_DeleteGroup"

STDMETHODIMP DNSP_DeleteGroup( IDP8ServiceProvider *pThis, SPDELETEGROUPDATA *pDeleteGroupData)
{
	HRESULT 		hr;
	BOOL			fInterfaceReferenceAdded;
	CSocketAddress	*pSocketAddress;
	CSPData			*pSPData;


	DNASSERT( pThis != NULL );
	DNASSERT( pDeleteGroupData != NULL );
	DNASSERT( pDeleteGroupData->pAdapterGuid != NULL );
	DNASSERT( pDeleteGroupData->pGroupData != NULL );
	DNASSERT( pDeleteGroupData->dwGroupDataSize == sizeof( MULTICAST_DATA ) );
	DNASSERT( pDeleteGroupData->dwFlags == 0 );


	//
	// initialize
	//
	hr = DPN_OK;
	fInterfaceReferenceAdded = FALSE;
	pSocketAddress = NULL;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	//
	// no need to tell the thread pool to lock down the thread counts because
	// creating a group would have done so.
	//

	//
	// validate SP state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		// provider is initialized
		case SPSTATE_INITIALIZED:
		{
			//
			// no problem, add a reference and proceed
			//
			IDP8ServiceProvider_AddRef( pThis );
			fInterfaceReferenceAdded = TRUE;
			DNASSERT( hr == DPN_OK );
			break;
		}

		// provider is uninitialized
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "DeleteGroup called on uninitialized SP!" );

			break;
		}

		// provider is closing
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "DeleteGroup called while SP closing!" );

			break;
		}

		// unknown
		default:
		{
			DNASSERT( FALSE );
			hr = DPNERR_GENERIC;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Failure;
	}

	pSocketAddress = pSPData->GetNewAddress();
	if ( pSocketAddress == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Failed to get new socket address when destroying multicast group!" );
		goto Failure;
	}

	//
	// destroy multicast group
	//
	hr = pSocketAddress->DeleteMulticastGroup( *pDeleteGroupData->pAdapterGuid,
											   pDeleteGroupData->pGroupData,
											   pDeleteGroupData->dwGroupDataSize );
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem deleting multicast group!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

Exit:
	if ( pSocketAddress != NULL )
	{
		pSPData->ReturnAddress( pSocketAddress );
		pSocketAddress = NULL;
	}

	if ( fInterfaceReferenceAdded != FALSE )
	{
		IDP8ServiceProvider_Release( pThis );
		fInterfaceReferenceAdded = FALSE;
	}

	return hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
/*
 *
 *	DNSP_AddToGroup adds a player or group to a group
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_AddToGroup"

STDMETHODIMP DNSP_AddToGroup( IDP8ServiceProvider *pThis, SPADDTOGROUPDATA *pAddToGroupData)
{
	HRESULT	    	hr;
	BOOL	    	fInterfaceReferenceAdded;
	CSocketAddress	*pSocketAddress;
	CEndpoint   	*pEndpoint;
	BOOL	    	fEndpointBound;
	CSPData	    	*pSPData;


	DNASSERT( pThis != NULL );
	DNASSERT( pAddToGroupData != NULL );
	DNASSERT( pAddToGroupData->pAdapterGuid != NULL );
	DNASSERT( pAddToGroupData->pGroupData != 0 );
	DNASSERT( pAddToGroupData->dwGroupDataSize != 0 );
	DNASSERT( ( pAddToGroupData->dwFlags != 0 ) &&
			  ( ( pAddToGroupData->dwFlags & ~( SP_ADD_TO_MULTICAST_GROUP_RECEIVE | SP_ADD_TO_MULTICAST_GROUP_SEND ) ) == 0 ) );

	//
	// initialize
	//
	hr = DPN_OK;
	fInterfaceReferenceAdded = FALSE;
	pSocketAddress = NULL;
	pEndpoint = NULL;
	fEndpointBound = FALSE;
	pAddToGroupData->hMulticastEndpoint = NULL;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	//
	// validate SP state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		//
		// provider is initialized, add a reference and proceed
		//
		case SPSTATE_INITIALIZED:
		{
			IDP8ServiceProvider_AddRef( pThis );
			fInterfaceReferenceAdded = TRUE;
			DNASSERT( hr == DPN_OK );
			break;
		}

		//
		// provider is uninitialized, fail
		//
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "AddToGroup called on uninitialized SP!" );

			break;
		}

		//
		// provider is closing, fail
		//
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "AddToGroup called while SP closing!" );

			break;
		}

		//
		// unknown
		//
		default:
		{
			DNASSERT( FALSE );
			hr = DPNERR_GENERIC;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Failure;
	}

	//
	// Check SP type
	//
		//
		// Multicast groups are allowed on IP.  Create a socket port and an endpoint
		// and then join the multicast group
		//
			pSocketAddress = pSPData->GetNewAddress();
			if ( pSocketAddress == NULL )
			{
			    hr = DPNERR_OUTOFMEMORY;
			    DPF( 0, "Could not get address to AddToGroup group!" );
			    goto Failure;
			}
			DNASSERT( pSocketAddress != NULL );
			pSocketAddress->ImportMulticastAdapterAddress( *pAddToGroupData->pAdapterGuid, pAddToGroupData->pGroupData, pAddToGroupData->dwGroupDataSize );

			pEndpoint = pSPData->GetNewEndpoint();
			if ( pEndpoint == NULL )
			{
			    hr = DPNERR_OUTOFMEMORY;
			    DPF( 0, "Could not create new endpoint when adding a multicast group!" );
			    goto Failure;
			}

			pSocketAddress->ImportMulticastTargetAddress( pAddToGroupData->pGroupData, pAddToGroupData->dwGroupDataSize );

			hr = pEndpoint->Open( ENDPOINT_TYPE_MULTICAST, NULL, pSocketAddress );
			if ( hr != DPN_OK )
			{
			    DPF( 0, "Failed to open endpoint for multicast!" );
			    DisplayDNError( 0, hr );
			    goto Failure;
			}

			DNASSERT( fEndpointBound == FALSE );
			hr = pSPData->BindEndpoint( pEndpoint, NULL, pSocketAddress, GATEWAY_BIND_TYPE_NONE );
			if ( hr != DPN_OK )
			{
			    DPF( 0, "Failed to bind endpoint to socket port!" );
			    DisplayDNError( 0, hr );
			    goto Failure;
			}
			fEndpointBound = TRUE;

			hr = pEndpoint->GetSocketPort()->AddToMulticastGroup( pAddToGroupData->pGroupData, pAddToGroupData->dwGroupDataSize );
			if ( hr != DPN_OK )
			{
			    DPF( 0, "Problem adding to multicast group!" );
			    DisplayDNError( 0, hr );
			    goto Failure;
			}

			DNASSERT( hr == DPN_OK );
			pAddToGroupData->hMulticastEndpoint = pEndpoint->GetHandle();

Exit:
	if ( pSocketAddress != NULL )
	{
		pSPData->ReturnAddress( pSocketAddress );
		pSocketAddress = NULL;
	}

	if ( fInterfaceReferenceAdded != FALSE )
	{
		IDP8ServiceProvider_Release( pThis );
		fInterfaceReferenceAdded = FALSE;
	}

	return hr;

Failure:
	if ( pEndpoint != NULL )
	{
		pSPData->CloseEndpointHandle( pEndpoint );
		pEndpoint = NULL;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
/*
 *
 *	DNSP_RemoveFromGroup removes a player or group from the specified group
 *
 */
//**********************************************************************
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_RemoveFromGroup"

STDMETHODIMP DNSP_RemoveFromGroup( IDP8ServiceProvider *pThis, SPREMOVEFROMGROUPDATA *pRemoveFromGroupData)
{
	HRESULT	    	hr;
	BOOL	    	fInterfaceReferenceAdded;
	CEndpoint   	*pEndpoint;
	CSPData	    	*pSPData;


	DNASSERT( pThis != NULL );
	DNASSERT( pRemoveFromGroupData != NULL );
	DNASSERT( pRemoveFromGroupData->hEndpoint != NULL );
	DNASSERT( pRemoveFromGroupData->pGroupData != NULL );
	DNASSERT( pRemoveFromGroupData->dwFlags == 0 );

	//
	// initialize
	//
	hr = DPN_OK;
	fInterfaceReferenceAdded = FALSE;
	pEndpoint = NULL;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	//
	// no need to tell thread pool to lock the thread count because adding to or
	// creating the group would have done so.
	//

	//
	// validate SP state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		//
		// provider is initialized, add a reference and proceed
		//
		case SPSTATE_INITIALIZED:
		{
			IDP8ServiceProvider_AddRef( pThis );
			fInterfaceReferenceAdded = TRUE;
			DNASSERT( hr == DPN_OK );
			break;
		}

		//
		// provider is uninitialized
		//
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "AddToGroup called on uninitialized SP!" );

			break;
		}

		//
		// provider is closing
		//
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "AddToGroup called while SP closing!" );

			break;
		}

		//
		// unknown
		//
		default:
		{
			DNASSERT( FALSE );
			hr = DPNERR_GENERIC;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Failure;
	}

	//
	// check SP type
	//
		//
		// Multicast groups are allowed on IP.  Unbind endpoint
		// and remove socket port from group.
		//
			CSocketPort		*pSocketPort;


			pEndpoint = pSPData->GetEndpointAndCloseHandle( pRemoveFromGroupData->hEndpoint );
			if ( pEndpoint != NULL )
			{
			    hr = DPNERR_INVALIDENDPOINT;
			    goto Failure;
			}

			pSocketPort = pEndpoint->GetSocketPort();
			DNASSERT( pSocketPort != NULL );

			pSocketPort->RemoveFromMulticastGroup( pRemoveFromGroupData->pGroupData, pRemoveFromGroupData->dwGroupDataSize );
			pSocketPort = NULL;

			pEndpoint->Close( DPNERR_USERCANCEL );
			pEndpoint->DecCommandRef();
			pEndpoint->DecRef();
			pEndpoint = NULL;

			DNASSERT( hr == DPN_OK );

Exit:
	if ( fInterfaceReferenceAdded != FALSE )
	{
		IDP8ServiceProvider_Release( pThis );
		fInterfaceReferenceAdded = FALSE;
	}

	return hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNSP_GetCaps - get SP capabilities
//
// Entry:		Pointer to DNSP interface
//				Pointer to caps data
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_GetCaps"

STDMETHODIMP	DNSP_GetCaps( IDP8ServiceProvider *pThis, SPGETCAPSDATA *pCapsData )
{
	HRESULT		hr;
	BOOL		fInterfaceReferenceAdded;
	CSPData		*pSPData;
	LONG		iIOThreadCount;
	

	DNASSERT( pThis != NULL );
	DNASSERT( pCapsData != NULL );
	DNASSERT( pCapsData->dwSize == sizeof( *pCapsData ) );

	//
	// initialize
	//
	hr = DPN_OK;
	fInterfaceReferenceAdded = FALSE;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	//
	// no need to tell thread pool to lock the thread count for this function.
	//

	//
	// validate SP state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		//
		// provider is initialized, add a reference and proceed
		//
		case SPSTATE_INITIALIZED:
		{
			IDP8ServiceProvider_AddRef( pThis );
			fInterfaceReferenceAdded = TRUE;
			DNASSERT( hr == DPN_OK );
			break;
		}

		//
		// provider is uninitialized
		//
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "AddToGroup called on uninitialized SP!" );

			break;
		}

		//
		// provider is closing
		//
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "AddToGroup called while SP closing!" );

			break;
		}

		//
		// unknown
		//
		default:
		{
			DNASSERT( FALSE );
			hr = DPNERR_GENERIC;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Failure;
	}

	//
	// set flags
	//
	pCapsData->dwFlags = 0;
	pCapsData->dwFlags |= ( DPNSPCAPS_SUPPORTSDPNSRV |
							DPNSPCAPS_SUPPORTSBROADCAST |
							DPNSPCAPS_SUPPORTSALLADAPTERS );

	//
	// set frame sizes
	//
	pCapsData->dwUserFrameSize = MAX_USER_PAYLOAD;
	pCapsData->dwEnumFrameSize = 1000;

	//
	// Set link speed, no need to check for endpoint because
	// the link speed cannot be determined.
	//
	pCapsData->dwLocalLinkSpeed = UNKNOWN_BANDWIDTH;

// BUGBUG: [mgere] [xbox] Always 0 since we removed threads
	pCapsData->dwIOThreadCount = 0;

	//
	// set enumeration defaults
	//
	pCapsData->dwDefaultEnumRetryCount = DEFAULT_ENUM_RETRY_COUNT;
	pCapsData->dwDefaultEnumRetryInterval = DEFAULT_ENUM_RETRY_INTERVAL;
	pCapsData->dwDefaultEnumTimeout = DEFAULT_ENUM_TIMEOUT;

	//
	// set buffering information
	//
	DNASSERT( g_dwWinsockReceiveBufferMultiplier <= UINT32_MAX );
	pCapsData->dwBuffersPerThread = static_cast<DWORD>( g_dwWinsockReceiveBufferMultiplier );
	pCapsData->dwSystemBufferSize = 8192;

	if ( g_fWinsockReceiveBufferSizeOverridden == FALSE )
	{
		SOCKET		TestSocket;
	
		
		TestSocket = INVALID_SOCKET;
				TestSocket = p_socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

		if ( TestSocket != INVALID_SOCKET )
		{
			INT		iBufferSize;
			INT		iBufferSizeSize;
			INT		iWSAReturn;


			iBufferSizeSize = sizeof( iBufferSize );
			iWSAReturn = p_getsockopt( TestSocket,									// socket
									   SOL_SOCKET,									// socket level option
									   SO_RCVBUF,									// socket option
									   reinterpret_cast<char*>( &iBufferSize ),		// pointer to destination
									   &iBufferSizeSize								// pointer to destination size
									   );
			if ( iWSAReturn != SOCKET_ERROR )
			{
				pCapsData->dwSystemBufferSize = iBufferSize;
			}
			else
			{
				DPF( 0, "Failed to get socket receive buffer options!" );
				DisplayWinsockError( 0, iWSAReturn );
			}

			p_closesocket( TestSocket );
			TestSocket = INVALID_SOCKET;
		}
	}
	else
	{
		pCapsData->dwSystemBufferSize = g_iWinsockReceiveBufferSize;
	}

Exit:
	if ( fInterfaceReferenceAdded != FALSE )
	{
		IDP8ServiceProvider_Release( pThis );
		fInterfaceReferenceAdded = FALSE;
	}

	return	hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNSP_SetCaps - set SP capabilities
//
// Entry:		Pointer to DNSP interface
//				Pointer to caps data
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_SetCaps"

STDMETHODIMP	DNSP_SetCaps( IDP8ServiceProvider *pThis, SPSETCAPSDATA *pCapsData )
{
	HRESULT			hr;
	BOOL			fInterfaceReferenceAdded;
	CSPData			*pSPData;


	DNASSERT( pThis != NULL );
	DNASSERT( pCapsData != NULL );
	DNASSERT( pCapsData->dwSize == sizeof( *pCapsData ) );


	//
	// initialize
	//
	hr = DPN_OK;
	fInterfaceReferenceAdded = FALSE;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	//
	// no need to tell thread pool to lock the thread count for this function.
	//

	//
	// validate SP state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		//
		// provider is initialized, add a reference and proceed
		//
		case SPSTATE_INITIALIZED:
		{
			IDP8ServiceProvider_AddRef( pThis );
			fInterfaceReferenceAdded = TRUE;
			DNASSERT( hr == DPN_OK );
			break;
		}

		//
		// provider is uninitialized
		//
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "AddToGroup called on uninitialized SP!" );

			break;
		}

		//
		// provider is closing
		//
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "AddToGroup called while SP closing!" );

			break;
		}

		//
		// unknown
		//
		default:
		{
			DNASSERT( FALSE );
			hr = DPNERR_GENERIC;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Failure;
	}

	//
	// validate caps
	//
	if ( pCapsData->dwBuffersPerThread == 0 )
	{
		DPF( 0, "Failing SetCaps because dwBuffersPerThread == 0" );
		hr = DPNERR_INVALIDPARAM;
		goto Failure;
	}

// BUGBUG: [mgere] [xbox] No longer able to set thread count -- document this change
//	hr = GetThreadPool()->SetIOThreadCount( pCapsData->dwIOThreadCount );

	//
	// Only update the thread multiplier if there is a difference.  Give precedence
	// to any values in the registry.
	//
// BUGBUG: [mgere] [xbox] No more than 1 overlapped receive per socket.
/*
	if ( pCapsData->dwBuffersPerThread > g_dwWinsockReceiveBufferMultiplier )
	{
		DWORD	dwDelta;

		dwDelta = 0;

		DNASSERT( g_dwWinsockReceiveBufferMultiplier <= UINT32_MAX );
		dwDelta = pCapsData->dwBuffersPerThread - static_cast<DWORD>( g_dwWinsockReceiveBufferMultiplier );
		g_dwWinsockReceiveBufferMultiplier = pCapsData->dwBuffersPerThread;

		IncreaseOutstandingReceivesOnAllSockets( dwDelta );
	}
*/

	//
	// Only update the buffer size if there is a difference.  Give precedence to
	// any values in the registry.
	//
	DBG_CASSERT( sizeof( pCapsData->dwSystemBufferSize ) == sizeof( g_iWinsockReceiveBufferSize ) );
	if ( pCapsData->dwSystemBufferSize != static_cast<DWORD>( g_iWinsockReceiveBufferSize ) )
	{
		g_fWinsockReceiveBufferSizeOverridden = TRUE;
		g_iWinsockReceiveBufferSize = pCapsData->dwSystemBufferSize;
		pSPData->SetWinsockBufferSizeOnAllSockets( g_iWinsockReceiveBufferSize );
	}

Exit:
	if ( fInterfaceReferenceAdded != FALSE )
	{
		IDP8ServiceProvider_Release( pThis );
		fInterfaceReferenceAdded = FALSE;
	}

	return	hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNSP_ReturnReceiveBuffers - return receive buffers to pool
//
// Entry:		Pointer to DNSP interface
//				Pointer to caps data
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_ReturnReceiveBuffers"

STDMETHODIMP	DNSP_ReturnReceiveBuffers( IDP8ServiceProvider *pThis, SPRECEIVEDBUFFER *pReceivedBuffers )
{
	SPRECEIVEDBUFFER	*pBuffers;


	//
	// no need to tell thread pool to lock the thread count for this function.
	//

	DNASSERT( pThis != NULL );
	DNASSERT( pReceivedBuffers != NULL );

	pBuffers = pReceivedBuffers;
	while ( pBuffers != NULL )
	{
		SPRECEIVEDBUFFER	*pTemp;
		CReadIOData			*pReadData;


		pTemp = pBuffers;
		pBuffers = pBuffers->pNext;
		pReadData = CReadIOData::ReadDataFromSPReceivedBuffer( pTemp );
		DEBUG_ONLY( pReadData->m_fRetainedByHigherLayer = FALSE );
		pReadData->DecRef();
	}

	return	DPN_OK;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNSP_GetAddressInfo - get address information for an endpoint
//
// Entry:		Pointer to DNSP Interface
//				Pointer to input data
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_GetAddressInfo"

STDMETHODIMP	DNSP_GetAddressInfo( IDP8ServiceProvider *pThis, SPGETADDRESSINFODATA *pGetAddressInfoData )
{
	HRESULT		hr;
	CEndpoint	*pEndpoint;
	CSPData		*pSPData;


	DNASSERT( pThis != NULL );
	DNASSERT( pGetAddressInfoData != NULL );
	DNASSERT( pGetAddressInfoData->hEndpoint != INVALID_HANDLE_VALUE );
	DNASSERT( ( pGetAddressInfoData->Flags & ~( SP_GET_ADDRESS_INFO_LOCAL_ADAPTER |
												SP_GET_ADDRESS_INFO_REMOTE_HOST |
												SP_GET_ADDRESS_INFO_LISTEN_HOST_ADDRESSES |
												SP_GET_ADDRESS_INFO_LOCAL_HOST_PUBLIC_ADDRESS ) ) == 0 );


	//
	// initialize
	//
	hr = DPN_OK;
	DBG_CASSERT( sizeof( pEndpoint ) == sizeof( pGetAddressInfoData->hEndpoint ) );
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	//
	// no need to tell thread pool to lock the thread count for this function.
	//
	pEndpoint = pSPData->EndpointFromHandle( pGetAddressInfoData->hEndpoint );
	if ( pEndpoint != NULL )
	{
		switch ( pGetAddressInfoData->Flags )
		{
			case SP_GET_ADDRESS_INFO_REMOTE_HOST:
			{
				pGetAddressInfoData->pAddress = pEndpoint->GetRemoteHostDP8Address();
				break;
			}

			case SP_GET_ADDRESS_INFO_LOCAL_ADAPTER:
			{
				pGetAddressInfoData->pAddress = pEndpoint->GetLocalAdapterDP8Address( SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT );
				break;
			}

			case SP_GET_ADDRESS_INFO_LISTEN_HOST_ADDRESSES:
			{
				pGetAddressInfoData->pAddress = pEndpoint->GetLocalAdapterDP8Address( SP_ADDRESS_TYPE_HOST );
				break;
			}

			case SP_GET_ADDRESS_INFO_LOCAL_HOST_PUBLIC_ADDRESS:
			{
				pGetAddressInfoData->pAddress = pEndpoint->GetLocalAdapterDP8Address( SP_ADDRESS_TYPE_PUBLIC_HOST_ADDRESS );
				break;
			}

			default:
			{
				DNASSERT( FALSE );
				break;
			}
		}
	
		pEndpoint->DecCommandRef();
		pEndpoint = NULL;
	}
	else
	{
		hr = DPNERR_INVALIDENDPOINT;
	}

	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem getting DNAddress from endpoint!" );
		DisplayDNError( 0, hr );
	}

	return	hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNSP_IsApplicationSupported - determine if this application is supported by this
//		SP.
//
// Entry:		Pointer to DNSP Interface
//				Pointer to input data
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_IsApplicationSupported"

STDMETHODIMP	DNSP_IsApplicationSupported( IDP8ServiceProvider *pThis, SPISAPPLICATIONSUPPORTEDDATA *pIsApplicationSupportedData )
{
	HRESULT			hr;
	BOOL			fInterfaceReferenceAdded;
	CSPData			*pSPData;


	DNASSERT( pThis != NULL );
	DNASSERT( pIsApplicationSupportedData != NULL );
	DNASSERT( pIsApplicationSupportedData->pApplicationGuid != NULL );
	DNASSERT( pIsApplicationSupportedData->dwFlags == 0 );

	//
	// initialize, we support all applications with this SP
	//
	hr = DPN_OK;
	fInterfaceReferenceAdded = FALSE;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );

	//
	// no need to tell thread pool to lock the thread count for this function.
	//

	//
	// validate SP state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		//
		// provider is initialized, add a reference and proceed
		//
		case SPSTATE_INITIALIZED:
		{
			IDP8ServiceProvider_AddRef( pThis );
			fInterfaceReferenceAdded = TRUE;
			DNASSERT( hr == DPN_OK );
			break;
		}

		//
		// provider is uninitialized
		//
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "AddToGroup called on uninitialized SP!" );

			break;
		}

		//
		// provider is closing
		//
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "AddToGroup called while SP closing!" );

			break;
		}

		//
		// unknown
		//
		default:
		{
			DNASSERT( FALSE );
			hr = DPNERR_GENERIC;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Failure;
	}

Exit:
	if ( fInterfaceReferenceAdded != FALSE )
	{
		IDP8ServiceProvider_Release( pThis );
		fInterfaceReferenceAdded = FALSE;
	}

	return	hr;

Failure:

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNSP_ProxyEnumQuery - proxy an enum query
//
// Entry:		Pointer DNSP Interface
//				Pointer to input data
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "DNSP_ProxyEnumQuery"

STDMETHODIMP	DNSP_ProxyEnumQuery( IDP8ServiceProvider *pThis, SPPROXYENUMQUERYDATA *pProxyEnumQueryData )
{
	HRESULT			hr;
	BOOL			fInterfaceReferenceAdded;
	CSPData			*pSPData;
	WRITE_IO_DATA_POOL_CONTEXT	PoolContext;
	CSocketAddress	*pDestinationAddress;
	CSocketAddress	*pReturnAddress;
	CWriteIOData	*pWriteData;
	CEndpoint		*pEndpoint;
	const ENDPOINT_ENUM_QUERY_CONTEXT	*pEndpointEnumContext;


	DNASSERT( pThis != NULL );
	DNASSERT( pProxyEnumQueryData != NULL );
	DNASSERT( pProxyEnumQueryData->dwFlags == 0 );

	//
	// initialize
	//
	hr = DPN_OK;
	DBG_CASSERT( OFFSETOF( ENDPOINT_ENUM_QUERY_CONTEXT, EnumQueryData ) == 0 );
	pEndpointEnumContext = reinterpret_cast<ENDPOINT_ENUM_QUERY_CONTEXT*>( pProxyEnumQueryData->pIncomingQueryData );
	fInterfaceReferenceAdded = FALSE;
	pSPData = CSPData::SPDataFromCOMInterface( pThis );
	pDestinationAddress = NULL;
	pReturnAddress = NULL;
	pWriteData = NULL;
	pEndpoint = NULL;

	//
	// No need to tell thread pool to lock the thread count for this function
	// because there's already an outstanding enum that did.
	//

	//
	// validate SP state
	//
	pSPData->Lock();
	switch ( pSPData->GetState() )
	{
		//
		// provider is initialized, add a reference and proceed
		//
		case SPSTATE_INITIALIZED:
		{
			IDP8ServiceProvider_AddRef( pThis );
			fInterfaceReferenceAdded = TRUE;
			DNASSERT( hr == DPN_OK );
			break;
		}

		//
		// provider is uninitialized
		//
		case SPSTATE_UNINITIALIZED:
		{
			hr = DPNERR_UNINITIALIZED;
			DPF( 0, "ProxyEnumQuery called on uninitialized SP!" );

			break;
		}

		//
		// provider is closing
		//
		case SPSTATE_CLOSING:
		{
			hr = DPNERR_ABORTED;
			DPF( 0, "ProxyEnumQuery called while SP closing!" );

			break;
		}

		//
		// unknown
		//
		default:
		{
			DNASSERT( FALSE );
			hr = DPNERR_GENERIC;

			break;
		}
	}
	pSPData->Unlock();
	if ( hr != DPN_OK )
	{
		goto Failure;
	}

	//
	// preallocate addresses
	//
	pDestinationAddress = pSPData->GetNewAddress();
	if ( pDestinationAddress == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	pReturnAddress = pSPData->GetNewAddress();
	if ( pReturnAddress == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Failure;
	}

	//
	// set the endpoint and send it along
	//
	pEndpoint = pSPData->EndpointFromHandle( pEndpointEnumContext->hEndpoint );
	if ( pEndpoint != NULL )
	{
		//
		// set destination address from the supplied data
		//
		hr = pDestinationAddress->SocketAddressFromDP8Address( pProxyEnumQueryData->pDestinationAdapter,
															   SP_ADDRESS_TYPE_DEVICE_PROXIED_ENUM_TARGET );
		if ( hr != DPN_OK )
		{
			DPF( 0, "ProxyEnumQuery: Failed to convert target adapter address" );
			goto Failure;
		}
	
		//
		// set return address from incoming enum query
		//
	#pragma	BUGBUG( johnkan, "Add a reference to the address!!" )
		DBG_CASSERT( sizeof( *pReturnAddress->GetWritableAddress() ) == sizeof( *( pEndpointEnumContext->pReturnAddress->GetAddress() ) ) );
		memcpy( pReturnAddress->GetWritableAddress(),
				pEndpointEnumContext->pReturnAddress->GetAddress(),
				sizeof( *pReturnAddress->GetWritableAddress() ) );
	
		//
		// get write data from pool
		//
		pWriteData = pSPData->GetThreadPool()->GetNewWriteIOData( &PoolContext );
		if ( pWriteData == NULL )
		{
			DPF( 0, "ProxyEnumQuery: Failed to get write data!" );
			hr = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
	
		//
		// a new address is allocated here and will be returned in the send complete
		// code
		//
		pWriteData->m_pDestinationSocketAddress = pDestinationAddress;
	
		//
		// Copy the input BUFFERDESC into the local buffers for sending proxied enum data.
		// The second local buffer is reserved for the SP to prepend data
		//
		pWriteData->m_pBuffers = &pWriteData->m_ProxyEnumSendBuffers[ 1 ];
		DBG_CASSERT( sizeof( pWriteData->m_ProxyEnumSendBuffers[ 1 ] ) == sizeof( pProxyEnumQueryData->pIncomingQueryData->pReceivedData->BufferDesc ) );
		memcpy( &pWriteData->m_ProxyEnumSendBuffers[ 1 ],
				&pProxyEnumQueryData->pIncomingQueryData->pReceivedData->BufferDesc,
				sizeof( pWriteData->m_ProxyEnumSendBuffers[ 1 ] ) );
		pWriteData->m_uBufferCount = 1;
		
		//
		// add a reference to the original receive buffer to prevent it from going
		// away while the enum response send is pending
		//
		pWriteData->m_pProxiedEnumReceiveBuffer = CReadIOData::ReadDataFromSPReceivedBuffer( pProxyEnumQueryData->pIncomingQueryData->pReceivedData );
		DNASSERT( pWriteData->m_pProxiedEnumReceiveBuffer != NULL );
		pWriteData->m_pProxiedEnumReceiveBuffer->AddRef();
	
		pWriteData->m_SendCompleteAction = SEND_COMPLETE_ACTION_PROXIED_ENUM_CLEANUP;
	
		pEndpoint->SendProxiedEnumData( pWriteData, pReturnAddress, pEndpointEnumContext->dwEnumKey );
		pEndpoint->DecCommandRef();
	}

Exit:
	if ( pReturnAddress != NULL )
	{
	    pSPData->ReturnAddress( pReturnAddress );
	    pReturnAddress = NULL;
	}

	if ( fInterfaceReferenceAdded != FALSE )
	{
		IDP8ServiceProvider_Release( pThis );
		fInterfaceReferenceAdded = FALSE;
	}

	return	hr;

Failure:
	goto Exit;
}
//**********************************************************************

