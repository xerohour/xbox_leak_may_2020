#ifdef  NV4
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4CLEAR.C                                                        *
*   NV4 Buffer Clear routines.                                              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       04/18/98 - created                      *
*                                                                           *
\***************************************************************************/
#include <math.h>
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

void nvSetClearSurface
(
    DWORD   dwDstOffset,
    DWORD   dwDstPitch,
    DWORD   dwDstFormat,
    DWORD   dwRectFormat,
    DWORD   dwFillValue
)
{
#ifdef  DDPUSH_0
    /*
     * Set the destination surface to be cleared.
     */
    while (nvFreeCount < (sizeSetRop5 + sizeSet2DSurfacesDestination + sizeSet2DSurfacesColorFormat + sizeSet2DSurfacesPitch + sizeDrawRopRectAndTextColor1A))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_ROP, (sizeSetRop5 + sizeSet2DSurfacesDestination + sizeSet2DSurfacesColorFormat + sizeSet2DSurfacesPitch + sizeDrawRopRectAndTextColor1A));
    nvglSetRop5(nvFifo, nvFreeCount, NV_DD_ROP, SRCCOPYINDEX);
    nvglSet2DSurfacesDestination(nvFifo, nvFreeCount, NV_DD_SURFACES, dwDstOffset);
    nvglSet2DSurfacesColorFormat(nvFifo, nvFreeCount, NV_DD_SURFACES, dwDstFormat);
    nvglSet2DSurfacesPitch(nvFifo, nvFreeCount, NV_DD_SURFACES, dwDstPitch, dwDstPitch);
    nvglDrawRopRectAndTextColor1A(nvFifo, nvFreeCount, NV_DD_ROP_RECT_AND_TEXT, dwFillValue);
#else   // D3D Channel
    /*
     * Set the destination surface to be cleared.
     */
    while (nvFreeCount < (sizeSetObject + sizeSet2DSurfacesDestination + sizeSet2DSurfacesColorFormat + sizeSet2DSurfacesPitch + sizeSetNv4SolidRectangleColorFormat + sizeNv4SolidRectangleColor))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSet2DSurfacesDestination + sizeSet2DSurfacesColorFormat + sizeSet2DSurfacesPitch + sizeSetNv4SolidRectangleColorFormat + sizeNv4SolidRectangleColor));
    nvglSet2DSurfacesDestination(nvFifo, nvFreeCount, NV_DD_SURFACES, dwDstOffset);
    nvglSet2DSurfacesColorFormat(nvFifo, nvFreeCount, NV_DD_SURFACES, dwDstFormat);
    nvglSet2DSurfacesPitch(nvFifo, nvFreeCount, NV_DD_SURFACES, dwDstPitch, dwDstPitch);
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_RENDER_SOLID_RECTANGLE);
    nvglSetNv4SolidRectangleColorFormat(nvFifo, nvFreeCount, NV_DD_SPARE, dwRectFormat);
    nvglNv4SolidRectangleColor(nvFifo, nvFreeCount, NV_DD_SPARE, dwFillValue);
    pDriverData->dDrawSpareSubchannelObject = D3D_RENDER_SOLID_RECTANGLE;
#endif
    return;
}
void nvReset2DSurfaceFormat
(
    void
)
{
    while (nvFreeCount < sizeSet2DSurfacesColorFormat)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SURFACES, sizeSet2DSurfacesColorFormat);

    if (pDriverData->bi.biBitCount == 8)
    {
        nvglSet2DSurfacesColorFormat(nvFifo, nvFreeCount, NV_DD_SURFACES, NV042_SET_COLOR_FORMAT_LE_Y8);
    }
    else if (pDriverData->bi.biBitCount == 16)
    {
        nvglSet2DSurfacesColorFormat(nvFifo, nvFreeCount, NV_DD_SURFACES, NV042_SET_COLOR_FORMAT_LE_R5G6B5);
    }
    else
    {
        nvglSet2DSurfacesColorFormat(nvFifo, nvFreeCount, NV_DD_SURFACES, NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8);
    }
    return;
}
void nvClearSurfaceRect
(
    DWORD   startx,
    DWORD   starty,
    DWORD   width,
    DWORD   height
)
{
#ifdef  DDPUSH_0
    /*
     * Clear the specified rectangle.
     */
    while (nvFreeCount < sizeDrawRopRectAndTextPointSize)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_ROP_RECT_AND_TEXT, sizeDrawRopRectAndTextPointSize);
    nvglDrawRopRectAndTextPointSize(nvFifo, nvFreeCount, NV_DD_ROP_RECT_AND_TEXT, startx, starty, width, height);
#else   // D3D Channel
    /*
     * Clear the specified rectangle.
     */
    while (nvFreeCount < sizeNv4SolidRectanglePointSize)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeNv4SolidRectanglePointSize);
    nvglNv4SolidRectanglePointSize(nvFifo, nvFreeCount, NV_DD_SPARE, startx, starty, width, height);
#endif  // D3D Channel
    return;
}

BOOL nvD3DClearZBuffer
(
    LPDDRAWI_DDRAWSURFACE_LCL   lpLcl,
    DWORD                       dwFillDepth
)
{
    DWORD                       dwDstOffset;
    DWORD                       dwDstPitch;
    DWORD                       startx = 0;
    DWORD                       starty = 0;
    DWORD                       width, height;
    DWORD                       dwBitCount;
    DWORD                       dwRectColorFormat;
    DWORD                       dwSurfColorFormat;
    LPDDRAWI_DDRAWSURFACE_GBL   dst;

    DPF_LEVEL(NVDBG_LEVEL_FUNCTION_CALL, "nvD3DClearZBuffer");

    if (!lpLcl)
        return(FALSE);

#if D3D_PUSH
#ifdef  D3D_HARDWARE
    if (!pDriverData->NvDevFlatDma)
        return (FALSE);
#endif  // D3D_HARDWARE
#endif  // D3DPUSH

    /*
     * Setup frequently accessed globals.
     */
    NV_D3D_GLOBAL_SETUP();


    dst         = lpLcl->lpGbl;
    dwDstOffset = dst->fpVidMem - pDriverData->BaseAddress;
    dwDstPitch  = dst->lPitch;
    width       = (DWORD)dst->wWidth;
    height      = (DWORD)dst->wHeight;

    /*
     * Anti-Aliasing
     */
    if (pCurrentContext
     && pCurrentContext->dwAntiAliasFlags & (AA_MODE_SUPER | AA_MODE_SINGLECHUNK))
    {
        pCurrentContext->dwAntiAliasFlags    |= AA_CLEAR_SCREEN;
        pCurrentContext->pAAContext->dwZFill  = dwFillDepth;
        return TRUE;
    }

    if (lpLcl->dwFlags & DDRAWISURF_HASPIXELFORMAT)
        dwBitCount = lpLcl->lpGbl->ddpfSurface.dwRGBBitCount;
    else
        dwBitCount = pDriverData->bi.biBitCount;
    switch (dwBitCount)
    {
        case 16:
            dwRectColorFormat = NV05E_SET_COLOR_FORMAT_LE_X16R5G6B5;
            dwSurfColorFormat = NV042_SET_COLOR_FORMAT_LE_Y16;
            break;
        case 24:
        case 32:
            dwRectColorFormat = NV05E_SET_COLOR_FORMAT_LE_X8R8G8B8;
            dwSurfColorFormat = NV042_SET_COLOR_FORMAT_LE_Y32;
            break;
    }
#ifdef  CACHE_FREECOUNT
    nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_ROP, sizeSet2DSurfacesDestination);
#endif  // CACHE_FREECOUNT
    nvSetClearSurface(dwDstOffset, dwDstPitch, dwSurfColorFormat, dwRectColorFormat, dwFillDepth);
    nvClearSurfaceRect(startx, starty, width, height);
    nvReset2DSurfaceFormat();
    pDriverData->TwoDRenderingOccurred   = TRUE;
    pDriverData->ThreeDRenderingOccurred = 0;
    //nvStartDmaBuffer();
    NV_D3D_GLOBAL_SAVE();
#ifdef  CACHE_FREECOUNT
    pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
    return (TRUE);
}

/*
 * Clear render target and zeta-buffer in single call.
 */
DWORD nvClear
(
    LPD3DHAL_CLEARDATA  pcd
)
{
    DWORD dwDstOffset;
    DWORD dwDstPitch;
    DWORD dwRectColorFormat;
    DWORD dwSurfColorFormat;

    DPF_LEVEL(NVDBG_LEVEL_DDI_ENTRY, "nvClear - hContext = %08lx", pcd->dwhContext);
    if (!pCurrentContext->lpLcl)
    {
        pcd->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }

    if (!pDriverData->NvDevFlatDma)
    {
        pcd->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Need to make sure that an unfriendly mode switch didn't sneak and not cause
     * us to get re-enabled properly.
     */
    if (pDriverData->fFullScreenDosOccurred)
    {
        if (pDriverData->fFullScreenDosOccurred & 0x01)
            nvD3DReenable();
        else
        {
            pcd->ddrval = DD_OK;
            return (DDHAL_DRIVER_HANDLED);
        }
    }
    if (pCurrentContext->dwContextReset)
        nvResetContext((PNVD3DTEXTURE)NULL);

    /*
     * Get the format of the surface being cleared.
     */
    switch (DRF_VAL(053, _SET_FORMAT, _COLOR, pCurrentContext->dwSurfaceFormat))
    {
        case NV053_SET_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5:
        case NV053_SET_FORMAT_COLOR_LE_X1R5G5B5_O1R5G5B5:
        case NV053_SET_FORMAT_COLOR_LE_R5G6B5:
            dwRectColorFormat = NV05E_SET_COLOR_FORMAT_LE_X16R5G6B5;
            dwSurfColorFormat = NV042_SET_COLOR_FORMAT_LE_Y16;
            break;
        case NV053_SET_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8:
        case NV053_SET_FORMAT_COLOR_LE_X8R8G8B8_O8R8G8B8:
        case NV053_SET_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8:
        case NV053_SET_FORMAT_COLOR_LE_X1A7R8G8B8_O1A7R8G8B8:
        case NV053_SET_FORMAT_COLOR_LE_A8R8G8B8:
            dwRectColorFormat = NV05E_SET_COLOR_FORMAT_LE_X8R8G8B8;
            dwSurfColorFormat = NV042_SET_COLOR_FORMAT_LE_Y32;
            break;
    }

    /*
     * Anti-Aliasing
     */
    if (pCurrentContext->dwAntiAliasFlags & (AA_MODE_SUPER | AA_MODE_SINGLECHUNK))
    {
        if (pcd->dwFlags & D3DCLEAR_TARGET)
        {
            if (dwSurfColorFormat == NV042_SET_COLOR_FORMAT_LE_Y16)
                /*
                 * Get the fill color (24bit RGB) and convert it to 16bpp (565) format.
                 */
                pCurrentContext->pAAContext->dwCFill = ((pcd->dwFillColor & 0x00F80000) >> 8)
                                                     | ((pcd->dwFillColor & 0x0000F800) >> 5)
                                                     | ((pcd->dwFillColor & 0x000000F8) >> 3);
            else
                pCurrentContext->pAAContext->dwCFill = pcd->dwFillColor;
        }
        if (pcd->dwFlags & D3DCLEAR_ZBUFFER) pCurrentContext->pAAContext->dwZFill = pcd->dwFillDepth;

        if (!(pCurrentContext->dwAntiAliasFlags & AA_COPY_SCREEN))
        {
            pCurrentContext->dwAntiAliasFlags |= AA_CLEAR_SCREEN;
            pcd->ddrval = DD_OK;
            return (DDHAL_DRIVER_HANDLED);
        }
    }

#ifdef NV_CONTROLTRAFFIC
    if (pDriverData->regD3DEnableBits1 & REG_BIT1_CONTROLTRAFFIC)
    {
        if ((pcd->dwFlags & (D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET)) == D3DCLEAR_ZBUFFER)
        {
            DWORD f = pCurrentContext->dwCTFlags;
            BOOL  x;

            if (f & NV_CT_ENABLED)
            {
                if (f & NV_CT_DRAWPRIM) f ^= NV_CT_FRONT | NV_CT_DRAWPRIM;
                x = TRUE;
            }
            else
            {
                if (dwSurfColorFormat == NV042_SET_COLOR_FORMAT_LE_Y16)
                {
                    f |= (pcd->dwFillDepth & 0x8000) ? (NV_CT_FRONT | NV_CT_ENABLED)
                                                     : (              NV_CT_ENABLED);
                }
                else
                {
                    f |= (pcd->dwFillDepth & 0x80000000) ? (NV_CT_FRONT | NV_CT_ENABLED)
                                                         : (              NV_CT_ENABLED);
                }
                f &= ~NV_CT_DRAWPRIM;
                x  = FALSE;
            }
            pCurrentContext->dwCTFlags = f;

            NV_FORCE_TRI_SETUP (pCurrentContext);
            if (x)
            {
                pcd->ddrval = DD_OK;
                return (DDHAL_DRIVER_HANDLED);
            }
        }
        else
        {
            pCurrentContext->dwCTFlags = NV_CT_DISABLED;
            NV_FORCE_TRI_SETUP (pCurrentContext);
        }
    }
    else
    {
        pCurrentContext->dwCTFlags = NV_CT_DISABLED;
    }
#endif //NV_CONTROLTRAFFIC

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
     * Make sure the context has the correct suface information in it.
     */
    pCurrentContext->dwSurfaceAddr = 0;
    if (dbgFrontRender)
    {
#ifdef WINNT
        if (pCurrentContext->lpLcl->lpGbl->lPitch)
#endif
        pCurrentContext->dwSurfaceAddr            = VIDMEM_ADDR(pDriverData->CurrentVisibleSurfaceAddress);
        pCurrentContext->dwSurfaceOffset          = VIDMEM_OFFSET(pDriverData->CurrentVisibleSurfaceAddress);
        pCurrentContext->surfacePitch.wColorPitch = (WORD)dbgFrontRenderPitch;
    }
    else
    {
#ifdef WINNT
        if (pCurrentContext->lpLcl->lpGbl->lPitch)
#endif
        pCurrentContext->dwSurfaceAddr            = VIDMEM_ADDR(pCurrentContext->lpLcl->lpGbl->fpVidMem);
        pCurrentContext->dwSurfaceOffset          = VIDMEM_OFFSET(pCurrentContext->lpLcl->lpGbl->fpVidMem - pDriverData->BaseAddress);
        pCurrentContext->surfacePitch.wColorPitch = (WORD)pCurrentContext->lpLcl->lpGbl->lPitch;
    }

    pCurrentContext->ZBufferAddr = 0;
    if (pCurrentContext->lpLclZ)
    {
        /*
         * I've seen it happen where the location of the z-buffer will actually
         * change without without the context being destroyed or the set render
         * target call happening. So it's neccessary to make sure that we have the
         * correct z-buffer address and offset here.
         */
    #ifdef WINNT
        if (pCurrentContext->lpLcl->lpGbl->lPitch)
    #endif
        pCurrentContext->ZBufferAddr             = VIDMEM_ADDR(pCurrentContext->lpLclZ->lpGbl->fpVidMem);
        pCurrentContext->ZBufferOffset           = VIDMEM_OFFSET(pCurrentContext->lpLclZ->lpGbl->fpVidMem);
        pCurrentContext->surfacePitch.wZetaPitch = (WORD)pCurrentContext->lpLclZ->lpGbl->lPitch;
    }

    if ((pcd->dwFlags & D3DCLEAR_ZBUFFER)
     && (pCurrentContext->lpLclZ && pCurrentContext->lpLclZ->lpGbl)
     && (pCurrentContext->ZBufferAddr))
    {
        DWORD dwFillDepth = pcd->dwFillDepth;

        /*
         * Normal (not AA)
         */
        {
            DWORD       dwNumRects  = pcd->dwNumRects;
            LPD3DRECT   lpRects     = pcd->lpRects;

            dwDstOffset = pCurrentContext->ZBufferOffset;
            dwDstPitch  = (DWORD)pCurrentContext->surfacePitch.wZetaPitch;

            if (dwNumRects)
            {
                /*
                 * Set the destination surface to be cleared.
                 */
                nvSetClearSurface(dwDstOffset, dwDstPitch, dwSurfColorFormat, dwRectColorFormat, dwFillDepth);
                while (dwNumRects)
                {
                    DWORD startx, starty;
                    DWORD width, height;

                    /*
                     * Get the next rectangle to clear.
                     */
                    startx = lpRects->x1;
                    starty = lpRects->y1;
                    width  = lpRects->x2 - startx;
                    height = lpRects->y2 - starty;

                    /*
                     * Use the hardware to do the clear.
                     */
                    nvClearSurfaceRect(startx, starty, width, height);

                    /*
                     * Move to the next rectangle.
                     */
                    lpRects++;
                    dwNumRects--;
                }
            }
        }
    }

    if ((pcd->dwFlags & D3DCLEAR_TARGET)
     && (pCurrentContext->lpLcl && pCurrentContext->lpLcl->lpGbl)
     && (pCurrentContext->dwSurfaceAddr))
    {
        DWORD       dwFillColor;
        DWORD       dwNumRects  = pcd->dwNumRects;
        LPD3DRECT   lpRects     = pcd->lpRects;

        if (dwSurfColorFormat == NV042_SET_COLOR_FORMAT_LE_Y16)
            /*
             * Get the fill color (24bit RGB) and convert it to 16bpp (565) format.
             */
            dwFillColor = ((pcd->dwFillColor & 0x00F80000) >> 8)
                        | ((pcd->dwFillColor & 0x0000F800) >> 5)
                        | ((pcd->dwFillColor & 0x000000F8) >> 3);
        else
            dwFillColor = pcd->dwFillColor;

        dwDstOffset = pCurrentContext->dwSurfaceOffset;
        dwDstPitch  = (DWORD)pCurrentContext->surfacePitch.wColorPitch;

        /*
         * Clear the rendering target.
         * Send the correct default objects for all the subchannels
         * just to be safe.
         */
        if (dwNumRects)
        {
            /*
             * Set the destination surface to be cleared.
             */
            nvSetClearSurface(dwDstOffset, dwDstPitch, dwSurfColorFormat, dwRectColorFormat, dwFillColor);
            while (dwNumRects)
            {
                DWORD startx, starty;
                DWORD width, height;

                /*
                 * Get the next rectangle to clear.
                 */
                startx = lpRects->x1;
                starty = lpRects->y1;
                width  = lpRects->x2 - startx;
                height = lpRects->y2 - starty;

                /*
                 * Use the hardware to do the clear.
                 */
                nvClearSurfaceRect(startx, starty, width, height);

                /*
                 * Move to the next rectangle.
                 */
                lpRects++;
                dwNumRects--;
            }
        }
    }
    nvReset2DSurfaceFormat();
    pDriverData->TwoDRenderingOccurred   = TRUE;
    pDriverData->ThreeDRenderingOccurred = 0;
    //nvStartDmaBuffer();
    dbgFlushDDI(pCurrentContext);
    NV_D3D_GLOBAL_SAVE();
#ifdef  CACHE_FREECOUNT
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
    pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#endif  // CACHE_FREECOUNT

    pcd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}

#ifdef  NVD3D_DX6
/*
 * DX6 callback for clearing render target, z-buffer and stencil buffer.
 */
DWORD nvClear2
(
    LPD3DHAL_CLEAR2DATA pc2d
)
{
    DWORD       dwFlags;
    DWORD       dwFillColor;
    DWORD       dwFillStencil;
    DWORD       dwNumRects;
    DWORD       dwTextureOffset, dwTextureFormat, dwTextureFilter;
    DWORD       dwCombine0Alpha, dwCombine0Color;
    DWORD       dwCombine1Alpha, dwCombine1Color;
    DWORD       dwCombineFactor, dwBlend, dwFogColor;
    DWORD       dwControl0, dwControl1, dwControl2;
    D3DVALUE    dvFillDepth;
    D3DVALUE    dvX1, dvY1, dvX2, dvY2;
    LPD3DRECT   lpRects;
    D3DTLVERTEX tlvVertex;

    DPF_LEVEL(NVDBG_LEVEL_DDI_ENTRY, "nvClear2 - hContext = %08lx", pc2d->dwhContext);

    if (!pDriverData->NvDevFlatDma)
    {
        pc2d->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Need to make sure that an unfriendly mode switch didn't sneak and not cause
     * us to get re-enabled properly.
     */
    if (pDriverData->fFullScreenDosOccurred)
    {
        if (pDriverData->fFullScreenDosOccurred & 0x01)
            nvD3DReenable();
        else
        {
            pc2d->ddrval = DD_OK;
            return (DDHAL_DRIVER_HANDLED);
        }
    }
    if (pCurrentContext->dwContextReset)
        nvResetContext((PNVD3DTEXTURE)NULL);

    // this is to fix some stupid WHQL bug. basically, they care what's stored in fields
    // that technically don't even exist. we have to make sure that alpha gets cleared to
    // zero when there isn't one.
    dwFillColor = pc2d->dwFillColor;
    if ((pCurrentContext->dwSurfaceFormat == (DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH) | DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_X1R5G5B5_Z1R5G5B5)))
     || (pCurrentContext->dwSurfaceFormat == (DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH) | DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_X8R8G8B8_Z8R8G8B8)))) {
        dwFillColor &= 0x00ffffff;
    }

    /*
     * if we do not have stencil, use faster clears
     */
    if (!(pc2d->dwFlags & D3DCLEAR_STENCIL))
    {
        /*
         * 16-bit
         */
        if ((pCurrentContext->dwSurfaceFormat == (DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH) | DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_X1R5G5B5_Z1R5G5B5)))
         || (pCurrentContext->dwSurfaceFormat == (DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH) | DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_R5G6B5))))
        {
            D3DHAL_CLEARDATA cd;

            cd.dwhContext  = pc2d->dwhContext;
            cd.dwFlags     = pc2d->dwFlags & ~D3DCLEAR_STENCIL;
            cd.dwFillColor = dwFillColor;
            cd.dwFillDepth = (DWORD)(pc2d->dvFillDepth * 65535.0f); // convert to 16-bit number
            cd.lpRects     = pc2d->lpRects;
            cd.dwNumRects  = pc2d->dwNumRects;

            nvClear (&cd);

            pc2d->ddrval = cd.ddrval;
            return DDHAL_DRIVER_HANDLED;
        }
        else
        /*
         * 32-bit (assumed if not 16-bit)
         */
        {
            D3DHAL_CLEARDATA cd;
            __int64          fd;

            /*
             * Can only do a fast clear of zbuffer if this is not a not a stencil buffer.
             */
            if ((pc2d->dwFlags & D3DCLEAR_ZBUFFER)
             && ((pCurrentContext->lpLclZ)
              && (pCurrentContext->lpLclZ->dwFlags & DDRAWISURF_HASPIXELFORMAT)
              && (pCurrentContext->lpLclZ->lpGbl->ddpfSurface.dwFlags & DDPF_STENCILBUFFER)))
                goto labelSlowClear;

            /*
             * It's ok to do a fast clear of the z-buffer.
             */
            fd = (__int64)((double)pc2d->dvFillDepth * (double)4294967295.0); // convert to 32-bit number
            if (fd > 0xffffffff) fd = 0xffffffff;
            if (fd < 0)          fd = 0;

            cd.dwhContext  = pc2d->dwhContext;
            cd.dwFlags     = pc2d->dwFlags & ~D3DCLEAR_STENCIL;
            cd.dwFillColor = dwFillColor;
            cd.dwFillDepth = (DWORD)fd;
            cd.lpRects     = pc2d->lpRects;
            cd.dwNumRects  = pc2d->dwNumRects;

            nvClear (&cd);

            pc2d->ddrval = cd.ddrval;
            return DDHAL_DRIVER_HANDLED;
        }
    }

labelSlowClear:
    dwFlags       = pc2d->dwFlags;
    dvFillDepth   = pc2d->dvFillDepth;
    dwFillStencil = pc2d->dwFillStencil;
    lpRects       = pc2d->lpRects;
    dwNumRects    = pc2d->dwNumRects;

    if (dwNumRects)
    {
        NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
        nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#else  // CACHE_FREECOUNT
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetRenderTargetContexts + sizeSetRenderTarget));
#endif  // CACHE_FREECOUNT

        /*
         * Make sure the surface is set up correctly.
         */
        pCurrentContext->dwSurfaceAddr = 0;
        if (dbgFrontRender)
        {
#ifdef WINNT
            if (pCurrentContext->lpLcl->lpGbl->lPitch)
#endif
            pCurrentContext->dwSurfaceAddr            = VIDMEM_ADDR(pDriverData->CurrentVisibleSurfaceAddress);
            pCurrentContext->dwSurfaceOffset          = VIDMEM_OFFSET(pDriverData->CurrentVisibleSurfaceAddress);
            pCurrentContext->surfacePitch.wColorPitch = (WORD)dbgFrontRenderPitch;
        }
        else
        {
#ifdef WINNT
            if (pCurrentContext->lpLcl->lpGbl->lPitch)
#endif
            pCurrentContext->dwSurfaceAddr            = VIDMEM_ADDR(pCurrentContext->lpLcl->lpGbl->fpVidMem);
            pCurrentContext->dwSurfaceOffset          = VIDMEM_OFFSET(pCurrentContext->lpLcl->lpGbl->fpVidMem);
            pCurrentContext->surfacePitch.wColorPitch = (WORD)pCurrentContext->lpLcl->lpGbl->lPitch;
        }
        pCurrentContext->ZBufferAddr = 0;
        if (pCurrentContext->lpLclZ)
        {
            /*
             * I've seen it happen where the location of the z-buffer will actually
             * change without without the context being destroyed or the set render
             * target call happening. So it's neccessary to make sure that we have the
             * correct z-buffer address and offset here.
             */
#ifdef WINNT
            if (pCurrentContext->lpLcl->lpGbl->lPitch)
#endif
            pCurrentContext->ZBufferAddr             = VIDMEM_ADDR(pCurrentContext->lpLclZ->lpGbl->fpVidMem);
            pCurrentContext->ZBufferOffset           = VIDMEM_OFFSET(pCurrentContext->lpLclZ->lpGbl->fpVidMem);
            pCurrentContext->surfacePitch.wZetaPitch = (WORD)pCurrentContext->lpLclZ->lpGbl->lPitch;
        }

        /*
         * Call the routine to set the surface if neccessary.
         */
        pDriverData->lpLast3DSurfaceRendered = 0; // force setting of surface.
        nvSetD3DSurfaceState(pCurrentContext);

        /*
         * Determine the triangle setup for the fill being done.
         */
        dwTextureOffset = pCurrentContext->dwDefaultTextureOffset;
        dwTextureFormat = DRF_NUM(055, _FORMAT,          _CONTEXT_DMA,          pCurrentContext->dwDefaultTextureContextDma)
                        | DRF_NUM(055, _FORMAT,          _COLOR,                pCurrentContext->dwDefaultTextureColorFormat)
                        | DRF_NUM(055, _FORMAT,          _MIPMAP_LEVELS,        1)
                        | DRF_DEF(055, _FORMAT,          _BASE_SIZE_U,          _1)
                        | DRF_DEF(055, _FORMAT,          _BASE_SIZE_V,          _1)
                        | DRF_DEF(055, _FORMAT,          _TEXTUREADDRESSU,      _CLAMP)
                        | DRF_DEF(055, _FORMAT,          _WRAPU,                _FALSE)
                        | DRF_DEF(055, _FORMAT,          _TEXTUREADDRESSV,      _CLAMP)
                        | DRF_DEF(055, _FORMAT,          _WRAPV,                _FALSE)
                        | DRF_DEF(055, _FORMAT,          _ORIGIN_ZOH,           _CENTER)
                        | DRF_DEF(055, _FORMAT,          _ORIGIN_FOH,           _CENTER);
        dwTextureFilter = DRF_DEF(055, _FILTER,          _MIPMAP_DITHER_ENABLE, _TRUE)
                        | DRF_NUM(055, _FILTER,          _MIPMAPLODBIAS,        1)
                        | DRF_DEF(055, _FILTER,          _TEXTUREMIN,           _NEAREST)
                        | DRF_DEF(055, _FILTER,          _TEXTUREMAG,           _NEAREST);
        dwCombine0Alpha = DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_0,            _INVERSE)
                        | DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_0,           _ZERO)
                        | DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_1,            _INVERSE)
                        | DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_1,           _ZERO)
                        | DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_2,            _NORMAL)
                        | DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_2,           _ZERO)
                        | DRF_DEF(055, _COMBINE_0_ALPHA, _INVERSE_3,            _NORMAL)
                        | DRF_DEF(055, _COMBINE_0_ALPHA, _ARGUMENT_3,           _ZERO)
                        | DRF_DEF(055, _COMBINE_0_ALPHA, _OPERATION,            _ADD);
        dwCombine0Color = DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_0,            _INVERSE)
                        | DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_0,              _COLOR)
                        | DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_0,           _ZERO)
                        | DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_1,            _NORMAL)
                        | DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_1,              _COLOR)
                        | DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_1,           _DIFFUSE)
                        | DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_2,            _NORMAL)
                        | DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_2,              _COLOR)
                        | DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_2,           _ZERO)
                        | DRF_DEF(055, _COMBINE_0_COLOR, _INVERSE_3,            _NORMAL)
                        | DRF_DEF(055, _COMBINE_0_COLOR, _ALPHA_3,              _COLOR)
                        | DRF_DEF(055, _COMBINE_0_COLOR, _ARGUMENT_3,           _ZERO)
                        | DRF_DEF(055, _COMBINE_0_COLOR, _OPERATION,            _ADD);
        dwCombine1Alpha = DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_0,            _INVERSE)
                        | DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_0,           _ZERO)
                        | DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_1,            _NORMAL)
                        | DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_1,           _INPUT)
                        | DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_2,            _NORMAL)
                        | DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_2,           _ZERO)
                        | DRF_DEF(055, _COMBINE_1_ALPHA, _INVERSE_3,            _NORMAL)
                        | DRF_DEF(055, _COMBINE_1_ALPHA, _ARGUMENT_3,           _ZERO)
                        | DRF_DEF(055, _COMBINE_1_ALPHA, _OPERATION,            _ADD);
        dwCombine1Color = DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_0,            _NORMAL)
                        | DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_0,              _COLOR)
                        | DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_0,           _INPUT)
                        | DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_1,            _INVERSE)
                        | DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_1,              _COLOR)
                        | DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_1,           _ZERO)
                        | DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_2,            _NORMAL)
                        | DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_2,              _COLOR)
                        | DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_2,           _ZERO)
                        | DRF_DEF(055, _COMBINE_1_COLOR, _INVERSE_3,            _NORMAL)
                        | DRF_DEF(055, _COMBINE_1_COLOR, _ALPHA_3,              _COLOR)
                        | DRF_DEF(055, _COMBINE_1_COLOR, _ARGUMENT_3,           _ZERO)
                        | DRF_DEF(055, _COMBINE_1_COLOR, _OPERATION,            _ADD);
        dwCombineFactor = 0;
        dwBlend         = DRF_DEF(055, _BLEND,           _MASK_BIT,             _MSB)
                        | DRF_DEF(055, _BLEND,           _SHADEMODE,            _FLAT)
                        | DRF_DEF(055, _BLEND,           _TEXTUREPERSPECTIVE,   _TRUE)
                        | DRF_DEF(055, _BLEND,           _SPECULARENABLE,       _FALSE)
                        | DRF_DEF(055, _BLEND,           _FOGENABLE,            _FALSE)
                        | DRF_DEF(055, _BLEND,           _ALPHABLENDENABLE,     _TRUE);
        dwControl0      = DRF_NUM(055, _CONTROL0,        _ALPHAREF,             0)
                        | DRF_DEF(055, _CONTROL0,        _ALPHAFUNC,            _ALWAYS)
                        | DRF_DEF(055, _CONTROL0,        _ALPHATESTENABLE,      _FALSE)
                        | DRF_DEF(055, _CONTROL0,        _ORIGIN,               _CENTER)
                        | DRF_DEF(055, _CONTROL0,        _CULLMODE,             _NONE)
                        | DRF_DEF(055, _CONTROL0,        _DITHERENABLE,         _FALSE)
                        | DRF_DEF(055, _CONTROL0,        _ALPHA_WRITE_ENABLE,   _FALSE)
                        | DRF_DEF(055, _CONTROL0,        _RED_WRITE_ENABLE,     _TRUE)
                        | DRF_DEF(055, _CONTROL0,        _GREEN_WRITE_ENABLE,   _TRUE)
                        | DRF_DEF(055, _CONTROL0,        _BLUE_WRITE_ENABLE,    _TRUE)
                        | DRF_DEF(055, _CONTROL0,        _ZENABLE,              _TRUE)
                        | DRF_DEF(055, _CONTROL0,        _ZFUNC,                _ALWAYS);
        dwFogColor      = 0;

        /*
         * Fill in the static fields of the TLVertex data structure.
         */
        tlvVertex.sz       = dvFillDepth;
        tlvVertex.rhw      = 1.0f;
        tlvVertex.color    = dwFillColor;
        tlvVertex.specular = 0xFF000000;
        tlvVertex.tu       = 0.0f;
        tlvVertex.tv       = 0.0f;

        /*
         * Enable the buffers that are being cleared.
         */
        if ((dwFlags & D3DCLEAR_TARGET)
         && (pCurrentContext->dwSurfaceAddr))
        {
            dwBlend |= DRF_DEF(055, _BLEND,    _SRCBLEND,  _ONE)
                    |  DRF_DEF(055, _BLEND,    _DESTBLEND, _ZERO);
        }
        else
        {
            dwBlend |= DRF_DEF(055, _BLEND,    _SRCBLEND,  _ZERO)
                    |  DRF_DEF(055, _BLEND,    _DESTBLEND, _ONE);
        }

        if ((dwFlags & D3DCLEAR_ZBUFFER)
         && (pCurrentContext->ZBufferAddr))
        {
            dwControl0 |= DRF_DEF(055, _CONTROL0, _ZWRITEENABLE, _TRUE);
            /*
             * Scale the fill value if this is a w-buffer.
             */
            if (pCurrentContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_TRUE)
            {
                dwControl0 |= DRF_NUM(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, pDriverData->regZPerspectiveEnable)
                           |  DRF_NUM(055, _CONTROL0, _Z_FORMAT,             pDriverData->regZFormat);
            }
            else
            {
#ifdef  FLOAT_W
                // floating point w-buffer.
                dwControl0 |= DRF_DEF(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, _FALSE)
                           |  DRF_DEF(055, _CONTROL0, _Z_FORMAT,             _FLOAT);
                tlvVertex.sz  *= pCurrentContext->dvWFar;
#else
                // fixed point w-buffer.
                dwControl0 |= DRF_DEF(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, _FALSE)
                           |  DRF_DEF(055, _CONTROL0, _Z_FORMAT,             _FIXED);
#endif
            }
        }
        else
        {
            dwControl0 |= DRF_DEF(055, _CONTROL0, _ZWRITEENABLE,         _FALSE)
                       |  DRF_DEF(055, _CONTROL0, _Z_PERSPECTIVE_ENABLE, _FALSE)
                       |  DRF_DEF(055, _CONTROL0, _Z_FORMAT,             _FIXED);
        }

        if ((dwFlags & D3DCLEAR_STENCIL)
         && (pCurrentContext->ZBufferAddr))
        {
            dwControl0 |= DRF_DEF(055, _CONTROL0, _STENCIL_WRITE_ENABLE, _TRUE);

            dwControl1 = DRF_DEF(055, _CONTROL1, _STENCIL_TEST_ENABLE,  _TRUE)
                       | DRF_DEF(055, _CONTROL1, _STENCIL_FUNC,         _ALWAYS)
                       | DRF_NUM(055, _CONTROL1, _STENCIL_REF,          dwFillStencil)
                       | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_READ,    0xFF)
                       | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_WRITE,   0xFF);
            dwControl2 = DRF_DEF(055, _CONTROL2, _STENCIL_OP_FAIL,      _REPLACE)
                       | DRF_DEF(055, _CONTROL2, _STENCIL_OP_ZFAIL,     _REPLACE)
                       | DRF_DEF(055, _CONTROL2, _STENCIL_OP_ZPASS,     _REPLACE);
        }
        else
        {
            dwControl0 |= DRF_DEF(055, _CONTROL0, _STENCIL_WRITE_ENABLE, _FALSE);
            dwControl1  = DRF_DEF(055, _CONTROL1, _STENCIL_TEST_ENABLE,  _FALSE)
                        | DRF_DEF(055, _CONTROL1, _STENCIL_FUNC,         _NEVER)
                        | DRF_NUM(055, _CONTROL1, _STENCIL_REF,          0)
                        | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_READ,    0)
                        | DRF_NUM(055, _CONTROL1, _STENCIL_MASK_WRITE,   0);
            dwControl2  = DRF_DEF(055, _CONTROL2, _STENCIL_OP_FAIL,      _KEEP)
                        | DRF_DEF(055, _CONTROL2, _STENCIL_OP_ZFAIL,     _KEEP)
                        | DRF_DEF(055, _CONTROL2, _STENCIL_OP_ZPASS,     _KEEP);
        }

        /*
         * Send the triangle rendering state to the hardware.
         */
        while (nvFreeCount < (sizeSetObject + sizeDX6TriangleState))
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, (sizeSetObject + sizeDX6TriangleState));
        nvglSetObject(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, D3D_DX6_MULTI_TEXTURE_TRIANGLE);
        nvglDx6TriangleOffset(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, dwTextureOffset, 0);
        nvglDx6TriangleOffset(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, dwTextureOffset, 1);
        nvglDx6TriangleFormat(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, dwTextureFormat, 0);
        nvglDx6TriangleFormat(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, dwTextureFormat, 1);
        nvglDx6TriangleFilter(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, dwTextureFilter, 0);
        nvglDx6TriangleFilter(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, dwTextureFilter, 1);
        nvglDx6TriangleStageState(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR,
                                  dwCombine0Alpha, dwCombine0Color,
                                  dwCombine1Alpha, dwCombine1Color,
                                  dwCombineFactor, dwBlend,
                                  dwControl0,      dwControl1,
                                  dwControl2,      dwFogColor);

        /*
         * Now clear the buffers.
         */
        while (dwNumRects)
        {
            /*
             * Get the Upper Left and Lower Right fill coordinates as D3DVALUEs.
             */
            dvX1 = (D3DVALUE)lpRects->x1;
            dvY1 = (D3DVALUE)lpRects->y1;
            dvX2 = (D3DVALUE)lpRects->x2;
            dvY2 = (D3DVALUE)lpRects->y2;

            while (nvFreeCount < ((sizeDx6TriangleTLVertex * 4) + sizeDx6TriangleDrawPrimitive))
                nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, ((sizeDx6TriangleTLVertex * 4) + sizeDx6TriangleDrawPrimitive));


            /*
             * Send the fill rectangle down as two triangles and double kickoff.
             * Upper Left Vertex = i0
             */
            tlvVertex.sx = dvX1;
            tlvVertex.sy = dvY1;
            nvglDx6TriangleTLVertex(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, 0, tlvVertex);

            /*
             * Upper Right Vertex = i1
             */
            tlvVertex.sx = dvX2;
            tlvVertex.sy = dvY1;
            nvglDx6TriangleTLVertex(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, 1, tlvVertex);

            /*
             * Lower Right Vertex = i2
             */
            tlvVertex.sx = dvX2;
            tlvVertex.sy = dvY2;
            nvglDx6TriangleTLVertex(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, 2, tlvVertex);

            /*
             * Lower Left Vertex = i3
             */
            tlvVertex.sx = dvX1;
            tlvVertex.sy = dvY2;
            nvglDx6TriangleTLVertex(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, 3, tlvVertex);

            /*
             * Kick off two triangles (i0, i1, i2) and (i0, i2, i3)
             */
            nvglDx6TriangleDrawPrimitive(nvFifo, nvFreeCount, NV_DD_TRANSCOLOR, 3, 0x00320210);

            /*
             * Move to the next rectangle.
             */
            lpRects++;
            dwNumRects--;
        }
        /*
         * Kick off the fills.
         */
        //nvStartDmaBuffer();
        dbgFlushDDI(pCurrentContext);

        /*
         * Clear the dDrawSpareSubchannel6Object.  This will force the channel to
         * be reloaded or the next use of the DX6_MULTI_TEXTURE_TRIANGLE to reload
         * the state.
         */
        pDriverData->dDrawSpareSubchannel6Object = 0;
        NV_D3D_GLOBAL_SAVE();
#ifdef  CACHE_FREECOUNT
        pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
    }
    pc2d->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}
#endif  NVD3D_DX6

#endif  // NV4
