/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvCelsiusComp.cpp                                                 *
*       Celsius inner loop compiler                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal             29Sep99         NV10 optimization effort    *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

#include "x86.h"
#include "nvILHash.h"

//////////////////////////////////////////////////////////////////////////////
// notes:
//
// inner loop uniqueness is a function of:
//  - fvfData.dwVertexStride
//  - fvfData.dwVertexType
//  - fvfData.dwUVCount[8]
//  - fvfData.dwUVOffset[8]
//  - pContext->hwState.dwStateFlags & CELSIUS_MASK_INNERLOOPUNIQUENESS
//  - pContext->hwState.celsius.dwTexUnitToTexStageMapping[2]
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// switches
//
//#define PRINT_NAME          // prints ilcFlags for every primitive batch

//////////////////////////////////////////////////////////////////////////////
// aliases
//
#define ilcData         global.dwILCData
#define ilcCount        global.dwILCCount
#define ilcMax          global.dwILCMax

#define KNIMEM(x)           ((((DWORD)&((*(KATMAI_STATE*)global.kni).x[0])) + 15) & ~15)

//////////////////////////////////////////////////////////////////////////////
// external declarations (mostly nvcomp.cpp)
//
void ILCCompile_GetSpace (void);
void ILCCompile_mul      (DWORD reg,DWORD num);

//////////////////////////////////////////////////////////////////////////////
// forward declarations
//
DWORD nvCelsiusILCompile_inline_prim      (PNVD3DCONTEXT pContext,DWORD dwFlags);    // nvCelsiusInlPrim.cpp
DWORD nvCelsiusILCompile_inline_tri_list  (PNVD3DCONTEXT pContext,DWORD dwFlags);    // nvCelsiusInlTri.cpp
DWORD nvCelsiusILCompile_vb_prim          (PNVD3DCONTEXT pContext,DWORD dwFlags);    // nvCelsiusVbPrim.cpp
DWORD nvCelsiusILCompile_str_prim         (PNVD3DCONTEXT pContext,DWORD dwFlags);    // nvCelsiusInlStrPrim.cpp
DWORD nvCelsiusILCompile_super_tri_list   (PNVD3DCONTEXT pContext,DWORD dwFlags);    // nvCelsiusSuperTri.cpp

//---------------------------------------------------------------------------

// nvCelsiusGetDispatchRoutine

CELSIUSDISPATCHPRIMITIVE nvCelsiusGetDispatchRoutine
(
    PNVD3DCONTEXT pContext,
    DWORD         dwFlags
)
{
    // check quick cache
    if (global.celsius.dwLoopCache)
    {
        CILHashEntry *pEntry = (CILHashEntry*)global.celsius.dwLoopCache;
        if (pEntry->match (pContext,NVCLASS_FAMILY_CELSIUS,dwFlags))
        {
            return (CELSIUSDISPATCHPRIMITIVE)(ilcData + pEntry->getOffset());
        }
    }

    // check hash table
    DWORD dwHashIndex = (DRAW_PRIM_TABLE_ENTRIES - 1) & ((dwFlags + (dwFlags >> 12) + (dwFlags >> 24))
                                                      + pContext->hwState.pVertexShader->getStride()
                                                      + pContext->hwState.pVertexShader->getFVF());

    CILHashEntry *pEntry = (CILHashEntry*)dwDrawPrimitiveTable[dwHashIndex];

    while (pEntry)
    {
        // found it?
        if (pEntry->match (pContext,NVCLASS_FAMILY_CELSIUS,dwFlags))
        {
            global.celsius.dwLoopCache = (DWORD)pEntry;
            return (CELSIUSDISPATCHPRIMITIVE)(ilcData + pEntry->getOffset());
        }
        // next
        pEntry = pEntry->getNext();
    }

    // build new innerloop
    //  loops require pContext->hwState.dwStateFlags to be set up correctly (CM hack, inline expansion)
    DWORD dwOffset;
    switch (dwFlags & CELSIUS_ILMASK_LOOPTYPE)
    {
        case CELSIUS_ILFLAG_IX_VB_PRIM:
        case CELSIUS_ILFLAG_OR_VB_PRIM:      dwOffset = nvCelsiusILCompile_vb_prim(pContext,dwFlags);
                                             break;
        case CELSIUS_ILFLAG_IX_STR_PRIM:
        case CELSIUS_ILFLAG_OR_STR_PRIM:     dwOffset = nvCelsiusILCompile_str_prim(pContext,dwFlags);
                                             break;
        case CELSIUS_ILFLAG_IX_INL_PRIM:
        case CELSIUS_ILFLAG_OR_INL_PRIM:     dwOffset = nvCelsiusILCompile_inline_prim(pContext,dwFlags);
                                             break;
        case CELSIUS_ILFLAG_IX_INL_TRI_LIST:
        case CELSIUS_ILFLAG_OR_INL_TRI_LIST: dwOffset = nvCelsiusILCompile_inline_tri_list(pContext,dwFlags);
                                             break;
        case CELSIUS_ILFLAG_SUPER_TRI_LIST:  dwOffset = 0;//nvCelsiusILCompile_super_tri_list(pContext,dwFlags);
                                             break;
        case CELSIUS_ILFLAG_IX_DVB_TRI:      dwOffset = 0;//todo - nvCelsiusILCompile_indexed_dvb_tri(pContext,dwFlags);
                                             break;
    }

    //
    // add to hash list
    //
    pEntry = new CILHashEntry (pContext,NVCLASS_FAMILY_CELSIUS,dwFlags,dwOffset,(CILHashEntry*)dwDrawPrimitiveTable[dwHashIndex]);
    if (!pEntry) {
        nvAssert(0);
        return NULL;
    }
    dwDrawPrimitiveTable[dwHashIndex] = (DWORD)pEntry;

#ifdef PRINT_NAME
    PF ("new celsius loop (%08x): type %x, dp2 %s (%2x), state = %08x, %s %s %s", pEntry,
        (dwFlags & 0xF0000000) >> 28,
        celsiusPrimitiveName[dwFlags & 0xFFFF], dwFlags & 0xFFFF,
        pContext->hwState.dwStateFlags & CELSIUS_MASK_INNERLOOPUNIQUENESS,
        dwFlags & CELSIUS_ILFLAG_LEGACY ? "LEGACY" : "",
        dwFlags & CELSIUS_ILFLAG_CMHACK ? "CMHACK" : "",
        dwFlags & CELSIUS_ILFLAG_NOCULL ? "NOCULL" : "");
#endif

    //
    // done
    //
    global.celsius.dwLoopCache = (DWORD)pEntry;
    return (CELSIUSDISPATCHPRIMITIVE)(ilcData + pEntry->getOffset());
}
/*
//---------------------------------------------------------------------------

// indexed vertex buffer copy

DWORD nvCelsiusILCompile_indexed_vb
(
    DWORD dwFlags
)
{
    DWORD label;
    xLABEL (label);
    xMOV_rm_imm (rmREG(rEAX),dwFlags);
    xRET

    return label;
}

//---------------------------------------------------------------------------

// ordered vertex buffer copy

DWORD nvCelsiusILCompile_ordered_vb
(
    DWORD dwFlags
)
{
    DWORD label;
    xLABEL (label);
    xMOV_rm_imm (rmREG(rEAX),dwFlags);
    xRET

    return label;
}

//---------------------------------------------------------------------------

// indexed copy

DWORD nvCelsiusILCompile_indexed
(
    DWORD dwFlags
)
{
    DWORD label;
    xLABEL (label);
    xMOV_rm_imm (rmREG(rEAX),dwFlags);
    xRET

    return label;
}

//---------------------------------------------------------------------------

// ordered copy

DWORD nvCelsiusILCompile_ordered
(
    DWORD dwFlags
)
{
    DWORD label;
    xLABEL (label);
    xMOV_rm_imm (rmREG(rEAX),dwFlags);
    xRET

    return label;
}

//---------------------------------------------------------------------------

// ordered transformed triangle

DWORD nvCelsiusILCompile_ordered_tri
(
    DWORD dwFlags
)
{
    DWORD label;
    xLABEL (label);
    xMOV_rm_imm (rmREG(rEAX),dwFlags);
    xRET

    return label;
}

//---------------------------------------------------------------------------

// helpers - called by inner loops

void __stdcall nvCelsiusDispatchGetFreeSpace (PNVD3DCONTEXT pContext)
{
    // make space
    nvPusherAdjust (0);
    // read back defVB values
    getDC()->defaultVB.updateOffset (global.celsius.dwVBOffset);
    // get more space
    DWORD dwOffset = getDC()->defaultVB.waitForSpace (12 * 64 * 10, TRUE);
    // reset defVB values
    getDC()->defaultVB.updateOffset (dwOffset);
    global.celsius.dwVBGetOffset = getDC()->defaultVB.getCachedOffset();
}
*/
void __stdcall nvCelsiusDispatchGetPusherSpace (void)
{
    // wrap around -or- make space
    nvPusherAdjust (0);
}

#ifdef DEBUG
void __stdcall nvCelsiusDispatchFlush (void)
{
    nvPusherAdjust (0);
    getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);
}

void __stdcall nvCelsiusDispatchPrintVertex (PNVD3DCONTEXT pContext,DWORD dwPutAddress)
{
    static DWORD dwVertexCount = 0;

    if (dbgShowState & NVDBG_SHOW_VERTICES)
    {
        DWORD *pdwData = ((DWORD*)dwPutAddress) + 1;

        dwVertexCount ++;

        DPF ("Vertex %d", dwVertexCount);

        if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_BLENDWEIGHT])) {
            DPF ("         B = %08x",pdwData[0]);
            pdwData += 1;
        }

        if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_NORMAL])) {
            DPF ("    Normal = %f, %f, %f", FLOAT_FROM_DWORD(pdwData[0]),FLOAT_FROM_DWORD(pdwData[1]),FLOAT_FROM_DWORD(pdwData[2]));
            DPF ("           = [%08x, %08x, %08x]", pdwData[0],pdwData[1],pdwData[2]);
            pdwData += 3;
        }

        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(1)) {
            DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[1];
            DWORD dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 16) & 0xffff;
            DWORD dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            DWORD dwCount    = (pContext->hwState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1) | CELSIUS_FLAG_TEXMATRIXSWFIX(1))) ?
                               4 : (pContext->hwState.pVertexShader->getVASize(dwVAIndex) >> 2);
            for (DWORD i = 0; i < dwCount; i++)
            {
                DPF ("   UV1[%d] = %f [%08x]", i,FLOAT_FROM_DWORD(pdwData[0]),pdwData[0]);
                pdwData += 1;
            }
        }

        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(0)) {
            DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[0];
            DWORD dwTCIndex  = (pContext->hwState.dwTexCoordIndices >>  0) & 0xffff;
            DWORD dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            DWORD dwCount    = (pContext->hwState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0) | CELSIUS_FLAG_TEXMATRIXSWFIX(0))) ?
                               4 : (pContext->hwState.pVertexShader->getVASize(dwVAIndex) >> 2);
            for (DWORD i = 0; i < dwCount; i++)
            {
                DPF ("   UV0[%d] = %f [%08x]", i,FLOAT_FROM_DWORD(pdwData[0]),pdwData[0]);
                pdwData += 1;
            }
        }

        if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])){
            DPF ("  Specular = %08x",pdwData[0]);
            pdwData += 1;
        }

        if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE])) {
            DPF ("   Diffuse = %08x",pdwData[0]);
            pdwData += 1;
        }

        nvAssert (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]));
        DPF ("       XYZ = %f, %f, %f", FLOAT_FROM_DWORD(pdwData[0]),FLOAT_FROM_DWORD(pdwData[1]),FLOAT_FROM_DWORD(pdwData[2]));
        DPF ("           = [%08x, %08x, %08x]", pdwData[0],pdwData[1],pdwData[2]);
        pdwData += 3;

        if (pContext->hwState.pVertexShader->getVASize(defaultInputRegMap[D3DVSDE_POSITION]) == 4*sizeof(DWORD)) {
            DPF ("       RHW = %f [%08x]", FLOAT_FROM_DWORD(pdwData[0]),pdwData[0]);
            pdwData += 1;
        }
        DPF ("");
    }
}
#endif
/*
//---------------------------------------------------------------------------

// helpers

void nvCelsiusILCompile_beginEnd
(
    DWORD dwPrimType
)
{
    xMOV_r_i32  (rEDX,mMEM32(pDriverData))
    xMOV_r_rm   (rEDI,rmIND32(rEDX)) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))
    xMOV_rm_imm (rmREG(rEAX),(((sizeSetNv10CelsiusBeginEnd2MthdCnt << 2) << 16) | ((NV_DD_CELSIUS) << 13) | NV056_SET_BEGIN_END2))
    xMOV_rm_imm (rmREG(rEBX),dwPrimType)
    xMOV_rm_r   (rmIND(rEDI),rEAX)
    xMOV_rm_r   (rmIND8(rEDI),rEBX) xOFS8(4)
    xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)
#ifndef NV_NULL_HW_DRIVER
    xMOV_rm_r   (rmIND32(rEDX),rEDI) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))
#endif
}

void nvCelsiusILCompile_computeThreshold (void)
{
    DWORD dwVBReservedSpace = 64 * 12; // max fvf size times 12
    DWORD dwDefaultVBSize = getDC()->defaultVB.getSize();

    // global.celsius.dwVBThreshold = (dwVBOffset < dwVBGetOffset) ? (dwVBGetOffset - dwVBSpace)
    //                                                             : (dwDefaultVBSize - dwVBSpace);
    xMOV_r_i32  (rEAX,mMEM32(global.celsius.dwVBOffset))
        xMOV_rm_imm (rmREG(rEDX),dwDefaultVBSize)
    xXOR_r_rm   (rECX,rmREG(rECX))
    xSUB_r_i32  (rEAX,mMEM32(global.celsius.dwVBGetOffset))
    xSETL_rm8   (rmREG(rCL))        // ecx = (ofs < get) ? 1 : 0
        xSUB_r_i32  (rEDX,mMEM32(global.celsius.dwVBGetOffset)) // edx = (dwDefaultVBSize - dwVBGetOffset)
    xDEC_rm     (rmREG(rECX))       // ecx = (ofs < get) ? 0 : ~0
    xAND_r_rm   (rECX,rmREG(rEDX))  // ecx = (ofs < get) ? 0 : edx -> (dwDefaultVBSize - dwVBGetOffset)
    xADD_r_i32  (rECX,mMEM32(global.celsius.dwVBGetOffset))
                                    // ecx = (ofs < get) ? get : vbsize
    xSUB_rm_imm (rmREG(rECX),dwVBReservedSpace)
    xMOV_i32_r  (mMEM32(global.celsius.dwVBThreshold),rECX)
}

void nvCelsiusILCompile_computeIndexAndOffset
(
    DWORD dwVBStride,
    DWORD dwLogStride       // must be zero if dwVBstride is not a power of 2
)
{
    // global.celsius.dwVBIndex = (getDC()->dwDefVBCurrentOffset + dwVBStride * 4 - 1) / dwVBStride;
    xMOV_r_i32  (rEAX,mMEM32(pDriverData))
    xMOV_rm_imm (rmREG(rEBX),dwVBStride * 4 - 1)
    if (dwLogStride)
    {
        xMOV_r_rm   (rEAX,rmIND32(rEAX)) xOFS32(OFFSETOF(CDriverContext,defaultVB.m_dwCurrentOffset))
        xADD_r_rm   (rEAX,rmREG(rEBX))
        xSHR_rm_imm (rmREG(rEAX),dwLogStride)
    }
    else
    {
        xXOR_r_rm   (rEDX,rmREG(rEDX))
        xMOV_r_rm   (rEAX,rmIND32(rEAX)) xOFS32(OFFSETOF(CDriverContext,defaultVB.m_dwCurrentOffset))
        xMOV_rm_imm (rmREG(rECX),dwVBStride)
        xADD_r_rm   (rEAX,rmREG(rEBX))
        xDIV_rm     (rmREG(rECX))
    }
    xMOV_i32_r  (mMEM32(global.celsius.dwVBIndex),rEAX)

    // global.celsius.dwVBOffset = global.celsius.dwVBIndex * dwVBStride;
    ILCCompile_mul (rEAX,dwVBStride);
    xMOV_i32_r  (mMEM32(global.celsius.dwVBOffset),rEAX)

    // compute threshold
    nvCelsiusILCompile_computeThreshold();

    // compute DVB banks
    xMOV_r_i32  (rEAX,mMEM32(global.celsius.dwVBIndex))
    xMOV_r_rm   (rEBX,rmREG(rEAX))
    xAND_rm_imm (rmREG(rEAX),0xffff8000)
    xAND_rm_imm (rmREG(rEBX),0x00007fff)
    xMOV_i32_r  (mMEM32(global.celsius.dwVBIndex),rEBX)
    // check if we need to reprogram FVF (dwVBBank is in eax)
    xMOV_r_i32  (rESI,mMEM32(global.celsius.pContext_celsiusState))
    ILCCompile_mul (rEAX,dwVBStride);
    // pContext->hwState.celsius.dwVertexOffsetInUse = dwVBBank * dwVBStride;
    xMOV_rm_r   (rmIND32(rESI),rEAX) xOFS32(OFFSETOF(CCelsiusState,dwVertexOffsetInUse))
    // set fvf dirty if (getDC()->dwVBCachedVertexOffset != dwDefVBBase))
    xMOV_r_i32  (rEDI,mMEM32(pDriverData))
    xXOR_r_rm   (rECX,rmREG(rECX))
    xCMP_r_rm   (rEAX,rmIND32(rEDI)) xOFS32(OFFSETOF(CDriverContext,dwVBCachedVertexOffset))
    xSETZ_rm8   (rmREG(rCL))
    xDEC_rm     (rmREG(rECX))
    xAND_rm_imm (rmREG(rECX),CELSIUS_DIRTY_FVF)
    xOR_rm_r    (rmIND32(rESI),rECX) xOFS32(OFFSETOF(CCelsiusState,dwDirtyFlags))

    //
    // set state
    //
    xMOV_r_i32  (rESI,mMEM32(global.celsius.pContext))
    xMOV_rm_imm (rmREG(rEBX),(DWORD)nvSetCelsiusState)
    xPUSH_r     (rESI)
    xCALL_rm    (rmREG(rEBX))
}
*/
// copies dwCount bytes from [esi] to [edi] with increment. if dwCount is zero then we copy ecx bytes
// we use esi,edi,eax and ecx. for kni we also use xmm0 to xmm3
void nvCelsiusILCompile_memcpy
(
    DWORD dwCount, bool recurse
)
{

//    if (dwCount)
//    {
//        xMOV_rm_imm (rmREG(rECX),dwCount)
//    }
//
//    xMOV_r_rm   (rEAX,rmREG(rECX))
//    xSHR_rm_imm (rmREG(rECX),2)
//    xAND_rm_imm (rmREG(rEAX),3)
//    xREP xMOVSD
//    xMOV_r_rm   (rECX,rmREG(rEAX))
//    xREP xMOVSB
//
    if (dwCount)
    {
        // do copy
        if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
        {
            //
            // KNI copy
            //
            while (dwCount >= 64)
            {
                xMOVLPS_r_rm    (rXMM0,rmIND(rESI))
                xMOVHPS_r_rm    (rXMM0,rmIND8(rESI)) xOFS8(8)
                xMOVLPS_r_rm    (rXMM1,rmIND8(rESI)) xOFS8(16)
                xMOVHPS_r_rm    (rXMM1,rmIND8(rESI)) xOFS8(24)
                xMOVLPS_r_rm    (rXMM2,rmIND8(rESI)) xOFS8(32)
                xMOVHPS_r_rm    (rXMM2,rmIND8(rESI)) xOFS8(40)
                xMOVLPS_r_rm    (rXMM3,rmIND8(rESI)) xOFS8(48)
                xMOVHPS_r_rm    (rXMM3,rmIND8(rESI)) xOFS8(56)
                xADD_rm_imm     (rmREG(rESI),64)
                xMOVLPS_rm_r    (rmIND(rEDI),rXMM0)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM0) xOFS8(8)
                xMOVLPS_rm_r    (rmIND8(rEDI),rXMM1) xOFS8(16)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM1) xOFS8(24)
                xMOVLPS_rm_r    (rmIND8(rEDI),rXMM2) xOFS8(32)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM2) xOFS8(40)
                xMOVLPS_rm_r    (rmIND8(rEDI),rXMM3) xOFS8(48)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM3) xOFS8(56)
                xADD_rm_imm     (rmREG(rEDI),64)
                dwCount -= 64;
            }

            while (dwCount >= 16)
            {
                xMOVLPS_r_rm    (rXMM0,rmIND(rESI))
                xMOVHPS_r_rm    (rXMM0,rmIND8(rESI)) xOFS8(8)
                xADD_rm_imm     (rmREG(rESI),16)
                xMOVLPS_rm_r    (rmIND(rEDI),rXMM0)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM0) xOFS8(8)
                xADD_rm_imm     (rmREG(rEDI),16)
                dwCount -= 16;
            }

            while (dwCount >= 8)
            {
                xMOV_r_rm    (rEAX,rmIND(rESI))
                xMOV_r_rm    (rECX,rmIND8(rESI)) xOFS8(4)
                xADD_rm_imm  (rmREG(rESI),8)
                xMOV_rm_r    (rmIND(rEDI),rEAX)
                xADD_rm_imm  (rmREG(rEDI),8)
                xMOV_rm_r    (rmIND8(rEDI),rECX) xOFS8(4 - 8)
                dwCount -= 8;
            }

            while (dwCount >= 4)
            {
                xMOV_r_rm    (rEAX,rmIND(rESI))
                xADD_rm_imm  (rmREG(rESI),4)
                xMOV_rm_r    (rmIND(rEDI),rEAX)
                xADD_rm_imm  (rmREG(rEDI),4)
                dwCount -= 4;
            }

            if (dwCount)
            {
                xMOV_rm_imm (rmREG(rECX),dwCount)
                xREP xMOVSB
            }
        }
        else if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)
        {
            //
            // AMD copy
            //
            bool needFemms = false;
            if (dwCount >= 16 && !recurse)
            {
                xFEMMS
                needFemms = true;
            }

            while (dwCount >= 64)
            {
                xMOVQ_r_rm      (rMM0,rmIND(rESI))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(8)
                xMOVQ_r_rm      (rMM2,rmIND8(rESI)) xOFS8(16)
                xMOVQ_r_rm      (rMM3,rmIND8(rESI)) xOFS8(24)
                xMOVQ_r_rm      (rMM4,rmIND8(rESI)) xOFS8(32)
                xMOVQ_r_rm      (rMM5,rmIND8(rESI)) xOFS8(40)
                xMOVQ_r_rm      (rMM6,rmIND8(rESI)) xOFS8(48)
                xMOVQ_r_rm      (rMM7,rmIND8(rESI)) xOFS8(56)
                xADD_rm_imm     (rmREG(rESI),64)
                xMOVNTQ_rm_r    (rmIND(rEDI),rMM0)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(8)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM2) xOFS8(16)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM3) xOFS8(24)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM4) xOFS8(32)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM5) xOFS8(40)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM6) xOFS8(48)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM7) xOFS8(56)
                xADD_rm_imm     (rmREG(rEDI),64)
                dwCount -= 64;
            }

            while (dwCount >= 16)
            {
                xMOVQ_r_rm      (rMM0,rmIND(rESI))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(8)
                xADD_rm_imm     (rmREG(rESI),16)
                xMOVNTQ_rm_r    (rmIND(rEDI),rMM0)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(8)
                xADD_rm_imm     (rmREG(rEDI),16)
                dwCount -= 16;
            }

            while (dwCount >= 8)
            {
                xMOV_r_rm    (rEAX,rmIND(rESI))
                xMOV_r_rm    (rECX,rmIND8(rESI)) xOFS8(4)
                xADD_rm_imm  (rmREG(rESI),8)
                xMOV_rm_r    (rmIND(rEDI),rEAX)
                xADD_rm_imm  (rmREG(rEDI),8)
                xMOV_rm_r    (rmIND8(rEDI),rECX) xOFS8(4 - 8)
                dwCount -= 8;
            }

            while (dwCount >= 4)
            {
                xMOV_r_rm    (rEAX,rmIND(rESI))
                xADD_rm_imm  (rmREG(rESI),4)
                xMOV_rm_r    (rmIND(rEDI),rEAX)
                xADD_rm_imm  (rmREG(rEDI),4)
                dwCount -= 4;
            }

            if (dwCount)
            {
                xMOV_rm_imm (rmREG(rECX),dwCount)
                xREP xMOVSB
            }

            if (needFemms)
            {
                xFEMMS
            }

        }
        else
        {
            //
            // normal 8086 copy
            //
            DWORD dwWordCount = dwCount / 4;
            if (dwWordCount)
            {
                xMOV_rm_imm (rmREG(rECX),dwWordCount)
                xREP xMOVSD
            }
            dwCount &= 3;
            if (dwCount)
            {
                xMOV_rm_imm (rmREG(rECX),dwCount)
                xREP xMOVSB
            }
        }
    }
    else
    {
        // variable length copy
        if ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI) || (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON))
        {
            bool needFemms = false;
            if (dwCount >= 16 && !recurse)
            {
                xFEMMS
                needFemms = true;
            }

            //
            // kni copy
            //
            DWORD label1;
            DWORD label2;
            // 64 byte xfers
            xLABEL      (label2)
            xCMP_rm_imm (rmREG(rECX),64)
            xLABEL      (label1)
            xJL32       (0)
            xPUSH_r     (rECX)
            nvCelsiusILCompile_memcpy (64, true);
            xPOP_r      (rECX)
            xLEA_r_rm   (rECX,rmIND8(rECX)) xOFS8(-64)
            xJMP        (label2)
            xTARGET_b32 (label1)
            // 16 byte xfers
            xLABEL      (label2)
            xCMP_rm_imm (rmREG(rECX),16)
            xLABEL      (label1)
            xJL32       (0)
            xPUSH_r     (rECX)
            nvCelsiusILCompile_memcpy (16, true);
            xPOP_r      (rECX)
            xLEA_r_rm   (rECX,rmIND8(rECX)) xOFS8(-16)
            xJMP        (label2)
            xTARGET_b32 (label1)
            // the rest
            xMOV_r_rm   (rEAX,rmREG(rECX))
            xSHR_rm_imm8(rmREG(rECX),2)
            xAND_rm_imm (rmREG(rEAX),3)
            xREP xMOVSD
            xMOV_r_rm   (rECX,rmREG(rEAX))
            xREP xMOVSB

            if (needFemms)
            {
                xFEMMS
            }
        }
        else
        {
            //
            // normal 8086 copy
            //
            xMOV_r_rm   (rEAX,rmREG(rECX))
            xSHR_rm_imm8(rmREG(rECX),2)
            xAND_rm_imm (rmREG(rEAX),3)
            xREP xMOVSD
            xMOV_r_rm   (rECX,rmREG(rEAX))
            xREP xMOVSB
        }
    }
}
// adds a base vertex index (16bit) to all 16bit chunks
// copies dwCount bytes from [esi] to [edi] with increment. if dwCount is zero then we copy ecx bytes
// we use esi,edi,eax and ecx. for kni we also use xmm0 to xmm3
void nvCelsiusILCompile_indexcpy
(
    DWORD dwCount
)
{
    if (dwCount)
    {
        // do copy
        if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE)
        {
            //
            // WNI copy
            //
            xMOV_r_i32      (rECX,mMEM32(global.celsius.dwBaseVertex))
            xSHL_rm_imm8    (rmREG(rECX),16)
            xOR_r_i32       (rECX,mMEM32(global.celsius.dwBaseVertex))
            xMOVDX_r_rm      (rXMM4, rmREG(rECX))
            xPSHUFLW_r_rm_imm (rXMM4, rmREG(rXMM4), 0)
            xPUNPCKLWDX_r_rm (rXMM4, rmREG(rXMM4))

            while (dwCount >= 64)
            {
                xMOVLPS_r_rm    (rXMM0,rmIND(rESI))
                xMOVHPS_r_rm    (rXMM0,rmIND8(rESI)) xOFS8(8)
                xPADDWX_r_rm    (rXMM0, rmREG(rXMM4))
                xMOVLPS_r_rm    (rXMM1,rmIND8(rESI)) xOFS8(16)
                xMOVHPS_r_rm    (rXMM1,rmIND8(rESI)) xOFS8(24)
                xPADDWX_r_rm    (rXMM1, rmREG(rXMM4))
                xMOVLPS_r_rm    (rXMM2,rmIND8(rESI)) xOFS8(32)
                xMOVHPS_r_rm    (rXMM2,rmIND8(rESI)) xOFS8(40)
                xPADDWX_r_rm    (rXMM2, rmREG(rXMM4))
                xMOVLPS_r_rm    (rXMM3,rmIND8(rESI)) xOFS8(48)
                xMOVHPS_r_rm    (rXMM3,rmIND8(rESI)) xOFS8(56)
                xPADDWX_r_rm    (rXMM3, rmREG(rXMM4))
                xADD_rm_imm     (rmREG(rESI),64)
                xMOVLPS_rm_r    (rmIND(rEDI),rXMM0)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM0) xOFS8(8)
                xMOVLPS_rm_r    (rmIND8(rEDI),rXMM1) xOFS8(16)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM1) xOFS8(24)
                xMOVLPS_rm_r    (rmIND8(rEDI),rXMM2) xOFS8(32)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM2) xOFS8(40)
                xMOVLPS_rm_r    (rmIND8(rEDI),rXMM3) xOFS8(48)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM3) xOFS8(56)
                xADD_rm_imm     (rmREG(rEDI),64)
                dwCount -= 64;
            }

            while (dwCount >= 16)
            {
                xMOVLPS_r_rm    (rXMM0,rmIND(rESI))
                xMOVHPS_r_rm    (rXMM0,rmIND8(rESI)) xOFS8(8)
                xPADDWX_r_rm    (rXMM0, rmREG(rXMM4))
                xADD_rm_imm     (rmREG(rESI),16)
                xMOVLPS_rm_r    (rmIND(rEDI),rXMM0)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM0) xOFS8(8)
                xADD_rm_imm     (rmREG(rEDI),16)
                dwCount -= 16;
            }
        }
        else if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
        {
            //
            // KNI copy - we use MMX because there's no integer add for KNI regs.
            //
            xEMMS
            xMOV_r_i32      (rECX,mMEM32(global.celsius.dwBaseVertex))
            xSHL_rm_imm8    (rmREG(rECX),16)
            xOR_r_i32       (rECX,mMEM32(global.celsius.dwBaseVertex))
            xMOVD_r_rm      (rMM4, rmREG(rECX))
            xPSHUFW_r_rm_imm (rMM4, rmREG(rMM4), 0)
            while (dwCount >= 64)
            {

                xMOVQ_r_rm      (rMM0,rmIND(rESI))
                xPADDW_r_rm     (rMM0, rmREG(rMM4))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(8)
                xPADDW_r_rm     (rMM1, rmREG(rMM4))
                xMOVQ_r_rm      (rMM2,rmIND8(rESI)) xOFS8(16)
                xPADDW_r_rm     (rMM2, rmREG(rMM4))
                xMOVQ_r_rm      (rMM3,rmIND8(rESI)) xOFS8(24)
                xPADDW_r_rm     (rMM3, rmREG(rMM4))

                xMOVNTQ_rm_r    (rmIND(rEDI),rMM0)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(8)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM2) xOFS8(16)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM3) xOFS8(24)

                xMOVQ_r_rm      (rMM0,rmIND8(rESI)) xOFS8(32)
                xPADDW_r_rm     (rMM0, rmREG(rMM4))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(40)
                xPADDW_r_rm     (rMM1, rmREG(rMM4))
                xMOVQ_r_rm      (rMM2,rmIND8(rESI)) xOFS8(48)
                xPADDW_r_rm     (rMM2, rmREG(rMM4))
                xMOVQ_r_rm      (rMM3,rmIND8(rESI)) xOFS8(56)
                xPADDW_r_rm     (rMM3, rmREG(rMM4))

                xADD_rm_imm     (rmREG(rESI),64)


                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM0) xOFS8(32)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(40)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM2) xOFS8(48)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM3) xOFS8(56)
                xADD_rm_imm     (rmREG(rEDI),64)


                dwCount -= 64;
            }

            while (dwCount >= 16)
            {
                xMOVQ_r_rm      (rMM0,rmIND(rESI))
                xPADDW_r_rm     (rMM0, rmREG(rMM4))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(8)
                xPADDW_r_rm     (rMM1, rmREG(rMM4))
                xADD_rm_imm     (rmREG(rESI),16)
                xMOVNTQ_rm_r    (rmIND(rEDI),rMM0)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(8)
                xADD_rm_imm     (rmREG(rEDI),16)
                dwCount -= 16;
            }
            xEMMS

        }
        else if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)
        {
            //
            // AMD copy
            //
            xFEMMS
            xMOV_r_i32      (rECX,mMEM32(global.celsius.dwBaseVertex))
            xSHL_rm_imm8    (rmREG(rECX),16)
            xOR_r_i32       (rECX,mMEM32(global.celsius.dwBaseVertex))
            xMOVD_r_rm      (rMM4, rmREG(rECX))
            xPUNPCKLDQ_r_rm (rMM4, rmREG(rMM4))
            while (dwCount >= 64)
            {
                xMOVQ_r_rm      (rMM0,rmIND(rESI))
                xPADDW_r_rm     (rMM0, rmREG(rMM4))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(8)
                xPADDW_r_rm     (rMM1, rmREG(rMM4))
                xMOVQ_r_rm      (rMM2,rmIND8(rESI)) xOFS8(16)
                xPADDW_r_rm     (rMM2, rmREG(rMM4))
                xMOVQ_r_rm      (rMM3,rmIND8(rESI)) xOFS8(24)
                xPADDW_r_rm     (rMM3, rmREG(rMM4))

                xMOVNTQ_rm_r    (rmIND(rEDI),rMM0)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(8)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM2) xOFS8(16)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM3) xOFS8(24)

                xMOVQ_r_rm      (rMM0,rmIND8(rESI)) xOFS8(32)
                xPADDW_r_rm     (rMM0, rmREG(rMM4))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(40)
                xPADDW_r_rm     (rMM1, rmREG(rMM4))
                xMOVQ_r_rm      (rMM2,rmIND8(rESI)) xOFS8(48)
                xPADDW_r_rm     (rMM2, rmREG(rMM4))
                xMOVQ_r_rm      (rMM3,rmIND8(rESI)) xOFS8(56)
                xPADDW_r_rm     (rMM3, rmREG(rMM4))

                xADD_rm_imm     (rmREG(rESI),64)


                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM0) xOFS8(32)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(40)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM2) xOFS8(48)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM3) xOFS8(56)
                xADD_rm_imm     (rmREG(rEDI),64)


                dwCount -= 64;
            }

            while (dwCount >= 16)
            {
                xMOVQ_r_rm      (rMM0,rmIND(rESI))
                xPADDW_r_rm     (rMM0, rmREG(rMM4))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(8)
                xPADDW_r_rm     (rMM1, rmREG(rMM4))
                xADD_rm_imm     (rmREG(rESI),16)
                xMOVNTQ_rm_r    (rmIND(rEDI),rMM0)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(8)
                xADD_rm_imm     (rmREG(rEDI),16)
                dwCount -= 16;
            }
            xFEMMS
        }
        if (dwCount)
        {
            //
            // normal 8086 copy
            //
            DWORD label;
            xLABEL (label)


            xMOV_r_i32  (rECX,mMEM32(global.celsius.dwBaseVertex))
            xSHL_rm_imm8 (rmREG(rECX),16)
            xOR_r_i32   (rECX,mMEM32(global.celsius.dwBaseVertex))
            //hmmm fully unrolled loop might not be the best... HMH
            //never mind, 64 indices is max (aka 32 moves)
            while (dwCount >= 4)
            {
                xMOV_r_rm    (rEAX,rmIND(rESI))
                xADD_rm_imm  (rmREG(rESI),4)
                xADD_r_rm    (rEAX, rmREG(rECX)) //add base index
                xMOV_rm_r    (rmIND(rEDI),rEAX)
                xADD_rm_imm  (rmREG(rEDI),4)
                dwCount -= 4;

            }
            if (dwCount) //it had better be 2
            {
                x16r xMOV_r_rm (rAX,rmIND(rESI))
                xADD_rm_imm  (rmREG(rESI),2)
                xADD_r_rm    (rEAX, rmREG(rECX)) //add base index
                xMOV_rm_r    (rmIND(rEDI),rAX)
                xADD_rm_imm  (rmREG(rEDI),2)
                dwCount -= 2;
            }
        }
    }
}

// vertex at ESI, pusher at EDI
//  thrashes eax,ebx,ecx,edx and ebp
void nvCelsiusILCompile_copyVertex_texMatrixFix
(
    PNVD3DCONTEXT pContext,
    DWORD         dwFlags,
    DWORD         dwD3DStage,
    DWORD         dwTCIndex,
    DWORD         dwInCount,
    DWORD         dwOutCount,
    BOOL          bProjected,
    DWORD        *pdwDstIndex
)
{
    // psuedo-C equivalent of the snippet below

    // D3DMATRIX *pMatrix = &(pContext->tssState[dwD3DStage].mTexTransformMatrix);
    // D3DVALUE dvCoordsIn[4], dvCoordsOut[4];
    // DWORD i;
    //
    // // fetch incoming coords
    // for (i = 0; i < dwInCount; i++) {
    //     dvCoordsIn[i] = rmIND8(rESI) xOFS8(i * 4 + fvfData.dwUVOffset[dwTCIndex])
    // }
    // // tack on a 1.0
    // dvCoordsIn[dwInCount] = 1.0;
    // // set the rest to zero
    // for (i = dwInCount+1; i < 4; i++) {
    //     dvCoordsIn[i] = 0;
    // }
    //
    // // transform
    // XformVector4 (D3DVALUE dvCoordsOut, D3DVALUE dvCoordsIn, pMatrix);
    //
    // // write out the real coordinates
    // for (i = 0; i < (dwOutCount - (bProjected ? 1 : 0)); i++) {
    //     xMOV_rm_r (rmIND8(rEDI), dvCoordsOut[i]) xOFS8(dwDstIndex)
    //     dwDstIndex += 4;
    // }
    // // pad with zeros
    // for (; i < 3; i++) {
    //     xMOV_rm_r (rmIND8(rEDI), 0);
    //     dwDstIndex += 4;
    // }
    // // fill the last slot with either 1.0 or the projected value
    // xMOV_rm_r (rmIND8(rEDI), bProjected ? dvCoordsOut[dwOutCount-1] : 1.0) xOFS8(dwDstIndex)
    // dwDstIndex += 4;

    //
    // get active tex transform matrix
    //
    xMOV_r_i32  (rEAX,mMEM32(global.celsius.pContext))
    xLEA_r_rm   (rEAX,rmIND32(rEAX)) xOFS32(OFFSETOF(NVD3DCONTEXT,tssState[dwD3DStage].mTexTransformMatrix))

    //
    // fetch
    //
    for (DWORD i = 0; i < dwInCount; i++)
    {
        xFLD_rm32 (rmIND8(rESI)) xOFS8(i * 4 + global.celsius.dwOffsetUV[dwTCIndex])
    }
    xFLD1
    for (i++; i < 4; i++)
    {
        xFLDZ
    }

    //
    // transform into temp space
    //
    for (i = 0; i < dwInCount; i++)
    {                                                           // w z y x
        xFLD_st (rST3)                                          // x w z y x
        xFMUL_rm (rmIND8(rEAX)) xOFS8((0*4+i)*4)                // x*m1i w z y x
        xFLD_st (rST3)                                          // y x*m1i w z y x
        xFLD_st (rST3)                                          // z y x*m1i w z y x
        xFMUL_rm (rmIND8(rEAX)) xOFS8((2*4+i)*4)                // z*m3i y x*m1i w z y x
        xFLD_st (rST3)                                          // w z*m3i y x*m1i w z y x
        xFMUL_rm (rmIND8(rEAX)) xOFS8((3*4+i)*4)                // w*m4i z*m3i y x*m1i w z y x
        xFXCH_st (rST2)                                         // y w*m4i z*m3i x*m1i w z y x
        xFMUL_rm (rmIND8(rEAX)) xOFS8((1*4+i)*4)                // y*m2i w*m4i z*m3i x*m1i w z y x
        xFADDP_st (rST1)                                        // y*m2i+w*m4i z*m3i x*m1i w z y x
        xFADDP_st (rST1)                                        // y*m2i+w*m4i+z*m3i x*m1i w z y x
        xFADDP_st (rST1)                                        // y*m2i+w*m4i+z*m3i+x*m1i w z y x
        xFSTP_i32 (KNIMEM(fRHW) + i*4)                          // w z y x
    }

    xFFREE_st (rST0)
    xFFREE_st (rST1)
    xFFREE_st (rST2)
    xFFREE_st (rST3)

    //
    // write out
    //
    for (i = 0; i < (dwOutCount - (bProjected ? 1 : 0)); i++)
    {
        xMOV_r_i32 (rEBX,KNIMEM(fRHW) + i*4)
        xMOV_rm_r (rmIND8(rEDI),rEBX) xOFS8(*pdwDstIndex)
        (*pdwDstIndex) += 4;
    }
    xXOR_r_rm (rEBX,rmREG(rEBX))
    for (; i < 3; i++) {
        xMOV_rm_r (rmIND8(rEDI),rEBX) xOFS8(*pdwDstIndex)
        (*pdwDstIndex) += 4;
    }
    // fill the last slot with either 1.0 or the projected value
    if (bProjected)
    {
        xMOV_r_i32 (rEBX,KNIMEM(fRHW) + 4*(dwOutCount-1))
    }
    else
    {
        xMOV_r_i32 (rEBX,KNIMEM(fOne))
    }
    xMOV_rm_r (rmIND8(rEDI),rEBX) xOFS8(*pdwDstIndex)
    (*pdwDstIndex) += 4;
}

// vertex at ESI, pusher at EDI
//  trashes eax,ebx,ecx,edx and ebp
void nvCelsiusILCompile_copyVertex
(
    PNVD3DCONTEXT pContext,
    DWORD         dwFlags
)
{
    CVertexShader *pVShader;
    DWORD          i;

    const DWORD    adwRegister[]        = { rEAX,rEBX,rECX,rEDX,rEBP };
    DWORD          dwDstIndex           = 0;
    DWORD          dwInlineVertexStride = pContext->hwState.dwInlineVertexStride;

    // cache the vertex shader
    pVShader = pContext->hwState.pVertexShader;
    nvAssert (pVShader);

    // rd: method
    xMOV_rm_imm (rmREG(rEAX),(((dwInlineVertexStride) << 16) | ((NV_DD_CELSIUS) << 13) | (NVPUSHER_NOINC(NV056_INLINE_ARRAY(0)))))

    // rd: b
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_BLENDWEIGHT]))
    {
        xMOV_r_rm   (rEBX,rmIND8(rESI)) xOFS8(global.celsius.dwOffsetWeight)
    }

    // rd: normal
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_NORMAL]))
    {
        xMOV_r_rm   (rECX,rmIND8(rESI)) xOFS8(global.celsius.dwOffsetNormal)
        xMOV_r_rm   (rEDX,rmIND8(rESI)) xOFS8(global.celsius.dwOffsetNormal + 4)
        xMOV_r_rm   (rEBP,rmIND8(rESI)) xOFS8(global.celsius.dwOffsetNormal + 8)
    }

    // wr: method
    xMOV_rm_r   (rmIND(rEDI),rEAX)
    dwDstIndex += 4;

    // wr: b
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_BLENDWEIGHT]))
    {
        xMOV_rm_r   (rmIND8(rEDI),rEBX) xOFS8(dwDstIndex)
        dwDstIndex += 4;
    }

    // wr: normal
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_NORMAL]))
    {
        xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(dwDstIndex)
        xMOV_rm_r   (rmIND8(rEDI),rEDX) xOFS8(dwDstIndex + 4)
        xMOV_rm_r   (rmIND8(rEDI),rEBP) xOFS8(dwDstIndex + 8)
        dwDstIndex += 12;
    }

    // rd & wr: tex coords 1
    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(1))
    {
        DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[1];
        DWORD dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 16) & 0xffff;
        DWORD dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
        DWORD dwCount    = (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1)) ?
                           3 : ((pVShader->getVASize(dwVAIndex)) >> 2);
        assert (dwCount < 5);

        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_TEXMATRIXSWFIX(1))
        {
            DWORD dwXFormFlags = pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS];
            DWORD dwOutCount   = dwXFormFlags & 0xff;
            BOOL  bProjected   = (dwXFormFlags & D3DTTFF_PROJECTED) ? TRUE : FALSE;
            nvCelsiusILCompile_copyVertex_texMatrixFix (pContext,dwFlags,dwD3DStage,dwTCIndex,dwCount,dwOutCount,bProjected,&dwDstIndex);
        }
        else
        {
            for (i=0; i<dwCount; i++) {
                xMOV_r_rm   (adwRegister[i],rmIND8(rESI)) xOFS8(i * 4 + global.celsius.dwOffsetUV[dwTCIndex])
            }
#ifdef STOMP_TEX_COORDS
            if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_STOMP_4TH_COORD(1)) {
                xMOV_r_imm (adwRegister[3],0x3f800000)
            }
#endif  // STOMP_TEX_COORDS
            for (i=0; i<dwCount; i++) {
                xMOV_rm_r   (rmIND8(rEDI),adwRegister[i]) xOFS8(dwDstIndex)
                dwDstIndex += 4;
            }
            if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1)) {
                xMOV_rm_r   (rmIND8(rEDI),adwRegister[2]) xOFS8(dwDstIndex)
                dwDstIndex += 4;
            }
        }
    }

    // rd & wr: tex coords 0
    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(0))
    {
        DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[0];
        DWORD dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 0) & 0xffff;
        DWORD dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
        DWORD dwCount    = (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0)) ?
                           3 : ((pVShader->getVASize(dwVAIndex)) >> 2);
        assert (dwCount < 5);

        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_TEXMATRIXSWFIX(0))
        {
            DWORD dwXFormFlags = pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS];
            DWORD dwOutCount   = dwXFormFlags & 0xff;
            BOOL  bProjected   = (dwXFormFlags & D3DTTFF_PROJECTED) ? TRUE : FALSE;
            nvCelsiusILCompile_copyVertex_texMatrixFix (pContext,dwFlags,dwD3DStage,dwTCIndex,dwCount,dwOutCount,bProjected,&dwDstIndex);
        }
        else
        {
            for (DWORD i = 0; i < dwCount; i++)
            {
                xMOV_r_rm   (adwRegister[i],rmIND8(rESI)) xOFS8(i * 4 + global.celsius.dwOffsetUV[dwTCIndex])
            }
#ifdef STOMP_TEX_COORDS
            if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_STOMP_4TH_COORD(0)) {
                xMOV_r_imm (adwRegister[3],0x3f800000)
            }
#endif  // STOMP_TEX_COORDS
            for (DWORD k = 0; k < dwCount; k++)
            {
                xMOV_rm_r   (rmIND8(rEDI),adwRegister[k]) xOFS8(dwDstIndex)
                dwDstIndex += 4;
            }
            if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0))
            {
                xMOV_rm_r   (rmIND8(rEDI),adwRegister[2]) xOFS8(dwDstIndex)
                dwDstIndex += 4;
            }
        }
    }

    // rd: specular
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR]))
    {
        xMOV_r_rm   (rEAX,rmIND8(rESI)) xOFS8(global.celsius.dwOffsetSpecular)
    }

    // rd: diffuse
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE]))
    {
        xMOV_r_rm   (rEBX,rmIND8(rESI)) xOFS8(global.celsius.dwOffsetDiffuse)
    }

    // rd: xyz
    nvAssert (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]));

    xMOV_r_rm   (rECX,rmIND8(rESI)) xOFS8(global.celsius.dwOffsetXYZ)
    xMOV_r_rm   (rEDX,rmIND8(rESI)) xOFS8(global.celsius.dwOffsetXYZ + 4)
    xMOV_r_rm   (rEBP,rmIND8(rESI)) xOFS8(global.celsius.dwOffsetXYZ + 8)

    // wr: specular
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR]))
    {
        xMOV_rm_r   (rmIND8(rEDI),rEAX) xOFS8(dwDstIndex)
        dwDstIndex += 4;
    }

    // wr: diffuse
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE]))
    {
        xMOV_rm_r   (rmIND8(rEDI),rEBX) xOFS8(dwDstIndex)
        dwDstIndex += 4;
    }

    // wr: xyz
    xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(dwDstIndex)
    xMOV_rm_r   (rmIND8(rEDI),rEDX) xOFS8(dwDstIndex + 4)
    xMOV_rm_r   (rmIND8(rEDI),rEBP) xOFS8(dwDstIndex + 8)
    dwDstIndex += 12;

    // rd & wr: rhw
    if (pVShader->getVASize(defaultInputRegMap[D3DVSDE_POSITION]) == 4*sizeof(DWORD))
    {
        xMOV_r_rm   (rEAX,rmIND8(rESI)) xOFS8(global.celsius.dwOffsetRHW)
        xMOV_rm_r   (rmIND8(rEDI),rEAX) xOFS8(dwDstIndex)
        dwDstIndex += 4;
    }

    // sanity check
    assert ((dwDstIndex - 4) == dwInlineVertexStride);

    // we can optionally print the vertex here
#if defined(DEBUG) || defined(NVSTATDRIVER)
    xMOV_rm_imm (rmREG(rEAX),(DWORD)nvCelsiusDispatchPrintVertex)
    xMOV_r_i32  (rEBX,mMEM32(global.celsius.pContext))
    xPUSH_r     (rEDI)
    xPUSH_r     (rEBX)
    xCALL_rm    (rmREG(rEAX))
#endif

    // adjust put
    xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(dwDstIndex)
}

//
// range limit <only on NT> (dummy must be eax, ebx, ecx or edx)
//
void nvCelsiusILCompile_limit
(
    DWORD regLimit,
    DWORD regDummy
)
{
    assert ((regDummy == rEAX)
         || (regDummy == rEBX)
         || (regDummy == rECX)
         || (regDummy == rEDX));

#ifdef WINNT
    xXOR_r_rm   (regDummy,rmREG(regDummy))
    xCMP_r_i32  (regLimit,mMEM32(global.dwMaxVertexOffset))
    xSETAE_rm8  (rmREG(regDummy))
    xDEC_rm     (rmREG(regDummy))
    xAND_r_rm   (regLimit,rmREG(regDummy))
#endif
}

// prefetch data
//  will trash ESI, ECX, EDX, EAX
void nvCelsiusILCompile_prefetch
(
    BOOL bIsIndexed,
    BOOL bLegacyStrides
)
{
    if (bIsIndexed)
    {
        // prefetch & range check indices
        DWORD labelPrefetch1;
        DWORD labelPrefetch2;
        DWORD labelPrefetch3;
        DWORD labelPrefetch4;
        xLABEL      (labelPrefetch1)
        xMOV_r_i32  (rESI,mMEM32(global.celsius.dwPrefetchBase))
        xCMP_r_i32  (rESI,mMEM32(global.celsius.pIndices))
        xLABEL      (labelPrefetch2)
        xJA32       (0)

        xMOV_rm_imm (rmREG(rECX),2048 / global.celsius.dwVertexStride)
        xLABEL      (labelPrefetch4)
        {
            xPUSH_r     (rECX)
            // read index
            xXOR_r_rm   (rEDX,rmREG(rEDX))
       x16r xMOV_r_rm   (rDX,rmIND(rESI))
            xLEA_r_rm   (rESI,rmIND8(rESI)) xOFS8(2)
            if (bLegacyStrides)
            {
                xMOV_r_rm   (rEAX,rmREG(rESI))
                xSUB_r_i32  (rEAX,mMEM32(global.celsius.pIndexBase))
                xAND_rm_imm (rmREG(rEAX),6)
                xXOR_r_rm   (rECX,rmREG(rECX))
                xCMP_rm_imm (rmREG(rEAX),6)
                xSETNZ_rm8  (rmREG(rCL))
                xDEC_rm     (rmREG(rECX))
                xAND_rm_imm (rmREG(rECX),2)
                xADD_r_rm   (rESI,rmREG(rECX))
            }
            ILCCompile_mul (rEDX,global.celsius.dwVertexStride);
            // range limit
            nvCelsiusILCompile_limit (rEDX,rECX);
            // compute vertex address
            xADD_r_i32  (rEDX,mMEM32(global.celsius.pVertices))
            // prefetch
            xMOV_r_rm   (rEAX,rmIND(rEDX))
            if (global.celsius.dwVertexStride > 32)
            {
                xMOV_r_rm   (rEAX,rmIND8(rEDX)) xOFS8(32)
            }
            xMOV_rm_imm (rmREG(rEAX),0)
            // end of valid run?
            xPOP_r      (rECX)
            xCMP_r_i32  (rESI,mMEM32(global.celsius.dwPrefetchMax))
            xLABEL      (labelPrefetch3)
            xJAE32      (0)
            // next
            xDEC_rm     (rmREG(rECX))
        }
        xJNZ32      (labelPrefetch4)
        xTARGET_b32 (labelPrefetch3)
        xMOV_i32_r  (mMEM32(global.celsius.dwPrefetchBase),rESI)
        xTARGET_b32 (labelPrefetch2)
    }
    else
    {
        // prefetch & range check indices
        DWORD labelPrefetch1;
        DWORD labelPrefetch2;
        DWORD labelPrefetch3;
        DWORD labelPrefetch4;
        xLABEL      (labelPrefetch1)
        xMOV_r_i32  (rESI,mMEM32(global.celsius.dwPrefetchBase))
        xCMP_r_i32  (rESI,mMEM32(global.celsius.pVertices))
        xLABEL      (labelPrefetch2)
        xJA32       (0)

        xMOV_rm_imm (rmREG(rECX),2048 / global.celsius.dwVertexStride)
        xLABEL      (labelPrefetch4)
        {
            // prefetch
            xMOV_r_rm   (rEAX,rmIND(rESI))
            xMOV_rm_imm (rmREG(rEAX),0)
            xADD_rm_imm (rmREG(rESI),32)
            // end of valid run?
            xCMP_r_i32  (rESI,mMEM32(global.celsius.dwPrefetchMax))
            xLABEL      (labelPrefetch3)
            xJAE32      (0)
            // next
            xDEC_rm     (rmREG(rECX))
        }
        xJNZ32      (labelPrefetch4)
        xTARGET_b32 (labelPrefetch3)
        xMOV_i32_r  (mMEM32(global.celsius.dwPrefetchBase),rESI)
        xTARGET_b32 (labelPrefetch2)
    }
}

#endif  // NVARCH == 0x10

