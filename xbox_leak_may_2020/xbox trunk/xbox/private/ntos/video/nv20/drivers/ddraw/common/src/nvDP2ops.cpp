/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvDP2ops.cpp                                                      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler    bertrem     02June99    created                 *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

//#define INSTRUMENT       // display information about DP2 surfaces
#ifdef WINNT
#define VALIDATE_PARAMS  // validate vertex data to make sure it's in range
#endif

#ifdef  NV_NULL_DRIVER_DYNAMIC
DWORD NullDriverBits = 0;
inline BOOL NullBitSet(DWORD token)
{
    return (NullDriverBits & (1 << token));
}
#endif //NV_NULL_DRIVER_DYNAMIC

nvDP2FunctionTable nvDP2SetFuncs =
{
    NULL,                               //   0  invalid
    nvDP2Points,                        //   1  D3DDP2OP_POINTS
    nvDP2IndexedLineList,               //   2  D3DDP2OP_INDEXEDLINELIST
    nvDP2IndexedTriangleList,           //   3  D3DDP2OP_INDEXEDTRIANGLELIST
    NULL,                               //   4  invalid
    NULL,                               //   5  invalid
    NULL,                               //   6  invalid
    NULL,                               //   7  invalid
    nvDP2SetRenderState,                //   8  D3DDP2OP_RENDERSTATE
    NULL,                               //   9  NULL
    NULL,                               //  10  invalid
    nvDP2StreamEnd,                     //  11  legacy end-of-buffer marker
    NULL,                               //  12  invalid
    NULL,                               //  13  invalid
    NULL,                               //  14  invalid
    nvDP2LineList,                      //  15  D3DDP2OP_LINELIST
    nvDP2LineStrip,                     //  16  D3DDP2OP_LINESTRIP
    nvDP2IndexedLineStrip,              //  17  D3DDP2OP_INDEXEDLINESTRIP
    nvDP2TriangleList,                  //  18  D3DDP2OP_TRIANGLELIST
    nvDP2TriangleStrip,                 //  19  D3DDP2OP_TRIANGLESTRIP
    nvDP2IndexedTriangleStrip,          //  20  D3DDP2OP_INDEXEDTRIANGLESTRIP
    nvDP2TriangleFan,                   //  21  D3DDP2OP_TRIANGLEFAN
    nvDP2IndexedTriangleFan,            //  22  D3DDP2OP_INDEXEDTRIANGLEFAN
    nvDP2TriangleFanImm,                //  23  D3DDP2OP_TRIANGLEFAN_IMM
    nvDP2LineListImm,                   //  24  D3DDP2OP_LINELIST_IMM
    nvDP2SetTextureStageState,          //  25  D3DDP2OP_TEXTURESTAGESTATE
    nvDP2IndexedTriangleList2,          //  26  D3DDP2OP_INDEXEDTRIANGLELIST2
    nvDP2IndexedLineList2,              //  27  D3DDP2OP_INDEXEDLINELIST2
    nvDP2SetViewport,                   //  28  D3DDP2OP_VIEWPORTINFO
    nvDP2SetWRange,                     //  29  D3DDP2OP_WINFO
    nvDP2SetPalette,                    //  30  D3DDP2OP_SETPALETTE
    nvDP2UpdatePalette,                 //  31  D3DDP2OP_UPDATEPALETTE
    nvDP2SetZRange,                     //  32  D3DDP2OP_ZRANGE
    nvDP2SetMaterial,                   //  33  D3DDP2OP_SETMATERIAL
    nvDP2SetLight,                      //  34  D3DDP2OP_SETLIGHT
    nvDP2CreateLight,                   //  35  D3DDP2OP_CREATELIGHT
    nvDP2SetTransform,                  //  36  D3DDP2OP_SETTRANSFORM
    nvDP2Extension,                     //  37  D3DDP2OP_EXT
    nvDP2TexBlt,                        //  38  D3DDP2OP_TEXBLT
    nvDP2StateSet,                      //  39  D3DDP2OP_STATESET
    nvDP2SetPriority,                   //  40  D3DDP2OP_SETPRIORITY
    nvDP2SetRenderTarget,               //  41  D3DDP2OP_SETRENDERTARGET
    nvDP2Clear,                         //  42  D3DDP2OP_CLEAR
    nvDP2SetTexLOD,                     //  43  D3DDP2OP_SETTEXLOD
    nvDP2SetClipPlane,                  //  44  D3DDP2OP_SETCLIPPLANE
    nvDP2CreateVertexShader,            //  45  D3DDP2OP_CREATEVERTEXSHADER
    nvDP2DeleteVertexShader,            //  46  D3DDP2OP_DELETEVERTEXSHADER
    nvDP2SetVertexShader,               //  47  D3DDP2OP_SETVERTEXSHADER
    nvDP2SetVertexShaderConst,          //  48  D3DDP2OP_SETVERTEXSHADERCONST
    nvDP2SetStreamSource,               //  49  D3DDP2OP_SETSTREAMSOURCE
    nvDP2SetStreamSourceUM,             //  50  D3DNTDP2OP_SETSTREAMSOURCEUM
    nvDP2SetIndices,                    //  51  D3DDP2OP_SETINDICES
    nvDP2DrawPrimitive,                 //  52  D3DDP2OP_DRAWPRIMITIVE
    nvDP2DrawIndexedPrimitive,          //  53  D3DDP2OP_DRAWINDEXEDPRIMITIVE
    nvDP2CreatePixelShader,             //  54  D3DDP2OP_CREATEPIXELSHADER
    nvDP2DeletePixelShader,             //  55  D3DDP2OP_DELETEPIXELSHADER
    nvDP2SetPixelShader,                //  56  D3DDP2OP_SETPIXELSHADER
    nvDP2SetPixelShaderConst,           //  57  D3DNTDP2OP_SETPIXELSHADERCONST
    nvDP2ClippedTriangleFan,            //  58  D3DNTDP2OP_CLIPPEDTRIANGLEFAN
    nvDP2DrawPrimitive2,                //  59  D3DNTDP2OP_DRAWPRIMITIVE2
    nvDP2DrawIndexedPrimitive2,         //  60  D3DNTDP2OP_DRAWINDEXEDPRIMITIVE2
    nvDP2DrawRectSurface,               //  61  D3DNTDP2OP_DRAWRECTSURFACE
    nvDP2DrawTriSurface,                //  62  D3DNTDP2OP_DRAWTRISURFACE
    nvDP2VolumeBlt,                     //  63  D3DDP2OP_VOLUMEBLT
    nvDP2BufferBlt,                     //  64  D3DDP2OP_BUFFERBLT
    nvDP2MultiplyTransform
};

nvDP2FunctionTable nvDP2RecordFuncs =
{
    NULL,                               //   0  invalid
    NULL,                               //   1  D3DDP2OP_POINTS
    NULL,                               //   2  D3DDP2OP_INDEXEDLINELIST
    NULL,                               //   3  D3DDP2OP_INDEXEDTRIANGLELIST
    NULL,                               //   4  invalid
    NULL,                               //   5  invalid
    NULL,                               //   6  invalid
    NULL,                               //   7  invalid
    nvDP2RecordSetRenderState,          //   8  D3DDP2OP_RENDERSTATE
    NULL,                               //   9  invalid
    NULL,                               //  10  invalid
    NULL,                               //  11  invalid
    NULL,                               //  12  invalid
    NULL,                               //  13  invalid
    NULL,                               //  14  invalid
    NULL,                               //  15  D3DDP2OP_LINELIST
    NULL,                               //  16  D3DDP2OP_LINESTRIP
    NULL,                               //  17  D3DDP2OP_INDEXEDLINESTRIP
    NULL,                               //  18  D3DDP2OP_TRIANGLELIST
    NULL,                               //  19  D3DDP2OP_TRIANGLESTRIP
    NULL,                               //  20  D3DDP2OP_INDEXEDTRIANGLESTRIP
    NULL,                               //  21  D3DDP2OP_TRIANGLEFAN
    NULL,                               //  22  D3DDP2OP_INDEXEDTRIANGLEFAN
    NULL,                               //  23  D3DDP2OP_TRIANGLEFAN_IMM
    NULL,                               //  24  D3DDP2OP_LINELIST_IMM
    nvDP2RecordSetTextureStageState,    //  25  D3DDP2OP_TEXTURESTAGESTATE
    NULL,                               //  26  D3DDP2OP_INDEXEDTRIANGLELIST2
    NULL,                               //  27  D3DDP2OP_INDEXEDLINELIST2
    nvDP2RecordSetViewport,             //  28  D3DDP2OP_VIEWPORTINFO
    nvDP2RecordSetWRange,               //  29  D3DDP2OP_WINFO
    NULL,                               //  30  D3DDP2OP_SETPALETTE
    NULL,                               //  31  D3DDP2OP_UPDATEPALETTE
    nvDP2RecordSetZRange,               //  32  D3DDP2OP_ZRANGE
    nvDP2RecordSetMaterial,             //  33  D3DDP2OP_SETMATERIAL
    nvDP2RecordSetLight,                //  34  D3DDP2OP_SETLIGHT
    NULL,                               //  35  D3DDP2OP_CREATELIGHT
    nvDP2RecordSetTransform,            //  36  D3DDP2OP_SETTRANSFORM
    nvDP2RecordExtension,               //  37  D3DDP2OP_EXT
    NULL,                               //  38  D3DDP2OP_TEXBLT
    nvDP2StateSet,                      //  39  D3DDP2OP_STATESET
    NULL,                               //  40  D3DDP2OP_SETPRIORITY
    NULL,                               //  41  D3DDP2OP_SETRENDERTARGET
    NULL,                               //  42  D3DDP2OP_CLEAR
    NULL,                               //  43  D3DDP2OP_SETTEXLOD
    nvDP2RecordSetClipPlane,            //  44  D3DDP2OP_SETCLIPPLANE
    NULL,                               //  45  D3DDP2OP_CREATEVERTEXSHADER
    NULL,                               //  46  D3DDP2OP_DELETEVERTEXSHADER
    nvDP2RecordSetVertexShader,         //  47  D3DDP2OP_SETVERTEXSHADER
    nvDP2RecordSetVertexShaderConst,    //  48  D3DDP2OP_SETVERTEXSHADERCONST
    nvDP2RecordSetStreamSource,         //  49  D3DDP2OP_SETSTREAMSOURCE
    nvDP2RecordSetStreamSourceUM,       //  50  D3DNTDP2OP_SETSTREAMSOURCEUM
    nvDP2RecordSetIndices,              //  51  D3DDP2OP_SETINDICES
    NULL,                               //  52  D3DDP2OP_DRAWPRIMITIVE
    NULL,                               //  53  D3DDP2OP_DRAWINDEXEDPRIMITIVE
    NULL,                               //  54  D3DDP2OP_CREATEPIXELSHADER
    NULL,                               //  55  D3DDP2OP_DELETEPIXELSHADER
    nvDP2RecordSetPixelShader,          //  56  D3DDP2OP_SETPIXELSHADER
    nvDP2RecordSetPixelShaderConst,     //  57  D3DNTDP2OP_SETPIXELSHADERCONST
    NULL,                               //  58  D3DNTDP2OP_CLIPPEDTRIANGLEFAN
    NULL,                               //  59  D3DNTDP2OP_DRAWPRIMITIVE2
    NULL,                               //  60  D3DNTDP2OP_DRAWINDEXEDPRIMITIVE2
    NULL,                               //  61  D3DNTDP2OP_DRAWRECTSURFACE
    NULL,                               //  62  D3DNTDP2OP_DRAWTRISURFACE
    NULL,                               //  63  D3DDP2OP_VOLUMEBLT
    NULL,                               //  64  D3DDP2OP_BUFFERBLT
    nvDP2RecordMultiplyTransform        //  65  D3DDP2OP_BUFFERBLT
};

nvDP2FunctionTable nvDP2CaptureFuncs =
{
    NULL,                               //   0  invalid
    NULL,                               //   1  D3DDP2OP_POINTS
    NULL,                               //   2  D3DDP2OP_INDEXEDLINELIST
    NULL,                               //   3  D3DDP2OP_INDEXEDTRIANGLELIST
    NULL,                               //   4  invalid
    NULL,                               //   5  invalid
    NULL,                               //   6  invalid
    NULL,                               //   7  invalid
    nvDP2CaptureSetRenderState,         //   8  D3DDP2OP_RENDERSTATE
    NULL,                               //   9  invalid
    NULL,                               //  10  invalid
    NULL,                               //  11  invalid
    NULL,                               //  12  invalid
    NULL,                               //  13  invalid
    NULL,                               //  14  invalid
    NULL,                               //  15  D3DDP2OP_LINELIST
    NULL,                               //  16  D3DDP2OP_LINESTRIP
    NULL,                               //  17  D3DDP2OP_INDEXEDLINESTRIP
    NULL,                               //  18  D3DDP2OP_TRIANGLELIST
    NULL,                               //  19  D3DDP2OP_TRIANGLESTRIP
    NULL,                               //  20  D3DDP2OP_INDEXEDTRIANGLESTRIP
    NULL,                               //  21  D3DDP2OP_TRIANGLEFAN
    NULL,                               //  22  D3DDP2OP_INDEXEDTRIANGLEFAN
    NULL,                               //  23  D3DDP2OP_TRIANGLEFAN_IMM
    NULL,                               //  24  D3DDP2OP_LINELIST_IMM
    nvDP2CaptureSetTextureStageState,   //  25  D3DDP2OP_TEXTURESTAGESTATE
    NULL,                               //  26  D3DDP2OP_INDEXEDTRIANGLELIST2
    NULL,                               //  27  D3DDP2OP_INDEXEDLINELIST2
    nvDP2CaptureSetViewport,            //  28  D3DDP2OP_VIEWPORTINFO
    nvDP2CaptureSetWRange,              //  29  D3DDP2OP_WINFO
    NULL,                               //  30  D3DDP2OP_SETPALETTE
    NULL,                               //  31  D3DDP2OP_UPDATEPALETTE
    nvDP2CaptureSetZRange,              //  32  D3DDP2OP_ZRANGE
    nvDP2CaptureSetMaterial,            //  33  D3DDP2OP_SETMATERIAL
    nvDP2CaptureSetLight,               //  34  D3DDP2OP_SETLIGHT
    NULL,                               //  35  D3DDP2OP_CREATELIGHT
    nvDP2CaptureSetTransform,           //  36  D3DDP2OP_SETTRANSFORM
    nvDP2CaptureExtension,              //  37  D3DDP2OP_EXT
    NULL,                               //  38  D3DDP2OP_TEXBLT
    nvDP2StateSet,                      //  39  D3DDP2OP_STATESET
    NULL,                               //  40  D3DDP2OP_SETPRIORITY
    NULL,                               //  41  D3DDP2OP_SETRENDERTARGET
    NULL,                               //  42  D3DDP2OP_CLEAR
    NULL,                               //  43  D3DDP2OP_SETTEXLOD
    nvDP2CaptureSetClipPlane,           //  44  D3DDP2OP_SETCLIPPLANE
    NULL,                               //  45  D3DDP2OP_CREATEVERTEXSHADER
    NULL,                               //  46  D3DDP2OP_DELETEVERTEXSHADER
    nvDP2CaptureSetVertexShader,        //  47  D3DDP2OP_SETVERTEXSHADER
    nvDP2CaptureSetVertexShaderConst,   //  48  D3DDP2OP_SETVERTEXSHADERCONST
    nvDP2CaptureSetStreamSource,        //  49  D3DDP2OP_SETSTREAMSOURCE
    nvDP2CaptureSetStreamSourceUM,      //  50  D3DNTDP2OP_SETSTREAMSOURCEUM
    nvDP2CaptureSetIndices,             //  51  D3DDP2OP_SETINDICES
    NULL,                               //  52  D3DDP2OP_DRAWPRIMITIVE
    NULL,                               //  53  D3DDP2OP_DRAWINDEXEDPRIMITIVE
    NULL,                               //  54  D3DDP2OP_CREATEPIXELSHADER
    NULL,                               //  55  D3DDP2OP_DELETEPIXELSHADER
    nvDP2CaptureSetPixelShader,         //  56  D3DDP2OP_SETPIXELSHADER
    nvDP2CaptureSetPixelShaderConst,    //  57  D3DNTDP2OP_SETPIXELSHADERCONST
    NULL,                               //  58  D3DNTDP2OP_CLIPPEDTRIANGLEFAN
    NULL,                               //  59  D3DNTDP2OP_DRAWPRIMITIVE2
    NULL,                               //  60  D3DNTDP2OP_DRAWINDEXEDPRIMITIVE2
    NULL,                               //  61  D3DNTDP2OP_DRAWRECTSURFACE
    NULL,                               //  62  D3DNTDP2OP_DRAWTRISURFACE
    NULL,                               //  63  D3DDP2OP_VOLUMEBLT
    NULL,                               //  64  D3DDP2OP_BUFFERBLT
    NULL
};

//-------------------------------------------------------------------------

LPNVFVFINDEXEDPRIM fnFillModeIndexedTriangle[] =
{
    nvTriangleDispatch,  // illegal index, render solid by default
    nvPointIndexedTriangle,
    nvWireframeIndexedTriangle,
    nvTriangleDispatch
};

LPNVFVFDRAWPRIM fnFillModeNonIndexedTriangle[] =
{
    nvTriangleDispatch,  // illegal index, render solid by default
    nvPointNonIndexedTriangle,
    nvWireframeNonIndexedTriangle,
    nvTriangleDispatch
};

//---------------------------------------------------------------------------

HRESULT nvDP2Points (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    DDSTARTTICK(PRIM_POINTNONINDEXEDTRI);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2Points");

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((lpPrim + (wCommandCount * sizeof(D3DHAL_DP2POINTS))) > pCommandBufferEnd) {
        DPF ("commands run beyond end of command buffer in nvDP2Points");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_POINTNONINDEXEDTRI);
        return (DDERR_INVALIDPARAMS);
    }
#endif

    for (DWORD i=0; i < wCommandCount; i++) {

        pContext->dp2.dwDP2Prim     = D3DDP2OP_POINTS;
        pContext->dp2.dwVStart      = (DWORD)((LPD3DHAL_DP2POINTS)lpPrim)->wVStart;
        pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
        pContext->dp2.dwIndices     = NULL;
        pContext->dp2.dwIndexStride = 0;

        WORD wCount = ((LPD3DHAL_DP2POINTS)lpPrim)->wCount;

#ifdef VALIDATE_PARAMS
        // parameter validation (our responsibility under winNT)
        if ((pContext->dp2.dwTotalOffset + (wCount * pContext->pCurrentVShader->getStride())) > global.dwMaxVertexOffset) {
            DPF ("vertices run beyond end of vertex buffer in nvDP2Points");
            dbgD3DError();
            dbgTracePop();
            DDENDTICK(PRIM_POINTNONINDEXEDTRI);
            return (DDERR_INVALIDPARAMS);
        }
#endif

#if (NVARCH >= 0x020)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            nvKelvinDispatchNonIndexedPrimitive (pContext, wCount);
        }
        else
#endif
#if (NVARCH >= 0x010)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
            nvCelsiusDispatchNonIndexedPrimitive (pContext, wCount);
        }
        else
#endif
        {
            nvHWLockTextures (pContext);
            if ((!pContext->bUseDX6Class) && (pContext->pCurrentVShader->getFVF() == D3DFVF_TLVERTEX)) {
                nvDrawPointList (pContext, wCount);
            }
            else {
                nvFVFDrawPointList (pContext, wCount);
            }
            nvHWUnlockTextures (pContext);
        }

        lpPrim += sizeof(D3DHAL_DP2POINTS);
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2POINTS, wCommandCount, 0);

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_POINTNONINDEXEDTRI);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// DX5 DrawOnePrimitive style non-indexed line list.

HRESULT nvDP2LineList (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    DDSTARTTICK(PRIM_DRAWLINELISTNONINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2LineList");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_LINELIST;
    pContext->dp2.dwVStart      = (DWORD)((LPD3DHAL_DP2LINELIST)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwIndices     = NULL;
    pContext->dp2.dwIndexStride = 0;

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((pContext->dp2.dwTotalOffset + (2 * wCommandCount * pContext->pCurrentVShader->getStride())) > global.dwMaxVertexOffset) {
        DPF ("vertices run beyond end of vertex buffer in nvDP2LineList");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWLINELISTNONINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvKelvinDispatchNonIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvCelsiusDispatchNonIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        if ((!pContext->bUseDX6Class) && (pContext->pCurrentVShader->getFVF() == D3DFVF_TLVERTEX)) {
            nvDrawLine (pContext, wCommandCount);
        }
        else {
            nvFVFDrawLine (pContext, wCommandCount);
        }
        nvHWUnlockTextures (pContext);
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2LINELIST, 1, 0);

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWLINELISTNONINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

//  Old Execute Buffer RenderPrimitive style indexed line list.

HRESULT nvDP2IndexedLineList (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    LPBYTE pIndices      = lpPrim;

    DDSTARTTICK(PRIM_DRAWLINELISTINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2IndexedLineList");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_INDEXEDLINELIST;
    pContext->dp2.dwVStart      = 0;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset;
    pContext->dp2.dwIndices     = (DWORD)pIndices;
    pContext->dp2.dwIndexStride = 2;

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((pIndices + (2 * wCommandCount * sizeof(WORD))) > pCommandBufferEnd) {
        DPF ("indices run beyond end of command buffer in nvDP2IndexedLineList");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWLINELISTINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvKelvinDispatchIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvCelsiusDispatchIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        if ((!pContext->bUseDX6Class) && (pContext->pCurrentVShader->getFVF() == D3DFVF_TLVERTEX)) {
            nvIndexedLine (pContext, wCommandCount);
        }
        else {
            nvFVFIndexedLine (pContext, wCommandCount);
        }
        nvHWUnlockTextures (pContext);
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2INDEXEDLINELIST, wCommandCount, 0);

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWLINELISTINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// DX5 DrawOneIndexedPrimitive style indexed line list.

HRESULT nvDP2IndexedLineList2 (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    LPBYTE pIndices      = lpPrim + sizeof(D3DHAL_DP2STARTVERTEX);

    DDSTARTTICK(PRIM_DRAWLINELISTINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2IndexedLineList2");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_INDEXEDLINELIST2;
    pContext->dp2.dwVStart      = (DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwIndices     = (DWORD)(pIndices);
    pContext->dp2.dwIndexStride = 2;

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((pIndices + (2 * wCommandCount * sizeof(WORD))) > pCommandBufferEnd) {
        DPF ("indices run beyond end of command buffer in nvDP2IndexedLineList2");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWLINELISTINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvKelvinDispatchIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvCelsiusDispatchIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        if ((!pContext->bUseDX6Class) && (pContext->pCurrentVShader->getFVF() == D3DFVF_TLVERTEX)) {
            nvIndexedLine (pContext, wCommandCount);
        }
        else {
            nvFVFIndexedLine (pContext, wCommandCount);
        }
        nvHWUnlockTextures (pContext);
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2INDEXEDLINELIST, wCommandCount, sizeof(D3DHAL_DP2STARTVERTEX));

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWLINELISTINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// DX5 DrawOnePrimitive style in-line command buffer non-indexed line list.

HRESULT nvDP2LineListImm (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    LPBYTE pVertices     = lpPrim;

    DDSTARTTICK(PRIM_DRAWLINELISTNONINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2LineListImm");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_LINELIST;
    pContext->dp2.dwVStart      = 0;
    pContext->dp2.dwTotalOffset = 0;
    pContext->dp2.dwIndices     = NULL;
    pContext->dp2.dwIndexStride = 0;

    // Make sure command buffer is DWORD aligned for immediate line list vertices.
    pVertices = (LPBYTE)(((DWORD)pVertices + 3) & ~3);

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if (((DWORD)pVertices + (2 * wCommandCount * pContext->pCurrentVShader->getStride())) > (DWORD)pCommandBufferEnd) {
        DPF ("vertices run beyond end of vertex buffer in nvDP2LineListImm");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWLINELISTNONINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

    CVertexBuffer  tempVB;
    CVertexBuffer *pIncomingVB;
    DWORD          dwIncomingDMACount;

    // cache incoming vertex buffer and temporarily override it with a new one containing the immediate mode data
    pIncomingVB        = pContext->ppDX8Streams[0];
    dwIncomingDMACount = pContext->dwStreamDMACount;

    tempVB.own ((DWORD)pVertices, 2*wCommandCount*pContext->pCurrentVShader->getStride(), CSimpleSurface::HEAP_SYS);
    tempVB.setVertexStride (pContext->pCurrentVShader->getStride());
    pContext->ppDX8Streams[0] = &tempVB;
    pContext->dwStreamDMACount = nvCountStreamDMAs (pContext, 1);

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
#endif

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvKelvinDispatchNonIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvCelsiusDispatchNonIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        if ((!pContext->bUseDX6Class) && (pContext->pCurrentVShader->getFVF() == D3DFVF_TLVERTEX)) {
            nvDrawLine (pContext, wCommandCount);
        }
        else {
            nvFVFDrawLine (pContext, wCommandCount);
        }
        nvHWUnlockTextures (pContext);
    }

    // reinstate the incoming vertex buffer
    pContext->ppDX8Streams[0]  = pIncomingVB;
    pContext->dwStreamDMACount = dwIncomingDMACount;

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
#endif

    NEXTINSTRUCTION((*ppCommands), BYTE, ((wCommandCount + 1) * pContext->pCurrentVShader->getStride()), 0);
    // Realign next command since vertices are dword aligned
    (*ppCommands) = (LPD3DHAL_DP2COMMAND)(( ((DWORD)(*ppCommands)) + 3 ) & ~ 3);

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWLINELISTNONINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// DX5 DrawOnePrimitive style non-indexed line strip.

HRESULT nvDP2LineStrip (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    DDSTARTTICK(PRIM_DRAWLINESTRIPNONINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2LineStrip");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_LINESTRIP;
    pContext->dp2.dwVStart      = (DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwIndices     = NULL;
    pContext->dp2.dwIndexStride = 0;

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((pContext->dp2.dwTotalOffset + ((wCommandCount+1) * pContext->pCurrentVShader->getStride())) > global.dwMaxVertexOffset) {
        DPF ("vertices run beyond end of vertex buffer in nvDP2LineStrip");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWLINESTRIPNONINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvKelvinDispatchNonIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvCelsiusDispatchNonIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        if ((!pContext->bUseDX6Class) && (pContext->pCurrentVShader->getFVF() == D3DFVF_TLVERTEX)) {
            nvDrawLine (pContext, wCommandCount);
        }
        else {
            nvFVFDrawLine (pContext, wCommandCount);
        }
        nvHWUnlockTextures (pContext);
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2LINESTRIP, 1, 0);

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWLINESTRIPNONINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// DX5 DrawOneIndexedPrimitive style indexed line strip.

HRESULT nvDP2IndexedLineStrip (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    LPBYTE pIndices      = lpPrim + sizeof(D3DHAL_DP2STARTVERTEX);

    DDSTARTTICK(PRIM_DRAWLINESTRIPINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2IndexedLineStrip");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_INDEXEDLINESTRIP;
    pContext->dp2.dwVStart      = (DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwIndices     = (DWORD)(pIndices);
    pContext->dp2.dwIndexStride = 2;

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((pIndices + ((wCommandCount+1) * sizeof(WORD))) > pCommandBufferEnd) {
        DPF ("indices run beyond end of command buffer in nvDP2IndexedLineStrip");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWLINESTRIPINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvKelvinDispatchIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvCelsiusDispatchIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        if ((!pContext->bUseDX6Class) && (pContext->pCurrentVShader->getFVF() == D3DFVF_TLVERTEX)) {
            nvIndexedLine (pContext, wCommandCount);
        }
        else {
            nvFVFIndexedLine (pContext, wCommandCount);
        }
        nvHWUnlockTextures (pContext);
    }

    NEXTINSTRUCTION((*ppCommands), WORD, wCommandCount + 1, sizeof(D3DHAL_DP2STARTVERTEX));

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWLINESTRIPINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// DX5 DrawOnePrimitive style non-indexed triangle list.

HRESULT nvDP2TriangleList (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    DDSTARTTICK(PRIM_DRAWTRILISTNONINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2TriangleList");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLELIST;
    pContext->dp2.dwVStart      = (DWORD)((LPD3DHAL_DP2TRIANGLELIST)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwIndices     = NULL;
    pContext->dp2.dwIndexStride = 0;

#ifdef NV_NULL_DRIVER_DYNAMIC
    if (NullBitSet(D3DDP2OP_TRIANGLELIST)) {
        NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2TRIANGLELIST, 1, 0);
        dbgTracePop();
        return (D3D_OK);
    }
#endif //NV_NULL_DRIVER_DYNAMIC

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((pContext->dp2.dwTotalOffset + (3 * wCommandCount * pContext->pCurrentVShader->getStride())) > global.dwMaxVertexOffset) {
        DPF ("vertices run beyond end of vertex buffer in nvDP2TriangleList");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWTRILISTNONINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

    // Seen 3d data so we turn Palettization off
    pContext->bSeenTriangles = TRUE;

#if (NVARCH >= 0x20)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvKelvinDispatchNonIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif

#if (NVARCH >= 0x10)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvCelsiusDispatchNonIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        fnFillModeNonIndexedTriangle[pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]] (pContext, wCommandCount);
        nvHWUnlockTextures (pContext);
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2TRIANGLELIST, 1, 0);

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWTRILISTNONINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// old style RenderPrimitive Execute Buffer entry.

HRESULT nvDP2IndexedTriangleList (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    LPBYTE pIndices      = lpPrim;

    DDSTARTTICK(PRIM_DRAWTRILISTINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2IndexedTriangleList");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_INDEXEDTRIANGLELIST;
    pContext->dp2.dwVStart      = 0;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset;
    pContext->dp2.dwIndices     = (DWORD)(pIndices);
    pContext->dp2.dwIndexStride = 2;

#ifdef NV_NULL_DRIVER_DYNAMIC
    if (NullBitSet(D3DDP2OP_INDEXEDTRIANGLELIST)) {
        NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2INDEXEDTRIANGLELIST, wCommandCount, 0);
        dbgTracePop();
        return (D3D_OK);
    }
#endif //NV_NULL_DRIVER_DYNAMIC

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((pIndices + (4 * wCommandCount * sizeof(WORD))) > pCommandBufferEnd) {
        DPF ("indices run beyond end of command buffer in nvDP2IndexedTriangleList");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWTRILISTINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

    // Seen 3d data so we turn Palettization off
    pContext->bSeenTriangles = TRUE;

#if (NVARCH >= 0x20)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        // if we're in wireframe mode, we have to heed edge flags...
        ((pContext->dwRenderState[D3DRENDERSTATE_FILLMODE] == D3DFILL_WIREFRAME) ?
         nvKelvinDispatchLegacyWireframePrimitive :
         nvKelvinDispatchIndexedPrimitive) (pContext, wCommandCount);
    }
    else
#endif

#if (NVARCH >= 0x10)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        // if we're in wireframe mode, we have to heed edge flags...
        ((pContext->dwRenderState[D3DRENDERSTATE_FILLMODE] == D3DFILL_WIREFRAME) ?
         nvCelsiusDispatchLegacyWireframePrimitive :
         nvCelsiusDispatchIndexedPrimitive) (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        fnFillModeIndexedTriangle[pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]] (pContext, wCommandCount);
        nvHWUnlockTextures (pContext);
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2INDEXEDTRIANGLELIST, wCommandCount, 0);

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWTRILISTINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------
// DX5 style DrawOneIndexedPrimitive indexed triangle strip.
HRESULT nvDP2IndexedTriangleList2 (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    LPBYTE pIndices      = lpPrim + sizeof(D3DHAL_DP2STARTVERTEX);

    DDSTARTTICK(PRIM_DRAWTRILISTINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2IndexedTriangleList2");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_INDEXEDTRIANGLELIST2;
    pContext->dp2.dwVStart      = (DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwIndices     = (DWORD)(pIndices);
    pContext->dp2.dwIndexStride = 2;

#ifdef NV_NULL_DRIVER_DYNAMIC
    if (NullBitSet(D3DDP2OP_INDEXEDTRIANGLELIST2)) {
        NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2INDEXEDTRIANGLELIST2, wCommandCount, sizeof(D3DHAL_DP2STARTVERTEX));
        dbgTracePop();
        return (D3D_OK);
    }
#endif //NV_NULL_DRIVER_DYNAMIC

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((pIndices + (3 * wCommandCount * sizeof(WORD))) > pCommandBufferEnd) {
        DPF ("indices run beyond end of command buffer in nvDP2IndexedTriangleList2");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWTRILISTINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

    // Seen 3d data so we turn Palettization off
    pContext->bSeenTriangles = TRUE;

#if (NVARCH >= 0x20)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvKelvinDispatchIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif

#if (NVARCH >= 0x10)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvCelsiusDispatchIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        fnFillModeIndexedTriangle[pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]] (pContext, wCommandCount);
        nvHWUnlockTextures (pContext);
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2INDEXEDTRIANGLELIST2, wCommandCount, sizeof(D3DHAL_DP2STARTVERTEX));

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWTRILISTINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// DX5 DrawOnePrimitive style non-indexed triangle strip.

HRESULT nvDP2TriangleStrip (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    DDSTARTTICK(PRIM_DRAWSTRIPSOLIDNONINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2TriangleStrip");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLESTRIP;
    pContext->dp2.dwVStart      = (DWORD)((LPD3DHAL_DP2TRIANGLESTRIP)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwIndices     = NULL;
    pContext->dp2.dwIndexStride = 0;

#ifdef NV_NULL_DRIVER_DYNAMIC
    if (NullBitSet(D3DDP2OP_TRIANGLESTRIP)) {
        NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2TRIANGLESTRIP, 1, 0);
        dbgTracePop();
        return (D3D_OK);
    }
#endif //NV_NULL_DRIVER_DYNAMIC

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((pContext->dp2.dwTotalOffset + ((wCommandCount+2) * pContext->pCurrentVShader->getStride())) > global.dwMaxVertexOffset) {
        DPF ("vertices run beyond end of vertex buffer in nvDP2TriangleStrip");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWSTRIPSOLIDNONINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

    // Seen 3d data so we turn Palettization off
    pContext->bSeenTriangles = TRUE;

#if (NVARCH >= 0x20)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvKelvinDispatchNonIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif

#if (NVARCH >= 0x10)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvCelsiusDispatchNonIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        fnFillModeNonIndexedTriangle[pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]] (pContext, wCommandCount);
        nvHWUnlockTextures (pContext);
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2TRIANGLESTRIP, 1, 0);

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWSTRIPSOLIDNONINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// DX5 DrawOneIndexedPrimitive style indexed triangle strip.

HRESULT nvDP2IndexedTriangleStrip (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    LPBYTE pIndices      = lpPrim + sizeof(D3DHAL_DP2STARTVERTEX);

    DDSTARTTICK(PRIM_DRAWSTRIPSOLIDINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2IndexedTriangleStrip");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_INDEXEDTRIANGLESTRIP;
    pContext->dp2.dwVStart      = (DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwIndices     = (DWORD)(pIndices);
    pContext->dp2.dwIndexStride = 2;

#ifdef NV_NULL_DRIVER_DYNAMIC
    if (NullBitSet(D3DDP2OP_INDEXEDTRIANGLESTRIP)) {
        NEXTINSTRUCTION((*ppCommands), WORD, wCommandCount + 2, sizeof(D3DHAL_DP2STARTVERTEX));
        dbgTracePop();
        return (D3D_OK);
    }
#endif //NV_NULL_DRIVER_DYNAMIC

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((pIndices + ((wCommandCount+2) * sizeof(WORD))) > pCommandBufferEnd) {
        DPF ("indices run beyond end of command buffer in nvDP2IndexedTriangleStrip");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWSTRIPSOLIDINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

    // Seen 3d data so we turn Palettization off
    pContext->bSeenTriangles = TRUE;

#if (NVARCH >= 0x20)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvKelvinDispatchIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif

#if (NVARCH >= 0x10)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvCelsiusDispatchIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        fnFillModeIndexedTriangle[pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]] (pContext, wCommandCount);
        nvHWUnlockTextures (pContext);
    }

    NEXTINSTRUCTION((*ppCommands), WORD, wCommandCount + 2, sizeof(D3DHAL_DP2STARTVERTEX));

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWSTRIPSOLIDINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// DX5 DrawOnePrimitive style non-indexed triangle fan.

HRESULT nvDP2TriangleFan (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    DDSTARTTICK(PRIM_DRAWFANSOLIDNONINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2TriangleFan");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLEFAN;
    pContext->dp2.dwVStart      = (DWORD)((LPD3DHAL_DP2TRIANGLEFAN)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwIndices     = NULL;
    pContext->dp2.dwIndexStride = 0;

#ifdef NV_NULL_DRIVER_DYNAMIC
    if (NullBitSet(D3DDP2OP_TRIANGLEFAN)) {
        NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2TRIANGLEFAN, 1, 0);
        dbgTracePop();
        return (D3D_OK);
    }
#endif //NV_NULL_DRIVER_DYNAMIC

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((pContext->dp2.dwTotalOffset + ((wCommandCount+2) * pContext->pCurrentVShader->getStride())) > global.dwMaxVertexOffset) {
        DPF ("vertices run beyond end of vertex buffer in nvDP2TriangleFan");
        dbgD3DError();
        DDENDTICK(PRIM_DRAWFANSOLIDNONINDEXED);
        dbgTracePop();
        return (DDERR_INVALIDPARAMS);
    }
#endif

    // Seen 3d data so we turn Palettization off
    pContext->bSeenTriangles = TRUE;

#if (NVARCH >= 0x20)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvKelvinDispatchNonIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
#if (NVARCH >= 0x10)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvCelsiusDispatchNonIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        fnFillModeNonIndexedTriangle[pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]] (pContext, wCommandCount);
        nvHWUnlockTextures (pContext);
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2TRIANGLEFAN, 1, 0);

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWFANSOLIDNONINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// DX5 DrawOneIndexedPrimitive style indexed triangle fan.

HRESULT nvDP2IndexedTriangleFan (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    LPBYTE pIndices      = lpPrim + sizeof(D3DHAL_DP2STARTVERTEX);

    DDSTARTTICK(PRIM_DRAWFANSOLIDINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2IndexedTriangleFan");

    pContext->dp2.dwVStart      = (DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwDP2Prim     = D3DDP2OP_INDEXEDTRIANGLEFAN;
    pContext->dp2.dwIndices     = (DWORD)(pIndices);
    pContext->dp2.dwIndexStride = 2;

#ifdef NV_NULL_DRIVER_DYNAMIC
    if (NullBitSet(D3DDP2OP_INDEXEDTRIANGLEFAN)) {
        NEXTINSTRUCTION((*ppCommands), WORD, wCommandCount + 2, sizeof(D3DHAL_DP2STARTVERTEX));
        dbgTracePop();
        return (D3D_OK);
    }
#endif //NV_NULL_DRIVER_DYNAMIC

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((pIndices + ((wCommandCount+2) * sizeof(WORD))) > pCommandBufferEnd) {
        DPF ("indices run beyond end of command buffer in nvDP2IndexedTriangleFan");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWFANSOLIDINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

    // Seen 3d data so we turn Palettization off
    pContext->bSeenTriangles = TRUE;

#if (NVARCH >= 0x20)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvKelvinDispatchIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
#if (NVARCH >= 0x10)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvCelsiusDispatchIndexedPrimitive (pContext, wCommandCount);
    }
    else
#endif
    {
        nvHWLockTextures (pContext);
        fnFillModeIndexedTriangle[pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]] (pContext, wCommandCount);
        nvHWUnlockTextures (pContext);
    }

    NEXTINSTRUCTION((*ppCommands), WORD, wCommandCount + 2, sizeof(D3DHAL_DP2STARTVERTEX));

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWFANSOLIDINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// Inline command buffer non-indexed triangle fans. Similar to DX5 style DrawOnePrimitive.

HRESULT nvDP2TriangleFanImm (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    LPBYTE pVertices     = lpPrim + sizeof(D3DHAL_DP2TRIANGLEFAN_IMM);
           pVertices     = (LPBYTE)(((DWORD)pVertices + 3) & ~3);

    DDSTARTTICK(PRIM_DRAWFANSOLIDNONINDEXED);
    DDSTATLOGWORDS(D3D_PRIM_COUNT_PARMS, 1, (DWORD)wCommandCount);
    dbgTracePush ("nvDP2TriangleFanImm");

#ifdef NV_NULL_DRIVER_DYNAMIC
    if (NullBitSet(D3DDP2OP_TRIANGLEFAN_IMM)) {
        NEXTINSTRUCTION((*ppCommands), BYTE, ((wCommandCount + 2) * fvfData.dwVertexStride), sizeof(D3DHAL_DP2TRIANGLEFAN_IMM));
        dbgTracePop();
        return (D3D_OK);
    }
#endif //NV_NULL_DRIVER_DYNAMIC

    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLEFAN_IMM;
    pContext->dp2.dwVStart      = 0;
    pContext->dp2.dwTotalOffset = 0;
    pContext->dp2.dwEdgeFlags   = ((LPD3DHAL_DP2TRIANGLEFAN_IMM)lpPrim)->dwEdgeFlags;
    pContext->dp2.dwIndices     = NULL;
    pContext->dp2.dwIndexStride = 0;

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if (((DWORD)pVertices + ((wCommandCount+2) * pContext->pCurrentVShader->getStride())) > (DWORD)pCommandBufferEnd) {
        DPF ("vertices run beyond end of vertex buffer in nvDP2TriangleFanImm");
        dbgD3DError();
        dbgTracePop();
        DDENDTICK(PRIM_DRAWFANSOLIDNONINDEXED);
        return (DDERR_INVALIDPARAMS);
    }
#endif

    // Seen 3d data so we turn Palettization off
    pContext->bSeenTriangles = TRUE;

    CVertexBuffer  tempVB;
    CVertexBuffer *pIncomingVB;
    DWORD          dwIncomingDMACount;

    // cache incoming vertex buffer and temporarily override it with a new one containing the immediate mode data
    pIncomingVB        = pContext->ppDX8Streams[0];
    dwIncomingDMACount = pContext->dwStreamDMACount;

    tempVB.own ((DWORD)pVertices, (wCommandCount+2)*pContext->pCurrentVShader->getStride(), CSimpleSurface::HEAP_SYS);
    tempVB.setVertexStride (pContext->pCurrentVShader->getStride());
    pContext->ppDX8Streams[0] = &tempVB;
    pContext->dwStreamDMACount = nvCountStreamDMAs (pContext, 1);

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
#endif

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        // if we're in wireframe mode, we have to heed edge flags...
        ((pContext->dwRenderState[D3DRENDERSTATE_FILLMODE] == D3DFILL_WIREFRAME) ?
         nvKelvinDispatchLegacyWireframePrimitive :
         nvKelvinDispatchNonIndexedPrimitive) (pContext, wCommandCount);
    }
    else
#endif

#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {

#ifdef WINNT    // setup for NT bounds checking, save current value
        DWORD temp = global.dwMaxVertexOffset;
        global.dwMaxVertexOffset = (wCommandCount + 2) * pContext->pCurrentVShader->getStride();
#endif

        // if we're in wireframe mode, we have to heed edge flags...
        ((pContext->dwRenderState[D3DRENDERSTATE_FILLMODE] == D3DFILL_WIREFRAME) ?
         nvCelsiusDispatchLegacyWireframePrimitive :
         nvCelsiusDispatchNonIndexedPrimitive) (pContext, wCommandCount);

#ifdef WINNT    // restore previous value
         global.dwMaxVertexOffset = temp;
#endif
    }

    else
#endif
    {
        nvHWLockTextures (pContext);
        fnFillModeNonIndexedTriangle[pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]] (pContext, wCommandCount);
        nvHWUnlockTextures (pContext);
    }

    // reinstate the incoming vertex buffer
    pContext->ppDX8Streams[0]  = pIncomingVB;
    pContext->dwStreamDMACount = dwIncomingDMACount;

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
#endif

    NEXTINSTRUCTION((*ppCommands), BYTE, ((wCommandCount + 2) * pContext->pCurrentVShader->getStride()), sizeof(D3DHAL_DP2TRIANGLEFAN_IMM));
    // Realign next command since vertices are dword aligned and store # of primitives before affecting the pointer
    (*ppCommands) = (LPD3DHAL_DP2COMMAND)((((DWORD)(*ppCommands)) + 3 ) & ~ 3);

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    DDENDTICK(PRIM_DRAWFANSOLIDNONINDEXED);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetRenderState (NV_DP2FUNCTION_ARGLIST)
{
    WORD    wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE  lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    DWORD   i, dwState, dwValue;
    HRESULT ddrval = D3D_OK;

    dbgTracePush ("nvDP2SetRenderState");

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((lpPrim + (wCommandCount * sizeof(D3DHAL_DP2POINTS))) > pCommandBufferEnd) {
        DPF ("commands run beyond end of command buffer in nvDP2SetRenderState");
        dbgD3DError();
        dbgTracePop();
        return (DDERR_INVALIDPARAMS);
    }
#endif

    // Run through the render state list.
    nvAssert (wCommandCount != 0);

    for (i = 0; i < wCommandCount; i++) {

        dwState = ((LPD3DHAL_DP2RENDERSTATE)lpPrim)->RenderState;
        dwValue = ((LPD3DHAL_DP2RENDERSTATE)lpPrim)->dwState;
        lpPrim += sizeof(D3DHAL_DP2RENDERSTATE);

        if (IS_OVERRIDE(dwState)) {
            DWORD override = GET_OVERRIDE(dwState);
            if (((override - 1) >> DWORD_SHIFT) >= (MAX_STATE >> DWORD_SHIFT))
            {
                DPF("Invalid state override\n");
                dbgD3DError();
                continue;
            }
            if (dwValue) {
                DPF_LEVEL (NVDBG_LEVEL_INFO,
                           "nvDrawPrimitives2 - setting override for state %08x",
                           override);
                STATESET_SET(pContext->overrides, override);
            }
            else {
                DPF_LEVEL (NVDBG_LEVEL_INFO,
                           "nvDrawPrimitives2, clearing override for state %08x",
                           override);
                STATESET_CLEAR(pContext->overrides, override);
            }
            continue;
        }
        if (STATESET_ISSET(pContext->overrides, dwState)) {
            DPF_LEVEL (NVDBG_LEVEL_INFO,
                       "nvDrawPrimitives2, state %08x is overridden, ignoring",
                       dwState);
            continue;
        }
        if ((pContext->dwDXAppVersion < 0x700)
         && pdwDP2RStates)
        {
            // state blocks are broken in the runtime of we write this state back

            pdwDP2RStates[dwState] = dwValue;
        }

        nvSetContextState (pContext, dwState, dwValue, &ddrval);

        // Want to show that the application has set texturemapblend renderstate even if
        // it's the same as the last time.
        if (dwState == D3DRENDERSTATE_TEXTUREMAPBLEND)
        {
            pContext->bUseTBlendSettings = TRUE;
#if (NVARCH >= 0x010)
            pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_COMBINERS;
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_COMBINERS_COLOR;
#endif
        }

    }

    // If the state has changed force a hardware state load.
    if (pContext->bStateChange) {
        // If this is a DX6 application, then force hardware steup through
        // the DX6 hardware setup routine, even if there is only one stage.
        pContext->bUseDX6Class = (pContext->dwDXAppVersion >= 0x0600);
        NV_FORCE_TRI_SETUP(pContext);
    }

    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvSetRenderState failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2RENDERSTATE, wCommandCount, 0);

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    return (ddrval);
}

//-------------------------------------------------------------------------------------

HRESULT nvDP2SetTextureStageState (NV_DP2FUNCTION_ARGLIST)
{
    WORD    wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE  lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    BOOL    bTSSChanged;
    DWORD   i, dwStage, dwState, dwValue;
    HRESULT ddrval = D3D_OK;

    dbgTracePush ("nvDP2SetTextureStageState");

#ifdef VALIDATE_PARAMS
    // parameter validation (our responsibility under winNT)
    if ((lpPrim + (wCommandCount * sizeof(D3DHAL_DP2POINTS))) > pCommandBufferEnd) {
        DPF ("commands run beyond end of command buffer in nvDP2SetTextureStageState");
        dbgD3DError();
        dbgTracePop();
        return (DDERR_INVALIDPARAMS);
    }
#endif

    bTSSChanged = FALSE;

    for (i = 0; i < wCommandCount; i++) {

        dwStage = (DWORD)((LPD3DHAL_DP2TEXTURESTAGESTATE)lpPrim)->wStage;
        dwState = ((LPD3DHAL_DP2TEXTURESTAGESTATE)lpPrim)->TSState;
        dwValue = ((LPD3DHAL_DP2TEXTURESTAGESTATE)lpPrim)->dwValue;

        if (dbgShowState & NVDBG_SHOW_RENDER_STATE)
            DPF("Texture Stage State Change - Stage: %d, State: %d, Value: %d", dwStage, dwState, dwValue);

        switch (dwState) {

            case D3DTSS_TEXTUREMAP:  // 0
                // assert that no texture has been set via D3DRENDERSTATE_TEXTUREHANDLE
                nvAssert (pContext->tssState[0].dwHandle == 0);
                //check to see if the current streams/rendertarget/Z/textures need to be unlocked
                if(pContext->dwHWUnlockAllPending){ //HMH
                    nvHWUnlockTextures (pContext);
                    // need to mark all streams as in use by hardware...
                    nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
                    pContext->dwHWUnlockAllPending=FALSE;
                }
                // Turn the texture handle into a texture pointer.
                if (dwValue) {
                    DWORD dwTranslatedValue;
                    if (global.dwDXRuntimeVersion >= 0x0700) {
                        dwTranslatedValue = (DWORD) nvGetObjectFromHandle (pContext->dwDDLclID, dwValue, &global.pNvSurfaceLists);
                    }
                    else {
                        dwTranslatedValue = dwValue | 0x80000000;
                    }
                    if (pContext->tssState[dwStage].dwValue[D3DTSS_TEXTUREMAP] != dwTranslatedValue) {
                        pContext->tssState[dwStage].dwValue[D3DTSS_TEXTUREMAP] = dwTranslatedValue;
                        bTSSChanged = TRUE;
                    }
                }
                else {
                    if (pContext->tssState[dwStage].dwValue[D3DTSS_TEXTUREMAP]) {
                        pContext->tssState[dwStage].dwValue[D3DTSS_TEXTUREMAP] = dwValue;
                        bTSSChanged = TRUE;
                    }
                }
                break;

            case D3DTSS_COLOROP:    // 1
            case D3DTSS_COLORARG1:  // 2
            case D3DTSS_COLORARG2:  // 3
            case D3DTSS_ALPHAOP:    // 4
            case D3DTSS_ALPHAARG1:  // 5
            case D3DTSS_ALPHAARG2:  // 6
            case D3DTSS_COLORARG0:  // 6
            case D3DTSS_ALPHAARG0:  // 6
                if ((pContext->tssState[dwStage].dwValue[dwState] != dwValue) || (pContext->bUseTBlendSettings)) {
                    pContext->tssState[dwStage].dwValue[dwState] = dwValue;
                    bTSSChanged = TRUE;
                    pContext->bUseTBlendSettings = FALSE;
                }
                break;

            case D3DTSS_ADDRESS:  // 12
                if ((pContext->tssState[dwStage].dwValue[D3DTSS_ADDRESSU] != dwValue) ||
                    (pContext->tssState[dwStage].dwValue[D3DTSS_ADDRESSV] != dwValue)) {
                    pContext->tssState[dwStage].dwValue[D3DTSS_ADDRESSU] = dwValue;
                    pContext->tssState[dwStage].dwValue[D3DTSS_ADDRESSV] = dwValue;
                    bTSSChanged = TRUE;
                }
                break;

            case D3DTSS_MIPMAPLODBIAS:  // 19
                if (pContext->tssState[dwStage].dwValue[D3DTSS_MIPMAPLODBIAS] != dwValue) {
                    pContext->tssState[dwStage].dwValue[D3DTSS_MIPMAPLODBIAS] = dwValue;
                    pContext->tssState[dwStage].dwLODBias = nvTranslateLODBias (dwValue);
                    bTSSChanged = TRUE;
                }
                break;

            default:
                nvAssert (dwState <= D3D_TEXSTAGESTATE_MAX);
                if (pContext->tssState[dwStage].dwValue[dwState] != dwValue) {
                    pContext->tssState[dwStage].dwValue[dwState] = dwValue;
                    bTSSChanged = TRUE;
                }
                break;

        } // switch

        nvAssert (dwState <= D3D_TEXSTAGESTATE_MAX);
        if (bTSSChanged) {
            #if (NVARCH >= 0x010)
                pContext->hwState.dwDirtyFlags |= celsiusDirtyBitsFromTextureStageState[dwState];
                pContext->hwState.dwDirtyFlags |= kelvinDirtyBitsFromTextureStageState[dwState];
            #endif
        }

        lpPrim += sizeof (D3DHAL_DP2TEXTURESTAGESTATE);

    } // for (i...

    // If the state has changed force a hardware state load.
    if (bTSSChanged) {
        // If this is a DX6 application, then force hardware steup through
        // the DX6 hardware setup routine, even if there is only one stage.
        pContext->bUseDX6Class  = (pContext->dwDXAppVersion >= 0x0600);
        pContext->bStateChange = TRUE;
        NV_FORCE_TRI_SETUP(pContext);
    }

    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvSetTSS failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2TEXTURESTAGESTATE, wCommandCount, 0);

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    DDTICKOFFSET(dwState+D3D_NUM_RENDERSTATES, dwValue);
    return (ddrval);
}

//-------------------------------------------------------------------------------------

HRESULT nvDP2SetViewport (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim;
    WORD   wX, wY, wWidth, wHeight;

    DDSTARTTICK(DEVICE7_SETVIEWPORT);
    dbgTracePush ("nvDP2SetViewport");

    lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    wX      = (WORD)(((LPD3DHAL_DP2VIEWPORTINFO)lpPrim)->dwX & 0x0000FFFF);
    wY      = (WORD)(((LPD3DHAL_DP2VIEWPORTINFO)lpPrim)->dwY & 0x0000FFFF);
    wWidth  = (WORD)(((LPD3DHAL_DP2VIEWPORTINFO)lpPrim)->dwWidth & 0x0000FFFF);
    wHeight = (WORD)(((LPD3DHAL_DP2VIEWPORTINFO)lpPrim)->dwHeight & 0x0000FFFF);

    if ((pContext->surfaceViewport.clipHorizontal.wX != wX)         ||
        (pContext->surfaceViewport.clipVertical.wY != wY)           ||
        (pContext->surfaceViewport.clipHorizontal.wWidth != wWidth) ||
        (pContext->surfaceViewport.clipVertical.wHeight != wHeight)) {

        pContext->surfaceViewport.clipHorizontal.wX     = wX;
        pContext->surfaceViewport.clipVertical.wY       = wY;
        pContext->surfaceViewport.clipHorizontal.wWidth = wWidth;
        pContext->surfaceViewport.clipVertical.wHeight  = wHeight;

#if (NVARCH >= 0x020)
        pContext->hwState.dwDirtyFlags  |= KELVIN_DIRTY_TRANSFORM;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TRANSFORM;
#endif
        if (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
            !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN))
        {
            nvSetD3DSurfaceViewport(pContext);
        }
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2VIEWPORTINFO, 1, 0);

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    DDENDTICK(DEVICE7_SETVIEWPORT);
    return (D3D_OK);
}

//-------------------------------------------------------------------------------------

void nvSetWRange(PNVD3DCONTEXT pContext, D3DVALUE dvWNear, D3DVALUE dvWFar)
{
    pContext->surfaceViewport.dvWNear = dvWNear;
    pContext->surfaceViewport.dvWFar  = dvWFar;

    // If the WFar value is 0.0, then set a default based on the z-buffer precision.
    if (pContext->surfaceViewport.dvWFar == 0.0f) {
        pContext->surfaceViewport.dvWNear = 0.0f;
        if (pContext->pZetaBuffer && pContext->pZetaBuffer->getBPP() == 2) {
            pContext->surfaceViewport.dvWFar = (D3DVALUE) getDC()->nvD3DRegistryData.regfWScale16;
        }
        else {
            pContext->surfaceViewport.dvWFar = (D3DVALUE) getDC()->nvD3DRegistryData.regfWScale24;
        }
    }

    nvCalcRWFar(pContext);

    // calculate the inverse
    nvAssert (pContext->surfaceViewport.dvWFar != 0.0f);
    pContext->surfaceViewport.dvInvWFar = 1.0f / pContext->surfaceViewport.dvWFar;

    nvDetermineEarlyCopyStrategy(pContext);
    pContext->hwState.SuperTri.setStrategy();

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_TRANSFORM | KELVIN_DIRTY_TRANSFORM);
#endif

    // determine if fog source is Z or W
    if ((*(DWORD *)&(pContext->surfaceViewport.dvWNear) == FP_ONE_BITS) &&
        (*(DWORD *)&(pContext->surfaceViewport.dvWFar)  == FP_ONE_BITS)) {
        pContext->hwState.dwFogSource = CHardwareState::FOG_SOURCE_Z;
    }
    else {
        pContext->hwState.dwFogSource = CHardwareState::FOG_SOURCE_W;
    }

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_SPECFOG_COMBINER | KELVIN_DIRTY_VERTEX_SHADER);
#endif
}

//-------------------------------------------------------------------------------------

HRESULT nvDP2SetWRange (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    DDSTARTTICK(DEVICE7_SETWRANGE);
    dbgTracePush ("nvDP2SetWRange");

    nvSetWRange(pContext, ((LPD3DHAL_DP2WINFO)lpPrim)->dvWNear, ((LPD3DHAL_DP2WINFO)lpPrim)->dvWFar);

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2WINFO, 1, 0);

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    DDENDTICK(DEVICE7_SETWRANGE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetPalette(NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    DWORD  hr     = DD_OK;

    DDSTARTTICK(SURF7_SETPALETTE);
    dbgTracePush ("nvDP2SetPalette");

    // on DX7 we get spurious and calls to this function with totally bogus data. just ignore them
    if (global.dwDXRuntimeVersion >= 0x0800) {

        LPD3DHAL_DP2SETPALETTE pSetPalette = (LPD3DHAL_DP2SETPALETTE)lpPrim;

        nvAssert (pSetPalette->dwPaletteHandle != 0);   //maybe a delete call?
        nvAssert (pSetPalette->dwSurfaceHandle != 0);   //maybe a delete call?

        CNvObject *pSurfObj = nvGetObjectFromHandle (pContext->dwDDLclID, pSetPalette->dwSurfaceHandle, &global.pNvSurfaceLists);
        CNvObject *pPalObj  = nvGetObjectFromHandle (pContext->dwDDLclID, pSetPalette->dwPaletteHandle, &global.pNvPaletteLists);

        nvAssert(pSurfObj);  // we should get a surface mapping back.

        if (!pSurfObj) {
            NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETPALETTE, (*ppCommands)->wStateCount, 0);
            dbgTracePop();
            DDENDTICK(SURF7_SETPALETTE);
            return DDERR_INVALIDPARAMS;
        }

        CSimpleSurface* pNvPalette;

        // if the palette object didn't already exists create and add it to the list
        // there is a problem here in that I'm not sure how to detach a palette once its been
        // attached.  The application says to set it to NULL -- but I receive palette handles here
        // so detaching a palette may be a bit problematic
        if (pPalObj == NULL) {

#ifndef WINNT
            //this is a hack cause I don't have a ddslcl to get a pid from here???
            //I don't get create calls like I do for surfaces and must lazily create
            //the palette surfaces at the last moment.  Make it match the surfaces PID.
            pPalObj = new CNvObject (pSurfObj->getPID());
#else
            pPalObj = new CNvObject (0);
#endif
            assert (pPalObj); // todo - handle properly
            PNV_OBJECT_LIST pNvPL = nvFindObjectList(pContext->dwDDLclID, &global.pNvPaletteLists);
            nvAddObjectToList (pNvPL, pPalObj, pSetPalette->dwPaletteHandle);

            pNvPalette = new CSimpleSurface();

            assert (pNvPalette); // todo - handle properly
            if (!pNvPalette->create (4, 256, 1, 4,                                         // pitch, height, depth, BPP
                                     CSimpleSurface::HEAP_VID | CSimpleSurface::HEAP_AGP,  // Allowed Heaps
                                     CSimpleSurface::HEAP_VID,                             // Preferred heaps
                                     CSimpleSurface::ALLOCATE_SIMPLE
#ifdef CAPTURE
                                    ,CAPTURE_SURFACE_KIND_PALETTE
#endif
                                     )) {
                pPalObj->setObject(CNvObject::NVOBJ_PALETTE, pNvPalette);

                // if failed to allocate a surface move the buffer ptr and return an error
                NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETPALETTE, (*ppCommands)->wStateCount, 0);
                dbgTracePop();
                DDENDTICK(SURF7_SETPALETTE);
                return (DDERR_NOPALETTEATTACHED);
            }

            if (pSetPalette->dwPaletteFlags & DDRAWIPAL_ALPHA) {
                pNvPalette->tagHasAlpha();
            }

            // wrap the new surface(palette) with the nvobject(pPalObj)
            pPalObj->setObject(CNvObject::NVOBJ_PALETTE, pNvPalette);
        }

        else {

            pNvPalette = pPalObj->getPaletteSurface();

        }

        // bind the created palette to the texture.
        CTexture *pTex = pSurfObj->getTexture();

        if (pTex) {
            if (pTex->isPalettized()) {
                if (pTex->getBPP() == 1) { // 1 byte -- 8bit palette
                    pSurfObj->setPalette (pPalObj);
                    DPF_LEVEL (NVDBG_LEVEL_PALETTE, "Palette attach succeeded: surfHandle=%x palHandle=%x (%08x)\n",
                               pSetPalette->dwSurfaceHandle,
                               pSetPalette->dwPaletteHandle,
                               pPalObj->getPaletteSurface()->getAddress());
                }
                else{
                    DPF ("nvDP2SetPalette: Non 8-bit palettes not yet supported");
                    hr = DDERR_NOT8BITCOLOR;
                }
                if (pNvPalette->hasAlpha()) {
                    pTex->tagHasAlpha();
                }
                else {
                    pTex->tagHasNoAlpha();
                }
            }
            else {
                DPF_LEVEL (NVDBG_LEVEL_PALETTE, "Surface not palettized: pnvobj=%08x surfhandle=%d palhandle=%d\n",
                           pSurfObj, pSetPalette->dwSurfaceHandle, pSetPalette->dwPaletteHandle);
                hr = DDERR_NOTPALETTIZED;
            }
        }

        else {
            DPF_LEVEL (NVDBG_LEVEL_PALETTE, "ignoring request to attach palette to nontexture. pnvobj=%08x surfhandle=%d palhandle=%d\n",
                       pSurfObj, pSetPalette->dwSurfaceHandle, pSetPalette->dwPaletteHandle);
        }

    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETPALETTE, (*ppCommands)->wStateCount, 0);

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    DDENDTICK(SURF7_SETPALETTE);
    return (hr);
}

//---------------------------------------------------------------------------

HRESULT nvDP2UpdatePalette(NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    DWORD  hr     = DD_OK;

    DDSTARTTICK(SURF7_UPDATEPALETTE);
    dbgTracePush ("nvDP2UpdatePalette");

    // check to see if the current streams/rendertarget/Z/textures need to be unlocked
    if(pContext->dwHWUnlockAllPending){ //HMH
        nvHWUnlockTextures (pContext);
        // need to mark all streams as in use by hardware...
        nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
        pContext->dwHWUnlockAllPending=FALSE;
    }

    LPD3DHAL_DP2UPDATEPALETTE pUpdatePalette = (LPD3DHAL_DP2UPDATEPALETTE)lpPrim;

    // on DX7 we get spurious and calls to this function with totally bogus data. just ignore them
    if (global.dwDXRuntimeVersion >= 0x0800) {


        CNvObject *pPalObj = nvGetObjectFromHandle (pContext->dwDDLclID, pUpdatePalette->dwPaletteHandle, &global.pNvPaletteLists);

        //we should never see a null palette object because it should have been created before
        //ANY updates to a palette would occur
        assert(pPalObj);

        // To handle the case where this gets called after SetPalette that recieved an invalid
        // surface object.  This causes a Palette object to not be created, which would make the
        // line after this conditional GPF.
        if (!pPalObj) {
            dbgTracePop();
            NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2UPDATEPALETTE, (*ppCommands)->wStateCount, (DWORD)pUpdatePalette->wNumEntries * sizeof(PALETTEENTRY));
            DDENDTICK(SURF7_UPDATEPALETTE);
            return DDERR_INVALIDPARAMS;
        }

        CSimpleSurface* pNvPalette = (CSimpleSurface *)pPalObj->getPaletteSurface();

        if (!pNvPalette->getAddress()) {
            if (!pNvPalette->create (4, 256, 1, 4,                                         // pitch, height, depth, BPP
                                     CSimpleSurface::HEAP_VID | CSimpleSurface::HEAP_AGP,  // Allowed Heaps
                                     CSimpleSurface::HEAP_VID,                             // Preferred heaps
                                     CSimpleSurface::ALLOCATE_SIMPLE
#ifdef CAPTURE
                                    ,CAPTURE_SURFACE_KIND_PALETTE
#endif
                                     )) {
                dbgTracePop();
                NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2UPDATEPALETTE, (*ppCommands)->wStateCount, (DWORD)pUpdatePalette->wNumEntries*sizeof(PALETTEENTRY));
                DDENDTICK(SURF7_UPDATEPALETTE);
                return DDERR_INVALIDPARAMS;
            }
        }

        // we should now have a valid pNvSurface to work with
        nvAssert(pNvPalette);

        // COPY THE PALETTE INFORMATION

        // an assumption is made here that the palette entry size is the same as the 4BBP size
        // according to HW docs I have, we ALWAYS load a 256 entry (the only size we support) and it is
        // pulled from memory as a 1Kb block.  So even if we support a 'smaller' palette the HW will
        // still pull 1Kb in so allocate space for it -- don't want memory faults
        BYTE *pDest, *pSrc;
        pDest = LPBYTE(pNvPalette->getAddress() + sizeof(PALETTEENTRY) * pUpdatePalette->wStartIndex);
        pSrc  = ((LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND) +
                         (((*ppCommands)->wStateCount) * sizeof(D3DHAL_DP2UPDATEPALETTE)) +
                         //offset by the start index
                         sizeof(PALETTEENTRY) * pUpdatePalette->wStartIndex);

        // this is an 8 bit palette we should never copy more than 256 entries
        assert(pUpdatePalette->wStartIndex + pUpdatePalette->wNumEntries <= 256);

        DDLOCKINDEX(NVSTAT_LOCK_PALETTE);
        pNvPalette->cpuLock(CSimpleSurface::LOCK_NORMAL);

        if (pNvPalette->hasAlpha()) {
            for (int i = 0; i < pUpdatePalette->wNumEntries; i++) {
                pDest[0] = pSrc[2];
                pDest[1] = pSrc[1];
                pDest[2] = pSrc[0];
                pDest[3] = pSrc[3];
                pSrc += 4; pDest += 4;
            }
        } else {
            for (int i = 0; i < pUpdatePalette->wNumEntries; i++) {
                pDest[0] = pSrc[2];
                pDest[1] = pSrc[1];
                pDest[2] = pSrc[0];
                pDest[3] = 0xFF; // stomp alpha
                pSrc += 4; pDest += 4;
            }
        }
        pNvPalette->cpuUnlock();

#if (NVARCH >= 0x010)
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TEXTURE_STATE;
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TEXTURE_STATE;
#endif
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2UPDATEPALETTE, (*ppCommands)->wStateCount, (DWORD)pUpdatePalette->wNumEntries * sizeof(PALETTEENTRY));

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    DDENDTICK(SURF7_UPDATEPALETTE);
    return (hr);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetZRange (NV_DP2FUNCTION_ARGLIST)
{
    LPD3DHAL_DP2ZRANGE pZRange;

    DDSTARTTICK(DEVICE7_SETZRANGE);
    dbgTracePush ("nvDP2SetZRange");

    // Keep only the last viewport notification
    pZRange = (D3DHAL_DP2ZRANGE *)((*ppCommands) + 1) + ((*ppCommands)->wStateCount - 1);

    if ((pContext->surfaceViewport.dvMinZ != pZRange->dvMinZ) ||
        (pContext->surfaceViewport.dvMaxZ != pZRange->dvMaxZ)) {
        // Update T&L viewport state
        pContext->surfaceViewport.dvMinZ = pZRange->dvMinZ;
        pContext->surfaceViewport.dvMaxZ = pZRange->dvMaxZ;
    }

    assert(pContext->surfaceViewport.dvMinZ >=0 && pContext->surfaceViewport.dvMinZ <= 1.0);
    assert(pContext->surfaceViewport.dvMaxZ >=0 && pContext->surfaceViewport.dvMaxZ <= 1.0);
    assert(pContext->surfaceViewport.dvMinZ < pContext->surfaceViewport.dvMaxZ);

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TRANSFORM;
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM;
#endif

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2ZRANGE, (*ppCommands)->wStateCount, 0);

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    DDENDTICK(DEVICE7_SETZRANGE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetMaterial (NV_DP2FUNCTION_ARGLIST)
{
    LPD3DHAL_DP2SETMATERIAL pSetMat;

    DDSTARTTICK(DEVICE7_SETMATERIAL);
    dbgTracePush ("nvDP2SetMaterial");

    // Keep only the last material notification
    pSetMat = (D3DHAL_DP2SETMATERIAL *)((*ppCommands) + 1) + ((*ppCommands)->wStateCount - 1);

    if (memcmp (&(pContext->Material), pSetMat, sizeof(D3DMATERIAL7))) {
        pContext->Material = *(D3DMATERIAL7 *)pSetMat;
#if (NVARCH >= 0x010)
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_LIGHTS;
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_LIGHTS;
#endif
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETMATERIAL, (*ppCommands)->wStateCount, 0);

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    DDENDTICK(DEVICE7_SETMATERIAL);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetLight (NV_DP2FUNCTION_ARGLIST)
{
#if (NVARCH >= 0x010)

    HRESULT hr = D3D_OK;
    WORD wNumSetLight = (*ppCommands)->wStateCount;
    LPD3DHAL_DP2SETLIGHT pSetLight;
    D3DLIGHT7 *pLightData;
    DWORD dwTotalStride;
    int i;

    DDSTARTTICK(DEVICE7_SETLIGHT);
    dbgTracePush ("nvDP2SetLight");

    dwTotalStride = sizeof(D3DHAL_DP2COMMAND);
    pSetLight = (LPD3DHAL_DP2SETLIGHT)((*ppCommands) + 1);
    pLightData = NULL;

    for (i=0; i < wNumSetLight; i++)
    {
        DWORD dwStride = sizeof(D3DHAL_DP2SETLIGHT);
        DWORD dwIndex = pSetLight->dwIndex;

        // Assert that create was not called here
        assert (dwIndex < pContext->dwLightArraySize);

        switch (SETLIGHT_DATATYPE(pSetLight)) {
            case D3DHAL_SETLIGHT_ENABLE:
                nvLightEnable(&(pContext->pLightArray[dwIndex]),
                              &(pContext->lighting.pActiveLights));
                break;
            case D3DHAL_SETLIGHT_DISABLE:
                nvLightDisable(&(pContext->pLightArray[dwIndex]),
                               &(pContext->lighting.pActiveLights));
                break;
            case D3DHAL_SETLIGHT_DATA:
                pLightData = (D3DLIGHT7 *)((LPBYTE)pSetLight + dwStride);
                dwStride += sizeof(D3DLIGHT7);
                hr |= nvLightSetLight (&(pContext->pLightArray[pSetLight->dwIndex]), pLightData);
                break;
            default:
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "Unknown SetLight command");
                hr |= DDERR_INVALIDPARAMS;
                break;
        }

//        if (hr != D3D_OK) return(hr); //can't do this because it hoses DP2 processing

        dwTotalStride += dwStride;
        // Update the command buffer pointer
        pSetLight = (D3DHAL_DP2SETLIGHT *)((LPBYTE)pSetLight + dwStride);
    }

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_LIGHTS;
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_LIGHTS;
#endif

    if (hr != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvDP2SetLight failed");
    (*ppCommands) = (LPD3DHAL_DP2COMMAND)((LPBYTE)(*ppCommands) + dwTotalStride);

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    DDENDTICK(DEVICE7_SETLIGHT);
    return (hr);

#else  // !(NVARCH >= 0x010)

    DPF ("got unexpected call to SetLight in driver with no T&L support");
    DDENDTICK(DEVICE7_SETLIGHT);
    return (~D3D_OK);

#endif  // !(NVARCH >= 0x010))

}

//---------------------------------------------------------------------------

HRESULT nvDP2CreateLight (NV_DP2FUNCTION_ARGLIST)
{

#if (NVARCH >= 0x010)

    WORD wNumCreateLight = (*ppCommands)->wStateCount;
    LPD3DHAL_DP2CREATELIGHT pCreateLight = (LPD3DHAL_DP2CREATELIGHT)((*ppCommands) + 1);
    HRESULT hr = D3D_OK;

    DDSTARTTICK(DEVICE7_CREATELIGHT);
    dbgTracePush ("nvDP2CreateLight");

    for (DWORD i=0; i<wNumCreateLight; i++, pCreateLight++)
    {
        // If the index is not already allocated, grow the light array
        // by REF_LIGHTARRAY_GROWTH_SIZE
        if (pCreateLight->dwIndex >= pContext->dwLightArraySize)
        {
            hr = nvGrowLightArray (pContext, pCreateLight->dwIndex);
            if (hr != D3D_OK) return (hr);
        }
    }

    if (hr != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvDP2CreateLight failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2CREATELIGHT, (*ppCommands)->wStateCount, 0);

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    DDSTARTTICK(DEVICE7_CREATELIGHT);
    return (hr);

#else  // !(NVARCH >= 0x010)

    DPF ("got unexpected call to CreateLight in driver with no T&L support");
    DDSTARTTICK(DEVICE7_CREATELIGHT);
    return (~D3D_OK);

#endif  // (NVARCH >= 0x010))

}

//---------------------------------------------------------------------------

HRESULT nvDP2SetTransform (NV_DP2FUNCTION_ARGLIST)
{

#if (NVARCH >= 0x010)

    LPD3DHAL_DP2SETTRANSFORM   pSetXfrm;
    WORD                       wNumXfrms;
    D3DTRANSFORMSTATETYPE      xfrmType;
    D3DMATRIX                  *pMat;
    BOOL                       bSetIdentity;
    DWORD                      dwxfrmType, i, dwStage;

    DDSTARTTICK(DEVICE7_SETTRANSFORM);
    dbgTracePush ("nvDP2SetTransform");

    pSetXfrm = (LPD3DHAL_DP2SETTRANSFORM)((*ppCommands) + 1);
    wNumXfrms = (*ppCommands)->wStateCount;

    for (i=0; i < (int) wNumXfrms; i++) {

        xfrmType = pSetXfrm->xfrmType;
        pMat     = &pSetXfrm->matrix;
        // BUGBUG is there a define for 0x80000000?
        bSetIdentity = (xfrmType & 0x80000000) != 0;
        dwxfrmType   = (DWORD)xfrmType & (~0x80000000);

        switch (dwxfrmType) {

            case D3DTRANSFORMSTATE_WORLD_DX7:
            case D3DTS_WORLD:
#if CHECK_DUP_MATRIX
                if  (!memcmp(&(pContext->xfmWorld[0]), pMat, sizeof(D3DMATRIX)))
                    break;
#endif
                nvMemCopy (&(pContext->xfmWorld[0]), pMat, sizeof(D3DMATRIX));
#if (NVARCH >= 0x010)
                pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_TRANSFORM |
                                                   CELSIUS_DIRTY_LIGHTS);
                pContext->hwState.dwDirtyFlags |= (KELVIN_DIRTY_TRANSFORM |
                                                   KELVIN_DIRTY_LIGHTS);
#endif
                break;

            case D3DTRANSFORMSTATE_WORLD1_DX7:
            case D3DTS_WORLD1:
#if CHECK_DUP_MATRIX
                if  (!memcmp(&(pContext->xfmWorld[1]), pMat, sizeof(D3DMATRIX)))
                    break;
#endif
                nvMemCopy (&(pContext->xfmWorld[1]), pMat, sizeof(D3DMATRIX));
#if (NVARCH >= 0x010)
                pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TRANSFORM;
                pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM;
#endif
                break;

            case D3DTRANSFORMSTATE_WORLD2_DX7:
            case D3DTS_WORLD2:
#if CHECK_DUP_MATRIX
                if  (!memcmp(&(pContext->xfmWorld[2]), pMat, sizeof(D3DMATRIX)))
                    break;
#endif
                nvMemCopy (&(pContext->xfmWorld[2]), pMat, sizeof(D3DMATRIX));
#if (NVARCH >= 0x020)
                pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM;
#endif
                break;

            case D3DTRANSFORMSTATE_WORLD3_DX7:
            case D3DTS_WORLD3:
#if CHECK_DUP_MATRIX
                if  (!memcmp(&(pContext->xfmWorld[3]), pMat, sizeof(D3DMATRIX)))
                    break;
#endif
                nvMemCopy (&(pContext->xfmWorld[3]), pMat, sizeof(D3DMATRIX));
#if (NVARCH >= 0x020)
                pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM;
#endif
                break;

            case D3DTRANSFORMSTATE_VIEW:
#if CHECK_DUP_MATRIX
                if  (!memcmp(&(pContext->xfmView), pMat, sizeof(D3DMATRIX)))
                    break;
#endif
                nvMemCopy (&(pContext->xfmView), pMat, sizeof(D3DMATRIX));
#if (NVARCH >= 0x010)
#if STATE_OPTIMIZE
                pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_TRANSFORM |
                                                   CELSIUS_DIRTY_LIGHTS);
                pContext->hwState.dwDirtyFlags |= (KELVIN_DIRTY_TRANSFORM |
                                                   KELVIN_DIRTY_LIGHTS);

                // Only set texture state if we're doing TEXGEN -- D3DTSS_TCI_CAMERASPACENORMAL
                // Setting texture state is a HEAVY operation, especially on Kelvin

                if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_TEXSTATENEEDSINVMV)
                    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TEXTURE_STATE;

                if (pContext->hwState.dwStateFlags & KELVIN_FLAG_TEXSTATENEEDSINVMV)
                    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TEXTURE_STATE;

#else
                pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_TEXTURE_STATE |
                                                   CELSIUS_DIRTY_TRANSFORM |
                                                   CELSIUS_DIRTY_LIGHTS);
                pContext->hwState.dwDirtyFlags |= (KELVIN_DIRTY_TEXTURE_STATE |
                                                   KELVIN_DIRTY_TRANSFORM |
                                                   KELVIN_DIRTY_LIGHTS);
#endif
#endif
                break;

            case D3DTRANSFORMSTATE_PROJECTION:
#if CHECK_DUP_MATRIX
                if  (!memcmp(&(pContext->xfmProj), pMat, sizeof(D3DMATRIX)))
                    break;
#endif
#ifdef  STEREO_SUPPORT
                if (STEREO_ENABLED)
                {
                    pMat = SetStereoProjection(pMat, pContext);
                }
#endif  //STEREO_SUPPORT
                nvMemCopy (&(pContext->xfmProj), pMat, sizeof(D3DMATRIX));
#if (NVARCH >= 0x010)
                pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TRANSFORM;
                pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM;
#endif
                if (pContext->dwDXAppVersion == 0x800) {
                    // Due to a bug in the runtime, SetWRange is not called for pure HAL
                    // apps in DX8. As a result, we have to determine if fog source is Z
                    // or W from the projection matrix. The pure HAL did not exit for DX7
                    // or before, and the bug is fixed in DX8.1, so we only need to worry
                    // about DX8.
                    if ((*(DWORD *)&(pMat->_14) == 0) &&
                        (*(DWORD *)&(pMat->_24) == 0) &&
                        (*(DWORD *)&(pMat->_34) == 0) &&
                        (*(DWORD *)&(pMat->_44) == FP_ONE_BITS)) {
                        pContext->hwState.dwFogSource = CHardwareState::FOG_SOURCE_Z;
                    }
                    else {
                        pContext->hwState.dwFogSource = CHardwareState::FOG_SOURCE_W;
                    }
                    pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_SPECFOG_COMBINER | KELVIN_DIRTY_VERTEX_SHADER);
                }

                // set the WNear and WFar values from the supplied projection
                if ((pMat->_33 == pMat->_34) || (pMat->_33 == 0.0f))
                {
                    DPF("Cannot compute WNear and WFar from the supplied projection");
                    DPF("Setting wNear to 0.0 and wFar to 1.0");
                    nvSetWRange(pContext, 0.0, 1.0);
                } else {
                    nvSetWRange(pContext,
                                pMat->_44 - pMat->_43 / pMat->_33 * pMat->_34,
                                (pMat->_44 - pMat->_43) / (pMat->_33 - pMat->_34) * pMat->_34 + pMat->_44);
                }

                break;

            case D3DTRANSFORMSTATE_TEXTURE0:
            case D3DTRANSFORMSTATE_TEXTURE1:
            case D3DTRANSFORMSTATE_TEXTURE2:
            case D3DTRANSFORMSTATE_TEXTURE3:
            case D3DTRANSFORMSTATE_TEXTURE4:
            case D3DTRANSFORMSTATE_TEXTURE5:
            case D3DTRANSFORMSTATE_TEXTURE6:
            case D3DTRANSFORMSTATE_TEXTURE7:
                dwStage = xfrmType - D3DTRANSFORMSTATE_TEXTURE0;
                if (bSetIdentity) {
#if CHECK_DUP_MATRIX
                if  (!memcmp(&(pContext->tssState[dwStage].mTexTransformMatrix), &matrixIdent, sizeof(D3DMATRIX)))
                    break;
#endif
                    nvMemCopy (&pContext->tssState[dwStage].mTexTransformMatrix, &matrixIdent, sizeof(D3DMATRIX));
                }
                else {
#if CHECK_DUP_MATRIX
                if  (!memcmp(&(pContext->tssState[dwStage].mTexTransformMatrix), pMat, sizeof(D3DMATRIX)))
                    break;
#endif
                    nvMemCopy (&pContext->tssState[dwStage].mTexTransformMatrix, pMat, sizeof(D3DMATRIX));
                }
#if (NVARCH >= 0x010)
                pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TEXTURE_TRANSFORM;
                pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TEXTURE_TRANSFORM;
#endif
                break;

            default:
                DPF_LEVEL (NVDBG_LEVEL_INFO,"Ignoring unknown transform type in SetXfrm");
                break;

        }  // switch (dwxfrmType)

        pSetXfrm++;

    } // for...

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETTRANSFORM, (*ppCommands)->wStateCount, 0);

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    DDENDTICK(DEVICE7_SETTRANSFORM);
    return (D3D_OK);

#else  // !(NVARCH >= 0x010)

    DPF ("got unexpected call to SetTransform in driver with no T&L support");
    DDENDTICK(DEVICE7_SETTRANSFORM);
    return (~D3D_OK);

#endif  // (NVARCH >= 0x010))

}

//---------------------------------------------------------------------------

HRESULT nvDP2Extension (NV_DP2FUNCTION_ARGLIST)
{
    // nada for now
    DPF ("DP2Extension called. doing nothing");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2EXT, (*ppCommands)->wStateCount, 0);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// wrapper routine to interface the DX7 DP2 token TEXBLT with the original Blit32 callback
// note that one DP2_TEXBLT command can handle multiple blits and/or entire mipmap chains

HRESULT nvDP2TexBlt (NV_DP2FUNCTION_ARGLIST)
{
    DDSTARTTICK(SURF7_TEXTUREBLIT);
    dbgTracePush ("nvDP2TexBlt");

#ifdef NO_DP2_TEX

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2TEXBLT, (*ppCommands)->wStateCount, 0);

    dbgTracePop();
    DDENDTICK(SURF7_TEXTUREBLIT);
    return (D3D_OK);

#endif

    DWORD dwNumBlits = (*ppCommands)->wStateCount;
    LPD3DHAL_DP2TEXBLT pTexBltData = (LPD3DHAL_DP2TEXBLT) ((*ppCommands) + 1);

    //check to see if the current streams/rendertarget/Z/textures need to be unlocked
    if(pContext->dwHWUnlockAllPending){ //HMH
        nvHWUnlockTextures (pContext);
        // need to mark all streams as in use by hardware...
        nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
        pContext->dwHWUnlockAllPending=FALSE;
    }

    for (DWORD i=0; i < dwNumBlits; i++) {
        if (pTexBltData->dwDDDestSurface) {
            CNvObject* pSrcObj = nvGetObjectFromHandle (pContext->dwDDLclID, pTexBltData->dwDDSrcSurface, &global.pNvSurfaceLists);
            CNvObject* pDstObj = nvGetObjectFromHandle (pContext->dwDDLclID, pTexBltData->dwDDDestSurface, &global.pNvSurfaceLists);

            RECTL rectSrc  = pTexBltData->rSrc;
            POINT pointDst = pTexBltData->pDest;
            DWORD dwFlags  = pTexBltData->dwFlags;

            BOOL bRVal = FALSE;

            CTexture* pDstTexture = pDstObj->getTexture();

            // check if this was previously auto-paletized and revert if needed
            BOOL bFullSurface = (!(pointDst.x & pointDst.y)
                             && ((rectSrc.right - rectSrc.left) == (LONG)pDstTexture->getWidth())
                             && ((rectSrc.bottom - rectSrc.top) == (LONG)pDstTexture->getHeight()));

            nvAutoPaletteCheckAndRevert(pDstTexture, bFullSurface);

            BOOL bPalettable = (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_AUTOPALETTE) // requested
                            && !pDstTexture->isReused()             // haven't done this one before
                            && !pDstTexture->isDXT()                // not DXT
                            && !pDstTexture->isCubeMap()            // not a cube map (we get separate blits for each face)
                            && (pDstTexture->getBPP() == 4)         // 32 bit only
                            && (bFullSurface)                       // full surface blit
                            && (pDstTexture->getWidth() >= 32)      // greater than 32x32
                            && (pDstTexture->getHeight() >= 32)
                            // we should be able to handle AGP too, but not multistage AGP & VID together due to hardware limitations
                            // this can be fixed by copying the AGP palette temporariliy to the free memory above the vid palette
                            && (pDstTexture->getSwizzled()->getHeapLocation() == CSimpleSurface::HEAP_VID)
                            && (pSrcObj->getClass() == CNvObject::NVOBJ_SIMPLESURFACE) // source must be sysmem surface
                            && !pContext->bSeenTriangles;   // Don't palettize if we've started 3d rendering.

            if (bPalettable) {
                // try a paletteBlt
                bRVal = nvPaletteBlt(pSrcObj, pDstTexture);
                if (!bRVal) {
                    pDstTexture->tagUnpalettized(); // tag so we don't try palettizing this texture again
                }
            }

            if (!bRVal) {
                // do a normal blit
                while (1) {
                    // the source is either a texture or a simple surface, get its width
                    nvAssert (pSrcObj->getTexture() || pSrcObj->getSimpleSurface());
                    DWORD dwSrcWidth = pSrcObj->getTexture() ? pSrcObj->getTexture()->getWidth()
                        : pSrcObj->getSimpleSurface()->getWidth();

                    // MS requires us to blt only from the matching level onwards (DCT300)
                    if (dwSrcWidth == pDstObj->getTexture()->getWidth()) {
                        D3DBOX boxSrc;

                        boxSrc.Left = rectSrc.left;
                        boxSrc.Right = rectSrc.right;
                        boxSrc.Top = rectSrc.top;
                        boxSrc.Bottom = rectSrc.bottom;
                        boxSrc.Front = 0;
                        boxSrc.Back = 1;

                        DWORD dwRV = nvTextureBltDX7 (pContext, pSrcObj, pDstObj, &boxSrc, pointDst.x, pointDst.y, 0, dwFlags);
                        if (dwRV != DD_OK) {
                            DPF_LEVEL (NVDBG_LEVEL_ERROR,"D3D: nvTexBltDP2 failed");
                            NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2TEXBLT, (*ppCommands)->wStateCount, 0);
                            dbgTracePop();
                            return (DDERR_GENERIC);
                        }
                        pDstObj = pDstObj->getAttachedA();
                    }

                    pSrcObj = pSrcObj->getAttachedA();

                    // terminate if we have reached the end of either mipmap chain
                    if (!pSrcObj || !pDstObj) break;

                    // according to MS, this is the algorithm we're supposed to use
                    // for coming up with the next rectangle. originally i was just doing
                    // a plain >> 1. the ++ changes the degeneration of rects and keeps
                    // them from ever going to zero. (in other words, we always update the
                    // 1x1 mipmap, no matter what sub-rect we started with.)
                    rectSrc.top    >>= 1;
                    rectSrc.left   >>= 1;
                    rectSrc.bottom ++;
                    rectSrc.bottom >>= 1;
                    rectSrc.right  ++;
                    rectSrc.right  >>= 1;

                    pointDst.x     >>= 1;
                    pointDst.y     >>= 1;
                }
            }
        }
#ifdef TEX_MANAGE
        // whith driver managed textures enabled, a NULL dest handle means load src texture into video memory
        else {
            CTexture* pTexture = nvGetObjectFromHandle (pContext->dwDDLclID, pTexBltData->dwDDSrcSurface)->getTexture();
            nvAssert (pTexture);
            if (pTexture->isManaged()) {
                // if the texture is in system memory, we have to bring it into video or AGP
                if (pTexture->getSwizzled()->getHeapLocation() == CSimpleSurface::HEAP_SYS) {
                    BOOL bSuccess = nvTexManageFetch (pContext, pTexture);
                    nvAssert (bSuccess);
                }
                else {
                    // just make a note of the fact that we've touched this texture
                    nvTexManageTouch (pContext, pTexture->getBaseTexture());
                }
            }
        }
#endif

        // next blit
        pTexBltData++;
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2TEXBLT, (*ppCommands)->wStateCount, 0);

    dbgFlushType (NVDBG_FLUSH_2D);

    dbgTracePop();
    DDENDTICK(SURF7_TEXTUREBLIT);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2StateSet (NV_DP2FUNCTION_ARGLIST)
{
    HRESULT retval;
    LPD3DHAL_DP2STATESET pStateSetOp = (LPD3DHAL_DP2STATESET)((*ppCommands) + 1);

    DDSTARTTICK(DEVICE7_STATESET);
    dbgTracePush ("nvDP2StateSet");

#ifdef DEBUG
    static int iNestDepth = 0;
#endif

    switch (pStateSetOp->dwOperation) {
        case D3DHAL_STATESETBEGIN:
#ifdef DEBUG
            iNestDepth++;
            DPF_LEVEL(NVDBG_LEVEL_INFO,"nvDP2StateSet::stateSetBegin (%d)",iNestDepth);
            nvAssert (iNestDepth == 1); // state blocks cannot be nested
#endif
            retval = stateSetBegin (pContext, pStateSetOp->dwParam);
            break;
        case D3DHAL_STATESETEND:
#ifdef DEBUG
            DPF_LEVEL (NVDBG_LEVEL_INFO,"nvDP2StateSet::stateSetEnd (%d)",iNestDepth);
            iNestDepth--;
#endif
            retval = stateSetEnd (pContext);
            break;
        case D3DHAL_STATESETDELETE:
            DPF_LEVEL (NVDBG_LEVEL_INFO,"nvDP2StateSet::stateSetDelete");
            retval = stateSetDelete (pContext, pStateSetOp->dwParam);
            break;
        case D3DHAL_STATESETEXECUTE:
            DPF_LEVEL (NVDBG_LEVEL_INFO,"nvDP2StateSet::stateSetExecute");
            retval = stateSetExecute (pContext, pStateSetOp->dwParam);
            break;
        case D3DHAL_STATESETCAPTURE:
            DPF_LEVEL (NVDBG_LEVEL_INFO,"nvDP2StateSet::stateSetCapture");
            retval = stateSetCapture (pContext, pStateSetOp->dwParam);
            break;
        case D3DHAL_STATESETCREATE:
            DPF_LEVEL (NVDBG_LEVEL_INFO,"nvDP2StateSet::stateSetCreate");
            retval = stateSetCreate (pContext, pStateSetOp->dwParam, pStateSetOp->sbType);
            break;
        default:
            DPF ("unhandled / unknown stateset operation");
            dbgD3DError();
            retval = DDERR_INVALIDPARAMS;
            break;
    }

    if (retval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: DP2OP_STATESET failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2STATESET, (*ppCommands)->wStateCount, 0);

    dbgTracePop();
    DDENDTICK(DEVICE7_STATESET);
    return (retval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetPriority (NV_DP2FUNCTION_ARGLIST)
{
    LPD3DHAL_DP2SETPRIORITY   pDp2PriorityData;
    DWORD                     i;

    DDSTARTTICK(SURF7_SETPRIORITY);
    dbgTracePush ("nvDP2SetPriority");

    pDp2PriorityData = (LPD3DHAL_DP2SETPRIORITY)((*ppCommands)+1);

    for (i=0; i<(*ppCommands)->wStateCount; i++) {
        CNvObject *pObj = nvGetObjectFromHandle (pContext->dwDDLclID, pDp2PriorityData->dwDDSurface, &global.pNvSurfaceLists);
        CTexture  *pTexture = pObj->getTexture();
        assert (pTexture);
        pTexture->setPriority (pDp2PriorityData->dwPriority);
        pDp2PriorityData++;
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETPRIORITY, (*ppCommands)->wStateCount, 0);

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    DDENDTICK(SURF7_SETPRIORITY);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetRenderTarget (NV_DP2FUNCTION_ARGLIST)
{
    LPD3DHAL_DP2SETRENDERTARGET pSRTData;
    HRESULT hr;

    DDSTARTTICK(DEVICE7_SETRENDERTARGET);
    dbgTracePush ("nvDP2SetRenderTarget");

    // Get new data by ignoring all but the last structure
    pSRTData = (LPD3DHAL_DP2SETRENDERTARGET)((*ppCommands) + 1) + ((*ppCommands)->wStateCount - 1);

    if(pContext->dwHWUnlockAllPending){ //HMH
        nvHWUnlockTextures (pContext);
        // need to mark all streams as in use by hardware...
        nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
        pContext->dwHWUnlockAllPending=FALSE;
    }

    hr = nvSetRenderTargetDX7 (pContext, pSRTData->hRenderTarget, pSRTData->hZBuffer);

    //AA Fix -- when we do a Flip for AA we traverse the pcontext list to execute the flip on the
    //proper super buffers.  Sometimes we have apps that have multiple pcontexts pointing to
    //the rendertargets.  When we do the lookup in the flip -- we get the first one in the list,
    //rather than 'current' one.  In an attempt to make them match anytime the SRT target is
    //set move the pcontext to the head of that list.  Ideally we should only ever do this once.
    PNVD3DCONTEXT pHead = (PNVD3DCONTEXT)getDC()->dwContextListHead;
    if (pContext != pHead) {
        pContext->pContextPrev->pContextNext = pContext->pContextNext;
        if (pContext->pContextNext) {
            pContext->pContextNext->pContextPrev = pContext->pContextPrev;
        }
        pContext->pContextPrev = NULL;
        pContext->pContextNext = pHead;
        pHead->pContextPrev = pContext;

        (PNVD3DCONTEXT)getDC()->dwContextListHead =pContext;
    }

    if (hr != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvDP2SetRenderTarget failed");
    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETRENDERTARGET, (*ppCommands)->wStateCount, 0);

    dbgFlushType (NVDBG_FLUSH_STATE);

    dbgTracePop();
    DDENDTICK(DEVICE7_SETRENDERTARGET);
    return (hr);
}

//---------------------------------------------------------------------------

// clear via DP2 token, valid post-DX7.
// basically this is just a wrapper for the old clear routines,
// serving to translate the new data structure to the old
// one before passing it on down.

HRESULT nvDP2Clear (NV_DP2FUNCTION_ARGLIST)
{
    LPD3DHAL_DP2CLEAR pDp2ClearData = (LPD3DHAL_DP2CLEAR)((*ppCommands)+1);
    D3DHAL_CLEAR2DATA c2d;
    D3DRECT rect;

    dbgTracePush ("nvDP2Clear");

    c2d.dwhContext = (DWORD) pContext;
    c2d.dwFlags = pDp2ClearData->dwFlags;
    c2d.dwFillColor = pDp2ClearData->dwFillColor;

#ifdef  NV_NULL_DRIVER_DYNAMIC_
    c2d.dwFlags |= D3DCLEAR_TARGET;
    c2d.dwFillColor = 0x818181; //Grey - to be able to see garbage
#endif //NV_NULL_DRIVER_DYNAMIC

    c2d.dvFillDepth = pDp2ClearData->dvFillDepth;
    c2d.dwFillStencil = pDp2ClearData->dwFillStencil;

    c2d.dwNumRects = (*ppCommands)->wStateCount;

    // only pay attention to this flag if there is no list (WHQL clear test)
    if ((c2d.dwNumRects == 0) && (c2d.dwFlags & D3DCLEAR_COMPUTERECTS)) {
        // assume we want to clear the entire rendertarget - this may need to be extended
        c2d.dwNumRects = 1;
        c2d.lpRects = &rect;
        rect.x1 = pContext->surfaceViewport.clipHorizontal.wX;
        rect.y1 = pContext->surfaceViewport.clipVertical.wY;
        rect.x2 = pContext->surfaceViewport.clipHorizontal.wWidth + rect.x1;
        rect.y2 = pContext->surfaceViewport.clipVertical.wHeight + rect.y1;
    } else {
        // yet another gratuitous change by MS requires translation.
        // for now we can get away with a cast because these two structures
        // happen to order things the same way. i hesitate to attribute
        // this to any intelligence on MS's part. if they ever whimsically
        // re-order the elements in either structure, we'll be screwed and
        // have to translate rectangle by rectangle, vertex by vertex.
        // compute clear rectangle if required
        c2d.lpRects = (LPD3DRECT) pDp2ClearData->Rects;
    }

    nvClear2 (&c2d);

    // put the rendering surface back to where it belongs since the DP2
    // clear may be interleaved with DP2 triangle rendering operations
    // is this really necessary?
    // nvSetD3DSurfaceState(pContext);

    if (c2d.ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvDP2Clear failed");
    NEXTINSTRUCTION ((*ppCommands), RECT, (*ppCommands)->wStateCount-1, sizeof(D3DHAL_DP2CLEAR));

    dbgFlushType (NVDBG_FLUSH_2D);

    dbgTracePop();
    return (c2d.ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetTexLOD (NV_DP2FUNCTION_ARGLIST)
{
    LPD3DHAL_DP2SETTEXLOD pDp2TexLODData = (LPD3DHAL_DP2SETTEXLOD)((*ppCommands)+1);

    dbgTracePush ("nvDP2SetTexLOD");

    // nada for now
    DPF ("DP2SetTexLOD called. doing nothing");

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETTEXLOD, (*ppCommands)->wStateCount, 0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetClipPlane (NV_DP2FUNCTION_ARGLIST)
{
    LPD3DHAL_DP2SETCLIPPLANE pDp2ClipPlaneData;
    DWORD                    count, index;
    D3DVALUE                 *pClipPlane;

    DDSTARTTICK(DEVICE7_SETCLIPPLANE);
    dbgTracePush ("nvDP2SetClipPlane");

    pDp2ClipPlaneData = (LPD3DHAL_DP2SETCLIPPLANE)((*ppCommands)+1);

    for (count=0; count<(*ppCommands)->wStateCount; count++) {

        index = pDp2ClipPlaneData->dwIndex;
        if (index > D3DMAXUSERCLIPPLANES) {
            DPF ("clipplane index %d exceeded D3DMAXUSERCLIPPLANES in nvDP2SetClipPlane", index);
            pDp2ClipPlaneData ++;
            continue;
        }

        pClipPlane = pContext->ppClipPlane[index];

        pClipPlane[0] = pDp2ClipPlaneData->plane[0];
        pClipPlane[1] = pDp2ClipPlaneData->plane[1];
        pClipPlane[2] = pDp2ClipPlaneData->plane[2];
        pClipPlane[3] = pDp2ClipPlaneData->plane[3];

        pDp2ClipPlaneData ++;

    }

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TEXTURE_STATE;
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TEXTURE_STATE;
#endif  // NVARCH >= 0x010

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETCLIPPLANE, (*ppCommands)->wStateCount, 0);

    dbgTracePop();
    DDENDTICK(DEVICE7_SETCLIPPLANE);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2StreamEnd (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2StreamEnd");
    *ppCommands = LPD3DHAL_DP2COMMAND(pCommandBufferEnd);
    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CreateVertexShader (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2CREATEVERTEXSHADER *lpCreateVShader;
    DWORD dwSize = 0;

    dbgTracePush ("nvDP2CreateVertexShader");

    for (DWORD count = 0; count < (*ppCommands)->wStateCount; count++)
    {
        lpCreateVShader = (D3DHAL_DP2CREATEVERTEXSHADER *)(lpPrim);

        // We should not be getting CreateVertexShader for FVFs.
        nvAssert( lpCreateVShader->dwHandle & 0x1 );

        if (lpCreateVShader->dwHandle & 0x1)
        {
    #ifndef WINNT
            //this is a hack cause I don't have a ddslcl to get a pid from here???
            //I don't get create calls like I do for surfaces and must lazily create
            //the shaders at the last moment.  Make it match the context's PID.
            CNvObject *pObj = new CNvObject (pContext->pid);
    #else
            CNvObject *pObj = new CNvObject (0);
    #endif
            CVertexShader *pVertexShader = new CVertexShader();

            // code vertex shader
            pVertexShader->create (pContext,
                                   lpCreateVShader->dwHandle,
                                   lpCreateVShader->dwDeclSize,
                                   lpCreateVShader->dwCodeSize,
                                   (DWORD *)((LPBYTE)lpCreateVShader + sizeof(D3DHAL_DP2CREATEVERTEXSHADER)),
                                   (DWORD *)((LPBYTE)lpCreateVShader + lpCreateVShader->dwDeclSize + sizeof(D3DHAL_DP2CREATEVERTEXSHADER)));

            pObj->setObject(CNvObject::NVOBJ_VERTEXSHADER, pVertexShader);
            PNV_OBJECT_LIST pNvList = nvFindObjectList(pContext->dwDDLclID, &global.pNvVShaderLists);
            nvAddObjectToList(pNvList, pObj, lpCreateVShader->dwHandle);
        }

        lpPrim += sizeof(D3DHAL_DP2CREATEVERTEXSHADER) +
                  lpCreateVShader->dwDeclSize +
                  lpCreateVShader->dwCodeSize;
        dwSize += lpCreateVShader->dwDeclSize +
                  lpCreateVShader->dwCodeSize;
    }

#ifdef KPFS
    pDriverData->nvKelvinPM.flush();
#endif
    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2CREATEVERTEXSHADER, (*ppCommands)->wStateCount, dwSize);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2DeleteVertexShader (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2VERTEXSHADER *lpDeleteVShader = (D3DHAL_DP2VERTEXSHADER *)(lpPrim);

    dbgTracePush ("nvDP2DeleteVertexShader");

    for (DWORD count = 0; count < (*ppCommands)->wStateCount; count++)
    {
        CNvObject *pNvObj;

        // We should not be getting DeleteVertexShader for FVFs.
        nvAssert( lpDeleteVShader->dwHandle & 0x1 );

        if( lpDeleteVShader->dwHandle & 0x1 )
        {
            pNvObj = nvGetObjectFromHandle(pContext->dwDDLclID, lpDeleteVShader->dwHandle, &global.pNvVShaderLists);

            // check if this is the active vertex shader
            if (pContext->pCurrentVShader == pNvObj->getVertexShader())
            {
                // restore the default vertex shader
                CNvObject *pNvObj = nvGetObjectFromHandle(pContext->dwDDLclID, 0, &global.pNvVShaderLists);
                pContext->pCurrentVShader = pNvObj->getVertexShader();
            }

            // delete it
            nvClearObjectListEntry(pNvObj, &global.pNvVShaderLists);
            pNvObj->release();
        }

        // next shader
        lpDeleteVShader++;
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2VERTEXSHADER, (*ppCommands)->wStateCount,0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetVertexShader (NV_DP2FUNCTION_ARGLIST)
{
    DWORD count;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    DWORD dwHandle       = ((LPD3DHAL_DP2VERTEXSHADER)lpPrim)->dwHandle;
    CVertexShader *pVertexShader;
#if STATE_OPTIMIZE
    CVertexShader *pPrevVertexShader;
#endif

    dbgTracePush ("nvDP2SetVertexShader");

    // if we get a handle of 0 -- unbind the current vertex shader
    if (dwHandle == 0)
    {
        // the default shader (handle 0) was created in nvContextCreate
        CNvObject *pNvObj = nvGetObjectFromHandle(pContext->dwDDLclID, 0, &global.pNvVShaderLists);
        pContext->pCurrentVShader = pNvObj->getVertexShader();
        pContext->pCurrentVShader->reset();
        NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2VERTEXSHADER, (*ppCommands)->wStateCount,0);
        dbgTracePop();
        return (D3D_OK);
    }

    //since we can't have more than one active shader, spin through list to the last one
    for (count = 1; count<(*ppCommands)->wStateCount; count++, lpPrim += sizeof(D3DHAL_DP2VERTEXSHADER));
    CNvObject *pNvObj;

    if (dwHandle & 0x1)
    {
        pNvObj = nvGetObjectFromHandle(pContext->dwDDLclID, dwHandle, &global.pNvVShaderLists);

        // code vertex shader
        nvAssert(pNvObj);
        pVertexShader = pNvObj->getVertexShader();
        nvAssert(pVertexShader);

        if (pVertexShader->hasConstants()) {
            nvAssert(pContext->pVShaderConsts);
            // copy the constants from the shader to the context storage
            for (int i=0; i < D3DVS_CONSTREG_MAX_V1_1;) {
                char numConsts = pVertexShader->m_numConsts[i];
                if (numConsts) {
                    VSHADERREGISTER *pConst = &pVertexShader->m_vertexShaderConstants[i];
#ifdef DEBUG
                    // debug stuff
                    for (char j=0; j < numConsts; j++) {
                        DPF_LEVEL (NVDBG_LEVEL_VSHADER_INFO, "Set const from VS [%d] = (%f, %f, %f, %f)",
                                   i+j, pConst[j].x, pConst[j].y, pConst[j].z, pConst[j].w);
                    }
#endif
                    memcpy (&(pContext->pVShaderConsts->vertexShaderConstants[i]),
                            pConst, numConsts * sizeof(VSHADERREGISTER));
                    i += numConsts;
                }
                else {
                    i ++;
                }
            }
        }

#ifdef DEBUG
        if (pVertexShader->hasProgram()) {
            DPF_LEVEL (NVDBG_LEVEL_VSHADER_INFO, "Set Vertex Shader %02d", dwHandle);
            for (DWORD dwReg = 0; dwReg < 16; dwReg++) {
                if (pVertexShader->bVAExists(dwReg)) {
                    DPF_LEVEL (NVDBG_LEVEL_VSHADER_INFO, "reg: %02d , stream: %02d, type: %d, offset %02x, src: %x, flag: %d",
                               dwReg, pVertexShader->getVAStream(dwReg), pVertexShader->getVAType(dwReg),
                               pVertexShader->getVAOffset(dwReg), pVertexShader->getVASrc(dwReg), pVertexShader->getVAFlag(dwReg));
                }
            }
        }
#endif

    }
    else {
        // The handle is actually an FVF. We don't want to use an FVF as a handle,
        // because numerically they can get quite large.
        DWORD dwFVF = dwHandle;

        dwHandle = CVertexShader::getHandleFromFvf( dwFVF );
        pNvObj = nvGetObjectFromHandle(pContext->dwDDLclID, dwHandle, &global.pNvVShaderLists);

        // create an FVF vertex shader if it doesn't already exist
        if (pNvObj == NULL) {
#ifndef WINNT
            pNvObj = new CNvObject (pContext->pid);
#else
            pNvObj = new CNvObject (0);
#endif
            pVertexShader = new CVertexShader();
            pVertexShader->create (pContext, dwFVF, dwHandle);
            pNvObj->setObject (CNvObject::NVOBJ_VERTEXSHADER, pVertexShader);
            PNV_OBJECT_LIST pNvList = nvFindObjectList (pContext->dwDDLclID, &global.pNvVShaderLists);
            nvAddObjectToList (pNvList, pNvObj, dwHandle);
        }
        else
        { // Just temporary until the hash stuff works.
            pVertexShader = pNvObj->getVertexShader();
            pVertexShader->create (pContext, dwFVF, dwHandle);
        }
    }

    nvAssert(pNvObj != NULL);

#if STATE_OPTIMIZE
    // TODO: (SK) This may be a bit conservative -- we may not need to check the previous
    // vertex shader
    pPrevVertexShader = pContext->pCurrentVShader;
#endif

    pContext->pCurrentVShader = pVertexShader;
    pContext->dwStreamDMACount = nvCountStreamDMAs (pContext, NV_CAPS_MAX_STREAMS);

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        DWORD dwPTFlag = (NV_VERTEX_TRANSFORMED(pContext->pCurrentVShader)) ? KELVIN_FLAG_PASSTHROUGHMODE : 0;

#if STATE_OPTIMIZE
        // TODO: (SK) This may be a bit conservative -- we may not need to check the previous
        // vertex shader
        if (pContext->pCurrentVShader->hasProgram() || pPrevVertexShader->hasProgram())
        {
            // TODO: (SK) Do we really need all this state?
            pContext->hwState.dwStateFlags &= ~KELVIN_FLAG_PASSTHROUGHMODE;
            pContext->hwState.dwStateFlags |= dwPTFlag;
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_COMBINERS_SPECFOG
                                           |  KELVIN_DIRTY_VERTEX_SHADER
                                           |  KELVIN_DIRTY_FVF
                                           |  KELVIN_DIRTY_LIGHTS
                                           |  KELVIN_DIRTY_TRANSFORM
                                           |  KELVIN_DIRTY_MISC_STATE;
        }
        else
        {
            pContext->hwState.dwStateFlags &= ~KELVIN_FLAG_PASSTHROUGHMODE;
            pContext->hwState.dwStateFlags |= dwPTFlag;
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_VERTEX_SHADER
                                           |  KELVIN_DIRTY_FVF
                                           |  KELVIN_DIRTY_TRANSFORM;           //wish I didn't have to set this...

        }
#else
        pContext->hwState.dwStateFlags &= ~KELVIN_FLAG_PASSTHROUGHMODE;
        pContext->hwState.dwStateFlags |= dwPTFlag;
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_COMBINERS_SPECFOG
                                       |  KELVIN_DIRTY_VERTEX_SHADER
                                       |  KELVIN_DIRTY_FVF
                                       |  KELVIN_DIRTY_LIGHTS
                                       |  KELVIN_DIRTY_TRANSFORM
                                       |  KELVIN_DIRTY_MISC_STATE;
#endif

    }
    else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        DWORD dwPTFlag = (NV_VERTEX_TRANSFORMED(pContext->pCurrentVShader)) ? CELSIUS_FLAG_PASSTHROUGHMODE : 0;
        pContext->hwState.dwStateFlags &= ~CELSIUS_FLAG_PASSTHROUGHMODE;
        pContext->hwState.dwStateFlags |= dwPTFlag;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TL_MODE
                                       |  CELSIUS_DIRTY_FVF
                                       |  CELSIUS_DIRTY_TRANSFORM
                                       |  CELSIUS_DIRTY_CONTROL0
                                       |  CELSIUS_DIRTY_SPECFOG_COMBINER;
    }
#endif

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2VERTEXSHADER, (*ppCommands)->wStateCount,0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetVertexShaderConst (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2SETVERTEXSHADERCONST *lpSetVShaderConst;
    DWORD   dwExtraBytes=0,count;

    dbgTracePush ("nvDP2SetVertexShaderConst");


    for (count=0; count<(*ppCommands)->wStateCount; count++) 
    {
        lpSetVShaderConst = (D3DHAL_DP2SETVERTEXSHADERCONST *)(lpPrim);
        VSHADERREGISTER *pConst = (VSHADERREGISTER*)(lpPrim + sizeof(D3DHAL_DP2SETVERTEXSHADERCONST));
    
        nvAssert((lpSetVShaderConst->dwRegister + lpSetVShaderConst->dwCount) <= D3DVS_CONSTREG_MAX_V1_1);
        nvAssert(pContext->pVShaderConsts);

#ifdef DEBUG
        // debug stuff
        for (DWORD i = 0; i < lpSetVShaderConst->dwCount; i++)
        {
            DPF_LEVEL (NVDBG_LEVEL_VSHADER_INFO, "Set VS const [%d] = (%f, %f, %f, %f)",
                       lpSetVShaderConst->dwRegister + i, pConst[i].x, pConst[i].y, pConst[i].z, pConst[i].w);
        }
#endif
    
        memcpy (&(pContext->pVShaderConsts->vertexShaderConstants[lpSetVShaderConst->dwRegister]),
                pConst, lpSetVShaderConst->dwCount * sizeof(VSHADERREGISTER));
        
        dwExtraBytes += lpSetVShaderConst->dwCount * sizeof(VSHADERREGISTER);   
        
        lpPrim += (lpSetVShaderConst->dwCount * sizeof(VSHADERREGISTER) ) + sizeof(D3DHAL_DP2SETVERTEXSHADERCONST);
    }
#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) 
    {
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_VERTEX_SHADER;
    }
#endif

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETVERTEXSHADERCONST, (*ppCommands)->wStateCount, dwExtraBytes);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CreatePixelShader (NV_DP2FUNCTION_ARGLIST)
{
    D3DHAL_DP2CREATEPIXELSHADER *lpCreatePShader;
    CNvObject      *pObj;
    CPixelShader   *pPShader;
    WORD            wCount;
    LPBYTE          lpPrim  = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    PNV_OBJECT_LIST pNvList = nvFindObjectList(pContext->dwDDLclID, &global.pNvPShaderLists);
    DWORD           dwSize  = 0;

    dbgTracePush ("nvDP2CreatePixelShader");

    for (wCount=0; wCount<(*ppCommands)->wStateCount; wCount++)
    {
        lpCreatePShader = (D3DHAL_DP2CREATEPIXELSHADER *)(lpPrim);
#ifndef WINNT
        //this is a hack cause I don't have a ddslcl to get a pid from here???
        //I don't get create calls like I do for surfaces and must lazily create
        //the palette surfaces at the last moment.  Make it match the surfaces PID.
        pObj  =  new CNvObject (pContext->pid);
#else
        pObj  =  new CNvObject (0);
#endif
        pPShader = new CPixelShader();
        pPShader->create (pContext, lpCreatePShader->dwHandle,lpCreatePShader->dwCodeSize, (DWORD *)(lpPrim+sizeof(D3DHAL_DP2CREATEPIXELSHADER) ));
        pObj->setObject (CNvObject::NVOBJ_PIXELSHADER, pPShader);

        nvAddObjectToList(pNvList ,pObj, lpCreatePShader->dwHandle);
        lpPrim += sizeof(D3DHAL_DP2CREATEPIXELSHADER) +
        lpCreatePShader->dwCodeSize;
        dwSize += lpCreatePShader->dwCodeSize;
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2CREATEPIXELSHADER, (*ppCommands)->wStateCount, dwSize );

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2DeletePixelShader (NV_DP2FUNCTION_ARGLIST)
{

    WORD   wCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2PIXELSHADER *lpDeletePShader = (D3DHAL_DP2PIXELSHADER *)(lpPrim);

    dbgTracePush ("nvDP2DeletePixelShader");

    for (wCount=0; wCount<(*ppCommands)->wStateCount; wCount++)
    {
        CNvObject *pNvObj = nvGetObjectFromHandle(pContext->dwDDLclID, lpDeletePShader->dwHandle, &global.pNvPShaderLists);
        nvClearObjectListEntry(pNvObj, &global.pNvPShaderLists);
        pNvObj->release();
        lpDeletePShader++;
    }
    //need to make sure it isn't active Pixel Shader...
    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2PIXELSHADER, (*ppCommands)->wStateCount, 0 );

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetPixelShader (NV_DP2FUNCTION_ARGLIST)
{
    WORD   wCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    pContext->pLastPShader = pContext->pCurrentPShader;

    dbgTracePush ("nvDP2SetPixelShader");

    DPF_LEVEL(NVDBG_LEVEL_PIXEL_SHADER, "nvDP2SetPixelShader: handle=%d", ((LPD3DHAL_DP2PIXELSHADER)lpPrim)->dwHandle);

    for (wCount=1; wCount<(*ppCommands)->wStateCount; wCount++,lpPrim += sizeof(D3DHAL_DP2PIXELSHADER) );

    if (((LPD3DHAL_DP2PIXELSHADER)lpPrim)->dwHandle) {
        CNvObject *pNvObj = nvGetObjectFromHandle(pContext->dwDDLclID, ((LPD3DHAL_DP2PIXELSHADER)lpPrim)->dwHandle, &global.pNvPShaderLists);
        pContext->pCurrentPShader = pNvObj->getPixelShader();

    } else if (pContext->pCurrentPShader) {
#if (NVARCH >= 0x020)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_REALLY_FILTHY;
        } else
#endif
#if (NVARCH >= 0x010)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_REALLY_FILTHY;
            }
#endif

        pContext->pCurrentPShader = 0;
    }

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_PIXEL_SHADER;
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_PIXEL_SHADER;
#endif

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2PIXELSHADER, (*ppCommands)->wStateCount, 0 );

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetStreamSource (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE                     lpPrim         = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2SETSTREAMSOURCE *lpStreamSource = (D3DHAL_DP2SETSTREAMSOURCE *)(lpPrim);
    WORD                       wCount;
    CNvObject                 *pObj;
#if STATE_OPTIMIZE
    bool                       bChanged       = false;
#endif

    dbgTracePush ("nvDP2SetStreamSource");

    //check to see if the current streams/rendertarget/Z/textures need to be unlocked
    if (pContext->dwHWUnlockAllPending) { //HMH
        nvHWUnlockTextures (pContext);
        // need to mark all streams as in use by hardware...
        nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
        pContext->dwHWUnlockAllPending=FALSE;
    }

    for (wCount=0; wCount< (*ppCommands)->wStateCount; wCount++)
    {
        if (lpStreamSource->dwVBHandle && (lpStreamSource->dwStream < 16)) //temporary check to avoid pixelshader bug
        {
            pObj = nvGetObjectFromHandle (pContext->dwDDLclID, lpStreamSource->dwVBHandle, &global.pNvSurfaceLists);
            if (pContext->ppDX8Streams[lpStreamSource->dwStream] != pObj->getVertexBuffer() ||
                pContext->ppDX8Streams[lpStreamSource->dwStream] &&
                pContext->ppDX8Streams[lpStreamSource->dwStream]->getVertexStride() != lpStreamSource->dwStride
                )
            {
            pContext->ppDX8Streams[lpStreamSource->dwStream] = pObj->getVertexBuffer();
            pContext->ppDX8Streams[lpStreamSource->dwStream]->setVertexStride(lpStreamSource->dwStride);

            DPF_LEVEL(NVDBG_LEVEL_INFO,
            "SetStreamSource Stream: [%d] Handle: %d VB:%08x (%08x)", lpStreamSource->dwStream,
                      lpStreamSource->dwVBHandle,
                      (DWORD)(pContext->ppDX8Streams[lpStreamSource->dwStream]),
                      pContext->ppDX8Streams[lpStreamSource->dwStream]->getAddress());
#if STATE_OPTIMIZE
            bChanged = true;
#endif
           }
#ifdef DEBUG
            else {
                nvAssert(pContext->ppDX8Streams[lpStreamSource->dwStream]->getVertexStride() == lpStreamSource->dwStride);
#if STATE_OPTIMIZE
                bChanged = true;
#endif
            }
#endif

        }
        else
        {
            // invalidate streams
            pContext->ppDX8Streams[lpStreamSource->dwStream] = 0;
        }
        lpStreamSource++;
    }

    pContext->dwStreamDMACount = nvCountStreamDMAs (pContext, NV_CAPS_MAX_STREAMS);

    CVertexShader *pVertexShader = pContext->pCurrentVShader;
    CVertexBuffer *pVertexBuffer = pContext->ppDX8Streams[pVertexShader->getVAStream(defaultInputRegMap[D3DVSDE_POSITION])];
    if (pVertexBuffer)
    {
        if (pVertexBuffer->isModified() && !pVertexBuffer->isDynamic()
            && (pVertexBuffer->getLockCount() == 1) && !pVertexBuffer->getSuperTriLookAsideBuffer())
        {
            // check more supertri criteria...
#ifdef MCFD
            if ((pVertexBuffer->hwCanRead())) {
#else
            if ((pVertexBuffer->getSurface()->hwCanRead())) {
#endif
                pVertexBuffer->prepareSuperTriLookAsideBuffer();
            }
        }
    }

#if (NVARCH >= 0x010)
#if STATE_OPTIMIZE
    if (bChanged)
    {
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF
                                       | CELSIUS_DIRTY_LIGHTS
                                       | CELSIUS_DIRTY_TEXTURE_STATE
                                       | CELSIUS_DIRTY_TEXTURE_TRANSFORM;

        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF
                                       | KELVIN_DIRTY_TEXTURE_TRANSFORM;

        if (pContext->dwRenderState[D3DRENDERSTATE_COLORVERTEX])
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_LIGHTS;

// (SK) These two may need to be set, mainly DIRTY_TEXTURE_STATE. If the
// texture state finds that dwNumTexCoordsNeeded != pContext->hwState.dwNumTexCoordsNeeded
// or
// pContext->hwState.dwTexCoordIndices != dwTexCoordIndices, it sets
// KELVIN_DIRTY_FVF. But we set that bit above anyway...

//             pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TEXTURE_STATE;

    }
#else
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF
                                   | CELSIUS_DIRTY_LIGHTS
                                   | CELSIUS_DIRTY_TEXTURE_STATE
                                   | CELSIUS_DIRTY_TEXTURE_TRANSFORM;
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF
                                   | KELVIN_DIRTY_LIGHTS
                                   | KELVIN_DIRTY_TEXTURE_STATE
                                   | KELVIN_DIRTY_TEXTURE_TRANSFORM;
#endif
#endif

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETSTREAMSOURCE, (*ppCommands)->wStateCount, 0 );

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetStreamSourceUM (NV_DP2FUNCTION_ARGLIST)
{
    WORD                         wCount;
    LPBYTE                       lpPrim           = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2SETSTREAMSOURCEUM *lpStreamSourceUM = (D3DHAL_DP2SETSTREAMSOURCEUM *)(lpPrim);

    dbgTracePush ("nvDP2SetStreamSourceUM");

    //this sets a certain stream to use the passed in pointer for it's data
    //this information is not persistant across DP2 calls, so we can just use the "disposible" VB

    //check to see if the current streams/rendertarget/Z/textures need to be unlocked
    if(pContext->dwHWUnlockAllPending){ //HMH
        nvHWUnlockTextures (pContext);
        // need to mark all streams as in use by hardware...
        nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
        pContext->dwHWUnlockAllPending=FALSE;
    }
    for (wCount=0; wCount< (*ppCommands)->wStateCount; wCount++,lpStreamSourceUM++)
    {
        pContext->ppDX8Streams[lpStreamSourceUM->dwStream] = getDC()->defaultSysVB.getVB();
        pContext->ppDX8Streams[lpStreamSourceUM->dwStream]->setVertexStride(lpStreamSourceUM->dwStride);

        //BUG! this COULD cause a problem.  If the user ever passed in AGP or video memory (impossible currently)
        // we would calculate the surface offset incorrectly in the nvsurface.h.  Since this is guaranteed to be
        // user accessible system memory, as of 5/31/00, we are safe.
        //but this could cause a memory leak/clean-up issue.  I'll check HMH.
        pContext->ppDX8Streams[lpStreamSourceUM->dwStream]->own ((DWORD)pContext->pVertexDataUM,
                                                                 pContext->dwVertexDataLengthUM,
                                                                 CSimpleSurface::HEAP_SYS);

    }

    pContext->dwStreamDMACount = nvCountStreamDMAs (pContext, NV_CAPS_MAX_STREAMS);

#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF
                                   | CELSIUS_DIRTY_LIGHTS
                                   | CELSIUS_DIRTY_TEXTURE_STATE
                                   | CELSIUS_DIRTY_TEXTURE_TRANSFORM;
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF
                                   | KELVIN_DIRTY_LIGHTS
                                   | KELVIN_DIRTY_TEXTURE_STATE
                                   | KELVIN_DIRTY_TEXTURE_TRANSFORM;
#endif

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETSTREAMSOURCEUM, (*ppCommands)->wStateCount, 0 );

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetIndices (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE     lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    CNvObject *pObj;
    WORD       wCount;

    dbgTracePush ("nvDP2SetIndices");

    for (wCount=1; wCount<(*ppCommands)->wStateCount; wCount++,lpPrim += sizeof(D3DHAL_DP2SETINDICES) );
    if (((LPD3DHAL_DP2SETINDICES)lpPrim)->dwVBHandle > 0 )
    {
        pObj = nvGetObjectFromHandle (pContext->dwDDLclID, ((LPD3DHAL_DP2SETINDICES)lpPrim)->dwVBHandle, &global.pNvSurfaceLists );
#if (NVARCH >= 0x010)
        pContext->pIndexBuffer = pObj->getVertexBuffer();
        pContext->pIndexBuffer->setVertexStride(((LPD3DHAL_DP2SETINDICES)lpPrim)->dwStride);
#endif
    }
    else
    {
        //better not use indexed prims
#if (NVARCH >= 0x010)
        pContext->pIndexBuffer = NULL;
#endif
    }
    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETINDICES, (*ppCommands)->wStateCount, 0 );

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2DrawPrimitive (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE                    lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    LPD3DHAL_DP2DRAWPRIMITIVE lpDPCommand = (LPD3DHAL_DP2DRAWPRIMITIVE)(lpPrim);
    WORD                      wCount;

    static DWORD PrimTypeToDP2[] = {
        0,
        D3DDP2OP_POINTS,
        D3DDP2OP_LINELIST,
        D3DDP2OP_LINESTRIP,
        D3DDP2OP_TRIANGLELIST,
        D3DDP2OP_TRIANGLESTRIP,
        D3DDP2OP_TRIANGLEFAN
    };

    dbgTracePush ("nvDP2DrawPrimitive");

    pContext->dp2.dwVertexBufferOffset = 0;
#if WINNT
    global.dwMaxVertexOffset = 0xFFFFFFFF;  // temp, need to fix for WHQL HMH
#endif

    CVertexShader *pVertexShader = pContext->pCurrentVShader;

    // If the last call to SetVertexShader passed in a handle of 0, we can't draw anything.
    nvAssert( pVertexShader->getHandle() || pVertexShader->getFVF() );
    // The FVF shader needs to have a non-null stream 0.
    nvAssert( !pVertexShader->isFvfShader() || (pContext->ppDX8Streams[0] != NULL) );

    if( (pContext->pCurrentVShader->getHandle() || pContext->pCurrentVShader->getFVF() ) &&
        (!pVertexShader->isFvfShader() || (pContext->ppDX8Streams[0] != NULL))  )
    {
        for (wCount=0; wCount<(*ppCommands)->wStateCount; wCount++,lpDPCommand++)
        {
            pContext->dp2.dwDP2Prim     = PrimTypeToDP2[lpDPCommand->primType];
            pContext->dp2.dwVStart      = lpDPCommand->VStart;
            pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
            pContext->dp2.dwIndices     = NULL;
            pContext->dp2.dwIndexStride = 0;

#if (NVARCH >= 0x020)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                nvKelvinDispatchNonIndexedPrimitive (pContext, lpDPCommand->PrimitiveCount);
            } else
#endif
#if (NVARCH >= 0x010)
            {
                // force Celsius Vertex Shaders into inline inner loops
                nvCelsiusDispatchNonIndexedPrimitive (pContext, lpDPCommand->PrimitiveCount);
            }
#endif
            dbgFlushType (NVDBG_FLUSH_PRIMITIVE);
        }
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2DRAWPRIMITIVE, (*ppCommands)->wStateCount, 0 );

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2DrawIndexedPrimitive (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE                           lpPrim      = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    LPD3DHAL_DP2DRAWINDEXEDPRIMITIVE lpDPCommand = (LPD3DHAL_DP2DRAWINDEXEDPRIMITIVE)(lpPrim);
    WORD                             wCount;

    static DWORD PrimTypeToDP2[] = {
        0,
        D3DDP2OP_POINTS,  //this should work since stride etc is the same.... HMH
        D3DDP2OP_INDEXEDLINELIST,
        D3DDP2OP_INDEXEDLINESTRIP,
        D3DDP2OP_INDEXEDTRIANGLELIST2,
        D3DDP2OP_INDEXEDTRIANGLESTRIP,
        D3DDP2OP_INDEXEDTRIANGLEFAN
    };

    dbgTracePush ("nvDP2DrawIndexedPrimitive");

    pContext->dp2.dwVertexBufferOffset = 0;

#if WINNT
    global.dwMaxVertexOffset = 0xFFFFFFFF;  //temp, need to fix for WHQL HMH
#endif

    CVertexShader *pVertexShader = pContext->pCurrentVShader;

    // If the last call to SetVertexShader passed in a handle of 0, we can't draw anything.
    nvAssert( pVertexShader->getHandle() || pVertexShader->getFVF() );
    // The FVF shader needs to have a non-null stream 0.
    nvAssert( !pVertexShader->isFvfShader() || (pContext->ppDX8Streams[0] != NULL) );

    if( (pContext->pCurrentVShader->getHandle() || pContext->pCurrentVShader->getFVF() ) &&
        (!pVertexShader->isFvfShader() || (pContext->ppDX8Streams[0] != NULL))  )
    {
        for (wCount=0; wCount<(*ppCommands)->wStateCount; wCount++,lpDPCommand++)
        {
            pContext->dp2.dwDP2Prim     = PrimTypeToDP2[lpDPCommand->primType];
            pContext->dp2.dwVStart      = lpDPCommand->BaseVertexIndex;
            pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset + (pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
            pContext->dp2.dwIndices     = pContext->pIndexBuffer->getAddress() + pContext->pIndexBuffer->getVertexStride()*lpDPCommand->StartIndex;
            pContext->dp2.dwIndexStride = pContext->pIndexBuffer->getVertexStride();

#ifdef NV_PROFILE_DP2OPS
            pDriverData->pBenchmark->logDrawPrimitive((WORD)lpDPCommand->PrimitiveCount);
#endif

#if (NVARCH >= 0x020)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                nvKelvinDispatchIndexedPrimitive (pContext, lpDPCommand->PrimitiveCount);
            } else
#endif
#if (NVARCH >= 0x010)
            {
                nvCelsiusDispatchIndexedPrimitive (pContext, lpDPCommand->PrimitiveCount);
            }
#endif
            dbgFlushType (NVDBG_FLUSH_PRIMITIVE);
        }
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2DRAWINDEXEDPRIMITIVE, (*ppCommands)->wStateCount, 0 );

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2SetPixelShaderConst (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    DWORD dwExtraBytes=0,count;
    
    dbgTracePush ("nvDP2SetPixelShaderConst");

    for (count=0; count<(*ppCommands)->wStateCount; count++) 
    {
        LPD3DHAL_DP2SETPIXELSHADERCONST lpConstInfo = (D3DHAL_DP2SETPIXELSHADERCONST *)(lpPrim);
        D3DCOLORVALUE *lpConsts = (D3DCOLORVALUE *)(lpPrim + sizeof(D3DHAL_DP2SETPIXELSHADERCONST) );

        for (DWORD i=0; i<lpConstInfo->dwCount; i++) {
            DPF_LEVEL(NVDBG_LEVEL_PIXEL_SHADER, "nvDP2SetPixelShaderConst: idx=%d, a=%f, r=%f, g=%f, b=%f",
                      lpConstInfo->dwRegister+i, lpConsts[i].a, lpConsts[i].r, lpConsts[i].g, lpConsts[i].b);
            pContext->pixelShaderConsts[lpConstInfo->dwRegister+i] = lpConsts[i];
        }
        lpPrim += 4*4*lpConstInfo->dwCount + sizeof(D3DHAL_DP2SETPIXELSHADERCONST);
        dwExtraBytes += 4*4*lpConstInfo->dwCount;
    }
#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_PIXEL_SHADER;
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_PIXEL_SHADER;
#endif

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETPIXELSHADERCONST, (*ppCommands)->wStateCount, dwExtraBytes );

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2ClippedTriangleFan (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    WORD   wCount;

    dbgTracePush ("nvDP2ClippedTriangleFan");

    pContext->dp2.dwDP2Prim     = D3DDP2OP_TRIANGLEFAN;
    pContext->dp2.dwVStart      = 0;
    pContext->dp2.dwTotalOffset = pContext->dp2.dwVertexBufferOffset;
    pContext->dp2.dwIndices     = NULL;
    pContext->dp2.dwIndexStride = 0;

#if WINNT
    global.dwMaxVertexOffset = 0xFFFFFFFF;  // temp, need to fix for WHQL HMH
#endif

    for (wCount=0; wCount<(*ppCommands)->wStateCount; wCount++)
    {
        pContext->dp2.dwVertexBufferOffset = ((LPD3DHAL_CLIPPEDTRIANGLEFAN)lpPrim)->FirstVertexOffset;
        pContext->dp2.dwEdgeFlags          = ((LPD3DHAL_CLIPPEDTRIANGLEFAN)lpPrim)->dwEdgeFlags;

#if (NVARCH >= 0x20)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            // if we're in wireframe mode, we have to heed edge flags...
            ((pContext->dwRenderState[D3DRENDERSTATE_FILLMODE] == D3DFILL_WIREFRAME) ?
                nvKelvinDispatchLegacyWireframePrimitive :
                nvKelvinDispatchNonIndexedPrimitive) (pContext, ((LPD3DHAL_CLIPPEDTRIANGLEFAN)lpPrim)->PrimitiveCount);
        }
        else
#endif
#if (NVARCH >= 0x10)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
            // force Celsius Vertex Shaders into inline inner loops
            // if we're in wireframe mode, we have to heed edge flags...
            ((pContext->dwRenderState[D3DRENDERSTATE_FILLMODE] == D3DFILL_WIREFRAME) ?
                nvCelsiusDispatchLegacyWireframePrimitive :
                nvCelsiusDispatchNonIndexedPrimitive) (pContext, ((LPD3DHAL_CLIPPEDTRIANGLEFAN)lpPrim)->PrimitiveCount);
        }
#endif

        lpPrim += sizeof (D3DHAL_CLIPPEDTRIANGLEFAN);
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_CLIPPEDTRIANGLEFAN, (*ppCommands)->wStateCount, 0 );

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2DrawPrimitive2 (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE                     lpPrim      = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    LPD3DHAL_DP2DRAWPRIMITIVE2 lpDPCommand = (LPD3DHAL_DP2DRAWPRIMITIVE2)(lpPrim);
    WORD                       wCount;

    static DWORD PrimTypeToDP2[] = {
        0,
        D3DDP2OP_POINTS,
        D3DDP2OP_LINELIST,
        D3DDP2OP_LINESTRIP,
        D3DDP2OP_TRIANGLELIST,
        D3DDP2OP_TRIANGLESTRIP,
        D3DDP2OP_TRIANGLEFAN
    };

    dbgTracePush ("nvDP2DrawPrimitive2");

    pContext->dp2.dwVStart             = 0;

#if WINNT
    global.dwMaxVertexOffset = 0xFFFFFFFF;  //temp, need to fix for WHQL HMH
#endif

    for (wCount=0; wCount<(*ppCommands)->wStateCount; wCount++,lpDPCommand++)
    {
        pContext->dp2.dwDP2Prim            = PrimTypeToDP2[lpDPCommand->primType];
        pContext->dp2.dwVertexBufferOffset = lpDPCommand->FirstVertexOffset;
        pContext->dp2.dwTotalOffset        = pContext->dp2.dwVertexBufferOffset;
        pContext->dp2.dwIndices            = NULL;
        pContext->dp2.dwIndexStride        = 0;

#if (NVARCH >= 0x020)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            nvKelvinDispatchNonIndexedPrimitive (pContext, lpDPCommand->PrimitiveCount);
        } else
#endif
#if (NVARCH >= 0x010)
        {
            nvCelsiusDispatchNonIndexedPrimitive (pContext, lpDPCommand->PrimitiveCount);
        }
#endif
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2DRAWPRIMITIVE2, (*ppCommands)->wStateCount, 0 );

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2DrawIndexedPrimitive2 (NV_DP2FUNCTION_ARGLIST)
{

    LPBYTE                            lpPrim      = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    LPD3DHAL_DP2DRAWINDEXEDPRIMITIVE2 lpDPCommand = (LPD3DHAL_DP2DRAWINDEXEDPRIMITIVE2)(lpPrim);
    WORD                              wCount;

    static DWORD PrimTypeToDP2[] = {
        0,
        D3DDP2OP_POINTS,  //this should work since stride etc is the same.... HMH
        D3DDP2OP_INDEXEDLINELIST,
        D3DDP2OP_INDEXEDLINESTRIP,
        D3DDP2OP_INDEXEDTRIANGLELIST2,
        D3DDP2OP_INDEXEDTRIANGLESTRIP,
        D3DDP2OP_INDEXEDTRIANGLEFAN
    };

    dbgTracePush ("nvDP2DrawIndexedPrimitive2");

    pContext->dp2.dwVStart             = 0;

#if WINNT
    global.dwMaxVertexOffset = 0xFFFFFFFF;  //temp, need to fix for WHQL HMH
#endif
    for (wCount=0; wCount<(*ppCommands)->wStateCount; wCount++,lpDPCommand++)
    {
        pContext->dp2.dwDP2Prim            = PrimTypeToDP2[lpDPCommand->primType];
        pContext->dp2.dwVertexBufferOffset = lpDPCommand->BaseVertexOffset;
        pContext->dp2.dwTotalOffset        = pContext->dp2.dwVertexBufferOffset;
        pContext->dp2.dwIndices            = pContext->pIndexBuffer->getAddress() + lpDPCommand->StartIndexOffset;
        pContext->dp2.dwIndexStride        = pContext->pIndexBuffer->getVertexStride();

#if (NVARCH >= 0x020)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            nvKelvinDispatchIndexedPrimitive (pContext, lpDPCommand->PrimitiveCount);
        } else
#endif
#if (NVARCH >= 0x010)
        {
            // force Celsius Vertex Shaders into inline inner loops
            // bug bug maybe? BaseVertexOffset is supposed to be able to be negative.  Since it's an unsigned
            // int, I think this is pretty hard, but it could cause issues.
            nvCelsiusDispatchIndexedPrimitive (pContext, lpDPCommand->PrimitiveCount);
        }
#endif
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2DRAWINDEXEDPRIMITIVE2, (*ppCommands)->wStateCount, 0 );

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2DrawRectSurface (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2DrawRectSurface");
    WORD wNumDrawRect = (*ppCommands)->wStateCount;
    LPBYTE lpPrim     = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    DWORD dwSize = 0;
    FLOAT *pfSegs = NULL;
    D3DRECTPATCH_INFO *pRSInfo = NULL;

    // this flag causes celsius state to compute a tessellator style vertex format
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        pContext->hwState.dwDirtyFlags  |= KELVIN_DIRTY_FVF;
        //TODO: make this smarter we are always forcing dirty now.  NOT good.
        nvSetKelvinState (pContext);
        //dirty FVF again so regular DP2 ops get reinitialized
        pContext->hwState.dwDirtyFlags  |= KELVIN_DIRTY_FVF;
    }
    else {
/*      pContext->hwState.dwStateFlags |= CELSIUS_FLAG_COMPUTE_TESS_VTX_STRIDE;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
        //TODO: make this smarter we are always forcing dirty now.  NOT good.
        nvSetCelsiusState (pContext);
        //dirty FVF again so regular DP2 ops get reinitialized
        pContext->hwState.dwStateFlags &= ~CELSIUS_FLAG_COMPUTE_TESS_VTX_STRIDE;
        pContext->hwState.dwDirtyFlags  |= CELSIUS_DIRTY_FVF;*/
        return(D3D_OK);
    }

    for (int i = 0; i < wNumDrawRect; i++) {
        LPD3DHAL_DP2DRAWRECTPATCH lpRectSurface = (LPD3DHAL_DP2DRAWRECTPATCH)(lpPrim);
        lpPrim += sizeof(D3DHAL_DP2DRAWRECTPATCH);

        if(lpRectSurface->Flags & RTPATCHFLAG_HASSEGS){
            pfSegs = (FLOAT *)lpPrim;
            lpPrim += sizeof(FLOAT)*4;
            dwSize += sizeof(FLOAT)*4;
        }
        if(lpRectSurface->Flags & RTPATCHFLAG_HASINFO){
            pRSInfo = (D3DRECTPATCH_INFO *)lpPrim;
            lpPrim += sizeof(D3DRECTPATCH_INFO);
            dwSize += sizeof(D3DRECTPATCH_INFO);
        }

#ifdef HOSURF_ENABLE
        nvDrawRectSurface(pContext, lpRectSurface->Handle, pfSegs, pRSInfo);
#endif
        pfSegs = NULL;
        pRSInfo = NULL;
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2DRAWRECTPATCH, (*ppCommands)->wStateCount, dwSize );

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2DrawTriSurface (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2DrawTriSurface");

    WORD wNumDrawRect = (*ppCommands)->wStateCount;
    LPBYTE lpPrim     = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    //this flag causes celsius state to compute a tessellator style vertex format
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)
    {
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
        //TODO: make this smarter we are always forcing dirty now.  NOT good.
        nvSetKelvinState (pContext);
        //dirty FVF again so regular DP2 ops get reinitialized
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
    }
    else
    {
/*        pContext->hwState.dwStateFlags |= CELSIUS_FLAG_COMPUTE_TESS_VTX_STRIDE;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
        //TODO: make this smarter we are always forcing dirty now.  NOT good.
        nvSetCelsiusState (pContext);
        //dirty FVF again so regular DP2 ops get reinitialized
        pContext->hwState.dwStateFlags &= ~CELSIUS_FLAG_COMPUTE_TESS_VTX_STRIDE;
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF; */
    return(D3D_OK);
    }

    DWORD dwSize = 0;
    FLOAT *pfSegs = NULL;
    D3DTRIPATCH_INFO *pTSInfo = NULL;
    for( int i = 0; i < wNumDrawRect; i++ ){
        LPD3DHAL_DP2DRAWTRIPATCH lpTriSurface = (LPD3DHAL_DP2DRAWTRIPATCH)(lpPrim);
        lpPrim += sizeof(D3DHAL_DP2DRAWTRIPATCH);

        if(lpTriSurface->Flags & RTPATCHFLAG_HASSEGS){
            pfSegs = (FLOAT *)lpPrim;
            lpPrim += sizeof(FLOAT)*3;
            dwSize += sizeof(FLOAT)*3;
        }
        if(lpTriSurface->Flags & RTPATCHFLAG_HASINFO){
            pTSInfo = (D3DTRIPATCH_INFO *)lpPrim;
            lpPrim += sizeof(D3DTRIPATCH_INFO);
            dwSize += sizeof(D3DTRIPATCH_INFO);
        }

        nvDrawTriSurface(pContext, lpTriSurface->Handle, pfSegs, pTSInfo); //-- not implemented yet

        pfSegs = NULL;
        pTSInfo = NULL;
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2DRAWTRIPATCH, (*ppCommands)->wStateCount, dwSize );

    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2VolumeBlt (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2VOLUMEBLT *lpBlitParam = (D3DHAL_DP2VOLUMEBLT *)(lpPrim);
    DWORD dwNumBlits = (*ppCommands)->wStateCount;

    dbgTracePush ("nvDP2VolumeBlit");

    for (DWORD i=0; i<dwNumBlits; i++) {

        if (lpBlitParam->dwDDDestSurface) {

            CNvObject* pSrcObj = nvGetObjectFromHandle (pContext->dwDDLclID, lpBlitParam->dwDDSrcSurface, &global.pNvSurfaceLists);
            CNvObject* pDstObj = nvGetObjectFromHandle (pContext->dwDDLclID, lpBlitParam->dwDDDestSurface, &global.pNvSurfaceLists);

            D3DBOX box = lpBlitParam->srcBox;
            DWORD dwDestX = lpBlitParam->dwDestX;
            DWORD dwDestY = lpBlitParam->dwDestY;
            DWORD dwDestZ = lpBlitParam->dwDestZ;

            while (1) {
                nvTextureBltDX7(pContext, pSrcObj, pDstObj, &box, dwDestX, dwDestY, dwDestZ, lpBlitParam->dwFlags);

                pSrcObj = pSrcObj->getAttachedA();

                pDstObj = pDstObj->getAttachedA();

                if (!pSrcObj || !pDstObj) break;

                box.Top    >>= 1;
                box.Left   >>= 1;
                box.Front  >>= 1;
                box.Bottom ++;
                box.Bottom >>= 1;
                box.Right  ++;
                box.Right  >>= 1;
                box.Back   ++;
                box.Back   >>= 1;

                dwDestX >>= 1;
                dwDestY >>= 1;
                dwDestZ >>= 1;
            }

            lpBlitParam++;
        }
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2VOLUMEBLT, (*ppCommands)->wStateCount, 0 );

    dbgFlushType (NVDBG_FLUSH_2D);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2BufferBlt (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2BUFFERBLT *lpBltData = (D3DHAL_DP2BUFFERBLT *)(lpPrim);
    BOOL bIsFullBlit;

    dbgTracePush ("nvDP2BufferBlit");

    //ASSUMPTIONS:
    //lpBltData->rSrc.Offset and lpBltData->rSrc.Size are byte quantities
    //src is a system memory vertexbuffer

    //QUESTION: What can be passed in dwFlags?

    //check to see if the current streams/rendertarget/Z/textures need to be unlocked
    if(pContext->dwHWUnlockAllPending){ //HMH
        nvHWUnlockTextures (pContext);
        // need to mark all streams as in use by hardware...
        nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
        pContext->dwHWUnlockAllPending=FALSE;
    }

    CNvObject* pSrcObj = nvGetObjectFromHandle (pContext->dwDDLclID, lpBltData->dwDDSrcSurface, &global.pNvSurfaceLists);
    CNvObject* pDstObj = nvGetObjectFromHandle (pContext->dwDDLclID, lpBltData->dwDDDestSurface, &global.pNvSurfaceLists);
    bIsFullBlit = FALSE;

    //assert that pSrc exists and the requested blit does not exceed src size
    nvAssert (pSrcObj && pSrcObj->getVertexBuffer());
    DWORD dwSrcSize = pSrcObj->getVertexBuffer()->getSize();
    nvAssert (dwSrcSize >= lpBltData->rSrc.Offset+lpBltData->rSrc.Size);

    //assert that pDst exists and the requested blit does not exceed dst size
    nvAssert (pDstObj && pDstObj->getVertexBuffer());
    DWORD dwDstSize = pDstObj->getVertexBuffer()->getSize();
    if(dwDstSize == lpBltData->dwOffset) bIsFullBlit = TRUE;
    nvAssert (dwDstSize >= lpBltData->dwOffset + lpBltData->rSrc.Size);

    //blind memcpy for now.  With managed vertex buffers (which should always be on)
    //we aren't ever supposed to see this call.
    BYTE *pbSrc, *pbDst;
    pbSrc = (BYTE*)pSrcObj->getVertexBuffer()->getAddress();
    pbDst = (BYTE*)pDstObj->getVertexBuffer()->getAddress();

#ifdef MCFD
    pSrcObj->getVertexBuffer()->LockForCPU(CSimpleSurface::LOCK_READ);
#else
    pSrcObj->getVertexBuffer()->cpuLock(CSimpleSurface::LOCK_NORMAL);
#endif
    /*if(bIsFullBlit) pDstObj->getVertexBuffer()->cpuLock(CSimpleSurface::LOCK_RENAME);
    else*/

#ifdef MCFD
    pDstObj->getVertexBuffer()->LockForCPU(CSimpleSurface::LOCK_WRITE);
#else
    pDstObj->getVertexBuffer()->cpuLock(CSimpleSurface::LOCK_NORMAL);
#endif

    if(pbSrc == pbDst)
        //possible overlap use memmove.
        memmove(pbDst + lpBltData->dwOffset, pbSrc + lpBltData->rSrc.Offset, lpBltData->rSrc.Size);
    else
        memcpy(pbDst + lpBltData->dwOffset, pbSrc + lpBltData->rSrc.Offset, lpBltData->rSrc.Size);

#ifdef MCFD
    pSrcObj->getVertexBuffer()->UnlockForCPU();
    pDstObj->getVertexBuffer()->UnlockForCPU();
#else
    pSrcObj->getVertexBuffer()->cpuUnlock();
    pDstObj->getVertexBuffer()->cpuUnlock();
#endif

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2BUFFERBLT, 1, 0 );

    dbgFlushType (NVDBG_FLUSH_2D);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2MultiplyTransform (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2MULTIPLYTRANSFORM *lpMultTrans = (D3DHAL_DP2MULTIPLYTRANSFORM *)(lpPrim);
    D3DMATRIX Result;
    DWORD dwxfrmType   = (DWORD)lpMultTrans->xfrmType & (~0x80000000);
    DWORD dwStage;

    dbgTracePush ("nvDP2MultiplyTransform");

    switch (lpMultTrans->xfrmType) {

        case D3DTRANSFORMSTATE_WORLD_DX7:
        case D3DTS_WORLD:
            MatrixProduct4x4 (&Result,&lpMultTrans->matrix,&(pContext->xfmWorld[0]) );
            nvMemCopy (&(pContext->xfmWorld[0]), &Result, sizeof(D3DMATRIX));
#if (NVARCH >= 0x010)
            pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_TRANSFORM |
                                               CELSIUS_DIRTY_LIGHTS);
            pContext->hwState.dwDirtyFlags |= (KELVIN_DIRTY_TRANSFORM |
                                               KELVIN_DIRTY_LIGHTS);
#endif
            break;

        case D3DTRANSFORMSTATE_WORLD1_DX7:
        case D3DTS_WORLD1:
            MatrixProduct4x4 (&Result,&lpMultTrans->matrix,&(pContext->xfmWorld[1]) );
            nvMemCopy (&(pContext->xfmWorld[1]), &Result, sizeof(D3DMATRIX));
#if (NVARCH >= 0x010)
            pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TRANSFORM;
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM;
#endif
            break;

        case D3DTRANSFORMSTATE_WORLD2_DX7:
        case D3DTS_WORLD2:
            MatrixProduct4x4 (&Result,&lpMultTrans->matrix,&(pContext->xfmWorld[2]) );
            nvMemCopy (&(pContext->xfmWorld[2]), &Result, sizeof(D3DMATRIX));
#if (NVARCH >= 0x020)
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM;
#endif
            break;

        case D3DTRANSFORMSTATE_WORLD3_DX7:
        case D3DTS_WORLD3:
            MatrixProduct4x4 (&Result,&lpMultTrans->matrix,&(pContext->xfmWorld[3]) );
            nvMemCopy (&(pContext->xfmWorld[3]), &Result, sizeof(D3DMATRIX));
#if (NVARCH >= 0x020)
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM;
#endif
            break;

        case D3DTRANSFORMSTATE_VIEW:
            MatrixProduct4x4 (&Result,&lpMultTrans->matrix,&(pContext->xfmView) );
            nvMemCopy (&(pContext->xfmView), &Result, sizeof(D3DMATRIX));
#if (NVARCH >= 0x010)
            pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_TEXTURE_STATE |
                                               CELSIUS_DIRTY_TRANSFORM |
                                               CELSIUS_DIRTY_LIGHTS);
            pContext->hwState.dwDirtyFlags |= (KELVIN_DIRTY_TEXTURE_STATE |
                                               KELVIN_DIRTY_TRANSFORM |
                                               KELVIN_DIRTY_LIGHTS);
#endif
            break;

        case D3DTRANSFORMSTATE_PROJECTION:
            MatrixProduct4x4 (&Result,&lpMultTrans->matrix,&(pContext->xfmProj) );
#ifdef  STEREO_SUPPORT
            if (STEREO_ENABLED)
            {

                Result = *(SetStereoProjection(&Result, pContext));
            }
#endif  //STEREO_SUPPORT
            nvMemCopy (&(pContext->xfmProj), &Result, sizeof(D3DMATRIX));
#if (NVARCH >= 0x010)
            pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TRANSFORM;
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM;
#endif
            break;

        case D3DTRANSFORMSTATE_TEXTURE0:
        case D3DTRANSFORMSTATE_TEXTURE1:
        case D3DTRANSFORMSTATE_TEXTURE2:
        case D3DTRANSFORMSTATE_TEXTURE3:
        case D3DTRANSFORMSTATE_TEXTURE4:
        case D3DTRANSFORMSTATE_TEXTURE5:
        case D3DTRANSFORMSTATE_TEXTURE6:
        case D3DTRANSFORMSTATE_TEXTURE7:
            dwStage = lpMultTrans->xfrmType - D3DTRANSFORMSTATE_TEXTURE0;
            if (!( lpMultTrans->xfrmType & 0x80000000)) { //set identity??
                MatrixProduct4x4 (&Result,&lpMultTrans->matrix,&(pContext->tssState[dwStage].mTexTransformMatrix) );
                nvMemCopy (&pContext->tssState[dwStage].mTexTransformMatrix, &Result, sizeof(D3DMATRIX));
            }
#if (NVARCH >= 0x010)
            pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TEXTURE_TRANSFORM;
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TEXTURE_TRANSFORM;
#endif
            break;

        default:
            DPF_LEVEL (NVDBG_LEVEL_INFO,"Ignoring unknown transform type in MultiplyXfrm");
            break;

    }  // switch (dwxfrmType)

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2MULTIPLYTRANSFORM, 1, 0 );

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------
// DP2 Recording Functions
//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetRenderState (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetRenderState");

    DWORD ddrval;
    DWORD dwSize = sizeof(D3DHAL_DP2COMMAND) + (*ppCommands)->wStateCount * sizeof(D3DHAL_DP2RENDERSTATE);
    ddrval = recordState (pContext, (LPBYTE)(*ppCommands), dwSize);
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordRenderState failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2RENDERSTATE, (*ppCommands)->wPrimitiveCount, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetTextureStageState (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetTextureStageState");

    DWORD ddrval;
    DWORD dwSize = sizeof(D3DHAL_DP2COMMAND) + (*ppCommands)->wStateCount * sizeof(D3DHAL_DP2TEXTURESTAGESTATE);
    ddrval = recordState (pContext, (LPBYTE)(*ppCommands), dwSize);
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordTSS failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2TEXTURESTAGESTATE, (*ppCommands)->wPrimitiveCount, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetViewport (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetViewport");

    DWORD ddrval;
    ddrval = recordLastStateOnly (pContext, (*ppCommands), sizeof(D3DHAL_DP2VIEWPORTINFO));
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordViewport failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2VIEWPORTINFO, 1, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetWRange (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetWRange");

    DWORD ddrval;
    ddrval = recordLastStateOnly (pContext, (*ppCommands), sizeof(D3DHAL_DP2WINFO));
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordWRange failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2WINFO, 1, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetZRange (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetZRange");

    DWORD ddrval;
    ddrval = recordLastStateOnly (pContext, (*ppCommands), sizeof(D3DHAL_DP2ZRANGE));
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordZRange failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2ZRANGE, (*ppCommands)->wStateCount, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetMaterial (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetMaterial");

    DWORD ddrval;
    ddrval = recordLastStateOnly (pContext, (*ppCommands), sizeof(D3DHAL_DP2SETMATERIAL));
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordMaterial failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETMATERIAL, (*ppCommands)->wStateCount, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetLight (NV_DP2FUNCTION_ARGLIST)
{
    WORD wNumSetLight;
    LPD3DHAL_DP2SETLIGHT pSetLight;
    DWORD i, dwStride, dwTotalStride;
    DWORD ddrval;

    dbgTracePush ("nvDP2RecordSetLight");

    wNumSetLight = (*ppCommands)->wStateCount;
    dwTotalStride = sizeof(D3DHAL_DP2COMMAND);
    pSetLight = (LPD3DHAL_DP2SETLIGHT)((*ppCommands) + 1);

    for (i=0; i<wNumSetLight; i++)
    {
        dwStride = sizeof (D3DHAL_DP2SETLIGHT);

        switch (SETLIGHT_DATATYPE(pSetLight)) {
            case D3DHAL_SETLIGHT_ENABLE:
                break;
            case D3DHAL_SETLIGHT_DISABLE:
                break;
            case D3DHAL_SETLIGHT_DATA:
                dwStride += sizeof(D3DLIGHT7);
                break;
        }

        dwTotalStride += dwStride;
        pSetLight = (D3DHAL_DP2SETLIGHT *)((LPBYTE)pSetLight + dwStride);
    }

    ddrval = recordState (pContext, (LPBYTE)(*ppCommands), dwTotalStride);
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordSetLight failed");
    (*ppCommands) = (LPD3DHAL_DP2COMMAND)((LPBYTE)(*ppCommands) + dwTotalStride);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetTransform (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetTransform");

    DWORD ddrval;
    DWORD dwSize = sizeof(D3DHAL_DP2COMMAND) +
                   (*ppCommands)->wStateCount * sizeof(D3DHAL_DP2SETTRANSFORM);
    ddrval = recordState (pContext, (LPBYTE)(*ppCommands), dwSize);
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordSetTransform failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETTRANSFORM, (*ppCommands)->wStateCount, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordExtension (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordExtension");

    DWORD ddrval = D3D_OK;
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2EXT, (*ppCommands)->wStateCount, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetClipPlane (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetClipPlane");

    DWORD ddrval;
    DWORD dwSize = sizeof(D3DHAL_DP2COMMAND) +
                   (*ppCommands)->wStateCount * sizeof(D3DHAL_DP2SETCLIPPLANE);
    ddrval = recordState (pContext, (LPBYTE)(*ppCommands), dwSize);
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordSetClipPlane failed");
    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETCLIPPLANE, (*ppCommands)->wStateCount, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetVertexShader (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetVertexShader");

    DWORD ddrval;
    ddrval = recordLastStateOnly (pContext, (*ppCommands), sizeof(D3DHAL_DP2VERTEXSHADER));
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvDP2RecordSetVertexShader failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2VERTEXSHADER, 1, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetVertexShaderConst (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    DWORD ddrval,count ;
    DWORD dwSize = sizeof(D3DHAL_DP2COMMAND) ;
    D3DHAL_DP2SETVERTEXSHADERCONST *lpSetVShaderConst;


    dbgTracePush ("nvDP2RecordSetVertexShaderConst");

    for (count=0; count<(*ppCommands)->wStateCount; count++) 
    {
        lpSetVShaderConst = (D3DHAL_DP2SETVERTEXSHADERCONST *)(lpPrim);
        dwSize += lpSetVShaderConst->dwCount * sizeof(VSHADERREGISTER) + sizeof(D3DHAL_DP2SETVERTEXSHADERCONST);   
        lpPrim += (lpSetVShaderConst->dwCount * sizeof(VSHADERREGISTER) ) + sizeof(D3DHAL_DP2SETVERTEXSHADERCONST);
    
    
    }
        ddrval = recordState (pContext, (LPBYTE)(*ppCommands), dwSize);
        if (ddrval != D3D_OK) 
        {
            DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvDP2RecordSetVertexShaderConst failed");
        }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETVERTEXSHADERCONST, 0, (dwSize - sizeof(D3DHAL_DP2COMMAND) )); //dwSize has everything

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetStreamSource (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetStreamSource");

    DWORD ddrval;
    DWORD dwSize = sizeof(D3DHAL_DP2COMMAND) + (*ppCommands)->wStateCount * sizeof(D3DHAL_DP2SETSTREAMSOURCE);
    ddrval = recordState (pContext, (LPBYTE)(*ppCommands), dwSize);
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordRenderState failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETSTREAMSOURCE, (*ppCommands)->wPrimitiveCount, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetStreamSourceUM (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetStreamSourceUM");

    DWORD ddrval;
    DWORD dwSize = sizeof(D3DHAL_DP2COMMAND) + (*ppCommands)->wStateCount * sizeof(D3DHAL_DP2SETSTREAMSOURCE);
    ddrval = recordState (pContext, (LPBYTE)(*ppCommands), dwSize);
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordRenderState failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETSTREAMSOURCEUM, (*ppCommands)->wPrimitiveCount, 0);

    //this is not a valid call because the UM needs to be mapped on a per call basis
    nvAssert(0);
    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetIndices (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetIndices");

    DWORD ddrval;
    ddrval = recordLastStateOnly (pContext, (*ppCommands), sizeof(D3DHAL_DP2SETINDICES));
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvDP2RecordSetIndices failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETINDICES, 1, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetPixelShader (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2RecordSetPixelShader");

    DWORD ddrval;
    ddrval = recordLastStateOnly (pContext, (*ppCommands), sizeof(D3DHAL_DP2PIXELSHADER));
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordSetPixelShader failed");
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2PIXELSHADER, 1, 0);

    recordDebugMarker (pContext);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordSetPixelShaderConst (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    DWORD dwExtraBytes=0,dwSize=sizeof(D3DHAL_DP2COMMAND),ddrval,count;
    

    dbgTracePush ("nvDP2RecordSetPixelShaderConst");

    for (count=0; count<(*ppCommands)->wStateCount; count++) 
    {
        LPD3DHAL_DP2SETPIXELSHADERCONST lpConstInfo = (D3DHAL_DP2SETPIXELSHADERCONST *)(lpPrim);

        lpPrim += 4*4*lpConstInfo->dwCount + sizeof(D3DHAL_DP2SETPIXELSHADERCONST);
        dwSize += 4*4*lpConstInfo->dwCount + sizeof(D3DHAL_DP2SETPIXELSHADERCONST);
        dwExtraBytes += 4*4*lpConstInfo->dwCount;
    }
    
    ddrval = recordState (pContext, (LPBYTE)(*ppCommands), dwSize);
    if (ddrval != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvRecordRenderState failed");

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETPIXELSHADERCONST, (*ppCommands)->wStateCount, dwExtraBytes );
    recordDebugMarker (pContext);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2RecordMultiplyTransform (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2MULTIPLYTRANSFORM *lpMultTrans = (D3DHAL_DP2MULTIPLYTRANSFORM *)(lpPrim);

    dbgTracePush ("nvDP2MultiplyTransform");

    nvAssert(0);
    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2MULTIPLYTRANSFORM, 1, 0 );

    recordDebugMarker (pContext);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------
// DP2 Capture Functions
//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetRenderState (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE  lpPrim;
    DWORD   i, dwState;

    dbgTracePush ("nvDP2CaptureSetRenderState");

    lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    // Run through the render state list and pull values from current context.
    for (i = 0; i < (*ppCommands)->wPrimitiveCount; i++) {
        dwState = ((LPD3DHAL_DP2RENDERSTATE)lpPrim)->RenderState;
        if (dwState == D3DRENDERSTATE_TEXTUREHANDLE) {
            // BUGBUG
            DPF ("ack. this is messed up. need to back-translate texture handle");
        }
        ((LPD3DHAL_DP2RENDERSTATE)lpPrim)->dwState = pContext->dwRenderState[dwState];
        lpPrim += sizeof(D3DHAL_DP2RENDERSTATE);
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2RENDERSTATE, (*ppCommands)->wPrimitiveCount, 0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetTextureStageState (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE  lpPrim;
    BOOL    bFilterChanged, bTSSChanged;
    DWORD   i, dwStage, dwState;
    HRESULT ddrval = D3D_OK;

    dbgTracePush ("nvDP2CaptureSetTextureStageState");

    lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    bFilterChanged = FALSE;
    bTSSChanged = FALSE;

    for (i = 0; i < (*ppCommands)->wPrimitiveCount; i++) {
        dwStage = (DWORD)((LPD3DHAL_DP2TEXTURESTAGESTATE)lpPrim)->wStage;
        dwState = ((LPD3DHAL_DP2TEXTURESTAGESTATE)lpPrim)->TSState;
        if (dwState == D3DTSS_TEXTUREMAP) {
            // BUGBUG
            DPF ("ack. this is messed up. need to back-translate texture handle");
        }
        ((LPD3DHAL_DP2TEXTURESTAGESTATE)lpPrim)->dwValue = pContext->tssState[dwStage].dwValue[dwState];
        lpPrim += sizeof(D3DHAL_DP2TEXTURESTAGESTATE);
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2TEXTURESTAGESTATE, (*ppCommands)->wPrimitiveCount, 0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetViewport (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim;

    dbgTracePush ("nvDP2CaptureSetViewport");

    lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    ((LPD3DHAL_DP2VIEWPORTINFO)lpPrim)->dwX      = pContext->surfaceViewport.clipHorizontal.wX;
    ((LPD3DHAL_DP2VIEWPORTINFO)lpPrim)->dwY      = pContext->surfaceViewport.clipVertical.wY;
    ((LPD3DHAL_DP2VIEWPORTINFO)lpPrim)->dwWidth  = pContext->surfaceViewport.clipHorizontal.wWidth;
    ((LPD3DHAL_DP2VIEWPORTINFO)lpPrim)->dwHeight = pContext->surfaceViewport.clipVertical.wHeight;

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2VIEWPORTINFO, 1, 0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetWRange (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim;

    dbgTracePush ("nvDP2CaptureSetWRange");

    lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    ((LPD3DHAL_DP2WINFO)lpPrim)->dvWNear = pContext->surfaceViewport.dvWNear;
    ((LPD3DHAL_DP2WINFO)lpPrim)->dvWFar  = pContext->surfaceViewport.dvWFar;

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2WINFO, 1, 0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetZRange (NV_DP2FUNCTION_ARGLIST)
{
    LPD3DHAL_DP2ZRANGE pZRange;

    dbgTracePush ("nvDP2CaptureSetZRange");

    // we only care about the last zrange they set
    pZRange = (D3DHAL_DP2ZRANGE *)((*ppCommands) + 1) + ((*ppCommands)->wStateCount - 1);
    pZRange->dvMinZ = pContext->surfaceViewport.dvMinZ;
    pZRange->dvMaxZ = pContext->surfaceViewport.dvMaxZ;

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2ZRANGE, (*ppCommands)->wStateCount, 0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetMaterial (NV_DP2FUNCTION_ARGLIST)
{
    LPD3DHAL_DP2SETMATERIAL pSetMat;

    dbgTracePush ("nvDP2CaptureSetMaterial");

    // we only care about the last material they set
    pSetMat = (D3DHAL_DP2SETMATERIAL *)((*ppCommands) + 1) + ((*ppCommands)->wStateCount - 1);
    *(D3DMATERIAL7 *)pSetMat = pContext->Material;

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETMATERIAL, (*ppCommands)->wStateCount, 0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetLight (NV_DP2FUNCTION_ARGLIST)
{
    HRESULT hr = D3D_OK;
    WORD wNumSetLight = (*ppCommands)->wStateCount;
    LPD3DHAL_DP2SETLIGHT pSetLight;
    D3DLIGHT7 *pLightData;
    DWORD dwTotalStride;
    int i;

    dbgTracePush ("nvDP2CaptureSetLight");

    dwTotalStride = sizeof(D3DHAL_DP2COMMAND);
    pSetLight = (LPD3DHAL_DP2SETLIGHT)((*ppCommands) + 1);
    pLightData = NULL;

    for (i=0; i < wNumSetLight; i++) {

        DWORD dwStride = sizeof(D3DHAL_DP2SETLIGHT);
        DWORD dwIndex  = pSetLight->dwIndex;

        // Assert that create was not called here
        assert (dwIndex < pContext->dwLightArraySize);

        switch (SETLIGHT_DATATYPE(pSetLight)) {
            case D3DHAL_SETLIGHT_ENABLE:
            case D3DHAL_SETLIGHT_DISABLE:
                if (nvLightIsEnabled(&(pContext->pLightArray[dwIndex]))) {
                    SETLIGHT_DATATYPE(pSetLight) = D3DHAL_SETLIGHT_ENABLE;
                }
                else {
                    SETLIGHT_DATATYPE(pSetLight) = D3DHAL_SETLIGHT_DISABLE;
                }
                break;
            case D3DHAL_SETLIGHT_DATA:
                pLightData = (D3DLIGHT7 *)((LPBYTE)pSetLight + dwStride);
                *pLightData = pContext->pLightArray[pSetLight->dwIndex].Light7;
                dwStride += sizeof(D3DLIGHT7);
                break;
            default:
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "Unknown SetLight command");
                hr = DDERR_INVALIDPARAMS;
        }

        dwTotalStride += dwStride;
        // Update the command buffer pointer
        pSetLight = (D3DHAL_DP2SETLIGHT *)((LPBYTE)pSetLight + dwStride);
    }

    if (hr != D3D_OK) DPF_LEVEL(NVDBG_LEVEL_ERROR,"D3D: nvDP2SetLight failed");
    (*ppCommands) = (LPD3DHAL_DP2COMMAND)((LPBYTE)(*ppCommands) + dwTotalStride);

    dbgTracePop();
    return (hr);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetTransform (NV_DP2FUNCTION_ARGLIST)
{
    LPD3DHAL_DP2SETTRANSFORM   pSetXfrm;
    WORD                       wNumXfrms;
    D3DTRANSFORMSTATETYPE      xfrmType;
    D3DMATRIX                  *pMat;
    DWORD                      dwXfrmType, i, dwStage;

    dbgTracePush ("nvDP2CaptureSetTransform");

    pSetXfrm = (LPD3DHAL_DP2SETTRANSFORM)((*ppCommands) + 1);
    wNumXfrms = (*ppCommands)->wStateCount;

    for (i=0; i < (int) wNumXfrms; i++) {

        xfrmType   = pSetXfrm->xfrmType;
        dwXfrmType = (DWORD)xfrmType & (~0x80000000);
        pMat       = &pSetXfrm->matrix;

        switch (dwXfrmType) {

            case D3DTRANSFORMSTATE_WORLD_DX7:
            case D3DTS_WORLD:
                nvMemCopy (pMat, &(pContext->xfmWorld[0]), sizeof(D3DMATRIX));
                break;
            case D3DTRANSFORMSTATE_WORLD1_DX7:
            case D3DTS_WORLD1:
                nvMemCopy (pMat, &(pContext->xfmWorld[1]), sizeof(D3DMATRIX));
                break;
            case D3DTRANSFORMSTATE_WORLD2_DX7:
            case D3DTS_WORLD2:
                nvMemCopy (pMat, &(pContext->xfmWorld[2]), sizeof(D3DMATRIX));
                break;
            case D3DTRANSFORMSTATE_WORLD3_DX7:
            case D3DTS_WORLD3:
                nvMemCopy (pMat, &(pContext->xfmWorld[3]), sizeof(D3DMATRIX));
                break;
            case D3DTRANSFORMSTATE_VIEW:
                nvMemCopy (pMat, &(pContext->xfmView), sizeof(D3DMATRIX));
                break;
            case D3DTRANSFORMSTATE_PROJECTION:
                nvMemCopy (pMat, &(pContext->xfmProj), sizeof(D3DMATRIX));
                break;
            case D3DTRANSFORMSTATE_TEXTURE0:
            case D3DTRANSFORMSTATE_TEXTURE1:
            case D3DTRANSFORMSTATE_TEXTURE2:
            case D3DTRANSFORMSTATE_TEXTURE3:
            case D3DTRANSFORMSTATE_TEXTURE4:
            case D3DTRANSFORMSTATE_TEXTURE5:
            case D3DTRANSFORMSTATE_TEXTURE6:
            case D3DTRANSFORMSTATE_TEXTURE7:
                dwStage = dwXfrmType - D3DTRANSFORMSTATE_TEXTURE0;
                nvMemCopy (pMat, &pContext->tssState[dwStage].mTexTransformMatrix, sizeof(D3DMATRIX));
                break;
            default:
                DPF_LEVEL (NVDBG_LEVEL_INFO,"Ignoring unknown transform type in SetXfrm");
                break;

        }  // switch (dwxfrmType)

        pSetXfrm++;

    } // for...

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETTRANSFORM, (*ppCommands)->wStateCount, 0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureExtension (NV_DP2FUNCTION_ARGLIST)
{
    dbgTracePush ("nvDP2CaptureExtension");

    DWORD ddrval = D3D_OK;
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2EXT, (*ppCommands)->wStateCount, 0);

    dbgTracePop();
    return (ddrval);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetClipPlane (NV_DP2FUNCTION_ARGLIST)
{
    LPD3DHAL_DP2SETCLIPPLANE pDp2ClipPlaneData;
    DWORD                    count, index;
    D3DVALUE                 *pClipPlane;

    dbgTracePush ("nvDP2CaptureSetClipPlane");

    pDp2ClipPlaneData = (LPD3DHAL_DP2SETCLIPPLANE)((*ppCommands)+1);

    for (count=0; count<(*ppCommands)->wStateCount; count++) {

        index = pDp2ClipPlaneData->dwIndex;
        if (index > D3DMAXUSERCLIPPLANES) {
            DPF ("clipplane index %d exceeded D3DMAXUSERCLIPPLANES in nvDP2CaptureSetClipPlane", index);
            pDp2ClipPlaneData ++;
            continue;
        }

        pClipPlane = pContext->ppClipPlane[index];

        pDp2ClipPlaneData->plane[0] = pClipPlane[0];
        pDp2ClipPlaneData->plane[1] = pClipPlane[1];
        pDp2ClipPlaneData->plane[2] = pClipPlane[2];
        pDp2ClipPlaneData->plane[3] = pClipPlane[3];

        pDp2ClipPlaneData ++;
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETCLIPPLANE, (*ppCommands)->wStateCount, 0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetVertexShader (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim;

    dbgTracePush ("nvDP2CaptureSetVertexShader");

    lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    
    ((LPD3DHAL_DP2VERTEXSHADER)lpPrim)->dwHandle = pContext->pCurrentVShader->isFvfShader() ?
        pContext->pCurrentVShader->getFVF():
        pContext->pCurrentVShader->getHandle();

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2VERTEXSHADER, 1, 0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetVertexShaderConst (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2SETVERTEXSHADERCONST *lpSetVShaderConst;
    DWORD   dwExtraBytes=0,count;

    dbgTracePush ("nvDP2CaptureSetVertexShaderConst");


    for (count=0; count<(*ppCommands)->wStateCount; count++) 
    {
        lpSetVShaderConst = (D3DHAL_DP2SETVERTEXSHADERCONST *)(lpPrim);
        VSHADERREGISTER *pConst = (VSHADERREGISTER*)(lpPrim + sizeof(D3DHAL_DP2SETVERTEXSHADERCONST));
    
        nvAssert(pContext->pVShaderConsts);
        nvAssert((lpSetVShaderConst->dwRegister + lpSetVShaderConst->dwCount) <= D3DVS_CONSTREG_MAX_V1_1);
        
        memcpy (pConst,
                &(pContext->pVShaderConsts->vertexShaderConstants[lpSetVShaderConst->dwRegister]),
                lpSetVShaderConst->dwCount * sizeof(VSHADERREGISTER));
        
        dwExtraBytes += lpSetVShaderConst->dwCount * sizeof(VSHADERREGISTER);   
        
        lpPrim += (lpSetVShaderConst->dwCount * sizeof(VSHADERREGISTER) ) + sizeof(D3DHAL_DP2SETVERTEXSHADERCONST);
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETVERTEXSHADERCONST, (*ppCommands)->wStateCount, dwExtraBytes);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetStreamSource (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2SETSTREAMSOURCE *lpStreamSource = (D3DHAL_DP2SETSTREAMSOURCE *)(lpPrim);
    DWORD count;

    dbgTracePush ("nvDP2CaptureSetStreamSource");

    for (count=0; count< (*ppCommands)->wStateCount; count++)
    {
        if (pContext->ppDX8Streams[lpStreamSource->dwStream])
        {
            lpStreamSource->dwVBHandle = pContext->ppDX8Streams[lpStreamSource->dwStream]->getWrapper()->getHandle();
        }
        lpStreamSource++;
    }
    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETSTREAMSOURCE, (*ppCommands)->wStateCount, 0 );

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetStreamSourceUM (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    D3DHAL_DP2SETSTREAMSOURCE *lpStreamSource = (D3DHAL_DP2SETSTREAMSOURCE *)(lpPrim);
    DWORD count;

    dbgTracePush ("nvDP2CaptureSetStreamSourceUM");

    for (count=0; count< (*ppCommands)->wStateCount; count++)
    {
        lpStreamSource->dwStream = 0; //need help here HMH
        lpStreamSource->dwVBHandle = 0; //need help here HMH
        lpStreamSource++;
    }
    //this is not a valid call because the UM needs to be mapped on a per call basis
    nvAssert(0);
    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETSTREAMSOURCE, (*ppCommands)->wStateCount, 0 );

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetIndices (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim;

    dbgTracePush ("nvDP2CaptureSetIndices");

    lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    if (pContext->pIndexBuffer) {
        ((LPD3DHAL_DP2SETINDICES)lpPrim)->dwVBHandle = pContext->pIndexBuffer->getWrapper()->getHandle(); //need help here HMH
    }

    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2SETINDICES, 1, 0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetPixelShader (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim;

    dbgTracePush ("nvDP2CaptureSetPixelShader");

    lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

    if (pContext->pCurrentPShader) {
        ((LPD3DHAL_DP2PIXELSHADER)lpPrim)->dwHandle = pContext->pCurrentPShader->getHandle();
    }
    else
    {
        ((LPD3DHAL_DP2PIXELSHADER)lpPrim)->dwHandle = 0;
    }
    NEXTINSTRUCTION((*ppCommands), D3DHAL_DP2PIXELSHADER, 1, 0);

    dbgTracePop();
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT nvDP2CaptureSetPixelShaderConst (NV_DP2FUNCTION_ARGLIST)
{
    LPBYTE lpPrim = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    DWORD dwExtraBytes=0,count;
    
    dbgTracePush ("nvDP2SetPixelShaderConst");

    for (count=0; count<(*ppCommands)->wStateCount; count++) 
    {
        LPD3DHAL_DP2SETPIXELSHADERCONST lpConstInfo = (D3DHAL_DP2SETPIXELSHADERCONST *)(lpPrim);
        D3DCOLORVALUE *lpConsts = (D3DCOLORVALUE *)(lpPrim + sizeof(D3DHAL_DP2SETPIXELSHADERCONST) );

        for (DWORD i=0; i<lpConstInfo->dwCount; i++) {
            lpConsts[i] = pContext->pixelShaderConsts[lpConstInfo->dwRegister+i];
        }
        lpPrim += 4*4*lpConstInfo->dwCount + sizeof(D3DHAL_DP2SETPIXELSHADERCONST);
        dwExtraBytes += 4*4*lpConstInfo->dwCount;
    }

    NEXTINSTRUCTION ((*ppCommands), D3DHAL_DP2SETPIXELSHADERCONST, (*ppCommands)->wStateCount, dwExtraBytes );

    dbgTracePop();
    return (D3D_OK);
}

#endif // NVARCH >= 0x04

