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
* Module: class047.C                                                        *
*   This module implements the NV04_VIDEO_OVERLAY object class and          *
*   its corresponding methods.                                              *
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

//
// The following routines are used by the HAL video engine
// manager in kernel/video/vidnv04.c.
//
RM_STATUS class047Method_NV04(PVIDEOMETHODARG_000);
RM_STATUS class047InitXfer_NV04(PHALHWINFO, VOID *, U032);
RM_STATUS class047GetEventStatus_NV04(PVIDEOGETEVENTSTATUSARG_000);
RM_STATUS class047Service_NV04(PVIDEOSERVICEEVENTARG_000);

//
// Statics
//
static RM_STATUS class047StartXfer_NV04(PHALHWINFO, VOID *, U032);
static VOID class047KickoffBuffer_NV04(PHALHWINFO, PVIDEO_OVERLAY_HAL_OBJECT, U032);

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
//  Miscellaneous class support routines.
//
//---------------------------------------------------------------------------

RM_STATUS class047InitXfer_NV04
(
    PHALHWINFO  pHalHwInfo,
    VOID        *pHalObjInfo,
    U032        buffNum
)
{
    RM_STATUS       status = RM_OK;
    V032            stateSU;
    V032            stateOE;
    V032            stateOESU;
    PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj;

    VIDEOCLASS_PRINTF((DBG_LEVEL_INFO, "NVRM: class047InitXfer_NV04\r\n"));

    pOverlayHalObj = (PVIDEO_OVERLAY_HAL_OBJECT)pHalObjInfo;
        
    //
    // Since we're not context switching any of the video registers across channels yet, we
    // can't just initialize the values once on boot and assume they'll look ok.  Let's 
    // hack for now and init them every new buffer.
    // 
    videoInit_NV04(pHalHwInfo, NV04_VIDEO_OVERLAY);

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
        
    status = class047StartXfer_NV04(pHalHwInfo, pOverlayHalObj, buffNum);
    if (status == RM_OK)
    {
    	// Actually kickoff the buffer that was just marked busy by writing the registers.
        // After this, buffers are actually kicked off after we get the completion interrupt.
        if (pOverlayHalObj->Overlay[0].State == OVERLAY_BUFFER_BUSY)
        {
            class047KickoffBuffer_NV04(pHalHwInfo, pOverlayHalObj, 0);
        }
        else
        {
            // Kickoff buffer 1.
            class047KickoffBuffer_NV04(pHalHwInfo, pOverlayHalObj, 1);
        }

        //
        // Set the enable bit in VBLANK. This is to prevent enabling the video in the middle of
        // the video window. That may cause only the "end window" processing to be done without
        // having done the "start window" processing. Making sure to enable the video pipeline
        // from VBLANK (and not doing it here) fixes 19991220-082127.
        //
        pHalHwInfo->pVideoHalInfo->UpdateFlags |= UPDATE_HWINFO_VIDEO_ENABLE;
        
        //
        // This instance has been initialized.  Call StartXfer for the next
        // buffer updates.
        //
        pOverlayHalObj->KickOffProc = class047StartXfer_NV04;
    }

    return (status);
}

static VOID class047KickoffBuffer_NV04
(
    PHALHWINFO                  pHalHwInfo,
    PVIDEO_OVERLAY_HAL_OBJECT   pOverlayHalObj,
    U032                        buffNum
)
{
    V032 stateSU;
    U032 data32;

    VIDEOCLASS_PRINTF((DBG_LEVEL_INFO, "NVRM: class047KickoffBuffer"));

    stateSU = REG_RD32(NV_PVIDEO_SU_STATE);

    if (buffNum == 0) {
        REG_WR32(NV_PVIDEO_BUFF0_START,  pOverlayHalObj->Overlay[buffNum].OverlayStart);
        REG_WR32(NV_PVIDEO_BUFF0_PITCH,  (pOverlayHalObj->Overlay[buffNum].Pitch & 0x00007FF0));
    } else {
        REG_WR32(NV_PVIDEO_BUFF1_START,  pOverlayHalObj->Overlay[buffNum].OverlayStart);
        REG_WR32(NV_PVIDEO_BUFF1_PITCH,  (pOverlayHalObj->Overlay[buffNum].Pitch & 0x00007FF0));
    }
        
    // this is an NV10 emulator
    // we don't have fractional bobbing abilities
    // we can do 0, -1/4 and -1/2.
    // since we want to keep the bobbing even and can't do -1/4 we'll
    // only use 0 and -1/2.
    // Therefor the system will be to check bit 3 of the 12.4 value
    // if bit 3 is set then we set the bob to -1/2 otherwise 
    if (buffNum == 0) {
        if (pOverlayHalObj->Overlay[buffNum].PointIn_t & 0x8)
            FLD_WR_DRF_NUM(_PVIDEO, _BUFF0_OFFSET, _Y, 2 );
        else
            FLD_WR_DRF_NUM(_PVIDEO, _BUFF0_OFFSET, _Y, 0);
    } else {
        if (pOverlayHalObj->Overlay[buffNum].PointIn_t & 0x8)
            FLD_WR_DRF_NUM(_PVIDEO, _BUFF1_OFFSET, _Y, 2 );
        else
            FLD_WR_DRF_NUM(_PVIDEO, _BUFF1_OFFSET, _Y, 0);
    }

    FLD_WR_DRF_DEF(_PVIDEO, _CONTROL_Y, _LINE, _HALF);

    if (IsNV4_NV04(pHalHwInfo->pMcHalInfo) && 
        (pHalHwInfo->pDacHalInfo->CrtcInfo[0].MonitorType == MONITOR_TYPE_NTSC ||
         pHalHwInfo->pDacHalInfo->CrtcInfo[0].MonitorType == MONITOR_TYPE_PAL))
    {
        // NV4 HW bug, shift position on TV
        pOverlayHalObj->Overlay[buffNum].WindowStart += NV4_TV_DAC_SHIFT; 
    }
    REG_WR32(NV_PVIDEO_WINDOW_START, pOverlayHalObj->Overlay[buffNum].WindowStart);
    REG_WR32(NV_PVIDEO_STEP_SIZE, pOverlayHalObj->Overlay[buffNum].ScaleFactor);
    REG_WR32(NV_PVIDEO_KEY, pOverlayHalObj->Overlay[buffNum].ColorKey);
    if((pHalHwInfo->pDacHalInfo->CrtcInfo[0].DisplayType == DISPLAY_TYPE_FLAT_PANEL) &&
        (REG_RD32(NV_PRAMDAC_FP_DEBUG_3) != 0x10001000)) // DFP scaling?
    {
        data32 = pOverlayHalObj->Overlay[buffNum].WindowSize;
        if (data32 != 0) 
            data32 -= 0x00010000;   // subtract 1 from Y
        REG_WR32(NV_PVIDEO_WINDOW_SIZE, data32);
    }
    else
        REG_WR32(NV_PVIDEO_WINDOW_SIZE, pOverlayHalObj->Overlay[buffNum].WindowSize);

    if (pOverlayHalObj->Overlay[buffNum].DisplayWhen == NV047_SET_OVERLAY_FORMAT_DISPLAY_ALWAYS)
        FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _KEY, _OFF);
    else
        FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _KEY, _ON);

    if (pOverlayHalObj->Overlay[buffNum].OverlayFormat == NV047_SET_OVERLAY_FORMAT_COLOR_LE_YB8CR8YA8CB8)
        FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _FORMAT, _CCIR);
    else
        FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _FORMAT, _YUY2);

    // Mark buffer as NOTIFY_PENDING.
    pOverlayHalObj->Overlay[buffNum].State = OVERLAY_BUFFER_NOTIFY_PENDING;
    if (buffNum == 0)
        stateSU ^= 1 << SF_SHIFT(NV_PVIDEO_SU_STATE_BUFF0_IN_USE);
    else
        stateSU ^= 1 << SF_SHIFT(NV_PVIDEO_SU_STATE_BUFF1_IN_USE);

#ifdef DEBUG_TIMING
    vmmOutDebugString("0K");
#endif

    REG_WR32(NV_PVIDEO_SU_STATE, stateSU);
}

//
// Start the transfer from the DMA buffer.
//
static RM_STATUS class047StartXfer_NV04
(
    PHALHWINFO  pHalHwInfo,
    VOID        *pHalObjInfo,
    U032        buffNum
)
{
    U032 overlayStartAddr;
    PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj;

    VIDEOCLASS_PRINTF((DBG_LEVEL_INFO, "NVRM: class047StartXfer_NV04\r\n"));

    pOverlayHalObj = (PVIDEO_OVERLAY_HAL_OBJECT)pHalObjInfo;

    // Calculate the offset from the begining of frame buffer where the overlay engine should start
    // scanning out.
#if 0
    overlayStartAddr =
        pOverlayHalObj->Overlay[buffNum].OverlayCtxDma->BufferBase + 
        pOverlayHalObj->Overlay[buffNum].Offset +
        ((pOverlayHalObj->Overlay[buffNum].Pitch * pOverlayObject->Overlay[buffNum].PointIn_t) +
         (pOverlayHalObj->Overlay[buffNum].PointIn_s << 1));
    pOverlayHalObj->Overlay[buffNum].OverlayStart = overlayStartAddr - (U032)pHalHwInfo->fbBaseAddr;
#else
    // remember that the point in specs are in 12.4 fixed point... so shift right by 4
    // and remember that's in pixels so 2 bytes per pixel
    
    if ((pOverlayHalObj->Overlay[buffNum].PointIn_t >>4 ) == 0xFFF ) {
        // for when the offset is ACTUALLY zero or just  -1/2
        overlayStartAddr = pOverlayHalObj->Overlay[buffNum].Offset +
            (pOverlayHalObj->Overlay[buffNum].PointIn_s >> 3);
    } else {
      if (pOverlayHalObj->Overlay[buffNum].PointIn_t & 0x8 ) {
          // there is a negative value in the bob, so add one to
          // the offset here
          overlayStartAddr = pOverlayHalObj->Overlay[buffNum].Offset +
                         ((pOverlayHalObj->Overlay[buffNum].Pitch * ((pOverlayHalObj->Overlay[buffNum].PointIn_t >> 4) + 1)) +
                          (pOverlayHalObj->Overlay[buffNum].PointIn_s >> 3));
      } else {
          overlayStartAddr = pOverlayHalObj->Overlay[buffNum].Offset +
              ((pOverlayHalObj->Overlay[buffNum].Pitch * (pOverlayHalObj->Overlay[buffNum].PointIn_t >> 4)) +
               (pOverlayHalObj->Overlay[buffNum].PointIn_s >> 3));
      }
    }
    
    pOverlayHalObj->Overlay[buffNum].OverlayStart = overlayStartAddr;
#endif

    // Mark the buffer as BUSY. That is all we do here. The hardware registers are
    // programmed in videoService after the current buffer completion.
    pOverlayHalObj->Overlay[buffNum].State = OVERLAY_BUFFER_BUSY;

    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

//
// class047Method
//
// Process all class047 methods.
//
#define OVERLAY_BUFFER_IS_IDLE(obj,buffer) (obj->Overlay[buffer].State == OVERLAY_BUFFER_IDLE)

RM_STATUS class047Method_NV04(PVIDEOMETHODARG_000 pVideoMethodArg)
{
    PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj;
    PHALHWINFO pHalHwInfo;
    U032 buffNum, offset, data;

    pOverlayHalObj = (PVIDEO_OVERLAY_HAL_OBJECT)pVideoMethodArg->pHalObjInfo;
    offset = pVideoMethodArg->offset;
    data = pVideoMethodArg->data;
    pHalHwInfo = pVideoMethodArg->pHalHwInfo;

    switch (offset)
    {
        case NV047_STOP_OVERLAY(0):
        case NV047_STOP_OVERLAY(1):
            if (pHalHwInfo->pDacHalInfo->CrtcInfo[0].DisplayType != DISPLAY_TYPE_DUALSURFACE)
                REG_WR32(NV_PVIDEO_OVERLAY, 0);     // quick shutoff
            pHalHwInfo->pVideoHalInfo->Enabled = 0;
            pHalHwInfo->pVideoHalInfo->ColorKeyEnabled = 0;
            pHalHwInfo->pVideoHalInfo->ScaleFactor = 0;
            pOverlayHalObj->KickOffProc = class047InitXfer_NV04;
            break;
        case NV047_SET_CONTEXT_DMA_NOTIFIES:
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, 0) ||
                !OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, 1))
                return NV047_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
            break;
        case NV047_SET_CONTEXT_DMA_OVERLAY(0):
        case NV047_SET_CONTEXT_DMA_OVERLAY(1):
        {
            buffNum = (offset == NV047_SET_CONTEXT_DMA_OVERLAY(0)) ? 0 : 1;
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, buffNum))
                return NV047_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
            break;
        }
        case NV047_SET_OVERLAY_COLORKEY(0):
        case NV047_SET_OVERLAY_COLORKEY(1):
        {
            buffNum = (offset == NV047_SET_OVERLAY_COLORKEY(0)) ? 0 : 1;
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, buffNum))
                return NV047_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            pOverlayHalObj->Overlay[buffNum].ColorKey = data;
            break;
        }
        case NV047_SET_OVERLAY_OFFSET(0):
        case NV047_SET_OVERLAY_OFFSET(1):
        {
            buffNum = (offset == NV047_SET_OVERLAY_OFFSET(0)) ? 0 : 1;
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, buffNum))
                return NV047_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            pOverlayHalObj->Overlay[buffNum].Offset = data;
            break;
        }
        case NV047_SET_OVERLAY_SIZE_IN(0):
        case NV047_SET_OVERLAY_SIZE_IN(1):
        {
            buffNum = (offset == NV047_SET_OVERLAY_SIZE_IN(0)) ? 0 : 1;
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, buffNum))
                return NV047_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            pOverlayHalObj->Overlay[buffNum].SizeIn_width  = 
                (U016)((data >> DRF_SHIFT(NV047_SET_OVERLAY_SIZE_IN_WIDTH)) & 
                       DRF_MASK(NV047_SET_OVERLAY_SIZE_IN_WIDTH));
            pOverlayHalObj->Overlay[buffNum].SizeIn_height = 
                (U016)((data >> DRF_SHIFT(NV047_SET_OVERLAY_SIZE_IN_HEIGHT)) & 
                       DRF_MASK(NV047_SET_OVERLAY_SIZE_IN_HEIGHT));
            break;
        }
        case NV047_SET_OVERLAY_POINT_IN(0):
        case NV047_SET_OVERLAY_POINT_IN(1):
        {
            buffNum = (offset == NV047_SET_OVERLAY_POINT_IN(0)) ? 0 : 1;
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, buffNum))
                return NV047_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            pOverlayHalObj->Overlay[buffNum].PointIn_s  = 
                (U016)((data >> DRF_SHIFT(NV047_SET_OVERLAY_POINT_IN_S)) & 
                       DRF_MASK(NV047_SET_OVERLAY_POINT_IN_S));
            pOverlayHalObj->Overlay[buffNum].PointIn_t = 
                (U016)((data >> DRF_SHIFT(NV047_SET_OVERLAY_POINT_IN_T)) & 
                       DRF_MASK(NV047_SET_OVERLAY_POINT_IN_T));
            break;
        }
        case NV047_SET_OVERLAY_DS_DX(0):
        case NV047_SET_OVERLAY_DS_DX(1):
        {
            buffNum = (offset == NV047_SET_OVERLAY_DS_DX(0)) ? 0 : 1;
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, buffNum))
                return NV047_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
            pOverlayHalObj->Overlay[buffNum].DsDx = data;

            // Setup ScaleFactor.
            pOverlayHalObj->Overlay[buffNum].ScaleFactor = 
                DRF_NUM(_PVIDEO, _STEP_SIZE, _X, videoConvertScaleFactor_NV04(pOverlayHalObj->Overlay[buffNum].DsDx))
                | DRF_NUM(_PVIDEO, _STEP_SIZE, _Y, videoConvertScaleFactor_NV04(pOverlayHalObj->Overlay[buffNum].DtDy));
            break;
        }
        case NV047_SET_OVERLAY_DT_DY(0):
        case NV047_SET_OVERLAY_DT_DY(1):
        {
            buffNum = (offset == NV047_SET_OVERLAY_DT_DY(0)) ? 0 : 1;
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, buffNum))
                return NV047_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            pOverlayHalObj->Overlay[buffNum].DtDy = data;

            // Setup ScaleFactor.
            pOverlayHalObj->Overlay[buffNum].ScaleFactor = 
                DRF_NUM(_PVIDEO, _STEP_SIZE, _X, videoConvertScaleFactor_NV04(pOverlayHalObj->Overlay[buffNum].DsDx))
                | DRF_NUM(_PVIDEO, _STEP_SIZE, _Y, videoConvertScaleFactor_NV04(pOverlayHalObj->Overlay[buffNum].DtDy));

            break;
        }
        case NV047_SET_OVERLAY_POINT_OUT(0):
        case NV047_SET_OVERLAY_POINT_OUT(1):
        {
            U032 x, y;

            buffNum = (offset == NV047_SET_OVERLAY_POINT_OUT(0)) ? 0 : 1;
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, buffNum))
                return NV047_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            x = (data >> DRF_SHIFT(NV047_SET_OVERLAY_POINT_OUT_X)) & 
                DRF_MASK(NV047_SET_OVERLAY_POINT_OUT_X);
            y = (data >> DRF_SHIFT(NV047_SET_OVERLAY_POINT_OUT_Y)) & 
                DRF_MASK(NV047_SET_OVERLAY_POINT_OUT_Y);

            // Setup the window start. 
            pOverlayHalObj->Overlay[buffNum].WindowStart = 
                DRF_NUM(_PVIDEO, _WINDOW_START, _X, x) | DRF_NUM(_PVIDEO, _WINDOW_START, _Y, y); 

            break;
        }
        case NV047_SET_OVERLAY_SIZE_OUT(0):
        case NV047_SET_OVERLAY_SIZE_OUT(1):
        {
            U032 width, height;

            buffNum = (offset == NV047_SET_OVERLAY_SIZE_OUT(0)) ? 0 : 1;
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, buffNum))
                return NV047_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            width  = (data >> DRF_SHIFT(NV047_SET_OVERLAY_SIZE_OUT_WIDTH)) & 
                     DRF_MASK(NV047_SET_OVERLAY_SIZE_OUT_WIDTH);
            height = (data >> DRF_SHIFT(NV047_SET_OVERLAY_SIZE_OUT_HEIGHT)) & 
                     DRF_MASK(NV047_SET_OVERLAY_SIZE_OUT_HEIGHT);

            // Setup the window size.
            pOverlayHalObj->Overlay[buffNum].WindowSize = 
                DRF_NUM(_PVIDEO, _WINDOW_SIZE, _X, width) | 
                DRF_NUM(_PVIDEO, _WINDOW_SIZE, _Y, height);

            break;
        }
        case NV047_SET_OVERLAY_FORMAT(0):
        case NV047_SET_OVERLAY_FORMAT(1):
        {
            buffNum = (offset == NV047_SET_OVERLAY_FORMAT(0)) ? 0 : 1;
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, buffNum))
                return NV047_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            pOverlayHalObj->Overlay[buffNum].Pitch  = 
                (U016)((data >> DRF_SHIFT(NV047_SET_OVERLAY_FORMAT_PITCH)) & 
                       DRF_MASK(NV047_SET_OVERLAY_FORMAT_PITCH));
                
            pOverlayHalObj->Overlay[buffNum].OverlayFormat = 
                (U016)((data >> DRF_SHIFT(NV047_SET_OVERLAY_FORMAT_COLOR)) & 
                       DRF_MASK(NV047_SET_OVERLAY_FORMAT_COLOR));
            
            pOverlayHalObj->Overlay[buffNum].DisplayWhen  = 
                (U016)((data >> DRF_SHIFT(NV047_SET_OVERLAY_FORMAT_DISPLAY)) & 
                       DRF_MASK(NV047_SET_OVERLAY_FORMAT_DISPLAY));


            // call the kickoff proc
			pOverlayHalObj->KickOffProc(pHalHwInfo, pOverlayHalObj, buffNum);

            break;
        }
        case NV047_SET_OVERLAY_POINT_OUT_A:
        {
            U032 x, y;

            x = (data >> DRF_SHIFT(NV047_SET_OVERLAY_POINT_OUT_X)) & 
                DRF_MASK(NV047_SET_OVERLAY_POINT_OUT_X);
            y = (data >> DRF_SHIFT(NV047_SET_OVERLAY_POINT_OUT_Y)) & 
                DRF_MASK(NV047_SET_OVERLAY_POINT_OUT_Y);
            // Setup the window start in both the overlay buffers. 
            pOverlayHalObj->Overlay[0].WindowStart = 
                DRF_NUM(_PVIDEO, _WINDOW_START, _X, x) |
                DRF_NUM(_PVIDEO, _WINDOW_START, _Y, y); 
            pOverlayHalObj->Overlay[1].WindowStart = 
                DRF_NUM(_PVIDEO, _WINDOW_START, _X, x) |
                DRF_NUM(_PVIDEO, _WINDOW_START, _Y, y); 

            // Actually write these values to the hardware right away.
            // There will not be a  notify method to kick this off.
            REG_WR32(NV_PVIDEO_WINDOW_START, pOverlayHalObj->Overlay[0].WindowStart);
            break;
        }
        default:
            // HAL doesn't have anything to do for this particular method
            ;
    }
    return NV047_NOTIFICATION_STATUS_DONE_SUCCESS;
}

//---------------------------------------------------------------------------
//
//  Exception handling routines.
//
//---------------------------------------------------------------------------

//
// class047 exception data fetcher
//
// Let's RM know which buffer (if any) needs servicing.
//
RM_STATUS class047GetEventStatus_NV04(PVIDEOGETEVENTSTATUSARG_000 pVideoGetEventStatusArg)
{
    PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj;
    PHALHWINFO pHalHwInfo;
    U032 intr0;
    V032 stateOE;
    V032 stateSU;
    V032 stateRM;
    V032 stateOESU;
    V032 stateOERM;

    pHalHwInfo = pVideoGetEventStatusArg->pHalHwInfo;

    pOverlayHalObj = (PVIDEO_OVERLAY_HAL_OBJECT)pVideoGetEventStatusArg->pHalObjInfo;

    //
    // Default is that none of the buffers needs servicing.  So tell
    // RM to bypass the nvHalVideoServiceEvent handler.
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
        VIDEOCLASS_PRINTF( (0x10, "                                                   stateOE = 0x%x\n",stateOE));
        VIDEOCLASS_PRINTF( (0x10, "                                                   stateSU = 0x%x\n",stateSU));
        VIDEOCLASS_PRINTF( (0x10, "                                                   stateRM = 0x%x\n",stateRM));
#endif

        if ((stateOESU & 0x00010000))
        {
            if (stateOERM & 0x00000001)
            {
#ifdef DEBUG_TIMING            
                DBG_PRINT_TIME("                                                   VideoServiceEvent0: ",REG_RD32(NV_PTIMER_TIME_0) - time0lo);
                
                time0hi = REG_RD32(NV_PTIMER_TIME_1);
                time0lo = REG_RD32(NV_PTIMER_TIME_0);
#endif
                // Mark buffer 0 pending if notify has been setup.
                if (pOverlayHalObj->Overlay[0].State == OVERLAY_BUFFER_NOTIFY_PENDING)
                    pVideoGetEventStatusArg->events |= (1 << 0);
            }
        }
        
        if ((stateOESU & 0x00100000))
        {
            if (stateOERM & 0x00000010)
            {

#ifdef DEBUG_TIMING            
                DBG_PRINT_TIME("                                                   VideoServiceEvent1: ",REG_RD32(NV_PTIMER_TIME_0) - time0lo);
                
                time0hi = REG_RD32(NV_PTIMER_TIME_1);
                time0lo = REG_RD32(NV_PTIMER_TIME_0);
#endif
                // Mark buffer 1 pending if notify has been setup.
                if (pOverlayHalObj->Overlay[1].State == OVERLAY_BUFFER_NOTIFY_PENDING)
                    pVideoGetEventStatusArg->events |= (1 << 1);
            }
        }
    }

    return RM_OK;
}

//
// class047 exception handler
//
// This interface relies on values setup by the nvHalVideoGetEventStatus
// interface (i.e. the PVIDEO_INTR_0 register contents).  We go to the
// hardware to get the rest of it.
//
RM_STATUS class047ServiceEvent_NV04(PVIDEOSERVICEEVENTARG_000 pVideoServiceEventArg)
{
    PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj;
    PHALHWINFO pHalHwInfo;
    U032 intr0;
    V032 stateOE;
    V032 stateSU;
    V032 stateRM;
    V032 stateOESU;
    V032 stateOERM;

    pHalHwInfo = pVideoServiceEventArg->pHalHwInfo;

    pOverlayHalObj = (PVIDEO_OVERLAY_HAL_OBJECT)pVideoServiceEventArg->pHalObjInfo;

    //
    // The pending interrupt register value was saved off by the
    // GetEventStatus entry point.
    //
    intr0 = pVideoServiceEventArg->intrStatus;

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
        VIDEOCLASS_PRINTF( (0x10, "                                                   stateOE = 0x%x\n",stateOE));
        VIDEOCLASS_PRINTF( (0x10, "                                                   stateSU = 0x%x\n",stateSU));
        VIDEOCLASS_PRINTF( (0x10, "                                                   stateRM = 0x%x\n",stateRM));
#endif

        //
        // Buffer 0.
        //
        if ((stateOESU & 0x00010000))
        {
            if (stateOERM & 0x00000001)
            {
                if (pVideoServiceEventArg->events & (1 << 0))
                    pOverlayHalObj->Overlay[0].State = OVERLAY_BUFFER_IDLE;

#ifdef DEBUG_TIMING            
                DBG_PRINT_TIME("                                                   VideoServiceEvent0: ",REG_RD32(NV_PTIMER_TIME_0) - time0lo);
                
                time0hi = REG_RD32(NV_PTIMER_TIME_1);
                time0lo = REG_RD32(NV_PTIMER_TIME_0);
#endif // DEBUG_TIMING                  


                stateRM ^= 1 << SF_SHIFT(NV_PVIDEO_RM_STATE_BUFF0_INTR_NOTIFY);
                REG_WR32(NV_PVIDEO_RM_STATE, stateRM);
                
                if (pOverlayHalObj->Overlay[1].State == OVERLAY_BUFFER_BUSY)
                {
                	// Software buffer 1 is ready. Kick it off.
                    class047KickoffBuffer_NV04(pHalHwInfo, pOverlayHalObj, 1 /* buffNum */);
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
                    if (pOverlayHalObj->Overlay[0].State == OVERLAY_BUFFER_BUSY)
                    {
                        class047KickoffBuffer_NV04(pHalHwInfo, pOverlayHalObj, 0 /* buffNum */);
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
                if (pVideoServiceEventArg->events & (1 << 1))
                    pOverlayHalObj->Overlay[1].State = OVERLAY_BUFFER_IDLE;

#ifdef DEBUG_TIMING            
                DBG_PRINT_TIME("                                                   VideoServiceEvent1: ",REG_RD32(NV_PTIMER_TIME_0) - time0lo);
                
                time0hi = REG_RD32(NV_PTIMER_TIME_1);
                time0lo = REG_RD32(NV_PTIMER_TIME_0);
#endif // DEBUG_TIMING                  

                stateRM ^= 1 << SF_SHIFT(NV_PVIDEO_RM_STATE_BUFF1_INTR_NOTIFY);
                REG_WR32(NV_PVIDEO_RM_STATE, stateRM);
                
                if (pOverlayHalObj->Overlay[0].State == OVERLAY_BUFFER_BUSY)
                {
                	// Software buffer 0 is ready. Kick it off.
                    class047KickoffBuffer_NV04(pHalHwInfo, pOverlayHalObj, 0 /* buffNum */);
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
                    if (pOverlayHalObj->Overlay[1].State == OVERLAY_BUFFER_BUSY)
                    {
                        class047KickoffBuffer_NV04(pHalHwInfo, pOverlayHalObj, 1 /* buffNum */);
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

        VIDEOCLASS_PRINTF( (0x10, "                                                   stateOE = 0x%x\n",stateOE));
        VIDEOCLASS_PRINTF( (0x10, "                                                   stateSU = 0x%x\n",stateSU));
        VIDEOCLASS_PRINTF( (0x10, "                                                   stateRM = 0x%x\n",stateRM));
#endif
    }  // Notify was pending
    
    // Return the current interrupt pending state.
    intr0 = REG_RD32(NV_PVIDEO_INTR_0);
    pVideoServiceEventArg->intrStatus = intr0 & DRF_DEF(_PVIDEO, _INTR_0, _NOTIFY, _PENDING);

    return RM_OK;
}
