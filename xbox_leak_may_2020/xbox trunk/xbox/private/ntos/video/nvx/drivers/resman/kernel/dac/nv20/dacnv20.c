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

/******************************** DAC Arbitration **************************\
*                                                                           *
* Module: DACNV20.C                                                         *
*   The DAC manager.                                                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nv20_ref.h>
#include <nvrm.h>
#include <nv20_hal.h>
#include "nvhw.h"
#include <vga.h>

/* This file simulates the NV arbitor */
#include <arb_fifo.h>
#include "nvhalcpp.h"

//////////////////////////////////////////
//
// Arbitration routines
//

static
VOID NV20_dacCalculateArbitration
(
    PHALHWINFO pHalHwInfo,
    fifo_info *fifo,
    sim_state  *arb
)
{
  int data, pagemiss, cas,width, video_enable, color_key_enable, bpp, align;
  int nvclks, mclks, pclks, vpagemiss, crtpagemiss, vbs;
  int nvclk_fill, us_extra;
  int found, mclk_extra, mclk_loop, cbs, m1;
  int mclk_freq, pclk_freq, nvclk_freq, mp_enable;
  int us_m, us_m_min, us_n, us_p, video_drain_rate, crtc_drain_rate;
  int vus_m, vus_n, vus_p;
  int vpm_us, us_video, vlwm, cpm_us, us_crt,clwm;
  int clwm_rnd_down;
  int craw, m2us, us_pipe, us_pipe_min, vus_pipe, p1clk, p2;
  int pclks_2_top_fifo, min_mclk_extra;
  int us_min_mclk_extra;

  fifo->valid = 1;
  pclk_freq = arb->pclk_khz; // freq in KHz
  mclk_freq = arb->mclk_khz;
  nvclk_freq = arb->nvclk_khz;
  pagemiss = arb->mem_page_miss;
  cas = arb->mem_latency;
  width = arb->memory_width/64;
  video_enable = arb->enable_video;
  color_key_enable = arb->gr_during_vid;
  bpp = arb->pix_bpp;
  align = arb->mem_aligned;
  mp_enable = arb->enable_mp;
  clwm = 0;
  vlwm = 1024;

  cbs = 512;
  vbs = 512;

  pclks = 4; // lwm detect.

  nvclks = 3; // lwm -> sync.
  nvclks += 2; // fbi bus cycles (1 req + 1 busy)

  mclks  = 1;   // 2 edge sync.  may be very close to edge so just put one.

  mclks += 1;   // arb_hp_req
  mclks += 5;   // ap_hp_req   tiling pipeline

  mclks += 2;    // tc_req     latency fifo
  mclks += 2;    // fb_cas_n_  memory request to fbio block
  mclks += 7;    // sm_d_rdv   data returned from fbio block

  // fb.rd.d.Put_gc   need to accumulate 256 bits for read
  if (arb->memory_type == 0)
  {
    if (arb->memory_width == 64) // 64 bit bus
      mclks += 4;
    else
      mclks += 2;
  }
  else
  {
    if (arb->memory_width == 64) // 64 bit bus
      mclks += 2;
    else
      mclks += 1;
  }

  if ((!video_enable) && (arb->memory_width == 128))
  {  
    mclk_extra = (bpp == 32) ? 31 : 42; // Margin of error
    min_mclk_extra = 17;
  }
  else
  {
    mclk_extra = (bpp == 32) ? 8 : 4; // Margin of error
    //mclk_extra = 4; // Margin of error
    min_mclk_extra = 18;
  }

  nvclks += 1; // 2 edge sync.  may be very close to edge so just put one.
  nvclks += 1; // fbi_d_rdv_n
  nvclks += 1; // Fbi_d_rdata
  nvclks += 1; // crtfifo load

  if(mp_enable)
    mclks+=4; // Mp can get in with a burst of 8.
  // Extra clocks determined by heuristics

  nvclks += 0;
  pclks += 0;
  found = 0;
  while(found != 1) {
    fifo->valid = 1;
    found = 1;
    mclk_loop = mclks+mclk_extra;
    us_m = mclk_loop *1000*1000 / mclk_freq; // Mclk latency in us
    us_m_min = mclks * 1000*1000 / mclk_freq; // Minimum Mclk latency in us
    us_min_mclk_extra = min_mclk_extra *1000*1000 / mclk_freq;
    us_n = nvclks*1000*1000 / nvclk_freq;// nvclk latency in us
    us_p = pclks*1000*1000 / pclk_freq;// nvclk latency in us
    us_pipe = us_m + us_n + us_p;
    us_pipe_min = us_m_min + us_n + us_p;
    us_extra = 0;

    vus_m = mclk_loop *1000*1000 / mclk_freq; // Mclk latency in us
    vus_n = (4)*1000*1000 / nvclk_freq;// nvclk latency in us
    vus_p = 0*1000*1000 / pclk_freq;// pclk latency in us
    vus_pipe = vus_m + vus_n + vus_p;

    if(video_enable) {
      video_drain_rate = pclk_freq * 4; // MB/s
      crtc_drain_rate = pclk_freq * bpp/8; // MB/s

      vpagemiss = 1; // self generating page miss
      vpagemiss += 1; // One higher priority before

      crtpagemiss = 2; // self generating page miss
      if(mp_enable)
          crtpagemiss += 1; // if MA0 conflict

      vpm_us = (vpagemiss * pagemiss)*1000*1000/mclk_freq;

      us_video = vpm_us + vus_m; // Video has separate read return path

      cpm_us = crtpagemiss  * pagemiss *1000*1000/ mclk_freq;
      us_crt =
        us_video  // Wait for video
        +cpm_us // CRT Page miss
        +us_m + us_n +us_p // other latency
        ;

      clwm = us_crt * crtc_drain_rate/(1000*1000);
      clwm++; // fixed point <= float_point - 1.  Fixes that
    } else {
      crtc_drain_rate = pclk_freq * bpp/8; // bpp * pclk/8

      crtpagemiss = 1; // self generating page miss
      crtpagemiss += 1; // MA0 page miss
      if(mp_enable)
          crtpagemiss += 1; // if MA0 conflict
      cpm_us = crtpagemiss  * pagemiss *1000*1000/ mclk_freq;
      us_crt =  cpm_us + us_m + us_n + us_p ;
      clwm = us_crt * crtc_drain_rate/(1000*1000);
      clwm++; // fixed point <= float_point - 1.  Fixes that

/*
        //
        // Another concern, only for high pclks so don't do this
        // with video:
        // What happens if the latency to fetch the cbs is so large that
        // fifo empties.  In that case we need to have an alternate clwm value
        // based off the total burst fetch
        //
        us_crt = (cbs * 1000 * 1000)/ (8*width)/mclk_freq ;
        us_crt = us_crt + us_m + us_n + us_p + (4 * 1000 * 1000)/mclk_freq;
        clwm_mt = us_crt * crtc_drain_rate/(1000*1000);
        clwm_mt ++;
        if(clwm_mt > clwm)
            clwm = clwm_mt;
*/
        // Finally, a heuristic check when width == 64 bits
        if(width == 1){
            nvclk_fill = nvclk_freq * 8;
            if(crtc_drain_rate * 100 >= nvclk_fill * 102)
                    clwm = 0xfff; //Large number to fail

            else if(crtc_drain_rate * 100  >= nvclk_fill * 98) {
                clwm = 1024;
                cbs = 512;
                us_extra = (cbs * 1000 * 1000)/ (8*width)/mclk_freq ;
            }
        }
    }


    /*
      Overfill check:

      */

    clwm_rnd_down = ((int)clwm/8)*8;
    if (clwm_rnd_down < clwm)
        clwm += 8;
        
    m1 = clwm + cbs -  1024; /* Amount of overfill */
    m2us = us_pipe_min + us_min_mclk_extra;
    pclks_2_top_fifo = (1024-clwm)/(8*width);
    
    /* pclk cycles to drain */
    p1clk = m2us * pclk_freq/(1000*1000); 
    p2 = p1clk * bpp / 8; // bytes drained.
    
    if((p2 < m1) && (m1 > 0)) {
    	fifo->valid = 0;
    	found = 0;
    	if(min_mclk_extra == 0)   {
    	  if(cbs <= 32) {
    	    found = 1; // Can't adjust anymore!
    	  } else {
    	    cbs = cbs/2;  // reduce the burst size
    	  }
    	} else {
    	  min_mclk_extra--;
    	}
    } else {
      if (clwm > 1023){ // Have some margin
    	fifo->valid = 0;
    	found = 0;
    	if(min_mclk_extra == 0)   
            found = 1; // Can't adjust anymore!
    	else 
            min_mclk_extra--;
      }
    }
    craw = clwm;
    
    if(clwm < (1024-cbs+8)) clwm = 1024-cbs+8;
    data = (int)(clwm);
    //  printf("CRT LWM: %f bytes, prog: 0x%x, bs: 256\n", clwm, data );
    fifo->crtc1_lwm = data;   fifo->crtc1_burst_size = cbs;

    //  printf("VID LWM: %f bytes, prog: 0x%x, bs: %d\n, ", vlwm, data, vbs );
    fifo->video_lwm = 1024;  fifo->video_burst_size = 512;
  }
}

/*
//
// Determine if the video overlay is usable in the current configuration.  This
// routine will also update the global variables in pDev which determine the 
// maximum downscaling allowed at various source image sizes.
//
//  Return 0 if not capable (at any upscale or downscale factor)
//  Return ~0 if capable (see pDev for downscale limits)
//
U032 NV20_dacCalculateVideoCaps
(
    PHWINFO pDev
)
{
    //
    // First look for a match of memory type, resolution, and clocks.
    //
    // NOTE: For now, until I can straighten out all of the hardware data,
    // assume a conservative limit.
    //
    switch (pDev->Chip.HalInfo.MaskRevision)
    {
        case 0xA1:
        case 0xA2:
        case 0xA6:
        
            pDev->Video.OverlayMaxDownscale_768  = 2;
            pDev->Video.OverlayMaxDownscale_1280 = 2;
            pDev->Video.OverlayMaxDownscale_1920 = 2;
            break;
            
        default:

            pDev->Video.OverlayMaxDownscale_768  = 8;
            pDev->Video.OverlayMaxDownscale_1280 = 4;
            pDev->Video.OverlayMaxDownscale_1920 = 2;
            break;
    }

    //
    // For now, always assume we can support some level of overlay
    // (although not to the same level of downscale ability)
    //
    return 1;

}
*/

typedef struct tagNV20_DacAGPClockEntry
{
    U032 frequency;
    U032 setting;
} NV20_DacAGPClockEntry;

NV20_DacAGPClockEntry NV20_dacAGPClockTable[] =
{
    { 133000000, NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_133MHZ },
    { 126000000, NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_126MHZ },
    { 120000000, NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_120MHZ },
    { 113000000, NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_113MHZ },
    { 106000000, NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_106MHZ },
    { 100000000, NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_100MHZ },
    { 94000000, NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_94MHZ },
    { 87000000, NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_87MHZ },
    { 80000000, NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_80MHZ },
    { 73000000, NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_73MHZ },
    { 66000000, NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_66MHZ },
    { 0, NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_66MHZ - 1 }
};

//
// HAL stuff
//

// forwards
RM_STATUS nvHalDacControl_NV20(VOID *);
RM_STATUS nvHalDacAlloc_NV20(VOID *);
RM_STATUS nvHalDacFree_NV20(VOID *);
RM_STATUS nvHalDacSetStartAddr_NV20(VOID *);
RM_STATUS nvHalDacProgramMClk_NV20(VOID *);
RM_STATUS nvHalDacProgramNVClk_NV20(VOID *);
RM_STATUS nvHalDacProgramPClk_NV20(VOID *);
RM_STATUS nvHalDacProgramCursorImage_NV20(VOID *);
RM_STATUS nvHalDacGetRasterPosition_NV20(VOID *);
RM_STATUS nvHalDacValidateArbSettings_NV20(VOID *arg);
RM_STATUS nvHalDacUpdateArbSettings_NV20(VOID *arg);

//
// nvHalDacControl
//
RM_STATUS
nvHalDacControl_NV20(VOID *arg)
{
    PDACCONTROLARG_000 pDacControlArg = (PDACCONTROLARG_000)arg;
    PHALHWINFO  pHalHwInfo;
    PDACHALINFO pDacHalInfo;
    PVIDEOHALINFO       pVideoHalInfo;
    PDACHALINFO_NV20    pDacHalPvtInfo;

    //
    // Verify interface revision.
    //
    if (pDacControlArg->id != DAC_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacControlArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pVideoHalInfo = pHalHwInfo->pVideoHalInfo;
    pDacHalPvtInfo = (PDACHALINFO_NV20)pHalHwInfo->pDacHalInfo;

    switch (pDacControlArg->cmd)
    {
        case DAC_CONTROL_INIT:
        {
            U032 i, data32, crtcAddr;
            U008 lock;

            // default to 128bit wide bus on nv20
            pDacHalInfo->InputWidth = 128;
            pDacHalInfo->CrtcInfo[0].PCLKLimit8bpp  = 350000000;
            pDacHalInfo->CrtcInfo[0].PCLKLimit16bpp = 350000000;
            pDacHalInfo->CrtcInfo[0].PCLKLimit32bpp = 300000000;
            pDacHalInfo->CrtcInfo[0].RegOffset = 0x00000000;

            //
            // Head 0 supports everything (?).
            //
            pDacHalInfo->CrtcInfo[0].DisplayTypesAllowed =
                DISPLAYTYPEBIT(DISPLAY_TYPE_MONITOR) |
                DISPLAYTYPEBIT(DISPLAY_TYPE_TV) |
                DISPLAYTYPEBIT(DISPLAY_TYPE_DUALSURFACE) |
                DISPLAYTYPEBIT(DISPLAY_TYPE_FLAT_PANEL);

            //
            // Head 1 supports TV and flat panel.
            //
            pDacHalInfo->CrtcInfo[1].DisplayTypesAllowed =
                DISPLAYTYPEBIT(DISPLAY_TYPE_TV) |
                DISPLAYTYPEBIT(DISPLAY_TYPE_FLAT_PANEL);


            // turn on CRTC big endian on 2nd head if we're setup for big endian mode
            if (pHalHwInfo->pMcHalInfo->EndianControl & MC_ENDIAN_CONTROL_CHIP_BIG_ENDIAN)
            {
                U032 crtcOffset;
                U032 config;
                crtcOffset = pDacHalInfo->CrtcInfo[1].RegOffset;
                config = REG_RD32(NV_PCRTC_CONFIG + crtcOffset);
                config |= DRF_DEF(_PCRTC, _CONFIG, _ENDIAN, _BIG);
                REG_WR32(NV_PCRTC_CONFIG + crtcOffset, config);
            }

            //
            // While we're here, increase TV out latency.
            //
            for (i = 0; i < pDacHalInfo->NumCrtcs; i++)
            {
                crtcAddr = pDacHalInfo->CrtcInfo[i].RegOffset;

                // Unlock CRTC extended regs
                HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock, crtcAddr);
                HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, crtcAddr);

                // Add 4 per Raj Rao...
                HAL_CRTC_RD(NV_CIO_CRE_TVOUT_LATENCY, data32, crtcAddr);
                data32 += 4;
                HAL_CRTC_WR(NV_CIO_CRE_TVOUT_LATENCY, data32, crtcAddr);

                // Relock if necessary...
                if (lock == 0)
                    HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, crtcAddr);
            }

            //
            // Set default downscale values.
            // 
            pVideoHalInfo->OverlayMaxDownscale_768  = 8;
            pVideoHalInfo->OverlayMaxDownscale_1280 = 8;
            pVideoHalInfo->OverlayMaxDownscale_1920 = 8;

            // turn on CRTC big endian if we're setup for big endian mode
            if (pHalHwInfo->pMcHalInfo->EndianControl & MC_ENDIAN_CONTROL_CHIP_BIG_ENDIAN)
            {
                FLD_WR_DRF_NUM(_PCRTC, _CONFIG, _ENDIAN, NV_PCRTC_CONFIG_ENDIAN_BIG);
	    }
            break;
        }
        case DAC_CONTROL_LOAD:
        case DAC_CONTROL_UNLOAD:
        case DAC_CONTROL_DESTROY:
        case DAC_CONTROL_UPDATE:
            break;
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }

    return (RM_OK);
}

RM_STATUS
nvHalDacAlloc_NV20(VOID *arg)
{
    PDACALLOCARG_000 pDacAllocArg = (PDACALLOCARG_000)arg;

    //
    // Verify interface revision.
    //
    if (pDacAllocArg->id != DAC_ALLOC_000)
        return (RM_ERR_VERSION_MISMATCH);

    return (RM_OK);
}

RM_STATUS
nvHalDacFree_NV20(VOID *arg)
{
    PDACFREEARG_000 pDacFreeArg = (PDACFREEARG_000)arg;

    //
    // Verify interface revision.
    //
    if (pDacFreeArg->id != DAC_FREE_000)
        return (RM_ERR_VERSION_MISMATCH);    

    return (RM_OK);
}

RM_STATUS
nvHalDacSetStartAddr_NV20(VOID *arg)
{
    PDACSETSTARTADDRARG_000 pDacSetStartAddrArg = (PDACSETSTARTADDRARG_000)arg;
    PHALHWINFO  pHalHwInfo;
    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pDacHalObj;
    PDACHALINFO pDacHalInfo;
    U032        crtcOffset;

    //
    // Verify interface revision.
    //
    if (pDacSetStartAddrArg->id != DAC_SET_START_ADDR_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacSetStartAddrArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pDacHalObj = (PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT)pDacSetStartAddrArg->pHalObjInfo;

    crtcOffset = pDacHalInfo->CrtcInfo[pDacHalObj->Head].RegOffset;
    REG_WR32(NV_PCRTC_START + crtcOffset, pDacSetStartAddrArg->startAddr);

    return (RM_OK);
}

//
// nvHalDacProgramMClk_NV20 - Program MCLK based on the value in pDev->Dac.HalInfo.MClk
//
// This routine uses the value in pDev->Dac.HalInfo.MClk (in Hz) to calculate
// M, N, and P values.  Those values are then stored in the pDev->Dac
// structure.  The hardware is then programmed with the new M, N, and P
// values in a manner that is safe for the PLL.
//
RM_STATUS
nvHalDacProgramMClk_NV20(VOID *arg)
{
    PDACPROGRAMMCLKARG_000 pDacProgramMClkArg = (PDACPROGRAMMCLKARG_000) arg;
    PHALHWINFO      pHalHwInfo;
    PDACHALINFO     pDacHalInfo;
    PMCHALINFO      pMcHalInfo;

    RM_STATUS status;
    U032 mNew;
    U032 nNew;
    U032 pNew;
    U032 mOld;
    U032 nOld;
    U032 pOld;
    U032 coeff;
    U032 oldMClk;
    U032 oldCyclesPerRefresh;
    U032 oldPeriod;
    U032 newCyclesPerRefresh;
    U032 Loops;
    U032 IsPllLocked;
    U032 Head = 0; // MPLL is on head 0
    status = RM_OK;

    //
    // Verify interface revision.
    //
    if (pDacProgramMClkArg->id != DAC_PROGRAMMCLK_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacProgramMClkArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pMcHalInfo = pHalHwInfo->pMcHalInfo;

    mNew = pDacHalInfo->MPllM;
    nNew = pDacHalInfo->MPllN;
    pNew = pDacHalInfo->MPllP;

    // Program the hardware

    // Retrieve original coefficients
    coeff = REG_RD32( NV_PRAMDAC_MPLL_COEFF );
    mOld = DRF_VAL( _PRAMDAC, _MPLL_COEFF, _MDIV, coeff);
    nOld = DRF_VAL( _PRAMDAC, _MPLL_COEFF, _NDIV, coeff);
    pOld = DRF_VAL( _PRAMDAC, _MPLL_COEFF, _PDIV, coeff);

    // If p decreates then be careful
    if ( pNew < pOld )
    {
        // Write M and N first
	    REG_WR32
            (
                NV_PRAMDAC_MPLL_COEFF,
                DRF_NUM( _PRAMDAC, _MPLL_COEFF, _MDIV, mNew) | 
                DRF_NUM( _PRAMDAC, _MPLL_COEFF, _NDIV, nNew) | 
                DRF_NUM( _PRAMDAC, _MPLL_COEFF, _PDIV, pOld)
            );

        // Wait until M PLL is locked
        IsPllLocked = FALSE;
        for ( Loops = 100; Loops; --Loops )
        {
            if
            (
                REG_RD_DRF(_PRAMDAC, _PLL_TEST_COUNTER, _MPLL_LOCK)
                    == NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCKED
            )
            {
                // Check again, just to make sure.
                if
                (
                    REG_RD_DRF(_PRAMDAC, _PLL_TEST_COUNTER, _MPLL_LOCK)
                        == NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCKED
                )
                {
                    IsPllLocked = TRUE;
                    break;
                }
            }

            osDelay(1 /* ms */);
        }

        if ( !IsPllLocked )
        {
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: M PLL not locked\n");
        }
    }

    // Now we can write the final value
	REG_WR32
        (
            NV_PRAMDAC_MPLL_COEFF,
            DRF_NUM( _PRAMDAC, _MPLL_COEFF, _MDIV, mNew) | 
            DRF_NUM( _PRAMDAC, _MPLL_COEFF, _NDIV, nNew) | 
            DRF_NUM( _PRAMDAC, _MPLL_COEFF, _PDIV, pNew)
        );

    // Update the number of clock cycles per memory refresh cycle
    
    // Determine original frequency from old M, N, P values
    oldMClk  = (nOld * pMcHalInfo->CrystalFreq / (1 << pOld) / mOld);

    // Determine original cycles per refresh
    oldCyclesPerRefresh = REG_RD_DRF( _PFB, _TIMING2, _REFRESH ) * 32;

    // Determine original refresh period
    oldPeriod = oldCyclesPerRefresh * 1024 / ( oldMClk / 1000 );
    
    // Now using the new MCLK, figure out the new cycles per refresh
    newCyclesPerRefresh = ( pDacHalInfo->MClk / 1000 ) * oldPeriod / 1024;

    // Store the new value
    FLD_WR_DRF_NUM( _PFB, _TIMING2, _REFRESH, newCyclesPerRefresh / 32 );
    
    return status;
}

//
// nvHalDacProgramNVClk_NV20 - Program NVCLK based on the value in pDev->Dac.HalInfo.NVClk
//
// This routine uses the value in pDev->Dac.HalInfo.NVClk (in Hz) to calculate
// M, N, and P values.  Those values are then stored in the pDev->Dac
// structure.  The hardware is then programmed with the new M, N, and P
// values in a manner that is safe for the PLL.
//
RM_STATUS
nvHalDacProgramNVClk_NV20(VOID *arg)
{
    PDACPROGRAMNVCLKARG_000 pDacProgramNVClkArg = (PDACPROGRAMNVCLKARG_000) arg;
    PHALHWINFO      pHalHwInfo;
    PDACHALINFO     pDacHalInfo;
    PMCHALINFO      pMcHalInfo;

    RM_STATUS status;
    U032 mNew;
    U032 nNew;
    U032 pNew;
    U032 mOld;
    U032 nOld;
    U032 pOld;
    U032 coeff;
    U032 i;
    U032 Loops;
    U032 IsPllLocked;
    U032 Head = 0; // NVPLL is on head 0
    status = RM_OK;

    //
    // Verify interface revision.
    //
    if (pDacProgramNVClkArg->id != DAC_PROGRAMNVCLK_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacProgramNVClkArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pMcHalInfo = pHalHwInfo->pMcHalInfo;

    mNew = pDacHalInfo->NVPllM;
    nNew = pDacHalInfo->NVPllN;
    pNew = pDacHalInfo->NVPllP;

    // Program the hardware

    // Retrieve original coefficients
    coeff = REG_RD32( NV_PRAMDAC_NVPLL_COEFF );
    mOld = DRF_VAL( _PRAMDAC, _NVPLL_COEFF, _MDIV, coeff);
    nOld = DRF_VAL( _PRAMDAC, _NVPLL_COEFF, _NDIV, coeff);
    pOld = DRF_VAL( _PRAMDAC, _NVPLL_COEFF, _PDIV, coeff);

    // If p decreates then be careful
    if ( pNew < pOld )
    {
        // Write M and N first
	    REG_WR32
            (
                NV_PRAMDAC_NVPLL_COEFF,
                DRF_NUM( _PRAMDAC, _NVPLL_COEFF, _MDIV, mNew) | 
                DRF_NUM( _PRAMDAC, _NVPLL_COEFF, _NDIV, nNew) | 
                DRF_NUM( _PRAMDAC, _NVPLL_COEFF, _PDIV, pOld)
            );

        // Wait until NV PLL is locked
        IsPllLocked = FALSE;
        for ( Loops = 100; Loops; --Loops )
        {
            if
            (
                REG_RD_DRF(_PRAMDAC, _PLL_TEST_COUNTER, _NVPLL_LOCK)
                    == NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCKED
            )
            {
                // Check again, just to make sure.
                if
                (
                    REG_RD_DRF(_PRAMDAC, _PLL_TEST_COUNTER, _NVPLL_LOCK)
                        == NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCKED
                )
                {
                    IsPllLocked = TRUE;
                    break;
                }
            }

            osDelay(1 /* ms */);
        }

        if ( !IsPllLocked )
        {
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: NV PLL not locked\n");
        }
    }

    // Now we can write the final value
	REG_WR32
        (
            NV_PRAMDAC_NVPLL_COEFF,
            DRF_NUM( _PRAMDAC, _NVPLL_COEFF, _MDIV, mNew) | 
            DRF_NUM( _PRAMDAC, _NVPLL_COEFF, _NDIV, nNew) | 
            DRF_NUM( _PRAMDAC, _NVPLL_COEFF, _PDIV, pNew)
        );

    // Update NV_PBUS_DEBUG_3 to control data throttling in AGP 4X mode
    i = 0;
    while ( ( pDacHalInfo->NVClk <= NV20_dacAGPClockTable[ i ].frequency ) &&
        ( i < sizeof( NV20_dacAGPClockTable ) / sizeof( NV20_dacAGPClockTable[ 0 ] ) - 1 ) )
    {
        i++;
    }

    FLD_WR_DRF_NUM( _PBUS, _DEBUG_3, _AGP_4X_NVCLK, NV20_dacAGPClockTable[ i ].setting );
    
    return status;
}

//
// nvHalDacProgramPClk_NV20
//
RM_STATUS
nvHalDacProgramPClk_NV20(VOID *arg)
{
    PDACPROGRAMPCLKARG_000 pDacProgramPClkArg = (PDACPROGRAMPCLKARG_000) arg;
    PHALHWINFO      pHalHwInfo;
    PDACHALINFO     pDacHalInfo;
    PMCHALINFO      pMcHalInfo;
    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pDacHalObj;
    U032            CurDacAdr; // must be defined for CRTC_RD macro & must point to pDacHalInfo
    U032            Head;
    U032            PixelClock;

    RM_STATUS status;
    U032 mNew;
    U032 nNew;
    U032 pNew;
    U032 mOld;
    U032 nOld;
    U032 pOld;
    U032 coeff;
    U032 Loops;
    U032 IsPllLocked;

    status = RM_OK;

    //
    // Verify interface revision.
    //
    if (pDacProgramPClkArg->id != DAC_PROGRAMPCLK_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacProgramPClkArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pMcHalInfo = pHalHwInfo->pMcHalInfo;
    pDacHalObj = (PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT) pDacProgramPClkArg->pHalObjInfo;
    CurDacAdr = pDacHalInfo->CrtcInfo[pDacHalObj->Head].RegOffset;

    mNew = pDacHalInfo->VPllM;
    nNew = pDacHalInfo->VPllN;
    pNew = pDacHalInfo->VPllP;

    Head = pDacHalObj->Head;
    PixelClock = pDacProgramPClkArg->PixelClock;

    // Program the hardware

    // Retrieve original coefficients
    // NV11 VPLL2 unfortunately is not at the "expected" offset, so we can't use the HAL macro (DAC_REG_RD32)
    if (Head == 0) 
    {
        coeff = REG_RD32( NV_PRAMDAC_VPLL_COEFF );
    }
    else
    {
        coeff = REG_RD32( NV_PRAMDAC_VPLL2_COEFF );
    }
    mOld = DRF_VAL( _PRAMDAC, _VPLL_COEFF, _MDIV, coeff);
    nOld = DRF_VAL( _PRAMDAC, _VPLL_COEFF, _NDIV, coeff);
    pOld = DRF_VAL( _PRAMDAC, _VPLL_COEFF, _PDIV, coeff);

    // If p decreates then be careful
    if ( pNew < pOld )
    {
        // Write M and N first. 
        if (Head == 0) 
        {
            REG_WR32
            (
                NV_PRAMDAC_VPLL_COEFF,
                DRF_NUM( _PRAMDAC, _VPLL_COEFF, _MDIV, mNew) | 
                DRF_NUM( _PRAMDAC, _VPLL_COEFF, _NDIV, nNew) | 
                DRF_NUM( _PRAMDAC, _VPLL_COEFF, _PDIV, pOld)
            );
        }
        else
        {
            REG_WR32
            (
                NV_PRAMDAC_VPLL2_COEFF,
                DRF_NUM( _PRAMDAC, _VPLL_COEFF, _MDIV, mNew) | 
                DRF_NUM( _PRAMDAC, _VPLL_COEFF, _NDIV, nNew) | 
                DRF_NUM( _PRAMDAC, _VPLL_COEFF, _PDIV, pOld)
            );
        }

        // Wait until V PLL is locked
        IsPllLocked = FALSE;
        for ( Loops = 100; Loops; --Loops )
        {
            if
            (
                HAL_DAC_REG_RD_DRF(_PRAMDAC, _PLL_TEST_COUNTER, _VPLL_LOCK, CurDacAdr)
                    == NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCKED
            )
            {
                // Check again, just to make sure.
                if
                (
                    HAL_DAC_REG_RD_DRF(_PRAMDAC, _PLL_TEST_COUNTER, _VPLL_LOCK, CurDacAdr)
                        == NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCKED
                )
                {
                    IsPllLocked = TRUE;
                    break;
                }
            }

            osDelay(1 /* ms */);
        }

        if ( !IsPllLocked )
        {
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: VPLL not locked\n");
        }
    }

    // Now we can write the final value
    if (Head == 0) 
    {
        REG_WR32
        (
            NV_PRAMDAC_VPLL_COEFF,
            DRF_NUM( _PRAMDAC, _VPLL_COEFF, _MDIV, mNew) | 
            DRF_NUM( _PRAMDAC, _VPLL_COEFF, _NDIV, nNew) | 
            DRF_NUM( _PRAMDAC, _VPLL_COEFF, _PDIV, pNew)
        );
    }
    else
    {
        REG_WR32
        (
            NV_PRAMDAC_VPLL2_COEFF,
            DRF_NUM( _PRAMDAC, _VPLL_COEFF, _MDIV, mNew) | 
            DRF_NUM( _PRAMDAC, _VPLL_COEFF, _NDIV, nNew) | 
            DRF_NUM( _PRAMDAC, _VPLL_COEFF, _PDIV, pNew)
        );
    }

    return status;
}

//
// nvHalDacProgramCursorImage_NV20
//
RM_STATUS
nvHalDacProgramCursorImage_NV20(VOID *arg)
{
    PDACPROGRAMCURSORARG_000 pDacProgramCursorImgArg = (PDACPROGRAMCURSORARG_000) arg;
    PHALHWINFO  pHalHwInfo;
    PDACHALINFO pDacHalInfo;
    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pDacHalObj;
    U032        CurDacAdr; // must be defined for CRTC_RD macro & must point to pDacHalInfo
    U032        cursorConfig;

    //
    // Verify interface revision.
    //
    if (pDacProgramCursorImgArg->id != DAC_PROGRAMCURSOR_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacProgramCursorImgArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pDacHalObj = (PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT) pDacProgramCursorImgArg->pHalObjInfo;
    CurDacAdr = pDacHalInfo->CrtcInfo[pDacHalObj->Head].RegOffset;

    // Read the cursorConfig register.
    cursorConfig = HAL_DAC_REG_RD32(NV_PCRTC_CURSOR_CONFIG, CurDacAdr);

    // Preserve the enable and scan_double bits. Clear the rest
    cursorConfig &= ((1 << DRF_SHIFT(NV_PCRTC_CURSOR_CONFIG_ENABLE)) |
                     (1 << DRF_SHIFT(NV_PCRTC_CURSOR_CONFIG_SCAN_DOUBLE)));

    // Mark the Address Space Indicator to indicate where the cursor image is, 
    // based on the param "asi".
    cursorConfig |= (pDacProgramCursorImgArg->asi << DRF_SHIFT(NV_PCRTC_CURSOR_CONFIG_ADDRESS_SPACE));

    // The supported cursor image sizes are 32x32 or 64x64.
    if (pDacProgramCursorImgArg->width == 32)
    {
        // 32x32
        cursorConfig |= DRF_NUM(_PCRTC, _CURSOR_CONFIG, _CUR_PIXELS, NV_PCRTC_CURSOR_CONFIG_CUR_PIXELS_32);
        cursorConfig |= DRF_NUM(_PCRTC, _CURSOR_CONFIG, _CUR_LINES, NV_PCRTC_CURSOR_CONFIG_CUR_LINES_32);
    }
    else
    {
        //64x64
        cursorConfig |= DRF_NUM(_PCRTC, _CURSOR_CONFIG, _CUR_PIXELS, NV_PCRTC_CURSOR_CONFIG_CUR_PIXELS_64);
        cursorConfig |= DRF_NUM(_PCRTC, _CURSOR_CONFIG, _CUR_LINES, NV_PCRTC_CURSOR_CONFIG_CUR_LINES_64);
    }

    // The different color formats are implemented by two bits: The Blend bit and Bpp bit.
    //       Blend      Bpp        Format
    //       ----------------------------
    //         0         0     ->  ROP1R5G5B5
    //         0         1     ->  PM_A8R8G8B8  *** This format is only supported in NV15 or later.
    //         1         0     ->  A1R5G5B5
    //         1         1     ->  A8R8G8B8
    switch (pDacProgramCursorImgArg->colorFormat)
    {
        case NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_A1R5G5B5:
            // Set blend = 0. Bpp is already 0
            cursorConfig |= (NV_PCRTC_CURSOR_CONFIG_CUR_BLEND_ALPHA << DRF_SHIFT(NV_PCRTC_CURSOR_CONFIG_CUR_BLEND));
            break;
        case NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_A8R8G8B8:
            // set blend = 1, bpp = 1
            cursorConfig |= (NV_PCRTC_CURSOR_CONFIG_CUR_BLEND_ALPHA << DRF_SHIFT(NV_PCRTC_CURSOR_CONFIG_CUR_BLEND));
            cursorConfig |= (NV_PCRTC_CURSOR_CONFIG_CUR_BPP_32 << DRF_SHIFT(NV_PCRTC_CURSOR_CONFIG_CUR_BPP));
            break;
        case NV07C_SET_CURSOR_IMAGE_FORMAT_COLOR_PM_LE_A8R8G8B8:
                // set bpp = 1, blend is already 0 (only supported in NV15 or later)
            cursorConfig |= (NV_PCRTC_CURSOR_CONFIG_CUR_BPP_32 << DRF_SHIFT(NV_PCRTC_CURSOR_CONFIG_CUR_BPP));
            // else fall through to default
        default:
            // Leave blend and bpp both 0 (ROP1R5G5B5)
            break;
    }

    // write the new cursor config register value.
    HAL_DAC_REG_WR32(NV_PCRTC_CURSOR_CONFIG, cursorConfig, CurDacAdr);

    //            
    // If not driving a TV and we're on an NV15 or later, turn on the LONG_PIPE to extend the cursor FIFO
    // when running 32bpp or alpha cursors; otherwise, keep it short.
    //
    // Note that this means that 16bpp Alpha and 32bpp cursors are not allowed on a TV.
    // XXXscottl - But we've increased the TVOUT_LATENCY value, so 
    //             lets allow 32bpp cursors on tv for now.
    //
    // preset to short 
    HAL_DAC_FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _PIPE, _SHORT, CurDacAdr); 

    if (HAL_GETDISPLAYTYPE(pDacHalObj->Head) != DISPLAY_TYPE_TV)    
    {
        if ((pDacProgramCursorImgArg->colorFormat == NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_A1R5G5B5) ||
            (pDacProgramCursorImgArg->colorFormat == NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_A8R8G8B8) ||
            (pDacProgramCursorImgArg->colorFormat == NV07C_SET_CURSOR_IMAGE_FORMAT_COLOR_PM_LE_A8R8G8B8))
        {    
            HAL_DAC_FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _PIPE, _LONG, CurDacAdr);
        }
    }

    // Program the start address.
    // If the address space indicator says it is in Instance Memory, the startAddr is in terms of 
    // paragraphs instead of bytes. Adjust it to bytes.
    if (pDacProgramCursorImgArg->asi == NV_PCRTC_CURSOR_CONFIG_ADDRESS_SPACE_PINST)
    {
        pDacProgramCursorImgArg->startAddr <<= 4;
    }

    HAL_DAC_REG_WR32(NV_PCRTC_CURSOR, pDacProgramCursorImgArg->startAddr, CurDacAdr);

    return RM_OK;
}

//
// nvHalDacGetRasterPosition_NV20
//
RM_STATUS
nvHalDacGetRasterPosition_NV20(VOID *arg)
{
    PDACRASTERPOSARG_000 pDacRasterPosArg = (PDACRASTERPOSARG_000) arg;
    PHALHWINFO pHalHwInfo;

    //
    // Verify interface revision.
    //
    if (pDacRasterPosArg->id != DAC_RASTERPOS_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacRasterPosArg->pHalHwInfo;

    // TO DO: use head number

    // no RL0/RL1 index regs, so return NV_PCRTC_RASTER_POSITION
    pDacRasterPosArg->result =  (U016) REG_RD_DRF(_PCRTC, _RASTER, _POSITION);

    return (RM_OK);
}

//
// Attempt to validate the given parameters will work with the current configuration
//
//  Return 0  if not capable
//  Return ~0 if valid
//
RM_STATUS
nvHalDacValidateArbSettings_NV20(VOID *arg)
{
    PDACVALIDATEARBSETTINGSARG_000    pDacValidateArbSettingsArg = (PDACVALIDATEARBSETTINGSARG_000) arg;
    PHALHWINFO          pHalHwInfo;
    PDACHALINFO         pDacHalInfo;
    PFBHALINFO          pFbHalInfo;
    PMCHALINFO          pMcHalInfo;
    PPROCESSORHALINFO   pProcessorHalInfo;
    PVIDEOHALINFO       pVideoHalInfo;

	fifo_info   fifo_data;
	sim_state   sim_data;
    U032        pclk_limit;

    //
    // Verify interface revision.
    //
    if (pDacValidateArbSettingsArg->id != DAC_VALIDATEARBSETTINGS_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacValidateArbSettingsArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pFbHalInfo = pHalHwInfo->pFbHalInfo;
    pMcHalInfo = pHalHwInfo->pMcHalInfo;
    pProcessorHalInfo = pHalHwInfo->pProcessorHalInfo;
    pVideoHalInfo = pHalHwInfo->pVideoHalInfo;

    //
    // First we must validate that the incoming vclk is within our DAC range.
    //
    switch (pDacValidateArbSettingsArg->bpp)
    {
        case 8:
            pclk_limit = pDacHalInfo->CrtcInfo[0].PCLKLimit8bpp;
            break;
        case 15:
        case 16:
            pclk_limit = pDacHalInfo->CrtcInfo[0].PCLKLimit16bpp;
            break;
        case 24:
        case 32:
            pclk_limit = pDacHalInfo->CrtcInfo[0].PCLKLimit32bpp;
            break;
        default:
            pclk_limit = pDacHalInfo->CrtcInfo[0].PCLKLimit16bpp;
            break;
    }

    //
    // If incoming clock is greater than DAC range, return failure
    //
    if (pDacValidateArbSettingsArg->vclk > pclk_limit)
    {
        pDacValidateArbSettingsArg->result = 0;
        return (RM_OK);
    }
        
	//
	// Build the sim table using current system settings
	//
	sim_data.pix_bpp	    = (char) pDacValidateArbSettingsArg->bpp;
	sim_data.enable_video	= pDacValidateArbSettingsArg->video_enabled ? 1 : 0;
	sim_data.enable_mp	    = pDacValidateArbSettingsArg->mport_enabled ? 1 : 0;
	sim_data.memory_width   = pDacHalInfo->InputWidth;
	sim_data.mem_latency	= 3;
    sim_data.memory_type    = (pFbHalInfo->RamType == BUFFER_DDRAM) ? 1 : 0;    // 1=DD,0=SD
	sim_data.mem_aligned	= 1;
    sim_data.mem_page_miss  = 10;
	sim_data.gr_during_vid	= pVideoHalInfo->ColorKeyEnabled ? 1 : 0;
	sim_data.pclk_khz	    = pDacValidateArbSettingsArg->vclk / 1000;  // in kHz, not MHz
	sim_data.mclk_khz	    = pDacHalInfo->MClk / 1000;	                // in kHz, not MHz
    sim_data.nvclk_khz      = pDacHalInfo->NVClk / 1000;                // in kHz, not MHz

    //
	// Run the numbers through the ringer
	//
    NV20_dacCalculateArbitration(pHalHwInfo, &fifo_data, &sim_data);

    /*
    //
    // If the CRTC can do this, double check to make sure the overlay is able as well
    //
    if (fifo_data.valid && video_enabled)
        fifo_data.valid = NV20_dacCalculateVideoCaps(pDev);
    */
    
    pDacValidateArbSettingsArg->result = (U032)fifo_data.valid;
    return (RM_OK);
}

RM_STATUS
nvHalDacUpdateArbSettings_NV20(VOID *arg)
{
    PDACUPDATEARBSETTINGSARG_000     pDacUpdateArbSettingsArg = (PDACUPDATEARBSETTINGSARG_000) arg;
    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pDacHalObj;
    PHALHWINFO          pHalHwInfo;
    PDACHALINFO         pDacHalInfo;
    PFBHALINFO          pFbHalInfo;
    PMCHALINFO          pMcHalInfo;
    PPROCESSORHALINFO   pProcessorHalInfo;
    PVIDEOHALINFO       pVideoHalInfo;
    PMPHALINFO          pMpHalInfo;
    U032                Head;
    U032                CurDacAdr;
    DACVALIDATEARBSETTINGSARG_000   dacValidateArbSettingsArg;

    fifo_info   fifo_data;
    sim_state   sim_data;
    U008        lock;
    U032        M, N, O, P;

    //
    // Verify interface revision.
    //
    if (pDacUpdateArbSettingsArg->id != DAC_UPDATEARBSETTINGS_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacUpdateArbSettingsArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pFbHalInfo = pHalHwInfo->pFbHalInfo;
    pMcHalInfo = pHalHwInfo->pMcHalInfo;
    pProcessorHalInfo = pHalHwInfo->pProcessorHalInfo;
    pVideoHalInfo = pHalHwInfo->pVideoHalInfo;
    pMpHalInfo = pHalHwInfo->pMpHalInfo;
    pDacHalObj = (PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT) pDacUpdateArbSettingsArg->pHalObjInfo;
    CurDacAdr = pDacHalInfo->CrtcInfo[pDacHalObj->Head].RegOffset;
    Head = pDacHalObj->Head;

    //
    // Recalculate the current PCLK setting.  We might have changed modes
    // recently.
    //
    M = REG_RD_DRF(_PRAMDAC, _VPLL_COEFF, _MDIV);
    N = REG_RD_DRF(_PRAMDAC, _VPLL_COEFF, _NDIV);
    P = REG_RD_DRF(_PRAMDAC, _VPLL_COEFF, _PDIV);
    O = 1;
    
    pDacHalInfo->VPllM = M;
    pDacHalInfo->VPllN = N;
    pDacHalInfo->VPllO = O;
    pDacHalInfo->VPllP = P;
    pDacHalInfo->VClk  = (N * pMcHalInfo->CrystalFreq / (1 << P) / M);
    
    //
    // Check if we can support an overlay in this resolution.  If we can, set the default desktop timings
    // to assume we are running an overlay.
    //
    dacValidateArbSettingsArg.id = DAC_VALIDATEARBSETTINGS_000;
    dacValidateArbSettingsArg.pHalHwInfo = pHalHwInfo;
    dacValidateArbSettingsArg.bpp = pDacHalInfo->Depth;
    dacValidateArbSettingsArg.video_enabled = 1;
    dacValidateArbSettingsArg.mport_enabled = 0;
    dacValidateArbSettingsArg.vclk = pDacHalInfo->PixelClock * 10000;

    // if (NV20_dacValidateArbitrationSettings(pHalHwInfo, pDacHalInfo->Depth, 1, 0, (pDev->Dac.PixelClock * 10000)))
    nvHalDacValidateArbSettings_NV20(&dacValidateArbSettingsArg);
    if (dacValidateArbSettingsArg.result)
        sim_data.enable_video = 1;
    else
        sim_data.enable_video = 0;
    
	sim_data.pix_bpp	    = (char) pDacHalInfo->Depth;
	//sim_data.enable_video	= pDev->Video.Enabled ? 1 : 0;
	sim_data.enable_mp	    = pMpHalInfo->Enabled ? 1 : 0;
	sim_data.memory_width   = pDacHalInfo->InputWidth;
	sim_data.mem_aligned	= 1;
    sim_data.memory_type    = (pFbHalInfo->RamType == BUFFER_DDRAM) ? 1 : 0; //1=DD,0=SD
	sim_data.mem_latency	= 3;
    sim_data.mem_page_miss  = 10;
	sim_data.gr_during_vid	= pVideoHalInfo->ColorKeyEnabled ? 1 : 0;
	sim_data.pclk_khz	    = pDacHalInfo->VClk / 1000;	    // in kHz, not MHz
	sim_data.mclk_khz	    = pDacHalInfo->MClk / 1000;	    // in kHz, not MHz
    sim_data.nvclk_khz      = pDacHalInfo->NVClk / 1000;    // in kHz, not MHz

    // 
	// Get those new numbers
	//
    NV20_dacCalculateArbitration(pHalHwInfo, &fifo_data, &sim_data);
    
    // for now, assume pass
    //fifo_data.valid = 1;
	
//	For TV, we should not change some of these. Gfx Fifo Low Water Mark in particular should
//		not be changed. 
//    if (pDev->Framebuffer.DisplayType != DISPLAY_TYPE_TV) // leave alone if TV

    //
	// If valid settings found, update the hardware
	//
	if (fifo_data.valid)
	{
        //
        // Set the DAC FIFO Thresholds and burst size

        /*
        //
        // Check overlay capabilities.
        //
        if (!NV20_dacCalculateVideoCaps(pDev))
            pDev->Video.OverlayAllowed = 0;
        */

        //
        // These are the PVIDEO burst/watermark hardcoded values
        // we've gotten from the HW engineers.
        //
        FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_2, _BURST1, 16);         // 512 bytes
        FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_2, _BURST2, 16);         // 512 bytes
        FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_3, _WATER_MARK1, 64);    // 1024 bytes
        FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_3, _WATER_MARK2, 64);    // 1024 bytes

		//
        // Update the CRTC watermarks
        //
		// Unlock CRTC extended regs
        HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock, CurDacAdr);
        HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, CurDacAdr);

#if 0
        // XXX these are temporary CRTC burst/watermark hardcoded values
        // we've gotten from the HW engineers.
        CRTC_WR(NV_CIO_CRE_FF_INDEX, NV_CIO_CRE_FF_BURST_512);  // 512 bytes

        // XXX bump up the LWM if we're running a higher resolution (we base
        // this check on the primary surface display pitch). This is still too
        // simple a heuristic ... but it has shown to work for 1920x1440x32.

        //if (pDev->Framebuffer.DisplayPitch < 0x2000) {
        if (pDev->Dac.HalInfo.VClk < 170000000) {
		    CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, 64);               // 512 bytes
        } else {
		    CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, 80);               // 640 bytes
        }

#else
        //
        // Set the CRTC watermarks and burst size
        //
		HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, (U008) (fifo_data.crtc1_lwm >> 3), CurDacAdr);
		switch (fifo_data.crtc1_burst_size)
		{
			case 512:
				HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 5, CurDacAdr);
				break;
			case 256:
				HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 4, CurDacAdr);
				break;
			case 128:
				HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 3, CurDacAdr);
				break;
			case 64:
				HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 2, CurDacAdr);
				break;
			case 32:
				HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 1, CurDacAdr);
				break;
			case 16:
				HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 0, CurDacAdr);
				break;
		}
#endif
        //
        // Relock if necessary
        //
		if (lock == 0)
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, CurDacAdr);
        
		return (RM_OK);
	}
	else
    {
        //
        // No valid setting was found!!!
        //
        //  Make a conservative guess and fail.
        //
        
		// Unlock CRTC extended regs
        HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock, CurDacAdr);
        HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, CurDacAdr);
        
		HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, 0x72, CurDacAdr);
		HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 5, CurDacAdr);
        
        //
        // Relock if necessary
        //
		if (lock == 0)
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, CurDacAdr);
        
		return (RM_ERROR);
    }        
}
