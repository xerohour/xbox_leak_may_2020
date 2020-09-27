/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		Command.c
 *  Content:	This file contains code which implements assorted APIs for the
 *				DirectNet protocol.  APIs implemented in this file are: CANCEL_COMMAND
 *				DISCONNECT_CONNECTION,  ABORT_CONNECTION.
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *  11/06/98	ejs		Created
 *
 ****************************************************************************/


#include "dnproti.h"

#undef		DPF_MODNAME
#define		DPF_MODNAME		"PROTOCOL"

VOID		AbortDatagramSend(PMSD, HRESULT);

/*
**		Cancel Command
**
**			This procedure is passed a HANDLE returned from a previous asynchronous
**	DPLAY command.  At the moment,  the handle is a pointer to an internal data
**	structure.  Problem with this is that due to FPM's (fixed pool manager) design
**	they will get recycled very quickly and frequently.  We might want to map them
**	into an external handle table which will force them to recycle much more slowly.
**	Perhaps,  I will let the upper DN layer do this mapping...
**
**		Anyway,  the only check I can do right now is that the HANDLE is currently
**	allocated to something.
**
**		We do not expect cancels to happen very often.  Therefore,  I do not feel
**	bad about walking the global command list to find the Handle.  Of course,  if
**	we do go to a handle mapped system then we should not need to do this walk.
**
**	I THINK - That any cancellable command will be on either MessageList or TimeoutList!
**
**		Things we can cancel and their possible states:
**
**		SEND Datagram
**			On SPD Send Queue
**			On EPD Send Queue
**			In SP call
**			
**		SEND Reliable
**			We can only cancel if it has not started transmitting.  Once its started, the
**				user program must Abort the link to cancel the send.
**
**		CONNECT
**			In SP call
**			On PD list
**
**		LISTEN
**			In SP call
**			On PD list
**
**		Remember,  if we cancel a command in SP then the CommandComplete is supposed to
**	occur.  This means that we should not have to explicitly free the MSD, etc in these
**	cases.
*/


#undef DPF_MODNAME
#define DPF_MODNAME "DNPCancelCommand"

HRESULT
DNPCancelCommand(PProtocolData pPData,  HANDLE hCommand)
{
	PMSD	pMSD = (PMSD) hCommand;
	HRESULT	hr;

	LOGPF(2, "Parameters: pPData[%p], hCommand[%x]", pPData, hCommand);

	if(pMSD->Sign != MSD_SIGN)
	{
		LOGPF(0, "Cancel called with invalid handle");
		return DPNERR_INVALIDHANDLE;
	}

	Lock(&pMSD->CommandLock);								// Take this early to freeze state of command
	
	// validate instance of MSD
	if(pMSD->uiRefCnt == -1)
	{	
		LOGPF(0, "Cancel called with invalid handle");
		Unlock(&pMSD->CommandLock);
		return DPNERR_INVALIDHANDLE;
	}

	hr = DoCancel(pMSD, DPNERR_USERCANCEL);

	return hr;
}


/*
**		Do Cancel
**
**		This function implements the meat of the cancel asynch operation.  It gets called from
**	two places.  Either from the User cancel API right above,  or from the global timeout handler.
**	This code requires the MSD->CommandLock to be help upon entry.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DoCancel"

HRESULT
DoCancel(PMSD pMSD, HRESULT CompletionCode)
{
	PMSD	pMSD1;
	PFMD	pFMD;
	PEPD	pEPD;
	HANDLE	hCommand;
	DWORD	dwDesc;
	HRESULT	hr = DPN_OK;

	LOGPF(7, "Cancelling pMSD=%p", pMSD);

	if((pMSD->ulMsgFlags1 & MFLAGS_ONE_CANCELLED) || (pMSD->ulMsgFlags2 & MFLAGS_TWO_COMPLETE))
	{
		LOGPF(7, "(%p) MSD is Cancelled or Complete, returning DPNERR_CANNOTCANCEL MSD=%p", pMSD->pEPD, pMSD);
		Unlock(&pMSD->CommandLock);
		return DPNERR_CANNOTCANCEL;
	}

	pMSD->ulMsgFlags1 |= MFLAGS_ONE_CANCELLED;
	
	switch(pMSD->CommandID)
	{
		case COMMAND_ID_SEND_DATAGRAM:

			pEPD = pMSD->pEPD;
			
			Lock(&pEPD->EPLock);
			
			if(pMSD->ulMsgFlags2 & (MFLAGS_TWO_ABORT | MFLAGS_TWO_TRANSMITTING))
			{				
				LOGPF(7, "(%p) MSD is Aborted or Transmitting, returning DPNERR_CANNOTCANCEL MSD=%p", pMSD->pEPD, pMSD);
				Unlock(&pEPD->EPLock);					// Link is dropping or DNET is terminating
				hr = DPNERR_CANNOTCANCEL;						// To cancel an xmitting reliable send you
				break;											// must Abort the connection.
			}
			
			pMSD->blQLinkage.RemoveFromList();							// Remove from SendQueue (whichever one)

			ASSERT(pEPD->uiQueuedMessageCount > 0);
			--pEPD->uiQueuedMessageCount;								// keep count of MSDs on all send queues

			// Clear data-ready flag if everything is sent
			if((pEPD->uiQueuedMessageCount == 0) && (pEPD->pCurrentSend == NULL))
			{	
				pEPD->ulEPFlags &= ~(EPFLAGS_SDATA_READY);
			}

			ASSERT(pMSD->ulMsgFlags2 & MFLAGS_TWO_ENQUEUED);
			pMSD->ulMsgFlags2 &= ~(MFLAGS_TWO_ENQUEUED);

			Unlock(&pEPD->EPLock);						// release lock on queue

			LOGPF(7, "(%p) Aborting Datagram send MSD=%p", pEPD, pMSD);

			AbortDatagramSend(pMSD, CompletionCode);
			return hr;
			
/**

			// CODE WORK -- We can improve the datagram CANCEL code by allowing a partially transmitted DG to be
			//	cancelled. There are two steps to this.  First,  we can try to cancel a Send that is submitted but
			//  not completed,  something which may be useful in limited situations,  AND second we can abort a DG send
			//  if all frames have not been submitted to the SP yet. In this case,  we would have to modify the receive code
			//  to pitch a partial DG receive if a new message is started before the partial message is completed.  It may
			//  very well work this way alredy.  but we would certainly want to verify it...


				Unlock(&pEPD->SendQLock);					// Release lock on EPD's send queues
				
				pFMD = CONTAINING_RECORD(pMSD->blFrameList.GetNext(), FMD, blMSDLinkage);
				ASSERT_FMD(pFMD);
				LOCK_FMD(pFMD);

				if(pFMD->bSubmitted){
					hCommand = pFMD->SendDataBlock.hCommand;
					dwDesc = pFMD->SendDataBlock.dwCommandDescriptor;

					if(hCommand != NULL){
						Unlock(&pMSD->CommandLock);						// Release lock on command structure
						LOGPF(3, "Submitting Cancel on DG pFMD=%p", pFMD);
						hr = IDP8ServiceProvider_CancelCommand(pMSD->pSPD->IISPIntf, hCommand, dwDesc);
						RELEASE_FMD(pFMD);

						return hr;
					}
				}
				
				hr = DPNERR_CANNOTCANCEL;
				RELEASE_FMD(pFMD);
				break;
**/			
			
		case COMMAND_ID_SEND_RELIABLE:
		
			pEPD = pMSD->pEPD;
			
			Lock(&pEPD->EPLock);
			
			if(pMSD->ulMsgFlags2 & (MFLAGS_TWO_ABORT | MFLAGS_TWO_TRANSMITTING))
			{				
				LOGPF(7, "(%p) MSD is Aborted or Transmitting, returning DPNERR_CANNOTCANCEL MSD=%p", pMSD->pEPD, pMSD);
				Unlock(&pEPD->EPLock);						// Link is dropping or DNET is terminating
				hr = DPNERR_CANNOTCANCEL;						// To cancel an xmitting reliable send you
				break;											// must Abort the connection.
			}
			
			if(pMSD->TimeoutTimer != NULL)
			{
				LOGPF(7, "(%p) Cancelling Timeout Timer", pEPD);
				if(CancelMyTimer(pMSD->TimeoutTimer, pMSD->TimeoutTimerUnique) == DPN_OK)
				{
					pMSD->TimeoutTimer = NULL;
					DECREMENT_MSD(pMSD, "Send Timeout Timer");
				}
				else
				{
					LOGPF(7, "(%p) Cancelling Timeout Timer Failed", pEPD);
				}
			}
			
			pMSD->blQLinkage.RemoveFromList();							// Remove cmd from queue

			ASSERT(pEPD->uiQueuedMessageCount > 0);
			--pEPD->uiQueuedMessageCount;								// keep count of MSDs on all send queues

			// Clear data-ready flag if everything is sent
			if((pEPD->uiQueuedMessageCount == 0) && (pEPD->pCurrentSend == NULL))
			{	
				pEPD->ulEPFlags &= ~(EPFLAGS_SDATA_READY);
			}

			ASSERT(pMSD->ulMsgFlags2 & MFLAGS_TWO_ENQUEUED);
			pMSD->ulMsgFlags2 &= ~(MFLAGS_TWO_ENQUEUED);

			// This only gets complex if the cancelled send was the "on deck" send for the endpoint.
			//
			// New Logic!  With advent of priority sending,  we can no longer prepare the On Deck send before we are
			// ready to transmit since the arrival of a higher priority send should be checked for before starting to
			// send a new message. This means that pCurrentSend != pMSD unless the MFLAGS_TRANSMITTING flag has also
			// been set,  rendering the message impossible to cancel.

			ASSERT(pEPD->pCurrentSend != pMSD);
			
			Unlock(&pEPD->EPLock);

			LOGPF(7, "(%p) Completing Reliable Send", pEPD);
			CompleteReliableSend(pMSD->pSPD, pMSD, CompletionCode);
			
			return hr;
			
		case COMMAND_ID_CONNECT:
			
			if(pMSD->ulMsgFlags1 & MFLAGS_ONE_IN_SERVICE_PROVIDER)
			{
				// SP owns the command - issue a cancel and let CompletionEvent clean up command
				
				Unlock(&pMSD->CommandLock);				// We could deadlock if we cancel with lock held

				LOGPF(3, "Calling SP CancelCommand on Connect, pMSD[%p]", pMSD);
				(void) IDP8ServiceProvider_CancelCommand(pMSD->pSPD->IISPIntf, pMSD->hCommand, pMSD->dwCommandDesc);
				
					// If the SP Cancel fails it should not matter.  It would usually mean we are
					// in a race with the command completing,  in which case the cancel flag will
					// nip it in the bud.

				return DPN_OK;
			}

			// Once we are past the call to SP,  we are either waiting on a timer or processing
			// a timer event.  If waiting on timer,  we can cancel timer and clean up.  If cancel
			// indicates that event will run,  then we bail out and let the timer event clean up.
			// If the event was processing already,  then we would have blocked on CommandLock so
			// it will be waiting for the next event by the time we reach this code.

			pEPD = pMSD->pEPD;
			Lock(&pEPD->EPLock);
			
			if (pEPD->ConnectTimer != NULL)
			{
				LOGPF(5, "(%p) Cancelling Connect Timer", pEPD);
				if(CancelMyTimer(pEPD->ConnectTimer, pEPD->ConnectTimerUnique) == DPN_OK)
				{
					pEPD->ConnectTimer = 0;
					RELEASE_EPD(pEPD, "UNLOCK: (Free Connect Timer in CancelConnect)");	// Remove reference for timer, SendQLock not already held
				}
				else
				{
					LOGPF(5, "(%p) Cancelling Connect Timer Failed", pEPD);
				}
			}

			if(pMSD->TimeoutTimer != NULL)
			{
				LOGPF(5, "Cancelling Timeout Timer, pMSD[%p]", pMSD);
				if(CancelMyTimer(pMSD->TimeoutTimer, pMSD->TimeoutTimerUnique) == DPN_OK)
				{
					pMSD->TimeoutTimer = NULL;
					DECREMENT_MSD(pMSD, "Connect Timeout Timer");
				}
				else
				{
					LOGPF(5, "Cancelling Timeout Timer Failed, pMSD[%p]", pMSD);
				}
			}

			if(pEPD->pCommand != NULL)
			{
				pEPD->pCommand = NULL;
				DECREMENT_MSD(pMSD, "EPD Ref");
			}

			Unlock(&pEPD->EPLock);
			
			LOGPF(5, "Completing Connect, pMSD[%p]", pMSD);
			CompleteConnect(pMSD, pMSD->pSPD, pMSD->pEPD, CompletionCode); // releases command lock

			return DPN_OK;
			
		case COMMAND_ID_LISTEN:

			/*
			**		Cancel Listen
			**
			**		SP will own parts of the MSD until the SPCommandComplete function is called.  We will
			**	defer much of our cancel processing to this handler.
			*/

			// Stop listening in SP -- This will prevent new connections from popping up while we are
			// closing down any left in progress.  Only problem is we need to release command lock to
			// do it.

			Unlock(&pMSD->CommandLock);								// We can deadlock if we hold across this call

			LOGPF(3, "Calling SP CancelCommand on Listen, pMSD[%p]", pMSD);
			if (FAILED(IDP8ServiceProvider_CancelCommand(pMSD->pSPD->IISPIntf, pMSD->hCommand, pMSD->dwCommandDesc)))
			{
				LOGPF(6, "Calling our own CommandComplete since SP->CancelCommand failed");
				(void) DNSP_CommandComplete((IDP8SPCallback *) pMSD->pSPD, NULL, CompletionCode, (PVOID) pMSD);
			}

			Lock(&pMSD->CommandLock);								// Lock this down again.
			
			// Are there any connections in progress?
			while(!pMSD->blFrameList.IsEmpty())
			{				
				pEPD = CONTAINING_RECORD(pMSD->blFrameList.GetNext(), EPD, blSPLinkage);

				LOGPF(1, "FOUND CONNECT IN PROGRESS ON CANCELLED LISTEN, EPD=%p", pEPD);

				Lock(&pEPD->EPLock);

				ASSERT(pEPD->pCommand == pMSD);						// This should be pointing back to this listen

				pEPD->ulEPFlags |= (EPFLAGS_TERMINATING | EPFLAGS_ABORT);
				pEPD->ulEPFlags &= ~(EPFLAGS_CONNECTING);

				LOGPF(7, "(%p) Cancelling Connect Timer", pEPD);
				if(CancelMyTimer(pEPD->ConnectTimer, pEPD->ConnectTimerUnique) == DPN_OK)
				{
					pEPD->ConnectTimer = 0;							// mark timer as cancelled
					RELEASE_EPD(pEPD, "UNLOCK (Free Retry Timer in CancelListen)");		// remove reference for timer, SendQLock not already held

					if(pEPD->pCommand != NULL)
					{
						pEPD->pCommand = NULL;
						DECREMENT_MSD(pMSD, "EPD Ref");				// Unlink from EPD and release associated reference
					}
				}
				else
				{
					LOGPF(7, "(%p) Cancelling Connect Timer Failed", pEPD);
				}

				if(pEPD->ulEPFlags & EPFLAGS_LINKED_TO_LISTEN)
				{
					pEPD->ulEPFlags &= ~EPFLAGS_LINKED_TO_LISTEN;
					pEPD->blSPLinkage.RemoveFromList();						// Unlink EPD from Listen Queue
				}
				
				SendDisconnectedFrame(pEPD);						// Tell partner that we are giving up
				
				Unlock(&pEPD->EPLock);
				
				KillConnection(pEPD);								// Make it go away...
			}	// for each connection in progress
			
			RELEASE_MSD(pMSD, "Release On Cancel");					// release base reference
			
			return DPN_OK;
	
		case COMMAND_ID_ENUM:
		{
			Unlock(&pMSD->CommandLock);

			LOGPF(3, "Calling SP->CancelCommand on Enum, pMSD[%p]", pMSD);
			return IDP8ServiceProvider_CancelCommand(pMSD->pSPD->IISPIntf, pMSD->hCommand, pMSD->dwCommandDesc);
			
			// We will pass HRESULT from SP directly to user
		}
		case COMMAND_ID_ENUMRESP:			
		{
			Unlock(&pMSD->CommandLock);

			LOGPF(3, "Calling SP->CancelCommand on EnumResp, pMSD[%p]", pMSD);
			return IDP8ServiceProvider_CancelCommand(pMSD->pSPD->IISPIntf, pMSD->hCommand, pMSD->dwCommandDesc);
			
			// We will pass HRESULT from SP directly to user
		}

		case COMMAND_ID_DISCONNECT:
		case COMMAND_ID_COPIED_RETRY:		// This should be on FMD's only
		case COMMAND_ID_CFRAME:				// This should be on FMD's only
		case COMMAND_ID_DISC_RESPONSE:		// These are never placed on the global list and aren't cancellable
		case COMMAND_ID_KEEPALIVE:			// These are never placed on the global list and aren't cancellable
		default:
			ASSERT(0);		// Should never get here
			break;
	}

	Unlock(&pMSD->CommandLock);
	
	return hr;
}


/*
**		Get Listen Info
**
**		Return a buffer full of interesting and provokative tidbits about a particular Listen command.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DNPGetListenAddressInfo"

HRESULT
DNPGetListenAddressInfo(HANDLE hCommand, PSPGETADDRESSINFODATA pSPData)
{
	PMSD	pMSD = (PMSD) hCommand;
	HRESULT	hr = DPNERR_INVALIDHANDLE;

	LOGPF(2, "Parameters: hCommand[%x], pSPData[%p]", hCommand, pSPData);

	ASSERT(pMSD != NULL);
	ASSERT_MSD(pMSD);

	if((pMSD->CommandID == COMMAND_ID_LISTEN) && (pMSD->ulMsgFlags1 & MFLAGS_ONE_IN_SERVICE_PROVIDER))
	{
		pSPData->hEndpoint = pMSD->hListenEndpoint;

		LOGPF(3, "Calling SP->GetAddressInfo");
		hr = IDP8ServiceProvider_GetAddressInfo(pMSD->pSPD->IISPIntf, pSPData);
	}

	return hr;
}

/*
**		Validate End Point
**
**		This routine checks standard flags,  validates the Service
**	Provider,  and bumps the reference count on an end point descriptor
**	which is passed in.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ValidateEndPoint"

HRESULT
ValidateEndPoint(PProtocolData pPData,  PEPD pEPD)
{
	PSPD	pSPD;

	if(pEPD == NULL)
		return DPNERR_INVALIDENDPOINT;
		
	ASSERT_EPD(pEPD);
	
	// Bump reference count on this baby
	if(!LOCK_EPD(pEPD, "LOCK (ValidateEndPoint)"))
	{
		LOGPF(1, "Validate EndPoint Fails on unreferenced EPD (%p)", pEPD);
		return DPNERR_INVALIDENDPOINT;
	}

	if( (pEPD->ulEPFlags & (EPFLAGS_END_POINT_IN_USE | EPFLAGS_CONNECTED)) !=
							(EPFLAGS_END_POINT_IN_USE | EPFLAGS_CONNECTED))
	{
		RELEASE_EPD(pEPD, "UNLOCK (Validate EP fails)"); // SendQLock not already held
		LOGPF(1, "Validate Endpoint Fails on unconnected EP (%p)", pEPD);
		return DPNERR_INVALIDENDPOINT;
	}

	pSPD = pEPD->pSPD;
	
	return DPN_OK;
}

/*
**		Disconnect End Point
**
**		This function is called when the client no longer wishes
**	to communicate with the specified end point.  We will initiate
**	the disconnect protocol with the endpoint,  and when it is
**	acknowleged,  we will disconnect the SP and release the handle.
**
**		Disconnect is defined in Direct Net to allow all previously
**	submitted sends to complete,  but no additional sends to be submitted.
**	Also, any sends the partner has in progress will be delivered,  but
**	no additional sends will be accepted following the indication that
**	a disconnect is in progress on the remote end.
**
**		This implies that two indications will be generated on the remote
**	machine,  Disconnect Initiated and Disconnect Complete.  Only the
**	Complete will be indicated on the issueing side.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DNPDisconnectEndPoint"

HRESULT
DNPDisconnectEndPoint(PProtocolData pPData,  HANDLE hEndPoint, PVOID pvContext, PHANDLE phCommand)
{
	PEPD	pEPD;
	PMSD	pMSD;
	HRESULT	hr;

	LOGPF(2, "Parameters: pPData[%p], hEndPoint[%x], pvContext[%p], phCommand[%p]", pPData, hEndPoint, pvContext, phCommand);

	if((pEPD = (PEPD) hEndPoint) == NULL)
	{
		LOGPF(0, "Attempt to disconnect NULL endpoint");
		return DPNERR_INVALIDENDPOINT;
	}

	ASSERT_EPD(pEPD);

	// This bumps REFCNT
	if((hr = ValidateEndPoint(pPData, pEPD)) != DPN_OK)
	{
		LOGPF(0, "Attempt to disconnect invalid endpoint");
		return hr;
	}

	Lock(&pEPD->EPLock);

	if(pEPD->ulEPFlags & (EPFLAGS_TERMINATING | EPFLAGS_SENT_DISCONNECT))
	{
		RELEASE_EPD(pEPD, "UNLOCK (Disconnect EP fails)"); // SendQLock not already held
		Unlock(&pEPD->EPLock);
		LOGPF(1, "Attempt to disconnect already disconnecting endpoint");
		return DPNERR_ALREADYDISCONNECTING;
	}

	pEPD->ulEPFlags |= (EPFLAGS_TERMINATING | EPFLAGS_SENT_DISCONNECT); 	// Accept no more sends, but dont scrap link yet

	if((pMSD = BuildDisconnectFrame(pEPD)) == NULL)
	{
		Unlock(&pEPD->EPLock);
		RELEASE_EPD(pEPD, "UNLOCK (no memory)"); // SendQLock not already held
		LOGPF(0, "Failed to build disconnect frame");
		return DPNERR_OUTOFMEMORY;								// The educated user will next try an Abort command
	}
	
	pMSD->CommandID = COMMAND_ID_DISCONNECT;
	pMSD->ulSendFlags = DN_SENDFLAGS_RELIABLE | DN_SENDFLAGS_LOW_PRIORITY; // Priority is LOW so all previously submitted traffic will be sent
	pMSD->pSPD = pEPD->pSPD;
	pMSD->Context = pvContext;									// retain user's context value
	*phCommand = pMSD;											// pass back command handle

	Lock(&pMSD->pSPD->SendQLock);
	pMSD->blSPLinkage.InsertBefore( &pMSD->pSPD->blMessageList);
	pMSD->ulMsgFlags1 |= MFLAGS_ONE_ON_GLOBAL_LIST;// BUGBUG: Is command lock held?
	Unlock(&pMSD->pSPD->SendQLock);

	LOGPF(5, "(%p) Queueing DISCONNECT message", pEPD);
	EnqueueMessage(pMSD, pEPD);									// Enqueue Disc frame on SendQ
	
	Unlock(&pEPD->EPLock);
	
	return DPNERR_PENDING;
}

/*
**		Get/Set Protocol Caps
**
**		Return or Set information about the entire protocol.
**
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DNPGetProtocolCaps"

HRESULT
DNPGetProtocolCaps(PProtocolData pPData, PDPN_CAPS pData)
{
	LOGPF(2, "Parameters: pPData[%p], pData[%p]", pPData, pData);
	
	ASSERT(pData->dwSize == sizeof(DPN_CAPS));
	ASSERT(pData->dwFlags == 0);

	pData->dwConnectTimeout = pPData->dwConnectTimeout;
	pData->dwConnectRetries = pPData->dwConnectRetries;
	pData->dwTimeoutUntilKeepAlive = pPData->tIdleThreshhold;
	
	return DPN_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "DNPSetProtocolCaps"

HRESULT
DNPSetProtocolCaps(PProtocolData pPData, const DPN_CAPS * const pData)
{
	LOGPF(2, "Parameters: pPData[%p], pData[%p]", pPData, pData);

	ASSERT(pData->dwSize == sizeof(DPN_CAPS));
	ASSERT(pData->dwFlags == 0);
	
	pPData->dwConnectTimeout = pData->dwConnectTimeout;
	pPData->dwConnectRetries = pData->dwConnectRetries;
	pPData->tIdleThreshhold = pData->dwTimeoutUntilKeepAlive;

	return DPN_OK;
}

/*
**		Get Endpoint Caps
**
**		Return information and statistics about a particular endpoint.
**
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DNPGetEPCaps"

HRESULT
DNPGetEPCaps(HANDLE hEndpoint, PDPN_CONNECTION_INFO pBuffer)
{
	PEPD	pEPD;

	LOGPF(6, "Parameters: hEndpoint[%x], pBuffer[%p]", hEndpoint, pBuffer);

	pEPD = (PEPD) hEndpoint;
	
	if(pEPD == NULL || !(pEPD->ulEPFlags & EPFLAGS_CONNECTED))
	{
		LOGPF(0, "Returning DPNERR_INVALIDENDPOINT - hEndpoint is NULL or Enpoint is not connected");
		return DPNERR_INVALIDENDPOINT;
	}
	if(pBuffer == NULL)
	{
		LOGPF(0, "Returning DPNERR_INVALIDPARAM - pBuffer is NULL");
		return DPNERR_INVALIDPARAM;
	}
	
	ASSERT_EPD(pEPD);
	ASSERT(pBuffer->dwSize == sizeof(DPN_CONNECTION_INFO));

    pBuffer->dwSize = sizeof(DPN_CONNECTION_INFO);
    pBuffer->dwRoundTripLatencyMS = pEPD->uiRTT;
    pBuffer->dwThroughputBPS = pEPD->uiPeriodRateB * 4;				// Convert to apx of bytes/second (really bytes/1024 ms)
    pBuffer->dwPeakThroughputBPS = pEPD->uiPeakRateB * 4;

	pBuffer->dwBytesSentGuaranteed = pEPD->uiGuaranteedBytesSent;
	pBuffer->dwPacketsSentGuaranteed = pEPD->uiGuaranteedFramesSent;
	pBuffer->dwBytesSentNonGuaranteed = pEPD->uiDatagramBytesSent;
	pBuffer->dwPacketsSentNonGuaranteed = pEPD->uiDatagramFramesSent;

	pBuffer->dwBytesRetried = pEPD->uiGuaranteedBytesDropped;
	pBuffer->dwPacketsRetried = pEPD->uiGuaranteedFramesDropped;
	pBuffer->dwBytesDropped = pEPD->uiDatagramBytesDropped;
	pBuffer->dwPacketsDropped = pEPD->uiDatagramFramesDropped;

	pBuffer->dwMessagesTransmittedHighPriority = pEPD->uiMsgSentHigh;
	pBuffer->dwMessagesTimedOutHighPriority = pEPD->uiMsgTOHigh;
	pBuffer->dwMessagesTransmittedNormalPriority = pEPD->uiMsgSentNorm;
	pBuffer->dwMessagesTimedOutNormalPriority = pEPD->uiMsgTONorm;
	pBuffer->dwMessagesTransmittedLowPriority = pEPD->uiMsgSentLow;
	pBuffer->dwMessagesTimedOutLowPriority = pEPD->uiMsgTOLow;

	pBuffer->dwBytesReceivedGuaranteed = pEPD->uiGuaranteedBytesReceived;
	pBuffer->dwPacketsReceivedGuaranteed = pEPD->uiGuaranteedFramesReceived;
	pBuffer->dwBytesReceivedNonGuaranteed = pEPD->uiDatagramBytesReceived;
	pBuffer->dwPacketsReceivedNonGuaranteed = pEPD->uiDatagramFramesReceived;
		
	pBuffer->dwMessagesReceived = pEPD->uiMessagesReceived;

	return DPN_OK;
}

/*		
**		Build Disconnect Frame
**
**		Build a DISC frame, a Message actually, because we return an MSD which can be inserted into
**	our reliable stream and will trigger one-side of the disconnect protocol when it is received
**	by a partner.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "BuildDisconnectFrame"

PMSD
BuildDisconnectFrame(PEPD pEPD)
{
	PFMD	pFMD;
	PMSD	pMSD;

	// Allocate and fill out a Message Descriptor for this operation
	
	if( (pMSD = static_cast<PMSD>( MSDPool->Get(MSDPool) )) == NULL)
	{
		LOGPF(0, "Failed to allocate MSD");
		return NULL;
	}

	pMSD->uiFrameCount = 1;
	pMSD->ulMsgFlags2 |= MFLAGS_TWO_END_OF_STREAM;
	pMSD->pEPD = pEPD;
	
	if((pFMD = static_cast<PFMD>( FMDPool->Get(FMDPool) )) == NULL)
	{
		LOGPF(0, "Failed to allocate FMD");
		Lock(&pMSD->CommandLock);
		RELEASE_MSD(pMSD, "Release On FMD Get Failed");
		return NULL;
	}

	pFMD->CommandID = COMMAND_ID_SEND_RELIABLE;
	pFMD->ulFFlags |= FFLAGS_END_OF_STREAM;								// Mark this frame as Disconnect
	pFMD->bPacketFlags = PACKET_COMMAND_DATA | PACKET_COMMAND_RELIABLE | PACKET_COMMAND_SEQUENTIAL | PACKET_COMMAND_END_MSG;
	pFMD->uiFrameLength = 0;											// No user data in this frame
	pFMD->blMSDLinkage.InsertAfter( &pMSD->blFrameList);				// Attach frame to MSD
	pFMD->pMSD = pMSD;													// Link frame back to message
	pFMD->pEPD = pEPD;

	return pMSD;
}

/*
**		Abort End Point
**
**		This function closes an EndPoint without performing the
**	disconnect protocol.  This allows the data structures to
**	be cleaned up immediately,  and therefore completes
**	synchronously.  We will attempt to notify the remote endpoint
**	although we dont guarentee delivery.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DNPAbortEndPoint"

HRESULT
DNPAbortEndPoint(PProtocolData pPData,  HANDLE hEndPoint)
{
	PEPD	pEPD;
	HRESULT	hr;

	pEPD = (PEPD) hEndPoint;

	LOGPF(2, "Paramters: pPData[%p], hEndPoint[%x]", pPData, hEndPoint);
	
	// This will add a reference to the endpoint
	if((hr = ValidateEndPoint(pPData, pEPD)) != DPN_OK)
	{		
		LOGPF(0, "(%p) Attempt to Abort invalid Endpoint", pEPD);
		return hr;
	}

	Lock(&pEPD->EPLock);

	pEPD->ulEPFlags |= EPFLAGS_TERMINATING;						// Accept no more sends, but dont scrap link yet
	pEPD->ulEPFlags &= ~(EPFLAGS_CONNECTED);
	
	LOGPF(5, "(%p) Sending Disconnected Frame", pEPD);
	SendDisconnectedFrame(pEPD);							// Tell partner that we've buggered off

	LOGPF(5, "(%p) Dropping Link", pEPD);
	DropLink(pEPD);											// releases SendQLock
	
	RELEASE_EPD(pEPD, "UNLOCK (Abort done)");				// Release THIS reference, SendQLock not already held

	return DPN_OK;
}

/*
**		Abort Sends on Connection
**
**		Walk the EPD's send queues and cancel all sends awaiting service.  We might add
**	code to issue Cancel commands to the SP for frames still owned by SP.  On one hand,
**	we are not expecting a big backlog to develop in SP,  but on the other hand it still
**	might happen.  Esp, if we dont fix behavior I have observed with SP being really pokey
**	about completing transmitted sends.
**
**	**  CALLED WITH EPD->SENDQLOCK HELD;  RETURNS WITH LOCK RELEASED  **
*/

#undef DPF_MODNAME
#define DPF_MODNAME "AbortSendsOnConnection"

VOID
AbortSendsOnConnection(PEPD pEPD)
{
	PFMD	pFMD;
	PMSD	pMSD;
	CBilink	*pLink;
	CBilink	TempList;
	PVOID	Context;
	UINT	counter;

	TempList.Initialize();										// We will empty all send queues onto this temporary list

	do 
	{
		if( (pLink = pEPD->blHighPriSendQ.GetNext()) == &pEPD->blHighPriSendQ)
		{
			if( (pLink = pEPD->blNormPriSendQ.GetNext()) == &pEPD->blNormPriSendQ)
			{
				if( (pLink = pEPD->blLowPriSendQ.GetNext()) == &pEPD->blLowPriSendQ)
				{
					if( (pLink = pEPD->blCompleteSendList.GetNext()) == &pEPD->blCompleteSendList)
					{
						break;										// ALL DONE - No more sends
					}
				}
			}
		}

		// We have found another send on one of our send queues.

		pLink->RemoveFromList();											// Remove it from the queue
		pMSD = CONTAINING_RECORD(pLink, MSD, blQLinkage);
		ASSERT(pMSD);
		pMSD->ulMsgFlags2 |= MFLAGS_TWO_ABORT;						// Do no further processing
		pMSD->ulMsgFlags2 &= ~(MFLAGS_TWO_ENQUEUED);

		LOGPF(5, "ABORT SENDS.  Found 0x%p", pMSD);

		pMSD->blQLinkage.InsertBefore( &TempList);				// Place on the temporary list
	} 
	while (1);

	pEPD->uiQueuedMessageCount = 0;								// keep count of MSDs on all send queues

	if((pMSD = pEPD->pCommand) != NULL)
	{
		// There may be a DISCONNECT command waiting on this special pointer for the final DISC frame
		// from partner to arrive

		if(pMSD->CommandID == COMMAND_ID_DISCONNECT)
		{
			pEPD->pCommand = NULL;

			pMSD->blQLinkage.InsertBefore( &TempList);
		}
	}

	//	If we clear out our SendWindow before we cancel the sends,  then we dont need to differentiate
	//	between sends that have or have not been transmitted.

	while(!pEPD->blSendWindow.IsEmpty())
	{
		pFMD = CONTAINING_RECORD(pEPD->blSendWindow.GetNext(), FMD, blWindowLinkage);
		pFMD->ulFFlags &= ~(FFLAGS_IN_SEND_WINDOW);
		pEPD->blSendWindow.GetNext()->RemoveFromList();						// Eliminate each frame from the Send Window
		RELEASE_FMD(pFMD);
		LOGPF(5, "(%p) ABORT CONN:  Release frame from Window: pFMD=0x%p", pEPD, pFMD);
	}
	
	pEPD->pCurrentSend = NULL;
	pEPD->pCurrentFrame = NULL;

	while(!pEPD->blRetryQueue.IsEmpty())
	{
		pFMD = CONTAINING_RECORD(pEPD->blRetryQueue.GetNext(), FMD, blQLinkage);
		pFMD->blQLinkage.RemoveFromList();
		pFMD->ulFFlags &= ~(FFLAGS_RETRY_QUEUED);				// No longer on the retry queue
		RELEASE_EPD(pEPD, "UNLOCK (Releasing Retry Frame)"); // SendQLock not already held
		RELEASE_FMD(pFMD);
	}
	pEPD->ulEPFlags &= ~(EPFLAGS_RETRIES_QUEUED);
		
	//	Now that we have emptied the EPD's queues we will release the SendQLock,  so we can lock each
	//	MSD before we complete it.
	
	Unlock(&pEPD->EPLock);

	while(!TempList.IsEmpty())
	{
		pMSD = CONTAINING_RECORD(TempList.GetNext(), MSD, blQLinkage);
		pMSD->blQLinkage.RemoveFromList();							// remove this send from temporary queue
		Lock(&pMSD->CommandLock);							// Complete call will Unlock MSD

		if((pMSD->ulMsgFlags1 & MFLAGS_ONE_IN_USE) == 0)
		{
			ASSERT(0);
			Unlock(&pMSD->CommandLock);
		}
		else if (pMSD->CommandID == COMMAND_ID_SEND_RELIABLE)
		{
			CompleteReliableSend(pMSD->pSPD, pMSD, DPNERR_CONNECTIONLOST);
		}
		else if (pMSD->CommandID == COMMAND_ID_SEND_DATAGRAM) 
		{
			AbortDatagramSend(pMSD, DPNERR_CONNECTIONLOST);
		}
		else if (pMSD->CommandID == COMMAND_ID_KEEPALIVE) 
		{
			RELEASE_EPD(pEPD, "UNLOCK (Aborted KeepAlive)"); // SendQLock not already held
			RELEASE_MSD(pMSD, "Release On Abort");
		}
		else if (pMSD->CommandID == COMMAND_ID_DISCONNECT)
		{
			Lock(&pMSD->pSPD->SendQLock);
			if(pMSD->ulMsgFlags1 & MFLAGS_ONE_ON_GLOBAL_LIST)
			{
				pMSD->blSPLinkage.RemoveFromList();					// Remove MSD from master command list
				pMSD->ulMsgFlags1 &= ~(MFLAGS_ONE_ON_GLOBAL_LIST);			
			}
			RELEASE_EPD_LOCKED(pMSD->pEPD, "UNLOCK (ABORT DISC COMMAND)");	// release hold on EPD for this send, SendQLock is held already
			Unlock(&pMSD->pSPD->SendQLock);
			Context = pMSD->Context;
			RELEASE_MSD(pMSD, "Release On Complete");			// Return resources,  including all frames
			LOGPF(3, "Calling Core->CompleteDisconnect - DPNERR_CONNECTIONLOST");
			pMSD->pSPD->pPData->pfVtbl->CompleteDisconnect(pMSD->pSPD->pPData->Parent, Context, DPNERR_CONNECTIONLOST);
		}
		else if (pMSD->CommandID == COMMAND_ID_DISC_RESPONSE)
		{
			RELEASE_EPD(pEPD, "UNLOCK (ABORT DISC RESP)"); // SendQLock not already held
			RELEASE_MSD(pMSD, "Release On Abort");
		}
		else 
		{
			LOGPF(0, "UNKNOWN COMMAND FOUND ON SEND Q");	
			ASSERT(0);
		}
	}
}

/*
**		Abort Datagram Send
**
**
**	THIS IS ENTERED WITH MSD->COMMANDLOCK HELD
*/

#undef DPF_MODNAME
#define DPF_MODNAME "AbortDatagramSend"

VOID
AbortDatagramSend(PMSD pMSD, HRESULT CompletionCode)
{
	PFMD	pFMD;
	CBilink	*pLink;

	if(pMSD->TimeoutTimer != NULL)
	{
		LOGPF(7, "Cancelling Timeout Timer");
		if(CancelMyTimer(pMSD->TimeoutTimer, pMSD->TimeoutTimerUnique) == DPN_OK)
		{
			pMSD->TimeoutTimer = NULL;
			DECREMENT_MSD(pMSD, "Timeout Timer");
		}
		else
		{
			LOGPF(7, "Cancelling Timeout Timer Failed");
		}
	}
					
	pLink = pMSD->blFrameList.GetNext();

	while(pLink != &pMSD->blFrameList)
	{
		pFMD = CONTAINING_RECORD(pLink, FMD, blMSDLinkage);
		pLink = pLink->GetNext();
		if((pFMD->ulFFlags & FFLAGS_TRANSMITTED)==0)
		{
			pFMD->blMSDLinkage.RemoveFromList();
			RELEASE_FMD(pFMD);
			pMSD->uiFrameCount--;
		}
	}

	if(pMSD->blFrameList.IsEmpty())
	{
		CompleteDatagramSend(pMSD->pSPD, pMSD, CompletionCode);
	}
	else
	{
		Unlock(&pMSD->CommandLock);
	}
}

#undef DPF_MODNAME
#define DPF_MODNAME "SetLinkParms"

VOID SetLinkParms(PEPD pEPD, PINT Data)
{
	if(Data[0])
	{
		pEPD->iGoodWindowF = pEPD->iWindowF = Data[0];
		pEPD->uiGoodWindowBI = pEPD->uiWindowBIndex = Data[0];
		
		pEPD->iWindowB = pEPD->uiWindowBIndex * pEPD->pSPD->uiFrameLength;
		LOGPF(7, "** ADJUSTING WINDOW TO %d FRAMES", Data[0]);
	}
	if(Data[1])
	{
		pEPD->uiGoodBurstSize = pEPD->uiBurstSize = Data[1];
		LOGPF(7, "** ADJUSTING BURST SIZE TO %d FRAMES", Data[1]);
	}
	if(Data[2])
	{
		pEPD->uiGoodBurstGap = pEPD->uiBurstGap = Data[2];
		LOGPF(7, "** ADJUSTING GAP TO %d ms", Data[2]);
	}
#ifdef POLL_COUNT	
	if(Data[3])
	{
		pEPD->uiPollFrequency = Data[3];
		LOGPF(7, "** ADJUSTING POLL FREQ TO %d", Data[3]);
	}
#endif

	pEPD->uiPeriodAcksBytes = 0;
	pEPD->uiPeriodXmitTime = 0;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DNP_Debug"

HRESULT WINAPI DNP_Debug(PProtocolData pPData, UINT OpCode, HANDLE EndPoint, PVOID Data)
{
	PEPD	pEPD = (PEPD) EndPoint;

	switch(OpCode){
		case	1:
			// Inquire current RTT calculation for Endpoint
			*((UINT *) Data) = pEPD->uiRTT;
			break;

		case	5:
			/* Manually set link parameters */
			SetLinkParms(pEPD, (int *) Data);
			break;

		case	6:
			/* Toggle Dynamic/Static Link control */

			pEPD->ulEPFlags ^= EPFLAGS_LINK_STABLE;
			pEPD->ulEPFlags ^= EPFLAGS_LINK_FROZEN;
			break;

		default:
			return DPNERR_GENERIC;
	}

	return DPN_OK;
}


