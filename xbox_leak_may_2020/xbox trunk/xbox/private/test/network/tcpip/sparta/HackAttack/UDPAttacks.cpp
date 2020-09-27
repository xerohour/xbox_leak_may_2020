// UDPAttacks.cpp : Contains the Sparta code for various UDP layer attacks
//

#include "stdafx.h"
#include "AttackConstants.h"

//==================================================================================
// SendUDPPacket
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with the specified parameters
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL SendUDPPacket(CInterface *pInterface, CHAR *szDestMac, CHAR *szDestIP, WORD wDestPort, CHAR *szSourceIP, WORD wSourcePort, WORD wLength, BOOL fCalcLength, WORD wChecksum, BOOL fCalcChecksum, DWORD dwPayloadSize)
{
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	CHAR *pBuffer = NULL;
	BOOL fSuccess = TRUE, fUDPPacket = FALSE;

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
		
		// Create the packet
		if(dwPayloadSize >= UDPHEADER_SIZE)
		{
			fUDPPacket = TRUE;
			pIPPacket = new CUDPPacket(MediaType);
		}
		else
			pIPPacket = new CIPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
		pIPPacket->MacHeader.SetProtocolType(0x800);
		pIPPacket->IPHeader.SetVersion(4);
		pIPPacket->IPHeader.SetTOS(0);
		pIPPacket->IPHeader.SetID(1);
		pIPPacket->IPHeader.SetFlag(0);
		pIPPacket->IPHeader.SetFragOffset(0);
		pIPPacket->IPHeader.SetTTL(64);
		pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
		pIPPacket->IPHeader.SetDestAddr(szDestIP);
		pIPPacket->IPHeader.SetSrcAddr(szSourceIP);
		pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
		pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
				
		// If we are dealing with a packet large enough for the UDP header
		if(fUDPPacket)
		{
			CUDPPacket *pUDPPacket = (CUDPPacket *) pIPPacket;
			
			pUDPPacket->UDPHeader.SetSrcPort(wSourcePort);
			pUDPPacket->UDPHeader.SetDestPort(wDestPort);

			// Set up the length fields
			if(fCalcLength)
			{
				pUDPPacket->UDPHeader.SetAutoCalcLength(ON);
				pUDPPacket->IPHeader.SetAutoCalcDatagramLength(ON);
			}
			else
			{
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pUDPPacket->UDPHeader.SetLength(wLength);
				pUDPPacket->IPHeader.SetDatagramLength(wLength + IPHEADER_SIZE);
			}
			
			// Set up the checksum field
			if(fCalcChecksum)
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(ON);
			else
			{
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(wChecksum);
			}
		}
		// Otherwise, this packet is too small for the UDP header
		else
		{
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
		}

		// If we are to send data beyond the header, set up the data buffer
		if(dwPayloadSize > UDPHEADER_SIZE)
		{	
			// Create the buffer for the remaining packet data
			pBuffer = (CHAR *) LocalAlloc(LPTR, dwPayloadSize - UDPHEADER_SIZE);
			memset(pBuffer, (int) 'x', dwPayloadSize - UDPHEADER_SIZE);
			
			// Associate that buffer with the packet
			pIPPacket->SetUserBuffer(pBuffer, dwPayloadSize - UDPHEADER_SIZE, TRUE);				
		}
		else if((dwPayloadSize > IPHEADER_SIZE) && !fUDPPacket)
		{
			// Create the buffer for the remaining packet data
			pBuffer = (CHAR *) LocalAlloc(LPTR, dwPayloadSize - IPHEADER_SIZE);
			memset(pBuffer, (int) 'x', dwPayloadSize - IPHEADER_SIZE);
			
			// Associate that buffer with the packet
			pIPPacket->SetUserBuffer(pBuffer, dwPayloadSize - IPHEADER_SIZE, TRUE);				
		}
		
		pInterface->Send(pIPPacket);
	}
	catch(CSpartaException *Ex)
	{
		delete Ex;
		fSuccess = FALSE;
	}

	delete pIPPacket;

	// Release the payload buffer
	pBuffer ? LocalFree(pBuffer) : 0;

	return fSuccess;
}

//==================================================================================
// UDPSourceMaxAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet from the maximum port number
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPSourceMaxAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, UDPPORT_DEST, IPADDR_SOURCE, MAXWORD, 0, TRUE, 0, TRUE, UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPSourceMinAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet from the minimum port number
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPSourceMinAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, UDPPORT_DEST, IPADDR_SOURCE, 1, 0, TRUE, 0, TRUE, UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPSourceZeroAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet from port number zero
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPSourceZeroAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, UDPPORT_DEST, IPADDR_SOURCE, 0, 0, TRUE, 0, TRUE, UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPDestMaxAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet to the maximum port number
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPDestMaxAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, MAXWORD, IPADDR_SOURCE, UDPPORT_SOURCE, 0, TRUE, 0, TRUE, UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPDestMinAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet to the minimum port number
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPDestMinAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, 1, IPADDR_SOURCE, UDPPORT_SOURCE, 0, TRUE, 0, TRUE, UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPDestZeroAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet to port number zero
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPDestZeroAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, 0, IPADDR_SOURCE, UDPPORT_SOURCE, 0, TRUE, 0, TRUE, UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPLenBelowMinAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a length below the minimum
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenBelowMinAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, UDPPORT_DEST, IPADDR_SOURCE, UDPPORT_SOURCE, UDPHEADER_SIZE - 1, FALSE, 0, TRUE, UDPHEADER_SIZE);
}

//==================================================================================
// UDPLenAtMinAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a length of exactly the minimum
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenAtMinAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, UDPPORT_DEST, IPADDR_SOURCE, UDPPORT_SOURCE, UDPHEADER_SIZE, FALSE, 0, TRUE, UDPHEADER_SIZE);
}

//==================================================================================
// UDPLenAboveMinAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a length above the minimum
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenAboveMinAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, UDPPORT_DEST, IPADDR_SOURCE, UDPPORT_SOURCE, UDPHEADER_SIZE + 1, FALSE, 0, TRUE, UDPHEADER_SIZE + 1);
}

//==================================================================================
// UDPLenAboveTotalAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a length above total bytes in packet
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenAboveTotalAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, UDPPORT_DEST, IPADDR_SOURCE, UDPPORT_SOURCE, UDPHEADER_SIZE + 1, FALSE, 0, TRUE, UDPHEADER_SIZE);
}

//==================================================================================
// UDPLenBelowTotalAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a length below the total bytes in packet
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenBelowTotalAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, UDPPORT_DEST, IPADDR_SOURCE, UDPPORT_SOURCE, UDPHEADER_SIZE + 1, FALSE, 0, TRUE, UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPLenMaxAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a length below the total bytes in packet
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenMaxAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, UDPPORT_DEST, IPADDR_SOURCE, UDPPORT_SOURCE, 1500 - IPHEADER_SIZE, FALSE, 0, TRUE, 1500 - IPHEADER_SIZE);
}

//==================================================================================
// UDPCheckOnesAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a valid checksum of all ones
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPCheckOnesAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, UDPPORT_DEST, IPADDR_SOURCE, UDPPORT_SOURCE, 0, TRUE, 0xFFFF, FALSE, UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPCheckZerosAttack
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a checksum of all zeros (indicating not computed)
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPCheckZerosAttack (CInterface *pInterface, CHAR *szDestMac)
{
	return SendUDPPacket(pInterface, szDestMac, IPADDR_DEST, UDPPORT_DEST, IPADDR_SOURCE, UDPPORT_SOURCE, 0, TRUE, 0x0000, FALSE, UDPHEADER_SIZE + 2);
}

PATTACK_FUNCTION g_UDPAttackFunctions[UDPATTACK_MAXTEST] = 
{
	UDPSourceMaxAttack,
	UDPSourceMinAttack,
	UDPSourceZeroAttack,
	UDPDestMaxAttack,
	UDPDestMinAttack,
	UDPDestZeroAttack,
	UDPLenBelowMinAttack,
	UDPLenAtMinAttack,
	UDPLenAboveMinAttack,
	UDPLenAboveTotalAttack,
	UDPLenBelowTotalAttack,
	UDPLenMaxAttack,
	UDPCheckOnesAttack,
	UDPCheckZerosAttack
};
