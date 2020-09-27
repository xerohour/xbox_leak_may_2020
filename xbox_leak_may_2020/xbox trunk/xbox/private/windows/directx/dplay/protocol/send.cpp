/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		Send.c
 *  Content:	This file contains code which implements the front end of the
 *				SendData API.  It also contains code to Get and Release Message
 *				Descriptors (MSD) with the FPM package.
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *  11/06/98	ejs		Created
 *
 ****************************************************************************/


/*
**		Direct Net Protocol  --  Send Data
**
**		Data is always address to a PlayerID,  which is represented internally
**	by an End Point Descriptor (EPD).
**
**		Data can be sent reliably or unreliably using the same API with the appropriate
**	class of service flag set.
**
**		Sends are never delivered directly to the SP because there will always be
**	a possibility that the thread might block.  So to guarentee immediate return
**	we will always queue the packet and submit it on our dedicated sending thread.
*/

#include "dnproti.h"

#if (DN_SENDFLAGS_SET_USER_FLAG - PACKET_COMMAND_USER_1)
This will not compile.  Flags must be equal
#endif
#if (DN_SENDFLAGS_SET_USER_FLAG_TWO - PACKET_COMMAND_USER_2)
This will not compile.  Flags must be equal
#endif


//	locals

VOID	SendDatagram(PMSD, PEPD);
VOID	SendReliable(PMSD, PEPD);

#undef		DPF_MODNAME
#define		DPF_MODNAME		"PROTOCOL"

/*
**		Send Data
**
**		This routine will initiate a data transfer with the specified endpoint.  It will
**	normally start the operation and then return immediately,  returning a handle used to
**	indicate completion of the operation at a later time.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DNPSendData"

HRESULT
DNPSendData(	PProtocolData pPData,
				HANDLE hDestination,
				UINT uiBufferCount,
				PBUFFERDESC pBufferDesc,
				UINT uiPriority,
				UINT uiTimeout,
				ULONG ulFlags,
				PVOID pvContext,				// User context returned upon completion
				PHANDLE phHandle)				// Returned completion handle
{
	PEPD 			pEPD;
	PMSD			pMSD;
	PFMD			pFMD;
	UINT			i;
	UINT			Length = 0;
	PSPD			pSPD;
	ULONG			ulFrameFlags;
	BYTE			bCommand;
	//  Following variables are used for mapping buffers to frames
	PBUFFERDESC		FromBuffer, ToBuffer;
	UINT			TotalRemain, FromRemain, ToRemain, size;
	PCHAR			FromPtr;
#ifdef	DEBUG
	INT				FromBufferCount;
#endif
	// End of variables for mapping frames

	LOGPF(6, "Parameters: pPData[%p], hDestination[%x], uiBufferCount[%x], pBufferDesc[%p], uiPriority[%x], uiTimeout[%x], ulFlags[%x], pvContext[%p], phHandle[%p]", pPData, hDestination, uiBufferCount, pBufferDesc, uiPriority, uiTimeout, ulFlags, pvContext, phHandle);

	//* Unified Send Processing -- Do this for all classes of service
	
	// BUGBUG - Find out how many descriptors I might ever need worst case
	
	// Count the bytes in all user buffers
	for(i=0; i < uiBufferCount; i++)
	{
		Length += pBufferDesc[i].dwBufferSize;
	}

	pEPD = (PEPD) hDestination;
	
	ASSERT(pEPD != NULL);	
	ASSERT_EPD(pEPD);
	
	// Bump reference count on this baby
	if(!LOCK_EPD(pEPD, "LOCK (SEND)"))
	{
		LOGPF(0, "(%p) Rejecting Send on unreferenced EPD, returning DPNERR_INVALIDENDPOINT", pEPD);
		return DPNERR_INVALIDENDPOINT;
	}

	if( ((pEPD->ulEPFlags & (EPFLAGS_END_POINT_IN_USE | EPFLAGS_CONNECTED)) !=
														(EPFLAGS_END_POINT_IN_USE | EPFLAGS_CONNECTED))
														|| ((pEPD->ulEPFlags & EPFLAGS_TERMINATING) != 0)) 
	{
		RELEASE_EPD(pEPD, "UNLOCK (invalid EP)"); // SendQLock not already held
		LOGPF(0, "(%p) Rejecting Send on invalid EPD, returning DPNERR_INVALIDENDPOINT", pEPD);
		return DPNERR_INVALIDENDPOINT;
	}

	pSPD = pEPD->pSPD;
	
	ASSERT(pSPD != NULL);
	
	// Allocate and fill out a Message Descriptor for this operation
	if( (pMSD = static_cast<PMSD>( MSDPool->Get(MSDPool) )) == NULL)
	{
		RELEASE_EPD(pEPD, "UNLOCK (no memory)"); // SendQLock not already held
		LOGPF(0, "Failed to allocate MSD, returning DPNERR_OUTOFMEMORY");
		return DPNERR_OUTOFMEMORY;
	}

	// Copy SendData parameters into the Message Descriptor
	pMSD->pSPD = pSPD;
	pMSD->pEPD = pEPD;
	pMSD->ulSendFlags = ulFlags;					// Store the actual flags passed into the API call
	pMSD->Context = pvContext;
	pMSD->iMsgLength = Length;
	pMSD->tSubmitTime = GETTIMESTAMP();

	pMSD->uiFrameCount = (Length + pEPD->uiUserFrameLength - 1) / pEPD->uiUserFrameLength; // round up

	if(ulFlags & DN_SENDFLAGS_RELIABLE)
	{
		pMSD->CommandID = COMMAND_ID_SEND_RELIABLE;
		ulFrameFlags = FFLAGS_RELIABLE;
		bCommand = PACKET_COMMAND_DATA | PACKET_COMMAND_RELIABLE;
	}
	else 
	{
		pMSD->CommandID = COMMAND_ID_SEND_DATAGRAM;
		ulFrameFlags = 0;
		bCommand = PACKET_COMMAND_DATA;
	}

	if(!(ulFlags & DN_SENDFLAGS_NON_SEQUENTIAL))
	{
		ulFrameFlags |= FFLAGS_SEQUENTIAL;
		bCommand |= PACKET_COMMAND_SEQUENTIAL;
	}

	bCommand |= (ulFlags & (DN_SENDFLAGS_SET_USER_FLAG | DN_SENDFLAGS_SET_USER_FLAG_TWO));	// preserve user flag values

	// Map user buffers directly into frame's buffer descriptors
	//
	//	We will loop through each required frame,  filling out buffer descriptors
	// from those provided as parameters.  Frames may span user buffers or vica-versa...

	TotalRemain = Length;
#ifdef	DEBUG
	FromBufferCount = uiBufferCount - 1;				// sanity check
#endif
	FromBuffer = pBufferDesc;
	FromRemain = FromBuffer->dwBufferSize;
	FromPtr = reinterpret_cast<PCHAR>( (FromBuffer++)->pBufferData );				// note post-increment to next descriptor
	
	for(i=0; i<pMSD->uiFrameCount; i++)
	{
		ASSERT(TotalRemain > 0);						// BUGBUG: Double check our math
		
		// Grab a new frame
		if( (pFMD = static_cast<PFMD>( FMDPool->Get(FMDPool) )) == NULL)
		{	
			// FAILED - Return resources and fail the send
			Lock(&pMSD->CommandLock);
			RELEASE_MSD(pMSD, "Release On FMD Get Failed");	// MSD Release operation will also free frames
			LOGPF(0, "Failed to allocate FMD, returning DPNERR_OUTOFMEMORY");
			return DPNERR_OUTOFMEMORY;
		}

		pFMD->pMSD = pMSD;								// Link frame back to message
		pFMD->pEPD = pEPD;
		pFMD->CommandID = pMSD->CommandID;
		pFMD->bPacketFlags = bCommand;					// save packet flags for each frame
		pFMD->blMSDLinkage.InsertBefore( &pMSD->blFrameList);
		ToRemain = pEPD->uiUserFrameLength;
		ToBuffer = pFMD->rgBufferList;					// Address first user buffer desc
		//FrameDescCount = 0;							// Only necessary if we allow >12 user buffers
		
		pFMD->uiFrameLength = pEPD->uiUserFrameLength;	// Assume we fill frame- only need to change size of last one
		pFMD->ulFFlags = ulFrameFlags;					// Set control flags for frame (Sequential, Reliable)

		// Until this frame is full
		while((ToRemain != 0) && (TotalRemain != 0))
		{	
 			size = MIN(FromRemain, ToRemain);			// choose smaller of framesize or buffersize
			FromRemain -= size;
			ToRemain -= size;
			TotalRemain -= size;

			ToBuffer->dwBufferSize = size;				// Fill in the next frame descriptor
			(ToBuffer++)->pBufferData = reinterpret_cast<BYTE*>( FromPtr );		// note post-increment
			pFMD->SendDataBlock.dwBufferCount++;		// Count buffers as we add them
			//FrameDescCount++;

			// Get next user buffer
			if((FromRemain == 0) && (TotalRemain != 0))
			{
				FromRemain = FromBuffer->dwBufferSize;
				FromPtr = reinterpret_cast<PCHAR>( (FromBuffer++)->pBufferData );	// note post-increment to next descriptor
#ifdef	DEBUG		
				FromBufferCount--;						// Keep this code honest...
				ASSERT(FromBufferCount >= 0);
#endif
			}
			else {										// Either filled this frame,  or have mapped the whole send
				FromPtr += size;						// advance ptr to start next frame (if any)
				pFMD->uiFrameLength = pEPD->uiUserFrameLength - ToRemain;		// wont be full at end of message
			}
		}	// While (frame not full)
	}  // For (each frame in message)

	pFMD->ulFFlags |= FFLAGS_END_OF_MESSAGE;			// Mark last frame with EOM
	pFMD->bPacketFlags |= PACKET_COMMAND_END_MSG;		// Set EOM in frame
	
#ifdef	DEBUG
	ASSERT(FromBufferCount == 0);
#endif
	ASSERT(TotalRemain == 0);

	// hang the message off a global command queue for timeout and cancel tracking

	Lock(&pMSD->pSPD->SendQLock);
	pMSD->blSPLinkage.InsertBefore( &pMSD->pSPD->blMessageList);
	pMSD->ulMsgFlags1 |= MFLAGS_ONE_ON_GLOBAL_LIST;
	Unlock(&pMSD->pSPD->SendQLock);

	*phHandle = pMSD;									// We will use the MSD as our handle.

	//	To implement priority-based sending I have unified the Datagram and Reliable traffic into a single set of
	//  three priority based queues.  Therefore we will insert new messages in the back of the appropriate queue
	//	regardless of their reliability.


	Lock(&pEPD->EPLock);												// LOCK ALL SEND QUEUES WITH SINGLE CRIT SEC
	
	if(uiTimeout != 0)
	{
		LOCK_MSD(pMSD, "Send Timeout Timer");							// Add reference for timer
		LOGPF(7, "(%p) Setting Timeout Send Timer", pEPD);
		SetMyTimer(uiTimeout, 100, TimeoutSend, pMSD, &pMSD->TimeoutTimer, &pMSD->TimeoutTimerUnique);
	}
	
	EnqueueMessage(pMSD, pEPD);
	Unlock(&pEPD->EPLock);

	return DPNERR_PENDING;
}

/*
**		Enqueue Message
**
**		Add complete MSD to the appropriate send queue,  and kick start sending process if necessary.
**
**		** This routine is called and returns with EPD->EPLOCK held **
*/

#undef DPF_MODNAME
#define DPF_MODNAME "EnqueueMessage"

VOID
EnqueueMessage(PMSD pMSD, PEPD pEPD)
{
	//	Place Message in appriopriate priority queue.  Datagrams get enqueued twice (!).  They get put in the Master
	// queue where they are processed FIFO with all messages of the same priority.  Datagrams also get placed in a priority
	// specific queue of only datagrams which is drawn from when the reliable stream is blocked.
	
	if(pMSD->ulSendFlags & DN_SENDFLAGS_HIGH_PRIORITY)
	{
		LOGPF(7, "(%p) Placing message on High Priority Q", pEPD);
		pMSD->blQLinkage.InsertBefore( &pEPD->blHighPriSendQ);
		pEPD->uiMsgSentHigh++;
	}
	else if (pMSD->ulSendFlags & DN_SENDFLAGS_LOW_PRIORITY)
	{
		LOGPF(7, "(%p) Placing message on Low Priority Q", pEPD);
		pMSD->blQLinkage.InsertBefore( &pEPD->blLowPriSendQ);
		pEPD->uiMsgSentLow++;
	}
	else
	{
		LOGPF(7, "(%p) Placing message on Normal Priority Q", pEPD);
		pMSD->blQLinkage.InsertBefore( &pEPD->blNormPriSendQ);
		pEPD->uiMsgSentNorm++;
	}

	pMSD->ulMsgFlags2 |= MFLAGS_TWO_ENQUEUED;

	pEPD->uiQueuedMessageCount++;
	pEPD->ulEPFlags |= EPFLAGS_SDATA_READY;							// Note that there is *something* in one or more queues

	// If the session is not currently in the send pipeline then we will want to insert it here as long as the
	// the stream is not blocked.

	if(((pEPD->ulEPFlags & EPFLAGS_IN_PIPELINE)==0) && (pEPD->ulEPFlags & EPFLAGS_STREAM_UNBLOCKED))
	{
		ASSERT(pEPD->SendTimer == NULL);
		LOGPF(7, "(%p) Send On Idle Link -- Returning to pipeline", pEPD);
	
		pEPD->uiTotalIdleTime += ( GETTIMESTAMP() - pEPD->tStartIdle );

#ifdef	DEBUG
		ASSERT(pEPD->tStartIdle != -1);
		pEPD->tStartIdle = -1;
#endif

		pEPD->ulEPFlags |= EPFLAGS_IN_PIPELINE;
		LOCK_EPD(pEPD, "LOCK (pipeline)");								// Add Ref for pipeline Q

		// We dont call send on users thread,  but we dont have a dedicated send thread either. Use a thread
		// from the timer-worker pool to submit the sends to SP

		LOGPF(7, "(%p) Scheduling Timer Thread", pEPD);
		ScheduleTimerThread(ScheduledSend, pEPD, &pEPD->SendTimer, &pEPD->SendTimerUnique);
	}
}


#undef DPF_MODNAME
#define DPF_MODNAME "TimeoutSend"

VOID CALLBACK
TimeoutSend(PVOID uID, UINT uMsg, PVOID dwUser)
{
	PMSD	pMSD = (PMSD) dwUser;
	PEPD	pEPD = pMSD->pEPD;
	ULONG	Flags = pMSD->ulSendFlags;

	LOGPF(7, "Timeout Send pMSD=%p,  RefCnt=%d", pMSD, pMSD->uiRefCnt);

	Lock(&pMSD->CommandLock);
	
	pMSD->TimeoutTimer = NULL;

	if((pMSD->ulMsgFlags1 & (MFLAGS_ONE_CANCELLED | MFLAGS_ONE_TIMEDOUT)) || (pMSD->ulMsgFlags2 & MFLAGS_TWO_COMPLETE))
	{
		LOGPF(7, "Timed out send has completed pMSD=%p", pMSD);
		RELEASE_MSD(pMSD, "Send Timout Timer");
		return;
	}

	pMSD->ulMsgFlags1 |= MFLAGS_ONE_TIMEDOUT;

	DECREMENT_MSD(pMSD, "Send Timout Timer");							// Release Ref for timer
	
	LOGPF(7, "Calling DoCancel to cancel pMSD=%p", pMSD);

	if(DoCancel(pMSD, DPNERR_TIMEDOUT) == DPN_OK)
	{
		if(Flags & DN_SENDFLAGS_HIGH_PRIORITY)
		{
			pEPD->uiMsgTOHigh++;
		}
		else if(Flags & DN_SENDFLAGS_LOW_PRIORITY)
		{
			pEPD->uiMsgTOLow++;
		}
		else
		{
			pEPD->uiMsgTONorm++;
		}
	}
	else
	{
		LOGPF(7, "DoCancel did not succeed pMSD=%p", pMSD);
	}
}


/***********************
========SPACER==========
************************/

/*
**		MSD Pool support routines
**
**		These are the functions called by Fixed Pool Manager as it handles MSDs.
*/

#define	pELEMENT		((PMSD) pElement)

#undef DPF_MODNAME
#define DPF_MODNAME "MSD_Allocate"

BOOL MSD_Allocate(PVOID pElement)
{
	ZeroMemory(pELEMENT, sizeof(messagedesc));

	INITCRITSEC(&pELEMENT->CommandLock);
	
	pELEMENT->blFrameList.Initialize();
	pELEMENT->blQLinkage.Initialize();
	pELEMENT->blSPLinkage.Initialize();
	pELEMENT->Sign = MSD_SIGN;
	pELEMENT->uiRefCnt = -1;

	return TRUE;
}

//	Get is called each time an MSD is used


#undef DPF_MODNAME
#define DPF_MODNAME "MSD_Get"

VOID MSD_Get(PVOID pElement)
{
	ASSERT(pELEMENT->uiRefCnt == -1);
	
	LOGPF(5, "CREATING MSD %p", pELEMENT);

	pELEMENT->CommandID = COMMAND_ID_NONE;
	pELEMENT->ulMsgFlags1 = MFLAGS_ONE_IN_USE;	// Dont need InUse flag since we have RefCnt
	pELEMENT->uiRefCnt = 0;
}

/*
**	MSD Release
**
**		This is called with the CommandLock held.  The Lock should not be
**	freed until the INUSE flag is cleared.  This is to synchronize with
**	last minute Cancel threads waiting on lock.
**
**		When freeing a message desc we will free all frame descriptors
**	attached to it first.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "MSD_Release"

VOID MSD_Release(PVOID pElement)
{
	CBilink	*pLink;
	PFMD	pFMD;

	ASSERT_MSD(pELEMENT);

	LOGPF(5, "RELEASING MSD %p", pELEMENT);

	ASSERT(pELEMENT->ulMsgFlags1 & MFLAGS_ONE_IN_USE);
	ASSERT(pELEMENT->uiRefCnt == -1);
	ASSERT((pELEMENT->ulMsgFlags1 & MFLAGS_ONE_ON_GLOBAL_LIST)==0);

	while( (pLink = pELEMENT->blFrameList.GetNext()) != &pELEMENT->blFrameList){
		pLink->RemoveFromList();							// remove from bilink

		pFMD = CONTAINING_RECORD(pLink, FMD, blMSDLinkage);
		RELEASE_FMD(pFMD);								// If this is still submitted it will be referenced and wont be released here.
	}
	pELEMENT->ulMsgFlags1 = 0;
	pELEMENT->ulMsgFlags2 = 0;
	Unlock(&pELEMENT->CommandLock);
}

#undef DPF_MODNAME
#define DPF_MODNAME "MSD_Free"

VOID MSD_Free(PVOID pElement)
{
	DELETECRITSEC(&pELEMENT->CommandLock);
}

#undef	pELEMENT

/*
**		FMD Pool support routines
*/

#define	pELEMENT		((PFMD) pElement)

#undef DPF_MODNAME
#define DPF_MODNAME "FMD_Allocate"

BOOL FMD_Allocate(PVOID pElement)
{
	pELEMENT->Sign = FMD_SIGN;
	pELEMENT->ulFFlags = 0;
	pELEMENT->uiRefCnt = 0;
	
	return TRUE;
}

//	Get is called each time an MSD is used
//
//	Probably dont need to do this everytime,  but some random SP might
//	munch the parameters someday and that could be bad if I dont...

#undef DPF_MODNAME
#define DPF_MODNAME "FMD_Get"

VOID FMD_Get(PVOID pElement)
{
	LOGPF(5, "CREATING FMD %p", pELEMENT);

	pELEMENT->CommandID = 0;
	pELEMENT->lpImmediatePointer = (LPVOID) pELEMENT->ImmediateData;
	pELEMENT->SendDataBlock.pBuffers = (PBUFFERDESC) &pELEMENT->uiImmediateLength;
	pELEMENT->SendDataBlock.dwBufferCount = 1;				// always count one buffer for immediate data
	pELEMENT->SendDataBlock.dwFlags = 0;
	pELEMENT->SendDataBlock.pvContext = pElement;
	pELEMENT->SendDataBlock.hCommand = 0;
	pELEMENT->ulFFlags = FFLAGS_IN_USE;
	pELEMENT->bSubmitted = FALSE;
	
	ASSERT(pELEMENT->uiRefCnt == 0);
	pELEMENT->uiRefCnt = 1;						// Assign first reference
}

#undef DPF_MODNAME
#define DPF_MODNAME "FMD_Release"

VOID FMD_Release(PVOID pElement)
{
	LOGPF(5, "RELEASING FMD %p", pELEMENT);

	ASSERT_FMD(pELEMENT);
	ASSERT(pELEMENT->uiRefCnt == 0);
	ASSERT(pELEMENT->bSubmitted == FALSE);
	pELEMENT->ulFFlags &= ~FFLAGS_IN_USE;	
	pELEMENT->pMSD = NULL;
}

#undef DPF_MODNAME
#define DPF_MODNAME "FMD_Free"

VOID FMD_Free(PVOID pElement)
{
}

#undef	pELEMENT


