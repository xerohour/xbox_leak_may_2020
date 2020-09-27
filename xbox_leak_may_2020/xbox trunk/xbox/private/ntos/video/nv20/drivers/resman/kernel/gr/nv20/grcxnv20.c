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
* Module: grcxnv20.c                                                        *
*   The nv20 graphics engine context switching is managed in this module.   *
*   On nv20, context switching is handled in hardware, so all we really     *
*   have to do here is handle methods in the 3d classes that require        *
*   accessing internal pipe state.                                          *
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
#include <heap.h>
#include "nvhw.h"
#include "nvhalcpp.h"

// forwards
static VOID grUnloadRdiContext(PHALHWINFO, U032, U032, U032, U032 *);
static VOID grLoadRdiContext(PHALHWINFO, U032, U032, U032, U032 *);

RM_STATUS nvHalGrLoadChannelContext_NV20(PHALHWINFO, U032);
RM_STATUS nvHalGrUnloadChannelContext_NV20(PHALHWINFO, U032);

// externs
extern RM_STATUS dmaHalInstanceToHalInfo_NV20(U032, U032, PDMAHALOBJINFO *, PHALHWINFO);

//
// Load Pipe Context
//
#if 0  // static, but never called
static RM_STATUS
grLoadPipeContext(
    PHALHWINFO pHalHwInfo,
    U032       Base,
    U032      *SaveAddr,
    U032       Count
)
{
    U032 i;

    REG_WR32(NV_PGRAPH_PIPE_ADDRESS, Base);
    for (i = 0; i < Count; i++) {
	  REG_WR32(NV_PGRAPH_PIPE_DATA, SaveAddr[i]);
    }
    return (RM_OK);
}

//
// Unload Pipe Context
//
static RM_STATUS
grUnloadPipeContext(
    PHALHWINFO pHalHwInfo,
    U032       Base,
    U032      *SaveAddr,
    U032       Count
)
{
    U032 i;

    REG_WR32(NV_PGRAPH_PIPE_ADDRESS, Base);
    for (i = 0; i < Count; i++) {
	  SaveAddr[i] = REG_RD32(NV_PGRAPH_PIPE_DATA);
    }
    return (RM_OK);
}
#endif

//
// Unload Rdi Context
//
static VOID
grUnloadRdiContext(
    PHALHWINFO pHalHwInfo,
    U032       ramSel,
    U032       offset,
    U032       count,
    U032      *saveAddr
)
{
    U032 i;

    REG_WR32(NV_PGRAPH_RDI_INDEX, (ramSel<<16 | offset));
    for (i = 0; i < count; i++)
        saveAddr[i] = REG_RD32(NV_PGRAPH_RDI_DATA);
}

//
// Load RDI Context
//
static VOID
grLoadRdiContext(
    PHALHWINFO pHalHwInfo,
    U032       ramSel,
    U032       offset,
    U032       count,
    U032      *saveAddr
)
{
    U032 i;

    REG_WR32(NV_PGRAPH_RDI_INDEX, (ramSel<<16 | offset));
    for (i = 0; i < count; i++)
        REG_WR32(NV_PGRAPH_RDI_DATA, saveAddr[i]);
}

RM_STATUS
nvHalGrGetState_NV20(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)
{
    PGRHALINFO_NV20 pGrHalPvtInfo;
    U032 ChID;
    U032 access, instance;
    U032 temp;
    NvGraphicsState *stateptr;
    RM_STATUS status;
    PDMAHALOBJINFO pDmaHalInfo;

    GR_PRINTF((DBG_LEVEL_INFO, "NVRM: nvHalGrGetState\r\n"));

    pGrHalPvtInfo = (PGRHALINFO_NV20)pHalHwInfo->pGrHalPvtInfo;
    ChID = pGrExceptionData->ChID;

#ifdef DEBUG
    // we're from a GR exception, so we should be idled/disable already
    if (REG_RD_DRF(_PGRAPH, _FIFO, _ACCESS) || REG_RD32(NV_PGRAPH_STATUS))
        DBG_BREAKPOINT();
#endif

    access = REG_RD32(NV_PGRAPH_FIFO);
    REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED);
    GR_DONE();

    // make sure we're the current channel
    if (REG_RD_DRF(_PGRAPH, _CTX_USER, _CHID) != ChID)
    {
        GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: GetState method issued on non-current channel: 0x%x\n", ChID));
        DBG_BREAKPOINT();
        return RM_ERR_INVALID_STATE;
    }

    // determine where the ContextDmaState buffer resides
    instance = REG_RD_DRF(_PGRAPH, _GETSTATE, _DMA_INSTANCE);

    if (!instance)
    {
        GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: NULL ContextDmaState buffer\n\r"));
        DBG_BREAKPOINT();
        return RM_ERR_INVALID_STATE;
    }

    // convert instance address to DmaHalInfo structure
    status = dmaHalInstanceToHalInfo_NV20(ChID, instance, &pDmaHalInfo, pHalHwInfo);
    if (status)
    {
        GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: ContextDmaState invalid instance\n\r"));
        DBG_BREAKPOINT();
        return RM_ERR_INVALID_STATE;
    }

    if (pDmaHalInfo->BufferSize < sizeof (NvGraphicsState))
    {
        GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: ContextDmaState smaller than GetState data\n\r"));
        DBG_BREAKPOINT();
        return status;
    }

    stateptr = (NvGraphicsState *)pDmaHalInfo->BufferBase;

    // first 4 DWORDS of NvGraphicsState is the PerFragment state
    ((U032 *)stateptr)[0] = REG_RD32(NV_PGRAPH_CONTROL_0);
    ((U032 *)stateptr)[1] = REG_RD32(NV_PGRAPH_CONTROL_1);
    ((U032 *)stateptr)[2] = REG_RD32(NV_PGRAPH_CONTROL_2);
    ((U032 *)stateptr)[3] = REG_RD32(NV_PGRAPH_BLEND);

    // save the WITHIN_BEGINEND state
    stateptr->glBeginEndState = REG_RD32(NV_PGRAPH_SHADOW);

    //
    // Fetch PerVertexState from the VAB.
    //
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_DIFF<<4, 4, (U032 *)(&stateptr->PerVertexState.primaryColor[0]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_SPEC<<4, 3, (U032 *)(&stateptr->PerVertexState.secondaryColor[0]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_FOG<<4, 1, (U032 *)(&stateptr->PerVertexState.fogCoord));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_TXT0<<4, 4, (U032 *)(&stateptr->PerVertexState.texCoord[0][0]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_TXT1<<4, 4, (U032 *)(&stateptr->PerVertexState.texCoord[1][0]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_NRM<<4, 3, (U032 *)(&stateptr->PerVertexState.normal[0]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_WGHT<<4, 1, (U032 *)(&stateptr->PerVertexState.vertexWeight));

    //
    // Fetch VertexArrayState from IDX_FMT.
    //
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_POS<<3, 2, (U032 *)(&stateptr->VertexArrayState.vertexOffset));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_DIFF<<3, 2, (U032 *)(&stateptr->VertexArrayState.diffuseOffset));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_SPEC<<3, 2, (U032 *)(&stateptr->VertexArrayState.specularOffset));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_TXT0<<3, 2, (U032 *)(&stateptr->VertexArrayState.texCoord0Offset));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_TXT1<<3, 2, (U032 *)(&stateptr->VertexArrayState.texCoord1Offset));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_NRM<<3, 2, (U032 *)(&stateptr->VertexArrayState.normalOffset));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_WGHT<<3, 2, (U032 *)(&stateptr->VertexArrayState.weightOffset));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_FOG<<3, 2, (U032 *)(&stateptr->VertexArrayState.fogOffset));

    //
    // Fetch edgeFlag from IDX_FMT (word 48, bit 28).
    //
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, 48<<2, 1, &temp);
    stateptr->PerVertexState.edgeFlag = (temp >> 28) & 1;

    //
    // Fetch PrimitiveAssmState from Cas0/Cas1/Cas2.
    //
    // The nv10 layout (16 word values):
    //
    //  X   Y   Z   W
    //  D   S   F   ef,ptsize
    //  S   T   R   Q
    //  S   T   R   Q
    //
    // The nv20 layout (spread out over 32 word values):
    //
    //  D   S   Z   W
    //  -   -   F   -
    //  S   T   R   Q
    //  S   T   R   Q
    //  -   -   -   -
    //  -   -   -   -
    //  X   Y   ef,ptsize -
    //
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 0<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+4]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 1<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+5]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 2<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+2]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 3<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+3]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 6<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+6]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 8<<2, 4, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+8]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 12<<2, 4, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+12]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 24<<2, 2, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+0]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 26<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+7]));

    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 0<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+4]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 1<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+5]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 2<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+2]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 3<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+3]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 6<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+6]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 8<<2, 4, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+8]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 12<<2, 4, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+12]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 24<<2, 2, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+0]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 26<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+7]));

    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 0<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+4]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 1<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+5]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 2<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+2]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 3<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+3]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 6<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+6]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 8<<2, 4, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+8]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 12<<2, 4, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+12]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 24<<2, 2, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+0]));
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 26<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+7]));

#ifdef CHECK_BEGINEND_MISMATCH
    if (stateptr->PerVertexState.pad3 && 
        (REG_RD_DRF(_PGRAPH, _SHADOW, _WITHIN_BEGINEND) == 0)) {
        _asm int 3;
    }
    if ((stateptr->PerVertexState.pad3 == 0) && 
         REG_RD_DRF(_PGRAPH, _SHADOW, _WITHIN_BEGINEND)) {
        _asm int 3;
    }
#endif

    GR_DONE();
    REG_WR32(NV_PGRAPH_FIFO, access);

    return RM_OK;
}

RM_STATUS
nvHalGrPutState_NV20(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)
{
    PGRHALINFO_NV20 pGrHalPvtInfo;
    U032 access, instance;
    U032 temp;
    NvGraphicsState *stateptr;
    RM_STATUS status;
    PDMAHALOBJINFO pDmaHalInfo;

    GR_PRINTF((DBG_LEVEL_INFO, "NVRM: nvHalGrPutState\r\n"));

    pGrHalPvtInfo = (PGRHALINFO_NV20)pHalHwInfo->pGrHalPvtInfo;

    access = REG_RD32(NV_PGRAPH_FIFO);
    REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED);
    GR_DONE();

    // make sure we're the current channel
    if (REG_RD_DRF(_PGRAPH, _CTX_USER, _CHID) != pGrExceptionData->ChID)
    {
        GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: PutState method issued on non-current channel: 0x%x\n", pGrExceptionData->ChID));
        DBG_BREAKPOINT();
        return RM_ERR_INVALID_STATE;
    }

    // determine where the ContextDmaState buffer resides
    instance = REG_RD_DRF(_PGRAPH, _GETSTATE, _DMA_INSTANCE);
    if (!instance)
    {
        GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: NULL ContextDmaState buffer\n\r"));
        DBG_BREAKPOINT();
        return RM_ERROR;
    }

    // convert instance address to DmaHalInfo structure
    status = dmaHalInstanceToHalInfo_NV20(pGrExceptionData->ChID, instance, &pDmaHalInfo, pHalHwInfo);
    if (status)
    {
        GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: Invalid ContextDmaState buffer\n\r"));
        DBG_BREAKPOINT();
        return RM_ERR_INVALID_STATE;
    }

    // ensure DmaState buffer is large enough for the GetState data
    if (pDmaHalInfo->BufferSize < sizeof (NvGraphicsState))
    {
        GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: ContextDmaState smaller than GetState data\n\r"));
        DBG_BREAKPOINT();
        return status;
    }

    stateptr = (NvGraphicsState *)pDmaHalInfo->BufferBase;

    // first 4 DWORDS of NvGraphicsState is the PerFragment state
    REG_WR32(NV_PGRAPH_CONTROL_0, ((U032 *)stateptr)[0]);
    REG_WR32(NV_PGRAPH_CONTROL_1, ((U032 *)stateptr)[1]);
    REG_WR32(NV_PGRAPH_CONTROL_2, ((U032 *)stateptr)[2]);
    REG_WR32(NV_PGRAPH_BLEND,    ((U032 *)stateptr)[3]);

    // restore the WITHIN_BEGINEND state
    REG_WR32(NV_PGRAPH_SHADOW, stateptr->glBeginEndState);

    //
    // Fetch PerVertexState from the VAB.
    //
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_DIFF<<4, 4, (U032 *)(&stateptr->PerVertexState.primaryColor[0]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_SPEC<<4, 3, (U032 *)(&stateptr->PerVertexState.secondaryColor[0]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_FOG<<4, 1, (U032 *)(&stateptr->PerVertexState.fogCoord));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_TXT0<<4, 4, (U032 *)(&stateptr->PerVertexState.texCoord[0][0]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_TXT1<<4, 4, (U032 *)(&stateptr->PerVertexState.texCoord[1][0]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_NRM<<4, 3, (U032 *)(&stateptr->PerVertexState.normal[0]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, NV_IGRAPH_XF_VAB_WGHT<<4, 1, (U032 *)(&stateptr->PerVertexState.vertexWeight));

    //
    // Fetch VertexArrayState from IDX_FMT.
    //
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_POS<<3, 2, (U032 *)(&stateptr->VertexArrayState.vertexOffset));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_DIFF<<3, 2, (U032 *)(&stateptr->VertexArrayState.diffuseOffset));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_SPEC<<3, 2, (U032 *)(&stateptr->VertexArrayState.specularOffset));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_TXT0<<3, 2, (U032 *)(&stateptr->VertexArrayState.texCoord0Offset));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_TXT1<<3, 2, (U032 *)(&stateptr->VertexArrayState.texCoord1Offset));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_NRM<<3, 2, (U032 *)(&stateptr->VertexArrayState.normalOffset));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_WGHT<<3, 2, (U032 *)(&stateptr->VertexArrayState.weightOffset));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, NV_IGRAPH_XF_VAB_FOG<<3, 2, (U032 *)(&stateptr->VertexArrayState.fogOffset));

    //
    // Fetch edgeFlag from IDX_FMT (word 48, bit 28).
    //
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, 48<<2, 1, &temp);
    temp &= ~0x10000000;
    temp |= ((stateptr->PerVertexState.edgeFlag & 1) << 28);
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, 48<<2, 1, &temp);

    //
    // Fetch PrimitiveAssmState from Cas0/Cas1/Cas2.
    //
    // The nv10 layout (16 word values):
    //
    //  X   Y   Z   W
    //  D   S   F   ef,ptsize
    //  S   T   R   Q
    //  S   T   R   Q
    //
    // The nv20 layout (spread out over 32 word values):
    //
    //  D   S   Z   W
    //  -   -   F   -
    //  S   T   R   Q
    //  S   T   R   Q
    //  -   -   -   -
    //  -   -   -   -
    //  X   Y   ef,ptsize -
    //
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 0<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+4]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 1<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+5]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 2<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+2]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 3<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+3]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 6<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+6]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 8<<2, 4, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+8]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 12<<2, 4, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+12]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 24<<2, 2, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+0]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, 26<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[0+7]));

    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 0<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+4]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 1<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+5]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 2<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+2]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 3<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+3]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 6<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+6]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 8<<2, 4, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+8]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 12<<2, 4, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+12]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 24<<2, 2, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+0]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, 26<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[16+7]));

    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 0<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+4]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 1<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+5]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 2<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+2]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 3<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+3]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 6<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+6]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 8<<2, 4, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+8]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 12<<2, 4, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+12]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 24<<2, 2, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+0]));
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, 26<<2, 1, (U032 *)(&stateptr->PrimitiveAssmState.primAssm[32+7]));

#ifdef CHECK_BEGINEND_MISMATCH
        // check for a state mismatch
        if (stateptr->PerVertexState.pad3 && 
            (REG_RD_DRF(_PGRAPH, _SHADOW, _WITHIN_BEGINEND) == 0)) {
            _asm int 3;
        }

        if ((stateptr->PerVertexState.pad3 == 0) && 
             REG_RD_DRF(_PGRAPH, _SHADOW, _WITHIN_BEGINEND)) {
            _asm int 3;
        }
#endif

    GR_DONE();
    REG_WR32(NV_PGRAPH_FIFO, access);

    return RM_OK;
}

RM_STATUS
nvHalGrSnapshot_NV20(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)
{
    PGRHALINFO_NV20 pGrHalPvtInfo;
    PGRAPHICSSTATE_NV20 pGrState;
    U032 access;

    GR_PRINTF((DBG_LEVEL_INFO, "NVRM: nvHalGrGetState\r\n"));

    pGrHalPvtInfo = (PGRHALINFO_NV20)pHalHwInfo->pGrHalPvtInfo;

#ifdef DEBUG
    // we're from a GR exception, so we should be idled/disable already
    if (REG_RD_DRF(_PGRAPH, _FIFO, _ACCESS) || REG_RD32(NV_PGRAPH_STATUS))
        DBG_BREAKPOINT();
#endif

    access = REG_RD32(NV_PGRAPH_FIFO);
    REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED);
    GR_DONE();

    // make sure we're the current channel
    if (REG_RD_DRF(_PGRAPH, _CTX_USER, _CHID) != pGrExceptionData->ChID)
    {
        GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: GetState method issued on non-current channel ", pGrExceptionData->ChID));
        DBG_BREAKPOINT();
        return RM_ERR_INVALID_STATE;
    }

    pGrState = &pGrHalPvtInfo->grSnapShots[pGrExceptionData->ChID];

    // save PerFragment state
    pGrState->PerFragmentState.Control0 = REG_RD32(NV_PGRAPH_CONTROL_0);
    pGrState->PerFragmentState.Control1 = REG_RD32(NV_PGRAPH_CONTROL_1);
    pGrState->PerFragmentState.Control2 = REG_RD32(NV_PGRAPH_CONTROL_2);
    pGrState->PerFragmentState.Control3 = REG_RD32(NV_PGRAPH_CONTROL_3);
    pGrState->PerFragmentState.Blend = REG_RD32(NV_PGRAPH_BLEND);

    // save the WITHIN_BEGINEND state
    pGrState->glBeginEndState = REG_RD32(NV_PGRAPH_SHADOW);

    // fetch PerVertexState from the VAB.
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, RDI_ADDR_ZERO, XL_VAB_COUNT, pGrState->PerVertexState.XlVab);

    // fetch VertexArrayState from IDX_FMT.
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, RDI_ADDR_ZERO, IDX_FMT_COUNT, pGrState->VertexArrayState.IdxFmt);

    // fetch PrimitiveAssmState from Cas0/Cas1/Cas2.
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, RDI_ADDR_ZERO, CAS0_COUNT, pGrState->PrimitiveAssmState.Cas0);
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, RDI_ADDR_ZERO, CAS1_COUNT, pGrState->PrimitiveAssmState.Cas1);
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, RDI_ADDR_ZERO, CAS2_COUNT, pGrState->PrimitiveAssmState.Cas2);
    grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_ASSM_STATE, RDI_ADDR_ZERO, ASSM_STATE_COUNT, pGrState->PrimitiveAssmState.AssmState);

#ifdef CHECK_BEGINEND_MISMATCH
    if (pGrState->PerVertexState.pad3 && 
        (REG_RD_DRF(_PGRAPH, _SHADOW, _WITHIN_BEGINEND) == 0)) {
        _asm int 3;
    }
    if ((pGrState->PerVertexState.pad3 == 0) && 
        REG_RD_DRF(_PGRAPH, _SHADOW, _WITHIN_BEGINEND)) {
        _asm int 3;
    }
#endif

    GR_DONE();
    REG_WR32(NV_PGRAPH_FIFO, access);

    return RM_OK;
}

RM_STATUS
nvHalGrReload_NV20(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)
{
    PGRHALINFO_NV20 pGrHalPvtInfo;
    PGRAPHICSSTATE_NV20 pGrState;
    U032 access;

    GR_PRINTF((DBG_LEVEL_INFO, "NVRM: nvHalGrPutState\r\n"));

    pGrHalPvtInfo = (PGRHALINFO_NV20)pHalHwInfo->pGrHalPvtInfo;

    access = REG_RD32(NV_PGRAPH_FIFO);
    REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED);
    GR_DONE();

    // make sure we're the current channel
    if (REG_RD_DRF(_PGRAPH, _CTX_USER, _CHID) != pGrExceptionData->ChID)
    {
        GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: PutState method issued on non-current channel ", pGrExceptionData->ChID));
        DBG_BREAKPOINT();
        return RM_ERR_INVALID_STATE;
    }

    pGrState = &pGrHalPvtInfo->grSnapShots[pGrExceptionData->ChID];

    // restore VAB state
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, RDI_ADDR_ZERO, XL_VAB_COUNT, pGrState->PerVertexState.XlVab);

    // restore VertexArrayState
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, RDI_ADDR_ZERO, IDX_FMT_COUNT, pGrState->VertexArrayState.IdxFmt);

    // restore PrimitiveAssmState
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, RDI_ADDR_ZERO, CAS0_COUNT, pGrState->PrimitiveAssmState.Cas0);
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, RDI_ADDR_ZERO, CAS1_COUNT, pGrState->PrimitiveAssmState.Cas1);
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, RDI_ADDR_ZERO, CAS2_COUNT, pGrState->PrimitiveAssmState.Cas2);
    grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_ASSM_STATE, RDI_ADDR_ZERO, ASSM_STATE_COUNT, pGrState->PrimitiveAssmState.AssmState);

    // restore per-fragment state
    REG_WR32(NV_PGRAPH_CONTROL_0, pGrState->PerFragmentState.Control0);
    REG_WR32(NV_PGRAPH_CONTROL_1, pGrState->PerFragmentState.Control1);
    REG_WR32(NV_PGRAPH_CONTROL_2, pGrState->PerFragmentState.Control2);
    REG_WR32(NV_PGRAPH_CONTROL_3, pGrState->PerFragmentState.Control3);
    REG_WR32(NV_PGRAPH_BLEND, pGrState->PerFragmentState.Blend);

    // restore the WITHIN_BEGINEND state
    REG_WR32(NV_PGRAPH_SHADOW, pGrState->glBeginEndState);

#ifdef CHECK_BEGINEND_MISMATCH
    // check for a state mismatch
    if (stateptr->PerVertexState.pad3 && 
        (REG_RD_DRF(_PGRAPH, _SHADOW, _WITHIN_BEGINEND) == 0)) {
        _asm int 3;
    }

    if ((stateptr->PerVertexState.pad3 == 0) && 
        REG_RD_DRF(_PGRAPH, _SHADOW, _WITHIN_BEGINEND)) {
        _asm int 3;
    }
#endif

    GR_DONE();
    REG_WR32(NV_PGRAPH_FIFO, access);

    return RM_OK;
}

//
// This method gives D3D access to the eye direction state.
// It would be nice to go thru RDI for this, but it turned
// out to be too problematic so we use PIPE_ADDRESS/PIPE_DATA
// instead.
//
RM_STATUS nvHalGrSetEyeDirection_NV20
(
    PHALHWINFO pHalHwInfo,
    PGREXCEPTIONDATA pGrExceptionData
)
{
    U032 offset = (pGrExceptionData->Offset - NV056_SET_EYE_DIRECTION_SW(0))/4;

    REG_WR32(NV_PGRAPH_PIPE_ADDRESS, NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_LTCTX,NV_IGRAPH_XF_LTCTX_EYED<<4) + (offset<<2));
    REG_WR32(NV_PGRAPH_PIPE_DATA, pGrExceptionData->Data);
    GR_DONE();

    return (RM_OK);
}

RM_STATUS nvHalGrSetViewportOffsetCelsius_NV20
(
    PHALHWINFO pHalHwInfo,
    PGREXCEPTIONDATA pGrExceptionData
)
{
    U032 offset = (pGrExceptionData->Offset - NV056_SET_VIEWPORT_OFFSET_SW(0))/4;
    REG_WR32(NV_PGRAPH_PIPE_ADDRESS, NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_XFCTX, NV_IGRAPH_XF_XFCTX_VPOFF<<4) + (offset<<2));
    REG_WR32(NV_PGRAPH_PIPE_DATA, pGrExceptionData->Data);
    GR_DONE();

    return (RM_OK);
}

RM_STATUS nvHalGrSetPassthruViewportOffsetCelsius_NV20
(
    PHALHWINFO pHalHwInfo,
    PGREXCEPTIONDATA pGrExceptionData
)
{
    U032 offset = (pGrExceptionData->Offset - NV056_SET_PASSTHRU_VIEWPORT_OFFSET_SW(0))/4;
    // Update second row of the second modelview matrix
    REG_WR32(NV_PGRAPH_PIPE_ADDRESS, NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_XFCTX, (NV_IGRAPH_XF_XFCTX_MMAT1+1)<<4) + (offset<<2));
    REG_WR32(NV_PGRAPH_PIPE_DATA, pGrExceptionData->Data);
    GR_DONE();

    return (RM_OK);
}

RM_STATUS nvHalGrSetViewportOffsetKelvin_NV20
(
    PHALHWINFO pHalHwInfo,
    PGREXCEPTIONDATA pGrExceptionData
)
{
    U032 offset = (pGrExceptionData->Offset - NV097_SET_VIEWPORT_OFFSET_SW(0))/4;
    REG_WR32(NV_PGRAPH_PIPE_ADDRESS, NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_XFCTX, NV_IGRAPH_XF_XFCTX_VPOFF<<4) + (offset<<2));
    REG_WR32(NV_PGRAPH_PIPE_DATA, pGrExceptionData->Data);
    GR_DONE();

    return (RM_OK);
}

RM_STATUS nvHalGrSetPassthruViewportOffsetKelvin_NV20
(
    PHALHWINFO pHalHwInfo,
    PGREXCEPTIONDATA pGrExceptionData
)
{
    U032 offset = (pGrExceptionData->Offset - NV097_SET_PASSTHRU_VIEWPORT_OFFSET_SW(0))/4;
    // Update second row of the second modelview matrix
    REG_WR32(NV_PGRAPH_PIPE_ADDRESS, NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_XFCTX, (NV_IGRAPH_XF_XFCTX_MMAT1+1)<<4) + (offset<<2));
    REG_WR32(NV_PGRAPH_PIPE_DATA, pGrExceptionData->Data);
    GR_DONE();

    return (RM_OK);
}

RM_STATUS
nvHalGrLoadChannelContext_NV20
(
    PHALHWINFO pHalHwInfo,
    U032 ChID
)
{
    PGRHALINFO_NV20 pGrHalPvtInfo = (PGRHALINFO_NV20)pHalHwInfo->pGrHalPvtInfo;
    U032 misc;
    U032 ctxSwitch1 = 0;

    //
    // Service any outstanding graphics exceptions.
    //
    if (REG_RD32(NV_PGRAPH_INTR))
        HALRMSERVICEINTR(pHalHwInfo, GR_ENGINE_TAG);
        
    //
    // Save current graphics interface state.
    //
    GR_SAVE_STATE(misc);
    HAL_GR_IDLE(pHalHwInfo);

    //
    // Unload current context.
    //   
    if (pGrHalPvtInfo->currentChID != ChID) {
        (void) nvHalGrUnloadChannelContext_NV20(pHalHwInfo, pGrHalPvtInfo->currentChID);
    }

    //
    // If the incoming channel is our "invalid" channel, then
    // invalidate and we're done.
    //
    if ((pGrHalPvtInfo->currentChID = ChID) == NUM_FIFOS_NV20)
    {
        REG_WR32(NV_PGRAPH_CTX_CONTROL,
                 DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US) |
                 DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME, _NOT_EXPIRED) |
                 DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID, _INVALID) |
                 DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING, _IDLE) |
                 DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE, _ENABLED));
        REG_WR_DRF_DEF(_PGRAPH, _FFINTFC_ST2, _CHID_STATUS, _VALID);

        //
        // Restore graphics interface state.
        //
		misc |= DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _ENABLED);
        GR_RESTORE_STATE(misc); // FIFO reenabled in here

        return (RM_OK);
    }
    else if (ChID > NUM_FIFOS_NV20)
    {
        GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: Invalid Channel on Graphics Context Switch: 0x%x\n", ChID));
        DBG_BREAKPOINT();
        return (RM_ERROR);
    }

    //
    // If we have 3D state in this channel, then we need to
    // workaround a problem with FD state (see magnus' chsw3.c
    // diag in //hw/nv20/diag/tests/src for more info).
    //
    if (pGrHalPvtInfo->currentObjects3d[ChID] != 0)
    {
        U032 i;

        // Reset IDX/VTX/CAS state.
        REG_WR32(NV_PGRAPH_DEBUG_0,
                 DRF_DEF(_PGRAPH, _DEBUG_0, _IDX_STATE, _RESET) |
                 DRF_DEF(_PGRAPH, _DEBUG_0, _VTX_STATE, _RESET) |
                 DRF_DEF(_PGRAPH, _DEBUG_0, _CAS_STATE, _RESET));
        i = REG_RD32(NV_PGRAPH_DEBUG_0);
        REG_WR32(NV_PGRAPH_DEBUG_0, 0x0);
        i = REG_RD32(NV_PGRAPH_DEBUG_0);

        // Clear FD mode by writing 0 to all FD registers.
        REG_WR32(NV_PGRAPH_RDI_INDEX,
                 DRF_NUM(_PGRAPH, _RDI_INDEX, _SELECT, RDI_RAMSEL_FD_CTRL) |
                 DRF_NUM(_PGRAPH, _RDI_INDEX, _ADDRESS, 0));
        for (i = 0; i < 15; i++)
            REG_WR32(NV_PGRAPH_RDI_DATA, 0);
    }

    //
    // Invalidate tcache.
    //
    FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_1, _CACHE, _INVALIDATE);

    REG_WR_DRF_NUM(_PGRAPH, _CTX_USER, _CHID, ChID);

    //
    // WAR PART1:
    //
    // There's a ctx switch bug that leads to IDX_FMT compsel
    // bit corruption.  These compsel bits are handled differently
    // between kelvin and celsius.  At ctx switch time, the IDX_FMT
    // kelvin mode is enabled if the current CTX_SWITCH1_GRCLASS isn't
    // celsius.  The problem is that it still could be a celsius
    // channel, but a different object currently loaded (e.g. a blit
    // object).  A kelvin mode save of this state when celsius is the
    // _real_ class leads to a hang.
    //
    // The war involves the following when the incoming channel is
    // a celsius channel:
    //    - save the incoming CTX_SWITCH1 register value
    //    - spoof a celsius instance in the CTX_SWITCH1 context buffer location
    //    - tell the hw to restore the context
    //    - update the CTX_SWITCH1 register in hw with the stashed
    //      original value
    //
    if (pGrHalPvtInfo->hasCelsius & (1 << ChID))
    {
        ctxSwitch1 = INST_RD32(pGrHalPvtInfo->CtxTable[ChID], 4);
        INST_WR32(pGrHalPvtInfo->CtxTable[ChID], 4, DRF_NUM(_PGRAPH, _CTX_SWITCH1, _GRCLASS, NV15_CELSIUS_PRIMITIVE));
    }

    //
    // Load new context.
    //
    REG_WR_DRF_NUM(_PGRAPH, _CHANNEL_CTX_POINTER, _INST, pGrHalPvtInfo->CtxTable[ChID]);
    REG_WR_DRF_DEF(_PGRAPH, _CHANNEL_CTX_TRIGGER, _READ_IN, _ACTIVATE);

    HAL_GR_IDLE(pHalHwInfo);

    //
    // WAR PART2:
    //
    // Update hardware with saved value.
    //
    if (pGrHalPvtInfo->hasCelsius & (1 << ChID))
    {
        REG_WR32(NV_PGRAPH_CTX_SWITCH1, ctxSwitch1);
    }

    //
    // Update hardware with new chid.
    // 
    FLD_WR_DRF_NUM(_PGRAPH, _CTX_USER, _CHID, ChID);
    REG_WR32(NV_PGRAPH_CTX_CONTROL,
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME, _NOT_EXPIRED) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID, _VALID) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING, _IDLE) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE, _ENABLED));

    REG_WR32(NV_PGRAPH_FFINTFC_ST2, REG_RD32(NV_PGRAPH_FFINTFC_ST2) & 0xCFFFFFFF);

    return (RM_OK);
}

RM_STATUS
nvHalGrUnloadChannelContext_NV20
(
    PHALHWINFO pHalHwInfo,
    U032 ChID
)
{
    PGRHALINFO_NV20 pGrHalPvtInfo = (PGRHALINFO_NV20)pHalHwInfo->pGrHalPvtInfo;
    U032 ctxSwitch1 = 0;

    //
    // No need to unload an invalid channel
    //
    if (ChID == NUM_FIFOS_NV20)
        return (RM_OK);

    //
    // WAR PART1:
    //
    // There's a ctx switch bug that leads to IDX_FMT compsel
    // bit corruption.  These compsel bits are handled differently
    // between kelvin and celsius.  At ctx switch time, the IDX_FMT
    // kelvin mode is enabled if the current CTX_SWITCH1_GRCLASS isn't
    // celsius.  The problem is that it still could be a celsius
    // channel, but a different object currently loaded (e.g. a blit
    // object).  A kelvin mode save of this state when celsius is the
    // _real_ class leads to a hang.
    //
    // The war involves the following when the outgoing channel is
    // a celsius channel:
    //    - stash the current CTX_SWITCH1 register value
    //    - spoof a celsius instance in CTX_SWITCH1
    //    - tell the hw to save the context
    //    - update the saved CTX_SWITCH1 value in the context buffer
    //      with the stashed original value
    //
    if (pGrHalPvtInfo->hasCelsius & (1 << ChID))
    {
        ctxSwitch1 = REG_RD32(NV_PGRAPH_CTX_SWITCH1);
        REG_WR32(NV_PGRAPH_CTX_SWITCH1, DRF_NUM(_PGRAPH, _CTX_SWITCH1, _GRCLASS, NV15_CELSIUS_PRIMITIVE));
    }


    // Unload context.
    //
    REG_WR_DRF_NUM(_PGRAPH, _CHANNEL_CTX_POINTER, _INST, pGrHalPvtInfo->CtxTable[ChID]);
    REG_WR_DRF_DEF(_PGRAPH, _CHANNEL_CTX_TRIGGER, _WRITE_OUT, _ACTIVATE);

    HAL_GR_IDLE(pHalHwInfo);

    //
    // WAR PART2:
    //
    // Update context buffer with saved value.  The CTX_SWITCH1 value
    // is at offset 4 from the start of the context buffer (see nv20_gr.h).
    //
    if (pGrHalPvtInfo->hasCelsius & (1 << ChID))
    {
        INST_WR32(pGrHalPvtInfo->CtxTable[ChID], 4, ctxSwitch1);
    }

    //
    // Invalidate hw's channel ID.
    //
    REG_WR32(NV_PGRAPH_CTX_CONTROL,
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME, _EXPIRED) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID, _INVALID) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING, _IDLE) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE, _ENABLED));

    return (RM_OK);
}

#ifdef NV20_SW_CTX_SWITCH
RM_STATUS nvHalGrLoadChannelContext_NV20(PHALHWINFO pHalHwInfo, U032 ChID)
{
    PGRHALINFO_NV20 pGrHalPvtInfo = (PGRHALINFO_NV20) pHalHwInfo->pGrHalPvtInfo;
    PGRAPHICSCHANNEL_NV20 pSwCtxTable = (PGRAPHICSCHANNEL_NV20) pGrHalPvtInfo->SwCtxTable;

    int i, status;
    U032 debug_1;
    
    GR_PRINTF((DBG_LEVEL_INFO, "NVRM: nvHalGrLoadChannelContext_NV20\r\n"));

    //
    // Service any outstanding graphics exceptions.
    //
    if (REG_RD32(NV_PGRAPH_INTR))
        HALRMSERVICEINTR(pHalHwInfo, GR_ENGINE_TAG);

    HAL_GR_IDLE(pHalHwInfo);

    //
    // Unload current context.
    //   
    if (pGrHalPvtInfo->currentChID != ChID)
    {
        nvHalGrUnloadChannelContext_NV20(pHalHwInfo,
                                         pGrHalPvtInfo->currentChID,
                                         &pGrHalPvtInfo->SwCtxTable[pGrHalPvtInfo->currentChID]);
    }

    debug_1 = REG_RD32(NV_PGRAPH_DEBUG_1);
    REG_WR32(NV_PGRAPH_DEBUG_1, debug_1 | DRF_DEF(_PGRAPH,_DEBUG_1,_CACHE,_INVALIDATE));
    
    REG_WR32(NV_PGRAPH_CTX_USER, pSwCtxTable[ChID].grCtxCmn.ContextUser);
    
    //
    // Reload context state
    //
    REG_WR32(NV_PGRAPH_CTX_SWITCH1,       pSwCtxTable[ChID].grCtxCmn.ContextSwitch1);
    REG_WR32(NV_PGRAPH_CTX_SWITCH2,       pSwCtxTable[ChID].grCtxCmn.ContextSwitch2);
    REG_WR32(NV_PGRAPH_CTX_SWITCH3,       pSwCtxTable[ChID].grCtxCmn.ContextSwitch3);
    REG_WR32(NV_PGRAPH_CTX_SWITCH4,       pSwCtxTable[ChID].grCtxCmn.ContextSwitch4);
    REG_WR32(NV_PGRAPH_CTX_SWITCH5,       pSwCtxTable[ChID].grCtxCmn.ContextSwitch5);
    
    for (i = 0; i < 8; i++)
      REG_WR32(NV_PGRAPH_CTX_CACHE1(i),   pSwCtxTable[ChID].grCtxCmn.ContextCache1[i]);
    
    for (i = 0; i < 8; i++)
      REG_WR32(NV_PGRAPH_CTX_CACHE2(i),   pSwCtxTable[ChID].grCtxCmn.ContextCache2[i]);
    
    for (i = 0; i < 8; i++)
      REG_WR32(NV_PGRAPH_CTX_CACHE3(i),   pSwCtxTable[ChID].grCtxCmn.ContextCache3[i]);
    
    for (i = 0; i < 8; i++)
      REG_WR32(NV_PGRAPH_CTX_CACHE4(i),   pSwCtxTable[ChID].grCtxCmn.ContextCache4[i]);
    
    for (i = 0; i < 8; i++)
      REG_WR32(NV_PGRAPH_CTX_CACHE5(i),   pSwCtxTable[ChID].grCtxCmn.ContextCache5[i]);
    
    
    REG_WR32(NV_PGRAPH_MONO_COLOR0,       pSwCtxTable[ChID].grCtxCmn.MonoColor0);
    REG_WR32(NV_PGRAPH_BSWIZZLE2,         pSwCtxTable[ChID].grCtxCmn.BufferSwizzle2);        
    REG_WR32(NV_PGRAPH_BSWIZZLE5,         pSwCtxTable[ChID].grCtxCmn.BufferSwizzle5);        
    
    for (i = 0; i < 6; i++)
      REG_WR32(NV_PGRAPH_BOFFSET(i),      pSwCtxTable[ChID].grCtxCmn.BufferOffset[i]);
    
    for (i = 0; i < 6; i++)
      REG_WR32(NV_PGRAPH_BBASE(i),        pSwCtxTable[ChID].grCtxCmn.BufferBase[i]);
    
    for (i = 0; i < 5; i++)
      REG_WR32(NV_PGRAPH_BPITCH(i),       pSwCtxTable[ChID].grCtxCmn.BufferPitch[i]);
    
    for (i = 0; i < 6; i++)
      REG_WR32(NV_PGRAPH_BLIMIT(i),       pSwCtxTable[ChID].grCtxCmn.BufferLimit[i]);
    
    REG_WR32(NV_PGRAPH_CHROMA,            pSwCtxTable[ChID].grCtxCmn.Chroma);
    REG_WR32(NV_PGRAPH_SURFACE,           pSwCtxTable[ChID].grCtxCmn.Surface);        
    REG_WR32(NV_PGRAPH_STATE,             pSwCtxTable[ChID].grCtxCmn.State);        
    REG_WR32(NV_PGRAPH_NOTIFY,            pSwCtxTable[ChID].grCtxCmn.Notify);
    
    REG_WR32(NV_PGRAPH_BPIXEL,            pSwCtxTable[ChID].grCtxCmn.BufferPixel);
    
    REG_WR32(NV_PGRAPH_DMA_PITCH,         pSwCtxTable[ChID].grCtxCmn.DmaPitch);
    
    REG_WR32(NV_PGRAPH_DVD_COLORFMT,      pSwCtxTable[ChID].grCtxCmn.DvdColorFmt);    
    REG_WR32(NV_PGRAPH_SCALED_FORMAT,     pSwCtxTable[ChID].grCtxCmn.ScaledFormat);    
    
    REG_WR32(NV_PGRAPH_PATT_COLOR0,       pSwCtxTable[ChID].grCtxCmn.PatternColor0);    
    REG_WR32(NV_PGRAPH_PATT_COLOR1,       pSwCtxTable[ChID].grCtxCmn.PatternColor1);
    
    REG_WR32(NV_PGRAPH_PATTERN(0),        pSwCtxTable[ChID].grCtxCmn.Pattern[0]);
    REG_WR32(NV_PGRAPH_PATTERN(1),        pSwCtxTable[ChID].grCtxCmn.Pattern[1]);
    REG_WR32(NV_PGRAPH_PATTERN_SHAPE,     pSwCtxTable[ChID].grCtxCmn.PatternShape);
    
    for (i = 0; i < 64; i++)
      REG_WR32(NV_PGRAPH_PATT_COLORRAM(i), pSwCtxTable[ChID].grCtxCmn.PattColorRam[i]);
    
    REG_WR32(NV_PGRAPH_ROP3,              pSwCtxTable[ChID].grCtxCmn.Rop3);
    REG_WR32(NV_PGRAPH_BETA_AND,          pSwCtxTable[ChID].grCtxCmn.BetaAnd);
    REG_WR32(NV_PGRAPH_BETA_PREMULT,      pSwCtxTable[ChID].grCtxCmn.BetaPreMult);
    
    REG_WR32(NV_PGRAPH_STORED_FMT,        pSwCtxTable[ChID].grCtxCmn.StoredFmt);
    
    for (i = 0; i < 10; i++)
      REG_WR32(NV_PGRAPH_ABS_X_RAM(i),    pSwCtxTable[ChID].grCtxCmn.AbsXRam[i]);
    
    for (i = 0; i < 10; i++)
      REG_WR32(NV_PGRAPH_ABS_Y_RAM(i),    pSwCtxTable[ChID].grCtxCmn.AbsYRam[i]);
    
    REG_WR32(NV_PGRAPH_ABS_ICLIP_XMAX,    pSwCtxTable[ChID].grCtxCmn.AbsIClipXMax);
    REG_WR32(NV_PGRAPH_ABS_ICLIP_YMAX,    pSwCtxTable[ChID].grCtxCmn.AbsIClipYMax);
    
    REG_WR32(NV_PGRAPH_ABS_UCLIP_XMIN,    pSwCtxTable[ChID].grCtxCmn.AbsUClipXMin);
    REG_WR32(NV_PGRAPH_ABS_UCLIP_XMAX,    pSwCtxTable[ChID].grCtxCmn.AbsUClipXMax);
    REG_WR32(NV_PGRAPH_ABS_UCLIP_YMIN,    pSwCtxTable[ChID].grCtxCmn.AbsUClipYMin);
    REG_WR32(NV_PGRAPH_ABS_UCLIP_YMAX,    pSwCtxTable[ChID].grCtxCmn.AbsUClipYMax);
    
    REG_WR32(NV_PGRAPH_ABS_UCLIPA_XMIN,   pSwCtxTable[ChID].grCtxCmn.AbsUClipAXMin);
    REG_WR32(NV_PGRAPH_ABS_UCLIPA_XMAX,   pSwCtxTable[ChID].grCtxCmn.AbsUClipAXMax);
    REG_WR32(NV_PGRAPH_ABS_UCLIPA_YMIN,   pSwCtxTable[ChID].grCtxCmn.AbsUClipAYMin);
    REG_WR32(NV_PGRAPH_ABS_UCLIPA_YMAX,   pSwCtxTable[ChID].grCtxCmn.AbsUClipAYMax);
    
    REG_WR32(NV_PGRAPH_SOURCE_COLOR,      pSwCtxTable[ChID].grCtxCmn.SourceColor);
    REG_WR32(NV_PGRAPH_MISC24_0,          pSwCtxTable[ChID].grCtxCmn.Misc24_0);
    
    REG_WR32(NV_PGRAPH_XY_LOGIC_MISC0,    pSwCtxTable[ChID].grCtxCmn.XYLogicMisc0);
    REG_WR32(NV_PGRAPH_XY_LOGIC_MISC1,    pSwCtxTable[ChID].grCtxCmn.XYLogicMisc1);
    REG_WR32(NV_PGRAPH_XY_LOGIC_MISC2,    pSwCtxTable[ChID].grCtxCmn.XYLogicMisc2);
    REG_WR32(NV_PGRAPH_XY_LOGIC_MISC3,    pSwCtxTable[ChID].grCtxCmn.XYLogicMisc3);
    
    REG_WR32(NV_PGRAPH_CLIPX_0,           pSwCtxTable[ChID].grCtxCmn.ClipX0);
    REG_WR32(NV_PGRAPH_CLIPX_1,           pSwCtxTable[ChID].grCtxCmn.ClipX1);
    REG_WR32(NV_PGRAPH_CLIPY_0,           pSwCtxTable[ChID].grCtxCmn.ClipY0);
    REG_WR32(NV_PGRAPH_CLIPY_1,           pSwCtxTable[ChID].grCtxCmn.ClipY1);
    
    REG_WR32(NV_PGRAPH_PASSTHRU_0,        pSwCtxTable[ChID].grCtxCmn.Passthru0);    
    REG_WR32(NV_PGRAPH_PASSTHRU_1,        pSwCtxTable[ChID].grCtxCmn.Passthru1);    
    REG_WR32(NV_PGRAPH_PASSTHRU_2,        pSwCtxTable[ChID].grCtxCmn.Passthru2);    
    
    REG_WR32(NV_PGRAPH_DIMX_TEXTURE,      pSwCtxTable[ChID].grCtxCmn.DimxTexture);    
    REG_WR32(NV_PGRAPH_WDIMX_TEXTURE,     pSwCtxTable[ChID].grCtxCmn.WdimxTexture);    
    
    REG_WR32(NV_PGRAPH_DMA_START_0,       pSwCtxTable[ChID].grCtxCmn.DmaStart0);
    REG_WR32(NV_PGRAPH_DMA_START_1,       pSwCtxTable[ChID].grCtxCmn.DmaStart1);
    REG_WR32(NV_PGRAPH_DMA_LENGTH,        pSwCtxTable[ChID].grCtxCmn.DmaLength);
    REG_WR32(NV_PGRAPH_DMA_MISC,          pSwCtxTable[ChID].grCtxCmn.DmaMisc);
    
    REG_WR32(NV_PGRAPH_MISC24_1,          pSwCtxTable[ChID].grCtxCmn.Misc24_1);
    REG_WR32(NV_PGRAPH_MISC24_2,          pSwCtxTable[ChID].grCtxCmn.Misc24_2);
    REG_WR32(NV_PGRAPH_X_MISC,            pSwCtxTable[ChID].grCtxCmn.XMisc);
    REG_WR32(NV_PGRAPH_Y_MISC,            pSwCtxTable[ChID].grCtxCmn.YMisc);
    REG_WR32(NV_PGRAPH_VALID1,            pSwCtxTable[ChID].grCtxCmn.Valid1);
    
    if (pGrHalPvtInfo->currentObjects3d[ChID] != 0) {
        // 3D stuff
        REG_WR32(NV_PGRAPH_DEBUG_0, 0x0007000); //reset IDX, VTX and CAS
        REG_RD32(NV_PGRAPH_DEBUG_0);
        REG_WR32(NV_PGRAPH_DEBUG_0, 0x0);
        
        REG_WR32(NV_PGRAPH_ANTIALIASING,      pSwCtxTable[ChID].grCtx3d.AntiAliasing);
        REG_WR32(NV_PGRAPH_BLEND,             pSwCtxTable[ChID].grCtx3d.Blend);
        REG_WR32(NV_PGRAPH_BLENDCOLOR,        pSwCtxTable[ChID].grCtx3d.BlendColor);
        REG_WR32(NV_PGRAPH_BORDERCOLOR0,      pSwCtxTable[ChID].grCtx3d.BorderColor0);
        REG_WR32(NV_PGRAPH_BORDERCOLOR1,      pSwCtxTable[ChID].grCtx3d.BorderColor1);
        REG_WR32(NV_PGRAPH_BORDERCOLOR2,      pSwCtxTable[ChID].grCtx3d.BorderColor2);
        REG_WR32(NV_PGRAPH_BORDERCOLOR3,      pSwCtxTable[ChID].grCtx3d.BorderColor3);
        
        REG_WR32(NV_PGRAPH_BUMPMAT00_1,       pSwCtxTable[ChID].grCtx3d.BumpMat00_1);
        REG_WR32(NV_PGRAPH_BUMPMAT00_2,       pSwCtxTable[ChID].grCtx3d.BumpMat00_2);
        REG_WR32(NV_PGRAPH_BUMPMAT00_3,       pSwCtxTable[ChID].grCtx3d.BumpMat00_3);
        REG_WR32(NV_PGRAPH_BUMPMAT01_1,       pSwCtxTable[ChID].grCtx3d.BumpMat01_1);
        REG_WR32(NV_PGRAPH_BUMPMAT01_2,       pSwCtxTable[ChID].grCtx3d.BumpMat01_2);
        REG_WR32(NV_PGRAPH_BUMPMAT01_3,       pSwCtxTable[ChID].grCtx3d.BumpMat01_3);
        REG_WR32(NV_PGRAPH_BUMPMAT10_1,       pSwCtxTable[ChID].grCtx3d.BumpMat10_1);
        REG_WR32(NV_PGRAPH_BUMPMAT10_2,       pSwCtxTable[ChID].grCtx3d.BumpMat10_2);
        REG_WR32(NV_PGRAPH_BUMPMAT10_3,       pSwCtxTable[ChID].grCtx3d.BumpMat10_3);
        REG_WR32(NV_PGRAPH_BUMPMAT11_1,       pSwCtxTable[ChID].grCtx3d.BumpMat11_1);
        REG_WR32(NV_PGRAPH_BUMPMAT11_2,       pSwCtxTable[ChID].grCtx3d.BumpMat11_2);
        REG_WR32(NV_PGRAPH_BUMPMAT11_3,       pSwCtxTable[ChID].grCtx3d.BumpMat11_3);
        
        REG_WR32(NV_PGRAPH_BUMPOFFSET1,       pSwCtxTable[ChID].grCtx3d.BumpOffset1);
        REG_WR32(NV_PGRAPH_BUMPOFFSET2,       pSwCtxTable[ChID].grCtx3d.BumpOffset2);
        REG_WR32(NV_PGRAPH_BUMPOFFSET3,       pSwCtxTable[ChID].grCtx3d.BumpOffset3);
        
        REG_WR32(NV_PGRAPH_BUMPSCALE1,        pSwCtxTable[ChID].grCtx3d.BumpScale1);
        REG_WR32(NV_PGRAPH_BUMPSCALE2,        pSwCtxTable[ChID].grCtx3d.BumpScale2);
        REG_WR32(NV_PGRAPH_BUMPSCALE3,        pSwCtxTable[ChID].grCtx3d.BumpScale3);
        
        REG_WR32(NV_PGRAPH_CLEARRECTX,        pSwCtxTable[ChID].grCtx3d.ClearRectX);
        REG_WR32(NV_PGRAPH_CLEARRECTY,        pSwCtxTable[ChID].grCtx3d.ClearRectY);
        
        REG_WR32(NV_PGRAPH_COLORCLEARVALUE,   pSwCtxTable[ChID].grCtx3d.ColorClearValue);
        
        REG_WR32(NV_PGRAPH_COLORKEYCOLOR0,    pSwCtxTable[ChID].grCtx3d.ColorKeyColor0);
        REG_WR32(NV_PGRAPH_COLORKEYCOLOR1,    pSwCtxTable[ChID].grCtx3d.ColorKeyColor1);
        REG_WR32(NV_PGRAPH_COLORKEYCOLOR2,    pSwCtxTable[ChID].grCtx3d.ColorKeyColor2);
        REG_WR32(NV_PGRAPH_COLORKEYCOLOR3,    pSwCtxTable[ChID].grCtx3d.ColorKeyColor3);
        
        REG_WR32(NV_PGRAPH_COMBINEFACTOR0_0,  pSwCtxTable[ChID].grCtx3d.CombineFactor0_0);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR0_1,  pSwCtxTable[ChID].grCtx3d.CombineFactor0_1);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR0_2,  pSwCtxTable[ChID].grCtx3d.CombineFactor0_2);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR0_3,  pSwCtxTable[ChID].grCtx3d.CombineFactor0_3);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR0_4,  pSwCtxTable[ChID].grCtx3d.CombineFactor0_4);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR0_5,  pSwCtxTable[ChID].grCtx3d.CombineFactor0_5);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR0_6,  pSwCtxTable[ChID].grCtx3d.CombineFactor0_6);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR0_7,  pSwCtxTable[ChID].grCtx3d.CombineFactor0_7);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR1_0,  pSwCtxTable[ChID].grCtx3d.CombineFactor1_0);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR1_1,  pSwCtxTable[ChID].grCtx3d.CombineFactor1_1);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR1_2,  pSwCtxTable[ChID].grCtx3d.CombineFactor1_2);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR1_3,  pSwCtxTable[ChID].grCtx3d.CombineFactor1_3);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR1_4,  pSwCtxTable[ChID].grCtx3d.CombineFactor1_4);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR1_5,  pSwCtxTable[ChID].grCtx3d.CombineFactor1_5);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR1_6,  pSwCtxTable[ChID].grCtx3d.CombineFactor1_6);
        REG_WR32(NV_PGRAPH_COMBINEFACTOR1_7,  pSwCtxTable[ChID].grCtx3d.CombineFactor1_7);
        
        REG_WR32(NV_PGRAPH_COMBINEALPHAI0,    pSwCtxTable[ChID].grCtx3d.CombineAlphaI0);
        REG_WR32(NV_PGRAPH_COMBINEALPHAI1,    pSwCtxTable[ChID].grCtx3d.CombineAlphaI1);
        REG_WR32(NV_PGRAPH_COMBINEALPHAI2,    pSwCtxTable[ChID].grCtx3d.CombineAlphaI2);
        REG_WR32(NV_PGRAPH_COMBINEALPHAI3,    pSwCtxTable[ChID].grCtx3d.CombineAlphaI3);
        REG_WR32(NV_PGRAPH_COMBINEALPHAI4,    pSwCtxTable[ChID].grCtx3d.CombineAlphaI4);
        REG_WR32(NV_PGRAPH_COMBINEALPHAI5,    pSwCtxTable[ChID].grCtx3d.CombineAlphaI5);
        REG_WR32(NV_PGRAPH_COMBINEALPHAI6,    pSwCtxTable[ChID].grCtx3d.CombineAlphaI6);
        REG_WR32(NV_PGRAPH_COMBINEALPHAI7,    pSwCtxTable[ChID].grCtx3d.CombineAlphaI7);
        REG_WR32(NV_PGRAPH_COMBINEALPHAO0,    pSwCtxTable[ChID].grCtx3d.CombineAlphaO0);
        REG_WR32(NV_PGRAPH_COMBINEALPHAO1,    pSwCtxTable[ChID].grCtx3d.CombineAlphaO1);
        REG_WR32(NV_PGRAPH_COMBINEALPHAO2,    pSwCtxTable[ChID].grCtx3d.CombineAlphaO2);
        REG_WR32(NV_PGRAPH_COMBINEALPHAO3,    pSwCtxTable[ChID].grCtx3d.CombineAlphaO3);
        REG_WR32(NV_PGRAPH_COMBINEALPHAO4,    pSwCtxTable[ChID].grCtx3d.CombineAlphaO4);
        REG_WR32(NV_PGRAPH_COMBINEALPHAO5,    pSwCtxTable[ChID].grCtx3d.CombineAlphaO5);
        REG_WR32(NV_PGRAPH_COMBINEALPHAO6,    pSwCtxTable[ChID].grCtx3d.CombineAlphaO6);
        REG_WR32(NV_PGRAPH_COMBINEALPHAO7,    pSwCtxTable[ChID].grCtx3d.CombineAlphaO7);
        
        REG_WR32(NV_PGRAPH_COMBINECOLORI0,    pSwCtxTable[ChID].grCtx3d.CombineColorI0);
        REG_WR32(NV_PGRAPH_COMBINECOLORI1,    pSwCtxTable[ChID].grCtx3d.CombineColorI1);
        REG_WR32(NV_PGRAPH_COMBINECOLORI2,    pSwCtxTable[ChID].grCtx3d.CombineColorI2);
        REG_WR32(NV_PGRAPH_COMBINECOLORI3,    pSwCtxTable[ChID].grCtx3d.CombineColorI3);
        REG_WR32(NV_PGRAPH_COMBINECOLORI4,    pSwCtxTable[ChID].grCtx3d.CombineColorI4);
        REG_WR32(NV_PGRAPH_COMBINECOLORI5,    pSwCtxTable[ChID].grCtx3d.CombineColorI5);
        REG_WR32(NV_PGRAPH_COMBINECOLORI6,    pSwCtxTable[ChID].grCtx3d.CombineColorI6);
        REG_WR32(NV_PGRAPH_COMBINECOLORI7,    pSwCtxTable[ChID].grCtx3d.CombineColorI7);
        REG_WR32(NV_PGRAPH_COMBINECOLORO0,    pSwCtxTable[ChID].grCtx3d.CombineColorO0);
        REG_WR32(NV_PGRAPH_COMBINECOLORO1,    pSwCtxTable[ChID].grCtx3d.CombineColorO1);
        REG_WR32(NV_PGRAPH_COMBINECOLORO2,    pSwCtxTable[ChID].grCtx3d.CombineColorO2);
        REG_WR32(NV_PGRAPH_COMBINECOLORO3,    pSwCtxTable[ChID].grCtx3d.CombineColorO3);
        REG_WR32(NV_PGRAPH_COMBINECOLORO4,    pSwCtxTable[ChID].grCtx3d.CombineColorO4);
        REG_WR32(NV_PGRAPH_COMBINECOLORO5,    pSwCtxTable[ChID].grCtx3d.CombineColorO5);
        REG_WR32(NV_PGRAPH_COMBINECOLORO6,    pSwCtxTable[ChID].grCtx3d.CombineColorO6);
        REG_WR32(NV_PGRAPH_COMBINECOLORO7,    pSwCtxTable[ChID].grCtx3d.CombineColorO7);
        
        REG_WR32(NV_PGRAPH_COMBINECTL,        pSwCtxTable[ChID].grCtx3d.CombineCtl);
        
        REG_WR32(NV_PGRAPH_COMBINESPECFOG0,   pSwCtxTable[ChID].grCtx3d.CombineSpecFog0);
        REG_WR32(NV_PGRAPH_COMBINESPECFOG1,   pSwCtxTable[ChID].grCtx3d.CombineSpecFog1);
        
        REG_WR32(NV_PGRAPH_CONTROL_0,         pSwCtxTable[ChID].grCtx3d.Control0);
        REG_WR32(NV_PGRAPH_CONTROL_1,         pSwCtxTable[ChID].grCtx3d.Control1);
        REG_WR32(NV_PGRAPH_CONTROL_2,         pSwCtxTable[ChID].grCtx3d.Control2);
        REG_WR32(NV_PGRAPH_CONTROL_3,         pSwCtxTable[ChID].grCtx3d.Control3);
        
        REG_WR32(NV_PGRAPH_FOGCOLOR,          pSwCtxTable[ChID].grCtx3d.FogColor);
        REG_WR32(NV_PGRAPH_FOGPARAM0,         pSwCtxTable[ChID].grCtx3d.FogParam0);
        REG_WR32(NV_PGRAPH_FOGPARAM1,         pSwCtxTable[ChID].grCtx3d.FogParam1);
        REG_WR32(NV_PGRAPH_POINTSIZE,         pSwCtxTable[ChID].grCtx3d.PointSize);
        REG_WR32(NV_PGRAPH_SETUPRASTER,       pSwCtxTable[ChID].grCtx3d.SetupRaster);
        
        REG_WR32(NV_PGRAPH_SHADERCLIPMODE,    pSwCtxTable[ChID].grCtx3d.ShaderClipMode);
        REG_WR32(NV_PGRAPH_SHADERCTL,         pSwCtxTable[ChID].grCtx3d.ShaderCtl);
        REG_WR32(NV_PGRAPH_SHADERPROG,        pSwCtxTable[ChID].grCtx3d.ShaderProg);
        
        REG_WR32(NV_PGRAPH_SEMAPHOREOFFSET,   pSwCtxTable[ChID].grCtx3d.SemaphoreOffset);
        REG_WR32(NV_PGRAPH_SHADOWCTL,         pSwCtxTable[ChID].grCtx3d.ShadowCtl);
        REG_WR32(NV_PGRAPH_SHADOWZSLOPETHRESHOLD, pSwCtxTable[ChID].grCtx3d.ShadowZSlopeThreshold);
        
        REG_WR32(NV_PGRAPH_SPECFOGFACTOR0,    pSwCtxTable[ChID].grCtx3d.SpecFogFactor0);
        REG_WR32(NV_PGRAPH_SPECFOGFACTOR1,    pSwCtxTable[ChID].grCtx3d.SpecFogFactor1);
        
        REG_WR32(NV_PGRAPH_SURFACECLIPX,      pSwCtxTable[ChID].grCtx3d.SurfaceClipX);
        REG_WR32(NV_PGRAPH_SURFACECLIPY,      pSwCtxTable[ChID].grCtx3d.SurfaceClipY);
        
        REG_WR32(NV_PGRAPH_TEXADDRESS0,       pSwCtxTable[ChID].grCtx3d.TexAddress0);
        REG_WR32(NV_PGRAPH_TEXADDRESS1,       pSwCtxTable[ChID].grCtx3d.TexAddress1);
        REG_WR32(NV_PGRAPH_TEXADDRESS2,       pSwCtxTable[ChID].grCtx3d.TexAddress2);
        REG_WR32(NV_PGRAPH_TEXADDRESS3,       pSwCtxTable[ChID].grCtx3d.TexAddress3);
        
        REG_WR32(NV_PGRAPH_TEXCTL0_0,         pSwCtxTable[ChID].grCtx3d.TexCtl0_0);
        REG_WR32(NV_PGRAPH_TEXCTL0_1,         pSwCtxTable[ChID].grCtx3d.TexCtl0_1);
        REG_WR32(NV_PGRAPH_TEXCTL0_2,         pSwCtxTable[ChID].grCtx3d.TexCtl0_2);
        REG_WR32(NV_PGRAPH_TEXCTL0_3,         pSwCtxTable[ChID].grCtx3d.TexCtl0_3);
        
        REG_WR32(NV_PGRAPH_TEXCTL1_0,         pSwCtxTable[ChID].grCtx3d.TexCtl1_0);
        REG_WR32(NV_PGRAPH_TEXCTL1_1,         pSwCtxTable[ChID].grCtx3d.TexCtl1_1);
        REG_WR32(NV_PGRAPH_TEXCTL1_2,         pSwCtxTable[ChID].grCtx3d.TexCtl1_2);
        REG_WR32(NV_PGRAPH_TEXCTL1_3,         pSwCtxTable[ChID].grCtx3d.TexCtl1_3);
        
        REG_WR32(NV_PGRAPH_TEXCTL2_0,         pSwCtxTable[ChID].grCtx3d.TexCtl2_0);
        REG_WR32(NV_PGRAPH_TEXCTL2_1,         pSwCtxTable[ChID].grCtx3d.TexCtl2_1);
        
        REG_WR32(NV_PGRAPH_TEXFILTER0,        pSwCtxTable[ChID].grCtx3d.TexFilter0);
        REG_WR32(NV_PGRAPH_TEXFILTER1,        pSwCtxTable[ChID].grCtx3d.TexFilter1);
        REG_WR32(NV_PGRAPH_TEXFILTER2,        pSwCtxTable[ChID].grCtx3d.TexFilter2);
        REG_WR32(NV_PGRAPH_TEXFILTER3,        pSwCtxTable[ChID].grCtx3d.TexFilter3);
        
        REG_WR32(NV_PGRAPH_TEXFMT0,           pSwCtxTable[ChID].grCtx3d.TexFormat0);
        REG_WR32(NV_PGRAPH_TEXFMT1,           pSwCtxTable[ChID].grCtx3d.TexFormat1);
        REG_WR32(NV_PGRAPH_TEXFMT2,           pSwCtxTable[ChID].grCtx3d.TexFormat2);
        REG_WR32(NV_PGRAPH_TEXFMT3,           pSwCtxTable[ChID].grCtx3d.TexFormat3);
        
        REG_WR32(NV_PGRAPH_TEXIMAGERECT0,     pSwCtxTable[ChID].grCtx3d.TexImageRect0);
        REG_WR32(NV_PGRAPH_TEXIMAGERECT1,     pSwCtxTable[ChID].grCtx3d.TexImageRect1);
        REG_WR32(NV_PGRAPH_TEXIMAGERECT2,     pSwCtxTable[ChID].grCtx3d.TexImageRect2);
        REG_WR32(NV_PGRAPH_TEXIMAGERECT3,     pSwCtxTable[ChID].grCtx3d.TexImageRect3);
        
        REG_WR32(NV_PGRAPH_TEXOFFSET0,        pSwCtxTable[ChID].grCtx3d.TexOffset0);
        REG_WR32(NV_PGRAPH_TEXOFFSET1,        pSwCtxTable[ChID].grCtx3d.TexOffset1);
        REG_WR32(NV_PGRAPH_TEXOFFSET2,        pSwCtxTable[ChID].grCtx3d.TexOffset2);
        REG_WR32(NV_PGRAPH_TEXOFFSET3,        pSwCtxTable[ChID].grCtx3d.TexOffset3);
        
        REG_WR32(NV_PGRAPH_TEXPALETTE0,       pSwCtxTable[ChID].grCtx3d.TexPallete0);
        REG_WR32(NV_PGRAPH_TEXPALETTE1,       pSwCtxTable[ChID].grCtx3d.TexPallete1);
        REG_WR32(NV_PGRAPH_TEXPALETTE2,       pSwCtxTable[ChID].grCtx3d.TexPallete2);
        REG_WR32(NV_PGRAPH_TEXPALETTE3,       pSwCtxTable[ChID].grCtx3d.TexPallete3);
        
        REG_WR32(NV_PGRAPH_WINDOWCLIPX0,      pSwCtxTable[ChID].grCtx3d.WindowClipX[0]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPX1,      pSwCtxTable[ChID].grCtx3d.WindowClipX[1]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPX2,      pSwCtxTable[ChID].grCtx3d.WindowClipX[2]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPX3,      pSwCtxTable[ChID].grCtx3d.WindowClipX[3]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPX4,      pSwCtxTable[ChID].grCtx3d.WindowClipX[4]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPX5,      pSwCtxTable[ChID].grCtx3d.WindowClipX[5]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPX6,      pSwCtxTable[ChID].grCtx3d.WindowClipX[6]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPX7,      pSwCtxTable[ChID].grCtx3d.WindowClipX[7]);
        
        REG_WR32(NV_PGRAPH_WINDOWCLIPY0,      pSwCtxTable[ChID].grCtx3d.WindowClipY[0]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPY1,      pSwCtxTable[ChID].grCtx3d.WindowClipY[1]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPY2,      pSwCtxTable[ChID].grCtx3d.WindowClipY[2]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPY3,      pSwCtxTable[ChID].grCtx3d.WindowClipY[3]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPY4,      pSwCtxTable[ChID].grCtx3d.WindowClipY[4]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPY5,      pSwCtxTable[ChID].grCtx3d.WindowClipY[5]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPY6,      pSwCtxTable[ChID].grCtx3d.WindowClipY[6]);
        REG_WR32(NV_PGRAPH_WINDOWCLIPY7,      pSwCtxTable[ChID].grCtx3d.WindowClipY[7]);
        
        REG_WR32(NV_PGRAPH_ZCOMPRESSOCCLUDE,  pSwCtxTable[ChID].grCtx3d.ZCompressOcclude);
        REG_WR32(NV_PGRAPH_ZSTENCILCLEARVALUE,pSwCtxTable[ChID].grCtx3d.ZStencilClearValue);
        
        REG_WR32(NV_PGRAPH_ZCLIPMAX,          pSwCtxTable[ChID].grCtx3d.ZClipMax);
        REG_WR32(NV_PGRAPH_ZCLIPMIN,          pSwCtxTable[ChID].grCtx3d.ZClipMin);
        
        REG_WR32(NV_PGRAPH_CONTEXTDMAA,       pSwCtxTable[ChID].grCtx3d.ContextDmaA);
        REG_WR32(NV_PGRAPH_CONTEXTDMAB,       pSwCtxTable[ChID].grCtx3d.ContextDmaB);
        REG_WR32(NV_PGRAPH_CONTEXTVTXA,       pSwCtxTable[ChID].grCtx3d.ContextDmaVtxA);
        REG_WR32(NV_PGRAPH_CONTEXTVTXB,       pSwCtxTable[ChID].grCtx3d.ContextDmaVtxB);
        
        REG_WR32(NV_PGRAPH_ZOFFSETBIAS,       pSwCtxTable[ChID].grCtx3d.ZOffsetBias);
        REG_WR32(NV_PGRAPH_ZOFFSETFACTOR,     pSwCtxTable[ChID].grCtx3d.ZOffsetFactor);
        
        REG_WR32(NV_PGRAPH_EYEVEC0,           pSwCtxTable[ChID].grCtx3d.Eyevec0);
        REG_WR32(NV_PGRAPH_EYEVEC1,           pSwCtxTable[ChID].grCtx3d.Eyevec1);
        REG_WR32(NV_PGRAPH_EYEVEC2,           pSwCtxTable[ChID].grCtx3d.Eyevec2);
        
        REG_WR32(NV_PGRAPH_SHADOW,            pSwCtxTable[ChID].grCtx3d.Shadow);
        REG_WR32(NV_PGRAPH_FD_DATA,           pSwCtxTable[ChID].grCtx3d.FdData);
        REG_WR32(NV_PGRAPH_FD_SWATCH,         pSwCtxTable[ChID].grCtx3d.FdSwatch);
        REG_WR32(NV_PGRAPH_FD_EXTRAS,         pSwCtxTable[ChID].grCtx3d.FdExtras);
        REG_WR32(NV_PGRAPH_EMISSION_BACKUP_0, pSwCtxTable[ChID].grCtx3d.Emission0);
        REG_WR32(NV_PGRAPH_EMISSION_BACKUP_1, pSwCtxTable[ChID].grCtx3d.Emission1);
        REG_WR32(NV_PGRAPH_EMISSION_BACKUP_2, pSwCtxTable[ChID].grCtx3d.Emission2);
        REG_WR32(NV_PGRAPH_SCENE_AMBIENT_BACKUP_0, pSwCtxTable[ChID].grCtx3d.SceneAmb0);
        REG_WR32(NV_PGRAPH_SCENE_AMBIENT_BACKUP_1, pSwCtxTable[ChID].grCtx3d.SceneAmb1);
        REG_WR32(NV_PGRAPH_SCENE_AMBIENT_BACKUP_2, pSwCtxTable[ChID].grCtx3d.SceneAmb2);
        REG_WR32(NV_PGRAPH_GETSTATE,          pSwCtxTable[ChID].grCtx3d.ContextDmaGetState);
        REG_WR32(NV_PGRAPH_BEGINPATCH0,       pSwCtxTable[ChID].grCtx3d.BeginPatch0);
        REG_WR32(NV_PGRAPH_BEGINPATCH1,       pSwCtxTable[ChID].grCtx3d.BeginPatch1);
        REG_WR32(NV_PGRAPH_BEGINPATCH2,       pSwCtxTable[ChID].grCtx3d.BeginPatch2);
        REG_WR32(NV_PGRAPH_BEGINPATCH3,       pSwCtxTable[ChID].grCtx3d.BeginPatch3);
        REG_WR32(NV_PGRAPH_CURVE,             pSwCtxTable[ChID].grCtx3d.BeginCurve);
        REG_WR32(NV_PGRAPH_BEGINTRANS0,       pSwCtxTable[ChID].grCtx3d.BeginTrans0);
        REG_WR32(NV_PGRAPH_BEGINTRANS1,       pSwCtxTable[ChID].grCtx3d.BeginTrans1);
        REG_WR32(NV_PGRAPH_CSV0_D,            pSwCtxTable[ChID].grCtx3d.Csv0_D);
        REG_WR32(NV_PGRAPH_CSV0_C,            pSwCtxTable[ChID].grCtx3d.Csv0_C);
        REG_WR32(NV_PGRAPH_CSV1_B,            pSwCtxTable[ChID].grCtx3d.Csv1_B);
        REG_WR32(NV_PGRAPH_CSV1_A,            pSwCtxTable[ChID].grCtx3d.Csv1_A);
        REG_WR32(NV_PGRAPH_CHEOPS_OFFSET,     pSwCtxTable[ChID].grCtx3d.CheopsOffset);
        
        do {
          status = REG_RD32(NV_PGRAPH_STATUS);
        } while (status);
        
        // wait for the pipe to go idle 
        HAL_GR_IDLE(pHalHwInfo);

        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, RDI_ADDR_ZERO, IDX_FMT_COUNT, pSwCtxTable[ChID].grCtxPipe.IdxFmt);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, RDI_ADDR_ZERO, CAS0_COUNT, pSwCtxTable[ChID].grCtxPipe.Cas0);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, RDI_ADDR_ZERO, CAS1_COUNT, pSwCtxTable[ChID].grCtxPipe.Cas1);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, RDI_ADDR_ZERO, CAS2_COUNT, pSwCtxTable[ChID].grCtxPipe.Cas2);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_ASSM_STATE, RDI_ADDR_ZERO, ASSM_STATE_COUNT, pSwCtxTable[ChID].grCtxPipe.AssmState);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_CACHE0, RDI_ADDR_ZERO, IDX_CACHE0_COUNT, pSwCtxTable[ChID].grCtxPipe.IdxCache0);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_CACHE1, RDI_ADDR_ZERO, IDX_CACHE1_COUNT, pSwCtxTable[ChID].grCtxPipe.IdxCache1);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_CACHE2, RDI_ADDR_ZERO, IDX_CACHE2_COUNT, pSwCtxTable[ChID].grCtxPipe.IdxCache2);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_CACHE3, RDI_ADDR_ZERO, IDX_CACHE3_COUNT, pSwCtxTable[ChID].grCtxPipe.IdxCache3);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_VTX_FILE0, RDI_ADDR_ZERO, VTX_FILE0_COUNT, pSwCtxTable[ChID].grCtxPipe.VtxFile0);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_VTX_FILE1, RDI_ADDR_ZERO, VTX_FILE1_COUNT, pSwCtxTable[ChID].grCtxPipe.VtxFile1);
        
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_P, RDI_ADDR_ZERO, XL_P_COUNT, pSwCtxTable[ChID].grCtxPipe.XlP);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_XFCTX, RDI_ADDR_ZERO, XL_XFCTX_COUNT, pSwCtxTable[ChID].grCtxPipe.XlXfCtx);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_LTCTX, RDI_ADDR_ZERO, XL_LTCTX_COUNT, pSwCtxTable[ChID].grCtxPipe.XlLtCtx);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_LTC, RDI_ADDR_ZERO, XL_LTC_COUNT, pSwCtxTable[ChID].grCtxPipe.XlLtc);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, RDI_ADDR_ZERO, XL_VAB_COUNT, pSwCtxTable[ChID].grCtxPipe.XlVab);
        
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_CRSTR_STIPP, RDI_ADDR_ZERO, CRSTR_STIPP_COUNT, pSwCtxTable[ChID].grCtxPipe.CrstrStipp);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_FD_CTRL, RDI_ADDR_ZERO, FD_CTRL_COUNT, pSwCtxTable[ChID].grCtxPipe.FdCtrl);
        grLoadRdiContext(pHalHwInfo, RDI_RAMSEL_ROP_REGS, RDI_ADDR_ZERO, ROP_REGS_COUNT, pSwCtxTable[ChID].grCtxPipe.RopRegs);
    }
    
    
    REG_WR32(NV_PGRAPH_CTX_CONTROL,
        DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME,   _33US)
        | DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME,         _NOT_EXPIRED)
        | DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID,         _VALID)
        | DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING,    _IDLE)
        | DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE,       _ENABLED));
                      
    
    FLD_WR_DRF_NUM(_PGRAPH, _CTX_USER, _CHID, ChID);

    REG_WR32(NV_PGRAPH_FFINTFC_ST2, REG_RD32(NV_PGRAPH_FFINTFC_ST2) & 0xCFFFFFFF);
    
    REG_WR_DRF_DEF(_PGRAPH,_FIFO,_ACCESS,_ENABLED);

    return (RM_OK);
}

RM_STATUS nvHalGrUnloadChannelContext_NV20(PHALHWINFO pHalHwInfo, U032 ChID, PGRAPHICSCHANNEL_NV20 pSwCtxTable)
{
    PGRHALINFO_NV20 pGrHalPvtInfo = (PGRHALINFO_NV20) pHalHwInfo->pGrHalPvtInfo;

    U032 ctxControl;
    int i;
    
    GR_PRINTF((DBG_LEVEL_INFO, "NVRM: nvHalGrUnloadChannelContext_NV20\r\n"));

    HAL_GR_IDLE(pHalHwInfo);

    ctxControl = REG_RD32(NV_PGRAPH_CTX_CONTROL);
    
    if (DRF_VAL(_PGRAPH,_CTX_CONTROL,_CHID, ctxControl) == NV_PGRAPH_CTX_CONTROL_CHID_INVALID)
    {
        GR_PRINTF((DBG_LEVEL_INFO, "NVRM: grUnloadCurrentChannelContext() current chid is invalid\n"));
        return (RM_OK);
    }
    
    pSwCtxTable->grCtxCmn.ContextUser       = REG_RD32(NV_PGRAPH_CTX_USER);
    
    //
    // Unload current context registers.
    //
    pSwCtxTable->grCtxCmn.ContextSwitch1    = REG_RD32(NV_PGRAPH_CTX_SWITCH1);
    pSwCtxTable->grCtxCmn.ContextSwitch2    = REG_RD32(NV_PGRAPH_CTX_SWITCH2);
    pSwCtxTable->grCtxCmn.ContextSwitch3    = REG_RD32(NV_PGRAPH_CTX_SWITCH3);        
    pSwCtxTable->grCtxCmn.ContextSwitch4    = REG_RD32(NV_PGRAPH_CTX_SWITCH4);
    pSwCtxTable->grCtxCmn.ContextSwitch5    = REG_RD32(NV_PGRAPH_CTX_SWITCH5);
    
    for (i = 0; i < 8; i++)
        pSwCtxTable->grCtxCmn.ContextCache1[i] = REG_RD32(NV_PGRAPH_CTX_CACHE1(i));
    
    for (i = 0; i < 8; i++)
        pSwCtxTable->grCtxCmn.ContextCache2[i] = REG_RD32(NV_PGRAPH_CTX_CACHE2(i));
    
    for (i = 0; i < 8; i++)
        pSwCtxTable->grCtxCmn.ContextCache3[i] = REG_RD32(NV_PGRAPH_CTX_CACHE3(i));
    
    for (i = 0; i < 8; i++)
        pSwCtxTable->grCtxCmn.ContextCache4[i] = REG_RD32(NV_PGRAPH_CTX_CACHE4(i));
    
    for (i = 0; i < 8; i++)
        pSwCtxTable->grCtxCmn.ContextCache5[i] = REG_RD32(NV_PGRAPH_CTX_CACHE5(i));
    
    pSwCtxTable->grCtxCmn.MonoColor0          = REG_RD32(NV_PGRAPH_MONO_COLOR0);
    pSwCtxTable->grCtxCmn.BufferSwizzle2      = REG_RD32(NV_PGRAPH_BSWIZZLE2);        
    pSwCtxTable->grCtxCmn.BufferSwizzle5      = REG_RD32(NV_PGRAPH_BSWIZZLE5);        
    
    for (i = 0; i < 6; i++)
        pSwCtxTable->grCtxCmn.BufferOffset[i] = REG_RD32(NV_PGRAPH_BOFFSET(i));
    
    for (i = 0; i < 6; i++)
        pSwCtxTable->grCtxCmn.BufferBase[i]   = REG_RD32(NV_PGRAPH_BBASE(i));
    
    for (i = 0; i < 5; i++)
        pSwCtxTable->grCtxCmn.BufferPitch[i]  = REG_RD32(NV_PGRAPH_BPITCH(i));
    
    for (i = 0; i < 6; i++)
        pSwCtxTable->grCtxCmn.BufferLimit[i]  = REG_RD32(NV_PGRAPH_BLIMIT(i));
    
    pSwCtxTable->grCtxCmn.Chroma              = REG_RD32(NV_PGRAPH_CHROMA);
    
    pSwCtxTable->grCtxCmn.Surface             = REG_RD32(NV_PGRAPH_SURFACE);        
    pSwCtxTable->grCtxCmn.State               = REG_RD32(NV_PGRAPH_STATE);        
    pSwCtxTable->grCtxCmn.Notify              = REG_RD32(NV_PGRAPH_NOTIFY);
    pSwCtxTable->grCtxCmn.BufferPixel         = REG_RD32(NV_PGRAPH_BPIXEL);
    
    pSwCtxTable->grCtxCmn.DmaPitch            = REG_RD32(NV_PGRAPH_DMA_PITCH);
    pSwCtxTable->grCtxCmn.DvdColorFmt         = REG_RD32(NV_PGRAPH_DVD_COLORFMT);    
    pSwCtxTable->grCtxCmn.ScaledFormat        = REG_RD32(NV_PGRAPH_SCALED_FORMAT);    
    
    pSwCtxTable->grCtxCmn.PatternColor0       = REG_RD32(NV_PGRAPH_PATT_COLOR0);    
    pSwCtxTable->grCtxCmn.PatternColor1       = REG_RD32(NV_PGRAPH_PATT_COLOR1);
    pSwCtxTable->grCtxCmn.Pattern[0]          = REG_RD32(NV_PGRAPH_PATTERN(0));
    pSwCtxTable->grCtxCmn.Pattern[1]          = REG_RD32(NV_PGRAPH_PATTERN(1));
    pSwCtxTable->grCtxCmn.PatternShape        = REG_RD32(NV_PGRAPH_PATTERN_SHAPE);
    
    for (i = 0; i < 64; i++)
        pSwCtxTable->grCtxCmn.PattColorRam[i] = REG_RD32(NV_PGRAPH_PATT_COLORRAM(i));
    
    
    pSwCtxTable->grCtxCmn.Rop3                = REG_RD32(NV_PGRAPH_ROP3);
    pSwCtxTable->grCtxCmn.BetaAnd             = REG_RD32(NV_PGRAPH_BETA_AND);
    pSwCtxTable->grCtxCmn.BetaPreMult         = REG_RD32(NV_PGRAPH_BETA_PREMULT);
    pSwCtxTable->grCtxCmn.StoredFmt           = REG_RD32(NV_PGRAPH_STORED_FMT);
      
    for (i = 0; i < 10; i++)
      pSwCtxTable->grCtxCmn.AbsXRam[i]        = REG_RD32(NV_PGRAPH_ABS_X_RAM(i));
    
    for (i = 0; i < 10; i++)
      pSwCtxTable->grCtxCmn.AbsYRam[i]        = REG_RD32(NV_PGRAPH_ABS_Y_RAM(i));
    
    pSwCtxTable->grCtxCmn.AbsIClipXMax        = REG_RD32(NV_PGRAPH_ABS_ICLIP_XMAX);
    pSwCtxTable->grCtxCmn.AbsIClipYMax        = REG_RD32(NV_PGRAPH_ABS_ICLIP_YMAX);
    
    pSwCtxTable->grCtxCmn.AbsUClipXMin        = REG_RD32(NV_PGRAPH_ABS_UCLIP_XMIN);
    pSwCtxTable->grCtxCmn.AbsUClipXMax        = REG_RD32(NV_PGRAPH_ABS_UCLIP_XMAX);
    pSwCtxTable->grCtxCmn.AbsUClipYMin        = REG_RD32(NV_PGRAPH_ABS_UCLIP_YMIN);
    pSwCtxTable->grCtxCmn.AbsUClipYMax        = REG_RD32(NV_PGRAPH_ABS_UCLIP_YMAX);
    
    pSwCtxTable->grCtxCmn.AbsUClipAXMin       = REG_RD32(NV_PGRAPH_ABS_UCLIPA_XMIN);
    pSwCtxTable->grCtxCmn.AbsUClipAXMax       = REG_RD32(NV_PGRAPH_ABS_UCLIPA_XMAX);
    pSwCtxTable->grCtxCmn.AbsUClipAYMin       = REG_RD32(NV_PGRAPH_ABS_UCLIPA_YMIN);
    pSwCtxTable->grCtxCmn.AbsUClipAYMax       = REG_RD32(NV_PGRAPH_ABS_UCLIPA_YMAX);
    
    pSwCtxTable->grCtxCmn.SourceColor         = REG_RD32(NV_PGRAPH_SOURCE_COLOR);
    
    pSwCtxTable->grCtxCmn.Misc24_0            = REG_RD32(NV_PGRAPH_MISC24_0);
    
    pSwCtxTable->grCtxCmn.XYLogicMisc0        = REG_RD32(NV_PGRAPH_XY_LOGIC_MISC0);
    pSwCtxTable->grCtxCmn.XYLogicMisc1        = REG_RD32(NV_PGRAPH_XY_LOGIC_MISC1);
    pSwCtxTable->grCtxCmn.XYLogicMisc2        = REG_RD32(NV_PGRAPH_XY_LOGIC_MISC2);
    pSwCtxTable->grCtxCmn.XYLogicMisc3        = REG_RD32(NV_PGRAPH_XY_LOGIC_MISC3);
    
    pSwCtxTable->grCtxCmn.ClipX0              = REG_RD32(NV_PGRAPH_CLIPX_0);
    pSwCtxTable->grCtxCmn.ClipX1              = REG_RD32(NV_PGRAPH_CLIPX_1);
    pSwCtxTable->grCtxCmn.ClipY0              = REG_RD32(NV_PGRAPH_CLIPY_0);
    pSwCtxTable->grCtxCmn.ClipY1              = REG_RD32(NV_PGRAPH_CLIPY_1);
    
    pSwCtxTable->grCtxCmn.Passthru0           = REG_RD32(NV_PGRAPH_PASSTHRU_0);    
    pSwCtxTable->grCtxCmn.Passthru1           = REG_RD32(NV_PGRAPH_PASSTHRU_1);    
    pSwCtxTable->grCtxCmn.Passthru2           = REG_RD32(NV_PGRAPH_PASSTHRU_2);    
    
    pSwCtxTable->grCtxCmn.DimxTexture         = REG_RD32(NV_PGRAPH_DIMX_TEXTURE);    
    pSwCtxTable->grCtxCmn.WdimxTexture        = REG_RD32(NV_PGRAPH_WDIMX_TEXTURE);    
    
    pSwCtxTable->grCtxCmn.DmaStart0           = REG_RD32(NV_PGRAPH_DMA_START_0);
    pSwCtxTable->grCtxCmn.DmaStart1           = REG_RD32(NV_PGRAPH_DMA_START_1);
    pSwCtxTable->grCtxCmn.DmaLength           = REG_RD32(NV_PGRAPH_DMA_LENGTH);
    pSwCtxTable->grCtxCmn.DmaMisc             = REG_RD32(NV_PGRAPH_DMA_MISC);
    
    pSwCtxTable->grCtxCmn.Misc24_1            = REG_RD32(NV_PGRAPH_MISC24_1);
    pSwCtxTable->grCtxCmn.Misc24_2            = REG_RD32(NV_PGRAPH_MISC24_2);
    pSwCtxTable->grCtxCmn.XMisc               = REG_RD32(NV_PGRAPH_X_MISC);
    pSwCtxTable->grCtxCmn.YMisc               = REG_RD32(NV_PGRAPH_Y_MISC);
    pSwCtxTable->grCtxCmn.Valid1              = REG_RD32(NV_PGRAPH_VALID1);
      
    if (pGrHalPvtInfo->currentObjects3d[ChID] != 0) {
        // 3D stuff
        pSwCtxTable->grCtx3d.AntiAliasing        = REG_RD32(NV_PGRAPH_ANTIALIASING);
        pSwCtxTable->grCtx3d.Blend               = REG_RD32(NV_PGRAPH_BLEND);
        pSwCtxTable->grCtx3d.BlendColor          = REG_RD32(NV_PGRAPH_BLENDCOLOR);
        pSwCtxTable->grCtx3d.BorderColor0        = REG_RD32(NV_PGRAPH_BORDERCOLOR0);
        pSwCtxTable->grCtx3d.BorderColor1        = REG_RD32(NV_PGRAPH_BORDERCOLOR1);
        pSwCtxTable->grCtx3d.BorderColor2        = REG_RD32(NV_PGRAPH_BORDERCOLOR2);
        pSwCtxTable->grCtx3d.BorderColor3        = REG_RD32(NV_PGRAPH_BORDERCOLOR3);
        
        pSwCtxTable->grCtx3d.BumpMat00_1         = REG_RD32(NV_PGRAPH_BUMPMAT00_1);
        pSwCtxTable->grCtx3d.BumpMat00_2         = REG_RD32(NV_PGRAPH_BUMPMAT00_2);
        pSwCtxTable->grCtx3d.BumpMat00_3         = REG_RD32(NV_PGRAPH_BUMPMAT00_3);
        pSwCtxTable->grCtx3d.BumpMat01_1         = REG_RD32(NV_PGRAPH_BUMPMAT01_1);
        pSwCtxTable->grCtx3d.BumpMat01_2         = REG_RD32(NV_PGRAPH_BUMPMAT01_2);
        pSwCtxTable->grCtx3d.BumpMat01_3         = REG_RD32(NV_PGRAPH_BUMPMAT01_3);
        pSwCtxTable->grCtx3d.BumpMat10_1         = REG_RD32(NV_PGRAPH_BUMPMAT10_1);
        pSwCtxTable->grCtx3d.BumpMat10_2         = REG_RD32(NV_PGRAPH_BUMPMAT10_2);
        pSwCtxTable->grCtx3d.BumpMat10_3         = REG_RD32(NV_PGRAPH_BUMPMAT10_3);
        pSwCtxTable->grCtx3d.BumpMat11_1         = REG_RD32(NV_PGRAPH_BUMPMAT11_1);
        pSwCtxTable->grCtx3d.BumpMat11_2         = REG_RD32(NV_PGRAPH_BUMPMAT11_2);
        pSwCtxTable->grCtx3d.BumpMat11_3         = REG_RD32(NV_PGRAPH_BUMPMAT11_3);
        
        pSwCtxTable->grCtx3d.BumpOffset1         = REG_RD32(NV_PGRAPH_BUMPOFFSET1);
        pSwCtxTable->grCtx3d.BumpOffset2         = REG_RD32(NV_PGRAPH_BUMPOFFSET2);
        pSwCtxTable->grCtx3d.BumpOffset3         = REG_RD32(NV_PGRAPH_BUMPOFFSET3);
        
        pSwCtxTable->grCtx3d.BumpScale1          = REG_RD32(NV_PGRAPH_BUMPSCALE1);
        pSwCtxTable->grCtx3d.BumpScale2          = REG_RD32(NV_PGRAPH_BUMPSCALE2);
        pSwCtxTable->grCtx3d.BumpScale3          = REG_RD32(NV_PGRAPH_BUMPSCALE3);
        
        pSwCtxTable->grCtx3d.ClearRectX          = REG_RD32(NV_PGRAPH_CLEARRECTX);
        pSwCtxTable->grCtx3d.ClearRectY          = REG_RD32(NV_PGRAPH_CLEARRECTY);
        
        pSwCtxTable->grCtx3d.ColorClearValue     = REG_RD32(NV_PGRAPH_COLORCLEARVALUE);
        
        pSwCtxTable->grCtx3d.ColorKeyColor0      = REG_RD32(NV_PGRAPH_COLORKEYCOLOR0);
        pSwCtxTable->grCtx3d.ColorKeyColor1      = REG_RD32(NV_PGRAPH_COLORKEYCOLOR1);
        pSwCtxTable->grCtx3d.ColorKeyColor2      = REG_RD32(NV_PGRAPH_COLORKEYCOLOR2);
        pSwCtxTable->grCtx3d.ColorKeyColor3      = REG_RD32(NV_PGRAPH_COLORKEYCOLOR3);
        
        pSwCtxTable->grCtx3d.CombineFactor0_0    = REG_RD32(NV_PGRAPH_COMBINEFACTOR0_0);
        pSwCtxTable->grCtx3d.CombineFactor0_1    = REG_RD32(NV_PGRAPH_COMBINEFACTOR0_1);
        pSwCtxTable->grCtx3d.CombineFactor0_2    = REG_RD32(NV_PGRAPH_COMBINEFACTOR0_2);
        pSwCtxTable->grCtx3d.CombineFactor0_3    = REG_RD32(NV_PGRAPH_COMBINEFACTOR0_3);
        pSwCtxTable->grCtx3d.CombineFactor0_4    = REG_RD32(NV_PGRAPH_COMBINEFACTOR0_4);
        pSwCtxTable->grCtx3d.CombineFactor0_5    = REG_RD32(NV_PGRAPH_COMBINEFACTOR0_5);
        pSwCtxTable->grCtx3d.CombineFactor0_6    = REG_RD32(NV_PGRAPH_COMBINEFACTOR0_6);
        pSwCtxTable->grCtx3d.CombineFactor0_7    = REG_RD32(NV_PGRAPH_COMBINEFACTOR0_7);
        pSwCtxTable->grCtx3d.CombineFactor1_0    = REG_RD32(NV_PGRAPH_COMBINEFACTOR1_0);
        pSwCtxTable->grCtx3d.CombineFactor1_1    = REG_RD32(NV_PGRAPH_COMBINEFACTOR1_1);
        pSwCtxTable->grCtx3d.CombineFactor1_2    = REG_RD32(NV_PGRAPH_COMBINEFACTOR1_2);
        pSwCtxTable->grCtx3d.CombineFactor1_3    = REG_RD32(NV_PGRAPH_COMBINEFACTOR1_3);
        pSwCtxTable->grCtx3d.CombineFactor1_4    = REG_RD32(NV_PGRAPH_COMBINEFACTOR1_4);
        pSwCtxTable->grCtx3d.CombineFactor1_5    = REG_RD32(NV_PGRAPH_COMBINEFACTOR1_5);
        pSwCtxTable->grCtx3d.CombineFactor1_6    = REG_RD32(NV_PGRAPH_COMBINEFACTOR1_6);
        pSwCtxTable->grCtx3d.CombineFactor1_7    = REG_RD32(NV_PGRAPH_COMBINEFACTOR1_7);
        
        pSwCtxTable->grCtx3d.CombineAlphaI0      = REG_RD32(NV_PGRAPH_COMBINEALPHAI0);
        pSwCtxTable->grCtx3d.CombineAlphaI1      = REG_RD32(NV_PGRAPH_COMBINEALPHAI1);
        pSwCtxTable->grCtx3d.CombineAlphaI2      = REG_RD32(NV_PGRAPH_COMBINEALPHAI2);
        pSwCtxTable->grCtx3d.CombineAlphaI3      = REG_RD32(NV_PGRAPH_COMBINEALPHAI3);
        pSwCtxTable->grCtx3d.CombineAlphaI4      = REG_RD32(NV_PGRAPH_COMBINEALPHAI4);
        pSwCtxTable->grCtx3d.CombineAlphaI5      = REG_RD32(NV_PGRAPH_COMBINEALPHAI5);
        pSwCtxTable->grCtx3d.CombineAlphaI6      = REG_RD32(NV_PGRAPH_COMBINEALPHAI6);
        pSwCtxTable->grCtx3d.CombineAlphaI7      = REG_RD32(NV_PGRAPH_COMBINEALPHAI7);
        pSwCtxTable->grCtx3d.CombineAlphaO0      = REG_RD32(NV_PGRAPH_COMBINEALPHAO0);
        pSwCtxTable->grCtx3d.CombineAlphaO1      = REG_RD32(NV_PGRAPH_COMBINEALPHAO1);
        pSwCtxTable->grCtx3d.CombineAlphaO2      = REG_RD32(NV_PGRAPH_COMBINEALPHAO2);
        pSwCtxTable->grCtx3d.CombineAlphaO3      = REG_RD32(NV_PGRAPH_COMBINEALPHAO3);
        pSwCtxTable->grCtx3d.CombineAlphaO4      = REG_RD32(NV_PGRAPH_COMBINEALPHAO4);
        pSwCtxTable->grCtx3d.CombineAlphaO5      = REG_RD32(NV_PGRAPH_COMBINEALPHAO5);
        pSwCtxTable->grCtx3d.CombineAlphaO6      = REG_RD32(NV_PGRAPH_COMBINEALPHAO6);
        pSwCtxTable->grCtx3d.CombineAlphaO7      = REG_RD32(NV_PGRAPH_COMBINEALPHAO7);
        
        pSwCtxTable->grCtx3d.CombineColorI0      = REG_RD32(NV_PGRAPH_COMBINECOLORI0);
        pSwCtxTable->grCtx3d.CombineColorI1      = REG_RD32(NV_PGRAPH_COMBINECOLORI1);
        pSwCtxTable->grCtx3d.CombineColorI2      = REG_RD32(NV_PGRAPH_COMBINECOLORI2);
        pSwCtxTable->grCtx3d.CombineColorI3      = REG_RD32(NV_PGRAPH_COMBINECOLORI3);
        pSwCtxTable->grCtx3d.CombineColorI4      = REG_RD32(NV_PGRAPH_COMBINECOLORI4);
        pSwCtxTable->grCtx3d.CombineColorI5      = REG_RD32(NV_PGRAPH_COMBINECOLORI5);
        pSwCtxTable->grCtx3d.CombineColorI6      = REG_RD32(NV_PGRAPH_COMBINECOLORI6);
        pSwCtxTable->grCtx3d.CombineColorI7      = REG_RD32(NV_PGRAPH_COMBINECOLORI7);
        pSwCtxTable->grCtx3d.CombineColorO0      = REG_RD32(NV_PGRAPH_COMBINECOLORO0);
        pSwCtxTable->grCtx3d.CombineColorO1      = REG_RD32(NV_PGRAPH_COMBINECOLORO1);
        pSwCtxTable->grCtx3d.CombineColorO2      = REG_RD32(NV_PGRAPH_COMBINECOLORO2);
        pSwCtxTable->grCtx3d.CombineColorO3      = REG_RD32(NV_PGRAPH_COMBINECOLORO3);
        pSwCtxTable->grCtx3d.CombineColorO4      = REG_RD32(NV_PGRAPH_COMBINECOLORO4);
        pSwCtxTable->grCtx3d.CombineColorO5      = REG_RD32(NV_PGRAPH_COMBINECOLORO5);
        pSwCtxTable->grCtx3d.CombineColorO6      = REG_RD32(NV_PGRAPH_COMBINECOLORO6);
        pSwCtxTable->grCtx3d.CombineColorO7      = REG_RD32(NV_PGRAPH_COMBINECOLORO7);
        
        pSwCtxTable->grCtx3d.CombineCtl          = REG_RD32(NV_PGRAPH_COMBINECTL);
        
        pSwCtxTable->grCtx3d.CombineSpecFog0     = REG_RD32(NV_PGRAPH_COMBINESPECFOG0);
        pSwCtxTable->grCtx3d.CombineSpecFog1     = REG_RD32(NV_PGRAPH_COMBINESPECFOG1);
        
        pSwCtxTable->grCtx3d.Control0            = REG_RD32(NV_PGRAPH_CONTROL_0);
        pSwCtxTable->grCtx3d.Control1            = REG_RD32(NV_PGRAPH_CONTROL_1);
        pSwCtxTable->grCtx3d.Control2            = REG_RD32(NV_PGRAPH_CONTROL_2);
        pSwCtxTable->grCtx3d.Control3            = REG_RD32(NV_PGRAPH_CONTROL_3);
        
        pSwCtxTable->grCtx3d.FogColor            = REG_RD32(NV_PGRAPH_FOGCOLOR);
        pSwCtxTable->grCtx3d.FogParam0           = REG_RD32(NV_PGRAPH_FOGPARAM0);
        pSwCtxTable->grCtx3d.FogParam1           = REG_RD32(NV_PGRAPH_FOGPARAM1);
        pSwCtxTable->grCtx3d.PointSize           = REG_RD32(NV_PGRAPH_POINTSIZE);
        pSwCtxTable->grCtx3d.SetupRaster         = REG_RD32(NV_PGRAPH_SETUPRASTER);
        
        pSwCtxTable->grCtx3d.ShaderClipMode      = REG_RD32(NV_PGRAPH_SHADERCLIPMODE);
        pSwCtxTable->grCtx3d.ShaderCtl           = REG_RD32(NV_PGRAPH_SHADERCTL);
        pSwCtxTable->grCtx3d.ShaderProg          = REG_RD32(NV_PGRAPH_SHADERPROG);
        
        pSwCtxTable->grCtx3d.SemaphoreOffset     = REG_RD32(NV_PGRAPH_SEMAPHOREOFFSET);
        pSwCtxTable->grCtx3d.ShadowCtl           = REG_RD32(NV_PGRAPH_SHADOWCTL);
        pSwCtxTable->grCtx3d.ShadowZSlopeThreshold = REG_RD32(NV_PGRAPH_SHADOWZSLOPETHRESHOLD);
        
        pSwCtxTable->grCtx3d.SpecFogFactor0      = REG_RD32(NV_PGRAPH_SPECFOGFACTOR0);
        pSwCtxTable->grCtx3d.SpecFogFactor1      = REG_RD32(NV_PGRAPH_SPECFOGFACTOR1);
        
        pSwCtxTable->grCtx3d.SurfaceClipX        = REG_RD32(NV_PGRAPH_SURFACECLIPX);
        pSwCtxTable->grCtx3d.SurfaceClipY        = REG_RD32(NV_PGRAPH_SURFACECLIPY);
        
        pSwCtxTable->grCtx3d.TexAddress0         = REG_RD32(NV_PGRAPH_TEXADDRESS0);
        pSwCtxTable->grCtx3d.TexAddress1         = REG_RD32(NV_PGRAPH_TEXADDRESS1);
        pSwCtxTable->grCtx3d.TexAddress2         = REG_RD32(NV_PGRAPH_TEXADDRESS2);
        pSwCtxTable->grCtx3d.TexAddress3         = REG_RD32(NV_PGRAPH_TEXADDRESS3);
        
        pSwCtxTable->grCtx3d.TexCtl0_0       = REG_RD32(NV_PGRAPH_TEXCTL0_0);
        pSwCtxTable->grCtx3d.TexCtl0_1       = REG_RD32(NV_PGRAPH_TEXCTL0_1);
        pSwCtxTable->grCtx3d.TexCtl0_2       = REG_RD32(NV_PGRAPH_TEXCTL0_2);
        pSwCtxTable->grCtx3d.TexCtl0_3       = REG_RD32(NV_PGRAPH_TEXCTL0_3);
        
        pSwCtxTable->grCtx3d.TexCtl1_0       = REG_RD32(NV_PGRAPH_TEXCTL1_0);
        pSwCtxTable->grCtx3d.TexCtl1_1       = REG_RD32(NV_PGRAPH_TEXCTL1_1);
        pSwCtxTable->grCtx3d.TexCtl1_2       = REG_RD32(NV_PGRAPH_TEXCTL1_2);
        pSwCtxTable->grCtx3d.TexCtl1_3       = REG_RD32(NV_PGRAPH_TEXCTL1_3);
        
        pSwCtxTable->grCtx3d.TexCtl2_0       = REG_RD32(NV_PGRAPH_TEXCTL2_0);
        pSwCtxTable->grCtx3d.TexCtl2_1       = REG_RD32(NV_PGRAPH_TEXCTL2_1);
        
        pSwCtxTable->grCtx3d.TexFilter0          = REG_RD32(NV_PGRAPH_TEXFILTER0);
        pSwCtxTable->grCtx3d.TexFilter1          = REG_RD32(NV_PGRAPH_TEXFILTER1);
        pSwCtxTable->grCtx3d.TexFilter2          = REG_RD32(NV_PGRAPH_TEXFILTER2);
        pSwCtxTable->grCtx3d.TexFilter3          = REG_RD32(NV_PGRAPH_TEXFILTER3);
        
        pSwCtxTable->grCtx3d.TexFormat0          = REG_RD32(NV_PGRAPH_TEXFMT0);
        pSwCtxTable->grCtx3d.TexFormat1          = REG_RD32(NV_PGRAPH_TEXFMT1);
        pSwCtxTable->grCtx3d.TexFormat2          = REG_RD32(NV_PGRAPH_TEXFMT2);
        pSwCtxTable->grCtx3d.TexFormat3          = REG_RD32(NV_PGRAPH_TEXFMT3);
        
        pSwCtxTable->grCtx3d.TexImageRect0       = REG_RD32(NV_PGRAPH_TEXIMAGERECT0);
        pSwCtxTable->grCtx3d.TexImageRect1       = REG_RD32(NV_PGRAPH_TEXIMAGERECT1);
        pSwCtxTable->grCtx3d.TexImageRect2       = REG_RD32(NV_PGRAPH_TEXIMAGERECT2);
        pSwCtxTable->grCtx3d.TexImageRect3       = REG_RD32(NV_PGRAPH_TEXIMAGERECT3);
        
        pSwCtxTable->grCtx3d.TexOffset0          = REG_RD32(NV_PGRAPH_TEXOFFSET0);
        pSwCtxTable->grCtx3d.TexOffset1          = REG_RD32(NV_PGRAPH_TEXOFFSET1);
        pSwCtxTable->grCtx3d.TexOffset2          = REG_RD32(NV_PGRAPH_TEXOFFSET2);
        pSwCtxTable->grCtx3d.TexOffset3          = REG_RD32(NV_PGRAPH_TEXOFFSET3);
        
        pSwCtxTable->grCtx3d.TexPallete0         = REG_RD32(NV_PGRAPH_TEXPALETTE0);
        pSwCtxTable->grCtx3d.TexPallete1         = REG_RD32(NV_PGRAPH_TEXPALETTE1);
        pSwCtxTable->grCtx3d.TexPallete2         = REG_RD32(NV_PGRAPH_TEXPALETTE2);
        pSwCtxTable->grCtx3d.TexPallete3         = REG_RD32(NV_PGRAPH_TEXPALETTE3);
        
        pSwCtxTable->grCtx3d.WindowClipX[0]      = REG_RD32(NV_PGRAPH_WINDOWCLIPX0);
        pSwCtxTable->grCtx3d.WindowClipX[1]      = REG_RD32(NV_PGRAPH_WINDOWCLIPX1);
        pSwCtxTable->grCtx3d.WindowClipX[2]      = REG_RD32(NV_PGRAPH_WINDOWCLIPX2);
        pSwCtxTable->grCtx3d.WindowClipX[3]      = REG_RD32(NV_PGRAPH_WINDOWCLIPX3);
        pSwCtxTable->grCtx3d.WindowClipX[4]      = REG_RD32(NV_PGRAPH_WINDOWCLIPX4);
        pSwCtxTable->grCtx3d.WindowClipX[5]      = REG_RD32(NV_PGRAPH_WINDOWCLIPX5);
        pSwCtxTable->grCtx3d.WindowClipX[6]      = REG_RD32(NV_PGRAPH_WINDOWCLIPX6);
        pSwCtxTable->grCtx3d.WindowClipX[7]      = REG_RD32(NV_PGRAPH_WINDOWCLIPX7);
        
        pSwCtxTable->grCtx3d.WindowClipY[0]      = REG_RD32(NV_PGRAPH_WINDOWCLIPY0);
        pSwCtxTable->grCtx3d.WindowClipY[1]      = REG_RD32(NV_PGRAPH_WINDOWCLIPY1);
        pSwCtxTable->grCtx3d.WindowClipY[2]      = REG_RD32(NV_PGRAPH_WINDOWCLIPY2);
        pSwCtxTable->grCtx3d.WindowClipY[3]      = REG_RD32(NV_PGRAPH_WINDOWCLIPY3);
        pSwCtxTable->grCtx3d.WindowClipY[4]      = REG_RD32(NV_PGRAPH_WINDOWCLIPY4);
        pSwCtxTable->grCtx3d.WindowClipY[5]      = REG_RD32(NV_PGRAPH_WINDOWCLIPY5);
        pSwCtxTable->grCtx3d.WindowClipY[6]      = REG_RD32(NV_PGRAPH_WINDOWCLIPY6);
        pSwCtxTable->grCtx3d.WindowClipY[7]      = REG_RD32(NV_PGRAPH_WINDOWCLIPY7);
        
        pSwCtxTable->grCtx3d.ZCompressOcclude    = REG_RD32(NV_PGRAPH_ZCOMPRESSOCCLUDE);
        pSwCtxTable->grCtx3d.ZStencilClearValue  = REG_RD32(NV_PGRAPH_ZSTENCILCLEARVALUE);
        
        pSwCtxTable->grCtx3d.ZClipMax            = REG_RD32(NV_PGRAPH_ZCLIPMAX);
        pSwCtxTable->grCtx3d.ZClipMin            = REG_RD32(NV_PGRAPH_ZCLIPMIN);
        
        pSwCtxTable->grCtx3d.ContextDmaA         = REG_RD32(NV_PGRAPH_CONTEXTDMAA);
        pSwCtxTable->grCtx3d.ContextDmaB         = REG_RD32(NV_PGRAPH_CONTEXTDMAB);
        pSwCtxTable->grCtx3d.ContextDmaVtxA      = REG_RD32(NV_PGRAPH_CONTEXTVTXA);
        pSwCtxTable->grCtx3d.ContextDmaVtxB      = REG_RD32(NV_PGRAPH_CONTEXTVTXB);
        
        pSwCtxTable->grCtx3d.ZOffsetBias         = REG_RD32(NV_PGRAPH_ZOFFSETBIAS);
        pSwCtxTable->grCtx3d.ZOffsetFactor       = REG_RD32(NV_PGRAPH_ZOFFSETFACTOR);
        
        pSwCtxTable->grCtx3d.Eyevec0             = REG_RD32(NV_PGRAPH_EYEVEC0);
        pSwCtxTable->grCtx3d.Eyevec1             = REG_RD32(NV_PGRAPH_EYEVEC1);
        pSwCtxTable->grCtx3d.Eyevec2             = REG_RD32(NV_PGRAPH_EYEVEC2);
        
        pSwCtxTable->grCtx3d.Shadow              = REG_RD32(NV_PGRAPH_SHADOW);
        pSwCtxTable->grCtx3d.FdData              = REG_RD32(NV_PGRAPH_FD_DATA);
        pSwCtxTable->grCtx3d.FdSwatch            = REG_RD32(NV_PGRAPH_FD_SWATCH);
        pSwCtxTable->grCtx3d.FdExtras            = REG_RD32(NV_PGRAPH_FD_EXTRAS);
        pSwCtxTable->grCtx3d.Emission0           = REG_RD32(NV_PGRAPH_EMISSION_BACKUP_0);
        pSwCtxTable->grCtx3d.Emission1           = REG_RD32(NV_PGRAPH_EMISSION_BACKUP_1);
        pSwCtxTable->grCtx3d.Emission2           = REG_RD32(NV_PGRAPH_EMISSION_BACKUP_2);
        pSwCtxTable->grCtx3d.SceneAmb0           = REG_RD32(NV_PGRAPH_SCENE_AMBIENT_BACKUP_0);
        pSwCtxTable->grCtx3d.SceneAmb1           = REG_RD32(NV_PGRAPH_SCENE_AMBIENT_BACKUP_1);
        pSwCtxTable->grCtx3d.SceneAmb2           = REG_RD32(NV_PGRAPH_SCENE_AMBIENT_BACKUP_2);
        pSwCtxTable->grCtx3d.ContextDmaGetState  = REG_RD32(NV_PGRAPH_GETSTATE);
        pSwCtxTable->grCtx3d.BeginPatch0         = REG_RD32(NV_PGRAPH_BEGINPATCH0);
        pSwCtxTable->grCtx3d.BeginPatch1         = REG_RD32(NV_PGRAPH_BEGINPATCH1);
        pSwCtxTable->grCtx3d.BeginPatch2         = REG_RD32(NV_PGRAPH_BEGINPATCH2);
        pSwCtxTable->grCtx3d.BeginPatch3         = REG_RD32(NV_PGRAPH_BEGINPATCH3);
        pSwCtxTable->grCtx3d.BeginCurve          = REG_RD32(NV_PGRAPH_CURVE);
        pSwCtxTable->grCtx3d.BeginTrans0         = REG_RD32(NV_PGRAPH_BEGINTRANS0);
        pSwCtxTable->grCtx3d.BeginTrans1         = REG_RD32(NV_PGRAPH_BEGINTRANS1);
        pSwCtxTable->grCtx3d.Csv0_D              = REG_RD32(NV_PGRAPH_CSV0_D);
        pSwCtxTable->grCtx3d.Csv0_C              = REG_RD32(NV_PGRAPH_CSV0_C);
        pSwCtxTable->grCtx3d.Csv1_B              = REG_RD32(NV_PGRAPH_CSV1_B);
        pSwCtxTable->grCtx3d.Csv1_A              = REG_RD32(NV_PGRAPH_CSV1_A);
        pSwCtxTable->grCtx3d.CheopsOffset        = REG_RD32(NV_PGRAPH_CHEOPS_OFFSET);
        
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_FMT, RDI_ADDR_ZERO, IDX_FMT_COUNT, pSwCtxTable->grCtxPipe.IdxFmt);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS0, RDI_ADDR_ZERO, CAS0_COUNT, pSwCtxTable->grCtxPipe.Cas0);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS1, RDI_ADDR_ZERO, CAS1_COUNT, pSwCtxTable->grCtxPipe.Cas1);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CAS2, RDI_ADDR_ZERO, CAS2_COUNT, pSwCtxTable->grCtxPipe.Cas2);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_ASSM_STATE, RDI_ADDR_ZERO, ASSM_STATE_COUNT, pSwCtxTable->grCtxPipe.AssmState);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_CACHE0, RDI_ADDR_ZERO, IDX_CACHE0_COUNT, pSwCtxTable->grCtxPipe.IdxCache0);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_CACHE1, RDI_ADDR_ZERO, IDX_CACHE1_COUNT, pSwCtxTable->grCtxPipe.IdxCache1);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_CACHE2, RDI_ADDR_ZERO, IDX_CACHE2_COUNT, pSwCtxTable->grCtxPipe.IdxCache2);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_IDX_CACHE3, RDI_ADDR_ZERO, IDX_CACHE3_COUNT, pSwCtxTable->grCtxPipe.IdxCache3);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_VTX_FILE0, RDI_ADDR_ZERO, VTX_FILE0_COUNT, pSwCtxTable->grCtxPipe.VtxFile0);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_VTX_FILE1, RDI_ADDR_ZERO, VTX_FILE1_COUNT, pSwCtxTable->grCtxPipe.VtxFile1);
        
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_P, RDI_ADDR_ZERO, XL_P_COUNT, pSwCtxTable->grCtxPipe.XlP);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_XFCTX, RDI_ADDR_ZERO, XL_XFCTX_COUNT, pSwCtxTable->grCtxPipe.XlXfCtx);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_LTCTX, RDI_ADDR_ZERO, XL_LTCTX_COUNT, pSwCtxTable->grCtxPipe.XlLtCtx);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_LTC, RDI_ADDR_ZERO, XL_LTC_COUNT, pSwCtxTable->grCtxPipe.XlLtc);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_XL_VAB, RDI_ADDR_ZERO, XL_VAB_COUNT, pSwCtxTable->grCtxPipe.XlVab);
        
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_CRSTR_STIPP, RDI_ADDR_ZERO, CRSTR_STIPP_COUNT, pSwCtxTable->grCtxPipe.CrstrStipp);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_FD_CTRL, RDI_ADDR_ZERO, FD_CTRL_COUNT, pSwCtxTable->grCtxPipe.FdCtrl);
        grUnloadRdiContext(pHalHwInfo, RDI_RAMSEL_ROP_REGS, RDI_ADDR_ZERO, ROP_REGS_COUNT, pSwCtxTable->grCtxPipe.RopRegs);
    }

    // wait for the pipe to go idle again after unloading the pipeline context
    HAL_GR_IDLE(pHalHwInfo);

    //
    // Invalidate hw's channel ID.
    //
    REG_WR32(NV_PGRAPH_CTX_CONTROL, DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME,         _EXPIRED)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID,         _INVALID)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING,    _IDLE)
                                  | DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE,       _ENABLED));

    FLD_WR_DRF_NUM(_PGRAPH, _CTX_USER, _CHID, (NUM_FIFOS_NV20 - 1));

    return (RM_OK);
}
#endif // #ifdef NV20_SW_CTX_SWITCH
