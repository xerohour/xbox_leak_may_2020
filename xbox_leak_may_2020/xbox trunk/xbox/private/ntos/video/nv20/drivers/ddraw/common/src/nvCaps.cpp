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
// **************************************************************************
//  Content:    Routines to export hardware capabilities
//
//  History:
//      Craig Duttweiler    bertrem     10Feb1999   added celsius
//      Craig Duttweiler    bertrem     26Apr2000   added kelvin
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#include "nv4caps.h"
#include "nvCelsiusCaps.h"
#include "nvKelvinCaps.h"

void nvSetHardwareCaps (void)
{
    dbgTracePush ("nvSetHardwareCaps");

#if (NVARCH >= 0x20)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {

        // D3DDEVICEDESC_V1 caps

        memset (&(getDC()->nvD3DDevCaps.dd1Caps), 0, sizeof(D3DDEVICEDESC_V1));

        getDC()->nvD3DDevCaps.dd1Caps.dwSize                            = sizeof(D3DDEVICEDESC_V1);
        getDC()->nvD3DDevCaps.dd1Caps.dwFlags                           = KELVIN_DEVDESC_FLAGS;
        getDC()->nvD3DDevCaps.dd1Caps.dcmColorModel                     = KELVIN_COLORMODEL;
        getDC()->nvD3DDevCaps.dd1Caps.dwDevCaps                         = KELVIN_DEVCAPS_FLAGS;

        getDC()->nvD3DDevCaps.dd1Caps.dtcTransformCaps.dwSize           = sizeof(D3DTRANSFORMCAPS);
        getDC()->nvD3DDevCaps.dd1Caps.dtcTransformCaps.dwCaps           = KELVIN_TRANSFORMCAPS_FLAGS;

        getDC()->nvD3DDevCaps.dd1Caps.bClipping                         = KELVIN_3DCLIPPINGCAPS;

        getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwSize            = sizeof(D3DLIGHTINGCAPS);
        getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwCaps            = KELVIN_LIGHTINGCAPS_FLAGS;
        getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwLightingModel   = D3DLIGHTINGMODEL_RGB;
        getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwNumLights       = KELVIN_MAX_LIGHTS;

        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwSize                = sizeof(D3DPRIMCAPS);
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwMiscCaps            = KELVIN_LINECAPS_MISC;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwRasterCaps          = KELVIN_LINECAPS_RASTER;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwZCmpCaps            = KELVIN_LINECAPS_ZCMP;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwSrcBlendCaps        = KELVIN_LINECAPS_SRCBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwDestBlendCaps       = KELVIN_LINECAPS_DESTBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwAlphaCmpCaps        = KELVIN_LINECAPS_ALPHACMP;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwShadeCaps           = KELVIN_LINECAPS_SHADE;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureCaps         = KELVIN_LINECAPS_TEXTURE;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureFilterCaps   = KELVIN_LINECAPS_TEXTUREFILTER;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureBlendCaps    = KELVIN_LINECAPS_TEXTUREBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureAddressCaps  = KELVIN_LINECAPS_TEXTUREADDRESS;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwStippleWidth        = KELVIN_LINECAPS_STIPPLEWIDTH;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwStippleHeight       = KELVIN_LINECAPS_STIPPLEHEIGHT;

        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwSize                 = sizeof(D3DPRIMCAPS);
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwMiscCaps             = KELVIN_TRICAPS_MISC;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwRasterCaps           = KELVIN_TRICAPS_RASTER;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwZCmpCaps             = KELVIN_TRICAPS_ZCMP;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwSrcBlendCaps         = KELVIN_TRICAPS_SRCBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwDestBlendCaps        = KELVIN_TRICAPS_DESTBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwAlphaCmpCaps         = KELVIN_TRICAPS_ALPHACMP;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwShadeCaps            = KELVIN_TRICAPS_SHADE;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureCaps          = KELVIN_TRICAPS_TEXTURE;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureFilterCaps    = KELVIN_TRICAPS_TEXTUREFILTER;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureBlendCaps     = KELVIN_TRICAPS_TEXTUREBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureAddressCaps   = KELVIN_TRICAPS_TEXTUREADDRESS;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwStippleWidth         = KELVIN_TRICAPS_STIPPLEWIDTH;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwStippleHeight        = KELVIN_TRICAPS_STIPPLEHEIGHT;

        getDC()->nvD3DDevCaps.dd1Caps.dwDeviceRenderBitDepth            = KELVIN_RENDERBITDEPTHCAPS;
        getDC()->nvD3DDevCaps.dd1Caps.dwDeviceZBufferBitDepth           = KELVIN_ZBUFFERBITDEPTHCAPS;
        getDC()->nvD3DDevCaps.dd1Caps.dwMaxBufferSize                   = KELVIN_MAXBUFFERSIZE;
        getDC()->nvD3DDevCaps.dd1Caps.dwMaxVertexCount                  = KELVIN_MAXVERTEXCOUNT;

        // D3DHAL_D3DEXTENDEDCAPS

        memset (&(getDC()->nvD3DDevCaps.d3dExtCaps), 0, sizeof(D3DHAL_D3DEXTENDEDCAPS));

        getDC()->nvD3DDevCaps.d3dExtCaps.dwSize                         = 0;  // to be set later in getDriverInfo
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMinTextureWidth              = 1;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxTextureWidth              = pDriverData->nvD3DPerfData.dwMaxTextureSize;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMinTextureHeight             = 1;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxTextureHeight             = pDriverData->nvD3DPerfData.dwMaxTextureSize;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMinStippleWidth              = 0;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxStippleWidth              = 0;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMinStippleHeight             = 0;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxStippleHeight             = 0;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxTextureRepeat             = KELVIN_CAPS_MAX_TEXTURE_REPEAT;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxTextureAspectRatio        = KELVIN_CAPS_MAX_TEXTURE_ASPECT_RATIO;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxAnisotropy                = KELVIN_CAPS_MAX_ANISOTROPY;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvGuardBandLeft                = KELVIN_CAPS_GUARD_BAND_LEFT;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvGuardBandTop                 = KELVIN_CAPS_GUARD_BAND_TOP;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvGuardBandRight               = KELVIN_CAPS_GUARD_BAND_RIGHT;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvGuardBandBottom              = KELVIN_CAPS_GUARD_BAND_BOTTOM;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvExtentsAdjust                = KELVIN_CAPS_EXTENTS_ADJUST;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwStencilCaps                  = KELVIN_CAPS_STENCILOPS;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwFVFCaps                      = KELVIN_CAPS_FVF_CAPS
                                                                            | D3DFVFCAPS_DONOTSTRIPELEMENTS; // bdw: leave here
        getDC()->nvD3DDevCaps.d3dExtCaps.dwTextureOpCaps                = KELVIN_CAPS_TEXTUREOPS;
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxTextureBlendStages         = KELVIN_CAPS_MAX_TEXTURE_BLEND_STATES;
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxSimultaneousTextures       = KELVIN_CAPS_MAX_SIMULTANEOUS_TEXTURES;

        // BUBGUG still need some data here
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxActiveLights              = KELVIN_CAPS_MAX_ACTIVE_LIGHTS;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvMaxVertexW                   = 1.0e10; //as per refrast
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxUserClipPlanes             = KELVIN_CAPS_MAX_USER_CLIP_PLANES_EXPORTED;
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxVertexBlendMatrices        = KELVIN_CAPS_MAX_VERTEX_BLEND_MATRICES;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwVertexProcessingCaps         = KELVIN_CAPS_VTXPCAPS;

        // HMH need to get rid of constants
        // set up D3DCAPS8, all the same info, new easier to swallow gelcap form.
        LPDDHALINFO pHalInfo = GET_HALINFO();

        //getDC()->nvD3DDevCaps.dwD3DCap8.DeviceType = ??
        getDC()->nvD3DDevCaps.dwD3DCap8.AdapterOrdinal = 0;

        /* Filled in by run-time */
        getDC()->nvD3DDevCaps.dwD3DCap8.Caps                            = pHalInfo->ddCaps.dwCaps;
        getDC()->nvD3DDevCaps.dwD3DCap8.Caps2                           = pHalInfo->ddCaps.dwCaps2;
        getDC()->nvD3DDevCaps.dwD3DCap8.Caps3                           = 0;
        getDC()->nvD3DDevCaps.dwD3DCap8.PresentationIntervals           = 0;
        getDC()->nvD3DDevCaps.dwD3DCap8.CursorCaps                      = 0;
        /* END Filled in by run-time */

        getDC()->nvD3DDevCaps.dwD3DCap8.DevCaps                         = KELVIN_DEVCAPS_FLAGS;

        getDC()->nvD3DDevCaps.dwD3DCap8.PrimitiveMiscCaps               = KELVIN_TRICAPS_MISC;
        getDC()->nvD3DDevCaps.dwD3DCap8.RasterCaps                      = KELVIN_TRICAPS_RASTER;
        getDC()->nvD3DDevCaps.dwD3DCap8.ZCmpCaps                        = KELVIN_TRICAPS_ZCMP;
        getDC()->nvD3DDevCaps.dwD3DCap8.SrcBlendCaps                    = KELVIN_TRICAPS_SRCBLEND;
        getDC()->nvD3DDevCaps.dwD3DCap8.DestBlendCaps                   = KELVIN_TRICAPS_DESTBLEND;
        getDC()->nvD3DDevCaps.dwD3DCap8.AlphaCmpCaps                    = KELVIN_TRICAPS_ALPHACMP;
        getDC()->nvD3DDevCaps.dwD3DCap8.ShadeCaps                       = KELVIN_TRICAPS_SHADE;
        getDC()->nvD3DDevCaps.dwD3DCap8.TextureCaps                     = KELVIN_TRICAPS_TEXTURE;

        getDC()->nvD3DDevCaps.dwD3DCap8.TextureFilterCaps               = KELVIN_TRICAPS_TEXTUREFILTER;
        getDC()->nvD3DDevCaps.dwD3DCap8.CubeTextureFilterCaps           = KELVIN_TRICAPS_TEXTUREFILTER;
        getDC()->nvD3DDevCaps.dwD3DCap8.VolumeTextureFilterCaps         = KELVIN_TRICAPS_VOLUMETEXFILTER;
        getDC()->nvD3DDevCaps.dwD3DCap8.TextureAddressCaps              = KELVIN_TRICAPS_TEXTUREADDRESS;
        getDC()->nvD3DDevCaps.dwD3DCap8.VolumeTextureAddressCaps        = KELVIN_TRICAPS_TEXTUREADDRESS;
        getDC()->nvD3DDevCaps.dwD3DCap8.LineCaps                        = D3DLINECAPS_TEXTURE     |
                                                                          D3DLINECAPS_ZTEST       |
                                                                          D3DLINECAPS_BLEND       |
                                                                          D3DLINECAPS_ALPHACMP    |
                                                                          D3DLINECAPS_FOG;

        getDC()->nvD3DDevCaps.dwD3DCap8.MaxTextureWidth                 = pDriverData->nvD3DPerfData.dwMaxTextureSize;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxTextureHeight                = pDriverData->nvD3DPerfData.dwMaxTextureSize;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxVolumeExtent                 = pDriverData->nvD3DPerfData.dwMaxVolumeTextureSize;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxTextureRepeat                = KELVIN_CAPS_MAX_TEXTURE_REPEAT;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxTextureAspectRatio           = KELVIN_CAPS_MAX_TEXTURE_ASPECT_RATIO;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxAnisotropy                   = KELVIN_CAPS_MAX_ANISOTROPY;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxVertexW                      = 1.0e10; // as per refrast

        getDC()->nvD3DDevCaps.dwD3DCap8.GuardBandLeft                   = KELVIN_CAPS_GUARD_BAND_LEFT;
        getDC()->nvD3DDevCaps.dwD3DCap8.GuardBandTop                    = KELVIN_CAPS_GUARD_BAND_TOP;
        getDC()->nvD3DDevCaps.dwD3DCap8.GuardBandRight                  = KELVIN_CAPS_GUARD_BAND_RIGHT;
        getDC()->nvD3DDevCaps.dwD3DCap8.GuardBandBottom                 = KELVIN_CAPS_GUARD_BAND_BOTTOM;
        getDC()->nvD3DDevCaps.dwD3DCap8.ExtentsAdjust                   = KELVIN_CAPS_EXTENTS_ADJUST;

        getDC()->nvD3DDevCaps.dwD3DCap8.StencilCaps                     = KELVIN_CAPS_STENCILOPS;
        getDC()->nvD3DDevCaps.dwD3DCap8.FVFCaps                         = KELVIN_CAPS_FVF_CAPS    |
                                                                          D3DFVFCAPS_DONOTSTRIPELEMENTS; // bdw: leave here
        getDC()->nvD3DDevCaps.dwD3DCap8.TextureOpCaps                   = KELVIN_CAPS_TEXTUREOPS;

        getDC()->nvD3DDevCaps.dwD3DCap8.MaxTextureBlendStages           = KELVIN_CAPS_MAX_TEXTURE_BLEND_STATES;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxSimultaneousTextures         = KELVIN_CAPS_MAX_SIMULTANEOUS_TEXTURES;

        getDC()->nvD3DDevCaps.dwD3DCap8.VertexProcessingCaps            = KELVIN_CAPS_VTXPCAPS;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxActiveLights                 = KELVIN_CAPS_MAX_ACTIVE_LIGHTS;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxUserClipPlanes               = KELVIN_CAPS_MAX_USER_CLIP_PLANES_EXPORTED;

        getDC()->nvD3DDevCaps.dwD3DCap8.MaxVertexBlendMatrices          = KELVIN_CAPS_MAX_VERTEX_BLEND_MATRICES;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxVertexBlendMatrixIndex       = 0;  // no indexed (palettized) matrices

        getDC()->nvD3DDevCaps.dwD3DCap8.MaxPointSize                    = 64.0f;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxPrimitiveCount               = 0xfffff;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxVertexIndex                  = 0xfffff;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxStreams                      = KELVIN_CAPS_MAX_STREAMS;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxStreamStride                 = 256;
        getDC()->nvD3DDevCaps.dwD3DCap8.VertexShaderVersion             = D3DVS_VERSION(1, 1);
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxVertexShaderConst            = KELVIN_CAPS_MAX_VSHADER_CONSTS;
        getDC()->nvD3DDevCaps.dwD3DCap8.PixelShaderVersion              = D3DPS_VERSION(1, 1);
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxPixelShaderValue             = 1.0f;

        // fix some stuff up
        if (!pDriverData->nvD3DPerfData.dwHaveVolumeTextures) {
            // no volume maps on nv20. hopefully fixed in nv25, etc...
            DWORD dwVolumeCapMask = ~(D3DPTEXTURECAPS_VOLUMEMAP      |
                                      D3DPTEXTURECAPS_VOLUMEMAP_POW2 |
                                      D3DPTEXTURECAPS_MIPVOLUMEMAP);
            getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureCaps &= dwVolumeCapMask;
            getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureCaps  &= dwVolumeCapMask;
            getDC()->nvD3DDevCaps.dwD3DCap8.TextureCaps             &= dwVolumeCapMask;
            getDC()->nvD3DDevCaps.dwD3DCap8.VolumeTextureFilterCaps  = 0;
            getDC()->nvD3DDevCaps.dwD3DCap8.VolumeTextureAddressCaps = 0;
        }

        if (!pDriverData->nvD3DPerfData.dwHaveAnisotropic) {
            DWORD dwAnisoCapMask = ~(D3DPTFILTERCAPS_MAGFANISOTROPIC |
                                     D3DPTFILTERCAPS_MINFANISOTROPIC);
            getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureFilterCaps &= dwAnisoCapMask;
            getDC()->nvD3DDevCaps.dwD3DCap8.TextureFilterCaps            &= dwAnisoCapMask;
            getDC()->nvD3DDevCaps.dwD3DCap8.CubeTextureFilterCaps        &= dwAnisoCapMask;
            getDC()->nvD3DDevCaps.dwD3DCap8.VolumeTextureFilterCaps      &= dwAnisoCapMask;
            getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwRasterCaps &= ~D3DPRASTERCAPS_ANISOTROPY;
            getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwRasterCaps  &= ~D3DPRASTERCAPS_ANISOTROPY;
            getDC()->nvD3DDevCaps.dwD3DCap8.RasterCaps             &= ~D3DPRASTERCAPS_ANISOTROPY;
            getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxAnisotropy = 0;
            getDC()->nvD3DDevCaps.dwD3DCap8.MaxAnisotropy    = 0;
        }

    }

    else
#endif // (NVARCH >= 0x20)

#if (NVARCH >= 0x10)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {

        // D3DDEVICEDESC_V1 caps

        memset (&(getDC()->nvD3DDevCaps.dd1Caps), 0, sizeof(D3DDEVICEDESC_V1));

        getDC()->nvD3DDevCaps.dd1Caps.dwSize                            = sizeof(D3DDEVICEDESC_V1);
        getDC()->nvD3DDevCaps.dd1Caps.dwFlags                           = CELSIUS_DEVDESC_FLAGS;
        getDC()->nvD3DDevCaps.dd1Caps.dcmColorModel                     = CELSIUS_COLORMODEL;
        getDC()->nvD3DDevCaps.dd1Caps.dwDevCaps                         = CELSIUS_DEVCAPS_FLAGS;

        getDC()->nvD3DDevCaps.dd1Caps.dtcTransformCaps.dwSize           = sizeof(D3DTRANSFORMCAPS);
        getDC()->nvD3DDevCaps.dd1Caps.dtcTransformCaps.dwCaps           = CELSIUS_TRANSFORMCAPS_FLAGS;

        getDC()->nvD3DDevCaps.dd1Caps.bClipping                         = CELSIUS_3DCLIPPINGCAPS;

        getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwSize            = sizeof(D3DLIGHTINGCAPS);
        getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwCaps            = CELSIUS_LIGHTINGCAPS_FLAGS;
        getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwLightingModel   = D3DLIGHTINGMODEL_RGB;
        getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwNumLights       = CELSIUS_MAX_LIGHTS;

        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwSize                = sizeof(D3DPRIMCAPS);
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwMiscCaps            = CELSIUS_LINECAPS_MISC;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwRasterCaps          = CELSIUS_LINECAPS_RASTER;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwZCmpCaps            = CELSIUS_LINECAPS_ZCMP;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwSrcBlendCaps        = CELSIUS_LINECAPS_SRCBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwDestBlendCaps       = CELSIUS_LINECAPS_DESTBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwAlphaCmpCaps        = CELSIUS_LINECAPS_ALPHACMP;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwShadeCaps           = CELSIUS_LINECAPS_SHADE;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureCaps         = CELSIUS_LINECAPS_TEXTURE;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureFilterCaps   = CELSIUS_LINECAPS_TEXTUREFILTER;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureBlendCaps    = CELSIUS_LINECAPS_TEXTUREBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureAddressCaps  = CELSIUS_LINECAPS_TEXTUREADDRESS;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwStippleWidth        = CELSIUS_LINECAPS_STIPPLEWIDTH;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwStippleHeight       = CELSIUS_LINECAPS_STIPPLEHEIGHT;

        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwSize                 = sizeof(D3DPRIMCAPS);
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwMiscCaps             = CELSIUS_TRICAPS_MISC;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwRasterCaps           = CELSIUS_TRICAPS_RASTER;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwZCmpCaps             = CELSIUS_TRICAPS_ZCMP;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwSrcBlendCaps         = CELSIUS_TRICAPS_SRCBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwDestBlendCaps        = CELSIUS_TRICAPS_DESTBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwAlphaCmpCaps         = CELSIUS_TRICAPS_ALPHACMP;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwShadeCaps            = CELSIUS_TRICAPS_SHADE;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureCaps          = CELSIUS_TRICAPS_TEXTURE;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureFilterCaps    = CELSIUS_TRICAPS_TEXTUREFILTER;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureBlendCaps     = CELSIUS_TRICAPS_TEXTUREBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureAddressCaps   = CELSIUS_TRICAPS_TEXTUREADDRESS;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwStippleWidth         = CELSIUS_TRICAPS_STIPPLEWIDTH;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwStippleHeight        = CELSIUS_TRICAPS_STIPPLEHEIGHT;

        getDC()->nvD3DDevCaps.dd1Caps.dwDeviceRenderBitDepth            = CELSIUS_RENDERBITDEPTHCAPS;
        getDC()->nvD3DDevCaps.dd1Caps.dwDeviceZBufferBitDepth           = CELSIUS_ZBUFFERBITDEPTHCAPS;
        getDC()->nvD3DDevCaps.dd1Caps.dwMaxBufferSize                   = CELSIUS_MAXBUFFERSIZE;
        getDC()->nvD3DDevCaps.dd1Caps.dwMaxVertexCount                  = CELSIUS_MAXVERTEXCOUNT;

        // D3DHAL_D3DEXTENDEDCAPS

        memset (&(getDC()->nvD3DDevCaps.d3dExtCaps), 0, sizeof(D3DHAL_D3DEXTENDEDCAPS));

        getDC()->nvD3DDevCaps.d3dExtCaps.dwSize                         = 0;  // to be set later in getDriverInfo
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMinTextureWidth              = 1;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxTextureWidth              = pDriverData->nvD3DPerfData.dwMaxTextureSize;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMinTextureHeight             = 1;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxTextureHeight             = pDriverData->nvD3DPerfData.dwMaxTextureSize;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMinStippleWidth              = 0;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxStippleWidth              = 0;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMinStippleHeight             = 0;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxStippleHeight             = 0;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxTextureRepeat             = CELSIUS_CAPS_MAX_TEXTURE_REPEAT;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxTextureAspectRatio        = CELSIUS_CAPS_MAX_TEXTURE_ASPECT_RATIO;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxAnisotropy                = CELSIUS_CAPS_MAX_ANISOTROPY;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvGuardBandLeft                = CELSIUS_CAPS_GUARD_BAND_LEFT;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvGuardBandTop                 = CELSIUS_CAPS_GUARD_BAND_TOP;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvGuardBandRight               = CELSIUS_CAPS_GUARD_BAND_RIGHT;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvGuardBandBottom              = CELSIUS_CAPS_GUARD_BAND_BOTTOM;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvExtentsAdjust                = CELSIUS_CAPS_EXTENTS_ADJUST;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwStencilCaps                  = CELSIUS_CAPS_STENCILOPS;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwFVFCaps                      = CELSIUS_CAPS_FVF_CAPS
                                                                            | D3DFVFCAPS_DONOTSTRIPELEMENTS; // bdw: leave here
        getDC()->nvD3DDevCaps.d3dExtCaps.dwTextureOpCaps                = CELSIUS_CAPS_TEXTUREOPS;
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxTextureBlendStages         = CELSIUS_CAPS_MAX_TEXTURE_BLEND_STATES;
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxSimultaneousTextures       = CELSIUS_CAPS_MAX_SIMULTANEOUS_TEXTURES;

        // BUBGUG still need some data here
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxActiveLights              = CELSIUS_CAPS_MAX_ACTIVE_LIGHTS;
        // getDC()->nvD3DDevCaps.d3dExtCaps.dvMaxVertexW                = ??;
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxUserClipPlanes             = CELSIUS_CAPS_MAX_USER_CLIP_PLANES;
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxVertexBlendMatrices        = CELSIUS_CAPS_MAX_VERTEX_BLEND_MATRICES;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwVertexProcessingCaps         = CELSIUS_CAPS_VTXPCAPS;

        // HMH need to get rid of constants
        LPDDHALINFO pHalInfo = GET_HALINFO();

        //getDC()->nvD3DDevCaps.dwD3DCap8.DeviceType = ??
        getDC()->nvD3DDevCaps.dwD3DCap8.AdapterOrdinal = 0;

        /* Filled in by run-time */
        getDC()->nvD3DDevCaps.dwD3DCap8.Caps                            =  pHalInfo->ddCaps.dwCaps;
        getDC()->nvD3DDevCaps.dwD3DCap8.Caps2                           = pHalInfo->ddCaps.dwCaps2;
        getDC()->nvD3DDevCaps.dwD3DCap8.Caps3                           = 0;
        getDC()->nvD3DDevCaps.dwD3DCap8.PresentationIntervals           = 0;

        getDC()->nvD3DDevCaps.dwD3DCap8.CursorCaps                      = 0;
        /* END Filled in by run-time */
        getDC()->nvD3DDevCaps.dwD3DCap8.DevCaps                         = CELSIUS_DEVCAPS_FLAGS;

        getDC()->nvD3DDevCaps.dwD3DCap8.PrimitiveMiscCaps               = CELSIUS_TRICAPS_MISC;
        getDC()->nvD3DDevCaps.dwD3DCap8.RasterCaps                      = CELSIUS_TRICAPS_RASTER;
        getDC()->nvD3DDevCaps.dwD3DCap8.ZCmpCaps                        = CELSIUS_TRICAPS_ZCMP;
        getDC()->nvD3DDevCaps.dwD3DCap8.SrcBlendCaps                    = CELSIUS_TRICAPS_SRCBLEND;
        getDC()->nvD3DDevCaps.dwD3DCap8.DestBlendCaps                   = CELSIUS_TRICAPS_DESTBLEND;
        getDC()->nvD3DDevCaps.dwD3DCap8.AlphaCmpCaps                    = CELSIUS_TRICAPS_ALPHACMP;
        getDC()->nvD3DDevCaps.dwD3DCap8.ShadeCaps                       = CELSIUS_TRICAPS_SHADE;
        getDC()->nvD3DDevCaps.dwD3DCap8.TextureCaps                     = CELSIUS_TRICAPS_TEXTURE;

        getDC()->nvD3DDevCaps.dwD3DCap8.TextureFilterCaps               = CELSIUS_TRICAPS_TEXTUREFILTER;
        getDC()->nvD3DDevCaps.dwD3DCap8.CubeTextureFilterCaps           = CELSIUS_TRICAPS_TEXTUREFILTER;
        getDC()->nvD3DDevCaps.dwD3DCap8.VolumeTextureFilterCaps         = NULL;
        getDC()->nvD3DDevCaps.dwD3DCap8.TextureAddressCaps              = CELSIUS_TRICAPS_TEXTUREADDRESS;
        getDC()->nvD3DDevCaps.dwD3DCap8.VolumeTextureAddressCaps        = NULL;

        getDC()->nvD3DDevCaps.dwD3DCap8.LineCaps                        = D3DLINECAPS_TEXTURE     |
                                                                          D3DLINECAPS_ZTEST       |
                                                                          D3DLINECAPS_BLEND       |
                                                                          D3DLINECAPS_ALPHACMP    |
                                                                          D3DLINECAPS_FOG         ;

        getDC()->nvD3DDevCaps.dwD3DCap8.MaxTextureWidth                 = pDriverData->nvD3DPerfData.dwMaxTextureSize;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxTextureHeight                = pDriverData->nvD3DPerfData.dwMaxTextureSize;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxVolumeExtent                 = pDriverData->nvD3DPerfData.dwMaxVolumeTextureSize;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxTextureRepeat                = CELSIUS_CAPS_MAX_TEXTURE_REPEAT;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxTextureAspectRatio           = CELSIUS_CAPS_MAX_TEXTURE_ASPECT_RATIO;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxAnisotropy                   = CELSIUS_CAPS_MAX_ANISOTROPY;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxVertexW                      = 1.0e10; // as per refrast

        getDC()->nvD3DDevCaps.dwD3DCap8.GuardBandLeft                   = CELSIUS_CAPS_GUARD_BAND_LEFT;
        getDC()->nvD3DDevCaps.dwD3DCap8.GuardBandTop                    = CELSIUS_CAPS_GUARD_BAND_TOP;
        getDC()->nvD3DDevCaps.dwD3DCap8.GuardBandRight                  = CELSIUS_CAPS_GUARD_BAND_RIGHT;
        getDC()->nvD3DDevCaps.dwD3DCap8.GuardBandBottom                 = CELSIUS_CAPS_GUARD_BAND_BOTTOM;
        getDC()->nvD3DDevCaps.dwD3DCap8.ExtentsAdjust                   = CELSIUS_CAPS_EXTENTS_ADJUST;

        getDC()->nvD3DDevCaps.dwD3DCap8.StencilCaps                     = CELSIUS_CAPS_STENCILOPS;
        getDC()->nvD3DDevCaps.dwD3DCap8.FVFCaps                         = CELSIUS_CAPS_FVF_CAPS
                                                                        | D3DFVFCAPS_DONOTSTRIPELEMENTS; // bdw: leave here
        getDC()->nvD3DDevCaps.dwD3DCap8.TextureOpCaps                   = CELSIUS_CAPS_TEXTUREOPS;

        getDC()->nvD3DDevCaps.dwD3DCap8.MaxTextureBlendStages           = CELSIUS_CAPS_MAX_TEXTURE_BLEND_STATES;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxSimultaneousTextures         = CELSIUS_CAPS_MAX_SIMULTANEOUS_TEXTURES;

        getDC()->nvD3DDevCaps.dwD3DCap8.VertexProcessingCaps            = CELSIUS_CAPS_VTXPCAPS;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxActiveLights                 = CELSIUS_CAPS_MAX_ACTIVE_LIGHTS;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxUserClipPlanes               = CELSIUS_CAPS_MAX_USER_CLIP_PLANES;

        getDC()->nvD3DDevCaps.dwD3DCap8.MaxVertexBlendMatrices          = CELSIUS_CAPS_MAX_VERTEX_BLEND_MATRICES;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxVertexBlendMatrixIndex       = 0;  // no indexed (palettized) matrices

        getDC()->nvD3DDevCaps.dwD3DCap8.MaxPointSize                    = 64.0f;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxPrimitiveCount               = 0xffff;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxVertexIndex                  = 0xffff;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxStreams                      = CELSIUS_CAPS_MAX_STREAMS;
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxStreamStride                 = 256;
        getDC()->nvD3DDevCaps.dwD3DCap8.VertexShaderVersion             = D3DVS_VERSION(0, 0);
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxVertexShaderConst            = CELSIUS_CAPS_MAX_VSHADER_CONSTS;
        getDC()->nvD3DDevCaps.dwD3DCap8.PixelShaderVersion              = D3DPS_VERSION(0, 0);
        getDC()->nvD3DDevCaps.dwD3DCap8.MaxPixelShaderValue             = 0.0f;

    }

    else
#endif // (NVARCH >= 0x10)

    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_DXTRI) {

        // D3DDEVICEDESC_V1 caps

        memset (&(getDC()->nvD3DDevCaps.dd1Caps), 0, sizeof(D3DDEVICEDESC_V1));

        getDC()->nvD3DDevCaps.dd1Caps.dwSize                            = sizeof(D3DDEVICEDESC_V1);
        getDC()->nvD3DDevCaps.dd1Caps.dwFlags                           = NV4_DEVDESC_FLAGS;
        getDC()->nvD3DDevCaps.dd1Caps.dcmColorModel                     = NV4_COLORMODEL;
        getDC()->nvD3DDevCaps.dd1Caps.dwDevCaps                         = NV4_DEVCAPS_FLAGS;

        getDC()->nvD3DDevCaps.dd1Caps.dtcTransformCaps.dwSize           = sizeof(D3DTRANSFORMCAPS);
        getDC()->nvD3DDevCaps.dd1Caps.dtcTransformCaps.dwCaps           = NV4_TRANSFORMCAPS_FLAGS;

        getDC()->nvD3DDevCaps.dd1Caps.bClipping                         = NV4_3DCLIPPINGCAPS;

        getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwSize            = sizeof(D3DLIGHTINGCAPS);
        getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwCaps            = NV4_LIGHTINGCAPS_FLAGS;
        getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwLightingModel   = D3DLIGHTINGMODEL_RGB;
        getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwNumLights       = NV4_MAX_LIGHTS;

        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwSize                = sizeof(D3DPRIMCAPS);
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwMiscCaps            = NV4_LINECAPS_MISC;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwRasterCaps          = NV4_LINECAPS_RASTER;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwZCmpCaps            = NV4_LINECAPS_ZCMP;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwSrcBlendCaps        = NV4_LINECAPS_SRCBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwDestBlendCaps       = NV4_LINECAPS_DESTBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwAlphaCmpCaps        = NV4_LINECAPS_ALPHACMP;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwShadeCaps           = NV4_LINECAPS_SHADE;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureCaps         = NV4_LINECAPS_TEXTURE;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureFilterCaps   = NV4_LINECAPS_TEXTUREFILTER;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureBlendCaps    = NV4_LINECAPS_TEXTUREBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwTextureAddressCaps  = NV4_LINECAPS_TEXTUREADDRESS;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwStippleWidth        = NV4_LINECAPS_STIPPLEWIDTH;
        getDC()->nvD3DDevCaps.dd1Caps.dpcLineCaps.dwStippleHeight       = NV4_LINECAPS_STIPPLEHEIGHT;

        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwSize                 = sizeof(D3DPRIMCAPS);
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwMiscCaps             = NV4_TRICAPS_MISC;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwRasterCaps           = NV4_TRICAPS_RASTER;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwZCmpCaps             = NV4_TRICAPS_ZCMP;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwSrcBlendCaps         = NV4_TRICAPS_SRCBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwDestBlendCaps        = NV4_TRICAPS_DESTBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwAlphaCmpCaps         = NV4_TRICAPS_ALPHACMP;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwShadeCaps            = NV4_TRICAPS_SHADE;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureCaps          = NV4_TRICAPS_TEXTURE;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureFilterCaps    = NV4_TRICAPS_TEXTUREFILTER;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureBlendCaps     = NV4_TRICAPS_TEXTUREBLEND;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwTextureAddressCaps   = NV4_TRICAPS_TEXTUREADDRESS;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwStippleWidth         = NV4_TRICAPS_STIPPLEWIDTH;
        getDC()->nvD3DDevCaps.dd1Caps.dpcTriCaps.dwStippleHeight        = NV4_TRICAPS_STIPPLEHEIGHT;

        getDC()->nvD3DDevCaps.dd1Caps.dwDeviceRenderBitDepth            = NV4_RENDERBITDEPTHCAPS;
        getDC()->nvD3DDevCaps.dd1Caps.dwDeviceZBufferBitDepth           = NV4_ZBUFFERBITDEPTHCAPS;
        getDC()->nvD3DDevCaps.dd1Caps.dwMaxBufferSize                   = NV4_MAXBUFFERSIZE;
        getDC()->nvD3DDevCaps.dd1Caps.dwMaxVertexCount                  = NV4_MAXVERTEXCOUNT;

        // D3DHAL_D3DEXTENDEDCAPS

        memset (&(getDC()->nvD3DDevCaps.d3dExtCaps), 0, sizeof(D3DHAL_D3DEXTENDEDCAPS));

        getDC()->nvD3DDevCaps.d3dExtCaps.dwSize                         = 0;  // to be set later in getDriverInfo
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMinTextureWidth              = 1;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxTextureWidth              = pDriverData->nvD3DPerfData.dwMaxTextureSize;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMinTextureHeight             = 1;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxTextureHeight             = pDriverData->nvD3DPerfData.dwMaxTextureSize;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMinStippleWidth              = 0;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxStippleWidth              = 0;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMinStippleHeight             = 0;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxStippleHeight             = 0;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxTextureRepeat             = NV4_CAPS_MAX_TEXTURE_REPEAT;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxTextureAspectRatio        = NV4_CAPS_MAX_TEXTURE_ASPECT_RATIO;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxAnisotropy                = NV4_CAPS_MAX_ANISOTROPY;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvGuardBandLeft                = NV4_CAPS_GUARD_BAND_LEFT;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvGuardBandTop                 = NV4_CAPS_GUARD_BAND_TOP;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvGuardBandRight               = NV4_CAPS_GUARD_BAND_RIGHT;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvGuardBandBottom              = NV4_CAPS_GUARD_BAND_BOTTOM;
        getDC()->nvD3DDevCaps.d3dExtCaps.dvExtentsAdjust                = NV4_CAPS_EXTENTS_ADJUST;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwStencilCaps                  = NV4_CAPS_STENCILOPS;
        getDC()->nvD3DDevCaps.d3dExtCaps.dwFVFCaps                      = NV4_CAPS_FVF_CAPS
                                                                            | D3DFVFCAPS_DONOTSTRIPELEMENTS; // bdw: leave here
        getDC()->nvD3DDevCaps.d3dExtCaps.dwTextureOpCaps                = NV4_CAPS_TEXTUREOPS;
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxTextureBlendStages         = NV4_CAPS_MAX_TEXTURE_BLEND_STATES;
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxSimultaneousTextures       = NV4_CAPS_MAX_SIMULTANEOUS_TEXTURES;

        // BUBGUG still need some data here
        getDC()->nvD3DDevCaps.d3dExtCaps.dwMaxActiveLights              = NV4_CAPS_MAX_ACTIVE_LIGHTS;
        // getDC()->nvD3DDevCaps.d3dExtCaps.dvMaxVertexW                = ??;
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxUserClipPlanes             = NV4_CAPS_MAX_USER_CLIP_PLANES;
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxVertexBlendMatrices        = NV4_CAPS_MAX_VERTEX_BLEND_MATRICES;
#ifndef WINNT
        getDC()->nvD3DDevCaps.d3dExtCaps.dwVertexProcessingCaps         = NV4_CAPS_VTXPCAPS;
#endif // WINNT

    }

    else {

        // unhandled hardware
        DPF ("unknown hardware in nvSetHardwareCaps");
        dbgD3DError();

    }

    dbgTracePop();
}

#endif // NVARCH >= 0x04

