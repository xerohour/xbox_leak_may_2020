/***************************************************************************\
* (C) Copyright NVIDIA Corporation Inc.,                                    *
*  1996,1997, 1998. All rights reserved.                                    *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: TransInit.c                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           8/17/98                                          *
*                                                                           *
\***************************************************************************/
// these are intented to be called from surfaces.c setupvp and teardownvp

// MS includes
#include "windows.h"
#include "ddraw.h"
//#include "Ddrawp.h"
#include "ddrawi.h"
#include "ddmini.h"
#include "dvp.h"
#include "dmemmgr.h"
#include "ddkmmini.h"
#include "ddkmapi.h"

#ifndef NV4
#include "real3nv32.h"
#include "nvwin32.h"
#include "nvos.h"
#else
#include "realnv32.h"
#include "nvwin32.h"
#include "nvos.h"
#endif  //NV4

#include "nvrmapi.h"
#include "nvrmarch.inc"

#include "surfaces.h"
#include "transfer.h"

extern vpSurfaces*			pMySurfaces;
extern NvNotification*		nvMyVPNotifiers;
NvChannel* 			nvPtr = NULL;


U032 SetupMTM() {
	U032	i;
	U032	error;
	nvPtr = pMySurfaces->pVPChanPtr;
	
	// allocate the MTM object
	error = NvRmAllocObject(  (GLOBDATAPTR)->ROOTHANDLE,
	                     		MY_VPE_CHANNEL,
	                     		MY_MTM_OBJECT,
#ifndef NV4
										NV_MEMORY_TO_MEMORY_FORMAT
#else
										NV03_MEMORY_TO_MEMORY_FORMAT
#endif// NV4
								);    
	if(checkNvAllocArchError(error )) {
		DPF("NVDD: Cannot allocate MTM object %d",MY_EXTERNAL_DECODER_OBJECT);
		return FALSE;
	}
	

	// allocate the notify context
	error = NvRmAllocContextDma(  (GLOBDATAPTR)->ROOTHANDLE,
	                     		MY_MTM_NOTIFIER,
	                     		NV01_CONTEXT_DMA,
	                     		(	ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR |
                        				ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
	                     		(PVOID)(((U032)nvMyVPNotifiers) + NUM039OFFSET*sizeof(NvNotification)),
	                     		(sizeof( NvNotification)*NUM039NOTIFIER  - 1)
	                     );
	if(checkNvAllocArchError(error )) {
		DPF("NVDD: Cannot allocate MTM Notifier object %d",MY_EXTERNAL_DECODER_OBJECT);
		return FALSE;
	}
	
	// allocate the FROM context
	error = NvRmAllocContextDma(  (GLOBDATAPTR)->ROOTHANDLE,
	                     		MY_MTM_FROM_CONTEXT,
	                     		NV01_CONTEXT_DMA,		//NV_CONTEXT_DMA_FROM_MEMORY ?
	                     		(	ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR |
                        				ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
	                     		(PVOID)(void*)((GLOBALDATA*)(pMySurfaces->pDriverData))->BaseAddress,
	                     		((((GLOBALDATA*)(pMySurfaces->pDriverData))->VideoHeapEnd - ((GLOBALDATA*)(pMySurfaces->pDriverData))->BaseAddress)) -1
	                     );

	if(checkNvAllocArchError(error )) {
		DPF("NVDD: Cannot allocate MTM FROM context object %d",MY_EXTERNAL_DECODER_OBJECT);
		return FALSE;
	}
	
	// allocate the MTM event
	error = NvRmAllocEvent	(	(GLOBDATAPTR)->ROOTHANDLE,
	 									MY_MTM_OBJECT,
	                      		MY_MTM_EVENT,
	                      		NV01_EVENT_KERNEL_CALLBACK,
										NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0),
										(void*)pMySurfaces->pMTMNotifyCallback);		    
							
	if(checkNvAllocArchError(error )) {
		DPF("NVDD: Cannot allocate MTM event object %d",MY_EXTERNAL_DECODER_OBJECT);
		return FALSE;
	} 
#if 0	
// second one until RM is working

	error = NvRmAllocEvent	(	(GLOBDATAPTR)->ROOTHANDLE,
	 									MY_MTM_OBJECT,
	                      		MY_MTM_EVENT2,
	                      		NV01_EVENT_KERNEL_CALLBACK,
										NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0),
										(void*)pMySurfaces->pMTMNotifyCallback);		    
							
	if(checkNvAllocArchError(error )) {
		DPF("NVDD: Cannot allocate MTM event object %d",MY_EXTERNAL_DECODER_OBJECT);
		return FALSE;
	} 
// done with second one
#endif

#ifndef NV4
	nvPtr->subchannel[5].control.object = MY_MTM_OBJECT;
#else 
	nvPtr->subchannel[5].SetObject  = MY_MTM_OBJECT;
#endif // NV4
	
			
#ifndef NV4
	nvPtr->subchannel[5].memoryToMemoryFormat.SetBufferNotifyCtxDma = MY_MTM_NOTIFIER;
	nvPtr->subchannel[5].memoryToMemoryFormat.SetBufferInCtxDma = MY_MTM_FROM_CONTEXT;
#else
	nvPtr->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaNotifies = MY_MTM_NOTIFIER;
	nvPtr->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaBufferIn = MY_MTM_FROM_CONTEXT;
#endif //NV4
	
	// Null out the transfers
	for(i=0;i<MAXTRANSFERS;i++) {
		pMySurfaces->MyTransfers[i].TransferID = TRANSFERNOTINUSE;
	}
//	pMySurfaces->pLastTransfer =
	pMySurfaces->pCurrentTransfer = NULL;
	pMySurfaces->bContextCreated	= FALSE;
}



void TearDownMTM() {
	U032	i;
	U032	error;
	nvPtr = pMySurfaces->pVPChanPtr;
	
	
	// set the transfers to something useless
	for(i=0;i<MAXTRANSFERS;i++) {
		pMySurfaces->MyTransfers[i].TransferID = 0;
	}
	
	if(nvPtr != NULL ) {
#ifndef NV4
		nvPtr->subchannel[5].control.object = MY_MTM_OBJECT;
#else 
		nvPtr->subchannel[5].SetObject  = MY_MTM_OBJECT;
#endif // NV4

																			// plug in some null objects
#ifndef NV4
		nvPtr->subchannel[5].memoryToMemoryFormat.SetBufferNotifyCtxDma = 0;
		nvPtr->subchannel[5].memoryToMemoryFormat.SetBufferInCtxDma = 0;
		nvPtr->subchannel[5].memoryToMemoryFormat.SetBufferOutCtxDma = 0;
#else
		nvPtr->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaNotifies = 0;
		nvPtr->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaBufferIn = 0;
		nvPtr->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaBufferOut = 0;
#endif //NV4


	// DO a SYNC here.
		// spin waiting for empty fifo
#ifdef NV4
		while(NvGetFreeCount(nvPtr, 5) < NV06A_FIFO_GUARANTEED_SIZE );
#else
		while(NvGetFreeCount(nvPtr, 5) < NV_GUARANTEED_FIFO_SIZE );
#endif
	
	}
	
	if(pMySurfaces->bContextCreated) {
		error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, (GLOBDATAPTR)->ROOTHANDLE, MY_MTM_TO_CONTEXT);

		checkNvAllocArchError(error);
		pMySurfaces->bContextCreated = FALSE;
	
	}

#ifdef NOEVENTFREEFUNCTIONYETFOLKS	
	error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, MY_MTM_OBJECT, MY_MTM_EVENT);
   checkNvAllocArchError(error);
#if 0
	error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, MY_MTM_OBJECT, MY_MTM_EVENT2);
   checkNvAllocArchError(error);
#endif
	
#endif
	
	error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, (GLOBDATAPTR)->ROOTHANDLE, MY_MTM_NOTIFIER);
	checkNvAllocArchError(error);
	error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, (GLOBDATAPTR)->ROOTHANDLE, MY_MTM_FROM_CONTEXT);
	checkNvAllocArchError(error);
	error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, MY_VPE_CHANNEL, MY_MTM_OBJECT);
	checkNvAllocArchError(error);
	
}