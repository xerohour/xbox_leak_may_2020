/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVTEXFMT.H                                                        *
*   Define the texture format available for the NV D3D driver.              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/98 - created.                     *
*                                                                           *
\***************************************************************************/

#ifndef _NVTEXFMT_H_
#define _NVTEXFMT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Define the various texture format definitions.
 * For any given hardware, the texture format definitions don't change,
 * just the formats that are available on that hardware change.
 */
/*
 * RGB 16 BPP (R5xG5xB5)
 */
#define NV_TFMT_X1R5G5B5                                                            \
{                                                                                   \
    sizeof(DDSURFACEDESC),                  /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,           /* dwFlags */                           \
    0,                                      /* dwHeight */                          \
    0,                                      /* dwWidth */                           \
    0,                                      /* lPitch */                            \
    0,                                      /* dwBackBufferCount */                 \
    0,                                      /* dwZBufferBitDepth */                 \
    0,                                      /* dwAlphaBitDepth */                   \
    0,                                      /* dwReserved */                        \
    NULL,                                   /* lpSurface */                         \
    { 0, 0 },                               /* ddckCKDestOverlay */                 \
    { 0, 0 },                               /* ddckCKDestBlt */                     \
    { 0, 0 },                               /* ddckCKSrcOverlay */                  \
    { 0, 0 },                               /* ddckCKSrcBlt */                      \
    {                                                                               \
      sizeof(DDPIXELFORMAT),                /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB,                             /* ddpfPixelFormat.dwFlags */           \
      0,                                    /* FOURCC code */                       \
      16,                                   /* ddpfPixelFormat.dwRGBBitCount */     \
      0x7C00,                                                                       \
      0x03E0,                                                                       \
      0x001F,                                                                       \
      0x0000                                                                        \
    },                                                                              \
    DDSCAPS_TEXTURE                         /* ddscaps.dwCaps */                    \
}

/*
 * RGB 16 BPP Alpha (A1xR5xG5xB5)
 */
#define NV_TFMT_A1R5G5B5                                                            \
{                                                                                  \
    sizeof(DDSURFACEDESC),                  /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,           /* dwFlags */                           \
    0,                                      /* dwHeight */                          \
    0,                                      /* dwWidth */                           \
    0,                                      /* lPitch */                            \
    0,                                      /* dwBackBufferCount */                 \
    0,                                      /* dwZBufferBitDepth */                 \
    0,                                      /* dwAlphaBitDepth */                   \
    0,                                      /* dwReserved */                        \
    NULL,                                   /* lpSurface */                         \
    { 0, 0 },                               /* ddckCKDestOverlay */                 \
    { 0, 0 },                               /* ddckCKDestBlt */                     \
    { 0, 0 },                               /* ddckCKSrcOverlay */                  \
    { 0, 0 },                               /* ddckCKSrcBlt */                      \
    {                                                                               \
      sizeof(DDPIXELFORMAT),                /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB | DDPF_ALPHAPIXELS,          /* ddpfPixelFormat.dwFlags */           \
      0,                                    /* FOURCC code */                       \
      16,                                   /* ddpfPixelFormat.dwRGBBitCount */     \
      0x7C00,                                                                       \
      0x03E0,                                                                       \
      0x001F,                                                                       \
      0x8000                                                                        \
    },                                                                              \
    DDSCAPS_TEXTURE                         /* ddscaps.dwCaps */                    \
}

/*
 * RGB 16 BPP Alpha (A4xR4xG4xB4)
 */
#define NV_TFMT_A4R4G4B4                                                            \
{                                                                                   \
    sizeof(DDSURFACEDESC),                  /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,           /* dwFlags */                           \
    0,                                      /* dwHeight */                          \
    0,                                      /* dwWidth */                           \
    0,                                      /* lPitch */                            \
    0,                                      /* dwBackBufferCount */                 \
    0,                                      /* dwZBufferBitDepth */                 \
    0,                                      /* dwAlphaBitDepth */                   \
    0,                                      /* dwReserved */                        \
    NULL,                                   /* lpSurface */                         \
    { 0, 0 },                               /* ddckCKDestOverlay */                 \
    { 0, 0 },                               /* ddckCKDestBlt */                     \
    { 0, 0 },                               /* ddckCKSrcOverlay */                  \
    { 0, 0 },                               /* ddckCKSrcBlt */                      \
    {                                                                               \
      sizeof(DDPIXELFORMAT),                /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB | DDPF_ALPHAPIXELS,          /* ddpfPixelFormat.dwFlags */           \
      0,                                    /* FOURCC code */                       \
      16,                                   /* ddpfPixelFormat.dwRGBBitCount */     \
      0x0F00,                                                                       \
      0x00F0,                                                                       \
      0x000F,                                                                       \
      0xF000                                                                        \
    },                                                                              \
    DDSCAPS_TEXTURE                         /* ddscaps.dwCaps */                    \
}

/*
 * RGB 16 BPP (R5xG6xB5)
 */
#define NV_TFMT_R5G6B5                                                              \
{                                                                                   \
    sizeof(DDSURFACEDESC),                  /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,           /* dwFlags */                           \
    0,                                      /* dwHeight */                          \
    0,                                      /* dwWidth */                           \
    0,                                      /* lPitch */                            \
    0,                                      /* dwBackBufferCount */                 \
    0,                                      /* dwZBufferBitDepth */                 \
    0,                                      /* dwAlphaBitDepth */                   \
    0,                                      /* dwReserved */                        \
    NULL,                                   /* lpSurface */                         \
    { 0, 0 },                               /* ddckCKDestOverlay */                 \
    { 0, 0 },                               /* ddckCKDestBlt */                     \
    { 0, 0 },                               /* ddckCKSrcOverlay */                  \
    { 0, 0 },                               /* ddckCKSrcBlt */                      \
    {                                                                               \
      sizeof(DDPIXELFORMAT),                /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB,                             /* ddpfPixelFormat.dwFlags */           \
      0,                                    /* FOURCC code */                       \
      16,                                   /* ddpfPixelFormat.dwRGBBitCount */     \
      0xF800,                                                                       \
      0x07E0,                                                                       \
      0x001F,                                                                       \
      0x0000                                                                        \
    },                                                                              \
    DDSCAPS_TEXTURE                         /* ddscaps.dwCaps */                    \
}

/*
 * RGB 32 BPP (X8xR8xG8xB8)
 */
#define NV_TFMT_X8R8G8B8                                                            \
{                                                                                   \
    sizeof(DDSURFACEDESC),                  /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,           /* dwFlags */                           \
    0,                                      /* dwHeight */                          \
    0,                                      /* dwWidth */                           \
    0,                                      /* lPitch */                            \
    0,                                      /* dwBackBufferCount */                 \
    0,                                      /* dwZBufferBitDepth */                 \
    0,                                      /* dwAlphaBitDepth */                   \
    0,                                      /* dwReserved */                        \
    NULL,                                   /* lpSurface */                         \
    { 0, 0 },                               /* ddckCKDestOverlay */                 \
    { 0, 0 },                               /* ddckCKDestBlt */                     \
    { 0, 0 },                               /* ddckCKSrcOverlay */                  \
    { 0, 0 },                               /* ddckCKSrcBlt */                      \
    {                                                                               \
      sizeof(DDPIXELFORMAT),                /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB,                             /* ddpfPixelFormat.dwFlags */           \
      0,                                    /* FOURCC code */                       \
      32,                                   /* ddpfPixelFormat.dwRGBBitCount */     \
      0x00FF0000,                           /* Red Component Mask */                \
      0x0000FF00,                           /* Green Component Mask */              \
      0x000000FF,                           /* Blue Component Mask */               \
      0x00000000                            /* Alpha Component Mask */              \
    },                                                                              \
    DDSCAPS_TEXTURE                         /* ddscaps.dwCaps */                    \
}

/*
 * RGB 32 BPP (A8xR8xG8xB8)
 */
#define NV_TFMT_A8R8G8B8                                                            \
{                                                                                   \
    sizeof(DDSURFACEDESC),                  /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,           /* dwFlags */                           \
    0,                                      /* dwHeight */                          \
    0,                                      /* dwWidth */                           \
    0,                                      /* lPitch */                            \
    0,                                      /* dwBackBufferCount */                 \
    0,                                      /* dwZBufferBitDepth */                 \
    0,                                      /* dwAlphaBitDepth */                   \
    0,                                      /* dwReserved */                        \
    NULL,                                   /* lpSurface */                         \
    { 0, 0 },                               /* ddckCKDestOverlay */                 \
    { 0, 0 },                               /* ddckCKDestBlt */                     \
    { 0, 0 },                               /* ddckCKSrcOverlay */                  \
    { 0, 0 },                               /* ddckCKSrcBlt */                      \
    {                                                                               \
      sizeof(DDPIXELFORMAT),                /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB | DDPF_ALPHAPIXELS,          /* ddpfPixelFormat.dwFlags */           \
      0,                                    /* FOURCC code */                       \
      32,                                   /* ddpfPixelFormat.dwRGBBitCount */     \
      0x00FF0000,                           /* Red Component Mask */                \
      0x0000FF00,                           /* Green Component Mask */              \
      0x000000FF,                           /* Blue Component Mask */               \
      0xFF000000                            /* Alpha Component Mask */              \
    },                                                                              \
    DDSCAPS_TEXTURE                         /* ddscaps.dwCaps */                    \
}

/*
 * RGB 24 BPP (R8xG8xB8)
 * Hardware doesn't actually support this.  The texture would actually
 * have to be expanded to a 32bpp X8xR8xG8xB8 format.
 * Haven't decided if I really want to do this.
 */
#define NV_TFMT_R8G8B8                                                              \
{                                                                                   \
    sizeof(DDSURFACEDESC),                  /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,           /* dwFlags */                           \
    0,                                      /* dwHeight */                          \
    0,                                      /* dwWidth */                           \
    0,                                      /* lPitch */                            \
    0,                                      /* dwBackBufferCount */                 \
    0,                                      /* dwZBufferBitDepth */                 \
    0,                                      /* dwAlphaBitDepth */                   \
    0,                                      /* dwReserved */                        \
    NULL,                                   /* lpSurface */                         \
    { 0, 0 },                               /* ddckCKDestOverlay */                 \
    { 0, 0 },                               /* ddckCKDestBlt */                     \
    { 0, 0 },                               /* ddckCKSrcOverlay */                  \
    { 0, 0 },                               /* ddckCKSrcBlt */                      \
    {                                                                               \
      sizeof(DDPIXELFORMAT),                /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB,                             /* ddpfPixelFormat.dwFlags */           \
      0,                                    /* FOURCC code */                       \
      24,                                   /* ddpfPixelFormat.dwRGBBitCount */     \
      0x00FF0000,                           /* Red Component Mask */                \
      0x0000FF00,                           /* Green Component Mask */              \
      0x000000FF,                           /* Blue Component Mask */               \
      0x00000000                            /* Alpha Component Mask */              \
    },                                                                              \
    DDSCAPS_TEXTURE                         /* ddscaps.dwCaps */                    \
}

/*
 * FFVII REQUIRES 8bit textures.  What a pain.  Of course so does PC98/PC99.
 */
#define NV_TFMT_RGB8                                                                \
{                                                                                   \
    sizeof(DDSURFACEDESC),                  /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,           /* dwFlags */                           \
    0,                                      /* dwHeight */                          \
    0,                                      /* dwWidth */                           \
    0,                                      /* lPitch */                            \
    0,                                      /* dwBackBufferCount */                 \
    0,                                      /* dwZBufferBitDepth */                 \
    0,                                      /* dwAlphaBitDepth */                   \
    0,                                      /* dwReserved */                        \
    NULL,                                   /* lpSurface */                         \
    { 0, 0 },                               /* ddckCKDestOverlay */                 \
    { 0, 0 },                               /* ddckCKDestBlt */                     \
    { 0, 0 },                               /* ddckCKSrcOverlay */                  \
    { 0, 0 },                               /* ddckCKSrcBlt */                      \
    {                                                                               \
      sizeof(DDPIXELFORMAT),                /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB | DDPF_PALETTEINDEXED8,      /* ddpfPixelFormat.dwFlags */           \
      0,                                    /* FOURCC code */                       \
      8,                                    /* ddpfPixelFormat.dwRGBBitCount */     \
      0x00000000,                           /* Red Component Mask */                \
      0x00000000,                           /* Green Component Mask */              \
      0x00000000,                           /* Blue Component Mask */               \
      0x00000000                            /* Alpha Component Mask */              \
    },                                                                              \
    DDSCAPS_TEXTURE                         /* ddscaps.dwCaps */                    \
}

/*
 * 16-bit bumpmap
 * dU occupies the LSByte, dV occupies the MSByte
 */
#define NV_TFMT_BUMPMAP_DV8_DU8                                                     \
{                                                                                   \
    sizeof(DDSURFACEDESC),                  /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,           /* dwFlags */                           \
    0,                                      /* dwHeight */                          \
    0,                                      /* dwWidth */                           \
    0,                                      /* lPitch */                            \
    0,                                      /* dwBackBufferCount */                 \
    0,                                      /* dwZBufferBitDepth */                 \
    0,                                      /* dwAlphaBitDepth */                   \
    0,                                      /* dwReserved */                        \
    NULL,                                   /* lpSurface */                         \
    { 0, 0 },                               /* ddckCKDestOverlay */                 \
    { 0, 0 },                               /* ddckCKDestBlt */                     \
    { 0, 0 },                               /* ddckCKSrcOverlay */                  \
    { 0, 0 },                               /* ddckCKSrcBlt */                      \
    {                                                                               \
      sizeof(DDPIXELFORMAT),                /* ddpfPixelFormat.dwSize */            \
      DDPF_BUMPDUDV,                        /* ddpfPixelFormat.dwFlags */           \
      0,                                    /* FOURCC code */                       \
      16,                                   /* ddpfPixelFormat.dwRGBBitCount */     \
      0x000000FF,                           /* dU mask */                           \
      0x0000FF00,                           /* dV mask */                           \
      0x00000000,                           /* luminance mask */                    \
      0                                     /* unused */                            \
    },                                                                              \
    DDSCAPS_TEXTURE                         /* ddscaps.dwCaps */                    \
}

/*
 * 16-bit bumpmap with luminance
 * dU occupies the LSByte, luminance occupies the MSByte, dV is in the middle
 */
#define NV_TFMT_BUMPMAP_L6_DV5_DU5                                                  \
{                                                                                   \
    sizeof(DDSURFACEDESC),                  /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,           /* dwFlags */                           \
    0,                                      /* dwHeight */                          \
    0,                                      /* dwWidth */                           \
    0,                                      /* lPitch */                            \
    0,                                      /* dwBackBufferCount */                 \
    0,                                      /* dwZBufferBitDepth */                 \
    0,                                      /* dwAlphaBitDepth */                   \
    0,                                      /* dwReserved */                        \
    NULL,                                   /* lpSurface */                         \
    { 0, 0 },                               /* ddckCKDestOverlay */                 \
    { 0, 0 },                               /* ddckCKDestBlt */                     \
    { 0, 0 },                               /* ddckCKSrcOverlay */                  \
    { 0, 0 },                               /* ddckCKSrcBlt */                      \
    {                                                                               \
      sizeof(DDPIXELFORMAT),                /* ddpfPixelFormat.dwSize */            \
      DDPF_BUMPDUDV | DDPF_BUMPLUMINANCE,   /* ddpfPixelFormat.dwFlags */           \
      0,                                    /* FOURCC code */                       \
      16,                                   /* ddpfPixelFormat.dwRGBBitCount */     \
      0x0000001F,                           /* dU mask */                           \
      0x000003E0,                           /* dV mask */                           \
      0x0000FC00,                           /* luminance mask */                    \
      0                                     /* unused */                            \
    },                                                                              \
    DDSCAPS_TEXTURE                         /* ddscaps.dwCaps */                    \
}

/*
 * 24-bit bumpmap with luminance
 * dU is in the LSByte, dV is in the second byte and luminance is in the third.
 * the MSByte is unused
 */
#define NV_TFMT_BUMPMAP_L8_DV8_DU8                                                  \
{                                                                                   \
    sizeof(DDSURFACEDESC),                  /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,           /* dwFlags */                           \
    0,                                      /* dwHeight */                          \
    0,                                      /* dwWidth */                           \
    0,                                      /* lPitch */                            \
    0,                                      /* dwBackBufferCount */                 \
    0,                                      /* dwZBufferBitDepth */                 \
    0,                                      /* dwAlphaBitDepth */                   \
    0,                                      /* dwReserved */                        \
    NULL,                                   /* lpSurface */                         \
    { 0, 0 },                               /* ddckCKDestOverlay */                 \
    { 0, 0 },                               /* ddckCKDestBlt */                     \
    { 0, 0 },                               /* ddckCKSrcOverlay */                  \
    { 0, 0 },                               /* ddckCKSrcBlt */                      \
    {                                                                               \
      sizeof(DDPIXELFORMAT),                /* ddpfPixelFormat.dwSize */            \
      DDPF_BUMPDUDV | DDPF_BUMPLUMINANCE,   /* ddpfPixelFormat.dwFlags */           \
      0,                                    /* FOURCC code */                       \
      32,                                   /* ddpfPixelFormat.dwRGBBitCount */     \
      0x000000FF,                           /* dU mask */                           \
      0x0000FF00,                           /* dV mask */                           \
      0x00FF0000,                           /* luminance mask */                    \
      0                                     /* unused */                            \
    },                                                                              \
    DDSCAPS_TEXTURE                         /* ddscaps.dwCaps */                    \
}

/*
 * fourcc macro
 */
#define NV_TFMT_FOURCC(x)                                                           \
{                                                                                   \
    sizeof(DDSURFACEDESC),                  /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,           /* dwFlags */                           \
    0,                                      /* dwHeight */                          \
    0,                                      /* dwWidth */                           \
    0,                                      /* lPitch */                            \
    0,                                      /* dwBackBufferCount */                 \
    0,                                      /* dwZBufferBitDepth */                 \
    0,                                      /* dwAlphaBitDepth */                   \
    0,                                      /* dwReserved */                        \
    NULL,                                   /* lpSurface */                         \
    { 0, 0 },                               /* ddckCKDestOverlay */                 \
    { 0, 0 },                               /* ddckCKDestBlt */                     \
    { 0, 0 },                               /* ddckCKSrcOverlay */                  \
    { 0, 0 },                               /* ddckCKSrcBlt */                      \
    {                                                                               \
      sizeof(DDPIXELFORMAT),                /* ddpfPixelFormat.dwSize */            \
      DDPF_FOURCC,                          /* ddpfPixelFormat.dwFlags */           \
      (x),                                  /* FOURCC code */                       \
      0,                                    /* ddpfPixelFormat.dwRGBBitCount */     \
      0,                                                                            \
      0,                                                                            \
      0,                                                                            \
      0,                                                                            \
    },                                                                              \
    DDSCAPS_TEXTURE                         /* ddscaps.dwCaps */                    \
}

// FourCC formats
#define NV_TFMT_DXT1            NV_TFMT_FOURCC(FOURCC_DXT1)
#define NV_TFMT_DXT2            NV_TFMT_FOURCC(FOURCC_DXT2)
#define NV_TFMT_DXT3            NV_TFMT_FOURCC(FOURCC_DXT3)
#define NV_TFMT_DXT4            NV_TFMT_FOURCC(FOURCC_DXT4)
#define NV_TFMT_DXT5            NV_TFMT_FOURCC(FOURCC_DXT5)

// HILO formats
#define NV_TFMT_NVHU            NV_TFMT_FOURCC(FOURCC_NVHU)
#define NV_TFMT_NVHS            NV_TFMT_FOURCC(FOURCC_NVHS)

#define NV_TFMT_DX8(x,y)                                                            \
{                                                                                   \
    sizeof(DDPIXELFORMAT),                  /* ddpfPixelFormat.dwSize */            \
    DDPF_D3DFORMAT,                         /* ddpfPixelFormat.dwFlags */           \
    (x),                                    /* FOURCC code */                       \
    0,                                      /* ddpfPixelFormat.dwRGBBitCount */     \
    y,                                                                              \
    0,                                                                              \
    0,                                                                              \
    0,                                                                              \
}

#define NV_TFMT_DX8_KELVIN_MULTISAMPLE(x,y)                                                            \
{                                                                                   \
    sizeof(DDPIXELFORMAT),                  /* ddpfPixelFormat.dwSize */            \
    DDPF_D3DFORMAT,                         /* ddpfPixelFormat.dwFlags */           \
    (x),                                    /* FOURCC code */                       \
    0,                                      /* ddpfPixelFormat.dwRGBBitCount */     \
    y,                                                                              \
    (/*(1 << (D3DMULTISAMPLE_5_SAMPLES-1)) | */(1 << (D3DMULTISAMPLE_4_SAMPLES-1)) | (1 << (D3DMULTISAMPLE_3_SAMPLES-1)) | (1 << (D3DMULTISAMPLE_2_SAMPLES-1))) |        \
    ((/*(1 << (D3DMULTISAMPLE_5_SAMPLES-1)) | */(1 << (D3DMULTISAMPLE_4_SAMPLES-1)) | (1 << (D3DMULTISAMPLE_3_SAMPLES-1)) | (1 << (D3DMULTISAMPLE_2_SAMPLES-1))) << 16), \
    0,                                                                              \
    0,                                                                              \
}

#define NV_TFMT_DX8_CELSIUS_MULTISAMPLE(x,y)                                                            \
{                                                                                   \
    sizeof(DDPIXELFORMAT),                  /* ddpfPixelFormat.dwSize */            \
    DDPF_D3DFORMAT,                         /* ddpfPixelFormat.dwFlags */           \
    (x),                                    /* FOURCC code */                       \
    0,                                      /* ddpfPixelFormat.dwRGBBitCount */     \
    y,                                                                              \
    ((1 << (D3DMULTISAMPLE_4_SAMPLES-1)) | (1 << (D3DMULTISAMPLE_2_SAMPLES-1))) |        \
    (((1 << (D3DMULTISAMPLE_4_SAMPLES-1)) | (1 << (D3DMULTISAMPLE_2_SAMPLES-1))) << 16), \
    0,                                                                              \
    0,                                                                              \
}

#define D3D_RENDER_TEXTURE  D3DFORMAT_OP_CUBETEXTURE |            \
                            D3DFORMAT_OP_TEXTURE     |            \
                            D3DFORMAT_OP_OFFSCREEN_RENDERTARGET

#define D3D_TEXTURE         D3DFORMAT_OP_CUBETEXTURE |           \
                            D3DFORMAT_OP_TEXTURE

#define D3D_FRONT_BUFFER    D3DFORMAT_OP_DISPLAYMODE |           \
                            D3DFORMAT_OP_3DACCELERATION

#define D3D_BACK_BUFFER     D3DFORMAT_OP_SAME_FORMAT_UP_TO_ALPHA_RENDERTARGET

#define NV_TFMT_X1R5G5B5_DX8              NV_TFMT_DX8(D3DFMT_X1R5G5B5, (D3DFORMAT_OP_VOLUMETEXTURE | D3D_RENDER_TEXTURE | D3D_BACK_BUFFER))
#define NV_TFMT_A1R5G5B5_DX8              NV_TFMT_DX8(D3DFMT_A1R5G5B5, (D3DFORMAT_OP_VOLUMETEXTURE | D3D_TEXTURE))
#define NV_TFMT_A4R4G4B4_DX8              NV_TFMT_DX8(D3DFMT_A4R4G4B4, (D3DFORMAT_OP_VOLUMETEXTURE | D3D_TEXTURE))
#define NV_TFMT_R5G6B5_DX8                NV_TFMT_DX8(D3DFMT_R5G6B5,(D3DFORMAT_OP_VOLUMETEXTURE | D3D_RENDER_TEXTURE | D3D_BACK_BUFFER | D3D_FRONT_BUFFER ))
#define NV_TFMT_X8R8G8B8_DX8              NV_TFMT_DX8(D3DFMT_X8R8G8B8,(D3DFORMAT_OP_VOLUMETEXTURE | D3D_RENDER_TEXTURE | D3D_FRONT_BUFFER | D3D_BACK_BUFFER))
#define NV_TFMT_A8R8G8B8_DX8              NV_TFMT_DX8(D3DFMT_A8R8G8B8,(D3DFORMAT_OP_VOLUMETEXTURE | D3D_RENDER_TEXTURE | D3D_BACK_BUFFER))
#define NV_TFMT_RGB8_DX8                  NV_TFMT_DX8(D3DFMT_P8, (D3DFORMAT_OP_VOLUMETEXTURE | D3D_TEXTURE))
#define NV_TFMT_BUMPMAP_DV8_DU8_DX8       NV_TFMT_DX8(D3DFMT_V8U8, D3D_TEXTURE)
#define NV_TFMT_BUMPMAP_L6_DV5_DU5_DX8    NV_TFMT_DX8(D3DFMT_L6V5U5, D3D_TEXTURE)
#define NV_TFMT_BUMPMAP_L8_DV8_DU8_DX8    NV_TFMT_DX8(D3DFMT_X8L8V8U8, D3D_TEXTURE)
#define NV_TFMT_DXT1_DX8                  NV_TFMT_DX8(D3DFMT_DXT1, D3D_TEXTURE)
#define NV_TFMT_DXT2_DX8                  NV_TFMT_DX8(D3DFMT_DXT2, D3D_TEXTURE)
#define NV_TFMT_DXT3_DX8                  NV_TFMT_DX8(D3DFMT_DXT3, D3D_TEXTURE)
#define NV_TFMT_DXT4_DX8                  NV_TFMT_DX8(D3DFMT_DXT4, D3D_TEXTURE)
#define NV_TFMT_DXT5_DX8                  NV_TFMT_DX8(D3DFMT_DXT5, D3D_TEXTURE)

#define NV_TFMT_NVHU_DX8                  NV_TFMT_DX8(FOURCC_NVHU, D3D_TEXTURE)
#define NV_TFMT_NVHS_DX8                  NV_TFMT_DX8(FOURCC_NVHS, D3D_TEXTURE)

#ifndef TEXFORMAT_CRD // -----------------------------------------------------

#ifdef NVxn_SUPPORT_DX7

#define NV_TFMT_NVxn \
    NV_TFMT_FOURCC(FOURCC_NVT0), \
    NV_TFMT_FOURCC(FOURCC_NVT1), \
    NV_TFMT_FOURCC(FOURCC_NVT2), \
    NV_TFMT_FOURCC(FOURCC_NVT3), \
    NV_TFMT_FOURCC(FOURCC_NVT4), \
    NV_TFMT_FOURCC(FOURCC_NVT5), \
    NV_TFMT_FOURCC(FOURCC_NVS0), \
    NV_TFMT_FOURCC(FOURCC_NVS1), \
    NV_TFMT_FOURCC(FOURCC_NVS2), \
    NV_TFMT_FOURCC(FOURCC_NVS3), \
    NV_TFMT_FOURCC(FOURCC_NVS4), \
    NV_TFMT_FOURCC(FOURCC_NVS5),

#else // NVxn_SUPPORT_DX7

#define NV_TFMT_NVxn

#endif // NVxn_SUPPORT_DX7

#ifdef NVxn_SUPPORT_DX8

#define NV_TFMT_NVxn_DX8 \
    NV_TFMT_DX8(FOURCC_NVT0, D3D_TEXTURE), \
    NV_TFMT_DX8(FOURCC_NVT1, D3D_TEXTURE), \
    NV_TFMT_DX8(FOURCC_NVT2, D3D_TEXTURE), \
    NV_TFMT_DX8(FOURCC_NVT3, D3D_TEXTURE), \
    NV_TFMT_DX8(FOURCC_NVT4, D3D_TEXTURE), \
    NV_TFMT_DX8(FOURCC_NVT5, D3D_TEXTURE), \
    NV_TFMT_DX8(FOURCC_NVS0, D3D_TEXTURE), \
    NV_TFMT_DX8(FOURCC_NVS1, D3D_TEXTURE), \
    NV_TFMT_DX8(FOURCC_NVS2, D3D_TEXTURE), \
    NV_TFMT_DX8(FOURCC_NVS3, D3D_TEXTURE), \
    NV_TFMT_DX8(FOURCC_NVS4, D3D_TEXTURE), \
    NV_TFMT_DX8(FOURCC_NVS5, D3D_TEXTURE),

#else // NVxn_SUPPORT_DX8

#define NV_TFMT_NVxn_DX8

#endif // NVxn_SUPPORT_DX8

#ifdef HILO_SUPPORT_DX7
#define NV_TFMT_HILO_FORMATS              NV_TFMT_NVHU, NV_TFMT_NVHS,
#else
#define NV_TFMT_HILO_FORMATS
#endif

#ifdef HILO_SUPPORT_DX8
#define NV_TFMT_HILO_FORMATS_DX8          NV_TFMT_NVHU_DX8, NV_TFMT_NVHS_DX8,
#else
#define NV_TFMT_HILO_FORMATS_DX8
#endif

#endif  // !TEXFORMAT_CRD --------------------------------------------

#define NV_TFMT_Q8W8V8U8_DX8              NV_TFMT_DX8(D3DFMT_Q8W8V8U8, D3D_TEXTURE)

#define NV_TFMT_Z16                       NV_TFMT_DX8(D3DFMT_D16_LOCKABLE, D3DFORMAT_OP_ZSTENCIL)
#define NV_TFMT_Z24_S8                    NV_TFMT_DX8(D3DFMT_D24S8, D3DFORMAT_OP_ZSTENCIL)
#define NV_TFMT_Z24                       NV_TFMT_DX8(D3DFMT_D24X8, D3DFORMAT_OP_ZSTENCIL)

// Multisample formats
#define NV_TFMT_R5G6B5_DX8_KELVIN_MULTISAMPLE    NV_TFMT_DX8_KELVIN_MULTISAMPLE(D3DFMT_R5G6B5,(D3DFORMAT_OP_VOLUMETEXTURE | D3D_RENDER_TEXTURE | D3D_BACK_BUFFER | D3D_FRONT_BUFFER ))
#define NV_TFMT_X8R8G8B8_DX8_KELVIN_MULTISAMPLE  NV_TFMT_DX8_KELVIN_MULTISAMPLE(D3DFMT_X8R8G8B8,(D3DFORMAT_OP_VOLUMETEXTURE | D3D_RENDER_TEXTURE | D3D_FRONT_BUFFER | D3D_BACK_BUFFER))
#define NV_TFMT_A8R8G8B8_DX8_KELVIN_MULTISAMPLE  NV_TFMT_DX8_KELVIN_MULTISAMPLE(D3DFMT_A8R8G8B8,(D3DFORMAT_OP_VOLUMETEXTURE | D3D_RENDER_TEXTURE | D3D_BACK_BUFFER))

#define NV_TFMT_Z16_KELVIN_MULTISAMPLE           NV_TFMT_DX8_KELVIN_MULTISAMPLE(D3DFMT_D16,   D3DFORMAT_OP_ZSTENCIL | D3DFORMAT_OP_ZSTENCIL_WITH_ARBITRARY_COLOR_DEPTH)
#define NV_TFMT_Z24_S8_KELVIN_MULTISAMPLE        NV_TFMT_DX8_KELVIN_MULTISAMPLE(D3DFMT_D24S8, D3DFORMAT_OP_ZSTENCIL | D3DFORMAT_OP_ZSTENCIL_WITH_ARBITRARY_COLOR_DEPTH)
#define NV_TFMT_Z24_KELVIN_MULTISAMPLE           NV_TFMT_DX8_KELVIN_MULTISAMPLE(D3DFMT_D24X8, D3DFORMAT_OP_ZSTENCIL | D3DFORMAT_OP_ZSTENCIL_WITH_ARBITRARY_COLOR_DEPTH)

#define NV_TFMT_R5G6B5_DX8_CELSIUS_MULTISAMPLE    NV_TFMT_DX8_CELSIUS_MULTISAMPLE(D3DFMT_R5G6B5,(D3DFORMAT_OP_VOLUMETEXTURE | D3D_RENDER_TEXTURE | D3D_BACK_BUFFER | D3D_FRONT_BUFFER ))
#define NV_TFMT_X8R8G8B8_DX8_CELSIUS_MULTISAMPLE  NV_TFMT_DX8_CELSIUS_MULTISAMPLE(D3DFMT_X8R8G8B8,(D3DFORMAT_OP_VOLUMETEXTURE | D3D_RENDER_TEXTURE | D3D_FRONT_BUFFER | D3D_BACK_BUFFER))
#define NV_TFMT_A8R8G8B8_DX8_CELSIUS_MULTISAMPLE  NV_TFMT_DX8_CELSIUS_MULTISAMPLE(D3DFMT_A8R8G8B8,(D3DFORMAT_OP_VOLUMETEXTURE | D3D_RENDER_TEXTURE | D3D_BACK_BUFFER))

#define NV_TFMT_Z16_CELSIUS_MULTISAMPLE           NV_TFMT_DX8_CELSIUS_MULTISAMPLE(D3DFMT_D16,   D3DFORMAT_OP_ZSTENCIL)
#define NV_TFMT_Z24_S8_CELSIUS_MULTISAMPLE        NV_TFMT_DX8_CELSIUS_MULTISAMPLE(D3DFMT_D24S8, D3DFORMAT_OP_ZSTENCIL)
#define NV_TFMT_Z24_CELSIUS_MULTISAMPLE           NV_TFMT_DX8_CELSIUS_MULTISAMPLE(D3DFMT_D24X8, D3DFORMAT_OP_ZSTENCIL)

#ifdef TEXFORMAT_CRD

// moved to NvFormats.cpp

#else  // !TEXFORMAT_CRD

/*
 * Potentially need 4bpp and 8bpp palettized textures.
 */

// Define the texture formats available on NV3 hardware.
// NV_TFMT_R5G6B5 texture format should be the last format in the
// structure so it can be easily enabled/disabled at runtime.
#define NV3_TEXTURE_FORMATS                NV_TFMT_X1R5G5B5,                \
                                           NV_TFMT_A1R5G5B5,                \
                                           NV_TFMT_A4R4G4B4,                \
                                           NV_TFMT_R5G6B5

// Define the texture formats available on NV4 hardware.
#define NV4_TEXTURE_FORMATS                NV_TFMT_X1R5G5B5,                \
                                           NV_TFMT_A1R5G5B5,                \
                                           NV_TFMT_A4R4G4B4,                \
                                           NV_TFMT_R5G6B5,                  \
                                           NV_TFMT_X8R8G8B8,                \
                                           NV_TFMT_A8R8G8B8

// Define the texture formats available in the celsius class
#define CELSIUS_TEXTURE_FORMATS            NV_TFMT_X1R5G5B5,                \
                                           NV_TFMT_A1R5G5B5,                \
                                           NV_TFMT_A4R4G4B4,                \
                                           NV_TFMT_R5G6B5,                  \
                                           NV_TFMT_X8R8G8B8,                \
                                           NV_TFMT_A8R8G8B8,                \
                                           NV_TFMT_DXT1,                    \
                                           NV_TFMT_DXT2,                    \
                                           NV_TFMT_DXT3,                    \
                                           NV_TFMT_DXT4,                    \
                                           NV_TFMT_DXT5,                    \
                                           NV_TFMT_NVxn                     \
                                           NV_TFMT_RGB8

#define CELSIUS_TEXTURE_FORMATS_NO_DXT     NV_TFMT_X1R5G5B5,                \
                                           NV_TFMT_A1R5G5B5,                \
                                           NV_TFMT_A4R4G4B4,                \
                                           NV_TFMT_R5G6B5,                  \
                                           NV_TFMT_X8R8G8B8,                \
                                           NV_TFMT_A8R8G8B8,                \
                                           NV_TFMT_NVxn                     \
                                           NV_TFMT_RGB8

#define CELSIUS_TEXTURE_FORMATS_DX8        NV_TFMT_X1R5G5B5_DX8,                    \
                                           NV_TFMT_A1R5G5B5_DX8,                    \
                                           NV_TFMT_A4R4G4B4_DX8,                    \
                                           NV_TFMT_R5G6B5_DX8_CELSIUS_MULTISAMPLE,  \
                                           NV_TFMT_X8R8G8B8_DX8_CELSIUS_MULTISAMPLE,\
                                           NV_TFMT_A8R8G8B8_DX8_CELSIUS_MULTISAMPLE,\
                                           NV_TFMT_DXT1_DX8,                        \
                                           NV_TFMT_DXT2_DX8,                        \
                                           NV_TFMT_DXT3_DX8,                        \
                                           NV_TFMT_DXT4_DX8,                        \
                                           NV_TFMT_DXT5_DX8,                        \
                                           NV_TFMT_Z16_CELSIUS_MULTISAMPLE,         \
                                           NV_TFMT_Z24_CELSIUS_MULTISAMPLE,         \
                                           NV_TFMT_Z24_S8_CELSIUS_MULTISAMPLE,      \
                                           NV_TFMT_NVxn_DX8                         \
                                           NV_TFMT_RGB8_DX8

#define CELSIUS_TEXTURE_FORMATS_NO_DXT_DX8 NV_TFMT_X1R5G5B5_DX8,                    \
                                           NV_TFMT_A1R5G5B5_DX8,                    \
                                           NV_TFMT_A4R4G4B4_DX8,                    \
                                           NV_TFMT_R5G6B5_DX8_CELSIUS_MULTISAMPLE,  \
                                           NV_TFMT_X8R8G8B8_DX8_CELSIUS_MULTISAMPLE,\
                                           NV_TFMT_A8R8G8B8_DX8_CELSIUS_MULTISAMPLE,\
                                           NV_TFMT_BUMPMAP_DV8_DU8_DX8,             \
                                           NV_TFMT_BUMPMAP_L6_DV5_DU5_DX8,          \
                                           NV_TFMT_BUMPMAP_L8_DV8_DU8_DX8,          \
                                           NV_TFMT_Z16_CELSIUS_MULTISAMPLE,         \
                                           NV_TFMT_Z24_CELSIUS_MULTISAMPLE,         \
                                           NV_TFMT_Z24_S8_CELSIUS_MULTISAMPLE,      \
                                           NV_TFMT_NVxn_DX8                         \
                                           NV_TFMT_RGB8_DX8

#if (NVARCH >= 0x020)

// Define the texture formats available in the kelvin class
#define KELVIN_TEXTURE_FORMATS             NV_TFMT_X1R5G5B5,                \
                                           NV_TFMT_A1R5G5B5,                \
                                           NV_TFMT_A4R4G4B4,                \
                                           NV_TFMT_R5G6B5,                  \
                                           NV_TFMT_X8R8G8B8,                \
                                           NV_TFMT_A8R8G8B8,                \
                                           NV_TFMT_BUMPMAP_DV8_DU8,         \
                                           NV_TFMT_BUMPMAP_L6_DV5_DU5,      \
                                           NV_TFMT_BUMPMAP_L8_DV8_DU8,      \
                                           NV_TFMT_DXT1,                    \
                                           NV_TFMT_DXT2,                    \
                                           NV_TFMT_DXT3,                    \
                                           NV_TFMT_DXT4,                    \
                                           NV_TFMT_DXT5,                    \
                                           NV_TFMT_NVxn                     \
                                           NV_TFMT_HILO_FORMATS             \
                                           NV_TFMT_RGB8

#define KELVIN_TEXTURE_FORMATS_NO_DXT      NV_TFMT_X1R5G5B5,                \
                                           NV_TFMT_A1R5G5B5,                \
                                           NV_TFMT_A4R4G4B4,                \
                                           NV_TFMT_R5G6B5,                  \
                                           NV_TFMT_X8R8G8B8,                \
                                           NV_TFMT_A8R8G8B8,                \
                                           NV_TFMT_BUMPMAP_DV8_DU8,         \
                                           NV_TFMT_BUMPMAP_L6_DV5_DU5,      \
                                           NV_TFMT_BUMPMAP_L8_DV8_DU8,      \
                                           NV_TFMT_NVxn                     \
                                           NV_TFMT_HILO_FORMATS             \
                                           NV_TFMT_RGB8

#define KELVIN_TEXTURE_FORMATS_DX8         NV_TFMT_X1R5G5B5_DX8,                   \
                                           NV_TFMT_A1R5G5B5_DX8,                   \
                                           NV_TFMT_A4R4G4B4_DX8,                   \
                                           NV_TFMT_R5G6B5_DX8_KELVIN_MULTISAMPLE,  \
                                           NV_TFMT_X8R8G8B8_DX8_KELVIN_MULTISAMPLE,\
                                           NV_TFMT_A8R8G8B8_DX8_KELVIN_MULTISAMPLE,\
                                           NV_TFMT_BUMPMAP_DV8_DU8_DX8,            \
                                           NV_TFMT_BUMPMAP_L6_DV5_DU5_DX8,         \
                                           NV_TFMT_BUMPMAP_L8_DV8_DU8_DX8,         \
                                           NV_TFMT_DXT1_DX8,                       \
                                           NV_TFMT_DXT2_DX8,                       \
                                           NV_TFMT_DXT3_DX8,                       \
                                           NV_TFMT_DXT4_DX8,                       \
                                           NV_TFMT_DXT5_DX8,                       \
                                           NV_TFMT_Z16_KELVIN_MULTISAMPLE,         \
                                           NV_TFMT_Z24_KELVIN_MULTISAMPLE,         \
                                           NV_TFMT_Z24_S8_KELVIN_MULTISAMPLE,      \
                                           NV_TFMT_Q8W8V8U8_DX8,                   \
                                           NV_TFMT_NVxn_DX8                        \
                                           NV_TFMT_HILO_FORMATS_DX8                \
                                           NV_TFMT_RGB8_DX8

#define KELVIN_TEXTURE_FORMATS_NO_DXT_DX8  NV_TFMT_X1R5G5B5_DX8,                   \
                                           NV_TFMT_A1R5G5B5_DX8,                   \
                                           NV_TFMT_A4R4G4B4_DX8,                   \
                                           NV_TFMT_R5G6B5_DX8_KELVIN_MULTISAMPLE,  \
                                           NV_TFMT_X8R8G8B8_DX8_KELVIN_MULTISAMPLE,\
                                           NV_TFMT_A8R8G8B8_DX8_KELVIN_MULTISAMPLE,\
                                           NV_TFMT_BUMPMAP_DV8_DU8_DX8,            \
                                           NV_TFMT_BUMPMAP_L6_DV5_DU5_DX8,         \
                                           NV_TFMT_BUMPMAP_L8_DV8_DU8_DX8,         \
                                           NV_TFMT_Z16_KELVIN_MULTISAMPLE,         \
                                           NV_TFMT_Z24_KELVIN_MULTISAMPLE,         \
                                           NV_TFMT_Z24_S8_KELVIN_MULTISAMPLE,      \
                                           NV_TFMT_Q8W8V8U8_DX8,                   \
                                           NV_TFMT_NVxn_DX8                        \
                                           NV_TFMT_HILO_FORMATS_DX8                \
                                           NV_TFMT_RGB8_DX8

#endif  // (NVARCH >= 0x020)

// number of 8-bit formats (common to all classes (for now at least))
#define NV_NUM_8BIT_TEXTURE_FORMATS        1

#endif  // !TEXFORMAT_CRD

#ifdef __cplusplus
}
#endif

#endif  // _NVTEXFMT_H_
