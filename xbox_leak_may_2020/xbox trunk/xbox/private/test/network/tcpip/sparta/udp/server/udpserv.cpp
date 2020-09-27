#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "udpserv.h"
#include "myaddr.h"


PUDP_SERV_FUNCTION	g_ServerFunctions[UDPMSG_MAXTEST] = 
{
	UDPSourceMaxServ,			// UDPMSG_SOURCEPORT_MAX
	UDPSourceMinServ,			// UDPMSG_SOURCEPORT_MIN
	UDPSourceZeroServ,			// UDPMSG_SOURCEPORT_ZERO
	UDPDestMaxServ,				// UDPMSG_DESTPORT_MAX
	UDPDestMinServ,				// UDPMSG_DESTPORT_MIN
	UDPDestZeroServ,			// UDPMSG_DESTPORT_ZERO
	UDPLenBelowMinServ,			// UDPMSG_LENGTH_BELOWMIN
	UDPLenAtMinServ,			// UDPMSG_LENGTH_ATMIN
	UDPLenAboveMinServ,			// UDPMSG_LENGTH_ABOVEMIN
	UDPLenAboveTotalServ,		// UDPMSG_LENGTH_ABOVETOTAL
	UDPLenBelowTotalServ,		// UDPMSG_LENGTH_BELOWTOTAL
	UDPLenMaxServ,				// UDPMSG_LENGTH_MAXIMUM
	UDPCheckZerosServ			// UDPMSG_CHECKSUM_ZEROSOK
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
// UdpServerCallbackFunction
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
BOOL UdpServerCallbackFunction(HANDLE hSessionObject, HANDLE hNetsyncObject, DWORD FromAddr, DWORD dwMessageType, DWORD dwMessageSize, CHAR *ReceiveMessage, LPVOID lpContext)
{
	PSESSION_INFO pSessionInfo = (PSESSION_INFO) lpContext;
	PUDP_TESTREQ pTestRequest = (PUDP_TESTREQ) ReceiveMessage;
	BOOL fSuccess = TRUE;

	switch(pTestRequest->dwMessageId)
	{
	case UDPMSG_TESTREQ:
		// Client is requesting a test.
		// If we support that test, ACK it and run the server side of the case
		if(pTestRequest->TestId < UDPMSG_MAXTEST)
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
	case UDPMSG_TESTDONE:
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
// UdpServerStartFunction
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
LPVOID UdpServerStartFunction(HANDLE hSessionObject, HANDLE hNetsyncObject, BYTE byClientCount, DWORD *ClientAddrs, WORD LowPort, WORD HighPort)
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
	pCurrentSession->pCurrentRequest = NULL;

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
// UdpServerStopFunction
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
VOID UdpServerStopFunction(HANDLE hSessionObject, HANDLE hNetsyncObject, LPVOID lpContext)
{
	PSESSION_INFO pCurrentSession = (PSESSION_INFO) lpContext;

	_tprintf(TEXT("Stopping session: 0x%08x\n"), pCurrentSession);

	// Uninitialize the SPARTA variables for this session
	pCurrentSession->pInterfaceList ? delete pCurrentSession->pInterfaceList : 0;
	pCurrentSession->pInterface ? delete pCurrentSession->pInterface : 0;
//	pCurrentSession->pAutoArp ? delete pCurrentSession->pAutoArp : 0;

	LocalFree(pCurrentSession);

	// If anything needs to be cleaned up, it will be done here.
	return;
}


//==================================================================================
// Test functions
//==================================================================================

//==================================================================================
// SendUDPPacket
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with the specified parameters
//
// Arguments:
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//	DWORD			FromAddr		Address of user that requested the test
//	WORD			SourcePort		Value for the source port field of the datagram
//	WORD			DestPort		Value for the dest port field of the datagram
//	WORD			Length			Value for the length field of the datagram
//	BOOL			fCalcLength		Bool indicating whether to use Length or auto generate this field		
//	WORD			Checksum		Value for the checksum field of the datagram
//	BOOL			fCalcChecksum	Bool indicating whether to use Checksum or auto generate this field
//	BOOL			fChecksumFix	Bool indicating if the packet should be doctored so that the supplied checksum works
//	DWORD			PayloadSize		Total number of bytes in the IP packet payload
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL SendUDPPacket(PSESSION_INFO pSessionInfo, DWORD FromAddr, WORD SourcePort, WORD DestPort, WORD Length, BOOL fCalcLength, WORD Checksum, BOOL fCalcChecksum, BOOL fChecksumFix, DWORD PayloadSize)
{
	CIPPacket *pIPPacket = NULL;
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16], *pBuffer = NULL;
	BOOL fSuccess = TRUE, fUDPPacket = FALSE;

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
		if(PayloadSize >= UDPHEADER_SIZE)
		{
			fUDPPacket = TRUE;
			pIPPacket = new CUDPPacket(MediaType);
		}
		else
			pIPPacket = new CIPPacket(MediaType);
		
		// Fill in the IP information from the input parameters
		pIPPacket->MacHeader.SetDestAddress(ClientMacAddr);
		pIPPacket->MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
		pIPPacket->MacHeader.SetProtocolType(0x800);
		pIPPacket->IPHeader.SetVersion(4);
		pIPPacket->IPHeader.SetTOS(0);
		pIPPacket->IPHeader.SetID(1);
		pIPPacket->IPHeader.SetFlag(0);
		pIPPacket->IPHeader.SetFragOffset(0);
		pIPPacket->IPHeader.SetTTL(64);
		pIPPacket->IPHeader.SetProtocolType(PROTOCOL_UDP);
		pIPPacket->IPHeader.SetDestAddr(szClientIPAddr);
		pIPPacket->IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
		pIPPacket->IPHeader.SetAutoCalcHeaderLength(ON);
		pIPPacket->IPHeader.SetAutoCalcChecksum(ON);
				
		// If we are dealing with a packet large enough for the UDP header
		if(fUDPPacket)
		{
			CUDPPacket *pUDPPacket = (CUDPPacket *) pIPPacket;
			
			pUDPPacket->UDPHeader.SetSrcPort(SourcePort);
			pUDPPacket->UDPHeader.SetDestPort(DestPort);

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
				pUDPPacket->UDPHeader.SetLength(Length);
				pUDPPacket->IPHeader.SetDatagramLength(Length + IPHEADER_SIZE);
			}
			
			// Set up the checksum field
			if(fCalcChecksum)
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(ON);
			else
			{
				pUDPPacket->UDPHeader.SetAutoCalcChecksum(OFF);
				pUDPPacket->UDPHeader.SetChecksum(Checksum);
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
		if(PayloadSize > UDPHEADER_SIZE)
		{	
			// Create the buffer for the remaining packet data
			pBuffer = (CHAR *) LocalAlloc(LPTR, PayloadSize - UDPHEADER_SIZE);
			memset(pBuffer, (int) 'x', PayloadSize - UDPHEADER_SIZE);
			
			// Associate that buffer with the packet
			pIPPacket->SetUserBuffer(pBuffer, PayloadSize - UDPHEADER_SIZE, TRUE);				
		}
		else if((PayloadSize > IPHEADER_SIZE) && !fUDPPacket)
		{
			// Create the buffer for the remaining packet data
			pBuffer = (CHAR *) LocalAlloc(LPTR, PayloadSize - IPHEADER_SIZE);
			memset(pBuffer, (int) 'x', PayloadSize - IPHEADER_SIZE);
			
			// Associate that buffer with the packet
			pIPPacket->SetUserBuffer(pBuffer, PayloadSize - IPHEADER_SIZE, TRUE);				
		}
		
		pSessionInfo->pInterface->Send(pIPPacket);
	}
	catch(CSpartaException *Ex)
	{
		_tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
		delete Ex;
		fSuccess = FALSE;
	}

	// Clean up the packet
	if(pIPPacket)
	{
		if(fUDPPacket)
			delete (CUDPPacket *) pIPPacket;
		else
			delete pIPPacket;
	}

	// Release the payload buffer
	pBuffer ? LocalFree(pBuffer) : 0;

	return fSuccess;
}

//==================================================================================
// UDPSourceMaxServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet from the maximum port number
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPSourceMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		MAXWORD, 
		pSessionInfo->LowPort,
		0, 
		TRUE, 
		0, 
		TRUE, 
		FALSE, 
		UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPSourceMinServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet from the minimum port number
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPSourceMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		1, 
		pSessionInfo->LowPort,
		0, 
		TRUE, 
		0, 
		TRUE, 
		FALSE, 
		UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPSourceZeroServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet from port number zero
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPSourceZeroServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		0, 
		pSessionInfo->LowPort,
		0, 
		TRUE, 
		0, 
		TRUE, 
		FALSE, 
		UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPDestMaxServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet to the maximum port number
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPDestMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		pSessionInfo->LowPort,
		MAXWORD - 1,
		0, 
		TRUE, 
		0, 
		TRUE, 
		FALSE, 
		UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPDestMinServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet to the minimum port number
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPDestMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		pSessionInfo->LowPort,
		1,
		0, 
		TRUE, 
		0, 
		TRUE, 
		FALSE, 
		UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPDestZeroServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet to port number zero
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPDestZeroServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		pSessionInfo->LowPort,
		0,
		0, 
		TRUE, 
		0, 
		TRUE, 
		FALSE, 
		UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPLenBelowMinServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a length below the minimum
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenBelowMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		pSessionInfo->LowPort,
		pSessionInfo->LowPort,
		UDPHEADER_SIZE - 1, 
		FALSE, 
		0, 
		TRUE, 
		FALSE, 
		UDPHEADER_SIZE);
}

//==================================================================================
// UDPLenAtMinServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a length of exactly the minimum
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenAtMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		pSessionInfo->LowPort,
		pSessionInfo->LowPort,
		UDPHEADER_SIZE, 
		FALSE,
		0, 
		TRUE, 
		FALSE, 
		UDPHEADER_SIZE);
}

//==================================================================================
// UDPLenAboveMinServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a length above the minimum
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenAboveMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		pSessionInfo->LowPort,
		pSessionInfo->LowPort,
		UDPHEADER_SIZE + 1, 
		FALSE, 
		0,
		TRUE, 
		FALSE, 
		UDPHEADER_SIZE + 1);
}

//==================================================================================
// UDPLenAboveTotalServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a length above total bytes in packet
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenAboveTotalServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		pSessionInfo->LowPort,
		pSessionInfo->LowPort,
		UDPHEADER_SIZE + 1, 
		FALSE, 
		0,
		TRUE, 
		FALSE, 
		UDPHEADER_SIZE);
}

//==================================================================================
// UDPLenBelowTotalServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a length below the total bytes in packet
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenBelowTotalServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		pSessionInfo->LowPort,
		pSessionInfo->LowPort,
		UDPHEADER_SIZE + 1, 
		FALSE, 
		0x0000,
		FALSE,
		FALSE, 
		UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPLenMaxServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a length below the total bytes in packet
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	// TODO - for now, just do maximum ethernet size
	// TODO - need to change the UDP length to 1500 - IPHEADER_SIZE
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		pSessionInfo->LowPort,
		pSessionInfo->LowPort,
		1500 - IPHEADER_SIZE, 
		FALSE, 
		0,
		TRUE, 
		FALSE, 
		1500 - IPHEADER_SIZE);
}

//==================================================================================
// UDPCheckOnesServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a valid checksum of all ones
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPCheckOnesServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		pSessionInfo->LowPort,
		pSessionInfo->LowPort,
		0, 
		TRUE, 
		0xFFFF,
		FALSE, 
		TRUE, 
		UDPHEADER_SIZE + 2);
}

//==================================================================================
// UDPCheckZerosServ
//----------------------------------------------------------------------------------
//
// Description: Test that sends a UDP packet with a checksum of all zeros (indicating not computed)
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPCheckZerosServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	return SendUDPPacket(pSessionInfo,
		FromAddr,
		pSessionInfo->LowPort,
		pSessionInfo->LowPort,
		0, 
		TRUE, 
		0x0000,
		FALSE, 
		FALSE, 
		UDPHEADER_SIZE + 2);
}
