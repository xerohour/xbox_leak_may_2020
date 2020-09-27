#ifdef  NV4
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4LINE.C                                                         *
*   The Direct 3d Line Rendereing routines.                                 *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       04/12/97 - created                      *
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
#include "nv4vxmac.h"

static WORD EdgeLine[6*3] =
{
    0,1,3,
    1,3,4,
    1,2,5,
    2,5,6,
    0,2,7,
    0,7,8
};

#define LINE_WIDTH  1.0f

/*
 * cull macro
 */
#define IF_NOT_CULLED_BEGIN(v0,v1,v2,cv) {             \
    DWORD _sx0  = v0 + 0;                              \
    DWORD _sy0  = v0 + 4;                              \
    DWORD _sx1  = v1 + 0;                              \
    DWORD _sy1  = v1 + 4;                              \
    DWORD _sx2  = v2 + 0;                              \
    DWORD _sy2  = v2 + 4;                              \
    float _dx10 = *(float*)_sx1 - *(float*)_sx0;       \
    float _dy10 = *(float*)_sy1 - *(float*)_sy0;       \
    float _dx20 = *(float*)_sx2 - *(float*)_sx0;       \
    float _dy20 = *(float*)_sy2 - *(float*)_sy0;       \
           cv   = _dx10*_dy20  - _dx20*_dy10;          \
    if (((*(DWORD*)&cv) ^ dwCullMask1) & dwCullMask2) {
#define IF_NOT_CULLED_END } }

/*
 * Non-Indexed TLVertex Format Line Lists.
 */
void nvDrawLine
(
    DWORD           dwPrimCount,
    DWORD           dwVertexInc,
    LPD3DTLVERTEX   lpVertices
)
{
    DWORD           dwControl;
    DWORD           dwOldCull;
    D3DTLVERTEX     LineVerts[4];

    if (dwPrimCount)
    {
        /*
         * Calculate the new hardware state if neccessary.
         */
        if (pCurrentContext->dwStateChange)
        {
#ifdef  NVD3D_DX6
            if (pCurrentContext->bUseDX6Class)
            {
                nvSetMultiTextureHardwareState();
                if (pCurrentContext->bUseDX6Class)
                    nvSetHardwareState();
            }
            else
#endif  // NVD3D_DX6
                nvSetHardwareState();
        }

        /*
         * Disable culling.
         */
        dwControl = pCurrentContext->ctxInnerLoop.dwControl;
        pCurrentContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
        pCurrentContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        dwOldCull = pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        /*
         * Force first call to be to the triangle state setup routine.
         */
        NV_FORCE_TRI_SETUP(pCurrentContext);

        /*
         * Draw the lines.
         */
        while (dwPrimCount--)
        {
            /*
             * Each line is rendered as a one pixel wide rectangle.
             */
            LineVerts[0] = lpVertices[0];
            LineVerts[2] = lpVertices[0];
            LineVerts[1] = lpVertices[1];
            LineVerts[3] = lpVertices[1];
            lpVertices += dwVertexInc;
            if (fabs(LineVerts[2].sx - LineVerts[3].sx) < fabs(LineVerts[2].sy - LineVerts[3].sy))
            {
                LineVerts[2].sx += LINE_WIDTH;
                LineVerts[3].sx += LINE_WIDTH;
            }
            else
            {
                LineVerts[2].sy += LINE_WIDTH;
                LineVerts[3].sy += LINE_WIDTH;
            }
            /*
             * Call the low level rendering routine to draw the "line".
             */
#ifdef NV_FASTLOOPS
            nvTriangleDispatch(2, NULL, STRIP_STRIDES, (LPBYTE)LineVerts);
#else
            fnDX5Table[pCurrentContext->dwFunctionLookup](2, STRIP_STRIDES, LineVerts);
#endif
        }
        /*
         * Restore the culling mode.
         */
        pCurrentContext->ctxInnerLoop.dwControl = dwControl;
        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;
    }

    /*
     * Force next render call to be to the triangle state setup routine.
     */
    NV_FORCE_TRI_SETUP(pCurrentContext);
    return;
}

/*
 * Indexed TLVertex Format Line Lists.
 */
void nvIndexedLine
(
    DWORD           dwPrimCount,
    LPWORD          lpIndices,
    DWORD           dwIndexInc,
    LPD3DTLVERTEX   lpVertices
)
{
    DWORD           dwControl;
    DWORD           dwOldCull;
    D3DTLVERTEX     LineVerts[4];

    if (dwPrimCount)
    {
        /*
         * Calculate the new hardware state if neccessary.
         */
        if (pCurrentContext->dwStateChange)
        {
#ifdef  NVD3D_DX6
            if (pCurrentContext->bUseDX6Class)
            {
                nvSetMultiTextureHardwareState();
                if (pCurrentContext->bUseDX6Class)
                    nvSetHardwareState();
            }
            else
#endif  // NVD3D_DX6
                nvSetHardwareState();
        }

        /*
         * Disable culling.
         */
        dwControl = pCurrentContext->ctxInnerLoop.dwControl;
        pCurrentContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
        pCurrentContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        dwOldCull = pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        /*
         * Force first call to be to the triangle state setup routine.
         */
        NV_FORCE_TRI_SETUP(pCurrentContext);

        /*
         * Draw the lines.
         */
        while (dwPrimCount--)
        {
            /*
             * Each line is rendered as a one pixel wide rectangle.
             */
            LineVerts[0] = lpVertices[lpIndices[0]];
            LineVerts[2] = lpVertices[lpIndices[0]];
            LineVerts[1] = lpVertices[lpIndices[1]];
            LineVerts[3] = lpVertices[lpIndices[1]];
            lpIndices += dwIndexInc;
            if (fabs(LineVerts[2].sx - LineVerts[3].sx) < fabs(LineVerts[2].sy - LineVerts[3].sy))
            {
                LineVerts[2].sx += LINE_WIDTH;
                LineVerts[3].sx += LINE_WIDTH;
            }
            else
            {
                LineVerts[2].sy += LINE_WIDTH;
                LineVerts[3].sy += LINE_WIDTH;
            }
            /*
             * Call the low level rendering routine to draw the "line".
             */
#ifdef NV_FASTLOOPS
            nvTriangleDispatch(2, NULL, STRIP_STRIDES, (LPBYTE)LineVerts);
#else
            fnDX5Table[pCurrentContext->dwFunctionLookup](2, STRIP_STRIDES, LineVerts);
#endif
        }
        /*
         * Restore the culling mode.
         */
        pCurrentContext->ctxInnerLoop.dwControl = dwControl;
        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;
    }

    /*
     * Force first call to be to the triangle state setup routine.
     */
    NV_FORCE_TRI_SETUP(pCurrentContext);
    return;
}

DWORD nvRenderLine
(
    DWORD           dwPrimCount,
    LPBYTE          lpPrim,
    LPD3DTLVERTEX   lpVertices
)
{
    nvIndexedLine(dwPrimCount, (LPWORD)lpPrim, 2, lpVertices);
    return (DD_OK);
}
void nvDrawWireframeTriangle
(
    DWORD         dwPrimCount,
    DWORD         dwStrides,
    LPD3DTLVERTEX lpVertices
)
{
    D3DTLVERTEX LineVerts[9];
    DWORD       v0,v1,v2;
    DWORD       dwControl;
    DWORD       dwOldCull;
    DWORD       dwIndex1, dwIndex2;
    DWORD       dwIndexStrides;
    DWORD       dwCullMask1;
    DWORD       dwCullMask2;
    float       dwCullValue;

    if (dwPrimCount)
    {
        /*
         * Calculate the new hardware state if neccessary.
         */
        if (pCurrentContext->dwStateChange)
        {
#ifdef  NVD3D_DX6
            if (pCurrentContext->bUseDX6Class)
            {
                nvSetMultiTextureHardwareState();
                if (pCurrentContext->bUseDX6Class)
                    nvSetHardwareState();
            }
            else
#endif  // NVD3D_DX6
                nvSetHardwareState();
        }

        /*
         * Disable lower level culling while drawing lines.
         */
        dwControl = pCurrentContext->ctxInnerLoop.dwControl;
        pCurrentContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
        pCurrentContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        dwOldCull = pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        /*
         * Calculate the culling masks.
         */
        dwCullMask1 = (dwOldCull == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
        dwCullMask2 = (dwOldCull == D3DCULL_NONE) ? 0xffffffff : 0x80000000;

        /*
         * Force first call to be to the triangle state setup routine.
         */
        NV_FORCE_TRI_SETUP(pCurrentContext);

        dwStrides     &= 0xffffff; // mask interesting bits
        dwIndex1       = 0;
        dwIndex2       = 1;
        dwIndexStrides = dwStrides;

        /*
         * Draw all triangles as a wireframe.
         */
        while (dwPrimCount--)
        {
            v0 = dwIndex1;
            v1 = dwIndex2 + (dwIndexStrides >> 24);
            v2 = dwIndex2 + ((dwIndexStrides >> 24) ^ 1);

            /*
             * Move on to the next triangle.
             */
            dwIndex1       += (BYTE)(dwIndexStrides >> 8);
            dwIndex2       += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwStrides << 8) & 0xff000000;

            /*
             * Since culling needs to be disabled to insure that the triangle edges get drawn properly,
             * back face culling of the triangle needs to be performed up front by software.
             */
            IF_NOT_CULLED_BEGIN((DWORD)&lpVertices[v0],(DWORD)&lpVertices[v1],(DWORD)&lpVertices[v2],dwCullValue)
                /*
                 * Each edge line is rendered as a one pixel wide rectangle.
                 */
                LineVerts[0] = lpVertices[v0];
                LineVerts[3] = lpVertices[v0];
                LineVerts[8] = lpVertices[v0];
                LineVerts[1] = lpVertices[v1];
                LineVerts[4] = lpVertices[v1];
                LineVerts[5] = lpVertices[v1];
                LineVerts[2] = lpVertices[v2];
                LineVerts[6] = lpVertices[v2];
                LineVerts[7] = lpVertices[v2];
                if (fabs(LineVerts[3].sx - LineVerts[4].sx) < fabs(LineVerts[3].sy - LineVerts[4].sy))
                {
                    LineVerts[3].sx += LINE_WIDTH;
                    LineVerts[4].sx += LINE_WIDTH;
                }
                else
                {
                    LineVerts[3].sy += LINE_WIDTH;
                    LineVerts[4].sy += LINE_WIDTH;
                }
                if (fabs(LineVerts[5].sx - LineVerts[6].sx) < fabs(LineVerts[5].sy - LineVerts[6].sy))
                {
                    LineVerts[5].sx += LINE_WIDTH;
                    LineVerts[6].sx += LINE_WIDTH;
                }
                else
                {
                    LineVerts[5].sy += LINE_WIDTH;
                    LineVerts[6].sy += LINE_WIDTH;
                }
                if (fabs(LineVerts[7].sx - LineVerts[8].sx) < fabs(LineVerts[7].sy - LineVerts[8].sy))
                {
                    LineVerts[7].sx += LINE_WIDTH;
                    LineVerts[8].sx += LINE_WIDTH;
                }
                else
                {
                    LineVerts[7].sy += LINE_WIDTH;
                    LineVerts[8].sy += LINE_WIDTH;
                }

                /*
                 * Draw the triangle edges.
                 */
#ifdef NV_FASTLOOPS
                nvTriangleDispatch(6, EdgeLine, LIST_STRIDES, (LPBYTE)LineVerts);
#else
                fnDX5IndexedTable[pCurrentContext->dwFunctionLookup](6, EdgeLine, LIST_STRIDES, LineVerts);
#endif
            IF_NOT_CULLED_END
        }
        /*
         * Restore the culling mode.
         */
        pCurrentContext->ctxInnerLoop.dwControl = dwControl;
        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;
    }

    /*
     * Force first call to be to the triangle state setup routine.
     */
    NV_FORCE_TRI_SETUP(pCurrentContext);
    return;
}
void nvIndexedWireframeTriangle
(
    DWORD         dwPrimCount,
    LPWORD        lpIndices,
    DWORD         dwStrides,
    LPD3DTLVERTEX lpVertices
)
{
    D3DTLVERTEX LineVerts[9];
    DWORD       v0,v1,v2;
    DWORD       dwControl;
    DWORD       dwOldCull;
    DWORD       dwIndex1, dwIndex2;
    DWORD       dwIndexStrides;
    DWORD       dwCullMask1;
    DWORD       dwCullMask2;
    float       dwCullValue;

    if (dwPrimCount)
    {
        /*
         * Calculate the new hardware state if neccessary.
         */
        if (pCurrentContext->dwStateChange)
        {
#ifdef  NVD3D_DX6
            if (pCurrentContext->bUseDX6Class)
            {
                nvSetMultiTextureHardwareState();
                if (pCurrentContext->bUseDX6Class)
                    nvSetHardwareState();
            }
            else
#endif  // NVD3D_DX6
                nvSetHardwareState();
        }

        /*
         * Disable lower level culling while drawing lines.
         */
        dwControl = pCurrentContext->ctxInnerLoop.dwControl;
        pCurrentContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
        pCurrentContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        dwOldCull = pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        /*
         * Calculate the culling masks.
         */
        dwCullMask1 = (dwOldCull == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
        dwCullMask2 = (dwOldCull == D3DCULL_NONE) ? 0xffffffff : 0x80000000;

        /*
         * Force first call to be to the triangle state setup routine.
         */
        NV_FORCE_TRI_SETUP(pCurrentContext);

        dwStrides     &= 0xffffff; // mask interesting bits
        dwIndex1       = (DWORD)lpIndices;
        dwIndex2       = (DWORD)lpIndices + 2;
        dwIndexStrides = dwStrides * 2;

        /*
         * Draw all triangles as a wireframe.
         */
        while (dwPrimCount--)
        {
            v0 = *(WORD*) dwIndex1;
            v1 = *(WORD*)(dwIndex2 +  (dwIndexStrides >> 24));
            v2 = *(WORD*)(dwIndex2 + ((dwIndexStrides >> 24) ^ 2));

            /*
             * Move on to the next triangle.
             */
            dwIndex1       += (BYTE)(dwIndexStrides >> 8);
            dwIndex2       += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwIndexStrides << 8) & 0xff000000;

            /*
             * Since culling needs to be disabled to insure that the triangle edges get drawn properly,
             * back face culling of the triangle needs to be performed up front by software.
             */
            IF_NOT_CULLED_BEGIN((DWORD)&lpVertices[v0],(DWORD)&lpVertices[v1],(DWORD)&lpVertices[v2],dwCullValue)
                /*
                 * Each edge line is rendered as a one pixel wide rectangle.
                 */
                LineVerts[0] = lpVertices[v0];
                LineVerts[3] = lpVertices[v0];
                LineVerts[8] = lpVertices[v0];
                LineVerts[1] = lpVertices[v1];
                LineVerts[4] = lpVertices[v1];
                LineVerts[5] = lpVertices[v1];
                LineVerts[2] = lpVertices[v2];
                LineVerts[6] = lpVertices[v2];
                LineVerts[7] = lpVertices[v2];
                if (fabs(LineVerts[3].sx - LineVerts[4].sx) < fabs(LineVerts[3].sy - LineVerts[4].sy))
                {
                    LineVerts[3].sx += LINE_WIDTH;
                    LineVerts[4].sx += LINE_WIDTH;
                }
                else
                {
                    LineVerts[3].sy += LINE_WIDTH;
                    LineVerts[4].sy += LINE_WIDTH;
                }
                if (fabs(LineVerts[5].sx - LineVerts[6].sx) < fabs(LineVerts[5].sy - LineVerts[6].sy))
                {
                    LineVerts[5].sx += LINE_WIDTH;
                    LineVerts[6].sx += LINE_WIDTH;
                }
                else
                {
                    LineVerts[5].sy += LINE_WIDTH;
                    LineVerts[6].sy += LINE_WIDTH;
                }
                if (fabs(LineVerts[7].sx - LineVerts[8].sx) < fabs(LineVerts[7].sy - LineVerts[8].sy))
                {
                    LineVerts[7].sx += LINE_WIDTH;
                    LineVerts[8].sx += LINE_WIDTH;
                }
                else
                {
                    LineVerts[7].sy += LINE_WIDTH;
                    LineVerts[8].sy += LINE_WIDTH;
                }
                /*
                 * If this was called with RenderPrim, dwStrides will be LEGACY_STRIDES, otherwise it won't.
                 */
                if ((dwStrides != (LEGACY_STRIDES & 0xffffff)) ||
                    ((  ((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLETRIANGLE) == D3DTRIFLAG_EDGEENABLETRIANGLE))
                {
                    /*
                     * Draw the triangle edges.
                     */
#ifdef NV_FASTLOOPS
                    nvTriangleDispatch(6, EdgeLine, LIST_STRIDES, (LPBYTE)LineVerts);
#else
                    fnDX5IndexedTable[pCurrentContext->dwFunctionLookup](6, EdgeLine, LIST_STRIDES, LineVerts);
#endif
                }
                else
                {
                    DWORD tri;
                    WORD  triLine[6*3];

                    tri = 0;
                    if (((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLE1)
                    {
                        triLine[0*3+0]   = EdgeLine[0*3+0];
                        triLine[0*3+1]   = EdgeLine[0*3+1];
                        triLine[0*3+2]   = EdgeLine[0*3+2];
                        tri++;
                        triLine[1*3+0] = EdgeLine[1*3+0];
                        triLine[1*3+1] = EdgeLine[1*3+1];
                        triLine[1*3+2] = EdgeLine[1*3+2];
                        tri++;
                    }
                    if (((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLE2)
                    {
                        triLine[tri*3+0] = EdgeLine[2*3+0];
                        triLine[tri*3+1] = EdgeLine[2*3+1];
                        triLine[tri*3+2] = EdgeLine[2*3+2];
                        tri++;
                        triLine[tri*3+0] = EdgeLine[3*3+0];
                        triLine[tri*3+1] = EdgeLine[3*3+1];
                        triLine[tri*3+2] = EdgeLine[3*3+2];
                        tri++;
                    }
                    if (((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLE3)
                    {
                        triLine[tri*3+0] = EdgeLine[4*3+0];
                        triLine[tri*3+1] = EdgeLine[4*3+1];
                        triLine[tri*3+2] = EdgeLine[4*3+2];
                        tri++;
                        triLine[tri*3+0] = EdgeLine[5*3+0];
                        triLine[tri*3+1] = EdgeLine[5*3+1];
                        triLine[tri*3+2] = EdgeLine[5*3+2];
                        tri++;
                    }
                    if (tri)
                    {
                        /*
                         * Draw the specified triangle edges.
                         */
#ifdef NV_FASTLOOPS
                        nvTriangleDispatch(tri, triLine, LIST_STRIDES, (LPBYTE)LineVerts);
#else
                        fnDX5IndexedTable[pCurrentContext->dwFunctionLookup](tri, triLine, LIST_STRIDES, LineVerts);
#endif
                    }
                }
            IF_NOT_CULLED_END
        }
        /*
         * Restore the culling mode.
         */
        pCurrentContext->ctxInnerLoop.dwControl = dwControl;
        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;
    }

    /*
     * Force first call to be to the triangle state setup routine.
     */
    NV_FORCE_TRI_SETUP(pCurrentContext);
    return;
}
#ifdef  NVD3D_DX6
/*
 * Non-Indexed Flexible Vertex Format Lines.
 */
void nvFVFDrawLine
(
    DWORD           dwPrimCount,
    DWORD           dwVertexInc,
    LPBYTE          lpVertices
)
{
    BYTE            LineVerts[4 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    DWORD           dwLineVert0Ptr, dwLineVert1Ptr, dwLineVert2Ptr, dwLineVert3Ptr;
    DWORD           dwVert0Ptr, dwVert1Ptr;
    DWORD           i, dwDwordsPerVert;
    DWORD           dwControl;
    DWORD           dwOldCull;
    DWORD           dwVertexStride;
    DWORD           dwNextLine;

    if (dwPrimCount)
    {
        /*
         * Calculate the new hardware state if neccessary.
         */
        if (pCurrentContext->dwStateChange)
        {
            if (pCurrentContext->bUseDX6Class)
            {
                nvSetMultiTextureHardwareState();
                if (pCurrentContext->bUseDX6Class)
                    nvSetHardwareState();
            }
            else
                nvSetHardwareState();
        }

        /*
         * Disable culling.
         */
        if (!pCurrentContext->bUseDX6Class)
        {
            dwControl = pCurrentContext->ctxInnerLoop.dwControl;
            pCurrentContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
            pCurrentContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        }
        else
        {
            dwControl = pCurrentContext->mtsState.dwControl0;
            pCurrentContext->mtsState.dwControl0 &= ~(DRF_MASK(NV055_CONTROL0_CULLMODE) << DRF_SHIFT(NV055_CONTROL0_CULLMODE));
            pCurrentContext->mtsState.dwControl0 |= DRF_DEF(055, _CONTROL0, _CULLMODE, _NONE);
        }
        dwOldCull = pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        /*
         * Force first call to be to the triangle state setup routine.
         */
        NV_FORCE_TRI_SETUP(pCurrentContext);

        /*
         * Calculate number of vertex bytes to increment after each line.
         */
        dwVertexStride  = fvfData.dwVertexStride;
        dwNextLine      = dwVertexInc * dwVertexStride;
        dwDwordsPerVert = fvfData.dwVertexStride >> 2;

        /*
         * Get pointers to each of the 4 FVF line vertices.
         */
        GET_FVF_POINTER(dwLineVert0Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 0, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert1Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 1, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert2Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 2, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert3Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 3, fvfData.dwVertexStride);

        /*
         * Draw the lines.
         */
        while (dwPrimCount--)
        {
            /*
             * Each line is rendered as a one pixel wide rectangle.
             * Since we're contstructing new TLVERTEX structures anyway,
             * Construct full TLVERTEX structures up front from the FVF data
             * structures and then just call the TLVERTEX rendering loop.
             *
             * Get pointers to each vertex of current line.
             */
            GET_FVF_POINTER(dwVert0Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, 0, fvfData.dwVertexStride);
            GET_FVF_POINTER(dwVert1Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, 1, fvfData.dwVertexStride);
            lpVertices += dwNextLine;

            /*
             * Copy the vertices.
             */
            for (i = 0; i < dwDwordsPerVert; i++)
            {
                DWORD   dwValue;
                dwValue = ((DWORD *)dwVert0Ptr)[i];
                ((DWORD *)dwLineVert0Ptr)[i] = dwValue;
                ((DWORD *)dwLineVert2Ptr)[i] = dwValue;
                dwValue = ((DWORD *)dwVert1Ptr)[i];
                ((DWORD *)dwLineVert1Ptr)[i] = dwValue;
                ((DWORD *)dwLineVert3Ptr)[i] = dwValue;
            }
            if (fabs(((LPD3DTLVERTEX)dwLineVert2Ptr)->sx - ((LPD3DTLVERTEX)dwLineVert3Ptr)->sx) < fabs(((LPD3DTLVERTEX)dwLineVert2Ptr)->sy - ((LPD3DTLVERTEX)dwLineVert3Ptr)->sy))
            {
                ((LPD3DTLVERTEX)dwLineVert2Ptr)->sx += LINE_WIDTH;
                ((LPD3DTLVERTEX)dwLineVert3Ptr)->sx += LINE_WIDTH;
            }
            else
            {
                ((LPD3DTLVERTEX)dwLineVert2Ptr)->sy += LINE_WIDTH;
                ((LPD3DTLVERTEX)dwLineVert3Ptr)->sy += LINE_WIDTH;
            }

            /*
             * Call the low level rendering routine to draw the "line".
             */
#ifdef NV_FASTLOOPS
            nvTriangleDispatch(2, NULL, STRIP_STRIDES, (LPBYTE)LineVerts);
#else
            if (!pCurrentContext->bUseDX6Class)
                fnDX5FlexTable[pCurrentContext->dwFunctionLookup](2, STRIP_STRIDES, (LPBYTE)LineVerts);
            else
                fnDX6FlexTable[pCurrentContext->dwFunctionLookup](2, STRIP_STRIDES, (LPBYTE)LineVerts);
#endif
        }
        /*
         * Restore the culling mode.
         */
        if (!pCurrentContext->bUseDX6Class)
            pCurrentContext->ctxInnerLoop.dwControl = dwControl;
        else
            pCurrentContext->mtsState.dwControl0 = dwControl;

        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;
    }

    /*
     * Force next render call to be to the triangle state setup routine.
     */
    NV_FORCE_TRI_SETUP(pCurrentContext);
    return;
}

/*
 * Indexed Flexible Vertex Format Lines.
 */
void nvFVFIndexedLine
(
    DWORD           dwPrimCount,
    LPWORD          lpIndices,
    DWORD           dwIndexInc,
    LPBYTE          lpVertices
)
{
    BYTE            LineVerts[4 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    DWORD           dwLineVert0Ptr, dwLineVert1Ptr, dwLineVert2Ptr, dwLineVert3Ptr;
    DWORD           dwVert0Ptr, dwVert1Ptr;
    DWORD           i, dwDwordsPerVert;
    DWORD           dwControl;
    DWORD           dwOldCull;

    if (dwPrimCount)
    {
        /*
         * Calculate the new hardware state if neccessary.
         */
        if (pCurrentContext->dwStateChange)
        {
            if (pCurrentContext->bUseDX6Class)
            {
                nvSetMultiTextureHardwareState();
                if (pCurrentContext->bUseDX6Class)
                    nvSetHardwareState();
            }
            else
                nvSetHardwareState();
        }

        /*
         * Disable culling.
         */
        if (!pCurrentContext->bUseDX6Class)
        {
            dwControl = pCurrentContext->ctxInnerLoop.dwControl;
            pCurrentContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
            pCurrentContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        }
        else
        {
            dwControl = pCurrentContext->mtsState.dwControl0;
            pCurrentContext->mtsState.dwControl0 &= ~(DRF_MASK(NV055_CONTROL0_CULLMODE) << DRF_SHIFT(NV055_CONTROL0_CULLMODE));
            pCurrentContext->mtsState.dwControl0 |= DRF_DEF(055, _CONTROL0, _CULLMODE, _NONE);
        }
        dwOldCull = pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        /*
         * Force first call to be to the triangle state setup routine.
         */
        NV_FORCE_TRI_SETUP(pCurrentContext);

        dwDwordsPerVert = fvfData.dwVertexStride >> 2;

        /*
         * Get pointers to each of the 9 FVF wireframe line vertices.
         */
        GET_FVF_POINTER(dwLineVert0Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 0, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert1Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 1, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert2Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 2, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert3Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 3, fvfData.dwVertexStride);

        /*
         * Draw the lines.
         */
        while (dwPrimCount--)
        {
            /*
             * Each line is rendered as a one pixel wide rectangle.
             * Since we're contstructing new TLVERTEX structures anyway,
             * Construct full TLVERTEX structures up front from the FVF data
             * structures and then just call the TLVERTEX rendering loop.
             *
             * Get pointers to each vertex of current line.
             */
            GET_FVF_POINTER(dwVert0Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, lpIndices[0], fvfData.dwVertexStride);
            GET_FVF_POINTER(dwVert1Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, lpIndices[1], fvfData.dwVertexStride);
            lpIndices += dwIndexInc;

            /*
             * Copy the vertices.
             */
            for (i = 0; i < dwDwordsPerVert; i++)
            {
                DWORD   dwValue;
                dwValue = ((DWORD *)dwVert0Ptr)[i];
                ((DWORD *)dwLineVert0Ptr)[i] = dwValue;
                ((DWORD *)dwLineVert2Ptr)[i] = dwValue;
                dwValue = ((DWORD *)dwVert1Ptr)[i];
                ((DWORD *)dwLineVert1Ptr)[i] = dwValue;
                ((DWORD *)dwLineVert3Ptr)[i] = dwValue;
            }
            if (fabs(((LPD3DTLVERTEX)dwLineVert2Ptr)->sx - ((LPD3DTLVERTEX)dwLineVert3Ptr)->sx) < fabs(((LPD3DTLVERTEX)dwLineVert2Ptr)->sy - ((LPD3DTLVERTEX)dwLineVert3Ptr)->sy))
            {
                ((LPD3DTLVERTEX)dwLineVert2Ptr)->sx += LINE_WIDTH;
                ((LPD3DTLVERTEX)dwLineVert3Ptr)->sx += LINE_WIDTH;
            }
            else
            {
                ((LPD3DTLVERTEX)dwLineVert2Ptr)->sy += LINE_WIDTH;
                ((LPD3DTLVERTEX)dwLineVert3Ptr)->sy += LINE_WIDTH;
            }

            /*
             * Call the low level rendering routine to draw the "line".
             */
#ifdef NV_FASTLOOPS
            nvTriangleDispatch(2, NULL, STRIP_STRIDES, (LPBYTE)LineVerts);
#else
            if (!pCurrentContext->bUseDX6Class)
                fnDX5FlexTable[pCurrentContext->dwFunctionLookup](2, STRIP_STRIDES, (LPBYTE)LineVerts);
            else
                fnDX6FlexTable[pCurrentContext->dwFunctionLookup](2, STRIP_STRIDES, (LPBYTE)LineVerts);
#endif
        }
        /*
         * Restore the culling mode.
         */
        if (!pCurrentContext->bUseDX6Class)
            pCurrentContext->ctxInnerLoop.dwControl = dwControl;
        else
            pCurrentContext->mtsState.dwControl0 = dwControl;

        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;
    }

    /*
     * Force next render call to be to the triangle state setup routine.
     */
    NV_FORCE_TRI_SETUP(pCurrentContext);
    return;
}

void nvFVFDrawWireframeTriangle
(
    DWORD         dwPrimCount,
    DWORD         dwStrides,
    LPBYTE        lpVertices
)
{
    BYTE        LineVerts[9 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    DWORD       dwLineVert0Ptr, dwLineVert1Ptr, dwLineVert2Ptr;
    DWORD       dwLineVert3Ptr, dwLineVert4Ptr, dwLineVert5Ptr;
    DWORD       dwLineVert6Ptr, dwLineVert7Ptr, dwLineVert8Ptr;
    DWORD       dwVert0Ptr, dwVert1Ptr, dwVert2Ptr;
    DWORD       i, dwDwordsPerVert;
    DWORD       v0,v1,v2;
    DWORD       dwControl;
    DWORD       dwOldCull;
    DWORD       dwIndex1, dwIndex2;
    DWORD       dwIndexStrides;
    DWORD       dwCullMask1;
    DWORD       dwCullMask2;
    float       dwCullValue;

    if (dwPrimCount)
    {
        /*
         * Calculate the new hardware state if neccessary.
         */
        if (pCurrentContext->dwStateChange)
        {
            if (pCurrentContext->bUseDX6Class)
            {
                nvSetMultiTextureHardwareState();
                if (pCurrentContext->bUseDX6Class)
                    nvSetHardwareState();
            }
            else
                nvSetHardwareState();
        }

        /*
         * Disable lower level culling while drawing lines.
         */
        if (!pCurrentContext->bUseDX6Class)
        {
            dwControl = pCurrentContext->ctxInnerLoop.dwControl;
            pCurrentContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
            pCurrentContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        }
        else
        {
            dwControl = pCurrentContext->mtsState.dwControl0;
            pCurrentContext->mtsState.dwControl0 &= ~(DRF_MASK(NV055_CONTROL0_CULLMODE) << DRF_SHIFT(NV055_CONTROL0_CULLMODE));
            pCurrentContext->mtsState.dwControl0 |= DRF_DEF(055, _CONTROL0, _CULLMODE, _NONE);
        }
        dwOldCull = pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        /*
         * Calculate the culling masks.
         */
        dwCullMask1 = (dwOldCull == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
        dwCullMask2 = (dwOldCull == D3DCULL_NONE) ? 0xffffffff : 0x80000000;

        /*
         * Force first call to be to the triangle state setup routine.
         */
        NV_FORCE_TRI_SETUP(pCurrentContext);

        dwStrides      &= 0xffffff; // mask interesting bits
        dwIndex1        = 0;
        dwIndex2        = 1;
        dwIndexStrides  = dwStrides;
        dwDwordsPerVert = fvfData.dwVertexStride >> 2;

        /*
         * Get pointers to each of the 9 FVF wireframe line vertices.
         */
        GET_FVF_POINTER(dwLineVert0Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 0, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert1Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 1, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert2Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 2, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert3Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 3, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert4Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 4, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert5Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 5, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert6Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 6, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert7Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 7, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert8Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 8, fvfData.dwVertexStride);

        /*
         * Draw all triangles as a wireframe.
         */
        while (dwPrimCount--)
        {
            v0 = dwIndex1;
            v1 = dwIndex2 + (dwIndexStrides >> 24);
            v2 = dwIndex2 + ((dwIndexStrides >> 24) ^ 1);

            /*
             * Move on to the next triangle.
             */
            dwIndex1       += (BYTE)(dwIndexStrides >> 8);
            dwIndex2       += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwStrides << 8) & 0xff000000;

            /*
             * Each edge line is rendered as a one pixel wide rectangle.
             *
             * Get pointers to each vertex of current triangle.
             */
            GET_FVF_POINTER(dwVert0Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, v0, fvfData.dwVertexStride);
            GET_FVF_POINTER(dwVert1Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, v1, fvfData.dwVertexStride);
            GET_FVF_POINTER(dwVert2Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, v2, fvfData.dwVertexStride);

            /*
             * Since culling needs to be disabled to insure that the triangle edges get drawn properly,
             * back face culling of the triangle needs to be performed up front by software.
             */
            IF_NOT_CULLED_BEGIN(dwVert0Ptr,dwVert1Ptr,dwVert2Ptr,dwCullValue)
                for (i = 0; i < dwDwordsPerVert; i++)
                {
                    DWORD   dwValue;
                    dwValue = ((DWORD *)dwVert0Ptr)[i];
                    ((DWORD *)dwLineVert0Ptr)[i] = dwValue;
                    ((DWORD *)dwLineVert3Ptr)[i] = dwValue;
                    ((DWORD *)dwLineVert8Ptr)[i] = dwValue;

                    dwValue = ((DWORD *)dwVert1Ptr)[i];
                    ((DWORD *)dwLineVert1Ptr)[i] = dwValue;
                    ((DWORD *)dwLineVert4Ptr)[i] = dwValue;
                    ((DWORD *)dwLineVert5Ptr)[i] = dwValue;

                    dwValue = ((DWORD *)dwVert2Ptr)[i];
                    ((DWORD *)dwLineVert2Ptr)[i] = dwValue;
                    ((DWORD *)dwLineVert6Ptr)[i] = dwValue;
                    ((DWORD *)dwLineVert7Ptr)[i] = dwValue;
                }
                if (fabs(((LPD3DTLVERTEX)dwLineVert3Ptr)->sx - ((LPD3DTLVERTEX)dwLineVert4Ptr)->sx) < fabs(((LPD3DTLVERTEX)dwLineVert3Ptr)->sy - ((LPD3DTLVERTEX)dwLineVert4Ptr)->sy))
                {
                    ((LPD3DTLVERTEX)dwLineVert3Ptr)->sx += LINE_WIDTH;
                    ((LPD3DTLVERTEX)dwLineVert4Ptr)->sx += LINE_WIDTH;
                }
                else
                {
                    ((LPD3DTLVERTEX)dwLineVert3Ptr)->sy += LINE_WIDTH;
                    ((LPD3DTLVERTEX)dwLineVert4Ptr)->sy += LINE_WIDTH;
                }
                if (fabs(((LPD3DTLVERTEX)dwLineVert5Ptr)->sx - ((LPD3DTLVERTEX)dwLineVert6Ptr)->sx) < fabs(((LPD3DTLVERTEX)dwLineVert5Ptr)->sy - ((LPD3DTLVERTEX)dwLineVert6Ptr)->sy))
                {
                    ((LPD3DTLVERTEX)dwLineVert5Ptr)->sx += LINE_WIDTH;
                    ((LPD3DTLVERTEX)dwLineVert6Ptr)->sx += LINE_WIDTH;
                }
                else
                {
                    ((LPD3DTLVERTEX)dwLineVert5Ptr)->sy += LINE_WIDTH;
                    ((LPD3DTLVERTEX)dwLineVert6Ptr)->sy += LINE_WIDTH;
                }
                if (fabs(((LPD3DTLVERTEX)dwLineVert7Ptr)->sx - ((LPD3DTLVERTEX)dwLineVert8Ptr)->sx) < fabs(((LPD3DTLVERTEX)dwLineVert7Ptr)->sy - ((LPD3DTLVERTEX)dwLineVert8Ptr)->sy))
                {
                    ((LPD3DTLVERTEX)dwLineVert7Ptr)->sx += LINE_WIDTH;
                    ((LPD3DTLVERTEX)dwLineVert8Ptr)->sx += LINE_WIDTH;
                }
                else
                {
                    ((LPD3DTLVERTEX)dwLineVert7Ptr)->sy += LINE_WIDTH;
                    ((LPD3DTLVERTEX)dwLineVert8Ptr)->sy += LINE_WIDTH;
                }

                /*
                 * Draw the triangle edges.
                 */
#ifdef NV_FASTLOOPS
                nvTriangleDispatch(6, EdgeLine, LIST_STRIDES, (LPBYTE)LineVerts);
#else
                if (!pCurrentContext->bUseDX6Class)
                    fnDX5FlexIndexedTable[pCurrentContext->dwFunctionLookup](6, EdgeLine, LIST_STRIDES, LineVerts);
                else
                    fnDX6FlexIndexedTable[pCurrentContext->dwFunctionLookup](6, EdgeLine, LIST_STRIDES, LineVerts);
#endif
            IF_NOT_CULLED_END
        }
        /*
         * Restore the culling mode.
         */
        if (!pCurrentContext->bUseDX6Class)
            pCurrentContext->ctxInnerLoop.dwControl = dwControl;
        else
            pCurrentContext->mtsState.dwControl0 = dwControl;

        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;
    }

    /*
     * Force first call to be to the triangle state setup routine.
     */
    NV_FORCE_TRI_SETUP(pCurrentContext);
    return;
}
void nvFVFIndexedWireframeTriangle
(
    DWORD         dwPrimCount,
    LPWORD        lpIndices,
    DWORD         dwStrides,
    LPBYTE        lpVertices
)
{
    BYTE        LineVerts[9 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    DWORD       dwLineVert0Ptr, dwLineVert1Ptr, dwLineVert2Ptr;
    DWORD       dwLineVert3Ptr, dwLineVert4Ptr, dwLineVert5Ptr;
    DWORD       dwLineVert6Ptr, dwLineVert7Ptr, dwLineVert8Ptr;
    DWORD       dwVert0Ptr, dwVert1Ptr, dwVert2Ptr;
    DWORD       i, dwDwordsPerVert;
    DWORD       v0,v1,v2;
    DWORD       dwControl;
    DWORD       dwOldCull;
    DWORD       dwIndex1, dwIndex2;
    DWORD       dwIndexStrides;
    DWORD       dwCullMask1;
    DWORD       dwCullMask2;
    float       dwCullValue;

    if (dwPrimCount)
    {
        /*
         * Calculate the new hardware state if neccessary.
         */
        if (pCurrentContext->dwStateChange)
        {
            if (pCurrentContext->bUseDX6Class)
            {
                nvSetMultiTextureHardwareState();
                if (pCurrentContext->bUseDX6Class)
                    nvSetHardwareState();
            }
            else
                nvSetHardwareState();
        }

        /*
         * Disable lower level culling while drawing lines.
         */
        if (!pCurrentContext->bUseDX6Class)
        {
            dwControl = pCurrentContext->ctxInnerLoop.dwControl;
            pCurrentContext->ctxInnerLoop.dwControl &= ~(DRF_MASK(NV054_CONTROL_CULLMODE) << DRF_SHIFT(NV054_CONTROL_CULLMODE));
            pCurrentContext->ctxInnerLoop.dwControl |= DRF_DEF(054, _CONTROL, _CULLMODE, _NONE);
        }
        else
        {
            dwControl = pCurrentContext->mtsState.dwControl0;
            pCurrentContext->mtsState.dwControl0 &= ~(DRF_MASK(NV055_CONTROL0_CULLMODE) << DRF_SHIFT(NV055_CONTROL0_CULLMODE));
            pCurrentContext->mtsState.dwControl0 |= DRF_DEF(055, _CONTROL0, _CULLMODE, _NONE);
        }
        dwOldCull = pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE];
        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;

        /*
         * Calculate the culling masks.
         */
        dwCullMask1 = (dwOldCull == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
        dwCullMask2 = (dwOldCull == D3DCULL_NONE) ? 0xffffffff : 0x80000000;

        /*
         * Force first call to be to the triangle state setup routine.
         */
        NV_FORCE_TRI_SETUP(pCurrentContext);

        dwStrides     &= 0xffffff; // mask interesting bits
        dwIndex1       = (DWORD)lpIndices;
        dwIndex2       = (DWORD)lpIndices + 2;
        dwIndexStrides = dwStrides * 2;
        dwDwordsPerVert = fvfData.dwVertexStride >> 2;

        /*
         * Get pointers to each of the 9 FVF wireframe line vertices.
         */
        GET_FVF_POINTER(dwLineVert0Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 0, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert1Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 1, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert2Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 2, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert3Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 3, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert4Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 4, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert5Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 5, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert6Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 6, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert7Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 7, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwLineVert8Ptr, LineVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 8, fvfData.dwVertexStride);

        /*
         * Draw all triangles as a wireframe.
         */
        while (dwPrimCount--)
        {
            v0 = *(WORD*) dwIndex1;
            v1 = *(WORD*)(dwIndex2 +  (dwIndexStrides >> 24));
            v2 = *(WORD*)(dwIndex2 + ((dwIndexStrides >> 24) ^ 2));

            /*
             * Move on to the next triangle.
             */
            dwIndex1       += (BYTE)(dwIndexStrides >> 8);
            dwIndex2       += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwIndexStrides << 8) & 0xff000000;

            /*
             * Each edge line is rendered as a one pixel wide rectangle.
             *
             * Get pointers to each vertex of current triangle.
             */
            GET_FVF_POINTER(dwVert0Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, v0, fvfData.dwVertexStride);
            GET_FVF_POINTER(dwVert1Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, v1, fvfData.dwVertexStride);
            GET_FVF_POINTER(dwVert2Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, v2, fvfData.dwVertexStride);

            /*
             * Since culling needs to be disabled to insure that the triangle edges get drawn properly,
             * back face culling of the triangle needs to be performed up front by software.
             */
            IF_NOT_CULLED_BEGIN(dwVert0Ptr,dwVert1Ptr,dwVert2Ptr,dwCullValue)
                for (i = 0; i < dwDwordsPerVert; i++)
                {
                    DWORD   dwValue;
                    dwValue = ((DWORD *)dwVert0Ptr)[i];
                    ((DWORD *)dwLineVert0Ptr)[i] = dwValue;
                    ((DWORD *)dwLineVert3Ptr)[i] = dwValue;
                    ((DWORD *)dwLineVert8Ptr)[i] = dwValue;

                    dwValue = ((DWORD *)dwVert1Ptr)[i];
                    ((DWORD *)dwLineVert1Ptr)[i] = dwValue;
                    ((DWORD *)dwLineVert4Ptr)[i] = dwValue;
                    ((DWORD *)dwLineVert5Ptr)[i] = dwValue;

                    dwValue = ((DWORD *)dwVert2Ptr)[i];
                    ((DWORD *)dwLineVert2Ptr)[i] = dwValue;
                    ((DWORD *)dwLineVert6Ptr)[i] = dwValue;
                    ((DWORD *)dwLineVert7Ptr)[i] = dwValue;
                }
                if (fabs(((LPD3DTLVERTEX)dwLineVert3Ptr)->sx - ((LPD3DTLVERTEX)dwLineVert4Ptr)->sx) < fabs(((LPD3DTLVERTEX)dwLineVert3Ptr)->sy - ((LPD3DTLVERTEX)dwLineVert4Ptr)->sy))
                {
                    ((LPD3DTLVERTEX)dwLineVert3Ptr)->sx += LINE_WIDTH;
                    ((LPD3DTLVERTEX)dwLineVert4Ptr)->sx += LINE_WIDTH;
                }
                else
                {
                    ((LPD3DTLVERTEX)dwLineVert3Ptr)->sy += LINE_WIDTH;
                    ((LPD3DTLVERTEX)dwLineVert4Ptr)->sy += LINE_WIDTH;
                }
                if (fabs(((LPD3DTLVERTEX)dwLineVert5Ptr)->sx - ((LPD3DTLVERTEX)dwLineVert6Ptr)->sx) < fabs(((LPD3DTLVERTEX)dwLineVert5Ptr)->sy - ((LPD3DTLVERTEX)dwLineVert6Ptr)->sy))
                {
                    ((LPD3DTLVERTEX)dwLineVert5Ptr)->sx += LINE_WIDTH;
                    ((LPD3DTLVERTEX)dwLineVert6Ptr)->sx += LINE_WIDTH;
                }
                else
                {
                    ((LPD3DTLVERTEX)dwLineVert5Ptr)->sy += LINE_WIDTH;
                    ((LPD3DTLVERTEX)dwLineVert6Ptr)->sy += LINE_WIDTH;
                }
                if (fabs(((LPD3DTLVERTEX)dwLineVert7Ptr)->sx - ((LPD3DTLVERTEX)dwLineVert8Ptr)->sx) < fabs(((LPD3DTLVERTEX)dwLineVert7Ptr)->sy - ((LPD3DTLVERTEX)dwLineVert8Ptr)->sy))
                {
                    ((LPD3DTLVERTEX)dwLineVert7Ptr)->sx += LINE_WIDTH;
                    ((LPD3DTLVERTEX)dwLineVert8Ptr)->sx += LINE_WIDTH;
                }
                else
                {
                    ((LPD3DTLVERTEX)dwLineVert7Ptr)->sy += LINE_WIDTH;
                    ((LPD3DTLVERTEX)dwLineVert8Ptr)->sy += LINE_WIDTH;
                }

                /*
                 * If this was called with RenderPrim, dwStrides will be LEGACY_STRIDES, otherwise it won't.
                 */
                if ((dwStrides != (LEGACY_STRIDES & 0xffffff)) ||
                    ((  ((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLETRIANGLE) == D3DTRIFLAG_EDGEENABLETRIANGLE))
                {
                    /*
                     * Draw the triangle edges.
                     */
#ifdef NV_FASTLOOPS
                    nvTriangleDispatch(6, EdgeLine, LIST_STRIDES, (LPBYTE)LineVerts);
#else
                    if (!pCurrentContext->bUseDX6Class)
                        fnDX5FlexIndexedTable[pCurrentContext->dwFunctionLookup](6, EdgeLine, LIST_STRIDES, LineVerts);
                    else
                        fnDX6FlexIndexedTable[pCurrentContext->dwFunctionLookup](6, EdgeLine, LIST_STRIDES, LineVerts);
#endif
                }
                else
                {
                    DWORD tri;
                    WORD  triLine[6*3];

                    tri = 0;
                    if (((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLE1)
                    {
                        triLine[0*3+0]   = EdgeLine[0*3+0];
                        triLine[0*3+1]   = EdgeLine[0*3+1];
                        triLine[0*3+2]   = EdgeLine[0*3+2];
                        tri++;
                        triLine[1*3+0] = EdgeLine[1*3+0];
                        triLine[1*3+1] = EdgeLine[1*3+1];
                        triLine[1*3+2] = EdgeLine[1*3+2];
                        tri++;
                    }
                    if (((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLE2)
                    {
                        triLine[tri*3+0] = EdgeLine[2*3+0];
                        triLine[tri*3+1] = EdgeLine[2*3+1];
                        triLine[tri*3+2] = EdgeLine[2*3+2];
                        tri++;
                        triLine[tri*3+0] = EdgeLine[3*3+0];
                        triLine[tri*3+1] = EdgeLine[3*3+1];
                        triLine[tri*3+2] = EdgeLine[3*3+2];
                        tri++;
                    }
                    if (((WORD*)dwIndex1)[-1] & D3DTRIFLAG_EDGEENABLE3)
                    {
                        triLine[tri*3+0] = EdgeLine[4*3+0];
                        triLine[tri*3+1] = EdgeLine[4*3+1];
                        triLine[tri*3+2] = EdgeLine[4*3+2];
                        tri++;
                        triLine[tri*3+0] = EdgeLine[5*3+0];
                        triLine[tri*3+1] = EdgeLine[5*3+1];
                        triLine[tri*3+2] = EdgeLine[5*3+2];
                        tri++;
                    }
                    if (tri)
                    {
                        /*
                         * Draw the specified triangle edges.
                         */
#ifdef NV_FASTLOOPS
                        nvTriangleDispatch(tri, triLine, LIST_STRIDES, (LPBYTE)LineVerts);
#else
                        if (!pCurrentContext->bUseDX6Class)
                            fnDX5FlexIndexedTable[pCurrentContext->dwFunctionLookup](tri, triLine, LIST_STRIDES, LineVerts);
                        else
                            fnDX6FlexIndexedTable[pCurrentContext->dwFunctionLookup](tri, triLine, LIST_STRIDES, LineVerts);
#endif
                    }
                }
            IF_NOT_CULLED_END
        }
        /*
         * Restore the culling mode.
         */
        if (!pCurrentContext->bUseDX6Class)
            pCurrentContext->ctxInnerLoop.dwControl = dwControl;
        else
            pCurrentContext->mtsState.dwControl0 = dwControl;

        pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;
    }

    /*
     * Force first call to be to the triangle state setup routine.
     */
    NV_FORCE_TRI_SETUP(pCurrentContext);
    return;
}
#endif  // NVD3D_DX6
#endif  // NV4