#ifndef _NV4_HAL_H_
#define _NV4_HAL_H_
 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/************************ NV4 HAL Defines and Structures *******************\
*                                                                           *
* Module: nv4_hal.h                                                         *
*       HAL interface defines and structures.                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv4_gr.h>

//--------------------------------------------------------------------
// Mc
//--------------------------------------------------------------------

//
// *All* chip-dependent power state is stored here regardless
// of the engine to which it belongs.
//
typedef struct _def_mc_hal_power_info_nv04
{
    U032 PfbDebug0;
    U032 PfbBoot0;
    U032 PfbConfig0;
    U032 PfbConfig1;
    U032 PfifoRamHt;
    U032 PfifoRamFc;
    U032 PfifoRamRo;
} MCHALPOWERINFO_NV04, *PMCHALPOWERINFO_NV04;

typedef struct _def_mc_hal_info_nv04
{
    U032 SavePmc;
    U032 SaveIntrEn0;
    MCHALPOWERINFO_NV04 PowerInfo;
} MCHALINFO_NV04, *PMCHALINFO_NV04;

// chip id functions
BOOL IsNV4_NV04(PMCHALINFO);
BOOL IsNV5_NV04(PMCHALINFO);
BOOL IsNV0A_NV04(PMCHALINFO);
BOOL IsNV5orBetter_NV04(PMCHALINFO);

//--------------------------------------------------------------------
// Fifo
//--------------------------------------------------------------------

//
// 32 channels * 32 bytes each
//
#define NUM_FIFOS_NV04      16
#define FC_SIZE_NV04        32

typedef struct _def_fifo_hal_info_nv04 {
    U032 Mode;
    U032 InUse;
} FIFOHALINFO_NV04, *PFIFOHALINFO_NV04;

//--------------------------------------------------------------------
// Framebuffer
//--------------------------------------------------------------------

typedef struct _def_fb_hal_info_nv04
{
    U032 hashTableAddr;
    U032 hashTableSize;
    U032 fifoContextAddr;
    U032 fifoContextSize;
    U032 fifoRunoutAddr;
    U032 fifoRunoutSize;
    U032 fifoRunoutMask;
    U032 fbSave[2];
    U032 tilingDisabled;
    VOID *bufferBase;
} FBHALINFO_NV04, *PFBHALINFO_NV04;

//--------------------------------------------------------------------
// Graphics
//--------------------------------------------------------------------

//
// Graphics channel context switch data.
// 
typedef struct _def_graphics_channel_nv04
{
    //PCOMMONOBJECT       NotifyObject;
    U032                Notify;
    GRAPHICSPATCH_NV4   CurrentPatch;
    U032                Debug3;      // postdither_2d control
    U032                Exceptions;
    U032                ContextSwitch1;
    U032                ContextSwitch2;
    U032                ContextSwitch3;
    U032                ContextSwitch4;
    U032                ContextSwitch5;
    U032                ContextCache1[8];
    U032                ContextCache2[8];
    U032                ContextCache3[8];
    U032                ContextCache4[8];
    U032                ContextCache5[8];
#ifdef KJK
    PCOMMONOBJECT       CurrentObjects[8];
#endif
    U032                ContextUser;
    U032                DmaStart0;
    U032                DmaStart1;
    U032                DmaLength;
    U032                DmaMisc;
    U032                DmaPitch;
} GRAPHICSCHANNEL_NV04, *PGRAPHICSCHANNEL_NV04;

typedef struct _def_graphics_hal_info_nv04
{
    U032 Debug0;
    U032 Debug1;
    U032 Debug2;
    U032 Debug3;
    U032 currentChID;
    GRAPHICSCHANNEL_NV04 grChannels[NUM_FIFOS_NV04];
} GRHALINFO_NV04, *PGRHALINFO_NV04;

//--------------------------------------------------------------------
// Dac
//--------------------------------------------------------------------
typedef struct _def_dac_hal_info_nv04
{
    U032 foo;
} DACHALINFO_NV04, *PDACHALINFO_NV04;

//--------------------------------------------------------------------
// MediaPort
//--------------------------------------------------------------------

typedef struct _def_mp_hal_info_nv04
{
    VOID *CurrentDecompressor;
} MPHALINFO_NV04, *PMPHALINFO_NV04;

// c04dnv04.c interfaces
extern RM_STATUS class04DMethod_NV04(PMPMETHODARG_000);
extern RM_STATUS class04DGetEventStatus_NV04(PMPGETEVENTSTATUSARG_000);
extern RM_STATUS class04DServiceEvent_NV04(PMPSERVICEEVENTARG_000);

// c04env04.c interfaces
extern RM_STATUS class04EMethod_NV04(PMPMETHODARG_000);
extern RM_STATUS class04EGetEventStatus_NV04(PMPGETEVENTSTATUSARG_000);
extern RM_STATUS class04EServiceEvent_NV04(PMPSERVICEEVENTARG_000);

// c04fnv04.c interfaces
extern RM_STATUS class04FMethod_NV04(PMPMETHODARG_000);
extern RM_STATUS class04FGetEventStatus_NV04(PMPGETEVENTSTATUSARG_000);
extern RM_STATUS class04FServiceEvent_NV04(PMPSERVICEEVENTARG_000);

//--------------------------------------------------------------------
// Video
//--------------------------------------------------------------------

// vidnv04.c interfaces
extern RM_STATUS videoInit_NV04(PHALHWINFO, U032);
extern U016 videoConvertScaleFactor_NV04(S012d020);

// c047nv04.c interfaces
extern RM_STATUS class047Method_NV04(PVIDEOMETHODARG_000);
extern RM_STATUS class047InitXfer_NV04(PHALHWINFO, VOID *, U032);
extern RM_STATUS class047GetEventStatus_NV04(PVIDEOGETEVENTSTATUSARG_000);
extern RM_STATUS class047ServiceEvent_NV04(PVIDEOSERVICEEVENTARG_000);

// c63nv04.c interfaces
extern RM_STATUS class63Method_NV04(PVIDEOMETHODARG_000);
extern RM_STATUS class63InitXfer_NV04(PHALHWINFO, VOID *, U032);
extern VOID class63DualSurfaceDesktop_NV04(PHALHWINFO, U032);
extern RM_STATUS class63GetEventStatus_NV04(PVIDEOGETEVENTSTATUSARG_000);
extern RM_STATUS class63ServiceEvent_NV04(PVIDEOSERVICEEVENTARG_000);

// c64nv04.c interfaces
extern RM_STATUS class64Method_NV04(PVIDEOMETHODARG_000);

// c65nv04.c interfaces
extern RM_STATUS class65Method_NV04(PVIDEOMETHODARG_000);

#endif // _NV4_HAL_H_
