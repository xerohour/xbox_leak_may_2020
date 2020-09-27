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
* Module: VIDSCALR.C                                                        *
*   This module implements the NV_VIDEO_SCALER object class and its         *
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

//
// The following routines are used by the HAL video engine
// manager in kernel/video/vidnv20.c.
//
RM_STATUS class64Method_NV20(PVIDEOMETHODARG_000);

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
// class64Method
//
// This routine performs any chip-specific handling for all class64
// methods.
//
RM_STATUS class64Method_NV20(PVIDEOMETHODARG_000 pVideoMethodArg)
{
    PVIDEOSCALERHALOBJECT pVidScalerHalObj;
    PHALHWINFO pHalHwInfo;
    U032 offset, data;
    PHWREG nvAddr;

    pVidScalerHalObj = (PVIDEOSCALERHALOBJECT)pVideoMethodArg->pHalObjInfo;
    offset = pVideoMethodArg->offset;
    data = pVideoMethodArg->data;
    pHalHwInfo = pVideoMethodArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;

    switch (offset)
    {
        case NVFF7_SET_DELTA_DU_DX(0):
            pVidScalerHalObj->DeltaDuDx = data;
            pHalHwInfo->pVideoHalInfo->VideoScaleX = videoConvertScaleFactor_NV20(data);
            break;
        case NVFF7_SET_DELTA_DV_DY(0):
            pVidScalerHalObj->DeltaDvDy = data;
            pHalHwInfo->pVideoHalInfo->VideoScaleY = videoConvertScaleFactor_NV20(data);
            break;
        case NVFF7_SET_POINT(0):
            pVidScalerHalObj->xStart = (data & 0xffff);
            pVidScalerHalObj->yStart = (data >> 16);
            pHalHwInfo->pVideoHalInfo->VideoStart =
                DRF_NUM(_PVIDEO, _POINT_OUT, _X, pVidScalerHalObj->xStart) |
                DRF_NUM(_PVIDEO, _POINT_OUT, _Y, pVidScalerHalObj->yStart);
            break;
        default:
            // HAL doesn't have anything to do for this particular method
            ;
    }

    return NVFF7_NOTIFICATION_STATUS_DONE_SUCCESS;
}


