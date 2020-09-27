#ifndef _NV4DINC_H_
#define _NV4DINC_H_
#ifdef  NV4
/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4DINC.H                                                         *
*   NV4 specific direct 3d structure definitions.                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/97 - created.                     *
*                                                                           *
\***************************************************************************/

/*
 * Always build for D3D.
 * Always build a non-syncing driver.
 */
#ifndef WINNT
#define NVD3D       1
#endif // #ifndef WINNT
#define D3D_NOSYNC  1
/*
 * Specular contribution check.
 */
#define SPEC_HACK   1
#ifdef  SPEC_HACK
#define SPEC_CHECK(SpecState, SpecCurr, S0, S1, S2, count, blend)           \
{                                                                           \
    if (SpecState)                                                          \
    {                                                                       \
        if (((S0) | (S1) | (S2)) & 0x00FCFCFC)                              \
        {                                                                   \
            if (!SpecCurr)                                                  \
            {                                                               \
                (blend) |= DRF_DEF(054, _BLEND, _SPECULARENABLE, _TRUE);    \
                while (nvFreeCount < 2)                                     \
                    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, 2);  \
                ((DWORD *)(nvFifo))[0] = (((1 << 2) << 16) | (NV_DD_SPARE << 13) | NV054_BLEND); \
                ((DWORD *)(nvFifo))[1] = (blend);                           \
                nvFreeCount -= 2;                                           \
                (DWORD *)nvFifo += 2;                                       \
                (count) = 0;                                                \
                (SpecCurr) = TRUE;                                          \
            }                                                               \
        }                                                                   \
        else                                                                \
        {                                                                   \
            (count)++;                                                      \
            if ((count) == 5)                                               \
            {                                                               \
                (blend) &= 0xFFFF0FFF;                                      \
                while (nvFreeCount < 2)                                     \
                    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, 2);  \
                ((DWORD *)(nvFifo))[0] = (((1 << 2) << 16) | (NV_DD_SPARE << 13) | NV054_BLEND); \
                ((DWORD *)(nvFifo))[1] = (blend);                           \
                nvFreeCount -= 2;                                           \
                (DWORD *)nvFifo += 2;                                       \
                (SpecCurr) = FALSE;                                         \
            }                                                               \
        }                                                                   \
    }                                                                       \
}
#else   // !SPEC_HACK
#define SPEC_CHECK(SpecState, SpecCurr, S0, S1, S2, count, blend)
#endif  // !SPEC_HACK

/*
 * Check current FVF vertex type to determine if recalculation is needed.
 */
#define NVFVF_NOTEXTURE_VERTEX  (D3DFVF_TLVERTEX & ~D3DFVF_TEXCOUNT_MASK)
#define CHECK_FVF_DATA_AND_LOAD_TL(_fvfData,_vertexType,_dwVertexStride,_bNeedFVFReload)\
{                                                                           \
    (_bNeedFVFReload) = FALSE;                                              \
    if ((_fvfData).dwVertexType != (_vertexType))                           \
    {                                                                       \
        if ((_vertexType) == D3DFVF_TLVERTEX) /* load TL vertex */          \
        {                                                                   \
            extern NVD3DFVFDATA   constFVFData;                             \
            (_fvfData)          = constFVFData;                             \
            (_dwVertexStride)   = 32;                                       \
        }                                                                   \
        else if (((_vertexType) & ~D3DFVF_TEXCOUNT_MASK) == NVFVF_NOTEXTURE_VERTEX) \
        {                                                                   \
            extern NVD3DFVFDATA   constFVFData;                             \
            extern D3DTLVERTEX    tlvDefault;                               \
            DWORD dwUVCount;                                                \
            DPF_LEVEL(NVDBG_LEVEL_INFO, "D3D:Calculate TLVERTEX with extra UV data"); \
            dwUVCount = (((_vertexType) & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT); \
            (_fvfData)          = constFVFData;                             \
            (_dwVertexStride)   = 24 + (dwUVCount * 2 * 4);                 \
            (_fvfData).dwVertexType   = (_vertexType);                      \
            (_fvfData).dwVertexStride = (_dwVertexStride);                  \
            (_fvfData).dwUVCount      = dwUVCount;                          \
            if (!dwUVCount)                                                 \
            {                                                               \
                (_fvfData).dwUVMask   = 0x00000000;                         \
                (_fvfData).dwUVOffset = (DWORD)&tlvDefault.tu;              \
            }                                                               \
        }                                                                   \
        else                                  /* tag external fvfData calc*/\
        {                                                                   \
            (_bNeedFVFReload) = TRUE;                                       \
        }                                                                   \
    }                                                                       \
    else                                      /* load cached stride */      \
    {                                                                       \
        (_dwVertexStride) = (_fvfData).dwVertexStride;                      \
    }                                                                       \
}

#ifdef  NVD3D_DX6
typedef struct _def_nv_d3d_fvf_data     NVD3DFVFDATA, *PNVD3DFVFDATA;
struct _def_nv_d3d_fvf_data
{
    DWORD   dwVertexType;
    DWORD   dwVertexStride;
    DWORD   dwXYZMask;
    DWORD   dwXYZOffset;
    DWORD   dwRHWMask;
    DWORD   dwRHWOffset;
    DWORD   dwARGBMask;
    DWORD   dwARGBOffset;
    DWORD   dwSpecularMask;
    DWORD   dwSpecularOffset;
    DWORD   dwUVMask;
    DWORD   dwUVOffset;
    DWORD   dwUVCount;
    DWORD   _pad0;
    DWORD   _pad1;
    DWORD   _pad2;
};

typedef DWORD (*PFNRENDERPRIMITIVEOP)(DWORD, LPBYTE, LPD3DTLVERTEX);
typedef void  (*LPNVINDEXEDPRIM)     (DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
typedef void  (*LPNVDRAWPRIM)        ( WORD,         DWORD, LPD3DTLVERTEX);
typedef void  (*LPNVFVFINDEXEDPRIM)  (DWORD, LPWORD, DWORD, LPBYTE);
typedef void  (*LPNVFVFDRAWPRIM)     ( WORD,         DWORD, LPBYTE);

#define NV_UV_COORDINATE_COUNT_MAX          2
#endif  // NVD3D_DX6

/*
 * Anti Aliasing context
 */
#define AA_MODE_SUPER                       0x00000001  // super sampled AA enabled
#define AA_MODE_SINGLECHUNK                 0x00000002  // single chunk super aa
#define AA_MODE_EDGE                        0x00000004  // edge AA enabled
#define AA_ENABLED_MASK                     0x0000000f  // all types of AA
#define AA_BUFALLOC_SUPER                   0x00000010  // super sampled aa buffers allocated
#define AA_BUFALLOC_SINGLECHUNK             0x00000020  // single chunk super sampled aa buffers allocated
#define AA_BUFALLOC_EDGE                    0x00000040  // edge aa buffers allocated
#define AA_BUFALLOC_MASK                    0x000000f0  // internal buffers allocated
#define AA_INVALID_SEMANTICS                0x00000100  // app behaves badly - aa is disabled
#define AA_CAPTURE_PUSHBUFFER               0x00000200  // aa captures to sysmem push buffer
#define AA_RENDER                           0x00000400  // aa is rendering
#define AA_CLEAR_SCREEN                     0x00000800  // set when we need to clear buffers
#define AA_CHECK_SEMANTICS                  0x00001000  // used to detect bad semantics (2d during 3d)
#define AA_COPY_SCREEN                      0x00002000  // copy frame buffer to super buffer instead of clear
#define AA_PENDING_DATA                     0x00004000  // set in singlechunk mode if we actually have rendered tris
#define AA_IN_SCENE                         0x00008000  // inside a scene - 2d is not tolerated
#define AA_2D_ADD                           0x10000000  // see below
#define AA_2D_MAX                           0x50000000  // see below ... max 2d to disable
#define AA_2D_COUNT_MASK                    0x70000000  // used to detect the app mixing 2d & 3d
#define AA_MIX_DP_NOAA                      0x00010000  // we drew primitives without aa
#define AA_MIX_DP_AA                        0x00020000  // we drew primitives with aa
#define AA_MIX_MASK                         0x00030000  // both

#define NV_AA_SEMANTICS_SETUP(_ctx)                                                 \
{                                                                                   \
    DPF_LEVEL(NVDBG_LEVEL_AA_INFO,"macro NV_AA_SEMANTICS_SETUP");                   \
    if ((_ctx)->dwAntiAliasFlags & (AA_MODE_SUPER | AA_MODE_SINGLECHUNK))           \
    {                                                                               \
        if (pDriverData->TwoDRenderingOccurred)                                     \
        {                                                                           \
            DPF_LEVEL(NVDBG_LEVEL_AA_INFO,"    2D rendering Occurred");             \
            (_ctx)->dwAntiAliasFlags |= AA_COPY_SCREEN;                             \
        }                                                                           \
        (_ctx)->dwAntiAliasFlags &= ~(AA_2D_COUNT_MASK | AA_MIX_MASK);              \
    }                                                                               \
    DPF_LEVEL(NVDBG_LEVEL_AA_INFO,"endm");                                          \
}
#define NV_AA_SEMANTICS_CHECK(_ctx)                                                 \
{                                                                                   \
    DPF_LEVEL(NVDBG_LEVEL_AA_INFO,"macro NV_AA_SEMANTICS_CHECK");                   \
    if ((_ctx)->dwAntiAliasFlags & (AA_MODE_SUPER | AA_MODE_SINGLECHUNK))           \
    {                                                                               \
        if (pDriverData->TwoDRenderingOccurred)                                     \
        {                                                                           \
            DPF_LEVEL(NVDBG_LEVEL_AA_INFO,"    2D rendering Occurred");             \
            (_ctx)->dwAntiAliasFlags += AA_2D_ADD;                                  \
            if (((_ctx)->dwAntiAliasFlags & AA_2D_COUNT_MASK) > AA_2D_MAX)          \
            {                                                                       \
                DPF_LEVEL(NVDBG_LEVEL_AA_INFO,"    Invalid Semantics Tagged");      \
                (_ctx)->dwAntiAliasFlags &= ~(AA_2D_COUNT_MASK | AA_ENABLED_MASK);  \
                (_ctx)->dwAntiAliasFlags |=  AA_2D_ADD | AA_INVALID_SEMANTICS;      \
                NV_FORCE_TRI_SETUP(_ctx);                                           \
                NV_D3D_GLOBAL_SAVE();                                               \
                nvFlushDmaBuffers();                                                \
                NV_D3D_GLOBAL_SETUP();                                              \
                pDriverData->lpLast3DSurfaceRendered = 0;                           \
                nvSetD3DSurfaceState (_ctx);                                        \
            }                                                                       \
            else                                                                    \
            {                                                                       \
                if (!((_ctx)->dwAntiAliasFlags & AA_PENDING_DATA))                  \
                {                                                                   \
                    (_ctx)->dwAntiAliasFlags |= AA_COPY_SCREEN;                     \
                }                                                                   \
            }                                                                       \
        }                                                                           \
    }                                                                               \
    DPF_LEVEL(NVDBG_LEVEL_AA_INFO,"endm");                                          \
}

typedef struct
{
    // bank 0
    DWORD dwChunkWidth;         // screen chunk width
    DWORD dwChunkHeight;        // screen chunk height
    DWORD dwSuperAddr;          // address of super buffer
    DWORD dwSuperOffset;        // aligned HW offset of super buffer
    DWORD dwSuperPitch;         // super surface pitch
    DWORD dwSuperWidth;         // super surface dimensions
    DWORD dwSuperHeight;        // super surface dimensions
    DWORD dwBPP;                // bytes per pixel
    // bank 1
    union {
        DWORD dwCapAddr;        // capture buffer address (chunking / edge)
        DWORD dwSuperZAddr;     // z buffer (single chunk)
    };
    union {
        DWORD dwCapSize;        // capture buffer size (chunking / edge)
        DWORD dwSuperZOffset;   // z buffer offset (single chunk)
    };
    union {
        DWORD dwCapIndex;       // capture index (chunking / edge)
        DWORD dwSuperZPitch;    // z buffer offset (single chunk)
    };
    float fAAScaleX;            // super sample in X
    float fAAScaleY;            // super sample in Y
    DWORD dwAASuperMode;        // lut index
    DWORD dwCFill;              // c fill     (super)
    DWORD dwZFill;              // z fill     (super)
} NVAACONTEXT,*PNVAACONTEXT;

/*
 * --------------------------------------------------------------------------
 * Define NV4 specific texture heap constants
 * --------------------------------------------------------------------------
 */
#define NV_MIN_TEXTURE_SIZE         ((1 * 2) * 1)
#define NV_MIN_TEXTURE_HEAP_SIZE    0x00100000
#define NV_TEXTURE_PAD              512
#define NV_TEXTURE_OFFSET_ALIGN     255

/*
 * Define Application DirectX version levels.
 */
#define APP_VERSION_DX2             0
#define APP_VERSION_DX3             0
#define APP_VERSION_DX5             1
#define APP_VERSION_DX6             2

/*
 * Define texture attributes
 */
#define NV4_TEXDEFAULT_AUTOMIPMAPTIME   3           // # of scenes before automips kick in
#define NV4_TEXDEFAULT_MAXBLTCOUNT      10          // # of CPU swizzle blits to take before staging
#define NV3_TEXDEFAULT_MIPSPERSCENE     3           // # if auto mips we allow to be generated per scene

#ifdef NV_TEX2
/*
 * texture manager 2.0 flags
 */
#define NV4_TEXCOUNT_SWIZZLE            8           // # of possible swizzle surface (must be power of 2)

#define NV4_TEXFLAG_SWIZZLE_INDEX       0x0000000f  // index of current swizzle surface
#define NV4_TEXFLAG_SWIZZLE_INDEX_MAX   0x000000f0  // max index of current swizzle surface

#define NV4_TEXFLAG_LINEAR_VALID        0x00010000  // linear copy has valid texels (up to date)
#define NV4_TEXMASK_LINEAR_LOCATION     0x00000f00  // linear copy location (vid,agp or pci)
#define NV4_TEXMASK_LINEAR_SHIFT        8

#define NV4_TEXFLAG_HANDLE_ALLOCATED    0x00020000  // Texture handle allocated for this texture.
#define NV4_TEXFLAG_AUTOMIPMAP          0x00040000  // we are allowed to automipmap this texture
#define NV4_TEXFLAG_AUTOMIPMAP_VALID    0x00080000  // we have valid automipmapped texels
#define NV4_TEXFLAG_USER_MIP_LEVEL      0x00100000  // This texture is a user defined mip level.
#define NV4_TEXFLAG_PALETTIZED          0x00200000  // set if texture is 8-bit palettized

#define NV4_TEXLOC_NOWHERE              0           // surface is not allocated
#define NV4_TEXLOC_VID                  1           // surface is (or may go) in video  (hw r/w)
#define NV4_TEXLOC_AGP                  2           // surface is (or may go) in agp    (hw r/-)
#define NV4_TEXLOC_PCI                  4           // surface is (or may go) in pci    (hw r/-)
#define NV4_TEXLOC_SYS                  8           // surface is (or may go) in sysmem (hw -/-)
#define NV4_TEXLOC_ANYWHERE             15          // texture may go anywhere

/*
 * swizzle surface flags
 */
#define NV4_TEXMASK_LOCATION            0x0000000f  // location of swizzle texture (do not move - hard coded)
#define NV4_TEXMASK_MIPMASK             0x0000fff0  // mask if updated mipmaps for this texture (0 means valid)
#define NV4_TEXFLAG_VALID               0x00010000  // texture is valid (mips dont have to be all valid)

/*
 * handy macros
 */
#define TEX_SWIZZLE_INDEX(flags)        ((flags) & NV4_TEXFLAG_SWIZZLE_INDEX)
#define TEX_SWIZZLE_ADDR(tex,flags)     ((tex)->dwSwizzleAddr[TEX_SWIZZLE_INDEX(flags)])
#define TEX_SWIZZLE_OFFSET(tex,flags)   ((tex)->dwSwizzleOffset[TEX_SWIZZLE_INDEX(flags)])
#define TEX_SWIZZLE_FLAGS(tex,flags)    ((tex)->dwSwizzleFlags[TEX_SWIZZLE_INDEX(flags)])
#define TEX_SWIZZLE_DATE(tex,flags)     ((tex)->dwSwizzleDate[TEX_SWIZZLE_INDEX(flags)])

#else //NV_TEX2
/*
 * texture manager 1.0 flags
 */
#define NV3_TEXTURE_VIDEO_MEMORY        0x00000001
#define NV3_TEXTURE_BLIT_UPDATE         0x00000002  // Texture modified with a DDRAW blit.
#define NV4_TEXTURE_CAN_AUTO_MIPMAP     0x00000004  // Specifies a texture can be automipmaped.
#define NV3_TEXTURE_MODIFIED            0x00000008  // texture memory has changed - force a internal reload
#define NV3_TEXTURE_IS_LOCKED           0x00000010  // texture is currently locked 
#define NV3_TEXTURE_MUST_RELOAD         0x00000020  // force current buffre DMA before reload (blt during scene)
#define NV3_TEXTURE_HANDLE_ALLOCATED    0x00000040  // Texture handle allocated for this texture.
#define NV3_TEXTURE_SURFACE_ALLOCATED   0x00000080  // Texture surface allocated for this texture.
#define NV3_TEXTURE_USER_ALLOCATED      0x00000100  // User system memory texture surface is allocated.
#define NV3_TEXTURE_INTERNAL_ALLOCATED  0x00000200  // Internal texture surface is allocated.
#define NV3_TEXTURE_USER_MIP_LEVEL      0x00000400  // This texture is a user defined mip level.
#define NV4_TEXTURE_WAS_IN_VIDEO_MEMORY 0x00000800  // texture was move from video memory by AA
#define NV4_TEXTURE_AUTOMIPMAPPED       0x00001000  // texture mipmaps are driver supplied (auto mipmapped)
#endif //NV_TEX2

/*
 * common flags
 */

struct _def_nv_d3d_texture
{
    /*
     * Second (final) pass texture definition.
     */
    DWORD                       pid;                /* Process ID */
    DWORD                       hContext;           /* Context Handle */
    DWORD                       hMipBaseTexture;    /* User Mip Base Texture Handle */

    LPDDRAWI_DIRECTDRAW_GBL     lpDriverObject;     /* Driver object used for allocating memory */
#ifndef WINNT
    LPDIRECTDRAWSURFACE         lpDDS;              /* Texture Surface */
#else
    HANDLE                      hDDS;               /* NT texture handle */
#endif
    LPDDRAWI_DDRAWSURFACE_LCL   lpLcl;              /* Texture */
    DWORD                       dwDriverData;       /* address of pDriverData */

    /*
     * Basic texture characteristics.
     */
    DWORD                       dwWidth;
    DWORD                       dwHeight;
    DWORD                       dwPitch;
#ifdef NV_TEX2
    DWORD                       dwBPP;               // bytes per pixel
    DWORD                       dwLockUpdateCount;   // # of lock performed
    DWORD                       dwBlitUpdateCount;   // # of blits performed
#else
    DWORD                       dwUpdateCount;       /* # of texture blts / locks performed */
#endif

    /*
     * MipMap Texture Information.
     */
    DWORD                       dwMipMapLevels;
    DWORD                       dwMipMapBaseU;
    DWORD                       dwMipMapBaseV;
    DWORD                       dwMipMapSizeBytes;

    /*
     * Internal hardware texture format information.
     */
    HANDLE                      hTextureMem;
    DWORD                       dwTextureFlags;
    DWORD                       dwTextureContextDma;
    DWORD                       dwTextureColorFormat;
#ifdef NV_TEX2
    DWORD                       dwSwizzleAddr  [NV4_TEXCOUNT_SWIZZLE];    // actual swizle surface address (not aligned)
    DWORD                       dwSwizzleOffset[NV4_TEXCOUNT_SWIZZLE];    // offset of swizzle surface
    DWORD                       dwSwizzleFlags [NV4_TEXCOUNT_SWIZZLE];    // flags for this surface
    DWORD                       dwSwizzleDate  [NV4_TEXCOUNT_SWIZZLE];    // scene it retires in
    DWORD                       dwRetireDate   [NV4_TEXCOUNT_SWIZZLE+1];  // when the texture surface retires, extra is for linear
    DWORD                       dwLinearAddr;       // actual linear address (not aligned)
    DWORD                       dwLinearOffset;     // offset of linear surface
#else
    DWORD                       dwTexturePointer;
    DWORD                       dwTextureOffset;    /* Texture offset within heap */
#endif

    /*
     * Reference pointer to texture data.
     * This is used only for validation and checking to see if the texture
     * pointer in the texture surface has changed.  This may occur during
     * texture flipping.  If this pointer does not match what is stored in
     * in the texture surface, then the texture needs to be reloaded.
     */
    FLATPTR                     fpTexture;

#ifdef NV_TEX2
    /*
     * auto mipmap code
     */
    DWORD                       dwAutoMipMapScene;      // scene # that automips kick in
    DWORD                       dwAutoMipMapCount;      // mipmap count (when activated)
    /*
     * dirty rectangle
     */
    DWORD                       dwDirtyX0;              // dirty rect (for efficient lin<->swiz)
    DWORD                       dwDirtyY0;              // dirty rect (for efficient lin<->swiz)
    DWORD                       dwDirtyX1;              // dirty rect (for efficient lin<->swiz)
    DWORD                       dwDirtyY1;              // dirty rect (for efficient lin<->swiz)
#else // !NV_TEX2
    /*
     * User texture surface allocated for this texture.
     */
    HANDLE                      hUserTexture;
    FLATPTR                     fpUserTexture;
#endif // !NV_TEX2

    /*
     * Application Texture Bug Workarounds.
     * Just handy for fixing developers bugs while we wait for the
     * real fix.
     */
    DWORD                       dwDebugFlags01;
    DWORD                       dwDebugFlags02;

    PNVD3DTEXTURE               pTexturePrev;
    PNVD3DTEXTURE               pTextureNext;
};

/*
 * Look in D3DTYPES.H to determine what this value is.
 * I'm not including any of the stipple state on purpose.
 */
#ifdef  NVD3D_DX6
#define D3D_RENDERSTATE_MAX     D3DRENDERSTATE_WRAP7
#else   // !NVD3D_DX6
#define D3D_RENDERSTATE_MAX     D3DRENDERSTATE_FLUSHBATCH
#endif  // !NVD3D_DX6

/*
 * Cache aligned inner loop context data.
 * Note also, that the hardware registers should be defined in the order that
 * they appear in the in the object class.
 */
struct _def_nv_d3d_context_inner_loop
{
// bank 0       !!!Don't change bank or element ordering!!!
    /*
     * Current rendering state.
     */
    DWORD                       dwColorKey;
    DWORD                       dwTextureOffset;       // Offset within DMA context to start of texture.
    DWORD                       dwFormat;
    DWORD                       dwFilter;              // Current filtering state.
    DWORD                       dwBlend;
    DWORD                       dwControl;
    DWORD                       dwFogColor;             // Current fog color.
    DWORD                       _dwPad0[1];

// bank 1       !!!Don't change bank or element ordering!!!!
#ifdef  SPEC_HACK
    /*
     * Values used for temporarily disabling hardware specular enable bit when
     * the vertex specular components do not contribute any specular highlight.
     * This is so that we can get two pixels per clock out of the DX5 class
     * whenever possible.

     */
    DWORD                       dwCurrentBlend;
    DWORD                       dwCurrentSpecularState;
    DWORD                       dwNoSpecularTriangleCount;
    DWORD                       _dwPad1[5];
#endif  // SPEC_HACK
};
#ifdef  NVD3D_DX6
typedef struct _def_nv_d3d_texture_stage_state NVD3DTEXSTAGESTATE, *PNVD3DTEXSTAGESTATE;
struct _def_nv_d3d_texture_stage_state
{
    DWORD   dwTextureMap;
    DWORD   dwAddressU;
    DWORD   dwAddressV;
    DWORD   dwMagFilter;
    DWORD   dwMinFilter;
    DWORD   dwMipFilter;
    DWORD   dwColorOp;
    DWORD   dwColorArg1;
    DWORD   dwColorArg2;
    DWORD   dwAlphaOp;
    DWORD   dwAlphaArg1;
    DWORD   dwAlphaArg2;
    DWORD   dwBumpEnvMat00;
    DWORD   dwBumpEnvMat01;
    DWORD   dwBumpEnvMat10;
    DWORD   dwBumpEnvMat11;
    DWORD   dwTexCoordIndex;
    DWORD   dwBorderColor;
    DWORD   dwMipMapLODBias;
    DWORD   dwMaxMipLevel;
    DWORD   dwMaxAnisotropy;
    DWORD   dwBumpEnvlScale;
    DWORD   dwBumpEnvlOffset;
    /*
     * Extra per stage state.
     */
    BYTE    bStageNumber;
    BYTE    bWrapU;
    BYTE    bWrapV;
    BYTE    bLODBias;
};

typedef struct _def_nv_d3d_multi_texture_state NVD3DMULTITEXTURESTATE, *PNVD3DMULTITEXTURESTATE;
struct _def_nv_d3d_multi_texture_state
{
    DWORD   dwTextureOffset[2];
    DWORD   dwTextureFormat[2];
    DWORD   dwTextureFilter[2];
    DWORD   dwCombine0Alpha;
    DWORD   dwCombine0Color;
    DWORD   dwCombine1Alpha;
    DWORD   dwCombine1Color;
    DWORD   dwCombineFactor;
    DWORD   dwBlend;
    DWORD   dwControl0;
    DWORD   dwControl1;
    DWORD   dwControl2;
    DWORD   dwFogColor;

    /*
     * The hardware requires the the UVx texture coordinates match up with
     * TEXTUREx.  i.e. tu0, tv0 are the coordinates for TEXTURE0, and tu1,
     * tv1 are the coordinates for TEXTURE1.
     * To simplify life, I am making TEXTURE0 always correspond to texture
     * stage 0, TEXTURE1 will always correspond to texture stage 1, etc.
     * However, there does not always have to be a one to one correspondence
     * to the Texture stage and the order in which the UV coordinate pairs
     * appear in the FVF Vertices.  The texture stage state defines which
     * set of UV coordinates applies to the texture in that stage.  This
     * allows for a single set of UV coordinates to be used for the multiple
     * texture stages.
     * The following offset array gives the byte offset from the start of the
     * FVF UV corrdinate pairs to the actual UV coordinates to be used for
     * a particular UVx pair.  They are initialized when the multi-texture state
     * is caclulated.
     */
    DWORD   dwUVOffset[8];

    /*
     * This flag defines if the current texture stage state is
     * valid.  This flag basicly defines whether or not multi-texturing
     * is possible with the current state.
     * This flag can be used during the ValidateTextureStageState callback
     * to report whether we can mulit-texture with the current state.
     * If the state is not valid, ddrval will contain the error that should
     * be returned during a ValidateTextureStageState callback.
     */
    BOOL    bTSSValid;
    HRESULT ddrval;
};
#endif  // NVD3D_DX6

/*
 * Define the context flags.
 */
#define NV_CONTEXT_ZETA_BUFFER_MISMATCH         0x00000001
#define NV_CONTEXT_TBLEND_UNINITIALIZED         0x00000002  // This flag will be cleared after the first TBLEND call.

#ifdef NV_CONTROLTRAFFIC
#define NV_CT_DISABLED          0x00000000
#define NV_CT_ENABLED           0x00000001
#define NV_CT_FRONT             0x00000002
#define NV_CT_DRAWPRIM          0x00010000
#endif //NV_CONTROLTRAFFIC

/*
 * Define the context header.
 */
struct _def_nv_d3d_context
{
    /*
     * The following structure is designed for a fast copy from the context data structure
     * to cache aligned global data for fast access within the inner loops.
     */
    NVD3DCONTEXTINNERLOOP       ctxInnerLoop;           // Context data used within inner loops.
    NVD3DCONTEXTINNERLOOP       ctxInnerLoopShadow;     // hw image of above

    /*
     * Application DX version.
     */
    DWORD                       dwDXVersionLevel;       // Tells the applications DX version level.
                                                        // 0=DX2, DX3; 1=DX5; 2=DX6

    /*
     * D3D Context Flags.
     */
    DWORD                       dwContextFlags;         // Various context related flags.

    /*
     * Default texture state for this context.
     */
    DWORD                       dwDefaultTextureContextDma;  // Default texture rendering object.
    DWORD                       dwDefaultTextureBase;
    DWORD                       dwDefaultTextureOffset;      // Offset within DMA context to start of default texture.
    DWORD                       dwDefaultTextureColorFormat; // Default texture format.

#ifdef NV_TEX2
    PNVD3DTEXTURE               pTexture0;              // texture HW is currently setup with
    PNVD3DTEXTURE               pTexture1;              // texture HW is currently setup with
    
    DWORD                       dwTexturePaletteStagingHandle;
    DWORD                       dwTexturePaletteAddr;   // address of palette staging area
    DWORD                       dwTexturePaletteOffset; // offset of palette staging area
#endif

    /*
     * Surface clip rectangle.
     */
    struct  _surface_viewport                           // Surface clip region.
    {
        union
        {
            DWORD   dwClipHorizontal;
            struct  _clip_horizontal
            {
                WORD    wX;                             // Surface left edge
                WORD    wWidth;                         // Surface Clip Width
            } clipHorizontal;
        };
        union
        {
            DWORD   dwClipVertical;
            struct  _clip_vertical
            {
                WORD    wY;                             // Surface top edge
                WORD    wHeight;                        // Surface Clip Height
            } clipVertical;
        };
    } surfaceViewport;

    /*
     * Surface Pitch
     */
    union
    {
        DWORD   dwSurfacePitch;                         // Surface pitches.
        struct _surface_pitch
        {
            WORD    wColorPitch;                        // Render target pitch.
            WORD    wZetaPitch;                         // Zeta buffer pitch.
        } surfacePitch;
    };

    /*
     * Current Surface.
     */
    DWORD                       dwSurfaceAddr;
    DWORD                       dwSurfaceOffset;
    DWORD                       dwSurfaceFormat;

    /*
     * Current Z Buffer address.
     */
    DWORD                       ZBufferAddr;            // Pointer to Z Buffer
    DWORD                       ZBufferOffset;

    /*
     * Current rendering state.
     */
    DWORD                       dwStateChange;
    DWORD                       dwRenderState[D3D_RENDERSTATE_MAX];
    DWORD                       dwFogTableMode;         // Current fog table mode.
    float                       fFogTableStart;         // Current fog table start.
    float                       fFogTableEnd;           // Current fog table end.
    float                       fFogTableDensity;       // Current fog table density.
    float                       fFogTableLinearScale;   // Current fog table linear scale.
    DWORD                       dwFunctionLookup;       // Lookup index for the appropriate render function.
    DWORD                       dwMipMapLODBias;        // LOD Bias value to stuff into the hardware.
    DWORD                       dwSceneCount;           // # of scenes we have rendered in this context
    DWORD                       dwMipMapsInThisScene;   // # of auto mips generated in this scene

    /*
     * Anti aliasing context storage
     */
    DWORD                       dwAntiAliasFlags;
    NVAACONTEXT                *pAAContext;

    /*
     * Optimized Z data
     */
#ifdef NV_CONTROLTRAFFIC
    DWORD                       dwCTFlags;
#endif

    /*
     * Miscellaneous context data.
     */
    LPDDRAWI_DDRAWSURFACE_LCL   lpLcl;                  // Rendering surface.
    LPDDRAWI_DDRAWSURFACE_LCL   lpLclZ;                 // Z surface.
    DWORD                       pid;                    // Process ID
    NVD3DSTATESET               overrides;              // Render state override bits.

    DWORD                       dwContextReset;         // This is set when a mode switch occurs.
    PNVD3DCONTEXT               pContextPrev;
    PNVD3DCONTEXT               pContextNext;

#ifdef  NVD3D_DX6
    /*
     * DX6 Texture Stage State.
     */
    DWORD                       dwStageCount;           // Number of enabled/valid texture stages.
    BOOL                        bUseDX6Class;           // Use DX6 Multi-Texture Triangle Class.
    BOOL                        bUseTBlendSettings;     //
    DWORD                       dwTBlendCombine0Alpha;
    DWORD                       dwTBlendCombine0Color;
    NVD3DMULTITEXTURESTATE      mtsState;               // DX6 Multi-Texture Triangle State.
    NVD3DMULTITEXTURESTATE      mtsShadowState;         //  for state update optimizations
    NVD3DTEXSTAGESTATE          tssState[8];            // Texture Stage State.

    /*
     * Near/Far Clip planes.
     */
    D3DVALUE                    dvWNear;
    D3DVALUE                    dvWFar;
    D3DVALUE                    dvRWFar;                // Reciprocal of WFar.
#endif  // NVD3D_DX6
#ifdef WINNT
    /*
     * Need this for Winnt to access the PDEV state struct maintained by display
     * driver. PDEV also contains a ptr to the GLOBALDATA struct.
     *
     */
    PDD_DIRECTDRAW_GLOBAL        lpDD;
#endif // #ifdef WINNT
};

/*
 * Function Prototypes
 */
void nvD3DCreateObjects(void);
void nvD3DDestroyObjects(void);
#ifndef WINNT
DWORD __stdcall Clear32(LPD3DHAL_CLEARDATA);
#endif

void  nvSetD3DSurfaceState(PNVD3DCONTEXT);
void  nvSetD3DSurfaceViewport(PNVD3DCONTEXT);
void  nvSetDx5TriangleState(PNVD3DCONTEXT);
DWORD nvRenderNoOp(DWORD, LPBYTE, LPD3DTLVERTEX);
DWORD nvRenderTriangle(DWORD, LPBYTE, LPD3DTLVERTEX);
DWORD nvRenderPoint(DWORD, LPBYTE, LPD3DTLVERTEX);
DWORD nvRenderLine(DWORD, LPBYTE, LPD3DTLVERTEX);

#ifndef WINNT
DWORD nvClear(LPD3DHAL_CLEARDATA);
#endif
void  nvTriangle(DWORD, LPD3DTRIANGLE, LPD3DTLVERTEX);
void  nvIndexedTriangle(DWORD, WORD, LPWORD, LPD3DTLVERTEX);

#ifdef  MULTI_MON
void D3DDestroyTextureContexts(void);
#endif  // MULTI_MON
#ifndef WINNT
/*
 * Non-Indexed Primitive Routines.
 */
DWORD __stdcall DrawOnePrimitive32(LPD3DHAL_DRAWONEPRIMITIVEDATA);
DWORD __stdcall DrawPrimitives32(LPD3DHAL_DRAWPRIMITIVESDATA);
DWORD nvDrawOnePrimitive(LPD3DHAL_DRAWONEPRIMITIVEDATA);
DWORD nvDrawPrimitives(LPD3DHAL_DRAWPRIMITIVESDATA);

/*
 * Indexed Primitive Routines.
 */
DWORD __stdcall DrawOneIndexedPrimitive32(LPD3DHAL_DRAWONEINDEXEDPRIMITIVEDATA);
DWORD nvDrawOneIndexedPrimitive(LPD3DHAL_DRAWONEINDEXEDPRIMITIVEDATA);
#endif
void nvDrawLine(DWORD, DWORD, LPD3DTLVERTEX);
void nvIndexedLine(DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDrawPointList(DWORD, LPD3DTLVERTEX);

void nvIndexedWireframeTriangle(DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDrawWireframeTriangle(DWORD, DWORD, LPD3DTLVERTEX);
void nvIndexedPointTriangle(DWORD, LPWORD, DWORD, LPD3DTLVERTEX);
void nvDrawPointTriangle(DWORD, DWORD, LPD3DTLVERTEX);

#ifdef  NVD3D_DX6
DWORD FAR PASCAL DrawPrimitives2(LPD3DHAL_DRAWPRIMITIVES2DATA);
DWORD FAR PASCAL Clear2(LPD3DHAL_CLEAR2DATA pc2d);

void  nvSetMultiTextureHardwareState(void);
DWORD FAR PASCAL nvValidateTextureStageState(LPD3DHAL_VALIDATETEXTURESTAGESTATEDATA);
DWORD nvDrawPrimitives2(LPD3DHAL_DRAWPRIMITIVES2DATA);
DWORD nvClear2(LPD3DHAL_CLEAR2DATA pc2d);

void nvFVFDrawLine(DWORD, DWORD, LPBYTE);
void nvFVFIndexedLine(DWORD, LPWORD, DWORD, LPBYTE);
void nvFVFDrawPointList(DWORD, LPBYTE);

void nvFVFIndexedWireframeTriangle(DWORD, LPWORD, DWORD, LPBYTE);
void nvFVFDrawWireframeTriangle(DWORD, DWORD, LPBYTE);
void nvFVFIndexedPointTriangle(DWORD, LPWORD, DWORD, LPBYTE);
void nvFVFDrawPointTriangle(DWORD, DWORD, LPBYTE);

void nvSetDx6MultiTextureState(PNVD3DCONTEXT);
#endif  // NVD3D_DX6

/*
 * External data.
 */
extern PFNRENDERPRIMITIVEOP     pfnRenderPrimitiveOperation[];
extern DWORD                    uvInterleave[];

#ifdef  NVD3D_DX6
extern PFND3DPARSEUNKNOWNCOMMAND    fnD3DParseUnknownCommandCallback;
/* - not used anymore ?
extern LPNVFVFDRAWPRIM              fnMultiDrawTriangleList[];
extern LPNVFVFDRAWPRIM              fnMultiDrawTriangleStrip[];
extern LPNVFVFDRAWPRIM              fnMultiDrawTriangleFan[];
extern LPNVFVFINDEXEDPRIM           fnMultiIndexedTriangleList[];
extern LPNVFVFINDEXEDPRIM           fnMultiIndexedTriangleStrip[];
extern LPNVFVFINDEXEDPRIM           fnMultiIndexedTriangleFan[];
*/
#endif  // NVD3D_DX6

/*
 * Texture Staging Manager Functions
 */
//#define TM_STAGER_IN_AGP
#define TM_STAGESPACE              (2*1024*1024) // 2048k

#define NV_PN_TEX_RETIRE           0    // depreciated in for nv_tex2
#define NV_PN_SPOOFED_FLIP_COUNT   4
#define NV_PN_TEXTURE_RETIRE       8

void __stdcall nvFastCopy          (DWORD,DWORD,DWORD);
DWORD nvPlacePsuedoNotifier        (DWORD,DWORD);
DWORD nvTMCreate                   (void);
void  nvTMDestroy                  (void);
DWORD nvTMAllocTextureStagingSpace (DWORD,DWORD*,DWORD*);
void  nvTMUpdateRetirementDate     (DWORD);
void  nvTMVideoTextureBlt          (DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD);

/*
 * Embedded Profiler Functions
 */
#ifdef NV_PROFILE

#if 0
/*
 * Constants
 */
#define NVP_T_CLOCK                 0x00    // 00000001
#define NVP_T_FLUSH                 0x01    // 00000002
#define NVP_T_SCENE                 0x02    // 00000004
#define NVP_T_FLIP                  0x03    // 00000008
#define NVP_T_DP2                   0x04    // 00000010
#define NVP_T_FREECOUNT             0x05    // 00000020
#define NVP_T_TSTAGE                0x06    // 00000040

#define NVP_E_PUT                   0x10    // 00010000
#define NVP_E_BEGINSCENE            0x11    // 00020000

#define NVP_C_PENDING               0x18    // 01000000

#define NVP_EVERYTHING                       0x0103007f

/*
 * Macros
 */
#define _NVP_PUSH  __asm push eax __asm push edx __asm push ebx
#define _NVP_POP   __asm pop ebx __asm pop edx __asm pop eax
#define _NVP_SYNC  __asm xchg al,al __asm nop __asm nop
#define _NVP_RDTSC _NVP_SYNC __asm _emit 0x0f __asm _emit 0x31

#define NVP_START(_t) { nvpTime[_t]=0; NVP_RESTART(_t); }
#define NVP_RESTART(_t) { _NVP_PUSH _NVP_RDTSC __asm mov ebx,(OFFSET nvpTime+(_t)*8) __asm sub [ebx],eax __asm sbb [ebx+4],edx _NVP_POP }
#define NVP_STOP(_t)  { _NVP_PUSH _NVP_RDTSC __asm mov ebx,(OFFSET nvpTime+(_t)*8) __asm add [ebx],eax __asm adc [ebx+4],edx _NVP_POP }

extern __int64 nvpTime[256];

/*
 * protoypes
 */
void nvpFlush    (void);
void nvpCreate   (void);
void nvpDestroy  (void);
void nvpLogCount (DWORD id,__int64 count);
void nvpLogTime  (DWORD id,__int64 time);
void nvpLogEvent (DWORD id);
#endif // 0

#if 1

/*
 * Constants
 */
#define NVP_E_BEGINSCENE            0
#define NVP_E_PUT                   1
#define NVP_E_TRIDISP               2

#define NVP_T_SCENE                 3
#define NVP_T_FREECOUNT             4
#define NVP_T_FLIP                  5
#define NVP_T_TSTAGE                6
#define NVP_T_DP2                   7
#define NVP_T_HDFLUSH               8
#define NVP_T_TEXWAIT               9
#define NVP_T_TEXSWBLT              10
#define NVP_T_TEXHWBLT              11
#define NVP_T_INNERLOOP             12
#define NVP_T_BUILDLOOP             13

#define NVP_C_PENDING               14
#define NVP_C_BLTSIZE               15
#define NVP_C_1PIXCLK               16
#define NVP_C_2PIXCLK               17
#define NVP_C_TRIAREA               18
#define NVP_C_PIXPERSEC             19
#define NVP_C_TRICOUNT              20

/*
 * Macros
 */
#define _NVP_PUSH  __asm push eax __asm push edx __asm push ebx
#define _NVP_POP   __asm pop ebx __asm pop edx __asm pop eax
#define _NVP_SYNC  __asm xchg al,al __asm nop __asm nop
#define _NVP_RDTSC _NVP_SYNC __asm _emit 0x0f __asm _emit 0x31

#define NVP_START(_t) { nvpTime[_t]=0; NVP_RESTART(_t); }
#define NVP_RESTART(_t) { _NVP_PUSH _NVP_RDTSC __asm mov ebx,(OFFSET nvpTime+(_t)*8) __asm sub [ebx],eax __asm sbb [ebx+4],edx _NVP_POP }
#define NVP_STOP(_t)  { _NVP_PUSH _NVP_RDTSC __asm mov ebx,(OFFSET nvpTime+(_t)*8) __asm add [ebx],eax __asm adc [ebx+4],edx _NVP_POP }

extern __int64 nvpTime[256];
extern DWORD   nvpEnableBits;
extern DWORD   nvpTriPerSecWindow;

/*
 * functions
 */
void nvpCreate   (void);
void nvpDestroy  (void);
void nvpFlush    (void);
void nvpLogEvent (DWORD id);
void nvpLogCount (DWORD id,int count);
void nvpLogTime  (DWORD id,__int64 time);

#endif // 1

#endif // NV_PROFILE

#ifdef NV_FASTLOOPS
/*
 * nv4comp.h
 */
typedef void (__stdcall *PFNDRAWPRIMITIVE)(DWORD dwPrimCount,LPWORD pIndices,DWORD dwStrides,LPBYTE pVertices);

#endif //NV_FASTLOOPS

#endif  // NV4
#endif  // _NV4DINC_H_
