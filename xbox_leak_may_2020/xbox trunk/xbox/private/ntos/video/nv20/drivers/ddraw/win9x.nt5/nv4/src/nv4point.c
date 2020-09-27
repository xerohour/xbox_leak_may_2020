#ifdef  NV4
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4POINT.C                                                        *
*   The Direct 3d Point Rendereing routines.                                *
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
 * Non-Indexed TLVertex Point Lists.
 * This routine renders the specified number of points starting
 * with the vertex pointed to by lpVertices.
 * lpVertices is assumed to point to the first point to be rendered.
 */
void nvDrawPointList
(
    DWORD           dwPrimCount,
    LPD3DTLVERTEX   lpVertices
)
{
    DWORD           dwControl;
    DWORD           dwOldCull;
    D3DTLVERTEX     PointVerts[3];

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
     * Force first call to be to the triangle state setup routine.
     */
    NV_FORCE_TRI_SETUP(pCurrentContext);

    /*
     * Draw all points.
     */
    while (dwPrimCount--)
    {
        /*
         * Render each point as a one pixel size triangle.
         */
        PointVerts[0] = lpVertices[0];

        PointVerts[0].sx += 0.5f;
        PointVerts[0].sy += 0.5f;
        nvFloor(PointVerts[0].sx, PointVerts[0].sx);
        nvFloor(PointVerts[0].sy, PointVerts[0].sy);

        PointVerts[1] = PointVerts[0];
        PointVerts[2] = PointVerts[0];
        PointVerts[1].sx += 1.0f;
        PointVerts[2].sy += 1.0f;

#ifdef NV_FASTLOOPS
        nvTriangleDispatch(1, NULL, STRIP_STRIDES, (LPBYTE)PointVerts);
#else
        fnDX5Table[pCurrentContext->dwFunctionLookup](1, STRIP_STRIDES, PointVerts);
#endif
        lpVertices++;
    }
    /*
     * Restore cull mode.
     */
    pCurrentContext->ctxInnerLoop.dwControl = dwControl;
    pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;

    /*
     * Force next render call to be to the triangle state setup routine.
     */
    NV_FORCE_TRI_SETUP(pCurrentContext);
    return;
}

DWORD nvRenderPoint
(
    DWORD           dwPrimCount,
    LPBYTE          lpPrim,
    LPD3DTLVERTEX   lpVertices
)
{
    nvDrawPointList(dwPrimCount, lpVertices);
    return (DD_OK);
}

void nvDrawPointTriangle
(
    DWORD         dwPrimCount,
    DWORD         dwStrides,
    LPD3DTLVERTEX lpVertices
)
{
    WORD        Point[9];
    D3DTLVERTEX PointVerts[9];
    DWORD       v1,v2,v3;
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
         * Make these a static array someday to improve performance.
         */
        Point[0] = 0;
        Point[1] = 1;
        Point[2] = 2;
        Point[3] = 3;
        Point[4] = 4;
        Point[5] = 5;
        Point[6] = 6;
        Point[7] = 7;
        Point[8] = 8;
        /*
         * Draw all triangles as vertex points.
         */
        while (dwPrimCount--)
        {
            v1 = dwIndex1;
            v2 = dwIndex2 + (dwIndexStrides >> 24);
            v3 = dwIndex2 + ((dwIndexStrides >> 24) ^ 1);

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
            IF_NOT_CULLED_BEGIN((DWORD)&lpVertices[v1],(DWORD)&lpVertices[v2],(DWORD)&lpVertices[v3],dwCullValue)
                /*
                 * Each vertex is rendered as a one pixel size triangle.
                 */
                PointVerts[0] = lpVertices[v1];
                PointVerts[0].sx += 0.5f;
                PointVerts[0].sy += 0.5f;
                nvFloor(PointVerts[0].sx, PointVerts[0].sx);
                nvFloor(PointVerts[0].sy, PointVerts[0].sy);
                PointVerts[1] = lpVertices[v1];
                PointVerts[2] = lpVertices[v1];
                PointVerts[1].sx += 1.0f;
                PointVerts[2].sy += 1.0f;


                PointVerts[3] = lpVertices[v2];
                PointVerts[3].sx += 0.5f;
                PointVerts[3].sy += 0.5f;
                nvFloor(PointVerts[3].sx, PointVerts[3].sx);
                nvFloor(PointVerts[3].sy, PointVerts[3].sy);
                PointVerts[4] = lpVertices[v2];
                PointVerts[5] = lpVertices[v2];
                PointVerts[4].sx += 1.0f;
                PointVerts[5].sy += 1.0f;

                PointVerts[6] = lpVertices[v3];
                PointVerts[6].sx += 0.5f;
                PointVerts[6].sy += 0.5f;
                nvFloor(PointVerts[6].sx, PointVerts[6].sx);
                nvFloor(PointVerts[6].sy, PointVerts[6].sy);
                PointVerts[7] = lpVertices[v3];
                PointVerts[8] = lpVertices[v3];
                PointVerts[7].sx += 1.0f;
                PointVerts[8].sy += 1.0f;

                /*
                 * Draw the triangle vertices.
                 */
#ifdef NV_FASTLOOPS
                nvTriangleDispatch(3, Point, LIST_STRIDES, (LPBYTE)PointVerts);
#else
                fnDX5IndexedTable[pCurrentContext->dwFunctionLookup](3, Point, LIST_STRIDES, PointVerts);
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
void nvIndexedPointTriangle
(
    DWORD         dwPrimCount,
    LPWORD        lpIndices,
    DWORD         dwStrides,
    LPD3DTLVERTEX lpVertices
)
{
    WORD        Point[9];
    D3DTLVERTEX PointVerts[9];
    DWORD       v1,v2,v3;
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
         * Make these a static array someday to improve performance.
         */
        Point[0] = 0;
        Point[1] = 1;
        Point[2] = 2;
        Point[3] = 3;
        Point[4] = 4;
        Point[5] = 5;
        Point[6] = 6;
        Point[7] = 7;
        Point[8] = 8;
        /*
         * Draw all triangles as vertex points.
         */
        while (dwPrimCount--)
        {
            v1 = *(WORD*) dwIndex1;
            v2 = *(WORD*)(dwIndex2 +  (dwIndexStrides >> 24));
            v3 = *(WORD*)(dwIndex2 + ((dwIndexStrides >> 24) ^ 2));

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
            IF_NOT_CULLED_BEGIN((DWORD)&lpVertices[v1],(DWORD)&lpVertices[v2],(DWORD)&lpVertices[v3],dwCullValue)
                /*
                 * Each vertex is rendered as a one pixel size triangle.
                 */
                PointVerts[0] = lpVertices[v1];
                PointVerts[0].sx += 0.5f;
                PointVerts[0].sy += 0.5f;
                nvFloor(PointVerts[0].sx, PointVerts[0].sx);
                nvFloor(PointVerts[0].sy, PointVerts[0].sy);
                PointVerts[1] = lpVertices[v1];
                PointVerts[2] = lpVertices[v1];
                PointVerts[1].sx += 1.0f;
                PointVerts[2].sy += 1.0f;

                PointVerts[3] = lpVertices[v2];
                PointVerts[3].sx += 0.5f;
                PointVerts[3].sy += 0.5f;
                nvFloor(PointVerts[3].sx, PointVerts[3].sx);
                nvFloor(PointVerts[3].sy, PointVerts[3].sy);
                PointVerts[4] = lpVertices[v2];
                PointVerts[5] = lpVertices[v2];
                PointVerts[4].sx += 1.0f;
                PointVerts[5].sy += 1.0f;

                PointVerts[6] = lpVertices[v3];
                PointVerts[6].sx += 0.5f;
                PointVerts[6].sy += 0.5f;
                nvFloor(PointVerts[6].sx, PointVerts[6].sx);
                nvFloor(PointVerts[6].sy, PointVerts[6].sy);
                PointVerts[7] = lpVertices[v3];
                PointVerts[8] = lpVertices[v3];
                PointVerts[7].sx += 1.0f;
                PointVerts[8].sy += 1.0f;

                /*
                 * Draw the triangle vertices.
                 */
#ifdef NV_FASTLOOPS
                nvTriangleDispatch(3, Point, LIST_STRIDES, (LPBYTE)PointVerts);
#else
                fnDX5IndexedTable[pCurrentContext->dwFunctionLookup](3, Point, LIST_STRIDES, PointVerts);
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
#ifdef  NVD3D_DX6
/*
 * Non-Indexed Flexible Vertex Format Point Lists.
 * This routine renders the specified number of points starting
 * with the vertex pointed to by lpVertices.
 * lpVertices is assumed to point to the first point to be rendered.
 */
void nvFVFDrawPointList
(
    DWORD           dwPrimCount,
    LPBYTE          lpVertices
)
{
    BYTE            PointVerts[3 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    DWORD           dwPointVert0Ptr, dwPointVert1Ptr, dwPointVert2Ptr;
    DWORD           dwVert0Ptr;
    DWORD           i, dwDwordsPerVert;
    DWORD           dwControl;
    DWORD           dwOldCull;
    DWORD           dwVertexStride;

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
     * Force first call to be to the triangle state setup routine.
     */
    NV_FORCE_TRI_SETUP(pCurrentContext);

    /*
     * Draw all points.
     */
    dwVertexStride  = fvfData.dwVertexStride;
    dwDwordsPerVert = fvfData.dwVertexStride >> 2;

    /*
     * Get pointers to each of the 3 FVF point vertices.
     */
    GET_FVF_POINTER(dwPointVert0Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 0, fvfData.dwVertexStride);
    GET_FVF_POINTER(dwPointVert1Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 1, fvfData.dwVertexStride);
    GET_FVF_POINTER(dwPointVert2Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 2, fvfData.dwVertexStride);
    while (dwPrimCount--)
    {
        /*
         * Render each point as a one pixel size triangle.
         *
         * Get pointers to the vertex of current point.
         */
        GET_FVF_POINTER(dwVert0Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, 0, fvfData.dwVertexStride);
        lpVertices += dwVertexStride;

        /*
         * Copy the vertex data.
         */
        for (i = 0; i < dwDwordsPerVert; i++)
        {
            DWORD   dwValue;
            dwValue = ((DWORD *)dwVert0Ptr)[i];
            ((DWORD *)dwPointVert0Ptr)[i] = dwValue;
            ((DWORD *)dwPointVert1Ptr)[i] = dwValue;
            ((DWORD *)dwPointVert2Ptr)[i] = dwValue;
        }

        /*
         * Adjust vertex 0.
         */
        ((LPD3DTLVERTEX)dwPointVert0Ptr)->sx += 0.5f;
        ((LPD3DTLVERTEX)dwPointVert0Ptr)->sy += 0.5f;
        nvFloor(((LPD3DTLVERTEX)dwPointVert0Ptr)->sx, ((LPD3DTLVERTEX)dwPointVert0Ptr)->sx);
        nvFloor(((LPD3DTLVERTEX)dwPointVert0Ptr)->sy, ((LPD3DTLVERTEX)dwPointVert0Ptr)->sy);

        /*
         * Adjust vertex 1.
         */
        ((LPD3DTLVERTEX)dwPointVert1Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert0Ptr)->sx + 1.0f;
        ((LPD3DTLVERTEX)dwPointVert1Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert0Ptr)->sy;

        /*
         * Adjust vertex 2.
         */
        ((LPD3DTLVERTEX)dwPointVert2Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert0Ptr)->sx;
        ((LPD3DTLVERTEX)dwPointVert2Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert0Ptr)->sy + 1.0f;

#ifdef NV_FASTLOOPS
        nvTriangleDispatch(1, NULL, STRIP_STRIDES, (LPBYTE)PointVerts);
#else
        if (!pCurrentContext->bUseDX6Class)
            fnDX5FlexTable[pCurrentContext->dwFunctionLookup](1, STRIP_STRIDES, (LPBYTE)PointVerts);
        else
            fnDX6FlexTable[pCurrentContext->dwFunctionLookup](1, STRIP_STRIDES, (LPBYTE)PointVerts);
#endif
    }
    /*
     * Restore cull mode.
     */
    if (!pCurrentContext->bUseDX6Class)
        pCurrentContext->ctxInnerLoop.dwControl = dwControl;
    else
        pCurrentContext->mtsState.dwControl0 = dwControl;
    pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] = dwOldCull;

    /*
     * Force next render call to be to the triangle state setup routine.
     */
    NV_FORCE_TRI_SETUP(pCurrentContext);
    return;
}
void nvFVFDrawPointTriangle
(
    DWORD         dwPrimCount,
    DWORD         dwStrides,
    LPBYTE        lpVertices
)
{
    WORD        Point[9];
    BYTE        PointVerts[9 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    DWORD       dwPointVert0Ptr, dwPointVert1Ptr, dwPointVert2Ptr;
    DWORD       dwPointVert3Ptr, dwPointVert4Ptr, dwPointVert5Ptr;
    DWORD       dwPointVert6Ptr, dwPointVert7Ptr, dwPointVert8Ptr;
    DWORD       dwVert0Ptr, dwVert1Ptr, dwVert2Ptr;
    DWORD       i, dwDwordsPerVert;
    DWORD       v1,v2,v3;
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
        dwIndex1       = 0;
        dwIndex2       = 1;
        dwIndexStrides = dwStrides;
        dwDwordsPerVert = fvfData.dwVertexStride >> 2;

        /*
         * Get pointers to each of the 9 FVF point triangle vertices.
         */
        GET_FVF_POINTER(dwPointVert0Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 0, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert1Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 1, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert2Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 2, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert3Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 3, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert4Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 4, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert5Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 5, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert6Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 6, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert7Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 7, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert8Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 8, fvfData.dwVertexStride);

        /*
         * Make these a static array someday to improve performance.
         */
        Point[0] = 0;
        Point[1] = 1;
        Point[2] = 2;
        Point[3] = 3;
        Point[4] = 4;
        Point[5] = 5;
        Point[6] = 6;
        Point[7] = 7;
        Point[8] = 8;
        /*
         * Draw all triangles as vertex points.
         */
        while (dwPrimCount--)
        {
            v1 = dwIndex1;
            v2 = dwIndex2 + (dwIndexStrides >> 24);
            v3 = dwIndex2 + ((dwIndexStrides >> 24) ^ 1);

            /*
             * Move on to the next triangle.
             */
            dwIndex1       += (BYTE)(dwIndexStrides >> 8);
            dwIndex2       += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwIndexStrides << 8) & 0xff000000;

            /*
             * Each vertex is rendered as a one pixel size triangle.
             *
             * Get pointers to each vertex of current triangle.
             */
            GET_FVF_POINTER(dwVert0Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, v1, fvfData.dwVertexStride);
            GET_FVF_POINTER(dwVert1Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, v2, fvfData.dwVertexStride);
            GET_FVF_POINTER(dwVert2Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, v3, fvfData.dwVertexStride);

            /*
             * Since culling needs to be disabled to insure that the triangle edges get drawn properly,
             * back face culling of the triangle needs to be performed up front by software.
             */
            IF_NOT_CULLED_BEGIN(dwVert0Ptr,dwVert1Ptr,dwVert2Ptr,dwCullValue)
                for (i = 0; i < dwDwordsPerVert; i++)
                {
                    DWORD   dwValue;
                    dwValue = ((DWORD *)dwVert0Ptr)[i];
                    ((DWORD *)dwPointVert0Ptr)[i] = dwValue;
                    ((DWORD *)dwPointVert1Ptr)[i] = dwValue;
                    ((DWORD *)dwPointVert2Ptr)[i] = dwValue;

                    dwValue = ((DWORD *)dwVert1Ptr)[i];
                    ((DWORD *)dwPointVert3Ptr)[i] = dwValue;
                    ((DWORD *)dwPointVert4Ptr)[i] = dwValue;
                    ((DWORD *)dwPointVert5Ptr)[i] = dwValue;

                    dwValue = ((DWORD *)dwVert2Ptr)[i];
                    ((DWORD *)dwPointVert6Ptr)[i] = dwValue;
                    ((DWORD *)dwPointVert7Ptr)[i] = dwValue;
                    ((DWORD *)dwPointVert8Ptr)[i] = dwValue;
                }

                ((LPD3DTLVERTEX)dwPointVert0Ptr)->sx += 0.5f;
                ((LPD3DTLVERTEX)dwPointVert0Ptr)->sy += 0.5f;
                nvFloor(((LPD3DTLVERTEX)dwPointVert0Ptr)->sx, ((LPD3DTLVERTEX)dwPointVert0Ptr)->sx);
                nvFloor(((LPD3DTLVERTEX)dwPointVert0Ptr)->sy, ((LPD3DTLVERTEX)dwPointVert0Ptr)->sy);
                ((LPD3DTLVERTEX)dwPointVert1Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert0Ptr)->sx + 1.0f;
                ((LPD3DTLVERTEX)dwPointVert1Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert0Ptr)->sy;
                ((LPD3DTLVERTEX)dwPointVert2Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert0Ptr)->sx;
                ((LPD3DTLVERTEX)dwPointVert2Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert0Ptr)->sy + 1.0f;

                ((LPD3DTLVERTEX)dwPointVert3Ptr)->sx += 0.5f;
                ((LPD3DTLVERTEX)dwPointVert3Ptr)->sy += 0.5f;
                nvFloor(((LPD3DTLVERTEX)dwPointVert3Ptr)->sx, ((LPD3DTLVERTEX)dwPointVert3Ptr)->sx);
                nvFloor(((LPD3DTLVERTEX)dwPointVert3Ptr)->sy, ((LPD3DTLVERTEX)dwPointVert3Ptr)->sy);
                ((LPD3DTLVERTEX)dwPointVert4Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert3Ptr)->sx + 1.0f;
                ((LPD3DTLVERTEX)dwPointVert4Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert3Ptr)->sy;
                ((LPD3DTLVERTEX)dwPointVert5Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert3Ptr)->sx;
                ((LPD3DTLVERTEX)dwPointVert5Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert3Ptr)->sy + 1.0f;

                ((LPD3DTLVERTEX)dwPointVert6Ptr)->sx += 0.5f;
                ((LPD3DTLVERTEX)dwPointVert6Ptr)->sy += 0.5f;
                nvFloor(((LPD3DTLVERTEX)dwPointVert6Ptr)->sx, ((LPD3DTLVERTEX)dwPointVert6Ptr)->sx);
                nvFloor(((LPD3DTLVERTEX)dwPointVert6Ptr)->sy, ((LPD3DTLVERTEX)dwPointVert6Ptr)->sy);
                ((LPD3DTLVERTEX)dwPointVert7Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert6Ptr)->sx + 1.0f;
                ((LPD3DTLVERTEX)dwPointVert7Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert6Ptr)->sy;
                ((LPD3DTLVERTEX)dwPointVert8Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert6Ptr)->sx;
                ((LPD3DTLVERTEX)dwPointVert8Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert6Ptr)->sy + 1.0f;

                /*
                 * Draw the triangle vertices.
                 */
#ifdef NV_FASTLOOPS
                nvTriangleDispatch(3, Point, LIST_STRIDES, (LPBYTE)PointVerts);
#else
                if (!pCurrentContext->bUseDX6Class)
                    fnDX5FlexIndexedTable[pCurrentContext->dwFunctionLookup](3, Point, LIST_STRIDES, PointVerts);
                else
                    fnDX6FlexIndexedTable[pCurrentContext->dwFunctionLookup](3, Point, LIST_STRIDES, PointVerts);
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
void nvFVFIndexedPointTriangle
(
    DWORD         dwPrimCount,
    LPWORD        lpIndices,
    DWORD         dwStrides,
    LPBYTE        lpVertices
)
{
    WORD        Point[9];
    BYTE        PointVerts[9 * ((sizeof (D3DTLVERTEX)) + (7*2*4))];
    DWORD       dwPointVert0Ptr, dwPointVert1Ptr, dwPointVert2Ptr;
    DWORD       dwPointVert3Ptr, dwPointVert4Ptr, dwPointVert5Ptr;
    DWORD       dwPointVert6Ptr, dwPointVert7Ptr, dwPointVert8Ptr;
    DWORD       dwVert0Ptr, dwVert1Ptr, dwVert2Ptr;
    DWORD       i, dwDwordsPerVert;
    DWORD       v1,v2,v3;
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
         * Get pointers to each of the 9 FVF point triangle vertices.
         */
        GET_FVF_POINTER(dwPointVert0Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 0, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert1Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 1, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert2Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 2, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert3Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 3, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert4Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 4, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert5Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 5, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert6Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 6, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert7Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 7, fvfData.dwVertexStride);
        GET_FVF_POINTER(dwPointVert8Ptr, PointVerts, fvfData.dwXYZMask, fvfData.dwXYZOffset, 8, fvfData.dwVertexStride);

        /*
         * Make these a static array someday to improve performance.
         */
        Point[0] = 0;
        Point[1] = 1;
        Point[2] = 2;
        Point[3] = 3;
        Point[4] = 4;
        Point[5] = 5;
        Point[6] = 6;
        Point[7] = 7;
        Point[8] = 8;
        /*
         * Draw all triangles as vertex points.
         */
        while (dwPrimCount--)
        {
            v1 = *(WORD*) dwIndex1;
            v2 = *(WORD*)(dwIndex2 +  (dwIndexStrides >> 24));
            v3 = *(WORD*)(dwIndex2 + ((dwIndexStrides >> 24) ^ 2));

            /*
             * Move on to the next triangle.
             */
            dwIndex1       += (BYTE)(dwIndexStrides >> 8);
            dwIndex2       += (BYTE)(dwIndexStrides >> 0);
            dwIndexStrides ^= (dwIndexStrides << 8) & 0xff000000;

            /*
             * Each vertex is rendered as a one pixel size triangle.
             *
             * Get pointers to each vertex of current triangle.
             */
            GET_FVF_POINTER(dwVert0Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, v1, fvfData.dwVertexStride);
            GET_FVF_POINTER(dwVert1Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, v2, fvfData.dwVertexStride);
            GET_FVF_POINTER(dwVert2Ptr, lpVertices, fvfData.dwXYZMask, fvfData.dwXYZOffset, v3, fvfData.dwVertexStride);

            /*
             * Since culling needs to be disabled to insure that the triangle edges get drawn properly,
             * back face culling of the triangle needs to be performed up front by software.
             */
            IF_NOT_CULLED_BEGIN(dwVert0Ptr,dwVert1Ptr,dwVert2Ptr,dwCullValue)
                for (i = 0; i < dwDwordsPerVert; i++)
                {
                    DWORD   dwValue;
                    dwValue = ((DWORD *)dwVert0Ptr)[i];
                    ((DWORD *)dwPointVert0Ptr)[i] = dwValue;
                    ((DWORD *)dwPointVert1Ptr)[i] = dwValue;
                    ((DWORD *)dwPointVert2Ptr)[i] = dwValue;

                    dwValue = ((DWORD *)dwVert1Ptr)[i];
                    ((DWORD *)dwPointVert3Ptr)[i] = dwValue;
                    ((DWORD *)dwPointVert4Ptr)[i] = dwValue;
                    ((DWORD *)dwPointVert5Ptr)[i] = dwValue;

                    dwValue = ((DWORD *)dwVert2Ptr)[i];
                    ((DWORD *)dwPointVert6Ptr)[i] = dwValue;
                    ((DWORD *)dwPointVert7Ptr)[i] = dwValue;
                    ((DWORD *)dwPointVert8Ptr)[i] = dwValue;
                }
                ((LPD3DTLVERTEX)dwPointVert0Ptr)->sx += 0.5f;
                ((LPD3DTLVERTEX)dwPointVert0Ptr)->sy += 0.5f;
                nvFloor(((LPD3DTLVERTEX)dwPointVert0Ptr)->sx, ((LPD3DTLVERTEX)dwPointVert0Ptr)->sx);
                nvFloor(((LPD3DTLVERTEX)dwPointVert0Ptr)->sy, ((LPD3DTLVERTEX)dwPointVert0Ptr)->sy);
                ((LPD3DTLVERTEX)dwPointVert1Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert0Ptr)->sx + 1.0f;
                ((LPD3DTLVERTEX)dwPointVert1Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert0Ptr)->sy;
                ((LPD3DTLVERTEX)dwPointVert2Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert0Ptr)->sx;
                ((LPD3DTLVERTEX)dwPointVert2Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert0Ptr)->sy + 1.0f;

                ((LPD3DTLVERTEX)dwPointVert3Ptr)->sx += 0.5f;
                ((LPD3DTLVERTEX)dwPointVert3Ptr)->sy += 0.5f;
                nvFloor(((LPD3DTLVERTEX)dwPointVert3Ptr)->sx, ((LPD3DTLVERTEX)dwPointVert3Ptr)->sx);
                nvFloor(((LPD3DTLVERTEX)dwPointVert3Ptr)->sy, ((LPD3DTLVERTEX)dwPointVert3Ptr)->sy);
                ((LPD3DTLVERTEX)dwPointVert4Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert3Ptr)->sx + 1.0f;
                ((LPD3DTLVERTEX)dwPointVert4Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert3Ptr)->sy;
                ((LPD3DTLVERTEX)dwPointVert5Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert3Ptr)->sx;
                ((LPD3DTLVERTEX)dwPointVert5Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert3Ptr)->sy + 1.0f;

                ((LPD3DTLVERTEX)dwPointVert6Ptr)->sx += 0.5f;
                ((LPD3DTLVERTEX)dwPointVert6Ptr)->sy += 0.5f;
                nvFloor(((LPD3DTLVERTEX)dwPointVert6Ptr)->sx, ((LPD3DTLVERTEX)dwPointVert6Ptr)->sx);
                nvFloor(((LPD3DTLVERTEX)dwPointVert6Ptr)->sy, ((LPD3DTLVERTEX)dwPointVert6Ptr)->sy);
                ((LPD3DTLVERTEX)dwPointVert7Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert6Ptr)->sx + 1.0f;
                ((LPD3DTLVERTEX)dwPointVert7Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert6Ptr)->sy;
                ((LPD3DTLVERTEX)dwPointVert8Ptr)->sx = ((LPD3DTLVERTEX)dwPointVert6Ptr)->sx;
                ((LPD3DTLVERTEX)dwPointVert8Ptr)->sy = ((LPD3DTLVERTEX)dwPointVert6Ptr)->sy + 1.0f;

                /*
                 * Draw the triangle vertices.
                 */
#ifdef NV_FASTLOOPS
                nvTriangleDispatch(3, Point, LIST_STRIDES, (LPBYTE)PointVerts);
#else
                if (!pCurrentContext->bUseDX6Class)
                    fnDX5FlexIndexedTable[pCurrentContext->dwFunctionLookup](3, Point, LIST_STRIDES, PointVerts);
                else
                    fnDX6FlexIndexedTable[pCurrentContext->dwFunctionLookup](3, Point, LIST_STRIDES, PointVerts);
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
#endif  // NVD3D_DX6
#endif  // NV4