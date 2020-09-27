// **************************************************************************
//
//       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.
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
//       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.
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
//  Module: nvKelvinSuperTri.cpp
//      Kelvin primitive rendering routines
//
// **************************************************************************
//
//  History:
//      Scott Kephart       06 Nov 00               Ported from Celsius
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
//#define USE_C_LOGIC     // force c logic (easier to debug)

//////////////////////////////////////////////////////////////////////////////
// aliases
//
#define ilcData         global.dwILCData
#define ilcCount        global.dwILCCount
#define ilcMax          global.dwILCMax

#define KNIMEM(x)       ((((DWORD)&((*(KATMAI_STATE*)global.kni).x[0])) + 15) & ~15)

//////////////////////////////////////////////////////////////////////////////
// externals
//
void ILCCompile_mul                            (DWORD reg,DWORD num);
void nvKelvinILCompile_limit                 (DWORD regLimit,DWORD regDummy);

void __stdcall nvKelvinDispatchGetFreeSpace   (PNVD3DCONTEXT pContext);
void __stdcall nvKelvinDispatchGetPusherSpace (void);
#ifdef DEBUG
void __stdcall nvKelvinDispatchFlush          (void);
void __stdcall nvKelvinDispatchPrintVertex    (PNVD3DCONTEXT pContext,DWORD dwPutAddress);
#endif

/*****************************************************************************
 *****************************************************************************
 *** helpers *****************************************************************
 *****************************************************************************
 *****************************************************************************/


/*****************************************************************************
 *****************************************************************************
 *** super triangle lists *********ASSUMES KNI OR 3DNOW !*************
 *****************************************************************************
 *****************************************************************************/

DWORD nvKelvinILCompile_super_tri_list
(
    PNVD3DCONTEXT pContext,
    DWORD         dwFlags
)
{

#if 1

    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_3DNOW)
    {
//*****************************************************************************************************
//*****************************************************************************************************
//
//      AMD Athlon
//
//*****************************************************************************************************
//*****************************************************************************************************

        //
        // align entry point
        //
        while (ilcCount & 31)
        {
            xINT3
        }
        DWORD lEntry;
        xLABEL (lEntry);

        //
        // setup stack frame
        //
        xPUSH_r     (rEBP)
        xPUSH_r     (rEBX)
        xPUSH_r     (rESI)
        xPUSH_r     (rEDI)

    #ifdef USE_C_LOGIC
        xMOV_rm_imm (rmREG(rEAX),dwFlags)
        xPUSH_r (rEAX)
        xMOV_rm_imm (rmREG(rEAX),proto)
        xCALL_rm (rmREG(rEAX))
    #else //USE_C_LOGIC

        //
        // setup push buffer
        //
        xMOV_r_i32  (rEDX,mMEM32(pDriverData))
        xMOV_r_rm   (rEDI,rmIND32(rEDX)) xOFS32(OFFSETOF(CDriverContext, nvPusher.m_dwPut))

        //
        // align push buffer
        //
        {
            DWORD skip,again;
            xMOV_rm_imm  (rmREG(rEAX),(((0 << 2) << 16) | ((NV_DD_KELVIN) << 13) | NV097_NO_OPERATION))
            xLABEL       (again)
            xMOV_r_rm    (rEBX,rmREG(rEDI))
            xAND_rm_imm  (rmREG(rEBX),0xf)
            xCMP_rm_imm  (rmREG(rEBX),8)
            xLABEL       (skip)
            xJZ          (0)
            xMOV_rm_r    (rmIND(rEDI),rEAX)
            xLEA_r_rm    (rEDI,rmIND8(rEDI)) xOFS8(4)
            xJMP         (again)
            xTARGET_b8   (skip)
        }

//         if (!(dwFlags & KELVIN_ILFLAG_NOCLIP))
//         {
//         }
//         else
        {

           xFEMMS
            //
            // start primitive
            //
            xMOVQ_r_i64  (rMM0,mMEM64(global.kelvin.qwBegin))
            xMOVQ_rm_r (rmIND(rEDI),rMM0)
            xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)

            //
            // setup
            //
            xMOV_r_i32  (rEAX,mMEM32(global.dwVertexSN))
            xADD_rm_imm (rmREG(rEAX),0x10000)
            xMOV_i32_r  (mMEM32(global.dwVertexSN),rEAX)

            xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
            xMOV_r_i32  (rECX,mMEM32(global.kelvin.dwPrimCount))
            xLEA_r_rm   (rEAX,rmSIB) xSIB(rECX,rECX,x2)
            xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchBase),rESI)
            xLEA_r_rm   (rEAX,rmSIB) xSIB(rEAX,rEAX,x1)
            xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchMax),rEAX)

            //
            // process all triangles
            //
            DWORD again;
            xLABEL  (again)
            xPUSH_r (rECX)

            //
            DWORD cullCheck = 0;
            if (!(dwFlags & KELVIN_ILFLAG_NOCULL))
            {
                // eye point in model space

                xMOV_r_i32 (rEAX,mMEM32(global.kelvin.dwEyeAddr))
                xMOVQ_r_rm (rMM6,rmIND(rEAX)) 
                xMOVQ_r_rm (rMM7,rmIND8(rEAX)) xOFS8(8)
                // get index
                //clear EDX

                // Vertex 0
        #define STMC_PREF_DIST_AMD (0x40)
                xXOR_r_rm   (rEDX,rmREG(rEDX))

                xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(0*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + 0*4,rEDX)

                // compute vertex address into edx
                ILCCompile_mul (rEDX,pContext->hwState.SuperTri.getSTLBStride());
                xADD_r_i32     (rEDX,mMEM32(global.kelvin.pVertices))

                xMOV_r_rm   (rEAX, rmREG(rEDX))

                xMOVQ_r_rm  (rMM0, rmIND(rEDX))             //MM0 = V0.Y V0.X
                xMOVQ_r_rm  (rMM1, rmIND8(rEDX)) xOFS8(8)   //MM1 = xxxx V0.Z

                // Vertex 1
                xXOR_r_rm   (rEDX,rmREG(rEDX))

                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(1*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + 1*4,rEDX)

                // compute vertex address into edx
                ILCCompile_mul (rEDX,pContext->hwState.SuperTri.getSTLBStride());
                xADD_r_i32     (rEDX,mMEM32(global.kelvin.pVertices))

                xMOV_r_rm   (rEBX, rmREG(rEDX))
                xMOVQ_r_rm  (rMM2, rmIND(rEDX))             //MM2 = V1.Y V1.X
                xMOVQ_r_rm  (rMM3, rmIND8(rEDX)) xOFS8(8)   //MM3 = xxxx V1.Z

                xPFSUB_r_rm     (rMM6, rmREG(rMM0))     //mm6 & mm7 = eyevec
                xPFSUB_r_rm     (rMM7, rmREG(rMM1))

                // Vertex 2
                xXOR_r_rm   (rEDX,rmREG(rEDX))

                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(2*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + 2*4,rEDX)

                // compute vertex address into edx
                ILCCompile_mul (rEDX,pContext->hwState.SuperTri.getSTLBStride());
                xADD_r_i32      (rEDX,mMEM32(global.kelvin.pVertices))

                xPFSUB_r_rm     (rMM2, rmREG(rMM0))     //mm2 & mm3 = v1 - v0 = v10
                xPFSUB_r_rm     (rMM3, rmREG(rMM1))


                xMOV_r_rm   (rECX, rmREG(rEDX))
                xMOVQ_r_rm  (rMM4, rmIND(rEDX))             //MM4 = V2.Y V2.X
                xMOVQ_r_rm  (rMM5, rmIND8(rEDX)) xOFS8(8)   //MM5 = xxxx V2.Z

                // Compute eye vector
//                xCMP_r_rm   (rEBX, rmREG(rEAX))


                // Compute Normal

//                xCMOVA_r_rm (rEAX, rmREG(rEBX))
                xAND_rm_imm (rmREG(rEAX), ~0x3f)
                if (pContext->hwState.SuperTri.isFixedLBStride())
                {
                    xPREFETCH_rm8(0, rmIND8(rEAX), STMC_PREF_DIST_AMD)
                }
                else
                {
                    if ((4 * pContext->hwState.SuperTri.getSTLBStride()) < 128) {
                        xPREFETCH_rm8(0, rmIND8(rEAX), 4 * pContext->hwState.SuperTri.getSTLBStride())
                    }
                    else {
                        xADD_rm_imm(rmREG(rEAX), 4 * pContext->hwState.SuperTri.getSTLBStride())
                        xPREFETCH_rm8(0, rmIND8(rEAX), 0)
                    }
                }

                xPFSUB_r_rm     (rMM4, rmREG(rMM0))     //mm4 & mm5 = v2 - v0 = v20
                xPFSUB_r_rm     (rMM5, rmREG(rMM1))

                //      0.0 V10.X   V10.Z   V10.Y
                //    * 0.0 V20.y   V20.X   V20.Z
                //    -
                //      0.0 V10.Y   V10.X   V10.Z
                //    * 0.0 V20.X   V20.Z   V20.Y



                // MM2 = V10.Y V10.X
                // MM3 = xxxxx V10.Z
                // MM4 = V20.Y V20.X
                // MM5 = xxxxx V20.Z

                xPUNPCKLDQ_r_rm (rMM3, rmREG(rMM3))     //mm3 = V10.Z V10.Z
                xPUNPCKLDQ_r_rm (rMM5, rmREG(rMM5))     //mm5 = v20.z v20.z

//                xCMP_r_rm   (rECX, rmREG(rEAX))

                // MM2 = V10.Y V10.X
                // MM3 = v10.z V10.Z
                // MM4 = V20.Y V20.X
                // MM5 = v20.z V20.Z


                xMOVQ_r_rm      (rMM0, rmREG(rMM2))
                xMOVQ_r_rm      (rMM1, rmREG(rMM4))

//                xCMOVA_r_rm (rEAX, rmREG(rECX))
                xAND_rm_imm (rmREG(rEBX), ~0x3f)
                if (pContext->hwState.SuperTri.isFixedLBStride())
                {
                    xPREFETCH_rm8(0, rmIND8(rEBX), STMC_PREF_DIST_AMD)
                }
                else
                {
                    if ((4 * pContext->hwState.SuperTri.getSTLBStride()) < 128) {
                        xPREFETCH_rm8(0, rmIND8(rEBX), 4 * pContext->hwState.SuperTri.getSTLBStride())
                    }
                    else {
                        xADD_rm_imm(rmREG(rEBX), 4 * pContext->hwState.SuperTri.getSTLBStride())
                        xPREFETCH_rm8(0, rmIND8(rEBX), 0)
                    }
                }

                xPUNPCKHDQ_r_rm (rMM0, rmREG(rMM0))
                xPUNPCKHDQ_r_rm (rMM1, rmREG(rMM1))

//                xAND_rm_imm (rmREG(rEAX), ~0x3f)
                xAND_rm_imm (rmREG(rECX), ~0x3f)

                //MM0 = V10.Y V10.Y
                //MM1 = V20.Y V20.Y

                xPFMUL_r_rm     (rMM0, rmREG(rMM4)) //mm0 = xxxx V10.Y*V20.X
                xPFMUL_r_rm     (rMM1, rmREG(rMM2)) //mm1 = xxxx v20.Y*V10.X
                xPFSUB_r_rm     (rMM1, rmREG(rMM0)) //mm1 = xxxx Z'

                xPUNPCKLDQ_r_rm (rMM3, rmREG(rMM2)) //mm3 = v10.x v10.z
                xPUNPCKLDQ_r_rm (rMM5, rmREG(rMM4)) //mm5 = v20.x v20.z

                xPUNPCKHDQ_r_rm (rMM2, rmREG(rMM2)) //mm2 = v10.Y v10.Y
                xPUNPCKHDQ_r_rm (rMM4, rmREG(rMM4)) //mm4 = V20.Y V20.Y

                xPUNPCKLDQ_r_rm (rMM2, rmREG(rMM3)) //mm2 = V10.Z V10.Y
                xPUNPCKLDQ_r_rm (rMM4, rmREG(rMM5)) //mm4 = v20.z v20.y

                xPFMUL_r_rm     (rMM2, rmREG(rMM5))
                xPFMUL_r_rm     (rMM4, rmREG(rMM3))

                if (pContext->hwState.SuperTri.isFixedLBStride())
                {
                    xPREFETCH_rm8   (0, rmIND8(rECX), STMC_PREF_DIST_AMD)
                }
                else
                {
                    if ((4 * pContext->hwState.SuperTri.getSTLBStride()) < 128) {
                        xPREFETCH_rm8(0, rmIND8(rECX), 4 * pContext->hwState.SuperTri.getSTLBStride())
                    }
                    else {
                        xADD_rm_imm(rmREG(rECX), 4 * pContext->hwState.SuperTri.getSTLBStride())
                        xPREFETCH_rm8(0, rmIND8(rECX), 0)
                    }
                }

                xPFSUB_r_rm     (rMM2, rmREG(rMM4)) //mm2 = Y' X'

                // Dot normal and eye vector

                xPFMUL_r_rm     (rMM2, rmREG(rMM6))     //mm2 = Y' * eyevec.Y   X'*eyevec.X
                xPFMUL_r_rm     (rMM1, rmREG(rMM7))     //mm1 = xxxx            Z' * eyevec.Z
                xMOVQ_r_rm      (rMM0, rmREG(rMM2))     //mm0 = Y' * eyevec.Y   X'*eyevec.X
                xPUNPCKHDQ_r_rm (rMM0, rmREG(rMM0))     //mm0 = Y' * eyevec.Y   Y'*eyevec.Y
                xPFADD_r_rm     (rMM2, rmREG(rMM1))
                xPFADD_r_rm     (rMM2, rmREG(rMM0))

                xMOV_r_i32      (rEAX,mMEM32(global.kelvin.dwCullAddr))
                xMOV_r_rm       (rEBX,rmIND(rEAX)) 

                xMOVD_rm_r      (rmREG(rEAX), rMM2)
                xSHR_rm_imm8    (rmREG(rEAX), 31)

                xXOR_r_rm       (rEAX, rmREG(rEBX))
                xXOR_rm_imm     (rmREG(rEAX), 0x00000001)
                xTEST_rm_imm    (rmREG(rEAX), 0x00000001)


                //reads EBX,ECX,EDX

                xLABEL           (cullCheck)
                xJNZ32            (0)
            }
            else
            {
                xXOR_r_rm   (rEDX,rmREG(rEDX))

                xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(0*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + 0*4,rEDX)

                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(1*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + 1*4,rEDX)

                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(2*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + 2*4,rEDX)
            }

            //
            // dispatch
            //

            xMOVQ_r_i64(rMM1,KNIMEM(dwKelvinTriDispatch))
            xMOVQ_r_i64(rMM2,KNIMEM(dwKelvinTriDispatch)+8)
            xMOVNTQ_rm_r (rmIND(rEDI),rMM1)
            xMOVNTQ_rm_r (rmIND8(rEDI),rMM2) xOFS8(8)
            xLEA_r_rm      (rEDI,rmIND8(rEDI)) xOFS8(16)

            //
            // send more triangles after this one, utilizing connectivity statistics
            //
    #if 1
            if (!pContext->hwState.SuperTri.isCheckAll() || (dwFlags & KELVIN_ILFLAG_NOCULL))
            {
                const int count = 4; // do not exceed 128 words (about 14 of these)
                DWORD sendMore;
                xPOP_r      (rECX)
                xCMP_rm_imm (rmREG(rECX),count)
                xLABEL      (sendMore)
                xJLE32      (0)

                xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
                for (int i = 0; i < count; i++)
                {
                    // prep tri
                    xLEA_r_rm   (rESI,rmIND8(rESI)) xOFS8(6)
                    xXOR_r_rm   (rEBX,rmREG(rEBX))
                    xMOV_r_rm   (rEAX,rmIND(rESI))
                    x16r xMOV_r_rm   (rBX,rmIND8(rESI)) xOFS8(4)
                    xADD_r_i32       (rEBX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    xMOV_r_rm   (rEDX,rmREG(rEAX))
                    xAND_rm_imm (rmREG(rEAX),0xffff)
                    xADD_r_i32       (rEAX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 12,rEBX)
                    xSHR_rm_imm8(rmREG(rEDX),16)
                    xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4,rEAX)
                    xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 8,rEDX)

                    // dispatch
                    xMOVQ_r_i64 (rMM0,KNIMEM(dwKelvinTriDispatch))
                    xMOVQ_r_i64 (rMM1,KNIMEM(dwKelvinTriDispatch)+8)
                    xMOVNTQ_rm_r   (rmIND(rEDI),rXMM0)
                    xMOVNTQ_rm_r   (rmIND8(rEDI),rXMM1) xOFS8(8)
                    xLEA_r_rm      (rEDI,rmIND8(rEDI)) xOFS8(16)

                }
                xSUB_rm_imm (rmREG(rECX),count)
                xMOV_i32_r  (mMEM32(global.kelvin.pIndices),rESI)
                xTARGET_b32 (sendMore)
                xPUSH_r     (rECX)
            }
    #endif
            // check for pusher space
            DWORD labelSpace;
            xMOV_r_i32  (rESI,mMEM32(pDriverData))
            xCMP_r_rm   (rEDI,rmIND32(rESI)) xOFS32(OFFSETOF(CDriverContext, nvPusher.m_dwThreshold))
            xLABEL      (labelSpace)
            xJL         (0)
            {
                xMOV_rm_imm (rmREG(rEBX),(DWORD)nvKelvinDispatchGetPusherSpace)
                xMOV_rm_r   (rmIND32(rESI),rEDI) xOFS32(OFFSETOF(CDriverContext, nvPusher.m_dwPut))
                xCALL_rm    (rmREG(rEBX))
                xMOV_r_rm   (rEDI,rmIND32(rESI)) xOFS32(OFFSETOF(CDriverContext, nvPusher.m_dwPut))
            }
            xTARGET_b8  (labelSpace)

            //
            // next triangle
            //
            if (cullCheck)
            { xTARGET_b32 (cullCheck)
            }
            xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
            xPOP_r      (rECX)
            xLEA_r_rm   (rESI,rmIND8(rESI)) xOFS8(6)
            xDEC_rm     (rmREG(rECX))
            xMOV_i32_r  (mMEM32(global.kelvin.pIndices),rESI)
            xJNZ32      (again)

            //
            // end primitive
            //
            xMOVQ_r_i64  (rMM0,mMEM64(global.kelvin.qwEnd))
            xMOVNTQ_rm_r (rmIND(rEDI),rMM0)
            xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)
            xFEMMS


        }//#endif // ~INVERSE_XFORM_CULL

        //
        // save off push buffer
        //
        xMOV_r_i32  (rEDX,mMEM32(pDriverData))
    #ifndef NV_NULL_HW_DRIVER
        xMOV_rm_r   (rmIND32(rEDX),rEDI) xOFS32(OFFSETOF(CDriverContext, nvPusher.m_dwPut))
    #endif

    #endif // USE_C_LOGIC
        //
        // clean up caching strategy
        //
        //     xMOV_r_i32  (rEAX,mMEM32(pDriverData))
        //     xMOV_rm_imm (rmREG(rEBX),rzSetAGPMTRR)
        //     xPUSH_imm   (1)
        //     xPUSH_rm    (rmIND32(rEAX)) xOFS32(OFFSETOF(GLOBALDATA, GARTPhysicalBase))
        //     xCALL_rm    (rmREG(rEBX))

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

    else
    {

//*****************************************************************************************************
//*****************************************************************************************************
//
//      Intel SSE & SSE2
//
//*****************************************************************************************************
//*****************************************************************************************************


        //
        // align entry point
        //
        while (ilcCount & 31)
        {
            xINT3
        }
        DWORD lEntry;
        xLABEL (lEntry);

        //
        // setup stack frame
        //
        xPUSH_r     (rEBP)
        xPUSH_r     (rEBX)
        xPUSH_r     (rESI)
        xPUSH_r     (rEDI)

    #ifdef USE_C_LOGIC
        xMOV_rm_imm (rmREG(rEAX),dwFlags)
        xPUSH_r (rEAX)
        xMOV_rm_imm (rmREG(rEAX),proto)
        xCALL_rm (rmREG(rEAX))
    #else //USE_C_LOGIC

        //
        // setup push buffer
        //
        xMOV_r_i32  (rEDX,mMEM32(pDriverData))
        xMOV_r_rm   (rEDI,rmIND32(rEDX)) xOFS32(OFFSETOF(CDriverContext, nvPusher.m_dwPut))

        //
        // align push buffer
        //
        {
            DWORD skip,again;
            xMOV_rm_imm  (rmREG(rEAX),(((0 << 2) << 16) | ((NV_DD_KELVIN) << 13) | NV097_NO_OPERATION))
            xLABEL       (again)
            xMOV_r_rm    (rEBX,rmREG(rEDI))
#ifndef SW_WC
            xAND_rm_imm  (rmREG(rEBX),0xf)
            xCMP_rm_imm  (rmREG(rEBX),8)
#else
            xAND_rm_imm  (rmREG(rEBX),0x3f)
            xCMP_rm_imm  (rmREG(rEBX),0x38)
#endif
            xLABEL       (skip)
            xJZ          (0)
            xMOV_rm_r    (rmIND(rEDI),rEAX)
            xLEA_r_rm    (rEDI,rmIND8(rEDI)) xOFS8(4)
            xJMP         (again)
            xTARGET_b8   (skip)
        }

        //
        // setup caching strategy
        //
        //     xMOV_r_i32  (rEAX,mMEM32(pDriverData))
        //     xMOV_rm_imm (rmREG(rEBX),rzSetAGPMTRR)
        //     xPUSH_imm   (4)
        //     xPUSH_rm    (rmIND32(rEAX)) xOFS32(OFFSETOF(GLOBALDATA, GARTPhysicalBase))
        //     xCALL_rm    (rmREG(rEBX))
        if (pContext->hwState.SuperTri.isXFormCull())
        {


            //
            // load KNI matrix
            //
            if ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI))
            {
                xMOV_r_i32   (rEAX,mMEM32(global.kelvin.pContext_kelvinState))
                xMOVLPS_r_rm (rXMM4,rmIND32(rEAX)) xOFS32(OFFSETOF(CHardwareState, mTransform._11))
                xMOVHPS_r_rm (rXMM4,rmIND32(rEAX)) xOFS32(OFFSETOF(CHardwareState, mTransform._13))
                xMOVLPS_r_rm (rXMM5,rmIND32(rEAX)) xOFS32(OFFSETOF(CHardwareState, mTransform._21))
                xMOVHPS_r_rm (rXMM5,rmIND32(rEAX)) xOFS32(OFFSETOF(CHardwareState, mTransform._23))
                xMOVLPS_r_rm (rXMM6,rmIND32(rEAX)) xOFS32(OFFSETOF(CHardwareState, mTransform._31))
                xMOVHPS_r_rm (rXMM6,rmIND32(rEAX)) xOFS32(OFFSETOF(CHardwareState, mTransform._33))
                xMOVLPS_r_rm (rXMM7,rmIND32(rEAX)) xOFS32(OFFSETOF(CHardwareState, mTransform._41))
                xMOVHPS_r_rm (rXMM7,rmIND32(rEAX)) xOFS32(OFFSETOF(CHardwareState, mTransform._43))
            }



            //
            // start primitive
            //
            xMOVQ_r_i64  (rMM0,mMEM64(global.kelvin.qwBegin))
            xMOVNTQ_rm_r (rmIND(rEDI),rMM0)
            xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)
            xEMMS


            //
            // setup
            //
            xMOV_r_i32  (rEAX,mMEM32(global.dwVertexSN))
            xADD_rm_imm (rmREG(rEAX),0x10000)
            xMOV_i32_r  (mMEM32(global.dwVertexSN),rEAX)

            xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
            xMOV_r_i32  (rECX,mMEM32(global.kelvin.dwPrimCount))
            xLEA_r_rm   (rEAX,rmSIB) xSIB(rECX,rECX,x2)
            xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchBase),rESI)
            xLEA_r_rm   (rEAX,rmSIB) xSIB(rEAX,rEAX,x1)
            xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchMax),rEAX)
            xXOR_r_rm   (rEAX, rmREG(rEAX))
            xMOV_i32_r  (mMEM32(global.kelvin.dwPrefMaxIdx), rEAX)

            //
            // process all triangles
            //
            {
                DWORD again;
                xLABEL  (again)
                xPUSH_r (rECX)

                //
                // prefetch HERE

                xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))

                const DWORD dist = 3;
                DWORD noPrefetch;
                xCMP_rm_imm     (rmREG(rECX), dist)
                xLABEL      (noPrefetch)
                xJBE32      (0)


                xMOV_r_i32  (rEAX,mMEM32(global.kelvin.dwPrefMaxIdx))
                xXOR_r_rm   (rEDX,rmREG(rEDX))
                for (int i = 0; i < 3; i++)
                {
                    x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(((dist*3)+i)*2)
                    xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))     //******** BV
                    if (!pContext->hwState.SuperTri.isFixedLBStride())
                        xAND_rm_imm (rmREG(rEDX), ~1)
                        DWORD noPre1=0;
                    xCMP_r_rm   (rEDX, rmREG(rEAX))
                    xLABEL      (noPre1)
                    xJBE        (0)

                    xMOV_r_rm   (rEAX, rmREG(rEDX))
                    xMOV_r_rm   (rEBX, rmREG(rEDX))
                    ILCCompile_mul (rEDX, pContext->hwState.SuperTri.getSTLBStride());
                    xADD_r_i32     (rEBX,mMEM32(global.kelvin.pVertices))
                    xPREFETCH_rm8(1, rmIND8(rEBX), 0)
                    xTARGET_b8 (noPre1)
                }

                xMOV_i32_r  (mMEM32(global.kelvin.dwPrefMaxIdx), rEAX)
                xTARGET_b32 (noPrefetch)

                //
                // do transform, outcode and w-divide calculations. use cached verts if possible
                //
                for (i = 0; i < 3; i++)
                {
                    // get index
                    //clear EDX
                    xXOR_r_rm   (rEDX,rmREG(rEDX))
                    xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
                    x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(i*2)
                    xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    // compute cache index (save edx)
                    xMOV_r_rm   (rEAX,rmREG(rEDX))
                    xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + i*4,rEDX)
                    xAND_rm_imm (rmREG(rEAX),VB_CACHE_SIZE - 1)
                    // arbitration (save eax,edx)
                    DWORD arb;
                    xLABEL      (arb)
                    switch (i)
                    {
                    case 1:
                        {
                            DWORD skip;

                            xCMP_r_i32  (rEAX,mMEM32(global.dwIndex0))
                            xLABEL      (skip)
                            xJNZ        (0)
                            xADD_rm_imm (rmREG(rEAX),3)
                            xAND_rm_imm (rmREG(rEAX),VB_CACHE_SIZE - 1)
                            xJMP        (arb)

                            xTARGET_b8  (skip)
                            break;
                        }
                    case 2:
                        {
                            DWORD skip1,skip2;

                            xCMP_r_i32  (rEAX,mMEM32(global.dwIndex0))
                            xLABEL      (skip1)
                            xJNZ        (0)
                            xADD_rm_imm (rmREG(rEAX),3)
                            xAND_rm_imm (rmREG(rEAX),VB_CACHE_SIZE - 1)
                            xJMP        (arb)

                            xTARGET_b8  (skip1)
                            xCMP_r_i32  (rEAX,mMEM32(global.dwIndex1))
                            xLABEL      (skip2)
                            xJNZ        (0)
                            xADD_rm_imm (rmREG(rEAX),3)
                            xAND_rm_imm (rmREG(rEAX),VB_CACHE_SIZE - 1)
                            xJMP        (arb)

                            xTARGET_b8  (skip2)
                            break;
                        }
                    }
                    xMOV_i32_r  (mMEM32(global.dwIndex0) + i*4,rEAX)
                    // compute TL vertex address (save eax,edx)
                    xMOV_r_rm   (rEBX,rmREG(rEAX))
                    xMOV_r_rm   (rECX,rmREG(rEDX))
                    ILCCompile_mul (rEBX,sizeof(KATMAI_STATE::TLVERTEX));
                    xADD_rm_imm (rmREG(rEBX),KNIMEM(adwTLVertexCache))
                    xOR_r_i32   (rECX,mMEM32(global.dwVertexSN))
                    xMOV_i32_r  (mMEM32(global.dwVertex0) + i*4,rEBX)
                    // test cache (save eax,ecx,edx)
                    DWORD cacheHit;
                    xCMP_r_rm   (rECX,rmSIB) xSIBID(rEAX,x4) xOFS32(mMEM32(global.dwVertexCacheIndex))
                    xLABEL      (cacheHit)
                    xJZ32       (0)

                    //
                    // cache miss
                    //

                    // occupy cache entry (save edx)
                    xMOV_rm_r (rmSIB,rECX) xSIBID(rEAX,x4) xOFS32(mMEM32(global.dwVertexCacheIndex))

                    // compute vertex address into edx
                    ILCCompile_mul (rEDX,pContext->hwState.SuperTri.getSTLBStride());
                    xADD_r_i32     (rEDX,mMEM32(global.kelvin.pVertices))


                    //
                    // transform vertex and apply min/max info
                    //

                    xMOV_r_i32       (rECX,mMEM32(global.dwVertex0) + i*4)

                    ///katmai
                    if ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI))
                    {



                        //0: 0 0 0 X
                        xMOVSS_r_rm      (rXMM0,rmIND(rEDX))
                        //1: 0 0 0 Y
                        xMOVSS_r_rm      (rXMM1,rmIND8(rEDX)) xOFS8(4)
                        //0: X X X X
                        xSHUFPS_r_rm_imm (rXMM0,rmREG(rXMM0),0)
                        //1: Y Y Y Y
                        xSHUFPS_r_rm_imm (rXMM1,rmREG(rXMM1),0)
                        //2: 0 0 0 Z
                        xMOVSS_r_rm      (rXMM2,rmIND8(rEDX)) xOFS8(8)
                        //0: 3X 2X 1X 0X
                        xMULPS_r_rm      (rXMM0,rmREG(rXMM4))
                        //2: Z Z Z Z
                        xSHUFPS_r_rm_imm (rXMM2,rmREG(rXMM2),0)
                        //1: 7y 6y 5y 4y
                        xMULPS_r_rm      (rXMM1,rmREG(rXMM5))
                        //2:11Z 10Z 9Z 8Z
                        xMULPS_r_rm      (rXMM2,rmREG(rXMM6))
                        //0: 0+1
                        xADDPS_r_rm      (rXMM0,rmREG(rXMM1))
                        //0: 0+2
                        xADDPS_r_rm      (rXMM0,rmREG(rXMM2))
                        //0: 0+7 (15 14 13 12)
                        xADDPS_r_rm (rXMM0,rmREG(rXMM7))
                        //store xformed WZYX -> XYZW
                        xMOVAPS_rm_r     (rmIND(rECX),rXMM0)

                        //!!!!stores to ECX

                        //deadbeef
                        // start w-divide (save ecx)
                        ///load wxyz into xmm1
                        xMOVAPS_r_rm     (rXMM1,rmREG(rXMM0))
                        /// spread W over xmm0
                        xSHUFPS_r_rm_imm (rXMM0,rmREG(rXMM0),0xff)
                        /// xmm1 -> w/w z/w y/w x/w
                        //                   xDIVPS_r_rm      (rXMM1,rmREG(rXMM0))
                        //                 Replace the DIVPS with approximation 1/w rcpps with 1 Newton-Raphson iteration
                        //                 This is faster on all cpu's, and is accurate to within 2 bits of precision of divps
                        // Compute 1/w
                        xRCPPS_r_rm      (rXMM2, rmREG(rXMM0))
                        xMULPS_r_rm      (rXMM0, rmREG(rXMM2))
                        xMULPS_r_rm      (rXMM0, rmREG(rXMM2))
                        xADDPS_r_rm      (rXMM2, rmREG(rXMM2))
                        xSUBPS_r_rm      (rXMM2, rmREG(rXMM0))
                        // Multiply w z y x by 1/w 1/w 1/w 1/w
                        xMULPS_r_rm      (rXMM1, rmREG(rXMM2))

                    }
                    /*
                    else
                    {
                    ///3dnow

                    //xFEMMS
                    xEMMS

                    //load EBX with matrix start
                    xMOV_r_i32   (rEBX,mMEM32(global.kelvin.pContext_celsiusState))

                    xMOV_r_i32   (rEAX,mMEM32(global.celsius.dwTouchedMin))
                    //MOVQ MM0, [EDX]
                    xMOVQ_r_rm  (rMM0,rmIND(rEDX));
                    //MOVQ MM1, [EDX+8]
                    xMOVQ_r_rm  (rMM1,rmIND8(rEDX)); xOFS8(8)

                    //MOVQ MM2, MM0
                    xMOVQ_r_rm(rMM2,rmREG(rMM0))
                    //MOVQ MM3, [EBX+M00] //1
                    xMOVQ_r_rm   (rMM3,rmIND32(rEBX)) xOFS32(OFFSETOF(CCelsiusState,mTransform) + 0)

                    xCMP_r_rm    (rEDX,rmREG(rEAX))

                    //PUNPCKLDQ MM0, MM0
                    xPUNPCKLDQ_r_rm(rMM0, rmREG(rMM0));
                    //MOVQ MM4, [EBX+M10] //2
                    xMOVQ_r_rm   (rMM4,rmIND32(rEBX)) xOFS32(OFFSETOF(CCelsiusState,mTransform) + 16)

                    xCMOVB_r_rm  (rEAX,rmREG(rEDX))

                    //PFMUL MM3, MM0
                    xPFMUL_r_rm(rMM3,rmREG(rMM0))
                    //PUNPCKHDQ MM2,MM2
                    xPUNPCKHDQ_r_rm(rMM2,rmREG(rMM2))

                    xMOV_i32_r   (mMEM32(global.celsius.dwTouchedMin),rEAX)

                    //PFMUL MM4, MM2
                    xPFMUL_r_rm(rMM4,rmREG(rMM2))
                    //MOVQ MM5, [EBX+M02] //3
                    xMOVQ_r_rm   (rMM5,rmIND32(rEBX)) xOFS32(OFFSETOF(CCelsiusState,mTransform) + 8)


                    //MOVQ MM7, [EBX+M12] //4
                    xMOVQ_r_rm   (rMM7,rmIND32(rEBX)) xOFS32(OFFSETOF(CCelsiusState,mTransform) + 24)
                    //MOVQ MM6, MM1
                    xMOVQ_r_rm(rMM6,rmREG(rMM1))


                    //==========don't move anything that's pipelined accross this line======
                    //switch to EAX for matrix base
                    xMOV_r_rm (rEAX, rmREG(rEBX))

                    //PFMUL MM5,MM0
                    xPFMUL_r_rm(rMM5,rmREG(rMM0))
                    //MOVQ MM0,[EAX+M20]  //5
                    xMOVQ_r_rm   (rMM0,rmIND32(rEAX)) xOFS32(OFFSETOF(CCelsiusState,mTransform) + 32)

                    //EBX -- merge in second part
                    xMOV_r_i32   (rEBX,mMEM32(global.celsius.dwTouchedMax))


                    //PUNPCKLDQ MM1, MM1
                    xPUNPCKLDQ_r_rm(rMM1,rmREG(rMM1))
                    //PFMUL MM7, MM2
                    xPFMUL_r_rm(rMM7,rmREG(rMM2))

                    //MOVQ MM2,[EAX+M22] //6
                    xMOVQ_r_rm   (rMM2,rmIND32(rEAX)) xOFS32(OFFSETOF(CCelsiusState,mTransform) + 40)
                    //PFMUL MM0,MM1
                    xPFMUL_r_rm(rMM0,rmREG(rMM1))

                    xCMP_r_rm    (rEDX,rmREG(rEBX))

                    //PFADD MM3, MM4
                    xPFADD_r_rm(rMM3,rmREG(rMM4))
                    //MOVQ MM4, [EAX+M30] //7
                    xMOVQ_r_rm   (rMM4,rmIND32(rEAX)) xOFS32(OFFSETOF(CCelsiusState,mTransform) + 48)

                    xCMOVA_r_rm  (rEBX,rmREG(rEDX))
                    xMOV_i32_r   (mMEM32(global.celsius.dwTouchedMax),rEBX)

                    //PFMUL MM2, MM1
                    xPFMUL_r_rm(rMM2,rmREG(rMM1))
                    //PFADD MM5, MM7
                    xPFADD_r_rm(rMM5,rmREG(rMM7))

                    //MOVQ MM1,[EAX+M32] //8
                    xMOVQ_r_rm   (rMM1,rmIND32(rEAX)) xOFS32(OFFSETOF(CCelsiusState,mTransform) + 56)
                    //PFADD MM3, MM0
                    xPFADD_r_rm(rMM3,rmREG(rMM0))

                    //PFADD MM5, MM2
                    xPFADD_r_rm(rMM5,rmREG(rMM2))
                    //PFADD MM3, MM4
                    xPFADD_r_rm(rMM3,rmREG(rMM4))

                    //MOVQ [EDX], MM3  Y X -> xy
                    xMOVQ_rm_r(rmIND(rECX),rMM3)

                    //PFADD  MM5, MM1
                    xPFADD_r_rm(rMM5,rmREG(rMM1))

                    //MOVQ [EDX+8], MM5 WZ -> zw
                    xMOVQ_rm_r(rmIND8(rECX),rMM5) xOFS8(8)


                    //xFEMMS
                    xEMMS




                    } ///xform and min/max loop
                    */

                    //             xMOV_r_i32  (rEBX,mMEM32(global.celsius.pContext))
                    //             xMOV_r_rm   (rEBX,rmIND32(rEBX)) xOFS32(OFFSETOF(NVD3DCONTEXT,dwRenderState[D3DRENDERSTATE_CLIPPING]))
                    //             xTEST_r_rm  (rEBX, rmREG(rEBX))
                    //             DWORD noClip = 0;
                    //             xLABEL      (noClip)
                    //             xJZ32       (0)


                    //
                    // generate out codes (under divide) (save ecx)
                    //

                    ///clear esi
                    xXOR_r_rm    (rESI,rmREG(rESI))
                    /// mov ebx, 1
                    xMOV_rm_imm  (rmREG(rEBX),1)
                    /// mov [ecx+16], dwtouchedmin
                    xMOV_rm_r    (rmIND8(rECX),rEAX) xOFS8(16)
                    xFLD_rm32    (rmIND8(rECX)) xOFS8(12)           // w
                    xFLD_st      (rST0)                             // w w
                    xFCHS                                           // -w w

                    // x < -w
                    xFLD_rm32    (rmIND(rECX))                      // x -w w
                    xXOR_r_rm    (rEAX,rmREG(rEAX))
                    xFCOMI_st    (rST1)                             // x -w w
                    xMOV_rm_imm  (rmREG(rEBX),1)
                    xCMOVB_r_rm  (rEAX,rmREG(rEBX))
                    xOR_r_rm     (rESI,rmREG(rEAX))

                    // x > w
                    xXOR_r_rm    (rEAX,rmREG(rEAX))
                    xFCOMIP_st   (rST2)                             // -w w
                    xMOV_rm_imm  (rmREG(rEBX),2)
                    xCMOVA_r_rm  (rEAX,rmREG(rEBX))
                    xOR_r_rm     (rESI,rmREG(rEAX))

                    // y < -w
                    xFLD_rm32    (rmIND8(rECX)) xOFS8(4)            // y -w w
                    xXOR_r_rm    (rEAX,rmREG(rEAX))
                    xFCOMI_st    (rST1)                             // y -w w
                    xMOV_rm_imm  (rmREG(rEBX),4)
                    xCMOVB_r_rm  (rEAX,rmREG(rEBX))
                    xOR_r_rm     (rESI,rmREG(rEAX))

                    // y > w
                    xXOR_r_rm    (rEAX,rmREG(rEAX))
                    xFCOMIP_st   (rST2)                             // -w w
                    xMOV_rm_imm  (rmREG(rEBX),8)
                    xCMOVA_r_rm  (rEAX,rmREG(rEBX))
                    xOR_r_rm     (rESI,rmREG(rEAX))

                    // z < 0
                    xFLDZ                                           // 0 -w w
                    xFLD_rm32    (rmIND8(rECX)) xOFS8(8)            // z 0 -w w
                    xXOR_r_rm    (rEAX,rmREG(rEAX))
                    xFCOMI_st    (rST1)                             // z 0 -w w
                    xMOV_rm_imm  (rmREG(rEBX),16)
                    xCMOVB_r_rm  (rEAX,rmREG(rEBX))
                    xOR_r_rm     (rESI,rmREG(rEAX))

                    // z > w
                    xXOR_r_rm    (rEAX,rmREG(rEAX))
                    xFCOMIP_st   (rST3)                             // 0 -w w
                    xMOV_rm_imm  (rmREG(rEBX),32)
                    xCMOVA_r_rm  (rEAX,rmREG(rEBX))
                    xOR_r_rm     (rESI,rmREG(rEAX))

                    xFFREE_st    (rST0)
                    xFINCSTP
                    xFFREE_st    (rST0)
                    xFINCSTP
                    xFFREE_st    (rST0)
                    xFINCSTP

                    xMOV_r_rm    (rEAX,rmIND8(rECX)) xOFS8(12)
                    xMOV_rm_r    (rmIND8(rECX),rESI) xOFS8(16)

                    //             xTARGET_b32  (noClip)

                    //
                    // store result of divide
                    //
                    ///katmai
                    if ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI))
                    {
                        ///store x/w y/w z/w w/w

                        // wzyx -> xyzw
                        xMOVAPS_rm_r (rmIND(rECX),rXMM1)
                        //!!! stores EAX tp ECX
                        xMOV_rm_r    (rmIND8(rECX),rEAX) xOFS8(20)


                    }
                    /*
                    else
                    {
                    ///3dnow, temporary, PIPELINE THE DIVIDE, REWRITE CLIP CODES TO 3DNOW AND MERGE

                    //xFEMMS
                    xEMMS

                    //reload vertex
                    //MOVQ MM0, [EDX]
                    ///0 =  Y X
                    xMOVQ_r_rm  (rMM0,rmIND(rECX));

                    //MOVQ MM1, [EDX+8]
                    ///1=  W Z
                    xMOVQ_r_rm  (rMM1,rmIND8(rECX)); xOFS8(8)

                    //2=W Z
                    xMOVQ_r_rm(rMM2,rmREG(rMM1))

                    //temp code
                    //4 = 0 W
                    xMOVD_r_rm(rMM4, rmIND8(rECX)) xOFS8(12)
                    xPFRCP_r_rm (rMM2,rmREG(rMM4))
                    xPFRCPIT1_r_rm (rMM4,rmREG(rMM2))
                    xPFRCPIT2_r_rm (rMM4,rmREG(rMM2))


                    //expand w
                    //2 = W W
                    //xPUNPCKHDQ_r_rm(rMM2,rmREG(rMM2))
                    // 1/w | 1/w
                    //xPFRCP_r_rm (rMM2,rmREG(rMM2))
                    //xPFRCPIT1_r_rm (rMM2,rmREG(rMM2))
                    //xPFRCPIT2_r_rm (rMM2,rmREG(rMM2))

                    //y/w, x/w
                    xPFMUL_r_rm(rMM0,rmREG(rMM2))  //was2
                    //w/w z/w
                    xPFMUL_r_rm(rMM1,rmREG(rMM2))  //was2

                    //store y,x
                    xMOVQ_rm_r(rmIND(rECX),rMM0);


                    //store w,z
                    xMOVQ_rm_r(rmIND8(rECX),rMM1) xOFS8(8)
                    //xPSWAPD_r_rm(rMM4,rmREG(rMM1))
                    //xMOVQ_rm_r(rmIND8(rECX),rMM4) xOFS8(8)

                    //stores EAX to ECX (kni does it)
                    xMOV_rm_r    (rmIND8(rECX),rEAX) xOFS8(20)

                    ///xFEMMS


                    xMOVD_r_rm(rMM0, rmIND8(rECX)) xOFS8(12)
                    xPFRCP_r_rm (rMM1,rmREG(rMM0))
                    xPUNPCKHDQ_r_rm(rMM0,rmREG(rMM0))
                    xPFRCPIT1_r_rm (rMM0,rmREG(rMM1))

                    xMOVQ_r_rm  (rMM2,rmIND(rECX));
                    xMOVQ_r_rm  (rMM3,rmIND8(rECX)); xOFS8(8)

                    xPFRCPIT2_r_rm (rMM0,rmREG(rMM1))

                    xPFMUL_r_rm(rMM2,rmREG(rMM0))
                    xPFMUL_r_rm(rMM3,rmREG(rMM0))

                    xMOVQ_rm_r(rmIND(rECX),rMM2);
                    xMOVQ_rm_r(rmIND8(rECX),rMM3) xOFS8(8)


                    //stores EAX to ECX (kni does it)
                    xMOV_rm_r    (rmIND8(rECX),rEAX) xOFS8(20)

                    xEMMS

                    }
                    */

                    xTARGET_b32 (cacheHit)

                    // instrumentation
    #ifdef INSTRUMENT_INNER_LOOPS
                    xMOV_rm_imm (rmREG(rEAX),global.kelvin.dwVertexStride) // not sizeof(CVertexBuffer::STVERTEX) since we compute effective efficiency
                    xADD_i32_r  (mMEM32(global.dwBytesCopied),rEAX)
    #endif
                }

                //
                // prepare for reject & cull
                //
                xMOV_r_i32  (rEBX,mMEM32(global.dwVertex0))
                xMOV_r_i32  (rECX,mMEM32(global.dwVertex1))
                xMOV_r_i32  (rEDX,mMEM32(global.dwVertex2))

                //
                // do trivial reject (save ebx,ecx,edx)
                //
                DWORD trivialReject = 0;

                xMOV_r_rm   (rEAX,rmIND8(rEBX)) xOFS8(16)
                xAND_r_rm   (rEAX,rmIND8(rECX)) xOFS8(16)
                xAND_r_rm   (rEAX,rmIND8(rEDX)) xOFS8(16)
                xLABEL      (trivialReject)
                xJNZ32      (0)

                //
                // do a cull check
                //

                DWORD cullCheck = 0;
                ///turn culling off temporarly, REMOVE
                //dwFlags |= KELVIN_ILFLAG_NOCULL;

                //katmai
                if ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI))
                {

                    if (!(dwFlags & KELVIN_ILFLAG_NOCULL))
                    {
                        //reads EBX,ECX,EDX

                        //0: W0 Z0 X0 Y0
                        xMOVAPS_r_rm     (rXMM0,rmIND(rEBX))
                        //1: W1 Z1 X1 Y1
                        xMOVAPS_r_rm     (rXMM1,rmIND(rECX))
                        //2: W2 Z2 X2 Y2
                        xMOVAPS_r_rm     (rXMM2,rmIND(rEDX))
                        //1: W10 Z10 Y10 X10
                        xSUBPS_r_rm      (rXMM1,rmREG(rXMM0))
                        //2: W20 Z20 Y20 X20
                        xSUBPS_r_rm      (rXMM2,rmREG(rXMM0))

                        //0: W10 Z10 Y10 X10
                        xMOVAPS_r_rm     (rXMM0,rmREG(rXMM1))
                        //3: W20 Z20 Y20 X20
                        xMOVAPS_r_rm     (rXMM3,rmREG(rXMM2))

                        //1: Y10 Y10 Y10 Y10
                        xSHUFPS_r_rm_imm (rXMM1,rmREG(rXMM1),1)
                        //2: Y20 Y20 Y20 Y20
                        xSHUFPS_r_rm_imm (rXMM3,rmREG(rXMM3),1)

                        //0: W10 Z10 Y10 X10*Y20
                        xMULSS_r_rm      (rXMM0,rmREG(rXMM3))

                        xMOV_r_rm        (rEAX,rmIND8(rEBX)) xOFS8(20)
                        xXOR_r_rm        (rEAX,rmIND8(rECX)) xOFS8(20)

                        //1: Y10 Y10 Y10 Y10*X20
                        xMULSS_r_rm      (rXMM1,rmREG(rXMM2))
                        xXOR_r_rm        (rEAX,rmIND8(rEDX)) xOFS8(20)

                        //1: Y10*Y20 - X10*X20
                        xSUBSS_r_rm      (rXMM1,rmREG(rXMM0))
                        xMOVSS_i32_r     (mMEM32(global.dwTemp),rXMM1)

                        xMOV_r_i32       (rEBX,mMEM32(global.dwTemp))
                        xXOR_r_rm        (rEBX,rmREG(rEAX))
                        xAND_rm_imm      (rmREG(rEBX),0x80000000)
                        xCMP_r_i32       (rEBX,mMEM32(global.kelvin.dwCullValue))
                        xLABEL           (cullCheck)
                        xJZ32            (0)
                    }

                }
                /*
                else
                {
                //3dnow
                if (!(dwFlags & KELVIN_ILFLAG_NOCULL))
                {

                ///xFEMMS
                xEMMS
                // Y0 | X0
                xMOVQ_r_rm     (rMM0,rmIND(rEBX))
                // Y1 | X1
                xMOVQ_r_rm     (rMM1,rmIND(rECX))
                // Y2 | X2
                xMOVQ_r_rm     (rMM2,rmIND(rEDX))
                // Y10 | X10
                xPFSUB_r_rm    (rMM1, rmREG(rMM0))
                // Y20 | X20
                xPFSUB_r_rm   (rMM2, rmREG(rMM0))
                // X10 | Y10
                xPSWAPD_r_rm (rMM1, rmREG(rMM1))
                //Y20*X10 | X20*Y10
                xPFMUL_r_rm (rMM1,rmREG(rMM2))
                // whatever | X20*Y10 - Y20*X10
                xPFNACC_r_rm (rMM1, rmREG(rMM1))

                //embed in above
                xMOV_r_rm        (rEAX,rmIND8(rEBX)) xOFS8(20)
                xXOR_r_rm        (rEAX,rmIND8(rECX)) xOFS8(20)
                xXOR_r_rm        (rEAX,rmIND8(rEDX)) xOFS8(20)


                //store that stuff
                //xMOVD_r_rm     (mMEM32(global.dwTemp),rMM1)
                xMOVD_r_rm (rEBX,rmREG(rMM1))

                //try to remove
                //xFEMMS
                xEMMS

                //this has to be after the store
                //xMOV_r_i32       (rEBX,mMEM32(global.dwTemp))
                xXOR_r_rm        (rEBX,rmREG(rEAX))
                xAND_rm_imm      (rmREG(rEBX),0x80000000)
                xCMP_r_i32       (rEBX,mMEM32(global.kelvin.dwCullValue))
                xLABEL           (cullCheck)
                xJZ32            (0)
                }
                }
                */

                //
                // dispatch
                //
                //katmai, okay
                if ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI))
                {

                    xMOVAPS_r_i128 (rXMM0,KNIMEM(dwKelvinTriDispatch))
                    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE)
                    {
                        xMOVAPS_rm_r   (rmIND(rEDI),rXMM0)
                    }
                    else
                    {
                        xMOVNTPS_rm_r  (rmIND(rEDI),rXMM0)
                    }
                    xLEA_r_rm      (rEDI,rmIND8(rEDI)) xOFS8(16)

                }
                else
                {

                    //generic, if you keep emms, replace with femms.
                    xEMMS
                    xMOVQ_r_i64(rMM1,KNIMEM(dwKelvinTriDispatch))
                    xMOVQ_r_i64(rMM2,KNIMEM(dwKelvinTriDispatch)+8)
                    xMOVNTQ_rm_r (rmIND(rEDI),rMM1)
                    xMOVNTQ_rm_r (rmIND8(rEDI),rMM2) xOFS8(8)
                    xLEA_r_rm      (rEDI,rmIND8(rEDI)) xOFS8(16)
                    xEMMS
                }
                //
                // send more triangles after this one, utilizing connectivity statistics
                //
    #if 1
                // SK -- even though the super-tri code is faster now, this is still a winning bet!
                const int count = 4; // do not exceed 128 words (about 14 of these)
                DWORD sendMore;
                xPOP_r      (rECX)
                xCMP_rm_imm (rmREG(rECX),count)
                xLABEL      (sendMore)
                xJLE32      (0)

                xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
                for (i = 0; i < count; i++)
                {
                    // prep tri
                    xLEA_r_rm   (rESI,rmIND8(rESI)) xOFS8(6)
                    xXOR_r_rm   (rEBX,rmREG(rEBX))
                    xMOV_r_rm   (rEAX,rmIND(rESI))
                    x16r xMOV_r_rm   (rBX,rmIND8(rESI)) xOFS8(4)
                    xMOV_r_rm   (rEDX,rmREG(rEAX))
                    xAND_rm_imm (rmREG(rEAX),0xffff)
                    xADD_r_i32  (rEBX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 12,rEBX)
                    xSHR_rm_imm8(rmREG(rEDX),16)
                    xADD_r_i32  (rEAX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4,rEAX)
                    xADD_r_i32  (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 8,rEDX)

                    // dispatch
                    //katmai, eek
                    if ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI))
                    {
                        xMOVAPS_r_i128 (rXMM0,KNIMEM(dwKelvinTriDispatch))
                        if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE)
                        {
                            xMOVAPS_rm_r   (rmIND(rEDI),rXMM0)
                        }
                        else
                        {
                            xMOVNTPS_rm_r  (rmIND(rEDI),rXMM0)
                        }
                        xLEA_r_rm      (rEDI,rmIND8(rEDI)) xOFS8(16)
                    }
                    else
                    {

                        ////generic
                        xEMMS
                        xMOVQ_r_i64(rMM1,KNIMEM(dwKelvinTriDispatch))
                        xMOVQ_r_i64(rMM2,KNIMEM(dwKelvinTriDispatch)+8)
                        xMOVNTQ_rm_r (rmIND(rEDI),rMM1)
                        xMOVNTQ_rm_r (rmIND8(rEDI),rMM2) xOFS8(8)
                        xLEA_r_rm      (rEDI,rmIND8(rEDI)) xOFS8(16)
                        xEMMS
                    }


                }
                xSUB_rm_imm (rmREG(rECX),count)
                xMOV_i32_r  (mMEM32(global.kelvin.pIndices),rESI)
                xTARGET_b32 (sendMore)
                xPUSH_r     (rECX)
    #endif
                // check for pusher space
                DWORD labelSpace;
                xMOV_r_i32  (rESI,mMEM32(pDriverData))
                xCMP_r_rm   (rEDI,rmIND32(rESI)) xOFS32(OFFSETOF(CDriverContext, nvPusher.m_dwThreshold))
                xLABEL      (labelSpace)
                xJL         (0)
                {
                    xMOV_rm_imm (rmREG(rEBX),(DWORD)nvKelvinDispatchGetPusherSpace)
                    xMOV_rm_r   (rmIND32(rESI),rEDI) xOFS32(OFFSETOF(CDriverContext, nvPusher.m_dwPut))
                    xCALL_rm    (rmREG(rEBX))
                    xMOV_r_rm   (rEDI,rmIND32(rESI)) xOFS32(OFFSETOF(CDriverContext, nvPusher.m_dwPut))
                }
                xTARGET_b8  (labelSpace)

                //
                // next triangle
                //
                if (trivialReject)
                { xTARGET_b32 (trivialReject)
                }
                if (cullCheck)
                { xTARGET_b32 (cullCheck)
                }
                xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
                xPOP_r      (rECX)
                xLEA_r_rm   (rESI,rmIND8(rESI)) xOFS8(6)
                xDEC_rm     (rmREG(rECX))
                xMOV_i32_r  (mMEM32(global.kelvin.pIndices),rESI)
                xJNZ32      (again)
            }

            //
            // end primitive
            //
            xMOVQ_r_i64  (rMM0,mMEM64(global.kelvin.qwEnd))
            xMOVNTQ_rm_r (rmIND(rEDI),rMM0)
            xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)
            xEMMS


            //***********************************************************************************************
            //                          Model Space Culling
            //***********************************************************************************************
        }
        else if (pContext->hwState.SuperTri.isModelCull())
        {


#ifdef SW_WC
#define NVPUSHER_NV097_ARRAY_ELELMENT32 ((((3) << 2) << 16) | ((NV_DD_KELVIN) << 13) | (NVPUSHER_NOINC(NV097_ARRAY_ELEMENT32)))
#endif
//#define NO_RAW1
//#define NO_RAW2
            //
            // start primitive
            //
            xMOVQ_r_i64  (rMM0,mMEM64(global.kelvin.qwBegin))
            xMOVNTQ_rm_r (rmIND(rEDI),rMM0)
            xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)
            xEMMS


            //
            // setup
            //
            xMOV_r_i32  (rEAX,mMEM32(global.dwVertexSN))
            xADD_rm_imm (rmREG(rEAX),0x10000)
            xMOV_i32_r  (mMEM32(global.dwVertexSN),rEAX)

            xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
            xMOV_r_i32  (rECX,mMEM32(global.kelvin.dwPrimCount))
            xLEA_r_rm   (rEAX,rmSIB) xSIB(rECX,rECX,x2)
            xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchBase),rESI)
            xLEA_r_rm   (rEAX,rmSIB) xSIB(rEAX,rEAX,x1)
            xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchMax),rEAX)

            // eye point in model space

            xMOV_r_i32 (rEAX,mMEM32(global.kelvin.dwEyeAddr))

            xMOVLPS_r_rm (rXMM7,rmIND(rEAX)) 
            xMOVHPS_r_rm (rXMM7,rmIND8(rEAX)) xOFS8(8)

#ifdef SW_WC
            // Setup SW WC pointers
            xXOR_r_rm   (rEBP, rmREG(rEBP))

#endif

            //
            // process all triangles
            //
            DWORD again;
            xLABEL  (again)
            xPUSH_r (rECX)

            //
            DWORD cullCheck = 0;
#ifdef SW_WC
            DWORD cullCheck2 = 0;
#endif
            if (!(dwFlags & KELVIN_ILFLAG_NOCULL))
            {
                // get index
                //clear EDX

#ifdef SW_WC
                xMOV_rm_imm(rmREG(rEDX), NVPUSHER_NV097_ARRAY_ELELMENT32)
                xMOV_rm_r(rmIND32(rEBP), rEDX) xOFS32(KNIMEM(dwKelvinTriDispatch))
#endif
                // Vertex 0
//#ifdef SW_WC
//        #define STMC_PREF_DIST 0x60
//#else
        #define STMC_PREF_DIST 0x40
//#endif
                xXOR_r_rm   (rEDX,rmREG(rEDX))

                xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(0*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
#ifdef SW_WC
                xMOV_rm_r(rmIND32(rEBP), rEDX) xOFS32(KNIMEM(dwKelvinTriDispatch)+1*4)
#else
#ifndef NO_RAW1
                xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + 0*4,rEDX)
#endif
#endif

                // compute vertex address into edx
                ILCCompile_mul (rEDX,pContext->hwState.SuperTri.getSTLBStride());
                xADD_r_i32     (rEDX,mMEM32(global.kelvin.pVertices))

                xMOV_r_rm   (rEAX, rmREG(rEDX))
                if ((pContext->hwState.SuperTri.getSTLBStride() & 0xf) != 0)
                {
                    xMOVLPS_r_rm (rXMM0, rmIND(rEDX))
                    xMOVHPS_r_rm (rXMM0, rmIND8(rEDX)) xOFS8(8)
                }
                else
                {
                    xMOVAPS_r_rm (rXMM0, rmIND(rEDX))
                }

                // Vertex 1
                xXOR_r_rm   (rEDX,rmREG(rEDX))

                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(1*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
#ifdef SW_WC
                xMOV_rm_r(rmIND32(rEBP), rEDX) xOFS32(KNIMEM(dwKelvinTriDispatch)+2*4)
#else
#ifndef NO_RAW1
                xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + 1*4,rEDX)
#endif
#endif
                // compute vertex address into edx
                ILCCompile_mul (rEDX,pContext->hwState.SuperTri.getSTLBStride());
                xADD_r_i32     (rEDX,mMEM32(global.kelvin.pVertices))

                xMOV_r_rm   (rEBX, rmREG(rEDX))
                if ((pContext->hwState.SuperTri.getSTLBStride() & 0xf) != 0)
                {
                    xMOVLPS_r_rm (rXMM1, rmIND(rEDX))
                    xMOVHPS_r_rm (rXMM1, rmIND8(rEDX)) xOFS8(8)
                }
                else
                {
                    xMOVAPS_r_rm (rXMM1, rmIND(rEDX))
                }

                // Vertex 2
                xXOR_r_rm   (rEDX,rmREG(rEDX))

                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(2*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
#ifdef SW_WC
                xMOV_rm_r(rmIND32(rEBP), rEDX) xOFS32(KNIMEM(dwKelvinTriDispatch)+3*4)
#else
#ifndef NO_RAW1
                xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + 2*4,rEDX)
#endif
#endif

                // compute vertex address into edx
                ILCCompile_mul (rEDX,pContext->hwState.SuperTri.getSTLBStride());
                xADD_r_i32      (rEDX,mMEM32(global.kelvin.pVertices))

                xMOV_r_rm   (rECX, rmREG(rEDX))
                if ((pContext->hwState.SuperTri.getSTLBStride() & 0xf) != 0)
                {
                    xMOVLPS_r_rm    (rXMM2, rmIND(rEDX))
                    xMOVHPS_r_rm    (rXMM2, rmIND8(rEDX)) xOFS8(8)
                }
                else
                {
                    xMOVAPS_r_rm (rXMM2, rmIND(rEDX))
                }

                // Compute eye vector

                xCMP_r_rm   (rEBX, rmREG(rEAX))

                xMOVAPS_r_rm    (rXMM6, rmREG(rXMM7))       //xmm6 = eye point
                xSUBPS_r_rm     (rXMM6, rmREG(rXMM0))       //xmm6 = eye vec

                // Compute Normal
                xCMOVA_r_rm (rEAX, rmREG(rEBX))

                xSUBPS_r_rm     (rXMM1, rmREG(rXMM0))       //v1 - v0 = v10
                xMOVAPS_r_rm    (rXMM3, rmREG(rXMM1))       //v1 - v0 = v10


                xSHUFPS_r_rm_imm (rXMM1,rmREG(rXMM1),0xC9)  //xmm1 = 1.0 v10.x v10.z v10.y

                xCMP_r_rm   (rECX, rmREG(rEAX))

                xSHUFPS_r_rm_imm (rXMM3,rmREG(rXMM3),0xD2)  //xmm3 = 1.0 v10.y v10.x v10.z


                xSUBPS_r_rm     (rXMM2, rmREG(rXMM0))       //v2 - v0 = v20
                xMOVAPS_r_rm    (rXMM4, rmREG(rXMM2))       //v2 - v0 = v20


                xSHUFPS_r_rm_imm (rXMM2, rmREG(rXMM2),0xC9) //xmm2 = 1.0 v20.x v20.z v20. y
                xCMOVA_r_rm (rEAX, rmREG(rECX))
                xSHUFPS_r_rm_imm (rXMM4, rmREG(rXMM4),0xD2) //xmm4 = 1.0 v20.y v20.x v20.z

                xMULPS_r_rm     (rXMM1, rmREG(rXMM4))
                xAND_rm_imm (rmREG(rEAX), ~0x1f)
                xMULPS_r_rm     (rXMM2, rmREG(rXMM3))

                if (pContext->hwState.SuperTri.isFixedLBStride())
                {
                    xPREFETCH_rm8(1, rmIND8(rEAX), STMC_PREF_DIST)
                }
                else
                {
                    if ((4 * pContext->hwState.SuperTri.getSTLBStride()) < 128) {
                        xPREFETCH_rm8(1, rmIND8(rEAX), 4 * pContext->hwState.SuperTri.getSTLBStride())
                    }
                    else {
                        xADD_rm_imm(rmREG(rEAX), 4 * pContext->hwState.SuperTri.getSTLBStride())
                        xPREFETCH_rm8(1, rmIND8(rEAX), 0)
                    }
                }

                xSUBPS_r_rm     (rXMM1, rmREG(rXMM2))       //xmm1 = 0.0 Z' Y' X'

                // Dot eye vector with normal

                xMULPS_r_rm     (rXMM1, rmREG(rXMM6))       // xxx n.z*v.z n.y*v.y n.x*v.x
                xMOVAPS_r_rm    (rXMM0, rmREG(rXMM1))
                xMOVAPS_r_rm    (rXMM2, rmREG(rXMM1))
                xSHUFPS_r_rm_imm (rXMM0, rmREG(rXMM0), 0x01)
                xMOV_r_i32      (rEAX,mMEM32(global.kelvin.dwCullAddr))
                xMOV_r_rm       (rEBX,rmIND(rEAX)) 
                xSHUFPS_r_rm_imm (rXMM2, rmREG(rXMM2), 0x02)
                xADDSS_r_rm     (rXMM0, rmREG(rXMM1))
                xADDSS_r_rm     (rXMM0, rmREG(rXMM2))

                // Extract sign bit from dot product

                xMOVMSKPS_r_r   (rEAX, rmREG(rXMM0))
                xXOR_r_rm       (rEAX, rmREG(rEBX))

//                 xXOR_rm_imm     (rmREG(rEAX), 0x00000001)
                xTEST_rm_imm    (rmREG(rEAX), 0x00000001)


                //reads EBX,ECX,EDX

                xLABEL           (cullCheck)
//                 xJNZ32            (0)
                xJZ32            (0)


#ifdef SW_WC
                xLEA_r_rm      (rEBP,rmIND8(rEBP)) xOFS8(16)
//                 xADD_r_rm       (rEBP, rmREG(rEBX))
#endif


            }
            else
            {
#ifdef SW_WC
                xMOV_rm_imm     (rmREG(rEDX), NVPUSHER_NV097_ARRAY_ELELMENT32)
                xMOV_rm_r       (rmIND32(rEBP), rEDX) xOFS32(KNIMEM(dwKelvinTriDispatch))

                xXOR_r_rm       (rEDX,rmREG(rEDX))

                xMOV_r_i32      (rESI,mMEM32(global.kelvin.pIndices))
                x16r xMOV_r_rm  (rDX,rmIND8(rESI)) xOFS8(0*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_rm_r       (rmIND32(rEBP), rEDX) xOFS32(KNIMEM(dwKelvinTriDispatch)+1*4)

                x16r xMOV_r_rm  (rDX,rmIND8(rESI)) xOFS8(1*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_rm_r       (rmIND32(rEBP), rEDX) xOFS32(KNIMEM(dwKelvinTriDispatch)+2*4)

                x16r xMOV_r_rm  (rDX,rmIND8(rESI)) xOFS8(2*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_rm_r       (rmIND32(rEBP), rEDX) xOFS32(KNIMEM(dwKelvinTriDispatch)+3*4)
                xLEA_r_rm       (rEBP,rmIND8(rEBP)) xOFS8(16)
#else
#ifndef NO_RAW1
                xXOR_r_rm   (rEDX,rmREG(rEDX))

                xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(0*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + 0*4,rEDX)

                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(1*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + 1*4,rEDX)

                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(2*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4 + 2*4,rEDX)
#endif
#endif
            }

            //
            // dispatch
            //
            //katmai, okay
#ifdef SW_WC
            xCMP_rm_imm     (rmREG(rEBP), SW_WC_BYTES)
            xLABEL          (cullCheck2)
            xJNZ32          (0)


            DWORD labelFlush0;
            xMOV_rm_imm (rmREG(rESI), KNIMEM(dwKelvinTriDispatch))
            xLABEL      (labelFlush0)
            xMOV_r_rm   (rEAX, rmIND(rESI))
            xMOV_rm_r   (rmIND(rEDI), rEAX)
            xMOV_r_rm   (rEAX, rmIND8(rESI)) xOFS8(4)
            xMOV_rm_r   (rmIND8(rEDI), rEAX) xOFS8(4)
            xMOV_r_rm   (rEAX, rmIND8(rESI)) xOFS8(8)
            xMOV_rm_r   (rmIND8(rEDI), rEAX) xOFS8(8)
            xMOV_r_rm   (rEAX, rmIND8(rESI)) xOFS8(12)
            xMOV_rm_r   (rmIND8(rEDI), rEAX) xOFS8(12)
            xLEA_r_rm   (rESI, rmIND8(rESI)) xOFS8(16)
            xLEA_r_rm   (rEDI, rmIND8(rEDI)) xOFS8(16)
            xSUB_rm_imm (rmREG(rEBP), 16)
            xJNZ        (labelFlush0)

//
//             for (int i = 0;i < SW_WC_LINES2FILL ;i++)
//             {
//                 xMOVAPS_r_i128  (rXMM0,KNIMEM(dwKelvinTriDispatch)+(i*64)+0*16)
//                 xMOVAPS_r_i128  (rXMM1,KNIMEM(dwKelvinTriDispatch)+(i*64)+1*16)
//                 xMOVAPS_r_i128  (rXMM2,KNIMEM(dwKelvinTriDispatch)+(i*64)+2*16)
//                 xMOVAPS_r_i128  (rXMM3,KNIMEM(dwKelvinTriDispatch)+(i*64)+3*16)
//                 xMOVAPS_rm_r   (rmIND32(rEDI),rXMM0) xOFS32(i*64+0*16)
//                 xMOVAPS_rm_r   (rmIND32(rEDI),rXMM1) xOFS32(i*64+1*16)
//                 xMOVAPS_rm_r   (rmIND32(rEDI),rXMM2) xOFS32(i*64+2*16)
//                 xMOVAPS_rm_r   (rmIND32(rEDI),rXMM3) xOFS32(i*64+3*16)
//
//             }
//             xLEA_r_rm      (rEDI,rmIND32(rEDI)) xOFS32(SW_WC_BYTES)
            xXOR_r_rm   (rEBP, rmREG(rEBP))


#else
#ifdef NO_RAW1

                xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
                xMOV_r_i32  (rEDX, KNIMEM(dwKelvinTriDispatch))
                xMOV_rm_r   (rmIND(rEDI), rEDX)

                xXOR_r_rm   (rEDX,rmREG(rEDX))

                x16r xMOV_r_rm   (rDX,rmIND(rESI))
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_rm_r  (rmIND8(rEDI),rEDX) xOFS8(1*4)

                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(1*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_rm_r  (rmIND8(rEDI),rEDX) xOFS8(2*4)

                x16r xMOV_r_rm   (rDX,rmIND8(rESI)) xOFS8(2*2)
                xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                xMOV_rm_r  (rmIND8(rEDI),rEDX) xOFS8(3*4)
                xLEA_r_rm      (rEDI,rmIND8(rEDI)) xOFS8(16)
#else // old code
            if ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI))
            {

                xMOVAPS_r_i128 (rXMM0,KNIMEM(dwKelvinTriDispatch))
                if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE)
                {
                    xMOVAPS_rm_r   (rmIND(rEDI),rXMM0)
                }
                else
                {
                    xMOVNTPS_rm_r  (rmIND(rEDI),rXMM0)
                }
                xLEA_r_rm      (rEDI,rmIND8(rEDI)) xOFS8(16)

            }
            else
            {

                //generic, if you keep emms, replace with femms.
                xEMMS
                xMOVQ_r_i64(rMM1,KNIMEM(dwKelvinTriDispatch))
                xMOVQ_r_i64(rMM2,KNIMEM(dwKelvinTriDispatch)+8)
                xMOVNTQ_rm_r (rmIND(rEDI),rMM1)
                xMOVNTQ_rm_r (rmIND8(rEDI),rMM2) xOFS8(8)
                xLEA_r_rm      (rEDI,rmIND8(rEDI)) xOFS8(16)
                xEMMS
            }
#endif
#endif

#ifndef SW_WC
            //
            // send more triangles after this one, utilizing connectivity statistics
            //
    #ifdef NO_RAW2
            if (!pContext->hwState.SuperTri.isCheckAll() || (dwFlags & KELVIN_ILFLAG_NOCULL))
            {
                const int count = 4; // do not exceed 128 words (about 14 of these)
                DWORD sendMore;
                xPOP_r      (rECX)
                xCMP_rm_imm (rmREG(rECX),count)
                xLABEL      (sendMore)
                xJLE32      (0)

                xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
                xMOV_r_i32  (rEDX, KNIMEM(dwKelvinTriDispatch))
                for (int i = 0; i < count; i++)
                {
                    // prep tri
                    xLEA_r_rm   (rESI,rmIND8(rESI)) xOFS8(6)
                    xMOV_rm_r   (rmIND(rEDI), rEDX)

                    xXOR_r_rm   (rEBX,rmREG(rEBX))

                    x16r xMOV_r_rm   (rBX,rmIND(rESI))
                    xADD_r_i32       (rEBX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    xMOV_rm_r  (rmIND8(rEDI),rEBX) xOFS8(1*4)

                    x16r xMOV_r_rm   (rBX,rmIND8(rESI)) xOFS8(1*2)
                    xADD_r_i32       (rEBX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    xMOV_rm_r  (rmIND8(rEDI),rEBX) xOFS8(2*4)

                    x16r xMOV_r_rm   (rBX,rmIND8(rESI)) xOFS8(2*2)
                    xADD_r_i32       (rEBX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    xMOV_rm_r  (rmIND8(rEDI),rEBX) xOFS8(3*4)
                    xLEA_r_rm      (rEDI,rmIND8(rEDI)) xOFS8(16)

                }
                xSUB_rm_imm (rmREG(rECX),count)
                xMOV_i32_r  (mMEM32(global.kelvin.pIndices),rESI)
                xTARGET_b32 (sendMore)
                xPUSH_r     (rECX)
            }
    #else
            if (!pContext->hwState.SuperTri.isCheckAll() || (dwFlags & KELVIN_ILFLAG_NOCULL))
            {
                const int count = 4; // do not exceed 128 words (about 14 of these)
                DWORD sendMore;
                xPOP_r      (rECX)
                xCMP_rm_imm (rmREG(rECX),count)
                xLABEL      (sendMore)
                xJLE32      (0)

                xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
                for (int i = 0; i < count; i++)
                {
                    // prep tri
                    xLEA_r_rm   (rESI,rmIND8(rESI)) xOFS8(6)
                    xXOR_r_rm   (rEBX,rmREG(rEBX))
                    xMOV_r_rm   (rEAX,rmIND(rESI))
                    x16r xMOV_r_rm   (rBX,rmIND8(rESI)) xOFS8(4)
                    xADD_r_i32       (rEBX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    xMOV_r_rm   (rEDX,rmREG(rEAX))
                    xAND_rm_imm (rmREG(rEAX),0xffff)
                    xADD_r_i32       (rEAX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 12,rEBX)
                    xSHR_rm_imm8(rmREG(rEDX),16)
                    xADD_r_i32       (rEDX, mMEM32(global.kelvin.dwBaseVertex))      //******** BV
                    xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 4,rEAX)
                    xMOV_i32_r  (KNIMEM(dwKelvinTriDispatch) + 8,rEDX)

                    // dispatch
                    //katmai, eek
                    if ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI))
                    {
                        xMOVAPS_r_i128 (rXMM0,KNIMEM(dwKelvinTriDispatch))
                        if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE)
                        {
                            xMOVAPS_rm_r   (rmIND(rEDI),rXMM0)
                        }
                        else
                        {
                            xMOVNTPS_rm_r  (rmIND(rEDI),rXMM0)
                        }
                        xLEA_r_rm      (rEDI,rmIND8(rEDI)) xOFS8(16)
                    }
                    else
                    {

                        ////generic
                        xEMMS
                        xMOVQ_r_i64(rMM1,KNIMEM(dwKelvinTriDispatch))
                        xMOVQ_r_i64(rMM2,KNIMEM(dwKelvinTriDispatch)+8)
                        xMOVNTQ_rm_r (rmIND(rEDI),rMM1)
                        xMOVNTQ_rm_r (rmIND8(rEDI),rMM2) xOFS8(8)
                        xLEA_r_rm      (rEDI,rmIND8(rEDI)) xOFS8(16)
                        xEMMS
                    }


                }
                xSUB_rm_imm (rmREG(rECX),count)
                xMOV_i32_r  (mMEM32(global.kelvin.pIndices),rESI)
                xTARGET_b32 (sendMore)
                xPUSH_r     (rECX)
            }
    #endif
#endif //SW_WC
            // check for pusher space
            DWORD labelSpace;
            xMOV_r_i32  (rESI,mMEM32(pDriverData))
            xCMP_r_rm   (rEDI,rmIND32(rESI)) xOFS32(OFFSETOF(CDriverContext, nvPusher.m_dwThreshold))
            xLABEL      (labelSpace)
            xJL         (0)
            {
                xMOV_rm_imm (rmREG(rEBX),(DWORD)nvKelvinDispatchGetPusherSpace)
                xMOV_rm_r   (rmIND32(rESI),rEDI) xOFS32(OFFSETOF(CDriverContext, nvPusher.m_dwPut))
                xCALL_rm    (rmREG(rEBX))
                xMOV_r_rm   (rEDI,rmIND32(rESI)) xOFS32(OFFSETOF(CDriverContext, nvPusher.m_dwPut))
            }
            xTARGET_b8  (labelSpace)

            //
            // next triangle
            //
            if (cullCheck)
            { xTARGET_b32 (cullCheck)
            }
#ifdef SW_WC
            if (cullCheck2)
            { xTARGET_b32 (cullCheck2)
            }
#endif
            xMOV_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
            xPOP_r      (rECX)
            xLEA_r_rm   (rESI,rmIND8(rESI)) xOFS8(6)
            xDEC_rm     (rmREG(rECX))
            xMOV_i32_r  (mMEM32(global.kelvin.pIndices),rESI)
            xJNZ32      (again)


#ifdef  SW_WC
            // flush out remaining triangles
            DWORD labelDone;
            DWORD labelFlush;
            xTEST_r_rm  (rEBP, rmREG(rEBP))
            xLABEL      (labelDone)
            xJZ         (0)

            xMOV_rm_imm (rmREG(rESI), KNIMEM(dwKelvinTriDispatch))
            xLABEL      (labelFlush)
            xMOV_r_rm   (rEAX, rmIND(rESI))
            xMOV_rm_r   (rmIND(rEDI), rEAX)
            xMOV_r_rm   (rEAX, rmIND8(rESI)) xOFS8(4)
            xMOV_rm_r   (rmIND8(rEDI), rEAX) xOFS8(4)
            xMOV_r_rm   (rEAX, rmIND8(rESI)) xOFS8(8)
            xMOV_rm_r   (rmIND8(rEDI), rEAX) xOFS8(8)
            xMOV_r_rm   (rEAX, rmIND8(rESI)) xOFS8(12)
            xMOV_rm_r   (rmIND8(rEDI), rEAX) xOFS8(12)
            xLEA_r_rm   (rESI, rmIND8(rESI)) xOFS8(16)
            xLEA_r_rm   (rEDI, rmIND8(rEDI)) xOFS8(16)
            xSUB_rm_imm (rmREG(rEBP), 16)
            xJNZ        (labelFlush)


            xTARGET_b8  (labelDone)
#endif


            //
            // end primitive
            //
            xMOVQ_r_i64  (rMM0,mMEM64(global.kelvin.qwEnd))
            xMOVNTQ_rm_r (rmIND(rEDI),rMM0)
            xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)
            xEMMS


        }//#endif // ~INVERSE_XFORM_CULL

        //
        // save off push buffer
        //
        xMOV_r_i32  (rEDX,mMEM32(pDriverData))
    #ifndef NV_NULL_HW_DRIVER
        xMOV_rm_r   (rmIND32(rEDX),rEDI) xOFS32(OFFSETOF(CDriverContext,nvPusher.m_dwPut))
    #endif

    #endif // USE_C_LOGIC
        //
        // clean up caching strategy
        //
        //     xMOV_r_i32  (rEAX,mMEM32(pDriverData))
        //     xMOV_rm_imm (rmREG(rEBX),rzSetAGPMTRR)
        //     xPUSH_imm   (1)
        //     xPUSH_rm    (rmIND32(rEAX)) xOFS32(OFFSETOF(GLOBALDATA,GARTPhysicalBase))
        //     xCALL_rm    (rmREG(rEBX))

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

#else //#if 0

    return 0;
#endif //#if 0

}


#endif KELVIN_ILC

#endif