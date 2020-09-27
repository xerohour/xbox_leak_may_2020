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
//  Module: nvKelvinInlPrim.cpp
//      compiled inner loops for Kelvin inline primitives
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        10Aug2000         port from celsius
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x20)

#include "x86.h"

#ifdef KELVIN_ILC
//////////////////////////////////////////////////////////////////////////////
// notes:
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
void nvKelvinILCompile_copyVertex              (PNVD3DCONTEXT pContext,DWORD dwFlags);
void nvKelvinILCompile_prefetch                (BOOL bIsIndexed,BOOL bLegacyStrides);
void nvKelvinILCompile_limit                   (DWORD regLimit,DWORD regDummy);

void __stdcall nvKelvinDispatchGetPusherSpace (void);

//---------------------------------------------------------------------------
DWORD nvKelvinILCompile_inline_prim
(
    PNVD3DCONTEXT pContext,
    DWORD         dwFlags
)
{
     //
    // get loop constants
    //
    DWORD  dwMagic                = kelvinPrimitiveToPrimitiveMagic[dwFlags & KELVIN_ILMASK_PRIMTYPE];
    DWORD  dwVerticesPerPrim      = (dwMagic >> 8) & 0xff;
    DWORD  dwStartVerticesPerPrim = (dwMagic >> 0) & 0xff;
    BOOL   bLegacyStrides         = dwFlags & KELVIN_ILFLAG_LEGACY;
    BOOL   bIsIndexed             = (dwFlags & KELVIN_ILMASK_LOOPTYPE) == KELVIN_ILFLAG_IX_INL_PRIM;

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
    xMOV_r_i32  (rEDX,mMEM32(global.kelvin.x))    \
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

    //  - pContext->hwState.dwStateFlags & KELVIN_MASK_INNERLOOPUNIQUENESS
    xMOV_r_i32  (rEDX,mMEM32(global.kelvin.pContext))
    xMOV_rm_imm (rmREG(rEBX),0x00040000)
    xMOV_r_rm   (rEAX,rmIND32(rEDX)) xOFS32(OFFSETOF(NVD3DCONTEXT, hwState.dwStateFlags))
    xAND_rm_imm (rmREG(rEAX),KELVIN_MASK_INNERLOOPUNIQUENESS)
    xCMP_rm_imm (rmREG(rEAX),pContext->hwState.dwStateFlags & KELVIN_MASK_INNERLOOPUNIQUENESS)
    xJNZ32      (ld2)

    if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(0))
    {
        CHECK2 (0x00050000,pContext,hwState.dwTexUnitToTexStageMapping[0],NVD3DCONTEXT)
    }
    if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(1))
    {
        CHECK2 (0x00050001,pContext,hwState.dwTexUnitToTexStageMapping[1],NVD3DCONTEXT)
    }
#endif

#ifdef KELVIN_INLINE_REUSE
    if (bIsIndexed)
    {
        // clear the reuse history buffer
        xMOV_rm_imm     (rmREG(rECX), KELVIN_MAX_REUSE)
        xMOV_rm_imm     (rmREG(rEDI), mMEM32(global.kelvin.dwVertexReuse))
        DWORD lClearReuse;
        xLABEL          (lClearReuse);
        xMOV_rm_imm     (rmIND(rEDI), 0xffffffff);
        xADD_rm_imm8    (rmREG(rEDI), 0x4);
        xDEC_r          (rECX)
        xJNZ            (lClearReuse)

        xXOR_r_rm       (rEAX, rmREG(rEAX))
        xMOV_i32_r      (mMEM32(global.kelvin.dwCurrentHistoryIndex), rEAX)


    }

#endif KELVIN_INLINE_REUSE

//     if (pContext->hwState.pVertexShader->bVAExists(CVertexShader::PSIZE_ARRAY)) // do the point size
//     {
// #if 0 //USE_C_LOGIC
//         xMOV_rm_imm (rmREG(rEAX),dwFlags)
//             xPUSH_r (rEAX)
//             xMOV_rm_imm (rmREG(rEAX),nvCelsiusDumpTLPoints)
//             xCALL_rm (rmREG(rEAX))
//             xADD_rm_imm (rmREG(rESP),4);
// #else //USE_C_LOGIC
//         nvAssert(0);
// #endif
//     }
//     else
    {
    //
    // setup push buffer
    //
    xMOV_r_i32  (rEDX,mMEM32(pDriverData))
    xMOV_r_rm   (rEDI,rmIND32(rEDX)) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))

    //
    // calc how many verts to copy into EBX
    //
    xMOV_r_i32  (rEBX,mMEM32(global.kelvin.dwPrimCount))
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
        xMOV_r_i32  (rEAX,mMEM32(global.kelvin.pIndices))
         xLEA_r_rm   (rECX,rmSIB) xSIB(rEAX,rEBX,x2)
        xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchBase),rEAX)
         xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchMax),rECX)
        if (bLegacyStrides)
        {
            xMOV_r_i32  (rEAX,mMEM32(global.kelvin.pIndices))
            xMOV_i32_r  (mMEM32(global.kelvin.pIndexBase),rEAX)
        }
    }
    else
    {
        xMOV_r_i32  (rEAX,mMEM32(global.kelvin.pVertices))
         xMOV_r_rm   (rECX,rmREG(rEBX))
         ILCCompile_mul (rECX,global.kelvin.dwVertexStride);
         xADD_r_rm   (rECX,rmREG(rEAX))
        xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchBase),rEAX)
         xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchMax),rECX)
    }

    //
    // start primitive
    //
    xMOV_rm_imm (rmREG(rEAX),((sizeSetNv20KelvinBeginEndMthdCnt << 2) << 16) | ((NV_DD_KELVIN) << 13) | NV097_SET_BEGIN_END);
    xMOV_rm_imm (rmREG(rECX),kelvinBeginEndOp[dwFlags & KELVIN_ILMASK_PRIMTYPE])
    xMOV_rm_r   (rmIND(rEDI),rEAX)
    xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(4)
    xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)

#ifdef KELVIN_INLINE_REUSE
    DWORD           nextVertex;
#endif

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
        nvKelvinILCompile_prefetch (bIsIndexed, bLegacyStrides);

        //
        // copy vertex
        //
        if (bIsIndexed)
        {
            // get index
            xMOV_r_i32  (rEDX,mMEM32(global.kelvin.pIndices))
            xXOR_r_rm   (rESI,rmREG(rESI))
       x16r xMOV_r_rm   (rSI,rmIND(rEDX))
            xLEA_r_rm   (rEDX,rmIND8(rEDX)) xOFS8(2)
            if (bLegacyStrides)
            {
                xMOV_r_rm   (rEAX,rmREG(rEDX))
                xSUB_r_i32  (rEAX,mMEM32(global.kelvin.pIndexBase))
                xAND_rm_imm (rmREG(rEAX),6)
                xXOR_r_rm   (rECX,rmREG(rECX))
                xCMP_rm_imm (rmREG(rEAX),6)
                xSETNZ_rm8  (rmREG(rCL))
                xDEC_rm     (rmREG(rECX))
                xAND_rm_imm (rmREG(rECX),2)
                xADD_r_rm   (rEDX,rmREG(rECX))
            }
            xMOV_i32_r  (mMEM32(global.kelvin.pIndices),rEDX)

#ifdef KELVIN_INLINE_REUSE
            // ESI contains the vertex

            xMOV_r_i32      (rEAX, mMEM32(global.kelvin.dwCurrentHistoryIndex))
            xMOV_rm_imm     (rmREG(rECX), KELVIN_MAX_REUSE)
            xXOR_r_rm       (rEDX, rmREG(rEDX))

            DWORD ReuseLoop;
            xLABEL          (ReuseLoop)


            xCMP_r_rm       (rESI, rmSIB) xSIBID(rEAX,x4) xOFS32(mMEM32(global.kelvin.dwVertexReuse[0]))
            DWORD notFound;
            xLABEL          (notFound)
            xJNZ            (0)

            // output reused index
            xMOV_rm_imm     (rmREG(rEBX),( ( ((1) << 2) << 16) | ((NV_DD_KELVIN) << 13) | (NV097_INLINE_VERTEX_REUSE)))
            xMOV_rm_r       (rmIND(rEDI),rEBX)
            xMOV_rm_r       (rmIND8(rEDI),rEDX) xOFS8(4)
            xLEA_r_rm       (rEDI, rmIND8(rEDI)) xOFS8(8)

            xLABEL          (nextVertex)
            xJMP            (0)

            xTARGET_b8      (notFound)
            xINC_r          (rEDX)

            xDEC_r          (rEAX)
            xAND_rm_imm8    (rmREG(rEAX), KELVIN_REUSE_MASK)

            xDEC_r          (rECX)
            xJNZ            (ReuseLoop)

            // New index -- output it and update cached indices

            xMOV_r_i32      (rEAX, mMEM32(global.kelvin.dwCurrentHistoryIndex))
            xINC_r          (rEAX)
            xAND_rm_imm8    (rmREG(rEAX), KELVIN_REUSE_MASK)
            xMOV_i32_r      (mMEM32(global.kelvin.dwCurrentHistoryIndex), rEAX)
            xMOV_rm_r       (rmSIB, rESI) xSIBID(rEAX,x4) xOFS32(mMEM32(global.kelvin.dwVertexReuse[0]))


#endif // KELVIN_INLINE_REUSE

            // compute vertex offset
            ILCCompile_mul (rESI,global.kelvin.dwVertexStride);
            // range limit for NT
            nvKelvinILCompile_limit (rESI,rECX);

            // compute vertex address
            xADD_r_i32  (rESI,mMEM32(global.kelvin.pVertices))
        }
        else
        {
            // get vertex & increment
            xMOV_r_i32  (rESI,mMEM32(global.kelvin.pVertices))
            xLEA_r_rm   (rEAX,rmIND32(rESI)) xOFS32(global.kelvin.dwVertexStride)
            xMOV_i32_r  (mMEM32(global.kelvin.pVertices),rEAX)
        }

        // copy vertex (from ESI to EDI)
        nvKelvinILCompile_copyVertex (pContext,dwFlags);

        // update instumentation stats
#ifdef INSTRUMENT_INNER_LOOPS
        {
            xMOV_rm_imm (rmREG(rEAX),fvfData.dwVertexStride)
            xADD_i32_r  (mMEM32(global.dwBytesCopied),rEAX)
        }
#endif

#ifdef KELVIN_INLINE_REUSE
        if (bIsIndexed)
        {
            xTARGET_jmp     (nextVertex)
        }
#endif//KELVIN_INLINE_REUSE

        // check for pusher space
        DWORD labelSpace;
        xMOV_r_i32  (rESI,mMEM32(pDriverData))
        xCMP_r_rm   (rEDI,rmIND32(rESI)) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwThreshold))
        xLABEL      (labelSpace)
        xJL         (0)
        {
            xMOV_rm_r   (rmIND32(rESI),rEDI) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))
            xMOV_rm_imm (rmREG(rEAX),(DWORD)nvKelvinDispatchGetPusherSpace)
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
    xMOV_rm_imm (rmREG(rEAX),((sizeSetNv20KelvinBeginEndMthdCnt << 2) << 16) | ((NV_DD_KELVIN) << 13) | NV097_SET_BEGIN_END);
    xMOV_rm_imm (rmREG(rECX),NV097_SET_BEGIN_END_OP_END)
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

#endif // KELVIN_ILC

#endif  // NVARCH == 0x20

