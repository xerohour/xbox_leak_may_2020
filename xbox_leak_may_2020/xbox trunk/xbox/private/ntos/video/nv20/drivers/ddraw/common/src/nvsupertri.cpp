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
//  Module: nvSuperTri.h
//      SuperTri state header file
//
// **************************************************************************
//
//  History:
//      Scott Kephart           08Nov00             Kelvin Development
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

bool ST_Strat_Orig(CSuperTri *st, DWORD dwDP2Operation, DWORD PrimCount, CVertexBuffer *pVertexBufferInUse)
{
    BOOL bDoSuperTri = false;
    BOOL bModelCull = false;
    BOOL bBandWidth = false;
    DWORD STflags = 0;
    DWORD nFrames = CURRENT_FLIP - COMPLETED_FLIP - 1;
    PNVD3DCONTEXT pContext = st->pContext;

    if (dwDP2Operation == D3DDP2OP_INDEXEDTRIANGLELIST2)
    {

        if (nFrames == 3)
        {
            bBandWidth = true;

            if (pContext->dwEarlyCopyStrategy == 6)
            {
                st->dwSuperTriThrottle++;
                st->dwSuperTriThrottle &= st->dwSuperTriMask;
                bDoSuperTri = !st->dwSuperTriThrottle;
            }
            else
            {
                if (!pContext->dwRenderState[D3DRENDERSTATE_CLIPPING])
                    bModelCull = true;
                bDoSuperTri = true;

            }
        }
        else if (nFrames == 2)
        {
            if (pContext->dwEarlyCopyStrategy == 6)
            {
                st->dwSuperTriThrottle++;
                st->dwSuperTriThrottle &= st->dwSuperTriMask;
                bDoSuperTri = !st->dwSuperTriThrottle;
            }
            else
            {
                bDoSuperTri = true;
                bModelCull = true;
            }

        }
        else if (nFrames == 1)
        {
            if (pContext->dwEarlyCopyStrategy != 6)
            {
                st->dwSuperTriThrottle++;
                st->dwSuperTriThrottle &= st->dwSuperTriMask;
                bDoSuperTri = !st->dwSuperTriThrottle;
                bModelCull = true;
            }
        }

        // SK - need to make this more realistic, temporary hack
        if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE)
        {
            if ((pContext->dwEarlyCopyStrategy != 0) && (pContext->dwEarlyCopyStrategy < 3))
            {
                bModelCull = true;
                bBandWidth = true;
            }
        }

        if (st->isSuperTri())
            STflags = pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_ST_MASK;

        if (!st->isModelCullOK())
        {
            bModelCull = false;
            STflags &= ~PS_ST_MODEL;
        }

    //***********************************************************
    // SuperTri test code -- uncomment the following to debug the super-tri code
    //          Always uncomment the following two lines
    //          Setting STflags = 0 or bDoSuperTri to false will cause super tri code to NEVER be executed.
    //        STflags = PS_ST_MASK;
//            bDoSuperTri = true;
    //
    //          This causes every triangle to pass thru the culling test. By default only 1 in 4 is cull-checked.
//         bBandWidth = true;
    //          Set bModelCull to FALSE to force transform and clip check culling
    //          Set bModelCull to TRUE to force model space culling
//         bModelCull = true;
    //***********************************************************

        if (STflags
         && (PrimCount >= 8)
         && (dwDP2Operation == D3DDP2OP_INDEXEDTRIANGLELIST2)
         && (!NV_VERTEX_TRANSFORMED(pContext->hwState.pVertexShader))
         && (!GET_VERTEXBLEND_STATE(pContext))
         && pVertexBufferInUse->getSuperTriLookAsideBuffer()
         && bDoSuperTri)
        {
            if (pVertexBufferInUse == getDC()->defaultVB.getVB())
                st->setVarLBStride();
            else
                st->setFixedLBStride();

            if (bBandWidth)
                st->setCheckAll();
            else
                st->clrCheckAll();

            if (bModelCull)
                st->setModelCull();
            else
                st->setXformCull();

            st->setSTLBStride();
            return true;

        }
    }

    return false;

}

bool ST_Strat_P3(CSuperTri *st, DWORD dwDP2Operation, DWORD PrimCount, CVertexShader *pVertexShader)
{
    BOOL bDoSuperTri = false;
    BOOL bModelCull = false;
    BOOL bBandWidth = false;
    DWORD STflags = 0;
    DWORD nFrames = CURRENT_FLIP - COMPLETED_FLIP;
    PNVD3DCONTEXT pContext = st->pContext;
    CVertexBuffer *pVertexBufferInUse;

    // no super tri for vertex shader programs
    if (pVertexShader->hasProgram())
        return FALSE;

    pVertexBufferInUse = pContext->ppDX8Streams[pContext->hwState.pVertexShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])];

    // check for invalid stream
    if (!pVertexBufferInUse)
        return FALSE;

    if (dwDP2Operation == D3DDP2OP_INDEXEDTRIANGLELIST2)
    {

        if (nFrames == 3)
            bBandWidth = true;
        if (nFrames >= 2)
        {
            bModelCull = true;
            if (pContext->dwEarlyCopyStrategy == 6)
            {
                st->dwSuperTriThrottle++;
                st->dwSuperTriThrottle &= st->dwSuperTriMask;
                bDoSuperTri = !st->dwSuperTriThrottle;
            }
            else            
                bDoSuperTri = true;
        }
        
        // SK - need to make this more realistic, temporary hack
        if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE)
        {
            if ((pContext->dwEarlyCopyStrategy != 0) && (pContext->dwEarlyCopyStrategy < 3))
            {
                bModelCull = true;
                bBandWidth = true;
            }
        }

        if (st->isSuperTri())
            STflags = pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_ST_MASK;

        if (!st->isModelCullOK())
        {
            bModelCull = false;
            STflags &= ~PS_ST_MODEL;
        }

    //***********************************************************
    // SuperTri test code -- uncomment the following to debug the super-tri code
    //          Always uncomment the following two lines
    //          Setting STflags = 0 or bDoSuperTri to false will cause super tri code to NEVER be executed.
    //        STflags = PS_ST_MASK;
//            bDoSuperTri = true;
    //
    //          This causes every triangle to pass thru the culling test. By default only 1 in 4 is cull-checked.
//         bBandWidth = true;
    //          Set bModelCull to FALSE to force transform and clip check culling
    //          Set bModelCull to TRUE to force model space culling
//         bModelCull = true;
    //***********************************************************


        st->clrCullILFlags();

        if (STflags
         && (PrimCount >= 8)
         && (dwDP2Operation == D3DDP2OP_INDEXEDTRIANGLELIST2)
         && (!NV_VERTEX_TRANSFORMED(pContext->hwState.pVertexShader))
         && (!GET_VERTEXBLEND_STATE(pContext))
         && pVertexBufferInUse->getSuperTriLookAsideBuffer()
         && bDoSuperTri)
        {
            if (pVertexBufferInUse == getDC()->defaultVB.getVB())
                st->setVarLBStride();
            else
                st->setFixedLBStride();

            if (bBandWidth)
                st->setCheckAll();
            else
                st->clrCheckAll();

            if (bModelCull)
                st->setModelCull();
            else
                st->setXformCull();

            st->setSTLBStride();
            return true;

        }
    }
    return false;
}

bool ST_Strat_P4(CSuperTri *st, DWORD dwDP2Operation, DWORD PrimCount, CVertexShader *pVertexShader)
{
    BOOL bDoSuperTri = false;
    BOOL bModelCull = false;
    BOOL bBandWidth = false;
    DWORD STflags = 0;
    DWORD nFrames = CURRENT_FLIP - COMPLETED_FLIP - 1;
    PNVD3DCONTEXT pContext = st->pContext;
    CVertexBuffer *pVertexBufferInUse;

    // no super tri for vertex shader programs
    if (pVertexShader->hasProgram())
        return FALSE;

    pVertexBufferInUse = pContext->ppDX8Streams[pContext->hwState.pVertexShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])];

    // check for invalid stream
    if (!pVertexBufferInUse)
        return FALSE;

    if (dwDP2Operation == D3DDP2OP_INDEXEDTRIANGLELIST2)
    {

        if (nFrames >= 1)
        {
            bBandWidth = true;
            bModelCull = true;
            bDoSuperTri = true;

        }

        if (!st->isModelCullOK())
        {
            bModelCull = false;
        }

    //***********************************************************
    // SuperTri test code -- uncomment the following to debug the super-tri code
    //          Always uncomment the following two lines
    //          Setting STflags = 0 or bDoSuperTri to false will cause super tri code to NEVER be executed.
    //        STflags = PS_ST_MASK;
//            bDoSuperTri = true;
    //
    //          This causes every triangle to pass thru the culling test. By default only 1 in 4 is cull-checked.
//         bBandWidth = true;
    //          Set bModelCull to FALSE to force transform and clip check culling
    //          Set bModelCull to TRUE to force model space culling
//         bModelCull = true;
    //***********************************************************
        st->clrCullILFlags();

        if ((PrimCount >= 8)
         && (dwDP2Operation == D3DDP2OP_INDEXEDTRIANGLELIST2)
         && (!NV_VERTEX_TRANSFORMED(pContext->hwState.pVertexShader))
         && (!GET_VERTEXBLEND_STATE(pContext))
         && pVertexBufferInUse->getSuperTriLookAsideBuffer()
         && bDoSuperTri)
        {
            if (pVertexBufferInUse == getDC()->defaultVB.getVB())
                st->setVarLBStride();
            else
                st->setFixedLBStride();

            if (pContext->dwEarlyCopyStrategy != 6 && bBandWidth)
                st->setCheckAll();
            else
                st->clrCheckAll();

            if (bModelCull)
                st->setModelCull();
            else
                st->setXformCull();

            st->setSTLBStride();
            return true;

        }
    }

    return false;

}

bool ST_Strat_Null(CSuperTri *st, DWORD dwDP2Operation, DWORD PrimCount, CVertexShader *pVertexShader)
{
    return false;
}

void CSuperTri::SuperTriInit(PNVD3DCONTEXT Context)
{
    dwSuperTriPendingFlips = 0;
    pContext = Context;
    dwSuperTriThrottle = 0;
    dwSuperTriMask = 0;
    dwNumLights = 0;
    cullsign = 0;
    eye.dvX = 0.0;
    eye.dvY = 0.0;
    eye.dvZ = 0.0;
    eye.dvW = 0.0;
    dwCullFlags = 0;
    dwSTILFlags = 0;
    dwSTLBStride = 0;

    if ((pContext->dwDXAppVersion >= 0x700) && (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_SUPERTRI))
    {
        setSuperTri();
        setModelCullOK();
    }
    pContext->hwState.SuperTri.setStrategy();
}

void CSuperTri::setSTLBStride() {
    dwSTLBStride = (dwSTILFlags & NV_STILFLAG_STDEFVB) ? pContext->hwState.pVertexShader->getStride() : sizeof(CVertexBuffer::STVERTEX);
}

void CSuperTri::setEye(D3DMATRIX *MV)
{

    static D3DVECTOR4 eye_in_eye = {0.0f, 0.0f, 0.0f, 1.0f};
    static D3DVECTOR4 p0 = {0.5f, 0.5f, 0.5f, 1.0f };
    static D3DVECTOR4 p1 = {0.75f, 0.25f, 0.25f, 1.0f };
    static D3DVECTOR4 p2 = {0.75f, 0.75f, 0.25f, 1.0f };
    D3DVECTOR4 norm, mp1, mp2, mp0, eyevec, v1, v2;
    float cw2;
    D3DMATRIX InvMV;
    DWORD cullconv = (pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_CCW) ? 0x1 : 0x0 ;
    DWORD cull;

    if (isSuperTri() && isModelCullOK())
    {
        // Invert the eye vector to model space
        //
        // Instead of culling in screen space, which requires that each vertex be transformed,
        // we'll cull in model space. To accomplish this, we inverse transform the eye point from it's '
        // fixed position in eye-space (it's the origin) back to model space by the inverse of the
        // model-view matrix. (inverse of the view matrix takes us back to world space.) Inverse of
        // the world matrix takes us back to model space.

        DWORD ret = STInverse4x4 (&InvMV, MV);
        {

            static float TOL = 0.001f;
            static D3DMATRIX mTest;
            MatrixProduct4x4 (&mTest, MV, &InvMV);
            if ((ret == -1)                     ||
                (fabs(fabs(mTest._11) - 1.0) > TOL) ||
                ((fabs(mTest._12) - 0.0) > TOL) ||
                ((fabs(mTest._13) - 0.0) > TOL) ||
                ((fabs(mTest._21) - 0.0) > TOL) ||
                (fabs(fabs(mTest._22) - 1.0) > TOL) ||
                ((fabs(mTest._23) - 0.0) > TOL) ||
                ((fabs(mTest._31) - 0.0) > TOL) ||
                ((fabs(mTest._32) - 0.0) > TOL) ||
                (fabs(fabs(mTest._33) - 1.0) > TOL)) {
#ifdef DEBUG
                    DPF ("matrix inversion failed");
#endif
                    // It's really, really hard to always detect non-invertable matrices. So if we ever
                    // find one,  parties over.
                    clrModelCullOK();
                }
        }

        XformVector4((float *)&eye, (float *)&eye_in_eye, &InvMV);

        // Discover what culling convention is being used.
        //
        // There's a complication with this scheme, however, and it's significant. D3D defines
        // D3DCULL_CW and D3DCULL_CCW. These are defined in screen space. This convention is *not*
        // always the same transformed back to model space! The World Matrix can contain rotations
        // that essentially reverse the handedness of our coordinate system. The following
        // takes a triangle that faces towards the eye in camera space, and inverse transforms
        // it to model space. It then performs a culling calculation with the eye, and checks to
        // see if it's still front facing. If not, then the transformation matrices reverse the sense
        // of culling and so we note this.

        XformVector4((float *) &mp1, (float *) &p1, &InvMV);
        XformVector4((float *) &mp2, (float *) &p2, &InvMV);
        XformVector4((float *) &mp0, (float *) &p0, &InvMV);
        // form vectors
        v1.dvX = mp1.dvX - mp0.dvX;
        v1.dvY = mp1.dvY - mp0.dvY;
        v1.dvZ = mp1.dvZ - mp0.dvZ;
        v2.dvX = mp2.dvX - mp0.dvX;
        v2.dvY = mp2.dvY - mp0.dvY;
        v2.dvZ = mp2.dvZ - mp0.dvZ;
        // create eyevector
        eyevec.dvX =  eye.dvX - mp0.dvX;
        eyevec.dvY =  eye.dvY - mp0.dvY;
        eyevec.dvZ =  eye.dvZ - mp0.dvZ;
        // create a normal in model space
        norm.dvX = v1.dvY*v2.dvZ - v1.dvZ*v2.dvY;
        norm.dvY = v1.dvZ*v2.dvX - v1.dvX*v2.dvZ;
        norm.dvZ = v1.dvX*v2.dvY - v1.dvY*v2.dvX;
        // cull
        cw2 = eyevec.dvX * norm.dvX + eyevec.dvY * norm.dvY + eyevec.dvZ * norm.dvZ;
        cull = (cw2 < 0.0f) ? 0 : 1;
        setCullsign(cull ^ cullconv);

    }
}


void CSuperTri::setStrategy()
{
    dwSuperTriThrottle = 0;
    if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_CT_11M)
    {
        if ((pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_1196_CELSIUS) && (pContext->dwEarlyCopyStrategy == 0x32))
            dwSuperTriMask = 1;
        else
            dwSuperTriMask = 0;
    
    }
    else
        dwSuperTriMask = 0;

    pStrategy = ST_Strat_Null;

    if (isSuperTri())
    {
        if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
        {
            pStrategy = ST_Strat_P3;

        }

        if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE)
        {
            pStrategy = ST_Strat_P4;

        }

        if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)
        {
            pStrategy = ST_Strat_P3;

        }

    }
}

#endif