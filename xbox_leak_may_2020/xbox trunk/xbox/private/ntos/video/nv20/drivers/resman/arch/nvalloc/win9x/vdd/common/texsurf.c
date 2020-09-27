/***************************************************************************\
* Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: texsurf.c                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           11/16/98                                          *
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
// #include "nvPusher.h"
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
#include "nvrmarch.inc"

#include "surfaces.h"
#include "VidTex.h"

// unit specific includes
#define RESMANSIDE

#define USERING0LIBRARY
#ifndef  USERING0LIBRARY
//
//
//
#include "nvkrmapi.c"

#endif //USERING0LIBRARY

extern  vpSurfaces 			MySurfaces;		
extern  NvNotification		nvMyVPNotifiers[NUMVPNOTIFIERS];


U032 vddSetVidTexSuf(U032 pInputPtr, U032 pOutputPtr ) {
	
	VidTexSurf* daPtr = (VidTexSurf*)pInputPtr;
	if(daPtr != NULL )
		//if(daPtr->frameBufferOffsetConv <  32*1024*1024 ) 
			if(daPtr->frameBufferOffsetSwiz <  32*1024*1024 ) 
			  if((daPtr->width <= 2048) && (daPtr->width > 0));
				if((daPtr->height <= 2048) && (daPtr->height > 0));
					if(daPtr->pitch <= 2048 )
						if(daPtr->format    ) {
							daPtr->dwFrameCount = 0;
							MySurfaces.pVidTexSurf = daPtr;
							(MySurfaces.pVidTexSurf)->bUpdateSystem = TRUE;
							return 0;
						}
	return 1;
}

void fillVidTexSurface(DWORD surface ) {
	Nv04ChannelPio* pMyNvChan = (Nv04ChannelPio*)MySurfaces.pVPChanPtr; 
	if(MySurfaces.pVidTexSurf == NULL ) {
		return;
	}
	// the pointer has been created.... start filling

	while (NvGetFreeCount(MySurfaces.pVPChanPtr, 4) < (17)*4)	 {
		NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);	
	};
	// Allocate the intermediate buffer context DMA
	pMyNvChan->subchannel[4].nv04ContextSurfaceSwizzled.SetContextDmaImage = MY_IMAGE0_BUFFER_CONTEXT;
	pMyNvChan->subchannel[4].nv04ContextSurfaceSwizzled.SetFormat = (MySurfaces.pVidTexSurf)->format;
	pMyNvChan->subchannel[4].nv04ContextSurfaceSwizzled.SetOffset = (MySurfaces.pVidTexSurf)->frameBufferOffsetSwiz;
	
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.SetContextDmaImage = MY_IMAGE0_BUFFER_CONTEXT;
	
	
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.SetContextDmaImage = MY_IMAGE0_BUFFER_CONTEXT;
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.SetColorFormat = NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.ClipPoint = 0;
	
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.ClipPoint = 0;
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.ClipSize = 0xFFFFFFFF;		//mega big clip
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.ImageOutPoint = 0;
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.ImageOutSize = ((MySurfaces.pVidTexSurf)->height << 16) ||
																							(MySurfaces.pVidTexSurf)->width ;;
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.DeltaDuDx = ((MySurfaces.Surfaces[surface]->dwWidth ) << 20 )/((MySurfaces.pVidTexSurf)->width);
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.DeltaDvDy = ((MySurfaces.Surfaces[surface]->dwHeight ) << 20 )/((MySurfaces.pVidTexSurf)->height);
	
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.ImageInSize = ((MySurfaces.Surfaces[surface]->dwHeight ) < 16 )  ||
																							((MySurfaces.Surfaces[surface]->dwWidth )   );
		
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.ImageInFormat = (MySurfaces.Surfaces[surface]->dwWidth && 0x0FFFF) ||
																							( 1 << 24 );
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.ImageInOffset = MySurfaces.Surfaces[surface]->dwOffset;
	pMyNvChan->subchannel[3].nv04ScaledImageFromMemory.ImageInPoint = 0;

}

