#ifdef  NV4
/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4PRIM2.C                                                        *
*   NV4 DX6 DrawPrimitives2 routines.                                       *
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

#ifdef  NVD3D_DX6

#ifdef  WINNT
#define CHECK_CMDBUF_LIMITS(pDP2Data, pBuf)                                                 \
{                                                                                           \
    LPBYTE pBase,pEnd;                                                                      \
    pBase = (LPBYTE)(pDP2Data->lpDDCommands->lpGbl->fpVidMem + pDP2Data->dwCommandOffset);  \
    pEnd  = pBase + pDP2Data->dwCommandLength;                                              \
    if (! ((LPBYTE)pBuf < pEnd) && ( pBase <= (LPBYTE)pBuf))                                \
    {                                                                                       \
        pDP2Data->dwErrorOffset = (DWORD)lpCommands - dwCommandBufferStart;                 \
        pDP2Data->ddrval = D3DERR_COMMAND_UNPARSED;                                         \
        return DDHAL_DRIVER_HANDLED;                                                        \
    }                                                                                       \
}

#define CHECK_DATABUF_LIMITS(pDP2Data, iIndex)                                              \
{                                                                                           \
    if (!(((LONG)iIndex >= 0)                                                               \
     &&   ((LONG)iIndex < (LONG)pDP2Data->dwVertexLength)))                                 \
    {                                                                                       \
        pDP2Data->dwErrorOffset = (DWORD)lpCommands - dwCommandBufferStart;                 \
        pDP2Data->ddrval = D3DERR_COMMAND_UNPARSED;                                         \
        return DDHAL_DRIVER_HANDLED;                                                        \
    }                                                                                       \
}
#else   // !WINNT
#define CHECK_CMDBUF_LIMITS(pDP2Data, pBuf)
#define CHECK_DATABUF_LIMITS(pDP2Data, iIndex)
#endif  // !WINNT

D3DTLVERTEX tlvDefault =
{
    0.0f,           // sx
    0.0f,           // sy
    0.0f,           // sz
    1.0f,           // rhw
    0xFFFFFFFF,     // argb
    0xFF000000,     // specular
    0.0f,           // tu
    0.0f            // tv
};

/*
 * TL Vertex in Flex Format
 */
NVD3DFVFDATA constFVFData =
{
    D3DFVF_TLVERTEX,
    32,
    0xffffffff,0,
    0xffffffff,12,
    0xffffffff,16,
    0xffffffff,20,
    0xffffffff,24,
    1,
    0,0,0
};

PFND3DPARSEUNKNOWNCOMMAND   fnD3DParseUnknownCommandCallback;

#ifndef NV_FASTLOOPS
/*
 * DirectX 6.0 DrawPrimitives2 entry point.
 */
void nvDX5FlexTriangleSetup
(
    WORD            wPrimCount,
    DWORD           dwStrides,
    LPBYTE          lpVertices
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

    fnDX5FlexTable[pCurrentContext->dwFunctionLookup](wPrimCount, dwStrides, lpVertices);
    return;
}

void nvDX5FlexIndexedTriangleSetup
(
    DWORD           dwPrimCount,
    LPWORD          lpwIndices,
    DWORD           dwStrides,
    LPBYTE          lpVertices
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

    fnDX5FlexIndexedTable[pCurrentContext->dwFunctionLookup](dwPrimCount, lpwIndices, dwStrides, lpVertices);
    return;
}
#endif //!NV_FASTLOOPS

void nvSolidIndexedTriangle
(
    DWORD           dwPrimCount,
    LPWORD          lpwIndices,
    DWORD           dwStrides,
    LPBYTE          lpVertices
)
{
#ifdef NV_FASTLOOPS
    nvTriangleDispatch(dwPrimCount, lpwIndices, dwStrides, lpVertices);
#else
    if (!pCurrentContext->bUseDX6Class)
    {
        if (fvfData.dwVertexType == D3DFVF_TLVERTEX)
            fnDX5IndexedTable[pCurrentContext->dwFunctionLookup](dwPrimCount, lpwIndices, dwStrides, (LPD3DTLVERTEX)lpVertices);
        else
            fnDX5FlexIndexedTable[pCurrentContext->dwFunctionLookup](dwPrimCount, lpwIndices, dwStrides, lpVertices);
    }
    else
    {
        /*
         * Call an inner loop that uses the DX6_MULTI_TEXTURE_TRIANGLE class.
         */
        fnDX6FlexIndexedTable[pCurrentContext->dwFunctionLookup](dwPrimCount, lpwIndices, dwStrides, lpVertices);
    }
    return;
#endif //!NV_FASTLOOPS
}

void nvWireframeIndexedTriangle
(
    DWORD           dwPrimCount,
    LPWORD          lpwIndices,
    DWORD           dwStrides,
    LPBYTE          lpVertices
)
{
    if (!pCurrentContext->bUseDX6Class)
    {
        if (fvfData.dwVertexType == D3DFVF_TLVERTEX)
            nvIndexedWireframeTriangle(dwPrimCount, lpwIndices, dwStrides, (LPD3DTLVERTEX)lpVertices);
        else
            nvFVFIndexedWireframeTriangle(dwPrimCount, lpwIndices, dwStrides, lpVertices);
    }
    else
        nvFVFIndexedWireframeTriangle(dwPrimCount, lpwIndices, dwStrides, lpVertices);
    return;
}
void nvPointIndexedTriangle
(
    DWORD           dwPrimCount,
    LPWORD          lpwIndices,
    DWORD           dwStrides,
    LPBYTE          lpVertices
)
{
    if (!pCurrentContext->bUseDX6Class)
    {
        if (fvfData.dwVertexType == D3DFVF_TLVERTEX)
            nvIndexedPointTriangle(dwPrimCount, lpwIndices, dwStrides, (LPD3DTLVERTEX)lpVertices);
        else
            nvFVFIndexedPointTriangle(dwPrimCount, lpwIndices, dwStrides, lpVertices);
    }
    else
        nvFVFIndexedPointTriangle(dwPrimCount, lpwIndices, dwStrides, lpVertices);
    return;
}

void nvSolidNonIndexedTriangle
(
    WORD            wPrimCount,
    DWORD           dwStrides,
    LPBYTE          lpVertices
)
{
#ifdef NV_FASTLOOPS
    nvTriangleDispatch(wPrimCount, NULL, dwStrides, lpVertices);
#else
    if (!pCurrentContext->bUseDX6Class)
    {
        if (fvfData.dwVertexType == D3DFVF_TLVERTEX)
            fnDX5Table[pCurrentContext->dwFunctionLookup](wPrimCount, dwStrides, (LPD3DTLVERTEX)lpVertices);
        else
            fnDX5FlexTable[pCurrentContext->dwFunctionLookup](wPrimCount, dwStrides, lpVertices);
    }
    else
    {
        /*
         * Call an inner loop that uses the DX6_MULTI_TEXTURE_TRIANGLE class.
         */
        fnDX6FlexTable[pCurrentContext->dwFunctionLookup](wPrimCount, dwStrides, lpVertices);
    }
    return;
#endif //!NV_FASTLOOPS
}
void nvWireframeNonIndexedTriangle
(
    WORD            wPrimCount,
    DWORD           dwStrides,
    LPBYTE          lpVertices
)
{
    if (!pCurrentContext->bUseDX6Class)
    {
        if (fvfData.dwVertexType == D3DFVF_TLVERTEX)
            nvDrawWireframeTriangle(wPrimCount, dwStrides, (LPD3DTLVERTEX)lpVertices);
        else
            nvFVFDrawWireframeTriangle(wPrimCount, dwStrides, lpVertices);
    }
    else
        nvFVFDrawWireframeTriangle(wPrimCount, dwStrides, lpVertices);
    return;
}
void nvPointNonIndexedTriangle
(
    WORD            wPrimCount,
    DWORD           dwStrides,
    LPBYTE          lpVertices
)
{
    if (!pCurrentContext->bUseDX6Class)
    {
        if (fvfData.dwVertexType == D3DFVF_TLVERTEX)
            nvDrawPointTriangle(wPrimCount, dwStrides, (LPD3DTLVERTEX)lpVertices);
        else
            nvFVFDrawPointTriangle(wPrimCount, dwStrides, lpVertices);
    }
    else
        nvFVFDrawPointTriangle(wPrimCount, dwStrides, lpVertices);
    return;
}

LPNVFVFINDEXEDPRIM fnFillModeIndexedTriangle[] =
{
    0,
    nvPointIndexedTriangle,
    nvWireframeIndexedTriangle,
    nvSolidIndexedTriangle
};

LPNVFVFDRAWPRIM fnFillModeNonIndexedTriangle[] =
{
    0,
    nvPointNonIndexedTriangle,
    nvWireframeNonIndexedTriangle,
    nvSolidNonIndexedTriangle
};

DWORD nvDrawPrimitives2
(
    LPD3DHAL_DRAWPRIMITIVES2DATA pdp2d
)
{
    WORD                i;
    WORD                wCommandCount;
    WORD                wIndex, wCount;
    BOOL                bFilterChanged;
    BOOL                bTSSChanged;
    BOOL                bLoadFVFVertex;
    DWORD               dwMagFilter;
    DWORD               dwMinFilter;
    DWORD               dwStage;
    DWORD               dwState;
    DWORD               dwValue;
    DWORD               dwVertexType;
    DWORD               dwVertexStride;
    DWORD               dwUVCount;
    DWORD               dwCommandBufferStart;
    DWORD               dwCommandBufferEnd;
    DWORD               dwCommandBufferOffset;
    DWORD               dwCommandBufferLength;
    DWORD               dwVertexBufferStart;
    DWORD               dwVertexBufferOffset;
    DWORD               dwVertexBufferLength;
    HRESULT             ddrval;
    LPBYTE              lpPrim, lpFVFVertices;
    LPD3DHAL_DP2COMMAND lpCommands, lpResumeCommands;
    LPD3DTLVERTEX       lpVertices;

#ifdef NV_NULL_DRIVER
    pdp2d->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
#endif //NV_NULL_DRIVER
#ifdef NV_PROFILE
    NVP_START (NVP_T_DP2);
#endif
#ifdef NV_STATS
    {
        extern DWORD dwDP2CallCount;
        dwDP2CallCount ++;
    }
#endif

    DPF_LEVEL(NVDBG_LEVEL_DDI_ENTRY, "nvDrawPrimitives2 - hContext = %08lx", pdp2d->dwhContext);

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
#ifdef NV_PROFILE
            NVP_STOP (NVP_T_DP2);
            nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif
            pdp2d->ddrval = DD_OK;
            return (DDHAL_DRIVER_HANDLED);
        }
    }
    if (pCurrentContext->dwContextReset)
        nvResetContext((PNVD3DTEXTURE)NULL);

    /*
     * Setup frequently accessed globals.
     */
    NV_D3D_GLOBAL_SETUP();

    /*
     * check aa semantics
     */
    NV_AA_SEMANTICS_CHECK(pCurrentContext);

    /*
     * Get address of command buffer.
     */
    if (!(dwCommandBufferStart = (DWORD)pdp2d->lpDDCommands->lpGbl->fpVidMem))
    {
#ifdef NV_PROFILE
        NVP_STOP (NVP_T_DP2);
        nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif
        pdp2d->ddrval = DDERR_CANTLOCKSURFACE;
        return (DDHAL_DRIVER_HANDLED);
    }
    dwCommandBufferOffset = pdp2d->dwCommandOffset;
    dwCommandBufferLength = pdp2d->dwCommandLength;
    dwCommandBufferEnd    = dwCommandBufferStart + dwCommandBufferOffset + dwCommandBufferLength;

    /*
     * Calculate pointer to the first command to be processed.
     */
    lpCommands = (LPD3DHAL_DP2COMMAND)(dwCommandBufferStart + dwCommandBufferOffset);

    /*
     * Get address of vertex buffer.
     */
    if (pdp2d->dwFlags & D3DHALDP2_USERMEMVERTICES)
        dwVertexBufferStart = (DWORD)pdp2d->lpVertices;
    else
        dwVertexBufferStart = (DWORD)pdp2d->lpDDVertex->lpGbl->fpVidMem;
    if (!dwVertexBufferStart)
    {
#ifdef NV_PROFILE
        NVP_STOP (NVP_T_DP2);
        nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif
        pdp2d->ddrval = DDERR_CANTLOCKSURFACE;
        return (DDHAL_DRIVER_HANDLED);
    }
    dwVertexBufferOffset = pdp2d->dwVertexOffset;
    dwVertexBufferLength = pdp2d->dwVertexLength;
    lpVertices = (LPD3DTLVERTEX)(dwVertexBufferStart + dwVertexBufferOffset);

    /*
     * Get local copy of vertex flags
     */
    dwVertexType = pdp2d->dwVertexType;
    if (dwVertexType == D3DFVF_TLVERTEX)
        DPF_LEVEL(NVDBG_LEVEL_INFO, "D3D:nvDrawPrimitives2 - FVF Type = TLVERTEX");
    else
        DPF_LEVEL(NVDBG_LEVEL_INFO, "D3D:nvDrawPrimitives2 - FVF Type = %08lx", dwVertexType);

    /*
     * check if cached & short circuit fvfData calculation
     *  bLoadFVFVertex becomes TRUE when we need to recalc
     *  it will not go TRUE for TL verts - instead we just copy from a preset table
     */
    CHECK_FVF_DATA_AND_LOAD_TL (fvfData,dwVertexType,dwVertexStride,bLoadFVFVertex);

    /*
     * Calculate fvf data only if needed
     */
    if (bLoadFVFVertex)
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "D3D:nvDrawPrimitives2 - Re-Calculate FVF Data.");

        /*
         * Make sure there aren't any totally invalid FVF bits set.
         */
        if (dwVertexType & (D3DFVF_RESERVED0 | D3DFVF_RESERVED1 | D3DFVF_RESERVED2 | D3DFVF_NORMAL))
        {
#ifdef NV_PROFILE
            NVP_STOP (NVP_T_DP2);
            nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif
            pdp2d->ddrval = D3DERR_COMMAND_UNPARSED;
            pdp2d->dwErrorOffset = dwCommandBufferOffset;
            return (DDHAL_DRIVER_HANDLED);
        }

        /*
         * Calculate the number of texture coordinates present.
         * On NV4 we can accept 2 sets of UV coordinates
         * bdw: this is a usage restriction - the flex vertex is allowed to contain more
         */
        dwUVCount = (dwVertexType & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
        /*
        if (dwUVCount > NV_UV_COORDINATE_COUNT_MAX)
        {
            pdp2d->ddrval = D3DERR_COMMAND_UNPARSED;
            pdp2d->dwErrorOffset = dwCommandBufferOffset;
            return (DDHAL_DRIVER_HANDLED);
        }
        */

        /*
         * Vertex always has to have XYZ components.
         */
        if ((dwVertexType & (D3DFVF_XYZ | D3DFVF_XYZRHW)) == 0)
        {
#ifdef NV_PROFILE
            NVP_STOP (NVP_T_DP2);
            nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif
            pdp2d->ddrval = D3DERR_COMMAND_UNPARSED;
            pdp2d->dwErrorOffset = dwCommandBufferOffset;
            return (DDHAL_DRIVER_HANDLED);
        }
        dwVertexStride = sizeof(D3DVALUE) * 3;

        /*
         * Set up masks and offsets for each component of the FVF.
         * The location to get the vertex component data from will be calculated as follows:
         *      (lpVertices & ComponentMask) + ComponentOffset;
         * When ComponentMask == 0xFFFFFFFF, ComponentOffset will be an offset from the start
         * of the vertex data to the specific component.
         * When ComponentMask == 0x00000000, ComponentOffset will be a pointer to the default vertex data.
         *
         * FVF's must always have XYZ components.
         */
        fvfData.dwXYZMask   = 0xFFFFFFFF;
        fvfData.dwXYZOffset = 0;

        /*
         * Run down the variable vertex component list in the order they
         * will appear in the vertex data.
         */
        if (dwVertexType & D3DFVF_XYZRHW)
        {
            fvfData.dwRHWMask    = 0xFFFFFFFF;
            fvfData.dwRHWOffset  = dwVertexStride;
            dwVertexStride      += sizeof(D3DVALUE);
        }
        else
        {
            /*
             * No RHW specified, get it from the default vertex structure.
             */
            fvfData.dwRHWMask    = 0x00000000;
            fvfData.dwRHWOffset  = (DWORD)&tlvDefault.rhw;
        }
        if (dwVertexType & D3DFVF_DIFFUSE)
        {
            fvfData.dwARGBMask    = 0xFFFFFFFF;
            fvfData.dwARGBOffset  = dwVertexStride;
            dwVertexStride += sizeof(D3DCOLOR);
        }
        else
        {
            /*
             * No Diffuse Color specified, get it from the default vertex structure.
             */
            fvfData.dwARGBMask    = 0x00000000;
            fvfData.dwARGBOffset  = (DWORD)&tlvDefault.color;
        }
        if (dwVertexType & D3DFVF_SPECULAR)
        {
            fvfData.dwSpecularMask    = 0xFFFFFFFF;
            fvfData.dwSpecularOffset  = dwVertexStride;
            dwVertexStride           += sizeof(D3DCOLOR);
        }
        else
        {
            /*
             * No Specular component specified, get it from the default vertex structure.
             */
            fvfData.dwSpecularMask    = 0x00000000;
            fvfData.dwSpecularOffset  = (DWORD)&tlvDefault.specular;
        }
        if (dwUVCount)
        {
            fvfData.dwUVMask    = 0xFFFFFFFF;
            fvfData.dwUVOffset  = dwVertexStride;
            dwVertexStride     += (sizeof(D3DVALUE) * 2) * dwUVCount;
        }
        else
        {
            /*
             * No UV components specified, get it from the default vertex structure.
             */
            fvfData.dwUVMask    = 0x00000000;
            fvfData.dwUVOffset  = (DWORD)&tlvDefault.tu;
        }

        /*
         * Fill in rest of FVF data structure.
         */
        fvfData.dwVertexType   = dwVertexType;
        fvfData.dwVertexStride = dwVertexStride;
        fvfData.dwUVCount      = dwUVCount;
    }
    else
        DPF_LEVEL(NVDBG_LEVEL_INFO, "D3D:nvDrawPrimitives2 - FVF Data already loaded.");

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
     * If the spare subchannel does not contain the correct triangle
     * object, force an object state load.
     */
    if (!pCurrentContext->bUseDX6Class)
    {
        if ((pCurrentContext->dwStateChange || pDriverData->TwoDRenderingOccurred)
         || (pDriverData->dDrawSpareSubchannelObject != D3D_DX5_TEXTURED_TRIANGLE))
            NV_FORCE_TRI_SETUP(pCurrentContext);
    }
    else
    {
        if ((pCurrentContext->dwStateChange || pDriverData->TwoDRenderingOccurred)
         || (pDriverData->dDrawSpareSubchannelObject != D3D_DX6_MULTI_TEXTURE_TRIANGLE))
            NV_FORCE_TRI_SETUP(pCurrentContext);
    }

    /*
     * Parse the command buffer.
     */
    while ((DWORD)lpCommands < dwCommandBufferEnd)
    {
        wCommandCount = lpCommands->wPrimitiveCount;
        lpPrim        = (LPBYTE)lpCommands + sizeof(D3DHAL_DP2COMMAND);
        dbgDisplayDrawPrimitives2Info(lpCommands->bCommand, wCommandCount);
        switch (lpCommands->bCommand)
        {
            /*
             * Point Lists.
             */
            case D3DDP2OP_POINTS:
#ifdef  WINNT
                {
                    DWORD               i;
                    D3DHAL_DP2POINTS   *lpPrimCheck;

                    lpPrimCheck = (D3DHAL_DP2POINTS *)lpPrim;
                    for (i = 0; i < wCommandCount; i++)
                    {
                        CHECK_CMDBUF_LIMITS(pdp2d, lpPrimCheck);
                        wIndex = lpPrimCheck->wVStart;
                        wCount = lpPrimCheck->wCount;
                        CHECK_DATABUF_LIMITS(pdp2d, wIndex);
                        CHECK_DATABUF_LIMITS(pdp2d, (LONG)wIndex + wCount - 1);
                        lpPrimCheck++;
                    }
                }
#endif
                for (i = 0; i < wCommandCount; i++)
                {
                    if (!pCurrentContext->bUseDX6Class)
                    {
                        if (dwVertexType == D3DFVF_TLVERTEX)
                            nvDrawPointList((DWORD)((LPD3DHAL_DP2POINTS)lpPrim)->wCount, (LPD3DTLVERTEX)&lpVertices[((LPD3DHAL_DP2POINTS)lpPrim)->wVStart]);
                        else
                        {
                            lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2POINTS)lpPrim)->wVStart * dwVertexStride);
                            nvFVFDrawPointList((DWORD)((LPD3DHAL_DP2POINTS)lpPrim)->wCount, lpFVFVertices);
                        }
                    }
                    else
                    {
                        lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2POINTS)lpPrim)->wVStart * dwVertexStride);
                        nvFVFDrawPointList((DWORD)((LPD3DHAL_DP2POINTS)lpPrim)->wCount, lpFVFVertices);
                    }
                    lpPrim += sizeof(D3DHAL_DP2POINTS);
                }
                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2POINTS, wCommandCount, 0);
                break;
            /*
             * Line Lists.
             *
             *
             * Old Execute Buffer RenderPrimitive style indexed line list.
             */
            case D3DDP2OP_INDEXEDLINELIST:
#ifdef  WINNT
                {
                    D3DHAL_DP2INDEXEDLINELIST  *lpPrimCheck;
                    DWORD                       i;

                    lpPrimCheck = (D3DHAL_DP2INDEXEDLINELIST *)lpPrim;
                    for (i = 0; i < wCommandCount; i++)
                    {
                        CHECK_CMDBUF_LIMITS(pdp2d, lpPrimCheck);
                        CHECK_DATABUF_LIMITS(pdp2d, lpPrimCheck->wV1);
                        CHECK_DATABUF_LIMITS(pdp2d, lpPrimCheck->wV2);
                        lpPrimCheck++;
                    }
                }
#endif
                if (!pCurrentContext->bUseDX6Class)
                {
                    if (dwVertexType == D3DFVF_TLVERTEX)
                        nvIndexedLine((DWORD)wCommandCount, (LPWORD)lpPrim, 2, lpVertices);
                    else
                        nvFVFIndexedLine((DWORD)wCommandCount, (LPWORD)lpPrim, 2, (LPBYTE)lpVertices);
                }
                else
                    nvFVFIndexedLine((DWORD)wCommandCount, (LPWORD)lpPrim, 2, (LPBYTE)lpVertices);
                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2INDEXEDLINELIST, wCommandCount, 0);
                break;
            /*
             * Triangle lists.
             *
             *
             * Old style RenderPrimitive Execute Buffer entry.
             */
            case D3DDP2OP_INDEXEDTRIANGLELIST:
#ifdef  WINNT
                {
                    D3DHAL_DP2INDEXEDTRIANGLELIST  *lpPrimCheck;
                    DWORD                           i;

                    lpPrimCheck = (D3DHAL_DP2INDEXEDTRIANGLELIST *)lpPrim;
                    for (i = 0; i < wCommandCount; i++)
                    {
                        CHECK_CMDBUF_LIMITS(pdp2d, lpPrimCheck);
                        CHECK_DATABUF_LIMITS(pdp2d, lpPrimCheck->wV1);
                        CHECK_DATABUF_LIMITS(pdp2d, lpPrimCheck->wV2);
                        CHECK_DATABUF_LIMITS(pdp2d, lpPrimCheck->wV3);
                        lpPrimCheck++;
                    }
                }
#endif
                fnFillModeIndexedTriangle[pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE]]((DWORD)wCommandCount, (LPWORD)lpPrim, LEGACY_STRIDES, (LPBYTE)lpVertices);
                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2INDEXEDTRIANGLELIST, wCommandCount, 0);
                break;
            /*
             * Render State change.
             */
            case D3DDP2OP_RENDERSTATE:
                /*
                 * Run through the render state list.
                 */
                for (i = 0; i < wCommandCount; i++)
                {
                    DWORD   dwStateType, dwStateValue;

                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrim);

                    dwStateType  = ((LPD3DHAL_DP2RENDERSTATE)lpPrim)->RenderState;
                    dwStateValue = ((LPD3DHAL_DP2RENDERSTATE)lpPrim)->dwState;
                    lpPrim += sizeof(D3DHAL_DP2RENDERSTATE);

                    if (IS_OVERRIDE(dwStateType))
                    {
                        DWORD override = GET_OVERRIDE(dwStateType);
                        if (dwStateValue)
                        {
                            DPF_LEVEL (NVDBG_LEVEL_INFO, "nvDrawPrimitives2 - setting override for state %08lx", override);
                            STATESET_SET(pCurrentContext->overrides, override);
                        }
                        else
                        {
                            DPF_LEVEL (NVDBG_LEVEL_INFO, "nvDrawPrimitives2, clearing override for state %08lx", override);
                            STATESET_CLEAR(pCurrentContext->overrides, override);
                        }
                        continue;
                    }
                    if (STATESET_ISSET(pCurrentContext->overrides, dwStateType))
                    {
                        DPF_LEVEL (NVDBG_LEVEL_INFO, "nvDrawPrimitives2, state %08lx is overridden, ignoring", dwStateType);
                        continue;
                    }
                    if (pdp2d->lpdwRStates)
                        pdp2d->lpdwRStates[dwStateType] = dwStateValue;
                    nvSetContextState(dwStateType, dwStateValue, &ddrval);

                    /*
                     * Want to show that the application has set texturemapblend renderstate even if
                     * it's the same as the last time.
                     */
                    if (dwStateType == D3DRENDERSTATE_TEXTUREMAPBLEND)
                    {
                        /*
                         * If this is the first call to set TBLEND, then we want to set bUseTBlendSettings to FALSE since
                         * it is just an initialization call and doesn't count as an application usage.
                         * All subsequent calls should set bUseTBlendSettings to TRUE.
                         * Upon the first entry, NV_CONTEXT_TBLEND_UNINITIALIZED will be set.  All subseqent calls it will be
                         * cleared.
                         */
                        pCurrentContext->dwContextFlags &= ~NV_CONTEXT_TBLEND_UNINITIALIZED;
                        pCurrentContext->bUseTBlendSettings = !(pCurrentContext->dwContextFlags & NV_CONTEXT_TBLEND_UNINITIALIZED);
                    }

                }
                /*
                 * If the state has changed force a hardware state load.
                 */
                if (pCurrentContext->dwStateChange)
                {
                    /*
                     * If this is a DX6 application, then force hardware steup through
                     * the DX6 hardware setup routine, even if there is only one stage.
                     */
                    pCurrentContext->bUseDX6Class     = (pCurrentContext->dwDXVersionLevel >= APP_VERSION_DX6);
                    NV_FORCE_TRI_SETUP(pCurrentContext);
                }
                /*
                 * Determine the fog table mode based on if fog is enabled and the
                 * selected fog table mode.  A computed value of 0 = vertex or no fog,
                 * non-zero values indicate the fog table mode.
                 */
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2RENDERSTATE, wCommandCount, 0);
                break;
            /*
             * DX5 DrawOnePrimitive style non-indexed line list.
             */
            case D3DDP2OP_LINELIST:
#ifdef  WINNT
                {
                    D3DHAL_DP2LINELIST  *lpPrimCheck;

                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrim);
                    lpPrimCheck = (D3DHAL_DP2LINELIST *)lpPrim;
                    wIndex = lpPrimCheck->wVStart;
                    CHECK_DATABUF_LIMITS(pdp2d, wIndex);
                    CHECK_DATABUF_LIMITS(pdp2d, (LONG)wIndex + (2 * wCommandCount) - 1);
                }
#endif
                if (!pCurrentContext->bUseDX6Class)
                {
                    if (dwVertexType == D3DFVF_TLVERTEX)
                    {
                        lpPrim = (LPBYTE)&lpVertices[((LPD3DHAL_DP2LINELIST)lpPrim)->wVStart];
                        nvDrawLine((DWORD)wCommandCount, 2, (LPD3DTLVERTEX)lpPrim);
                    }
                    else
                    {
                        lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2LINELIST)lpPrim)->wVStart * dwVertexStride);
                        nvFVFDrawLine((DWORD)wCommandCount, 2, lpFVFVertices);
                    }
                }
                else
                {
                    lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2LINELIST)lpPrim)->wVStart * dwVertexStride);
                    nvFVFDrawLine((DWORD)wCommandCount, 2, lpFVFVertices);
                }
                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2LINELIST, 1, 0);
                break;
            /*
             * DX5 DrawOnePrimitive style non-indexed line strip.
             */
            case D3DDP2OP_LINESTRIP:
#ifdef  WINNT
                {
                    D3DHAL_DP2LINESTRIP *lpPrimCheck;

                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrim);
                    lpPrimCheck = (D3DHAL_DP2LINESTRIP *)lpPrim;
                    wIndex = lpPrimCheck->wVStart;
                    CHECK_DATABUF_LIMITS(pdp2d, wIndex);
                    CHECK_DATABUF_LIMITS(pdp2d, wIndex + wCommandCount);
                }
#endif
                if (!pCurrentContext->bUseDX6Class)
                {
                    if (dwVertexType == D3DFVF_TLVERTEX)
                    {
                        lpPrim = (LPBYTE)&lpVertices[((LPD3DHAL_DP2LINESTRIP)lpPrim)->wVStart];
                        nvDrawLine((DWORD)wCommandCount, 1, (LPD3DTLVERTEX)lpPrim);
                    }
                    else
                    {
                        lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2LINESTRIP)lpPrim)->wVStart * dwVertexStride);
                        nvFVFDrawLine((DWORD)wCommandCount, 1, lpFVFVertices);
                    }
                }
                else
                {
                    lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2LINESTRIP)lpPrim)->wVStart * dwVertexStride);
                    nvFVFDrawLine((DWORD)wCommandCount, 1, lpFVFVertices);
                }
                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2LINESTRIP, 1, 0);
                break;
            /*
             * Line Strips.
             *
             *
             * DX5 DrawOneIndexedPrimitive style indexed line strip.
             */
            case D3DDP2OP_INDEXEDLINESTRIP:
#ifdef  WINNT
                {
                    DWORD                       i;
                    WORD                        wIndexBase, wIndex1, wIndex2;
                    D3DHAL_DP2INDEXEDLINESTRIP *lpPrimCheck;
                    LPBYTE                      lpV1, lpV2;

                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrim);
                    wIndexBase = ((D3DHAL_DP2STARTVERTEX *)lpPrim)->wVStart;
                    lpPrimCheck = (D3DHAL_DP2INDEXEDLINESTRIP *)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX));
                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrimCheck);
                    wIndex1 = lpPrimCheck->wV[0];
                    wIndex2 = lpPrimCheck->wV[1];
                    CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex1);
                    for (i = 0; i < wCommandCount; i++)
                    {
                        CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex2);
                        if (i % 2)
                            wIndex2 = lpPrimCheck->wV[1];
                        else
                        {
                            lpPrimCheck++;
                            CHECK_CMDBUF_LIMITS(pdp2d, lpPrimCheck);
                            wIndex2 = lpPrimCheck->wV[0];
                        }
                    }
                }
#endif

                if (!pCurrentContext->bUseDX6Class)
                {
                    if (dwVertexType == D3DFVF_TLVERTEX)
                        nvIndexedLine((DWORD)wCommandCount, (LPWORD)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX)), 1, (LPD3DTLVERTEX)&lpVertices[((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart]);
                    else
                    {
                        lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart * dwVertexStride);
                        nvFVFIndexedLine((DWORD)wCommandCount, (LPWORD)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX)), 1, lpFVFVertices);
                    }
                }
                else
                {
                    lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart * dwVertexStride);
                    nvFVFIndexedLine((DWORD)wCommandCount, (LPWORD)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX)), 1, lpFVFVertices);
                }

                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, WORD, wCommandCount + 1, sizeof(D3DHAL_DP2STARTVERTEX));
                break;
            /*
             * DX5 DrawOnePrimitive style non-indexed triangle list.
             */
            case D3DDP2OP_TRIANGLELIST:
#ifdef  WINNT
                {
                    DWORD                   i;
                    D3DHAL_DP2TRIANGLELIST *lpPrimCheck;

                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrim);
                    lpPrimCheck = (D3DHAL_DP2TRIANGLELIST *)lpPrim;
                    wIndex = lpPrimCheck->wVStart;

                    CHECK_DATABUF_LIMITS(pdp2d, wIndex);
                    CHECK_DATABUF_LIMITS(pdp2d, (LONG)wIndex + 3 * wCommandCount - 1);
                }
#endif
                lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2TRIANGLELIST)lpPrim)->wVStart * dwVertexStride);
                fnFillModeNonIndexedTriangle[pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE]](wCommandCount, LIST_STRIDES, lpFVFVertices);

                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2TRIANGLELIST, 1, 0);
                break;
            /*
             * DX5 DrawOnePrimitive style non-indexed triangle strip.
             */
            case D3DDP2OP_TRIANGLESTRIP:
#ifdef  WINNT
                {
                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrim);
                    wIndex = ((LPD3DHAL_DP2TRIANGLESTRIP)lpPrim)->wVStart;
                    CHECK_DATABUF_LIMITS(pdp2d, wIndex);
                    CHECK_DATABUF_LIMITS(pdp2d, wIndex + wCommandCount + 1);
                }
#endif
                lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2TRIANGLESTRIP)lpPrim)->wVStart * dwVertexStride);
                fnFillModeNonIndexedTriangle[pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE]](wCommandCount, STRIP_STRIDES, lpFVFVertices);

                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2TRIANGLESTRIP, 1, 0);
                break;
            /*
             * Triangle strips.
             *
             *
             * DX5 DrawOneIndexedPrimitive style indexed triangle strip.
             */
            case D3DDP2OP_INDEXEDTRIANGLESTRIP:
#ifdef  WINNT
                {
                    WORD                            wIndexBase, wIndex0, wIndex1, wIndex2;
                    DWORD                           i;
                    D3DHAL_DP2INDEXEDTRIANGLESTRIP *lpPrimCheck;

                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrim);
                    wIndexBase = ((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart;
                    lpPrimCheck = (D3DHAL_DP2INDEXEDTRIANGLESTRIP *)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX));
                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrimCheck);
                    wIndex0 = lpPrimCheck->wV[0];
                    wIndex1 = lpPrimCheck->wV[1];
                    CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex0);
                    CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex1);
                    for (i = 0; i < wCommandCount; i++)
                    {
                        CHECK_CMDBUF_LIMITS(pdp2d, lpPrimCheck);
                        wIndex2 = lpPrimCheck->wV[2];
                        CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex2);
                        lpPrimCheck = (LPBYTE)lpPrimCheck + sizeof(WORD);
                    }
                }
#endif
                lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart * dwVertexStride);
                fnFillModeIndexedTriangle[pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE]]((DWORD)wCommandCount, (LPWORD)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX)), STRIP_STRIDES, lpFVFVertices);

                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, WORD, wCommandCount + 2, sizeof(D3DHAL_DP2STARTVERTEX));
                break;
            /*
             * Triangle fans.
             *
             * DX5 DrawOnePrimitive style non-indexed triangle fan.
             */
            case D3DDP2OP_TRIANGLEFAN:
#ifdef  WINNT
                {
                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrim);
                    wIndex = ((LPD3DHAL_DP2TRIANGLEFAN)lpPrim)->wVStart;
                    CHECK_DATABUF_LIMITS(pdp2d, wIndex);
                    CHECK_DATABUF_LIMITS(pdp2d, wIndex + wCommandCount + 1);
                }
#endif
                lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2TRIANGLEFAN)lpPrim)->wVStart * dwVertexStride);
                fnFillModeNonIndexedTriangle[pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE]](wCommandCount, FAN_STRIDES, lpFVFVertices);

                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2TRIANGLEFAN, 1, 0);
                break;
            /*
             * DX5 DrawOneIndexedPrimitive style indexed triangle fan.
             */
            case D3DDP2OP_INDEXEDTRIANGLEFAN:
#ifdef  WINNT
                {
                    WORD                            wIndexBase, wIndex0, wIndex1, wIndex2;
                    DWORD                           i;
                    D3DHAL_DP2INDEXEDTRIANGLEFAN   *lpPrimCheck;

                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrim);
                    wIndexBase = ((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart;
                    lpPrimCheck = (D3DHAL_DP2INDEXEDTRIANGLEFAN *)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX));
                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrimCheck);
                    wIndex0 = lpPrimCheck->wV[0];
                    wIndex1 = lpPrimCheck->wV[1];
                    CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex0);
                    CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex1);
                    for (i = 0; i < wCommandCount; i++)
                    {
                        CHECK_CMDBUF_LIMITS(pdp2d, lpPrimCheck);
                        wIndex2 = lpPrimCheck->wV[2];
                        CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex2);
                        lpPrimCheck = (LPBYTE)lpPrimCheck + sizeof(WORD);
                    }
                }
#endif
                lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart * dwVertexStride);
                fnFillModeIndexedTriangle[pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE]]((DWORD)wCommandCount, (LPWORD)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX)), FAN_STRIDES, lpFVFVertices);

                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, WORD, wCommandCount + 2, sizeof(D3DHAL_DP2STARTVERTEX));
                break;
            /*
             * Inline command buffer non-indexed triangle fans.
             * Similar to DX5 style DrawOnePrimitive.
             */
            case D3DDP2OP_TRIANGLEFAN_IMM:
                /*
                 * Skip over edge flags to get pointer to start of vertex list.
                 * Also, vertices are DWORD aligned, so just make sure that the
                 * alignment is correct.
                 */
                lpPrim += sizeof(D3DHAL_DP2TRIANGLEFAN_IMM);
                lpPrim = (LPBYTE)(((DWORD)lpPrim + 3) & ~3);

#ifdef  WINNT
                {
                    LPBYTE  lpV0, lpV1, lpV2;
                    lpV0 = lpPrim;
                    lpV1 = lpV0 + dwVertexStride;
                    lpV2 = lpV1 + dwVertexStride;
                    CHECK_CMDBUF_LIMITS(pdp2d, lpV0);
                    CHECK_CMDBUF_LIMITS(pdp2d, lpV1);
                    for (i = 0; i < wCommandCount; i++)
                    {
                        CHECK_CMDBUF_LIMITS(pdp2d, lpV2);
                        lpV1 = lpV2;
                        lpV2 += dwVertexStride;
                    }
                }
#endif

                fnFillModeNonIndexedTriangle[pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE]](wCommandCount, FAN_STRIDES, lpPrim);

                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, BYTE, ((wCommandCount + 2) * dwVertexStride), sizeof(D3DHAL_DP2TRIANGLEFAN_IMM));
                /*
                 * Realign next command since vertices are dword aligned
                 * and store # of primitives before affecting the pointer
                 */
                lpCommands = (LPD3DHAL_DP2COMMAND)(( ((DWORD)lpCommands) + 3 ) & ~ 3);
                break;
            /*
             * DX5 DrawOnePrimitive style in-line command buffer non-indexed line list.
             */
            case D3DDP2OP_LINELIST_IMM:
                /*
                 * Make sure command buffer is DWORD aligned for immediate line list vertices.
                 */
                lpPrim = (LPBYTE)(((DWORD)lpPrim + 3) & ~3);
#ifdef  WINNT
                {
                    LPBYTE  lpV0, lpV1;

                    lpV0 = lpPrim;
                    lpV1 = lpV0 + dwVertexStride;
                    CHECK_CMDBUF_LIMITS(pdp2d, lpV0);
                    for (i = 0; i < wCommandCount; i++)
                    {
                        CHECK_CMDBUF_LIMITS(pdp2d, lpV1);
                        lpV0 = lpV1;
                        lpV1 += dwVertexStride;
                    }
                }
#endif

                if (!pCurrentContext->bUseDX6Class)
                {
                    if (dwVertexType == D3DFVF_TLVERTEX)
                        nvDrawLine((DWORD)wCommandCount, 2, (LPD3DTLVERTEX)lpPrim);
                    else
                        nvFVFDrawLine((DWORD)wCommandCount, 2, lpPrim);
                }
                else
                    nvFVFDrawLine((DWORD)wCommandCount, 2, lpPrim);

                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, BYTE, ((wCommandCount + 1) * dwVertexStride), 0);
                /*
                 * Realign next command since vertices are dword aligned
                 */
                lpCommands = (LPD3DHAL_DP2COMMAND)(( ((DWORD)lpCommands) + 3 ) & ~ 3);
                break;
            /*
             * Change the texture stage state.
             */
            case D3DDP2OP_TEXTURESTAGESTATE:
                bFilterChanged = FALSE;
                bTSSChanged = FALSE;

                /*
                 * Display the texture stage state changes.
                 */
                for (i = 0; i < wCommandCount; i++)
                {
                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrim);

                    dwStage = (DWORD)((LPD3DHAL_DP2TEXTURESTAGESTATE)lpPrim)->wStage;
                    dwState = ((LPD3DHAL_DP2TEXTURESTAGESTATE)lpPrim)->TSState;
                    dwValue = ((LPD3DHAL_DP2TEXTURESTAGESTATE)lpPrim)->dwValue;
                    if (dbgShowState & NVDBG_SHOW_RENDER_STATE)
                        DPF("Texture Stage State Change - Stage: %ld, Type: %ld, State: %ld", dwStage, dwState, dwValue);
                    switch (dwState)
                    {
                        // D3DTSS_TEXTUREMAP
                        case 0:
                            /*
                             * Turn the texture handle into a texture pointer.
                             */
                            // test with default texture -paul

                            if (dwValue)
                            {
                                dwValue |= 0x80000000;
                                if (pCurrentContext->tssState[dwStage].dwTextureMap != dwValue)
                                {
                                    pCurrentContext->tssState[dwStage].dwTextureMap = dwValue;
                                    bTSSChanged = TRUE;
                                }
                            }
                            else
                            {
                                if (pCurrentContext->tssState[dwStage].dwTextureMap)
                                {
                                    pCurrentContext->tssState[dwStage].dwTextureMap = dwValue;
                                    bTSSChanged = TRUE;
                                }
                            }
                            /*
                             * For texture stage 0, propogate the change to DX5 class renderstate.
                             */
                            if (dwStage == 0)
                                nvSetContextState(D3DRENDERSTATE_TEXTUREHANDLE, dwValue, &ddrval);
                            break;
                        case D3DTSS_ADDRESS:
                            if ((pCurrentContext->tssState[dwStage].dwAddressU != dwValue)
                             || (pCurrentContext->tssState[dwStage].dwAddressV != dwValue))
                            {
                                pCurrentContext->tssState[dwStage].dwAddressU = dwValue;
                                pCurrentContext->tssState[dwStage].dwAddressV = dwValue;
                                bTSSChanged = TRUE;
                            }
                            /*
                             * For texture stage 0, propogate the change to DX5 class renderstate.
                             */
                            if (dwStage == 0)
                                nvSetContextState(D3DRENDERSTATE_TEXTUREADDRESS, dwValue, &ddrval);
                            break;
                        case D3DTSS_ADDRESSU:
                            if (pCurrentContext->tssState[dwStage].dwAddressU != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwAddressU = dwValue;
                                bTSSChanged = TRUE;
                            }
                            /*
                             * For texture stage 0, propogate the change to DX5 class renderstate.
                             */
                            if (dwStage == 0)
                                nvSetContextState(D3DRENDERSTATE_TEXTUREADDRESSU, dwValue, &ddrval);
                            break;
                        case D3DTSS_ADDRESSV:
                            if (pCurrentContext->tssState[dwStage].dwAddressV != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwAddressV = dwValue;
                                bTSSChanged = TRUE;
                            }
                            /*
                             * For texture stage 0, propogate the change to DX5 class renderstate.
                             */
                            if (dwStage == 0)
                                nvSetContextState(D3DRENDERSTATE_TEXTUREADDRESSV, ((LPD3DHAL_DP2TEXTURESTAGESTATE)lpPrim)->dwValue, &ddrval);
                            break;
                        case D3DTSS_MAGFILTER:
                            if (pCurrentContext->tssState[dwStage].dwMagFilter != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwMagFilter = dwValue;
                                bTSSChanged = TRUE;
                            }
                            if (dwStage == 0)
                                bFilterChanged = TRUE;
                            break;
                        case D3DTSS_MINFILTER:
                            if (pCurrentContext->tssState[dwStage].dwMinFilter != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwMinFilter = dwValue;
                                bTSSChanged = TRUE;
                            }
                            if (dwStage == 0)
                                bFilterChanged = TRUE;
                            break;
                        case D3DTSS_MIPFILTER:
                            if (pCurrentContext->tssState[dwStage].dwMipFilter != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwMipFilter = dwValue;
                                bTSSChanged = TRUE;
                            }
                            if (dwStage == 0)
                                bFilterChanged = TRUE;
                            break;
                        case D3DTSS_COLOROP:
                            if (pCurrentContext->tssState[dwStage].dwColorOp != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwColorOp = dwValue;
                                bTSSChanged = TRUE;
                            }
                            pCurrentContext->bUseTBlendSettings = FALSE;
                            break;
                        case D3DTSS_COLORARG1:
                            if (pCurrentContext->tssState[dwStage].dwColorArg1 != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwColorArg1 = dwValue;
                                bTSSChanged = TRUE;
                            }
                            pCurrentContext->bUseTBlendSettings = FALSE;
                            break;
                        case D3DTSS_COLORARG2:
                            if (pCurrentContext->tssState[dwStage].dwColorArg2 != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwColorArg2 = dwValue;
                                bTSSChanged = TRUE;
                            }
                            pCurrentContext->bUseTBlendSettings = FALSE;
                            break;
                        case D3DTSS_ALPHAOP:
                            if (pCurrentContext->tssState[dwStage].dwAlphaOp != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwAlphaOp = dwValue;
                                bTSSChanged = TRUE;
                            }
                            pCurrentContext->bUseTBlendSettings = FALSE;
                            break;
                        case D3DTSS_ALPHAARG1:
                            if (pCurrentContext->tssState[dwStage].dwAlphaArg1 != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwAlphaArg1 = dwValue;
                                bTSSChanged = TRUE;
                            }
                            pCurrentContext->bUseTBlendSettings = FALSE;
                            break;
                        case D3DTSS_ALPHAARG2:
                            if (pCurrentContext->tssState[dwStage].dwAlphaArg2 != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwAlphaArg2 = dwValue;
                                bTSSChanged = TRUE;
                            }
                            pCurrentContext->bUseTBlendSettings = FALSE;
                            break;
                        case D3DTSS_BUMPENVMAT00:
                            if (pCurrentContext->tssState[dwStage].dwBumpEnvMat00 != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwBumpEnvMat00 = dwValue;
                                bTSSChanged = TRUE;
                            }
                            break;
                        case D3DTSS_BUMPENVMAT01:
                            if (pCurrentContext->tssState[dwStage].dwBumpEnvMat01 != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwBumpEnvMat01 = dwValue;
                                bTSSChanged = TRUE;
                            }
                            break;
                        case D3DTSS_BUMPENVMAT10:
                            if (pCurrentContext->tssState[dwStage].dwBumpEnvMat10 != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwBumpEnvMat10 = dwValue;
                                bTSSChanged = TRUE;
                            }
                            break;
                        case D3DTSS_BUMPENVMAT11:
                            if (pCurrentContext->tssState[dwStage].dwBumpEnvMat11 != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwBumpEnvMat11 = dwValue;
                                bTSSChanged = TRUE;
                            }
                            break;
                        case D3DTSS_TEXCOORDINDEX:
                            if (pCurrentContext->tssState[dwStage].dwTexCoordIndex != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwTexCoordIndex = dwValue;
                                bTSSChanged = TRUE;
                            }
                            break;
                        case D3DTSS_BORDERCOLOR:
                            if (pCurrentContext->tssState[dwStage].dwBorderColor != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwBorderColor = dwValue;
                                bTSSChanged = TRUE;
                            }
                            break;
                        case D3DTSS_MIPMAPLODBIAS:
                            if (pCurrentContext->tssState[dwStage].dwMipMapLODBias != dwValue)
                            {
                                /*
                                 * Convert LODBIAS value to NV hardware value.
                                 * Here is the true meaning of this value as implemented by the reference rasterizer:
                                 *    Floating-point D3DVALUE value used to change the level of detail (LOD) bias.
                                 *    This value offsets the value of the mipmap level that is computed by trilinear
                                 *    texturing. It is usually in the range  -1.0 to 1.0; the default value is 0.0.
                                 *    Each unit bias (+/-1.0) biases the selection by exactly one mipmap level.
                                 *    A negative bias will cause the use of larger mipmap levels, resulting in a
                                 *    sharper but more aliased image.
                                 *    A positive bias will cause the use of smaller mipmap levels, resulting in a
                                 *    blurrier image.
                                 *    Applying a positive bias also results in the referencing of a smaller amount
                                 *    of texture data, which can boost performance on some systems.
                                 */
                                pCurrentContext->tssState[dwStage].dwMipMapLODBias = dwValue;
                                if (!dwValue)
                                    pCurrentContext->dwMipMapLODBias = pDriverData->regLODBiasAdjust;
                                else
                                {
                                    D3DVALUE dvLODBias;
                                    dvLODBias = *((D3DVALUE *)&dwValue);
                                    if (dvLODBias > 15.0f)
                                        dvLODBias = 15.0f;
                                    else if (dvLODBias < -15.0f)
                                        dvLODBias = -15.0f;
                                    /*
                                     * Convert the floating point value from D3D into a 5.3 NV value.
                                     * 0x08 == 1.0f
                                     */
                                    dvLODBias *= 8.0;
                                    pCurrentContext->tssState[dwStage].bLODBias = (BYTE)((long)dvLODBias & 0x000000FF);
                                }
                                bTSSChanged = TRUE;
                            }
                            /*
                             * For texture stage 0, propogate the change to DX5 class renderstate.
                             */
                            if (dwStage == 0)
                                nvSetContextState(D3DRENDERSTATE_MIPMAPLODBIAS, dwValue, &ddrval);
                            break;
                        case D3DTSS_MAXMIPLEVEL:
                            if (pCurrentContext->tssState[dwStage].dwMaxMipLevel != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwMaxMipLevel = dwValue;
                                bTSSChanged = TRUE;
                            }
                            break;
                        case D3DTSS_MAXANISOTROPY:
                            if (pCurrentContext->tssState[dwStage].dwMaxAnisotropy != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwMaxAnisotropy = dwValue;
                                bTSSChanged = TRUE;
                            }
                            break;
                        case D3DTSS_BUMPENVLSCALE:
                            if (pCurrentContext->tssState[dwStage].dwBumpEnvlScale != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwBumpEnvlScale = dwValue;
                                bTSSChanged = TRUE;
                            }
                            break;
                        case D3DTSS_BUMPENVLOFFSET:
                            if (pCurrentContext->tssState[dwStage].dwBumpEnvlOffset != dwValue)
                            {
                                pCurrentContext->tssState[dwStage].dwBumpEnvlOffset = dwValue;
                                bTSSChanged = TRUE;
                            }
                            break;
                        default:
                            DPF_LEVEL(NVDBG_LEVEL_ERROR, "Unimplemented Texture Stage State!!!  Stage: %ld, Type: %ld, State: %ld",
                                      dwStage, dwState, dwValue);
                            dbgD3DError();
                            break;
                    }
                    lpPrim += sizeof(D3DHAL_DP2TEXTURESTAGESTATE);
                }
                if (bFilterChanged)
                {
                    /*
                     * Calculate equivalent TEXTUREMIN and TEXTUREMAG bits from MINFILTER, MAGFILTER and MIPFILTER.
                     */
                    switch (pCurrentContext->tssState[0].dwMagFilter)
                    {
                        case D3DTFG_POINT:
                            dwMagFilter = D3DFILTER_NEAREST;
                            break;
                        case D3DTFG_LINEAR:
                            dwMagFilter = D3DFILTER_LINEAR;
                            break;
                        default:
                            dwMagFilter = D3DFILTER_LINEAR;
                            break;
                    }

                    if (pCurrentContext->tssState[0].dwMipFilter == D3DTFP_NONE)
                    {
                        switch (pCurrentContext->tssState[0].dwMinFilter)
                        {
                            case D3DTFN_POINT:
                                dwMinFilter = D3DFILTER_NEAREST;
                                break;
                            case D3DTFN_LINEAR:
                                dwMinFilter = D3DFILTER_LINEAR;
                                break;
                            default:
                                dwMinFilter = D3DFILTER_LINEAR;
                                break;
                        }
                    }
                    else if (pCurrentContext->tssState[0].dwMipFilter == D3DTFP_POINT)
                    {
                        switch (pCurrentContext->tssState[0].dwMinFilter)
                        {
                            case D3DTFN_POINT:
                                dwMinFilter = D3DFILTER_MIPNEAREST;
                                break;
                            case D3DTFN_LINEAR:
                                dwMinFilter = D3DFILTER_MIPLINEAR;
                                break;
                            default:
                                dwMinFilter = D3DFILTER_MIPLINEAR;
                                break;
                        }
                    }
                    else
                    {
                        switch (pCurrentContext->tssState[0].dwMinFilter)
                        {
                            case D3DTFN_POINT:
                                dwMinFilter = D3DFILTER_LINEARMIPNEAREST;
                                break;
                            case D3DTFN_LINEAR:
                                dwMinFilter = D3DFILTER_LINEARMIPLINEAR;
                                break;
                            default:
                                dwMinFilter = D3DFILTER_LINEARMIPLINEAR;
                                break;
                        }
                    }
                    nvSetContextState(D3DRENDERSTATE_TEXTUREMAG, dwMagFilter, &ddrval);
                    nvSetContextState(D3DRENDERSTATE_TEXTUREMIN, dwMinFilter, &ddrval);
                }
                /*
                 * If the state has changed force a hardware state load.
                 */
                if (bTSSChanged)
                {
                    /*
                     * If this is a DX6 application, then force hardware steup through
                     * the DX6 hardware setup routine, even if there is only one stage.
                     */
                    pCurrentContext->bUseDX6Class     = (pCurrentContext->dwDXVersionLevel >= APP_VERSION_DX6);
                    pCurrentContext->dwStateChange    = TRUE;
                    NV_FORCE_TRI_SETUP(pCurrentContext);
                }
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2TEXTURESTAGESTATE, wCommandCount, 0);
                break;
            /*
             * DX5 style DrawOneIndexedPrimitive indexed triangle strip.
             */
            case D3DDP2OP_INDEXEDTRIANGLELIST2:
#ifdef  WINNT
                {
                    WORD    wIndexBase, wIndex1, wIndex2, wIndex3;
                    DWORD   i;
                    D3DHAL_DP2INDEXEDTRIANGLELIST2 *lpPrimCheck;

                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrim);
                    wIndexBase = ((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart;
                    lpPrimCheck = (D3DHAL_DP2INDEXEDTRIANGLELIST2 *)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX));
                    for (i = 0; i < wCommandCount; i++)
                    {
                        CHECK_CMDBUF_LIMITS(pdp2d, lpPrimCheck);
                        wIndex1 = lpPrimCheck->wV1;
                        wIndex2 = lpPrimCheck->wV2;
                        wIndex3 = lpPrimCheck->wV3;
                        CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex1);
                        CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex2);
                        CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex3);
                        lpPrimCheck++;
                    }
                }
#endif
                lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart * dwVertexStride);
                fnFillModeIndexedTriangle[pCurrentContext->dwRenderState[D3DRENDERSTATE_FILLMODE]]((DWORD)wCommandCount, (LPWORD)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX)), LIST_STRIDES, lpFVFVertices);

                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2INDEXEDTRIANGLELIST2, wCommandCount, sizeof(D3DHAL_DP2STARTVERTEX));
                break;
            /*
             * DX5 DrawOneIndexedPrimitive style indexed line list.
             */
            case D3DDP2OP_INDEXEDLINELIST2:
#ifdef  WINNT
                {
                    WORD    wIndexBase, wIndex1, wIndex2;
                    DWORD   i;
                    D3DHAL_DP2INDEXEDLINELIST *lpPrimCheck;

                    CHECK_CMDBUF_LIMITS(pdp2d, lpPrim);
                    wIndexBase = ((D3DHAL_DP2STARTVERTEX*)lpPrim)->wVStart;
                    lpPrimCheck = (D3DHAL_DP2INDEXEDLINELIST *)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX));
                    for (i = 0; i < wCommandCount; i++)
                    {
                        CHECK_CMDBUF_LIMITS(pdp2d, lpPrimCheck);
                        wIndex1 = lpPrimCheck->wV1;
                        wIndex2 = lpPrimCheck->wV2;
                        CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex1);
                        CHECK_DATABUF_LIMITS(pdp2d, wIndexBase + wIndex2);
                        lpPrimCheck++;
                    }
                }
#endif
                if (!pCurrentContext->bUseDX6Class)
                {
                    if (dwVertexType == D3DFVF_TLVERTEX)
                        nvIndexedLine((DWORD)wCommandCount, (LPWORD)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX)), 2, (LPD3DTLVERTEX)&lpVertices[((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart]);
                    else
                    {
                        lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart * dwVertexStride);
                        nvFVFIndexedLine((DWORD)wCommandCount, (LPWORD)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX)), 2, lpFVFVertices);
                    }
                }
                else
                {
                    lpFVFVertices = (LPBYTE)lpVertices + ((DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart * dwVertexStride);
                    nvFVFIndexedLine((DWORD)wCommandCount, (LPWORD)(lpPrim + sizeof(D3DHAL_DP2STARTVERTEX)), 2, lpFVFVertices);
                }

                /*
                 * Update the put offset.
                 */
                //nvStartDmaBuffer(); // For some reason, taking these lines out caused dropped triangles, especially in 3D winbench!  Why???
                dbgFlushDmaBuffers(pCurrentContext);
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2INDEXEDLINELIST, wCommandCount, sizeof(D3DHAL_DP2STARTVERTEX));
                break;
            /*
             *
             */
            case D3DDP2OP_VIEWPORTINFO:
            {
                WORD    wX      = (WORD)(((LPD3DHAL_DP2VIEWPORTINFO)lpPrim)->dwX & 0x0000FFFF);
                WORD    wY      = (WORD)(((LPD3DHAL_DP2VIEWPORTINFO)lpPrim)->dwY & 0x0000FFFF);
                WORD    wWidth  = (WORD)(((LPD3DHAL_DP2VIEWPORTINFO)lpPrim)->dwWidth & 0x0000FFFF);
                WORD    wHeight = (WORD)(((LPD3DHAL_DP2VIEWPORTINFO)lpPrim)->dwHeight & 0x0000FFFF);
                if ((pCurrentContext->surfaceViewport.clipHorizontal.wX != wX)
                 || (pCurrentContext->surfaceViewport.clipVertical.wY != wY)
                 || (pCurrentContext->surfaceViewport.clipHorizontal.wWidth != wWidth)
                 || (pCurrentContext->surfaceViewport.clipVertical.wHeight != wHeight))
                {
                    pCurrentContext->surfaceViewport.clipHorizontal.wX     = wX;
                    pCurrentContext->surfaceViewport.clipVertical.wY       = wY;
                    pCurrentContext->surfaceViewport.clipHorizontal.wWidth = wWidth;
                    pCurrentContext->surfaceViewport.clipVertical.wHeight  = wHeight;
                    nvSetD3DSurfaceViewport(pCurrentContext);
                }
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2VIEWPORTINFO, 1, 0);
                break;
            }
            /*
             *
             */
            case D3DDP2OP_WINFO:
                pCurrentContext->dvWNear = ((LPD3DHAL_DP2WINFO)lpPrim)->dvWNear;
                pCurrentContext->dvWFar  = ((LPD3DHAL_DP2WINFO)lpPrim)->dvWFar;
                /*
                 * If the WFar value is 0.0, then set a default based on the z-buffer precision.
                 */
                if (pCurrentContext->dvWFar == 0.0f)
                {
                    pCurrentContext->dvWNear = 0.0f;
                    if ((pCurrentContext->dwSurfaceFormat == (DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH) | DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_X1R5G5B5_Z1R5G5B5)))
                     || (pCurrentContext->dwSurfaceFormat == (DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH) | DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_R5G6B5))))
                    {
                        pCurrentContext->dvWFar  = (D3DVALUE)(1 << 16);
                    }
                    else
                    {
                        pCurrentContext->dvWFar  = (D3DVALUE)(1 << 24);
                    }
                }
//                pCurrentContext->dvRWFar = 1.0f / ((LPD3DHAL_DP2WINFO)lpPrim)->dvWFar;
                pCurrentContext->dvRWFar = ((LPD3DHAL_DP2WINFO)lpPrim)->dvWFar;
                NEXTINSTRUCTION(lpCommands, D3DHAL_DP2WINFO, 1, 0);
                break;

            /*
             * Unknown command.  Pass it to the Unknown Command Callback for handling.
             */
            default:
                if (fnD3DParseUnknownCommandCallback)
                {
                    ddrval=fnD3DParseUnknownCommandCallback(lpCommands, &lpResumeCommands);
                    if (ddrval != DD_OK)
                    {
                        nvStartDmaBuffer (TRUE);
                        pdp2d->ddrval = ddrval;
                        pdp2d->dwErrorOffset = (DWORD)lpCommands - dwCommandBufferStart;
                        NV_D3D_GLOBAL_SAVE();
#ifdef  CACHE_FREECOUNT
                        pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
                        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
#ifdef NV_PROFILE
                        NVP_STOP (NVP_T_DP2);
                        nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif
                        return (DDHAL_DRIVER_HANDLED);
                    }

                    /*
                     * Set the resume address.
                     */
                    lpCommands = lpResumeCommands;
                }
                else
                {
                    nvStartDmaBuffer (TRUE);
                    pdp2d->ddrval = D3DERR_COMMAND_UNPARSED;
                    pdp2d->dwErrorOffset = (DWORD)lpCommands - dwCommandBufferStart;
                    NV_D3D_GLOBAL_SAVE();
#ifdef  CACHE_FREECOUNT
                    pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
                    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
#ifdef NV_PROFILE
                    NVP_STOP (NVP_T_DP2);
                    nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif
                    return (DDHAL_DRIVER_HANDLED);
                }
                break;
        }
    }
    nvStartDmaBuffer (TRUE);
    pDriverData->TwoDRenderingOccurred   = 0;
    pDriverData->ThreeDRenderingOccurred = TRUE;
    NV_D3D_GLOBAL_SAVE();
    dbgFlushDDI(pCurrentContext);

#ifdef  CACHE_FREECOUNT
    pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT

#ifdef NV_PROFILE
    NVP_STOP (NVP_T_DP2);
    nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif

    pdp2d->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}
#endif  // NVD3D_DX6
#endif  // NV4
