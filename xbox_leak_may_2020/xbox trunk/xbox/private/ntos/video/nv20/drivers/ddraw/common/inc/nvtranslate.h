/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVTRANSLATE.H                                                     *
*   declatations of constants and prototypes for tables needed to           *
*   translate from generic definitions (either NV or D3D) to HW-specific    *
*   definitions                                                             *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler    bertrem     03May99     created                 *
*                                                                           *
\***************************************************************************/

#ifndef _NVTRANSLATE_H_
#define _NVTRANSLATE_H_

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------------
// surface formats
//--------------------------------------------------------------------------

// generic names
// note these are all named by enumerating components from MSB to LSB.
// please keep it this way (despite what people in redmond do)!
#define NV_SURFACE_FORMAT_Y8                            0
#define NV_SURFACE_FORMAT_AY8                           1
#define NV_SURFACE_FORMAT_A1R5G5B5                      2
#define NV_SURFACE_FORMAT_X1R5G5B5                      3
#define NV_SURFACE_FORMAT_A4R4G4B4                      4
#define NV_SURFACE_FORMAT_R5G6B5                        5
#define NV_SURFACE_FORMAT_A8R8G8B8                      6
#define NV_SURFACE_FORMAT_X1A7R8G8B8                    7
#define NV_SURFACE_FORMAT_X8R8G8B8                      8
#define NV_SURFACE_FORMAT_I8_A1R5G5B5                   9
#define NV_SURFACE_FORMAT_I8_R5G6B5                     10
#define NV_SURFACE_FORMAT_I8_A4R4G4B4                   11
#define NV_SURFACE_FORMAT_I8_A8R8G8B8                   12
#define NV_SURFACE_FORMAT_DXT1_A1R5G5B5                 13
#define NV_SURFACE_FORMAT_DXT23_A8R8G8B8                14
#define NV_SURFACE_FORMAT_DXT45_A8R8G8B8                15
#define NV_SURFACE_FORMAT_IMAGE_A1R5G5B5                16
#define NV_SURFACE_FORMAT_IMAGE_R5G6B5                  17
#define NV_SURFACE_FORMAT_IMAGE_A8R8G8B8                18
#define NV_SURFACE_FORMAT_IMAGE_Y8                      19
#define NV_SURFACE_FORMAT_IMAGE_SY8                     20
#define NV_SURFACE_FORMAT_IMAGE_X7SY9                   21
#define NV_SURFACE_FORMAT_IMAGE_R8B8                    22
#define NV_SURFACE_FORMAT_IMAGE_G8B8                    23
#define NV_SURFACE_FORMAT_IMAGE_SG8SB8                  24
#define NV_SURFACE_FORMAT_Z16                           25
#define NV_SURFACE_FORMAT_Z24S8                         26
#define NV_SURFACE_FORMAT_Z24X8                         27
#define NV_SURFACE_FORMAT_DV8DU8                        28
#define NV_SURFACE_FORMAT_L6DV5DU5                      29
#define NV_SURFACE_FORMAT_X8L8DV8DU8                    30
#define NV_SURFACE_FORMAT_HILO_1_V16U16                 31
#define NV_SURFACE_FORMAT_HILO_HEMI_V16U16              32
#define NV_SURFACE_FORMAT_A8B8G8R8                      33
#define NV_SURFACE_FORMAT_UNKNOWN                       34 //used for sys surfaces we don't control

#define NV_SURFACE_FORMAT_COUNT                         35

// format to BPP conversion
extern const DWORD nvSurfaceFormatToBPP[NV_SURFACE_FORMAT_COUNT];

// generic -> HW surface format conversion tables
extern const DWORD nv052SurfaceFormat[NV_SURFACE_FORMAT_COUNT];
extern const DWORD nv053SurfaceFormat[NV_SURFACE_FORMAT_COUNT];
extern const DWORD nv056SurfaceFormat[NV_SURFACE_FORMAT_COUNT];
extern const DWORD nv062SurfaceFormat[NV_SURFACE_FORMAT_COUNT];
extern const DWORD nv077SurfaceFormat[NV_SURFACE_FORMAT_COUNT];
extern const DWORD nv089SurfaceFormat[NV_SURFACE_FORMAT_COUNT];
extern const DWORD nv097SurfaceFormat[NV_SURFACE_FORMAT_COUNT];

// generic -> HW texture format conversion tables
extern const DWORD nv054TextureFormat[NV_SURFACE_FORMAT_COUNT];
extern const DWORD nv055TextureFormat[NV_SURFACE_FORMAT_COUNT];
extern const DWORD nv056TextureFormat[NV_SURFACE_FORMAT_COUNT];
extern const DWORD nv097TextureFormat[NV_SURFACE_FORMAT_COUNT];

//--------------------------------------------------------------------------
// context DMAs
//--------------------------------------------------------------------------

// generic names
#define NV_CONTEXT_DMA_NONE                             0   // system memory
#define NV_CONTEXT_DMA_AGP_OR_PCI                       1   // pci / agp
#define NV_CONTEXT_DMA_VID                              2   // video

#define NV_CONTEXT_DMA_COUNT                            3

// generic -> HW texture format context DMA conversion tables
extern const DWORD nv054TextureContextDma[NV_CONTEXT_DMA_COUNT];
extern const DWORD nv055TextureContextDma[NV_CONTEXT_DMA_COUNT];
extern const DWORD nv056TextureContextDma[NV_CONTEXT_DMA_COUNT];
extern const DWORD nv097TextureContextDma[NV_CONTEXT_DMA_COUNT];
extern const DWORD nv056PaletteContextDma[NV_CONTEXT_DMA_COUNT];
extern const DWORD nv097PaletteContextDma[NV_CONTEXT_DMA_COUNT];

//--------------------------------------------------------------------------
// D3D conversion stuff
//--------------------------------------------------------------------------

#define D3D_STENCIL_OP_COUNT                            9
#define D3D_CMP_FUNC_COUNT                              9
#define D3D_BLEND_COUNT                                14
#define D3D_BLEND_OP_COUNT                              6
#define D3D_TADDRESS_COUNT                              5
#define D3D_MAGFILTER_COUNT                             6
#define D3D_MINFILTER_COUNT                             4
#define D3D_MIPFILTER_COUNT                             4

#define D3D_INPUTREGMAP_COUNT                           D3DVSDE_NORMAL2+1
#define D3D_OUTPUTREGMAP_COUNT                          16

// D3D -> HW conversion tables
extern const DWORD nv056StencilOp[D3D_STENCIL_OP_COUNT];
extern const DWORD nv097StencilOp[D3D_STENCIL_OP_COUNT];
extern const DWORD nv056StencilFunc[D3D_CMP_FUNC_COUNT];
extern const DWORD nv097StencilFunc[D3D_CMP_FUNC_COUNT];
extern const DWORD nv056AlphaFunc[D3D_CMP_FUNC_COUNT];
extern const DWORD nv097AlphaFunc[D3D_CMP_FUNC_COUNT];
extern const DWORD nv056DepthFunc[D3D_CMP_FUNC_COUNT];
extern const DWORD nv097DepthFunc[D3D_CMP_FUNC_COUNT];
extern const DWORD nv056BlendFunc[D3D_BLEND_COUNT];
extern const DWORD nv097BlendFunc[D3D_BLEND_COUNT];
extern const DWORD nv056BlendOp[D3D_BLEND_OP_COUNT];
extern const DWORD nv097BlendOp[D3D_BLEND_OP_COUNT];
extern const DWORD nv056TextureAddress[D3D_TADDRESS_COUNT];
extern const DWORD nv097TextureAddress[D3D_TADDRESS_COUNT];
extern const DWORD nv056TextureMagFilter[D3D_MAGFILTER_COUNT];
extern const DWORD nv097TextureMagFilter[D3D_MAGFILTER_COUNT];
extern const DWORD nv056TextureMinFilter[D3D_MINFILTER_COUNT][D3D_MIPFILTER_COUNT];
extern const DWORD nv097TextureMinFilter[D3D_MINFILTER_COUNT][D3D_MIPFILTER_COUNT];

extern const DWORD dx8MinFilterMapping[];
extern const DWORD dx8MagFilterMapping[];
extern const DWORD dx8MipFilterMapping[];

extern const DWORD defaultInputRegMap[D3D_INPUTREGMAP_COUNT];
extern const DWORD defaultOutputRegMap[D3D_OUTPUTREGMAP_COUNT];

//--------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // _NVTRANSLATE_H_
