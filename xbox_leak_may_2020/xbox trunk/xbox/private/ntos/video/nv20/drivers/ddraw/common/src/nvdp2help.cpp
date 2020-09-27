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
//  Module: nvDP2Help.cpp
//      helper routines for DP2 operations
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        08Nov00         created
//
// **************************************************************************

#include "nvprecomp.h"
#pragma hdrstop

//-------------------------------------------------------------------------

void __stdcall nvWireframeIndexedTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    dbgTracePush ("nvWireframeIndexedTriangle (%d,%x)", pContext->dp2.dwDP2Prim, dwCount);

    DDSTARTTICK(PRIM_WIREFRAMEINDEXEDTRI);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, dwCount);

    if ((!pContext->bUseDX6Class) && (pContext->pCurrentVShader->getFVF() == D3DFVF_TLVERTEX)) {
        nvIndexedWireframeTriangle (NV_INNERLOOP_ARGS);
    }
    else {
        nvFVFIndexedWireframeTriangle (NV_INNERLOOP_ARGS);
    }
    DDENDTICK(PRIM_WIREFRAMEINDEXEDTRI);

    dbgTracePop();
}

//-------------------------------------------------------------------------

void __stdcall nvPointIndexedTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    dbgTracePush ("nvPointIndexedTriangle (%d,%x)", pContext->dp2.dwDP2Prim, dwCount);

    DDSTARTTICK(PRIM_POINTINDEXEDTRI);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, dwCount);

    if ((!pContext->bUseDX6Class) && (pContext->pCurrentVShader->getFVF() == D3DFVF_TLVERTEX)) {
        nvIndexedPointTriangle (NV_INNERLOOP_ARGS);
    }
    else {
        nvFVFIndexedPointTriangle (NV_INNERLOOP_ARGS);
    }
    DDENDTICK(PRIM_POINTINDEXEDTRI);

    dbgTracePop();
}

//-------------------------------------------------------------------------

void __stdcall nvWireframeNonIndexedTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    dbgTracePush ("nvWireframeNonIndexedTriangle (%d,%x)", pContext->dp2.dwDP2Prim, dwCount);

    DDSTARTTICK(PRIM_WIREFRAMENONINDEXEDTRI);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, dwCount);

    if ((!pContext->bUseDX6Class) && (pContext->pCurrentVShader->getFVF() == D3DFVF_TLVERTEX)) {
        nvDrawWireframeTriangle (NV_INNERLOOP_ARGS);
    }
    else {
        nvFVFDrawWireframeTriangle (NV_INNERLOOP_ARGS);
    }
    DDENDTICK(PRIM_WIREFRAMENONINDEXEDTRI);

    dbgTracePop();
}

//-------------------------------------------------------------------------

void __stdcall nvPointNonIndexedTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    dbgTracePush ("nvPointNonIndexedTriangle (%d,%x)", pContext->dp2.dwDP2Prim, dwCount);

    DDSTARTTICK(PRIM_POINTNONINDEXEDTRI);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, dwCount);

    if ((!pContext->bUseDX6Class) && (pContext->pCurrentVShader->getFVF() == D3DFVF_TLVERTEX)) {
        nvDrawPointTriangle (NV_INNERLOOP_ARGS);
    }
    else {
        nvFVFDrawPointTriangle (NV_INNERLOOP_ARGS);
    }
    DDENDTICK(PRIM_POINTNONINDEXEDTRI);

    dbgTracePop();
}

//-------------------------------------------------------------------------------------

void nvCalcRWFar (PNVD3DCONTEXT pContext)
{
    if ((getDC()->nvD3DRegistryData.regW16Format == D3D_REG_WFORMAT_FIXED) ||
        (getDC()->nvD3DRegistryData.regW32Format == D3D_REG_WFORMAT_FIXED)) {
        // For fixed point W-Buffers, always set dvRWFar to the specified WFar value.
        pContext->surfaceViewport.dvRWFar = pContext->surfaceViewport.dvWFar;
    }
    else {
        // For floating point W-Buffers, dvRWFar gets calculated slightly differently
        // for 16-bit buffers vs 24 bit buffers.
        if (pContext->pZetaBuffer) {
            if (pContext->pZetaBuffer->getBPP() == 2) {
                // 16 Bit W-Buffer.
                pContext->surfaceViewport.dvRWFar = pContext->surfaceViewport.dvWFar * (1.0f / 256.0f);
            }
            else {
                // 24 Bit W-Buffer.
                pContext->surfaceViewport.dvRWFar = 1.0f;
                // pContext->surfaceViewport.dvRWFar = 1.0f / (2 ** 127); // alternate
            }
        }
        else {
            // we don't have a render target yet
            pContext->surfaceViewport.dvRWFar = 1.0f;
        }
    }
}

//---------------------------------------------------------------------------

BOOL nvPaletteBlt (CNvObject *pSrcObj, CTexture *pDstTexture)
{
    CNvObject *pOrigSrcObj = pSrcObj;
    // get palette address and 64 byte align
    DWORD      pPalAddr    = (pDstTexture->getSwizzled()->getAddress() + pDstTexture->getSwizzled()->getSize() / 2 + 63) & ~63;

    dbgTracePush ("nvPaletteBlt");

    // determine the new texture format and create palette builder object
    CPaletteBuilder palBuilder;

    DWORD dwNewFormat, dwOriginalFormat = pDstTexture->getLinear()->getFormat();
    DWORD dwPalType;

    switch(dwOriginalFormat)
    {
        case NV_SURFACE_FORMAT_A8R8G8B8:
            dwPalType   = PT_8888;
            dwNewFormat = NV_SURFACE_FORMAT_I8_A8R8G8B8;
            break;
        case NV_SURFACE_FORMAT_X8R8G8B8:
            dwPalType   = PT_X888;
            dwNewFormat = NV_SURFACE_FORMAT_I8_A8R8G8B8;
            break;
        default:
            // unhandled texture format
            DPF_LEVEL(NVDBG_LEVEL_PALETTE, "nvPaletteBlt: Unhandled autopalettization texture format");
            dbgTracePop();
            return FALSE;
    }

    BOOL bRVal = palBuilder.initTable(dwPalType);
    if (!bRVal) {
        palBuilder.freeTable(dwPalType);
        dbgTracePop();
        return FALSE;
    }

    // pass 1 - extract the palette

    do {
        // get source surface
        CSimpleSurface* pSrcSurface = pSrcObj->getSimpleSurface();

        bRVal = palBuilder.buildTable(dwPalType,
                                      pSrcSurface->getAddress(),
                                      pSrcSurface->getWidth(), pSrcSurface->getHeight());
        if (!bRVal) {
            palBuilder.freeTable(dwPalType);
            dbgTracePop();
            return FALSE;
        }

        pSrcObj = pSrcObj->getAttachedA();

    } while (pSrcObj); // terminate if we have reached the end of the mipmap chain

    // lock the destination surface
    pDstTexture->cpuLockSwz (CSimpleSurface::LOCK_NORMAL);

    // optimise the palette
    bRVal = palBuilder.palettize(dwPalType,(BYTE*)pPalAddr);
    if (!bRVal) {
        palBuilder.freeTable(dwPalType);
        dbgTracePop();
        return FALSE;
    }

    // pass 2 - convert the texture

    // get the original source object
    pSrcObj = pOrigSrcObj;

    DWORD dwWidth     = pDstTexture->getWidth();
    DWORD dwHeight    = pDstTexture->getHeight();
    DWORD dwLogWidth  = pDstTexture->getLogWidth();
    DWORD dwLogHeight = pDstTexture->getLogHeight();
    DWORD dwDestBaseAddr = pDstTexture->getSwizzled()->getAddress();
    DWORD dwOffset = 0;

    do {
        // get source surface
        CSimpleSurface* pSrcSurface = pSrcObj->getSimpleSurface();

        palBuilder.apply (dwPalType, pSrcSurface->getAddress(), dwDestBaseAddr + dwOffset,
                          dwWidth, dwLogWidth, dwHeight, dwLogHeight);

        dwOffset += dwWidth * dwHeight;
        if (dwLogWidth)  dwLogWidth--;
        if (dwLogHeight) dwLogHeight--;
        dwWidth  = 1 << dwLogWidth;
        dwHeight = 1 << dwLogHeight;

        pSrcObj = pSrcObj->getAttachedA();

    } while (pSrcObj); // terminate if we have reached the end of either mipmap chain

#ifdef CAPTURE
    if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
        CAPTURE_MEMORY_WRITE memwr;
        memwr.dwJmpCommand         = CAPTURE_JMP_COMMAND;
        memwr.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
        memwr.dwExtensionID        = CAPTURE_XID_MEMORY_WRITE;
        memwr.dwCtxDMAHandle       = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;
        memwr.dwOffset             = pDstTexture->getSwizzled()->getOffset();
        memwr.dwSize               = pDstTexture->getSwizzled()->getSize();
        captureLog (&memwr, sizeof(memwr));
        captureLog ((void*)dwDestBaseAddr, memwr.dwSize);
    }
#endif

    // set the pixel formats, flag and cleanup
    pDstTexture->setOriginalFormat(dwOriginalFormat);
    if (pDstTexture->hasColorKey() && (dwOriginalFormat == NV_SURFACE_FORMAT_X8R8G8B8)) {
        pDstTexture->setColorKey(pDstTexture->getColorKey() | 0xFF000000); // fix colour-key alpha
    }
    pDstTexture->setFormat(dwNewFormat);
    DWORD pPalOffset = (pDstTexture->getSwizzled()->getOffset() + pDstTexture->getSwizzled()->getSize() / 2 + 63) & ~63;
    pDstTexture->setAutoPaletteOffset(pPalOffset);
    pDstTexture->tagAutoPalettized();
    pDstTexture->cpuUnlockSwz();
    palBuilder.freeTable(dwPalType);

    dbgTracePop();
    return TRUE;
}

//---------------------------------------------------------------------------

BOOL nvUnpalettize (CNvObject *pDstObj)
{
    CTexture *pTexture = pDstObj->getTexture();

    dbgTracePush ("nvUnpalettize");

    // get palette address and 64 byte align
    DWORD dwAddr   = pTexture->getSwizzled()->getAddress();
    DWORD pPalAddr = (dwAddr + pTexture->getSwizzled()->getSize() / 2 + 63) & ~63;
    DWORD dwBPP    = pTexture->getLinear()->getBPP();

    // copy palette away
    DWORD adwPalette[256];
    memcpy (adwPalette, (char*)pPalAddr, 256 * dwBPP);

    // determine the new texture format and create palette builder object
    CPaletteBuilder palBuilder;
    DWORD dwOriginalFormat = pTexture->getOriginalFormat();
    DWORD dwPalType;
    switch(dwOriginalFormat)
    {
    case NV_SURFACE_FORMAT_A8R8G8B8:
        dwPalType   = PT_8888;
        break;
    case NV_SURFACE_FORMAT_X8R8G8B8:
        dwPalType   = PT_X888;
        break;
    default:
        // unhandled texture format
        nvAssert(0);
        dbgTracePop();
        return FALSE;
    }

    // convert back
    DWORD dwWidth  = pTexture->getWidth();
    DWORD dwHeight = pTexture->getHeight();
    int nMMC    = pTexture->getMipMapCount();
    int nCount;
    DWORD dwSrcOffset = 0, dwDestOffset = 0;

    DWORD sx[20];       // to keep history of sizes
    DWORD sy[20];

    // for correct handling of deep mipmaps (ex: 128x512x9)
    // the correct sequence to be generated is:  512x128 (level 8), 256x64, .....8x2 (level 2), 4x1 (1), 2x1 (0)

    for (nCount = 0; nCount < nMMC; nCount++)
    {
        dwSrcOffset  += dwWidth * dwHeight;
        dwDestOffset += dwWidth * dwHeight * dwBPP;

        sx[nCount]=dwWidth;
        sy[nCount]=dwHeight;

        dwWidth >>= (dwWidth == 1 ? 0 : 1);
        dwHeight >>= (dwHeight == 1 ? 0 : 1);
    }

    for (nCount = nMMC-1; nCount >= 0; nCount--)
    {
        dwSrcOffset  -= sx[nCount] * sy[nCount];
        dwDestOffset -= sx[nCount] * sy[nCount] * dwBPP;

        palBuilder.depalettizeInPlace(dwPalType, dwAddr + dwSrcOffset, dwAddr + dwDestOffset, sx[nCount], sy[nCount], (DWORD*)adwPalette);
    }

#ifdef CAPTURE
    if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
        CAPTURE_MEMORY_WRITE memwr;
        memwr.dwJmpCommand         = CAPTURE_JMP_COMMAND;
        memwr.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
        memwr.dwExtensionID        = CAPTURE_XID_MEMORY_WRITE;
        memwr.dwCtxDMAHandle       = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;
        memwr.dwOffset             = pTexture->getSwizzled()->getOffset();
        memwr.dwSize               = pTexture->getSwizzled()->getSize();
        captureLog (&memwr, sizeof(memwr));
        captureLog ((void*)pTexture->getSwizzled()->getAddress(), memwr.dwSize);
    }
#endif

    dbgTracePop();
    return TRUE;
}

//---------------------------------------------------------------------------

void nvAutoPaletteCheckAndRevert(CTexture* pTexture, BOOL bFullSurface)
{
    dbgTracePush ("nvAutoPaletteCheckAndRevert");

    if (pTexture->hasAutoPalette()) {
        // will need to modify palettization logic if this assert fails
        nvAssert(pTexture == pTexture->getBaseTexture());
        if (!bFullSurface) {
            // copy texture contents forward only for partial blits
            nvUnpalettize(pTexture->getWrapper());
        }
        if (pTexture->hasColorKey() && (pTexture->getOriginalFormat() == NV_SURFACE_FORMAT_X8R8G8B8)) {
            pTexture->setColorKey(pTexture->getColorKey() & 0x00FFFFFF); // restore colour-key alpha
        }
        pTexture->resetOriginalFormat();
        pTexture->untagAutoPalettized();
        pTexture->tagUnpalettized(); // tag so we don't try palettizing this texture again
    }

    dbgTracePop();
}

//---------------------------------------------------------------------------

// nvCountStreamDMAs

// counts the number of unique context DMAs and the total number
// of system streams from among the first 'dwStreamCount' streams.
// the former is returned in bits 15:0 and the latter in 31:16

DWORD nvCountStreamDMAs (PNVD3DCONTEXT pContext, DWORD dwStreamCount)
{
    DWORD KnownContextDMAs[NV_CAPS_MAX_STREAMS];
    DWORD dwStreamSelector;
    DWORD dwContextDMA;
    DWORD dwUniqueDMACount;
    DWORD dwSystemDMACount;
    DWORD i,j;

    CVertexShader *pVertexShader = pContext->pCurrentVShader;
    nvAssert (pVertexShader);

    dwUniqueDMACount = 0;
    dwSystemDMACount = 0;

    for (i=0; i<dwStreamCount; i++)
    {
        dwStreamSelector = pVertexShader->getVAStream(i);
        if (dwStreamSelector != CVertexShader::VA_STREAM_NONE)
        {
            if (!pContext->ppDX8Streams[dwStreamSelector]) {
                // simply aborts if we hit a NULL pointer. presumably things aren't entirely set up
                // yet and this function will get called again once everything is in its final state
                return (0);
            }
            dwContextDMA = pContext->ppDX8Streams[dwStreamSelector]->getContextDMA();
            if (dwContextDMA == NV_CONTEXT_DMA_NONE)
            {
                //fake the context DMA by using the stream number or'd with 0xFFFF0000
                //this way we'll track the total number of system streams in use
                dwContextDMA = dwStreamSelector | 0xFFFF0000;

                for (j=0; j<dwUniqueDMACount; j++) {
                    if (dwContextDMA == KnownContextDMAs[j]) break;
                }
                if (j == dwUniqueDMACount) {
                    KnownContextDMAs[j] = dwContextDMA;
                    dwUniqueDMACount++;
                    dwSystemDMACount++;
                }
            }
            else
            {
                for (j=0; j<dwUniqueDMACount; j++) {
                    if (dwContextDMA == KnownContextDMAs[j]) break;
                }
                if (j == dwUniqueDMACount) {
                    KnownContextDMAs[j] = dwContextDMA;
                    dwUniqueDMACount++;
                }
            }
        }
    }

    return ((dwUniqueDMACount << 0) | (dwSystemDMACount << 16));
}

