/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Utils.cpp
 *  Content:	Serial service provider utility functions
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	11/25/98	jtk		Created
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

//
// global variables that are unique for the process
//
static	DNCRITICAL_SECTION	g_InterfaceGlobalsLock;

static volatile	LONG	g_iThreadPoolRefCount = 0;

CThreadPool		*g_pThreadPool = NULL;
CThreadPool		*g_pThreadPool2 = NULL;

static volatile LONG	g_iWinsockRefCount = 0;

//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************


//**********************************************************************
// ------------------------------
// InitProcessGlobals - initialize the global items needed for the SP to operate
//
// Entry:		Nothing
//
// Exit:		Boolean indicating success
//				TRUE = success
//				FALSE = failure
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "InitProcessGlobals"

BOOL	InitProcessGlobals( void )
{
	BOOL		fReturn;
	BOOL		fCriticalSectionInitialized;
	BOOL		fPoolsInitialized;


	//
	// initialize
	//
	fReturn = TRUE;
	fCriticalSectionInitialized = FALSE;
	fPoolsInitialized = FALSE;

	if ( DNInitializeCriticalSection( &g_InterfaceGlobalsLock ) == FALSE )
	{
		fReturn = FALSE;
		goto Failure;
	}

	if ( InitializePools() == FALSE )
	{
		fReturn = FALSE;
		goto Failure;
	}

	DNASSERT( g_pThreadPool == NULL );


Exit:
	return	fReturn;

Failure:
	if ( fPoolsInitialized != FALSE )
	{
		DeinitializePools();
		fPoolsInitialized = FALSE;
	}

	if ( fCriticalSectionInitialized != FALSE )
	{
		DNDeleteCriticalSection( &g_InterfaceGlobalsLock );
		fCriticalSectionInitialized = FALSE;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DeinitProcessGlobals - deinitialize the global items
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "DeinitProcessGlobals"

void	DeinitProcessGlobals( void )
{
	DNASSERT( g_pThreadPool == NULL );
	DNASSERT( g_iThreadPoolRefCount == 0 );

	DeinitializePools();
	DNDeleteCriticalSection( &g_InterfaceGlobalsLock );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CreateSPData - create instance data for SP
//
// Entry:		Pointer to pointer to SPData
//				Pointer to class GUID
//				Interface type
//				Pointer to COM interface vtable
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CreateSPData"

HRESULT	CreateSPData( CSPData **const ppSPData,
					  const CLSID *const pClassID,
					  IDP8ServiceProviderVtbl *const pVtbl )
{
	HRESULT		hr;
	CSPData		*pSPData;


	DNASSERT( ppSPData != NULL );
	DNASSERT( pVtbl != NULL );

	//
	// initialize
	//
	hr = DPN_OK;
	*ppSPData = NULL;
	pSPData = NULL;

	//
	// create data
	//
	pSPData = new CSPData;
	if ( pSPData == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Cannot create data for Winsock interface!" );
		goto Failure;
	}
	pSPData->AddRef();

	hr = pSPData->Initialize( pClassID, pVtbl );
	if ( hr != DPN_OK  )
	{
		DPF( 0, "Failed to intialize SP data!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

Exit:
	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem with CreateSPData!" );
		DisplayDNError( 0, hr );
	}

	*ppSPData = pSPData;

	return	hr;

Failure:
	if ( pSPData != NULL )
	{
		pSPData->DecRef();
		pSPData = NULL;	
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// InitializeInterfaceGlobals - perform global initialization for an interface.
//		This entails starting the thread pool and RSIP (if applicable).
//
// Entry:		Pointer to SPData
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "InitializeInterfaceGlobals"

HRESULT	InitializeInterfaceGlobals( CSPData *const pSPData )
{
	HRESULT	hr;
	CThreadPool	*pThreadPool;
// BUGBUG: [mgere] [xbox] Removed RSIP
//	CRsip		*pRsip;


	DNASSERT( pSPData != NULL );

	//
	// initialize
	//
	hr = DPN_OK;
	pThreadPool = NULL;
// BUGBUG: [mgere] [xbox] Removed RSIP
//	pRsip = NULL;

	DNEnterCriticalSection( &g_InterfaceGlobalsLock );

	if ( g_pThreadPool == NULL )
	{
		DNASSERT( g_iThreadPoolRefCount == 0 );
		g_pThreadPool = CreateThreadPool();
		if ( g_pThreadPool != NULL )
		{
			hr = g_pThreadPool->Initialize();
			if ( hr != DPN_OK )
			{
				g_pThreadPool->DecRef();
				g_pThreadPool = NULL;
				hr = DPNERR_OUTOFMEMORY;
				goto Failure;
			}
			else
			{
				g_iThreadPoolRefCount++;
				pThreadPool = g_pThreadPool;
			}
		}
	}
	else
	{
		DNASSERT( g_iThreadPoolRefCount != 0 );
		g_iThreadPoolRefCount++;
		g_pThreadPool->AddRef();
		pThreadPool = g_pThreadPool;
	}

	g_pThreadPool2 = g_pThreadPool;

Exit:
	DNLeaveCriticalSection( &g_InterfaceGlobalsLock );

	pSPData->SetThreadPool( g_pThreadPool );

	return	hr;

Failure:

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DeinitializeInterfaceGlobals - deinitialize thread pool and Rsip
//
// Entry:		Pointer to service provider
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "DeinitializeInterfaceGlobals"

void	DeinitializeInterfaceGlobals( CSPData *const pSPData )
{
	CThreadPool		*pThreadPool;


	DNASSERT( pSPData != NULL );

	//
	// initialize
	//
	pThreadPool = NULL;

	//
	// Process as little as possible inside the lock.  If any of the items
	// need to be released, pointers to them will be set.
	//
	DNEnterCriticalSection( &g_InterfaceGlobalsLock );

	DNASSERT( g_pThreadPool != NULL );
	DNASSERT( g_iThreadPoolRefCount != 0 );
	DNASSERT( g_pThreadPool == pSPData->GetThreadPool() );

	pThreadPool = pSPData->GetThreadPool();

	//
	// remove thread pool reference
	//
	DNASSERT( pThreadPool != NULL );
	g_iThreadPoolRefCount--;
	if ( g_iThreadPoolRefCount == 0 )
	{
		g_pThreadPool = NULL;
	}
	else
	{
		pThreadPool = NULL;
	}

	DNLeaveCriticalSection( &g_InterfaceGlobalsLock );

	//
	// Now that we're outside of the lock, clean up and pointers we have.
	// The thread pool will be cleaned up when all of the outstanding interfaces
	// close.
	//
	if ( pThreadPool != NULL )
	{
		pThreadPool->StopAllIO();
		pThreadPool = NULL;
	}
}
//**********************************************************************



/* BUGBUG: [mgere] [xbox] Removed -- multiple adapters not supported on xbox
//**********************************************************************
// ------------------------------
// AddNetworkAdapterToBuffer - add a network address to a packed buffer
//
// Entry:		Pointer to packed buffer
//				Pointer to adapter name
//				Pointer to adapter guid
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "AddNetworkAdapterToBuffer"

HRESULT	AddNetworkAdapterToBuffer( CPackedBuffer *const pPackedBuffer,
								   const char *const pAdapterName,
								   const GUID *const pAdapterGUID )
{
	HRESULT	hr;
	DPN_SERVICE_PROVIDER_INFO	AdapterInfo;


	DNASSERT( pPackedBuffer != NULL );
	DNASSERT( pAdapterName != NULL );
	DNASSERT( pAdapterGUID != NULL );

	//
	// initialize
	//
	hr = DPN_OK;

	memset( &AdapterInfo, 0x00, sizeof( AdapterInfo ) );
	DNASSERT( AdapterInfo.dwFlags == 0 );
	DNASSERT( AdapterInfo.pvReserved == NULL );
	DNASSERT( AdapterInfo.dwReserved == NULL );

	AdapterInfo.guid = *pAdapterGUID;

	hr = pPackedBuffer->AddStringToBack( pAdapterName );
	if ( ( hr != DPNERR_BUFFERTOOSMALL ) && ( hr != DPN_OK ) )
	{
		DPF( 0, "Failed to add adapter name to buffer!" );
		goto Failure;
	}
	AdapterInfo.pwszName = static_cast<WCHAR*>( pPackedBuffer->GetTailAddress() );

	hr = pPackedBuffer->AddToFront( &AdapterInfo, sizeof( AdapterInfo ) );

Exit:
	return	hr;

Failure:
	goto Exit;
}
//**********************************************************************
*/

