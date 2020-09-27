#ifdef  NV4
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4TRI.C                                                          *
*   NV4 Old Execute Buffer Style RenderPrimitive DDI routines.              *
*   NOTE: Uses indexed primitive rendering routines in NV4INDEX.C           *
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
#include "nv3ddbg.h"
#include "nv4vxmac.h"


PFNRENDERPRIMITIVEOP    pfnRenderPrimitiveOperation[] =
{
    nvRenderNoOp,           // Op codes are 1 based, so 0 is a no op.
    nvRenderPoint,
    nvRenderLine,
    nvRenderTriangle,
    nvRenderNoOp,           // 4  - not supported.
    nvRenderNoOp,           // 5  - not supported.
    nvRenderNoOp,           // 6  - not supported.
    nvRenderNoOp,           // 7  - not supported.
    nvRenderNoOp,           // 8  - not supported.
    nvRenderNoOp,           // 9  - not supported.
    nvRenderNoOp,           // 10 - not supported.
    nvRenderNoOp,           // 11 - not supported.
    nvRenderNoOp,           // 12 - not supported.
    nvRenderNoOp,           // 13 - not supported.
    nvRenderNoOp            // 14 - not supported.
};

DWORD nvRenderNoOp
(
    DWORD           dwPrimCount,
    LPBYTE          lpPrim,
    LPD3DTLVERTEX   lpVertices
)
{
    DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvRenderNoOp - Unhandled instruction opcode");
    return (DD_OK);
}

DWORD nvRenderTriangle
(
    DWORD           dwPrimCount,
    LPBYTE          lpPrim,
    LPD3DTLVERTEX   lpVertices
)
{
    /*
     * Call the appropriate wrapper function.
     */
    switch (pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE])
    {
        case D3DFILL_SOLID:
#ifdef NV_FASTLOOPS
            nvTriangleDispatch(dwPrimCount,(LPWORD)lpPrim,LEGACY_STRIDES,(LPBYTE)lpVertices);
#else
            fnDX5IndexedTable[pCurrentContext->dwFunctionLookup](dwPrimCount, (LPWORD)lpPrim, LEGACY_STRIDES, lpVertices);
#endif
            break;
        case D3DFILL_WIREFRAME:
            nvIndexedWireframeTriangle(dwPrimCount, (LPWORD)lpPrim, LEGACY_STRIDES, lpVertices);
            break;
        case D3DFILL_POINT:
            nvIndexedPointTriangle(dwPrimCount, (LPWORD)lpPrim, LEGACY_STRIDES, lpVertices);
            break;
    }
    return (DD_OK);
}

#ifndef WINNT
/*
 * Old style execute buffer entry point.
 */
DWORD __stdcall nvRenderPrimitive
(
    LPD3DHAL_RENDERPRIMITIVEDATA    prd
)
{
    LPD3DINSTRUCTION    lpIns;
    LPBYTE              lpData;
    DWORD               dwPrimCount;
    LPBYTE              lpPrim;
    LPD3DTLVERTEX       lpVertices;

#ifdef NV_NULL_DRIVER
    prd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
#endif //NV_NULL_DRIVER

    /*
     * NOTES:
     *
     * This callback is invoked when a primitive is to be rendered.
     * All the data is known to be clipped.
     *
     * Get the pointer to the context this texture is associated with.
     */
    DPF_LEVEL(NVDBG_LEVEL_DDI_ENTRY, "nvRenderPrimitive - hContext = %08lx", prd->dwhContext);
    if (!(pCurrentContext = (PNVD3DCONTEXT)prd->dwhContext))
    {
        dbgD3DError();
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvRenderPrimitive - Bad Context");
        prd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Get pointer to global driver. data structure.
     */
    NvSetDriverDataPtrFromContext(pCurrentContext);
    if (!pDriverData->NvDevFlatDma)
    {
        dbgD3DError();
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvRenderPrimitive - Bad Context");
        prd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Check z-visibility test.
     */
    if (pCurrentContext->dwRenderState[D3DRENDERSTATE_ZVISIBLE])
    {
        /*
         * If you don't implement Z visibility testing, just do this.
         */
        prd->dwStatus &= ~D3DSTATUS_ZNOTVISIBLE;
        prd->ddrval    = DD_OK;
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
            prd->ddrval = DD_OK;
            return (DDHAL_DRIVER_HANDLED);
        }
    }
    if (pCurrentContext->dwContextReset)
        nvResetContext((PNVD3DTEXTURE)NULL);

    /*
     * Setup frequently accessed globals.
     */
    NV_D3D_GLOBAL_SETUP();
    NV_AA_SEMANTICS_CHECK(pCurrentContext);
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
    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeDx5TriangleTLVertex);
#endif  // CACHE_FREECOUNT

    /*
     * If the context has changed since the last render call, switch it now.
     */
    nvSetD3DSurfaceState(pCurrentContext);

    /*
     * Prepare to parse the instructions.
     */
    prd->ddrval = DD_OK;
    lpData      = (LPBYTE)(((LPDDRAWI_DDRAWSURFACE_INT)prd->lpExeBuf)->lpLcl->lpGbl->fpVidMem);
    lpIns       = &prd->diInstruction;
    dwPrimCount = lpIns->wCount;
    lpPrim      = (LPBYTE)(lpData + prd->dwOffset);
    lpVertices  = (LPD3DTLVERTEX)((LPBYTE)((LPDDRAWI_DDRAWSURFACE_INT)prd->lpTLBuf)->lpLcl->lpGbl->fpVidMem + prd->dwTLOffset);
    pfnRenderPrimitiveOperation[lpIns->bOpcode](dwPrimCount, lpPrim, lpVertices);

    /*
     * Update the put offset.
     */
    nvStartDmaBuffer (TRUE);

    /*
     * The DEBUG driver has a flag that can be set to force a flush after every triangle
     * rendering call.  This is helpful for debugging when dbgFrontRender is enabled.
     */
    dbgFlushDmaBuffers(pCurrentContext);

    /*
     * Tell DDRAW that thee global clip state has changed.
     * Probably don't need this, but I'd rather be safe.
     */
    pDriverData->ddClipUpdate = TRUE;
#ifdef  CACHE_FREECOUNT
    pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT

    /*
     * Show that 3D rendering has occurred.
     */
    pDriverData->TwoDRenderingOccurred   = 0;
    pDriverData->ThreeDRenderingOccurred = TRUE;

    /*
     * Return successfully.
     */
    NV_D3D_GLOBAL_SAVE();
    return (DDHAL_DRIVER_HANDLED);
}
#endif // #ifndef WINNT
#endif  NV4
