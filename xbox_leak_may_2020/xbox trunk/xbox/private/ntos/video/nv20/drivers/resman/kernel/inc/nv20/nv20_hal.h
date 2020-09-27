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

//
// Macro for mirroring PFB reg values into texture cache via RDI.
//
#define RDI_REG_WR32(select, addr, data)    \
    {                                       \
        REG_WR32(NV_PGRAPH_RDI_INDEX,       \
                 DRF_NUM(_PGRAPH, _RDI_INDEX, _SELECT, select) |    \
                 DRF_NUM(_PGRAPH, _RDI_INDEX, _ADDRESS, addr));     \
        REG_WR32(NV_PGRAPH_RDI_DATA, data);                         \
    }

#define RDI_REG_RD32(select, addr, data)    \
    {                                       \
        REG_WR32(NV_PGRAPH_RDI_INDEX,       \
                 DRF_NUM(_PGRAPH, _RDI_INDEX, _SELECT, select) |    \
                 DRF_NUM(_PGRAPH, _RDI_INDEX, _ADDRESS, addr));     \
        data = REG_RD32(NV_PGRAPH_RDI_DATA);                        \
    }


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
    U032 PfbRemap;
    U032 PfbCstatus;
    U032 PfbRefCtrl;
    U032 PfbMrs;
    U032 PfbEmrs;
    U032 PfbRef;
    U032 PfbArbXferSz;
    U032 PfbArbTimeout;
    U032 PfbArbPredivider;
    U032 PfifoRamHt;
    U032 PfifoRamFc;
    U032 PfifoRamRo;
    NV20_TILE_VALUES TilingValues[NV_PFB_TILE__SIZE_1];
    NV20_ZCOMP_VALUES ZcompValues[NV_PFB_ZCOMP__SIZE_1];
    U032 ZcompOffset;
    U032 SaveCrtcConfig[2];             // should be MAX_CRTCS
    U032 PcrtcRasterStart[2];           // ""
    U032 PcrtcRasterStop[2];            // ""
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
BOOL IsNV20MaskRevA02_NV20(PMCHALINFO);
BOOL IsNV20MaskRevA03_NV20(PMCHALINFO);

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
// Drive/slew calculation constants.
//

// ************** Operating condition based on speedometer
#define SLOW_COUNT_EXT                  91
#define SLOW_COUNTB_EXT                 72
#define SLOW_COUNT_AVG                  100
#define SLOW_COUNTB_AVG                 79
#define TYPI_COUNT                      108
#define TYPI_COUNTB                     86
#define FAST_COUNT_AVG                  116
#define FAST_COUNTB_AVG                 93
#define FAST_COUNT_EXT                  123
#define FAST_COUNTB_EXT                 100

// ********** Interpolation Mode
//  Mode = 0 (Clamp to extreme limits)
//  Mode = 1 (Clamp to average limits)
//  Mode = 2 (Interpolate between extreme &
//            avg limits)
#define FBIO_INTERP_MODE     			0x2


// ********** EXTREME FAST Case Values
#define ADR_DRV_FALL_FAST_EXT 			0xa
#define ADR_DRV_RISE_FAST_EXT 			0x8
#define ADR_SLW_FALL_FAST_EXT 			0xb
#define ADR_SLW_RISE_FAST_EXT 			0x8

#define CLK_DRV_FALL_FAST_EXT 			0xa
#define CLK_DRV_RISE_FAST_EXT 			0x8
#define CLK_SLW_FALL_FAST_EXT 			0xb
#define CLK_SLW_RISE_FAST_EXT 			0x8

#define DAT_DRV_FALL_FAST_EXT 			0x9
#define DAT_DRV_RISE_FAST_EXT 			0x9
#define DAT_SLW_FALL_FAST_EXT 			0xe
#define DAT_SLW_RISE_FAST_EXT 			0x8

#define DQS_DRV_FALL_FAST_EXT 			0x9
#define DQS_DRV_RISE_FAST_EXT 			0x9
#define DQS_SLW_FALL_FAST_EXT 			0xe
#define DQS_SLW_RISE_FAST_EXT 			0x8

#define DQS_INB_DELY_FAST_EXT 			0x4 


// ********** Average FAST Case Values
#define ADR_DRV_FALL_FAST_AVG 			0xa
#define ADR_DRV_RISE_FAST_AVG 			0x8
#define ADR_SLW_FALL_FAST_AVG 			0xb
#define ADR_SLW_RISE_FAST_AVG 			0x8

#define CLK_DRV_FALL_FAST_AVG 			0xa
#define CLK_DRV_RISE_FAST_AVG 			0x8
#define CLK_SLW_FALL_FAST_AVG 			0xb
#define CLK_SLW_RISE_FAST_AVG 			0x8

#define DAT_DRV_FALL_FAST_AVG 			0x9
#define DAT_DRV_RISE_FAST_AVG 			0x9
#define DAT_SLW_FALL_FAST_AVG 			0xe
#define DAT_SLW_RISE_FAST_AVG 			0x8

#define DQS_DRV_FALL_FAST_AVG 			0x9
#define DQS_DRV_RISE_FAST_AVG 			0x9
#define DQS_SLW_FALL_FAST_AVG 			0xe
#define DQS_SLW_RISE_FAST_AVG 			0x8

#define DQS_INB_DELY_FAST_AVG 			0x6 


// ********** TYPICAL Case Values
#define ADR_DRV_FALL_TYPI    			0xa
#define ADR_DRV_RISE_TYPI    			0x8
#define ADR_SLW_FALL_TYPI    			0xb
#define ADR_SLW_RISE_TYPI    			0x8

#define CLK_DRV_FALL_TYPI    			0xa
#define CLK_DRV_RISE_TYPI    			0x8
#define CLK_SLW_FALL_TYPI    			0xb
#define CLK_SLW_RISE_TYPI    			0x8

#define DAT_DRV_FALL_TYPI    			0x9
#define DAT_DRV_RISE_TYPI    			0x9
#define DAT_SLW_FALL_TYPI    			0xe
#define DAT_SLW_RISE_TYPI    			0x8

#define DQS_DRV_FALL_TYPI    			0x9
#define DQS_DRV_RISE_TYPI    			0x9
#define DQS_SLW_FALL_TYPI    			0xe
#define DQS_SLW_RISE_TYPI    			0x8

#define DQS_INB_DELY_TYPI    			0x5 


// ********** Average SLOW Case Values
#define ADR_DRV_FALL_SLOW_AVG 			0xc
#define ADR_DRV_RISE_SLOW_AVG 			0xb
#define ADR_SLW_FALL_SLOW_AVG 			0xb
#define ADR_SLW_RISE_SLOW_AVG 			0x8

#define CLK_DRV_FALL_SLOW_AVG 			0xc
#define CLK_DRV_RISE_SLOW_AVG 			0xb
#define CLK_SLW_FALL_SLOW_AVG 			0xb
#define CLK_SLW_RISE_SLOW_AVG 			0x8

#define DAT_DRV_FALL_SLOW_AVG 			0xa
#define DAT_DRV_RISE_SLOW_AVG 			0xb
#define DAT_SLW_FALL_SLOW_AVG 			0xe
#define DAT_SLW_RISE_SLOW_AVG 			0x8

#define DQS_DRV_FALL_SLOW_AVG 			0xa
#define DQS_DRV_RISE_SLOW_AVG 			0xb
#define DQS_SLW_FALL_SLOW_AVG 			0xe
#define DQS_SLW_RISE_SLOW_AVG 			0x8

#define DQS_INB_DELY_SLOW_AVG 			0x4 

// ********** EXTREME SLOW Case Values
#define ADR_DRV_FALL_SLOW_EXT 			0xe
#define ADR_DRV_RISE_SLOW_EXT 			0xe
#define ADR_SLW_FALL_SLOW_EXT 			0xb
#define ADR_SLW_RISE_SLOW_EXT 			0x8

#define CLK_DRV_FALL_SLOW_EXT 			0xe
#define CLK_DRV_RISE_SLOW_EXT 			0xe
#define CLK_SLW_FALL_SLOW_EXT 			0xb
#define CLK_SLW_RISE_SLOW_EXT 			0x8

#define DAT_DRV_FALL_SLOW_EXT 			0xb
#define DAT_DRV_RISE_SLOW_EXT 			0xe
#define DAT_SLW_FALL_SLOW_EXT 			0xe
#define DAT_SLW_RISE_SLOW_EXT 			0x8

#define DQS_DRV_FALL_SLOW_EXT 			0xb
#define DQS_DRV_RISE_SLOW_EXT 			0xe
#define DQS_SLW_FALL_SLOW_EXT 			0xe
#define DQS_SLW_RISE_SLOW_EXT 			0x8

#define DQS_INB_DELY_SLOW_EXT 			0x3 

//
// This data structure contains all of the drive/slew parameters.
//
typedef struct _drv_slw_parms
{
    U016 MaxMClk;

    U008 SlowCountExt;
    U008 SlowCountBExt;
    U008 SlowCountAvg;
    U008 SlowCountBAvg;
    U008 TypiCount;
    U008 TypiCountB;
    U008 FastCountAvg;
    U008 FastCountBAvg;
    U008 FastCountExt;
    U008 FastCountBExt;

    U008 FbioInterpMode;

    // Extreme FAST ....
    U008 AdrDrvFallFastExt;
    U008 AdrDrvRiseFastExt;
    U008 AdrSlwFallFastExt;
    U008 AdrSlwRiseFastExt;

    U008 ClkDrvFallFastExt;
    U008 ClkDrvRiseFastExt;
    U008 ClkSlwFallFastExt;
    U008 ClkSlwRiseFastExt;

    U008 DatDrvFallFastExt;
    U008 DatDrvRiseFastExt;
    U008 DatSlwFallFastExt;
    U008 DatSlwRiseFastExt;

    U008 DqsDrvFallFastExt;
    U008 DqsDrvRiseFastExt;
    U008 DqsSlwFallFastExt;
    U008 DqsSlwRiseFastExt;

    U008 DqsInbDelyFastExt;

    // Average FAST ...
    U008 AdrDrvFallFastAvg;
    U008 AdrDrvRiseFastAvg;
    U008 AdrSlwFallFastAvg;
    U008 AdrSlwRiseFastAvg;

    U008 ClkDrvFallFastAvg;
    U008 ClkDrvRiseFastAvg;
    U008 ClkSlwFallFastAvg;
    U008 ClkSlwRiseFastAvg;

    U008 DatDrvFallFastAvg;
    U008 DatDrvRiseFastAvg;
    U008 DatSlwFallFastAvg;
    U008 DatSlwRiseFastAvg;

    U008 DqsDrvFallFastAvg;
    U008 DqsDrvRiseFastAvg;
    U008 DqsSlwFallFastAvg;
    U008 DqsSlwRiseFastAvg;

    U008 DqsInbDelyFastAvg;

    // Typical...
    U008 AdrDrvFallTypi;
    U008 AdrDrvRiseTypi;
    U008 AdrSlwFallTypi;
    U008 AdrSlwRiseTypi;

    U008 ClkDrvFallTypi;
    U008 ClkDrvRiseTypi;
    U008 ClkSlwFallTypi;
    U008 ClkSlwRiseTypi;

    U008 DatDrvFallTypi;
    U008 DatDrvRiseTypi;
    U008 DatSlwFallTypi;
    U008 DatSlwRiseTypi;

    U008 DqsDrvFallTypi;
    U008 DqsDrvRiseTypi;
    U008 DqsSlwFallTypi;
    U008 DqsSlwRiseTypi;

    U008 DqsInbDelyTypi;

    // Average SLOW ...
    U008 AdrDrvFallSlowAvg;
    U008 AdrDrvRiseSlowAvg;
    U008 AdrSlwFallSlowAvg;
    U008 AdrSlwRiseSlowAvg;

    U008 ClkDrvFallSlowAvg;
    U008 ClkDrvRiseSlowAvg;
    U008 ClkSlwFallSlowAvg;
    U008 ClkSlwRiseSlowAvg;

    U008 DatDrvFallSlowAvg;
    U008 DatDrvRiseSlowAvg;
    U008 DatSlwFallSlowAvg;
    U008 DatSlwRiseSlowAvg;

    U008 DqsDrvFallSlowAvg;
    U008 DqsDrvRiseSlowAvg;
    U008 DqsSlwFallSlowAvg;
    U008 DqsSlwRiseSlowAvg;

    U008 DqsInbDelySlowAvg;

    // Extreme SLOW ...
    U008 AdrDrvFallSlowExt;
    U008 AdrDrvRiseSlowExt;
    U008 AdrSlwFallSlowExt;
    U008 AdrSlwRiseSlowExt;

    U008 ClkDrvFallSlowExt;
    U008 ClkDrvRiseSlowExt;
    U008 ClkSlwFallSlowExt;
    U008 ClkSlwRiseSlowExt;

    U008 DatDrvFallSlowExt;
    U008 DatDrvRiseSlowExt;
    U008 DatSlwFallSlowExt;
    U008 DatSlwRiseSlowExt;

    U008 DqsDrvFallSlowExt;
    U008 DqsDrvRiseSlowExt;
    U008 DqsSlwFallSlowExt;
    U008 DqsSlwRiseSlowExt;

    U008 DqsInbDelySlowExt;
} DRVSLWPARMS, *PDRVSLWPARMS;

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
    U032 linesPerTileShift;         // based on # of column bits
    U032 partitions;
    U032 usedTiles;                 // bitmask of used HW tile ranges
    FBHALZCOMPSTATE_NV20 zCompState[NV_PFB_ZCOMP__SIZE_1];
    U008 zTagPool[MAX_ZTAG_BITS/8]; // ztag bitmap
    DRVSLWPARMS drvSlwParms;        // drive/slew parms
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
    U032 hasCelsius;                // needed for VTX_FMT war
    GRAPHICSSTATE_NV20 grSnapShots[NUM_FIFOS_NV20];

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

//--------------------------------------------------------------------
// Misc
//--------------------------------------------------------------------

//
// We need to workaround a scaled_image bug in the hardware that
// causes an extra 64bytes to be fetched and limit checked
// on the input image buffer.
//
// The war involves bumping up the limit of the context dma to
// avoid the limit check and to also be sure there's some sort
// of real memory to cover the bogus 64byte read.
//
// This macro defines the necessary offset.  We currently use 256bytes
// since that seems to be the minimum necessary alignment constraint.
//
#define NV20_SCALED_IMAGE_WAR_OFFSET            0x100

#endif // _NV20_HAL_H_
