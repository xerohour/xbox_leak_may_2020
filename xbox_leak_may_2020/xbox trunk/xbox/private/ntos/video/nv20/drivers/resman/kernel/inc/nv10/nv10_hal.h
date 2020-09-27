#ifndef _NV10_HAL_H_
#define _NV10_HAL_H_
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

/************************ NV10 HAL Defines and Structures ******************\
*                                                                           *
* Module: nv10_hal.h                                                        *
*       HAL interface defines and structures.                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv10_gr.h>

//--------------------------------------------------------------------
// Mc
//--------------------------------------------------------------------

typedef struct _tile_values_nv10 {
    U032    tile;                   // base and valid bit
    U032    tlimit;                 // limit of tile range
    U032    tsize;                  // tile pitch value
} NV10_TILE_VALUES;

typedef struct _def_mc_hal_power_info_nv10
{
    U032 PbusDebug1;
    U032 PfbTiming2;
    U032 PfbBoot0;
    U032 PfbConfig0;
    U032 PfbConfig1;
    U032 PfbMrs;
    U032 PfbEmrs;
    U032 PfbRef;
    U032 PfifoRamHt;
    U032 PfifoRamFc;
    U032 PfifoRamRo;
    NV10_TILE_VALUES TilingValues[NV_PFB_TILE__SIZE_1];
    U032 SaveCrtcConfig[2];
    U032 PcrtcRasterStart[MAX_CRTCS];
    U032 PcrtcRasterStop[MAX_CRTCS];
    U032 PvideoIntrEn;
    U032 PvideoBuffer;
    U032 PvideoStop;
    U032 PvideoBase0;
    U032 PvideoBase1;
    U032 PvideoLimit0;
    U032 PvideoLimit1;
    U032 PvideoLuminance0;
    U032 PvideoLuminance1;
    U032 PvideoChrominance0;
    U032 PvideoChrominance1;
    U032 PvideoOffset0;
    U032 PvideoOffset1;
    U032 PvideoSizeIn0;
    U032 PvideoSizeIn1;
    U032 PvideoPointIn0;
    U032 PvideoPointIn1;
    U032 PvideoDsDx0;
    U032 PvideoDsDx1;
    U032 PvideoDtDy0;
    U032 PvideoDtDy1;
    U032 PvideoPointOut0;
    U032 PvideoPointOut1;
    U032 PvideoSizeOut0;
    U032 PvideoSizeOut1;
    U032 PvideoFormat0;
    U032 PvideoFormat1;
    U032 PvideoColorKey;
} MCHALPOWERINFO_NV10, *PMCHALPOWERINFO_NV10;

typedef struct _def_mc_hal_info_nv10
{
    U032 SavePmc;
    U032 SaveIntrEn0;
    MCHALPOWERINFO_NV10 PowerInfo;
} MCHALINFO_NV10, *PMCHALINFO_NV10;

// chip id functions
BOOL IsNV10_NV10(PMCHALINFO);
BOOL IsNV15_NV10(PMCHALINFO);
BOOL IsNV10MaskRevA02orBetter_NV10(PMCHALINFO);
BOOL IsNV10orBetter_NV10(PMCHALINFO);
BOOL IsNV15orBetter_NV10(PMCHALINFO);
BOOL IsNV11orBetter_NV10(PMCHALINFO);
BOOL IsNV11_NV10(PMCHALINFO);
BOOL IsNV1A_NV10(PMCHALINFO);
BOOL IsNV15MaskRevA02_NV10(PMCHALINFO);
BOOL IsNV15MaskRevA03_NV10(PMCHALINFO);

VOID Nv1AInvalidateTile( PHALHWINFO, U032, U032 );

//--------------------------------------------------------------------
// Fifo
//--------------------------------------------------------------------

//
// 32 channels * 32 bytes each
//
#define NUM_FIFOS_NV10          32
#define FC_SIZE_NV10            32

typedef struct _def_fifo_hal_info_nv10 {
    U032 Mode;
    U032 InUse;
} FIFOHALINFO_NV10, *PFIFOHALINFO_NV10;

//--------------------------------------------------------------------
// Framebuffer
//--------------------------------------------------------------------
typedef struct _def_fb_hal_info_nv10
{
    U032 hashTableAddr;
    U032 hashTableSize;
    U032 fifoContextAddr;
    U032 fifoContextSize;
    U032 fifoRunoutAddr;
    U032 fifoRunoutSize;
    U032 fifoRunoutMask;
    U032 fbSave[1];
    U032 usedTiles;                 // bitmask of used HW tile ranges
    U032 uTileAlign;
} FBHALINFO_NV10, *PFBHALINFO_NV10;

//--------------------------------------------------------------------
// Graphics
//--------------------------------------------------------------------
typedef struct _def_graphics_channel_nv10
{
    //PCOMMONOBJECT       NotifyObject;
    U032                Notify;
    GRAPHICSPATCH_NV10  CurrentPatch;
    //U032                Debug3;      // postdither_2d control
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
    U032                ContextUser;
    U032                DmaStart0;
    U032                DmaStart1;
    U032                DmaLength;
    U032                DmaMisc;
    U032                DmaPitch;
} GRAPHICSCHANNEL_NV10, *PGRAPHICSCHANNEL_NV10;

typedef struct _def_graphics_hal_info_nv10
{
    U032 Debug0;                // overrides
    U032 Debug1;
    U032 Debug2;
    U032 Debug3;
    U032 Debug4;
    U032 SwathControl;

    U032 currentChID;
    GRAPHICSCHANNEL_NV10    grChannels[NUM_FIFOS_NV10];
    NvGraphicsState         grSnapShots[NUM_FIFOS_NV10];
    U032                    currentObjects3d[NUM_FIFOS_NV10];
} GRHALINFO_NV10, *PGRHALINFO_NV10;

//--------------------------------------------------------------------
// MediaPort
//--------------------------------------------------------------------

typedef struct _def_mp_hal_info_nv10
{
    U032 VIPSlavePresent;
} MPHALINFO_NV10, *PMPHALINFO_NV10;

// c04dnv10.c interfaces
extern RM_STATUS class04DMethod_NV10(PMPMETHODARG_000);
extern RM_STATUS class04DGetEventStatus_NV10(PMPGETEVENTSTATUSARG_000);
extern RM_STATUS class04DServiceEvent_NV10(PMPSERVICEEVENTARG_000);

// c04env10.c interfaces
extern RM_STATUS class04EMethod_NV10(PMPMETHODARG_000);
extern RM_STATUS class04EGetEventStatus_NV10(PMPGETEVENTSTATUSARG_000);
extern RM_STATUS class04EServiceEvent_NV10(PMPSERVICEEVENTARG_000);

// c04fnv10.c interfaces
extern RM_STATUS class04FMethod_NV10(PMPMETHODARG_000);
extern RM_STATUS class04FGetEventStatus_NV10(PMPGETEVENTSTATUSARG_000);
extern RM_STATUS class04FServiceEvent_NV10(PMPSERVICEEVENTARG_000);

//--------------------------------------------------------------------
// Video
//--------------------------------------------------------------------

// vidnv10.c interfaces
extern RM_STATUS videoInit_NV10(PHALHWINFO, U032);
extern U016 videoConvertScaleFactor_NV10(S012d020);
extern BOOL videoHwOwnsBuffer_NV10(PHALHWINFO, U032, PVIDEO_OVERLAY_HAL_OBJECT);
extern VOID videoAdjustScalarForTV_NV10(PHALHWINFO, U032);
extern VOID videoKickOffOverlay_NV10(PHALHWINFO, PVIDEO_OVERLAY_HAL_OBJECT, U032);
extern VOID videoStopOverlay_NV10(PHALHWINFO, PVIDEO_OVERLAY_HAL_OBJECT, U032);

// c047nv10.c interfaces
extern RM_STATUS class047Method_NV10(PVIDEOMETHODARG_000);

// c07anv10.c interfaces
extern RM_STATUS class07aMethod_NV10(PVIDEOMETHODARG_000);

// c63nv10.c interfaces
extern RM_STATUS class63Method_NV10(PVIDEOMETHODARG_000);
extern RM_STATUS class63InitXfer_NV10(PHALHWINFO, VOID *, U032);
extern VOID class63DualSurfaceDesktop_NV10(PHALHWINFO, U032);

// c64nv10.c interfaces
extern RM_STATUS class64Method_NV10(PVIDEOMETHODARG_000);

// c65nv10.c interfaces
extern RM_STATUS class65Method_NV10(PVIDEOMETHODARG_000);


//--------------------------------------------------------------------
// Dac
//--------------------------------------------------------------------
typedef struct _def_dac_hal_info_nv10
{
    U032 foo;
} DACHALINFO_NV10, *PDACHALINFO_NV10;

#endif // _NV10_HAL_H_
