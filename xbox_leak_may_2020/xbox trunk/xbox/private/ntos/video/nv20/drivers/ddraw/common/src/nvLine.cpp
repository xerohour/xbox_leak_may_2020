/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvLine.cpp                                                        *
*   The Direct3D line rendering routines.                                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       04/12/97 - created                      *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

static WORD awWireframeIndices[6*3] =
{
    0,  1,  2,
    1,  2,  3,
    4,  5,  6,
    5,  6,  7,
    8,  9, 10,
    9, 10, 11
};

// true if line is more vertical than horizontal
#define Y_MAJOR(a,b)     (fabs(LineVerts[a].sx - LineVerts[b].sx) < fabs(LineVerts[a].sy - LineVerts[b].sy))
#define Y_MAJOR_FVF(a,b) (fabs (((LPD3DTLVERTEX)(pLineVert[a]))->sx - ((LPD3DTLVERTEX)(pLineVert[b]))->sx)  <   \
                          fabs (((LPD3DTLVERTEX)(pLineVert[a]))->sy - ((LPD3DTLVERTEX)(pLineVert[b]))->sy))

//---------------------------------------------------------------------------

/*
 * Non-Indexed TLVertex Format Line Lists.
 */
void nvDrawLine
(
    NV_INNERLOOP_ARGLIST
)
{
    DWORD           dwControl;
    DWORD           dwOldCull;
    D3DTLVERTEX     LineVerts[4];

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
    tempVB.own ((DWORD)(&LineVerts[0]), 4*sizeof(D3DTLVERTEX), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLESTRIP;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 4*sizeof(D3DTLVERTEX);

    if (dwCount)
    {
        LPD3DTLVERTEX pVertices   = (LPD3DTLVERTEX)(pVertexBufferBase + dwVertexBufferOffset);
        DWORD         dwVertexInc = ((dwCachedDP2Op == D3DDP2OP_INDEXEDLINESTRIP) || (dwCachedDP2Op == D3DDP2OP_LINESTRIP)) ? 1 : 2;

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

        // Disable culling.
        dwControl = pContext->ctxInnerLoop.dwControl;
        pContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
        pContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        dwOldCull = pContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        // Force first call to be to the triangle state setup routine.
        NV_FORCE_TRI_SETUP(pContext);

        // Draw the lines.
        while (dwCount--)
        {
            // Each line is rendered as a one pixel wide rectangle.
            LineVerts[0] = pVertices[0];
            LineVerts[2] = pVertices[0];
            LineVerts[1] = pVertices[1];
            LineVerts[3] = pVertices[1];
            pVertices += dwVertexInc;

            if (Y_MAJOR(0,1))
            {
                LineVerts[0].sx -= HALF_LINE_WIDTH;
                LineVerts[1].sx -= HALF_LINE_WIDTH;
                LineVerts[2].sx += HALF_LINE_WIDTH;
                LineVerts[3].sx += HALF_LINE_WIDTH;
            }
            else
            {
                LineVerts[0].sy -= HALF_LINE_WIDTH;
                LineVerts[1].sy -= HALF_LINE_WIDTH;
                LineVerts[2].sy += HALF_LINE_WIDTH;
                LineVerts[3].sy += HALF_LINE_WIDTH;
            }

            // Call the low level rendering routine to draw the "line".
            nvTriangleDispatch (pContext, 2);
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

    // Force next render call to be to the triangle state setup routine.
    NV_FORCE_TRI_SETUP(pContext);
    return;
}

//-------------------------------------------------------------------------

/*
 * Indexed TLVertex Format Line Lists.
 */
void nvIndexedLine
(
    NV_INNERLOOP_ARGLIST
)
{
    DWORD           dwControl;
    DWORD           dwOldCull;
    D3DTLVERTEX     LineVerts[4];

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
    tempVB.own ((DWORD)(&LineVerts[0]), 4*sizeof(D3DTLVERTEX), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLESTRIP;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 4*sizeof(D3DTLVERTEX);

    if (dwCount)
    {
        LPD3DTLVERTEX pVertices  = (LPD3DTLVERTEX)(pVertexBufferBase + dwVertexBufferOffset);
        LPWORD        pIndices   = (LPWORD)(pContext->dp2.dwIndices);
        DWORD         dwIndexInc = (dwCachedDP2Op == D3DDP2OP_INDEXEDLINESTRIP) ? 1 : 2;

        pContext->dp2.dwIndices = NULL;

        // Calculate the new hardware state if neccessary.
        if (pContext->bStateChange) {
            if (pContext->bUseDX6Class) {
                nvSetMultiTextureHardwareState (pContext);
                if (pContext->bUseDX6Class)
                    nvSetHardwareState (pContext);
            }
            else {
                nvSetHardwareState (pContext);
            }
        }

        // Disable culling.
        dwControl = pContext->ctxInnerLoop.dwControl;
        pContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
        pContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        dwOldCull = pContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        // Force first call to be to the triangle state setup routine.
        NV_FORCE_TRI_SETUP(pContext);

        // Draw the lines.
        while (dwCount--)
        {
            // Each line is rendered as a one pixel wide rectangle.

#ifdef WINNT
            // we're responsible for validating data under NT. ugh.
            DWORD dwLocalOffset;
            LPD3DTLVERTEX pVert;
            dwLocalOffset = pIndices[0] * sizeof(D3DTLVERTEX);

            if ((dwVertexBufferOffset + dwLocalOffset) < dwCachedMaxOffset) {
                pVert   =(LPD3DTLVERTEX) ((DWORD)pVertices + dwLocalOffset);
                LineVerts[0] = *pVert;
                LineVerts[2] = *pVert;
            }
            else {
                DPF ("vertex data is out of range in nvIndexedLine");
                dbgD3DError();
                LineVerts[0] = pVertices[0];
                LineVerts[2] = pVertices[0];
            }
            dwLocalOffset = pIndices[1] * sizeof(D3DTLVERTEX);
            if ((dwVertexBufferOffset + dwLocalOffset) < dwCachedMaxOffset) {
                pVert   =(LPD3DTLVERTEX) ((DWORD)pVertices + dwLocalOffset);
                LineVerts[1] = *pVert;
                LineVerts[3] = *pVert;
            }
            else {
                DPF ("vertex data is out of range in nvIndexedLine");
                dbgD3DError();
                LineVerts[1] = pVertices[0];
                LineVerts[3] = pVertices[0];
            }
#else  // !WINNT

            LineVerts[0] = pVertices[pIndices[0]];
            LineVerts[2] = pVertices[pIndices[0]];
            LineVerts[1] = pVertices[pIndices[1]];
            LineVerts[3] = pVertices[pIndices[1]];
#endif  // !WINNT

            pIndices += dwIndexInc;

            if (Y_MAJOR(0,1))
            {
                LineVerts[0].sx -= HALF_LINE_WIDTH;
                LineVerts[1].sx -= HALF_LINE_WIDTH;
                LineVerts[2].sx += HALF_LINE_WIDTH;
                LineVerts[3].sx += HALF_LINE_WIDTH;
            }
            else
            {
                LineVerts[0].sy -= HALF_LINE_WIDTH;
                LineVerts[1].sy -= HALF_LINE_WIDTH;
                LineVerts[2].sy += HALF_LINE_WIDTH;
                LineVerts[3].sy += HALF_LINE_WIDTH;
            }

            // Call the low level rendering routine to draw the "line".
            nvTriangleDispatch (pContext, 2);
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

    return;
}

//-------------------------------------------------------------------------

DWORD nvRenderLine
(
    NV_INNERLOOP_ARGLIST
)
{
    nvIndexedLine (NV_INNERLOOP_ARGS);
    return (DD_OK);
}

//-------------------------------------------------------------------------

void nvDrawWireframeTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    D3DTLVERTEX LineVerts[12];
    DWORD       v0,v1,v2;
    DWORD       dwControl;
    DWORD       dwOldCull;
    DWORD       dwIndex1, dwIndex2;
    DWORD       dwIndexStrides;
    DWORD       dwCullMask1;
    DWORD       dwCullMask2;
    DWORD       dwEdgeFlags, dwEdgeMask;
    float       dwCullValue;

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
    tempVB.own ((DWORD)(&LineVerts[0]), 12*sizeof(D3DTLVERTEX), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_INDEXEDTRIANGLELIST2;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 12*sizeof(D3DTLVERTEX);

    dwEdgeFlags = pContext->dp2.dwEdgeFlags;
    dwEdgeMask  = 0x1;

    if (dwCount) {

        LPD3DTLVERTEX pVertices = (LPD3DTLVERTEX)(pVertexBufferBase + dwVertexBufferOffset);
        DWORD         dwStrides = nv4DP2OpToStride[dwCachedDP2Op];

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
        dwIndex1       = 0;
        dwIndex2       = 1;
        dwIndexStrides = dwStrides;

        // Draw all triangles as a wireframe.
        nvAssert (dwCount <= 0xffff);
        WORD  wPrim = dwCount;  // save the number of primitives
        WORD  wTri, wTriangleNum;
        WORD  awMaskedIndices[6*3];

        while (dwCount--)
        {
            wTriangleNum = wPrim-dwCount; // (1...nPrim)

            v0 = dwIndex1;
            v1 = dwIndex2 + (dwIndexStrides >> 24);
            v2 = dwIndex2 + ((dwIndexStrides >> 24) ^ 1);

            // Move on to the next triangle.
            dwIndex1       += (BYTE)(dwIndexStrides >> 8);
            dwIndex2       += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwStrides << 8) & 0xff000000;

            // Since culling needs to be disabled to insure that the triangle edges get drawn properly,
            // back face culling of the triangle needs to be performed up front by software.
            IF_NOT_CULLED_BEGIN ((BYTE *)&pVertices[v0], (BYTE *)&pVertices[v1], (BYTE *)&pVertices[v2], dwCullValue)

                // render each edge as a "line" (thin rectangle)
                LineVerts[ 0] = pVertices[v0];
                LineVerts[ 2] = pVertices[v0];
                LineVerts[ 9] = pVertices[v0];
                LineVerts[11] = pVertices[v0];

                LineVerts[ 1] = pVertices[v1];
                LineVerts[ 3] = pVertices[v1];
                LineVerts[ 4] = pVertices[v1];
                LineVerts[ 6] = pVertices[v1];

                LineVerts[ 5] = pVertices[v2];
                LineVerts[ 7] = pVertices[v2];
                LineVerts[ 8] = pVertices[v2];
                LineVerts[10] = pVertices[v2];

                // fd. a bit uggly here, but I don't know the correct answer,
                // that makes WHQL happy.

                if ((dwCachedDP2Op!=D3DDP2OP_TRIANGLESTRIP)
                    && (pContext->dwRenderState[D3DRS_SHADEMODE]==D3DSHADE_FLAT))
                {
                    DWORD color=pVertices[v0].color;

                    LineVerts[ 1].color = color;
                    LineVerts[ 3].color = color;
                    LineVerts[ 4].color = color;
                    LineVerts[ 6].color = color;

                    LineVerts[ 5].color = color;
                    LineVerts[ 7].color = color;
                    LineVerts[ 8].color = color;
                    LineVerts[10].color = color;
                }

                if (Y_MAJOR(0,1))
                {
                    LineVerts[ 0].sx -= HALF_LINE_WIDTH;
                    LineVerts[ 1].sx -= HALF_LINE_WIDTH;
                    LineVerts[ 2].sx += HALF_LINE_WIDTH;
                    LineVerts[ 3].sx += HALF_LINE_WIDTH;
                }
                else
                {
                    LineVerts[ 0].sy -= HALF_LINE_WIDTH;
                    LineVerts[ 1].sy -= HALF_LINE_WIDTH;
                    LineVerts[ 2].sy += HALF_LINE_WIDTH;
                    LineVerts[ 3].sy += HALF_LINE_WIDTH;
                }

                if (Y_MAJOR(4,5))
                {
                    LineVerts[ 4].sx -= HALF_LINE_WIDTH;
                    LineVerts[ 5].sx -= HALF_LINE_WIDTH;
                    LineVerts[ 6].sx += HALF_LINE_WIDTH;
                    LineVerts[ 7].sx += HALF_LINE_WIDTH;
                }
                else
                {
                    LineVerts[ 4].sy -= HALF_LINE_WIDTH;
                    LineVerts[ 5].sy -= HALF_LINE_WIDTH;
                    LineVerts[ 6].sy += HALF_LINE_WIDTH;
                    LineVerts[ 7].sy += HALF_LINE_WIDTH;
                }

                if (Y_MAJOR(8,9))
                {
                    LineVerts[ 8].sx -= HALF_LINE_WIDTH;
                    LineVerts[ 9].sx -= HALF_LINE_WIDTH;
                    LineVerts[10].sx += HALF_LINE_WIDTH;
                    LineVerts[11].sx += HALF_LINE_WIDTH;
                }
                else
                {
                    LineVerts[ 8].sy -= HALF_LINE_WIDTH;
                    LineVerts[ 9].sy -= HALF_LINE_WIDTH;
                    LineVerts[10].sy += HALF_LINE_WIDTH;
                    LineVerts[11].sy += HALF_LINE_WIDTH;
                }

                // if D3DDP2OP_TRIANGLEFAN_IMM, decide which edges are drawn and which are not
                // by inference from the reference rasterizer, the rules appear to be:
                // - interior spokes are NEVER drawn
                // - bit0 controls edge 0->1   where 0 is center vertex
                // - bitN controls edge N->N+1 where N = 1...triangle_count
                // - bitX controls edge X->0   where X = triangle_count+1

                if (dwCachedDP2Op == D3DDP2OP_TRIANGLEFAN_IMM)
                {
                    wTri = 0;

                    // v0-v1
                    // we never draw this edge for anything but the first triangle
                    if (wTriangleNum == 1) {
                        if (dwEdgeFlags & dwEdgeMask) {
                            awMaskedIndices[wTri*3+0] = awWireframeIndices[0*3+0];
                            awMaskedIndices[wTri*3+1] = awWireframeIndices[0*3+1];
                            awMaskedIndices[wTri*3+2] = awWireframeIndices[0*3+2];
                            wTri++;
                            awMaskedIndices[wTri*3+0] = awWireframeIndices[1*3+0];
                            awMaskedIndices[wTri*3+1] = awWireframeIndices[1*3+1];
                            awMaskedIndices[wTri*3+2] = awWireframeIndices[1*3+2];
                            wTri++;
                        }
                        dwEdgeMask <<= 1;
                    }

                    // v1-v2
                    if (dwEdgeFlags & dwEdgeMask) {
                        awMaskedIndices[wTri*3+0] = awWireframeIndices[2*3+0];
                        awMaskedIndices[wTri*3+1] = awWireframeIndices[2*3+1];
                        awMaskedIndices[wTri*3+2] = awWireframeIndices[2*3+2];
                        wTri++;
                        awMaskedIndices[wTri*3+0] = awWireframeIndices[3*3+0];
                        awMaskedIndices[wTri*3+1] = awWireframeIndices[3*3+1];
                        awMaskedIndices[wTri*3+2] = awWireframeIndices[3*3+2];
                        wTri++;
                    }
                    dwEdgeMask <<= 1;

                    // v2-v0
                    // we never draw this edge for anything but the last triangle
                    if (wTriangleNum == wPrim) {
                        if (dwEdgeFlags & dwEdgeMask) {
                            awMaskedIndices[wTri*3+0] = awWireframeIndices[4*3+0];
                            awMaskedIndices[wTri*3+1] = awWireframeIndices[4*3+1];
                            awMaskedIndices[wTri*3+2] = awWireframeIndices[4*3+2];
                            wTri++;
                            awMaskedIndices[wTri*3+0] = awWireframeIndices[5*3+0];
                            awMaskedIndices[wTri*3+1] = awWireframeIndices[5*3+1];
                            awMaskedIndices[wTri*3+2] = awWireframeIndices[5*3+2];
                            wTri++;
                        }
                        dwEdgeMask <<= 1;
                    }

                    pContext->dp2.dwIndices = (DWORD)(awMaskedIndices);

                    // Draw the specified triangle edges.
                    nvTriangleDispatch (pContext, wTri);

                }

                else
                {
                    pContext->dp2.dwIndices = (DWORD)(awWireframeIndices);

                    // Draw all triangle edges
                    nvTriangleDispatch (pContext, 6);
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
    return;
}

//-------------------------------------------------------------------------

void nvIndexedWireframeTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    D3DTLVERTEX LineVerts[12];
    DWORD       v0,v1,v2;
    DWORD       dwControl;
    DWORD       dwOldCull;
    DWORD       dwIndex1, dwIndex2;
    DWORD       dwIndexStrides;
    DWORD       dwCullMask1;
    DWORD       dwCullMask2;
    float       dwCullValue;

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
    tempVB.own ((DWORD)(&LineVerts[0]), 12*sizeof(D3DTLVERTEX), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_INDEXEDTRIANGLELIST2;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 12*sizeof(D3DTLVERTEX);

    if (dwCount)
    {
        LPD3DTLVERTEX pVertices = (LPD3DTLVERTEX)(pVertexBufferBase + dwVertexBufferOffset);
        LPWORD        pIndices  = (LPWORD)(pContext->dp2.dwIndices);
        DWORD         dwStrides = nv4DP2OpToStride[dwCachedDP2Op];

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
        dwIndex1       = (DWORD)pIndices;
        dwIndex2       = (DWORD)pIndices + 2;
        dwIndexStrides = dwStrides * 2;

        // Draw all triangles as a wireframe.
        while (dwCount--)
        {
            v0 = *(WORD*) dwIndex1;
            v1 = *(WORD*)(dwIndex2 +  (dwIndexStrides >> 24));
            v2 = *(WORD*)(dwIndex2 + ((dwIndexStrides >> 24) ^ 2));

            // Move on to the next triangle.
            dwIndex1       += (BYTE)(dwIndexStrides >> 8);
            dwIndex2       += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwIndexStrides << 8) & 0xff000000;

            // Since culling needs to be disabled to insure that the triangle edges get drawn properly,
            // back face culling of the triangle needs to be performed up front by software.
            IF_NOT_CULLED_BEGIN ((BYTE *)&pVertices[v0], (BYTE *)&pVertices[v1], (BYTE *)&pVertices[v2], dwCullValue)

                // render each edge as a "line" (thin rectangle)
                LineVerts[ 0] = pVertices[v0];
                LineVerts[ 2] = pVertices[v0];
                LineVerts[ 9] = pVertices[v0];
                LineVerts[11] = pVertices[v0];

                LineVerts[ 1] = pVertices[v1];
                LineVerts[ 3] = pVertices[v1];
                LineVerts[ 4] = pVertices[v1];
                LineVerts[ 6] = pVertices[v1];

                LineVerts[ 5] = pVertices[v2];
                LineVerts[ 7] = pVertices[v2];
                LineVerts[ 8] = pVertices[v2];
                LineVerts[10] = pVertices[v2];

                if (pContext->dwRenderState[D3DRS_SHADEMODE]==D3DSHADE_FLAT)
                {
                    DWORD color=pVertices[v0].color;
                    LineVerts[ 1].color = color;
                    LineVerts[ 3].color = color;
                    LineVerts[ 4].color = color;
                    LineVerts[ 6].color = color;

                    LineVerts[ 5].color = color;
                    LineVerts[ 7].color = color;
                    LineVerts[ 8].color = color;
                    LineVerts[10].color = color;
                }

                if (Y_MAJOR(0,1))
                {
                    LineVerts[ 0].sx -= HALF_LINE_WIDTH;
                    LineVerts[ 1].sx -= HALF_LINE_WIDTH;
                    LineVerts[ 2].sx += HALF_LINE_WIDTH;
                    LineVerts[ 3].sx += HALF_LINE_WIDTH;
                }
                else
                {
                    LineVerts[ 0].sy -= HALF_LINE_WIDTH;
                    LineVerts[ 1].sy -= HALF_LINE_WIDTH;
                    LineVerts[ 2].sy += HALF_LINE_WIDTH;
                    LineVerts[ 3].sy += HALF_LINE_WIDTH;
                }

                if (Y_MAJOR(4,5))
                {
                    LineVerts[ 4].sx -= HALF_LINE_WIDTH;
                    LineVerts[ 5].sx -= HALF_LINE_WIDTH;
                    LineVerts[ 6].sx += HALF_LINE_WIDTH;
                    LineVerts[ 7].sx += HALF_LINE_WIDTH;
                }
                else
                {
                    LineVerts[ 4].sy -= HALF_LINE_WIDTH;
                    LineVerts[ 5].sy -= HALF_LINE_WIDTH;
                    LineVerts[ 6].sy += HALF_LINE_WIDTH;
                    LineVerts[ 7].sy += HALF_LINE_WIDTH;
                }

                if (Y_MAJOR(8,9))
                {
                    LineVerts[ 8].sx -= HALF_LINE_WIDTH;
                    LineVerts[ 9].sx -= HALF_LINE_WIDTH;
                    LineVerts[10].sx += HALF_LINE_WIDTH;
                    LineVerts[11].sx += HALF_LINE_WIDTH;
                }
                else
                {
                    LineVerts[ 8].sy -= HALF_LINE_WIDTH;
                    LineVerts[ 9].sy -= HALF_LINE_WIDTH;
                    LineVerts[10].sy += HALF_LINE_WIDTH;
                    LineVerts[11].sy += HALF_LINE_WIDTH;
                }

                // If this was called with RenderPrim, dwStrides will be LEGACY_STRIDES, otherwise it won't.
                if ((dwStrides != (LEGACY_STRIDES & 0xffffff)) ||
                    (( ((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLETRIANGLE) == D3DTRIFLAG_EDGEENABLETRIANGLE))
                {
                    pContext->dp2.dwIndices = (DWORD)(awWireframeIndices);
                    // Draw the triangle edges.
                    nvTriangleDispatch (pContext, 6);
                }

                else
                {
                    WORD wTri;
                    WORD awMaskedIndices[6*3];

                    wTri = 0;
                    if (((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLE1)
                    {
                        awMaskedIndices[0*3+0] = awWireframeIndices[0*3+0];
                        awMaskedIndices[0*3+1] = awWireframeIndices[0*3+1];
                        awMaskedIndices[0*3+2] = awWireframeIndices[0*3+2];
                        wTri++;
                        awMaskedIndices[1*3+0] = awWireframeIndices[1*3+0];
                        awMaskedIndices[1*3+1] = awWireframeIndices[1*3+1];
                        awMaskedIndices[1*3+2] = awWireframeIndices[1*3+2];
                        wTri++;
                    }
                    if (((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLE2)
                    {
                        awMaskedIndices[wTri*3+0] = awWireframeIndices[2*3+0];
                        awMaskedIndices[wTri*3+1] = awWireframeIndices[2*3+1];
                        awMaskedIndices[wTri*3+2] = awWireframeIndices[2*3+2];
                        wTri++;
                        awMaskedIndices[wTri*3+0] = awWireframeIndices[3*3+0];
                        awMaskedIndices[wTri*3+1] = awWireframeIndices[3*3+1];
                        awMaskedIndices[wTri*3+2] = awWireframeIndices[3*3+2];
                        wTri++;
                    }
                    if (((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLE3)
                    {
                        awMaskedIndices[wTri*3+0] = awWireframeIndices[4*3+0];
                        awMaskedIndices[wTri*3+1] = awWireframeIndices[4*3+1];
                        awMaskedIndices[wTri*3+2] = awWireframeIndices[4*3+2];
                        wTri++;
                        awMaskedIndices[wTri*3+0] = awWireframeIndices[5*3+0];
                        awMaskedIndices[wTri*3+1] = awWireframeIndices[5*3+1];
                        awMaskedIndices[wTri*3+2] = awWireframeIndices[5*3+2];
                        wTri++;
                    }

                    if (wTri)
                    {
                        pContext->dp2.dwIndices = (DWORD)(awMaskedIndices);
                        // Draw the specified triangle edges.
                        nvTriangleDispatch (pContext, wTri);
                    }
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
    return;
}

//-------------------------------------------------------------------------

/*
 * Non-Indexed Flexible Vertex Format Lines.
 */
void nvFVFDrawLine
(
    NV_INNERLOOP_ARGLIST
)
{
    BYTE   LineVerts[4 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    BYTE  *pLineVert[4];
    BYTE  *pVert0, *pVert1;
    DWORD  i, dwDwordsPerVert;
    DWORD  dwControl;
    DWORD  dwOldCull;
    DWORD  dwVertexStride;
    DWORD  dwNextLine;

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
    tempVB.own ((DWORD)(&LineVerts[0]), 4*((sizeof(D3DTLVERTEX))+(7*2*4)), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLESTRIP;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 4 * ((sizeof(D3DTLVERTEX)) + (7*2*4));

    if (dwCount)
    {
        LPBYTE pVertices   = pVertexBufferBase + dwVertexBufferOffset;
        DWORD  dwVertexInc = ((dwCachedDP2Op == D3DDP2OP_INDEXEDLINESTRIP) || (dwCachedDP2Op == D3DDP2OP_LINESTRIP)) ? 1 : 2;

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

        // Disable culling.
        if (!pContext->bUseDX6Class)
        {
            dwControl = pContext->ctxInnerLoop.dwControl;
            pContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
            pContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        }
        else
        {
            dwControl = pContext->mtsState.dwControl0;
            pContext->mtsState.dwControl0 &= ~(DRF_MASK(NV055_CONTROL0_CULLMODE) << DRF_SHIFT(NV055_CONTROL0_CULLMODE));
            pContext->mtsState.dwControl0 |= DRF_DEF(055, _CONTROL0, _CULLMODE, _NONE);
        }
        dwOldCull = pContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        // Force first call to be to the triangle state setup routine.
        NV_FORCE_TRI_SETUP(pContext);

        // Calculate number of vertex bytes to increment after each line.
        dwVertexStride  = pContext->pCurrentVShader->getStride();
        dwNextLine      = dwVertexInc * dwVertexStride;
        dwDwordsPerVert = pContext->pCurrentVShader->getStride() >> 2;

        // Get pointers to each of the 4 FVF line vertices.
        GET_FVF_POINTER (pLineVert[0], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 0, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[1], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 1, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[2], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 2, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[3], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 3, pContext->pCurrentVShader->getStride());

        // Draw the lines.
        while (dwCount--)
        {
            // Each line is rendered as a one pixel wide rectangle.
            // Since we're contstructing new TLVERTEX structures anyway,
            // Construct full TLVERTEX structures up front from the FVF data
            // structures and then just call the TLVERTEX rendering loop.

            // Get pointers to each vertex of current line.
            GET_FVF_POINTER (pVert0, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 0, pContext->pCurrentVShader->getStride());
            GET_FVF_POINTER (pVert1, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 1, pContext->pCurrentVShader->getStride());
            pVertices += dwNextLine;

            // copy vertex data
            for (i = 0; i < dwDwordsPerVert; i++)
            {
                DWORD   dwValue;
                dwValue = ((DWORD *)pVert0)[i];
                ((DWORD *)(pLineVert[0]))[i] = dwValue;
                ((DWORD *)(pLineVert[2]))[i] = dwValue;

                dwValue = ((DWORD *)pVert1)[i];
                ((DWORD *)(pLineVert[1]))[i] = dwValue;
                ((DWORD *)(pLineVert[3]))[i] = dwValue;
            }

            if (Y_MAJOR_FVF(0,1))
            {
                ((LPD3DTLVERTEX)(pLineVert[0]))->sx -= HALF_LINE_WIDTH;
                ((LPD3DTLVERTEX)(pLineVert[1]))->sx -= HALF_LINE_WIDTH;
                ((LPD3DTLVERTEX)(pLineVert[2]))->sx += HALF_LINE_WIDTH;
                ((LPD3DTLVERTEX)(pLineVert[3]))->sx += HALF_LINE_WIDTH;
            }
            else
            {
                ((LPD3DTLVERTEX)(pLineVert[0]))->sy -= HALF_LINE_WIDTH;
                ((LPD3DTLVERTEX)(pLineVert[1]))->sy -= HALF_LINE_WIDTH;
                ((LPD3DTLVERTEX)(pLineVert[2]))->sy += HALF_LINE_WIDTH;
                ((LPD3DTLVERTEX)(pLineVert[3]))->sy += HALF_LINE_WIDTH;
            }

            // Call the low level rendering routine to draw the "line".
            nvTriangleDispatch (pContext, 2);
        }  // while (dwCount)

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

    // Force next render call to be to the triangle state setup routine.
    NV_FORCE_TRI_SETUP(pContext);
    return;
}

//-------------------------------------------------------------------------

/*
 * Indexed Flexible Vertex Format Lines.
 */
void nvFVFIndexedLine
(
    NV_INNERLOOP_ARGLIST
)
{
    BYTE   LineVerts[4 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    BYTE  *pLineVert[4];
    BYTE  *pVert0, *pVert1;
    DWORD  i, dwDwordsPerVert;
    DWORD  dwControl;
    DWORD  dwOldCull;

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
    tempVB.own ((DWORD)(&LineVerts[0]), 4*((sizeof(D3DTLVERTEX))+(7*2*4)), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLESTRIP;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 4 * ((sizeof(D3DTLVERTEX)) + (7*2*4));

    if (dwCount)
    {
        LPBYTE pVertices  = pVertexBufferBase + dwVertexBufferOffset;
        LPWORD pIndices   = (LPWORD)(pContext->dp2.dwIndices);
        DWORD  dwIndexInc = (dwCachedDP2Op == D3DDP2OP_INDEXEDLINESTRIP) ? 1 : 2;

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

        // Disable culling.
        if (!pContext->bUseDX6Class)
        {
            dwControl = pContext->ctxInnerLoop.dwControl;
            pContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
            pContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        }
        else
        {
            dwControl = pContext->mtsState.dwControl0;
            pContext->mtsState.dwControl0 &= ~(DRF_MASK(NV055_CONTROL0_CULLMODE) << DRF_SHIFT(NV055_CONTROL0_CULLMODE));
            pContext->mtsState.dwControl0 |= DRF_DEF(055, _CONTROL0, _CULLMODE, _NONE);
        }
        dwOldCull = pContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        // Force first call to be to the triangle state setup routine.
        NV_FORCE_TRI_SETUP(pContext);

        dwDwordsPerVert = pContext->pCurrentVShader->getStride() >> 2;

        // Get pointers to each of the 4 vertices of the rectangle that
        // will approximate the line
        GET_FVF_POINTER (pLineVert[ 0], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 0, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 1], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 1, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 2], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 2, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 3], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 3, pContext->pCurrentVShader->getStride());

        // Draw the lines.
        while (dwCount--)
        {
            // Each line is rendered as a one pixel wide rectangle.
            // Since we're contstructing new TLVERTEX structures anyway,
            // Construct full TLVERTEX structures up front from the FVF data
            // structures and then just call the TLVERTEX rendering loop.

            // Get pointers to each vertex of current line.
            GET_FVF_POINTER (pVert0, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), pIndices[0], pContext->pCurrentVShader->getStride());
            GET_FVF_POINTER (pVert1, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), pIndices[1], pContext->pCurrentVShader->getStride());

            pIndices += dwIndexInc;

            // Copy vertex data
            for (i = 0; i < dwDwordsPerVert; i++)
            {
                DWORD dwValue;
                dwValue = ((DWORD *)pVert0)[i];
                ((DWORD *)(pLineVert[0]))[i] = dwValue;
                ((DWORD *)(pLineVert[2]))[i] = dwValue;

                dwValue = ((DWORD *)pVert1)[i];
                ((DWORD *)(pLineVert[1]))[i] = dwValue;
                ((DWORD *)(pLineVert[3]))[i] = dwValue;
            }

            if (Y_MAJOR_FVF(0,1))
            {
                ((LPD3DTLVERTEX)(pLineVert[0]))->sx -= HALF_LINE_WIDTH;
                ((LPD3DTLVERTEX)(pLineVert[1]))->sx -= HALF_LINE_WIDTH;
                ((LPD3DTLVERTEX)(pLineVert[2]))->sx += HALF_LINE_WIDTH;
                ((LPD3DTLVERTEX)(pLineVert[3]))->sx += HALF_LINE_WIDTH;
            }
            else
            {
                ((LPD3DTLVERTEX)(pLineVert[0]))->sy -= HALF_LINE_WIDTH;
                ((LPD3DTLVERTEX)(pLineVert[1]))->sy -= HALF_LINE_WIDTH;
                ((LPD3DTLVERTEX)(pLineVert[2]))->sy += HALF_LINE_WIDTH;
                ((LPD3DTLVERTEX)(pLineVert[3]))->sy += HALF_LINE_WIDTH;
            }

            // Call the low level rendering routine to draw the "line".
            nvTriangleDispatch (pContext, 2);
        }  // while (dwCount)

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

    // Force next render call to be to the triangle state setup routine.
    NV_FORCE_TRI_SETUP(pContext);
    return;
}

//-------------------------------------------------------------------------

void nvFVFDrawWireframeTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    BYTE        LineVerts[12 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    BYTE       *pLineVert[12];
    BYTE       *pVert0, *pVert1, *pVert2;
    DWORD       i, dwDwordsPerVert;
    DWORD       v0,v1,v2;
    DWORD       dwControl;
    DWORD       dwOldCull;
    DWORD       dwIndex1, dwIndex2;
    DWORD       dwIndexStrides;
    DWORD       dwCullMask1;
    DWORD       dwCullMask2;
    DWORD       dwEdgeFlags, dwEdgeMask;
    float       dwCullValue;

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
    tempVB.own ((DWORD)(&LineVerts[0]), 12*((sizeof(D3DTLVERTEX))+(7*2*4)), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_INDEXEDTRIANGLELIST2;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 12 * ((sizeof(D3DTLVERTEX)) + (7*2*4));

    dwEdgeFlags = pContext->dp2.dwEdgeFlags;
    dwEdgeMask  = 0x1;

    if (dwCount) {

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
        if (!pContext->bUseDX6Class)
        {
            dwControl = pContext->ctxInnerLoop.dwControl;
            pContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
            pContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        }
        else
        {
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

        // Get pointers to each of the 12 FVF wireframe line vertices.
        GET_FVF_POINTER (pLineVert[ 0], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 0, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 1], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 1, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 2], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 2, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 3], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 3, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 4], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 4, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 5], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 5, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 6], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 6, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 7], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 7, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 8], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 8, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 9], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 9, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[10], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 10, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[11], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 11, pContext->pCurrentVShader->getStride());

        // Draw all triangles as a wireframe.
        nvAssert (dwCount <= 0xffff);
        WORD  wPrim = dwCount;  // save the number of primitives
        WORD  wTri, wTriangleNum;
        WORD  awMaskedIndices[6*3];

        while (dwCount--)
        {
            wTriangleNum = wPrim-dwCount; // (1...nPrim)

            v0 = dwIndex1;
            v1 = dwIndex2 + (dwIndexStrides >> 24);
            v2 = dwIndex2 + ((dwIndexStrides >> 24) ^ 1);

            // Move on to the next triangle.
            dwIndex1       += (BYTE)(dwIndexStrides >> 8);
            dwIndex2       += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwStrides << 8) & 0xff000000;

            // Each edge line is rendered as a one pixel wide rectangle.

            // Get pointers to each vertex of current triangle.
            GET_FVF_POINTER (pVert0, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), v0, pContext->pCurrentVShader->getStride());
            GET_FVF_POINTER (pVert1, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), v1, pContext->pCurrentVShader->getStride());
            GET_FVF_POINTER (pVert2, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), v2, pContext->pCurrentVShader->getStride());

            // Since culling needs to be disabled to insure that the triangle edges get drawn properly,
            // back face culling of the triangle needs to be performed up front by software.
            IF_NOT_CULLED_BEGIN (pVert0, pVert1, pVert2, dwCullValue)

                // copy vertex data
                for (i = 0; i < dwDwordsPerVert; i++)
                {
                    DWORD dwValue;
                    dwValue = ((DWORD *)pVert0)[i];
                    ((DWORD *)(pLineVert[ 0]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 2]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 9]))[i] = dwValue;
                    ((DWORD *)(pLineVert[11]))[i] = dwValue;

                    dwValue = ((DWORD *)pVert1)[i];
                    ((DWORD *)(pLineVert[ 1]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 3]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 4]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 6]))[i] = dwValue;

                    dwValue = ((DWORD *)pVert2)[i];
                    ((DWORD *)(pLineVert[ 5]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 7]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 8]))[i] = dwValue;
                    ((DWORD *)(pLineVert[10]))[i] = dwValue;
                }

                if (Y_MAJOR_FVF(0,1))
                {
                    ((LPD3DTLVERTEX)(pLineVert[ 0]))->sx -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 1]))->sx -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 2]))->sx += HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 3]))->sx += HALF_LINE_WIDTH;
                }
                else
                {
                    ((LPD3DTLVERTEX)(pLineVert[ 0]))->sy -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 1]))->sy -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 2]))->sy += HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 3]))->sy += HALF_LINE_WIDTH;
                }

                if (Y_MAJOR_FVF(4,5))
                {
                    ((LPD3DTLVERTEX)(pLineVert[ 4]))->sx -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 5]))->sx -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 6]))->sx += HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 7]))->sx += HALF_LINE_WIDTH;
                }
                else
                {
                    ((LPD3DTLVERTEX)(pLineVert[ 4]))->sy -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 5]))->sy -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 6]))->sy += HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 7]))->sy += HALF_LINE_WIDTH;
                }

                if (Y_MAJOR_FVF(8,9))
                {
                    ((LPD3DTLVERTEX)(pLineVert[ 8]))->sx -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 9]))->sx -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[10]))->sx += HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[11]))->sx += HALF_LINE_WIDTH;
                }
                else
                {
                    ((LPD3DTLVERTEX)(pLineVert[ 8]))->sy -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 9]))->sy -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[10]))->sy += HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[11]))->sy += HALF_LINE_WIDTH;
                }

                // if D3DDP2OP_TRIANGLEFAN_IMM, decide which edges are drawn and which are not
                // by inference from the reference rasterizer, the rules appear to be:
                // - interior spokes are NEVER drawn
                // - bit0 controls edge 0->1   where 0 is center vertex
                // - bitN controls edge N->N+1 where N = 1...triangle_count
                // - bitX controls edge X->0   where X = triangle_count+1

                if (dwCachedDP2Op == D3DDP2OP_TRIANGLEFAN_IMM)
                {
                    wTri = 0;

                    // v0-v1
                    // we never draw this edge for anything but the first triangle
                    if (wTriangleNum == 1) {
                        if (dwEdgeFlags & dwEdgeMask) {
                            awMaskedIndices[wTri*3+0] = awWireframeIndices[0*3+0];
                            awMaskedIndices[wTri*3+1] = awWireframeIndices[0*3+1];
                            awMaskedIndices[wTri*3+2] = awWireframeIndices[0*3+2];
                            wTri++;
                            awMaskedIndices[wTri*3+0] = awWireframeIndices[1*3+0];
                            awMaskedIndices[wTri*3+1] = awWireframeIndices[1*3+1];
                            awMaskedIndices[wTri*3+2] = awWireframeIndices[1*3+2];
                            wTri++;
                        }
                        dwEdgeMask <<= 1;
                    }

                    // v1-v2
                    if (dwEdgeFlags & dwEdgeMask) {
                        awMaskedIndices[wTri*3+0] = awWireframeIndices[2*3+0];
                        awMaskedIndices[wTri*3+1] = awWireframeIndices[2*3+1];
                        awMaskedIndices[wTri*3+2] = awWireframeIndices[2*3+2];
                        wTri++;
                        awMaskedIndices[wTri*3+0] = awWireframeIndices[3*3+0];
                        awMaskedIndices[wTri*3+1] = awWireframeIndices[3*3+1];
                        awMaskedIndices[wTri*3+2] = awWireframeIndices[3*3+2];
                        wTri++;
                    }
                    dwEdgeMask <<= 1;

                    // v2-v0
                    // we never draw this edge for anything but the last triangle
                    if (wTriangleNum == wPrim) {
                        if (dwEdgeFlags & dwEdgeMask) {
                            awMaskedIndices[wTri*3+0] = awWireframeIndices[4*3+0];
                            awMaskedIndices[wTri*3+1] = awWireframeIndices[4*3+1];
                            awMaskedIndices[wTri*3+2] = awWireframeIndices[4*3+2];
                            wTri++;
                            awMaskedIndices[wTri*3+0] = awWireframeIndices[5*3+0];
                            awMaskedIndices[wTri*3+1] = awWireframeIndices[5*3+1];
                            awMaskedIndices[wTri*3+2] = awWireframeIndices[5*3+2];
                            wTri++;
                        }
                        dwEdgeMask <<= 1;
                    }

                    pContext->dp2.dwIndices = (DWORD)(awMaskedIndices);

                    // Draw the specified triangle edges.
                    nvTriangleDispatch (pContext, wTri);
                }

                else {

                    pContext->dp2.dwIndices = (DWORD)(awWireframeIndices);

                    // Draw the all triangle edges.
                    nvTriangleDispatch (pContext, 6);
                }

                IF_NOT_CULLED_END

            }  // while (dwCount)

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
    return;
}

//-------------------------------------------------------------------------

void nvFVFIndexedWireframeTriangle
(
    NV_INNERLOOP_ARGLIST
)
{
    BYTE        LineVerts[12 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    BYTE       *pLineVert[12];
    BYTE       *pVert0, *pVert1, *pVert2;
    DWORD       i, dwDwordsPerVert;
    DWORD       v0,v1,v2;
    DWORD       dwControl;
    DWORD       dwOldCull;
    DWORD       dwIndex1, dwIndex2;
    DWORD       dwIndexStrides;
    DWORD       dwCullMask1;
    DWORD       dwCullMask2;
    float       dwCullValue;

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
    tempVB.own ((DWORD)(&LineVerts[0]), 12*((sizeof(D3DTLVERTEX))+(7*2*4)), CSimpleSurface::HEAP_SYS);
    pContext->ppDX8Streams[0] = &tempVB;

    DWORD dwCachedDP2Op         = pContext->dp2.dwDP2Prim;
    pContext->dp2.dwDP2Prim     = D3DDP2OP_INDEXEDTRIANGLELIST2;

    DWORD dwCachedTotalOffset   = pContext->dp2.dwTotalOffset;
    pContext->dp2.dwTotalOffset = 0ul;

    DWORD dwCachedMaxOffset     = global.dwMaxVertexOffset;
    global.dwMaxVertexOffset    = 12 * ((sizeof(D3DTLVERTEX)) + (7*2*4));

    if (dwCount)
    {
        LPBYTE pVertices = pVertexBufferBase + dwVertexBufferOffset;
        LPWORD pIndices  = (LPWORD)(pContext->dp2.dwIndices);
        DWORD  dwStrides = nv4DP2OpToStride[dwCachedDP2Op];

        // Calculate the new hardware state if neccessary.
        if (pContext->bStateChange)
        {
            if (pContext->bUseDX6Class)
            {
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
        if (!pContext->bUseDX6Class)
        {
            dwControl = pContext->ctxInnerLoop.dwControl;
            pContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
            pContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        }
        else
        {
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
        dwIndex1        = (DWORD)pIndices;
        dwIndex2        = (DWORD)pIndices + 2;
        dwIndexStrides  = dwStrides * 2;
        dwDwordsPerVert = pContext->pCurrentVShader->getStride() >> 2;

        // Get pointers to each of the 12 FVF wireframe line vertices.
        GET_FVF_POINTER (pLineVert[ 0], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 0, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 1], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 1, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 2], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 2, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 3], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 3, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 4], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 4, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 5], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 5, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 6], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 6, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 7], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 7, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 8], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 8, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[ 9], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 9, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[10], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 10, pContext->pCurrentVShader->getStride());
        GET_FVF_POINTER (pLineVert[11], LineVerts, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), 11, pContext->pCurrentVShader->getStride());

        // Draw all triangles as a wireframe.
        while (dwCount--)
        {
            v0 = *(WORD*) dwIndex1;
            v1 = *(WORD*)(dwIndex2 +  (dwIndexStrides >> 24));
            v2 = *(WORD*)(dwIndex2 + ((dwIndexStrides >> 24) ^ 2));

            // Move on to the next triangle.
            dwIndex1       += (BYTE)(dwIndexStrides >> 8);
            dwIndex2       += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwIndexStrides << 8) & 0xff000000;

            // Each edge line is rendered as a one pixel wide rectangle.

            // Get pointers to each vertex of current triangle.
            GET_FVF_POINTER (pVert0, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), v0, pContext->pCurrentVShader->getStride());
            GET_FVF_POINTER (pVert1, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), v1, pContext->pCurrentVShader->getStride());
            GET_FVF_POINTER (pVert2, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_POSITION]), v2, pContext->pCurrentVShader->getStride());

            // Since culling needs to be disabled to insure that the triangle edges get drawn properly,
            // back face culling of the triangle needs to be performed up front by software.
            IF_NOT_CULLED_BEGIN (pVert0, pVert1, pVert2, dwCullValue)

                // copy vertex data
                for (i = 0; i < dwDwordsPerVert; i++)
                {
                    DWORD dwValue;
                    dwValue = ((DWORD *)pVert0)[i];
                    ((DWORD *)(pLineVert[ 0]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 2]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 9]))[i] = dwValue;
                    ((DWORD *)(pLineVert[11]))[i] = dwValue;

                    dwValue = ((DWORD *)pVert1)[i];
                    ((DWORD *)(pLineVert[ 1]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 3]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 4]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 6]))[i] = dwValue;

                    dwValue = ((DWORD *)pVert2)[i];
                    ((DWORD *)(pLineVert[ 5]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 7]))[i] = dwValue;
                    ((DWORD *)(pLineVert[ 8]))[i] = dwValue;
                    ((DWORD *)(pLineVert[10]))[i] = dwValue;
                }


                if ((dwCachedDP2Op==D3DDP2OP_INDEXEDTRIANGLESTRIP)
                    && (pContext->dwRenderState[D3DRS_SHADEMODE]==D3DSHADE_FLAT)
                    && pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE]))
                {
                    BYTE * c0, * c1, * c2;
                    DWORD Off;

                    GET_FVF_POINTER (c0, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_DIFFUSE]), v0, pContext->pCurrentVShader->getStride());
                    GET_FVF_POINTER (c1, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_DIFFUSE]), v1, pContext->pCurrentVShader->getStride());
                    GET_FVF_POINTER (c2, pVertices, (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE]) ? 0xFFFFFFFF : 0), pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_DIFFUSE]), v2, pContext->pCurrentVShader->getStride());

                    Off=pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_DIFFUSE]);

                    *((DWORD *)(pLineVert[ 1]+Off))=*((DWORD *)c0);
                    *((DWORD *)(pLineVert[ 3]+Off))=*((DWORD *)c0);
                    *((DWORD *)(pLineVert[ 4]+Off))=*((DWORD *)c0);
                    *((DWORD *)(pLineVert[ 6]+Off))=*((DWORD *)c0);

                    *((DWORD *)(pLineVert[ 5]+Off))=*((DWORD *)c0);
                    *((DWORD *)(pLineVert[ 7]+Off))=*((DWORD *)c0);
                    *((DWORD *)(pLineVert[ 8]+Off))=*((DWORD *)c0);
                    *((DWORD *)(pLineVert[ 10]+Off))=*((DWORD *)c0);
                }


                if (Y_MAJOR_FVF(0,1))
                {
                    ((LPD3DTLVERTEX)(pLineVert[ 0]))->sx -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 1]))->sx -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 2]))->sx += HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 3]))->sx += HALF_LINE_WIDTH;
                }
                else
                {
                    ((LPD3DTLVERTEX)(pLineVert[ 0]))->sy -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 1]))->sy -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 2]))->sy += HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 3]))->sy += HALF_LINE_WIDTH;
                }

                if (Y_MAJOR_FVF(4,5))
                {
                    ((LPD3DTLVERTEX)(pLineVert[ 4]))->sx -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 5]))->sx -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 6]))->sx += HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 7]))->sx += HALF_LINE_WIDTH;
                }
                else
                {
                    ((LPD3DTLVERTEX)(pLineVert[ 4]))->sy -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 5]))->sy -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 6]))->sy += HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 7]))->sy += HALF_LINE_WIDTH;
                }

                if (Y_MAJOR_FVF(8,9))
                {
                    ((LPD3DTLVERTEX)(pLineVert[ 8]))->sx -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 9]))->sx -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[10]))->sx += HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[11]))->sx += HALF_LINE_WIDTH;
                }
                else
                {
                    ((LPD3DTLVERTEX)(pLineVert[ 8]))->sy -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[ 9]))->sy -= HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[10]))->sy += HALF_LINE_WIDTH;
                    ((LPD3DTLVERTEX)(pLineVert[11]))->sy += HALF_LINE_WIDTH;
                }

                // If this was called with RenderPrim, dwStrides will be LEGACY_STRIDES, otherwise it won't.
                if ((dwStrides != (LEGACY_STRIDES & 0xffffff)) ||
                    (( ((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLETRIANGLE) == D3DTRIFLAG_EDGEENABLETRIANGLE))
                {
                    pContext->dp2.dwIndices = (DWORD)(awWireframeIndices);
                    // Draw the triangle edges.
                    nvTriangleDispatch (pContext, 6);
                }

                else
                {
                    WORD wTri;
                    WORD awMaskedIndices[6*3];

                    wTri = 0;
                    if (((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLE1)
                    {
                        awMaskedIndices[0*3+0] = awWireframeIndices[0*3+0];
                        awMaskedIndices[0*3+1] = awWireframeIndices[0*3+1];
                        awMaskedIndices[0*3+2] = awWireframeIndices[0*3+2];
                        wTri++;
                        awMaskedIndices[1*3+0] = awWireframeIndices[1*3+0];
                        awMaskedIndices[1*3+1] = awWireframeIndices[1*3+1];
                        awMaskedIndices[1*3+2] = awWireframeIndices[1*3+2];
                        wTri++;
                    }
                    if (((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLE2)
                    {
                        awMaskedIndices[wTri*3+0] = awWireframeIndices[2*3+0];
                        awMaskedIndices[wTri*3+1] = awWireframeIndices[2*3+1];
                        awMaskedIndices[wTri*3+2] = awWireframeIndices[2*3+2];
                        wTri++;
                        awMaskedIndices[wTri*3+0] = awWireframeIndices[3*3+0];
                        awMaskedIndices[wTri*3+1] = awWireframeIndices[3*3+1];
                        awMaskedIndices[wTri*3+2] = awWireframeIndices[3*3+2];
                        wTri++;
                    }
                    if (((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLE3)
                    {
                        awMaskedIndices[wTri*3+0] = awWireframeIndices[4*3+0];
                        awMaskedIndices[wTri*3+1] = awWireframeIndices[4*3+1];
                        awMaskedIndices[wTri*3+2] = awWireframeIndices[4*3+2];
                        wTri++;
                        awMaskedIndices[wTri*3+0] = awWireframeIndices[5*3+0];
                        awMaskedIndices[wTri*3+1] = awWireframeIndices[5*3+1];
                        awMaskedIndices[wTri*3+2] = awWireframeIndices[5*3+2];
                        wTri++;
                    }
                    if (wTri)
                    {
                        pContext->dp2.dwIndices = (DWORD)(awMaskedIndices);
                        // Draw the specified triangle edges.
                        nvTriangleDispatch (pContext, wTri);
                    }
                }

            IF_NOT_CULLED_END

        }  // while (nPrimCount)

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
    return;
}

#endif  // NVARCH >= 0x04

