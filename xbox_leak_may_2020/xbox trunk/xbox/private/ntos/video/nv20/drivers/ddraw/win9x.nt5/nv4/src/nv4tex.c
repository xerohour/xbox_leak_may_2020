#ifdef  NV4
/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4TEX.C                                                          *
*   HW Specific Texture routines.                                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/98 - re-wrote and cleaned up.     *
*       Ben de Waal                 10/12/98 - overhaul                     *
*                                                                           *
\***************************************************************************/
#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "nvddobj.h"
#include "d3dinc.h"
#include "nvd3dmac.h"
#include "nv4dreg.h"
#include "nv3ddbg.h"
#include "nv4vxmac.h"
#include "nvheap.h"

#ifdef  MULTI_MON
#include "nvrmapi.h"
#include "nvrmarch.inc"
#include "nvos.h"
#endif  /* MULTI_MON */

/***************************************************************************\
  texture manager 2.0 implementation notes:

  * texture has one optional linear surface. Depending on its heap location
    the logic will use it to stage for AGP/PCI -> VID transfers. The linear
    surface is not maintained at all times. For AGP swizzled surfaces the
    linear surface will only be made valid for lock/unlocks and will be
    invalidated on Blts. This saves us from maintaining more than one surface
    with the CPU. If the swizzle surface is in VID the the linear surface
    can be used for staging during blt and it will stay valid.

  * texture has 1 or more swizzled surfaces. Only one surface is current at
    a time but the others retire lazily (and therefore more than one swizzled
    surface can be used at a time). The current surface might have mipmaps
    that is not carriend forward from the previous texture. In order to use
    them one must bring them forward. nvTextureRef takse care of this and is
    called before we render triangles

  * We maintain dirty rects for locks and blts

\***************************************************************************/

#ifndef WINNT
FLATPTR EXTERN_DDAPI DDHAL32_VidMemAlloc (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, DWORD dwXDim, DWORD dwYDim);
void EXTERN_DDAPI DDHAL32_VidMemFree (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, FLATPTR ptr);
#endif // #ifndef WINNT

/*
 * external
 */
void nvTextureAutoMipMap (PNVD3DTEXTURE);

#ifdef NV_TEX2
/*****************************************************************************
 * helpers
 * -------
 *****************************************************************************/

/*
 * nvTextureCalcMipMapSize
 *
 * calculates the amount of bytes needed with a given number of mipmaps
 */
DWORD nvTextureCalcMipMapSize
(
    DWORD dwLogU,
    DWORD dwLogV,
    DWORD dwMipMapLevels
)
{
    DWORD dwBits  = ((dwLogU + dwLogV) & 1) ? 0xaaaaaaaa : 0x55555555;
    DWORD dwTotal =  ((1 << (dwLogU + dwLogV + 1)) - 1) & dwBits;
    DWORD dwMask  = ~((1 << (dwLogU + dwLogV + 2 - dwMipMapLevels * 2)) - 1);
    return dwTotal & dwMask;
}

/*
 * GetTextureAddrFromOffset
 *
 * computes the surface address from a heap ID and an offset
 */
__inline DWORD GetTextureAddrFromOffset
(
    DWORD dwOffset,
    DWORD dwLocation
)
{
#ifndef WINNT
    return (dwLocation & NV4_TEXLOC_VID)
           ? (dwOffset + pDriverData->BaseAddress)
           : (dwLocation & NV4_TEXLOC_AGP)
                ? (dwOffset + pDriverData->GARTLinearBase)
                : (dwLocation & NV4_TEXLOC_PCI)
                    ? (dwOffset + pDriverData->pTextureHeapBase)
                    : dwOffset; // base address == 0 for sys memory
#else
    return (dwLocation & NV4_TEXLOC_VID)
           ? (dwOffset + pDriverData->ppdev->pjScreen)
           : (dwLocation & NV4_TEXLOC_AGP)
                ? (dwOffset + pDriverData->GARTLinearBase)
                : (dwLocation & NV4_TEXLOC_PCI)
                    ? (dwOffset + pDriverData->pTextureHeapBase)
                    : dwOffset; // base address == 0 for sys memory
#endif  // WINNT
}

/*
 * GetTextureOffsetFromAddr
 *
 * computes the surface address from a heap ID and an offset
 */
__inline DWORD GetTextureOffsetFromAddr
(
    DWORD dwAddr,
    DWORD dwLocation
)
{
#ifndef WINNT
    return (dwLocation & NV4_TEXLOC_VID)
           ? (dwAddr - pDriverData->BaseAddress)
           : (dwLocation & NV4_TEXLOC_AGP)
                ? (dwAddr - pDriverData->GARTLinearBase)
                : (dwLocation & NV4_TEXLOC_PCI)
                    ? (dwAddr - pDriverData->pTextureHeapBase)
                    : 0xffffffff; // HW cannot reach sys memory
#else
    return (dwLocation & NV4_TEXLOC_VID)
           ? (dwAddr - (DWORD)pDriverData->ppdev->pjScreen)
           : (dwLocation & NV4_TEXLOC_AGP)
                ? (dwAddr - pDriverData->GARTLinearBase)
                : (dwLocation & NV4_TEXLOC_PCI)
                    ? (dwAddr - pDriverData->pTextureHeapBase)
                    : 0xffffffff; // HW cannot reach sys memory
#endif  // WINNT
}

/*
 * nvTextureGrowDirtyRect
 *
 * grows dirty rect to include the given rectangle
 */
__inline void nvTextureGrowDirtyRect
(
    PNVD3DTEXTURE pTexture,
    DWORD         x0,
    DWORD         y0,
    DWORD         x1,
    DWORD         y1
)
{
#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvTextureGrowDirtyRect: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return;
    }
#endif //DEBUG

    /*
     * if rect is empty, trivially accept new
     */
    if ((pTexture->dwDirtyX0 == pTexture->dwDirtyX1)
     || (pTexture->dwDirtyY0 == pTexture->dwDirtyY1))
    {
        pTexture->dwDirtyX0 = x0;
        pTexture->dwDirtyY0 = y0;
        pTexture->dwDirtyX1 = x1;
        pTexture->dwDirtyY1 = y1;
        return;
    }

    /*
     * grow
     */
    pTexture->dwDirtyX0 = min(pTexture->dwDirtyX0,x0);
    pTexture->dwDirtyY0 = min(pTexture->dwDirtyY0,y0);
    pTexture->dwDirtyX1 = min(pTexture->dwDirtyX1,x1);
    pTexture->dwDirtyY1 = min(pTexture->dwDirtyY1,y1);

    /*
     * done
     */
    return;
}

/*
 * nvTextureDirtyRectEmpty
 *
 * returns true is dirty rect is empty
 */
__inline BOOL nvTextureDirtyRectEmpty
(
    PNVD3DTEXTURE pTexture
)
{
#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvTextureGrowDirtyRect: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return FALSE;
    }
#endif //DEBUG

    return (pTexture->dwDirtyX0 == pTexture->dwDirtyX1) || (pTexture->dwDirtyY0 == pTexture->dwDirtyY1);
}

/*
 * nvTextureAllocSurface
 *
 * allocates a surface in any of the given heaps
 */
BOOL nvTextureAllocSurface
(
    LPDDRAWI_DIRECTDRAW_GBL lpDriverObject,
    DWORD  dwAllocFlags,
    DWORD  dwAllocPreference,   // which heap gets preference (1 set bit only)
    DWORD  dwSize,
    DWORD *pdwAddr,
    DWORD *pdwResultFlags
)
{
    DWORD dwAddr;

    // we have to allow for the "legitimate" possibility of a zero-size texture
    // here since some dumb apps seem to set un-created textures from time to time
    // without actually using them to render.
    if (!dwSize)
    {
        DPF ("nvTextureAllocSurface: dwSize == 0");
        return FALSE;
    }

    /*
     * no preference would start with video memory
     */
    dwAllocPreference = (dwAllocPreference) ? (dwAllocPreference & NV4_TEXLOC_ANYWHERE)
                                            : NV4_TEXLOC_VID;

    /*
     * if the video res is 1k or larger
     */
    if (pCurrentContext && (pCurrentContext->lpLcl->lpGbl->wWidth >= pDriverData->regAGPTexCutOff))
    {
        /*
         * if we are allowed agp and video memory to allocate in
         */
        if ((dwAllocFlags & (NV4_TEXLOC_VID | NV4_TEXLOC_AGP)) == (NV4_TEXLOC_VID | NV4_TEXLOC_AGP))
        {
            /*
             * if we want video
             */
            if (dwAllocPreference == NV4_TEXLOC_VID)
            {
                /*
                 * override to agp
                 */
                dwAllocPreference = NV4_TEXLOC_AGP;
            }
        }
    }

    /*
     * try to allocate in all allowed memory heaps
     */
    while (dwAllocFlags)
    {
        /*
         * video
         */
        if (dwAllocFlags & dwAllocPreference & NV4_TEXLOC_VID)
        {
            if (pDriverData->regD3DEnableBits1 & REG_BIT1_VIDEO_TEXTURE_ENABLE)
            {
                /*
                 * Call DDRAW to allocate memory from the video memory heap.
                 */
                NVHEAP_ALLOC(dwAddr, dwSize, TYPE_TEXTURE);
                if (dwAddr)
                {
                    /*
                     * Increment count of video memory surfaces allocated.
                     */
                    pDriverData->DDrawVideoSurfaceCount++;

                    /*
                     * Handy statistics.
                     */
                    statTextureCountVideo++;
                    statTextureSizeVideo += dwSize;
                    //statTextureDimensionMinVideo = min(statTextureDimensionMinVideo, pTexture->dwWidth);
                    //statTextureDimensionMaxVideo = max(statTextureDimensionMaxVideo, pTexture->dwWidth);
                    /*
                     * done
                     */
#ifndef WINNT
                    *pdwAddr        = dwAddr;
#else
                    *pdwAddr        = (dwAddr + (DWORD)pDriverData->ppdev->pjScreen);
#endif  // WINNT
                    *pdwResultFlags = NV4_TEXLOC_VID;
                    return TRUE;
                }
            }

            /*
             * fail - don't try again
             */
            DPF_LEVEL(NVDBG_LEVEL_INFO,"nvTextureAllocSurface: Texture Surface in VID failed");
            dwAllocFlags &= ~NV4_TEXLOC_VID;
        }

        /*
         * agp
         */
        if (dwAllocFlags & dwAllocPreference & NV4_TEXLOC_AGP)
        {
            if (pDriverData->GARTLinearBase)
            {
                /*
                 * On AGP, allocate call DDRAW to allocate the AGP memory.
                 */
                dwAddr = (DWORD)AllocAGPHeap(lpDriverObject, dwSize);
                if (dwAddr)
                {
                    /*
                     * Handy statistics.
                     */
                    statTextureCountAgpHeap++;
                    statTextureSizeAgpHeap += dwSize;
                    //statTextureDimensionMinAgpHeap = min(statTextureDimensionMinAgpHeap, pTexture->dwWidth);
                    //statTextureDimensionMaxAgpHeap = max(statTextureDimensionMaxAgpHeap, pTexture->dwWidth);
                    /*
                     * done
                     */
                    *pdwAddr        = dwAddr;
                    *pdwResultFlags = NV4_TEXLOC_AGP;
                    return TRUE;
                }
            }

            /*
             * fail - don't try again
             */
            DPF_LEVEL(NVDBG_LEVEL_INFO,"nvTextureAllocSurface: Texture Surface in AGP failed");
            dwAllocFlags &= ~NV4_TEXLOC_AGP;
        }

        /*
         * pci
         */
        if (dwAllocFlags & dwAllocPreference & NV4_TEXLOC_PCI)
        {
            /*
             * On PCI, allocate from our internal texture heap.
             */
            dwAddr = AllocTextureHeap(dwSize);
            if (dwAddr)
            {
                /*
                 * Handy statistics.
                 */
                statTextureCountPciHeap++;
                statTextureSizePciHeap += dwSize;
                //statTextureDimensionMinPciHeap = min(statTextureDimensionMinPciHeap, pTexture->dwWidth);
                //statTextureDimensionMaxPciHeap = max(statTextureDimensionMaxPciHeap, pTexture->dwWidth);
                /*
                 * done
                 */
                *pdwAddr        = dwAddr;
                *pdwResultFlags = NV4_TEXLOC_PCI;
                return TRUE;
            }

            /*
             * fail - don't try again
             */
            DPF_LEVEL(NVDBG_LEVEL_INFO,"nvTextureAllocSurface: Texture Surface in PCI failed");
            dwAllocFlags &= ~NV4_TEXLOC_PCI;
        }

        /*
         * system memory
         */
        if (dwAllocFlags & dwAllocPreference & NV4_TEXLOC_SYS)
        {
            /*
             * For system meory, allocate from global heap.
             */
            dwAddr = (DWORD)GlobalAlloc(GPTR,dwSize);
            if (dwAddr)
            {
                /*
                 * done
                 */
                *pdwAddr        = dwAddr;
                *pdwResultFlags = NV4_TEXLOC_SYS;
                return TRUE;
            }

            /*
             * fail - don't try again
             */
            DPF_LEVEL(NVDBG_LEVEL_INFO,"nvTextureAllocSurface: Texture Surface in SYSMEM failed");
            dwAllocFlags &= ~NV4_TEXLOC_SYS;
        }

        /*
         * next
         */
        dwAllocPreference = (dwAllocPreference << 1) & NV4_TEXLOC_ANYWHERE;
        dwAllocPreference = dwAllocPreference ? dwAllocPreference
                                              : NV4_TEXLOC_VID;
    }

    /*
     * not allocated
     */
    DPF_LEVEL(NVDBG_LEVEL_INFO,"nvTextureAllocSurface: Texture Surface Not allocated");
    dbgD3DError();

    *pdwAddr        = 0;
    *pdwResultFlags = NV4_TEXLOC_NOWHERE;
    return FALSE;
}

/*
 * nvTextureFreeSurface
 *
 * frees a surface
 */
BOOL nvTextureFreeSurface
(
    LPDDRAWI_DIRECTDRAW_GBL lpDriverObject,
    DWORD dwAddr,
    DWORD dwSize,
    DWORD dwAllocFlags
)
{
    /*
     * video
     */
    if (dwAllocFlags & NV4_TEXLOC_VID)
    {
        /*
         * Call DDRAW to de-allocate memory from the video memory heap.
         */
        NVHEAP_FREE (dwAddr);

        /*
         * Decrement count of video memory surfaces allocated.
         */
        pDriverData->DDrawVideoSurfaceCount--;

        /*
         * Update texture statistics.
         */
        statTextureCountVideo--;
        statTextureSizeVideo -= dwSize;

        /*
         * done
         */
        return TRUE;
    }

    /*
     * agp
     */
    if (dwAllocFlags & NV4_TEXLOC_AGP)
    {
        /*
         * On AGP call DDRAW to de-allocate memory from AGP heap.
         */
        FreeAGPHeap(lpDriverObject, dwAddr);

        /*
         * Update texture statistics.
         */
        statTextureCountAgpHeap--;
        statTextureSizeAgpHeap -= dwSize;

        /*
         * done
         */
        return TRUE;
    }

    /*
     * pci
     */
    if (dwAllocFlags & NV4_TEXLOC_PCI)
    {
        /*
         * On PCI, de-allocate from our internal texture heap.
         */
        FreeTextureHeap(dwAddr);

        /*
         * Update texture statistics.
         */
        statTextureCountPciHeap--;
        statTextureSizePciHeap -= dwSize;

        /*
         * done
         */
        return TRUE;
    }

    /*
     * sys
     */
    if (dwAllocFlags & NV4_TEXLOC_SYS)
    {
        /*
         * free from global heap
         */
        GlobalFree ((HGLOBAL)dwAddr);

        /*
         * done
         */
        return TRUE;
    }

    /*
     * failed
     */
    return FALSE;
}

/*****************************************************************************
 * exported code
 * -------------
 *****************************************************************************/

/*
 * nvTextureRef
 *
 * called whenever the HW is about to use the texture.
 * the swizzle surface MUST be made valid (NV4_TEXFLAG_SWIZZLE_VALID)
 * this is also where we would automipmap if we are allowed to and the proper
 * timeout elapsed
 *
 */
BOOL nvTextureRef
(
    PNVD3DTEXTURE pTexture
)
{
    DWORD dwFlags;
    DWORD dwIndex;
    DWORD dwSurfaceFlags;
    BOOL  bForceHWUpdate = FALSE;

    /*
     * valid texture?
     */
    if (!pTexture) return FALSE;

    /*
     * extract texture info
     */
    dwFlags        = pTexture->dwTextureFlags;
    dwIndex        = TEX_SWIZZLE_INDEX(dwFlags);
    dwSurfaceFlags = pTexture->dwSwizzleFlags[dwIndex];

    /*
     * check if swizzle surface is valid
     */
    if (!(dwSurfaceFlags & NV4_TEXFLAG_VALID))
    {
        /*
         * make valid
         */
        if (!nvUpdateSwizzleSurface(pTexture)) return FALSE;
        /*
         * force hw reprogram
         */
        bForceHWUpdate = TRUE;
        /*
         * re-read texture info
         */
        dwFlags        = pTexture->dwTextureFlags;
        dwIndex        = TEX_SWIZZLE_INDEX(dwFlags);
        dwSurfaceFlags = pTexture->dwSwizzleFlags[dwIndex];
    }

#if 0 // obsolete
    /*
     * check if have dirty user mipmaps
     */
    if (pTexture->dwTextureFlags & NV4_TEXFLAG_MIPMAPS_DIRTY)
    {
        LPDDRAWI_DDRAWSURFACE_LCL lpLcl;
        LPDDRAWI_DDRAWSURFACE_LCL lclMip;
        /*
         * run down the chain and make sure the swizzled versions are up to date
         */
        lpLcl = pTexture->lpLcl;
        lclMip = lpLcl->lpAttachList->lpAttached;

        while (lclMip)
        {
            PNVD3DTEXTURE   pMipMap;

            pMipMap = (PNVD3DTEXTURE)lclMip->dwReserved1;
            if (pMipMap)
            {
                if (!(pMipMap->dwTextureFlags & NV4_TEXFLAG_SWIZZLE_VALID))
                {
                    /*
                     * make valid
                     */
                    nvUpdateSwizzleSurface (pMipMap);
                }
            }

            /*
             * next
             */
            lclMip = (lclMip->lpAttachList) ? lclMip->lpAttachList->lpAttached
                                            : NULL;
        }

        /*
         * update flags
         */
        pTexture->dwTextureFlags &= ~NV4_TEXFLAG_MIPMAPS_DIRTY;

        /*
         * force hw reprogram
         */
        bForceHWUpdate = TRUE;
    }
#endif //0

    /*
     * check if we have auto mipmaps enabled for this texture
     */
    if (dwFlags & NV4_TEXFLAG_AUTOMIPMAP)
    {
        /*
         * check if we still have to create the auto mipmaps this texture
         */
        if (!(dwFlags & NV4_TEXFLAG_AUTOMIPMAP_VALID))
        {
            /*
             * is it time to create them yet?
             */
            if (pTexture->dwAutoMipMapScene < pCurrentContext->dwSceneCount)
            {
                /*
                 * do we have an opportunity to still mipmap stuff in this scene?
                 */
                if (pCurrentContext->dwMipMapsInThisScene < NV3_TEXDEFAULT_MIPSPERSCENE)
                {
                    /*
                     * update flags
                     */
                    pTexture->dwTextureFlags    = dwFlags | NV4_TEXFLAG_AUTOMIPMAP_VALID;
                    pTexture->dwMipMapLevels    = pTexture->dwAutoMipMapCount;
                    pTexture->dwAutoMipMapScene = 0xffffffff; // inhibit further mipmapping (above test always fails)

                    /*
                     * perform mipmap (nv4mip.c)
                     */
                    nvTextureAutoMipMap (pTexture);

                    /*
                     * force hw reprogram
                     */
                    pCurrentContext->dwMipMapsInThisScene ++;
                    bForceHWUpdate = TRUE;

                    /*
                     * re-read texture info
                     */
                    dwFlags        = pTexture->dwTextureFlags;
                    dwIndex        = TEX_SWIZZLE_INDEX(dwFlags);
                    dwSurfaceFlags = pTexture->dwSwizzleFlags[dwIndex];
                }
            }
        }
    }
    else
    {
        /*
         * check is we have to bring user mipmaps forward
         */
        if ((dwSurfaceFlags & NV4_TEXMASK_MIPMASK) != NV4_TEXMASK_MIPMASK)
        {
            // txtodo
            // remember to store & load values
        }
    }

    /*
     * store cached values
     */
    pTexture->dwTextureFlags = dwFlags;

    /*
     * HW update
     */
    if (bForceHWUpdate)
    {
        pCurrentContext->dwStateChange = TRUE;
        NV_FORCE_TRI_SETUP(pCurrentContext);
    }

    /*
     * done
     */
    return TRUE;
}

/*
 * nvTextureTestBlock
 *
 * return TRUE if the CPU will have to block for this texture - similar
 *  logic to nvTextureBlock
 */
BOOL nvTextureTestBlock
(
    DWORD dwDate
)
{
    /*
     * aa override
     */
    if (pCurrentContext->dwAntiAliasFlags & (AA_MODE_SUPER | AA_MODE_SINGLECHUNK))
    {
        return FALSE; // capturing nature of AA implicitly provides syncronization
    }

    /*
     * check if we know it has retired
     */
    if (global.dwHWTextureDate > dwDate) return FALSE;

    /*
     * read latest HW date
     */
#ifndef WINNT
    global.dwHWTextureDate = *(DWORD*)(pDriverData->dwTMPseudoNotifierOffset + NV_PN_TEXTURE_RETIRE + pDriverData->BaseAddress);
#else
    global.dwHWTextureDate = *(DWORD*)(pDriverData->dwTMPseudoNotifierOffset + NV_PN_TEXTURE_RETIRE + pDriverData->ppdev->pjScreen);
#endif

    /*
     * test again
     */
    if (global.dwHWTextureDate > dwDate) return FALSE;

    /*
     * we will have to stall
     */
    return TRUE;
}

/*
 * nvTextureBlock
 *
 * block the CPU until HW has finished with the given frame.
 */
void nvTextureBlock
(
    DWORD dwDate
)
{
    /*
     * aa override
     */
    if (pCurrentContext->dwAntiAliasFlags & (AA_MODE_SUPER | AA_MODE_SINGLECHUNK))
    {
        return; // capturing nature of AA implicitly provides syncronization
    }

    /*
     * check if we know it has retired
     */
    if (global.dwHWTextureDate > dwDate) return;

    /*
     * read latest HW date
     */
#ifndef WINNT
    global.dwHWTextureDate = *(DWORD*)(pDriverData->dwTMPseudoNotifierOffset + NV_PN_TEXTURE_RETIRE + pDriverData->BaseAddress);
#else
    global.dwHWTextureDate = *(DWORD*)(pDriverData->dwTMPseudoNotifierOffset + NV_PN_TEXTURE_RETIRE + pDriverData->ppdev->pjScreen);
#endif  // WINNT

    /*
     * test again
     */
    if (global.dwHWTextureDate > dwDate) return;

    /*
     * check if we can do a partial stall
     *  this can happen when we wait for a date that is not the current SW date
     */
    if (dwDate < global.dwSWTextureDate)
    {
        for (;;)
        {
            /*
             * wait
             */
            NV_DELAY;

            /*
             * get new HW date
             */
#ifndef WINNT
            global.dwHWTextureDate = *(DWORD*)(pDriverData->dwTMPseudoNotifierOffset + NV_PN_TEXTURE_RETIRE + pDriverData->BaseAddress);
#else
            global.dwHWTextureDate = *(DWORD*)(pDriverData->dwTMPseudoNotifierOffset + NV_PN_TEXTURE_RETIRE + pDriverData->ppdev->pjScreen);
#endif  // WINNT

            /*
             * test again
             */
            if (global.dwHWTextureDate > dwDate) return;
        }
    }

    /*
     * date must be this frame so we sync hard. texture renaming tries to
     * avoid this case
     */
#ifdef NV_PROFILE
    NVP_START(NVP_T_TEXWAIT);
#endif
    NV_D3D_GLOBAL_SAVE();
    nvFlushDmaBuffers();
    NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
   nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
   NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
   nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_ROP, sizeSet2DSurfacesDestination);
#endif  // CACHE_FREECOUNT
#ifdef NV_PROFILE
    NVP_STOP(NVP_T_TEXWAIT);
    nvpLogTime (NVP_T_TEXWAIT,nvpTime[NVP_T_TEXWAIT]);
#endif
}

/*
 * nvTextureSetBlockPoint
 *
 * writes the proper semaphores to block texture accesses
 *  this should be called very infrequently
 */
void nvTextureSetBlockPoint
(
    void
)
{
    if (pCurrentContext->dwAntiAliasFlags & (AA_MODE_SUPER | AA_MODE_SINGLECHUNK))
    {
        return; // capturing nature of AA implicitly provides syncronization
    }

    /*
     * update block value
     */
    global.dwSWTextureDate ++;

    /*
     * write into command stream
     */
    nvPlacePsuedoNotifier (NV_PN_TEXTURE_RETIRE,global.dwSWTextureDate);
}

/*
 * nvTextureLock
 *
 * locks a texture so the user can party on the texels
 */
DWORD nvTextureLock
(
    PNVD3DTEXTURE    pTexture,
    LPDDHAL_LOCKDATA lpLockData
)
{
#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvTextureLock: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return 0;
    }
#endif // DEBUG

    /*
     * enter
     */
    NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
    nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_ROP, sizeSet2DSurfacesDestination);
#endif  // CACHE_FREECOUNT


    /*
     * make linear valid if we need to
     */
    nvUpdateLinearSurface (pTexture);

    /*
     * update texture flags for this lock - kill mipmaps and tag swizzled surface as invalid
     */
    TEX_SWIZZLE_FLAGS(pTexture,pTexture->dwTextureFlags) &= ~NV4_TEXFLAG_VALID;
    pTexture->dwTextureFlags   &= ~NV4_TEXFLAG_AUTOMIPMAP_VALID;
    pTexture->dwMipMapLevels    = 1;
    pTexture->dwAutoMipMapScene = pCurrentContext->dwSceneCount + NV4_TEXDEFAULT_AUTOMIPMAPTIME;

    /*
     * force hw reprogram
     */
    pCurrentContext->dwStateChange = TRUE;
    NV_FORCE_TRI_SETUP(pCurrentContext);

/* - txtodo - decide if this is really needed
    if ((++pTexture->dwUpdateCount) > NV4_TEXDEFAULT_MAXMODIFYCOUNT)
    {
        / *
         * modified too many times
         * /
        pTexture->dwTextureFlags   &= ~NV4_TEXFLAG_AUTOMIPMAP;
    }
*/

    /*
     * grow dirty rectangle to lock extents if it exists (else the whole thing)
     */
    if (lpLockData->bHasRect)
    {
        nvTextureGrowDirtyRect (pTexture,
                                lpLockData->rArea.left,
                                lpLockData->rArea.top,
                                lpLockData->rArea.right,
                                lpLockData->rArea.bottom);
    }
    else
    {
        pTexture->dwDirtyX0 = pTexture->dwDirtyY0
                            = 0;
        pTexture->dwDirtyX1 = pTexture->dwWidth;
        pTexture->dwDirtyY1 = pTexture->dwHeight;
    }

    /*
     * setup DDRAW with surface information
     */
    lpLockData->lpDDSurface->lpGbl->fpVidMem = pTexture->dwLinearAddr;
    lpLockData->lpDDSurface->lpGbl->lPitch   = pTexture->dwPitch;

    /*
     * sync HW - even when READONLY is specified (due to deswizzle possibly being done by HW)
     */
    nvTextureBlock (pTexture->dwRetireDate[NV4_TEXCOUNT_SWIZZLE]);

    /*
     * leave
     */
    NV_D3D_GLOBAL_SAVE();

    /*
     * done
     *  return that we did not handle the lock although we did - really weird ms semantics...
     */
    lpLockData->ddRVal = DD_OK;
#ifndef WINNT
    return DDHAL_DRIVER_NOTHANDLED/*DDHAL_DRIVER_HANDLED*/;
#else
    return DDHAL_DRIVER_HANDLED;
#endif  // WINNT
}

/*
 * nvTextureUnlock
 *
 * unlocks a texture after the user partied on the texels
 */
DWORD nvTextureUnlock
(
    PNVD3DTEXTURE      pTexture,
    LPDDHAL_UNLOCKDATA lpUnlockData
)
{
#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvTextureUnlock: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return 0;
    }
#endif // DEBUG

    /*
     * unlock ddraw's view of this texture
     */
    // txtodo - we might want to swizzle here instead of later

    /*
     * done
     */
    lpUnlockData->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

/*
 * nvTextureBlt
 *
 * performs a 16 or 32 bpp blt to a texture
 */
DWORD nvTextureBlt
(
    LPDDHAL_BLTDATA lpBltData
)
{
    /*
     * source surface
     */
    DWORD dwSrcAddr  = lpBltData->lpDDSrcSurface->lpGbl->fpVidMem;
    DWORD dwSrcAlloc = 0;
    DWORD dwSrcX0    = lpBltData->rSrc.left;
    DWORD dwSrcY0    = lpBltData->rSrc.top;
    DWORD dwSrcX1    = lpBltData->rSrc.right;
    DWORD dwSrcY1    = lpBltData->rSrc.bottom;
    DWORD dwSrcPitch = lpBltData->lpDDSrcSurface->lpGbl->lPitch;
    DWORD dwSrcBPP   = ((lpBltData->lpDDSrcSurface->lpGbl->ddpfSurface.dwFlags & DDPF_RGB)
                     ? lpBltData->lpDDSrcSurface->lpGbl->ddpfSurface.dwRGBBitCount
                     : pDriverData->bi.biBitCount) / 8;
    DWORD dwSrcLogW;
    DWORD dwSrcLogH;

    // txtodo - if source is a texture then we can do a swiz to swiz blt
    //PNVD3DTEXTURE pSrcTexture = (PNVD3DTEXTURE)lpBltData->lpDDSrcSurface->dwReserved1;

    /*
     * dest texture surface
     */
    PNVD3DTEXTURE pTexture   = (PNVD3DTEXTURE)lpBltData->lpDDDestSurface->dwReserved1;
    DWORD         dwDestX0   = lpBltData->rDest.left;
    DWORD         dwDestY0   = lpBltData->rDest.top;
    DWORD         dwDestX1   = lpBltData->rDest.right;
    DWORD         dwDestY1   = lpBltData->rDest.bottom;
    DWORD         dwDestLogW;
    DWORD         dwDestLogH;
    DWORD         dwIndex;

#ifdef NV_NULL_DRIVER
    lpBltData->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
#endif //NV_NULL_DRIVER

    /*
     * determine source texture allocation heap
     */
    if (lpBltData->lpDDSrcSurface->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
    {
        if (lpBltData->lpDDSrcSurface->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM)
        {
            if (pDriverData->GARTLinearBase)
                dwSrcAlloc = NV4_TEXLOC_AGP;
            else
                dwSrcAlloc = NV4_TEXLOC_PCI;
        }
        else if (lpBltData->lpDDSrcSurface->ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM)
        {
            dwSrcAlloc = NV4_TEXLOC_VID;
        }
        else { // unknown location - assume system (the safe option)
            dwSrcAlloc = NV4_TEXLOC_SYS;
        }
    }
    else
    {
        dwSrcAlloc = NV4_TEXLOC_SYS;
    }

    /*
     * enter
     */
    NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
    nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_ROP, sizeSet2DSurfacesDestination);
#endif  // CACHE_FREECOUNT

    /*
     * sanity checks
     */
    if (!pTexture)
    {
        // no dest texture structure
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureBlt - pTexture == NULL!!");
        dbgD3DError();
        // fail
        NV_D3D_GLOBAL_SAVE();
        lpBltData->ddRVal = DDERR_UNSUPPORTED;
        return DDHAL_DRIVER_HANDLED;
    }

    if (dwSrcBPP != pTexture->dwBPP)
    {
        if ((pDriverData->regPal8TextureConvert == REG_P8TC_NONE) ||
            (!(pTexture->dwTextureFlags & NV4_TEXFLAG_PALETTIZED))) {
            // different bit depths are not allowed
            DPF_LEVEL(NVDBG_LEVEL_ERROR,
                      "nvTextureBlt - Source and destination have different bit depths!!");
            dbgD3DError();
            // fail
            NV_D3D_GLOBAL_SAVE();
            lpBltData->ddRVal = DDERR_UNSUPPORTED;
            return DDHAL_DRIVER_HANDLED;
        }
    }

    if (dwSrcBPP == 1)
    {
        if (!(pTexture->dwTextureFlags & NV4_TEXFLAG_PALETTIZED)) {
            // 8 bpp non-palettized
            DPF_LEVEL(NVDBG_LEVEL_ERROR,
                      "nvTextureBlt - 8 bpp not supported with unpalettized textures!!");
            dbgD3DError();
            // fail
            NV_D3D_GLOBAL_SAVE();
            lpBltData->ddRVal = DDERR_UNSUPPORTED;
            return DDHAL_DRIVER_HANDLED;
        }
    }
    else if ((dwSrcBPP != 2) && (dwSrcBPP != 4))
    {
        // not 16 or 32 bpp
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureBlt - %d bpp not supported!!",dwSrcBPP*8);
        dbgD3DError();
        // fail
        NV_D3D_GLOBAL_SAVE();
        lpBltData->ddRVal = DDERR_UNSUPPORTED;
        return DDHAL_DRIVER_HANDLED;
    }

    /*
     * cache some texture info
     */
    dwIndex = TEX_SWIZZLE_INDEX(pTexture->dwTextureFlags);

    /*
     * get log width and height of whole surface - need this for swizzle
     */
    {
        dwSrcLogW  = lpBltData->lpDDSrcSurface->lpGbl->wWidth;
        dwSrcLogH  = lpBltData->lpDDSrcSurface->lpGbl->wHeight;
        dwDestLogW = lpBltData->lpDDDestSurface->lpGbl->wWidth;
        dwDestLogH = lpBltData->lpDDDestSurface->lpGbl->wHeight;
        __asm
        {
            mov eax,[dwSrcLogW]
            mov ebx,[dwSrcLogH]
            bsf ecx,eax
            bsf edx,ebx
            mov [dwSrcLogW],ecx
            mov [dwSrcLogH],edx

            mov eax,[dwDestLogW]
            mov ebx,[dwDestLogH]
            bsf ecx,eax
            bsf edx,ebx
            mov [dwSrcLogW],ecx
            mov [dwSrcLogH],edx
        }
    }

    /*
     * update stats
     */
    pTexture->dwBlitUpdateCount ++;

#ifdef NV_PROFILE
    nvpLogCount (NVP_C_BLTSIZE,(dwSrcX1 - dwSrcX0) * (dwSrcY1 - dwSrcY0) * dwSrcBPP);
#endif

    /*
     * do we have swizzled memory allocated yet?
     */
    if (!(TEX_SWIZZLE_FLAGS(pTexture,pTexture->dwTextureFlags) & NV4_TEXMASK_LOCATION))
    {
        /*
         * do we have enough info to do so now?
         */
        if (pTexture->dwMipMapSizeBytes)
        {
            nvAllocateInternalTexture (pTexture);
        }
        else
        {
            DPF_LEVEL(NVDBG_LEVEL_ERROR,
                      "nvTextureBlt - internal texture cannot be allocated (createsurface was never called)!!");
            dbgD3DError();
            // fail
            NV_D3D_GLOBAL_SAVE();
            lpBltData->ddRVal = DDERR_UNSUPPORTED;
            return DDHAL_DRIVER_HANDLED;
        }
    }

    /*
     * if source is in a HW reachable location, do blt directly from there
     *  implied syncronous operation and will not have to idle HW
     */
    if (dwSrcAlloc & (NV4_TEXLOC_PCI | NV4_TEXLOC_AGP | NV4_TEXLOC_VID))
    {
#ifdef NV_PROFILE
            NVP_START(NVP_T_TEXHWBLT);
#endif
        /*
         * make swizzle valid
         * - if it is not valid
         * - we already have dirty swizzled texels
         * - and we do not intend to blt the whole thing
         */
        if (dwDestX0 || dwDestY0 || (dwDestX1 != pTexture->dwWidth) || (dwDestY1 != pTexture->dwHeight))
        {
            if (!(pTexture->dwSwizzleFlags[dwIndex] & NV4_TEXFLAG_VALID)
              && (pTexture->dwTextureFlags & NV4_TEXFLAG_LINEAR_VALID))
            {
                if (!nvTextureDirtyRectEmpty(pTexture))
                {
                    nvUpdateSwizzleSurface (pTexture);
                }
            }
        }

        /*
         * perform lin -> swizzle copy
         */
        nvSwizzleBlt (dwSrcAddr,dwSrcAlloc,
                           dwSrcLogW,dwSrcLogH,
                           dwSrcX0,dwSrcY0,dwSrcX1,dwSrcY1,
                           dwSrcPitch,
                           NULL,
                      pTexture->dwSwizzleAddr[dwIndex],pTexture->dwSwizzleFlags[dwIndex] & NV4_TEXMASK_LOCATION,
                           dwDestLogW,dwDestLogH,
                           dwDestX0,dwDestY0,
                           pTexture->dwPitch,
                           pTexture,
                      dwSrcBPP,
                      NV_SWIZFLAG_SRCLINEAR | NV_SWIZFLAG_DESTSWIZZLED | NV_SWIZFLAG_ALLOWHW);

        /*
         * update flags
         */
        pTexture->dwTextureFlags          &= ~NV4_TEXFLAG_LINEAR_VALID;
        pTexture->dwSwizzleFlags[dwIndex] |=  NV4_TEXFLAG_VALID;
        if (pTexture->dwTextureFlags & NV4_TEXFLAG_AUTOMIPMAP)
        {
            pTexture->dwTextureFlags   &= ~NV4_TEXFLAG_AUTOMIPMAP_VALID;
            pTexture->dwMipMapLevels    = 1;
            pTexture->dwAutoMipMapScene = pCurrentContext->dwSceneCount + NV4_TEXDEFAULT_AUTOMIPMAPTIME;

            /*
             * force hw reprogram
             */
            pCurrentContext->dwStateChange = TRUE;
            NV_FORCE_TRI_SETUP(pCurrentContext);
        }
        if (pTexture->dwTextureFlags & NV4_TEXFLAG_USER_MIP_LEVEL)
        {
            /*
             * tag mipmap as current
             */
            ((PNVD3DTEXTURE)pTexture->hMipBaseTexture)->dwSwizzleFlags[dwIndex] &= ~(4 << pTexture->dwMipMapLevels);
        }

        /*
         * update dirty rect
         */
        nvTextureGrowDirtyRect (pTexture,dwDestX0,dwDestY0,dwDestX1,dwDestY1);

#ifdef NV_PROFILE
        NVP_STOP(NVP_T_TEXHWBLT);
        nvpLogTime (NVP_T_TEXHWBLT,nvpTime[NVP_T_TEXHWBLT]);
#endif

        /*
         * done
         */
        NV_D3D_GLOBAL_SAVE();
        lpBltData->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }

    /*
     * if we have a linear copy of the texture try to use a HW swizzle blit
     */
    if (pTexture->dwLinearAddr)
    {
        /*
         * can HW read from linear texture?
         */
        if (pTexture->dwTextureFlags & ((NV4_TEXLOC_VID | NV4_TEXLOC_AGP | NV4_TEXLOC_PCI) << NV4_TEXMASK_LINEAR_SHIFT))
        {
#ifdef NV_PROFILE
            NVP_START(NVP_T_TEXHWBLT);
#endif
            /*
             * make linear valid
             * - if it is not valid
             * - we already have dirty swizzled texels
             * - and we do not intend to blt the whole thing
             */
            if (dwDestX0 || dwDestY0 || (dwDestX1 != pTexture->dwWidth) || (dwDestY1 != pTexture->dwHeight))
            {
                if (!(pTexture->dwTextureFlags & (NV4_TEXFLAG_LINEAR_VALID)
                  && (pTexture->dwSwizzleFlags[dwIndex] & NV4_TEXFLAG_VALID)))
                {
                    if (!nvTextureDirtyRectEmpty(pTexture))
                    {
                        nvUpdateLinearSurface (pTexture);
                    }
                }
            }

            /*
             * perform linear copy
             */
            nvSwizzleBlt (dwSrcAddr,dwSrcAlloc,
                               dwSrcLogW,dwSrcLogH,
                               dwSrcX0,dwSrcY0,dwSrcX1,dwSrcY1,
                               dwSrcPitch,
                               NULL,
                          pTexture->dwLinearAddr,pTexture->dwTextureFlags >> NV4_TEXMASK_LINEAR_SHIFT,
                               dwDestLogW,dwDestLogH,
                               dwDestX0,dwDestY0,
                               pTexture->dwPitch,
                               pTexture,
                          dwSrcBPP,
                          NV_SWIZFLAG_SRCLINEAR | NV_SWIZFLAG_DESTLINEAR | NV_SWIZFLAG_ALLOWHW);
            /*
             * update flags
             */
            pTexture->dwSwizzleFlags[dwIndex] &= ~NV4_TEXFLAG_VALID;
            pTexture->dwTextureFlags          |=  NV4_TEXFLAG_LINEAR_VALID;
            if (pTexture->dwTextureFlags & NV4_TEXFLAG_AUTOMIPMAP)
            {
                pTexture->dwTextureFlags   &= ~NV4_TEXFLAG_AUTOMIPMAP_VALID;
                pTexture->dwMipMapLevels    = 1;
                pTexture->dwAutoMipMapScene = (pCurrentContext ? pCurrentContext->dwSceneCount : 0 ) + NV4_TEXDEFAULT_AUTOMIPMAPTIME;

                /*
                 * force hw reprogram
                 */
                pCurrentContext->dwStateChange = TRUE;
                NV_FORCE_TRI_SETUP(pCurrentContext);
            }
            if (pTexture->dwTextureFlags & NV4_TEXFLAG_USER_MIP_LEVEL)
            {
                /*
                 * tag mipmap as current
                 */
                ((PNVD3DTEXTURE)pTexture->hMipBaseTexture)->dwSwizzleFlags[dwIndex] &= ~(4 << pTexture->dwMipMapLevels);
            }

            /*
             * update dirty rect
             */
            nvTextureGrowDirtyRect (pTexture,dwDestX0,dwDestY0,dwDestX1,dwDestY1);

#ifdef NV_PROFILE
            NVP_STOP(NVP_T_TEXHWBLT);
            nvpLogTime (NVP_T_TEXHWBLT,nvpTime[NVP_T_TEXHWBLT]);
#endif

            /*
             * done
             */
            NV_D3D_GLOBAL_SAVE();
            lpBltData->ddRVal = DD_OK;
            return DDHAL_DRIVER_HANDLED;
        }
    }

    /*
     * see if we should do a staged HW swizzle blt
     */
    if ((!pTexture->dwLinearAddr)
     && (pTexture->dwSwizzleFlags[dwIndex] & NV4_TEXLOC_VID))
    {
        extern BOOL bTMHadToWaitForStagingSpace;

#if 0
        /*
         * prep call
         */
        bTMHadToWaitForStagingSpace = FALSE;
#endif

#ifdef NV_PROFILE
        NVP_START(NVP_T_TEXHWBLT);
#endif

        /*
         * do it
         */
        nvTMVideoTextureBlt (dwSrcAddr,dwSrcPitch,
                                dwSrcX0,dwSrcY0,dwSrcX1,dwSrcY1,
                             0,0, /* fake these parameters - nv_tex2 does not need this */
                                dwDestX0,dwDestY0,
                             pTexture->dwSwizzleOffset[dwIndex],
                                pTexture->dwMipMapBaseU,pTexture->dwMipMapBaseV,
                                pTexture->dwTextureColorFormat);

        /*
         * update flags
         */
        pTexture->dwTextureFlags          &= ~NV4_TEXFLAG_LINEAR_VALID;
        pTexture->dwSwizzleFlags[dwIndex] |= NV4_TEXFLAG_VALID;
        if (pTexture->dwTextureFlags & NV4_TEXFLAG_AUTOMIPMAP)
        {
            pTexture->dwTextureFlags   &= ~NV4_TEXFLAG_AUTOMIPMAP_VALID;
            pTexture->dwMipMapLevels    = 1;
            pTexture->dwAutoMipMapScene = (pCurrentContext ? pCurrentContext->dwSceneCount : 0) + NV4_TEXDEFAULT_AUTOMIPMAPTIME;

            /*
             * force hw reprogram
             */
            pCurrentContext->dwStateChange = TRUE;
            NV_FORCE_TRI_SETUP(pCurrentContext);
        }
        if (pTexture->dwTextureFlags & NV4_TEXFLAG_USER_MIP_LEVEL)
        {
            /*
             * tag mipmap as current
             */
            ((PNVD3DTEXTURE)pTexture->hMipBaseTexture)->dwSwizzleFlags[dwIndex] &= ~(4 << pTexture->dwMipMapLevels);
        }

        /*
         * update dirty rect
         */
        nvTextureGrowDirtyRect (pTexture,dwDestX0,dwDestY0,dwDestX1,dwDestY1);

        /*
         * block access to this texture
         */
        nvTextureSetBlockPoint ();

#ifdef NV_PROFILE
        NVP_STOP(NVP_T_TEXHWBLT);
        nvpLogTime (NVP_T_TEXHWBLT,nvpTime[NVP_T_TEXHWBLT]);
#endif

#if 0
        /*
         * check if we can balance staging space by allocating linear copies
         */
        if (bTMHadToWaitForStagingSpace
         && (pTexture->dwBlitUpdateCount >= NV4_TEXDEFAULT_MAXBLTCOUNT))
        {
            __asm int 3;
            goto forceLinearTextureCreation;
        }
#endif

        /*
         * done
         */
        NV_D3D_GLOBAL_SAVE();
        lpBltData->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }

#ifdef NV_PROFILE
    NVP_START(NVP_T_TEXSWBLT);
#endif

    /*
     * make swizzled valid if it is not valid and we already have dirty linear texels
     *   not likely but it can happen
     */
    if ((pTexture->dwTextureFlags & NV4_TEXFLAG_LINEAR_VALID)
    && !(pTexture->dwSwizzleFlags[dwIndex] & NV4_TEXFLAG_VALID))
    {
        if (!nvTextureDirtyRectEmpty(pTexture))
        {
            nvUpdateSwizzleSurface (pTexture);
        }
    }

#if 0 // txtodo
    /*
     * If the texture is in AGP memory we kick in a dynamic renamings scheme
     *  so the cpu will not wait for HW idle on blts
     */
    if (pTexture->dwSwizzleFlags[dwIndex] & NV4_TEXLOC_AGP)
    {
        /*
         * are we going to have to wait on this texture in order to blt?
         */
        if (nvTextureTestBlock(pTexture->dwRetireDate[dwIndex]))
        {
            __asm int 3;

            /*
             * check if the blt operation will fill the whole surface
             */
            /*if*/
            {
                /*
                 * do we have a free surface or do we need to quickly allocate one
                 */
                /*if*/
                {
                    /*
                     * quickly allocate new surface
                     */
                }

                /*
                 * switch to new surface
                 */
                // date = old
            }
        }
    }
#endif

    /*
     * move & swizzle texels
     */
    nvSwizzleBlt (dwSrcAddr,dwSrcAlloc,
                       dwSrcLogW,dwSrcLogH,
                       dwSrcX0,dwSrcY0,dwSrcX1,dwSrcY1,
                       dwSrcPitch,
                       NULL,
                  pTexture->dwSwizzleAddr[dwIndex],pTexture->dwSwizzleFlags[dwIndex] & NV4_TEXMASK_LOCATION,
                       pTexture->dwMipMapBaseU,pTexture->dwMipMapBaseV,
                       dwDestX0,dwDestY0,
                       pTexture->dwPitch,
                       pTexture,
                  dwSrcBPP,
                  NV_SWIZFLAG_SRCLINEAR | NV_SWIZFLAG_DESTSWIZZLED | NV_SWIZFLAG_ALLOWHW);

    /*
     * update flags
     */
    pTexture->dwTextureFlags          &= ~NV4_TEXFLAG_LINEAR_VALID;
    pTexture->dwSwizzleFlags[dwIndex] |=  NV4_TEXFLAG_VALID;

    /*
     * update auto-mipmaps
     *  if we have not rendered anything yet, do it now, else tag for
     *  later completion.
     */
    if (pTexture->dwTextureFlags & NV4_TEXFLAG_AUTOMIPMAP)
    {
        if (pCurrentContext->dwSceneCount)
        {
            /*
             * update flags
             */
            pTexture->dwTextureFlags   &= ~NV4_TEXFLAG_AUTOMIPMAP_VALID;
            pTexture->dwMipMapLevels    = 1;
            pTexture->dwAutoMipMapScene = (pCurrentContext ? pCurrentContext->dwSceneCount : 0) + NV4_TEXDEFAULT_AUTOMIPMAPTIME;

            /*
             * force hw reprogram
             */
            pCurrentContext->dwStateChange = TRUE;
            NV_FORCE_TRI_SETUP(pCurrentContext);
        }
        else
        {
            /*
             * update flags
             */
            pTexture->dwTextureFlags   |= NV4_TEXFLAG_AUTOMIPMAP_VALID;
            pTexture->dwMipMapLevels    = pTexture->dwAutoMipMapCount;
            pTexture->dwAutoMipMapScene = 0xffffffff; // inhibit further mipmapping

            /*
             * perform mipmap (nv4mip.c)
             */
            nvTextureAutoMipMap (pTexture);

            /*
             * force hw reprogram
             */
            pCurrentContext->dwStateChange = TRUE;
            NV_FORCE_TRI_SETUP(pCurrentContext);
        }
    }

    /*
     * update dirty rect
     */
    nvTextureGrowDirtyRect (pTexture,dwDestX0,dwDestY0,dwDestX1,dwDestY1);

//NVP_START (NVP_X_FLOAT0);

#if 0 // this does not work fast enough...
    /*
     * keep track of how many times we used the slow swizzle blit
     */
    if (pTexture->dwBlitUpdateCount >= NV4_TEXDEFAULT_MAXBLTCOUNT)
    {
#if 0
forceLinearTextureCreation:
#endif
        if (!pTexture->dwLinearAddr)
        {
            /*
             * create a linear texture for staging - if this succeeds we will use
             * HW swizzling in the future
             */
            DWORD dwAllocFlags = (pDriverData->GARTLinearBase) ? (NV4_TEXLOC_VID | NV4_TEXLOC_AGP)
                                                               : (NV4_TEXLOC_VID | NV4_TEXLOC_PCI);
            nvTextureAllocLinearSurface (pTexture,
                                         pTexture->dwPitch * pTexture->dwHeight,
                                         dwAllocFlags,
                                         dwAllocFlags & (NV4_TEXLOC_AGP | NV4_TEXLOC_PCI));
        }
    }
#endif //0

//NVP_STOP (NVP_X_FLOAT0);
//nvpLogTime (NVP_X_FLOAT0,nvpTime[NVP_X_FLOAT0]);

#ifdef NV_PROFILE
    NVP_STOP(NVP_T_TEXSWBLT);
    nvpLogTime (NVP_T_TEXSWBLT,nvpTime[NVP_T_TEXSWBLT]);
#endif

    /*
     * done
     */
    NV_D3D_GLOBAL_SAVE();
    lpBltData->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

/*
 * nvTextureAllocSwizzleSurface
 *
 * allocate a swizzled surface
 */
BOOL nvTextureAllocSwizzleSurface
(
    PNVD3DTEXTURE pTexture,
    DWORD         dwSize,
    DWORD         dwAllocFlags,
    DWORD         dwAllocPrefs
)
{
    DWORD dwIndex;
    DWORD dwAddr;

#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvTextureAllocSwizzleSurface: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return FALSE;
    }
#endif // DEBUG

    dwIndex = TEX_SWIZZLE_INDEX(pTexture->dwTextureFlags);

    if (pTexture->dwSwizzleAddr[dwIndex])
    {
        /*
         * the texure is allocated already - assume we are relocating it (destructive)
         */
        if (!nvTextureFreeSwizzleSurface(pTexture)) return FALSE;
    }

    /*
     * allocate texture surface
     */

    pTexture->dwSwizzleAddr[dwIndex] = pTexture->dwSwizzleOffset[dwIndex]
                                     = 0;
    if (!nvTextureAllocSurface(pTexture->lpDriverObject,dwAllocFlags,dwAllocPrefs,dwSize,
                               &dwAddr,&dwAllocFlags)) return FALSE;

    /*
     * get offset
     */
    pTexture->dwSwizzleAddr[dwIndex]   = dwAddr;
    pTexture->dwSwizzleOffset[dwIndex] = GetTextureOffsetFromAddr(dwAddr,dwAllocFlags);
    pTexture->dwSwizzleFlags[dwIndex] &= ~NV4_TEXMASK_LOCATION;
    pTexture->dwSwizzleFlags[dwIndex] |= dwAllocFlags;
    pTexture->dwTextureContextDma      = (dwAllocFlags & NV4_TEXLOC_VID)
                                       ? (NV054_FORMAT_CONTEXT_DMA_B)
                                       : (NV054_FORMAT_CONTEXT_DMA_A);

    /*
     * done
     */
    return TRUE;
}

/*
 * nvTextureFreeSwizzleSurface
 *
 * frees a swizzled surface
 */
BOOL nvTextureFreeSwizzleSurface
(
    PNVD3DTEXTURE pTexture
)
{
    DWORD i,j;

#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvTextureFreeSwizzleSurface: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return FALSE;
    }
#endif // DEBUG

    j = (pTexture->dwTextureFlags & NV4_TEXFLAG_SWIZZLE_INDEX_MAX) >> 4; // # of swizzle surfaces allocated
    for (i=0; i<=j; i++)
    {
        if (pTexture->dwSwizzleAddr[i])
        {
            if (!nvTextureFreeSurface(pTexture->lpDriverObject,
                                      pTexture->dwSwizzleAddr[i],
                                      pTexture->dwMipMapSizeBytes,
                                      pTexture->dwSwizzleFlags[i] & NV4_TEXMASK_LOCATION)) return FALSE;
        }

        pTexture->dwSwizzleAddr[i]   = pTexture->dwSwizzleOffset[i]
                                     = 0;
        pTexture->dwSwizzleFlags[i] &= ~NV4_TEXMASK_LOCATION;
    }


    return FALSE;
}

/*
 * nvTextureAllocLinearSurface
 *
 * allocate a linear surface
 */
BOOL nvTextureAllocLinearSurface
(
    PNVD3DTEXTURE pTexture,
    DWORD         dwSize,
    DWORD         dwAllocFlags,
    DWORD         dwAllocPrefs
)
{
    DWORD dwAddr;

#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvTextureAllocLinearSurface: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return FALSE;
    }
#endif // DEBUG

    if (pTexture->dwLinearAddr)
    {
        /*
         * the texure is allocated already - assume we are relocating it (destructive)
         */
        if (!nvTextureFreeLinearSurface(pTexture)) return FALSE;
    }

    /*
     * allocate texture surface
     */
    pTexture->dwLinearAddr = pTexture->dwLinearOffset
                           = 0;
    if (!nvTextureAllocSurface(pTexture->lpDriverObject,dwAllocFlags,dwAllocPrefs,dwSize,
                               &dwAddr,&dwAllocFlags)) return FALSE;

    /*
     * get offset
     */
    pTexture->dwLinearAddr    = dwAddr;
    pTexture->dwLinearOffset  = GetTextureOffsetFromAddr(dwAddr,dwAllocFlags);
    pTexture->dwTextureFlags &= ~(NV4_TEXLOC_ANYWHERE << NV4_TEXMASK_LINEAR_SHIFT);
    pTexture->dwTextureFlags |=  (dwAllocFlags        << NV4_TEXMASK_LINEAR_SHIFT);

    /*
     * done
     */
    return TRUE;
}

/*
 * nvTextureFreeLinearSurface
 *
 * frees a linear surface
 */
BOOL nvTextureFreeLinearSurface
(
    PNVD3DTEXTURE pTexture
)
{
#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvTextureFreeLinearSurface: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return FALSE;
    }
#endif // DEBUG

    if (pTexture->dwLinearAddr)
    {
        if (!nvTextureFreeSurface(pTexture->lpDriverObject,
                                  pTexture->dwLinearAddr,
                                  pTexture->dwMipMapSizeBytes,
                                  pTexture->dwTextureFlags >> NV4_TEXMASK_LINEAR_SHIFT)) return FALSE;
    }

    pTexture->dwLinearAddr = pTexture->dwLinearOffset
                           = 0;
    pTexture->dwTextureFlags &= ~(NV4_TEXLOC_ANYWHERE << NV4_TEXMASK_LINEAR_SHIFT);

    return FALSE;
}

/*
 * nvUpdateSwizzleSurface
 *
 * create and/or fill the swizzled surface from the linear surface
 * very much like the legacy nvLoadTexture
 */
BOOL nvUpdateSwizzleSurface
(
    PNVD3DTEXTURE pTexture
)
{
    DWORD dwIndex;

#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvUpdateSwizzleSurface: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return FALSE;
    }
#endif // DEBUG

    /*
     * get texture info
     */
    dwIndex = TEX_SWIZZLE_INDEX(pTexture->dwTextureFlags);

    /*
     * does surface exist already?
     */
    if (!TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags))
    {
        /*
         * no - create it now
         */
        DWORD dwAllocFlags = pDriverData->GARTLinearBase ? (NV4_TEXLOC_VID | NV4_TEXLOC_AGP)
                                                         : (NV4_TEXLOC_VID | NV4_TEXLOC_PCI);
        DWORD dwAllocPrefs = NV4_TEXLOC_NOWHERE; // preference
        if (pTexture->lpLcl)
        {
            /*
             * agp / pci
             */
            if (pTexture->lpLcl->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM)
            {
                if (pDriverData->GARTLinearBase)
                    dwAllocPrefs = NV4_TEXLOC_AGP;
                else
                    dwAllocPrefs = NV4_TEXLOC_PCI;
            }

            /*
             * video
             */
            if (dwAllocPrefs == NV4_TEXLOC_NOWHERE)
            {
                dwAllocPrefs = NV4_TEXLOC_VID;
            }
        }
        if (!nvTextureAllocSwizzleSurface(pTexture,pTexture->dwMipMapSizeBytes,dwAllocFlags,dwAllocPrefs))
        {
            return FALSE;
        }
    }
    else
    {
        /*
         * is surface already up to date?
         *  texels must be valid and
         */
        if (pTexture->dwSwizzleFlags[dwIndex] & NV4_TEXFLAG_VALID)
        {
            return TRUE;
        }
    }

#ifdef DEBUG
    /*
     * is the source up to date?
     *  if not we cannot do any work. the algorithm always assume at least one
     *  surface to be valid (have up-to-date texels)
     */
    if (!(pTexture->dwTextureFlags & NV4_TEXFLAG_LINEAR_VALID))
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvUpdateSwizzleSurface: Both swizzle and linear not valid.");
    }
#endif


    /*
     * perform swizzle (nv4swiz.c)
     */
    nvSwizzleBlt (pTexture->dwLinearAddr, pTexture->dwTextureFlags >> NV4_TEXMASK_LINEAR_SHIFT,
                      pTexture->dwMipMapBaseU,pTexture->dwMipMapBaseV,
                      pTexture->dwDirtyX0,pTexture->dwDirtyY0,pTexture->dwDirtyX1,pTexture->dwDirtyY1,
                      pTexture->dwPitch,
                      pTexture,
                  TEX_SWIZZLE_ADDR(pTexture,dwIndex),TEX_SWIZZLE_FLAGS(pTexture,dwIndex) & NV4_TEXMASK_LOCATION,
                      pTexture->dwMipMapBaseU,pTexture->dwMipMapBaseV,
                      pTexture->dwDirtyX0,pTexture->dwDirtyY0,
                      pTexture->dwPitch,
                      pTexture,
                  pTexture->dwBPP,
                  NV_SWIZFLAG_SRCLINEAR | NV_SWIZFLAG_DESTSWIZZLED | NV_SWIZFLAG_ALLOWHW
#ifdef HW_PAL8
                      | ((pTexture->dwTextureFlags & NV4_TEXFLAG_PALETTIZED) ?
                      NV_SWIZFLAG_PALETTIZED : 0)
#endif
                  );

    /*
     * update flags & reset dirty rectangle. kill auto mips if they are enabled
     */
    if (pTexture->dwTextureFlags & NV4_TEXFLAG_AUTOMIPMAP)
    {
        pTexture->dwTextureFlags   &= ~NV4_TEXFLAG_AUTOMIPMAP_VALID;
        pTexture->dwMipMapLevels    = 1;
        pTexture->dwAutoMipMapScene = pCurrentContext->dwSceneCount + NV4_TEXDEFAULT_AUTOMIPMAPTIME;

        /*
         * force hw reprogram
         */
        pCurrentContext->dwStateChange = TRUE;
        NV_FORCE_TRI_SETUP(pCurrentContext);
    }
    pTexture->dwSwizzleFlags[dwIndex] |= NV4_TEXFLAG_VALID;
    pTexture->dwDirtyX0 = pTexture->dwDirtyY0
                        = pTexture->dwDirtyX1
                        = pTexture->dwDirtyY1
                        = 0;
    /*
     * done
     */
    return TRUE;
}

/*
 * nvUpdateLinearSurface
 *
 * create and/or fill the linear surface from the swizzled surface
 */
BOOL nvUpdateLinearSurface
(
    PNVD3DTEXTURE pTexture
)
{
    DWORD dwIndex;

#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvUpdateLinearSurface: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return FALSE;
    }
#endif // DEBUG

    /*
     * cache texture info
     */
    dwIndex = TEX_SWIZZLE_INDEX(pTexture->dwTextureFlags);

    /*
     * does surface exist already?
     */
    if (!pTexture->dwLinearAddr)
    {
        /*
         * no - create it now
         */
        DWORD dwAllocFlags = pDriverData->GARTLinearBase ? (NV4_TEXLOC_VID | NV4_TEXLOC_AGP | NV4_TEXLOC_SYS)
                                                         : (NV4_TEXLOC_VID | NV4_TEXLOC_PCI | NV4_TEXLOC_SYS);
        if (!nvTextureAllocLinearSurface(pTexture,
                                         pTexture->dwPitch * pTexture->dwHeight,
                                         dwAllocFlags,
                                         dwAllocFlags & (NV4_TEXLOC_AGP | NV4_TEXLOC_PCI)))
        {
            return FALSE;
        }
    }
    else
    {
        /*
         * is surface already up to date?
         */
        if (pTexture->dwTextureFlags & NV4_TEXFLAG_LINEAR_VALID)
        {
            return TRUE;
        }
    }

    /*
     * Is the swizzle surface valid?
     *  if so then we deswizzle here. This is skipped usually when the app wants to
     *  lock the texture and haven't touched a pixel yet
     */
    if (pTexture->dwSwizzleFlags[dwIndex] & NV4_TEXFLAG_VALID)
    {
        /*
         * obtain access
         */
        nvTextureBlock (pTexture->dwRetireDate[dwIndex]);

        /*
         * perform deswizzle (nv4swiz.c)
         */
        nvSwizzleBlt (TEX_SWIZZLE_ADDR(pTexture,dwIndex),TEX_SWIZZLE_FLAGS(pTexture,dwIndex) & NV4_TEXMASK_LOCATION,
                          pTexture->dwMipMapBaseU,pTexture->dwMipMapBaseV,
                          pTexture->dwDirtyX0,pTexture->dwDirtyY0,pTexture->dwDirtyX1,pTexture->dwDirtyY1,
                          pTexture->dwPitch,
                          pTexture,
                      pTexture->dwLinearAddr, pTexture->dwTextureFlags >> NV4_TEXMASK_LINEAR_SHIFT,
                          pTexture->dwMipMapBaseU,pTexture->dwMipMapBaseV,
                          pTexture->dwDirtyX0,pTexture->dwDirtyY0,
                          pTexture->dwPitch,
                          pTexture,
                      pTexture->dwBPP,
                      NV_SWIZFLAG_SRCSWIZZLED | NV_SWIZFLAG_DESTLINEAR | NV_SWIZFLAG_ALLOWHW);
    }

    /*
     * update flags & reset dirty rectangle
     */
    pTexture->dwTextureFlags |= NV4_TEXFLAG_LINEAR_VALID;
    pTexture->dwDirtyX0       = pTexture->dwDirtyY0
                              = pTexture->dwDirtyX1
                              = pTexture->dwDirtyY1
                              = 0;

    /*
     * done
     */
    return TRUE;
}

#endif // NV_TEX2
/*****************************************************************************
 * Legacy code
 *****************************************************************************/

#ifndef NV_TEX2

BOOL nvFillTextureBuffer16Bpp(DWORD,DWORD,DWORD,DWORD,DWORD,DWORD);
BOOL nvFillTextureBuffer32Bpp(DWORD,DWORD,DWORD,DWORD,DWORD,DWORD);
BOOL nvHWSwizzleBlit(DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD);
BOOL nvVideoTextureBlit(WORD,WORD,WORD,WORD,WORD,WORD,DWORD,DWORD,WORD,WORD,WORD,WORD,WORD,WORD,DWORD,DWORD,DWORD,DWORD,WORD,WORD,DWORD);

#endif //!NV_TEX2


/*
 * --------------------------------------------------------------------------
 * NV Specific Texture routines. (legacy)
 * --------------------------------------------------------------------------
 */

/*
 * nvAllocateInternalTexture
 *
 * allocates the swizzled surface for a texture.
 * legacy code names 'internal' what we now call 'swizzle'
 *               and 'user'     what we now call 'linear'
 */
BOOL nvAllocateInternalTexture
(
    PNVD3DTEXTURE   pTexture
)
{
#ifdef NV_TEX2
    DWORD dwAllocFlags;
    DWORD dwAllocPrefs;

#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvAllocateInternalTexture: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return FALSE;
    }
    if (!pTexture->dwMipMapSizeBytes)
    {
        DPF ("nvAllocateInternalTexture: pTexture->dwMipMapSizeBytes == 0");
        dbgD3DError();
        __asm int 3;
        return FALSE;
    }
#endif // DEBUG

    /*
     * deterine alloc flags & prefs
     */
    dwAllocFlags = (pDriverData->GARTLinearBase) ? (NV4_TEXLOC_VID | NV4_TEXLOC_AGP)
                                                 : (NV4_TEXLOC_VID | NV4_TEXLOC_PCI);

    dwAllocPrefs = NV4_TEXLOC_NOWHERE; // preference
    if (pTexture->lpLcl)
    {
        /*
         * agp / pci
         */
        if (pTexture->lpLcl->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM)
        {
            if (pDriverData->GARTLinearBase)
                dwAllocPrefs = NV4_TEXLOC_AGP;
            else
                dwAllocPrefs = NV4_TEXLOC_PCI;
        }

        /*
         * video
         */
        if (dwAllocPrefs == NV4_TEXLOC_NOWHERE)
        {
            dwAllocPrefs = NV4_TEXLOC_VID;
        }
    }

    /*
     * allocate surface
     */
    if (!nvTextureAllocSwizzleSurface(pTexture,pTexture->dwMipMapSizeBytes,dwAllocFlags,dwAllocPrefs))
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvAllocateInternalTexture: Internal Texture Allocation Failed");
        dbgD3DError();
        return (FALSE);
    }

    /*
     * Handy statistics.
     */
    statTextureCountTotal++;
    statTextureOmniCountTotal++;
    statTextureSizeTotal += pTexture->dwMipMapSizeBytes;
    statTextureOmniSizeTotal += pTexture->dwMipMapSizeBytes;

    if (pTexture->lpLcl->lpGbl->ddpfSurface.dwFlags & DDPF_PALETTEINDEXED8) {
        statTextureOmniCountPalette++;
        statTextureOmniSizePalette += pTexture->dwMipMapSizeBytes;
    }

    /*
     * done
     */
    return TRUE;

#else // !NV_TEX2

#ifndef NVHEAPMGR
    LPDDRAWI_DIRECTDRAW_GBL pdrv = pTexture->lpDriverObject;
#endif
    /*
     * First check if Non-Local video memory explicitly specified.
     * If so, then don't try and put this texture in video memory unless there's no
     * room left in non-local memory.
     */
    if ((pTexture->lpLcl)
     && (pTexture->lpLcl->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM))
    {
        if (pDriverData->GARTLinearBase > 0)
        {
            /*
             * On AGP, allocate call DDRAW to allocate the AGP memory.
             */
            pTexture->dwTexturePointer = AllocAGPHeap(pTexture->lpDriverObject, pTexture->dwMipMapSizeBytes);
            if (pTexture->dwTexturePointer)
            {
                /*
                 * Calculate the offset from the AGP heap base to the start of the texture data.
                 */
                pTexture->dwTextureOffset     = pTexture->dwTexturePointer - pDriverData->GARTLinearBase;
                pTexture->dwTextureContextDma = NV054_FORMAT_CONTEXT_DMA_A;

                /*
                 * Handy statistics.
                 */
                statTextureCountAgpHeap++;
                statTextureSizeAgpHeap += pTexture->dwMipMapSizeBytes;
                statTextureDimensionMinAgpHeap = min(statTextureDimensionMinAgpHeap, pTexture->dwWidth);
                statTextureDimensionMaxAgpHeap = max(statTextureDimensionMaxAgpHeap, pTexture->dwWidth);
                DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateInternalTexture: AGP Memory Allocated");
            }
            else
                DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateInternalTexture: AGP Memory Allocation failed");

        }
        else
        {
            /*
             * On PCI, allocate from our internal texture heap.
             */
            pTexture->dwTexturePointer = AllocTextureHeap(pTexture->dwMipMapSizeBytes);
            if (pTexture->dwTexturePointer)
            {
                pTexture->dwTextureOffset     = pTexture->dwTexturePointer;
                pTexture->dwTextureContextDma = NV054_FORMAT_CONTEXT_DMA_A;

                /*
                 * Handy statistics.
                 */
                statTextureCountPciHeap++;
                statTextureSizePciHeap += pTexture->dwMipMapSizeBytes;
                statTextureDimensionMinPciHeap = min(statTextureDimensionMinPciHeap, pTexture->dwWidth);
                statTextureDimensionMaxPciHeap = max(statTextureDimensionMaxPciHeap, pTexture->dwWidth);
                DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateInternalTexture: PCI Memory Allocated");
            }
            else
                DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateInternalTexture: PCI Memory Allocation failed");
        }
    }

    /*
     * If the memory hasn't been allocated yet, try putting it in local video memory.
     */
    if ((!pTexture->dwTexturePointer)
     && (pDriverData->regD3DEnableBits1 & REG_BIT1_VIDEO_TEXTURE_ENABLE))
    {
        /*
         * Try putting the texture in video memory first if it meets the size requirement.
         */
        if (pTexture->dwWidth >= pDriverData->regMinVideoTextureSize)
        {
            /*
             * Call DDRAW to allocate memory from the video memory heap.
             */
#ifndef WINNT
            NVHEAP_ALLOC(pTexture->dwTexturePointer, pTexture->dwMipMapSizeBytes, TYPE_TEXTURE);
#else
            NVHEAP_ALLOC(pTexture->dwTexturePointer, (pTexture->dwMipMapSizeBytes + NV_TEXTURE_PAD + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad, TYPE_TEXTURE);
#endif  // !WINNT
            //pTexture->dwTexturePointer = DDHAL32_VidMemAlloc(pTexture->lpDriverObject, 0, (pTexture->dwMipMapSizeBytes + NV_TEXTURE_PAD + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad, 1);
            if (pTexture->dwTexturePointer)
            {
                /*
                 * Calculate the offset from the video heap base to the start of texture data.
                 */
                pTexture->dwTextureOffset      = ((pTexture->dwTexturePointer + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN) - pDriverData->BaseAddress;
                pTexture->dwTextureContextDma  = NV054_FORMAT_CONTEXT_DMA_B;
                pTexture->dwTextureFlags      |= NV3_TEXTURE_VIDEO_MEMORY;

                /*
                 * Show where the texture was allocated.
                 */
                pTexture->lpLcl->ddsCaps.dwCaps &= ~(DDSCAPS_NONLOCALVIDMEM | DDSCAPS_LOCALVIDMEM);
                pTexture->lpLcl->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;

                /*
                 * Increment count of video memory surfaces allocated.
                 */
                pDriverData->DDrawVideoSurfaceCount++;

                /*
                 * Handy statistics.
                 */
                statTextureCountVideo++;
                statTextureSizeVideo += pTexture->dwMipMapSizeBytes;
                statTextureDimensionMinVideo = min(statTextureDimensionMinVideo, pTexture->dwWidth);
                statTextureDimensionMaxVideo = max(statTextureDimensionMaxVideo, pTexture->dwWidth);
                DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateInternalTexture: Video Memory Allocated");
            }
            else
                DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateInternalTexture: Video Memory Allocation failed");
        }
    }

    /*
     * If the texture wasn't put into video memory, then try to put it in the system memory heap.
     */
    if (!pTexture->dwTexturePointer)
    {
        if (pDriverData->GARTLinearBase > 0)
        {
            /*
             * On AGP, allocate call DDRAW to allocate the AGP memory.
             */
            pTexture->dwTexturePointer = AllocAGPHeap(pTexture->lpDriverObject, pTexture->dwMipMapSizeBytes);
            if (pTexture->dwTexturePointer)
            {
                /*
                 * Calculate the offset from the AGP heap base to the start of the texture data.
                 */
                pTexture->dwTextureOffset     = pTexture->dwTexturePointer - pDriverData->GARTLinearBase;
                pTexture->dwTextureContextDma = NV054_FORMAT_CONTEXT_DMA_A;

                /*
                 * Show where the texture was allocated.
                 */
                pTexture->lpLcl->ddsCaps.dwCaps &= ~(DDSCAPS_NONLOCALVIDMEM | DDSCAPS_LOCALVIDMEM);
                pTexture->lpLcl->ddsCaps.dwCaps |= DDSCAPS_NONLOCALVIDMEM;

                /*
                 * Handy statistics.
                 */
                statTextureCountAgpHeap++;
                statTextureSizeAgpHeap += pTexture->dwMipMapSizeBytes;
                statTextureDimensionMinAgpHeap = min(statTextureDimensionMinAgpHeap, pTexture->dwWidth);
                statTextureDimensionMaxAgpHeap = max(statTextureDimensionMaxAgpHeap, pTexture->dwWidth);
                DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateInternalTexture: AGP Memory Allocated");
            }
            else
                DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateInternalTexture: AGP Memory Allocation failed");
        }
        else
        {
            /*
             * On PCI, allocate from our internal texture heap.
             */
            pTexture->dwTexturePointer = AllocTextureHeap(pTexture->dwMipMapSizeBytes);
            if (pTexture->dwTexturePointer)
            {
                pTexture->dwTextureOffset     = pTexture->dwTexturePointer;
                pTexture->dwTextureContextDma = NV054_FORMAT_CONTEXT_DMA_A;

                /*
                 * Show where the texture was allocated.
                 */
                pTexture->lpLcl->ddsCaps.dwCaps &= ~(DDSCAPS_NONLOCALVIDMEM | DDSCAPS_LOCALVIDMEM);
                pTexture->lpLcl->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;

                /*
                 * Handy statistics.
                 */
                statTextureCountPciHeap++;
                statTextureSizePciHeap += pTexture->dwMipMapSizeBytes;
                statTextureDimensionMinPciHeap = min(statTextureDimensionMinPciHeap, pTexture->dwWidth);
                statTextureDimensionMaxPciHeap = max(statTextureDimensionMaxPciHeap, pTexture->dwWidth);
                DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateInternalTexture: PCI Memory Allocated");
            }
            else
                DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateInternalTexture: PCI Memory Allocation failed");
        }
        if (pTexture->dwTexturePointer)
            pTexture->dwTextureFlags   &= ~NV3_TEXTURE_VIDEO_MEMORY;
        else
        {
            /*
             * Unable to allocate the texture.  Will need to do special
             * handling for this texture every time it is used.
             */
            if (pDriverData->regD3DEnableBits1 & REG_BIT1_VIDEO_TEXTURE_ENABLE)
            {
                /*
                 * Call DDRAW to allocate memory from the video memory heap.
                 */
#ifndef WINNT
                NVHEAP_ALLOC(pTexture->dwTexturePointer, pTexture->dwMipMapSizeBytes, TYPE_TEXTURE);
#else
                NVHEAP_ALLOC(pTexture->dwTexturePointer, (pTexture->dwMipMapSizeBytes + NV_TEXTURE_PAD + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad, TYPE_TEXTURE);
#endif  // !WINNT
                //pTexture->dwTexturePointer = DDHAL32_VidMemAlloc(pTexture->lpDriverObject, 0, (pTexture->dwMipMapSizeBytes + NV_TEXTURE_PAD + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad, 1);
                if (pTexture->dwTexturePointer)
                {
                    /*
                     * Calculate the offset from the video heap base to the start of texture data.
                     */
                    pTexture->dwTextureOffset      = ((pTexture->dwTexturePointer + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN) - pDriverData->BaseAddress;
                    pTexture->dwTextureContextDma  = NV054_FORMAT_CONTEXT_DMA_B;
                    pTexture->dwTextureFlags      |= NV3_TEXTURE_VIDEO_MEMORY;

                    /*
                     * Show where the texture was allocated.
                     */
                    pTexture->lpLcl->ddsCaps.dwCaps &= ~(DDSCAPS_NONLOCALVIDMEM | DDSCAPS_LOCALVIDMEM);
                    pTexture->lpLcl->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;

                    /*
                     * Increment count of video memory surfaces allocated.
                     */
                    pDriverData->DDrawVideoSurfaceCount++;

                    /*
                     * Handy statistics.
                     */
                    statTextureCountVideo++;
                    statTextureSizeVideo += pTexture->dwMipMapSizeBytes;
                    statTextureDimensionMinVideo = min(statTextureDimensionMinVideo, pTexture->dwWidth);
                    statTextureDimensionMaxVideo = max(statTextureDimensionMaxVideo, pTexture->dwWidth);
                    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateInternalTexture: Video Memory Allocated");
                }
                else
                    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateInternalTexture: Video Memory Allocation failed");
            }
        }
    }

    /*
     * If there is no valid texture pointer at this point, then there simply is no memory left
     * to allocate it.  Return the error.
     */
    if (!pTexture->dwTexturePointer)
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvAllocateInternalTexture: Internal Texture Allocation Failed");
        dbgD3DError();
        return (FALSE);
    }

    /*
     * Show that the internal texture memory has been allocated.
     * Also, since the memory has just been allocted, it can't possibly have valid
     * texture data, so mark it in need of loading.
     */
    pTexture->dwTextureFlags |= NV3_TEXTURE_INTERNAL_ALLOCATED;

    /*
     * Handy statistics.
     */
    statTextureCountTotal++;
    statTextureOmniCountTotal++;
    statTextureSizeTotal += pTexture->dwMipMapSizeBytes;
    statTextureOmniSizeTotal += pTexture->dwMipMapSizeBytes;

    if (pTexture->lpLcl->lpGbl->ddpfSurface.dwFlags & DDPF_PALETTEINDEXED8) {
        statTextureOmniCountPalette++;
        statTextureOmniSizePalette += pTexture->dwMipMapSizeBytes;
    }

    /*
     * Texture has been successfully created (but NOT loaded)
     */
    return (TRUE);
#endif // !NV_TEX2
}

/*
 * nvDestroyInternalTexture
 *
 * frees the swizzled surface for a texture.
 * legacy code names 'internal' what we now call 'swizzle'
 *               and 'user'     what we now call 'linear'
 */
BOOL nvDestroyInternalTexture
(
    PNVD3DCONTEXT   pContext,
    PNVD3DTEXTURE   pTexture
)
{
#ifdef NV_TEX2

#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvDestroyInternalTexture: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return FALSE;
    }
#endif // DEBUG

    /*
     * Validate the texture.
     */
    if (!pTexture) return FALSE;

    /*
     * Check for work
     */
    if (!TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags)) return TRUE;

    /*
     * Deallocate the internal texture surface.
     */
    if (!nvTextureFreeSwizzleSurface(pTexture))
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvDestroyInternalTexture: Internal Texture Deallocation Failed");
        dbgD3DError();
        return FALSE;
    }

    /*
     * Update texture statistics.
     */
    statTextureCountTotal--;
    statTextureSizeTotal -= pTexture->dwMipMapSizeBytes;

    /*
     * done
     */
    return TRUE;

#else !NV_TEX2

#ifndef NVHEAPMGR
    LPDDRAWI_DIRECTDRAW_GBL pdrv     = pTexture->lpDriverObject;
#endif

    /*
     * Validate the texture.
     */
    if (!pTexture)
        return (FALSE);

    /*
     * Deallocate the internal texture surface.
     */
    if (!pTexture->dwTexturePointer)
        return (TRUE);

    if (pTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
    {
        /*
         * Call DDRAW to de-allocate memory from the video memory heap.
         */
        NVHEAP_FREE (pTexture->dwTexturePointer);
        //DDHAL32_VidMemFree(pTexture->lpDriverObject, 0, pTexture->dwTexturePointer);

        /*
         * Decrement count of video memory surfaces allocated.
         */
        pDriverData->DDrawVideoSurfaceCount--;

        /*
         * Update texture statistics.
         */
        statTextureCountVideo--;
        statTextureSizeVideo -= pTexture->dwMipMapSizeBytes;
    }
    else
    {

        if (pDriverData->GARTLinearBase > 0)
        {
            /*
             * On AGP call DDRAW to de-allocate memory from AGP heap.
             */
            FreeAGPHeap(pTexture->lpDriverObject, pTexture->dwTexturePointer);

            /*
             * Update texture statistics.
             */
            statTextureCountAgpHeap--;
            statTextureSizeAgpHeap -= pTexture->dwMipMapSizeBytes;
        }
        else
        {
            /*
             * On PCI, de-allocate from our internal texture heap.
             */
            FreeTextureHeap(pTexture->dwTexturePointer);

            /*
             * Update texture statistics.
             */
            statTextureCountPciHeap--;
            statTextureSizePciHeap -= pTexture->dwMipMapSizeBytes;
        }
    }
    pTexture->dwTexturePointer  = (DWORD)NULL;
    pTexture->dwTextureOffset   = (DWORD)NULL;
    pTexture->dwTextureFlags   &= ~NV3_TEXTURE_INTERNAL_ALLOCATED;

    /*
     * Update texture statistics.
     */
    statTextureCountTotal--;
    statTextureSizeTotal -= pTexture->dwMipMapSizeBytes;
    return (TRUE);

#endif // !NV_TEX2
}


#ifndef NV_TEX2

void nvSwizzleTexture16Bpp
(
    DWORD   dwSrcX0,
    DWORD   dwSrcY0,
    DWORD   dwSrcX1,
    DWORD   dwSrcY1,
    DWORD   dwWidth,
    DWORD   dwHeight,
    DWORD   dwSrcPitch,
    DWORD   dwSrcLinearBase,
    DWORD   dwDstX0,
    DWORD   dwDstY0,
    DWORD   dwDstSwizzleBase
)
{
    DWORD   u0, u, v;
    DWORD   uInc, uInc2, vInc, uMask, vMask;
    DWORD   xSrcCurrent, ySrcCurrent;
    unsigned short *pSrcLinear;
    unsigned short *pDstSwizzle;

    U_INTERLEAVE(u0, dwDstX0, dwWidth, dwHeight);
    V_INTERLEAVE(v,  dwDstY0, dwWidth, dwHeight);
    U_INC2_MASK(uInc, uInc2, uMask, dwHeight);
    V_INC_MASK(vInc, vMask, dwWidth);
    pDstSwizzle = (unsigned short *)dwDstSwizzleBase;
    /*
     * X1 is exclusive, so if X1 is even, then the real last texel is on an odd boundry and will
     * be included in the last DWORD copied by the inner loop.
     * If X1 is odd, then the last texel is an even texel and the odd texel component will
     * need to be copied as a WORD value after the inner loop completes.
     */
    switch (((dwSrcX0 & 0x01) << 1) | (dwSrcX1 & 0x01))
    {
        /*
         * Even start, even end.
         */
        case 0:
            for (ySrcCurrent = dwSrcY0; ySrcCurrent < dwSrcY1; ySrcCurrent++, v = UV_INC(v, vInc, vMask))
            {
                pSrcLinear = (unsigned short *)(dwSrcLinearBase + (ySrcCurrent * dwSrcPitch));
                xSrcCurrent = dwSrcX0;
                u = u0;
                while (xSrcCurrent < dwSrcX1)
                {
                    *(DWORD *)(&pDstSwizzle[UV_OFFSET(u,v)]) = *(DWORD *)(&pSrcLinear[xSrcCurrent]);
                    xSrcCurrent += 2;
                    u = UV_INC(u, uInc2, uMask);
                }
            }
            break;

        /*
         * Even start, odd end.
         */
        case 1:
            for (ySrcCurrent = dwSrcY0; ySrcCurrent < dwSrcY1; ySrcCurrent++, v = UV_INC(v, vInc, vMask))
            {
                pSrcLinear = (unsigned short *)(dwSrcLinearBase + (ySrcCurrent * dwSrcPitch));
                xSrcCurrent = dwSrcX0;
                u = u0;
                // X1 is exclusive, so need to bail out of inner loop before doing last texel.
                while (xSrcCurrent < (dwSrcX1 - 1))
                {
                    *(DWORD *)(&pDstSwizzle[UV_OFFSET(u,v)]) = *(DWORD *)(&pSrcLinear[xSrcCurrent]);
                    xSrcCurrent += 2;
                    u = UV_INC(u, uInc2, uMask);
                }
                pDstSwizzle[UV_OFFSET(u,v)] = pSrcLinear[xSrcCurrent];
            }
            break;

        /*
         * Odd start, even end.
         */
        case 2:
            for (ySrcCurrent = dwSrcY0; ySrcCurrent < dwSrcY1; ySrcCurrent++, v = UV_INC(v, vInc, vMask))
            {
                pSrcLinear = (unsigned short *)(dwSrcLinearBase + (ySrcCurrent * dwSrcPitch));
                xSrcCurrent = dwSrcX0;
                u = u0;
                pDstSwizzle[UV_OFFSET(u,v)] = pSrcLinear[xSrcCurrent];
                xSrcCurrent++;
                u = UV_INC(u, uInc, uMask);
                while (xSrcCurrent < dwSrcX1)
                {
                    *(DWORD *)(&pDstSwizzle[UV_OFFSET(u,v)]) = *(DWORD *)(&pSrcLinear[xSrcCurrent]);
                    xSrcCurrent += 2;
                    u = UV_INC(u, uInc2, uMask);
                }
            }
            break;

        /*
         * Odd start, odd end.
         */
        case 3:
            for (ySrcCurrent = dwSrcY0; ySrcCurrent < dwSrcY1; ySrcCurrent++, v = UV_INC(v, vInc, vMask))
            {
                pSrcLinear = (unsigned short *)(dwSrcLinearBase + (ySrcCurrent * dwSrcPitch));
                xSrcCurrent = dwSrcX0;
                u = u0;
                pDstSwizzle[UV_OFFSET(u,v)] = pSrcLinear[xSrcCurrent];
                xSrcCurrent++;
                u = UV_INC(u, uInc, uMask);
                // X1 is exclusive, so need to bail out of inner loop before doing last texel.
                while (xSrcCurrent < (dwSrcX1 - 1))
                {
                    *(DWORD *)(&pDstSwizzle[UV_OFFSET(u,v)]) = *(DWORD *)(&pSrcLinear[xSrcCurrent]);
                    xSrcCurrent += 2;
                    u = UV_INC(u, uInc2, uMask);
                }
                pDstSwizzle[UV_OFFSET(u,v)] = pSrcLinear[xSrcCurrent];
            }
            break;
    }
}

void nvSwizzleTexture32Bpp
(
    DWORD   dwSrcX0,
    DWORD   dwSrcY0,
    DWORD   dwSrcX1,
    DWORD   dwSrcY1,
    DWORD   dwWidth,
    DWORD   dwHeight,
    DWORD   dwSrcPitch,
    DWORD   dwSrcLinearBase,
    DWORD   dwDstX0,
    DWORD   dwDstY0,
    DWORD   dwDstSwizzleBase
)
{
    DWORD   u0, u, v;
    DWORD   uInc, uInc2, vInc, uMask, vMask;
    DWORD   xSrcCurrent, ySrcCurrent;
    unsigned long *pSrcLinear;
    unsigned long *pDstSwizzle;

    U_INTERLEAVE(u0, dwDstX0, dwWidth, dwHeight);
    V_INTERLEAVE(v,  dwDstY0, dwWidth, dwHeight);
    U_INC2_MASK(uInc, uInc2, uMask, dwHeight);
    V_INC_MASK(vInc, vMask, dwWidth);
    pDstSwizzle = (unsigned long *)dwDstSwizzleBase;
    // X1, Y1 are exclusive.
    for (ySrcCurrent = dwSrcY0; ySrcCurrent < dwSrcY1; ySrcCurrent++, v = UV_INC(v, vInc, vMask))
    {
        pSrcLinear = (unsigned long *)(dwSrcLinearBase + (ySrcCurrent * dwSrcPitch));
        xSrcCurrent = dwSrcX0;
        u = u0;
        while (xSrcCurrent < dwSrcX1)
        {
            pDstSwizzle[UV_OFFSET(u,v)]   = pSrcLinear[xSrcCurrent];
            pDstSwizzle[UV_OFFSET(u,v)+1] = pSrcLinear[xSrcCurrent+1];
            xSrcCurrent += 2;
            u = UV_INC(u, uInc2, uMask);
        }
    }
}

/*
 * General purpose texture de-swizzling.  This routine is called at most
 * once for any given texture, so there's only really a need for a single
 * general purpose routine.
 */
void nvDeSwizzleTexture
(
    DWORD   dwWidth,
    DWORD   dwHeight,
    DWORD   dwPitch,
    DWORD   dwDstLinearBase,
    DWORD   dwSrcSwizzleBase,
    DWORD   dwTextureFormat
)
{
    DWORD   u, v;
    DWORD   xDstCurrent, yDstCurrent;
    DWORD   dwAdjustedWidth;
    unsigned long *pDstLinear;
    unsigned long *pSrcSwizzle;

    switch (dwTextureFormat)
    {
        case NV054_FORMAT_COLOR_LE_X1R5G5B5:
        case NV054_FORMAT_COLOR_LE_R5G6B5:
        case NV054_FORMAT_COLOR_LE_A1R5G5B5:
        case NV054_FORMAT_COLOR_LE_A4R4G4B4:
            // for 16-bit textures, divide the width by two since
            // we move four bytes at a time
            dwAdjustedWidth = dwWidth >> 1;
            break;
        default:
            dwAdjustedWidth = dwWidth;
            break;
    }

    pSrcSwizzle = (unsigned long *)dwSrcSwizzleBase;

    // for 1xN and Mx1 textures, no deswizzling is necessary. just copy
    if ((dwAdjustedWidth <= 1) || (dwHeight == 1)) {
        pDstLinear = (unsigned long *)dwDstLinearBase;
        for (u=0; u < ((dwHeight*dwPitch)>>2); u++)
            pDstLinear[u] = pSrcSwizzle[u];
        return;
    }

    for (yDstCurrent = 0;
         yDstCurrent < dwHeight;
         yDstCurrent++)
    {
        V_INTERLEAVE(v, yDstCurrent, dwWidth, dwHeight);
        pDstLinear = (unsigned long *)(dwDstLinearBase + (yDstCurrent * dwPitch));
        for (xDstCurrent = 0;
             xDstCurrent < dwAdjustedWidth;
             xDstCurrent++)
        {
            U_INTERLEAVE(u, xDstCurrent, dwWidth, dwHeight);
            pDstLinear[xDstCurrent] = pSrcSwizzle[UV_OFFSET(u,v)];
        }
    }
    return;
}

BOOL nvLoadTexture
(
    PNVD3DTEXTURE   pTexture
)
{
    DWORD                       texBPP;
    DWORD                       texWidth, texHeight, texPitch;
    DWORD                       logbaseu, logbasev, mipmaplevels;
    DWORD                       dwSrcLinearBase;
    DWORD                       dwDstSwizzleBase;
    DWORD                       dwTextureOffset;
    DWORD                       dwCurrentSize;
    DWORD                       dwTextureFlags;
    DWORD                       dwSrcLimit;
    DWORD                       dwImageColorFormat;
    DWORD                       dwSurfaceColorFormat;
    DWORD                       dwBytesPerPixel;
    PNVD3DTEXTURE               pTextureMipMapLevel;
    LPDDRAWI_DDRAWSURFACE_LCL   lclDDS;
    LPDDRAWI_DDRAWSURFACE_GBL   gblDDS;

    /*
     * Validate the context and texture pointers.
     */
    if (!pCurrentContext || !pTexture || !pTexture->dwTexturePointer)
        return (FALSE);

    /*
     * Validate the texture surface.
     */
    if (!(lclDDS = pTexture->lpLcl))
        return (FALSE);
    if (!(gblDDS = lclDDS->lpGbl))
        return (FALSE);
    if ((!gblDDS->fpVidMem) && (!pTexture->fpUserTexture))
        return (FALSE);

    /*
     * Get information about the source texture out of the texture structure.
     */
    dwTextureFlags  = pTexture->dwTextureFlags;
    texBPP          = pTexture->dwTextureColorFormat;
    texWidth        = lclDDS->lpGbl->wWidth;
    texHeight       = lclDDS->lpGbl->wHeight;
    texPitch        = lclDDS->lpGbl->lPitch;

    logbaseu        = pTexture->dwMipMapBaseU;
    logbasev        = pTexture->dwMipMapBaseV;
    mipmaplevels    = pTexture->dwMipMapLevels;

    /*
     * Setup frequently accessed globals.
     */
    NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
    nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetObject);
#endif  // CACHE_FREECOUNT

    /*
     * Get pointers to the source and destination textures.
     */
    dwSrcLinearBase  = (DWORD)((gblDDS->fpVidMem) ? gblDDS->fpVidMem : pTexture->fpUserTexture);
    if (pTexture->dwTextureFlags & NV3_TEXTURE_USER_MIP_LEVEL)
        dwDstSwizzleBase = pTexture->dwTexturePointer;
    else
    {
        if (pTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
        {
            dwDstSwizzleBase = pDriverData->BaseAddress + pTexture->dwTextureOffset;
#ifdef WINNT
            dwDstSwizzleBase += (ULONG) pDriverData->ppdev->pjScreen;
#endif
        }
        else
        {
            if (pDriverData->GARTLinearBase > 0)
                dwDstSwizzleBase = pTexture->dwTexturePointer;
            else
                dwDstSwizzleBase = GetPointerTextureHeap(pTexture->dwTextureOffset);
        }
    }
    dwTextureOffset = pTexture->dwTextureOffset;
    pTextureMipMapLevel = pTexture;
    while (mipmaplevels)
    {
        /*
         * Only need to swizzle the texture if it has been modified.
         */
        if (pTextureMipMapLevel->dwTextureFlags & NV3_TEXTURE_MODIFIED)
        {
            /*
             * Display the source texture on the screen. (DEBUG only)
             */
            dbgDisplayLinearTexture(pTextureMipMapLevel->lpLcl, pTextureMipMapLevel->lpLcl, pTextureMipMapLevel->dwTextureColorFormat);

            if (pTextureMipMapLevel->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
            {
                dwSrcLimit = (texPitch * texHeight) - 1;
                switch (texBPP)
                {
                    case NV054_FORMAT_COLOR_LE_X1R5G5B5:
                    case NV054_FORMAT_COLOR_LE_R5G6B5:
                    case NV054_FORMAT_COLOR_LE_A1R5G5B5:
                    case NV054_FORMAT_COLOR_LE_A4R4G4B4:
                        dwImageColorFormat   = NV077_SET_COLOR_FORMAT_LE_R5G6B5;
                        dwSurfaceColorFormat = NV052_SET_FORMAT_COLOR_LE_R5G6B5;
                        dwBytesPerPixel      = 2;
                        break;

                    case NV054_FORMAT_COLOR_LE_X8R8G8B8:
                    case NV054_FORMAT_COLOR_LE_A8R8G8B8:
                        dwImageColorFormat   = NV077_SET_COLOR_FORMAT_LE_A8R8G8B8;
                        dwSurfaceColorFormat = NV052_SET_FORMAT_COLOR_LE_A8R8G8B8;
                        dwBytesPerPixel      = 4;
                        break;
                }
                nvTMVideoTextureBlt (0,texPitch,0,0,texWidth,texHeight,
                                     dwSrcLinearBase,texPitch,0,0,
                                     dwTextureOffset,
                                     (WORD)logbaseu, (WORD)logbasev,
                                     texBPP);
            }
            else
            {
                /*
                 * Call correct swizzle routine based on the texture format (16bpp vs 32bpp)
                 */
                switch (texBPP)
                {
                    case NV054_FORMAT_COLOR_LE_X1R5G5B5:
                    case NV054_FORMAT_COLOR_LE_R5G6B5:
                    case NV054_FORMAT_COLOR_LE_A1R5G5B5:
                    case NV054_FORMAT_COLOR_LE_A4R4G4B4:
                        dwBytesPerPixel = 2;
                        nvSwizzleTexture16Bpp(0, 0,
                                              texWidth, texHeight,
                                              texWidth, texHeight,
                                              texPitch,
                                              dwSrcLinearBase,
                                              0, 0,
                                              dwDstSwizzleBase);
                        break;

                    case NV054_FORMAT_COLOR_LE_X8R8G8B8:
                    case NV054_FORMAT_COLOR_LE_A8R8G8B8:
                        dwBytesPerPixel = 4;
                        nvSwizzleTexture32Bpp(0, 0,
                                              texWidth, texHeight,
                                              texWidth, texHeight,
                                              texPitch,
                                              dwSrcLinearBase,
                                              0, 0,
                                              dwDstSwizzleBase);
                        break;
                }
            }
            dbgDisplayTexture((PNVD3DCONTEXT)pTextureMipMapLevel->hContext, pTextureMipMapLevel);
        }
        mipmaplevels--;

        if ((pDriverData->regD3DEnableBits1 & REG_BIT1_USER_MIPMAPS_ENABLE)
         && (lclDDS->ddsCaps.dwCaps & DDSCAPS_MIPMAP))
        {
            if ((lclDDS->lpAttachList)
                && (lclDDS = lclDDS->lpAttachList->lpAttached))
            {
                    dwCurrentSize     = ((1 << logbaseu) * dwBytesPerPixel) * (1 << logbasev);
                    texWidth          = lclDDS->lpGbl->wWidth;
                    texHeight         = lclDDS->lpGbl->wHeight;
                    texPitch          = lclDDS->lpGbl->lPitch;
                    dwSrcLinearBase   = (DWORD)lclDDS->lpGbl->fpVidMem;
                    dwDstSwizzleBase += dwCurrentSize;
                    logbaseu--;
                    logbasev--;
                    pTextureMipMapLevel = (PNVD3DTEXTURE)lclDDS->dwReserved1;
            }
            else
                mipmaplevels = 0;
        }
        else
        {
            mipmaplevels = 0;
        }
    }

    /*
     * Auto generate mipmaps
     */
    if (pTexture->dwTextureFlags & NV4_TEXTURE_AUTOMIPMAPPED)
    {
        nvTextureAutoMipMap (pTexture);
    }

    /*
     * Save frequently accessed globals.
     */
    NV_D3D_GLOBAL_SAVE();

    /*
     * Set the hardware texture format for this texture.
     */
    pTexture->dwTextureFlags  &= ~NV3_TEXTURE_MODIFIED;

    /*
     * Save the pointer to the texture data that the internal texture
     * was created from. This will be used for validation. If this pointer
     * is ever different from the pointer stored in the texture surface,
     * then the texture must be reloaded.
     */
    pTexture->fpTexture = (FLATPTR)pTexture->lpLcl->lpGbl->fpVidMem;

#ifdef  CACHE_FREECOUNT
    pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
    return (TRUE);
}

#endif //!NV_TEX2

/*
 * nvAllocateUserTexture
 *
 * Application texture memory allocation.
 * legacy code names 'internal' what we now call 'swizzle'
 *               and 'user'     what we now call 'linear'
 */
BOOL nvAllocateUserTexture
(
    PNVD3DTEXTURE   pTexture
)
{
#ifdef NV_TEX2

#ifdef DEBUG
    if (!pTexture)
    {
        DPF ("nvAllocateUserTexture: pTexture == NULL");
        dbgD3DError();
        __asm int 3;
        return FALSE;
    }
#endif // DEBUG

    /*
     * allocate if it does not already exist
     *
     * note that for mipmap chains we allocate seperate linear textures (unlike the linked version for swizzle)
     */
    if (!pTexture->dwLinearAddr)
    {
        DWORD dwAllocFlags = pDriverData->GARTLinearBase ? (NV4_TEXLOC_VID | NV4_TEXLOC_AGP | NV4_TEXLOC_SYS)
                                                         : (NV4_TEXLOC_VID | NV4_TEXLOC_PCI | NV4_TEXLOC_SYS);
        if (!nvTextureAllocLinearSurface(pTexture,
                                         pTexture->dwPitch * pTexture->dwHeight,
                                         dwAllocFlags,
                                         dwAllocFlags & (NV4_TEXLOC_AGP | NV4_TEXLOC_PCI)))
        {
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvAllocateUserTexture: User Texture Allocation Failed");
            dbgD3DError();
            return (FALSE);
        }
    }

    /*
     * tag as invalid - will be deswizzled before use
     */
    pTexture->dwTextureFlags &= ~NV4_TEXFLAG_LINEAR_VALID;

    /*
     * done
     */
    return TRUE;

#else //!NV_TEX2

    DWORD                       dwBlockSize;
    HGLOBAL                     hMem;
    PNVD3DTEXTURE               pMipMapTexture;
    LPDDRAWI_DDRAWSURFACE_LCL   lpLcl;
    LPDDRAWI_DDRAWSURFACE_GBL   lpGbl;
    LPDDRAWI_DDRAWSURFACE_LCL   lpMipMapLcl;
    LPDDRAWI_DDRAWSURFACE_GBL   lpMipMapGbl;

    /*
     * Make sure there is a valid texture pointer.
     */
    if ((!pTexture)
     || (!pTexture->lpLcl)
     || (!pTexture->lpLcl->lpGbl))
        return (FALSE);

    lpLcl = pTexture->lpLcl;
    lpGbl = lpLcl->lpGbl;

    /*
     * Allocate application's system memory texture surface if neccessary.
     */
    if (pTexture->dwTextureFlags & NV3_TEXTURE_USER_ALLOCATED)
    {
        /*
         * Reuse the system memory texture surface already allocated.
         */
        lpGbl->fpVidMem    = pTexture->fpUserTexture;
        lpGbl->dwReserved1 = (DWORD)pTexture->hUserTexture;

        if (pTexture->dwTextureFlags & NV3_TEXTURE_BLIT_UPDATE)
        {
            /*
             * The internal texture memory was modified with a blit but the user texture memory
             * was not updated.  The internal texture needs to be de-swizzled before the application
             * can be given access to it.
             */
            nvDeSwizzleTexture((DWORD)lpGbl->wWidth,
                               (DWORD)lpGbl->wHeight,
                               (DWORD)lpGbl->lPitch,
                               (DWORD)lpGbl->fpVidMem,
                               (DWORD)pTexture->dwTexturePointer,
                               (DWORD)pTexture->dwTextureColorFormat);

            /*
             * Once the texture has been de-swizzled once, it should never need to be
             * de-swizzled again.  Clear the flag, if the need for de-swizzling ever comes
             * up again, it will get re-set.
             */
            pTexture->dwTextureFlags &= ~NV3_TEXTURE_BLIT_UPDATE;
        }
    }
    else
    {
        /*
         * If this is a mipmap chain, we want to allocate memory for all mip
         * levels all at once, so find the base, mipmap texture and run
         * through the chain allocating and de-swizzling each level.
         * If this is not a mipmap chain, only only a single iteration will
         * be neccessary.
         */
        if (lpLcl->ddsCaps.dwCaps & DDSCAPS_MIPMAP)
        {
            if (!pTexture->hMipBaseTexture)
                pMipMapTexture = pTexture;
            else
                pMipMapTexture = (PNVD3DTEXTURE)pTexture->hMipBaseTexture;
        }
        else
        {
            /*
             * Not a user mipmap chain, 1 mipmap level (i.e. only base texture)
             */
            pMipMapTexture = pTexture;
        }

        /*
         * Traverse the mipmap chain and allocate memory for each level.
         */
        while (pMipMapTexture)
        {
            lpMipMapLcl = pMipMapTexture->lpLcl;
            lpMipMapGbl = lpMipMapLcl->lpGbl;

            /*
             * Allocate the user system memory texture surface.
             */
            dwBlockSize = (DWORD)(lpMipMapGbl->lPitch * (LONG)lpMipMapGbl->wHeight);
            hMem = GlobalAlloc(GHND | GMEM_SHARE, dwBlockSize);
            if (!hMem)
            {
                /*
                 * Could not allocate the internal texture memory.  Cleanup and return the error.
                 */
                lpMipMapGbl->dwReserved1      = (DWORD)NULL;
                pMipMapTexture->hUserTexture  = (HANDLE)NULL;
                pMipMapTexture->fpUserTexture = (FLATPTR)NULL;
                dbgD3DError();
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvAllocateUserTexture - User texture memory allocation failed");
                return (FALSE);
            }

            /*
             * Store the handle and pointer for the system memory surface in the global surface
             * data structure.
             */
            lpMipMapGbl->fpVidMem           = (FLATPTR)GlobalLock(hMem);
            lpMipMapGbl->dwReserved1        = (DWORD)hMem;
            pMipMapTexture->hUserTexture    = hMem;
            pMipMapTexture->fpUserTexture   = lpMipMapGbl->fpVidMem;
            pMipMapTexture->dwTextureFlags |= NV3_TEXTURE_USER_ALLOCATED;
            DPF_LEVEL(NVDBG_LEVEL_INFO, "nvAllocateUserTexture - User Texture = %08lx", pMipMapTexture->fpUserTexture);

            if (pMipMapTexture->dwTextureFlags & NV3_TEXTURE_BLIT_UPDATE)
            {
                /*
                 * The internal texture memory was modified with a blit but the user texture memory
                 * was not updated.  The internal texture needs to be de-swizzled before the application
                 * can be given access to it.
                 */
                nvDeSwizzleTexture((DWORD)lpMipMapGbl->wWidth,
                                   (DWORD)lpMipMapGbl->wHeight,
                                   (DWORD)lpMipMapGbl->lPitch,
                                   (DWORD)lpMipMapGbl->fpVidMem,
                                   (DWORD)pMipMapTexture->dwTexturePointer,
                                   (DWORD)pTexture->dwTextureColorFormat);

                /*
                 * Once the texture has been de-swizzled once, it should never need to be
                 * de-swizzled again.  Clear the flag, if the need for de-swizzling ever comes
                 * up again, it will get re-set.
                 */
                pMipMapTexture->dwTextureFlags &= ~NV3_TEXTURE_BLIT_UPDATE;
            }

            /*
             * Move to the next mipmap in the chain.
             */
            if ((lpMipMapLcl->lpAttachList)
             && (lpMipMapLcl->lpAttachList->lpAttached)
             && (lpMipMapLcl->lpAttachList->lpAttached->dwReserved1))
            {
                pMipMapTexture = (PNVD3DTEXTURE)lpMipMapLcl->lpAttachList->lpAttached->dwReserved1;
            }
            else
                pMipMapTexture = (PNVD3DTEXTURE)NULL;
        }
    }

    return (TRUE);

#endif //!NV_TEX2
}

/*
 * nvCreateTextureSurface
 *
 * Direct Draw Texture Surface routines.
 */
BOOL nvCreateTextureSurface
(
    LPDDHAL_CREATESURFACEDATA   lpCreateSurface,
    LPDDRAWI_DDRAWSURFACE_LCL   lclDDS,
    LPDDRAWI_DDRAWSURFACE_GBL   gblDDS
)
{
    BOOL            bCanAutoMipMap;
#ifndef NV_TEX2
    DWORD           logCurrentU, logCurrentV;
    DWORD           j;
    HGLOBAL         hMem;
#endif
    DWORD           dwTextureFlags;
    DWORD           dwTextureFormat;
    DWORD           width, height;
    DWORD           mipmaplevels;
    DWORD           texWidth, texHeight, texPitch, minPitch;
    DWORD           texSizeBytes;
    DWORD           logWidth, logHeight;
    DWORD           dwBytesPerPixel;
    DWORD           dwSrcBitCount;
    LONG            lPitch;
    HDDRVITEM       hTexture;
    PNVD3DTEXTURE   pTexture = 0;

    /*
     * It's possible that the internal texture data structure has already been allocated (in
     * the case where the texture handle was allocated first).  Try and get the pointer to
     * it.
     */
    DPF_LEVEL(NVDBG_LEVEL_FUNCTION_CALL, "nvCreateTextureSurface - lclDDS = %08lx, gblDDS = %08lx", lclDDS, gblDDS);
    hTexture = lclDDS->dwReserved1;
    pTexture = (PNVD3DTEXTURE)hTexture;

    /*
     * Initialize the texture flags.  Use the current flags in the texture data structure
     * if there is one.  Otherwise, start from scratch.
     */
    dwTextureFlags = (pTexture) ? pTexture->dwTextureFlags : 0;

    /*
     * Only supporting RGB 16 and 32bpp formats.
     */
    if (lclDDS->dwFlags & DDRAWISURF_HASPIXELFORMAT)
    {
        if ((gblDDS->ddpfSurface.dwFlags & DDPF_RGB)
         || ((gblDDS->ddpfSurface.dwFlags & DDPF_FOURCC)
         &&  (gblDDS->ddpfSurface.dwFourCC == FOURCC_RGB0)))
        {
            if ((gblDDS->ddpfSurface.dwRGBBitCount != 16)
             && (gblDDS->ddpfSurface.dwRGBBitCount != 32)
             && ((gblDDS->ddpfSurface.dwRGBBitCount != 8)
              || ((gblDDS->ddpfSurface.dwFlags & DDPF_PALETTEINDEXED8) != DDPF_PALETTEINDEXED8)))
            {
                /*
                 * Invalid pixel format. Return the error.
                 */
                dbgD3DError();
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Invalid texture format!");
                lpCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                return (FALSE);
            }
        }
    }

    /*
     * Determine the color format of the texture.
     */
    bCanAutoMipMap = FALSE;
    if (lclDDS->dwFlags & DDRAWISURF_HASPIXELFORMAT)
    {
        if (gblDDS->ddpfSurface.dwRGBBitCount == 16)
        {
            switch (gblDDS->ddpfSurface.dwRGBAlphaBitMask)
            {
                case 0x0000:
                    if (gblDDS->ddpfSurface.dwRBitMask == 0x7C00)
                    {
                        dwTextureFormat = NV054_FORMAT_COLOR_LE_X1R5G5B5;
                        bCanAutoMipMap  = TRUE;
                        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_X1R5G5B5");
                    }
                    else
                    {
                        dwTextureFormat = NV054_FORMAT_COLOR_LE_R5G6B5;
                        bCanAutoMipMap  = TRUE;
                        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_R5G6B5");
                    }
                    break;
                case 0x8000:
                    dwTextureFormat = NV054_FORMAT_COLOR_LE_A1R5G5B5;
                    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_A1R5G5B5");
                    break;
                case 0xF000:
                    dwTextureFormat = NV054_FORMAT_COLOR_LE_A4R4G4B4;
                    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_A4R4G4B4");
                    break;
            }
            dwBytesPerPixel = 2;
            dwSrcBitCount = 16;
        }
        else if (gblDDS->ddpfSurface.dwRGBBitCount == 32)
        {
            switch (gblDDS->ddpfSurface.dwRGBAlphaBitMask)
            {
                case 0x00000000:
                    dwTextureFormat = NV054_FORMAT_COLOR_LE_X8R8G8B8;
                    bCanAutoMipMap  = TRUE;
                    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_X8R8G8B8");
                    break;
                case 0xFF000000:
                    dwTextureFormat = NV054_FORMAT_COLOR_LE_A8R8G8B8;
                    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_A8R8G8B8");
                    break;
            }
            dwBytesPerPixel = 4;
            dwSrcBitCount = 32;
        }
        else if (gblDDS->ddpfSurface.dwRGBBitCount == 8)
        {
            if (pDriverData->regPal8TextureConvert == REG_P8TC_NONE)
            {
                dbgD3DError();
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Invalid texture format!");
                lpCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                return (FALSE);
            }
            if (gblDDS->ddpfSurface.dwFlags & DDPF_PALETTEINDEXED8)
            {
                switch (pDriverData->regPal8TextureConvert)
                {
                    case REG_P8TC_8TOR5G6B5:
                        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface: Allocating 8 bit palettized texture as a 565!");
                        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_R5G6B5");
                        dwTextureFormat = NV054_FORMAT_COLOR_LE_R5G6B5;
                        dwBytesPerPixel = 2;
                        break;
                    case REG_P8TC_8TOX1R5G5B5:
                        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface: Allocating 8 bit palettized texture as a X555!");
                        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_X1R5G5B5");
                        dwTextureFormat = NV054_FORMAT_COLOR_LE_X1R5G5B5;
                        dwBytesPerPixel = 2;
                        break;
                    case REG_P8TC_8TOA1R5G5B5:
                        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface: Allocating 8 bit palettized texture as a 1555!");
                        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_A1R5G5B5");
                        dwTextureFormat = NV054_FORMAT_COLOR_LE_A1R5G5B5;
                        dwBytesPerPixel = 2;
                        break;

                    case REG_P8TC_8TOX8R8G8B8:
                        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface: Allocating 8 bit palettized texture as a X888!");
                        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_X8R8G8B8");
                        dwTextureFormat = NV054_FORMAT_COLOR_LE_X8R8G8B8;
                        dwBytesPerPixel = 4;
                        break;
                    case REG_P8TC_8TOA8R8G8B8:
                        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface: Allocating 8 bit palettized texture as a 8888!");
                        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_A8R8G8B8");
                        dwTextureFormat = NV054_FORMAT_COLOR_LE_X8R8G8B8;
                        dwBytesPerPixel = 4;
                        break;
                    default:
                        dbgD3DError();
                        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Invalid texture format!");
                        lpCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                        return (FALSE);
                }
            }
            else if (gblDDS->ddpfSurface.dwFlags & DDPF_LUMINANCE)
            {
                DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface: Allocating 8 bit Luminance Texture!");
                DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_Y8");
                dwTextureFormat = NV054_FORMAT_COLOR_LE_X8R8G8B8;
                dwBytesPerPixel = 1;
            }
            else
            {
                dbgD3DError();
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Invalid texture format!");
                lpCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                return (FALSE);
            }
            dwSrcBitCount = 8;
#ifdef HW_PAL8
            dwTextureFlags |= NV4_TEXFLAG_PALETTIZED;
#endif
        }
        else
        {
            dbgD3DError();
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Invalid texture format!");
            lpCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            return (FALSE);
        }
    }
    else
    {
        /*
         * If the texture doesn't specify a pixel format, that means it's the same format
         * as the device.  One application that actually does this is Indy Racing League.
         */
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface - No Pixel format Given!");
        if (pDriverData->bi.biBitCount == 16)
        {
            dwTextureFormat = NV054_FORMAT_COLOR_LE_R5G6B5;
            DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_R5G6B5");
            dwBytesPerPixel = 2;
            dwSrcBitCount   = 16;
            bCanAutoMipMap  = TRUE;
        }
        else if (pDriverData->bi.biBitCount == 32)
        {
            dwTextureFormat = NV054_FORMAT_COLOR_LE_X8R8G8B8;
            DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface:TextureFormat = FORMAT_COLOR_LE_X8R8G8B8");
            dwBytesPerPixel = 4;
            dwSrcBitCount   = 32;
            bCanAutoMipMap  = TRUE;
        }
        else
        {
            dbgD3DError();
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Invalid texture format!");
            lpCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            return (FALSE);
        }
    }

    /*
     * Make sure there's a valid color format in the texture format register.
     */
    if (!dwTextureFormat)
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Texture format not set!!");

    if (pDriverData->regD3DEnableBits1 & REG_BIT1_RESTRICT_AUTO_MIPMAPS)
    {
        if (lclDDS->dwFlags & DDRAWISURF_HASCKEYSRCBLT)
            bCanAutoMipMap = FALSE;
    }
    else
        bCanAutoMipMap = TRUE;

    /*
     * Validate width, height and pitch. Get commonly used values local.
     */
    minPitch = (pDriverData->regPal8TextureConvert == REG_P8TC_NONE) ? 2 : 1;

    if (((texWidth  = (long)gblDDS->wWidth)  == 0)
     || ((texHeight = (long)gblDDS->wHeight) == 0)
     || ((texPitch  = (long)gblDDS->lPitch)  <  minPitch))
    {
        /*
         * Invalid texture size. Return the error.
         */
        dbgD3DError();
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Invalid texture size!");
        lpCreateSurface->ddRVal = DDERR_INVALIDPARAMS;
        return (FALSE);
    }

    /*
     * Validate that the texture dimensions are both powers of two.
     */
    if ((texWidth & (texWidth - 1))
     || (texHeight & (texHeight - 1)))
    {
        /*
         * The texture is not a power of two. Return the error.
         */
        dbgD3DError();
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Texture not power of 2");
        lpCreateSurface->ddRVal = DDERR_INVALIDPARAMS;
        return (FALSE);
    }

    /*
     * Determine which power of two by finding the bit position.
     */
    __asm
    {
        mov ecx,[texWidth]
        mov ebx,[texHeight]
        mov [width],ecx             // width = texWidth
        bsf eax,ecx                 //   486 instruction to count the bit position
        mov [height],ebx            // height = texHeight
        bsf edx,ebx                 //   486 instruction to count the bit position
        mov [logWidth],eax
        mov [logHeight],edx;
    }

    /*
     * Determine the number of MipMap levels based on the maximum image
     * size and the maximum number of MipMap levels. For auto-generated
     * MipMaps, this is based on a registry setting.  For user defined
     * MipMaps, this will be a combination of the number of levels supplied
     * by the application as well as the maximum number of levels set in
     * the registry.
     */
    if (lclDDS->ddsCaps.dwCaps & DDSCAPS_MIPMAP)
    {
        if (pDriverData->regD3DEnableBits1 & REG_BIT1_USER_MIPMAPS_ENABLE)
        {
            LPDDRAWI_DDRAWSURFACE_LCL   lclMip;

            /*
             * Rather than grab the mipmap count from the structure, I'm just
             * going to traverse the chain and manually count the levels.
             * This way I can't get it wrong.
             */
            mipmaplevels = 1;
            lclMip = lclDDS;
            while (lclMip
                && lclMip->lpAttachList)
            {
                mipmaplevels++;
                lclMip = lclMip->lpAttachList->lpAttached;
            }

            /*
             * Check if this is the base level or one of the mip levels.
             */
            if (lclDDS->lpAttachListFrom)
#ifdef NV_TEX2
                dwTextureFlags |= NV4_TEXFLAG_USER_MIP_LEVEL;
#else
                dwTextureFlags |= NV3_TEXTURE_USER_MIP_LEVEL;
#endif
        }
        else
        {
            mipmaplevels = 1;
        }
    }
    else
    {
        /*
         * Set the number of mipmap levels to auto generate.
         */
        mipmaplevels = 1L; // default
        /*
         * if we haven't drawn some frames yet and ...
         */
        if (bCanAutoMipMap
#ifndef NV_TEX2
         && (pCurrentContext && (pCurrentContext->dwSceneCount < 3))
#endif
           )
        {
            /*
             * the texture dimensions are okay
             */
            if ((logWidth + 1 == logHeight    )     // 2:1
             || (logWidth     == logHeight    )     // 1:1
             || (logWidth     == logHeight + 1))    // 1:2
            {
                if ((dwSrcBitCount != 8)
                 && (pDriverData->regMipMapLevels))
                {
                    mipmaplevels = min(logWidth,logHeight) + 1;
                    mipmaplevels = min(mipmaplevels, pDriverData->regMipMapLevels);
                }
            }
        }

        /*
         * update texture flags
         */
#ifdef NV_TEX2
        // test: disable auto mipmaps totally for now.
        //       also update mipmap disables to not redo HW if not really needed
        //dwTextureFlags |= (mipmaplevels > 1) ? NV4_TEXFLAG_AUTOMIPMAP : 0;
#else
        dwTextureFlags |= (mipmaplevels > 1) ? NV4_TEXTURE_AUTOMIPMAPPED : 0;
#endif
    }

    /*
     * Calculate the size of the NV3 format texture including mip maps.
     */
#ifdef NV_TEX2
    texSizeBytes = nvTextureCalcMipMapSize(logWidth,logHeight,mipmaplevels) * dwBytesPerPixel;
#else
    texSizeBytes    = 0;
    logCurrentU     = logWidth;
    logCurrentV     = logHeight;
    for (j = 0; j < mipmaplevels; j++)
    {
        texSizeBytes += (1 << (logCurrentU + logCurrentV)) * dwBytesPerPixel;
        logCurrentU--;
        logCurrentV--;
    }
#endif // !NV_TEX2

    /*
     * All the pertinent texture data has now been validated and calculated.
     * Time for allocating all the memory for the texture surface and texture data structures.
     *
     * Calculate the pitch for the application system memory texture.
     */
    lPitch = texWidth * dwBytesPerPixel;
    gblDDS->lPitch = lPitch;

    /*
     * Allocate an internal texture data structure if there isn't one allocated yet.
     */
    if (!pTexture)
    {
        /*
         * Internal texture data structure not allocated yet.  Allocate it now.
         */
#ifndef OLDSTUFF
        hTexture = (HDDRVITEM)HEAPALLOC((HANDLE)pDriverData->hTextures_gbl,
                                sizeof(NVD3DTEXTURE));
#else
        hTexture = (HDDRVITEM)HeapAlloc((HANDLE)pDriverData->hTextures_gbl,
                                        HEAP_ZERO_MEMORY,
                                        sizeof(NVD3DTEXTURE));
#endif // #ifndef OLDSTUFF
        pTexture = (PNVD3DTEXTURE)hTexture;
        if (!pTexture)
        {
            /*
             * Could not allocate the internal data structure.  Cleanup and return the error.
             */
            gblDDS->fpVidMem        = (FLATPTR)NULL;
            gblDDS->dwReserved1     = (DWORD)NULL;
            lpCreateSurface->ddRVal = DDERR_OUTOFVIDEOMEMORY;
            dbgD3DError();
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Could not allocate internal data structure!");
            return (FALSE);
        }
        pDriverData->dwD3DTextureCount++;

        /*
         * Store the handle for the internal texture data structure in the driver reserved field.
         */
        lclDDS->dwReserved1 = hTexture;

        /*
         * Make sure a few key fields are intialized to zero.
         */
/* allocated with HEAP_ZERO_MEMORY - not needed
        pTexture->dwTexturePointer  = 0;
        pTexture->dwTextureOffset   = 0;
        pTexture->fpTexture         = 0;
        pTexture->hUserTexture      = 0;
        pTexture->fpUserTexture     = 0;
*/
        pTexture->dwDriverData      = (DWORD)pDriverData;

        /*
         * Update the texture list.
         */
        pTexture->pTexturePrev = (PNVD3DTEXTURE)NULL;
        pTexture->pTextureNext = (PNVD3DTEXTURE)pDriverData->dwTextureListHead;
        if (pTexture->pTextureNext)
            pTexture->pTextureNext->pTexturePrev = pTexture;
        pDriverData->dwTextureListHead = (DWORD)pTexture;
    }
    DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface - pTexture = %08lx", pTexture);

    /*
     * Initialize texture information
     */
    pTexture->lpDriverObject       = lpCreateSurface->lpDD;
    pTexture->lpLcl                = lclDDS;
    pTexture->dwWidth              = texWidth;
    pTexture->dwHeight             = texHeight;
    pTexture->dwPitch              = (DWORD)lPitch;
    pTexture->dwMipMapBaseU        = logWidth;
    pTexture->dwMipMapBaseV        = logHeight;
    pTexture->dwMipMapSizeBytes    = texSizeBytes;
    pTexture->dwTextureColorFormat = dwTextureFormat;
#ifdef NV_TEX2
    pTexture->dwBPP                = dwBytesPerPixel;
    pTexture->dwMipMapLevels       = (dwTextureFlags & NV4_TEXFLAG_AUTOMIPMAP) ? 1 : mipmaplevels;
    pTexture->dwTextureFlags       = dwTextureFlags;
    pTexture->dwAutoMipMapScene    = NV4_TEXDEFAULT_AUTOMIPMAPTIME;
    if (pCurrentContext) pTexture->dwAutoMipMapScene += pCurrentContext->dwSceneCount;
    pTexture->dwAutoMipMapCount    = mipmaplevels;
    pTexture->dwDirtyX0            = pTexture->dwDirtyY0
                                   = 0;
    pTexture->dwDirtyX1            = pTexture->dwWidth;
    pTexture->dwDirtyY1            = pTexture->dwHeight;
    pTexture->dwLockUpdateCount    = 0;
    pTexture->dwBlitUpdateCount    = 0;
#else
    pTexture->dwUpdateCount        = 0;
    pTexture->dwMipMapLevels       = mipmaplevels;
    pTexture->dwTextureFlags       = dwTextureFlags | (bCanAutoMipMap ? NV4_TEXTURE_CAN_AUTO_MIPMAP : 0);
#endif

    /*
     * update user mip levels - this code assumes that the
     * base texture is already allocated
     */
#ifdef NV_TEX2
    if (dwTextureFlags & NV4_TEXFLAG_USER_MIP_LEVEL)
#else
    if (dwTextureFlags & NV3_TEXTURE_USER_MIP_LEVEL)
#endif
    {
        DWORD                       dwMipMapLevel;
        DWORD                       dwMipMapOffset;
        PNVD3DTEXTURE               pMipBaseTexture;
        LPDDRAWI_DDRAWSURFACE_LCL   lclMip;

        /*
         * Find the base texture handle.
         */
        dwMipMapLevel = 1;
        lclMip = lclDDS->lpAttachListFrom->lpAttached;
        while (lclMip->lpAttachListFrom)
        {
            lclMip = lclMip->lpAttachListFrom->lpAttached;
            dwMipMapLevel++;
        }
        pTexture->hMipBaseTexture = lclMip->dwReserved1;

        /*
         * Calculate the starting offset and pointer for this
         * mipmap level.
         */
        pMipBaseTexture = (PNVD3DTEXTURE)pTexture->hMipBaseTexture;
        if (pMipBaseTexture)
        {
#ifdef NV_TEX2
            DWORD dwIndex = TEX_SWIZZLE_INDEX(pMipBaseTexture->dwTextureFlags);

            dwMipMapOffset = nvTextureCalcMipMapSize(pMipBaseTexture->dwMipMapBaseU,
                                                     pMipBaseTexture->dwMipMapBaseV,
                                                     dwMipMapLevel) * dwBytesPerPixel;
#else
            logCurrentU    = pMipBaseTexture->dwMipMapBaseU;
            logCurrentV    = pMipBaseTexture->dwMipMapBaseV;
            dwMipMapOffset = 0;
            for (j = dwMipMapLevel; j > 0; j--)
            {
                dwMipMapOffset += ((1 << logCurrentU) * dwBytesPerPixel) * (1 << logCurrentV);
                logCurrentU--;
                logCurrentV--;
            }
#endif //!NV_TEX2
            pTexture->dwTextureContextDma = pMipBaseTexture->dwTextureContextDma;

#ifdef NV_TEX2

            pTexture->dwTextureFlags  &= ~(NV4_TEXMASK_LINEAR_LOCATION | NV4_TEXFLAG_SWIZZLE_INDEX);
            pTexture->dwTextureFlags  |= dwIndex | (pMipBaseTexture->dwTextureFlags & NV4_TEXMASK_LINEAR_LOCATION);
            TEX_SWIZZLE_FLAGS(pTexture,dwIndex) = (TEX_SWIZZLE_FLAGS(pTexture,dwIndex) & ~NV4_TEXMASK_LOCATION)
                                                | (TEX_SWIZZLE_FLAGS(pMipBaseTexture,dwIndex) & NV4_TEXMASK_LOCATION);
            pTexture->dwSwizzleOffset[dwIndex]  = pMipBaseTexture->dwSwizzleOffset[dwIndex] + dwMipMapOffset;

            pTexture->dwSwizzleAddr[dwIndex]    = GetTextureAddrFromOffset(pTexture->dwSwizzleOffset[dwIndex],TEX_SWIZZLE_FLAGS(pTexture,dwIndex) & NV4_TEXMASK_LOCATION);
#else
            pTexture->dwTextureOffset = pMipBaseTexture->dwTextureOffset + dwMipMapOffset;
            if (pMipBaseTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
            {
#ifndef WINNT
                pTexture->dwTexturePointer = pDriverData->BaseAddress + pTexture->dwTextureOffset;
#else
                pTexture->dwTexturePointer = pDriverData->ppdev->pjScreen + pTexture->dwTextureOffset;
#endif
                pTexture->dwTextureFlags |= NV3_TEXTURE_VIDEO_MEMORY;
            }
            else
            {
                if (pDriverData->GARTLinearBase > 0)
                    pTexture->dwTexturePointer = pMipBaseTexture->dwTexturePointer + dwMipMapOffset;
                else
                    pTexture->dwTexturePointer = GetPointerTextureHeap(pMipBaseTexture->dwTextureOffset) + dwMipMapOffset;
            }
#endif // !NV_TEX2
        }
    }
    else
        pTexture->hMipBaseTexture = 0;

    /*
     * Allocate the internal texture surface if it's not already allocated.
     */
#ifdef NV_TEX2
    if ((!TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags))
      && !(pTexture->dwTextureFlags & NV4_TEXFLAG_USER_MIP_LEVEL))
#else
    if ((!(pTexture->dwTextureFlags & NV3_TEXTURE_INTERNAL_ALLOCATED))
     && (!(pTexture->dwTextureFlags & NV3_TEXTURE_USER_MIP_LEVEL)))
#endif
    {
        /*
         * Setup frequently accessed globals.
         */
        NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
        nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_ROP, sizeSet2DSurfacesDestination);
#endif  // CACHE_FREECOUNT

        /*
         * Allocate memory for NV internal texture format buffer.
         */
        if (!nvAllocateInternalTexture(pTexture))
        {
            /*
             * Could not allocate the internal texture memory.  Cleanup and return the error.
             */
            gblDDS->fpVidMem        = (FLATPTR)NULL;
            gblDDS->dwReserved1     = (DWORD)NULL;
#ifndef NV_TEX2
            pTexture->hUserTexture  = (HANDLE)NULL;
            pTexture->fpUserTexture = (FLATPTR)NULL;
#endif
            nvDestroyTextureStructure(pTexture);
            lpCreateSurface->ddRVal = DDERR_OUTOFVIDEOMEMORY;
            dbgD3DError();
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Could not allocate internal texture memory!");
            NV_D3D_GLOBAL_SAVE();
            return (FALSE);
        }
#ifdef NV_TEX2
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface - Internal Texture = %08lx", TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags));
#else
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface - Internal Texture = %08lx", pTexture->dwTexturePointer);
#endif

        /*
         * Save frequently accessed globals.
         */
        NV_D3D_GLOBAL_SAVE();
    }

#ifdef HW_PAL8
    if (pTexture->dwTextureFlags & NV4_TEXFLAG_PALETTIZED)
    {
        /*
         * Setup frequently accessed globals.
         */
        NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
        nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_ROP, sizeSet2DSurfacesDestination);
#endif  // CACHE_FREECOUNT

        if (!pTexture->dwLinearAddr)
        {
            nvTextureAllocLinearSurface (pTexture,
                                         pTexture->dwPitch * pTexture->dwHeight,
                                         NV4_TEXLOC_SYS,
                                         NV4_TEXLOC_SYS);
        }

        /*
         * Save frequently accessed globals.
         */
        NV_D3D_GLOBAL_SAVE();
    }
#endif

#ifdef DIRECTSWIZZLE
    {
        /*
         * Setup frequently accessed globals.
         */
        NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
        nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_ROP, sizeSet2DSurfacesDestination);
#endif  // CACHE_FREECOUNT

        if (!pTexture->dwLinearAddr)
        {
            /*
             * create a linear texture for staging - if this succeeds we will use
             * HW swizzling in the future
             */
            DWORD dwAllocFlags = (pDriverData->GARTLinearBase) ? (NV4_TEXLOC_VID | NV4_TEXLOC_AGP)
                                                               : (NV4_TEXLOC_VID | NV4_TEXLOC_PCI);
            nvTextureAllocLinearSurface (pTexture,
                                         pTexture->dwPitch * pTexture->dwHeight,
                                         dwAllocFlags,
                                         dwAllocFlags & (NV4_TEXLOC_AGP | NV4_TEXLOC_PCI));
        }

        /*
         * Save frequently accessed globals.
         */
        NV_D3D_GLOBAL_SAVE();
    }
#endif //DIRECTSWIZZLE

#if 0
#ifdef NV_TEX2
    /*
     * if texture is dynamic, give it a linear copy - this way we do not have to wait for staging space
     */
    if ((lpCreateSurface->lpDDSurfaceDesc->dwSize >= sizeof(DDSURFACEDESC2))
     && (((DDSURFACEDESC2*)&lpCreateSurface->lpDDSurfaceDesc)->ddsCaps.dwCaps2 & DDSCAPS2_HINTDYNAMIC)) {
        /*
         * Setup frequently accessed globals.
         */
        NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
        nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_ROP, sizeSet2DSurfacesDestination);
#endif  // CACHE_FREECOUNT

        if (!pTexture->dwLinearAddr)
        {
            /*
             * create a linear texture for staging - if this succeeds we will use
             * HW swizzling in the future
             */
            DWORD dwAllocFlags = (pDriverData->GARTLinearBase) ? (NV4_TEXLOC_VID | NV4_TEXLOC_AGP)
                                                               : (NV4_TEXLOC_VID | NV4_TEXLOC_PCI);
            nvTextureAllocLinearSurface (pTexture,
                                         pTexture->dwPitch * pTexture->dwHeight,
                                         dwAllocFlags,
                                         dwAllocFlags & (NV4_TEXLOC_AGP | NV4_TEXLOC_PCI));
        }

        /*
         * Save frequently accessed globals.
         */
        NV_D3D_GLOBAL_SAVE();
    }
#endif  // NV_TEX2
#endif 0

#ifdef NV_TEX2
    /*
     * Set the fpVidMem pointer in the global surface structure to be the internal
     * texture memory pointer unless we already have a linear surface lying around
     */
#ifndef WINNT
    gblDDS->fpVidMem    = pTexture->dwLinearAddr ? pTexture->dwLinearAddr
                                                 : TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags);
#else
    gblDDS->fpVidMem    = pTexture->dwLinearAddr ? pTexture->dwLinearAddr
                                                 : TEX_SWIZZLE_OFFSET(pTexture,pTexture->dwTextureFlags);
#endif
#else
    /*
     * Set the fpVidMem pointer in the global surface structure to be the internal
     * texture memory pointer.  The only time that seperate user texture memory needs
     * to be allocated is in the event that the application is directly touching the
     * texture memory.  If all accesses to the texture are done using DDRAW, then there
     * is no need for a seperate copy of the texture in system memory since the texture
     * swizzling is now done at DDRAW Blit time.  If, however, the application locks the
     * texture surface, then seperate texture memory will need to be alloacted and the
     * the fpVidMem pointer will be switched at that point.
     */
    gblDDS->fpVidMem    = pTexture->dwTexturePointer;
#endif
    gblDDS->dwReserved1 = 0;

#ifndef NV_TEX2
    /*
     * Show that the texture surface has been allocated.
     */
    pTexture->dwTextureFlags |= NV3_TEXTURE_SURFACE_ALLOCATED;
#endif

    /*
     * If this is really an 8 bit palettized texture, then an extra system memory
     * texture surface needs to be allocated and some information in the surface structure
     * needs to be modified.
     */
    if ((dwSrcBitCount == 8) && (dwBytesPerPixel != 1))
    {
#ifdef NV_TEX2
        DWORD dwAllocFlags;
#endif
        NV_D3D_GLOBAL_SETUP();
        /*
         * Allocate the user system memory texture surface.
         */
        lPitch = texWidth;
        gblDDS->lPitch = lPitch;
        texSizeBytes = (DWORD)(lPitch * (LONG)texHeight);
#ifdef NV_TEX2
        dwAllocFlags = pDriverData->GARTLinearBase ? (NV4_TEXLOC_VID | NV4_TEXLOC_AGP | NV4_TEXLOC_SYS)
                                                   : (NV4_TEXLOC_VID | NV4_TEXLOC_PCI | NV4_TEXLOC_SYS);
        if (!nvTextureAllocLinearSurface(pTexture,
                                         texSizeBytes,
                                         dwAllocFlags,
                                         dwAllocFlags & (NV4_TEXLOC_AGP | NV4_TEXLOC_PCI)))
#else  // !NV_TEX2
        hMem = GlobalAlloc(GHND | GMEM_SHARE, texSizeBytes);
        if (!hMem)
#endif // !NV_TEX2
        {
            /*
             * Could not allocate the internal texture memory.  Cleanup and return the error.
             */
#ifndef NV_TEX2
            pTexture->hUserTexture  = (HANDLE)NULL;
            pTexture->fpUserTexture = (FLATPTR)NULL;
#endif
            dbgD3DError();
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - User texture memory allocation failed");
        }
        else
        {
            /*
             * Store the handle and pointer for the system memory surface in the global surface
             * data structure.
             */
#ifdef NV_TEX2
            DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface - User Texture = %08lx", pTexture->dwLinearAddr);
#else
            gblDDS->fpVidMem          = (FLATPTR)GlobalLock(hMem);
            gblDDS->dwReserved1       = (DWORD)hMem;
            pTexture->hUserTexture    = hMem;
            pTexture->fpUserTexture   = gblDDS->fpVidMem;
            pTexture->dwTextureFlags |= NV3_TEXTURE_USER_ALLOCATED;
            DPF_LEVEL(NVDBG_LEVEL_INFO, "nvCreateTextureSurface - User Texture = %08lx", pTexture->fpUserTexture);
#endif //!NV_TEX2
        }
        NV_D3D_GLOBAL_SAVE();
    }

    /*
     * Texture surface allocated successfully.
     */
    return (TRUE);
}

BOOL nvDestroyTextureSurface
(
    LPDDHAL_DESTROYSURFACEDATA  lpDestroySurface,
    LPDDRAWI_DDRAWSURFACE_LCL   lclDDS,
    LPDDRAWI_DDRAWSURFACE_GBL   gblDDS
)
{
    HGLOBAL         hMem;
    HDDRVITEM       hTexture;
    PNVD3DCONTEXT   pContext = 0;
    PNVD3DTEXTURE   pTexture = 0;

    DPF_LEVEL(NVDBG_LEVEL_FUNCTION_CALL, "nvDestroyTextureSurface - lclDDS = %08lx, gblDDS = %08lx", lclDDS, gblDDS);

    /*
     * Get the handle of the system memory texture surface.
     */
    hMem = (HGLOBAL)gblDDS->dwReserved1;

    /*
     * Get pointer to the texture being destroyed.
     */
    hTexture = lclDDS->dwReserved1;
    pTexture = (PNVD3DTEXTURE)hTexture;
    if (pTexture)
    {
        /*
         * Get pointer to the context.
         */
        pContext = (PNVD3DCONTEXT)pTexture->hContext;
        if (pContext)
        {
            /*
             * Make sure the context does not refer to the texture being destroyed.
             */
            if (pContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE] == hTexture)
                pContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE] = 0;
        }

#ifndef NV_TEX2
        /*
         * Mark the texture surface as de-allocated.
         */
        pTexture->dwTextureFlags &= ~NV3_TEXTURE_SURFACE_ALLOCATED;
#endif

        /*
         * Free any memory that is currently allocated.
         */
#ifdef NV_TEX2
        if (pTexture->dwLinearAddr)
        {
            DPF_LEVEL(NVDBG_LEVEL_INFO, "nvDestroyTextureSurface - Free User Texture = %08lx", pTexture->dwLinearAddr);
            NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
            nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
            NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_ROP, sizeSet2DSurfacesDestination);
#endif  // CACHE_FREECOUNT
            nvTextureFreeLinearSurface (pTexture);
            NV_D3D_GLOBAL_SAVE();
        }
        if (TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags))
        {
            DPF_LEVEL(NVDBG_LEVEL_INFO, "nvDestroyTextureSurface - Free Internal Texture = %08lx", TEX_SWIZZLE_ADDR(pTexture,pTexture->dwTextureFlags));
            NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
            nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
            NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_ROP, sizeSet2DSurfacesDestination);
#endif  // CACHE_FREECOUNT
            nvTextureFreeSwizzleSurface (pTexture);
            NV_D3D_GLOBAL_SAVE();
        }
#else // !NV_TEX2
        /* Under WINNT, defer destruction of user allocated texture surface
         * until the texture handle is destroyed. We can't free the user
         * address here since we may not be running in the context of the
         * process which created the texture.
         */
#ifndef WINNT
        if (pTexture->dwTextureFlags & NV3_TEXTURE_USER_ALLOCATED)
        {
            DPF_LEVEL(NVDBG_LEVEL_INFO, "nvDestroyTextureSurface - Free User Texture = %08lx", pTexture->fpUserTexture);
            pTexture->dwTextureFlags &= ~NV3_TEXTURE_USER_ALLOCATED;
            GlobalUnlock(hMem);
            GlobalFree(hMem);
        }
#endif // #ifndef WINNT
        if (pTexture->dwTextureFlags & NV3_TEXTURE_INTERNAL_ALLOCATED)
        {
            DPF_LEVEL(NVDBG_LEVEL_INFO, "nvDestroyTextureSurface - Free Internal Texture = %08lx", pTexture->dwTexturePointer);
            NV_D3D_GLOBAL_SETUP();
            nvDestroyInternalTexture(pContext, pTexture);
            NV_D3D_GLOBAL_SAVE();
       }
#endif // !NV_TEX2

        /*
         * Do a little cleanup.
         */
        gblDDS->fpVidMem           = 0;
        gblDDS->dwReserved1        = 0;
#ifndef NV_TEX2
        pTexture->hUserTexture     = 0;
        pTexture->fpUserTexture    = 0;
        pTexture->dwTexturePointer = 0;
        pTexture->dwTextureOffset  = 0;
#endif
        pTexture->lpLcl            = 0;

        /*
         * Only destroy the internal texture data structure if the texture handle has already
         * been de-allocated.
         */
#ifdef NV_TEX2
        if (!(pTexture->dwTextureFlags & NV4_TEXFLAG_HANDLE_ALLOCATED))
#else
        if (!(pTexture->dwTextureFlags & NV3_TEXTURE_HANDLE_ALLOCATED))
#endif
        {
            /*
             * Setup frequently accessed globals.
             */
            NV_D3D_GLOBAL_SETUP();

            /*
             * Free the memory allocated for the internal texture data structure.
             */
            DPF_LEVEL(NVDBG_LEVEL_INFO, "nvDestroyTextureSurface - Free Texture Structure = %08lx", pTexture);
            nvDestroyTextureStructure(pTexture);
            lclDDS->dwReserved1 = (DWORD)NULL;
            NV_D3D_GLOBAL_SAVE();
        }
    }
    else
    {
        /*
         * Free the system memory texture surface.
         */
        if (hMem)
        {
            GlobalUnlock(hMem);
            GlobalFree(hMem);
        }
        gblDDS->fpVidMem    = 0;
        gblDDS->dwReserved1 = 0;
    }

    /*
     * Return successful return codes.
     */
    lpDestroySurface->ddRVal = DD_OK;
    return (TRUE);
}

#ifndef NV_TEX2

void nvSwizzleBlit16Bpp
(
    DWORD   dwSrcX0,
    DWORD   dwSrcY0,
    DWORD   dwSrcX1,
    DWORD   dwSrcY1,
    DWORD   dwWidth,
    DWORD   dwHeight,
    DWORD   dwSrcPitch,
    DWORD   dwSrcLinearBase,
    DWORD   dwDstX0,
    DWORD   dwDstY0,
    DWORD   dwDstPitch,
    DWORD   dwDstLinearBase,
    DWORD   dwDstSwizzleBase,
    DWORD   dwTextureFormat
)
{
    WORD    wSrcTexel;
    DWORD   u0, u, v;
    DWORD   uInc, uInc2, vInc, uMask, vMask;
    DWORD   xSrcCurrent, ySrcCurrent;
    DWORD   xDstCurrent, yDstCurrent;
    DWORD   dwSrcTexel;
    unsigned short *pSrcLinear;
    unsigned short *pDstLinear;
    unsigned short *pDstSwizzle;

    U_INTERLEAVE(u0, dwDstX0, dwWidth, dwHeight);
    V_INTERLEAVE(v,  dwDstY0, dwWidth, dwHeight);
    U_INC2_MASK(uInc, uInc2, uMask, dwHeight);
    V_INC_MASK(vInc, vMask, dwWidth);
    pDstSwizzle = (unsigned short *)dwDstSwizzleBase;
    // X1 is exclusive, so if X1 is even, then the real last texel is on an odd boundry.
    switch (((dwSrcX0 & 0x01) << 1) | (dwSrcX1 & 0x01))
    {
        /*
         * Even start, even end.
         */
        case 0:
            // X1, Y1 are exclusive.
            for (ySrcCurrent = dwSrcY0, yDstCurrent = dwDstY0;
                 ySrcCurrent < dwSrcY1;
                 ySrcCurrent++, yDstCurrent++, v = UV_INC(v, vInc, vMask))
            {
                pSrcLinear = (unsigned short *)(dwSrcLinearBase + (ySrcCurrent * dwSrcPitch));
                pDstLinear = (unsigned short *)(dwDstLinearBase + (yDstCurrent * dwDstPitch));
                xSrcCurrent = dwSrcX0;
                xDstCurrent = dwDstX0;
                u = u0;
                while (xSrcCurrent < dwSrcX1)
                {
                    dwSrcTexel = *(DWORD *)(&pSrcLinear[xSrcCurrent]);
                    *(DWORD *)(&pDstSwizzle[UV_OFFSET(u,v)]) = dwSrcTexel;
                    *(DWORD *)(&pDstLinear[xDstCurrent])     = dwSrcTexel;
                    xSrcCurrent += 2;
                    xDstCurrent += 2;
                    u = UV_INC(u, uInc2, uMask);
                }
            }
            break;

        /*
         * Even start, odd end.
         */
        case 1:
            // X1, Y1 are exclusive.
            for (ySrcCurrent = dwSrcY0, yDstCurrent = dwDstY0;
                 ySrcCurrent < dwSrcY1;
                 ySrcCurrent++, yDstCurrent++, v = UV_INC(v, vInc, vMask))
            {
                pSrcLinear = (unsigned short *)(dwSrcLinearBase + (ySrcCurrent * dwSrcPitch));
                pDstLinear = (unsigned short *)(dwDstLinearBase + (yDstCurrent * dwDstPitch));
                xSrcCurrent = dwSrcX0;
                xDstCurrent = dwDstX0;
                u = u0;
                // X1 is exclusive, so need to bail out of inner loop before doing last texel.
                while (xSrcCurrent < (dwSrcX1 - 1))
                {
                    dwSrcTexel = *(DWORD *)(&pSrcLinear[xSrcCurrent]);
                    *(DWORD *)(&pDstSwizzle[UV_OFFSET(u,v)]) = dwSrcTexel;
                    *(DWORD *)(&pDstLinear[xDstCurrent])     = dwSrcTexel;
                    xSrcCurrent += 2;
                    xDstCurrent += 2;
                    u = UV_INC(u, uInc2, uMask);
                }
                wSrcTexel = pSrcLinear[xSrcCurrent];
                pDstSwizzle[UV_OFFSET(u,v)] = wSrcTexel;
                pDstLinear[xDstCurrent]     = wSrcTexel;
            }
            break;

        /*
         * Odd start, even end.
         */
        case 2:
            // X1, Y1 are exclusive.
            for (ySrcCurrent = dwSrcY0, yDstCurrent = dwDstY0;
                 ySrcCurrent < dwSrcY1;
                 ySrcCurrent++, yDstCurrent++, v = UV_INC(v, vInc, vMask))
            {
                pSrcLinear = (unsigned short *)(dwSrcLinearBase + (ySrcCurrent * dwSrcPitch));
                pDstLinear = (unsigned short *)(dwDstLinearBase + (yDstCurrent * dwDstPitch));
                xSrcCurrent = dwSrcX0;
                xDstCurrent = dwDstX0;
                u = u0;
                wSrcTexel = pSrcLinear[xSrcCurrent];
                pDstSwizzle[UV_OFFSET(u,v)] = wSrcTexel;
                pDstLinear[xDstCurrent]     = wSrcTexel;
                xSrcCurrent++;
                xDstCurrent++;
                u = UV_INC(u, uInc, uMask);
                while (xSrcCurrent < dwSrcX1)
                {
                    dwSrcTexel = *(DWORD *)(&pSrcLinear[xSrcCurrent]);
                    *(DWORD *)(&pDstSwizzle[UV_OFFSET(u,v)]) = dwSrcTexel;
                    *(DWORD *)(&pDstLinear[xDstCurrent])     = dwSrcTexel;
                    xSrcCurrent += 2;
                    xDstCurrent += 2;
                    u = UV_INC(u, uInc2, uMask);
                }
            }
            break;

        /*
         * Odd start, odd end.
         */
        case 3:
            // X1, Y1 are exclusive.
            for (ySrcCurrent = dwSrcY0, yDstCurrent = dwDstY0;
                 ySrcCurrent < dwSrcY1;
                 ySrcCurrent++, yDstCurrent++, v = UV_INC(v, vInc, vMask))
            {
                pSrcLinear = (unsigned short *)(dwSrcLinearBase + (ySrcCurrent * dwSrcPitch));
                pDstLinear = (unsigned short *)(dwDstLinearBase + (yDstCurrent * dwDstPitch));
                xSrcCurrent = dwSrcX0;
                xDstCurrent = dwDstX0;
                u = u0;
                wSrcTexel = pSrcLinear[xSrcCurrent];
                pDstSwizzle[UV_OFFSET(u,v)] = wSrcTexel;
                pDstLinear[xDstCurrent]     = wSrcTexel;
                xSrcCurrent++;
                xDstCurrent++;
                u = UV_INC(u, uInc, uMask);
                // X1 is exclusive, so need to bail out of inner loop before doing last texel.
                while (xSrcCurrent < (dwSrcX1 - 1))
                {
                    dwSrcTexel = *(DWORD *)(&pSrcLinear[xSrcCurrent]);
                    *(DWORD *)(&pDstSwizzle[UV_OFFSET(u,v)]) = dwSrcTexel;
                    *(DWORD *)(&pDstLinear[xDstCurrent])     = dwSrcTexel;
                    xSrcCurrent += 2;
                    xDstCurrent += 2;
                    u = UV_INC(u, uInc2, uMask);
                }
                wSrcTexel = pSrcLinear[xSrcCurrent];
                pDstSwizzle[UV_OFFSET(u,v)] = wSrcTexel;
                pDstLinear[xDstCurrent]     = wSrcTexel;
            }
            break;
    }
}

void nvSwizzleBlit32Bpp
(
    DWORD   dwSrcX0,
    DWORD   dwSrcY0,
    DWORD   dwSrcX1,
    DWORD   dwSrcY1,
    DWORD   dwWidth,
    DWORD   dwHeight,
    DWORD   dwSrcPitch,
    DWORD   dwSrcLinearBase,
    DWORD   dwDstX0,
    DWORD   dwDstY0,
    DWORD   dwDstPitch,
    DWORD   dwDstLinearBase,
    DWORD   dwDstSwizzleBase,
    DWORD   dwTextureFormat
)
{
    DWORD   u0, u, v;
    DWORD   uInc, uInc2, vInc, uMask, vMask;
    DWORD   xSrcCurrent, ySrcCurrent;
    DWORD   xDstCurrent, yDstCurrent;
    DWORD   dwSrcTexel;
    unsigned long *pSrcLinear;
    unsigned long *pDstLinear;
    unsigned long *pDstSwizzle;

    U_INTERLEAVE(u0, dwDstX0, dwWidth, dwHeight);
    V_INTERLEAVE(v,  dwDstY0, dwWidth, dwHeight);
    U_INC2_MASK(uInc, uInc2, uMask, dwHeight);
    V_INC_MASK(vInc, vMask, dwWidth);
    pDstSwizzle = (unsigned long *)dwDstSwizzleBase;
    for (ySrcCurrent = dwSrcY0, yDstCurrent = dwDstY0;
         ySrcCurrent < dwSrcY1;
         ySrcCurrent++, yDstCurrent++, v = UV_INC(v, vInc, vMask))
    {
        pSrcLinear = (unsigned long *)(dwSrcLinearBase + (ySrcCurrent * dwSrcPitch));
        pDstLinear = (unsigned long *)(dwDstLinearBase + (yDstCurrent * dwDstPitch));
        xSrcCurrent = dwSrcX0;
        xDstCurrent = dwDstX0;
        u = u0;
        while (xSrcCurrent < dwSrcX1)
        {
            dwSrcTexel = pSrcLinear[xSrcCurrent];
            pDstSwizzle[UV_OFFSET(u,v)]   = dwSrcTexel;
            pDstLinear[xDstCurrent]       = dwSrcTexel;
            dwSrcTexel = pSrcLinear[xSrcCurrent+1];
            pDstSwizzle[UV_OFFSET(u,v)+1] = dwSrcTexel;
            pDstLinear[xDstCurrent+1]     = dwSrcTexel;
            xSrcCurrent += 2;
            xDstCurrent += 2;
            u = UV_INC(u, uInc2, uMask);
        }
    }
}

void nvTextureBlit8Bpp
(
    LPDDHAL_BLTDATA pbd
)
{
    unsigned long               dwSrcLinearBase, dwDstLinearBase;
    unsigned char              *pSrcLinear;
    unsigned char              *pDstLinear;
    long                        xWidth, yHeight;
    PNVD3DTEXTURE               pBaseTexture = 0;
    PNVD3DTEXTURE               pTexture = 0;
    BOOL                        bLuminanceTexture = FALSE;
    DWORD                       dwSrcX0, dwSrcY0;
    DWORD                       dwSrcX1, dwSrcY1;
    DWORD                       dwSrcPitch;
    DWORD                       dwSrcWidth;
    DWORD                       dwSrcHeight;
    DWORD                       dwSrcLimit;
    DWORD                       dwDstX0, dwDstY0;
    DWORD                       dwDstX1, dwDstY1;
    DWORD                       dwDstPitch;
    DWORD                       dwDstWidth;
    DWORD                       dwDstHeight;
    DWORD                       dwDstLimit;
    DWORD                       dwTexturePointer = 0;
    DWORD                       dwTextureFormat  = 0;
    DWORD                       dwSrcBitCount;
    DWORD                       dwDstBitCount;
    DWORD                       xSrc, ySrc, xDst, yDst;
    DWORD                       dwColor;
    DWORD                       u, v;
    LPPALETTEENTRY              pPalette;
    BYTE                        bIndex;
    LPDDRAWI_DDRAWSURFACE_LCL   lclSrcDDS;
    LPDDRAWI_DDRAWSURFACE_LCL   lclDstDDS;
    LPDDRAWI_DDRAWSURFACE_GBL   gblSrcDDS;
    LPDDRAWI_DDRAWSURFACE_GBL   gblDstDDS;

    /*
     * Get source surface information.
     */
    lclSrcDDS       = pbd->lpDDSrcSurface;
    gblSrcDDS       = lclSrcDDS->lpGbl;
    dwSrcPitch      = gblSrcDDS->lPitch;
    dwSrcLinearBase = (unsigned long)gblSrcDDS->fpVidMem;
    dwSrcLimit      = (dwSrcPitch * (DWORD)gblSrcDDS->wHeight) - 1;
    if (gblSrcDDS->ddpfSurface.dwFlags & DDPF_RGB)
        dwSrcBitCount = gblSrcDDS->ddpfSurface.dwRGBBitCount;
    else if (gblSrcDDS->ddpfSurface.dwFlags & DDPF_LUMINANCE)
    {
        dwSrcBitCount = gblSrcDDS->ddpfSurface.dwLuminanceBitCount;
        bLuminanceTexture = TRUE;
    }
    else
        dwSrcBitCount = pDriverData->bi.biBitCount;

    /*
     * Get destination surface information.
     */
    lclDstDDS       = pbd->lpDDDestSurface;
    gblDstDDS       = lclDstDDS->lpGbl;
    dwDstPitch      = gblDstDDS->lPitch;
    dwDstLinearBase = (unsigned long)gblDstDDS->fpVidMem;
    dwDstLimit      = (dwDstPitch * (DWORD)gblDstDDS->wHeight) - 1;
    if (gblDstDDS->ddpfSurface.dwFlags & DDPF_RGB)
        dwDstBitCount = gblDstDDS->ddpfSurface.dwRGBBitCount;
    else if (gblDstDDS->ddpfSurface.dwFlags & DDPF_LUMINANCE)
    {
        dwSrcBitCount = gblDstDDS->ddpfSurface.dwLuminanceBitCount;
        bLuminanceTexture = TRUE;
    }
    else
        dwDstBitCount = pDriverData->bi.biBitCount;

    if ((dwSrcBitCount != dwDstBitCount)
     || (dwSrcBitCount != 8))
    {
        /*
         * Not supporting bit depth conversion blits.
         */
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureBlit8Bpp - Source and destination have different bit depths!!");
        dbgD3DError();
        return;
    }

    /*
     * Get the pointer to the internal texture structure.
     */
    pTexture = (PNVD3DTEXTURE)lclDstDDS->dwReserved1;

    /*
     * Get the pointer to the internal texture surface.
     */
    if (pTexture)
    {
        if (pTexture->dwTextureFlags & NV3_TEXTURE_USER_MIP_LEVEL)
            dwTexturePointer = pTexture->dwTexturePointer;
        else
        {
            if (pTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
#ifndef WINNT
                dwTexturePointer = pDriverData->BaseAddress + pTexture->dwTextureOffset;
#else
                dwTexturePointer = pDriverData->ppdev->pjScreen + pTexture->dwTextureOffset;
#endif // #ifdef WINNT
            else
            {
                if (pDriverData->GARTLinearBase > 0)
                    dwTexturePointer = pTexture->dwTexturePointer;
                else
                    dwTexturePointer = GetPointerTextureHeap(pTexture->dwTextureOffset);
            }
        }
    }

    /*
     * Get the blit postion.
     */
    dwSrcX0 = pbd->rSrc.left;
    dwSrcX1 = pbd->rSrc.right;
    dwSrcY0 = pbd->rSrc.top;
    dwSrcY1 = pbd->rSrc.bottom;
    dwDstX0 = pbd->rDest.left;
    dwDstY0 = pbd->rDest.top;
    dwDstX1 = pbd->rDest.right;
    dwDstY1 = pbd->rDest.bottom;

    /*
     * Calculate the width and height of the blit.
     */
    dwSrcWidth  = dwSrcX1 - dwSrcX0;
    dwSrcHeight = dwSrcY1 - dwSrcY0;
    dwDstWidth  = dwDstX1 - dwDstX0;
    dwDstHeight = dwDstY1 - dwDstY1;
    xWidth      = (dwDstWidth  <= dwSrcWidth)  ? dwDstWidth  : dwSrcWidth;
    yHeight     = (dwDstHeight <= dwSrcHeight) ? dwDstHeight : dwSrcHeight;

    if (bLuminanceTexture)
    {
        pPalette = 0;
    }
    else
    {
#ifndef WINNT // BUGBUG - Under WINNT, where's the palette?
        if ((!lclDstDDS->lpDDPalette)
         || (!lclDstDDS->lpDDPalette->lpLcl)
         || (!lclDstDDS->lpDDPalette->lpLcl->lpGbl)
         || (!lclDstDDS->lpDDPalette->lpLcl->lpGbl->lpColorTable))
        {
            if (pTexture)
                pTexture->dwTextureFlags |= NV3_TEXTURE_MODIFIED;
            pPalette = 0;
        }
        else
            pPalette = lclDstDDS->lpDDPalette->lpLcl->lpGbl->lpColorTable;
#endif // #ifndef WINNT
    }

    /*
     * Display the source texture on the screen. (DEBUG only)
     */
    dbgDisplayLinearTexture(lclSrcDDS, lclDstDDS, pTexture->dwTextureColorFormat);

    if (bLuminanceTexture)
    {
        unsigned char   *pDstSwizzle;
        unsigned char   bColor;

        /*
         * Straight 8bit swizzle.
         */
        pDstSwizzle = (unsigned char *)dwTexturePointer;
        for (ySrc = dwSrcY0, yDst = dwDstY0; ySrc < dwSrcY1; ySrc++, yDst++)
        {
            pSrcLinear = (char *)(dwSrcLinearBase + (ySrc * dwSrcPitch));
            V_INTERLEAVE(v,  yDst, gblDstDDS->wWidth, gblDstDDS->wHeight);
            for (xSrc = dwSrcX0, xDst = dwDstX0; xSrc < dwSrcX1; xSrc++, xDst++)
            {
                bColor = pSrcLinear[xSrc];
                U_INTERLEAVE(u,  xDst, gblDstDDS->wWidth, gblDstDDS->wHeight);
                pDstSwizzle[UV_OFFSET(u,v)] = bColor;
            }
        }
    }
    else if (pDriverData->regPal8TextureConvert == REG_P8TC_8TOR5G6B5)
    {
        unsigned short             *pDstSwizzle;
        WORD                        wColor;
        pDstSwizzle = (unsigned short *)dwTexturePointer;
        for (ySrc = dwSrcY0, yDst = dwDstY0; ySrc < dwSrcY1; ySrc++, yDst++)
        {
            pSrcLinear = (char *)(dwSrcLinearBase + (ySrc * dwSrcPitch));
            pDstLinear = (char *)(dwDstLinearBase + (yDst * dwDstPitch));
            V_INTERLEAVE(v,  yDst, gblDstDDS->wWidth, gblDstDDS->wHeight);
            for (xSrc = dwSrcX0, xDst = dwDstX0; xSrc < dwSrcX1; xSrc++, xDst++)
            {
                bIndex = pSrcLinear[xSrc];
                pDstLinear[xDst] = bIndex;
                if (pPalette)
                {
                    wColor = ((WORD)(pPalette[bIndex].peRed & 0xF8) << 8)
                           | ((WORD)(pPalette[bIndex].peGreen & 0xFC) << 3)
                           | ((WORD)(pPalette[bIndex].peBlue & 0xF8) >> 3);
                    U_INTERLEAVE(u,  xDst, gblDstDDS->wWidth, gblDstDDS->wHeight);
                    pDstSwizzle[UV_OFFSET(u,v)] = wColor;
                }
            }
        }
    }
    else if ((pDriverData->regPal8TextureConvert == REG_P8TC_8TOX1R5G5B5)
          || (pDriverData->regPal8TextureConvert == REG_P8TC_8TOA1R5G5B5))
    {
        unsigned short             *pDstSwizzle;
        WORD                        wColor;
        pDstSwizzle = (unsigned short *)dwTexturePointer;
        for (ySrc = dwSrcY0, yDst = dwDstY0; ySrc < dwSrcY1; ySrc++, yDst++)
        {
            pSrcLinear = (char *)(dwSrcLinearBase + (ySrc * dwSrcPitch));
            pDstLinear = (char *)(dwDstLinearBase + (yDst * dwDstPitch));
            V_INTERLEAVE(v,  yDst, gblDstDDS->wWidth, gblDstDDS->wHeight);
            for (xSrc = dwSrcX0, xDst = dwDstX0; xSrc < dwSrcX1; xSrc++, xDst++)
            {
                bIndex = pSrcLinear[xSrc];
                pDstLinear[xDst] = bIndex;
                if (pPalette)
                {
                    wColor = ((WORD)(pPalette[bIndex].peRed & 0xF8) << 7)
                           | ((WORD)(pPalette[bIndex].peGreen & 0xF8) << 2)
                           | ((WORD)(pPalette[bIndex].peBlue & 0xF8) >> 3)
                           | 0x8000;
                    U_INTERLEAVE(u,  xDst, gblDstDDS->wWidth, gblDstDDS->wHeight);
                    pDstSwizzle[UV_OFFSET(u,v)] = wColor;
                }
            }
        }
    }
    else if ((pDriverData->regPal8TextureConvert == REG_P8TC_8TOX8R8G8B8)
          || (pDriverData->regPal8TextureConvert == REG_P8TC_8TOX8R8G8B8))
    {
        unsigned long              *pDstSwizzle;
        pDstSwizzle = (unsigned long *)dwTexturePointer;
        for (ySrc = dwSrcY0, yDst = dwDstY0; ySrc < dwSrcY1; ySrc++, yDst++)
        {
            pSrcLinear = (char *)(dwSrcLinearBase + (ySrc * dwSrcPitch));
            pDstLinear = (char *)(dwDstLinearBase + (yDst * dwDstPitch));
            V_INTERLEAVE(v,  yDst, gblDstDDS->wWidth, gblDstDDS->wHeight);
            for (xSrc = dwSrcX0, xDst = dwDstX0; xSrc < dwSrcX1; xSrc++, xDst++)
            {
                bIndex = pSrcLinear[xSrc];
                pDstLinear[xDst] = bIndex;
                if (pPalette)
                {
                    dwColor = ((DWORD)(pPalette[bIndex].peRed) << 16)
                            | ((DWORD)(pPalette[bIndex].peGreen) << 8)
                            | (DWORD)(pPalette[bIndex].peBlue)
                            | 0xFF000000;
                    U_INTERLEAVE(u,  xDst, gblDstDDS->wWidth, gblDstDDS->wHeight);
                    pDstSwizzle[UV_OFFSET(u,v)] = dwColor;
                }
            }
        }
    }
    dbgDisplayTexture((PNVD3DCONTEXT)pTexture->hContext, pTexture);
}

void nvTextureBlit16Bpp
(
    LPDDHAL_BLTDATA pbd
)
{
    unsigned long               dwSrcLinearBase, dwDstLinearBase;
    unsigned short             *pSrcLinear;
    unsigned short             *pDstLinear;
    long                        xCount, yCount;
    long                        xWidth, yHeight;
    PNVD3DTEXTURE               pBaseTexture = 0;
    PNVD3DTEXTURE               pTexture = 0;
    DWORD                       ySrcCurrent;
    DWORD                       yDstCurrent;
    DWORD                       dwSrcX0, dwSrcY0;
    DWORD                       dwSrcX1, dwSrcY1;
    DWORD                       dwSrcPitch;
    DWORD                       dwSrcWidth;
    DWORD                       dwSrcHeight;
    DWORD                       dwSrcLimit;
    DWORD                       dwDstX0, dwDstY0;
    DWORD                       dwDstX1, dwDstY1;
    DWORD                       dwDstPitch;
    DWORD                       dwDstWidth;
    DWORD                       dwDstHeight;
    DWORD                       dwDstLimit;
    DWORD                       dwTexturePointer = 0;
    DWORD                       dwTextureFormat  = 0;
    DWORD                       dwSrcBitCount;
    DWORD                       dwDstBitCount;
    LPDDRAWI_DDRAWSURFACE_LCL   lclSrcDDS;
    LPDDRAWI_DDRAWSURFACE_LCL   lclDstDDS;
    LPDDRAWI_DDRAWSURFACE_GBL   gblSrcDDS;
    LPDDRAWI_DDRAWSURFACE_GBL   gblDstDDS;

    /*
     * Get source surface information.
     */
    lclSrcDDS       = pbd->lpDDSrcSurface;
    gblSrcDDS       = lclSrcDDS->lpGbl;
    dwSrcPitch      = gblSrcDDS->lPitch;
    dwSrcLinearBase = (unsigned long)gblSrcDDS->fpVidMem;
    dwSrcLimit      = (dwSrcPitch * (DWORD)gblSrcDDS->wHeight) - 1;
    if (gblSrcDDS->ddpfSurface.dwFlags & DDPF_RGB)
        dwSrcBitCount = gblSrcDDS->ddpfSurface.dwRGBBitCount;
    else
        dwSrcBitCount = pDriverData->bi.biBitCount;

    /*
     * Get destination surface information.
     */
    lclDstDDS       = pbd->lpDDDestSurface;
    gblDstDDS       = lclDstDDS->lpGbl;
    dwDstPitch      = gblDstDDS->lPitch;
    dwDstLinearBase = (unsigned long)gblDstDDS->fpVidMem;
    dwDstLimit      = (dwDstPitch * (DWORD)gblDstDDS->wHeight) - 1;
    if (gblDstDDS->ddpfSurface.dwFlags & DDPF_RGB)
        dwDstBitCount = gblDstDDS->ddpfSurface.dwRGBBitCount;
    else
        dwDstBitCount = pDriverData->bi.biBitCount;

    if (dwSrcBitCount != dwSrcBitCount)
    {
        /*
         * Not supporting bit depth conversion blits.
         */
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureBlit16Bpp - Source and destination have different bit depths!!");
        dbgD3DError();
        return;
    }
    else if (dwSrcBitCount == 32)
    {
        nvTextureBlit32Bpp(pbd);
        return;
    }
    else if (dwSrcBitCount == 8)
    {
        nvTextureBlit8Bpp(pbd);
        return;
    }

    /*
     * Check for a non-square texture.
     */
    if (gblDstDDS->wWidth != gblDstDDS->wHeight)
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvTextureBlit16bpp - Non-Square Texture!!");

    /*
     * Get the pointer to the internal texture structure.
     */
    pTexture = (PNVD3DTEXTURE)lclDstDDS->dwReserved1;

    /*
     * Get the pointer to the internal texture surface.
     */
    if (pTexture)
    {
        if (pTexture->dwTextureFlags & NV3_TEXTURE_USER_MIP_LEVEL)
            dwTexturePointer = pTexture->dwTexturePointer;
        else
        {
            if (pTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
#ifndef WINNT
                dwTexturePointer = pDriverData->BaseAddress + pTexture->dwTextureOffset;
#else
                dwTexturePointer = pDriverData->ppdev->pjScreen + pTexture->dwTextureOffset;
#endif // #ifdef WINNT

            else
            {
                if (pDriverData->GARTLinearBase > 0)
                    dwTexturePointer = pTexture->dwTexturePointer;
                else
                    dwTexturePointer = GetPointerTextureHeap(pTexture->dwTextureOffset);
            }
        }
    }

    /*
     * Get the blit postion.
     */
    dwSrcX0 = pbd->rSrc.left;
    dwSrcX1 = pbd->rSrc.right;
    dwSrcY0 = pbd->rSrc.top;
    dwSrcY1 = pbd->rSrc.bottom;
    dwDstX0 = pbd->rDest.left;
    dwDstY0 = pbd->rDest.top;
    dwDstX1 = pbd->rDest.right;
    dwDstY1 = pbd->rDest.bottom;

    /*
     * Calculate the width and height of the blit.
     */
    dwSrcWidth  = dwSrcX1 - dwSrcX0;
    dwSrcHeight = dwSrcY1 - dwSrcY0;
    dwDstWidth  = dwDstX1 - dwDstX0;
    dwDstHeight = dwDstY1 - dwDstY1;
    xWidth      = (dwDstWidth  <= dwSrcWidth)  ? dwDstWidth  : dwSrcWidth;
    yHeight     = (dwDstHeight <= dwSrcHeight) ? dwDstHeight : dwSrcHeight;

    /*
     * Display the source texture on the screen. (DEBUG only)
     */
    dbgDisplayLinearTexture(lclSrcDDS, lclDstDDS, pTexture->dwTextureColorFormat);

    /*
     * If there's a valid pointer to an internal texture surface, then a dual
     * internal/user texture blit can be performed.
     */
    if (dwTexturePointer)
    {
        /*
         * Setup frequently accessed globals.
         */
        NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
        nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetObject);
#endif  // CACHE_FREECOUNT

        dwTextureFormat = pTexture->dwTextureColorFormat;

        /*
         * Decide what kind of texture blit to do.
         */
        if (pTexture->dwTextureFlags & NV3_TEXTURE_USER_ALLOCATED)
        {
            /*
             * The application has locked the surface so there are two
             * copies of the texture to update.
             * If the swizzled texture is in video memory:
             * First do a fast linear copy from the source surface to the
             * linear user surface, then use the hardware to swizzle the
             * texture.
             * If the swizzled texture is in non-local video memory:
             * Do the swizzle and the linear blit at the same time.
             *
             * Hardware Restrictions require the destination texture offset to be 32 byte aligned
             * and the source width to be 2 or more.  If this isn't the case must use software to
             * do the swizzling.
             * The only time the offset should be misaligned at this point is for mipmap levels of
             * 2x2 and 1x1.
             */
            if ((pTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
             && (pDriverData->NvDevFlatDma)
             && ((pTexture->dwTextureOffset & 0x0000001F) == 0)
             && (dwSrcWidth > 1))
            {
                /*
                 * Now use hardware to do the swizzle from user texture memory.
                 */
                nvTMVideoTextureBlt (dwSrcLinearBase,dwSrcPitch,dwSrcX0,dwSrcY0,dwSrcX1,dwSrcY1,
                                     dwDstLinearBase,dwDstPitch,dwDstX0,dwDstY0,
                                     pTexture->dwTextureOffset,
                                     (WORD)pTexture->dwMipMapBaseU, (WORD)pTexture->dwMipMapBaseV,
                                     dwTextureFormat);
            }
            else
            {
                nvSwizzleBlit16Bpp((DWORD)dwSrcX0,
                                   (DWORD)dwSrcY0,
                                   (DWORD)dwSrcX1,
                                   (DWORD)dwSrcY1,
                                   (DWORD)gblDstDDS->wWidth,
                                   (DWORD)gblDstDDS->wHeight,
                                   (DWORD)dwSrcPitch,
                                   (DWORD)dwSrcLinearBase,
                                   (DWORD)dwDstX0,
                                   (DWORD)dwDstY0,
                                   (DWORD)dwDstPitch,
                                   (DWORD)dwDstLinearBase,
                                   (DWORD)dwTexturePointer,
                                   (DWORD)dwTextureFormat);
            }
        }
        else
        {
            /*
             * Hardware Restrictions require the destination texture offset to be 32 byte aligned
             * and the source width to be 2 or more.  If this isn't the case must use software to
             * do the swizzling.
             * The only time the offset should be misaligned at this point is for mipmap levels of
             * 2x2 and 1x1.
             */
            if ((pTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
             && (pDriverData->NvDevFlatDma)
             && ((pTexture->dwTextureOffset & 0x0000001F) == 0)
             && (dwSrcWidth > 1))
            {
                nvTMVideoTextureBlt (dwSrcLinearBase,dwSrcPitch,dwSrcX0,dwSrcY0,dwSrcX1,dwSrcY1,
                                     0,dwDstPitch,dwDstX0,dwDstY0,
                                     pTexture->dwTextureOffset,
                                     (WORD)pTexture->dwMipMapBaseU, (WORD)pTexture->dwMipMapBaseV,
                                     dwTextureFormat);
            }
            else
            {
                /*
                 * This is a good application.  It's not locking and touching the texture directly
                 * so there is only one copy of the texture to deal with.  Only need to swizzle the
                 * texture to the internal texture memory.
                 */
                nvSwizzleTexture16Bpp((DWORD)dwSrcX0,
                                      (DWORD)dwSrcY0,
                                      (DWORD)dwSrcX1,
                                      (DWORD)dwSrcY1,
                                      (DWORD)gblDstDDS->wWidth,
                                      (DWORD)gblDstDDS->wHeight,
                                      (DWORD)dwSrcPitch,
                                      (DWORD)dwSrcLinearBase,
                                      (DWORD)dwDstX0,
                                      (DWORD)dwDstY0,
                                      (DWORD)dwTexturePointer);
            }

            /*
             * Only the internal texture surface has been modified.  If a lock on this texture
             * comes through then it will be neccessary to de-swizzle the internal texture
             * to the user texture memory, so set the flag that shows this has happened.
             */
            pTexture->dwTextureFlags  |= NV3_TEXTURE_BLIT_UPDATE;
        }
        pTexture->fpTexture        = (FLATPTR)dwDstLinearBase;
        pTexture->dwTextureFlags  &= ~(NV3_TEXTURE_MODIFIED | NV3_TEXTURE_MUST_RELOAD);

        /*
         * Auto generate mipmaps
         */
        if (pTexture->dwTextureFlags & NV4_TEXTURE_AUTOMIPMAPPED)
        {
            nvTextureAutoMipMap (pTexture);
        }

        /*
         * Save frequently accessed globals.
         */
        NV_D3D_GLOBAL_SAVE();

#ifdef  CACHE_FREECOUNT
        pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT

        /*
         * Display the texture surface to the current display buffer (debug only).
         */
        dbgDisplayTexture((PNVD3DCONTEXT)pTexture->hContext, pTexture);
    }
    else
    {
        /*
         * Set up the starting postion of the blit.
         */
        yDstCurrent = dwDstY0;
        ySrcCurrent = dwSrcY0;

        /*
         * Don't have an internal texture suface, so just do the blit to
         * the user texture surface.
         */
        for (yCount = 0; yCount < yHeight; yCount++)
        {
            pSrcLinear = (unsigned short *)(dwSrcLinearBase + (ySrcCurrent++ * dwSrcPitch));
            pDstLinear = (unsigned short *)(dwDstLinearBase + (yDstCurrent++ * dwDstPitch));
            for (xCount = 0; xCount < xWidth; xCount++)
                pDstLinear[dwDstX0 + xCount] = pSrcLinear[dwSrcX0 + xCount];
        }
    }

    return;
}
void nvTextureBlit32Bpp
(
    LPDDHAL_BLTDATA pbd
)
{
    unsigned long               dwSrcLinearBase, dwDstLinearBase;
    unsigned long              *pSrcLinear;
    unsigned long              *pDstLinear;
    long                        xCount, yCount;
    long                        xWidth, yHeight;
    PNVD3DTEXTURE               pBaseTexture = 0;
    PNVD3DTEXTURE               pTexture = 0;
    DWORD                       ySrcCurrent;
    DWORD                       yDstCurrent;
    DWORD                       dwSrcX0, dwSrcY0;
    DWORD                       dwSrcX1, dwSrcY1;
    DWORD                       dwSrcPitch;
    DWORD                       dwSrcWidth;
    DWORD                       dwSrcHeight;
    DWORD                       dwSrcLimit;
    DWORD                       dwDstX0, dwDstY0;
    DWORD                       dwDstX1, dwDstY1;
    DWORD                       dwDstPitch;
    DWORD                       dwDstWidth;
    DWORD                       dwDstHeight;
    DWORD                       dwDstLimit;
    DWORD                       dwTexturePointer = 0;
    DWORD                       dwTextureFormat = 0;
    DWORD                       dwSrcBitCount;
    DWORD                       dwDstBitCount;
    LPDDRAWI_DDRAWSURFACE_LCL   lclSrcDDS;
    LPDDRAWI_DDRAWSURFACE_LCL   lclDstDDS;
    LPDDRAWI_DDRAWSURFACE_GBL   gblSrcDDS;
    LPDDRAWI_DDRAWSURFACE_GBL   gblDstDDS;

    /*
     * Get source surface information.
     */
    lclSrcDDS       = pbd->lpDDSrcSurface;
    gblSrcDDS       = lclSrcDDS->lpGbl;
    dwSrcPitch      = gblSrcDDS->lPitch;
    dwSrcLinearBase = (unsigned long)gblSrcDDS->fpVidMem;
    dwSrcLimit      = (dwSrcPitch * (DWORD)gblSrcDDS->wHeight) - 1;
    if (gblSrcDDS->ddpfSurface.dwFlags & DDPF_RGB)
        dwSrcBitCount = gblSrcDDS->ddpfSurface.dwRGBBitCount;
    else
        dwSrcBitCount = pDriverData->bi.biBitCount;

    /*
     * Get destination surface information.
     */
    lclDstDDS       = pbd->lpDDDestSurface;
    gblDstDDS       = lclDstDDS->lpGbl;
    dwDstPitch      = gblDstDDS->lPitch;
    dwDstLinearBase = (unsigned long)gblDstDDS->fpVidMem;
    dwDstLimit      = (dwDstPitch * (DWORD)gblDstDDS->wHeight) - 1;
    if (gblDstDDS->ddpfSurface.dwFlags & DDPF_RGB)
        dwDstBitCount = gblDstDDS->ddpfSurface.dwRGBBitCount;
    else
        dwDstBitCount = pDriverData->bi.biBitCount;

    if (dwSrcBitCount != dwSrcBitCount)
    {
        /*
         * Not supporting bit depth conversion blits.
         */
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvTextureBlit32Bpp - Source and destination have different bit depths!!");
        dbgD3DError();
        return;
    }
    else if (dwSrcBitCount == 16)
    {
        nvTextureBlit16Bpp(pbd);
        return;
    }
    else if (dwSrcBitCount == 8)
    {
        nvTextureBlit8Bpp(pbd);
        return;
    }

    /*
     * Check for a non-square texture.
     */
    if (gblDstDDS->wWidth != gblDstDDS->wHeight)
        DPF_LEVEL(NVDBG_LEVEL_INFO, "nvTextureBlit32bpp - Non-Square Texture!!");

    /*
     * Get the pointer to the internal texture structure.
     */
    pTexture = (PNVD3DTEXTURE)lclDstDDS->dwReserved1;

    /*
     * Get the pointer to the internal texture surface.
     */
    if (pTexture)
    if (pTexture)
    {
        if (pTexture->dwTextureFlags & NV3_TEXTURE_USER_MIP_LEVEL)
            dwTexturePointer = pTexture->dwTexturePointer;
        else
        {
            if (pTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
#ifndef WINNT
                dwTexturePointer = pDriverData->BaseAddress + pTexture->dwTextureOffset;
#else
                dwTexturePointer = pDriverData->ppdev->pjScreen + pTexture->dwTextureOffset;
#endif // #ifdef WINNT
            else
            {
                if (pDriverData->GARTLinearBase > 0)
                    dwTexturePointer = pTexture->dwTexturePointer;
                else
                    dwTexturePointer = GetPointerTextureHeap(pTexture->dwTextureOffset);
            }
        }
    }

    /*
     * Get the blit postion.
     * NOTE: X1 and Y1 are exclusive!!
     */
    dwSrcX0 = pbd->rSrc.left;
    dwSrcX1 = pbd->rSrc.right;
    dwSrcY0 = pbd->rSrc.top;
    dwSrcY1 = pbd->rSrc.bottom;
    dwDstX0 = pbd->rDest.left;
    dwDstY0 = pbd->rDest.top;
    dwDstX1 = pbd->rDest.right;
    dwDstY1 = pbd->rDest.bottom;

    /*
     * Calculate the width and height of the blit.
     */
    dwSrcWidth  = dwSrcX1 - dwSrcX0;
    dwSrcHeight = dwSrcY1 - dwSrcY0;
    dwDstWidth  = dwDstX1 - dwDstX0;
    dwDstHeight = dwDstY1 - dwDstY1;
    xWidth      = (dwDstWidth  <= dwSrcWidth)  ? dwDstWidth  : dwSrcWidth;
    yHeight     = (dwDstHeight <= dwSrcHeight) ? dwDstHeight : dwSrcHeight;

    /*
     * Display the source texture on the screen. (DEBUG only)
     */
    dbgDisplayLinearTexture(lclSrcDDS, lclDstDDS, pTexture->dwTextureColorFormat);

    /*
     * If there's a valid pointer to an internal texture surface, then a dual
     * internal/user texture blit can be performed.
     */
    if (dwTexturePointer)
    {
        /*
         * Setup frequently accessed globals.
         */
        NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
        nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetObject);
#endif  // CACHE_FREECOUNT

        dwTextureFormat = pTexture->dwTextureColorFormat;

        /*
         * Decide what kind of texture blit to do.
         */
        if (pTexture->dwTextureFlags & NV3_TEXTURE_USER_ALLOCATED)
        {
            /*
             * The application has locked the surface so there are two
             * copies of the texture to update.
             * If the swizzled texture is in video memory:
             * First do a fast linear copy from the source surface to the
             * linear user surface, then use the hardware to swizzle the
             * texture.
             * If the swizzled texture is in non-local video memory:
             * Do the swizzle and the linear blit at the same time.
             *
             * Hardware Restrictions require the destination texture offset to be 32 byte aligned
             * and the source width to be 2 or more.  If this isn't the case must use software to
             * do the swizzling.
             * The only time the offset should be misaligned at this point is for mipmap levels of
             * 2x2 and 1x1.
             */
            if ((pTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
             && (pDriverData->NvDevFlatDma)
             && ((pTexture->dwTextureOffset & 0x0000001F) == 0)
             && (dwSrcWidth > 1))
            {
                /*
                 * Now use hardware to do the swizzle from user texture memory.
                 */
                nvTMVideoTextureBlt (dwSrcLinearBase,dwSrcPitch,dwSrcX0,dwSrcY0,dwSrcX1,dwSrcY1,
                                     dwDstLinearBase,dwDstPitch,dwDstX0,dwDstY0,
                                     pTexture->dwTextureOffset,
                                     (WORD)pTexture->dwMipMapBaseU, (WORD)pTexture->dwMipMapBaseV,
                                     dwTextureFormat);
            }
            else
            {
                nvSwizzleBlit32Bpp((DWORD)dwSrcX0,
                                   (DWORD)dwSrcY0,
                                   (DWORD)dwSrcX1,
                                   (DWORD)dwSrcY1,
                                   (DWORD)gblDstDDS->wWidth,
                                   (DWORD)gblDstDDS->wHeight,
                                   (DWORD)dwSrcPitch,
                                   (DWORD)dwSrcLinearBase,
                                   (DWORD)dwDstX0,
                                   (DWORD)dwDstY0,
                                   (DWORD)dwDstPitch,
                                   (DWORD)dwDstLinearBase,
                                   (DWORD)dwTexturePointer,
                                   (DWORD)dwTextureFormat);
            }
        }
        else
        {
            /*
             * Hardware Restrictions require the destination texture offset to be 32 byte aligned
             * and the source width to be 2 or more.  If this isn't the case must use software to
             * do the swizzling.
             * The only time the offset should be misaligned at this point is for mipmap levels of
             * 2x2 and 1x1.
             */
            if ((pTexture->dwTextureFlags & NV3_TEXTURE_VIDEO_MEMORY)
             && (pDriverData->NvDevFlatDma)
             && ((pTexture->dwTextureOffset & 0x0000001F) == 0)
             && (dwSrcWidth > 1))
            {
                nvTMVideoTextureBlt (dwSrcLinearBase,dwSrcPitch,dwSrcX0,dwSrcY0,dwSrcX1,dwSrcY1,
                                     0,dwDstPitch,dwDstX0,dwDstY0,
                                     pTexture->dwTextureOffset,
                                     (WORD)pTexture->dwMipMapBaseU, (WORD)pTexture->dwMipMapBaseV,
                                     dwTextureFormat);
            }
            else
            {
                /*
                 * This is a good application.  It's not locking and touching the texture directly
                 * so there is only one copy of the texture to deal with.  Only need to swizzle the
                 * texture to the internal texture memory.
                 */
                nvSwizzleTexture32Bpp((DWORD)dwSrcX0,
                                      (DWORD)dwSrcY0,
                                      (DWORD)dwSrcX1,
                                      (DWORD)dwSrcY1,
                                      (DWORD)gblDstDDS->wWidth,
                                      (DWORD)gblDstDDS->wHeight,
                                      (DWORD)dwSrcPitch,
                                      (DWORD)dwSrcLinearBase,
                                      (DWORD)dwDstX0,
                                      (DWORD)dwDstY0,
                                      (DWORD)dwTexturePointer);
            }

            /*
             * Only the internal texture surface has been modified.  If a lock on this texture
             * comes through then it will be neccessary to de-swizzle the internal texture
             * to the user texture memory, so set the flag that shows this has happened.
             */
            pTexture->dwTextureFlags  |= NV3_TEXTURE_BLIT_UPDATE;
        }
        pTexture->fpTexture        = (FLATPTR)dwDstLinearBase;
        pTexture->dwTextureFlags  &= ~(NV3_TEXTURE_MODIFIED | NV3_TEXTURE_MUST_RELOAD);

        /*
         * Auto generate mipmaps
         */
        if (pTexture->dwTextureFlags & NV4_TEXTURE_AUTOMIPMAPPED)
        {
            nvTextureAutoMipMap (pTexture);
        }

        /*
         * Save frequently accessed globals.
         */
        NV_D3D_GLOBAL_SAVE();

#ifdef  CACHE_FREECOUNT
        pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT

        /*
         * Display the texture surface to the current display buffer (debug only).
         */
        dbgDisplayTexture((PNVD3DCONTEXT)pTexture->hContext, pTexture);
    }
    else
    {

        /*
         * Set up the starting postion of the blit.
         */
        yDstCurrent = dwDstY0;
        ySrcCurrent = dwSrcY0;

        /*
         * Don't have an internal texture suface, so just do the blit to
         * the user texture surface.
         */
        for (yCount = 0; yCount < yHeight; yCount++)
        {
            pSrcLinear = (unsigned long *)(dwSrcLinearBase + (ySrcCurrent++ * dwSrcPitch));
            pDstLinear = (unsigned long *)(dwDstLinearBase + (yDstCurrent++ * dwDstPitch));
            for (xCount = 0; xCount < xWidth; xCount++)
                pDstLinear[dwDstX0 + xCount] = pSrcLinear[dwSrcX0 + xCount];
        }
    }

    return;
}
#endif // !NV_TEX2

/*
 *
 */
DWORD UnplugSystemMemoryTextureContext
(
    void
)
{
    NvNotification             *npDmaPusherSyncNotifier;

    npDmaPusherSyncNotifier = (NvNotification *)pDriverData->NvDmaPusherSyncNotifierFlat;

    /*
     * Unplug from DX6 triangle
     */
#ifndef CACHE_FREECOUNT
    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetStartMethod + sizeSetData));
#endif  // !CACHE_FREECOUNT
    while (nvFreeCount < (sizeSetObject + sizeSetStartMethod + sizeSetData))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                         (sizeSetObject + sizeSetDx6TriangleContexts));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_DX6_MULTI_TEXTURE_TRIANGLE);
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV055_SET_CONTEXT_DMA_A, 1);
    nvglSetData(nvFifo, nvFreeCount, NV01_NULL_OBJECT);
    nvStartDmaBuffer (FALSE);

    /*
     * Send a couple NOPS and notify on completion.
     * This should hopefully ensure that the contexts are unplugged before they are changed.
     */
#ifndef NV_NULL_HW_DRIVER
    while (npDmaPusherSyncNotifier->status == NV_IN_PROGRESS);
#endif //!NV_NULL_HW_DRIVER
    while (nvFreeCount < ((sizeSetStartMethod + sizeSetData) * 3))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                         ((sizeSetStartMethod + sizeSetData) * 3));
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV055_NO_OPERATION, 1);
    nvglSetData(nvFifo, nvFreeCount, 0);
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV055_NOTIFY, 1);
    nvglSetData(nvFifo, nvFreeCount, NV055_NOTIFY_WRITE_ONLY);
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV055_NO_OPERATION, 1);
    nvglSetData(nvFifo, nvFreeCount, 0);
    npDmaPusherSyncNotifier->status = NV_IN_PROGRESS;
    nvStartDmaBuffer (FALSE);
#ifndef NV_NULL_HW_DRIVER
    while (npDmaPusherSyncNotifier->status == NV_IN_PROGRESS);
#endif //!NV_NULL_HW_DRIVER

    /*
     * Unplug from DX5 triangle
     */
    while (nvFreeCount < (sizeSetObject + sizeSetStartMethod + sizeSetData))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                         (sizeSetObject + sizeSetStartMethod + sizeSetData));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_DX5_TEXTURED_TRIANGLE);
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV054_SET_CONTEXT_DMA_A, 1);
    nvglSetData(nvFifo, nvFreeCount, NV01_NULL_OBJECT);
    nvStartDmaBuffer (FALSE);

    /*
     * Send a couple NOPS and notify on completion.
     * This should hopefully ensure that the contexts are unplugged before they are changed.
     */
#ifndef NV_NULL_HW_DRIVER
    while (npDmaPusherSyncNotifier->status == NV_IN_PROGRESS);
#endif //!NV_NULL_HW_DRIVER
    while (nvFreeCount < (sizeSetStartMethod + sizeSetData + sizeDx5TriangleNotify))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                         (sizeSetStartMethod + sizeSetData + sizeDx5TriangleNotify));
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV054_NO_OPERATION, 1);
    nvglSetData(nvFifo, nvFreeCount, 0);
    nvglDx5TriangleNotify(nvFifo, nvFreeCount, NV_DD_SPARE);
    npDmaPusherSyncNotifier->status = NV_IN_PROGRESS;
    nvStartDmaBuffer (FALSE);
#ifndef NV_NULL_HW_DRIVER
    while (npDmaPusherSyncNotifier->status == NV_IN_PROGRESS);
#endif //!NV_NULL_HW_DRIVER

    /*
     * Force both triangle states to be resent.
     */
    pDriverData->dDrawSpareSubchannelObject = 0;
    return (0);
}

/*
 * Some commonly used code for setting up the system memory/agp memory texture contexts and
 * DMA pusher contexts.
 */
DWORD AdjustTextureLimit
(
    DWORD   dwContext,
    DWORD   dwHeapBase,
    DWORD   dwNewLimit,
    DWORD   dwOldLimit
)
{
#ifdef  MULTI_MON
    DWORD   dwStatus;

    if (pDriverData->dwTextureContextValid)
    {
        /*
         * Should probably unplug the object before destroying.
         */
        while (nvFreeCount < (sizeSetObject + sizeSetDx6TriangleContexts))
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                             (sizeSetObject + sizeSetDx6TriangleContexts));
        nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_DX6_MULTI_TEXTURE_TRIANGLE);
        nvglSetDx6TriangleContexts(nvFifo, nvFreeCount, NV_DD_SPARE, NV01_NULL_OBJECT, NV01_NULL_OBJECT);

        while (nvFreeCount < (sizeSetObject + sizeSetDx5TriangleContexts))
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                             (sizeSetObject + sizeSetDx5TriangleContexts));
        nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_DX5_TEXTURED_TRIANGLE);
        nvglSetDx5TriangleContexts(nvFifo, nvFreeCount, NV_DD_SPARE, NV01_NULL_OBJECT, NV01_NULL_OBJECT);
        nvStartDmaBuffer (FALSE);

        /*
         * First destroy the current context.
         */
        if (NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                 D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY) != NVOS00_STATUS_SUCCESS)
                    return(TRUE);
        pDriverData->dwTextureContextValid = FALSE;
    }

    dwStatus = 7; // NVOS03_STATUS_
    if (dwHeapBase && dwNewLimit)
    {
        dwStatus = NvRmAllocContextDma(pDriverData->dwRootHandle,
                                       D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY,
                                       NV01_CONTEXT_DMA,
                                       (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                                        ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED),
                                       (PVOID)dwHeapBase,
                                       dwNewLimit);
    }

    if (dwStatus != 0) // NVOS03_STATUS_SUCCESS
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "AdjustTextureLimit: Could not resize system memory texture context");
        dbgD3DError();
        if (dwHeapBase && dwOldLimit)
        {
            if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                                D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY,
                                NV01_CONTEXT_DMA,
                                (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                                 ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED),
                                (PVOID)dwHeapBase,
                                dwOldLimit) != ALLOC_CTX_DMA_STATUS_SUCCESS)
                                    return(TRUE);
        }
        else
        {
#ifndef WINNT
            if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                                D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY,
                                NV01_CONTEXT_DMA,
                                (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                                 ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                                (PVOID)pDriverData->BaseAddress,
                                0xFF) != ALLOC_CTX_DMA_STATUS_SUCCESS)
                                    return(TRUE);
#else
            if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                                D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY,
                                NV01_CONTEXT_DMA,
                                (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                                 ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                                (PVOID)pDriverData->ppdev->pjScreen,
                                0xFF) != ALLOC_CTX_DMA_STATUS_SUCCESS)
                                    return(TRUE);
#endif // #ifdef WINNT
        }
    }
    pDriverData->dwTextureContextValid = TRUE;

    /*
     * Force both triangle states to be resent.
     */
    pDriverData->dDrawSpareSubchannelObject = 0;
    return (dwStatus);
#else   // !MULTI_MON
    NvNotification             *npDmaPusherSyncNotifier;

    npDmaPusherSyncNotifier = (NvNotification *)pDriverData->NvDmaPusherSyncNotifierFlat;

#ifndef NV_NULL_HW_DRIVER
    while (npDmaPusherSyncNotifier->status == NV_IN_PROGRESS);
#endif //!NV_NULL_HW_DRIVER
    npDmaPusherSyncNotifier->status = NV_IN_PROGRESS;

    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetDmaFromMemoryNotify));
    while (nvFreeCount < (sizeSetObject + sizeSetDmaFromMemoryNotify))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetDmaFromMemoryNotify));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, dwContext);
    nvglSetDmaFromMemoryNotify(nvFifo, nvFreeCount, NV_DD_SPARE,
                               dwHeapBase, pDriverData->flatSelector, dwNewLimit, 0);
    nvStartDmaBuffer (FALSE);
    pDriverData->dDrawSpareSubchannelObject = dwContext;
#ifndef NV_NULL_HW_DRIVER
    while (npDmaPusherSyncNotifier->status == NV_IN_PROGRESS);
#endif //!NV_NULL_HW_DRIVER

    if (npDmaPusherSyncNotifier->status)
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "AdjustTextureLimit - Could not adjust the texture limit!!");
        dbgD3DError();
        npDmaPusherSyncNotifier->status = NV_IN_PROGRESS;
        if (dwOldLimit)
        {
            nvglSetDmaFromMemoryNotify(nvFifo, nvFreeCount, NV_DD_SPARE,
                                       dwHeapBase, pDriverData->flatSelector, dwOldLimit, 0);
        }
        else
        {
            nvglSetDmaFromMemoryNotify(nvFifo, nvFreeCount, NV_DD_SPARE,
                                       0, 0, 0, 0);
        }
        nvStartDmaBuffer (FALSE);
#ifndef NV_NULL_HW_DRIVER
        while (npDmaPusherSyncNotifier->status == NV_IN_PROGRESS);
#endif //!NV_NULL_HW_DRIVER
        /*
         * non-0 = bad.  error locking memory.
         */
        return (1);
    }

    /*
     * 0 = good, no error.
     */
    return (0);
#endif  // !MULTI_MON
}
DWORD UpdateTriangleContexts
(
    void
)
{
    NvNotification             *npDmaPusherSyncNotifier;

    npDmaPusherSyncNotifier = (NvNotification *)pDriverData->NvDmaPusherSyncNotifierFlat;

    /*
     * Plug into DX6 triangle.
     */
    while (nvFreeCount < (sizeSetObject + sizeSetDx6TriangleContexts))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                         (sizeSetObject + sizeSetDx6TriangleContexts));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_DX6_MULTI_TEXTURE_TRIANGLE);
    nvglSetDx6TriangleContexts(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
    nvStartDmaBuffer (FALSE);

    /*
     * Send a couple NOPS and notify on completion.
     * This should hopefully ensure that the contexts are unplugged before they are changed.
     */
#ifndef NV_NULL_HW_DRIVER
    while (npDmaPusherSyncNotifier->status == NV_IN_PROGRESS);
#endif //!NV_NULL_HW_DRIVER
    while (nvFreeCount < ((sizeSetStartMethod + sizeSetData) * 3))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                         ((sizeSetStartMethod + sizeSetData) * 3));
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV055_NO_OPERATION, 1);
    nvglSetData(nvFifo, nvFreeCount, 0);
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV055_NOTIFY, 1);
    nvglSetData(nvFifo, nvFreeCount, NV055_NOTIFY_WRITE_ONLY);
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV055_NO_OPERATION, 1);
    nvglSetData(nvFifo, nvFreeCount, 0);
    npDmaPusherSyncNotifier->status = NV_IN_PROGRESS;
    nvStartDmaBuffer (FALSE);
#ifndef NV_NULL_HW_DRIVER
    while (npDmaPusherSyncNotifier->status == NV_IN_PROGRESS);
#endif //!NV_NULL_HW_DRIVER

    /*
     * Plug into DX5 triangle.
     */
    while (nvFreeCount < (sizeSetObject + sizeSetDx5TriangleContexts))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                         (sizeSetObject + sizeSetDx5TriangleContexts));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_DX5_TEXTURED_TRIANGLE);
    nvglSetDx5TriangleContexts(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
    nvStartDmaBuffer (FALSE);

    /*
     * Send a couple NOPS and notify on completion.
     * This should hopefully ensure that the contexts are unplugged before they are changed.
     */
#ifndef NV_NULL_HW_DRIVER
    while (npDmaPusherSyncNotifier->status == NV_IN_PROGRESS);
#endif //!NV_NULL_HW_DRIVER
    while (nvFreeCount < (sizeSetStartMethod + sizeSetData + sizeDx5TriangleNotify))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                         (sizeSetStartMethod + sizeSetData + sizeDx5TriangleNotify));
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV054_NO_OPERATION, 1);
    nvglSetData(nvFifo, nvFreeCount, 0);
    nvglDx5TriangleNotify(nvFifo, nvFreeCount, NV_DD_SPARE);
    npDmaPusherSyncNotifier->status = NV_IN_PROGRESS;
    nvStartDmaBuffer (FALSE);
#ifndef NV_NULL_HW_DRIVER
    while (npDmaPusherSyncNotifier->status == NV_IN_PROGRESS);
#endif //!NV_NULL_HW_DRIVER

    /*
     * Force both triangle states to be resent.
     */
    pDriverData->dDrawSpareSubchannelObject = 0;
    return (0);
}

#ifdef  MULTI_MON
/*
 * Destroy all texture DMA contexts and free all allocated system memory for shutdown.
 */
void D3DDestroyTextureContexts
(
    void
)
{
    /*
     * Free the texture heap DMA context.
     */
    if (pDriverData->dwTextureContextValid)
    {
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                 D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY);
        pDriverData->dwTextureContextValid = FALSE;
    }

    /*
     * Free the system memory texture heap.
     */
    if (pDriverData->hTextureHeap)
    {
        if (pDriverData->GARTLinearBase == 0)
        {
            /*
             * Only need to free the heap if it was allocated.  (it won't be
             * allocated on an AGP system)
             */
#ifndef OLDSTUFF
            HEAPFREE((HANDLE)pDriverData->hTextureHeap, (PTEXHEAPHEADER)pDriverData->pRealHeapBase);
            HEAPDESTROY((HANDLE)pDriverData->hTextureHeap);
#else
            HeapFree((HANDLE)pDriverData->hTextureHeap, 0, (PTEXHEAPHEADER)pDriverData->pRealHeapBase);
            HeapDestroy((HANDLE)pDriverData->hTextureHeap);
#endif // #ifdef OLDSTUFF

        }
        else
            DestroyAGPListArray();
    }

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

    /*
     * Next free the texture manager heap context DMA.
     */
    if (pDriverData->dwTMStageHandle)
    {
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                 D3D_CONTEXT_LINEAR_TEXTURE_FROM_MEMORY);

        /*
         * And free the texture manager memory.
         */
        nvTMDestroy();
    }
}
#endif  // MULTI_MON

#endif  // NV4
