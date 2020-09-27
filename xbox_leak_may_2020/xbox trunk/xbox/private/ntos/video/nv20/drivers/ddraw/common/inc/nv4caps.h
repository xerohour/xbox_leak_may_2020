#ifndef _NV4DCAPS_H_
#define _NV4DCAPS_H_

/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4DCAPS.H                                                        *
*   Define the capabilities of the NV4 D3D driver.                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/26/98 - created                      *
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
#define NV4_DEVDESC_FLAGS           D3DDD_COLORMODEL                    |   \
                                    D3DDD_DEVCAPS                       |   \
                                    D3DDD_LINECAPS                      |   \
                                    D3DDD_TRICAPS                       |   \
                                    D3DDD_DEVICERENDERBITDEPTH          |   \
                                    D3DDD_DEVICEZBUFFERBITDEPTH

/*
 * Define the color model for the D3D driver.
 */
#define NV4_COLORMODEL              D3DCOLOR_RGB

/*
 * Define the device capabilities.
 */
#define NV4_DEVCAPS_FLAGS           D3DDEVCAPS_FLOATTLVERTEX            |   \
                                    D3DDEVCAPS_EXECUTESYSTEMMEMORY      |   \
                                    D3DDEVCAPS_TLVERTEXSYSTEMMEMORY     |   \
                                    D3DDEVCAPS_TEXTUREVIDEOMEMORY       |   \
                                    D3DDEVCAPS_DRAWPRIMTLVERTEX         |   \
                                    D3DDEVCAPS_TEXTURENONLOCALVIDMEM    |   \
                                    D3DDEVCAPS_DRAWPRIMITIVES2          |   \
                                    D3DDEVCAPS_DRAWPRIMITIVES2EX        |   \
                                    D3DDEVCAPS_HWRASTERIZATION

/*
 * Define the transformation capabilities.
 */
#define NV4_TRANSFORMCAPS_FLAGS     0

/*
 * Define 3D clipping capabilities.
 */
#define NV4_3DCLIPPINGCAPS          FALSE

/*
 * Define the lighting capabilities.
 */
#define NV4_LIGHTINGCAPS_FLAGS      0
#define NV4_MAX_LIGHTS              0

/*
 * Define NULL primary capabilities.
 */
#define NV4_PRIMCAPS_NULL           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/*
 * Define the line capabilities.
 *
 * Miscellaneous capabilities (dwMiscCaps)
 */
#define NV4_LINECAPS_MISC           D3DPMISCCAPS_MASKZ                  |   \
                                    D3DPMISCCAPS_CULLNONE               |   \
                                    D3DPMISCCAPS_CULLCW                 |   \
                                    D3DPMISCCAPS_CULLCCW

/*
 * Raster capabilities (dwRasterCaps)
 */
#define NV4_LINECAPS_RASTER         D3DPRASTERCAPS_DITHER               |   \
                                    D3DPRASTERCAPS_SUBPIXEL             |   \
                                    D3DPRASTERCAPS_FOGVERTEX            |   \
                                    D3DPRASTERCAPS_FOGTABLE             |   \
                                    D3DPRASTERCAPS_MIPMAPLODBIAS        |   \
                                    D3DPRASTERCAPS_FOGRANGE             |   \
                                    D3DPRASTERCAPS_WBUFFER              |   \
                                    D3DPRASTERCAPS_WFOG                 |   \
                                    D3DPRASTERCAPS_ZFOG			
//                                  D3DPRASTERCAPS_ANISOTROPY           |   \

/*
 * Z Compare capabilities (dwZCmpCaps)
 */
#define NV4_LINECAPS_ZCMP           D3DPCMPCAPS_NEVER                   |   \
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
#define NV4_LINECAPS_SRCBLEND       D3DPBLENDCAPS_ZERO                  |   \
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
#define NV4_LINECAPS_DESTBLEND      D3DPBLENDCAPS_ZERO                  |   \
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
#define NV4_LINECAPS_ALPHACMP       D3DPCMPCAPS_NEVER                   |   \
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
#define NV4_LINECAPS_SHADE          D3DPSHADECAPS_COLORFLATMONO         |   \
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
#define NV4_LINECAPS_TEXTURE        D3DPTEXTURECAPS_PERSPECTIVE         |   \
                                    D3DPTEXTURECAPS_POW2                |   \
                                    D3DPTEXTURECAPS_ALPHA               |   \
                                    D3DPTEXTURECAPS_TRANSPARENCY        |   \
                                    D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE

/*
 * Texture filtering capabilities (dwTextureFilterCaps)
 */
#define NV4_LINECAPS_TEXTUREFILTER  D3DPTFILTERCAPS_NEAREST             |   \
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
#define NV4_LINECAPS_TEXTUREBLEND   D3DPTBLENDCAPS_DECAL                |   \
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
#define NV4_LINECAPS_TEXTUREADDRESS D3DPTADDRESSCAPS_WRAP               |   \
                                    D3DPTADDRESSCAPS_MIRROR             |   \
                                    D3DPTADDRESSCAPS_CLAMP              |   \
                                    D3DPTADDRESSCAPS_INDEPENDENTUV

/*
 * Maximum stipple dimensions.
 */
#define NV4_LINECAPS_STIPPLEWIDTH    0
#define NV4_LINECAPS_STIPPLEHEIGHT   0

/*
 * Define the triangle capabilities.
 *
 * Miscellaneous capabilities (dwMiscCaps)
 */
#define NV4_TRICAPS_MISC            D3DPMISCCAPS_MASKZ                  |   \
                                    D3DPMISCCAPS_CULLNONE               |   \
                                    D3DPMISCCAPS_CULLCW                 |   \
                                    D3DPMISCCAPS_CULLCCW

/*
 * Raster capabilities (dwRasterCaps)
 */
#define NV4_TRICAPS_RASTER          D3DPRASTERCAPS_DITHER               |   \
                                    D3DPRASTERCAPS_SUBPIXEL             |   \
                                    D3DPRASTERCAPS_FOGVERTEX            |   \
                                    D3DPRASTERCAPS_FOGTABLE             |   \
                                    D3DPRASTERCAPS_MIPMAPLODBIAS        |   \
                                    D3DPRASTERCAPS_FOGRANGE             |   \
                                    D3DPRASTERCAPS_WBUFFER              |   \
                                    D3DPRASTERCAPS_WFOG                 |   \
                                    D3DPRASTERCAPS_ZFOG
//                                  D3DPRASTERCAPS_ANISOTROPY           |   \

/*
 * Z Compare capabilities (dwZCmpCaps)
 */
#define NV4_TRICAPS_ZCMP            D3DPCMPCAPS_NEVER                   |   \
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
#define NV4_TRICAPS_SRCBLEND        D3DPBLENDCAPS_ZERO                  |   \
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
#define NV4_TRICAPS_DESTBLEND       D3DPBLENDCAPS_ZERO                  |   \
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
#define NV4_TRICAPS_ALPHACMP        D3DPCMPCAPS_NEVER                   |   \
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
#define NV4_TRICAPS_SHADE           D3DPSHADECAPS_COLORFLATMONO         |   \
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
#define NV4_TRICAPS_TEXTURE         D3DPTEXTURECAPS_PERSPECTIVE         |   \
                                    D3DPTEXTURECAPS_POW2                |   \
                                    D3DPTEXTURECAPS_ALPHA               |   \
                                    D3DPTEXTURECAPS_TRANSPARENCY        |   \
                                    D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE

/*
 * Texture filtering capabilities (dwTextureFilterCaps)
 */
#define NV4_TRICAPS_TEXTUREFILTER   D3DPTFILTERCAPS_NEAREST             |   \
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
                                    //D3DPTFILTERCAPS_MINFANISOTROPIC   
                                    //D3DPTFILTERCAPS_MAGFANISOTROPIC

/*
 * Texture blending capabilities (dwTextureBlendCaps)
 */
#define NV4_TRICAPS_TEXTUREBLEND    D3DPTBLENDCAPS_DECAL                |   \
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
#define NV4_TRICAPS_TEXTUREADDRESS  D3DPTADDRESSCAPS_WRAP               |   \
                                    D3DPTADDRESSCAPS_MIRROR             |   \
                                    D3DPTADDRESSCAPS_CLAMP              |   \
                                    D3DPTADDRESSCAPS_INDEPENDENTUV

/*
 * Maximum stipple dimensions.
 */
#define NV4_TRICAPS_STIPPLEWIDTH    0
#define NV4_TRICAPS_STIPPLEHEIGHT   0

/*
 * Texture Ops
 */
#define NV4_CAPS_TEXTUREOPS   D3DTEXOPCAPS_DISABLE                      |   \
                              D3DTEXOPCAPS_SELECTARG1                   |   \
                              D3DTEXOPCAPS_SELECTARG2                   |   \
                              D3DTEXOPCAPS_MODULATE                     |   \
                              D3DTEXOPCAPS_MODULATE2X                   |   \
                              D3DTEXOPCAPS_MODULATE4X                   |   \
                              D3DTEXOPCAPS_ADD                          |   \
                              D3DTEXOPCAPS_ADDSIGNED                    |   \
                              D3DTEXOPCAPS_ADDSIGNED2X                  |   \
                              D3DTEXOPCAPS_ADDSMOOTH                    |   \
                              D3DTEXOPCAPS_SUBTRACT                     |   \
                              D3DTEXOPCAPS_BLENDDIFFUSEALPHA            |   \
                              D3DTEXOPCAPS_BLENDTEXTUREALPHA            |   \
                              D3DTEXOPCAPS_BLENDFACTORALPHA             |   \
                              D3DTEXOPCAPS_BLENDTEXTUREALPHAPM          |   \
                              D3DTEXOPCAPS_BLENDCURRENTALPHA            |   \
                              D3DTEXOPCAPS_PREMODULATE                  |   \
                              D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR       |   \
                              D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA       |   \
                              D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR    |   \
                              D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA

/*
 * Stencil Ops
 */
#define NV4_CAPS_STENCILOPS   D3DSTENCILCAPS_KEEP                       |   \
                              D3DSTENCILCAPS_ZERO                       |   \
                              D3DSTENCILCAPS_REPLACE                    |   \
                              D3DSTENCILCAPS_INCRSAT                    |   \
                              D3DSTENCILCAPS_DECRSAT                    |   \
                              D3DSTENCILCAPS_INVERT                     |   \
                              D3DSTENCILCAPS_INCR                       |   \
                              D3DSTENCILCAPS_DECR

/*
 * Vertex Processing Caps
 */
#define NV4_CAPS_VTXPCAPS     0

/*
 * Define the render bit depth capabilites.
 */
#define NV4_RENDERBITDEPTHCAPS      (DDBD_16 | DDBD_32)

/*
 * Define the Z buffer bit depth capabilities.
 */
#define NV4_ZBUFFERBITDEPTHCAPS     (DDBD_16 | DDBD_24)

/*
 * Define the maximum execute buffer size capabilites.
 */
#define NV4_MAXBUFFERSIZE           0

/*
 * Define the maximum vertex count
 */
#define NV4_MAXVERTEXCOUNT          NVD3D_DEFAULT_TL_NUM

#define NV4_CAPS_MAX_TEXTURE_REPEAT                  0 // nv4's max tex coord is actually ~2^19, independent of texture
                                                       // size and geometry size, but apparently MS doesn't look at this
                                                       // value if D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE is set
#define NV4_CAPS_MAX_TEXTURE_ASPECT_RATIO            0
#define NV4_CAPS_MAX_ANISOTROPY                      0 // Means we support anisotropic filtering.
//#define NV4_CAPS_MAX_ANISOTROPY                      2 // Means we support anisotropic filtering.

#define NV4_CAPS_GUARD_BAND_LEFT                     -2048
#define NV4_CAPS_GUARD_BAND_TOP                      -2048
#define NV4_CAPS_GUARD_BAND_RIGHT                    2047
#define NV4_CAPS_GUARD_BAND_BOTTOM                   2047

#define NV4_CAPS_EXTENTS_ADJUST                      0
#define NV4_CAPS_MAX_UV_PAIRS                        8 // Number of texture uv's supported.
#define NV4_CAPS_FVF_CAPS                            (NV4_CAPS_MAX_UV_PAIRS | D3DFVFCAPS_DONOTSTRIPELEMENTS)
#define NV4_CAPS_MAX_TEXTURE_BLEND_STATES            4
#define NV4_CAPS_MAX_SIMULTANEOUS_TEXTURES           2

// DX7 stuff
#define NV4_CAPS_MAX_ACTIVE_LIGHTS                   0
#define NV4_CAPS_MAX_USER_CLIP_PLANES                0
#define NV4_CAPS_MAX_VERTEX_BLEND_MATRICES           0

#endif  // _NV4DCAPS_H_
