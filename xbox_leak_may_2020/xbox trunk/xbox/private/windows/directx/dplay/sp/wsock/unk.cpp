/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		Unk.cpp
 *  Content:	IUnknown implementation
 *  History:
 *   Date	By	Reason
 *   ====	==	======
 *  08/06/00    RichGr  IA64: Use %p format specifier in DPFs for 32/64-bit pointers and handles.
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

static	STDMETHODIMP DNSP_QueryInterface( IDP8ServiceProvider *lpDNSP, REFIID riid, LPVOID * ppvObj);

//**********************************************************************
// Function definitions
//**********************************************************************

// these are the vtables for IPX and IP.  One or the other is used depending on
// what is passed to DoCreateInstance.  The interfaces are presently the same,
// but are different structures to facilitate potential future changes.
static IDP8ServiceProviderVtbl	ipInterface =
{
	DNSP_QueryInterface,
	DNSP_AddRef,
	DNSP_Release,
	DNSP_Initialize,
	DNSP_Close,
	DNSP_Connect,
	DNSP_Disconnect,
	DNSP_Listen,
	DNSP_SendData,
	DNSP_EnumQuery,
	DNSP_EnumRespond,
	DNSP_CancelCommand,
	DNSP_CreateGroup,
	DNSP_DeleteGroup,
	DNSP_AddToGroup,
	DNSP_RemoveFromGroup,
	DNSP_GetCaps,
	DNSP_SetCaps,
	DNSP_ReturnReceiveBuffers,
	DNSP_GetAddressInfo,
	DNSP_IsApplicationSupported,
	DNSP_ProxyEnumQuery
};

//**********************************************************************
// ------------------------------
// DNSP_QueryInterface - query for interface
//
// Entry:		Pointer to current interface
//				GUID of desired interface
//				Pointer to pointer to new interface
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "DNSP_QueryInterface"

static	STDMETHODIMP DNSP_QueryInterface( IDP8ServiceProvider *lpDNSP, REFIID riid, LPVOID * ppvObj)
{
	HRESULT hr = S_OK;
	 *ppvObj=NULL;

	// hmmm, switch would be cleaner...
		*ppvObj = lpDNSP;
		DNSP_AddRef(lpDNSP);

	return hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CreateIPInterface - create an IP interface
//
// Entry:		Pointer to pointer to SP interface
//				Pointer to associated SP data
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CreateIPInterface"

static	HRESULT CreateIPInterface( IDP8ServiceProvider **const ppiDNSP, CSPData **const ppSPData )
{
	HRESULT 	hr;
	CSPData		*pSPData;

	
	DNASSERT( ppiDNSP != NULL );
	DNASSERT( ppSPData != NULL );

	//
	// initialize
	//
	hr = DPN_OK;
	pSPData = NULL;
	*ppiDNSP = NULL;
	*ppSPData = NULL;

	//
	// create main data class
	//
	hr = CreateSPData( &pSPData, &GUID_NULL, &ipInterface );
	if ( hr != DPN_OK )
	{
		DNASSERT( pSPData != NULL );
		DPF( 0, "Problem creating SPData!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

	DNASSERT( pSPData != NULL );
	*ppiDNSP = pSPData->COMInterface();
	*ppSPData = pSPData;

Exit:
	return hr;

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
// DoCreateInstance - create an instance of an interface
//
// Entry:		Pointer to class factory
//				Pointer to unknown interface
//				Refernce of GUID of desired interface
//				Reference to another GUID?
//				Pointer to pointer to interface
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "DoCreateInstance"

HRESULT DoCreateInstance( LPCLASSFACTORY This,
						  LPUNKNOWN pUnkOuter,
						  REFCLSID rclsid,
						  REFIID riid,
						  LPVOID *ppvObj )
{
	HRESULT			 	hr;
	IDP8ServiceProvider	**ppIDNSP;
	CSPData				*pSPData;


	DNASSERT( ppvObj != NULL );

	//
	// initialize
	//
	*ppvObj = NULL;
	ppIDNSP = NULL;
	pSPData = NULL;

	DBG_CASSERT( sizeof( ppvObj ) == sizeof( ppIDNSP ) );
	ppIDNSP = reinterpret_cast<IDP8ServiceProvider**>( ppvObj );
		hr = CreateIPInterface( ppIDNSP, &pSPData );

	if (hr == S_OK)
	{
		// get the right interface and bump the refcount
		hr = IDP8ServiceProvider_QueryInterface( *ppIDNSP, riid, ppvObj );
		DNASSERT( hr == S_OK );
	}

	//
	// release any outstanding reference on the SP data
	//
	if ( pSPData != NULL )
	{
		pSPData->DecRef();
		pSPData = NULL;
	}

	return hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// IsClassImplemented - determine if a class is implemented in this .DLL
//
// Entry:		Reference to class GUID
//
// Exit:		Boolean indicating whether this .DLL implements the class
//				TRUE = this .DLL implements the class
//				FALSE = this .DLL doesn't implement the class
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "IsClassImplemented"

BOOL IsClassImplemented(REFCLSID rclsid)
{
	return TRUE;
}
//**********************************************************************


