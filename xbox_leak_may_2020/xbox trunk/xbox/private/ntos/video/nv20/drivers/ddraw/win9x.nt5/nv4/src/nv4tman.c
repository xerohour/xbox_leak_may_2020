/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4TMAN.C                                                         *
*   Texture Blit Manager routines.                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 06/03/98 - created.                     *
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
#ifdef  MULTI_MON
#include "nvrmapi.h"
#include "nvrmarch.inc"
#endif  // MULTI_MON

/*
 * externals
 */
void nvReset2DSurfaceFormat (void);

/*
 * Local Switches
 */
#define TM_OFFSETALIGN          32
#define TM_PITCHALIGN           4

/*
 * Locals
 */
BOOL bTMHadToWaitForStagingSpace;

/*
 * Local Routines
 */
DWORD __inline nvTMGetFreeSpace
(
    void
)
{
    long size;

    if (pDriverData->dwTMHead >= pDriverData->dwTMTail)
        size = (long)(pDriverData->dwTMStageMax - pDriverData->dwTMHead);
    else
        size = (long)(pDriverData->dwTMTail - pDriverData->dwTMHead - 1);

    return (size < 0) ? 0 : size;
}

/*
 * Public Routines
 */
DWORD nvPlacePsuedoNotifier
(
    DWORD offset,
    DWORD value
)
{
    DWORD topLft = 0x00000000 + (offset / 4);

    /*
     * Set proper surface info
     */
    while (nvFreeCount < (sizeSet2DSurfacesDestination + sizeSet2DSurfacesColorFormat + sizeSet2DSurfacesPitch))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSet2DSurfacesDestination + sizeSet2DSurfacesColorFormat + sizeSet2DSurfacesPitch));
    nvglSet2DSurfacesDestination (nvFifo,nvFreeCount,NV_DD_SURFACES, pDriverData->dwTMPseudoNotifierOffset);
    nvglSet2DSurfacesColorFormat (nvFifo,nvFreeCount,NV_DD_SURFACES, NV042_SET_COLOR_FORMAT_LE_Y32);
    nvglSet2DSurfacesPitch       (nvFifo,nvFreeCount,NV_DD_SURFACES, 0x20,0x20);

    /*
     * Do solid rect fill (with argument as color)
     */
    while (nvFreeCount < (sizeSetObject + 2 * sizeSetStartMethod + 3 * sizeSetData))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + 2 * sizeSetStartMethod + 3 * sizeSetData));
    nvglSetObject      (nvFifo,nvFreeCount,NV_DD_SPARE, D3D_RENDER_SOLID_RECTANGLE_2);
    nvglSetStartMethod (nvFifo,nvFreeCount,NV_DD_SPARE, NV05E_COLOR,1);
    nvglSetData        (nvFifo,nvFreeCount,             value);
    nvglSetStartMethod (nvFifo,nvFreeCount,NV_DD_SPARE, NV05E_RECTANGLE(0),2);
    nvglSetData        (nvFifo,nvFreeCount,             topLft);
    nvglSetData        (nvFifo,nvFreeCount,             0x00010001);

    pDriverData->dDrawSpareSubchannelObject = D3D_RENDER_SOLID_RECTANGLE_2;

    /*
     * reset when we are done
     */
    nvReset2DSurfaceFormat();
    return 0;
}

/*
 * Create TexMan Heap
 */
DWORD nvTMCreate
(
    void
)
{
#ifdef TM_STAGER_IN_AGP
    DWORD dwAddr = 0;

    __asm int 3;

    if (pDriverData->GARTLinearBase)
    {
        /*
         * allocate agp memory
         */
        dwAddr = (DWORD)AllocAGPHeap(lpDriverObject, TM_STAGESPACE+0x2000);
        if (dwAddr)
        {
            /*
             * Setup variables
             */
            pDriverData->dwTMStageHandle = dwAddr; // keep addr here too
            pDriverData->dwTMStageMem    = dwAddr;
            pDriverData->dwTMStageAddr   = (pDriverData->dwTMStageMem + 0xfff) & 0xfffff000;
            pDriverData->dwTMStageMax    = pDriverData->dwTMStageAddr + TM_STAGESPACE;
            pDriverData->dwTMHead        = pDriverData->dwTMStageAddr;
            pDriverData->dwTMTail        = pDriverData->dwTMStageAddr;

            /*
             * Debug
             */
#ifdef DEBUG
            memset ((void*)pDriverData->dwTMStageAddr,0x55,TM_STAGESPACE);
#endif

            /*
             * Success
             */
            return 0;
        }
    }

    if (!dwAddr)
    {
        __asm int 3;
        // ***************************** fall back to PCI then - todo
        pDriverData->dwTMStageHandle = 0;
    }
#else
    /*
     * init
     */
    pDriverData->dwTMStageHandle = 0;

    /*
     * Allocate staging space
     */
#ifndef OLDSTUFF
    if (pDriverData->dwTMStageHandle = (DWORD)HEAPCREATE(TM_STAGESPACE+0x3000))
#else
    if (pDriverData->dwTMStageHandle = (DWORD)HeapCreate(HEAP_SHARED, TM_STAGESPACE+0x3000, 0))
#endif
    {
        /*
         * Allocate TexMan stage
         */
#ifndef OLDSTUFF
        if (pDriverData->dwTMStageMem = (DWORD)HEAPALLOC(pDriverData->dwTMStageHandle, TM_STAGESPACE+0x2000))
#else
        if (pDriverData->dwTMStageMem = (DWORD)HeapAlloc((HANDLE)pDriverData->dwTMStageHandle, HEAP_ZERO_MEMORY, TM_STAGESPACE+0x2000))
#endif
        {
            /*
             * Setup vairables
             */
            pDriverData->dwTMStageAddr = (pDriverData->dwTMStageMem + 0xfff) & 0xfffff000;
            pDriverData->dwTMStageMax  = pDriverData->dwTMStageAddr + TM_STAGESPACE;
            pDriverData->dwTMHead      = pDriverData->dwTMStageAddr;
            pDriverData->dwTMTail      = pDriverData->dwTMStageAddr;

            /*
             * Lock down the heap
             */
#ifdef  MULTI_MON
            if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                                D3D_CONTEXT_LINEAR_TEXTURE_FROM_MEMORY,
                                NV01_CONTEXT_DMA,
                                (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                                 ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED),
                                (PVOID)pDriverData->dwTMStageAddr,
                                TM_STAGESPACE + 0xfff) != ALLOC_CTX_DMA_STATUS_SUCCESS)
                                    return(TRUE);
#else   // !MULTI_MON
            while (nvFreeCount < (sizeSetObject + sizeSetDmaFromMemoryNotify))
                nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetDmaFromMemoryNotify));
            nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_LINEAR_TEXTURE_FROM_MEMORY);
            nvglSetDmaFromMemoryNotify(nvFifo, nvFreeCount, NV_DD_SPARE,
                                       pDriverData->dwTMStageAddr, pDriverData->flatSelector, TM_STAGESPACE + 0xfff, 0);
            pDriverData->dDrawSpareSubchannelObject = D3D_CONTEXT_LINEAR_TEXTURE_FROM_MEMORY;
#endif  // !MULTI_MON

            /*
             * Debug
             */
#ifdef DEBUG
            memset ((void*)pDriverData->dwTMStageAddr,0x55,TM_STAGESPACE);
#endif

            /*
             * Success
             */
            return 0;
        }
        else
        {
#ifndef OLDSTUFF
            HEAPDESTROY (pDriverData->dwTMStageHandle);
#else
            HeapDestroy ((HANDLE)pDriverData->dwTMStageHandle);
#endif
        }
    }
#endif //TM_STAGER_IN_AGP

    pDriverData->dwTMStageHandle = 0; // <- this will indicate that we failed creating the texture manager
    return 1;
}

/*
 * Destroy TexMan Heap
 */
void nvTMDestroy
(
    void
)
{

#ifdef TM_STAGER_IN_AGP
    if (pDriverData->dwTMStageHandle)
    {
        FreeAGPHeap(lpDriverObject, pDriverData->dwTMStageHandle);
    }
#else
    if (pDriverData->dwTMStageHandle)
    {
        if (pDriverData->dwTMStageMem)
        {
#ifndef OLDSTUFF
            HEAPFREE (pDriverData->dwTMStageHandle,(void*)pDriverData->dwTMStageMem);
#else
            HeapFree ((HANDLE)pDriverData->dwTMStageHandle,0,(void*)pDriverData->dwTMStageMem);
#endif
            pDriverData->dwTMStageMem = 0;
        }
#ifndef OLDSTUFF
        HEAPDESTROY (pDriverData->dwTMStageHandle);
#else
        HeapDestroy ((HANDLE)pDriverData->dwTMStageHandle);
#endif
    }
#endif //TM_STAGER_IN_AGP

    pDriverData->dwTMStageHandle = 0;
}

/*
 * nvTMAllocTextureStagingSpace
 */
DWORD nvTMAllocTextureStagingSpace
(
    DWORD  dwSize,
    DWORD *pdwAddr,
    DWORD *pdwOffset
)
{
    DWORD dwSizeNeeded;
    DWORD dwFreeSpace;
    DWORD dwFirst;

#ifdef NV_NULL_HW_DRIVER
    *pdwAddr   = pDriverData->dwTMStageAddr;
    *pdwOffset = 0;
    return dwTMHead;
#endif //NV_NULL_HW_DRIVER

    /*
     * Get free space in staging heap
     */
    dwSizeNeeded = dwSize + TM_OFFSETALIGN;

    DPF_LEVEL(NVDBG_LEVEL_INFO, "TM: Check 1");
    DPF_LEVEL(NVDBG_LEVEL_INFO, "TM:   dwTMHead   = %08x",pDriverData->dwTMHead);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "TM:   dwTMTail   = %08x",pDriverData->dwTMTail);
    DPF_LEVEL(NVDBG_LEVEL_INFO, "TM:   free space = %08x",nvTMGetFreeSpace());
    DPF_LEVEL(NVDBG_LEVEL_INFO, "TM:");

    /*
     * Check if we can commit the space
     */
    dwFreeSpace  = nvTMGetFreeSpace();
    if (dwFreeSpace >= dwSizeNeeded)
    {
        DWORD addr;

    cont:
        addr     = (pDriverData->dwTMHead + (TM_OFFSETALIGN - 1)) & ~(TM_OFFSETALIGN-1);
        pDriverData->dwTMHead = addr + dwSize;

        DPF_LEVEL(NVDBG_LEVEL_INFO, "TM: Commit Buffer");
        DPF_LEVEL(NVDBG_LEVEL_INFO, "TM:   New Head = %08x",pDriverData->dwTMHead);
        DPF_LEVEL(NVDBG_LEVEL_INFO, "TM:   Buf Addr = %08x",addr);
        DPF_LEVEL(NVDBG_LEVEL_INFO, "TM:   Buf Ofs  = %08x",addr - pDriverData->dwTMStageAddr);

        *pdwAddr   = addr;
#ifdef TM_STAGER_IN_AGP
        *pdwOffset = addr - pDriverData->GARTLinearBase;
#else
        *pdwOffset = addr - pDriverData->dwTMStageAddr;
#endif

        return pDriverData->dwTMHead; // this is what tail should be when texture blit retires
    }

    /*
     * Not enough space
     */
    DPF_LEVEL(NVDBG_LEVEL_INFO, "TM: Out of staging space");
    DPF_LEVEL(NVDBG_LEVEL_INFO, "TM:");

#ifdef NV_PROFILE
    NVP_START(NVP_T_TSTAGE);
#endif

    dwFirst = 1;
    for (;;)
    {
        /*
         * Read Tail from psuedo notifier
         */
#ifdef DEBUG
        {
            DWORD old = pDriverData->dwTMTail;
#endif
#ifndef WINNT
        pDriverData->dwTMTail = *(DWORD*)(pDriverData->dwTMPseudoNotifierOffset + NV_PN_TEX_RETIRE + pDriverData->BaseAddress);
#else
        pDriverData->dwTMTail = *(DWORD*)(pDriverData->dwTMPseudoNotifierOffset + NV_PN_TEX_RETIRE + pDriverData->ppdev->pjScreen);
#endif // #ifdef WINNT

#ifdef DEBUG
            if (old < pDriverData->dwTMTail) memset ((void*)old,0x55,pDriverData->dwTMTail-old);
            if (old > pDriverData->dwTMTail) memset ((void*)old,0x55,pDriverData->dwTMStageMax-old);
        }
#endif

        DPF_LEVEL(NVDBG_LEVEL_INFO, "TM: Check 2");
        DPF_LEVEL(NVDBG_LEVEL_INFO, "TM:   dwTMHead   = %08x",pDriverData->dwTMHead);
        DPF_LEVEL(NVDBG_LEVEL_INFO, "TM:   dwTMTail   = %08x",pDriverData->dwTMTail);
        DPF_LEVEL(NVDBG_LEVEL_INFO, "TM:   free space = %08x",nvTMGetFreeSpace());
        DPF_LEVEL(NVDBG_LEVEL_INFO, "TM:");

        /*
         * Attempt to wrap around
         */
        if ((pDriverData->dwTMHead >= pDriverData->dwTMTail) && (pDriverData->dwTMTail > pDriverData->dwTMStageAddr)) {
            DPF_LEVEL(NVDBG_LEVEL_INFO, "TM: Wrap around");
            /*
             * Set head to start of buffer
             */
            pDriverData->dwTMHead = pDriverData->dwTMStageAddr;
            /*
             * Update tail
             */
            nvTMUpdateRetirementDate (pDriverData->dwTMHead);
            /*
             * Kick off data
             */
            nvStartDmaBuffer (FALSE);
        }

        /*
         * Check if we have enough space now
         */
        dwFreeSpace  = nvTMGetFreeSpace();
        if (dwFreeSpace >= dwSizeNeeded) goto retire;

        /*
         * Not enough space yet
         *   go to never-never land for a while
         *   save some PCI bus activity
         */
        if (dwFirst)
        {
            /*
             * Start Buffer just in case we have pending data (avoids hanging too)
             */
            nvStartDmaBuffer (FALSE);
            dwFirst = 0;
        }

        bTMHadToWaitForStagingSpace = TRUE;
        NV_DELAY;
    }

retire:
#ifdef NV_PROFILE
    NVP_STOP(NVP_T_TSTAGE);
    nvpLogTime(NVP_T_TSTAGE,nvpTime[NVP_T_TSTAGE]);
#endif
    goto cont;
}

void nvTMUpdateRetirementDate
(
    DWORD dwRetirementAddress
)
{
    DPF_LEVEL(NVDBG_LEVEL_FUNCTION_CALL, "TM: Placing notifier at %08x",dwRetirementAddress);
    nvPlacePsuedoNotifier (NV_PN_TEX_RETIRE,dwRetirementAddress);
}

/*
 * Copy Texels
 */
void __inline nvTMCopyTexels
(
    DWORD dwSrcLine,
    DWORD dwSrcPitch,
    DWORD dwDst1Line,       // 1st destination
    DWORD dwDst1Pitch,
    DWORD dwDst2Line,       // 2nd destination (may be NULL)
    DWORD dwDst2Pitch,      // must be 0 when above is NULL
    DWORD dwSpanX,          // in bytes
    DWORD dwSpanY           // in lines
)
{
#ifdef NV_TEX2
    DWORD i,y;

    i = 4096 / dwSpanX; // move 4k at a time

    for (y = dwSpanY; y;)
    {
        DWORD h = min(y,i);
        DWORD z;

        if (h)
        {
            DWORD src = dwSrcLine;

            for (z=h; z; z--)
            {
                nvMemTouch (src,dwSpanX);
                src += dwSrcPitch;
            }
            for (z=h; z; z--)
            {
                nvMemCopy (dwDst1Line,dwSrcLine,dwSpanX,NV_MEMCOPY_WANTDESTALIGNED);
                if (dwDst2Line) nvMemCopy (dwDst2Line,dwSrcLine,dwSpanX,NV_MEMCOPY_WANTDESTALIGNED);

                dwDst1Line += dwDst1Pitch;
                dwDst2Line += dwDst2Pitch;
                dwSrcLine  += dwSrcPitch;
            }

            y -= h;
        }
        else // width too large for multi spans, do a line at a time
        {
            nvMemCopy (dwDst1Line,dwSrcLine,dwSpanX,NV_MEMCOPY_WANTDESTALIGNED);
            if (dwDst2Line) nvMemCopy (dwDst2Line,dwSrcLine,dwSpanX,NV_MEMCOPY_WANTDESTALIGNED);

            dwDst1Line += dwDst1Pitch;
            dwDst2Line += dwDst2Pitch;
            dwSrcLine  += dwSrcPitch;
            y--;
        }

    }

#else // !NV_TEX2

    long y;

    if (dwDst2Line) {
        if ((dwSpanX == dwDst1Pitch) && (dwSpanX == dwDst2Pitch) && (dwSpanX == dwSrcPitch))
        {
            DWORD size = dwSpanX * dwSpanY;
            while (size)
            {
                if (size >= 4096)
                {
                    memcpy ((void*)dwDst1Line,(void*)dwSrcLine,4096);
                    memcpy ((void*)dwDst2Line,(void*)dwSrcLine,4096);
                    size -= 4096;
                }
                else
                {
                    memcpy ((void*)dwDst1Line,(void*)dwSrcLine,size);
                    size = 0;
                }
            }
            return;
        }

        for (y=(long)dwSpanY; y>0; y--)
        {
            memcpy ((void*)dwDst1Line,(void*)dwSrcLine,dwSpanX);
            memcpy ((void*)dwDst2Line,(void*)dwSrcLine,dwSpanX);
            dwDst1Line += dwDst1Pitch;
            dwDst2Line += dwDst2Pitch;
            dwSrcLine  += dwSrcPitch;
        }
    }
    else
    {
        if ((dwSpanX == dwDst1Pitch) && (dwSpanX == dwSrcPitch))
        {
            DWORD size = dwSpanX * dwSpanY;
            memcpy ((void*)dwDst1Line,(void*)dwSrcLine,size);
            return;
        }

        for (y=(long)dwSpanY; y>0; y--)
        {
            memcpy ((void*)dwDst1Line,(void*)dwSrcLine,dwSpanX);
            dwDst1Line += dwDst1Pitch;
            dwSrcLine  += dwSrcPitch;
        }
    }
#endif // !NV_TEX2
}

/*
 * Setup swizzled Blt
 */
void __inline nvTMSwizzleBlt
(
    DWORD dwSrcOffset,
    DWORD dwSrcPitch,
    DWORD dwImageColorFormat,

    DWORD dwDstSwizzleOffset,
    DWORD dwMipMapBaseU,
    DWORD dwMipMapBaseV,
    DWORD dwSurfaceColorFormat,

    DWORD dwDstX0,
    DWORD dwDstY0,

    DWORD dwBlitWidth,
    DWORD dwBlitHeight
)
{
    /*
     * target surface
     */
    while (nvFreeCount < (sizeSetObject + sizeSetSwizzledSurface))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetSwizzledSurface));
    nvglSetObject          (nvFifo,nvFreeCount,NV_DD_SPARE, D3D_CONTEXT_SURFACE_SWIZZLED);
    nvglSetSwizzledSurface (nvFifo,nvFreeCount,NV_DD_SPARE,
                                        (DRF_NUM(052,_SET_FORMAT,_COLOR ,dwSurfaceColorFormat)
                                        |DRF_NUM(052,_SET_FORMAT,_WIDTH ,dwMipMapBaseU)
                                        |DRF_NUM(052,_SET_FORMAT,_HEIGHT,dwMipMapBaseV)),
                                         dwDstSwizzleOffset);

    /*
     * format
     */
    while (nvFreeCount < (sizeSetObject + sizeSetScaledImageFormat))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetScaledImageFormat));
    nvglSetObject               (nvFifo,nvFreeCount,NV_DD_SPARE, D3D_SCALED_IMAGE_FROM_MEMORY);
    nvglSetScaledImageFormat    (nvFifo,nvFreeCount,NV_DD_SPARE, dwImageColorFormat);

    /*
     * source memory context
     */
    while (nvFreeCount < sizeSetScaledImageContextImage)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetScaledImageContextImage);
#ifdef TM_STAGER_IN_AGP
    nvglSetScaledImageContextImage(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY);
#else
    nvglSetScaledImageContextImage(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_LINEAR_TEXTURE_FROM_MEMORY);
#endif

    /*
     * clip
     */
    while (nvFreeCount < (sizeScaledImageClip + sizeScaledImageOut + sizeScaledImageDeltaDuDxDvDy))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeScaledImageClip + sizeScaledImageOut + sizeScaledImageDeltaDuDxDvDy));
    nvglScaledImageClip          (nvFifo,nvFreeCount,NV_DD_SPARE, ((DWORD)dwDstX0), ((DWORD)dwDstY0), dwBlitWidth, dwBlitHeight);
    nvglScaledImageOut           (nvFifo,nvFreeCount,NV_DD_SPARE, ((DWORD)dwDstX0), ((DWORD)dwDstY0), dwBlitWidth, dwBlitHeight);
    nvglScaledImageDeltaDuDxDvDy (nvFifo,nvFreeCount,NV_DD_SPARE, (1<<20), (1<<20));

    /*
     * src & go
     */
    while (nvFreeCount < (sizeScaledImageInSize + sizeScaledImageInFormat + sizeScaledImageInOffset + sizeScaledImageInPoint))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeScaledImageInSize + sizeScaledImageInFormat + sizeScaledImageInOffset + sizeScaledImageInPoint));
    nvglScaledImageInSize   (nvFifo, nvFreeCount, NV_DD_SPARE, dwBlitWidth, dwBlitHeight);
    nvglScaledImageInFormat (nvFifo, nvFreeCount, NV_DD_SPARE,
                             (DRF_NUM(077,_IMAGE_IN_FORMAT,_PITCH,        dwSrcPitch)
                             |DRF_DEF(077,_IMAGE_IN_FORMAT,_ORIGIN,       _CORNER)
                             |DRF_DEF(077,_IMAGE_IN_FORMAT,_INTERPOLATOR, _ZOH)));
    nvglScaledImageInOffset (nvFifo, nvFreeCount, NV_DD_SPARE, dwSrcOffset);
    nvglScaledImageInPoint  (nvFifo, nvFreeCount, NV_DD_SPARE, 0, 0);

    pDriverData->dDrawSpareSubchannelObject = D3D_SCALED_IMAGE_FROM_MEMORY;
}

/*
 * Stage a Texture Swizzle Blt
 */
void nvTMVideoTextureBlt
(
    DWORD dwSrcLinearBase,
    DWORD dwSrcPitch,
    DWORD dwSrcX0,
    DWORD dwSrcY0,
    DWORD dwSrcX1,
    DWORD dwSrcY1,
    DWORD dwDstLinearBase,
    DWORD dwDstPitch,
    DWORD dwDstX0,
    DWORD dwDstY0,
    DWORD dwDstSwizzleOffset,
    DWORD dwMipMapBaseU,
    DWORD dwMipMapBaseV,
    DWORD dwTextureColorFormat
)
{
    DWORD dwImageColorFormat;
    DWORD dwSurfaceColorFormat;
    DWORD dwBytesPerPixel;
    DWORD dwWidth;
    DWORD dwHeight;
    DWORD dwStagePitch;
    DWORD dwStageSize;
    DWORD dwSrcLinearAddr;
    DWORD dwDstLinearAddr;
    DWORD dwSpan;

    /*
     * Determine surface attributes
     */
    switch (dwTextureColorFormat)
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

    /*
     * Calc staging area
     */
    dwHeight        = dwSrcY1 - dwSrcY0;
    dwWidth         = dwSrcX1 - dwSrcX0;
    dwSpan          = dwWidth * dwBytesPerPixel;
    dwStagePitch    = (dwSpan + (TM_PITCHALIGN-1)) & ~(TM_PITCHALIGN-1);
    dwStageSize     = dwStagePitch * dwHeight;
    dwSrcLinearAddr = dwSrcLinearBase + (dwSrcPitch * dwSrcY0) + (dwSrcX0 * dwBytesPerPixel);
    dwDstLinearAddr = dwDstLinearBase + (dwDstPitch * dwDstY0) + (dwDstX0 * dwBytesPerPixel);

    if (dwWidth >= 2048) {

        // the HW can't swizzle textures this wide. do it via CPU

        nvFlushDmaBuffers();

        if (dwSrcLinearBase) {

            // we've got a linear source and two destinations: one linear. one swizzled

            if (dwBytesPerPixel == 2) {
                nvSwizzleBlit16Bpp (dwSrcX0, dwSrcY0,
                                    dwSrcX1, dwSrcY1,
                                    dwWidth, dwHeight,
                                    dwSrcPitch,
                                    dwSrcLinearBase,
                                    dwDstX0, dwDstY0,
                                    dwDstPitch,
                                    dwDstLinearBase,
                                    pDriverData->ppdev->pjScreen + dwDstSwizzleOffset,
                                    dwTextureColorFormat);
            }
            else {
                nvSwizzleBlit32Bpp (dwSrcX0, dwSrcY0,
                                    dwSrcX1, dwSrcY1,
                                    dwWidth, dwHeight,
                                    dwSrcPitch,
                                    dwSrcLinearBase,
                                    dwDstX0, dwDstY0,
                                    dwDstPitch,
                                    dwDstLinearBase,
                                    pDriverData->ppdev->pjScreen + dwDstSwizzleOffset,
                                    dwTextureColorFormat);
            }

        }

        else {

            // we've got one linear source (passed in via dwDstLinearBase for reasons
            // that totally escape me) and one swizzled destination

            if (dwBytesPerPixel == 2) {
                nvSwizzleBlit16Bpp (dwSrcX0, dwSrcY0,
                                    dwSrcX1, dwSrcY1,
                                    dwWidth, dwHeight,
                                    dwSrcPitch,
                                    dwDstLinearBase,
                                    dwDstX0, dwDstY0,
                                    dwDstPitch,
                                    dwDstLinearBase,  // copy it back on top of itself
                                    pDriverData->ppdev->pjScreen + dwDstSwizzleOffset,
                                    dwTextureColorFormat);
            }
            else {
                nvSwizzleBlit32Bpp (dwSrcX0, dwSrcY0,
                                    dwSrcX1, dwSrcY1,
                                    dwWidth, dwHeight,
                                    dwSrcPitch,
                                    dwDstLinearBase,
                                    dwDstX0, dwDstY0,
                                    dwDstPitch,
                                    dwDstLinearBase,  // copy it back on top of itself
                                    pDriverData->ppdev->pjScreen + dwDstSwizzleOffset,
                                    dwTextureColorFormat);
            }

        }

        return;
    }

    /*
     * Small requests can be handled elegantly in one step
     */
    if (dwStageSize < (TM_STAGESPACE / 2))
    {
        DWORD dwStageHandle;
        DWORD dwStageAddr;
        DWORD dwStageOffset;

        /*
         * Allocate staging space - this function may kick off pusher
         */
        dwStageHandle = nvTMAllocTextureStagingSpace(dwStageSize,&dwStageAddr,&dwStageOffset);

        /*
         * Copy texels
         */
        if (dwSrcLinearBase)
        {
            if (dwDstLinearBase)
                nvTMCopyTexels (dwSrcLinearAddr,dwSrcPitch,     // source
                                dwStageAddr,dwStagePitch,       // dest 1
                                dwDstLinearAddr,dwDstPitch,     // dest 2
                                dwSpan,dwHeight);
            else
                nvTMCopyTexels (dwSrcLinearAddr,dwSrcPitch,     // source
                                dwStageAddr,dwStagePitch,       // dest
                                0,0,                            // nop
                                dwSpan,dwHeight);
        }
        else
            nvTMCopyTexels (dwDstLinearAddr,dwDstPitch,     // source
                            dwStageAddr,dwStagePitch,       // dest
                            0,0,                            // nop
                            dwSpan,dwHeight);

        /*
         * Setup the swizzle Blt
         */
        nvTMSwizzleBlt (dwStageOffset,dwStagePitch,dwImageColorFormat,
                        dwDstSwizzleOffset,dwMipMapBaseU,dwMipMapBaseV,dwSurfaceColorFormat,
                        dwDstX0,dwDstY0,dwWidth,dwHeight);

        /*
         * Report Retirement
         */
        nvTMUpdateRetirementDate (dwStageHandle);

        /*
         * Start Buffer just in case HW is idle
         */
        nvStartDmaBuffer (TRUE);
    }
    else
    /*
     * Large textures are sent in sections
     */
    {
        DWORD dwSection;
        DWORD dwSectionDelta;
        DWORD dwSectionSrcPitch;
        DWORD dwSectionDstPitch;
        DWORD dwSectionHeight;

        /*
         * init
         */
        dwSectionDelta    = (TM_STAGESPACE / 3) / dwStagePitch; // # of lines to fill 1/3 of the buffer
        dwSectionSrcPitch = (dwSrcLinearBase) ? (dwSrcPitch * dwSectionDelta) : 0;
        dwSectionDstPitch = (dwDstLinearBase) ? (dwDstPitch * dwSectionDelta) : 0;

        /*
         * step through sections
         */

        /*
        { // fill swizzle buffer
            DWORD size = (1<<dwMipMapBaseU)*(1<<dwMipMapBaseV)*dwBytesPerPixel;
            memset ((void*)(dwDstSwizzleOffset+pDriverData->BaseAddress),0xaa,size);
        }
        /**/

        for (dwSection = 0; dwSection < dwHeight; dwSection += dwSectionDelta)
        {
            DWORD dwStageHandle;
            DWORD dwStageAddr;
            DWORD dwStageOffset;

            /*
             * Prepare section
             */
            dwSectionHeight = dwHeight - dwSection;
            if (dwSectionHeight > dwSectionDelta) dwSectionHeight = dwSectionDelta;
            dwStageSize     = dwStagePitch * dwSectionHeight;

            /*
             * Allocate staging space - this function may kick off pusher
             */
            dwStageHandle = nvTMAllocTextureStagingSpace(dwStageSize,&dwStageAddr,&dwStageOffset);

            /*
             * Copy texels
             */
            if (dwSrcLinearBase)
            {
                if (dwDstLinearBase)
                    nvTMCopyTexels (dwSrcLinearAddr,dwSrcPitch,     // source
                                    dwStageAddr,dwStagePitch,       // dest 1
                                    dwDstLinearAddr,dwDstPitch,     // dest 2
                                    dwSpan,dwSectionHeight);
                else
                    nvTMCopyTexels (dwSrcLinearAddr,dwSrcPitch,     // source
                                    dwStageAddr,dwStagePitch,       // dest
                                    0,0,                            // nop
                                    dwSpan,dwSectionHeight);
            }
            else
                nvTMCopyTexels (dwDstLinearAddr,dwDstPitch,     // source
                                dwStageAddr,dwStagePitch,       // dest
                                0,0,                            // nop
                                dwSpan,dwSectionHeight);

            /*
             * Setup the swizzle Blt
             */
            nvTMSwizzleBlt (dwStageOffset,dwStagePitch,dwImageColorFormat,
                            dwDstSwizzleOffset,dwMipMapBaseU,dwMipMapBaseV,dwSurfaceColorFormat,
                            dwDstX0,dwDstY0,dwWidth,dwSectionHeight);

            /*
             * Report Retirement
             */
            nvTMUpdateRetirementDate (dwStageHandle);

            /*
             * Start Buffer
             */
            nvStartDmaBuffer (TRUE);

        /*
        NV_D3D_GLOBAL_SAVE();
        nvFlushDmaBuffers();
        NV_D3D_GLOBAL_SETUP();
        Sleep (10);
        /**/

            /*
             * prep for next
             */
            dwSrcLinearAddr += dwSectionSrcPitch;
            dwDstLinearAddr += dwSectionDstPitch;
            dwDstY0         += dwSectionDelta;
        }
    }
}
