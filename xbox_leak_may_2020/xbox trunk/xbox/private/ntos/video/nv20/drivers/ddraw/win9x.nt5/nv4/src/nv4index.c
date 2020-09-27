#ifdef  NV4
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4INDX.C                                                         *
*   NV4 DX5 DrawOneIndexedPrimitive DDI routines.                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/98 - created                      *
*                                                                           *
\***************************************************************************/
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
/*
 * DirectX 5.0 DrawOneIndexedPrimitive entry point.
 */
void nvDX5IndexedTriangleSetup
(
    DWORD           dwPrimCount,
    LPWORD          lpwIndices,
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

    fnDX5IndexedTable[pCurrentContext->dwFunctionLookup](dwPrimCount, lpwIndices, dwStrides, lpVertices);
    return;
}
#endif //!NV_FASTLOOPS

#ifndef WINNT
DWORD nvDrawOneIndexedPrimitive
(
    LPD3DHAL_DRAWONEINDEXEDPRIMITIVEDATA    pdoipd
)
{
    DWORD   dwPrimCount;

#ifdef NV_NULL_DRIVER
    pdoipd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
#endif //NV_NULL_DRIVER

    DPF_LEVEL(NVDBG_LEVEL_DDI_ENTRY, "nvDrawOneIndexedPrimitive - PrimitiveType = %08lx, Index Count = %08lx", pdoipd->PrimitiveType, pdoipd->dwNumIndices);

    if (!pCurrentContext->lpLcl)
    {
        pdoipd->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }

    if (!pDriverData->NvDevFlatDma)
    {
        pdoipd->ddrval = DD_OK;
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
            pdoipd->ddrval = DD_OK;
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

    if (pdoipd->dwNumVertices && pdoipd->dwNumIndices)
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
         * Send down the surface information if neccessary.
         */
        nvSetD3DSurfaceState(pCurrentContext);
        switch (pdoipd->PrimitiveType)
        {
            case D3DPT_TRIANGLELIST:
                dwPrimCount =  pdoipd->dwNumIndices / 3;
                switch (pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE])
                {
                    case D3DFILL_SOLID:
#ifdef NV_FASTLOOPS
                        nvTriangleDispatch(dwPrimCount, (LPWORD)pdoipd->lpwIndices, LIST_STRIDES, (LPBYTE)pdoipd->lpvVertices);
#else
                        fnDX5IndexedTable[pCurrentContext->dwFunctionLookup](dwPrimCount, (LPWORD)pdoipd->lpwIndices, LIST_STRIDES, (LPD3DTLVERTEX)pdoipd->lpvVertices);
#endif
                        break;
                    case D3DFILL_WIREFRAME:
                        nvIndexedWireframeTriangle(dwPrimCount, (LPWORD)pdoipd->lpwIndices, LIST_STRIDES, (LPD3DTLVERTEX)pdoipd->lpvVertices);
                        break;
                    case D3DFILL_POINT:
                        nvIndexedPointTriangle(dwPrimCount, (LPWORD)pdoipd->lpwIndices, LIST_STRIDES, (LPD3DTLVERTEX)pdoipd->lpvVertices);
                        break;
                }
                break;
            case D3DPT_TRIANGLESTRIP:
                dwPrimCount = pdoipd->dwNumIndices - 2;
                switch (pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE])
                {
                    case D3DFILL_SOLID:
#ifdef NV_FASTLOOPS
                        nvTriangleDispatch(dwPrimCount, (LPWORD)pdoipd->lpwIndices, STRIP_STRIDES, (LPBYTE)pdoipd->lpvVertices);
#else
                        fnDX5IndexedTable[pCurrentContext->dwFunctionLookup](dwPrimCount, (LPWORD)pdoipd->lpwIndices, STRIP_STRIDES, (LPD3DTLVERTEX)pdoipd->lpvVertices);
#endif
                        break;
                    case D3DFILL_WIREFRAME:
                        nvIndexedWireframeTriangle(dwPrimCount, (LPWORD)pdoipd->lpwIndices, STRIP_STRIDES, (LPD3DTLVERTEX)pdoipd->lpvVertices);
                        break;
                    case D3DFILL_POINT:
                        nvIndexedPointTriangle(dwPrimCount, (LPWORD)pdoipd->lpwIndices, STRIP_STRIDES, (LPD3DTLVERTEX)pdoipd->lpvVertices);
                        break;
                }
                break;
            case D3DPT_TRIANGLEFAN:
                dwPrimCount = pdoipd->dwNumIndices - 2;
                switch (pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE])
                {
                    case D3DFILL_SOLID:
#ifdef NV_FASTLOOPS
                        nvTriangleDispatch(dwPrimCount, (LPWORD)pdoipd->lpwIndices, FAN_STRIDES, (LPBYTE)pdoipd->lpvVertices);
#else
                        fnDX5IndexedTable[pCurrentContext->dwFunctionLookup](dwPrimCount, (LPWORD)pdoipd->lpwIndices, FAN_STRIDES, (LPD3DTLVERTEX)pdoipd->lpvVertices);
#endif
                        break;
                    case D3DFILL_WIREFRAME:
                        nvIndexedWireframeTriangle(dwPrimCount, (LPWORD)pdoipd->lpwIndices, FAN_STRIDES, (LPD3DTLVERTEX)pdoipd->lpvVertices);
                        break;
                    case D3DFILL_POINT:
                        nvIndexedPointTriangle(dwPrimCount, (LPWORD)pdoipd->lpwIndices, FAN_STRIDES, (LPD3DTLVERTEX)pdoipd->lpvVertices);
                        break;
                }
                break;
            case D3DPT_LINELIST:
                /*
                 * Calculate the number of lines to draw.
                 */
                dwPrimCount = pdoipd->dwNumIndices / 2;
                nvIndexedLine(dwPrimCount, (LPWORD)pdoipd->lpwIndices, 2, (LPD3DTLVERTEX)pdoipd->lpvVertices);
                break;
            case D3DPT_LINESTRIP:
                dwPrimCount = pdoipd->dwNumIndices - 1;
                nvIndexedLine(dwPrimCount, (LPWORD)pdoipd->lpwIndices, 1, (LPD3DTLVERTEX)pdoipd->lpvVertices);
                break;
            case D3DPT_POINTLIST:
                /*
                 * Indexed points kind of suck under the current scheme.
                 */


                break;
        }

        /*
         * Tell DDRAW that thee global clip state has changed.
         * Probably don't need this, but I'd rather be safe.
         */
        pDriverData->ddClipUpdate = TRUE;
    }

    /*
     * Update the put offset.
     */
    nvStartDmaBuffer (TRUE);
    pDriverData->TwoDRenderingOccurred   = 0;
    pDriverData->ThreeDRenderingOccurred = TRUE;

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

    pdoipd->ddrval = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}
#endif // #ifndef WINNT
#endif  // NV4