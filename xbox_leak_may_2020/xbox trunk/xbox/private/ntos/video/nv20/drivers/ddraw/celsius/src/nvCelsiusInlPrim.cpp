/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvCelsiusInlPrim.cpp                                              *
*       indexed and ordered inline primitive                                *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal             20Oct99         NV10 optimization effort    *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

#include "x86.h"

//////////////////////////////////////////////////////////////////////////////
// notes:
//  - when offset changes, only program it, dont call setcelsiusstate
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// switches
//

//////////////////////////////////////////////////////////////////////////////
// aliases
//
#define ilcData         global.dwILCData
#define ilcCount        global.dwILCCount
#define ilcMax          global.dwILCMax

//////////////////////////////////////////////////////////////////////////////
// externals
//
void ILCCompile_mul                            (DWORD reg,DWORD num);
//void nvCelsiusILCompile_beginEnd               (DWORD dwPrimType);
//void nvCelsiusILCompile_computeIndexAndOffset  (DWORD dwVBStride,DWORD dwVBLogStride);
//void nvCelsiusILCompile_memcpy                 (DWORD dwCount, bool recurse);
void nvCelsiusILCompile_copyVertex             (PNVD3DCONTEXT pContext,DWORD dwFlags);
void nvCelsiusILCompile_prefetch               (BOOL bIsIndexed,BOOL bLegacyStrides);
void nvCelsiusILCompile_limit                  (DWORD regLimit,DWORD regDummy);

//void __stdcall nvCelsiusDispatchGetFreeSpace   (PNVD3DCONTEXT pContext);
void __stdcall nvCelsiusDispatchGetPusherSpace (void);
#ifdef DEBUG
//void __stdcall nvCelsiusDispatchFlush          (void);
//void __stdcall nvCelsiusDispatchPrintVertex    (PNVD3DCONTEXT pContext,DWORD dwPutAddress);
#endif

//---------------------------------------------------------------------------

// transformed points with per-vertex pointsize

// this handles the specific case of SW-transformed vertices that include a per-vertex
// point size. we are guaranteed that it is a single-stream primitive from stream zero.
// the vertex format will be a DX6-style TL vertex plus a point size.

// the HW may or may not have been programmed to expect one set of texture coordinates,
// continent on whether this is a textured point sprite or just a plain point. note that
// if we do send texture coordinates, we generate them upside down relative to D3D so
// that we match the manner in which celsius itself generates them in the HW T&L case.

void __cdecl nvCelsiusDumpTLPoints (DWORD dwFlags)
{
    DWORD          dwDiffuse,dwSpecular,dwU,dwV;
    float          fx,fy,fz,fw,fpsize;
    float          fx1,fx2,fy1,fy2;
    BOOL           bIsIndexed, bTexCoords;
    WORD          *pIndices   = (WORD *)(global.celsius.pIndices);
    PBYTE          pVertices  = (PBYTE)(global.celsius.pVertices);
    PBYTE          pVertex;
    PNVD3DCONTEXT  pContext   = (PNVD3DCONTEXT)global.celsius.pContext;
    CVertexShader *pVertexShader = pContext->pCurrentVShader;

    nvAssert ((pContext->dp2.dwDP2Prim == D3DDP2OP_POINTS) ||
              (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE] == D3DFILL_POINT));
    nvAssert (pContext->pCurrentVShader->getStride() == 0x24);

    bIsIndexed = ((dwFlags & CELSIUS_ILMASK_LOOPTYPE) == CELSIUS_ILFLAG_IX_INL_PRIM)    ? TRUE : FALSE;
    bTexCoords = (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(0)) ? TRUE : FALSE;

    // cache current values, then set solid fill, solid shading, and disable culling
    DWORD dwOldFillMode  = pContext->dwRenderState[D3DRENDERSTATE_FILLMODE];
    DWORD dwOldShadeMode = pContext->dwRenderState[D3DRENDERSTATE_SHADEMODE];
    DWORD dwOldCullMode  = pContext->dwRenderState[D3DRENDERSTATE_CULLMODE];

    pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]  = D3DFILL_SOLID;
    pContext->dwRenderState[D3DRENDERSTATE_SHADEMODE] = D3DSHADE_FLAT;
    pContext->dwRenderState[D3DRENDERSTATE_CULLMODE]  = D3DCULL_NONE;

    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_MISC_STATE;
    nvSetCelsiusState (pContext);

    // begin primitive
    nvPushData (0, ((sizeSetNv10CelsiusBeginEnd3MthdCnt << 2) << 16) | (NV_DD_CELSIUS << 13) | NV056_SET_BEGIN_END4);
    nvPushData (1, NV056_SET_BEGIN_END_OP_TRIANGLES);
    nvPusherAdjust (2);

    for (DWORD i=0; i<global.celsius.dwPrimCount; i++)
    {
        pVertex = pVertices + ((bIsIndexed ? pIndices[i] : i) * pVertexShader->getStride());

        fpsize  = *(float*)(pVertex + pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_PSIZE]));
        fpsize *= 0.5f;
        fpsize  = min (fpsize, 32.0f);
        fx1     = *(float*)(pVertex + 0) - fpsize;
        fx2     = *(float*)(pVertex + 0) + fpsize;
        fy1     = *(float*)(pVertex + 4) - fpsize;
        fy2     = *(float*)(pVertex + 4) + fpsize;
        fz      = *(float*)(pVertex + 8);
        fw      = *(float*)(pVertex + 12);
        dwDiffuse  = *(DWORD *)(pVertex + pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_DIFFUSE]));
        dwSpecular = *(DWORD *)(pVertex + pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_SPECULAR]));

        if (bTexCoords) {
            for (DWORD j=0; j<6; j++) {
                switch (j) {
                    case 0:        fx=fx1; fy=fy1; dwU=FP_ONE_BITS; dwV=FP_ONE_BITS; break;
                    case 5:        fx=fx2; fy=fy2; dwU=0;           dwV=0;           break;
                    case 2:case 3: fx=fx1; fy=fy2; dwU=FP_ONE_BITS; dwV=0;           break;
                    case 1:case 4: fx=fx2; fy=fy1; dwU=0;           dwV=FP_ONE_BITS; break;
                }
                getDC()->nvPusher.push (0, (8 << 18) | (NV_DD_CELSIUS << 13) | NVPUSHER_NOINC(NV056_INLINE_ARRAY(0)));
                getDC()->nvPusher.push (1, dwU);
                getDC()->nvPusher.push (2, dwV);
                getDC()->nvPusher.push (3, dwSpecular);
                getDC()->nvPusher.push (4, dwDiffuse);
                getDC()->nvPusher.push (5, DWORD_FROM_FLOAT(fx));
                getDC()->nvPusher.push (6, DWORD_FROM_FLOAT(fy));
                getDC()->nvPusher.push (7, DWORD_FROM_FLOAT(fz));
                getDC()->nvPusher.push (8, DWORD_FROM_FLOAT(fw));
                nvPusherAdjust (9);
            }
        }
        else {
            for (DWORD j=0; j<6; j++) {
                switch (j) {
                    case 0:        fx=fx1; fy=fy1; break;
                    case 5:        fx=fx2; fy=fy2; break;
                    case 2:case 3: fx=fx1; fy=fy2; break;
                    case 1:case 4: fx=fx2; fy=fy1; break;
                }
                getDC()->nvPusher.push (0, (6 << 18) | (NV_DD_CELSIUS << 13) | NVPUSHER_NOINC(NV056_INLINE_ARRAY(0)));
                getDC()->nvPusher.push (1, dwSpecular);
                getDC()->nvPusher.push (2, dwDiffuse);
                getDC()->nvPusher.push (3, DWORD_FROM_FLOAT(fx));
                getDC()->nvPusher.push (4, DWORD_FROM_FLOAT(fy));
                getDC()->nvPusher.push (5, DWORD_FROM_FLOAT(fz));
                getDC()->nvPusher.push (6, DWORD_FROM_FLOAT(fw));
                nvPusherAdjust (7);
            }
        }
    }

    // end primitive
    nvPushData (0,((1 << 2) << 16) | ((NV_DD_CELSIUS) << 13) | NV056_SET_BEGIN_END4);
    nvPushData (1, NV056_SET_BEGIN_END_OP_END);
    nvPusherAdjust(2);

    // restore original fill, shade, and cull values
    pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]  = dwOldFillMode;
    pContext->dwRenderState[D3DRENDERSTATE_SHADEMODE] = dwOldShadeMode;
    pContext->dwRenderState[D3DRENDERSTATE_CULLMODE]  = dwOldCullMode;

    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_MISC_STATE;
}

//---------------------------------------------------------------------------

DWORD nvCelsiusILCompile_inline_prim
(
    PNVD3DCONTEXT pContext,
    DWORD         dwFlags
)
{
    //
    // get loop constants
    //
    DWORD  dwMagic                = celsiusPrimitiveToPrimitiveMagic[dwFlags & CELSIUS_ILMASK_PRIMTYPE];
    DWORD  dwVerticesPerPrim      = (dwMagic >> 8) & 0xff;
    DWORD  dwStartVerticesPerPrim = (dwMagic >> 0) & 0xff;
    BOOL   bLegacyStrides         = dwFlags & CELSIUS_ILFLAG_LEGACY;
    BOOL   bIsIndexed             = (dwFlags & CELSIUS_ILMASK_LOOPTYPE) == CELSIUS_ILFLAG_IX_INL_PRIM;

    //
    // align entry point
    //
    while (ilcCount & 31) { xINT3 }
    DWORD lEntry;
    xLABEL (lEntry);

    //
    // setup stack frame
    //
    xPUSH_r     (rEBP)
    xPUSH_r     (rEBX)
    xPUSH_r     (rESI)
    xPUSH_r     (rEDI)

    //
    // verify that we are using the correct inner loop - debug only
    //
#ifdef DEBUG
    DWORD ld1,ld2;
    xLABEL      (ld1)
    xJMP        (0)
    xLABEL      (ld2)
    xINT3

#define CHECK(a,x)                  \
{                                   \
    xMOV_rm_imm (rmREG(rEBX),a)     \
    xMOV_rm_imm (rmREG(rEAX),x)     \
    xCMP_r_i32  (rEAX,mMEM32(x))    \
    xJNZ32      (ld2)               \
}
#define CHECK2(a,x,y,t)             \
{                                   \
    xMOV_r_i32  (rEDX,mMEM32(global.celsius.x))    \
    xMOV_rm_imm (rmREG(rEBX),a)     \
    xMOV_rm_imm (rmREG(rEAX),x->y)   \
    xCMP_r_rm   (rEAX,rmIND32(rEDX)) xOFS32(OFFSETOF(t,y)) \
    xJNZ32      (ld2)               \
}

    xTARGET_jmp (ld1)

/*
    CHECK (0x00000000,fvfData.dwVertexStride)

    CHECK (0x00010000,fvfData.dwVertexType)

    CHECK (0x00020000,fvfData.dwUVCount[0])
    CHECK (0x00020001,fvfData.dwUVCount[1])
    CHECK (0x00020002,fvfData.dwUVCount[2])
    CHECK (0x00020003,fvfData.dwUVCount[3])
    CHECK (0x00020004,fvfData.dwUVCount[4])
    CHECK (0x00020005,fvfData.dwUVCount[5])
    CHECK (0x00020006,fvfData.dwUVCount[6])
    CHECK (0x00020007,fvfData.dwUVCount[7])

    CHECK (0x00030000,fvfData.dwUVOffset[0])
    CHECK (0x00030001,fvfData.dwUVOffset[1])
    CHECK (0x00030002,fvfData.dwUVOffset[2])
    CHECK (0x00030003,fvfData.dwUVOffset[3])
    CHECK (0x00030004,fvfData.dwUVOffset[4])
    CHECK (0x00030005,fvfData.dwUVOffset[5])
    CHECK (0x00030006,fvfData.dwUVOffset[6])
    CHECK (0x00030007,fvfData.dwUVOffset[7])
*/

    //  - pContext->hwState.dwStateFlags & CELSIUS_MASK_INNERLOOPUNIQUENESS
    xMOV_r_i32  (rEDX,mMEM32(global.celsius.pContext))
    xMOV_rm_imm (rmREG(rEBX),0x00040000)
    xMOV_r_rm   (rEAX,rmIND32(rEDX)) xOFS32(OFFSETOF(NVD3DCONTEXT, hwState.dwStateFlags))
    xAND_rm_imm (rmREG(rEAX),CELSIUS_MASK_INNERLOOPUNIQUENESS)
    xCMP_rm_imm (rmREG(rEAX),pContext->hwState.dwStateFlags & CELSIUS_MASK_INNERLOOPUNIQUENESS)
    xJNZ32      (ld2)

    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(0))
    {
        CHECK2 (0x00050000,pContext,hwState.dwTexUnitToTexStageMapping[0],NVD3DCONTEXT)
    }
    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(1))
    {
        CHECK2 (0x00050001,pContext,hwState.dwTexUnitToTexStageMapping[1],NVD3DCONTEXT)
    }
#endif

    if ((pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_PSIZE]))
        &&
        ((pContext->dp2.dwDP2Prim == D3DDP2OP_POINTS) || (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE] == D3DFILL_POINT)))
    {
        // do the point size expansion
#if 1 //USE_C_LOGIC
        xMOV_rm_imm (rmREG(rEAX),dwFlags)
            xPUSH_r (rEAX)
            xMOV_rm_imm (rmREG(rEAX),nvCelsiusDumpTLPoints)
            xCALL_rm (rmREG(rEAX))
            xADD_rm_imm (rmREG(rESP),4);
#else //USE_C_LOGIC
        nvAssert(0);
#endif
    }
    else
    {
    //
    // setup push buffer
    //
    xMOV_r_i32  (rEDX,mMEM32(pDriverData))
    xMOV_r_rm   (rEDI,rmIND32(rEDX)) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))

    //
    // calc how many verts to copy into EBX
    //
    xMOV_r_i32  (rEBX,mMEM32(global.celsius.dwPrimCount))
    ILCCompile_mul (rEBX, dwVerticesPerPrim);
    if (dwStartVerticesPerPrim)
    {
        xADD_rm_imm (rmREG(rEBX),dwStartVerticesPerPrim)
    }

    //
    // setup prefetcher (EBX has # of vertices to copy in total)
    //
    if (bIsIndexed)
    {
        xMOV_r_i32  (rEAX,mMEM32(global.celsius.pIndices))
         xLEA_r_rm   (rECX,rmSIB) xSIB(rEAX,rEBX,x2)
        xMOV_i32_r  (mMEM32(global.celsius.dwPrefetchBase),rEAX)
         xMOV_i32_r  (mMEM32(global.celsius.dwPrefetchMax),rECX)
        if (bLegacyStrides)
        {
            xMOV_r_i32  (rEAX,mMEM32(global.celsius.pIndices))
            xMOV_i32_r  (mMEM32(global.celsius.pIndexBase),rEAX)
        }
    }
    else
    {
        xMOV_r_i32  (rEAX,mMEM32(global.celsius.pVertices))
         xMOV_r_rm   (rECX,rmREG(rEBX))
         ILCCompile_mul (rECX,global.celsius.dwVertexStride);
         xADD_r_rm   (rECX,rmREG(rEAX))
        xMOV_i32_r  (mMEM32(global.celsius.dwPrefetchBase),rEAX)
         xMOV_i32_r  (mMEM32(global.celsius.dwPrefetchMax),rECX)
    }

    //
    // start primitive
    //
    xMOV_rm_imm (rmREG(rEAX),((sizeSetNv10CelsiusBeginEnd4MthdCnt << 2) << 16) | ((NV_DD_CELSIUS) << 13) | NV056_SET_BEGIN_END4);
    xMOV_rm_imm (rmREG(rECX),celsiusBeginEndOp[dwFlags & CELSIUS_ILMASK_PRIMTYPE])
    xMOV_rm_r   (rmIND(rEDI),rEAX)
    xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(4)
    xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)

    //
    // start copying
    //   for this loop EDI points to PUT and EBX has the amount of prims to go
    //
    DWORD labelLoop;
    xLABEL  (labelLoop)
    {
        xPUSH_r     (rEBX)

        //
        // prefetch
        //
        nvCelsiusILCompile_prefetch (bIsIndexed, bLegacyStrides);

        //
        // copy vertex
        //
        if (bIsIndexed)
        {
            // get index
            xMOV_r_i32  (rEDX,mMEM32(global.celsius.pIndices))
            xXOR_r_rm   (rESI,rmREG(rESI))
       x16r xMOV_r_rm   (rSI,rmIND(rEDX))
            xLEA_r_rm   (rEDX,rmIND8(rEDX)) xOFS8(2)
            if (bLegacyStrides)
            {
                xMOV_r_rm   (rEAX,rmREG(rEDX))
                xSUB_r_i32  (rEAX,mMEM32(global.celsius.pIndexBase))
                xAND_rm_imm (rmREG(rEAX),6)
                xXOR_r_rm   (rECX,rmREG(rECX))
                xCMP_rm_imm (rmREG(rEAX),6)
                xSETNZ_rm8  (rmREG(rCL))
                xDEC_rm     (rmREG(rECX))
                xAND_rm_imm (rmREG(rECX),2)
                xADD_r_rm   (rEDX,rmREG(rECX))
            }
            xMOV_i32_r  (mMEM32(global.celsius.pIndices),rEDX)
            // compute vertex offset
            ILCCompile_mul (rESI,global.celsius.dwVertexStride);
            // range limit for NT
            nvCelsiusILCompile_limit (rESI,rECX);

            // compute vertex address
            xADD_r_i32  (rESI,mMEM32(global.celsius.pVertices))
        }
        else
        {
            // get vertex & increment
            xMOV_r_i32  (rESI,mMEM32(global.celsius.pVertices))
            xLEA_r_rm   (rEAX,rmIND32(rESI)) xOFS32(global.celsius.dwVertexStride)
            xMOV_i32_r  (mMEM32(global.celsius.pVertices),rEAX)
        }

        // copy vertex (from ESI to EDI)
        nvCelsiusILCompile_copyVertex (pContext,dwFlags);

        // update instumentation stats
#ifdef INSTRUMENT_INNER_LOOPS
        {
            xMOV_rm_imm (rmREG(rEAX),fvfData.dwVertexStride)
            xADD_i32_r  (mMEM32(global.dwBytesCopied),rEAX)
        }
#endif


        // check for pusher space
        DWORD labelSpace;
        xMOV_r_i32  (rESI,mMEM32(pDriverData))
        xCMP_r_rm   (rEDI,rmIND32(rESI)) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwThreshold))
        xLABEL      (labelSpace)
        xJL         (0)
        {
            xMOV_rm_r   (rmIND32(rESI),rEDI) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))
            xMOV_rm_imm (rmREG(rEAX),(DWORD)nvCelsiusDispatchGetPusherSpace)
            xCALL_rm    (rmREG(rEAX))
            xMOV_r_rm   (rEDI,rmIND32(rESI)) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))
        }
        xTARGET_b8  (labelSpace)

        // next
        xPOP_r  (rEBX)
        xDEC_rm (rmREG(rEBX))
    }
    xJNZ32  (labelLoop)

    //
    // end primitive
    //
    xMOV_rm_imm (rmREG(rEAX),((sizeSetNv10CelsiusBeginEnd4MthdCnt << 2) << 16) | ((NV_DD_CELSIUS) << 13) | NV056_SET_BEGIN_END4);
    xMOV_rm_imm (rmREG(rECX),NV056_SET_BEGIN_END4_OP_END)
    xMOV_rm_r   (rmIND(rEDI),rEAX)
    xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(4)
    xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)

    //
    // save off push buffer
    //
    xMOV_r_i32  (rEDX,mMEM32(pDriverData))
#ifndef NV_NULL_HW_DRIVER
    xMOV_rm_r   (rmIND32(rEDX),rEDI) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))
#endif
    }
    //
    // done
    //
    xPOP_r      (rEDI)
    xPOP_r      (rESI)
    xPOP_r      (rEBX)
    xPOP_r      (rEBP)
    xRET

    return lEntry;
}

#endif  // NVARCH == 0x10

