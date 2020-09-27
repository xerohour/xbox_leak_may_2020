/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       addtcp.cpp
 *  Content:    DirectPlay8Address TCP interace file
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *  ====       ==      ======
 * 02/04/2000	rmt		Created
 * 02/12/2000	rmt		Split Get into GetByName and GetByIndex
 * 02/17/2000	rmt		Parameter validation work
 * 02/21/2000	rmt		Updated to make core Unicode and remove ANSI calls
 * 03/21/2000   rmt     Renamed all DirectPlayAddress8's to DirectPlay8Addresses
 *                      Added support for the new ANSI type
 *                      Added SetEqual function
 * 03/24/2000	rmt		Added IsEqual function
 * 04/21/2000   rmt     Bug #32952 - Does not run on Win95 GOLD pre-IE4
 * 05/01/2000   rmt     Bug #33074 - Debug accessing invalid memory
 * 05/17/2000   rmt     Bug #35051 - Incorrect function names in debug spew
 *  06/09/00    rmt     Updates to split CLSID and allow whistler compat
 * 07/21/2000	rmt		Fixed bug w/directplay 4 address parsing
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "dnaddri.h"

//**********************************************************************
// Function prototypes
//**********************************************************************

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_IsEqual"
HRESULT DP8ADDRESSOBJECT::DP8A_IsEqual( IDirectPlay8Address *pInterface, PDIRECTPLAY8ADDRESS pdp8ExternalAddress )
{
	DP8ADDRESSOBJECT *pdp8Address = this;

	RIP_ON_NOT_TRUE( "IsEqual() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	HRESULT hr;
	WCHAR *wszFirstURL = NULL,
		  *wszSecondURL = NULL;
	DWORD dwFirstBufferSize = 0,
	      dwSecondBuffersize = 0;

	DPF( DP8A_ENTERLEVEL, "Enter" );

	RIP_ON_NOT_TRUE( "IsEqual() Invalid pointer specified", pdp8ExternalAddress != NULL );
	RIP_ON_NOT_TRUE( "IsEqual() Invalid object", DP8A_VALID( pdp8ExternalAddress ) );

	hr = pInterface->GetURLW( wszFirstURL, &dwFirstBufferSize );

	if( hr != DPNERR_BUFFERTOOSMALL )
	{
		DPF( 0, "Could not get URL size for current object hr=[0x%lx]", hr );
		goto ISEQUAL_ERROR;
	}

	wszFirstURL = new WCHAR[dwFirstBufferSize];

	if( wszFirstURL == NULL )
	{
		DPF( 0, "Error allocating memory hr=[0x%lx]", hr );
		goto ISEQUAL_ERROR;
	}

	hr = pInterface->GetURLW( wszFirstURL, &dwFirstBufferSize );

	if( FAILED( hr ) )
	{
		DPF( 0, "Could not get URL for current object hr=[0x%lx]", hr );
		goto ISEQUAL_ERROR;
	}

	hr = pdp8ExternalAddress->GetURLW( wszSecondURL, &dwSecondBuffersize );

	if( hr != DPNERR_BUFFERTOOSMALL )
	{
		DPF( 0, "Could not get URL size for exterior object hr=[0x%lx]", hr );
		goto ISEQUAL_ERROR;
	}

	wszSecondURL = new WCHAR[dwSecondBuffersize];

	if( wszSecondURL == NULL )
	{
		DPF( 0, "Error allocating memory hr=[0x%lx]", hr );
		goto ISEQUAL_ERROR;
	}

	hr = pdp8ExternalAddress->GetURLW( wszSecondURL, &dwSecondBuffersize );

	if( FAILED( hr ) )
	{
		DPF( 0, "Could not get URL for exterior object hr=[0x%lx]", hr );
		goto ISEQUAL_ERROR;
	}

	if( _wcsicmp( wszFirstURL, wszSecondURL ) == 0 )
	{
		hr = DPNSUCCESS_EQUAL;
	}
	else
	{
		hr = DPNSUCCESS_NOTEQUAL;
	}

ISEQUAL_ERROR:

	if( wszFirstURL != NULL )
		delete [] wszFirstURL;

	if( wszSecondURL != NULL )
		delete [] wszSecondURL;

	DP8A_RETURN( hr );

}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_SetEqual"
HRESULT DP8ADDRESSOBJECT::DP8A_SetEqual( IDirectPlay8Address *pInterface, PDIRECTPLAY8ADDRESS pdp8ExternalAddress )
{
	RIP_ON_NOT_TRUE( "SetEqual() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;

	HRESULT hr;
	WCHAR *wszURLBuffer = NULL;
	DWORD dwBufferSize = 0;

	DPF( DP8A_ENTERLEVEL, "Enter" );

	RIP_ON_NOT_TRUE( "SetEqual() Invalid pointer specified", pdp8ExternalAddress != NULL );
	RIP_ON_NOT_TRUE( "SetEqual() Invalid object", DP8A_VALID( pdp8ExternalAddress ) );

    // Get ourselves a reference for duration of the call
	pdp8ExternalAddress->AddRef();

	hr = pdp8ExternalAddress->GetURLW( wszURLBuffer, &dwBufferSize );

	if( hr != DPNERR_BUFFERTOOSMALL )
	{
	    DPF( DP8A_ERRORLEVEL, "Error getting contents of passed address hr=0x%x", hr );
        goto SETEQUAL_CLEANUP;
	}

	wszURLBuffer = new WCHAR[dwBufferSize];

	if( wszURLBuffer == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( DP8A_ERRORLEVEL, "Error allocating memory" );
		goto SETEQUAL_CLEANUP;
	}

	hr = pdp8ExternalAddress->GetURLW( wszURLBuffer, &dwBufferSize );

	if( FAILED( hr ) )
	{
	    DPF( DP8A_ERRORLEVEL, "Error getting contents of passed address w/buffer hr=0x%x", hr );
        goto SETEQUAL_CLEANUP;
	}

	hr = pdp8Address->SetURL( wszURLBuffer );

	if( FAILED( hr ) )
	{
	    DPF( DP8A_ERRORLEVEL, "Error setting address to match passed address hr=0x%x", hr );
        goto SETEQUAL_CLEANUP;
	}

SETEQUAL_CLEANUP:

    pdp8ExternalAddress->Release();

    if( wszURLBuffer != NULL )
        delete [] wszURLBuffer;

    DP8A_RETURN( hr );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_BuildFromDirectPlay4Address"
HRESULT DP8ADDRESSOBJECT::DP8A_BuildFromDirectPlay4Address( IDirectPlay8Address *pInterface, void * pvDataBuffer, DWORD dwDataSize )
{
    DP8A_RETURN( DPNERR_UNSUPPORTED );
}

// DP8A_BuildFromURLA
//
// Initializes this object with URL specified in ANSI
//
#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_BuildFromURLA"
HRESULT DP8ADDRESSOBJECT::DP8A_BuildFromURLA( IDirectPlay8Address *pInterface, CHAR * pszAddress )
{
	RIP_ON_NOT_TRUE( "BuildFromURLA() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;

	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	RIP_ON_NOT_TRUE( "BuildFromURLA() Invalid pointer to address.  An address must be specified", pszAddress != NULL );
	RIP_ON_NOT_TRUE( "BuildFromURLA() Invalid string specified for address", DNVALID_STRING_A( pszAddress ) );

	DPF( DP8A_PARAMLEVEL, "pszAddress = %s", pszAddress );

	WCHAR *szShadowBuffer = NULL;

	DWORD dwStrSize = 0;

	if( pszAddress != NULL )
	{
		dwStrSize = strlen(pszAddress)+1;
		
		szShadowBuffer = new WCHAR[dwStrSize];

		if( szShadowBuffer == NULL )
		{
			DPF( DP8A_ERRORLEVEL, "Error allocating memory" );
			hr = DPNERR_OUTOFMEMORY;
			goto BUILDFROMURLW_RETURN;
		}

		if( FAILED( hr = STR_jkAnsiToWide( szShadowBuffer, pszAddress, dwStrSize ) )  )
		{
			DPF( DP8A_ERRORLEVEL, "Error converting URL to ANSI hr=0x%x", hr );
			hr = DPNERR_CONVERSION;
			goto BUILDFROMURLW_RETURN;
		}
	}

	hr = pdp8Address->SetURL( szShadowBuffer );

BUILDFROMURLW_RETURN:

	if( szShadowBuffer )
		delete [] szShadowBuffer;

	DP8A_RETURN( hr );	
}

// DP8A_BuildFromURLW
//
// Initializes this object with URL specified in Unicode
//
#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_BuildFromURLW"
HRESULT DP8ADDRESSOBJECT::DP8A_BuildFromURLW( IDirectPlay8Address *pInterface, WCHAR * pwszAddress )

{
	RIP_ON_NOT_TRUE( "BuildFromURLW() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;

	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	RIP_ON_NOT_TRUE( "BuildFromURLW() Invalid pointer to address.  An address must be specified", pwszAddress != NULL );
	RIP_ON_NOT_TRUE( "BuildFromURLW() Invalid string specified for address", DNVALID_STRING_W( pwszAddress ) );

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pwszAddress = 0x%p", pwszAddress );

	hr = pdp8Address->SetURL( pwszAddress );

	DP8A_RETURN( hr );	
}

// DP8A_Duplicate
//
// Creates and initializes another address object as a duplicate to this one.
//
#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_Duplicate"
HRESULT DP8ADDRESSOBJECT::DP8A_Duplicate( IDirectPlay8Address *pInterface, PDIRECTPLAY8ADDRESS *ppInterface )
{
	RIP_ON_NOT_TRUE( "Duplicate() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	HRESULT hr;

	WCHAR *szTmpURL = NULL;
	DWORD dwURLSize = 0;
	LPDIRECTPLAY8ADDRESS lpdp8Address = NULL;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	RIP_ON_NOT_TRUE( "Duplicate() Invalid pointer to pointer specified in ppInterface", !( ppInterface == NULL || !DNVALID_WRITEPTR( ppInterface, sizeof(LPVOID) ) ) );

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "ppInterface = 0x%p", ppInterface );	

	hr = pdp8Address->BuildURL( szTmpURL, &dwURLSize );

	if( hr != DPNERR_BUFFERTOOSMALL )
	{
		DPF( DP8A_ERRORLEVEL, "BuildURL from object failed hr=0x%x", hr );
		goto DUPLICATE_FAIL;
	}

	szTmpURL = new WCHAR[dwURLSize];

	if( szTmpURL == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( DP8A_ERRORLEVEL, "Memory alloc failure" );
		goto DUPLICATE_FAIL;	
	}

	hr = pdp8Address->BuildURL( szTmpURL, &dwURLSize );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "BuildURL from object failed hr=0x%x", hr );
		goto DUPLICATE_FAIL;
	}
	
	hr = DirectPlay8AddressCreate( IID_IDirectPlay8Address, (void **) &lpdp8Address, NULL );

	if( FAILED( hr ) )
    {
		DPF( DP8A_ERRORLEVEL, "CoCreate failed hr=0x%x", hr );
		goto DUPLICATE_FAIL;
    }

    hr = lpdp8Address->BuildFromURLW( szTmpURL );

    if( FAILED( hr ) )
    {
    	DPF( DP8A_ERRORLEVEL, "BuildFailed hr=0x%x", hr );
    	goto DUPLICATE_FAIL;
    }

    *ppInterface = lpdp8Address;

	if( szTmpURL != NULL )
		delete [] szTmpURL;

    return DPN_OK;

DUPLICATE_FAIL:

	if( lpdp8Address != NULL )
		lpdp8Address->Release();

	if( szTmpURL != NULL )
		delete [] szTmpURL;

	return hr;
	
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_GetURLA"
HRESULT DP8ADDRESSOBJECT::DP8A_GetURLA( IDirectPlay8Address *pInterface, CHAR * pszAddress, PDWORD pdwAddressSize )

{
	RIP_ON_NOT_TRUE( "GetURLA() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;

	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	RIP_ON_NOT_TRUE( "GetURLA() Invalid pointer specified for address size", !( pdwAddressSize == NULL || !DNVALID_WRITEPTR( pdwAddressSize, sizeof(DWORD) ) ) );
	RIP_ON_NOT_TRUE( "GetURLA() Invalid pointer specified for address", !( *pdwAddressSize > 0 && (pszAddress == NULL || !DNVALID_WRITEPTR( pszAddress, (*pdwAddressSize) ) ) ) );

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pwszAddress = 0x%p pdwAddressSize = 0x%p (%u)",
	     pszAddress , pdwAddressSize, *pdwAddressSize );

	WCHAR *szShadowBuffer = NULL;

	if( *pdwAddressSize  != 0 )
	{
		szShadowBuffer = new WCHAR[*pdwAddressSize];

		if( szShadowBuffer == NULL )
		{
			DPF( DP8A_ERRORLEVEL, "Error allocating memory" );
			hr = DPNERR_OUTOFMEMORY;
			goto GETURLW_RETURN;
		}
	}
	else
	{	
		szShadowBuffer= NULL;
	}

	hr = pdp8Address->BuildURL( szShadowBuffer, pdwAddressSize );

	if( hr == DPN_OK )
	{
		if( FAILED( hr = STR_jkWideToAnsi( pszAddress, szShadowBuffer, *pdwAddressSize ) ) )
		{
			DPF( DP8A_ERRORLEVEL, "Error converting ANSI->WIDE hr=0x%x", hr );
			hr = DPNERR_CONVERSION;
			goto GETURLW_RETURN;
		}
	}
	
GETURLW_RETURN:

	if( szShadowBuffer != NULL )
		delete [] szShadowBuffer;

	DP8A_RETURN( hr );
}

// DP8A_GetURLW
//
// Retrieves the URL represented by this object in Unicode format
//
#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_GetURLW"
HRESULT DP8ADDRESSOBJECT::DP8A_GetURLW( IDirectPlay8Address *pInterface, WCHAR * pwszAddress, PDWORD pdwAddressSize )
{
	RIP_ON_NOT_TRUE( "GetURLW() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;

	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	RIP_ON_NOT_TRUE( "GetURLW() Invalid pointer specified for address size", !( pdwAddressSize == NULL || !DNVALID_WRITEPTR( pdwAddressSize, sizeof(DWORD) ) ) );
	RIP_ON_NOT_TRUE( "GetURLW() Invalid pointer specified for address", !( *pdwAddressSize > 0 && (pwszAddress == NULL || !DNVALID_WRITEPTR( pwszAddress, (*pdwAddressSize)*sizeof(WCHAR) ) ) ) );

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pwszAddress = 0x%p pdwAddressSize = 0x%p (%u)",
	     pwszAddress, pdwAddressSize, *pdwAddressSize );

	hr = pdp8Address->BuildURL( pwszAddress, pdwAddressSize );

	DP8A_RETURN( hr );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_GetSP"
HRESULT DP8ADDRESSOBJECT::DP8A_GetSP( IDirectPlay8Address *pInterface, GUID * pguidSP )
{
	RIP_ON_NOT_TRUE( "GetSP() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pguidSP = 0x%p ", pguidSP );

	hr = pdp8Address->GetSP( pguidSP );

	DP8A_RETURN( hr );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_GetUserData"
HRESULT DP8ADDRESSOBJECT::DP8A_GetUserData( IDirectPlay8Address *pInterface, void * pBuffer, PDWORD pdwBufferSize )
{
	RIP_ON_NOT_TRUE( "GetUserData() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	RIP_ON_NOT_TRUE( "GetUserData() Invalid pointer specified for pdwBufferSize", !( pdwBufferSize == NULL || !DNVALID_WRITEPTR( pdwBufferSize, sizeof( DWORD ) ) ) );
	RIP_ON_NOT_TRUE( "GetUserData() Invalid pointer specified for pdwBufferSize", !( *pdwBufferSize > 0 && (pBuffer == NULL || !DNVALID_WRITEPTR( pBuffer, *pdwBufferSize ) ) ) );

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pBuffer = 0x%p pdwBufferSize = 0x%p(%u) ", pBuffer, pdwBufferSize, *pdwBufferSize );

	hr = pdp8Address->GetUserData( pBuffer, pdwBufferSize );

	DP8A_RETURN( hr );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_SetSP"
HRESULT DP8ADDRESSOBJECT::DP8A_SetSP( IDirectPlay8Address *pInterface, const GUID * const pguidSP )
{
	RIP_ON_NOT_TRUE( "SetSP() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pguidSP = 0x%p", pguidSP );

	hr = pdp8Address->SetSP( pguidSP );

	DP8A_RETURN( hr );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_GetDevice"
HRESULT DP8ADDRESSOBJECT::DP8A_GetDevice( IDirectPlay8Address *pInterface, GUID * pguidSP )
{
	RIP_ON_NOT_TRUE( "GetDevice() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pguidDevice = 0x%p", pguidSP );	

	hr = pdp8Address->GetDevice( pguidSP );

	DP8A_RETURN( hr );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_SetDevice"
HRESULT DP8ADDRESSOBJECT::DP8A_SetDevice( IDirectPlay8Address *pInterface, const GUID * const pguidSP )
{
	RIP_ON_NOT_TRUE( "SetDevice() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pguidDevice = 0x%p", pguidSP );		

	hr = pdp8Address->SetDevice( pguidSP );
	RIP_ON_NOT_TRUE( "AddComponentW() Invalid parameters", hr != DPNERR_INVALIDPARAM );

	DP8A_RETURN( hr );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_SetUserData"
HRESULT DP8ADDRESSOBJECT::DP8A_SetUserData( IDirectPlay8Address *pInterface, const void * const pBuffer, const DWORD dwBufferSize )
{
	RIP_ON_NOT_TRUE( "SetUserData() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	RIP_ON_NOT_TRUE( "SetUserData() Invalid pointer specified for pBuffer", !( dwBufferSize > 0 && (pBuffer == NULL || !DNVALID_READPTR( pBuffer, dwBufferSize ) ) ) );

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pBuffer = 0x%p dwBufferSize = %u", pBuffer, dwBufferSize );		

	hr = pdp8Address->SetUserData( pBuffer, dwBufferSize );

	DP8A_RETURN( hr );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_GetNumComponents"
HRESULT DP8ADDRESSOBJECT::DP8A_GetNumComponents( IDirectPlay8Address *pInterface, PDWORD pdwNumComponents )
{
	RIP_ON_NOT_TRUE( "GetNumComponents() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	RIP_ON_NOT_TRUE( "GetNumComponents() Invalid ptr for num of components", !( pdwNumComponents == NULL || !DNVALID_WRITEPTR( pdwNumComponents, sizeof(DWORD) ) ) );

	*pdwNumComponents = pdp8Address->GetNumComponents();

	DP8A_RETURN( DPN_OK );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_GetComponentByNameW"
HRESULT DP8ADDRESSOBJECT::DP8A_GetComponentByNameW( IDirectPlay8Address *pInterface, const WCHAR * const pwszTag, void * pComponentBuffer, PDWORD pdwComponentSize, PDWORD pdwDataType )
{
	RIP_ON_NOT_TRUE( "GetComponentByNameW() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	RIP_ON_NOT_TRUE( "GetComponentByNameW() Invalid pointer to tag.  A name must be specified", pwszTag != NULL );
	RIP_ON_NOT_TRUE( "GetComponentByNameW() Invalid Pointer to data size", !( pdwComponentSize == NULL || !DNVALID_WRITEPTR( pdwComponentSize, sizeof(DWORD)) ) );
	RIP_ON_NOT_TRUE( "GetComponentByNameW() Invalid pointer to data type", !( pdwDataType == NULL || !DNVALID_READPTR( pdwDataType, sizeof(DWORD)) ) );
	RIP_ON_NOT_TRUE( "GetComponentByNameW() Invalid pointer to component data", !( *pdwComponentSize > 0 && (pComponentBuffer == NULL || !DNVALID_WRITEPTR( pComponentBuffer, *pdwComponentSize ) ) ) );
	RIP_ON_NOT_TRUE( "GetComponentByNameW() Invalid string specified for tag", DNVALID_STRING_W( pwszTag ) );

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pwszTag = 0x%p pComponentBuffer = 0x%p, pdwComponentSize = 0x%p, pdwDataType = 0x%p",
		(pwszTag==NULL) ? NULL : pwszTag, pComponentBuffer, pdwComponentSize, pdwDataType );

	hr = pdp8Address->GetElement( pwszTag, pComponentBuffer, pdwComponentSize, pdwDataType );

	DP8A_RETURN( hr );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_GetComponentByIndexW"
HRESULT DP8ADDRESSOBJECT::DP8A_GetComponentByIndexW( IDirectPlay8Address *pInterface,
	const DWORD dwComponentID, WCHAR * pwszTag, PDWORD pdwNameLen,
	void * pComponentBuffer, PDWORD pdwComponentSize, PDWORD pdwDataType )
{
	RIP_ON_NOT_TRUE( "GetComponentByIndexW() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	
	
	RIP_ON_NOT_TRUE( "GetComponentByIndexW() Invalid pointer specified for pdwNameLen", !( pdwNameLen == NULL || !DNVALID_WRITEPTR( pdwNameLen, sizeof(DWORD) ) ) );
	RIP_ON_NOT_TRUE( "GetComponentByIndexW() Invalid pointer specified for pwszTag", !( *pdwNameLen != 0 && (pwszTag == NULL || !DNVALID_WRITEPTR( pwszTag, *pdwNameLen*sizeof(WCHAR) ) ) ) );
	RIP_ON_NOT_TRUE( "GetComponentByIndexW() Invalid pointer specified for pdwComponentSize", !( pdwComponentSize == NULL || !DNVALID_WRITEPTR( pdwComponentSize, sizeof(DWORD) ) ) );
	RIP_ON_NOT_TRUE( "GetComponentByIndexW() Invalid pointer specified for pwszTag", !( *pdwComponentSize != 0 && (pComponentBuffer == NULL || !DNVALID_WRITEPTR( pComponentBuffer, *pdwComponentSize ) ) ) );
	RIP_ON_NOT_TRUE( "GetComponentByIndexW() Invalid pointer specified for pdwDataType", !( pdwDataType == NULL || !DNVALID_WRITEPTR( pdwDataType, sizeof(DWORD) ) ) );

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "dwComponentID = %u pwszTag = 0x%p pdwNameLen = 0x%p (%u)  pComponentBuffer = 0x%p, pdwComponentSize = 0x%p (%u), pdwDataType = 0x%p",
		dwComponentID, pwszTag, pdwNameLen, *pdwNameLen, pComponentBuffer, pdwComponentSize, *pdwComponentSize, pdwDataType );

	hr = pdp8Address->GetElement( dwComponentID, pwszTag, pdwNameLen, pComponentBuffer, pdwComponentSize, pdwDataType );

	DP8A_RETURN( hr );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_AddComponentW"
HRESULT DP8ADDRESSOBJECT::DP8A_AddComponentW( IDirectPlay8Address *pInterface, const WCHAR * const pwszTag, const void * const pComponentData, const DWORD dwComponentSize, const DWORD dwDataType )

{
	RIP_ON_NOT_TRUE( "AddComponentW() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	RIP_ON_NOT_TRUE( "AddComponentW() Invalid pointer for tag string", pwszTag != NULL );
	RIP_ON_NOT_TRUE( "AddComponentW() Invalid pointer specified for component", !( pComponentData == NULL || !DNVALID_READPTR( pComponentData, dwComponentSize ) ) );
	RIP_ON_NOT_TRUE( "AddComponentW() Invalid string specified for tag", DNVALID_STRING_W( pwszTag ) );
	RIP_ON_NOT_TRUE( "AddComponentW() Invalid datatype specified", !( dwDataType != DPNA_DATATYPE_STRING && dwDataType != DPNA_DATATYPE_DWORD && dwDataType != DPNA_DATATYPE_GUID && dwDataType != DPNA_DATATYPE_BINARY && dwDataType != DPNA_DATATYPE_STRING_ANSI ) );
	RIP_ON_NOT_TRUE( "AddComponentW() Invalid string component specified", !( ( dwDataType == DPNA_DATATYPE_STRING ) && !DNVALID_STRING_W( (const WCHAR * const) pComponentData ) ) );
	RIP_ON_NOT_TRUE( "AddComponentW() String size and component size don't match", !( ( dwDataType == DPNA_DATATYPE_STRING ) && ( ((wcslen( (const WCHAR * const) pComponentData)+1)*sizeof(WCHAR)) != dwComponentSize ) ) );
	RIP_ON_NOT_TRUE( "AddComponentW() Invalid string component specified", !( ( dwDataType == DPNA_DATATYPE_STRING_ANSI ) && !DNVALID_STRING_A( (const CHAR * const) pComponentData ) ) );
	RIP_ON_NOT_TRUE( "AddComponentW() String size and component size don't match", !( ( dwDataType == DPNA_DATATYPE_STRING_ANSI ) && ( ((strlen( (const CHAR * const) pComponentData)+1)*sizeof(char)) != dwComponentSize ) ) );
	RIP_ON_NOT_TRUE( "AddComponentW() Invalid size for DWORD component", !( ( dwDataType == DPNA_DATATYPE_DWORD ) && ( dwComponentSize != sizeof( DWORD ) ) ) );
	RIP_ON_NOT_TRUE( "AddComponentW() Invalid size for GUID component", !( ( dwDataType == DPNA_DATATYPE_GUID ) && ( dwComponentSize != sizeof( GUID ) ) ) );

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pwszTag = 0x%p pComponentData = 0x%p dwComponentSize = %d dwDataType = %d",
	     pwszTag, pComponentData, dwComponentSize, dwDataType );

	hr = pdp8Address->SetElement( pwszTag, pComponentData, dwComponentSize, dwDataType );
	RIP_ON_NOT_TRUE( "AddComponentW() Invalid parameters", hr != DPNERR_INVALIDPARAM );

	DP8A_RETURN( hr );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8A_Clear"
HRESULT DP8ADDRESSOBJECT::DP8A_Clear( IDirectPlay8Address *pInterface )
{
	RIP_ON_NOT_TRUE( "Clear() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );	

	hr = pdp8Address->Clear(  );

	DP8A_RETURN( hr );
}

BOOL IsValidDP8AObject( LPVOID lpvObject )
{
// BUGBUG: [mgere] [xbox] Revisit this later.  For now always return TRUE
/*
	INTERFACE_LIST *pIntList = (INTERFACE_LIST *) lpvObject;
	
	if( !DNVALID_READPTR( lpvObject, sizeof( INTERFACE_LIST ) ) )
	{
		DPF( DP8A_ERRORLEVEL, "Invalid object pointer" );
		return FALSE;
	}

	if( pIntList->lpVtbl != &DP8A_BaseVtbl &&
	   pIntList->lpVtbl != &DP8A_IPVtbl &&
	   pIntList->lpVtbl != &DP8A_InternalVtbl &&
	   pIntList->lpVtbl != &DP8A_UnknownVtbl )
	{
		DPF( DP8A_ERRORLEVEL, "Invalid object" );
		return FALSE;
	}

	if( pIntList->iid != IID_IDirectPlay8Address &&
	   pIntList->iid != IID_IDirectPlay8AddressIP &&
	   pIntList->iid != IID_IDirectPlay8AddressInternal &&
	   pIntList->iid != IID_IUnknown )
	{
		DPF( DP8A_ERRORLEVEL, "Unknown object" );
		return FALSE;
	}

	if( pIntList->lpObject == NULL ||
	   !DNVALID_READPTR( pIntList->lpObject, sizeof( OBJECT_DATA ) ) )
	{
		DPF( DP8A_ERRORLEVEL, "Invalid object" );
		return FALSE;
	}

	DP8ADDRESSOBJECT *pdp8Address = (PDP8ADDRESSOBJECT) GET_OBJECT_FROM_INTERFACE( lpvObject );

	if( pdp8Address == NULL ||
	   !DNVALID_READPTR( pdp8Address, sizeof( DP8ADDRESSOBJECT ) ) )
	{
		DPF( DP8A_ERRORLEVEL, "Invalid object" );
		return FALSE;
	}
*/
	return TRUE;

}



