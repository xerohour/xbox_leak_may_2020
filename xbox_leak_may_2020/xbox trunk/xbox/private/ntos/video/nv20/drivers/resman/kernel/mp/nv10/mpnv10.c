//
// (C) Copyright NVIDIA Corporation Inc., 1997-2000. All rights reserved.
//
/****************************** MediaPort Manager ***************************\
*                                                                           *
* Module: MPNV10.C                                                          *
*   The NV10 HAL MediaPort engine is managed in this module.                *
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
// HAL entry points.
//
RM_STATUS nvHalMpControl_NV10(VOID *);
RM_STATUS nvHalMpAlloc_NV10(VOID *);
RM_STATUS nvHalMpFree_NV10(VOID *);
RM_STATUS nvHalMpMethod_NV10(VOID *);
RM_STATUS nvHalMpGetEventStatus_NV10(VOID *);
RM_STATUS nvHalMpServiceEvent_NV10(VOID *);

//
// nvHalMpControl
//
RM_STATUS
nvHalMpControl_NV10(VOID *arg)
{
    PMPCONTROLARG_000 pMpControlArg = (PMPCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo = pMpControlArg->pHalHwInfo;
    PMPHALINFO_NV10 pMpHalPvtInfo;

    //
    // Verify interface revision.
    //
    if (pMpControlArg->id != MP_CONTROL_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalMpControl bad revision ", pMpControlArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    pHalHwInfo = pMpControlArg->pHalHwInfo;
    pMpHalPvtInfo = (PMPHALINFO_NV10)pHalHwInfo->pMpHalPvtInfo;

    switch (pMpControlArg->cmd)
    {
        case MP_CONTROL_UPDATE:
        case MP_CONTROL_LOAD:
        case MP_CONTROL_UNLOAD:
            break;
        case MP_CONTROL_INIT:
        {
            U032 hostConfig;

#ifdef IKOS
            //
            // Don't check for VIP slave (IKOS Temporary)
            //
            if (IsNV10orBetter_NV10(pHalHwInfo->pMcHalInfo)) {
                return (RM_OK);
            }
#endif

            pMpHalPvtInfo->VIPSlavePresent = 0;
            hostConfig = REG_RD32(NV_PME_HOST_CONFIG);
            if ((hostConfig & DRF_DEF(_PME, _HOST_CONFIG, _SLAVE_DETECTED, _TRUE)) &&
                (hostConfig & DRF_DEF(_PME, _HOST_CONFIG, _SLAVE_NOTDETECTED, _FALSE)) ) {
                pMpHalPvtInfo->VIPSlavePresent = 1;
            }

            //
            // Initialize media engine's config
            //

            REG_WR32(NV_PME_INTR_EN_0, 0);      // make sure all the media engine interrupts are disabled

            FLD_WR_DRF_NUM(_PME, _HOST_CONFIG, _FIFOMAXTX, 32);
            FLD_WR_DRF_DEF(_PME, _HOST_CONFIG, _CLOCK_SELECT, _PCI);
            FLD_WR_DRF_NUM(_PME, _HOST_CONFIG, _TIMEOUT, 7);	// just in case

            //
            // Initialize media engine's various DMA engines
            //

            REG_WR32(NV_PME_ANC_ME_STATE, 0);
            REG_WR32(NV_PME_ANC_SU_STATE, 0);
            REG_WR32(NV_PME_ANC_RM_STATE, 0);
            REG_WR32(NV_PME_TASKA_ME_STATE, 0);
            REG_WR32(NV_PME_TASKA_SU_STATE, 0);
            REG_WR32(NV_PME_TASKA_RM_STATE, 0);
            REG_WR32(NV_PME_TASKB_ME_STATE, 0);
            REG_WR32(NV_PME_TASKB_SU_STATE, 0);
            REG_WR32(NV_PME_TASKB_RM_STATE, 0);
            REG_WR32(NV_PME_FOUT_ME_STATE, 0);
            REG_WR32(NV_PME_FOUT_SU_STATE, 0);
            REG_WR32(NV_PME_FOUT_RM_STATE, 0);
            REG_WR32(NV_PME_FIN_ME_STATE, 0);
            REG_WR32(NV_PME_FIN_SU_STATE, 0);
            REG_WR32(NV_PME_FIN_RM_STATE, 0);

            //
            // Clear 656_CONFIG
            //

            REG_WR32(NV_PME_656_CONFIG, 0);

            //
            // initialize FOUT_ADDR and FIN_ADDR to something on our VIP test card
            //
            // XXX need a method to set these addresses though!  (class04E has no address method)
            //
            FLD_WR_DRF_NUM(_PME, _FOUT_ADDR, _FIFO, 4);		// FIFO A
            FLD_WR_DRF_NUM(_PME, _FOUT_ADDR, _DEVICE, 0);

            FLD_WR_DRF_NUM(_PME, _FIN_ADDR, _FIFO, 5);		// FIFO B
            FLD_WR_DRF_NUM(_PME, _FIN_ADDR, _DEVICE, 0);

            break;
        }
        case MP_CONTROL_DESTROY:
            break;
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }

    return (RM_OK);
}

//
// nvHalMpAlloc
//
RM_STATUS
nvHalMpAlloc_NV10(VOID *arg)
{
    PMPALLOCARG_000 pMpAllocArg = (PMPALLOCARG_000)arg;
    PHALHWINFO pHalHwInfo = pMpAllocArg->pHalHwInfo;
    RM_STATUS status = RM_OK;

    //
    // Verify interface revision.
    //
    if (pMpAllocArg->id != MP_ALLOC_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalMpAlloc bad revision ", pMpAllocArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    pHalHwInfo = pMpAllocArg->pHalHwInfo;

    switch (pMpAllocArg->classNum)
    {
        case NV03_EXTERNAL_VIDEO_DECODER:
        {
            PVIDEODECODERHALOBJECT pDecoderHalObj;

            pDecoderHalObj = (PVIDEODECODERHALOBJECT)pMpAllocArg->pHalObjInfo;

            pDecoderHalObj->lastWidthIn     = 0;
            pDecoderHalObj->lastWidthOut    = 0;
            pDecoderHalObj->lastHeightIn    = 0;
            pDecoderHalObj->lastHeightOut   = 0;

            pDecoderHalObj->lastImageConfig = 0xFFFFFFFF;
            //
            // To force initialization of the registers
            // this is used to indicate the first time around... 99
            // will likely never be used at a startline value since it
            // should be something like 0 - 21.
            //
            pDecoderHalObj->lastImageStartLine  = 99;     

            //
            // Clear the various DMA controls 
            //
            REG_WR32(NV_PME_ANC_ME_STATE, 0);
            REG_WR32(NV_PME_ANC_SU_STATE, 0);
            REG_WR32(NV_PME_ANC_RM_STATE, 0);
            REG_WR32(NV_PME_TASKA_ME_STATE, 0);
            REG_WR32(NV_PME_TASKA_SU_STATE, 0);
            REG_WR32(NV_PME_TASKA_RM_STATE, 0);
            REG_WR32(NV_PME_TASKB_ME_STATE, 0);
            REG_WR32(NV_PME_TASKB_SU_STATE, 0);
            REG_WR32(NV_PME_TASKB_RM_STATE, 0);

            //
            // Initialize 656 config
            // (For now we only handle TASKA streams.)
            //
            FLD_WR_DRF_NUM(_PME, _656_CONFIG, _TASKA_ENABLE, 1);
            FLD_WR_DRF_DEF(_PME, _656_CONFIG, _TASKA_ONLY, _DISABLED);

            //
            // initialize maximum line length
            //
            REG_WR32(NV_PME_TASKA_LINE_LENGTH, 0xfffff0);
            REG_WR32(NV_PME_TASKB_LINE_LENGTH, 0xfffff0);

            break;
        }
        case NV03_EXTERNAL_VIDEO_DECOMPRESSOR:
            //
            // initialize the media engine
            //
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _FOUT_NOTIFY, _DISABLED);
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _FIN_NOTIFY, _DISABLED);
    
            REG_WR32(NV_PME_FOUT_ME_STATE, 0);
            REG_WR32(NV_PME_FOUT_SU_STATE, 0);
            REG_WR32(NV_PME_FOUT_RM_STATE, 0);
            REG_WR32(NV_PME_FOUT_CURRENT, 0);
            REG_WR32(NV_PME_FIN_ME_STATE, 0);
            REG_WR32(NV_PME_FIN_SU_STATE, 0);
            REG_WR32(NV_PME_FIN_RM_STATE, 0);
            REG_WR32(NV_PME_FIN_CURRENT, 0);

            break;
        case NV1_EXTERNAL_PARALLEL_BUS:
            //
            // Nothing interesting for this class.
            //
            break;
        default:
            status = RM_ERR_ILLEGAL_OBJECT;
            break;
    }

    return (status);
}

//
// nvHalMpFree
//
RM_STATUS
nvHalMpFree_NV10(VOID *arg)
{
    PMPFREEARG_000 pMpFreeArg = (PMPFREEARG_000)arg;
    PHALHWINFO pHalHwInfo = pMpFreeArg->pHalHwInfo;

    //
    // Verify interface revision.
    //
    if (pMpFreeArg->id != MP_FREE_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalMpFree bad revision ", pMpFreeArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    pHalHwInfo = pMpFreeArg->pHalHwInfo;

    switch (pMpFreeArg->classNum)
    {
        case NV03_EXTERNAL_VIDEO_DECODER:
            //
            // Nothing interesting for this class.
            //
            break;
        case NV03_EXTERNAL_VIDEO_DECOMPRESSOR:
            //
            // Nothing interesting for this class.
            //
            break;
        case NV1_EXTERNAL_PARALLEL_BUS:
            //
            // Let's be forceful and stop either read/write if any in progress.
            //
            FLD_WR_DRF_DEF(_PME, _VIPREG_CTRL, _READ, _NOT_PENDING);
            FLD_WR_DRF_DEF(_PME, _VIPREG_CTRL, _WRITE, _NOT_PENDING);
            break;
        default:
            return RM_ERR_ILLEGAL_OBJECT;
    }

    return (RM_OK);
}

//
// nvHalMpMethod
//
RM_STATUS
nvHalMpMethod_NV10(VOID *arg)
{
    PMPMETHODARG_000 pMpMethodArg = (PMPMETHODARG_000)arg;
    PHALHWINFO pHalHwInfo = pMpMethodArg->pHalHwInfo;

    //
    // Verify interface revision.
    //
    if (pMpMethodArg->id != MP_METHOD_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalMpMethod bad revision ", pMpMethodArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    switch (pMpMethodArg->classNum)
    {
        case NV03_EXTERNAL_VIDEO_DECODER:
            pMpMethodArg->mthdStatus = class04DMethod_NV10(pMpMethodArg);
            break;
        case NV03_EXTERNAL_VIDEO_DECOMPRESSOR:
            pMpMethodArg->mthdStatus = class04EMethod_NV10(pMpMethodArg);
            break;
        case NV1_EXTERNAL_PARALLEL_BUS:
            pMpMethodArg->mthdStatus = class04FMethod_NV10(pMpMethodArg);
            break;
        default:
            return RM_ERR_ILLEGAL_OBJECT;
    }

    return (RM_OK);
}

//
// nvHalMpGetEventStatus
//
// This interface accepts a bit mask of events for which
// the RM wishes to obtain pending status.  If an event bit
// bit is set, this routine will leave it set if it's pending,
// or clear it if it is not.
//
RM_STATUS
nvHalMpGetEventStatus_NV10(VOID *arg)
{
    PMPGETEVENTSTATUSARG_000 pMpGetEventStatusArg = (PMPGETEVENTSTATUSARG_000)arg;
    PHALHWINFO pHalHwInfo = pMpGetEventStatusArg->pHalHwInfo;
    RM_STATUS status;

    //
    // Verify interface revision.
    //
    if (pMpGetEventStatusArg->id != MP_GET_EVENT_STATUS_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalMpGetEventStatus bad revision ", pMpGetEventStatusArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    switch (pMpGetEventStatusArg->classNum)
    {
        case NV03_EXTERNAL_VIDEO_DECODER:
            status = class04DGetEventStatus_NV10(pMpGetEventStatusArg);
            break;
        case NV03_EXTERNAL_VIDEO_DECOMPRESSOR:
            status = class04EGetEventStatus_NV10(pMpGetEventStatusArg);
            break;
        case NV1_EXTERNAL_PARALLEL_BUS:
            status = class04FGetEventStatus_NV10(pMpGetEventStatusArg);
            break;
        default:
            status = RM_ERR_ILLEGAL_OBJECT;
            break;
    }

    return (status);
}

//
// nvHalMpServiceEvent
//
// This interface completes event processing (e.g. resets
// buffer status).
//
RM_STATUS
nvHalMpServiceEvent_NV10(VOID *arg)
{
    PMPSERVICEEVENTARG_000 pMpServiceEventArg = (PMPSERVICEEVENTARG_000)arg;
    PHALHWINFO pHalHwInfo = pMpServiceEventArg->pHalHwInfo;
    RM_STATUS status;

    //
    // Verify interface revision.
    //
    if (pMpServiceEventArg->id != MP_SERVICE_EVENT_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalMpServiceEvent bad revision ", pMpServiceEventArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    switch (pMpServiceEventArg->classNum)
    {
        case NV03_EXTERNAL_VIDEO_DECODER:
            status = class04DServiceEvent_NV10(pMpServiceEventArg);
            break;
        case NV03_EXTERNAL_VIDEO_DECOMPRESSOR:
            status = class04EServiceEvent_NV10(pMpServiceEventArg);
            break;
        case NV1_EXTERNAL_PARALLEL_BUS:
            status = class04FServiceEvent_NV10(pMpServiceEventArg);
            break;
        default:
            status = RM_ERR_ILLEGAL_OBJECT;
            break;
    }

    return (status);
}
