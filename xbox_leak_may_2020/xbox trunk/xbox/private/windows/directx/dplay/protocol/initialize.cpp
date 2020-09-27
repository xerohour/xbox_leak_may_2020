/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		Initialize.c
 *  Content:	This file contains code to both initialize and shutdown the
 *				protocol,  as well as to Add and Remove service providers
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *  11/06/98	ejs		Created
 *  08/15/00    RichGr  Bug #41363: Trigger timer and memory pool initialization at DLL startup,
 *                      but actually do it during the first DPlay8 object instantiation.  New functions
 *                      Pools_Pre_Init() and Pools_Deinit() are called from DNet.dll's DllMain.  
 *
 ****************************************************************************/

#include "dnproti.h"

class DIRECTNETOBJECT;

/*
**		GLOBAL VARIABLES
**
**			There are two kinds of global variables.  Instance specific globals
**	(not really global,  i know) which are members of the ProtocolData structure,
**	and true globals which are shared amonst all instances.  The following
**	definitions are true globals,  such as FixedPools and Timers.
*/

LPFPOOL			ChkPtPool = NULL;	// Pool of CheckPoint data structure
LPFPOOL			EPDPool = NULL;		// Pool of End Point descriptors
LPFPOOL			MSDPool = NULL;		// Pool of Message Descriptors
LPFPOOL			FMDPool = NULL;		// Pool of Frame Descriptors
LPFPOOL			RCDPool = NULL;		// Pool of Receive Descriptors

LPFPOOL			BufPool = NULL;		// Pool of buffers to store rcvd frames
LPFPOOL			MedBufPool = NULL;
LPFPOOL			BigBufPool = NULL;

BOOL volatile   g_bFirstTime;
DNCRITICAL_SECTION  g_FirstTimeCritSec;
extern CThreadPool *g_pThreadPool2;

HRESULT			TimerWorkaround = DPNERR_UNINITIALIZED;


/*
**		Pools Pre-Initialization
**
**		This procedure should be called by DllMain at startup time before
**      DPlay8 object instantiation.
*/
#undef DPF_MODNAME
#define DPF_MODNAME "Pools_Pre_Init"

void  Pools_Pre_Init()
{
    g_bFirstTime = TRUE;
    DNInitializeCriticalSection(&g_FirstTimeCritSec);
}

/*
**		Pools Initialization
**
**		This procedure should be called once only at DPlay8 object instantiation.
*/
#undef DPF_MODNAME
#define DPF_MODNAME "Pools_Init"

BOOL  Pools_Init()
{

	if((ChkPtPool = FPM_Create(sizeof(CHKPT), NULL, NULL, NULL, NULL)) == NULL){
		Pools_Deinit();
		return FALSE;
	}
	if((EPDPool = FPM_Create(sizeof(EPD), EPD_Allocate, EPD_Get, EPD_Release, EPD_Free)) == NULL){
		Pools_Deinit();
		return FALSE;
	}
	if((MSDPool = FPM_Create(sizeof(MSD), MSD_Allocate, MSD_Get, MSD_Release, MSD_Free)) == NULL){
		Pools_Deinit();
		return FALSE;
	}
	if((FMDPool = FPM_Create(sizeof(FMD), FMD_Allocate, FMD_Get, FMD_Release, FMD_Free)) == NULL){
		Pools_Deinit();
		return FALSE;
	}
	if((RCDPool = FPM_Create(sizeof(RCD), RCD_Allocate, RCD_Get, RCD_Release, RCD_Free)) == NULL){
		Pools_Deinit();
		return FALSE;
	}
	if((BufPool = FPM_Create(sizeof(BUF), Buf_Allocate, Buf_Get, NULL, NULL)) == NULL){
		Pools_Deinit();
		return FALSE;
	}
	if((MedBufPool = FPM_Create(sizeof(MEDBUF), Buf_Allocate, Buf_GetMed, NULL, NULL)) == NULL){
		Pools_Deinit();
		return FALSE;
	}
	if((BigBufPool = FPM_Create(sizeof(BIGBUF), Buf_Allocate, Buf_GetBig, NULL, NULL)) == NULL){
		Pools_Deinit();
		return FALSE;
	}
	if((TimerWorkaround = InitTimerWorkaround()) != DPN_OK){
		Pools_Deinit();
		return FALSE;
	}


    return TRUE;
}

/*
**		Pools Deinitialization
**
**		This procedure should be called by DllMain at shutdown time after
**      DPlay8 object release.
*/
#undef DPF_MODNAME
#define DPF_MODNAME "Pools_Deinit"

void  Pools_Deinit()
{

	if(TimerWorkaround == DPN_OK){
		FiniTimerWorkaround();
		TimerWorkaround = DPNERR_UNINITIALIZED;
	}
	if(ChkPtPool != NULL){
		ChkPtPool->Fini(ChkPtPool);
		ChkPtPool = NULL;
	}
	if(EPDPool != NULL){
		EPDPool->Fini(EPDPool);
		EPDPool = NULL;
	}
	if(MSDPool != NULL){
		MSDPool->Fini(MSDPool);
		MSDPool = NULL;
	}
	if(FMDPool != NULL){
		FMDPool->Fini(FMDPool);
		FMDPool = NULL;
	}
	if(RCDPool != NULL){
		RCDPool->Fini(RCDPool);
		RCDPool = NULL;
	}
	if(BufPool != NULL){
		BufPool->Fini(BufPool);
		BufPool = NULL;
	}
	if(MedBufPool != NULL){
		MedBufPool->Fini(MedBufPool);
		MedBufPool = NULL;
	}
	if(BigBufPool != NULL){
		BigBufPool->Fini(BigBufPool);
		BigBufPool = NULL;
	}

    DNDeleteCriticalSection(&g_FirstTimeCritSec);

    return;
}


/*
**		Protocol Initialize
**
**		This procedure should be called by DirectPlay at startup time before
**	any other calls in the protocol are made.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DNPProtocolInitialize"

HRESULT DNPProtocolInitialize(PVOID pCoreContext, PProtocolData pPData, PDN_PROTOCOL_INTERFACE_VTBL pVtbl)
{
	HRESULT			hr;
	UINT			InitState = 0;
    BOOL            bOK;

	LOGPF(2, "Parameters: pCoreContext[%p], pPData[%p], pVtbl[%p]", pCoreContext, pPData, pVtbl);

    //  8/15/00(RichGr) - Bug #41363: Timer and memory pool initialization is triggered at DLL startup,
    //     but actually done during the first DPlay8 object instantiation.
    if (g_bFirstTime)
    {
        DNEnterCriticalSection(&g_FirstTimeCritSec);
        bOK = TRUE;

        // Ask again in case 2 threads got thru the first "if" test.
        if (g_bFirstTime)
            bOK = Pools_Init();

        g_bFirstTime = FALSE;
        DNLeaveCriticalSection(&g_FirstTimeCritSec);

        if ( !bOK)
	    {
			LOGPF(0, "Returning DPNERR_OUTOFMEMORY - Couldn't initialize pools");
    	    return DPNERR_OUTOFMEMORY;
        }
    }

	pPData->ulProtocolFlags = 0;
	pPData->Parent = pCoreContext;	// Changed this for debug ONLY - MJN
	pPData->pfVtbl = pVtbl;
	pPData->Sign = PD_SIGN;

	pPData->lSPActiveCount = 0;
	
	// Init 1,  allocate End Point Descriptor stuff

	srand(GETTIMESTAMP());
	pPData->dwNextSessID = rand() | (rand() << 16);			// build a 32 bit value out of two 16 bit values

	pPData->tIdleThreshhold = DEFAULT_KEEPALIVE_INTERVAL;	// 60 second keep-alive interval
	pPData->dwConnectTimeout = CONNECT_DEFAULT_TIMEOUT;
	pPData->dwConnectRetries = CONNECT_DEFAULT_RETRIES;
	
	pPData->ulProtocolFlags |= PFLAGS_PROTOCOL_INITIALIZED;

	return DPN_OK;
}

/*
**		Protocol Shutdown
**
**		This procedure should be called at termination time,  and should be the
**	last call made to the protocol.
**
**		All SPs should have been removed prior to this call which in turn means
**	that we should not have any sends pending in a lower layer.
*/

#ifdef	DEBUG
extern	long ThreadsInReceive;
extern	long BuffersInReceive;
#endif

#undef DPF_MODNAME
#define DPF_MODNAME "DNPProtocolShutdown"

HRESULT DNPProtocolShutdown(PProtocolData pPData)
{
	LOGPF(2, "Parameters: pPData[%p]", pPData);

	if(pPData->lSPActiveCount != 0)
	{
		LOGPF(0, "Returning DPNERR_INVALIDCOMMAND - There are still active SPs, DNPRemoveSP wasn't called");
		return DPNERR_INVALIDCOMMAND;					// Must remove Service Providers first
	}

#ifdef DEBUG
	if (BuffersInReceive != 0)
	{
		LOGPF(0, "*** %d receive buffers were leaked", BuffersInReceive);	
	}
#endif

	pPData->ulProtocolFlags = 0;

	return	DPN_OK;
}

/*
**		Add Service Provider
**
**		This procedure is called by Direct Play to bind us to a service provider.
**	We can bind up to 256 service providers at one time,  although I would not ever
**	expect to do so.  This procedure will fail if Protocol Initialize has not
**	been called.
**
**
**		We check the size of the SP table to make sure we have a slot free.  If table
**	is full we double the table size until we reach maximum size.  If table cannot grow
**	then we fail the AddServiceProvider call.
*/

extern	IDP8SPCallbackVtbl DNPLowerEdgeVtbl;

#undef DPF_MODNAME
#define DPF_MODNAME "DNPAddServiceProvider"

HRESULT DNPAddServiceProvider(PProtocolData pPData, IDP8ServiceProvider *pISP, HANDLE *pContext)
{
	int			NewSize;
	PSPD		*pNewTable = NULL;
	PSPD		pSPD=0;
	SPINITIALIZEDATA	SPInitData;
	SPGETCAPSDATA		SPCapsData;
	HRESULT		hr;

	*pContext = NULL;

	LOGPF(2, "Parameters: pPData[%p], pISP[%p], pContext[%p]", pPData, pISP, pContext);

	if(pPData->ulProtocolFlags & PFLAGS_PROTOCOL_INITIALIZED)
	{
		if (SALLOC(pSPD, SPD, 1) == NULL)
		{
			LOGPF(0, "Returning DPNERR_OUTOFMEMORY - couldn't allocate SP Descriptor");
			return DPNERR_OUTOFMEMORY;
		}

		// MAKE THE INITIALIZE CALL TO THE Service Provider...  give him our Object

		MEMSET(pSPD, 0, sizeof(SPD));				// init to zero

		pSPD->LowerEdgeVtable = &DNPLowerEdgeVtbl;	// Put Vtbl into the interface Object
		pSPD->Sign = SPD_SIGN;

		SPInitData.pIDP = (IDP8SPCallback *) pSPD;
		SPInitData.dwFlags = 0;

		LOGPF(3, "Calling SP->Initialize");
		if((hr = IDP8ServiceProvider_Initialize(pISP, &SPInitData)) != DPN_OK)
		{
			FREE(pSPD);
			LOGPF(0, "Returning hr=%x - SP->Initialize failed", hr);
			return hr;
		}

		INITCRITSEC(&pSPD->SendQLock);
		pSPD->blSendQueue.Initialize();
		pSPD->blPendingQueue.Initialize();
		pSPD->blPipeQueue.Initialize();
		pSPD->blListenList.Initialize();
		pSPD->blEPDActiveList.Initialize();
		pSPD->blMessageList.Initialize();
		

		// MAKE THE SP GET CAPS CALL TO FIND FRAMESIZE AND LINKSPEED

		SPCapsData.dwSize = sizeof(SPCapsData);
		SPCapsData.hEndpoint = INVALID_HANDLE_VALUE;
		
		LOGPF(3, "Calling SP->GetCaps");
		if((hr = IDP8ServiceProvider_GetCaps(pISP, &SPCapsData)) != DPN_OK)
		{
			LOGPF(3, "Calling SP->Close");
			IDP8ServiceProvider_Close(pISP);
			DELETECRITSEC(&pSPD->SendQLock);
			FREE(pSPD);

			LOGPF(0, "Returning hr=%x - SP->GetCaps failed", hr);
			return hr;
		}

		pSPD->uiLinkSpeed = SPCapsData.dwLocalLinkSpeed;
		pSPD->uiFrameLength = SPCapsData.dwUserFrameSize;
		pSPD->uiUserFrameLength = pSPD->uiFrameLength - DNP_MAX_HEADER_SIZE;

		//	Place new SP in table

		LOGPF(3, "Calling SP->AddRef");
		IDP8ServiceProvider_AddRef(pISP);
		pSPD->IISPIntf = pISP;
		pSPD->pPData = pPData;
		InterlockedIncrement(&pPData->lSPActiveCount);
	}
	else
	{
		LOGPF(0, "Returning DPNERR_UNINITIALIZED - DNPInitialize has not been called");
		return DPNERR_UNINITIALIZED;
	}

	*pContext = pSPD;

	return DPN_OK;
}

/*
**		Remove Service Provider
**
**			It is higher layer's responsibility to make sure that there are no pending commands
**		when this function is called,  although we can do a certain amount of cleanup ourselves.
**		For the moment will we ASSERT that everything is in fact finished up.
**
**			SPTable stuff...  Since we use the table slot as the SP identiier,  we must not compact
**		the SP table upon removal.  Therefore,  we must have a way of validating the SP index,  and
**		we may not want to re-use table slots after an SP is removed.  Since we have virtually
**		unlimited space in the table,  and SPs are generally not intended to be transitory,  its
**		probably safe to invalidate the old table slot and just keep increasing the IDs.
*/

#undef DPF_MODNAME
#define DPF_MODNAME "DNPRemoveServiceProvider"

HRESULT DNPRemoveServiceProvider(PProtocolData pPData,  HANDLE hSPHandle)
{
	PSPD	pSPD = NULL;
	PEPD	pEPD;
	PMSD	pMSD;
	PFMD	pFMD;
	CBilink	*pLink;
	UINT	i;


	pSPD = (PSPD) hSPHandle;

	LOGPF(2, "Parameters: pPData[%p], hSPHandle[%x]", pPData, hSPHandle);
	
	if(pSPD == NULL)
	{
		LOGPF(0, "Returning DPNERR_INVALIDOBJECT - SP Handle is NULL");
		return DPNERR_INVALIDOBJECT;
	}

	pSPD->ulSPFlags |= SPFLAGS_TERMINATING;				// Nothing new gets in...

	// First cancel any pending listen
	while(!pSPD->blListenList.IsEmpty())
	{
		pLink = pSPD->blListenList.GetNext();
		pMSD = CONTAINING_RECORD(pLink, MSD, blQLinkage);
		LOGPF(1, "Pending Listen found when removing SP, pMSD[%p]", pMSD);

		if (FAILED(DNPCancelCommand(pPData, pMSD)))
		{
			// Cancel failed, so we don't want to sit in this loop forever
			pLink->RemoveFromList();
		}

		// EPDs may still hold a reference to the MSD.  It will be released below.
	}

	// Next we drop any open connections.  Drop code will cancel any pending sends.
/*	Lock(&pSPD->SendQLock);

#ifdef	DEBUG
	if(!pSPD->blEPDActiveList.IsEmpty())
	{
		for(pLink = pSPD->blEPDActiveList.GetNext(); pLink != &pSPD->blEPDActiveList; pLink = pLink->GetNext())
		{
			pEPD = CONTAINING_RECORD(pLink, EPD, blActiveLinkage);
			LOGPF(1, "Shutting down with Active EPD (%p) refcount=%d", pEPD, pEPD->uiRefCnt);
		}
	}
#endif

	Unlock(&pSPD->SendQLock);
	
	// Most likely reason that EPDs are not closed is that there are sends that haven't returned from the SP.
	// This is typical when apps shutdown immediately after Disconnect completes.  Best way to deal with this 
	// is to chill for a little while and see if things don't clean themselves up...

	for(i=0; (!pSPD->blEPDActiveList.IsEmpty()) && i<1000; i++)
	{
		if(i == 999)
		{
			LOGPF(1, "EPDs still active after waiting 10 seconds");
		}
		Sleep(10);
	}
*/
	// See if we still have a Send Event pending

	Lock(&pSPD->SendQLock);
	
	if(pSPD->SendHandle != NULL)
	{
		LOGPF(1, "Shutting down with send event still pending, cancelling, pSPD=[%p]", pSPD);
		
		if(CancelMyTimer(pSPD->SendHandle, pSPD->SendHandleUnique) == DPN_OK)
		{
			pSPD->SendHandle = NULL;
			pSPD->ulSPFlags &= ~(SPFLAGS_SEND_THREAD_SCHEDULED);
		}
		else 
		{
			LOGPF(1, "Failed to cancel send event", pSPD);
		}
	}

	while(!pSPD->blSendQueue.IsEmpty())
	{
		pFMD = CONTAINING_RECORD(pSPD->blSendQueue.GetNext(), FMD, blQLinkage);
		ASSERT_FMD(pFMD);

		LOGPF(1, "Cleaning FMD off of SendQueue pSPD=%p, pFMD=%p", pSPD, pFMD);

		pFMD->blQLinkage.RemoveFromList();
		RELEASE_EPD_LOCKED(pFMD->pEPD, "UNLOCK (Releasing Leftover CMD FMD)"); // SendQLock already held
		RELEASE_FMD(pFMD);
	}

	Unlock(&pSPD->SendQLock);

	while(pSPD->ulSPFlags & SPFLAGS_SEND_THREAD_SCHEDULED)
	{
		Sleep(10);
	}
	
	Lock(&pSPD->SendQLock);
	
	pLink = pSPD->blEPDActiveList.GetNext();
	while(pLink != &pSPD->blEPDActiveList)
	{
		BOOL fNeedToResetList = FALSE;
		pEPD = CONTAINING_RECORD(pLink, EPD, blActiveLinkage);
		pLink = pLink->GetNext();  // Set up for the next iteration
		ASSERT_EPD(pEPD);
		if(LOCK_EPD(pEPD, "LOCK (EPD LOOP)"))
		{
			if(!(pEPD->ulEPFlags & EPFLAGS_ABORT))
			{
				fNeedToResetList = TRUE;
				Unlock(&pSPD->SendQLock);
				Lock(&pEPD->EPLock);
				LOGPF(1, "Active EPD (%p) refcount=%d being forced closed", pEPD, pEPD->uiRefCnt);
				pEPD->ulEPFlags |= EPFLAGS_ABORT;
				DropLink(pEPD);				// Releases EPLock
				Lock(&pSPD->SendQLock);
			}

			RELEASE_EPD_LOCKED(pEPD, "UNLOCK (EPD LOOP)"); // SendQLock already held

			if (fNeedToResetList)
			{
				pLink = pSPD->blEPDActiveList.GetNext();
			}

		}
	}
	Unlock(&pSPD->SendQLock);

	// Sessions will not get cleaned up until any sends in SP complete (we have cancelled them if possible).
	// They will also wait to be cleared from the SendPipeline.

	for(i=0; (!pSPD->blEPDActiveList.IsEmpty()) && i<500; i++)
	{
#ifdef DPLAY_DOWORK
		if (g_pThreadPool2 != NULL)
		{
			g_pThreadPool2->PrimaryWin9xThread();
		}
#endif
		Sleep(10);
	}

	// By this time everything pending had better be gone!
	ASSERT(pSPD->blEPDActiveList.IsEmpty());
	ASSERT(pSPD->blSendQueue.IsEmpty());			// Should not be any frames on sendQ.

	Lock(&pSPD->SendQLock);
	while (!pSPD->blPendingQueue.IsEmpty())
	{
		pFMD = CONTAINING_RECORD(pSPD->blPendingQueue.GetNext(), FMD, blQLinkage);
		ASSERT_FMD(pFMD);

		pFMD->blQLinkage.RemoveFromList();

		LOGPF(1, "Cancelling Frame (%p) still on the pending queue", pFMD);
		Unlock(&pSPD->SendQLock);
		LOGPF(3, "Calling SP->CancelCommand");
		(void) IDP8ServiceProvider_CancelCommand(pSPD->IISPIntf, pFMD->SendDataBlock.hCommand, pFMD->SendDataBlock.dwCommandDescriptor);
		Lock(&pSPD->SendQLock);
	}
	Unlock(&pSPD->SendQLock);

	ASSERT(pSPD->blPendingQueue.IsEmpty());

	Lock(&pSPD->SendQLock);
	while ((pLink = pSPD->blMessageList.GetNext()) != &pSPD->blMessageList)
	{
		pMSD = CONTAINING_RECORD(pLink, MSD, blSPLinkage);
		ASSERT_MSD(pMSD);
		ASSERT(pMSD->ulMsgFlags1 & MFLAGS_ONE_ON_GLOBAL_LIST);
		LOGPF(0, "There are un-cancelled commands remaining on the Command List, Core didn't clean up properly - pMSD[%p], Context[%x]", pMSD, pMSD->Context);
		ASSERT(0);
	}
	Unlock(&pSPD->SendQLock);

	// Now that all frames are cleared out of SP,  there should be no more End Points waiting around to close
	//
	// If there are EPDs still active,  its because customer didnt issue a Disconnect or Abort.  Since all
	// commands and sends should be done,  there should be only ONE reference on any leftover EPDs.

	LOGPF(3, "Calling SP->Close");
	IDP8ServiceProvider_Close(pSPD->IISPIntf);
	LOGPF(3, "Calling SP->Release");
	IDP8ServiceProvider_Release(pSPD->IISPIntf);

	pSPD->ulSPFlags |= SPFLAGS_TERMINATED;				// Nothing new gets in...

	DELETECRITSEC(&pSPD->SendQLock);

	FREE(pSPD);

	ASSERT(pPData->lSPActiveCount > 0);
	InterlockedDecrement(&pPData->lSPActiveCount);

	return DPN_OK;
}


