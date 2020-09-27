/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvPrim.cpp                                                        *
*   NV4 DX5 DrawPrimitives and DrawOnePrimitive DDI routines.               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/98 - created                      *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"


#if (NVARCH >= 0x04)

#ifndef WINNT

void nvDrawPrimitive
(
    PNVD3DCONTEXT pContext,
    WORD          wPrimType,
    WORD          wNumVertices,
    LPBYTE        lpVertexData
)
{
    WORD            wPrimCount;
    CVertexBuffer  *pVertexBuffer;

    pContext->dp2.dwVertexBufferOffset = 0;
    pContext->dp2.dwVStart = 0;
    pContext->dp2.dwTotalOffset = 0;

    DDSTARTTICK(PRIM_DRAWPRIMITIVE);

#if (NVARCH >= 0x010)
    pContext->pCurrentVShader->create (pContext, D3DFVF_TLVERTEX, CVertexShader::getHandleFromFvf(D3DFVF_TLVERTEX));
    pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_TL_MODE |
                                       CELSIUS_DIRTY_FVF |
                                       CELSIUS_DIRTY_LIGHTS);
    pContext->hwState.dwDirtyFlags |= (KELVIN_DIRTY_FVF |
                                       KELVIN_DIRTY_LIGHTS);
#endif  // nv10

    // cast plain vertex data into a vertex buffer
    pVertexBuffer = getDC()->defaultSysVB.getVB();
    pVertexBuffer->own ((DWORD)(lpVertexData), wNumVertices*sizeof(D3DTLVERTEX), CSimpleSurface::HEAP_SYS);
    // cast vertex buffer into DX8-style streams
    for (int i=0; i<NV_CAPS_MAX_STREAMS; i++) {
        pContext->ppDX8Streams[i] = pVertexBuffer;
    }

    switch (wPrimType) {

        case D3DPT_TRIANGLELIST:

            pContext->dp2.dwDP2Prim = D3DDP2OP_TRIANGLELIST;
            pContext->dp2.dwIndices = NULL;

            wPrimCount = wNumVertices / 3;

#if (NVARCH >= 0x020)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                DPF ("ugh. got a legacy call to nvDrawPrimitive on an nv20.");
                DPF ("this is totally untested, may very well not work.");
                dbgD3DError();
                nvKelvinDispatchNonIndexedPrimitive (pContext, wPrimCount);
            }
            else
#endif  // (NVARCH >= 0x020)
#if (NVARCH >= 0x010)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                DPF ("ugh. got a legacy call to nvDrawPrimitive on an nv10.");
                DPF ("this is totally untested, may very well not work.");
                dbgD3DError();
                nvCelsiusDispatchNonIndexedPrimitive (pContext, wPrimCount);
            }
            else
#endif  // NVARCH >= 0x010
            {
                switch (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE])
                {
                    case D3DFILL_SOLID:
                        DDSTARTTICK(PRIM_SOLIDNONINDEXEDTRI);
                        DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wNumVertices);
                        nvTriangleDispatch (pContext, wPrimCount);
                        DDENDTICK(PRIM_SOLIDNONINDEXEDTRI);
                        break;
                    case D3DFILL_WIREFRAME:
                        DDSTARTTICK(PRIM_WIREFRAMENONINDEXEDTRI);
                        DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wNumVertices);
                        nvDrawWireframeTriangle (pContext, wPrimCount);
                        DDENDTICK(PRIM_WIREFRAMENONINDEXEDTRI);
                        break;
                    case D3DFILL_POINT:
                        DDSTARTTICK(PRIM_POINTNONINDEXEDTRI);
                        DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wNumVertices);
                        nvDrawPointTriangle (pContext, wPrimCount);
                        DDENDTICK(PRIM_POINTNONINDEXEDTRI);
                        break;
                }
            }

            break;

        case D3DPT_TRIANGLESTRIP:

            pContext->dp2.dwDP2Prim = D3DDP2OP_TRIANGLESTRIP;
            pContext->dp2.dwIndices = NULL;

            wPrimCount = wNumVertices - 2;

#if (NVARCH >= 0x020)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                DPF ("ugh. got a legacy call to nvDrawPrimitive on an nv20.");
                DPF ("this is totally untested, may very well not work.");
                dbgD3DError();
                nvKelvinDispatchNonIndexedPrimitive (pContext, wPrimCount);
            }
            else
#endif  // (NVARCH >= 0x020)
#if (NVARCH >= 0x010)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                DPF ("ugh. got a legacy call to nvDrawPrimitive on an nv10.");
                DPF ("this is totally untested, may very well not work.");
                dbgD3DError();
                nvCelsiusDispatchNonIndexedPrimitive (pContext, wPrimCount);
            }
            else
#endif  // NVARCH >= 0x010
            {
                switch (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE])
                {
                    case D3DFILL_SOLID:
                        DDSTARTTICK (PRIM_DRAWSTRIPSOLIDNONINDEXED);
                        DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wNumVertices);
                        nvTriangleDispatch (pContext, wPrimCount);
                        DDENDTICK (PRIM_DRAWSTRIPSOLIDNONINDEXED);
                        break;
                    case D3DFILL_WIREFRAME:
                        DDSTARTTICK (PRIM_DRAWSTRIPWIRENONINDEXED);
                        DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wNumVertices);
                        nvDrawWireframeTriangle (pContext, wPrimCount);
                        DDENDTICK (PRIM_DRAWSTRIPWIRENONINDEXED);
                        break;
                    case D3DFILL_POINT:
                        DDSTARTTICK (PRIM_DRAWSTRIPSOLIDNONINDEXED);
                        DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wNumVertices);
                        nvDrawWireframeTriangle (pContext, wPrimCount);
                        DDENDTICK (PRIM_DRAWSTRIPSOLIDNONINDEXED);
                        break;
                }
            }

            break;

        case D3DPT_TRIANGLEFAN:

            pContext->dp2.dwDP2Prim = D3DDP2OP_TRIANGLEFAN;
            pContext->dp2.dwIndices = NULL;

            wPrimCount = wNumVertices - 2;
            if (wPrimCount == 0) break;

#if (NVARCH >= 0x020)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                DPF ("ugh. got a legacy call to nvDrawPrimitive on an nv20.");
                DPF ("this is totally untested, may very well not work.");
                dbgD3DError();
                nvKelvinDispatchNonIndexedPrimitive (pContext, wPrimCount);
            }
            else
#endif  // (NVARCH >= 0x020)
#if (NVARCH >= 0x010)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                DPF ("ugh. got a legacy call to nvDrawPrimitive on an nv10.");
                DPF ("this is totally untested, may very well not work.");
                dbgD3DError();
                nvCelsiusDispatchNonIndexedPrimitive (pContext, wPrimCount);
            }
            else
#endif  // NVARCH >= 0x010
            {
                switch (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE])
                {
                    case D3DFILL_SOLID:
                        DDSTARTTICK(PRIM_DRAWFANSOLIDNONINDEXED);
                        DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wNumVertices);
                        nvTriangleDispatch (pContext, wPrimCount);
                        DDENDTICK(PRIM_DRAWFANSOLIDNONINDEXED);
                        break;
                    case D3DFILL_WIREFRAME:
                        DDSTARTTICK(PRIM_DRAWFANWIREDNONINDEXED);
                        DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wNumVertices);
                        nvDrawWireframeTriangle (pContext, wPrimCount);
                        DDENDTICK(PRIM_DRAWFANWIREDNONINDEXED);
                        break;
                    case D3DFILL_POINT:
                        DDSTARTTICK(PRIM_DRAWFANPOINTNONINDEXED);
                        DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wNumVertices);
                        nvDrawWireframeTriangle (pContext, wPrimCount);
                        DDSTARTTICK(PRIM_DRAWFANPOINTNONINDEXED);
                        break;
                }
            }

            break;

        case D3DPT_LINELIST:

            pContext->dp2.dwDP2Prim = D3DDP2OP_LINELIST;
            pContext->dp2.dwIndices = NULL;

            wPrimCount = wNumVertices / 2;

#if (NVARCH >= 0x020)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                DPF ("ugh. got a legacy call to nvDrawPrimitive on an nv20.");
                DPF ("this is totally untested, may very well not work.");
                dbgD3DError();
                nvKelvinDispatchNonIndexedPrimitive (pContext, wPrimCount);
            }
            else
#endif  // (NVARCH >= 0x020)
#if (NVARCH >= 0x010)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                DPF ("ugh. got a legacy call to nvDrawPrimitive on an nv10.");
                DPF ("this is totally untested, may very well not work.");
                dbgD3DError();
                nvCelsiusDispatchNonIndexedPrimitive (pContext, wPrimCount);
            }
            else
#endif  // NVARCH >= 0x010
            {
                DDSTARTTICK(PRIM_DRAWLINELISTNONINDEXED);
                DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wNumVertices);
                nvDrawLine (pContext, wPrimCount);
                DDENDTICK(PRIM_DRAWLINELISTNONINDEXED);
            }

            break;

        case D3DPT_LINESTRIP:

            pContext->dp2.dwDP2Prim = D3DDP2OP_LINESTRIP;
            pContext->dp2.dwIndices = NULL;

            wPrimCount = wNumVertices - 1;

#if (NVARCH >= 0x020)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                DPF ("ugh. got a legacy call to nvDrawPrimitive on an nv20.");
                DPF ("this is totally untested, may very well not work.");
                dbgD3DError();
                nvKelvinDispatchNonIndexedPrimitive (pContext, wPrimCount);
            }
            else
#endif  // (NVARCH >= 0x020)
#if (NVARCH >= 0x010)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                DPF ("ugh. got a legacy call to nvDrawPrimitive on an nv10.");
                DPF ("this is totally untested, may very well not work.");
                dbgD3DError();
                nvCelsiusDispatchNonIndexedPrimitive (pContext, wPrimCount);
            }
            else
#endif  // NVARCH >= 0x010
            {
                DDSTARTTICK(PRIM_DRAWLINESTRIPNONINDEXED);
                DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wNumVertices);
                nvDrawLine (pContext, wPrimCount);
                DDENDTICK(PRIM_DRAWLINESTRIPNONINDEXED);
            }

            break;

        case D3DPT_POINTLIST:

            // nvDrawPointList((DWORD)((LPD3DHAL_DP2POINTS)lpPrim)->wCount, (LPD3DTLVERTEX)&pVertices[((LPD3DHAL_DP2POINTS)lpPrim)->wVStart]);
            break;

    }

    /*
     * Update the put offset.
     */
    nvPusherStart (FALSE);
    pDriverData->TwoDRenderingOccurred   = 0;
    pDriverData->ThreeDRenderingOccurred = TRUE;
    DDENDTICK(PRIM_DRAWPRIMITIVE);
    return;
}

//---------------------------------------------------------------------------

/*
 * DirectX 5.0 DrawOnePrimitive entry point.
 */

DWORD nvDrawOnePrimitive
(
    LPD3DHAL_DRAWONEPRIMITIVEDATA pdopd
)
{
    dbgTracePush ("nvDrawOnePrimitive");

    DDSTARTTICK(PRIM_DRAWONEPRIMITIVE);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, pdopd->dwNumVertices);

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

#ifdef NV_NULL_DRIVER
    pdopd->ddrval = DD_OK;
    DDENDTICK(PRIM_DRAWONEPRIMITIVE);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
#endif //NV_NULL_DRIVER

    nvAssert (global.dwDXRuntimeVersion < 0x0700);

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pdopd);

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED)
    {
        pdopd->ddrval = DD_OK;
        DDENDTICK(PRIM_DRAWONEPRIMITIVE);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

#if (NVARCH >= 0x010)
    if (pContext && pContext->aa.isEnabled())
    {
        pContext->aa.makeSuperBufferValid(pContext);
        if(pContext->aa.pSuperZetaBuffer) pContext->aa.makeSuperZBValid(pContext);
    }
#endif  // NVARCH >= 0x010

    /*
     * Need to make sure that an unfriendly mode switch didn't sneak and not cause
     * us to get re-enabled properly.
     */
    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvD3DRecover();
        }
        else {
            pdopd->ddrval = DD_OK;
            DDENDTICK(PRIM_DRAWONEPRIMITIVE);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }

    // Just pass off this call to the DrawPrimitive helper routine.
    if (pdopd->dwNumVertices)
    {
        // If the surface has changed since the last render call, switch it now.
        nvDrawPrimitive (pContext,
                         (WORD)pdopd->PrimitiveType,
                         (WORD)pdopd->dwNumVertices,
                         (LPBYTE)pdopd->lpvVertices);
    }

    // Return successfully.
    pdopd->ddrval = DD_OK;
    DDENDTICK(PRIM_DRAWONEPRIMITIVE);

    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

//---------------------------------------------------------------------------

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

    dbgTracePush ("nvDrawPrimitives");

    DDSTARTTICK(PRIM_DRAWPRIMITIVES);

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

#ifdef NV_NULL_DRIVER
    pdpd->ddrval = DD_OK;
    DDENDTICK(PRIM_DRAWPRIMITIVES);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
#endif //NV_NULL_DRIVER

    nvAssert (global.dwDXRuntimeVersion < 0x0700);

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pdpd);

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        pdpd->ddrval = DD_OK;
        DDENDTICK(PRIM_DRAWPRIMITIVES);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Need to make sure that an unfriendly mode switch didn't sneak and not cause
     * us to get re-enabled properly.
     */
    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvD3DRecover();
        }
        else {
            pdpd->ddrval = DD_OK;
            DDENDTICK(PRIM_DRAWPRIMITIVES);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }

    if ((pContext->bStateChange || pDriverData->TwoDRenderingOccurred)
     || (pDriverData->dDrawSpareSubchannelObject != D3D_DX5_TEXTURED_TRIANGLE))
        NV_FORCE_TRI_SETUP(pContext);

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

        DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)lpDrawPrimCounts->wNumVertices);
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
                // Process the state changes for the next set of vertices.
                nvSetContextState (pContext, lpStateChanges[0], lpStateChanges[1], &pdpd->ddrval);
                if (pdpd->ddrval != DD_OK)
                {
                    DDENDTICK(PRIM_DRAWPRIMITIVES);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
                lpStateChanges += 2;
            }
            /*
             * If the state has changed force a hardware state load.
             */
            if (pContext->bStateChange) {
                NV_FORCE_TRI_SETUP(pContext);
            }
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

            nvDrawPrimitive (pContext,
                             wPrimitiveType,
                             lpDrawPrimCounts->wNumVertices,
                             lpData);

            lpData += (lpDrawPrimCounts->wNumVertices * sizeof(D3DTLVERTEX));
        }
    } while (lpDrawPrimCounts->wNumVertices);

    /*
     * Return successfully.
     */
    pdpd->ddrval = DD_OK;
    DDENDTICK(PRIM_DRAWPRIMITIVES);

    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

//---------------------------------------------------------------------------

DWORD nvDrawOneIndexedPrimitive
(
    LPD3DHAL_DRAWONEINDEXEDPRIMITIVEDATA pdoipd
)
{
    CVertexBuffer *pVertexBuffer;
    LPBYTE pIndices;
    DWORD  dwPrimCount;

    dbgTracePush ("nvDrawOneIndexedPrimitive");

    DDSTARTTICK(PRIM_DRAWONEINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)pdoipd->dwNumIndices);

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

#ifdef NV_NULL_DRIVER
    pdoipd->ddrval = DD_OK;
    DDENDTICK(PRIM_DRAWONEINDEXED);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
#endif //NV_NULL_DRIVER

    nvAssert (global.dwDXRuntimeVersion < 0x0700);

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pdoipd);

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        pdoipd->ddrval = DD_OK;
        DDENDTICK(PRIM_DRAWONEINDEXED);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

#if (NVARCH >= 0x010)
    if (pContext && pContext->aa.isEnabled())
    {
        pContext->aa.makeSuperBufferValid(pContext);
        if(pContext->aa.pSuperZetaBuffer) pContext->aa.makeSuperZBValid(pContext);
    }
#endif  // NVARCH >= 0x010

    /*
     * Need to make sure that an unfriendly mode switch didn't sneak and not cause
     * us to get re-enabled properly.
     */
    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvD3DRecover();
        }
        else {
            pdoipd->ddrval = DD_OK;
            DDENDTICK(PRIM_DRAWONEINDEXED);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }

    pContext->pCurrentVShader->create (pContext, D3DFVF_TLVERTEX, CVertexShader::getHandleFromFvf(D3DFVF_TLVERTEX));

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_TL_MODE |
                                       CELSIUS_DIRTY_FVF |
                                       CELSIUS_DIRTY_LIGHTS);
    pContext->hwState.dwDirtyFlags |= (KELVIN_DIRTY_FVF |
                                       KELVIN_DIRTY_LIGHTS);
#endif  // nv20

    if (pdoipd->dwNumVertices && pdoipd->dwNumIndices)
    {
        pIndices           = (LPBYTE)(pdoipd->lpwIndices);

        // cast plain vertex data into a vertex buffer
        pVertexBuffer = getDC()->defaultSysVB.getVB();
        pVertexBuffer->own ((DWORD)(pdoipd->lpvVertices),
                            pdoipd->dwNumVertices * sizeof(D3DTLVERTEX),
                            CSimpleSurface::HEAP_SYS);
        // cast vertex buffer into DX8-style streams
        for (int i=0; i<NV_CAPS_MAX_STREAMS; i++) {
            pContext->ppDX8Streams[i] = pVertexBuffer;
        }

        /*
         * Send down the surface information if neccessary.
         */
        switch (pdoipd->PrimitiveType) {

            case D3DPT_TRIANGLELIST:

                pContext->dp2.dwDP2Prim = D3DDP2OP_INDEXEDTRIANGLELIST2;
                pContext->dp2.dwIndices = (DWORD)(pIndices);

                dwPrimCount =  pdoipd->dwNumIndices / 3;

#if (NVARCH >= 0x020)
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                    DPF ("ugh. got a legacy call to nvDrawOneIndexedPrimitive on an nv20.");
                    DPF ("this is totally untested, may very well not work.");
                    dbgD3DError();
                    nvKelvinDispatchIndexedPrimitive (pContext, (WORD)dwPrimCount);
                }
                else
#endif  // (NVARCH >= 0x020)
#if (NVARCH >= 0x010)
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                    DPF ("ugh. got a legacy call to nvDrawOneIndexedPrimitive on an nv10.");
                    DPF ("this is totally untested, may very well not work.");
                    dbgD3DError();
                    nvCelsiusDispatchIndexedPrimitive (pContext, (WORD)dwPrimCount);
                }
                else
#endif  // NVARCH >= 0x010
                {
                    switch (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]) {
                        case D3DFILL_SOLID:
                            nvTriangleDispatch (pContext, (WORD)dwPrimCount);
                            break;
                        case D3DFILL_WIREFRAME:
                            nvIndexedWireframeTriangle (pContext, (WORD)dwPrimCount);
                            break;
                        case D3DFILL_POINT:
                            nvIndexedPointTriangle (pContext, (WORD)dwPrimCount);
                            break;
                    }
                }

                break;

            case D3DPT_TRIANGLESTRIP:

                pContext->dp2.dwDP2Prim = D3DDP2OP_INDEXEDTRIANGLESTRIP;
                pContext->dp2.dwIndices = (DWORD)(pIndices);

                dwPrimCount = pdoipd->dwNumIndices - 2;

#if (NVARCH >= 0x020)
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                    DPF ("ugh. got a legacy call to nvDrawOneIndexedPrimitive on an nv20.");
                    DPF ("this is totally untested, may very well not work.");
                    dbgD3DError();
                    nvKelvinDispatchIndexedPrimitive (pContext, (WORD)dwPrimCount);
                }
                else
#endif  // (NVARCH >= 0x020)
#if (NVARCH >= 0x010)
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                    DPF ("ugh. got a legacy call to nvDrawOneIndexedPrimitive on an nv10.");
                    DPF ("this is totally untested, may very well not work.");
                    dbgD3DError();
                    nvCelsiusDispatchIndexedPrimitive (pContext, (WORD)dwPrimCount);
                }
                else
#endif  // NVARCH >= 0x010
                {
                    switch (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]) {
                        case D3DFILL_SOLID:
                            nvTriangleDispatch (pContext, (WORD)dwPrimCount);
                            break;
                        case D3DFILL_WIREFRAME:
                            nvIndexedWireframeTriangle (pContext, (WORD)dwPrimCount);
                            break;
                        case D3DFILL_POINT:
                            nvIndexedPointTriangle (pContext, (WORD)dwPrimCount);
                            break;
                    }
                }

                break;

            case D3DPT_TRIANGLEFAN:

                pContext->dp2.dwDP2Prim = D3DDP2OP_INDEXEDTRIANGLEFAN;
                pContext->dp2.dwIndices = (DWORD)(pIndices);

                dwPrimCount = pdoipd->dwNumIndices - 2;

#if (NVARCH >= 0x020)
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                    DPF ("ugh. got a legacy call to nvDrawOneIndexedPrimitive on an nv20.");
                    DPF ("this is totally untested, may very well not work.");
                    dbgD3DError();
                    nvKelvinDispatchIndexedPrimitive (pContext, (WORD)dwPrimCount);
                }
                else
#endif  // (NVARCH >= 0x020)
#if (NVARCH >= 0x010)
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                    DPF ("ugh. got a legacy call to nvDrawOneIndexedPrimitive on an nv10.");
                    DPF ("this is totally untested, may very well not work.");
                    dbgD3DError();
                    nvCelsiusDispatchIndexedPrimitive (pContext, (WORD)dwPrimCount);
                }
                else
#endif  // NVARCH >= 0x010
                {
                    switch (pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]) {
                        case D3DFILL_SOLID:
                            nvTriangleDispatch (pContext, (WORD)dwPrimCount);
                            break;
                        case D3DFILL_WIREFRAME:
                            nvIndexedWireframeTriangle (pContext, (WORD)dwPrimCount);
                            break;
                        case D3DFILL_POINT:
                            nvIndexedPointTriangle (pContext, (WORD)dwPrimCount);
                            break;
                    }
                }

                break;

            case D3DPT_LINELIST:

                pContext->dp2.dwDP2Prim = D3DDP2OP_INDEXEDLINELIST;
                pContext->dp2.dwIndices = (DWORD)(pIndices);

                dwPrimCount = pdoipd->dwNumIndices / 2;

#if (NVARCH >= 0x020)
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                    DPF ("ugh. got a legacy call to nvDrawOneIndexedPrimitive on an nv20.");
                    DPF ("this is totally untested, may very well not work.");
                    dbgD3DError();
                    nvKelvinDispatchIndexedPrimitive (pContext, (WORD)dwPrimCount);
                }
                else
#endif  // (NVARCH >= 0x020)
#if (NVARCH >= 0x010)
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                    DPF ("ugh. got a legacy call to nvDrawOneIndexedPrimitive on an nv10.");
                    DPF ("this is totally untested, may very well not work.");
                    dbgD3DError();
                    nvCelsiusDispatchIndexedPrimitive (pContext, (WORD)dwPrimCount);
                }
                else
#endif  // NVARCH >= 0x010
                {
                    nvIndexedLine (pContext, (WORD)dwPrimCount);
                }

                break;

            case D3DPT_LINESTRIP:

                pContext->dp2.dwDP2Prim = D3DDP2OP_INDEXEDLINESTRIP;
                dwPrimCount = pdoipd->dwNumIndices - 1;

#if (NVARCH >= 0x020)
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                    DPF ("ugh. got a legacy call to nvDrawOneIndexedPrimitive on an nv20.");
                    DPF ("this is totally untested, may very well not work.");
                    dbgD3DError();
                    nvKelvinDispatchIndexedPrimitive (pContext, (WORD)dwPrimCount);
                }
                else
#endif // (NVARCH >= 0x020)
#if (NVARCH >= 0x010)
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                    DPF ("ugh. got a legacy call to nvDrawOneIndexedPrimitive on an nv10.");
                    DPF ("this is totally untested, may very well not work.");
                    dbgD3DError();
                    nvCelsiusDispatchIndexedPrimitive (pContext, (WORD)dwPrimCount);
                }
                else
#endif  // NVARCH >= 0x010
                {
                    nvIndexedLine (pContext, (WORD)dwPrimCount);
                }

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
    nvPusherStart (FALSE);

    pDriverData->TwoDRenderingOccurred   = 0;
    pDriverData->ThreeDRenderingOccurred = TRUE;

    /*
     * Return successfully.
     */
    pdoipd->ddrval = DD_OK;
    DDENDTICK(PRIM_DRAWONEINDEXED);

    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

#endif  // !WINNT

#endif  // NVARCH >= 0x04

