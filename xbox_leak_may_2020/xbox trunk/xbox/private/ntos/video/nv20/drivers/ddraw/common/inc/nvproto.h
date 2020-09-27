/*
 * Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV_PROTO.H                                                        *
*       Function Prototypes                                                 *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler         11Feb99           created                  *
*                                                                           *
\***************************************************************************/

#ifndef _NVPROTO_H
#define _NVPROTO_H

#ifdef __cplusplus
extern "C" {
#endif

// d3dctxt.cpp
BOOL            nvCreateContextListHeap (GLOBALDATA *pDriverData);
BOOL            nvDestroyContextListHeap (void);
DWORD __stdcall nvContextCreate (LPD3DHAL_CONTEXTCREATEDATA);
DWORD __stdcall nvContextDestroy (LPD3DHAL_CONTEXTDESTROYDATA);
DWORD __stdcall nvContextDestroyAll (LPD3DHAL_CONTEXTDESTROYALLDATA);
void            nvD3DRecover(void);
void            nvDDrawRecover(void);
DWORD __stdcall nvDestroyDDLocal (LPDDHAL_DESTROYDDLOCALDATA);
PNVD3DCONTEXT   nvGetPreapprovedContext(void);
void            nvDestroyPreapprovedContexts(void);

// d3drendr.cpp
#ifndef WINNT  // these calls are obsolete on NT (DX7)
DWORD   __stdcall nvRenderState              (LPD3DHAL_RENDERSTATEDATA);
DWORD   __stdcall nvRenderPrimitive          (LPD3DHAL_RENDERPRIMITIVEDATA);
#endif // !WINNT
DWORD   __stdcall nvSetRenderTarget          (LPD3DHAL_SETRENDERTARGETDATA);
BOOL              nvD3DClearZBuffer          (LPDDRAWI_DDRAWSURFACE_LCL, DWORD, DWORD,DWORD,DWORD,DWORD);
BOOL              nvCheckBufferCompatibility (CSimpleSurface *pRenderTarget, CSimpleSurface *pZetaBuffer,
                                              LPDDRAWI_DDRAWSURFACE_LCL pDDSLclZ, DWORD dwDXAppVersion);
DWORD             nvSetRenderTargetDX7       (PNVD3DCONTEXT pContext, DWORD dwTargetHandle, DWORD dwZBufferHandle);

// D3Dmini.cpp
extern BOOL __stdcall D3DHALCreateDriver     (LPD3DHAL_GLOBALDRIVERDATA lpGlobal,
                                              LPD3DHAL_CALLBACKS* lplpHALCallbacks,
                                              LPDDHAL_DDEXEBUFCALLBACKS* lplpBufCallbacks,
                                              GLOBALDATA* pDriverData);
// d3dstate.cpp
DWORD __stdcall nvGetDriverState(LPDDHAL_GETDRIVERSTATEDATA);
#ifndef WINNT  // these calls are obsolete on NT (DX7)
DWORD __stdcall nvGetState(LPD3DHAL_GETSTATEDATA);
#endif

DWORD __stdcall nvTextureCreate  (LPD3DHAL_TEXTURECREATEDATA);
DWORD __stdcall nvTextureDestroy (LPD3DHAL_TEXTUREDESTROYDATA);
DWORD __stdcall nvTextureSwap    (LPD3DHAL_TEXTURESWAPDATA);
DWORD __stdcall nvTextureGetSurf (LPD3DHAL_TEXTUREGETSURFDATA);

// d3dtex.cpp / nvtext.cpp
BOOL  nvPCICreateHeap  (void);
BOOL  nvPCIDestroyHeap (void);
DWORD nvPCIAlloc       (DWORD);
BOOL  nvPCIFree        (DWORD);

// nvAGP.cpp
BOOL  nvAGPCreateHeap   (void);
BOOL  nvAGPDestroyHeap  (void);
void  nvAGPResetHeap    (void);
void  nvAGPInvalidate   (void);
void* nvAGPAlloc        (DWORD dwSize);
DWORD nvAGPGetUserAddr  (DWORD dwAddr);
void  nvAGPFree         (void *pMemory);
void  nvAGPCombine      (void);
void  nvAGPReclaim      (void);
void  nvAGPGetMemory    (DWORD *pdwTotal, DWORD *pdwFree);

// dddrv32.cpp
DWORD __stdcall       GetDriverInfo32           (LPDDHAL_GETDRIVERINFODATA lpData);
DWORD __stdcall       Blit32                    (LPDDHAL_BLTDATA pbd );
DWORD                 WaitForIdle               (BOOL bWait, BOOL bUseEvent);
BOOL                  ResetTwinViewState        (GLOBALDATA *pDriverData);

#ifndef WINNT
void  __stdcall       AddModeToTable            (DDHALMODEINFO *pMode, int nXRes, int nYRes, int nBitsPerPixel);
int   __stdcall       SortDDHalModeList         (void);
DWORD __stdcall       DrawOneIndexedPrimitive32 (LPD3DHAL_DRAWONEINDEXEDPRIMITIVEDATA);
DWORD __stdcall       DrawOnePrimitive32        (LPD3DHAL_DRAWONEPRIMITIVEDATA);
DWORD __stdcall       DrawPrimitives32          (LPD3DHAL_DRAWPRIMITIVESDATA);
int                   DrvCallbackFunc           (DWORD dwFunction, void *pIn, void *pOut);
BOOL                  MyExtEscape               (DWORD dwHead, int nEscapeID, int nInBytes, LPCSTR lpIn, int nOutBytes, LPSTR lpOut);
int                   getAdapterIndex           (DISPDRVDIRECTXCOMMON *pNewDXShare, DWORD *pdwRootIndex);
DISPDRVDIRECTXCOMMON* getDXShare                (DWORD dwRootHandle, DWORD dwHeadNumber);
BOOL                  nvFillAdapterTable        (void);
HDC                   nvCreateDCHandle          (DISPDRVDIRECTXCOMMON *pDXS);
void                  nvDestroyDCHandle         (HDC hDC);
DWORD __stdcall		  GetModePitch				(int nXRes, int nYRes, int nBitsPerPixel);
#endif  // !WINNT

// nvCaps.cpp
void  nvSetHardwareCaps (void);

// nvctxt.cpp
BOOL  nvSetSurfaceViewport (PNVD3DCONTEXT);
void  nvRefreshSurfaceInfo (PNVD3DCONTEXT);
BOOL  nvSetupContext       (PNVD3DCONTEXT);
void  nvInitTLData         (PNVD3DCONTEXT pContext);
#if (NVARCH >= 0x010)
void  nvInitCelsiusData    (PNVD3DCONTEXT pContext);
#endif  // NVARCH >= 0x010
#if (NVARCH >= 0x020)
void  nvInitKelvinData     (PNVD3DCONTEXT pContext);
#endif  // NVARCH >= 0x020

// nvline.cpp
DWORD nvRenderLine                  (NV_INNERLOOP_ARGLIST);
void  nvDrawLine                    (NV_INNERLOOP_ARGLIST);
void  nvIndexedLine                 (NV_INNERLOOP_ARGLIST);
void  nvIndexedWireframeTriangle    (NV_INNERLOOP_ARGLIST);
void  nvDrawWireframeTriangle       (NV_INNERLOOP_ARGLIST);
void  nvFVFDrawLine                 (NV_INNERLOOP_ARGLIST);
void  nvFVFIndexedLine              (NV_INNERLOOP_ARGLIST);
void  nvFVFIndexedWireframeTriangle (NV_INNERLOOP_ARGLIST);
void  nvFVFDrawWireframeTriangle    (NV_INNERLOOP_ARGLIST);

// nvmini.cpp
void  D3DReadRegistry       (GLOBALDATA *pDriverData);
void  D3DModifyCapabilities (D3DHAL_GLOBALDRIVERDATA *pNvGlobal, GLOBALDATA *pDriverData);
BOOL  CreateItemArrays      (GLOBALDATA *pDriverData);
HKEY  OpenLocalD3DRegKey    (void);

BOOL nvReadRegistryDWORD (
#ifdef WINNT
    PDEV   *pDev,
#else
    HKEY    hKey,
#endif
    LPCTSTR lpValueName,
    LPDWORD lpData
);

// nvpoint.cpp
DWORD nvRenderPoint             (NV_INNERLOOP_ARGLIST);
void  nvDrawPointList           (NV_INNERLOOP_ARGLIST);
void  nvIndexedPointTriangle    (NV_INNERLOOP_ARGLIST);
void  nvDrawPointTriangle       (NV_INNERLOOP_ARGLIST);
void  nvFVFDrawPointList        (NV_INNERLOOP_ARGLIST);
void  nvFVFIndexedPointTriangle (NV_INNERLOOP_ARGLIST);
void  nvFVFDrawPointTriangle    (NV_INNERLOOP_ARGLIST);

// nvprim.cpp
#ifndef WINNT
DWORD nvDrawOnePrimitive        (LPD3DHAL_DRAWONEPRIMITIVEDATA);
DWORD nvDrawPrimitives          (LPD3DHAL_DRAWPRIMITIVESDATA);
DWORD nvDrawOneIndexedPrimitive (LPD3DHAL_DRAWONEINDEXEDPRIMITIVEDATA);
#endif  // !WINNT

// nvPatch.cpp
DWORD nvDrawRectSurface (PNVD3DCONTEXT pContext, DWORD dwHandle,
                         float *pfSegs, D3DRECTPATCH_INFO *pRSInfo);
DWORD nvDrawTriSurface  (PNVD3DCONTEXT pContext, DWORD dwHandle,
                         float *pfSegs, D3DTRIPATCH_INFO *pRSInfo);

// nvprim2.cpp
DWORD __stdcall nvDrawPrimitives2 (LPD3DHAL_DRAWPRIMITIVES2DATA);

// nvprof.cpp
#ifdef __cplusplus
extern "C" {
#endif
#ifdef WINNT
BOOL  __stdcall nvDeterminePerformanceStrategy (GLOBALDATA *pDriverData, PDEV* ppdev);
#else // !WINNT
BOOL  __stdcall nvDeterminePerformanceStrategy (GLOBALDATA *pDriverData);
#endif // !WINNT
void nvDetermineEarlyCopyStrategy(PNVD3DCONTEXT pContext);
#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef NV_PROFILE
void nvpCreate   (void);
void nvpDestroy  (void);
void nvpFlush    (void);
void nvpLogEvent (DWORD id);
void nvpLogCount (DWORD id,int count);
void nvpLogTime  (DWORD id,__int64 time);
void nvpSetLogPusher(bool log);
bool nvpGetLogPusher();
#endif  // NV_PROFILE

// nvscene.cpp
DWORD __stdcall nvSceneCapture (LPD3DHAL_SCENECAPTUREDATA);

// nvstate.cpp
DWORD            CTFunc                         (PNVD3DCONTEXT pContext, DWORD dw);
BOOL             nvStencilBufferExists          (PNVD3DCONTEXT pContext);
DWORD            nvTranslateLODBias             (DWORD);
void             nvSetD3DSurfaceState           (PNVD3DCONTEXT);
void             nvSetD3DSurfaceViewport        (PNVD3DCONTEXT);
void             nvSetDx5TriangleState          (PNVD3DCONTEXT);
DWORD            nvSetContextState              (PNVD3DCONTEXT pContext, DWORD, DWORD, HRESULT *);
BOOL             nvSetHardwareState             (PNVD3DCONTEXT pContext);
void             nvSetDx6MultiTextureState      (PNVD3DCONTEXT);
DWORD FAR PASCAL nvValidateTextureStageState    (LPD3DHAL_VALIDATETEXTURESTAGESTATEDATA);
void             nvSetMultiTextureHardwareState (PNVD3DCONTEXT pContext);
void             cacheFogValues                 (PNVD3DCONTEXT pContext);

// nvstateset.cpp
#ifdef DEBUG
void    recordDebugMarker   (PNVD3DCONTEXT pContext);
#else
#define recordDebugMarker(a)
#endif
HRESULT recordState         (PNVD3DCONTEXT pContext, LPBYTE, DWORD);
HRESULT recordLastStateOnly (PNVD3DCONTEXT pContext, LPD3DHAL_DP2COMMAND, DWORD);
HRESULT stateSetBegin       (PNVD3DCONTEXT, DWORD);
HRESULT stateSetEnd         (PNVD3DCONTEXT);
HRESULT stateSetExecute     (PNVD3DCONTEXT, DWORD);
HRESULT stateSetCapture     (PNVD3DCONTEXT, DWORD);
HRESULT stateSetCreate      (PNVD3DCONTEXT, DWORD, D3DSTATEBLOCKTYPE);
HRESULT stateSetDelete      (PNVD3DCONTEXT, DWORD);

// nvtri.cpp
DWORD   nvRenderNoOp     (NV_INNERLOOP_ARGLIST);
DWORD   nvRenderTriangle (NV_INNERLOOP_ARGLIST);

// nvlight.cpp
HRESULT nvLightSetLight  (nvLight *, LPD3DLIGHT7);
void    nvLightEnable    (nvLight *, nvLight **);
void    nvLightDisable   (nvLight *, nvLight **);
HRESULT nvGrowLightArray (PNVD3DCONTEXT, DWORD);
BOOL    nvLightIsEnabled (nvLight *);
BOOL    nvLightIsDefined (nvLight *);

// nvRZ.cpp
BOOL            rzDetectCachingCapabilities (void);
BOOL  __stdcall rzSetAGPMTRR                (DWORD dwPhysBase,DWORD dwType);
DWORD           rzMapPhysicalToLinear       (DWORD dwPhysBase,DWORD dwLength);
BOOL            rzSetPATIndices             (DWORD dwStart,DWORD dwCount,DWORD dwValue);

#if (NVARCH >= 0x010)

// nvCelsiusAlphaCombiner.cpp
void    SetAlphaInputOne (PNVD3DCONTEXT pContext, celsiusCombinerInput eInput,
                          DWORD dwCombinerStage, BOOL bNegate);
HRESULT SelectAlphaInput (PNVD3DCONTEXT pContext, celsiusCombinerInput eInput,
                          DWORD dwCombinerStage, DWORD dwD3DStage, DWORD dwD3DTSSArg,
                          BOOL bComplement, BOOL bExpandNormal);
void    ConstructAlphaCombiners (PNVD3DCONTEXT pContext, int, int, BOOL);
void    ConstructOutputAlphaCombiners (PNVD3DCONTEXT pContext, int, int, int, int, int);

// nvCelsiusColorCombiner.cpp
void    SetColorMapping  (PNVD3DCONTEXT pContext, celsiusCombinerInput eInput,
                          DWORD dwCombinerStage, DWORD dwMapping);
void    SetColorInputOne (PNVD3DCONTEXT pContext, celsiusCombinerInput eInput,
                          DWORD dwCombinerStage, BOOL bNegate);
HRESULT SelectColorInput (PNVD3DCONTEXT pContext, celsiusCombinerInput eInput,
                          DWORD dwCombinerStage, DWORD dwD3DStage, DWORD dwD3DTSSArg,
                          BOOL bComplement, BOOL bExpandNormal, BOOL bAlphaReplicate);
BOOL    ConstructColorCombiners (PNVD3DCONTEXT pContext, int, int);
void    ConstructOutputColorCombiners (PNVD3DCONTEXT pContext, int, int, int, int, int, int, int);

// nvCelsiusPrim.cpp
void    nvCelsiusDispatchNonIndexedPrimitive      (NV_INNERLOOP_ARGLIST);
void    nvCelsiusDispatchIndexedPrimitive         (NV_INNERLOOP_ARGLIST);
void    nvCelsiusDispatchLegacyWireframePrimitive (NV_INNERLOOP_ARGLIST);

#endif  // NVARCH == 0x010

#if (NVARCH >= 0x020)
// nvKelvinAlphaCombiners.cpp
void    nvConstructKelvinAlphaCombiners (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwDXStage, BOOL bStageActive);
void    nvConstructKelvinOutputAlphaCombiners (PNVD3DCONTEXT pContext, BOOL bEnable,
                                               DWORD dwHWStage, DWORD dwDXStage,
                                               BOOL bBias, DWORD dwShift, BOOL bOutTemp);

// nvKelvinColorCombiners.cpp
BOOL    nvConstructKelvinColorCombiners (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwDXStage);
void    nvConstructKelvinOutputColorCombiners (PNVD3DCONTEXT pContext, BOOL bEnable, DWORD dwStage,
                                               BOOL bBias, DWORD dwShift, BOOL bDotAB, BOOL bDotCD, BOOL bOutTemp);

// nvKelvinPrim.cpp
void    nvKelvinDispatchNonIndexedPrimitive      (NV_INNERLOOP_ARGLIST);
void    nvKelvinDispatchIndexedPrimitive         (NV_INNERLOOP_ARGLIST);
void    nvKelvinDispatchLegacyWireframePrimitive (NV_INNERLOOP_ARGLIST);

#endif

// nvpkrnl.c (nvpe.lib)
#ifdef NVPE
void  __stdcall NVPInitialState(GLOBALDATA *pDriverData);
BOOL  __stdcall NVPIsVPEEnabled(GLOBALDATA *pDriverData, DWORD hDriver);
DWORD __stdcall NVPCleanUp(GLOBALDATA *pDriverData, LPDDRAWI_DIRECTDRAW_GBL pDDGbl);
DWORD __stdcall NVPUpdateOverlay(GLOBALDATA *pDriverData, LPDDRAWI_DIRECTDRAW_GBL pDDGbl);
#endif // NVPE

#ifdef __cplusplus
}
#endif // __cplusplus

#endif  // _NVPROTO_H
