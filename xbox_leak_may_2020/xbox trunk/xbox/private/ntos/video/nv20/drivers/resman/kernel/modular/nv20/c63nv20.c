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
#include <nv20_ref.h>
#include <nvrm.h>
#include <nv20_hal.h>
#include "nvhw.h"
#include "nvhalcpp.h"

#ifdef DEBUG_TIMING

#define DBG_PRINT_TIME(s,v) DBG_PRINT_STRING_VALUE(0x10, s, v);
U032 time0hi=0, time0lo=0, time1hi=0, time1lo=0;

#endif // DEBUG_TIMING

//
// We can't quite use the DRF macros due to how they concatenate strings
// to create field accesses and do some bit shifting ... these are only
// used for NV_PVIDEO_FORMAT updates
//
#define VIDEO_FORMAT_FLD_WR_DEF(buf, fld, def)                                                  \
        REG_WR32(NV_PVIDEO_FORMAT(buf), (REG_RD32(NV_PVIDEO_FORMAT(buf)) &~                     \
                (DRF_MASK(NV_PVIDEO_FORMAT ## fld) << DRF_SHIFT(NV_PVIDEO_FORMAT ## fld))) |    \
                DRF_DEF(_PVIDEO, _FORMAT, fld, def))

#define VIDEO_FORMAT_FLD_WR_PITCH(buf, val)                                                       \
        REG_WR32(NV_PVIDEO_FORMAT(buf), (REG_RD32(NV_PVIDEO_FORMAT(buf)) &~                       \
                (DRF_MASK(NV_PVIDEO_FORMAT_PITCH) << DRF_SHIFT(NV_PVIDEO_FORMAT_PITCH))) | val)

//
// The following routines are used by the HAL video engine
// manager in kernel/video/vidnv20.c.
//
RM_STATUS class63Method_NV20(PVIDEOMETHODARG_000);
RM_STATUS class63InitXfer_NV20(PHALHWINFO, VOID *, U032);
VOID class63DualSurfaceDesktop_NV20(PHALHWINFO, U032);

//
// Statics
//
static RM_STATUS class63StartXfer_NV20(PHALHWINFO, VOID *, U032);
static VOID class63DualSurface_NV20(PHALHWINFO, PVIDEOFROMMEMHALOBJECT, U032);
static VOID class63ColorKey_NV20(PHALHWINFO, PVIDEOFROMMEMHALOBJECT, U032);

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

//
// Class instantiation/destruction is handled by video/videoobj.c and
// video/nv20/vidnv20.c.
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
RM_STATUS class63Method_NV20(PVIDEOMETHODARG_000 pVideoMethodArg)
{
    PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj;
    PHALHWINFO pHalHwInfo;
    U032 buffNum, offset, data;

    pVidFromMemHalObj = (PVIDEOFROMMEMHALOBJECT)pVideoMethodArg->pHalObjInfo;
    offset = pVideoMethodArg->offset;
    data = pVideoMethodArg->data;
    pHalHwInfo = pVideoMethodArg->pHalHwInfo;

    switch (offset)
    {
        case NVFF8_STOP_TRANSFER(0):
            if (pHalHwInfo->pDacHalInfo->CrtcInfo[0].DisplayType != DISPLAY_TYPE_DUALSURFACE)
            {
                // activate a PVIDEO_STOP and disappear the window by
                // setting SIZE_OUT to 0
                VIDEOCLASS_PRINTF((DBG_LEVEL_INFO, "NVRM: class63StopTransfer: set STOP_ACTIVE/IMMEDIATELY\n\r"));
                VIDEOCLASS_PRINTF((DBG_LEVEL_INFO, "NVRM:     PVIDEO BUFFER: 0x%x\n", REG_RD32(NV_PVIDEO_BUFFER)));
                REG_WR32(NV_PVIDEO_STOP,
                         DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _ACTIVE) |
                         DRF_DEF(_PVIDEO, _STOP, _METHOD, _IMMEDIATELY));
                REG_WR32(NV_PVIDEO_SIZE_OUT(0), 0);
                REG_WR32(NV_PVIDEO_SIZE_OUT(1), 0);
            }
            pHalHwInfo->pVideoHalInfo->Enabled = 0;
            pHalHwInfo->pVideoHalInfo->ColorKeyEnabled = 0;
            pHalHwInfo->pVideoHalInfo->ScaleFactor = 0;
            pVidFromMemHalObj->KickOffProc = class63InitXfer_NV20;
            break;
        case NVFF8_IMAGE_SCAN_NOTIFY(0,0):
        case NVFF8_IMAGE_SCAN_NOTIFY(0,1):
            buffNum = (offset == NVFF8_IMAGE_SCAN_NOTIFY(0,0)) ? 0 : 1;
            if (pHalHwInfo->pDacHalInfo->CrtcInfo[0].DisplayType == DISPLAY_TYPE_DUALSURFACE)
                class63DualSurface_NV20(pHalHwInfo, pVidFromMemHalObj, buffNum);
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

static VOID class63ColorKey_NV20
(
    PHALHWINFO pHalHwInfo,
    PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj,
    U032 buffNum
)
{
    U032 color, colorFormat;

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
                REG_WR32(NV_PVIDEO_COLOR_KEY, color & 0xFF);
                break;
            
            case NVFF6_SET_COLOR_FORMAT_LE_X16A1R5G5B5:
            case NVFF6_SET_COLOR_FORMAT_LE_X17R5G5B5:
                REG_WR32(NV_PVIDEO_COLOR_KEY, color & 0x7FFF);
                break;
                
            case NVFF6_SET_COLOR_FORMAT_LE_A16R5G6B5:
            case NVFF6_SET_COLOR_FORMAT_LE_A16Y16:
            case NVFF6_SET_COLOR_FORMAT_LE_X16Y16:
                REG_WR32(NV_PVIDEO_COLOR_KEY, color & 0xFFFF);
                break;
                
            case NVFF6_SET_COLOR_FORMAT_LE_A8R8G8B8:
            case NVFF6_SET_COLOR_FORMAT_LE_X8R8G8B8:                
                REG_WR32(NV_PVIDEO_COLOR_KEY, color & 0xFFFFFF);
                break; 
                
            default:
                REG_WR32(NV_PVIDEO_COLOR_KEY, color);
                break; 
        }
        VIDEO_FORMAT_FLD_WR_DEF(buffNum, _DISPLAY, _COLOR_KEY_EQUAL);
    } 
    else
    {
        // XXX does this do it??
        REG_WR32(NV_PVIDEO_COLOR_KEY, NV_PVIDEO_COLOR_KEY_VALUE_DONT_CARE);
        VIDEO_FORMAT_FLD_WR_DEF(buffNum, _DISPLAY, _ALWAYS);
    }
}

//
//      Program the video scalar to scan out the video to the TV encoder.
//
static VOID class63DualSurface_NV20(PHALHWINFO pHalHwInfo, PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj, U032 buffNum)

{
    U032    pitch0, pitch1, vbs, vbe, hbs, hbe, hw;
    U032    length, width, pitch, height;
    int     step_x, step_y;

    //
    // Class63 has set up the scalar to render into a window, but we'll
    // change it to render to the full TV screen.
    // This means getting the source buffer size and scaling up to the
    // current resolution.
    //

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
        // XXX for now we'll leave it at masking 12 bits
        step_x = (((width - 1) << 11) / (pHalHwInfo->pFbHalInfo->HorizDisplayWidth - 1)) & 0xfff;
        step_y = (((height - 1) << 11) / (pHalHwInfo->pFbHalInfo->VertDisplayWidth - 1)) & 0xfff;
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
    REG_WR32(NV_PRAMDAC_TV_VBLANK_START, vbs);   
    REG_WR32(NV_PRAMDAC_TV_VBLANK_END, vbe);    
    REG_WR32(NV_PRAMDAC_TV_HBLANK_START, hbs);     
    REG_WR32(NV_PRAMDAC_TV_HBLANK_END, hbe);    

    // Video scalar
    // XXX I guess we'll set both??
    REG_WR32(NV_PVIDEO_DS_DX(0), step_x);
    REG_WR32(NV_PVIDEO_DT_DY(0), step_y);
    REG_WR32(NV_PVIDEO_DS_DX(1), step_x);
    REG_WR32(NV_PVIDEO_DT_DY(1), step_y);
    //WritePriv32(scale, NV_PVIDEO_STEP_SIZE);        // scale factor 1:1

    VIDEO_FORMAT_FLD_WR_PITCH(0, pitch0);
    //WritePriv32(pitch0, NV_PVIDEO_BUFF0_PITCH);     // pitch buffer 0

    VIDEO_FORMAT_FLD_WR_PITCH(1, pitch1);
    //WritePriv32(pitch1, NV_PVIDEO_BUFF1_PITCH);     // pitch buffer 1

    // XXX I guess we'll set both??
    REG_WR32(NV_PVIDEO_SIZE_OUT(0), hw);
    REG_WR32(NV_PVIDEO_SIZE_OUT(1), hw);
    //WritePriv32(hw, NV_PVIDEO_WINDOW_SIZE);         // h (26:16), w (10:0)
}


//****************************************************************************************
//
//      Program the video scalar to scan out the framebuffer (desktop) to the TV encoder.
//      Or, turn off the video scalar for TV only or monitor.
VOID class63DualSurfaceDesktop_NV20(PHALHWINFO pHalHwInfo, U032 mode)
{
    U032 coeff=0, setup=0, vbs=0, vbe=0, hbs=0, hbe=0;
    U032 start_buf0=0, start_buf1=0, pitch0=0, pitch1=0, xy_pos=0, hw=0;
    U032 step_x=0, step_y=0;
    U032 fifo0, fifo1, reg32;
    int dualSurface = 0;

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
                step_x = DRF_DEF(_PVIDEO, _DS_DX, _RATIO, _UNITY) >> 1; // 2:1
                step_y = DRF_DEF(_PVIDEO, _DT_DY, _RATIO, _UNITY) >> 1; // 2:1
                //scale = 0x04000400;     // 2:1
            }
            else                                                            
            {
                hw = pHalHwInfo->pFbHalInfo->HorizDisplayWidth | (pHalHwInfo->pFbHalInfo->VertDisplayWidth << 16);
                step_x = DRF_DEF(_PVIDEO, _DS_DX, _RATIO, _UNITY);      // 1:1
                step_y = DRF_DEF(_PVIDEO, _DT_DY, _RATIO, _UNITY);      // 1:1
                //scale = 0x08000800;     // 1:1
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
        // XXX idle everything??
        REG_WR32(NV_PVIDEO_BUFFER, 0);

        REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, coeff);   
        REG_WR32(NV_PRAMDAC_TV_SETUP        , setup);          
        REG_WR32(NV_PRAMDAC_TV_VBLANK_START , vbs);   
        REG_WR32(NV_PRAMDAC_TV_VBLANK_END   , vbe);    
        REG_WR32(NV_PRAMDAC_TV_HBLANK_START , hbs);     
        REG_WR32(NV_PRAMDAC_TV_HBLANK_END   , hbe);    

        // Video scalar
        REG_WR32(NV_PVIDEO_DS_DX(0), step_x);
        REG_WR32(NV_PVIDEO_DT_DY(0), step_y);
        REG_WR32(NV_PVIDEO_DS_DX(1), step_x);
        REG_WR32(NV_PVIDEO_DT_DY(1), step_y);

        VIDEO_FORMAT_FLD_WR_PITCH(0, pitch0);

        VIDEO_FORMAT_FLD_WR_PITCH(1, pitch1);

        // XXX I guess we'll set both??
        REG_WR32(NV_PVIDEO_SIZE_OUT(0), hw);
        REG_WR32(NV_PVIDEO_SIZE_OUT(1), hw);

#if 0 
        // could set this, but for now lets just use the defaults
        FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_3, _WATER_MARK1, fifo0);
        FLD_WR_DRF_NUM(_PVIDEO, _DEBUG_2, _BURST1, fifo1);
        //WritePriv32(fifo0, NV_PVIDEO_FIFO_THRES);       // fifo fill threshold
        //WritePriv32(fifo1, NV_PVIDEO_FIFO_BURST);       // fifo burst length
#endif

        // XXX I guess we'll set both??
        REG_WR32(NV_PVIDEO_POINT_OUT(0), xy_pos);
        REG_WR32(NV_PVIDEO_POINT_OUT(1), xy_pos);
        //WritePriv32(xy_pos, NV_PVIDEO_WINDOW_START);    // x,y position within buffer (start display here)

        REG_WR32(NV_PVIDEO_OFFSET(0), start_buf0);
        //WritePriv32(start_buf0, NV_PVIDEO_BUFF0_START); // start address buffer 0

        REG_WR32(NV_PVIDEO_OFFSET(1), start_buf1);
        //WritePriv32(start_buf1, NV_PVIDEO_BUFF1_START); // start address buffer 1
            
        FLD_WR_DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _ACTIVE);  // enable scalar
        //FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _VIDEO, _ON);  // enable scalar

        VIDEO_FORMAT_FLD_WR_DEF(0, _COLOR, _LE_YB8CR8YA8CB8); // set format
        //FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _FORMAT, _CCIR);  // set format

        // switch to buffer 0, which will force the scalar to load new values
        FLD_WR_DRF_DEF(_PVIDEO, _BUFFER, _0_USE, _SET);
#if 0
        reg32 = REG_RD32(NV_PVIDEO_SU_STATE);
        ////reg32 = ReadPriv32(NV_PVIDEO_SU_STATE);
        reg32 &= ~0x00010000; // NV_PVIDEO_SU_STATE_BUFF0_IN_USE = 0
        REG_WR32(NV_PVIDEO_SU_STATE, reg32); // clear BUFF0_IN_USE
        //WritePriv32(reg32, NV_PVIDEO_SU_STATE); // clear BUFF0_IN_USE
#endif
    }    
    else    // need to set up for either TV only or Monitor only
    {

        reg32 = REG_RD32(NV_PRAMDAC_TV_SETUP);
        //reg32 = ReadPriv32(NV_PRAMDAC_TV_SETUP);
        reg32 &= NV_PRAMDAC_TV_SETUP_DEV_TYPE_SLAVE;
        reg32 |= setup;
        REG_WR32(0x680700, setup);   // TV SETUP
        //WritePriv32(setup, 0x680700);   // TV SETUP

        FLD_WR_DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _INACTIVE);  // disable scalar
        //FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _VIDEO, _OFF);  // disable scalar
    }
}

//---------------------------------------------------------------------------
//
//  Buffer transfer routines.
//
//---------------------------------------------------------------------------

RM_STATUS class63InitXfer_NV20
(
    PHALHWINFO    pHalHwInfo,
    VOID          *pHalObj,
    U032          buffNum
)
{
    RM_STATUS       status = RM_OK;
    PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj;

    VIDEOCLASS_PRINTF((DBG_LEVEL_INFO, "NVRM: class63InitXfer\r\n"));

    //
    // A nice cast to make the code more readable.
    //
    pVidFromMemHalObj = (PVIDEOFROMMEMHALOBJECT)pHalObj;

    //
    // Since we're not context switching any of the video registers across channels yet, we
    // can't just initialize the values once on boot and assume they'll look ok.  Let's 
    // hack for now and init them every new buffer.
    // 
    videoInit_NV20(pHalHwInfo, NV_VIDEO_FROM_MEMORY);

    if (((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_V8YB8U8YA8)) /* YUYV, YUY2, YUV2 */
        VIDEO_FORMAT_FLD_WR_DEF(0, _COLOR, _LE_CR8YB8CB8YA8);
    if (((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_YB8V8YA8U8)) /* UYVY, CCIR601    */
        VIDEO_FORMAT_FLD_WR_DEF(0, _COLOR, _LE_YB8CR8YA8CB8);

    pVidFromMemHalObj->KickOffProc = class63StartXfer_NV20;

    status = class63StartXfer_NV20(pHalHwInfo, (VOID *)pVidFromMemHalObj, buffNum);

    return (status);
}

//
// Start the transfer from the DMA buffer.
//
static RM_STATUS class63StartXfer_NV20
(
    PHALHWINFO    pHalHwInfo,
    VOID          *pHalObj,
    U032          buffNum
)
{
    PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj;
    RM_STATUS status = RM_OK;

    VIDEOCLASS_PRINTF((DBG_LEVEL_INFO, "NVRM: class63StartXfer\r\n"));

    pVidFromMemHalObj = (PVIDEOFROMMEMHALOBJECT)pHalObj;

    //
    // XXX (scottl):
    // I've made a *MAJOR* assumption here that we do not use the BBufferXfer
    // proc to handle RGB video.  In looking at the transfer method in 
    // in class63.c (formally c63nv20.c), it looks like it's entirely
    // vblank driven.  By making this assumption, I'm able to avoid having
    // vblank info in the HAL.
    //
#ifdef DEBUG
    if (((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_Y8) ||
        ((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_GY1R5G5B5) ||
        ((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_R5G6B5) ||
        ((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_GY1X7R8G8B8))
    {
        VIDEOCLASS_PRINTF((DBG_LEVEL_ERRORS, "NVRM: class63StartXfer: format is RGB!\r\n"));
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

    if (((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_V8YB8U8YA8)) /* YUYV, YUY2, YUV2 */
    {
        VIDEO_FORMAT_FLD_WR_DEF(buffNum, _COLOR, _LE_CR8YB8CB8YA8);
    }
    if (((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_YB8V8YA8U8)) /* UYVY, CCIR601    */
    {
        VIDEO_FORMAT_FLD_WR_DEF(buffNum, _COLOR, _LE_YB8CR8YA8CB8);
    }

    // Write the offset straight away (must be 64byte aligned). If any of the low order
    // 6 bits are set from ImageScan.offset, we'll divide by 2 to convert bytes to texels
    // and shift this value by 4 to get into the integer portion of POINT_IN_S.
    REG_WR32(NV_PVIDEO_OFFSET(buffNum), pVidFromMemHalObj->Buffer[buffNum].Start);
    REG_WR32(NV_PVIDEO_POINT_IN(buffNum),
             (REG_RD32(NV_PVIDEO_POINT_IN(buffNum)) & ~(DRF_MASK(NV_PVIDEO_POINT_IN_S) << DRF_SHIFT(NV_PVIDEO_POINT_IN_S))) |
             DRF_NUM(_PVIDEO, _POINT, _IN_S, ((pVidFromMemHalObj->Buffer[buffNum].Start & 0x3F) >> 1) << 4));

    // Set the pitch (must be 64byte aligned).
    // If the LSB's are 0x2 then we're doing a bob and POINT_IN_T needs to be -1/2.
    //
    VIDEO_FORMAT_FLD_WR_PITCH(buffNum, pVidFromMemHalObj->Buffer[buffNum].Pitch);
    if ((pVidFromMemHalObj->Buffer[buffNum].Pitch & 0x3) == 0x2)
    {
        REG_WR32(NV_PVIDEO_POINT_IN(buffNum),
                 (REG_RD32(NV_PVIDEO_POINT_IN(buffNum)) & ~(DRF_MASK(NV_PVIDEO_POINT_IN_T) << DRF_SHIFT(NV_PVIDEO_POINT_IN_T))) |
                 DRF_NUM(_PVIDEO, _POINT, _IN_T, (0xFFF8 >> 1)));
    }

    REG_WR32(NV_PVIDEO_POINT_OUT(buffNum), pVidFromMemHalObj->Buffer[buffNum].VideoStart);
    REG_WR32(NV_PVIDEO_SIZE_OUT(buffNum), pVidFromMemHalObj->Buffer[buffNum].VideoSize);
    REG_WR32(NV_PVIDEO_DS_DX(buffNum), pVidFromMemHalObj->Buffer[buffNum].VideoScaleX);
    REG_WR32(NV_PVIDEO_DT_DY(buffNum), pVidFromMemHalObj->Buffer[buffNum].VideoScaleY);

    class63ColorKey_NV20(pHalHwInfo, pVidFromMemHalObj, buffNum);

    pVidFromMemHalObj->Buffer[buffNum].State = OVERLAY_BUFFER_NOTIFY_PENDING;

    // HW now owns the buffer
    if (buffNum == 0)
        FLD_WR_DRF_DEF(_PVIDEO, _BUFFER, _0_USE, _SET);
    else
        FLD_WR_DRF_DEF(_PVIDEO, _BUFFER, _1_USE, _SET);

#ifdef DEBUG_TIMING
    if (buffNum == 0)
        vmmOutDebugString("\n0B");
    else
        vmmOutDebugString("\n1B");
#endif
    return (status);
}
