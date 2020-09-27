/***************************************************************************\
* Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: timing.c                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           9/23/98                                          *
*                                                                           *
\***************************************************************************/
#define RESMANSIDE
#define INVDDCODE

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
// #include "nvPusher.h"
#include "ddmini.h"
#include "dvp.h"
#include "dmemmgr.h"
//  vdd specific MS includes
#include "minivdd.h"
#include "ddkmmini.h"

#include "vmm.h"

// NV includes
#include "nvwin32.h"
#include "nvhw.h"
#include <nv_ref.h>
#include "nvrmr0api.h"

// unit specific includes
#include "surfaces.h"
#include "vddsurfs.h"
#include "vpvdd.h"

#include "timing.h"

#ifndef DPF
#define  DPF(a) _Debug_Printf_Service(a)
#endif

extern vpSurfaces 			MySurfaces;

void InitTiming() {

	
}

U032 CheckExclude(TIMINGTYPES timingType) {
  // when this function returns TRUE then this timing mark is NOT recorded...

  switch (timingType) {
		case TRANSFERLAUNCH :
		case NOTIFYTRAN :
//		case NOTIFYIMG :
//		case OVFLIPREQ :
//  	case OVFLIPNOT :
		case TRANSFERRECRE :
		case TRANSBUFTYPE :
//		case OVERLAYFAIL :
		case PROGRAMVBI :
//		case PROGRAMIMG :
		case NOTIFYVBI :
		case CHECKCURBUF :
		case CHECKLASTBUF :
		case CHECKCURFLD :
		case ENTERCOLORCTL :
		case LEAVECOLORCTL :
		case WAITOCCCOLORCTL :
		case WAITCHROMAARRIVE :
		case WAITCHROMARETURN :
		case WAITLUMAARIVE :
		case WAITLUMARETURN :
        return 1;
  		default:		
  			break;
  }
  

  return 0;
}

//#define NOLOOPTIMING

void MarkTimingNot(TIMINGTYPES timingType,NvNotification*  pNot, U032 pSurf, U032 Indices) {

	U032 Time0;
	U032 Time1;
	U032 count;
	
  if (CheckExclude(timingType)) return;

	Time0 =	pNot->timeStamp.nanoseconds[0];
	Time1 =	pNot->timeStamp.nanoseconds[1];
			
	count = MySurfaces.Timings.LoopCount;
	MySurfaces.Timings.Time0[count] = Time0;
	MySurfaces.Timings.Time1[count] = Time1;
	MySurfaces.Timings.SurfDat[count] = timingType;
	MySurfaces.Timings.SurfIndex[count] = Indices;
	MySurfaces.Timings.SurfPtr[count] = pSurf;
	
	count++;
	if(count >= MAXDATASTORE ) {
#ifdef NOLOOPTIMING
		count = MAXDATASTORE-1;
#else 
		count = 0;
#endif
	}

	MySurfaces.Timings.LoopCount = count;

	return;
}
			

void	MarkTiming(TIMINGTYPES timingType,U032 pSurf, U032 Indices) {
	U032 Time0;
	U032 Time1;
	U032 count;
	
	if (CheckExclude(timingType)) return;

	if(MySurfaces.pNvTimer != NULL) {
		Time0 = *(MySurfaces.pNvTimer);
		Time1 = *((U032*)(((U032)(MySurfaces.pNvTimer))+0x10));
	} else {
		Time0 = 0;
		Time1 = 0;
	}
	
	count = MySurfaces.Timings.LoopCount;
	MySurfaces.Timings.Time0[count] = Time0;
	MySurfaces.Timings.Time1[count] = Time1;
	MySurfaces.Timings.SurfDat[count] = timingType;
	MySurfaces.Timings.SurfIndex[count] = Indices;
	MySurfaces.Timings.SurfPtr[count] = pSurf;
	
	count++;
	if(count >= MAXDATASTORE ) {
#ifdef NOLOOPTIMING
		count = MAXDATASTORE-1;
#else 
		count = 0;
#endif
	}

	MySurfaces.Timings.LoopCount = count;

	return;
}

