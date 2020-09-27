/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		Receive.c
 *  Content:	This file contains code which receives indications of incoming data
 *				from a ServiceProvider,  cracks the data,  and handles it appropriately.
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *  11/06/98	ejs		Created
 *  09/29/2000	masonb	Fixed Sequence issue with Out-of-order Receives
 *
 ****************************************************************************/

#include "dnproti.h"

#ifdef	DEBUG
BOOL	UsePollDelay = FALSE;
#endif

// local protos

BOOL	CancelFrame(PEPD, BYTE);
VOID	CompleteSends(PEPD);
VOID 	DropReceive(PEPD, PRCD);
HRESULT IndicateReceive(PSPD, PSPIE_DATA);
HRESULT	IndicateConnect(PSPD, PSPIE_CONNECT);
HRESULT	ProcessEnumQuery( PSPD, PSPIE_QUERY );
HRESULT	ProcessQueryResponse( PSPD, PSPIE_QUERYRESPONSE );
VOID	ProcessConnectedResponse(PSPD, PEPD, PCFRAME);
VOID	ProcessConnectRequest(PSPD, PEPD, PCFRAME);
VOID	ProcessDisconnectedFrame(PSPD, PEPD, PCFRAME);
VOID	ProcessEndOfStream(PEPD);
VOID	ProcessListenStatus(PSPD, PSPIE_LISTENSTATUS);
VOID	ProcessConnectAddressInfo(PSPD, PSPIE_CONNECTADDRESSINFO);
VOID	ProcessEnumAddressInfo(PSPD, PSPIE_ENUMADDRESSINFO);
VOID	ProcessListenAddressInfo(PSPD, PSPIE_LISTENADDRESSINFO);
VOID	ProcessSendMask(PEPD, BYTE, ULONG, ULONG);
VOID	ProcessSPDisconnect(PSPD, PSPIE_DISCONNECT);
VOID 	ReceiveInOrderFrame(PEPD, PRCD);
VOID 	ReceiveOutOfOrderFrame(PEPD, PRCD, ULONG);
VOID	RejectFrame(PEPD, PDFRAME);


HRESULT	CrackCommand(PSPD, PEPD, PSPRECEIVEDBUFFER);
HRESULT	CrackDataFrame(PSPD, PEPD, PSPRECEIVEDBUFFER, DWORD);

 /*
**		Indicate Receive
**
**			Service Provider calls this entry when data arrives on the network.
**		We will quickly validate the frame and then figure what to do with it...
**
**			Poll/Response activity should be handled before data is indicated to
**		clients.  We want to measure the network latency up to delivery,  not including
**		delivery.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DNSP_IndicateEvent"

HRESULT WINAPI DNSP_IndicateEvent(IDP8SPCallback *pIDNSP, SP_EVENT_TYPE Opcode, PVOID DataBlock)
{
	PSPD			pSPD = (PSPD) pIDNSP;
	ASSERT_SPD(pSPD);
	
	switch(Opcode)
	{
		case	SPEV_DATA:
			return IndicateReceive(pSPD, (PSPIE_DATA) DataBlock);

		case	SPEV_CONNECT:
			return IndicateConnect(pSPD, (PSPIE_CONNECT) DataBlock);

		case	SPEV_ENUMQUERY:
			return ProcessEnumQuery( pSPD, (PSPIE_QUERY) DataBlock );

		case	SPEV_QUERYRESPONSE:
			return ProcessQueryResponse( pSPD, (PSPIE_QUERYRESPONSE) DataBlock );

		case	SPEV_DISCONNECT:
			ProcessSPDisconnect(pSPD, (PSPIE_DISCONNECT) DataBlock);
			break;

		case	SPEV_LISTENSTATUS:
			ProcessListenStatus(pSPD, (PSPIE_LISTENSTATUS) DataBlock);
			break;

		case	SPEV_LISTENADDRESSINFO:
			ProcessListenAddressInfo(pSPD, (PSPIE_LISTENADDRESSINFO) DataBlock);
			break;

		case	SPEV_CONNECTADDRESSINFO:
			ProcessConnectAddressInfo(pSPD, (PSPIE_CONNECTADDRESSINFO) DataBlock);
			break;

		case	SPEV_ENUMADDRESSINFO:
			ProcessEnumAddressInfo(pSPD, (PSPIE_ENUMADDRESSINFO) DataBlock);
			break;

		//
		// SP passed something unexpected
		//
		default:
			LOGPF(0, "Unknown Event indicated by SP");
			break;
	}

	return DPN_OK;
}


/*
**	Indicate Connect
**
**		This event is indicated for both calling and listening sides.  The
**	calling side will do most of its work when the SP Connect call completes
**	and the listening side will do most of its work when the CONNECT frame
**	gets delivered.  All we do here is allocate the EPD and attach it to the
**	MSD (for calling case)
**
**		Since we have a connect protocol,  there will always be a CONNECT
**	frame following closely on the heels of this indication.  Therefore,
**	there is not a whole lot of stuff that we need to do here.  We will
**	allocate the EndPoint and leave it dormant.
**
**		Synchronization Issue:  We have decided that if an SP Listen command is cancelled,
**	the cancel call will not complete until all ConnectIndications have returned from the
**	protocol.  This means that we are GUARANTEED that the Listen command in the context
**	will be valid throughout this call.  This is important because now we can add a reference
**	to the Listen's MSD here and now and we will know that it wont disappear on us before we
**	do it.  Truth,  however,  is that there will be a race until SP fixes itself to follow
**	this behavior.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "IndicateConnect"

HRESULT	IndicateConnect(PSPD pSPD, PSPIE_CONNECT pConnData)
{
	PEPD	pEPD;
	PMSD	pMSD;

	pMSD = (PMSD) pConnData->pCommandContext;
	ASSERT_MSD(pMSD);

	LOGPF(2, "Parameters: pSPD[%p], pConnData[%p] - pMSD[%p]", pSPD, pConnData, pMSD);

	Lock(&pMSD->CommandLock);
	
	LOCK_MSD(pMSD, "EPD Ref");				// Place a reference on the listen object.  This prevents it from
														// going away during the Connect Protocol,  an assumption which simplifies
														// life extraordinarily.  We will want to ASSERT this fact,  however,  to make
														// sure that SP is playing by our rules.

	ASSERT((pMSD->CommandID == COMMAND_ID_CONNECT)||(pMSD->CommandID == COMMAND_ID_LISTEN));
	
	if(pMSD->ulMsgFlags1 & MFLAGS_ONE_CANCELLED)
	{
		LOGPF(2, "Connect Rejected - Listen is cancelling, returning DPNERR_ABORTED");
		RELEASE_MSD(pMSD, "EPD Ref");
		return DPNERR_ABORTED;
	}

	if((pEPD = NewEndPoint(pSPD, pConnData->hEndpoint)) == NULL)
	{
		LOGPF(0, "Failed to allocate new EPD, returning DPNERR_ABORTED");
		RELEASE_MSD(pMSD, "EPD Ref");
		return DPNERR_ABORTED;	// This error will implicitly DISCONNECT from Endpoint
	}

	pEPD->ulEPFlags |= EPFLAGS_END_POINT_DORMANT;		// Not connecting yet
	pEPD->pCommand = pMSD;								// Associate the context with our data structure

	if(pMSD->CommandID == COMMAND_ID_CONNECT)
	{
		LOGPF(5, "INDICATE CONNECT (CALLING) -- EPD = %p", pEPD);
		pMSD->pEPD = pEPD;
	}
	else
	{
		LOGPF(5, "INDICATE CONNECT (LISTENING) -- EPD = %p", pEPD);
	}

	pConnData->pEndpointContext = pEPD;

	Unlock(&pMSD->CommandLock);

	return DPN_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "RejectConnection"

VOID
RejectConnection(PEPD pEPD)
{
	PMSD	pMSD;
	
	Lock(&pEPD->EPLock);
	if( (pMSD = pEPD->pCommand) != NULL)
	{
		pEPD->pCommand = NULL;
	}
	pEPD->ulEPFlags |= EPFLAGS_TERMINATING;
	
	Unlock(&pEPD->EPLock);

	if(pMSD)
	{
		Lock(&pMSD->CommandLock);
		RELEASE_MSD(pMSD, "EPD Ref");
	}

	KillConnection(pEPD);
}

/*
**		Indicate Receive
**
**		A frame has been delivered by the service provider.  We are Guaranteed to
**	have an active Endpoint in our hash table (or else something is wrong).  I have not
**	decided whether I will respond to POLL bits at this high level or else let
**	each handler repond in its own particular... eh...  idiom.
**
**		Our return value controls whether SP will recycle the receive buffer, or whether
**	we can keep the buffer around until we are ready to indicate it to higher levels
**	later on.   If we return DPN_OK then we are done with the buffer and it will be recycled.
**	If we return DPNERR_PENDING then we may hold on to the buffer until we release them later.
*/


#undef DPF_MODNAME
#define DPF_MODNAME "IndicateReceive"

HRESULT IndicateReceive(PSPD pSPD, PSPIE_DATA pDataBlock)
{
	PEPD 				pEPD = static_cast<PEPD>( pDataBlock->pEndpointContext );
	HRESULT				hr;
	PPacketHeader		pFrame = (PPacketHeader) pDataBlock->pReceivedData->BufferDesc.pBufferData;
	DWORD				tNow = GETTIMESTAMP();

	LOGPF(6, "Parameters: pSPD[%p], pDataBlock[%p] - pEPD[%p]", pSPD, pDataBlock, pEPD);

#ifdef	FRAME_DROPS
	if(pFrame->State == DROP_STATE)
	{
		LOGPF(1, "(%p) DROP FRAME, returning DPN_OK", pEPD);
		return DPN_OK;
	}

#endif
	// BUGBUG: No lock held
	if(pSPD->ulSPFlags & SPFLAGS_TERMINATING)
	{
		LOGPF(1, "(%p) SP is terminating, returning DPN_OK", pEPD);
		return DPN_OK;
	}

	ASSERT_EPD(pEPD);

	if(LOCK_EPD(pEPD, "LOCK (IND RECEIVE)") == 0)
	{
		LOGPF(1, "(%p) Rejecting receive on unreferenced EPD, returning DPN_OK", pEPD);
		return DPN_OK;
	}

	pEPD->tLastPacket = tNow;						// Track last time each guy writes to us

#ifdef DEBUG
		// copy this frame to buffer in EPD so we can look after a break.
		DWORD dwLen = MIN(32, pDataBlock->pReceivedData->BufferDesc.dwBufferSize);
		memcpy(pEPD->LastPacket, pDataBlock->pReceivedData->BufferDesc.pBufferData, dwLen);
#endif

	if(pFrame->bCommand & PACKET_COMMAND_DATA)
	{
		hr = CrackDataFrame(pSPD, pEPD, pDataBlock->pReceivedData, tNow);
	}
	else
	{
		hr = CrackCommand(pSPD, pEPD, pDataBlock->pReceivedData);
	}

	RELEASE_EPD(pEPD, "UNLOCK (IND RCV DONE)"); // SendQLock not already held

	LOGPF(6, "Returning hr[%x]", hr);
	return hr;
}


/*
**		Process Enum Query
**
**		A frame has been delivered by the service provider representing an enumereation
**	query.
*/
#undef DPF_MODNAME
#define DPF_MODNAME "ProcessEnumQuery"

HRESULT ProcessEnumQuery( PSPD pSPD, PSPIE_QUERY pQueryBlock )
{
	MSD		*pMSD;
	PROTOCOL_ENUM_DATA	EnumData;

	pMSD = static_cast<MSD*>( pQueryBlock->pUserContext );

	LOGPF(2, "Parameters: pSPD[%p], pQueryBlock[%p] - pMSD[%p]", pSPD, pQueryBlock, pMSD);

	EnumData.pSenderAddress = pQueryBlock->pAddressSender;
	EnumData.pDeviceAddress = pQueryBlock->pAddressDevice;
	EnumData.ReceivedData.pBufferData = pQueryBlock->pReceivedData->BufferDesc.pBufferData;
	EnumData.ReceivedData.dwBufferSize = pQueryBlock->pReceivedData->BufferDesc.dwBufferSize;
	EnumData.hEnumQuery = pQueryBlock;

	DBG_CASSERT( sizeof( &EnumData ) == sizeof( PBYTE ) );

	LOGPF(3, "Calling Core->IndicateEnumQuery");
	pSPD->pPData->pfVtbl->IndicateEnumQuery(	pSPD->pPData->Parent,
												pMSD->Context,
												pMSD,
												reinterpret_cast<PBYTE>( &EnumData ),
												sizeof( EnumData ));

	return	DPN_OK;
}

/*
**		Process Query Response
**
**		A frame has been delivered by the service provider representing a response to an enum.
*/
#undef DPF_MODNAME
#define DPF_MODNAME "ProcessQueryResponse"

HRESULT ProcessQueryResponse( PSPD pSPD, PSPIE_QUERYRESPONSE pQueryResponseBlock)
{
	MSD		*pMSD;
	PROTOCOL_ENUM_RESPONSE_DATA	EnumResponseData;

	pMSD = static_cast<MSD*>( pQueryResponseBlock->pUserContext );

	LOGPF(2, "Parameters: pSPD[%p], pQueryResponseBlock[%p] - pMSD[%p]", pSPD, pQueryResponseBlock, pMSD);

	EnumResponseData.pSenderAddress = pQueryResponseBlock->pAddressSender;
	EnumResponseData.pDeviceAddress = pQueryResponseBlock->pAddressDevice;
	EnumResponseData.ReceivedData.pBufferData = pQueryResponseBlock->pReceivedData->BufferDesc.pBufferData;
	EnumResponseData.ReceivedData.dwBufferSize = pQueryResponseBlock->pReceivedData->BufferDesc.dwBufferSize;
	EnumResponseData.dwRoundTripTime = pQueryResponseBlock->dwRoundTripTime;

	DBG_CASSERT( sizeof( &EnumResponseData ) == sizeof( PBYTE ) );
	
	LOGPF(3, "Calling Core->IndicateEnumResponse");
	pSPD->pPData->pfVtbl->IndicateEnumResponse(	pSPD->pPData->Parent,
												pMSD,
												pMSD->Context,
												reinterpret_cast<PBYTE>( &EnumResponseData ),
												sizeof( EnumResponseData ));

	return	DPN_OK;
}

/*
**		Crack Command
**
**			This frame is a maintainance frame containing no user data
**
*/

#undef DPF_MODNAME
#define DPF_MODNAME "CrackCommand"

HRESULT CrackCommand(PSPD pSPD, PEPD pEPD, PSPRECEIVEDBUFFER pRcvBuffer)
{
	DWORD 		dwDataLength = pRcvBuffer->BufferDesc.dwBufferSize;
	ULONG		*array_ptr;
	ULONG		mask1, mask2;
	
	union {
		PCFRAME			pCFrame;
		PSFBIG			pSack;
		}		pData;

	pData.pCFrame = (PCFRAME) pRcvBuffer->BufferDesc.pBufferData;

	// Drop short frames (should not happen)
	if(dwDataLength < MIN(sizeof(CFRAME), sizeof(SACKFRAME)))
	{		
		LOGPF(0, "(%p) Dropping short frame", pEPD);
		return DPN_OK;
	}

	switch(pData.pCFrame->bExtOpcode)
	{
		case FRAME_EXOPCODE_SACK:
			LOGPF(7, "(%p) SACK Frame Received", pEPD);

			if(pEPD->ulEPFlags & EPFLAGS_END_POINT_DORMANT)
			{
				LOGPF(0, "(%p) Endpoint is dormant, rejecting connection", pEPD);
				RejectConnection(pEPD);
				break;
			}
			if( pData.pSack->bFlags & SACK_FLAGS_RESPONSE )
			{
				LOGPF(7, "(%p) ACK RESP RCVD: Rty=%d,N(R)=0x%x", pEPD, pData.pSack->bRetry, pData.pSack->bNRcv);
			}
			if(pData.pSack->bFlags & (SACK_FLAGS_SACK_MASK1 | SACK_FLAGS_SACK_MASK2 | SACK_FLAGS_SEND_MASK1 | SACK_FLAGS_SEND_MASK2))
			{
				LOGPF(7, "(%p) *MASKS IN SACK* flags=%x", pEPD, pData.pSack->bFlags);
			}

			array_ptr = pData.pSack->rgMask;
			mask1 = pData.pSack->bFlags & SACK_FLAGS_SACK_MASK1 ? *array_ptr++ : 0;
			mask2 = pData.pSack->bFlags & SACK_FLAGS_SACK_MASK2 ? *array_ptr++ : 0;
			
			Lock(&pEPD->EPLock);
			
			UpdateXmitState(pEPD, pData.pSack->bNRcv, (PSACKFRAME) pData.pSack, mask1, mask2, GETTIMESTAMP());

			mask1 = pData.pSack->bFlags & SACK_FLAGS_SEND_MASK1 ? *array_ptr++ : 0;
			mask2 = pData.pSack->bFlags & SACK_FLAGS_SEND_MASK2 ? *array_ptr++ : 0;

			if(mask1 | mask2)
			{
				ProcessSendMask(pEPD, pData.pSack->bNSeq, mask1, mask2);

				if( (!pEPD->blCompleteList.IsEmpty()) && ((pEPD->ulEPFlags & EPFLAGS_IN_RECEIVE_COMPLETE) == FALSE))
				{
					pEPD->ulEPFlags |= EPFLAGS_IN_RECEIVE_COMPLETE;	// ReceiveComplete will clear this flag when done
					ReceiveComplete(pEPD); 							// Deliver the goods,  returns with EPLock released
				}
				else 
				{
					Unlock(&pEPD->EPLock);
				}
			}
			else 
			{
				Unlock(&pEPD->EPLock);
			}
			
			if(pEPD->ulEPFlags & EPFLAGS_COMPLETE_SENDS)
			{
				CompleteSends(pEPD);
			}
			break;

		case FRAME_EXOPCODE_CONNECT:
			LOGPF(7, "(%p) CONNECT Frame Received", pEPD);
			ProcessConnectRequest(pSPD, pEPD, pData.pCFrame);
			break;

		case FRAME_EXOPCODE_CONNECTED:
			LOGPF(7, "(%p) CONNECTED Frame Received", pEPD);
			ProcessConnectedResponse(pSPD, pEPD, pData.pCFrame);
			break;

		case FRAME_EXOPCODE_DISCONNECTED:
			LOGPF(7, "(%p) DISCONNECTED Frame Received", pEPD);
			ProcessDisconnectedFrame(pSPD, pEPD, pData.pCFrame);
			break;

		default:
			LOGPF(1, "(%p) RECEIVED INVALID FRAME, Data: %x, rejecting connection", pEPD, *((DWORD*)pRcvBuffer->BufferDesc.pBufferData));
			RejectConnection(pEPD);
			break;
	}

	return DPN_OK;
}

/*
**		Crack Data Frame
**
**			In addition to delivering data contained in the frame,  we also must
**	use the included state info to drive the transmission process.  We will update
**	our link state according to this info and see if we need to put this session
**	back into the sending pipeline.
**
**		Of course, data will only be delivered if we have completed an entire message.
**
**	CRITICAL SECTION NOTE -- It might seem rather lavish the way we hold the EPD->StateLock
**		thru this entire routine,  but anything less would require an obscene level of complexity
**		to keep ironed out.  This is why I defer all ReceiveIndications and buffer mappings until
**		the end of the routine when the Lock can be released.
**
*/

#undef DPF_MODNAME
#define DPF_MODNAME "CrackDataFrame"

HRESULT CrackDataFrame(PSPD pSPD, PEPD pEPD, PSPRECEIVEDBUFFER pRcvBuffer, DWORD tNow)
{
	LPVOID 	pData = pRcvBuffer->BufferDesc.pBufferData;
	DWORD 	dwDataLength = pRcvBuffer->BufferDesc.dwBufferSize;
	PDFBIG	pFrame = (PDFBIG) pData;
	PRCD	pRCD;
	PRCD	pThisRCD;
	CBilink	*pLink;
	BYTE	bSeq = pFrame->bSeq;
	BOOL	flag;
	ULONG	bit;
	UINT	count;
	ULONG	*array_ptr;
	ULONG	MaskArray[4];
	UINT	header_size;
	ULONG	mask;


	LOGPF(7, "(%p) Data Frame Arrives Seq=%x; NRcv=%x", pEPD, bSeq, pFrame->bNRcv);

	// Determine how large the variable length header is
	mask = (pFrame->bControl & PACKET_CONTROL_VARIABLE_MASKS) / PACKET_CONTROL_SACK_MASK1;
	
	Lock(&pEPD->EPLock);
	
	//	Data on an unconnected link
	//
	//	There are two possibilities (as I see it today).  Either we have dropped our link because partner
	//	went silent,  but now he has started sending again.  OR We have disconnected and are now reconnecting
	//  but there are some old data frames bouncing around (less likely).
	//
	//	If we have dropped and partner is just figuring it out,  we must kill the endpoint or else it will hang
	//	around forever after partner stops bothering us.  We can help out partner by sending him a DISC frame
	//  so he knows that we arent playing anymore,  buts its not technically necessary.
	//
	//	In the second case,  we do not want to close the EP because that will crush the session startup that
	//	is supposedly in progress.  Therefore,  if we are not in a DORMANT state,  then we know a session
	//	startup is in progress,  and we will let the EP remain open.

	if(!(pEPD->ulEPFlags & EPFLAGS_CONNECTED))
	{				
		// CODEWORK - send some sort of rejection frame...
		LOGPF(0, "Received data on unconnected link");
		if(pEPD->ulEPFlags & EPFLAGS_END_POINT_DORMANT)
		{
			Unlock(&pEPD->EPLock);
			LOGPF(0, "Rejecting connection on dormant link that received data");
			RejectConnection(pEPD);
		}
		else
		{
			Unlock(&pEPD->EPLock);
		}
		return DPN_OK;										// do not accept data before we have connected
	}

	// Make sure that new frame is within our receive window
	if((BYTE)(bSeq - pEPD->bNextReceive) >= (BYTE) MAX_FRAME_OFFSET)
	{	
		LOGPF(1, "(%p) Rejecting frame that is out of receive window SeqN=%x", pEPD, bSeq);
		RejectFrame(pEPD, (PDFRAME) pFrame);		// This unlocks the EPLock
		return DPN_OK;								// REJECT Frame
	}

	if(mask)
	{
		array_ptr = pFrame->rgMask;
		for(count = 0; count < 4; count++, mask >>= 1)
		{
			MaskArray[count] = (mask & 1) ? *array_ptr++ : 0;
		}
		
		header_size = (UINT) ((UINT_PTR) array_ptr - (UINT_PTR) pFrame);

		// See if this frame Acknowledges any of our outstanding data
		LOGPF(7, "(%p) UpdateXmitState - Mask %x %x", pEPD, MaskArray[1], MaskArray[0]);
		UpdateXmitState(pEPD, pFrame->bNRcv, NULL, MaskArray[0], MaskArray[1], tNow);				// Do this before taking StateLock
	}
	else 
	{
		header_size = sizeof(DFRAME);
		LOGPF(7, "(%p) UpdateXmitState - No Mask", pEPD);
		UpdateXmitState(pEPD, pFrame->bNRcv, NULL, 0, 0, tNow);			// Do this before taking StateLock
	}

	// Determine if there is a SendMask in this frame which identifies dropped frames as unreliable
	if(pFrame->bControl & (PACKET_CONTROL_SEND_MASK1 | PACKET_CONTROL_SEND_MASK2))
	{
		LOGPF(7, "(%p) Processing Send Mask %x %x", pEPD, MaskArray[3], MaskArray[2]);
		ProcessSendMask(pEPD, pFrame->bSeq, MaskArray[2], MaskArray[3]);
	}

	// NOTE: ProcessSendMask may have advanced N(R)

	// We can receive this frame.   Copy relevant info into Receive descriptor
	if((pThisRCD = pRCD = static_cast<PRCD>( RCDPool->Get(RCDPool) )) == NULL)
	{
		LOGPF(0, "(%p) Failed to allocate new RCD", pEPD);
		Unlock(&pEPD->EPLock);
		return DPN_OK;
	}

	pEPD->ulEPFlags |= EPFLAGS_DELAY_ACKNOWLEDGE;	// State has changed.  Make sure it gets sent.

 	pRCD->bSeq = bSeq;
	pRCD->bFrameFlags = pFrame->bCommand;
 	pRCD->bFrameControl = pFrame->bControl;
	pRCD->pbData = (PBYTE) (((PBYTE) pFrame) + header_size);
	pRCD->uiDataSize = dwDataLength - header_size;
	pRCD->tTimestamp = tNow;
	pRCD->pRcvBuff = pRcvBuffer;

	// Update our receiving state info.
	//
	//	RCDs go onto one of two queues.  If it is the next numbered (expected) frame then it is
	// placed on the ReceiveList (in EPD).  If this frame completes a message it can now be
	// indicated.  If this frame fills a hole left by previous frames then a condensation with
	// the second list must occur.
	//	If it is not the next numbered frame then it is placed,  in order,  on the MisOrdered frame
	// list,  and the bitmask is updated.
	//
	//	Condensation of lists is performed by testing the LSB of the ReceiveMask. Each time LSB is set,
	// the first frame on the list can be moved to the ReceiveList, and the mask is shifted right.
	// As each frame is moved to the ReceiveList,  the EOM flag must be checked for and if set,  then
	// everything on the ReceiveList should be moved to the CompleteList for indication to the user.

	pEPD->uiBytesReceived += dwDataLength;				// track statistics

	if(bSeq == pEPD->bNextReceive)
	{
		// Frame is the next expected # in sequence

		LOGPF(7, "(%p) Receiving In-Order Frame, pRCD[%p]", pEPD, pRCD);
		ReceiveInOrderFrame(pEPD, pRCD);				// Build frame into message AND move adjacent frames off OddFrameList

		// See if we need to respond right away...
		//
		//	Because there are lots of way to generate POLL bits (full window, empty queue, poll count) we sometimes find ourselves
		// generating too much dedicated ack-traffic.  Therefore,  we will treat the POLL not as Respond-Immediately but instead as
		// Respond-Soon. We will not wait the full Delayed_Ack_Timeout interval but we will wait long enough to allow a quick piggyback
		// response (say 5ms) (we may want this longer on a slow connection...)

		// Is he asking for an instant response?
		if(pFrame->bControl & PACKET_CONTROL_CORRELATE)
		{
			LOGPF(7, "(%p) Sending Ack Frame", pEPD);
			SendAckFrame(pEPD, 0);						// Send Ack w/timing info
		}
		// Is he asking for a response soon?
		else if(pFrame->bCommand & PACKET_COMMAND_POLL)
		{
			if(pEPD->ulEPFlags & EPFLAGS_USE_POLL_DELAY)
			{		
				if(pEPD->DelayedAckTimer != NULL)
				{
					LOGPF(7, "(%p) Cancelling Delayed Ack Timer", pEPD);
					if(CancelMyTimer(pEPD->DelayedAckTimer, pEPD->DelayedAckTimerUnique)!= DPN_OK)
					{
						LOGPF(7, "(%p) Cancelling Delayed Ack Timer Failed", pEPD);
						LOCK_EPD(pEPD, "LOCK (re-start delayed ack timer)");
					}
				}
				else 
				{
					LOCK_EPD(pEPD, "LOCK (start short delayed ack timer)");
				}
				LOGPF(7, "Delaying POLL RESP");
				pEPD->ulEPFlags |= EPFLAGS_DELAY_ACKNOWLEDGE;

				LOGPF(7, "(%p) Setting Delayed Ack Timer", pEPD);
				SetMyTimer(4, 4, DelayedAckTimeout, (PVOID) pEPD, &pEPD->DelayedAckTimer, &pEPD->DelayedAckTimerUnique);
			}
			else 
			{
				LOGPF(7, "(%p) Sending Ack Frame", pEPD);
				SendAckFrame(pEPD, 0);						// Send Ack w/timing info
			}
		}
		else if(pEPD->DelayedAckTimer == 0){			// If timer is not running better start it now
			LOCK_EPD(pEPD, "LOCK (DelayedAckTimer)");	// Bump RefCnt for timer
			SetMyTimer(DELAYED_ACK_TIMEOUT, 0, DelayedAckTimeout, (PVOID) pEPD, &pEPD->DelayedAckTimer, &pEPD->DelayedAckTimerUnique);
		}
	}		// IF frame is in order

	// Frame arrives out of order
	else 
	{	
		bit = (BYTE) ((bSeq - pEPD->bNextReceive) - 1);						// bit location in mask for this frame

		// Make sure this is not a duplicate frame
		if( ((bit < 32) && (pEPD->ulReceiveMask & (1 << bit))) || ((bit > 31) && (pEPD->ulReceiveMask2 & (1 << (bit - 32)))) ) 
		{
			LOGPF(3, "(%p) REJECT DUPLICATE OUT-OF-ORDER Frame Seq=%x", pEPD, bSeq);
		
			Unlock(&pEPD->EPLock);
			RCDPool->Release(RCDPool, pRCD);
			return DPN_OK;
		}
		
		LOGPF(7, "(%p) Receiving Out-of-Order Frame, pRCD[%p]", pEPD, pRCD);
		ReceiveOutOfOrderFrame(pEPD, pRCD, bit);

		if(pFrame->bCommand & PACKET_COMMAND_POLL)
		{
			if(pEPD->DelayedAckTimer != NULL)
			{
				LOGPF(7, "(%p) Cancelling Delayed Ack Timer", pEPD);
				if(CancelMyTimer(pEPD->DelayedAckTimer, pEPD->DelayedAckTimerUnique)!= DPN_OK)
				{
					LOGPF(7, "(%p) Cancelling Delayed Ack Timer Failed", pEPD);
					LOCK_EPD(pEPD, "LOCK (re-start delayed ack timer)");
				}

				// Start an abreviated delayed ack timer in case NACK gets cancelled
			}
			else 
			{
				LOCK_EPD(pEPD, "LOCK (start short delayed ack timer)");
			}

			LOGPF(7, "Delaying POLL RESP");
			pEPD->ulEPFlags |= EPFLAGS_DELAY_ACKNOWLEDGE;

			LOGPF(7, "(%p) Setting Delayed Ack Timer", pEPD);
			SetMyTimer(5, 5, DelayedAckTimeout, (PVOID) pEPD, &pEPD->DelayedAckTimer, &pEPD->DelayedAckTimerUnique);
		}

	}	
	// EPD->StateLock is still HELD
	//
	// We use a FLAG for exclusive access to ReceiveComplete routine. This is safe because the flag is only
	// tested and modified while holding the EPD->StateLock.  Lets be sure to keep it that way...

	if( (!pEPD->blCompleteList.IsEmpty()) && ((pEPD->ulEPFlags & EPFLAGS_IN_RECEIVE_COMPLETE) == FALSE))
	{
		LOGPF(7, "(%p) Completing Receives", pEPD);
		pEPD->ulEPFlags |= EPFLAGS_IN_RECEIVE_COMPLETE;	// ReceiveComplete will clear this flag when done
		ReceiveComplete(pEPD); 							// Deliver the goods,  returns with EPLock released
	}
	else 
	{
		Unlock(&pEPD->EPLock);
	}

	if(pEPD->ulEPFlags & EPFLAGS_COMPLETE_SENDS)
	{
		LOGPF(7, "(%p) Completing Sends", pEPD);
		CompleteSends(pEPD);
	}

	return DPNERR_PENDING;
}

/*
**		Receive In Order Frame
**
**		The interesting part of this function is moving frames off of the OddFrameList that
**	the new frame adjoins.  This may also be called with a NULL frame which will happen when
**	a cancelled frame is the next-in-order receive.
**
**		One result of having cancelled frames running around is that we may miss the SOM or EOM
**	flags which delimit messages.  Therefore, we must watch as we assemble messages that we do not
**	see unexpected flags,  ie a new message w/o an SOM on first frame which means that part of the
**	message must have been lost,  and the whole thing must be trashed...
**
**	** EPLOCK is HELD through this entire function **
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ReceiveInOrderFrame"

VOID
ReceiveInOrderFrame(PEPD pEPD, PRCD pRCD)
{
	CBilink	*pLink;
	UINT	flag;

	// Condensing Loop  WHILE (Next In-Order Frame has been received)
	do 
	{	
		// BUGBUG: Note that we may be stuffing an older time into tLastDataFrame, is that okay?
		pEPD->tLastDataFrame = pRCD->tTimestamp;		// Always keep the receive time of (N(R) - 1)
		pEPD->bLastDataRetry = (pRCD->bFrameControl & PACKET_CONTROL_RETRY);
#ifdef	DEBUG
		pEPD->bLastDataSeq = pRCD->bSeq;
#endif
		pRCD->pMsgLink = NULL;
		if(pEPD->pNewMessage == NULL)
		{				
			// Add this frame to the in-order rcv list
			ASSERT(pEPD->pNewTail ==  NULL);		// pNewMessage implies we have no current message, head or tail

			pEPD->pNewMessage = pRCD;				// Even if we get rid of it, we will need this below

			if((pRCD->bFrameFlags & PACKET_COMMAND_NEW_MSG)==0)
			{
				// Oops!  There is no NEW_MESSAGE flag on the first frame we see. Bad news guys.  We must
				// have lost the first frame.  This had better be an UNRELIABLE message and we can throw
				// this frame away...

				ASSERT(!(pRCD->bFrameFlags & PACKET_COMMAND_RELIABLE));	// we better not have a RELIABLE fragment in here
				LOGPF(7, "(%p) Scrapping frame (%x)", pEPD, pRCD->bSeq);
				pRCD->ulRFlags |= RFLAGS_FRAME_LOST;
				pRCD->bFrameFlags |= PACKET_COMMAND_END_MSG;			// Turn this on so we will release buffer right away
			}
			else
			{
				pRCD->uiFrameCount = 1;
				pRCD->uiMsgSize = pRCD->uiDataSize;
				pEPD->pNewTail = pRCD;
				LOGPF(7, "(%p) Queuing Frame (NEW MESSAGE) (%x)", pEPD, pRCD->bSeq);
			}
		}
		else 
		{
			ASSERT((!(pRCD->bFrameFlags & PACKET_COMMAND_NEW_MSG)) || (pRCD->ulRFlags & RFLAGS_FRAME_LOST));
			ASSERT((pEPD->pNewTail->bSeq) == (BYTE)(pRCD->bSeq - 1)); // Make sure they stay sequential
			
			pEPD->pNewTail->pMsgLink = pRCD;
			pEPD->pNewMessage->uiFrameCount++;
			pEPD->pNewMessage->uiMsgSize += pRCD->uiDataSize;
			pEPD->pNewMessage->ulRFlags |= (pRCD->ulRFlags & RFLAGS_FRAME_LOST);// UNION FRAME_LOST flag from all message elements
			pEPD->pNewTail = pRCD;
			LOGPF(7, "(%p) Queuing Frame (ON TAIL) (%x)", pEPD, pRCD->bSeq);
		}

		if(pRCD->bFrameFlags & PACKET_COMMAND_END_MSG) 
		{
			// Either this frame completes a message or we decided to drop this one above

			// All frames on the ReceiveList should now be removed and delivered

			// Get either the message we are dropping above, or the beginning of the sequence of messages we are completing
			pRCD = pEPD->pNewMessage;
			pEPD->pNewMessage = NULL;

			if(pRCD->ulRFlags & RFLAGS_FRAME_LOST)
			{
				// We need to throw this away
				LOGPF(7, "(%p) Throwing away message with missing frames Seq=%x", pEPD, pRCD->bSeq);
				pEPD->pNewTail = NULL;
				DropReceive(pEPD, pRCD);
			}
			else
			{
				// We need to complete this sequence
				pRCD->blCompleteLinkage.InsertBefore( &pEPD->blCompleteList);	// place on end of Completion list
				LOGPF(7, "(%p) Adding msg to complete list FirstSeq=%x, LastSeq=%x QueueSize=%d", 
																pEPD, pRCD->bSeq, pEPD->pNewTail->bSeq, pEPD->uiCompleteMsgCount);
				pEPD->pNewTail = NULL;
				pEPD->uiCompleteMsgCount++;
			}
		}

		// 		Since we are allowing out of order indication of receives it is possible that frames later than
		// the new one have already been indicated.  This means that there may be bits set in the ReceiveMask
		// whose correlating frames do not need to be indicated.  The most straightforward way to implement this
		// is to leave the early-indicated frames in the list,  but mark them as INDICATED_NONSEQ.  So inside this
		// master DO loop there will be an inner DO loop which passes over INDICATED frames and just takes them off
		// the list.
		//
		//		Now its possible that a NonSeq indicated frame is still sitting on the CompleteList awaiting indication,
		// so I am using a ref count.  An extra ref is added when a frame is completed non-seq.  When a completed frame
		// is removed below we will release one reference,  and the indication code will release one reference when it
		// finishes on that end.  Happily,  we can release the actual buffers while the RCD turd is still sitting on the
		// OddFrameList.

		BOOL fIndicatedNonSeq = FALSE;
		do 
		{
			flag = pEPD->ulReceiveMask & 1;				// set flag if next frame in stream is present

			pEPD->bNextReceive += 1;					// Update receive window
			RIGHT_SHIFT_64(pEPD->ulReceiveMask2, pEPD->ulReceiveMask);// shift mask because base has changed
			LOGPF(7, "(%p) N(R) incremented to %x, Mask %x %x", pEPD, pEPD->bNextReceive, pEPD->ulReceiveMask2, pEPD->ulReceiveMask);

			if(flag) 
			{
				// The next frame in the sequence has arrived already since low bit of ulReceiveMask was set

				// Several things can happen here:
				// 1) We are in the middle of a message, in which case, its next piece is on the out of order list
				// 2) We have just finished a message, which leaves two subcases:
				//		a) We are beginning a new message.  In this case our first piece is on the out of order list
				//		b) Out-of-order non-sequential messages have completed while we were completing our in-order message
				//		   In this case there are some already indicated RCDs on the out of order list, and a new partial
				//		   message may or may not follow.
				pLink = pEPD->blOddFrameList.GetNext();

				ASSERT(pLink != &pEPD->blOddFrameList); // Make sure we didn't run out of RCDs on the list
				pRCD = CONTAINING_RECORD(pLink, RCD, blOddFrameLinkage);
				pLink->RemoveFromList();							// take next frame out of OddFrameList

				pRCD->ulRFlags &= ~(RFLAGS_ON_OUT_OF_ORDER_LIST);
				
				// Make sure everything previous got removed from the odd frame list, and it is sorted correctly
				ASSERT(pRCD->bSeq == pEPD->bNextReceive);

				if (pRCD->ulRFlags & RFLAGS_FRAME_INDICATED_NONSEQ)
				{
					fIndicatedNonSeq = TRUE;

					LOGPF(7, "(%p) Pulling Indicated-NonSequential message off of Out-of-Order List Seq=%x", pEPD, pRCD->bSeq);

					pEPD->tLastDataFrame = pRCD->tTimestamp;		// Always keep the receive time of (N(R) - 1)
					pEPD->bLastDataRetry = (pRCD->bFrameControl & PACKET_CONTROL_RETRY);
					DEBUG_ONLY(pEPD->bLastDataSeq = pRCD->bSeq);

					RELEASE_RCD(pRCD);
				}
				else
				{
					// In the case of cancelling one of the messages in the middle of a large message,
					// we will drop all previous, drop that one, and then we will get to a situation where we aren't
					// currently working on a new message (pNewMessage was NULL'ed in the cancelling) and the current 
					// message does not have the NEW_MSG flag, in which case we scrap it above.
//					ASSERT(!fIndicatedNonSeq || (pRCD->bFrameFlags & PACKET_COMMAND_NEW_MSG));
					ASSERT(!fIndicatedNonSeq || (pRCD->bFrameFlags & PACKET_COMMAND_NEW_MSG) ||
						(!pEPD->pNewMessage && !(pRCD->bFrameFlags & PACKET_COMMAND_NEW_MSG)));

					// go ahead and move this to the receive list
					LOGPF(7, "(%p) Moving OutOfOrder frame to received list Seq=%x", pEPD, pRCD->bSeq);
					break;
				}
			}
		} 
		while (flag);	// DO WHILE (There are still in order frames that have already arrived with no incomplete messages)
	} 
	while (flag);		// DO WHILE (There are still in order frames that have already arrived with an incomplete message)

	if((pEPD->ulReceiveMask | pEPD->ulReceiveMask2)==0)
	{
		pEPD->ulEPFlags &= ~(EPFLAGS_DELAYED_NACK);
		if(((pEPD->ulEPFlags & EPFLAGS_DELAYED_SENDMASK)==0)&&(pEPD->DelayedMaskTimer != NULL))
		{
			LOGPF(7, "(%p) Cancelling Delayed Mask Timer", pEPD);
			if(CancelMyTimer(pEPD->DelayedMaskTimer, pEPD->DelayedMaskTimerUnique) == DPN_OK)
			{
				RELEASE_EPD(pEPD, "UNLOCK (cancel DelayedMask)"); // SendQLock not already held
				pEPD->DelayedMaskTimer = 0;
			}
			else
			{
				LOGPF(7, "(%p) Cancelling Delayed Mask Timer Failed", pEPD);
			}
		}
	}
}

/*
**		Receive Out Of Order Frame
**
**		Its like the title says.  We must set the appropriate bit in the 64-bit ReceiveMask
**	and then place it into the OddFrameList in its proper sorted place.  After that,  we must
**	scan to see if a complete message has been formed and see if we are able to indicate it
**	early.
**
**	** EPLOCK is HELD through this entire function **
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ReceiveOutOfOrderFrame"

VOID
ReceiveOutOfOrderFrame(PEPD pEPD, PRCD pRCD, ULONG bit)
{
	PRCD	pRCD1;
	PRCD	pRCD2;
	CBilink	*pLink;
	BYTE	NextSeq;
	BYTE	mask;
	ULONG	highbit;
	ULONG	Mask;
	ULONG	WorkMaskHigh;
	ULONG	WorkMaskLow;
	ULONG	MaskHigh;
	ULONG	MaskLow;
	BOOL	nack = FALSE;
	UINT	count;
	BOOL	lost = FALSE;

	UINT	frame_count = 0;
	UINT	msg_length = 0;

	//	RECEIVE OUT OF ORDER FRAME
	//
	//  DO WE SEND IMMEDIATE ACK FOR THIS OUT OF ORDER FRAME?
	//
	//		When we receive an OutOfOrder frame it is almost certainly because the missing frame has been lost.
	// So we can accelerate the re-transmission process greatly by telling partner right away that frames
	// are missing. HOWEVER,  with a large send window we will get many mis-ordered frames for each drop,
	// but we only want to send a Negative Ack once.  SO, we will only initiate a NACK here if we have
	// created a NEW HOLE in our receive mask!
	//
	// 		First,  we will not have created a new hole unless we added to the END of the OddFrameList.
	//		Second, we will not have created a new hole unless the FIRST BIT TO THE RIGHT of the new bit
	//	is CLEAR.
	//
	//	So we will only generate an immediate NACK frame if both of the above cases are true!
	//	NOTE - if this is the only OoO frame, then we should always send a NACK
	//
	//  ANOTHER NOTE.  SP  implementation has been changed so that it frequently misorders receives in close
	// proximity.  One effect of this is that we must not immediately send a NACK for an out of order frame, but
	// instead should wait a short period (say ~5ms) and see if the missing frame hasnt showed up.  What a drag...

	// Make sure this RCD is within the receive window
//	ASSERT((BYTE)(pRCD->bSeq - pEPD->bNextReceive) < (BYTE)MAX_FRAME_OFFSET);
		
	pRCD->ulRFlags |= (RFLAGS_FRAME_OUT_OF_ORDER | RFLAGS_ON_OUT_OF_ORDER_LIST);
	
	
	// We will insert frame in OddFrameList maintaining sort by Seq number
	//
	// We can optimize for most likely case,  which is new frames are added to END of list.  We can
	// check for this first case by investigating whether the new bit is the left-most bit in the mask.
	// If it is LMB,  then it trivially gets added to the END of the list.
	//
	// Please note that both this and the following algorithms assume that we have already verified
	// that the new frame is NOT already in the list

	MaskLow = pEPD->ulReceiveMask;					// Get scratch copy of Mask
	MaskHigh = pEPD->ulReceiveMask2;

	if(bit < 32)
	{									
		// Frame is within 32 of N(Rcv)	
		WorkMaskLow = 1 << bit;						// Find bit in mask for new frame
		WorkMaskHigh = 0;
		pEPD->ulReceiveMask |= WorkMaskLow;			// Set appropriate bit in mask

		if( (MaskLow & (WorkMaskLow >> 1)) == 0){	// check immediately preceeding bit for NACK determination
			nack = TRUE;							// preceeding bit is not set
		}
	}
	else {
		highbit = bit - 32;
		WorkMaskHigh = 1 << highbit;
		WorkMaskLow = 0;
		pEPD->ulReceiveMask2 |= WorkMaskHigh;		// Set appropriate bit in mask

		if(highbit){
			if( (MaskHigh & (WorkMaskHigh >> 1)) == 0){	// check preceeding bit for NACK determination
				nack = TRUE;						// preceeding bit is not set
			}
		}
		else{
			if( (MaskLow & 0x80000000) == 0){
				nack = TRUE;
			}
		}
	}

	// Insert frame in sorted OddFrameList.
	//
	//		First test for trivial insert at tail condition.  True if new bit is LEFTMOST set bit in
	// both masks.

	if( (WorkMaskHigh > MaskHigh) || ( (MaskHigh == 0) && (WorkMaskLow > MaskLow) ) )
	{	
		// TAIL INSERTION
		LOGPF(7, "(%p) Received %x OUT OF ORDER - INSERT AT TAIL NRcv=%x MaskL=%x MaskH=%x",
							pEPD, pRCD->bSeq, pEPD->bNextReceive, pEPD->ulReceiveMask, pEPD->ulReceiveMask2);
		pLink = &pEPD->blOddFrameList;

		// Make sure this is either the only RCD in the list, or it is farther in the window than the last one
//		ASSERT(pLink->IsEmpty() || ((BYTE)(CONTAINING_RECORD(pLink->GetPrev(), RCD, blOddFrameLinkage))->bSeq - pEPD->bNextReceive) < (BYTE)(pRCD->bSeq - pEPD->bNextReceive));
		pRCD->blOddFrameLinkage.InsertBefore( pLink);

		// Check to see if we should NACK (negative acknowledge) any frames.  We only want to NACK a given
		// frame once so we will NACK if this is the first frame added to the OOF list,  or if the immediately
		// preceeding frame is missing.  First condition is trivial to test.

		if( ((MaskLow | MaskHigh) == 0) || (nack == 1) )
		{
			pEPD->ulEPFlags |= EPFLAGS_DELAYED_NACK;
			
			if(pEPD->DelayedMaskTimer == 0)
			{
				LOGPF(7, "(%p) Setting Delayed Mask Timer", pEPD);
				LOCK_EPD(pEPD, "LOCK (DelayedMaskTimer)");	// Bump RefCnt for timer
				SetMyTimer(SHORT_DELAYED_ACK_TIMEOUT, 5, DelayedAckTimeout, (PVOID) pEPD, &pEPD->DelayedMaskTimer, &pEPD->DelayedMaskTimerUnique);
				pEPD->tReceiveMaskDelta = GETTIMESTAMP();
			}
			else 
			{
				LOGPF(7, "(%p) *** DELAYED NACK *** Timer already running", pEPD);
			}
		}
	}
	else 
	{	
		// NOT TAIL INSERTION

		// This is the non-trivial case,  ie new frame goes at beginning or in middle of OddFrameList.
		// So we need to count the ONE bits that are to the RIGHT of the new bit in the ReceiveMask.
		// We will mask off bits higher then the New Bit and then do a quick bit-count...

		LOGPF(7, "(%p) Receive OUT OF ORDER - Walking Frame List (Seq=%x, NRcv=%x) MaskL=%x MaskH=%x", 	pEPD, pRCD->bSeq, pEPD->bNextReceive, pEPD->ulReceiveMask, pEPD->ulReceiveMask2);

		// If we are inserting into high mask,  we must count all one-bits in low mask
		//
		// We will test for the special case of all-bits-set at the outset...

		pLink = pEPD->blOddFrameList.GetNext();			// pLink = First frame in list; we will walk list as we count

		if(WorkMaskHigh)
		{
			// new frame in high mask. only count bits to right of new bit
			WorkMaskHigh -= 1;						// Convert to mask preserving all bits to right of new bit
			WorkMaskHigh &= MaskHigh;				// WMH now represents all bits to right of new bit
			while(WorkMaskHigh)
			{
				// Make sure this is farther in the window than the one we are skipping
//				ASSERT(((BYTE)(CONTAINING_RECORD(pLink, RCD, blOddFrameLinkage))->bSeq - pEPD->bNextReceive) < (BYTE)(pRCD->bSeq - pEPD->bNextReceive));

				// Count bits in WMH
				Mask = WorkMaskHigh - 1;
				WorkMaskHigh &= Mask;
				pLink = pLink->GetNext();
			}
			if(MaskLow == 0xFFFFFFFF)
			{
				// special case if low mask is full
				for(count = 0; count < 32; count++)
				{
					// Make sure this is farther in the window than the one we are skipping
//					ASSERT(((BYTE)(CONTAINING_RECORD(pLink, RCD, blOddFrameLinkage))->bSeq - pEPD->bNextReceive) < (BYTE)(pRCD->bSeq - pEPD->bNextReceive));

					pLink = pLink->GetNext();
				}
			}
			else
			{					
				// else count all bits in lower mask
				while(MaskLow)
				{
					// Make sure this is farther in the window than the one we are skipping
//					ASSERT(((BYTE)(CONTAINING_RECORD(pLink, RCD, blOddFrameLinkage))->bSeq - pEPD->bNextReceive) < (BYTE)(pRCD->bSeq - pEPD->bNextReceive));

					Mask = MaskLow - 1;
					MaskLow &= Mask;			// Mask off low 1-bit
					pLink = pLink->GetNext();
				}
			}
		}
		else 
		{
			WorkMaskLow -= 1;
			WorkMaskLow &= MaskLow;					// WML == bits to the right of new bit

			while(WorkMaskLow)
			{
				// Make sure this is farther in the window than the one we are skipping
//				ASSERT(((BYTE)(CONTAINING_RECORD(pLink, RCD, blOddFrameLinkage))->bSeq - pEPD->bNextReceive) < (BYTE)(pRCD->bSeq - pEPD->bNextReceive));

				Mask = WorkMaskLow - 1;
				WorkMaskLow &= Mask;				// Mask off low 1-bit
				pLink = pLink->GetNext();
			}
		}

		// Make sure this is farther in the window than the last one
//		ASSERT(((BYTE)(CONTAINING_RECORD(pLink->GetPrev(), RCD, blOddFrameLinkage))->bSeq - pEPD->bNextReceive) < (BYTE)(pRCD->bSeq - pEPD->bNextReceive));

		pRCD->blOddFrameLinkage.InsertBefore( pLink);		// Insert new frame in sorted list
	}  // Receive not at tail

	// 	**	Non-Sequential Indication
	//
	//		This is the Non-Trivial implementation of non-sequential receive indication.
	//	We will work from the assumption that we  only need to complete messages that are touched by the new frame.
	//	So we must back up in the OddFrame list until we see either a gap or a Start of Message marker. Then we must work
	// 	forward looking for an End of Message...
	//
	//		One more complication is the fact that dropped packets might in the list via place-holding dummies.  Since we
	//	do not know what SOM/EOM flags would have been present on a dropped frame,  we can consider them to have BOTH flags.
	//	Then we also need to be aware that frames bordering on dropped frames without a delimiter (SOM or EOM) are fragments
	//	and therefore count as dropped data too.  I think to keep this from getting too complex,  we wont probe further for
	//	neighbors of lost data frames.  We will discover them when we are building a message later.
	//
	//	pLink = Item after new element in Out of Order list
	//	pRCD  = new Item

	// IF this frame is not marked as SEQUENTIAL
	if((pRCD->bFrameFlags & PACKET_COMMAND_SEQUENTIAL)==0)
	{	
		LOGPF(7, "(%p) Received Non-Seq %x out of order; flags=%x", pEPD, pRCD->bSeq, pRCD->bFrameFlags);
		NextSeq = pRCD->bSeq;
			
		// NOTE: The first pLink will be the passed in RCD so we will have included that in the frame_count and msg_length after leaving this while
		while ( (pLink = pLink->GetPrev()) != &pEPD->blOddFrameList )
		{
			pRCD1 = CONTAINING_RECORD(pLink, RCD, blOddFrameLinkage);

			frame_count++;
			msg_length += pRCD1->uiDataSize;

			if((pRCD1->bFrameFlags & PACKET_COMMAND_NEW_MSG) || (pRCD1->bSeq != NextSeq))
			{
				break;		// Stop probing when we find a NEW_MSG flag OR a gap in the frame sequence numbers
			}
			--NextSeq;
		}

		// We have either found a NEW_MSG or a sequence gap.  If its a NEW_MSG, then we probe forward for an END_MSG
		if((pRCD1->bFrameFlags & PACKET_COMMAND_NEW_MSG) && (pRCD1->bSeq == NextSeq))
		{
			// So far so good.  We have a sequential message start frame
			//
			//	pRCD  = frame just arrived
			//	pRCD1 = Start of message frame
			//	pLink = Start of message linkage

			pLink = &pRCD->blOddFrameLinkage;
			NextSeq = pRCD->bSeq;

			// Look for the message end or a sequence gap
			while ( ( (pRCD->bFrameFlags & PACKET_COMMAND_END_MSG)==0 ) && (pRCD->bSeq == NextSeq))
			{
				// Stop if we hit the end of the OddFrameList
				if((pLink = pLink->GetNext()) == &pEPD->blOddFrameList)
				{
					break;
				}

				// NOTE: the first pLink here will be the one after the passed in RCD.  If there is a gap that won't 
				// matter because we are out of here after the next if.  If it is the next message we will continue until
				// we hit the END_MSG and have a proper frame_count and msg_length.
				pRCD = CONTAINING_RECORD(pLink, RCD, blOddFrameLinkage);
				frame_count++;
				msg_length += pRCD->uiDataSize;
				NextSeq++;
			}

			// pLink should not be used after this point due to the way the above while could have left it either valid
			// or at &pEPD->blOddFrameList.
			pLink = NULL;

			if((pRCD->bFrameFlags & PACKET_COMMAND_END_MSG) && (pRCD->bSeq == NextSeq))
			{
				// We have completed a message
				//
				// pRCD1 = First frame in message
				// pRCD = Last frame in message

				LOGPF(7, "(%p) Completed Non-Seq Msg: First=%x, Last=%x", pEPD, pRCD1->bSeq, pRCD->bSeq);

				lost = FALSE;

				pRCD->ulRFlags |= RFLAGS_FRAME_INDICATED_NONSEQ;
				pRCD->pMsgLink = NULL;
				lost |= pRCD->ulRFlags & RFLAGS_FRAME_LOST;
				
				// Get the pointer to the next to last message so we can remove the last
				pLink = pRCD->blOddFrameLinkage.GetPrev();
				LOCK_RCD(pRCD); // ReceiveInOrderFrame must remove this

				// Walk from the last message to the first message accumulating lost flags, linking messages, 
				// setting indicated flag, and pulling off of the odd frame list
				while (pRCD != pRCD1)
				{
					ASSERT(pLink != &pEPD->blOddFrameList); // Make sure we didn't run out of RCDs on the list
					pRCD2 = CONTAINING_RECORD(pLink, RCD, blOddFrameLinkage);
					pRCD2->pMsgLink = pRCD;
					LOCK_RCD(pRCD2); // ReceiveInOrderFrame must remove this

					pRCD2->ulRFlags |= RFLAGS_FRAME_INDICATED_NONSEQ;
					lost |= pRCD2->ulRFlags & RFLAGS_FRAME_LOST;
					pLink = pRCD2->blOddFrameLinkage.GetPrev();

					pRCD = pRCD2;
				}
				
				// Both RCD and RCD1 point to the first message now

				// If any were lost, drop the receive, otherwise complete it
				if(!lost)
				{
					pRCD->uiFrameCount = frame_count;
					pRCD->uiMsgSize = msg_length;
					pRCD->blCompleteLinkage.InsertBefore( &pEPD->blCompleteList);
					pEPD->uiCompleteMsgCount++;
				}
				else
				{
					LOGPF(7, "(%p) Complete Non-Seq MSG is dropped due to missing frames", pEPD);
					DropReceive(pEPD, pRCD);
				}
			}
		} // else  there is nothing to complete at this time...
	}	// IF NON SEQUENTIAL
}

/*
**		Drop Receive
**
**			One or more frames composing a message have been dropped,  so the entire message can be scrapped.
**		If this was determined during an out of order receive then the RCDs will remain on the OddFrameList
**		as usual.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DropReceive"

VOID
DropReceive(PEPD pEPD, PRCD pRCD)
{
	PRCD 					pNext;
	PSPRECEIVEDBUFFER		pRcvBuff = NULL;
	
	while(pRCD != NULL)
	{
		LOGPF(7, "(%p) dropping receive frame %x", pEPD, pRCD->bSeq);

		if(pRCD->pRcvBuff)
		{
			RELEASE_SP_BUFFER(pRCD->pRcvBuff);
		}

		pNext = pRCD->pMsgLink;
		RELEASE_RCD(pRCD);
		pRCD = pNext;
	}
	if(pRcvBuff != NULL)
	{
		LOGPF(6, "(%p) Calling SP->ReturnReceiveBuffers", pEPD);
		IDP8ServiceProvider_ReturnReceiveBuffers(pEPD->pSPD->IISPIntf, pRcvBuff);
	}
}

/*
**		Receive Complete
**
**		We have received an entire reliable message,  potentially spanning
**	multiple messages.  We are still on the receive thread right now so depending
**	upon our desired indication behavior we will either indicate it directly or
**	else queue it to be indicated on a background thread of some sort.
**
**		Messages spanning multiple frames (for now) will be copied into a contiguous
**	buffer for delivery.  CODEWORK -- Server implementations should be able to receive
**	large messages as buffer chains (or arrays of BufDescs).
**
**		This is also where we must notice that an End Of Stream flag is set in a message,
**	indicating that the connection is being closed.
**
**	*** CALLED WITH EPD->STATELOCK HELD *** RETURNS WITH STATELOCK RELEASED  ***
*/

#ifdef	DEBUG
	// For Debugging we will track the total number of receives outstanding in the higher layers
	// at all times.
LONG	ThreadsInReceive	= 0;
LONG	BuffersInReceive	= 0;
#endif

#undef DPF_MODNAME
#define DPF_MODNAME "ReceiveComplete"

VOID
ReceiveComplete(PEPD pEPD)
{
	CBilink					*pLink;
	PRCD					pRCD;
	PRCD					pNext;
	PSPRECEIVEDBUFFER		pRcvBuff = NULL;
	PBIGBUF					pBuf;
	PBYTE					write;
	UINT					length;
	UINT					frames;
	DWORD					flag;
	UINT					MsgSize;
	HRESULT					hr;


	while((pLink = pEPD->blCompleteList.GetNext()) != &pEPD->blCompleteList)
	{
		pLink->RemoveFromList();
		ASSERT(pEPD->uiCompleteMsgCount > 0);
		pEPD->uiCompleteMsgCount--;
		
		Unlock(&pEPD->EPLock);
		pRCD = CONTAINING_RECORD(pLink, RCD, blCompleteLinkage);
		
		// Handle easy case first
		if(pRCD->uiFrameCount == 1)
		{						
			if(pRCD->uiDataSize > 0)
			{
				pEPD->uiMessagesReceived++;
				
				if(pRCD->bFrameFlags & PACKET_COMMAND_RELIABLE)
				{
					pEPD->uiGuaranteedFramesReceived++;
					pEPD->uiGuaranteedBytesReceived += pRCD->uiDataSize;
				}
				else
				{
					pEPD->uiDatagramFramesReceived++;
					pEPD->uiDatagramBytesReceived += pRCD->uiDataSize;
				}
				
				pRCD->pRcvBuff->dwProtocolData = RBT_SERVICE_PROVIDER_BUFFER;
				pRCD->pRcvBuff->pServiceProviderDescriptor = pEPD->pSPD;
				flag = (((DWORD) pRCD->bFrameFlags) & (PACKET_COMMAND_USER_1 | PACKET_COMMAND_USER_2)) * (DN_SENDFLAGS_SET_USER_FLAG / PACKET_COMMAND_USER_1);

				DEBUG_ONLY(InterlockedIncrement(&ThreadsInReceive));
				DEBUG_ONLY(InterlockedIncrement(&BuffersInReceive));
				LOGPF(6, "(%p) Calling Core->IndicateReceive, pRCD[%p]", pEPD, pRCD);
				if(pEPD->pSPD->pPData->pfVtbl->IndicateReceive(pEPD->pSPD->pPData->Parent, pEPD->Context, pRCD->pbData, pRCD->uiDataSize, pRCD->pRcvBuff, flag) == DPN_OK)
				{
					RELEASE_SP_BUFFER(pRCD->pRcvBuff);		// Really only queues it to be released
					DEBUG_ONLY(InterlockedDecrement(&BuffersInReceive));
					DEBUG_ONLY(InterlockedDecrement(&ThreadsInReceive));
				}
				DEBUG_ONLY(InterlockedDecrement(&ThreadsInReceive));
			}
			else 
			{
				//	If DataSize == 0 & EOS is not set,  then this was a keep alive message and is ignored
				if(pRCD->bFrameControl & PACKET_CONTROL_END_STREAM)
				{  
					// END OF STREAM indicated
					LOGPF(7, "(%p) Processing EndOfStream, pRCD[%p]", pEPD, pRCD);
					ProcessEndOfStream(pEPD);
				}
				
				RELEASE_SP_BUFFER(pRCD->pRcvBuff);			// Really only queues it to be released
			}
			
			RELEASE_RCD(pRCD);								// Release reference for Complete Processing
		}
		else 
		{											
			// Multiple buffers.  Need to copy data into contiguous buffer.
			if((MsgSize = pRCD->uiMsgSize) <= SMALL_BUFFER_SIZE)
			{
				pBuf = static_cast<PBIGBUF>( BufPool->Get(BufPool) );
			}
			else if (MsgSize <= MEDIUM_BUFFER_SIZE)
			{
				pBuf = static_cast<PBIGBUF>( MedBufPool->Get(MedBufPool) );
			}
			else if (MsgSize <= LARGE_BUFFER_SIZE)
			{
				pBuf = static_cast<PBIGBUF>( BigBufPool->Get(BigBufPool) );
			}
			else
			{
				LOGPF(7, "(%p) RECEIVE HUGE MESSAGE", pEPD);
				// Receive is larger then our biggest static receive buffer.  This means we have to allocate a dynamic buffer.
				pBuf = (PBIGBUF) MALLOC(MsgSize + sizeof(DYNBUF));
				if(pBuf)
				{
					pBuf->Type = RBT_DYNAMIC_BUFFER;
				}	
			}
			
			if(pBuf == NULL)
			{
				LOGPF(0, "MEMORY ALLOC FAILED.  Cannot deliver data");
				while(pRCD != NULL)
				{
					pNext = pRCD->pMsgLink;
					RELEASE_SP_BUFFER(pRCD->pRcvBuff);
					RELEASE_RCD(pRCD);
					pRCD = pNext;
				}
				Lock(&pEPD->EPLock);
				continue;								// blow it off!
			}
			write = pBuf->data;							// initialize write pointer
			length = 0;
			frames = 0;
			while(pRCD != NULL)
			{
				memcpy(write, pRCD->pbData, pRCD->uiDataSize);
				write += pRCD->uiDataSize;
				length += pRCD->uiDataSize;
				frames++;
				pNext = pRCD->pMsgLink;
				flag = (DWORD) pRCD->bFrameFlags;
				RELEASE_SP_BUFFER(pRCD->pRcvBuff);
				RELEASE_RCD(pRCD);
				pRCD = pNext;
			}
			
			pEPD->uiMessagesReceived++;
			if(flag & PACKET_COMMAND_RELIABLE)
			{
				pEPD->uiGuaranteedFramesReceived += frames;
				pEPD->uiGuaranteedBytesReceived += length;
			}
			else
			{
				pEPD->uiDatagramFramesReceived += frames;
				pEPD->uiDatagramBytesReceived += length;
			}
			
			flag = (flag & (PACKET_COMMAND_USER_1 | PACKET_COMMAND_USER_2)) * (DN_SENDFLAGS_SET_USER_FLAG / PACKET_COMMAND_USER_1);
			LOGPF(6, "(%p) Calling Core->IndicateReceive", pEPD);
			hr = pEPD->pSPD->pPData->pfVtbl->IndicateReceive(pEPD->pSPD->pPData->Parent, pEPD->Context, pBuf->data, length, pBuf, flag);

			// BUGBUG: Don't we still need to free on fail?
			if(hr == DPN_OK)
			{
				if(pBuf->Type == RBT_PROTOCOL_BUFFER)
				{
					pBuf->Owner->Release(pBuf->Owner, pBuf);
				}
				else 
				{
					FREE(pBuf);
				}
			}
		}
		Lock(&pEPD->EPLock);
	}

	pEPD->ulEPFlags &= ~(EPFLAGS_IN_RECEIVE_COMPLETE);	// Clear this before releasing Lock final time
	Unlock(&pEPD->EPLock);

	if(pRcvBuff != NULL)
	{
		LOGPF(6, "(%p) Calling SP->ReleaseReceiveBuffers", pEPD);
		IDP8ServiceProvider_ReturnReceiveBuffers(pEPD->pSPD->IISPIntf, pRcvBuff);
	}
}


/*
**		Process Send Mask
**
**		The send mask is what our partner uses to tell us to stop waiting for particular frames.
**	This will happen after an Unreliable frame is dropped.  Instead of retransmitting the unreliable
**	frame,  the sender will forward the appropriate bit in a send mask.  In this routine,  we attempt
**	to update our receive state pursuant to the newly received mask.
**
**		THIS IS CALLED WITH STATELOCK HELD AND RETURNS WITH STATELOCK HELD
**
**		suffice it to say that we should not release statelock anywhere in the following
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ProcessSendMask"

VOID
ProcessSendMask(PEPD pEPD, BYTE bSeq, ULONG MaskLow, ULONG MaskHigh)
{
	INT		deltaS;
	ULONG	workmaskS;
	BYTE	bThisFrame;
	UINT	skip;
	BOOL	receive = FALSE;

	ASSERT(MaskLow | MaskHigh);

	LOGPF(7, "(%p) PROCESS SEND MASK  N(R)=%x, bSeq=%x, MaskL=%x, MaskH=%x", pEPD, pEPD->bNextReceive, bSeq, MaskLow, MaskHigh);

	// The mask can only refer to frames earlier than the Seq number in this frame.  So if this frame
	// is the next In-Order then the mask can have nothing interesting in it.
	//
	// The SendMask is coded with decending frame numbers starting at the Seq in this frame - 1.
	// The ReceiveMask is coded with ascending frame numbers start at N(Rcv) + 1.
	//
	// We will walk forward through the rcvmask and backward through the sendmask looking for the magic combo
	// of a bit clear in the rcvmask and the corresponding bit set in the sendmask.  For each of these matches,
	// a dummy cancel frame can be 'received' for that sequence number.
	//
	// This would be fairly straightforward if it wasn't for the fact that both masks are 64 bits so the code has
	// to track which DWORD of each mask we are dealing with at any given time. 

top:

	if (bSeq != pEPD->bNextReceive)
	{
		deltaS = (INT) (((BYTE)(bSeq - pEPD->bNextReceive)) - 1);			// count of frames between first missing frame and sequence base of mask
		bThisFrame = pEPD->bNextReceive;

		if ( deltaS < ( MAX_FRAME_OFFSET - 1 ) ) 
		{
			// If the difference is greater then 32 frames then we need to look at the high mask first and
			// then fall through to the low mask.  Otherwise,  we can ignore the highmask and start with the low.
			while((deltaS > 31) && (MaskHigh)) // Any work to do in the upper bits?
			{
				workmaskS = 1 << (deltaS - 32); 	// walks bit positions backward in send mask
				
				// See if the next frame we are interested in is covered by this mask
				if(workmaskS & MaskHigh)
				{
					receive |= CancelFrame(pEPD, bThisFrame);
					MaskHigh &= ~workmaskS;

					// N(R) may have been bumped up multiple times by CancelFrame, reset to make sure we work with
					// up to date information.
					goto top;
				}
				else
				{
					bThisFrame++;
					deltaS--;
				}
			}

			if(deltaS > 31)
			{
				skip = deltaS - 31;								// how many bit positions did we skip
				bThisFrame += (BYTE) skip;
				deltaS -= skip;
			}

			while((deltaS >= 0) && (MaskLow)) // Any work to do in the lower bits?
			{
				workmaskS = 1 << deltaS;

				if(workmaskS & MaskLow)
				{
					receive |= CancelFrame(pEPD, bThisFrame);
					MaskLow &= ~workmaskS;

					// N(R) may have been bumped up multiple times by CancelFrame, reset to make sure we work with
					// up to date information.
					goto top;
				}
				else
				{
					bThisFrame++;
					deltaS--;
				}
			}
		}
	}
	else
	{
		LOGPF(7, "(%p) bSeq == N(R) = %x, nothing to do", pEPD, pEPD->bNextReceive);
	}

	// If timer is not running better start it now
	if((receive)&&(pEPD->DelayedAckTimer == 0))
	{	
		LOCK_EPD(pEPD, "LOCK (DelayedAckTimer)");		// Bump RefCnt for timer
		pEPD->ulEPFlags |= EPFLAGS_DELAY_ACKNOWLEDGE;
		LOGPF(7, "(%p) Setting Delayed Ack Timer", pEPD);
		SetMyTimer(DELAYED_ACK_TIMEOUT, 0, DelayedAckTimeout, (PVOID) pEPD, &pEPD->DelayedAckTimer, &pEPD->DelayedAckTimerUnique);
	}
}

#undef DPF_MODNAME
#define DPF_MODNAME "BuildCancelledRCD"

PRCD
BuildCancelledRCD(PEPD pEPD, BYTE bSeq)
{
	PRCD	pRCD;
	
	if((pRCD = static_cast<PRCD> (RCDPool->Get(RCDPool))) == NULL)
	{
		LOGPF(0, "Failed to allocate RCD");
		return NULL;
	}
	
 	pRCD->bSeq = bSeq;
	pRCD->bFrameFlags = PACKET_COMMAND_NEW_MSG | PACKET_COMMAND_END_MSG;
 	pRCD->bFrameControl = 0;
	pRCD->pbData = NULL;
	pRCD->uiDataSize = 0;
	pRCD->tTimestamp = GETTIMESTAMP();
	pRCD->pRcvBuff = NULL;
	pRCD->ulRFlags = RFLAGS_FRAME_LOST;

	return pRCD;
}

/*			Cancel Frame
**
**		An unreliable frame has been reported as lost by sender.  This means we should consider it acknowledged
**	and remove it from our receive window.  This may require us to place a dummy receive descriptor in the OddFrameList
**	to hold its place until the window moves past it.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "CancelFrame"

BOOL
CancelFrame(PEPD pEPD, BYTE bSeq)
{
	PRCD	pRCD;
	ULONG	bit;

	LOGPF(7, "(%p) CANCEL FRAME: Seq=%x", pEPD, bSeq);

	// Trivial case is when the cancelled frame is at the front of the window.  In this case we can complete not only
	// this frame but any contiguous frames following it in the OddFrameList
	
	if(pEPD->bNextReceive == bSeq)
	{
		if((pRCD = BuildCancelledRCD(pEPD, bSeq)) == NULL)
		{
			return FALSE;
		}
		ReceiveInOrderFrame(pEPD, pRCD);
	}

	// Here we have to place a dummy node on the OddFrameList to represent this frame.
	else 
	{
		bit = (bSeq - pEPD->bNextReceive) - 1;							// bit location in mask for this frame

		// Make sure this is not a duplicate frame
		if( ((bit < 32) && (pEPD->ulReceiveMask & (1 << bit))) || ((bit > 31) && (pEPD->ulReceiveMask2 & (1 << (bit - 32)))) ) 
		{
			LOGPF(7, "(%p) Received CancelMask for frame that's already received Seq=%x", pEPD, bSeq);
			return FALSE;
		}
		
		if((pRCD = BuildCancelledRCD(pEPD, bSeq)) == NULL)
		{
			return FALSE;
		}
		ReceiveOutOfOrderFrame(pEPD, pRCD, bit);
	}

	return TRUE;
}


/*
**		Release Receive Buffer
**
**		The core calls this function to return buffers previously handed
**	over in an IndicateUserData call.  This call may be made before the
**	actual indication returns.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DNPReleaseReceiveBuffer"

HRESULT
DNPReleaseReceiveBuffer(HANDLE hBuffer)
{
	union {
		PBIGBUF 			pBuf;
		PSPRECEIVEDBUFFER	pRcvBuff;
	}					pBuffer;

	PSPD				pSPD;

	pBuffer.pBuf = (PBIGBUF) hBuffer;
	
	switch(pBuffer.pBuf->Type)
	{
		case RBT_SERVICE_PROVIDER_BUFFER:
			DEBUG_ONLY(InterlockedDecrement(&BuffersInReceive));

			pSPD = (PSPD) pBuffer.pRcvBuff->pServiceProviderDescriptor;
			pBuffer.pRcvBuff->pNext = NULL;

			LOGPF(6, "Calling SP->ReturnReceiveBuffers, pRcvBuff[%p]", pBuffer.pRcvBuff);
			IDP8ServiceProvider_ReturnReceiveBuffers(pSPD->IISPIntf, pBuffer.pRcvBuff);
			break;
			
		case RBT_PROTOCOL_BUFFER:
			pBuffer.pBuf->Owner->Release(pBuffer.pBuf->Owner, pBuffer.pBuf);
			break;

		case RBT_DYNAMIC_BUFFER:
			FREE(hBuffer);
			break;

		default:
			LOGPF(0, "RELEASE RECEIVE BUFFER CALLED WITH BAD PARAMETER");
			return DPNERR_INVALIDPARAM;
	}
	
	return DPN_OK;
}

/*
**		Complete Sends
**
**		Reliable sends are completed upon acknowlegement.  Acknowlegements are discovered inside
**	the UpdateXmitState routine while walking through the pending window.  Since the actual completion
**	event requires the user to be called,  state can change.  So the easiest thing to do is defer these
**	completion callbacks until we are finished walking and can release any state locks.  Also,  this way
**	we can defer the callbacks until after we have indicated any data which the acks were piggybacking on,
**	something which ought to have priority anyway.
**
**		So we will place all completed reliable sends onto a complete list and after all other processing
**	we will come here and callback everything on the list.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "CompleteSends"

VOID CompleteSends(PEPD pEPD)
{
	PMSD	pMSD;
	CBilink	*pLink;

	Lock(&pEPD->EPLock);

	while( (pLink = pEPD->blCompleteSendList.GetNext()) != &pEPD->blCompleteSendList)
	{
		pMSD = CONTAINING_RECORD(pLink, MSD, blQLinkage);

		if(pMSD->ulMsgFlags2 & MFLAGS_TWO_COMPLETE)
		{
			pLink->RemoveFromList();

			Unlock(&pEPD->EPLock);
			Lock(&pMSD->CommandLock);
			CompleteReliableSend(pEPD->pSPD, pMSD, DPN_OK);
			
			Lock(&pEPD->EPLock);
		}
		else 
		{
			break;		// These will complete in order, so stop checking when we see one that's not Complete.
		}
	}

	pEPD->ulEPFlags &= ~(EPFLAGS_COMPLETE_SENDS);
	
	Unlock(&pEPD->EPLock);
}

/*
**		Lookup CheckPoint
**
**		Walk the EndPoint's list of active CPs looking for one with the provided
**	response correlator.
**		We keep the CKPT queue sorted by age so the matches should be at the front
**	of the queue.  So as we pass by entries at the head we will check the age of each
**	and timeout the ones that are 4(RTT) or greater.
**		Since DG drops are reported by the partner,  we dont need to do any booking about
**	the orphaned checkpoints.
**
**		*!* This link's StateLock must be held on entry
*/

#ifdef	DEBUG
#undef DPF_MODNAME
#define DPF_MODNAME "DumpChkPtList"

VOID
DumpChkPtList(PEPD pEPD)
{
	CBilink	*pLink;
	PCHKPT	pCP;

	LOGPF(1, "==== DUMPING CHECKPOINT LIST ==== (pEPD = %p)", pEPD);
	
	pLink = pEPD->blChkPtQueue.GetNext();
	while(pLink != &pEPD->blChkPtQueue) 
	{
		pCP = CONTAINING_RECORD(pLink, CHKPT, blLinkage);
		LOGPF(1, "(%p) MsgID=%x; Timestamp=%x", pEPD, pCP->bMsgID, pCP->tTimestamp);
		pLink = pLink->GetNext();
	}
}
#endif

#undef DPF_MODNAME
#define DPF_MODNAME "LookupCheckPoint"

PCHKPT LookupCheckPoint(PEPD pEPD, BYTE bRspID)
{
	CBilink	*pLink;
	PCHKPT	pCP;

	pCP = CONTAINING_RECORD((pLink = pEPD->blChkPtQueue.GetNext()), CHKPT, blLinkage);
	while(pLink != &pEPD->blChkPtQueue) 
	{
		// Look for checkpoint that matches correlator
		if(pCP->bMsgID == bRspID)
		{	
			pLink->RemoveFromList();
			return pCP;		
		}
		// We have passed the spot for this correlator!		
		else if ((bRspID - pCP->bMsgID) & 0x80)
		{				
			LOGPF(1, "(%p) CHECKPOINT NOT FOUND - Later Chkpt found in list (%x)", pEPD, bRspID);
			return NULL;
		}
		else 
		{
			pLink = pLink->GetNext();								// Remove ChkPts prior to the one received
			pCP->blLinkage.RemoveFromList();							// ..target and remove the stale ones.
			ChkPtPool->Release(ChkPtPool, pCP);					// we expect them to complete in order of queue
			pCP = CONTAINING_RECORD(pLink, CHKPT, blLinkage);
		}
	}

	LOGPF(1, "(%p) CHECKPOINT NOT FOUND -  EXHAUSTED LIST W/O MATCH (%x)", pEPD, bRspID);
#ifdef	DEBUG
	DumpChkPtList(pEPD);
#endif
	return NULL;
}

#undef DPF_MODNAME
#define DPF_MODNAME "FlushCheckPoints"

VOID FlushCheckPoints(PEPD pEPD)
{
	PCHKPT	pCP;

	while(!pEPD->blChkPtQueue.IsEmpty())
	{
		pCP = CONTAINING_RECORD(pEPD->blChkPtQueue.GetNext(), CHKPT, blLinkage);
		pCP->blLinkage.RemoveFromList();
		ChkPtPool->Release(ChkPtPool, pCP);
	}
}

/*
**		Process End Of Stream
**
**		Our partner has initiated an orderly link termination.  He will not be
**	sending us any more data.  We are allowed to finish sending any data in our
**	pipeline, but should not allow any new sends to be accepted.  When our send
**	pipeline is emptied,  we should send an EOS frame and take down our
**	link.  Easiest way to do this is enqueue the EOS at the end of send queue now.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ProcessEndOfStream"

VOID ProcessEndOfStream(PEPD pEPD)
{
	PMSD	pMSD;

	Lock(&pEPD->EPLock);
	
	LOGPF(7, "(%p) Process EndOfStream", pEPD);

	// Have we started closing on this side?
	if((pEPD->ulEPFlags & EPFLAGS_TERMINATING)==0)
	{			
		// Our side has not started closing yet,  so our partner must have initiated a Disconnect.
		// We are allowed to finish sending all data in our pipeline,  but we should not accept
		// any new data.  We must deliver an indication to the application telling him that
		// Disconnection is now underway.
		//
		// Please note that we do not set the TERMINATING flag until the Disconnecting indication
		// returns.  This allows the application to send any final messages (last words) before
		// the gate is slammed shut.

		LOGPF(7, "(%p) Partner Disconnect received (refcnt=%d)", pEPD, pEPD->uiRefCnt);

		Unlock(&pEPD->EPLock);

		LOGPF(3, "(%p) Calling Core->IndicateDisconnect", pEPD);
		pEPD->pSPD->pPData->pfVtbl->IndicateDisconnect(pEPD->pSPD->pPData->Parent, pEPD->Context);

		Lock(&pEPD->EPLock);
		
		pEPD->ulEPFlags |= (EPFLAGS_TERMINATING | EPFLAGS_RECEIVED_DISCONNECT);	

		if((pEPD->ulEPFlags & EPFLAGS_SENT_DISCONNECT)==0)
		{
			if((pMSD = BuildDisconnectFrame(pEPD)) == NULL)
			{	
				DropLink(pEPD);									// DROPLINK will release SendQLock for us
				return;
			}

			pEPD->ulEPFlags |= EPFLAGS_SENT_DISCONNECT;

			LOCK_EPD(pEPD, "LOCK (DISC RESP)");					// Add reference for this frame
			pMSD->CommandID = COMMAND_ID_DISC_RESPONSE;			// Mark MSD so we know its not a user command
			pMSD->ulSendFlags = DN_SENDFLAGS_RELIABLE | DN_SENDFLAGS_LOW_PRIORITY; // Priority is LOW so all previously submitted traffic will be sent

			LOGPF(7, "(%p) Responding to Disconnect. pMSD=0x%p", pEPD, pMSD);
			EnqueueMessage(pMSD, pEPD);							// Enqueue the DISC frame at end of sendQ
		}

		//	We will fall thru to the already closing case below.  We do this because lots might have happened while we
		// were in our IndicateDisconnect call above.  Like he might have called Disconnect API and hung around a while
		// so that our DISC frame was acknowledged already.  This would mean we are ready to Complete the Disco
	}

	// This side is already closing.  There are three ways that this can happen:
	//
	// 1 - We originated the Disconnect and our parner is now done transmitting
	// 2 - Both sides Disconnected and Disc frames crossed	
	// 3 - We are in the process of aborting our link
	//
	// 1 & 2 can be handled identically.  Case 3 can be ignored, we are dropping abnormally anyway.
	//
	//	THE TRICKY PART  --  What if this EOS frame's ACK gets lost?  We will not respond to retries
	//		because we will have closed the endpoint when the link was dropped.  This leaves partner
	//		flapping in the breeze until he times out and drops abnormally.
	// Possible solutions:
	//		* we can achieve Pretty Good results by always xmitting the final ACK 3 times.  This
	//		fails 1/8000 with the %5 drop rate.
	//		* we can delay disconnecting the endpoint for several seconds (some multiple of RTT). This
	// 		allows us to respond to EOS retries (reset delay timer each time a retry is sent).  This
	//		is only a problem if user immediately re-connects from the same port... doesnt have to be
	//		a problem,  actually,  just a special case.
	//
	//	And the answer is -- second solution is more work but more robust.  I will implement easy
	//		solution for the time being,  but will upgrade to better solution if resource permits.
	//
	//  To tell the truth,  first solution is worse. 1-in-8000 assumes an even distribution of packet
	//	loss,  which is almost certainly not the reality...
	//
	//  But look,  if we respond to frames with DISC when we are unconnected then the problem goes away...
	//
	//	** EPD->EPLOCK is Held at this point

	pEPD->ulEPFlags |= EPFLAGS_RECEIVED_DISCONNECT;		// Note that we have seen partner's DISC

	if(pEPD->ulEPFlags & EPFLAGS_DISCONNECT_ACKED)
	{		
		// He has seen our discon,  we are free to close

		// This is the typical Close path for the initiating side...
		pEPD->ulEPFlags |= EPFLAGS_DELAY_ACKNOWLEDGE;

		// CODEWORK -- Handle THE TRICKY PART as described above...

		pMSD = pEPD->pCommand;
		pEPD->pCommand = NULL;
		PSPD pSPD = pEPD->pSPD;      					// Keep the SPD for use below

		SendAckFrame(pEPD, 1);							// Make sure we transmit an ack for this
														// This unlocks the EPLock since param 2 is 1
														 
		ASSERT_MSD(pMSD);								// Its Guaranteed to be here
		ASSERT(pMSD->CommandID == COMMAND_ID_DISCONNECT);

		pMSD->blQLinkage.RemoveFromList();
		
		ASSERT(pEPD->blHighPriSendQ.IsEmpty());
		ASSERT(pEPD->blNormPriSendQ.IsEmpty());
		ASSERT(pEPD->blLowPriSendQ.IsEmpty());

		Lock(&pMSD->CommandLock);						// Must be held for the following call

		CompleteReliableSend(pSPD, pMSD, DPN_OK);
	}
	else 
	{
		Unlock(&pEPD->EPLock);
	}
}

/*
**		Process Disconnected Frame
**
**			We have received a DISCONNECTED frame which means that the link to
**		this End Point is closed.  This may be a response to our DISCONNECT
**		frame or it might be a unilateral signal of an abnormally aborted link.
**		Doesnt really matter either way-  the conversation is over.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ProcessDisconnectedFrame"

VOID ProcessDisconnectedFrame(PSPD pSPD, PEPD pEPD, PCFRAME pCFrame)
{
	PMSD	pMSD;

	if(pCFrame->dwSessID != pEPD->dwSessID)
	{
		LOGPF(7, "(%p) Received DISCONNECTED frame with incorrect SessID. (%x)", pEPD, pCFrame->dwSessID);
		if(pEPD->ulEPFlags & EPFLAGS_END_POINT_DORMANT)
		{
			LOGPF(7, "(%p) Rejecting Connection on dormant Endpoint", pEPD);
			RejectConnection(pEPD);
		}
		return;
	}

	Lock(&pEPD->EPLock);

	// Did we ever achieve connected status?
	if(pEPD->ulEPFlags & (EPFLAGS_CONNECTED | EPFLAGS_CONNECTING))
	{	
		LOGPF(7, "(%p) Disconnect Frame Received, Dropping Link", pEPD);
		DropLink(pEPD);										// releases SendQLock
	}
	else 
	{
		LOGPF(7, "(%p) Disconnect Frame Received on unconnected link, Rejecting Connection", pEPD);
		Unlock(&pEPD->EPLock);
		RejectConnection(pEPD);
	}
}

/*
**		Process SP Disconnect
**
**		Service Provider has told us that an endpoint has gone away.  This is probably
**	because we have Disconnected it ourselves, in which case the IN_USE flag will be
**	clear.  Otherwise,  we need to clean this thing up ourselves...
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ProcessSPDisconnect"

VOID
ProcessSPDisconnect(PSPD pSPD, PSPIE_DISCONNECT pDataBlock)
{
	PEPD 	pEPD = static_cast<PEPD>( pDataBlock->pEndpointContext );

	LOGPF(2, "Parameters: pSPD[%p], pDataBlock[%p] - pEPD[%p]", pSPD, pDataBlock, pEPD);

	if(pEPD->ulEPFlags & EPFLAGS_END_POINT_IN_USE)
	{
		if(pEPD->ulEPFlags & EPFLAGS_CONNECTED)
		{
			LOGPF(5, "(%p) SP DROPS CONNECTED ENDPOINT", pEPD);
			Lock(&pEPD->EPLock);
			DropLink(pEPD);								// DropLink releases SendQLock
		}
		else
		{
			LOGPF(5, "(%p) SP DROPS DISCONNECTED ENDPOINT", pEPD);
		}
	}
	else 
	{
		LOGPF(5, "(%p) SP REPORTS CONNECTION CLOSED", pEPD);
	}
}

/*
**		Process Listen Status
**
**		This call tells us that a submitted Listen command has become active.  Truth is, we
**	dont care.  We are just interested in seeing the Connect indications as they arrive.  What we
**	do care about,  however, is the Endpoint handle associated with this listen in case we are
**	later asked about the address associated with the listen.  So we will pull it out of the
**	data block and save it in our MSD.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ProcessListenStatus"

VOID
ProcessListenStatus(PSPD pSPD, PSPIE_LISTENSTATUS pDataBlock)
{
	PMSD	pMSD;
	
	LOGPF(2, "Parameters: pSPD[%p], pDataBlock[%p] - hr=%x", pSPD, pDataBlock, pDataBlock->hResult);

	pMSD = (PMSD) pDataBlock->pUserContext;

	ASSERT_MSD(pMSD);
	ASSERT(pMSD->CommandID == COMMAND_ID_LISTEN);

	pMSD->hListenEndpoint = pDataBlock->hEndpoint;

	LOGPF(3, "Calling Core->CompleteListen, pMSD[%p]", pMSD);
	pSPD->pPData->pfVtbl->CompleteListen(pSPD->pPData->Parent, &pMSD->Context, pDataBlock->hResult, pDataBlock->hEndpoint);
	
	if(pDataBlock->hResult != DPN_OK)
	{
		Lock(&pMSD->CommandLock);
		RELEASE_MSD(pMSD, "Release On Complete");
	}

	return;
}

/*
**		Process Connect Address Info
**
**		This call tells us what addressing information has been used to start a connect.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ProcessConnectAddressInfo"

VOID
ProcessConnectAddressInfo(PSPD pSPD, PSPIE_CONNECTADDRESSINFO pDataBlock)
{
	PMSD	pMSD;
	
	pMSD = (PMSD) pDataBlock->pCommandContext;

	LOGPF(2, "Parameters: pSPD[%p], pDataBlock[%p] - pMSD[%p]", pSPD, pDataBlock, pMSD);

	ASSERT_MSD(pMSD);
	ASSERT(pMSD->CommandID == COMMAND_ID_CONNECT);

	LOGPF(3, "Calling Core->AddressInfoConnect");
	pSPD->pPData->pfVtbl->AddressInfoConnect( pSPD->pPData->Parent,
											  pMSD->Context,
											  pDataBlock->hCommandStatus,
											  pDataBlock->pHostAddress,
											  pDataBlock->pDeviceAddress );
	
	return;
}


/*
**		Process Enum Address Info
**
**		This call tells us what addressing information has been used to start an enum.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ProcessEnumAddressInfo"

VOID
ProcessEnumAddressInfo(PSPD pSPD, PSPIE_ENUMADDRESSINFO pDataBlock)
{
	PMSD	pMSD;
	
	pMSD = (PMSD) pDataBlock->pCommandContext;

	LOGPF(2, "Parameters: pSPD[%p], pDataBlock[%p] - pMSD[%p]", pSPD, pDataBlock, pMSD);
	
	ASSERT_MSD(pMSD);
	ASSERT(pMSD->CommandID == COMMAND_ID_ENUM );

	LOGPF(3, "Calling Core->AddressInfoEnum");
	pSPD->pPData->pfVtbl->AddressInfoEnum( pSPD->pPData->Parent,
										   pMSD->Context,
										   pDataBlock->hCommandStatus,
										   pDataBlock->pHostAddress,
										   pDataBlock->pDeviceAddress );
	
	return;
}

/*
**		Process Listen Address Info
**
**		This call tells us what addressing information has been used to start a listen.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ProcessListenAddressInfo"

VOID
ProcessListenAddressInfo(PSPD pSPD, PSPIE_LISTENADDRESSINFO pDataBlock)
{
	PMSD	pMSD;
	
	pMSD = (PMSD) pDataBlock->pCommandContext;

	LOGPF(2, "Parameters: pSPD[%p], pDataBlock[%p] - pMSD[%p]", pSPD, pDataBlock, pMSD);

	ASSERT_MSD(pMSD);
	ASSERT(pMSD->CommandID == COMMAND_ID_LISTEN );

	LOGPF(3, "Calling Core->AddressInfoListen");
	pSPD->pPData->pfVtbl->AddressInfoListen( pSPD->pPData->Parent,
											 pMSD->Context,
											 pDataBlock->hCommandStatus,
											 pDataBlock->pDeviceAddress );
	
	return;
}

/*
**		Reject Frame
**
**		We have received a frame that is outside of our receive window.  It is most
**	likely a retransmitted frame which we ACK'd,  but the ACK was lost.  Best thing
**	we can do is send another ACK pretty soon.
**
**	** CALLED WITH EPLOCK, LEAVES WITH EPLOCK RELEASED **
*/

#undef DPF_MODNAME
#define DPF_MODNAME "RejectFrame"

VOID
RejectFrame(PEPD pEPD, PDFRAME pFrame)
{
	pEPD->ulEPFlags |= EPFLAGS_DELAY_ACKNOWLEDGE;

	if(pFrame->bCommand & PACKET_COMMAND_POLL)
	{
		// Is he asking for an immediate response
		LOGPF(7, "(%p) Sending Ack Frame", pEPD);
		SendAckFrame(pEPD, 1); 						// This unlocks the EPLock since param 2 is 1
	}
	else if(pEPD->DelayedAckTimer == 0)
	{	
		// If timer is not running better start it now
		LOCK_EPD(pEPD, "LOCK (DelayedAckTimer)");								// Bump RefCnt for new timer
		LOGPF(7, "(%p) Setting Delayed Ack Timer", pEPD);
		SetMyTimer(SHORT_DELAYED_ACK_TIMEOUT, 0, DelayedAckTimeout, (PVOID) pEPD, &pEPD->DelayedAckTimer, &pEPD->DelayedAckTimerUnique);
		Unlock(&pEPD->EPLock);		
	}
	else
	{
		Unlock(&pEPD->EPLock);		
	}
}

/*************************************
**
**		RECEIVE BUFFER MANAGEMENT
**
**		When multiple frame messages arrive we *may* have to copy them in to a  single contiguous
**	buffer.  We are supporting an OPTIONAL scatter-receive option which would allow sophisticated
**	clients to receive a BUFFER_DESCRIPTOR array instead of a single cont-buffer,  and avoiding
**	a large datacopy.
**
**		For clients which dont support scatter-receive,  we need a pooling strategy for large receive
**	buffers.  We will only need buffers LARGER then our frame limit because smaller receives are delivered
**	in the SPs buffer.
**
**		Try placing receives into generally sized buffers.  If frame size is usu 1.4K bytes, 2K is a small
**	buffer, 4K, 16K, 32K, 64K.  If frame size is <1K we can have 1K buffers too.
**
**
*************************************/


#undef DPF_MODNAME
#define DPF_MODNAME "GetReceiveBuffer"

VOID
GetReceiveBuffer(UINT size)
{
}


/***********************
========SPACER==========
************************/

/*
**		RCD Pool support routines
**
**		These are the functions called by Fixed Pool Manager as it handles RCDs.
*/

#define	pELEMENT		((PRCD) pElement)

#undef DPF_MODNAME
#define DPF_MODNAME "RCD_Allocate"

BOOL RCD_Allocate(PVOID pElement)
{
	pELEMENT->blOddFrameLinkage.Initialize();
	pELEMENT->blCompleteLinkage.Initialize();
	pELEMENT->Sign = RCD_SIGN;

	return TRUE;
}

//	Get is called each time an MSD is used

#undef DPF_MODNAME
#define DPF_MODNAME "RCD_Get"

VOID RCD_Get(PVOID pElement)
{
	LOGPF(5, "CREATING RCD %p", pELEMENT);

	pELEMENT->pRcvBuff = NULL;
	pELEMENT->uiRefCnt = 1;
	pELEMENT->ulRFlags = 0;
}

//	RCD Release  --  This release call will release an entire chain of RCDs
// 	that are linked together... or then again maybe not.

#undef DPF_MODNAME
#define DPF_MODNAME "RCD_Release"

VOID RCD_Release(PVOID pElement)
{
	ASSERT_RCD(pELEMENT);

	LOGPF(5, "RELEASING RCD %p", pELEMENT);
}

#undef DPF_MODNAME
#define DPF_MODNAME "RCD_Free"

VOID RCD_Free(PVOID pElement)
{
}

#undef	pELEMENT

/*
**		Buffer pool support
**
**
*/

#define	pELEMENT		((PBUF) pElement)

#undef DPF_MODNAME
#define DPF_MODNAME "Buf_Allocate"

BOOL	Buf_Allocate(PVOID pElement)
{
	pELEMENT->Type = RBT_PROTOCOL_BUFFER;

	return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "Buf_Get"

VOID Buf_Get(PVOID pElement)
{
	pELEMENT->Owner = BufPool;
}

#undef DPF_MODNAME
#define DPF_MODNAME "Buf_GetMed"

VOID Buf_GetMed(PVOID pElement)
{
	pELEMENT->Owner = MedBufPool;
}

#undef DPF_MODNAME
#define DPF_MODNAME "Buf_GetBig"

VOID Buf_GetBig(PVOID pElement)
{
	pELEMENT->Owner = BigBufPool;
}

#undef	pELEMENT


