/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvPoint.cpp                                                       *
*   The Direct3D point rendering routines.                                  *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       04/12/97 - created                      *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"


#if (NVARCH >= 0x04)

//-------------------------------------------------------------------------

/*
 * Non-Indexed TLVertex Point Lists.
 * This routine renders the specified number of points starting
 * with the vertex pointed to by lpVertices.
 * lpVertices is assumed to point to the first point to be rendered.
 */
void nvDrawPointList
(
    NV_INNERLOOP_ARGLIST
)
{
    DWORD           dwControl;
    DWORD           dwOldCull;
    D3DTLVERTEX     PointVerts[4];

    dbgTracePush ("nvDrawPointList");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    CVertexBuffer *pIncomingVB;
    CVertexBuffer  tempVB;
    LPBYTE         pVertexBufferBase;
    DWORD          dwVertexBufferOffset = pContext->dp2.dwTotalOffset;
    // cache incoming vertex buffer
    pIncomingVB = pContext->ppDX8Streams[0];
    pVertexBufferBase = (LPBYTE)(pIncomingVB->getAddress());

    // temporarily override the incoming vertex buffer with the one we'll construct
    tempVB.own ((DWORD)(&PointVerts[0]), 4*sizeof(D3DTLVERTEX), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLEFAN;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 4 * sizeof(D3DTLVERTEX);

    LPD3DTLVERTEX pVertices = (LPD3DTLVERTEX)(pVertexBufferBase + dwVertexBufferOffset);

    // Calculate the new hardware state if neccessary.
    if (pContext->bStateChange) {
        if (pContext->bUseDX6Class){
            nvSetMultiTextureHardwareState (pContext);
            if (pContext->bUseDX6Class) {
                nvSetHardwareState (pContext);
            }
        }
        else {
            nvSetHardwareState (pContext);
        }
    }

    // Disable lower level culling while drawing lines.
    dwControl = pContext->ctxInnerLoop.dwControl;
    pContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
    pContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
    dwOldCull = pContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
    pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

    // Force first call to be to the triangle state setup routine.
    NV_FORCE_TRI_SETUP(pContext);

    // Draw all points.
    while (dwCount--)
    {
        // Render each point as a POINT_SIZE square

        // replicate the data
        PointVerts[0] = *pVertices;
        PointVerts[1] = *pVertices;
        PointVerts[2] = *pVertices;
        PointVerts[3] = *pVertices;

        // adjust vertex 0
        PointVerts[0].sx -= HALF_POINT_SIZE;
        PointVerts[0].sy -= HALF_POINT_SIZE;
        // adjust vertex 1
        PointVerts[1].sx -= HALF_POINT_SIZE;
        PointVerts[1].sy += HALF_POINT_SIZE;
        // adjust vertex 2
        PointVerts[2].sx += HALF_POINT_SIZE;
        PointVerts[2].sy += HALF_POINT_SIZE;
        // adjust vertex 3
        PointVerts[3].sx += HALF_POINT_SIZE;
        PointVerts[3].sy -= HALF_POINT_SIZE;

        nvTriangleDispatch (pContext, 2);

        pVertices++;
    }

    // Restore cull mode
    pContext->ctxInnerLoop.dwControl = dwControl;
    pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;

    // reinstate the incoming vertex buffer
    pContext->ppDX8Streams[0]   = pIncomingVB;
    pContext->dp2.dwDP2Prim     = dwCachedDP2Op;
    pContext->dp2.dwTotalOffset = dwCachedTotalOffset;
    global.dwMaxVertexOffset    = dwCachedMaxOffset;

    // Force next render call to be to the triangle state setup routine.
    NV_FORCE_TRI_SETUP(pContext);

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------

DWORD nvRenderPoint
(
    NV_INNERLOOP_ARGLIST
)
{
    nvDrawPointList (NV_INNERLOOP_ARGS);
    return (DD_OK);
}

//-------------------------------------------------------------------------

void nvDrawPointTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    D3DTLVERTEX PointVerts[4];
    DWORD       adwIndex[3], dwIndex;
    DWORD       dwControl;
    DWORD       dwOldCull;
    DWORD       dwIndex1, dwIndex2;
    DWORD       dwIndexStrides;
    DWORD       dwCullMask1;
    DWORD       dwCullMask2;
    float       dwCullValue;

    dbgTracePush ("nvDrawPointTriangle");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    CVertexBuffer *pIncomingVB;
    CVertexBuffer  tempVB;
    LPBYTE         pVertexBufferBase;
    DWORD          dwVertexBufferOffset = pContext->dp2.dwTotalOffset;

    // cache incoming vertex buffer
    pIncomingVB = pContext->ppDX8Streams[0];
    pVertexBufferBase = (LPBYTE)(pIncomingVB->getAddress());

    // temporarily override the incoming vertex buffer with the one we'll construct
    tempVB.own ((DWORD)(&PointVerts[0]), 4*sizeof(D3DTLVERTEX), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLEFAN;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 4 * sizeof(D3DTLVERTEX);

    if (dwCount)
    {
        LPD3DTLVERTEX pVertices = (LPD3DTLVERTEX)(pVertexBufferBase + dwVertexBufferOffset);
        DWORD         dwStrides = nv4DP2OpToStride[dwCachedDP2Op];

        // Calculate the new hardware state if neccessary
        if (pContext->bStateChange) {
            if (pContext->bUseDX6Class) {
                nvSetMultiTextureHardwareState (pContext);
                if (pContext->bUseDX6Class) {
                    nvSetHardwareState (pContext);
                }
            }
            else {
                nvSetHardwareState (pContext);
            }
        }

        // Disable lower level culling while drawing lines
        dwControl = pContext->ctxInnerLoop.dwControl;
        pContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
        pContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        dwOldCull = pContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        // Calculate the culling masks.
        dwCullMask1 = (dwOldCull == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
        dwCullMask2 = (dwOldCull == D3DCULL_NONE) ? 0xffffffff : 0x80000000;

        // Force first call to be to the triangle state setup routine
        NV_FORCE_TRI_SETUP(pContext);

        dwStrides     &= 0xffffff; // mask interesting bits
        dwIndex1       = 0;
        dwIndex2       = 1;
        dwIndexStrides = dwStrides;

        // draw all triangles
        while (dwCount--)
        {
            adwIndex[0] = dwIndex1;
            adwIndex[1] = dwIndex2 + (dwIndexStrides >> 24);
            adwIndex[2] = dwIndex2 + ((dwIndexStrides >> 24) ^ 1);

            // Move on to the next triangle.
            dwIndex1       += (BYTE)(dwIndexStrides >> 8);
            dwIndex2       += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwIndexStrides << 8) & 0xff000000;

            // Since culling needs to be disabled to insure that the triangle edges get drawn properly,
            // back face culling of the triangle needs to be performed up front by software.
            IF_NOT_CULLED_BEGIN((DWORD)&pVertices[adwIndex[0]],(DWORD)&pVertices[adwIndex[1]],(DWORD)&pVertices[adwIndex[2]],dwCullValue)

                // draw each vertex, expanding it to a POINT_SIZE square
                for (DWORD dwVert=0; dwVert<3; dwVert++) {

                    dwIndex = adwIndex[dwVert];

                    // replicate the data
                    PointVerts[0] = pVertices[dwIndex];
                    PointVerts[1] = pVertices[dwIndex];
                    PointVerts[2] = pVertices[dwIndex];
                    PointVerts[3] = pVertices[dwIndex];

                    // adjust vertex 0
                    PointVerts[0].sx -= HALF_POINT_SIZE;
                    PointVerts[0].sy -= HALF_POINT_SIZE;
                    // adjust vertex 1
                    PointVerts[1].sx -= HALF_POINT_SIZE;
                    PointVerts[1].sy += HALF_POINT_SIZE;
                    // adjust vertex 2
                    PointVerts[2].sx += HALF_POINT_SIZE;
                    PointVerts[2].sy += HALF_POINT_SIZE;
                    // adjust vertex 3
                    PointVerts[3].sx += HALF_POINT_SIZE;
                    PointVerts[3].sy -= HALF_POINT_SIZE;

                    // draw
                    nvTriangleDispatch (pContext, 2);
                }

            IF_NOT_CULLED_END
        }

        // Restore the culling mode.
        pContext->ctxInnerLoop.dwControl = dwControl;
        pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;
    }

    // reinstate the incoming vertex buffer
    pContext->ppDX8Streams[0]   = pIncomingVB;
    pContext->dp2.dwDP2Prim     = dwCachedDP2Op;
    pContext->dp2.dwTotalOffset = dwCachedTotalOffset;
    global.dwMaxVertexOffset    = dwCachedMaxOffset;

    // Force first call to be to the triangle state setup routine.
    NV_FORCE_TRI_SETUP(pContext);

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------

void nvIndexedPointTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    D3DTLVERTEX PointVerts[4];
    DWORD       adwIndex[3], dwIndex;
    DWORD       dwControl;
    DWORD       dwOldCull;
    PBYTE       pIndex1, pIndex2;
    DWORD       dwIndexStrides;
    DWORD       dwCullMask1;
    DWORD       dwCullMask2;
    float       dwCullValue;

    dbgTracePush ("nvIndexedPointTriangle");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    CVertexBuffer *pIncomingVB;
    CVertexBuffer  tempVB;
    LPBYTE         pVertexBufferBase;
    DWORD          dwVertexBufferOffset = pContext->dp2.dwTotalOffset;

    // cache incoming vertex buffer
    pIncomingVB = pContext->ppDX8Streams[0];
    pVertexBufferBase = (LPBYTE)(pIncomingVB->getAddress());

    // temporarily override the incoming vertex buffer with the one we'll construct
    tempVB.own ((DWORD)(&PointVerts[0]), 4*sizeof(D3DTLVERTEX), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLEFAN;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 4 * sizeof(D3DTLVERTEX);

    if (dwCount)
    {
        LPD3DTLVERTEX pVertices = (LPD3DTLVERTEX)(pVertexBufferBase + dwVertexBufferOffset);
        LPWORD        pIndices  = (LPWORD)(pContext->dp2.dwIndices);
        DWORD         dwStrides = nv4DP2OpToStride[dwCachedDP2Op];

        pContext->dp2.dwIndices = NULL;

        // Calculate the new hardware state if neccessary.
        if (pContext->bStateChange) {
            if (pContext->bUseDX6Class) {
                nvSetMultiTextureHardwareState (pContext);
                if (pContext->bUseDX6Class) {
                    nvSetHardwareState (pContext);
                }
            }
            else {
                nvSetHardwareState (pContext);
            }
        }

        // Disable lower level culling while drawing lines.
        dwControl = pContext->ctxInnerLoop.dwControl;
        pContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
        pContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        dwOldCull = pContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        // Calculate the culling masks.
        dwCullMask1 = (dwOldCull == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
        dwCullMask2 = (dwOldCull == D3DCULL_NONE) ? 0xffffffff : 0x80000000;

        // Force first call to be to the triangle state setup routine.
        NV_FORCE_TRI_SETUP(pContext);

        dwStrides     &= 0xffffff; // mask interesting bits
        pIndex1        = (PBYTE)pIndices;
        pIndex2        = (PBYTE)pIndices + 2;
        dwIndexStrides = dwStrides * 2;

        // draw all triangles
        while (dwCount--)
        {
            adwIndex[0] = *(WORD*)(pIndex1);
            adwIndex[1] = *(WORD*)(pIndex2 +  (dwIndexStrides >> 24));
            adwIndex[2] = *(WORD*)(pIndex2 + ((dwIndexStrides >> 24) ^ 2));

            // Move on to the next triangle.
            pIndex1        += (BYTE)(dwIndexStrides >> 8);
            pIndex2        += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwIndexStrides << 8) & 0xff000000;

            // Since culling needs to be disabled to insure that the triangle edges get drawn properly,
            // back face culling of the triangle needs to be performed up front by software.
            IF_NOT_CULLED_BEGIN((DWORD)&pVertices[adwIndex[0]],(DWORD)&pVertices[adwIndex[1]],(DWORD)&pVertices[adwIndex[2]],dwCullValue)

            // draw each vertex, expanding it to a POINT_SIZE square
            for (DWORD dwVert=0; dwVert<3; dwVert++) {

                dwIndex = adwIndex[dwVert];

                // replicate the data
                PointVerts[0] = pVertices[dwIndex];
                PointVerts[1] = pVertices[dwIndex];
                PointVerts[2] = pVertices[dwIndex];
                PointVerts[3] = pVertices[dwIndex];

                // adjust vertex 0
                PointVerts[0].sx -= HALF_POINT_SIZE;
                PointVerts[0].sy -= HALF_POINT_SIZE;
                // adjust vertex 1
                PointVerts[1].sx -= HALF_POINT_SIZE;
                PointVerts[1].sy += HALF_POINT_SIZE;
                // adjust vertex 2
                PointVerts[2].sx += HALF_POINT_SIZE;
                PointVerts[2].sy += HALF_POINT_SIZE;
                // adjust vertex 3
                PointVerts[3].sx += HALF_POINT_SIZE;
                PointVerts[3].sy -= HALF_POINT_SIZE;

                // draw
                nvTriangleDispatch (pContext, 2);
            }

            IF_NOT_CULLED_END
        }

        // Restore the culling mode.
        pContext->ctxInnerLoop.dwControl = dwControl;
        pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;
    }

    // reinstate the incoming vertex buffer
    pContext->ppDX8Streams[0]   = pIncomingVB;
    pContext->dp2.dwDP2Prim     = dwCachedDP2Op;
    pContext->dp2.dwTotalOffset = dwCachedTotalOffset;
    global.dwMaxVertexOffset    = dwCachedMaxOffset;

    // Force first call to be to the triangle state setup routine.
    NV_FORCE_TRI_SETUP(pContext);

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------

/*
 * Non-Indexed Flexible Vertex Format Point Lists.
 * This routine renders the specified number of points starting
 * with the vertex pointed to by pVertices.
 * pVertices is assumed to point to the first point to be rendered.
 */
void nvFVFDrawPointList
(
    NV_INNERLOOP_ARGLIST
)
{
    BYTE          PointVerts[4 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    BYTE         *pPointVert[4];
    BYTE         *pVert0;
    DWORD         i, dwDwordsPerVert;
    DWORD         dwControl;
    DWORD         dwOldCull;
    DWORD         dwVertexStride;

    dbgTracePush ("nvFVFDrawPointList");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    CVertexBuffer *pIncomingVB;
    CVertexBuffer  tempVB;
    LPBYTE         pVertexBufferBase;
    DWORD          dwVertexBufferOffset = pContext->dp2.dwTotalOffset;

    // cache incoming vertex buffer
    pIncomingVB = pContext->ppDX8Streams[0];
    pVertexBufferBase = (LPBYTE)(pIncomingVB->getAddress());

    // temporarily override the incoming vertex buffer with the one we'll construct
    tempVB.own ((DWORD)(&PointVerts[0]), 4*((sizeof(D3DTLVERTEX))+(7*2*4)), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLEFAN;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 4 * ((sizeof(D3DTLVERTEX)) + (7*2*4));

    LPBYTE pVertices = pVertexBufferBase + dwVertexBufferOffset;

    // Calculate the new hardware state if neccessary.
    if (pContext->bStateChange) {
        if (pContext->bUseDX6Class) {
            nvSetMultiTextureHardwareState (pContext);
            if (pContext->bUseDX6Class) {
                nvSetHardwareState (pContext);
            }
        }
        else {
            nvSetHardwareState (pContext);
        }
    }

    // Disable lower level culling while drawing lines.
    if (!pContext->bUseDX6Class) {
        dwControl = pContext->ctxInnerLoop.dwControl;
        pContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
        pContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
    }
    else {
        dwControl = pContext->mtsState.dwControl0;
        pContext->mtsState.dwControl0 &= ~(DRF_MASK(NV055_CONTROL0_CULLMODE) << DRF_SHIFT(NV055_CONTROL0_CULLMODE));
        pContext->mtsState.dwControl0 |= DRF_DEF(055, _CONTROL0, _CULLMODE, _NONE);
    }
    dwOldCull = pContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
    pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

    // Force first call to be to the triangle state setup routine.
    NV_FORCE_TRI_SETUP(pContext);

    dwVertexStride  = pContext->pCurrentVShader->getStride();
    dwDwordsPerVert = pContext->pCurrentVShader->getStride() >> 2;

    // get pointers to each of the 4 FVF point vertices.
    GET_FVF_POINTER (pPointVert[0], PointVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 0, pContext->pCurrentVShader->getStride());
    GET_FVF_POINTER (pPointVert[1], PointVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 1, pContext->pCurrentVShader->getStride());
    GET_FVF_POINTER (pPointVert[2], PointVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 2, pContext->pCurrentVShader->getStride());
    GET_FVF_POINTER (pPointVert[3], PointVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 3, pContext->pCurrentVShader->getStride());

    // Render each point as a POINT_SIZE square
    while (dwCount--)
    {
        // Get pointers to the vertex of current point.
        GET_FVF_POINTER (pVert0, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 0, pContext->pCurrentVShader->getStride());

        pVertices += dwVertexStride;

        // replicate the vertex data
        for (i = 0; i < dwDwordsPerVert; i++) {
            DWORD dwValue;
            dwValue = ((DWORD *)pVert0)[i];
            ((DWORD *)(pPointVert[0]))[i] = dwValue;
            ((DWORD *)(pPointVert[1]))[i] = dwValue;
            ((DWORD *)(pPointVert[2]))[i] = dwValue;
            ((DWORD *)(pPointVert[3]))[i] = dwValue;
        }

        // adjust vertex 0
        ((LPD3DTLVERTEX)(pPointVert[0]))->sx -= HALF_POINT_SIZE;
        ((LPD3DTLVERTEX)(pPointVert[0]))->sy -= HALF_POINT_SIZE;
        // adjust vertex 1
        ((LPD3DTLVERTEX)(pPointVert[1]))->sx -= HALF_POINT_SIZE;
        ((LPD3DTLVERTEX)(pPointVert[1]))->sy += HALF_POINT_SIZE;
        // adjust vertex 2
        ((LPD3DTLVERTEX)(pPointVert[2]))->sx += HALF_POINT_SIZE;
        ((LPD3DTLVERTEX)(pPointVert[2]))->sy += HALF_POINT_SIZE;
        // adjust vertex 3
        ((LPD3DTLVERTEX)(pPointVert[3]))->sx += HALF_POINT_SIZE;
        ((LPD3DTLVERTEX)(pPointVert[3]))->sy -= HALF_POINT_SIZE;

        nvTriangleDispatch (pContext, 2);

    }  // while (dwCount)

    // Restore cull mode.
    if (!pContext->bUseDX6Class)
        pContext->ctxInnerLoop.dwControl = dwControl;
    else
        pContext->mtsState.dwControl0 = dwControl;
    pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;

    // reinstate the incoming vertex buffer
    pContext->ppDX8Streams[0]   = pIncomingVB;
    pContext->dp2.dwDP2Prim     = dwCachedDP2Op;
    pContext->dp2.dwTotalOffset = dwCachedTotalOffset;
    global.dwMaxVertexOffset    = dwCachedMaxOffset;

    // Force next render call to be to the triangle state setup routine.
    NV_FORCE_TRI_SETUP(pContext);

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------

void nvFVFDrawPointTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    BYTE        PointVerts[4 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    BYTE       *pPointVert[4];
    BYTE       *pVert[3];
    DWORD       i, dwDwordsPerVert;
    DWORD       adwIndex[3];
    DWORD       dwControl;
    DWORD       dwOldCull;
    DWORD       dwIndex1, dwIndex2;
    DWORD       dwIndexStrides;
    DWORD       dwCullMask1;
    DWORD       dwCullMask2;
    float       dwCullValue;

    dbgTracePush ("nvFVFDrawPointTriangle");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    CVertexBuffer *pIncomingVB;
    CVertexBuffer  tempVB;
    LPBYTE         pVertexBufferBase;
    DWORD          dwVertexBufferOffset = pContext->dp2.dwTotalOffset;

    // cache incoming vertex buffer
    pIncomingVB = pContext->ppDX8Streams[0];
    pVertexBufferBase = (LPBYTE)(pIncomingVB->getAddress());

    // temporarily override the incoming vertex buffer with the one we'll construct
    tempVB.own ((DWORD)(&PointVerts[0]), 4*((sizeof(D3DTLVERTEX))+(7*2*4)), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLEFAN;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 4 * ((sizeof(D3DTLVERTEX)) + (7*2*4));

    if (dwCount)
    {
        LPBYTE pVertices = pVertexBufferBase + dwVertexBufferOffset;
        DWORD  dwStrides = nv4DP2OpToStride[dwCachedDP2Op];

        // Calculate the new hardware state if neccessary.
        if (pContext->bStateChange) {
            if (pContext->bUseDX6Class) {
                nvSetMultiTextureHardwareState (pContext);
                if (pContext->bUseDX6Class) {
                    nvSetHardwareState (pContext);
                }
            }
            else {
                nvSetHardwareState (pContext);
            }
        }

        // Disable lower level culling while drawing lines.
        if (!pContext->bUseDX6Class) {
            dwControl = pContext->ctxInnerLoop.dwControl;
            pContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
            pContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        }
        else {
            dwControl = pContext->mtsState.dwControl0;
            pContext->mtsState.dwControl0 &= ~(DRF_MASK(NV055_CONTROL0_CULLMODE) << DRF_SHIFT(NV055_CONTROL0_CULLMODE));
            pContext->mtsState.dwControl0 |= DRF_DEF(055, _CONTROL0, _CULLMODE, _NONE);
        }
        dwOldCull = pContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        // Calculate the culling masks.
        dwCullMask1 = (dwOldCull == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
        dwCullMask2 = (dwOldCull == D3DCULL_NONE) ? 0xffffffff : 0x80000000;

        // Force first call to be to the triangle state setup routine.
        NV_FORCE_TRI_SETUP(pContext);

        dwStrides      &= 0xffffff; // mask interesting bits
        dwIndex1        = 0;
        dwIndex2        = 1;
        dwIndexStrides  = dwStrides;
        dwDwordsPerVert = pContext->pCurrentVShader->getStride() >> 2;

        // Get pointers to each of the 9 FVF point triangle vertices.
        GET_FVF_POINTER (pPointVert[0], PointVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 0, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pPointVert[1], PointVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 1, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pPointVert[2], PointVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 2, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pPointVert[3], PointVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 3, pContext->pCurrentVShader->getStride());

        // draw all triangles
        while (dwCount--)
        {
            adwIndex[0] = dwIndex1;
            adwIndex[1] = dwIndex2 + (dwIndexStrides >> 24);
            adwIndex[2] = dwIndex2 + ((dwIndexStrides >> 24) ^ 1);

            // Move on to the next triangle.
            dwIndex1       += (BYTE)(dwIndexStrides >> 8);
            dwIndex2       += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwIndexStrides << 8) & 0xff000000;

            // Get a pointer to triangle vertex
            GET_FVF_POINTER (pVert[0], pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), adwIndex[0], pContext->pCurrentVShader->getStride());
            GET_FVF_POINTER (pVert[1], pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), adwIndex[1], pContext->pCurrentVShader->getStride());
            GET_FVF_POINTER (pVert[2], pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), adwIndex[2], pContext->pCurrentVShader->getStride());

            // Since culling needs to be disabled to insure that the triangle edges get drawn properly,
            // back face culling of the triangle needs to be performed up front by software.
            IF_NOT_CULLED_BEGIN(pVert[0],pVert[1],pVert[2],dwCullValue)

                // draw each vertex, expanding it to a POINT_SIZE square
                for (DWORD dwVert=0; dwVert<3; dwVert++) {

                    // replicate data
                    for (i=0; i < dwDwordsPerVert; i++) {
                        DWORD dwValue;
                        dwValue = ((DWORD *)pVert[dwVert])[i];
                        ((DWORD *)(pPointVert[0]))[i] = dwValue;
                        ((DWORD *)(pPointVert[1]))[i] = dwValue;
                        ((DWORD *)(pPointVert[2]))[i] = dwValue;
                        ((DWORD *)(pPointVert[3]))[i] = dwValue;
                    }

                    // adjust vertex 0
                    ((LPD3DTLVERTEX)(pPointVert[0]))->sx -= HALF_POINT_SIZE;
                    ((LPD3DTLVERTEX)(pPointVert[0]))->sy -= HALF_POINT_SIZE;
                    // adjust vertex 1
                    ((LPD3DTLVERTEX)(pPointVert[1]))->sx -= HALF_POINT_SIZE;
                    ((LPD3DTLVERTEX)(pPointVert[1]))->sy += HALF_POINT_SIZE;
                    // adjust vertex 2
                    ((LPD3DTLVERTEX)(pPointVert[2]))->sx += HALF_POINT_SIZE;
                    ((LPD3DTLVERTEX)(pPointVert[2]))->sy += HALF_POINT_SIZE;
                    // adjust vertex 3
                    ((LPD3DTLVERTEX)(pPointVert[3]))->sx += HALF_POINT_SIZE;
                    ((LPD3DTLVERTEX)(pPointVert[3]))->sy -= HALF_POINT_SIZE;

                    // draw
                    nvTriangleDispatch (pContext, 2);
                }

            IF_NOT_CULLED_END
        }

        // Restore the culling mode.
        if (!pContext->bUseDX6Class)
            pContext->ctxInnerLoop.dwControl = dwControl;
        else
            pContext->mtsState.dwControl0 = dwControl;

        pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;
    }

    // reinstate the incoming vertex buffer
    pContext->ppDX8Streams[0]   = pIncomingVB;
    pContext->dp2.dwDP2Prim     = dwCachedDP2Op;
    pContext->dp2.dwTotalOffset = dwCachedTotalOffset;
    global.dwMaxVertexOffset    = dwCachedMaxOffset;

    // Force first call to be to the triangle state setup routine.
    NV_FORCE_TRI_SETUP(pContext);

    dbgTracePop();
    return;
}

//-------------------------------------------------------------------------

void nvFVFIndexedPointTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    BYTE        PointVerts[4 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    BYTE       *pPointVert[4];
    BYTE       *pVert[3];
    DWORD       i, dwDwordsPerVert;
    DWORD       adwIndex[3];
    DWORD       dwControl;
    DWORD       dwOldCull;
    PBYTE       pIndex1, pIndex2;
    DWORD       dwIndexStrides;
    DWORD       dwCullMask1;
    DWORD       dwCullMask2;
    float       dwCullValue;

    dbgTracePush ("nvFVFIndexedPointTriangle");

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    CVertexBuffer *pIncomingVB;
    CVertexBuffer  tempVB;
    LPBYTE         pVertexBufferBase;
    DWORD          dwVertexBufferOffset = pContext->dp2.dwTotalOffset;

    // cache incoming vertex buffer
    pIncomingVB = pContext->ppDX8Streams[0];
    pVertexBufferBase = (LPBYTE)(pIncomingVB->getAddress());

    // temporarily override the incoming vertex buffer with the one we'll construct
    tempVB.own ((DWORD)(&PointVerts[0]), 4*((sizeof(D3DTLVERTEX))+(7*2*4)), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLEFAN;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 4 * ((sizeof(D3DTLVERTEX)) + (7*2*4));

    if (dwCount)
    {
        LPBYTE pVertices = pVertexBufferBase + dwVertexBufferOffset;
        LPWORD pIndices  = (LPWORD)(pContext->dp2.dwIndices);
        DWORD  dwStrides = nv4DP2OpToStride[dwCachedDP2Op];

        pContext->dp2.dwIndices = NULL;

        // Calculate the new hardware state if neccessary.
        if (pContext->bStateChange) {
            if (pContext->bUseDX6Class) {
                nvSetMultiTextureHardwareState (pContext);
                if (pContext->bUseDX6Class) {
                    nvSetHardwareState (pContext);
                }
            }
            else {
                nvSetHardwareState (pContext);
            }
        }

        // Disable lower level culling while drawing lines.
        if (!pContext->bUseDX6Class) {
            dwControl = pContext->ctxInnerLoop.dwControl;
            pContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
            pContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        }
        else {
            dwControl = pContext->mtsState.dwControl0;
            pContext->mtsState.dwControl0 &= ~(DRF_MASK(NV055_CONTROL0_CULLMODE) << DRF_SHIFT(NV055_CONTROL0_CULLMODE));
            pContext->mtsState.dwControl0 |= DRF_DEF(055, _CONTROL0, _CULLMODE, _NONE);
        }
        dwOldCull = pContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        // Calculate the culling masks.
        dwCullMask1 = (dwOldCull == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
        dwCullMask2 = (dwOldCull == D3DCULL_NONE) ? 0xffffffff : 0x80000000;

        // Force first call to be to the triangle state setup routine.
        NV_FORCE_TRI_SETUP(pContext);

        dwStrides      &= 0xffffff; // mask interesting bits
        pIndex1         = (PBYTE)pIndices;
        pIndex2         = (PBYTE)pIndices + 2;
        dwIndexStrides  = dwStrides * 2;
        dwDwordsPerVert = pContext->pCurrentVShader->getStride() >> 2;

        // Get pointers to each of the 4 FVF point triangle vertices.
        GET_FVF_POINTER (pPointVert[0], PointVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 0, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pPointVert[1], PointVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 1, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pPointVert[2], PointVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 2, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pPointVert[3], PointVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 3, pContext->pCurrentVShader->getStride());

        // draw all triangles
        while (dwCount--)
        {
            adwIndex[0] = *(WORD*)(pIndex1);
            adwIndex[1] = *(WORD*)(pIndex2 +  (dwIndexStrides >> 24));
            adwIndex[2] = *(WORD*)(pIndex2 + ((dwIndexStrides >> 24) ^ 2));

            // Move on to the next triangle.
            pIndex1        += (BYTE)(dwIndexStrides >> 8);
            pIndex2        += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwIndexStrides << 8) & 0xff000000;

            // Get pointers to each vertex of current triangle.
            GET_FVF_POINTER (pVert[0], pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), adwIndex[0], pContext->pCurrentVShader->getStride());
            GET_FVF_POINTER (pVert[1], pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), adwIndex[1], pContext->pCurrentVShader->getStride());
            GET_FVF_POINTER (pVert[2], pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), adwIndex[2], pContext->pCurrentVShader->getStride());

            // Since culling needs to be disabled to insure that the triangle edges get drawn properly,
            // back face culling of the triangle needs to be performed up front by software.
            IF_NOT_CULLED_BEGIN(pVert[0],pVert[1],pVert[2],dwCullValue)

                // draw each vertex, expanding it to a POINT_SIZE square
                for (DWORD dwVert=0; dwVert<3; dwVert++) {

                    // replicate data
                    for (i=0; i < dwDwordsPerVert; i++) {
                        DWORD dwValue;
                        dwValue = ((DWORD *)pVert[dwVert])[i];
                        ((DWORD *)(pPointVert[0]))[i] = dwValue;
                        ((DWORD *)(pPointVert[1]))[i] = dwValue;
                        ((DWORD *)(pPointVert[2]))[i] = dwValue;
                        ((DWORD *)(pPointVert[3]))[i] = dwValue;
                    }

                    // adjust vertex 0
                    ((LPD3DTLVERTEX)(pPointVert[0]))->sx -= HALF_POINT_SIZE;
                    ((LPD3DTLVERTEX)(pPointVert[0]))->sy -= HALF_POINT_SIZE;
                    // adjust vertex 1
                    ((LPD3DTLVERTEX)(pPointVert[1]))->sx -= HALF_POINT_SIZE;
                    ((LPD3DTLVERTEX)(pPointVert[1]))->sy += HALF_POINT_SIZE;
                    // adjust vertex 2
                    ((LPD3DTLVERTEX)(pPointVert[2]))->sx += HALF_POINT_SIZE;
                    ((LPD3DTLVERTEX)(pPointVert[2]))->sy += HALF_POINT_SIZE;
                    // adjust vertex 3
                    ((LPD3DTLVERTEX)(pPointVert[3]))->sx += HALF_POINT_SIZE;
                    ((LPD3DTLVERTEX)(pPointVert[3]))->sy -= HALF_POINT_SIZE;

                    // draw
                    nvTriangleDispatch (pContext, 2);

                }

            IF_NOT_CULLED_END

        }

        // Restore the culling mode.
        if (!pContext->bUseDX6Class)
            pContext->ctxInnerLoop.dwControl = dwControl;
        else
            pContext->mtsState.dwControl0 = dwControl;

        pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;
    }

    // reinstate the incoming vertex buffer
    pContext->ppDX8Streams[0]   = pIncomingVB;
    pContext->dp2.dwDP2Prim     = dwCachedDP2Op;
    pContext->dp2.dwTotalOffset = dwCachedTotalOffset;
    global.dwMaxVertexOffset    = dwCachedMaxOffset;

    // Force first call to be to the triangle state setup routine.
    NV_FORCE_TRI_SETUP(pContext);

    dbgTracePop();
    return;
}

#endif  // NVARCH >= 0x04

