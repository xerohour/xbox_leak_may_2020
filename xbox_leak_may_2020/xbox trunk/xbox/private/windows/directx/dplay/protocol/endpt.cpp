/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		EndPt.c
 *  Content:	This file contains EndPoint management routines.
 *				An End Point is a DirectNet instance that we know about and may communicate
 *				with.  An End Point Descriptor (EPD) tracks each known End Point and was mapped
 *				onto an hEndPoint by a hash table. Now, the SP maintains the mapping and hands
 *				us our EPD address as a context with each indication ReceiveEvent.
 *
 *				In addition to EndPoint creation and destruction,  this file contains routines
 *				which handle link tuning.  This is described in detailed comments below.
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *  11/06/98	ejs		Created
 *	07/14/99	ejs		Removed all of our hashing support (most of this file)
 *
 ****************************************************************************/

#include "dnproti.h"

VOID	BackOff(PEPD);
VOID	IncreaseTransmitRate(PEPD, DWORD);
VOID	RunAdaptiveAlg(PEPD, DWORD);
VOID	ThrottleBack(PEPD, DWORD);

#define 	USE_BURST_GAP		TRUE

/*
**		Crack EndPoint Descriptor
**
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DNPCrackEndPointDescriptor"

HRESULT DNPCrackEndPointDescriptor(HANDLE hEndPoint, PSPGETADDRESSINFODATA pSPData)
{
	PEPD	pEPD = (PEPD) hEndPoint;
	HRESULT	hr = DPNERR_INVALIDENDPOINT;

	LOGPF(2, "Parameters: hEndPoint[%x], pSPData[%p]", hEndPoint, pSPData);
	
	ASSERT_EPD(pEPD);

	if(LOCK_EPD(pEPD, "LOCK (Crack EPD)"))
	{
		if(pEPD->ulEPFlags & EPFLAGS_CONNECTED)
		{
			pSPData->hEndpoint = pEPD->hEndPt;
			LOGPF(3, "(%p) Calling SP->GetAddressInfo", pEPD);
			hr = IDP8ServiceProvider_GetAddressInfo(pEPD->pSPD->IISPIntf, pSPData);
		}
		
		RELEASE_EPD(pEPD, "UNLOCK (Crack EPD)"); // SendQLock not already held
	}
	return hr;
}

/*
**		INTERNAL - EndPoint management functions
*/

/*
**		New End Point
**
**		Everytime a packet is indicated with an address that we dont recognize we will allocate
**	an EPD for it and add it to our tables.  It is a higher layer's responsibility to tell
**	us when it no longer wants to talk to the EP so that we can clear it out of our
**	(and the SP's) table.
**
*/

#undef DPF_MODNAME
#define DPF_MODNAME "NewEndPoint"

PEPD NewEndPoint(PSPD pSPD, HANDLE hEP)
{
	PEPD	pEPD;

	if((pEPD = static_cast<PEPD> (EPDPool->Get(EPDPool))) == NULL)
	{	
		LOGPF(0, "Failed to allocate new EPD");
		return NULL;
	}

	ASSERT(hEP != INVALID_HANDLE_VALUE);

	pEPD->hEndPt = hEP;								// Record ID in structure
	pEPD->pSPD = pSPD;

	pEPD->bNextMsgID = 0;
//	pEPD->bNextRcvID = 0;
	
	pEPD->uiRTT = 0;
	pEPD->uiBytesReceived = 0;
	pEPD->uiBytesSent = 0;
	pEPD->uiFramesSent = 0;
	pEPD->uiFramesReceived = 0;
	pEPD->uiFramesAcked = 0;
	pEPD->uiBytesAcked = 0;

	pEPD->uiQueuedMessageCount = 0;
#ifdef	DEBUG
	pEPD->bLastDataSeq = 0xFF;
#endif

	// We track a byte-window and a frame-window separately.  Start with 2 discreet frames,  but only 1 FULL frame.
													
	pEPD->iWindowF = 2;								// start with 1 full-frame window (this could still be many frames)
	pEPD->uiWindowBIndex = 1;
	pEPD->iWindowB = pSPD->uiFrameLength;			// ditto for bytes
	pEPD->iUnackedFrames = 0;						// outstanding frame count
	pEPD->iUnackedBytes = 0;						// outstanding byte count
	pEPD->uiBurstSize = 1;							// start at 1 frame/interval (we never increase this any more ao 9/19/00)
	pEPD->iBurstSizeBytes = (INT) pSPD->uiFrameLength;
	pEPD->uiBurstGap = 100;							// starting point 100ms/burst
	pEPD->tNextSend = 0;							// next send is anytime!
	pEPD->dwSessID = 0;

	// ReceiveComplete flag prevents received data from being indicated to core until after new connection is indicated
	pEPD->ulEPFlags = EPFLAGS_END_POINT_IN_USE | EPFLAGS_END_POINT_DORMANT | EPFLAGS_IN_RECEIVE_COMPLETE;

	ASSERT(pEPD->uiRefCnt == 0);					// WE NOW HAVE A -1 BASED REFCNT INSTEAD OF ZERO BASED (FOR EPDs)

	pEPD->SendTimer = 0;							// Timer for next send-burst opportunity
	pEPD->RetryTimer = 0;							// window to receive Ack
	pEPD->ConnectTimer = 0;
	pEPD->DelayedAckTimer = 0;						// wait for piggyback opportunity before sending Ack
	pEPD->DelayedMaskTimer = 0;						// wait for piggyback opportunity before sending Mask frame
	pEPD->BGTimer = 0;								// Periodic background timer
	pEPD->uiCompleteMsgCount = 0;

	pEPD->fpDropsPer128Frames = 0x04000000;			// start looking like we have a 4% drop rate to make more sensitive.
	
	Lock(&pEPD->pSPD->SendQLock);
	pEPD->blActiveLinkage.InsertAfter( &pEPD->pSPD->blEPDActiveList); // Place this guy in active list
	Unlock(&pEPD->pSPD->SendQLock);

	
	return pEPD;
}

/*
**		Initial Link Parameters
**
**		we have kept a checkpoint structure matching everying frame we sent in the Connect
**	handshake so that we can match a response to a specific frame or retry.  This allows us
**	to measure a single sample Round Trip Time (RTT),  which we will use below to generate
**	initial values for our link-state variables.
**
**	BurstSize will always init to 1.  BurstGap will init to (RTT/2). When BurstGap reaches
**	a minimum of 5 or 10 ms,  we will start increasing the BurstSize.  However,  here for
**	initial values we will max out a one frame/10ms.  This should quickly increase for
**	speedsters.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "InitLinkParameters"

VOID InitLinkParameters(PEPD pEPD, UINT uiRTT, UINT normal_load, UINT bias)
{
	DWORD	tNow = GETTIMESTAMP();
	DWORD	dwTimerInterval;

	if(uiRTT == 0)
	{
		uiRTT = 1;
	}
		
	pEPD->uiRTT = uiRTT;										// we know the base RTT
	pEPD->fpRTT = TO_FP(uiRTT);									// 16.16 fixed point version
//	pEPD->fpFastRTT = TO_FP(uiRTT);								// 16.16 fast-moving avg
//	pEPD->fpDev = 0;											// 16.16 avg deviation
//	pEPD->uiRTTgrowing = 0;										// numbers of consecutive samples with avg RTT growing
//	pEPD->uiRecoveryBGap = 0;
//	pEPD->uiRecoveryBSize = 0;
	pEPD->uiDropCount = 0;
//	pEPD->tDropInterval = 0;
//	pEPD->uiDropEvents = 0;
	pEPD->uiThrottleEvents = 0;									// Count times we throttle-back for all reasons
#ifdef	DEBUG
	pEPD->uiTotalThrottleEvents = 0;
#endif
	
//	pEPD->uiBurstGap = MAX((uiRTT+1)/2, 5);						// Initially send frames twice per RTT
																// but no more then 1 every 5 seconds

	pEPD->uiBurstGap = 0;	// For now assume we dont need a burst gap
	pEPD->uiBurstSize = 1;
	pEPD->iBurstSizeBytes = (INT) pEPD->pSPD->uiFrameLength;
#ifdef POLL_COUNT	
	pEPD->uiPollFrequency = 1;									// POLL early,  POLL often for new links
	pEPD->uiPollCount = 1;										// We will POLL on our first sequential frame
#endif

	pEPD->uiMsgSentHigh = 0;
	pEPD->uiMsgSentNorm = 0;
	pEPD->uiMsgSentLow = 0;
	pEPD->uiMsgTOHigh = 0;
	pEPD->uiMsgTONorm = 0;
	pEPD->uiMsgTOLow = 0;
	
	pEPD->uiMessagesReceived = 0;

	pEPD->uiGuaranteedFramesSent = 0;
	pEPD->uiGuaranteedBytesSent = 0;
	pEPD->uiDatagramFramesSent = 0;
	pEPD->uiDatagramBytesSent = 0;
	
	pEPD->uiGuaranteedFramesReceived = 0;
	pEPD->uiGuaranteedBytesReceived = 0;
	pEPD->uiDatagramFramesReceived = 0;
	pEPD->uiDatagramBytesReceived = 0;
	
	pEPD->uiGuaranteedFramesDropped = 0;
	pEPD->uiGuaranteedBytesDropped = 0;
	pEPD->uiDatagramFramesDropped = 0;
	pEPD->uiDatagramBytesDropped = 0;

	pEPD->uiGoodBurstSize = 1;
	pEPD->uiGoodBurstGap = 0;									// No Known Good Gap!
	pEPD->iGoodWindowF = 1;
	pEPD->uiGoodWindowBI = 1;
	pEPD->iBurstCredit = 0;
	pEPD->uiLastBadGap = 0;
	pEPD->tLastDelta = tNow;//
	pEPD->uiWindowFilled = 0;
	
	pEPD->tStartIdle = tNow;									// Timestamp when link went idle
	pEPD->uiTotalIdleTime = 0;									// Time link has spent idle this period (since last run of DynAlg)
	pEPD->tLastThruPutSample = tNow;

	pEPD->uiLastBytesAcked = 0;
	pEPD->uiPeriodAcksBytes = 0;
	pEPD->uiPeriodXmitTime = 0;
	pEPD->uiPeriodRateB = 0;
	pEPD->uiPeakRateB = 0;
	
	pEPD->uiLastRateB = 0;
//	pEPD->fpLastRTT = -1;
	
	pEPD->ulReceiveMask = 0;
	pEPD->ulReceiveMask2 = 0;
	pEPD->tReceiveMaskDelta = 0;
	
	pEPD->ulSendMask = 0;
	pEPD->ulSendMask2 = 0;
	
	pEPD->Context = NULL;
	pEPD->dwSetPoint = 0;
	
//	pEPD->bLastAckID = pEPD->bNextMsgID;
	
//	pEPD->uiNormalSize = (UINT) normal_load * 1.25;  			// this wont compile
//	pEPD->uiNormalSize = normal_load + (normal_load >> 2);		// Normal Size = normal_load * 1.25
//	pEPD->uiNormalBias = bias;
	
//	pEPD->iPeriodBiasDelta = 0;
//
//	pEPD->uiLoadedBias = 0;
//	pEPD->uiLoadedSize = 0;
//	pEPD->tLoadedSample = 0;
	
//	pEPD->uiMidLoadBias = 0;
//	pEPD->tMidLoadSample = 0;

	LOGPF(7, "CONNECTION ESTABLISHED pEPD = 0x%p RTT = %dms, BurstGap=%dms", pEPD, pEPD->uiRTT, pEPD->uiBurstGap);

	// We set the IdleThreshhold very low to generate a little bit of traffic for initial link tuning in case the
	// application doesnt do any right away

	pEPD->ulEPFlags |= EPFLAGS_USE_POLL_DELAY;					// always assume balanced traffic at start-up
	
	pEPD->uiAdaptAlgCount = 4;									// start running adpt alg fairly often
//	pEPD->State = (uiRTT < 25) ? EP_STATE_LAN_DYNAMIC : EP_STATE_WAN_DYNAMIC; // Start in DYNAMIC state
	
	//pEPD->tIdleThreshhold = pEPD->pSPD->pPData->tIdleThreshhold;// THIS IS CONTROLLED BY GLOBAL VAR FOR EVERYONE
	
	pEPD->uiRetryTimeout = (pEPD->uiRTT + (pEPD->uiRTT >> 2) + DELAYED_ACK_TIMEOUT) * 2;
	// don't want to get more aggressive because we drop a frame.
	if(pEPD->uiRetryTimeout < pEPD->uiBurstGap)
	{
		pEPD->uiRetryTimeout = pEPD->uiBurstGap;	
	}
	

	pEPD->uiUserFrameLength = pEPD->pSPD->uiUserFrameLength;
	
	if(pEPD->BGTimer == 0)
	{
		if (pEPD->pSPD->pPData->tIdleThreshhold > ENDPOINT_BACKGROUND_INTERVAL)
			dwTimerInterval = ENDPOINT_BACKGROUND_INTERVAL;
		else
			dwTimerInterval = pEPD->pSPD->pPData->tIdleThreshhold;

		LOGPF(7, "(%p) Setting Endpoint Background Timer for %u ms", pEPD, dwTimerInterval);
		SetMyTimer(dwTimerInterval, 1000, EndPointBackgroundProcess, (PVOID) pEPD, &pEPD->BGTimer, &pEPD->BGTimerUnique);
		LOCK_EPD(pEPD, "LOCK (BG Timer)");												// create reference for this timer
	}
}

/*
**		Kill Connection
**
**		This routine shall release the 'base' reference on an endpoint,  taking pains to ensure
**	that this reference only gets released once.  We will also attempt to cancel the Background Timer
**	for the endpoint too.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "KillConnection"

VOID
KillConnection(PEPD pEPD)
{
	
	Lock(&pEPD->EPLock);
	
	LOGPF(7, "Kill Connection %p (refcnt=%d)", pEPD, pEPD->uiRefCnt);

	ASSERT((pEPD->pCommand == NULL)||(pEPD->ulEPFlags & EPFLAGS_ABORT));
	
	pEPD->ulEPFlags &= ~(EPFLAGS_CONNECTED);
	
	if(!(pEPD->ulEPFlags & EPFLAGS_KILLED))
	{
		pEPD->ulEPFlags |= EPFLAGS_KILLED;
		if(pEPD->BGTimer != 0)
		{
			LOGPF(7, "(%p) Cancelling Endpoint Background Timer", pEPD);
			if(CancelMyTimer(pEPD->BGTimer, pEPD->BGTimerUnique)== DPN_OK)
			{
				RELEASE_EPD(pEPD, "UNLOCK (KILL BG TIMER)"); // SendQLock not already held
				pEPD->BGTimer = 0;
			}
			else
			{
				LOGPF(7, "(%p) Cancelling Endpoint Background Timer Failed", pEPD);
			}
		}
		
		Unlock(&pEPD->EPLock);

		RELEASE_EPD(pEPD, "UNLOCK (KILLCONN)");					// RELEASE the EPD, SendQLock not already held
	}
	else{
		Unlock(&pEPD->EPLock);
	}

}

/*
**		Disconnect Connection
**
**		Pull the actual plug in the service provider.  After this call we may no longer send or receive
**	on this endpoint.  Typically we will want to do this when our refcnt reaches zero,  but there are
**	cases where we want to abort things abnormally (partner has re-initialized the link) and do this
**	right away.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DisconnectConnection"

VOID
DisconnectConnection(PEPD pEPD)
{
	SPDISCONNECTDATA	Block;
	HRESULT				hr;

	Lock(&pEPD->EPLock);
	
	if(pEPD->hEndPt != INVALID_HANDLE_VALUE)
	{
		Block.hEndpoint = pEPD->hEndPt;
		Block.dwFlags = 0;
		Block.pvContext = NULL;

		pEPD->hEndPt = INVALID_HANDLE_VALUE;
		
		Unlock(&pEPD->EPLock);

		LOGPF(3, "(%p) Calling SP->Disconnect", pEPD);
		hr = IDP8ServiceProvider_Disconnect(pEPD->pSPD->IISPIntf, &Block);
	}
	else 
	{
		Unlock(&pEPD->EPLock);
	}
}

/****************
*
*			Link Tuning
*
*		Here are current ideas about link tuning.  Idea is to track Round Trip Time of key-frames and throttle
*	based upon changes in this measured RTT when possible.  This would benefit us in determining link saturation
*	before packet loss occurs, instead of waiting for the inevitable packet loss before throttling back.
*
*		On high-speed media,  the average RTT is small compared to the standard deviations making it hard to
*	predict anything useful from them.  In these cases,  we must look at packet drops.  Except for one exception:
*	We will look for large spikes in RTT and we will respond to these with an immediate, temporary throttle back.
*	This will allow a bottle-neck to clear hopefully without packet-loss.  So far,  I have not been able to verfify
*	any benefit from this behavior on reliable links.  It is more likely to be beneficial with datagram traffic
*	where send windows do not limit write-ahead.
*
*		I would like to take a measurement of the through-put acheived compared to the transmission rate,  but I
*	havent yet come up with a good way to measure this.  What I do calculate is packet acknowledgement rate,  which
*	can be calculated without any additional input from the remote side.  We will store AckRates acheived at the
*	previous transmission rate,  so we can look for improvements in Acks as we increase Transmissions.  When we
*	no longer detect AckRate improvements then we assume we have plateaued and we stop trying to increase the rate.
*
*	TRANSMISSION RATE
*
*		Transmission rate is controlled by two distinct parameters: Insertion Rate and Window Size.  Where a
*	conventional protocol would dump a window full of packets onto the wire in one burst,  we would like to
*	spread the packet insertions out over the full RTT so that the window never completely fills and hence
*	blocks the link from transmitting.  This has a wide array of potential benefits:  Causes less congestions
*	throughout the network path; Allows more balenced access to the wire to all Endpoints (especially on
*	slower media); Allows MUCH more accurate measurements to be made of trasmission times when packets
*	spend less time enqueued locally;  Allows retry timers to be set much lower giving us quicker error
*	recovery (because there is less queue time fudged into the timer);  Allows recovery to be made more
*	quickly when we dont have a lot of data enqueued in SP (both our own data and other Endpoint's data).
*	...And I am sure there are more.
*
*		So,  we would like to trickle out packets just fast enough to fill the window as the next ACK is received.
*	We will grow the window fairly liberally and let the burst rate increase more cautiously.
*	
*		On high-speed media the insertion time becomes fairly small (near zero) and we are less likely to queue
*	up large quantities of data.  Therefore we may allow insertion rate to go max and use the window alone to
*	control flow. I will experiment with this more.
*
******************/

#define		RTT_SLOW_WEIGHT					8					// fpRTT gain = 1/8
#define		RTT_FAST_WEIGHT					4					// fpFastRTT gain = 1/4
#define		THROTTLE_EVENT_THRESHOLD		20

/*
**		NEW REDUCED-FAT UPDATE ENDPOINT
**
**		This is the first draft of the simplified UpdateEndpoint suite.  Here we stop trying to divine mysterious
**	truths through chicken entrails and latency fluctuations,  etc.  We will let the sliding window control the flow
**	and increase the window as long as through-put continues to increase and frames continue to get delivered without
**	excessive droppage.
**	
**		We still calculate RTT for the purpose of determining RetryTimer values.  For cases with large RTTs we may still
**	implement an inter-packet gap,  but we will try to make it an aggressive gap (conservatively small) because we would
**	rather feed the pipe too quickly then artificially add latency by letting the pipe go idle with data ready to be sent.
**
**		** CALLED WITH EPD STATELOCK HELD **
*/

#undef DPF_MODNAME
#define DPF_MODNAME "UpdateEndPoint"

VOID UpdateEndPoint(PEPD pEPD, UINT uiRTT, UINT payload, UINT bias, DWORD tNow)
{
	UINT	fpRTT;
	INT		fpDiff;
	
	// Don't allow zero RTTs
	if(uiRTT == 0)
	{												
		uiRTT = 1;
	}
	
	// Filter out HUGE samples,  they often popup during debug sessions
	else if(uiRTT > (pEPD->uiRTT * 128))
	{
		LOGPF(7, "Tossing huge sample (%dms)", uiRTT);
		return;
	}

	// Perform next iteration of math on new RTT sample in 16.16 fixed point

	fpRTT = TO_FP(uiRTT);										// Fixed point sample
	fpDiff = fpRTT - pEPD->fpRTT;								// Current Delta (signed)

	pEPD->fpRTT = pEPD->fpRTT + (fpDiff / RTT_SLOW_WEIGHT);		// .0625 weighted avg
	pEPD->uiRTT = FP_INT(pEPD->fpRTT);							// Store integer portion

//	fpDiff = fpRTT - pEPD->fpRTTFast;
//	pEPD->fpRTTFast += (fpDiff / RTT_FAST_WEIGHT);				// .250 weighted avg
	
	// Calc a retry timeout value based upon the measured RTT (2.5 * RTT) + MAX_DELAY
	
	pEPD->uiRetryTimeout = ((pEPD->uiRTT * 4) + DELAYED_ACK_TIMEOUT);
	pEPD->uiRetryTimeout += (pEPD->uiRetryTimeout >> 2);	// 1.25 X
	// don't want to get more aggressive because we drop a frame.
	if(pEPD->uiRetryTimeout < pEPD->uiBurstGap)
	{
		pEPD->uiRetryTimeout = pEPD->uiBurstGap;	
	}
	
	LOGPF(7, "(%p) RTT SAMPLE: Size = %d; RTT = %d, Avg = %d <<<<", pEPD, payload, uiRTT, FP_INT(pEPD->fpRTT));

	// If throttle is engaged we will see if we can release it yet
	
	if(pEPD->ulEPFlags & EPFLAGS_THROTTLED_BACK)
	{
		if((tNow - pEPD->tThrottleTime) > (pEPD->uiRTT * 8)) 
		{
			pEPD->ulEPFlags &= ~(EPFLAGS_THROTTLED_BACK);
			pEPD->uiDropCount = 0;
			pEPD->uiBurstGap = pEPD->uiRestoreBurstGap;
			pEPD->uiBurstSize = pEPD->uiRestoreBurstSize;
			pEPD->iBurstSizeBytes = (INT) pEPD->uiBurstSize * pEPD->pSPD->uiFrameLength;
			pEPD->iWindowF =  pEPD->iRestoreWindowF;
			pEPD->uiWindowBIndex = pEPD->uiRestoreWindowBI;
			pEPD->iWindowB = pEPD->uiWindowBIndex * pEPD->pSPD->uiFrameLength;

			LOGPF(7, "** (%p) RECOVER FROM THROTTLE EVENT: Window(F:%d,B:%d); Gap=%d; Size=%d", pEPD, pEPD->iWindowF, pEPD->uiWindowBIndex, pEPD->uiBurstGap, pEPD->uiBurstSize);
			pEPD->tLastDelta = tNow;							// Enforce waiting period after back-off before tuning up again
		}
	}
	// Throttle Event tracks how often a packet-drop has caused us to throttle back transmission rate.  We will let this value
	// decay over time.  If throttle events happen faster then the decay occurs then this value will grow un-bounded.  This
	// growth is what causes a decrease in the actual send window/transmit rate that will persist beyond the throttle event.
	
	else if(pEPD->uiThrottleEvents)
	{
		pEPD->uiThrottleEvents--;								// Let this decay...
	}

	if(--pEPD->uiAdaptAlgCount == 0)
	{
		RunAdaptiveAlg(pEPD, tNow);
	}
}

/*
**		Grow Send Window
**
**		The two parallel send windows,  frame-based and byte-based,  can grow and shrink independently.  In this
**	routine we will grow one or both windows.  We will grow each window providing that it has been filled in the
**	last period, during which we have determined that thru-put has increased.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "GrowSendWindow"

VOID
GrowSendWindow(PEPD pEPD, DWORD tNow)
{
	UINT	delta = 0;

#ifdef USE_BURST_GAP
	if(pEPD->uiBurstGap)
	{
		// first store current good values for a restore
		pEPD->uiGoodBurstGap=pEPD->uiBurstGap;
		pEPD->uiGoodBurstSize=pEPD->uiBurstSize;
		
		// cut the burst gap by 25% if less than 3 ms go to 0.
		if(pEPD->uiBurstGap > 3)
		{
			pEPD->uiBurstGap -= pEPD->uiBurstGap >> 2;
		} 
		else 
		{
			pEPD->uiBurstGap = 0;
		}
		LOGPF(7, "(%p), burst gap set to %d ms", pEPD, pEPD->uiBurstGap);
		pEPD->tLastDelta = tNow;
	} 
	else 
	{
		pEPD->uiGoodBurstGap=pEPD->uiBurstGap;
		pEPD->uiGoodBurstSize=pEPD->uiBurstSize;
#endif
	
		pEPD->ulEPFlags &= ~(EPFLAGS_SECOND_SAMPLE);
		pEPD->tLastDelta = tNow;
		
		pEPD->iGoodWindowF = pEPD->iWindowF;
		pEPD->uiGoodWindowBI = pEPD->uiWindowBIndex;

		if((pEPD->ulEPFlags & EPFLAGS_FILLED_WINDOW_FRAME) && (pEPD->iWindowF < MAX_RECEIVE_RANGE))
		{
			pEPD->iWindowF++;
			delta = 1;
		}
		if((pEPD->ulEPFlags & EPFLAGS_FILLED_WINDOW_BYTE) && (pEPD->uiWindowBIndex < MAX_RECEIVE_RANGE))
		{
			pEPD->uiWindowBIndex++;
			pEPD->iWindowB += pEPD->pSPD->uiFrameLength;
			delta = 1;
		}

		pEPD->ulEPFlags &= ~(EPFLAGS_FILLED_WINDOW_FRAME | EPFLAGS_FILLED_WINDOW_BYTE);
		pEPD->uiWindowFilled = 0;

		if(delta)
		{
			pEPD->uiLastRateB = pEPD->uiPeriodRateB;
			pEPD->uiPeriodRateB = 0;
			pEPD->uiPeriodAcksBytes = 0;
			pEPD->uiPeriodXmitTime = 0;
			LOGPF(7, "(%p) ** GROW SEND WINDOW to %d frames and %d (%d) bytes", pEPD, pEPD->iWindowF, pEPD->iWindowB, pEPD->uiWindowBIndex);
		}
		else 
		{
			LOGPF(7, "(%p) GROW SEND WINDOW -- Nothing to grow. Transition to Stable!", pEPD);
			pEPD->ulEPFlags |= EPFLAGS_LINK_STABLE;
		}

#ifdef POLL_COUNT	
		pEPD->uiPollFrequency = pEPD->iWindowF * 2;
#endif

#ifdef USE_BURST_GAP
	}
#endif	
}


#undef DPF_MODNAME
#define DPF_MODNAME "RunAdaptiveAlg"

VOID
RunAdaptiveAlg(PEPD pEPD, DWORD tNow)
{
	LONG	tDelta;											// Time the link was transmitting since last run of AdaptAlg
	UINT	fpRealRate;
	DWORD	dwSetPoint;
	UINT	uiBytesAcked;
	UINT	uiNewSum;
	UINT	uiThreshhold;
#ifdef	DEBUG
//	UINT	fpAcks;
//	INT		fpAckRate;
#endif

	// Calculate the time during which this link was actually transmitting to make sure we have enough
	// data to run the Adaptive Alg.  This is easy unless we are currently idle...

	tDelta = tNow - pEPD->tLastThruPutSample;

	if((pEPD->ulEPFlags & (EPFLAGS_SDATA_READY | EPFLAGS_IN_PIPELINE))==0)
	{
		ASSERT(pEPD->tStartIdle != -1);
		pEPD->uiTotalIdleTime += (tNow - pEPD->tStartIdle);
		pEPD->tStartIdle = tNow;
	}
	
	LOGPF(7, "Adaptive Alg tDelta = %d; Idle Time = %d (diff = %d)", tDelta, pEPD->uiTotalIdleTime, tDelta - pEPD->uiTotalIdleTime);

	tDelta -= pEPD->uiTotalIdleTime;

	// THIS PROBABLY IS UNNECESSARY NOW...
	if(tDelta <= 0)
	{
		LOGPF(7, "DELAYING Adaptive Alg");
		pEPD->uiAdaptAlgCount = 4;
		return;
	}

	//  Calculate current throughput acheived
	//
	//		We will determine the amount of time the link was not idle and then number of bytes (& frames) which
	//	was acknowleged by our partner.
	//
	//	tDelta = Time since last calculation minus the time the link was idle.
	
	uiBytesAcked = pEPD->uiBytesAcked - pEPD->uiLastBytesAcked;
	uiNewSum = pEPD->uiPeriodAcksBytes + (uiBytesAcked * 256);

	if(uiNewSum < pEPD->uiPeriodAcksBytes)
	{
		LOGPF(7, "THRUPUT is about to wrap. Correcting...");
		pEPD->uiPeriodAcksBytes /= 2;
		pEPD->uiPeriodXmitTime /= 2;
		pEPD->uiPeriodAcksBytes += (uiBytesAcked * 256);
	}
	else 
	{
		pEPD->uiPeriodAcksBytes = uiNewSum;
	}
	
	pEPD->uiPeriodXmitTime += tDelta;								// Track complete values for this period
	pEPD->uiTotalIdleTime = 0;
	pEPD->tLastThruPutSample = tNow;
	
	pEPD->uiLastBytesAcked = pEPD->uiBytesAcked;
	pEPD->uiPeriodRateB = pEPD->uiPeriodAcksBytes / pEPD->uiPeriodXmitTime;

	if(pEPD->uiPeriodRateB > pEPD->uiPeakRateB)
	{
		pEPD->uiPeakRateB = pEPD->uiPeriodRateB;					// Track the largest value we ever measure
	}
	
	LOGPF(7, "(%p) PERIOD COUNT BYTES = %d, XmitTime = %d  (%x)", pEPD, pEPD->uiPeriodAcksBytes, pEPD->uiPeriodXmitTime, pEPD->uiPeriodRateB);

	if(pEPD->ulEPFlags & EPFLAGS_LINK_STABLE)
	{
		/*		We are in a STABLE state,  meaning we think we are transmitting at an optimal
		**	rate for the current network conditions.  Conditions may change.  If things slow down
		**	or grow congested a Backoff will trigger normally.  Since conditions might also change
		**	for the better,  we will still want to periodically probe higher rates,  but much less
		**	often then when we are in DYNAMIC mode,  which means we are searching for an optimal rate.
		*/
		
		pEPD->uiAdaptAlgCount = 32;										// tNow + (pEPD->uiRTT * 32) + 32;
#ifdef POLL_COUNT	
		pEPD->uiPollFrequency = pEPD->iWindowF * 4;
#endif

		if((pEPD->ulEPFlags & EPFLAGS_LINK_FROZEN) == 0)
		{
			// BUGBUG we want this factor to grow as we continue to
			if((tNow - pEPD->tLastDelta) > pEPD->uiStaticPeriod)
			{
				if(pEPD->ulEPFlags & (EPFLAGS_FILLED_WINDOW_FRAME | EPFLAGS_FILLED_WINDOW_BYTE))
				{
					LOGPF(7, "(%p) RETURNING LINK TO DYNAMIC MODE", pEPD);
					
					pEPD->ulEPFlags &= ~(EPFLAGS_LINK_STABLE);

					pEPD->uiLastRateB = 0;
					pEPD->uiPeriodRateB = 0;
					pEPD->uiPeriodAcksBytes = 0;
					pEPD->uiPeriodXmitTime = 0;

					pEPD->uiWindowFilled = 0;
					pEPD->ulEPFlags &= ~(EPFLAGS_FILLED_WINDOW_FRAME | EPFLAGS_FILLED_WINDOW_BYTE);
					pEPD->uiAdaptAlgCount = 12;
				}
				else 
				{
					// SHRINK WINDOW HERE!!!! BUGBUG

					LOGPF(7, "(%p) NO WINDOWS FILLED,  Not returning to Dynamic Mode", pEPD);
					pEPD->uiStaticPeriod = MIN(pEPD->uiStaticPeriod * 2, INITIAL_STATIC_PERIOD * 8);
				}
			
			}
		}
	}

	// DYNAMIC STATE LINK
	else 
	{  
		pEPD->uiAdaptAlgCount = 8;

		// Possibly increase transmission rates.  We will not do this if we have had a ThrottleEvent
		// in recent memory,  or if we have not been actually transmitting for enough of the interval
		// to have collected worthwhile data
		//
		//		Also,  we dont want to even consider growing the send window unless we are consistantly
		// filling it.  Since one job of the window is to prevent us from flooding the net during a backup,
		// we dont want to grow the window following each backup.  The best way to distinguish between a 
		// backup and too small of a window is that the small window should fill up regularly while the
		// backups should only occur intermittantly.  The hard part is coming up with the actual test.
		// Truth is,  we can be fairly lax about allowing growth because it will also have to meet the increased
		// bandwidth test before the larger window is accepted.  So a crude rule would be to fix a number like 3.
		// Yes, crude but probably effective.  Perhaps a more reasonable figure would be a ratio of the total
		// number of packets sent divided by the window size.  I.e., if your window size is 10 frames then one
		// packet in ten should fill the window.  Of course, this would have to be calculated in bytes...

		if((pEPD->uiWindowFilled > 12)&&(pEPD->uiThrottleEvents == 0))
		{
			LOGPF(7, "(%p) DYNAMIC ALG: Window Fills: %d; B-Ack = (%x vs %x)", pEPD, pEPD->uiWindowFilled, pEPD->uiPeriodRateB, pEPD->uiLastRateB);
									
			pEPD->uiWindowFilled = 0;	
			
			// GETTING HERE means that we have used our current transmit parameters long enough
			// to have an idea of their performance.  We will now compare this to the performance
			// of the previous transmit parameters and we will either Revert to the previous set if
			// the perf is not improved,  or else we will advance to faster parameters if we did see
			// a jump.

			uiThreshhold = pEPD->uiLastRateB;	// BUGBUG -- scale this up by some small factor like 1/8 or 1/16
			if( pEPD->uiPeriodRateB > uiThreshhold)
			{
				GrowSendWindow(pEPD, tNow);
			}
			else 
			{
				// We did not see a thru-put improvement so we will back off the previous value
				// and transition the link to STABLE state. (actually we will try twice before stabilizing).

				LOGPF(7, "(%p) INSUFFICENT INCREASE IN THRUPUT", pEPD);

				if(pEPD->ulEPFlags & EPFLAGS_SECOND_SAMPLE)
				{
					LOGPF(7, "(%p) BACK OFF AND TRANSITION TO STABLE", pEPD);

#ifdef USE_BURST_GAP					
					pEPD->uiBurstGap = pEPD->uiGoodBurstGap;
					pEPD->uiBurstSize = pEPD->uiGoodBurstSize;
					pEPD->iBurstSizeBytes = (INT) (pEPD->uiBurstSize * pEPD->pSPD->uiFrameLength);
#endif					
					
					pEPD->iWindowF = pEPD->iGoodWindowF;
					pEPD->uiWindowBIndex = pEPD->uiGoodWindowBI;
					pEPD->iWindowB = pEPD->uiWindowBIndex * pEPD->pSPD->uiFrameLength;

					pEPD->ulEPFlags |= EPFLAGS_LINK_STABLE;				// TRANSITION TO STABLE STATE
					
					pEPD->ulEPFlags &= ~(EPFLAGS_SECOND_SAMPLE);

					dwSetPoint = (pEPD->iWindowF << 16) | pEPD->uiWindowBIndex;

					if(pEPD->dwSetPoint == dwSetPoint)
					{
						pEPD->uiStaticPeriod *= 2;
						LOGPF(7, "(%p) STABILIZED AT REPEATED SET-POINT.  New Static Period = %dms", pEPD, pEPD->uiStaticPeriod);
					}
					else 
					{
						pEPD->uiStaticPeriod = INITIAL_STATIC_PERIOD;
						pEPD->dwSetPoint = dwSetPoint;
						LOGPF(7, "(%p) Static Period = %dms", pEPD, pEPD->uiStaticPeriod);
					}
					
					pEPD->uiPeriodAcksBytes = 0;
					pEPD->uiPeriodXmitTime = 0;
					
					LOGPF(7, "(%p) ** TUNING LINK:  BurstGap=%d; BurstSize=%d; FWindow=%d, BWindow=%d (%d)",pEPD, pEPD->uiBurstGap, 	pEPD->uiBurstSize, pEPD->iWindowF, pEPD->iWindowB, pEPD->uiWindowBIndex);
				}
				else
				{
					//	We did not detect a significant increase in thruput since we last increased
					// our transmit rate.  Since there are variables in internet response and in client
					// behavior we will take another sample before determining that we have reached
					// a ceiling.  We will not toss our first data completely but we will scale back
					// both values in order to weight the new samples higher.
					//
					// Alternatively we could just zero them altogether...

					pEPD->ulEPFlags |= EPFLAGS_SECOND_SAMPLE;
					pEPD->uiPeriodAcksBytes = 0;
					pEPD->uiPeriodXmitTime = 0;
				}
			}
		}
		else 
		{
			LOGPF(7, "(%p) DYN ALG -- Not trying to increase:  WindowFills = %d, ThrottleCount = %d", pEPD, pEPD->uiWindowFilled, pEPD->uiThrottleEvents);
		}
	}	// END IF DYNAMIC STATE LINK
	
	pEPD->ulEPFlags &= ~(EPFLAGS_IDLED_PIPELINE);
}


/*
**		End Point Dropped Frame
**
**		We have two levels of Backoff.  We have an immediate BackOff implemented
**	upon first detection of a drop-event in order to relieve the congestion which
**	caused the drop.  An immediate backoff will resume transmitting at the original
**	rate without going through slow-start again after the congestion event has passed.
**	If we have multiple immediate-backoffs in a certain interval we will have a
**	hard backoff which will not restore.
**
**		The title says it all.  Keep books for EPD re: drop event.
**
**	CALLED WITH EPD->SendQLock held (and sometimes with StateLock held too)
*/

#undef DPF_MODNAME
#define DPF_MODNAME "EndPointDroppedFrame"

VOID
EndPointDroppedFrame(PEPD pEPD, DWORD tNow)
{
	pEPD->fpDropsPer128Frames += 0x01000000;

	// If this is first drop in short interval
	if(!pEPD->uiDropCount++)
	{	
		LOGPF(7, "DROP EVENT INITIATED: Throttling Back");
		ThrottleBack(pEPD, tNow);
	}
	
	// Multiple drops reported in short order
	else 
	{		

		// There are two possibilities and trick is to distinguish them.  Either we are still xmitting too fast
		// OR we are seeing additional drops caused by the previous xmit rate.  In first case we must back off
		// further.  In second case we should do nothing.  As a heuristic,  we can say we will ignore the second
		// and third drop,  but backoff further by the fourth.  Kinda kludgy but will probably work pretty well.
		// Robust solution would be to try to calculate back-log ala aaron,  but I am not convinced that we could
		// do that efficiently.  Alternatively,  we could just fake the calc for the back-off and back off some
		// fudge based upon RTT and outstanding frames.

		// Throttle back every fourth drop!
		if((pEPD->uiDropCount & 3) == 0)
		{			
			LOGPF(7, "(%p) THROTTLING BACK FURTHER", pEPD);
			ThrottleBack(pEPD, tNow);
		}
	}
}

/*
**		Throttle Back
**
**		We suspect network congestion due to dropped frames ((or a spike in latency)).  We want
**	to quickly scale back our transmit rate to releive the congestion and avoid further packet drops.
**	This is a temporary backoff and we will resume our current transmit rate when the congestions
**	clears.
**
**		If we find that we are throttling back frequently then we may conclude that our current xmit
**	rate is higher then optimal and we will BackOff to a lower rate,  and transition to a STABLE link
**	state (if not already there) to indicate that we have plateaued.
**
**		A note on convergence.  The ThrottleEvents variable is incremented 10 points each time a throttle
**	event is triggered.  This variable also decays slowly when the link is running without events.  So if
**	the variable grows faster then it decays we will eventually trigger a switch to STABLE state
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ThrottleBack"

VOID
ThrottleBack(PEPD pEPD, DWORD tNow)
{
	pEPD->ulEPFlags |= EPFLAGS_THROTTLED_BACK;		// Set link to THROTTLED state
	pEPD->uiThrottleEvents += 10;					// Count times we throttle-back for all reasons
	pEPD->tThrottleTime = tNow;						// Remember time that throttle was engaged
	
#ifdef	DEBUG
	pEPD->uiTotalThrottleEvents++;					// Count times we throttle-back for all reasons
#endif

	pEPD->uiRestoreBurstSize = pEPD->uiBurstSize;
	pEPD->uiRestoreBurstGap = pEPD->uiBurstGap;
	pEPD->iRestoreWindowF = pEPD->iWindowF;
	pEPD->uiRestoreWindowBI = pEPD->uiWindowBIndex;

#ifdef USE_BURST_GAP
	if(pEPD->iWindowF == 1)
	{
		// Don't backoff any more if the drop rate is not at least 5%

		if(pEPD->fpDropsPer128Frames < 0x06000000)
		{
			// BUGBUG: we might want to apply the 5% criteria even when not at 1 frame.
			
			if(pEPD->uiBurstGap == 0)
			{
				pEPD->uiBurstGap = MAX(1,pEPD->uiRTT/2);
				LOGPF(7, "(%p), first burst gap, set to %d ms", pEPD, pEPD->uiBurstGap);
			} 
			else 
			{
				pEPD->uiBurstGap = pEPD->uiBurstGap*2;						
				LOGPF(7, "(%p), burst gap doubled to %d ms", pEPD, pEPD->uiBurstGap);
			}
			pEPD->uiBurstGap = MIN(pEPD->uiBurstGap, MAX_RETRY_INTERVAL/2);
			LOGPF(7, "(%p), burst gap is now %d ms", pEPD, pEPD->uiBurstGap);
		} 
		else 
		{
			LOGPF(7, "(%p) Not increasing burst gap in throttle back because drop rate < 5 currently %d ms%", pEPD, pEPD->uiBurstGap);
		}
	}
#endif

	pEPD->iWindowF = MAX(pEPD->iWindowF / 2, 1);	// be sure window remains > 0.
	pEPD->uiWindowBIndex = MAX(pEPD->uiWindowBIndex / 2, 1);
	pEPD->iWindowB = pEPD->uiWindowBIndex * pEPD->pSPD->uiFrameLength;
	LOGPF(7, "(%p) THROTTLE ENGAGED (%d):  Backoff to Window=%d; Gap=%d; Size=%d", pEPD, pEPD->uiThrottleEvents, pEPD->iWindowF, pEPD->uiBurstGap, pEPD->uiBurstSize);
	
	if(pEPD->uiThrottleEvents > THROTTLE_EVENT_THRESHOLD)
	{
		LOGPF(7, "(%p) ** DETECT TRANSMIT CEILING ** Reducing 'good' speed and marking link STABLE", pEPD);

		// We have already reduced our current transmit rates.  Here we will reduce the "good" rates that
		// we will restore to when we clear the throttled state.
		
		pEPD->uiThrottleEvents = 0;

		pEPD->iRestoreWindowF = MAX((pEPD->iRestoreWindowF - 1), 1);
		pEPD->uiRestoreWindowBI = MAX((pEPD->uiRestoreWindowBI - 1), 1);

#ifdef USE_BURST_GAP		
		// If we are sending multi-bursts,  make them smaller
		if( pEPD->uiRestoreBurstSize > 1 )
		{			
			pEPD->uiRestoreBurstSize -= 1;				// Reduce burst!
			pEPD->iRestoreWindowF = pEPD->uiRestoreBurstSize;
		}
		else if (pEPD->uiRestoreBurstGap)
		{
			UINT t;
			t=pEPD->uiRestoreBurstGap;
			pEPD->uiRestoreBurstGap = (t+1) + (t >> 2); // 1.25*pEPD->uiRestoreBurstGap
		}
#endif
		LOGPF(7, "(%p) New Restore Values:  Window=%d; Gap=%d; Size = %d", pEPD, pEPD->iRestoreWindowF, pEPD->uiRestoreBurstGap, pEPD->uiRestoreBurstSize);

		pEPD->ulEPFlags |= EPFLAGS_LINK_STABLE;
	}
}


/*
**		Update End Point
**
**		We have just received a correlated response on a particular endpoint.
**	This means we can adjust our link calculations.  
**
**		Link timing calcs:  We are now calculating two different intervals for each data-point
**	packet.  We are calculating the total Round Trip Time (RTT) which is useful for calculating
**	retry timeout values,  and we are also attempting to calculate *changes* in the outbound
**	transit latency.  We can only calculate changes because we never truely synchronize clocks with
**	our partner.  We do track the clock bias though,  so we can infer a larger or shorter transmit latency
**	by a growing or shrinking clock bias.  I believe that we can ignore clock drift because we are more
**	interested in dramitic jumps or drops and can ignore a gradual change.
**
**		RTT Math - We will do our RTT math in fixed point 16.16 to allow for more
**	granularity in our moving averages.  So every time we compute a new RTT we will
**	also store the integer portion which is used in calculating timeouts.
**
**		Payload - Since on WANs the packet size can have a dramatic effect on the transmission latency,
**	we would like to factor it into our calculations (but only on links in WAN state).  We will track the
**	latency bias for minimal frames and for the largest frames that we see.  We will expect to see latencies
**	distributed over this range in proportion to the additional data in the frame.  I hope this works...
**
**		** CALLED WITH EPD STATELOCK HELD **
*/

#define		RTT_GROWTH_THRESHOLD			6					// consective samples of RTT incr to trigger BackOff
#define		RECOVERY_THRESHHOLD				3					// consecutive AdaptAlg runs before clearing BackOff state
#define		CEILING_THRESHHOLD				3					// ceiling hit occurances before switch to STABLE mode

#define		BURST_GAP_TOLERANCE				5					// Smallest increment to use for burst gaps


/*
**		Release End Point
**
**			We are finished communicating with this end point.  If this was the
**		last reference then we should free the descriptor and Disconnect the SP.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "ReleaseEndPoint"

VOID ReleaseEndPoint(PEPD pEPD, BOOL Locked)
{
	PSPD				pSPD = pEPD->pSPD;

	ASSERT((pEPD->ulEPFlags & EPFLAGS_CONNECTED)==0);	// clear connected state,  just is case...

	DisconnectConnection(pEPD);							// Close the endpoint in the Service Provider
	
	if(!Locked)
		Lock(&pEPD->pSPD->SendQLock);
	pEPD->blActiveLinkage.RemoveFromList(); 					// remove guy from active list
	if(!Locked)
		Unlock(&pEPD->pSPD->SendQLock);

	EPDPool->Release(EPDPool, pEPD);
}



/*
**		EPD Pool Support Routines
**
**		These are the functions called by Fixed Pool Manager as it handles EPDs.
*/

//	Allocate is called when a new EPD is first created

#define	pELEMENT	((PEPD) pElement)

#undef DPF_MODNAME
#define DPF_MODNAME "EPD_Allocate"

BOOL EPD_Allocate(PVOID pElement)
{
	//pELEMENT->blDatagramQueue.Initialize();
	//pELEMENT->blRelySendQ.Initialize();
	
	pELEMENT->blHighPriSendQ.Initialize();				// Can you beleive there are SIX send queues per Endpoint?
	pELEMENT->blNormPriSendQ.Initialize();				// Six send queues.  
	pELEMENT->blLowPriSendQ.Initialize();				// Well,  it beats sorting the sends into the queues upon submission.
	pELEMENT->blCompleteSendList.Initialize();
	
	pELEMENT->blSendWindow.Initialize();
	pELEMENT->blRetryQueue.Initialize();
	pELEMENT->blCompleteList.Initialize();
	pELEMENT->blOddFrameList.Initialize();
	pELEMENT->blChkPtQueue.Initialize();
	INITCRITSEC(&pELEMENT->EPLock);
	pELEMENT->Sign = EPD_SIGN;
	pELEMENT->pCurrentSend = NULL;
	pELEMENT->pCurrentFrame = NULL;
	pELEMENT->pCommand = NULL;

	pELEMENT->RetryTimer = 0;
	pELEMENT->ConnectTimer = 0;
	pELEMENT->DelayedAckTimer = 0;


	return TRUE;
}

//	Get is called each time an EPD is used

#undef DPF_MODNAME
#define DPF_MODNAME "EPD_Get"

VOID EPD_Get(PVOID pElement)
{
	LOGPF(5, "CREATING EPD %p", pELEMENT);

	pELEMENT->pNewMessage = NULL;
	pELEMENT->pNewTail = NULL;
	
	pELEMENT->ulEPFlags = 0;	// This line can go away...  can this function go away???
	pELEMENT->uiRefCnt = 0;

	ASSERT_EPD(pELEMENT);
}

#undef DPF_MODNAME
#define DPF_MODNAME "EPD_Release"

VOID EPD_Release(PVOID pElement)
{
	PCHKPT pCP;

	ASSERT_EPD(pELEMENT);

	LOGPF(5, "RELEASING EPD %p", pELEMENT);

	ASSERT((pELEMENT->ulEPFlags & EPFLAGS_LINKED_TO_LISTEN)==0);

	// These lists should be empty before End Point is released...

	ASSERT(pELEMENT->blOddFrameList.IsEmpty());
	ASSERT(pELEMENT->blSendWindow.IsEmpty());
	ASSERT(pELEMENT->blHighPriSendQ.IsEmpty());
	ASSERT(pELEMENT->blNormPriSendQ.IsEmpty());
	ASSERT(pELEMENT->blLowPriSendQ.IsEmpty());
	ASSERT(pELEMENT->blRetryQueue.IsEmpty());
	ASSERT(pELEMENT->pCurrentSend == NULL);
	ASSERT(pELEMENT->pCurrentFrame == NULL);
	
	// Clear any checkpoints still waiting on EP

	while(!pELEMENT->blChkPtQueue.IsEmpty()){
		pCP = CONTAINING_RECORD(pELEMENT->blChkPtQueue.GetNext(), CHKPT, blLinkage);
		pCP->blLinkage.RemoveFromList();
		ChkPtPool->Release(ChkPtPool, pCP);
	}
	pELEMENT->ulEPFlags = 0;
	pELEMENT->pCommand = NULL;
	pELEMENT->Context = NULL;
}

#undef DPF_MODNAME
#define DPF_MODNAME "EPD_Free"

VOID EPD_Free(PVOID pElement)
{
	DELETECRITSEC(&pELEMENT->EPLock);
}

#undef	ELEMENT
