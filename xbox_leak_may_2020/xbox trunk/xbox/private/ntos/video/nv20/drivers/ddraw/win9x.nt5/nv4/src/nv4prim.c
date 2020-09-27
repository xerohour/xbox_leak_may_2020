#ifdef  NV4
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4PRIM.C                                                         *
*   NV4 DX5 DrawPrimitives and DrawOnePrimitive DDI routines.               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/98 - created                      *
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
#include "nv3ddbg.h"
#include "nv4vxmac.h"

#ifndef NV_FASTLOOPS
void nvDX5TriangleSetup
(
    WORD            wPrimCount,
    DWORD           dwStrides,
    LPD3DTLVERTEX   lpVertices
)
{
    /*
     * Send the context state down to the hardware.
     */
    if (pCurrentContext->dwStateChange)
        nvSetHardwareState();
    nvSetDx5TriangleState(pCurrentContext);

    /*
     * Now calculate the appropriate rendering routine and call it.
     *
     * Determine the fog table mode based on if fog is enabled and the
     * selected fog table mode.  A computed value of 0 = vertex or no fog,
     * non-zero values indicate the fog table mode.
     */
    CALC_FUNCTION_INDEX (pCurrentContext);

    fnDX5Table[pCurrentContext->dwFunctionLookup](wPrimCount, dwStrides, lpVertices);
    return;
}
#endif //!NV_FASTLOOPS

#ifndef WINNT
void nvDrawPrimitive
(
    WORD            wPrimType,
    WORD            wNumVertices,
    LPBYTE          lpVertexData

)
{
    WORD            wPrimCount;
    LPD3DTLVERTEX   lpVertices;

    lpVertices = (LPD3DTLVERTEX)lpVertexData;
    switch (wPrimType)
    {
        case D3DPT_TRIANGLELIST:
            wPrimCount = wNumVertices / 3;
            switch (pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE])
            {
                case D3DFILL_SOLID:
#ifdef NV_FASTLOOPS
                    nvTriangleDispatch(wPrimCount, NULL, LIST_STRIDES, (LPBYTE)lpVertices);
#else
                    fnDX5Table[pCurrentContext->dwFunctionLookup](wPrimCount, LIST_STRIDES, lpVertices);
#endif
                    break;
                case D3DFILL_WIREFRAME:
                    nvDrawWireframeTriangle(wPrimCount, LIST_STRIDES, lpVertices);
                    break;
                case D3DFILL_POINT:
                    nvDrawPointTriangle(wPrimCount, LIST_STRIDES, lpVertices);
                    break;
            }
            break;
        case D3DPT_TRIANGLESTRIP:
            wPrimCount = wNumVertices - 2;
            switch (pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE])
            {
                case D3DFILL_SOLID:
#ifdef NV_FASTLOOPS
                    nvTriangleDispatch(wPrimCount, NULL, STRIP_STRIDES, (LPBYTE)lpVertices);
#else
                    fnDX5Table[pCurrentContext->dwFunctionLookup](wPrimCount, STRIP_STRIDES, lpVertices);
#endif
                    break;
                case D3DFILL_WIREFRAME:
                    nvDrawWireframeTriangle(wPrimCount, STRIP_STRIDES, lpVertices);
                    break;
                case D3DFILL_POINT:
                    nvDrawWireframeTriangle(wPrimCount, STRIP_STRIDES, lpVertices);
                    break;
            }
            break;
        case D3DPT_TRIANGLEFAN:
            if ((wPrimCount = wNumVertices - 2) == 0)
                break;
            switch (pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE])
            {
                case D3DFILL_SOLID:
#ifdef NV_FASTLOOPS
                    nvTriangleDispatch(wPrimCount, NULL, FAN_STRIDES, (LPBYTE)lpVertices);
#else
                    fnDX5Table[pCurrentContext->dwFunctionLookup](wPrimCount, FAN_STRIDES, lpVertices);
#endif
                    break;
                case D3DFILL_WIREFRAME:
                    nvDrawWireframeTriangle(wPrimCount, FAN_STRIDES, lpVertices);
                    break;
                case D3DFILL_POINT:
                    nvDrawWireframeTriangle(wPrimCount, FAN_STRIDES, lpVertices);
                    break;
            }
            break;
        case D3DPT_LINELIST:
            wPrimCount = wNumVertices / 2;
            nvDrawLine((DWORD)wPrimCount, 2, lpVertices);
            break;
        case D3DPT_LINESTRIP:
            wPrimCount = wNumVertices - 1;
            nvDrawLine((DWORD)wPrimCount, 1, lpVertices);
            break;
        case D3DPT_POINTLIST:
//            nvDrawPointList((DWORD)((LPD3DHAL_DP2POINTS)lpPrim)->wCount, (LPD3DTLVERTEX)&lpVertices[((LPD3DHAL_DP2POINTS)lpPrim)->wVStart]);
            break;
    }
    /*
     * Update the put offset.
     */
    nvStartDmaBuffer (TRUE);
    pDriverData->TwoDRenderingOccurred   = 0;
    pDriverData->ThreeDRenderingOccurred = TRUE;
    return;
}

/*
 * DirectX 5.0 DrawOnePrimitive entry point.
 */
DWORD nvDrawOnePrimitive
(
    LPD3DHAL_DRAWONEPRIMITIVEDATA   pdopd
)
{
#ifdef NV_NULL_DRIVER
    pdopd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
#endif //NV_NULL_DRIVER

    DPF_LEVEL(NVDBG_LEVEL_DDI_ENTRY, "nvDrawOnePrimitive - PrimitiveType = %08lx, Vertex Count = %08lx", pdopd->PrimitiveType, pdopd->dwNumVertices);

    if (!pCurrentContext->lpLcl)
    {
        pdopd->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }

    if (!pDriverData->NvDevFlatDma)
    {
        pdopd->ddrval = DD_OK;
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
            pdopd->ddrval = DD_OK;
            return (DDHAL_DRIVER_HANDLED);
        }
    }
    if (pCurrentContext->dwContextReset)
        nvResetContext((PNVD3DTEXTURE)NULL);

    /*
     * prep fvf data
     */
    {
        DWORD dwDummy1,dwDummy2;
        CHECK_FVF_DATA_AND_LOAD_TL (fvfData,D3DFVF_TLVERTEX,dwDummy1,dwDummy2);
    }

    /*
     * Just pass off this call to the DrawPrimitive helper routine.
     */
    if (pdopd->dwNumVertices)
    {
        /*
         * Setup frequently accessed globals.
         */
        NV_D3D_GLOBAL_SETUP();
        NV_AA_SEMANTICS_CHECK(pCurrentContext);
        if ((pCurrentContext->dwStateChange || pDriverData->TwoDRenderingOccurred)
         || (pDriverData->dDrawSpareSubchannelObject != D3D_DX5_TEXTURED_TRIANGLE))
            NV_FORCE_TRI_SETUP(pCurrentContext);

        /*
         * Always read the current free count on entry.
         */
#ifdef  CACHE_FREECOUNT
        nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetRenderTargetContexts + sizeSetRenderTarget));
#endif  // CACHE_FREECOUNT

        /*
         * If the surface has changed since the last render call, switch it now.
         */
        nvSetD3DSurfaceState(pCurrentContext);

        nvDrawPrimitive((WORD)pdopd->PrimitiveType,
                        (WORD)pdopd->dwNumVertices,
                        (LPBYTE)pdopd->lpvVertices);
        NV_D3D_GLOBAL_SAVE();
#ifdef  CACHE_FREECOUNT
        pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
    }

    /*
     * Return successfully.
     */
    pdopd->ddrval = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

/*
 * DirectX 5.0 DrawPrimitives entry point.
 */
DWORD nvDrawPrimitives
(
    LPD3DHAL_DRAWPRIMITIVESDATA pdpd
)
{
    WORD                        wStateCount;
    LPBYTE                      lpData;
    LPDWORD                     lpStateChanges;
    LPD3DHAL_DRAWPRIMCOUNTS     lpDrawPrimCounts;

#ifdef NV_NULL_DRIVER
    pdpd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
#endif //NV_NULL_DRIVER

    DPF_LEVEL(NVDBG_LEVEL_DDI_ENTRY, "nvDrawPrimitives - hContext = %08lx", pdpd->dwhContext);

    /*
     * assert valid
     */
    if (!pCurrentContext->lpLcl)
    {
        pdpd->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }

    if (!pDriverData->NvDevFlatDma)
    {
        pdpd->ddrval = DD_OK;
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
            pdpd->ddrval = DD_OK;
            return (DDHAL_DRIVER_HANDLED);
        }
    }
    if (pCurrentContext->dwContextReset)
        nvResetContext((PNVD3DTEXTURE)NULL);

    /*
     * Setup frequently accessed globals.
     */
    NV_D3D_GLOBAL_SETUP();
    if ((pCurrentContext->dwStateChange || pDriverData->TwoDRenderingOccurred)
     || (pDriverData->dDrawSpareSubchannelObject != D3D_DX5_TEXTURED_TRIANGLE))
        NV_FORCE_TRI_SETUP(pCurrentContext);

    /*
     * prep fvf data
     */
    {
        DWORD dwDummy1,dwDummy2;
        CHECK_FVF_DATA_AND_LOAD_TL (fvfData,D3DFVF_TLVERTEX,dwDummy1,dwDummy2);
    }

    /*
     * Always read the current free count on entry.
     */
#ifdef  CACHE_FREECOUNT
    nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetRenderTargetContexts + sizeSetRenderTarget));
#endif  // CACHE_FREECOUNT

    /*
     * If the surface has changed since the last render call, switch it now.
     */
    nvSetD3DSurfaceState(pCurrentContext);

    /*
     * Get the pointer to the primitive data.
     */
    lpData            = (LPBYTE)pdpd->lpvData;
    lpDrawPrimCounts  = (LPD3DHAL_DRAWPRIMCOUNTS)lpData;

    /*
     * Step through callback data until there are no more vertices left
     * in the structure.
     */
    do
    {
        /*
         * Get the next draw primitive data structure.
         */
        lpDrawPrimCounts  = (LPD3DHAL_DRAWPRIMCOUNTS)lpData;
        lpData           += sizeof(D3DHAL_DRAWPRIMCOUNTS);

        if (!lpDrawPrimCounts)
        {
            dbgD3DError();
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvDrawPrimitives - Invalid lpDrawPrimCounts!!");
        }

        /*
         * Get the pointer to the first set of state change data.
         * If the wNumStateChanges value is 0, then there are no
         * state changes for this set of vertices.  This can happen
         * at any time, including for the first set of vertices, which
         * means that the current state should be used, or even for
         * two or more blocks of vertices in a row.
         */
        lpStateChanges    = (LPDWORD)lpData;
        if (!lpStateChanges)
        {
            dbgD3DError();
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvDrawPrimitives - Invalid lpDrawPrimCounts!!");
        }
        if (wStateCount = lpDrawPrimCounts->wNumStateChanges)
        {
            while (wStateCount--)
            {
                /*
                 * Process the state changes for the next set of vertices.
                 */
                nvSetContextState(lpStateChanges[0], lpStateChanges[1], &pdpd->ddrval);
                if (pdpd->ddrval != DD_OK)
                    return (DDHAL_DRIVER_HANDLED);
                lpStateChanges += 2;
            }
            /*
             * If the state has changed force a hardware state load.
             */
            if (pCurrentContext->dwStateChange)
                NV_FORCE_TRI_SETUP(pCurrentContext);
        }

        /*
         * Move the pointer to the first block of vertices to be rendered.
         */
        lpData += ((lpDrawPrimCounts->wNumStateChanges * sizeof(DWORD) * 2) + 31);
        lpData = (LPBYTE)((DWORD)lpData & ~31);

        /*
         * Render the vertices.
         */
        if (lpDrawPrimCounts->wNumVertices)
        {
            WORD wPrimitiveType = lpDrawPrimCounts->wPrimitiveType;

            nvDrawPrimitive(wPrimitiveType,
                            lpDrawPrimCounts->wNumVertices,
                            lpData);

            lpData += (lpDrawPrimCounts->wNumVertices * sizeof(D3DTLVERTEX));
        }
    } while (lpDrawPrimCounts->wNumVertices);

    /*
     * Return successfully.
     */
    NV_D3D_GLOBAL_SAVE();
#ifdef  CACHE_FREECOUNT
    pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
    pdpd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}
#endif // #ifndef WINNT
#endif  // NV4