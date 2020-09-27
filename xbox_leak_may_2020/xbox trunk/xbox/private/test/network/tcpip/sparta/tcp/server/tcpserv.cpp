#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "tcpserv.h"

BYTE g_bLocalSpoofIpNextHost = LOCAL_SPOOF_IP_BASE_HOST;

PTCP_SERV_FUNCTION	g_ServerFunctions[TCPMSG_MAXTEST] = 
{
	TCPSeqNumMaxServ,													// TCPMSG_SEQNUM_MAX,
	TCPSeqNumMinServ,													// TCPMSG_SEQNUM_MIN,
	TCPDataOffBelowMinServ,												// TCPMSG_DATAOFF_BELOWMIN,
	TCPDataOffAboveAvailServ											// TCPMSG_DATAOFF_ABOVEAVAIL,
#if 0
	TCPReservedSetServ,													// TCPMSG_RESERVED_SET,
	TCPWindowMinServ,													// TCPMSG_WINDOW_MIN,
	TCPWindowMaxServ,													// TCPMSG_WINDOW_MAX,
	TCPChecksumBadServ,													// TCPMSG_CHECKSUM_BAD,
	TCPUrgentAfterEndServ,												// TCPMSG_URGENT_AFTEREND,
	TCPUrgentAtEndServ,													// TCPMSG_URGENT_ATEND,
	TCPUrgentBeforeEndServ,												// TCPMSG_URGENT_BEFOREEND,
	TCPUrgentMaxServ,													// TCPMSG_URGENT_MAX,
	TCPUrgentNoFlagServ,												// TCPMSG_URGENT_NOFLAG,
	TCPOptCutoffServ,													// TCPMSG_OPT_CUTOFF,
	TCPOptAtEndServ,													// TCPMSG_OPT_ATEND,
	TCPOptBeforeEndServ,												// TCPMSG_OPT_BEFOREEND,
	TCPOptNoOptsServ,													// TCPMSG_OPT_NOOPTIONS,
	TCPOptMaxOptsServ,													// TCPMSG_OPT_MAXOPTIONS,
	TCPOptMaxOptLenServ,												// TCPMSG_OPT_MAXOPTLEN,
	TCPOptEndOfOptsServ,												// TCPMSG_OPT_ENDOPTFIRST,
	TCPOptMssZeroServ,													// TCPMSG_OPT_MSSZERO,
	TCPOptMssOneServ,													// TCPMSG_OPT_MSSONE,
	TCPOptMssMaxServ,													// TCPMSG_OPT_MSSMAX,
	TCPConnSynAndNoAckServ,												// TCPMSG_CONN_SYNANDNOACK,
	TCPConnExtraSynServ,												// TCPMSG_CONN_EXTRASYN,
	TCPConnAckWrongSynServ,												// TCPMSG_CONN_ACKWRONGSYN,
	TCPConnSynAckWrongSynServ,											// TCPMSG_CONN_SYNACKWRONGSYN,
	TCPConnAckNoConnServ,												// TCPMSG_CONN_ACKNOCONN,
	TCPConnSynAckListenerServ,											// TCPMSG_CONN_SYNACKLISTENER,
	TCPShutFinAndNoAckServ,												// TCPMSG_SHUT_FINANDNOACK
#endif
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
// TcpServerCallbackFunction
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
BOOL TcpServerCallbackFunction(HANDLE hSessionObject, HANDLE hNetsyncObject, DWORD FromAddr, DWORD dwMessageType, DWORD dwMessageSize, CHAR *ReceiveMessage, LPVOID lpContext)
{
	PSESSION_INFO pSessionInfo = (PSESSION_INFO) lpContext;
	PTCP_TESTREQ pTestRequest = (PTCP_TESTREQ) ReceiveMessage;
	BOOL fSuccess = TRUE;

	switch(pTestRequest->dwMessageId)
	{
	case TCPMSG_TESTREQ:
		// Client is requesting a test.
		// If we support that test, ACK it and run the server side of the case
		if(pTestRequest->TestId < TCPMSG_MAXTEST)
		{
			pSessionInfo->pCurrentRequest = pTestRequest;

			// Either wrap around at 254 or increment the current host
			(g_bLocalSpoofIpNextHost == 254) ? g_bLocalSpoofIpNextHost = LOCAL_SPOOF_IP_BASE_HOST : ++g_bLocalSpoofIpNextHost;

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
	case TCPMSG_TESTDONE:
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
// TcpServerStartFunction
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
LPVOID TcpServerStartFunction(HANDLE hSessionObject, HANDLE hNetsyncObject, BYTE byClientCount, DWORD *ClientAddrs, WORD LowPort, WORD HighPort)
{
	PSESSION_INFO pCurrentSession = NULL;
	IN_ADDR INAddr;
	TCHAR *szLocalMacAddress = NULL;
	CHAR szClientIPAddr[16];

	pCurrentSession = (PSESSION_INFO) LocalAlloc(LPTR, sizeof(SESSION_INFO));
	_tprintf(TEXT("Starting session: 0x%08x\n"), pCurrentSession);

	// Fill the session info structure
	pCurrentSession->hNetsyncObject = hNetsyncObject;
	pCurrentSession->byClientCount = byClientCount;
	pCurrentSession->ClientAddrs = ClientAddrs;
	pCurrentSession->LowPort = LowPort;
	pCurrentSession->HighPort = HighPort;


	// Calculate the spoofed server address string
	sprintf(pCurrentSession->szServerIPAddr, "%s%u", LOCAL_SPOOF_IP_BASE_STRING, g_bLocalSpoofIpNextHost);
	++g_bLocalSpoofIpNextHost;

//	INAddr.S_un.S_addr = GetMyAddr();
//	strcpy(pCurrentSession->szServerIPAddr, inet_ntoa(INAddr));

	// Calculate the server address string
	INAddr.S_un.S_addr = ClientAddrs[0];
	strcpy(pCurrentSession->szClientIPAddr, inet_ntoa(INAddr));

	// Initialize the SPARTA variables for this session
	try
    {
		BYTE bProtocol = 0x06;

		pCurrentSession->pInterfaceList = new CInterfaceList;
		pCurrentSession->pInterface = new CInterface(pCurrentSession->pInterfaceList->GetNameAt(0));

		szLocalMacAddress = (pCurrentSession->pInterface->GetMediaAddress())->GetString();

		pCurrentSession->pAutoArp = new CAutoArp(szLocalMacAddress, pCurrentSession->szServerIPAddr);

		MAC_MEDIA_TYPE MediaType = pCurrentSession->pInterface->GetMediaType();

		// Create the receive patterns to filter for during the TCP tests
		pCurrentSession->pInterface->EnableDirectedReceiveMode();
		pCurrentSession->pInterface->EnableBroadcastReceiveMode();
		pCurrentSession->pProtocolPattern = CMacHeader::CreateProtocolTypePattern(MediaType, 0x800);
		pCurrentSession->pDestAddrPattern = CMacHeader::CreateDestAddressPattern(MediaType, pCurrentSession->pInterface->GetMediaAddress());
		pCurrentSession->pSrcIpPattern = CIPHeader::CreateSrcIPAddressPattern(MediaType, pCurrentSession->szClientIPAddr);
		pCurrentSession->pDestIpPattern = CIPHeader::CreateDestIPAddressPattern(MediaType, pCurrentSession->szServerIPAddr);
		pCurrentSession->pProtocolIpPattern = new CPattern(23, 1, &bProtocol);

		// Add the pattern filters
		pCurrentSession->pInterface->AddPatternFilter(pCurrentSession->pProtocolPattern);
		pCurrentSession->pInterface->AddPatternFilter(pCurrentSession->pDestAddrPattern);
		pCurrentSession->pInterface->AddPatternFilter(pCurrentSession->pSrcIpPattern);
		pCurrentSession->pInterface->AddPatternFilter(pCurrentSession->pDestIpPattern);
		pCurrentSession->pInterface->AddPatternFilter(pCurrentSession->pProtocolIpPattern);

		pCurrentSession->pInterface->EnablePatternFiltering();
		pCurrentSession->pInterface->StartListening();
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
// TcpServerStopFunction
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
VOID TcpServerStopFunction(HANDLE hSessionObject, HANDLE hNetsyncObject, LPVOID lpContext)
{
	PSESSION_INFO pCurrentSession = (PSESSION_INFO) lpContext;

	_tprintf(TEXT("Stopping session: 0x%08x\n"), pCurrentSession);

	// Stop auto arping
	pCurrentSession->pAutoArp ? delete pCurrentSession->pAutoArp : 0;

	// Stop listening to the interface
    pCurrentSession->pInterface->StopListening();

	// Release the SPARTA pattern variables
	pCurrentSession->pProtocolPattern ? delete pCurrentSession->pProtocolPattern : 0;
    pCurrentSession->pDestAddrPattern ? delete pCurrentSession->pDestAddrPattern : 0;
    pCurrentSession->pSrcIpPattern ? delete pCurrentSession->pSrcIpPattern : 0;
    pCurrentSession->pDestIpPattern ? delete pCurrentSession->pDestIpPattern : 0;
    pCurrentSession->pProtocolIpPattern ? delete pCurrentSession->pProtocolIpPattern : 0;

	// Stop doing pattern filtering
    pCurrentSession->pInterface->DisablePatternFiltering();
	pCurrentSession->pInterface->StopListening();

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
// ReceiveTCPPacket
//----------------------------------------------------------------------------------
//
// Description: Function that receives a TCP packet
//
// Arguments:
//	PSESSION_INFO	pSessionInfo	Pointer to information on this test session
//	BYTE			bFlagValue		Indicates the flags that must be present
//	INT				*pnAckNum		Ack number to be used in next packet sent out
//	INT				*pnSeqNum		Seq number to be used in next packet sent out
//
// Returns:
//	Pointer to a TCP packet if successful, NULL otherwise
//==================================================================================
CTCPPacket* ReceiveTCPPacket(PSESSION_INFO pSessionInfo, BYTE bFlagValue, INT *pnAckNum, INT *pnSeqNum)
{
    CTCPPacket *pNewPacket;

    try
    {
		// Set the timer for receiving a packet to 5 seconds
        CReceiveTimer Timer;
        Timer.Start(3000); 
		
		// Receive next TCP packet
		pNewPacket = (CTCPPacket*) pSessionInfo->pInterface->Receive(Timer);
		if (!pNewPacket)
			goto Exit;

		if ((pNewPacket->TCPHeader.GetFlag() & bFlagValue) == bFlagValue)
		{
			USHORT usLength;
			usLength = pNewPacket->IPHeader.GetDatagramLength() -                    
				pNewPacket->IPHeader.GetHdrLength() * 4 -
				pNewPacket->TCPHeader.GetHdrLength() * 4;

			*pnAckNum = pNewPacket->TCPHeader.GetSeqNumber() + usLength;
			*pnSeqNum = pNewPacket->TCPHeader.GetAckNumber();
			goto Exit;
		}

		// Packet didn't meet the flag requirements so release it
		delete pNewPacket;
		pNewPacket = NULL;
    }
    catch(CSpartaException *Ex)
    {
        DEBUGPRINT(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
		delete pNewPacket;
		pNewPacket = NULL;
        delete Ex;
    }

Exit:

    return pNewPacket;
}

//==================================================================================
// SendTCPPacket
//----------------------------------------------------------------------------------
//
// Description: Function that sends a TCP packet
//
// Arguments:
//	PSESSION_INFO	pSessionInfo	Pointer to information on this test session
//	DWORD			FromAddr		IP address of client that requested test
//	INT				nSeqNum			Seq num to be used for this packet
//	INT				nAckNum			Ack num to be used for this packet
//	WORD			HeaderLength	Header length of outgoing TCP packet
//	BYTE			bFlag			Flags to be used for this packet
//	BOOL			fCalcHdrLen		Determines whether header length should be determined automatically
//
// Returns:
//	TRUE if successful, FALSE otherwise
//==================================================================================
BOOL SendTCPPacket(PSESSION_INFO pSessionInfo, DWORD FromAddr, INT nSeqNum, INT nAckNum, WORD HeaderLength, BYTE bFlag, BOOL fCalcHdrLen)
{
	IN_ADDR ClientAddr;
	CHAR szClientMacAddr[18], szClientIPAddr[16];
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
		// Create a TCP packet
		MAC_MEDIA_TYPE MediaType = pSessionInfo->pInterface->GetMediaType();
        CTCPPacket TCPPacket(MediaType);
    
		// Create Mac addresses for the source and destination
        CMacAddress ClientMacAddr(MediaType, szClientMacAddr); 
    
        TCPPacket.MacHeader.SetDestAddress(ClientMacAddr);
        TCPPacket.MacHeader.SetSrcAddress(*pSessionInfo->pInterface->GetMediaAddress());
        TCPPacket.MacHeader.SetProtocolType(PROTOCOL_TYPE_IP4);
    
        TCPPacket.IPHeader.SetVersion(4);
        TCPPacket.IPHeader.SetTOS(0);
        TCPPacket.IPHeader.SetID(1);
        TCPPacket.IPHeader.SetTTL(1);
        TCPPacket.IPHeader.SetFragOffset(0);
        TCPPacket.IPHeader.SetFlag(0);
        TCPPacket.IPHeader.SetProtocolType(PROTOCOL_TYPE_TCP);
        TCPPacket.IPHeader.SetDestAddr(szClientIPAddr);
        TCPPacket.IPHeader.SetSrcAddr(pSessionInfo->szServerIPAddr);
    
        TCPPacket.TCPHeader.SetSrcPort(pSessionInfo->LowPort);
        TCPPacket.TCPHeader.SetDestPort(pSessionInfo->LowPort);
        TCPPacket.TCPHeader.SetSeqNumber(nSeqNum);
        TCPPacket.TCPHeader.SetAckNumber(nAckNum);
        TCPPacket.TCPHeader.SetFlag(bFlag);
		TCPPacket.TCPHeader.SetWindowSize(16384);

		if(!fCalcHdrLen)
		{
			TCPPacket.TCPHeader.SetAutoCalcLength(OFF);
			TCPPacket.TCPHeader.SetHdrLength(HeaderLength);

			// If SYN flag is set, set the RemoteMSS option
			if (bFlag & TCP_SYN)
				TCPPacket.TCPHeader.Options[0].AddMSS(MSS_DEFAULT);

		}
		else
		{
			// If SYN flag is set, set the RemoteMSS option
			if (bFlag & TCP_SYN)
				TCPPacket.TCPHeader.Options[0].AddMSS(MSS_DEFAULT);
		}

        pSessionInfo->pInterface->Send(TCPPacket);
    }
    catch(CSpartaException *Ex)
    {
        DEBUGPRINT(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
		fSuccess = FALSE;
        delete Ex;
    }

    return fSuccess;
}

//==================================================================================
// EstablishTCPConnection
//----------------------------------------------------------------------------------
//
// Description: Function that uses SPARTA APIs to establish a TCP connection with
// a remote machine
//
// Arguments:
//	PSESSION_INFO	pSessionInfo	Pointer to information on this test session
//	DWORD			FromAddr		IP address of client that requested test
//	INT				*pnAckNum		Ack number to be used in next packet sent out
//	INT				*pnSeqNum		Seq number to be used in next packet sent out
//	WORD			HeaderLength	Header length to use for initial SYN, if 0, then automatically generate
//
// Returns:
//	TRUE on successful connection, FALSE otherwise
//==================================================================================
BOOL EstablishTCPConnection(PSESSION_INFO pSessionInfo, DWORD FromAddr, INT *pnAckNum, INT *pnSeqNum, WORD HeaderLength)
{
	INT nInitialAckNum = *pnAckNum, nInitialSeqNum = *pnSeqNum;
    BOOL fPassed = TRUE;
    CTCPPacket* pPacket = NULL;

    // Send a syn-ack
    if (!SendTCPPacket(pSessionInfo, FromAddr, *pnSeqNum, *pnAckNum, HeaderLength, TCP_SYN, (HeaderLength == 0)))
	{
        _tprintf(TEXT("Couldn't send SYN to start connection\n"));
		fPassed = FALSE;
        goto Exit;
    }

    // Grab the syn packet
    if (!(pPacket = ReceiveTCPPacket(pSessionInfo, TCP_SYN | TCP_ACK, pnAckNum, pnSeqNum)))
	{
        _tprintf(TEXT("Didn't receive SYN-ACK in response to SYN\n"));
        fPassed = FALSE;
		goto Exit;
    }

	if(*pnSeqNum != nInitialSeqNum + 1)
	{
        _tprintf(TEXT("Received SYN-ACK's ack number: %u, expected %u\n"), (DWORD) *pnSeqNum, nInitialSeqNum + 1);
        fPassed = FALSE;
		goto Exit;
	}

	// Delete packet after processing
    delete pPacket;
	pPacket = NULL;

    // Send a syn-ack
    if (!SendTCPPacket(pSessionInfo, FromAddr, *pnSeqNum, ++(*pnAckNum), 0, TCP_ACK, TRUE))
	{
        _tprintf("Couldn't send ACK to finish connection establishment\n");
		fPassed = FALSE;
        return 1;
    }    

Exit:

    pPacket ? delete pPacket : 0;

    return fPassed;
}

//==================================================================================
// TCPSeqNumMaxServ
//----------------------------------------------------------------------------------
//
// Description: Server sends a TCP SYN with the maximum sequence number.  Then
// ack's the SYN-ACK received from the client side of the test.
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL TCPSeqNumMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	INT nAckNum = 0, nSeqNum = -1;

	return EstablishTCPConnection(pSessionInfo, FromAddr, &nAckNum, &nSeqNum, 0);
}

//==================================================================================
// TCPSeqNumMinServ
//----------------------------------------------------------------------------------
//
// Description: Server sends a TCP SYN with the minimum sequence number.  Then
// ack's the SYN-ACK received from the client side of the test.
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL TCPSeqNumMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	INT nAckNum = 0, nSeqNum = 0;

	return EstablishTCPConnection(pSessionInfo, FromAddr, &nAckNum, &nSeqNum, 0);
}

//==================================================================================
// TCPDataOffBelowMinServ
//----------------------------------------------------------------------------------
//
// Description: Sends a TCP SYN packet with a data offset that is lower than the
// minimum header length of a TCP packet
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL TCPDataOffBelowMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	INT nAckNum = 0, nSeqNum = 0;

	return SendTCPPacket(pSessionInfo, FromAddr, nSeqNum, nAckNum, 16, TCP_SYN, FALSE);
}

//==================================================================================
// TCPDataOffAboveAvailServ
//----------------------------------------------------------------------------------
//
// Description: Sends a TCP SYN packet with a data offset that is the maximum value
//
// Arguments:
//	DWORD			FromAddr		Address of user that requested the test
//	PSESSION_INFO	pSessionInfo	Context indicating which session this belongs to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL TCPDataOffAboveAvailServ (DWORD FromAddr, PSESSION_INFO pSessionInfo)
{
	INT nAckNum = 0, nSeqNum = 0;

	return SendTCPPacket(pSessionInfo, FromAddr, nSeqNum, nAckNum, 60, TCP_SYN, FALSE);
}
