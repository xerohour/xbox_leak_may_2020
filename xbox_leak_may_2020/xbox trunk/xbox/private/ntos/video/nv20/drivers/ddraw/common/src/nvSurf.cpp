/*
 * Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 * (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvSurf.cpp                                                        *
*   Routines for management of surface lists added in DX7                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler (bertrem)   16Dec98    created                     *
*       Craig Duttweiler (bertrem)   05Aug99    major rework for G.U.D.     *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

//--------------------------------------------------------------------------

DWORD nvCreateSystemMemorySurface (LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl)
{
    dbgTracePush ("nvCreateSystemMemorySurface");

    // create the basic objects
#ifndef WINNT
    CNvObject *pNvObj = new CNvObject (pDDSLcl->dwProcessId);
#else
    CNvObject *pNvObj = new CNvObject (0);
#endif // WINNT

    if (!pNvObj) {
        DPF ("nvCreateSystemMemorySurface: failed allocation of CNvObject");
        dbgD3DError();
        dbgTracePop();
        return DDERR_OUTOFMEMORY;
    }

    if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_EXECUTEBUFFER)
    {
        if (pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_COMMANDBUFFER)
        {
            CCommandBuffer *pCommandBuffer = new CCommandBuffer (pNvObj, pDDSLcl);
            
            if (!pCommandBuffer) {
                pNvObj->release();
                DPF ("nvCreateSystemMemorySurface: failed to create CCommandBuffer");
                dbgD3DError();
                dbgTracePop();
                return DDERR_OUTOFMEMORY;
            }

            pNvObj->setObject (CNvObject::NVOBJ_COMMANDBUFFER, pCommandBuffer);
            pCommandBuffer->own(pDDSLcl->lpGbl->fpVidMem, pDDSLcl->lpGbl->dwLinearSize, CSimpleSurface::HEAP_SYS);
        }
        else
        {
            // assume everything else is a vertex buffer
            CVertexBuffer *pVertexBuffer = new CVertexBuffer (pNvObj);

            if (!pVertexBuffer) {
                pNvObj->release();
                DPF ("nvCreateSystemMemorySurface: failed to create CVertexBuffer");
                dbgD3DError();
                dbgTracePop();
                return DDERR_OUTOFMEMORY;
            }

            pNvObj->setObject (CNvObject::NVOBJ_VERTEXBUFFER, pVertexBuffer);
            pVertexBuffer->own(pDDSLcl->lpGbl->fpVidMem, pDDSLcl->lpGbl->dwLinearSize, CSimpleSurface::HEAP_SYS);
            pVertexBuffer->setVertexStride (0);
        }
    }

    else {

        CSimpleSurface *pSurf  = new CSimpleSurface;

        if (!pSurf) {
            pNvObj->release();
            DPF ("nvCreateSystemMemorySurface: failed allocation of CSimpleSurface");
            dbgD3DError();
            dbgTracePop();
            return DDERR_OUTOFMEMORY;
        }

        pNvObj->setObject (CNvObject::NVOBJ_SIMPLESURFACE, pSurf);

#ifdef WINNT
        // workaround for Win2K DX7 runtime bug - DXT surface size is set incorrectly
        if (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_FOURCC) {
            switch(pDDSLcl->lpGbl->ddpfSurface.dwFourCC) {
            case FOURCC_DXT1:
                pDDSLcl->lpGbl->wHeight <<= 2;
                pDDSLcl->lpGbl->wWidth  >>= 1;
                break;
            case FOURCC_DXT2:
            case FOURCC_DXT3:
            case FOURCC_DXT4:
            case FOURCC_DXT5:
                pDDSLcl->lpGbl->wHeight <<= 2;
                pDDSLcl->lpGbl->wWidth  >>= 2;
                break;
            }
        }
#endif // WINNT

        // determine common surface parameters
        DWORD dwFormat, dwWidth, dwHeight, dwDepth, dwBPPRequested, dwBPPGranted, dwPitch, dwSlicePitch, dwMultiSampleBits;
        BOOL  bFourCC;
        DWORD dwRV = nvCalculateSurfaceParams (pDDSLcl, &dwFormat, &bFourCC, &dwWidth, &dwHeight, &dwDepth, &dwMultiSampleBits,
                                               &dwBPPRequested, &dwBPPGranted, &dwPitch, &dwSlicePitch, FALSE);

        if (dwRV == DDERR_INVALIDPIXELFORMAT) {
            // allow invalid (ie. unknown) pixel formats. just trust the surface
            dwFormat     = NV_SURFACE_FORMAT_UNKNOWN;
            dwPitch      = pDDSLcl->lpGbl->lPitch;
            dwSlicePitch = pDDSLcl->lpGbl->lSlicePitch;
            dwHeight     = pDDSLcl->lpGbl->wHeight;
            dwWidth      = pDDSLcl->lpGbl->wWidth;
            dwDepth      = pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps4 & 0xffff;
            dwBPPGranted = dwBPPRequested = pDDSLcl->lpGbl->ddpfSurface.dwRGBBitCount/8;
        }
        else if (dwRV != DD_OK) {
            delete pSurf;
            pNvObj->release();
            dbgTracePop();
            return dwRV;
        }

#ifdef WINNT
        // MPR - MS runtime bug. pDDSLcl->lpGbl->lSlicePitch is not filled out for
        // system memory volume textures. Fortunately it is easily calculated
        if (pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_VOLUME) {
            dwSlicePitch = dwPitch * dwHeight;
        }
#endif

        pSurf->own (pDDSLcl->lpGbl->fpVidMem, dwPitch, dwHeight, CSimpleSurface::HEAP_SYS, FALSE);
        pSurf->setWrapper (pNvObj);
        pSurf->setFormat (dwFormat);
        pSurf->setWidth (dwWidth);
        pSurf->setDepth(dwDepth);
        pSurf->setSlicePitch (dwSlicePitch);
        pSurf->setBPP (dwBPPGranted);
        pSurf->setBPPRequested (dwBPPRequested);
        if (bFourCC) {
            pSurf->setFourCC (pDDSLcl->lpGbl->ddpfSurface.dwFourCC);
        }

    }

    SET_PNVOBJ (pDDSLcl, pNvObj);

    dbgTracePop();
    return DD_OK;
}

//--------------------------------------------------------------------------

// return a pointer to the given surface's mipmap or flip chain. this is
// straightforward for regular textures and other chains but totally convoluted
// for cubemaps because MS is dumb

LPDDRAWI_DDRAWSURFACE_LCL nvFindSurfaceChain
(
    LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl,
    LPATTACHLIST *ppAttachList
)
{
    DWORD dwRootFace, dwAttachedFace;

    dbgTracePush ("nvFindSurfaceChain");

    nvAssert (pDDSLcl);

    if (pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP) {
        // cubemap. seek out the proper mipmap chain
        dwRootFace = pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP_ALLFACES;
        *ppAttachList = pDDSLcl->lpAttachList;
        while (*ppAttachList) {
            dwAttachedFace = (*ppAttachList)->lpAttached->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP_ALLFACES;
            if (dwAttachedFace == dwRootFace) {
                dbgTracePop();
                return (*ppAttachList)->lpAttached;
            }
            *ppAttachList = (*ppAttachList)->lpLink;
        }
    }

    else {
        // not a cubemap. just return the first thing attached
        *ppAttachList = pDDSLcl->lpAttachList;
        if (*ppAttachList) {
            dbgTracePop();
            return pDDSLcl->lpAttachList->lpAttached;
        }
    }

    dbgTracePop();
    return NULL;
}

//--------------------------------------------------------------------------

// return a pointer to the next face of the cubemap. this is convoluted
// because MS is annoying. we sort it all out here and store it internally
// in a more sane manner.

LPDDRAWI_DDRAWSURFACE_LCL nvFindNextCubemapFace
(
    LPDDRAWI_DDRAWSURFACE_LCL pDDSLclRoot,   // the root of the whole ratnest
    LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl        // the surface whose next face we want to find
)
{
    LPATTACHLIST pAttachList;
    DWORD        dwThisFace, dwNextFace;

    dbgTracePush ("nvFindNextCubemapFace");

    nvAssert (pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP);
    nvAssert ((pDDSLclRoot->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP_ALLFACES) == DDSCAPS2_CUBEMAP_POSITIVEX);

    // we rely on these definitions. if MS changes them, we'll have to fix
    nvAssert ((DDSCAPS2_CUBEMAP_POSITIVEX == 0x00000400L) &&
              (DDSCAPS2_CUBEMAP_NEGATIVEX == 0x00000800L) &&
              (DDSCAPS2_CUBEMAP_POSITIVEY == 0x00001000L) &&
              (DDSCAPS2_CUBEMAP_NEGATIVEY == 0x00002000L) &&
              (DDSCAPS2_CUBEMAP_POSITIVEZ == 0x00004000L) &&
              (DDSCAPS2_CUBEMAP_NEGATIVEZ == 0x00008000L));

    dwThisFace = pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP_ALLFACES;
    dwNextFace = dwThisFace << 1;

    if (dwThisFace == DDSCAPS2_CUBEMAP_NEGATIVEZ) {
        dbgTracePop();
        return NULL;
    }

    if (pDDSLclRoot->lpAttachList == NULL) {
        dbgTracePop();
        return NULL;
    }

    pAttachList = pDDSLclRoot->lpAttachList;

    while (pAttachList) {
        dwThisFace = pAttachList->lpAttached->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP_ALLFACES;
        if (dwThisFace == dwNextFace) {
            dbgTracePop();
            return (pAttachList->lpAttached);
        }
        pAttachList = pAttachList->lpLink;
    }

    dbgTracePop();
    return NULL;
}

//--------------------------------------------------------------------------

DWORD nvCheckSystemMemorySurface
(
    LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl
)
{
    dbgTracePush ("nvCheckSystemMemorySurface");

    nvAssert (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY);

    CNvObject *pNvObj = GET_PNVOBJ (pDDSLcl);

    if (!IS_VALID_PNVOBJ(pNvObj)) {
        // if this is a system memory surface, we may not have ever seen it before.
        // CreateSurfaceEx (and its subsidiaries) have to play the part of CreateSurface32.
        DWORD dwRV = nvCreateSystemMemorySurface (pDDSLcl);
        dbgTracePop();
        return dwRV;
    }

    else {
        // we've seen this surface before and have created an nvObject, but MS may have
        // moved it since we last saw it, so make sure that our pointer is up to date.
        //  (bdw) - actually, it seems that anything about it may have changed - check dimentions also
        //        - bugbug: we do not check format (although I have seen this change also)
        nvAssert (pNvObj->getClass() == CNvObject::NVOBJ_SIMPLESURFACE);
        CSimpleSurface *pSurf = pNvObj->getSimpleSurface();
        if ((pSurf->getAddress() != pDDSLcl->lpGbl->fpVidMem)
         || (pSurf->getWidth()   != pDDSLcl->lpGbl->wWidth)
         || (pSurf->getHeight()  != pDDSLcl->lpGbl->wHeight))
        {
            DWORD dwFormat, dwWidth, dwHeight, dwDepth, dwMultiSampleBits, dwBPPRequested, dwBPPGranted, dwPitch, dwSlicePitch, dwRV;
            BOOL  bFourCC;
            dwRV = nvCalculateSurfaceParams (pDDSLcl, &dwFormat, &bFourCC, &dwWidth, &dwHeight, &dwDepth, &dwMultiSampleBits,
                                             &dwBPPRequested, &dwBPPGranted, &dwPitch, &dwSlicePitch, FALSE);
            nvAssert (dwRV == DD_OK);  // really?  what about UNKNOWN formats?
            pSurf->disown();
            pSurf->own (pDDSLcl->lpGbl->fpVidMem, dwPitch, dwHeight, CSimpleSurface::HEAP_SYS, FALSE);
            pSurf->setFormat (dwFormat);
            pSurf->setWidth (dwWidth);
            pSurf->setBPP (dwBPPGranted);
            pSurf->setBPPRequested (dwBPPRequested);
        }
        dbgTracePop();
        return DD_OK;
    }

}

//--------------------------------------------------------------------------

DWORD nvAddSurfaceChainToList
(
    PNV_OBJECT_LIST           pNvObjectList,
    LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl
)
{
    CNvObject                *pNvObj;
    DWORD                     dwHandle;
    LPDDRAWI_DDRAWSURFACE_LCL pChain;

    dbgTracePush ("nvAddSurfaceChainToList");

    if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) {
        DWORD dwRV = nvCheckSystemMemorySurface (pDDSLcl);
        if (dwRV != DD_OK) {
            dbgTracePop();
            return dwRV;
        }
    }

    // at creation time, we will have cached the NvObject pointer in the
    // dwReserved field. retrieve it now and associate it with dwHandle

    pNvObj = GET_PNVOBJ (pDDSLcl);
    if (pNvObj) {
        dwHandle = pDDSLcl->lpSurfMore->dwSurfaceHandle;
        DWORD dwRV = nvAddObjectToList (pNvObjectList, pNvObj, dwHandle);
        if (dwRV != DD_OK) {
            dbgTracePop();
            return dwRV;
        }
    }
    else {
        nvAssert (pDDSLcl->lpGbl->fpVidMem == NULL);
        DPF ("runtime called CSEx on a surface that's already been destroyed (or was never created)");
        dbgD3DError();
    }

#ifdef DEBUG
    if (pNvObj && (pNvObj->getClass() == CNvObject::NVOBJ_SIMPLESURFACE)) {
        CSimpleSurface *pSurf = pNvObj->getSimpleSurface();
        nvAssert (pSurf->getfpVidMem() == pDDSLcl->lpGbl->fpVidMem);
        if ( pSurf->getFormat() != NV_SURFACE_FORMAT_UNKNOWN ) {
            DWORD dwPitch, dwSlicePitch, dwFormat, dwWidth, dwHeight, dwDepth, dwMultiSampleBits, dwBPPRequested, dwBPPGranted, dwRV;
            BOOL  bFourCC;
            dwRV = nvCalculateSurfaceParams (pDDSLcl, &dwFormat, &bFourCC, &dwWidth, &dwHeight, &dwDepth, &dwMultiSampleBits,
                                             &dwBPPRequested, &dwBPPGranted, &dwPitch, &dwSlicePitch, FALSE);
            nvAssert (dwRV     == DD_OK);
            nvAssert (dwFormat == pSurf->getFormat());
    #ifndef CAPTURE  // not when capturing b/c we may be spoofing allocation sizes
            if (!pSurf->isDXT()) {
                nvAssert (dwWidth  == pSurf->getWidth());
                nvAssert (dwHeight == pSurf->getHeight());
            }
    #endif
            nvAssert (dwBPPGranted == pSurf->getBPP());
        }
    }
#endif

    // get this surface's mipmap or flip chain, if any
    LPATTACHLIST lpAttachList;
    pChain = nvFindSurfaceChain (pDDSLcl, &lpAttachList);

    while (pChain) {

        // check if we are back at the beginning - this happens with flip chains
        if (pChain == pDDSLcl) break;

#ifndef WINNT
        BOOL bImplicit = lpAttachList->dwFlags & DDAL_IMPLICIT;
#else
        // simulate the implicit flag by checking for a NULL dwReserved1
        // this is only needed for flip chains with attached z-buffers, so it should
        // always be true for textures
        BOOL bImplicit = (GET_PNVOBJ(pChain) != NULL) | (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_TEXTURE);
#endif // !WINNT

        // assign handle for implicitly attached surfaces only
        if (bImplicit) {
            // create an NvObject etc if necessary
            if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) {
                DWORD dwRV = nvCheckSystemMemorySurface (pChain);
                if (dwRV != DD_OK) {
                    dbgTracePop();
                    return dwRV;
                }
            }

            if (pNvObj) {
                // attach the last one to this one
                pNvObj->setAttachedA (GET_PNVOBJ (pChain));
            }

            // add the new one to the list
            pNvObj = GET_PNVOBJ (pChain);
            if (pNvObj) {
                dwHandle = pChain->lpSurfMore->dwSurfaceHandle;
                DWORD dwRV = nvAddObjectToList (pNvObjectList, pNvObj, dwHandle);
                if (dwRV != DD_OK) {
                    dbgTracePop();
                    return dwRV;
                }
            }
            else {
                nvAssert (pChain->lpGbl->fpVidMem == NULL);
                DPF ("runtime called CSEx on a surface that's already been destroyed (or was never created)");
                dbgD3DError();
            }

#ifdef DEBUG
            if (pNvObj && (pNvObj->getClass() == CNvObject::NVOBJ_SIMPLESURFACE)) {
                CSimpleSurface *pSurf = pNvObj->getSimpleSurface();
                nvAssert (pSurf->getfpVidMem() == pChain->lpGbl->fpVidMem);
                if (pSurf->getFormat() != NV_SURFACE_FORMAT_UNKNOWN) {
                    DWORD dwPitch, dwSlicePitch, dwFormat, dwWidth, dwHeight, dwDepth, dwMultiSampleBits, dwBPPRequested, dwBPPGranted, dwRV;
                    BOOL  bFourCC;
                    dwRV = nvCalculateSurfaceParams (pChain, &dwFormat, &bFourCC, &dwWidth, &dwHeight, &dwDepth, &dwMultiSampleBits,
                                                     &dwBPPRequested, &dwBPPGranted, &dwPitch, &dwSlicePitch, FALSE);
                    nvAssert (dwRV     == DD_OK);
                    nvAssert (dwFormat == pSurf->getFormat());
                    if (!pSurf->isDXT()) {  // what if CAPTURE is on?
                        nvAssert (dwWidth  == pSurf->getWidth());
                        nvAssert (dwHeight == pSurf->getHeight());
                    }
                    nvAssert (dwBPPGranted == pSurf->getBPP());
                }
            }
#endif
            // get the next attach list
            lpAttachList = pChain->lpAttachList;
        }
        else { // !bImplicit
            lpAttachList = lpAttachList->lpLink;
        }

        // get the next surface
        pChain = lpAttachList ? lpAttachList->lpAttached : NULL;

    }  // while (pChain)

    dbgTracePop();
    return DD_OK;
}

//--------------------------------------------------------------------------

DWORD nvDeleteSystemMemorySurface (LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl)
{
    dbgTracePush ("nvDeleteSystemMemorySurface");

    // retrieve the basic objects
    CNvObject *pNvObj = GET_PNVOBJ (pDDSLcl);

    if (IS_VALID_PNVOBJ(pNvObj)) {
        nvClearObjectListEntry (pNvObj, &global.pNvSurfaceLists);
        pNvObj->release();
        SET_PNVOBJ (pDDSLcl, NULL);
    }
    else if (pNvObj) {
        // this is likely a contextdma
        NvRmFree (pDriverData->dwRootHandle, pDriverData->dwRootHandle, (DWORD)pNvObj);
    }

    dbgTracePop();
    return DD_OK;
}

//--------------------------------------------------------------------------

// DX7 callback to let the driver know the value of the handle with
// which a newly created surface (an NvObject) will be associated

DWORD __stdcall nvCreateSurfaceEx
(
    LPDDHAL_CREATESURFACEEXDATA lpcsed
)
{
    LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl;
    PNV_OBJECT_LIST           pNvObjectList;
    CNvObject                *pLastFace;
    DWORD                     dwDDLclID;

    dbgTracePush ("nvCreateSurfaceEx");

    nvSetDriverDataPtrFromDDGbl (lpcsed->lpDDLcl->lpGbl);

    // get ourselves a surface list
    dwDDLclID = (DWORD)(lpcsed->lpDDLcl);
    pNvObjectList = nvFindObjectList (dwDDLclID, &global.pNvSurfaceLists);
    if (pNvObjectList == NULL) {
        // find will create a list if it doesn't already exist
        // a return value of NULL indicates that a new list could not be created
        NvReleaseSemaphore(pDriverData);
        lpcsed->ddRVal = DDERR_OUTOFMEMORY;
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;
    }

    pDDSLcl = lpcsed->lpDDSLcl;

    if ((pDDSLcl->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) && (!pDDSLcl->lpGbl->fpVidMem)) {
        // Microsoft's way of letting us know that a system memory surface is being destroyed.
        DPF_LEVEL(NVDBG_LEVEL_SURFACE_ACTIVITY, "CSEx: destroy system surface, handle = %02x",
            pDDSLcl->lpSurfMore->dwSurfaceHandle);
        nvDeleteSystemMemorySurface (pDDSLcl);
        NvReleaseSemaphore(pDriverData);
        lpcsed->ddRVal = DD_OK;
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;
    }

    DPF_LEVEL(NVDBG_LEVEL_SURFACE_ACTIVITY, "CreateSurfaceEx: pDDSLcl = 0x%08x, fpVidMem = 0x%08x, handle = %02x",
        pDDSLcl, pDDSLcl->lpGbl->fpVidMem, pDDSLcl->lpSurfMore->dwSurfaceHandle);
    // handle the first chain of surfaces (flip chain, mipmap chain, etc)
    DWORD dwRV = nvAddSurfaceChainToList (pNvObjectList, pDDSLcl);
    if (dwRV != DD_OK)
    {
        NvReleaseSemaphore(pDriverData);
        lpcsed->ddRVal = dwRV;
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;
    }

    // iterate through linked attach lists to handle cube maps
    if (pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP) {

        // cache a pointer to the current face then get the next one
        pLastFace = GET_PNVOBJ (pDDSLcl);
        pDDSLcl = nvFindNextCubemapFace (lpcsed->lpDDSLcl, pDDSLcl);

        while (pDDSLcl) {
            // add this face's chain to out list
            dwRV = nvAddSurfaceChainToList (pNvObjectList, pDDSLcl);
            if (dwRV != DD_OK)
            {
                NvReleaseSemaphore(pDriverData);
                lpcsed->ddRVal = dwRV;
                dbgTracePop();
                return DDHAL_DRIVER_HANDLED;
            }

            // make the last face point to this one
            pLastFace->setAttachedB (GET_PNVOBJ (pDDSLcl));

            // advance
            pLastFace = GET_PNVOBJ (pDDSLcl);
            pDDSLcl = nvFindNextCubemapFace (lpcsed->lpDDSLcl, pDDSLcl);
        }

    }

    NvReleaseSemaphore(pDriverData);
    lpcsed->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

//-------------------------------------------------------------------------

// lock all textures referenced in the current combiner setup for use by the HW

void nvHWLockTextures
(
    PNVD3DCONTEXT pContext
)
{
    // late binding of texture handles set with D3DRENDERSTATE_TEXTUREHANDLE (accomodates retained mode junk)
    DWORD dwTextureHandle = pContext->tssState[0].dwHandle;
    if (dwTextureHandle) {
        pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] = (DWORD) nvGetObjectFromHandle (pContext->dwDDLclID, dwTextureHandle, &global.pNvSurfaceLists);
        pContext->tssState[0].dwHandle = 0;
    }

    for (DWORD i = 0; i < 8; i++) {
        CNvObject *pNvObj = (CNvObject *)(pContext->tssState[i].dwValue[D3DTSS_TEXTUREMAP]);
        if (pNvObj) {
            CTexture *pTexture = pNvObj->getTexture();
            // we have to check that the 'textures' that are loaded really are
            // textures. some apps (like lego island) will pass in normal surfaces
            // as textures and cause us to fault
            if (pTexture && 
               (pTexture->getLinear() != pContext->pRenderTarget) &&
               (pTexture->getSwizzled() != pContext->pRenderTarget)) {
                // lock the texture
                pTexture->hwLock (pContext, CSimpleSurface::LOCK_NORMAL);
                // lock the palette
                CNvObject *pPaletteObj = pNvObj->getPalette();
                if (pPaletteObj) {
                    pPaletteObj->getPaletteSurface()->hwLock(CSimpleSurface::LOCK_NORMAL);
                }
            } else {
//              pContext->tssState[i].dwValue[D3DTSS_TEXTUREMAP] = NULL;
                //don't set this to NULL-- it should be failed in validate instead
                //set to a default texture at last second if they really insist
            }
        }
    }

    //lock the zbuffer and render targets as well for synchronizations with Locks of RT or ZB.
    // -/- could optimize this a bit if the 'ZBuffer' writes have been turned off -/-
#if (NVARCH >= 0x020)
    CSimpleSurface *pRenderTarget = pContext->kelvinAA.GetCurrentRT(pContext);
    CSimpleSurface *pZetaBuffer = pContext->kelvinAA.GetCurrentZB(pContext);
#else
    CSimpleSurface *pRenderTarget = pContext->pRenderTarget;
    CSimpleSurface *pZetaBuffer = pContext->pZetaBuffer;
#endif

    pRenderTarget->hwLock(CSimpleSurface::LOCK_NORMAL);
    if (pZetaBuffer) pZetaBuffer->hwLock(CSimpleSurface::LOCK_NORMAL);
}

//-------------------------------------------------------------------------

// unlock all textures referenced in the current combiner setup for use by the HW

void nvHWUnlockTextures
(
    PNVD3DCONTEXT pContext
)
{
    nvAssert(pContext);

    for (DWORD i = 0; i < 8; i++) {
        CNvObject *pNvObj = (CNvObject *)(pContext->tssState[i].dwValue[D3DTSS_TEXTUREMAP]);
        if (pNvObj) {
            CTexture  *pTexture = pNvObj->getTexture();
            
            if (pTexture &&
               (pTexture->getLinear() != pContext->pRenderTarget) &&
               (pTexture->getSwizzled() != pContext->pRenderTarget)) {
                // unlock the texture
                pNvObj->getTexture()->hwUnlock();
                // unlock the palette
                CNvObject *pPaletteObj = pNvObj->getPalette();
                if (pPaletteObj) {
                    pPaletteObj->getPaletteSurface()->hwUnlock();
                }
            }
        }
    }

    //unlock rendertarget and zbuffer
#if (NVARCH >= 0x020)
    CSimpleSurface *pRenderTarget = pContext->kelvinAA.GetCurrentRT(pContext);
    CSimpleSurface *pZetaBuffer = pContext->kelvinAA.GetCurrentZB(pContext);
#else
    CSimpleSurface *pRenderTarget = pContext->pRenderTarget;
    CSimpleSurface *pZetaBuffer = pContext->pZetaBuffer;
#endif

    nvAssert(pRenderTarget);
    pRenderTarget->hwUnlock();
    if (pZetaBuffer) pZetaBuffer->hwUnlock();
}

//-------------------------------------------------------------------------

// lock the first dwCount vertex streams for use by HW

void nvHWLockStreams
(
    PNVD3DCONTEXT pContext,
    DWORD         dwCount
)
{
    if(pContext->dwFlags & CONTEXT_NEEDS_VTX_CACHE_FLUSH)
    {
#if (NVARCH >= 0x020)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            nvglSetNv20KelvinInvalidateVertexCache (NV_DD_KELVIN);
        }
        else
#endif
#if (NVARCH >= 0x010)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
            nvglSetNv10CelsiusInvalidateVertexCache (NV_DD_CELSIUS);
        }
#endif
        pContext->dwFlags &= ~CONTEXT_NEEDS_VTX_CACHE_FLUSH;
    }

    for (DWORD i=0; i<dwCount; i++) {
        if (pContext->ppDX8Streams[i] != NULL) {
#ifdef MCFD
            pContext->ppDX8Streams[i]->LockForGPU (CSimpleSurface::LOCK_READ);
#else
            pContext->ppDX8Streams[i]->hwLock (CSimpleSurface::LOCK_NORMAL);
#endif
        }
    }
}

//-------------------------------------------------------------------------

// release the HW lock on the first dwCount vertex streams

void nvHWUnlockStreams
(
    PNVD3DCONTEXT pContext,
    DWORD         dwCount
)
{
    for (DWORD i=0; i<dwCount; i++) {
        if (pContext->ppDX8Streams[i] != NULL) {
#ifdef MCFD
            pContext->ppDX8Streams[i]->UnlockForGPU ();
#else
            pContext->ppDX8Streams[i]->hwUnlock ();
#endif
        }
    }
}

#endif // NVARCH >= 0x04
