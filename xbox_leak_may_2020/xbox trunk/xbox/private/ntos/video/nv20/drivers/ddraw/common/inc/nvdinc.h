/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVDINC.H                                                         *
*   NV4 specific direct 3d structure definitions.                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/97 - created.                     *
*                                                                           *
\***************************************************************************/

#ifndef _NVDINC_H_
#define _NVDINC_H_

#ifndef __cplusplus
#error C++ compiler needed
#endif

/*****************************************************************************
 * macros
 ****************************************************************************/

/*
 * Always build for D3D.
 * Always build a non-syncing driver.
 */
#define NVD3D       1
#define D3D_NOSYNC  1

// macros to push & restore current context
#define NVPUSH(pType,pPtr)        pType __##pPtr##_TempStorage__ = (pPtr);
#define NVPOP(pType,pPtr)         (pPtr) = __##pPtr##_TempStorage__;

// basically this needs to be the union of arguments required
// by all the various DP2 functions
#define NV_DP2FUNCTION_ARGLIST  PNVD3DCONTEXT        pContext,              \
                                LPD3DHAL_DP2COMMAND *ppCommands,            \
                                LPBYTE               pCommandBufferEnd,     \
                                LPDWORD              pdwDP2RStates,         \
                                DWORD                dwDP2Flags

#define NV_INNERLOOP_ARGLIST    PNVD3DCONTEXT        pContext,              \
                                DWORD                dwCount

#define NV_INNERLOOP_ARGS       pContext,                                   \
                                dwCount

typedef HRESULT (*LPNVDP2FUNCTION) (NV_DP2FUNCTION_ARGLIST);
typedef LPNVDP2FUNCTION     nvDP2FunctionTable[D3D_DP2OP_MAX+1];
typedef nvDP2FunctionTable *pnvDP2FunctionTable;

typedef DWORD (*PFNRENDERPRIMITIVEOP)         (NV_INNERLOOP_ARGLIST);
typedef void  (*LPNVINDEXEDPRIM)              (NV_INNERLOOP_ARGLIST);
typedef void  (*LPNVDRAWPRIM)                 (NV_INNERLOOP_ARGLIST);
typedef void  (__stdcall *LPNVFVFINDEXEDPRIM) (NV_INNERLOOP_ARGLIST);
typedef void  (__stdcall *LPNVFVFDRAWPRIM)    (NV_INNERLOOP_ARGLIST);

#define NV_UV_COORDINATE_COUNT_MAX  2

/*
 * --------------------------------------------------------------------------
 * Define texture heap constants
 * --------------------------------------------------------------------------
 */
#define NV_MIN_TEXTURE_SIZE                        ((1*2)*1)
#define NV_TEXTURE_PAD                             512
#define NV_TEXTURE_OFFSET_ALIGN                    255
#define NV_KELVIN_CUBEMAP_FACE_ALIGN               127            // alignment of subordinate faces -x thru -z, both compressed and not
#define NV_CELSIUS_CUBEMAP_FACE_ALIGN              127            // alignment of uncompressed subordinate faces -x thru -z
#define NV_CELSIUS_CUBEMAP_FACE_ALIGN_COMPRESSED   31             // alignment of compressed subordinate faces -x thru -z

/*
 * ScaledImageFromMemory size in limitation
 */
#define NV4_SCALED_IMAGE_CUTOFF         16

/*
 * Cache aligned inner loop context data.
 * Note also, that the hardware registers should be defined in the order that
 * they appear in the in the object class.
 */
typedef struct _def_nv_d3d_context_inner_loop
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
} NVD3DCONTEXTINNERLOOP, *PNVD3DCONTEXTINNERLOOP;

//---------------------------------------------------------------------------

typedef struct _def_nv_d3d_texture_stage_state
{
    DWORD   dwValue[D3D_TEXSTAGESTATE_MAX+1];
    DWORD   dwHandle;      // texture handle
    DWORD   dwLODBias;     // LOD bias value to feed to HW
    D3DMATRIX mTexTransformMatrix;
} NVD3DTEXSTAGESTATE, *PNVD3DTEXSTAGESTATE;

//---------------------------------------------------------------------------

typedef struct _def_nv_d3d_multi_texture_state
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
} NVD3DMULTITEXTURESTATE, *PNVD3DMULTITEXTURESTATE;

//---------------------------------------------------------------------------

// Surface clip rectangle.
typedef struct _SurfaceViewport {                   // Surface clip region
    union {
        DWORD   dwClipHorizontal;
        struct  _clip_horizontal {
            WORD    wX;                             // Surface left edge
            WORD    wWidth;                         // Surface Clip Width
        } clipHorizontal;
    };
    union {
        DWORD   dwClipVertical;
        struct  _clip_vertical {
            WORD    wY;                             // Surface top edge
            WORD    wHeight;                        // Surface Clip Height
        } clipVertical;
    };

    D3DVALUE dvMinZ;                                // Min/max of clip Volume
    D3DVALUE dvMaxZ;

    D3DVALUE dvWNear;
    D3DVALUE dvWFar;
    D3DVALUE dvRWFar;                               // Fudged reciprocal of WFar.
    D3DVALUE dvInvWFar;                             // the real reciprocal of WFar
} SurfaceViewport, *pSurfaceViewport;

//---------------------------------------------------------------------------

// vertex shader constants
typedef struct _VertexShaderConsts {
    // The zero padding is here to handle the constant addressing offsets in software
    // efficiently.  Rather than performing a bounds check upon each memory
    VSHADERREGISTER zeroPadding[128];
    VSHADERREGISTER vertexShaderConstants[NV_CAPS_MAX_VSHADER_CONSTS];
    VSHADERREGISTER zeroPadding2[128];
    VSHADERREGISTER vertexShaderTemp[NV_CAPS_MAX_VSHADER_TEMPS];
} VertexShaderConsts;

//---------------------------------------------------------------------------

// Vertex buffer offset parameters from DP2. These structure fields are used in place of passing parameters to the
// DP2 routines and to the inner loops. Sometimes the inner loops need to recalculate the offset. Passing in a precomputed
// offset made this difficult, because the inner loop did not have have access to the DP2 command.

// dwVertexBufferOffset is set in nvDrawPrimitives and nvDrawPrimitives2 and takes the place of the dwVertexBufferOffset argument
// to the DP2 routines.
//
// dwVStart is set by DP2 routines to the starting index, i.e. it's scaled by the stride of the VB. It's set to zero when not
// needed.
//
// dwTotalOffset is set by DP2 routines to be dwVertexBufferOffset + (dwVStart * Stride). Inner loops that change the stride
// of the VB must recompute this. It's provided by the DP2 routines for compatibility with the older inner loop code.

typedef struct _nv_d3d_context_dp2_param {
    DWORD           dwDP2Prim;                     // current prim
    DWORD           dwVertexBufferOffset;          // pdp2d->dwVertexOffset
    DWORD           dwVertexBufferOffsetSave;      // temp copy of pdp2d->dwVertexOffset
    DWORD           dwVStart;                      // lpPrim->wVStart
    DWORD           dwTotalOffset;                 // pdp2d->dwVertexOffset + (lpPrim->wVStart * Stride)
    DWORD           dwVertexLength;                // pdp2d->dwVertexLength;
    DWORD           dwEdgeFlags;                   // uhhh... take a wild guess
    DWORD           dwIndices;                     // address of indices (if indexed primitive, else NULL)
    DWORD           dwIndexStride;                 // index stride in bytes (2 or 4)
} NVD3DCONTEXTDP2PARAM, *PNVD3DCONTEXTDP2PARAM;

//---------------------------------------------------------------------------

typedef struct _nv_d3d_context_sysVB {
    CVertexBuffer  *pSysMemVB;
    DWORD           dwDefVBOffset;
    DWORD           dwSysMemVBStream;
    BOOL            bWriteOffset;
    BYTE            _dwPad0[3];
    DWORD           dwStreamDMACount;
    DWORD           _dwPad[3];
} NVD3DCONTEXTSYSVB, *PNVD3DCONTEXTSYSVB;

//---------------------------------------------------------------------------

typedef struct _nv_fog_data {
    DWORD           dwFogTableMode;
    float           fFogTableStart;
    float           fFogTableEnd;
    float           fFogTableDensity;
    float           fFogTableLinearScale;
} NVFOGDATA;

//---------------------------------------------------------------------------

extern "C++"
{

class CCommonState
{
public:
    enum
    {
        FOG_SOURCE_Z = 1,
        FOG_SOURCE_W = 2
    };

    // this area contains state that is common to all devices
    // this includes state for which the meaning is identical
    // as well as flags that are either identical in meaning or mutually exclusive in usage
    // when flags are neither identical nor mutually exclusive, we must ensure that they are only set for the correct class
    DWORD          dwDirtyFlags;                        // dirty state that needs to be resent to HW
    DWORD          dwStateFlags;                        // hardware state flags
    DWORD          dwTexCoordIndices;                   // texture coordinate indices used for texture unit 0 (0x0000ffff) and 1 (0xffff0000)
    DWORD          dwNumTexCoordsNeeded;                // number of texture coordinates needed for the currently set texture
    DWORD          dwNumActiveCombinerStages;           // number of enabled HW combiner stages
    DWORD          dwNextAvailableTextureUnit;          // which texture unit / combiner input will be used for the next texture
    DWORD          dwInlineVertexStride;                // stride of inline vertices (in bytes)
    DWORD          dwVertexOffset;                      // last vertex offset for whch the HW was programmed
    DWORD          dwFogSource;                         // indicates if we should use Z or W based fog
    DWORD          dwCombinerDP2Prim;                   // last DP2 Primitive for which combiners were programmed (affects point sprites)

    D3DVALUE       dvZScale;                            // the scale applied to a Z in [0...1] to make it fill the z buffer
    D3DVALUE       dvInvZScale;                         // the inverse

    CVertexShader *pVertexShader;                       // last vertex shader for which the HW was programmed

    CVertexBuffer *ppStreams          [NV_CAPS_MAX_STREAMS];         // last stream set for which hardware was programmed
    DWORD          dwTexUnitToTexStageMapping[NV_CAPS_MAX_TEXTURES]; // which D3D texture stage (if any) is being handled by this HW texture unit
    DWORD          dwTextureUnitStatus[NV_CAPS_MAX_TEXTURES];        // what is each hardware texture unit being used for?
    CTexture      *pTextureInUse      [NV_CAPS_MAX_TEXTURES];        // texture (if any) currently in use by HW texture unit i

    CSuperTri      SuperTri;

    DWORD          alphacull_mode;                                   // tracks which alpha cull mode I'm using for dirty bit checks

    DWORD          dwColorICW         [NV_CAPS_MAX_COMBINERS];       // color combiner input control words
    DWORD          dwColorOCW         [NV_CAPS_MAX_COMBINERS];       // color combiner output control words
    DWORD          dwAlphaICW         [NV_CAPS_MAX_COMBINERS];       // color combiner input control words
    DWORD          dwAlphaOCW         [NV_CAPS_MAX_COMBINERS];       // alpha combiner output control words

    D3DMATRIX      mTransform;                                       // supertri transform matrix
    DWORD          specfog_cw[2];
};

//---------------------------------------------------------------------------

class CHardwareState : public CCommonState
{
public:
#if (NVARCH >= 0x010)
    CCelsiusState celsius;
#endif // NVARCH == 0x010
#if (NVARCH >= 0x020)
    CKelvinState  kelvin;
#endif // NVARCH == 0x020
};

//---------------------------------------------------------------------------

typedef enum _D3DCONTEXTFLAGS
{
    CONTEXT_NEEDS_VTX_CACHE_FLUSH = 0x01
} D3DCONTEXTFLAGS;

// Define the context header.
typedef struct _def_nv_d3d_context
{
    // Application DX version.
    DWORD                       dwDXAppVersion;         // the application's DX version level.

    // Default texture state for this context.
    DWORD                       dwTickLastUsed;         // cheesy way of determining the active context
    DWORD                       dwLastFillColor;        // most reset fill color (used for logo clear)
    BOOL                        bClearLogo;             // TRUE if we must clear the area behind the logo

#if (NVARCH >= 0x010)
    AASTATE                     aa;
#endif  // NVARCH >= 0x010
#if (NVARCH >= 0x020)
    CKelvinAAState              kelvinAA;
#endif  // NVARCH >= 0x010

    // palette
    DWORD                       dwTexturePaletteStagingHandle;
    DWORD                       dwTexturePaletteAddr;   // address of palette staging area
    DWORD                       dwTexturePaletteOffset; // offset of palette staging area

    CSimpleSurface             *pRenderTarget;
    DWORD                       dwRTHandle;             // cached for RT restore after mode switch
    CSimpleSurface             *pZetaBuffer;
    DWORD                       dwZBHandle;             // cached for RT restore after mode switch

    // Number of flips to allow pending before using SUPER_TRI code.
    DWORD                       dwSuperTriPendingFlips;
    DWORD                       dwSuperTriThrottle;
    DWORD                       dwSuperTriMask;

    NVD3DCONTEXTDP2PARAM        dp2;
    DWORD                       dwHWUnlockAllPending;
    DWORD                       dwFlags;
    // the following fields of this array have been superceded by equivalent fields in tssState[0].
    //      D3DRENDERSTATE_TEXTUREHANDLE    (1)
    //      D3DRENDERSTATE_TEXTUREADDRESS   (3)
    //      D3DRENDERSTATE_BORDERCOLOR      (43)
    //      D3DRENDERSTATE_TEXTUREADDRESSU  (44)
    //      D3DRENDERSTATE_TEXTUREADDRESSV  (45)
    //      D3DRENDERSTATE_MIPMAPLODBIAS    (46)
    //      D3DRENDERSTATE_ANISOTROPY       (49)
    // values for these fields are no longer saved in dwRenderState and the fields are therefore
    // liable to contain junk! the real values MUST be obtained from tssState[0].
    DWORD                       dwRenderState[D3D_RENDERSTATE_MAX+1];
    BOOL                        bStateChange;
    NVD3DSTATESET               overrides;              // Render state override bits.

    NVFOGDATA                   fogData;

    struct _def_nv_d3d_context *pContextPrev;
    struct _def_nv_d3d_context *pContextNext;

    // DX6 Texture Stage State.
    DWORD                       dwStageCount;           // Number of enabled/valid texture stages.
    BOOL                        bUseDX6Class;           // Use DX6 Multi-Texture Triangle Class.
    BOOL                        bUseTBlendSettings;     //
    DWORD                       dwTBlendCombine0Alpha;
    DWORD                       dwTBlendCombine0Color;
    NVD3DMULTITEXTURESTATE      mtsState;               // DX6 Multi-Texture Triangle State.
    NVD3DMULTITEXTURESTATE      mtsShadowState;         //  for state update optimizations

    NVD3DTEXSTAGESTATE          tssState[8];            // Texture Stage State.

    BOOL                        bStencilEnabled;        // has the stencil buffer ever been enabled?
    DWORD                       dwStencilFill;          // the last value to which the stencil buffer was filled

    DWORD                       pid;                    // Process ID
    DWORD                       dwDDLclID;              // tag for this context's DIRECTDRAW_LCL
    DWORD                       dwDDGblID;              // tag for this context's DIRECTDRAW_GBL
    CDriverContext             *pDriverData;            // this context's pDriverData
#ifdef WINNT
    PDEV                       *ppdev;
#else WINNT
    DISPDRVDIRECTXCOMMON       *pDXShare;               // this context's pDXShare
#endif

    NVLIGHTING                  lighting;               // Lighting state
    nvLight                    *pLightArray;
    DWORD                       dwLightArraySize;       // Size of the light array allocated

    D3DMATERIAL7                Material;               // the current material

    D3DMATRIX                   xfmProj;
    D3DMATRIX                   xfmView;
    D3DMATRIX                   xfmWorld[NV_CAPS_MAX_MATRICES];  // world matrices

    D3DVALUE                    ppClipPlane[D3DMAXUSERCLIPPLANES][4];

    // stateset stuff
    pnvDP2FunctionTable         pDP2FunctionTable;      // current DP2 functions (set, record, or capture)
    DWORD                       dwCurrentStateSet;
    DWORD                       dwMaxStateSetHandles;
    STATESET                   *pStateSets;

    DWORD                       dwFunctionLookup;       // Lookup index for the appropriate nv4 render function.
    NVD3DCONTEXTINNERLOOP       ctxInnerLoop;           // Context data used within inner loops.
    NVD3DCONTEXTINNERLOOP       ctxInnerLoopShadow;     // hw image of above
    CHardwareState              hwState;

    // viewport
    SurfaceViewport             surfaceViewport;

    CVertexShader              *pCurrentVShader;
    VertexShaderConsts         *pVShaderConsts;

    CPixelShader               *pCurrentPShader;
    CPixelShader               *pLastPShader;
    D3DCOLORVALUE               pixelShaderConsts[NV_CAPS_MAX_PSHADER_CONSTS];

    CVertexBuffer              *ppDX8Streams[NV_CAPS_MAX_STREAMS];   // Streams for DX8 prims, note they all use dwVertexOffsetInUse
    DWORD                       dwStreamDMACount;                    // number of unique context DMAs in 15:0, number of system streams in 31:16

    CVertexBuffer              *pIndexBuffer;                        // Index buffer for DX8 primitives

    CNvObject                  *pCelsiusPointTexture;
    DWORD                       dwPointHandle;

    NVD3DCONTEXTSYSVB           sysvb;

    PBYTE                       pVertexDataUM;
    DWORD                       dwVertexDataLengthUM;

#ifdef WINNT
    // Need this for Winnt to access the PDEV state struct maintained by display
    // driver. PDEV also contains a ptr to the GLOBALDATA struct.
    PDD_DIRECTDRAW_GLOBAL       lpDD;
#endif // #ifdef WINNT

#ifdef TEX_MANAGE
    NV_TEXMAN_DATA              texManData;  // data used by the texture manager
#endif  // TEX_MANAGE

    DWORD                       dwEarlyCopyStrategy; // ask ScottC for details
    DWORD                       dwClearCount;        // number of full clears this frame
    DWORD                       dwTotalClearCount;   // total number of clears on the primary Z surface, modulo 256
    BOOL                        bSeenTriangles;      // Have we seen any triangle calls
    BOOL                        bScaledFlatPanel;   // are we in a scaled, flat panel mode?

} NVD3DCONTEXT, *PNVD3DCONTEXT;  // struct _def_nv_d3d_context

} // extern C++

// NV_OBJECT_LIST is a list of pointers to the NV objects
// associated with a particular DIRECTDRAW_LCL. the list is indexed by the
// handles that have been associated with the objects via CreateSurfaceEx.
// we maintain a linked list of these structures for each DIRECTDRAW_GBL.

#define NV_OBJECT_LIST_SIZE_DEFAULT      128
#define NV_OBJECT_LIST_SIZE_INCREMENTAL  128

typedef struct _NV_OBJECT_LIST {
    DWORD                     dwDDLclID;     // ID tag for the DD_LCL for which this list is kept
    DWORD                     dwListSize;    // the size of the list
    CNvObject               **ppObjectList;  // the list itself (just an array)
    struct _NV_OBJECT_LIST   *pNext;         // the next list
} NV_OBJECT_LIST, *PNV_OBJECT_LIST;

/*
 * External data.
 */
extern PFNRENDERPRIMITIVEOP       pfnRenderPrimitiveOperation[];
extern PFND3DPARSEUNKNOWNCOMMAND  fnD3DParseUnknownCommandCallback;

/*
 * Texture Staging Manager Functions
 */
#define TM_STAGESPACE              (2*1024*1024) // 2048k

#define NV_PN_TEX_RETIRE           0    // depreciated in for nv_tex2
#define NV_PN_SPOOFED_FLIP_COUNT   4
#define NV_PN_TEXTURE_RETIRE       8

/*
 * Embedded Profiler Functions
 */
#ifdef NV_PROFILE

/*
 * Constants
 */
 #define NVP_E_BEGINSCENE            0 // beginscene event
 #define NVP_E_PUT                   1 // put event
 #define NVP_T_HDFLUSH               2 // hd flush time
 #define NVP_T_SCENE                 3 // scene time
 #define NVP_T_DP2                   4 // total dp2 time

#ifdef NV_PROFILE_CPULOCK
 #define NVP_T_VBDP2_LOCK            5 // dp2 exit cpu lock
 #define NVP_T_VB_LOCK               6 // D3DLockExecuteBuffer
 #define NVP_T_VB_INLINE             7 // inline VB lock
 #define NVP_T_TEX_ULS               8 // update linear surface
 #define NVP_T_TEX_USS               9 // update swizzle surface
 #define NVP_T_TEX_BLT              10 // nvTextureBlt
 #define NVP_T_TEX_LOCK             11 // nvTextureLock
 #define NVP_T_FLOAT0               12
 #define NVP_T_FLOAT1               13
#endif

#ifdef NV_PROFILE_PUSHER
 #define NVP_T_WRAPAROUND            5
 #define NVP_T_MAKESPACE             6
 #define NVP_T_ADJUST                7
 #define NVP_T_FLUSH                 8 // time spend waiting in nvPusherFlush
 #define NVP_C_PENDINGSIZE           9 // size of pending pushe buffer (at put time)
 #define NVP_C_PUT                  10
 #define NVP_C_GET                  11
 #define NVP_C_VELOCITY             12
#endif

#ifdef NV_PROFILE_CALLSTACK
 #define NVP_T_SETCELSIUSSTATE       5
 #define NVP_T_SETKELVINSTATE        6
 #define NVP_T_IDXDISPATCH           7
 #define NVP_T_ORDDISPATCH           8
#endif

#ifdef NV_PROFILE_DEFVB
 #define NVP_C_PUT                   5
 #define NVP_C_GET                   6
 #define NVP_C_FREECOUNT             7
 #define NVP_E_PRIM                  8
#endif

#ifdef NV_PROFILE_COPIES
 #define NVP_T_RENAME_AUTO_COPY      5
 #define NVP_T_LIN2SWZ2              6
 #define NVP_T_RENAME_FWD            7
#endif

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

#endif // !NV_PROFILE

// nv4comp.h
typedef void (__stdcall *PFNDRAWPRIMITIVE)(DWORD dwPrimCount,LPWORD pIndices,DWORD dwStrides,LPBYTE pVertices);

// Check the return value and return if something wrong.
// Assume hr has been declared
#define HR_RET(exp)       \
{                         \
    hr = (exp);           \
    if (hr != D3D_OK)     \
    {                     \
        return hr;        \
    }                     \
}

// FVF related macros
#define FVF_TRANSFORMED(dwFVF)          ((dwFVF & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW)
#define FVF_TEXCOORD_NUMBER(dwFVF)      (((dwFVF) & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT)
#define NV_VERTEX_TRANSFORMED(pVShader) (((pVShader->hasProgram() == FALSE) && (pVShader->getVASize(defaultInputRegMap[D3DVSDE_POSITION]) == 4*sizeof(DWORD))) ? TRUE : FALSE)

// page constants
#ifndef PAGE_MASK
#define PAGE_MASK   0xFFFFF000
#endif
#ifndef PAGE_SIZE
#define PAGE_SIZE   0x00001000
#endif

#endif  // _NVDINC_H_
