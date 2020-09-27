/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       caps.hpp
 *  Content:    Defintions for Kelvin device capabilities.
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#define KELVIN_CAPS_CAPS                D3DCAPS_READ_SCANLINE

// Define the device capabilities.
#define KELVIN_DEVCAPS_FLAGS           (D3DDEVCAPS_EXECUTESYSTEMMEMORY     | \
                                       D3DDEVCAPS_EXECUTEVIDEOMEMORY      | \
                                       D3DDEVCAPS_TLVERTEXSYSTEMMEMORY    | \
                                       D3DDEVCAPS_TLVERTEXVIDEOMEMORY     | \
                                       D3DDEVCAPS_TEXTUREVIDEOMEMORY      | \
                                       D3DDEVCAPS_DRAWPRIMTLVERTEX        | \
                                       D3DDEVCAPS_CANRENDERAFTERFLIP      | \
                                       D3DDEVCAPS_TEXTURENONLOCALVIDMEM   | \
                                       D3DDEVCAPS_DRAWPRIMITIVES2         | \
                                       D3DDEVCAPS_DRAWPRIMITIVES2EX       | \
                                       D3DDEVCAPS_HWTRANSFORMANDLIGHT     | \
                                       D3DDEVCAPS_CANBLTSYSTONONLOCAL     | \
                                       D3DDEVCAPS_PUREDEVICE              | \
                                       D3DDEVCAPS_HWRASTERIZATION         | \
                                       D3DDEVCAPS_QUINTICRTPATCHES        | \
                                       D3DDEVCAPS_RTPATCHES)


// Miscellaneous capabilities (dwMiscCaps)
#define KELVIN_TRICAPS_MISC            (D3DPMISCCAPS_MASKZ                | \
                                        D3DPMISCCAPS_CULLNONE             | \
                                        D3DPMISCCAPS_CULLCW               | \
                                        D3DPMISCCAPS_CULLCCW              | \
                                        D3DPMISCCAPS_BLENDOP              | \
                                        D3DPMISCCAPS_COLORWRITEENABLE     | \
                                        D3DPMISCCAPS_TSSARGTEMP)

// Raster capabilities (dwRasterCaps)
//
// Note that unlike the Nvidia driver, we don't say we support FOGVERTEX
// mode (since the hardware doesn't actually do it)
#define KELVIN_TRICAPS_RASTER          (D3DPRASTERCAPS_DITHER             | \
                                        D3DPRASTERCAPS_FOGTABLE           | \
                                        D3DPRASTERCAPS_MIPMAPLODBIAS      | \
                                        D3DPRASTERCAPS_ZBIAS              | \
                                        D3DPRASTERCAPS_FOGRANGE           | \
                                        D3DPRASTERCAPS_ANISOTROPY         | \
                                        D3DPRASTERCAPS_WBUFFER            | \
                                        D3DPRASTERCAPS_WFOG               | \
                                        D3DPRASTERCAPS_ZFOG               | \
                                        D3DPRASTERCAPS_ANTIALIASEDGES)

// Z Compare capabilities (dwZCmpCaps)
#define KELVIN_TRICAPS_ZCMP            (D3DPCMPCAPS_NEVER                 | \
                                        D3DPCMPCAPS_LESS                  | \
                                        D3DPCMPCAPS_EQUAL                 | \
                                        D3DPCMPCAPS_LESSEQUAL             | \
                                        D3DPCMPCAPS_GREATER               | \
                                        D3DPCMPCAPS_NOTEQUAL              | \
                                        D3DPCMPCAPS_GREATEREQUAL          | \
                                        D3DPCMPCAPS_ALWAYS)

// Source blend capabilities (dwSrcBlendCaps)
#define KELVIN_TRICAPS_SRCBLEND        (D3DPBLENDCAPS_ZERO                | \
                                        D3DPBLENDCAPS_ONE                 | \
                                        D3DPBLENDCAPS_SRCCOLOR            | \
                                        D3DPBLENDCAPS_INVSRCCOLOR         | \
                                        D3DPBLENDCAPS_SRCALPHA            | \
                                        D3DPBLENDCAPS_INVSRCALPHA         | \
                                        D3DPBLENDCAPS_DESTALPHA           | \
                                        D3DPBLENDCAPS_INVDESTALPHA        | \
                                        D3DPBLENDCAPS_DESTCOLOR           | \
                                        D3DPBLENDCAPS_INVDESTCOLOR        | \
                                        D3DPBLENDCAPS_SRCALPHASAT         | \
                                        D3DPBLENDCAPS_BOTHSRCALPHA        | \
                                        D3DPBLENDCAPS_BOTHINVSRCALPHA)

// Destination blend capabilities (dwDestBlendCaps)
#define KELVIN_TRICAPS_DESTBLEND       (D3DPBLENDCAPS_ZERO                | \
                                        D3DPBLENDCAPS_ONE                 | \
                                        D3DPBLENDCAPS_SRCCOLOR            | \
                                        D3DPBLENDCAPS_INVSRCCOLOR         | \
                                        D3DPBLENDCAPS_SRCALPHA            | \
                                        D3DPBLENDCAPS_INVSRCALPHA         | \
                                        D3DPBLENDCAPS_DESTALPHA           | \
                                        D3DPBLENDCAPS_INVDESTALPHA        | \
                                        D3DPBLENDCAPS_DESTCOLOR           | \
                                        D3DPBLENDCAPS_INVDESTCOLOR        | \
                                        D3DPBLENDCAPS_SRCALPHASAT         | \
                                        D3DPBLENDCAPS_BOTHSRCALPHA        | \
                                        D3DPBLENDCAPS_BOTHINVSRCALPHA)

// Alpha compare capabilities (dwAlphaCmpCaps)
#define KELVIN_TRICAPS_ALPHACMP        (D3DPCMPCAPS_NEVER                 | \
                                        D3DPCMPCAPS_LESS                  | \
                                        D3DPCMPCAPS_EQUAL                 | \
                                        D3DPCMPCAPS_LESSEQUAL             | \
                                        D3DPCMPCAPS_GREATER               | \
                                        D3DPCMPCAPS_NOTEQUAL              | \
                                        D3DPCMPCAPS_GREATEREQUAL          | \
                                        D3DPCMPCAPS_ALWAYS)
// Shading capabilities (dwShadeCaps)
#define KELVIN_TRICAPS_SHADE           (D3DPSHADECAPS_COLORGOURAUDRGB     | \
                                        D3DPSHADECAPS_SPECULARGOURAUDRGB  | \
                                        D3DPSHADECAPS_ALPHAGOURAUDBLEND   | \
                                        D3DPSHADECAPS_FOGGOURAUD)

// Texture capabilities (dwTextureCaps)
#define KELVIN_TRICAPS_TEXTURE         (D3DPTEXTURECAPS_PERSPECTIVE       | \
                                        D3DPTEXTURECAPS_POW2              | \
                                        D3DPTEXTURECAPS_ALPHA             | \
                                        D3DPTEXTURECAPS_CUBEMAP           | \
                                        D3DPTEXTURECAPS_CUBEMAP_POW2      | \
                                        D3DPTEXTURECAPS_MIPMAP            | \
                                        D3DPTEXTURECAPS_MIPCUBEMAP        | \
                                        D3DPTEXTURECAPS_PROJECTED         | \
                                        D3DPTEXTURECAPS_ALPHAPALETTE      | \
                                        D3DPTEXTURECAPS_VOLUMEMAP         | \
                                        D3DPTEXTURECAPS_VOLUMEMAP_POW2    | \
                                        D3DPTEXTURECAPS_MIPVOLUMEMAP)

// Texture filtering capabilities (dwTextureFilterCaps)
#define KELVIN_TRICAPS_TEXTUREFILTER   (D3DPTFILTERCAPS_MINFPOINT         | \
                                        D3DPTFILTERCAPS_MINFLINEAR        | \
                                        D3DPTFILTERCAPS_MIPFPOINT         | \
                                        D3DPTFILTERCAPS_MIPFLINEAR        | \
                                        D3DPTFILTERCAPS_MAGFPOINT         | \
                                        D3DPTFILTERCAPS_MAGFLINEAR        | \
                                        D3DPTFILTERCAPS_MINFANISOTROPIC   | \
                                        D3DPTFILTERCAPS_MAGFANISOTROPIC   | \
                                        D3DPTFILTERCAPS_MAGFAFLATCUBIC    | \
                                        D3DPTFILTERCAPS_MAGFGAUSSIANCUBIC)

// Texture addressing capabilities (dwTextureAddressCaps)
#define KELVIN_TRICAPS_TEXTUREADDRESS  (D3DPTADDRESSCAPS_WRAP             | \
                                        D3DPTADDRESSCAPS_MIRROR           | \
                                        D3DPTADDRESSCAPS_CLAMP            | \
                                        D3DPTADDRESSCAPS_INDEPENDENTUV)

#define KELVIN_CAPS_LINECAPS           (D3DLINECAPS_TEXTURE               | \
                                        D3DLINECAPS_ZTEST                 | \
                                        D3DLINECAPS_BLEND                 | \
                                        D3DLINECAPS_ALPHACMP              | \
                                        D3DLINECAPS_FOG)

#define KELVIN_CAPS_MAX_TEXTURE_REPEAT           8192

#define KELVIN_CAPS_MAX_TEXTURE_ASPECT_RATIO     0

// Means we support anisotropic filtering
#define KELVIN_CAPS_MAX_ANISOTROPY               2

const float KELVIN_CAPS_GUARD_BAND_LEFT = -1.0e8f;   // nv10/20 can use a guardband as large as 1e25, but 1e8 appears
const float KELVIN_CAPS_GUARD_BAND_TOP = -1.0e8f;    // to be the largest value that the game re-volt will handle.
const float KELVIN_CAPS_GUARD_BAND_RIGHT = 1.0e8f;   // anything larger aggravates an app bug that causes it to
const float KELVIN_CAPS_GUARD_BAND_BOTTOM = 1.0e8f;  // send us completely broken geometry.

#define KELVIN_CAPS_EXTENTS_ADJUST               0

#define KELVIN_CAPS_STENCILOPS         (D3DSTENCILCAPS_KEEP               | \
                                        D3DSTENCILCAPS_ZERO               | \
                                        D3DSTENCILCAPS_REPLACE            | \
                                        D3DSTENCILCAPS_INCRSAT            | \
                                        D3DSTENCILCAPS_DECRSAT            | \
                                        D3DSTENCILCAPS_INVERT             | \
                                        D3DSTENCILCAPS_INCR               | \
                                        D3DSTENCILCAPS_DECR)

#define KELVIN_CAPS_MAX_UV_PAIRS                  4    // Number of texture uv's supported

#define KELVIN_CAPS_FVF_CAPS           (KELVIN_CAPS_MAX_UV_PAIRS | \
                                        D3DFVFCAPS_DONOTSTRIPELEMENTS)

#define KELVIN_CAPS_TEXTUREOPS         (D3DTEXOPCAPS_DISABLE                   | \
                                        D3DTEXOPCAPS_SELECTARG1                | \
                                        D3DTEXOPCAPS_SELECTARG2                | \
                                        D3DTEXOPCAPS_MODULATE                  | \
                                        D3DTEXOPCAPS_MODULATE2X                | \
                                        D3DTEXOPCAPS_MODULATE4X                | \
                                        D3DTEXOPCAPS_ADD                       | \
                                        D3DTEXOPCAPS_ADDSIGNED                 | \
                                        D3DTEXOPCAPS_ADDSIGNED2X               | \
                                        D3DTEXOPCAPS_SUBTRACT                  | \
                                        D3DTEXOPCAPS_ADDSMOOTH                 | \
                                        D3DTEXOPCAPS_BLENDDIFFUSEALPHA         | \
                                        D3DTEXOPCAPS_BLENDTEXTUREALPHA         | \
                                        D3DTEXOPCAPS_BLENDFACTORALPHA          | \
                                        D3DTEXOPCAPS_BLENDTEXTUREALPHAPM       | \
                                        D3DTEXOPCAPS_BLENDCURRENTALPHA         | \
                                        D3DTEXOPCAPS_PREMODULATE               | \
                                        D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR    | \
                                        D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA    | \
                                        D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR | \
                                        D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA | \
                                        D3DTEXOPCAPS_BUMPENVMAP                | \
                                        D3DTEXOPCAPS_BUMPENVMAPLUMINANCE       | \
                                        D3DTEXOPCAPS_DOTPRODUCT3)

#define KELVIN_CAPS_MAX_TEXTURE_BLEND_STAGES     4 // no remapping done on xbox, max is 4

#define KELVIN_CAPS_MAX_SIMULTANEOUS_TEXTURES    4

#define KELVIN_CAPS_VTXPCAPS           (D3DVTXPCAPS_TEXGEN                | \
                                        D3DVTXPCAPS_MATERIALSOURCE7       | \
                                        D3DVTXPCAPS_DIRECTIONALLIGHTS     | \
                                        D3DVTXPCAPS_POSITIONALLIGHTS      | \
                                        D3DVTXPCAPS_LOCALVIEWER           | \
                                        D3DVTXPCAPS_NO_VSDT_UBYTE4)

#define KELVIN_CAPS_MAX_ACTIVE_LIGHTS            8

#define KELVIN_CAPS_MAX_VERTEX_BLEND_MATRICES    4

#define KELVIN_CAPS_MAX_STREAMS                  16

#define KELVIN_CAPS_MAX_VSHADER_CONSTS           96

} // end namespace
