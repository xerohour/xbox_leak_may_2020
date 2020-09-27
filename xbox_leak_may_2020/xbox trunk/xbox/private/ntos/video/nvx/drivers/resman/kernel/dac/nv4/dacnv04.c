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
#include <nv4_ref.h>
#include <nvrm.h>
#include <nv4_hal.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <dma.h>
#include <gr.h>
#include <dac.h>
#include <os.h>
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
// Calculate the closest arbitration values for a given system configuration
//

static
VOID NV04_dacCalculateArbitration
(
    PHALHWINFO pHalHwInfo,
    fifo_info *fifo,
    sim_state  *arb
)
{
    PFBHALINFO      pFbHalInfo = pHalHwInfo->pFbHalInfo;
    PMCHALINFO      pMcHalInfo = pHalHwInfo->pMcHalInfo;
    PVIDEOHALINFO   pVideoHalInfo = pHalHwInfo->pVideoHalInfo;

    int data, pagemiss, cas,width, video_enable, color_key_enable, bpp, align;
    int nvclks, mclks, pclks, vpagemiss, crtpagemiss, vbs;
    int nvclk_fill, us_extra, clwm_mt;
    int found, mclk_extra, mclk_loop, cbs, m1;
    int mclk_freq, pclk_freq, nvclk_freq, mp_enable;
    int us_m, us_n, us_p, video_drain_rate, crtc_drain_rate;
    int vpm_us, us_video, vlwm, video_fill_us, cpm_us, us_crt,clwm;
    int craw, vraw, m2us, us_pipe, p1clk, p2;
    int h_size, v_size;
    int ram_conf;
  
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
    vlwm = 0;

    h_size   = pFbHalInfo->HorizDisplayWidth;
    v_size   = pFbHalInfo->VertDisplayWidth;
    ram_conf = arb->ram_config;

    // NV4 can not support 256 burst
    if (IsNV4_NV04(pMcHalInfo))
        cbs = 128;
    else
        cbs = 256;

    pclks = 2;   // lwm detect.

    nvclks = 2;  // lwm -> sync.
    nvclks += 2; // fbi bus cycles (1 req + 1 busy)
    nvclks += 1; // fbi reqsync

    mclks = 5;  // Fifo
    mclks += 3; // MA0 -> MA2
    mclks += 1; // pad->out
    mclks += cas; // Cas Latency.
    mclks += 1; // pad in
    mclks += 1; // latch data
    mclks += 1; // fifo load
    mclks += 1; // fifo write
  
  
    if ((video_enable) && (arb->memory_width == 128))
        mclk_extra = 3; // Margin of error
    else    
        mclk_extra = 17; // Margin of error

    nvclks += 2; // fifo sync
    nvclks += 1; // read data
    nvclks += 1; // fbi_rdata
    nvclks += 1; // crtfifo load

    if(mp_enable)
        mclks+=4; // Mp can get in with a burst of 8.
        
    // Extra clocks determined by heuristics
    nvclks += 0;
    pclks += 0;
    found = 0;
    while(found != 1) 
    {
        fifo->valid = 1;
        found = 1;
        mclk_loop = mclks+mclk_extra;
        us_m = mclk_loop *1000*1000 / mclk_freq;    // Mclk latency in us
        us_n = nvclks*1000*1000 / nvclk_freq;       // nvclk latency in us
        us_p = nvclks*1000*1000 / pclk_freq;        // nvclk latency in us
        us_pipe = us_m + us_n + us_p;
        us_extra = 0;
        if(video_enable) 
        {
            video_drain_rate = pclk_freq * 2;       // MB/s
            crtc_drain_rate = pclk_freq * bpp/8;    // MB/s

            vpagemiss = 1; // self generating page miss
            vpagemiss += 1; // One higher priority before

            crtpagemiss = 2; // self generating page miss

            vpm_us = (vpagemiss * pagemiss)*1000*1000/mclk_freq;


            if(nvclk_freq * 2 > mclk_freq * width) // nvclk is faster than mclk
                video_fill_us = 256*1000*1000 / 16 / nvclk_freq ;
            else
                video_fill_us = 256*1000*1000 / (8 * width) / mclk_freq;

            us_video = vpm_us + us_m + us_n + us_p + video_fill_us;

            vlwm = us_video * video_drain_rate/(1000*1000);
            vlwm+=2; // fixed point <= float_point - 1.  Fixes that
            vbs = 128;

            //
            // Video tweaking specific to Canopus
            //
            if (pVideoHalInfo->CustomizationCode & CUSTOMER_CANOPUS)
            {
                //
                // Canopus specific code
                //      
                if (IsNV4_NV04(pMcHalInfo))
                {
                    if(bpp == 32)
                    {
                        if(vlwm > 110)
                            vbs = 64;
                        if(vlwm > (256-64))
                            vbs = 32;
                    }
                    else
                    {
                        if(vlwm > 110)
                            vbs = 64;
                        if(vlwm > (256-27)) // 229
                            vbs = 32;
                    }
                }
                else
                {
                    if(bpp == 32)
                    {
                        if(vlwm > 77)
                            vbs = 64;
                        if(vlwm > (256-64)) // 192
                            vbs = 32;
                    }
                    else
                    {
                        if(vlwm > 135)      // 146)
                            vbs = 64;
                        if(vlwm > 201)
                            vbs = 32;
                    }
                }

            } else {
                //
                // Generic code
                //
                if(vlwm > 256 - 64)
                    vbs = 32;
                else if(vlwm > 146) 
                    vbs = 64;
            }        

            if(nvclk_freq * 2 > mclk_freq * width) // nvclk is faster than mclk
                video_fill_us = vbs *1000*1000/ 16 / nvclk_freq ;
            else
                video_fill_us = vbs*1000*1000 / (8 * width) / mclk_freq;

            cpm_us = crtpagemiss  * pagemiss *1000*1000/ mclk_freq;
            us_crt =
              us_video  // Wait for video
              +video_fill_us // Wait for video to fill up
              +cpm_us // CRT Page miss
              +us_m + us_n +us_p // other latency
              ;
              
            clwm = us_crt * crtc_drain_rate/(1000*1000);
            clwm++; // fixed point <= float_point - 1.  Fixes that

            //
            // Video tweaking specific to Canopus
            //
            if (pVideoHalInfo->CustomizationCode & CUSTOMER_CANOPUS)
            {
                if(ram_conf == 0x0003)  
                {
                    clwm -= 32;     // SPECTRA Lite 
                    if(bpp == 32)
                        clwm -= 72; // SPECTRA Lite 
                }
            }

        } 
        else    // !video_enable
        {
        
            crtc_drain_rate = pclk_freq * bpp/8; // bpp * pclk/8

            crtpagemiss = 1; // self generating page miss
            crtpagemiss += 1; // MA0 page miss
            cpm_us = crtpagemiss  * pagemiss *1000*1000/ mclk_freq;
            us_crt =  cpm_us + us_m + us_n + us_p ;
            clwm = us_crt * crtc_drain_rate/(1000*1000);
            clwm++; // fixed point <= float_point - 1.  Fixes that
    
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
                
            //    
            // Finally, a heuristic check when width == 64 bits
            //
            if(width == 1)
            {
                nvclk_fill = nvclk_freq * 8;
                if(crtc_drain_rate * 100 >= nvclk_fill * 102) 
                    clwm = 0xfff; //Large number to fail
                else if (crtc_drain_rate * 100  >= nvclk_fill * 98) 
                {
                    clwm = 512;
                    cbs = 256;
                    us_extra = (cbs * 1000 * 1000)/ (8*width)/mclk_freq ;
                    //if(crtc_drain_rate * 100 >= nvclk_fill * 102) 
                    //    clwm = 0xfff; //Large number to fail
                }
            }         
        }
    
    
        //
        // Overfill check:
        //

        m1 = clwm + cbs -  512; /* Amount of overfill */

        //
        // Video tweaking specific to Canopus
        //
        if (pVideoHalInfo->CustomizationCode & CUSTOMER_CANOPUS)
        {
            p2 = m1 * pclk_freq / mclk_freq;  // pclk cycles to drain
            p2 = p2 * bpp / 8;                // bytes drained.
        }
        else
        {
            // Generic coding
            //      
            m2us = us_pipe + us_extra;
            p1clk = m2us * pclk_freq/(1000*1000); /* pclk cycles to drain */
            p2 = p1clk * bpp / 8; // bytes drained.
        }   
           
        if((p2 < m1) && (m1 > 0)) 
        {
            fifo->valid = 0;
            found = 0;
            if (video_enable)
            {
                if(cbs <= 32) 
                    found = 1;      // Can't adjust anymore!
                else
                    cbs = cbs/2;    // reduce the burst size
            } 
            else
            {
            
                //
                // Video tweaking specific to Canopus
                //
                if (pVideoHalInfo->CustomizationCode & CUSTOMER_CANOPUS)
                {
                    if(mclk_extra ==0)   
                        found = 1; // Can't adjust anymore!
                    mclk_extra--;
                }
                else
                {         
                    //
                    // Generic coding
                    //   
                    if(mclk_extra ==0)   
                    {
                        if(cbs <= 32) 
                            found = 1;      // Can't adjust anymore!
                        else
                            cbs = cbs/2;    // reduce the burst size
                    } 
                    else 
                        mclk_extra--;
                }            
            }            
        }
        else if (video_enable)
        {

            //
            // Canopus specific code
            //
            if (IsNV4_NV04(pMcHalInfo) && (pVideoHalInfo->CustomizationCode & CUSTOMER_CANOPUS))
            {
                if ((clwm > 621) || (vlwm > 255)) 
                {
                    fifo->valid = 0;
                    found = 0;
                    if(mclk_extra == 0)   
                        found = 1; // Can't adjust anymore!
                    mclk_extra--;
                }
            }
            else
            {
                //
                // Generic coding
                //
                if ((clwm > 511) || (vlwm > 255)) 
                {
                    fifo->valid = 0;
                    found = 0;
                    if(mclk_extra <= 5)   
                        found = 1; // Can't adjust anymore!
                    mclk_extra--;
                }
            }            
            
        } 
        else 
        {  
            if (clwm > 519)
            { // Have some margin
                fifo->valid = 0;
                found = 0;
                if(mclk_extra ==0)   
                    found = 1; // Can't adjust anymore!
                mclk_extra--;
            }
        }
        
        craw = clwm;
        vraw = vlwm;
        //if(clwm < 256) clwm = 256;
        if (clwm < (512-cbs)) 
            clwm = 512-cbs;

        //
        // Video tweaking specific to Canopus
        //
        if (pVideoHalInfo->CustomizationCode & CUSTOMER_CANOPUS)
        {    
            if (IsNV4_NV04(pMcHalInfo))
            {
                if (vlwm < 128)
                {
                    if (bpp == 32)
                    {
                        if (h_size == 1408)
                        {
                            vlwm = 176;
                            vbs = 64;
                        }
                        else if (h_size == 1024)
                        {
                            vlwm = 144;
                            vbs = 64;
                        }
                        else if (h_size < 1024)
                        {
                            vlwm = 128-16;
                        }
                        else
                        {
                            vlwm = 160;
                            vbs = 64;
                        }
                    }
                    else
                        vlwm = 128;
                }
            }
            else
            {
                if (vlwm < 128)
                {
                    if (bpp == 32)  
                    {
                        if (h_size >= 1152)
                        {
                            vlwm = 176;
                            vbs = 64;
                        }
                        else
                            vlwm = 144-16;
                    }
                    else
                        vlwm = 128;
                }

                if ((!video_enable) && (ram_conf == 0x0003))    
                {
                    if (bpp == 32)
                    {
                        if ((h_size >= 1024) && (h_size <= 1408))
                            clwm += 88;     // SPECTRA Lite 
                        else if (h_size == 960)
                            clwm += 48;
                        else
                            clwm += 32;
                    }
                    else if ((bpp == 16) && (h_size == 1920) && (v_size == 1440))
                    {
                        clwm += 8;
                    }
                }

                if (cbs == 256)    
                {
                    if (clwm > 271)    
                        clwm -= 16;
                    if ((!video_enable) && (ram_conf == 0x0003))    
                    {
                        if (clwm > 400)     // SP Lite
                            clwm = 400;
                    }
                    else
                    {
                        if (clwm > 384)     
                            clwm = 384; // 400;
                    }
                }

                // For 1920x1440x32 of Spectra5400PE
                if ((!video_enable) && (ram_conf != 0x0003) && (clwm < 384) &&
                    (h_size == 1920) && (v_size == 1440) && (bpp == 32))
                {
                    clwm = 384;
                }

            }
            
            if(clwm > 511)
                clwm = 511;
        } 
        else
        {    
            //
            // Generic coding
            //
            if(vlwm < 128) 
                vlwm = 128;
        }        
    
    
        if(vlwm < 128) vlwm = 128;
    
        data = (int)(clwm);
        //  printf("CRT LWM: %f bytes, prog: 0x%x, bs: 256\n", clwm, data );
        fifo->crtc1_lwm = data;   fifo->crtc1_burst_size = cbs;
        //    fifo->craw = craw;

        //
        // Video tweaking specific to Canopus
        //
        if (pVideoHalInfo->CustomizationCode & CUSTOMER_CANOPUS)
        {    
            if (IsNV4_NV04(pMcHalInfo))
                data = (int)((vlwm+24));
            else
                data = (int)((vlwm+24));        // 15));

            if (data > 255)
                data = 255;
        }
        else      
        {
            // Generic coding    
            //
            data = (int)((vlwm+15));
            
            // Preset ranges for vlwm and vbs based on calculated vlwm
            if (vlwm > 256-32) 
            {
                data = 256;
                vbs = 32;
            } 
            else if (vlwm > 256-64) 
            {
                data = 256-32+16;
                vbs = 32;
            } 
            else if (vlwm > 256-128) 
            {
                data = 256-64+16;
                vbs = 64;
            } 
            else 
            {
                data = 256-128+16;
                vbs = 128;
            }
            
        }        

        //  printf("VID LWM: %f bytes, prog: 0x%x, bs: %d\n, ", vlwm, data, vbs );
        fifo->video_lwm = data;  
        fifo->video_burst_size = vbs;
    }
}


static
void NV04_dacCalculateArbitration_SMA
(
    PHALHWINFO pHalHwInfo,
    fifo_info *fifo,
    sim_state  *arb
)
{
    PFBHALINFO  pFbHalInfo = pHalHwInfo->pFbHalInfo;

    int data, pagemiss, cas,width, video_enable, color_key_enable, bpp, align;
    int nvclks, mclks, pclks, vpagemiss, crtpagemiss, vbs;
    int nvclk_fill, us_extra, clwm_mt;
    int found, mclk_extra, mclk_loop, cbs, m1;
    int mclk_freq, pclk_freq, nvclk_freq, mp_enable;
    int us_m, us_n, us_p, video_drain_rate, crtc_drain_rate;
    int vpm_us, us_video, vlwm, video_fill_us, cpm_us, us_crt,clwm;
    int craw, vraw, m2us, us_pipe, p1clk, p2;
    int h_size, v_size;
    int ram_conf;
  
  
    fifo->valid = 1;
    pclk_freq = arb->pclk_khz; // freq in KHz
    //mclk_freq = arb->mclk_khz;
    mclk_freq = 100000;
    nvclk_freq = arb->nvclk_khz;
    //pagemiss = arb->mem_page_miss;
    pagemiss = 7;
    //cas = arb->mem_latency;
    cas = 3;
    width = 1;
    video_enable = arb->enable_video;
    color_key_enable = arb->gr_during_vid;
    bpp = arb->pix_bpp;
    align = arb->mem_aligned;
    mp_enable = arb->enable_mp;
    clwm = 0;
    vlwm = 0;

    h_size   = pFbHalInfo->HorizDisplayWidth;
    v_size   = pFbHalInfo->VertDisplayWidth;
    ram_conf = arb->ram_config;

    // NV4 can not support 256 burst
    //if (IsNV4(pDev))
    if (/*IsNV4(pDev)*/ 1)
        cbs = 128;
    else
        cbs = 256;

    pclks = 2;   // lwm detect.

    nvclks = 2;  // lwm -> sync.
    nvclks += 2; // fbi bus cycles (1 req + 1 busy)
    nvclks += 1; // fbi reqsync

    mclks = 5;  // Fifo
    mclks += 3; // MA0 -> MA2
    mclks += 4; // prev burst
    mclks += 2; // ALI arb
    mclks += 1; // pad->out
    mclks += cas; // Cas Latency.
    mclks += 1; // pad in
    mclks += 2; // ALI rd pipe
    mclks += 1; // latch data
    mclks += 1; // fifo load
    mclks += 1; // fifo write
  
  
    if ((video_enable) && (arb->memory_width == 128))
        mclk_extra = 12; // Margin of error
    else    
        mclk_extra = 17; // Margin of error

    nvclks += 2; // fifo sync
    nvclks += 1; // read data
    nvclks += 1; // fbi_rdata
    nvclks += 1; // crtfifo load

    if(mp_enable)
        mclks+=4; // Mp can get in with a burst of 8.
        
    // Extra clocks determined by heuristics
    nvclks += 0;
    pclks += 0;
    found = 0;
    while(found != 1) 
    {
        fifo->valid = 1;
        found = 1;
        mclk_loop = mclks+mclk_extra;
        us_m = mclk_loop *1000*1000 / mclk_freq; // Mclk latency in us
        us_n = nvclks*1000*1000 / nvclk_freq;// nvclk latency in us
        us_p = nvclks*1000*1000 / pclk_freq;// nvclk latency in us
        us_pipe = us_m + us_n + us_p;
        us_extra = 0;
        if(video_enable) 
        {
            video_drain_rate = pclk_freq * 2; // MB/s
            crtc_drain_rate = pclk_freq * bpp/8; // MB/s

            vpagemiss = 1; // self generating page miss
            vpagemiss += 1; // One higher priority before

            crtpagemiss = 2; // self generating page miss

            vpm_us = (vpagemiss * pagemiss)*1000*1000/mclk_freq;


            if(nvclk_freq * 2 > mclk_freq * width) // nvclk is faster than mclk
                video_fill_us = 256*1000*1000 / 16 / nvclk_freq ;
            else
                video_fill_us = 256*1000*1000 / (8 * width) / mclk_freq;

            us_video = vpm_us + us_m + us_n + us_p + video_fill_us;

            vlwm = us_video * video_drain_rate/(1000*1000);
            vlwm+=2; // fixed point <= float_point - 1.  Fixes that
            vbs = 128;

            //
            // Generic code
            //
            if(vlwm > 256 - 64)
                vbs = 32;
            else if(vlwm > 146) 
                vbs = 64;

            if(nvclk_freq * 2 > mclk_freq * width) // nvclk is faster than mclk
                video_fill_us = vbs *1000*1000/ 16 / nvclk_freq ;
            else
                video_fill_us = vbs*1000*1000 / (8 * width) / mclk_freq;

            cpm_us = crtpagemiss  * pagemiss *1000*1000/ mclk_freq;
            us_crt =
              us_video  // Wait for video
              +video_fill_us // Wait for video to fill up
              +cpm_us // CRT Page miss
              +us_m + us_n +us_p // other latency
              ;
              
            clwm = us_crt * crtc_drain_rate/(1000*1000);
            clwm++; // fixed point <= float_point - 1.  Fixes that

        } 
        else    // !video_enable
        {
            crtc_drain_rate = pclk_freq * bpp/8; // bpp * pclk/8

            crtpagemiss = 1; // self generating page miss
            crtpagemiss += 1; // MA0 page miss
            cpm_us = crtpagemiss  * pagemiss *1000*1000/ mclk_freq;
            us_crt =  cpm_us + us_m + us_n + us_p ;
            clwm = us_crt * crtc_drain_rate/(1000*1000);
            clwm++; // fixed point <= float_point - 1.  Fixes that
    
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
                
            //    
            // Finally, a heuristic check when width == 64 bits
            //
            if(width == 1)
            {
                nvclk_fill = nvclk_freq * 8;
                if(crtc_drain_rate * 100 >= nvclk_fill * 102) 
                    clwm = 0xfff; //Large number to fail
                else if (crtc_drain_rate * 100  >= nvclk_fill * 98) 
                {
                    clwm = 512;
                    cbs = 256;
                    us_extra = (cbs * 1000 * 1000)/ (8*width)/mclk_freq ;
                    //if(crtc_drain_rate * 100 >= nvclk_fill * 102) 
                    //    clwm = 0xfff; //Large number to fail
                }
        }         
        }
    
        //
        // Overfill check:
        //

        m1 = clwm + cbs -  512; /* Amount of overfill */

        // Generic coding
        //      
        m2us = us_pipe + us_extra;
        p1clk = m2us * pclk_freq/(1000*1000); /* pclk cycles to drain */
        p2 = p1clk * bpp / 8; // bytes drained.
           
        if((p2 < m1) && (m1 > 0)) 
        {
            fifo->valid = 0;
            found = 0;
            if (video_enable)
            {
                if(cbs <= 32) 
                    found = 1;      // Can't adjust anymore!
                else
                    cbs = cbs/2;    // reduce the burst size
            } 
            else
            {
            
                //
                // Generic coding
                //   
                if(mclk_extra ==0)   
                {
                    if(cbs <= 32) 
                        found = 1;      // Can't adjust anymore!
                    else
                        cbs = cbs/2;    // reduce the burst size
                } 
                else 
                    mclk_extra--;
            }            
        }
        else if (video_enable)
        {

            //
            // Generic coding
            //
            if ((clwm > 511) || (vlwm > 255)) 
            {
                fifo->valid = 0;
                found = 0;
                if(mclk_extra <= 5)   
                    found = 1; // Can't adjust anymore!
                mclk_extra--;
            }
            
        } 
        else 
        {  
            if (clwm > 519)
            { // Have some margin
                fifo->valid = 0;
                found = 0;
                if(mclk_extra ==0)   
                    found = 1; // Can't adjust anymore!
                mclk_extra--;
            }
        }
        
        craw = clwm;
        vraw = vlwm;
        //if(clwm < 256) clwm = 256;
        if (clwm < (512-cbs)) 
            clwm = 512-cbs;

        //
        // Generic coding
        //
        if(vlwm < 128) 
            vlwm = 128;
    
    
        if(vlwm < 128) vlwm = 128;
    
        data = (int)(clwm);
        //  printf("CRT LWM: %f bytes, prog: 0x%x, bs: 256\n", clwm, data );
        fifo->crtc1_lwm = data;   fifo->crtc1_burst_size = cbs;
        if (data > 511) fifo->valid = 0;
        //    fifo->craw = craw;

        //
        // Video tweaking 
        //
        data = (int)((vlwm+15));
        
        // Preset ranges for vlwm and vbs based on calculated vlwm
        if (vlwm > 256-32) 
        {
            data = 256;
            vbs = 32;
        } 
        else if (vlwm > 256-64) 
        {
            data = 256-32+16;
            vbs = 32;
        } 
        else if (vlwm > 256-128) 
        {
            data = 256-64+16;
            vbs = 64;
        } 
        else 
        {
            data = 256-128+16;
            vbs = 128;
        }
            

        if (data > 255) fifo->valid = 0;

        //  printf("VID LWM: %f bytes, prog: 0x%x, bs: %d\n, ", vlwm, data, vbs );
        fifo->video_lwm = data;  
        fifo->video_burst_size = vbs;
    }
}


typedef struct tagNV04_DacAGPClockEntry
{
    U032 frequency;
    U032 setting;
} NV04_DacAGPClockEntry;

NV04_DacAGPClockEntry NV04_dacAGPClockTable[] =
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
RM_STATUS nvHalDacAlloc_NV04(VOID *);
RM_STATUS nvHalDacFree_NV04(VOID *);
RM_STATUS nvHalDacControl_NV04(VOID *);
RM_STATUS nvHalDacUpdateStartAddress_NV04(VOID *);
RM_STATUS nvHalDacProgramMClk_NV04(VOID *);
RM_STATUS nvHalDacProgramNVClk_NV04(VOID *);
RM_STATUS nvHalDacProgramPClk_NV04(VOID *);
RM_STATUS nvHalDacProgramCursorImage_NV04(VOID *);
RM_STATUS nvHalDacGetRasterPosition_NV04(VOID *);
RM_STATUS nvHalDacValidateArbSettings_NV04(VOID *arg);
RM_STATUS nvHalDacUpdateArbSettings_NV04(VOID *arg);

//
// nvHalDacControl
//
RM_STATUS
nvHalDacControl_NV04(VOID *arg)
{
    PDACCONTROLARG_000 pDacControlArg = (PDACCONTROLARG_000)arg;
    PHALHWINFO  pHalHwInfo;
    PDACHALINFO pDacHalInfo;
    PFBHALINFO  pFbHalInfo;
    PMCHALINFO  pMcHalInfo;
    PVIDEOHALINFO pVideoHalInfo;
    PPROCESSORHALINFO   pProcessorHalInfo;
    PDACHALINFO_NV04    pDacHalPvtInfo;

    //
    // Verify interface revision.
    //
    if (pDacControlArg->id != DAC_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacControlArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pFbHalInfo = pHalHwInfo->pFbHalInfo;
    pMcHalInfo = pHalHwInfo->pMcHalInfo;
    pVideoHalInfo = pHalHwInfo->pVideoHalInfo;
    pProcessorHalInfo = pHalHwInfo->pProcessorHalInfo;
    pDacHalPvtInfo = (PDACHALINFO_NV04)pHalHwInfo->pDacHalPvtInfo;

    switch (pDacControlArg->cmd)
    {
        case DAC_CONTROL_INIT:
            //
            // Set bus width.
            //
            if (REG_RD_DRF(_PFB, _BOOT_0, _RAM_WIDTH_128) == NV_PFB_BOOT_0_RAM_WIDTH_128_ON)
                pDacHalInfo->InputWidth = 128;
            else
                pDacHalInfo->InputWidth = 64;

            //
            // Initialize video clock.
            //
            pDacHalInfo->VClk = 0;

            //
            // Determine the maximum pixel clock for 8bpp, 16bpp, and 32bpp for this configuration.
            //
            // This will rely on silicon DAC speed, memory bandwidth, and arbitration limits.
            //
            // For now, hardcode the known values.
            //
            if (IsNV4_NV04(pMcHalInfo))
            {
                pDacHalInfo->CrtcInfo[0].PCLKLimit8bpp  = 250000000;
                pDacHalInfo->CrtcInfo[0].PCLKLimit16bpp = 250000000;
                pDacHalInfo->CrtcInfo[0].PCLKLimit32bpp = 215000000;
            }
            else if (IsNV5_NV04(pMcHalInfo))
            {
                switch (REG_RD_DRF(_PBUS, _PCI_NV_0, _DEVICE_ID_FUNC))
                {
                    // Std TNT2
                    case NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC_VGA:
                    // Std Ultra
                    case NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC_ALT1:
                    // Std TNT2 Model64
                    case NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC_LC1:
                        pDacHalInfo->CrtcInfo[0].PCLKLimit8bpp  = 300000000;
                        pDacHalInfo->CrtcInfo[0].PCLKLimit16bpp = 300000000;
                        pDacHalInfo->CrtcInfo[0].PCLKLimit32bpp = 250000000;
                        break;

                    // Std Vanta
                    case NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC_LC0:
                        pDacHalInfo->CrtcInfo[0].PCLKLimit8bpp  = 250000000;
                        pDacHalInfo->CrtcInfo[0].PCLKLimit16bpp = 250000000;
                        pDacHalInfo->CrtcInfo[0].PCLKLimit32bpp = 203000000;
                        break;

                    // Default
                    default:
                        pDacHalInfo->CrtcInfo[0].PCLKLimit8bpp  = 250000000;
                        pDacHalInfo->CrtcInfo[0].PCLKLimit16bpp = 250000000;
                        pDacHalInfo->CrtcInfo[0].PCLKLimit32bpp = 215000000;
                        break;
                } 

                // 
                // If we're running in SMA mode, drop down the limits accordingly
                //
                if (pFbHalInfo->RamType == BUFFER_CRUSH_UMA)
                {
                    //
                    // Determine how much bandwidth we probably have to system memory
                    //
                    if (pProcessorHalInfo->SystemMemoryFreq == 133000000)
                    {
                        pDacHalInfo->CrtcInfo[0].PCLKLimit8bpp  = 300000000;
                        pDacHalInfo->CrtcInfo[0].PCLKLimit16bpp = 300000000;
                        pDacHalInfo->CrtcInfo[0].PCLKLimit32bpp = 162000000;
                    }
                    else if (pProcessorHalInfo->SystemMemoryFreq == 100000000)
                    {
                        pDacHalInfo->CrtcInfo[0].PCLKLimit8bpp  = 250000000;
                        pDacHalInfo->CrtcInfo[0].PCLKLimit16bpp = 250000000;
                        pDacHalInfo->CrtcInfo[0].PCLKLimit32bpp = 126000000;
                    }
                    else // (pProcessorHalInfo->SystemMemoryFreq == 66000000)
                    {
                        pDacHalInfo->CrtcInfo[0].PCLKLimit8bpp  = 162000000;
                        pDacHalInfo->CrtcInfo[0].PCLKLimit16bpp = 162000000;
                        pDacHalInfo->CrtcInfo[0].PCLKLimit32bpp =  85000000;
                    }
                }

            }
            else
            {
                DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: unknown chip; using pixelclock defaults\n");
                // defaults
                pDacHalInfo->CrtcInfo[0].PCLKLimit8bpp  = 250000000;
                pDacHalInfo->CrtcInfo[0].PCLKLimit16bpp = 250000000;
                pDacHalInfo->CrtcInfo[0].PCLKLimit32bpp = 215000000;
            }
            pDacHalInfo->CrtcInfo[0].RegOffset = 0x00000000;

            //
            // All display types allowed.
            //
            pDacHalInfo->CrtcInfo[0].DisplayTypesAllowed =
                DISPLAYTYPEBIT(DISPLAY_TYPE_MONITOR) |
                DISPLAYTYPEBIT(DISPLAY_TYPE_TV) |
                DISPLAYTYPEBIT(DISPLAY_TYPE_DUALSURFACE) |
                DISPLAYTYPEBIT(DISPLAY_TYPE_FLAT_PANEL);

            //
            // No downscaling on nv4 and family...
            // 
            pVideoHalInfo->OverlayMaxDownscale_768  = 1;
            pVideoHalInfo->OverlayMaxDownscale_1280 = 1;
            pVideoHalInfo->OverlayMaxDownscale_1920 = 1;

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
nvHalDacAlloc_NV04(VOID *arg)
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
nvHalDacFree_NV04(VOID *arg)
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
nvHalDacSetStartAddr_NV04(VOID *arg)
{
    PDACSETSTARTADDRARG_000 pDacSetStartAddrArg = (PDACSETSTARTADDRARG_000)arg;
    PHALHWINFO  pHalHwInfo;
    PDACHALINFO pDacHalInfo;
    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pDacHalObj;
    U032 StartAddress = pDacSetStartAddrArg->startAddr;
    U032 prev_bit24, curr_bit24;
    U008 lock, crtc_index, temp;
    U032 CurDacAdr; // must be defined for CRTC_RD macro & must point to pDacHalInfo
    //
    // Verify interface revision.
    //
    if (pDacSetStartAddrArg->id != DAC_SET_START_ADDR_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacSetStartAddrArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pDacHalObj = (PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT)pDacSetStartAddrArg->pHalObjInfo;
    CurDacAdr = pDacHalInfo->CrtcInfo[pDacHalObj->Head].RegOffset;
    
    // Only NV5 needs special treatment regarding PCRTC_START
    if (!IsNV5_NV04(pHalHwInfo->pMcHalInfo))
    {
        REG_WR32(NV_PCRTC_START, StartAddress);
        return (RM_OK);
    }

    //
    // NV5 Bug. Eventhough bit24 should be writable in PCRTC_START, it's not.
    // So, if we need to change its value (i.e. bit24 goes from 0->1 or 1->0),
    // then we'll set/clear bit24 value via the CRE_HEB CRTC reg.
    //
    prev_bit24 = REG_RD32(NV_PCRTC_START) & 0x1000000;
    curr_bit24 = StartAddress & 0x1000000;

    // For NV5, this will properly write bits 23:0
    REG_WR32(NV_PCRTC_START, StartAddress);
    if (prev_bit24 ^ curr_bit24)
    {
        // Save the current CRTC index
        if (REG_RD08(NV_PRMVIO_MISC__READ) & 0x01)          // color or mono?
            crtc_index = REG_RD08(NV_PRMCIO_CRX__COLOR);    // save crtc index
        else
            crtc_index = REG_RD08(NV_PRMCIO_CRX__MONO);     // save crtc index

        // Unlock CRTC extended regs (following is from ReadCRTCLock)
        // lock = ReadCRTCLock(pDev);
        CRTC_READ(NV_CIO_SR_LOCK_INDEX, temp);
        switch (temp)
        {
            case 0x03:
                lock = 0x57;
                break;
            case 0x01:
                lock = 0x75;
                break;
            case 0x00:
                lock = 0x99;
                break;
            default:
                lock = temp;
                break;
        }
        CRTC_WRITE(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE);

        // bit6 corresponds to StartAddress bit24. All other
        // bits were successfully written via PCRTC_START.
        CRTC_READ(NV_CIO_CRE_HEB__INDEX, temp);
        temp &= 0xBF;                           // mask off bit6
        temp |= ((curr_bit24 >> 24) << 6);      // set/clear bit6
        CRTC_WRITE(NV_CIO_CRE_HEB__INDEX, temp);

        // Relock extended regs
        CRTC_WRITE(NV_CIO_SR_LOCK_INDEX, lock);
        // Restore index
        if (REG_RD08(NV_PRMVIO_MISC__READ) & 0x01)
            REG_WR08(NV_PRMCIO_CRX__COLOR, crtc_index);     // restore crtc index
        else
            REG_WR08(NV_PRMCIO_CRX__MONO, crtc_index);      // restore crtc index
    }

    return (RM_OK);
}

//  Read extended CRTC lock register and return corresponding write value
static U008 ReadCRTCLock_NV04
(
    PHALHWINFO pHalHwInfo,
    U032    Head
)
{
    U008 data;

    HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, data, Head);
    switch (data)
    {
        case 0x03:
            data = 0x57;
            break;
        case 0x01:
            data = 0x75;
            break;
        case 0x00:
            data = 0x99;
            break;
    }
    return data;
}

//
// nvHalDacProgramMClk_NV04 - Program MCLK based on the value in pDev->Dac.HalInfo.MClk
//
// This routine uses the value in pDev->Dac.HalInfo.MClk (in Hz) to calculate
// M, N, and P values.  Those values are then stored in the pDev->Dac
// structure.  The hardware is then programmed with the new M, N, and P
// values in a manner that is safe for the PLL.
//
RM_STATUS
nvHalDacProgramMClk_NV04(VOID *arg)
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
    U032 temp;
    U032 oldMClk;
    U032 oldCyclesPerRefresh;
    U032 oldPeriod;
    U032 newCyclesPerRefresh;
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

        // Read the register to give the PLL enough time to adjust
        temp = REG_RD32( NV_PRAMDAC_MPLL_COEFF );
        temp = REG_RD32( NV_PRAMDAC_MPLL_COEFF );
        temp = REG_RD32( NV_PRAMDAC_MPLL_COEFF );
        temp = REG_RD32( NV_PRAMDAC_MPLL_COEFF );
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
    oldCyclesPerRefresh = REG_RD_DRF( _PFB, _DEBUG_0, _REFRESH_COUNTX64 ) * 64;

    // Determine original refresh period
    oldPeriod = oldCyclesPerRefresh * 1024 / ( oldMClk / 1000 );
    
    // Now using the new MCLK, figure out the new cycles per refresh
    newCyclesPerRefresh = ( pDacHalInfo->MClk / 1000 ) * oldPeriod / 1024;

    // Store the new value
    FLD_WR_DRF_NUM( _PFB, _DEBUG_0, _REFRESH_COUNTX64, newCyclesPerRefresh / 64 );
    
    return status;
}

//
// nvHalDacProgramNVClk_NV04 - Program NVCLK based on the value in pDev->Dac.HalInfo.NVClk
//
// This routine uses the value in pDev->Dac.HalInfo.NVClk (in Hz) to calculate
// M, N, and P values.  Those values are then stored in the pDev->Dac
// structure.  The hardware is then programmed with the new M, N, and P
// values in a manner that is safe for the PLL.
//
RM_STATUS
nvHalDacProgramNVClk_NV04(VOID *arg)
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
    U032 temp;
    U032 i;
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

        // Read the register to give the PLL enough time to adjust
        temp = REG_RD32( NV_PRAMDAC_NVPLL_COEFF );
        temp = REG_RD32( NV_PRAMDAC_NVPLL_COEFF );
        temp = REG_RD32( NV_PRAMDAC_NVPLL_COEFF );
        temp = REG_RD32( NV_PRAMDAC_NVPLL_COEFF );
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
    while ( ( pDacHalInfo->NVClk <= NV04_dacAGPClockTable[ i ].frequency ) &&
        ( i < sizeof( NV04_dacAGPClockTable ) / sizeof( NV04_dacAGPClockTable[ 0 ] ) - 1 ) )
    {
        i++;
    }

    FLD_WR_DRF_NUM( _PBUS, _DEBUG_3, _AGP_4X_NVCLK, NV04_dacAGPClockTable[ i ].setting );
    
    return status;
}

//
// nvHalDacProgramPClk_NV04 - Program PCLK based on the value in pDev->Dac.PClk
//
// This routine uses the value in pDev->Dac.PClk (in Hz) to calculate
// M, N, and P values.  Those values are then stored in the pDev->Dac
// structure.  The hardware is then programmed with the new M, N, and P
// values in a manner that is safe for the PLL.
//
RM_STATUS
nvHalDacProgramPClk_NV04(VOID *arg)
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
// nvHalDacProgramCursorImage_NV04
//
RM_STATUS
nvHalDacProgramCursorImage_NV04(VOID *arg)
{
    PDACPROGRAMCURSORARG_000 pDacProgramCursorImgArg = (PDACPROGRAMCURSORARG_000) arg;
    PHALHWINFO  pHalHwInfo;
    PDACHALINFO pDacHalInfo;
    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pDacHalObj;
    U032        CurDacAdr; // must be defined for CRTC_RD macro & must point to pDacHalInfo

    U008    cr31, cr2f;
    U008    lock;

    //
    // Verify interface revision.
    //
    if (pDacProgramCursorImgArg->id != DAC_PROGRAMCURSOR_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDacProgramCursorImgArg->pHalHwInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;
    pDacHalObj = (PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT) pDacProgramCursorImgArg->pHalObjInfo;
    CurDacAdr = pDacHalInfo->CrtcInfo[pDacHalObj->Head].RegOffset;

    // Unlock CRTC extended regs
    lock = ReadCRTCLock_NV04(pHalHwInfo, pDacHalObj->Head);
    HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, CurDacAdr);

    // On chips < NV10, the cursor image is always assumed to be a 32x32 image with a color
    // format LE_ROP1R5G5B5. So we ignore the width, height and colorFormat parameters.

    // Mark the Address Space Indicator (bit 7 in NV_CIO_CRE_HCUR_ADDR0_INDEX) 
    // to indicate where the cursor image is based on the param "asi".

    // If the address space indicator says it is in Instance Memory, the startAddr is in terms of 
    // paragraphs instead of bytes. Adjust it to bytes.
    if (pDacProgramCursorImgArg->asi == DAC_ASI_INSTMEM)
    {
        pDacProgramCursorImgArg->startAddr <<= 4;
    }

    // The startAddr is programmed into two config registers as follows:
    // cr2f<0>   = startAddr<24>
    // cr30<6:0> = startAddr<23:17>
    // cr31<7:2> = startAddr<16:11>
    // startAddr<10:0> is assumed to be 0.
    HAL_CRTC_RD(NV_CIO_CRE_HCUR_ADDR2_INDEX, cr2f, CurDacAdr);
    cr2f &= 0xFE;
    cr2f |= (U008)((pDacProgramCursorImgArg->startAddr >> 24) & 0x01);
    HAL_CRTC_WR(NV_CIO_CRE_HCUR_ADDR2_INDEX, cr2f, CurDacAdr);
    
    HAL_CRTC_WR(NV_CIO_CRE_HCUR_ADDR0_INDEX, 
               (U008) ( ((pDacProgramCursorImgArg->startAddr >> 17) & 0x7f) | (pDacProgramCursorImgArg->asi << DEVICE_BASE(NV_CIO_CRE_HCUR_ASI)) ),
               CurDacAdr);
            
    // store startAddr bits 16:11 in cr31 bits 7:2
    HAL_CRTC_RD(NV_CIO_CRE_HCUR_ADDR1_INDEX, cr31, CurDacAdr);
    cr31 &= 0x03; // Clear bits 7:2
    cr31 |= (U008)((pDacProgramCursorImgArg->startAddr >> 9) & 0xfc); // Set the new start address
    HAL_CRTC_WR(NV_CIO_CRE_HCUR_ADDR1_INDEX, cr31, CurDacAdr);

    // Restore original state of CRTC lock register
    HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, CurDacAdr);
    
    return RM_OK;
}

//
// nvHalDacGetRasterPosition_NV04
//
RM_STATUS
nvHalDacGetRasterPosition_NV04(VOID *arg)
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
    pDacRasterPosArg->result = (U016) REG_RD_DRF(_PCRTC, _RASTER, _POSITION);

    return (RM_OK);
}

//
// Attempt to validate the given parameters will work with the current configuration
//       
//  Return 0  if not capable
//  Return ~0 if valid  
//
RM_STATUS
nvHalDacValidateArbSettings_NV04(VOID *arg)
{
    PDACVALIDATEARBSETTINGSARG_000    pDacValidateArbSettingsArg = (PDACVALIDATEARBSETTINGSARG_000) arg;
    PHALHWINFO          pHalHwInfo;
    PDACHALINFO         pDacHalInfo;
    PFBHALINFO          pFbHalInfo;
    PMCHALINFO          pMcHalInfo;
    PPROCESSORHALINFO   pProcessorHalInfo;
    PVIDEOHALINFO       pVideoHalInfo;

    fifo_info fifo_data;
    sim_state sim_data;
    U032 pclk_limit;

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
    // Although the memory arbiter may allow video accesses, we can't exceed
    // the video pipeline's circuit limits
    //
    // Hardcode NV4 to 163MHz.  32bpp video may fail due to arbiter failure
    // even under this rate, but that check comes next.
    //
    if (IsNV4_NV04(pMcHalInfo))
        if ((pDacValidateArbSettingsArg->video_enabled) &&
            (pDacValidateArbSettingsArg->vclk > 163000000))
        {
            pDacValidateArbSettingsArg->result = 0;
            return (RM_OK);
        }
    //
    // Hardcode NV5 to 235MHz.  32bpp video may fail due to arbiter failure
    // even under this rate, but that check comes next.
    //
    if (IsNV5_NV04(pMcHalInfo))
        if ((pDacValidateArbSettingsArg->video_enabled) &&
            (pDacValidateArbSettingsArg->vclk > 235000000))
        {
            pDacValidateArbSettingsArg->result = 0;
            return (RM_OK);
        }
    //
    // Hardcode NV0A 8/16bpp to a 206MHz limit, or in the UMA mode, based on memory speed
    //
    // NOTE This is just for Crush level integrated products
    //
    if (IsNV0A_NV04(pMcHalInfo))
    {
        //
        // Never let Crush enable video above 206MHz
        //
        if ((pDacValidateArbSettingsArg->video_enabled) &&
            (pDacValidateArbSettingsArg->bpp < 32) &&
            (pDacValidateArbSettingsArg->vclk > 206000000))
        {
            pDacValidateArbSettingsArg->result = 0;
            return (RM_OK);
        }

        //
        // Further restrictions for UMA configurations due to limited bandwidth
        //
        if (pFbHalInfo->RamType == BUFFER_CRUSH_UMA)
        {
            // Is this 133MHz memory?
            if (pProcessorHalInfo->SystemMemoryFreq == 133000000)
            {
                if ((pDacValidateArbSettingsArg->bpp == 32) &&
                    (pDacValidateArbSettingsArg->vclk > 70000000))
                {
                    pDacValidateArbSettingsArg->result = 0;
                    return (RM_OK);
                }
            }
            // or 100MHz memory?
            else if (pProcessorHalInfo->SystemMemoryFreq == 100000000)
            {
                if ((pDacValidateArbSettingsArg->bpp < 32) &&
                    (pDacValidateArbSettingsArg->vclk > 110000000))
                {
                    pDacValidateArbSettingsArg->result = 0;
                    return (RM_OK);
                }           

                if ((pDacValidateArbSettingsArg->bpp == 32) &&
                    (pDacValidateArbSettingsArg->vclk > 70000000))
                {
                    pDacValidateArbSettingsArg->result = 0;
                    return (RM_OK);
                }      
            }
            else // we must be using 66MHz memory or lower
            {
                if ((pDacValidateArbSettingsArg->bpp < 32) &&
                    (pDacValidateArbSettingsArg->vclk > 60000000))
                {
                    pDacValidateArbSettingsArg->result = 0;
                    return (RM_OK);
                }      

                if ((pDacValidateArbSettingsArg->bpp == 32) &&
                    (pDacValidateArbSettingsArg->vclk > 53000000))
                {
                    pDacValidateArbSettingsArg->result = 0;
                    return (RM_OK);
                }      
            }
        }
    }

    //
    // Current NV10 video has no limit in this range
    //            
            
    //
    // Build the sim table using current system settings
    //
    sim_data.pix_bpp        = (char) pDacValidateArbSettingsArg->bpp;
    sim_data.enable_video   = pDacValidateArbSettingsArg->video_enabled ? 1 : 0;
    sim_data.enable_mp      = pDacValidateArbSettingsArg->mport_enabled ? 1 : 0;
    sim_data.memory_width   = pDacHalInfo->InputWidth;
    sim_data.mem_latency    = (char)REG_RD_DRF(_PFB, _CONFIG_1, _CAS_LATENCY); //3
    sim_data.mem_aligned    = 1;
    sim_data.mem_page_miss  = (char)(REG_RD_DRF(_PFB, _CONFIG_1, _RAS_RAS) + REG_RD_DRF(_PFB, _CONFIG_1, _READ_TO_PCHG)); //10;
    sim_data.ram_config     = (int)REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_RAMCFG); 
    sim_data.gr_during_vid  = pVideoHalInfo->ColorKeyEnabled ? 1 : 0;
    sim_data.pclk_khz       = pDacValidateArbSettingsArg->vclk / 1000;  // in kHz, not MHz
    sim_data.mclk_khz       = pDacHalInfo->MClk / 1000;     // in kHz, not MHz
    sim_data.nvclk_khz      = pDacHalInfo->NVClk / 1000;    // in kHz, not MHz

    // 
    // Run the numbers through the ringer
    //
    if (pFbHalInfo->RamType == BUFFER_CRUSH_UMA)
        NV04_dacCalculateArbitration_SMA(pHalHwInfo, &fifo_data, &sim_data);
    else
        NV04_dacCalculateArbitration(pHalHwInfo, &fifo_data, &sim_data);
    
    pDacValidateArbSettingsArg->result = (U032)fifo_data.valid;
    return (RM_OK);
}                
                                                      
RM_STATUS
nvHalDacUpdateArbSettings_NV04(VOID *arg)
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
    // Build the sim table using current system settings
    //
    sim_data.pix_bpp        = (char) pDacHalInfo->Depth;
    sim_data.enable_video   = pVideoHalInfo->Enabled ? 1 : 0;
    sim_data.enable_mp      = pMpHalInfo->Enabled ? 1 : 0;
    sim_data.memory_width   = pDacHalInfo->InputWidth;
    sim_data.mem_latency    = (char) REG_RD_DRF(_PFB, _CONFIG_1, _CAS_LATENCY); //3
    sim_data.mem_aligned    = 1;
    sim_data.mem_page_miss  = (char) (REG_RD_DRF(_PFB, _CONFIG_1, _RAS_RAS) + REG_RD_DRF(_PFB, _CONFIG_1, _READ_TO_PCHG)); //10;
    sim_data.ram_config     = (int) REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_RAMCFG); 
    sim_data.gr_during_vid  = pVideoHalInfo->ColorKeyEnabled ? 1 : 0;
    sim_data.pclk_khz       = pDacHalInfo->VClk / 1000;     // in kHz, not MHz
    sim_data.mclk_khz       = pDacHalInfo->MClk / 1000;     // in kHz, not MHz
    sim_data.nvclk_khz      = pDacHalInfo->NVClk / 1000;    // in kHz, not MHz

    // 
    // Get those new numbers
    //
    if (pFbHalInfo->RamType == BUFFER_CRUSH_UMA)
        NV04_dacCalculateArbitration_SMA(pHalHwInfo, &fifo_data, &sim_data);
    else
        NV04_dacCalculateArbitration(pHalHwInfo, &fifo_data, &sim_data);
    
    //
    // If valid settings found, update the hardware
    //
    if (fifo_data.valid)
    {
        //
        // Set the DAC FIFO Thresholds and burst size
        //
        if (HAL_GETDISPLAYTYPE(Head) == DISPLAY_TYPE_TV)
        {
            REG_WR32(NV_PVIDEO_FIFO_BURST, 3);
            REG_WR32(NV_PVIDEO_FIFO_THRES, fifo_data.video_lwm >> 1);
        }
        else
        {
            // If we are upscaling video, we may not drain fast enough (fifo overflow), since
            // for every pixel from the framebuffer, we've got to send out more than one pixel. 
            // On 64 bit devices at high pixel clocks, the watermark will be set to maximum,
            // to solve the opposite problem (fifo underflow, draining faster than we fill),
            // This makes it more likely to overflow, so make sure we are not set to max 
            // low water mark. (At 16x12 > 60 Hz, we will get set to max lwm, causing us to
            // overflow when scaling the video to full screen.)
            if ((pDacHalInfo->InputWidth == 64) && pVideoHalInfo->Enabled)
            {
                if (fifo_data.video_lwm == 128*2)           // if set to max
                    fifo_data.video_lwm -= 8*2;             // drop down one notch
                // We must also prevent underflow. Letting the CRTC fifo use max bursts can starve video.
                if (fifo_data.crtc1_burst_size == 256)      // if set to max
                    fifo_data.crtc1_burst_size >>= 1;       // drop down on notch
            }

            REG_WR32(NV_PVIDEO_FIFO_THRES, fifo_data.video_lwm >> 1);
            switch (fifo_data.video_burst_size)
            {
                case 128:
                    REG_WR32(NV_PVIDEO_FIFO_BURST, 3);
                    break;

                case 64:
                    REG_WR32(NV_PVIDEO_FIFO_BURST, 2);
                    break;

                case 32:
                    REG_WR32(NV_PVIDEO_FIFO_BURST, 1);
                    break;
            }
        }

        //
        // If the video LWM is maxed out in a 64bit device, make sure to drop the graphics burst down
        // to provide additional bandwidth.
        //
        if ((pDacHalInfo->InputWidth == 64) && pVideoHalInfo->Enabled && ((fifo_data.video_lwm >> 1) == 0x80))
        {        
            // Make sure we've got room to drop
            if ((fifo_data.crtc1_burst_size > 128) && (pDacHalInfo->Depth==8)) 
                fifo_data.crtc1_burst_size >>= 2;
        }            
        
        //
        // Update the CRTC watermarks
        //
        // Unlock CRTC extended regs
        HAL_CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock, CurDacAdr);
        HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, CurDacAdr);

        //
        // Set the CRTC watermarks and burst size
        //
        if (HAL_GETDISPLAYTYPE(Head) != DISPLAY_TYPE_TV) // leave alone if TV
        {       
            HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, (U008) (fifo_data.crtc1_lwm >> 3), CurDacAdr);

            switch (fifo_data.crtc1_burst_size)
            {
                // If the DFP uses shortened blanking, we may run out of time.
                // Limiting burst to 64 prevents starving the fifo when running video.
                // This is not a problem on NV10 because of larger fifo size.
                // We're not checking for shortened blanking, instead we'll always limit burst
                // size for video + DFP. This may reduce performance, but we assume no one cares.
                case 256:
                    if ((HAL_GETDISPLAYTYPE(Head) == DISPLAY_TYPE_FLAT_PANEL) && (pVideoHalInfo->Enabled))
                    {
                        HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 2, CurDacAdr);
                    }
                    else
                        HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 4, CurDacAdr);
                    break;

                case 128:
                    if ((HAL_GETDISPLAYTYPE(Head) == DISPLAY_TYPE_FLAT_PANEL) && (pVideoHalInfo->Enabled))
                    {
                        HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 2, CurDacAdr);
                    }
                    else
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
            //
            // Manual override of LWM and Burst, if necessary and possible
            //
            if (pDacHalInfo->Enable256Burst && !pVideoHalInfo->Enabled && (pDacHalInfo->InputWidth == 128))
            {
        
                if ((pFbHalInfo->HorizDisplayWidth == 1024)
                    && (pFbHalInfo->VertDisplayWidth == 768)
                    && (pDacHalInfo->Depth == 16)
                    && ((pFbHalInfo->RefreshRate == 75) || (pFbHalInfo->RefreshRate == 85)))
                {
                    HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 4, CurDacAdr);
                    HAL_CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, 0x0F, CurDacAdr);
                }            
        
                if ((pFbHalInfo->HorizDisplayWidth == 1024)
                    && (pFbHalInfo->VertDisplayWidth == 768)
                    && (pDacHalInfo->Depth >= 24)
                    && ((pFbHalInfo->RefreshRate == 75) || (pFbHalInfo->RefreshRate == 85)))
                {
                    HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 4, CurDacAdr);
                }            

                if ((pFbHalInfo->HorizDisplayWidth == 1280)
                    && (pFbHalInfo->VertDisplayWidth == 1024)
                    && (pDacHalInfo->Depth >= 24)
                    && ((pFbHalInfo->RefreshRate == 75) || (pFbHalInfo->RefreshRate == 85)))
                {
                    HAL_CRTC_WR(NV_CIO_CRE_FF_INDEX, 4, CurDacAdr);
                }            
            
            }
        }        
        //
        // Relock if necessary
        //
        if (lock == 0)
            HAL_CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, CurDacAdr);
        
        return (RM_OK);
    }
    else
        //
        // No valid setting was found!!!  Either we fail this configuration
        // or we live with the current default settings for this mode.
        //
        return (RM_ERROR);
}


