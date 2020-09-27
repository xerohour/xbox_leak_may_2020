// (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.

/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVDMAC.H                                                          *
*   Define various macros used to interact with the hardware                *
*   Please put other more general D3D macros in nvd3dmac or elsewhere!      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       1/26/98 - Created                       *
*                                                                           *
\***************************************************************************/

#ifndef _NV4DMAC_H_
#define _NV4DMAC_H_

#ifdef __cplusplus
extern "C" {
#endif

// NV Reference Manual register access definitions.
#define BIT(b)                  (1<<(b))
#define DEVICE_BASE(d)          (0?d)
#define DEVICE_EXTENT(d)        (1?d)
#define DRF_SHIFT(drf)          ((0?drf) % 32)
#define DRF_MASK(drf)           (0xFFFFFFFF>>(31-((1?drf) % 32)+((0?drf) % 32)))
#define DRF_DEF(d,r,f,c)        ((NV ## d ## r ## f ## c)<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_NUM(d,r,f,n)        (((n)&DRF_MASK(NV ## d ## r ## f))<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_VAL(d,r,f,v)        (((v)>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))
#define REG_WR_DRF_NUM(d,r,f,n) REG_WR32(NV ## d ## r, DRF_NUM(d,r,f,n))
#define REG_WR_DRF_DEF(d,r,f,c) REG_WR32(NV ## d ## r, DRF_DEF(d,r,f,c))
#define FLD_WR_DRF_NUM(d,r,f,n) REG_WR32(NV##d##r,(REG_RD32(NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define FLD_WR_DRF_DEF(d,r,f,c) REG_WR32(NV##d##r,(REG_RD32(NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define REG_RD_DRF(d,r,f)       (((REG_RD32(NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//-------------------------------------------------------------------------
//                 GENERIC DMA PUSH BUFFER OPERATIONS
//-------------------------------------------------------------------------

#define sizeSetObject                       2
#define nvglSetObject(ch, obj)                                              \
{                                                                           \
    nvPushData (0, (0x00040000 | ((ch) << 13)));                            \
    nvPushData (1, obj);                                                    \
    nvPusherAdjust (sizeSetObject);                                         \
}

#define sizeSetStartMethod                  1
#define nvglSetStartMethod(index, ch, method, methodcount)                  \
{                                                                           \
    nvPushData (index, ((((methodcount) << 2) << 16) | ((ch) << 13) | (method))); \
}

#define sizeSetData                         1
#define nvglSetData(index, data)                                            \
{                                                                           \
    nvPushData (index, (data));                                             \
}

#define nvglAdjust(size)                                                    \
{                                                                           \
    nvPusherAdjust (size);                                                  \
}

#define sizeDmaJump         1
#define nvglDmaJump(offset)                                                 \
{                                                                           \
    nvPushData (0, (0x20000000 | (offset)));                                \
    nvPusherAdjust (sizeDmaJump);                                           \
}

//-------------------------------------------------------------------------
//                              NV 002
//                   NV01_CONTEXT_DMA_FROM_MEMORY
//-------------------------------------------------------------------------

#define sizeSetDmaFromMemoryMthdCnt                 3
#define sizeSetDmaFromMemory                        4
#define nvglSetDmaFromMemory(ch, base, selector, limit)                     \
{                                                                           \
    nvPushData (0, (((sizeSetDmaFromMemoryMthdCnt << 2) << 16) | ((ch) << 13) | 0x300)); \
    nvPushData (1, (base));                                                 \
    nvPushData (2, (selector));                                             \
    nvPushData (3, (limit));                                                \
    nvPusherAdjust (sizeSetDmaFromMemory);                                  \
}

#define sizeSetDmaFromMemoryContextNotifyMthdCnt    1
#define sizeSetDmaFromMemoryContextNotify           2
#define nvglSetDmaFromMemoryContextNotify(ch, ctxdmanotify)                 \
{                                                                           \
    nvPushData (0, (((sizeSetDmaFromMemoryContextNotifyMthdCnt << 2) << 16) | ((ch) << 13) | 0x180)); \
    nvPushData (1, (ctxdmanotify));                                         \
    nvPusherAdjust (sizeSetDmaFromMemoryContextNotify);                     \
}

#define sizeSetDmaFromMemoryNotify                  7
#define nvglSetDmaFromMemoryNotify(ch, base, selector, limit, notify)       \
{                                                                           \
    nvPushData (0, (((2 << 2) << 16) | ((ch) << 13) | 0x300));              \
    nvPushData (1, (base));                                                 \
    nvPushData (2, (selector));                                             \
    nvPushData (3, (((1 << 2) << 16) | ((ch) << 13) | 0x104));              \
    nvPushData (4, (notify));                                               \
    nvPushData (5, (((1 << 2) << 16) | ((ch) << 13) | 0x308));              \
    nvPushData (6, (limit));                                                \
    nvPusherAdjust (sizeSetDmaFromMemoryNotify);                            \
}

//-------------------------------------------------------------------------
//                              NV 003
//                    NV01_CONTEXT_DMA_TO_MEMORY
//-------------------------------------------------------------------------

#define sizeSetDmaToMemoryMthdCnt                 3
#define sizeSetDmaToMemory                        4
#define nvglSetDmaToMemory(ch, base, selector, limit)                       \
{                                                                           \
    nvPushData (0, (((sizeSetDmaToMemoryMthdCnt << 2) << 16) | ((ch) << 13) | 0x300)); \
    nvPushData (1, (base));                                                 \
    nvPushData (2, (selector));                                             \
    nvPushData (3, (limit));                                                \
    nvPusherAdjust (sizeSetDmaToMemory);                                    \
}

#define sizeSetDmaToMemoryContextNotifyMthdCnt    1
#define sizeSetDmaToMemoryContextNotify           2
#define nvglSetDmaToMemoryContextNotify(ch, ctxdmanotify)                   \
{                                                                           \
    nvPushData (0, (((sizeSetDmaToMemoryContextNotifyMthdCnt << 2) << 16) | ((ch) << 13) | 0x180)); \
    nvPushData (1, (ctxdmanotify));                                         \
    nvPusherAdjust (sizeSetDmaToMemoryContextNotify);                       \
}

#define sizeSetDmaToMemoryNotify                  7
#define nvglSetDmaToMemoryNotify(ch, base, selector, limit, notify)         \
{                                                                           \
    nvPushData (0, (((2 << 2) << 16) | ((ch) << 13) | 0x300));              \
    nvPushData (1, (base));                                                 \
    nvPushData (2, (selector));                                             \
    nvPushData (3, (((1 << 2) << 16) | ((ch) << 13) | 0x104));              \
    nvPushData (4, (notify));                                               \
    nvPushData (5, (((1 << 2) << 16) | ((ch) << 13) | 0x308));              \
    nvPushData (6, (limit));                                                \
    nvPusherAdjust (sizeSetDmaToMemoryNotify);                              \
}

//-------------------------------------------------------------------------
//                              NV 019
//                    NV01_CONTEXT_CLIP_RECTANGLE
//-------------------------------------------------------------------------

#define sizeSetClipMthdCnt                          2
#define sizeSetClip                                 3
#define nvglSetClip(ch, x, y, w, h)                                         \
{                                                                           \
    nvPushData (0, (((sizeSetClipMthdCnt << 2) << 16) | ((ch) << 13) | NV019_SET_POINT)); \
    nvPushData (1, ((DWORD)y << 16) | (DWORD)x);                            \
    nvPushData (2, ((DWORD)h << 16) | (DWORD)w);                            \
    nvPusherAdjust (sizeSetClip);                                           \
}

//-------------------------------------------------------------------------
//                              NV 042
//                     NV04_CONTEXT_SURFACES_2D
//-------------------------------------------------------------------------

#define sizeSet2DSurfacesNoOpMthdCnt    1
#define sizeSet2DSurfacesNoOp           2
#define nvglSet2DSurfacesNoOp(ch)                                           \
{                                                                           \
    nvPushData (0, (((sizeSet2DSurfacesNoOpMthdCnt << 2) << 16) | ((ch) << 13) | NV042_NO_OPERATION)); \
    nvPushData (1, 0);                                                      \
    nvPusherAdjust (sizeSet2DSurfacesNoOp);                                 \
}

#define sizeSet2DSurfacesContextsMthdCnt            2
#define sizeSet2DSurfacesContexts                   3
#define nvglSet2DSurfacesContexts(ch, ctxsrc, ctxdest)                      \
{                                                                           \
    nvPushData (0, (((sizeSet2DSurfacesContextsMthdCnt << 2) << 16) | ((ch) << 13) | NV042_SET_CONTEXT_DMA_IMAGE_SOURCE)); \
    nvPushData (1, (ctxsrc));                                               \
    nvPushData (2, (ctxdest));                                              \
    nvPusherAdjust (sizeSet2DSurfacesContexts);                             \
}

#define sizeSet2DSurfacesColorFormatMthdCnt         1
#define sizeSet2DSurfacesColorFormat                2
#define nvglSet2DSurfacesColorFormat(ch, format)                            \
{                                                                           \
    if (pDriverData->bltData.dwLastColourFormat != format) {                \
        nvPushData (0, (((sizeSet2DSurfacesColorFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV042_SET_COLOR_FORMAT)); \
        nvPushData (1, (format));                                           \
        nvPusherAdjust (sizeSet2DSurfacesColorFormat);                      \
        pDriverData->bltData.dwLastColourFormat = format;                   \
    }                                                                       \
}

#define sizeSet2DSurfacesPitchMthdCnt               1
#define sizeSet2DSurfacesPitch                      2
#define nvglSet2DSurfacesPitch(ch, srcpitch, dstpitch)                      \
{                                                                           \
DWORD dwCombinedPitch = ((((dstpitch) & 0x0000FFFF) << 16) | ((srcpitch) & 0x0000FFFF));   \
    if (pDriverData->bltData.dwLastCombinedPitch != dwCombinedPitch) {      \
        nvPushData (0, (((sizeSet2DSurfacesPitchMthdCnt << 2) << 16) | ((ch) << 13) | NV042_SET_PITCH)); \
        nvPushData (1, dwCombinedPitch);                                    \
        nvPusherAdjust (sizeSet2DSurfacesPitch);                            \
        pDriverData->bltData.dwLastCombinedPitch = dwCombinedPitch;         \
    }                                                                       \
}

#define sizeSet2DSurfacesDestinationMthdCnt         1
#define sizeSet2DSurfacesDestination                2
#define nvglSet2DSurfacesDestination(ch, offset)                            \
{                                                                           \
    if (pDriverData->bltData.dwLastDstOffset != offset) {                   \
        nvPushData (0, (((sizeSet2DSurfacesDestinationMthdCnt << 2) << 16) | ((ch) << 13) | NV042_SET_OFFSET_DESTIN)); \
        nvPushData (1, (offset));                                           \
        nvPusherAdjust (sizeSet2DSurfacesDestination);                      \
        pDriverData->bltData.dwLastDstOffset = offset;                      \
    }                                                                       \
}

//-------------------------------------------------------------------------
//                              NV 043
//                         NV03_CONTEXT_ROP
//-------------------------------------------------------------------------

#define sizeSetRop5MthdCnt                          1
#define sizeSetRop5                                 2
#define nvglSetRop5(ch, rop5)                                               \
{                                                                           \
    if (pDriverData->bltData.dwLastRop != rop5) {                           \
        nvPushData (0, (((sizeSetRop5MthdCnt << 2) << 16) | ((ch) << 13) | NV043_SET_ROP5)); \
        nvPushData (1, (rop5));                                             \
        nvPusherAdjust (sizeSetRop5);                                       \
        pDriverData->bltData.dwLastRop = rop5;                              \
    }                                                                       \
}

//-------------------------------------------------------------------------
//                              NV 044
//                       NV04_CONTEXT_PATTERN
//-------------------------------------------------------------------------

// 0x00000304
#define sizeSetNv4ContextPatternMonochromeFormatMthdCnt  1
#define sizeSetNv4ContextPatternMonochromeFormat         2
#define nvglSetNv4ContextPatternMonochromeFormat(ch, format)                \
{                                                                           \
    nvPushData (0, (((sizeSetNv4ContextPatternMonochromeFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV044_SET_MONOCHROME_FORMAT)); \
    nvPushData (1, (format));                                               \
    nvPusherAdjust (sizeSetNv4ContextPatternMonochromeFormat);              \
}
// 0x00000308
#define sizeSetNv4ContextPatternMonochromeShapeMthdCnt  1
#define sizeSetNv4ContextPatternMonochromeShape         2
#define nvglSetNv4ContextPatternMonochromeShape(ch, shape)                  \
{                                                                           \
    nvPushData (0, (((sizeSetNv4ContextPatternMonochromeShapeMthdCnt << 2) << 16) | ((ch) << 13) | NV044_SET_MONOCHROME_SHAPE)); \
    nvPushData (1, (shape));                                                \
    nvPusherAdjust (sizeSetNv4ContextPatternMonochromeShape);               \
}
// 0x0000030c
#define sizeSetNv4ContextPatternPatternSelectMthdCnt  1
#define sizeSetNv4ContextPatternPatternSelect         2
#define nvglSetNv4ContextPatternPatternSelect(ch, select)                   \
{                                                                           \
    nvPushData (0, (((sizeSetNv4ContextPatternPatternSelectMthdCnt << 2) << 16) | ((ch) << 13) | NV044_SET_PATTERN_SELECT)); \
    nvPushData (1, (select));                                               \
    nvPusherAdjust (sizeSetNv4ContextPatternPatternSelect);                 \
}
// 0x00000310
#define sizeSetNv4ContextPatternMonochromeColorsMthdCnt  2
#define sizeSetNv4ContextPatternMonochromeColors         3
#define nvglSetNv4ContextPatternMonochromeColors(ch, c0, c1)                \
{                                                                           \
    nvPushData (0, (((sizeSetNv4ContextPatternMonochromeColorsMthdCnt << 2) << 16) | ((ch) << 13) | NV044_SET_MONOCHORME_COLOR0)); \
    nvPushData (1, (c0));                                                   \
    nvPushData (2, (c1));                                                   \
    nvPusherAdjust (sizeSetNv4ContextPatternMonochromeColors);              \
}
// 0x00000318
#define sizeSetNv4ContextPatternMonochromePatternsMthdCnt  2
#define sizeSetNv4ContextPatternMonochromePatterns         3
#define nvglSetNv4ContextPatternMonochromePatterns(ch, pattern0, pattern1)  \
{                                                                           \
    nvPushData (0, (((sizeSetNv4ContextPatternMonochromePatternsMthdCnt << 2) << 16) | ((ch) << 13) | NV044_SET_MONOCHORME_PATTERN0)); \
    nvPushData (1, (pattern0));                                             \
    nvPushData (2, (pattern1));                                             \
    nvPusherAdjust (sizeSetNv4ContextPatternMonochromePatterns);            \
}

//-------------------------------------------------------------------------
//                              NV 04A
//                     NV04_GDI_RECTANGLE_TEXT
//-------------------------------------------------------------------------

//0x00000300
#define sizeDrawRopRectAndTextSetColorFormatMthdCnt 1
#define sizeDrawRopRectAndTextSetColorFormat        2
#define nvglDrawRopRectAndTextSetColorFormat(ch, format)                    \
{                                                                           \
    nvPushData (0, (((sizeDrawRopRectAndTextSetColorFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV04A_SET_COLOR_FORMAT)); \
    nvPushData (1, (format));                                               \
    nvPusherAdjust (sizeDrawRopRectAndTextSetColorFormat);                  \
}

//0x0000018C
#define sizeDrawRopRectAndTextSetROPMthdCnt 1
#define sizeDrawRopRectAndTextSetROP        2
#define nvglDrawRopRectAndTextSetROP(ch, rop)                               \
{                                                                           \
    nvPushData (0, (((sizeDrawRopRectAndTextSetROPMthdCnt << 2) << 16) | ((ch) << 13) | NV04A_SET_CONTEXT_ROP)); \
    nvPushData (1, (rop));                                                  \
    nvPusherAdjust (sizeDrawRopRectAndTextSetROP);                          \
}

//0x00000188
#define sizeDrawPatternRectAndTextSetPatternMthdCnt 1
#define sizeDrawPatternRectAndTextSetPattern        2
#define nvglDrawPatternRectAndTextSetPattern(ch, pattern)                   \
{                                                                           \
    nvPushData (0, (((sizeDrawPatternRectAndTextSetPatternMthdCnt << 2) << 16) | ((ch) << 13) | NV04A_SET_CONTEXT_PATTERN)); \
    nvPushData (1, (pattern));                                              \
    nvPusherAdjust (sizeDrawPatternRectAndTextSetPattern);                  \
}

//0x00000198
#define sizeDrawRopRectAndTextSetSurfaceMthdCnt 1
#define sizeDrawRopRectAndTextSetSurface        2
#define nvglDrawRopRectAndTextSetSurface(ch, surface)                       \
{                                                                           \
    nvPushData (0, (((sizeDrawRopRectAndTextSetSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV04A_SET_CONTEXT_SURFACE)); \
    nvPushData (1, (surface));                                              \
    nvPusherAdjust (sizeDrawRopRectAndTextSetSurface);                      \
}

//0x000002FC
#define sizeDrawRopRectAndTextSetOperatioonMthdCnt 1
#define sizeDrawRopRectAndTextSetOperation         2
#define nvglDrawRopRectAndTextSetOperation(ch, operation)                   \
{                                                                           \
    nvPushData (0, (((sizeDrawRopRectAndTextSetOperatioonMthdCnt << 2) << 16) | ((ch) << 13) | NV04A_SET_OPERATION)); \
    nvPushData (1, (operation));                                            \
    nvPusherAdjust (sizeDrawRopRectAndTextSetOperation);                    \
}

#define sizeDrawRopRectAndTextColor1AMthdCnt        1
#define sizeDrawRopRectAndTextColor1A               2
#define nvglDrawRopRectAndTextColor1A(ch, color)                            \
{                                                                           \
    nvPushData (0, (((sizeDrawRopRectAndTextColor1AMthdCnt << 2) << 16) | ((ch) << 13) | NV04A_COLOR1_A)); \
    nvPushData (1, (color));                                                \
    nvPusherAdjust (sizeDrawRopRectAndTextColor1A);                         \
}

#define sizeDrawRopRectAndTextPointSizeMthdCnt      2
#define sizeDrawRopRectAndTextPointSize             3
#define nvglDrawRopRectAndTextPointSize(ch, x, y, width, height)            \
{                                                                           \
    nvPushData (0, (((sizeDrawRopRectAndTextPointSizeMthdCnt << 2) << 16) | ((ch) << 13) | NV04A_UNCLIPPED_RECTANGLE_POINT(0))); \
    nvPushData (1, ((((x) & 0x0000FFFF) << 16) | ((y) & 0x0000FFFF)));      \
    nvPushData (2, ((((width) & 0x0000FFFF) << 16) | ((height) & 0x0000FFFF))); \
    nvPusherAdjust (sizeDrawRopRectAndTextPointSize);                       \
}

//-------------------------------------------------------------------------
//                              NV 052
//                   NV04_CONTEXT_SURFACE_SWIZZLED
//-------------------------------------------------------------------------

#define sizeSetSwizzledSurfaceContextImageMthdCnt   1
#define sizeSetSwizzledSurfaceContextImage          2
#define nvglSetSwizzledSurfaceContextImage(ch, ctxdmaimage)                 \
{                                                                           \
    nvPushData (0, (((sizeSetSwizzledSurfaceContextImageMthdCnt << 2) << 16) | ((ch) << 13) | NV052_SET_CONTEXT_DMA_IMAGE)); \
    nvPushData (1, (ctxdmaimage));                                          \
    nvPusherAdjust (sizeSetSwizzledSurfaceContextImage);                    \
}

#define sizeSetSwizzledSurfaceMthdCnt        2
#define sizeSetSwizzledSurface               3
#define nvglSetSwizzledSurface(ch, format, offset)                          \
{                                                                           \
    nvPushData (0, (((sizeSetSwizzledSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV052_SET_FORMAT)); \
    nvPushData (1, (format));                                               \
    nvPushData (2, (offset));                                               \
    nvPusherAdjust (sizeSetSwizzledSurface);                                \
}

//-------------------------------------------------------------------------
//                              NV 053
//                     NV04_CONTEXT_SURFACES_3D
//-------------------------------------------------------------------------

#define sizeSetRenderTargetContextsMthdCnt  2
#define sizeSetRenderTargetContexts         3
#define nvglSetRenderTargetContexts(ch, ctxdmacolor, ctxdmazeta)            \
{                                                                           \
    nvPushData (0, (((sizeSetRenderTargetContextsMthdCnt << 2) << 16) | ((ch) << 13) | NV053_SET_CONTEXT_DMA_COLOR)); \
    nvPushData (1, (ctxdmacolor));                                          \
    nvPushData (2, (ctxdmazeta));                                           \
    nvPusherAdjust (sizeSetRenderTargetContexts);                           \
}

#define sizeSetRenderTarget                 8
#define nvglSetRenderTarget(ch, cliphorizontal, clipvertical, format,       \
                            pitch, coloroffset, zetaoffset)                 \
{                                                                           \
    nvPushData (0, (((3 << 2) << 16) | ((ch) << 13) | NV053_SET_CLIP_HORIZONTAL)); \
    nvPushData (1, (cliphorizontal));                                       \
    nvPushData (2, (clipvertical));                                         \
    nvPushData (3, (format));                                               \
    nvPushData (4, (((3 << 2) << 16) | ((ch) << 13) | NV053_SET_PITCH));    \
    nvPushData (5, (pitch));                                                \
    nvPushData (6, (coloroffset));                                          \
    nvPushData (7, (zetaoffset));                                           \
    nvPusherAdjust (sizeSetRenderTarget);                                   \
}

#define sizeSetRenderTarget2MthdCnt         5
#define sizeSetRenderTarget2                6
#define nvglSetRenderTarget2(ch, format, clipsize, pitch,                   \
                             coloroffset, zetaoffset)                       \
{                                                                           \
    nvPushData (0, (((sizeSetRenderTarget2MthdCnt << 2) << 16) | ((ch) << 13) | NV053_SET_FORMAT)); \
    nvPushData (1, (format));                                               \
    nvPushData (2, (clipsize));                                             \
    nvPushData (3, (pitch));                                                \
    nvPushData (4, (coloroffset));                                          \
    nvPushData (5, (zetaoffset));                                           \
    nvPusherAdjust (sizeSetRenderTarget2);                                  \
}

#define sizeSetRenderTargetViewportMthdCnt  2
#define sizeSetRenderTargetViewport         3
#define nvglSetRenderTargetViewport(ch, cliphorizontal, clipvertical)       \
{                                                                           \
    nvPushData (0, (((sizeSetRenderTargetViewportMthdCnt << 2) << 16) | ((ch) << 13) | NV053_SET_CLIP_HORIZONTAL)); \
    nvPushData (1, (cliphorizontal));                                       \
    nvPushData (2, (clipvertical));                                         \
    nvPusherAdjust (sizeSetRenderTargetViewport);                           \
}

//-------------------------------------------------------------------------
//                              NV 054
//                     NV04_DX5_TEXTURED_TRIANGLE
//-------------------------------------------------------------------------

#define sizeSetDx5TriangleContextsMthdCnt   2
#define sizeSetDx5TriangleContexts          3
#define nvglSetDx5TriangleContexts(ch, ctxa, ctxb)                          \
{                                                                           \
    nvPushData (0, (((sizeSetDx5TriangleContextsMthdCnt << 2) << 16) | ((ch) << 13) | NV054_SET_CONTEXT_DMA_A)); \
    nvPushData (1, (ctxa));                                                 \
    nvPushData (2, (ctxb));                                                 \
    nvPusherAdjust (sizeSetDx5TriangleContexts);                            \
}

#define sizeSetDx5TriangleSurfaceMthdCnt    1
#define sizeSetDx5TriangleSurface           2
#define nvglSetDx5TriangleSurface(ch, ctxs)                                 \
{                                                                           \
    nvPushData (0, (((sizeSetDx5TriangleSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV054_SET_CONTEXT_SURFACES)); \
    nvPushData (1, (ctxs));                                                 \
    nvPusherAdjust (sizeSetDx5TriangleSurface);                             \
}

#define sizeDx5TriangleStateMthdCnt         7
#define sizeDx5TriangleState                8
#define nvglDx5TriangleState(ch, ckey, offset, format, filter,              \
                             blend, control, fogcolor)                      \
{                                                                           \
    nvPushData (0, (((sizeDx5TriangleStateMthdCnt << 2) << 16) | ((ch) << 13) | NV054_COLOR_KEY)); \
    nvPushData (1, (ckey));                                                 \
    nvPushData (2, (offset));                                               \
    nvPushData (3, (format));                                               \
    nvPushData (4, (filter));                                               \
    nvPushData (5, (blend));                                                \
    nvPushData (6, (control));                                              \
    nvPushData (7, (fogcolor));                                             \
    nvPusherAdjust (sizeDx5TriangleState);                                  \
}

#define sizeDx5TriangleDrawPrimitiveMthdCnt 1
#define sizeDx5TriangleDrawPrimitive        2
#define nvglDx5TriangleDrawPrimitive(ch, alias, vertices)                   \
{                                                                           \
    nvPushData (0, (((sizeDx5TriangleDrawPrimitiveMthdCnt << 2) << 16) | ((ch) << 13) | NV054_DRAW_PRIMITIVE((alias)))); \
    nvPushData (1, (vertices));                                             \
    nvPusherAdjust (sizeDx5TriangleDrawPrimitive);                          \
}

#define sizeDx5TriangleNotify               4
#define nvglDx5TriangleNotify(ch)                                           \
{                                                                           \
    nvPushData (0, (((1 << 2) << 16) | ((ch) << 13) | NV054_NOTIFY));       \
    nvPushData (1, NV054_NOTIFY_WRITE_ONLY);                                \
    nvPushData (2, (((1 << 2) << 16) | ((ch) << 13) | NV054_NO_OPERATION)); \
    nvPushData (3, 0x00000000);                                             \
    nvPusherAdjust (sizeDx5TriangleNotify);                                 \
}

//-------------------------------------------------------------------------
//                              NV 055
//                   NV04_DX6_MULTI_TEXTURE_TRIANGLE
//-------------------------------------------------------------------------

#define sizeSetDx6TriangleContextsMthdCnt   2
#define sizeSetDx6TriangleContexts          3
#define nvglSetDx6TriangleContexts(ch, ctxa, ctxb)                          \
{                                                                           \
    nvPushData (0, (((sizeSetDx6TriangleContextsMthdCnt << 2) << 16) | ((ch) << 13) | NV055_SET_CONTEXT_DMA_A)); \
    nvPushData (1, (ctxa));                                                 \
    nvPushData (2, (ctxb));                                                 \
    nvPusherAdjust (sizeSetDx6TriangleContexts);                            \
}

#define sizeSetDx6TriangleSurfaceMthdCnt    1
#define sizeSetDx6TriangleSurface           2
#define nvglSetDx6TriangleSurface(ch, ctxs)                                 \
{                                                                           \
    nvPushData (0, (((sizeSetDx6TriangleSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV055_SET_CONTEXT_SURFACES)); \
    nvPushData (1, (ctxs));                                                 \
    nvPusherAdjust (sizeSetDx6TriangleSurface);                             \
}

#define sizeDx6TriangleOffsetMthdCnt        1
#define sizeDx6TriangleOffset               2
#define nvglDx6TriangleOffset(ch, offset, buffer)                           \
{                                                                           \
    nvPushData (0, (((sizeDx6TriangleOffsetMthdCnt << 2) << 16) | ((ch) << 13) | NV055_OFFSET(buffer))); \
    nvPushData (1, (offset));                                               \
    nvPusherAdjust (sizeDx6TriangleOffset);                                 \
}

#define sizeDx6TriangleFormatMthdCnt        1
#define sizeDx6TriangleFormat               2
#define nvglDx6TriangleFormat(ch, format, buffer)                           \
{                                                                           \
    nvPushData (0, (((sizeDx6TriangleFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV055_FORMAT(buffer))); \
    nvPushData (1, (format));                                               \
    nvPusherAdjust (sizeDx6TriangleFormat);                                 \
}

#define sizeDx6TriangleFilterMthdCnt        1
#define sizeDx6TriangleFilter               2
#define nvglDx6TriangleFilter(ch, filter, buffer)                           \
{                                                                           \
    nvPushData (0, (((sizeDx6TriangleFilterMthdCnt << 2) << 16) | ((ch) << 13) | NV055_FILTER(buffer))); \
    nvPushData (1, (filter));                                               \
    nvPusherAdjust (sizeDx6TriangleFilter);                                 \
}

#define sizeDx6TriangleStageState           12
#define nvglDx6TriangleStageState(ch, c0alpha, c0color, c1alpha, c1color,   \
                                  factor, blend,                            \
                                  control0, control1, control2,             \
                                  fogcolor)                                 \
{                                                                           \
    nvPushData (0, (((2 << 2) << 16) | ((ch) << 13) | NV055_COMBINE_0_ALPHA)); \
    nvPushData (1, (c0alpha));                                              \
    nvPushData (2, (c0color));                                              \
    nvPushData (3, (((8 << 2) << 16) | ((ch) << 13) | NV055_COMBINE_1_ALPHA)); \
    nvPushData (4, (c1alpha));                                              \
    nvPushData (5, (c1color));                                              \
    nvPushData (6, (factor));                                               \
    nvPushData (7, (blend));                                                \
    nvPushData (8, (control0));                                             \
    nvPushData (9, (control1));                                             \
    nvPushData (10, (control2));                                            \
    nvPushData (11, (fogcolor));                                            \
    nvPusherAdjust (sizeDx6TriangleStageState);                             \
}

#define sizeDX6TriangleState    (((sizeDx6TriangleOffset + sizeDx6TriangleFormat + sizeDx6TriangleFilter) * 2) + sizeDx6TriangleStageState)

#define sizeDx6TriangleTLVertexMthdCnt      10
#define sizeDx6TriangleTLVertex1            11
#define nvglDx6TriangleTLVertex(ch, alias, vd)                              \
{                                                                           \
    nvPushData (0, (((sizeDx6TriangleTLVertexMthdCnt << 2) << 16) | ((ch) << 13) | NV055_TLMTVERTEX((alias)))); \
    nvPushData (1, *(U032 *)&(vd.sx));                                      \
    nvPushData (2, *(U032 *)&(vd.sy));                                      \
    nvPushData (3, *(U032 *)&(vd.sz));                                      \
    nvPushData (4, *(U032 *)&(vd.rhw));                                     \
    nvPushData (5, (vd.color));                                             \
    nvPushData (6, (vd.specular));                                          \
    nvPushData (7, *(U032 *)&(vd.tu));                                      \
    nvPushData (8, *(U032 *)&(vd.tv));                                      \
    nvPushData (9, 0);                                                      \
    nvPushData (10, 0);                                                     \
    nvPusherAdjust (sizeDx6TriangleTLVertex1);                              \
    dbgDisplayVertexData(FALSE, (DWORD *)&(vd.sx));                         \
}

#define sizeDx6TriangleDrawPrimitiveMthdCnt 1
#define sizeDx6TriangleDrawPrimitive        2
#define nvglDx6TriangleDrawPrimitive(ch, alias, vertices)                   \
{                                                                           \
    nvPushData (0, (((sizeDx6TriangleDrawPrimitiveMthdCnt << 2) << 16) | ((ch) << 13) | NV055_DRAW_PRIMITIVE((alias)))); \
    nvPushData (1, (vertices));                                             \
    nvPusherAdjust (sizeDx6TriangleDrawPrimitive);                          \
}

//-------------------------------------------------------------------------
//                              NV 056
//                      NV10_CELSIUS_PRIMITIVE
//-------------------------------------------------------------------------
#if (NVARCH >= 0x010)
#define NV056_SET_VERTEX_FORMAT_SHADOW      NV056_SET_INVERSE_MODEL_VIEW_MATRIX1(15)

// 0x00000100
#define sizeSetNv10CelsiusNoOpMthdCnt    1
#define sizeSetNv10CelsiusNoOp           2
#define nvglSetNv10CelsiusNoOp(ch)                                          \
{                                                                           \
    nvPushData (0, (((sizeSetNv10CelsiusNoOpMthdCnt << 2) << 16) | ((ch) << 13) | NV056_NO_OPERATION)); \
    nvPushData (1, 0);                                                      \
    nvPusherAdjust (sizeSetNv10CelsiusNoOp);                                \
}
// 0x00000104
#define sizeSetNv10CelsiusNotifyMthdCnt    1
#define sizeSetNv10CelsiusNotify           2
#define nvglSetNv10CelsiusNotify(ch, type)                                  \
{                                                                           \
    nvPushData (0, (((sizeSetNv10CelsiusNotifyMthdCnt << 2) << 16) | ((ch) << 13) | NV056_NOTIFY)); \
    nvPushData (1, (type));                                                 \
    nvPusherAdjust (sizeSetNv10CelsiusNotify);                              \
}
// 0x00000180
#define sizeSetNv10CelsiusContextDMAsMthdCnt    7
#define sizeSetNv10CelsiusContextDMAs           8
#define nvglSetNv10CelsiusContextDMAs(ch, notifies, dmaa, dmab, vertex, state, color, zeta) \
{                                                                           \
    nvPushData (0, (((sizeSetNv10CelsiusContextDMAsMthdCnt << 2) << 16) | ((ch) << 13) | NV056_SET_CONTEXT_DMA_NOTIFIES)); \
    nvPushData (1, (notifies));                                             \
    nvPushData (2, (dmaa));                                                 \
    nvPushData (3, (dmab));                                                 \
    nvPushData (4, (vertex));                                               \
    nvPushData (5, (state));                                                \
    nvPushData (6, (color));                                                \
    nvPushData (7, (zeta));                                                 \
    nvPusherAdjust (sizeSetNv10CelsiusContextDMAs);                         \
}
// 0x00000180
#define sizeSetNv10CelsiusNotifierContextDMAMthdCnt    1
#define sizeSetNv10CelsiusNotifierContextDMA           2
#define nvglSetNv10CelsiusNotifierContextDMA(ch, notifies)                  \
{                                                                           \
    nvPushData (0, (((sizeSetNv10CelsiusNotifierContextDMAMthdCnt << 2) << 16) | ((ch) << 13) | NV056_SET_CONTEXT_DMA_NOTIFIES)); \
    nvPushData (1, (notifies));                                             \
    nvPusherAdjust (sizeSetNv10CelsiusNotifierContextDMA);                  \
}
// 0x00000184
#define sizeSetNv10CelsiusTextureAndVertexContextDMAsMthdCnt    3
#define sizeSetNv10CelsiusTextureAndVertexContextDMAs           4
#define nvglSetNv10CelsiusTextureAndVertexContextDMAs(ch, dmaa, dmab, vertex) \
{                                                                           \
    nvPushData (0, (((sizeSetNv10CelsiusTextureAndVertexContextDMAsMthdCnt << 2) << 16) | ((ch) << 13) | NV056_SET_CONTEXT_DMA_A)); \
    nvPushData (1, (dmaa));                                                 \
    nvPushData (2, (dmab));                                                 \
    nvPushData (3, (vertex));                                               \
    nvPusherAdjust (sizeSetNv10CelsiusTextureAndVertexContextDMAs);         \
}
// 0x0000072c
#define sizeSetNv10CelsiusEyeDirectionMthdCnt                   3
#define sizeSetNv10CelsiusEyeDirection                          4
#define nvglSetNv10CelsiusEyeDirection(ch, x, y, z)                         \
{                                                                           \
    float x1 = x, y1 = y, z1 = z;                                           \
    nvPushData (0, (((sizeSetNv10CelsiusEyeDirectionMthdCnt << 2) << 16) | ((ch) << 13) | NV056_SET_EYE_DIRECTION_SW(0))); \
    nvPushData (1, *(DWORD*)&x1);                                           \
    nvPushData (2, *(DWORD*)&y1);                                           \
    nvPushData (3, *(DWORD*)&z1);                                           \
    nvPusherAdjust (sizeSetNv10CelsiusEyeDirection);                        \
}
// 0x00000cf0
#define sizeSetNv10CelsiusInvalidateVertexCacheMthdCnt    1
#define sizeSetNv10CelsiusInvalidateVertexCache           2
#define nvglSetNv10CelsiusInvalidateVertexCache(ch)                         \
{                                                                           \
    nvPushData (0, (((sizeSetNv10CelsiusInvalidateVertexCacheMthdCnt << 2) << 16) | ((ch) << 13) | NV056_INVALIDATE_VERTEX_CACHE_FILE)); \
    nvPushData (1, 0);                                                      \
    nvPusherAdjust (sizeSetNv10CelsiusInvalidateVertexCache);               \
}
// 0x000010fc
#define sizeSetNv10CelsiusBeginEnd2MthdCnt    1
#define sizeSetNv10CelsiusBeginEnd2           2
#define nvglSetNv10CelsiusBeginEnd2(ch, op)                                  \
{                                                                           \
    nvPushData (0, (((sizeSetNv10CelsiusBeginEnd2MthdCnt << 2) << 16) | ((ch) << 13) | NV056_SET_BEGIN_END2)); \
    nvPushData (1, (op));                                                   \
    nvPusherAdjust (sizeSetNv10CelsiusBeginEnd2);                            \
}
// 0x000013fc
#define sizeSetNv10CelsiusBeginEnd3MthdCnt    1
#define sizeSetNv10CelsiusBeginEnd3           2
#define nvglSetNv10CelsiusBeginEnd3(ch, op)                                 \
{                                                                           \
    nvPushData (0, (((sizeSetNv10CelsiusBeginEnd3MthdCnt << 2) << 16) | ((ch) << 13) | NV056_SET_BEGIN_END3)); \
    nvPushData (1, (op));                                                   \
    nvPusherAdjust (sizeSetNv10CelsiusBeginEnd3);                           \
}
// 0x000017fc
#define sizeSetNv10CelsiusBeginEnd4MthdCnt    1
#define sizeSetNv10CelsiusBeginEnd4           2
#define nvglSetNv10CelsiusBeginEnd4(ch, op)                                 \
{                                                                           \
    nvPushData (0, (((sizeSetNv10CelsiusBeginEnd4MthdCnt << 2) << 16) | ((ch) << 13) | NV056_SET_BEGIN_END4)); \
    nvPushData (1, (op));                                                   \
    nvPusherAdjust (sizeSetNv10CelsiusBeginEnd4);                           \
}
// 0x00001400
#define sizeSetNv10CelsiusDrawArraysMthdCnt    1
#define sizeSetNv10CelsiusDrawArrays           2
#define nvglSetNv10CelsiusDrawArrays(ch, data)                              \
{                                                                           \
    nvPushData (0, (((sizeSetNv10CelsiusDrawArraysMthdCnt << 2) << 16) | ((ch) << 13) | NV056_DRAW_ARRAYS(0))); \
    nvPushData (1, (data));                                                 \
    nvPusherAdjust (sizeSetNv10CelsiusDrawArrays);                          \
}
#endif  // NVARCH >= 0x010

//-------------------------------------------------------------------------
//                              NV 097
//                       NV20_KELVIN_PRIMITIVE
//-------------------------------------------------------------------------
#if (NVARCH >= 0x020)
// 0x00000100
#define sizeSetNv20KelvinNoOpMthdCnt    1
#define sizeSetNv20KelvinNoOp           2
#define nvglSetNv20KelvinNoOp(ch)                                           \
{                                                                           \
    nvPushData (0, (((sizeSetNv20KelvinNoOpMthdCnt << 2) << 16) | ((ch) << 13) | NV097_NO_OPERATION)); \
    nvPushData (1, 0);                                                      \
    nvPusherAdjust (sizeSetNv20KelvinNoOp);                                 \
}
// 0x00000104
#define sizeSetNv20KelvinNotifyMthdCnt    1
#define sizeSetNv20KelvinNotify           2
#define nvglSetNv20KelvinNotify(ch, type)                                   \
{                                                                           \
    nvPushData (0, (((sizeSetNv20KelvinNotifyMthdCnt << 2) << 16) | ((ch) << 13) | NV097_NOTIFY)); \
    nvPushData (1, (type));                                                 \
    nvPusherAdjust (sizeSetNv20KelvinNotify);                               \
}
// 0x00000180
#define sizeSetNv20KelvinNotifierContextDMAMthdCnt    1
#define sizeSetNv20KelvinNotifierContextDMA           2
#define nvglSetNv20KelvinNotifierContextDMA(ch, notifies)                   \
{                                                                           \
    nvPushData (0, (((sizeSetNv20KelvinNotifierContextDMAMthdCnt << 2) << 16) | ((ch) << 13) | NV097_SET_CONTEXT_DMA_NOTIFIES)); \
    nvPushData (1, (notifies));                                             \
    nvPusherAdjust (sizeSetNv20KelvinNotifierContextDMA);                   \
}
// 0x00000180
#define sizeSetNv20KelvinContextDMAsMthdCnt1    3
#define sizeSetNv20KelvinContextDMAs1           4
#define sizeSetNv20KelvinContextDMAsMthdCnt2    7
#define sizeSetNv20KelvinContextDMAs2           8
#define nvglSetNv20KelvinContextDMAs(ch, notifies, dmaa, dmab, state, color, zeta, vertexa, vertexb, semaphore, report) \
{                                                                           \
    nvPushData (0, (((sizeSetNv20KelvinContextDMAsMthdCnt1 << 2) << 16) | ((ch) << 13) | NV097_SET_CONTEXT_DMA_NOTIFIES)); \
    nvPushData (1, (notifies));                                             \
    nvPushData (2, (dmaa));                                                 \
    nvPushData (3, (dmab));                                                 \
    nvPusherAdjust (sizeSetNv20KelvinContextDMAs1);                         \
    nvPushData (0, (((sizeSetNv20KelvinContextDMAsMthdCnt2 << 2) << 16) | ((ch) << 13) | NV097_SET_CONTEXT_DMA_STATE)); \
    nvPushData (1, (state));                                                \
    nvPushData (2, (color));                                                \
    nvPushData (3, (zeta));                                                 \
    nvPushData (4, (vertexa));                                              \
    nvPushData (5, (vertexb));                                              \
    nvPushData (6, (semaphore));                                            \
    nvPushData (7, (report));                                               \
    nvPusherAdjust (sizeSetNv20KelvinContextDMAs2);                         \
}
// 0x00000184
#define sizeSetNv20KelvinTextureContextDMAsMthdCnt    2
#define sizeSetNv20KelvinTextureContextDMAs           3
#define nvglSetNv20KelvinTextureContextDMAs(ch, dmaa, dmab)                 \
{                                                                           \
    nvPushData (0, (((sizeSetNv20KelvinTextureContextDMAsMthdCnt << 2) << 16) | ((ch) << 13) | NV097_SET_CONTEXT_DMA_A)); \
    nvPushData (1, (dmaa));                                                 \
    nvPushData (2, (dmab));                                                 \
    nvPusherAdjust (sizeSetNv20KelvinTextureContextDMAs);                   \
}
// 0x0000019c
#define sizeSetNv20KelvinVertexContextDMAsMthdCnt    2
#define sizeSetNv20KelvinVertexContextDMAs           3
#define nvglSetNv20KelvinVertexContextDMAs(ch, vertexa, vertexb)            \
{                                                                           \
    nvPushData (0, (((sizeSetNv20KelvinVertexContextDMAsMthdCnt << 2) << 16) | ((ch) << 13) | NV097_SET_CONTEXT_DMA_VERTEX_A)); \
    nvPushData (1, (vertexa));                                              \
    nvPushData (2, (vertexb));                                              \
    nvPusherAdjust (sizeSetNv20KelvinVertexContextDMAs);                    \
}
// 0x00001710
#define sizeSetNv20KelvinInvalidateVertexCacheMthdCnt    1
#define sizeSetNv20KelvinInvalidateVertexCache           2
#define nvglSetNv20KelvinInvalidateVertexCache(ch)                          \
{                                                                           \
    nvPushData (0, (((sizeSetNv20KelvinInvalidateVertexCacheMthdCnt << 2) << 16) | ((ch) << 13) | NV097_INVALIDATE_VERTEX_CACHE_FILE)); \
    nvPushData (1, 0);                                                      \
    nvPusherAdjust (sizeSetNv20KelvinInvalidateVertexCache);                \
}
// 0x000017fc
#define sizeSetNv20KelvinBeginEndMthdCnt    1
#define sizeSetNv20KelvinBeginEnd           2
#define nvglSetNv20KelvinBeginEnd(ch, op)                                   \
{                                                                           \
    nvPushData (0, (((sizeSetNv20KelvinBeginEndMthdCnt << 2) << 16) | ((ch) << 13) | NV097_SET_BEGIN_END)); \
    nvPushData (1, (op));                                                   \
    nvPusherAdjust (sizeSetNv20KelvinBeginEnd);                             \
}
// 0x00001810
#define sizeSetNv20KelvinDrawArraysMthdCnt    1
#define sizeSetNv20KelvinDrawArrays           2
#define nvglSetNv20KelvinDrawArrays(ch, data)                               \
{                                                                           \
    nvPushData (0, (((sizeSetNv20KelvinDrawArraysMthdCnt << 2) << 16) | ((ch) << 13) | NONINC_METHOD(NV097_DRAW_ARRAYS))); \
    nvPushData (1, (data));                                                 \
    nvPusherAdjust (sizeSetNv20KelvinDrawArrays);                           \
}

#endif // (NVARCH >= 0x020)

//-------------------------------------------------------------------------
//                              NV 05E
//                    NV04_RENDER_SOLID_RECTANGLE
//-------------------------------------------------------------------------

// 0x00000100
#define sizeSetNv4SolidRectangleNoOpMthdCnt    1
#define sizeSetNv4SolidRectangleNoOp           2
#define nvglSetNv4SolidRectangleNoOp(ch)                                    \
{                                                                           \
    nvPushData (0, (((sizeSetNv4SolidRectangleNoOpMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_NO_OPERATION)); \
    nvPushData (1, 0);                                                      \
    nvPusherAdjust (sizeSetNv4SolidRectangleNoOp);                          \
}
// 0x00000184
#define sizeSetNv4SolidRectangleClipRectangleMthdCnt  1
#define sizeSetNv4SolidRectangleClipRectangle         2
#define nvglSetNv4SolidRectangleClipRectangle(ch, rect)                     \
{                                                                           \
    nvPushData (0, (((sizeSetNv4SolidRectangleClipRectangleMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_SET_CONTEXT_CLIP_RECTANGLE)); \
    nvPushData (1, (rect));                                                 \
    nvPusherAdjust (sizeSetNv4SolidRectangleClipRectangle);                 \
}
// 0x00000188
#define sizeSetNv4SolidRectanglePatternMthdCnt  1
#define sizeSetNv4SolidRectanglePattern         2
#define nvglSetNv4SolidRectanglePattern(ch, pattern)                        \
{                                                                           \
    nvPushData (0, (((sizeSetNv4SolidRectanglePatternMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_SET_CONTEXT_PATTERN)); \
    nvPushData (1, (pattern));                                              \
    nvPusherAdjust (sizeSetNv4SolidRectanglePattern);                       \
}
// 0x0000018c
#define sizeSetNv4SolidRectangleRopMthdCnt  1
#define sizeSetNv4SolidRectangleRop         2
#define nvglSetNv4SolidRectangleRop(ch, rop)                                \
{                                                                           \
    nvPushData (0, (((sizeSetNv4SolidRectangleRopMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_SET_CONTEXT_ROP)); \
    nvPushData (1, (rop));                                                  \
    nvPusherAdjust (sizeSetNv4SolidRectangleRop);                           \
}
// 0x00000198
#define sizeSetNv4SolidRectangleSurfaceMthdCnt  1
#define sizeSetNv4SolidRectangleSurface         2
#define nvglSetNv4SolidRectangleSurface(ch, surface)                        \
{                                                                           \
    nvPushData (0, (((sizeSetNv4SolidRectangleSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_SET_CONTEXT_SURFACE)); \
    nvPushData (1, (surface));                                              \
    nvPusherAdjust (sizeSetNv4SolidRectangleSurface);                       \
}
// 0x000002fc
#define sizeSetNv4SolidRectangleOperationMthdCnt    1
#define sizeSetNv4SolidRectangleOperation           2
#define nvglSetNv4SolidRectangleOperation(ch, op)                           \
{                                                                           \
    nvPushData (0, (((sizeSetNv4SolidRectangleOperationMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_SET_OPERATION)); \
    nvPushData (1, (op));                                                   \
    nvPusherAdjust (sizeSetNv4SolidRectangleOperation);                     \
}
// 0x00000300
#define sizeSetNv4SolidRectangleColorFormatMthdCnt  1
#define sizeSetNv4SolidRectangleColorFormat         2
#define nvglSetNv4SolidRectangleColorFormat(ch, cfmt)                       \
{                                                                           \
    nvPushData (0, (((sizeSetNv4SolidRectangleColorFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_SET_COLOR_FORMAT)); \
    nvPushData (1, (cfmt));                                                 \
    nvPusherAdjust (sizeSetNv4SolidRectangleColorFormat);                   \
}
// 0x00000304
#define sizeNv4SolidRectangleColorMthdCnt           1
#define sizeNv4SolidRectangleColor                  2
#define nvglNv4SolidRectangleColor(ch, color)                               \
{                                                                           \
    nvPushData (0, (((sizeNv4SolidRectangleColorMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_COLOR)); \
    nvPushData (1, (color));                                                \
    nvPusherAdjust (sizeNv4SolidRectangleColor);                            \
}
// 0x00000400
#define sizeNv4SolidRectanglePointSizeMthdCnt       2
#define sizeNv4SolidRectanglePointSize              3
#define nvglNv4SolidRectanglePointSize(ch, x, y, width, height)             \
{                                                                           \
    nvPushData (0, (((sizeNv4SolidRectanglePointSizeMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_RECTANGLE(0))); \
    nvPushData (1, ((((y) & 0x0000FFFF) << 16) | ((x) & 0x0000FFFF)));      \
    nvPushData (2, ((((height) & 0x0000FFFF) << 16) | ((width) & 0x0000FFFF))); \
    nvPusherAdjust (sizeNv4SolidRectanglePointSize);                        \
}

//-------------------------------------------------------------------------
//                              NV 05F
//                         NV04_IMAGE_BLIT
//-------------------------------------------------------------------------

// 0x00000188
#define sizeSetNv4ImageBlitClipRectangleMthdCnt     1
#define sizeSetNv4ImageBlitClipRectangle            2
#define nvglSetNv4ImageBlitClipRectangle(ch, rect)                          \
{                                                                           \
    nvPushData (0, (((sizeSetNv4ImageBlitClipRectangleMthdCnt << 2) << 16) | ((ch) << 13) | NV05F_SET_CONTEXT_CLIP_RECTANGLE)); \
    nvPushData (1, (rect));                                                 \
    nvPusherAdjust (sizeSetNv4ImageBlitClipRectangle);                      \
}
// 0x0000018c
#define sizeSetNv4ImageBlitContextPatternMthdCnt     1
#define sizeSetNv4ImageBlitContextPattern            2
#define nvglSetNv4ImageBlitContextPattern(ch, pattern)                      \
{                                                                           \
    nvPushData (0, (((sizeSetNv4ImageBlitContextPatternMthdCnt << 2) << 16) | ((ch) << 13) | NV05F_SET_CONTEXT_PATTERN)); \
    nvPushData (1, (pattern));                                              \
    nvPusherAdjust (sizeSetNv4ImageBlitContextPattern);                     \
}
// 0x00000190
#define sizeSetNv4ImageBlitContextRopMthdCnt     1
#define sizeSetNv4ImageBlitContextRop            2
#define nvglSetNv4ImageBlitContextRop(ch, rop)                              \
{                                                                           \
    nvPushData (0, (((sizeSetNv4ImageBlitContextRopMthdCnt << 2) << 16) | ((ch) << 13) | NV05F_SET_CONTEXT_ROP)); \
    nvPushData (1, (rop));                                                  \
    nvPusherAdjust (sizeSetNv4ImageBlitContextRop);                         \
}
// 0x0000019c
#define sizeSetNv4ImageBlitContextSurfacesMthdCnt     1
#define sizeSetNv4ImageBlitContextSurfaces            2
#define nvglSetNv4ImageBlitContextSurfaces(ch, surface)                     \
{                                                                           \
    nvPushData (0, (((sizeSetNv4ImageBlitContextSurfacesMthdCnt << 2) << 16) | ((ch) << 13) | NV05F_SET_CONTEXT_SURFACES)); \
    nvPushData (1, (surface));                                              \
    nvPusherAdjust (sizeSetNv4ImageBlitContextSurfaces);                    \
}
// 0x000002fc
#define sizeSetNv4ImageBlitOperationMthdCnt     1
#define sizeSetNv4ImageBlitOperation            2
#define nvglSetNv4ImageBlitOperation(ch, op)                                \
{                                                                           \
    nvPushData (0, (((sizeSetNv4ImageBlitOperationMthdCnt << 2) << 16) | ((ch) << 13) | NV05F_SET_OPERATION)); \
    nvPushData (1, (op));                                                   \
    nvPusherAdjust (sizeSetNv4ImageBlitOperation);                          \
}
// 0x00000300
#define sizeSetNv4ImageBlitControlPointsMthdCnt     2
#define sizeSetNv4ImageBlitControlPoints            3
#define nvglSetNv4ImageBlitControlPoints(ch, xin, yin, xout, yout)          \
{                                                                           \
    nvPushData (0, (((sizeSetNv4ImageBlitControlPointsMthdCnt << 2) << 16) | ((ch) << 13) | NV05F_CONTROL_POINT_IN)); \
    nvPushData (1, (((xin)  & 0xffff) | ((yin)  << 16)));                   \
    nvPushData (2, (((xout) & 0xffff) | ((yout) << 16)));                   \
    nvPusherAdjust (sizeSetNv4ImageBlitControlPoints);                      \
}
// 0x00000308
#define sizeSetNv4ImageBlitSizeMthdCnt     1
#define sizeSetNv4ImageBlitSize            2
#define nvglSetNv4ImageBlitSize(ch, width, height)                          \
{                                                                           \
    nvPushData (0, (((sizeSetNv4ImageBlitSizeMthdCnt << 2) << 16) | ((ch) << 13) | NV05F_SIZE)); \
    nvPushData (1, (((width)  & 0xffff) | ((height)  << 16)));              \
    nvPusherAdjust (sizeSetNv4ImageBlitSize);                               \
}

//-------------------------------------------------------------------------
//                              NV 060
//                    NV04_INDEXED_IMAGE_FROM_CPU
//-------------------------------------------------------------------------

#define sizeSetIndexedImageDmaNotifiesMthdCnt           1
#define sizeSetIndexedImageDmaNotifies                  2
#define nvglSetIndexedImageDmaNotifies(ch, ctxdmanotify)                    \
{                                                                           \
    nvPushData (0, (((sizeSetIndexedImageDmaNotifiesMthdCnt << 2) << 16) | ((ch) << 13) | NV060_SET_CONTEXT_DMA_NOTIFIES)); \
    nvPushData (1, (ctxdmanotify));                                         \
    nvPusherAdjust (sizeSetIndexedImageDmaNotifies);                        \
}

#define sizeSetIndexedImageContextDmaLUTMthdCnt           1
#define sizeSetIndexedImageContextDmaLUT                  2
#define nvglSetIndexedImageContextDmaLUT(ch, handle)                        \
{                                                                           \
    nvPushData (0, (((sizeSetIndexedImageContextDmaLUTMthdCnt << 2) << 16) | ((ch) << 13) | NV060_SET_CONTEXT_DMA_LUT)); \
    nvPushData (1, (handle));                                               \
    nvPusherAdjust (sizeSetIndexedImageContextDmaLUT);                      \
}

#define sizeSetIndexedImageContextSurfaceMthdCnt        1
#define sizeSetIndexedImageContextSurface               2
#define nvglSetIndexedImageContextSurface(ch, surface)                      \
{                                                                           \
    nvPushData (0, (((sizeSetIndexedImageContextSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV060_SET_CONTEXT_SURFACE)); \
    nvPushData (1, (surface));                                              \
    nvPusherAdjust (sizeSetIndexedImageContextSurface);                     \
}

#define sizeSetIndexedImageOperationMthdCnt     1
#define sizeSetIndexedImageOperation            2
#define nvglSetIndexedImageOperation(ch, operation)                         \
{                                                                           \
    nvPushData (0, (((sizeSetIndexedImageOperationMthdCnt << 2) << 16) | ((ch) << 13) | NV060_SET_OPERATION)); \
    nvPushData (1, (operation));                                            \
    nvPusherAdjust (sizeSetIndexedImageOperation);                          \
}

#define sizeSetIndexedImageColorFormatMthdCnt   1
#define sizeSetIndexedImageColorFormat          2
#define nvglSetIndexedImageColorFormat(ch, colorformat)                     \
{                                                                           \
    nvPushData (0, (((sizeSetIndexedImageColorFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV060_SET_COLOR_FORMAT)); \
    nvPushData (1, (colorformat));                                          \
    nvPusherAdjust (sizeSetIndexedImageColorFormat);                        \
}

#define sizeSetIndexedImageIndexFormatMthdCnt   1
#define sizeSetIndexedImageIndexFormat          2
#define nvglSetIndexedImageIndexFormat(ch, indexformat)                     \
{                                                                           \
    nvPushData (0, (((sizeSetIndexedImageIndexFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV060_INDEX_FORMAT)); \
    nvPushData (1, (indexformat));                                          \
    nvPusherAdjust (sizeSetIndexedImageIndexFormat);                        \
}

#define sizeSetIndexedImageLUTOffsetMthdCnt     1
#define sizeSetIndexedImageLUTOffset            2
#define nvglSetIndexedImageLUTOffset(ch, offset)                            \
{                                                                           \
    nvPushData (0, (((sizeSetIndexedImageLUTOffsetMthdCnt << 2) << 16) | ((ch) << 13) | NV060_LUT_OFFSET)); \
    nvPushData (1, (offset));                                               \
    nvPusherAdjust (sizeSetIndexedImageLUTOffset);                          \
}

//-------------------------------------------------------------------------
//                              NV 077
//                   NV04_SCALED_IMAGE_FROM_MEMORY
//-------------------------------------------------------------------------

#define sizeSetScaledImageContextNotifiesMthdCnt    1
#define sizeSetScaledImageContextNotifies           2
#define nvglSetScaledImageContextNotifies(ch, ctxdmanotify)                 \
{                                                                           \
    nvPushData (0, (((sizeSetScaledImageContextNotifiesMthdCnt << 2) << 16) | ((ch) << 13) | NV077_SET_CONTEXT_DMA_NOTIFIES)); \
    nvPushData (1, (ctxdmanotify));                                         \
    nvPusherAdjust (sizeSetScaledImageContextNotifies);                     \
}

#define sizeSetScaledImageContextImageMthdCnt       1
#define sizeSetScaledImageContextImage              2
#define nvglSetScaledImageContextImage(ch, ctxdmaimage)                     \
{                                                                           \
    nvPushData (0, (((sizeSetScaledImageContextImageMthdCnt << 2) << 16) | ((ch) << 13) | NV077_SET_CONTEXT_DMA_IMAGE)); \
    nvPushData (1, (ctxdmaimage));                                          \
    nvPusherAdjust (sizeSetScaledImageContextImage);                        \
}

#define sizeSetScaledImageContextSurfaceMthdCnt     1
#define sizeSetScaledImageContextSurface            2
#define nvglSetScaledImageContextSurface(ch, ctxdmasurface)                 \
{                                                                           \
    nvPushData (0, (((sizeSetScaledImageContextSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV077_SET_CONTEXT_SURFACE)); \
    nvPushData (1, (ctxdmasurface));                                        \
    nvPusherAdjust (sizeSetScaledImageContextSurface);                      \
}

#define sizeSetScaledImageFormatMthdCnt             1
#define sizeSetScaledImageFormat                    2
#define nvglSetScaledImageFormat(ch, format)                                \
{                                                                           \
    nvPushData (0, (((sizeSetScaledImageFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV077_SET_COLOR_FORMAT)); \
    nvPushData (1, (format));                                               \
    nvPusherAdjust (sizeSetScaledImageFormat);                              \
}

#define sizeSetScaledImageOperationMthdCnt          1
#define sizeSetScaledImageOperation                 2
#define nvglSetScaledImageOperation(ch, operation)                          \
{                                                                           \
    nvPushData (0, (((sizeSetScaledImageOperationMthdCnt << 2) << 16) | ((ch) << 13) | NV077_SET_OPERATION)); \
    nvPushData (1, (operation));                                            \
    nvPusherAdjust (sizeSetScaledImageOperation);                           \
}

#define sizeScaledImageClipMthdCnt                  2
#define sizeScaledImageClip                         3
#define nvglScaledImageClip(ch, x, y, width, height)                        \
{                                                                           \
    nvPushData (0, (((sizeScaledImageClipMthdCnt << 2) << 16) | ((ch) << 13) | NV077_CLIP_POINT)); \
    nvPushData (1, ((((y) & 0x0000FFFF) << 16) | ((x) & 0x0000FFFF)));      \
    nvPushData (2, ((((height) & 0x0000FFFF) << 16) | ((width) & 0x0000FFFF))); \
    nvPusherAdjust (sizeScaledImageClip);                                   \
}

#define sizeScaledImageOutMthdCnt                   2
#define sizeScaledImageOut                          3
#define nvglScaledImageOut(ch, x, y, width, height)                         \
{                                                                           \
    nvPushData (0, (((sizeScaledImageOutMthdCnt << 2) << 16) | ((ch) << 13) | NV077_IMAGE_OUT_POINT)); \
    nvPushData (1, ((((y) & 0x0000FFFF) << 16) | ((x) & 0x0000FFFF)));      \
    nvPushData (2, ((((height) & 0x0000FFFF) << 16) | ((width) & 0x0000FFFF))); \
    nvPusherAdjust (sizeScaledImageOut);                                    \
}

#define sizeScaledImageDeltaDuDxDvDyMthdCnt         2
#define sizeScaledImageDeltaDuDxDvDy                3
#define nvglScaledImageDeltaDuDxDvDy(ch, dudx, dvdy)                        \
{                                                                           \
    nvPushData (0, (((sizeScaledImageDeltaDuDxDvDyMthdCnt << 2) << 16) | ((ch) << 13) | NV077_DELTA_DU_DX)); \
    nvPushData (1, (dudx));                                                 \
    nvPushData (2, (dvdy));                                                 \
    nvPusherAdjust (sizeScaledImageDeltaDuDxDvDy);                          \
}

#define sizeScaledImageInSizeMthdCnt                1
#define sizeScaledImageInSize                       2
#define nvglScaledImageInSize(ch, width, height)                            \
{                                                                           \
    nvPushData (0, (((sizeScaledImageInSizeMthdCnt << 2) << 16) | ((ch) << 13) | NV077_IMAGE_IN_SIZE)); \
    nvPushData (1, ((((height) & 0x0000FFFF) << 16) | ((width) & 0x0000FFFF))); \
    nvPusherAdjust (sizeScaledImageInSize);                                 \
}

#define sizeScaledImageInFormatMthdCnt              1
#define sizeScaledImageInFormat                     2
#define nvglScaledImageInFormat(ch, format)                                 \
{                                                                           \
    nvPushData (0, (((sizeScaledImageInFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV077_IMAGE_IN_FORMAT)); \
    nvPushData (1, (format));                                               \
    nvPusherAdjust (sizeScaledImageInFormat);                               \
}

#define sizeScaledImageInOffsetMthdCnt              1
#define sizeScaledImageInOffset                     2
#define nvglScaledImageInOffset(ch, offset)                                 \
{                                                                           \
    nvPushData (0, (((sizeScaledImageInOffsetMthdCnt << 2) << 16) | ((ch) << 13) | NV077_IMAGE_IN_OFFSET)); \
    nvPushData (1, (offset));                                               \
    nvPusherAdjust (sizeScaledImageInOffset);                               \
}

#define sizeScaledImageInPointMthdCnt               1
#define sizeScaledImageInPoint                      2
#define nvglScaledImageInPoint(ch, x, y)                                    \
{                                                                           \
    nvPushData (0, (((sizeScaledImageInPointMthdCnt << 2) << 16) | ((ch) << 13) | NV077_IMAGE_IN)); \
    nvPushData (1, ((((y) & 0x0000FFFF) << 16) | ((x) & 0x0000FFFF))); \
    nvPusherAdjust (sizeScaledImageInPoint); \
}

#define sizeScaledImageNotify                       4
#define nvglScaledImageNotify(ch)                                           \
{                                                                           \
    nvPushData (0, (((1 << 2) << 16) | ((ch) << 13) | NV077_NOTIFY));       \
    nvPushData (1, NV077_NOTIFY_WRITE_ONLY);                                \
    nvPushData (2, (((1 << 2) << 16) | ((ch) << 13) | NV077_NO_OPERATION)); \
    nvPushData (3, 0);                                                      \
    nvPusherAdjust (sizeScaledImageNotify);                                 \
}

//-------------------------------------------------------------------------
//                              NV 07C
//                     NV15_VIDEO_LUT_CURSOR_DAC
//-------------------------------------------------------------------------

// 0x00000300
#define sizeSetNv15VideoLUTCursorDACImageDataMthdCnt   2
#define sizeSetNv15VideoLUTCursorDACImageData          3
#define nvglSetNv15VideoLUTCursorDACImageData(ch, index, offset, format)    \
{                                                                           \
    nvPushData (0, (((sizeSetNv15VideoLUTCursorDACImageDataMthdCnt << 2) << 16) | ((ch) << 13) | NV07C_SET_IMAGE_OFFSET(index))); \
    nvPushData (1, offset);                                                 \
    nvPushData (2, format);                                                 \
    nvPusherAdjust (sizeSetNv15VideoLUTCursorDACImageData);                 \
}

//-------------------------------------------------------------------------
//                              NV 096
//                      NV15_CELSIUS_PRIMITIVE
//-------------------------------------------------------------------------

// 0x00000120
#define sizeSetNv15CelsiusSyncSetRWMMthdCnt        3
#define sizeSetNv15CelsiusSyncSetRWM               4
#define nvglSetNv15CelsiusSyncSetRWM(ch, read, write, modulo)               \
{                                                                           \
    nvPushData (0, (((sizeSetNv15CelsiusSyncSetRWMMthdCnt << 2) << 16) | ((ch) << 13) | NV096_SYNC_SET_READ)); \
    nvPushData (1, (read));                                                 \
    nvPushData (2, (write));                                                \
    nvPushData (3, (modulo));                                               \
    nvPusherAdjust (sizeSetNv15CelsiusSyncSetRWM);                          \
}
// 0x0000012c
#define sizeSetNv15CelsiusSyncIncWriteMthdCnt      1
#define sizeSetNv15CelsiusSyncIncWrite             2
#define nvglSetNv15CelsiusSyncIncWrite(ch)                                  \
{                                                                           \
    nvPushData (0, (((sizeSetNv15CelsiusSyncIncWriteMthdCnt << 2) << 16) | ((ch) << 13) | NV096_SYNC_INCREMENT_WRITE)); \
    nvPushData (1, 0);                                                      \
    nvPusherAdjust (sizeSetNv15CelsiusSyncIncWrite);                        \
}
// 0x00000130
#define sizeSetNv15CelsiusSyncStallMthdCnt         1
#define sizeSetNv15CelsiusSyncStall                2
#define nvglSetNv15CelsiusSyncStall(ch)                                     \
{                                                                           \
    nvPushData (0, (((sizeSetNv15CelsiusSyncStallMthdCnt << 2) << 16) | ((ch) << 13) | NV096_SYNC_STALL)); \
    nvPushData (1, 0);                                                      \
    nvPusherAdjust (sizeSetNv15CelsiusSyncStall);                           \
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NV4DMAC_H_
