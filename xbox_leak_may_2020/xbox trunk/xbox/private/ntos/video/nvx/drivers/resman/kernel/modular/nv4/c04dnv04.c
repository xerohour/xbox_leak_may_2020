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

/****************************************************************************\
*                                                                           *
* Module: C04DNV04.C                                                        *
*   This module implements the chip-dependent portion of the                *
*   NV_EXTERNAL_VIDEO_DECODER object class and its corresponding methods.   *
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
// The following routines are used by the HAL mediaport engine
// manager in kernel/mp/nv4/mpnv04.c.
//
RM_STATUS class04DMethod_NV04(PMPMETHODARG_000);
RM_STATUS class04DGetEventStatus_NV04(PMPGETEVENTSTATUSARG_000);
RM_STATUS class04DServiceEvent_NV04(PMPSERVICEEVENTARG_000);

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

//
// Class instantiation/destruction is handled by mp/mpobj.c and
// mp/nv4/mpnv04.c.
//

//---------------------------------------------------------------------------
//
//  Miscellaneous class support routines.
//
//---------------------------------------------------------------------------
    
static RM_STATUS class04DFindNearestScaleFactor(
    U032 dwRealDivisor,
    U032 *dwSpecialIndex
)
{
    U032 dwIndex=0;
    U032 X_ScaleFactors[] = { 1,2,3,4,6,8,12,16,24, 0xFFFF };

    //
    // Converts dwRealDivisor into an index which will point to nearest best quality divisor,
    //  will always be greater than RealDivisor if not equal
    //
    *dwSpecialIndex = 0xFFFF;
    
    while (X_ScaleFactors[dwIndex] != 0xFFFF)
    {
        if (dwRealDivisor >= X_ScaleFactors[dwIndex])
                *dwSpecialIndex = dwIndex;
        dwIndex++;
    }   

    // this should never occur
    if (*dwSpecialIndex == 0xFFFF)
        {
        *dwSpecialIndex = 1; // could not find index so just do a verbatim copy
        return (RM_ERROR);
        }

    return (RM_OK);
}
    
static RM_STATUS class04DCalculateScaleIncrement(
    U032 WidthIn,
    U032 HeightIn,
    U032 WidthOut,
    U032 HeightOut,
    U032 *ScaleFactor
)
{
    U032        FittedX, FittedY;
    U032        NearestScaleFactor;
    U032        X_ScaleFactors[]  = { 1,2,3,4,6,8,12,16,24, 0xFFFF };
    U032        X_n0[]            = { 0<<5,1<<5,2<<5,2<<5,3<<5,3<<5,4<<5,4<<5,5<<5, 0xFFFF};
    U032        X_n1[]            = { 0<<8,1<<8,1<<8,2<<8,2<<8,3<<8,3<<8,4<<8,4<<8, 0xFFFF};
    RM_STATUS   status;

    if (WidthOut == 0)
        WidthOut = WidthIn; // if this is too big, should be caught on length boundary

    FittedX = WidthIn / WidthOut;

    if (FittedX > 24)
        FittedX = 24; // max out scale 

    status = class04DFindNearestScaleFactor(FittedX, &NearestScaleFactor);
    if (status)
        return (status);
        
    FittedX = X_ScaleFactors[ NearestScaleFactor ] | X_n0[ NearestScaleFactor ] 
                                | X_n1[ NearestScaleFactor ] ;

    if (HeightOut == 0)
        HeightOut = HeightIn;

    FittedY = ((HeightOut -1 ) << 10) / (HeightIn - 1) ;
                                        
    if (FittedY > 0x400)
        FittedY = 0x400; // check these values (2^11 - 1), bloody small picture!!               

    *ScaleFactor = (FittedY << 16) | FittedX;

    return (RM_OK);
}

        
static RM_STATUS class04DStartVbi
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECODERHALOBJECT pDecoderHalObj,
    U032 Buffer
)
{
    PMPHALINFO_NV04 pMpHalPvtInfo;
    V032 stateSU, stateME;

    pMpHalPvtInfo = (PMPHALINFO_NV04)pHalHwInfo->pMpHalPvtInfo;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DStartVbi\r\n");

    //
    // For now, we're going to assume the given buffer maps directly
    // to the hardware buffer.  We can add virtualization later.
    //

    //
    // Spend some time to validate all data
    //

    //
    // Make sure the engine is configured correctly (in case the old
    // class66 overwrites)
    //
    // only change the bus mode if there isn't a decompressor object
    // using the MPC
    //

    if ( pMpHalPvtInfo->CurrentDecompressor == 0 )
        FLD_WR_DRF_DEF(_PME, _CONFIG_0, _BUS_MODE, _CCIR656);

    FLD_WR_DRF_DEF(_PME, _CONFIG_0, _VBI_MODE, _2);
    FLD_WR_DRF_DEF(_PME, _CONFIG_0, _IMAGE, _ENABLED);  // also need to enable image (from Joe Yeun)
    FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _VBI_NOTIFY, _ENABLED);
    
    //
    // The hardware can only support vbi dma transfers to framebuffer
    // addresses.  If this transfer is not completely contiguous,
    // we need to break the transfer up.
    //
    if (pDecoderHalObj->SetVbi[Buffer].CtxDma->AddressSpace != ADDR_FBMEM)
        return (RM_ERROR);
    
    //
    // Load up the buffer location and sizes
    //
    if (Buffer == 0)
    {
        REG_WR32(NV_PME_VBI_BUFF0_START, pDecoderHalObj->SetVbi[0].Offset
                                       + (pDecoderHalObj->SetVbi[0].CtxDma->PteArray[0] & 0xFFFFF000)
                                       + pDecoderHalObj->SetVbi[0].CtxDma->PteAdjust);

        REG_WR32(NV_PME_VBI_BUFF0_PITCH, pDecoderHalObj->SetVbi[0].Pitch);
        REG_WR32(NV_PME_VBI_BUFF0_LENGTH, pDecoderHalObj->SetVbi[0].Pitch * pDecoderHalObj->SetVbi[0].Height);
    } 
    else
    {
        REG_WR32(NV_PME_VBI_BUFF1_START, pDecoderHalObj->SetVbi[1].Offset
                                       + (pDecoderHalObj->SetVbi[1].CtxDma->PteArray[0] & 0xFFFFF000)
                                       + pDecoderHalObj->SetVbi[1].CtxDma->PteAdjust);

        REG_WR32(NV_PME_VBI_BUFF1_PITCH, pDecoderHalObj->SetVbi[1].Pitch);
        REG_WR32(NV_PME_VBI_BUFF1_LENGTH, pDecoderHalObj->SetVbi[1].Pitch * pDecoderHalObj->SetVbi[1].Height);
    } 
    
    //
    // Program the global VBI settings
    //
    FLD_WR_DRF_NUM(_PME, _VBI, _START_LINE, pDecoderHalObj->SetVbi[1].FirstLine);
    FLD_WR_DRF_NUM(_PME, _VBI, _NUM_LINES, pDecoderHalObj->SetVbi[1].Height);
    
    //
    // Start the transfer
    //
    stateSU = REG_RD32(NV_PME_VBI_SU_STATE);
    stateME = REG_RD32(NV_PME_VBI_ME_STATE);
    //if (Buffer != (stateME >> 24))
    //    REG_WR32(NV_PME_VBI_ME_STATE, stateME ^ 0x01000000);

    if (Buffer == 0)
    {
        //
        // Choose the capture field (progressive == even)
        //
        if ((pDecoderHalObj->SetVbi[0].Field == NV04D_SET_VBI_FORMAT_FIELD_PROGRESSIVE)
         || (pDecoderHalObj->SetVbi[0].Field == NV04D_SET_VBI_FORMAT_FIELD_EVEN_FIELD))
            stateSU |= 1 << SF_SHIFT(NV_PME_VBI_SU_STATE_BUFF0_FIELD);
        else            
            stateSU &= ~(1 << SF_SHIFT(NV_PME_VBI_SU_STATE_BUFF0_FIELD));
    
        stateSU ^= 1 << SF_SHIFT(NV_PME_VBI_SU_STATE_BUFF0_IN_USE);
        REG_WR32(NV_PME_VBI_SU_STATE, stateSU);
    } 
    else // Buffer == 1
    {
        //
        // Choose the capture field (progressive == even)
        //
        if ((pDecoderHalObj->SetVbi[1].Field == NV04D_SET_VBI_FORMAT_FIELD_PROGRESSIVE)
         || (pDecoderHalObj->SetVbi[1].Field == NV04D_SET_VBI_FORMAT_FIELD_EVEN_FIELD))
            stateSU |= 1 << SF_SHIFT(NV_PME_VBI_SU_STATE_BUFF1_FIELD);
        else            
            stateSU &= ~(1 << SF_SHIFT(NV_PME_VBI_SU_STATE_BUFF1_FIELD));
    
        stateSU ^= 1 << SF_SHIFT(NV_PME_VBI_SU_STATE_BUFF1_IN_USE);
        REG_WR32(NV_PME_VBI_SU_STATE, stateSU);
    }

    return(RM_OK);
}


static RM_STATUS class04DStartImage
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECODERHALOBJECT pDecoderHalObj,
    U032 Buffer
)
{
    PMPHALINFO_NV04 pMpHalPvtInfo;
    V032        stateSU, stateME;
    U032        ScaleFactor;
    RM_STATUS   status;
    
    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DStartImage\r\n");

    pMpHalPvtInfo = (PMPHALINFO_NV04)pHalHwInfo->pMpHalPvtInfo;

    //
    // For now, we're going to assume the given buffer maps directly
    // to the hardware buffer.  We can add virtualization later.
    //

    //
    // Spend some time to validate all data
    //

    //
    // Make sure the engine is configured correctly (in case the old class66 overwrites)
    //
    if ( pMpHalPvtInfo->CurrentDecompressor == 0 )
        FLD_WR_DRF_DEF(_PME, _CONFIG_0, _BUS_MODE, _CCIR656);

    FLD_WR_DRF_DEF(_PME, _CONFIG_0, _IMAGE, _ENABLED);
    FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _IMAGE_NOTIFY, _ENABLED);
    
    //
    // The hardware can only support vbi dma transfers to framebuffer
    // addresses.  If this transfer is not completely contiguous,
    // we need to break the transfer up.
    //
    if (pDecoderHalObj->SetImage[Buffer].CtxDma->AddressSpace != ADDR_FBMEM)
        return (RM_ERROR);
    
    //
    // Calculate the scale factors given the in & out sizes
    //
    status = class04DCalculateScaleIncrement(
                pDecoderHalObj->SetImage[0].WidthIn,
                pDecoderHalObj->SetImage[0].HeightIn,
                pDecoderHalObj->SetImage[0].WidthOut,
                pDecoderHalObj->SetImage[0].HeightOut,
                &ScaleFactor);
    if (status)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: class04DFindNearestScaleFactor, cannot find nearest scale factor !\r\n");
        return (status);
    }

    //
    // Load up the buffer location and sizes
    //
    if (Buffer == 0)
    {
        REG_WR32(NV_PME_IMAGE_BUFF0_START, pDecoderHalObj->SetImage[0].Offset
                                         + (pDecoderHalObj->SetImage[0].CtxDma->PteArray[0] & 0xFFFFF000)
                                         + pDecoderHalObj->SetImage[0].CtxDma->PteAdjust);

        REG_WR32(NV_PME_IMAGE_BUFF0_PITCH, pDecoderHalObj->SetImage[0].Pitch);
        REG_WR32(NV_PME_IMAGE_BUFF0_LENGTH, pDecoderHalObj->SetImage[0].Pitch * pDecoderHalObj->SetImage[0].HeightOut);
        
        REG_WR32(NV_PME_IMAGE_BUFF0_SCALE_INCR, ScaleFactor);
    } 
    else
    {
        REG_WR32(NV_PME_IMAGE_BUFF1_START, pDecoderHalObj->SetImage[1].Offset
                                         + (pDecoderHalObj->SetImage[1].CtxDma->PteArray[0] & 0xFFFFF000)
                                         + pDecoderHalObj->SetImage[1].CtxDma->PteAdjust);

        REG_WR32(NV_PME_IMAGE_BUFF1_PITCH, pDecoderHalObj->SetImage[1].Pitch);
        REG_WR32(NV_PME_IMAGE_BUFF1_LENGTH, pDecoderHalObj->SetImage[1].Pitch * pDecoderHalObj->SetImage[1].HeightOut);
        
        REG_WR32(NV_PME_IMAGE_BUFF1_SCALE_INCR, ScaleFactor);
    } 
    
    //
    // Program the global image settings
    //
    REG_WR_DRF_NUM(_PME, _IMAGE_Y_CROP, _STARTLINE, pDecoderHalObj->ImageStartLine);
    
    //
    // Start the transfer
    //
    stateSU = REG_RD32(NV_PME_IMAGE_SU_STATE);
    stateME = REG_RD32(NV_PME_IMAGE_ME_STATE);
    //if (Buffer != (stateME >> 24))
    //    REG_WR32(NV_PME_IMAGE_ME_STATE, stateME ^ 0x01000000);

    if (Buffer == 0)
    {
        //
        // Choose the capture field (progressive == even)
        //
        if ((pDecoderHalObj->SetImage[0].Field == NV04D_SET_IMAGE_FORMAT_FIELD_PROGRESSIVE)
         || (pDecoderHalObj->SetImage[0].Field == NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD))
            stateSU |= 1 << SF_SHIFT(NV_PME_IMAGE_SU_STATE_BUFF0_FIELD);
        else            
            stateSU &= ~(1 << SF_SHIFT(NV_PME_IMAGE_SU_STATE_BUFF0_FIELD));
            
        stateSU ^= 1 << SF_SHIFT(NV_PME_IMAGE_SU_STATE_BUFF0_IN_USE);
        REG_WR32(NV_PME_IMAGE_SU_STATE, stateSU);
    } 
    else // Buffer == 1
    {
        //
        // Choose the capture field (progressive == even)
        //
        if ((pDecoderHalObj->SetImage[1].Field == NV04D_SET_IMAGE_FORMAT_FIELD_PROGRESSIVE)
         || (pDecoderHalObj->SetImage[1].Field == NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD))
            stateSU |= 1 << SF_SHIFT(NV_PME_IMAGE_SU_STATE_BUFF1_FIELD);
        else            
            stateSU &= ~(1 << SF_SHIFT(NV_PME_IMAGE_SU_STATE_BUFF1_FIELD));
    
        stateSU ^= 1 << SF_SHIFT(NV_PME_IMAGE_SU_STATE_BUFF1_IN_USE);
        REG_WR32(NV_PME_IMAGE_SU_STATE, stateSU);
    }
    
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

RM_STATUS class04DMethod_NV04(PMPMETHODARG_000 pMpMethodArg)
{
    PVIDEODECODERHALOBJECT pDecoderHalObj;
    PHALHWINFO pHalHwInfo;
    U032 offset, data, buffNum;

    pDecoderHalObj = (PVIDEODECODERHALOBJECT)pMpMethodArg->pHalObjInfo;
    offset = pMpMethodArg->offset;
    data = pMpMethodArg->data;
    pHalHwInfo = pMpMethodArg->pHalHwInfo;

    switch (offset)
    {
        case NV04D_STOP_TRANSFER_VBI:
            //
            // As per Joe Yuen email comments 10/30/98
            // You can force termination immediately by writing CONFIG_0_IMAGE
            // to DISABLED and CONFIG_0_VBI_MODE to DISABLED. This will stop
            // capture, flush the internal FIFOs, and set the error codes in
            // IMAGE_ME_STATE and VBI_ME_STATE to DISABLED (which you can
            // choose to ignore).
            //
            FLD_WR_DRF_DEF(_PME, _CONFIG_0, _VBI_MODE, _DISABLED);
    
            // silly us forgetting to disable interrupts before we should
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _VBI_NOTIFY, _DISABLED);
    
            // return things to a known state...
            // even though a buffer may STILL be running
            REG_WR32(NV_PME_VBI_ME_STATE, 0);
            REG_WR32(NV_PME_VBI_SU_STATE, 0);
            REG_WR32(NV_PME_VBI_RM_STATE, 0);
            break;
        case NV04D_STOP_TRANSFER_IMAGE:
            //
            // As per Joe Yuen email comments 10/30/98
            // You can force termination immediately by writing CONFIG_0_IMAGE
            // to DISABLED and CONFIG_0_VBI_MODE to DISABLED. This will stop
            // capture, flush the internal FIFOs, and set the error codes
            // in IMAGE_ME_STATE and VBI_ME_STATE to DISABLED (which you
            // can choose to ignore).
            //
            FLD_WR_DRF_DEF(_PME, _CONFIG_0, _IMAGE, _DISABLED);
         
            // silly us forgetting to disable interrupts before we should
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _IMAGE_NOTIFY, _DISABLED);
    
            // return things to a known state...
            // even though a buffer may STILL be running
            REG_WR32(NV_PME_IMAGE_ME_STATE, 0);
            REG_WR32(NV_PME_IMAGE_SU_STATE, 0);
            REG_WR32(NV_PME_IMAGE_RM_STATE, 0);
            break;
        case NV04D_SET_IMAGE_START_LINE:
            //
            // Load the local copy, but don't hit the hardware until
            // we actually need it
            //
            pDecoderHalObj->ImageStartLine = data;
            break;
        case NV04D_SET_VBI_SIZE(0):
        case NV04D_SET_VBI_SIZE(1):
            buffNum = (offset == NV04D_SET_VBI_SIZE(0)) ? 0 : 1;
            pDecoderHalObj->SetVbi[buffNum].FirstLine = (data & 0xFFFF);
            pDecoderHalObj->SetVbi[buffNum].Height    = ((data >> 16) & 0xFFFF);
            break;
        case NV04D_SET_VBI_OFFSET(0):
        case NV04D_SET_VBI_OFFSET(1):
            buffNum = (offset == NV04D_SET_VBI_OFFSET(0)) ? 0 : 1;
            pDecoderHalObj->SetVbi[buffNum].Offset = data;
            break;
        case NV04D_SET_VBI_FORMAT(0):
        case NV04D_SET_VBI_FORMAT(1):
            buffNum = (offset == NV04D_SET_VBI_FORMAT(0)) ? 0 : 1;
            pDecoderHalObj->SetVbi[buffNum].Pitch  = (data & 0xFFFF);
            pDecoderHalObj->SetVbi[buffNum].Field  = ((data >> 16) & 0xFF);
            pDecoderHalObj->SetVbi[buffNum].Notify = ((data >> 24) & 0xFF);
            //
            // Kick it off!!
            //
            class04DStartVbi(pHalHwInfo, pDecoderHalObj, buffNum);
            break;
        case NV04D_GET_VBI_OFFSET_NOTIFY(0):
        case NV04D_GET_VBI_OFFSET_NOTIFY(1):
            //
            // The scan position is the same for both buffers.
            //
            buffNum = (offset == NV04D_GET_VBI_OFFSET_NOTIFY(0)) ? 0 : 1;
            pDecoderHalObj->SetVbi[buffNum].GetOffsetData = REG_RD32(NV_PME_FIFO_CURRENT);
            break;
        case NV04D_SET_IMAGE_SIZE_IN(0):
        case NV04D_SET_IMAGE_SIZE_IN(1):
            buffNum = (offset == NV04D_SET_IMAGE_SIZE_IN(0)) ? 0 : 1;
            pDecoderHalObj->SetImage[buffNum].WidthIn  = (data & 0xFFFF);
            pDecoderHalObj->SetImage[buffNum].HeightIn = ((data >> 16) & 0xFFFF);
            break;
        case NV04D_SET_IMAGE_SIZE_OUT(0):
        case NV04D_SET_IMAGE_SIZE_OUT(1):
            buffNum = (offset == NV04D_SET_IMAGE_SIZE_OUT(0)) ? 0 : 1;
            pDecoderHalObj->SetImage[buffNum].WidthOut  = (data & 0xFFFF);
            pDecoderHalObj->SetImage[buffNum].HeightOut = ((data >> 16) & 0xFFFF);
            break;
        case NV04D_SET_IMAGE_OFFSET(0):
        case NV04D_SET_IMAGE_OFFSET(1):
            buffNum = (offset == NV04D_SET_IMAGE_OFFSET(0)) ? 0 : 1;
            pDecoderHalObj->SetImage[buffNum].Offset = data;
            break;
        case NV04D_SET_IMAGE_FORMAT(0):
        case NV04D_SET_IMAGE_FORMAT(1):
            buffNum = (offset == NV04D_SET_IMAGE_FORMAT(0)) ? 0 : 1;
            pDecoderHalObj->SetImage[buffNum].Pitch  = (data & 0xFFFF);
            pDecoderHalObj->SetImage[buffNum].Field  = ((data >> 16) & 0xFF);
            pDecoderHalObj->SetImage[buffNum].Notify = ((data >> 24) & 0xFF);
            
            //
            // Kick it off
            //
            class04DStartImage(pHalHwInfo, pDecoderHalObj, buffNum);
            break;
        case NV04D_GET_IMAGE_OFFSET_NOTIFY(0):
        case NV04D_GET_IMAGE_OFFSET_NOTIFY(1):
            //
            // The scan position is the same for both buffers.
            //
            buffNum = (offset == NV04D_GET_IMAGE_OFFSET_NOTIFY(0)) ? 0 : 1;
            pDecoderHalObj->SetImage[buffNum].GetOffsetData = REG_RD32(NV_PME_FIFO_CURRENT);
            break;
        default:
            // HAL doesn't have anything to do for this particular method
            ;
    }
    return NV04D_NOTIFICATION_STATUS_DONE_SUCCESS;
}

//---------------------------------------------------------------------------
//
//  Exception handling routines
//
//---------------------------------------------------------------------------

static VOID GetVbiEvents
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECODERHALOBJECT pDecoderHalObj,
    U032 *pEventsPending
)
{
    U032 stateRM, stateSU, stateME, stateMESU, stateMERM;
    U032 eventsPending = 0;

    REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _VBI_NOTIFY, _RESET));

    //
    // Grab the current states
    //
    stateME = REG_RD32(NV_PME_VBI_ME_STATE);
    stateSU = REG_RD32(NV_PME_VBI_SU_STATE);
    stateRM = REG_RD32(NV_PME_VBI_RM_STATE);
    stateMESU = stateME ^ stateSU;
    stateMERM = stateME ^ stateRM;

    // buffer 0
    if (*pEventsPending & CLASS04D_VBI_EVENT(0))
    {
        if (!(stateMESU & 0x00010000))
        {
            if (stateMERM & 0x00000001)
            {
                eventsPending |= CLASS04D_VBI_EVENT(0);
            }
        }
    }

    // buffer 1
    if (*pEventsPending & CLASS04D_VBI_EVENT(1))
    {
        if (!(stateMESU & 0x00100000))
        {
            if (stateMERM & 0x00000010)
            {
                eventsPending |= CLASS04D_VBI_EVENT(1);
            }
        }
    }

    *pEventsPending &= ~(CLASS04D_VBI_EVENT(0)|CLASS04D_VBI_EVENT(1));
    *pEventsPending |= eventsPending;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Leaving GetVbiEvents\n");

    return;
}

static VOID GetImageEvents
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECODERHALOBJECT pDecoderHalObj,
    U032 *pEventsPending
)
{
    U032 stateRM, stateSU, stateME, stateMESU, stateMERM;
    U032 eventsPending = 0;
    
        // clear the interrupt, in case another shows up!
        REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _IMAGE_NOTIFY, _RESET));
        
    //
    // Grab the current states
    //
    stateME = REG_RD32(NV_PME_IMAGE_ME_STATE);
    stateSU = REG_RD32(NV_PME_IMAGE_SU_STATE);
    stateRM = REG_RD32(NV_PME_IMAGE_RM_STATE);
    stateMESU = stateME ^ stateSU;
    stateMERM = stateME ^ stateRM;

    // buffer 0
    if (*pEventsPending & CLASS04D_IMAGE_EVENT(0))
    {
        pDecoderHalObj->SetImage[0].Info32 = 0;
        if (!(stateMESU & 0x00010000))
        {
            if (stateMERM & 0x00000001)
            {
                eventsPending |= CLASS04D_IMAGE_EVENT(0);
                //
                // Used for callback support
                //
                pHalHwInfo->pMpHalInfo->ImageBufferNumber = REG_RD32( NV_PME_IMAGE_BUFF0_START );
            }
        }
    }

    // buffer 1
    if (*pEventsPending & CLASS04D_IMAGE_EVENT(1))
    {
        pDecoderHalObj->SetImage[1].Info32 = 0;
        if (!(stateMESU & 0x00100000))
        {
            if (stateMERM & 0x00000010)
            {
                eventsPending |= CLASS04D_IMAGE_EVENT(1);
                //
                // Used for callback support
                //
                pHalHwInfo->pMpHalInfo->ImageBufferNumber = REG_RD32( NV_PME_IMAGE_BUFF1_START );
            }
        }
    }   

    *pEventsPending &= ~(CLASS04D_IMAGE_EVENT(0)|CLASS04D_IMAGE_EVENT(1));
    *pEventsPending |= eventsPending;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Leaving GetImageEvents\n");

    return;
}

//
// class04DGetEventStatus
//
// This call returns the status of the specified MP event(s).
//
RM_STATUS class04DGetEventStatus_NV04
(
    PMPGETEVENTSTATUSARG_000 pMpGetEventStatusArg
)
{
    PHALHWINFO pHalHwInfo;
    PVIDEODECODERHALOBJECT pDecoderHalObj;
    U032 intr0;
    U032 eventsPending;

    pHalHwInfo = pMpGetEventStatusArg->pHalHwInfo;

    //
    // Get current interrupt status and save off in exception frame.
    //
    intr0 = REG_RD32(NV_PME_INTR_0);
    pMpGetEventStatusArg->intrStatus = intr0;

    //
    // Check for pending VBI/IMAGE events.
    //
    if (!(intr0 & (DRF_DEF(_PME, _INTR_0, _VBI_NOTIFY, _PENDING) |
                   DRF_DEF(_PME, _INTR_0, _IMAGE_NOTIFY, _PENDING))))
    {
        pMpGetEventStatusArg->events = 0;
        return RM_OK;
    }

    //
    // No current decoder object, so treat exception as spurious.
    //
    if (pMpGetEventStatusArg->pHalObjInfo == NULL)
    {
        REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _VBI_NOTIFY, _RESET));
        REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _IMAGE_NOTIFY, _RESET));
        pMpGetEventStatusArg->events = 0;
        return (RM_ERROR);
    }

    pDecoderHalObj = (PVIDEODECODERHALOBJECT)pMpGetEventStatusArg->pHalObjInfo;

    //
    // The caller specifies a bit field of events for which it wishes
    // to get status.  If an event of interest isn't pending, then it's
    // bit is cleared.
    //
    eventsPending = pMpGetEventStatusArg->events;
    if (eventsPending & (CLASS04D_VBI_EVENT(0)|CLASS04D_VBI_EVENT(1)))
    {
        if (intr0 & DRF_DEF(_PME, _INTR_0, _VBI_NOTIFY, _PENDING))
            GetVbiEvents(pHalHwInfo, pDecoderHalObj, &eventsPending);
        else
            eventsPending &= ~(CLASS04D_VBI_EVENT(0)|CLASS04D_VBI_EVENT(1));
    }
    if (eventsPending & (CLASS04D_IMAGE_EVENT(0)|CLASS04D_IMAGE_EVENT(1)))
    {
        if (intr0 & DRF_DEF(_PME, _INTR_0, _IMAGE_NOTIFY, _PENDING))
            GetImageEvents(pHalHwInfo, pDecoderHalObj, &eventsPending);
        else
            eventsPending &= ~(CLASS04D_IMAGE_EVENT(0)|CLASS04D_IMAGE_EVENT(1));
    }

    //
    // Return set of events that are actually pending.
    //
    pMpGetEventStatusArg->events = eventsPending;

    return RM_OK;
}

//
// class04DServiceEvent
//
// Finish servicing specified event (i.e. reset buffer status).
//
RM_STATUS class04DServiceEvent_NV04
(
    PMPSERVICEEVENTARG_000 pMpServiceEventArg
)
{
    PHALHWINFO pHalHwInfo;
    U032 event, stateRM;
    RM_STATUS status = RM_OK;

    pHalHwInfo = pMpServiceEventArg->pHalHwInfo;

    //
    // The values were setup by the GetEventStatus interface.
    //
    event = pMpServiceEventArg->event;

    //
    // The eventsPending value should only specify a single
    // VBI *or* image buffer.
    //
    switch (event)
    {
        case CLASS04D_VBI_EVENT(0):
            stateRM = REG_RD32(NV_PME_VBI_RM_STATE);
            stateRM ^= 1 << SF_SHIFT(NV_PME_VBI_RM_STATE_BUFF0_INTR_NOTIFY);
            REG_WR32(NV_PME_VBI_RM_STATE, stateRM);
            break;
        case CLASS04D_VBI_EVENT(1):
            stateRM = REG_RD32(NV_PME_VBI_RM_STATE);
            stateRM ^= 1 << SF_SHIFT(NV_PME_VBI_RM_STATE_BUFF1_INTR_NOTIFY);
            REG_WR32(NV_PME_VBI_RM_STATE, stateRM);
            break;
        case CLASS04D_IMAGE_EVENT(0):
            stateRM = REG_RD32(NV_PME_IMAGE_RM_STATE);
            stateRM ^= 1 << SF_SHIFT(NV_PME_IMAGE_RM_STATE_BUFF0_INTR_NOTIFY);
            REG_WR32(NV_PME_IMAGE_RM_STATE, stateRM);
            break;
        case CLASS04D_IMAGE_EVENT(1):
            stateRM = REG_RD32(NV_PME_IMAGE_RM_STATE);
            stateRM ^= 1 << SF_SHIFT(NV_PME_IMAGE_RM_STATE_BUFF1_INTR_NOTIFY);
            REG_WR32(NV_PME_IMAGE_RM_STATE, stateRM);
            break;
        default:
            status = RM_ERR_BAD_ARGUMENT;
            break;
    }

    //
    // Return updated pending interrupt values.
    //
    pMpServiceEventArg->intrStatus = REG_RD32(NV_PME_INTR_0);
    
    return status;
}
