/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

#ifndef _nvvxmac_h_
#define _nvvxmac_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Inner Loop Prototypes and Vertex Macros
 */

/*
 * General Constants
 */
#define NV_FORCE_TRI_SETUP(_ctx) (_ctx)->dwFunctionLookup = NV_SETUP_FUNCTION_INDEX;

#define NV_DX6_FUNCTION_ADJUST      2
#define NV_FIXED_FUNCTION_INDEX     2048
#define NV_AA_FUNCTION_INDEX        2048
#define NV_SETUP_FUNCTION_INDEX     2049

#define sizeDX5TriangleVertex       9
#define sizeDx5TriangleTLVertex     sizeDX5TriangleVertex

#define sizeDX6TriangleVertex       11
#define sizeDx6TriangleTLVertex     sizeDX6TriangleVertex

/*
 * Vertex Macros
 */

#define nvglDX5TriangleVertex(fifo,freecount,ch,alias,vertex)\
{\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    memcpy ((void*)(fifo + 4),(void*)(vertex),32);\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5TriangleVertexExpFog(fifo,freecount,ch,alias,vertex)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = (*(DWORD*)(vertex + 12));\
    *(DWORD*)(fifo + 16) = dwrhw = dwTemp;\
    *(DWORD*)(fifo + 20) = (*(DWORD*)(vertex + 16));\
    dwTemp = (*(DWORD*)(vertex + 20));\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog  = fz * pContext->fFogTableDensity;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5TriangleVertexExp2Fog(fifo,freecount,ch,alias,vertex)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = (*(DWORD*)(vertex + 12));\
    *(DWORD*)(fifo + 16) = dwrhw = dwTemp;\
    *(DWORD*)(fifo + 20) = (*(DWORD*)(vertex + 16));\
    dwTemp = (*(DWORD*)(vertex + 20));\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog  = fz * pContext->fFogTableDensity;\
    ffog *= ffog;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5TriangleVertexLinearFog(fifo,freecount,ch,alias,vertex)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = (*(DWORD*)(vertex + 12));\
    *(DWORD*)(fifo + 16) = dwrhw = dwTemp;\
    *(DWORD*)(fifo + 20) = (*(DWORD*)(vertex + 16));\
    dwTemp = (*(DWORD*)(vertex + 20));\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog = (fz >= pContext->fFogTableEnd) ? 0.0f :\
           (fz <= pContext->fFogTableStart) ? 1.0f :\
           (pContext->fFogTableEnd - fz) * pContext->fFogTableLinearScale;\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5TriangleVertexWBuf(fifo,freecount,ch,alias,vertex)\
{\
    DWORD dwTemp;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = (*(DWORD*)(vertex +  8));\
    dwTemp = (*(DWORD*)(vertex + 12));\
    frhw = (FLOAT_FROM_DWORD(dwTemp)) * pContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = (*(DWORD*)(vertex + 16));\
    dwTemp = (*(DWORD*)(vertex + 20));\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5TriangleVertexExpFogWBuf(fifo,freecount,ch,alias,vertex)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = (*(DWORD*)(vertex + 12));\
    frhw = (FLOAT_FROM_DWORD(dwTemp)) * pContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = (*(DWORD*)(vertex + 16));\
    dwTemp = (*(DWORD*)(vertex + 20));\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog  = fz * pContext->fFogTableDensity;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5TriangleVertexExp2FogWBuf(fifo,freecount,ch,alias,vertex)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = (*(DWORD*)(vertex + 12));\
    frhw = (FLOAT_FROM_DWORD(dwTemp)) * pContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = (*(DWORD*)(vertex + 16));\
    dwTemp = (*(DWORD*)(vertex + 20));\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog  = fz * pContext->fFogTableDensity;\
    ffog *= ffog;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5TriangleVertexLinearFogWBuf(fifo,freecount,ch,alias,vertex)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = (*(DWORD*)(vertex + 12));\
    frhw = (FLOAT_FROM_DWORD(dwTemp)) * pContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = (*(DWORD*)(vertex + 16));\
    dwTemp = (*(DWORD*)(vertex + 20));\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog = (fz >= pContext->fFogTableEnd) ? 0.0f :\
           (fz <= pContext->fFogTableStart) ? 1.0f :\
           (pContext->fFogTableEnd - fz) * pContext->fFogTableLinearScale;\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5FlexTriangleVertex(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    *(DWORD*)(fifo + 16) = dwTemp;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5FlexTriangleVertexExpFog(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    *(DWORD*)(fifo + 16) = dwrhw = dwTemp;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog  = fz * pContext->fFogTableDensity;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5FlexTriangleVertexExp2Fog(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    *(DWORD*)(fifo + 16) = dwrhw = dwTemp;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog  = fz * pContext->fFogTableDensity;\
    ffog *= ffog;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5FlexTriangleVertexLinearFog(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    *(DWORD*)(fifo + 16) = dwrhw = dwTemp;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog = (fz >= pContext->fFogTableEnd) ? 0.0f :\
           (fz <= pContext->fFogTableStart) ? 1.0f :\
           (pContext->fFogTableEnd - fz) * pContext->fFogTableLinearScale;\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5FlexTriangleVertexWBuf(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    frhw = (FLOAT_FROM_DWORD(dwTemp)) * pContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5FlexTriangleVertexExpFogWBuf(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    frhw = (FLOAT_FROM_DWORD(dwTemp)) * pContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog  = fz * pContext->fFogTableDensity;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5FlexTriangleVertexExp2FogWBuf(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    frhw = (FLOAT_FROM_DWORD(dwTemp)) * pContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog  = fz * pContext->fFogTableDensity;\
    ffog *= ffog;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX5FlexTriangleVertexLinearFogWBuf(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    frhw = (FLOAT_FROM_DWORD(dwTemp)) * pContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog = (fz >= pContext->fFogTableEnd) ? 0.0f :\
           (fz <= pContext->fFogTableStart) ? 1.0f :\
           (pContext->fFogTableEnd - fz) * pContext->fFogTableLinearScale;\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(FALSE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX5TriangleVertex * 4;\
    freecount -= sizeDX5TriangleVertex;\
}

#define nvglDX6FlexTriangleVertex(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    *(DWORD*)(fifo +  0) = ((sizeDX6TriangleVertex-1)<<18) | ((ch)<<13) | NV055_TLMTVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    *(DWORD*)(fifo + 16) = dwTemp;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(TRUE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX6TriangleVertex * 4;\
    freecount -= sizeDX6TriangleVertex;\
}

#define nvglDX6FlexTriangleVertexExpFog(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX6TriangleVertex-1)<<18) | ((ch)<<13) | NV055_TLMTVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    *(DWORD*)(fifo + 16) = dwrhw = dwTemp;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog  = fz * pContext->fFogTableDensity;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(TRUE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX6TriangleVertex * 4;\
    freecount -= sizeDX6TriangleVertex;\
}

#define nvglDX6FlexTriangleVertexExp2Fog(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX6TriangleVertex-1)<<18) | ((ch)<<13) | NV055_TLMTVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    *(DWORD*)(fifo + 16) = dwrhw = dwTemp;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog  = fz * pContext->fFogTableDensity;\
    ffog *= ffog;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(TRUE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX6TriangleVertex * 4;\
    freecount -= sizeDX6TriangleVertex;\
}

#define nvglDX6FlexTriangleVertexLinearFog(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX6TriangleVertex-1)<<18) | ((ch)<<13) | NV055_TLMTVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    *(DWORD*)(fifo + 16) = dwrhw = dwTemp;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog = (fz >= pContext->fFogTableEnd) ? 0.0f :\
           (fz <= pContext->fFogTableStart) ? 1.0f :\
           (pContext->fFogTableEnd - fz) * pContext->fFogTableLinearScale;\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(TRUE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX6TriangleVertex * 4;\
    freecount -= sizeDX6TriangleVertex;\
}

#define nvglDX6FlexTriangleVertexWBuf(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX6TriangleVertex-1)<<18) | ((ch)<<13) | NV055_TLMTVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    frhw = (FLOAT_FROM_DWORD(dwTemp)) * pContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(TRUE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX6TriangleVertex * 4;\
    freecount -= sizeDX6TriangleVertex;\
}

#define nvglDX6FlexTriangleVertexExpFogWBuf(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX6TriangleVertex-1)<<18) | ((ch)<<13) | NV055_TLMTVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    frhw = (FLOAT_FROM_DWORD(dwTemp)) * pContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog  = fz * pContext->fFogTableDensity;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(TRUE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX6TriangleVertex * 4;\
    freecount -= sizeDX6TriangleVertex;\
}

#define nvglDX6FlexTriangleVertexExp2FogWBuf(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX6TriangleVertex-1)<<18) | ((ch)<<13) | NV055_TLMTVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    frhw = (FLOAT_FROM_DWORD(dwTemp)) * pContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog  = fz * pContext->fFogTableDensity;\
    ffog *= ffog;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(TRUE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX6TriangleVertex * 4;\
    freecount -= sizeDX6TriangleVertex;\
}

#define nvglDX6FlexTriangleVertexLinearFogWBuf(fifo,freecount,ch,alias,vertex,fvf,uvoff)\
{\
    DWORD dwTemp;\
    DWORD zmask,dwrhw,dwz;\
    int   ifog;\
    float ffog,fz,fw;\
    float frhw;\
    *(DWORD*)(fifo +  0) = ((sizeDX6TriangleVertex-1)<<18) | ((ch)<<13) | NV055_TLMTVERTEX((alias));\
    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\
    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\
    *(DWORD*)(fifo + 12) = dwz = (*(DWORD*)(vertex +  8));\
    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\
    frhw = (FLOAT_FROM_DWORD(dwTemp)) * pContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwDiffuseMask) + fvf.dwDiffuseOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = FLOAT_FROM_DWORD(dwrhw);\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = FLOAT_FROM_DWORD(dwz);\
    ffog = (fz >= pContext->fFogTableEnd) ? 0.0f :\
           (fz <= pContext->fFogTableStart) ? 1.0f :\
           (pContext->fFogTableEnd - fz) * pContext->fFogTableLinearScale;\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset[0];\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(TRUE, &(((DWORD*)nvFifo)[1]));\
    fifo      += sizeDX6TriangleVertex * 4;\
    freecount -= sizeDX6TriangleVertex;\
}

#ifdef __cplusplus
}
#endif

#endif // _nvvxmac_h_
