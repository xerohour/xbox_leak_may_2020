/***************************************************************************\
* (C) Copyright NVIDIA Corporation Inc.,                                    *
*  1996,1997, 1998. All rights reserved.                                    *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: transfer.c                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           8/17/98                                          *
*                                                                           *
\***************************************************************************/

#ifdef NV4_HW
#define NV4
#endif
#ifdef NV10_HW
#define NV4
#endif

// to make the references to channel data cool here in ring0
#define INVDDCODE

// MS includes
#include "windows.h"
#include "ddraw.h"
#include "Ddrawp.h"
#include "ddrawi.h"
#include "ddmini.h"
#include "dvp.h"
#include "dmemmgr.h"

//  vdd specific MS includes
#include "minivdd.h"
#include "ddkmmini.h"
#include "nvos.h"

// NV includes
#include "nvwin32.h"
#include "nvhw.h"
#include <nv_ref.h>
#include "nvrmr0api.h"

// This is here because it is defined in nvrmarch.inc. We can't include that file here.
#define NV_WIN_DEVICE                            0x00002002

// unit specific includes
#define RESMANSIDE
#include "surfaces.h"
#include "vddsurfs.h"
#include "vpvdd.h"

#include "transfer.h"
#include "timing.h"
#define USERING0LIBRARY
#ifndef  USERING0LIBRARY

#include "nvkrmapi.c"

#endif //USERING0LIBRARY

//
// prototypes
//
U032    getSurfaceIndex     (U032);

//#define  DPF(a) _Debug_Printf_Service(a)

//
// Use static storage for now.  assume no more than MAXTRANSFERS transfers queued.
//
extern  vpSurfaces 			MySurfaces;		
extern  NvNotification		nvMyVPNotifiers[NUMVPNOTIFIERS];

#ifndef DPF
#define DPF(a) NULL
#endif

U032 checkNvAllocArchError(U032 error) {
	// returns true if there IS an error
	// otherwise false
	switch (error) {
			case NVOS04_STATUS_SUCCESS:
				return 0;
//				DPF("NVOS04_STATUS_SUCCESS");
				break;
			case NVOS04_STATUS_ERROR_OPERATING_SYSTEM:
				DPF("NVOS04_STATUS_ERROR_OPERATING_SYSTEM");
				return error;
				break;
			case NVOS04_STATUS_ERROR_BAD_OBJECT_PARENT:
				DPF("NVOS04_STATUS_ERROR_BAD_OBJECT_PARENT");
				return error;
				break;
			case NVOS04_STATUS_ERROR_BAD_OBJECT_NEW:
				DPF("NVOS04_STATUS_ERROR_BAD_OBJECT_NEW");
				return error;
				break;
			case NVOS04_STATUS_ERROR_BAD_CLASS:
				DPF("NVOS04_STATUS_ERROR_BAD_CLASS");
				return error;
				break;
			case NVOS04_STATUS_ERROR_BAD_OBJECT_ERROR:
				DPF("NVOS04_STATUS_ERROR_BAD_OBJECT_ERROR");
				return error;
				break;
			case NVOS04_STATUS_ERROR_BAD_FLAGS:
				DPF("NVOS04_STATUS_ERROR_BAD_FLAGS");
				return error;
				break;
			case NVOS04_STATUS_ERROR_INSUFFICIENT_RESOURCES:
				DPF("NVOS04_STATUS_ERROR_INSUFFICIENT_RESOURCES");
				return error;
				break;
			default:
					DPF("UNKNOWN ERROR");
					return 1;
	}
	
	return 0;
}


pTransfer AddTransfer(U032 TransferID) {
	pTransfer	pMyTransfer;
	U032 i;
	// check for duplicate transfer ID
	for(i=0;i<MAXTRANSFERS;i++) {
		if(MySurfaces.MyTransfers[i].TransferID == TransferID) {
			DPF("That ID already in use!");
			return NULL;
		}
	}
	
	// check for room in queue?
	
	if(MySurfaces.dwNextEmpty == MySurfaces.dwLastEmpty ) {
		return NULL;
	}
	
	
	pMyTransfer = &(MySurfaces.MyTransfers[MySurfaces.dwNextEmpty]);
	pMyTransfer->TransferID = TransferID;
	
	MySurfaces.dwNextEmpty = MySurfaces.dwNextEmpty+1;
	if(MySurfaces.dwNextEmpty >= MAXTRANSFERS )
		MySurfaces.dwNextEmpty = 0;
		
	return pMyTransfer;
#if 0	
	for(i=0;i<MAXTRANSFERS;i++) {
		if(MySurfaces.MyTransfers[i].TransferID == TRANSFERNOTINUSE ) {
			U032 temp =i +1;
			pMyTransfer = &(MySurfaces.MyTransfers[i]);
			pMyTransfer->TransferID = TransferID;
			
			if(temp >= MAXTRANSFERS ) {
				temp = 0;
			}
			pMyTransfer->pNextTransfer = (U032*) &(MySurfaces.MyTransfers[temp]);
			return pMyTransfer;
		}
	}
#endif
	// couldn't find an empty Transfer to use
	DPF("couldn't find an empty Transfer to use");
	return NULL;	
}


U032 DeleteTransfer(pTransfer pTrans) {
	U032	temp;
	// theoretically the one we're deleting here should be the same as the "last full"
	// unless we're the first 
	// find the transfer that's one ahead of the nextEmpty zone
	temp = MySurfaces.dwLastEmpty +1;
	if(temp >= MAXTRANSFERS ) {
		temp = 0;
	}
	// make sure it's the one we want to delete;
	if(pTrans != &(MySurfaces.MyTransfers[temp]) ) {
		DPF("Major error here. the one we're trying to delete is NOT in order ");
	}
	// delete it!
	pTrans->TransferID = TRANSFERNOTINUSE;
	pTrans->SurfaceData = 0;
	pTrans->StartLine =0;
	pTrans->EndLine = 0;
	pTrans->DestMDL = NULL;
	pTrans->TransferFlags = 0;
	pTrans->pNextTransfer = NULL;
	MySurfaces.dwLastEmpty = temp;
	
	
	return 0	;
}


pTransfer GetNextTransfer() {
	pTransfer pRetValue;
	U032	temp;
	
	// find the transfer that's one ahead of the nextEmpty zone
	temp = MySurfaces.dwLastEmpty +1;
	if(temp >= MAXTRANSFERS ) {
		temp = 0;
	}
	// get a pointer to it
	pRetValue = &MySurfaces.MyTransfers[temp];
	// make sure it's QUEUED
	if(pRetValue->transferStatus != QUEUED) { 
		// if not return NULL;
		return NULL;
	}
	// else return that one!
	MySurfaces.pCurrentTransfer = pRetValue;
	return pRetValue;
	
}


void ReCreateContext(PMDL aDestMDL) {
	U032	error;
	U032	start;
	U032	limit;
	NvChannel*	pMyNvChan = MySurfaces.pVPChanPtr;
	
	// first check to see if we can reuse the OLD context... since context creation is expensive.
	start = (U032)(aDestMDL->lpMappedSystemVa);
	limit = (aDestMDL->ByteCount  ) -1;
	
	if(MySurfaces.bContextCreated) {
		if((start == MySurfaces.savedStart) && (limit == MySurfaces.savedLimit) ) {
			// no problem this is the same thing, so don't recreate
			return;
		}
	}
	ASSERT(4*4 < NV06A_FIFO_GUARANTEED_SIZE );			
	while (NvGetFreeCount(pMyNvChan, 5) < 4*4 )	{
			NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);	
	}					
	
	if(MySurfaces.bContextCreated) {
		// set context on MTM to NULLOBJECT
		
#ifdef NV3_HW
		pMyNvChan->subchannel[5].control.object = MY_MTM_OBJECT;
#else 
		pMyNvChan->subchannel[5].SetObject  = MY_MTM_OBJECT;
#endif // NV3_HW

#ifdef NV3_HW
		pMyNvChan->subchannel[5].memoryToMemoryFormat.SetBufferOutCtxDma = 0;
#else
		pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaBufferOut = 0;
#endif
		// FREE the context first
		//   DO A SOFTWARE METHOD FLUSH HERE
		
#ifdef NV3_HW
		while (NvGetFreeCount(pMyNvChan, 5) < NV_GUARANTEED_FIFO_SIZE )	{
#else
		while (NvGetFreeCount(pMyNvChan, 5) < NV06A_FIFO_GUARANTEED_SIZE )	{
#endif
				NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);	
		}

		error = NvRmR0Free((GLOBDATAPTR)->ROOTHANDLE, (GLOBDATAPTR)->ROOTHANDLE, MY_MTM_TO_CONTEXT);

		checkNvAllocArchError(error);

		MySurfaces.bContextCreated = FALSE;
	}
	
	// ALLOC the context
	// allocate the TO context
//	DPF("ReCreateContext: Now creating a context which starts at %x",(U032)(aDestMDL->lpStartVa));
//	DPF("ReCreateContext:                            and runs to %x",(U032)((aDestMDL->ByteCount + aDestMDL->ByteOffset ) -1));
	
	
	error = NvRmR0AllocContextDma(  (GLOBDATAPTR)->ROOTHANDLE,
	                     		MY_MTM_TO_CONTEXT,
	                     		NV01_CONTEXT_DMA,		//NV_CONTEXT_DMA_FROM_MEMORY ?
	                     		(	NVOS03_FLAGS_ACCESS_READ_WRITE |
                        				NVOS03_FLAGS_COHERENCY_UNCACHED),
	                     		(PVOID)start,
	                     //		(aDestMDL->ByteCount + aDestMDL->ByteOffset ) -1
	                     		limit
	                     );

	if(checkNvAllocArchError(error )) {
		DPF("NVDD: Cannot re-allocate MTM TO context object ");
		MySurfaces.bContextCreated = FALSE;
	} else {
		// set context on MTM to real object

#ifdef NV3_HW
		pMyNvChan->subchannel[5].control.object = MY_MTM_OBJECT;
#else 
		pMyNvChan->subchannel[5].SetObject  = MY_MTM_OBJECT;
#endif // NV3_HW

#ifdef NV3_HW
	pMyNvChan->subchannel[5].memoryToMemoryFormat.SetBufferOutCtxDma = MY_MTM_TO_CONTEXT;
#else
	pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaBufferOut = MY_MTM_TO_CONTEXT;
#endif // NV3_HW

		MySurfaces.bContextCreated = TRUE;
		
		//   DO A SOFTWARE METHOD FLUSH HERE
		NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);	

	}

}

U032 UpdateStatus(void) {
	// check the notifiers and see if thing completed ok
	// then fill in the status field and be on your way
#ifdef NV3_HW
	if( nvMyVPNotifiers[NUM039OFFSET + 1].status == NV_STATUS_IN_PROGRESS ) {
#else
	if( nvMyVPNotifiers[NUM039OFFSET + NV039_NOTIFIERS_BUFFER_NOTIFY].status == NV039_NOTIFICATION_STATUS_IN_PROGRESS ) {
#endif
		// still in progress
		return FALSE;
	} else {
#ifdef NV3_HW
		if( nvMyVPNotifiers[NUM039OFFSET + 1].status == NV_STATUS_DONE_OK ) {
#else
		if( nvMyVPNotifiers[NUM039OFFSET + NV039_NOTIFIERS_BUFFER_NOTIFY].status == NV039_NOTIFICATION_STATUS_DONE_SUCCESS ) {
#endif
			// finished ok!
			if( MySurfaces.pCurrentTransfer->transferStatus == PROGRAMMED) { 
				U032 temp = ((LPDDSURFACEDATA)(MySurfaces.pCurrentTransfer->SurfaceData))->fpLockPtr;
				U032 surfIndex = getSurfaceIndex(temp);
				MySurfaces.pCurrentTransfer->transferStatus = FINISHED;
				if( surfIndex < MySurfaces.VBIstart ) 
					MarkTiming(NOTIFYTRAN,temp, makeInd(INCNOTAP,INCNOTAP,INCNOTAP,surfIndex));
				else
					MarkTiming(NOTIFYTRAN,temp, makeInd(INCNOTAP,surfIndex,INCNOTAP,INCNOTAP));
			}
			// Do the callback to MS here
			if(MySurfaces.pDXIRQCallback != 0) {
				//if( MySurfaces.DXIRQCallbackSources & DDIRQ_BUSMASTER ) {
				// BUG in MS code... they should have set this flag but didn't for some reason...
				if(1) {			
					// CODE  -  this is where we'd do the callback
					
					// for now let's just get STUCK here
					// is it a void fun(void)   function?	
					
					// here's the one line of docs that we have:
					// When calling the IRQCallback, EAX should contain the DDIRQ_xxxx flags indicating 
					// what caused the IRQ and EBX should contain the specified dwContext. 
					
					
					U032 temp1 = MySurfaces.dwIrqContext;		//  the same context which dd gave me before
					U032 temp2 = DDIRQ_BUSMASTER;				// at this point we only have an interrupt for ONE event
					MySurfaces.dwMostRecentFinishTransferId = MySurfaces.pCurrentTransfer->TransferID;
				
	// to allow chris's editor to deal with funky assembly code
#define SKIPOMPARSE
#ifdef SKIPOMPARSE		
			
					// push register values
					_asm mov EAX, temp2
					_asm mov EBX, temp1
#endif							
					// call the callback  --- this will call vddGetTransferStatus......talk about TWISTED!!!!
					((ACALLBACKTYPEPROC)MySurfaces.pDXIRQCallback)();
				}
			};

			
		} else {
			// probably finished with an error!
			if( MySurfaces.pCurrentTransfer->transferStatus == PROGRAMMED) { 
				U032 temp = ((LPDDSURFACEDATA)(MySurfaces.pCurrentTransfer->SurfaceData))->fpLockPtr;
				U032 surfIndex = getSurfaceIndex(temp);
				MySurfaces.pCurrentTransfer->transferStatus = ERRORED;
				if( surfIndex < MySurfaces.VBIstart ) 
					MarkTiming(NOTIFYTRAN,temp, makeInd(INCNOTAP,INCNOTAP,INCNOTAP,surfIndex));
				else
					MarkTiming(NOTIFYTRAN,temp, makeInd(INCNOTAP,surfIndex,INCNOTAP,INCNOTAP));
							
			}
			// Do the callback to MS here
			if(MySurfaces.pDXIRQCallback != 0) {
				//if( MySurfaces.DXIRQCallbackSources & DDIRQ_BUSMASTER ) {
				// BUG in MS code... they should have set this flag but didn't for some reason...
				if(1) {		
					// CODE  -  this is where we'd do the callback
					
					// for now let's just get STUCK here
					// is it a void fun(void)   function?	
					
					// here's the one line of docs that we have:
					// When calling the IRQCallback, EAX should contain the DDIRQ_xxxx flags indicating 
					// what caused the IRQ and EBX should contain the specified dwContext. 
					
					
					U032 temp1 = MySurfaces.dwIrqContext;		//  the same context which dd gave me before
					U032 temp2 = DDIRQ_BUSMASTER;				// at this point we only have an interrupt for ONE event
					MySurfaces.dwMostRecentFinishTransferId = MySurfaces.pCurrentTransfer->TransferID;
				
	// to allow chris's editor to deal with funky assembly code
#define SKIPOMPARSE
#ifdef SKIPOMPARSE		
			
					// push register values
					_asm mov EAX, temp2
					_asm mov EBX, temp1
#endif							
					// call the callback
					((ACALLBACKTYPEPROC)MySurfaces.pDXIRQCallback)();
				}
			};
		}
	
		return TRUE;
	}
	
}

U032 CancelTransfer(DDTRANSFERININFO* pTransferInInfo) {
	pTransfer pATransfer;
	
	pATransfer = FindTransfer(pTransferInInfo->dwTransferID);
	
	if( pATransfer == NULL ) {
		return 1;
	}
	
	if( pATransfer->transferStatus == QUEUED ) {
		
		DeleteTransfer(pATransfer);
		return 0;
	} 
	return 1;
	
}



void CheckForLaunch(void) {
	NvChannel*	pMyNvChan = MySurfaces.pVPChanPtr;
	pTransfer pTheTransfer;
	U032 offsetStartByte;
	U032 lineCount;
	U032 lineLength;
	long linePitch;
	LPDDSURFACEDATA pSurfaceData;
	U032 bVbiSurf = FALSE;
	
	// Determine if the MTM engine is free, and attempt to launch a transfer
	
	
	// check for free engine
#ifdef NV3_HW
	if( nvMyVPNotifiers[NUM039OFFSET + 1].status == NV_STATUS_IN_PROGRESS ) {
#else
	if( nvMyVPNotifiers[NUM039OFFSET + NV039_NOTIFIERS_BUFFER_NOTIFY].status == NV039_NOTIFICATION_STATUS_IN_PROGRESS ) {
#endif

		// no free engine to start transfer on...
		
		return;
	} else {
		//  program up the MTM
		pTheTransfer = GetNextTransfer();
		if(pTheTransfer != NULL ) {
			U032 surfIndex;
			pSurfaceData = ((LPDDSURFACEDATA)(pTheTransfer->SurfaceData));
			
			surfIndex = getSurfaceIndex(pSurfaceData->fpLockPtr);
			if( surfIndex < MySurfaces.VBIstart ) 
				MarkTiming(TRANSFERLAUNCH,(U032)pSurfaceData->fpLockPtr, makeInd(INCNOTAP,INCNOTAP,INCNOTAP,surfIndex));
			else
				MarkTiming(TRANSFERLAUNCH,(U032)pSurfaceData->fpLockPtr, makeInd(INCNOTAP,surfIndex,INCNOTAP,INCNOTAP));
						
			ReCreateContext(pTheTransfer->DestMDL);
			
			if(surfIndex < MySurfaces.VBIstart ) 
				MarkTiming(TRANSFERRECRE,(U032)pSurfaceData->fpLockPtr, makeInd(INCNOTAP,INCNOTAP,INCNOTAP,surfIndex));
			else if(surfIndex < MySurfaces.numberOfSurfaces ) 
				MarkTiming(TRANSFERRECRE,(U032)pSurfaceData->fpLockPtr, makeInd(INCNOTAP,INCNOTAP,INCNOTAP,surfIndex));
			else
				MarkTiming(TRANSFERRECRE,(U032)pSurfaceData->fpLockPtr, makeInd(INCNOTAP,surfIndex,INCNOTAP,INCNOTAP));
			
#ifdef NV3_HW
			nvMyVPNotifiers[NUM039OFFSET + 1].status = NV_STATUS_IN_PROGRESS;
#else
			nvMyVPNotifiers[NUM039OFFSET + NV039_NOTIFIERS_BUFFER_NOTIFY].status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;
#endif
			
			
			MYCHECKFREECOUNT(pMyNvChan,9*4);
	
					
#ifdef NV3_HW
			pMyNvChan->subchannel[5].control.object = MY_MTM_OBJECT;
#else 
			pMyNvChan->subchannel[5].SetObject = MY_MTM_OBJECT;
#endif // NV3_HW
			
			
			if (pSurfaceData->dwFormatFourCC == mmioFOURCC('R','A','W','8') ) {
				bVbiSurf = TRUE;
			} else if (pSurfaceData->dwFormatFourCC == mmioFOURCC('U','Y','V','Y') ) {
				bVbiSurf = FALSE;
			} else {
				bVbiSurf = FALSE;
			}
			
//			offsetStartByte = pSurfaceData->dwSurfaceOffset
//					+ pSurfaceData->lPitch * pTheTransfer->StartLine;
			// assume 1 based numbering for now...
			if(!bVbiSurf) {
				offsetStartByte =  ( pSurfaceData->fpLockPtr - (U032)((GLOBALDATA*)(MySurfaces.pDriverData))->BaseAddress)
						+ pSurfaceData->lPitch * (pTheTransfer->StartLine );
			} else {
				offsetStartByte =  ( pSurfaceData->fpLockPtr - (U032)((GLOBALDATA*)(MySurfaces.pDriverData))->BaseAddress)
						+ pSurfaceData->lPitch * (pTheTransfer->StartLine - 1);
			}

			//lineLength = pSurfaceData->dwWidth * 2;
			// apparently this width is already in bytes
			lineLength = pSurfaceData->dwWidth;
			linePitch = pSurfaceData->lPitch;
			
// WARNING THIS IS A HACK HACK HACK...			

// after developing this emprically it looks like it's an anti-interleaved factor... 
// we should probably check to make sure that the surface is interleaved before we impose it!
			
			if( (surfIndex < MySurfaces.VBIstart ) && (MySurfaces.dwVPFlags & DDVP_INTERLEAVE ) ) {
				

			
				if( lineLength < 90 ) {
					// 80X60
					linePitch = linePitch *2;
				} else if( lineLength < 170 ) {
					// 160X120
					linePitch = linePitch *2 ;
				} else if( lineLength < 330 ) {
					// 320X240
					linePitch = linePitch * 2;
				} else if( lineLength < 650 ) {
					// 640X480
					linePitch = linePitch ;
				} else {
					// this is some weird 720 things... do the same as 640 for now
					// 720X480
					linePitch = linePitch ;
				} 
			
			}			
			lineCount = ( pTheTransfer->EndLine - pTheTransfer->StartLine + 1 );	
				// SMAC@microsoft.com  :  Yes, you should transfer the end line.  
				// Therefore, if we only wanted you to
				// transfer a single line, start line would equal end line.	
			if( pTheTransfer->TransferFlags & DDTRANSFER_INVERT ) {
				// if we want to invert things, then we should 
				// negate the pitch and put the start offset at the end.
				
				offsetStartByte = offsetStartByte + linePitch * (lineCount - 1 );
				linePitch = -linePitch;
			}
			if(!bVbiSurf) {
				if((pTheTransfer->DestMDL->ByteCount  )  < (lineLength * 2 * lineCount )  ) { 
					DPF("Destination memory is not large enough to hold this image, truncating line count");
				//	lineCount = (pTheTransfer->DestMDL->ByteCount + pTheTransfer->DestMDL->ByteOffset ) / 	lineLength; 
					lineCount = (pTheTransfer->DestMDL->ByteCount ) / 	(lineLength * 2); 
				}
			} else {			
				if((pTheTransfer->DestMDL->ByteCount  )  < (lineLength * lineCount )  ) { 
					DPF("Destination memory is not large enough to hold this image, truncating line count");
				//	lineCount = (pTheTransfer->DestMDL->ByteCount + pTheTransfer->DestMDL->ByteOffset ) / 	lineLength; 
					lineCount = (pTheTransfer->DestMDL->ByteCount ) / 	(lineLength ); 
				}
			}			
			
			ASSERT(8*4 < NV06A_FIFO_GUARANTEED_SIZE );			
			while (NvGetFreeCount(pMyNvChan, 5) < 8*4 )	{
				NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);	
			}					
			
			
#ifdef NV3_HW
			pMyNvChan->subchannel[5].memoryToMemoryFormat.BufferTransfer.offsetin	= offsetStartByte;
			//pMyNvChan->subchannel[5].memoryToMemoryFormat.BufferTransfer.offsetout = (unsigned long)pTheTransfer->DestMDL->ByteOffset;
			pMyNvChan->subchannel[5].memoryToMemoryFormat.BufferTransfer.offsetout = 0;
			pMyNvChan->subchannel[5].memoryToMemoryFormat.BufferTransfer.pitchin	= linePitch;
			pMyNvChan->subchannel[5].memoryToMemoryFormat.BufferTransfer.pitchout	= lineLength * 2;
			pMyNvChan->subchannel[5].memoryToMemoryFormat.BufferTransfer.linelengthin = lineLength *2;
			pMyNvChan->subchannel[5].memoryToMemoryFormat.BufferTransfer.linecount = lineCount;
			pMyNvChan->subchannel[5].memoryToMemoryFormat.BufferTransfer.format	=  ( 1 | 1 << 8 );
			pMyNvChan->subchannel[5].memoryToMemoryFormat.BufferTransfer.notify	= NV_OS_WRITE_THEN_AWAKEN;
#else 


			pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.OffsetIn	= offsetStartByte;
			//pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.OffsetOut = (unsigned long)pTheTransfer->DestMDL->ByteOffset;
			pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.OffsetOut = 0;
			pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.PitchIn	= linePitch;	// use the pitch in so that we can do interleaved
			if(!bVbiSurf) {
				pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.PitchOut	= lineLength * 2;  // use line lenght on pitch out so that it squishes together
				pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.LineLengthIn = lineLength * 2;	// remember there are two bytes per pixel 
			} else {
				pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.PitchOut	= lineLength;  // use line lenght on pitch out so that it squishes together
				pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.LineLengthIn = lineLength;	// remember there are two bytes per pixel 
			} 
			pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.LineCount = lineCount;
			pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.Format	=  ( 1 | 1 << 8 );
			pMyNvChan->subchannel[5].nv03MemoryToMemoryFormat.BufferNotify	= NV039_NOTIFY_WRITE_THEN_AWAKEN_1;
#endif // NV3_HW
			
			pTheTransfer->transferStatus = PROGRAMMED;
		} else {
			// there are no more transfers in the queue. or something bad happened
		
		}
		return;
	}
}

pTransfer FindTransfer(U032 TransferID) {
	U032 i;
	for(i=0;i<MAXTRANSFERS;i++) {
		if(MySurfaces.MyTransfers[i].TransferID == TransferID ) {
			return &(MySurfaces.MyTransfers[i]);
		}
	}
	return NULL;
}

U032 RemoveTransfer(U032 TransferID) {
	U032 i;
	for(i=0;i<MAXTRANSFERS;i++) {
		if(MySurfaces.MyTransfers[i].TransferID == TransferID ) {
			DeleteTransfer(&MySurfaces.MyTransfers[i]);
			return 0;
		}
	}
	return -1;
}

