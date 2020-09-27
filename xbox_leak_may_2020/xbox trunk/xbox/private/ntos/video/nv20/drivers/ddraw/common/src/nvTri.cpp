/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvTri.cpp                                                         *
*   NV4 Old Execute Buffer Style RenderPrimitive DDI routines.              *
*   NOTE: Uses indexed primitive rendering routines in NV4INDEX.C           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/98 - created                      *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

PFNRENDERPRIMITIVEOP pfnRenderPrimitiveOperation[] =
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

//---------------------------------------------------------------------------

DWORD nvRenderNoOp
(
    NV_INNERLOOP_ARGLIST
)
{
    DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvRenderNoOp - Unhandled instruction opcode");
    return (DD_OK);
}

//---------------------------------------------------------------------------

DWORD nvRenderTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    /*
     * Call the appropriate wrapper function.
     */
    switch (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE])
    {
        case D3DFILL_SOLID:
            nvTriangleDispatch (NV_INNERLOOP_ARGS);
            break;
        case D3DFILL_WIREFRAME:
            nvIndexedWireframeTriangle (NV_INNERLOOP_ARGS);
            break;
        case D3DFILL_POINT:
            nvIndexedPointTriangle (NV_INNERLOOP_ARGS);
            break;
    }
    return (DD_OK);
}

//---------------------------------------------------------------------------

#ifndef WINNT  // these calls are obsolete on NT (DX7)

// Old style execute buffer entry point.
// This callback is invoked when a primitive is to be rendered.
// All the data is known to be clipped.

DWORD __stdcall nvRenderPrimitive
(
    LPD3DHAL_RENDERPRIMITIVEDATA prd
)
{
    LPD3DINSTRUCTION    lpIns;
    LPBYTE              lpData;
    DWORD               dwPrimCount;
    LPWORD              lpPrim;
    LPBYTE              lpVertices;

#if (NVARCH >= 0x10)
    if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN)) {
        DPF ("ugh. need to implement nvRenderPrimitive for celsius / kelvin");
        dbgD3DError();
        prd->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }
#endif

#ifdef NV_NULL_DRIVER
    prd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
#endif //NV_NULL_DRIVER

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, prd);

    // Get pointer to global driver. data structure.
    nvSetDriverDataPtrFromContext(pContext);

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED)
    {
        dbgD3DError();
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvRenderPrimitive - Bad Context");
        prd->ddrval = D3DHAL_CONTEXT_BAD;
        NvReleaseSemaphore(pDriverData);
        return (DDHAL_DRIVER_HANDLED);
    }

    // Check z-visibility test.
    if (pContext->dwRenderState[D3DRENDERSTATE_ZVISIBLE])
    {
        // If you don't implement Z visibility testing, just do this.
        prd->dwStatus &= ~D3DSTATUS_ZNOTVISIBLE;
        prd->ddrval    = DD_OK;
        NvReleaseSemaphore(pDriverData);
        return (DDHAL_DRIVER_HANDLED);
    }

    // Need to make sure that an unfriendly mode switch didn't sneak and not cause
    // us to get re-enabled properly.
    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvD3DRecover();
        }
        else {
            prd->ddrval = DD_OK;
            NvReleaseSemaphore(pDriverData);
            return (DDHAL_DRIVER_HANDLED);
        }
    }

    if ((pContext->bStateChange || pDriverData->TwoDRenderingOccurred)
     || (pDriverData->dDrawSpareSubchannelObject != D3D_DX5_TEXTURED_TRIANGLE))
        NV_FORCE_TRI_SETUP(pContext);

    pContext->pCurrentVShader->create (pContext, D3DFVF_TLVERTEX, CVertexShader::getHandleFromFvf(D3DFVF_TLVERTEX));

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TL_MODE |
                                      CELSIUS_DIRTY_FVF |
                                      CELSIUS_DIRTY_LIGHTS;
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF |
                                      KELVIN_DIRTY_LIGHTS;
#endif  // nv10

    // If the context has changed since the last render call, switch it now.
    nvSetD3DSurfaceState(pContext);

    // Prepare to parse the instructions.
    prd->ddrval = DD_OK;
    lpData      = (LPBYTE)(VIDMEM_ADDR(((LPDDRAWI_DDRAWSURFACE_INT)prd->lpExeBuf)->lpLcl->lpGbl->fpVidMem));
    lpIns       = &prd->diInstruction;
    dwPrimCount = lpIns->wCount;
    lpPrim      = (LPWORD)(lpData + prd->dwOffset);
    lpVertices  = (LPBYTE)(VIDMEM_ADDR((LPDDRAWI_DDRAWSURFACE_INT)prd->lpTLBuf)->lpLcl->lpGbl->fpVidMem + prd->dwTLOffset);

    switch (lpIns->bOpcode) {
        case 1:
            pContext->dp2.dwDP2Prim = D3DDP2OP_POINTS;
            break;
        case 2:
            pContext->dp2.dwDP2Prim = D3DDP2OP_INDEXEDLINELIST;
            break;
        case 3:
            pContext->dp2.dwDP2Prim = D3DDP2OP_INDEXEDTRIANGLELIST;
            break;
        default:
            DPF ("unsupported primitive type in nvRenderPrimitive");
            dbgD3DError();
            break;
    }

    pContext->dp2.dwIndices = (DWORD)(lpPrim);

    pfnRenderPrimitiveOperation[lpIns->bOpcode] (pContext, (WORD)dwPrimCount);

    // Update the put offset.
    nvPusherStart (FALSE);

    // The DEBUG driver has a flag that can be set to force a flush after every triangle
    // rendering call.  This is helpful for debugging when dbgFrontRender is enabled.
    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    // Tell DDRAW that thee global clip state has changed.
    // Probably don't need this, but I'd rather be safe.
    pDriverData->ddClipUpdate = TRUE;

    // Show that 3D rendering has occurred.
    pDriverData->TwoDRenderingOccurred   = 0;
    pDriverData->ThreeDRenderingOccurred = TRUE;

    // Return successfully.
    NvReleaseSemaphore(pDriverData);
    return (DDHAL_DRIVER_HANDLED);
}

#endif // !WINNT

#endif  // NVARCH >= 0x04

