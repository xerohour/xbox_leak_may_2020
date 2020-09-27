// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: nvKelvinPrim.cpp
//      Kelvin primitive rendering routines
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        26Apr2000         NV20 development
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x20)

#ifdef KELVIN_ILC
DWORD UseKelvinILC = 1;     //Temp - set to zero to disable Kelvin compiled inner loops.
#endif

#ifdef PROFILE_INDEX
void nvLogIndexData(DWORD, DWORD, WORD *);
#endif

// switches
#define VERTEX_REUSE

//---------------------------------------------------------------------------
// forward declarations

void nvKelvinDispatchIndexedPrimitiveInline    (PNVD3DCONTEXT       pContext,
                                                DWORD               dwCount,
                                                BOOL                bLegacyStrides);
void nvKelvinDispatchNonIndexedPrimitiveInline (PNVD3DCONTEXT       pContext,
                                                DWORD               dwCount);

//---------------------------------------------------------------------------

void nvKelvinDispatchIndexedPrimitive
(
    NV_INNERLOOP_ARGLIST
)
{
    BOOL bLegacyStrides;

#ifdef FORCE_NULL
    return;
#endif

#ifdef PROFILE_INDEX
    nvLogIndexData(dwDP2Operation, (DWORD)wCount, pIndices);
#endif //PROFILE_INDEX

    // any work?
    if (dwCount == 0) return;

    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, dwCount);
    bLegacyStrides = (pContext->dp2.dwDP2Prim == D3DDP2OP_INDEXEDTRIANGLELIST) ? TRUE : FALSE;

    // book keeping
    if (pContext->pZetaBuffer) {
        pContext->pZetaBuffer->getWrapper()->setCTDrawPrim();
    }

    // lock surfaces for use by the HW
    nvHWLockTextures (pContext);
    nvHWLockStreams (pContext, KELVIN_CAPS_MAX_STREAMS);

#if SYSVB2AGP
    DWORD bAltVBPerfStrategy = ( (pContext->dwEarlyCopyStrategy) || (pContext->dwEarlyCopyStrategy > 0x20));

    if (bAltVBPerfStrategy) {

        if (pContext->dp2.dwDP2Prim == D3DDP2OP_INDEXEDTRIANGLELIST2 && pContext->dp2.dwVertexBufferOffset == 0)
        {
            if (!pContext->sysvb.pSysMemVB)
            {
                DWORD stream = pContext->hwState.pVertexShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION]);
                CVertexBuffer *pVertexBuffer = pContext->ppDX8Streams[stream];
                if (!pVertexBuffer->cachedhwCanRead())
                {

                    DWORD dwTotalSize = pContext->hwState.pVertexShader->getStride() * pContext->dp2.dwVertexLength;
                    if (dwTotalSize < pDriverData->nvD3DPerfData.dwRecommendedStageBufferSize)   // never fill more than 1/4 at once
                    {
                        pContext->sysvb.dwStreamDMACount = pContext->dwStreamDMACount;
                        pContext->dwStreamDMACount &= 0xffff;
                        // get default VB
                        CVertexBuffer *pDefVertexBuffer = getDC()->defaultVB.getVB();
                        DWORD stride = pContext->hwState.pVertexShader->getStride();

                        pDefVertexBuffer->setVertexStride(stride);

                        // check for space
                        DWORD dwVBOffset = (getDC()->defaultVB.waitForSpace(dwTotalSize + 64 * 12, TRUE) + 32) & ~31;
                        // copy verts
                        nvMemCopy (pDefVertexBuffer->getAddress() + dwVBOffset ,
                                   (DWORD)pVertexBuffer->getAddress(),
                                   dwTotalSize);
                        // set up supertri
                        pDefVertexBuffer->setSuperTriLookAsideBuffer ((void*)((DWORD)pVertexBuffer->getAddress()));
                        pContext->dp2.dwVertexBufferOffsetSave = pContext->dp2.dwVertexBufferOffset;
                        pContext->sysvb.dwDefVBOffset = pContext->dp2.dwVertexBufferOffset = dwVBOffset;

                        pContext->sysvb.pSysMemVB = pVertexBuffer;
                        pContext->sysvb.dwSysMemVBStream = stream;
                        pContext->ppDX8Streams[stream] = pDefVertexBuffer;

                        // unlock vb
                        getDC()->defaultVB.m_dwTotalVerticesSincePut += pContext->dp2.dwVertexLength;
                        if (getDC()->defaultVB.m_dwTotalVerticesSincePut >= 128)
                        {
                            pContext->sysvb.bWriteOffset = TRUE;
                            getDC()->defaultVB.m_dwTotalVerticesSincePut = 0;
                        }
                        getDC()->defaultVB.m_dwCurrentOffset = dwVBOffset + dwTotalSize;

                    }
                }
            }
            else
            {
                pContext->sysvb.dwStreamDMACount = pContext->dwStreamDMACount;
                pContext->dwStreamDMACount &= 0xffff;
                pContext->dp2.dwVertexBufferOffsetSave = pContext->dp2.dwVertexBufferOffset;
                pContext->dp2.dwVertexBufferOffset = pContext->sysvb.dwDefVBOffset;
                pContext->sysvb.pSysMemVB = pContext->ppDX8Streams[pContext->sysvb.dwSysMemVBStream];
                pContext->ppDX8Streams[pContext->sysvb.dwSysMemVBStream] = getDC()->defaultVB.getVB();
            }
        }
    }
#endif //SYSVB2AGP

    // set the vertex sources and if anything has changed, dirty the fvf flag
    if (pContext->hwState.dwVertexOffset != pContext->dp2.dwVertexBufferOffset)
    {
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
    }

    // ugh.  Special case to set up the combiners for pointsprites..
    if ((pContext->dwRenderState[D3DRS_POINTSPRITEENABLE]) &&
        (pContext->hwState.dwCombinerDP2Prim != pContext->dp2.dwDP2Prim))
    {
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_COMBINERS_COLOR;
    }

#ifdef KELVIN_ILC
// ToDo: Insert code to copy SYSMEM VB's to AGP here. Make it work for things other than test 6 & 7.

//     if (!pContext->ppDX8Streams[pContext->pCurrentVShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])]->cachedhwCanRead())
//     {
//         __asm int 3
//     }
#endif //KELVIN_ILC

    // set kelvin state (required to get the correct inner loop)
    nvSetKelvinState (pContext);

#ifdef FORCE_INLINE_VERTICES

    nvKelvinDispatchIndexedPrimitiveInline (pContext, dwCount, pIndices, bLegacyStrides);

#else // !FORCE_INLINE_VERTICES

#ifdef KELVIN_ILC
    // get inner loop entry point flags
    DWORD dwFlags = (pContext->dp2.dwDP2Prim & KELVIN_ILMASK_PRIMTYPE)
                  | (bLegacyStrides ? KELVIN_ILFLAG_LEGACY : 0);
#endif KELVIN_ILC


#ifdef KELVIN_ILC

    if (UseKelvinILC)
    {
        if (pContext->dp2.dwIndexStride == 4) {
            goto C_Code;
        }

        if (pContext->hwState.dwInlineVertexStride)
        {
            if ((pContext->hwState.pVertexShader->hasProgram())
             || (NV_NUM_UNIQUE_STREAMS(pContext->dwStreamDMACount) > 1) // asm code doesn't handle multiple streams
             || (NV_NUM_SYSTEM_STREAMS(pContext->dwStreamDMACount) > 1)) {
                goto C_Code;
            }

            if ((dwCount >= KELVIN_SHORT_INDEXED_PRIM_COUNT_CUTOFF) &&
                ((kelvinPrimitiveToPrimitiveMagic[pContext->dp2.dwDP2Prim] & 0x00ffffff) == 0x00030300))
            {
                if (NV_VERTEX_TRANSFORMED(pContext->hwState.pVertexShader)) {
                    dwFlags |= KELVIN_ILFLAG_IX_INL_TRI_LIST;
                }
                else {
                    dwFlags |= KELVIN_ILFLAG_IX_INL_PRIM;
                }
                dwFlags                    |= ((pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_NONE) ? KELVIN_ILFLAG_NOCULL : 0);
                global.kelvin.dwCullValue  =  (pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
            }
            else {
                dwFlags |= KELVIN_ILFLAG_IX_INL_PRIM;
            }
        }

        else
        {
            if (pContext->hwState.SuperTri.Strategy (pContext->dp2.dwDP2Prim, dwCount, pContext->hwState.pVertexShader))
            {
                dwFlags |= KELVIN_ILFLAG_SUPER_TRI_LIST;
            }
            else
            {
                dwFlags |= KELVIN_ILFLAG_IX_VB_PRIM;
            }
        }

        // setup variables passed to innerloop
        global.kelvin.pContext              = (DWORD)pContext;
        global.kelvin.pContext_kelvinState  = (DWORD)&pContext->hwState;
        global.kelvin.dwPrimCount           = dwCount;

        global.kelvin.dwVertexStride        = pContext->hwState.pVertexShader->getStride();  // vertex stride of source data
        global.kelvin.pIndices              = pContext->dp2.dwIndices;
        global.kelvin.dwIndexStride         = pContext->dp2.dwIndexStride;

        // BUGBUG this won't work with vertex shaders
        // pVertices is used in the inline functions and needs the base vertex (dwVStart) built in.
        // pVertices is not used in the VB routines - they use dwBaseVertex
        global.kelvin.pVertices             = pContext->hwState.ppStreams[pContext->hwState.pVertexShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])]->getAddress() +
                                              pContext->hwState.dwVertexOffset +
                                              pContext->dp2.dwVStart * pContext->hwState.ppStreams[pContext->hwState.pVertexShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])]->getVertexStride() +
                                              pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]);
        //set base vertex for use in VB Prim and super tri routines
        global.kelvin.dwBaseVertex          = pContext->dp2.dwVStart;

        // set component offsets
        global.kelvin.dwOffsetXYZ           = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]);
        global.kelvin.dwOffsetRHW           = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]) + 3*sizeof(DWORD);
        global.kelvin.dwOffsetWeight        = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_BLENDWEIGHT]);
        global.kelvin.dwOffsetNormal        = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_NORMAL]);
        global.kelvin.dwOffsetDiffuse       = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_DIFFUSE]);
        global.kelvin.dwOffsetSpecular      = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_SPECULAR]);
        global.kelvin.dwOffsetPointSize     = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_PSIZE]);
        for (DWORD i=0; i<KELVIN_CAPS_MAX_UV_PAIRS; i++) {
            global.kelvin.dwOffsetUV[i]     = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_TEXCOORD0+i]);
        }
        global.kelvin.dwEyeAddr             = pContext->hwState.SuperTri.getEyeAddr();
        global.kelvin.dwCullAddr            = pContext->hwState.SuperTri.getCullsignAddr();

        if ((dwFlags & KELVIN_ILMASK_LOOPTYPE) == KELVIN_ILFLAG_SUPER_TRI_LIST)
        {
            // do preprocessing
            dwFlags                   |= ((pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_NONE) ? KELVIN_ILFLAG_NOCULL : 0);
            global.kelvin.dwCullValue  = (pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
            CVertexBuffer *pVertexBufferInUse = pContext->ppDX8Streams[pContext->hwState.pVertexShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])];
            global.kelvin.pVertices           = (DWORD) pVertexBufferInUse->getSuperTriLookAsideBuffer() +
                                                pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]);
        }

        KELVINDISPATCHPRIMITIVE pfn = nvKelvinGetDispatchRoutine(pContext,dwFlags);
        if (pfn) pfn();
    }
    else
    {

#endif //KELVIN_ILC
C_Code:
    if (pContext->hwState.dwInlineVertexStride)
    {

        // vertices are not in video memory or not accessible by HW. shove them through the push buffer
        nvKelvinDispatchIndexedPrimitiveInline (pContext, dwCount, bLegacyStrides);
    }
    else
    {
        // vertices are in video memory. allow NV20 to fetch them from there

        nvAssert (kelvinBeginEndOp[pContext->dp2.dwDP2Prim] != ~0);
        nvglSetNv20KelvinBeginEnd (NV_DD_KELVIN, kelvinBeginEndOp[pContext->dp2.dwDP2Prim]);

        // determine the number of indices
        DWORD dwScaleAndBias = kelvinPrimitiveToPrimitiveMagic[pContext->dp2.dwDP2Prim];
        nvAssert (dwScaleAndBias != ~0);
        DWORD dwIndexCount = (((dwScaleAndBias >> 8) & 0xff) * dwCount) + (dwScaleAndBias & 0xff);

        // cache index info
        PBYTE pIndices  = (PBYTE)(pContext->dp2.dwIndices);
        DWORD dwIStride = pContext->dp2.dwIndexStride;

        // dump 32-bit blocks of indices
        DWORD dw32BitCount, dw32BitStart, dwMethod;
        if (dwIStride == 2) {
            dw32BitCount = dwIndexCount >> 1;
            dw32BitStart = (pContext->dp2.dwVStart << 16) | (pContext->dp2.dwVStart << 0);
            dwMethod     = NV097_ARRAY_ELEMENT16;
        }
        else {
            nvAssert (dwIStride == 4);
            dw32BitCount = dwIndexCount;
            dw32BitStart = pContext->dp2.dwVStart;
            dwMethod     = NV097_ARRAY_ELEMENT32;
        }

        while (dw32BitCount) {
            DWORD dwCount = min (dw32BitCount, KELVIN_MAX_NONINC_DATA);
            getDC()->nvPusher.makeSpace (sizeSetStartMethod + dwCount);
            nvglSetStartMethod (0, NV_DD_KELVIN, NONINC_METHOD(dwMethod), dwCount);
            getDC()->nvPusher.inc (sizeSetStartMethod);
            for (DWORD i=0; i<dwCount; i++) {
                nvglSetData (i, *((DWORD*)pIndices) + dw32BitStart);
                pIndices += 4;
            }
            getDC()->nvPusher.inc (dwCount);
            dw32BitCount -= dwCount;
        }

        // handle the odd 16-bit index if there is one
        if ((dwIStride == 2) && (dwIndexCount & 0x1)) {
            getDC()->nvPusher.makeSpace (sizeSetStartMethod + 1);
            nvglSetStartMethod (0, NV_DD_KELVIN, NONINC_METHOD(NV097_ARRAY_ELEMENT32), 1);
            getDC()->nvPusher.inc (sizeSetStartMethod);
            nvglSetData (0, (DWORD)(*((WORD*)pIndices)) + pContext->dp2.dwVStart);
            getDC()->nvPusher.inc (1);
        }

        nvglSetNv20KelvinBeginEnd (NV_DD_KELVIN, NV097_SET_BEGIN_END_OP_END);

    }


#ifdef KELVIN_ILC
    }
#endif



#endif // !FORCE_INLINE_VERTICES

#if SYSVB2AGP
    if (pContext->sysvb.pSysMemVB)
    {
        DWORD stream = pContext->sysvb.dwSysMemVBStream;
        pContext->ppDX8Streams[pContext->sysvb.dwSysMemVBStream] = pContext->sysvb.pSysMemVB;
        pContext->sysvb.pSysMemVB = getDC()->defaultVB.getVB();
        pContext->dwStreamDMACount = pContext->sysvb.dwStreamDMACount;
        pContext->dp2.dwVertexBufferOffset = pContext->dp2.dwVertexBufferOffsetSave;
    }
#endif
    //set that we need to unlock all surfaces.  defer till we need to to lower number of CPU
    //clocks per primitive.
    pContext->dwHWUnlockAllPending = TRUE;
}

//---------------------------------------------------------------------------

void nvKelvinDispatchNonIndexedPrimitive
(
    NV_INNERLOOP_ARGLIST
)
{
#ifdef FORCE_NULL
    return;
#endif

    // any work?
    if (dwCount == 0) return;

    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)dwCount);
    // book keeping
    if (pContext->pZetaBuffer) {
        pContext->pZetaBuffer->getWrapper()->setCTDrawPrim();
    }

    // lock surfaces for use by the HW
    nvHWLockTextures (pContext);
    nvHWLockStreams (pContext, KELVIN_CAPS_MAX_STREAMS);

    // set the vertex sources and if anything has changed, dirty the fvf flag
    if (pContext->hwState.dwVertexOffset != pContext->dp2.dwVertexBufferOffset)
    {
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;

    }
    // ugh.  Special case to set up the combiners for pointsprites..
    if ((pContext->dwRenderState[D3DRS_POINTSPRITEENABLE]) &&
        (pContext->hwState.dwCombinerDP2Prim != pContext->dp2.dwDP2Prim))
    {
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_COMBINERS_COLOR;
    }

    // set kelvin state (required to get the correct inner loop)
    nvSetKelvinState (pContext);

#ifdef FORCE_INLINE_VERTICES

    nvKelvinDispatchNonIndexedPrimitiveInline (pContext, dwCount);

#else // !FORCE_INLINE_VERTICES

    if (pContext->hwState.dwInlineVertexStride) {

        nvKelvinDispatchNonIndexedPrimitiveInline (pContext, dwCount);

    }

    else {

        // vertices are in video memory. allow NV20 to fetch them from there

        nvAssert (kelvinBeginEndOp[pContext->dp2.dwDP2Prim] != ~0);
        nvglSetNv20KelvinBeginEnd (NV_DD_KELVIN, kelvinBeginEndOp[pContext->dp2.dwDP2Prim]);

        DWORD dwScaleAndBias = kelvinPrimitiveToPrimitiveMagic[pContext->dp2.dwDP2Prim];
        nvAssert (dwScaleAndBias != ~0);
        DWORD dwNumVertices = (((dwScaleAndBias >> 8) & 0xff) * dwCount) + (dwScaleAndBias & 0xff);

        DWORD dwIndex = 0;
        while (dwNumVertices > KELVIN_MAX_INLINE_VERTICES) {
            nvglSetNv20KelvinDrawArrays (NV_DD_KELVIN,
                                         DRF_NUM(097, _DRAW_ARRAYS, _COUNT, KELVIN_MAX_INLINE_VERTICES-1) |
                                         DRF_NUM(097, _DRAW_ARRAYS, _START_INDEX, dwIndex + pContext->dp2.dwVStart));
            dwNumVertices -= KELVIN_MAX_INLINE_VERTICES;
            dwIndex += KELVIN_MAX_INLINE_VERTICES;
        }
        if (dwNumVertices) {
            nvglSetNv20KelvinDrawArrays (NV_DD_KELVIN,
                                         DRF_NUM(097, _DRAW_ARRAYS, _COUNT, dwNumVertices-1) |
                                         DRF_NUM(097, _DRAW_ARRAYS, _START_INDEX, dwIndex + pContext->dp2.dwVStart));
        }

        nvglSetNv20KelvinBeginEnd (NV_DD_KELVIN, NV097_SET_BEGIN_END_OP_END);

    }

#endif // !FORCE_INLINE_VERTICES

    //set that we need to unlock all surfaces.  defer till we need to to lower number of CPU
    //clocks per primitive.
    pContext->dwHWUnlockAllPending = TRUE;
}

//---------------------------------------------------------------------------

void nvKelvinDispatchLegacyWireframePrimitive
(
    NV_INNERLOOP_ARGLIST
)
{
    nvAssert (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE] == D3DFILL_WIREFRAME);

    DWORD dwCachedDP2Op     = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim = D3DDP2OP_INDEXEDLINELIST;

    if (dwCachedDP2Op == D3DDP2OP_INDEXEDTRIANGLELIST) {

        LPD3DHAL_DP2INDEXEDTRIANGLELIST pTriListData;
        DWORD dwEdge;
        WORD  wIndicesIn[3];
        WORD  wIndicesOut[2];

        pTriListData = (LPD3DHAL_DP2INDEXEDTRIANGLELIST)(pContext->dp2.dwIndices);
        pContext->dp2.dwIndices     = (DWORD)(&(wIndicesOut[0]));
        pContext->dp2.dwIndexStride = 2;

        while (dwCount) {

            wIndicesIn[0] = pTriListData->wV1;
            wIndicesIn[1] = pTriListData->wV2;
            wIndicesIn[2] = pTriListData->wV3;

            for (dwEdge=0; dwEdge<3; dwEdge++) {
                if (pTriListData->wFlags & (D3DTRIFLAG_EDGEENABLE1 << dwEdge)) {
                    wIndicesOut[0] = wIndicesIn [(dwEdge+0) % 3];
                    wIndicesOut[1] = wIndicesIn [(dwEdge+1) % 3];
                    nvKelvinDispatchIndexedPrimitive (pContext, 1);
                }
            }

            pTriListData ++;
            dwCount --;

        }

    }

    else if ((dwCachedDP2Op == D3DDP2OP_TRIANGLEFAN_IMM) || (dwCachedDP2Op == D3DDP2OP_TRIANGLEFAN)) {

        DWORD dwEdgeFlags, dwEdgeMask;
        WORD  wTriNum;
        WORD  wIndicesOut[2];

        nvAssert (dwCount < 32);

        dwEdgeFlags = pContext->dp2.dwEdgeFlags;
        dwEdgeMask  = 0x1;

        pContext->dp2.dwIndices     = (DWORD)(&(wIndicesOut[0]));
        pContext->dp2.dwIndexStride = 2;

        // we never draw edge v0-v1 for anything but the first triangle
        wIndicesOut[0] = 1;
        wIndicesOut[1] = 0;
        if (dwEdgeFlags & dwEdgeMask) {
            nvKelvinDispatchIndexedPrimitive (pContext, 1);
            dbgFlushType (NVDBG_FLUSH_PRIMITIVE);
        }
        dwEdgeMask <<= 1;

        // we may draw edge v1-v2 for any or all triangles
        for (wTriNum=0; wTriNum<dwCount; wTriNum++) {
            wIndicesOut[0] = wTriNum+1;
            wIndicesOut[1] = wTriNum+2;
            if (dwEdgeFlags & dwEdgeMask) {
                nvKelvinDispatchIndexedPrimitive (pContext, 1);
                dbgFlushType (NVDBG_FLUSH_PRIMITIVE);
            }
            dwEdgeMask <<= 1;
        }

        // we never draw edge v2-v0 for anything but the last triangle
        wIndicesOut[0] = dwCount+1;
        wIndicesOut[1] = 0;
        if (dwEdgeFlags & dwEdgeMask) {
            nvKelvinDispatchIndexedPrimitive (pContext, 1);
            dbgFlushType (NVDBG_FLUSH_PRIMITIVE);
        }
        dwEdgeMask <<= 1;

    }

    else {

        // we don't handle any other legacy primitives
        nvAssert (0);

    }

    pContext->dp2.dwDP2Prim = dwCachedDP2Op;
}

/*****************************************************************************
 *****************************************************************************
 *** Inline Vertex Code ******************************************************
 *****************************************************************************
 *****************************************************************************/

// this code is typically unused. it's used only for debugging
// purposes if FORCE_INLINE_VERTICES is defined

// table mapping dp2ops to the scale and bias required to calculate the number
// of vertices from the number of primitives. the scale is in the upper half
// of the word and the bias is in the lower.
// numVertices = scale * numPrimitives + bias

DWORD kelvinPrimitiveToVertexCountScaleAndBias[D3D_DP2OP_MAX+1] =
{
    ~0,                                          //   0  invalid
    0x00010000,                                  //   1  D3DDP2OP_POINTS
    0x00020000,                                  //   2  D3DDP2OP_INDEXEDLINELIST
    0x00030000,                                  //   3  D3DDP2OP_INDEXEDTRIANGLELIST
    ~0,                                          //   4  invalid
    ~0,                                          //   5  invalid
    ~0,                                          //   6  invalid
    ~0,                                          //   7  invalid
    ~0,                                          //   8  D3DDP2OP_RENDERSTATE
    ~0,                                          //   9  invalid
    ~0,                                          //  10  invalid
    ~0,                                          //  11  invalid
    ~0,                                          //  12  invalid
    ~0,                                          //  13  invalid
    ~0,                                          //  14  invalid
    0x00020000,                                  //  15  D3DDP2OP_LINELIST
    0x00010001,                                  //  16  D3DDP2OP_LINESTRIP
    0x00010001,                                  //  17  D3DDP2OP_INDEXEDLINESTRIP
    0x00030000,                                  //  18  D3DDP2OP_TRIANGLELIST
    0x00010002,                                  //  19  D3DDP2OP_TRIANGLESTRIP
    0x00010002,                                  //  20  D3DDP2OP_INDEXEDTRIANGLESTRIP
    0x00010002,                                  //  21  D3DDP2OP_TRIANGLEFAN
    0x00010002,                                  //  22  D3DDP2OP_INDEXEDTRIANGLEFAN
    0x00010002,                                  //  23  D3DDP2OP_TRIANGLEFAN_IMM
    0x00020000,                                  //  24  D3DDP2OP_LINELIST_IMM
    ~0,                                          //  25  D3DDP2OP_TEXTURESTAGESTATE
    0x00030000,                                  //  26  D3DDP2OP_INDEXEDTRIANGLELIST2
    0x00020000,                                  //  27  D3DDP2OP_INDEXEDLINELIST2
    ~0,                                          //  28  D3DDP2OP_VIEWPORTINFO
    ~0,                                          //  29  D3DDP2OP_WINFO
    ~0,                                          //  30  D3DDP2OP_SETPALETTE
    ~0,                                          //  31  D3DDP2OP_UPDATEPALETTE
    ~0,                                          //  32  D3DDP2OP_ZRANGE
    ~0,                                          //  33  D3DDP2OP_SETMATERIAL
    ~0,                                          //  34  D3DDP2OP_SETLIGHT
    ~0,                                          //  35  D3DDP2OP_CREATELIGHT
    ~0,                                          //  36  D3DDP2OP_SETTRANSFORM
    ~0,                                          //  37  D3DDP2OP_EXT
    ~0,                                          //  38  D3DDP2OP_TEXBLT
    ~0,                                          //  39  D3DDP2OP_STATESET
    ~0,                                          //  40  D3DDP2OP_SETPRIORITY
    ~0,                                          //  41  D3DDP2OP_SETRENDERTARGET
    ~0,                                          //  42  D3DDP2OP_CLEAR
    ~0,                                          //  43  D3DDP2OP_SETTEXLOD
    ~0                                           //  44  D3DDP2OP_SETCLIPPLANE
};

//---------------------------------------------------------------------------

// dump one vertex attribute into the push buffer (conditional on its existence)

__inline void nvKelvinDumpInlineVA
(
    DWORD           dwReg,
    CVertexShader  *pShader,
    CVertexBuffer **ppStreams,
    DWORD           dwVertexBufferOffset,
    DWORD           dwIndex,
    DWORD          *dwPutIndex
#ifdef STOMP_TEX_COORDS
    BOOL           ,bStompLast
#endif  // STOMP_TEX_COORDS
)
{
    DWORD dwStreamSelector, pComponent, dwCount, i;

    dwStreamSelector = pShader->getVAStream(dwReg);
    if (dwStreamSelector != CVertexShader::VA_STREAM_NONE) {
        nvAssert (ppStreams[dwStreamSelector]);
        pComponent = ppStreams[dwStreamSelector]->getAddress()                // address of buffer
                   + dwVertexBufferOffset                                     // global offset within buffer
                   + pShader->getVAOffset (dwReg)                             // offset of this component
                   + dwIndex*ppStreams[dwStreamSelector]->getVertexStride();  // offset of this particular vertex
        dwCount = pShader->getVASize (dwReg) / sizeof(DWORD);
        for (i=0; i<dwCount; i++) {
            nvglSetData (*dwPutIndex, ((DWORD*)pComponent)[i]);
            (*dwPutIndex) ++;
        }
#ifdef STOMP_TEX_COORDS
        if (bStompLast) {
            nvglSetData ((*dwPutIndex)-1, FP_ONE_BITS);
        }
#endif  // STOMP_TEX_COORDS
    }
}

//---------------------------------------------------------------------------

void nvKelvinDumpDebugVertex
(
    PNVD3DCONTEXT pContext,
    DWORD         dwIndex
)
{
    CVertexShader  *pShader;
    CVertexBuffer **ppStreams;
    DWORD           dwVertexBufferOffset;
    DWORD           dwPBIndex, dwReg;
    DWORD           dwHWStage, dwD3DStage, dwTCIndex, dwVAIndex;

    pShader              = pContext->hwState.pVertexShader;
    ppStreams            = pContext->hwState.ppStreams;
    dwVertexBufferOffset = pContext->hwState.dwVertexOffset;
    dwIndex             += pContext->dp2.dwVStart; //add in the base vertex
    dwPBIndex=0;

    DWORD  *pComponent;
    DWORD   dwSs;
    DWORD   flag = NVDBG_SHOW_VERTICES;

    dbgDPFAndDump(flag, "\nkelvin vertex, index = %d", dwIndex);

    // x,y,z,w
    dwReg=defaultInputRegMap[D3DVSDE_POSITION];
    dwSs=pShader->getVAStream(dwReg);
    if (dwSs!=CVertexShader::VA_STREAM_NONE)
    {
        pComponent = (DWORD*)(ppStreams[dwSs]->getAddress()
            + dwVertexBufferOffset
            + pShader->getVAOffset (dwReg)
            + dwIndex*ppStreams[dwSs]->getVertexStride());

        dbgDPFAndDump(flag, "             X = %f",    FLOAT_FROM_DWORD(pComponent[0]));
        dbgDPFAndDump(flag, "             Y = %f",    FLOAT_FROM_DWORD(pComponent[1]));
        dbgDPFAndDump(flag, "             Z = %f",    FLOAT_FROM_DWORD(pComponent[2]));
        dbgDPFAndDump(flag, "             W = %f",    FLOAT_FROM_DWORD(pComponent[3]));
    }

    // blendweight
    dwReg=defaultInputRegMap[D3DVSDE_BLENDWEIGHT];
    dwSs=pShader->getVAStream(dwReg);
    if (dwSs!=CVertexShader::VA_STREAM_NONE)
    {
        pComponent = (DWORD*)(ppStreams[dwSs]->getAddress()
            + dwVertexBufferOffset
            + pShader->getVAOffset (dwReg)
            + dwIndex*ppStreams[dwSs]->getVertexStride());

        dbgDPFAndDump(flag, " blendWeight = %f",    FLOAT_FROM_DWORD(pComponent[0]));
    }

    // normal
    dwReg=defaultInputRegMap[D3DVSDE_NORMAL];
    dwSs=pShader->getVAStream(dwReg);
    if (dwSs!=CVertexShader::VA_STREAM_NONE)
    {
        pComponent = (DWORD*)(ppStreams[dwSs]->getAddress()
            + dwVertexBufferOffset
            + pShader->getVAOffset (dwReg)
            + dwIndex*ppStreams[dwSs]->getVertexStride());

        dbgDPFAndDump(flag, "    nX       = %f",    FLOAT_FROM_DWORD(pComponent[0]));
        dbgDPFAndDump(flag, "    nY       = %f",    FLOAT_FROM_DWORD(pComponent[1]));
        dbgDPFAndDump(flag, "    nZ       = %f",    FLOAT_FROM_DWORD(pComponent[2]));
    }

    // diffuse
    dwReg=defaultInputRegMap[D3DVSDE_DIFFUSE];
    dwSs=pShader->getVAStream(dwReg);
    if (dwSs!=CVertexShader::VA_STREAM_NONE)
    {
        pComponent = (DWORD*)(ppStreams[dwSs]->getAddress()
            + dwVertexBufferOffset
            + pShader->getVAOffset (dwReg)
            + dwIndex*ppStreams[dwSs]->getVertexStride());

        dbgDPFAndDump(flag, "     diffuse = %x",    pComponent[0]);
    }

    // specular
    dwReg=defaultInputRegMap[D3DVSDE_SPECULAR];
    dwSs=pShader->getVAStream(dwReg);
    if (dwSs!=CVertexShader::VA_STREAM_NONE)
    {
        pComponent = (DWORD*)(ppStreams[dwSs]->getAddress()
            + dwVertexBufferOffset
            + pShader->getVAOffset (dwReg)
            + dwIndex*ppStreams[dwSs]->getVertexStride());

        dbgDPFAndDump(flag, "    specular = %x",    pComponent[0]);
    }

    for (dwHWStage=0; dwHWStage < KELVIN_NUM_TEXTURES; dwHWStage++)
    {
        if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(dwHWStage))
        {
            dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
            nvAssert (dwD3DStage != KELVIN_UNUSED);
            dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> (8*dwHWStage)) & 0xff;  // BUGBUG?? are these indices even relevant in DX8?
            dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            nvAssert (pShader->bVAExists(dwVAIndex));  // they better have given us coordinates


            dwReg=dwVAIndex;
            dwSs=pShader->getVAStream(dwReg);
            if (dwSs!=CVertexShader::VA_STREAM_NONE)
            {
                pComponent = (DWORD*)(ppStreams[dwSs]->getAddress()
                    + dwVertexBufferOffset
                    + pShader->getVAOffset (dwReg)
                    + dwIndex*ppStreams[dwSs]->getVertexStride());

                switch (pShader->getVASize(dwReg)>>2) {
                case 1: dbgDPFAndDump(flag, "    Texture %d = <%f>", dwHWStage, FLOAT_FROM_DWORD(pComponent[0])); break;
                case 2: dbgDPFAndDump(flag, "    Texture %d = <%f, %f>", dwHWStage, FLOAT_FROM_DWORD(pComponent[0]), FLOAT_FROM_DWORD(pComponent[1])); break;
                case 3: dbgDPFAndDump(flag, "    Texture %d = <%f, %f, %f>", dwHWStage, FLOAT_FROM_DWORD(pComponent[0]), FLOAT_FROM_DWORD(pComponent[1]), FLOAT_FROM_DWORD(pComponent[2])); break;
                case 4: dbgDPFAndDump(flag, "    Texture %d = <%f, %f, %f, %f>", dwHWStage, FLOAT_FROM_DWORD(pComponent[0]), FLOAT_FROM_DWORD(pComponent[1]), FLOAT_FROM_DWORD(pComponent[2]), FLOAT_FROM_DWORD(pComponent[3])); break;
                default: dbgDPFAndDump(flag, "    Invalid number of texture coordinates"); dbgD3DError();
                }
            }
        }
    }
}

//---------------------------------------------------------------------------

// dumps one vertex into the push buffer with the components in the order
// kelvin requires. note that adequate push buffer space is assumed to have
// been verified already!

void nvKelvinDumpInlineVertex
(
    PNVD3DCONTEXT pContext,
    DWORD         dwIndex
)
{
    CVertexShader  *pShader;
    CVertexBuffer **ppStreams;
    DWORD           dwVertexBufferOffset;
    DWORD           dwPBIndex, dwReg;
    DWORD           dwHWStage, dwD3DStage, dwTCIndex, dwVAIndex;

    pShader              = pContext->hwState.pVertexShader;
    ppStreams            = pContext->hwState.ppStreams;
    dwVertexBufferOffset = pContext->hwState.dwVertexOffset;
    dwIndex             += pContext->dp2.dwVStart; //add in the base vertex
    dwPBIndex=0;

    if (pShader->hasProgram()) {

        // dump all enabled vertex attributes
        for (dwReg=0; dwReg < NV_CAPS_MAX_STREAMS; dwReg++) {
#ifdef STOMP_TEX_COORDS
            nvKelvinDumpInlineVA (dwReg, pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex, FALSE);
#else  // !STOMP_TEX_COORDS
            nvKelvinDumpInlineVA (dwReg, pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex);
#endif  // !STOMP_TEX_COORDS
        }
        if (pContext->hwState.dwInlineVertexStride == 4) //workaround for kelvin bug
        {
            //add some extra
            nvglSetData (dwPBIndex, 0);
            dwPBIndex++;
        }
    }
    else {

        if ((dbgShowState & NVDBG_SHOW_VERTICES) || (dbgDumpState & NVDBG_SHOW_VERTICES)) {
            nvKelvinDumpDebugVertex (pContext, dwIndex);
        }

#ifdef STOMP_TEX_COORDS
        // dump pre-ordained components
        nvKelvinDumpInlineVA (defaultInputRegMap[D3DVSDE_POSITION],    pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex, FALSE);
        nvKelvinDumpInlineVA (defaultInputRegMap[D3DVSDE_BLENDWEIGHT], pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex, FALSE);
        nvKelvinDumpInlineVA (defaultInputRegMap[D3DVSDE_NORMAL],      pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex, FALSE);
        nvKelvinDumpInlineVA (defaultInputRegMap[D3DVSDE_DIFFUSE],     pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex, FALSE);
        nvKelvinDumpInlineVA (defaultInputRegMap[D3DVSDE_SPECULAR],    pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex, FALSE);
        nvKelvinDumpInlineVA (defaultInputRegMap[D3DVSDE_PSIZE],       pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex, FALSE);
        for (dwHWStage=0; dwHWStage < KELVIN_NUM_TEXTURES; dwHWStage++) {
            if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(dwHWStage)) {
                dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
                nvAssert (dwD3DStage != KELVIN_UNUSED);
                dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> (8*dwHWStage)) & 0xff;  // BUGBUG?? are these indices even relevant in DX8?
                dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
                nvAssert (pShader->bVAExists(dwVAIndex));  // they better have given us coordinates
                BOOL bStompLast = (pContext->hwState.dwStateFlags & KELVIN_FLAG_STOMP_4TH_COORD(dwHWStage));
                nvKelvinDumpInlineVA (dwVAIndex, pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex, bStompLast);
            }
        }
#else  // !STOMP_TEX_COORDS
        // dump pre-ordained components
        nvKelvinDumpInlineVA (defaultInputRegMap[D3DVSDE_POSITION],    pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex);
        nvKelvinDumpInlineVA (defaultInputRegMap[D3DVSDE_BLENDWEIGHT], pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex);
        nvKelvinDumpInlineVA (defaultInputRegMap[D3DVSDE_NORMAL],      pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex);
        nvKelvinDumpInlineVA (defaultInputRegMap[D3DVSDE_DIFFUSE],     pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex);
        nvKelvinDumpInlineVA (defaultInputRegMap[D3DVSDE_SPECULAR],    pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex);
        nvKelvinDumpInlineVA (defaultInputRegMap[D3DVSDE_PSIZE],       pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex);
        for (dwHWStage=0; dwHWStage < KELVIN_NUM_TEXTURES; dwHWStage++) {
            if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(dwHWStage)) {
                dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
                nvAssert (dwD3DStage != KELVIN_UNUSED);
                dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> (8*dwHWStage)) & 0xff;  // BUGBUG?? are these indices even relevant in DX8?
                dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
                nvAssert (pShader->bVAExists(dwVAIndex));  // they better have given us coordinates
                nvKelvinDumpInlineVA (dwVAIndex, pShader, ppStreams, dwVertexBufferOffset, dwIndex, &dwPBIndex);
            }
        }
#endif  // !STOMP_TEX_COORDS

    }

    nvAssert ((dwPBIndex == (pContext->hwState.dwInlineVertexStride >> 2)) || pContext->hwState.dwInlineVertexStride == 4 );
}

//---------------------------------------------------------------------------

// dumps a block of 'dwCount' non-indexed vertices starting with index 'i0'

void nvKelvinDumpNonIndexedInlineVertexBlock
(
    PNVD3DCONTEXT pContext,
    DWORD         i0,
    DWORD         dwCount,
    DWORD         dwInlineVertexStride
)
{
    DWORD dwMethodCount;

    dwMethodCount = dwCount * dwInlineVertexStride;  // number of 4-byte words
    nvAssert (dwMethodCount < (1<<11));

    getDC()->nvPusher.makeSpace (sizeSetStartMethod + dwMethodCount);
    nvglSetStartMethod (0, NV_DD_KELVIN, NVPUSHER_NOINC(NV097_INLINE_ARRAY), dwMethodCount);
    getDC()->nvPusher.inc (sizeSetStartMethod);

    for (DWORD i=i0; i<i0+dwCount; i++) {
        nvKelvinDumpInlineVertex (pContext, i);
        getDC()->nvPusher.inc (dwInlineVertexStride);
    }
}

#ifndef VERTEX_REUSE

//---------------------------------------------------------------------------

// dumps a block of 'dwCount' indexed vertices beginning with index 'i0'

void nvKelvinDumpIndexedInlineVertexBlock
(
    PNVD3DCONTEXT pContext,
    DWORD         i0,
    DWORD         dwCount,
    BOOL          bLegacyStrides,
    DWORD         dwInlineVertexStride
)
{
    DWORD dwMethodCount;
    DWORD i, i1;
    PWORD lpIndices  = (PWORD)(pContext->dp2.dwIndices);

    dwMethodCount = dwCount * dwInlineVertexStride;  // number of 4-byte words
    nvAssert (dwMethodCount < 2048);

    getDC()->nvPusher.makeSpace (sizeSetStartMethod + dwMethodCount);
    nvglSetStartMethod (0, NV_DD_KELVIN, NVPUSHER_NOINC(NV097_INLINE_ARRAY), dwMethodCount);
    getDC()->nvPusher.inc (sizeSetStartMethod);

    if (bLegacyStrides) {

        // stupid old style indexing - {index16:index16 index16:junk16} repeat
        for (i=i0; i < (i0+dwCount); i++) {
            i1 = 4 * i / 3;  // offset of the i'th index
            nvKelvinDumpInlineVertex (pContext, lpIndices[i1]);
            getDC()->nvPusher.inc (dwInlineVertexStride);
        }

    }

    else {

        // new indexing - fully packed
        for (i=i0; i < (i0+dwCount); i++) {
            nvKelvinDumpInlineVertex (pContext, lpIndices[i]);
            getDC()->nvPusher.inc (dwInlineVertexStride);
        }

    }
}

#else  // VERTEX_REUSE


#if 0 // old way

//---------------------------------------------------------------------------

// dumps a block of 'dwCount' indexed vertices beginning with index 'i0'

void nvKelvinDumpIndexedInlineVertexBlock
(
    PNVD3DCONTEXT pContext,
    LPWORD        lpIndices,
    DWORD         i0,
    DWORD         dwCount,
    BOOL          bLegacyStrides,
    DWORD         dwInlineVertexStride
)
{
    DWORD dwMethodCount;
    DWORD i, i1;
    DWORD adwBitField[16];
    DWORD dwIndexMask;
    DWORD dwBitIndex, dwBit;
#ifdef DEBUG
    DWORD dwNewCount = 0;
    DWORD dwReuseCount = 0;
#endif

    if (bLegacyStrides) {

        // stupid old style indexing - {index16:index16 index16:junk16} repeat

        dwMethodCount = dwCount * dwInlineVertexStride;  // number of 4-byte words
        nvAssert (dwMethodCount <= 2048);

        getDC()->nvPusher.makeSpace (sizeSetStartMethod + dwMethodCount);
        nvglSetStartMethod (0, NV_DD_KELVIN, NVPUSHER_NOINC(NV097_INLINE_ARRAY), dwMethodCount);
        getDC()->nvPusher.inc (sizeSetStartMethod);

        for (i=i0; i < (i0+dwCount); i++) {
            i1 = 4 * i / 3;  // offset of the i'th index
            nvKelvinDumpInlineVertex (pContext, lpIndices[i1]);
            getDC()->nvPusher.inc (dwInlineVertexStride);
        }

    }

    else {

        // new indexing - fully packed

        // conservatively make space as if we're not going to reuse any
        getDC()->nvPusher.makeSpace (dwCount * (sizeSetStartMethod + dwInlineVertexStride));

        // clear the cached values
        memset ((void*)adwBitField, 0, 16*sizeof(DWORD));

        for (i=i0; i < (i0+dwCount); i++) {

            i1 = lpIndices[i];

            // see if we've used this index before
            dwIndexMask = ~0;
            for (dwBitIndex=0; dwBitIndex<16; dwBitIndex++) {
                dwBit = (i1 >> dwBitIndex) & 0x1;
                dwIndexMask &= ((adwBitField[dwBitIndex] >> (dwBit<<4)) & 0xffff);
            }

            if (dwIndexMask) {

                // we've seen this index before. re-use the vertex
#ifdef DEBUG
                dwReuseCount++;

                // only one bit should be set
                DWORD aa = dwIndexMask;
                DWORD bb = dwIndexMask;
                ASM_LOG2(aa);
                ASM_LOG2_R(bb);
                nvAssert (aa==bb);
#endif
                ASM_LOG2(dwIndexMask);
                nvglSetStartMethod (0, NV_DD_KELVIN, NV097_INLINE_VERTEX_REUSE, 1);
                nvglSetData (1, dwIndexMask);
                getDC()->nvPusher.inc (2);

            }

            else {

                // this is a new index. emit the vertex and update cached indices
#ifdef DEBUG
                dwNewCount++;
#endif
                nvglSetStartMethod (0, NV_DD_KELVIN, NVPUSHER_NOINC(NV097_INLINE_ARRAY), dwInlineVertexStride);
                getDC()->nvPusher.inc (sizeSetStartMethod);
                nvKelvinDumpInlineVertex (pContext, i1);
                getDC()->nvPusher.inc (dwInlineVertexStride);
                for (dwBitIndex=0; dwBitIndex<16; dwBitIndex++) {
                    dwBit = (i1 >> dwBitIndex) & 0x1;
                    adwBitField[dwBitIndex] <<= 1;
                    adwBitField[dwBitIndex]  &= 0xfffefffe;
                    adwBitField[dwBitIndex]  |= ((dwBit << 16) | (dwBit ^ 0x1));
                }

            }

        }

    }

#ifdef DEBUG
    DPF_LEVEL (NVDBG_LEVEL_VERTEX_REUSE, "Vertex Reuse Statistics:");
    DPF_LEVEL (NVDBG_LEVEL_VERTEX_REUSE, "      new : %d (%.2f)", dwNewCount, (float)dwNewCount/(dwNewCount+dwReuseCount));
    DPF_LEVEL (NVDBG_LEVEL_VERTEX_REUSE, "   reused : %d (%.2f)", dwReuseCount, (float)dwReuseCount/(dwNewCount+dwReuseCount));
    DPF_LEVEL (NVDBG_LEVEL_VERTEX_REUSE, "    total : %d (%.2f)", dwNewCount+dwReuseCount, 100.f);
#endif

}


#else // new way

//---------------------------------------------------------------------------

// dumps a block of 'dwCount' indexed vertices beginning with index 'i0'

#define INDEX_HISTORY_SIZE 16
#define INDEX_HISTORY_MASK (INDEX_HISTORY_SIZE-1)
void nvKelvinDumpIndexedInlineVertexBlock
(
    PNVD3DCONTEXT pContext,
    DWORD         i0,
    DWORD         dwCount,
    BOOL          bLegacyStrides,
    DWORD         dwInlineVertexStride
)
{
    DWORD dwMethodCount;
    DWORD i, i1;
    DWORD dwReuseIndex, dwVertexIndex;
    DWORD adwIndexHistory[INDEX_HISTORY_SIZE];
    DWORD dwCurrentHistoryIndex, dwHistoryIndex;

    PBYTE pIndices  = (PBYTE)(pContext->dp2.dwIndices);
    DWORD dwIStride = pContext->dp2.dwIndexStride;

    if (bLegacyStrides) {

        // stupid old style indexing - {index16:index16 index16:junk16} repeat

        dwMethodCount = dwCount * dwInlineVertexStride;  // number of 4-byte words
        nvAssert (dwMethodCount <= 2048);
        nvAssert (dwIStride == 2);  // legacy shouldn't be using 32-bit indices

        getDC()->nvPusher.makeSpace (sizeSetStartMethod + dwMethodCount);
        nvglSetStartMethod (0, NV_DD_KELVIN, NVPUSHER_NOINC(NV097_INLINE_ARRAY), dwMethodCount);
        getDC()->nvPusher.inc (sizeSetStartMethod);

        for (i=i0; i < (i0+dwCount); i++) {
            i1 = 4 * i / 3;  // offset of the i'th index
            nvKelvinDumpInlineVertex (pContext, ((WORD*)pIndices)[i1]);
            getDC()->nvPusher.inc (dwInlineVertexStride);
        }

    }

    else {

        // new indexing - fully packed

        // conservatively make space as if we're not going to reuse any
        getDC()->nvPusher.makeSpace (dwCount * (sizeSetStartMethod + dwInlineVertexStride));

        // clear the history
        dwCurrentHistoryIndex = 0;
        memset ((void*)adwIndexHistory, 0xff, INDEX_HISTORY_SIZE*sizeof(DWORD));

        for (i=i0; i < (i0+dwCount); i++) {

            dwVertexIndex = (dwIStride == 2) ? ((WORD*)pIndices)[i] : ((DWORD*)pIndices)[i];

            // see if we've used this index before
            dwHistoryIndex = dwCurrentHistoryIndex;
            for (dwReuseIndex=0; dwReuseIndex<INDEX_HISTORY_SIZE; dwReuseIndex++) {
                if (adwIndexHistory[dwHistoryIndex] == dwVertexIndex) {
                    // re-use index dwReuseIndex
                    nvglSetStartMethod (0, NV_DD_KELVIN, NV097_INLINE_VERTEX_REUSE, 1);
                    nvglSetData (1, dwReuseIndex);
                    getDC()->nvPusher.inc (2);
                    goto next_i;
                }
                dwHistoryIndex--;
                dwHistoryIndex &= INDEX_HISTORY_MASK;
            }

            // this is a new index. emit the vertex and update cached indices
            nvglSetStartMethod (0, NV_DD_KELVIN, NVPUSHER_NOINC(NV097_INLINE_ARRAY), dwInlineVertexStride);
            getDC()->nvPusher.inc (sizeSetStartMethod);
            nvKelvinDumpInlineVertex (pContext, dwVertexIndex);
            getDC()->nvPusher.inc (dwInlineVertexStride);

            dwCurrentHistoryIndex++;
            dwCurrentHistoryIndex &= INDEX_HISTORY_MASK;
            adwIndexHistory[dwCurrentHistoryIndex] = dwVertexIndex;

            next_i:
            ;  // null statement to make label happy

        }

    }
}

#endif  // new way

#endif  // VERTEX_REUSE

//---------------------------------------------------------------------------

void nvKelvinDispatchNonIndexedPrimitiveInline
(
    PNVD3DCONTEXT       pContext,
    DWORD               dwCount
)
{
    DWORD numVertices, emittedVertices, dwScaleAndBias, dwInlineVertexStride, dwKelvinMaxInlineVertices;

    dwInlineVertexStride      = pContext->hwState.dwInlineVertexStride >> 2;
    if (dwInlineVertexStride == 1) dwInlineVertexStride = 2; //workaround for kelvin bug
    dwKelvinMaxInlineVertices = NVPUSHER_MAX_METHOD_COUNT / dwInlineVertexStride;

    nvAssert (kelvinBeginEndOp[pContext->dp2.dwDP2Prim] != ~0);

    nvglSetStartMethod (0, NV_DD_KELVIN, NV097_SET_BEGIN_END, 1);
    nvglSetData (1, kelvinBeginEndOp[pContext->dp2.dwDP2Prim]);
    nvglAdjust (2);

    dwScaleAndBias = kelvinPrimitiveToVertexCountScaleAndBias[pContext->dp2.dwDP2Prim];
    nvAssert (dwScaleAndBias != ~0);
    numVertices = (dwScaleAndBias >> 16) * dwCount + (dwScaleAndBias & 0xffff);

    emittedVertices = 0;

    while (numVertices > dwKelvinMaxInlineVertices) {
        // attempt to kick off some work already
        nvPusherStart (FALSE);
        // send more vertices
        nvKelvinDumpNonIndexedInlineVertexBlock (pContext, emittedVertices, dwKelvinMaxInlineVertices, dwInlineVertexStride);
        emittedVertices += dwKelvinMaxInlineVertices;
        numVertices -= dwKelvinMaxInlineVertices;
    }

    nvKelvinDumpNonIndexedInlineVertexBlock (pContext, emittedVertices, numVertices, dwInlineVertexStride);

    nvglSetStartMethod (0, NV_DD_KELVIN, NV097_SET_BEGIN_END, 1);
    nvglSetData (1, NV097_SET_BEGIN_END_OP_END);
    nvglAdjust (2);

    nvPusherStart (FALSE);
}

//---------------------------------------------------------------------------

void nvKelvinDispatchIndexedPrimitiveInline
(
    PNVD3DCONTEXT       pContext,
    DWORD               dwCount,
    BOOL                bLegacyStrides
)
{
    DWORD numVertices, emittedVertices, dwScaleAndBias, dwInlineVertexStride, dwKelvinMaxInlineVertices;

    dwInlineVertexStride      = pContext->hwState.dwInlineVertexStride >> 2;
    if (dwInlineVertexStride == 1) dwInlineVertexStride = 2; // workaround for kelvin bug
    dwKelvinMaxInlineVertices = NVPUSHER_MAX_METHOD_COUNT / dwInlineVertexStride;

    // tell the hardware what kind of thing we're going to draw
    nvAssert (kelvinBeginEndOp[pContext->dp2.dwDP2Prim] != ~0);

    nvglSetStartMethod (0, NV_DD_KELVIN, NV097_SET_BEGIN_END, 1);
    nvglSetData (1, kelvinBeginEndOp[pContext->dp2.dwDP2Prim]);
    nvglAdjust (2);

    // determine the number of vertices
    dwScaleAndBias = kelvinPrimitiveToVertexCountScaleAndBias[pContext->dp2.dwDP2Prim];
    nvAssert (dwScaleAndBias != ~0);
    numVertices = (dwScaleAndBias >> 16) * dwCount + (dwScaleAndBias & 0xffff);

    emittedVertices = 0;

    while (numVertices > dwKelvinMaxInlineVertices) {
        // attempt to kick off some work already
        nvPusherStart (FALSE);
        // send more vertices
        nvKelvinDumpIndexedInlineVertexBlock (pContext, emittedVertices, dwKelvinMaxInlineVertices,
                                              bLegacyStrides, dwInlineVertexStride);
        emittedVertices += dwKelvinMaxInlineVertices;
        numVertices     -= dwKelvinMaxInlineVertices;
    }

    // dump the remainder
    nvKelvinDumpIndexedInlineVertexBlock (pContext, emittedVertices, numVertices,
                                          bLegacyStrides, dwInlineVertexStride);

    nvglSetStartMethod (0, NV_DD_KELVIN, NV097_SET_BEGIN_END, 1);
    nvglSetData (1, NV097_SET_BEGIN_END_OP_END);
    nvglAdjust (2);

    // attempt to kick off some work already
    nvPusherStart (FALSE);
}

#endif  // NVARCH > 0x020

