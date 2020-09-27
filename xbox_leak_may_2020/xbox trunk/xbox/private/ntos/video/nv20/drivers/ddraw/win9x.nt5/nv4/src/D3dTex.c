/*
 * Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 * (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: D3DTEX.C                                                          *
*   The Direct 3d texture routines.                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/02/97 - re-wrote and cleaned up.     *
*       Ben de Waal (bdw)           08/28/97 - enabled locked tex status    *
*                                   09/19/97 - optimize mipmap creation     *
*                                                                           *
\***************************************************************************/

#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "nvddobj.h"
#include "d3dinc.h"
#include "nv3ddbg.h"

#ifndef WINNT
FLATPTR EXTERN_DDAPI DDHAL32_VidMemAlloc (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, DWORD dwXDim, DWORD dwYDim);
void EXTERN_DDAPI DDHAL32_VidMemFree (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, FLATPTR ptr);
#endif // #ifndef WINNT

/*
 * Some statistical data.
 */

// omnitemporal texture counts
DWORD   statTextureOmniCountTotal      = 0;
DWORD   statTextureOmniCountPalette    = 0;

// omnitemporal texture sizes
DWORD   statTextureOmniSizeTotal       = 0;
DWORD   statTextureOmniSizePalette     = 0;

// extant texture counts
DWORD   statTextureCountTotal          = 0;
DWORD   statTextureCountVideo          = 0;
DWORD   statTextureCountAgpHeap        = 0;
DWORD   statTextureCountPciHeap        = 0;
DWORD   statTextureCountCache          = 0;

// extant texture sizes
DWORD   statTextureSizeTotal           = 0;
DWORD   statTextureSizeVideo           = 0;
DWORD   statTextureSizeAgpHeap         = 0;
DWORD   statTextureSizePciHeap         = 0;
DWORD   statTextureSizeCache           = 0;

// texture dimensions
DWORD   statTextureDimensionMinVideo   = 2048;
DWORD   statTextureDimensionMinAgpHeap = 2048;
DWORD   statTextureDimensionMinPciHeap = 2048;
DWORD   statTextureDimensionMinCache   = 2048;
DWORD   statTextureDimensionMaxVideo   = 0;
DWORD   statTextureDimensionMaxAgpHeap = 0;
DWORD   statTextureDimensionMaxPciHeap = 0;
DWORD   statTextureDimensionMaxCache   = 0;

/*
 * --------------------------------------------------------------------------
 * Miscellaneous D3D texture related routines.
 * --------------------------------------------------------------------------
 */
BOOL nvCreateTextureListHeap
(
    void
)
{
    /*
     * Create a global heap to allocate internal D3D Texture data structures out of.
     * The number of D3D Textures available can be adjusted by a setting in the registry.
     * It is set reasonably large by default, so hopefully the registry override will never
     * be needed....but just in case.
     */
#ifndef OLDSTUFF
    pDriverData->hTextures_gbl = (DWORD)HEAPCREATE((pDriverData->regD3DTextureMax * sizeof(NVD3DTEXTURE)));
#else
    pDriverData->hTextures_gbl = (DWORD)HeapCreate(HEAP_SHARED,
                                                   (pDriverData->regD3DTextureMax * sizeof(NVD3DTEXTURE)),
                                                   0);
#endif
    if (!pDriverData->hTextures_gbl)
    {
        dbgD3DError();
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureListHeap - Could not create texture list heap!");
        return (FALSE);
    }
    return (TRUE);
}
BOOL nvDestroyTextureListHeap
(
    void
)
{
    PNVD3DTEXTURE               pTexture;
    PNVD3DTEXTURE               pTextureNext;

    /*
     * If there's no heap currently allocated, then there is no heap to destroy.
     */
    if (!pDriverData->hTextures_gbl)
        return (TRUE);

    /*
     * Free any textures that are still allocated.
     * This will hopefully never happen, because it gets quite complicated if the texture
     * surface that this texture structure is associated with has not been destroyed yet.
     */
    pTexture = (PNVD3DTEXTURE)pDriverData->dwTextureListHead;

    while (pTexture)
    {
        /*
         * Get the pointer to the next texture in the list before actually freeing the
         * memory for this structure.
         */
        pTextureNext = pTexture->pTextureNext;

        /*
         * Before freeing the memory for this texture structure, we will need to get the
         * pointer to the texture surface so that we can zero out the reserved field
         * containing the handle for this structure.  That is, if the texture surface field
         * is still valid.
         */


        /*
         * If the surface is no longer allocated, but the user texture memory is, then
         * free up the memory now.
         */
#ifdef NV_TEX2
        if (pTexture->dwLinearAddr)
        {
            nvTextureFreeLinearSurface (pTexture);
        }
#else // !NV_TEX2
        if ((!(pTexture->dwTextureFlags & NV3_TEXTURE_SURFACE_ALLOCATED))
         && (pTexture->dwTextureFlags & NV3_TEXTURE_USER_ALLOCATED))
        {
            GlobalUnlock(pTexture->hUserTexture);
            GlobalFree(pTexture->hUserTexture);
            pTexture->hUserTexture    = (HANDLE)NULL;
            pTexture->fpUserTexture   = (FLATPTR)NULL;
            pTexture->dwTextureFlags &= ~NV3_TEXTURE_USER_ALLOCATED;
        }
#endif // NV_TEX2

        /*
         * No matter what, if the internal texture memory is still allocated, then
         * free it now.
         */
#ifdef NV_TEX2
        if (TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags))
        {
            nvTextureFreeSwizzleSurface (pTexture);
        }
#else // !NV_TEX2
        if (pTexture->dwTextureFlags & NV3_TEXTURE_INTERNAL_ALLOCATED)
        {
            PNVD3DCONTEXT   pContext;

            /*
             * Free the internal texture memory.
             */
            pContext = (PNVD3DCONTEXT)pTexture->hContext;
            nvDestroyInternalTexture(pContext, pTexture);
        }
#endif // NV_TEX2

        /*
         * Zero out the handle field in the texture surface structure if there is still
         * a valid pointer to the surface (which there should only be if the surface
         * wasn't destroyed yet.
         */
        NV_LOCK_TEXTURE_SURFACE(pTexture);
        if ((pTexture->lpLcl)
#ifndef NV_TEX2
         && (pTexture->dwTextureFlags & NV3_TEXTURE_SURFACE_ALLOCATED)
#endif
         ) pTexture->lpLcl->dwReserved1 = (DWORD)NULL;
        NV_UNLOCK_TEXTURE_SURFACE(pTexture);

        /*
         * Zero out the memory so that there's no question about the contents.
         */
        memset(pTexture, 0, sizeof(PNVD3DTEXTURE));

        /*
         * Alrighty, go ahead and free the memory for this texture structure.
         */
#ifndef OLDSTUFF
        HEAPFREE(pDriverData->hTextures_gbl, pTexture);
#else
        HeapFree((HANDLE)pDriverData->hTextures_gbl, 0, pTexture);
#endif
        pDriverData->dwD3DTextureCount--;
        pTexture = pTextureNext;
    }

    /*
     * Destroy the heap.
     */
#ifndef OLDSTUFF
    HEAPDESTROY(pDriverData->hTextures_gbl);
#else
    HeapDestroy((HANDLE)pDriverData->hTextures_gbl);
#endif
    pDriverData->hTextures_gbl = (DWORD)NULL;
    return (TRUE);
}
BOOL nvDestroyTextureStructure
(
    PNVD3DTEXTURE   pTexture
)
{
    /*
     * NOTE: This routine assumes only valid (non-null) texture pointers will be
     *       passed in!!
     * Is there still a surface allocated?
     * This can vary.  It is possible that a structure has been destroyed already
     * (i.e. this is common in the case of full screen applications being minimized
     * and restored)
     */
#ifdef NV_TEX2
    if (!TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags) && !pTexture->dwLinearAddr)
#else
#ifdef WINNT
    /*
     * Under WINNT, free up user copy of texture since we are now running
     * in the context of the process which created it (we hope).
     */
    if (pTexture->dwTextureFlags & NV3_TEXTURE_USER_ALLOCATED)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvDestroyTextureSurface - Free User Texture = %08lx", pTexture->fpUserTexture);
        pTexture->dwTextureFlags &= ~NV3_TEXTURE_USER_ALLOCATED;
        GlobalUnlock(pTexture->hUserTexture);
        GlobalFree(pTexture->hUserTexture);
    }
#endif // #ifdef WINNT
    if (!(pTexture->dwTextureFlags & NV3_TEXTURE_SURFACE_ALLOCATED))
#endif
    {
        /*
         * Perform all neccessary context cleanup.
         */
        pTexture->pid             = 0L;
        pTexture->hContext        = 0L;

        /*
         * The internal data structure has been destroyed, make sure to clear the driver
         * reserved field out so that we don't try and access an invalid internal texture
         * structure.
         * REMOVING THIS LINE IS BAD FOR BENCHMARK TEST 10 AND 19.
         */
        NV_LOCK_TEXTURE_SURFACE(pTexture);
        if (pTexture->lpLcl)
            pTexture->lpLcl->dwReserved1 = 0;
        NV_UNLOCK_TEXTURE_SURFACE(pTexture);

        /*
         * Free the context from the array.
         */
        if (pTexture->pTexturePrev)
            pTexture->pTexturePrev->pTextureNext = pTexture->pTextureNext;
        if (pTexture->pTextureNext)
            pTexture->pTextureNext->pTexturePrev = pTexture->pTexturePrev;
        if (pDriverData->dwTextureListHead == (DWORD)pTexture)
            pDriverData->dwTextureListHead = (DWORD)pTexture->pTextureNext;
#ifndef OLDSTUFF
        HEAPFREE(pDriverData->hTextures_gbl, pTexture);
#else
        HeapFree((HANDLE)pDriverData->hTextures_gbl, 0, (PVOID)pTexture);
#endif
        pDriverData->dwD3DTextureCount--;
    }
    else
    {
        /*
         * Return FALSE if the texture structure was not deallocated.
         */
        return (FALSE);
    }

    /*
     * Return TRUE if the texture structure was actually destroyed.
     */
    return (TRUE);
}
BOOL nvDestroyTextureProcess
(
    DWORD   pid
)
{
    PNVD3DTEXTURE   pTexture;
    PNVD3DTEXTURE   pTextureNext;

    /*
     * Start at the head of the texture list.
     */
    pTexture = (PNVD3DTEXTURE)pDriverData->dwTextureListHead;

    /*
     * Search for all textures belonging to this process.
     */
    while (pTexture)
    {
        /*
         * Get the pointer to the next texture in the list before possibly
         * destroying this texture structure.
         */
        pTextureNext = pTexture->pTextureNext;

        /*
         * Destroy this texture if it belongs to this process.
         */
        if (pTexture->pid == pid)
        {
            /*
             * Mark the texture handle as being deallocated.
             */
#ifdef NV_TEX2
            pTexture->dwTextureFlags &= ~NV4_TEXFLAG_HANDLE_ALLOCATED;
            if (!TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags))
#else
            pTexture->dwTextureFlags &= ~NV3_TEXTURE_HANDLE_ALLOCATED;
            if (!(pTexture->dwTextureFlags & NV3_TEXTURE_SURFACE_ALLOCATED))
#endif
                pTexture->lpLcl = 0;

            nvDestroyTextureStructure(pTexture);
        }

        /*
         * Set the current texture to the next texture in the list.
         */
        pTexture = pTextureNext;
    }
    return (TRUE);
}
BOOL nvDestroyTextureContext
(
    DWORD   hContext
)
{
    PNVD3DTEXTURE   pTexture;
    PNVD3DTEXTURE   pTextureNext;

    /*
     * Start at the head of the texture list.
     */
    pTexture = (PNVD3DTEXTURE)pDriverData->dwTextureListHead;

    /*
     * Search for all textures belonging to this context.
     */
    while (pTexture)
    {
        /*
         * Get the pointer to the next texture in the list before possibly
         * destroying this texture structure.
         */
        pTextureNext = pTexture->pTextureNext;

        /*
         * Destroy this texture if it belongs to this process.
         */
        if (pTexture->hContext == hContext)
        {
#ifndef WINNT
            /*
             * Mark the texture handle as being deallocated.
             */
#ifdef NV_TEX2
            pTexture->dwTextureFlags &= ~NV4_TEXFLAG_HANDLE_ALLOCATED;
#else
            pTexture->dwTextureFlags &= ~NV3_TEXTURE_HANDLE_ALLOCATED;
#endif
            nvDestroyTextureStructure(pTexture);
#else
            /*
             * We might not be running in the proper context to
             * free the user mode copy of the texture. Defer
             * destruction of the surface til the texture is
             * explicitly destroyed via DestroySurface. Get rid
             * of the context ptr as the context is about to
             * be destroyed.
             */
            pTexture->hContext = 0;
#endif // #ifndef WINNT
        }

        /*
         * Set the current texture to the next texture in the list.
         */
        pTexture = pTextureNext;
    }
    return (TRUE);
}

/*
 * --------------------------------------------------------------------------
 * D3D HAL texture callback routines
 * --------------------------------------------------------------------------
 */
DWORD __stdcall nvTextureCreate
(
    LPD3DHAL_TEXTURECREATEDATA  ptcd
)
{
    HDDRVITEM                   hTexture;
    PNVD3DTEXTURE               pTexture = 0;
    LPDDRAWI_DDRAWSURFACE_LCL   lpLcl;

    /*
     * NOTES:
     *
     * This callback is invoked when a texture is to be created from a
     * DirectDrawSurface.
     * We must pass back the created handle.
     *
     * Get the pointer to the context this texture is associated with.
     */
    DPF_LEVEL (NVDBG_LEVEL_DDI_ENTRY, "nvTextureCreate - hContext = %08lx", ptcd->dwhContext);
    pCurrentContext = (PNVD3DCONTEXT)ptcd->dwhContext;
    if (!pCurrentContext)
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureCreate - Bad Context");
        dbgD3DError();
        ptcd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Validate the parameters of the texture being created.
     */
#ifndef WINNT
    lpLcl = ((LPDDRAWI_DDRAWSURFACE_INT)ptcd->lpDDS)->lpLcl;
#else
     lpLcl = EngLockDirectDrawSurface(ptcd->hDDS);
#endif // #ifndef WINNT

    if (!lpLcl)
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, ":nvTextureCreate - Texture Create Failed");
        dbgD3DError();
        ptcd->ddrval = D3DERR_TEXTURE_CREATE_FAILED;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Get pointer to global driver. data structure.
     */
    NvSetDriverDataPtrFromContext(pCurrentContext);

    /*
     * Setup frequently accessed globals.
     */
    NV_D3D_GLOBAL_SETUP();

    /*
     * Try and get a texture pointer based on the value stored in the local surface structure
     * driver reserved field.
     *
     * DUMB APPLICATION ALERT!!!!!
     * This code is going to look really strange, but there is a reason.  Believe it or not,
     * an application actually exists (Nightmare Creatures by Kalisto) that does not work if
     * the handle returned has bit 31 set.  Because of where the memory for our texture structures
     * is located, the high bit is always set, but I don't want to just blindly assume that this
     * is always going to be the case.  So, what I'm going to do is mask off the high bit of the
     * texture structure and return the remaining 31 bits as the handle, then in places where the
     * application passes in a texture handle, I will take the pointer to the head of the texture
     * list, mask off all but the high bit and OR it in with the handle to get the pointer to the
     * texture structure.  This should be safe since the texture structures are all allocated from
     * a linearly contiguous heap.
     *
     * bdw: except if this heap straddles the 2GB point where the texture will live with bit 31 set
     *      and the heap base with bit 31 not set. Windows architecture will of course not do such a
     *      thing since the 2GB point splits app and shared memory.
     */
    hTexture = lpLcl->dwReserved1 & 0x7FFFFFFF;
    pTexture = (PNVD3DTEXTURE)lpLcl->dwReserved1;

    /*
     * Is the internal texture data structure already allocated for this texture?
     */
    if (!pTexture)
    {
        /*
         * The internal texture data structure has not been allocated yet. This can
         * happen if the application allocates it's texture handles before it creates the
         * texture surfaces.  (Ziff Davis 3D Winbench)
         * So we have to handle both cases.  If the data structure has not been allocated
         * yet, allocate it here.
         */
#ifndef OLDSTUFF
        hTexture = (HDDRVITEM)HEAPALLOC(pDriverData->hTextures_gbl, sizeof(NVD3DTEXTURE));
#else
        hTexture = (HDDRVITEM)HeapAlloc((HANDLE)pDriverData->hTextures_gbl,
                                        HEAP_ZERO_MEMORY,
                                        sizeof(NVD3DTEXTURE));
#endif
        pTexture = (PNVD3DTEXTURE)hTexture;
        if (!pTexture)
        {
            /*
             * Bad news.  Could not allocate the internal data structure.  No choice but to
             * return an error.
             */
            dbgD3DError();
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureCreate - Bad Texture");
            ptcd->ddrval = D3DERR_TEXTURE_CREATE_FAILED;
            return (DDHAL_DRIVER_HANDLED);
        }
        pDriverData->dwD3DTextureCount++;

        /*
         * Store the handle of the internal texture structure in the local surface structure
         * driver reserved field.
         */
        lpLcl->dwReserved1 = hTexture;

        /*
         * Make sure a few key fields are intialized to zero.
         */
/* allocated with HEAP_ZERO_MEMORY - waste of time
        pTexture->hMipBaseTexture  = 0;
        pTexture->dwTextureFlags   = 0;
        pTexture->dwTexturePointer = 0;
        pTexture->dwTextureOffset  = 0;
        pTexture->fpTexture        = 0;
        pTexture->hUserTexture     = 0;
        pTexture->fpUserTexture    = 0;
*/
        pTexture->dwDriverData     = (DWORD)pDriverData;

        /*
         * Update the texture list.
         */
        pTexture->pTexturePrev = (PNVD3DTEXTURE)NULL;
        pTexture->pTextureNext = (PNVD3DTEXTURE)pDriverData->dwTextureListHead;
        if (pTexture->pTextureNext)
            pTexture->pTextureNext->pTexturePrev = pTexture;
        pDriverData->dwTextureListHead = (DWORD)pTexture;
    }

    /*
     * Return the new texture handle.
     */
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvTextureCreate - New texture handle = %08lx", hTexture);
    ptcd->dwHandle = hTexture;

    /*
     * Initialize the new texture header.
     */
    pTexture->pid      = pCurrentContext->pid;
    pTexture->hContext = ptcd->dwhContext;
    pTexture->lpLcl    = lpLcl;
#ifndef WINNT
    pTexture->lpDDS    = ptcd->lpDDS;
#else
    pTexture->hDDS = ptcd->hDDS;
#endif  // #ifndef WINNT

    /*
     * A handle is only ever allocated for the top most level of the mipmap chain.
     * run down the chain and updated the mipmap base texture for each mipmap.
     */
    if ((lpLcl->ddsCaps.dwCaps & DDSCAPS_MIPMAP)
     && (!lpLcl->lpAttachListFrom)
     && (lpLcl->lpAttachList))
    {
        LPDDRAWI_DDRAWSURFACE_LCL   lclMip;
        lclMip = lpLcl->lpAttachList->lpAttached;
        while (lclMip)
        {
            PNVD3DTEXTURE   pMipMap;

            pMipMap = (PNVD3DTEXTURE)lclMip->dwReserved1;
            if (pMipMap)
            {
                pMipMap->hMipBaseTexture = (DWORD)pTexture;
#ifdef NV_TEX2
                pMipMap->dwTextureFlags |= NV4_TEXFLAG_USER_MIP_LEVEL;
#else
                pMipMap->dwTextureFlags |= NV3_TEXTURE_USER_MIP_LEVEL;
#endif
            }
            if (lclMip->lpAttachList)
                lclMip = lclMip->lpAttachList->lpAttached;
            else
                lclMip = 0;
        }
    }

    /*
     * If this is not a user mipmap level then see if the internal texture
     * memory needs to be re-allocated.  For user mip maps, only the base
     * level
     */
#ifdef NV_TEX2
    if (!(pTexture->dwTextureFlags & NV4_TEXFLAG_USER_MIP_LEVEL))
#else
    if (!(pTexture->dwTextureFlags & NV3_TEXTURE_USER_MIP_LEVEL))
#endif
    {
        /*
         * We don't normally want to allocate the internal texture memory here, but there are
         * situations where we need to try.
         * First, if the surface is already allocated but there is no internal memory allocated
         * for it.  Then try and allocate it.
         */
#ifdef NV_TEX2
        if (!TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags)
          && pTexture->dwMipMapSizeBytes)
#else
        if ((pTexture->dwTextureFlags & NV3_TEXTURE_SURFACE_ALLOCATED)
         && (!(pTexture->dwTextureFlags & NV3_TEXTURE_INTERNAL_ALLOCATED)))
#endif
        {
#ifdef NV_TEX2
            nvAllocateInternalTexture(pTexture);
            /* txtodo - remove this comment
            nvTextureAllocSwizzleSurface (pTexture,
                                          pTexture->dwMipMapSizeBytes,
                                          pDriverData->GARTLinearBase ? (NV4_TEXLOC_VID | NV4_TEXLOC_AGP)
                                                                      : (NV4_TEXLOC_VID | NV4_TEXLOC_PCI),
                                          pTexture->dwTextureFlags >> NV4_TEXMASK_SWIZZLE_SHIFT);
            pTexture->dwTextureFlags &= ~NV4_TEXFLAG_SWIZZLE_VALID;
            */
#else
            pTexture->dwTexturePointer = 0;
            pTexture->dwTextureOffset  = 0;
            nvAllocateInternalTexture(pTexture);
            pTexture->dwTextureFlags |= NV3_TEXTURE_MODIFIED;
#endif // NV_TEX2
        }

        /*
         * If the texture flags show that the internal texture surface has been allocated and
         * the internal texture is in the PCI system memory texture heap, validate the texture
         * pointer.  If it is invalid, then it must be reallocated.
         */
#ifdef NV_TEX2
        if (TEX_SWIZZLE_FLAGS(pTexture,pTexture->dwTextureFlags) & NV4_TEXLOC_PCI)
#else
        if ((pTexture->dwTextureFlags & NV3_TEXTURE_INTERNAL_ALLOCATED)
         && !(pTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
         && (pDriverData->GARTLinearBase == 0))
#endif
        {
            /*
             * Validate the PCI system memory texture pointer.
             */
#ifdef NV_TEX2
            if (!GetPointerTextureHeap(TEX_SWIZZLE_OFFSET(pTexture,pTexture->dwTextureFlags)))
#else
            if (!GetPointerTextureHeap(pTexture->dwTextureOffset))
#endif
            {
#ifdef NV_TEX2
                nvTextureAllocSwizzleSurface (pTexture,
                                              pTexture->dwMipMapSizeBytes,
                                              pDriverData->GARTLinearBase ? (NV4_TEXLOC_VID | NV4_TEXLOC_AGP)
                                                                          : (NV4_TEXLOC_VID | NV4_TEXLOC_PCI),
                                              TEX_SWIZZLE_FLAGS(pTexture,pTexture->dwTextureFlags) & NV4_TEXMASK_LOCATION);
                TEX_SWIZZLE_FLAGS(pTexture,pTexture->dwTextureFlags) &= ~NV4_TEXFLAG_VALID;
#else
                pTexture->dwTexturePointer = 0;
                pTexture->dwTextureOffset  = 0;
                nvAllocateInternalTexture(pTexture);
                pTexture->dwTextureFlags |= NV3_TEXTURE_MODIFIED;
#endif // NV_TEX2
            }
        }
    }

    /*
     * Show that the texture has not been loaded yet and that it is not locked.
     */
#ifdef NV_TEX2
    pTexture->dwTextureFlags |= NV4_TEXFLAG_HANDLE_ALLOCATED;
#else
    pTexture->dwTextureFlags |= NV3_TEXTURE_HANDLE_ALLOCATED;
    pTexture->dwTextureFlags &= ~(NV3_TEXTURE_IS_LOCKED | NV3_TEXTURE_MUST_RELOAD);
#endif

    /*
     * Load convert the texture immediately so that hopefully real game speed does not
     * get impacted during actual game play.  This has no effect on the benchmarks, but
     * it can sometimes help out games (like moto racer with the polygon patch).
     */
#ifdef NV_TEX2
    if (!(TEX_SWIZZLE_FLAGS(pTexture,pTexture->dwTextureFlags) & NV4_TEXFLAG_VALID)
      && (pTexture->dwTextureFlags & NV4_TEXFLAG_LINEAR_VALID))
        nvUpdateSwizzleSurface(pTexture);
#else
    if ((pTexture->dwTextureFlags & NV3_TEXTURE_MODIFIED)
     && (pTexture->dwTextureFlags & NV3_TEXTURE_USER_ALLOCATED))
        nvLoadTexture(pTexture);
#endif

    /*
     * Texture creation successful.
     */
    NV_D3D_GLOBAL_SAVE();
    ptcd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}
DWORD __stdcall nvTextureDestroy
(
    LPD3DHAL_TEXTUREDESTROYDATA ptcd
)
{
    HDDRVITEM       hTexture;
    PNVD3DTEXTURE   pTexture = 0;

    /*
     * DUMB APPLICATION ALERT!!!!!
     * This code is going to look really strange, but there is a reason.  Believe it or not,
     * an application actually exists (Nightmare Creatures by Kalisto) that does not work if
     * the handle returned has bit 31 set.  Because of where the memory for our texture structures
     * is located, the high bit is always set, but I don't want to just blindly assume that this
     * is always going to be the case.  So, what I'm going to do is mask off the high bit of the
     * texture structure and return the remaining 31 bits as the handle, then in places where the
     * application passes in a texture handle, I will take the pointer to the head of the texture
     * list, mask off all but the high bit and OR it in with the handle to get the pointer to the
     * texture structure.  This should be safe since the texture structures are all allocated from
     * a linearly contiguous heap.
     */

    DPF_LEVEL(NVDBG_LEVEL_DDI_ENTRY, "nvTextureDestroy - Destroy texture handle = %08lx", ptcd->dwHandle);

    /*
     * This is a little ugly.  I know however that the handle is supposed to have bit 31
     * set all the time in order to reference it as a pointer.  And I need the pointer to the
     * texture so that I can get the pointer to the global driver data structure out of the
     * surface structure before I can access pDriverData which I would normally use to construct
     * the texture structure pointer.
     */
    hTexture = ptcd->dwHandle | 0x80000000;
    pTexture = (PNVD3DTEXTURE)hTexture;

    /*
     * The order in which the texture handle is allocated/deallocated and the texture
     * surface is created/destroyed can vary based on the application (and also a problem
     * between D3D and DDRAW). Check to see if the internal data structure is still allcocated
     * and destroy it if it still is AND the texture surface has already been destroyed.
     * If the texture surface has not been destroyed yet, then we don't want to destroy the
     * internal texture structure yet.
     */
//    hTexture = ptcd->dwHandle | (pDriverData->dwTextureListHead & 0x80000000);
//    pTexture = (PNVD3DTEXTURE)hTexture;

    /*
     * Perform all the neccessary steps for destroying this texture handle.
     * Destruction of the actual texture data structure is actually
     * dependent on the current state of the texture surface structure.
     */
    if (pTexture)
    {
        /*
         * Get pointer to global driver. data structure.
         */
        pDriverData = (GLOBALDATA *)pTexture->dwDriverData;

        /*
         * Mark the texture handle as being deallocated.
         */
#ifdef NV_TEX2
        pTexture->dwTextureFlags &= ~NV4_TEXFLAG_HANDLE_ALLOCATED;
#else
        pTexture->dwTextureFlags &= ~NV3_TEXTURE_HANDLE_ALLOCATED;
#endif

        /*
         * Destroy the internal texture data structure.
         */
        nvDestroyTextureStructure(pTexture);
    }

    /*
     * Texture destroyed successfully.
     */
    ptcd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}
DWORD __stdcall nvTextureSwap
(
    LPD3DHAL_TEXTURESWAPDATA    ptsd
)
{
    PNVD3DCONTEXT   pContext  = 0;
    PNVD3DTEXTURE   pTexture1 = 0;
    PNVD3DTEXTURE   pTexture2 = 0;
    NVD3DTEXTURE    texTemp;

    /*
     * DUMB APPLICATION ALERT!!!!!
     * This code is going to look really strange, but there is a reason.  Believe it or not,
     * an application actually exists (Nightmare Creatures by Kalisto) that does not work if
     * the handle returned has bit 31 set.  Because of where the memory for our texture structures
     * is located, the high bit is always set, but I don't want to just blindly assume that this
     * is always going to be the case.  So, what I'm going to do is mask off the high bit of the
     * texture structure and return the remaining 31 bits as the handle, then in places where the
     * application passes in a texture handle, I will take the pointer to the head of the texture
     * list, mask off all but the high bit and OR it in with the handle to get the pointer to the
     * texture structure.  This should be safe since the texture structures are all allocated from
     * a linearly contiguous heap.
     */

    DPF_LEVEL(NVDBG_LEVEL_DDI_ENTRY, "nvTextureSwap - hContext = %08lx", ptsd->dwhContext);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "Swap texture handle 1 = %08lx", ptsd->dwHandle1);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvTextureSwap - Swap texture handle 2 = %08lx", ptsd->dwHandle2);

    /*
     * NOTES:
     *
     * This callback is invoked when two texture handles are to be swapped.
     * I.e. the data refered to by the two handles is to be swapped.
     *
     * Get the pointer to the specified context.
     */
    pContext = (PNVD3DCONTEXT)ptsd->dwhContext;
    if (!pContext)
    {
        dbgD3DError();
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureSwap - Bad Context");
        ptsd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Get pointer to global driver. data structure.
     */
    NvSetDriverDataPtrFromContext(pContext);

    /*
     * Get pointer to first texture.
     */
    pTexture1 = (PNVD3DTEXTURE)(ptsd->dwHandle1 | (pDriverData->dwTextureListHead & 0x80000000));
    if (!pTexture1)
    {
        dbgD3DError();
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureSwap - Bad Texture 1");
        ptsd->ddrval = DDERR_INVALIDPARAMS;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Get pointer to second texture.
     */
    pTexture2 = (PNVD3DTEXTURE)(ptsd->dwHandle2 | (pDriverData->dwTextureListHead & 0x80000000));
    if (!pTexture2)
    {
        dbgD3DError();
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureSwap - Bad Texture 2");
        ptsd->ddrval = DDERR_INVALIDPARAMS;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Swap the contents of the internal texture data structures.
     */
    texTemp    = *pTexture1;
    *pTexture1 = *pTexture2;
    *pTexture2 = texTemp;

    /*
     * Restore the original structure list pointers.
     * Even though the contents of the texture structure have been swapped,
     * the positions of the structures in the texture list does not change.
     */
    pTexture2->pTexturePrev = pTexture1->pTexturePrev;
    pTexture2->pTextureNext = pTexture1->pTextureNext;
    pTexture1->pTexturePrev = texTemp.pTexturePrev;
    pTexture1->pTextureNext = texTemp.pTextureNext;
    NV_LOCK_TEXTURE_SURFACE(pTexture1);
#ifdef NV_TEX2
    if ((TEX_SWIZZLE_ADDR(pTexture1,pTexture1->dwTextureFlags))
#else
    if ((pTexture1->dwTextureFlags & NV3_TEXTURE_SURFACE_ALLOCATED)
#endif
     && (pTexture1->lpLcl))
    {
        /*
         * Make sure the handle in the local surface structure is correct
         * after the swap.
         */
        pTexture1->lpLcl->dwReserved1 = ptsd->dwHandle1 | (pDriverData->dwTextureListHead & 0x80000000);

        /*
         * If the texture being swapped is the base of a user mipmap chain,
         * then it is neccessary to traverse the chain and update the mipmap base texture
         * field for each mipmap in the chain.
         */
        if ((pTexture1->lpLcl->ddsCaps.dwCaps & DDSCAPS_MIPMAP)
         && (pTexture1->lpLcl->lpAttachList))
        {
            LPDDRAWI_DDRAWSURFACE_LCL   lclMip;

            lclMip = pTexture1->lpLcl->lpAttachList->lpAttached;
            while (lclMip)
            {
                PNVD3DTEXTURE   pMipMap;

                pMipMap = (PNVD3DTEXTURE)lclMip->dwReserved1;
                if (pMipMap)
                    pMipMap->hMipBaseTexture = (DWORD)pTexture1;
                if (lclMip->lpAttachList)
                    lclMip = lclMip->lpAttachList->lpAttached;
                else
                    lclMip = 0;
            }
        }
    }
    NV_LOCK_TEXTURE_SURFACE(pTexture2);
#ifdef NV_TEX2
    if ((TEX_SWIZZLE_ADDR(pTexture2,pTexture2->dwTextureFlags))
#else
    if ((pTexture2->dwTextureFlags & NV3_TEXTURE_SURFACE_ALLOCATED)
#endif
     && (pTexture2->lpLcl))
    {
        /*
         * Make sure the handle in the local surface structure is correct
         * after the swap.
         */
        pTexture2->lpLcl->dwReserved1 = ptsd->dwHandle2 | (pDriverData->dwTextureListHead & 0x80000000);

        /*
         * If the texture being swapped is the base of a user mipmap chain,
         * then it is neccessary to traverse the chain and update the mipmap base texture
         * field for each mipmap in the chain.
         */
        if ((pTexture2->lpLcl->ddsCaps.dwCaps & DDSCAPS_MIPMAP)
         && (pTexture2->lpLcl->lpAttachList))
        {
            LPDDRAWI_DDRAWSURFACE_LCL   lclMip;

            lclMip = pTexture2->lpLcl->lpAttachList->lpAttached;
            while (lclMip)
            {
                PNVD3DTEXTURE   pMipMap;

                pMipMap = (PNVD3DTEXTURE)lclMip->dwReserved1;
                if (pMipMap)
                    pMipMap->hMipBaseTexture = (DWORD)pTexture2;
                if (lclMip->lpAttachList)
                    lclMip = lclMip->lpAttachList->lpAttached;
                else
                    lclMip = 0;
            }
        }
    }

    /*
     * Texture swap was successful.
     */
    ptsd->ddrval = DD_OK;
    NV_UNLOCK_TEXTURE_SURFACE(pTexture1);
    NV_UNLOCK_TEXTURE_SURFACE(pTexture2);

    return (DDHAL_DRIVER_HANDLED);
}
DWORD __stdcall nvTextureGetSurf
(
    LPD3DHAL_TEXTUREGETSURFDATA ptgd
)
{
    PNVD3DCONTEXT   pContext = 0;
    PNVD3DTEXTURE   pTexture = 0;

    /*
     * DUMB APPLICATION ALERT!!!!!
     * This code is going to look really strange, but there is a reason.  Believe it or not,
     * an application actually exists (Nightmare Creatures by Kalisto) that does not work if
     * the handle returned has bit 31 set.  Because of where the memory for our texture structures
     * is located, the high bit is always set, but I don't want to just blindly assume that this
     * is always going to be the case.  So, what I'm going to do is mask off the high bit of the
     * texture structure and return the remaining 31 bits as the handle, then in places where the
     * application passes in a texture handle, I will take the pointer to the head of the texture
     * list, mask off all but the high bit and OR it in with the handle to get the pointer to the
     * texture structure.  This should be safe since the texture structures are all allocated from
     * a linearly contiguous heap.
     */

    DPF_LEVEL(NVDBG_LEVEL_DDI_ENTRY, "nvTextureGetSurf - Get texture surface handle = %08lx", ptgd->dwHandle);

    /*
     * NOTES:
     *
     * This callback is invoked when the d3d needs to obtain the surface
     * refered to by a handle.
     *
     * Get the pointer to the specified context.
     */
    pContext = (PNVD3DCONTEXT)ptgd->dwhContext;
    if (!pContext)
    {
        dbgD3DError();
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureGetSurf - Bad Context");
        ptgd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Get pointer to global driver data structure.
     */
    NvSetDriverDataPtrFromContext(pContext);

    /*
     * Get pointer to the texture.
     */
    pTexture = (PNVD3DTEXTURE)(ptgd->dwHandle | (pDriverData->dwTextureListHead & 0x80000000));

    if (!pTexture)
    {
        dbgD3DError();
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureGetSurf - Bad Texture");
        ptgd->ddrval = DDERR_INVALIDPARAMS;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Return the pointer to the texture surface.
     */
#ifndef WINNT
    ptgd->lpDDS = (DWORD)pTexture->lpDDS;
#else
    ptgd->hDDS = pTexture->hDDS;
#endif // #ifndef WINNT


    /*
     * Texture surface was gotten successfully.
     */
    ptgd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}

/*
 * --------------------------------------------------------------------------
 * Texture heap management routines.
 * --------------------------------------------------------------------------
 */
BOOL CreateTextureHeap
(
    void
)
{
    DWORD   status;
    DWORD   dwBlockStartOffset;

    /*
     * Something is very wrong if we don't have a pointer to NV at this point.
     */
    if ((!pDriverData->NvDevFlatDma) || (!pDriverData->NvBaseFlat))
        return (FALSE);

    /*
     * Only need to create the heap once.
     */
    if (pDriverData->hTextureHeap)
        return (TRUE);

    /*
     * The texture heap always starts out empty.
     */
    pDriverData->dwAllocSize = 0;

    /*
     * On AGP don't allocate a heap just use the GART address.
     */
    if (pDriverData->GARTLinearBase > 0)
    {
        /*
         * Make sure we don't do this more than neccessary.
         */
        CreateAGPListArray();
        pDriverData->hTextureHeap = 1;
        return (TRUE);
    }

    /*
     * Get the maximum size of the texture heap.
     */
    if (!(pDriverData->dwTextureHeapSizeMax = pDriverData->regTexHeap))
        return (TRUE);

    do
    {
        /*
         * Create the texture heap.
         */
#ifndef OLDSTUFF
        pDriverData->hTextureHeap = (DWORD)HEAPCREATE(pDriverData->dwTextureHeapSizeMax);
#else
        pDriverData->hTextureHeap = (DWORD)HeapCreate(HEAP_SHARED, pDriverData->dwTextureHeapSizeMax, 0);
#endif // #ifdef OLDSTUFF
        if (!pDriverData->hTextureHeap)
            return (FALSE);

        /*
         * Allocate the entire heap up front for management.
         */
#ifndef OLDSTUFF
        (PTEXHEAPHEADER)pDriverData->pRealHeapBase = (PTEXHEAPHEADER)HEAPALLOC((HANDLE)pDriverData->hTextureHeap, pDriverData->dwTextureHeapSizeMax);
#else
        (PTEXHEAPHEADER)pDriverData->pRealHeapBase = (PTEXHEAPHEADER)HeapAlloc((HANDLE)pDriverData->hTextureHeap, HEAP_ZERO_MEMORY, pDriverData->dwTextureHeapSizeMax);
#endif
        if (!pDriverData->pRealHeapBase)
        {
#ifndef OLDSTUFF
            HEAPDESTROY((HANDLE)pDriverData->hTextureHeap);
#else
            HeapDestroy((HANDLE)pDriverData->hTextureHeap);
#endif // #ifdef OLDSTUFF
            pDriverData->hTextureHeap = (DWORD)NULL;
            return (FALSE);
        }

        /*
         * Get the size of the allocated heap and set the maximum heap limit.
         * The size shouldn't be different than what we asked for, but do
         * the check anyway, just for fun.
         */
#ifndef OLDSTUFF
#ifndef WINNT
        pDriverData->dwTextureHeapSizeMax  = (DWORD)HEAPSIZE((HANDLE)pDriverData->hTextureHeap, (PTEXHEAPHEADER)pDriverData->pRealHeapBase);
#endif // #ifndef WINNT
#else
        pDriverData->dwTextureHeapSizeMax  = (DWORD)HeapSize((HANDLE)pDriverData->hTextureHeap, 0, (PTEXHEAPHEADER)pDriverData->pRealHeapBase);
#endif // #ifdef OLDSTUFF
        pDriverData->dwTextureHeapLimitMax = pDriverData->dwTextureHeapSizeMax - 1;

        /*
         * Align the heap properly.
         */
        pDriverData->pTextureHeapBase       = ((pDriverData->pRealHeapBase + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN);
        pDriverData->dwTextureHeapSizeMax  -= (pDriverData->pTextureHeapBase - pDriverData->pRealHeapBase);
        pDriverData->dwTextureHeapSizeMax  &= 0xFFFFFF00;
        pDriverData->dwTextureHeapLimitMax  = pDriverData->dwTextureHeapSizeMax - 1;
        pDriverData->dwFreeSize             = pDriverData->dwTextureHeapSizeMax - NV_TEXTURE_PAD;
        pDriverData->dwTextureHeapSize      = pDriverData->dwTextureHeapSizeMax;
        pDriverData->dwTextureHeapLimit     = pDriverData->dwTextureHeapSize - 1;

        /*
         * Try and lock down the entire heap.
         */
#ifdef  CACHE_FREECOUNT
        nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
        UnplugSystemMemoryTextureContext();
        status = AdjustTextureLimit(D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, pDriverData->pTextureHeapBase, pDriverData->dwTextureHeapLimit, 0);
        UpdateTriangleContexts();
        if (status)
        {
            /*
             * Map the context to the frame buffer because the RM/Hardware doesn't
             * like having limts of 0.
             */
            UnplugSystemMemoryTextureContext();
#ifndef WINNT
            AdjustTextureLimit(D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, pDriverData->BaseAddress, 0xFF, 0xFF);
#else
            AdjustTextureLimit(D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, pDriverData->ppdev->pjScreen, 0xFF, 0xFF);
#endif // #ifdef WINNT
            UpdateTriangleContexts();
#ifndef OLDSTUFF
            HEAPFREE(pDriverData->hTextureHeap, (PTEXHEAPHEADER)pDriverData->pRealHeapBase);
            HEAPDESTROY((HANDLE)pDriverData->hTextureHeap);
#else
            HeapFree((HANDLE)pDriverData->hTextureHeap, 0, (PTEXHEAPHEADER)pDriverData->pRealHeapBase);
            HeapDestroy((HANDLE)pDriverData->hTextureHeap);
#endif
            pDriverData->hTextureHeap  = (DWORD)NULL;
            (PTEXHEAPHEADER)pDriverData->pRealHeapBase = (PTEXHEAPHEADER)NULL;
            if (status)
            {
                pDriverData->dwTextureHeapSizeMax -= min(pDriverData->dwTextureHeapSizeMax, HEAP_SIZE_ADJUST);
                if (!pDriverData->dwTextureHeapSizeMax)
                {
                    /*
                     * Can't lock down even a minimal heap.  Disable system memory textures.
                     */
                    pDriverData->regTexHeap             = 0;
                    pDriverData->dwTextureHeapSizeMax  = 0;
                    pDriverData->dwTextureHeapLimitMax = 0;
#ifdef  CACHE_FREECOUNT
                    pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
                    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
                    return (FALSE);
                }
            }
            else
            {
                /*
                 * Something more severe occured.  Disable system memory textures.
                 */
                pDriverData->regTexHeap            = 0;
                pDriverData->dwTextureHeapSizeMax  = 0;
                pDriverData->dwTextureHeapLimitMax = 0;
#ifdef  CACHE_FREECOUNT
                pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
                NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
                return (FALSE);
            }
        }
#ifdef  CACHE_FREECOUNT
        pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
    } while (!pDriverData->hTextureHeap);

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
    dwBlockStartOffset = (((pDriverData->pTextureHeapBase + sizeof(TEXHEAPHEADER) + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN) - pDriverData->pTextureHeapBase);

    /*
     * Fill in the initial block structure.
     */
    ((PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead)                     = (PTEXHEAPHEADER)(pDriverData->pTextureHeapBase + dwBlockStartOffset - sizeof(TEXHEAPHEADER));
    ((PTEXHEAPHEADER)pDriverData->pTextureHeapFreeTail)                     = (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead;
    ((PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead)->dwTag              = HEAP_TAG_FREE;
    ((PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead)->dwBlockSize        = pDriverData->dwTextureHeapSizeMax - dwBlockStartOffset;
    ((PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead)->dwBlockStartOffset = dwBlockStartOffset;
    ((PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead)->pNextFreeBlock     = (PTEXHEAPHEADER)NULL;
    ((PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead)->pPrevFreeBlock     = (PTEXHEAPHEADER)NULL;
    ((PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead)->pNextAllocBlock    = (PTEXHEAPHEADER)NULL;
    ((PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead)->pPrevAllocBlock    = (PTEXHEAPHEADER)NULL;

    /*
     * Dump some information about the texture heap.
     */
    DPF_LEVEL(NVDBG_LEVEL_INFO, "CreateTextureHeap - pTextureHeapBase      = %08lx", pDriverData->pTextureHeapBase);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "CreateTextureHeap - dwTextureHeapSizeMax  = %08lx", pDriverData->dwTextureHeapSizeMax);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "CreateTextureHeap - dwTextureHeapLimitMax = %08lx", pDriverData->dwTextureHeapLimitMax);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "CreateTextureHeap - dwTextureHeapSize     = %08lx", pDriverData->dwTextureHeapSize);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "CreateTextureHeap - dwTextureHeapLimit    = %08lx", pDriverData->dwTextureHeapLimit);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "CreateTextureHeap - dwFreeSize            = %08lx", pDriverData->dwFreeSize);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "CreateTextureHeap - dwAllocSize           = %08lx", pDriverData->dwAllocSize);

    /*
     * Return successfully.
     */
    return (TRUE);
}
BOOL DestroyTextureHeap
(
    void
)
{
    /*
     * Can't destroy a heap if there isn't one.
     */
    if (!pDriverData->hTextureHeap)
        return (TRUE);

    /*
     * Tear down the Texture DMA context.
     * Note: Need to be careful here, because there are some instances that
     * the channel may already be closed at this point.
     */
    /*
     * Something is very wrong if we don't have a pointer to NV at this point.
     */
    if ((pDriverData->NvDevFlatDma) && (pDriverData->NvBaseFlat))
    {
        /*
         * Map the context to the frame buffer because the RM/Hardware doesn't
         * like having limts of 0.
         */
#ifdef  CACHE_FREECOUNT
        nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
        UnplugSystemMemoryTextureContext();
#ifndef WINNT
        AdjustTextureLimit(D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, pDriverData->BaseAddress, 0xFF, 0xFF);
#else
        AdjustTextureLimit(D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, pDriverData->ppdev->pjScreen, 0xFF, 0xFF);
#endif // #ifdef WINNT

        UpdateTriangleContexts();
#ifdef  CACHE_FREECOUNT
        pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
    }

    /*
     * Free and destroy the texture heap.
     */
    if (pDriverData->GARTLinearBase == 0)
    {
        /*
         * Only need to free the heap if it was allocated.  (it won't be
         * allocated on an AGP system)
         */
#ifndef OLDSTUFF
        HEAPFREE(pDriverData->hTextureHeap, (PTEXHEAPHEADER)pDriverData->pRealHeapBase);
        HEAPDESTROY(pDriverData->hTextureHeap);
#else
        HeapFree((HANDLE)pDriverData->hTextureHeap, 0, (PTEXHEAPHEADER)pDriverData->pRealHeapBase);
        HeapDestroy((HANDLE)pDriverData->hTextureHeap);
#endif
    }
    else
        DestroyAGPListArray();

    /*
     * Clean up a little bit.
     */
    pDriverData->hTextureHeap                           = (DWORD)NULL;
    (PTEXHEAPHEADER)pDriverData->pRealHeapBase         = (PTEXHEAPHEADER)NULL;
    (PTEXHEAPHEADER)pDriverData->pTextureHeapBase      = (PTEXHEAPHEADER)NULL;
    (PTEXHEAPHEADER)pDriverData->pTextureHeapAllocHead = (PTEXHEAPHEADER)NULL;
    (PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail = (PTEXHEAPHEADER)NULL;
    (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead  = (PTEXHEAPHEADER)NULL;
    (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeTail  = (PTEXHEAPHEADER)NULL;
    pDriverData->dwTextureHeapSize                     = 0;
    pDriverData->dwTextureHeapSizeMax                  = 0;
    pDriverData->dwTextureHeapLimit                    = 0;
    pDriverData->dwTextureHeapLimitMax                 = 0;
    pDriverData->dwAllocSize                           = 0;
    pDriverData->dwFreeSize                            = 0;
    return (TRUE);
}
DWORD AllocTextureHeap
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

    DPF_LEVEL(NVDBG_LEVEL_FUNCTION_CALL, "AllocTextureHeap - dwSizeRequest = %08lx", dwSizeRequest);

    /*
     * Create the texture heap if it doesn't exist yet.
     */
    if (!pDriverData->hTextureHeap)
        CreateTextureHeap();

    /*
     * If the free list is null, then there's no memory to be allocated.
     */
    if ((!pDriverData->pTextureHeapFreeHead)
     || (pDriverData->dwFreeSize < dwSizeRequest))
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "AllocTextureHeap - Not enough free memory to fill request!");
        return (0);
    }

    /*
     * Search the free block list for a block large enough to fill this
     * request.
     */
    pAllocBlock      = (PTEXHEAPHEADER)NULL;
    dwAllocBlockSize = 0;
    pList            = (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead;

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
    if (!pAllocBlock)
        return (0);

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

        dwNextBlockOffset         = pAllocBlock->dwBlockStartOffset + dwSizeRequest;
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
        dwNewFreeBlockStartOffset = ((((pDriverData->pTextureHeapBase + dwNextBlockOffset + sizeof(TEXHEAPHEADER)) + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN) - pDriverData->pTextureHeapBase);
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
        DPF_LEVEL(NVDBG_LEVEL_INFO, "AllocTextureHeap - Create new free block at end of allocation block.");

        /*
         * Create the new free block.
         */
        pNewFree                     = (PTEXHEAPHEADER)(pDriverData->pTextureHeapBase + dwNewFreeBlockOffset);
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
        if (pPrevFree)
            pPrevFree->pNextFreeBlock = pNewFree;
        else
            (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead = pNewFree;
        if (pNextFree)
            pNextFree->pPrevFreeBlock = pNewFree;
        else
            (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeTail = pNewFree;
    }
    else
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "AllocTextureHeap - Tack excess free memory  onto end of allocation block.");

        /*
         * Tack the excess memory on to the end of the block.
         */
        pAllocBlock->dwBlockSize += dwNewFreeBlockSize;

        /*
         * Remove the block from the free block list.
         */
        if (pPrevFree)
            pPrevFree->pNextFreeBlock = pNextFree;
        else
            (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead = pNextFree;

        if (pNextFree)
            pNextFree->pPrevFreeBlock = pPrevFree;
        else
            (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeTail = pPrevFree;
    }

    /*
     * Update the allocation sizes.
     */
    pDriverData->dwAllocSize += pAllocBlock->dwBlockSize;
    pDriverData->dwFreeSize  -= pAllocBlock->dwBlockSize;

    /*
     * Insert the newly allocated block into the allocation list.
     * First handle the best case scenarios, the block is either the first block
     * to be allocated, the block comes before the current head of the allocation
     * list, or the block comes after the current tail of the allocation list.
     * In these cases, its simply a case of updating a few pointers.
     */
    if (!pDriverData->pTextureHeapAllocHead)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "AllocTextureHeap - Insert new allocation block at head of allocation list. First time.");
        pAllocBlock->pPrevAllocBlock                        = (PTEXHEAPHEADER)NULL;
        pAllocBlock->pNextAllocBlock                        = (PTEXHEAPHEADER)NULL;
        (PTEXHEAPHEADER)pDriverData->pTextureHeapAllocHead = pAllocBlock;
        (PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail = pAllocBlock;

        /*
         * The tail of allocation list has changed. Check to see if the texture context
         * needs to be udpated to reflect the change.
         */
        if (pDriverData->pTextureHeapAllocTail)
            dwAllocLimit = ((PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail)->dwBlockStartOffset + ((PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail)->dwBlockSize - 1;
        else
            dwAllocLimit = 0;
    }
    else if (pAllocBlock < (PTEXHEAPHEADER)pDriverData->pTextureHeapAllocHead)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "AllocTextureHeap - Insert new allocation block at head of allocation list.");
        ((PTEXHEAPHEADER)pDriverData->pTextureHeapAllocHead)->pPrevAllocBlock = pAllocBlock;
        pAllocBlock->pPrevAllocBlock                                           = (PTEXHEAPHEADER)NULL;
        pAllocBlock->pNextAllocBlock                                           = (PTEXHEAPHEADER)pDriverData->pTextureHeapAllocHead;
        (PTEXHEAPHEADER)pDriverData->pTextureHeapAllocHead                    = pAllocBlock;
    }
    else if (pAllocBlock > (PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "AllocTextureHeap - Insert new allocation block at tail of allocation list.");
        ((PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail)->pNextAllocBlock = pAllocBlock;
        pAllocBlock->pPrevAllocBlock                                           = (PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail;
        pAllocBlock->pNextAllocBlock                                           = (PTEXHEAPHEADER)NULL;
        (PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail                    = pAllocBlock;

        /*
         * The tail of allocation list has changed. Check to see if the texture context
         * needs to be udpated to reflect the change.
         */
        if (pDriverData->pTextureHeapAllocTail)
            dwAllocLimit = ((PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail)->dwBlockStartOffset + ((PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail)->dwBlockSize - 1;
        else
            dwAllocLimit = 0;
    }
    else
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "AllocTextureHeap - Insert new allocation block in middle of allocation list.");

        /* WORST CASE
         * The newly allocated block falls between the head and tail of the allocation
         * list. Search all memory blocks following the new allocation block to find the
         * next allocated block, then just update the pointers.
         */
        pList = (PTEXHEAPHEADER)((DWORD)pAllocBlock + sizeof(TEXHEAPHEADER) + pAllocBlock->dwBlockSize);
        while ((pList)
            && (pList->dwTag != HEAP_TAG_ALLOC))
        {
            pList = (PTEXHEAPHEADER)((DWORD)pList + sizeof(TEXHEAPHEADER) + pList->dwBlockSize);
        }
        /*
         * This should never happen, if it does, then we've got problems.
         */
        if (!pList)
        {
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "AllocTextureHeap - Error inserting new allocation block!!");
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
    DPF_LEVEL(NVDBG_LEVEL_INFO, "AllocTextureHeap - pAllocBlock->dwBlockStartOffset = %08lx", (DWORD)pAllocBlock->dwBlockStartOffset);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "AllocTextureHeap - dwTextureHeapSize               = %08lx", pDriverData->dwTextureHeapSize);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "AllocTextureHeap - dwTextureHeapLimit              = %08lx", pDriverData->dwTextureHeapLimit);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "AllocTextureHeap - dwFreeSize                      = %08lx", pDriverData->dwFreeSize);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "AllocTextureHeap - dwAllocSize                     = %08lx", pDriverData->dwAllocSize);

    /*
     * Return the offset to the allocated block.
     */
    return (pAllocBlock->dwBlockStartOffset);
}
BOOL FreeTextureHeap
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

    DPF_LEVEL(NVDBG_LEVEL_FUNCTION_CALL, "FreeTextureHeap - dwFreeBlockOffset       = %08lx", dwFreeBlockOffset);

    /*
     * Make sure there is a valid texture heap to free the memory from.
     */
    if (!pDriverData->pTextureHeapBase)
        return (TRUE);

    /*
     * Make sure there is actually a valid texture block to free.
     */
    if (!dwFreeBlockOffset)
        return (FALSE);

    /*
     * Calculate the pointer to the header of the block being freed.
     */
    pFreeBlock = (PTEXHEAPHEADER)(pDriverData->pTextureHeapBase + (dwFreeBlockOffset - sizeof(TEXHEAPHEADER)));
    DPF_LEVEL(NVDBG_LEVEL_INFO, "FreeTextureHeap - pFreeBlock->dwBlockSize = %08lx", pFreeBlock->dwBlockSize);

    /*
     * Make sure we are pointer to an actual allocated block header.
     */
    if (pFreeBlock->dwTag != HEAP_TAG_ALLOC)
        return (FALSE);

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
        (PTEXHEAPHEADER)pDriverData->pTextureHeapAllocHead = pNextAlloc;
    if (pNextAlloc)
        pNextAlloc->pPrevAllocBlock = pPrevAlloc;
    else
    {
        (PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail = pPrevAlloc;

        /*
         * The tail of allocation list has changed. Check to see if the texture context
         * needs to be udpated to reflect the change.
         */
        if (pDriverData->pTextureHeapAllocTail)
            dwAllocLimit = ((PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail)->dwBlockStartOffset + ((PTEXHEAPHEADER)pDriverData->pTextureHeapAllocTail)->dwBlockSize - 1;
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
    pDriverData->dwAllocSize -= pFreeBlock->dwBlockSize;
    pDriverData->dwFreeSize  += pFreeBlock->dwBlockSize;

    /*
     * Insert the newly freed block into the free list.
     */
    if (!pDriverData->pTextureHeapFreeHead)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "FreeTextureHeap - Insert new free block at head of free list. First time.");
        pFreeBlock->pPrevFreeBlock                         = (PTEXHEAPHEADER)NULL;
        pFreeBlock->pNextFreeBlock                         = (PTEXHEAPHEADER)NULL;
        (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead = pFreeBlock;
        (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeTail = pFreeBlock;
    }
    else if (pFreeBlock < (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "FreeTextureHeap - Insert new free block at head of free list.");
        ((PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead)->pPrevFreeBlock = pFreeBlock;
        pFreeBlock->pPrevFreeBlock                                           = (PTEXHEAPHEADER)NULL;
        pFreeBlock->pNextFreeBlock                                           = (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead;
        (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeHead                   = pFreeBlock;
    }
    else if (pFreeBlock > (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeTail)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "FreeTextureHeap - Insert new free block at tail of free list.");
        ((PTEXHEAPHEADER)pDriverData->pTextureHeapFreeTail)->pNextFreeBlock = pFreeBlock;
        pFreeBlock->pPrevFreeBlock                                           = (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeTail;
        pFreeBlock->pNextFreeBlock                                           = (PTEXHEAPHEADER)NULL;
        (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeTail                   = pFreeBlock;
    }
    else
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "FreeTextureHeap - Insert new free block in middle of free list.");

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
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "FreeTextureHeap - Error inserting newly freed block!!");
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
        if (pPrevFree->pNextFreeBlock = pNextFree)
            pNextFree->pPrevFreeBlock = pPrevFree;
        else
            (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeTail = pPrevFree;

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
            (PTEXHEAPHEADER)pDriverData->pTextureHeapFreeTail = pFreeBlock;

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
    DPF_LEVEL(NVDBG_LEVEL_INFO, "FreeTextureHeap - dwTextureHeapSize  = %08lx", pDriverData->dwTextureHeapSize);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "FreeTextureHeap - dwTextureHeapLimit = %08lx", pDriverData->dwTextureHeapLimit);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "FreeTextureHeap - dwFreeSize         = %08lx", pDriverData->dwFreeSize);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "FreeTextureHeap - dwAllocSize        = %08lx", pDriverData->dwAllocSize);

    /*
     * Destroy the texture heap if there's nothing left allocated in it.
     */
    if (!pDriverData->pTextureHeapAllocHead)
        DestroyTextureHeap();

    /*
     * Return successfully.
     */
    return (TRUE);
}
DWORD GetPointerTextureHeap
(
    DWORD   dwBlockOffset
)
{
    DWORD           dwBlockPointer;
    PTEXHEAPHEADER  pBlockHeader;

    dwBlockPointer = pDriverData->pTextureHeapBase + dwBlockOffset;
    pBlockHeader   = (PTEXHEAPHEADER)(dwBlockPointer - sizeof(TEXHEAPHEADER));

    if ((!pBlockHeader)
     || (pBlockHeader->dwTag != HEAP_TAG_ALLOC))
        return (0);

    /*
     * Return the pointer to the start of the memory block.
     */
    return (dwBlockPointer);
}

typedef struct _def_agp_heap_list   AGPHEAPLIST, *PAGPHEAPLIST;
struct _def_agp_heap_list
{
    DWORD           hCurrent;
    DWORD           dwPointer;
    DWORD           dwAlignedPointer;
    DWORD           dwSize;
    DWORD           dwLimit;
    HDDRVITEM       hPrev;
    HDDRVITEM       hNext;
};
HDDRVITEM           hAGPHeapHead = (HDDRVITEM)0;
HDDRVITEM           hAGPHeapTail = (HDDRVITEM)0;
DWORD AllocAGPHeap
(
    LPDDRAWI_DIRECTDRAW_GBL pDrv,
    DWORD                   dwSize
)
{
    DWORD           dwOffset;
    DWORD           dwCurrentLimit;
    DWORD           dwNewBlockLimit;
    DWORD           pAGPMem;
    HDDRVITEM       hAGPListBlock;
    PAGPHEAPLIST    pAGPHeap;
    PAGPHEAPLIST    pAGPListBlock;
    PAGPHEAPLIST    pAGPHeapHead;
    PAGPHEAPLIST    pAGPHeapTail;
    PAGPHEAPLIST    pNext;

    /*
     * Something is very wrong if we don't have a pointer to NV at this point.
     */
    if ((!pDriverData->NvDevFlatDma) || (!pDriverData->NvBaseFlat))
        return (0);

    /*
     * Make sure this is AGP.
     */
    if (pDriverData->GARTLinearBase == 0)
        return (0);

    /*
     * Create the texture heap if it doesn't exist yet.
     */
    if (!pDriverData->hTextureHeap)
        CreateTextureHeap();

    /*
     * Allocate an entry in the texture item array for the new texture.
     */
#ifndef OLDSTUFF
    hAGPListBlock = (HDDRVITEM)HEAPALLOC(pDriverData->hAGPList_gbl, sizeof(AGPHEAPLIST));
    if (!hAGPListBlock)
        return (0);
    pAGPListBlock = (PAGPHEAPLIST)hAGPListBlock;
    pAGPHeapHead  = (PAGPHEAPLIST)hAGPHeapHead; // Needs to be in pDriverData
    pAGPHeapTail  = (PAGPHEAPLIST)hAGPHeapTail; // Needs to be in pDriverData
#else
    DDrvItemArrayAlloc(pDriverData->hAGPList_gbl, &hAGPListBlock);
    if (!hAGPListBlock)
        return (0);
    pAGPListBlock = (PAGPHEAPLIST)DDrvItemArrayGetPtr(pDriverData->hAGPList_gbl, hAGPListBlock);
    if (!pAGPListBlock)
    {
        DDrvItemArrayFree(pDriverData->hAGPList_gbl, hAGPListBlock);
        return (0);
    }
    pAGPHeapHead = (PAGPHEAPLIST)DDrvItemArrayGetPtr(pDriverData->hAGPList_gbl, hAGPHeapHead);
    pAGPHeapTail = (PAGPHEAPLIST)DDrvItemArrayGetPtr(pDriverData->hAGPList_gbl, hAGPHeapTail);
#endif

    /*
     * Allocate the memory from the AGP heap.
     * Add padding to prevent DMA limit errors.
     */
    dwSize += NV_TEXTURE_PAD;
    pAGPMem = (DWORD)DDHAL32_VidMemAlloc(pDrv, AGP_HEAP, dwSize, 1);
    if (pAGPMem)
    {
        dwNewBlockLimit = ((pAGPMem - pDriverData->GARTLinearBase) + dwSize & 0xFFFFFF00) - 1;
        dwCurrentLimit = (pAGPHeapTail) ? pAGPHeapTail->dwLimit : 0;
        if (dwNewBlockLimit > dwCurrentLimit)
        {
            /*
             * Adjust the limit on the AGP context.
             */

#ifdef  CACHE_FREECOUNT
            nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
            NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
            UnplugSystemMemoryTextureContext();
            if (AdjustTextureLimit(D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, pDriverData->GARTLinearBase, dwNewBlockLimit, dwCurrentLimit))
            {
                /*
                 * Free the memory since the new limit could not be set.
                 */
                DDHAL32_VidMemFree(pDrv, AGP_HEAP, pAGPMem);

#ifndef OLDSTUFF
                HEAPFREE(pDriverData->hAGPList_gbl, (HDDRVITEM)hAGPListBlock);
#else
                DDrvItemArrayFree(pDriverData->hAGPList_gbl, (HDDRVITEM)hAGPListBlock);
#endif

                /*
                 * Could not set the new limit.  Return an error.
                 */
                UpdateTriangleContexts();
#ifdef  CACHE_FREECOUNT
                pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
                NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
                return (0);
            }
            UpdateTriangleContexts();
#ifdef  CACHE_FREECOUNT
            pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
            NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
            pDriverData->dwAllocSize += dwSize;

            /*
             * Store the current texture heap limit.
             */
            pDriverData->dwTextureHeapLimit = dwNewBlockLimit;
        }

        /*
         * Insert the new allocation block into the heap allocation list.
         */
        pAGPListBlock->hCurrent  = hAGPListBlock;
        pAGPListBlock->dwPointer = pAGPMem;

        /*
         * Align the start of the texture.
         * 1) Original way was each individual texture start offset off of the
         *    texture heap base address.
         */
//        dwOffset = ((pAGPMem - pDriverData->GARTLinearBase) + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN;
        /*
         * 2) New way is to align the start address of each texture.
         */
        dwOffset = ((pAGPMem + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN) - pDriverData->GARTLinearBase;
        pAGPMem  = pDriverData->GARTLinearBase + dwOffset;
        pAGPListBlock->dwAlignedPointer = pAGPMem;
        pAGPListBlock->dwSize           = dwSize;
        pAGPListBlock->dwLimit          = dwNewBlockLimit;
        pAGPHeap = pAGPHeapTail;
#ifndef OLDSTUFF
        while ((pAGPHeap) && (dwNewBlockLimit < pAGPHeap->dwLimit))
            pAGPHeap = (PAGPHEAPLIST)pAGPHeap->hPrev;
#else
        while ((pAGPHeap) && (dwNewBlockLimit < pAGPHeap->dwLimit))
            pAGPHeap = (PAGPHEAPLIST)DDrvItemArrayGetPtr(pDriverData->hAGPList_gbl, pAGPHeap->hPrev);
#endif
        if (pAGPHeap)
        {
#ifndef OLDSTUFF
            pNext = (PAGPHEAPLIST)pAGPHeap->hNext;
#else
            pNext = (PAGPHEAPLIST)DDrvItemArrayGetPtr(pDriverData->hAGPList_gbl, pAGPHeap->hNext);
#endif
            pAGPListBlock->hPrev = pAGPHeap->hCurrent;
            pAGPListBlock->hNext = pAGPHeap->hNext;
            pAGPHeap->hNext      = hAGPListBlock;
            if (pNext)
                pNext->hPrev = hAGPListBlock;
            else
                hAGPHeapTail = hAGPListBlock;
        }
        else
        {
            if (hAGPHeapHead)
            {
                pAGPListBlock->hPrev = (HDDRVITEM)NULL;
                pAGPListBlock->hNext = hAGPHeapHead;
            }
            else
            {
                pAGPListBlock->hPrev = (HDDRVITEM)NULL;
                pAGPListBlock->hNext = (HDDRVITEM)NULL;
                hAGPHeapTail = hAGPListBlock;
            }
            hAGPHeapHead = hAGPListBlock;
        }
    }
    else
    {
#ifndef OLDSTUFF
        HEAPFREE(pDriverData->hAGPList_gbl, (HDDRVITEM)hAGPListBlock);
#else
        DDrvItemArrayFree(pDriverData->hAGPList_gbl, (HDDRVITEM)hAGPListBlock);
#endif
        return (0);
    }
    if ((pAGPListBlock->dwAlignedPointer & NV_TEXTURE_OFFSET_ALIGN)
     || (((pAGPListBlock->dwAlignedPointer - pDriverData->GARTLinearBase) & NV_TEXTURE_OFFSET_ALIGN)))
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "AllocAGPHeap - Alignment Error - dwPointer        = %08lx", pAGPListBlock->dwPointer);
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "                                 dwAlignedPointer = %08lx", pAGPListBlock->dwAlignedPointer);
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "                                 dwOffset         = %08lx", (pAGPListBlock->dwAlignedPointer - pDriverData->GARTLinearBase));
    }
    return (pAGPMem);
}
BOOL FreeAGPHeap
(
    LPDDRAWI_DIRECTDRAW_GBL pDrv,
    DWORD                   pMem
)
{
    DWORD           dwBlockLimit;
    DWORD           dwNewLimit;
    HDDRVITEM       hAGPListBlock;
    HDDRVITEM       hAGPHeap;
    PAGPHEAPLIST    pAGPHeap;
    PAGPHEAPLIST    pAGPHeapHead;
    PAGPHEAPLIST    pAGPHeapTail;
    PAGPHEAPLIST    pPrev;
    PAGPHEAPLIST    pNext;

    /*
     * Make sure this is AGP.
     */
    if (pDriverData->GARTLinearBase == 0)
        return (FALSE);

    /*
     * Find the allocation list block for this address.
     */
#ifndef OLDSTUFF
    pAGPHeapHead = (PAGPHEAPLIST)hAGPHeapHead;
    pAGPHeapTail = (PAGPHEAPLIST)hAGPHeapTail;
#else
    pAGPHeapHead = (PAGPHEAPLIST)DDrvItemArrayGetPtr(pDriverData->hAGPList_gbl, hAGPHeapHead);
    pAGPHeapTail = (PAGPHEAPLIST)DDrvItemArrayGetPtr(pDriverData->hAGPList_gbl, hAGPHeapTail);
#endif
    pAGPHeap     = pAGPHeapHead;
    hAGPHeap     = hAGPHeapHead;
    while ((pAGPHeap) && (pMem != pAGPHeap->dwAlignedPointer))
    {
        /*
         * Watch out for infinite looping.
         */
        if ((pAGPHeap->hNext) && (pAGPHeap->hNext != hAGPHeap))
        {
            hAGPHeap = pAGPHeap->hNext;
#ifndef OLDSTUFF
            pAGPHeap = (PAGPHEAPLIST)hAGPHeap;
#else
            pAGPHeap = (PAGPHEAPLIST)DDrvItemArrayGetPtr(pDriverData->hAGPList_gbl, hAGPHeap);
#endif
        }
        else
        {
            /*
             * Something is messed up because this block is pointing to itself.
             */
            pAGPHeap->hNext = (HDDRVITEM)NULL;
            hAGPHeapTail    = hAGPHeap;
            pAGPHeap        = (PAGPHEAPLIST)NULL;
            hAGPHeap        = (HDDRVITEM)NULL;
        }
    }

    if (pAGPHeap)
    {
        /*
         * Need to free the orignal pointer that was allocated.
         */
        pMem = pAGPHeap->dwPointer;

        /*
         * Remove the allocation block from the list.
         */
#ifndef OLDSTUFF
        pPrev = (PAGPHEAPLIST)pAGPHeap->hPrev;
        pNext = (PAGPHEAPLIST)pAGPHeap->hNext;
#else
        pPrev = (PAGPHEAPLIST)DDrvItemArrayGetPtr(pDriverData->hAGPList_gbl, pAGPHeap->hPrev);
        pNext = (PAGPHEAPLIST)DDrvItemArrayGetPtr(pDriverData->hAGPList_gbl, pAGPHeap->hNext);
#endif
        if (pPrev)
        {
            if (!(pPrev->hNext = pAGPHeap->hNext))
                hAGPHeapTail = pAGPHeap->hPrev;
        }
        else
        {
            hAGPHeapHead = pAGPHeap->hNext;
            pAGPHeapHead = pNext;
        }
        if (pNext)
        {
            if (!(pNext->hPrev = pAGPHeap->hPrev))
                hAGPHeapHead = pAGPHeap->hNext;
        }
        else
        {
            hAGPHeapTail = pAGPHeap->hPrev;
            pAGPHeapTail = pPrev;
        }
        dwNewLimit                 = (pAGPHeapTail) ? pAGPHeapTail->dwLimit : 0;
        dwBlockLimit               = pAGPHeap->dwLimit;
        pDriverData->dwAllocSize -= pAGPHeap->dwSize;

        /*
         * Free the allocation block memory.
         */
        hAGPListBlock = pAGPHeap->hCurrent;
#ifndef OLDSTUFF
        HEAPFREE(pDriverData->hAGPList_gbl, (HDDRVITEM)hAGPListBlock);
#else
        DDrvItemArrayFree(pDriverData->hAGPList_gbl, (HDDRVITEM)hAGPListBlock);
#endif

        /*
         * Check if reduction of AGP context limit is neccessary.
         */
        if ((pDriverData->NvDevFlatDma) && (pDriverData->NvBaseFlat))
        {
            if (dwBlockLimit > dwNewLimit)
            {
                /*
                 * Shrink the AGP limit in the context DMA.
                 * Setting old limit to new limit will make sure that this
                 * operation happens.  There's no reason it shouldn't since
                 * the limit is getting smaller.
                 * If the limit is 0, then map the context to the frame buffer
                 * because the RM/Hardware doesn't like having limts of 0.
                 */
#ifdef  CACHE_FREECOUNT
                nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
                NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
                UnplugSystemMemoryTextureContext();
                if (dwNewLimit != 0)
                    AdjustTextureLimit(D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, pDriverData->GARTLinearBase, dwNewLimit, dwNewLimit);
                else
#ifndef WINNT
                    AdjustTextureLimit(D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, pDriverData->BaseAddress, 0xFF, 0xFF);
#else
                    AdjustTextureLimit(D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, pDriverData->ppdev->pjScreen, 0xFF, 0xFF);
#endif // #ifdef WINNT

                UpdateTriangleContexts();

#ifdef  CACHE_FREECOUNT
                pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
                NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
                /*
                 * Store the current texture heap limit.
                 */
                pDriverData->dwTextureHeapLimit = dwNewLimit;
            }
        }
    }

    /*
     * Destroy the texture heap if there's nothing left allocated in it.
     */
    if (pDriverData->dwAllocSize == 0)
        DestroyTextureHeap();

    /*
     * Free the AGP memory.
     */
    DDHAL32_VidMemFree(pDrv, AGP_HEAP, pMem);
    return (TRUE);
}
BOOL CreateAGPListArray
(
    void
)
{
#ifndef OLDSTUFF
    pDriverData->hAGPList_gbl = (HDDRVITEM)HEAPCREATE(AGPLIST_ARRAY_SIZE * sizeof(AGPHEAPLIST));
#else
    if (!DDrvItemArrayCreate(AGPLIST_ARRAY_SIZE,
                             sizeof(AGPHEAPLIST),
                             AGPLIST_ARRAY_DELTA,
                             &pDriverData->hAGPList_gbl))
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "CreateAGPListArray - AGP List array creation failed!");
        return (FALSE);
    }
#endif
    return (TRUE);
}
BOOL ResetAGPHeap
(
    void
)
{
    /*
     * Make sure this is AGP.
     */
    if ((pDriverData->dwD3DContextCount > 0)
     && (pDriverData->GARTLinearBase > 0))
        DestroyTextureHeap();
    return (TRUE);
}
BOOL DestroyAGPListArray
(
    void
)
{
    if (!pDriverData->hAGPList_gbl)
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "DestroyAGPListArray - No current AGP List array to destroy");
        return (TRUE);
    }
#ifndef OLDSTUFF
    HEAPDESTROY(pDriverData->hAGPList_gbl);
#else
    DDrvItemArrayApplyValid(pDriverData->hAGPList_gbl, DestroyAGPListCallback, 0L);
    DDrvItemArrayDestroy(pDriverData->hAGPList_gbl);
#endif

    /*
     * Once the list is destroyed, there can't be a head or a tail.
     */
    hAGPHeapHead = 0;
    hAGPHeapTail = 0;
    return (TRUE);
}
DWORD DestroyAGPListCallback
(
    LPVOID      lpData,
    HDDRVITEM   hItem,
    DWORD       dwExtra
)
{

    /*
     * No context was actually destroyed on this call, so return an
     * error so that the success count does not get updated for anyone
     * keeping track of these things.
     */
    return (DDRV_ERROR_CONTINUE);
}
