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
#include <nv10_ref.h>
#include <nvrm.h>
#include <nv10_hal.h>
#include "nvhw.h"
#include "nvhalcpp.h"

#ifdef DO_INTERRUPT_TIMING
U032 timePrev = 0;
#endif

//
// The following routines are used by the HAL mediaport engine
// manager in kernel/mp/nv10/mpnv10.c.
//
RM_STATUS class04EMethod_NV10(PMPMETHODARG_000);
RM_STATUS class04EGetEventStatus_NV10(PMPGETEVENTSTATUSARG_000);
RM_STATUS class04EServiceEvent_NV10(PMPSERVICEEVENTARG_000);

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

//
// Class instantiation/destruction is handled by mp/mpobj.c and
// mp/nv10/mpnv10.c.
//

//---------------------------------------------------------------------------
//
//  Miscellaneous class support routines.
//
//---------------------------------------------------------------------------
    
static RM_STATUS class04EStartRead(
    PHALHWINFO pHalHwInfo,
    PVIDEODECOMPRESSORHALOBJECT pDecompHalObj,
    U032 Buffer
)
{
    V032 stateSU, stateME;
#ifdef DEBUG
    V032 stateINTR0;
#endif

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04EStartRead\r\n");

#ifdef DEBUG
    //
    // Make sure there isn't an interrupt pending
    //
    stateINTR0 = REG_RD32(NV_PME_INTR_0);
    if ( stateINTR0 & DRF_DEF(_PME, _INTR_0, _FOUT_NOTIFY, _PENDING) ) {
		DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: class04EStartRead couldn't reset interrupt!\n");
		DBG_BREAKPOINT();
    }
#endif

    //
    // Make sure the engine is configured correctly 
    //
    FLD_WR_DRF_DEF(_PME, _HOST_CONFIG, _FOUT, _DISABLED);
    FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _FOUT_NOTIFY, _ENABLED);
    
    //
    // The hardware can only support dma transfers from contiguous
    // addresses.  If this transfer is not completely contiguous,
    // we need to break the transfer up.
	//
	// XXX for now we only support transfers out framebuffer memory
    //
    if (pDecompHalObj->ReadData[Buffer].CtxDma->AddressSpace != ADDR_FBMEM)
        return (RM_ERROR);
    
    //
    // Load up the buffer location and sizes
    //
	FLD_WR_DRF_DEF(_PME, _HOST_CONFIG, _FOUT_SYSMEM, _DISABLED);
	if (Buffer == 0)
	{
		REG_WR32(NV_PME_FOUT_BUFF0_START, pDecompHalObj->ReadData[0].Offset
										+ (pDecompHalObj->ReadData[0].CtxDma->PteArray[0] & 0xFFFFF000)
										//- pDev->Mapping.PhysFbAddr
										+ pDecompHalObj->ReadData[0].CtxDma->PteAdjust);

		// verify 4k!!
		REG_WR32(NV_PME_FOUT_BUFF0_LENGTH, pDecompHalObj->ReadData[0].Size);
	} 
	else
	{
		REG_WR32(NV_PME_FOUT_BUFF1_START, pDecompHalObj->ReadData[1].Offset
										+ (pDecompHalObj->ReadData[1].CtxDma->PteArray[0] & 0xFFFFF000)
										//- pDev->Mapping.PhysFbAddr
										+ pDecompHalObj->ReadData[1].CtxDma->PteAdjust);

		// verify 4k!!
		REG_WR32(NV_PME_FOUT_BUFF1_LENGTH, pDecompHalObj->ReadData[1].Size);
	} 
							
    //
    // Start the transfer
    //
    stateSU = REG_RD32(NV_PME_FOUT_SU_STATE);
    stateME = REG_RD32(NV_PME_FOUT_ME_STATE);

    if (Buffer == 0)
    {
        stateSU ^= 1 << SF_SHIFT(NV_PME_FOUT_SU_STATE_BUFF0_IN_USE);
        REG_WR32(NV_PME_FOUT_SU_STATE, stateSU);
    } 
    else // Buffer == 1
    {
        stateSU ^= 1 << SF_SHIFT(NV_PME_FOUT_SU_STATE_BUFF1_IN_USE);
        REG_WR32(NV_PME_FOUT_SU_STATE, stateSU);
    }

	FLD_WR_DRF_DEF(_PME, _HOST_CONFIG, _FOUT, _ENABLED);

    return(RM_OK);
}

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

RM_STATUS class04EMethod_NV10(PMPMETHODARG_000 pMpMethodArg)
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
            //
            // Stop the current data transfer immediately.
            //
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _FOUT_NOTIFY, _DISABLED);
            FLD_WR_DRF_DEF(_PME, _HOST_CONFIG, _FOUT, _DISABLED);
            REG_WR32(NV_PME_FOUT_SU_STATE, 0);
            REG_WR32(NV_PME_FOUT_ME_STATE, 0);
            REG_WR32(NV_PME_FOUT_RM_STATE, 0);
            REG_WR32(NV_PME_FOUT_CURRENT, 0);
            break;
        case NV04E_SET_DATA_OFFSET(0):
        case NV04E_SET_DATA_OFFSET(1):
            buffNum = (offset == NV04E_SET_DATA_OFFSET(0)) ? 0 : 1;
            pDecompHalObj->ReadData[buffNum].Offset = data;
            break;
        case NV04E_SET_DATA_LENGTH(0):
        case NV04E_SET_DATA_LENGTH(1):
            buffNum = (offset == NV04E_SET_DATA_LENGTH(0)) ? 0 : 1;
            pDecompHalObj->ReadData[buffNum].Size = data;
            break;
        case NV04E_SET_DATA_NOTIFY(0):
        case NV04E_SET_DATA_NOTIFY(1):
            buffNum = (offset == NV04E_SET_DATA_NOTIFY(0)) ? 0 : 1;
            pDecompHalObj->ReadData[buffNum].Notify = data;
            //
            // KICK IT OFF!!
            //
            class04EStartRead(pHalHwInfo, pDecompHalObj, buffNum);
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
            // NOT SUPPORTED!!
            //
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

static VOID GetVideoEvents(
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
        REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _FOUT_NOTIFY, _RESET));
        *pEventsPending &= ~(CLASS04E_VIDEO_EVENT(0)|CLASS04E_VIDEO_EVENT(1));
        return ;
    }

    //
    // Grab the current states
    //
    stateME = REG_RD32(NV_PME_FOUT_ME_STATE);
    stateSU = REG_RD32(NV_PME_FOUT_SU_STATE);
    stateRM = REG_RD32(NV_PME_FOUT_RM_STATE);
    stateMESU = stateME ^ stateSU;
    stateMERM = stateME ^ stateRM;

    REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _FOUT_NOTIFY, _RESET));

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


#if 0
static VOID GetImageEvents(
    PHALHWINFO pHalHwInfo,
    PVIDEODECOMPRESSORHALOBJECT pDecompHalObj,
    U032 *pEventsPending
)
{
    //
    // Not supported!!!
    //
    *pEventsPending &= ~(CLASS04E_IMAGE_EVENT(0)|CLASS04E_IMAGE_EVENT(1));

    return ;
}
#endif

//
// class04EGetEventStatus
//
// This call returns the status of the specified MP event(s).
//
RM_STATUS class04EGetEventStatus_NV10(
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
    if (!(intr0 & DRF_DEF(_PME, _INTR_0, _FOUT_NOTIFY, _PENDING)))
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
        if (intr0 & DRF_DEF(_PME, _INTR_0, _FOUT_NOTIFY, _PENDING))
            GetVideoEvents(pHalHwInfo, pDecompHalObj, &eventsPending);
        else
            eventsPending &= ~(CLASS04E_VIDEO_EVENT(0)|CLASS04E_VIDEO_EVENT(1));
    }
    if (eventsPending & (CLASS04E_IMAGE_EVENT(0)|CLASS04E_IMAGE_EVENT(1)))
    {
        //
        // Not supported!
        //if (intr0 & DRF_DEF(_PME, _INTR_0, _IMAGE_NOTIFY, _PENDING))
        //    GetImageEvents(pHalHwInfo, pDecoderHalObj, &eventsPending);
        //else
            eventsPending &= ~(CLASS04E_IMAGE_EVENT(0)|CLASS04E_IMAGE_EVENT(1));
    }

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
RM_STATUS class04EServiceEvent_NV10
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
            stateRM = REG_RD32(NV_PME_FOUT_RM_STATE);
            stateRM ^= 1 << SF_SHIFT(NV_PME_FOUT_RM_STATE_BUFF0_INTR_NOTIFY);
            REG_WR32(NV_PME_FOUT_RM_STATE, stateRM);
            break;
        case CLASS04E_VIDEO_EVENT(1):
            stateRM = REG_RD32(NV_PME_FOUT_RM_STATE);
            stateRM ^= 1 << SF_SHIFT(NV_PME_FOUT_RM_STATE_BUFF1_INTR_NOTIFY);
            REG_WR32(NV_PME_FOUT_RM_STATE, stateRM);
            break;
        case CLASS04E_AUDIO_EVENT(0):
        case CLASS04E_AUDIO_EVENT(1):
        case CLASS04E_IMAGE_EVENT(0):
        case CLASS04E_IMAGE_EVENT(1):
            //
            // These events aren't supported on nv10.
            //
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: class04EServiceEvent; bogus event type ", event);
            DBG_BREAKPOINT();
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
