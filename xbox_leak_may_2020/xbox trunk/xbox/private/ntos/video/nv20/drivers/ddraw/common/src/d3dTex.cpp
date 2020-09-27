/*
 * Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 * (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: d3dTex.cpp                                                        *
*   The Direct3D texture routines.                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/02/97 - re-wrote and cleaned up.     *
*       Ben de Waal (bdw)           08/28/97 - enabled locked tex status    *
*                                   09/19/97 - optimize mipmap creation     *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

/*
 * --------------------------------------------------------------------------
 * D3D HAL texture callback routines
 * --------------------------------------------------------------------------
 */
DWORD __stdcall nvTextureCreate
(
    LPD3DHAL_TEXTURECREATEDATA  pTextureCreateData
)
{
    dbgTracePush ("nvTextureCreate");

    LPDDRAWI_DDRAWSURFACE_LCL   lpLcl;

    /*
     * NOTES:
     *
     * This callback is invoked when a texture is to be created from a
     *  DirectDrawSurface.
     * We must pass back the created handle.
     */

    /*
     * Get the pointer to the context this texture is associated with.
     */
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pTextureCreateData);

    /*
     * Validate the parameters of the texture being created.
     */
    lpLcl = NvLockTextureSurface(pTextureCreateData);
    if (!lpLcl)
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureCreate - Texture Create Failed");
        dbgD3DError();
        pTextureCreateData->ddrval = D3DERR_TEXTURE_CREATE_FAILED;
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Get pointer to global driver. data structure.
     */
    nvSetDriverDataPtrFromContext(pContext);

    /*
     * verify that we have the DX6 runtime. Should never happen on DX7.
     */
    nvAssert (global.dwDXRuntimeVersion < 0x0700);

    /*
     * get texture pointer
     */
    CNvObject *pNvObj = GET_PNVOBJ (lpLcl);

    if (lpLcl->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) {

        // Not that much we can do about a system memory texture. We just need to
        // create an object which we will use from now on.
        if (!IS_VALID_PNVOBJ(pNvObj)) {
            //The object hasn't been allocated yet.
            if ((pTextureCreateData->ddrval = nvCreateSystemMemorySurface(lpLcl)) != DD_OK)
            {
                dbgD3DError();
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureCreate - out of memory");
                pTextureCreateData->ddrval = D3DERR_TEXTURE_CREATE_FAILED;
            }
            pNvObj = GET_PNVOBJ (lpLcl);
        }
        else {
            //Nothing to do at all
            pTextureCreateData->ddrval = DD_OK;
        }

        nvAssert (IS_VALID_PNVOBJ(pNvObj));
        pTextureCreateData->dwHandle = (DWORD) pNvObj;
        // For DX6 runtime we will reuse the Object->Handle to keep a pointer to the local.
        pNvObj->setHandle ((DWORD)lpLcl);

        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    CTexture *pTexture = pNvObj ? pNvObj->getTexture() : NULL;

    /*
     * is texture object allocated yet?
     */
    if (!pTexture)
    {
        /*
         * verify that we do not overwrite a different object
         */
        nvAssert (GET_PNVOBJ(lpLcl) == NULL);

        /*
         * The internal texture data structure has not been allocated yet. This can
         * happen if the application allocates it's texture handles before it creates the
         * texture surfaces.  (Ziff Davis 3D Winbench)
         * So we have to handle both cases.  If the data structure has not been allocated
         * yet, allocate it here.
         */
        pNvObj   = new CNvObject (pContext->pid);
        pTexture = new CTexture;

        if (!pNvObj || !pTexture)
        {
            /*
             * Bad news.  Could not allocate the internal data structure.  No choice but to
             * return an error.
             */
            dbgD3DError();
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureCreate - out of memory");
            if (pNvObj) pNvObj->release();
            NvReleaseSemaphore(pDriverData);
            pTextureCreateData->ddrval = D3DERR_TEXTURE_CREATE_FAILED;
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }

        /*
         * update stats
         */
        getDC()->dwD3DTextureCount++;

        /*
         * Store the handle of the internal texture structure in the local surface structure
         * driver reserved field.
         */
        pNvObj->setObject (CNvObject::NVOBJ_TEXTURE, pTexture);
        SET_PNVOBJ (lpLcl, pNvObj);
    }

    /*
     * Return the new texture handle.
     */
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvTextureCreate - New texture handle = %08x", pTexture);
    pTextureCreateData->dwHandle = (DWORD)pNvObj;

    /*
     * A handle is only ever allocated for the top-most level of the mipmap chain.
     * run down the chain and update the mipmap base texture for each mipmap.
     */
    if ((lpLcl->ddsCaps.dwCaps & DDSCAPS_MIPMAP)
     && (!lpLcl->lpAttachListFrom)
     && (lpLcl->lpAttachList))
    {
        LPDDRAWI_DDRAWSURFACE_LCL lclMip;
        lclMip = lpLcl->lpAttachList->lpAttached;
        while (lclMip)
        {
            CTexture *pMipMap;

            pMipMap = GET_PNVOBJ(lclMip)->getTexture();
            if (IS_VALID_PNVOBJ(pMipMap))
            {
                pMipMap->tagUserMipLevel();
                pMipMap->setBaseTexture (pTexture);
            }
            if (lclMip->lpAttachList) {
                lclMip = lclMip->lpAttachList->lpAttached;
            }
            else {
                lclMip = 0;
            }
        }
    }

    NvUnlockTextureSurface(lpLcl);
    /*
     * Show that the texture has not been loaded yet and that it is not locked.
     */
    pTexture->tagHasHandle();

    /*
     * Load convert the texture immediately so that hopefully real game speed does not
     * get impacted during actual game play.  This has no effect on the benchmarks, but
     * it can sometimes help out games (like moto racer with the polygon patch).
     */
    if (!pTexture->getSwizzled()->isValid() && pTexture->getLinear()->isValid())
    {
        pTexture->updateSwizzleSurface (pContext);
    }

    /*
     * For DX6 runtime we will reuse the Object->Handle to keep a pointer to
     * the local.
     */
     pNvObj->setHandle((DWORD)lpLcl);
     pNvObj->reference();    //add a reference so we don't go and delete this object
                             //when we 'release' the texture in the symmetric destroy call.

    /*
     * Texture creation successful.
     */
    NvReleaseSemaphore(pDriverData);
    pTextureCreateData->ddrval = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

//-------------------------------------------------------------------------

DWORD __stdcall nvTextureDestroy
(
    LPD3DHAL_TEXTUREDESTROYDATA pTextureDestroyData
)
{
    CNvObject *pNvObj;
    CTexture  *pTexture;

    dbgTracePush ("nvTextureDestroy - handle=%08x", pTextureDestroyData->dwHandle);

    /*
     * get context
     */
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pTextureDestroyData);

    nvSetDriverDataPtrFromContext (pContext);

    /*
     * verify that we have the DX6 runtime. Should never happen on DX7.
     */
    nvAssert (global.dwDXRuntimeVersion < 0x0700);

    pNvObj   = (CNvObject *)pTextureDestroyData->dwHandle;

    if (pNvObj->getClass() == CNvObject::NVOBJ_SIMPLESURFACE) {
        /* This handle is associated with a texture in system memory.
         * We just need to release the object.
         */
        LPDDRAWI_DDRAWSURFACE_LCL lpLcl = (LPDDRAWI_DDRAWSURFACE_LCL)pNvObj->getHandle();
        nvDeleteSystemMemorySurface(lpLcl);
        pTextureDestroyData->ddrval = DD_OK;
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    pTexture = pNvObj->getTexture();

    /*
     * Perform all the neccessary steps for destroying this texture handle.
     * Destruction of the actual texture data structure is actually
     * dependent on the current state of the texture surface structure.
     */
    if (pTexture)
    {
        // Make sure the texture is not the currently selected handle in the context.
        DWORD   dwStage;

        for (dwStage = 0; dwStage < 8; dwStage++)
        {
            if (pContext->tssState[dwStage].dwValue[D3DTSS_TEXTUREMAP] == (DWORD)pNvObj) {
                pContext->tssState[dwStage].dwValue[D3DTSS_TEXTUREMAP] = NULL;
                pContext->tssState[dwStage].dwHandle = 0;
            }
        }

        // Destroy the internal texture data structure.
        //this seems circular if we had a pNvObj above.
        //CNvObject *pNvObj = pTexture->getWrapper();

        //don't remove the pnvobj here -- we shouldn't actually destroy surfaces here
        //LPDDRAWI_DDRAWSURFACE_LCL lpLcl = (LPDDRAWI_DDRAWSURFACE_LCL)pNvObj->getHandle();
        //SET_PNVOBJ (lpLcl, NULL);

        pNvObj->release();
    }

    // Texture destroyed successfully.
    pTextureDestroyData->ddrval = DD_OK;
    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

//-------------------------------------------------------------------------

/*
 * nvTextureSwap
 *
 * This callback is invoked when two texture handles are to be swapped.
 * I.e. the data refered to by the two handles is to be swapped.
 */
DWORD __stdcall nvTextureSwap
(
    LPD3DHAL_TEXTURESWAPDATA pTextureSwapData
)
{
    dbgTracePush ("nvTextureSwap - hContext=%08x, h1=%08x, h2=%08x",
                  pTextureSwapData->dwhContext, pTextureSwapData->dwHandle1, pTextureSwapData->dwHandle2);

    // get a pointer to the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pTextureSwapData);

    nvSetDriverDataPtrFromContext(pContext);

    // verify that we have the DX6 runtime. Should never happen on DX7.
    nvAssert (global.dwDXRuntimeVersion < 0x0700);

    // Get pointers and classes
    CNvObject *pNvObj1, *pNvObj2;
    DWORD      dwClass1, dwClass2;

    pNvObj1  = (CNvObject *)(pTextureSwapData->dwHandle1);
    pNvObj2  = (CNvObject *)(pTextureSwapData->dwHandle2);
    nvAssert (pNvObj1);
    nvAssert (pNvObj2);
    dwClass1 = pNvObj1->getClass();
    dwClass2 = pNvObj2->getClass();

    if ((dwClass1 != CNvObject::NVOBJ_SIMPLESURFACE) &&
        (dwClass1 != CNvObject::NVOBJ_TEXTURE)) {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureSwap - Bad Texture 1");
        dbgD3DError();
        NvReleaseSemaphore(pDriverData);
        pTextureSwapData->ddrval = DDERR_INVALIDPARAMS;
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    if ((dwClass2 != CNvObject::NVOBJ_SIMPLESURFACE) &&
        (dwClass2 != CNvObject::NVOBJ_TEXTURE)) {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureSwap - Bad Texture 2");
        dbgD3DError();
        NvReleaseSemaphore(pDriverData);
        pTextureSwapData->ddrval = DDERR_INVALIDPARAMS;
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    // swap the contents of the pNvObjects
    pNvObj1->swap (pNvObj2);

    // fix up the back-pointers in the DDSLcls
    SET_PNVOBJ (((LPDDRAWI_DDRAWSURFACE_LCL)pNvObj1->getHandle()), pNvObj1);
    SET_PNVOBJ (((LPDDRAWI_DDRAWSURFACE_LCL)pNvObj2->getHandle()), pNvObj2);

    // Texture swap was successful.
    NvReleaseSemaphore(pDriverData);
    pTextureSwapData->ddrval = DD_OK;
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

//-------------------------------------------------------------------------

/*
 * nvTextureGetSurf
 *
 * This callback is invoked when d3d needs to obtain the surface
 * referred to by a handle.
 */
DWORD __stdcall nvTextureGetSurf
(
    LPD3DHAL_TEXTUREGETSURFDATA pTextureGetSurfData
)
{
    dbgTracePush ("nvTextureGetSurf - handle=%08x", pTextureGetSurfData->dwHandle);

    /*
     * Get the pointer to the specified context.
     */
    PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)pTextureGetSurfData->dwhContext;
    if (!pContext)
    {
        dbgD3DError();
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureGetSurf - Bad Context");
        pTextureGetSurfData->ddrval = D3DHAL_CONTEXT_BAD;
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * verify that we have the DX6 runtime. Should never happen on DX7.
     */
    nvAssert (global.dwDXRuntimeVersion < 0x0700);
#ifndef WINNT
// WINNT BUG
    CNvObject *pNvObj = (CNvObject *)pTextureGetSurfData->dwHandle;
    LPDDRAWI_DDRAWSURFACE_LCL lpLcl = (LPDDRAWI_DDRAWSURFACE_LCL)pNvObj->getHandle();
    pTextureGetSurfData->lpDDS = (ULONG_PTR)lpLcl;
#endif

    // Texture surface was gotten successfully.
    pTextureGetSurfData->ddrval = DD_OK;
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

/*
 * --------------------------------------------------------------------------
 * Texture heap management routines.
 * --------------------------------------------------------------------------
 */

// nvPCICreateHeap
// top-level routine for creation of the PCI heap

BOOL nvPCICreateHeap
(
    void
)
{
    DWORD status;
    DWORD dwBlockStartOffset;

    dbgTracePush ("nvPCICreateHeap");

    // shouldn't create the heap more than once
    nvAssert(!pDriverData->nvD3DTexHeapData.dwHandle);

    // should only do this if AGP is not available
    nvAssert(!pDriverData->GARTLinearBase);

    // The texture heap always starts out empty.
    getDC()->nvD3DTexHeapData.dwAllocSize = 0;

    // Get the maximum size of the texture heap.
    getDC()->nvD3DTexHeapData.dwSizeMax = getDC()->nvD3DRegistryData.regPCITexHeapSize ?
                                          getDC()->nvD3DRegistryData.regPCITexHeapSize :
                                          pDriverData->nvD3DPerfData.dwRecommendedPCITexHeapSize;

    if (!(getDC()->nvD3DTexHeapData.dwSizeMax)) {
        dbgTracePop();
        return (TRUE);
    }

    do {
        // Create the texture heap.
        getDC()->nvD3DTexHeapData.dwHandle = (DWORD)HeapCreate(HEAP_SHARED, getDC()->nvD3DTexHeapData.dwSizeMax, 0);
        if (!getDC()->nvD3DTexHeapData.dwHandle) {
            dbgTracePop();
            return (FALSE);
        }

        // Allocate the entire heap up front for management.
        getDC()->nvD3DTexHeapData.dwBaseRaw = (unsigned long)HeapAlloc((HANDLE)getDC()->nvD3DTexHeapData.dwHandle, HEAP_ZERO_MEMORY, getDC()->nvD3DTexHeapData.dwSizeMax);
        if (!getDC()->nvD3DTexHeapData.dwBaseRaw)
        {
            HeapDestroy((HANDLE)getDC()->nvD3DTexHeapData.dwHandle);
            getDC()->nvD3DTexHeapData.dwHandle = (DWORD)NULL;
            dbgTracePop();
            return (FALSE);
        }

        // Get the size of the allocated heap and set the maximum heap limit. The size shouldn't be different
        // than what we asked for, but on win9x, we do the check just to be safe. (On WinNT, we can't, because
        // the function HeapSize does not exist)
#ifndef WINNT
        getDC()->nvD3DTexHeapData.dwSizeMax = (DWORD) HeapSize ((HANDLE)getDC()->nvD3DTexHeapData.dwHandle, 0, (PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwBaseRaw);
#endif

        // Align the heap properly.
        getDC()->nvD3DTexHeapData.dwBase      = ((getDC()->nvD3DTexHeapData.dwBaseRaw + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN);
        getDC()->nvD3DTexHeapData.dwSizeMax  -= (getDC()->nvD3DTexHeapData.dwBase - getDC()->nvD3DTexHeapData.dwBaseRaw);
        getDC()->nvD3DTexHeapData.dwSizeMax  &= 0xFFFFFF00;
        getDC()->nvD3DTexHeapData.dwLimitMax  = getDC()->nvD3DTexHeapData.dwSizeMax - 1;
        getDC()->nvD3DTexHeapData.dwFreeSize  = getDC()->nvD3DTexHeapData.dwSizeMax - NV_TEXTURE_PAD;
        getDC()->nvD3DTexHeapData.dwSize      = getDC()->nvD3DTexHeapData.dwSizeMax;
        getDC()->nvD3DTexHeapData.dwLimit     = getDC()->nvD3DTexHeapData.dwSize - 1;

        // Try and lock down the entire heap.
        status = D3DCreateTextureContexts (getDC()->nvD3DTexHeapData.dwBase, getDC()->nvD3DTexHeapData.dwLimit);

        if (status)
        {
            // Map the context to the frame buffer because the RM/Hardware doesn't
            // like having limts of 0.
//            D3DCreateTextureContexts (VIDMEM_ADDR(pDriverData->BaseAddress), 0xFF, 0xFF);
            HeapFree((HANDLE)getDC()->nvD3DTexHeapData.dwHandle, 0, (PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwBaseRaw);
            HeapDestroy((HANDLE)getDC()->nvD3DTexHeapData.dwHandle);
            getDC()->nvD3DTexHeapData.dwHandle  = (DWORD)NULL;
            getDC()->nvD3DTexHeapData.dwBaseRaw = (unsigned long)NULL;

            // reduce the size and try again
            getDC()->nvD3DTexHeapData.dwSizeMax -= min(getDC()->nvD3DTexHeapData.dwSizeMax, HEAP_SIZE_ADJUST);
            if (!getDC()->nvD3DTexHeapData.dwSizeMax)
            {
                nvAssert(0);
                // Can't lock down even a minimal heap.  Disable system memory textures.
                pDriverData->nvD3DPerfData.dwRecommendedPCITexHeapSize = 0;
                getDC()->nvD3DRegistryData.regPCITexHeapSize           = 0;
                getDC()->nvD3DTexHeapData.dwSizeMax                    = 0;
                getDC()->nvD3DTexHeapData.dwLimitMax                   = 0;
                dbgTracePop();
                return (FALSE);
            }
        }

    } while (!getDC()->nvD3DTexHeapData.dwHandle);

    /*
     * Align the start of the texture.
     * 1) Original way was aligning the texture heap base address then aligning each
     *    individual texture start offset off of the texture heap base address.
     */
//    dwBlockStartOffset                       = (sizeof(TEXHEAPHEADER) + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN;
    /*
     * 2) New way is to align the texture heap address and align the start address of each
     *    texture.
     */
    dwBlockStartOffset = (((getDC()->nvD3DTexHeapData.dwBase + sizeof(TEXHEAPHEADER) + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN) - getDC()->nvD3DTexHeapData.dwBase);

    // Fill in the initial block structure.
    (getDC()->nvD3DTexHeapData.dwFreeHead)                                     = (unsigned long)(getDC()->nvD3DTexHeapData.dwBase + dwBlockStartOffset - sizeof(TEXHEAPHEADER));
    (getDC()->nvD3DTexHeapData.dwFreeTail)                                     = (unsigned long)getDC()->nvD3DTexHeapData.dwFreeHead;
    ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeHead)->dwTag              = HEAP_TAG_FREE;
    ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeHead)->dwBlockSize        = getDC()->nvD3DTexHeapData.dwSizeMax - dwBlockStartOffset;
    ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeHead)->dwBlockStartOffset = dwBlockStartOffset;
    ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeHead)->pNextFreeBlock     = (PTEXHEAPHEADER)NULL;
    ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeHead)->pPrevFreeBlock     = (PTEXHEAPHEADER)NULL;
    ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeHead)->pNextAllocBlock    = (PTEXHEAPHEADER)NULL;
    ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeHead)->pPrevAllocBlock    = (PTEXHEAPHEADER)NULL;

    // Dump some information about the texture heap.
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCICreateHeap:");
    DPF_LEVEL(NVDBG_LEVEL_INFO, "   dwBase      = %08x", getDC()->nvD3DTexHeapData.dwBase);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "   dwSizeMax   = %08x", getDC()->nvD3DTexHeapData.dwSizeMax);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "   dwLimitMax  = %08x", getDC()->nvD3DTexHeapData.dwLimitMax);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "   dwSize      = %08x", getDC()->nvD3DTexHeapData.dwSize);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "   dwLimit     = %08x", getDC()->nvD3DTexHeapData.dwLimit);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "   dwFreeSize  = %08x", getDC()->nvD3DTexHeapData.dwFreeSize);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "   dwAllocSize = %08x", getDC()->nvD3DTexHeapData.dwAllocSize);

    // Return successfully.
    dbgTracePop();
    return (TRUE);
}

//-------------------------------------------------------------------------

// DestroyTextureHeap
// top-level routine for destruction of the PCI heap

BOOL nvPCIDestroyHeap
(
    void
)
{
    dbgTracePush ("nvPCIDestroyHeap");

    D3DDestroyTextureContexts (pDriverData);

    // Can't destroy a heap if there isn't one.
    nvAssert (getDC()->nvD3DTexHeapData.dwHandle);

    if (!(getDC()->nvD3DTexHeapData.dwHandle)) { return TRUE; }

    nvAssert(pDriverData->GARTLinearBase == 0);

    // Only need to free the heap if it was allocated.  (it won't be allocated on an AGP system)
    HeapFree((HANDLE)getDC()->nvD3DTexHeapData.dwHandle, 0, (PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwBaseRaw);
    HeapDestroy((HANDLE)getDC()->nvD3DTexHeapData.dwHandle);

    // Clean up a little bit.
    getDC()->nvD3DTexHeapData.dwHandle    = (DWORD)NULL;
    getDC()->nvD3DTexHeapData.dwBaseRaw   = (unsigned long)NULL;
    getDC()->nvD3DTexHeapData.dwBase      = (unsigned long)NULL;
    getDC()->nvD3DTexHeapData.dwAllocHead = (unsigned long)NULL;
    getDC()->nvD3DTexHeapData.dwAllocTail = (unsigned long)NULL;
    getDC()->nvD3DTexHeapData.dwFreeHead  = (unsigned long)NULL;
    getDC()->nvD3DTexHeapData.dwFreeTail  = (unsigned long)NULL;
    getDC()->nvD3DTexHeapData.dwSize      = 0;
    getDC()->nvD3DTexHeapData.dwSizeMax   = 0;
    getDC()->nvD3DTexHeapData.dwLimit     = 0;
    getDC()->nvD3DTexHeapData.dwLimitMax  = 0;
    getDC()->nvD3DTexHeapData.dwAllocSize = 0;
    getDC()->nvD3DTexHeapData.dwFreeSize  = 0;

    dbgTracePop();
    return (TRUE);
}

//-------------------------------------------------------------------------

DWORD nvPCIAlloc
(
    DWORD   dwSizeRequest
)
{
    DWORD           dwAllocBlockSize;
    DWORD           dwNewFreeBlockSize;
    DWORD           dwNewFreeBlockOffset;
    DWORD           dwNewFreeBlockStartOffset;
    DWORD           dwAllocLimit;
    PTEXHEAPHEADER  pAllocBlock;
    PTEXHEAPHEADER  pList;
    PTEXHEAPHEADER  pNextFree;
    PTEXHEAPHEADER  pPrevFree;
    PTEXHEAPHEADER  pNewFree;

    dbgTracePush ("nvPCIAlloc");

    // make sure we have a heap to allocate from
    nvAssert (getDC()->nvD3DTexHeapData.dwHandle);

    /*
     * If the free list is null, then there's no memory to be allocated.
     */
    if ((!getDC()->nvD3DTexHeapData.dwFreeHead)
     || (getDC()->nvD3DTexHeapData.dwFreeSize < dwSizeRequest))
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvPCIAlloc - Not enough free memory to fill request!");
        dbgTracePop();
        return (0);
    }

    /*
     * Search the free block list for a block large enough to fill this
     * request.
     */
    pAllocBlock      = (PTEXHEAPHEADER)NULL;
    dwAllocBlockSize = 0;
    pList            = (PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeHead;

    /*
     * The search will end when a block has been found that exactly matches
     * the requested size or the end of the free list has been reached. In the
     * event that an exact size match is not found, then the first block that is
     * closest to the requested size will be used.
     *
     * CHASNOTE: Potential problem with this method is that a larger block might
     * be passed up for a smaller block later in the heap and this could mean that
     * the context limit will fail shrink when it may have otherwise if the larger
     * block was used.
     */
    while ((pList)
        && (dwAllocBlockSize != dwSizeRequest))
    {
        if (pList->dwBlockSize >= dwSizeRequest)
        {
            if ((!dwAllocBlockSize)
             || (pList->dwBlockSize < dwAllocBlockSize))
            {
                pAllocBlock      = pList;
                dwAllocBlockSize = pList->dwBlockSize;
            }
        }
        pList = pList->pNextFreeBlock;
    }

    /*
     * If there was no block large enough to fill the request, return unsuccessful.
     */
    if (!pAllocBlock) {
        dbgTracePop();
        return (0);
    }

    /*
     * Get a few pointers.
     */
    pPrevFree                   = pAllocBlock->pPrevFreeBlock;
    pNextFree                   = pAllocBlock->pNextFreeBlock;
    pAllocBlock->pPrevFreeBlock = (PTEXHEAPHEADER)NULL;
    pAllocBlock->pNextFreeBlock = (PTEXHEAPHEADER)NULL;

    /*
     * Initialize the newly allocated block header and create
     * the new free block as neccessary.
     */
    pAllocBlock->dwTag       = HEAP_TAG_ALLOC;
    pAllocBlock->dwBlockSize = dwSizeRequest;

    /*
     * Allocate any new free block if the block being used for the allocation
     * is larger than the requested block size.
     */
    dwNewFreeBlockSize = dwAllocBlockSize - dwSizeRequest;

    /*
     * Adjust alignment so the next free block starts on the correct boundry for an NV3
     * texture.
     */
    if (dwNewFreeBlockSize >= (sizeof(TEXHEAPHEADER) + NV_MIN_TEXTURE_SIZE))
    {
        DWORD dwNextBlockOffset;
        DWORD dwExcess;

        dwNextBlockOffset = pAllocBlock->dwBlockStartOffset + dwSizeRequest;
        /*
         * Align the start of the texture.
         * 1) Original way was aligning the texture heap base address then aligning each
         *    individual texture start offset off of the texture heap base address.
         */
//        dwNewFreeBlockStartOffset = (dwNextBlockOffset + sizeof(TEXHEAPHEADER) + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN;
        /*
         * 2) New way is to align the texture heap address and align the start address of each
         *    texture.
         */
        dwNewFreeBlockStartOffset = ((((getDC()->nvD3DTexHeapData.dwBase + dwNextBlockOffset + sizeof(TEXHEAPHEADER)) + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN) - getDC()->nvD3DTexHeapData.dwBase);
        dwNewFreeBlockOffset      = dwNewFreeBlockStartOffset - sizeof(TEXHEAPHEADER);
        dwExcess                  = dwNewFreeBlockOffset - dwNextBlockOffset;
        dwNewFreeBlockSize       -= dwExcess;
        pAllocBlock->dwBlockSize += dwExcess;
    }

    /*
     * Now that the block has been aligned on the correct boundry, check again to make sure
     * that there's still enough room for a minimum sized texture.
     */
    if (dwNewFreeBlockSize >= (sizeof(TEXHEAPHEADER) + NV_MIN_TEXTURE_SIZE))
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIAlloc - Create new free block at end of allocation block.");

        /*
         * Create the new free block.
         */
        pNewFree                     = (PTEXHEAPHEADER)(getDC()->nvD3DTexHeapData.dwBase + dwNewFreeBlockOffset);
        pNewFree->dwTag              = HEAP_TAG_FREE;
        pNewFree->dwBlockStartOffset = dwNewFreeBlockStartOffset;
        pNewFree->dwBlockSize        = dwNewFreeBlockSize - sizeof(TEXHEAPHEADER);
        pNewFree->pNextFreeBlock     = pNextFree;
        pNewFree->pPrevFreeBlock     = pPrevFree;
        pNewFree->pNextAllocBlock    = (PTEXHEAPHEADER)NULL;
        pNewFree->pPrevAllocBlock    = (PTEXHEAPHEADER)NULL;

        /*
         * Insert the new free block into the free block list.
         */
        if (pPrevFree) {
            pPrevFree->pNextFreeBlock = pNewFree;
        }
        else {
            getDC()->nvD3DTexHeapData.dwFreeHead = (unsigned long)pNewFree;
        }

        if (pNextFree) {
            pNextFree->pPrevFreeBlock = pNewFree;
        }
        else {
            getDC()->nvD3DTexHeapData.dwFreeTail = (unsigned long)pNewFree;
        }
    }
    else
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIAlloc - Tack excess free memory  onto end of allocation block.");

        /*
         * Tack the excess memory on to the end of the block.
         */
        pAllocBlock->dwBlockSize += dwNewFreeBlockSize;

        /*
         * Remove the block from the free block list.
         */
        if (pPrevFree) {
            pPrevFree->pNextFreeBlock = pNextFree;
        }
        else {
            getDC()->nvD3DTexHeapData.dwFreeHead = (unsigned long)pNextFree;
        }

        if (pNextFree) {
            pNextFree->pPrevFreeBlock = pPrevFree;
        }
        else {
            getDC()->nvD3DTexHeapData.dwFreeTail = (unsigned long)pPrevFree;
        }
    }

    /*
     * Update the allocation sizes.
     */
    getDC()->nvD3DTexHeapData.dwAllocSize += pAllocBlock->dwBlockSize;
    getDC()->nvD3DTexHeapData.dwFreeSize  -= pAllocBlock->dwBlockSize;

    /*
     * Insert the newly allocated block into the allocation list.
     * First handle the best case scenarios, the block is either the first block
     * to be allocated, the block comes before the current head of the allocation
     * list, or the block comes after the current tail of the allocation list.
     * In these cases, its simply a case of updating a few pointers.
     */
    if (!getDC()->nvD3DTexHeapData.dwAllocHead)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIAlloc - Insert new allocation block at head of allocation list. First time.");
        pAllocBlock->pPrevAllocBlock          = (PTEXHEAPHEADER)NULL;
        pAllocBlock->pNextAllocBlock          = (PTEXHEAPHEADER)NULL;
        getDC()->nvD3DTexHeapData.dwAllocHead = (unsigned long)pAllocBlock;
        getDC()->nvD3DTexHeapData.dwAllocTail = (unsigned long)pAllocBlock;

        /*
         * The tail of allocation list has changed. Check to see if the texture context
         * needs to be udpated to reflect the change.
         */
        if (getDC()->nvD3DTexHeapData.dwAllocTail) {
            dwAllocLimit = ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwAllocTail)->dwBlockStartOffset +
                           ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwAllocTail)->dwBlockSize - 1;
        }
        else {
            dwAllocLimit = 0;
        }
    }
    else if (pAllocBlock < (PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwAllocHead)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIAlloc - Insert new allocation block at head of allocation list.");
        ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwAllocHead)->pPrevAllocBlock = pAllocBlock;
        pAllocBlock->pPrevAllocBlock          = (PTEXHEAPHEADER)NULL;
        pAllocBlock->pNextAllocBlock          = (PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwAllocHead;
        getDC()->nvD3DTexHeapData.dwAllocHead = (unsigned long)pAllocBlock;
    }
    else if (pAllocBlock > (PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwAllocTail)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIAlloc - Insert new allocation block at tail of allocation list.");
        ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwAllocTail)->pNextAllocBlock = pAllocBlock;
        pAllocBlock->pPrevAllocBlock          = (PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwAllocTail;
        pAllocBlock->pNextAllocBlock          = (PTEXHEAPHEADER)NULL;
        getDC()->nvD3DTexHeapData.dwAllocTail = (unsigned long)pAllocBlock;

        /*
         * The tail of allocation list has changed. Check to see if the texture context
         * needs to be udpated to reflect the change.
         */
        if (getDC()->nvD3DTexHeapData.dwAllocTail) {
            dwAllocLimit = ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwAllocTail)->dwBlockStartOffset +
                           ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwAllocTail)->dwBlockSize - 1;
        }
        else {
            dwAllocLimit = 0;
        }
    }
    else
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIAlloc - Insert new allocation block in middle of allocation list.");

        /* WORST CASE
         * The newly allocated block falls between the head and tail of the allocation
         * list. Search all memory blocks following the new allocation block to find the
         * next allocated block, then just update the pointers.
         */
        pList = (PTEXHEAPHEADER)((DWORD)pAllocBlock + sizeof(TEXHEAPHEADER) + pAllocBlock->dwBlockSize);
        while ((pList) && (pList->dwTag != HEAP_TAG_ALLOC))
        {
            pList = (PTEXHEAPHEADER)((DWORD)pList + sizeof(TEXHEAPHEADER) + pList->dwBlockSize);
        }
        /*
         * This should never happen, if it does, then we've got problems.
         */
        if (!pList)
        {
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvPCIAlloc - Error inserting new allocation block!!");
            dbgTracePop();
            return (pAllocBlock->dwBlockStartOffset);
        }

        /*
         * Update the pointers.
         */
        pAllocBlock->pPrevAllocBlock                  = pList->pPrevAllocBlock;
        pAllocBlock->pNextAllocBlock                  = pList;
        pList->pPrevAllocBlock                        = pAllocBlock;
        pAllocBlock->pPrevAllocBlock->pNextAllocBlock = pAllocBlock;
    }

    /*
     * Dump some information about the texture heap allocation.
     */
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIAlloc - pAllocBlock->dwBlockStartOffset = %08x", (DWORD)pAllocBlock->dwBlockStartOffset);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIAlloc - dwSize                          = %08x", getDC()->nvD3DTexHeapData.dwSize);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIAlloc - dwLimit                         = %08x", getDC()->nvD3DTexHeapData.dwLimit);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIAlloc - dwFreeSize                      = %08x", getDC()->nvD3DTexHeapData.dwFreeSize);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIAlloc - dwAllocSize                     = %08x", getDC()->nvD3DTexHeapData.dwAllocSize);

    /*
     * Return the offset to the allocated block.
     */
    dbgTracePop();
    return (pAllocBlock->dwBlockStartOffset);
}

//-------------------------------------------------------------------------

BOOL nvPCIFree
(
    DWORD   dwFreeBlockOffset
)
{
    DWORD           dwAllocLimit;
    PTEXHEAPHEADER  pList;
    PTEXHEAPHEADER  pFreeBlock;
    PTEXHEAPHEADER  pPrevFree;
    PTEXHEAPHEADER  pNextFree;
    PTEXHEAPHEADER  pPrevAlloc;
    PTEXHEAPHEADER  pNextAlloc;

    dbgTracePush ("nvPCIFree");

    /*
     * Make sure there is a valid texture heap to free the memory from.
     */
    if (!getDC()->nvD3DTexHeapData.dwBase) {
        dbgTracePop();
        return (TRUE);
    }

    /*
     * Make sure there is actually a valid texture block to free.
     */
    if (!dwFreeBlockOffset) {
        dbgTracePop();
        return (FALSE);
    }

    /*
     * Calculate the pointer to the header of the block being freed.
     */
    pFreeBlock = (PTEXHEAPHEADER)(getDC()->nvD3DTexHeapData.dwBase + (dwFreeBlockOffset - sizeof(TEXHEAPHEADER)));
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIFree - pFreeBlock->dwBlockSize = %08x", pFreeBlock->dwBlockSize);

    /*
     * Make sure we are pointer to an actual allocated block header.
     */
    if (pFreeBlock->dwTag != HEAP_TAG_ALLOC) {
        dbgTracePop();
        return (FALSE);
    }

    /*
     * Get a few pointers.
     */
    pPrevAlloc                  = pFreeBlock->pPrevAllocBlock;
    pNextAlloc                  = pFreeBlock->pNextAllocBlock;
    pFreeBlock->pPrevAllocBlock = (PTEXHEAPHEADER)NULL;
    pFreeBlock->pNextAllocBlock = (PTEXHEAPHEADER)NULL;

    /*
     * Remove the block from the allocation list.
     */
    if (pPrevAlloc)
        pPrevAlloc->pNextAllocBlock = pNextAlloc;
    else
        getDC()->nvD3DTexHeapData.dwAllocHead = (unsigned long)pNextAlloc;
    if (pNextAlloc)
        pNextAlloc->pPrevAllocBlock = pPrevAlloc;
    else
    {
        getDC()->nvD3DTexHeapData.dwAllocTail = (unsigned long)pPrevAlloc;

        /*
         * The tail of allocation list has changed. Check to see if the texture context
         * needs to be udpated to reflect the change.
         */
        if (getDC()->nvD3DTexHeapData.dwAllocTail)
            dwAllocLimit = ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwAllocTail)->dwBlockStartOffset + ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwAllocTail)->dwBlockSize - 1;
        else
            dwAllocLimit = 0;
    }

    /*
     * Mark the block as freed and get the size of the block.
     */
    pFreeBlock->dwTag = HEAP_TAG_FREE;

    /*
     * Update the allocation sizes.
     */
    getDC()->nvD3DTexHeapData.dwAllocSize -= pFreeBlock->dwBlockSize;
    getDC()->nvD3DTexHeapData.dwFreeSize  += pFreeBlock->dwBlockSize;

    /*
     * Insert the newly freed block into the free list.
     */
    if (!getDC()->nvD3DTexHeapData.dwFreeHead)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIFree - Insert new free block at head of free list. First time.");
        pFreeBlock->pPrevFreeBlock    = (PTEXHEAPHEADER)NULL;
        pFreeBlock->pNextFreeBlock    = (PTEXHEAPHEADER)NULL;
        getDC()->nvD3DTexHeapData.dwFreeHead = (unsigned long)pFreeBlock;
        getDC()->nvD3DTexHeapData.dwFreeTail = (unsigned long)pFreeBlock;
    }
    else if (pFreeBlock < (PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeHead)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIFree - Insert new free block at head of free list.");
        ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeHead)->pPrevFreeBlock = pFreeBlock;
        pFreeBlock->pPrevFreeBlock           = (PTEXHEAPHEADER)NULL;
        pFreeBlock->pNextFreeBlock           = (PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeHead;
        getDC()->nvD3DTexHeapData.dwFreeHead = (unsigned long)pFreeBlock;
    }
    else if (pFreeBlock > (PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeTail)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIFree - Insert new free block at tail of free list.");
        ((PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeTail)->pNextFreeBlock = pFreeBlock;
        pFreeBlock->pPrevFreeBlock           = (PTEXHEAPHEADER)getDC()->nvD3DTexHeapData.dwFreeTail;
        pFreeBlock->pNextFreeBlock           = (PTEXHEAPHEADER)NULL;
        getDC()->nvD3DTexHeapData.dwFreeTail = (unsigned long)pFreeBlock;
    }
    else
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIFree - Insert new free block in middle of free list.");

        /* WORST CASE
         * The newly freed block falls between the head and tail of the free
         * list. Search all memory blocks following the new freed block to find the
         * next free block, then just update the pointers.
         */
        pList = (PTEXHEAPHEADER)((DWORD)pFreeBlock + sizeof(TEXHEAPHEADER) + pFreeBlock->dwBlockSize);
        while ((pList)
            && (pList->dwTag != HEAP_TAG_FREE))
        {
            pList = (PTEXHEAPHEADER)((DWORD)pList + sizeof(TEXHEAPHEADER) + pList->dwBlockSize);
        }
        /*
         * This should never happen, if it does, then we've got problems.
         */
        if (!pList)
        {
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvPCIFree - Error inserting newly freed block!!");
            dbgTracePop();
            return (TRUE);
        }

        /*
         * Update the pointers.
         */
        pFreeBlock->pPrevFreeBlock                 = pList->pPrevFreeBlock;
        pFreeBlock->pNextFreeBlock                 = pList;
        pList->pPrevFreeBlock                      = pFreeBlock;
        pFreeBlock->pPrevFreeBlock->pNextFreeBlock = pFreeBlock;
    }

    /*
     * Do some basic garbage collection to try and consolidate contiguous free
     * blocks.
     */
    pPrevFree = pFreeBlock->pPrevFreeBlock;
    pNextFree = pFreeBlock->pNextFreeBlock;

    if ((pPrevFree)
     && (((DWORD)pPrevFree + sizeof(TEXHEAPHEADER) + pPrevFree->dwBlockSize) == (DWORD)pFreeBlock))
    {
        /*
         * Merge the previous free block with the new free block.
         */
        pPrevFree->dwBlockSize += sizeof(TEXHEAPHEADER) + pFreeBlock->dwBlockSize;
        if (pPrevFree->pNextFreeBlock = pNextFree) {
            pNextFree->pPrevFreeBlock = pPrevFree;
        }
        else {
            getDC()->nvD3DTexHeapData.dwFreeTail = (unsigned long)pPrevFree;
        }

        /*
         * Clean out the old free block header.
         */
        pFreeBlock->dwTag              = 0;
        pFreeBlock->dwBlockSize        = 0;
        pFreeBlock->dwBlockStartOffset = 0;
        pFreeBlock->pPrevFreeBlock     = (PTEXHEAPHEADER)NULL;
        pFreeBlock->pNextFreeBlock     = (PTEXHEAPHEADER)NULL;
        pFreeBlock->pPrevAllocBlock    = (PTEXHEAPHEADER)NULL;
        pFreeBlock->pNextAllocBlock    = (PTEXHEAPHEADER)NULL;

        /*
         * Update the free block pointer to point to the new consolidated block.
         */
        pFreeBlock = pPrevFree;
    }

    if ((pNextFree)
     && (((DWORD)pFreeBlock + sizeof(TEXHEAPHEADER) + pFreeBlock->dwBlockSize) == (DWORD)pNextFree))
    {
        /*
         * Merge the new free block with the next free block.
         */
        pFreeBlock->dwBlockSize += sizeof(TEXHEAPHEADER) + pNextFree->dwBlockSize;
        if (pFreeBlock->pNextFreeBlock = pNextFree->pNextFreeBlock)
            pNextFree->pNextFreeBlock->pPrevFreeBlock = pFreeBlock;
        else
            getDC()->nvD3DTexHeapData.dwFreeTail = (unsigned long)pFreeBlock;

        /*
         * Clean out the old free block header.
         */
        pNextFree->dwTag              = 0;
        pNextFree->dwBlockSize        = 0;
        pNextFree->dwBlockStartOffset = 0;
        pNextFree->pPrevFreeBlock     = (PTEXHEAPHEADER)NULL;
        pNextFree->pNextFreeBlock     = (PTEXHEAPHEADER)NULL;
        pNextFree->pPrevAllocBlock    = (PTEXHEAPHEADER)NULL;
        pNextFree->pNextAllocBlock    = (PTEXHEAPHEADER)NULL;
    }

    /*
     * Dump some information about the texture heap allocation.
     */
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIFree - dwSize      = %08x", getDC()->nvD3DTexHeapData.dwSize);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIFree - dwLimit     = %08x", getDC()->nvD3DTexHeapData.dwLimit);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIFree - dwFreeSize  = %08x", getDC()->nvD3DTexHeapData.dwFreeSize);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvPCIFree - dwAllocSize = %08x", getDC()->nvD3DTexHeapData.dwAllocSize);

    /*
     * Return successfully.
     */
    dbgTracePop();
    return (TRUE);
}

//-------------------------------------------------------------------------

#ifdef NV_TRACKAGP

#undef DDHAL32_VidMemAlloc
#undef DDHAL32_VidMemFree

FLATPTR EXTERN_DDAPI DDHAL32_VidMemAlloc (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, DWORD dwXDim, DWORD dwYDim);
void EXTERN_DDAPI DDHAL32_VidMemFree (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, FLATPTR ptr);

typedef struct
{
    FLATPTR ptr;
    DWORD size;
} TABLE;

static TABLE *lpTable = NULL;
static DWORD dwMax    = 0;
static DWORD dwTotal  = 0;

//-------------------------------------------------------------------------

FLATPTR EXTERN_DDAPI __DDHAL32_VidMemAlloc
(
    LPDDRAWI_DIRECTDRAW_GBL pdrv,
    int heap,
    DWORD dwXDim,
    DWORD dwYDim
)
{
    dbgTracePush ("__DDHAL32_VidMemAlloc");

    FLATPTR ptr = DDHAL32_VidMemAlloc (pdrv, heap, dwXDim, dwYDim);

    if (ptr && (heap == AGP_HEAP))
    {
        DWORD i;

        dwTotal += dwXDim * dwYDim;
        DPF ("a %d", dwTotal);

        for (i = 0; i < dwMax; i++)
        {
            if (lpTable[i].ptr == (FLATPTR)NULL)
            {
                lpTable[i].ptr = ptr;
                lpTable[i].size = dwXDim * dwYDim;
                dbgTracePop();
                return ptr;
            }
        }
        dwMax += 128;
        lpTable = (TABLE*)ReallocIPM(lpTable, dwMax * sizeof(TABLE));
        lpTable[i].ptr = ptr;
        lpTable[i].size = dwXDim * dwYDim;
        dbgTracePop();
        return ptr;
    }

    dbgTracePop();
    return ptr;
}

//-------------------------------------------------------------------------

void EXTERN_DDAPI __DDHAL32_VidMemFree
(
    LPDDRAWI_DIRECTDRAW_GBL pdrv,
    int heap,
    FLATPTR ptr
)
{
    dbgTracePush ("__DDHAL32_VidMemFree");

    if (heap == AGP_HEAP)
    {
        DWORD i;

        for (i = 0; i<dwMax; i++)
        {
            if (lpTable[i].ptr == ptr)
            {
                dwTotal -= lpTable[i].size;
                lpTable[i].ptr = (FLATPTR)NULL;

                DPF ("f %d", dwTotal);

                DDHAL32_VidMemFree (pdrv, heap, ptr);
                dbgTracePop();
                return;
            }
        }

        // should not get here
        __asm int 3;
    }

    DDHAL32_VidMemFree (pdrv, heap, ptr);
    dbgTracePop();
}

//-------------------------------------------------------------------------

void __Reset (void)
{
    dbgTracePush ("__Reset");

    FreeIPM (lpTable);
    lpTable = NULL;
    dwTotal = dwMax = 0;
    DPF ("reset");

    dbgTracePop();
}

#endif //NV_TRACKAGP

#endif  // NVARCH >= 0x04

