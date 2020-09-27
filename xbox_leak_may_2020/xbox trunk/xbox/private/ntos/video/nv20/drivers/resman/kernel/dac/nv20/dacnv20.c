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

//
// We're using NV11-style arbitration in anticipation of dualhead
// support on nv20.
//
static U032
adjust_scaler_burstsize(U032 raw, U032 min, U032 max)
{
  U032 adj;
  
  /* scaler burst sizes are a multiple of min bytes */
  adj = (raw + (min - 1)) / min * min;
  /* max burst size is 2048 bytes */
  if (adj > max)
    adj = max;  
  return adj;
}

//LPL Moved from init.c area.
static void
check_scaler_burstsize(scaler_t *scaler)
{
  U032 raw;

  if (!(scaler->enable)) 
    return;

  raw = adjust_scaler_burstsize(scaler->burst_size, scaler->min_burst_size, scaler->max_burst_size);

  if (raw != scaler->burst_size)
    scaler->burst_size = raw;
}


// Data structure init routines

static void
check_crtc_burstsizes(crtc_t *crtc)
{
  U032 raw = crtc->burst_size;
  
  if (!crtc->enable) return;
 
  /* crtc burst sizes are a multiple of 32 bytes */
  raw = (raw + 31) / 31 * 31;
  /* burst sizes step by 16 up to 2048 bytes*/
  if (raw > crtc->max_burst_size)
    raw = crtc->max_burst_size;
  if (raw != crtc->burst_size)
    crtc->burst_size = raw;
}

static void
init_crtc(crtc_t *crtc,
      U032 enable,
      U032 hres,
      U032 vres,
      U032 nativeVRes,
      U032 depth,
      U032 refresh,
      U032 request_delay_mclks,
      U032 request_delay_pclks,
      U032 burst_size,
      U032 min_burst_size,
      U032 max_burst_size,
      U032 cursor_burst_size,
      U032 pclk,
      U032 htotal,
      mem_t *mem
      )
{
  crtc->fifo.lwm = 0;
  crtc->enable = enable;
  crtc->hres = hres;
  crtc->vres = vres;
  crtc->nativeVRes = nativeVRes;
  crtc->depth = depth;
  crtc->drain_rate = depth*100 / 8;
  crtc->refresh = refresh;
  crtc->pclk = pclk;
  crtc->pclk_period = 1000000 / crtc->pclk; //now in ps, no longer ns
  crtc->request_delay = request_delay_mclks * mem->mclk_period + request_delay_pclks * crtc->pclk_period;
  crtc->burst_size = burst_size;
  crtc->min_burst_size = min_burst_size;
  crtc->max_burst_size = max_burst_size;
  crtc->cursor_burst_size = cursor_burst_size;
  /* make sure set burst sizes are legitimate */
  check_crtc_burstsizes(crtc);
  crtc->fifo.size = 0;
  crtc->hclk_period = crtc->pclk_period * htotal / 1000;
}

static void
init_scaler(scaler_t *scaler,
        crtc_t *crtc,
        crtc_t *crtc2,
        U032 nvclk,
        U032 enable,
        U032 source_hres,
        U032 source_vres,
        U032 h2_owns_vs,
        U032 depth,
        U032 hres,
        U032 vres,
        U032 burst_size,
        U032 min_burst_size,
        U032 max_burst_size,
        U032 request_delay_nvclks,
        U032 request_delay_pclks,
        U032 max_fifo_size
        )
{
  if (h2_owns_vs)
    scaler->pclk_period = crtc2->pclk_period;
  else
    scaler->pclk_period = crtc->pclk_period;

  scaler->h2_owns_vs    = h2_owns_vs;
  scaler->max_fifo_size = max_fifo_size;
 
  scaler->enable = enable;
  if (!enable) {
    scaler->burst_size = 0;
    return;
  }
  scaler->depth = depth;

  scaler->source_hres = source_hres;
  scaler->source_vres = source_vres;

  /* assume full screen scaling */
  scaler->window_hres = (h2_owns_vs) ? crtc2->hres : crtc->hres;
  scaler->window_vres = (h2_owns_vs) ? crtc2->vres : crtc->vres;

  /* This code does not allow for distorted scaling.
   * I.e., the horizontal and vertical scale factors are 
   * forced to be the same such that the video window is as
   * large as possible on the display without any geometric
   * distortion.
   */
  scaler->hscale = (1000*scaler->window_hres) / scaler->source_hres;
  scaler->vscale = (1000*scaler->window_vres) / scaler->source_vres;
  if (scaler->hscale > scaler->vscale)
    scaler->hscale = scaler->vscale;
  else
    scaler->vscale = scaler->hscale;

  //Normally, the bandwidth taken by the video scaler is:
  //  source_hres*source_vres*refresh rate*(2 bytes)
  //However, if the soruce width is greater than can be stored
  //in the video scaler's FIFOs, then two source lines are fetched
  //for each destination line.
  //Also, for large downscaling greater than or equal to 2x, two
  //source lines are fetched for each destination line.
  if (((scaler->source_hres*scaler->depth/8) > scaler->max_fifo_size) || (scaler->vscale <= 500))
    scaler->bwFactor = 2 * scaler->vscale;
  else
    scaler->bwFactor = 1;

  scaler->burst_size = burst_size;
  scaler->min_burst_size = min_burst_size;
  scaler->max_burst_size = max_burst_size;
  scaler->request_delay = request_delay_pclks * scaler->pclk_period + request_delay_nvclks * (1000000 / nvclk);
  /* note that we never read more than the source image,
     even if we're upscaling. that's why we take the MIN
     of 1 and hscale. */
  //scaler->drain_rate = ((scaler->depth / 8)*100000) / MIN(scaler->hscale, 1000);
  scaler->drain_rate = ((scaler->depth / 8)*100000) / ((scaler->hscale < 1000) ? scaler->hscale:1000);
  if ((scaler->vscale < 1000) || 
      ((max_fifo_size != 0) && 
       ((scaler->source_hres*(scaler->depth/8)) > max_fifo_size)
      ) 
     )
    /* we're going to decimate, so we may read 2 new lines for an output line */
    scaler->drain_rate *= 2;
  /* check for a legitimate burst size */
  check_scaler_burstsize(scaler);
  scaler->fifo.size = 0;
  scaler->fifo.lwm  = 0;
}

static void
init_mem(mem_t *mem,
     U032 mclk,
     U032 ddr,
     arb_type arbiter,
     U032 partitions,
     U032 partition_data_width,
     U032 page_fault_penalty,
     U032 page_fault_width,
     U032 refresh_penalty,
     U032 arbitration_latency,
     U032 pipeline_latency,
     U032 stall_cycles,
     U032 depth_cmd,
     U032 depth_rw,
     U032 interrupt_rate
     )
{
  mem->mclk = mclk;
  mem->mclk_period = 1000000 / mclk;
  mem->partitions = partitions;
  mem->ddr = ddr;
  mem->arbiter = arbiter;
  mem->data_rate = ((ddr) ? 2 : 1) * partitions * partition_data_width / 8;
  mem->page_fault_penalty = page_fault_penalty;
  mem->page_fault_width = page_fault_width;
  mem->refresh_penalty = refresh_penalty;
  mem->arbitration_latency = arbitration_latency;
  mem->pipeline_latency = pipeline_latency;
  mem->stall_cycles = stall_cycles;
  mem->cmdq_depth_cmd = depth_cmd;
  mem->cmdq_depth_rw = depth_rw;
  /* the first number is the amount of time taken to process all the read/write
     commands; the second is to allow for bus turnaround from read->write.
     the third and fourth allow for two page faults and a refresh in the
     command queue. */
//the command_latency is included in the pipeline_latency of the FBI.
//mem->command_latency = depth_rw * ((ddr) ? 2 : 4) + (depth_rw / 2) + (depth_cmd - 1) * page_fault_penalty + refresh_penalty;
  mem->command_latency = 0;
  mem->interrupt_rate = interrupt_rate;
}

static void
init_rtr(rtr_t *rtr,
     U032 burst_size,
     U032 request_delay_mclks,
     mem_t *mem
     )
{
  rtr->burst_size = burst_size;
  rtr->request_delay_mclks = request_delay_mclks;
  rtr->request_delay = request_delay_mclks * mem->mclk_period;
  rtr->latency = 0;
}


// Actual buffer size calculation routines
static void
guess_scaler_burstsize(scaler_t *scaler, crtc_t *crtc, crtc_t *crtc2)
{
  U032 raw, raw1, raw2;
  
  /* take a simplistic approach to guess this */

  raw1 = ((scaler->drain_rate*1000)/crtc->drain_rate )*crtc->burst_size;
  raw2 = ((scaler->drain_rate*1000)/crtc2->drain_rate)*crtc2->burst_size;
  raw  = (scaler->h2_owns_vs) ? raw2/1000 : raw1/1000;
 
  if (raw < scaler->min_burst_size) raw = scaler->min_burst_size;
  scaler->burst_size = adjust_scaler_burstsize(raw, scaler->min_burst_size, scaler->max_burst_size);
}

static U032
adjust_crtc2_burstsize(U032 raw, U032 min, U032 max)
{
  U032 adj;
  
  /* scaler burst sizes are a multiple of min bytes */
  adj = (raw + (min - 1)) / min * min;
  /* max burst size is 2048 bytes */
  if (adj > max)
    adj = max;  
  return adj;
}
    
static void
guess_crtc2_burstsize(crtc_t *crtc2, crtc_t *crtc)
{
  U032 raw;
  U032 period_factorx100, drain_factorx100;
  /* take a simplistic approach to guess this */
  period_factorx100 = (100*crtc->pclk_period)/crtc2->pclk_period;
  drain_factorx100  = (100*crtc2->drain_rate)/crtc->drain_rate;
  raw = (crtc->burst_size*period_factorx100*drain_factorx100)/10000;
  if (raw < crtc2->min_burst_size) raw = crtc2->min_burst_size;
  crtc2->burst_size = adjust_crtc2_burstsize(raw, crtc2->min_burst_size, crtc->max_burst_size);
}

/************************************************/
static U032
compute_fifo_size(fifo_params *fifo, U032 hres, U032 drain_rate, U032 burst_size, U032 request_delay, U032 dclk_period,
          U032 burst_latency, U032 bursts_during_active, mem_t *mem)
{
  U032 lwm, lwm_min;
  U032 fifo_size;
  U032 bursts_to_fill_lwm;
  
  /* figure out the low water mark in the fifo */
  if (((hres * drain_rate)/100) > (bursts_during_active * burst_size))
    lwm = (hres * drain_rate)/100 - bursts_during_active * burst_size;
  else
    lwm = 0;

  /* make sure we have enough data in the fifo to satisfy the
     initial wait in the active phase */
//LPL need:  NVASSERT((request_delay + mem->pipeline_latency + mem->command_latency + burst_latency - burst_size) >= 0);
  lwm_min = ((((request_delay + mem->pipeline_latency + mem->command_latency + burst_latency - burst_size)
      * mem->mclk_period) / mem->data_rate) * (drain_rate / dclk_period)
      +99)/100; //forces rounding up, needed because drain_rateis 100x

  //LPL: Eep!  How many loops are possible here?
  while (lwm < lwm_min) {
    lwm += burst_size;
    bursts_during_active--;
  }

  /*added because bursts during active could be more than necessary to fetch a line*/
  bursts_to_fill_lwm = lwm/burst_size;
  if (lwm%burst_size)
    bursts_to_fill_lwm++;

  while (((bursts_to_fill_lwm + bursts_during_active - 1)*burst_size) >= ((hres*drain_rate)/100)) {
    bursts_during_active--;
  }
  
  /* fifo_size is lwm + burst_size */
  fifo_size = lwm + burst_size;

  fifo->lwm_min = lwm_min;
  fifo->lwm     = lwm;
  fifo->size    = fifo_size;

  return bursts_during_active;
}

/************************************************/
/* check to see that we are consuming sufficient bandwidth. this
   is really just an approximation since it only really considers
   the bandwidth consumed by the crtc and scaler. */
static U032
not_consuming_sufficient_bandwidth(crtc_t *crtc, crtc_t *crtc2, scaler_t *scaler, mem_t *mem, U032 burst_latency, U032 cursor_latency)
{
  U032 reqd_crtc_bw, reqd_crtc2_bw, reqd_scaler_bw;
  U032 actual_bw;
  U032 error;

  //LPL is this if needed? What's the correct return in case of error?
  //LPL Range check.
  if ((crtc->hclk_period > cursor_latency/1000) && (crtc2->hclk_period > cursor_latency/1000)) {
      reqd_crtc_bw   = (100*crtc->enable)  *crtc->hres *(crtc->depth /8) / (crtc->hclk_period  - cursor_latency/1000);
      reqd_crtc2_bw  = (100*crtc2->enable) *crtc2->hres*(crtc2->depth/8) / (crtc2->hclk_period - cursor_latency/1000);
      reqd_scaler_bw = (100*scaler->enable)*scaler->source_hres * (scaler->depth / 8) * ((scaler->vscale <= 500) ? 2 : 1) / 
                       (crtc->hclk_period - cursor_latency/1000);
      actual_bw = (1000*100*(crtc->enable*crtc->burst_size + crtc2->enable*crtc2->burst_size + scaler->enable*scaler->burst_size))
                  / (burst_latency * mem->mclk_period); //x1000000 because other bw as bytes/ns, not bytes/ps
    error = (actual_bw < (reqd_crtc_bw + reqd_crtc2_bw + reqd_scaler_bw)) ? 1 : 0;
  } else {
      error = 0;    //LPL: proper failure?
  }
//  if (actual_bw < 0) error = 0;//overflow in weird cases
  return error;
}

/*makes sure fifo size isn't larger than the maximum*/
static U032 test_fifo_constraints(U032 fifo_size, crtc_t *crtc, crtc_t *crtc2, scaler_t *scaler) 
{
  /* If dm ("Display Matrix") isn't specified at the command line, dm
   * (fifo_size here) will be 0.  In this case, no error will be returned since
   * any fifo size is OK.
   */
  if (fifo_size == 0)
    return (0);

  /* Note that fifo.size is initialized to zero for the CRTCs and scaler.
   * So, this function will not return an error just because a fifo size has 
   * yet to be computed.
   */
  return (( crtc->enable  && (crtc->fifo.size   > fifo_size))             || 
          ( crtc2->enable && (crtc2->fifo.size  > fifo_size))             || 
          (scaler->enable && (scaler->fifo.size > scaler->max_fifo_size))   );
}

/* check that there is sufficient total bandwidth available to
   support this resolution. */
static U032
insufficient_total_bandwidth(crtc_t *crtc, crtc_t *crtc2, scaler_t *scaler, mem_t *mem)
{
  U032 error;
  U032 video_bandwidth;
  U032 available_bandwidth;

  //nativeVRes is used in place of VRes because, for flat panels, one line is read from memory for each
  //destination line, not each source line.  For non-flat panels (i.e., the FP scaler is not used),
  //nativeVRes should be the same as vres.
  video_bandwidth = (crtc->enable  *(crtc->hres  * crtc->nativeVRes  * crtc->depth  / 8 ) *  crtc->refresh / 10 + 
                     crtc2->enable *(crtc2->hres * crtc2->nativeVRes * crtc2->depth / 8 ) * crtc2->refresh / 10 + 
                     scaler->enable*(scaler->source_hres * scaler->source_vres * 2) * scaler->bwFactor *
                       ((scaler->h2_owns_vs)?crtc2->refresh : crtc->refresh) / 10           );

  //LPL: this overflowed before, but switch to /10 and U032 should
  // give us plenty of breathing room
  available_bandwidth = mem->data_rate * mem->mclk * 90000;
  error = (video_bandwidth > available_bandwidth) ? 1 : 0;
  return error;
}

/* check to see that we can read sufficient data during active time assuming LWM is already reached*/
/* ASSUMES that LWM can be filled during blank.  That is, this function should always be used with 
   cant_fill_to_lwm.  Also assumes that bursts_during_active actually fits in active. */
static U032
insufficient_data(U032 hres, U032 depth, U032 lwm, U032 bursts_during_active, U032 burst_size)
{
  U032 error;
  error = ((hres * depth / 8) > 
           ((lwm + burst_size - 1) / burst_size + bursts_during_active) * burst_size
          ) ? 1 : 0;
  return error;
}


/**************************************************/
/*use for NV11, NV20 and onward*/
/*should also be used for scaler NV10 onward*/
static U032
new_cant_fill_to_lwm(crtc_t *crtc, mem_t *mem, U032 burst_latency, U032 cursor_latency, U032 bursts_during_active)
{
    U032 error;
    U032 bursts_during_available;

#if 1//def TWINHEAD_OPTS
    U032 temp;
    temp = (bursts_during_active * burst_latency + cursor_latency + mem->pipeline_latency)
        * mem->mclk_period + crtc->request_delay;
    if ((crtc->hclk_period * 1000) >= temp) {
        bursts_during_available = (crtc->hclk_period * 1000 - temp)
            / (burst_latency * mem->mclk_period);
        error = (crtc->fifo.lwm > bursts_during_available * crtc->burst_size) ? 1 : 0;
    } else {
        error = 1;  //LPL: proper error?
    }
#else    //#ifdef TWINHEAD_OPTS
  bursts_during_available = ((crtc->hclk_period * 1000 -
                  bursts_during_active * burst_latency * mem.mclk_period -
                  cursor_latency                       * mem.mclk_period -
                  mem.pipeline_latency                 * mem.mclk_period -
                  crtc->request_delay
                 ) /
                 (burst_latency * mem.mclk_period)
                );
  error = (crtc->fifo.lwm > bursts_during_available * crtc->burst_size) ? 1 : 0;
#endif    //#ifdef TWINHEAD_OPTS

  return error;
}

static U032
scaler_cant_fill_to_lwm(scaler_t *scaler, crtc_t *crtc, crtc_t *crtc2, mem_t *mem, U032 burst_latency, U032 cursor_latency, U032 bursts_during_active)
{
    U032 error;
    U032 bursts_during_available;
    U032 hclk_period;

#if 1//def TWINHEAD_OPTS
    U032 temp;

    if (scaler->h2_owns_vs) hclk_period = crtc2->hclk_period;
    else hclk_period = crtc->hclk_period;

    temp = (bursts_during_active * burst_latency + cursor_latency + mem->pipeline_latency) * mem->mclk_period
        + scaler->request_delay;

    if ((hclk_period * 1000) >= temp) {
        bursts_during_available = (hclk_period * 1000 - temp)
            / (burst_latency * mem->mclk_period);
        error = (scaler->fifo.lwm > bursts_during_available * scaler->burst_size) ? 1 : 0;
    } else {
        error = 0;  //LPL proper error?
    }
#else    //#ifdef TWINHEAD_OPTS
  if (scaler->h2_owns_vs)
  {
    bursts_during_available = ((crtc2->hclk_period * 1000 -
                      bursts_during_active * burst_latency * mem.mclk_period -
                    cursor_latency                       * mem.mclk_period -
                    mem.pipeline_latency                 * mem.mclk_period -
                    scaler->request_delay
                   ) /
                   (burst_latency * mem.mclk_period)
                  );
  } else
  {
    bursts_during_available = ((crtc->hclk_period * 1000  -
                      bursts_during_active * burst_latency * mem.mclk_period -
                    cursor_latency                       * mem.mclk_period -
                    mem.pipeline_latency                 * mem.mclk_period -
                    crtc->request_delay
                   ) /
                   (burst_latency * mem.mclk_period)
                  );
  }
  error = (scaler->fifo.lwm > bursts_during_available * scaler->burst_size) ? 1 : 0;
#endif    //#ifdef TWINHEAD_OPTS

  return error;
}

/***********************************************/
/*Note: this function is analogous to test_scaler_constraints.  
  Any modifications here will often also need to be made to 
  test_scaler_constraints.
  "someday" the crtc_t and scaler_t data types should be unified so these
  two functions can also be unified.  OOP would have been nice here. . .
  */
static U032
test_crtc_constraints(crtc_t *crtc, crtc_t *crtc2, scaler_t *scaler, rtr_t *rtr, mem_t *mem)
{
  U032 errors = 0;
  U032 burst_latency;
  U032 cursor_latency;
  U032 bursts_during_active;
  U032 final_bursts_during_active;
  U032 num_rtrs, total_burst_size;
  U032 burst_temp, burst_temp2;

  if (!crtc->enable) return 0;
 
  num_rtrs         = crtc->enable + crtc2->enable + scaler->enable;
  total_burst_size =  crtc->enable*crtc->burst_size + crtc2->enable*crtc2->burst_size + scaler->enable*scaler->burst_size;

  burst_latency = num_rtrs         * mem->arbitration_latency + 
                  total_burst_size / mem->data_rate           +
                  num_rtrs         * mem->stall_cycles        +
                  (crtc->enable    * ((crtc->burst_size   + mem->page_fault_width - 1) / mem->page_fault_width) +
                   crtc2->enable   * ((crtc2->burst_size  + mem->page_fault_width - 1) / mem->page_fault_width) +
                   scaler->enable  * ((scaler->burst_size + mem->page_fault_width - 1) / mem->page_fault_width)
                  ) * mem->page_fault_penalty;

  
  /*note that cursor_latency is a on a per-CRTC basis, not total cusor_latency.*/
  /*note also that NV11 needs to treat cursor latency as taking a CRTC burst.  */
  cursor_latency = mem->arbitration_latency + (crtc->cursor_burst_size) / mem->data_rate + mem->stall_cycles;
  if (not_consuming_sufficient_bandwidth(crtc, crtc2, scaler, mem, burst_latency, cursor_latency * mem->mclk_period)) {
    errors++;
  }
  if (errors)
    return errors;

  /* optimistic case */
#if 1
  burst_temp = crtc->hres * crtc->pclk_period + crtc->burst_size * mem->mclk_period / mem->data_rate;
  burst_temp2 =  crtc->request_delay
      + (mem->pipeline_latency + mem->command_latency) * mem->mclk_period
      + 100 * crtc->burst_size * crtc->pclk_period / crtc->drain_rate;

  if (burst_temp >= burst_temp2) {
      bursts_during_active =  (burst_temp - burst_temp2) / (burst_latency * mem->mclk_period);
  } else {
      bursts_during_active = 0;
  }

#else
  bursts_during_active = (crtc->hres           * crtc->pclk_period -
                  crtc->request_delay                      -
                  mem->pipeline_latency * mem->mclk_period   -
                  mem->command_latency  * mem->mclk_period   -
                  100 * crtc->burst_size     * crtc->pclk_period / crtc->drain_rate +
                  crtc->burst_size     * mem->mclk_period   / mem->data_rate
                 ) /
                 (burst_latency * mem->mclk_period);
#endif

  final_bursts_during_active = compute_fifo_size(&crtc->fifo, crtc->hres, crtc->drain_rate, crtc->burst_size,
                         crtc->request_delay, crtc->pclk_period, burst_latency, bursts_during_active,
                         mem);

  /* can we fill to the LWM mark in the blanking period?
     note that we have to wait for the cursor burst to go first.
     for "new" mode operation, we can also start filling during
     the previous scan-out. */
  if (new_cant_fill_to_lwm(crtc, mem, burst_latency, (crtc->enable+crtc2->enable)*cursor_latency, final_bursts_during_active)) {
    errors++;
  }
  
  /* can we get enough data? */
  if (insufficient_data(crtc->hres, crtc->depth, crtc->fifo.lwm, final_bursts_during_active, crtc->burst_size)) {
    errors++;
  }

  if (errors) return errors;

  /* pessimistic case -- if this is the pessimistic case, why even try the previous case?*/
  //burst_latency += MAX(((100*crtc->burst_size * crtc->pclk_period / crtc->drain_rate / mem.mclk_period) - burst_latency, 0);
  burst_temp = 100 * crtc->burst_size * crtc->pclk_period / crtc->drain_rate / mem->mclk_period;
  if (burst_temp > burst_latency)
      burst_latency = burst_temp;

#if 1
    burst_temp = crtc->hres * crtc->pclk_period + crtc->burst_size * mem->mclk_period / mem->data_rate;
    burst_temp2 = crtc->request_delay
        + (mem->pipeline_latency + mem->command_latency) * mem->mclk_period
        + 100 * crtc->burst_size * crtc->pclk_period / crtc->drain_rate;

    if (burst_temp >= burst_temp2) {
        bursts_during_active = (burst_temp - burst_temp2) / (burst_latency * mem->mclk_period);
    } else {
      bursts_during_active = 0;
    }
#else
    bursts_during_active = (crtc->hres * crtc->pclk_period -
              crtc->request_delay -
              mem->pipeline_latency * mem->mclk_period -
              mem->command_latency * mem->mclk_period -
              100*crtc->burst_size * crtc->pclk_period / crtc->drain_rate +
              crtc->burst_size * mem->mclk_period / mem->data_rate
              ) /
             (burst_latency * mem->mclk_period);
#endif

  final_bursts_during_active = compute_fifo_size(&crtc->fifo, crtc->hres, crtc->drain_rate, crtc->burst_size,
                       crtc->request_delay, crtc->pclk_period, burst_latency, bursts_during_active,
                         mem);

  /* can we fill to the LWM mark in the blanking period? */
  if (new_cant_fill_to_lwm(crtc, mem, burst_latency, (crtc->enable+crtc2->enable)*cursor_latency, final_bursts_during_active)) {
    errors++;
  }
  
  /* can we get enough data? */
  if (insufficient_data(crtc->hres, crtc->depth, crtc->fifo.lwm, final_bursts_during_active, crtc->burst_size)) {
    errors++;
  }

  return errors;
}

/***********************************************/
static U032
test_scaler_constraints(scaler_t *scaler, crtc_t *crtc, crtc_t *crtc2, rtr_t *rtr, mem_t *mem)
{
  U032 errors = 0;
  U032 burst_latency;
  U032 bursts_during_active;
  U032 final_bursts_during_active;
  U032 num_rtrs, total_burst_size;
  U032 burst_temp, burst_temp2;
      
  if (!scaler->enable) return 0;

  /*intermediate values used to calculate burst_latency*/
  num_rtrs         = crtc->enable + crtc2->enable + scaler->enable;
  total_burst_size = crtc->enable*crtc->burst_size + crtc2->enable*crtc2->burst_size + scaler->enable*scaler->burst_size;

  burst_latency = num_rtrs         * mem->arbitration_latency + 
                  total_burst_size / mem->data_rate           +
                  num_rtrs         * mem->stall_cycles        +
                  (crtc->enable   * ((crtc->burst_size  +mem->page_fault_width-1) / mem->page_fault_width) +
                   crtc2->enable  * ((crtc2->burst_size +mem->page_fault_width-1) / mem->page_fault_width) +
                   scaler->enable * ((scaler->burst_size+mem->page_fault_width-1) / mem->page_fault_width)
                  ) * mem->page_fault_penalty;

  
  /* optimistic case */
#if 1
  burst_temp = scaler->window_hres * scaler->pclk_period + scaler->burst_size * mem->mclk_period / mem->data_rate;
  burst_temp2 = scaler->request_delay
      + (mem->pipeline_latency + mem->command_latency) * mem->mclk_period
      + 100 * scaler->burst_size * scaler->pclk_period / scaler->drain_rate;

    if (burst_temp >= burst_temp2) {
        bursts_during_active = (burst_temp - burst_temp2) / (burst_latency * mem->mclk_period);
    } else {
      bursts_during_active = 0;
    }
#else
  bursts_during_active = (scaler->window_hres  * scaler->pclk_period -
                 scaler->request_delay -
                 mem->pipeline_latency * mem->mclk_period -
                 mem->command_latency  * mem->mclk_period -
                 100*scaler->burst_size   * scaler->pclk_period / scaler->drain_rate +
                 scaler->burst_size   * mem->mclk_period / mem->data_rate
                 ) /
                (burst_latency * mem->mclk_period);
#endif

  final_bursts_during_active = compute_fifo_size(&scaler->fifo, scaler->window_hres, scaler->drain_rate, scaler->burst_size,
               scaler->request_delay, scaler->pclk_period, burst_latency, bursts_during_active,
               mem);

  if (scaler_cant_fill_to_lwm(scaler, crtc, crtc2, mem, burst_latency, 0, final_bursts_during_active)) {
    errors++;
  }
  
  /* can we get enough data? */
  if (insufficient_data(scaler->source_hres, scaler->depth, scaler->fifo.lwm, final_bursts_during_active, scaler->burst_size)) {
    errors++;
  }

  if (errors) return errors;

  /* pessimistic case */
  //burst_latency += MAX((100* scaler->burst_size * scaler->pclk_period / scaler->drain_rate / mem.mclk_period) - burst_latency, 0);
  burst_temp = 100* scaler->burst_size * scaler->pclk_period / scaler->drain_rate / mem->mclk_period;
  if (burst_temp > burst_latency)
      burst_latency = burst_temp;

#if 1
  burst_temp = scaler->window_hres * scaler->pclk_period + scaler->burst_size * mem->mclk_period / mem->data_rate;
  burst_temp2 = scaler->request_delay
      + (mem->pipeline_latency + mem->command_latency) * mem->mclk_period
      + 100 * scaler->burst_size * scaler->pclk_period / scaler->drain_rate;

    if (burst_temp >= burst_temp2) {
        bursts_during_active = (burst_temp - burst_temp2) / (burst_latency * mem->mclk_period);
    } else {
      bursts_during_active = 0;
    }
#else
  bursts_during_active = (scaler->window_hres  * scaler->pclk_period -
                 scaler->request_delay -
                 mem->pipeline_latency * mem->mclk_period -
                 mem->command_latency  * mem->mclk_period -
                 100* scaler->burst_size   * scaler->pclk_period / scaler->drain_rate +
                 scaler->burst_size   * mem->mclk_period / mem->data_rate
                 ) /
                (burst_latency * mem->mclk_period);
#endif

  final_bursts_during_active = compute_fifo_size(&scaler->fifo, scaler->window_hres, scaler->drain_rate, scaler->burst_size,
               scaler->request_delay, scaler->pclk_period, burst_latency, bursts_during_active,
               mem);

  /* can we fill to the LWM mark in the blanking period? */
  if (scaler_cant_fill_to_lwm(scaler, crtc, crtc2, mem, burst_latency, 0, final_bursts_during_active)) {
    errors++;
  }
  
  /* can we get enough data? */
  if (insufficient_data(scaler->source_hres, scaler->depth, scaler->fifo.lwm, final_bursts_during_active, scaler->burst_size)) {
    errors++;
  }

  return errors;
}

/******************************/
static U032
test_configuration(crtc_t *crtc, crtc_t *crtc2, scaler_t *scaler, rtr_t *rtr, U032 max_burst_size, mem_t *mem, U032 fifo_size)
{
  U032 error;

  error = insufficient_total_bandwidth(crtc, crtc2, scaler, mem);
  if (error) {
    return error;
  }

  for (crtc->burst_size = crtc->min_burst_size; crtc->burst_size <= max_burst_size; crtc->burst_size += 32) {

    error = 0;

    if (scaler->enable)
    guess_scaler_burstsize(scaler, crtc, crtc2);
    if (crtc2->enable)
    guess_crtc2_burstsize(crtc2, crtc);

    error = test_crtc_constraints(crtc, crtc2, scaler, rtr, mem);
    if (error)
      continue;
    if (crtc2->enable) {
      error = test_crtc_constraints(crtc2, crtc, scaler, rtr, mem);
      if (error)
        continue;
    }
    if (test_scaler_constraints(scaler, crtc, crtc2, rtr, mem))
      error++;
    if (scaler->burst_size > max_burst_size)
    error++;

    if (test_fifo_constraints(fifo_size, crtc, crtc2, scaler))
      error++;
    if (!error) 
      break;

     if (scaler->enable) {
    for (scaler->burst_size += scaler->min_burst_size; scaler->burst_size <= max_burst_size; scaler->burst_size += 32) {
        if (test_scaler_constraints(scaler, crtc, crtc2, rtr, mem))
          error = 1;
        if (test_fifo_constraints(fifo_size, crtc, crtc2, scaler))
          error++;
      if (!error) break;
    }
    if (!error) {
      if (test_crtc_constraints(crtc, crtc2, scaler, rtr, mem))
            error = 1;
      if (crtc2->enable && test_crtc_constraints(crtc2, crtc, scaler, rtr, mem))
            error++;
    }
    }
    if (test_fifo_constraints(fifo_size, crtc, crtc2, scaler))
      error++;
    if (!error) break;
  }

  return(error);
}

void
NV20_dacCalculateArbitration
(
    PHALHWINFO pHalHwInfo,
    fifo_info *fifo,
    sim_state *arb
)
{
    crtc_t crtc;
    crtc_t crtc2;
    scaler_t scaler;
    rtr_t rtr;
    mem_t mem;
//  U032 i;
//  U032 maxd, maxr;
    U032 error;
    U032 data_in_line1, data_in_line2, data_in_lineS;
 
    /*NV20 default values*/
    U032 page_fault_penalty   = 0;
    U032 page_fault_width     = 256;
    U032 arbitration_latency  = 34;
    U032 pipeline_latency     = 40;
    U032 stall_cycles         = 10;

    U032 min_burst_size   =   32;
    U032 max_burst_size   = 4096;
    U032 crtc_fifo_size   = 4096; /*CRTC fifo size in bytes*/
    U032 scaler_fifo_size = 4096; /*video scaler's fifo size*/
 
    U032 partition_data_width;
    U032 ddr;
 
    /* set up some default values for the CRTC ... */
    U032 hres    ;
    U032 htotal  ;
    U032 hres2   ;
    U032 htotal2 ;
    U032 vres    ;
    U032 vres2   ;
    U032 nativeVRes;
    U032 nativeVRes2;
    U032 refresh ;
    U032 refresh2;
    U032 depth   ;
    U032 depth2  ;
    U032 crtc_request_delay_mclks = 6;
    U032 crtc_request_delay_pclks = 6;
    U032 crtc_burst_size = 0;
    U032 cursor_burst_size = 256;
    U032 crtc2_enable, crtc_enable;
    U032 video_source_hres;
    U032 video_source_vres;
    U032 h2_owns_vs  ;
    U032 mclk;
    U032 pclk, pclk2;
 
    /* ... the scaler ... */
    U032 scaler_disable;
    U032 scaler_depth = 16;
    U032 scaler_burst_size = 0;
    /*Same as FBI interface, in bytes*/
    U032 scaler_window_hres = 0;
    U032 scaler_window_vres = 0;
    U032 scaler_request_delay_mclks = 4;
    U032 scaler_request_delay_pclks = 6;
  
    /* ... the memory controller ... */
    U032 partitions = 1;
    U032 refresh_penalty = 9;
    arb_type arbiter = round_robin;
    U032 depth_cmd = 3;
    U032 depth_rw = 8;
    U032 rtr_interrupt_rate = 0;
  
    /* ... and the real-time requestor */
    /* to disable this, just leave the burst_size as 0 */
    U032 rtr_request_delay_mclks = 4;
    U032 rtr_burst_size = 0;

    U032 who_owns_scaler;

    crtc_enable       = arb->enable_h1;
    crtc2_enable      = arb->enable_h2;
    scaler_disable    = !(arb->enable_video);
    video_source_hres = arb->video_source_hres;
    video_source_vres = arb->video_source_vres;
    h2_owns_vs        = arb->h2_owns_vs;

    hres         = arb->hres;
    htotal       = arb->htotal;
    vres         = arb->vres;
    depth        = arb->pix_bpp;
    refresh      = arb->refresh;
    pclk         = arb->pclk_khz / 1000;
    nativeVRes   = arb->nativeVRes;

    hres2        = arb->hres2;
    htotal2      = arb->htotal2;
    vres2        = arb->vres2;
    depth2       = arb->pix_bpp_h2;
    refresh2     = arb->refresh2;
    pclk2        = arb->pclk2_khz / 1000;
    nativeVRes2  = arb->nativeVRes2;

    partition_data_width = arb->memory_width;
    ddr                  = arb->memory_type;
    mclk                 = arb->mclk_khz / 1000;

    init_mem(&mem, mclk, ddr, arbiter, partitions, partition_data_width,
             page_fault_penalty, page_fault_width, refresh_penalty,
             arbitration_latency, pipeline_latency, stall_cycles,
             depth_cmd, depth_rw, rtr_interrupt_rate);
    init_crtc(&crtc, crtc_enable,
              hres, vres, nativeVRes, depth, refresh,
              crtc_request_delay_mclks, crtc_request_delay_pclks,
              crtc_burst_size, min_burst_size, max_burst_size,
              cursor_burst_size, pclk, htotal, &mem);
    init_crtc(&crtc2, crtc2_enable,
              hres2, vres2, nativeVRes2, depth2, refresh2,
              crtc_request_delay_mclks, crtc_request_delay_pclks,
              crtc_burst_size, min_burst_size, max_burst_size,
              cursor_burst_size, pclk2, htotal2, &mem);
    init_scaler(&scaler, &crtc, &crtc2, mclk,
                (scaler_disable) ? 0 : 1,
                video_source_hres, video_source_vres, h2_owns_vs, scaler_depth,
                scaler_window_hres, scaler_window_vres,
                scaler_burst_size, min_burst_size, max_burst_size,
                scaler_request_delay_mclks, scaler_request_delay_pclks,
                scaler_fifo_size);
    init_rtr(&rtr, rtr_burst_size, rtr_request_delay_mclks, &mem);

//  if ((hres2*depth2*1000/crtc2.hclk_period) > (hres*depth*1000/crtc.hclk_period)) {
    //For the purposes of this calc, assign scaler to head with worst bandwidth constraints.
    who_owns_scaler = 0;
    if (crtc2.enable && crtc.enable) {
        if ((hres2*depth2*1000/crtc2.hclk_period) > (hres*depth*1000/crtc.hclk_period))
            who_owns_scaler = 1;
    } else {
        if (crtc2.enable)
            who_owns_scaler = 1;
    }

    if (!crtc.enable) {
        /*swap which head scaler is owned by when swapping heads*/
        scaler.h2_owns_vs = !scaler.h2_owns_vs;
        error = test_configuration(&crtc2, &crtc,  &scaler, &rtr,
                                   max_burst_size, &mem, crtc_fifo_size); 
        scaler.h2_owns_vs = !scaler.h2_owns_vs;
    } else if (!crtc2.enable) {
        error = test_configuration(&crtc,  &crtc2, &scaler, &rtr,
                                   max_burst_size, &mem, crtc_fifo_size); 
    } else if (who_owns_scaler) {
        /*swap which head scaler is owned by when swapping heads*/
        scaler.h2_owns_vs = !scaler.h2_owns_vs;
        error = test_configuration(&crtc2, &crtc,  &scaler, &rtr,
                                   max_burst_size, &mem, crtc_fifo_size); 
        scaler.h2_owns_vs = !scaler.h2_owns_vs;
    } else
        error = test_configuration(&crtc,  &crtc2, &scaler, &rtr,
                                   max_burst_size, &mem, crtc_fifo_size); 


    //Compute the amount of data in a single horizontal line
    data_in_line1 = crtc.hres          * crtc.depth   / 8;
    data_in_line2 = crtc2.hres         * crtc2.depth  / 8; 
    data_in_lineS = scaler.source_hres * scaler.depth / 8;

    //It is really inefficent to have a burst size smaller than 256 bytes.
    if (crtc.burst_size   < 256)
        crtc.burst_size   = 256;
    if (crtc2.burst_size  < 256)
        crtc2.burst_size  = 256;
    if (scaler.burst_size < 256)
        scaler.burst_size = 256;

    //It is wasteful to have a burst size larger than the amount of data in a line
    if (crtc.burst_size   > data_in_line1)
        crtc.burst_size   = data_in_line1;
    if (crtc2.burst_size  > data_in_line2)
        crtc2.burst_size  = data_in_line2;
    if (scaler.burst_size > data_in_lineS)
        scaler.burst_size = data_in_lineS;


    //LPL: addition to set watermarks as high as possible instead
    // of as low as possible.  This presumably obviates the need
    // for some other calculation effort earler.

    //Get best performance for the real-time requestors by setting LWM
    //as high as possible without risking overflow.
    crtc.fifo.lwm   = crtc_fifo_size   - crtc.burst_size  ;
    crtc2.fifo.lwm  = crtc_fifo_size   - crtc2.burst_size ;
    scaler.fifo.lwm = scaler_fifo_size - scaler.burst_size;

    //LWM should not be larger than the amount of data in a line
    if (crtc.fifo.lwm > data_in_line1)
        crtc.fifo.lwm = data_in_line1;
    if (crtc2.fifo.lwm > data_in_line2)
        crtc2.fifo.lwm = data_in_line2;
    if (scaler.fifo.lwm > data_in_lineS)
        scaler.fifo.lwm = data_in_lineS;
 
    fifo->valid = !error;
    fifo->crtc1_lwm = crtc.fifo.lwm;
    fifo->crtc2_lwm = crtc2.fifo.lwm;
    fifo->video_lwm = scaler.fifo.lwm;
    fifo->crtc1_burst_size = crtc.burst_size;
    fifo->crtc2_burst_size = crtc2.burst_size;
    fifo->video_burst_size = scaler.burst_size;
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
RM_STATUS nvHalDacValidateBandwidth_NV20(VOID *arg);
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
        case DAC_CONTROL_DITHER_DISABLE:
            FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_DITHER_2D, _DISABLED);
            FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_DITHER_2D, _DISABLED);
            break;
        case DAC_CONTROL_DITHER_ENABLE:
            FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_DITHER_2D, _ENABLED);
            FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_DITHER_2D, _ENABLED);
            break;
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
    U032 oldMClkScaled;
    U032 newMClkScaled;
    U032 oldTiming2Refresh;
    U032 newTiming2Refresh;
    U032 Loops;
    U032 IsPllLocked;
    U032 Emrs;
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

    // Wait for M PLL to lock.
    osDelay(64 /* ms */);

    // Reset EMRS.
    Emrs = REG_RD32(NV_PFB_EMRS);
    Emrs |= DRF_DEF(_PFB, _EMRS, _BA0, _1);
    REG_WR32(NV_PFB_EMRS, Emrs);

    // Reset (resync) the frame buffer DRAM DLL.
    FLD_WR_DRF_DEF(_PFB, _MRS, _A8, _1);

    // Update the number of clock cycles per memory refresh cycle.
    
    // Determine original frequency from old M, N, P values, 
    // scale down both old and new to avoid 32-bit overflow later.  
    // NV_PFB_TIMING2_REFRESH is max 11 bits, mclk is max 30 bits.
    
    oldMClkScaled  = (nOld * pMcHalInfo->CrystalFreq / (1 << pOld) / mOld) >> (11 + 30 - 32);
    
    newMClkScaled  = pDacHalInfo->MClk >> (11 + 30 - 32);

    if (oldMClkScaled != newMClkScaled)
    {
        // Scale the refresh-delay by the ratio between new and old frequencies.
        // Be careful to round rather than truncate.
        //
        // NOTE: we are accumulating round-off errors in NV_PFB_TIMING2_REFRESH.
        //       Over many changes of mclk, the refresh interval may drift up or down.
        //       We really should be calculating from the initial bios refresh and
        //       mclk each time, but we don't have that data cached away anywhere.
        
        oldTiming2Refresh = REG_RD_DRF( _PFB, _TIMING2, _REFRESH );
    
        newTiming2Refresh = (oldTiming2Refresh * newMClkScaled + oldMClkScaled/2) / oldMClkScaled;
        
        DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "old cycles per refresh=", oldTiming2Refresh*32);
        DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "new cycles per refresh=", newTiming2Refresh*32);
        
        // Store the new value
        FLD_WR_DRF_NUM( _PFB, _TIMING2, _REFRESH, newTiming2Refresh );
    }       
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
        case NV207C_SET_CURSOR_IMAGE_FORMAT_COLOR_XOR_LE_A8R8G8B8:
            // set bpp = 1, blend already 0 (only supported in NV15 or later)
            cursorConfig |= (NV_PCRTC_CURSOR_CONFIG_CUR_BPP_32 << DRF_SHIFT(NV_PCRTC_CURSOR_CONFIG_CUR_BPP));
            break;
        default:
            // Leave blend and bpp both 0 (ROP1R5G5B5)
            break;
    }

    // write the new cursor config register value.
    HAL_DAC_REG_WR32(NV_PCRTC_CURSOR_CONFIG, cursorConfig, CurDacAdr);

    //
    // Turn on LONG_PIPE to extend the cursor FIFO.
    //
    HAL_DAC_FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _PIPE, _LONG, CurDacAdr);

    // preset xor cursor control to disabled
    HAL_DAC_FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _CUR_32B_ROP, _DISABLE, CurDacAdr); 

    //
    // Set xor mode if specified.
    //
    if (pDacProgramCursorImgArg->colorFormat == NV207C_SET_CURSOR_IMAGE_FORMAT_COLOR_XOR_LE_A8R8G8B8)
        HAL_DAC_FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _CUR_32B_ROP, _ENABLE, CurDacAdr);

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
// Called from display driver to confirm that sufficient bandwidth is available
// for the proposed configuration.
// For this chip, stubbed out to always return success...
//
RM_STATUS
nvHalDacValidateBandwidth_NV20( VOID *arg)
{
    return RM_OK;
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
    U032        i, pclk_limit;
    U032        head;

    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pVidLutCurDac0;
    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pVidLutCurDac1;

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

    pVidLutCurDac0 = (PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT) pDacHalInfo->CrtcInfo[0].pVidLutCurDac;
    pVidLutCurDac1 = (PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT) pDacHalInfo->CrtcInfo[1].pVidLutCurDac;

    head = pDacValidateArbSettingsArg->head;

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

    // Make sure any uninitialized sim data starts with 0's
    for (i = 0; i < sizeof(sim_data); i++)
        ((U008 *)&sim_data)[i] = 0x0;

    //
    // Build the sim table using current system settings
    //
    sim_data.enable_video   = pDacValidateArbSettingsArg->video_enabled ? 1 : 0;
    sim_data.enable_mp      = pDacValidateArbSettingsArg->mport_enabled ? 1 : 0;
    sim_data.memory_width   = pDacHalInfo->InputWidth;
    sim_data.mem_latency    = 3;
    sim_data.memory_type    = (pFbHalInfo->RamType == BUFFER_DDRAM) ? 1 : 0; //1=DD,0=SD
    sim_data.mem_aligned    = 1;
    sim_data.mem_page_miss  = 10;
    sim_data.gr_during_vid  = pVideoHalInfo->ColorKeyEnabled ? 1 : 0;
    sim_data.pclk_khz       = pDacValidateArbSettingsArg->vclk / 1000;  // in kHz, not MHz
    sim_data.mclk_khz       = pDacHalInfo->MClk / 1000;     // in kHz, not MHz
    sim_data.nvclk_khz      = pDacHalInfo->NVClk / 1000;    // in kHz, not MHz

    //NV11
    fifo_data.crtc1_lwm = 0;
    fifo_data.crtc2_lwm = 0;
    fifo_data.video_lwm = 0;
    fifo_data.crtc1_burst_size = 0;
    fifo_data.crtc2_burst_size = 0;
    fifo_data.video_burst_size = 0;
    fifo_data.valid = 0;

    //
    // We're using NV11-style arbitration here in anticipation
    // of dual-head support for nv20.
    //
    sim_data.enable_h1      =    (pVidLutCurDac0)?1:0;
    if (pVidLutCurDac0) {
        sim_data.pix_bpp    = pVidLutCurDac0->Dac[0].PixelDepth;
        sim_data.hres       = pVidLutCurDac0->Dac[0].VisibleImageWidth;
        sim_data.htotal     = pVidLutCurDac0->Dac[0].TotalWidth;
        sim_data.vres       = pVidLutCurDac0->Dac[0].VisibleImageHeight;
        sim_data.nativeVRes = (HAL_GETDISPLAYTYPE(0) == DISPLAY_TYPE_FLAT_PANEL)?pDacHalInfo->fpVMax:sim_data.vres;

        //LPL: copied math below, but I don't get it... *10000?
        sim_data.refresh    =
            (pVidLutCurDac0->Dac[0].PixelClock*10000) /
            (pVidLutCurDac0->Dac[0].TotalWidth * pVidLutCurDac0->Dac[0].TotalHeight);    //75;

        // sim_data.pix_bpp   =   32;
        sim_data.pclk_khz   =  pVidLutCurDac0->Dac[0].PixelClock * 10;
    } else {
        sim_data.hres       = 640;
        sim_data.htotal     = 832;
        sim_data.vres       = 480;
        sim_data.nativeVRes = sim_data.vres;
        sim_data.refresh    = 85;    //75;
        // sim_data.pix_bpp    =   32;
        sim_data.pclk_khz   =  36000;   //3404772;
    }
    
    sim_data.enable_h2    = (pVidLutCurDac1)?1:0;       //0
    if (pVidLutCurDac1) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "nvHalDacValidateArbSettings: head1 has a dac object!!!\n");
        DBG_BREAKPOINT();
        sim_data.pix_bpp_h2   = pVidLutCurDac1->Dac[0].PixelDepth; 
        sim_data.hres2        = pVidLutCurDac1->Dac[0].VisibleImageWidth;
        sim_data.htotal2      = pVidLutCurDac1->Dac[0].TotalWidth;
        sim_data.vres2        = pVidLutCurDac1->Dac[0].VisibleImageHeight;
        sim_data.nativeVRes2  = (HAL_GETDISPLAYTYPE(1) == DISPLAY_TYPE_FLAT_PANEL)?pDacHalInfo->fpVMax:sim_data.vres2;

        //LPL: copied math below, but I don't get it... *10000?
        sim_data.refresh2     = (pVidLutCurDac1->Dac[0].PixelClock*10000) /
                                (pVidLutCurDac1->Dac[0].TotalWidth * pVidLutCurDac1->Dac[0].TotalHeight);    //60;

        sim_data.pclk2_khz    =   pVidLutCurDac1->Dac[0].PixelClock * 10;  //641088;
    } else {
        sim_data.hres2        = 640;
        sim_data.htotal2      = 832;
        sim_data.vres2        = 480;
        sim_data.nativeVRes2  = sim_data.vres2;
        sim_data.refresh2     = 85;    //75;
        // sim_data.pix_bpp      =   32;
        sim_data.pclk2_khz    =  36000;   //3404772;
    }

//    sim_data.enable_video =    1;
//    sim_data.enable_mp    =    0;

//    sim_data.memory_width =  128;
//    sim_data.memory_type  =    0;
//    sim_data.mclk_mhz     =  166;

    //
    // If the overlay is active, use the head that owns the video scaler,
    // otherwise, for calculating if the overlay is allowed, assume this
    // head.
    //
	if (pVideoHalInfo->ActiveVideoOverlayObject)
        sim_data.h2_owns_vs = pVideoHalInfo->Head;
    else
        sim_data.h2_owns_vs = head;

    sim_data.video_source_hres = 720;
    sim_data.video_source_vres = 480;
//end NV11

    //Override the bit plane depth with passed parameter.
    if (head) {
        sim_data.pix_bpp_h2 = pDacValidateArbSettingsArg->bpp;   //32;
    } else {
        sim_data.pix_bpp    = pDacValidateArbSettingsArg->bpp;   //bpp;
    }

    //
    // What if we're still not sure?  Well, for now let's pick
    // the worst case...
    if (!sim_data.pix_bpp) sim_data.pix_bpp = 32;
    if (!sim_data.pix_bpp_h2) sim_data.pix_bpp_h2 = 32;

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
    U008        lock0, lock1;
    U032        i;
    U032        M, N, O, P;

    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pVidLutCurDac0;
    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pVidLutCurDac1;

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

    pVidLutCurDac0 = (PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT) pDacHalInfo->CrtcInfo[0].pVidLutCurDac;
    pVidLutCurDac1 = (PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT) pDacHalInfo->CrtcInfo[1].pVidLutCurDac;

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

    // Make sure any uninitialized sim data starts with 0's
    for (i = 0; i < sizeof(sim_data); i++)
        ((U008 *)&sim_data)[i] = 0x0;

    RM_ASSERT(pDacHalInfo->CrtcInfo[Head].pVidLutCurDac);
    
    //
    // Check if we can support an overlay in this resolution.  If we can, set the default desktop timings
    // to assume we are running an overlay.
    //
    dacValidateArbSettingsArg.id = DAC_VALIDATEARBSETTINGS_000;
    dacValidateArbSettingsArg.pHalHwInfo = pHalHwInfo;
    dacValidateArbSettingsArg.bpp = (pDacHalInfo->CrtcInfo[Head].pVidLutCurDac)
                                  ? ((PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT) pDacHalInfo->CrtcInfo[Head].pVidLutCurDac)->Dac[0].PixelDepth
                                  : 32;
    dacValidateArbSettingsArg.video_enabled = 1;
    dacValidateArbSettingsArg.mport_enabled = 0;
    dacValidateArbSettingsArg.vclk = pDacHalInfo->PixelClock * 10000;
    dacValidateArbSettingsArg.head = Head;

    nvHalDacValidateArbSettings_NV20(&dacValidateArbSettingsArg);
    if (dacValidateArbSettingsArg.result)
        sim_data.enable_video = 1;
    else
        sim_data.enable_video = 0;

    // Need to update the flag
    pVideoHalInfo->OverlayAllowed = sim_data.enable_video;

    //sim_data.pix_bpp      = (char)pDacHalInfo->Depth;
    //sim_data.enable_video = pDev->Video.Enabled ? 1 : 0;
    sim_data.enable_mp      = pMpHalInfo->Enabled ? 1 : 0;
    sim_data.memory_width   = pDacHalInfo->InputWidth;
    sim_data.mem_aligned    = 1;
    sim_data.memory_type    = (pFbHalInfo->RamType == BUFFER_DDRAM) ? 1 : 0; //1=DD,0=SD
    sim_data.mem_latency    = 3;
    sim_data.mem_page_miss  = 10;
    sim_data.gr_during_vid  = pVideoHalInfo->ColorKeyEnabled ? 1 : 0;
    sim_data.pclk_khz       = pDacHalInfo->VClk / 1000;     // in kHz, not MHz
    sim_data.mclk_khz       = pDacHalInfo->MClk / 1000;     // in kHz, not MHz
    sim_data.nvclk_khz      = pDacHalInfo->NVClk / 1000;    // in kHz, not MHz
 
    // 
    // Get those new numbers
    //
    fifo_data.crtc1_lwm = 0;
    fifo_data.crtc2_lwm = 0;
    fifo_data.video_lwm = 0;
    fifo_data.crtc1_burst_size = 0;
    fifo_data.crtc2_burst_size = 0;
    fifo_data.video_burst_size = 0;
    fifo_data.valid = 0;

    sim_data.enable_h1        = (pVidLutCurDac0)?1:0;    //1
    if (pVidLutCurDac0) {
        sim_data.pix_bpp      = pVidLutCurDac0->Dac[0].PixelDepth;          //May override below

        sim_data.hres         = pVidLutCurDac0->Dac[0].VisibleImageWidth;   //2048;
        sim_data.htotal       = pVidLutCurDac0->Dac[0].TotalWidth;          //2832;
        sim_data.vres         = pVidLutCurDac0->Dac[0].VisibleImageHeight;  //1536;
        sim_data.nativeVRes = (HAL_GETDISPLAYTYPE(0) == DISPLAY_TYPE_FLAT_PANEL)?pDacHalInfo->fpVMax:sim_data.vres;

        //LPL: copied math below, but I don't get it... *10000?
        sim_data.refresh      = (pVidLutCurDac0->Dac[0].PixelClock*10000) /
                                (pVidLutCurDac0->Dac[0].TotalWidth * pVidLutCurDac0->Dac[0].TotalHeight);    //75;

    //    sim_data.pix_bpp    =   32;
        sim_data.pclk_khz     =  pVidLutCurDac0->Dac[0].PixelClock * 10;    //3404772;
    } else {
        sim_data.hres         = 640;
        sim_data.htotal       = 832;
        sim_data.vres         = 480;
        sim_data.nativeVRes   = sim_data.vres;
        sim_data.refresh      = 85;     //75;
    //    sim_data.pix_bpp    = 32;
        sim_data.pclk_khz     = 36000;  //3404772;
    }
    
    sim_data.enable_h2    = (pVidLutCurDac1)?1:0;       //0
    if (pVidLutCurDac1) {
        sim_data.pix_bpp_h2   = pVidLutCurDac1->Dac[0].PixelDepth;          //May override below
        sim_data.hres2        = pVidLutCurDac1->Dac[0].VisibleImageWidth;   //1024;
        sim_data.htotal2      = pVidLutCurDac1->Dac[0].TotalWidth;          //1344;
        sim_data.vres2        = pVidLutCurDac1->Dac[0].VisibleImageHeight;  //768;
        sim_data.nativeVRes2  = (HAL_GETDISPLAYTYPE(1) == DISPLAY_TYPE_FLAT_PANEL)?pDacHalInfo->fpVMax:sim_data.vres2;

        //LPL: copied math below, but I don't get it... *10000?
        sim_data.refresh2     = (pVidLutCurDac1->Dac[0].PixelClock*10000) /
                                (pVidLutCurDac1->Dac[0].TotalWidth * pVidLutCurDac1->Dac[0].TotalHeight);    //60;

        sim_data.pclk2_khz    =   pVidLutCurDac1->Dac[0].PixelClock * 10;   //641088;
    } else {
        sim_data.hres2        = 640;
        sim_data.htotal2      = 832;
        sim_data.vres2        = 480;
        sim_data.nativeVRes2  = sim_data.vres2;
        sim_data.refresh2     = 85;     //75;
    //    sim_data.pix_bpp    = 32;
        sim_data.pclk2_khz    = 36000;  //3404772;
    }

    if (!sim_data.pix_bpp) sim_data.pix_bpp = 32;
    if (!sim_data.pix_bpp_h2) sim_data.pix_bpp_h2 = 32;

//    sim_data.enable_video =    1;
//    sim_data.enable_mp    =    0;

//    sim_data.memory_width =  128;
//    sim_data.memory_type  =    0;
//    sim_data.mclk_mhz     =  166;

    //
    // If the overlay is active, use the head that owns the video scaler,
    // otherwise, for calculating if the overlay is allowed, assume this
    // head.
    //
	if (pVideoHalInfo->ActiveVideoOverlayObject)
        sim_data.h2_owns_vs = pVideoHalInfo->Head;
    else
        sim_data.h2_owns_vs = Head;

    sim_data.video_source_hres = 720;
    sim_data.video_source_vres = 480;

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

#ifdef IKOS
        if (do_pvideo)
        {
#endif
        //
        // Check overlay capabilities.
        //
        //if (!NV20_dacCalculateVideoCaps(pDev))
        //    pDev->Video.HalInfo.OverlayAllowed = 0;


        // Eliminate delay between memory requests by the video scalar
        REG_WR32(NV_PVIDEO_DEBUG_1, 0);

        //Final clipping -- check with Jeff to see if the values are already 'safe'
        fifo_data.video_burst_size /= 32;
        if (!fifo_data.video_burst_size) fifo_data.video_burst_size = 1;
        if (fifo_data.video_burst_size > 16) fifo_data.video_burst_size = 16;
        FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_2, _BURST1, fifo_data.video_burst_size);
        FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_2, _BURST2, fifo_data.video_burst_size);

        fifo_data.video_lwm /= 16;
        FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_3, _WATER_MARK1, fifo_data.video_lwm);
        FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_3, _WATER_MARK2, fifo_data.video_lwm);

#ifdef IKOS
        }
#endif

        //
        // Update the CRTC watermarks
        //
        if (pVidLutCurDac0)
        {
            // Unlock CRTC extended regs
            HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock0, 0);
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 0);

            //
            // Set the CRTC watermarks and burst size
            //
            fifo_data.crtc1_lwm /= 8;
            if (fifo_data.crtc1_lwm > 511) fifo_data.crtc1_lwm = 511;
            HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, (fifo_data.crtc1_lwm & 0xff), 0);
            if (fifo_data.crtc1_lwm > 255)
            {
                HAL_CRTC_WR(NV_CIO_CRE_FFLWM_MSB_INDEX, 1, 0);
            }
            else
            {
                HAL_CRTC_WR(NV_CIO_CRE_FFLWM_MSB_INDEX, 0, 0);
            }

            fifo_data.crtc1_burst_size /= 32;
            if (fifo_data.crtc1_burst_size) fifo_data.crtc1_burst_size--;
            if (!fifo_data.crtc1_burst_size) fifo_data.crtc1_burst_size++;
            if (fifo_data.crtc1_burst_size >= (2048/32)) fifo_data.crtc1_burst_size = 2048/32 - 1;
            HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, fifo_data.crtc1_burst_size, 0);

            // Relock if necessary
            if (lock0 == 0)
                HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, 0);
        }

        if (pVidLutCurDac1)
        {
            // Unlock CRTC extended regs
            HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock1, 1);
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 1);

            //
            // Set the CRTC watermarks and burst size
            //
            fifo_data.crtc2_lwm /= 8;
            if (fifo_data.crtc2_lwm > 511) fifo_data.crtc2_lwm = 511;
            HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, (fifo_data.crtc2_lwm & 0xff), 1);
            if (fifo_data.crtc2_lwm > 255)
            {
                HAL_CRTC_WR(NV_CIO_CRE_FFLWM_MSB_INDEX, 1, 1);
            }
            else
            {
                HAL_CRTC_WR(NV_CIO_CRE_FFLWM_MSB_INDEX, 0, 1);
            }

            fifo_data.crtc2_burst_size /= 32;
            if (fifo_data.crtc2_burst_size) fifo_data.crtc2_burst_size--;
            if (!fifo_data.crtc2_burst_size) fifo_data.crtc2_burst_size++;
            if (fifo_data.crtc2_burst_size >= (2048/32)) fifo_data.crtc2_burst_size = 2048/32 - 1;
            HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, fifo_data.crtc2_burst_size, 1);

            // Relock if necessary
            if (lock1 == 0)
                HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, 1);
        }

        //
        // Relock if necessary
        //
        return (RM_OK);
    }
    else    //if (fifo_data.valid)
    {
        //
        // No valid setting was found!!!
        //
        //  Make a conservative guess and fail.
        //
        
        // Unlock CRTC extended regs
        if (pVidLutCurDac0)
        {
            HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock0, 0);
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 0);
        
            HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, 0x72, 0);
            HAL_CRTC_WR(NV_CIO_CRE_FFLWM_MSB_INDEX, 0, 0);
            HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 1024/32 - 32, 0);

            if (lock0 == 0)
                HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, 0);
        }
        if (pVidLutCurDac1)
        {
            HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock1, 1);
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 1);
        
            HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, 0x72, 1);
            HAL_CRTC_WR(NV_CIO_CRE_FFLWM_MSB_INDEX, 0, 1);
            HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 1024/32 - 32, 1);

            if (lock1 == 0)
                HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, 1);
        }
        
        return (RM_ERROR);
    }
}

RM_STATUS
nvHalDacHotPlugInterrupts_NV20(VOID * pArgs)
{
    return RM_ERROR;
}
