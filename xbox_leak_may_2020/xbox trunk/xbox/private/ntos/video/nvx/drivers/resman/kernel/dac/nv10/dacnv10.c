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
* Module: DACARB.C                                                          *
*   The DAC Arbitration management.                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nv10_ref.h>
#include <nvrm.h>
#include <nv10_hal.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <dma.h>
#include <gr.h>
#include <dac.h>
#include <os.h>
#include "nvhw.h"
#include <vga.h>

//#include <nvd3dmac.h>

/* This file simulates the NV arbitor */
#include <arb_fifo.h>
#include "nvhalcpp.h"

//LPL - memo to myself
// - clean-up to do: may not need default settings when pVidLutCurDac ptrs NULL.
// - clean up bpp overrides
// - with new top-of-buffer lwm approach, can some of the math be simplified?
// - does setting lwm based on pre-clipped burst size cause discrepancies?

//#define NV11_DEBUG

#ifdef NV11_DEBUG
U032 nv11Aware = 1;    //debug toggle between nv10/nv11 arb fns
#endif  //#ifdef NV11_DEBUG

//////////////////////////////////////////
//
// NV 11 arbitration routines
//

U032
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
void
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

void
check_crtc_burstsizes(crtc_t *crtc)
{
  U032 raw = crtc->burst_size;
  
  if (!crtc->enable) return;
 
  /* crtc burst sizes are a multiple of 16 bytes */
  raw = (raw + 15) / 16 * 16;
  /* burst sizes step by 16 up to 2048 bytes*/
  if (raw > crtc->max_burst_size)
    raw = crtc->max_burst_size;
  if (raw != crtc->burst_size)
    crtc->burst_size = raw;
}

void
init_crtc(crtc_t *crtc,
      U032 enable,
      U032 hres,
      U032 vres,
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

void
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

void
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

void
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


void
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

U032
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
    
void
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
U032
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
U032
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
U032 test_fifo_constraints(U032 fifo_size, crtc_t *crtc, crtc_t *crtc2, scaler_t *scaler) 
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
U032
insufficient_total_bandwidth(crtc_t *crtc, crtc_t *crtc2, scaler_t *scaler, mem_t *mem)
{
  U032 error;
  U032 video_bandwidth;
  U032 available_bandwidth;

  video_bandwidth = (crtc->enable  *(crtc->hres  * crtc->vres  * crtc->depth  / 8 ) *  crtc->refresh / 10 + 
                     crtc2->enable *(crtc2->hres * crtc2->vres * crtc2->depth / 8 ) * crtc2->refresh / 10 + 
                     scaler->enable*(scaler->source_hres * scaler->source_vres * 2) *
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
U032
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
U032
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



U032
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
U032
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
U032
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
U032
test_configuration(crtc_t *crtc, crtc_t *crtc2, scaler_t *scaler, rtr_t *rtr, U032 max_burst_size, mem_t *mem, U032 fifo_size)
{
  U032 error;

  error = insufficient_total_bandwidth(crtc, crtc2, scaler, mem);
  if (error) {
    return error;
  }

  for (crtc->burst_size = crtc->min_burst_size; crtc->burst_size <= max_burst_size; crtc->burst_size += 16) {

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
    for (scaler->burst_size += scaler->min_burst_size; scaler->burst_size <= max_burst_size; scaler->burst_size += 16) {
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

//
// Main NV11 arbitration routine
//
void NV11_dacCalculateArbitration(
    PHALHWINFO pHalHwInfo,
    fifo_info *fifo,
    sim_state *arb)
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
 
  /*NV11 default values*/
  U032 page_fault_penalty   = 2;
  U032 page_fault_width     = 256;
  U032 arbitration_latency  = 10;
  U032 pipeline_latency     = 28;
  U032 stall_cycles         = 3;

  U032 min_burst_size   =   16;
  U032 max_burst_size   = 2048;
  U032 crtc_fifo_size   = 2048; /*CRTC fifo size in bytes*/
  U032 scaler_fifo_size = 1856; /*video scaler's fifo size*/
 
  U032 partition_data_width;
  U032 ddr;
 
  /* set up some default values for the CRTC ... */
  U032 hres    ;
  U032 htotal  ;
  U032 hres2   ;
  U032 htotal2 ;
  U032 vres    ;
  U032 vres2   ;
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

  hres2        = arb->hres2;
  htotal2      = arb->htotal2;
  vres2        = arb->vres2;
  depth2       = arb->pix_bpp_h2;
  refresh2     = arb->refresh2;
  pclk2        = arb->pclk2_khz / 1000;

  partition_data_width = arb->memory_width;
  ddr                  = arb->memory_type;
  mclk                 = arb->mclk_khz / 1000;

  init_mem(&mem, mclk, ddr, arbiter, partitions, partition_data_width, page_fault_penalty,
       page_fault_width, refresh_penalty, arbitration_latency, pipeline_latency, stall_cycles,
       depth_cmd, depth_rw, rtr_interrupt_rate);
  init_crtc(&crtc, crtc_enable,
            hres, vres, depth, refresh,
            crtc_request_delay_mclks, crtc_request_delay_pclks,
         crtc_burst_size, min_burst_size, max_burst_size, cursor_burst_size,
        pclk, htotal, &mem);
  init_crtc(&crtc2, crtc2_enable,
            hres2, vres2, depth2, refresh2,
            crtc_request_delay_mclks, crtc_request_delay_pclks,
         crtc_burst_size, min_burst_size, max_burst_size, cursor_burst_size,
            pclk2, htotal2, &mem);
  init_scaler(&scaler, &crtc, &crtc2, mclk,
              (scaler_disable) ? 0 : 1, video_source_hres, video_source_vres, h2_owns_vs, scaler_depth,
              scaler_window_hres, scaler_window_vres,
           scaler_burst_size, min_burst_size, max_burst_size,
           scaler_request_delay_mclks, scaler_request_delay_pclks, scaler_fifo_size);
  init_rtr(&rtr, rtr_burst_size, rtr_request_delay_mclks, &mem);

//  if ((hres2*depth2*1000/crtc2.hclk_period) > (hres*depth*1000/crtc.hclk_period)) {
  who_owns_scaler = 0;
  if (crtc2.enable && crtc.enable) {
    if ((hres2*depth2*1000/crtc2.hclk_period) > (hres*depth*1000/crtc.hclk_period))
        who_owns_scaler = 1;
  } else {
      if (crtc2.enable)
        who_owns_scaler = 1;
  }

  if (who_owns_scaler) {
    if (crtc.enable) scaler.h2_owns_vs = !scaler.h2_owns_vs; /*swap which head scaler is owned by when swapping heads*/
    error = test_configuration(&crtc2, &crtc,  &scaler, &rtr, max_burst_size, &mem, crtc_fifo_size); 
    if (crtc.enable) scaler.h2_owns_vs = !scaler.h2_owns_vs;
  } else
    error = test_configuration(&crtc,  &crtc2, &scaler, &rtr, max_burst_size, &mem, crtc_fifo_size); 

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


//////////////////////////////////////////
//
// NV 10 arbitration routines
//
static
VOID NV10_dacCalculateArbitration
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
U032 NV10_dacCalculateVideoCaps
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

typedef struct tagNV10_DacAGPClockEntry
{
    U032 frequency;
    U032 setting;
} NV10_DacAGPClockEntry;

NV10_DacAGPClockEntry NV10_dacAGPClockTable[] =
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
RM_STATUS nvHalDacControl_NV10(VOID *);
RM_STATUS nvHalDacAlloc_NV10(VOID *);
RM_STATUS nvHalDacFree_NV10(VOID *);
RM_STATUS nvHalDacSetStartAddr_NV10(VOID *);
RM_STATUS nvHalDacProgramMClk_NV10(VOID *);
RM_STATUS nvHalDacProgramNVClk_NV10(VOID *);
RM_STATUS nvHalDacProgramPClk_NV10(VOID *);
RM_STATUS nvHalDacProgramCursorImage_NV10(VOID *);
RM_STATUS nvHalDacGetRasterPosition_NV10(VOID *);
RM_STATUS nvHalDacValidateArbSettings_NV10(VOID *arg);
RM_STATUS nvHalDacUpdateArbSettings_NV10(VOID *arg);

//
// nvHalDacControl
//
RM_STATUS
nvHalDacControl_NV10(VOID *arg)
{
    PDACCONTROLARG_000 pDacControlArg = (PDACCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PDACHALINFO pDacHalInfo;
    PVIDEOHALINFO pVideoHalInfo;
    PDACHALINFO_NV10 pDacHalPvtInfo;

    //
    // Verify interface revision.
    //
    if (pDacControlArg->id != DAC_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacControlArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pVideoHalInfo = pHalHwInfo->pVideoHalInfo;
    pDacHalPvtInfo = (PDACHALINFO_NV10)pHalHwInfo->pDacHalInfo;

    switch (pDacControlArg->cmd)
    {
        case DAC_CONTROL_INIT:

            // set bus width
            if (REG_RD_DRF(_PFB, _CFG, _BUS) == NV_PFB_CFG_BUS_128)
                pDacHalInfo->InputWidth = 128;
            else
                pDacHalInfo->InputWidth = 64;

            // initialize default crtc parameters
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

            // if we're an nv11, we need to setup a second crtc
            // The capabilities of the second head should be determined by the BIOS,
            // but for now we will hard code them. These capabilities are such things 
            // as the presence of an external DAC, and the frequency specification.

            // The values set here will be replaced in dacinit.c/initDac if it is able
            // to find the proper tables in the BIOS
            if (IsNV11_NV10(pHalHwInfo->pMcHalInfo))
            {
                U032 i, data32, crtcAddr;
                U008 lock;

                //NVASSERT(pDacHalInfo->NumCrtcs == 2);
                pDacHalInfo->CrtcInfo[1].PCLKLimit8bpp  = 150000000;
                pDacHalInfo->CrtcInfo[1].PCLKLimit16bpp = 150000000;
                pDacHalInfo->CrtcInfo[1].PCLKLimit32bpp = 150000000;
                pDacHalInfo->CrtcInfo[1].RegOffset = 0x00002000;

                //
                // Head 1 supports TV and flat panel, and CRT, if an external DAC is present
                //
                pDacHalInfo->CrtcInfo[1].DisplayTypesAllowed =
                    DISPLAYTYPEBIT(DISPLAY_TYPE_MONITOR) |      // assume external DAC
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
            }

            // turn on CRTC big endian if we're setup for big endian mode
            if (pHalHwInfo->pMcHalInfo->EndianControl & MC_ENDIAN_CONTROL_CHIP_BIG_ENDIAN)
            {
                FLD_WR_DRF_NUM(_PCRTC, _CONFIG, _ENDIAN, NV_PCRTC_CONFIG_ENDIAN_BIG);
            }

            //
            // Set default downscale values.
            // 
            pVideoHalInfo->OverlayMaxDownscale_768  = 8;
            pVideoHalInfo->OverlayMaxDownscale_1280 = 8;
            pVideoHalInfo->OverlayMaxDownscale_1920 = 8;

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
nvHalDacAlloc_NV10(VOID *arg)
{
    PDACALLOCARG_000 pDacAllocArg = (PDACALLOCARG_000)arg;

    //
    // Verify interface revision.
    //
    if (pDacAllocArg->id != DAC_ALLOC_000)
        return (RM_ERR_VERSION_MISMATCH);

    //
    // Verify that class and head parameters are legal for this chip.
    //

    return (RM_OK);
}

RM_STATUS
nvHalDacFree_NV10(VOID *arg)
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
nvHalDacSetStartAddr_NV10(VOID *arg)
{
    PDACSETSTARTADDRARG_000 pDacSetStartAddrArg = (PDACSETSTARTADDRARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pDacHalObj;
    PDACHALINFO pDacHalInfo;
    U032    crtcOffset;
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
// nvHalDacProgramMClk_NV10 - Program MCLK based on the value in pDacHalInfo->MClk
//
// This routine uses the value in pDacHalInfo->MClk (in Hz) to calculate
// M, N, and P values.  Those values are then stored in the pDev->Dac
// structure.  The hardware is then programmed with the new M, N, and P
// values in a manner that is safe for the PLL.
//
RM_STATUS
nvHalDacProgramMClk_NV10(VOID *arg)
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

    // Wait for M PLL to lock.
    osDelay(64 /* ms */);

    // Reset (resync) the frame buffer DRAM DLL.
    FLD_WR_DRF_DEF( _PFB, _MRS, _DLL, _RESET );

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
// nvHalDacProgramNVClk_NV10 - Program NVCLK based on the value in pDacHalInfo->NVClk
//
// This routine uses the value in pDacHalInfo->NVClk (in Hz) to calculate
// M, N, and P values.  Those values are then stored in the pDev->Dac
// structure.  The hardware is then programmed with the new M, N, and P
// values in a manner that is safe for the PLL.
//
RM_STATUS
nvHalDacProgramNVClk_NV10(VOID *arg)
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
    U032 IsPllSlowed;
    U032 Head = 0;  // NVPLL is on head 0
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

    // Make sure to disable the clock slowdown circuit
    if (IsNV11_NV10(pMcHalInfo))
    {
        IsPllSlowed = REG_RD_DRF(_PBUS, _DEBUG_1, _CORE_SLOWDWN);
        FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, 0);
    }

    // Retrieve original coefficients
    coeff = REG_RD32( NV_PRAMDAC_NVPLL_COEFF );
    mOld = DRF_VAL( _PRAMDAC, _NVPLL_COEFF, _MDIV, coeff);
    nOld = DRF_VAL( _PRAMDAC, _NVPLL_COEFF, _NDIV, coeff);
    pOld = DRF_VAL( _PRAMDAC, _NVPLL_COEFF, _PDIV, coeff);

    // Restore the clock slowdown circuit
    if (IsNV11_NV10(pMcHalInfo))
    	FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, IsPllSlowed);

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
    while ( ( pDacHalInfo->NVClk <= NV10_dacAGPClockTable[ i ].frequency ) &&
        ( i < sizeof( NV10_dacAGPClockTable ) / sizeof( NV10_dacAGPClockTable[ 0 ] ) - 1 ) )
    {
        i++;
    }

    FLD_WR_DRF_NUM( _PBUS, _DEBUG_3, _AGP_4X_NVCLK, NV10_dacAGPClockTable[ i ].setting );
    
    return status;
}

//
// nvHalDacProgramPClk_NV10
//
RM_STATUS
nvHalDacProgramPClk_NV10(VOID *arg)
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
// nvHalDacProgramCursorImage_NV10
//
RM_STATUS
nvHalDacProgramCursorImage_NV10(VOID *arg)
{
    PDACPROGRAMCURSORARG_000 pDacProgramCursorImgArg = (PDACPROGRAMCURSORARG_000) arg;
    PHALHWINFO  pHalHwInfo;
    PMCHALINFO  pMcHalInfo;
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
    pMcHalInfo = pHalHwInfo->pMcHalInfo;
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
            if (IsNV15orBetter_NV10(pMcHalInfo))
            {
                // set bpp = 1, blend is already 0 (only supported in NV15 or later)
                cursorConfig |= (NV_PCRTC_CURSOR_CONFIG_CUR_BPP_32 << DRF_SHIFT(NV_PCRTC_CURSOR_CONFIG_CUR_BPP));
            }
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
    //
    // TO DO: there is much unnecessary logic here -- can this all be collapsed into one write?
    if (IsNV15orBetter_NV10(pMcHalInfo))
    {   
        if (HAL_GETDISPLAYTYPE(pDacHalObj->Head) != DISPLAY_TYPE_TV)
        {
            if ((pDacProgramCursorImgArg->colorFormat == NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_A1R5G5B5) ||
                (pDacProgramCursorImgArg->colorFormat == NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_A8R8G8B8) ||
                (pDacProgramCursorImgArg->colorFormat == NV07C_SET_CURSOR_IMAGE_FORMAT_COLOR_PM_LE_A8R8G8B8))
            {    
                HAL_DAC_FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _PIPE, _LONG, CurDacAdr);                
            }
            else
            {
                // forcing a long pipe works around a HW bug where inverted background causes screen tearing effects
                HAL_DAC_FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _PIPE, _LONG, CurDacAdr); 
            }    
        }
        else
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
// nvHalDacGetRasterPosition_NV10
//
RM_STATUS
nvHalDacGetRasterPosition_NV10(VOID *arg)
{
    PDACRASTERPOSARG_000 pDacRasterPosArg = (PDACRASTERPOSARG_000) arg;
    PHALHWINFO  pHalHwInfo;
    PDACHALINFO pDacHalInfo;
    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pDacHalObj;
    U032        CurDacAdr; // must be defined for CRTC_RD macro & must point to pDacHalInfo

    //
    // Verify interface revision.
    //
    if (pDacRasterPosArg->id != DAC_RASTERPOS_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacRasterPosArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pDacHalObj = (PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT) pDacRasterPosArg->pHalObjInfo;
    CurDacAdr = pDacHalInfo->CrtcInfo[pDacHalObj->Head].RegOffset;

    // no RL0/RL1 index regs, so return NV_PCRTC_RASTER_POSITION
    pDacRasterPosArg->result =  (U016) HAL_DAC_REG_RD_DRF(_PCRTC, _RASTER, _POSITION, CurDacAdr);
                                        
    return (RM_OK);
}

//
// Attempt to validate the given parameters will work with the current configuration
//
//  Return 0  if not capable
//  Return ~0 if valid
//
RM_STATUS
nvHalDacValidateArbSettings_NV10(VOID *arg)
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
    U032        head = 0;

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

    //
    // First we must validate that the incoming vclk is within our DAC range.
    //
    switch (pDacValidateArbSettingsArg->bpp)
    {
        case 8:
            pclk_limit = pDacHalInfo->CrtcInfo[head].PCLKLimit8bpp;
            break;
        case 15:
        case 16:
            pclk_limit = pDacHalInfo->CrtcInfo[head].PCLKLimit16bpp;
            break;
        case 24:
        case 32:
            pclk_limit = pDacHalInfo->CrtcInfo[head].PCLKLimit32bpp;
            break;
        default:
            pclk_limit = pDacHalInfo->CrtcInfo[head].PCLKLimit16bpp;
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

    // NV11 params
    fifo_data.crtc1_lwm = 0;
    fifo_data.crtc2_lwm = 0;
    fifo_data.video_lwm = 0;
    fifo_data.crtc1_burst_size = 0;
    fifo_data.crtc2_burst_size = 0;
    fifo_data.video_burst_size = 0;
    fifo_data.valid = 0;

    sim_data.enable_h1 = (pVidLutCurDac0) ? 1 : 0;    //1
    if (pVidLutCurDac0) {
        sim_data.pix_bpp      = pVidLutCurDac0->Dac[0].PixelDepth;          //May override below
        sim_data.hres         = pVidLutCurDac0->Dac[0].VisibleImageWidth;   //2048;
        sim_data.htotal       = pVidLutCurDac0->Dac[0].TotalWidth;          //2832;
        sim_data.vres         = pVidLutCurDac0->Dac[0].VisibleImageHeight;  //1536;

        //LPL: copied math below, but I don't get it... *10000?
        sim_data.refresh      = (pVidLutCurDac0->Dac[0].PixelClock*10000) /
                                (pVidLutCurDac0->Dac[0].TotalWidth * pVidLutCurDac0->Dac[0].TotalHeight);    //75;

    //    sim_data.pix_bpp    = 32;
        sim_data.pclk_khz     = pVidLutCurDac0->Dac[0].PixelClock * 10;   //3404772;
    } else {
        sim_data.hres         = 640;
        sim_data.htotal       = 832;
        sim_data.vres         = 480;
        sim_data.refresh      = 85;     //75;
    //    sim_data.pix_bpp    = 32;
        sim_data.pclk_khz     = 36000;  //3404772;
    }
    
    sim_data.enable_h2        = (pVidLutCurDac1)?1:0;       //0
    if (pVidLutCurDac1) {
        sim_data.pix_bpp_h2   = pVidLutCurDac1->Dac[0].PixelDepth;          //May override below
        sim_data.hres2        = pVidLutCurDac1->Dac[0].VisibleImageWidth;   //1024;
        sim_data.htotal2      = pVidLutCurDac1->Dac[0].TotalWidth;          //1344;
        sim_data.vres2        = pVidLutCurDac1->Dac[0].VisibleImageHeight;  // 768;

        //LPL: copied math below, but I don't get it... *10000?
        sim_data.refresh2     = (pVidLutCurDac1->Dac[0].PixelClock*10000) /
                                (pVidLutCurDac1->Dac[0].TotalWidth * pVidLutCurDac1->Dac[0].TotalHeight);    //60;

        sim_data.pclk2_khz    =   pVidLutCurDac1->Dac[0].PixelClock * 10;   //641088;
    } else {
        sim_data.hres2        = 640;
        sim_data.htotal2      = 832;
        sim_data.vres2        = 480;
        sim_data.refresh2     = 85;     //75;
    //    sim_data.pix_bpp    =   32;
        sim_data.pclk2_khz    =  36000; //3404772;
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
// end NV11

    //Override the bit plane depth with passed parameter.
    if (head) {
        sim_data.pix_bpp_h2 = pDacValidateArbSettingsArg->bpp;   //32;
    } else {
        sim_data.pix_bpp    = pDacValidateArbSettingsArg->bpp;   //bpp;
    }

    //What if we're still not sure?  Well, for now let's pick the worst case...
    if (!sim_data.pix_bpp) sim_data.pix_bpp = 32;
    if (!sim_data.pix_bpp_h2) sim_data.pix_bpp_h2 = 32;

    //
    // Run the numbers through the ringer
    //
#ifdef NV11_DEBUG
    if (IsNV11_NV10(pMcHalInfo) && nv11Aware)
#else  //#ifdef NV11_DEBUG
    if (IsNV11_NV10(pMcHalInfo))
#endif  //#ifdef NV11_DEBUG
        NV11_dacCalculateArbitration(pHalHwInfo, &fifo_data, &sim_data);
    else
        NV10_dacCalculateArbitration(pHalHwInfo, &fifo_data, &sim_data);

    /*
    //
    // If the CRTC can do this, double check to make sure the overlay is able as well
    //
    if (fifo_data.valid && video_enabled)
        fifo_data.valid = NV10_dacCalculateVideoCaps(pDev);
    */
    
    pDacValidateArbSettingsArg->result = (U032)fifo_data.valid;
    return (RM_OK);
}

#ifdef IKOS
U032 do_pvideo = 0;
#endif

RM_STATUS
nvHalDacUpdateArbSettings_NV10(VOID *arg)
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

    // if (NV11_dacValidateArbitrationSettings(pHalHwInfo, pDacHalInfo->Depth, 1, 0, (pDacHalInfo->PixelClock * 10000), Head))
    nvHalDacValidateArbSettings_NV10(&dacValidateArbSettingsArg);
    if (dacValidateArbSettingsArg.result)
        sim_data.enable_video = 1;
    else
        sim_data.enable_video = 0;
    
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

        //LPL: copied math below, but I don't get it... *10000?
        sim_data.refresh      = (pVidLutCurDac0->Dac[0].PixelClock*10000) /
                                (pVidLutCurDac0->Dac[0].TotalWidth * pVidLutCurDac0->Dac[0].TotalHeight);    //75;

    //    sim_data.pix_bpp    =   32;
        sim_data.pclk_khz     =  pVidLutCurDac0->Dac[0].PixelClock * 10;    //3404772;
    } else {
        sim_data.hres         = 640;
        sim_data.htotal       = 832;
        sim_data.vres         = 480;
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

        //LPL: copied math below, but I don't get it... *10000?
        sim_data.refresh2     = (pVidLutCurDac1->Dac[0].PixelClock*10000) /
                                (pVidLutCurDac1->Dac[0].TotalWidth * pVidLutCurDac1->Dac[0].TotalHeight);    //60;

        sim_data.pclk2_khz    =   pVidLutCurDac1->Dac[0].PixelClock * 10;   //641088;
    } else {
        sim_data.hres2        = 640;
        sim_data.htotal2      = 832;
        sim_data.vres2        = 480;
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
#ifdef NV11_DEBUG
    if (IsNV11_NV10(pMcHalInfo) && nv11Aware)
#else  //#ifdef NV11_DEBUG
    if (IsNV11_NV10(pMcHalInfo))
#endif //#ifdef NV11_DEBUG
        NV11_dacCalculateArbitration(pHalHwInfo, &fifo_data, &sim_data);
    else
        NV10_dacCalculateArbitration(pHalHwInfo, &fifo_data, &sim_data);
    
    // for now, assume pass
    //fifo_data.valid = 1;
    
//    For TV, we should not change some of these. Gfx Fifo Low Water Mark in particular should
//        not be changed. 
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
        /*
        //
        // Check overlay capabilities.
        //
        if (!NV10_dacCalculateVideoCaps(pDev))
            pDev->Video.OverlayAllowed = 0;
        */

        if (IsNV11_NV10(pMcHalInfo)) {
            //Final clipping -- check with Jeff to see if the values are already 'safe'
            fifo_data.video_burst_size /= 32;
            if (!fifo_data.video_burst_size) fifo_data.video_burst_size = 1;
            if (fifo_data.video_burst_size > 16) fifo_data.video_burst_size = 16;
            FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_2, _BURST1, fifo_data.video_burst_size);
            FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_2, _BURST2, fifo_data.video_burst_size);

            fifo_data.video_lwm /= 16;
            FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_3, _WATER_MARK1, fifo_data.video_lwm);
            FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_3, _WATER_MARK2, fifo_data.video_lwm);
        } else {
            //
            // These are the PVIDEO burst/watermark hardcoded values
            // we've gotten from the HW engineers.
            //
            FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_2, _BURST1, 16);         // 512 bytes
            FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_2, _BURST2, 16);         // 512 bytes
            FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_3, _WATER_MARK1, 64);    // 1024 bytes
            FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_3, _WATER_MARK2, 64);    // 1024 bytes
        }

#ifdef IKOS
        }
#endif

        //
        // Update the CRTC watermarks
        // Note that for NV11 we've set the burst size registers to a different
        // operating mode -- therefore, the numbers written for NV11 and non-NV11
        // paths are not compatible.
        //
        if (IsNV11_NV10(pMcHalInfo)) {
            U032 CurDacAdr0, CurDacAdr1;

            CurDacAdr0 = pDacHalInfo->CrtcInfo[0].RegOffset;
            CurDacAdr1 = pDacHalInfo->CrtcInfo[1].RegOffset;

            // Unlock CRTC extended regs
            HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock0, CurDacAdr0);
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, CurDacAdr0);
            HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock1, CurDacAdr1);
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, CurDacAdr1);

            //
            // Set the CRTC watermarks and burst size
            //

            fifo_data.crtc1_lwm /= 8;
            if (fifo_data.crtc1_lwm > 255) fifo_data.crtc1_lwm = 255;
            HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, fifo_data.crtc1_lwm, CurDacAdr0);

            fifo_data.crtc1_burst_size /= 32;
            if (fifo_data.crtc1_burst_size) fifo_data.crtc1_burst_size--;
            if (!fifo_data.crtc1_burst_size) fifo_data.crtc1_burst_size++;
            if (fifo_data.crtc1_burst_size >= (2048/32)) fifo_data.crtc1_burst_size = 2048/32 - 1;
            HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, fifo_data.crtc1_burst_size, CurDacAdr0);

            fifo_data.crtc2_lwm /= 8;
            if (fifo_data.crtc2_lwm > 255) fifo_data.crtc2_lwm = 255;
            HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, fifo_data.crtc2_lwm, CurDacAdr1);

            fifo_data.crtc2_burst_size /= 32;
            if (fifo_data.crtc2_burst_size) fifo_data.crtc2_burst_size--;
            if (!fifo_data.crtc2_burst_size) fifo_data.crtc2_burst_size++;
            if (fifo_data.crtc2_burst_size >= (2048/32)) fifo_data.crtc2_burst_size = 2048/32 - 1;
            HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, fifo_data.crtc2_burst_size, CurDacAdr1);

            //
            // Relock if necessary
            //
            if (lock0 == 0)
                HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, CurDacAdr0);
            if (lock1 == 0)
                HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, CurDacAdr1);

        } else {    //if (IsNV11(pDev))
            // Unlock CRTC extended regs
            HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock0, CurDacAdr);
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, CurDacAdr);
       
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
            if (lock0 == 0)
                HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, CurDacAdr);
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
        if (IsNV11_NV10(pMcHalInfo)) {
            U032 CurDacAdr0, CurDacAdr1;

            CurDacAdr0 = pDacHalInfo->CrtcInfo[0].RegOffset;
            CurDacAdr1 = pDacHalInfo->CrtcInfo[1].RegOffset;

            HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock0, CurDacAdr0);
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, CurDacAdr0);
        
            HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, 0x72, CurDacAdr0);
            HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 1024/32 - 32, CurDacAdr0);

            HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock1, CurDacAdr1);
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, CurDacAdr1);
        
            HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, 0x72, CurDacAdr1);
            HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 1024/32 - 32, CurDacAdr1);

            //
            // Relock if necessary
            //
            if (lock0 == 0)
                HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, CurDacAdr0);
            if (lock1 == 0)
                HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, CurDacAdr1);

        } else {    //if (IsNV11(pDev))
            HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock0, CurDacAdr);
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, CurDacAdr);
        
            HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, 0x72, CurDacAdr);
            HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 5, CurDacAdr);
         
            //
            // Relock if necessary
            //
            if (lock0 == 0)
                HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, CurDacAdr);
        }
        
        return (RM_ERROR);
    }        
}
