// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
// Module: nvKelvinCaps.h
//      Define the capabilities of the kelvin D3D driver.
//
// **************************************************************************
//
// History:
//      Craig Duttweiler    bertrem     26Apr2000     created
//
// **************************************************************************

#ifndef _NVKELVINCAPS_H_
#define _NVKELVINCAPS_H_

#if (NVARCH >= 0x020)

// --------------------------------------------------------------------------
//  Direct 3D HAL Capabilities
// --------------------------------------------------------------------------

// Define which fields of the D3DDEVICECAPS structure are valid.
#define KELVIN_DEVDESC_FLAGS            D3DDD_COLORMODEL                    |   \
                                        D3DDD_DEVCAPS                       |   \
                                        D3DDD_LINECAPS                      |   \
                                        D3DDD_TRICAPS                       |   \
                                        D3DDD_DEVICERENDERBITDEPTH          |   \
                                        D3DDD_DEVICEZBUFFERBITDEPTH

// Define the color model for the D3D driver.
#define KELVIN_COLORMODEL               D3DCOLOR_RGB

// Define the device capabilities.
#define KELVIN_DEVCAPS_FLAGS            D3DDEVCAPS_FLOATTLVERTEX            |   \
                                        D3DDEVCAPS_EXECUTESYSTEMMEMORY      |   \
                                        D3DDEVCAPS_EXECUTEVIDEOMEMORY       |   \
                                        D3DDEVCAPS_TLVERTEXSYSTEMMEMORY     |   \
                                        D3DDEVCAPS_TLVERTEXVIDEOMEMORY      |   \
                                        D3DDEVCAPS_TEXTUREVIDEOMEMORY       |   \
                                        D3DDEVCAPS_DRAWPRIMTLVERTEX         |   \
                                        D3DDEVCAPS_CANRENDERAFTERFLIP       |   \
                                        D3DDEVCAPS_TEXTURENONLOCALVIDMEM    |   \
                                        D3DDEVCAPS_DRAWPRIMITIVES2          |   \
                                        D3DDEVCAPS_DRAWPRIMITIVES2EX        |   \
                                        D3DDEVCAPS_HWTRANSFORMANDLIGHT      |   \
                                        D3DDEVCAPS_CANBLTSYSTONONLOCAL      |   \
                                        D3DDEVCAPS_PUREDEVICE               |   \
                                        D3DDEVCAPS_QUINTICRTPATCHES         |   \
                                        D3DDEVCAPS_RTPATCHES                |   \
                                        D3DDEVCAPS_HWVERTEXBUFFER           |   \
                                        D3DDEVCAPS_HWINDEXBUFFER            |   \
                                        D3DDEVCAPS_HWRASTERIZATION

// Define the transformation capabilities.
#define KELVIN_TRANSFORMCAPS_FLAGS      D3DTRANSFORMCAPS_CLIP

// Define 3D clipping capabilities.
#define KELVIN_3DCLIPPINGCAPS           TRUE

// Define the lighting capabilities.
#define KELVIN_LIGHTINGCAPS_FLAGS       D3DLIGHTINGMODEL_RGB                |   \
                                        D3DLIGHTCAPS_POINT                  |   \
                                        D3DLIGHTCAPS_SPOT                   |   \
                                        D3DLIGHTCAPS_DIRECTIONAL
#define KELVIN_MAX_LIGHTS               8

// Define NULL primary capabilities.
#define KELVIN_PRIMCAPS_NULL            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

// Define the line capabilities.
//
// Miscellaneous capabilities (dwMiscCaps)
#define KELVIN_LINECAPS_MISC            D3DPMISCCAPS_MASKZ                  |   \
                                        D3DPMISCCAPS_CULLNONE               |   \
                                        D3DPMISCCAPS_CULLCW                 |   \
                                        D3DPMISCCAPS_CULLCCW                |   \
                                        D3DPMISCCAPS_BLENDOP                |   \
                                        D3DPMISCCAPS_COLORWRITEENABLE       |   \
                                        D3DPMISCCAPS_TSSARGTEMP

// Raster capabilities (dwRasterCaps)
#define KELVIN_LINECAPS_RASTER          D3DPRASTERCAPS_DITHER                   |   \
                                        D3DPRASTERCAPS_SUBPIXEL                 |   \
                                        D3DPRASTERCAPS_FOGVERTEX                |   \
                                        D3DPRASTERCAPS_FOGTABLE                 |   \
                                        D3DPRASTERCAPS_MIPMAPLODBIAS            |   \
                                        D3DPRASTERCAPS_ZBIAS                    |   \
                                        D3DPRASTERCAPS_FOGRANGE                 |   \
                                        D3DPRASTERCAPS_WBUFFER                  |   \
                                        D3DPRASTERCAPS_WFOG                     |   \
                                        D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT |   \
                                        D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE    |   \
                                        D3DPRASTERCAPS_ANISOTROPY               |   \
                                        D3DPRASTERCAPS_ZFOG

// Z Compare capabilities (dwZCmpCaps)
#define KELVIN_LINECAPS_ZCMP            D3DPCMPCAPS_NEVER                   |   \
                                        D3DPCMPCAPS_LESS                    |   \
                                        D3DPCMPCAPS_EQUAL                   |   \
                                        D3DPCMPCAPS_LESSEQUAL               |   \
                                        D3DPCMPCAPS_GREATER                 |   \
                                        D3DPCMPCAPS_NOTEQUAL                |   \
                                        D3DPCMPCAPS_GREATEREQUAL            |   \
                                        D3DPCMPCAPS_ALWAYS

// Source blend capabilities (dwSrcBlendCaps)
#define KELVIN_LINECAPS_SRCBLEND        D3DPBLENDCAPS_ZERO                  |   \
                                        D3DPBLENDCAPS_ONE                   |   \
                                        D3DPBLENDCAPS_SRCCOLOR              |   \
                                        D3DPBLENDCAPS_INVSRCCOLOR           |   \
                                        D3DPBLENDCAPS_SRCALPHA              |   \
                                        D3DPBLENDCAPS_INVSRCALPHA           |   \
                                        D3DPBLENDCAPS_DESTALPHA             |   \
                                        D3DPBLENDCAPS_INVDESTALPHA          |   \
                                        D3DPBLENDCAPS_DESTCOLOR             |   \
                                        D3DPBLENDCAPS_INVDESTCOLOR          |   \
                                        D3DPBLENDCAPS_SRCALPHASAT           |   \
                                        D3DPBLENDCAPS_BOTHSRCALPHA          |   \
                                        D3DPBLENDCAPS_BOTHINVSRCALPHA

// Destination blend capabilities (dwDestBlendCaps)
#define KELVIN_LINECAPS_DESTBLEND       D3DPBLENDCAPS_ZERO                  |   \
                                        D3DPBLENDCAPS_ONE                   |   \
                                        D3DPBLENDCAPS_SRCCOLOR              |   \
                                        D3DPBLENDCAPS_INVSRCCOLOR           |   \
                                        D3DPBLENDCAPS_SRCALPHA              |   \
                                        D3DPBLENDCAPS_INVSRCALPHA           |   \
                                        D3DPBLENDCAPS_DESTALPHA             |   \
                                        D3DPBLENDCAPS_INVDESTALPHA          |   \
                                        D3DPBLENDCAPS_DESTCOLOR             |   \
                                        D3DPBLENDCAPS_INVDESTCOLOR          |   \
                                        D3DPBLENDCAPS_SRCALPHASAT           |   \
                                        D3DPBLENDCAPS_BOTHSRCALPHA          |   \
                                        D3DPBLENDCAPS_BOTHINVSRCALPHA

// Alpha compare capabilities (dwAlphaCmpCaps)
#define KELVIN_LINECAPS_ALPHACMP        D3DPCMPCAPS_NEVER                   |   \
                                        D3DPCMPCAPS_LESS                    |   \
                                        D3DPCMPCAPS_EQUAL                   |   \
                                        D3DPCMPCAPS_LESSEQUAL               |   \
                                        D3DPCMPCAPS_GREATER                 |   \
                                        D3DPCMPCAPS_NOTEQUAL                |   \
                                        D3DPCMPCAPS_GREATEREQUAL            |   \
                                        D3DPCMPCAPS_ALWAYS

// Shading capabilities (dwShadeCaps)
#define KELVIN_LINECAPS_SHADE           D3DPSHADECAPS_COLORFLATMONO         |   \
                                        D3DPSHADECAPS_COLORFLATRGB          |   \
                                        D3DPSHADECAPS_COLORGOURAUDMONO      |   \
                                        D3DPSHADECAPS_COLORGOURAUDRGB       |   \
                                        D3DPSHADECAPS_SPECULARFLATRGB       |   \
                                        D3DPSHADECAPS_SPECULARGOURAUDRGB    |   \
                                        D3DPSHADECAPS_ALPHAFLATBLEND        |   \
                                        D3DPSHADECAPS_ALPHAGOURAUDBLEND     |   \
                                        D3DPSHADECAPS_FOGFLAT               |   \
                                        D3DPSHADECAPS_FOGGOURAUD

// Texture capabilities (dwTextureCaps)
#define KELVIN_LINECAPS_TEXTURE         D3DPTEXTURECAPS_PERSPECTIVE         |   \
                                        D3DPTEXTURECAPS_POW2                |   \
                                        D3DPTEXTURECAPS_ALPHA               |   \
                                        D3DPTEXTURECAPS_TRANSPARENCY        |   \
                                        D3DPTEXTURECAPS_CUBEMAP             |   \
                                        D3DPTEXTURECAPS_CUBEMAP_POW2        |   \
                                        D3DPTEXTURECAPS_MIPMAP              |   \
                                        D3DPTEXTURECAPS_MIPCUBEMAP          |   \
                                        D3DPTEXTURECAPS_PROJECTED           |   \
                                        D3DPTEXTURECAPS_ALPHAPALETTE        |   \
                                        D3DPTEXTURECAPS_VOLUMEMAP           |   \
                                        D3DPTEXTURECAPS_VOLUMEMAP_POW2      |   \
                                        D3DPTEXTURECAPS_MIPVOLUMEMAP

// Texture filtering capabilities (dwTextureFilterCaps)
#define KELVIN_LINECAPS_TEXTUREFILTER   D3DPTFILTERCAPS_NEAREST             |   \
                                        D3DPTFILTERCAPS_LINEAR              |   \
                                        D3DPTFILTERCAPS_MIPNEAREST          |   \
                                        D3DPTFILTERCAPS_MIPLINEAR           |   \
                                        D3DPTFILTERCAPS_LINEARMIPNEAREST    |   \
                                        D3DPTFILTERCAPS_LINEARMIPLINEAR     |   \
                                        D3DPTFILTERCAPS_MINFPOINT           |   \
                                        D3DPTFILTERCAPS_MINFLINEAR          |   \
                                        D3DPTFILTERCAPS_MIPFPOINT           |   \
                                        D3DPTFILTERCAPS_MIPFLINEAR          |   \
                                        D3DPTFILTERCAPS_MAGFPOINT           |   \
                                        D3DPTFILTERCAPS_MAGFLINEAR

// Texture blending capabilities (dwTextureBlendCaps)
#define KELVIN_LINECAPS_TEXTUREBLEND    D3DPTBLENDCAPS_DECAL                |   \
                                        D3DPTBLENDCAPS_MODULATE             |   \
                                        D3DPTBLENDCAPS_DECALALPHA           |   \
                                        D3DPTBLENDCAPS_MODULATEALPHA        |   \
                                        D3DPTBLENDCAPS_DECALMASK            |   \
                                        D3DPTBLENDCAPS_MODULATEMASK         |   \
                                        D3DPTBLENDCAPS_COPY                 |   \
                                        D3DPTBLENDCAPS_ADD

// Texture addressing capabilities (dwTextureAddressCaps)
#define KELVIN_LINECAPS_TEXTUREADDRESS  D3DPTADDRESSCAPS_WRAP               |   \
                                        D3DPTADDRESSCAPS_MIRROR             |   \
                                        D3DPTADDRESSCAPS_CLAMP              |   \
                                        D3DPTADDRESSCAPS_BORDER             |   \
                                        D3DPTADDRESSCAPS_INDEPENDENTUV

// Maximum stipple dimensions.
#define KELVIN_LINECAPS_STIPPLEWIDTH    0
#define KELVIN_LINECAPS_STIPPLEHEIGHT   0

// Define the triangle capabilities.
//
// Miscellaneous capabilities (dwMiscCaps)
#define KELVIN_TRICAPS_MISC             D3DPMISCCAPS_MASKZ                  |   \
                                        D3DPMISCCAPS_CULLNONE               |   \
                                        D3DPMISCCAPS_CULLCW                 |   \
                                        D3DPMISCCAPS_CULLCCW                |   \
                                        D3DPMISCCAPS_BLENDOP                |   \
                                        D3DPMISCCAPS_COLORWRITEENABLE       |   \
                                        D3DPMISCCAPS_TSSARGTEMP

// Raster capabilities (dwRasterCaps)
#define KELVIN_TRICAPS_RASTER           D3DPRASTERCAPS_DITHER                   |   \
                                        D3DPRASTERCAPS_SUBPIXEL                 |   \
                                        D3DPRASTERCAPS_FOGVERTEX                |   \
                                        D3DPRASTERCAPS_FOGTABLE                 |   \
                                        D3DPRASTERCAPS_MIPMAPLODBIAS            |   \
                                        D3DPRASTERCAPS_ZBIAS                    |   \
                                        D3DPRASTERCAPS_FOGRANGE                 |   \
                                        D3DPRASTERCAPS_WBUFFER                  |   \
                                        D3DPRASTERCAPS_WFOG                     |   \
                                        D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT |   \
                                        D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE    |   \
                                        D3DPRASTERCAPS_ANISOTROPY               |   \
                                        D3DPRASTERCAPS_ZFOG

// Z Compare capabilities (dwZCmpCaps)
#define KELVIN_TRICAPS_ZCMP             D3DPCMPCAPS_NEVER                   |   \
                                        D3DPCMPCAPS_LESS                    |   \
                                        D3DPCMPCAPS_EQUAL                   |   \
                                        D3DPCMPCAPS_LESSEQUAL               |   \
                                        D3DPCMPCAPS_GREATER                 |   \
                                        D3DPCMPCAPS_NOTEQUAL                |   \
                                        D3DPCMPCAPS_GREATEREQUAL            |   \
                                        D3DPCMPCAPS_ALWAYS

// Source blend capabilities (dwSrcBlendCaps)
#define KELVIN_TRICAPS_SRCBLEND         D3DPBLENDCAPS_ZERO                  |   \
                                        D3DPBLENDCAPS_ONE                   |   \
                                        D3DPBLENDCAPS_SRCCOLOR              |   \
                                        D3DPBLENDCAPS_INVSRCCOLOR           |   \
                                        D3DPBLENDCAPS_SRCALPHA              |   \
                                        D3DPBLENDCAPS_INVSRCALPHA           |   \
                                        D3DPBLENDCAPS_DESTALPHA             |   \
                                        D3DPBLENDCAPS_INVDESTALPHA          |   \
                                        D3DPBLENDCAPS_DESTCOLOR             |   \
                                        D3DPBLENDCAPS_INVDESTCOLOR          |   \
                                        D3DPBLENDCAPS_SRCALPHASAT           |   \
                                        D3DPBLENDCAPS_BOTHSRCALPHA          |   \
                                        D3DPBLENDCAPS_BOTHINVSRCALPHA

// Destination blend capabilities (dwDestBlendCaps)
#define KELVIN_TRICAPS_DESTBLEND        D3DPBLENDCAPS_ZERO                  |   \
                                        D3DPBLENDCAPS_ONE                   |   \
                                        D3DPBLENDCAPS_SRCCOLOR              |   \
                                        D3DPBLENDCAPS_INVSRCCOLOR           |   \
                                        D3DPBLENDCAPS_SRCALPHA              |   \
                                        D3DPBLENDCAPS_INVSRCALPHA           |   \
                                        D3DPBLENDCAPS_DESTALPHA             |   \
                                        D3DPBLENDCAPS_INVDESTALPHA          |   \
                                        D3DPBLENDCAPS_DESTCOLOR             |   \
                                        D3DPBLENDCAPS_INVDESTCOLOR          |   \
                                        D3DPBLENDCAPS_SRCALPHASAT           |   \
                                        D3DPBLENDCAPS_BOTHSRCALPHA          |   \
                                        D3DPBLENDCAPS_BOTHINVSRCALPHA

// Alpha compare capabilities (dwAlphaCmpCaps)
#define KELVIN_TRICAPS_ALPHACMP         D3DPCMPCAPS_NEVER                   |   \
                                        D3DPCMPCAPS_LESS                    |   \
                                        D3DPCMPCAPS_EQUAL                   |   \
                                        D3DPCMPCAPS_LESSEQUAL               |   \
                                        D3DPCMPCAPS_GREATER                 |   \
                                        D3DPCMPCAPS_NOTEQUAL                |   \
                                        D3DPCMPCAPS_GREATEREQUAL            |   \
                                        D3DPCMPCAPS_ALWAYS

// Shading capabilities (dwShadeCaps)
#define KELVIN_TRICAPS_SHADE            D3DPSHADECAPS_COLORFLATMONO         |   \
                                        D3DPSHADECAPS_COLORFLATRGB          |   \
                                        D3DPSHADECAPS_COLORGOURAUDMONO      |   \
                                        D3DPSHADECAPS_COLORGOURAUDRGB       |   \
                                        D3DPSHADECAPS_SPECULARFLATRGB       |   \
                                        D3DPSHADECAPS_SPECULARGOURAUDRGB    |   \
                                        D3DPSHADECAPS_ALPHAFLATBLEND        |   \
                                        D3DPSHADECAPS_ALPHAGOURAUDBLEND     |   \
                                        D3DPSHADECAPS_FOGFLAT               |   \
                                        D3DPSHADECAPS_FOGGOURAUD

// Texture capabilities (dwTextureCaps)
#define KELVIN_TRICAPS_TEXTURE          D3DPTEXTURECAPS_PERSPECTIVE         |   \
                                        D3DPTEXTURECAPS_POW2                |   \
                                        D3DPTEXTURECAPS_ALPHA               |   \
                                        D3DPTEXTURECAPS_TRANSPARENCY        |   \
                                        D3DPTEXTURECAPS_CUBEMAP             |   \
                                        D3DPTEXTURECAPS_CUBEMAP_POW2        |   \
                                        D3DPTEXTURECAPS_MIPMAP              |   \
                                        D3DPTEXTURECAPS_MIPCUBEMAP          |   \
                                        D3DPTEXTURECAPS_PROJECTED           |   \
                                        D3DPTEXTURECAPS_ALPHAPALETTE        |   \
                                        D3DPTEXTURECAPS_VOLUMEMAP           |   \
                                        D3DPTEXTURECAPS_VOLUMEMAP_POW2      |   \
                                        D3DPTEXTURECAPS_MIPVOLUMEMAP


// Texture filtering capabilities (dwTextureFilterCaps)
#define KELVIN_TRICAPS_TEXTUREFILTER    D3DPTFILTERCAPS_NEAREST             |   \
                                        D3DPTFILTERCAPS_LINEAR              |   \
                                        D3DPTFILTERCAPS_MIPNEAREST          |   \
                                        D3DPTFILTERCAPS_MIPLINEAR           |   \
                                        D3DPTFILTERCAPS_LINEARMIPNEAREST    |   \
                                        D3DPTFILTERCAPS_LINEARMIPLINEAR     |   \
                                        D3DPTFILTERCAPS_MINFPOINT           |   \
                                        D3DPTFILTERCAPS_MINFLINEAR          |   \
                                        D3DPTFILTERCAPS_MINFANISOTROPIC     |   \
                                        D3DPTFILTERCAPS_MIPFPOINT           |   \
                                        D3DPTFILTERCAPS_MIPFLINEAR          |   \
                                        D3DPTFILTERCAPS_MAGFPOINT           |   \
                                        D3DPTFILTERCAPS_MAGFLINEAR          |   \
                                        D3DPTFILTERCAPS_MAGFANISOTROPIC

// Volume texture filtering capabilities
#define KELVIN_TRICAPS_VOLUMETEXFILTER  D3DPTFILTERCAPS_NEAREST             |   \
                                        D3DPTFILTERCAPS_LINEAR              |   \
                                        D3DPTFILTERCAPS_MIPNEAREST          |   \
                                        D3DPTFILTERCAPS_MIPLINEAR           |   \
                                        D3DPTFILTERCAPS_LINEARMIPNEAREST    |   \
                                        D3DPTFILTERCAPS_LINEARMIPLINEAR     |   \
                                        D3DPTFILTERCAPS_MINFPOINT           |   \
                                        D3DPTFILTERCAPS_MINFLINEAR          |   \
                                        D3DPTFILTERCAPS_MINFANISOTROPIC     |   \
                                        D3DPTFILTERCAPS_MIPFPOINT           |   \
                                        D3DPTFILTERCAPS_MIPFLINEAR          |   \
                                        D3DPTFILTERCAPS_MAGFPOINT


// Texture blending capabilities (dwTextureBlendCaps)
#define KELVIN_TRICAPS_TEXTUREBLEND     D3DPTBLENDCAPS_DECAL                |   \
                                        D3DPTBLENDCAPS_MODULATE             |   \
                                        D3DPTBLENDCAPS_DECALALPHA           |   \
                                        D3DPTBLENDCAPS_MODULATEALPHA        |   \
                                        D3DPTBLENDCAPS_DECALMASK            |   \
                                        D3DPTBLENDCAPS_MODULATEMASK         |   \
                                        D3DPTBLENDCAPS_COPY                 |   \
                                        D3DPTBLENDCAPS_ADD

// Texture addressing capabilities (dwTextureAddressCaps)
#define KELVIN_TRICAPS_TEXTUREADDRESS   D3DPTADDRESSCAPS_WRAP               |   \
                                        D3DPTADDRESSCAPS_MIRROR             |   \
                                        D3DPTADDRESSCAPS_CLAMP              |   \
                                        D3DPTADDRESSCAPS_BORDER             |   \
                                        D3DPTADDRESSCAPS_INDEPENDENTUV

// Maximum stipple dimensions.
#define KELVIN_TRICAPS_STIPPLEWIDTH     0
#define KELVIN_TRICAPS_STIPPLEHEIGHT    0

// Texture Ops
#define KELVIN_CAPS_TEXTUREOPS   D3DTEXOPCAPS_DISABLE                       |   \
                                 D3DTEXOPCAPS_SELECTARG1                    |   \
                                 D3DTEXOPCAPS_SELECTARG2                    |   \
                                 D3DTEXOPCAPS_MODULATE                      |   \
                                 D3DTEXOPCAPS_MODULATE2X                    |   \
                                 D3DTEXOPCAPS_MODULATE4X                    |   \
                                 D3DTEXOPCAPS_ADD                           |   \
                                 D3DTEXOPCAPS_ADDSIGNED                     |   \
                                 D3DTEXOPCAPS_ADDSIGNED2X                   |   \
                                 D3DTEXOPCAPS_SUBTRACT                      |   \
                                 D3DTEXOPCAPS_ADDSMOOTH                     |   \
                                 D3DTEXOPCAPS_BLENDDIFFUSEALPHA             |   \
                                 D3DTEXOPCAPS_BLENDTEXTUREALPHA             |   \
                                 D3DTEXOPCAPS_BLENDFACTORALPHA              |   \
                                 D3DTEXOPCAPS_BLENDTEXTUREALPHAPM           |   \
                                 D3DTEXOPCAPS_BLENDCURRENTALPHA             |   \
                                 D3DTEXOPCAPS_PREMODULATE                   |   \
                                 D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR        |   \
                                 D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA        |   \
                                 D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR     |   \
                                 D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA     |   \
                                 D3DTEXOPCAPS_BUMPENVMAP                    |   \
                                 D3DTEXOPCAPS_BUMPENVMAPLUMINANCE           |   \
                                 D3DTEXOPCAPS_DOTPRODUCT3                   |   \
                                 D3DTEXOPCAPS_MULTIPLYADD                   |   \
                                 D3DTEXOPCAPS_LERP

// Stencil Ops
#define KELVIN_CAPS_STENCILOPS   D3DSTENCILCAPS_KEEP                        |   \
                                 D3DSTENCILCAPS_ZERO                        |   \
                                 D3DSTENCILCAPS_REPLACE                     |   \
                                 D3DSTENCILCAPS_INCRSAT                     |   \
                                 D3DSTENCILCAPS_DECRSAT                     |   \
                                 D3DSTENCILCAPS_INVERT                      |   \
                                 D3DSTENCILCAPS_INCR                        |   \
                                 D3DSTENCILCAPS_DECR

// Vertex Processing Caps
#define KELVIN_CAPS_VTXPCAPS   D3DVTXPCAPS_TEXGEN                           |   \
                               D3DVTXPCAPS_MATERIALSOURCE7                  |   \
                               D3DVTXPCAPS_VERTEXFOG                        |   \
                               D3DVTXPCAPS_DIRECTIONALLIGHTS                |   \
                               D3DVTXPCAPS_POSITIONALLIGHTS                 |   \
                               D3DVTXPCAPS_LOCALVIEWER                      |   \
                               D3DVTXPCAPS_NO_VSDT_UBYTE4 // NV20 does not support this, NV25 does

// Define the render bit depth capabilites.
#define KELVIN_RENDERBITDEPTHCAPS      (DDBD_16 | DDBD_32)

// Define the Z buffer bit depth capabilities.
#define KELVIN_ZBUFFERBITDEPTHCAPS     (DDBD_16 | DDBD_24)

#endif  // NVARCH > 0x020
#endif  // _NVKELVINCAPS_H_

