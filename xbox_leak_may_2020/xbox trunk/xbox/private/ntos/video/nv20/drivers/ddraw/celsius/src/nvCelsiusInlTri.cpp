/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvCelsiusInlTri.cpp                                               *
*       indexed and ordered inline triangle lists                           *
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
void nvCelsiusILCompile_beginEnd               (DWORD dwPrimType);
void nvCelsiusILCompile_computeIndexAndOffset  (DWORD dwVBStride,DWORD dwVBLogStride);
void nvCelsiusILCompile_memcpy                 (DWORD dwCount, bool recurse);
void nvCelsiusILCompile_copyVertex             (PNVD3DCONTEXT pContext,DWORD dwFlags);
void nvCelsiusILCompile_prefetch               (BOOL bIsIndexed,BOOL bLegacyStrides);
void nvCelsiusILCompile_limit                  (DWORD regLimit,DWORD regDummy);

void __stdcall nvCelsiusDispatchGetFreeSpace   (PNVD3DCONTEXT pContext);
void __stdcall nvCelsiusDispatchGetPusherSpace (void);
#ifdef DEBUG
void __stdcall nvCelsiusDispatchFlush          (void);
void __stdcall nvCelsiusDispatchPrintVertex    (PNVD3DCONTEXT pContext,DWORD dwPutAddress);
#endif

/*****************************************************************************
 *****************************************************************************
 *** helpers *****************************************************************
 *****************************************************************************
 *****************************************************************************/

void nvCelsiusILCompile_cullcheck
(
    BOOL bIsIndexed
)
{
    //
    // get vertices into eax, ebx and ecx
    //
    if (bIsIndexed)
    {
        // already set up correctly
    }
    else
    {
        xMOV_r_i32       (rEAX,mMEM32(global.celsius.pVertices))
        xMOV_r_rm        (rEBX,rmREG(rEAX))
        xMOV_r_rm        (rECX,rmREG(rEAX))
        xADD_rm_imm      (rmREG(rEBX),global.celsius.dwVertexStride)
        xADD_rm_imm      (rmREG(rECX),global.celsius.dwVertexStride * 2)
    }

    //
    // compute cross product
    //
    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
    {
        /*
         * katmai
         */
        xMOVLPS_r_rm     (rXMM0,rmIND(rEAX))
        xMOVLPS_r_rm     (rXMM1,rmIND(rEBX))
        xSHUFPS_r_rm_imm (rXMM0,rmREG(rXMM0),0x44)          // xmm0: y0     x0      y0      x0
        xMOVHPS_r_rm     (rXMM1,rmIND(rECX))                // xmm1: y2     x2      y1      x1

        xSUBPS_r_rm      (rXMM1,rmREG(rXMM0))               // xmm1: y2-y0  x2-x0   y1-y0   x1-x0
        xSHUFPS_r_rm_imm (rXMM1,rmREG(rXMM1),0xe1)          // xmm1: y2-y0  x2-x0   x1-x0   y1-y0

        xMOVHLPS_r_r     (rXMM0,rXMM1)                      // xmm0: ?      ?       y2-y0   x2-x0
        xMULPS_r_rm      (rXMM0,rmREG(rXMM1))               // xmm0: ?      ?       (x1-x0)*(y2-y0) (y1-y0)*(x2-x0)

        xMOVAPS_r_rm     (rXMM1,rmREG(rXMM0))
        xSHUFPS_r_rm_imm (rXMM1,rmREG(rXMM1),0xe1)          // xmm1: ?      ?       ?       (x1-x0)*(y2-y0)

        xSUBSS_r_rm      (rXMM1,rmREG(rXMM0))               // xmm1: ?      ?       ?       (x1-x0)*(y2-y0)-(y1-y0)*(x2-x0)
        xMOVSS_i32_r     (mMEM32(global.dwTemp),rXMM1)
        xMOV_r_i32       (rEDX,mMEM32(global.dwTemp))
        xAND_rm_imm      (rmREG(rEDX),0x80000000)
        xCMP_r_i32       (rEDX,mMEM32(global.celsius.dwCullValue))
    }
    else
    {
        /*
         * generic
         */
        xFLD_rm32  (rmIND(rEAX))                    // X0
        xFLD_rm32  (rmIND(rEBX))                    // X1 | X0
        xFXCH_st   (rST1)                           // X0 | X1
        xFSUBP_st  (rST1)                           // X1-X0
        xFLD_rm32  (rmIND8(rECX)) xOFS8(4)          // Y2 | X1-X0
        xFXCH_st   (rST1)                           // X1-X0 | Y2
        xFLD_rm32  (rmIND8(rEAX)) xOFS8(4)          // Y0 | X1-X0 | Y2
        xFXCH_st   (rST1)                           // X1-X0 | Y0 | Y2
        xFLD_rm32  (rmIND(rECX))                    // X2 | X1-X0 | Y0 | Y2
        xFXCH_st   (rST3)                           // Y2 | X1-X0 | Y0 | X2
        xFLD_rm32  (rmIND(rEAX))                    // X0 | Y2 | X1-X0 | Y0 | X2
        xFXCH_st   (rST3)                           // Y0 | Y2 | X1-X0 | X0 | X2
        xFSUBP_st  (rST1)                           // Y2-Y0 | X1-X0 | X0 | X2
        xFXCH_st   (rST3)                           // X2 | X1-X0 | X0 | Y2-Y0
        xFLD_rm32  (rmIND8(rEBX)) xOFS8(4)          // Y1 | X2 | X1-X0 | X0 | Y2-Y0
        xFXCH_st   (rST3)                           // X0 | X2 | X1-X0 | Y1 | Y2-Y0
        xFSUBP_st  (rST1)                           // X2-X0 | X1-X0 | Y1 | Y2-Y0
        xFXCH_st   (rST3)                           // Y2-Y0 | X1-X0 | Y1 | X2-X0
        xFLD_rm32  (rmIND8(rEAX)) xOFS8(4)          // Y0 | Y2-Y0 | X1-X0 | Y1 | X2-X0
        xFXCH_st   (rST2)                           // X1-X0 | Y2-Y0 | Y0 | Y1 | X2-X0
        xFMULP_st  (rST1)                           // (X1-X0)(Y2-Y0) | Y0 | Y1 | X2-X0
        xFXCH_st   (rST1)                           // Y0 | (X1-X0)(Y2-Y0) | Y1 | X2-X0
        xFSUBP_st  (rST2)                           // (X1-X0)(Y2-Y0) | Y1-Y0 | X2-X0
        xFXCH_st   (rST2)                           // X2-X0 | Y1-Y0 | (X1-X0)(Y2-Y0)
        xFMULP_st  (rST1)                           // (X2-X0)(Y1-Y0) | (X1-X0)(Y2-Y0)
        xFSUBP_st  (rST1)                           // (X1-X0)(Y2-Y0)-(X2-X0)(Y1-Y0)
        xFSTP_i32  (mMEM32(global.dwTemp))
        xMOV_r_i32       (rEDX,mMEM32(global.dwTemp))
        xAND_rm_imm      (rmREG(rEDX),0x80000000)
        xCMP_r_i32       (rEDX,mMEM32(global.celsius.dwCullValue))
    }
}

/*****************************************************************************
 *****************************************************************************
 *** inline triangle lists ***************************************************
 *****************************************************************************
 *****************************************************************************/
DWORD nvCelsiusILCompile_inline_tri_list
(
    PNVD3DCONTEXT pContext,
    DWORD         dwFlags
)
{
    //
    // get loop constants
    //
    BOOL   bLegacyStrides = dwFlags & CELSIUS_ILFLAG_LEGACY;
    BOOL   bIsIndexed     = (dwFlags & CELSIUS_ILMASK_LOOPTYPE) == CELSIUS_ILFLAG_IX_INL_TRI_LIST;

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

    //
    // setup push buffer
    //
    xMOV_r_i32  (rEDX,mMEM32(pDriverData))
    xMOV_r_rm   (rEDI,rmIND32(rEDX)) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))

    //
    // calc how many verts to copy into EBX
    //
    xMOV_r_i32  (rEBX,mMEM32(global.celsius.dwPrimCount))
    ILCCompile_mul (rEBX,bLegacyStrides ? 4 : 3);

    //
    // setup prefetcher (EBX has # of vertices to copy in total)
    //
    if (bIsIndexed)
    {
        xMOV_r_i32  (rEAX,mMEM32(global.celsius.pIndices))
         xLEA_r_rm   (rECX,rmSIB) xSIB(rEAX,rEBX,x2)
        xMOV_i32_r  (mMEM32(global.celsius.dwPrefetchBase),rEAX)
         xMOV_i32_r  (mMEM32(global.celsius.dwPrefetchMax),rECX)
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
    xMOV_rm_imm (rmREG(rECX),NV056_SET_BEGIN_END_OP_TRIANGLES)
    xMOV_rm_r   (rmIND(rEDI),rEAX)
    xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(4)
    xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)

    //
    // load prim count into EBX
    //
    xMOV_r_i32  (rEBX,mMEM32(global.celsius.dwPrimCount))

    //
    // start copying
    //   for this loop EDI points to PUT and EBX has the amount of prims to go
    //
    DWORD labelEarlyExit;
    DWORD labelLoop;
    xLABEL  (labelLoop)
    {
        xPUSH_r     (rEBX)

        //
        // prefetch
        //
        nvCelsiusILCompile_prefetch (bIsIndexed,bLegacyStrides);

        //
        // do two triangles at a time
        //
        for (DWORD dwTriNum = 0; dwTriNum < 2; dwTriNum++)
        {
            if (bIsIndexed)
            {
                //
                // compute vertices
                //
                xMOV_r_i32  (rEDX,mMEM32(global.celsius.pIndices))
                xXOR_r_rm   (rECX,rmREG(rECX))
                xLEA_r_rm   (rEAX,rmIND8(rEDX)) xOFS8 (bLegacyStrides ? 8 : 6)
                xMOV_r_rm   (rEBX,rmIND(rEDX))
           x16r xMOV_r_rm   (rCX,rmIND8(rEDX)) xOFS8(4)
                xMOV_i32_r  (mMEM32(global.celsius.pIndices),rEAX)
                xMOV_r_rm   (rEAX,rmREG(rEBX))
                xSHR_rm_imm8(rmREG(rEBX),16)
                xAND_rm_imm (rmREG(rEAX),0xffff)
                // convert indices to vertex address with limit checking (on NT)
                        ILCCompile_mul (rECX,global.celsius.dwVertexStride);
                    ILCCompile_mul (rEBX,global.celsius.dwVertexStride);
                ILCCompile_mul (rEAX,global.celsius.dwVertexStride);
                        nvCelsiusILCompile_limit (rECX,rEDX);
                    nvCelsiusILCompile_limit (rEBX,rEDX);
                nvCelsiusILCompile_limit (rEAX,rEDX);
                    xADD_r_i32  (rEBX,mMEM32(global.celsius.pVertices))
                xADD_r_i32  (rEAX,mMEM32(global.celsius.pVertices))
                        xADD_r_i32  (rECX,mMEM32(global.celsius.pVertices))
            }

            //
            // do cull check
            //      indexed - eax,ebx and ecx has vertex addresses
            //      ordered - pVertices has vertex0, rest follow in dwVertexStride increments
            DWORD labelCull;
            if (!(dwFlags & CELSIUS_ILFLAG_NOCULL))
            {
                nvCelsiusILCompile_cullcheck (bIsIndexed);
                xLABEL      (labelCull)
                xJZ32       (0)
            }

            //
            // copy vertices
            //
            if (bIsIndexed)
            {
                xPUSH_r     (rECX)
                xPUSH_r     (rEBX)
                xMOV_r_rm   (rESI,rmREG(rEAX))
                nvCelsiusILCompile_copyVertex (pContext,dwFlags);
                xPOP_r      (rESI)
                nvCelsiusILCompile_copyVertex (pContext,dwFlags);
                xPOP_r      (rESI)
                nvCelsiusILCompile_copyVertex (pContext,dwFlags);
            }
            else
            {
                xMOV_r_i32  (rESI,mMEM32(global.celsius.pVertices))
                nvCelsiusILCompile_copyVertex (pContext,dwFlags);
                xADD_rm_imm (rmREG(rESI),global.celsius.dwVertexStride)
                nvCelsiusILCompile_copyVertex (pContext,dwFlags);
                xADD_rm_imm (rmREG(rESI),global.celsius.dwVertexStride)
                nvCelsiusILCompile_copyVertex (pContext,dwFlags);
            }

            if (!(dwFlags & CELSIUS_ILFLAG_NOCULL))
            {
                xTARGET_b32 (labelCull)
            }

            // update instrumentation count (include culled tris so we have a normalized result)
#ifdef INSTRUMENT_INNER_LOOPS
            {
                xMOV_rm_imm (rmREG(rEAX),fvfData.dwVertexStride * 3)
                xADD_i32_r  (mMEM32(global.dwBytesCopied),rEAX)
            }
#endif

            //
            // next triangle
            //
            if (bIsIndexed)
            {
                // nothing to do here
            }
            else
            {
                xMOV_r_i32  (rEAX,mMEM32(global.celsius.pVertices))
                xADD_rm_imm (rmREG(rEAX),global.celsius.dwVertexStride * 3)
                xMOV_i32_r  (mMEM32(global.celsius.pVertices),rEAX)
            }

            //
            // done already? (odd # of triangles)
            //
            if (dwTriNum == 0)
            {
                xPOP_r  (rEBX)
                xDEC_rm (rmREG(rEBX))
                xLABEL  (labelEarlyExit)
                xJZ32   (0)
                xPUSH_r (rEBX)
            }
        }

/*
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

// do in setup ....
          if (bLegacyStrides)
        {
            xMOV_r_i32  (rEAX,mMEM32(global.celsius.pIndices))
            xMOV_i32_r  (mMEM32(global.celsius.pIndexBase),rEAX)
        }


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
            ILCCompile_mul (rESI,fvfData.dwVertexStride);
            // range limit
            xXOR_r_rm   (rECX,rmREG(rECX))
            xCMP_r_i32  (rESI,mMEM32(global.dwMaxVertexOffset))
            xSETAE_rm8  (rmREG(rCL))
            xDEC_rm     (rmREG(rECX))
            xAND_r_rm   (rESI,rmREG(rECX))
            // compute vertex address
            xADD_r_i32  (rESI,mMEM32(global.celsius.pVertices))
        }
        else
        {
            // get vertex & increment
            xMOV_r_i32  (rESI,mMEM32(global.celsius.pVertices))
            xLEA_r_rm   (rEAX,rmIND32(rESI)) xOFS32(fvfData.dwVertexStride)
            xMOV_i32_r  (mMEM32(global.celsius.pVertices),rEAX)
        }

        // copy vertex (from ESI to EDI)
        nvCelsiusILCompile_copyVertex (pContext);
*/

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
    xJNZ32      (labelLoop)
    xTARGET_b32 (labelEarlyExit)

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

