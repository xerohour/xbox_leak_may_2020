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

/*********************** Chip Specific HAL VIDEO Routines ******************\
*                                                                           *
* Module: VIDNV10.c                                                         *
*   The NV10 specific HAL VIDEO routines reside in this file.               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <stddef.h>    // for size_t
#include <nv_ref.h>
#include <nv10_ref.h>
#include <nvrm.h>
#include <nv10_hal.h>
#include "nvhw.h"
#include "nvhalcpp.h"

// HAL interface forwards.
RM_STATUS nvHalVideoControl_NV10(VOID *);
RM_STATUS nvHalVideoAlloc_NV10(VOID *);
RM_STATUS nvHalVideoFree_NV10(VOID *);
RM_STATUS nvHalVideoMethod_NV10(VOID *);
RM_STATUS nvHalVideoGetEventStatus_NV10(VOID *);
RM_STATUS nvHalVideoServiceEvent_NV10(VOID *);

// Exports needed by class-dependent files in modular/nv10.
RM_STATUS videoInit_NV10(PHALHWINFO, U032);
U016 videoConvertScaleFactor_NV10(S012d020);
BOOL videoHwOwnsBuffer_NV10(PHALHWINFO, U032, PVIDEO_OVERLAY_HAL_OBJECT);
VOID videoAdjustScalarForTV_NV10(PHALHWINFO, U032);
VOID videoKickOffOverlay_NV10(PHALHWINFO, PVIDEO_OVERLAY_HAL_OBJECT, U032);
VOID videoStopOverlay_NV10(PHALHWINFO, PVIDEO_OVERLAY_HAL_OBJECT, U032);

// Exception handling routines.
static RM_STATUS videoGetEventStatus_NV10(PVIDEOGETEVENTSTATUSARG_000);
static RM_STATUS videoServiceEvent_NV10(PVIDEOSERVICEEVENTARG_000);

//
// nvHalVideoControl
//
RM_STATUS
nvHalVideoControl_NV10(VOID *arg)
{
    PVIDEOCONTROLARG_000 pVideoControlArg = (PVIDEOCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo = pVideoControlArg->pHalHwInfo;
    PHWREG nvAddr;

    //
    // Verify interface revision.
    //
    if (pVideoControlArg->id != VIDEO_CONTROL_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalVideoControl bad revision ", pVideoControlArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    pHalHwInfo = pVideoControlArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;
    switch (pVideoControlArg->cmd)
    {
        case VIDEO_CONTROL_UPDATE:
        case VIDEO_CONTROL_LOAD:
            break;
        case VIDEO_CONTROL_INIT:
        {
            U032 i;

            pVideoControlArg->pHalHwInfo->pVideoHalInfo->Enabled = 0;

            //
            // Establish default register settings since we
            // don't use the videoInit_NV10 routine when doing
            // buffer kickoffs for these classes.
            //
            for (i = 0; i < 2; i++)
            {
                REG_WR32(NV_PVIDEO_LUMINANCE(i),
                         DRF_DEF(_PVIDEO, _LUMINANCE, _CONTRAST, _UNITY) |
                         DRF_DEF(_PVIDEO, _LUMINANCE, _BRIGHTNESS, _UNITY));
                REG_WR32(NV_PVIDEO_CHROMINANCE(i),
                         DRF_DEF(_PVIDEO, _CHROMINANCE, _SAT_COS, _UNITY) |
                         DRF_DEF(_PVIDEO, _CHROMINANCE, _SAT_SIN, _UNITY));
                REG_WR32(NV_PVIDEO_DS_DX(i),
                         DRF_DEF(_PVIDEO, _DS_DX, _RATIO, _UNITY));
                REG_WR32(NV_PVIDEO_DT_DY(i),
                         DRF_DEF(_PVIDEO, _DT_DY, _RATIO, _UNITY));
                REG_WR32(NV_PVIDEO_POINT_IN(i), 0);
                REG_WR32(NV_PVIDEO_SIZE_IN(i), 0xFFFFFFFF);
            }
            break;
        }
        case VIDEO_CONTROL_DESTROY:
        case VIDEO_CONTROL_ENABLE:
            break;
        case VIDEO_CONTROL_UNLOAD:
        case VIDEO_CONTROL_DISABLE:
            //
            // make the NV10 overlay window disappear by setting the
            // SIZE_OUT register to zero ... to reenable, the old/new
            // overlay classes will need to come in again and hit the methods
            //
            REG_WR32(NV_PVIDEO_SIZE_OUT(0), 0);
            REG_WR32(NV_PVIDEO_SIZE_OUT(1), 0);
            break;
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }
    return (RM_OK);
}

//
// nvHalVideoAlloc
//
RM_STATUS
nvHalVideoAlloc_NV10(VOID *arg)
{
    PVIDEOALLOCARG_000 pVideoAllocArg = (PVIDEOALLOCARG_000)arg;
    PHALHWINFO pHalHwInfo = pVideoAllocArg->pHalHwInfo;
    PHWREG nvAddr;
    RM_STATUS status = RM_OK;

    //
    // Verify interface revision.
    //
    if (pVideoAllocArg->id != VIDEO_ALLOC_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalVideoAlloc bad revision ", pVideoAllocArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    pHalHwInfo = pVideoAllocArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;

    switch (pVideoAllocArg->classNum)
    {
        case NV_VIDEO_FROM_MEMORY:
        {
            PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj;

            pVidFromMemHalObj = (PVIDEOFROMMEMHALOBJECT)pVideoAllocArg->pHalObjInfo;
            pVidFromMemHalObj->KickOffProc = class63InitXfer_NV10;
            break;
        }
        case NV_VIDEO_SCALER:
        {
            PVIDEOSCALERHALOBJECT pVidScalerHalObj;

            pVidScalerHalObj = (PVIDEOSCALERHALOBJECT)pVideoAllocArg->pHalObjInfo;
            pVidScalerHalObj->DeltaDuDx = 0;
            pVidScalerHalObj->DeltaDvDy = 0;
            pVidScalerHalObj->xStart = 0;
            pVidScalerHalObj->yStart = 0;
            break;
        }
        case NV_VIDEO_COLOR_KEY:
        {
            PVIDEOCOLORKEYHALOBJECT pVidColorKeyHalObj;

            pVidColorKeyHalObj = (PVIDEOCOLORKEYHALOBJECT)pVideoAllocArg->pHalObjInfo;
            pVidColorKeyHalObj->ColorFormat = 0;
            pVidColorKeyHalObj->Color = 0;
            pVidColorKeyHalObj->xClipMin = 0;
            pVidColorKeyHalObj->xClipMax = 0;
            pVidColorKeyHalObj->yClipMin = 0;
            pVidColorKeyHalObj->yClipMax = 0;
            break;
        }
        case NV04_VIDEO_OVERLAY:
        case NV10_VIDEO_OVERLAY:
            break;
        default:
            status = RM_ERR_ILLEGAL_OBJECT;
            break;
    }

    return (status);
}

//
// nvHalVideoFree
//
RM_STATUS
nvHalVideoFree_NV10(VOID *arg)
{
    PVIDEOFREEARG_000 pVideoFreeArg = (PVIDEOFREEARG_000)arg;
    PHALHWINFO pHalHwInfo = pVideoFreeArg->pHalHwInfo;

    //
    // Verify interface revision.
    //
    if (pVideoFreeArg->id != VIDEO_FREE_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalVideoFree bad revision ", pVideoFreeArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    switch (pVideoFreeArg->classNum)
    {
        case NV_VIDEO_FROM_MEMORY:
        {
            PHALHWINFO pHalHwInfo;
            PHWREG nvAddr;
            PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj;

            pHalHwInfo = pVideoFreeArg->pHalHwInfo;
            nvAddr = pHalHwInfo->nvBaseAddr;
            pVidFromMemHalObj = (PVIDEOFROMMEMHALOBJECT)pVideoFreeArg->pHalObjInfo;
            //
            // Just to be sure, shut down the overlay.  Need to verify this is YUV.
            //
            if (((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_V8YB8U8YA8)
                || ((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_YB8V8YA8U8))
            {
                // if the desktop is using the scalar, don't shut off
                if (pHalHwInfo->pDacHalInfo->CrtcInfo[0].DisplayType != DISPLAY_TYPE_DUALSURFACE) {
                    // activate a PVIDEO_STOP and disappear the window by
                    // setting SIZE_OUT to 0
                    REG_WR32(NV_PVIDEO_STOP,
                             DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _ACTIVE) |
                             DRF_DEF(_PVIDEO, _STOP, _METHOD, _IMMEDIATELY));
                    
                    REG_WR32(NV_PVIDEO_SIZE_OUT(0), 0);
                    REG_WR32(NV_PVIDEO_SIZE_OUT(1), 0);
                }
                pHalHwInfo->pVideoHalInfo->Enabled = 0;
                pHalHwInfo->pVideoHalInfo->ColorKeyEnabled = 0;
                pHalHwInfo->pVideoHalInfo->ScaleFactor = 0;
            }

            // return the TV to desktop if we are in dual surface mode
            if (pHalHwInfo->pDacHalInfo->CrtcInfo[0].DisplayType == DISPLAY_TYPE_DUALSURFACE)
                class63DualSurfaceDesktop_NV10(pHalHwInfo, DISPLAY_TYPE_DUALSURFACE);
            break;
        }
        case NV_VIDEO_SCALER:
        case NV_VIDEO_COLOR_KEY:
            break;
        case NV04_VIDEO_OVERLAY:
        case NV10_VIDEO_OVERLAY:
        {
            PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj;

            pOverlayHalObj = (PVIDEO_OVERLAY_HAL_OBJECT)pVideoFreeArg->pHalObjInfo;
            videoStopOverlay_NV10(pVideoFreeArg->pHalHwInfo, pOverlayHalObj, 0);
            pVideoFreeArg->pHalHwInfo->pVideoHalInfo->Enabled = 0;
            break;
        }
        default:
            return RM_ERR_ILLEGAL_OBJECT;
    }
    
    return (RM_OK);
}

//
// nvHalVideoMethod
//
// This entry point directs video engine object methods
// to the correct class-dependent handler.
//
RM_STATUS
nvHalVideoMethod_NV10(VOID *arg)
{
    RM_STATUS status = RM_OK;
    PVIDEOMETHODARG_000 pVideoMethodArg = (PVIDEOMETHODARG_000)arg;
    PHALHWINFO pHalHwInfo = pVideoMethodArg->pHalHwInfo;

    //
    // Verify interface revision.
    //
    if (pVideoMethodArg->id != VIDEO_METHOD_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalVideoMethod bad revision ", pVideoMethodArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    switch (pVideoMethodArg->classNum)
    {
        case NV_VIDEO_FROM_MEMORY:
            pVideoMethodArg->mthdStatus = class63Method_NV10(pVideoMethodArg);
            break;
        case NV_VIDEO_SCALER:
            pVideoMethodArg->mthdStatus = class64Method_NV10(pVideoMethodArg);
            break;
        case NV_VIDEO_COLOR_KEY:
            pVideoMethodArg->mthdStatus = class65Method_NV10(pVideoMethodArg);
            break;
        case NV04_VIDEO_OVERLAY:
            pVideoMethodArg->mthdStatus = class047Method_NV10(pVideoMethodArg);
            break;
        case NV10_VIDEO_OVERLAY:
            pVideoMethodArg->mthdStatus = class07aMethod_NV10(pVideoMethodArg);
            break;
        default:
            status = RM_ERR_ILLEGAL_OBJECT;
            break;
    }
    
    return (status);
}

//
// nvHalVideoGetEventStatus
//
// This entry point fetches video buffer status.
//
RM_STATUS
nvHalVideoGetEventStatus_NV10(VOID *arg)
{
    RM_STATUS status = RM_OK;
    PVIDEOGETEVENTSTATUSARG_000 pVideoGetEventStatusArg = (PVIDEOGETEVENTSTATUSARG_000)arg;
    PHALHWINFO pHalHwInfo = pVideoGetEventStatusArg->pHalHwInfo;
    PHWREG nvAddr;

    //
    // Verify interface revision.
    //
    if (pVideoGetEventStatusArg->id != VIDEO_GET_EVENT_STATUS_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalVideoGetEventStatus bad revision ", pVideoGetEventStatusArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    //
    // Make sure video is enabled
    //
    pHalHwInfo = pVideoGetEventStatusArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;
    if (!pHalHwInfo->pVideoHalInfo->Enabled)
	{
        // ignore this interrupt because we do not care
        // what caused it because the enable has not been set
        REG_WR_DRF_NUM(_PVIDEO, _INTR, _BUFFER_0, NV_PVIDEO_INTR_BUFFER_0_RESET);
        REG_WR_DRF_NUM(_PVIDEO, _INTR, _BUFFER_1, NV_PVIDEO_INTR_BUFFER_1_RESET);

#if 0
        //
        // XXX (scottl):
        //
        // I'm going to ignore the NV_VIDEO_FROM_MEMORY class in 
        // this case because it makes things easier.
        //

        //
        // We deviate here between classes.  This is probably unnecessary,
        // but I don't want to mess around.
        //
        if (pVideoGetEventStatusArg->classNum == NV_VIDEO_FROM_MEMORY)
        {
            // Return the current interrupt pending state. We could be here because of a
            // STOP_ACTIVE ... reading NV_PVIDEO_INTR one more time ensures the writes
            // were flushed (seen on IKOS, but in general shouldn't hurt.

            intr0 = REG_RD32(NV_PVIDEO_INTR);
            pVideoGetEventStatusArg->intrStatus =
                (intr0 & (DRF_DEF(_PVIDEO, _INTR, _BUFFER_0, _PENDING) |
                          (DRF_DEF(_PVIDEO, _INTR, _BUFFER_1, _PENDING))));
        }
        else
        {
            // clear a pending stop (destroy method can beat a STOP intr coming in)
            REG_WR_DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _INACTIVE);
        }
#endif

        //
        // If we're here, it means a video's classDestroy has come in before this STOP
        // or buffer completion intr ... this means 2 things:
        //
        //      - we can't use the pOverlayObject state, since it's already been freed
        //      - based on register state only we need to allow a STOP to still reclaim
        //          all buffers, otherwise we may leave the last buffer visible
        //
        if (REG_RD32(NV_PVIDEO_BUFFER) == (DRF_DEF(_PVIDEO, _BUFFER, _0_USE, _NOT_PENDING) |
                                           DRF_DEF(_PVIDEO, _BUFFER, _1_USE, _NOT_PENDING))) {
            // clear a pending stop (destroy method can beat a STOP intr coming in)
            REG_WR_DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _INACTIVE);
        }
        pVideoGetEventStatusArg->intrStatus = 0;
        pVideoGetEventStatusArg->events = 0;
        return RM_OK;
	}

    switch (pVideoGetEventStatusArg->classNum)
    {
        case NV_VIDEO_FROM_MEMORY:
        case NV04_VIDEO_OVERLAY:
        case NV10_VIDEO_OVERLAY:
            status = videoGetEventStatus_NV10(pVideoGetEventStatusArg);
            break;
        default:
            status = RM_ERR_ILLEGAL_OBJECT;
            break;
    }
    
    return (status);
}

//
// nvHalVideoServiceEvent
//
// This entry point handles video engine exceptions.
//
RM_STATUS
nvHalVideoServiceEvent_NV10(VOID *arg)
{
    RM_STATUS status = RM_OK;
    PVIDEOSERVICEEVENTARG_000 pVideoServiceEventArg = (PVIDEOSERVICEEVENTARG_000)arg;
    PHALHWINFO pHalHwInfo = pVideoServiceEventArg->pHalHwInfo;

    //
    // Verify interface revision.
    //
    if (pVideoServiceEventArg->id != VIDEO_SERVICE_EVENT_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalVideoServiceEvent bad revision ", pVideoServiceEventArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    switch (pVideoServiceEventArg->classNum)
    {
        case NV_VIDEO_FROM_MEMORY:
        case NV04_VIDEO_OVERLAY:
        case NV10_VIDEO_OVERLAY:
            status = videoServiceEvent_NV10(pVideoServiceEventArg);
            break;
        default:
            status = RM_ERR_ILLEGAL_OBJECT;
            break;
    }
    
    return (status);
}

//--------------------------------------------------------------------
// NV10_VIDEO_OVERLAY implementation (class07a)
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Utility routines
//--------------------------------------------------------------------
BOOL videoHwOwnsBuffer_NV10(PHALHWINFO pHalHwInfo, U032 buffer_number, PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj)
{
    // Check if stop is active.
    if (pOverlayHalObj->PvideoStopActive)
        return TRUE;

    switch (buffer_number){
        case 0:
          if (pOverlayHalObj->PvideoBufferCopy & DRF_NUM(_PVIDEO, _BUFFER, _0_USE, NV_PVIDEO_BUFFER_0_USE_PENDING))
            return TRUE;
          break;
        case 1:
          if (pOverlayHalObj->PvideoBufferCopy & DRF_NUM(_PVIDEO, _BUFFER, _1_USE, NV_PVIDEO_BUFFER_1_USE_PENDING))
            return TRUE;
          break;
    }
    return FALSE;
}

VOID videoKickOffOverlay_NV10(PHALHWINFO pHalHwInfo, PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj, U032 buffNum)
{
    PHWREG nvAddr;
    U032 data;

    nvAddr = pHalHwInfo->nvBaseAddr;

    // Enable the overlay
    pHalHwInfo->pVideoHalInfo->Enabled = 1;

    // Enable the interrupt for the overlay. 
    data = REG_RD32(NV_PVIDEO_INTR_EN);
    if (buffNum == 0)
        REG_WR32(NV_PVIDEO_INTR_EN, data | DRF_NUM(_PVIDEO, _INTR_EN, _BUFFER_0, NV_PVIDEO_INTR_EN_BUFFER_0_ENABLED));
    else
        REG_WR32(NV_PVIDEO_INTR_EN, data | DRF_NUM(_PVIDEO, _INTR_EN, _BUFFER_1, NV_PVIDEO_INTR_EN_BUFFER_1_ENABLED));

    // BufferBase is an offset value here
    data = (U032)((size_t)pOverlayHalObj->Overlay[buffNum].OverlayCtxDma->BufferBase);
    REG_WR32(NV_PVIDEO_BASE(buffNum), data);
    
    data = pOverlayHalObj->Overlay[buffNum].OverlayCtxDma->BufferSize;
    REG_WR32(NV_PVIDEO_LIMIT(buffNum), data-1);

    if (pOverlayHalObj->PvideoStopActive == 0) {
        REG_WR32(NV_PVIDEO_STOP,
                 DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _INACTIVE) |
                 DRF_DEF(_PVIDEO, _STOP, _METHOD, _IMMEDIATELY));
    }

    // Kick Off the buffer to Hardware.
    REG_WR32(NV_PVIDEO_BUFFER, pOverlayHalObj->PvideoBufferCopy);
}

//
// videoStopOverlay
//
// Handle StopOverlay method for both classes.
//
VOID videoStopOverlay_NV10(PHALHWINFO pHalHwInfo, PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj, U032 data)
{
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;

    //
    // We can't rely on our PvideoBufferCopy to tell us if the
    // hardware is still using a given buffer because we're getting
    // out of sync (somehow).
    // Go to the hardware directly to make sure we reclaim buffer
    // via stop when we need to.
#if 0
    if ((videoHwOwnsBuffer_NV10(pHalHwInfo, 0, pOverlayHalObj)) ||
        (videoHwOwnsBuffer_NV10(pHalHwInfo, 1, pOverlayHalObj))) {
#endif
    if (REG_RD32(NV_PVIDEO_BUFFER))
    {
        pOverlayHalObj->PvideoStopActive = 1;
        if (data) { //NV_PVIDEO_STOP_METHOD_NORMALLY 
            REG_WR32(NV_PVIDEO_STOP,
                     DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _ACTIVE) |
                     DRF_DEF(_PVIDEO, _STOP, _METHOD, _NORMALLY));
        } else {
            REG_WR32(NV_PVIDEO_STOP,
                     DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _ACTIVE) |
                     DRF_DEF(_PVIDEO, _STOP, _METHOD, _IMMEDIATELY));
        }
    }
    else {
        pOverlayHalObj->PvideoStopActive = FALSE;
            REG_WR32(NV_PVIDEO_STOP,
                     DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _INACTIVE) |
                     DRF_DEF(_PVIDEO, _STOP, _METHOD, _IMMEDIATELY));
    }
}

//
// Exception data fetcher
//
// Let's RM know which buffer (if any) needs servicing.
//
static RM_STATUS videoGetEventStatus_NV10(PVIDEOGETEVENTSTATUSARG_000 pVideoGetEventStatusArg)
{
    PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj;
    U032 intr0;
    PHALHWINFO pHalHwInfo;
    PHWREG nvAddr;

    pHalHwInfo = pVideoGetEventStatusArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;

    //
    // Default is that none of the buffers needs servicing.  So tell
    // RM to bypass the nvHalVideoServiceEvent handler.
    //    
    pVideoGetEventStatusArg->events = 0;
    
    pOverlayHalObj = (PVIDEO_OVERLAY_HAL_OBJECT)pVideoGetEventStatusArg->pHalObjInfo;
    pVideoGetEventStatusArg->intrStatus = 0;

    //
    // Find out which buffer(s) are pending.
    //
    intr0 = REG_RD32(NV_PVIDEO_INTR);
    if (intr0 & DRF_NUM(_PVIDEO, _INTR, _BUFFER_0, NV_PVIDEO_INTR_BUFFER_0_PENDING)) {
        REG_WR_DRF_NUM(_PVIDEO, _INTR, _BUFFER_0, NV_PVIDEO_INTR_BUFFER_0_RESET);
        pOverlayHalObj->PvideoBufferCopy &= ~(DRF_NUM(_PVIDEO, _BUFFER, _0_USE, NV_PVIDEO_BUFFER_0_USE_SET));
        pVideoGetEventStatusArg->events |= (1 << 0);
    }

    if (intr0 & DRF_NUM(_PVIDEO, _INTR, _BUFFER_1, NV_PVIDEO_INTR_BUFFER_1_PENDING)) {
        REG_WR_DRF_NUM(_PVIDEO, _INTR, _BUFFER_1, NV_PVIDEO_INTR_BUFFER_1_RESET);
        pOverlayHalObj->PvideoBufferCopy &= ~(DRF_NUM(_PVIDEO, _BUFFER, _1_USE, NV_PVIDEO_BUFFER_1_USE_SET));
        pVideoGetEventStatusArg->events |= (1 << 1);
    }

    return RM_OK;
}

//
// video exception handler
//
static RM_STATUS videoServiceEvent_NV10(PVIDEOSERVICEEVENTARG_000 pVideoServiceEventArg)
{
    PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj;
    PHALHWINFO pHalHwInfo;
    PHWREG nvAddr;

    pHalHwInfo = pVideoServiceEventArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;

    pOverlayHalObj = (PVIDEO_OVERLAY_HAL_OBJECT)pVideoServiceEventArg->pHalObjInfo;

    //
    // We shouldn't be in here if we don't have a valid overlay
    // object, but make sure it's non-null just the same...
    //
    if (pOverlayHalObj && pOverlayHalObj->PvideoStopActive &&
        (pOverlayHalObj->PvideoBufferCopy == (DRF_DEF(_PVIDEO, _BUFFER, _0_USE, _NOT_PENDING) |
                                              DRF_DEF(_PVIDEO, _BUFFER, _1_USE, _NOT_PENDING)))) {
        REG_WR32(NV_PVIDEO_STOP,
                 DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _INACTIVE) |
                 DRF_DEF(_PVIDEO, _STOP, _METHOD, _IMMEDIATELY));
        pOverlayHalObj->PvideoStopActive = 0;
    }

    //
    // On NV10 it doesn't look like we return our current NV_PVDEO_INTR
    // value for the overlay classes.
    //
    if (pVideoServiceEventArg->classNum == NV_VIDEO_FROM_MEMORY)
        pVideoServiceEventArg->intrStatus =
            (REG_RD32(NV_PVIDEO_INTR) &
             (DRF_DEF(_PVIDEO, _INTR, _BUFFER_0, _PENDING) |
              DRF_DEF(_PVIDEO, _INTR, _BUFFER_1, _PENDING)));
    else
        pVideoServiceEventArg->intrStatus = 0;

    return RM_OK;
}

//---------------------------------------------------------------------------
//
//  Adjust the video overlay position if running on TV.
//
//---------------------------------------------------------------------------

//	Because of the way the video scalar syncs with the CRTC, we need to adjust both CRTC timing
//  and make overlay adjustments differently for different devices. NV4 had this problem, NV5
//	was "fixed", and in NV10 the problem is back. NV12 and later device may or may not need
//	special adjustments.
VOID videoAdjustScalarForTV_NV10(PHALHWINFO pHalHwInfo, U032 buffNum)
{
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;
    U032 data;

    if (pHalHwInfo->pDacHalInfo->CrtcInfo[0].DisplayType == DISPLAY_TYPE_TV)
    {
        data = REG_RD32(NV_PVIDEO_POINT_OUT(buffNum));
        REG_WR32(NV_PVIDEO_POINT_OUT(buffNum), data + NV10_VSCALAR_TV_ADJUST);
    }
}

//
// Convert the ds/dX or dt/dY architectural scale factor (in 12.20) into the 
// hardware's step factor (in 1.11).
//
// Basically, 1 / (ds/dX) = hardware step, considering format conversions
//
U016 videoConvertScaleFactor_NV10
(
    S012d020 scale
)
{
    //
    // First convert the 12.20 into 32.0 and divide it out of 1.  To have it
    // result in a 1.11, premultiply the equation by 0x800.
    //
    // Note that we're making an important assumption that scale will always be
    // greater than 1.0!!
    //
    if (scale > 0x100000)
        scale = 0x100000;

    return( (U016)((scale * 0x800) / 0x100000) );
}

//
// videoInit
//
// Generic video engine initialization.
//
RM_STATUS videoInit_NV10
(
    PHALHWINFO pHalHwInfo,
    U032 classNum
)
{
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;

    if (pHalHwInfo->pVideoHalInfo->Enabled)
        return (RM_OK);
        
    REG_WR32(NV_PVIDEO_OFFSET(0), 0);
    REG_WR32(NV_PVIDEO_OFFSET(1), 0);

    // Make sure we're in a known idle state.
    // Wait for any HW owned buffers to complete (IN_USE bits go to 0).
    while (REG_RD32(NV_PVIDEO_BUFFER))
        ;

    // Next, complete an active StopOverlay and reset PGRAPH_INTR
    REG_WR32(NV_PVIDEO_STOP, DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _INACTIVE));
    REG_WR32(NV_PVIDEO_INTR, DRF_DEF(_PVIDEO, _INTR, _BUFFER_0, _RESET) |
                               DRF_DEF(_PVIDEO, _INTR, _BUFFER_1, _RESET));

    REG_WR32(NV_PVIDEO_INTR_EN, DRF_DEF(_PVIDEO, _INTR_EN, _BUFFER_0, _ENABLED) |
                                DRF_DEF(_PVIDEO, _INTR_EN, _BUFFER_1, _ENABLED));

    // set SIZE_IN to infinite values (ImageScan.size isn't correct for NV10 HW)
    REG_WR32(NV_PVIDEO_SIZE_IN(0), 0xFFFFFFFF);
    REG_WR32(NV_PVIDEO_SIZE_IN(1), 0xFFFFFFFF);

    // cleanup any previous values
    REG_WR32(NV_PVIDEO_POINT_IN(0), 0);
    REG_WR32(NV_PVIDEO_POINT_IN(1), 0);

    REG_WR32(NV_PVIDEO_BASE(0), 0);
    REG_WR32(NV_PVIDEO_BASE(1), 0);

    REG_WR32(NV_PVIDEO_LIMIT(0), pHalHwInfo->pFbHalInfo->Limit[0]);
    REG_WR32(NV_PVIDEO_LIMIT(1), pHalHwInfo->pFbHalInfo->Limit[0]);

    REG_WR32(NV_PVIDEO_LUMINANCE(0), DRF_DEF(_PVIDEO, _LUMINANCE, _CONTRAST, _UNITY) |
             DRF_DEF(_PVIDEO, _LUMINANCE, _BRIGHTNESS, _UNITY));
    REG_WR32(NV_PVIDEO_LUMINANCE(1), DRF_DEF(_PVIDEO, _LUMINANCE, _CONTRAST, _UNITY) |
             DRF_DEF(_PVIDEO, _LUMINANCE, _BRIGHTNESS, _UNITY));

    REG_WR32(NV_PVIDEO_CHROMINANCE(0), DRF_DEF(_PVIDEO, _CHROMINANCE, _SAT_COS, _UNITY) |
             DRF_DEF(_PVIDEO, _CHROMINANCE, _SAT_SIN, _UNITY));
    REG_WR32(NV_PVIDEO_CHROMINANCE(1), DRF_DEF(_PVIDEO, _CHROMINANCE, _SAT_COS, _UNITY) |
             DRF_DEF(_PVIDEO, _CHROMINANCE, _SAT_SIN, _UNITY));

    //
    // Initialize default arbitration settings
    //
    pHalHwInfo->pVideoHalInfo->Enabled = 1;
    pHalHwInfo->pVideoHalInfo->ColorKeyEnabled = 1;
    pHalHwInfo->pVideoHalInfo->ScaleFactor = 1;

    return (RM_OK);
}
