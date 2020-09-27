#ifndef _D3DINC_H_
#define _D3DINC_H_
/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: D3DINC.H                                                          *
*   NV specific direct 3d structure definitions.                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Eugene Lapidous             ??/??/96 - wrote it.                    *
*       Charles Inman (chasi)       01/02/97 - re-wrote and cleaned up.     *
*                                                                           *
\***************************************************************************/

/*
 * --------------------------------------------------------------------------
 * Vertex cache.
 * --------------------------------------------------------------------------
 */
#define VINDEX_MASK                 0x0F

/*
 * --------------------------------------------------------------------------
 * Data Cache macros
 * --------------------------------------------------------------------------
 */
#define PREFETCH(address)       ((volatile DWORD)*(DWORD*)(address))

/*
 * --------------------------------------------------------------------------
 * Texture heap management defines and structures.
 * --------------------------------------------------------------------------
 */
typedef struct _def_tex_heap_header TEXHEAPHEADER, *PTEXHEAPHEADER;

#define PACKCHARS(c0, c1, c2, c3)               \
    (((DWORD)(unsigned char)(c0))       |       \
     ((DWORD)(unsigned char)(c1) << 8)  |       \
     ((DWORD)(unsigned char)(c2) << 16) |       \
     ((DWORD)(unsigned char)(c3) << 24))

#define HEAP_SHARED                 0x04000000UL    // put heap in shared memory
#define HEAP_SIZE_ADJUST            0x00100000
#define HEAP_TAG_FREE               PACKCHARS('F', 'R', 'E', 'E')
#define HEAP_TAG_ALLOC              PACKCHARS('L', 'O', 'C', 'K')

struct _def_tex_heap_header
{
    DWORD           dwTag;
    DWORD           dwBlockSize;
    DWORD           dwBlockStartOffset;
    DWORD           dwReserved;
    PTEXHEAPHEADER  pNextFreeBlock;
    PTEXHEAPHEADER  pPrevFreeBlock;
    PTEXHEAPHEADER  pNextAllocBlock;
    PTEXHEAPHEADER  pPrevAllocBlock;
};

/*
 * --------------------------------------------------------------------------
 * General purpose constants
 * --------------------------------------------------------------------------
 */
#define CONTEXT_ARRAY_SIZE          64
#define CONTEXT_ARRAY_DELTA         64
#define TEXTURE_ARRAY_SIZE          256
#define TEXTURE_ARRAY_DELTA         256
#ifndef OLDSTUFF
#define AGPLIST_ARRAY_SIZE          1000
#else
#define AGPLIST_ARRAY_SIZE          256
#endif
#define AGPLIST_ARRAY_DELTA         256
#define NVD3D_DEFAULT_TL_NUM        0           // was: ((32 * 2048) / sizeof(D3DTLVERTEX))

#define MAX_STATE                   D3DSTATE_OVERRIDE_BIAS
#define DWORD_BITS                  32
#define DWORD_SHIFT                 5

#ifdef  DO_XF_LIGHT
#define PIPELINE_ARRAY_SIZE         64
#define PIPELINE_ARRAY_DELTA        64
#define MATERIAL_ARRAY_SIZE         64
#define MATERIAL_ARRAY_DELTA        64
#endif  // DO_XF_LIGHT

/*
 * --------------------------------------------------------------------------
 *  Data structures.
 * --------------------------------------------------------------------------
 */

/*
 * DWORD triangle
 */
typedef struct NVD3DTRIANGLE {
    union {
        struct {
            WORD v1,v2;
        };
        DWORD v1v2;
    };
    union {
        struct {
            WORD v3,wFlags;
        };
        DWORD v3wFlags;
    };
} NVD3DTRIANGLE,*LPNVD3DTRIANGLE;

/*
 * Define function pointer prototypes.
 */
typedef void  (*LPNVDRAWTRI) (DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
typedef void  (*PFNDRAWPRIMITIVEOP)(DWORD, LPD3DTLVERTEX);
typedef void  (*PFNDRAWINDEXPRIMITIVEOP)(DWORD, LPWORD, LPD3DTLVERTEX);

#ifdef  DO_XF_LIGHT
/*
 * Define the pipeline flags.
 */
typedef struct _def_nv_d3d_pipeline  NVD3DPIPELINE, *PNVD3DPIPELINE;

struct _def_nv_d3d_pipeline
{
    DWORD                       pid;            /* Process ID */
    DWORD                       hContext;       /* Context Handle */
};

/*
 * Define the material flags.
 */
typedef struct _def_nv_d3d_material  NVD3DMATERIAL, *PNVD3DMATERIAL;

struct _def_nv_d3d_material
{
    DWORD                       pid;            /* Process ID */
    DWORD                       hContext;       /* Context Handle */
};
#endif  // DO_XF_LIGHT

/*
 * Define structure type definitions.
 */
typedef struct _def_nv_d3d_texture              NVD3DTEXTURE, *PNVD3DTEXTURE;
typedef struct _def_nv_d3d_context              NVD3DCONTEXT, *PNVD3DCONTEXT;
typedef struct _def_nv_d3d_context_inner_loop   NVD3DCONTEXTINNERLOOP, *PNVD3DCONTEXTINNERLOOP;

/*
 * D3D state set data.
 */
typedef struct _nv_d3d_state_set
{
    DWORD   bits[MAX_STATE >> DWORD_SHIFT];
} NVD3DSTATESET;

/*
 * Hardware Independant Texture Flags
 * Flags specifing that a special work around is required for this texture.
 */
#define NV_TEXTURE_DEBUG_01_HEAVY_GEAR  0x00000001  // Heavy Gear Patch Color Key Workaround.

/*
 * Include hardware specific include files here.
 * In order to be compatible with old source code, these need to be
 * included here.
 */
#ifdef  NV4
#include "nv4dinc.h"
#endif  // NV4

/*
 * --------------------------------------------------------------------------
 * External data declarations.
 * --------------------------------------------------------------------------
 */
#include "global.h"

/*
 * Some statistical values.
 */

// omnitemporal texture counts
extern DWORD   statTextureOmniCountTotal;
extern DWORD   statTextureOmniCountPalette;

// omnitemporal texture sizes
extern DWORD   statTextureOmniSizeTotal;
extern DWORD   statTextureOmniSizePalette;

// extant texture counts
extern DWORD   statTextureCountTotal;
extern DWORD   statTextureCountVideo;
extern DWORD   statTextureCountAgpHeap;
extern DWORD   statTextureCountPciHeap;
extern DWORD   statTextureCountCache;

// extant texture sizes
extern DWORD   statTextureSizeTotal;
extern DWORD   statTextureSizeVideo;
extern DWORD   statTextureSizeAgpHeap;
extern DWORD   statTextureSizePciHeap;
extern DWORD   statTextureSizeCache;

// texture dimensions
extern DWORD   statTextureDimensionMinVideo;
extern DWORD   statTextureDimensionMinAgpHeap;
extern DWORD   statTextureDimensionMinPciHeap;
extern DWORD   statTextureDimensionMinCache;
extern DWORD   statTextureDimensionMaxVideo;
extern DWORD   statTextureDimensionMaxAgpHeap;
extern DWORD   statTextureDimensionMaxPciHeap;
extern DWORD   statTextureDimensionMaxCache;

// dma stats
extern DWORD   statDmaSyncLoop;
extern DWORD   statDmaSyncOverflow;
extern DWORD   statDmaBusyLoop;
extern DWORD   statDmaBusyOverflow;
extern DWORD   statDmaFlushLoop;
extern DWORD   statDmaFlushOverflow;

/*
 * --------------------------------------------------------------------------
 *  Prototypes.
 * --------------------------------------------------------------------------
 */

/*
 * d3dmini.c
 */
//BOOL __stdcall D3DGetDriverInfo(LPDDHAL_GETDRIVERINFODATA);
void D3DReadRegistry(void);
void D3DModifyCapabilities(D3DHAL_GLOBALDRIVERDATA  *pNvGlobal);
BOOL CreateItemArrays(void);

/*
 * d3dctxt.c
 */
BOOL  nvCreateContextListHeap(void);
BOOL  nvDestroyContextListHeap(void);
DWORD __stdcall nvContextCreate(LPD3DHAL_CONTEXTCREATEDATA);
DWORD __stdcall nvContextDestroy(LPD3DHAL_CONTEXTDESTROYDATA);
DWORD __stdcall nvContextDestroyAll(LPD3DHAL_CONTEXTDESTROYALLDATA);
void  nvAllocDefaultTexture(PNVD3DCONTEXT);
void  nvFreeDefaultTexture(PNVD3DCONTEXT);
void  nvD3DReenable();
BOOL  nvSetContextSurface(PNVD3DCONTEXT);
BOOL  nvSetupContext(PNVD3DCONTEXT);
BOOL  nvFinalContextCleanup(void);

/*
 * d3drendr.c
 */
void  nvDMAStartTransfer(void);
#ifndef WINNT
DWORD __stdcall nvRenderState(LPD3DHAL_RENDERSTATEDATA);
#endif
DWORD __stdcall nvSetRenderTarget(LPD3DHAL_SETRENDERTARGETDATA);
#ifndef WINNT
DWORD __stdcall nvRenderPrimitive(LPD3DHAL_RENDERPRIMITIVEDATA);
#endif
DWORD nvSetContextState(DWORD, DWORD, HRESULT *);
BOOL  nvSetHardwareState();
void  nvResetContext(PNVD3DTEXTURE);
DWORD __stdcall nvGetDmaFreeCount(long);
void  nvStartDmaBuffer(BOOL);
void  nvFlushDmaBuffers(void);
BOOL  nvD3DClearZBuffer(LPDDRAWI_DDRAWSURFACE_LCL, DWORD);

/*
 * d3dtex.c
 */

#ifdef NV_TEX2
/*
 * ver 2.0
 */
#define NV4_TEXWAIT_READACCESS      1
#define NV4_TEXWAIT_WRITEACCESS     2
#define NV4_TEXWAIT_AVAILABLENOW    4   // when semaphore is seen the resource is available
#define NV4_TEXWAIT_AVAILABLEAFTER  0   // resource is available after the next semaphore is seen

BOOL  nvTextureRef           (PNVD3DTEXTURE);
BOOL  nvTextureTestBlock     (DWORD); // report if we need to stall CPU until given texture is not in use
void  nvTextureBlock         (DWORD); // stall CPU until given texture is not in use
void  nvTextureSetBlockPoint (void);

DWORD nvTextureBlt          (LPDDHAL_BLTDATA);
DWORD nvTextureLock         (PNVD3DTEXTURE,LPDDHAL_LOCKDATA);
DWORD nvTextureUnlock       (PNVD3DTEXTURE,LPDDHAL_UNLOCKDATA);

BOOL  nvTextureAllocSwizzleSurface  (PNVD3DTEXTURE,DWORD,DWORD,DWORD);
BOOL  nvTextureFreeSwizzleSurface   (PNVD3DTEXTURE);
BOOL  nvTextureAllocLinearSurface   (PNVD3DTEXTURE,DWORD,DWORD,DWORD);
BOOL  nvTextureFreeLinearSurface    (PNVD3DTEXTURE);

BOOL  nvUpdateSwizzleSurface        (PNVD3DTEXTURE);
BOOL  nvUpdateLinearSurface         (PNVD3DTEXTURE);

#else // NV_TEX2
/*
 * ver 1.0
 */
BOOL  nvLoadTexture(PNVD3DTEXTURE);
void  nvTextureBlit16Bpp(LPDDHAL_BLTDATA);
void  nvTextureBlit32Bpp(LPDDHAL_BLTDATA);
void  nvSwizzleBlit16Bpp(DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,
                         DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD);
void  nvSwizzleBlit32Bpp(DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,
                         DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD);

#endif //NV_TEX2

/*
 * common
 */
BOOL  nvCreateTextureListHeap(void);
BOOL  nvDestroyTextureListHeap(void);
BOOL  nvDestroyTextureStructure(PNVD3DTEXTURE);
BOOL  nvDestroyTextureProcess(DWORD);
BOOL  nvDestroyTextureContext(DWORD);
BOOL  CreateAGPListArray(void);
BOOL  DestroyAGPListArray(void);
BOOL  CreateTextureHeap(void);
BOOL  DestroyTextureHeap(void);
DWORD AllocTextureHeap(DWORD);
BOOL  FreeTextureHeap(DWORD);
DWORD GetPointerTextureHeap(DWORD);
DWORD AllocAGPHeap(LPDDRAWI_DIRECTDRAW_GBL, DWORD);
BOOL  FreeAGPHeap(LPDDRAWI_DIRECTDRAW_GBL, DWORD);
BOOL  ResetAGPHeap(void);
DWORD DestroyAGPListCallback(LPVOID, HDDRVITEM, DWORD);
DWORD __stdcall nvTextureCreate(LPD3DHAL_TEXTURECREATEDATA);
DWORD __stdcall nvTextureDestroy(LPD3DHAL_TEXTUREDESTROYDATA);
DWORD __stdcall nvTextureSwap(LPD3DHAL_TEXTURESWAPDATA);
DWORD __stdcall nvTextureGetSurf(LPD3DHAL_TEXTUREGETSURFDATA);
BOOL  nvAllocateInternalTexture(PNVD3DTEXTURE);
BOOL  nvDestroyInternalTexture(PNVD3DCONTEXT, PNVD3DTEXTURE);
BOOL  nvGenerateColorKeyAutoMipMap(long, long, long, DWORD, unsigned short *);
BOOL  nvGenerateAutoMipMap(long, long, long, unsigned short *);
BOOL  nvAllocateUserTexture(PNVD3DTEXTURE);
BOOL  nvCreateTextureSurface(LPDDHAL_CREATESURFACEDATA, LPDDRAWI_DDRAWSURFACE_LCL, LPDDRAWI_DDRAWSURFACE_GBL);
BOOL  nvDestroyTextureSurface(LPDDHAL_DESTROYSURFACEDATA, LPDDRAWI_DDRAWSURFACE_LCL, LPDDRAWI_DDRAWSURFACE_GBL);
DWORD UnplugSystemMemoryTextureContext(void);
DWORD AdjustTextureLimit(DWORD, DWORD, DWORD, DWORD);
DWORD UpdateTriangleContexts(void);
BOOL  nvMarkTextureDirty(PNVD3DCONTEXT, PNVD3DTEXTURE);
BOOL  nvMarkAllTexturesDirty(PNVD3DCONTEXT);
BOOL  nvMarkTextureValid(PNVD3DCONTEXT, PNVD3DTEXTURE);

#ifndef WINNT
/*
 * d3dstate.c
 */
DWORD __stdcall nvGetState(LPD3DHAL_GETSTATEDATA);
#endif

/*
 * d3dscene.c
 */
DWORD __stdcall nvSceneCapture(LPD3DHAL_SCENECAPTUREDATA);

/*
 * d3dtri.c
 */
void  nvNoOp(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);

void  nvDmaPushTri(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriFog(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriSpecular(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriFogSpecular(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriFlat(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriFogFlat(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriSpecularFlat(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriFogSpecularFlat(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriDecal(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriFogDecal(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriSpecularDecal(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriFogSpecularDecal(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);

void  nvDmaPushTriScaled(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriScaledFog(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriScaledSpecular(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriScaledFogSpecular(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriScaledFlat(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriScaledFogFlat(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriScaledSpecularFlat(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriScaledFogSpecularFlat(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriScaledDecal(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriScaledFogDecal(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriScaledSpecularDecal(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriScaledFogSpecularDecal(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);

void  nvDmaPushSpecularPass(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushSpecularPassScaled(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushFlatSpecularPass(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushFlatSpecularPassScaled(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushFogSpecularPass(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushFogSpecularPassScaled(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushFogFlatSpecularPass(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushFogFlatSpecularPassScaled(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvNonPerspectiveTriangle(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvFogTableTriangle(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvGapFillTriangle(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriangleOnePass(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriangleOnePassSpecular(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriangleTwoPass(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriangleTwoPassSpecular(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvDmaPushTriangleTwoPassSpecularNoZeta(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);

void nvTriangleStateChange(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);

/*
 * d3dpoint.c
 */
void  nvPointTriangle(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX, LPNVDRAWTRI, LPNVDRAWTRI, LPNVDRAWTRI);
void  nvDmaPushPointTriangle(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvPoint(DWORD, LPD3DPOINT, LPD3DTLVERTEX);
DWORD nvRenderPointDmaPush(DWORD, LPWORD, LPD3DTLVERTEX);

/*
 * d3dline.c
 */
void  nvWireframeTriangle(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX, LPNVDRAWTRI, LPNVDRAWTRI, LPNVDRAWTRI);
void  nvDmaPushWireframeTriangle(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvLine(DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
DWORD nvDrawLineDmaPush(DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
DWORD nvRenderLineDmaPush(DWORD, LPWORD, LPD3DTLVERTEX);

/*
 * d3dmisc.c
 */
VOID  uvFixDmaPush(D3DVALUE, LPD3DTLVERTEX, LPD3DTLVERTEX, LPD3DTLVERTEX, DWORD, DWORD);

/*
 * d3dmip.c
 */
void  nvMipMapRenderPrimitive(DWORD, LPWORD, DWORD, DWORD, LPD3DTLVERTEX);
void  nvMidPointSubdivision(PNVD3DTEXTURE, D3DTLVERTEX, D3DTLVERTEX, D3DTLVERTEX, LPNVDRAWTRI);
int   nvLODCompare(int, int, DWORD, DWORD);
DWORD nvConcurrentPass (void);
DWORD nvGetCPUSpeed    (void);
BOOL  nvDetectMMX      (void);
BOOL  nvDetectKATMAI   (void);

#ifdef NV_TEX2
/*
 * nv4swiz.c
 */
#define NV_SWIZFLAG_SRCLINEAR       0x00000000
#define NV_SWIZFLAG_SRCSWIZZLED     0x00000001
#define NV_SWIZFLAG_DESTLINEAR      0x00000000
#define NV_SWIZFLAG_DESTSWIZZLED    0x00000002
#define NV_SWIZFLAG_PALETTIZED      0x00000004
#define NV_SWIZFLAG_ALLOWHW         0x80000000

#define NV_SWIZVAL_MAX              16

BOOL nvSwizzleBlt   (DWORD dwSrcAddr,DWORD dwSrcAlloc,
                          DWORD dwSrcLogW,DWORD dwSrcLogH,
                          DWORD dwSrcX0,DWORD dwSrcY0,DWORD dwSrcX1,DWORD dwSrcY1,
                          DWORD dwSrcPitch,
                          PNVD3DTEXTURE pSrcTexture,
                     DWORD dwDestAddr,DWORD dwDestAlloc,
                          DWORD dwDestLogW,DWORD dwDestLogH,
                          DWORD dwDestX,DWORD dwDestY,
                          DWORD dwDestPitch,
                          PNVD3DTEXTURE pDestTexture,
                     DWORD dwBPP,
                     DWORD dwFlags);

#endif // NV_TEX2

/*
 * d3danti.c
 */
BOOL nvAACreate           (void);
void nvAADestroy          (void);
BOOL nvAACreateBuffers    (void);
BOOL __stdcall nvAACapturePrimitive (DWORD,LPWORD,DWORD,LPBYTE);
BOOL nvAARenderPrimitive  (void);

#ifdef  DO_XF_LIGHT
/*
 * d3dmatrx.c
 */
BOOL  CreatePipelineArray(void);
BOOL  DestroyPipelineArray(void);
DWORD __stdcall nvMatrixCreate(LPD3DHAL_MATRIXCREATEDATA pmcd);
DWORD __stdcall nvMatrixDestroy(LPD3DHAL_MATRIXDESTROYDATA pmdd);
DWORD __stdcall nvMatrixSetData(LPD3DHAL_MATRIXSETDATADATA pmsd);
DWORD __stdcall nvMatrixGetData(LPD3DHAL_MATRIXGETDATADATA pmgd);
DWORD __stdcall nvSetViewportData(LPD3DHAL_SETVIEWPORTDATADATA pvsd);
DWORD DestroyPipelineCallback(LPVOID lpData, HDDRVITEM hItem, DWORD dwExtra);
DWORD DestroyPipelineProcessCallback(LPVOID, HDDRVITEM, DWORD);
DWORD DestroyPipelineContextCallback(LPVOID, HDDRVITEM, DWORD);

/*
 * d3dmater.c
 */
BOOL  CreateMaterialArray(void);
BOOL  DestroyMaterialArray(void);
DWORD __stdcall nvLightSet(LPD3DHAL_LIGHTSETDATA plsd);
DWORD __stdcall nvMaterialCreate(LPD3DHAL_MATERIALCREATEDATA pmcd);
DWORD __stdcall nvMaterialDestroy(LPD3DHAL_MATERIALDESTROYDATA pmdd);
DWORD __stdcall nvMaterialDestroy(LPD3DHAL_MATERIALDESTROYDATA pmdd);
DWORD __stdcall nvMaterialSetData(LPD3DHAL_MATERIALSETDATADATA pmsd);
DWORD __stdcall nvMaterialGetData(LPD3DHAL_MATERIALGETDATADATA pmgd);
DWORD DestroyMaterialCallback(LPVOID lpData, HDDRVITEM hItem, DWORD dwExtra);
DWORD DestroyMaterialProcessCallback(LPVOID, HDDRVITEM, DWORD);
DWORD DestroyMaterialContextCallback(LPVOID, HDDRVITEM, DWORD);
#endif  // DO_XF_LIGHT

/*
 * For easy compatability with old code.
 * These include files are protected from being loaded twice.
 */
#include    "nvd3dmac.h"

#define HEAP_ALLOC_DEPTH_WIDTH_HEIGHT        1
#define HEAP_ALLOC_SIZE                      2
#define HEAP_FREE                            3
#define HEAP_PURGE                           4
#define HEAP_INFO                            5
#define HEAP_ALLOC_TILED_PITCH_HEIGHT        6
#define TYPE_IMAGE                           0
#define TYPE_DEPTH                           1
#define TYPE_TEXTURE                         2
#define TYPE_OVERLAY                         3
#define TYPE_FONT                            4
#define TYPE_CURSOR                          5
#define TYPE_DMA                             6
#define TYPE_INSTANCE                        7
#define MEM_TYPE_IMAGE_TILED                 9
#define STATUS_SUCCESS                       (0x00000000)
#define STATUS_ERROR_INSUFFICIENT_RESOURCES  (0x00000001)
#define STATUS_ERROR_INVALID_FUNCTION        (0x00000002)
#define STATUS_ERROR_INVALID_OWNER           (0x00000003)

#ifdef NVHEAPMGR
/*
 * Heap manager interface.
 */
extern void __stdcall NvIoControl(DWORD, PVOID);

/* parameter values */
#ifndef WINNT
#ifdef MULTI_MON
#define NVHEAP_ALLOC(pvm,sz,tp)             \
{                                           \
    HeapParams.hRoot = pDriverData->dwRootHandle; \
    HeapParams.hObjectParent = NV_WIN_DEVICE; \
    HeapParams.function = HEAP_ALLOC_SIZE;  \
    HeapParams.owner    = 'NVDD';           \
    HeapParams.type     = (tp);             \
    HeapParams.size     = (sz);             \
    NvIoControl(11, (PVOID)&HeapParams);    \
    (pvm) = HeapParams.status ? 0 : HeapParams.address;\
    pDriverData->VideoHeapFree = HeapParams.free;\
}
#define NVHEAP_FREE(pvm)                    \
{                                           \
    HeapParams.hRoot = pDriverData->dwRootHandle; \
    HeapParams.hObjectParent = NV_WIN_DEVICE; \
    HeapParams.function = HEAP_FREE;        \
    HeapParams.owner    = 'NVDD';           \
    HeapParams.offset   = (U032)(pvm) - pDriverData->BaseAddress;\
    NvIoControl(11, (PVOID)&HeapParams);    \
    pDriverData->VideoHeapFree = HeapParams.free;\
}
#else   // !MULTI_HEAP
#define NVHEAP_ALLOC(pvm,sz,tp)             \
{                                           \
    HeapParams.function = HEAP_ALLOC_SIZE;  \
    HeapParams.device   = 0;                \
    HeapParams.owner    = 'NVDD';           \
    HeapParams.type     = (tp);             \
    HeapParams.size     = (sz);             \
    NvIoControl(11, (PVOID)&HeapParams);    \
    (pvm) = HeapParams.status ? 0 : HeapParams.address;\
    pDriverData->VideoHeapFree = HeapParams.free;\
}
#define NVHEAP_FREE(pvm)                    \
{                                           \
    HeapParams.function = HEAP_FREE;        \
    HeapParams.device   = 0;                \
    HeapParams.owner    = 'NVDD';           \
    HeapParams.offset   = (U032)(pvm) - pDriverData->BaseAddress;\
    NvIoControl(11, (PVOID)&HeapParams);    \
    pDriverData->VideoHeapFree = HeapParams.free;\
}
#endif  // !MULTI_HEAP
#else
#ifndef RM_HEAPMGR
#define NVHEAP_ALLOC(pvm,sz,tp)                                 \
{                                                               \
    (pvm) = heap_alloc(sz, pDriverData->ppdev->heap.heap_2d_ptr);       \
    if(!(pvm))                                                  \
    {                                                           \
        bMoveAllDfbsFromOffscreenToDibs(pDriverData->ppdev);    \
        (pvm) = heap_alloc(sz, pDriverData->ppdev->heap.heap_2d_ptr);   \
    }                                                           \
}

#define NVHEAP_FREE(pvm)             \
    heap_free((pvm), pDriverData->ppdev->heap.heap_2d_ptr)
#else
extern BOOL bMoveAllDfbsFromOffscreenToDibs(PDEV *);

#define NVHEAP_ALLOC(pvm,sz,tp)             \
{                                           \
    NVOS11_PARAMETERS   HeapParams;         \
    PVOID pParms = (PVOID) &HeapParams;     \
    DWORD cbReturned;                       \
                                            \
    if (pDriverData && !pDriverData->DDrawVideoSurfaceCount)         \
    {                                       \
        pDriverData->ppdev->cbGdiHeap = pDriverData->ppdev->VideoHeapTotal -   \
            pDriverData->ppdev->VideoHeapFree;                      \
                                                                    \
    }                                       \
                                                    \
    HeapParams.hRoot = pDriverData->ppdev->hClient;  \
    HeapParams.hObjectParent = pDriverData->ppdev->hDevice;                  \
    HeapParams.function = HEAP_ALLOC_SIZE;                      \
    HeapParams.owner    = 'NVDD';                               \
    HeapParams.type     = (tp);                                 \
    HeapParams.size     = (sz);                                 \
    EngDeviceIoControl(                         \
        pDriverData->ppdev->hDriver,            \
        (DWORD)IOCTL_NV01_ARCH_HEAP,            \
        (&pParms),                              \
        sizeof(PVOID),                          \
        pParms,                                 \
        sizeof(NVOS11_PARAMETERS),              \
        &cbReturned                             \
    );                                          \
    (pvm) = HeapParams.status ? 0 : HeapParams.offset;          \
    if (!(pvm))                                                 \
    {                                                           \
        bMoveAllDfbsFromOffscreenToDibs(pDriverData->ppdev);    \
        HeapParams.hRoot = pDriverData->ppdev->hClient;  \
        HeapParams.hObjectParent = pDriverData->ppdev->hDevice;                  \
        HeapParams.function = HEAP_ALLOC_SIZE;                      \
        HeapParams.owner    = 'NVDD';                               \
        HeapParams.type     = (tp);                                 \
        HeapParams.size     = (sz);                                 \
        EngDeviceIoControl(                         \
            pDriverData->ppdev->hDriver,            \
            (DWORD)IOCTL_NV01_ARCH_HEAP,            \
            (&pParms),                              \
            sizeof(PVOID),                          \
            pParms,                                 \
            sizeof(NVOS11_PARAMETERS),              \
            &cbReturned                             \
        );                                          \
        (pvm) = HeapParams.status ? 0 : HeapParams.offset;          \
    }                                               \
    if (pvm)                                        \
        pDriverData->ppdev->VideoHeapFree = HeapParams.free;    \
}

#define NVHEAP_ALLOC_TILED(pvm,pch,ht,tp)   \
{                                           \
    NVOS11_PARAMETERS HeapParams;           \
    PVOID pParms = (PVOID) &HeapParams;     \
    DWORD cbReturned;                       \
    HeapParams.hRoot = pDriverData->ppdev->hClient; \
    HeapParams.hObjectParent = pDriverData->ppdev->hDevice; \
    HeapParams.function = HEAP_ALLOC_TILED_PITCH_HEIGHT;\
    HeapParams.owner    = 'NVDD';           \
    HeapParams.type     = (tp);             \
    HeapParams.pitch    = (pch);            \
    HeapParams.height   = (ht);             \
    EngDeviceIoControl(                         \
        pDriverData->ppdev->hDriver,            \
        (DWORD)IOCTL_NV01_ARCH_HEAP,            \
        (&pParms),                              \
        sizeof(PVOID),                          \
        pParms,                                 \
        sizeof(NVOS11_PARAMETERS),              \
        &cbReturned                             \
    );                                          \
                                                \
    (pvm) = HeapParams.status ? 0 : HeapParams.offset;\
    (pch) = HeapParams.pitch;                   \
    if ((pvm))                                  \
        pDriverData->VideoHeapFree = HeapParams.free;\
}

#define NVHEAP_FREE(pvm)                    \
{                                           \
    NVOS11_PARAMETERS HeapParams;           \
    PVOID pParms = (PVOID) &HeapParams;     \
    DWORD cbReturned;                       \
    HeapParams.hRoot = pDriverData->ppdev->hClient;      \
    HeapParams.hObjectParent = pDriverData->ppdev->hDevice; \
    HeapParams.function = HEAP_FREE;        \
    HeapParams.owner    = 'NVDD';         \
    HeapParams.offset   = (U032)(pvm);      \
    EngDeviceIoControl(                         \
        pDriverData->ppdev->hDriver,                                \
        (DWORD)IOCTL_NV01_ARCH_HEAP,            \
        (&pParms),                              \
        sizeof(PVOID),                          \
        pParms,                                 \
        sizeof(NVOS11_PARAMETERS),              \
        &cbReturned                             \
    );                                          \
    pDriverData->ppdev->VideoHeapFree = HeapParams.free;              \
}

#endif // RM_HEAPMGR
#endif // #ifdef WINNT
#define AGP_HEAP                                    0
#else
#ifndef WINNT
#define NVHEAP_ALLOC(pvm,size,tp)                                                \
{                                                                                \
    DWORD _actual = size;                                                        \
    if ((tp) == TYPE_TEXTURE) _actual = (_actual + NV_TEXTURE_PAD + NV_TEXTURE_OFFSET_ALIGN) & ~NV_BYTE_ALIGNMENT_PAD;\
    (pvm) = DDHAL32_VidMemAlloc(pdrv, 0, _actual, 1);                            \
}
#define NVHEAP_FREE(pvm)                            DDHAL32_VidMemFree(pdrv, 0, pvm)
#define AGP_HEAP                                    1
#else
#define NVHEAP_ALLOC(pfm,size,tp) \
{                                                   \
SURFACEALIGNMENT    _align;                         \
ULONG               DummyPitch;                     \
switch (tp)                                         \
    {                                               \
    case TYPE_TEXTURE:                              \
        _align.Linear.dwSurfaceAlignment = NV_TEXTURE_OFFSET_ALIGN + 1; \
        _align.Linear.dwPitchAlignment = NV_BYTE_ALIGNMENT; \
        break;                                      \
                                                    \
    case TYPE_IMAGE:                                \
    case TYPE_OVERLAY:                              \
    case TYPE_DEPTH:                                \
        _align.Linear.dwSurfaceAlignment = _align.Linear.dwPitchAlignment = NV_BYTE_ALIGNMENT; \
        break;                                      \
                                                    \
    default:                                        \
        DPF("Bad allocation type passed to NVHEAP_ALLOC"); \
            _align.Linear.dwSurfaceAlignment = _align.Linear.dwPitchAlignment = NV_BYTE_ALIGNMENT_PAD; \
                                                    \
    }                                               \
(pfm) = HeapVidMemAllocAligned(pdrv, size, 1,       \
            &_align,                                \
            &DummyPitch                             \
            );                                      \
}

#define NVHEAP_FREE(pfm)            VidMemFree(pdrv, pfm)
#define AGP_HEAP                    0
#endif // #ifdef WINNT
#endif

#ifndef WINNT
#define NV_DELAY                                          \
{                                                         \
    __asm { push ecx                                    } \
    __asm { mov ecx,[global.dwProcessorSpeed]           } \
    __asm { shl ecx,3                                   } \
x:  __asm { loop x                                      } \
    __asm { pop ecx                                     } \
}

#define NV_DELAY_(x)                                      \
{                                                         \
    __asm { push ecx                                    } \
    __asm { mov ecx,[global.dwProcessorSpeed]           } \
    __asm { shl ecx,3                                   } \
x:  __asm { loop x                                      } \
    __asm { pop ecx                                     } \
}
#else
#define NV_DELAY
#endif // #ifdef WINNT

void WaitForVSync (void);

/*
 * wait for VSYNC strategy:
 *
 * 1) we should have the registry bit set that enable wait for VSYNC
 * 2) we should have a flip pending
 * 3) the flip should involve the current render target
 */
#define WAIT_FOR_VSYNC(_ctx)                                                                        \
{                                                                                                   \
    if (!(pDriverData->regD3DEnableBits1 & REG_BIT1_NO_WAIT_4_VSYNC) /* vsync == true */            \
      && pDriverData->flipPrimaryRecord.bFlipFlag)                                                  \
    {                                                                                               \
        /* skip wait if we do not render to any flipping surfaces */                                \
        if ((_ctx)                                                                                  \
         && (pDriverData->flipPrimaryRecord.fpFlipFrom != (_ctx)->lpLcl->lpGbl->fpVidMem)           \
         && (pDriverData->flipPrimaryRecord.fpFlipTo   != (_ctx)->lpLcl->lpGbl->fpVidMem))          \
        {                                                                                           \
            /* do nothing */                                                                        \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            /* have to wait for vsync */                                                            \
            WaitForVSync();                                                                         \
        }                                                                                           \
    }                                                                                               \
}


#endif // _D3DINC_H_
