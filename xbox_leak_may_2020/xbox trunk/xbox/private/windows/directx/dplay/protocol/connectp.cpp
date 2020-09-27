/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		Connect.c
 *  Content:	This file contains support for the CONNECT/DISCONNECT protocol in DirectNet.
 *				It is organized with FrontEnd routines first (Connect, Listen),
 *				and Backend handlers (timeouts,  frame crackers) after.
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *  11/11/98	ejs		Created
 *	03/22/00	ejs		Changed address structures to Dplay8Address
 *
 ****************************************************************************/


#include "dnproti.h"

/***  FRONT END  ***/

/*
**		Connect
**
**			This function attempts to make a connection to a specified address.
**		The function establishes the existance of a DirectNet entity and maps
**		an EndPoint handle.  Then we exchange CONNECT packets which allows each
**		side to establish a baseline RTT.
**
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DNPConnect"

HRESULT DNPConnect(	PProtocolData pPData,
							IDirectPlay8Address *const lpaLocal,
							IDirectPlay8Address *const lpaRemote,
							//IDP8ServiceProvider* pISP,
							const HANDLE hSPHandle,
							const DWORD dwTimeout,
							const ULONG ulFlags,
							void *const Context,
							PHANDLE phHandle)
{
	PMSD			pMSD;
	SPCONNECTDATA	ConnData;					// Parameter Block
	PSPD			pSPD;						// Service Provider to handle this connect
	HRESULT			hr;
	UINT			i;


	// Determine which SP will take this call
	//

	LOGPF(2, "Parameters: pPData[%p], paLocal[%p], paRemote[%p], hSPHandle[%x], dwTimeout[%x], ulFlags[%x], Context[%p], phHandle[%p]", pPData, lpaLocal, lpaRemote, hSPHandle, dwTimeout, ulFlags, Context, phHandle);

	pSPD = (PSPD) hSPHandle;
	ASSERT(pSPD != NULL);
	
	if(pSPD == NULL)
	{
		LOGPF(0, "Returning DPNERR_NOTREADY - hSPHandle is NULL");
		return DPNERR_NOTREADY;
	}

	ASSERT_SPD(pSPD);

	if(pSPD->ulSPFlags & SPFLAGS_TERMINATING)
	{
		LOGPF(0, "Returning DPNERR_NOTREADY - SP is terminating");
		return DPNERR_NOTREADY;
	}

	// We use an MSD to describe this op even though it isn't technically a message
	if((pMSD = static_cast<PMSD>( MSDPool->Get(MSDPool) )) == NULL)
	{	
		LOGPF(0, "Returning DPNERR_OUTOFMEMORY - failed to create new MSD");
		return DPNERR_OUTOFMEMORY;	
	}

	pMSD->CommandID = COMMAND_ID_CONNECT;
	pMSD->pSPD = pSPD;
	pMSD->Context = Context;
	pMSD->tSubmitTime = GETTIMESTAMP();
	pMSD->pEPD = NULL;

	// Prepare to call SP to map the endpoint.
	ConnData.pAddressDeviceInfo = lpaLocal;
	ConnData.pAddressHost = lpaRemote;
	ConnData.dwTimeout = dwTimeout;

	ConnData.dwFlags = 0;

	ConnData.pvContext = pMSD;
	ConnData.hCommand = 0;

	// Hook up MSD before calling into SP
	Lock(&pMSD->pSPD->SendQLock);
	pMSD->blSPLinkage.InsertBefore( &pMSD->pSPD->blMessageList);	// Put this on cmd list, not timer list
	pMSD->ulMsgFlags1 |= MFLAGS_ONE_ON_GLOBAL_LIST;
	pMSD->ulMsgFlags1 |= MFLAGS_ONE_IN_SERVICE_PROVIDER;
	Unlock(&pMSD->pSPD->SendQLock);

	*phHandle = pMSD;

	// SP Connect call is guarenteed to return immediately

	LOCK_MSD(pMSD, "SP Ref");												// Add reference for call into SP

	LOGPF(3, "Calling SP->Connect");
/**/hr = IDP8ServiceProvider_Connect(pSPD->IISPIntf, &ConnData);		/** CALL SP **/

	if(hr != DPNERR_PENDING)
	{
		// SP Connect should always be asynchronous so if it isnt PENDING then it must have failed
		LOGPF(0, "SP->Connect did not return DPNERR_PENDING, assuming failure, hr[%x]", hr);

		ASSERT(hr != DPN_OK);

		Lock(&pMSD->CommandLock);								// This will be unlocked by final RELEASE_MSD

		pMSD->ulMsgFlags1 &= ~(MFLAGS_ONE_IN_SERVICE_PROVIDER);		// clear InSP flag
		pMSD->ulMsgFlags1 |= MFLAGS_ONE_CANCELLED;				// In case timer cannot be cancelled

		Lock(&pMSD->pSPD->SendQLock);
		pMSD->blSPLinkage.RemoveFromList();								// knock this off the pending list
		pMSD->ulMsgFlags1 &= ~(MFLAGS_ONE_ON_GLOBAL_LIST);
		Unlock(&pMSD->pSPD->SendQLock);

		DECREMENT_MSD(pMSD, "SP Ref");									// Remove one ref for SP call
		RELEASE_MSD(pMSD, "Release On Fail");							// Remove one ref to free resource

		LOGPF(0, "Returning hr[%x]", hr);
		return hr;
	}

	pMSD->hCommand = ConnData.hCommand;							// retain SP command handle
	pMSD->dwCommandDesc = ConnData.dwCommandDescriptor;

	// This cannot be called until after pMSD->hCommand and pMSD->dwCommandDesc are
	// valid or it will try to cancel with bogus handles for the SP
	if(dwTimeout != 0)
	{
		LOGPF(5, "Setting Connect Timeout Timer");
		LOCK_MSD(pMSD, "Connect Timeout Timer");			// Add reference for timer
		SetMyTimer(dwTimeout, 100, TimeoutConnect, pMSD, &pMSD->TimeoutTimer, &pMSD->TimeoutTimerUnique);
	}

	LOGPF(2, "Returning DPNERR_PENDING, pMSD[%p]", pMSD);
	return DPNERR_PENDING;
}

/*
**		Listen
**
**		This command tells DN that it should start to accept connection requests.
**	This command will return pending,  and will continue to indicate connections
**	until it is explicitly cancelled.  It may be desireable to establish a limit
**	mechanism of some sort,  but for the time being this will do.
**
**		Now it is desirable to Listen on multiple ports on a single adapter.  This
**	means that we need to accept multiple concurrent Listen commands on each adapter.
**	Another fact of life is that we need to crack the Target address far enough to
**	determine which SP to submit the Listen on.
*/



#undef DPF_MODNAME
#define DPF_MODNAME "DNPListen"

HRESULT DNPListen(	PProtocolData pPData,
							IDirectPlay8Address *const lpaTarget,
							// IDP8ServiceProvider* pISP,
							const HANDLE hSPHandle,
							ULONG ulFlags,
							PVOID Context,
							PHANDLE phHandle)
{
	PSPD			pSPD;
	PMSD			pMSD;
	SPLISTENDATA	ListenData;
	UINT			i;
	HRESULT			hr;

	LOGPF(2, "Parameters: pPData[%p], paTarget[%p], hSPHandle[%x], ulFlags[%x], Context[%p], phHandle[%p]", pPData, lpaTarget, hSPHandle, ulFlags, Context, phHandle);

	pSPD = (PSPD) hSPHandle;
	
	ASSERT(pSPD != NULL);
	if(pSPD == NULL)
	{
		LOGPF(0, "Returning DPNERR_NOTREADY - hSPHandle is NULL");
		return DPNERR_NOTREADY;
	}

	ASSERT_SPD(pSPD);

	if(pSPD->ulSPFlags & SPFLAGS_TERMINATING)
	{
		LOGPF(0, "Returning DPNERR_NOTREADY - SP is terminating");
		return DPNERR_NOTREADY;
	}

	// We use an MSD to describe this op even though it isn't technically a message
	if((pMSD = static_cast<PMSD>( MSDPool->Get(MSDPool) )) == NULL)
	{	
		LOGPF(0, "Returning DPNERR_OUTOFMEMORY - failed to create new MSD");
		return DPNERR_OUTOFMEMORY;	
	}

	pMSD->CommandID = COMMAND_ID_LISTEN;
	pMSD->pSPD = pSPD;
	pMSD->Context = Context;

	ListenData.pAddressDeviceInfo = lpaTarget;
	ListenData.dwFlags = 0;
	ListenData.pvContext = pMSD;
	ListenData.hCommand = 0;

	*phHandle = pMSD;

	// SP Listen call is guarenteed to return immediately
	Lock(&pSPD->SendQLock);
	pSPD->ulSPFlags |= SPFLAGS_LISTENING;
	pMSD->blQLinkage.InsertBefore( &pSPD->blListenList);

	pMSD->blSPLinkage.InsertBefore( &pMSD->pSPD->blMessageList);		// Dont support timeouts for Listen
	pMSD->ulMsgFlags1 |= MFLAGS_ONE_ON_GLOBAL_LIST;
	pMSD->ulMsgFlags1 |= MFLAGS_ONE_IN_SERVICE_PROVIDER;
	Unlock(&pMSD->pSPD->SendQLock);

	LOCK_MSD(pMSD, "SP Ref");											// AddRef for SP

	LOGPF(3, "Calling SP->Listen");
/**/hr = IDP8ServiceProvider_Listen(pSPD->IISPIntf, &ListenData);		/** CALL SP **/

	if(hr != DPNERR_PENDING){

		// SP Listen should always be asynchronous so if it isnt PENDING then it must have failed
		LOGPF(0, "SP->Listen did not return DPNERR_PENDING, assuming failure, hr[%x]", hr);

		ASSERT(hr != DPN_OK);

		Lock(&pSPD->SendQLock);

		pMSD->blQLinkage.RemoveFromList();

		if(pSPD->blListenList.IsEmpty())
		{
			pSPD->ulSPFlags &= ~(SPFLAGS_LISTENING);
		}
		
		Unlock(&pSPD->SendQLock);

		Lock(&pMSD->CommandLock);

		Lock(&pSPD->SendQLock);
		pMSD->blSPLinkage.RemoveFromList();					// knock this off the pending list
		pMSD->ulMsgFlags1 &= ~(MFLAGS_ONE_ON_GLOBAL_LIST);
		Unlock(&pSPD->SendQLock);

		DECREMENT_MSD(pMSD, "SP Ref");						// release once for SP
		RELEASE_MSD(pMSD, "Release On Fail");				// release again to return resource

		LOGPF(0, "Returning hr[%x]", hr);
		return hr;
	}

	pMSD->hCommand = ListenData.hCommand;			// retail SP command handle
	pMSD->dwCommandDesc = ListenData.dwCommandDescriptor;

	LOGPF(2, "Returning DPNERR_PENDING, pMSD[%p]", pMSD);
	return DPNERR_PENDING;
}

/***  BACKEND ROUTINES  ***/

/*
**		Complete Connect
**
**		The user's Connect operation has completed.  Clean everything up
**	and signal the user.
**
**		THIS IS ALWAYS CALLED WITH THE COMMAND LOCK HELD IN MSD
*/

#undef DPF_MODNAME
#define DPF_MODNAME "CompleteConnect"

VOID CompleteConnect(PMSD pMSD, PSPD pSPD, PEPD pEPD, HRESULT hr)
{
	PVOID	Context;

	if(pMSD->TimeoutTimer != NULL)
	{
		LOGPF(5, "Cancelling Connect Timeout Timer");
		if(CancelMyTimer(pMSD->TimeoutTimer, pMSD->TimeoutTimerUnique) == DPN_OK)
		{
			DECREMENT_MSD(pMSD, "Connect Timeout Timer");
			pMSD->TimeoutTimer = NULL;
		}
		else
		{
			LOGPF(5, "Cancelling Connect Timeout Timer Failed");
		}
	}

	Lock(&pSPD->SendQLock);
	if(pMSD->ulMsgFlags1 & MFLAGS_ONE_ON_GLOBAL_LIST)
	{
		pMSD->blSPLinkage.RemoveFromList();							// Remove MSD from master command list
		pMSD->ulMsgFlags1 &= ~(MFLAGS_ONE_ON_GLOBAL_LIST);
	}
	Unlock(&pSPD->SendQLock);

	if(pEPD)
	{
		if(hr != DPN_OK)
		{
			LOGPF(0, "SP returned Connect failed, hr[%x]", hr);
			KillConnection(pEPD);
			pEPD = NULL;
		}
	}
	Context = pMSD->Context;
	RELEASE_MSD(pMSD, "Final Release On Complete");					// Finished with this one

	if(pEPD)
	{
		LOGPF(3, "Calling Core->CompleteConnect");
		pSPD->pPData->pfVtbl->CompleteConnect(pSPD->pPData->Parent, Context, hr, (PHANDLE) pEPD, (PVOID *) &pEPD->Context);
		Lock(&pEPD->EPLock);
		pEPD->ulEPFlags &= ~(EPFLAGS_CALLING | EPFLAGS_CONNECTING);
		ReceiveComplete(pEPD);								// Complete any
	}
	else
	{
		LOGPF(3, "Calling Core->CompleteConnect with NULL EPD");
		pSPD->pPData->pfVtbl->CompleteConnect(pSPD->pPData->Parent, Context, hr, NULL, NULL);
	}
}

/*
**		Timeout Connect
**
**		Connect Command's user-set timeout has fired.  Need to nip this operation.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "TimeoutConnect"

VOID CALLBACK
TimeoutConnect(PVOID uID, UINT uMsg, PVOID dwUser)
{
	PMSD	pMSD = (PMSD) dwUser;

	Lock(&pMSD->CommandLock);
	
	if( (pMSD->TimeoutTimer == uID)&&(pMSD->TimeoutTimerUnique == uMsg))
	{
		LOGPF(5, "Connect Timed Out");

		pMSD->TimeoutTimer = NULL;

		if((pMSD->ulMsgFlags1 & (MFLAGS_ONE_CANCELLED | MFLAGS_ONE_TIMEDOUT)) || (pMSD->ulMsgFlags2 & MFLAGS_TWO_COMPLETE))
		{
			LOGPF(5, "MSD is cancelled, timed-out, or complete");
			RELEASE_MSD(pMSD, "Connect Timeout Timer");
			return;
		}

		pMSD->ulMsgFlags1 |= MFLAGS_ONE_TIMEDOUT;

		DECREMENT_MSD(pMSD, "Connect Timeout Timer");			// Release Ref for timer

		DoCancel(pMSD, DPNERR_TIMEDOUT);
	}
	else
	{
		LOGPF(5, "Connect Timed Out - Timer already cancelled");
		RELEASE_MSD(pMSD, "Connect Timeout Timer");
	}
}


/*
**		Complete SP Connect
**
**		A Connect Command has completed in the Service Provider.  This does not mean our
**	work is done...  this means we now have a mapped EndPoint so we can exchange packets
**	with this partner.  We will now ping this partner to get an initial RTT and make sure
**	there really is a protocol over there that will talk to us.
**
**		Of course,  if SP does not return success then we can nip this whole thing in
**	the proverbial bud.
**
**		**  COMMAND LOCK is held on entry  **
*/


#undef DPF_MODNAME
#define DPF_MODNAME "CompleteSPConnect"

VOID CompleteSPConnect(PMSD pMSD, PSPD pSPD, HRESULT hr)
{
	PEPD		pEPD;
	PFMD		pFMD;

	LOGPF(5, "SP Completes Connect (Endpoint Opened (0x%p))", pMSD->pEPD);

	if(hr != DPN_OK)
	{
		LOGPF(5, "(%p) Connect was not successful, Complete Connect", pMSD->pEPD);
		CompleteConnect(pMSD, pSPD, NULL, hr);				// SP failed the connect call
		return;
	}

	pEPD = pMSD->pEPD;

	ASSERT_EPD(pEPD);
	ASSERT(pEPD->pCommand == pMSD);

	if(pMSD->ulMsgFlags1 & (MFLAGS_ONE_CANCELLED | MFLAGS_ONE_TIMEDOUT))
	{
		DECREMENT_MSD(pMSD, "SP Ref");						// Release Reference from SP
		
		ASSERT(pEPD->pCommand != NULL);

		pEPD->pCommand = NULL;
		DECREMENT_MSD(pMSD, "EPD Ref");						// Release Reference from EPD
		
		LOGPF(5, "(%p) Command is cancelled or timed out, Complete Connect", pMSD->pEPD);
		CompleteConnect(pMSD, pSPD, pEPD, ((pMSD->ulMsgFlags1 & MFLAGS_ONE_TIMEDOUT) ? DPNERR_TIMEDOUT : DPNERR_USERCANCEL));
		return;
	}


	Lock(&pEPD->EPLock);

	if(pEPD->ulEPFlags & (EPFLAGS_CONNECTING | EPFLAGS_CONNECTED))
	{
		// Otherside beat us to it!   Let him finish...
		LOGPF(5, "SP Completes CONNECT but partner is already in process, Complete Connect");
		Unlock(&pEPD->EPLock);
		CompleteConnect(pMSD, pSPD, NULL, DPNERR_CONNECTING);
		return;
	}

	// Set up End Point Data

	pEPD->ulEPFlags &= ~(EPFLAGS_END_POINT_DORMANT);
	pEPD->ulEPFlags |= EPFLAGS_CONNECTING | EPFLAGS_CALLING;
	pMSD->pEPD = pEPD;										// Link new EPD to command descriptor

	// Send Ping

	pEPD->dwSessID = pSPD->pPData->dwNextSessID++;

	LOGPF(5, "(%p) Sending CONNECT Frame", pEPD);
	if(SendCommandFrame(pEPD, FRAME_EXOPCODE_CONNECT, 0) != DPN_OK)
	{
		pEPD->pCommand = NULL;
		DECREMENT_MSD(pMSD, "SP Ref");								// Release Reference from SP
		Unlock(&pEPD->EPLock);
		LOGPF(5, "(%p) Sending CONNECT Frame Failed, Complete Connect", pEPD);
		CompleteConnect(pMSD, pSPD, pEPD, DPNERR_OUTOFMEMORY);
		return;
	}

	// Set timer for reply,  then wait for reply or TO.  We will do this first against the
	// loopback possibility of the response arriving before the timer is set
	//
	// CODEWORK - we should scale the initial TO according to local linkspeed

	pEPD->uiRetryTimeout = pSPD->pPData->dwConnectTimeout;
	pEPD->uiRetryCount = pSPD->pPData->dwConnectRetries;	

	LOCK_EPD(pEPD, "LOCK (CONN Retry Timer)");						// Create reference for timer
	LOGPF(5, "(%p) Setting Connect Retry Timer", pEPD);
	SetMyTimer(pEPD->uiRetryTimeout, 100, ConnectRetryTimeout, (PVOID) pEPD, &pEPD->ConnectTimer, &pEPD->ConnectTimerUnique);

	Unlock(&pEPD->EPLock);
	Unlock(&pMSD->CommandLock);
}

/*
**		Connect Retry Timeout
**
**		Retry timer has expired on a Connect operation.  This one function
**	is shared by Calling and Listening partners.  Complexity is due to the
**	fact that cancel code cannot always ensure that this handler will not
**	run,  so there are flags to signal various edge conditions (cancel, abort,
**	completion, high-level timeout).
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ConnectRetryTimeout"

VOID CALLBACK
ConnectRetryTimeout(PVOID pvHandle, UINT uiUnique, PVOID pvUser)
{
	PMSD	pMSD;
	PEPD	pEPD = (PEPD) pvUser;
	HRESULT	hr = DPNERR_NORESPONSE;

	LOGPF(5, "ENTER Connect Retry Timeout pEPD=%p", pEPD);

	ASSERT_EPD(pEPD);

	if((pEPD->ConnectTimer != pvHandle)||(pEPD->ConnectTimerUnique != uiUnique))
	{
		// Timer been reset!  This is a spurious fire and should be ignored.
		RELEASE_EPD(pEPD, "UNLOCK: (Spurious (ie late) firing of CONNECT timer)"); // SendQLock not already held
		LOGPF(5, "(%p) Ignoring late CONNECT timer", pEPD);
		return;
	}
	
	pMSD = pEPD->pCommand;

	if(pMSD == NULL)
	{
		pEPD->ConnectTimer = 0;
		RELEASE_EPD(pEPD, "UNLOCK: (CONNECT timer after completion)"); // SendQLock not already held
		return;
	}
	
	ASSERT_MSD(pMSD);

	Lock(&pMSD->CommandLock);
	Lock(&pEPD->EPLock);

	pEPD->ConnectTimer = 0;

	if(		(pMSD->ulMsgFlags1 & MFLAGS_ONE_CANCELLED) || 
			(pMSD->ulMsgFlags2 & (MFLAGS_TWO_COMPLETE | MFLAGS_TWO_ABORT)) || 
			((pEPD->ulEPFlags & EPFLAGS_CONNECTING)==0))
	{

		pEPD->pCommand = NULL;									// Remove EPD Link
		RELEASE_MSD(pMSD, "EPD Ref");							// Remove reference for EPD Link
		Unlock(&pEPD->EPLock);
		RELEASE_EPD(pEPD, "UNLOCK (Conn Retry Timer)");			// Remove reference for timer, SendQLock not already held
		return;													// and thats all for now
	}

	// IF more retries are allowed and command is still active, send another CONNECT frame

	if(	(pEPD->uiRetryCount-- > 0) && ((pMSD->ulMsgFlags1 & MFLAGS_ONE_TIMEDOUT)==0) && ((pMSD->ulMsgFlags2 & MFLAGS_TWO_COMPLETE)==0))
	{	
		pEPD->uiRetryTimeout *= 2;		// exp backoff until we establish our first RTT

		if(pMSD->CommandID == COMMAND_ID_CONNECT)
		{
			LOGPF(5, "(%p) Sending CONNECT Frame", pEPD);
			if((hr = SendCommandFrame(pEPD, FRAME_EXOPCODE_CONNECT, 0)) != DPN_OK)
			{
				LOGPF(5, "(%p) Sending CONNECT Frame Failed", pEPD);
				goto Failure;
			}
		}
		// Listen -- retry CONNECTED frame
		else 
		{
			pEPD->ulEPFlags |= EPFLAGS_CHECKPOINT_INIT;	// We will expect a reply to this frame
			LOGPF(5, "(%p) Sending CONNECTED Frame", pEPD);
			if((hr = SendCommandFrame(pEPD, FRAME_EXOPCODE_CONNECTED, 0)) != DPN_OK)
			{
				LOGPF(5, "(%p) Sending CONNECTED Frame Failed", pEPD);
				goto Failure;
			}
		}

		// Send the next ping
		LOGPF(5, "(%p) Setting Connect Retry Timer", pEPD);
		SetMyTimer(pEPD->uiRetryTimeout, 100, ConnectRetryTimeout, (PVOID) pEPD, &pEPD->ConnectTimer, &pEPD->ConnectTimerUnique);

		Unlock(&pEPD->EPLock);
		Unlock(&pMSD->CommandLock);

		return;											// Since we have re-started timer, we dont adjust refcnt
	}

	/*
	** 	Failure - there are a couple failure scenarios.  Most likely is that we get no response and timed out.
	*/

Failure:

	RELEASE_EPD(pEPD, "UNLOCK: (Connect Timer (Failure Path))");// Dec Ref for this timer, SendQLock not already held

	if(pMSD->CommandID == COMMAND_ID_CONNECT)
	{
		pEPD->pCommand = NULL;
		DECREMENT_MSD(pMSD, "EPD Ref");					// Release Reference from EPD
		
		if (pMSD->ulMsgFlags1 & MFLAGS_ONE_TIMEDOUT)
		{
			hr = DPNERR_TIMEDOUT;
		}
		else if(pMSD->ulMsgFlags1 & MFLAGS_ONE_CANCELLED)
		{
			hr = DPNERR_USERCANCEL;
		}
		Unlock(&pEPD->EPLock);
		LOGPF(5, "Connect Retry Failed, Complete Connect");
		CompleteConnect(pMSD, pEPD->pSPD, pEPD, hr);			// releases CommandLock
	}

	// Listen - clean up associated state info,  then blow away end point
	else 
	{

		LOGPF(5, "Connect Retry Failed on Listen, Kill Connection");

		if(pEPD->ulEPFlags & EPFLAGS_LINKED_TO_LISTEN)
		{
			pEPD->ulEPFlags &= ~(EPFLAGS_LINKED_TO_LISTEN);
			pEPD->blSPLinkage.RemoveFromList();							// Unlink EPD from Listen Queue
		}

		pEPD->pCommand = NULL;										// Unlink listen from EPD
		RELEASE_MSD(pMSD, "EPD Ref");								// release reference for link to EPD and CommandLock
		
		Unlock(&pEPD->EPLock);
		KillConnection(pEPD);									// Remove 'base' reference from End Point
	}
}

/*
**		Process Connection Request
**
**		Somebody wants to connect to us.  If we have a listen posted we will
**	fill out a checkpoint structure to correlate his response and we will fire
**	off a CONNECTED frame ourselves
**
**		In the case of concurrent connection requests from both partners,  the
**  partner with the LOWER Session ID will backoff in favor of the HIGHER session ID.
**	This means that if we receive a lower request we can ignore it.  If we receive a
**	higher request we must abort our CONNECT in progress...
**
**		Since our connection will not be up until we receive a CONNECTED response
**	to our response we will need to set up a retry timer ourselves.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ProcessConnectRequest"

VOID ProcessConnectRequest(PSPD pSPD, PEPD pEPD, PCFRAME pCFrame)
{
	PMSD	pMSD = NULL;

	LOGPF(2, "CONNECT REQUEST RECEIVED; EPD=%p SessID=%x", pEPD, pCFrame->dwSessID);

	// Verify Listen
	//
	//		We have already referenced the Listen MSD in the IndicateConnection code so we know that the
	// listen has not evaporated...  unless,  of course,  this ConnectRequest arrived on an already connected
	// endpoint.  In this case,  the pCommand pointer in the EPD will be NULL indicated that there was no
	// Listen command involved.  Now here is the rub...  We need to make sure that we are still accepting
	// CONNECT commands on the port that this connection came in on,  but when no listen was involved we have
	// no way of knowing this.  It is insufficent to track whether our original Listen is still active because
	// it may or may not have been replaced with an equivalent listen.  The easiest (perhaps only sensible) way
	// to ensure we are still listening on this guy's port is to Disconnect the endpoint (ignoring this CONNECT
	// request) and continue when partner's RETRY triggers an IndicateConnect (or else is never heard from again).

	if((pMSD = pEPD->pCommand) == NULL)
	{
		// This must be a re-init of an existing connection
		Lock(&pEPD->EPLock);
		
		if(pEPD->dwSessID == pCFrame->dwSessID)
		{
			// Partner is retransmitting a CONN request that we have already seen & responded to.
			//
			//		Since he is calling with the correct SessID,  the odds are good that is he re-transmitting
			// because he dropped our original response.

			LOGPF(5, "(%p) CONNECT Frame received on CONNECTED link, Re-Sending CONNECTED Frame", pEPD);
			(void) SendCommandFrame(pEPD, FRAME_EXOPCODE_CONNECTED, pCFrame->bMsgID);	// If this fails we have no remedy...

			Unlock(&pEPD->EPLock);
			return;
		}

		LOGPF(5, "EXISTING CONNECTION RE-INITED BY PARTNER (%p)", pEPD);

		if((pEPD->ulEPFlags & EPFLAGS_TERMINATING)==0){				// Make sure DropLink never gets called more then once
			LOGPF(5, "(%p) Endpoint is terminating, Drop Link", pEPD);
			DropLink(pEPD);											// This call releases EPLock
		}
		else
		{
			Unlock(&pEPD->EPLock);
		}
		DisconnectConnection(pEPD);									// Force this EP closed early!
		return;
	}

	ASSERT_MSD(pMSD);

	Lock(&pMSD->CommandLock);

	Lock(&pEPD->EPLock);											// Serialize access to EPD (this may not really be new sess)

	if(pMSD->CommandID == COMMAND_ID_CONNECT)
	{
		LOGPF(0, "(%p) PROTOCOL RECEIVED CONNECT REQUEST ON A CONNECTING (NOT LISTENING) ENDPOINT, IGNORING", pEPD);
		Unlock(&pEPD->EPLock);
		Unlock(&pMSD->CommandLock);
		return;
	}

	if((pCFrame->dwVersion >> 16) != (DNET_VERSION_NUMBER >> 16))
	{
		LOGPF(0, "(%p) PROTOCOL RECEIVED CONNECT REQUEST FROM AN INCOMPATIBLE VERSION, DROPPING LINK", pEPD);
		Unlock(&pMSD->CommandLock);
		pEPD->ulEPFlags |= EPFLAGS_ABORT;	// This will prevent Core notification for a connection it has never seen
		DropLink(pEPD);		// This releases the EPLock
		return;
	}

	if((pMSD->ulMsgFlags1 & (MFLAGS_ONE_CANCELLED)) || (pMSD->ulMsgFlags2 & MFLAGS_TWO_ABORT))
	{
		LOGPF(0, "(%p) PROTOCOL RECEIVED CONNECT REQUEST ON A COMMAND THAT IS ABORTED OR CANCELLED, DROPPING LINK", pEPD);
		pEPD->ulEPFlags |= EPFLAGS_CONNECTING;						// This will allow proper cleanup to occur in DropLink
		DropLink(pEPD);												// releases SendQLock
		return;														// Not interested in CONNECT frames w/o listen pending
	}

	// If we are already connected to this guy,  then there are two possibilities:
	// There is a left-over Connect frame,  which should be ignored;  or
	// The partner has rebooted and is trying to re-connect,  in which case we need to reset the link

	// If we are already in CONNECTING state then either this is not the first CONNECT frame we have seen,  both sides
	// issued CONNECT commands concurrently,  or maybe partner has ABORTED and re-started with a new ConnID.
	//
	// If the ConnID is consistant then we should send a new CONNECTED response because he probably lost our first one.
	// Although we retry,  he needs a correlatable response to get an accurate RTT and clock Bias.
	//
	// If the ConnID in different then we follow the Concurrent Connect Rules,  which say that the higher SessID will
	// be used.


	if(pEPD->ulEPFlags & EPFLAGS_CONNECTED) 
	{						// Are we already connected?
		// If connection has been completed then we dont need to do more work
		Unlock(&pEPD->EPLock);
		Unlock(&pMSD->CommandLock);
		return;
	}

	if(pEPD->ulEPFlags & EPFLAGS_CONNECTING) 
	{						// Is this not the first frame we've seen?
		if(pEPD->dwSessID != pCFrame->dwSessID)
		{
			// Looks like a simultaneous CONNECT scenario.  High ID wins...

			if(pCFrame->dwSessID < pEPD->dwSessID)
			{
				// Our larger session number wins.  We ignore this guy's request
				LOGPF(1, "(%p) Received synchronous CONNECT request. Lower ID is IGNORED", pEPD);
				Unlock(&pEPD->EPLock);
				Unlock(&pMSD->CommandLock);
				return;
			}
			else 
			{
				// Will use partner's SessID.
				//
				// We will fall thru and let EPD get reset with new SessID and then we
				// will become an eerie hybrid Connect/Listen MSD.

				LOGPF(1, "(%p) Received synchronous CONNECT request. We will YIELD", pEPD);
				
				if(pEPD->ConnectTimer != 0)
				{
					LOGPF(5, "(%p) Cancelling Connect Timer", pEPD);
					if(CancelMyTimer(pEPD->ConnectTimer, pEPD->ConnectTimerUnique) == DPN_OK)
					{
						RELEASE_EPD(pEPD, "UNLOCK (Stop Connect Timer)");	// release lock for timer, SendQLock not already held
						pEPD->ConnectTimer = 0;						// stop timer from previous CONNECT
					}
					else
					{
						LOGPF(5, "(%p) Cancelling Connect Timer Failed", pEPD);
					}
				}
				// FALLTHRU and we will be re-initialized to reflect the new SessID
			}
		} // SESSID !=  CFrame->SESSID
		else 
		{
			// Unexpected CONNECT Frame has same Session ID.  Partner probably lost our response.  We will
			// respond again to this one.
			
			LOGPF(1, "(%p) Received duplicate CONNECT request. Sending another response...", pEPD);

			(void) SendCommandFrame(pEPD, FRAME_EXOPCODE_CONNECTED, pCFrame->bMsgID);
			Unlock(&pEPD->EPLock);
			Unlock(&pMSD->CommandLock);
			return;
		}
	}
	else 
	{
		// This IS a new connection.  We need to hook the EPD up to the Listen.

		ASSERT(pMSD->CommandID == COMMAND_ID_LISTEN);
		ASSERT((pEPD->ulEPFlags & EPFLAGS_LINKED_TO_LISTEN)==0);

		pEPD->blSPLinkage.InsertBefore( &pMSD->blFrameList);
		pEPD->ulEPFlags |= EPFLAGS_LINKED_TO_LISTEN;
		pMSD->pEPD = pEPD;
	}

	pEPD->ulEPFlags |= EPFLAGS_CONNECTING;
	pEPD->ulEPFlags &= ~(EPFLAGS_END_POINT_DORMANT);

	pEPD->dwSessID = pCFrame->dwSessID;							// Use this SessID in all C-traffic
	pEPD->ulEPFlags |= EPFLAGS_CHECKPOINT_INIT;					// We will expect a reply to this frame

	LOGPF(5, "(%p) Sending CONNECTED Frame", pEPD);
	if(SendCommandFrame(pEPD, FRAME_EXOPCODE_CONNECTED, pCFrame->bMsgID) != DPN_OK)
	{
		LOGPF(0, "(%p) Sending CONNECTED Frame Failed", pEPD);
		pEPD->pCommand = NULL;									// Unlock Cmd from EPD

		ASSERT(pEPD->ConnectTimer == NULL);
		
		if(pMSD->CommandID == COMMAND_ID_LISTEN)
		{
			pEPD->ulEPFlags &= ~(EPFLAGS_LINKED_TO_LISTEN);
			pEPD->blSPLinkage.RemoveFromList();
		
			Unlock(&pEPD->EPLock);
			KillConnection(pEPD);
			RELEASE_MSD(pMSD, "EPD Ref");						// releases CommandLock & ref for EPD Link
			return;
		}
		else
		{
			DECREMENT_MSD(pMSD, "EPD Ref");						// release reference for EPD link
			Unlock(&pEPD->EPLock);
			CompleteConnect(pMSD, pSPD, pEPD,  DPNERR_OUTOFMEMORY);
			return;
		}
	}

	pEPD->uiRetryTimeout = pSPD->pPData->dwConnectTimeout;		// Got to start somewhere
	pEPD->uiRetryCount = pSPD->pPData->dwConnectRetries;		// w/exponential wait,  we will have ~22 minutes unless user times out first

	LOCK_EPD(pEPD, "LOCK: (CONNECT RETRY TIMER)");				// Create reference for timer
	LOGPF(5, "(%p) Setting Connect Timer", pEPD);
	SetMyTimer(pEPD->uiRetryTimeout, 100, ConnectRetryTimeout, (PVOID) pEPD, &pEPD->ConnectTimer, &pEPD->ConnectTimerUnique);

	Unlock(&pEPD->EPLock);
	Unlock(&pMSD->CommandLock);
}

/*
**		Process Connected Response
**
**		A response to a connection request has arrived (or a response to
**	our connection response).  Now the connection is officially up (on
**	our end of the circuit).  Set the link-state according to our first
**	RT sample and get ready to party.
**
**		If we are the originating party,  we will want to send a
**	CONNECTED frame to our partner, even though the connection is
**	complete from our perspective.  This will allow partner to establish
**	his baseline RTT and clock bias as we can do here.  In this case,  he
**	will have his POLL bit set in the frame we just received.
**
**		Now,  we might get additional CONNECTED frames after the first one
**	where we startup the link.  This would most likely be due to our CONNECTED
**	response getting lost.  So if we get a CONNECTED frame with POLL set
**	after our link is up,  we will just go ahead and respond again without
**	adjusting our state.
**
**		Note about Locks:
**
**		This code is complicated by the precedence of CritSec ownership.  To simplify
**	as much as possible we will take the Listen command lock at the very start of the
**	procedure (when appropriate) because it has the highest level lock.  This prevents
**	us from completing the whole connection process and then finding that the Listen
**	went away so we cant indicate it to the user.
**
**		DELTA:  We now keep a RefCnt on the Listen so it wont go away while a new session
**	is pending on it.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ProcessConnectedResponse"

VOID ProcessConnectedResponse(PSPD pSPD, PEPD pEPD, PCFRAME pCFrame)
{
	PCHKPT		pCP;
	PMSD		pMSD = NULL;
	DWORD		tNow = GETTIMESTAMP();
	PVOID		Context;

	LOGPF(2, "CONNECT RESPONSE RECEIVED (pEPD=0x%p)", pEPD);

	if((pEPD->dwSessID != pCFrame->dwSessID)||(pEPD->ulEPFlags & EPFLAGS_END_POINT_DORMANT))
	{	// We can ignore these babies...
		LOGPF(0, "CONNECTED response received with bad SessID");

		if((pEPD->ulEPFlags & (EPFLAGS_CONNECTING | EPFLAGS_CONNECTED))==0)
		{
			LOGPF(0, "(%p) CONNECTED response received with on Endpoint that is not connecting or connected, rejecting connection", pEPD);
			RejectConnection(pEPD);
		}
		return;
	}

	//	If we have completed our side of the connection then our only responsibility is to send responses
	// if our partner is still POLLING us.
	
	if(pEPD->ulEPFlags & EPFLAGS_CONNECTED)
	{
		if(pCFrame->bCommand & PACKET_COMMAND_POLL)
		{	// Is partner asking for immediate resp?
			Lock(&pEPD->EPLock);
			LOGPF(5, "(%p) Sending CONNECTED Frame", pEPD);
			(void) SendCommandFrame(pEPD, FRAME_EXOPCODE_CONNECTED, pCFrame->bMsgID);
			Unlock(&pEPD->EPLock);
		}

		return;
	}

	//	Since we are not CONNECTED yet,  we must be in a CONNECTING_STATE in order to receive a CONNECTED response.
	// If not,  then we are outof sync with our partner and we can ignore this frame.
	
	if((pEPD->ulEPFlags & EPFLAGS_CONNECTING)==0)
	{
		// Should we send a DISC frame to let him know we are not communicating?  Dont think we need to..
		// We MAY want to KillEndpoint,  unless we do garbage collection for dormant Endpoints.
		LOGPF(0, "Received a CONNECTED RESPONSE but we are not in CONNECTING state!");
		return;
	}

	Lock(&pEPD->EPLock);						// Protect the pCommand field
	pMSD = pEPD->pCommand;			
	if(pMSD == NULL)
	{
		Unlock(&pEPD->EPLock);
		return;
	}

	LOCK_MSD(pMSD, "Hold For Lock");		// Place reference on Cmd until we can lock it
	Unlock(&pEPD->EPLock);					// Protect the pCommand field

 	Lock(&pMSD->CommandLock);

	if((pMSD->ulMsgFlags1 & (MFLAGS_ONE_CANCELLED)) || (pMSD->ulMsgFlags2 & MFLAGS_TWO_ABORT))
	{
		LOGPF(1, "Connect/Listen command toasted; Disregarding CONNECTED frame");

		// Whoever cancelled the Listen should be disconnecting this connection too
		// so all we have to do here is bail out.

		RELEASE_MSD(pMSD, "Hold For Lock");
		
		return;
	}
		
	// Next, take care of this guy's reply if we still owe him one

	Lock(&pEPD->EPLock);
	
	if(pCFrame->bCommand & PACKET_COMMAND_POLL)
	{				// Is partner asking for immediate resp?
		LOGPF(5, "(%p) Sending CONNECTED Frame", pEPD);
		if(SendCommandFrame(pEPD, FRAME_EXOPCODE_CONNECTED, pCFrame->bMsgID) != DPN_OK)
		{
			LOGPF(5, "(%p) Sending CONNECTED Frame Failed", pEPD);

			// We cannot complete the connection...  we will just let things time out
			RELEASE_MSD(pMSD, "Hold For Lock");
			Unlock(&pEPD->EPLock);					// Protect the pCommand field
			return;
		}
	}

	// Now we can setup our new link,  but only if this frame Correlates to a checkpoint we have outstanding
	// so we can seed our state variables.

	// Can we correlate resp?
	if((pCP = LookupCheckPoint(pEPD, pCFrame->bRspID)) != NULL)
	{
		// We are connected, so shut off retry timer
		if(pEPD->ConnectTimer != 0)
		{
			LOGPF(5, "(%p) Cancelling Connect Timer", pEPD);
			if(CancelMyTimer(pEPD->ConnectTimer, pEPD->ConnectTimerUnique) == DPN_OK)
			{
				RELEASE_EPD(pEPD, "UNLOCK: (Release CONN Timer)");		//  remove reference for timer, SendQLock not already held

				// If we cannot cancel the timer then we must leave the MSD pointer in the EPD so that the timer
				// will find out that the command is complete and will finish cleaning this up
				pEPD->pCommand = NULL;
				DECREMENT_MSD(pMSD, "EPD Ref");							// Release reference for EPD link

				pEPD->ConnectTimer = 0;								// This will prevent timer from trying to do any work if it couldnt cancel
			}
			else 
			{
				LOGPF(5, "(%p) Cancelling Connect Timer Failed", pEPD);
			}
		}
		else 
		{
			pEPD->pCommand = NULL;
			DECREMENT_MSD(pMSD, "EPD Ref");							// Release reference for EPD link
		}
		
		pEPD->ulEPFlags |= EPFLAGS_CONNECTED | EPFLAGS_STREAM_UNBLOCKED;// Link is open for business

		if(pEPD->ulEPFlags & EPFLAGS_LINKED_TO_LISTEN)
		{
			pEPD->ulEPFlags &= ~(EPFLAGS_LINKED_TO_LISTEN);
			pEPD->blSPLinkage.RemoveFromList();							// Unlink EPD from Listen Queue
		}
		
		InitLinkParameters(pEPD, (tNow - pCP->tTimestamp), sizeof(CFRAME), (pCP->tTimestamp - pCFrame->tTimestamp));
		ChkPtPool->Release(ChkPtPool, pCP);

		LOGPF(5, "(%p) N(R) = 0, N(S) = 0", pEPD);
		pEPD->bNextSend = 0;
		pEPD->bNextReceive = 0;

		FlushCheckPoints(pEPD);									// Make sure we do this before the InitCheckPoint
		
		/*
		**	It turns out that the first RTT measurement is a very bad one (slow) because because
		**	it includes overhead for opening and binding a new socket,  endpoint creation,  etc.
		**	Therefore each side will take another quick sample right away.  The initial calculations
		**	above will still serve as an initial RTT until this better sample is available
		*/

		PerformCheckpoint(pEPD);								// Take another RTT sample

		// Cleanup connect operation
		if(pEPD->ulEPFlags & EPFLAGS_CALLING)
		{
			// There was a CONNECT Command issued that now must be completed
			
			pMSD->ulMsgFlags2 |= MFLAGS_TWO_COMPLETE;

			Unlock(&pEPD->EPLock);
			
			ASSERT(pMSD->CommandID == COMMAND_ID_CONNECT);
			
			DECREMENT_MSD(pMSD, "Hold For Lock");					// Remove temporary reference from above.
			
			CompleteConnect(pMSD, pSPD, pEPD, DPN_OK);
		}
		else 
		{		// LISTENING
			// We were the listener.  We will indicate a Connect event on the listen
			// command w/o completing the Listen

			ASSERT(pMSD->CommandID == COMMAND_ID_LISTEN);

			Context = pMSD->Context;
			RELEASE_MSD(pMSD, "Hold For Lock");					// release temp MSD (releases lock)
			LOGPF(3, "(%p) Calling Core->IndicateConnect", pEPD);
			pSPD->pPData->pfVtbl->IndicateConnect(pSPD->pPData->Parent, Context, (PHANDLE) pEPD, (PVOID *) &pEPD->Context);
			pEPD->ulEPFlags &= ~(EPFLAGS_CONNECTING);				// Reset opening state
			ReceiveComplete(pEPD);									// Complete any receives that queued while waiting for IndicateConnect
		}		
	}
	else 
	{
		/*
		**		Uncorrelated CONNECTED frame.  How can this happen?  Parter's response must
		**	have been dropped,  so he is retrying his CONN frame.  Since we are trying to
		**	measure an accurate RTT we dont want to use his retry against our original
		**	request,  so he zeros out his Resp correlator.  We will eventually retry with
		**	new correlator and hopefully that frame will get through.
		*/

		LOGPF(1, "(%p) Uncorrelated CONNECTED frame arrives", pEPD);
		Unlock(&pEPD->EPLock);
		RELEASE_MSD(pMSD, "Hold For Lock");
	}
}

/*
**		Drop Link
**
**			For whatever reason we are dropping an active link.  This requires us to
**		Cancel any outstanding commands and give an indication to the user.
**
**
**		**  CALLED WITH EPD->EPLOCK HELD;  RETURNS WITH LOCK RELEASED  **
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DropLink"

VOID DropLink(PEPD pEPD)
{
	PRCD				pRCD;
	PRCD				pNext;
	CBilink				*pLink;
	PSPRECEIVEDBUFFER	pRcvBuff = NULL;
	PMSD				pMSD;					// Ptr for connection in progress
	PMSD				pListen = NULL;
	PProtocolData		pPData;
	PVOID				Context;

	LOGPF(5, "Drop Link %p (refcnt=%d)", pEPD, pEPD->uiRefCnt);

	ASSERT_EPD(pEPD);

	//	First set/clear flags to prevent any new commands from issueing

	pEPD->ulEPFlags &= ~(EPFLAGS_CONNECTED | EPFLAGS_SDATA_READY | EPFLAGS_STREAM_UNBLOCKED);	// Link is now down
	pEPD->ulEPFlags |= EPFLAGS_TERMINATING;							// Accept no new commands

	// I am creating a RefCnt bump for the send pipeline,  which means we will no longer pull EPDs off
	// the pipeline here.  The clearing of the flags above will cause the EPD to be dropped from the
	// pipeline the next time it is due to be serviced.  We CAN still clean up all the frames'n'stuff
	// because the send loop doesnt need to actually DO anything with this EPD.  This behavior allows
	// the SendThread to loop through the pipeline queue, surrendering locks, without having the queue
	// changing beneath it.

	//  Stop all timers (there are five now)

	if(pEPD->RetryTimer)
	{
		if(CancelMyTimer(pEPD->RetryTimer, pEPD->RetryTimerUnique) == DPN_OK)
		{
			RELEASE_EPD(pEPD, "UNLOCK (DROP RETRY)"); // SendQLock not already held
		}
		pEPD->RetryTimer = 0;
	}
	if(pEPD->ConnectTimer)
	{
		if(CancelMyTimer(pEPD->ConnectTimer, pEPD->ConnectTimerUnique) == DPN_OK)
		{
			RELEASE_EPD(pEPD, "UNLOCK (DROP RETRY)"); // SendQLock not already held
		}
		pEPD->ConnectTimer = 0;
	}
	if(pEPD->DelayedAckTimer)
	{
		if(CancelMyTimer(pEPD->DelayedAckTimer, pEPD->DelayedAckTimerUnique) == DPN_OK)
		{
			RELEASE_EPD(pEPD, "UNLOCK (DROP DELAYEDACK)"); // SendQLock not already held
		}
		pEPD->DelayedAckTimer = 0;
	}
	if(pEPD->DelayedMaskTimer)
	{
		if(CancelMyTimer(pEPD->DelayedMaskTimer, pEPD->DelayedMaskTimerUnique) == DPN_OK)
		{
			RELEASE_EPD(pEPD, "UNLOCK (DROP DELAYED MASK)"); // SendQLock not already held
		}
		pEPD->DelayedMaskTimer = 0;
	}
	if(pEPD->SendTimer)
	{
		if(CancelMyTimer(pEPD->SendTimer, pEPD->SendTimerUnique) == DPN_OK)
		{
			RELEASE_EPD(pEPD, "UNLOCK (DROP SENDTIMER)"); // SendQLock not already held
			pEPD->SendTimer = 0;
			// BUGBUG: Do we want to NULL this and the one below in the else case as above?
		}
	}
	if(pEPD->BGTimer)
	{
		if(CancelMyTimer(pEPD->BGTimer, pEPD->BGTimerUnique) == DPN_OK)
		{
			RELEASE_EPD(pEPD, "UNLOCK (DROP BG TIMER)"); // SendQLock not already held
			pEPD->BGTimer = 0;
		}
	}

	AbortSendsOnConnection(pEPD);									// Cancel pending commands; releases SendQLock

	Lock(&pEPD->EPLock);
	
	// Are we still opening this connection?
	// After IndicateConnect we will be dormant, but not connecting
	if(pEPD->ulEPFlags & (EPFLAGS_CONNECTING | EPFLAGS_END_POINT_DORMANT))
	{
		pMSD = pEPD->pCommand;

		if(pMSD != NULL)
		{
			ASSERT_MSD(pMSD);

			Unlock(&pEPD->EPLock);										// To avoid deadlock we must release SendQLock before taking cmd lock
			Lock(&pMSD->CommandLock);
			Lock(&pEPD->EPLock);

			// We left the lock, so ensure there is still something to clean up
			if (pEPD->pCommand)
			{
				if(pEPD->ulEPFlags & EPFLAGS_LINKED_TO_LISTEN)
				{
					pEPD->ulEPFlags &= ~EPFLAGS_LINKED_TO_LISTEN;
					pEPD->blSPLinkage.RemoveFromList();						// Unlink EPD from Listen Queue
				}
				
				pEPD->pCommand = NULL; // Tell everyone after us this is cleaned up

				if(pMSD->CommandID == COMMAND_ID_CONNECT)
				{
					Unlock(&pEPD->EPLock);
					DECREMENT_MSD(pMSD, "EPD Ref");		// Remove temporary reference, placed in IndicateConnect
					CompleteConnect(pMSD, pEPD->pSPD, pEPD, DPNERR_ABORTED); // This unlocks the MSD Lock
					Lock(&pEPD->EPLock);
				}
				else
				{
					// This unlocks the MSD lock
					RELEASE_MSD(pMSD, "EPD Ref");		// Remove temporary reference, placed in IndicateConnect
				}
			}
		}
	}


	// Now we clean up any receives in progress.  We throw away any partial or mis-ordered messages.
	while((pRCD = pEPD->pNewMessage) != NULL)
	{
		pEPD->pNewMessage = pRCD->pMsgLink;
		if(pRCD->pRcvBuff)
		{
			pRCD->pRcvBuff->pNext = pRcvBuff;
			pRcvBuff = pRCD->pRcvBuff;
		}
		else
		{
			ASSERT(pRCD->ulRFlags & (RFLAGS_FRAME_INDICATED_NONSEQ | RFLAGS_FRAME_LOST));
		}
		RELEASE_RCD(pRCD);
	}

	while(!pEPD->blOddFrameList.IsEmpty())
	{
		pLink = pEPD->blOddFrameList.GetNext();
		pRCD = CONTAINING_RECORD(pLink, RCD, blOddFrameLinkage);
		pLink->RemoveFromList();
		if(pRCD->pRcvBuff)
		{
			pRCD->pRcvBuff->pNext = pRcvBuff;
			pRcvBuff = pRCD->pRcvBuff;
		}
		else
		{
			ASSERT(pRCD->ulRFlags & (RFLAGS_FRAME_INDICATED_NONSEQ | RFLAGS_FRAME_LOST));
		}
		RELEASE_RCD(pRCD);
	}

	while(!pEPD->blCompleteList.IsEmpty())
	{
		pLink = pEPD->blCompleteList.GetNext();
		pRCD = CONTAINING_RECORD(pLink, RCD, blCompleteLinkage);
		pLink->RemoveFromList();
		
		ASSERT(pEPD->uiCompleteMsgCount > 0);
		pEPD->uiCompleteMsgCount--;
		
		while(pRCD != NULL)
		{
			pNext = pRCD->pMsgLink;
			
			RELEASE_SP_BUFFER(pRCD->pRcvBuff);
			
			RELEASE_RCD(pRCD);
			pRCD = pNext;
		}
	}

	Unlock(&pEPD->EPLock);

	if(pRcvBuff)
	{
		LOGPF(3, "(%p) Calling SP->ReturnReceiveBuffers", pEPD);
		IDP8ServiceProvider_ReturnReceiveBuffers(pEPD->pSPD->IISPIntf, pRcvBuff);
	}

	pPData = pEPD->pSPD->pPData;
	Context = pEPD->Context;

	KillConnection(pEPD);
	
	// Tell user that session is over

	Lock(&pEPD->EPLock);

	if(( (pEPD->ulEPFlags & (EPFLAGS_ABORT | EPFLAGS_INDICATED_DISCONNECT | EPFLAGS_CONNECTING)) == 0))
	{
		pEPD->ulEPFlags |= EPFLAGS_INDICATED_DISCONNECT;
		Unlock(&pEPD->EPLock);
		LOGPF(3, "(%p) Calling Core->IndicateConnectionTerminated", pEPD);
		pEPD->pSPD->pPData->pfVtbl->IndicateConnectionTerminated(pEPD->pSPD->pPData->Parent, Context, DPNERR_CONNECTIONLOST);
	}
	else 
	{
		Unlock(&pEPD->EPLock);
	}
}
