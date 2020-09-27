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
*       Hadden Hoppert          20Oct99         Dx8 Stream support          *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

#include "x86.h"

//////////////////////////////////////////////////////////////////////////////
// aliases

#define ilcData     global.dwILCData
#define ilcCount    global.dwILCCount
#define ilcMax      global.dwILCMax

//=========================================================================

extern DWORD dwCelsiusMethodDispatch[11][16][2];

static int inline_renderorder[] = {
    D3DVSDE_BLENDWEIGHT,
    D3DVSDE_NORMAL,
    D3DVSDE_TEXCOORD1,
    D3DVSDE_TEXCOORD0,
    D3DVSDE_SPECULAR,
    D3DVSDE_DIFFUSE,
    D3DVSDE_POSITION
};

inline void vsCopyVertex(PVSHADERREGISTER pDest, DWORD dwSrc, DWORD dwType)
{
    switch(dwType)
    {
    case D3DVSDT_FLOAT1:
        pDest->x = ((float*)dwSrc)[0];
        pDest->y = 0.0f;
        pDest->z = 0.0f;
        pDest->w = 1.0f;
        break;
    case D3DVSDT_FLOAT2:
        pDest->x = ((float*)dwSrc)[0];
        pDest->y = ((float*)dwSrc)[1];
        pDest->z = 0.0f;
        pDest->w = 1.0f;
        break;
    case D3DVSDT_FLOAT3:
        pDest->x = ((float*)dwSrc)[0];
        pDest->y = ((float*)dwSrc)[1];
        pDest->z = ((float*)dwSrc)[2];
        pDest->w = 1.0f;
        break;
    case D3DVSDT_FLOAT4:
        pDest->x = ((float*)dwSrc)[0];
        pDest->y = ((float*)dwSrc)[1];
        pDest->z = ((float*)dwSrc)[2];
        pDest->w = ((float*)dwSrc)[3];
        break;
    case D3DVSDT_D3DCOLOR: {
        const DWORD v = *((DWORD*)dwSrc);
        pDest->x = RGBA_GETRED  (v) / 255.;
        pDest->y = RGBA_GETGREEN(v) / 255.;
        pDest->z = RGBA_GETBLUE (v) / 255.;
        pDest->w = RGBA_GETALPHA(v) / 255.;
        break;             }
    case D3DVSDT_UBYTE4:   {
        const BYTE *v = (BYTE*)dwSrc;
        pDest->x = v[0];
        pDest->y = v[1];
        pDest->z = v[2];
        pDest->w = v[3];
        break;             }
    case D3DVSDT_SHORT2:   {
        const SHORT *v = (SHORT*)dwSrc;
        pDest->x = v[0];
        pDest->y = v[1];
        pDest->z = 0.0f;
        pDest->w = 1.0f;
        break;             }
    case D3DVSDT_SHORT4:   {
        const SHORT *v = (SHORT*)dwSrc;
        pDest->x = v[0];
        pDest->y = v[1];
        pDest->z = v[2];
        pDest->w = v[3];
        break;             }
    default:
        DPF("Unknown vertex type");
        nvAssert(0);
    }
}

//---------------------------------------------------------------------------

__inline void pushPassthruCubeMapCoords (DWORD *pPusherIndex, PBYTE pVAData)
{
    // expand the three incoming coords into 4 slots
    nvPushData (*pPusherIndex, *(DWORD*)(pVAData + 0*sizeof(DWORD)));    (*pPusherIndex)++;
    nvPushData (*pPusherIndex, *(DWORD*)(pVAData + 1*sizeof(DWORD)));    (*pPusherIndex)++;
    nvPushData (*pPusherIndex, 0);                                       (*pPusherIndex)++;
    nvPushData (*pPusherIndex, *(DWORD*)(pVAData + 2*sizeof(DWORD)));    (*pPusherIndex)++;
}

//---------------------------------------------------------------------------

void pushTransformedTexCoords (DWORD *pPusherIndex, PBYTE pVAData, DWORD dwHWStage, DWORD dwInCount)
{
    PNVD3DCONTEXT pContext     = (PNVD3DCONTEXT)(global.celsius.pContext);
    DWORD         dwD3DStage   = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
    DWORD         dwXFormFlags = pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS];
    DWORD         dwOutCount   = dwXFormFlags & 0xff;
    BOOL          bProjected   = (dwXFormFlags & D3DTTFF_PROJECTED) ? TRUE : FALSE;
    D3DMATRIX    *pMatrix      = &(pContext->tssState[dwD3DStage].mTexTransformMatrix);
    D3DVALUE      dvCoordsIn[4], dvCoordsOut[4];

    // fetch incoming coords
    for (DWORD ii = 0; ii < dwInCount; ii++) {
        dvCoordsIn[ii] = *(D3DVALUE*)(pVAData + ii*sizeof(DWORD));
    }
    // tack on a 1.0
    dvCoordsIn[dwInCount] = 1.0;
    // set the rest to zero
    for (ii = dwInCount+1; ii < 4; ii++) {
        dvCoordsIn[ii] = 0;
    }
    // transform
    XformVector4 (dvCoordsOut, dvCoordsIn, pMatrix);
    // write out the real coordinates
    for (ii = 0; ii < (dwOutCount - (bProjected ? 1 : 0)); ii++) {
        nvPushData (*pPusherIndex, DWORD_FROM_FLOAT(dvCoordsOut[ii]));
        (*pPusherIndex)++;
    }
    // pad with zeros
    for (; ii < 3; ii++) {
        nvPushData (*pPusherIndex, 0);
        (*pPusherIndex)++;
    }
    // fill the last slot with either 1.0 or the projected value
    nvPushData (*pPusherIndex, bProjected ?
                               DWORD_FROM_FLOAT(dvCoordsOut[dwOutCount-1]) :
                               FP_ONE_BITS);
    (*pPusherIndex)++;
}

//---------------------------------------------------------------------------

void __cdecl nvCelsiusDumpStrPrimData (DWORD dwFlags)
{
    if (!global.celsius.dwPrimCount) return;

    PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)(global.celsius.pContext);

    CVertexShader *pVertexShader  = pContext->pCurrentVShader;

    BOOL   bIsIndexed             = (dwFlags & CELSIUS_ILMASK_LOOPTYPE) <= CELSIUS_ILFLAG_IX_DVB_TRI;
    DWORD  dwMagic                = celsiusPrimitiveToPrimitiveMagic[dwFlags & CELSIUS_ILMASK_PRIMTYPE];
    DWORD  dwVerticesPerPrim      = (dwMagic >> 8) & 0xff;
    DWORD  dwStartVerticesPerPrim = (dwMagic >> 0) & 0xff;
    DWORD  dwCount = global.celsius.dwPrimCount * dwVerticesPerPrim + dwStartVerticesPerPrim;

    WORD  *pIndices = (WORD*)global.celsius.pIndices;
    DWORD  dwVertexBufferOffset = pContext->dp2.dwVStart;

    DWORD  dwCopyAddr  [NV_CAPS_MAX_STREAMS];
    DWORD  dwCopyLength[NV_CAPS_MAX_STREAMS];
    DWORD  dwCopyStride[NV_CAPS_MAX_STREAMS];

    DWORD  dwReg, dwStreamSelector;
    DWORD  dwD3DStage, dwHWStage, dwTCIndex, dwVAIndex;

    if (pVertexShader->hasProgram()) {

        // calculate fetch addresses
        for (dwReg = 0; dwReg < NV_CAPS_MAX_STREAMS; dwReg++)
        {
            dwStreamSelector = pVertexShader->getVAStream(dwReg);
            if (dwStreamSelector != CVertexShader::VA_STREAM_NONE)
            {
                CVertexBuffer *pStream = pContext->ppDX8Streams[dwStreamSelector];
                DPF_LEVEL (NVDBG_LEVEL_VSHADER_INFO, "reg: %02d , stream: %02d, type: %d, offset %02x, src: %x, flag: %d",
                           dwReg, pVertexShader->getVAStream(dwReg), pVertexShader->getVAType(dwReg),
                           pVertexShader->getVAOffset(dwReg), pVertexShader->getVASrc(dwReg), pVertexShader->getVAFlag(dwReg));
                dwCopyAddr  [dwReg]  = pStream->getAddress();
                dwCopyStride[dwReg]  = pStream->getVertexStride();
                dwCopyLength[dwReg]  = pVertexShader->getVASize(dwReg) / sizeof(DWORD);
                dwCopyAddr  [dwReg] += dwCopyStride[dwReg] * dwVertexBufferOffset + pVertexShader->getVAOffset(dwReg);
            }
            else
            {
                dwCopyAddr[dwReg] = 0;
            }
        }

        // VShader program
        nvPushData (0, ((1 << 2) << 16) | ((NV_DD_CELSIUS) << 13) | NV056_SET_BEGIN_END4);
        nvPushData (1, celsiusBeginEndOp[dwFlags & CELSIUS_ILMASK_PRIMTYPE]);
        nvPusherAdjust (2);

        // get the vertex program
        void *pfVertexProgram = (void *)pVertexShader->m_ProgramOutput.residentProgram;

        // counter
        DWORD k = 0;

        if (bIsIndexed)
        {
            while (k < dwCount)
            {
                // load
                for (dwReg = 0; dwReg < NV_CAPS_MAX_STREAMS; dwReg++)
                {
                    if (dwCopyAddr[dwReg])
                    {
                        vsCopyVertex (&pVertexShader->m_Attrib[dwReg],
                                      dwCopyAddr[dwReg] + pIndices[k] * dwCopyStride[dwReg],
                                      pVertexShader->getVAType(dwReg));

                        DPF_LEVEL (NVDBG_LEVEL_VSHADER_IO, "reg[%2d] indx (%f, %f, %f, %f)", dwReg,
                                   pVertexShader->m_Attrib[dwReg].x, pVertexShader->m_Attrib[dwReg].y,
                                   pVertexShader->m_Attrib[dwReg].z, pVertexShader->m_Attrib[dwReg].w);
                    }
                }

                // execute
                ((void (__cdecl *)(void *))pfVertexProgram)(NULL);

                // store
                for (int ii = 8; ii >= 0; ii--)
                {
                    DWORD  dwRegWriteMask = pVertexShader->m_ParsedProgram.resultRegsWritten[ii];
                    DWORD *pResult        = (DWORD*)&(pVertexShader->m_Result[ii]);

                    // data in this stream ?
                    if (dwRegWriteMask)
                    {
                        DWORD c1 = 0;
                        DWORD c2 = 0;
                        DWORD dwMethod = dwCelsiusMethodDispatch[ii][dwRegWriteMask][0];
                        DWORD dwCount  = dwCelsiusMethodDispatch[ii][dwRegWriteMask][1];

                        nvAssert (dwMethod != 0xDEADBEEF);

                        nvPushData (0,((((dwCount) << 2) << 16) | ((NV_DD_CELSIUS) << 13) | (dwMethod)));
                        // for all the reg components
                        while (dwRegWriteMask)
                        {
                            if (dwRegWriteMask & 1)
                            {
                                c1 ++;
                                nvPushData (c1, pResult[c2]);
                            }
                            dwRegWriteMask >>= 1;
                            c2 ++;
                        }

                        float *pFloat = (float*)pResult;
                        DPF_LEVEL (NVDBG_LEVEL_VSHADER_IO, "out(%2d): %x (%f, %f, %f, %f)", ii,
                                   dwMethod, pFloat[0], pFloat[1], pFloat[2], pFloat[3]);

                        nvPusherAdjust (dwCount + 1);
                    }
                }
                k++;
            }
        }

        else {

            // non indexed
            while (k < dwCount)
            {
                // load
                for (dwReg = 0; dwReg < NV_CAPS_MAX_STREAMS; dwReg++)
                {
                    if (dwCopyAddr[dwReg])
                    {
                        vsCopyVertex (&pVertexShader->m_Attrib[dwReg],
                                      dwCopyAddr[dwReg],
                                      pVertexShader->getVAType(dwReg));

                        DPF_LEVEL (NVDBG_LEVEL_VSHADER_IO, "reg[%2d] ninx (%f, %f, %f, %f)", dwReg,
                                   pVertexShader->m_Attrib[dwReg].x, pVertexShader->m_Attrib[dwReg].y,
                                   pVertexShader->m_Attrib[dwReg].z, pVertexShader->m_Attrib[dwReg].w);

                        dwCopyAddr[dwReg] += dwCopyStride[dwReg];
                    }
                }

                // execute
                ((void (__cdecl *)(void *))pfVertexProgram)(NULL);

                // store
                for (int ii = 8; ii >= 0; ii--)
                {
                    DWORD dwRegWriteMask = pVertexShader->m_ParsedProgram.resultRegsWritten[ii];
                    DWORD *pResult       = (DWORD*)&pVertexShader->m_Result[ii];

                    // data in this stream ?
                    if (dwRegWriteMask)
                    {
                        DWORD c1 = 0;
                        DWORD c2 = 0;
                        DWORD dwMethod = dwCelsiusMethodDispatch[ii][dwRegWriteMask][0];
                        DWORD dwCount  = dwCelsiusMethodDispatch[ii][dwRegWriteMask][1];

                        nvAssert (dwMethod != 0xDEADBEEF);

                        nvPushData (0,((((dwCount) << 2) << 16) | ((NV_DD_CELSIUS) << 13) | (dwMethod)));
                        // for all the reg components
                        while (dwRegWriteMask)
                        {
                            if (dwRegWriteMask & 1)
                            {
                                c1 ++;
                                nvPushData (c1, pResult[c2]);
                            }
                            dwRegWriteMask >>= 1;
                            c2 ++;
                        }

                        float *pFloat = (float*)pResult;
                        DPF_LEVEL (NVDBG_LEVEL_VSHADER_IO, "out(%2d): %x (%f, %f, %f, %f)", ii,
                                   dwMethod, pFloat[0], pFloat[1], pFloat[2], pFloat[3]);

                        nvPusherAdjust (dwCount + 1);
                    }
                }
                k++;
            }
        }
    }

    else {

        // no vertex program. we should be here because we have multiple DMAs
        nvAssert ((NV_NUM_UNIQUE_STREAMS(pContext->dwStreamDMACount) > 1)
               || (NV_NUM_SYSTEM_STREAMS(pContext->dwStreamDMACount) > 1));

        // calculate fetch addresses
        for (dwReg = defaultInputRegMap[D3DVSDE_POSITION]; dwReg < defaultInputRegMap[D3DVSDE_TEXCOORD0]; dwReg++)
        {
            dwStreamSelector = pVertexShader->getVAStream(dwReg);
            if (dwStreamSelector != CVertexShader::VA_STREAM_NONE)
            {
                CVertexBuffer *pStream = pContext->ppDX8Streams[dwStreamSelector];
                DPF_LEVEL (NVDBG_LEVEL_VSHADER_INFO, "reg: %02d , stream: %02d, type: %d, offset %02x, src: %x, flag: %d",
                           dwReg, pVertexShader->getVAStream(dwReg), pVertexShader->getVAType(dwReg),
                           pVertexShader->getVAOffset(dwReg), pVertexShader->getVASrc(dwReg), pVertexShader->getVAFlag(dwReg));
                dwCopyAddr  [dwReg]  = pStream->getAddress();
                dwCopyStride[dwReg]  = pStream->getVertexStride();
                dwCopyLength[dwReg]  = pVertexShader->getVASize(dwReg) / sizeof(DWORD);
                dwCopyAddr  [dwReg] += dwCopyStride[dwReg] * dwVertexBufferOffset + pVertexShader->getVAOffset(dwReg);
            }
            else
            {
                dwCopyAddr[dwReg] = 0;
            }
        }
        for (dwReg = defaultInputRegMap[D3DVSDE_TEXCOORD0]; dwReg < defaultInputRegMap[D3DVSDE_TEXCOORD2]; dwReg++)
        {
            dwHWStage = dwReg - defaultInputRegMap[D3DVSDE_TEXCOORD0];
            if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(dwHWStage))
            {
                dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
                nvAssert (dwD3DStage != CELSIUS_UNUSED);
                dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> (16*dwHWStage)) & 0xffff;
                dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
                dwStreamSelector = pVertexShader->getVAStream (dwVAIndex);
                nvAssert ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (pContext->ppDX8Streams[dwStreamSelector])); // they better have given us coordinates
                CVertexBuffer *pStream = pContext->ppDX8Streams[dwStreamSelector];
                DPF_LEVEL (NVDBG_LEVEL_VSHADER_INFO, "reg: %02d , stream: %02d, type: %d, offset %02x, src: %x, flag: %d",
                           dwReg, pVertexShader->getVAStream(dwVAIndex), pVertexShader->getVAType(dwVAIndex),
                           pVertexShader->getVAOffset(dwVAIndex), pVertexShader->getVASrc(dwVAIndex), pVertexShader->getVAFlag(dwVAIndex));
                dwCopyAddr  [dwReg]  = pStream->getAddress();
                dwCopyStride[dwReg]  = pStream->getVertexStride();
                dwCopyLength[dwReg]  = pVertexShader->getVASize(dwVAIndex) / sizeof(DWORD);
                dwCopyAddr  [dwReg] += dwCopyStride[dwReg] * dwVertexBufferOffset + pVertexShader->getVAOffset(dwVAIndex);
            }
            else
            {
                dwCopyAddr[dwReg] = 0;
            }
        }

        // fixed path
        nvPushData (0,((1 << 2) << 16) | ((NV_DD_CELSIUS) << 13) | NV056_SET_BEGIN_END4);
        nvPushData (1, celsiusBeginEndOp[dwFlags & CELSIUS_ILMASK_PRIMTYPE]);
        nvPusherAdjust(2);

        DWORD dwVertexSize = pContext->hwState.dwInlineVertexStride >> 2;
        DWORD dwPushed;

        for (DWORD k=0; k < dwCount; k++)
        {
            dwPushed = 0;
            getDC()->nvPusher.makeSpace(dwVertexSize + 1);
            getDC()->nvPusher.push (dwPushed, (dwVertexSize << 18) | (NV_DD_CELSIUS << 13) | NVPUSHER_NOINC(NV056_INLINE_ARRAY(0)));
            dwPushed++;

            for (DWORD i=0; i<7; i++)
            {
                DWORD dwCurrentArray = defaultInputRegMap[inline_renderorder[i]];
                PBYTE pVAData        = (PBYTE)(dwCopyAddr[dwCurrentArray]) + ((bIsIndexed ? pIndices[k] : k) * dwCopyStride[dwCurrentArray]);

                if (dwCopyAddr[dwCurrentArray])
                {
                    BOOL  bTex = ((dwCurrentArray == defaultInputRegMap[D3DVSDE_TEXCOORD0]) ||
                                  (dwCurrentArray == defaultInputRegMap[D3DVSDE_TEXCOORD1]));
                    DWORD dwTexIndex = dwCurrentArray - defaultInputRegMap[D3DVSDE_TEXCOORD0];

                    if (bTex && (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(dwTexIndex))) {
                        pushPassthruCubeMapCoords (&dwPushed, pVAData);
                    }
                    else if (bTex && (pContext->hwState.dwStateFlags & CELSIUS_FLAG_TEXMATRIXSWFIX(dwTexIndex))) {
                        pushTransformedTexCoords (&dwPushed, pVAData, dwTexIndex, dwCopyLength[dwCurrentArray]);
                    }
                    else {
                        for (DWORD j=0; j < dwCopyLength[dwCurrentArray]; j++) {
                            nvPushData (dwPushed, *(DWORD*)(pVAData + j*sizeof(DWORD)));
                            dwPushed++;
                        }
                    }
                }
            }

            nvPusherAdjust(dwPushed);
        }

    }

    nvPushData (0,((1 << 2) << 16) | ((NV_DD_CELSIUS) << 13) | NV056_SET_BEGIN_END4);
    nvPushData (1, NV056_SET_BEGIN_END_OP_END);
    nvPusherAdjust(2);
}

//---------------------------------------------------------------------------

// must be used to handle the following cases not handled by the regular inner loops:
// 1. streams are coming from more than one context DMA
// 2. the current vertex shader includes a program

DWORD nvCelsiusILCompile_str_prim
(
    PNVD3DCONTEXT pContext,
    DWORD         dwFlags
)
{
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

        //  - pContext->hwState.dwStateFlags & CELSIUS_MASK_INNERLOOPUNIQUENESS
        xMOV_r_i32  (rEDX,mMEM32(global.celsius.pContext))
        xMOV_rm_imm (rmREG(rEBX),0x00040000)
        xMOV_r_rm   (rEAX,rmIND32(rEDX)) xOFS32(OFFSETOF(NVD3DCONTEXT,hwState.dwStateFlags))
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

#if 1 //USE_C_LOGIC
        xMOV_rm_imm (rmREG(rEAX),dwFlags)
            xPUSH_r (rEAX)
            xMOV_rm_imm (rmREG(rEAX),nvCelsiusDumpStrPrimData)
            xCALL_rm (rmREG(rEAX))
            xADD_rm_imm (rmREG(rESP),4);
#else //USE_C_LOGIC
        nvAssert(0);
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
#endif  // NVARCH >= 0x010

