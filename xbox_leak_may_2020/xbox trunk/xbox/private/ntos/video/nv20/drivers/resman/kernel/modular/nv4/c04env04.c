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
* Module: CLASS04E.C                                                        *
*   This module implements the NV_EXTERNAL_VIDEO_DECOMPRESSOR object class  *
*   and its corresponding methods.                                          *
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

#ifdef DO_INTERRUPT_TIMING
U032 timePrev = 0;
#endif

//
// The following routines are used by the HAL mediaport engine
// manager in kernel/mp/nv4/mpnv04.c.
//
RM_STATUS class04EMethod_NV04(PMPMETHODARG_000);
RM_STATUS class04EGetEventStatus_NV04(PMPGETEVENTSTATUSARG_000);
RM_STATUS class04EServiceEvent_NV04(PMPSERVICEEVENTARG_000);

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
    
static RM_STATUS class04EStartVidRead
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECOMPRESSORHALOBJECT pDecompHalObj,
    U032 Buffer
)
{
    V032 stateSU, stateME;
#ifdef DEBUG
    V032 stateINTR0;
#endif
        
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04EStartVidRead\r\n");

#ifdef DEBUG
    //
    // Make sure there isn't an interrupt pending
    //
    stateINTR0 = REG_RD32(NV_PME_INTR_0);
    if ( stateINTR0 & DRF_DEF(_PME, _INTR_0, _VID_NOTIFY, _PENDING) ) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: class04EStartVidRead detected unexpected interrupt!\r\n");
        DBG_BREAKPOINT();
    }
#endif

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
    REG_WR32(NV_PME_EXTERNAL(2), 6);    /* disable MPC polling */
    FLD_WR_DRF_DEF(_PME, _CONFIG_0, _BUS_MODE, _VMI);
    FLD_WR_DRF_DEF(_PME, _CONFIG_0, _VID_CD, _ENABLED);
    FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _VID_NOTIFY, _ENABLED);
    
    //
    // The hardware can only support dma transfers from contiguous
    // addresses.  If this transfer is not completely contiguous,
    // we need to break the transfer up.
    //
#if 0
    if (pDecompHalObj->ReadData[Buffer].CtxDma->AddressSpace == ADDR_SYSMEM)
    {
        REG_WR32(NV_PME_CONFIG_1, DRF_DEF(_PME, _CONFIG_1, _BUFFS, _SYS)
                                | DRF_DEF(_PME, _CONFIG_1, _HOST, _PCI));

        if (Buffer == 0)
        {
            REG_WR32(NV_PME_VID_BUFF0_START_SYS, pDecompHalObj->ReadData[0].Offset
                                               + (U032)pDecompHalObj->ReadData[0].CtxDma->BufferBase
                                               + pDecompHalObj->ReadData[0].CtxDma->PteAdjust);

            // verify 4k!!
            REG_WR32(NV_PME_VID_BUFF0_LENGTH, pDecompHalObj->ReadData[0].Size);
        } 
        else
        {
            REG_WR32(NV_PME_VID_BUFF1_START_SYS, pDecompHalObj->ReadData[1].Offset
                                               + (U032)pDecompHalObj->ReadData[1].CtxDma->BufferBase
                                               + pDecompHalObj->ReadData[1].CtxDma->PteAdjust);

            // verify 4k!!
            REG_WR32(NV_PME_VID_BUFF1_LENGTH, pDecompHalObj->ReadData[1].Size);
        } 

    }
    else if (pDecompHalObj->ContextDmaData[Buffer]->AddressSpace == ADDR_AGPMEM)
    {
        REG_WR32(NV_PME_CONFIG_1, DRF_DEF(_PME, _CONFIG_1, _BUFFS, _SYS)
                                | DRF_DEF(_PME, _CONFIG_1, _HOST, _AGP));
    }
    else
    {
#endif
        // FB_MEM
        REG_WR32(NV_PME_CONFIG_1, DRF_DEF(_PME, _CONFIG_1, _BUFFS, _PNVM)
                                | DRF_DEF(_PME, _CONFIG_1, _HOST, _PCI));
                            
        if (Buffer == 0)
        {
            REG_WR32(NV_PME_VID_BUFF0_START_PNVM, pDecompHalObj->ReadData[0].Offset
                                                + (pDecompHalObj->ReadData[0].CtxDma->PteArray[0] & 0xFFFFF000)
                                                //- pDev->Mapping.PhysFbAddr
                                                + pDecompHalObj->ReadData[0].CtxDma->PteAdjust);

            // verify 4k!!
            REG_WR32(NV_PME_VID_BUFF0_LENGTH, pDecompHalObj->ReadData[0].Size);
        } 
        else
        {
            REG_WR32(NV_PME_VID_BUFF1_START_PNVM, pDecompHalObj->ReadData[1].Offset
                                                + (pDecompHalObj->ReadData[1].CtxDma->PteArray[0] & 0xFFFFF000)
                                                //- pDev->Mapping.PhysFbAddr
                                                + pDecompHalObj->ReadData[1].CtxDma->PteAdjust);

            // verify 4k!!
            REG_WR32(NV_PME_VID_BUFF1_LENGTH, pDecompHalObj->ReadData[1].Size);
        } 
#if 0                                
    }
#endif    
    
    //
    // Start the transfer
    //
    stateSU = REG_RD32(NV_PME_VID_SU_STATE);
    stateME = REG_RD32(NV_PME_VID_ME_STATE);
    //if (Buffer != (stateME >> 24))
    //    REG_WR32(NV_PME_VID_ME_STATE, stateME ^ 0x01000000);

    if (Buffer == 0)
    {
        stateSU ^= 1 << SF_SHIFT(NV_PME_VID_SU_STATE_BUFF0_IN_USE);
        REG_WR32(NV_PME_VID_SU_STATE, stateSU);
    } 
    else // Buffer == 1
    {
        stateSU ^= 1 << SF_SHIFT(NV_PME_VID_SU_STATE_BUFF1_IN_USE);
        REG_WR32(NV_PME_VID_SU_STATE, stateSU);
    }

    //
    // we also need to enable Audio in order for Video to work (NV4 only)
    //
    if ( IsNV4_NV04(pHalHwInfo->pMcHalInfo) ) {
        REG_WR32(NV_PME_AUD_ME_STATE, 0);
        stateSU = (1 << SF_SHIFT(NV_PME_AUD_SU_STATE_BUFF0_IN_USE)) |
                  (1 << SF_SHIFT(NV_PME_AUD_SU_STATE_BUFF1_IN_USE));
        REG_WR32(NV_PME_AUD_SU_STATE, stateSU);
    }

    //
    // enable MPC polling
    //
    REG_WR32(NV_PME_EXTERNAL(2), 7);
    
    return(RM_OK);
}

static RM_STATUS class04EStartAudRead
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECOMPRESSORHALOBJECT pDecompHalObj,
    U032 Buffer
)
{
    V032 stateSU, stateME;
#ifdef DEBUG
    V032 stateINTR0;
#endif

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04EStartAudRead\r\n");

#ifdef DEBUG
    //
    // Make sure there isn't an interrupt pending
    //
    stateINTR0 = REG_RD32(NV_PME_INTR_0);
    if ( stateINTR0 & DRF_DEF(_PME, _INTR_0, _AUD_NOTIFY, _PENDING) ) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: class04EStartAudRead detected unexpected interrupt!\r\n");
        DBG_BREAKPOINT();
    }
#endif

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
    REG_WR32(NV_PME_EXTERNAL(2), 0x26); /* disable MPC polling */
    FLD_WR_DRF_DEF(_PME, _CONFIG_0, _BUS_MODE, _VMI);
    FLD_WR_DRF_DEF(_PME, _CONFIG_0, _AUD_CD, _ENABLED);
    FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _AUD_NOTIFY, _ENABLED);
    
    {
        // FB_MEM
        REG_WR32(NV_PME_CONFIG_1, DRF_DEF(_PME, _CONFIG_1, _BUFFS, _PNVM)
                                | DRF_DEF(_PME, _CONFIG_1, _HOST, _PCI));
                            
        if (Buffer == 0)
        {
            REG_WR32(NV_PME_AUD_BUFF0_START_PNVM, pDecompHalObj->ReadData[0].Offset
                                                + (pDecompHalObj->ReadData[0].CtxDma->PteArray[0] & 0xFFFFF000)
                                                //- pDev->Mapping.PhysFbAddr
                                                + pDecompHalObj->ReadData[0].CtxDma->PteAdjust);

            REG_WR32(NV_PME_AUD_BUFF0_LENGTH, pDecompHalObj->ReadData[0].Size);
        } 
        else
        {
            REG_WR32(NV_PME_AUD_BUFF1_START_PNVM, pDecompHalObj->ReadData[1].Offset
                                                + (pDecompHalObj->ReadData[1].CtxDma->PteArray[0] & 0xFFFFF000)
                                                //- pDev->Mapping.PhysFbAddr
                                                + pDecompHalObj->ReadData[1].CtxDma->PteAdjust);

            REG_WR32(NV_PME_AUD_BUFF1_LENGTH, pDecompHalObj->ReadData[1].Size);
        } 
                                
    }
    
    //
    // Start the transfer
    //
    stateSU = REG_RD32(NV_PME_AUD_SU_STATE);
    stateME = REG_RD32(NV_PME_AUD_ME_STATE);
    //if (Buffer != (stateME >> 24))
    //    REG_WR32(NV_PME_VID_ME_STATE, stateME ^ 0x01000000);

    if (Buffer == 0)
    {
        stateSU ^= 1 << SF_SHIFT(NV_PME_AUD_SU_STATE_BUFF0_IN_USE);
        REG_WR32(NV_PME_AUD_SU_STATE, stateSU);
    } 
    else // Buffer == 1
    {
        stateSU ^= 1 << SF_SHIFT(NV_PME_AUD_SU_STATE_BUFF1_IN_USE);
        REG_WR32(NV_PME_AUD_SU_STATE, stateSU);
    }

    //
    // we also need to enable Video in order for Audio to work (NV4 only)
    //
    if ( IsNV4_NV04(pHalHwInfo->pMcHalInfo) ) {
        REG_WR32(NV_PME_VID_ME_STATE, 0);
        stateSU = (1 << SF_SHIFT(NV_PME_VID_SU_STATE_BUFF0_IN_USE)) |
                  (1 << SF_SHIFT(NV_PME_VID_SU_STATE_BUFF1_IN_USE));
        REG_WR32(NV_PME_VID_SU_STATE, stateSU);
    }

    //
    // enable MPC polling
    //
    REG_WR32(NV_PME_EXTERNAL(2), 0x27);
    
    return(RM_OK);
}

static RM_STATUS class04EStartWrite
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECOMPRESSORHALOBJECT pDecompHalObj,
    U032 Buffer
)
{
    U032 stateSU, stateME;
    U032 size;

    //
    // For now, we're going to assume the given buffer maps directly
    // to the hardware buffer.  We can add virtualization later.
    //
    //
    // Spend some time to validate all data
    //

    //
    // The hardware can only support dma transfers from contiguous
    // addresses.  If this transfer is not completely contiguous,
    // we need to break the transfer up.
    //
    // For now, assume we are contiguous and in fb memory
    //
    if (pDecompHalObj->ReadData[Buffer].CtxDma->AddressSpace == ADDR_FBMEM)
    {
        if (Buffer == 0)
        {
            REG_WR32(NV_PME_IMAGE_BUFF0_START, (U032)(pDecompHalObj->WriteImage[0].Offset
                                               // TO DO: this is a known truncation for IA64
                                             + (SIZE_PTR)pDecompHalObj->WriteImage[0].CtxDma->BufferBase
                                             + pDecompHalObj->WriteImage[0].CtxDma->PteAdjust));

            REG_WR32(NV_PME_IMAGE_BUFF0_PITCH, pDecompHalObj->WriteImage[0].FormatOut & 0xFFFF);

            size = (pDecompHalObj->WriteImage[0].Size & 0xFFFF) * (pDecompHalObj->WriteImage[0].Size >> 16);
            REG_WR32(NV_PME_IMAGE_BUFF0_LENGTH, size);
        } 
        else
        {
            REG_WR32(NV_PME_IMAGE_BUFF1_START, (U032)(pDecompHalObj->WriteImage[1].Offset
                                               // TO DO: this is a known truncation for IA64
                                             + (SIZE_PTR)pDecompHalObj->WriteImage[1].CtxDma->BufferBase
                                             + pDecompHalObj->WriteImage[1].CtxDma->PteAdjust));

            REG_WR32(NV_PME_IMAGE_BUFF1_PITCH, pDecompHalObj->WriteImage[1].FormatOut & 0xFFFF);

            size = (pDecompHalObj->WriteImage[1].Size & 0xFFFF) * (pDecompHalObj->WriteImage[1].Size >> 16);
            REG_WR32(NV_PME_IMAGE_BUFF1_LENGTH, size);
        } 

    }
    else 
        ;// NOT SUPPORTED!!!
    
    //
    // Start the transfer
    //
    stateSU = REG_RD32(NV_PME_IMAGE_SU_STATE);
    stateME = REG_RD32(NV_PME_IMAGE_ME_STATE);
    if (Buffer != (stateME >> 24))
        REG_WR32(NV_PME_IMAGE_ME_STATE, stateME ^ 0x01000000);

    if (Buffer == 0)
    {
        stateSU ^= 1 << SF_SHIFT(NV_PME_IMAGE_SU_STATE_BUFF0_IN_USE);
        REG_WR32(NV_PME_IMAGE_SU_STATE, stateSU);
    } 
    else // Buffer == 1
    {
        stateSU ^= 1 << SF_SHIFT(NV_PME_IMAGE_SU_STATE_BUFF1_IN_USE);
        REG_WR32(NV_PME_IMAGE_SU_STATE, stateSU);
    }

    return(RM_OK);
}

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

RM_STATUS class04EMethod_NV04(PMPMETHODARG_000 pMpMethodArg)
{
    PVIDEODECOMPRESSORHALOBJECT pDecompHalObj;
    PHALHWINFO pHalHwInfo;
    U032 offset, data;
    U032 buffNum;

    pDecompHalObj = (PVIDEODECOMPRESSORHALOBJECT)pMpMethodArg->pHalObjInfo;
    offset = pMpMethodArg->offset;
    data = pMpMethodArg->data;
    pHalHwInfo = pMpMethodArg->pHalHwInfo;

    switch (offset)
    {
        case NV04E_STOP_TRANSFER:
            FLD_WR_DRF_DEF(_PME, _CONFIG_0, _VID_CD, _DISABLED);
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _VID_NOTIFY, _DISABLED);
            FLD_WR_DRF_DEF(_PME, _CONFIG_0, _AUD_CD, _DISABLED);
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _AUD_NOTIFY, _DISABLED);

            REG_WR32(NV_PME_VID_SU_STATE, 0);
            REG_WR32(NV_PME_VID_ME_STATE, 0);
            REG_WR32(NV_PME_VID_RM_STATE, 0);
            REG_WR32(NV_PME_VID_CURRENT, 0);
            REG_WR32(NV_PME_AUD_SU_STATE, 0);
            REG_WR32(NV_PME_AUD_ME_STATE, 0);
            REG_WR32(NV_PME_AUD_RM_STATE, 0);
            REG_WR32(NV_PME_AUD_CURRENT, 0);

            // XXX make sure the MPC's FIFO's are empty
            REG_WR32(NV_PME_EXTERNAL(2), 6);    /* disable MPC polling */
            break;
        case NV04E_SET_DATA_OFFSET(0):
        case NV04E_SET_DATA_OFFSET(1):
            buffNum = (offset == NV04E_SET_DATA_OFFSET(0)) ? 0 : 1;
            pDecompHalObj->ReadData[buffNum].Offset = data;
            //
            // Go ahead and write the start address .
            // (write both and aud and vid registers since we
            // don't know which one we'll use yet)
            // 
            if (buffNum == 0) {
                REG_WR32(NV_PME_VID_BUFF0_START_PNVM, pDecompHalObj->ReadData[0].Offset
                         + (pDecompHalObj->ReadData[0].CtxDma->PteArray[0] & 0xFFFFF000)
                         + pDecompHalObj->ReadData[0].CtxDma->PteAdjust);
                REG_WR32(NV_PME_AUD_BUFF0_START_PNVM, pDecompHalObj->ReadData[0].Offset
                         + (pDecompHalObj->ReadData[0].CtxDma->PteArray[0] & 0xFFFFF000)
                         + pDecompHalObj->ReadData[0].CtxDma->PteAdjust);
            } 
            else {
                REG_WR32(NV_PME_VID_BUFF1_START_PNVM, pDecompHalObj->ReadData[1].Offset
                         + (pDecompHalObj->ReadData[1].CtxDma->PteArray[0] & 0xFFFFF000)
                         + pDecompHalObj->ReadData[1].CtxDma->PteAdjust);
                REG_WR32(NV_PME_AUD_BUFF1_START_PNVM, pDecompHalObj->ReadData[1].Offset
                         + (pDecompHalObj->ReadData[1].CtxDma->PteArray[0] & 0xFFFFF000)
                         + pDecompHalObj->ReadData[1].CtxDma->PteAdjust);
            } 
            break;
        case NV04E_SET_DATA_LENGTH(0):
        case NV04E_SET_DATA_LENGTH(1):
            buffNum = (offset == NV04E_SET_DATA_LENGTH(0)) ? 0 : 1;
            pDecompHalObj->ReadData[buffNum].Size = data;
            //
            // go ahead and write the length
            // (use audio engine if size < 4K)
            // 
            if (buffNum == 0) {
                if ( data < 4096 ) {
                    REG_WR32(NV_PME_AUD_BUFF0_LENGTH, pDecompHalObj->ReadData[0].Size);
                    REG_WR32(NV_PME_VID_BUFF0_LENGTH, 0);
                }
                else {
                    REG_WR32(NV_PME_AUD_BUFF0_LENGTH, 0);
                    REG_WR32(NV_PME_VID_BUFF0_LENGTH, pDecompHalObj->ReadData[0].Size);
                }
            } 
            else {
                if ( data < 4096 ) {
                    REG_WR32(NV_PME_AUD_BUFF1_LENGTH, pDecompHalObj->ReadData[1].Size);
                    REG_WR32(NV_PME_VID_BUFF1_LENGTH, 0);
                }
                else {
                    REG_WR32(NV_PME_AUD_BUFF1_LENGTH, 0);
                    REG_WR32(NV_PME_VID_BUFF1_LENGTH, pDecompHalObj->ReadData[1].Size);
                }
            }
            break;
        case NV04E_SET_DATA_NOTIFY(0):
        case NV04E_SET_DATA_NOTIFY(1):
            buffNum = (offset == NV04E_SET_DATA_NOTIFY(0)) ? 0 : 1;
            pDecompHalObj->ReadData[buffNum].Notify = data;

            //
            // KICK IT OFF!!
            //
            if (pDecompHalObj->ReadData[buffNum].Size < 4096) {
                class04EStartAudRead(pHalHwInfo, pDecompHalObj, buffNum);
            }
            else {
                class04EStartVidRead(pHalHwInfo, pDecompHalObj, buffNum);
            }
            break;
        case NV04E_SET_IMAGE_FORMAT_IN(0):
        case NV04E_SET_IMAGE_FORMAT_IN(1):
            buffNum = (offset == NV04E_SET_IMAGE_FORMAT_IN(0)) ? 0 : 1;
            pDecompHalObj->WriteImage[buffNum].FormatIn = data;
            break;
        case NV04E_SET_IMAGE_SIZE(0):
        case NV04E_SET_IMAGE_SIZE(1):
            buffNum = (offset == NV04E_SET_IMAGE_SIZE(0)) ? 0 : 1;
            pDecompHalObj->WriteImage[buffNum].Size = data;
            break;
        case NV04E_SET_IMAGE_OFFSET_OUT(0):
        case NV04E_SET_IMAGE_OFFSET_OUT(1):
            buffNum = (offset == NV04E_SET_IMAGE_OFFSET_OUT(0)) ? 0 : 1;
            pDecompHalObj->WriteImage[buffNum].Offset = data;
            break;
        case NV04E_SET_IMAGE_FORMAT_OUT(0):
        case NV04E_SET_IMAGE_FORMAT_OUT(1):
            buffNum = (offset == NV04E_SET_IMAGE_FORMAT_OUT(0)) ? 0 : 1;
            pDecompHalObj->WriteImage[buffNum].FormatOut = data;
            break;
        case NV04E_SET_IMAGE_NOTIFY(0):
        case NV04E_SET_IMAGE_NOTIFY(1):
            buffNum = (offset == NV04E_SET_IMAGE_NOTIFY(0)) ? 0 : 1;
            pDecompHalObj->WriteImage[buffNum].Notify = data;
            //
            // Kick it off
            //
            class04EStartWrite(pHalHwInfo, pDecompHalObj, buffNum);
            break;
        default:
            // HAL doesn't have anything to do for this particular method
            ;
    }
    return NV04E_NOTIFICATION_STATUS_DONE_SUCCESS;
}

//---------------------------------------------------------------------------
//
//  Exception handling routines
//
//---------------------------------------------------------------------------

static VOID GetVideoEvents
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECOMPRESSORHALOBJECT pDecompHalObj,
    U032 *pEventsPending
)
{
    U032 stateRM, stateSU, stateME, stateMESU, stateMERM;
    U032 eventsPending = 0;
#ifdef DEBUG
    V032 stateINTR0;
#endif

    // pause any transfers that might be going
    FLD_WR_DRF_DEF(_PME, _CONFIG_0, _VID_CD, _DISABLED);

    //
    // Check for spurious interrupt.
    //
    if (pDecompHalObj == NULL)
    {
        REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _VID_NOTIFY, _RESET));
        *pEventsPending &= ~(CLASS04E_VIDEO_EVENT(0)|CLASS04E_VIDEO_EVENT(1));
        return ;
    }

    //
    // Grab the current states
    //
    stateME = REG_RD32(NV_PME_VID_ME_STATE);
    stateSU = REG_RD32(NV_PME_VID_SU_STATE);
    stateRM = REG_RD32(NV_PME_VID_RM_STATE);
    stateMESU = stateME ^ stateSU;
    stateMERM = stateME ^ stateRM;

    REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _VID_NOTIFY, _RESET));

#ifdef DEBUG
    stateINTR0 = REG_RD32(NV_PME_INTR_0);
    if ( stateINTR0 & DRF_DEF(_PME, _INTR_0, _VID_NOTIFY, _PENDING) ) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: class04EVideoNotify couldn't reset interrupt!\r\n");
        DBG_BREAKPOINT();
    }
#endif

    // buffer 0
    if (*pEventsPending & CLASS04E_VIDEO_EVENT(0))
    {
        if (!(stateMESU & 0x00010000))
        {
            if (stateMERM & 0x00000001)
            {
                eventsPending |= CLASS04E_VIDEO_EVENT(0);
            }
        }
    }

    // buffer 1
    if (*pEventsPending & CLASS04E_VIDEO_EVENT(1))
    {
        if (!(stateMESU & 0x00100000))
        {
            if (stateMERM & 0x00000010)
            {
                eventsPending |= CLASS04E_VIDEO_EVENT(1);
            }
        }
    }

    *pEventsPending &= ~(CLASS04E_VIDEO_EVENT(0)|CLASS04E_VIDEO_EVENT(1));
    *pEventsPending |= eventsPending;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Leaving GetVideoEvents\n");

    return;
}

static VOID GetAudioEvents
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECOMPRESSORHALOBJECT pDecompHalObj,
    U032 *pEventsPending
)
{
    U032 stateRM, stateSU, stateME, stateMESU, stateMERM;
    U032 eventsPending = 0;
#ifdef DEBUG
    V032 stateINTR0;
#endif

    // pause any transfers that might be going
    FLD_WR_DRF_DEF(_PME, _CONFIG_0, _AUD_CD, _DISABLED);

    //
    // Check for spurious interrupt.
    //
    if (pDecompHalObj == NULL)
    {
        REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _AUD_NOTIFY, _RESET));
        *pEventsPending &= ~(CLASS04E_AUDIO_EVENT(0)|CLASS04E_AUDIO_EVENT(1));
        return ;
    }

    //
    // Grab the current states
    //
    stateME = REG_RD32(NV_PME_AUD_ME_STATE);
    stateSU = REG_RD32(NV_PME_AUD_SU_STATE);
    stateRM = REG_RD32(NV_PME_AUD_RM_STATE);
    stateMESU = stateME ^ stateSU;
    stateMERM = stateME ^ stateRM;

    REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _AUD_NOTIFY, _RESET));

#ifdef DEBUG
    stateINTR0 = REG_RD32(NV_PME_INTR_0);
    if ( stateINTR0 & DRF_DEF(_PME, _INTR_0, _AUD_NOTIFY, _PENDING) ) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: class04EVideoNotify couldn't reset interrupt!\r\n");
        DBG_BREAKPOINT();
    }
#endif

    // buffer 0
    if (*pEventsPending & CLASS04E_AUDIO_EVENT(0))
    {
        if (!(stateMESU & 0x00010000))
        {
            if (stateMERM & 0x00000001)
            {
                eventsPending |= CLASS04E_AUDIO_EVENT(0);
            }
        }
    }

    // buffer 1
    if (*pEventsPending & CLASS04E_AUDIO_EVENT(1))
    {
        if (!(stateMESU & 0x00100000))
        {
            if (stateMERM & 0x00000010)
            {
                eventsPending |= CLASS04E_AUDIO_EVENT(1);
            }
        }
    }

    *pEventsPending &= ~(CLASS04E_AUDIO_EVENT(0)|CLASS04E_AUDIO_EVENT(1));
    *pEventsPending |= eventsPending;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Leaving GetAudioEvents\n");

    return;
}

#ifdef NOTYET
static VOID GetImageEvents
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECOMPRESSORHALOBJECT pDecompHalObj,
    U032 *pEventsPending
)
{
    U032 stateRM, stateSU, stateME, stateMESU, stateMERM;
    U032 eventsPending = 0;

    //
    // Check for spurious interrupt.
    //
    if (pDecompHalObj == NULL)
    {
        REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _IMAGE_NOTIFY, _RESET));
        *pEventsPending &= ~(CLASS04E_IMAGE_EVENT(0)|CLASS04E_IMAGE_EVENT(1));
        return ;
    }

    //
    // Grab the current states
    //
    stateME = REG_RD32(NV_PME_IMAGE_ME_STATE);
    stateSU = REG_RD32(NV_PME_IMAGE_SU_STATE);
    stateRM = REG_RD32(NV_PME_IMAGE_RM_STATE);
    stateMESU = stateME ^ stateSU;
    stateMERM = stateME ^ stateRM;

    REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _IMAGE_NOTIFY, _RESET));

    // buffer 0
    if (*pEventsPending & CLASS04E_IMAGE_EVENT(0))
    {
        if (!(stateMESU & 0x00010000))
        {
            if (stateMERM & 0x00000001)
            {
                eventsPending |= CLASS04E_IMAGE_EVENT(0);
            }
        }
    }

    // buffer 1
    if (*pEventsPending & CLASS04E_IMAGE_EVENT(1))
    {
        if (!(stateMESU & 0x00100000))
        {
            if (stateMERM & 0x00000010)
            {
                eventsPending |= CLASS04E_IMAGE_EVENT(1);
            }
        }
    }

    *pEventsPending &= ~(CLASS04E_IMAGE_EVENT(0)|CLASS04E_IMAGE_EVENT(1));
    *pEventsPending |= eventsPending;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Leaving GetImageEvents\n");

    return;
}
#endif

//
// class04EGetEventStatus
//
// This call returns the status of the specified MP event(s).
//
RM_STATUS class04EGetEventStatus_NV04
(
    PMPGETEVENTSTATUSARG_000 pMpGetEventStatusArg
)
{
    PHALHWINFO pHalHwInfo;
    PVIDEODECOMPRESSORHALOBJECT pDecompHalObj;
    U032 intr0;
    U032 eventsPending;

    pHalHwInfo = pMpGetEventStatusArg->pHalHwInfo;

    //
    // Get current interrupt status and save off in exception frame.
    //
    intr0 = REG_RD32(NV_PME_INTR_0);
    pMpGetEventStatusArg->intrStatus = intr0;

    //
    // Check for pending events.
    //
    if (!(intr0 & (DRF_DEF(_PME, _INTR_0, _VID_NOTIFY, _PENDING) |
                   DRF_DEF(_PME, _INTR_0, _AUD_NOTIFY, _PENDING))))
    {
        pMpGetEventStatusArg->events = 0;
        return RM_OK;
    }

    //
    // Setup HAL object pointer.
    //
    if (pMpGetEventStatusArg->pHalObjInfo == NULL)
        pDecompHalObj = NULL;
    else
        pDecompHalObj = (PVIDEODECOMPRESSORHALOBJECT)pMpGetEventStatusArg->pHalObjInfo;

    //
    // The caller specifies a bit field of events for which it wishes
    // to get status.  If an event of interest isn't pending, then it's
    // bit is cleared.
    //
    eventsPending = pMpGetEventStatusArg->events;
    if (eventsPending & (CLASS04E_VIDEO_EVENT(0)|CLASS04E_VIDEO_EVENT(1)))
    {
        if (intr0 & DRF_DEF(_PME, _INTR_0, _VID_NOTIFY, _PENDING))
            GetVideoEvents(pHalHwInfo, pDecompHalObj, &eventsPending);
        else
            eventsPending &= ~(CLASS04E_VIDEO_EVENT(0)|CLASS04E_VIDEO_EVENT(1));
    }
    if (eventsPending & (CLASS04E_AUDIO_EVENT(0)|CLASS04E_AUDIO_EVENT(1)))
    {
        if (intr0 & DRF_DEF(_PME, _INTR_0, _AUD_NOTIFY, _PENDING))
            GetAudioEvents(pHalHwInfo, pDecompHalObj, &eventsPending);
        else
            eventsPending &= ~(CLASS04E_AUDIO_EVENT(0)|CLASS04E_AUDIO_EVENT(1));
    }

#ifdef NOTYET
    if (eventsPending & (CLASS04E_IMAGE_EVENT(0)|CLASS04E_IMAGE_EVENT(1)))
    {
        if (intr0 & DRF_DEF(_PME, _INTR_0, _IMAGE_NOTIFY, _PENDING))
            GetImageEvents(pHalHwInfo, pDecompHalObj, &eventsPending);
        else
            eventsPending &= ~(CLASS04E_IMAGE_EVENT(0)|CLASS04E_IMAGE_EVENT(1));
    }
#endif

    //
    // Return set of events that are actually pending.
    //
    pMpGetEventStatusArg->events = eventsPending;

    return RM_OK;
}

//
// class04EServiceEvent
//
// Finish servicing specified event (i.e. reset buffer status).
//
RM_STATUS class04EServiceEvent_NV04
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
        case CLASS04E_VIDEO_EVENT(0):
            stateRM = REG_RD32(NV_PME_VID_RM_STATE);
            stateRM ^= 1 << SF_SHIFT(NV_PME_VID_RM_STATE_BUFF0_INTR_NOTIFY);
            REG_WR32(NV_PME_VID_RM_STATE, stateRM);
            break;
        case CLASS04E_VIDEO_EVENT(1):
            stateRM = REG_RD32(NV_PME_VID_RM_STATE);
            stateRM ^= 1 << SF_SHIFT(NV_PME_VID_RM_STATE_BUFF1_INTR_NOTIFY);
            REG_WR32(NV_PME_VID_RM_STATE, stateRM);
            break;
        case CLASS04E_AUDIO_EVENT(0):
            stateRM = REG_RD32(NV_PME_AUD_RM_STATE);
            stateRM ^= 1 << SF_SHIFT(NV_PME_AUD_RM_STATE_BUFF0_INTR_NOTIFY);
            REG_WR32(NV_PME_AUD_RM_STATE, stateRM);
            break;
        case CLASS04E_AUDIO_EVENT(1):
            stateRM = REG_RD32(NV_PME_AUD_RM_STATE);
            stateRM ^= 1 << SF_SHIFT(NV_PME_AUD_RM_STATE_BUFF1_INTR_NOTIFY);
            REG_WR32(NV_PME_AUD_RM_STATE, stateRM);
            break;
#ifdef NOTYET
        case CLASS04E_IMAGE_EVENT(0):
            stateRM = REG_RD32(NV_PME_IMAGE_RM_STATE);
            stateRM ^= 1 << SF_SHIFT(NV_PME_IMAGE_RM_STATE_BUFF0_INTR_NOTIFY);
            REG_WR32(NV_PME_IMAGE_RM_STATE, stateRM);
            break;
        case CLASS04E_IMAGE_EVENT(1):
            stateRM = REG_RD32(NV_PME_IMAGE_RM_STATE);
            stateRM ^= 1 << SF_SHIFT(NV_PME_IMAGE_RM_STATE_BUFF1_INTR_NOTIFY);
            REG_WR32(NV_PME_IMAGE_RM_STATE, stateRM);
            break;
#endif
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

