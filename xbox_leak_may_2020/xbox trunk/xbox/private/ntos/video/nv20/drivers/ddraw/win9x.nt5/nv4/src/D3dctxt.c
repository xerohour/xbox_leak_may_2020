/*
 * Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 * (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: D3DCTXT.C                                                         *
*   The Direct 3d Context Management routines.                              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/02/97 - re-wrote and cleaned up.     *
*                                                                           *
\***************************************************************************/
#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "d3dinc.h"
#include "nv3ddbg.h"
#include "nvheap.h"

#ifdef NV_DISASM
HANDLE nvPusherFile               = 0;
BOOL   nvPusherDisassemblerEnable = 0;
#endif

//#define NV_TETRIS
#ifdef NV_TETRIS

/*
 * nvTetrisTile
 *
 * reorgs memory to cancel tiling reorg
 */
void nvTetrisTile
(
    DWORD  dwTetrisShift,
    DWORD  dwTetrisMode
)
{
    DWORD dwBase  = pDriverData->BaseAddress;
    DWORD dwSize  = pDriverData->TotalVRAM;

    DWORD dwMask2 = ((1 << dwTetrisMode) - 1) << (10 + dwTetrisShift);
    DWORD dwMask3 = dwMask2 >> dwTetrisMode;
    DWORD dwMask1 = ~(dwMask2 | dwMask3);

    DWORD dwGran  = 1 << (10 + dwTetrisShift - dwTetrisMode);

    BYTE  *p1     = GlobalAlloc(GPTR,dwGran);
    BYTE  *p2     = GlobalAlloc(GPTR,dwGran);

    DWORD dwAddr1;
    DWORD dwAddr2;

    for (dwAddr1 = 0; dwAddr1 < dwSize; dwAddr1 += dwGran)
    {
        dwAddr2 = (dwAddr1 & dwMask1)
                | ((dwAddr1 >> dwTetrisMode) & dwMask3)
                | ((dwAddr1 << dwTetrisMode) & dwMask2);

        if (dwAddr2 > dwAddr1)
        {
            memcpy (p1,(void*)(dwBase + dwAddr1),dwGran);
            memcpy (p2,(void*)(dwBase + dwAddr2),dwGran);
            memcpy ((void*)(dwBase + dwAddr1),p2,dwGran);
            memcpy ((void*)(dwBase + dwAddr2),p1,dwGran);
        }
    }

    GlobalFree (p1);
    GlobalFree (p2);
}

/*
 * nvTetrisTilingEnable
 *
 * safely enables tetris tiling
 */
void nvTetrisTilingEnable
(
    DWORD  dwTetrisShift,
    DWORD  dwTetrisMode
)
{
    DWORD dwCtrl   = ((DWORD)pDriverData->NvBaseFlat) + 0x100200;
    DWORD t;

    /*
     * read/modify register
     */
    t  = *(DWORD*)dwCtrl;
    t &= 0xfff00000;
    t |= 0x2000 | (dwTetrisMode << 15) | (dwTetrisShift << 18);

    /*
     * clear ints
     */
    __asm cli;

    /*
     * restore memory
     */
    nvTetrisTile (dwTetrisShift,dwTetrisMode);

    /*
     * write register
     */
    *(DWORD*)dwCtrl = t;

    /*
     * enable ints
     */
    __asm sti;
}

/*
 * nvTetrisTilingDisable
 *
 * safely enables tetris tiling
 */
void nvTetrisTilingDisable
(
    DWORD  dwTetrisShift,
    DWORD  dwTetrisMode
)
{
    DWORD dwCtrl   = ((DWORD)pDriverData->NvBaseFlat) + 0x100200;
    DWORD t;

    /*
     * read/modify register
     */
    t  = *(DWORD*)dwCtrl;
    t &= 0xfff00000;
    t |= 0x00001114;

    /*
     * clear ints
     */
    __asm cli;

    /*
     * restore memory
     */
    nvTetrisTile (dwTetrisShift,dwTetrisMode);

    /*
     * write register
     */
    *(DWORD*)dwCtrl = t;

    /*
     * enable ints
     */
    __asm sti;
}

#endif //NV_TETRIS

/*
 * Miscellaneous D3D context related routines.
 */
BOOL nvCreateContextListHeap
(
    void
)
{
    /*
     * Create a global heap to allocate internal D3D Context data structures out of.
     * The number of D3D Contexts available can be adjusted by a setting in the registry.
     * It is set reasonably large by default, so hopefully the registry override will never
     * be needed....but just in case.
     */
#ifndef OLDSTUFF
    pDriverData->hContexts_gbl = (DWORD)HEAPCREATE((pDriverData->regD3DContextMax * sizeof(NVD3DCONTEXT)));
#else
    pDriverData->hContexts_gbl = (DWORD)HeapCreate(HEAP_SHARED,
                                                   (pDriverData->regD3DContextMax * sizeof(NVD3DCONTEXT)),
                                                   0);
#endif
    if (!pDriverData->hContexts_gbl)
    {
        DPF_LEVEL (NVDBG_LEVEL_ERROR, "nvCreateContextListHeap - Could not create context list heap!");
        return (FALSE);
    }
    return (TRUE);
}
BOOL nvDestroyContextListHeap
(
    void
)
{
    PNVD3DCONTEXT   pContext;
    PNVD3DCONTEXT   pContextNext;

    /*
     * If there's no heap currently allocated, then there is no heap to destroy.
     */
    if (!pDriverData->hContexts_gbl)
        return (TRUE);

    /*
     * Free any contexts that are still allocated.
     */
    pContext = (PNVD3DCONTEXT)pDriverData->dwContextListHead;
    while (pContext)
    {
        pContextNext = pContext->pContextNext;
#ifndef OLDSTUFF
        HEAPFREE(pDriverData->hContexts_gbl, pContext);
#else
        HeapFree((HANDLE)pDriverData->hContexts_gbl, 0, pContext);
#endif
        pDriverData->dwD3DContextCount--;
        pContext = pContextNext;
    }

    /*
     * Destroy the heap.
     */
#ifndef OLDSTUFF
    HEAPDESTROY(pDriverData->hContexts_gbl);
#else
    HeapDestroy((HANDLE)pDriverData->hContexts_gbl);
#endif
    pDriverData->hContexts_gbl = (DWORD)NULL;
    return (TRUE);
}

/*
 * D3D HAL context callback routines
 */

/*
 * --------------------------------------------------------------------------
 * NvContextCreate
 *
 * LPD3DHAL_CONTEXTCREATEDATA pccd
 *    Pointer to a structure containing things including the current
 *    rendering surface, the current Z surface, and the DirectX object
 *    handle, etc.
 * --------------------------------------------------------------------------
 */
DWORD __stdcall nvContextCreate
(
    LPD3DHAL_CONTEXTCREATEDATA  pccd
)
{
    unsigned char              *pData = 0;
    HDDRVITEM                   hContext;
    LPDDRAWI_DDRAWSURFACE_LCL   lpLcl  = 0;
    LPDDRAWI_DDRAWSURFACE_GBL   lpGbl  = 0;
    LPDDRAWI_DDRAWSURFACE_LCL   lpLclZ = 0;

    /*
     * analyzer markers
     */
#ifdef NV_MARKER
    {
        extern DWORD dwMarkerCount;
        dwMarkerCount = 0;
    }
#endif
#ifdef NV_STATS
    {
        extern DWORD dwSceneCount;
        dwSceneCount = 0;
    }
#endif
    /*
     * push buffer disassembler
     */
#ifdef NV_DISASM
    {
        nvPusherFile = CreateFile("\\PushData.bin",GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
        if (nvPusherFile == INVALID_HANDLE_VALUE)
        {
            nvPusherFile = 0;
        }
    }
#endif

    /*
     * Validate the direct draw surface pointers.
     */
    if ((pccd->lpDDS) == NULL)
    {
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return (DDHAL_DRIVER_HANDLED);
    }
    if ((lpLcl = DDS_LCL(pccd->lpDDS)) == NULL)
    {
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return (DDHAL_DRIVER_HANDLED);
    }
    if (lpLcl->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
    {
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return (DDHAL_DRIVER_HANDLED);
    }
    if ((lpGbl = lpLcl->lpGbl) == NULL)
    {
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return DDHAL_DRIVER_HANDLED;
    }
    if ((pccd->lpDDSZ != NULL)
     && ((lpLclZ = DDS_LCL(pccd->lpDDSZ)) == NULL))
    {
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return (DDHAL_DRIVER_HANDLED);
    }
    if (lpLclZ && (lpLclZ->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY))
    {
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Get pointer to global driver data structure.
     */
    NvSetDriverDataPtr(pccd->lpDDGbl);

    /*
     * Setup frequently accessed globals.
     */
    NV_D3D_GLOBAL_SETUP();

#ifdef NV_TETRIS
    /*
     * enable tetris tiling
     */
    nvTetrisTilingEnable (1/* dwBPP / 8 - 1 */,4 /* # lines for a 4:1 aspect tile */);
#endif

    /*
     * NOTES:
     *
     * This callback is invoked when a new surface is to be used as a
     * rendering target.
     * The context handled returned will be used whenever rendering
     * to this surface is to be performed.
     *
     * Allocate a new context from the context array.
     */
#ifndef  OLDSTUFF
    hContext = (HDDRVITEM)HEAPALLOC(pDriverData->hContexts_gbl, sizeof(NVD3DCONTEXT));
#else
    hContext = (HDDRVITEM)HeapAlloc((HANDLE)pDriverData->hContexts_gbl,
                                    HEAP_ZERO_MEMORY,
                                    sizeof(NVD3DCONTEXT));
#endif

    /*
     * Get the pointer to the newly allocated context.
     */
    pCurrentContext = (PNVD3DCONTEXT)hContext;
    if (!pCurrentContext)
    {
        DPF_LEVEL (NVDBG_LEVEL_ERROR, "nvContextCreate - Bad Context");
        NV_D3D_GLOBAL_SAVE();
        pccd->ddrval = D3DHAL_OUTOFCONTEXTS;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Start with a clean slate.
     */
    memset(pCurrentContext, 0, sizeof(NVD3DCONTEXT));

    /*
     * Save off the applications DirectX version level.
     */
    pCurrentContext->dwDXVersionLevel = pccd->dwhContext;

    /*
     * Return the handle of the new context.
     */
    pccd->dwhContext = hContext;

    /*
     * Set the process ID for the context.
     */
    pCurrentContext->pid = pccd->dwPID;
#ifdef WINNT
    pCurrentContext->lpDD = pccd->lpDDGbl;
#endif // #ifdef WINNT

    /*
     * Initialize the surface specific context data.
     */
    pCurrentContext->lpLcl  = lpLcl;
    pCurrentContext->lpLclZ = lpLclZ;

    /*
     * Do the hardware specific context setup.
     */
    if (!nvSetupContext(pCurrentContext))
    {
        /*
         * When this call fails, it means there was some sort of problem with the
         * surface format.  Either an unsupported pixel format was specified or
         * an unsupported pixel/zeta format was specified.
         * Free the context memory and return an error.
         */
#ifndef OLDSTUFF
        HEAPFREE(pDriverData->hContexts_gbl, pCurrentContext);
#else
        HeapFree((HANDLE)pDriverData->hContexts_gbl, 0, (PVOID)pCurrentContext);
#endif
        pccd->dwhContext = 0;
        pCurrentContext  = 0;
        NV_D3D_GLOBAL_SAVE();
        pccd->ddrval = DDERR_INVALIDPIXELFORMAT;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Update the open context count.
     */
    pDriverData->dwD3DContextCount++;

    /*
     * Update the context list.
     */
    pCurrentContext->pContextPrev = (PNVD3DCONTEXT)NULL;
    pCurrentContext->pContextNext = (PNVD3DCONTEXT)pDriverData->dwContextListHead;
    if (pCurrentContext->pContextNext)
        pCurrentContext->pContextNext->pContextPrev = pCurrentContext;
    pDriverData->dwContextListHead = (DWORD)pCurrentContext;

    /*
     * Init scene counter
     */
    pCurrentContext->dwSceneCount = 0;


    /*
     * Initiate Anti-Aliasing runtime
     */
    if (!nvAACreate()) {
        DPF_LEVEL (NVDBG_LEVEL_ERROR, "Cannot create AA Context.");
        NV_D3D_GLOBAL_SAVE();
        pccd->ddrval = D3DHAL_OUTOFCONTEXTS;
        return (DDHAL_DRIVER_HANDLED);
    }

#ifdef HW_PAL8
    // allocate some video memory for texture palettes
    //    pCurrentContext->dwTexturePaletteStagingHandle =
    //    nvTMAllocTextureStagingSpace(256*sizeof(DWORD),
    //                                 &(pCurrentContext->dwTexturePaletteAddr),
    //                                 &(pCurrentContext->dwTexturePaletteOffset));
    NVHEAP_ALLOC(pCurrentContext->dwTexturePaletteAddr, 512*sizeof(DWORD), TYPE_IMAGE);
    if (!pCurrentContext->dwTexturePaletteAddr) dbgD3DError();
    pCurrentContext->dwTexturePaletteOffset =
        pCurrentContext->dwTexturePaletteAddr - pDriverData->BaseAddress;
#endif

#ifdef NV_PROFILE
    // Start up profiler
    nvpCreate();
#endif //NV_PROFILE

#ifdef NV_MARKER
    *(DWORD*)(pDriverData->BaseAddress) = 0xcccccccc;
#endif

    /*
     * Context creation complete, return successfully.
     */
    NV_D3D_GLOBAL_SAVE();
    pccd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}

DWORD __stdcall nvContextDestroy
(
    LPD3DHAL_CONTEXTDESTROYDATA pcdd
)
{

    /*
     * NOTES:
     * This callback is invoked when a context is to be destroyed.
     * Perform any cleanup required.
     *
     * Get pointer to context being destroyed.
     */
    pCurrentContext = (PNVD3DCONTEXT)pcdd->dwhContext;
    if (!pCurrentContext)
    {
        DPF_LEVEL (NVDBG_LEVEL_ERROR, "nvContextDestroy - Bad Context");
        pcdd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Get pointer to global driver data structure.
     */
    NvSetDriverDataPtrFromContext(pCurrentContext);

    if (pDriverData->dwD3DContextCount == 0)
    {
        pcdd->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }

#ifdef NV_MARKER
    *(DWORD*)(pDriverData->BaseAddress) = 0xdddddddd;
#endif

    /*
     * push buffer disassembler
     */
#ifdef NV_DISASM
    {
        if (nvPusherFile)
        {
            CloseHandle (nvPusherFile);
            nvPusherFile = 0;
        }
    }
#endif

#ifdef NV_PROFILE
    /*
     * Shut down profiler
     */
    nvpDestroy();
#endif //NV_PROFILE

    /*
     * Shutdown Anti-Aliasing runtime
     */
    if (pCurrentContext->pAAContext) nvAADestroy();

    /*
     * Setup frequently accessed globals.
     */
    NV_D3D_GLOBAL_SETUP();

#ifdef NV_TETRIS
    /*
     * disable tetris tiling
     */
    nvTetrisTilingDisable (1/* dwBPP / 8 - 1 */,4 /* # lines for a 4:1 aspect tile */);
#endif

    /*
     * Destroy all textures associated with this context. The Win2K D3D runtimes
     * seem to dissociate textures from contexts, and will call nvTextureDestroy
     * for each texture created in the context. Thus under Win2K only we defer
     * destroying the textures created in this context.
     */
    nvDestroyTextureContext(pcdd->dwhContext);

    /*
     * Free any default texture associated with this context.
     */
    nvFreeDefaultTexture(pCurrentContext);

    if (pCurrentContext->pContextPrev)
        pCurrentContext->pContextPrev->pContextNext = pCurrentContext->pContextNext;
    if (pCurrentContext->pContextNext)
        pCurrentContext->pContextNext->pContextPrev = pCurrentContext->pContextPrev;
    if (pDriverData->dwContextListHead == (DWORD)pCurrentContext)
        pDriverData->dwContextListHead = (DWORD)pCurrentContext->pContextNext;
#ifndef OLDSTUFF
    HEAPFREE(pDriverData->hContexts_gbl, pCurrentContext);
#else
    HeapFree((HANDLE)pDriverData->hContexts_gbl, 0, (PVOID)pCurrentContext);
#endif
    pDriverData->dwD3DContextCount--;

    /*
     * If there are no more open D3D contexts.  Do any neccessary cleanup.
     */
    if (!pDriverData->dwD3DContextCount)
        nvFinalContextCleanup();

    /*
     * Reset the global context and texture handles.
     */
    pCurrentContext = NULL;
    pDriverData->dwCurrentContextHandle = 0;

    /*
     * Return successfully.
     */
    NV_D3D_GLOBAL_SAVE();
    pcdd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}
DWORD __stdcall nvContextDestroyAll
(
    LPD3DHAL_CONTEXTDESTROYALLDATA  pcdd
)
{
    PNVD3DCONTEXT   pContext     = 0;
    PNVD3DCONTEXT   pContextNext = 0;

    /*
     * BUGBUG WINNT only - We believe this is an obsolete fct and therefore
     * will never be called, therefore we don't worry about loading
     * pDriverData.
     */

    /*
     * This routine is legacy and will only ever be called in non-mulitmonitor
     * situations, so it's ok to trust what has been previously loaded into
     * pDriverData.
     */
    if (pDriverData->dwD3DContextCount == 0)
    {
        pcdd->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Setup frequently accessed globals.
     */
    NV_D3D_GLOBAL_SETUP();

    /*
     * NOTES:
     *
     * This callback is invoked when a process dies.  All the contexts
     * which were created by this context need to be destroyed.
     *
     * Destroy all valid texture's associated with this process ID.
     */
    nvDestroyTextureProcess(pcdd->dwPID);

    pContext = (PNVD3DCONTEXT)pDriverData->dwContextListHead;
    while (pContext)
    {
        pContextNext = pContext->pContextNext;
        if (pContext->pid == pcdd->dwPID)
        {
            if (pContext->pContextPrev)
                pContext->pContextPrev->pContextNext = pContext->pContextNext;
            if (pContext->pContextNext)
                pContext->pContextNext->pContextPrev = pContext->pContextPrev;
            if (pDriverData->dwContextListHead == (DWORD)pContext)
                pDriverData->dwContextListHead = (DWORD)pContext->pContextNext;

#ifndef OLDSTUFF
            HEAPFREE(pDriverData->hContexts_gbl, pContext);
#else
            HeapFree((HANDLE)pDriverData->hContexts_gbl, 0, pContext);
#endif
            pDriverData->dwD3DContextCount--;
        }
        pContext = pContextNext;
    }

    /*
     * If there are no more open D3D contexts.  Do any neccessary cleanup.
     */
    if (!pDriverData->dwD3DContextCount)
        nvFinalContextCleanup();

    /*
     * Reset the global context and texture handles.
     */
    pDriverData->dwCurrentContextHandle = 0;
    NV_D3D_GLOBAL_SAVE();
    pcdd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}
