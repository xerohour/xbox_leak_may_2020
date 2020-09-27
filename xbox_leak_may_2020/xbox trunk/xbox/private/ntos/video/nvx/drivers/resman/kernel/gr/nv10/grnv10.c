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
* Module: GRCHIP.C                                                          *
*   The graphics chip dependent routines are kept here.                     *
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

// forwards
RM_STATUS nvHalGrControl_NV10(VOID *);
RM_STATUS nvHalGrAlloc_NV10(VOID *);
RM_STATUS nvHalGrFree_NV10(VOID *);
RM_STATUS nvHalGrGetExceptionData_NV10(VOID *);
RM_STATUS nvHalGrService_NV10(VOID *);
RM_STATUS nvHalGrGetNotifyData_NV10(VOID *);
RM_STATUS nvHalGrSetObjectContext_NV10(VOID *);
RM_STATUS nvHalGrLoadOverride_NV10(VOID *);

// statics
static RM_STATUS _nvHalGrInitObjectContext_NV10(PHALHWINFO, U032, U032);
static VOID _nvHalGrInitCelsius_NV10(PHALHWINFO, U032);
static VOID _nvHalGrInitDx5_NV10(PHALHWINFO, U032);

// externs
extern RM_STATUS nvHalGrClassSoftwareMethod_NV10(PHALHWINFO, PGREXCEPTIONDATA);
extern RM_STATUS nvHalGrLoadChannelContext_NV10(PHALHWINFO, U032);
extern RM_STATUS nvHalGrUnloadChannelContext_NV10(PHALHWINFO, U032);

//
// nvHalGrControl
//
RM_STATUS
nvHalGrControl_NV10(VOID *arg)
{
    PGRCONTROLARG_000 pGrControlArg = (PGRCONTROLARG_000)arg;
    PHALHWINFO pHalHwInfo = pGrControlArg->pHalHwInfo;
    PGRHALINFO pGrHalInfo;
    PGRHALINFO_NV10 pGrHalPvtInfo;
    PHWREG nvAddr;
    U032 i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrControl_NV10\r\n");

    // ASSERTIONS

    //
    // Verify interface revision.
    //
    if (pGrControlArg->id != GR_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrControlArg->pHalHwInfo;
    pGrHalInfo = pHalHwInfo->pGrHalInfo;
    pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;
    
    switch (pGrControlArg->cmd)
    {
        case GR_CONTROL_INIT:
            //
            // Initialize graphics debug values.
            //
            switch (REG_RD_DRF(_PMC, _BOOT_0, _MAJOR_REVISION))
            {
                case NV_PMC_BOOT_0_MAJOR_REVISION_A:
                default:
                    pGrHalPvtInfo->Debug0 = 0x0;        // all DEBUG0 bits are RESET bits in NV10
                    pGrHalPvtInfo->Debug1 = (U032)
                        DRF_DEF(_PGRAPH, _DEBUG_1, _VOLATILE_RESET,   _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_PTE,          _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_CACHE,        _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_FILE,         _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _DRAWDIR_AUTO,     _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _DRAWDIR_Y,        _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _INSTANCE,         _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_1, _CTX,              _INIT);
                    pGrHalPvtInfo->Debug2 = (U032)
                        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_LIMIT_CHECK,    _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_LIMIT_INT,      _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_OVRFLW_INT,     _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_COALESCE_3D,    _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_COALESCE_2D,    _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_TRIEND_FLUSH,   _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_DITHER_3D,      _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_DITHER_2D,      _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_TILE3D2HIGH,    _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_SWALLOW_REQS,   _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_ALPHA_ABORT,    _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_FIXED_ADRS,     _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_DITHER_RANGE,   _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PITCH_CHECK_2D, _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PITCH_CHECK_3D, _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_TILEVIOL,          _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_DITHER_3D,         _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_DITHER_2D,         _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_EARLY_ZABORT,      _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_BLIT_MULTILINE,    _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_BLIT_DST_LIMIT,    _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_SDR_FAST_BLEND,    _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_2D_FAST_CONV,      _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_HPREQ,             _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_BITBUCKET,         _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_FIXED_ADRS,        _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_FAST_KEEP_DST,     _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_FORCE_CREAD,       _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_FORCE_ZREAD,       _INIT);

                    pGrHalPvtInfo->Debug3 = (U032)
                        DRF_DEF(_PGRAPH, _DEBUG_3, _FAST_DATA_STRTCH,      _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _FAST_3D_SHADOW_DATA,   _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _FAST_DMA_READ,         _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _IDLE_FILTER,           _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _INHIBIT_IMCLASS_BLOCK, _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _SYNCHRONIZE,           _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _D3D_STATE3D,           _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _OBJECT_RELOAD,         _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _PM_TRIGGER,            _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _ALTARCH,               _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _SINGLE_CYCLE_LOAD,     _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _BILINEAR_3D,           _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _VOLATILE_RESET,        _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _DATA_CHECK,            _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _DATA_CHECK_FAIL,       _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _FORMAT_CHECK,          _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _DMA_CHECK,             _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _STATE_CHECK,           _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _IMAGE_64BIT,           _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _CELSIUS_64BIT,         _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _STATE3D_CHECK,         _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _XFMODE_COALESCE,       _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _CTX_METHODS,           _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _OP_METHODS,            _INIT)
                        | DRF_DEF(_PGRAPH, _DEBUG_3, _IGNORE_PATCHVALID,     _INIT);

                    //
                    // Turn this bit on for nv15/nv11 to avoid problems when class07c
                    // writes NV_PGRAPH_INCREMENT_READ_3D when gfx isn't idle. The gfx
                    // engine can't be idled because it's stalled waiting for the
                    // increment trigger.
                    //
                    if (IsNV15_NV10(pHalHwInfo->pMcHalInfo) ||
                        IsNV11_NV10(pHalHwInfo->pMcHalInfo))
                        pGrHalPvtInfo->Debug3 |= DRF_DEF(_PGRAPH, _DEBUG_3, _INHIBIT_IMCLASS_BLOCK, _ENABLED);

                    pGrHalPvtInfo->Debug4 = 0x0;       // all DEBUG4 bits are SPARES currently
                    break;
            }

            //
            // NV11 seems to be faster if we don't separate color/Z with SDR memory,
            // I still need to get confirmation from Jonah on this DEBUG bit though ...
            //
            if (IsNV11_NV10(pHalHwInfo->pMcHalInfo)) {
                pGrHalPvtInfo->Debug2 |= DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_SEP_ZC_READS, _DISABLED);
            } else {
                // Separate color and Z reads on SDR memory configurations
                if (REG_RD_DRF(_PFB, _CFG, _TYPE) == NV_PFB_CFG_TYPE_SDR)
                    pGrHalPvtInfo->Debug2 |= DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_SEP_ZC_READS, _ENABLED);
                else
                    pGrHalPvtInfo->Debug2 |= DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_SEP_ZC_READS, _DISABLED);
            }

            // Also set some NV11 specific DEBUG bits (e.g. enable Z/C mix)
            if (IsNV11_NV10(pHalHwInfo->pMcHalInfo)) {
                pGrHalPvtInfo->Debug2 |= (DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_NV11_OPTS,   _INIT)
                                      |   DRF_DEF(_PGRAPH, _DEBUG_2, _ROP_ALLOW_ZCMIX, _INIT));
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
            
            REG_WR32(NV_PGRAPH_DEBUG_0, pGrHalPvtInfo->Debug0);
            REG_WR32(NV_PGRAPH_DEBUG_1, pGrHalPvtInfo->Debug1);
            REG_WR32(NV_PGRAPH_DEBUG_2, pGrHalPvtInfo->Debug2);
            REG_WR32(NV_PGRAPH_DEBUG_3, pGrHalPvtInfo->Debug3);
            REG_WR32(NV_PGRAPH_DEBUG_4, pGrHalPvtInfo->Debug4);

            // FROM NV10_NvChipRestoreTiledRanges(pDev)

            // 
            // PGRAPH_TILE values will be reset after a PGRAPH_DEBUG_0_STATE_RESET.
            // Unfortunately, it may have had values we wanted (e.g. from stateFb),
            // so after a DEBUG_0_STATE_RESET, restore the PGRAPH values from PFB.
            //
            HAL_GR_IDLE(pHalHwInfo);

            for (i = 0; i < NV_PFB_TILE__SIZE_1; i++)
            {
                REG_WR32(NV_PGRAPH_TLIMIT(i), REG_RD32(NV_PFB_TLIMIT(i)));
                REG_WR32(NV_PGRAPH_TSIZE(i), REG_RD32(NV_PFB_TSIZE(i)));
                REG_WR32(NV_PGRAPH_TILE(i), REG_RD32(NV_PFB_TILE(i)));
            }

            // FROM NV10_grStateLoadContext

            //
            // Load debug overrides.
            //
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
            (void) nvHalGrLoadChannelContext_NV10(pHalHwInfo, pGrHalPvtInfo->currentChID);
            break;
        case GR_CONTROL_UNLOAD:
            //
            // Pull out current graphics state.
            //
            (void) nvHalGrLoadChannelContext_NV10(pHalHwInfo, INVALID_CHID);

            // FROM NV10_grStateUnLoadContext
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
nvHalGrAlloc_NV10(VOID *arg)
{
    PGRALLOCARG_000 pGrAllocArg = (PGRALLOCARG_000)arg;
    PHALHWINFO pHalHwInfo = pGrAllocArg->pHalHwInfo;
    PGRHALINFO pGrHalInfo;
    PGRHALINFO_NV10 pGrHalPvtInfo;
    PGRAPHICSCHANNEL_NV10 pGrChannel;
    PHWREG nvAddr;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrAlloc_NV10\r\n");

    //
    // Verify interface revision.
    //
    if (pGrAllocArg->id != GR_ALLOC_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrAllocArg->pHalHwInfo;
    pGrHalInfo = pHalHwInfo->pGrHalInfo;
    pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;
    pGrChannel = &pGrHalPvtInfo->grChannels[pGrAllocArg->chid];
    nvAddr = pHalHwInfo->nvBaseAddr;

    //
    // If we've got a 3d object, keep note of it.
    //
    if ((pGrAllocArg->classNum == NV10_CELSIUS_PRIMITIVE) ||
        (pGrAllocArg->classNum == NV10_DX5_TEXTURED_TRIANGLE) ||
        (pGrAllocArg->classNum == NV10_DX6_MULTI_TEXTURE_TRIANGLE) ||
        (pGrAllocArg->classNum == NV15_CELSIUS_PRIMITIVE) ||
        (pGrAllocArg->classNum == NV11_CELSIUS_PRIMITIVE))
    {
        U032 currClass = 0;

        if (pGrHalPvtInfo->currentObjects3d[pGrAllocArg->chid] != 0)
            currClass = DRF_VAL(_PGRAPH, _CTX_SWITCH1, _GRCLASS, INST_RD32(pGrHalPvtInfo->currentObjects3d[pGrAllocArg->chid], SF_OFFSET(NV_PRAMIN_CONTEXT_0)));

        //
        // For DX5/DX6, we're not able to handle a Celsius object in the same channel,
        // since their init graphics routines are different. Check that a Celsius 3D
        // object hasn't already been created (multiple DX5/DX6 are OK).
        //
        if ((pGrAllocArg->classNum == NV10_DX6_MULTI_TEXTURE_TRIANGLE ||
             pGrAllocArg->classNum == NV10_DX5_TEXTURED_TRIANGLE) &&
            (currClass == NV10_CELSIUS_PRIMITIVE ||
             currClass == NV15_CELSIUS_PRIMITIVE ||
             currClass == NV11_CELSIUS_PRIMITIVE))
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: 3d state in use by celsius instance ", pGrHalPvtInfo->currentObjects3d[pGrAllocArg->chid]);
            return (RM_ERR_ILLEGAL_OBJECT);
        }

        //
        // For Celsius, we're not able to handle either DX5/DX6 or another Celsius
        // object in the same channel, since their init graphics routines (in gr\nv10 dir)
        // are different. Check that a current 3D object hasn't already been created.
        //
        if ((pGrAllocArg->classNum == NV10_CELSIUS_PRIMITIVE ||
             pGrAllocArg->classNum == NV15_CELSIUS_PRIMITIVE ||
             pGrAllocArg->classNum == NV11_CELSIUS_PRIMITIVE) &&
            (currClass == NV10_DX6_MULTI_TEXTURE_TRIANGLE ||
             currClass == NV10_DX5_TEXTURED_TRIANGLE))
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: 3d state in use by DX instance ", pGrHalPvtInfo->currentObjects3d[pGrAllocArg->chid]);
            return (RM_ERR_ILLEGAL_OBJECT);
        }

        pGrHalPvtInfo->currentObjects3d[pGrAllocArg->chid] = pGrAllocArg->instance;

        if ((pGrAllocArg->classNum == NV10_CELSIUS_PRIMITIVE) ||
            (pGrAllocArg->classNum == NV15_CELSIUS_PRIMITIVE) ||
            (pGrAllocArg->classNum == NV11_CELSIUS_PRIMITIVE))
            _nvHalGrInitCelsius_NV10(pHalHwInfo, pGrAllocArg->chid);
        else
            _nvHalGrInitDx5_NV10(pHalHwInfo, pGrAllocArg->chid);
    }

    //
    // Initialize context.
    //
    _nvHalGrInitObjectContext_NV10(pHalHwInfo, pGrAllocArg->instance, pGrAllocArg->classNum);

    return (RM_OK);
}

//
// nvHalGrFree
//
RM_STATUS
nvHalGrFree_NV10(VOID *arg)
{
    PGRFREEARG_000 pGrFreeArg = (PGRFREEARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PGRHALINFO pGrHalInfo;
    PGRHALINFO_NV10 pGrHalPvtInfo;
    PGRAPHICSCHANNEL_NV10 pGrChannel;
    PHWREG nvAddr;
    U032 i;

    //
    // Verify interface revision.
    //
    if (pGrFreeArg->id != GR_FREE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrFreeArg->pHalHwInfo;
    pGrHalInfo = pHalHwInfo->pGrHalInfo;
    pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;
    pGrChannel = &pGrHalPvtInfo->grChannels[pGrFreeArg->chid];
    nvAddr = pHalHwInfo->nvBaseAddr;

    //
    // Clear 3d current object pointer if necessary.
    //
    if (pGrFreeArg->instance == pGrHalPvtInfo->currentObjects3d[pGrFreeArg->chid])
    {
        // we could ASSERT that this is a 3d object here as well
        pGrHalPvtInfo->currentObjects3d[pGrFreeArg->chid] = 0;

        // clear the Celsius bit (either never had one or it was just freed)
        pGrHalInfo->has3dObject &= ~(0x1 << pGrFreeArg->chid);
    }

    //
    // Zero out the object context for good measure.
    //
    for (i = 0; i < 4; i++)
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
nvHalGrGetExceptionData_NV10(VOID *arg)
{
    PGRGETEXCEPTIONDATAARG_000 pGrGetExceptionDataArg = (PGRGETEXCEPTIONDATAARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PGREXCEPTIONDATA pGrExceptionData;
    PHWREG nvAddr;

    //
    // Verify interface revision.
    //
    if (pGrGetExceptionDataArg->id != GR_GET_EXCEPTION_DATA_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrGetExceptionDataArg->pHalHwInfo;
    pGrExceptionData = pGrGetExceptionDataArg->pExceptionData;
    nvAddr = pHalHwInfo->nvBaseAddr;

    //
    // Fill in exception data.
    //
    pGrExceptionData->Offset = REG_RD32(NV_PGRAPH_TRAPPED_ADDR);
    pGrExceptionData->ChID = DRF_VAL(_PGRAPH, _TRAPPED_ADDR, _CHID, pGrExceptionData->Offset);
    pGrExceptionData->NotifyInstance = REG_RD_DRF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE);
    pGrExceptionData->classNum = REG_RD_DRF(_PGRAPH, _CTX_SWITCH1, _GRCLASS);
    pGrExceptionData->Data = REG_RD32(NV_PGRAPH_TRAPPED_DATA_LOW); 
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
nvHalGrService_NV10(VOID *arg)
{
    PGRSERVICEARG_000 pGrServiceArg = (PGRSERVICEARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PGRHALINFO_NV10 pGrHalPvtInfo;
    PGREXCEPTIONDATA pGrExceptionData;
    PHWREG nvAddr;

    //
    // Verify interface revision.
    //
    if (pGrServiceArg->id != GR_SERVICE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrServiceArg->pHalHwInfo;
    pGrExceptionData = pGrServiceArg->pExceptionData;
    pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;

    //
    // Make sure object is valid.
    //
    //if (pGrExceptionData->instance == 0)
    //    return (NV_HAL_ERROR_INVALID_STATE);

    //
    // Handle any exceptions...
    //
    if (pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _SINGLE_STEP, _PENDING))
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "SingleStep method: ", REG_RD32(NV_PGRAPH_TRAPPED_ADDR));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "SingleStep data  : ", REG_RD32(NV_PGRAPH_TRAPPED_DATA_LOW));

        // turn on the single step bit for this channel
        REG_WR_DRF_DEF(_PGRAPH, _INTR, _SINGLE_STEP, _RESET);
        DBG_BREAKPOINT();
    }

    if (pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _CONTEXT_SWITCH, _PENDING))
    {
        //
        // Reset condition.
        //
        REG_WR_DRF_DEF(_PGRAPH, _INTR, _CONTEXT_SWITCH, _RESET);

        //
        // Wait for idle
        //
        GR_DONE();
    
        //
        // Graphics channel switch.
        //
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Graphics context switch exception.\n\r");
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Previous graphics channel:",pGrHalPvtInfo->currentChID);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: New graphics channel:", pGrExceptionData->ChID);

        (void) nvHalGrLoadChannelContext_NV10(pHalHwInfo, pGrExceptionData->ChID);
    }

    //
    // Handle methods in software.
    //
    if (pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _MISSING_HW, _PENDING))
    {
        if (nvHalGrClassSoftwareMethod_NV10(pHalHwInfo, pGrExceptionData) == RM_OK)
        {
            // Reset condition.
            REG_WR_DRF_DEF(_PGRAPH, _INTR, _MISSING_HW, _RESET);
        }
    }

    if ((pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _PENDING)) ||
        (pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _ERROR, _PENDING)))
    {
        if (pGrExceptionData->Nsource & DRF_DEF(_PGRAPH, _NSOURCE, _ILLEGAL_MTHD, _PENDING))
        {
            if (nvHalGrClassSoftwareMethod_NV10(pHalHwInfo, pGrExceptionData) == RM_OK)
            {
                // Reset condition.
                if (pGrServiceArg->intr & DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _PENDING))
                    REG_WR_DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _RESET);
                else
                    REG_WR_DRF_DEF(_PGRAPH, _INTR, _ERROR, _RESET);
            }
        }
    }

    return (RM_OK);
}

//
// nvHalGrNotifyData
//
// Fill in notify data (trigger status and action).
//
RM_STATUS
nvHalGrGetNotifyData_NV10(VOID *arg)
{
    PGRGETNOTIFYDATAARG_000 pGrGetNotifyDataArg = (PGRGETNOTIFYDATAARG_000)arg;
    PHALHWINFO pHalHwInfo = pGrGetNotifyDataArg->pHalHwInfo;
    PGRNOTIFYDATA pGrNotifyData;
    PHWREG nvAddr;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrGetNotifyData_NV10\r\n");

    //
    // Verify interface revision.
    //
    if (pGrGetNotifyDataArg->id != GR_GET_NOTIFY_DATA_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrGetNotifyDataArg->pHalHwInfo;
    pGrNotifyData = pGrGetNotifyDataArg->pGrNotifyData;
    nvAddr = pHalHwInfo->nvBaseAddr;

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

//
// nvHalGrSetObjectContext
//
// Update various graphics object context bits.
// On NV10, these bits are managed by hardware so this
// routine does some error checking and returns.
//
RM_STATUS
nvHalGrSetObjectContext_NV10(VOID *arg)
{
    PGRSETOBJECTCONTEXTARG_000 pGrSetObjectContextArg = (PGRSETOBJECTCONTEXTARG_000)arg;
    PHALHWINFO pHalHwInfo = pGrSetObjectContextArg->pHalHwInfo;
    PHWREG nvAddr;
    U032 context, instance;
    U032 fifo;
    U032 i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrSetObjectContext_NV10\r\n");

    //
    // Verify interface revision.
    //
    if (pGrSetObjectContextArg->id != GR_SET_OBJECT_CONTEXT_000)
        return (RM_ERR_VERSION_MISMATCH);

    //
    // Verify chid.
    //
    if (pGrSetObjectContextArg->chid > NUM_FIFOS_NV10)
        return (RM_ERR_BAD_ARGUMENT);

    //
    // Verify instance (what else can we do here without
    // keeping track of all objects that pass through the
    // nvHalGrAlloc interface down in the HAL?).
    //
    if (pGrSetObjectContextArg->instance == 0)
        return (RM_ERR_BAD_ARGUMENT);

    pHalHwInfo = pGrSetObjectContextArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;
    instance = pGrSetObjectContextArg->instance;
    
    //
    // Fetch current context.
    //
    context = INST_RD32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_0));

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
nvHalGrLoadOverride_NV10(VOID *arg)
{
    PGRLOADOVERRIDEARG_000 pGrLoadOverrideArg = (PGRLOADOVERRIDEARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PGRHALINFO_NV10 pGrHalPvtInfo;
    PHWREG nvAddr;

    //
    // Verify interface revision.
    //
    if (pGrLoadOverrideArg->id != GR_LOAD_OVERRIDE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pGrLoadOverrideArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;
    pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;

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
        case GR_LOAD_OVERRIDE_SWATHCTL:
            pGrHalPvtInfo->SwathControl = pGrLoadOverrideArg->value;
            break;
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }

    return (RM_OK);
}



static RM_STATUS
_nvHalGrInitObjectContext_NV10(
    PHALHWINFO  pHalHwInfo,
    U032        instance,
    U032        classNum
)
{
    U032 ctx1, ctx2, ctx3;
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;

    // default _CTX_SWITCH1 value
    ctx1 = DRF_NUM(_PGRAPH, _CTX_SWITCH1, _GRCLASS, classNum)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_CONFIG, _SRCCOPY_AND)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _INVALID);    

    // turn on big endian for this class if supported and we're in big endian mode
    if (pHalHwInfo->pMcHalInfo->EndianControl & MC_ENDIAN_CONTROL_CHIP_BIG_ENDIAN)
        ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _ENDIAN_MODE, _BIG);

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
        case NV10_CONTEXT_SURFACES_2D:
        case NV10_CONTEXT_SURFACES_3D:
        case NV15_IMAGE_BLIT:
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
        case NV10_SCALED_IMAGE_FROM_MEMORY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A1R5G5B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV11_SCALED_IMAGE_FROM_MEMORY:
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _CLASS_TYPE, _PERFORMANCE);
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A1R5G5B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_DVD_SUBPICTURE:
        case NV10_DVD_SUBPICTURE:
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
        case NV10_DX5_TEXTURED_TRIANGLE:
            //
            // DX5 class is patchless, so it is always considered valid.  HW doesn't
            // use this bit, but let's set it anyway to be consistent with other
            // objects
            //
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _VALID);
            break;
        case NV04_DX6_MULTI_TEXTURE_TRIANGLE:
        case NV10_DX6_MULTI_TEXTURE_TRIANGLE:
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
        case NV10_IMAGE_FROM_CPU:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV05_SCALED_IMAGE_FROM_MEMORY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A1R5G5B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV05_INDEXED_IMAGE_FROM_CPU:
        case NV05_IMAGE_FROM_CPU:
        case NV05_STRETCHED_IMAGE_FROM_CPU:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV11_CELSIUS_PRIMITIVE:
            //
            // The above DRF_NUM macro on GRCLASS has already masked this class
            // number to an 8bit value, we just need to set the PERFORMANCE bit.
            //
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _CLASS_TYPE, _PERFORMANCE);
        case NV10_CELSIUS_PRIMITIVE:
        case NV15_CELSIUS_PRIMITIVE:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV10_TEXTURE_FROM_CPU:
            // TEXTURE_FROM_CPU is similar to the IMAGE_FROM_CPU classes, but with
            // only a subset of the methods, so we'll need to make sure things are
            // initialized in a way that makes sense for this class.
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_CONFIG, _SRCCOPY)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _DITHER_MODE, _TRUNCATE);    
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        default:
            // assume defaults are cool
            break;
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
    //
    // Be sure to clear this dword clear to keep the graphics method
    // trapped disabled.
    //
    INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_3), 0);

    return (RM_OK);
}

//
// Given a pipe address and ChID return the corresponding pipe context save area in grTable.
// This way, pipe state can be preloaded in the grTable and put in the HW during the first
// grctxt switch.
//
static U032 *
grInitPipeContextAddr(
    PGRAPHICSCHANNEL_NV10 pGrChan,
    U032 base
)
{
    U032 index;

    if (base >= PIPE_FORMAT_BASE && base < (PIPE_FORMAT_BASE + (INDEX_DMA_FORMAT_COUNT*4)))
    {
        index = (base - PIPE_FORMAT_BASE) / sizeof(U032);
        return &pGrChan->CurrentPatch.CurrentPipe.IndexDmaFormat[index];
    }
    if (base >= PIPE_PRIM_BASE && base < (PIPE_PRIM_BASE + (INDEX_DMA_PRIM_COUNT*4)))
    {
        index = (base - PIPE_PRIM_BASE) / sizeof(U032);
        return &pGrChan->CurrentPatch.CurrentPipe.IndexDmaPrim[index];
    }
    if (base >= PIPE_ASSM_BASE && base < (PIPE_ASSM_BASE + (PRIM_ASSM_COUNT*4)))
    {
        index = (base - PIPE_ASSM_BASE) / sizeof(U032);
        return &pGrChan->CurrentPatch.CurrentPipe.PrimAssm[index];
    }
    if (base >= PIPE_FILE_BASE && base < (PIPE_FILE_BASE + (VERTEX_FILE_COUNT*4)))
    {
        index = (base - PIPE_FILE_BASE) / sizeof(U032);
        return &pGrChan->CurrentPatch.CurrentPipe.VertexFile[index];
    }
    if (base >= PIPE_VAB_BASE && base < (PIPE_VAB_BASE + (XF_VAB_COUNT*4)))
    {
        index = (base - PIPE_VAB_BASE) / sizeof(U032);
        return &pGrChan->CurrentPatch.CurrentPipe.XF_VAB[index];
    }
    if (base >= PIPE_CHEOPS_BASE && base < (PIPE_CHEOPS_BASE + (XF_CHEOPS_COUNT*4)))
    {
        index = (base - PIPE_CHEOPS_BASE) / sizeof(U032);
        return &pGrChan->CurrentPatch.CurrentPipe.XF_CHEOPS[index];
    }
    if (base >= PIPE_ZOSER_BASE && base < (PIPE_ZOSER_BASE + (XF_ZOSER_COUNT*4)))
    {
        index = (base - PIPE_ZOSER_BASE) / sizeof(U032);
        return &pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER[index];
    }
    if (base >= PIPE_ZOSER_C0_BASE && base < (PIPE_ZOSER_C0_BASE + (XF_ZOSER_C0_COUNT*4)))
    {
        index = (base - PIPE_ZOSER_C0_BASE) / sizeof(U032);
        return &pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER_C0[index];
    }
    if (base >= PIPE_ZOSER_C1_BASE && base < (PIPE_ZOSER_C1_BASE + (XF_ZOSER_C1_COUNT*4)))
    {
        index = (base - PIPE_ZOSER_C1_BASE) / sizeof(U032);
        return &pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER_C1[index];
    }
    if (base >= PIPE_ZOSER_C2_BASE && base < (PIPE_ZOSER_C2_BASE + (XF_ZOSER_C2_COUNT*4)))
    {
        index = (base - PIPE_ZOSER_C2_BASE) / sizeof(U032);
        return &pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER_C2[index];
    }
    if (base >= PIPE_ZOSER_C3_BASE && base < (PIPE_ZOSER_C3_BASE + (XF_ZOSER_C3_COUNT*4)))
    {
        index = (base - PIPE_ZOSER_C3_BASE) / sizeof(U032);
        return &pGrChan->CurrentPatch.CurrentPipe.XF_ZOSER_C3[index];
    }

    return NULL;        // error
}

#define grInitPipeContextData(data)     *pipectx++ = data;

static VOID
_nvHalGrInitDx5_NV10(
    PHALHWINFO      pHalHwInfo,
    U032            ChID
)
{
    PGRHALINFO_NV10 pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;
    PGRAPHICSCHANNEL_NV10 pGrChan;
    U032      i, *pipectx;
    U008      *ptr;

    pGrChan = &pGrHalPvtInfo->grChannels[ChID];
    
    //
    // Load the channel with the desired initial state.
    //
    for (i = 0, ptr = (U008 *)pGrChan; i < sizeof (GRAPHICSCHANNEL_NV10); i++)
        *ptr++ = 0x0;

    // Load the non-zero initial values for DX5 classes
    // Initialize the register state
    pGrChan->CurrentPatch.TexFormat0           = 0x1000;
    pGrChan->CurrentPatch.TexFormat1           = 0x1000;
    pGrChan->CurrentPatch.TexControl0_0        = 0x4003ff80;
    pGrChan->CurrentPatch.ImageRect0           = 0x80008;
    pGrChan->CurrentPatch.ImageRect1           = 0x80008;
    pGrChan->CurrentPatch.Combine1ColorOCW     = 0x10000000;
    pGrChan->CurrentPatch.ZClipMax             = 0x4b7fffff;
    pGrChan->CurrentPatch.WinClipVert[0]       = 0x7ff0800;
    pGrChan->CurrentPatch.WinClipHorz[0]       = 0x7ff0800;

    // Initialize the internal XF constants
    pipectx = grInitPipeContextAddr(pGrChan, 0x6740);   // CHEOP_CTX, eye pos
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x3f800000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6750);   // CHEOP_CTX_CONST0
    grInitPipeContextData(0x40000000);
    grInitPipeContextData(0x40000000);
    grInitPipeContextData(0x40000000);
    grInitPipeContextData(0x40000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6760);   // CHEOP_CTX_CONST1
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6770);   // CHEOP_CTX_CONST2
    grInitPipeContextData(0xc5000000);
    grInitPipeContextData(0xc5000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6780);   // CHEOP_CTX, fog plane
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x67a0);   // CHEOP_CTX_CONST3
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x3f800000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6ab0);   // FOG k0, k1, k2
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x3f800000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6ac0);   // CHEOP_CTX_ZERO
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6c10);   // ZOSER_C0
    grInitPipeContextData(0xbf800000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x7030);   // LIGHT0 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30

    pipectx = grInitPipeContextAddr(pGrChan, 0x7040);   // LIGHT1 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30

    pipectx = grInitPipeContextAddr(pGrChan, 0x7050);   // LIGHT2 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30

    pipectx = grInitPipeContextAddr(pGrChan, 0x7060);   // LIGHT3 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30

    pipectx = grInitPipeContextAddr(pGrChan, 0x7070);   // LIGHT4 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30

    pipectx = grInitPipeContextAddr(pGrChan, 0x7080);   // LIGHT5 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30

    pipectx = grInitPipeContextAddr(pGrChan, 0x7090);   // LIGHT6 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30

    pipectx = grInitPipeContextAddr(pGrChan, 0x70a0);   // LIGHT7 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30

    pipectx = grInitPipeContextAddr(pGrChan, 0x6a80);   // ZOSER_EYE_DIR
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x3f800000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6aa0);   // ZOSER_FRONT_AMBIENT2
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x0040);   // begin-end
    grInitPipeContextData(0x00000005);                  // triangles

    pipectx = grInitPipeContextAddr(pGrChan, 0x6400);   // model/view matrix
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x4b7fffff);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6410);   // model/view matrix
    grInitPipeContextData(0xc5000000);
    grInitPipeContextData(0xc5000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6420);   // model/view matrix
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6430);   // model/view matrix
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x64c0);   // model/view matrix
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x4b7fffff);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x64d0);   // model/view matrix
    grInitPipeContextData(0xc5000000);
    grInitPipeContextData(0xc5000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x64e0);   // model/view matrix
    grInitPipeContextData(0xc4fff000);
    grInitPipeContextData(0xc4fff000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x64f0);   // model/view matrix
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);

}

static VOID
_nvHalGrInitCelsius_NV10(
    PHALHWINFO  pHalHwInfo,
    U032        ChID
)
{
    PGRHALINFO_NV10 pGrHalPvtInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalPvtInfo;
    PGRAPHICSCHANNEL_NV10 pGrChan;
    U032      i, *pipectx;
    U008      *ptr;
    PHWREG nvAddr = pHalHwInfo->nvBaseAddr;

    pGrChan = &pGrHalPvtInfo->grChannels[ChID];
    pHalHwInfo->pGrHalInfo->has3dObject |= (0x1 << ChID);

    //
    // Load the channel with the desired initial state.
    //
    for (i = 0, ptr = (U008 *)pGrChan; i < sizeof (GRAPHICSCHANNEL_NV10); i++)
        *ptr++ = 0x0;

    // Load the non-zero initial values for Celsius classes
    // Initialize the register state
    pGrChan->CurrentPatch.TexFormat0           = 0x1000;
    pGrChan->CurrentPatch.TexFormat1           = 0x1000;
    pGrChan->CurrentPatch.ImageRect0           = 0x80008;
    pGrChan->CurrentPatch.ImageRect1           = 0x80008;
    pGrChan->CurrentPatch.ZClipMax             = 0x4b7fffff;
    pGrChan->CurrentPatch.SetupRaster          = DRF_DEF(_PGRAPH, _SETUPRASTER, _SWATHWIDTH, _32);

    // Check for an override for the swath width and width multiplier
    if (pGrHalPvtInfo->SwathControl & SWATH_CONTROL_VALID_BIT) {
        U032 data32 = pGrHalPvtInfo->SwathControl;

        // setup the swath width from the registry key
        pGrChan->CurrentPatch.SetupRaster      = DRF_NUM(_PGRAPH, _SETUPRASTER, _SWATHWIDTH, (data32 & 0xF));

        // setup the multiplier from the registry key
        pGrHalPvtInfo->Debug4 &= ~(DRF_MASK(NV_PGRAPH_DEBUG_4_SWATHCONTROL) << DRF_SHIFT(NV_PGRAPH_DEBUG_4_SWATHCONTROL));
        pGrHalPvtInfo->Debug4 |= DRF_NUM(_PGRAPH, _DEBUG_4, _SWATHCONTROL, (data32 >> 4));

        FLD_WR_DRF_NUM(_PGRAPH, _DEBUG_4, _SWATHCONTROL, (data32 >> 4));
    } else {
        // reset the width multiplier back to the default
        pGrHalPvtInfo->Debug4 &= ~(DRF_MASK(NV_PGRAPH_DEBUG_4_SWATHCONTROL) << DRF_SHIFT(NV_PGRAPH_DEBUG_4_SWATHCONTROL));
        pGrHalPvtInfo->Debug4 |= DRF_NUM(_PGRAPH, _DEBUG_4, _SWATHCONTROL, 0x0);

        if (IsNV15orBetter_NV10(pHalHwInfo->pMcHalInfo))
        {
            FLD_WR_DRF_NUM(_PGRAPH, _DEBUG_4, _SWATHCONTROL, 0x0);
        }
    }

    // Initialize internal XF constants, also has side effect of initializing
    // upper half of passthrough slot for bundles.

    pipectx = grInitPipeContextAddr(pGrChan, 0x6740);   // CHEOP_CTX, eye pos
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x3f800000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6750);   // CHEOP_CTX_CONST0
    grInitPipeContextData(0x40000000);
    grInitPipeContextData(0x40000000);
    grInitPipeContextData(0x40000000);
    grInitPipeContextData(0x40000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6760);   // CHEOP_CTX_CONST1
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6770);   // CHEOP_CTX_CONST2
    grInitPipeContextData(0x3f000000);
    grInitPipeContextData(0x3f000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6780);   // CHEOP_CTX (fog plane)
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x67a0);   // CHEOP_CTX_CONST3
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x3f800000);
    grInitPipeContextData(0x3f800000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6ac0);   // CHEOP_CTX_ZERO
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x6c10);   // ZOSER C0
    grInitPipeContextData(0xbf800000);

    pipectx = grInitPipeContextAddr(pGrChan, 0x7030);   // LIGHT0 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30
          
    pipectx = grInitPipeContextAddr(pGrChan, 0x7040);   // LIGHT1 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30
          
    pipectx = grInitPipeContextAddr(pGrChan, 0x7050);   // LIGHT2 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30
          
    pipectx = grInitPipeContextAddr(pGrChan, 0x7060);   // LIGHT3 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30
          
    pipectx = grInitPipeContextAddr(pGrChan, 0x7070);   // LIGHT4 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30
          
    pipectx = grInitPipeContextAddr(pGrChan, 0x7080);   // LIGHT5 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30
          
    pipectx = grInitPipeContextAddr(pGrChan, 0x7090);   // LIGHT6 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30
          
    pipectx = grInitPipeContextAddr(pGrChan, 0x70a0);   // LIGHT7 range
    grInitPipeContextData(0x7149f2ca);                  // 1.0e30
          
    pipectx = grInitPipeContextAddr(pGrChan, 0x6a80);   // ZOSER_EYE_DIR
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x3f800000);
         
    pipectx = grInitPipeContextAddr(pGrChan, 0x6aa0);   // ZOSER_FRONT_AMBIENT2 (MCOL)
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
    grInitPipeContextData(0x00000000);
}
