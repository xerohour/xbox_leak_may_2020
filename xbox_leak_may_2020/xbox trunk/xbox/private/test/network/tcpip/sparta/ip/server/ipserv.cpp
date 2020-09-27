#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "ipserv.h"
#include "myaddr.h"


PIP_SERV_FUNCTION	g_ServerFunctions[IPMSG_MAXTEST] = 
{
	IPVersionSixServ,												// IPMSG_VERSION_SIX,
	IPVersionBadServ,												// IPMSG_VERSION_BAD,
	IPHeaderLenBelowMinServ,										// IPMSG_HEADLEN_BELOWMIN,
	IPHeaderLenMaxServ,												// IPMSG_HEADLEN_MAX,
	IPHeaderLenAboveTotalLenServ,									// IPMSG_HEADLEN_ABOVETOTALLEN,
	IPHeaderLenAboveAvailServ,										// IPMSG_HEADLEN_ABOVEAVAIL,
	IPTosNormalServ,												// IPMSG_TOS_NORMAL,
	IPTosNetControlServ,											// IPMSG_TOS_NETCONTROL,
	IPTosLowDelayServ,												// IPMSG_TOS_LOWDELAY,
	IPTosHighThroughputServ,										// IPMSG_TOS_HIGHTHROUGH,
	IPTosHighReliabilityServ,										// IPMSG_TOS_HIGHRELI,
	IPTosReservedServ,												// IPMSG_TOS_RESERVED,
	IPLengthBelowMinServ,											// IPMSG_LENGTH_BELOWMIN,
	IPLengthAtMinServ,												// IPMSG_LENGTH_ATMIN,
	IPLengthAboveMinServ,											// IPMSG_LENGTH_ABOVEMIN,
	IPLengthAboveTotalServ,											// IPMSG_LENGTH_ABOVETOTAL,
	IPLengthBelowTotalServ,											// IPMSG_LENGTH_BELOWTOTAL,
	IPLengthMaxServ,												// IPMSG_LENGTH_MAXIMUM,
	IPIDMinServ,													// IPMSG_ID_MIN,
	IPIDMaxServ,													// IPMSG_ID_MAX,
	IPFlagReservedSetServ,											// IPMSG_FLAG_RESERVEDSET,
	IPFlagDontFragAndMoreFragsServ,									// IPMSG_FLAG_DFANDMF,
	IPTtlMinServ,													// IPMSG_TTL_MIN,
	IPTtlMaxServ,													// IPMSG_TTL_MAX,
	IPProtocolUnsupportedServ,										// IPMSG_PRO_UNSUP,
	IPProtocolICMPTooSmallServ,										// IPMSG_PRO_ICMP_TOOSMALL,
	IPProtocolIGMPTooSmallServ,										// IPMSG_PRO_IGMP_TOOSMALL,
	IPProtocolTCPTooSmallServ,										// IPMSG_PRO_TCP_TOOSMALL,
	IPProtocolUDPTooSmallServ,										// IPMSG_PRO_UDP_TOOSMALL,
	IPProtocolUnsupportedTooSmallServ,								// IPMSG_PRO_UNSUP_TOOSMALL,
	IPChecksumBadServ,												// IPMSG_CHECKSUM_BAD,
	IPOptionsAboveAvailServ,										// IPMSG_OPT_ABOVEAVAIL,
	IPOptionsExactSizeServ,											// IPMSG_OPT_EXACTSIZE,
	IPOptionsEndOfOpsLastServ,										// IPMSG_OPT_ENDOFOPSLAST,
	IPOptionsEndOfOpsFirstServ,										// IPMSG_OPT_ENDOFOPSFIRST,
	IPFragMaxSimulReassemblyServ,									// IPMSG_FRAG_MAXSIMREASM,
	IPFragAboveMaxSimulReassemblyServ,								// IPMSG_FRAG_ABOVEMAXSIMREASM,
	IPFragFullReverseServ,											// IPMSG_FRAG_FULLREVERSE,
	IPFragMidReverseServ,											// IPMSG_FRAG_MIDREVERSE,
	IPFragLastInSecondServ,											// IPMSG_FRAG_LASTINSECOND,
	IPFragMixedSizeServ,											// IPMSG_FRAG_MIXEDSIZE,
	IPFragOneHundredServ,											// IPMSG_FRAG_HUNDRED,
	IPFragMultipleLastFragmentServ,									// IPMSG_FRAG_MULTILASTFRAG,
	IPFragOverlappingFragmentsServ,									// IPMSG_FRAG_OVERLAPPING,
	IPFragMaxDatagramSizeServ,										// IPMSG_FRAG_MAXDGRAMSIZE,
	IPFragMaxReassemblySizeServ,									// IPMSG_FRAG_MAXREASMSIZE,
	IPFragAboveMaxReassemblySizeServ,								// IPMSG_FRAG_ABOVEMAXREASMSIZE,
	IPFragOversizedFragmentsServ,									// IPMSG_FRAG_OVERSIZED,
	IPAttackTeardropServ,											// IPMSG_ATTACK_TEARDROP,
	IPAttackNewTeardropServ,										// IPMSG_ATTACK_NEWTEAR,
	IPAttackImpTeardropServ,										// IPMSG_ATTACK_IMPTEAR,
	IPAttackSynDropServ,											// IPMSG_ATTACK_SYNDROP,
	IPAttackZeroLenOptServ,											// IPMSG_ATTACK_ZEROLENOPT,
	IPAttackBonkServ,												// IPMSG_ATTACK_BONK,
	IPAttackNesteaServ												// IPMSG_ATTACK_NESTEA,
};

extern "C" void PrintDebug(TCHAR *strMessage)
{
   _tprintf(strMessage);		// change to redirect your output
}

extern "C" void LogDebugMessage(TCHAR *strMessage)
{
   _tprintf(strMessage);		// change to redirect your output
}

//==================================================================================
// DllMain
//----------------------------------------------------------------------------------
//
// Description: Global entry point
//
// Arguments:
//	HINSTANCE	hInstance	Handle to calling instance
//	DWORD		dwReason	Reason for calling entry point
//	LPVOID		lpContext	
//
// Returns:
//	TRUE on successful attach or detach, FALSE otherwise
//==================================================================================
BOOL WINAPI DllMain(IN HINSTANCE hInstance, IN DWORD dwReason, IN LPVOID lpContext)
{
    switch (dwReason) {

    case DLL_PROCESS_ATTACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}


//==================================================================================
// IpServerCallbackFunction
//----------------------------------------------------------------------------------
//
// Description: Function exported for Netsync callbacks
//
// Arguments:
//  HANDLE      hSessionObject  Handle to session instance
//	HANDLE		hNetsyncObject	Handle to calling instance
//	DWORD		FromAddr		Address of sender of message that caused the callback
//	CHAR		*ReceiveMessage	Buffer containing the packet that caused the callback
//	LPVOID		lpContext		Context indicating which session this belongs to
//
// Returns:
//	Returns TRUE if the message was received was successfully handled, FALSE otherwise
//==================================================================================
BOOL IpServerCallbackFunction(HANDLE hSessionObject, HANDLE hNetsyncObject, DWORD FromAddr, DWORD dwMessageType, DWORD dwMessageSize, CHAR *ReceiveMessage, LPVOID lpContext)
{
	PSESSION_INFO pSessionInfo = (PSESSION_INFO) lpContext;
	PIP_TESTREQ pTestRequest = (PIP_TESTREQ) ReceiveMessage;
	BOOL fSuccess = TRUE;

	switch(pTestRequest->dwMessageId)
	{
	case IPMSG_TESTREQ:
		// Client is requesting a test.
		// If we support that test, ACK it and run the server side of the case
		if(pTestRequest->TestId < IPMSG_MAXTEST)
		{
			pSessionInfo->pCurrentRequest = pTestRequest;

			Sleep(2000);

			// Run the test
			if(!g_ServerFunctions[pTestRequest->TestId](FromAddr, pSessionInfo))
			{
				_tprintf(TEXT("Error running server-side of test %u\n"), pTestRequest->TestId);
				break;
			}

			pSessionInfo->pCurrentRequest = NULL;
		}

		break;
	case IPMSG_TESTDONE:
		// Client is signalling that this test is done
		// May want to add and ack later if this isn't reliable enough
		fSuccess = FALSE;
		break;
	default:
		// Unknown message received
		break;
	}

	return fSuccess;
}


//==================================================================================
// IpServerStartFunction
//----------------------------------------------------------------------------------
//
// Description: Funtion called once for each session that is created
//
// Arguments:
//  HANDLE      hSessionObject  Handle to session instance
//	HANDLE		hNetsyncObject	Handle to calling instance
//	BYTE		byClientCount	Number of clients this test is being started with
//	DWORD		*ClientAddrs	List of client addresses (number of addresses indicated by byClientCount)
//	WORD		LowPort			The lowest port to use for this test
//	WORD		HighPort		The higest port to use for this test
//
// Returns:
//	Returns a pointer to the session information that is being stored for this session
//==================================================================================
LPVOID IpServerStartFunction(HANDLE hSessionObject, HANDLE hNetsyncObject, BYTE byClientCount, DWORD *ClientAddrs, WORD LowPort, WORD HighPort)
{
	PSESSION_INFO pCurrentSession = NULL;
	IN_ADDR ServerAddr;
	CHAR szServerAddr[16];

	pCurrentSession = (PSESSION_INFO) LocalAlloc(LPTR, sizeof(SESSION_INFO));
	_tprintf(TEXT("Starting session: 0x%08x\n"), pCurrentSession);

	// Fill the session info structure
	pCurrentSession->hNetsyncObject = hNetsyncObject;
	pCurrentSession->byClientCount = byClientCount;
	pCurrentSession->ClientAddrs = ClientAddrs;
	pCurrentSession->LowPort = LowPort;
	pCurrentSession->HighPort = HighPort;

	// Calculate the server address string
	ServerAddr.S_un.S_addr = GetMyAddr();
	strcpy(pCurrentSession->szServerIPAddr, inet_ntoa(ServerAddr));

	// Initialize the SPARTA variables for this session
	try
    {
		pCurrentSession->pInterfaceList = new CInterfaceList;
		pCurrentSession->pInterface = new CInterface(pCurrentSession->pInterfaceList->GetNameAt(0));
	}
    catch(CSpartaException *Ex)
    {
        _tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
        delete Ex;
    }

	// If anything needs to be setup for all the tests or if the currently
	// connected clients need to be tracked, it will be done here
	return (LPVOID) pCurrentSession;
}

//==================================================================================
// IpServerStopFunction
//----------------------------------------------------------------------------------
//
// Description: Function called once when each session is closed
//
// Arguments:
//  HANDLE      hSessionObject  Handle to session instance
//	HANDLE		hNetsyncObject	Handle to calling instance
//	LPVOID		lpContext		Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
VOID IpServerStopFunction(HANDLE hSessionObject, HANDLE hNetsyncObject, LPVOID lpContext)
{
	PSESSION_INFO pCurrentSession = (PSESSION_INFO) lpContext;

	_tprintf(TEXT("Stopping session: 0x%08x\n"), pCurrentSession);

	// Uninitialize the SPARTA variables for this session
	pCurrentSession->pInterfaceList ? delete pCurrentSession->pInterfaceList : 0;
	pCurrentSession->pInterface ? delete pCurrentSession->pInterface : 0;
	pCurrentSession->pCurrentRequest = NULL;

	LocalFree(pCurrentSession);

	// If anything needs to be cleaned up, it will be done here.
	return;
}


//==================================================================================
// Test functions
//==================================================================================

//==================================================================================
// SendIPPacket
//----------------------------------------------------------------------------------
//
// Description: Test that sends an IP packet with the specified parameters
//
// Arguments:
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//	DWORD			FromAddr		Address of user that requested the test
//	BYTE			Version			Version number of the outgoing datagram
//	BYTE			HeaderLength	Header length of the outgoing datagram
//	BYTE			TypeOfService	Type-of-service of the outgoing datagram
//	WORD			TotalLength		Total length of the outgoing datagram
//	WORD			Id				IP identification number of the outgoing datagram
//	BYTE			Flags			Flags field of the outgoing datagram
//	WORD			FragOffset		Fragment offsett of the outgoing datagram
//	BYTE			TimeToLive		Time-to-live of the outgoing datagram
//	BYTE			Protocol		Protocol field of the outgoing datagram
//	DWORD			PayloadSize		Number of bytes allocated for the packet
//	BOOL			fCalcChecksum	Indicates whether to generate a checksum or use a default bad value
//	BOOL			fCalcHdrLen		Indicates whether to generate the header len or use the HeaderLength value
//  BOOL			fPadHdr			Indicates whether to pad the header with options to the size of HeaderLength
//	BOOL			fCalcTotalLen	Indicates whether to generate the total len or use the TotalLength value
//	
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL SendIPPacket(PSESSION_INFO pSessionInfo, DWORD FromAddr, BYTE Version, BYTE HeaderLength, BYTE TypeOfService, WORD TotalLength, WORD Id, BYTE Flags, WORD FragOffset, BYTE TimeToLive, BYTE Protocol, DWORD PayloadSize, BOOL fCalcChecksum, BOOL fCalcHdrLen, BOOL fPadHdr, BOOL fCalcTotalLen)
{
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16], *pBuffer = NULL;
	BOOL fSuccess = TRUE;

	// Format the client Mac address string
	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Format the client IP address string
	ClientAddr.S_un.S_addr = FromAddr;
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	// Watch for SPARTA exceptions
	try
	{
		// Create a destination mac address from the address reported by the client
		MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
		CMacAddress ClientMacAddr(MediaType, szClientMacAddr);
		
		// Create the packet
		((Protocol == PROTOCOL_UDP) && (PayloadSize >= UDPHEADER_SIZE)) ? pIPPacket = new CUDPPacket(MediaType) : pIPPacket = new CIPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
		pIPPacket->MacHeader.SetProtocolType(0x800);
		pIPPacket->IPHeader.SetVersion(Version);
		pIPPacket->IPHeader.SetTOS(TypeOfService);
		pIPPacket->IPHeader.SetID(Id);
		pIPPacket->IPHeader.SetFlag(Flags);
		pIPPacket->IPHeader.SetFragOffset(FragOffset);
		pIPPacket->IPHeader.SetTTL(TimeToLive);
		pIPPacket->IPHeader.SetProtocolType(Protocol);
		pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
		pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
		
		// Set up the total length field
		if(fCalcTotalLen)
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(ON);
		else
		{
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
			pIPPacket->IPHeader.SetDatagramLength(TotalLength);
		}
		
		// Set up the header length field
		if(fCalcHdrLen)
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
		else
		{
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(OFF);
			pIPPacket->IPHeader.SetHdrLength(HeaderLength);

			// If the header is bigger than the default, fill it with no-ops
			if((HeaderLength > IPHEADER_SIZE) && fPadHdr)
			{
				// For each additional double-word in the header beyond the default, add 4 no-opts (1 byte each)
				for(INT DoubleWordIndex = 0; DoubleWordIndex < HeaderLength - IPHEADER_SIZE ;  DoubleWordIndex++)
				{
					pIPPacket->IPHeader.CreateIPOption(IP_OPTIONS_NO_OPERATION);
				}
			}
		}
		
		// Set up the IP header checksum
		if(fCalcChecksum)
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
		else
		{
			pIPPacket->IPHeader.SetAutoCalcChecksum(OFF);
			pIPPacket->IPHeader.SetChecksum(0xFFFF);	// Not guaranteed to fail, but close enough
		}
		
		// Set up the UDP fields if necessary
		if((Protocol == PROTOCOL_UDP)  && (PayloadSize >= UDPHEADER_SIZE))
		{
			WORD HeaderSize = fCalcHdrLen ? IPHEADER_SIZE : HeaderLength * 4;

			CUDPPacket *pUDPPacket = (CUDPPacket *) pIPPacket;
			
			pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
			pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);

			// Hard to explain this one...  Basically, for tests where we are overriding the total length
			// field of the IP datagram, and that overridden value is less than the actual amount of data in
			// the packet (including its payload)...  Then we also want to override the length field in the
			// UDP header so that the generated UDP packet is still valid
			if(!fCalcTotalLen && (TotalLength >= UDPHEADER_SIZE + IPHEADER_SIZE) && (TotalLength < PayloadSize + IPHEADER_SIZE))
			{
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(TotalLength - IPHEADER_SIZE);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
			}
			else
			{
				pUDPPacket->UDPHeader.SetAutoCalcLength(ON);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(ON);
			}
			
			// If we are to send data beyond the header, set up the data buffer
			if(PayloadSize > UDPHEADER_SIZE)
			{
				// Create the buffer for the remaining packet data
				pBuffer = (CHAR *) LocalAlloc(LPTR, PayloadSize - UDPHEADER_SIZE);
				memset(pBuffer, (int) 'x', PayloadSize - UDPHEADER_SIZE);
				
				// Associate that buffer with the packet
				pUDPPacket->SetUserBuffer(pBuffer, PayloadSize - UDPHEADER_SIZE, TRUE);				
			}
		}
		else if(PayloadSize > 0)
		{
			// Create the buffer for the remaining packet data
			pBuffer = (CHAR *) LocalAlloc(LPTR, PayloadSize);
			memset(pBuffer, (int) 'x', PayloadSize);
			
			// Associate that buffer with the packet
			pIPPacket->SetUserBuffer(pBuffer, PayloadSize, TRUE);								
		}
		
		pSessionInfo->pInterface->Send(*pIPPacket);
	}
	catch(CSpartaException *Ex)
	{
		_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
		delete Ex;
		fSuccess = FALSE;
	}

	if(pIPPacket)
		((Protocol == PROTOCOL_UDP) && (PayloadSize >= UDPHEADER_SIZE)) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;

	pBuffer ? LocalFree(pBuffer) : 0;

	return fSuccess;
}

//==================================================================================
// IPVersionSixServ
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
BOOL IPVersionSixServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_SIX, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1, 
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPVersionBadServ
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
BOOL IPVersionBadServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_BAD, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1, 
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPHeaderLenBelowMinServ
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
BOOL IPHeaderLenBelowMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		IPHEADER_BELOWMIN, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1, 
		TRUE, 
		FALSE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPHeaderLenMaxServ
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
BOOL IPHeaderLenMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		IPHEADER_MAX, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1, 
		TRUE, 
		FALSE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPHeaderLenAboveTotalLenServ
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
BOOL IPHeaderLenAboveTotalLenServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		IPHEADER_MAX, 
		IPTOS_DEFAULT, 
		UDPHEADER_SIZE + IPHEADER_SIZE + 1, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1, 
		TRUE, 
		FALSE,
		TRUE,
		FALSE);
}

//==================================================================================
// IPHeaderLenAboveAvailServ
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
BOOL IPHeaderLenAboveAvailServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		IPHEADER_MAX, 
		IPTOS_DEFAULT, 
		UDPHEADER_SIZE + IPHEADER_SIZE + 1, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1, 
		TRUE, 
		FALSE,
		FALSE,
		FALSE);
}

//==================================================================================
// IPTosNormalServ
//----------------------------------------------------------------------------------
//
// Description: Server sends IP datagram indicating a normal type-of-service
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosNormalServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_NORMAL, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1, 
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPTosNetControlServ
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
BOOL IPTosNetControlServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_NETCONTROL, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1, 
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPTosLowDelayServ
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
BOOL IPTosLowDelayServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_LOWDELAY, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1, 
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPTosHighThroughputServ
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
BOOL IPTosHighThroughputServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_HIGHTHRUPUT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1, 
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPTosHighReliabilityServ
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
BOOL IPTosHighReliabilityServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_HIGHRELI, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1, 
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPTosReservedServ
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
BOOL IPTosReservedServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_RESERVED, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1, 
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPLengthBelowMinServ
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
BOOL IPLengthBelowMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		IPLENGTH_BELOWMIN, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		0, 
		0, 
		TRUE, 
		TRUE,
		TRUE,
		FALSE);
}

//==================================================================================
// IPLengthAtMinServ
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
BOOL IPLengthAtMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		IPLENGTH_ATMIN, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		0, 
		0,
		TRUE, 
		TRUE,
		TRUE,
		FALSE);
}

//==================================================================================
// IPLengthAboveMinServ
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
BOOL IPLengthAboveMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		UDPHEADER_SIZE + IPHEADER_SIZE + 1, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1,
		TRUE, 
		TRUE,
		TRUE,
		FALSE);
}

//==================================================================================
// IPLengthAboveTotalServ
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
BOOL IPLengthAboveTotalServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		UDPHEADER_SIZE + IPHEADER_SIZE + 2, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1,
		TRUE, 
		TRUE,
		TRUE,
		FALSE);
}

//==================================================================================
// IPLengthBelowTotalServ
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
BOOL IPLengthBelowTotalServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		UDPHEADER_SIZE + IPHEADER_SIZE + 1, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 2,
		TRUE, 
		TRUE,
		TRUE,
		FALSE);
}

//==================================================================================
// IPLengthMaxServ
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
BOOL IPLengthMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		IPLENGTH_MAX, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		IPLENGTH_MAX - IPHEADER_SIZE,
		TRUE, 
		TRUE,
		TRUE,
		FALSE);
}

//==================================================================================
// IPIDMinServ
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
BOOL IPIDMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_MIN, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1,
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPIDMaxServ
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
BOOL IPIDMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_MAX, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1,
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPFlagReservedSetServ
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
BOOL IPFlagReservedSetServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		IPFLAG_RESERVED, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1,
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPFlagDontFragAndMoreFragsServ
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
BOOL IPFlagDontFragAndMoreFragsServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		IPFLAG_DONTFRAG | IPFLAG_MOREFRAGS, 
		0, 
		IPTTL_DEFAULT, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1,
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPTtlMinServ
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
BOOL IPTtlMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_MIN, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1,
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPTtlMaxServ
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
BOOL IPTtlMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_MAX, 
		PROTOCOL_UDP, 
		UDPHEADER_SIZE + 1,
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPProtocolUnsupportedServ
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
BOOL IPProtocolUnsupportedServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		IPPROTOCOL_UNSUPPORTED, 
		1,
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPProtocolICMPTooSmallServ
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
BOOL IPProtocolICMPTooSmallServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		IPPROTOCOL_ICMP, 
		0,
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPProtocolIGMPTooSmallServ
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
BOOL IPProtocolIGMPTooSmallServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		IPPROTOCOL_IGMP, 
		0,
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPProtocolTCPTooSmallServ
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
BOOL IPProtocolTCPTooSmallServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		IPPROTOCOL_TCP, 
		0,
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPProtocolUDPTooSmallServ
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
BOOL IPProtocolUDPTooSmallServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		IPPROTOCOL_UDP, 
		0,
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPProtocolUnsupportedTooSmallServ
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
BOOL IPProtocolUnsupportedTooSmallServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		IPPROTOCOL_UNSUPPORTED, 
		0,
		TRUE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPChecksumBadServ
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
BOOL IPChecksumBadServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendIPPacket(pSessionInfo,
		FromAddr, 
		IPVERSION_DEFAULT, 
		0, 
		IPTOS_DEFAULT, 
		0, 
		IPID_DEFAULT, 
		0, 
		0, 
		IPTTL_DEFAULT, 
		IPPROTOCOL_UDP, 
		UDPHEADER_SIZE + 1,
		FALSE, 
		TRUE,
		TRUE,
		TRUE);
}

//==================================================================================
// IPOptionsAboveAvailServ
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
BOOL IPOptionsAboveAvailServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CIPPacket *pIPPacket = NULL;
	CIPOption *pIPOption = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16], *pBuffer = NULL;
	BOOL fSuccess = TRUE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	// Watch for SPARTA exceptions
	try
	{
		// Hard-code the address for now.  We'll work something better out later.
		MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
		CMacAddress ClientMacAddr(MediaType, szClientMacAddr);
		
		// Create the packet
		pIPPacket = new CIPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
		pIPPacket->MacHeader.SetProtocolType(0x800);
		pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
		pIPPacket->IPHeader.SetTOS(IPTOS_DEFAULT);
		pIPPacket->IPHeader.SetID(IPID_DEFAULT);
		pIPPacket->IPHeader.SetFlag(0);
		pIPPacket->IPHeader.SetFragOffset(0);
		pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
		pIPPacket->IPHeader.SetProtocolType(IPPROTOCOL_UDP);
		pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
		pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
		
		pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
		pIPPacket->IPHeader.SetDatagramLength(IPHEADER_DEFAULT + 4);

		pIPPacket->IPHeader.SetAutoCalcHeaderLength(OFF);
		pIPPacket->IPHeader.SetHdrLength(IPHEADER_DEFAULT + 4);

		pIPOption = pIPPacket->IPHeader.CreateIPOption(IP_OPTIONS_STRICT_SOURCE_RECORD_ROUTE);
		pIPOption->SetOptionBufferLength(sizeof(DWORD));
		pIPOption->SetOptionLength(11);
		pIPOption->SetOptionOffset(8);

		pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
		
		pSessionInfo->pInterface->Send(*pIPPacket);
	}
	catch(CSpartaException *Ex)
	{
		_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
		delete Ex;
		fSuccess = FALSE;
	}

	pIPPacket ? delete pIPPacket : 0;

	return fSuccess;
}

//==================================================================================
// IPOptionsExactSizeServ
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
BOOL IPOptionsExactSizeServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPOption *pIPOption = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16], PayloadData[1], *pBuffer = NULL;
	BOOL fSuccess = TRUE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	// Watch for SPARTA exceptions
	try
	{
		// Hard-code the address for now.  We'll work something better out later.
		MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
		CMacAddress ClientMacAddr(MediaType, szClientMacAddr);
		
		// Create the packet
		pUDPPacket = new CUDPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pUDPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pUDPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
		pUDPPacket->MacHeader.SetProtocolType(0x800);
		pUDPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
		pUDPPacket->IPHeader.SetTOS(IPTOS_DEFAULT);
		pUDPPacket->IPHeader.SetID(IPID_DEFAULT);
		pUDPPacket->IPHeader.SetFlag(0);
		pUDPPacket->IPHeader.SetFragOffset(0);
		pUDPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
		pUDPPacket->IPHeader.SetProtocolType(IPPROTOCOL_UDP);
		pUDPPacket->IPHeader.SetDestAddr(szClientIPAddr);
		pUDPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
		
		pUDPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
		pUDPPacket->IPHeader.SetDatagramLength(IPHEADER_DEFAULT + 8 + UDPHEADER_SIZE + 1);

		pUDPPacket->IPHeader.SetAutoCalcHeaderLength(OFF);
		pUDPPacket->IPHeader.SetHdrLength(IPHEADER_DEFAULT + 8);

		pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_NO_OPERATION);
		pIPOption = pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_STRICT_SOURCE_RECORD_ROUTE);
		pIPOption->SetOptionBufferLength(3 + sizeof(DWORD));
		pIPOption->SetOptionLength(3 + sizeof(DWORD));
		pIPOption->SetOptionOffset(4);
		pIPOption->SetIPAddrAt(4, szClientIPAddr);

		pUDPPacket->IPHeader.SetAutoCalcChecksum(ON);
		
		pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
		pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
		pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
		pUDPPacket->UDPHeader.SetLength(9);
		pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
		pUDPPacket->UDPHeader.SetChecksum(0);

		PayloadData[0] = 'x';
		pUDPPacket->SetUserBuffer(PayloadData, 1, TRUE);

		pSessionInfo->pInterface->Send(*pUDPPacket);
	}
	catch(CSpartaException *Ex)
	{
		_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
		delete Ex;
		fSuccess = FALSE;
	}

	pUDPPacket ? delete pUDPPacket : 0;

	return fSuccess;
}

//==================================================================================
// IPOptionsEndOfOpsLastServ
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
BOOL IPOptionsEndOfOpsLastServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPOption *pIPOption = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16], PayloadData[1], *pBuffer = NULL;
	BOOL fSuccess = TRUE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	// Watch for SPARTA exceptions
	try
	{
		// Hard-code the address for now.  We'll work something better out later.
		MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
		CMacAddress ClientMacAddr(MediaType, szClientMacAddr);
		
		// Create the packet
		pUDPPacket = new CUDPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pUDPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pUDPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
		pUDPPacket->MacHeader.SetProtocolType(0x800);
		pUDPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
		pUDPPacket->IPHeader.SetTOS(IPTOS_DEFAULT);
		pUDPPacket->IPHeader.SetID(IPID_DEFAULT);
		pUDPPacket->IPHeader.SetFlag(0);
		pUDPPacket->IPHeader.SetFragOffset(0);
		pUDPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
		pUDPPacket->IPHeader.SetProtocolType(IPPROTOCOL_UDP);
		pUDPPacket->IPHeader.SetDestAddr(szClientIPAddr);
		pUDPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
		
		pUDPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
		pUDPPacket->IPHeader.SetDatagramLength(IPHEADER_DEFAULT + 8 + UDPHEADER_SIZE + 1);

		pUDPPacket->IPHeader.SetAutoCalcHeaderLength(OFF);
		pUDPPacket->IPHeader.SetHdrLength(IPHEADER_DEFAULT + 8);

		pIPOption = pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_STRICT_SOURCE_RECORD_ROUTE);
		pIPOption->SetOptionBufferLength(3 + sizeof(DWORD));
		pIPOption->SetOptionLength(3 + sizeof(DWORD));
		pIPOption->SetOptionOffset(4);
		pIPOption->SetIPAddrAt(4, szClientIPAddr);
		pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_END_OF_OPTIONLIST);

		pUDPPacket->IPHeader.SetAutoCalcChecksum(ON);
		
		pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
		pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
		pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
		pUDPPacket->UDPHeader.SetLength(9);
		pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
		pUDPPacket->UDPHeader.SetChecksum(0);
		
		PayloadData[0] = 'x';
		pUDPPacket->SetUserBuffer(PayloadData, 1, TRUE);

		pSessionInfo->pInterface->Send(*pUDPPacket);
	}
	catch(CSpartaException *Ex)
	{
		_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
		delete Ex;
		fSuccess = FALSE;
	}

	pUDPPacket ? delete pUDPPacket : 0;

	return fSuccess;
}

//==================================================================================
// IPOptionsEndOfOpsFirstServ
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
BOOL IPOptionsEndOfOpsFirstServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPOption *pIPOption = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16], PayloadData[1], *pBuffer = NULL;
	BOOL fSuccess = TRUE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	// Watch for SPARTA exceptions
	try
	{
		// Hard-code the address for now.  We'll work something better out later.
		MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
		CMacAddress ClientMacAddr(MediaType, szClientMacAddr);
		
		// Create the packet
		pUDPPacket = new CUDPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pUDPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pUDPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
		pUDPPacket->MacHeader.SetProtocolType(0x800);
		pUDPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
		pUDPPacket->IPHeader.SetTOS(IPTOS_DEFAULT);
		pUDPPacket->IPHeader.SetID(IPID_DEFAULT);
		pUDPPacket->IPHeader.SetFlag(0);
		pUDPPacket->IPHeader.SetFragOffset(0);
		pUDPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
		pUDPPacket->IPHeader.SetProtocolType(IPPROTOCOL_UDP);
		pUDPPacket->IPHeader.SetDestAddr(szClientIPAddr);
		pUDPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
		
		pUDPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
		pUDPPacket->IPHeader.SetDatagramLength(IPHEADER_DEFAULT + 8 + UDPHEADER_SIZE);

		pUDPPacket->IPHeader.SetAutoCalcHeaderLength(OFF);
		pUDPPacket->IPHeader.SetHdrLength(IPHEADER_DEFAULT + 8);

		pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_END_OF_OPTIONLIST);
		pIPOption = pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_STRICT_SOURCE_RECORD_ROUTE);
		pIPOption->SetOptionBufferLength(3 + sizeof(DWORD));
		pIPOption->SetOptionLength(3 + sizeof(DWORD));
		pIPOption->SetOptionOffset(4);
		pIPOption->SetIPAddrAt(4, szClientIPAddr);

		pUDPPacket->IPHeader.SetAutoCalcChecksum(ON);
		
		pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
		pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
		pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
		pUDPPacket->UDPHeader.SetLength(9);
		pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
		pUDPPacket->UDPHeader.SetChecksum(0);
		
		PayloadData[0] = 'x';
		pUDPPacket->SetUserBuffer(PayloadData, 1, TRUE);

		pSessionInfo->pInterface->Send(*pUDPPacket);
	}
	catch(CSpartaException *Ex)
	{
		_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
		delete Ex;
		fSuccess = FALSE;
	}

	pUDPPacket ? delete pUDPPacket : 0;

	return fSuccess;
}

//==================================================================================
// IPFragFullReverseServ
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
BOOL IPFragFullReverseServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[16];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr); // tristanjX

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = (DEFAULT_FRAG_COUNT - 1); i >= 0; --i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
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
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(64);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
			}

			// Send the packet
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragMidReverseServ
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
BOOL IPFragMidReverseServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[16];

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr); // tristanjX

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < DEFAULT_FRAG_COUNT; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			pIPPacket->IPHeader.SetFlag((i == DEFAULT_FRAG_COUNT - 1) ? 0 : IPFLAG_MOREFRAGS);

			// Based on which packet we are sending set the payload data and the fragment offset
			switch(i)
			{
				CUDPPacket *pUDPPacket;
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragLastInSecondServ
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
BOOL IPFragLastInSecondServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[16];

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr); // tristanjX

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < DEFAULT_FRAG_COUNT; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);


			// Based on which packet we are sending set the payload data and the fragment offset
			switch(i)
			{
				CUDPPacket *pUDPPacket;
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragMixedSizeServ
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
BOOL IPFragMixedSizeServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[32];

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr); // tristanjX

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < DEFAULT_FRAG_COUNT; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
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
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragOneHundredServ
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
BOOL IPFragOneHundredServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[16];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr); // tristanjX

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < 100; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
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
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(1600);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
			}

			// Send the packet
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragMultipleLastFragmentServ
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
BOOL IPFragMultipleLastFragmentServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[16];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr); // tristanjX

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < 3; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			// Manually calculate length.  We're really adding extra data.
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
			pIPPacket->IPHeader.SetDatagramLength(IPHEADER_SIZE + 16);

			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragOverlappingFragmentsServ
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
BOOL IPFragOverlappingFragmentsServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[16];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr); // tristanjX

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < 7; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			// Manually calculate length.  We're really adding extra data.
			pIPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);

			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragMaxDatagramSizeServ
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
BOOL IPFragMaxDatagramSizeServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[1024];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr); // tristanjX

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < 64; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 1);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
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
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
				pUDPPacket->UDPHeader.SetLength(64 * 1024 - 1);
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(0);
			}

			// Send the packet
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragMaxReassemblySizeServ
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
BOOL IPFragMaxReassemblySizeServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[1024];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr);

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 2);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragAboveMaxReassemblySizeServ
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
BOOL IPFragAboveMaxReassemblySizeServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[1024];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr); // tristanjX

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 3);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
			
			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragMaxSimulReassemblyServ
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
BOOL IPFragMaxSimulReassemblyServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[16];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr); // tristanjX

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));
	
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
				pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
				pIPPacket->MacHeader.SetProtocolType(0x800);
				pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
				pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
				pIPPacket->IPHeader.SetID(IPID_DEFAULT + j);
				pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
				pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
				pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
				pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
				pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
				pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

				switch(i)
				{
				case 0:
					pUDPPacket = (CUDPPacket *) pIPPacket;
					pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
					pUDPPacket->UDPHeader.SetDestPort((pSessionInfo->LowPort + j + MAX_SIMUL_PORT_OFFSET) % MAXWORD + 1);
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
				pSessionInfo->pInterface->Send(*pIPPacket);
			}
			catch(CSpartaException *Ex)
			{
				_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
				delete Ex;
				
			}
			
			// Release the packet
			if(pIPPacket)
			{
				(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
				pIPPacket = NULL;
			}
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragAboveMaxSimulReassemblyServ
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
BOOL IPFragAboveMaxSimulReassemblyServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[16];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr); // tristanjX

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));
	
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
				pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
				pIPPacket->MacHeader.SetProtocolType(0x800);
				pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
				pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
				pIPPacket->IPHeader.SetID(IPID_DEFAULT + j);
				pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
				pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
				pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
				pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
				pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
				pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
			
				switch(i)
				{
				case 0:
					pUDPPacket = (CUDPPacket *) pIPPacket;
					pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
					pUDPPacket->UDPHeader.SetDestPort((pSessionInfo->LowPort + j + ABOVE_MAX_SIMUL_PORT_OFFSET) % MAXWORD + 1);
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
				pSessionInfo->pInterface->Send(*pIPPacket);
			}
			catch(CSpartaException *Ex)
			{
				_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
				delete Ex;
				
			}
			
			// Release the packet
			if(pIPPacket)
			{
				(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
				pIPPacket = NULL;
			}
		}
	}

	return TRUE;
}

//==================================================================================
// IPFragOversizedFragmentsServ
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
BOOL IPFragOversizedFragmentsServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[100];

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr); // tristanjX

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < OVERSIZED_FRAG_COUNT; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
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
// IPAttackTeardropServ
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
BOOL IPAttackTeardropServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[1024];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr);

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 5);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPAttackNewTeardropServ
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
BOOL IPAttackNewTeardropServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[1024];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr);

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 5);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPAttackImpTeardropServ
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
BOOL IPAttackImpTeardropServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[1024];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr);

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 5);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPAttackSynDropServ
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
BOOL IPAttackSynDropServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CTCPPacket *pTCPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[1024];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr);

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CTCPPacket(MediaType) : new CIPPacket(MediaType);

			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 5);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_TCP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);

			switch(i)
			{
			case 0:
				pTCPPacket = (CTCPPacket *) pIPPacket;
				pTCPPacket->TCPHeader.SetSrcPort(pSessionInfo->LowPort);
				pTCPPacket->TCPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CTCPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPAttackBonkServ
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
BOOL IPAttackBonkServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[1024];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);
	
	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr);
	
	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;
	
	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));
	
	for(INT i = 0; i < 2; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pIPPacket = (i == 0) ? new CUDPPacket(MediaType) : new CIPPacket(MediaType);
			
			// Fill in the IP header fields
			pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pIPPacket->MacHeader.SetProtocolType(0x800);
			pIPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pIPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pIPPacket->IPHeader.SetID(IPID_DEFAULT + 5);
			pIPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
			pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
			pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
			
			switch(i)
			{
			case 0:
				pUDPPacket = (CUDPPacket *) pIPPacket;
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pIPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}
		
		// Release the packet
		if(pIPPacket)
		{
			(i == 0) ? delete (CUDPPacket *) pIPPacket : delete pIPPacket;
			pIPPacket = NULL;
		}
	}

	return TRUE;
}

//==================================================================================
// IPAttackZeroLenOptServ
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
BOOL IPAttackZeroLenOptServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	CIPOption *pIPOption = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16], PayloadData[1], *pBuffer = NULL;
	BOOL fSuccess = TRUE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	// Watch for SPARTA exceptions
	try
	{
		// Hard-code the address for now.  We'll work something better out later.
		MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
		CMacAddress ClientMacAddr(MediaType, szClientMacAddr);
		
		// Create the packet
		pUDPPacket = new CUDPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pUDPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pUDPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
		pUDPPacket->MacHeader.SetProtocolType(0x800);
		pUDPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
		pUDPPacket->IPHeader.SetTOS(IPTOS_DEFAULT);
		pUDPPacket->IPHeader.SetID(IPID_DEFAULT);
		pUDPPacket->IPHeader.SetFlag(0);
		pUDPPacket->IPHeader.SetFragOffset(0);
		pUDPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
		pUDPPacket->IPHeader.SetProtocolType(IPPROTOCOL_UDP);
		pUDPPacket->IPHeader.SetDestAddr(szClientIPAddr);
		pUDPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
		
		pUDPPacket->IPHeader.SetAutoCalcDatagramLength(OFF);
		pUDPPacket->IPHeader.SetDatagramLength(IPHEADER_DEFAULT + 8 + UDPHEADER_SIZE + 1);

		pUDPPacket->IPHeader.SetAutoCalcHeaderLength(OFF);
		pUDPPacket->IPHeader.SetHdrLength(IPHEADER_DEFAULT + 8);

		pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_NO_OPERATION);
		pIPOption = pUDPPacket->IPHeader.CreateIPOption(IP_OPTIONS_STRICT_SOURCE_RECORD_ROUTE);
		pIPOption->SetOptionBufferLength(3 + sizeof(DWORD));
		pIPOption->SetOptionLength(0);
		pIPOption->SetOptionOffset(4);
		pIPOption->SetIPAddrAt(4, szClientIPAddr);

		pUDPPacket->IPHeader.SetAutoCalcChecksum(ON);
		
		pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
		pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
		pUDPPacket->UDPHeader.SetAutoCalcLength(OFF);
		pUDPPacket->UDPHeader.SetLength(9);
		pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
		pUDPPacket->UDPHeader.SetChecksum(0);

		PayloadData[0] = 'x';
		pUDPPacket->SetUserBuffer(PayloadData, 1, TRUE);

		pSessionInfo->pInterface->Send(*pUDPPacket);
	}
	catch(CSpartaException *Ex)
	{
		_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
		delete Ex;
		fSuccess = FALSE;
	}

	pUDPPacket ? delete pUDPPacket : 0;

	return fSuccess;
}

//==================================================================================
// IPAttackNesteaServ
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
BOOL IPAttackNesteaServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	CUDPPacket *pUDPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
	BYTE PayloadBuffer[1024];
	BOOL fUDPPacket = FALSE;

	sprintf(szClientMacAddr, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		pSessionInfo->pCurrentRequest->MacAddr[1],
		pSessionInfo->pCurrentRequest->MacAddr[2],
		pSessionInfo->pCurrentRequest->MacAddr[3],
		pSessionInfo->pCurrentRequest->MacAddr[4],
		pSessionInfo->pCurrentRequest->MacAddr[5],
		pSessionInfo->pCurrentRequest->MacAddr[6],
		pSessionInfo->pCurrentRequest->MacAddr[7],
		pSessionInfo->pCurrentRequest->MacAddr[8],
		pSessionInfo->pCurrentRequest->MacAddr[9],
		pSessionInfo->pCurrentRequest->MacAddr[10],
		pSessionInfo->pCurrentRequest->MacAddr[11],
		pSessionInfo->pCurrentRequest->MacAddr[12]);

	// Hard-code the address for now.  We'll work something better out later.
	MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
	CMacAddress ClientMacAddr(MediaType, szClientMacAddr);

	// Calculate the server address string
	ClientAddr.S_un.S_addr = FromAddr;

	// Calculate the server address string
	strcpy(szClientIPAddr, inet_ntoa((IN_ADDR) ClientAddr));

	for(INT i = 0; i < 3; ++i)
	{
		// Watch for SPARTA exceptions
		try
		{
			// Create and initialize the IP packet
			pUDPPacket = new CUDPPacket(MediaType);

			// Fill in the IP header fields
			pUDPPacket->MacHeader.SetDestAddress(ClientMacAddr);
			pUDPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
			pUDPPacket->MacHeader.SetProtocolType(0x800);
			pUDPPacket->IPHeader.SetVersion(IPVERSION_DEFAULT);
			pUDPPacket->IPHeader.SetTOS(IPTOS_NORMAL);
			pUDPPacket->IPHeader.SetID(IPID_DEFAULT + 5);
			pUDPPacket->IPHeader.SetTTL(IPTTL_DEFAULT);
			pUDPPacket->IPHeader.SetProtocolType(PROTOCOL_TCP);
			pUDPPacket->IPHeader.SetDestAddr(szClientIPAddr);
			pUDPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
			pUDPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
			pUDPPacket->IPHeader.SetAutoCalcChecksum(ON);

			switch(i)
			{
			case 0:
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
				pUDPPacket->UDPHeader.SetSrcPort(pSessionInfo->LowPort);
				pUDPPacket->UDPHeader.SetDestPort(pSessionInfo->LowPort);
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
			pSessionInfo->pInterface->Send(*pUDPPacket);
		}
		catch(CSpartaException *Ex)
		{
			_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
			delete Ex;
			
		}

		// Release the packet
		pUDPPacket ? delete pUDPPacket : 0;
		pUDPPacket = NULL;
	}

	return TRUE;
}
