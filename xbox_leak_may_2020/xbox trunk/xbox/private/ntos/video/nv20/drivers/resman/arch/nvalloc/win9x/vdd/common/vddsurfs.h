/***************************************************************************\
* (C) Copyright NVIDIA Corporation Inc.,                                    *
*  1996,1997, 1998. All rights reserved.                                    *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: vddsurfs.h                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           8/21/98                                          *
*                                                                           *
\***************************************************************************/

#ifndef VDDSURFS_H
#define VDDSURFS_H

#define  MYCHECKFREECOUNT(a,b)								\
	ASSERT(b < NV06A_FIFO_GUARANTEED_SIZE );			\
	while (MySurfaces.myFreeCount < b)								\
		MySurfaces.myFreeCount = NvGetFreeCount(a, 0);			\
	MySurfaces.myFreeCount -= b;										


typedef VOID (*ACALLBACKTYPEPROC)(void);


U032 vddhandleBufferInterrupts(U032 dummy1, U032 dummy2, U032 dummy3);

U032 startSurfaces(U032	bInterleave, VPSTATE*	pVideoPortStatus );

U032 	scheduleFlipToSurface(DDSURFACEDATA* pCurSurf,FIELDTYPE	fieldType);

void Overlay0NotifyProc(void);
void Overlay1NotifyProc(void);

void    reportError(U032 interruptingBuffer,U032 oldSurface);


#define NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_0 (0x04000000)
#define NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_1 (0x02000000)
#define NV_VFM_FORMAT_BOBFROMINTERLEAVE           (0x08000000)

#endif // VDDSURFS_H