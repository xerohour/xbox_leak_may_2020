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

/**************************** Graphics Manager *****************************\
*                                                                           *
* Module: GRNV04.C                                                          *
*   The graphics chip dependent HAL GR routines are kept here.              *
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

// forwards
RM_STATUS nvHalGrControl_NV04(VOID *);
RM_STATUS nvHalGrAlloc_NV04(VOID *);
RM_STATUS nvHalGrFree_NV04(VOID *);
RM_STATUS nvHalGrGetExceptionData_NV04(VOID *);
RM_STATUS nvHalGrService_NV04(VOID *);
RM_STATUS nvHalGrGetNotifyData_NV04(VOID *);
RM_STATUS nvHalGrSetObjectContext_NV04(VOID *);
RM_STATUS nvHalGrLoadOverride_NV04(VOID *);

// statics
static RM_STATUS _nvHalGrInitInstance_NV04(PHALHWINFO, U032, U032);
static RM_STATUS _nvHalGrSetContextColorKeyWar_NV04(PHALHWINFO, PGREXCEPTIONDATA);

// externs
extern RM_STATUS nvHalGrClassSoftwareMethod_NV04(PHALHWINFO, PGREXCEPTIONDATA);
extern RM_STATUS nvHalGrLoadChannelContext_NV04(PHALHWINFO, U032);
extern RM_STATUS nvHalGrUnloadChannelContext_NV04(PHALHWINFO, U032);

//
// nvHalGrControl
//
RM_STATUS
nvHalGrControl_NV04(VOID *arg)
{
    PGRCONTROLARG_000 pGrControlArg = (PGRCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PGRHALINFO pGrHalInfo;
    PGRHALINFO_NV04 pGrHalPvtInfo;
    U032 i;

    //
    // Verify interface revision.
    //
    if (pGrControlArg->id != GR_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrControlArg->pHalHwInfo;
    pGrHalInfo = pHalHwInfo->pGrHalInfo;
    pGrHalPvtInfo = (PGRHALINFO_NV04)pHalHwInfo->pGrHalPvtInfo;

    switch (pGrControlArg->cmd)
    {
        case GR_CONTROL_INIT:
            //
            // Some of these bits will be dynamically modified.
            //
            switch (REG_RD_DRF(_PMC, _BOOT_0, _MAJOR_REVISION))
            {
                case NV4_REV_A_00:
                    pGrHalPvtInfo->Debug0 = DRF_DEF(_PGRAPH, _DEBUG_0, _LIMIT_CHECK,     _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_0, _LIMIT_INT,       _DISABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_0, _OVRFLW_INT,      _DISABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_0, _WRITE_ONLY_ROPS_2D, _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_0, _WRITE_ONLY_ROPS_3D, _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_0, _DRAWDIR_AUTO,    _DISABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_0, _DRAWDIR_Y,       _INCR)
                        | DRF_DEF(_PGRAPH, _DEBUG_0, _ALPHA_ABORT,     _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_0, _MINUSD5,         _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_0, _BLIT_DST_LIMIT,  _ENABLED);
        
                    pGrHalPvtInfo->Debug1 = DRF_DEF(_PGRAPH, _DEBUG_1, _VOLATILE_RESET,  _NOT_LAST)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _DMA_ACTIVITY,    _IGNORE) 
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _PATCH_INV,       _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _TRI_OPTS,        _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _TRICLIP_OPTS,    _DISABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _INSTANCE,        _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _CTX,             _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _CACHE,           _IGNORE)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _CACHE_MODE,      _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _ZCLAMP,          _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _UCLAMP,          _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _RCLAMP,          _ENABLED);

                    pGrHalPvtInfo->Debug2 = DRF_DEF(_PGRAPH, _DEBUG_2,  _PINGPONG,        _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ZBUF_SEQ,        _AUTO)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _FAST_VERTEX_LOAD, _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _BILINEAR_3D,     _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ANISOTROPIC_3D,  _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _FOG_3D,          _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _SPECULAR_3D,     _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ALPHA_3D,        _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _COELESCE_D3D,    _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _COELESCE_2D,     _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _BLIT_MULTILINE,  _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _VOLATILE_RESET,  _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _SPARE1,          _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _SPARE2,          _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _SPARE3,          _ENABLED);

                    pGrHalPvtInfo->Debug3 = DRF_DEF(_PGRAPH, _DEBUG_3, _CULLING,         _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _CULLING_TYPE,    _DX3)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _FAST_DATA_STRTCH, _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _FAST_DATA_D3D,   _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _ZFLUSH,          _IGNORE)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _AUTOZFLUSH_PTZ,  _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _AUTOZFLUSH_D3D,  _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _POSTDITHER_2D,   _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _POSTDITHER_3D,   _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _PREDITHER_2D,    _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _PREDITHER_2D,    _DISABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _PREDITHER_3D,    _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _SLOT_CONFLICT_PTZ, _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _SLOT_CONFLICT_D3D, _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _EARLYZ_ABORT,    _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _FORCE_CREAD,     _DISABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _FORCE_ZREAD,     _DISABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _FORMAT_CHECK,    _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _TRIEND_FLUSH,    _DISABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _DMA_CHECK,       _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _DATA_CHECK,      _ENABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _DATA_CHECK_FAIL, _DISABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _RAMREADBACK,     _DISABLED)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _STATE_CHECK,     _ENABLED);
                    break;
            }

            if (IsNV5orBetter_NV04(pHalHwInfo->pMcHalInfo)) {
                pGrHalPvtInfo->Debug1 |= DRF_DEF(_PGRAPH, _DEBUG_1, _BIDIR_DRAIN,         _DISABLED)
                    | DRF_DEF(_PGRAPH, _DEBUG_1, _EARLY_POST,          _ENABLED)
                    | DRF_DEF(_PGRAPH, _DEBUG_1, _FIXED_ADRS,          _DISABLED)
                    | DRF_DEF(_PGRAPH, _DEBUG_1, _ALT_RW_SEQ,          _DISABLED)
                    | DRF_DEF(_PGRAPH, _DEBUG_1, _DITHER_RANGE_ADJ_2D, _ENABLED)
                    | DRF_DEF(_PGRAPH, _DEBUG_1, _DITHER_RANGE_ADJ_3D, _ENABLED)
                    | DRF_DEF(_PGRAPH, _DEBUG_1, _DX6_2PIXMODE,        _ENABLED);

                /* Uncomment this after the hardware is fixed.
                   pDev->Graphics.Debug1 |= DRF_DEF(_PGRAPH, _DEBUG_1, _ALOM_BURST,          _ENABLED);
                   */

                pGrHalPvtInfo->Debug2 |= DRF_DEF(_PGRAPH, _DEBUG_2, _MCLK_RECTS,        _ENABLED);

                //
                // Disable HONOR_SRCFMT.  Note this is overloaded from PINGPONG in NV4
                //
                pGrHalPvtInfo->Debug2 &= ~DRF_DEF(_PGRAPH, _DEBUG_2, _HONOR_SRCFMT,      _ENABLED);

                pGrHalPvtInfo->Debug3 |= DRF_DEF(_PGRAPH, _DEBUG_3, _EXCLUDE_ROP_IN_IDLE, _ENABLED);

                // Enable setContext, SetOpetaion, and setClip methods in hardware.
                pGrHalPvtInfo->Debug3 |= DRF_DEF(_PGRAPH, _DEBUG_3, _PREDITHER_2D,        _ENABLED)
                    | DRF_DEF(_PGRAPH, _DEBUG_3, _CLIP_METHODS,        _ENABLED)
                    | DRF_DEF(_PGRAPH, _DEBUG_3, _CONTEXT_METHODS,     _ENABLED)
                    | DRF_DEF(_PGRAPH, _DEBUG_3, _OPERATION_METHOD,    _ENABLED)
                    | DRF_DEF(_PGRAPH, _DEBUG_3, _IGNORE_PATCHVALID,   _ENABLED);
                /* Uncomment this after the hardware is fixed.
                   // Enable fast image_from_memory.
                   pGrHalInfo->Debug3 |= DRF_DEF(_PGRAPH, _DEBUG_3, _FAST_DATA_IMAGE, _ENABLED);
                   */
            }

            //
            // We're now context switching PGRAPH_DEBUG_3, in order to support SetColorConversion
            // for NV4_INDEXED_IMAGE (NV5_INDEXED_IMAGE has a real method/context state). Load the
            // grTable version of Debug3 with what we've figured out above.
            //
            for (i = 0; i < NUM_FIFOS_NV04; i++)
            {
                pGrHalPvtInfo->grChannels[i].Debug3 = pGrHalPvtInfo->Debug3;
            }

            //
            // Set default channel to unused chid
            //
            pGrHalPvtInfo->currentChID = INVALID_CHID;
            break;
        case GR_CONTROL_LOAD:
            REG_WR_DRF_DEF(_PGRAPH, _DEBUG_1, _VOLATILE_RESET, _LAST);
            REG_WR_DRF_DEF(_PGRAPH, _DEBUG_0, _STATE, _RESET);
            FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_1, _DMA_ACTIVITY, _CANCEL);

            //
            // Load debug overrides.
            //            
            REG_WR32(NV_PGRAPH_DEBUG_0, pGrHalPvtInfo->Debug0);
            REG_WR32(NV_PGRAPH_DEBUG_1, pGrHalPvtInfo->Debug1);
            REG_WR32(NV_PGRAPH_DEBUG_2, pGrHalPvtInfo->Debug2);
            REG_WR32(NV_PGRAPH_DEBUG_3, pGrHalPvtInfo->Debug3);

            REG_WR32(NV_PGRAPH_CTX_SWITCH1,  0x00000000);
            REG_WR32(NV_PGRAPH_CTX_SWITCH2,  0x00000000);
            REG_WR32(NV_PGRAPH_CTX_SWITCH3,  0x00000000);
            REG_WR32(NV_PGRAPH_CTX_SWITCH4,  0x00000000);
            REG_WR32(NV_PGRAPH_CTX_CONTROL,
                     DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US)
                     | DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME,         _EXPIRED)
                     | DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID,         _INVALID)
                     | DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING,    _IDLE)
                     | DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE,       _ENABLED));
            REG_WR_DRF_DEF(_PGRAPH, _FFINTFC_ST2, _CHID_STATUS, _VALID);

            //
            // Reload current graphics context.
            //
            (void) nvHalGrLoadChannelContext_NV04(pHalHwInfo, pGrHalPvtInfo->currentChID);
            break;
        case GR_CONTROL_UNLOAD:
            //
            // Pull out current graphics state.
            //
            (void) nvHalGrLoadChannelContext_NV04(pHalHwInfo, INVALID_CHID);

            REG_WR32(NV_PGRAPH_CTX_CONTROL,
                     DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US)
                     | DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME,         _EXPIRED)
                     | DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID,         _INVALID)
                     | DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING,    _IDLE)
                     | DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE,       _ENABLED));
            break;
        case GR_CONTROL_DESTROY:
        case GR_CONTROL_UPDATE:
            break;
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }
    return (RM_OK);
}


//
// nvHalGrAlloc
//
RM_STATUS
nvHalGrAlloc_NV04(VOID *arg)
{
    PGRALLOCARG_000 pGrAllocArg = (PGRALLOCARG_000)arg;

    //
    // Verify interface revision.
    //
    if (pGrAllocArg->id != GR_ALLOC_000)
        return (RM_ERR_VERSION_MISMATCH);

    //
    // Initialize instance memory.
    //
    _nvHalGrInitInstance_NV04(pGrAllocArg->pHalHwInfo, pGrAllocArg->instance, pGrAllocArg->classNum);

    return (RM_OK);
}

//
// nvHalGrFree
//
RM_STATUS
nvHalGrFree_NV04(VOID *arg)
{
    PGRFREEARG_000 pGrFreeArg = (PGRFREEARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PGRHALINFO pGrHalInfo;
    PGRHALINFO_NV04 pGrHalPvtInfo;
    PGRAPHICSCHANNEL_NV04 pGrChannel;
#ifdef NOTYET
    U032 classNum;
#endif
    U032 i;

    //
    // Verify interface revision.
    //
    if (pGrFreeArg->id != GR_FREE_000)
        return (RM_ERR_VERSION_MISMATCH);

    if (pGrFreeArg->chid > NUM_FIFOS_NV04)
        return (RM_ERR_BAD_ARGUMENT);

    pHalHwInfo = pGrFreeArg->pHalHwInfo;
    pGrHalInfo = pHalHwInfo->pGrHalInfo;
    pGrHalPvtInfo = (PGRHALINFO_NV04)pHalHwInfo->pGrHalPvtInfo;
    pGrChannel = &pGrHalPvtInfo->grChannels[pGrFreeArg->chid];

#ifdef NOTYET
    //
    // Get class out of instance memory.
    //
    classNum = DRF_VAL(_PGRAPH, _CTX_SWITCH1, _GRCLASS, INST_RD32(pGrFreeArg->instance, SF_OFFSET(NV_PRAMIN_CONTEXT_0)));
#endif

    //
    // Zero the out the object context for good measure.
    //
    for (i = 0; i < 3; i++)
    {
        INST_WR32(pGrFreeArg->instance, i, 0);
    }

    return (RM_OK);
}

//
// nvHalGrGetExceptionData
//
// Retrieve graphics engine exception data.
//
RM_STATUS
nvHalGrGetExceptionData_NV04(VOID *arg)
{
    PGRGETEXCEPTIONDATAARG_000 pGrGetExceptionDataArg = (PGRGETEXCEPTIONDATAARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PGREXCEPTIONDATA pGrExceptionData;

    //
    // Verify interface revision.
    //
    if (pGrGetExceptionDataArg->id != GR_GET_EXCEPTION_DATA_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrGetExceptionDataArg->pHalHwInfo;
    pGrExceptionData = pGrGetExceptionDataArg->pExceptionData;

    //
    // Fill in exception data.
    //
    pGrExceptionData->Offset = REG_RD32(NV_PGRAPH_TRAPPED_ADDR);
    pGrExceptionData->ChID = DRF_VAL(_PGRAPH, _TRAPPED_ADDR, _CHID, pGrExceptionData->Offset);
    pGrExceptionData->NotifyInstance = REG_RD_DRF(_PGRAPH, _CTX_SWITCH2,
                                        _NOTIFY_INSTANCE);
    pGrExceptionData->classNum = REG_RD_DRF(_PGRAPH, _CTX_SWITCH1, _GRCLASS);
    pGrExceptionData->Data = REG_RD32(NV_PGRAPH_TRAPPED_DATA);
    pGrExceptionData->Instance = REG_RD32(NV_PGRAPH_CTX_SWITCH4);
    pGrExceptionData->Offset &= (DRF_MASK(NV_PGRAPH_TRAPPED_ADDR_MTHD) << DRF_SHIFT(NV_PGRAPH_TRAPPED_ADDR_MTHD));
    pGrExceptionData->Nsource = REG_RD32(NV_PGRAPH_NSOURCE);
    pGrExceptionData->MethodStatus = RM_OK;

    return (RM_OK);
}

//
// nvHalGrService
//
// Handle selected graphics object exceptions.
//
RM_STATUS
nvHalGrService_NV04(VOID *arg)
{
    PGRSERVICEARG_000 pGrServiceArg = (PGRSERVICEARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PGRHALINFO_NV04 pGrHalPvtInfo;
    PGREXCEPTIONDATA pGrExceptionData;
    U032 i;

    //
    // Verify interface revision.
    //
    if (pGrServiceArg->id != GR_SERVICE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrServiceArg->pHalHwInfo;
    pGrExceptionData = pGrServiceArg->pExceptionData;
    pGrHalPvtInfo = (PGRHALINFO_NV04)pHalHwInfo->pGrHalPvtInfo;

    //
    // Handle any exceptions...
    //
    if (pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _CONTEXT_SWITCH, _PENDING))
    {
        //
        // Reset condition.
        //
        REG_WR_DRF_DEF(_PGRAPH, _INTR, _CONTEXT_SWITCH, _RESET);

        if (IsNV4_NV04(pHalHwInfo->pMcHalInfo))
        {
            //*****  workaround for NV4 MP DMA channel change HW bug -- jsw...
            // According to Rick I. this bug was fixed both for nv5/nv10
            PFIFOHALINFO_NV04 pFifoHalInfo;
            U032 currentCh, get, put, pushPending;
            BOOL isDma, isInUse;

            pFifoHalInfo = (PFIFOHALINFO_NV04)pHalHwInfo->pFifoHalInfo;
            // determine current channel
            currentCh = REG_RD_DRF(_PFIFO, _CACHE1_PUSH1, _CHID); 

            // analyze all DMA channels other than the current channel that are in use 
            for (i = 0; i < NUM_FIFOS_NV04; i++)
            {
                isDma = REG_RD32(NV_PFIFO_MODE) & (1 << i);
                isInUse = pFifoHalInfo->InUse & (1 << i);
                if (isInUse && isDma && i != currentCh)
                {
                    // look for a channel where (put != get) yet is not flagged as pending
                    put = REG_RD32(0x714000 + i * 32);
                    get = REG_RD32(0x714000 + i * 32 + 4);
                    pushPending = REG_RD32(NV_PFIFO_DMA);
                    if (put != get && !(pushPending & (1 << i)))
                    {
                        pushPending |= (1 << i);
                        REG_WR32(NV_PFIFO_DMA, pushPending);
                    }
                }
            }
        }
        //***** ...jsw

        //
        // Wait for idle
        //
        GR_DONE();
    
        //
        // Graphics channel switch.
        //
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Graphics context switch exception.\n\r");
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Previous graphics channel:", pGrHalPvtInfo->currentChID);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: New graphics channel:", pGrExceptionData->ChID);

        (void) nvHalGrLoadChannelContext_NV04(pHalHwInfo, pGrExceptionData->ChID);
    }

    //
    // Handle methods in software.
    //
    if (pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _MISSING_HW, _PENDING))
    {
        if (nvHalGrClassSoftwareMethod_NV04(pHalHwInfo, pGrExceptionData) == RM_OK)
        {
            // Reset condition.
            REG_WR_DRF_DEF(_PGRAPH, _INTR, _MISSING_HW, _RESET);
        }   
    }

    if (pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _PENDING))
    {
        if (pGrExceptionData->Nsource & DRF_DEF(_PGRAPH, _NSOURCE, _ILLEGAL_MTHD, _PENDING))
        {
            if (nvHalGrClassSoftwareMethod_NV04(pHalHwInfo, pGrExceptionData) == RM_OK)
            {
                // Reset condition.
                REG_WR_DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _RESET);
            }
        }

        //
        // Handle context color key workaround if necessary.
        //
        if (pGrExceptionData->Nsource & DRF_DEF(_PGRAPH, _NSOURCE, _DATA_ERROR, _PENDING))
        {
            if (_nvHalGrSetContextColorKeyWar_NV04(pHalHwInfo, pGrExceptionData) == RM_OK)
            {
                // Reset condition.
                REG_WR_DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _RESET);
            }
        }
    }

    return (RM_OK);
}

RM_STATUS
nvHalGrGetNotifyData_NV04(VOID *arg)
{
    PGRGETNOTIFYDATAARG_000 pGrGetNotifyDataArg = (PGRGETNOTIFYDATAARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PGRNOTIFYDATA pGrNotifyData;

    //
    // Verify interface revision.
    //
    if (pGrGetNotifyDataArg->id != GR_GET_NOTIFY_DATA_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrGetNotifyDataArg->pHalHwInfo;
    pGrNotifyData = pGrGetNotifyDataArg->pGrNotifyData;

    if (REG_RD32(NV_PGRAPH_NOTIFY) &
        (DRF_DEF(_PGRAPH, _NOTIFY, _REQ, _PENDING) |
         DRF_DEF(_PGRAPH, _NOTIFY, _STYLE, _WRITE_THEN_AWAKEN)))
    {
        pGrNotifyData->trigger = TRUE;
        pGrNotifyData->action =  REG_RD_DRF(_PGRAPH, _NOTIFY, _STYLE);
        // clear the notify condition
        FLD_WR_DRF_DEF(_PGRAPH, _NOTIFY, _REQ, _NOT_PENDING);
    } else {
        pGrNotifyData->trigger = FALSE;
        pGrNotifyData->action = 0;
    }

    return (RM_OK);
}

RM_STATUS
nvHalGrSetObjectContext_NV04(VOID *arg)
{
    PGRSETOBJECTCONTEXTARG_000 pGrSetObjectContextArg = (PGRSETOBJECTCONTEXTARG_000)arg;
    PHALHWINFO pHalHwInfo;
    U032 context, instance;
    U032 fifo;
    U032 i;

    //
    // Verify interface revision.
    //
    if (pGrSetObjectContextArg->id != GR_SET_OBJECT_CONTEXT_000)
        return (RM_ERR_VERSION_MISMATCH);

    //
    // Verify chid.
    //
    if (pGrSetObjectContextArg->chid > NUM_FIFOS_NV04)
        return (RM_ERR_BAD_ARGUMENT);

    //
    // Verify instance (what else can we do here without
    // keeping track of all objects that pass through the
    // nvHalGrAlloc interface down in the HAL?).
    //
    if (pGrSetObjectContextArg->instance == 0)
        return (RM_ERR_BAD_ARGUMENT);

    pHalHwInfo = pGrSetObjectContextArg->pHalHwInfo;
    instance = pGrSetObjectContextArg->instance;
    
    //
    // Fetch current context.
    //
    context = INST_RD32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_0));

    //
    // The context surface valid bit is managed by sw on nv4, so
    // be sure it's clear when updating object context.
    //
    // Bits 31:29 in NV_PGRAPH_DEBUG_3 indicate if SetContext is being
    // handled by hardware.
    //
    if (IsNV5orBetter_NV04(pHalHwInfo->pMcHalInfo) && REG_RD32(NV_PGRAPH_DEBUG_3) & 0xe0000000)
    {
        // Context_Surface is managed by SW.
        context &= ~(DRF_DEF(_PGRAPH, _CTX_SWITCH1, _CONTEXT_SURFACE, _VALID));
    }

    //
    // Make necessary adjustments.
    //
    context &= pGrSetObjectContextArg->andMask;
    context |= pGrSetObjectContextArg->orMask;

    //
    // Write it back out                   
    //
    INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_0), context);
        
    //
    // Now we need to update the hardware context if appropriate.
    //
    if (pGrSetObjectContextArg->chid == REG_RD_DRF(_PGRAPH, _CTX_USER, _CHID))
    {
        //
        // First disable the graphics fifo so we can get to the context rams
        //
        fifo = REG_RD32(NV_PGRAPH_FIFO);
        REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED);
            
        //
        // Walk to subchannels and see if we find a match; update when we do
        //
        for (i = 0 ; i < 8 ; i++)
        {            
            if (REG_RD32(NV_PGRAPH_CTX_CACHE4(i)) == instance)
            {   
                //
                // Get the current settings
                //
                context = REG_RD32(NV_PGRAPH_CTX_CACHE1(i));

                //
                // The context surface valid bit is managed by sw on nv4, so
                // be sure it's clear when updating object context.
                //
                // Bits 31:29 in NV_PGRAPH_DEBUG_3 indicate if SetContext is being
                // handled by hardware.
                //
                if (IsNV5orBetter_NV04(pHalHwInfo->pMcHalInfo) && REG_RD32(NV_PGRAPH_DEBUG_3) & 0xe0000000)
                {
                    // Context_Surface is managed by SW.
                    context &= ~(DRF_DEF(_PGRAPH, _CTX_SWITCH1, _CONTEXT_SURFACE, _VALID));
                }

                //
                // Make necessary adjustments.
                //
                context &= pGrSetObjectContextArg->andMask;
                context |= pGrSetObjectContextArg->orMask;

                //
                // Write it back out                   
                //
                REG_WR32(NV_PGRAPH_CTX_CACHE1(i), context);
            }
        }                                    
                             
        //
        // See if the currently active subchannel also needs updating
        //
        if (REG_RD_DRF(_PGRAPH, _CTX_SWITCH4, _USER_INSTANCE) == instance)
        {   
            //
            // Get the current settings
            //
            context = REG_RD32(NV_PGRAPH_CTX_SWITCH1);

            //
            // The context surface valid bit is managed by sw on nv4, so
            // be sure it's clear when updating object context.
            //
            // Bits 31:29 in NV_PGRAPH_DEBUG_3 indicate if SetContext is being
            // handled by hardware.
            //
            if (IsNV5orBetter_NV04(pHalHwInfo->pMcHalInfo) && REG_RD32(NV_PGRAPH_DEBUG_3) & 0xe0000000)
            {
                // Context_Surface is managed by SW.
                context &= ~(DRF_DEF(_PGRAPH, _CTX_SWITCH1, _CONTEXT_SURFACE, _VALID));
            }

            //
            // Make necessary adjustments.
            //
            context &= pGrSetObjectContextArg->andMask;
            context |= pGrSetObjectContextArg->orMask;

            //
            // Write it back out                   
            //
            REG_WR32(NV_PGRAPH_CTX_SWITCH1, context);
        }
            
        //
        // Restore the fifo state
        //
        REG_WR32(NV_PGRAPH_FIFO, fifo);
        
    }    
    return (RM_OK);
}

RM_STATUS
nvHalGrLoadOverride_NV04(VOID *arg)
{
    PGRLOADOVERRIDEARG_000 pGrLoadOverrideArg = (PGRLOADOVERRIDEARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PGRHALINFO_NV04 pGrHalPvtInfo;

    //
    // Verify interface revision.
    //
    if (pGrLoadOverrideArg->id != GR_LOAD_OVERRIDE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrLoadOverrideArg->pHalHwInfo;
    pGrHalPvtInfo = (PGRHALINFO_NV04)pHalHwInfo->pGrHalPvtInfo;

    switch (pGrLoadOverrideArg->cmd)
    {
        case GR_LOAD_OVERRIDE_DEBUG0:
            pGrHalPvtInfo->Debug0 = pGrLoadOverrideArg->value;
            break;
        case GR_LOAD_OVERRIDE_DEBUG1:
            pGrHalPvtInfo->Debug1 = pGrLoadOverrideArg->value;
            break;
        case GR_LOAD_OVERRIDE_DEBUG2:
            pGrHalPvtInfo->Debug2 = pGrLoadOverrideArg->value;
            break;
        case GR_LOAD_OVERRIDE_DEBUG3:
            pGrHalPvtInfo->Debug3 = pGrLoadOverrideArg->value;
            break;
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }

    return (RM_OK);
}

//
// nvHalGrSetContextColorKeyWar
//
// This routine implements the workaround for the nv5 context color
// key bug.  On nv5, the NV01_CONTEXT_COLOR_KEY class is not accepted
// by classes with the SetContextColorKey method (a _DATA_ERROR
// exception is raised).
//
// If that's the case here, we munge the PGREXCEPTIONDATA structure
// to make it look like a legal method, and allow the resource manager
// to field it accordingly.
//
static RM_STATUS
_nvHalGrSetContextColorKeyWar_NV04(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)
{
    U032 grClass;

    //
    // This WAR applies to NV5.
    //
    if (!(IsNV5orBetter_NV04(pHalHwInfo->pMcHalInfo) && REG_RD32(NV_PGRAPH_DEBUG_3) & 0xe0000000) ||
        pGrExceptionData->Instance == 0)
    {
        //
        // This isn't really an error, but by returning RM_ERROR
        // we're going to avoid clearing the pending interrupt
        // condition.
        //
        return RM_ERROR;
    }

    //
    // First determine if this is a class that requires this war.
    // The following classes supported on NV5 accept a
    // SetContextColorKey argument=NV01_CONTEXT_COLOR_KEY:
    //  NV01_IMAGE_BLIT
    //  NV01_IMAGE_FROM_CPU
    //  NV03_STRETCHED_IMAGE_FROM_CPU
    //
    if (((pGrExceptionData->classNum != NV01_IMAGE_BLIT) ||
         (pGrExceptionData->Offset != NV01F_SET_CONTEXT_COLOR_KEY)) &&
        ((pGrExceptionData->classNum != NV01_IMAGE_FROM_CPU) ||
         (pGrExceptionData->Offset != NV021_SET_CONTEXT_COLOR_KEY)) &&
        ((pGrExceptionData->classNum != NV03_STRETCHED_IMAGE_FROM_CPU) ||
         (pGrExceptionData->Offset != NV036_SET_CONTEXT_COLOR_KEY)))
    {
        return RM_ERROR;
    }

    //
    // Now check to see if Data is the instance pointer to an
    // NV01_CONTEXT_COLOR_KEY object.
    //
    grClass = INST_RD32(pGrExceptionData->Data, SF_OFFSET(NV_PRAMIN_CONTEXT_0)) & DRF_MASK(NV_PGRAPH_CTX_SWITCH1_GRCLASS);
    if (grClass == NV01_CONTEXT_COLOR_KEY)
    {
        //
        // Mark the NotifySource as an _ILLEGAL_METHOD, which will cause
        // the RM to handle it in software (which will turn on the
        // CHROMA_ENABLE bit and refresh the PATCH_STATUS bit).
        //
        pGrExceptionData->Nsource &= ~DRF_DEF(_PGRAPH, _NSOURCE, _DATA_ERROR, _PENDING);
        pGrExceptionData->Nsource |= DRF_DEF(_PGRAPH, _NSOURCE, _ILLEGAL_MTHD, _PENDING);
        return RM_OK;
    }

    return RM_ERROR;
} 

static RM_STATUS
_nvHalGrInitInstance_NV04(
    PHALHWINFO  pHalHwInfo,
    U032        instance,
    U032        classNum
)
{
    U032 ctx1, ctx2, ctx3;

    // default _CTX_SWITCH1 value
    ctx1 = DRF_NUM(_PGRAPH, _CTX_SWITCH1, _GRCLASS, classNum)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_CONFIG, _SRCCOPY_AND)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _INVALID);    

    // default _CTX_SWITCH2 value
    ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _INVALID)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);

    // default _CTX_SWITCH3 value
    ctx3 = DRF_DEF(_PGRAPH, _CTX_SWITCH3, _DMA_INSTANCE_0, _INVALID)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH3, _DMA_INSTANCE_1, _INVALID);

    switch (classNum)
    {
        case NV01_CONTEXT_BETA:
        case NV01_CONTEXT_CLIP_RECTANGLE:
        case NV01_IMAGE_BLIT:
        case NV03_CONTEXT_ROP:
        case NV04_CONTEXT_SURFACES_2D:
        case NV03_DX3_TEXTURED_TRIANGLE:
        case NV04_CONTEXT_SURFACE_SWIZZLED:
        case NV04_CONTEXT_SURFACES_3D:
        case NV03_CONTEXT_SURFACE_2D_DESTINATION:
        case NV03_CONTEXT_SURFACE_2D_SOURCE:
        case NV03_CONTEXT_SURFACE_3D_COLOR:
        case NV03_CONTEXT_SURFACE_3D_DEPTH:
        case NV04_IMAGE_BLIT:
        case NV04_CONTEXT_BETA:
            // defaults are fine
            break;
        case NV01_CONTEXT_COLOR_KEY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16A8Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV01_CONTEXT_PATTERN:
        case NV04_CONTEXT_PATTERN:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _CGA6_M1)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16A8Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV01_RENDER_SOLID_LIN:
        case NV01_RENDER_SOLID_TRIANGLE:
        case NV01_RENDER_SOLID_RECTANGLE:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X24Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV01_IMAGE_FROM_CPU:
        case NV03_STRETCHED_IMAGE_FROM_CPU:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV1_NULL:
            //
            // The NULL object is really a pseudo-dma object, so it needs a valid
            // limit value and should also have the PAGE_TABLE_PRESENT bit set.
            //
            ctx1 |= SF_DEF(_DMA, _PAGE_TABLE, _PRESENT);
            ctx2 = 0xffffffff;
            //INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_1), 0xFFFFFFFF);
            //FLD_WR_ISF_DEF(instance, _DMA, _PAGE_TABLE, _PRESENT);
            break;
        case NV03_SCALED_IMAGE_FROM_MEMORY:
        case NV04_SCALED_IMAGE_FROM_MEMORY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A1R5G5B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_DVD_SUBPICTURE:
            //
            // DVD class is patchless, so it is always considered valid.  HW doesn't
            // use this bit, but let's set it anyway to be consistent with other
            // objects
            //
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _VALID);
            break;
        case NV03_MEMORY_TO_MEMORY_FORMAT:
            // Note that this object is always considered patch valid
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _VALID);
            break;
        case NV04_GDI_RECTANGLE_TEXT:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _CGA6_M1)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV03_GDI_RECTANGLE_TEXT:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _CGA6_M1)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X24Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_DX5_TEXTURED_TRIANGLE:
            //
            // DX5 class is patchless, so it is always considered valid.  HW doesn't
            // use this bit, but let's set it anyway to be consistent with other
            // objects
            //
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _VALID);
            break;
        case NV04_DX6_MULTI_TEXTURE_TRIANGLE:
            //
            // DX6 class is patchless, so it is always considered valid.  HW doesn't
            // use this bit, but let's set it anyway to be consistent with other
            // objects
            //
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _VALID);
            break;
        case NV04_CONTEXT_COLOR_KEY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A16R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_RENDER_SOLID_LIN:
        case NV04_RENDER_SOLID_TRIANGLE:
        case NV04_RENDER_SOLID_RECTANGLE:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_INDEXED_IMAGE_FROM_CPU:
        case NV04_IMAGE_FROM_CPU:
        case NV04_STRETCHED_IMAGE_FROM_CPU:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV05_SCALED_IMAGE_FROM_MEMORY:
            //
            // NOTE: Eventhough we are creating an object of class NV5_SCALED_IMAGE_FROM_CPU,
            //       we have to tell the hardware we are creating the NV4 object. The hardware
            //       does not know the new object class. The only difference between the NV5 and NV4
            //       objects is a new method to set the dithering that is valid for an NV5 object.
            //       The hardware will generate an illegal method interrupt for that method and is 
            //       handled by the RM.
            //
            ctx1 = DRF_NUM(_PGRAPH, _CTX_SWITCH1, _GRCLASS, NV04_SCALED_IMAGE_FROM_MEMORY)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_CONFIG, _SRCCOPY_AND)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _INVALID);    

            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A1R5G5B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV05_INDEXED_IMAGE_FROM_CPU:
            //
            // NOTE: Eventhough we are creating an object of class NV05_INDEXED_IMAGE_FROM_CPU,
            //       we have to tell the hardware we are creating the NV4 object. The hardware
            //       does not know the new object class. The only difference between the NV5 and NV4
            //       objects is a new method to set the dithering that is valid for an NV5 object.
            //       The hardware will generate an illegal method interrupt for that method and is 
            //       handled by the RM.
            //
            ctx1 = DRF_NUM(_PGRAPH, _CTX_SWITCH1, _GRCLASS, NV04_INDEXED_IMAGE_FROM_CPU)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_CONFIG, _SRCCOPY_AND)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _INVALID);

            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV05_IMAGE_FROM_CPU:
            //
            // NOTE: Eventhough we are creating an object of class NV05_IMAGE_FROM_CPU,
            //       we have to tell the hardware we are creating the NV4 object. The hardware
            //       does not know the new object class. The only difference between the NV5 and NV4
            //       objects is a new method to set the dithering that is valid for an NV5 object.
            //       The hardware will generate an illegal method interrupt for that method and is 
            //       handled by the RM.
            //
            ctx1 = DRF_NUM(_PGRAPH, _CTX_SWITCH1, _GRCLASS, NV04_IMAGE_FROM_CPU)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_CONFIG, _SRCCOPY_AND)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _INVALID);
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV05_STRETCHED_IMAGE_FROM_CPU:
            //
            // NOTE: Eventhough we are creating an object of class NV05_STRETCHED_IMAGE_FROM_CPU,
            //       we have to tell the hardware we are creating the NV4 object. The hardware
            //       does not know the new object class. The only difference between the NV5 and NV4
            //       objects is a new method to set the dithering that is valid for an NV5 object.
            //       The hardware will generate an illegal method interrupt for that method and is 
            //       handled by the RM.
            //
            ctx1 = DRF_NUM(_PGRAPH, _CTX_SWITCH1, _GRCLASS, NV04_STRETCHED_IMAGE_FROM_CPU)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_CONFIG, _SRCCOPY_AND)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _INVALID);
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        default:
            return (RM_ERR_CREATE_BAD_CLASS);
    }

    //
    // Write out the values.
    //
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: instance data for classNum ", classNum);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "      instance offset ", instance);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "      ctx1 ", ctx1);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "      ctx2 ", ctx2);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "      ctx3 ", ctx3);

    INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_0), ctx1);
    INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_1), ctx2);
    INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_2), ctx3);

    return (RM_OK);
}
