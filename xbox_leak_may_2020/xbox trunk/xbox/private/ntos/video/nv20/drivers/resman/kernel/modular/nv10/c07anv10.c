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
* Module: C07ANV10.C                                                        *
*   This module implements the NV10_VIDEO_OVERLAY object class and          *
*   its corresponding methods.                                              *
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
#include "nvhw.h"
#include "nvhalcpp.h"

//
// The following routines are used by the HAL video engine
// manager in kernel/video/vidnv10.c.
//
RM_STATUS class07aMethod_NV10(PVIDEOMETHODARG_000);
static RM_STATUS class07aUpdatePointIn_NV10(PHALHWINFO, PVIDEO_OVERLAY_HAL_OBJECT, U032);
static RM_STATUS class07aUpdateSizeOut_NV10(PHALHWINFO, PVIDEO_OVERLAY_HAL_OBJECT, U032);
static RM_STATUS class07aUpdatePointOut_NV10(PHALHWINFO, PVIDEO_OVERLAY_HAL_OBJECT, U032);

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

//
// Class instantiation/destruction is handled by video/videoobj.c and
// video/nv10/vidnv10.c.
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
RM_STATUS class07aMethod_NV10(PVIDEOMETHODARG_000 pVideoMethodArg)
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
        case NV07A_STOP_OVERLAY(0):
        case NV07A_STOP_OVERLAY(1):
            videoStopOverlay_NV10(pHalHwInfo, pOverlayHalObj, data);
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
            if (videoHwOwnsBuffer_NV10(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            REG_WR32(NV_PVIDEO_LUMINANCE(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_CHROMINANCE(0):
        case NV07A_SET_OVERLAY_CHROMINANCE(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_CHROMINANCE(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV10(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            REG_WR32(NV_PVIDEO_CHROMINANCE(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_COLORKEY(0):
        case NV07A_SET_OVERLAY_COLORKEY(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_COLORKEY(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV10(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // Only 1 ColorKey register in NV10 ... 
            REG_WR32(NV_PVIDEO_COLOR_KEY, data);

            break;
        }
        case NV07A_SET_OVERLAY_OFFSET(0):
        case NV07A_SET_OVERLAY_OFFSET(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_OFFSET(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV10(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            REG_WR32(NV_PVIDEO_OFFSET(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_SIZE_IN(0):
        case NV07A_SET_OVERLAY_SIZE_IN(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_SIZE_IN(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV10(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // Check for overflow and limit if necessary
            if ((data & 0xFFFF) > 0x7fe) {
                VIDEOCLASS_PRINTF((DBG_LEVEL_WARNINGS, "NVRM: Overlay Warning! Width Overflow.\n\r"));
                data = (data & 0xffff0000) | 0x7fe;
            }
            if ((data >> 16) > 0x7fe) {
                VIDEOCLASS_PRINTF((DBG_LEVEL_WARNINGS, "NVRM: Overlay Warning! Height Overflow.\n\r"));
                data = (data & 0xffff) | 0x7fe0000;
            }

            //
            // Check to see that the height is > 0; if we program the hardware with zero (which is against the class def'n),
            // the hardware will most likely protection fault
            //
            if (((data >> DRF_SHIFT(NV07A_SET_OVERLAY_SIZE_IN_HEIGHT)) & DRF_MASK(NV07A_SET_OVERLAY_SIZE_IN_HEIGHT)) == 0)
            {
                VIDEOCLASS_PRINTF((DBG_LEVEL_INFO, "NVRM: class07ASizeIn Invalid Height!!\r\n"));
                return (NV07A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
            }

            //
            // Check to see that the height is > 0; if we program the hardware with zero (which is against the class def'n),
            // the hardware will most likely protection fault
            //
            if (((data >> DRF_SHIFT(NV07A_SET_OVERLAY_SIZE_IN_HEIGHT)) & DRF_MASK(NV07A_SET_OVERLAY_SIZE_IN_HEIGHT)) == 0)
            {
                VIDEOCLASS_PRINTF((DBG_LEVEL_INFO, "NVRM: class07ASizeIn Invalid Height!!\r\n"));
                return (NV07A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
            }

            REG_WR32(NV_PVIDEO_SIZE_IN(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_POINT_IN(0):
        case NV07A_SET_OVERLAY_POINT_IN(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_POINT_IN(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV10(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // save a copy of the POINT_IN values
            pOverlayHalObj->Overlay[buffNum].PointIn_s = 
                (U016)(DRF_VAL(07A, _SET_OVERLAY_POINT_IN, _S, data));
            pOverlayHalObj->Overlay[buffNum].PointIn_t =
                (U016)(DRF_VAL(07A, _SET_OVERLAY_POINT_IN, _T, data));

            class07aUpdatePointIn_NV10(pHalHwInfo, pOverlayHalObj, buffNum);
            break;
        }
        case NV07A_SET_OVERLAY_DU_DX(0):
        case NV07A_SET_OVERLAY_DU_DX(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_DU_DX(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV10(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            pOverlayHalObj->Overlay[buffNum].DsDx = data;

            REG_WR32(NV_PVIDEO_DS_DX(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_DV_DY(0):
        case NV07A_SET_OVERLAY_DV_DY(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_DV_DY(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV10(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            pOverlayHalObj->Overlay[buffNum].DtDy = data;

            REG_WR32(NV_PVIDEO_DT_DY(buffNum), data);

            break;
        }
        case NV07A_SET_OVERLAY_POINT_OUT(0):
        case NV07A_SET_OVERLAY_POINT_OUT(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_POINT_OUT(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV10(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // save a copy of the POINT_OUT values
            pOverlayHalObj->Overlay[buffNum].PointOut_x = 
                (U016)(DRF_VAL(07A, _SET_OVERLAY_POINT_OUT, _X, data));
            pOverlayHalObj->Overlay[buffNum].PointOut_y = 
                (U016)(DRF_VAL(07A, _SET_OVERLAY_POINT_OUT, _Y, data));

            class07aUpdatePointOut_NV10(pHalHwInfo, pOverlayHalObj, buffNum);
			videoAdjustScalarForTV_NV10(pHalHwInfo, buffNum);

            break;
        }
        case NV07A_SET_OVERLAY_SIZE_OUT(0):
        case NV07A_SET_OVERLAY_SIZE_OUT(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_SIZE_OUT(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV10(pHalHwInfo, buffNum, pOverlayHalObj))
                return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // save a copy of the SIZE_OUT values
            pOverlayHalObj->Overlay[buffNum].SizeOut_width = 
                (U016)(DRF_VAL(07A, _SET_OVERLAY_SIZE_OUT, _WIDTH, data));
            pOverlayHalObj->Overlay[buffNum].SizeOut_height =
                (U016)(DRF_VAL(07A, _SET_OVERLAY_SIZE_OUT, _HEIGHT, data));

            class07aUpdateSizeOut_NV10(pHalHwInfo, pOverlayHalObj, buffNum);

            break;
        }
        case NV07A_SET_OVERLAY_FORMAT(0):
        case NV07A_SET_OVERLAY_FORMAT(1):
        {
            buffNum = (offset == NV07A_SET_OVERLAY_FORMAT(0)) ? 0 : 1;
            if (videoHwOwnsBuffer_NV10(pHalHwInfo, buffNum, pOverlayHalObj))
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
			videoKickOffOverlay_NV10(pHalHwInfo, pOverlayHalObj, buffNum);
            break;
        }
        case NV07A_SET_OVERLAY_POINT_OUT_A:
        {
            // save a copy of the POINT_OUT values
            pOverlayHalObj->Overlay[0].PointOut_x = 
                (U016)(DRF_VAL(07A, _SET_OVERLAY_POINT_OUT, _X, data));
            pOverlayHalObj->Overlay[0].PointOut_y = 
                (U016)(DRF_VAL(07A, _SET_OVERLAY_POINT_OUT, _Y, data));
            pOverlayHalObj->Overlay[1].PointOut_x = 
                (U016)(DRF_VAL(07A, _SET_OVERLAY_POINT_OUT, _X, data));
            pOverlayHalObj->Overlay[1].PointOut_y = 
                (U016)(DRF_VAL(07A, _SET_OVERLAY_POINT_OUT, _Y, data));


            class07aUpdatePointOut_NV10(pHalHwInfo, pOverlayHalObj, 0);
            class07aUpdatePointOut_NV10(pHalHwInfo, pOverlayHalObj, 1);

            videoAdjustScalarForTV_NV10(pHalHwInfo, 0);
            videoAdjustScalarForTV_NV10(pHalHwInfo, 1);

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

//
// Apply possible panning delta to POINT_IN method data.
//
static RM_STATUS
class07aUpdatePointIn_NV10(PHALHWINFO pHalHwInfo, PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj, U032 buffNum)
{
    PVIDEOHALINFO pVideoHalInfo;
    U016 pointoutX, pointoutY;
    U032 pointinS, pointinT;
    U032 dsdx, dtdy;

    pVideoHalInfo = pHalHwInfo->pVideoHalInfo;

    // apply any panning deltas to the SIZE_OUT data
    pointoutX = (S016)pOverlayHalObj->Overlay[buffNum].PointOut_x +
        (S016)(pVideoHalInfo->PanOffsetDeltaX[pVideoHalInfo->Head] * -1);
    pointoutY = (S016)pOverlayHalObj->Overlay[buffNum].PointOut_y +
        (S016)(pVideoHalInfo->PanOffsetDeltaY[pVideoHalInfo->Head] * -1);

    pointinS = (S032)pOverlayHalObj->Overlay[buffNum].PointIn_s;
    // PointInT is 12.4 in the class def, but 12.3 in the hw
    pointinT = (S032)pOverlayHalObj->Overlay[buffNum].PointIn_t >> 1;

    // handle negative X
    if (pointoutX & 0x8000)
    {
        dsdx = pOverlayHalObj->Overlay[buffNum].DsDx;
        pointinS +=(((((S016)pointoutX * -1) * dsdx) + (1 << 15)) >> (20 - 4));
    }

    // handle negative Y
    if (pointoutY & 0x8000)
    {
        dtdy = pOverlayHalObj->Overlay[buffNum].DtDy;
        pointinT += (((((S016)pointoutY * -1) * dtdy) + (1 << 15)) >> (20 - 3));
    }

    // finally, write out new SIZE_OUT value
    REG_WR32(NV_PVIDEO_POINT_IN(buffNum),
             DRF_NUM(_PVIDEO, _POINT_IN, _S, pointinS) |
             DRF_NUM(_PVIDEO, _POINT_IN, _T, pointinT));

    return RM_OK;
}

//
// Apply possible panning delta to SIZE_OUT method data.
//
static RM_STATUS
class07aUpdateSizeOut_NV10(PHALHWINFO pHalHwInfo, PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj, U032 buffNum)
{
    PVIDEOHALINFO pVideoHalInfo;
    U016 pointoutX, pointoutY;
    U016 sizeoutW, sizeoutH;

    pVideoHalInfo = pHalHwInfo->pVideoHalInfo;

    // apply any panning deltas to the SIZE_OUT data
    pointoutX = (S016)pOverlayHalObj->Overlay[buffNum].PointOut_x +
        (S016)(pVideoHalInfo->PanOffsetDeltaX[pVideoHalInfo->Head] * -1);
    pointoutY = (S016)pOverlayHalObj->Overlay[buffNum].PointOut_y +
        (S016)(pVideoHalInfo->PanOffsetDeltaY[pVideoHalInfo->Head] * -1);

    sizeoutW = (S016)pOverlayHalObj->Overlay[buffNum].SizeOut_width;
    sizeoutH = (S016)pOverlayHalObj->Overlay[buffNum].SizeOut_height;

    // handle negative X
    if (pointoutX & 0x8000)
    {
        // redo sizeout_width
        sizeoutW += (S016)pointoutX;
        if (sizeoutW & 0x8000)
            sizeoutW = 0;
    }

    // handle negative Y
    if (pointoutY & 0x8000)
    {
        sizeoutH += (S016)pointoutY;
        if (sizeoutH & 0x8000)
            sizeoutH = 0;
    }

    // finally, write out new SIZE_OUT value
    REG_WR32(NV_PVIDEO_SIZE_OUT(buffNum),
             DRF_NUM(_PVIDEO, _SIZE_OUT, _HEIGHT, sizeoutH)|
             DRF_NUM(_PVIDEO, _SIZE_OUT, _WIDTH, sizeoutW));

    return RM_OK;
}

//
// The point out method require updates to other state if we're in panning
// mode and move the overlay window in the negative x or y direction.
//
static RM_STATUS
class07aUpdatePointOut_NV10(PHALHWINFO pHalHwInfo, PVIDEO_OVERLAY_HAL_OBJECT pOverlayHalObj, U032 buffNum)
{
    PVIDEOHALINFO pVideoHalInfo;
    U016 pointoutX, pointoutY;
    U032 pointinS, pointinT;
    U016 sizeoutW, sizeoutH;
    U032 dsdx, dtdy;

    pVideoHalInfo = pHalHwInfo->pVideoHalInfo;

    // apply any panning deltas to the POINT_OUT data
    pointoutX = (S016)pOverlayHalObj->Overlay[buffNum].PointOut_x +
        (S016)(pVideoHalInfo->PanOffsetDeltaX[pVideoHalInfo->Head] * -1);
    pointoutY = (S016)pOverlayHalObj->Overlay[buffNum].PointOut_y +
        (S016)(pVideoHalInfo->PanOffsetDeltaY[pVideoHalInfo->Head] * -1);

    pointinS = (S016)pOverlayHalObj->Overlay[buffNum].PointIn_s;
    // PointInT is 12.4 in the class def, but 12.3 in the hw
    pointinT = (S032)pOverlayHalObj->Overlay[buffNum].PointIn_t >> 1;
    sizeoutW = (S016)pOverlayHalObj->Overlay[buffNum].SizeOut_width;
    sizeoutH = (S016)pOverlayHalObj->Overlay[buffNum].SizeOut_height;

    // handle negative X
    if (pointoutX & 0x8000)
    {
        // redo POINT_IN_S
        dsdx = pOverlayHalObj->Overlay[buffNum].DsDx;
        pointinS +=(((((S016)pointoutX * -1) * dsdx) + (1 << 15)) >> (20 - 4));
        REG_WR32(NV_PVIDEO_POINT_IN(buffNum),
                 DRF_NUM(_PVIDEO, _POINT_IN, _S, pointinS) |
                 DRF_NUM(_PVIDEO, _POINT_IN, _T, pointinT));

        // redo SIZE_OUT_WIDTH
        sizeoutW = (S016)pOverlayHalObj->Overlay[buffNum].SizeOut_width + (S016)pointoutX;
        if (sizeoutW & 0x8000)
            sizeoutW = 0;
        REG_WR32(NV_PVIDEO_SIZE_OUT(buffNum),
                 DRF_NUM(_PVIDEO, _SIZE_OUT, _HEIGHT, sizeoutH)|
                 DRF_NUM(_PVIDEO, _SIZE_OUT, _WIDTH, sizeoutW));

        // now clip negative point out 0
        pointoutX = 0;
    }

    // handle negative Y
    if (pointoutY & 0x8000)
    {
        // redo POINT_IN_T
        dtdy = pOverlayHalObj->Overlay[buffNum].DtDy;
        pointinT += (((((S016)pointoutY * -1) * dtdy) + (1 << 15)) >> (20 - 3));
        REG_WR32(NV_PVIDEO_POINT_IN(buffNum),
                 DRF_NUM(_PVIDEO, _POINT_IN, _S, pointinS) |
                 DRF_NUM(_PVIDEO, _POINT_IN, _T, pointinT));

        // redo SIZE_OUT_HEIGHT
        sizeoutH = (S016)pOverlayHalObj->Overlay[buffNum].SizeOut_height + (S016)pointoutY;
        if (sizeoutH & 0x8000)
            sizeoutH = 0;
        REG_WR32(NV_PVIDEO_SIZE_OUT(buffNum),
                 DRF_NUM(_PVIDEO, _SIZE_OUT, _HEIGHT, sizeoutH)|
                 DRF_NUM(_PVIDEO, _SIZE_OUT, _WIDTH, sizeoutW));

        // now clip negative point out to 0
        pointoutY = 0;
    }

    // finally, write out new POINT_OUT value
    REG_WR32(NV_PVIDEO_POINT_OUT(buffNum),
             DRF_NUM(_PVIDEO, _POINT_OUT, _X, pointoutX) |
             DRF_NUM(_PVIDEO, _POINT_OUT, _Y, pointoutY));

    return RM_OK;
}
