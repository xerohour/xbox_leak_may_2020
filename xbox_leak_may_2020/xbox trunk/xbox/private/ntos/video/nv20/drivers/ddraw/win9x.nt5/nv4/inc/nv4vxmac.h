/*
 * **** MACHINE GENERATED - DO NOT EDIT ****
 * **** MACHINE GENERATED - DO NOT EDIT ****
 * **** MACHINE GENERATED - DO NOT EDIT ****
 *
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

/*
 * Inner Loop Prototypes and Vertex Macros
 */

/*
 * General Constants
 */
#define NV_FORCE_TRI_SETUP(_ctx) (_ctx)->dwFunctionLookup = NV_SETUP_FUNCTION_INDEX;

#ifdef NV_FASTLOOPS
#define NV_DX6_FUNCTION_ADJUST      2
#define NV_FIXED_FUNCTION_INDEX     1024
#define NV_AA_FUNCTION_INDEX        1024
#define NV_SETUP_FUNCTION_INDEX     1025
#else //!NV_FASTLOOPS
#define NV_AA_FUNCTION_INDEX        8
#define NV_SETUP_FUNCTION_INDEX     10
#endif //!NV_FASTLOOPS

#define sizeDX5TriangleVertex       9
#define sizeDx5TriangleTLVertex     sizeDX5TriangleVertex

#define sizeDX6TriangleVertex       11
#define sizeDx6TriangleTLVertex     sizeDX6TriangleVertex

#define LIST_STRIDES                0x00000303
#define STRIP_STRIDES               0x01010101
#define FAN_STRIDES                 0x02000001
#define LEGACY_STRIDES              0x03000404

#ifdef NV_FASTLOOPS

void __stdcall nvTriangleDispatch (DWORD, LPWORD, DWORD, LPBYTE);
void __stdcall nvDX5TriangleSetup (DWORD, LPWORD, DWORD, LPBYTE);
void __stdcall nvDX6TriangleSetup (DWORD, LPWORD, DWORD, LPBYTE);

extern DWORD dwDrawPrimitiveTable[];

#else //!NV_FASTLOOPS

/*
 * Inner Loop Prototypes
 */
void nvDX5Triangle                            ( WORD,         DWORD, LPD3DTLVERTEX);
void nvDX5TriangleExpFog                      ( WORD,         DWORD, LPD3DTLVERTEX);
void nvDX5TriangleExp2Fog                     ( WORD,         DWORD, LPD3DTLVERTEX);
void nvDX5TriangleLinearFog                   ( WORD,         DWORD, LPD3DTLVERTEX);
void nvDX5TriangleWBuf                        ( WORD,         DWORD, LPD3DTLVERTEX);
void nvDX5TriangleExpFogWBuf                  ( WORD,         DWORD, LPD3DTLVERTEX);
void nvDX5TriangleExp2FogWBuf                 ( WORD,         DWORD, LPD3DTLVERTEX);
void nvDX5TriangleLinearFogWBuf               ( WORD,         DWORD, LPD3DTLVERTEX);
void nvDX5TriangleIndexed                     (DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDX5TriangleExpFogIndexed               (DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDX5TriangleExp2FogIndexed              (DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDX5TriangleLinearFogIndexed            (DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDX5TriangleIndexedWBuf                 (DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDX5TriangleExpFogIndexedWBuf           (DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDX5TriangleExp2FogIndexedWBuf          (DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDX5TriangleLinearFogIndexedWBuf        (DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDX5TriangleAACapture                   ( WORD,         DWORD, LPD3DTLVERTEX);
void nvDX5TriangleAACaptureWBuf               ( WORD,         DWORD, LPD3DTLVERTEX);
void nvDX5TriangleIndexedAACapture            (DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDX5TriangleIndexedAACaptureWBuf        (DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDX5FlexTriangle                        ( WORD,         DWORD, LPBYTE);
void nvDX5FlexTriangleExpFog                  ( WORD,         DWORD, LPBYTE);
void nvDX5FlexTriangleExp2Fog                 ( WORD,         DWORD, LPBYTE);
void nvDX5FlexTriangleLinearFog               ( WORD,         DWORD, LPBYTE);
void nvDX5FlexTriangleWBuf                    ( WORD,         DWORD, LPBYTE);
void nvDX5FlexTriangleExpFogWBuf              ( WORD,         DWORD, LPBYTE);
void nvDX5FlexTriangleExp2FogWBuf             ( WORD,         DWORD, LPBYTE);
void nvDX5FlexTriangleLinearFogWBuf           ( WORD,         DWORD, LPBYTE);
void nvDX5FlexTriangleIndexed                 (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX5FlexTriangleExpFogIndexed           (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX5FlexTriangleExp2FogIndexed          (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX5FlexTriangleLinearFogIndexed        (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX5FlexTriangleIndexedWBuf             (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX5FlexTriangleExpFogIndexedWBuf       (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX5FlexTriangleExp2FogIndexedWBuf      (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX5FlexTriangleLinearFogIndexedWBuf    (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX5FlexTriangleAACapture               ( WORD,         DWORD, LPBYTE);
void nvDX5FlexTriangleAACaptureWBuf           ( WORD,         DWORD, LPBYTE);
void nvDX5FlexTriangleIndexedAACapture        (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX5FlexTriangleIndexedAACaptureWBuf    (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX6FlexTriangle                        ( WORD,         DWORD, LPBYTE);
void nvDX6FlexTriangleExpFog                  ( WORD,         DWORD, LPBYTE);
void nvDX6FlexTriangleExp2Fog                 ( WORD,         DWORD, LPBYTE);
void nvDX6FlexTriangleLinearFog               ( WORD,         DWORD, LPBYTE);
void nvDX6FlexTriangleWBuf                    ( WORD,         DWORD, LPBYTE);
void nvDX6FlexTriangleExpFogWBuf              ( WORD,         DWORD, LPBYTE);
void nvDX6FlexTriangleExp2FogWBuf             ( WORD,         DWORD, LPBYTE);
void nvDX6FlexTriangleLinearFogWBuf           ( WORD,         DWORD, LPBYTE);
void nvDX6FlexTriangleIndexed                 (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX6FlexTriangleExpFogIndexed           (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX6FlexTriangleExp2FogIndexed          (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX6FlexTriangleLinearFogIndexed        (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX6FlexTriangleIndexedWBuf             (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX6FlexTriangleExpFogIndexedWBuf       (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX6FlexTriangleExp2FogIndexedWBuf      (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX6FlexTriangleLinearFogIndexedWBuf    (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX6FlexTriangleAACapture               ( WORD,         DWORD, LPBYTE);
void nvDX6FlexTriangleAACaptureWBuf           ( WORD,         DWORD, LPBYTE);
void nvDX6FlexTriangleIndexedAACapture        (DWORD, LPWORD, DWORD, LPBYTE);
void nvDX6FlexTriangleIndexedAACaptureWBuf    (DWORD, LPWORD, DWORD, LPBYTE);

extern LPNVDRAWPRIM fnDX5Table[];
void nvDX5TriangleSetup  ( WORD,         DWORD, LPD3DTLVERTEX);
extern LPNVINDEXEDPRIM fnDX5IndexedTable[];
void nvDX5IndexedTriangleSetup  (DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
extern LPNVFVFDRAWPRIM fnDX5FlexTable[];
void nvDX5FlexTriangleSetup  ( WORD,         DWORD, LPBYTE);
extern LPNVFVFINDEXEDPRIM fnDX5FlexIndexedTable[];
void nvDX5FlexIndexedTriangleSetup  (DWORD, LPWORD, DWORD, LPBYTE);
extern LPNVFVFDRAWPRIM fnDX6FlexTable[];
void nvDX6FlexTriangleSetup  ( WORD,         DWORD, LPBYTE);
extern LPNVFVFINDEXEDPRIM fnDX6FlexIndexedTable[];
void nvDX6FlexIndexedTriangleSetup  (DWORD, LPWORD, DWORD, LPBYTE);

#endif //!NV_FASTLOOPS
/*
 * Vertex Macros
 */

#define nvglDX5TriangleVertex(fifo,freecount,ch,alias,vertex)\
{\
    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\
    memcpy ((void*)(fifo + 4),(void*)(vertex),32);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog  = fz * pCurrentContext->fFogTableDensity;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog  = fz * pCurrentContext->fFogTableDensity;\
    ffog *= ffog;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog = (fz >= pCurrentContext->fFogTableEnd) ? 0.0f :\
           (fz <= pCurrentContext->fFogTableStart) ? 1.0f :\
           (pCurrentContext->fFogTableEnd - fz) * pCurrentContext->fFogTableLinearScale;\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = (*(DWORD*)(vertex + 16));\
    dwTemp = (*(DWORD*)(vertex + 20));\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = (*(DWORD*)(vertex + 16));\
    dwTemp = (*(DWORD*)(vertex + 20));\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog  = fz * pCurrentContext->fFogTableDensity;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = (*(DWORD*)(vertex + 16));\
    dwTemp = (*(DWORD*)(vertex + 20));\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog  = fz * pCurrentContext->fFogTableDensity;\
    ffog *= ffog;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = (*(DWORD*)(vertex + 16));\
    dwTemp = (*(DWORD*)(vertex + 20));\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog = (fz >= pCurrentContext->fFogTableEnd) ? 0.0f :\
           (fz <= pCurrentContext->fFogTableStart) ? 1.0f :\
           (pCurrentContext->fFogTableEnd - fz) * pCurrentContext->fFogTableLinearScale;\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\
    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog  = fz * pCurrentContext->fFogTableDensity;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog  = fz * pCurrentContext->fFogTableDensity;\
    ffog *= ffog;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog = (fz >= pCurrentContext->fFogTableEnd) ? 0.0f :\
           (fz <= pCurrentContext->fFogTableStart) ? 1.0f :\
           (pCurrentContext->fFogTableEnd - fz) * pCurrentContext->fFogTableLinearScale;\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog  = fz * pCurrentContext->fFogTableDensity;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog  = fz * pCurrentContext->fFogTableDensity;\
    ffog *= ffog;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog = (fz >= pCurrentContext->fFogTableEnd) ? 0.0f :\
           (fz <= pCurrentContext->fFogTableStart) ? 1.0f :\
           (pCurrentContext->fFogTableEnd - fz) * pCurrentContext->fFogTableLinearScale;\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog  = fz * pCurrentContext->fFogTableDensity;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog  = fz * pCurrentContext->fFogTableDensity;\
    ffog *= ffog;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog = (fz >= pCurrentContext->fFogTableEnd) ? 0.0f :\
           (fz <= pCurrentContext->fFogTableStart) ? 1.0f :\
           (pCurrentContext->fFogTableEnd - fz) * pCurrentContext->fFogTableLinearScale;\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog  = fz * pCurrentContext->fFogTableDensity;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog  = fz * pCurrentContext->fFogTableDensity;\
    ffog *= ffog;\
    FP_EXP (ffog,ffog);\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
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
    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\
    *(DWORD*)(fifo + 16) = dwrhw = *(DWORD*)&frhw;\
    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\
    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\
    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\
    frhw = *(float*)&dwrhw;\
    FP_INV (fw, frhw);\
    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\
    fz = *(float *)&dwz;\
    ffog = (fz >= pCurrentContext->fFogTableEnd) ? 0.0f :\
           (fz <= pCurrentContext->fFogTableStart) ? 1.0f :\
           (pCurrentContext->fFogTableEnd - fz) * pCurrentContext->fFogTableLinearScale;\
    FP_NORM_TO_BIT24 (ifog,ffog);\
    dwTemp = (dwTemp & 0x00ffffff) | ifog;\
    *(DWORD*)(fifo + 24) = dwTemp;\
    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\
    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\
    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\
    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\
    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\
    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\
                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\
                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\
    fifo      += sizeDX6TriangleVertex * 4;\
    freecount -= sizeDX6TriangleVertex;\
}

