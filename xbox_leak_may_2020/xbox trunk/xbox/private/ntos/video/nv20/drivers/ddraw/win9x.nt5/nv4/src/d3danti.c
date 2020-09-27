#ifdef NV4
/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: D3DANTI.C                                                         *
*   AntiAliased Triangles                                                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal (bdw)           04/27/97 - NV4 port                     *
*                                                                           *
\***************************************************************************/

#include <math.h>
#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "d3dinc.h"
#include "nv3ddbg.h"
#include "nvddobj.h"
#include "nv4vxmac.h"
#include "nvheap.h"

/*****************************************************************************

SuperSampled AA Algorithm:

  * Calls to Clear only logs the color and zbuffer value
  * Calls to render geometry are captured and sorted into buckets. nothing
     is rendered at this point
  * All state are logged as well
  * at EndScene we render the buckets one by one into the super buffer and
     filter it down to screen.
  * ZBuffer will be corrupt at EndScene.

Limitations:
  * Multiple Begin/EndScene apps that rely on z-buffer will not work
  * Texture destroy or blts during the scene will corrupt at playback time
     since the texels are not captured

******************************************************************************

Edge AA Algorithm:

  * Calls to clear are performed normally
  * Calls to render geometry are captured and also rendered. Edges are
     placed in tables for 2nd pass (only for triangles that do not
     alpha blend)
  * All state are logged
  * At EndScene we render the edges as quads
  * ZBuffer will be okay

Limitations:
  * Multiple Begin/EndScene apps with 2d in between will occationally have
     edges appear over 2D.
  * Texture destroy or blts during the scene will corrupt edges at playback
     time since the texels are not captured

*****************************************************************************/

/*
 * constants
 */
#define AA_ALIGNPITCH                       32
#define AA_MIN                              -2047.0f
#define AA_MAX                               2047.0f

#define VF_NEGX                             0x00000001
#define VF_POSX                             0x00000002
#define VF_NEGY                             0x00000004
#define VF_POSY                             0x00000008
#define VF_OUTCODEMASK                      0x0000000f
#define VF_RESTORE                          0x80000000

/*
 * macros
 */
#define AA_ALLOC(x)                         while ((dwIndex + (x)) >= dwSize) { dwSize += 4096; dwAddr=(DWORD)ReallocIPM((void*)dwAddr,dwSize); if (!dwAddr) return FALSE; }
#define AACOMBINE(a,b)                      (((a)<<16)|(b))
#define ISDX6(x)                            ((x) == ((sizeDx6TriangleTLVertex - 1) * 4))

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
 * externals
 */
#ifndef WINNT
FLATPTR EXTERN_DDAPI DDHAL32_VidMemAlloc  (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, DWORD dwXDim, DWORD dwYDim);
void    EXTERN_DDAPI DDHAL32_VidMemFree   (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, FLATPTR ptr);
#endif
void nvSetClearSurface  (DWORD,DWORD,DWORD,DWORD,DWORD);
void nvClearSurfaceRect (DWORD,DWORD,DWORD,DWORD);
void nvReset2DSurfaceFormat (void);
void nvAASingleChunkRenderPrimitive (DWORD,LPWORD,DWORD,LPBYTE);

/*
 * tables
 */
typedef struct
{
    float fScaleX;
    float fScaleY;
    DWORD dwSrcPass1;
    DWORD dwDXPass1;
    DWORD dwDYPass1;
    DWORD dwSrcPass2;
    DWORD dwDXPass2;
    DWORD dwDYPass2;
    DWORD dwDU;
    DWORD dwDV;
} NVAATABLE;

NVAATABLE nvAATable[] =
{
    //* test */{ 1.0f, 1.0f, 0x00000000,(1<<20),(1<<20), 0,0,0 },
    /*0*/ { 2.0f, 1.0f, 0x00000008,(2<<20),(1<<20), 0,0,0,                      (1<<20)/2,(1<<20)/1}, // 2x1
    /*1*/ { 2.0f, 2.0f, 0x00080008,(2<<20),(2<<20), 0,0,0,                      (1<<20)/2,(1<<20)/2}, // 2x2
    /*2*/ { 3.0f, 2.0f, 0x00000008,(1<<20),(1<<20), 0x00080008,(3<<20),(2<<20), (1<<20)/3,(1<<20)/2}, // 3x2
    /*3*/ { 3.0f, 3.0f, 0x00080008,(1<<20),(1<<20), 0x00080008,(3<<20),(3<<20), (1<<20)/3,(1<<20)/3}, // 3x3
    /*4*/ { 4.0f, 2.0f, 0x00080008,(2<<20),(2<<20), 0x00000008,(2<<20),(1<<20), (1<<20)/4,(1<<20)/2}, // 4x2
    /*5*/ { 4.0f, 3.0f, 0x00000008,(1<<20),(1<<20), 0x00080008,(2<<20),(3<<20), (1<<20)/4,(1<<20)/3}, // 4x3
    /*6*/ { 4.0f, 4.0f, 0x00080008,(2<<20),(2<<20), 0x00080008,(2<<20),(2<<20), (1<<20)/4,(1<<20)/4}  // 4x4
};

/*
 * nvAACreate
 *
 * creates an AA context - called at createContext time
 */
BOOL nvAACreate
(
    void
)
{
    PNVAACONTEXT pCtx;

    DPF_LEVEL (NVDBG_LEVEL_FUNCTION_CALL, "nvAACreate");

    /*
     * assume AA unavailable
     */
    pCurrentContext->pAAContext       = NULL;
    pCurrentContext->dwAntiAliasFlags = 0;

    /*
     * allocate aa context memory
     */
    pCtx = pCurrentContext->pAAContext = (NVAACONTEXT*)AllocIPM(sizeof(NVAACONTEXT));
    if (!pCtx)
    {
        DPF_LEVEL (NVDBG_LEVEL_ERROR,"nvAACreate: failed to allocate AA Context");
        return FALSE;
    }

    /*
     * setup super sampling constants
     */
    pCtx->dwAASuperMode = pDriverData->regAAMethod;
    pCtx->fAAScaleX     = nvAATable[pCtx->dwAASuperMode].fScaleX;
    pCtx->fAAScaleY     = nvAATable[pCtx->dwAASuperMode].fScaleY;

    /*
     * initialize context
     */
    pCtx->dwChunkWidth  = pCurrentContext->lpLcl->lpGbl->wWidth  / (DWORD)pCtx->fAAScaleX;
    pCtx->dwSuperWidth  = pCurrentContext->lpLcl->lpGbl->wWidth;//pCtx->dwChunkWidth  * (DWORD)pCtx->fAAScaleX;
    pCtx->dwChunkHeight = pCurrentContext->lpLcl->lpGbl->wHeight / (DWORD)pCtx->fAAScaleY;
    pCtx->dwSuperHeight = pCurrentContext->lpLcl->lpGbl->wHeight;//pCtx->dwChunkHeight * (DWORD)pCtx->fAAScaleY;
    if (pCurrentContext->lpLcl->dwFlags & DDRAWISURF_HASPIXELFORMAT)
        pCtx->dwBPP     = pCurrentContext->lpLcl->lpGbl->ddpfSurface.dwRGBBitCount / 8;
    else
        pCtx->dwBPP     = pDriverData->bi.biBitCount / 8;
    pCtx->dwCFill       = 0x00000000;
    pCtx->dwZFill       = 0xffffffff;

    /*
     * success
     */
    return TRUE;
}

/*
 * nvAADestroy
 *
 * free up AA context memory
 */
void nvAADestroy
(
    void
)
{
    PNVAACONTEXT pCtx;

    DPF_LEVEL (NVDBG_LEVEL_FUNCTION_CALL, "nvAADestroy");

    /*
     * get context
     */
    pCtx = pCurrentContext->pAAContext;

    /*
     * free surface memory (if any)
     */
    if (pCtx->dwSuperAddr)
    {
        NVHEAP_FREE(pCtx->dwSuperAddr);
    }

    /*
     * free capture memory
     */
    if (pCtx->dwCapAddr)
    {
        FreeIPM ((void*)(pCtx->dwCapAddr));
    }

    /*
     * free context memory
     */
    FreeIPM (pCtx);

    /*
     * tag AA unavailable
     */
    pCurrentContext->pAAContext       = NULL;
    pCurrentContext->dwAntiAliasFlags = 0;
}

/*
 * nvAACreateBuffers
 *
 * create buffers for AA
 */
BOOL nvAACreateBuffers
(
    void
)
{
    PNVAACONTEXT pCtx;

    DPF_LEVEL (NVDBG_LEVEL_FUNCTION_CALL, "nvAACreateBuffers");

    /*
     * get context
     */
    pCtx = pCurrentContext->pAAContext;

    /*
     * assume buffers not allocated
     */
    pCurrentContext->dwAntiAliasFlags &= ~AA_BUFALLOC_MASK;

    switch (pCurrentContext->dwAntiAliasFlags & AA_ENABLED_MASK)
    {
        /*
         * allocate super sampled buffers
         */
        case AA_MODE_SUPER:
        case AA_MODE_SINGLECHUNK: // not selectable from registry - automatic detection
            {
                DWORD dwSize;

                /*
                 * attempt to allocate the whole super buffer for single-chunk super-sampled aa
                 */
                {
                    DWORD dwSCWidth;
                    DWORD dwSCHeight;
                    DWORD dwSCPitch;
                    DWORD dwSCFAddr = 0;
                    DWORD dwSCZAddr = 0;

                    /*
                     * calc width & check if it will fit to HW limits for single chunk
                     */
                    dwSCWidth = pCtx->dwSuperWidth * (DWORD)pCtx->fAAScaleX;
                    if (dwSCWidth <= AA_MAX)
                    {
                        dwSCHeight = pCtx->dwSuperHeight * (DWORD)pCtx->fAAScaleY;
                        dwSCPitch = ((dwSCWidth * pCtx->dwBPP) + AA_ALIGNPITCH - 1) & ~(AA_ALIGNPITCH - 1);
                        dwSize    = dwSCPitch * dwSCHeight;
                        NVHEAP_ALLOC (dwSCFAddr,dwSize,TYPE_IMAGE);
                        if (dwSCFAddr)
                        {
                            NVHEAP_ALLOC (dwSCZAddr,dwSize,TYPE_DEPTH);
                            if (dwSCZAddr)
                            {
                                /*
                                 * enough memory present - select SCSSAA
                                 */
                                pCtx->dwChunkWidth   = pCtx->dwSuperWidth;   // the window width
                                pCtx->dwSuperWidth   = dwSCWidth;            // scaled width
                                pCtx->dwChunkHeight  = pCtx->dwSuperHeight;  // the window height
                                pCtx->dwSuperHeight  = dwSCHeight;           // scaled height

                                pCtx->dwSuperAddr    = dwSCFAddr;
                                pCtx->dwSuperZAddr   = dwSCZAddr;
                                pCtx->dwSuperOffset  = dwSCFAddr - pDriverData->BaseAddress;
                                pCtx->dwSuperZOffset = dwSCZAddr - pDriverData->BaseAddress;
                                pCtx->dwSuperPitch   = dwSCPitch;
                                pCtx->dwSuperZPitch  = dwSCPitch;

                                /*
                                 * tag buffers allocated
                                 */
                                pCurrentContext->dwAntiAliasFlags &= ~AA_ENABLED_MASK;
                                pCurrentContext->dwAntiAliasFlags |=  AA_MODE_SINGLECHUNK
                                                                  |   AA_BUFALLOC_SINGLECHUNK
                                                                  |   AA_CLEAR_SCREEN;

                                /*
                                 * clear the buffers - courtesy to app
                                 */
                                memset ((void*)pCtx->dwSuperAddr,0,dwSize);
                                memset ((void*)pCtx->dwSuperZAddr,0xff,dwSize);

                                /*
                                 * success
                                 */
                                return TRUE;
                            }
                            else
                            {
                                /*
                                 * free frame buffer
                                 */
                                NVHEAP_FREE (dwSCFAddr);
                                dwSCFAddr = 0;
                            }
                        }
                    }
                }

                /*
                 * compute super buffer size for chunking super sampled AA
                 */
                pCtx->dwSuperPitch = ((pCtx->dwSuperWidth * pCtx->dwBPP) + AA_ALIGNPITCH - 1) & ~(AA_ALIGNPITCH - 1);
                dwSize             = pCtx->dwSuperPitch * pCtx->dwSuperHeight;

                /*
                 * allocate the buffer (allow for alignment)
                 */
                NVHEAP_ALLOC (pCtx->dwSuperAddr,dwSize,TYPE_IMAGE);
                if (!pCtx->dwSuperAddr)
                {
                    /*
                     * fail to allocate memory
                     */
                    return FALSE;
                }

                /*
                 * setup super surface
                 */
                pCtx->dwSuperOffset = pCtx->dwSuperAddr - pDriverData->BaseAddress;

                /*
                 * tag buffers allocated
                 */
                pCurrentContext->dwAntiAliasFlags |= AA_BUFALLOC_SUPER
                                                  |  AA_CLEAR_SCREEN;
            }
            break;
        /*
         * allocate edge buffers
         */
        case AA_MODE_EDGE:
            {
            }
            return FALSE;
            // aatodo - implement

            /*
             * tag buffers allocated
             */
            pCurrentContext->dwAntiAliasFlags |= AA_BUFALLOC_EDGE;
            break;
    }

    /*
     * allocate geometry capture area
     */
    pCtx->dwCapIndex = 0;
    pCtx->dwCapSize  = 4096;
    pCtx->dwCapAddr  = (DWORD)AllocIPM(pCtx->dwCapSize);
    if (!pCtx->dwCapAddr)
    {
        /*
         * not enough memory - fail
         */
        pCurrentContext->dwAntiAliasFlags &= ~AA_BUFALLOC_MASK;
        return FALSE;
    }

    /*
     * success
     */
    return TRUE;
}

/*
 * nvAACapturePrimitive
 *
 * capture triangles for 2nd pass. aa mode == edge will pass this batch
 * through to be rendered as well.
 */
BOOL __stdcall nvAACapturePrimitive
(
    DWORD  nPrimCount,
    LPWORD pIndices,        // NULL for non-indexed cases
    DWORD  dwStrides,
    LPBYTE pVertices
)
{
    PNVAACONTEXT pCtx;
    DWORD        dwOldFifo;
    DWORD        dwOldFreeCount;
#ifndef NV_FASTLOOPS
    DWORD        dwFlags;
#endif

    DPF_LEVEL (NVDBG_LEVEL_FUNCTION_CALL, "nvAACapturePrimitive");
    DPF_LEVEL (NVDBG_LEVEL_AA_INFO,"nvAACapturePrimitive: aa = %08x",pCurrentContext->dwAntiAliasFlags);

    /*
     * check for single-chunking
     */
    if (pCurrentContext->dwAntiAliasFlags & AA_MODE_SINGLECHUNK)
    {
        /*
         * render triangles
         */
        nvAASingleChunkRenderPrimitive (nPrimCount,pIndices,dwStrides,pVertices);

        /*
         * success
         */
        return TRUE;
    }

    /*
     * get context
     */
    pCtx = pCurrentContext->pAAContext;

    /*
     * override fifo
     */
    dwOldFifo      = (DWORD)nvFifo;
    dwOldFreeCount = nvFreeCount;
    nvFifo         = pCtx->dwCapAddr + pCtx->dwCapIndex;
    nvFreeCount    = (pCtx->dwCapSize - pCtx->dwCapIndex) >> 2;
#ifndef NV_FASTLOOPS
    dwFlags        = _CALC_FUNCTION_INDEX(pCurrentContext);
#endif

    /*
     * start fifo capture
     */
    pCurrentContext->dwAntiAliasFlags |= AA_CAPTURE_PUSHBUFFER;

    /*
     * render
     */
    pDriverData->dDrawSpareSubchannelObject = 0;

#ifndef NV_FASTLOOPS
    if (pCurrentContext->bUseDX6Class)
    {
        /*
         * force state load
         */
        nvSetDx6MultiTextureState(pCurrentContext);
        /*
         * dispatch triangles
         */
        if (pIndices)
            fnDX6FlexIndexedTable[dwFlags](nPrimCount,pIndices,dwStrides,pVertices);
        else
            fnDX6FlexTable[dwFlags]((WORD)nPrimCount,dwStrides,pVertices);
    }
    else
    {
        /*
         * force state load
         */
        nvSetDx5TriangleState(pCurrentContext);
        /*
         * dispatch triangles
         */
        if (pIndices)
            fnDX5FlexIndexedTable[dwFlags](nPrimCount,pIndices,dwStrides,pVertices);
        else
            fnDX5FlexTable[dwFlags]((WORD)nPrimCount,dwStrides,pVertices);
    }
#else //NV_FASTLOOPS
    /*
     * force state update
     */
    pCurrentContext->dwStateChange    = TRUE;
    pCurrentContext->dwFunctionLookup = NV_SETUP_FUNCTION_INDEX;
    /*
     * draw them
     */
    nvTriangleDispatch(nPrimCount,pIndices,dwStrides,pVertices);
    /*
     * force innerloop reset since the capturing spoofed the index
     */
    pCurrentContext->dwFunctionLookup = NV_SETUP_FUNCTION_INDEX;
#endif //NV_FASTLOOPS

    /*
     * reset fifo
     */
    pCurrentContext->dwAntiAliasFlags &= ~AA_CAPTURE_PUSHBUFFER;
    pCtx->dwCapIndex = nvFifo - pCtx->dwCapAddr;
    nvFifo           = dwOldFifo;
    nvFreeCount      = dwOldFreeCount;

    /*
     * kill subchannel
     */
    pDriverData->dDrawSpareSubchannelObject = 0;

    /*
     * success
     */
    return TRUE;
}

/*
 * nvAABlt
 *
 * Arbitrary filter blt
 */
void __inline nvAABlt
(
    DWORD dwSrcOffset,
    DWORD dwSrcPitch,
    DWORD dwSrcYX,          // 12.4 12.4
    DWORD dwSrcHW,
    DWORD dwDstOffset,
    DWORD dwDstPitch,
    DWORD dwDstYX,
    DWORD dwDstHW,
    DWORD dwClpYX,
    DWORD dwClpHW,
    DWORD dwDUDX,
    DWORD dwDVDY,
    DWORD dwFormat
)
{
    while (nvFreeCount < 21) nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, 21);
    *(DWORD*)(nvFifo+ 0) = dDrawSubchannelOffset(NV_DD_ROP) + SET_ROP_OFFSET | 0x40000;
    *(DWORD*)(nvFifo+ 4) = SRCCOPYINDEX;
    *(DWORD*)(nvFifo+ 8) = dDrawSubchannelOffset(NV_DD_SURFACES) + SURFACES_2D_DEST_OFFSET | 0x40000;
    *(DWORD*)(nvFifo+12) = dwDstOffset;
    *(DWORD*)(nvFifo+16) = dDrawSubchannelOffset(NV_DD_SURFACES) + SURFACES_2D_PITCH_OFFSET | 0x40000;
    *(DWORD*)(nvFifo+20) = (dwDstPitch << 16) | dwSrcPitch;
    *(DWORD*)(nvFifo+24) = dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000;
    *(DWORD*)(nvFifo+28) = NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
    *(DWORD*)(nvFifo+32) = dDrawSubchannelOffset(NV_DD_STRETCH) + SCALED_IMAGE_CLIPPOINT_OFFSET | 0x80000;
    *(DWORD*)(nvFifo+36) = dwClpYX;                  // clip point
    *(DWORD*)(nvFifo+40) = dwClpHW;                  // clip size
    *(DWORD*)(nvFifo+44) = dDrawSubchannelOffset(NV_DD_STRETCH) + SCALED_IMAGE_IMAGE_OUT_POINT_OFFSET | 0x100000;
    *(DWORD*)(nvFifo+48) = dwDstYX;                  // dst point
    *(DWORD*)(nvFifo+52) = dwDstHW;                  // dst size
    *(DWORD*)(nvFifo+56) = dwDUDX;                   // dudx
    *(DWORD*)(nvFifo+60) = dwDVDY;                   // dvdy
    *(DWORD*)(nvFifo+64) = dDrawSubchannelOffset(NV_DD_STRETCH) + SCALED_IMAGE_IMAGE_IN_SIZE_OFFSET | 0x100000;
    *(DWORD*)(nvFifo+68) = dwSrcHW;                  // src size
    *(DWORD*)(nvFifo+72) = dwSrcPitch | dwFormat; // src format
    *(DWORD*)(nvFifo+76) = dwSrcOffset;              // src offset
    *(DWORD*)(nvFifo+80) = dwSrcYX;                  // src point x.4
    nvFifo      += 84;
    nvFreeCount -= 21;

    pDriverData->dDrawSpareSubchannelObject = 0;
    //pDriverData->TwoDRenderingOccurred   = TRUE;
    pDriverData->ThreeDRenderingOccurred = 0;
}

/*
 * nvAACut
 *
 * subdivides an edge in 2. with t==0 c=a, t==1 c=b.
 */
void __inline nvAACut
(
    float t,
    DWORD len,
    DWORD a,
    DWORD b,
    DWORD c
)
{
    PREFETCH (a);
    PREFETCH (b);

    /*
     * boundary cases
     */
    if (t <= 0.0f)
    {
        memcpy ((void*)c,(void*)a,len);
        return;
    }
    if (t >= 1.0f)
    {
        memcpy ((void*)c,(void*)b,len);
        return;
    }

    /*
     * harder (real-world) case
     */
    if (ISDX6(len))
    {
        PREFETCH (a+32);
        PREFETCH (b+32);
    }
    {
        float nt   = 1.0f - t;
        float w,rhw,rhw0,rhw1;

        /*
         * sx,sy,sz
         */
        *(float*)(c+0) = nt * (*(float*)(a+0)) + t * (*(float*)(b+0));
        *(float*)(c+4) = nt * (*(float*)(a+4)) + t * (*(float*)(b+4));
        *(float*)(c+8) = nt * (*(float*)(a+8)) + t * (*(float*)(b+8));

        /*
         * 1/w aka rhw
         */
        rhw0 = *(float*)(a+12);
        rhw1 = *(float*)(b+12);
        *(float*)(c+12) = rhw = nt * rhw0 + t * rhw1;
        w   = (rhw == 0.0f) ? 1.0f : (1.0f / rhw);
        t  *= w * rhw1;
        nt *= w * rhw0;

        /*
         * color
         */
        {
            DWORD v0 = *(DWORD*)(a+16);
            DWORD v1 = *(DWORD*)(b+16);
            if (v0 == v1)
            {
                *(DWORD*)(c+16) = v0;
            }
            else
            {
                DWORD a = 0xff000000 & (DWORD)(nt * (float)(v0 & 0xff000000) + t * (float)(v1 & 0xff000000));
                DWORD r = 0x00ff0000 & (DWORD)(nt * (float)(v0 & 0x00ff0000) + t * (float)(v1 & 0x00ff0000));
                DWORD g = 0x0000ff00 & (DWORD)(nt * (float)(v0 & 0x0000ff00) + t * (float)(v1 & 0x0000ff00));
                DWORD b = 0x000000ff & (DWORD)(nt * (float)(v0 & 0x000000ff) + t * (float)(v1 & 0x000000ff));
                *(DWORD*)(c+16) = a | r | g | b;
            }
        }

        /*
         * specular
         */
        {
            DWORD v0 = *(DWORD*)(a+20);
            DWORD v1 = *(DWORD*)(b+20);
            if (v0 == v1)
            {
                *(DWORD*)(c+20) = v0;
            }
            else
            {
                DWORD a = 0xff000000 & (DWORD)(nt * (float)(v0 & 0xff000000) + t * (float)(v1 & 0xff000000));
                DWORD r = 0x00ff0000 & (DWORD)(nt * (float)(v0 & 0x00ff0000) + t * (float)(v1 & 0x00ff0000));
                DWORD g = 0x0000ff00 & (DWORD)(nt * (float)(v0 & 0x0000ff00) + t * (float)(v1 & 0x0000ff00));
                DWORD b = 0x000000ff & (DWORD)(nt * (float)(v0 & 0x000000ff) + t * (float)(v1 & 0x000000ff));
                *(DWORD*)(c+20) = a | r | g | b;
            }
        }

        /*
         * uv0
         */
        *(float*)(c+24) = nt * (*(float*)(a+24)) + t * (*(float*)(b+24));
        *(float*)(c+28) = nt * (*(float*)(a+28)) + t * (*(float*)(b+28));

        /*
         * uv1
         */
        if (ISDX6(len))
        {
            *(float*)(c+32) = nt * (*(float*)(a+32)) + t * (*(float*)(b+32));
            *(float*)(c+36) = nt * (*(float*)(a+36)) + t * (*(float*)(b+36));
        }
    }
}

/*
 * nvAAClip
 *
 * clip triangle to AA_MIN - AA_MAX
 */
void __inline nvAAClip
(
    DWORD i0,
    DWORD i1,
    DWORD i2,
    DWORD len,
    float *XYCache,
    DWORD *VCache,
    DWORD *VFlags
)
{
    DWORD vertex[2 * 6 * 16];
    DWORD *vx0,*vx1,vm0,vm1;
    DWORD o,e,i;

    /*
     * trivial reject
     */
    if (VFlags[i0] & VFlags[i1] & VFlags[i2] & VF_OUTCODEMASK) return;

    /*
     * do the evil copy of vertices. Really simplifies coding. Viva Data Cache.
     */
    vertex[0*16 + 0] = *(DWORD*)&XYCache[i0*2+0];
    vertex[0*16 + 1] = *(DWORD*)&XYCache[i0*2+1];
    memcpy (vertex + 0*16 + 2,(void*)(VCache[i0]+8),len-8);
    vertex[1*16 + 0] = *(DWORD*)&XYCache[i1*2+0];
    vertex[1*16 + 1] = *(DWORD*)&XYCache[i1*2+1];
    memcpy (vertex + 1*16 + 2,(void*)(VCache[i1]+8),len-8);
    vertex[2*16 + 0] = *(DWORD*)&XYCache[i2*2+0];
    vertex[2*16 + 1] = *(DWORD*)&XYCache[i2*2+1];
    memcpy (vertex + 2*16 + 2,(void*)(VCache[i2]+8),len-8);

    /*
     * setup
     */
    vx0 = vertex + 0*16;
    vm0 = 3;
    vx1 = vertex + 6*16;
    vm1 = 0;
    o   = (VFlags[i0] | VFlags[i1] | VFlags[i2]) & VF_OUTCODEMASK;

    /*
     * for four edges
     */
    for (e=1; e<16; e<<=1)
    {
        /*
         * quick edge clip reject
         */
        if (!(o & e))
            continue;

        /*
         * traverse triangle
         */
        for (i=0; i<vm0; i++)
        {
            float edge;
            DWORD index;
            DWORD j  = (i+1) % vm0;
            float *vi = ((float*)vx0) + i*16;
            float *vj = ((float*)vx0) + j*16;
            DWORD oi = 0;
            DWORD oj = 0;

            /*
             * determine outcodes
             */
            switch (e)
            {
                case 1: if (vi[0] < AA_MIN) oi=1;
                        if (vj[0] < AA_MIN) oj=1;
                        edge = AA_MIN; index = 0;
                        break;
                case 2: if (vi[0] > AA_MAX) oi=1;
                        if (vj[0] > AA_MAX) oj=1;
                        edge = AA_MAX; index = 0;
                        break;
                case 4: if (vi[1] < AA_MIN) oi=1;
                        if (vj[1] < AA_MIN) oj=1;
                        edge = AA_MIN; index = 1;
                        break;
                case 8: if (vi[1] > AA_MAX) oi=1;
                        if (vj[1] > AA_MAX) oj=1;
                        edge = AA_MAX; index = 1;
                        break;
            }

            /*
             * outside-outside
             */
            if (oi && oj)
            {
                continue;
            }

            /*
             * inside-outside or outside-inside
             */
            if (oi != oj) // (!oi && oj) || (oi && !oj)
            {
                if (vj[index] > vi[index])
                {
                    float t = vj[index] - vi[index];
                          t = (t == 0.0f) ? 0.5f : ((edge - vi[index]) / t);
                    nvAACut (t,len,(DWORD)vi,(DWORD)vj,(DWORD)&vx1[vm1*16]);
                    vm1++;
                }
                else
                {
                    float t = vi[index] - vj[index];
                          t = (t == 0.0f) ? 0.5f : ((edge - vj[index]) / t);
                    nvAACut (t,len,(DWORD)vj,(DWORD)vi,(DWORD)&vx1[vm1*16]);
                    vm1++;
                }
            }

            /*
             * inside-inside or outside-inside
             */
            if (!oj) // (!oi && !oj) || (oi && !oj)
            {
                memcpy (&vx1[vm1*16],vj,len);
                vm1++;
            }
        }

        /*
         * early exit
         */
        if (vm1 < 3) return;

        /*
         * next edge
         */
        {
            DWORD *t = vx0; vx0 = vx1; vx1 = t;
        }
        vm0 = vm1;
        vm1 = 0;
    }

    /*
     * now that we have clipped - emit triangle fan
     */
    {
        DWORD f[16]; // table to make sure we do not punt used vertices
        DWORD m = ISDX6(len) ? 7 : 15;
        DWORD launch;

        /*
         * tag all as unused
         */
        memset (f,0,sizeof(f));

        /*
         * for all vertices in clipped polygon
         */
        for (i=0; i<vm0; i++)
        {
            DWORD j;

            /*
             * displace vertex entry
             */
            for (j=m; j>0; j--)
            {
                /*
                 * find 1) an uninitialized or 2) invalid entry or 3) an entry that needs restoring already
                 */
                if (!f[j] && (!VCache[j] || VFlags[j]))
                {
                    /*
                     * tag as used & no need to restore
                     */
                    f[j] = 1;
                    goto emit;
                }
            }
            for (j=m; j>0; j--)
            {
                /*
                 * find any entry that we haven't used before
                 */
                if (!f[j])
                {
                    /*
                     * tag as used & need restore
                     */
                    f[j] = 1;
                    VFlags[j] |= VF_RESTORE;
                    goto emit;
                }
            }
            /*
             * control cannot go here unless we clip a triangle into more than 15 vertices and
             *  since we only have 4 clip planes you cannot end up with more than 6.
             * j == 0 at this point which will not cause a crash but will cause degenerate
             *  triangles.
             */
#ifdef DEBUG
            __asm int 3;
#endif

            /*
             * emit vertex
             */
        emit:
            if (ISDX6(len))
            {
                while (nvFreeCount < sizeDx6TriangleTLVertex)
                    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeDx6TriangleTLVertex);
                *(DWORD*)(nvFifo + 0) = ((sizeDx6TriangleTLVertex-1)<<18) | ((NV_DD_SPARE)<<13) | NV055_TLMTVERTEX((j));
                memcpy ((void*)(nvFifo+4),vx0+i*16,len);
                nvFifo      += sizeDx6TriangleTLVertex * 4;
                nvFreeCount -= sizeDx6TriangleTLVertex;
            }
            else
            {
                while (nvFreeCount < sizeDx5TriangleTLVertex)
                    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeDx5TriangleTLVertex);
                *(DWORD*)(nvFifo + 0) = ((sizeDx5TriangleTLVertex-1)<<18) | ((NV_DD_SPARE)<<13) | NV054_TLVERTEX((j));
                memcpy ((void*)(nvFifo+4),vx0+i*16,len);
                nvFifo      += sizeDx5TriangleTLVertex * 4;
                nvFreeCount -= sizeDx5TriangleTLVertex;
            }
            /*
             * store launch codes & launch if needed
             */
            if (i==0)
                launch  = j << 8;
            else
            if (i==1)
                launch |= j << 4;
            else
            {
                launch |= j;
                // note: assume sizeDx5TriangleDrawPrimitive == sizeDx6TriangleDrawPrimitive
                while (nvFreeCount < sizeDx5TriangleDrawPrimitive)
                    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeDx5TriangleDrawPrimitive);
                if (ISDX6(len))
                {
                    nvglDx6TriangleDrawPrimitive (nvFifo, nvFreeCount, NV_DD_SPARE, 0, launch);
                }
                else
                {
                    nvglDx5TriangleDrawPrimitive (nvFifo, nvFreeCount, NV_DD_SPARE, 0, launch);
                }
                launch = (launch & 0xf00) | (j << 4);
            }
        }
    }
}

/*
 * nvAARenderPrimitive
 *
 * play back captured data - called at endScene
 */
BOOL nvAARenderPrimitive
(
    void
)
{
    PNVAACONTEXT pCtx;
    DWORD        dwAddr;
    DWORD        dwIndex;
    DWORD        dwScreenOffset;
    DWORD        dwScreenPitch;
    DWORD        dwScreenHW;
    DWORD        dwScreenYX;
    DWORD        dwSuperHW;
    DWORD        dwChunkHW;
    DWORD        dwClipYX;
    DWORD        dwClipHW;
    float        fAAScaleX;
    float        fAAScaleY;
    float        fAAOffsetX;
    float        fAAOffsetY;

    DPF_LEVEL (NVDBG_LEVEL_FUNCTION_CALL, "nvAARenderPrimitive");
    DPF_LEVEL (NVDBG_LEVEL_AA_INFO,"nvAARenderPrimitive: aa = %08x",pCurrentContext->dwAntiAliasFlags);

    /*
     * get context
     */
    pCtx           = pCurrentContext->pAAContext;

    /*
     * get state
     */
    dwAddr         = pCtx->dwCapAddr;
    dwIndex        = pCtx->dwCapIndex;
    dwScreenOffset = pCurrentContext->dwSurfaceOffset;
    dwScreenPitch  = pCurrentContext->surfacePitch.wColorPitch;
    dwScreenHW     = AACOMBINE(pCurrentContext->lpLcl->lpGbl->wHeight,pCurrentContext->lpLcl->lpGbl->wWidth);
    dwSuperHW      = AACOMBINE(pCtx->dwSuperHeight,pCtx->dwSuperWidth);
    dwChunkHW      = AACOMBINE(pCtx->dwChunkHeight,pCtx->dwChunkWidth);
    fAAScaleX      = pCtx->fAAScaleX;
    fAAScaleY      = pCtx->fAAScaleY;
    dwClipYX       = AACOMBINE(pCurrentContext->surfaceViewport.clipVertical.wY,pCurrentContext->surfaceViewport.clipHorizontal.wX);
    dwClipHW       = AACOMBINE(pCurrentContext->surfaceViewport.clipVertical.wHeight,pCurrentContext->surfaceViewport.clipHorizontal.wWidth);

    /*
     * get an accurate freecount reading
     */
/*
    nvFreeCount = nvGetDmaFreeCount(1);
*/

/*
    //early abort
    pCtx->dwCapIndex = 0;
    return 1;
/**/

    /*
     * play back
     */
    switch (pCurrentContext->dwAntiAliasFlags & AA_ENABLED_MASK)
    {
        case AA_MODE_SUPER:
        {
            DWORD VCache[16];
            DWORD VFlags[16];
            float XYCache[32];
            float chunkW = pCtx->fAAScaleX * (float)pCtx->dwChunkWidth;
            float chunkH = pCtx->fAAScaleY * (float)pCtx->dwChunkHeight;
            DWORD scrnX;
            DWORD scrnY;
            DWORD chunkX;
            DWORD chunkY;
            DWORD chunkMaxX = (DWORD)pCtx->fAAScaleX;
            DWORD chunkMaxY = (DWORD)pCtx->fAAScaleY;
            DWORD dwRectColorFormat;
            DWORD dwSurfColorFormat;

            /*
             * early exit if nothing todo
             */
            if (!pCtx->dwCapIndex)
            {
                return TRUE;
            }

            /*
             * setup
             */
            switch (pCtx->dwBPP)
            {
                case 2:
                    dwRectColorFormat = NV05E_SET_COLOR_FORMAT_LE_X16R5G6B5;
                    dwSurfColorFormat = NV042_SET_COLOR_FORMAT_LE_Y16;
                    break;
                case 3:
                case 4:
                    dwRectColorFormat = NV05E_SET_COLOR_FORMAT_LE_X8R8G8B8;
                    dwSurfColorFormat = NV042_SET_COLOR_FORMAT_LE_Y32;
                    break;
            }

            /*
             * set render phase tag
             */
            pCurrentContext->dwAntiAliasFlags |= AA_RENDER;
            pDriverData->lpLast3DSurfaceRendered = 0;
            nvSetD3DSurfaceState (pCurrentContext);

            /*
             * for all chunks
             */
            fAAOffsetY = 0.0f;
            scrnY      = 0;
            for (chunkY=0; chunkY<chunkMaxY; chunkY++)
            {
                fAAOffsetX = 0.0f;
                scrnX      = 0;

                for (chunkX=0; chunkX<chunkMaxX; chunkX++)
                {
                    DWORD ptr,end,cls;

                    /*
                     * setup
                     */
                    dwScreenYX = AACOMBINE(scrnY,scrnX);

                    /*
                     * clear z buffer and frame buffer of scaled chunk
                     */
                    if (pCurrentContext->lpLclZ)
                    {
                        nvSetClearSurface(
                            pCurrentContext->lpLclZ->lpGbl->fpVidMem - pDriverData->BaseAddress,
                            (WORD)pCurrentContext->lpLclZ->lpGbl->lPitch,
                            dwSurfColorFormat,dwRectColorFormat,pCtx->dwZFill);
                        nvClearSurfaceRect(0,0,pCtx->dwSuperWidth,pCtx->dwSuperHeight);
                    }
                    if (pCurrentContext->dwAntiAliasFlags & AA_CLEAR_SCREEN)
                    {
                        nvSetClearSurface(
                            pCtx->dwSuperOffset,pCtx->dwSuperPitch,
                            dwSurfColorFormat,dwRectColorFormat,pCtx->dwCFill);
                        nvClearSurfaceRect(0,0,pCtx->dwSuperWidth,pCtx->dwSuperHeight);
                    }
                    else
                    {
                        nvReset2DSurfaceFormat();
                        nvAABlt (dwScreenOffset,dwScreenPitch,          dwScreenYX<<4,dwScreenHW,
                                 pCtx->dwSuperOffset,pCtx->dwSuperPitch,0x00000000,dwSuperHW,
                                                                        0x00000000,dwSuperHW,
                                 nvAATable[pCtx->dwAASuperMode].dwDU,nvAATable[pCtx->dwAASuperMode].dwDV,
                                 (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24) | (NV077_IMAGE_IN_FORMAT_ORIGIN_CORNER << 16));
                    }
                    nvReset2DSurfaceFormat();

                    /*
                     * blt target chunk to scaled chunk
                     */
                    /* keep for future reference
                    nvAABlt (dwScreenOffset,dwScreenPitch,          dwScreenYX<<4,dwChunkHW,
                             pCtx->dwSuperOffset,pCtx->dwSuperPitch,0,dwSuperHW,
                                                                    0,dwSuperHW,
                             (1<<20)/AA_SCALE,(1<<20)/AA_SCALE,
                             (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24) | (NV077_IMAGE_IN_FORMAT_ORIGIN_CORNER << 16));
                    */

                    /*
                     * play back geometry
                     */
                    ptr = dwAddr;
                    end = dwAddr + dwIndex;
                    cls = 0;
                    memset (VCache,0,sizeof(VCache)); // reset cache for clipper vc displacement algo
                    memset (VFlags,0,sizeof(VFlags));
                    while (ptr < end)
                    {
                        DWORD data = *(DWORD*)ptr;
                        DWORD size = 1 + ((data >> 18) & 0x3f);
                        DWORD skip = size << 2;

                        /*
                        if (nvFreeCount < 128)
                        {
                             nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, 128);
                        }
                        */
                        /*
                        if (data & 0xc0000000)
                        {
                            __asm lea eax,dword ptr [global]
                            __asm int 3;
                        }
                        /**/

                        switch (data)
                        {
                            /*
                             * object
                             */
                            case (0x040000 | (NV_DD_SPARE << 13)):
                            {
                                cls = *(DWORD*)(ptr+4);
                                goto copy;
                            }
                            break;
                            /*
                             * tri launch
                             */
                            case (0x040000 | (NV_DD_SPARE << 13) | NV054_DRAW_PRIMITIVE(0)):
                            case (0x040000 | (NV_DD_SPARE << 13) | NV055_DRAW_PRIMITIVE(0)):
                            {
                                DWORD len    = (data == (0x040000 | (NV_DD_SPARE << 13) | NV055_DRAW_PRIMITIVE(0)))
                                             ? ((sizeDx6TriangleTLVertex - 1) * 4)
                                             : ((sizeDx5TriangleTLVertex - 1) * 4);
                                DWORD launch = *(DWORD*)(ptr+4);

                                /*
                                 * extract vertices of triangle
                                 */
                                DWORD i0     = (launch >> 8)     ;
                                DWORD i1     = (launch >> 4) & 15;
                                DWORD i2     = (launch     ) & 15;

                                /*
                                 * check for clipping
                                 */
                                if ((VFlags[i0] | VFlags[i1] | VFlags[i2]) & VF_OUTCODEMASK)
                                {
                                    /*
                                     * clip
                                     */
                                    nvAAClip (i0,i1,i2,len,XYCache,VCache,VFlags);
                                    /*
                                     * suppress copy
                                     */
                                    break;
                                }

                                /*
                                 * restore punted vertices
                                 */
                                {
                                    DWORD i;
                                    for (i=0; i<3; i++)
                                    {
                                        DWORD x = launch & 15;
                                        launch >>= 4;
                                        if (VFlags[x] & VF_RESTORE)
                                        {
                                            VFlags[x] &= ~VF_RESTORE;
                                            if (ISDX6(len))
                                            {
                                                while (nvFreeCount < sizeDx6TriangleTLVertex)
                                                    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeDx6TriangleTLVertex);
                                                *(DWORD*)(nvFifo + 0) = ((sizeDx6TriangleTLVertex-1)<<18) | ((NV_DD_SPARE)<<13) | NV055_TLMTVERTEX((x));
                                                *(float*)(nvFifo + 4) = XYCache[x*2+0];
                                                *(float*)(nvFifo + 8) = XYCache[x*2+1];
                                                memcpy ((void*)(nvFifo+12),(void*)(VCache[x]+8),len-8);
                                                nvFifo      += sizeDx6TriangleTLVertex * 4;
                                                nvFreeCount -= sizeDx6TriangleTLVertex;
                                            }
                                            else
                                            {
                                                while (nvFreeCount < sizeDx5TriangleTLVertex)
                                                    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeDx5TriangleTLVertex);
                                                *(DWORD*)(nvFifo + 0) = ((sizeDx5TriangleTLVertex-1)<<18) | ((NV_DD_SPARE)<<13) | NV054_TLVERTEX((x));
                                                *(float*)(nvFifo + 4) = XYCache[x*2+0];
                                                *(float*)(nvFifo + 8) = XYCache[x*2+1];
                                                memcpy ((void*)(nvFifo+12),(void*)(VCache[x]+8),len-8);
                                                nvFifo      += sizeDx5TriangleTLVertex * 4;
                                                nvFreeCount -= sizeDx5TriangleTLVertex;
                                            }
                                        }
                                    }
                                }

                                /*
                                 * launch tri
                                 */
                                goto copy;
                            }
                            break;
                            /*
                             * something else
                             */
                            default:
                            {
                                DWORD method = data & 0x7ff;
                                /*
                                 * vertex
                                 */
                                if (((cls == D3D_DX5_TEXTURED_TRIANGLE)      && (method >= 0x400) && (method < 0x600))
                                 || ((cls == D3D_DX6_MULTI_TEXTURE_TRIANGLE) && (method >= 0x400) && (method < 0x540)))
                                {
                                    DWORD idx = (cls == D3D_DX5_TEXTURED_TRIANGLE) ? ((method-0x400)/0x20) : ((method-0x400)/0x28);
                                    float sx,sy;

                                    VCache[idx] = ptr+4;
                                    VFlags[idx] = VF_RESTORE; // load on demand

                                    sx = *(float*)(ptr+4) * fAAScaleX + fAAOffsetX;
                                    if (sx < AA_MIN) VFlags[idx] |= VF_NEGX; else
                                    if (sx > AA_MAX) VFlags[idx] |= VF_POSX;
                                    XYCache[idx*2+0] = sx;

                                    sy = *(float*)(ptr+8) * fAAScaleY + fAAOffsetY;
                                    if (sy < AA_MIN) VFlags[idx] |= VF_NEGY; else
                                    if (sy > AA_MAX) VFlags[idx] |= VF_POSY;
                                    XYCache[idx*2+1] = sy;

                                    break;
                                }
                                /*
                                 * blind copy
                                 */
                            copy:
                                while (nvFreeCount < size)
                                    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, size);
                                memcpy ((void*)nvFifo,(void*)ptr,skip);
                                nvFifo      += skip;
                                nvFreeCount -= size;
                            }
                            break;
                        }
                        ptr += skip;
                    }

                    /*
                     * filter down
                     */
                    if (nvAATable[pCtx->dwAASuperMode].dwDXPass2)
                    {
                        /*
                         * 2-pass filter
                         */
                        nvAABlt (pCtx->dwSuperOffset,pCtx->dwSuperPitch,nvAATable[pCtx->dwAASuperMode].dwSrcPass1,dwSuperHW,
                                 pCtx->dwSuperOffset,pCtx->dwSuperPitch,0x00000000,dwSuperHW,
                                                                        0x00000000,dwSuperHW,
                                 nvAATable[pCtx->dwAASuperMode].dwDXPass1,nvAATable[pCtx->dwAASuperMode].dwDYPass1,
                                 (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) | (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16));
                        nvAABlt (pCtx->dwSuperOffset,pCtx->dwSuperPitch,nvAATable[pCtx->dwAASuperMode].dwSrcPass2,dwSuperHW,
                                 dwScreenOffset,dwScreenPitch,          dwScreenYX,dwChunkHW,
                                                                        dwClipYX,dwClipHW,
                                 nvAATable[pCtx->dwAASuperMode].dwDXPass2,nvAATable[pCtx->dwAASuperMode].dwDYPass2,
                                 (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) | (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16));
                    }
                    else
                    {
                        /*
                         * 1-pass filter
                         */
                        nvAABlt (pCtx->dwSuperOffset,pCtx->dwSuperPitch,nvAATable[pCtx->dwAASuperMode].dwSrcPass1,dwSuperHW,
                                 dwScreenOffset,dwScreenPitch,          dwScreenYX,dwChunkHW,
                                                                        dwClipYX,dwClipHW,
                                 nvAATable[pCtx->dwAASuperMode].dwDXPass1,nvAATable[pCtx->dwAASuperMode].dwDYPass1,
                                 (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) | (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16));
                    }
                    nvReset2DSurfaceFormat();
                    /* unfiltered blt
                    nvAABlt (pCtx->dwSuperOffset,pCtx->dwSuperPitch,0x00000000,dwSuperHW,
                             dwScreenOffset,dwScreenPitch,          0x00000000,dwSuperHW,
                                                                    0x00000000,dwSuperHW,
                             (1<<20),(1<<20),
                             (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24) | (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16));
                    */

                    /*
                     * kick off
                     */
                    nvStartDmaBuffer (FALSE);

                    /*
                     * next
                     */
                    fAAOffsetX -= chunkW;
                    scrnX      += pCtx->dwChunkWidth;
                }

                fAAOffsetY -= chunkH;
                scrnY      += pCtx->dwChunkHeight;
            }

            /*
             * reset render phase tag
             */
            pCurrentContext->dwAntiAliasFlags &= ~(AA_RENDER | AA_CLEAR_SCREEN | AA_COPY_SCREEN);
            pCtx->dwCapIndex = 0;
        }
        break;
        case AA_MODE_SINGLECHUNK:
        {
            if (pCurrentContext->dwAntiAliasFlags & AA_PENDING_DATA)
            {
                /*
                 * force the blitter to operate properly (test on simply3d)
                 */
                nvReset2DSurfaceFormat();

                /*
                 * scale down super buffer
                 */
                if (nvAATable[pCtx->dwAASuperMode].dwDXPass2)
                {
                    /*
                     * 2-pass filter
                     */
                    nvAABlt (pCtx->dwSuperOffset,pCtx->dwSuperPitch,nvAATable[pCtx->dwAASuperMode].dwSrcPass1,dwSuperHW,
                             pCtx->dwSuperOffset,pCtx->dwSuperPitch,0x00000000,dwSuperHW,
                                                                    0x00000000,dwSuperHW,
                             nvAATable[pCtx->dwAASuperMode].dwDXPass1,nvAATable[pCtx->dwAASuperMode].dwDYPass1,
                             (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) | (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16));
                    nvAABlt (pCtx->dwSuperOffset,pCtx->dwSuperPitch,nvAATable[pCtx->dwAASuperMode].dwSrcPass2,dwSuperHW,
                             dwScreenOffset,dwScreenPitch,          0,dwChunkHW,
                                                                    dwClipYX,dwClipHW,
                             nvAATable[pCtx->dwAASuperMode].dwDXPass2,nvAATable[pCtx->dwAASuperMode].dwDYPass2,
                             (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) | (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16));
                }
                else
                {
                    /*
                     * 1-pass filter
                     */
                    nvAABlt (pCtx->dwSuperOffset,pCtx->dwSuperPitch,nvAATable[pCtx->dwAASuperMode].dwSrcPass1,dwSuperHW,
                             dwScreenOffset,dwScreenPitch,          0,dwChunkHW,
                                                                    dwClipYX,dwClipHW,
                             nvAATable[pCtx->dwAASuperMode].dwDXPass1,nvAATable[pCtx->dwAASuperMode].dwDYPass1,
                             (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) | (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16));
                }
                pCurrentContext->dwAntiAliasFlags &= ~AA_PENDING_DATA;
            }
        }
        break;
        case AA_MODE_EDGE:
        {
            /*
             * early exit if nothing todo
             */
            if (!pCtx->dwCapIndex)
            {
                return TRUE;
            }
            //aatodo - implement
            pCtx->dwCapIndex = 0;
        }
        break;
    }

    /*
     * reset counters for next frame
     */
    pDriverData->dDrawSpareSubchannelObject = 0;
    //pDriverData->TwoDRenderingOccurred      = TRUE;
    pDriverData->ThreeDRenderingOccurred    = 0;
    nvReset2DSurfaceFormat();

    /*
     * success
     */
    return TRUE;
}

/*
 * nvAASingleChunkRenderPrimitive
 *
 * renders triangles into super buffer
 */
void nvAASingleChunkRenderPrimitive
(
    DWORD  nPrimCount,
    LPWORD pIndices,        // NULL for non-indexed cases
    DWORD  dwStrides,
    LPBYTE pVertices
)
{
    PNVAACONTEXT pCtx;
    DWORD        a0,a1,a2;
    DWORD        dwVertexStride;
    float        fAAScaleX;
    float        fAAScaleY;
    DWORD        dwCullMask1;
    DWORD        dwCullMask2;
    DWORD        dwFogMode;
    DWORD       *dwUVOffset;

    DPF_LEVEL (NVDBG_LEVEL_FUNCTION_CALL, "nvAASingleChunkRenderPrimitive");

    /*
     * get context
     */
    pCtx = pCurrentContext->pAAContext;
    pCurrentContext->dwAntiAliasFlags |= AA_PENDING_DATA;

    /*
     * clear render target if needed, always clear z buffer
     */
    if (pCurrentContext->dwAntiAliasFlags & (AA_CLEAR_SCREEN | AA_COPY_SCREEN))
    {
        DWORD dwRectColorFormat;
        DWORD dwSurfColorFormat;

        /*
         * setup formats
         */
        switch (pCtx->dwBPP)
        {
            case 2:
                dwRectColorFormat = NV05E_SET_COLOR_FORMAT_LE_X16R5G6B5;
                dwSurfColorFormat = NV042_SET_COLOR_FORMAT_LE_Y16;
                break;
            case 3:
            case 4:
                dwRectColorFormat = NV05E_SET_COLOR_FORMAT_LE_X8R8G8B8;
                dwSurfColorFormat = NV042_SET_COLOR_FORMAT_LE_Y32;
                break;
        }

        /*
         * do clear
         */
        if  (pCurrentContext->lpLclZ)
        {
            nvSetClearSurface(
                pCtx->dwSuperZOffset,pCtx->dwSuperZPitch,
                dwSurfColorFormat,dwRectColorFormat,pCtx->dwZFill);
            nvClearSurfaceRect(0,0,pCtx->dwSuperWidth,pCtx->dwSuperHeight);
            nvReset2DSurfaceFormat();
        }

        if (pCurrentContext->dwAntiAliasFlags & AA_CLEAR_SCREEN)
        {
            nvSetClearSurface(
                pCtx->dwSuperOffset,pCtx->dwSuperPitch,
                dwSurfColorFormat,dwRectColorFormat,pCtx->dwCFill);
            nvClearSurfaceRect(0,0,pCtx->dwSuperWidth,pCtx->dwSuperHeight);
            nvReset2DSurfaceFormat();
            /*
             * tag as clear (clear takes precedence over copy)
             */
            pCurrentContext->dwAntiAliasFlags &= ~(AA_CLEAR_SCREEN | AA_COPY_SCREEN);
        }
        /*
         * do copy
         */
        else
        {
            DWORD dwScreenOffset;
            DWORD dwScreenPitch;
            DWORD dwScreenHW;
            DWORD dwSuperHW;

            /*
             * setup
             */
            dwScreenOffset = pCurrentContext->dwSurfaceOffset;
            dwScreenPitch  = pCurrentContext->surfacePitch.wColorPitch;
            dwScreenHW     = AACOMBINE(pCurrentContext->lpLcl->lpGbl->wHeight,pCurrentContext->lpLcl->lpGbl->wWidth);
            dwSuperHW      = AACOMBINE(pCtx->dwSuperHeight,pCtx->dwSuperWidth);

            /*
             * copy screen
             */
            nvAABlt (dwScreenOffset,dwScreenPitch,          0x00000000,dwScreenHW,
                     pCtx->dwSuperOffset,pCtx->dwSuperPitch,0x00000000,dwSuperHW,
                                                            0x00000000,dwSuperHW,
                     nvAATable[pCtx->dwAASuperMode].dwDU,nvAATable[pCtx->dwAASuperMode].dwDV,
                     (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24) | (NV077_IMAGE_IN_FORMAT_ORIGIN_CORNER << 16));

            /*
             * tag as copied
             */
            pCurrentContext->dwAntiAliasFlags &= ~AA_COPY_SCREEN;
        }

        /*
         * set render target
         */
        pDriverData->lpLast3DSurfaceRendered = 0;
        nvSetD3DSurfaceState (pCurrentContext);

        /*
         * reset triangle object
         */
        if (pCurrentContext->bUseDX6Class)
        {
            nvSetDx6MultiTextureState (pCurrentContext);
        }
        else
        {
            nvSetDx5TriangleState (pCurrentContext);
        }

        /*
         * let it rip while we send some triangles
         */
        nvStartDmaBuffer (FALSE);
    }

    /*
     * setup
     */
    a0 = 0;
    a1 = 1;
    a2 = 2;
    dwVertexStride = fvfData.dwVertexStride;
    fAAScaleX      = pCtx->fAAScaleX;
    fAAScaleY      = pCtx->fAAScaleY;
    dwCullMask1    = (pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
    dwCullMask2    = (pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_NONE) ? 0xffffffff : 0x80000000;
    dwFogMode      = pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGENABLE]
                   ? pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE]
                   : 0;
    dwUVOffset     = ((PNVD3DMULTITEXTURESTATE)(&pCurrentContext->mtsState))->dwUVOffset;

    /*
     * for all primitives
     */
    while (nPrimCount)
    {
        DWORD v0,v1,v2;
        float cv;

        /*
         * extract vertices
         */
        if (pIndices)
        {
            v0 = (DWORD)pVertices + dwVertexStride * (DWORD)pIndices[a0];
            PREFETCH (v0);
            v1 = (DWORD)pVertices + dwVertexStride * (DWORD)pIndices[a1];
            PREFETCH (v1);
            v2 = (DWORD)pVertices + dwVertexStride * (DWORD)pIndices[a2];
            PREFETCH (v2);
        }
        else
        {
            v0 = (DWORD)pVertices + dwVertexStride * a0;
            PREFETCH (v0);
            v1 = (DWORD)pVertices + dwVertexStride * a1;
            PREFETCH (v1);
            v2 = (DWORD)pVertices + dwVertexStride * a2;
            PREFETCH (v2);
        }

        /*
         * cull check
         */
        IF_NOT_CULLED_BEGIN(v0,v1,v2,cv)
            DWORD o0,o1,o2;
            float x0,x1,x2;
            float y0,y1,y2;
            DWORD s0[11],s1[11],s2[11];

            /*
             * scale vertices & compute outcodes
             */
            o0 = 0;
            x0 = *(float*)(v0 + 0) * fAAScaleX;
            if (x0 < AA_MIN) o0 |= VF_NEGX;
            if (x0 > AA_MAX) o0 |= VF_POSX;
            y0 = *(float*)(v0 + 4) * fAAScaleY;
            if (y0 < AA_MIN) o0 |= VF_NEGY;
            if (y0 > AA_MAX) o0 |= VF_POSY;

            o1 = 0;
            x1 = *(float*)(v1 + 0) * fAAScaleX;
            if (x1 < AA_MIN) o1 |= VF_NEGX;
            if (x1 > AA_MAX) o1 |= VF_POSX;
            y1 = *(float*)(v1 + 4) * fAAScaleY;
            if (y1 < AA_MIN) o1 |= VF_NEGY;
            if (y1 > AA_MAX) o1 |= VF_POSY;

            o2 = 0;
            x2 = *(float*)(v2 + 0) * fAAScaleX;
            if (x2 < AA_MIN) o2 |= VF_NEGX;
            if (x2 > AA_MAX) o2 |= VF_POSX;
            y2 = *(float*)(v2 + 4) * fAAScaleY;
            if (y2 < AA_MIN) o2 |= VF_NEGY;
            if (y2 > AA_MAX) o2 |= VF_POSY;

            /*
             * expand vertices into TL format
             */
            if (pCurrentContext->bUseDX6Class)
            {
                DWORD ptr,fc=0;
                switch (dwFogMode)
                {
                    case 0: ptr = (DWORD)s0;
                            nvglDX6FlexTriangleVertex(ptr,fc,NV_DD_SPARE,0,v0,fvfData,dwUVOffset);
                            ptr = (DWORD)s1;
                            nvglDX6FlexTriangleVertex(ptr,fc,NV_DD_SPARE,1,v1,fvfData,dwUVOffset);
                            ptr = (DWORD)s2;
                            nvglDX6FlexTriangleVertex(ptr,fc,NV_DD_SPARE,2,v2,fvfData,dwUVOffset);
                            break;
                    case 1: ptr = (DWORD)s0;
                            nvglDX6FlexTriangleVertexExpFog(ptr,fc,NV_DD_SPARE,0,v0,fvfData,dwUVOffset);
                            ptr = (DWORD)s1;
                            nvglDX6FlexTriangleVertexExpFog(ptr,fc,NV_DD_SPARE,1,v1,fvfData,dwUVOffset);
                            ptr = (DWORD)s2;
                            nvglDX6FlexTriangleVertexExpFog(ptr,fc,NV_DD_SPARE,2,v2,fvfData,dwUVOffset);
                            break;
                    case 2: ptr = (DWORD)s0;
                            nvglDX6FlexTriangleVertexExp2Fog(ptr,fc,NV_DD_SPARE,0,v0,fvfData,dwUVOffset);
                            ptr = (DWORD)s1;
                            nvglDX6FlexTriangleVertexExp2Fog(ptr,fc,NV_DD_SPARE,1,v1,fvfData,dwUVOffset);
                            ptr = (DWORD)s2;
                            nvglDX6FlexTriangleVertexExp2Fog(ptr,fc,NV_DD_SPARE,2,v2,fvfData,dwUVOffset);
                            break;
                    case 3: ptr = (DWORD)s0;
                            nvglDX6FlexTriangleVertexLinearFog(ptr,fc,NV_DD_SPARE,0,v0,fvfData,dwUVOffset);
                            ptr = (DWORD)s1;
                            nvglDX6FlexTriangleVertexLinearFog(ptr,fc,NV_DD_SPARE,1,v1,fvfData,dwUVOffset);
                            ptr = (DWORD)s2;
                            nvglDX6FlexTriangleVertexLinearFog(ptr,fc,NV_DD_SPARE,2,v2,fvfData,dwUVOffset);
                            break;
                }
            }
            else
            {
                DWORD ptr,fc=0;
                switch (dwFogMode)
                {
                    case 0: ptr = (DWORD)s0;
                            nvglDX5FlexTriangleVertex(ptr,fc,NV_DD_SPARE,0,v0,fvfData,dwUVOffset);
                            ptr = (DWORD)s1;
                            nvglDX5FlexTriangleVertex(ptr,fc,NV_DD_SPARE,1,v1,fvfData,dwUVOffset);
                            ptr = (DWORD)s2;
                            nvglDX5FlexTriangleVertex(ptr,fc,NV_DD_SPARE,2,v2,fvfData,dwUVOffset);
                            break;
                    case 1: ptr = (DWORD)s0;
                            nvglDX5FlexTriangleVertexExpFog(ptr,fc,NV_DD_SPARE,0,v0,fvfData,dwUVOffset);
                            ptr = (DWORD)s1;
                            nvglDX5FlexTriangleVertexExpFog(ptr,fc,NV_DD_SPARE,1,v1,fvfData,dwUVOffset);
                            ptr = (DWORD)s2;
                            nvglDX5FlexTriangleVertexExpFog(ptr,fc,NV_DD_SPARE,2,v2,fvfData,dwUVOffset);
                            break;
                    case 2: ptr = (DWORD)s0;
                            nvglDX5FlexTriangleVertexExp2Fog(ptr,fc,NV_DD_SPARE,0,v0,fvfData,dwUVOffset);
                            ptr = (DWORD)s1;
                            nvglDX5FlexTriangleVertexExp2Fog(ptr,fc,NV_DD_SPARE,1,v1,fvfData,dwUVOffset);
                            ptr = (DWORD)s2;
                            nvglDX5FlexTriangleVertexExp2Fog(ptr,fc,NV_DD_SPARE,2,v2,fvfData,dwUVOffset);
                            break;
                    case 3: ptr = (DWORD)s0;
                            nvglDX5FlexTriangleVertexLinearFog(ptr,fc,NV_DD_SPARE,0,v0,fvfData,dwUVOffset);
                            ptr = (DWORD)s1;
                            nvglDX5FlexTriangleVertexLinearFog(ptr,fc,NV_DD_SPARE,1,v1,fvfData,dwUVOffset);
                            ptr = (DWORD)s2;
                            nvglDX5FlexTriangleVertexLinearFog(ptr,fc,NV_DD_SPARE,2,v2,fvfData,dwUVOffset);
                            break;
                }
            }

            /*
             * clip if needed
             */
            if (o0 | o1 | o2)
            {
                static DWORD VFlags[16] = { VF_RESTORE,VF_RESTORE,VF_RESTORE,VF_RESTORE,
                                            VF_RESTORE,VF_RESTORE,VF_RESTORE,VF_RESTORE,
                                            VF_RESTORE,VF_RESTORE,VF_RESTORE,VF_RESTORE,
                                            VF_RESTORE,VF_RESTORE,VF_RESTORE,VF_RESTORE };
                DWORD  VCache[3] = {(DWORD)&s0[1], (DWORD)&s1[1], (DWORD)&s2[1]};
                float XYCache[6] = {x0,y0,x1,y1,x2,y2};
                DWORD len        = pCurrentContext->bUseDX6Class
                                 ? ((sizeDx6TriangleTLVertex - 1) * 4)
                                 : ((sizeDx5TriangleTLVertex - 1) * 4);

                VFlags[0] = VF_RESTORE | o0;
                VFlags[1] = VF_RESTORE | o1;
                VFlags[2] = VF_RESTORE | o2;

                nvAAClip (2,1,0,len,XYCache,VCache,VFlags);
            }
            else
            {
                /*
                 * assign scaled x & y values
                 */
                *(float*)(&s0[1]) = x0;
                *(float*)(&s0[2]) = y0;
                *(float*)(&s1[1]) = x1;
                *(float*)(&s1[2]) = y1;
                *(float*)(&s2[1]) = x2;
                *(float*)(&s2[2]) = y2;

                /*
                 * send to HW
                 */
                if (pCurrentContext->bUseDX6Class)
                {
                    while (nvFreeCount < (sizeDx6TriangleTLVertex * 3 + sizeDx6TriangleDrawPrimitive))
                        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeDx6TriangleTLVertex * 3 + sizeDx6TriangleDrawPrimitive));
                    memcpy ((DWORD*)(nvFifo +  0),s0,sizeDx6TriangleTLVertex * 4);
                    memcpy ((DWORD*)(nvFifo + 44),s1,sizeDx6TriangleTLVertex * 4);
                    memcpy ((DWORD*)(nvFifo + 88),s2,sizeDx6TriangleTLVertex * 4);
                    *(DWORD*)(nvFifo + 132) = (((sizeDx6TriangleDrawPrimitiveMthdCnt << 2) << 16) | ((NV_DD_SPARE) << 13) | NV055_DRAW_PRIMITIVE((0)));
                    *(DWORD*)(nvFifo + 136) = 0x210;
                    nvFifo      += (sizeDx6TriangleTLVertex * 3 + sizeDx6TriangleDrawPrimitive) * 4;
                    nvFreeCount -=  sizeDx6TriangleTLVertex * 3 + sizeDx6TriangleDrawPrimitive;
                }
                else
                {
                    while (nvFreeCount < (sizeDx5TriangleTLVertex * 3 + sizeDx5TriangleDrawPrimitive))
                        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeDx5TriangleTLVertex * 3 + sizeDx5TriangleDrawPrimitive));
                    memcpy ((DWORD*)(nvFifo +  0),s0,sizeDx5TriangleTLVertex * 4);
                    memcpy ((DWORD*)(nvFifo + 36),s1,sizeDx5TriangleTLVertex * 4);
                    memcpy ((DWORD*)(nvFifo + 72),s2,sizeDx5TriangleTLVertex * 4);
                    *(DWORD*)(nvFifo + 108) = (((sizeDx5TriangleDrawPrimitiveMthdCnt << 2) << 16) | ((NV_DD_SPARE) << 13) | NV054_DRAW_PRIMITIVE((0)));
                    *(DWORD*)(nvFifo + 112) = 0x210;
                    nvFifo      += (sizeDx5TriangleTLVertex * 3 + sizeDx5TriangleDrawPrimitive) * 4;
                    nvFreeCount -=  sizeDx5TriangleTLVertex * 3 + sizeDx5TriangleDrawPrimitive;
                }
            }

        IF_NOT_CULLED_END

        /*
         * next
         */
        a0 += (dwStrides & 0x00ff00) >> 8;
        a1 +=  dwStrides & 0x0000ff;
        a2 +=  dwStrides & 0x0000ff;
        if (dwStrides & 0xff0000) a1 ^= a2 , a2 ^= a1 , a1 ^= a2;
        nPrimCount--;
    }

    /*
     * kick off if needed
     */
    nvStartDmaBuffer (TRUE);

}

#endif  // NV4
