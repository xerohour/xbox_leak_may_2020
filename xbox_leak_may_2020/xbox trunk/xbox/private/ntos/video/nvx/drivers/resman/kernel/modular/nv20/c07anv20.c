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
* Module: C07ANV20.C                                                        *
*   This module implements the NV10_VIDEO_OVERLAY object class and          *
*   its corresponding methods.                                              *
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

//
// The following routines are used by the HAL video engine
// manager in kernel/video/vidnv20.c.
//
RM_STATUS class07aMethod_NV20(PVIDEOMETHODARG_000);

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
//  Miscellaneous class support routines.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

//
// class07aMethod
//
// Process all class07a methods.
//
RM_STATUS class07aMethod_NV20(PVIDEOMETHODARG_000 pVideoMethodArg)
{
    PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj;
    PHALHWINFO pHalHwInfo;
    U032 buffNum, offset, data;
    PHWREG nvAddr;

    pOverlayHalObj = (PVIDEO_OVERLAY_HAL_OBJECT)pVideoMethodArg->pHalObjInfo;
    offset = pVideoMethodArg->offset;
    data = pVideoMethodArg->data;
    pHalHwInfo = pVideoMethodArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;

    switch (offset)
    {
        case NV07A_STOP_OVERLAY(0):
        case NV07A_STOP_OVERLAY(1):
            videoStopOverlay_NV20(pHalHwInfo, pOverlayHalObj, data);
            break;
        case NV07A_SET_CONTEXT_DMA_NOTIFIES:
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, 0) ||
                !OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, 1))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
            break;
        case NV07A_SET_CONTEXT_DMA_OVERLAY(0):
        case NV07A_SET_CONTEXT_DMA_OVERLAY(1):
        {
            buffNum = (offset == NV07A_SET_CONTEXT_DMA_OVERLAY(0)) ? 0 : 1;
            if (!OVERLAY_BUFFER_IS_IDLE(pOverlayHalObj, buffNum))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
            break;
        }
        case NV07A_SET_OVERLAY_LUMINANCE(0):
        case NV07A_SET_OVERLAY_LUMINANCE(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_LUMINANCE(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV20(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            REG_WR32(NV_PVIDEO_LUMINANCE(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_CHROMINANCE(0):
        case NV07A_SET_OVERLAY_CHROMINANCE(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_CHROMINANCE(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV20(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            REG_WR32(NV_PVIDEO_CHROMINANCE(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_COLORKEY(0):
        case NV07A_SET_OVERLAY_COLORKEY(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_COLORKEY(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV20(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // Only 1 ColorKey register in NV20 ... 
            REG_WR32(NV_PVIDEO_COLOR_KEY, data);

            break;
        }
        case NV07A_SET_OVERLAY_OFFSET(0):
        case NV07A_SET_OVERLAY_OFFSET(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_OFFSET(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV20(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            REG_WR32(NV_PVIDEO_OFFSET(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_SIZE_IN(0):
        case NV07A_SET_OVERLAY_SIZE_IN(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_SIZE_IN(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV20(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // Check for overflow and limit if necessary
            if ((data & 0xFFFF) > 0x7fe) {
                DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: Overlay Warning! Width Overflow.\n\r");
                data = (data & 0xffff0000) | 0x7fe;
            }
            if ((data >> 16) > 0x7fe) {
                DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: Overlay Warning! Height Overflow.\n\r");
                data = (data & 0xffff) | 0x7fe0000;
            }

            //
            // Check to see that the height is > 0; if we program the hardware with zero (which is against the class def'n),
            // the hardware will most likely protection fault
            //
            if (((data >> DRF_SHIFT(NV07A_SET_OVERLAY_SIZE_IN_HEIGHT)) & DRF_MASK(NV07A_SET_OVERLAY_SIZE_IN_HEIGHT)) == 0)
            {
                DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07ASizeIn Invalid Height!!\r\n");
                return (NV07A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
            }

            //
            // Check to see that the height is > 0; if we program the hardware with zero (which is against the class def'n),
            // the hardware will most likely protection fault
            //
            if (((data >> DRF_SHIFT(NV07A_SET_OVERLAY_SIZE_IN_HEIGHT)) & DRF_MASK(NV07A_SET_OVERLAY_SIZE_IN_HEIGHT)) == 0)
            {
                DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07ASizeIn Invalid Height!!\r\n");
                return (NV07A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
            }

            REG_WR32(NV_PVIDEO_SIZE_IN(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_POINT_IN(0):
        case NV07A_SET_OVERLAY_POINT_IN(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_POINT_IN(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV20(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            REG_WR32(NV_PVIDEO_POINT_IN(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_DU_DX(0):
        case NV07A_SET_OVERLAY_DU_DX(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_DU_DX(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV20(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            REG_WR32(NV_PVIDEO_DS_DX(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_DV_DY(0):
        case NV07A_SET_OVERLAY_DV_DY(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_DV_DY(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV20(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            REG_WR32(NV_PVIDEO_DT_DY(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_POINT_OUT(0):
        case NV07A_SET_OVERLAY_POINT_OUT(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_POINT_OUT(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV20(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            REG_WR32(NV_PVIDEO_POINT_OUT(buffNum), data);
			videoAdjustScalarForTV_NV20(pHalHwInfo, buffNum);

            break;
        }
        case NV07A_SET_OVERLAY_SIZE_OUT(0):
        case NV07A_SET_OVERLAY_SIZE_OUT(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_SIZE_OUT(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV20(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            REG_WR32(NV_PVIDEO_SIZE_OUT(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_FORMAT(0):
        case NV07A_SET_OVERLAY_FORMAT(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_FORMAT(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV20(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            data &= 0x7FFFFFFF;   // AND of the notify bit.
            REG_WR32(NV_PVIDEO_FORMAT(buffNum), data);

            switch (buffNum){
                case 0:
                    pOverlayHalObj->PvideoBufferCopy = DRF_NUM(_PVIDEO, _BUFFER, _0_USE, NV_PVIDEO_BUFFER_0_USE_SET);
                    break;
                case 1:
                    pOverlayHalObj->PvideoBufferCopy = DRF_NUM(_PVIDEO, _BUFFER, _1_USE, NV_PVIDEO_BUFFER_1_USE_SET);
                    break;
            }
                
            // call the kickoff proc
			videoKickOffOverlay_NV20(pHalHwInfo, pOverlayHalObj, buffNum);
            break;
        }
        case NV07A_SET_OVERLAY_POINT_OUT_A:
        {
            REG_WR32(NV_PVIDEO_POINT_OUT(0), data);
            REG_WR32(NV_PVIDEO_POINT_OUT(1), data);
            videoAdjustScalarForTV_NV20(pHalHwInfo, 0);
            videoAdjustScalarForTV_NV20(pHalHwInfo, 1);
            break;
        }
        case NV07A_SET_OVERLAY_LUMINANCE_A:
        {
            REG_WR32(NV_PVIDEO_LUMINANCE(0), data);
            REG_WR32(NV_PVIDEO_LUMINANCE(1), data);
            break;
        }
        case NV07A_SET_OVERLAY_CHROMINANCE_A:
        {
            REG_WR32(NV_PVIDEO_CHROMINANCE(0), data);
            REG_WR32(NV_PVIDEO_CHROMINANCE(1), data);
            break;
        }
        default:
            // HAL doesn't have anything to do for this particular method
            ;
    }
    return NV07A_NOTIFICATION_STATUS_DONE_SUCCESS;
}
