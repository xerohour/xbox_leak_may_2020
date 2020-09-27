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
* Module: VIDNV04.c                                                         *
*   The NV4 specific HAL VIDEO routines reside in this file.                *
*   Class specific routines are contained in the corresponding modular      *
*   directory file.
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

// HAL interface forwards.
RM_STATUS nvHalVideoControl_NV04(VOID *);
RM_STATUS nvHalVideoAlloc_NV04(VOID *);
RM_STATUS nvHalVideoFree_NV04(VOID *);
RM_STATUS nvHalVideoMethod_NV04(VOID *);
RM_STATUS nvHalVideoGetEventStatus_NV04(VOID *);
RM_STATUS nvHalVideoServiceEvent_NV04(VOID *);

// Exports needed by class-dependent files in modular/nv4.
RM_STATUS videoInit_NV04(PHALHWINFO, U032);
U016 videoConvertScaleFactor_NV04(S012d020);

//
// nvHalVideoControl
//
RM_STATUS
nvHalVideoControl_NV04(VOID *arg)
{
    PVIDEOCONTROLARG_000 pVideoControlArg = (PVIDEOCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo = pVideoControlArg->pHalHwInfo;
    PHWINFO pDev;

    //
    // Verify interface revision.
    //
    if (pVideoControlArg->id != VIDEO_CONTROL_000)
    {
        VIDEO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: nvHalVideoControl bad revision: 0x%x\n", pVideoControlArg->id));
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    pDev = (PHWINFO)pHalHwInfo->pDeviceId;
    switch (pVideoControlArg->cmd)
    {
        case VIDEO_CONTROL_UPDATE:
        case VIDEO_CONTROL_LOAD:
            break;
        case VIDEO_CONTROL_INIT:
            pVideoControlArg->pHalHwInfo->pVideoHalInfo->Enabled = 0;
            break;
        case VIDEO_CONTROL_DESTROY:
        case VIDEO_CONTROL_ENABLE:
            //
            // For NV4 overlay, we need to be absolutely certain we start the pipeline
            // outside of active display, otherwise we can hang it during the kickoff.
            // Eventhough called from vblank, spin to guarantee we kickoff in blank.
            //
            if (DAC_REG_RD_DRF(_PCRTC, _INTR_EN_0, _VBLANK, 0))
            {
                while ((REG_RD08(NV_PRMCIO_INP0__COLOR) & 0x8) == 0x8)
                    // while in vertical blank, wait for active display
                    ;
                while ((REG_RD08(NV_PRMCIO_INP0__COLOR) & 0x8) == 0x0)
                    // while in active display, wait for vertical blank
                    ;
            }

            // Now, we should definitely be in vertical blank
            FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _VIDEO, _ON);
            break;
        case VIDEO_CONTROL_UNLOAD:
        case VIDEO_CONTROL_DISABLE:
            FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _VIDEO, _OFF);
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
nvHalVideoAlloc_NV04(VOID *arg)
{
    PVIDEOALLOCARG_000 pVideoAllocArg = (PVIDEOALLOCARG_000)arg;
    PHALHWINFO pHalHwInfo = pVideoAllocArg->pHalHwInfo;
    RM_STATUS status = RM_OK;

    //
    // Verify interface revision.
    //
    if (pVideoAllocArg->id != VIDEO_ALLOC_000)
    {
        VIDEO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: nvHalVideoAlloc bad revision: 0x%x\n", pVideoAllocArg->id));
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    pHalHwInfo = pVideoAllocArg->pHalHwInfo;

    //
    // Connect class-dependent kick off routine.
    //
    switch (pVideoAllocArg->classNum)
    {
        case NV_VIDEO_FROM_MEMORY:
        {
            PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj;

            pVidFromMemHalObj = (PVIDEOFROMMEMHALOBJECT)pVideoAllocArg->pHalObjInfo;
            pVidFromMemHalObj->KickOffProc = class63InitXfer_NV04;
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
        {
            PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj;

            pOverlayHalObj = (PVIDEO_OVERLAY_HAL_OBJECT)pVideoAllocArg->pHalObjInfo;
            pOverlayHalObj->KickOffProc = class047InitXfer_NV04;
            break;
        }
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
nvHalVideoFree_NV04(VOID *arg)
{
    PVIDEOFREEARG_000 pVideoFreeArg = (PVIDEOFREEARG_000)arg;
    PHALHWINFO pHalHwInfo = pVideoFreeArg->pHalHwInfo;

    //
    // Verify interface revision.
    //
    if (pVideoFreeArg->id != VIDEO_FREE_000)
    {
        VIDEO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: nvHalVideoFree bad revision: 0x%x\n", pVideoFreeArg->id));
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    pHalHwInfo = pVideoFreeArg->pHalHwInfo;

    switch (pVideoFreeArg->classNum)
    {
        case NV_VIDEO_FROM_MEMORY:
        {
            PVIDEOFROMMEMHALOBJECT pVidFromMemHalObj;

            pVidFromMemHalObj = (PVIDEOFROMMEMHALOBJECT)pVideoFreeArg->pHalObjInfo;
            //
            // Just to be sure, shut down the overlay.  Need to verify this is YUV.
            //
            if (((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_V8YB8U8YA8)
                || ((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_YB8V8YA8U8))
            {
                if (pHalHwInfo->pDacHalInfo->CrtcInfo[0].DisplayType != DISPLAY_TYPE_DUALSURFACE)   // if the desktop is using the scalar, don't shut off
                    REG_WR32(NV_PVIDEO_OVERLAY, 0);     // quick shutoff
                pHalHwInfo->pVideoHalInfo->Enabled = 0;
                pHalHwInfo->pVideoHalInfo->ColorKeyEnabled = 0;
                pHalHwInfo->pVideoHalInfo->ScaleFactor = 0;
            }

            // return the TV to desktop if we are in dual surface mode
            if (pHalHwInfo->pDacHalInfo->CrtcInfo[0].DisplayType == DISPLAY_TYPE_DUALSURFACE)
                class63DualSurfaceDesktop_NV04(pHalHwInfo, DISPLAY_TYPE_DUALSURFACE);
            break;
        }
        case NV_VIDEO_SCALER:
        case NV_VIDEO_COLOR_KEY:
            break;
        case NV04_VIDEO_OVERLAY:
            if (pHalHwInfo->pDacHalInfo->CrtcInfo[0].DisplayType != DISPLAY_TYPE_DUALSURFACE)
                REG_WR32(NV_PVIDEO_OVERLAY, 0);     // quick shutoff
            pHalHwInfo->pVideoHalInfo->Enabled = 0;
            pHalHwInfo->pVideoHalInfo->ColorKeyEnabled = 0;
            pHalHwInfo->pVideoHalInfo->ScaleFactor = 0;
            break;
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
nvHalVideoMethod_NV04(VOID *arg)
{
    RM_STATUS status = RM_OK;
    PVIDEOMETHODARG_000 pVideoMethodArg = (PVIDEOMETHODARG_000)arg;
    PHALHWINFO pHalHwInfo = pVideoMethodArg->pHalHwInfo;

    //
    // Verify interface revision.
    //
    if (pVideoMethodArg->id != VIDEO_METHOD_000)
    {
        VIDEO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: nvHalVideoMethod bad revision: 0x%x\n", pVideoMethodArg->id));
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    switch (pVideoMethodArg->classNum)
    {
        case NV_VIDEO_FROM_MEMORY:
            pVideoMethodArg->mthdStatus = class63Method_NV04(pVideoMethodArg);
            break;
        case NV_VIDEO_SCALER:
            pVideoMethodArg->mthdStatus = class64Method_NV04(pVideoMethodArg);
            break;
        case NV_VIDEO_COLOR_KEY:
            pVideoMethodArg->mthdStatus = class65Method_NV04(pVideoMethodArg);
            break;
        case NV04_VIDEO_OVERLAY:
            pVideoMethodArg->mthdStatus = class047Method_NV04(pVideoMethodArg);
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
nvHalVideoGetEventStatus_NV04(VOID *arg)
{
    RM_STATUS status = RM_OK;
    PVIDEOGETEVENTSTATUSARG_000 pVideoGetEventStatusArg = (PVIDEOGETEVENTSTATUSARG_000)arg;
    PHALHWINFO pHalHwInfo = pVideoGetEventStatusArg->pHalHwInfo;

    //
    // Verify interface revision.
    //
    if (pVideoGetEventStatusArg->id != VIDEO_GET_EVENT_STATUS_000)
    {
        VIDEO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: nvHalVideoGetEventStatus bad revision: 0x%x\n", pVideoGetEventStatusArg->id));
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    //
    // Make sure video is enabled
    //
    if (!pHalHwInfo->pVideoHalInfo->Enabled)
    {
        //
        // Ignore this interrupt because we do not care
        // what caused it because the enable has not been set
        //
        REG_WR32(NV_PVIDEO_INTR_0, NV_PVIDEO_INTR_0_NOTIFY_RESET);
        pVideoGetEventStatusArg->intrStatus = 0;
        return RM_OK;
    }

    switch (pVideoGetEventStatusArg->classNum)
    {
        case NV04_VIDEO_OVERLAY:
            status = class047GetEventStatus_NV04(pVideoGetEventStatusArg);
            break;
        case NV_VIDEO_FROM_MEMORY:
            status = class63GetEventStatus_NV04(pVideoGetEventStatusArg);
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
nvHalVideoServiceEvent_NV04(VOID *arg)
{
    RM_STATUS status = RM_OK;
    PVIDEOSERVICEEVENTARG_000 pVideoServiceEventArg = (PVIDEOSERVICEEVENTARG_000)arg;
    PHALHWINFO pHalHwInfo = pVideoServiceEventArg->pHalHwInfo;

    //
    // Verify interface revision.
    //
    if (pVideoServiceEventArg->id != VIDEO_SERVICE_EVENT_000)
    {
        VIDEO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: nvHalVideoServiceEvent bad revision: 0x%x\n", pVideoServiceEventArg->id));
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    switch (pVideoServiceEventArg->classNum)
    {
        case NV04_VIDEO_OVERLAY:
            status = class047ServiceEvent_NV04(pVideoServiceEventArg);
            break;
        case NV_VIDEO_FROM_MEMORY:
            status = class63ServiceEvent_NV04(pVideoServiceEventArg);
            break;
        default:
            status = RM_ERR_ILLEGAL_OBJECT;
            break;
    }
    
    return (status);
}

//
// Convert the ds/dX or dt/dY architectural scale factor (in 12.20) into the 
// hardware's step factor (in 1.11).
//
// Basically, 1 / (ds/dX) = hardware step, considering format conversions
//
U016 videoConvertScaleFactor_NV04
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
RM_STATUS videoInit_NV04
(
    PHALHWINFO pHalHwInfo,
    U032 classNum
)
{
    // Force the hardware to be reset if this routine is called at all.
    // We need to revisit this fix for our modeset problems to see if there is
    // a better solution (JSUN).
    // 

    if (classNum == NV_VIDEO_FROM_MEMORY)
        if (pHalHwInfo->pVideoHalInfo->Enabled)
            return (RM_OK);
        
    REG_WR32(NV_PVIDEO_CONTROL_Y, DRF_DEF(_PVIDEO, _CONTROL_Y, _BLUR, _ON));
    REG_WR32(NV_PVIDEO_CONTROL_X, DRF_DEF(_PVIDEO, _CONTROL_X, _SMOOTHING, _ON)
                                | DRF_DEF(_PVIDEO, _CONTROL_X, _SHARPENING, _ON));

    REG_WR32(NV_PVIDEO_BUFF0_OFFSET, 0);
    REG_WR32(NV_PVIDEO_BUFF1_OFFSET, 0);

    REG_WR32(NV_PVIDEO_OE_STATE, 0);
    REG_WR_DRF_DEF(_PVIDEO, _OE_STATE, _CURRENT_BUFFER, _1);
    REG_WR32(NV_PVIDEO_SU_STATE, DRF_NUM(_PVIDEO, _SU_STATE, _BUFF0_IN_USE, 1)
                               | DRF_NUM(_PVIDEO, _SU_STATE, _BUFF1_IN_USE, 1));
    REG_WR32(NV_PVIDEO_RM_STATE, 0);

    REG_WR_DRF_DEF(_PVIDEO, _INTR_EN_0, _NOTIFY, _ENABLED);

    //
    // Initialize default conversion values
    // These value are per the NV3/4 manual as the default
    // offsets for YUV conversion
    //
    REG_WR32(NV_PVIDEO_CSC_ADJUST, 0x10000);
    REG_WR32(NV_PVIDEO_RED_CSC, 105);
    REG_WR32(NV_PVIDEO_GREEN_CSC, 62);
    REG_WR32(NV_PVIDEO_BLUE_CSC, 137);

    //
    // Initialize default arbitration settings
    //
    pHalHwInfo->pVideoHalInfo->Enabled = 1;
    pHalHwInfo->pVideoHalInfo->ColorKeyEnabled = 1;
    pHalHwInfo->pVideoHalInfo->ScaleFactor = 1;

    return (RM_OK);
}
