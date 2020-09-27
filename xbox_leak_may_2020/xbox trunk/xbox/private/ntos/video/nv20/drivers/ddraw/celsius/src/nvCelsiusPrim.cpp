/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvCelsiusPrim.cpp                                                 *
*       Celsius primitive rendering routines.                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler        04Mar99         NV10 development            *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

//////////////////////////////////////////////////////////////////////////////
// switches
//
//#define FORCE_NULL                // force no work
//#define FORCE_FROMDVB_VERTICES    // force CPU copy of verts (may be slow if read from AGP or VID vertex buffers)

//////////////////////////////////////////////////////////////////////////////
// forward declarations
//
#ifdef FORCE_INLINE_VERTICES
void nvCelsiusDispatchIndexedPrimitiveInline    (PNVD3DCONTEXT       pContext,
                                                 WORD                wCount,
                                                 LPWORD              lpIndices,
                                                 BOOL                bLegacyStrides);
void nvCelsiusDispatchNonIndexedPrimitiveInline (PNVD3DCONTEXT       pContext,
                                                 WORD                wCount);
#endif


#ifdef PROFILE_LIST2STRIP
void profile_lists2strips(WORD PrimCount, LPWORD pIndices);
#endif

// helper function that takes a src texture and updates the destination with
// an inverted (both horizontally and vertically mirrored) copy of the source

static void nvInvertSwizzle (CTexture *pSrcText,CTexture *pDestText)
{
    BYTE *pSrc,*pDest,*pDestScrap,*pSrcScrap;
    DWORD x,y,dwSrc,dwSrc2;

    //make sure both textures have linear surfaces
    //hmm... what about mipmap case? ignore mip mapped
    //point sprites for now...
    pSrcText->updateLinearSurface();
    pSrcText->cpuLockLin(CSimpleSurface::LOCK_NORMAL);
    pDestText->updateLinearSurface();
    pDestText->cpuLockLin(CSimpleSurface::LOCK_NORMAL);
    pDestText->getSwizzled()->tagOutOfDate();
    //alrighty, both texture exist, linear copies are locked, do the bit twiddling...
    pSrc  = (BYTE *)(pSrcText->getLinear()->getfpVidMem() + (pSrcText->getHeight() * pSrcText->getPitch()) );
    pDest = (BYTE *)(pDestText->getLinear()->getfpVidMem() );
    for (y = 0; y < pSrcText->getHeight(); y++)
    {
        pSrcScrap = pSrc;
        pDestScrap = pDest;
        for (x=0; x < pSrcText->getWidth();x++)
        {
            pSrcScrap -= pSrcText->getBPP();
            switch(pSrcText->getBPP())
            {
                case 1:
                    *pDestScrap = *pSrcScrap;
                    break;
                case 2:
                    *(WORD *)pDestScrap = *(WORD *)pSrcScrap;
                    break;
                case 4:
                    *(DWORD *)pDestScrap = *(DWORD *)pSrcScrap;
                    break;
                case 8:
                    //DXT1
                    *(DWORD *)pDestScrap = *(DWORD *)pSrcScrap;
                    dwSrc = *(DWORD *)(pSrcScrap+4 );
                    *(DWORD *)(pDestScrap +4) = (
                        (dwSrc & 0xC0000000) >> 30 | (dwSrc & 0x30000000) >> 26 |
                        (dwSrc & 0x0C000000) >> 22 | (dwSrc & 0x03000000) >> 18 |
                        (dwSrc & 0x00C00000) >> 14 | (dwSrc & 0x00300000) >> 10 |
                        (dwSrc & 0x000C0000) >> 6  | (dwSrc & 0x00030000) >> 2  |
                        (dwSrc & 0x0000C000) << 2  | (dwSrc & 0x00003000) << 6  |
                        (dwSrc & 0x00000C00) << 10 | (dwSrc & 0x00000300) << 14 |
                        (dwSrc & 0x000000C0) << 18 | (dwSrc & 0x00000030) << 22 |
                        (dwSrc & 0x0000000C) << 26 | (dwSrc & 0x00000003) << 30);
                    break;
                case 16:
                    if (pSrcText->getFormat() == NV_SURFACE_FORMAT_DXT23_A8R8G8B8)
                    {
                        dwSrc = *(DWORD *)(pSrcScrap + 4);
                        *(DWORD *)(pDestScrap) = (
                          (dwSrc & 0xF0000000) >> 28 | (dwSrc & 0x0F000000) >> 20 |
                          (dwSrc & 0x00F00000) >> 12 | (dwSrc & 0x000F0000) >>  4 |
                          (dwSrc & 0x0000F000) <<  4 | (dwSrc & 0x00000F00) << 12 |
                          (dwSrc & 0x000000F0) << 20 | (dwSrc & 0x0000000F) << 28);
                        dwSrc = *(DWORD *)(pSrcScrap);
                        *(DWORD *)(pDestScrap + 4) = (
                          (dwSrc & 0xF0000000) >> 28 | (dwSrc & 0x0F000000) >> 20 |
                          (dwSrc & 0x00F00000) >> 12 | (dwSrc & 0x000F0000) >>  4 |
                          (dwSrc & 0x0000F000) <<  4 | (dwSrc & 0x00000F00) << 12 |
                          (dwSrc & 0x000000F0) << 20 | (dwSrc & 0x0000000F) << 28);
                    }
                    else
                    {
                        dwSrc = *(DWORD *)(pSrcScrap);
                        dwSrc2 = *(DWORD *)(pSrcScrap + 4);
                        *(DWORD *)(pDestScrap) = (
                        (dwSrc  & 0x0000FFFF) |
                        (dwSrc2 & 0xE0000000) >> 13 | (dwSrc2 & 0x1C000000) >> 7  |
                        (dwSrc2 & 0x03800000) >> 1  | (dwSrc2 & 0x00700000) << 5  |
                        (dwSrc2 & 0x000E0000) << 11 | (dwSrc2 & 0x0001C000) << 17);
                        *(DWORD *)(pDestScrap + 4) = (
                        (dwSrc2 & 0x0001C000) >> 15 | (dwSrc2 & 0x00003800) >> 9  |
                        (dwSrc2 & 0x00000700) >> 3  | (dwSrc2 & 0x000000E0) << 3  |
                        (dwSrc2 & 0x0000001C) << 9  | (dwSrc2 & 0x00000003) << 14 |
                        (dwSrc  & 0x80000000) >> 15 | (dwSrc  & 0x70000000) >> 11 |
                        (dwSrc  & 0x0E000000) >> 5  | (dwSrc  & 0x01C00000) << 1  |
                        (dwSrc  & 0x00380000) << 7  | (dwSrc  & 0x00070000) << 13);
                    }
                    *(DWORD *)(pDestScrap + 8) = *(DWORD *)(pSrcScrap + 8);
                    dwSrc = *(DWORD *)(pSrcScrap + 12 );
                    *(DWORD *)(pDestScrap + 12) = (
                        (dwSrc & 0xC0000000) >> 30 | (dwSrc & 0x30000000) >> 26 |
                        (dwSrc & 0x0C000000) >> 22 | (dwSrc & 0x03000000) >> 18 |
                        (dwSrc & 0x00C00000) >> 14 | (dwSrc & 0x00300000) >> 10 |
                        (dwSrc & 0x000C0000) >> 6  | (dwSrc & 0x00030000) >> 2  |
                        (dwSrc & 0x0000C000) << 2  | (dwSrc & 0x00003000) << 6  |
                        (dwSrc & 0x00000C00) << 10 | (dwSrc & 0x00000300) << 14 |
                        (dwSrc & 0x000000C0) << 18 | (dwSrc & 0x00000030) << 22 |
                        (dwSrc & 0x0000000C) << 26 | (dwSrc & 0x00000003) << 30);
                    break;
                default:
                    DPF ("unknown or unhandled texture bit depth in nvInvertSwizzle");
                    dbgD3DError();
                    pSrcText->cpuUnlockLin();
                    pDestText->cpuUnlockLin();
                    return;
                    break;
            }
            pDestScrap += pSrcText->getBPP();
        }
        pSrc -= pSrcText->getPitch();
        pDest += pSrcText->getPitch();
    }
    //unlock the textures and continue
    pSrcText->cpuUnlockLin();
    pDestText->cpuUnlockLin();
}

//helper function for point sprite texture
static void nvPreparePointSpriteTexture(PNVD3DCONTEXT pContext)
{
    //create the inverted texture

    if (((pContext->dp2.dwDP2Prim == D3DDP2OP_POINTS)  ||
         (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE] == D3DFILL_POINT) ) &&
         (pContext->dwRenderState[D3DRS_POINTSPRITEENABLE] == TRUE) &&
         (pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] ) &&
         ((DWORD)pContext->pCelsiusPointTexture != pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP]) )
    {
        if (pContext->dwPointHandle != pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP])
        {
            //we have to recreate the point texture
            CNvObject *pSrcObj = (CNvObject *)(pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP]);
            CTexture  *pBaseTexture = pSrcObj->getTexture();
            CNvObject *pNvObj   = new CNvObject (0);
            CNvObject *pCurrObj,*pNextObj;
            CTexture  *pPointTexture = new CTexture;
            CTexture  *pNextTexture;
            DWORD     dwMipCount;

            //check to see if the current streams/rendertarget/Z/textures need to be unlocked
            if(pContext->dwHWUnlockAllPending){ //HMH
                nvHWUnlockTextures (pContext);
                // need to mark all streams as in use by hardware...
                nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
                pContext->dwHWUnlockAllPending=FALSE;
            }

            if(pContext->pCelsiusPointTexture)
            {
                //destroy the old texture;
                pCurrObj=pContext->pCelsiusPointTexture;
                pNextObj=pCurrObj->getAttachedA();
                for (dwMipCount=1;dwMipCount < pContext->pCelsiusPointTexture->getTexture()->getMipMapCount();dwMipCount++ ) //destroy each mip map
                {
                    pCurrObj=pNextObj;
                    pNextObj=pCurrObj->getAttachedA();
                    pCurrObj->getTexture()->destroy();
                    pCurrObj->release();
                }
                pContext->pCelsiusPointTexture->getTexture()->destroy();
                pContext->pCelsiusPointTexture->release(); //call destructor
            }
            if (!pNvObj || !pPointTexture || !pBaseTexture) {
                //something went horribly wrong
                if (pNvObj) pNvObj->release();
                if (pPointTexture ) pPointTexture->destroy();
            }
            else
            {
#ifdef WINNT
                pNvObj->setDDSLcl(NULL);
#endif
                pNvObj->setObject (CNvObject::NVOBJ_TEXTURE, pPointTexture);
                pPointTexture->create (pNvObj,
                                       pBaseTexture->getWidth(),
                                       pBaseTexture->getHeight(),
                                       pBaseTexture->getDepth(),
                                       pBaseTexture->getBPP(),
                                       pBaseTexture->getBPP(),
                                       pBaseTexture->getMipMapCount(),
                                       pBaseTexture->getFormat(),
                                       pBaseTexture->getSwizzled()->getHeapLocation(),
                                       pBaseTexture->getSwizzled()->getHeapLocation());
                pPointTexture->setFourCC (pBaseTexture->getFourCC());
                //although this looks stupid, we have to do it for reset the control words
                pPointTexture->calculateNV056ControlWords();
                nvInvertSwizzle (pBaseTexture,pPointTexture);
                pCurrObj = pNvObj;
                for (dwMipCount=1;dwMipCount<pBaseTexture->getMipMapCount();dwMipCount++)
                {
                    pNextObj = new CNvObject(0);
                    pNextTexture = new CTexture;
                    pCurrObj->setAttachedA(pNextObj);
#ifdef WINNT
                    pNextObj->setDDSLcl(NULL);
#endif
                    pNextObj->setObject (CNvObject::NVOBJ_TEXTURE, pNextTexture);

                    pSrcObj = pSrcObj->getAttachedA();

                    if (!pNextObj || !pNextTexture) {
                        //something went horribly wrong
                        if (pNextObj) pNextObj->release();
                        if (pNextTexture ) pNextTexture->destroy();
                    }
                    else
                    {
                        pNextTexture->create(pNextObj,0,pPointTexture,dwMipCount);
                        pNextTexture->setFourCC (pBaseTexture->getFourCC());
                        pNextTexture->calculateNV056ControlWords();
                        nvInvertSwizzle(pSrcObj->getTexture(),pNextTexture);
                    }
                    pCurrObj = pCurrObj->getAttachedA();
                }
                pContext->pCelsiusPointTexture = pNvObj;
            }
            pContext->dwPointHandle = pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP];
        }
        //check to see if the current streams/rendertarget/Z/textures need to be unlocked
        if(pContext->dwHWUnlockAllPending){ //HMH
            nvHWUnlockTextures (pContext);
            // need to mark all streams as in use by hardware...
            nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
            pContext->dwHWUnlockAllPending=FALSE;
        }

        pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] = (DWORD)pContext->pCelsiusPointTexture;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TEXTURE_STATE |
                                          CELSIUS_DIRTY_TEXTURE_TRANSFORM |
                                          CELSIUS_DIRTY_COMBINERS;
    }
    if( (((pContext->dp2.dwDP2Prim != D3DDP2OP_POINTS) &&
          (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE] != D3DFILL_POINT)) ||
        (pContext->dwRenderState[D3DRS_POINTSPRITEENABLE] != TRUE)) &&
        ( pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP]) &&
    pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] == (DWORD)pContext->pCelsiusPointTexture)
    {
        //check to see if the current streams/rendertarget/Z/textures need to be unlocked
        if(pContext->dwHWUnlockAllPending){ //HMH
            nvHWUnlockTextures (pContext);
            // need to mark all streams as in use by hardware...
            nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
            pContext->dwHWUnlockAllPending=FALSE;
        }

        //oops, set the real texture
        pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] = pContext->dwPointHandle;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TEXTURE_STATE |
                                          CELSIUS_DIRTY_TEXTURE_TRANSFORM |
                                          CELSIUS_DIRTY_COMBINERS;
    }
}


/*****************************************************************************
 *****************************************************************************
 *** nvCelsiusDispatchIndexedPrimitive ****************************************
 *****************************************************************************
 *****************************************************************************/

void nvCelsiusDispatchIndexedPrimitive
(
    NV_INNERLOOP_ARGLIST
)
{
    BOOL bLegacyStrides;

#ifdef FORCE_NULL
    return;
#endif

    // any work?
    if (dwCount == 0) return;

    // we must at least have position data
    if (!pContext->ppDX8Streams[pContext->pCurrentVShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])]) {
        DPF ("caught attempt to render without a position stream");
        nvAssert (0);
        // return;
    }

#ifdef INSTRUMENT_INNER_LOOPS
    CNVTimer sw;
    global.dwBytesCopied = 0;
    sw.start (0);
#endif

#ifdef PROFILE_LIST2STRIP
    profile_lists2strips(dwCount, pIndices);
#endif //PROFILE_LISTS2STRIPS

    bLegacyStrides = (pContext->dp2.dwDP2Prim == D3DDP2OP_INDEXEDTRIANGLELIST) ? TRUE : FALSE;

    // book keeping
    if (pContext->pZetaBuffer) {
        pContext->pZetaBuffer->getWrapper()->setCTDrawPrim();
    }

#if SYSVB2AGP
    DWORD bAltVBPerfStrategy = ((pContext->dwEarlyCopyStrategy == 6) || (pContext->dwEarlyCopyStrategy == 7) || (pContext->dwEarlyCopyStrategy > 0x20));

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
    if (pContext->hwState.dwVertexOffset != pContext->dp2.dwVertexBufferOffset) {
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
    }

    //this is code to fix the upside down point sprites on NV10/NV11/NV15
    //It is NOT fast/efficient/optimized
    //It will perform well assuming that there is a single small texture used for points
    //if the point texture changes this code will thrash horribly.
    //This is pretty much strictly to pass WHQL since asymetrical point sprites are
    //basically never seen in real world apps
    //
    //HMH
    nvPreparePointSpriteTexture(pContext);

    // lock surfaces for use by the HW
    nvHWLockTextures (pContext);
    nvHWLockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);

    // set celsius state (required to get the correct inner loop)
    nvSetCelsiusState (pContext);

#ifdef FORCE_INLINE_VERTICES

    nvCelsiusDispatchIndexedPrimitiveInline (pContext, dwCount, pIndices, bLegacyStrides);

#else // !FORCE_INLINE_VERTICES

    // get inner loop entry point flags
    DWORD dwFlags = (pContext->dp2.dwDP2Prim & CELSIUS_ILMASK_PRIMTYPE)
                  | (bLegacyStrides ? CELSIUS_ILFLAG_LEGACY : 0);

    if (pContext->hwState.dwInlineVertexStride) {

        // set up inner loops to inline vertices
        if (pContext->hwState.pVertexShader->hasProgram()
            || (NV_NUM_UNIQUE_STREAMS(pContext->dwStreamDMACount) > 1)
            || (NV_NUM_SYSTEM_STREAMS(pContext->dwStreamDMACount) > 1)) {
            dwFlags |= CELSIUS_ILFLAG_IX_STR_PRIM;
        }
        else if ((dwCount >= CELSIUS_SHORT_INDEXED_PRIM_COUNT_CUTOFF) &&
                 ((celsiusPrimitiveToPrimitiveMagic[pContext->dp2.dwDP2Prim] & 0x00ffffff) == 0x00030300))
        {
            if (NV_VERTEX_TRANSFORMED(pContext->hwState.pVertexShader)) {
                dwFlags |= CELSIUS_ILFLAG_IX_INL_TRI_LIST;
            }
            else {
                dwFlags |= CELSIUS_ILFLAG_IX_INL_PRIM;
            }
            dwFlags                    |= ((pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_NONE) ? CELSIUS_ILFLAG_NOCULL : 0);
            global.celsius.dwCullValue  =  (pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
        }
        else {
            dwFlags |= CELSIUS_ILFLAG_IX_INL_PRIM;
        }
    }

    else {
        if (pContext->hwState.SuperTri.Strategy (pContext->dp2.dwDP2Prim, dwCount, pContext->hwState.pVertexShader))
        {
            dwFlags &= ~CELSIUS_ILMASK_LOOPTYPE;
            dwFlags |= CELSIUS_ILFLAG_SUPER_TRI_LIST;
        }
        else
        {
            dwFlags |= CELSIUS_ILFLAG_IX_VB_PRIM;
        }
    }


#ifdef LPC_OPT
    static DWORD savedFlags=0;
    if ((savedFlags == dwFlags) && (pDriverData->dwLastDP2 == pDriverData->dwThisDP2))
    {
        global.celsius.dwPrimCount           = dwCount;
        global.celsius.pIndices              = pContext->dp2.dwIndices;
        global.celsius.dwIndexStride         = pContext->dp2.dwIndexStride;

        // BUGBUG this won't work with vertex shaders
        // pVertices is used in the inline functions and needs the base vertex (dwVStart) built in.
        // pVertices is not used in the VB routines - they use dwBaseVertex
        global.celsius.pVertices             = pContext->hwState.ppStreams[0]->getAddress() +
            pContext->hwState.dwVertexOffset +
            pContext->dp2.dwVStart * pContext->hwState.ppStreams[0]->getVertexStride() +
            pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]);
        //set base vertex for use in VB Prim and super tri routines
        global.celsius.dwBaseVertex          = pContext->dp2.dwVStart;
        global.celsius.dwEyeAddr             = pContext->hwState.SuperTri.getEyeAddr();
        global.celsius.dwCullAddr            = pContext->hwState.SuperTri.getCullsignAddr();
    }
    else
#endif
    {
        // setup variables passed to innerloop
        global.celsius.pContext              = (DWORD)pContext;
        global.celsius.pContext_hwState      = (DWORD)&pContext->hwState;
        global.celsius.dwPrimCount           = dwCount;

        global.celsius.dwVertexStride        = pContext->hwState.pVertexShader->getStride();  // vertex stride of source data
        global.celsius.pIndices              = pContext->dp2.dwIndices;
        global.celsius.dwIndexStride         = pContext->dp2.dwIndexStride;
        // BUGBUG this won't work with vertex shaders
        // pVertices is used in the inline functions and needs the base vertex (dwVStart) built in.
        // pVertices is not used in the VB routines - they use dwBaseVertex
        global.celsius.pVertices             = pContext->hwState.ppStreams[pContext->hwState.pVertexShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])]->getAddress() +
                                               pContext->hwState.dwVertexOffset +
                                               pContext->dp2.dwVStart * pContext->hwState.ppStreams[pContext->hwState.pVertexShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])]->getVertexStride() +
                                               pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]);
        //set base vertex for use in VB Prim and super tri routines
        global.celsius.dwBaseVertex          = pContext->dp2.dwVStart;

        // set component offsets
        global.celsius.dwOffsetXYZ           = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]);
        global.celsius.dwOffsetRHW           = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]) + 3*sizeof(DWORD);
        global.celsius.dwOffsetDiffuse       = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_DIFFUSE]);
        global.celsius.dwOffsetSpecular      = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_SPECULAR]);
        global.celsius.dwOffsetNormal        = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_NORMAL]);
        global.celsius.dwOffsetWeight        = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_BLENDWEIGHT]);
        for (DWORD i=0; i<CELSIUS_CAPS_MAX_UV_PAIRS; i++) {
            global.celsius.dwOffsetUV[i]     = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_TEXCOORD0+i]);
        }

        global.celsius.dwEyeAddr             = pContext->hwState.SuperTri.getEyeAddr();
        global.celsius.dwCullAddr            = pContext->hwState.SuperTri.getCullsignAddr();


    }

    if ((dwFlags & CELSIUS_ILMASK_LOOPTYPE) == CELSIUS_ILFLAG_SUPER_TRI_LIST)
    {
        //
        // do preprocessing
        //
        dwFlags                    |= ((pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_NONE) ? CELSIUS_ILFLAG_NOCULL : 0);
        global.celsius.dwCullValue  = (pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
        CVertexBuffer *pVertexBufferInUse = pContext->ppDX8Streams[pContext->hwState.pVertexShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])];
        global.celsius.pVertices             = (DWORD) pVertexBufferInUse->getSuperTriLookAsideBuffer() +
                                               pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]);

    }


    // go
    CELSIUSDISPATCHPRIMITIVE pfn = nvCelsiusGetDispatchRoutine(pContext,dwFlags);
    if (pfn) pfn();

    // restore important values
    nvPusherAdjust (0);

#ifdef LPC_OPT
    savedFlags = dwFlags;
#endif

#endif // !FORCE_INLINE_VERTICES

    //set that we need to unlock all surfaces.  defer till we need to lower number of CPU
    //clocks per primitive.
    pContext->dwHWUnlockAllPending = TRUE;

#ifdef INSTRUMENT_INNER_LOOPS
    sw.stop (0);
    PF ("Prim=%d, VPS=%f",dwFlags & 0xffff,float(global.dwBytesCopied)/sw.getTime(0));
#endif

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
}

/*****************************************************************************
 *****************************************************************************
 *** nvCelsiusDispatchNonIndexedPrimitive ************************************
 *****************************************************************************
 *****************************************************************************/
void nvCelsiusDispatchNonIndexedPrimitive
(
    NV_INNERLOOP_ARGLIST
)
{
#ifdef FORCE_NULL
    return;
#endif

    // any work?
    if (dwCount == 0) return;

    // we must at least have position data
    if (!pContext->ppDX8Streams[pContext->pCurrentVShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])]) {
        DPF ("caught attempt to render without a position stream");
        nvAssert (0);
        return;
    }

#ifdef INSTRUMENT_INNER_LOOPS
    CNVTimer sw;
    global.dwBytesCopied = 0;
    sw.start (0);
#endif

    // book keeping
    if (pContext->pZetaBuffer) {
        pContext->pZetaBuffer->getWrapper()->setCTDrawPrim();
    }

    // set the vertex sources and if anything has changed, dirty the fvf flag
    if (pContext->hwState.dwVertexOffset != pContext->dp2.dwVertexBufferOffset) {
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
    }

    //this is code to fix the upside down point sprites on NV10/NV11/NV15
    //It is NOT fast/efficient/optimized
    //It will perform well assuming that there is a single small texture used for points
    //if the point texture changes this code will thrash horribly.
    //This is pretty much strictly to pass WHQL since asymetrical point sprites are
    //basically never seen in real world apps
    //
    //HMH
    nvPreparePointSpriteTexture(pContext);

    // lock surfaces for use by the HW
    nvHWLockTextures (pContext);
    nvHWLockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);

    // set celsius state (required to get the correct inner loop)
    nvSetCelsiusState (pContext);

#ifdef FORCE_INLINE_VERTICES

    nvCelsiusDispatchNonIndexedPrimitiveInline (pContext, dwCount);

#else // !FORCE_INLINE_VERTICES

    // get inner loop entry point flags
    DWORD dwFlags = (pContext->dp2.dwDP2Prim & CELSIUS_ILMASK_PRIMTYPE)
                  | ((pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_NONE) ? CELSIUS_ILFLAG_NOCULL : 0);


    if (pContext->hwState.dwInlineVertexStride ) {

        // set up inner loops to inline vertices
        if (pContext->hwState.pVertexShader->hasProgram()
            || (NV_NUM_UNIQUE_STREAMS(pContext->dwStreamDMACount) > 1)
            || (NV_NUM_SYSTEM_STREAMS(pContext->dwStreamDMACount) > 1)) {
            dwFlags |= CELSIUS_ILFLAG_OR_STR_PRIM;
        }
        else {
            if ((dwCount >= CELSIUS_SHORT_ORDERED_PRIM_COUNT_CUTOFF)
             && (NV_VERTEX_TRANSFORMED(pContext->hwState.pVertexShader))
             && ((celsiusPrimitiveToPrimitiveMagic[pContext->dp2.dwDP2Prim] & 0x00ffffff) == 0x00030300)) {
                dwFlags |= CELSIUS_ILFLAG_OR_INL_TRI_LIST
                        |  ((pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_NONE) ? CELSIUS_ILFLAG_NOCULL : 0);
                global.celsius.dwCullValue  = (pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
            }
            else {
                dwFlags |= CELSIUS_ILFLAG_OR_INL_PRIM;
            }
        }

    }

    else {

#ifdef LPC_OPT
        //hw can pull verts directly from vid mem
        if (pContext->hwState.ppStreams[0]->getContextDMA() == NV_CONTEXT_DMA_VID && dwCount < 40)
        {
            nvAssert (celsiusBeginEndOp[pContext->dp2.dwDP2Prim] != ~0);
            nvglSetNv10CelsiusBeginEnd (NV_DD_CELSIUS, celsiusBeginEndOp[pContext->dp2.dwDP2Prim]);

            DWORD dwScaleAndBias = celsiusPrimitiveToPrimitiveMagic[pContext->dp2.dwDP2Prim];
            nvAssert (dwScaleAndBias != ~0);
            DWORD dwNumVertices = (((dwScaleAndBias >> 8) & 0xff) * dwCount) + (dwScaleAndBias & 0xff);

            DWORD dwIndex = 0;
            if (dwNumVertices) {
                nvglSetNv10CelsiusDrawArrays (NV_DD_CELSIUS,
                    DRF_NUM(056, _DRAW_ARRAYS, _COUNT, dwNumVertices-1) |
                    DRF_NUM(056, _DRAW_ARRAYS, _START_INDEX, dwIndex + pContext->dp2.dwVStart));
            }

            nvglSetNv10CelsiusBeginEnd (NV_DD_CELSIUS, NV056_SET_BEGIN_END_OP_END);

            return;
        }
#endif
        // set up inner loops to pull vertices
        dwFlags |= CELSIUS_ILFLAG_OR_VB_PRIM;

    }

#ifdef LPC_OPT
    static DWORD savedFlags=0;
    if ((savedFlags == dwFlags) && (pDriverData->dwLastDP2 == pDriverData->dwThisDP2))
    {
        global.celsius.dwPrimCount           = dwCount;
        global.celsius.pIndices              = pContext->dp2.dwIndices;
        global.celsius.dwBaseVertex          = pContext->dp2.dwVStart;
        global.celsius.pVertices             =
            pContext->hwState.ppStreams[0]->getAddress() +
            pContext->hwState.dwVertexOffset +
            pContext->dp2.dwVStart * pContext->hwState.ppStreams[0]->getVertexStride() +
            pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]);


    }
    else
#endif
    {
        // setup variables passed to innerloop
        global.celsius.pContext              = (DWORD)pContext;
        global.celsius.pContext_hwState      = (DWORD)&pContext->hwState;
        global.celsius.dwPrimCount           = dwCount;
        global.celsius.dwVertexStride        = pContext->hwState.pVertexShader->getStride();  // vertex stride of source data
        global.celsius.pIndices              = pContext->dp2.dwIndices;
        global.celsius.dwIndexStride         = pContext->dp2.dwIndexStride;
        global.celsius.pVertices             = pContext->hwState.ppStreams[pContext->hwState.pVertexShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])]->getAddress() +
                                               pContext->hwState.dwVertexOffset +
                                               pContext->dp2.dwVStart * pContext->hwState.ppStreams[pContext->hwState.pVertexShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])]->getVertexStride() +
                                               pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]);
        //set base vertex
        global.celsius.dwBaseVertex          = pContext->dp2.dwVStart;
        // set component offsets
        global.celsius.dwOffsetXYZ           = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]);
        global.celsius.dwOffsetRHW           = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]) + 3*sizeof(DWORD);
        global.celsius.dwOffsetDiffuse       = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_DIFFUSE]);
        global.celsius.dwOffsetSpecular      = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_SPECULAR]);
        global.celsius.dwOffsetNormal        = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_NORMAL]);
        global.celsius.dwOffsetWeight        = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_BLENDWEIGHT]);
        for (DWORD i = 0; i < 7; i++) {
            global.celsius.dwOffsetUV[i]     = pContext->hwState.pVertexShader->getVAOffset(defaultInputRegMap[D3DVSDE_TEXCOORD0+i]);
        }

    }

    // go
    CELSIUSDISPATCHPRIMITIVE pfn = nvCelsiusGetDispatchRoutine(pContext,dwFlags);
    if (pfn) pfn();

    // restore pusher
    nvPusherAdjust (0);

#ifdef LPC_OPT
    savedFlags = dwFlags;
#endif

#endif // !FORCE_INLINE_VERTICES
    //set that we need to unlock all surfaces.  defer till we need to to lower number of CPU
    //clocks per primitive.
    pContext->dwHWUnlockAllPending = TRUE;
#ifdef INSTRUMENT_INNER_LOOPS
    sw.stop (0);
    PF ("Prim=%d, VPS=%f",dwFlags & 0xffff,float(global.dwBytesCopied)/sw.getTime(0));
#endif

}

/*****************************************************************************
 *****************************************************************************
 *** nvCelsiusDispatchLegacyWireframePrimitive *******************************
 *****************************************************************************
 *****************************************************************************/

void nvCelsiusDispatchLegacyWireframePrimitive
(
    NV_INNERLOOP_ARGLIST
)
{
    // this routine handles DX5 primitives in wireframe mode, which may
    // have edge flags. we don't really care much about being efficient here.

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
                    nvCelsiusDispatchIndexedPrimitive (pContext, 1);
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
            nvCelsiusDispatchIndexedPrimitive (pContext, 1);
            dbgFlushType (NVDBG_FLUSH_PRIMITIVE);
        }
        dwEdgeMask <<= 1;

        // we may draw edge v1-v2 for any or all triangles
        for (wTriNum=0; wTriNum<dwCount; wTriNum++) {
            wIndicesOut[0] = wTriNum+1;
            wIndicesOut[1] = wTriNum+2;
            if (dwEdgeFlags & dwEdgeMask) {
                nvCelsiusDispatchIndexedPrimitive (pContext, 1);
                dbgFlushType (NVDBG_FLUSH_PRIMITIVE);
            }
            dwEdgeMask <<= 1;
        }

        // we never draw edge v2-v0 for anything but the last triangle
        wIndicesOut[0] = dwCount+1;
        wIndicesOut[1] = 0;
        if (dwEdgeFlags & dwEdgeMask) {
            nvCelsiusDispatchIndexedPrimitive (pContext, 1);
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

#ifdef FORCE_INLINE_VERTICES

// this code is typically unused. it's used only for debugging
// purposes if FORCE_INLINE_VERTICES is defined

//---------------------------------------------------------------------------

// table mapping dp2ops to the scale and bias required to calculate the number
// of vertices from the number of primitives. the scale is in the upper half
// of the word and the bias is in the lower.
// numVertices = scale * numPrimitives + bias

DWORD celsiusPrimitiveToVertexCountScaleAndBias[D3D_DP2OP_MAX+1] =
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

// dumps one FVF vertex into the push buffer with the components in the order
// celsius requires. note that adequate push buffer space is assumed to have
// been verified already!

void nvCelsiusDumpInlineVertex
(
    PNVD3DCONTEXT pContext,
    DWORD         dwIndex
)
{
    CVertexShader  *pShader;
    CVertexBuffer **ppStreams;
    DWORD           dwStreamSelector;
    DWORD           dwVertexBufferOffset;
    DWORD           dwBaseVertex;
    D3DTLVERTEX     vertex;
    DWORD           pComponent;
    DWORD           i, j, n;
    DWORD           dwD3DStage, dwTCIndex, dwVAIndex;

    pShader              = pContext->hwState.pVertexShader;
    ppStreams            = pContext->hwState.ppStreams;
    dwVertexBufferOffset = pContext->hwState.dwVertexOffset;
    dwBaseVertex         = pContext->dp2.dwVStart;
    j=0;
    memset (&vertex, 0, sizeof(D3DTLVERTEX));

    // weights
    dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_BLENDWEIGHT]);
    if ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])) {
        pComponent = ppStreams[dwStreamSelector]->getAddress()                      // address of buffer
                   + dwVertexBufferOffset                                           // global offset within buffer
                   + dwBaseVertex * ppStreams[dwStreamSelector]->getVertexStride()
                   + pShader->getVAOffset (defaultInputRegMap[D3DVSDE_BLENDWEIGHT]) // offset of this component
                   + dwIndex*ppStreams[dwStreamSelector]->getVertexStride();        // offset of this particular vertex
        for (i=0; i<1; i++) { nvglSetData (j, ((LPDWORD)pComponent)[i]); j++; }
    }

    // normal
    dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_NORMAL]);
    if ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])) {
        pComponent = ppStreams[dwStreamSelector]->getAddress()                      // address of buffer
                   + dwVertexBufferOffset                                           // global offset within buffer
                   + dwBaseVertex * ppStreams[dwStreamSelector]->getVertexStride()
                   + pShader->getVAOffset (defaultInputRegMap[D3DVSDE_NORMAL])      // offset of this component
                   + dwIndex*ppStreams[dwStreamSelector]->getVertexStride();        // offset of this particular vertex
        for (i=0; i<3; i++) { nvglSetData (j, ((LPDWORD)pComponent)[i]); j++; }
    }

    // tex1 coords
    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(1)) {
        dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[1];
        nvAssert (dwD3DStage != CELSIUS_UNUSED);
        dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 16) & 0xffff;
        dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
        dwStreamSelector = pShader->getVAStream (dwVAIndex);
        nvAssert ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])); // they better have given us coordinates
        pComponent = ppStreams[dwStreamSelector]->getAddress()                      // address of buffer
                   + dwVertexBufferOffset                                           // global offset within buffer
                   + dwBaseVertex * ppStreams[dwStreamSelector]->getVertexStride()
                   + pShader->getVAOffset (dwVAIndex)                               // offset of this component
                   + dwIndex*ppStreams[dwStreamSelector]->getVertexStride();        // offset of this particular vertex
        n = (pContext->hwState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1) | CELSIUS_FLAG_TEXMATRIXSWFIX(1))) ?
            4 : (pShader->getVASize (dwVAIndex) / sizeof(DWORD));

        for (i=0; i<n; i++) { nvglSetData (j, ((LPDWORD)pComponent)[i]); j++; }
#ifdef STOMP_TEX_COORDS
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_STOMP_4TH_COORD(1)) nvglSetData (3, 0x3f800000);
#endif  // STOMP_TEX_COORDS
    }

    // tex0 coords
    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(0)) {
        dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[0];
        nvAssert (dwD3DStage != CELSIUS_UNUSED);
        dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 0) & 0xffff;
        dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
        dwStreamSelector = pShader->getVAStream (dwVAIndex);
        nvAssert ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])); // they better have given us coordinates
        pComponent = ppStreams[dwStreamSelector]->getAddress()                      // address of buffer
                   + dwVertexBufferOffset                                           // global offset within buffer
                   + dwBaseVertex * ppStreams[dwStreamSelector]->getVertexStride()
                   + pShader->getVAOffset (dwVAIndex)                               // offset of this component
                   + dwIndex*ppStreams[dwStreamSelector]->getVertexStride();        // offset of this particular vertex
        n = (pContext->hwState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0) | CELSIUS_FLAG_TEXMATRIXSWFIX(0))) ?
            4 : (pShader->getVASize (dwVAIndex) / sizeof(DWORD));

        for (i=0; i<n; i++) { nvglSetData (j, ((LPDWORD)pComponent)[i]); j++; }
#ifdef STOMP_TEX_COORDS
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_STOMP_4TH_COORD(0)) nvglSetData (3, 0x3f800000);
#endif  // STOMP_TEX_COORDS

        vertex.tu = FLOAT_FROM_DWORD(((LPDWORD)pComponent)[0]);
        vertex.tv = FLOAT_FROM_DWORD(((LPDWORD)pComponent)[1]);
    }

    // specular
    dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_SPECULAR]);
    if ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])) {
        pComponent = ppStreams[dwStreamSelector]->getAddress()                      // address of buffer
                   + dwVertexBufferOffset                                           // global offset within buffer
                   + dwBaseVertex * ppStreams[dwStreamSelector]->getVertexStride()
                   + pShader->getVAOffset (defaultInputRegMap[D3DVSDE_SPECULAR])    // offset of this component
                   + dwIndex*ppStreams[dwStreamSelector]->getVertexStride();        // offset of this particular vertex
        for (i=0; i<1; i++) { nvglSetData (j, ((LPDWORD)pComponent)[i]); j++; }
        vertex.specular = ((LPDWORD)pComponent)[0];
    }

    // diffuse
    dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_DIFFUSE]);
    if ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector])) {
        pComponent = ppStreams[dwStreamSelector]->getAddress()                      // address of buffer
                   + dwVertexBufferOffset                                           // global offset within buffer
                   + dwBaseVertex * ppStreams[dwStreamSelector]->getVertexStride()
                   + pShader->getVAOffset (defaultInputRegMap[D3DVSDE_DIFFUSE])     // offset of this component
                   + dwIndex*ppStreams[dwStreamSelector]->getVertexStride();        // offset of this particular vertex
        for (i=0; i<1; i++) { nvglSetData (j, ((LPDWORD)pComponent)[i]); j++; }
        vertex.color = ((LPDWORD)pComponent)[0];
    }

    // x,y,z
    dwStreamSelector = pShader->getVAStream (defaultInputRegMap[D3DVSDE_POSITION]);
    nvAssert ((dwStreamSelector != CVertexShader::VA_STREAM_NONE) && (ppStreams[dwStreamSelector]));
    pComponent = ppStreams[dwStreamSelector]->getAddress()                          // address of buffer
               + dwVertexBufferOffset                                               // global offset within buffer
               + dwBaseVertex * ppStreams[dwStreamSelector]->getVertexStride()
               + pShader->getVAOffset (defaultInputRegMap[D3DVSDE_POSITION])        // offset of this component
               + dwIndex*ppStreams[dwStreamSelector]->getVertexStride();            // offset of this particular vertex
    for (i=0; i<3; i++) { nvglSetData (j, ((LPDWORD)pComponent)[i]); j++; }
    vertex.sx = FLOAT_FROM_DWORD(((LPDWORD)pComponent)[0]);
    vertex.sy = FLOAT_FROM_DWORD(((LPDWORD)pComponent)[1]);
    vertex.sz = FLOAT_FROM_DWORD(((LPDWORD)pComponent)[2]);
#ifdef CHECK_DEPTHS
    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE) {
        float fz = FLOAT_FROM_DWORD(((LPDWORD)pComponent)[2]);
        if ((fz < 0.f) || (fz > 1.f)) {
            DPF ("unbiased z=%f is out of bounds", fz);
        }
        if (pContext->dwRenderState[D3DRENDERSTATE_ZBIAS] && pContext->hwState.celsius.dwZEnable) {
            float fZBias  = -(float)(pContext->dwRenderState[D3DRENDERSTATE_ZBIAS]);
            float fZScale = 0.25f * fZBias;
            fZBias  *= pContext->hwState.dvInvZScale;
            fZScale *= pContext->hwState.dvInvZScale;
            fz = fZScale*fz + fZBias;
            if ((fz < 0.f) || (fz > 1.f)) {
                DPF ("biased z'=%f is out of bounds", fz);
            }
        }
    }
#endif  // CHECK_DEPTHS

    // w
    if (pShader->getVASize (defaultInputRegMap[D3DVSDE_POSITION]) == 4*sizeof(DWORD)) {
        pComponent += 3*sizeof(DWORD);
        for (i=0; i<1; i++) { nvglSetData (j, ((LPDWORD)pComponent)[i]); j++; }
        vertex.rhw = FLOAT_FROM_DWORD(((LPDWORD)pComponent)[0]);
#ifdef CHECK_DEPTHS
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE) {
            float frhw = FLOAT_FROM_DWORD(((LPDWORD)pComponent)[0]);
            if (frhw < 0.f) {
                DPF ("rhw=%f is out of bounds", frhw);
            }
        }
#endif  // CHECK_DEPTHS
    }

    dbgDisplayVertexData (FALSE, (DWORD*)(&vertex));

    nvAssert (j == (pContext->hwState.dwInlineVertexStride >> 2));
    getDC()->nvPusher.inc (j);
}

//---------------------------------------------------------------------------

// dumps a block of 'count' non-indexed vertices starting with index 'i0'

void nvCelsiusDumpNonIndexedInlineVertexBlock
(
    PNVD3DCONTEXT pContext,
    DWORD         i0,
    DWORD         count,
    DWORD         dwInlineVertexStride
)
{
    DWORD methodCount;

    methodCount = count * dwInlineVertexStride;  // number of 4-byte words
    assert (methodCount <= 2048);

    getDC()->nvPusher.makeSpace (sizeSetStartMethod + methodCount);
    nvglSetStartMethod (0, NV_DD_CELSIUS, NVPUSHER_NOINC(NV056_INLINE_ARRAY(0)), methodCount);
    getDC()->nvPusher.inc (sizeSetStartMethod);

    for (DWORD i=i0; i<i0+count; i++) {
        nvCelsiusDumpInlineVertex (pContext, i);
    }
}

//---------------------------------------------------------------------------

// dumps a block of 'count' indexed vertices beginning with index 'i0'

void nvCelsiusDumpIndexedInlineVertexBlock
(
    PNVD3DCONTEXT pContext,
    LPWORD        lpIndices,
    DWORD         i0,
    DWORD         count,
    BOOL          bLegacyStrides,
    DWORD         dwInlineVertexStride
)
{
    DWORD methodCount;

    methodCount = count * dwInlineVertexStride;  // number of 4-byte words
    assert (methodCount <= 2048);

    getDC()->nvPusher.makeSpace (sizeSetStartMethod + methodCount);
    nvglSetStartMethod (0, NV_DD_CELSIUS, NVPUSHER_NOINC(NV056_INLINE_ARRAY(0)), methodCount);
    getDC()->nvPusher.inc (sizeSetStartMethod);

    if (bLegacyStrides) {
        DWORD i1;
        // stupid old style indexing - {index16:index16 index16:junk16} repeat
        for (DWORD i=i0; i < (i0+count); i++) {
            i1 = 4*i / 3;  // offset of the i'th index
            nvCelsiusDumpInlineVertex (pContext, lpIndices[i1]);
        }
    }
    else {
        // new indexing - fully packed
        for (DWORD i=i0; i < (i0+count); i++) {
            nvCelsiusDumpInlineVertex (pContext, lpIndices[i]);
        }
    }
}

//---------------------------------------------------------------------------

void nvCelsiusDispatchNonIndexedPrimitiveInline
(
    PNVD3DCONTEXT       pContext,
    WORD                dwCount
)
{
    DWORD numVertices, emittedVertices, dwScaleAndBias, dwInlineVertexStride;

    dwInlineVertexStride = pContext->hwState.dwInlineVertexStride >> 2;

    nvAssert (celsiusBeginEndOp[pContext->dp2.dwDP2Prim] != ~0);
    nvglSetNv10CelsiusBeginEnd4 (NV_DD_CELSIUS, celsiusBeginEndOp[pContext->dp2.dwDP2Prim]);

    dwScaleAndBias = celsiusPrimitiveToVertexCountScaleAndBias[pContext->dp2.dwDP2Prim];
    nvAssert (dwScaleAndBias != ~0);
    numVertices = (dwScaleAndBias >> 16) * dwCount + (dwScaleAndBias & 0xffff);

    emittedVertices = 0;

    while (numVertices > CELSIUS_MAX_INLINE_VERTICES) {
        // attempt to kick off some work already
        nvPusherStart (FALSE);
        // send more vertices
        nvCelsiusDumpNonIndexedInlineVertexBlock (pContext, emittedVertices, CELSIUS_MAX_INLINE_VERTICES, dwInlineVertexStride);
        emittedVertices += CELSIUS_MAX_INLINE_VERTICES;
        numVertices -= CELSIUS_MAX_INLINE_VERTICES;
    }

    nvCelsiusDumpNonIndexedInlineVertexBlock (pContext, emittedVertices, numVertices, dwInlineVertexStride);

    nvglSetNv10CelsiusBeginEnd4 (NV_DD_CELSIUS, NV056_SET_BEGIN_END4_OP_END);

    // attempt to kick off some work already
    nvPusherStart (FALSE);
}

//---------------------------------------------------------------------------

void nvCelsiusDispatchIndexedPrimitiveInline
(
    PNVD3DCONTEXT       pContext,
    DWORD               dwCount,
    LPWORD              lpIndices,
    BOOL                bLegacyStrides
)
{
    DWORD numVertices, emittedVertices, dwScaleAndBias, dwInlineVertexStride;

    dwInlineVertexStride = pContext->hwState.dwInlineVertexStride >> 2;

    // tell the hardware what kind of thing we're going to draw
    nvAssert (celsiusBeginEndOp[pContext->dp2.dwDP2Prim] != ~0);
    nvglSetNv10CelsiusBeginEnd4 (NV_DD_CELSIUS, celsiusBeginEndOp[pContext->dp2.dwDP2Prim]);

    // determine the number of vertices
    dwScaleAndBias = celsiusPrimitiveToVertexCountScaleAndBias[pContext->dp2.dwDP2Prim];
    nvAssert (dwScaleAndBias != ~0);
    numVertices = (dwScaleAndBias >> 16) * dwCount + (dwScaleAndBias & 0xffff);

    emittedVertices = 0;

    while (numVertices > CELSIUS_MAX_INLINE_VERTICES) {
        // attempt to kick off some work already
        nvPusherStart (FALSE);
        // send more vertices
        nvCelsiusDumpIndexedInlineVertexBlock (pContext, lpIndices, emittedVertices,
                                               CELSIUS_MAX_INLINE_VERTICES, bLegacyStrides, dwInlineVertexStride);
        emittedVertices += CELSIUS_MAX_INLINE_VERTICES;
        numVertices -= CELSIUS_MAX_INLINE_VERTICES;
    }

    nvCelsiusDumpIndexedInlineVertexBlock (pContext, lpIndices, emittedVertices,
                                           numVertices, bLegacyStrides, dwInlineVertexStride);

    nvglSetNv10CelsiusBeginEnd4 (NV_DD_CELSIUS, NV056_SET_BEGIN_END4_OP_END);

    nvPusherStart (FALSE);
}

#endif  // FORCE_INLINE_VERTICES

#endif  // NVARCH == 0x10


#ifdef PROFILE_LIST2STRIP

#define A1A2_B1C2 1     //New Vtx = B2
#define A1A2_C1B2 2     //New Vtx = C2
#define A1B2_B1A2 3     //New Vtx = C2
#define A1B2_C1C2 4     //New Vtx = A2
#define A1C2_B1B2 5     //New Vtx = A2
#define A1C2_C1A2 6     //New Vtx = B2
#define B1A2_C1C2 7     //New Vtx = B2
#define B1B2_C1A2 8     //New Vtx = C2
#define B1C2_C1B2 9     //New Vtx = A2

#define A1 0
#define B1 1
#define C1 2
#define A2 3
#define B2 4
#define C2 5

static DWORD listmatch[10] =
{
    0,0,0,0,0,0,0,0,0,0
};

typedef struct _strip_length_stats
{
    DWORD len2;     //length 2-4
    DWORD len5;     //length 5-9
    DWORD len10;    //length 10-19
    DWORD len20;    //length 20-49
    DWORD len50;    //length 50-99
    DWORD len100;   //length 100+
    DWORD count;    //total count;
} STRIP_STATS;

#define MAX_PAT_LEN 16
typedef struct _strip_length_pat
{
  BYTE pattern[MAX_PAT_LEN];
  DWORD pat_len;
  STRIP_STATS stats;
} STRIP_PATTERN;

#define MAX_PATS 200
static STRIP_PATTERN strip_pats[MAX_PATS+2];
static DWORD num_pats = 0;
static DWORD num_tris = 0;
static DWORD num_tris_in_strips = 0;
static STRIP_STATS stripstats;
static DWORD first_time = 1;

bool match(BYTE *s1, BYTE *s2, DWORD len)
{
    for (DWORD i = 0; i < len; i++)
    {
        if (*s1++ != *s2++)
            return false;
    }
    return true;
}

DWORD pat_len(BYTE *src, DWORD len)
{
    BYTE *pat;
    BYTE *subpat;
    for (DWORD length = 2; length < len/2 ;length++ )
    {
        pat = subpat = src;
        for (DWORD i = 0; i < len/length; i++ )
        {
            if (!match(pat, subpat+i*length, length)) goto next_len;
        }
        return length;
next_len:;

    }
    return len;
}

STRIP_PATTERN *get_smallest()
{
    DWORD i;
    DWORD count = 0xffffffff;
    STRIP_PATTERN *p = strip_pats;
    STRIP_PATTERN *low = 0;
    for (i = 0; i < MAX_PATS; i++,p++)
    {
        if (p->stats.count < count)
        {
            low = p;
            count = p->stats.count;

        }
    }
    return low;

}


DWORD match_pair(LPWORD pIndices)
{
        if (pIndices[A1] == pIndices[A2])
        {
            if (pIndices[B1] == pIndices[C2])
                return A1A2_B1C2;
            else if (pIndices[C1] == pIndices[B2])
                return A1A2_C1B2;
            // else no match
        }
        else if (pIndices[A1] == pIndices[B2])
        {
            if (pIndices[B1] == pIndices[A2])
                return A1B2_B1A2;
            else if (pIndices[C1] == pIndices[C2])
                return A1B2_C1C2;
            // else no match

        }
        else if (pIndices[A1] == pIndices[C2])
        {
            if (pIndices[B1] == pIndices[B2])
                return A1C2_B1B2;
            else if (pIndices[C1] == pIndices[A2])
                return A1C2_C1A2;
            // else no match
        }
        else if (pIndices[B1] == pIndices[A2])
        {
            if (pIndices[C1] == pIndices[C2])
                return B1A2_C1C2;
            // else no match
        }
        else if (pIndices[B1] == pIndices[B2])
        {
            if (pIndices[C1] == pIndices[A2])
                return B1B2_C1A2;
            // else no match
        }
        else if (pIndices[B1] == pIndices[C2])
        {
            if (pIndices[C1] == pIndices[B2])
                return B1C2_C1B2;
            // else no match
        }

        return 0;
}

void init_strip_stats()
{
    int i, j;
    num_pats = 0;
    for (i = 0; i < MAX_PATS ; i++)
    {
        for (j = 0; j < MAX_PAT_LEN; j++ )
        {
            strip_pats[i].pattern[j] = 0;
        }
        strip_pats[i].pat_len = 0;
        strip_pats[i].stats.len2 = 0;
        strip_pats[i].stats.len5 = 0;
        strip_pats[i].stats.len10 = 0;
        strip_pats[i].stats.len20 = 0;
        strip_pats[i].stats.len50 = 0;
        strip_pats[i].stats.len100 = 0;
        strip_pats[i].stats.count = 0;
    }
    first_time = 0;
    stripstats.len2 = 0;
    stripstats.len5 = 0;
    stripstats.len10 = 0;
    stripstats.len20 = 0;
    stripstats.len50 = 0;
    stripstats.len100 = 0;
    stripstats.count = 0;
    num_tris = 0;
    num_tris_in_strips = 0;
}

void strip_stats(STRIP_PATTERN *pat, DWORD len)
{
    pat->stats.count++;
    stripstats.count++;

    if (len >= 2 && len < 5)
    {
        pat->stats.len2++;
        stripstats.len2++;
    }
    else if (len >= 5 && len < 10)
    {
        pat->stats.len5++;
        stripstats.len5++;
    }
    else if (len >= 10 && len < 20)
    {
        pat->stats.len10++;
        stripstats.len10++;
    }
    else if (len >= 20 && len < 50)
    {
        pat->stats.len20++;
        stripstats.len20++;
    }
    else if (len >= 50 && len < 100)
    {
        pat->stats.len50++;
        stripstats.len50++;
    }
    else if (len >= 100)
    {
        pat->stats.len100++;
        stripstats.len100++;
    }
}

void add_strip(BYTE *strip, DWORD patlen, DWORD striplen)
{
    STRIP_PATTERN *p;
    p = strip_pats;
    DWORD i;
    if (num_pats == 0)
    {
        for (i = 0; i < patlen; i++)
        {
            p->pattern[i] = strip[i];
        }
        p->pat_len = patlen;
        strip_stats(p, striplen);
        num_pats++;
    }
    else
    {
        for (i = 0; i < num_pats ; i++)
        {
            if (patlen == p->pat_len)
            {
                if (match(p->pattern, strip, patlen))
                {
                    strip_stats(p, striplen);
                    goto done;
                }
            }
            p++;
        }

        if (num_pats < MAX_PATS)
        {
            num_pats++;
            for (i = 0; i < patlen; i++)
            {
                p->pattern[i] = strip[i];
            }
            p->pat_len = patlen;
            strip_stats(p, striplen);

        }
        else
        {
            p = get_smallest();
            if (p)
            {

                for (i = 0; i < patlen; i++)
                {
                    p->pattern[i] = strip[i];
                }
                p->pat_len = patlen;
                p->stats.len2 = 0;
                p->stats.len5 = 0;
                p->stats.len10 = 0;
                p->stats.len20 = 0;
                p->stats.len50 = 0;
                p->stats.len100 = 0;
                p->stats.count = 0;
                strip_stats(p, striplen);
            }
        }
    }
done: ;
}


void profile_lists2strips(WORD PrimCount, LPWORD pIndices)
{
    bool inStrip = false;
    int striplen = 0;
    int patlen = 0;
    BYTE strip[MAX_PAT_LEN];
    if (first_time)
        init_strip_stats();

    for (int i = 0; i < PrimCount-1; i++)
    {
        num_tris++;
        DWORD p = match_pair(pIndices);
        if (p)
        {
            num_tris_in_strips++;
            listmatch[p]++;
            if (!inStrip)
            {
                inStrip = true;
                striplen = 1;
                strip[0] = p;
            }
            else
            {
                if (striplen < MAX_PAT_LEN)
                    strip[striplen] = p;
                striplen++;
            }

        }
        else
        {
            if (inStrip)
            {
                patlen = pat_len(strip, (striplen<MAX_PAT_LEN) ? striplen : MAX_PAT_LEN);
                add_strip(strip, patlen, striplen);
            }
            inStrip = false;
            striplen = 0;
        }

        pIndices += 3;
    }

    if (inStrip)
    {
        patlen = pat_len(strip, (striplen<MAX_PAT_LEN) ? striplen : MAX_PAT_LEN);
        add_strip(strip, patlen, striplen);
    }
}

#endif // PROFILE_LIST2STRIP

