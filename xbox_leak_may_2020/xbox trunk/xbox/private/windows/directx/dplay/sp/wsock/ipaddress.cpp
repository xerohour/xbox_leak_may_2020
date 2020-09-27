/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       IPAddress.cpp
 *  Content:	Winsock IP address class
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

//
// length of IP address string including NULL  "xxx.xxx.xxx.xxx\0"
//
#define	IP_ADDRESS_STRING_LENGTH		16

//
// multicast address to use
//
#pragma	BUGBUG( johnkan, "Hard-coded group ID!" )
#define	IP_MULTICAST_ADDRESS	0xE2E2E2E2

//
// default size of buffers when parsing
//
#define	DEFAULT_COMPONENT_BUFFER_SIZE	1000

//
// default broadcast and listen addresses
//
const WCHAR	g_IPBroadcastAddress[] = L"255.255.255.255";
const DWORD	g_dwIPBroadcastAddressSize = sizeof( g_IPBroadcastAddress );
static const WCHAR	g_IPListenAddress[] = L"0.0.0.0";

//
// string for IP helper API
//
static const char	g_MultipleIPTemplate[] = "%s - %s";

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
// Function definitions
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::InitializeWithBroadcastAddress - initialize with the IP broadcast address
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
void	CIPAddress::InitializeWithBroadcastAddress( void )
{
	m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr = p_htonl( INADDR_BROADCAST );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::InitializeWithAnyAddress - initialize with the IP Any address
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
void	CIPAddress::InitializeWithAnyAddress( void )
{
	m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr = p_htonl( INADDR_ANY );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::SetAddressFromSOCKADDR - set address from a socket address
//
// Entry:		Reference to address
//				Size of address
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::SetAddressFromSOCKADDR"

void	CIPAddress::SetAddressFromSOCKADDR( const SOCKADDR &Address, const INT_PTR iAddressSize )
{
	DNASSERT( iAddressSize == GetAddressSize() );
	memcpy( &m_SocketAddress.SocketAddress, &Address, iAddressSize );

	//
	// Since Winsock won't guarantee that the sin_zero part of an IP address is
	// really zero, we ned to do it ourself.  If we don't, it'll make a mess out
	// of the Guid<-->Address code.
	//
	DBG_CASSERT( sizeof( &m_SocketAddress.IPSocketAddress.sin_zero[ 0 ] ) == sizeof( DWORD* ) );
	DBG_CASSERT( sizeof( &m_SocketAddress.IPSocketAddress.sin_zero[ sizeof( DWORD ) ] ) == sizeof( DWORD* ) );
	*reinterpret_cast<DWORD*>( &m_SocketAddress.IPSocketAddress.sin_zero[ 0 ] ) = 0;
	*reinterpret_cast<DWORD*>( &m_SocketAddress.IPSocketAddress.sin_zero[ sizeof( DWORD ) ] ) = 0;
	DNASSERT( SinZeroIsZero( &m_SocketAddress.IPSocketAddress ) != FALSE );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::SocketAddressFromDP8Address - convert a DP8Address to a socket address
//
// Entry:		Pointer to address
//				Address type
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::SocketAddressFromDP8Address"

HRESULT	CIPAddress::SocketAddressFromDP8Address( IDirectPlay8Address *const pDP8Address,
												 const SP_ADDRESS_TYPE AddressType )
{
	HRESULT		hr;
	DWORD		dwAddressSize;
	IDirectPlay8Address		*pDuplicateAddress;
	SOCKADDR	*pSocketAddresses;


	DNASSERT( pDP8Address != NULL );

	//
	// initialize
	//
	hr = DPN_OK;
	pDuplicateAddress = NULL;
	pSocketAddresses = NULL;

	//
	// reset internal flags
	//
	Reset();

	switch ( AddressType )
	{
		//
		// local device address, ask for the device guid and port to build a socket
		// address
		//
		case SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT:
		case SP_ADDRESS_TYPE_DEVICE_PROXIED_ENUM_TARGET:
		{
			HRESULT	hTempResult;
			DWORD	dwTempSize;
			GUID	AdapterGuid;
			DWORD	dwPort;
			DWORD	dwDataType;
			union
			{
				SOCKADDR	SocketAddress;
				SOCKADDR_IN	INetAddress;
			} INetSocketAddress;


			//
			// Ask for the port.  If none is found, choose a default.
			//
			dwTempSize = sizeof( dwPort );
			hTempResult = IDirectPlay8Address_GetComponentByName( pDP8Address, DPNA_KEY_PORT, &dwPort, &dwTempSize, &dwDataType );
			switch ( hTempResult )
			{
				//
				// port present, nothing to do
				//
				case DPN_OK:
				{
					DNASSERT( dwDataType == DPNA_DATATYPE_DWORD );
					break;
				}

				//
				// port not present, fill in the appropriate default
				//
				case DPNERR_DOESNOTEXIST:
				{
					DNASSERT( hr == DPN_OK );
					switch ( AddressType )
					{
						case SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT:
						{
							dwPort = ANY_PORT;
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
				// other error, fail
				//
				default:
				{
					hr = hTempResult;
					goto Failure;
					break;
				}
			}
			DNASSERT( sizeof( dwPort ) == dwTempSize );

// BUGBUG: [mgere] [xbox] New stuff added here.

		char	CharBuffer[1000];
        	HOSTENT			*pHostData;

            if ( p_gethostname( CharBuffer, sizeof( CharBuffer ) ) == SOCKET_ERROR )
	        {
		        DWORD	dwWinsockError;

		        hr = DPNERR_GENERIC;
		        dwWinsockError = p_WSAGetLastError();
		        DPF( 0, "Failed to get host name into static buffer!" );
		        DisplayWinsockError( 0, dwWinsockError );
		        DNASSERT( FALSE );
		        goto Failure;
	        }

	        pHostData = p_gethostbyname( CharBuffer );
	        if ( pHostData == NULL )
	        {
		        DWORD	dwWinsockError;

		        hr = DPNERR_GENERIC;
		        dwWinsockError = p_WSAGetLastError();
		        DPF( 0, "Failed to get host data!" );
		        DisplayWinsockError( 0, dwWinsockError );
		        DNASSERT( FALSE );
		        goto Failure;
	        }

	IN_ADDR			TempAddresses;

	memcpy( &TempAddresses, pHostData->h_addr_list[0], sizeof(TempAddresses) );

// BUGBUG: [mgere] [xbox] End new stuff
            
/* BUGBUG: [mgere] [xbox] Removed temporarily
			//
			// if this is a proxied enum, check for the all-adapters address
			// being passed because it's not a valid target (it needs to be
			// remapped to the local loopback).
			//
			if ( ( AddressType == SP_ADDRESS_TYPE_DEVICE_PROXIED_ENUM_TARGET ) &&
				 ( INetSocketAddress.INetAddress.sin_addr.S_un.S_addr == p_htonl( INADDR_ANY ) ) )
			{
				m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr = p_htonl( INADDR_LOOPBACK );
			}
			else
			{
				m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr = INetSocketAddress.INetAddress.sin_addr.S_un.S_addr;
			}
*/

//          m_SocketAddress.SockAddr.sa_family = GetFamily();
	        m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr = TempAddresses.S_un.S_addr;
//	        m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr = NULL;

            m_SocketAddress.IPSocketAddress.sin_port = p_htons( static_cast<WORD>( dwPort ) );
			break;
		}

		//
		// hostname
		//
		case SP_ADDRESS_TYPE_HOST:
		{
			HRESULT	hTempResult;
			DWORD	dwPort;
			DWORD	dwTempSize;
			DWORD	dwDataType;



			//
			// duplicate the input address because it might need to be modified
			//
			DNASSERT( pDuplicateAddress == NULL );
			IDirectPlay8Address_Duplicate( pDP8Address, &pDuplicateAddress );
			if ( pDuplicateAddress == NULL )
			{
				hr = DPNERR_OUTOFMEMORY;
				goto Failure;
			}

			//
			// Ask for the port.  If none is found, choose a default.
			//
			dwTempSize = sizeof( dwPort );
			hTempResult = IDirectPlay8Address_GetComponentByName( pDP8Address, DPNA_KEY_PORT, &dwPort, &dwTempSize, &dwDataType );
			switch ( hTempResult )
			{
				//
				// port present, nothing to do
				//
				case DPN_OK:
				{
					DNASSERT( dwDataType == DPNA_DATATYPE_DWORD );
					m_SocketAddress.IPSocketAddress.sin_port = p_htons( static_cast<WORD>( dwPort ) );
					break;
				}

				//
				// port not present, fill in the appropriate default
				//
				case DPNERR_DOESNOTEXIST:
				{
					const DWORD	dwTempPort = DPNA_DPNSVR_PORT;


					m_SocketAddress.IPSocketAddress.sin_port = p_htons( static_cast<const WORD>( dwTempPort ) );
					hTempResult = IDirectPlay8Address_AddComponent( pDuplicateAddress,
																	DPNA_KEY_PORT,
																	&dwTempPort,
																	sizeof( dwTempPort ),
																	DPNA_DATATYPE_DWORD
																	);
					if ( hTempResult != DPN_OK )
					{
						hr = hTempResult;
						goto Failure;
					}

					break;
				}

				//
				// remap everything else to an addressing failure
				//
				default:
				{
					hr = DPNERR_ADDRESSING;
					goto Failure;
				}
			}

			//
			// attempt to determine the host name
			//
			dwAddressSize = 0;
			DNASSERT( pSocketAddresses == NULL );
			
RegetSocketAddressData:
			memset( pSocketAddresses, 0x00, dwAddressSize );
			hr = IDirectPlay8Address_GetSockAddress( pDuplicateAddress, pSocketAddresses, &dwAddressSize );
			switch ( hr )
			{
				//
				// conversion succeeded, check the size of the data returned
				// before setting the information in the local address structure.
				//
				case DPN_OK:
				{
					if ( dwAddressSize < sizeof( *pSocketAddresses ) )
					{
						hr = DPNERR_ADDRESSING;
						goto Failure;
					}
					
					SetAddressFromSOCKADDR( *pSocketAddresses, sizeof( *pSocketAddresses ) );					

					break;
				}

				//
				// Buffer too small, if there is no buffer, allocate one.  If
				// there is a buffer, resize it to containt the data before
				// attempting to get the data again.
				//
				case DPNERR_BUFFERTOOSMALL:
				{
					if ( pSocketAddresses == NULL )
					{
						pSocketAddresses = static_cast<SOCKADDR*>( DNMalloc( dwAddressSize ) );
						if ( pSocketAddresses == NULL )
						{
							hr = DPNERR_OUTOFMEMORY;
							goto Failure;
						}
					}
					else
					{
						void	*pTemp;


						pTemp = DNRealloc( pSocketAddresses, dwAddressSize );
						if ( pTemp == NULL )
						{
							hr = DPNERR_OUTOFMEMORY;
							goto Failure;
						}
					
						pSocketAddresses = static_cast<SOCKADDR*>( pTemp );
					}

					goto RegetSocketAddressData;
					break;
				}

				//
				// Incomplete address, set the address type and return.  It's
				// up to the caller to decide if this is really a problem.
				//
				case DPNERR_INCOMPLETEADDRESS:
				{
					break;
				}

				//
				// pass these error returns untouched
				//
				case DPNERR_OUTOFMEMORY:
				{
					goto Failure;
					break;
				}
			
				//
				// other problem, map it to an addressing error
				//
				default:
				{
					hr = DPNERR_ADDRESSING;
					goto Failure;
					break;
				}
			}

			break;
		}

		//
		// unknown address type
		//
		default:
		{
			DNASSERT( FALSE );
			break;
		}
	}

	DNASSERT( SinZeroIsZero( &m_SocketAddress.IPSocketAddress ) != FALSE );

	//
	// now that the address has been completely parsed, set the address type
	//
	m_AddressType = AddressType;

Exit:
	if ( pDuplicateAddress != NULL )
	{
		IDirectPlay8Address_Release( pDuplicateAddress );
		pDuplicateAddress = NULL;
	}

	if ( pSocketAddresses != NULL )
	{
		DNFree( pSocketAddresses );
		pSocketAddresses = NULL;
	}

	if ( hr != DPN_OK )
	{
		DPF( 0, "Problem with IPAddress::SocketAddressFromDNAddress()" );
		DisplayDNError( 0, hr );
	}

	return	hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::DP8AddressFromSocketAddress - convert a socket address to a DP8Address
//
// Entry:		Nothing
//
// Exit:		Pointer to DP8Address
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::DP8AddressFromSocketAddress"

IDirectPlay8Address *CIPAddress::DP8AddressFromSocketAddress( void ) const
{
	HRESULT		hr;
	IDirectPlay8Address		*pDP8Address;


	DNASSERT( ( m_ComponentInitializationState[ SPADDRESS_PARSE_KEY_DEVICE ] != SP_ADDRESS_COMPONENT_INITIALIZATION_FAILED ) &&
			  ( m_ComponentInitializationState[ SPADDRESS_PARSE_KEY_HOSTNAME ] != SP_ADDRESS_COMPONENT_INITIALIZATION_FAILED ) &&
			  ( m_ComponentInitializationState[ SPADDRESS_PARSE_KEY_PORT ] != SP_ADDRESS_COMPONENT_INITIALIZATION_FAILED ) );


	//
	// intialize
	//
	hr = DPN_OK;
	pDP8Address = NULL;

	//
	// create and initialize the address
	//
	hr = DirectPlay8AddressCreate( IID_IDirectPlay8Address, (void **) &pDP8Address, NULL );
	if ( hr != S_OK )
	{
		DPF( 0, "DP8AddressFromSocketAddress: Failed to create IPAddress when converting socket address do DP8Address" );
		DNASSERT( FALSE );
		goto Failure;
	}

	switch ( m_AddressType )
	{
		case SP_ADDRESS_TYPE_DEVICE_USE_ANY_PORT:
		{
			GUID		DeviceGuid;


			GuidFromInternalAddressWithoutPort( DeviceGuid );
			hr = IDirectPlay8Address_BuildLocalAddress( pDP8Address,
														  &DeviceGuid,
														  p_ntohs( m_SocketAddress.IPSocketAddress.sin_port )
														  );
			break;
		}

		case SP_ADDRESS_TYPE_PUBLIC_HOST_ADDRESS:
		case SP_ADDRESS_TYPE_READ_HOST:
		case SP_ADDRESS_TYPE_HOST:
		{
			hr = IDirectPlay8Address_BuildFromSockAddr( pDP8Address, &m_SocketAddress.SocketAddress );
			break;
		}

		default:
		{
			INT3;
			break;
		}
	}

Exit:

	return	pDP8Address;

Failure:

	if ( pDP8Address != NULL )
	{
		IDirectPlay8Address_Release( pDP8Address );
		pDP8Address = NULL;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::CompareFunction - compare against another address
//
// Entry:		Pointer to other address
//
// Exit:		Integer indicating relative magnitude:
//				0 = items equal
//				-1 = other item is of greater magnitude
//				1 = this item is of lesser magnitude
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::CompareFunction"

INT_PTR	CIPAddress::CompareFunction( const CSocketAddress *const pOtherAddress ) const
{
	INT_PTR	iReturn;
	const CIPAddress *const pIPAddress = static_cast<const CIPAddress*>( pOtherAddress );


	DNASSERT( pOtherAddress != NULL );
	DNASSERT( m_SocketAddress.IPSocketAddress.sin_family == pIPAddress->m_SocketAddress.IPSocketAddress.sin_family );

	//
	// we need to compare the IP address and port to guarantee uniqueness
	//
	if ( m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr == pIPAddress->m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr )
	{
		if ( m_SocketAddress.IPSocketAddress.sin_port == pIPAddress->m_SocketAddress.IPSocketAddress.sin_port )
		{
			iReturn = 0;
		}
		else
		{
			if ( m_SocketAddress.IPSocketAddress.sin_port < pIPAddress->m_SocketAddress.IPSocketAddress.sin_port )
			{
				iReturn = -1;
			}
			else
			{
				iReturn = 1;
			}
		}
	}
	else
	{
		if ( m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr < pIPAddress->m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr )
		{
			iReturn = -1;
		}
		else
		{
			iReturn = 1;
		}
	}

	return	iReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::CreateBroadcastAddress - create DP8Address used for broadcast sends
//
// Entry:		Nothing
//
// Exit:		Pointer to address
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::CreateBroadcastAddress"

IDirectPlay8Address *CIPAddress::CreateBroadcastAddress( void )
{
	HRESULT	hr;
	IDirectPlay8Address		*pDPlayAddress;


	//
	// initialize
	//
	pDPlayAddress = NULL;

	hr = DirectPlay8AddressCreate( IID_IDirectPlay8Address, (void **) &pDPlayAddress, NULL );
	if ( hr != S_OK )
	{
		DNASSERT( pDPlayAddress == NULL );
		DPF( 0, "CreateBroadcastAddress: Failed to create IPAddress when converting socket address do DP8Address" );
		DNASSERT( FALSE );
		goto Failure;
	}

	hr = IDirectPlay8Address_BuildAddress( pDPlayAddress, g_IPBroadcastAddress, DPNA_DPNSVR_PORT );
	if ( hr != DPN_OK )
	{
		DPF( 0, "CreateBroadcastAddress: Failed to set hostname and port!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

Exit:
	return	pDPlayAddress;

Failure:
	if ( pDPlayAddress != NULL )
	{
		IDirectPlay8Address_Release( pDPlayAddress );
		pDPlayAddress = NULL;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::CreateListenAddress - create DP8Address used for listens
//
// Entry:		Nothing
//
// Exit:		Pointer to address
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::CreateListenAddress"

IDirectPlay8Address *CIPAddress::CreateListenAddress( void )
{
	HRESULT	hr;
	IDirectPlay8Address		*pDPlayAddress;


	//
	// initialize
	//
	pDPlayAddress = NULL;

	hr = DirectPlay8AddressCreate( IID_IDirectPlay8Address, (void **) &pDPlayAddress, NULL );
	if ( hr != S_OK )
	{
		DNASSERT( pDPlayAddress == NULL );
		DPF( 0, "CreateListenAddress: Failed to create IPAddress when converting socket address do DP8Address" );
		DNASSERT( FALSE );
		goto Failure;
	}

	hr = IDirectPlay8Address_BuildAddress( pDPlayAddress, g_IPListenAddress, ANY_PORT );
	if ( hr != DPN_OK )
	{
		DPF( 0, "CreateListenAddress: Failed to set hostname and port!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

Exit:
	return	pDPlayAddress;

Failure:
	if ( pDPlayAddress != NULL )
	{
		IDirectPlay8Address_Release( pDPlayAddress );
		pDPlayAddress = NULL;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::CreateGenericAddress - create a generic address
//
// Entry:		Nothing
//
// Exit:		Pointer to address
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::CreateGenericAddress"

IDirectPlay8Address *CIPAddress::CreateGenericAddress( void )
{
	HRESULT	hr;
	IDirectPlay8Address		*pDPlayAddress;


	//
	// initialize
	//
	pDPlayAddress = NULL;

	hr = DirectPlay8AddressCreate( IID_IDirectPlay8Address, (void **) &pDPlayAddress, NULL );
	if ( hr != S_OK )
	{
		DNASSERT( pDPlayAddress == NULL );
		DPF( 0, "CreateGenericAddress: Failed to create IPAddress when converting socket address do DP8Address" );
		DNASSERT( FALSE );
		goto Failure;
	}

	hr = IDirectPlay8Address_BuildAddress( pDPlayAddress, g_IPListenAddress, ANY_PORT );
	if ( hr != DPN_OK )
	{
		DPF( 0, "CreateGenericAddress: Failed to set hostname and port!" );
		DisplayDNError( 0, hr );
		goto Failure;
	}

Exit:
	return	pDPlayAddress;

Failure:
	if ( pDPlayAddress != NULL )
	{
		IDirectPlay8Address_Release( pDPlayAddress );
		pDPlayAddress = NULL;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::CompareToBaseAddress - compare this address to a 'base' address
//		of this class
//
// Entry:		Pointer to base address
//
// Exit:		Integer indicating relative magnitude:
//				0 = items equal
//				-1 = other item is of greater magnitude
//				1 = this item is of lesser magnitude
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::CompareToBaseAddress"

INT_PTR	CIPAddress::CompareToBaseAddress( const SOCKADDR *const pBaseAddress ) const
{
	const SOCKADDR_IN	*pBaseIPAddress;
	DNASSERT( pBaseAddress != NULL );

	
	DNASSERT( pBaseAddress->sa_family == m_SocketAddress.SocketAddress.sa_family );
	pBaseIPAddress = reinterpret_cast<const SOCKADDR_IN*>( pBaseAddress );
	if ( m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr == pBaseIPAddress->sin_addr.S_un.S_addr )
	{
		return 0;
	}
	else
	{
		if ( m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr < pBaseIPAddress->sin_addr.S_un.S_addr )
		{
			return	1;
		}
		else
		{
			return	-1;
		}
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::HashFunction - hash address to N bits
//
// Entry:		Count of bits to hash to
//
// Exit:		Hashed value
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::HashFunction"

INT_PTR	CIPAddress::HashFunction( const INT_PTR iHashBitCount ) const
{
	INT_PTR		iReturn;
	UINT_PTR	Temp;


	DNASSERT( iHashBitCount != 0 );

	//
	// initialize
	//
	iReturn = 0;

	//
	// hash IP address
	//
	Temp = m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr;
	do
	{
		iReturn ^= Temp & ( ( 1 << iHashBitCount ) - 1 );
		Temp >>= iHashBitCount;
	} while ( Temp != 0 );

	//
	// hash IP port
	//
	Temp = m_SocketAddress.IPSocketAddress.sin_port;
	do
	{
		iReturn^= Temp & ( ( 1 << iHashBitCount ) - 1 );
		Temp >>= iHashBitCount;
	}
	while ( Temp != 0 );

	return iReturn;
}
//**********************************************************************




//**********************************************************************
// ------------------------------
// CIPAddress::CreateMulticastGroup - create a multicast group on IP.
//
// Entry:		Reference to adapter GUID
//				Pointer to socket address of multicast group (may be NULL)
//				Pointer to group data to fill
//				Pointer to size of group data
//
// Exit:		Error code
//
// Note:	Since IP multicast groups exist at all times (you need to join them).
//			This code will choose a multicast address for future use and return
//			the data.
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::CreateMulticastGroup"

HRESULT	CIPAddress::CreateMulticastGroup( const GUID &AdapterGuid,
										  const CSocketAddress *pMulticastGroupAddress,
										  void *const pGroupData,
										  DWORD *const pdwGroupDataSize
										  ) const
{
	HRESULT			hr;
	MULTICAST_DATA	*pMulticastData;
	union
	{
		SOCKADDR	SocketAddress;
		SOCKADDR_IN	INetAddress;
	} MulticastAddress;


	DNASSERT( pdwGroupDataSize != NULL );
	DNASSERT( ( pGroupData != NULL ) || ( *pdwGroupDataSize == 0 ) );

	//
	// initialize
	//
	hr = DPN_OK;
	pMulticastData = NULL;

	//
	// check for sufficient output space
	//
	if ( *pdwGroupDataSize < sizeof( *pMulticastData ) )
	{
		hr = DPNERR_BUFFERTOOSMALL;
		*pdwGroupDataSize = sizeof( *pMulticastData );
		DPF( 8, "Insufficient buffer space for creating multicast group data!" );
		goto Failure;
	}

	//
	// If we have a valid multicast group address, use it.  Otherwise, choose a
	// default.
	//
	pMulticastData = static_cast<MULTICAST_DATA*>( pGroupData );
	memset( &MulticastAddress, 0x00, sizeof( MulticastAddress ) );
	MulticastAddress.SocketAddress.sa_family = GetFamily();
	if ( pMulticastGroupAddress != NULL )
	{
		const	SOCKADDR_IN	*pINetSocketAddress;


		INT3;
		DBG_CASSERT( sizeof( SOCKADDR ) == sizeof( SOCKADDR_IN ) );
		pINetSocketAddress = reinterpret_cast<const SOCKADDR_IN*>( pMulticastGroupAddress->GetAddress() );
		MulticastAddress.INetAddress.sin_addr = pINetSocketAddress->sin_addr;
		MulticastAddress.INetAddress.sin_port = pINetSocketAddress->sin_port;
	}
	else
	{
#pragma	BUGBUG( johnkan, "Hard-coded multicast address!" )
		MulticastAddress.INetAddress.sin_addr.S_un.S_addr = p_htonl( IP_MULTICAST_ADDRESS );
	}

	//
	// If a port isn't specified, use the default DNet port.
	//
	if ( MulticastAddress.INetAddress.sin_port == 0 )
	{
		MulticastAddress.INetAddress.sin_port = p_htons( DPNA_DPNSVR_PORT );
	}

	GuidFromAddress( pMulticastData->MulticastAddressGuid, MulticastAddress.SocketAddress );
	*pdwGroupDataSize = sizeof( *pMulticastData );

Exit:
	return	hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::DeleteMulticastGroup - delete a multicast group in IP.
//
// Entry:		Reference to GUID of adapter to use to delete group
//				Pointer to group data
//				Size of group data
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::DeleteMulticastGroup"

HRESULT	CIPAddress::DeleteMulticastGroup( const GUID &AdapterGuid,
										  const void *const pGroupData,
										  const DWORD dwGroupDataSize ) const
{
	HRESULT	hr;


	DNASSERT( pGroupData != NULL );
	DNASSERT( dwGroupDataSize == sizeof( MULTICAST_DATA ) );

	//
	// initialize
	//
	hr = DPN_OK;

	//
	// Since IP multicast groups exist at all times (you cannot delete them), there
	// is nothing to do for this function.
	//

	return	hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::AddToMulticastGroup - join a multicast group
//
// Entry:		Pointer to group data
//				Size of group data
//				Socket to use
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::AddToMulticastGroup"

HRESULT	CIPAddress::AddToMulticastGroup( const void *const pGroupData,
										 const DWORD dwGroupDataSize,
										 const SOCKET &Socket
										 ) const
{
	HRESULT	hr;
	ip_mreq	MulticastRequest;
	int		SocketOption;
	const MULTICAST_DATA	*pMulticastData;
	union
	{
		SOCKADDR	SocketAddress;
		SOCKADDR_IN	INetAddress;
	} MulticastSocketAddress;
	UINT_PTR	WSAReturn;


	DNASSERT( pGroupData != NULL );
	DNASSERT( dwGroupDataSize == sizeof( *pMulticastData ) );

	//
	// initialize
	//
	hr = DPN_OK;
	memset( &MulticastRequest, 0x00, sizeof( MulticastRequest ) );

	//
	// convert multicast address GUID to a pure address and build our multiacast
	// join request
	//
	pMulticastData = static_cast<const MULTICAST_DATA*>( pGroupData );
	AddressFromGuid( pMulticastData->MulticastAddressGuid, MulticastSocketAddress.SocketAddress );
	DPF( 8, "Socket address for multicast destination" );
	DumpSocketAddress( 8, &MulticastSocketAddress.SocketAddress, GetFamily() );
	MulticastRequest.imr_multiaddr = MulticastSocketAddress.INetAddress.sin_addr;
	MulticastRequest.imr_interface = m_SocketAddress.IPSocketAddress.sin_addr;

		//
		// Winsock2, or greater, use the IP_ADD_MEMBERSHIP value for Winsock2
		// see WS2TCPIP.H
		//
			SocketOption = 12;

	DBG_CASSERT( sizeof( &MulticastRequest ) == sizeof( char* ) );
	WSAReturn = p_setsockopt( Socket,											// socket
							  IPPROTO_IP,										// level (TCP/IP)
							  SocketOption,										// option (join multicast group)
							  reinterpret_cast<char*>( &MulticastRequest ),		// option data
							  sizeof( MulticastRequest )						// size of option data
							  );
	if ( WSAReturn == SOCKET_ERROR )
	{
		hr = DPNERR_GENERIC;
		DPF( 0, "Failed to join IP multicast group!" );
		DisplayWinsockError( 0, p_WSAGetLastError() );
		goto Failure;
	}

Exit:
	return	hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::RemoveFromMulticastGroup - leave a multicast group
//
// Entry:		Pointer to group data
//				Size of group data
//				Socket to use
//
// Exit:		Error code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::RemoveFromMulticastGroup"

HRESULT	CIPAddress::RemoveFromMulticastGroup( const void *const pGroupData,
											  const DWORD dwGroupDataSize,
											  const SOCKET &Socket
											  ) const
{
	HRESULT	hr;
	ip_mreq	MulticastRequest;
	int		SocketOption;
	const MULTICAST_DATA	*pMulticastData;
	union
	{
		SOCKADDR	SocketAddress;
		SOCKADDR_IN	INetAddress;
	} MulticastSocketAddress;
	UINT_PTR	WSAReturn;


	DNASSERT( pGroupData != NULL );
	DNASSERT( dwGroupDataSize == sizeof( *pMulticastData ) );

	//
	// initialize
	//
	hr = DPN_OK;
	memset( &MulticastRequest, 0x00, sizeof( MulticastRequest ) );

	//
	// convert multicast address GUID to a pure address and build our multiacast
	// join request
	//
	pMulticastData = static_cast<const MULTICAST_DATA*>( pGroupData );
	AddressFromGuid( pMulticastData->MulticastAddressGuid, MulticastSocketAddress.SocketAddress );
	MulticastRequest.imr_multiaddr = MulticastSocketAddress.INetAddress.sin_addr;
	MulticastRequest.imr_interface = m_SocketAddress.IPSocketAddress.sin_addr;

		//
		// Winsock2, or greater, use the IP_DROP_MEMBERSHIP value for Winsock2
		// see WS2TCPIP.H
		//
			SocketOption = 13;

	DBG_CASSERT( sizeof( &MulticastRequest ) == sizeof ( char* ) );
	WSAReturn = p_setsockopt( Socket,											// socket to work with
							  IPPROTO_IP,										// level (TCPIP)
							  SocketOption,										// option (remove from multicast group)
							  reinterpret_cast<char*>( &MulticastRequest ),		// option data
							  sizeof( MulticastRequest )						// size of option data
							  );
	if ( WSAReturn == SOCKET_ERROR )
	{
		hr = DPNERR_GENERIC;
		DPF( 0, "Failed to leave IP multicast group!" );
		DisplayWinsockError( 0, p_WSAGetLastError() );
		goto Failure;
	}

Exit:
	return	hr;

Failure:
	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::GuidFromInternalAddressWithoutPort - get a guid from the internal
//		address without a port.
//
// Entry:		Reference to desintation GUID
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::GuidFromInternalAddressWithoutPort"

void	CIPAddress::GuidFromInternalAddressWithoutPort( GUID &OutputGuid ) const
{
	union
	{
		SOCKADDR	SockAddr;
		SOCKADDR_IN	IPSockAddr;
	} TempSocketAddress;


	DBG_CASSERT( sizeof( TempSocketAddress.SockAddr ) == sizeof( m_SocketAddress.SocketAddress ) );
	memcpy( &TempSocketAddress.SockAddr, &m_SocketAddress.SocketAddress, sizeof( TempSocketAddress.SockAddr ) );
	TempSocketAddress.IPSockAddr.sin_port = 0;
	GuidFromAddress( OutputGuid, TempSocketAddress.SockAddr );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::ImportMulticastAdapterAddress - set this address to the adapter
//		address used to manipulate a multicast group address
//
// Entry:		Reference to base adapter address
//				Pointer to group data
//				Size of group data
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::ImportMulticastAdapterAddress"

void	CIPAddress::ImportMulticastAdapterAddress( const GUID &AdapterGuid, const void *const pGroupData, const UINT_PTR GroupDataSize )
{
	const	MULTICAST_DATA	*pMulticastData;
	union
	{
		SOCKADDR	SocketAddress;
		SOCKADDR_IN	IPSocketAddress;
	} TempAddress;


	//
	// combine the given adapter address with the port specified in the
	// multicast address
	//
	DNASSERT( pGroupData != NULL );
	DNASSERT( GroupDataSize == sizeof( *pMulticastData ) );
	pMulticastData = static_cast<const MULTICAST_DATA*>( pGroupData );
	AddressFromGuid( AdapterGuid, *GetWritableAddress() );
	memset( &TempAddress, 0x00, sizeof( TempAddress ) );
	AddressFromGuid( pMulticastData->MulticastAddressGuid, TempAddress.SocketAddress );
	m_SocketAddress.IPSocketAddress.sin_port = TempAddress.IPSocketAddress.sin_port;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::IsUndefinedHostAddress - determine if this is an undefined host
//		address
//
// Entry:		Nothing
//
// Exit:		Boolean indicating whether this is an undefined host address
//				TRUE = this is an undefined address
//				FALSE = this is not an undefined address
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::IsUndefinedHostAddress"

BOOL	CIPAddress::IsUndefinedHostAddress( void ) const
{
	BOOL	fReturn;


	fReturn = FALSE;
	if ( m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr == p_htonl( INADDR_ANY ) )
	{
		fReturn = TRUE;
	}

	return	fReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::ChangeLoopBackToLocalAddress - change loopback to a local address
//
// Entry:		Pointer to other address
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::ChangeLoopBackToLocalAddress"

void	CIPAddress::ChangeLoopBackToLocalAddress( const CSocketAddress *const pOtherSocketAddress )
{
	const CIPAddress	*pOtherIPAddress;

	
	DNASSERT( pOtherSocketAddress != NULL );
	pOtherIPAddress = static_cast<const CIPAddress*>( pOtherSocketAddress );

	if ( m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr == p_htonl( INADDR_LOOPBACK ) )
	{
		m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr = pOtherIPAddress->m_SocketAddress.IPSocketAddress.sin_addr.S_un.S_addr;
	}
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CIPAddress::CopyInternalSocketAddressWithoutPort - copy socket address
//		without the port field.
//
// Entry:		Reference to destination address
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "CIPAddress::CopyInternalSocketAddressWithoutPort"

void	CIPAddress::CopyInternalSocketAddressWithoutPort( SOCKADDR &AddressDestination ) const
{
	SOCKADDR_IN	*pIPSocketAddress;


	DNASSERT( SinZeroIsZero( &m_SocketAddress.IPSocketAddress ) != FALSE );

	//
	// copy address and zero out the port
	//
	DBG_CASSERT( sizeof( AddressDestination ) == sizeof( m_SocketAddress.SocketAddress ) );
	memcpy( &AddressDestination, &m_SocketAddress.SocketAddress, sizeof( AddressDestination ) );

	DBG_CASSERT( sizeof( SOCKADDR_IN* ) == sizeof( &AddressDestination ) );
	pIPSocketAddress = reinterpret_cast<SOCKADDR_IN*>( &AddressDestination );
	pIPSocketAddress->sin_port = p_htons( 0 );
}
//**********************************************************************

