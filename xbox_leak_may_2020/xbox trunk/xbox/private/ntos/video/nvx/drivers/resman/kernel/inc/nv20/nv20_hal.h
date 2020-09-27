#ifndef _NV20_HAL_H_
#define _NV20_HAL_H_
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

/************************ NV20 HAL Defines and Structures ******************\
*                                                                           *
* Module: nv20_hal.h                                                        *
*       HAL interface defines and structures.                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv20/nv20_gr.h>

//--------------------------------------------------------------------
// Mc
//--------------------------------------------------------------------

typedef struct _tile_values_nv20 {
    U032 tile;                      // base and valid bit
    U032 tlimit;                    // limit of tile range
    U032 tsize;                     // tile pitch value
} NV20_TILE_VALUES;

typedef struct _zcomp_values_nv20 {
    U032    zcomp;
} NV20_ZCOMP_VALUES;

typedef struct _def_mc_hal_power_info_nv20
{
    U032 PbusDebug1;
    U032 PfbTiming2;
    U032 PfbCfg0;
    U032 PfbCfg1;
    U032 PfbCstatus;
    U032 PfbRefCtrl;
    U032 PfbMrs;
    U032 PfbEmrs;
    U032 PfbRef;
    U032 PfifoRamHt;
    U032 PfifoRamFc;
    U032 PfifoRamRo;
    NV20_TILE_VALUES TilingValues[NV_PFB_TILE__SIZE_1];
    NV20_ZCOMP_VALUES ZcompValues[NV_PFB_ZCOMP__SIZE_1];
    U032 ZcompOffset;
    U032 SaveCrtcConfig[2];
    U032 PcrtcRasterStart;
    U032 PcrtcRasterStop;    
    //mac vals
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
} MCHALPOWERINFO_NV20, *PMCHALPOWERINFO_NV20;

typedef struct _def_mc_hal_info_nv20
{
    U032 SavePmc;
    U032 SaveIntrEn0;
    MCHALPOWERINFO_NV20 PowerInfo;
} MCHALINFO_NV20, *PMCHALINFO_NV20;

// chip id functions
BOOL IsNV20_NV20(PMCHALINFO);

//--------------------------------------------------------------------
// Fifo
//--------------------------------------------------------------------

//
// 32 channels * 64 bytes each for fifo context area 1
// 32 channels * 16 bytes each for fifo context area 2
//
#define NUM_FIFOS_NV20          32
#define FC1_SIZE_NV20           64
#define FC2_SIZE_NV20           16

typedef struct _def_fifo_hal_info_nv20 {
    U032 Mode;
    U032 InUse;
} FIFOHALINFO_NV20, *PFIFOHALINFO_NV20;

//--------------------------------------------------------------------
// Framebuffer
//--------------------------------------------------------------------

//
// Each ZTAG bit maps to one 64byte tile per partition.
// So if there's 4 partitions, then each bit represents
// 256bytes of allocated fb space.
//
#define MAX_ZTAG_BITS               0x8000

typedef struct _def_fb_hal_zcomp_state_nv20
{
    U032 offset;                            // offset into ztag pool
    U032 size;                              // number of ztag bits
} FBHALZCOMPSTATE_NV20, *PFBHALZCOMPSTATE_NV20;

typedef struct _def_fb_hal_info_nv20
{
    U032 hashTableAddr;
    U032 hashTableSize;
    U032 fifoContextAddr1;          
    U032 fifoContextSize1;
    U032 fifoContextAddr2;
    U032 fifoContextSize2;
    U032 fifoRunoutAddr;
    U032 fifoRunoutSize;
    U032 fifoRunoutMask;
    U032 fbSave[2];
    U032 partitions;
    U032 usedTiles;                         // bitmask of used HW tile ranges
    FBHALZCOMPSTATE_NV20 zCompState[NV_PFB_ZCOMP__SIZE_1];
    U008 zTagPool[MAX_ZTAG_BITS/8];         // ztag bitmap
} FBHALINFO_NV20, *PFBHALINFO_NV20;

//--------------------------------------------------------------------
// Graphics
//--------------------------------------------------------------------

typedef struct _def_graphics_hal_info_nv20
{
    U032 Debug0;                    // NV_PGRAPH_DEBUG_0
    U032 Debug1;                    // NV_PGRAPH_DEBUG_1
    U032 Debug2;                    // NV_PGRAPH_DEBUG_2
    U032 Debug3;                    // NV_PGRAPH_DEBUG_3
    U032 Debug4;                    // NV_PGRAPH_DEBUG_4
    U032 Debug5;                    // NV_PGRAPH_DEBUG_5
    U032 Debug6;                    // NV_PGRAPH_DEBUG_6
    U032 Debug7;                    // NV_PGRAPH_DEBUG_7
    U032 Debug8;                    // NV_PGRAPH_DEBUG_8
    U032 Debug9;                    // NV_PGRAPH_DEBUG_9
    U032 currentChID;
    U032 CtxTable[NUM_FIFOS_NV20];  // graphics channel context table entries
    U032 CtxTableBase;              // base instance offset of table
    U032 currentObjects3d[NUM_FIFOS_NV20];
    NvGraphicsState grSnapShots[NUM_FIFOS_NV20];

#ifdef NV20_SW_CTX_SWITCH
    GRAPHICSCHANNEL_NV20    SwCtxTable[NUM_FIFOS_NV20];
#endif // NV20_SW_CTX_SWITCH
} GRHALINFO_NV20, *PGRHALINFO_NV20;

//--------------------------------------------------------------------
// MediaPort
//--------------------------------------------------------------------

typedef struct _def_mp_hal_info_nv20
{
    U032 VIPSlavePresent;
} MPHALINFO_NV20, *PMPHALINFO_NV20;

// c04dnv20.c interfaces
extern RM_STATUS class04DMethod_NV20(PMPMETHODARG_000);
extern RM_STATUS class04DGetEventStatus_NV20(PMPGETEVENTSTATUSARG_000);
extern RM_STATUS class04DServiceEvent_NV20(PMPSERVICEEVENTARG_000);

// c04env20.c interfaces
extern RM_STATUS class04EMethod_NV20(PMPMETHODARG_000);
extern RM_STATUS class04EGetEventStatus_NV20(PMPGETEVENTSTATUSARG_000);
extern RM_STATUS class04EServiceEvent_NV20(PMPSERVICEEVENTARG_000);

// c04fnv20.c interfaces
extern RM_STATUS class04FMethod_NV20(PMPMETHODARG_000);
extern RM_STATUS class04FGetEventStatus_NV20(PMPGETEVENTSTATUSARG_000);
extern RM_STATUS class04FServiceEvent_NV20(PMPSERVICEEVENTARG_000);

//--------------------------------------------------------------------
// Video
//--------------------------------------------------------------------

// vidnv20.c interfaces
extern RM_STATUS videoInit_NV20(PHALHWINFO, U032);
extern U016 videoConvertScaleFactor_NV20(S012d020);
extern BOOL videoHwOwnsBuffer_NV20(PHALHWINFO, U032, PVIDEO_OVERLAY_HAL_OBJECT);
extern VOID videoAdjustScalarForTV_NV20(PHALHWINFO, U032);
extern VOID videoKickOffOverlay_NV20(PHALHWINFO, PVIDEO_OVERLAY_HAL_OBJECT, U032);
extern VOID videoStopOverlay_NV20(PHALHWINFO, PVIDEO_OVERLAY_HAL_OBJECT, U032);

// c047nv10.c interfaces
extern RM_STATUS class047Method_NV20(PVIDEOMETHODARG_000);

// c07anv10.c interfaces
extern RM_STATUS class07aMethod_NV20(PVIDEOMETHODARG_000);

// c63nv10.c interfaces
extern RM_STATUS class63Method_NV20(PVIDEOMETHODARG_000);
extern RM_STATUS class63InitXfer_NV20(PHALHWINFO, VOID *, U032);
extern VOID class63DualSurfaceDesktop_NV20(PHALHWINFO, U032);

// c64nv10.c interfaces
extern RM_STATUS class64Method_NV20(PVIDEOMETHODARG_000);

// c65nv10.c interfaces
extern RM_STATUS class65Method_NV20(PVIDEOMETHODARG_000);


//--------------------------------------------------------------------
// Dac
//--------------------------------------------------------------------
typedef struct _def_dac_hal_info_nv20
{
    U032 foo;
} DACHALINFO_NV20, *PDACHALINFO_NV20;

#endif // _NV20_HAL_H_
