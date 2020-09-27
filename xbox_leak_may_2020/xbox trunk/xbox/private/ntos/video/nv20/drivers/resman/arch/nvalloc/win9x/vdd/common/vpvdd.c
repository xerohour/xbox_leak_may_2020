/***************************************************************************\
* (C) Copyright NVIDIA Corporation Inc.,                                    *
*  1996,1997, 1998. All rights reserved.                                    *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: vpvdd.c                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           4/22/98                                          *
*                                                                           *
\***************************************************************************/

#ifdef NV4_HW
#define NV4
#endif
#ifdef NV10_HW
#define NV4
#endif

// MS includes
#include "windows.h"
#include "ddraw.h"
#include "Ddrawp.h"
#include "ddrawi.h"
#include "nvPusher.h"
#include "ddmini.h"
#include "dvp.h"
#include "dmemmgr.h"
//  vdd specific MS includes
#include "minivdd.h"
#include "ddkmmini.h"


// NV includes
#include "nvwin32.h"
#include "nvhw.h"
#include <nv_ref.h>
// unit specific includes
#define RESMANSIDE
#include "surfaces.h"
#include "vddsurfs.h"
#include "vpvdd.h"
#include "vddsurfs.h"
#include "transfer.h"
#include "nvrmr0api.h"


extern vpSurfaces 			MySurfaces;
extern NvNotification		nvMyVPNotifiers[NUMVPNOTIFIERS];

U032 vddWriteVPEPrescale(U032 ulX, U032 ulY, U032 ulCap, U032 ulVPUseOverlay, U032 ulFlag);

U032	vddGetIRQInfo(U032 dwMiniVDDContext, U032 dummy, DDGETIRQINFO* pGetIrgInfo) {
/*
;   DESCRIPTION: If the Mini VDD is already managing the IRQ, this
;          function returns that information; otherwise, it returns the
;          IRQ number assigned to the device so DDraw can manage the IRQ.
;
;          The returning the IRQ number, it is important that it get the
;          value assigned by the Config Manager rather than simply get
;          the value from the hardware (since it can be remapped by PCI).
*/
	// Not gonna do it.... 
	pGetIrgInfo->dwFlags =IRQINFO_HANDLED;
	
	return 0;
}

U032	vddIsOurIRQ(U032 dwMiniVDDContext, U032 dummy) {
/*
;   DESCRIPTION: Called when the VDD's IRQ handled is triggered.  This
;          determines if the IRQ was caused by our VGA and if so, it
;          clears the IRQ and returns which event(s) generated the IRQ.
*/

	return 1;
}

U032	vddEnableIRQ(U032 dwMiniVDDContext, DDENABLEIRQINFO* pEnableIrqInfo, U032 dummy) {
/*
;   DESCRIPTION: Notifies the Mini VDD which IRQs should be enabled.  If
;          a previously enabled IRQ is not specified in this call,
;          it should be disabled.
*/


  //  Here's where we're going to call NvRmR0interrupt()  
  // the purpose is so that the APIEntry variable will get filled
  // now instead of in the future where we're possibly inside an ISR
  #define NV_WIN_DEVICE                            0x00002002
  if(MySurfaces.pDriverData != 0 ) {
    if(MySurfaces.pDriverData->dwRootHandle != 0 ) { 
      NvRmR0Interrupt (MySurfaces.pDriverData->dwRootHandle, NV_WIN_DEVICE);   
    }
  }                         
  
	// just remember these guys, and check them AFTER we've done the real flips in vddsurfs
#define KMVTON
#ifdef KMVTON
	MySurfaces.DXIRQCallbackSources = pEnableIrqInfo->dwIRQSources;
	MySurfaces.pDXIRQCallback = pEnableIrqInfo->IRQCallback;
	MySurfaces.dwIrqContext = pEnableIrqInfo->dwContext;
	return 0;
#else
	return 1;
#endif

}

U032	vddSkipNextField(U032 dwMiniVDDContext, DDSKIPINFO* pSkipInfo, U032 dummy) {
/*
;   DESCRIPTION: Called when they want to skip the next field, usually
;       to undo a 3:2 pulldown but also for decreasing the frame rate.
;       The driver should not lose the VBI lines if dwVBIHeight contains
;       a valid value.
;
*/
	// heh this one is easy!
	if(pSkipInfo->dwSkipFlags & DDSKIP_SKIPNEXT )
		MySurfaces.bSkipNextField = TRUE;
	else if(pSkipInfo->dwSkipFlags & DDSKIP_ENABLENEXT )
		MySurfaces.bSkipNextField = FALSE;
	else 
		return 1;
	return 0;
}


U032	vddBobNextField(U032 dwMiniVDDContext, DDBOBINFO* pBobInfo, U032 dummy) {
/*
;   DESCRIPTION: Called when "bob" is used and a VPORT VSYNC occurs that does
;       not cause a flip to occur (e.g. bobbing while interleaved).  When
;       bobbing, the overlay must adjust itself on every VSYNC, so this
;       function notifies it of the VSYNCs that it doesn't already know
;       about (e.g. VSYNCs that trigger a flip to occur).
*/

	// this is used to change to the next bob in bob from interleaved...
	// however only really useful during software autoflipping.
	
	// we do hardware, so this does not have utility.

	return 1;
}

U032	vddSetState(U032 dwMiniVDDContext, DDSTATEININFO* pSetStateInfo, DDSTATEOUTINFO* pSetStateInfoOut) {
/*
;   DESCRIPTION: Called when the client wants to switch from bob to weave.
;	The overlay flags indicate which state to use. Only called for interleaved
;   surfaces.
;
;	NOTE: When this is called, the specified surface may not be
;	displaying the overlay (due to a flip).  Instead of failing
;	the call, change the bob/weave state for the overlay that would
;	be used if the overlay was flipped again to the specified surface.
*/
	U032	dwSrcHeight;
	U032	dwDstHeight;
	U032	dwDstWidth;
	U032	dwSrcWidth;
	U032	showInterleavedFields = 0;
	long	srcDeltaX;
	long	srcDeltaY;
	U032 	index;
	U032	dwFormat;
	
	
	DDSURFACEDATA* pCurSurf = (DDSURFACEDATA*) pSetStateInfo->lpSurfaceData;
	//pSetStateInfo->lpVideoPortData;

	pCurSurf->dwOverlayFlags;			// may change
	pCurSurf->dwOverlayOffset;				// what the hell is this?  X,Y  or offset from base, or it's it Precalced from X,Y
													// golly gee I love these docs!
	pCurSurf->lPitch;

	dwSrcHeight = pCurSurf->dwOverlaySrcHeight;
	dwDstHeight = pCurSurf->dwOverlayDestHeight;
	dwDstWidth = pCurSurf->dwOverlayDestWidth;
	dwSrcWidth = pCurSurf->dwOverlaySrcWidth;

	if (!(pCurSurf->dwOverlayFlags & DDOVER_AUTOFLIP)) {
		// we only work in auto flip mode so this is bad!
		
		return 0;
	}
	
	(MySurfaces.pDriverData)->dwOverlaySrcHeight = dwSrcHeight;
	(MySurfaces.pDriverData)->dwOverlayDstHeight = dwDstHeight;
	
	if (dwSrcHeight > dwDstHeight) {
		dwSrcHeight >>= 1; /* We only support vertical shrinking and it can't be by more than .5 */
		if (dwSrcHeight > dwDstHeight) {
 				return DDERR_UNSUPPORTED;
		}    
	}

	
	if ((pCurSurf->dwOverlayFlags & DDOVER_INTERLEAVED) &&
		(pCurSurf->dwOverlayFlags & DDOVER_BOB)) {
 				showInterleavedFields = 1;
 				dwSrcHeight >>= 1;
	}
	
	dwDstWidth = (dwDstWidth + 2) & ~1L;
	dwDstHeight = (dwDstHeight + 2) & ~1L;
	
	srcDeltaX = (dwSrcWidth - 1) << 16;
	srcDeltaX /= dwDstWidth - 1;
	srcDeltaX <<= 4;
	
	srcDeltaY = (dwSrcHeight - 1) << 16;
	srcDeltaY /= dwDstHeight - 1;
	srcDeltaY <<= 4;

	index = 0;

#define NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_0 (0x04000000)
#define NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_1 (0x02000000)
#define NV_VFM_FORMAT_BOBFROMINTERLEAVE           (0x08000000)
	
	dwFormat = NV_VFM_FORMAT_COLOR_LE_Y8V8Y8U8;

	if((pCurSurf->dwOverlayFlags & DDOVER_BOB) && (pCurSurf->dwOverlayFlags & DDOVER_INTERLEAVED) ){
		dwFormat = dwFormat | NV_VFM_FORMAT_BOBFROMINTERLEAVE;
	}

	if (pCurSurf->dwOverlayFlags & DDOVER_AUTOFLIP) {
		dwFormat |= (index ? NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_1 : 
			NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_0);
	}

	(MySurfaces.pDriverData)->OverlayFormat = dwFormat;

	// output
	/*
	typedef struct _DDSTATEOUTINFO {
    DWORD dwSize;
    DWORD dwSoftwareAutoflip;
    DWORD dwSurfaceIndex;
    DWORD dwVBISurfaceIndex;
	} DDSTATEOUTINFO;
	*/

	pSetStateInfoOut->dwSoftwareAutoflip = 0;
	pSetStateInfoOut->dwSurfaceIndex = 0;
	pSetStateInfoOut->dwVBISurfaceIndex = 0;

	// this is called to switch from bob to weave or back again
	return 0;
}

U032	vddLock(U032 dwMiniVDDContext, DDLOCKININFO* pLockInfoIn, DDLOCKOUTINFO* pLockInfoOut) {
/*
;   DESCRIPTION: Called when the client wants to lock the surface to
;	access the frame buffer. The driver doens't have to do anything,
;       but it can if it needs to.
*/

	// we don't need to do anything here because of the way we work
	return 0;
}

U032	vddFlipOverlay(U032 dwMiniVDDContext, DDFLIPOVERLAYINFO* pFlipOverlayInfo, U032 dummy) {
/*
;   DESCRIPTION: Flips the overlay to the target surface.
*/	
	vpSurfaces*	pTheSurfaces = (vpSurfaces*) dwMiniVDDContext;
	DDSURFACEDATA* pCurSurf =	(DDSURFACEDATA*) pFlipOverlayInfo->lpCurrentSurface;
	DDSURFACEDATA* pTargSurf =	(DDSURFACEDATA*) pFlipOverlayInfo->lpTargetSurface;
	DWORD daFlags = pFlipOverlayInfo->dwFlags;
	FIELDTYPE	fieldType = UNKNOWN;
/* 
	YIKES   Ok, until I can break throught the mind lock of BMAC and be able to modify some of his code to acheieve
	      a sharing of the overlay class object, I'm just going to hacks something together here which will hit registers
			to get this done.   First part will attempt to get 
			
			We might be able to use the infamous "reserved" fields to get things down into here
*/
	//  We do Autoflip pretty much exclusively!
	// but jonka says that dell's gonna want this for their cheesier bt 848 card.  so oh well.
	
	
	// we need to figure out which index the pCurSurf is, and then pass this in
	// also we need to determine if we're doing bob from Interleave
	//U032 	scheduleOverlayFlip(U032 oldSurface,  U032 bIsBobFromInterleave) {
	
	if(daFlags & DDFLIP_EVEN  ) fieldType = EVEN;
	else if(daFlags & DDFLIP_ODD  ) fieldType = ODD;
	
	return scheduleFlipToSurface( pTargSurf, fieldType);

}

U032	vddFlipVideoPort(U032 dwMiniVDDContext, U032* pFlipVideoPort, U032 dummy) {
/*
;   DESCRIPTION: Flips the video port to the target surface.
*/
	//  We do Autoflip pretty much exclusively!
	return 1;
}

U032	vddGetFieldPolarity(U032 dwMiniVDDContext, DDPOLARITYININFO* pGetPolarityInInfo, DDPOLARITYOUTINFO* pGetPolarityOutInfo) {
	U032	returnval;
	
/*
;
;   DESCRIPTION: Returns the polarity of the current field being written
;	to the specified video port.
*/	

//	DPF(" vvv  vddGetFieldPolarity ");
	
	
	
	// we think that EVEN should be true...
	if( GetField() ) {
		pGetPolarityOutInfo->bPolarity = TRUE;
		returnval = 0;
	} else {
		pGetPolarityOutInfo->bPolarity = FALSE;
		returnval = 0;
	}
	MarkTiming (CHECKCURFLD,0, makeInd(INCNOTAP,pGetPolarityOutInfo->bPolarity,INCNOTAP,MySurfaces.curImageSurf));

	return returnval;
}


U032	vddSetSkipPattern(U032 dwMiniVDDContext, DDSETSKIPINFO* pSetSkipInfo, U032 dummy) {
/*
;
;   DESCRIPTION: Sets the skip pattern in hardware
;
*/
	return 1;
}

U032	vddGetCurrentAutoFlip(U032 dwMiniVDDContext, DDGETAUTOFLIPININFO* pGetAutoFlipInfoIn, DDGETAUTOFLIPOUTINFO* pGetAutoFlipInfoOut) {
/*
;   DESCRIPTION: Returns the current surface receiving data from the
;	video port while autoflipping is taking palce.  Only called when
;   hardware autoflipping.
;
*/
#if 0
	pGetAutoFlipInfoOut->dwSurfaceIndex = MySurfaces.curImageSurf;
	pGetAutoFlipInfoOut->dwVBISurfaceIndex = MySurfaces.curVBISurf  - MySurfaces.VBIstart;
#endif //0

	// first count the number of buffers that we have "in progress"
	// there should always be at least one in progress, and possibly two.
	// if there are zero in progress, then we're not capturing at this moment
	// so it's ok to return a faulty value
	long curIMG;
	long curVBI;
	long countVBI = -1;
	long countIMG = -1;

#ifndef NV4
	if( nvMyVPNotifiers[notifyIndexFromBuffer(0)].status == NV_STATUS_IN_PROGRESS )
		countIMG++;
	if( nvMyVPNotifiers[notifyIndexFromBuffer(1)].status == NV_STATUS_IN_PROGRESS )
		countIMG++;
	if( nvMyVPNotifiers[notifyIndexFromBuffer(2)].status == NV_STATUS_IN_PROGRESS )
		countVBI++;
	if( nvMyVPNotifiers[notifyIndexFromBuffer(3)].status == NV_STATUS_IN_PROGRESS )
		countVBI++;
#else
	if( nvMyVPNotifiers[notifyIndexFromBuffer(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS )
		countIMG++;
	if( nvMyVPNotifiers[notifyIndexFromBuffer(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS )
		countIMG++;
	if( nvMyVPNotifiers[notifyIndexFromBuffer(2)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS )
		countVBI++;
	if( nvMyVPNotifiers[notifyIndexFromBuffer(3)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS )
		countVBI++;
#endif

	// now subtract this from 	the official current count, but add one since it's not the "next" buffer but the "current"
	curIMG = MySurfaces.curImageSurf - countIMG;
	curVBI = MySurfaces.curVBISurf - MySurfaces.VBIstart - countVBI;
	// now check for reverse wraparound
	if( MySurfaces.VBIstart > 0 )
		while(curIMG < 0 )
			curIMG = curIMG + MySurfaces.VBIstart;
	if( (MySurfaces.numberOfSurfaces - MySurfaces.VBIstart) > 0 )
		while(curVBI < 0 )
			curVBI = curVBI + (MySurfaces.numberOfSurfaces - MySurfaces.VBIstart);
		
	pGetAutoFlipInfoOut->dwSurfaceIndex = curIMG;
	pGetAutoFlipInfoOut->dwVBISurfaceIndex = curVBI;

	MarkTiming (CHECKCURBUF,0, makeInd(INCNOTAP,curVBI,INCNOTAP,curIMG));

	
	return 0;
}

U032	vddGetPreviousAutoFlip(U032 dwMiniVDDContext, DDGETAUTOFLIPININFO* pGetAutoFlipInfoIn, DDGETAUTOFLIPOUTINFO* pGetAutoFlipInfoOut) {
/*
;   DESCRIPTION: Returns the surface that received the data from the
;	previous field of video port while autoflipping is taking palce. Only
;   called for hardware autoflipping.
;
*/
	
	// This code is not quite working right....
	
	// first count the number of buffers that we have "in progress"
	// there should always be at least one in progress, and possibly two.
	// if there are zero in progress, then we're not capturing at this moment
	// so it's ok to return a faulty value
	long curIMG;
	long curVBI;
	long countVBI = -1;
	long countIMG = -1;
#ifndef NV4
	if( nvMyVPNotifiers[notifyIndexFromBuffer(0)].status == NV_STATUS_IN_PROGRESS )
		countIMG++;
	if( nvMyVPNotifiers[notifyIndexFromBuffer(1)].status == NV_STATUS_IN_PROGRESS )
		countIMG++;
	if( nvMyVPNotifiers[notifyIndexFromBuffer(2)].status == NV_STATUS_IN_PROGRESS )
		countVBI++;
	if( nvMyVPNotifiers[notifyIndexFromBuffer(3)].status == NV_STATUS_IN_PROGRESS )
		countVBI++;
#else
	if( nvMyVPNotifiers[notifyIndexFromBuffer(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS )
		countIMG++;
	if( nvMyVPNotifiers[notifyIndexFromBuffer(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS )
		countIMG++;
	if( nvMyVPNotifiers[notifyIndexFromBuffer(2)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS )
		countVBI++;
	if( nvMyVPNotifiers[notifyIndexFromBuffer(3)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS )
		countVBI++;
#endif
	// now subtract this from 	the official current count but subtract an additional since this is
	// the "previous" autoflip
	
	curIMG = MySurfaces.curImageSurf - countIMG -1;
	curVBI = MySurfaces.curImageSurf - MySurfaces.VBIstart - countVBI -1;
	// now check for reverse wraparound
	while(curIMG < 0 )
		curIMG = curIMG + MySurfaces.VBIstart;
	while(curVBI < 0 )
		curVBI = curVBI + (MySurfaces.numberOfSurfaces - MySurfaces.VBIstart);
		
	pGetAutoFlipInfoOut->dwSurfaceIndex = curIMG;
	pGetAutoFlipInfoOut->dwVBISurfaceIndex = curVBI;


	MarkTiming (CHECKLASTBUF,0, makeInd(INCNOTAP,curVBI,INCNOTAP,curIMG) );
	return 0;
}


		

U032	vddTransfer(U032 dwMiniVddContext, DDTRANSFERININFO* pTransferInInfo, DDTRANSFEROUTINFO* pTransferOutInfo ) {
	pTransfer	pNewTransfer;
	U032			surfIndex = 99;

	// we don't do AGP transfers
	if(DDTRANSFER_NONLOCALVIDMEM & pTransferInInfo->dwTransferFlags ) {
		return 1;
	}
	
	// we DO handle cancels for things not yet queued
	if(DDTRANSFER_CANCEL & pTransferInInfo->dwTransferFlags ) {
		return CancelTransfer(pTransferInInfo);
	}
	
	ASSERT(pTransferInInfo != NULL );
	ASSERT(pTransferInInfo->dwSize == sizeof(DDTRANSFERININFO));

	NULL;
	
	pNewTransfer = AddTransfer(pTransferInInfo->dwTransferID);
	if(pNewTransfer == NULL )
		return 1;
		
	NULL;
	

#ifndef DDTRANSFER_HALFLINES
#define DDTRANSFER_HALFLINES 0x00000100
#endif
	
	if(DDTRANSFER_HALFLINES & pTransferInInfo->dwTransferFlags ) {
		pNewTransfer->StartLine = pTransferInInfo->dwStartLine;
		pNewTransfer->EndLine = pTransferInInfo->dwEndLine;
	} else {
		pNewTransfer->StartLine = pTransferInInfo->dwStartLine+1;
		pNewTransfer->EndLine = pTransferInInfo->dwEndLine+1;
	}
	
	pNewTransfer->SurfaceData = pTransferInInfo->lpSurfaceData;
	pNewTransfer->TransferFlags = pTransferInInfo->dwTransferFlags;
	pNewTransfer->DestMDL = pTransferInInfo->lpDestMDL;
	pNewTransfer->transferStatus = QUEUED;
			
	CheckForLaunch();
	{
		surfIndex = getSurfaceIndex(((DDSURFACEDATA*) pTransferInInfo->lpSurfaceData)->fpLockPtr);
		
		if(surfIndex != 99 ) {
			if( MySurfaces.Surfaces[surfIndex]->field == EVEN ) {
				pTransferOutInfo->dwBufferPolarity = TRUE;
			} else {
				pTransferOutInfo->dwBufferPolarity = FALSE;
			}
		} else {
			// can't tell what surface we were on... just report one
			pTransferOutInfo->dwBufferPolarity = TRUE;
		}
		
		// Hack.... for whatever reason, then VBI surface polarities appear to be reversed... hack the fix in
		if (surfIndex < MySurfaces.VBIstart) {
			// do nothing... 'tho these may be reversed as well
		} else {
			if( pTransferOutInfo->dwBufferPolarity )
				pTransferOutInfo->dwBufferPolarity = FALSE;
			else 
				pTransferOutInfo->dwBufferPolarity = TRUE;
		}
		
	}
	//pTransferOutInfo->dwBufferPolarity = 0;
	if (surfIndex < MySurfaces.VBIstart) {
		MarkTiming(TRANSBUFTYPE,(U032)pTransferInInfo->lpDestMDL->lpMappedSystemVa,makeInd2(INCNOTAP,INCNOTAP, INCNOTAP,surfIndex,MySurfaces.Surfaces[surfIndex]->field) );
	} else if(surfIndex < MySurfaces.numberOfSurfaces){
		MarkTiming(TRANSBUFTYPE,(U032)pTransferInInfo->lpDestMDL->lpMappedSystemVa,makeInd2(INCNOTAP,surfIndex,INCNOTAP,INCNOTAP, MySurfaces.Surfaces[surfIndex]->field,) );
	} else {
		MarkTiming(TRANSBUFTYPE,(U032)pTransferInInfo->lpDestMDL->lpMappedSystemVa,makeInd2(INCNOTAP,surfIndex,INCNOTAP,INCNOTAP,UNKNOWN) );
	}
	
	return 0;
}

U032 vddGetTransferStatus(U032 dwMiniVddContext, U032 dummy, DDGETTRANSFERSTATUSOUTINFO* pGetTransStatusInfo ) {
	U032 retValue;
	// return 0 means everything ok  1 means not good.

#if 0
// Input to vddGetTransferStatus
typedef struct _DDGETTRANSFERSTATUSOUTINFO {
    DWORD dwSize;
    DWORD dwTransferID;
} DDGETTRANSFERSTATUSOUTINFO;

	
#endif
	pTransfer pTheTransfer;
	pTheTransfer = FindTransfer(MySurfaces.dwMostRecentFinishTransferId);
	retValue = 1;
	if(pTheTransfer != NULL ) {
		switch (pTheTransfer->transferStatus) {
				case QUEUED:	
					pGetTransStatusInfo->dwTransferID = MySurfaces.dwMostRecentFinishTransferId;			
					retValue = 1;
				case PROGRAMMED:				
					pGetTransStatusInfo->dwTransferID = MySurfaces.dwMostRecentFinishTransferId;			
					retValue = 1;
				case FINISHED:	
					pGetTransStatusInfo->dwTransferID = MySurfaces.dwMostRecentFinishTransferId;			
					retValue = 0;
					
		}
		RemoveTransfer(MySurfaces.dwMostRecentFinishTransferId);
		MySurfaces.dwMostRecentFinishTransferId = TRANSFERNOTINUSE;
	} else {
		retValue = 1;
	}
	
	// this should error as well
	return retValue;
} 


/*
 * nvVPECommand() is a backdoor function to allow other Ring0 programs (like WDM)
 * to change or request various VPE functions.
 */

#define NV_VPE_CMD_SET_PRESCALE_MODE    1
#define NV_VPE_CMD_SET_CROP_MODE        2

ULONG nvVPECommand (
    ULONG cmd,
    PVOID pData )
{

    switch ( cmd ) {

    case NV_VPE_CMD_SET_PRESCALE_MODE:

        /*
         * This command is to allow WDM drivers to tell VPE to prescale
         * as oppose to cropping.  This is because the overlay mixer does
         * not seem to pass on this information to us.
         */

        {
            PI2CControl	pI2CCtrl = pData;

            /*
             * set prescale flag and size
             */
			//Data(lower nibble)	=	0	not capturing
			//							1	capturing
			//Data(upper nibble)	=	2	X scale
			//							4	Y scale
			//Status	=	prescale/height * 1000
			if((pI2CCtrl->Data)& 0x40){
				vddWriteVPEPrescale(0, 
									((PI2CControl)pI2CCtrl)->Status,//set Y-prescale 
									((PI2CControl)pI2CCtrl)->Data,	//capture flag
									0, 
									6);								//prog. y-pre & cap. flag
			}else if((pI2CCtrl->Data)& 0x20){
				vddWriteVPEPrescale(((PI2CControl)pI2CCtrl)->Status,//set X-prescale
									0, 
									((PI2CControl)pI2CCtrl)->Data,	//capture flag
									0, 
									0xa);							//prog. x-pre & cap.flag
			}else{
				//error
			}
        }
        break;

    case NV_VPE_CMD_SET_CROP_MODE:

        /*
         * This command is to allow WDM drivers to tell VPE to crop
         * as oppose to prescaling.  This is because the overlay mixer does
         * not seem to pass on this information to us.
         */

        {
            PI2CControl	pI2CCtrl = pData;

            /*
             * turn off prescale flag
             */

        }
        break;

    default:
        return 1;   
    }

    return 0;
}

void vddVPEInit()
{
	//Initialize VPE capture hack for WDM
	vddWriteVPEPrescale(1000,	//setting ME X-prescale factor. 1000=no scale
						1000,	//setting ME Y-prescale factor. 1000=no scale
						0,		//set default VP surf type = overlay  
						1,		//set to not vpe capture mode
						0xf);	//use all values

}

