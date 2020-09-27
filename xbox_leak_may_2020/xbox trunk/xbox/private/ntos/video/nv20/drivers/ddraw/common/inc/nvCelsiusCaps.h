#ifndef _NVCELSIUSCAPS_H_
#define _NVCELSIUSCAPS_H_

#if (NVARCH >= 0x010)
/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvCelsiusCaps.h                                                   *
*   Define the capabilities of the celsius D3D driver.                      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler    bertrem     10Feb99     created                 *
*                                                                           *
\***************************************************************************/

/*
 * --------------------------------------------------------------------------
 *  Direct 3D HAL Capabilities
 * --------------------------------------------------------------------------
 */

/*
 * Define which fields of the D3DDEVICECAPS structure are valid.
 */
#define CELSIUS_DEVDESC_FLAGS           D3DDD_COLORMODEL                    |   \
                                        D3DDD_DEVCAPS                       |   \
                                        D3DDD_LINECAPS                      |   \
                                        D3DDD_TRICAPS                       |   \
                                        D3DDD_DEVICERENDERBITDEPTH          |   \
                                        D3DDD_DEVICEZBUFFERBITDEPTH

/*
 * Define the color model for the D3D driver.
 */
#define CELSIUS_COLORMODEL              D3DCOLOR_RGB

/*
 * Define the device capabilities.
 */
#define CELSIUS_DEVCAPS_FLAGS           D3DDEVCAPS_FLOATTLVERTEX            |   \
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
                                        D3DDEVCAPS_CANBLTSYSTONONLOCAL      |   \
                                        D3DDEVCAPS_PUREDEVICE               |   \
                                        D3DDEVCAPS_HWVERTEXBUFFER           |   \
                                        D3DDEVCAPS_HWINDEXBUFFER            |   \
                                        D3DDEVCAPS_HWTRANSFORMANDLIGHT      |   \
                                        D3DDEVCAPS_TEXTURENONLOCALVIDMEM    |   \
                                        D3DDEVCAPS_HWRASTERIZATION

/*
 * Define the transformation capabilities.
 */
#define CELSIUS_TRANSFORMCAPS_FLAGS     D3DTRANSFORMCAPS_CLIP

/*
 * Define 3D clipping capabilities.
 */
#define CELSIUS_3DCLIPPINGCAPS          TRUE

/*
 * Define the lighting capabilities.
 */
#define CELSIUS_LIGHTINGCAPS_FLAGS      D3DLIGHTINGMODEL_RGB                |   \
                                        D3DLIGHTCAPS_POINT                  |   \
                                        D3DLIGHTCAPS_SPOT                   |   \
                                        D3DLIGHTCAPS_DIRECTIONAL
#define CELSIUS_MAX_LIGHTS              8

/*
 * Define NULL primary capabilities.
 */
#define CELSIUS_PRIMCAPS_NULL           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/*
 * Define the line capabilities.
 *
 * Miscellaneous capabilities (dwMiscCaps)
 */
#define CELSIUS_LINECAPS_MISC           D3DPMISCCAPS_MASKZ                  |   \
                                        D3DPMISCCAPS_CULLNONE               |   \
                                        D3DPMISCCAPS_CULLCW                 |   \
                                        D3DPMISCCAPS_CULLCCW                |   \
                                        D3DPMISCCAPS_BLENDOP                |   \
                                        D3DPMISCCAPS_COLORWRITEENABLE       |   \
                                        D3DPMISCCAPS_TSSARGTEMP

/*
 * Raster capabilities (dwRasterCaps)
 */
#define CELSIUS_LINECAPS_RASTER         D3DPRASTERCAPS_DITHER                   |   \
                                        D3DPRASTERCAPS_SUBPIXEL                 |   \
                                        D3DPRASTERCAPS_FOGVERTEX                |   \
                                        D3DPRASTERCAPS_FOGTABLE                 |   \
                                        D3DPRASTERCAPS_MIPMAPLODBIAS            |   \
                                        D3DPRASTERCAPS_ZBIAS                    |   \
                                        D3DPRASTERCAPS_FOGRANGE                 |   \
                                        D3DPRASTERCAPS_ANISOTROPY               |   \
                                        D3DPRASTERCAPS_WBUFFER                  |   \
                                        D3DPRASTERCAPS_WFOG                     |   \
                                        D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE    |   \
                                        D3DPRASTERCAPS_ZFOG

/*
 * Z Compare capabilities (dwZCmpCaps)
 */
#define CELSIUS_LINECAPS_ZCMP           D3DPCMPCAPS_NEVER                   |   \
                                        D3DPCMPCAPS_LESS                    |   \
                                        D3DPCMPCAPS_EQUAL                   |   \
                                        D3DPCMPCAPS_LESSEQUAL               |   \
                                        D3DPCMPCAPS_GREATER                 |   \
                                        D3DPCMPCAPS_NOTEQUAL                |   \
                                        D3DPCMPCAPS_GREATEREQUAL            |   \
                                        D3DPCMPCAPS_ALWAYS

/*
 * Source blend capabilities (dwSrcBlendCaps)
 */
#define CELSIUS_LINECAPS_SRCBLEND       D3DPBLENDCAPS_ZERO                  |   \
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

/*
 * Destination blend capabilities (dwDestBlendCaps)
 */
#define CELSIUS_LINECAPS_DESTBLEND      D3DPBLENDCAPS_ZERO                  |   \
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

/*
 * Alpha compare capabilities (dwAlphaCmpCaps)
 */
#define CELSIUS_LINECAPS_ALPHACMP       D3DPCMPCAPS_NEVER                   |   \
                                        D3DPCMPCAPS_LESS                    |   \
                                        D3DPCMPCAPS_EQUAL                   |   \
                                        D3DPCMPCAPS_LESSEQUAL               |   \
                                        D3DPCMPCAPS_GREATER                 |   \
                                        D3DPCMPCAPS_NOTEQUAL                |   \
                                        D3DPCMPCAPS_GREATEREQUAL            |   \
                                        D3DPCMPCAPS_ALWAYS

/*
 * Shading capabilities (dwShadeCaps)
 */
#define CELSIUS_LINECAPS_SHADE          D3DPSHADECAPS_COLORFLATMONO         |   \
                                        D3DPSHADECAPS_COLORFLATRGB          |   \
                                        D3DPSHADECAPS_COLORGOURAUDMONO      |   \
                                        D3DPSHADECAPS_COLORGOURAUDRGB       |   \
                                        D3DPSHADECAPS_SPECULARFLATRGB       |   \
                                        D3DPSHADECAPS_SPECULARGOURAUDRGB    |   \
                                        D3DPSHADECAPS_ALPHAFLATBLEND        |   \
                                        D3DPSHADECAPS_ALPHAGOURAUDBLEND     |   \
                                        D3DPSHADECAPS_FOGFLAT               |   \
                                        D3DPSHADECAPS_FOGGOURAUD

/*
 * Texture capabilities (dwTextureCaps)
 */
#define CELSIUS_LINECAPS_TEXTURE        D3DPTEXTURECAPS_PERSPECTIVE         |   \
                                        D3DPTEXTURECAPS_POW2                |   \
                                        D3DPTEXTURECAPS_ALPHA               |   \
                                        D3DPTEXTURECAPS_TRANSPARENCY        |   \
                                        D3DPTEXTURECAPS_CUBEMAP             |   \
                                        D3DPTEXTURECAPS_CUBEMAP_POW2        |   \
                                        D3DPTEXTURECAPS_MIPCUBEMAP          |   \
                                        D3DPTEXTURECAPS_MIPMAP              |   \
                                        D3DPTEXTURECAPS_PROJECTED           |   \
                                        D3DPTEXTURECAPS_ALPHAPALETTE

/*
 * Texture filtering capabilities (dwTextureFilterCaps)
 */
#define CELSIUS_LINECAPS_TEXTUREFILTER  D3DPTFILTERCAPS_NEAREST             |   \
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

/*
 * Texture blending capabilities (dwTextureBlendCaps)
 */
#define CELSIUS_LINECAPS_TEXTUREBLEND   D3DPTBLENDCAPS_DECAL                |   \
                                        D3DPTBLENDCAPS_MODULATE             |   \
                                        D3DPTBLENDCAPS_DECALALPHA           |   \
                                        D3DPTBLENDCAPS_MODULATEALPHA        |   \
                                        D3DPTBLENDCAPS_DECALMASK            |   \
                                        D3DPTBLENDCAPS_MODULATEMASK         |   \
                                        D3DPTBLENDCAPS_COPY                 |   \
                                        D3DPTBLENDCAPS_ADD

/*
 * Texture addressing capabilities (dwTextureAddressCaps)
 */
#define CELSIUS_LINECAPS_TEXTUREADDRESS D3DPTADDRESSCAPS_WRAP               |   \
                                        D3DPTADDRESSCAPS_MIRROR             |   \
                                        D3DPTADDRESSCAPS_CLAMP              |   \
                                        D3DPTADDRESSCAPS_INDEPENDENTUV

/*
 * Maximum stipple dimensions.
 */
#define CELSIUS_LINECAPS_STIPPLEWIDTH    0
#define CELSIUS_LINECAPS_STIPPLEHEIGHT   0

/*
 * Define the triangle capabilities.
 *
 * Miscellaneous capabilities (dwMiscCaps)
 */
#define CELSIUS_TRICAPS_MISC            D3DPMISCCAPS_MASKZ                  |   \
                                        D3DPMISCCAPS_CULLNONE               |   \
                                        D3DPMISCCAPS_CULLCW                 |   \
                                        D3DPMISCCAPS_CULLCCW                |   \
                                        D3DPMISCCAPS_BLENDOP                |   \
                                        D3DPMISCCAPS_COLORWRITEENABLE       |   \
                                        D3DPMISCCAPS_TSSARGTEMP

/*
 * Raster capabilities (dwRasterCaps)
 */
#define CELSIUS_TRICAPS_RASTER          D3DPRASTERCAPS_DITHER                   |   \
                                        D3DPRASTERCAPS_SUBPIXEL                 |   \
                                        D3DPRASTERCAPS_FOGVERTEX                |   \
                                        D3DPRASTERCAPS_FOGTABLE                 |   \
                                        D3DPRASTERCAPS_MIPMAPLODBIAS            |   \
                                        D3DPRASTERCAPS_ZBIAS                    |   \
                                        D3DPRASTERCAPS_FOGRANGE                 |   \
                                        D3DPRASTERCAPS_ANISOTROPY               |   \
                                        D3DPRASTERCAPS_WBUFFER                  |   \
                                        D3DPRASTERCAPS_WFOG                     |   \
                                        D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE    |   \
                                        D3DPRASTERCAPS_ZFOG

/*
 * Z Compare capabilities (dwZCmpCaps)
 */
#define CELSIUS_TRICAPS_ZCMP            D3DPCMPCAPS_NEVER                   |   \
                                        D3DPCMPCAPS_LESS                    |   \
                                        D3DPCMPCAPS_EQUAL                   |   \
                                        D3DPCMPCAPS_LESSEQUAL               |   \
                                        D3DPCMPCAPS_GREATER                 |   \
                                        D3DPCMPCAPS_NOTEQUAL                |   \
                                        D3DPCMPCAPS_GREATEREQUAL            |   \
                                        D3DPCMPCAPS_ALWAYS

/*
 * Source blend capabilities (dwSrcBlendCaps)
 */
#define CELSIUS_TRICAPS_SRCBLEND        D3DPBLENDCAPS_ZERO                  |   \
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

/*
 * Destination blend capabilities (dwDestBlendCaps)
 */
#define CELSIUS_TRICAPS_DESTBLEND       D3DPBLENDCAPS_ZERO                  |   \
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

/*
 * Alpha compare capabilities (dwAlphaCmpCaps)
 */
#define CELSIUS_TRICAPS_ALPHACMP        D3DPCMPCAPS_NEVER                   |   \
                                        D3DPCMPCAPS_LESS                    |   \
                                        D3DPCMPCAPS_EQUAL                   |   \
                                        D3DPCMPCAPS_LESSEQUAL               |   \
                                        D3DPCMPCAPS_GREATER                 |   \
                                        D3DPCMPCAPS_NOTEQUAL                |   \
                                        D3DPCMPCAPS_GREATEREQUAL            |   \
                                        D3DPCMPCAPS_ALWAYS

/*
 * Shading capabilities (dwShadeCaps)
 */
#define CELSIUS_TRICAPS_SHADE           D3DPSHADECAPS_COLORFLATMONO         |   \
                                        D3DPSHADECAPS_COLORFLATRGB          |   \
                                        D3DPSHADECAPS_COLORGOURAUDMONO      |   \
                                        D3DPSHADECAPS_COLORGOURAUDRGB       |   \
                                        D3DPSHADECAPS_SPECULARFLATRGB       |   \
                                        D3DPSHADECAPS_SPECULARGOURAUDRGB    |   \
                                        D3DPSHADECAPS_ALPHAFLATBLEND        |   \
                                        D3DPSHADECAPS_ALPHAGOURAUDBLEND     |   \
                                        D3DPSHADECAPS_FOGFLAT               |   \
                                        D3DPSHADECAPS_FOGGOURAUD

/*
 * Texture capabilities (dwTextureCaps)
 */
#define CELSIUS_TRICAPS_TEXTURE         D3DPTEXTURECAPS_PERSPECTIVE         |   \
                                        D3DPTEXTURECAPS_POW2                |   \
                                        D3DPTEXTURECAPS_ALPHA               |   \
                                        D3DPTEXTURECAPS_TRANSPARENCY        |   \
                                        D3DPTEXTURECAPS_CUBEMAP             |   \
                                        D3DPTEXTURECAPS_CUBEMAP_POW2        |   \
                                        D3DPTEXTURECAPS_MIPMAP              |   \
                                        D3DPTEXTURECAPS_MIPCUBEMAP          |   \
                                        D3DPTEXTURECAPS_PROJECTED           |   \
                                        D3DPTEXTURECAPS_ALPHAPALETTE

/*
 * Texture filtering capabilities (dwTextureFilterCaps)
 */
#define CELSIUS_TRICAPS_TEXTUREFILTER   D3DPTFILTERCAPS_NEAREST             |   \
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
                                        D3DPTFILTERCAPS_MAGFLINEAR          |   \
                                        D3DPTFILTERCAPS_MINFANISOTROPIC     |   \
                                        D3DPTFILTERCAPS_MAGFANISOTROPIC

/*
 * Texture blending capabilities (dwTextureBlendCaps)
 */
#define CELSIUS_TRICAPS_TEXTUREBLEND    D3DPTBLENDCAPS_DECAL                |   \
                                        D3DPTBLENDCAPS_MODULATE             |   \
                                        D3DPTBLENDCAPS_DECALALPHA           |   \
                                        D3DPTBLENDCAPS_MODULATEALPHA        |   \
                                        D3DPTBLENDCAPS_DECALMASK            |   \
                                        D3DPTBLENDCAPS_MODULATEMASK         |   \
                                        D3DPTBLENDCAPS_COPY                 |   \
                                        D3DPTBLENDCAPS_ADD

/*
 * Texture addressing capabilities (dwTextureAddressCaps)
 */
#define CELSIUS_TRICAPS_TEXTUREADDRESS  D3DPTADDRESSCAPS_WRAP               |   \
                                        D3DPTADDRESSCAPS_MIRROR             |   \
                                        D3DPTADDRESSCAPS_CLAMP              |   \
                                        D3DPTADDRESSCAPS_INDEPENDENTUV

/*
 * Maximum stipple dimensions.
 */
#define CELSIUS_TRICAPS_STIPPLEWIDTH    0
#define CELSIUS_TRICAPS_STIPPLEHEIGHT   0

/*
 * Texture Ops
 */
#define CELSIUS_CAPS_TEXTUREOPS  D3DTEXOPCAPS_DISABLE                       |   \
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
                                 D3DTEXOPCAPS_DOTPRODUCT3                   |   \
                                 D3DTEXOPCAPS_MULTIPLYADD                   |   \
                                 D3DTEXOPCAPS_LERP

/*
 * Stencil Ops
 */
#define CELSIUS_CAPS_STENCILOPS  D3DSTENCILCAPS_KEEP                        |   \
                                 D3DSTENCILCAPS_ZERO                        |   \
                                 D3DSTENCILCAPS_REPLACE                     |   \
                                 D3DSTENCILCAPS_INCRSAT                     |   \
                                 D3DSTENCILCAPS_DECRSAT                     |   \
                                 D3DSTENCILCAPS_INVERT                      |   \
                                 D3DSTENCILCAPS_INCR                        |   \
                                 D3DSTENCILCAPS_DECR

/*
 * Vertex Processing Caps
 */
#define CELSIUS_CAPS_VTXPCAPS  D3DVTXPCAPS_TEXGEN                           |   \
                               /*D3DVTXPCAPS_MATERIALSOURCE7                |*/ \
                               D3DVTXPCAPS_VERTEXFOG                        |   \
                               D3DVTXPCAPS_DIRECTIONALLIGHTS                |   \
                               D3DVTXPCAPS_POSITIONALLIGHTS                 |   \
                               D3DVTXPCAPS_LOCALVIEWER

/*
 * Define the render bit depth capabilites.
 */
#define CELSIUS_RENDERBITDEPTHCAPS      (DDBD_16 | DDBD_32)

/*
 * Define the Z buffer bit depth capabilities.
 */
#define CELSIUS_ZBUFFERBITDEPTHCAPS     (DDBD_16 | DDBD_24)

#endif  // NVARCH > 0x010
#endif  // _NVCELSIUSCAPS_H_

