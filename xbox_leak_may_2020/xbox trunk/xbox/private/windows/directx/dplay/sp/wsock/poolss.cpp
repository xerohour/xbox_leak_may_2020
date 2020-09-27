/*==========================================================================
 *
 *  Copyright (C) 2000-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Pools.cpp
 *  Content:	Pool utility functions
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	01/20/2000	jtk		Derived from Utils.h
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
// pool for adapter entries
//
static	CLockedPool< CAdapterEntry >	*g_pAdapterEntryPool = NULL;

//
// pool for command data
//
static	CLockedPool< CCommandData >	*g_pCommandDataPool = NULL;

//
// Pools for addresses.  Since these pools don't
// see a lot of action, they share one lock
//
static	DNCRITICAL_SECTION	g_AddressLock;
static	CFixedPool< CIPAddress >	*g_pIPAddressPool = NULL;

static	CLockedContextFixedPool< CIPEndpoint, ENDPOINT_POOL_CONTEXT* >	*g_pIPEndpointPool = NULL;

//
// pool for Rsip objects
//
// BUGBUG: [mgere] [xbox] Removed RSIP
//static	CLockedFixedPool< CRsip >	*g_pRsipPool = NULL;

//
// pool for socket ports
//
static	CLockedFixedPool< CSocketPort >	*g_pSocketPortPool = NULL;

//
// pool for thread pools
//
static	CLockedFixedPool< CThreadPool >	*g_pThreadPoolPool = NULL;



//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************


//**********************************************************************
// ------------------------------
// InitializePools - initialize pools
//
// Entry:		Nothing
//
// Exit:		Boolean indicating success
//				TRUE = success
//				FALSE = failure
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "InitializePools"

BOOL	InitializePools( void )
{
	BOOL	fReturn;


	//
	// initialize
	//
	fReturn = TRUE;

	//
	// AdapterEntry object pool
	//
	DNASSERT( g_pAdapterEntryPool == NULL );
	g_pAdapterEntryPool = new CLockedPool< CAdapterEntry >;
	if ( g_pAdapterEntryPool != NULL )
	{
		if ( g_pAdapterEntryPool->Initialize() == FALSE )
		{
			delete	g_pAdapterEntryPool;
			g_pAdapterEntryPool = NULL;
			goto Failure;
		}
	}
	else
	{
		goto Failure;
	}
	
	
	//
	// command data pool
	//
	DNASSERT( g_pCommandDataPool == NULL );
	g_pCommandDataPool = new CLockedPool< CCommandData >;
	if ( g_pCommandDataPool != NULL )
	{
		if ( g_pCommandDataPool->Initialize() == FALSE )
		{
			delete	g_pCommandDataPool;
			g_pCommandDataPool = NULL;
			goto Failure;
		}
	}
	else
	{
		goto Failure;
	}

	//
	// initialize lock for address and endpoint pools
	//
	if ( DNInitializeCriticalSection( &g_AddressLock ) == FALSE )
	{
		goto Failure;
	}
	DebugSetCriticalSectionRecursionCount( &g_AddressLock, 0 );

	//
	// address pools
	//
	DNASSERT( g_pIPAddressPool == NULL );
	g_pIPAddressPool = new CFixedPool< CIPAddress >;
	if ( g_pIPAddressPool == NULL )
	{
		goto Failure;
	}

	//
	// endpoint pools
	//
	DNASSERT( g_pIPEndpointPool == NULL );
	g_pIPEndpointPool = new CLockedContextFixedPool< CIPEndpoint, ENDPOINT_POOL_CONTEXT* >;
	if ( g_pIPEndpointPool != NULL )
	{
		if ( g_pIPEndpointPool->Initialize() == FALSE )
		{
			delete	g_pIPEndpointPool;
			g_pIPEndpointPool = NULL;
			goto Failure;
		}
	}
	else
	{
		goto Failure;
	}

	//
	// Rsip object pool
	//
// BUGBUG: [mgere] [xbox] Removed RSIP
//	DNASSERT( g_pRsipPool == NULL );
//	g_pRsipPool = new CLockedFixedPool< CRsip >;
//	if ( g_pRsipPool != NULL )
//	{
//		if ( g_pRsipPool->Initialize() == FALSE )
//		{
//			delete	g_pRsipPool;
//			g_pRsipPool = NULL;
//			goto Failure;
//		}
//	}
//	else
//	{
//		goto Failure;
//	}


	//
	// socket port pool
	//
	DNASSERT( g_pSocketPortPool == NULL );
	g_pSocketPortPool = new CLockedFixedPool< CSocketPort >;
	if ( g_pSocketPortPool != NULL )
	{
		if ( g_pSocketPortPool->Initialize() == FALSE )
		{
			delete	g_pSocketPortPool;
			g_pSocketPortPool = NULL;
			goto Failure;
		}
	}
	else
	{
		goto Failure;
	}


	//
	// thread pool pool
	//
	DNASSERT( g_pThreadPoolPool == NULL );
	g_pThreadPoolPool = new CLockedFixedPool< CThreadPool >;
	if ( g_pThreadPoolPool != NULL )
	{
		if ( g_pThreadPoolPool->Initialize() == FALSE )
		{
			delete	g_pThreadPoolPool;
			g_pThreadPoolPool = NULL;
			goto Failure;
		}
	}
	else
	{
		goto Failure;
	}


Exit:
	return	fReturn;

Failure:
	fReturn = FALSE;
	DeinitializePools();

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DeinitializePools - deinitialize the pools
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "DeinitializePools"

void	DeinitializePools( void )
{
	//
	// thread pool pool
	//
	if ( g_pThreadPoolPool != NULL )
	{
		g_pThreadPoolPool->Deinitialize();
		delete	g_pThreadPoolPool;
		g_pThreadPoolPool = NULL;
	}

	//
	// socket port pool
	//
	if ( g_pSocketPortPool != NULL )
	{
		g_pSocketPortPool->Deinitialize();
		delete	g_pSocketPortPool;
		g_pSocketPortPool = NULL;
	}

	//
	// Rsip pool
	//
// BUGBUG: [mgere] [xbox] Removed RSIP
/*
	if ( g_pRsipPool != NULL )
	{
		g_pRsipPool->Deinitialize();
		delete	g_pRsipPool;
		g_pRsipPool = NULL;
	}
*/

//	//
//	// receive buffer pool
//	//
//	if ( g_pReceiveBufferPool != NULL )
//	{
//	    g_pReceiveBufferPool->Deinitialize();
//	    delete	g_pReceiveBufferPool;
//	    g_pReceiveBufferPool = NULL;
//	}

	//
	// endpoint pools
	//
	if ( g_pIPEndpointPool != NULL )
	{
		g_pIPEndpointPool->Deinitialize();
		delete	g_pIPEndpointPool;
		g_pIPEndpointPool = NULL;
	}

	//
	// address pools
	//
	if ( g_pIPAddressPool != NULL )
	{
		delete	g_pIPAddressPool;
		g_pIPAddressPool = NULL;
	}

	//
	// remove lock for endpoint and address pools
	//
	DNDeleteCriticalSection( &g_AddressLock );

	//
	// command data pool
	//
	if ( g_pCommandDataPool != NULL )
	{
		g_pCommandDataPool->Deinitialize();
		delete	g_pCommandDataPool;
		g_pCommandDataPool = NULL;
	}
	
	//
	// AdapterEntry pool
	//
	if ( g_pAdapterEntryPool != NULL )
	{
		g_pAdapterEntryPool->Deinitialize();
		delete	g_pAdapterEntryPool;
		g_pAdapterEntryPool = NULL;
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CreateAdapterEntry - create an adapter entry
//
// Entry:		Nothing
//
// Exit:		Poiner to entry (NULL = out of memory)
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CreateAdapterEntry"

CAdapterEntry	*CreateAdapterEntry( void )
{
	DNASSERT( g_pAdapterEntryPool != NULL );
	return	g_pAdapterEntryPool->Get();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// ReturnAdapterEntry - return an adapter entry to the pool
//
// Entry:		Pointer to entry
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "ReturnAdapterEntry"

void	ReturnAdapterEntry( CAdapterEntry *const pAdapterEntry )
{
	INT3;
	DNASSERT( pAdapterEntry != NULL );
	DNASSERT( g_pAdapterEntryPool != NULL );
	g_pAdapterEntryPool->Release( pAdapterEntry );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CreateCommand - create command
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CreateCommand"

CCommandData	*CreateCommand( void )
{
	DNASSERT( g_pCommandDataPool != NULL );
	return	g_pCommandDataPool->Get();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// ReturnCommand - return a command
//
// Entry:		Pointer to command
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "ReturnCommand"

void	ReturnCommand( CCommandData *const pCommand )
{
	DNASSERT( pCommand != NULL );
	DNASSERT( g_pCommandDataPool != NULL );
	g_pCommandDataPool->Release( pCommand );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CreateIPAddress - create IP address
//
// Entry:		Nothing
//
// Exit:		Pointer to IP address
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CreateIPAddress"

CIPAddress	*CreateIPAddress( void )
{
	CIPAddress	*pReturnAddress;

	
	DNASSERT( g_pIPAddressPool != NULL );

	DNEnterCriticalSection( &g_AddressLock );
	pReturnAddress = g_pIPAddressPool->Get();
	DNLeaveCriticalSection( &g_AddressLock );
	return	pReturnAddress;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// ReturnIPAddress - return an IP address
//
// Entry:		Pointer to IP address
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "ReturnIPAddress"

void	ReturnIPAddress( CIPAddress *const pIPAddress )
{
	DNASSERT( pIPAddress != NULL );
	DNASSERT( g_pIPAddressPool != NULL );
	DNEnterCriticalSection( &g_AddressLock );
	g_pIPAddressPool->Release( pIPAddress );
	DNLeaveCriticalSection( &g_AddressLock );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CreateIPEndpoint - create IP endpoint
//
// Entry:		Pointer to context
//
// Exit:		Pointer to IP endpoint
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CreateEndpoint"

CIPEndpoint	*CreateIPEndpoint( ENDPOINT_POOL_CONTEXT *const pContext )
{
	DNASSERT( g_pIPEndpointPool != NULL );
	return	g_pIPEndpointPool->Get( pContext );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CreateRsip - create an Rsip object
//
// Entry:		Nothing
//
// Exit:		Pointer to Rsip object
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CreateRsip"
/* BUGBUG: [mgere] [xbox] Removed RSIP support
CRsip	*CreateRsip( void )
{
	CRsip	*pRsip;


	DNASSERT( g_pRsipPool != NULL );
	pRsip = g_pRsipPool->Get();
	if ( pRsip != NULL )
	{
		pRsip->AddRef();
	}

	return	pRsip;
}
*/
//**********************************************************************


//**********************************************************************
// ------------------------------
// ReturnRsip - return Rsip object to pool
//
// Entry:		Pointer to Rsip Object
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "ReturnRsip"

// BUGBUG: [mgere] [xbox] Removed RSIP
/*
void	ReturnRsip( CRsip *const pRsip )
{
	DNASSERT( pRsip != NULL );
	DNASSERT( g_pRsipPool != NULL );
	g_pRsipPool->Release( pRsip );
}
*/
//**********************************************************************


//**********************************************************************
// ------------------------------
// CreateSocketPort - create a socket port
//
// Entry:		Nothing
//
// Exit:		Pointer to socket port
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CreateSocketPort"

CSocketPort	*CreateSocketPort( void )
{
	CSocketPort	*pReturn;


	DNASSERT( g_pSocketPortPool != NULL );

	pReturn = g_pSocketPortPool->Get();
	if ( pReturn != NULL )
	{
		pReturn->AddRef();
	}
	
	return	pReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// ReturnSocketPort - return socket port to pool
//
// Entry:		Pointer to socket port
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "ReturnSocketPort"

void	ReturnSocketPort( CSocketPort *const pSocketPort )
{
	DNASSERT( pSocketPort != NULL );
	DNASSERT( g_pSocketPortPool != NULL );
	g_pSocketPortPool->Release( pSocketPort );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CreateThreadPool - create a thread pool
//
// Entry:		Nothing
//
// Exit:		Pointer to thread pool
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CreateThreadPool"

CThreadPool	*CreateThreadPool( void )
{
	CThreadPool	*pReturn;


	DNASSERT( g_pThreadPoolPool != NULL );
	pReturn = g_pThreadPoolPool->Get();
	if ( pReturn != NULL )
	{
		pReturn->AddRef();
	}

	return	pReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// ReturnThreadPool - return a thread pool
//
// Entry:		Pointer to thread pool
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "ReturnThreadPool"

void	ReturnThreadPool( CThreadPool *const pThreadPool )
{
	DNASSERT( pThreadPool != NULL );
	DNASSERT( g_pThreadPoolPool != NULL );
	g_pThreadPoolPool->Release( pThreadPool );
}
//**********************************************************************

