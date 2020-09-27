/*
 * Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 * (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: D3DRENDR.C                                                        *
*   The Direct 3d Rendereing routines.                                      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/02/97 - re-wrote and cleaned up.     *
*       Ben de Waal (bdw)           11/06/97 - Linear MipMap xxx            *
*                                                                           *
\***************************************************************************/
#include <math.h>
#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "d3dinc.h"
#include "nv3ddbg.h"

/*
 * Dma Pushing loop stats.
 */
DWORD   statDmaSyncLoop      = 0;
DWORD   statDmaSyncOverflow  = 0;
DWORD   statDmaBusyLoop      = 0;
DWORD   statDmaBusyOverflow  = 0;
DWORD   statDmaFlushLoop     = 0;
DWORD   statDmaFlushOverflow = 0;
#ifndef WINNT
/*
 * --------------------------------------------------------------------------
 * nvRenderState
 *
 * LPD3DHAL_RENDERSTATEDATA
 * - Points to the instruction in the execute buffer.
 *
 * We get the count of states to set and then walk the execute buffer
 * handling each one at a time.
 * --------------------------------------------------------------------------
 */
DWORD __stdcall nvRenderState
(
    LPD3DHAL_RENDERSTATEDATA    prd
)
{
    DWORD           i;
    LPBYTE          lpData;
    LPD3DSTATE      lpState;

    /*
     * NOTES:
     * This callback is invoked when a set of render states are changed.
     *
     * Get the pointer to the context this texture is associated with.
     */
    DPF_LEVEL (NVDBG_LEVEL_DDI_ENTRY, "nvRenderState - hContext = %08lx", prd->dwhContext);
    SET_CURRENT_CONTEXT(prd);

    /*
     * Get pointer to global driver. data structure.
     */
    NvSetDriverDataPtrFromContext(pCurrentContext);
    NV_D3D_GLOBAL_SETUP();

    /*
     *
     */
    lpData = (LPBYTE)(((LPDDRAWI_DDRAWSURFACE_INT)prd->lpExeBuf)->lpLcl->lpGbl->fpVidMem);
    for (i = 0, lpState = (LPD3DSTATE)(lpData + prd->dwOffset);
         i < prd->dwCount; i++, lpState++)
    {
        DWORD type = (DWORD) lpState->drstRenderStateType;

        if (IS_OVERRIDE(type))
        {
            DWORD override = GET_OVERRIDE(type);
            if (lpState->dwArg[0])
            {
                DPF_LEVEL (NVDBG_LEVEL_INFO, "nvRenderState - setting override for state %08lx", override);
                STATESET_SET(pCurrentContext->overrides, override);
            }
            else
            {
                DPF_LEVEL (NVDBG_LEVEL_INFO, "nvRenderState, clearing override for state %08lx", override);
                STATESET_CLEAR(pCurrentContext->overrides, override);
            }
            continue;
        }
        if (STATESET_ISSET(pCurrentContext->overrides, type))
        {
            DPF_LEVEL (NVDBG_LEVEL_INFO, "nvRenderState, state %08lx is overridden, ignoring", type);
            continue;
        }
        nvSetContextState(lpState->drstRenderStateType, lpState->dwArg[0], &prd->ddrval);
        if (prd->ddrval != DD_OK)
        {
            NV_D3D_GLOBAL_SAVE();
            return (DDHAL_DRIVER_HANDLED);
        }

    }
    NV_D3D_GLOBAL_SAVE();
    prd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}
#endif // #ifndef WINNT
DWORD __stdcall nvSetRenderTarget
(
    LPD3DHAL_SETRENDERTARGETDATA    psrtd
)
{
    LPDDRAWI_DDRAWSURFACE_LCL   lpLcl    = 0;
    LPDDRAWI_DDRAWSURFACE_LCL   lpLclZ   = 0;

    /*
     * Get the pointer to the context this texture is associated with.
     */
    DPF_LEVEL (NVDBG_LEVEL_DDI_ENTRY, "nvSetRenderTarget - hContext = %08lx", psrtd->dwhContext);
    SET_CURRENT_CONTEXT(psrtd);

    /*
     * Get pointer to global driver. data structure.
     */
    NvSetDriverDataPtrFromContext(pCurrentContext);

    /*
     * Set the new rendering surface into the D3D context.
     */
    if (!psrtd->lpDDS)
    {
        dbgD3DError();
        psrtd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return (DDHAL_DRIVER_HANDLED);
    }
    if (!(lpLcl = DDS_LCL(psrtd->lpDDS)))
    {
        dbgD3DError();
        psrtd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return (DDHAL_DRIVER_HANDLED);
    }
    if (psrtd->lpDDSZ)
        lpLclZ = DDS_LCL(psrtd->lpDDSZ);

    /*
     * Check for an invalid rendering and ZETA sufaces
     * (i.e. system memory surface)
     */
    if (lpLcl->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
    {
        dbgD3DError();
        psrtd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return (DDHAL_DRIVER_HANDLED);
    }
    if (lpLclZ
     && (lpLclZ->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY))
    {
        dbgD3DError();
        psrtd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Set the Rendering and Zeta surfaces into the D3D context.
     */
    pCurrentContext->lpLcl  = lpLcl;
    pCurrentContext->lpLclZ = lpLclZ;

    /*
     * Call the hardware specific context surface setup.
     */
    if (!nvSetContextSurface(pCurrentContext))
    {
        /*
         * Generally the only way this fails if if a stencil buffer is
         * being used with a 16 bit rendering target.
         */
        dbgD3DError();
        psrtd->ddrval = DDERR_INVALIDPIXELFORMAT;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Return invalid pixel format if the z-buffer depth does not match the render target depth.
     */
    if (pCurrentContext->dwContextFlags & NV_CONTEXT_ZETA_BUFFER_MISMATCH)
    {
        dbgD3DError();
        psrtd->ddrval = DDERR_INVALIDPIXELFORMAT;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Return successfully.
     */
    psrtd->ddrval = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

