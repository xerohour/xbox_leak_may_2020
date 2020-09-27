//
// (C) Copyright NVIDIA Corporation Inc., 1997-2000. All rights reserved.
//
/****************************** MediaPort Manager ***************************\
*                                                                           *
* Module: MPNV04.C                                                          *
*   The NV04 HAL MediaPort engine is managed in this module.                *
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
#include <i2c.h>
#include "nvhalcpp.h"

//
// HAL entry points.
//
RM_STATUS nvHalMpControl_NV04(VOID *);
RM_STATUS nvHalMpAlloc_NV04(VOID *);
RM_STATUS nvHalMpFree_NV04(VOID *);
RM_STATUS nvHalMpMethod_NV04(VOID *);
RM_STATUS nvHalMpGetEventStatus_NV04(VOID *);
RM_STATUS nvHalMpServiceEvent_NV04(VOID *);

//
// nvHalMpControl
//
RM_STATUS
nvHalMpControl_NV04(VOID *arg)
{
    PMPCONTROLARG_000 pMpControlArg = (PMPCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo = pMpControlArg->pHalHwInfo;
    PMPHALINFO pMpHalInfo;
    PDACHALINFO pDacHalInfo;

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
    pMpHalInfo = pHalHwInfo->pMpHalInfo;
    pDacHalInfo = pHalHwInfo->pDacHalInfo;

    switch (pMpControlArg->cmd)
    {
        case MP_CONTROL_UPDATE:
        case MP_CONTROL_LOAD:
        case MP_CONTROL_UNLOAD:
            break;
        case MP_CONTROL_INIT:
        {
            U008 daddr, subaddr, data;
            U008 lock;

            pMpHalInfo->MPCPresent = 0;
            pMpHalInfo->MPCDeviceAddr = 0;

#ifdef IKOS
            return RM_OK;
#endif

            //
            // Allow registry setting to bypass MPC detection.
            //
            if (pMpHalInfo->DetectEnable == 0)
                return RM_OK;

            //
            // Attempt to detect the presence of the external MP ASIC.  If it is not present, don't
            // enable decompressor functionality or attempt to initialize the mediaport
            // registers.  If there's no external clock, we could lock the system.

            // Unlock CRTC extended regs
            CRTC_READ(NV_CIO_SR_LOCK_INDEX, lock);
            CRTC_WRITE(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE);
    
            // wayne's MPC detection algorithm
            // (for now only go up to chip addr 0x25, eventhough 0x26/0x27
            // should also be reserved for our use)
            for (daddr = 0x24; daddr <= 0x25; daddr++)
            {
                subaddr = 0x1;
                data = 0x8F;
                if (HALRMI2CWRITE(pHalHwInfo, NV_I2C_PORT_SECONDARY, daddr, 1, &subaddr, 1, &data) != RM_OK)
                    continue;
        
                subaddr = 0x0;
                data = 0x80;
                if (HALRMI2CWRITE(pHalHwInfo, NV_I2C_PORT_SECONDARY, daddr, 1, &subaddr, 1, &data) != RM_OK)
                    continue;
        
                if (HALRMI2CREAD(pHalHwInfo, NV_I2C_PORT_SECONDARY, daddr, 1, &subaddr, 1, &data) != RM_OK)
                    continue;
                if ((data & 0xc0) != 0xc0)
                    continue;

                subaddr = 0x0;
                data = 0x40;
                if (HALRMI2CWRITE(pHalHwInfo, NV_I2C_PORT_SECONDARY, daddr, 1, &subaddr, 1, &data) != RM_OK)
                    continue;

                if (HALRMI2CREAD(pHalHwInfo, NV_I2C_PORT_SECONDARY, daddr, 1, &subaddr, 1, &data) != RM_OK)
                    continue;
                if ((data & 0xc0) == 0x0)
                {
                    // Found the MediaPort (save addr for use in class51)
                    pMpHalInfo->MPCPresent = 1;
                    pMpHalInfo->MPCDeviceAddr = daddr;
                    break;
                }
            }
    
            //
            // Initialize the mp registers to a known state.  Only do this
            // if we know someone is out there.
            //
            if (pMpHalInfo->MPCPresent)
            {    
    
                //
                // Reset the MPC.
                //
                // MPC Enabled, Video Decoder Disable, TV Encoder Disable
                // Toggle the MPC Reset and DVD Reset low then high
                //
        
                //
                // Internal MPC I2C
                //
                subaddr = 1;    // index
                data = 0xFF;
                HALRMI2CWRITE(pHalHwInfo, NV_I2C_PORT_SECONDARY, (U008)pMpHalInfo->MPCDeviceAddr, 1, &subaddr, 1, &data);
                subaddr = 0;    // data
                data = 0;
                HALRMI2CWRITE(pHalHwInfo, NV_I2C_PORT_SECONDARY, (U008)pMpHalInfo->MPCDeviceAddr, 1, &subaddr, 1, &data);
                subaddr = 0;    // data
                data = 0x07;
                HALRMI2CWRITE(pHalHwInfo, NV_I2C_PORT_SECONDARY, (U008)pMpHalInfo->MPCDeviceAddr, 1, &subaddr, 1, &data);
    
                //
                // Init the mediaport registers
                //
                // Initialize BUS_MODE to be disabled.  It will be initialized by either 
                // class04d (external decoder) or class04e (external decompressor) - JSUN
                //
                REG_WR32(NV_PME_CONFIG_0, DRF_DEF( _PME, _CONFIG_0, _BUS_MODE, _VMI));
                REG_WR32(NV_PME_VID_ME_STATE, 0);
                REG_WR32(NV_PME_VID_SU_STATE, 0);
                REG_WR32(NV_PME_VID_RM_STATE, 0);
                REG_WR32(NV_PME_AUD_ME_STATE, 0);
                REG_WR32(NV_PME_AUD_SU_STATE, 0);
                REG_WR32(NV_PME_AUD_RM_STATE, 0);
                REG_WR32(NV_PME_VBI_ME_STATE, 0);
                REG_WR32(NV_PME_VBI_SU_STATE, 0);
                REG_WR32(NV_PME_VBI_RM_STATE, 0);
                REG_WR32(NV_PME_IMAGE_ME_STATE, 0);
                REG_WR32(NV_PME_IMAGE_SU_STATE, 0);
                REG_WR32(NV_PME_IMAGE_RM_STATE, 0);
        
                REG_WR_DRF_DEF(_PME, _DEBUG_0, _DET_FIELD_SWITCH, _DISABLED);
                REG_WR32(NV_PME_DEBUG_1, DRF_DEF(_PME, _DEBUG_1, _SEL, _VIPCLK)
                         | DRF_DEF(_PME, _DEBUG_1, _VIPCLK_SEL, _DEFAULT)
                         | DRF_DEF(_PME, _DEBUG_1, _MCLK_SEL, _DEFAULT));
                                       
                //
                // enable MPC display data delivery
                //
                REG_WR32(NV_PME_EXTERNAL(2), 2);
    	
            }                               
            else
            {
                //
                // Default legacy state.  This is currently set manually when the old class66.c
                // is initialized and started.  When that class is retired and replaced with the
                // newer ExternalDecoder class, put this back in.
                //
                //REG_WR32(NV_PME_CONFIG_0, DRF_DEF( _PME, _CONFIG_0, _BUS_MODE, _CCIR656));
            }
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
nvHalMpAlloc_NV04(VOID *arg)
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

    pHalHwInfo =  pMpAllocArg->pHalHwInfo;

    switch (pMpAllocArg->classNum)
    {
        case NV03_EXTERNAL_VIDEO_DECODER:
        {
            PVIDEODECODERHALOBJECT pDecoderHalObj;
            PMPHALINFO_NV04 pMpHalPvtInfo;

            pDecoderHalObj = (PVIDEODECODERHALOBJECT)pMpAllocArg->pHalObjInfo;
            pMpHalPvtInfo = (PMPHALINFO_NV04)pMpAllocArg->pHalHwInfo->pMpHalPvtInfo;
#ifdef COMMENT
            //
            // Disable the MPC if there are no decompressor objects
            // using the MPC.
            //
            if ( pMpHalInfo->CurrentDecompressor == NULL ) {
                if ( pMpHalInfo->MPCPresent ) {
                    subaddr = 1;
                    data = 0x0f;
                    HALRMI2CWRITE(pHalHwInfo, (U008)(pMpHalInfo->MPCDeviceAddr), 1, &subaddr, 1, &data);
                    subaddr = 0;
                    data = 0x08;
                    HALRMI2CWRITE(pHalHwInfo, (U008)(pMpHalInfo->MPCDeviceAddr), 1, &subaddr, 1, &data);
                }
            }
#endif
            //
            // Make sure to clear the h/w state
            //
            REG_WR32(NV_PME_VBI_ME_STATE, 0);
            REG_WR32(NV_PME_VBI_SU_STATE, 0);
            REG_WR32(NV_PME_VBI_RM_STATE, 0);
            REG_WR32(NV_PME_IMAGE_ME_STATE, 0);
            REG_WR32(NV_PME_IMAGE_SU_STATE, 0);
            REG_WR32(NV_PME_IMAGE_RM_STATE, 0);
            break;
        }
        case NV03_EXTERNAL_VIDEO_DECOMPRESSOR:
        {
            PMPHALINFO_NV04 pMpHalPvtInfo;

            pMpHalPvtInfo = (PMPHALINFO_NV04)pHalHwInfo->pMpHalPvtInfo;

            //
            // MPC initialization
            //
            REG_WR32(NV_PME_EXTERNAL(3), 0x60);		// set wait states to 3
            //
            // initialize the media engine
            //
            FLD_WR_DRF_DEF(_PME, _CONFIG_0, _VID_CD, _DISABLED);
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _VID_NOTIFY, _DISABLED);
    
            REG_WR32(NV_PME_VID_ME_STATE, 0);
            REG_WR32(NV_PME_VID_SU_STATE, 0);
            REG_WR32(NV_PME_VID_RM_STATE, 0);
            REG_WR32(NV_PME_VID_CURRENT, 0);
    
            //
            // we also need to make sure Audio is not enabled (NV4 only)
            //
            if (IsNV4_NV04(pMpAllocArg->pHalHwInfo->pMcHalInfo))
            {
                REG_WR32(NV_PME_AUD_SU_STATE, 0);
                REG_WR32(NV_PME_AUD_ME_STATE, 0);
            }

            pMpHalPvtInfo->CurrentDecompressor = pMpAllocArg->pHalObjInfo;
            break;
        }
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
nvHalMpFree_NV04(VOID *arg)
{
    PMPFREEARG_000 pMpFreeArg = (PMPFREEARG_000)arg;
    PHALHWINFO pHalHwInfo = pMpFreeArg->pHalHwInfo;
    PMPHALINFO_NV04 pMpHalPvtInfo;

    //
    // Verify interface revision.
    //
    if (pMpFreeArg->id != MP_FREE_000)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: nvHalMpFree bad revision ", pMpFreeArg->id);
        DBG_BREAKPOINT();
        return (RM_ERR_VERSION_MISMATCH);
    }

    pMpHalPvtInfo = (PMPHALINFO_NV04)pMpFreeArg->pHalHwInfo->pMpHalPvtInfo;
    switch (pMpFreeArg->classNum)
    {
        case NV03_EXTERNAL_VIDEO_DECODER:
            //
            // Nothing interesting for this class.
            //
            break;
        case NV03_EXTERNAL_VIDEO_DECOMPRESSOR:
        {
#ifdef DISABLE_FOR_NOW
            //
            // disable the MPC 
            //
            subaddr = 1;
            data = 0x0f;
            HALRMI2CWRITE(pHalHwInfo, (U008)(pMpHalPvtInfo->MPCDeviceAddr), 1, &subaddr, 1, &data);
            subaddr = 0;
            data = 0x08;
            HALRMI2CWRITE(pHalHwInfo, (U008)(pMpHalPvtInfo->MPCDeviceAddr), 1, &subaddr, 1, &data);
#endif
            pMpHalPvtInfo = 0;
            break;
        }
        case NV1_EXTERNAL_PARALLEL_BUS:
            //
            // Nothing interesting for this class.
            //
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
nvHalMpMethod_NV04(VOID *arg)
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
            pMpMethodArg->mthdStatus = class04DMethod_NV04(pMpMethodArg);
            break;
        case NV03_EXTERNAL_VIDEO_DECOMPRESSOR:
            pMpMethodArg->mthdStatus = class04EMethod_NV04(pMpMethodArg);
            break;
        case NV1_EXTERNAL_PARALLEL_BUS:
            pMpMethodArg->mthdStatus = class04FMethod_NV04(pMpMethodArg);
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
nvHalMpGetEventStatus_NV04(VOID *arg)
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
            status = class04DGetEventStatus_NV04(pMpGetEventStatusArg);
            break;
        case NV03_EXTERNAL_VIDEO_DECOMPRESSOR:
            status = class04EGetEventStatus_NV04(pMpGetEventStatusArg);
            break;
        case NV1_EXTERNAL_PARALLEL_BUS:
            status = class04FGetEventStatus_NV04(pMpGetEventStatusArg);
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
nvHalMpServiceEvent_NV04(VOID *arg)
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
            status = class04DServiceEvent_NV04(pMpServiceEventArg);
            break;
        case NV03_EXTERNAL_VIDEO_DECOMPRESSOR:
            status = class04EServiceEvent_NV04(pMpServiceEventArg);
            break;
        case NV1_EXTERNAL_PARALLEL_BUS:
            status = class04FServiceEvent_NV04(pMpServiceEventArg);
            break;
        default:
            status = RM_ERR_ILLEGAL_OBJECT;
            break;
    }

    return (status);
}

