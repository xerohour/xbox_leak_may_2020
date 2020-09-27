// ESPAttacks.cpp : Contains the Sparta code for various ESP-UDP layer attacks
//

#include "stdafx.h"
#include "AttackConstants.h"

#pragma pack(push, 1) // one byte packing for all wire structures

typedef struct {
    BYTE        bSizeOfStruct;                  // sizeof(XNADDR)
    BYTE        bFlags;                         // XNET_XNADDR_* flags below
    BYTE        abEnet[6];                      // Ethernet MAC address of the host
    IN_ADDR     ina;                            // IP address of the host
} XNADDR;

typedef struct {
    BYTE        ab[8];                          // key identifier
} XNKID;

typedef struct CKeyExMsg
{
    DWORD           _dwSpiZero;         // Must be zero
    DWORD           _dwSpiInit;         // SPI of the initiator
    DWORD           _dwSpiResp;         // SPI of the responder
    BYTE            _abNonceInit[8];    // Nonce of the initiator
    BYTE            _abNonceResp[8];    // Nonce of the responder
    BYTE            _abDhGX[CBDHLEN];   // diffie-hellman g^X (or g^Y)
    XNKID           _xnkid;             // key identifier
    XNADDR          _xnaddr;            // XNADDR of the sender of this message
    LARGE_INTEGER   _liTime;            // increasing time-value of the sender
    BYTE            _abHash[12];        // HMAC-SHA-1-96 digest of CKeyExMsg
} XLKX_KEYEXMSG, *PXLKX_KEYEXMSG;

typedef struct CEspHead
{
	DWORD			dwSpi;
	DWORD			dwSeqNum;
} XESP_HEAD, *PXESP_HEAD;

typedef struct CEspTail
{
	BYTE			bPadLen;
	BYTE			bNextHeader;
	BYTE			abAuthData[12];
} XESP_TAIL, *PXESP_TAIL;

typedef struct CUdpHead
{
	WORD			wSourcePort;
	WORD			wDestPort;
	WORD			wLength;
	WORD			wChecksum;
} UDP_HEAD, *PUDP_HEAD;

#pragma pack(pop)

//==================================================================================
// SendEncryptedESPPacket
//----------------------------------------------------------------------------------
//
// Description: Test that sends an encrypted ESP-UDP packet with the specified parameters
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL SendEncryptedESPPacket(CInterface *pInterface, CHAR *szDestMac, DWORD dwSpi, DWORD dwSeqNum, DWORD dwEncapsulatedLen)
{
	CUDPPacket *pUDPPacket = NULL;
	CHAR szFormattedDestMac[18];
	CHAR *pBuffer = NULL;
	BOOL fSuccess = TRUE;

	if(!szDestMac || (strlen(szDestMac) < 12))
		return FALSE;

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Watch for SPARTA exceptions
	try
	{
		// Create a destination mac address from the address reported by the client
		MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
		CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);
		
		pUDPPacket = new CUDPPacket(MediaType);
		
		// Ethernet fields
		pUDPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pUDPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
		pUDPPacket->MacHeader.SetProtocolType(0x800);

		// IP fields
		pUDPPacket->IPHeader.SetVersion(4);
		pUDPPacket->IPHeader.SetTOS(0);
		pUDPPacket->IPHeader.SetID(1);
		pUDPPacket->IPHeader.SetFlag(0);
		pUDPPacket->IPHeader.SetFragOffset(0);
		pUDPPacket->IPHeader.SetTTL(64);
		pUDPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
		pUDPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
		pUDPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
		pUDPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
		pUDPPacket->IPHeader.SetAutoCalcChecksum(ON);
		pUDPPacket->IPHeader.SetAutoCalcDatagramLength(ON);
	
		// UDP fields
		pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
		pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
		pUDPPacket->UDPHeader.SetAutoCalcLength(ON);
		pUDPPacket->UDPHeader.SetAutoCalcChecksum(ON);

		if(dwEncapsulatedLen)
		{
			PXESP_HEAD pEspHeader = NULL;

			// Create the buffer for the remaining packet data
			pBuffer = (CHAR *) LocalAlloc(LPTR, dwEncapsulatedLen);
			pEspHeader = (PXESP_HEAD) pBuffer;

			// If the supplied buffer is large enough for the ESP header
			if(dwEncapsulatedLen >= sizeof(XESP_HEAD))
			{
				pEspHeader->dwSpi = dwSpi;
				pEspHeader->dwSeqNum = dwSeqNum;

				memset(pBuffer + sizeof(XESP_HEAD), 'x', dwEncapsulatedLen - sizeof(XESP_HEAD));
			}			

			// Associate that buffer with the packet
			pUDPPacket->SetUserBuffer(pBuffer, dwEncapsulatedLen, TRUE);
		}
		
		pInterface->Send(pUDPPacket);
	}
	catch(CSpartaException *Ex)
	{
		delete Ex;
		fSuccess = FALSE;
	}

	delete pUDPPacket;

	// Release the payload buffer
	pBuffer ? LocalFree(pBuffer) : 0;

	return fSuccess;
}

//==================================================================================
// SendAuthenticatedESPPacket
//----------------------------------------------------------------------------------
//
// Description: Test that sends an authenticated ESP-UDP packet with the specified parameters
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL SendAuthenticatedESPPacket(CInterface *pInterface, CHAR *szDestMac, DWORD dwSpi, DWORD dwSeqNum, BYTE bPadLen, BYTE bNextHeader, DWORD dwEncapsulatedLen)
{
	CUDPPacket *pUDPPacket = NULL;
	CHAR szFormattedDestMac[18];
	CHAR *pBuffer = NULL;
	BOOL fSuccess = TRUE;

	if(!szDestMac || (strlen(szDestMac) < 12))
		return FALSE;

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Watch for SPARTA exceptions
	try
	{
		// Create a destination mac address from the address reported by the client
		MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
		CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);
		
		pUDPPacket = new CUDPPacket(MediaType);
		
		// Ethernet fields
		pUDPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pUDPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
		pUDPPacket->MacHeader.SetProtocolType(0x800);

		// IP fields
		pUDPPacket->IPHeader.SetVersion(4);
		pUDPPacket->IPHeader.SetTOS(0);
		pUDPPacket->IPHeader.SetID(1);
		pUDPPacket->IPHeader.SetFlag(0);
		pUDPPacket->IPHeader.SetFragOffset(0);
		pUDPPacket->IPHeader.SetTTL(64);
		pUDPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
		pUDPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
		pUDPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
		pUDPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
		pUDPPacket->IPHeader.SetAutoCalcChecksum(ON);
		pUDPPacket->IPHeader.SetAutoCalcDatagramLength(ON);
	
		// UDP fields
		pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
		pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
		pUDPPacket->UDPHeader.SetAutoCalcLength(ON);
		pUDPPacket->UDPHeader.SetAutoCalcChecksum(ON);

		if(dwEncapsulatedLen)
		{
			PXESP_HEAD pEspHeader = NULL;
			PXESP_TAIL pEspTail = NULL;

			// Create the buffer for the remaining packet data
			pBuffer = (CHAR *) LocalAlloc(LPTR, dwEncapsulatedLen);
			pEspHeader = (PXESP_HEAD) pBuffer;


			if(dwEncapsulatedLen < sizeof(XESP_HEAD))
			{
				memset(pBuffer + sizeof(XESP_HEAD), 'x', dwEncapsulatedLen);
			}
			else
			{
				// If the supplied buffer is large enough for the ESP header
				pEspHeader->dwSpi = dwSpi;
				pEspHeader->dwSeqNum = dwSeqNum;

				if(dwEncapsulatedLen < (sizeof(XESP_HEAD) + sizeof(XESP_TAIL)))
				{
					memset(pBuffer + sizeof(XESP_HEAD), 'x', dwEncapsulatedLen - sizeof(XESP_HEAD));
				}
				else
				{
					pEspTail = (PXESP_TAIL) (pBuffer + dwEncapsulatedLen - sizeof(XESP_TAIL));
					pEspTail->bPadLen = bPadLen;
					pEspTail->bNextHeader = bNextHeader;
					memset(pEspTail->abAuthData, 'z', sizeof(pEspTail->abAuthData));
					memset(pBuffer + sizeof(XESP_HEAD), 'x', dwEncapsulatedLen - sizeof(XESP_HEAD) - sizeof(XESP_TAIL));
				}

			}

			// Associate that buffer with the packet
			pUDPPacket->SetUserBuffer(pBuffer, dwEncapsulatedLen, TRUE);
		}
		
		pInterface->Send(pUDPPacket);
	}
	catch(CSpartaException *Ex)
	{
		delete Ex;
		fSuccess = FALSE;
	}

	delete pUDPPacket;

	// Release the payload buffer
	pBuffer ? LocalFree(pBuffer) : 0;

	return fSuccess;
}

//==================================================================================
// ESPMaxTitleEncryptAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with SPI/SeqNum of 0xFFFFFFFF
//   and maximum size to the Xbox via a broadcast
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPMaxTitleEncryptAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendEncryptedESPPacket(pInterface, "FFFFFFFFFFFF", 0xFFFFFFFF, 0xFFFFFFFF, 1500 - IPHEADER_SIZE - UDPHEADER_SIZE);
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPMinTitleEncryptAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with SPI/SeqNum of 0xFFFFFFFF
//   and minimum size to the Xbox via a broadcast
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPMinTitleEncryptAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendEncryptedESPPacket(pInterface, "FFFFFFFFFFFF", 0xFFFFFFFF, 0xFFFFFFFF, sizeof(XESP_HEAD));
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPMaxTitleEncryptUnicastAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with SPI/SeqNum of 0xFFFFFFFF
//   and maximum size to the Xbox via a unicast
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPMaxTitleEncryptUnicastAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendEncryptedESPPacket(pInterface, szDestMac, 0xFFFFFFFF, 0xFFFFFFFF, 1500 - IPHEADER_SIZE - UDPHEADER_SIZE);
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPMinTitleEncryptUnicastAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with SPI/SeqNum of 0xFFFFFFFF
//   and minimum size to the Xbox via a unicast
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPMinTitleEncryptUnicastAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendEncryptedESPPacket(pInterface, szDestMac, 0xFFFFFFFF, 0xFFFFFFFF, sizeof(XESP_HEAD));
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPOneAboveTitleEncryptBroadcastAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with SPI/SeqNum of 0xFFFFFFFF
//   and a size that is one larger than normal to the Xbox via a broadcast
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPOneAboveTitleEncryptBroadcastAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendEncryptedESPPacket(pInterface, "FFFFFFFFFFFF", 0xFFFFFFFF, 0xFFFFFFFF, TITLEENCRYPT_SIZE + 1);
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPOneBelowTitleEncryptBroadcastAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with SPI/SeqNum of 0xFFFFFFFF
//   and a size that is one smaller than normal to the Xbox via a broadcast
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPOneBelowTitleEncryptBroadcastAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendEncryptedESPPacket(pInterface, "FFFFFFFFFFFF", 0xFFFFFFFF, 0xFFFFFFFF, TITLEENCRYPT_SIZE - 1);
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPMaxLanExchangeUnicastAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with SPI of 0x00000000
//   and maximum size to the Xbox via a unicast
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPMaxLanExchangeUnicastAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendEncryptedESPPacket(pInterface, szDestMac, 0x00000000, 0x00112200, 1500 - IPHEADER_SIZE - UDPHEADER_SIZE);
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPMinLanExchangeUnicastAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with SPI of 0x00000000
//   and minimum size to the Xbox via a unicast
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPMinLanExchangeUnicastAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendEncryptedESPPacket(pInterface, szDestMac, 0x00000000, 0x00112200, sizeof(XESP_HEAD));
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPOneAboveLanExchangeUnicastAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with SPI of 0x00000000
//   and a size that is one larger than normal to the Xbox via a unicast
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPOneAboveLanExchangeUnicastAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendEncryptedESPPacket(pInterface, szDestMac, 0x00000000, 0x00112200, LANEXCHANGE_SIZE + 1);
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPOneBelowLanExchangeUnicastAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with SPI of 0x00000000
//   and a size that is one smaller than normal to the Xbox via a unicast
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPOneBelowLanExchangeUnicastAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendEncryptedESPPacket(pInterface, szDestMac, 0x00000000, 0x00112200, LANEXCHANGE_SIZE - 1);
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPLargePadLenAuthAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with an authenticated SPI
//   and a maximum pad length
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPLargePadLenAuthAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendAuthenticatedESPPacket(pInterface, szDestMac, 0x00112200, 0x00112200, 255, UDP_PROTOCOL_VALUE,
			sizeof(XESP_HEAD) + sizeof(XESP_TAIL) + 1);
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPUnknownProtocolAuthAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with an authenticated SPI
//   and a next header field containing an unknown protocol
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPUnknownProtocolAuthAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendAuthenticatedESPPacket(pInterface, szDestMac, 0x00112200, 0x00112200, 0, UNKNOWN_PROTOCOL_VALUE,
			sizeof(XESP_HEAD) + sizeof(XESP_TAIL) + 1);
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPMaxAuthAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with an authenticated SPI
//   and maximum size to the Xbox via a unicast
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPMaxAuthAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendAuthenticatedESPPacket(pInterface, szDestMac, 0x00112200, 0x00112200, 0, UDP_PROTOCOL_VALUE,
			1500 - IPHEADER_SIZE - UDPHEADER_SIZE);
		if(!fRet)
			break;
	}

	return fRet;
}

//==================================================================================
// ESPMaxEncryptAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends an ESP-UDP packet with an encrypted SPI
//   and maximum size to the Xbox via a unicast
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ESPMaxEncryptAttack (CInterface *pInterface, CHAR *szDestMac)
{
	BOOL fRet = FALSE;

	for(INT i = 0; i < ESP_ITERATIONS; ++i)
	{
		fRet = SendEncryptedESPPacket(pInterface, szDestMac, 0x00112201, 0x00112200, 1500 - IPHEADER_SIZE - UDPHEADER_SIZE);
		if(!fRet)
			break;
	}

	return fRet;
}


PATTACK_FUNCTION g_ESPAttackFunctions[ESPATTACK_MAXTEST] = 
{
	ESPMaxTitleEncryptAttack,
	ESPMinTitleEncryptAttack,
	ESPMaxTitleEncryptUnicastAttack,
	ESPMinTitleEncryptUnicastAttack,
	ESPOneAboveTitleEncryptBroadcastAttack,
	ESPOneBelowTitleEncryptBroadcastAttack,
	ESPMaxLanExchangeUnicastAttack,
	ESPMinLanExchangeUnicastAttack,
	ESPOneAboveLanExchangeUnicastAttack,
	ESPOneBelowLanExchangeUnicastAttack,
	ESPLargePadLenAuthAttack,
	ESPUnknownProtocolAuthAttack,
	ESPMaxAuthAttack,
	ESPMaxEncryptAttack
};
