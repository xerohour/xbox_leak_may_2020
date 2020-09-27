 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/******************************** ARB Structures ****************************\
*                                                                           *
* Module: ARB_FIFO.H                                                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#ifndef _ARB_FIFO_H
#define _ARB_FIFO_H

#define DEFAULT_GR_LWM 100
#define DEFAULT_VID_LWM 100
#define DEFAULT_GR_BURST_SIZE 256
#define DEFAULT_VID_BURST_SIZE 128


typedef struct {

  U032 crtc1_lwm;    // GR LWM
  U032 crtc2_lwm;    // GR LWM (NV11)
  U032 video_lwm;    // Video LWM

  U032 crtc1_burst_size;
  U032 crtc2_burst_size;
  U032 video_burst_size;

  U032 valid;  // good values

} fifo_info;

typedef struct {
  U032 hres;     //horizontal resolution, for head 1, in pixels
  U032 htotal;   //Total number of horizontal pixels, including blanking and sync pixels.
                //Used to calculate horizontal line period from PClk.
  U032 vres;     //vertical resolution, for head 1, in lines
  U032 refresh;  //refresh rate for head 1, in Hz

  //Same values, but for head 2:
  U032 hres2;
  U032 htotal2;
  U032 vres2;
  U032 refresh2;

  U032 nativeVRes;   //Native vertical resolution of flat panel driven by Head A.  If not displaying to a FP, nativeVRes shoudl equal vres.
  U032 nativeVRes2;  //Native vertical resolution of flat panel driven by Head B.  If not displaying to a FP, nativeVRes2 shoudl equal vres2.

  U032 pclk_khz;
  U032 pclk2_khz;
  U032 mclk_khz;
  U032 nvclk_khz;
  U032 memory_width;   //FBI width, in bits (e.g., 256)
  U032 memory_type;    //0=sdr, 1=ddr

  U032 h2_owns_vs;     //1 = scaler is displaying on head 2.  0 = scaler is displaying on head 1.
  U032 video_source_hres; //number of pixels/texels/samples in the video source horizontally, and
  U032 video_source_vres; //vertically.

//  U032  memory_width;
//  char memory_type;
  //8, 16 or 32 bits per pixel
  U032 pix_bpp;        //color depth on head 1
  U032 pix_bpp_h2;     //color depth on head 2
  //(scaler is always 16 bpp)
//  char pix_bpp;

  //Are these engines in use?  1=yes, 0=no.
  char enable_h1;     //First head
  char enable_h2;     //Second head
  char enable_video;
  char enable_mp;
  U032  ram_config;

  //No longer used (NV11)
  char mem_page_miss;
  char mem_latency; //cas_latency
  char gr_during_vid;
  char mem_aligned;
} sim_state;


//    new NV11
typedef struct {
  U032 hres;
  U032 vres;
  U032 depth;
  U032 refresh;
} screen_t;

typedef struct {
  U032 lwm;
  U032 lwm_min;
  U032 size;
} fifo_params;
  
typedef enum { CRTC_MODE_OLD, CRTC_MODE_NEW } crtc_mode;

typedef struct {
  U032 enable;
  U032 hres;
  U032 vres;
  U032 depth;
  U032 refresh;
  U032 pclk;         //in MHz
  U032 pclk_period;  //in ps (no longer ns!!!)
  U032 hclk_period;  //in ns
  U032 drain_rate;
  crtc_mode mode;
  U032 burst_size;
  U032 min_burst_size;
  U032 max_burst_size;
  U032 cursor_burst_size;
  U032 request_delay;
  U032 nativeVRes;
  fifo_params fifo;
} crtc_t;

typedef struct {
  U032 enable;
  U032 source_hres;
  U032 source_vres;
  U032 depth;
  U032 window_hres;
  U032 window_vres;
  U032 hscale; //now in 1000x units?
  U032 vscale;
  U032 burst_size;
  U032 min_burst_size;
  U032 max_burst_size;
  U032 request_delay;
  U032 drain_rate;
  fifo_params fifo;
  U032 h2_owns_vs;
  U032 max_fifo_size;
  U032 pclk_period;//now in ps, not ns
  U032 bwFactor;
} scaler_t;

typedef enum { round_robin, rtr_interleave, rtr_interrupt, rtr_priority } arb_type;

typedef struct {
  U032 mclk;
  U032 mclk_period; //now in ps, no longer ns
  U032 partitions;
  U032 ddr;
  U032 data_rate;
  U032 page_fault_penalty;
  U032 page_fault_width;
  U032 refresh_penalty;
  U032 arbitration_latency;
  arb_type arbiter;
  U032 pipeline_latency;
  U032 stall_cycles;
  U032 cmdq_depth_rw;
  U032 cmdq_depth_cmd;
  U032 command_latency;
  U032 interrupt_rate;
} mem_t;

typedef struct {
  U032 burst_size;
  U032 request_delay_mclks;
  U032 request_delay;
  U032 latency;
} rtr_t;


#define VIDEO       0
#define GRAPHICS    1
#define MPORT       2
#define ENGINE      3


#define GFIFO_SIZE      320     /* Graphics Fifo Sizes */
#define GFIFO_SIZE_128  256     /* Graphics Fifo Sizes */
#define MFIFO_SIZE      120     /* MP  Fifo Sizes */
#define VFIFO_SIZE      256     /* Video Fifo Sizes */

#define    ABS(a)    (a>0?a:-a)

#endif

