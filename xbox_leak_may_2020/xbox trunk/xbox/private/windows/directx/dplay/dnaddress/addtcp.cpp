/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       addtcp.cpp
 *  Content:    DirectPlay8Address TCP interface file
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *  ====       ==      ======
 * 02/04/2000	rmt		Created
 * 02/12/2000	rmt		Completed first implementation
 * 02/17/2000	rmt		Parameter validation work
 * 02/20/2000	rmt		Changed ports to USHORTs
 * 02/21/2000	 rmt	Updated to make core Unicode and remove ANSI calls
 * 02/23/2000	rmt		Further parameter validation
 * 03/21/2000   rmt     Renamed all DirectPlayAddress8's to DirectPlay8Addresses
 * 03/24/2000	rmt		Added IsEqual function
 *	05/04/00	mjn		Fixed leak in DP8ATCP_GetSockAddress()
 *  06/09/00    rmt     Updates to split CLSID and allow whistler compat and support external create funcs
 * 08/03/2000 	rmt		Bug #41246 - Remove IP versions of Duplicate, SetEqual, IsEqual, BuildURL
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "dnaddri.h"

HRESULT IDirectPlay8Address_BuildFromSockAddr(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, const SOCKADDR * const pSockAddr)
{
    return DP8ADDRESSOBJECT::GetDirectPlay8Address(pDirectPlay8Address)->DP8ATCP_BuildFromSockAddr(pDirectPlay8Address, pSockAddr);
}

HRESULT IDirectPlay8Address_BuildAddress(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, const WCHAR * const wszAddress, const USHORT usPort)
{
    return DP8ADDRESSOBJECT::GetDirectPlay8Address(pDirectPlay8Address)->DP8ATCP_BuildAddressW(pDirectPlay8Address, wszAddress, usPort);
}

HRESULT IDirectPlay8Address_BuildLocalAddress(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, const GUID * const pguidAdapter, const USHORT usPort)
{
    return DP8ADDRESSOBJECT::GetDirectPlay8Address(pDirectPlay8Address)->DP8ATCP_BuildLocalAddress(pDirectPlay8Address, pguidAdapter, usPort);
}

HRESULT IDirectPlay8Address_GetSockAddress(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, SOCKADDR *pSockAddr, PDWORD pDword)
{
    return DP8ADDRESSOBJECT::GetDirectPlay8Address(pDirectPlay8Address)->DP8ATCP_GetSockAddress(pDirectPlay8Address, pSockAddr, pDword);
}

HRESULT IDirectPlay8Address_GetLocalAddress(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, GUID *pguidAdapter, USHORT *pusPort)
{
    return DP8ADDRESSOBJECT::GetDirectPlay8Address(pDirectPlay8Address)->DP8ATCP_GetLocalAddress(pDirectPlay8Address, pguidAdapter, pusPort);
}

HRESULT IDirectPlay8Address_GetAddress(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, WCHAR *wszAddress, PDWORD pdwAddressLength, USHORT *psPort)
{
    return DP8ADDRESSOBJECT::GetDirectPlay8Address(pDirectPlay8Address)->DP8ATCP_GetAddressW(pDirectPlay8Address, wszAddress, pdwAddressLength, psPort);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DP8ATCP_BuildLocalAddress"
HRESULT DP8ADDRESSOBJECT::DP8ATCP_BuildLocalAddress( IDirectPlay8Address *pInterface, const GUID * const pguidAdapter, const USHORT usPort )
{
	RIP_ON_NOT_TRUE( "BuildLocalAddress() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );		
	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pguidAdapter: 0x%p usPort: %u", pguidAdapter, (DWORD)usPort );	

	RIP_ON_NOT_TRUE( "BuildLocalAddress() Invalid pointer", !( pguidAdapter == NULL || !DNVALID_READPTR( pguidAdapter, sizeof( GUID ) ) ) );

	hr = pdp8Address->Clear();

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Failed to clear current address hr=0x%x", hr );
		DP8A_RETURN( hr );	
	}

// BUGBUG: [mgere] [xbox] Must set the SP sometime...
//	hr = pdp8Address->SetSP( &CLSID_DP8SP_TCPIP );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Error setting service provider hr=0x%x", hr );
		DP8A_RETURN( hr );	
	}

	hr = pdp8Address->SetDevice( pguidAdapter );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Error setting device hr=0x%x", hr );
		DP8A_RETURN( hr );	
	}

	DWORD dwTmpPort = usPort;

	hr = pdp8Address->SetElement( DPNA_KEY_PORT, &dwTmpPort, sizeof( DWORD ), DPNA_DATATYPE_DWORD );

	if( FAILED( hr ) )
	{
		DPF( 0, "Adding SP element failed hr=0x%x", hr );
		DP8A_RETURN( hr );	
	}
	
	DP8A_RETURN( hr );	
}


#undef DPF_MODNAME
#define DPF_MODNAME "DP8ATCP_BuildFromSockAddr"
HRESULT DP8ADDRESSOBJECT::DP8ATCP_BuildFromSockAddr( IDirectPlay8Address *pInterface, const SOCKADDR * const pSockAddr )
{
	RIP_ON_NOT_TRUE( "BuildFromSockAddr() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );
	
	DP8ADDRESSOBJECT *pdp8Address = this;
	
	HRESULT hr;
	DWORD dwTmpPort;
	LPSTR szHostName = NULL;
	LPWSTR swzHostName = NULL;
	DWORD dwTmpLength;
	sockaddr_in *saIPAddress;

	DPF( DP8A_ENTERLEVEL, "Enter" );
	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pSockAddr: 0x%p", pSockAddr );	

	RIP_ON_NOT_TRUE( "BuildFromSockAddr() Invalid pointer to sockaddr", !( pSockAddr == NULL || !DNVALID_READPTR( pSockAddr, sizeof( SOCKADDR ) ) ) );
	RIP_ON_NOT_TRUE( "BuildFromSockAddr() Only TCP addresses are supported", pSockAddr->sa_family == AF_INET );

	saIPAddress = (sockaddr_in * ) pSockAddr;

	hr = pdp8Address->Clear();

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Failed clearing object hr=0x%x", hr );
		goto BUILDFROMSOCKADDR_ERROR;
	}

// BUGBUG: [mgere] [xbox] Must set the SP sometime...
//	hr = pdp8Address->SetSP( &CLSID_DP8SP_TCPIP );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Error setting service provider hr=0x%x", hr );
		DP8A_RETURN( hr );	
	}

	// Sockaddr is in network byte order, convert to host order
	dwTmpPort = ntohs(saIPAddress->sin_port);

	szHostName = inet_ntoa( saIPAddress->sin_addr );

	if( szHostName == NULL )
	{
		DPF( DP8A_ERRORLEVEL, "Error converting from address to string" );
		hr = DPNERR_INVALIDPARAM;
		goto BUILDFROMSOCKADDR_ERROR;
	}

	dwTmpLength = strlen(szHostName)+1;

	swzHostName = new WCHAR[dwTmpLength];

	if( swzHostName == NULL )
	{
		DPF( DP8A_ERRORLEVEL, "Error allocating memory" );
		hr = DPNERR_OUTOFMEMORY;
		goto BUILDFROMSOCKADDR_ERROR;
	}

	if( FAILED( hr = STR_jkAnsiToWide(swzHostName,szHostName,dwTmpLength) ) )
	{
		DPF( DP8A_ERRORLEVEL, "Error converting hostname 0x%x", hr );
		hr = DPNERR_CONVERSION;
		goto BUILDFROMSOCKADDR_ERROR;
	}

	hr = pdp8Address->SetElement( DPNA_KEY_HOSTNAME, swzHostName, dwTmpLength*sizeof(WCHAR), DPNA_DATATYPE_STRING );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Failed to set hostname hr=0x%x", hr );
		DP8A_RETURN( hr );
	}	

	hr = pdp8Address->SetElement( DPNA_KEY_PORT, &dwTmpPort, sizeof(DWORD), DPNA_DATATYPE_DWORD );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ENTERLEVEL, "Failed setting port hr=0x%x", hr );
		DP8A_RETURN( hr );
	}

	delete [] swzHostName;

	DP8A_RETURN( DPN_OK );

BUILDFROMSOCKADDR_ERROR:

	if( swzHostName != NULL )
		delete [] swzHostName;

	DP8A_RETURN( hr );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8ATCP_BuildAddressW"
HRESULT DP8ADDRESSOBJECT::DP8ATCP_BuildAddressW( IDirectPlay8Address *pInterface, const WCHAR * const pwszAddress, const USHORT usPort )
{
	RIP_ON_NOT_TRUE( "BuildAddressW() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );

	RIP_ON_NOT_TRUE( "BuildAddressW() Invalid pointer to address", pwszAddress != NULL );
	RIP_ON_NOT_TRUE( "BuildAddressW() Invalid string for address", DNVALID_STRING_W( pwszAddress ) );

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pwszAddress: 0x%p, usPort = %u", pwszAddress, (DWORD)usPort );

	hr = pdp8Address->Clear();

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Error clearing current address hr=0x%x", hr );
		DP8A_RETURN( hr );		
	}

// BUGBUG: [mgere] [xbox] Must set the SP sometime...
//	hr = pdp8Address->SetSP( &CLSID_DP8SP_TCPIP );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Error setting service provider hr=0x%x", hr );
		DP8A_RETURN( hr );	
	}

	hr = pdp8Address->SetElement( DPNA_KEY_HOSTNAME, pwszAddress, (wcslen(pwszAddress)+1)*sizeof(WCHAR), DPNA_DATATYPE_STRING );

	if( FAILED( hr ) )
	{
		DPF( 0, "Adding SP element failed hr=0x%x", hr );
		DP8A_RETURN( hr );	
	}	

	DWORD dwTmpPort = usPort;
	
	hr = pdp8Address->SetElement( DPNA_KEY_PORT, &dwTmpPort, sizeof( DWORD ), DPNA_DATATYPE_DWORD );

	if( FAILED( hr ) )
	{
		DPF( 0, "Adding SP element failed hr=0x%x", hr );
		DP8A_RETURN( hr );	
	}
	
	DP8A_RETURN( hr );	
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8ATCP_GetSockAddress"
HRESULT DP8ADDRESSOBJECT::DP8ATCP_GetSockAddress( IDirectPlay8Address *pInterface, SOCKADDR *pSockAddr, PDWORD pdwBufferSize )
{
	RIP_ON_NOT_TRUE( "GetSockAddress() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	HRESULT hr;
	WCHAR *swzAddress = NULL;		// Unicode version of hostname
	CHAR *szAddress = NULL; 		// ANSI version of hostname
	DWORD dwAddressSize = 0;
	USHORT usPort;
	LPHOSTENT lpHostEntry;
	in_addr iaTmp;
	in_addr *piaTmp;
	DWORD dwIndex;
	DWORD dwRequiredSize;
	DWORD dwNumElements;
	sockaddr_in *psinCurAddress;
	SOCKADDR *pCurLoc;
	GUID guidSP;

	dwAddressSize = 0;

	RIP_ON_NOT_TRUE( "GetSockAddress() Invalid pointer for pdwBufferSize", !( pdwBufferSize == NULL || !DNVALID_WRITEPTR( pdwBufferSize, sizeof( DWORD ) ) ) );
	RIP_ON_NOT_TRUE( "GetSockAddress() Invalid pointer for sockaddress", !( *pdwBufferSize > 0 && (pSockAddr == NULL || !DNVALID_WRITEPTR( pSockAddr, *pdwBufferSize ) ) ) );

	DPF( DP8A_ENTERLEVEL, "Enter" );
	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pSockAddr = 0x%p, pdwBufferSize = 0x%p (%d)", pSockAddr, pdwBufferSize, *pdwBufferSize );	

	hr = DP8ATCP_GetAddressW( pInterface, swzAddress, &dwAddressSize, &usPort );

	if( hr != DPNERR_BUFFERTOOSMALL )
	{
		DPF( DP8A_ERRORLEVEL, "Unable to retrieve size required hr=0x%x", hr );
		goto GETSOCKADDRESS_ERROR;
	}

	swzAddress = new WCHAR[dwAddressSize];

	if( swzAddress == NULL )
	{
		DPF( DP8A_ERRORLEVEL, "Error allocating memory hr=0x%x", hr );
		hr = DPNERR_OUTOFMEMORY;
		goto GETSOCKADDRESS_ERROR;
	}

	hr = DP8ATCP_GetAddressW( pInterface, swzAddress, &dwAddressSize, &usPort );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Unable to retrieve address hr=0x%x", hr );
		goto GETSOCKADDRESS_ERROR;
	}	

	szAddress = new CHAR[dwAddressSize];

	if( szAddress == NULL )
	{
		DPF( DP8A_ERRORLEVEL, "Error allocating memory hr=0x%x", hr );
		hr = DPNERR_OUTOFMEMORY;
		goto GETSOCKADDRESS_ERROR;	
	}

	if( FAILED( hr = STR_jkWideToAnsi( szAddress, swzAddress, dwAddressSize ) ) )
	{
		DPF( DP8A_ERRORLEVEL, "Error converting address to ANSI hr=0x%x", hr );
		hr = DPNERR_CONVERSION;
		goto GETSOCKADDRESS_ERROR;
	}

	iaTmp.s_addr = inet_addr( szAddress );

    if( iaTmp.s_addr != INADDR_NONE || strcmp( szAddress, "255.255.255.255" ) == 0 )
    {
        dwRequiredSize = sizeof( SOCKADDR );

	    if( *pdwBufferSize < dwRequiredSize )
	    {
		    *pdwBufferSize = dwRequiredSize;
		    DPF( DP8A_WARNINGLEVEL, "Buffer too small" );
		    hr = DPNERR_BUFFERTOOSMALL;
		    goto GETSOCKADDRESS_ERROR;
	    }

        memset( pSockAddr, 0x00, sizeof( SOCKADDR ) );

        psinCurAddress = (sockaddr_in *) pSockAddr;

   		psinCurAddress->sin_family = AF_INET;
		psinCurAddress->sin_port = htons(usPort);
		psinCurAddress->sin_addr = iaTmp;

		hr = DPN_OK;

		goto GETSOCKADDRESS_ERROR;
    }

	lpHostEntry = gethostbyname( szAddress );	

	if( lpHostEntry == NULL )
	{
		DPF( DP8A_ERRORLEVEL, "Invalid host specified hr=0x%x" , hr );
		hr = DPNERR_INVALIDHOSTADDRESS;
		goto GETSOCKADDRESS_ERROR;
	}

	// Count addresses
	for( dwNumElements = 0; ; dwNumElements++ )
	{
		piaTmp = ((LPIN_ADDR)lpHostEntry->h_addr_list[dwNumElements]);

		if( piaTmp == NULL )
			break;
	}

	dwRequiredSize = dwNumElements * sizeof( SOCKADDR );

	if( *pdwBufferSize < dwRequiredSize )
	{
		*pdwBufferSize = dwRequiredSize;
		DPF( DP8A_WARNINGLEVEL, "Buffer too small" );
		hr = DPNERR_BUFFERTOOSMALL;
		goto GETSOCKADDRESS_ERROR;
	}

	*pdwBufferSize = dwRequiredSize;

	pCurLoc = pSockAddr;

	memset( pCurLoc, 0x00, *pdwBufferSize );

	// Build addresses and copy them to the buffer
	for( dwIndex = 0; dwIndex < dwNumElements; dwIndex++ )
	{
		psinCurAddress = (sockaddr_in *) pCurLoc;
		psinCurAddress->sin_family = AF_INET;
		psinCurAddress->sin_port = htons(usPort);
		psinCurAddress->sin_addr = *((LPIN_ADDR)lpHostEntry->h_addr_list[dwIndex]);
		
		pCurLoc++;
	}

	delete [] swzAddress;
	delete [] szAddress;

	DP8A_RETURN( DPN_OK );

GETSOCKADDRESS_ERROR:

	if( swzAddress != NULL )
		delete [] swzAddress;

	if( szAddress != NULL )
		delete [] szAddress;

	DP8A_RETURN( hr );
	
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8ATCP_GetLocalAddress"
HRESULT DP8ADDRESSOBJECT::DP8ATCP_GetLocalAddress( IDirectPlay8Address *pInterface, GUID * pguidAdapter, PUSHORT pusPort )
{
	RIP_ON_NOT_TRUE( "GetLocalAddress() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	
	HRESULT hr;

	DPF( DP8A_ENTERLEVEL, "Enter" );

	GUID guidDevice;
	DWORD dwPort;
	DWORD dwType;
	DWORD dwSize;	
	GUID guidSP;

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pguidAdapter = 0x%p pusPort = 0x%p",
	     pguidAdapter, pusPort );

	RIP_ON_NOT_TRUE( "GetLocalAddress() Invalid pointer for adapter", !( pguidAdapter == NULL || !DNVALID_WRITEPTR( pguidAdapter, sizeof( GUID ) ) ) );
	RIP_ON_NOT_TRUE( "GetLocalAddress() Invalid pointer for port", !( pusPort == NULL || !DNVALID_WRITEPTR( pusPort, sizeof( USHORT ) ) ) );

	hr = pdp8Address->GetElementType( DPNA_KEY_DEVICE, &dwType );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "This address does not have a device element hr=0x%x", hr );
		hr = DPNERR_INCOMPLETEADDRESS;
		DP8A_RETURN( hr );
	}

	if( dwType != DPNA_DATATYPE_GUID )
	{
		DPF( DP8A_ERRORLEVEL, "Invalid Address: The device is not a GUID hr=0x%x", hr );
		hr = DPNERR_GENERIC;		
		DP8A_RETURN( hr );
	}

	hr = pdp8Address->GetElementType( DPNA_KEY_PORT, &dwType );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "This address does not have a port element hr=0x%x", hr );
		hr = DPNERR_INCOMPLETEADDRESS;
		DP8A_RETURN( hr );
	}

	if( dwType != DPNA_DATATYPE_DWORD )
	{
		DPF( DP8A_ERRORLEVEL, "Invalid Address: The port is not a dword hr=0x%x", hr );
		hr = DPNERR_GENERIC;		
		DP8A_RETURN( hr );
	}

	dwSize = sizeof(DWORD);

	hr = pdp8Address->GetElement( DPNA_KEY_PORT, &dwPort, &dwSize, &dwType );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Unable to retrieve port element hr=0x%x", hr );
		hr = DPNERR_GENERIC;
		DP8A_RETURN( hr );
	}

	dwSize = sizeof(GUID);

	hr = pdp8Address->GetElement( DPNA_KEY_DEVICE, &guidDevice, &dwSize, &dwType );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Unable to retrieve device element hr=0x%x", hr );
		hr = DPNERR_GENERIC;		
		DP8A_RETURN( hr );
	}	

	*pguidAdapter = guidDevice;
	*pusPort = (USHORT) dwPort;

	DP8A_RETURN( DPN_OK );	
}

#undef DPF_MODNAME
#define DPF_MODNAME "DP8ATCP_GetAddressW"
HRESULT DP8ADDRESSOBJECT::DP8ATCP_GetAddressW( IDirectPlay8Address *pInterface, WCHAR * pwszAddress, PDWORD pdwAddressLength, PUSHORT pusPort )
{
	RIP_ON_NOT_TRUE( "GetAddressW() Invalid object", !( pInterface == NULL || !DP8A_VALID( pInterface ) ) );

	DP8ADDRESSOBJECT *pdp8Address = this;
	
	HRESULT hr;
	DWORD dwPort;
	DWORD dwType;
	DWORD dwSize;
	GUID guidSP;

	DPF( DP8A_ENTERLEVEL, "Enter" );

	RIP_ON_NOT_TRUE( "GetAddressW() Invalid pointer for pdwAddressLength", !( pdwAddressLength == NULL || !DNVALID_WRITEPTR( pdwAddressLength, sizeof( DWORD ) ) ) );
	RIP_ON_NOT_TRUE( "GetAddressW() Invalid pointer for pwszAddress", !( *pdwAddressLength > 0 && (pwszAddress == NULL || !DNVALID_WRITEPTR( pwszAddress, (*pdwAddressLength)*sizeof(WCHAR) ) ) ) );
	RIP_ON_NOT_TRUE( "GetAddressW() Invalid pointer for port", !( pusPort == NULL || !DNVALID_WRITEPTR( pusPort, sizeof( USHORT ) ) ) );

	// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPF( DP8A_PARAMLEVEL, "pwszAddress = 0x%p pdwAddressLength = 0x%p (%u) pusPort = 0x%p (%u)",
	     pwszAddress, pdwAddressLength, *pdwAddressLength, pusPort, (DWORD)*pusPort );

	hr = pdp8Address->GetElementType( DPNA_KEY_HOSTNAME, &dwType );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "This address does not have a hostname element hr=0x%x", hr );
		hr = DPNERR_INCOMPLETEADDRESS;				
		DP8A_RETURN( hr );
	}

	if( dwType != DPNA_DATATYPE_STRING )
	{
		DPF( DP8A_ERRORLEVEL, "Invalid Address: The host name is not a string hr=0x%x", hr );
		hr = DPNERR_GENERIC;
		DP8A_RETURN( hr );
	}

	hr = pdp8Address->GetElementType( DPNA_KEY_PORT, &dwType );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "This address does not have a port element hr=0x%x", hr );
		hr = DPNERR_INCOMPLETEADDRESS;
		DP8A_RETURN( hr );
	}

	if( dwType != DPNA_DATATYPE_DWORD )
	{
		DPF( DP8A_ERRORLEVEL, "Invalid Address: The port is not a dword hr=0x%x", hr );
		hr = DPNERR_GENERIC;		
		DP8A_RETURN( hr );
	}

	dwSize = sizeof(DWORD);

	hr = pdp8Address->GetElement( DPNA_KEY_PORT, &dwPort, &dwSize, &dwType );

	if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Unable to retrieve port element hr=0x%x", hr );
		hr = DPNERR_GENERIC;
		DP8A_RETURN( hr );
	}

	*pdwAddressLength *= 2;

	hr = pdp8Address->GetElement( DPNA_KEY_HOSTNAME, pwszAddress, pdwAddressLength, &dwType );

	*pdwAddressLength /= 2;

	if( hr == DPNERR_BUFFERTOOSMALL )
	{
		DPF( DP8A_WARNINGLEVEL, "Buffer too small hr=0x%x", hr );
		DP8A_RETURN( hr );
	}
	else if( FAILED( hr ) )
	{
		DPF( DP8A_ERRORLEVEL, "Unable to retrieve hostname element hr=0x%x", hr );
 		hr = DPNERR_GENERIC;
		DP8A_RETURN( hr );
	}	

	*pusPort = (USHORT) dwPort;
	
	DP8A_RETURN( DPN_OK );		
}


