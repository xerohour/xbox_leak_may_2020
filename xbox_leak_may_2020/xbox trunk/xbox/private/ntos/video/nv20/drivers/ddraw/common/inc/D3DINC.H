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

#ifndef _D3DINC_H_
#define _D3DINC_H_

#ifdef __cplusplus
extern "C" {
#endif

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
#define AGPLIST_ARRAY_SIZE          256
#define AGPLIST_ARRAY_DELTA         256
#define NVD3D_DEFAULT_TL_NUM        0           // was: ((32 * 2048) / sizeof(D3DTLVERTEX))

#define MAX_STATE                   D3DSTATE_OVERRIDE_BIAS
#define DWORD_BITS                  32
#define DWORD_SHIFT                 5

// point and line sizes
#define LINE_WIDTH                  1.0f
#define HALF_LINE_WIDTH             (0.5f*LINE_WIDTH)
#define POINT_SIZE                  1.0f
#define HALF_POINT_SIZE             (0.5f*POINT_SIZE)

#define NV4_VERTEX_CACHE_SIZE       16

// define global values to be the greatest of those required by the various architectures
#if (NVARCH >= 0x020)
#define NV_CAPS_MAX_COMBINERS       KELVIN_NUM_COMBINERS
#define NV_CAPS_MAX_TEXTURES        KELVIN_NUM_TEXTURES
#define NV_CAPS_MAX_MATRICES        KELVIN_NUM_MATRICES
#define NV_CAPS_MAX_UV_SETS         KELVIN_CAPS_MAX_UV_PAIRS
#define NV_CAPS_MAX_CLIPPLANES      KELVIN_CAPS_MAX_USER_CLIP_PLANES_ALLOWED
#define NV_CAPS_MAX_STREAMS         KELVIN_CAPS_MAX_STREAMS
#define NV_CAPS_MAX_PSHADER_CONSTS  KELVIN_CAPS_MAX_PSHADER_CONSTS
#define NV_CAPS_MAX_VSHADER_TEMPS   KELVIN_CAPS_MAX_VSHADER_TEMPS
#define NV_CAPS_MAX_VSHADER_CONSTS  KELVIN_CAPS_MAX_VSHADER_CONSTS
#elif (NVARCH >= 0x010)
#define NV_CAPS_MAX_COMBINERS       CELSIUS_NUM_COMBINERS
#define NV_CAPS_MAX_TEXTURES        CELSIUS_NUM_TEXTURES
#define NV_CAPS_MAX_MATRICES        CELSIUS_CAPS_MAX_VERTEX_BLEND_MATRICES
#define NV_CAPS_MAX_UV_SETS         CELSIUS_CAPS_MAX_UV_PAIRS
#define NV_CAPS_MAX_CLIPPLANES      CELSIUS_CAPS_MAX_USER_CLIP_PLANES
#define NV_CAPS_MAX_STREAMS         CELSIUS_CAPS_MAX_STREAMS
#define NV_CAPS_MAX_PSHADER_CONSTS  CELSIUS_CAPS_MAX_PSHADER_CONSTS
#define NV_CAPS_MAX_VSHADER_TEMPS   CELSIUS_CAPS_MAX_VSHADER_TEMPS
#define NV_CAPS_MAX_VSHADER_CONSTS  CELSIUS_CAPS_MAX_VSHADER_CONSTS
#else  // nv4
#define NV_CAPS_MAX_TEXTURES        2
#define NV_CAPS_MAX_STREAMS         1
#define NV_CAPS_MAX_MATRICES        NV4_CAPS_MAX_VERTEX_BLEND_MATRICES
#define NV_CAPS_MAX_UV_SETS         NV4_CAPS_MAX_UV_PAIRS
#define NV_CAPS_MAX_CLIPPLANES      NV4_CAPS_MAX_USER_CLIP_PLANES
#define NV_CAPS_MAX_PSHADER_CONSTS  0
#define NV_CAPS_MAX_VSHADER_TEMPS   0
#define NV_CAPS_MAX_VSHADER_CONSTS  0
#endif

// helper functions to cope with vertex shader programs
#define GET_LIGHTING_STATE(pContext)    \
    (pContext->pCurrentVShader->hasProgram() ? FALSE : (pContext->dwRenderState[D3DRENDERSTATE_LIGHTING]))
#define GET_VERTEXBLEND_STATE(pContext) \
    (pContext->pCurrentVShader->hasProgram() ? D3DVBLEND_DISABLE : (pContext->dwRenderState[D3DRENDERSTATE_VERTEXBLEND]))

/*
 * --------------------------------------------------------------------------
 *  Data structures.
 * --------------------------------------------------------------------------
 */

/*
 * D3D state set data.
 */
typedef struct _nv_d3d_state_set
{
    DWORD bits[MAX_STATE >> DWORD_SHIFT];
} NVD3DSTATESET;

// DP2 stateset stuff
#define DEFAULT_STATESET_SIZE     4096
#define INCREMENTAL_STATESET_SIZE 2048

typedef struct _STATESET {
    int    allocatedSize;
    int    dataSize;
    LPBYTE pData;
} STATESET;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _D3DINC_H_

