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
//  Module: nvKelvinVbPrim.cpp
//       indexed and ordered vb primitives
//
//***************************************************************************
//
//  History:
//       Craig Duttweiler         10Aug2000         ported from celsius
//
//***************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x20)

#include "x86.h"

#ifdef KELVIN_ILC

//////////////////////////////////////////////////////////////////////////////
// aliases
//
#define ilcData         global.dwILCData
#define ilcCount        global.dwILCCount
#define ilcMax          global.dwILCMax

//////////////////////////////////////////////////////////////////////////////
// externals
//
void ILCCompile_mul                      (DWORD reg,DWORD num);
void nvKelvinILCompile_beginEnd              (DWORD dwPrimType);
void nvKelvinILCompile_computeIndexAndOffset (DWORD dwVBStride,DWORD dwVBLogStride);
void nvKelvinILCompile_memcpy                 (DWORD dwCount, bool recurse);
void nvKelvinILCompile_indexcpy               (DWORD dwCount);
void nvKelvinILCompile_copyVertex            (PNVD3DCONTEXT pContext,DWORD dwFlags);
void nvKelvinILCompile_prefetch              (BOOL bIsIndexed,BOOL bLegacyStrides);
void nvKelvinILCompile_limit                 (DWORD regLimit,DWORD regDummy);

void __stdcall nvKelvinDispatchGetFreeSpace   (PNVD3DCONTEXT pContext);
void __stdcall nvKelvinDispatchGetPusherSpace (void);
#ifdef DEBUG
void __stdcall nvKelvinDispatchFlush          (void);
void __stdcall nvKelvinDispatchPrintVertex    (PNVD3DCONTEXT pContext,DWORD dwPutAddress);
#endif

//---------------------------------------------------------------------------

// vb primitive

DWORD nvKelvinILCompile_vb_prim
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
    BOOL   bIsIndexed             = (dwFlags & KELVIN_ILMASK_LOOPTYPE) == KELVIN_ILFLAG_IX_VB_PRIM;

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

    //
    // setup push buffer
    //
    xMOV_r_i32  (rEDX,mMEM32(pDriverData))
    xMOV_r_rm   (rEDI,rmIND32(rEDX)) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))

    //
    // start prim
    //
    xMOV_rm_imm (rmREG(rEAX),((sizeSetNv20KelvinBeginEndMthdCnt << 2) << 16) | ((NV_DD_KELVIN) << 13) | NV097_SET_BEGIN_END);
    xMOV_rm_imm (rmREG(rECX),kelvinBeginEndOp[dwFlags & KELVIN_ILMASK_PRIMTYPE])
    xMOV_rm_r   (rmIND(rEDI),rEAX)
    xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(4)
    xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)

    //
    // do work
    //
    if (bIsIndexed)
    {
        //
        // indexed VB
        //

        //
        // setup prefetcher
        //
        xMOV_r_i32  (rEBX,mMEM32(global.kelvin.dwPrimCount))
        ILCCompile_mul (rEBX,bLegacyStrides ? 4 : dwVerticesPerPrim);
        if (dwStartVerticesPerPrim)
        {
            xADD_rm_imm (rmREG(rEBX),dwStartVerticesPerPrim)
        }
        xLEA_r_rm   (rECX,rmSIB) xSIB(rEBX,rEBX,x1)

        xMOV_r_i32  (rEAX,mMEM32(global.kelvin.pIndices))
        xADD_r_rm   (rECX,rmREG(rEAX))
        xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchBase),rEAX)
        xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchMax),rECX)

        //
        // send indices
        //
        xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
        DWORD labelNext;
        xLABEL      (labelNext)

        //
        // prefetch
        //
        DWORD labelPrefetch1;
        DWORD labelPrefetch2;
        DWORD labelPrefetch3;
        xMOV_r_i32  (rEDX,mMEM32(global.kelvin.dwPrefetchBase))
        xCMP_r_rm   (rEDX,rmREG(rESI))
        xLABEL      (labelPrefetch1)
        xJA32       (0)

        xMOV_rm_imm (rmREG(rECX),2048 / 32)
        xLABEL      (labelPrefetch2)
        {
            xCMP_r_i32  (rEDX,mMEM32(global.kelvin.dwPrefetchMax))
            xLABEL      (labelPrefetch3)
            xJAE32      (0)
            xMOV_r_rm   (rEAX,rmIND(rEDX))
            xDEC_rm     (rmREG(rECX))
            xLEA_r_rm   (rEDX,rmIND8(rEDX)) xOFS8(32)
            xJNZ32      (labelPrefetch2)
        }
        xTARGET_b32 (labelPrefetch3)
        xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchBase),rEDX)
        xTARGET_b32 (labelPrefetch1)

        //
        // copy indices
        //
        if (bLegacyStrides)
        {
            //
            // legacy - pretty simple since it MUST be a tri list
            //          source = index0:16,index1:16,index2:16,flags:16
            //
            xMOV_r_rm   (rEAX,rmIND(rESI))
            xMOV_r_rm   (rECX,rmIND8(rESI)) xOFS8(4)
            xLEA_r_rm   (rESI,rmIND8(rESI)) xOFS8(8)
            xMOV_r_rm   (rEBP,rmREG(rEAX))
            xAND_rm_imm (rmREG(rEAX),0xffff)
            xADD_r_i32  (rEAX, mMEM32(global.kelvin.dwBaseVertex) ) //add base index
            xMOV_rm_imm (rmREG(rEDX),((((3) << 2) << 16) | ((NV_DD_KELVIN) << 13) | (NVPUSHER_NOINC(NV097_ARRAY_ELEMENT32))))
            xSHR_rm_imm8(rmREG(rEBP),16)
            xADD_r_i32  (rEBP, mMEM32(global.kelvin.dwBaseVertex) ) //add base index
            xMOV_rm_r   (rmIND(rEDI),rEDX)
            xMOV_rm_r   (rmIND8(rEDI),rEAX) xOFS8(4)
            xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(16)
            xAND_rm_imm (rmREG(rECX),0xffff)
            xADD_r_i32  (rECX, mMEM32(global.kelvin.dwBaseVertex) ) //add base index
            xMOV_rm_r   (rmIND8(rEDI),rEBP) xOFS8(8 - 16)
            xSUB_rm_imm (rmREG(rEBX),4)
            xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(12 - 16)
            // next tri
            xJNZ32      (labelNext)
        }
        else
        {
            //
            // do a 64 index batch
            //
            DWORD labelCopy0;
            xCMP_rm_imm (rmREG(rEBX),64)
            xLABEL      (labelCopy0)
            xJB32       (0)
            {
                xMOV_rm_imm (rmREG(rEAX),((((32) << 2) << 16) | ((NV_DD_KELVIN) << 13) | (NVPUSHER_NOINC(NV097_ARRAY_ELEMENT16))))
                xMOV_rm_r   (rmIND(rEDI),rEAX)
                xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(4)

                nvKelvinILCompile_indexcpy (128);

                // check for pusher space
                DWORD labelSpace;
                xMOV_r_i32  (rEDX,mMEM32(pDriverData))
                xCMP_r_rm   (rEDI,rmIND32(rEDX)) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwThreshold))
                xLABEL      (labelSpace)
                xJL         (0)
                {
                    xMOV_rm_r   (rmIND32(rEDX),rEDI) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))
                    xMOV_rm_imm (rmREG(rEAX),(DWORD)nvKelvinDispatchGetPusherSpace)
                    xPUSH_r     (rEDX)
                    xCALL_rm    (rmREG(rEAX))
                    xPOP_r      (rEDX)
                    xMOV_r_rm   (rEDI,rmIND32(rEDX)) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))
                }
                xTARGET_b8  (labelSpace)

                xSUB_rm_imm (rmREG(rEBX),64)
                xJMP        (labelNext)
            }
            xTARGET_b32 (labelCopy0) // back to prefetch

            //
            // do a 16 index batch
            //
            DWORD labelCopy1;
            xCMP_rm_imm (rmREG(rEBX),16)
            xLABEL      (labelCopy1)
            xJB32       (0)
            {
                xMOV_rm_imm (rmREG(rEAX),((((8) << 2) << 16) | ((NV_DD_KELVIN) << 13) | (NVPUSHER_NOINC(NV097_ARRAY_ELEMENT16))))
                xMOV_rm_r   (rmIND(rEDI),rEAX)
                xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(4)

                nvKelvinILCompile_indexcpy (32);

                xSUB_rm_imm (rmREG(rEBX),16)
                xJMP        (labelNext) // back to prefetch
            }
            xTARGET_b32 (labelCopy1)

            //
            // do a 4 index batch
            //
            DWORD labelCopy2,labelNext2;
            xLABEL      (labelNext2)
            xCMP_rm_imm (rmREG(rEBX),4)
            xLABEL      (labelCopy2)
            xJB32       (0)
            {
                xMOV_rm_imm (rmREG(rEAX),((((2) << 2) << 16) | ((NV_DD_KELVIN) << 13) | (NVPUSHER_NOINC(NV097_ARRAY_ELEMENT16))))
                xMOV_rm_r   (rmIND(rEDI),rEAX)
                xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(4)

                nvKelvinILCompile_indexcpy (8);

                xSUB_rm_imm (rmREG(rEBX),4)
                xJMP        (labelNext2)
            }
            xTARGET_b32 (labelCopy2)

            //
            // do a single index
            //
            DWORD labelCopy3,labelNext3;
            xOR_r_rm    (rEBX,rmREG(rEBX))
            xLABEL      (labelCopy3)
            xJZ32       (0)
            xMOV_rm_imm (rmREG(rEDX),8)
            xLABEL      (labelNext3)
            {
                xXOR_r_rm   (rECX,rmREG(rECX))
                xMOV_rm_imm (rmREG(rEAX),((((1) << 2) << 16) | ((NV_DD_KELVIN) << 13) | (NVPUSHER_NOINC(NV097_ARRAY_ELEMENT32))))
           x16r xMOV_r_rm   (rCX,rmIND(rESI))
                xLEA_r_rm   (rESI,rmIND8(rESI)) xOFS8(2)
                xMOV_rm_r   (rmIND(rEDI),rEAX)
                xADD_r_rm   (rEDI,rmREG(rEDX))
                xADD_r_i32  (rECX, mMEM32(global.kelvin.dwBaseVertex) ) //add base index
                xDEC_rm     (rmREG(rEBX))
                xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(4 - 8)
                xJNZ32      (labelNext3)
            }
            xTARGET_b32 (labelCopy3)
        }
    }
    else
    {
        //
        // ordered VB
        //

        //
        // compute how many vertices to send
        //
        xMOV_r_i32  (rEBX,mMEM32(global.kelvin.dwPrimCount))
        ILCCompile_mul (rEBX,dwVerticesPerPrim);
        if (dwStartVerticesPerPrim)
        {
            xADD_rm_imm (rmREG(rEBX),dwStartVerticesPerPrim)
        }

        //
        // zero out the index counter
        //
        xXOR_r_rm   (rESI,rmREG(rESI))

        //
        // send
        //
        DWORD labelNext1;
        DWORD labelNext3;
        xLABEL      (labelNext3)
        xCMP_rm_imm (rmREG(rEBX),128)
        xLABEL      (labelNext1)
        xJB32       (0)
        {
            // send 128 vertices
            xMOV_rm_imm (rmREG(rEAX),(((sizeSetNv20KelvinDrawArraysMthdCnt << 2) << 16) | ((NV_DD_KELVIN) << 13) | NV097_DRAW_ARRAYS))
            xMOV_rm_r   (rmREG(rECX),rESI)
            // xSHL_rm_imm (rmREG(rECX),0 ? NV056_DRAW_ARRAYS_START_INDEX)  // in principle this should be here, but it's a <<0
            xOR_rm_imm  (rmREG(rECX),DRF_NUM(056, _DRAW_ARRAYS, _COUNT, (128 - 1)))
            xMOV_rm_r   (rmIND(rEDI),rEAX)
            xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(4)
            xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)
            xSUB_rm_imm (rmREG(rEBX),128)  // decrement the vertex counter
            xADD_rm_imm (rmREG(rESI),128)  // increment the index counter

            // check for pusher space
            xMOV_r_i32  (rEDX,mMEM32(pDriverData))
            xCMP_r_rm   (rEDI,rmIND32(rEDX)) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwThreshold))
            xJL32       (labelNext3)
            {
                xMOV_rm_r   (rmIND32(rEDX),rEDI) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))
                xMOV_rm_imm (rmREG(rEAX),(DWORD)nvKelvinDispatchGetPusherSpace)
                xPUSH_r     (rEDX)
                xCALL_rm    (rmREG(rEAX))
                xPOP_r      (rEDX)
                xMOV_r_rm   (rEDI,rmIND32(rEDX)) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))
            }
            xJMP        (labelNext3)
        }
        xTARGET_b32     (labelNext1)

        DWORD labelNext2;
        xOR_r_rm        (rEBX,rmREG(rEBX))
        xLABEL          (labelNext2)
        xJZ32           (0)
        {
            // send rest of vertices
            xMOV_rm_imm (rmREG(rEAX),(((sizeSetNv20KelvinDrawArraysMthdCnt << 2) << 16) | ((NV_DD_KELVIN) << 13) | NV097_DRAW_ARRAYS))
            xDEC_rm     (rmREG(rEBX))
            xSHL_rm_imm8(rmREG(rEBX),0 ? NV097_DRAW_ARRAYS_COUNT)
            // xSHL_rm_imm (rmREG(rESI),0 ? NV056_DRAW_ARRAYS_START_INDEX)  // in principle this should be here, but it's a <<0
            xOR_rm_r    (rmREG(rEBX),rESI)

            xMOV_rm_r   (rmIND(rEDI),rEAX)
            xMOV_rm_r   (rmIND8(rEDI),rEBX) xOFS8(4)
            xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)
        }
        xTARGET_b32     (labelNext2)
    }

    //
    // end prim
    //
    xMOV_rm_imm (rmREG(rEAX),((sizeSetNv20KelvinBeginEndMthdCnt << 2) << 16) | ((NV_DD_KELVIN) << 13) | NV097_SET_BEGIN_END);
    xMOV_rm_imm (rmREG(rECX),NV097_SET_BEGIN_END_OP_END)
    xMOV_rm_r   (rmIND(rEDI),rEAX)
    xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(4)
    xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)

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

    //
    // save off push buffer
    //
    xMOV_r_i32  (rEDX,mMEM32(pDriverData))
#ifndef NV_NULL_HW_DRIVER
    xMOV_rm_r   (rmIND32(rEDX),rEDI) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))
#endif

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

#endif KELVIN_ILC

#endif  // NVARCH == 0x20

