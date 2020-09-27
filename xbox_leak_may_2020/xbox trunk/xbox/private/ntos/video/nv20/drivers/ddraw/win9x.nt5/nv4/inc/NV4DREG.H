#ifndef _NV4DREG_H_
#define _NV4DREG_H_
#ifdef  NV4
/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4DREG.H                                                         *
*   Define the D3D driver registry strings and default settings.            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/98 - Created.                     *
*                                                                           *
\***************************************************************************/

#define REG_BIT1_FOG_TABLE_ENABLE           0x00000001
#define REG_BIT1_ANTI_ALIAS_ENABLE          0x00000002
#define REG_BIT1_USER_MIPMAPS_ENABLE        0x00000004
#define REG_BIT1_VIDEO_TEXTURE_ENABLE       0x00000008
#define REG_BIT1_DMA_PUSH_SIZE_OVERRIDE     0x00000010
#define REG_BIT1_DMA_PUSH_COUNT_OVERRIDE    0x00000020
#define REG_BIT1_MIPMAP_DITHER_ENABLE       0x00000040
#define REG_BIT1_NO_WAIT_4_VSYNC            0x00000080
#define REG_BIT1_RESTRICT_AUTO_MIPMAPS      0x00000100
#define REG_BIT1_CKCOMPATABILITY_ENABLE     0x00000200
#define REG_BIT1_CONTROLTRAFFIC             0x00000400

/*
 * Value defines for some registry settings.
 * These are the values that are actually set in the registry.
 */
/*
 * FogTableEnable definitioans.
 */
#define REG_FTE_DISABLE                     0           // Disable Fog Table support.
#define REG_FTE_ENABLE                      1           // Enable Fog Table support
#define REG_FTE_MAX                         1

/*
 * AntiAliasEnable definitioans.
 */
#define REG_AAE_DISABLE                     0           // Disable Anti-Aliasing support.
#define REG_AAE_ENABLE                      1           // Enable Anti-Aliasing support.
#define REG_AAE_MAX                         1

/*
 * UserMipmaps definitioans.
 */
#define REG_UMM_DISABLE                     0           // Disable User supplied mipmaps.
#define REG_UMM_ENABLE                      1           // Enable User supplied mipmaps.
#define REG_UMM_MAX                         1

/*
 * VideoTexture definitioans.
 */
#define REG_VT_DISABLE                      0           // Disable texturing from video memory.
#define REG_VT_ENABLE                       1           // Enable texturing from video memory.
#define REG_VT_MAX                          1

/*
 * MipmapLevels definitioans.
 */
#define REG_MML_DISABLE                     0           // Disable Auto generated mipmaps.
#define REG_MML_ENABLE                      12          // Enable Auto generated mipmaps.
#define REG_MML_RESTRICTED                  13          // Restrict auto generation to non-colorkey/non-alpha textures.
#define REG_MML_MAX                         13

/*
 * AutoMipMapMethod definitions
 */
#define REG_AMMM_BILINEAR                   0
#define REG_AMMM_TRILINEAR                  1
#define REG_AMMM_MAX                        1

/*
 * TexelAlignment definitions.
 * Bit flags (bit 0 = ZOH, bit 1 = FOH, bit 2 = Texel Origin)
 */
#define REG_TA_ZOH_CENTER                   0x00
#define REG_TA_ZOH_CORNER                   0x01
#define REG_TA_FOH_CENTER                   0x00
#define REG_TA_FOH_CORNER                   0x02
#define REG_TA_TEXEL_CENTER                 0x00
#define REG_TA_TEXEL_CORNER                 0x04
#define REG_TA_MAX                          0x07
#define REG_TA_ZOH_MASK                     0x01
#define REG_TA_FOH_MASK                     0x02
#define REG_TA_TEXEL_MASK                   0x04

/*
 * MipMapDitherEnable definitions.
 */
#define REG_MMDE_FALSE                      0           // Disable mipmap dithering (true trilinear)
#define REG_MMDE_TRUE                       1           // Enable mipmap dithering (fake trilinear)
#define REG_MMDE_MAX                        1

/*
 * Pal8TextureConvert definitions
 */
#define REG_P8TC_NONE                       0           // Don't support 8 bit palettized textures.
#define REG_P8TC_8TOR5G6B5                  1           // Convert 8 bit palettized textures to 16 bit R5G6B5 textures.
#define REG_P8TC_8TOX1R5G5B5                2           // Convert 8 bit palettized textures to 16 bit X1R5G5B5 textures.
#define REG_P8TC_8TOA1R5G5B5                3           // Convert 8 bit palettized textures to 16 bit A1R5G5B5 textures.
#define REG_P8TC_8TOX8R8G8B8                4           // Convert 8 bit palettized textures to 32 bit X8R8G8B8 textures.
#define REG_P8TC_8TOA8R8G8B8                5           // Convert 8 bit palettized textures to 32 bit A8R8G8B8 textures.
#define REG_P8TC_Y8ONLY                     6           // Support Luminance Textures only
#define REG_P8TC_MAX                        6

/*
 * DX6Enable definitions.
 */
#define REG_DX6E_DISABLE                    0           // Don't export DX6 callbacks (DX5 Equivalent)
#define REG_DX6E_OPTIMIZED                  1           // Enable optimal DX6 functionality.
#define REG_DX6E_FORCE_FVF                  2           // Force use of FVF inner loops for non-multi pass cases.
#define REG_DX6E_FORCE_MULTI                3           // Force use of DX6 Multi-Texture inner loops for all cases.
#define REG_DX6E_MAX                        3

/*
 * Prerender limits
 */
#define REG_PRERENDER_MIN                   1
#define REG_PRERENDER_MAX                   100

/*
 * AAMethod limits
 */
#define REG_AAMETHOD_MIN                    0
#define REG_AAMETHOD_2x1                    0
#define REG_AAMETHOD_2x2                    1
#define REG_AAMETHOD_3x2                    2
#define REG_AAMETHOD_3x3                    3
#define REG_AAMETHOD_4x2                    4
#define REG_AAMETHOD_4x3                    5
#define REG_AAMETHOD_4x4                    6
#define REG_AAMETHOD_MAX                    6

/*
 * ValidateZMethod definitions.
 */
#define REG_VZM_STRICT                      0           // Z-Buffer must always match rendering depth.
#define REG_VZM_MIXED                       1           // DX6 applications = STRICT, DX5 applications = flexible
#define REG_VZM_FLEXIBLE                    2           // Driver will match z-buffer to rendering depth.
#define REG_VZM_MAX                         2

/*
 * Colorkey Reference Values range.
 */
#define REG_CKREF_MIN                       0x00
#define REG_CKREF_MAX                       0x7F

/*
 * CKCompatabilityEnable definitions.
 */
#define REG_CKCE_FALSE                      0x00
#define REG_CKCE_TRUE                       0x01
#define REG_CKCE_MAX                        0x01

#define NV4D3D_APP_STRING                       "SOFTWARE\\NVIDIA Corporation\\RIVA TNT\\Direct3D"
/*
 * Driver registry strings.
 *
 * Enable/Disable settings.
 */
#define NVD3D_FOG_TABLE_STRING                  "FOGTABLEENABLE"
#define NVD3D_ANTI_ALIAS_ENABLE_STRING          "ANTIALIASENABLE"
#define NVD3D_USER_MIPMAPS_STRING               "USERMIPMAPS"
#define NVD3D_VIDEO_TEXTURE_STRING              "VIDEOTEXTURE"
#define NVD3D_NO_WAIT_4_VSYNC_STRING            "NOVSYNC"
#define NVD3D_TEXEL_ALIGNMENT_STRING            "TEXELALIGNMENT"
#define NVD3D_MIPMAP_DITHER_ENABLE_STRING       "MIPMAPDITHERENABLE"
#define NVD3D_CKCOMPATABILITY_ENABLE_STRING     "CKCOMPATABILITYENABLE"
#define NVD3D_CKREF_STRING                      "CKREF"
#define NVD3D_CONTROLTRAFFIC_STRING             "CONTROLTRAFFIC"

/*
 * Value defined settings.
 */
#define NVD3D_MIPMAP_LEVELS_STRING              "MIPMAPLEVELS"
#define NVD3D_AUTO_MIPMAP_METHOD_STRING         "AUTOMIPMAPMETHOD"
#define NVD3D_TEX_HEAP_STRING                   "TEXHEAP"
#define NVD3D_MIN_VIDEO_TEX_SIZE_STRING         "MINVIDEOTEXSIZE"
#define NVD3D_DMA_MIN_PUSH_COUNT_STRING         "DMAMINPUSHCOUNT"
#define NVD3D_DMA_PUSH_BUFFER_SIZE_MAX_STRING   "DMAPUSHBUFFERSIZEMAX"
#define NVD3D_CONTEXT_MAX_STRING                "D3DCONTEXTMAX"
#define NVD3D_TEXTURE_MAX_STRING                "D3DTEXTUREMAX"
#define NVD3D_LOD_BIAS_ADJUST_STRING            "LODBIASADJUST"
#define NVD3D_PAL8_TEXTURE_CONVERT_STRING       "PAL8TEXTURECONVERT"
#define NVD3D_DX6_ENABLE_STRING                 "DX6ENABLE"
#define NVD3D_PRERENDER_LIMIT_STRING            "NOVSYNCPRERENDERLIMIT"
#define NVD3D_AAMETHOD_STRING                   "AAMETHOD"
#define NVD3D_VALIDATE_Z_METHOD_STRING          "VALIDATEZMETHOD"
#define NVD3D_AGPTEXTURE_CUTOFF                 "AGPTEXCUTOFF"

/*
 * Default values for registry configurable driver settings.
 */
#define DEFAULT_FOG_TABLE               REG_BIT1_FOG_TABLE_ENABLE       // Enable software implemented fog table support.
#define DEFAULT_ANTI_ALIAS_ENABLE       0//REG_BIT1_ANTI_ALIAS_ENABLE      // Enable software implemented Anti Aliasing.
#define DEFAULT_USER_MIPMAPS            REG_BIT1_USER_MIPMAPS_ENABLE    // Enable use of user supplied mip maps.
#define DEFAULT_VIDEO_TEXTURE           REG_BIT1_VIDEO_TEXTURE_ENABLE   // Enable texturing from video memory.
#define DEFAULT_NO_WAIT_4_VSYNC         0                               // Enable Proper VSync & Flip
#define DEFAULT_TEXEL_ALIGNMENT         (REG_TA_ZOH_CORNER | REG_TA_FOH_CORNER | REG_TA_TEXEL_CENTER)
#define DEFAULT_MIPMAP_DITHER_ENABLE    0                               //
#define DEFAULT_PRERENDER_LIMIT         5
#define DEFAULT_CKCOMPATABILITY_ENABLE  REG_BIT1_CKCOMPATABILITY_ENABLE // Perform application colorkey fixups.
#define DEFAULT_CONTROLTRAFFIC          0

/*
 * The definitions for ZOH and ZOH_MS are reversed.
 * The way they are implemented in hardware is actually:
 * ZOH_MS = 0
 * ZOH    = 1
 * The mode we really want to use by default is ZOH_MS.
 */
#define DEFAULT_MIPMAP_LEVELS           REG_MML_DISABLE                 // Set max number of mip map levels to auto-generate.
#define DEFAULT_AUTO_MIPMAP_METHOD      REG_AMMM_BILINEAR               // Bilinearly filter auto-generated mipmaps.
#define DEFAULT_TEX_HEAP                0x00500000                      // Set maximum size of PCI system memory texture heap.
#define DEFAULT_MIN_VIDEO_TEX_SIZE      1                               // Set minimum texture size to be placed in video memory.
#define DEFAULT_DMA_MIN_PUSH_COUNT_PCI  0x600                           // Set the minimum push count.
#define DEFAULT_DMA_MIN_PUSH_COUNT_AGP  0x900                           // Set the minimum push count.
#define DEFAULT_PUSH_BUFFER_SIZE_MAX_PCI 0x40000                        // Set the maximum push buffer size.
#define DEFAULT_PUSH_BUFFER_SIZE_MAX_AGP 0x200000                       // Set the maximum push buffer size.
#define DEFAULT_D3D_CONTEXT_MAX         64                              // D3D Context heap allocation max.
#define DEFAULT_D3D_TEXTURE_MAX         1024                            // D3D Texture heap allocation max.
#define DEFAULT_LOD_BIAS_ADJUST         0                               //
#define DEFAULT_PAL8_TEXTURE_CONVERT    REG_P8TC_NONE                   //
#define DEFAULT_AAMETHOD                REG_AAMETHOD_2x2                // 2x2 Supersampling.
#define DEFAULT_VALIDATE_Z_METHOD       REG_VZM_FLEXIBLE                // REG_VZM_MIXED
#define DEFAULT_CKREF                   REG_CKREF_MIN
#define DEFAULT_AGPTEX_CUTOFF           1024

/*
 * This will eventually go away.
 */
#define DEFAULT_DX6_ENABLE              REG_DX6E_OPTIMIZED
#endif  // NV4
#endif _NV4DREG_H_
