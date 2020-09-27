/***************************************************************************\
* Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: timing.h                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           9/23/98                                          *
*                                                                           *
\***************************************************************************/

#ifndef timing_h
#define timing_h


#define	TIMINGTYPES			U032
#define  NOTIFYTRAN			(0x0 + 1 )

#define  NOTIFYIMG			(0x0 + 2 )
#define 	OVFLIPREQ			(0x0 + 3 )
#define 	OVFLIPNOT			(0x0 + 4 )
#define  TRANSFERRECRE		(0x0 + 5 )
#define  TRANSBUFTYPE      (0x0 + 6 ) 
#define  OVERLAYFAIL       (0x0 + 7 )
#define  PROGRAMVBI        (0x0 + 8 )
#define  PROGRAMIMG        (0x0 + 9 )
#define  NOTIFYVBI         (0x0 + 10 )
#define  CHECKCURBUF       (0x0 + 11 )
#define  CHECKLASTBUF      (0x0 + 12 )
#define  CHECKCURFLD       (0x0 + 13 )
// for the color control section
#define  ENTERCOLORCTL     (0x0 + 14 )
#define  LEAVECOLORCTL     (0x0 + 15 )
#define  WAITOCCCOLORCTL   (0x0 + 16 )
#define  WAITCHROMAARRIVE  (0x0 + 17 )
#define  WAITCHROMARETURN  (0x0 + 18 )
#define  WAITLUMAARIVE     (0x0 + 19 )
#define  WAITLUMARETURN    (0x0 + 20 )
#define  OVERLAYFLIPDEFERED (0x0 + 21 )
#define  DEFEREDOVERFLIP    (0x0 + 22 )

#define  TRANSFERLAUNCH		(0x0 + 23 )
#define  ALMEMRESTART      (0x0 + 24)

#define  NUMTIMTYPES 			 (ALMEMRESTART)


// this next one is used when the indice is hard to define in  that particular location
#define  INCNOTAP				(0xE)

#define  makeInd(vbibuf,vbisurf,imgbug,imgsurf) \
        (((((MySurfaces.DropSystem.dwDropFields & 0x3) | (MySurfaces.DropSystem.dwFullBool << 2))  & 0x0f) << 20)  |          \
			   ((INCNOTAP   & 0x0f) << 16)  |				\
			   ((vbibuf  & 0x0f) << 12)  |				\
			   ((vbisurf & 0x0f) << 8)  |				\
			   ((imgbug  & 0x0f) << 4 )  |				\
			   ((imgsurf & 0x0f)      )  )


#define  makeInd2(vbibuf,vbisurf,imgbug,imgsurf,field) \
         ( ((field   & 0x0f) << 16)  |          \
			  ((vbibuf  & 0x0f) << 12)  |				\
			  ((vbisurf & 0x0f) << 8)  |				\
			  ((imgbug  & 0x0f) << 4 )  |				\
			  ((imgsurf & 0x0f)      )  )


typedef struct _timeStore {

	U032					marker;
	U032					LoopCount;
	U032					Time0[MAXDATASTORE];
	U032					Time1[MAXDATASTORE];
	U032					SurfDat[MAXDATASTORE];
	U032					SurfIndex[MAXDATASTORE];
	U032					SurfPtr[MAXDATASTORE];
} timeStore;

void MarkTimingNot(TIMINGTYPES timingType,NvNotification*  pNot, U032 pSurf, U032 Indices);
void	MarkTiming(TIMINGTYPES timingType,U032 pSurf, U032 Indices);
void InitTiming(void);


#endif //timing_h
