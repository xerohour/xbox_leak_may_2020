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

/******************************* Video Manager *****************************\
*                                                                           *
* Module: VIDFRMEM.C                                                        *
*   This module implements the NV_VIDEO_FROM_MEMORY object class and its    *
*   corresponding methods.                                                  *
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
#include "nvhw.h"
#include "nvhalcpp.h"

#ifdef DEBUG_TIMING

#define DBG_PRINT_TIME(s,v) DBG_PRINT_STRING_VALUE(0x10, s, v);
U032 time0hi=0, time0lo=0, time1hi=0, time1lo=0;

#endif // DEBUG_TIMING

//
// The following routines are used by the HAL video engine
// manager in kernel/video/vidnv04.c.
//
RM_STATUS class63Method_NV04(PVIDEOMETHODARG_000);
RM_STATUS class63InitXfer_NV04(PHALHWINFO, VOID *, U032);
VOID class63DualSurfaceDesktop_NV04(PHALHWINFO, U032);
RM_STATUS class63GetEventStatus_NV04(PVIDEOGETEVENTSTATUSARG_000);
RM_STATUS class63Service_NV04(PVIDEOSERVICEEVENTARG_000);

//
// Statics
//
static RM_STATUS class63StartXfer_NV04(PHALHWINFO, VOID *, U032);
static VOID class63KickoffBuffer_NV04(PHALHWINFO, PVIDEOFROMMEMHALOBJECT, U032);
static VOID class63DualSurface_NV04(PHALHWINFO, PVIDEOFROMMEMHALOBJECT, U032);

static VOID class63ColorKey_NV04(PHALHWINFO, PVIDEOFROMMEMHALOBJECT, U032);

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

//
// Class instantiation/destruction is handled by video/videoobj.c and
// video/nv4/vidnv04.c.
//

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

//
// class63Method
//
// This routine performs any chip-specific handling for all class63
// methods.
//
RM_STATUS class63Method_NV04(PVIDEOMETHODARG_000 pVideoMethodArg)
{
    PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj;
    PHALHWINFO pHalHwInfo;
    U032 buffNum, offset, data;
    PHWREG nvAddr;

    pVidFromMemHalObj = (PVIDEOFROMMEMHALOBJECT)pVideoMethodArg->pHalObjInfo;
    offset = pVideoMethodArg->offset;
    data = pVideoMethodArg->data;
    pHalHwInfo = pVideoMethodArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;

    switch (offset)
    {
        case NVFF8_STOP_TRANSFER(0):
            if (pHalHwInfo->pDacHalInfo->CrtcInfo[0].DisplayType != DISPLAY_TYPE_DUALSURFACE)
                REG_WR32(NV_PVIDEO_OVERLAY, 0);     // quick shutoff
            pHalHwInfo->pVideoHalInfo->Enabled = 0;
            pHalHwInfo->pVideoHalInfo->ColorKeyEnabled = 0;
            pHalHwInfo->pVideoHalInfo->ScaleFactor = 0;
            pVidFromMemHalObj->KickOffProc = class63InitXfer_NV04;
            break;
        case NVFF8_IMAGE_SCAN_NOTIFY(0,0):
        case NVFF8_IMAGE_SCAN_NOTIFY(0,1):
            buffNum = (offset == NVFF8_IMAGE_SCAN_NOTIFY(0,0)) ? 0 : 1;
            if (pHalHwInfo->pDacHalInfo->CrtcInfo[0].DisplayType == DISPLAY_TYPE_DUALSURFACE)
                class63DualSurface_NV04(pHalHwInfo, pVidFromMemHalObj, buffNum);
            return pVidFromMemHalObj->KickOffProc(pHalHwInfo, (VOID *)pVidFromMemHalObj, buffNum);
        default:
            // HAL doesn't have anything to do for this particular method
            ;
    }

    return NVFF8_NOTIFICATION_STATUS_DONE_SUCCESS;
}

//---------------------------------------------------------------------------
//
//  Miscellaneous class support routines.
//
//---------------------------------------------------------------------------

//
//      Program the video scalar to scan out the video to the TV encoder.
//
static VOID class63DualSurface_NV04(PHALHWINFO pHalHwInfo, PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj, U032 buffNum)
{
    U032    pitch0, pitch1, vbs, vbe, hbs, hbe, scale, hw;
    U032    length, width, pitch, height;
    int     step_x, step_y;
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;

    //
    // Class63 has set up the scalar to render into a window, but we'll
    // change it to render to the full TV screen.
    // This means getting the source buffer size and scaling up to the
    // current resolution.
    //
        
    // calculate scaling factors

    // get the width and height of the video object
    // height = length/width
    // step x = ((src_width -1) << 11) / (dest_width -1 )
    // step y = ((src_height -1) << 11) / (dest_height - 1)
    length = pVidFromMemHalObj->Buffer[buffNum].Length;
    pitch  = pVidFromMemHalObj->Buffer[buffNum].Pitch;
    width  = pVidFromMemHalObj->Buffer[buffNum].Width;

    if (width != 0) // we'll get objects with size 0
    {
        height = length/width;
        step_x = ((width - 1) << 11) / (pHalHwInfo->pFbHalInfo->HorizDisplayWidth - 1);
        step_y = ((height - 1) << 11) / (pHalHwInfo->pFbHalInfo->VertDisplayWidth - 1);
        scale = ((step_y & 0xfff) << 16) | (step_x & 0xfff);
    }
    else
        return;

    // We have a valid video object
                    
    // find screen size
    if (pHalHwInfo->pFbHalInfo->HorizDisplayWidth < 512)
    {
        // doubled modes
        hw = pHalHwInfo->pFbHalInfo->HorizDisplayWidth*2 | (pHalHwInfo->pFbHalInfo->VertDisplayWidth*2 << 16);
    }
    else                                                            
    {
        hw = pHalHwInfo->pFbHalInfo->HorizDisplayWidth | (pHalHwInfo->pFbHalInfo->VertDisplayWidth << 16);
    }
    pitch1 = pitch0 = pHalHwInfo->pFbHalInfo->HorizDisplayWidth * 2;

    // set up timing default values (640x480)
    vbs   = 0x00000240;
    vbe   = 0x0000005c;
    hbs   = 0x00000300;
    hbe   = 0x00000050;
        
    // adjust horizontal and vertical position/size
    // low-res modes don't quite use the same timings as their doubles
    switch (pHalHwInfo->pFbHalInfo->VertDisplayWidth)
    {
        case 200:     // 320x200x16
            hbs   = 0x00000308;
            hbe   = 0x00000068;
            vbs   = 0x000001ec;
            vbe   = 0x0000005c;
            break;             
        case 240:     // 320x240x16
            hbs   = 0x00000308;
            hbe   = 0x00000050;
            break;             
        case 300:     // 400x300x16
            vbs   = 0x000002b0;
            vbe   = 0x00000058;
            hbs   = 0x000003d0;
            hbe   = 0x00000088;
            break;             
        case 384:     // 512x384x16
            vbs   = 0x00000230;
            vbe   = 0x00000080;
            hbs   = 0x00000290;
            hbe   = 0x00000090;
            break;             
        case 400:     // 640x400x16
            vbs   = 0x000001ec;
            vbe   = 0x0000005c;
            break;             
        case 480:     // 640x480x16
            vbs   = 0x00000240;
            vbe   = 0x0000005c;
            hbs   = 0x00000300;
            hbe   = 0x00000050;
            break;
        case 600:     // 800x600x16
            vbs   = 0x000002b0;
            vbe   = 0x00000058;
            hbs   = 0x000003a8;
            hbe   = 0x00000088;
            break;
        default:
            break;
    }            
    vbe--;
        
    // Output timing
    REG_WR32(NV_PRAMDAC_TV_VBLANK_START ,vbs);   
    REG_WR32(NV_PRAMDAC_TV_VBLANK_END   ,vbe);    
    REG_WR32(NV_PRAMDAC_TV_HBLANK_START ,hbs);     
    REG_WR32(NV_PRAMDAC_TV_HBLANK_END   ,hbe);    
        
    // Video scalar
    REG_WR32(NV_PVIDEO_STEP_SIZE  ,scale);        // scale factor 1:1
    REG_WR32(NV_PVIDEO_BUFF0_PITCH,pitch0);     // pitch buffer 0
    REG_WR32(NV_PVIDEO_BUFF1_PITCH,pitch1);     // pitch buffer 1
    REG_WR32(NV_PVIDEO_WINDOW_SIZE,hw);         // h (26:16), w (10:0)
}

//
// Program the video scalar to scan out the framebuffer (desktop) to the
// TV encoder.
// Or, turn off the video scalar for TV only or monitor.
//
VOID class63DualSurfaceDesktop_NV04(PHALHWINFO pHalHwInfo, U032 mode)
{
    U032 coeff, setup, vbs, vbe, hbs, hbe, scale, start_buf0, start_buf1, pitch0, pitch1, xy_pos, hw, fifo0, fifo1, reg32;
    int dualSurface = 0;
    PHWREG nvAddr;

    nvAddr = pHalHwInfo->nvBaseAddr;
    switch (mode)
    {
        case DISPLAY_TYPE_MONITOR:
            setup = NV_PRAMDAC_TV_SETUP_DEV_TYPE_SLAVE;
            break;
        case DISPLAY_TYPE_TV:
            setup = NV_PRAMDAC_TV_SETUP_DEV_TYPE_MASTER;
            break;
        case DISPLAY_TYPE_DUALSURFACE:
            dualSurface = 1;
        
            start_buf0 = 0;     // desktop starts at 0
            start_buf1 = 0;
            if (pHalHwInfo->pFbHalInfo->HorizDisplayWidth < 512)
            {
                // doubled modes
                hw = pHalHwInfo->pFbHalInfo->HorizDisplayWidth*2 | (pHalHwInfo->pFbHalInfo->VertDisplayWidth*2 << 16);
                scale = 0x04000400;     // 2:1
            }
            else                                                            
            {
                hw = pHalHwInfo->pFbHalInfo->HorizDisplayWidth | (pHalHwInfo->pFbHalInfo->VertDisplayWidth << 16);
                scale = 0x08000800;     // 1:1
            }            
            pitch1 = pitch0 = pHalHwInfo->pFbHalInfo->HorizDisplayWidth * 2;
            // set up timing default values (640x480)
            coeff = 0x10700;
            setup = 0x00001111;     // 565 pixel data, use second surface
            vbs   = 0x00000240;
            vbe   = 0x0000005c;
            hbs   = 0x00000300;
            hbe   = 0x00000050;
            xy_pos = 0;
            fifo0 = 0x20;           // watermark
            fifo1 = 0x03;
                
            // adjust horizontal and vertical position/size
            // low-res modes don't quite use the same timings as their doubles
            switch (pHalHwInfo->pFbHalInfo->VertDisplayWidth)
            {
                case 200:     // 320x200x16
                    hbs   = 0x00000308;
                    hbe   = 0x00000068;
                    vbs   = 0x000001ec;
                    vbe   = 0x0000005c;
                    break;             
                case 240:     // 320x240x16
                    hbs   = 0x00000308;
                    hbe   = 0x00000050;
                    break;             
                case 300:     // 400x300x16
                    vbs   = 0x000002b0;
                    vbe   = 0x00000058;
                    hbs   = 0x000003d0;
                    hbe   = 0x00000088;
                    break;             
                case 384:     // 512x384x16
                    vbs   = 0x00000230;
                    vbe   = 0x00000080;
                    hbs   = 0x00000290;
                    hbe   = 0x00000090;
                    break;             
                case 400:     // 640x400x16
                    vbs   = 0x000001ec;
                    vbe   = 0x0000005c;
                    break;             
                case 480:     // 640x480x16
                    vbs   = 0x00000240;
                    vbe   = 0x0000005c;
                    hbs   = 0x00000300;
                    hbe   = 0x00000050;
                    break;
                case 600:     // 800x600x16
                    vbs   = 0x000002b0;
                    vbe   = 0x00000058;
                    hbs   = 0x000003a8;
                    hbe   = 0x00000088;
                    break;
                default:
                    dualSurface = 0;
                    break;
            }            
            break;
    }

    if (dualSurface)    // DualSurface requested and its a valid mode
    {
        // put video scalar in idle state, pointing at buffer 1            
        REG_WR32(NV_PVIDEO_OE_STATE, 0x01000000);             
        REG_WR32(NV_PVIDEO_OE_STATE, 0x01000000);             
        //WritePriv32(0x00110000, NV_PVIDEO_SU_STATE);             
        //WritePriv32(0x00110000, NV_PVIDEO_SU_STATE);             
#if 0 // we can get stuck if we are in VGA mode; we'll never reach HalInfo.VertDisplayWidth
        // wait end of screen
        reg32 = 0;
        while (reg32 < pHalHwInfo->pFbHalInfo->VertDisplayWidth)
        {
            reg32 = REG_RD_DRF(_PCRTC, _RASTER, _POSITION);
        }
#endif
        REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, coeff);   
        REG_WR32(NV_PRAMDAC_TV_SETUP        , setup);          
        REG_WR32(NV_PRAMDAC_TV_VBLANK_START , vbs);   
        REG_WR32(NV_PRAMDAC_TV_VBLANK_END   , vbe);    
        REG_WR32(NV_PRAMDAC_TV_HBLANK_START , hbs);     
        REG_WR32(NV_PRAMDAC_TV_HBLANK_END   , hbe);    
            
        ////WritePriv32(coeff, NV_PRAMDAC_PLL_COEFF_SELECT);   
        ////WritePriv32(setup, NV_PRAMDAC_TV_SETUP);          
        ////WritePriv32(vbs, NV_PRAMDAC_TV_VBLANK_START);   
        ////WritePriv32(vbe, NV_PRAMDAC_TV_VBLANK_END);    
        ////WritePriv32(hbs, NV_PRAMDAC_TV_HBLANK_START);     
        ////WritePriv32(hbe, NV_PRAMDAC_TV_HBLANK_END);    

        // Video scalar
        REG_WR32(NV_PVIDEO_STEP_SIZE   , scale);        // scale factor 1:1
        REG_WR32(NV_PVIDEO_BUFF0_PITCH , pitch0);     // pitch buffer 0
        REG_WR32(NV_PVIDEO_BUFF1_PITCH , pitch1);     // pitch buffer 1
        REG_WR32(NV_PVIDEO_WINDOW_SIZE , hw);         // h (26:16), w (10:0)
        REG_WR32(NV_PVIDEO_FIFO_THRES  , fifo0);       // fifo fill threshold
        REG_WR32(NV_PVIDEO_FIFO_BURST  , fifo1);       // fifo burst length
        REG_WR32(NV_PVIDEO_WINDOW_START, xy_pos);    // x,y position within buffer (start display here)
        REG_WR32(NV_PVIDEO_BUFF0_START , start_buf0); // start address buffer 0
        REG_WR32(NV_PVIDEO_BUFF1_START , start_buf1); // start address buffer 1
            
        ////WritePriv32(scale, NV_PVIDEO_STEP_SIZE);        // scale factor 1:1
        ////WritePriv32(pitch0, NV_PVIDEO_BUFF0_PITCH);     // pitch buffer 0
        ////WritePriv32(pitch1, NV_PVIDEO_BUFF1_PITCH);     // pitch buffer 1
        ////WritePriv32(hw, NV_PVIDEO_WINDOW_SIZE);         // h (26:16), w (10:0)
        ////WritePriv32(fifo0, NV_PVIDEO_FIFO_THRES);       // fifo fill threshold
        ////WritePriv32(fifo1, NV_PVIDEO_FIFO_BURST);       // fifo burst length
        ////WritePriv32(xy_pos, NV_PVIDEO_WINDOW_START);    // x,y position within buffer (start display here)
        ////WritePriv32(start_buf0, NV_PVIDEO_BUFF0_START); // start address buffer 0
        ////WritePriv32(start_buf1, NV_PVIDEO_BUFF1_START); // start address buffer 1

        // 
        FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _VIDEO, _ON);  // enable scalar
        FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _FORMAT, _CCIR);  // set format

        // switch to buffer 0, which will force the scalar to load new values
        reg32 = REG_RD32(NV_PVIDEO_SU_STATE);
        ////reg32 = ReadPriv32(NV_PVIDEO_SU_STATE);
        reg32 &= ~0x00010000; // NV_PVIDEO_SU_STATE_BUFF0_IN_USE = 0
        REG_WR32(NV_PVIDEO_SU_STATE, reg32); // clear BUFF0_IN_USE
        ////WritePriv32(reg32, NV_PVIDEO_SU_STATE); // clear BUFF0_IN_USE

    }    
    else    // need to set up for either TV only or Monitor only
    {
        reg32 = REG_RD32(NV_PRAMDAC_TV_SETUP);
        reg32 &= NV_PRAMDAC_TV_SETUP_DEV_TYPE_SLAVE;
        reg32 |= setup;
        REG_WR32(0x680700, setup);   // TV SETUP
        ////WritePriv32(setup, 0x680700);   // TV SETUP
        FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _VIDEO, _OFF);  // disable scalar
    }            
}

static VOID class63ColorKey_NV04
(
    PHALHWINFO pHalHwInfo,
    PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj,
    U032 buffNum
)
{
    U032 color, colorFormat;
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;

    color = pVidFromMemHalObj->Buffer[buffNum].VideoColor;
    colorFormat = pVidFromMemHalObj->Buffer[buffNum].VideoColorFormat;

    //
    // Set the hardware to the specified color.
    //
    if (color)
    {
        switch (colorFormat)
        {
            case NVFF6_SET_COLOR_FORMAT_LE_X16A8Y8:
            case NVFF6_SET_COLOR_FORMAT_LE_X24Y8:
                REG_WR32(NV_PVIDEO_KEY, color & 0xFF);
                break;
            
            case NVFF6_SET_COLOR_FORMAT_LE_X16A1R5G5B5:
            case NVFF6_SET_COLOR_FORMAT_LE_X17R5G5B5:
                REG_WR32(NV_PVIDEO_KEY, color & 0x7FFF);
                break;
                
            case NVFF6_SET_COLOR_FORMAT_LE_A16R5G6B5:
            case NVFF6_SET_COLOR_FORMAT_LE_A16Y16:
            case NVFF6_SET_COLOR_FORMAT_LE_X16Y16:
                REG_WR32(NV_PVIDEO_KEY, color & 0xFFFF);
                break;
                
            case NVFF6_SET_COLOR_FORMAT_LE_A8R8G8B8:
            case NVFF6_SET_COLOR_FORMAT_LE_X8R8G8B8:                
                REG_WR32(NV_PVIDEO_KEY, color & 0xFFFFFF);
                break; 
                
            default:
                REG_WR32(NV_PVIDEO_KEY, color);
                break; 
        }
            
        FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _KEY, _ON);
    } 
    else
        FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _KEY, _OFF);
}

//---------------------------------------------------------------------------
//
//  Buffer transfer routines.
//
//---------------------------------------------------------------------------

RM_STATUS class63InitXfer_NV04
(
    PHALHWINFO    pHalHwInfo,
    VOID          *pHalObj,
    U032          buffNum
)
{
    RM_STATUS       status = RM_OK;
    PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj;
    V032            stateSU;
    V032            stateOE;
    V032            stateOESU;
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class63InitXfer\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pVidFromMemHalObj = (PVIDEOFROMMEMHALOBJECT)pHalObj;

    //
    // Since we're not context switching any of the video registers across channels yet, we
    // can't just initialize the values once on boot and assume they'll look ok.  Let's 
    // hack for now and init them every new buffer.
    // 
    videoInit_NV04(pHalHwInfo, NV_VIDEO_FROM_MEMORY);

    if (((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_V8YB8U8YA8)) /* YUYV, YUY2, YUV2 */
        FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _FORMAT, _YUY2);
    if (((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_YB8V8YA8U8)) /* UYVY, CCIR601    */
        FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _FORMAT, _CCIR);
        
    stateSU = REG_RD32(NV_PVIDEO_SU_STATE);
    stateOE = REG_RD32(NV_PVIDEO_OE_STATE);
    if (buffNum == (stateOE >> 24))
    {
        // The current buffer in stateOE is same as the one we want to start.
        // If both buffers are idle in this state, we have to toggle the current
        // buffer to make the overlay engine look at the buffer we are about to
        // setup.
        stateOESU = stateOE ^ stateSU;
        if ((stateOESU & 0x00100000) &&  //buffer 1 is idle
            (stateOESU & 0x00010000))    //buffer 0 is idle
        {
            stateOE ^= 0x01000000;
            REG_WR32(NV_PVIDEO_OE_STATE, stateOE);
        }
    }
        
    status = class63StartXfer_NV04(pHalHwInfo, (VOID *)pVidFromMemHalObj, buffNum);
    if (status == RM_OK)
    {
    	// Actually kickoff the buffer that was just marked busy by writing the registers.
        // After this, buffers are actually kicked off after we get the completion interrupt.

        if (pVidFromMemHalObj->Buffer[0].State == OVERLAY_BUFFER_BUSY)
        {
            class63KickoffBuffer_NV04(pHalHwInfo, pVidFromMemHalObj, 0);
        }
        else
        {
            class63KickoffBuffer_NV04(pHalHwInfo, pVidFromMemHalObj, 1);
        }
        
        // Set the enable bit in VBLANK. This is to prevent enabling the video in the middle of
        // the video window. That may cause only the "end window" processing to be done without
        // having done the "start window" processing.
        // FLD_WR_DRF_NUM(_PVIDEO, _OVERLAY, _VIDEO, pDev->Video.Enabled);
        pHalHwInfo->pVideoHalInfo->UpdateFlags |= UPDATE_HWINFO_VIDEO_ENABLE;

        //
        // This instance has been initialized.  Call StartXfer for the next
        // buffer updates.
        //
        pVidFromMemHalObj->KickOffProc = class63StartXfer_NV04;
    }
    return (status);
}

//
// Start the transfer from the DMA buffer.
//
static RM_STATUS class63StartXfer_NV04
(
    PHALHWINFO    pHalHwInfo,
    VOID          *pHalObj,
    U032          buffNum
)
{
    PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj;
    RM_STATUS       status = RM_OK;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class63StartXfer\r\n");

    pVidFromMemHalObj = (PVIDEOFROMMEMHALOBJECT)pHalObj;

    //
    // XXX (scottl):
    // I've made a *MAJOR* assumption here that we do not use the BBufferXfer
    // proc to handle RGB video.  In looking at the transfer method in 
    // in class63.c (formally c63nv04.c), it looks like it's entirely
    // vblank driven.  By making this assumption, I'm able to avoid having
    // vblank info in the HAL.
    //
#ifdef DEBUG
    if (((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_Y8) ||
        ((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_GY1R5G5B5) ||
        ((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_R5G6B5) ||
        ((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_GY1X7R8G8B8))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: class63StartXfer: format is RGB!\r\n");
        DBG_BREAKPOINT();
    }
#endif

    //
    // VideoStart, VideoSize and VideoScale are not double buffered.
    // So copy the values into the Buff0 versions to simulate true
    // double buffering.
    //
    pVidFromMemHalObj->Buffer[buffNum].VideoStart = pHalHwInfo->pVideoHalInfo->VideoStart;
    pVidFromMemHalObj->Buffer[buffNum].VideoSize = pHalHwInfo->pVideoHalInfo->VideoSize;
    pVidFromMemHalObj->Buffer[buffNum].VideoScale = pHalHwInfo->pVideoHalInfo->VideoScale;
    pVidFromMemHalObj->Buffer[buffNum].VideoColorFormat = pHalHwInfo->pVideoHalInfo->VideoColorFormat;
    pVidFromMemHalObj->Buffer[buffNum].VideoColor = pHalHwInfo->pVideoHalInfo->VideoColor;

    //
    // Mark the buffer as BUSY. That is all we do here. The hardware
    // registers are programmed in videoService after the current buffer
    // completion.
    //
    pVidFromMemHalObj->Buffer[buffNum].State = OVERLAY_BUFFER_BUSY;
#ifdef DEBUG_TIMING
    vmmOutDebugString("\n0B");
#endif

    return (status);
}

static VOID class63KickoffBuffer_NV04
(
    PHALHWINFO                  pHalHwInfo,
    PVIDEOFROMMEMHALOBJECT      pVidFromMemHalObj,
    U032                        buffNum
)
{
    V032 stateSU;
    PHWREG nvAddr;

    nvAddr = pHalHwInfo->nvBaseAddr;

    stateSU = REG_RD32(NV_PVIDEO_SU_STATE);

    if (buffNum == 0) {
        REG_WR32(NV_PVIDEO_BUFF0_START, pVidFromMemHalObj->Buffer[buffNum].Start);
        REG_WR32(NV_PVIDEO_BUFF0_PITCH, (pVidFromMemHalObj->Buffer[buffNum].Pitch & 0x00007FF0));
        FLD_WR_DRF_NUM(_PVIDEO, _BUFF0_OFFSET, _Y, (pVidFromMemHalObj->Buffer[buffNum].Pitch & 3));
    } else {
        REG_WR32(NV_PVIDEO_BUFF1_START, pVidFromMemHalObj->Buffer[buffNum].Start);
        REG_WR32(NV_PVIDEO_BUFF1_PITCH, (pVidFromMemHalObj->Buffer[buffNum].Pitch & 0x00007FF0));
        FLD_WR_DRF_NUM(_PVIDEO, _BUFF1_OFFSET, _Y, (pVidFromMemHalObj->Buffer[buffNum].Pitch & 3));
    }
            
    FLD_WR_DRF_DEF(_PVIDEO, _CONTROL_Y, _LINE, _HALF);
        
    if (IsNV4_NV04(pHalHwInfo->pMcHalInfo) && 
        (pHalHwInfo->pDacHalInfo->CrtcInfo[0].MonitorType == MONITOR_TYPE_NTSC ||
         pHalHwInfo->pDacHalInfo->CrtcInfo[0].MonitorType == MONITOR_TYPE_PAL))
    {
        // NV4 HW bug, shift position on TV
        pVidFromMemHalObj->Buffer[buffNum].VideoStart += NV4_TV_DAC_SHIFT;
    }

    REG_WR32(NV_PVIDEO_WINDOW_START, pVidFromMemHalObj->Buffer[buffNum].VideoStart);
    REG_WR32(NV_PVIDEO_WINDOW_SIZE, pVidFromMemHalObj->Buffer[buffNum].VideoSize);
    REG_WR32(NV_PVIDEO_STEP_SIZE, pVidFromMemHalObj->Buffer[buffNum].VideoScale);
 
    class63ColorKey_NV04(pHalHwInfo, pVidFromMemHalObj, buffNum);
        
    // Mark buffer 1 as NOTIFY_PENDING.
    pVidFromMemHalObj->Buffer[buffNum].State = OVERLAY_BUFFER_NOTIFY_PENDING;

    if (buffNum == 0)
        stateSU ^= 1 << SF_SHIFT(NV_PVIDEO_SU_STATE_BUFF0_IN_USE);
    else
        stateSU ^= 1 << SF_SHIFT(NV_PVIDEO_SU_STATE_BUFF1_IN_USE);

    REG_WR32(NV_PVIDEO_SU_STATE, stateSU);
}

//---------------------------------------------------------------------------
//
//  Exception handling routines.
//
//---------------------------------------------------------------------------

//
// class63 exception data fetcher
//
// Let's RM know which buffer (if any) needs servicing.
//
RM_STATUS class63GetEventStatus_NV04(PVIDEOGETEVENTSTATUSARG_000 pVideoGetEventStatusArg)
{
    PHALHWINFO pHalHwInfo;
    PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj;
    PHWREG nvAddr;
    U032 intr0;
    V032 stateOE;
    V032 stateSU;
    V032 stateRM;
    V032 stateOESU;
    V032 stateOERM;

    pHalHwInfo = pVideoGetEventStatusArg->pHalHwInfo;
    pVidFromMemHalObj = (PVIDEOFROMMEMHALOBJECT)pVideoGetEventStatusArg->pHalObjInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;

    //
    // Default is that none of the buffers needs servicing.  So tell
    // RM to bypass the nvHalVideoService handler.
    //    
    pVideoGetEventStatusArg->events = 0;

    intr0 = REG_RD32(NV_PVIDEO_INTR_0);
    pVideoGetEventStatusArg->intrStatus = intr0;

    //
    // Pending notify (buffer completion) ?
    //
    if (intr0 & DRF_DEF(_PVIDEO, _INTR_0, _NOTIFY, _PENDING))
    {

        // Reset the pending bit right away. This way, if another buffer completion becomes pending
        // while we are servicing the current buffer completion, it will not be lost. 
        // This is the best we can do to close this window.
        // We will return intr_notify_pending state so if another interrupt happened, 
        // we will be called again.
        REG_WR32(NV_PVIDEO_INTR_0, NV_PVIDEO_INTR_0_NOTIFY_RESET);
        
        //
        // Grab the current states
        //
        stateOE = REG_RD32(NV_PVIDEO_OE_STATE);
        stateSU = REG_RD32(NV_PVIDEO_SU_STATE);
        stateRM = REG_RD32(NV_PVIDEO_RM_STATE);
        stateOESU = stateOE ^ stateSU;
        stateOERM = stateOE ^ stateRM;

#ifdef DEBUG_TIMING
        DBG_PRINT_STRING_VALUE (0x10, "                                                   stateOE = ",stateOE);
        DBG_PRINT_STRING_VALUE (0x10, "                                                   stateSU = ",stateSU);
        DBG_PRINT_STRING_VALUE (0x10, "                                                   stateRM = ",stateRM);
#endif

        if ((stateOESU & 0x00010000))
        {
            if (stateOERM & 0x00000001)
            {
#ifdef DEBUG_TIMING            
                DBG_PRINT_TIME("                                                   VideoService0: ",REG_RD32(NV_PTIMER_TIME_0) - time0lo);
                
                time0hi = REG_RD32(NV_PTIMER_TIME_1);
                time0lo = REG_RD32(NV_PTIMER_TIME_0);
#endif
                // Mark buffer 0 pending if notify has been setup.
                if (pVidFromMemHalObj->Buffer[0].State == OVERLAY_BUFFER_NOTIFY_PENDING)
                    pVideoGetEventStatusArg->events |= (1 << 0);
            }
        }
        
        if ((stateOESU & 0x00100000))
        {
            if (stateOERM & 0x00000010)
            {

#ifdef DEBUG_TIMING            
                DBG_PRINT_TIME("                                                   VideoService1: ",REG_RD32(NV_PTIMER_TIME_0) - time0lo);
                
                time0hi = REG_RD32(NV_PTIMER_TIME_1);
                time0lo = REG_RD32(NV_PTIMER_TIME_0);
#endif
                // Mark buffer 1 pending if notify has been setup.
                if (pVidFromMemHalObj->Buffer[1].State == OVERLAY_BUFFER_NOTIFY_PENDING)
                    pVideoGetEventStatusArg->events |= (1 << 1);
            }
        }
    }

    return RM_OK;
}

//
// class63 exception handler
//
// This interface relies on values setup by the nvHalVideoGetEventStatus
// interface (i.e. the PVIDEO_INTR_0 register contents).  We go to the
// hardware to get the rest of it.
//
RM_STATUS class63ServiceEvent_NV04(PVIDEOSERVICEEVENTARG_000 pVideoServiceArg)
{
    PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj;
    PHALHWINFO pHalHwInfo;
    PHWREG nvAddr;
    U032 intr0;
    V032 stateOE;
    V032 stateSU;
    V032 stateRM;
    V032 stateOESU;
    V032 stateOERM;

    pHalHwInfo = pVideoServiceArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;

    pVidFromMemHalObj = (PVIDEOFROMMEMHALOBJECT)pVideoServiceArg->pHalObjInfo;

    //
    // The pending interrupt register value was saved off by the
    // GetEventStatus entry point.
    //
    intr0 = pVideoServiceArg->intrStatus;

    //
    // Pending notify (buffer completion) ?
    //
    if (intr0 & DRF_DEF(_PVIDEO, _INTR_0, _NOTIFY, _PENDING))
    {
        //
        // Grab the current states
        //
        stateOE = REG_RD32(NV_PVIDEO_OE_STATE);
        stateSU = REG_RD32(NV_PVIDEO_SU_STATE);
        stateRM = REG_RD32(NV_PVIDEO_RM_STATE);
        stateOESU = stateOE ^ stateSU;
        stateOERM = stateOE ^ stateRM;

#ifdef DEBUG_TIMING
        DBG_PRINT_STRING_VALUE (0x10, "                                                   stateOE = ",stateOE);
        DBG_PRINT_STRING_VALUE (0x10, "                                                   stateSU = ",stateSU);
        DBG_PRINT_STRING_VALUE (0x10, "                                                   stateRM = ",stateRM);
#endif

        //
        // Buffer 0.
        //
        if ((stateOESU & 0x00010000))
        {
            if (stateOERM & 0x00000001)
            {
                if (pVideoServiceArg->events & (1 << 0))
                    pVidFromMemHalObj->Buffer[0].State = OVERLAY_BUFFER_IDLE;

#ifdef DEBUG_TIMING            
                DBG_PRINT_TIME("                                                   VideoService0: ",REG_RD32(NV_PTIMER_TIME_0) - time0lo);
                
                time0hi = REG_RD32(NV_PTIMER_TIME_1);
                time0lo = REG_RD32(NV_PTIMER_TIME_0);
#endif // DEBUG_TIMING

                stateRM ^= 1 << SF_SHIFT(NV_PVIDEO_RM_STATE_BUFF0_INTR_NOTIFY);
                REG_WR32(NV_PVIDEO_RM_STATE, stateRM);
                
                if (pVidFromMemHalObj->Buffer[1].State == OVERLAY_BUFFER_BUSY)
                {
                	// Software buffer 1 is ready. Kick it off.
                    class63KickoffBuffer_NV04(pHalHwInfo, pVidFromMemHalObj, 1 /* buffNum */);
                }
                else
                {
                    // Software buffer 1 is not ready.
                    // Program OE and SU so that the hardware flips to 0 again.
                    stateOE = REG_RD32(NV_PVIDEO_OE_STATE);
                    if (0 == (stateOE >> 24))
                    {
                        stateOE ^= 0x01000000;
                        REG_WR32(NV_PVIDEO_OE_STATE, stateOE);
                    }

                    // If buffer 0 itself is busy again. Load the new values.
                    if (pVidFromMemHalObj->Buffer[0].State == OVERLAY_BUFFER_BUSY)
                    {
                        class63KickoffBuffer_NV04(pHalHwInfo, pVidFromMemHalObj, 0 /* buffNum */);
                    } // buffer 0 has new values
                    else
                    { 
                        // We just need to redisplay the same buffer 0 values again.
                        // We don't have to load any hardware registers. 
                        // Flip SU_STATE_BUFF0_IN_USE
                        stateSU = REG_RD32(NV_PVIDEO_SU_STATE);
                        stateSU ^= 1 << SF_SHIFT(NV_PVIDEO_SU_STATE_BUFF0_IN_USE);
                        REG_WR32(NV_PVIDEO_SU_STATE, stateSU);
#ifdef DEBUG_TIMING
                        vmmOutDebugString("0R");
#endif
                    } // redisplay buffer 0 values
                } // Buffer 1 is not busy    
                        
                // Return the current interrupt pending state.
                // intr0 = REG_RD32(NV_PVIDEO_INTR_0);
                // return(intr0 & DRF_DEF(_PVIDEO, _INTR_0, _NOTIFY, _PENDING));
            } // Hardware says buffer 0 completed.
        }

        //
        // Buffer 1.
        //
        if ((stateOESU & 0x00100000))
        {
            if (stateOERM & 0x00000010)
            {
                if (pVideoServiceArg->events & (1 << 1))
                    pVidFromMemHalObj->Buffer[1].State = OVERLAY_BUFFER_IDLE;

#ifdef DEBUG_TIMING            
                DBG_PRINT_TIME("                                                   VideoService1: ",REG_RD32(NV_PTIMER_TIME_0) - time0lo);
                
                time0hi = REG_RD32(NV_PTIMER_TIME_1);
                time0lo = REG_RD32(NV_PTIMER_TIME_0);
#endif // DEBUG_TIMING

                stateRM ^= 1 << SF_SHIFT(NV_PVIDEO_RM_STATE_BUFF1_INTR_NOTIFY);
                REG_WR32(NV_PVIDEO_RM_STATE, stateRM);
                
                if (pVidFromMemHalObj->Buffer[0].State == OVERLAY_BUFFER_BUSY)
                {
                	// Software buffer 0 is ready. Kick it off.
                    class63KickoffBuffer_NV04(pHalHwInfo, pVidFromMemHalObj, 0 /* buffNum */);
                }
                else
                {
                    // Software buffer 0 is not ready.
                    // Program OE and SU so that the hardware flips to 1 again.
                    stateOE = REG_RD32(NV_PVIDEO_OE_STATE);
                    if (1 == (stateOE >> 24))
                    {
                        stateOE ^= 0x01000000;
                        REG_WR32(NV_PVIDEO_OE_STATE, stateOE);
                    }

                    // If buffer 1 itself is busy again. Load the new values.
                    if (pVidFromMemHalObj->Buffer[1].State == OVERLAY_BUFFER_BUSY)
                    {
                        class63KickoffBuffer_NV04(pHalHwInfo, pVidFromMemHalObj, 1 /* buffNum */);
                    } // Buffer 1 has new values
                    else
                    { 
                        // We just need to redisplay the same buffer 1 values again.
                        // We don't have to load any hardware registers. 
                        // Flip SU_STATE_BUFF0_IN_USE
                        stateSU = REG_RD32(NV_PVIDEO_SU_STATE);
                        stateSU ^= 1 << SF_SHIFT(NV_PVIDEO_SU_STATE_BUFF1_IN_USE);
                        REG_WR32(NV_PVIDEO_SU_STATE, stateSU);
#ifdef DEBUG_TIMING
                        vmmOutDebugString("1R");
#endif
                    } // Redisplay buffer 1 values
                } // Buffer 0 is not busy    
            } // Hardware says buffer 1 completed.
        }

#ifdef DEBUG_TIMING
        // Grab the current states again and print it.
        //
        stateOE = REG_RD32(NV_PVIDEO_OE_STATE);
        stateSU = REG_RD32(NV_PVIDEO_SU_STATE);
        stateRM = REG_RD32(NV_PVIDEO_RM_STATE);

        DBG_PRINT_STRING_VALUE (0x10, "                                                   stateOE = ",stateOE);
        DBG_PRINT_STRING_VALUE (0x10, "                                                   stateSU = ",stateSU);
        DBG_PRINT_STRING_VALUE (0x10, "                                                   stateRM = ",stateRM);
#endif
    }  // Notify was pending
    
    // Return the current interrupt pending state.
    intr0 = REG_RD32(NV_PVIDEO_INTR_0);
    pVideoServiceArg->intrStatus = intr0 & DRF_DEF(_PVIDEO, _INTR_0, _NOTIFY, _PENDING);

    return RM_OK;
}
