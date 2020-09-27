#ifndef _NVTEXFMT_H_
#define _NVTEXFMT_H_
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

/*
 * Define the various texture format definitions.
 * For any given hardware, the texture format definitions don't change,
 * just the formats that are available on that hardware change.
 */
/*
 * RGB 16 BPP (R5xG5xB5)
 */
#define NV_TFMT_X1R5G5B5                                                        \
{                                                                               \
    sizeof(DDSURFACEDESC),              /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */                           \
    0,                                  /* dwHeight */                          \
    0,                                  /* dwWidth */                           \
    0,                                  /* lPitch */                            \
    0,                                  /* dwBackBufferCount */                 \
    0,                                  /* dwZBufferBitDepth */                 \
    0,                                  /* dwAlphaBitDepth */                   \
    0,                                  /* dwReserved */                        \
    NULL,                               /* lpSurface */                         \
    { 0, 0 },                           /* ddckCKDestOverlay */                 \
    { 0, 0 },                           /* ddckCKDestBlt */                     \
    { 0, 0 },                           /* ddckCKSrcOverlay */                  \
    { 0, 0 },                           /* ddckCKSrcBlt */                      \
    {                                                                           \
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB,                         /* ddpfPixelFormat.dwFlags */           \
      0,                                /* FOURCC code */                       \
      16,                               /* ddpfPixelFormat.dwRGBBitCount */     \
      0x7C00,                                                                   \
      0x03E0,                                                                   \
      0x001F,                                                                   \
      0x0000                                                                    \
    },                                                                          \
    DDSCAPS_TEXTURE,                    /* ddscaps.dwCaps */                    \
}

/*
 * RGB 16 BPP Alpha (A1xR5xG5xB5)
 */
#define NV_TFMT_A1R5G5B5                                                        \
{                                                                               \
    sizeof(DDSURFACEDESC),              /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */                           \
    0,                                  /* dwHeight */                          \
    0,                                  /* dwWidth */                           \
    0,                                  /* lPitch */                            \
    0,                                  /* dwBackBufferCount */                 \
    0,                                  /* dwZBufferBitDepth */                 \
    0,                                  /* dwAlphaBitDepth */                   \
    0,                                  /* dwReserved */                        \
    NULL,                               /* lpSurface */                         \
    { 0, 0 },                           /* ddckCKDestOverlay */                 \
    { 0, 0 },                           /* ddckCKDestBlt */                     \
    { 0, 0 },                           /* ddckCKSrcOverlay */                  \
    { 0, 0 },                           /* ddckCKSrcBlt */                      \
    {                                                                           \
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB | DDPF_ALPHAPIXELS,      /* ddpfPixelFormat.dwFlags */           \
      0,                                /* FOURCC code */                       \
      16,                               /* ddpfPixelFormat.dwRGBBitCount */     \
      0x7C00,                                                                   \
      0x03E0,                                                                   \
      0x001F,                                                                   \
      0x8000                                                                    \
    },                                                                          \
    DDSCAPS_TEXTURE,                    /* ddscaps.dwCaps */                    \
}

/*
 * RGB 16 BPP Alpha (A4xR4xG4xB4)
 */
#define NV_TFMT_A4R4G4B4                                                        \
{                                                                               \
    sizeof(DDSURFACEDESC),              /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */                           \
    0,                                  /* dwHeight */                          \
    0,                                  /* dwWidth */                           \
    0,                                  /* lPitch */                            \
    0,                                  /* dwBackBufferCount */                 \
    0,                                  /* dwZBufferBitDepth */                 \
    0,                                  /* dwAlphaBitDepth */                   \
    0,                                  /* dwReserved */                        \
    NULL,                               /* lpSurface */                         \
    { 0, 0 },                           /* ddckCKDestOverlay */                 \
    { 0, 0 },                           /* ddckCKDestBlt */                     \
    { 0, 0 },                           /* ddckCKSrcOverlay */                  \
    { 0, 0 },                           /* ddckCKSrcBlt */                      \
    {                                                                           \
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB | DDPF_ALPHAPIXELS,      /* ddpfPixelFormat.dwFlags */           \
      0,                                /* FOURCC code */                       \
      16,                               /* ddpfPixelFormat.dwRGBBitCount */     \
      0x0F00,                                                                   \
      0x00F0,                                                                   \
      0x000F,                                                                   \
      0xF000                                                                    \
    },                                                                          \
    DDSCAPS_TEXTURE,                    /* ddscaps.dwCaps */                    \
}

/*
 * RGB 16 BPP (R5xG6xB5)
 */
#define NV_TFMT_R5G6B5                                                          \
{                                                                               \
    sizeof(DDSURFACEDESC),              /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */                           \
    0,                                  /* dwHeight */                          \
    0,                                  /* dwWidth */                           \
    0,                                  /* lPitch */                            \
    0,                                  /* dwBackBufferCount */                 \
    0,                                  /* dwZBufferBitDepth */                 \
    0,                                  /* dwAlphaBitDepth */                   \
    0,                                  /* dwReserved */                        \
    NULL,                               /* lpSurface */                         \
    { 0, 0 },                           /* ddckCKDestOverlay */                 \
    { 0, 0 },                           /* ddckCKDestBlt */                     \
    { 0, 0 },                           /* ddckCKSrcOverlay */                  \
    { 0, 0 },                           /* ddckCKSrcBlt */                      \
    {                                                                           \
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB,                         /* ddpfPixelFormat.dwFlags */           \
      0,                                /* FOURCC code */                       \
      16,                               /* ddpfPixelFormat.dwRGBBitCount */     \
      0xF800,                                                                   \
      0x07E0,                                                                   \
      0x001F,                                                                   \
      0x0000                                                                    \
    },                                                                          \
    DDSCAPS_TEXTURE,                    /* ddscaps.dwCaps */                    \
}

/*
 * RGB 32 BPP (X8xR8xG8xB8)
 */
#define NV_TFMT_X8R8G8B8                                                        \
{                                                                               \
    sizeof(DDSURFACEDESC),              /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */                           \
    0,                                  /* dwHeight */                          \
    0,                                  /* dwWidth */                           \
    0,                                  /* lPitch */                            \
    0,                                  /* dwBackBufferCount */                 \
    0,                                  /* dwZBufferBitDepth */                 \
    0,                                  /* dwAlphaBitDepth */                   \
    0,                                  /* dwReserved */                        \
    NULL,                               /* lpSurface */                         \
    { 0, 0 },                           /* ddckCKDestOverlay */                 \
    { 0, 0 },                           /* ddckCKDestBlt */                     \
    { 0, 0 },                           /* ddckCKSrcOverlay */                  \
    { 0, 0 },                           /* ddckCKSrcBlt */                      \
    {                                                                           \
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB,                         /* ddpfPixelFormat.dwFlags */           \
      0,                                /* FOURCC code */                       \
      32,                               /* ddpfPixelFormat.dwRGBBitCount */     \
      0x00FF0000,                       /* Red Component Mask */                \
      0x0000FF00,                       /* Green Component Mask */              \
      0x000000FF,                       /* Blue Component Mask */               \
      0x00000000                        /* Alpha Component Mask */              \
    },                                                                          \
    DDSCAPS_TEXTURE,                    /* ddscaps.dwCaps */                    \
}

/*
 * RGB 32 BPP (A8xR8xG8xB8)
 */
#define NV_TFMT_A8R8G8B8                                                        \
{                                                                               \
    sizeof(DDSURFACEDESC),              /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */                           \
    0,                                  /* dwHeight */                          \
    0,                                  /* dwWidth */                           \
    0,                                  /* lPitch */                            \
    0,                                  /* dwBackBufferCount */                 \
    0,                                  /* dwZBufferBitDepth */                 \
    0,                                  /* dwAlphaBitDepth */                   \
    0,                                  /* dwReserved */                        \
    NULL,                               /* lpSurface */                         \
    { 0, 0 },                           /* ddckCKDestOverlay */                 \
    { 0, 0 },                           /* ddckCKDestBlt */                     \
    { 0, 0 },                           /* ddckCKSrcOverlay */                  \
    { 0, 0 },                           /* ddckCKSrcBlt */                      \
    {                                                                           \
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB | DDPF_ALPHAPIXELS,      /* ddpfPixelFormat.dwFlags */           \
      0,                                /* FOURCC code */                       \
      32,                               /* ddpfPixelFormat.dwRGBBitCount */     \
      0x00FF0000,                       /* Red Component Mask */                \
      0x0000FF00,                       /* Green Component Mask */              \
      0x000000FF,                       /* Blue Component Mask */               \
      0xFF000000                        /* Alpha Component Mask */              \
    },                                                                          \
    DDSCAPS_TEXTURE,                    /* ddscaps.dwCaps */                    \
}

/*
 * RGB 24 BPP (R8xG8xB8)
 * Hardware doesn't actually support this.  The texture would actually
 * have to be expanded to a 32bpp X8xR8xG8xB8 format.
 * Haven't decided if I really want to do this.
 */
#define NV_TFMT_R8G8B8                                                          \
{                                                                               \
    sizeof(DDSURFACEDESC),              /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */                           \
    0,                                  /* dwHeight */                          \
    0,                                  /* dwWidth */                           \
    0,                                  /* lPitch */                            \
    0,                                  /* dwBackBufferCount */                 \
    0,                                  /* dwZBufferBitDepth */                 \
    0,                                  /* dwAlphaBitDepth */                   \
    0,                                  /* dwReserved */                        \
    NULL,                               /* lpSurface */                         \
    { 0, 0 },                           /* ddckCKDestOverlay */                 \
    { 0, 0 },                           /* ddckCKDestBlt */                     \
    { 0, 0 },                           /* ddckCKSrcOverlay */                  \
    { 0, 0 },                           /* ddckCKSrcBlt */                      \
    {                                                                           \
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB,                         /* ddpfPixelFormat.dwFlags */           \
      0,                                /* FOURCC code */                       \
      24,                               /* ddpfPixelFormat.dwRGBBitCount */     \
      0x00FF0000,                       /* Red Component Mask */                \
      0x0000FF00,                       /* Green Component Mask */              \
      0x000000FF,                       /* Blue Component Mask */               \
      0x00000000                        /* Alpha Component Mask */              \
    },                                                                          \
    DDSCAPS_TEXTURE,                    /* ddscaps.dwCaps */                    \
}

/*
 * FFVII REQUIRES 8bit textures.  What a pain.  Of course so does PC98/PC99.
 */
#define NV_TFMT_RGB8                                                            \
{                                                                               \
    sizeof(DDSURFACEDESC),              /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */                           \
    0,                                  /* dwHeight */                          \
    0,                                  /* dwWidth */                           \
    0,                                  /* lPitch */                            \
    0,                                  /* dwBackBufferCount */                 \
    0,                                  /* dwZBufferBitDepth */                 \
    0,                                  /* dwAlphaBitDepth */                   \
    0,                                  /* dwReserved */                        \
    NULL,                               /* lpSurface */                         \
    { 0, 0 },                           /* ddckCKDestOverlay */                 \
    { 0, 0 },                           /* ddckCKDestBlt */                     \
    { 0, 0 },                           /* ddckCKSrcOverlay */                  \
    { 0, 0 },                           /* ddckCKSrcBlt */                      \
    {                                                                           \
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */            \
      DDPF_RGB | DDPF_PALETTEINDEXED8,  /* ddpfPixelFormat.dwFlags */           \
      0,                                /* FOURCC code */                       \
      8,                                /* ddpfPixelFormat.dwRGBBitCount */     \
      0x00000000,                       /* Red Component Mask */                \
      0x00000000,                       /* Green Component Mask */              \
      0x00000000,                       /* Blue Component Mask */               \
      0x00000000                        /* Alpha Component Mask */              \
    },                                                                          \
    DDSCAPS_TEXTURE,                    /* ddscaps.dwCaps */                    \
}

/*
 * Y8 Texture Format
 */
#define NV_TFMT_Y8                                                              \
{                                                                               \
    sizeof(DDSURFACEDESC),              /* dwSize */                            \
    DDSD_CAPS | DDSD_PIXELFORMAT,       /* dwFlags */                           \
    0,                                  /* dwHeight */                          \
    0,                                  /* dwWidth */                           \
    0,                                  /* lPitch */                            \
    0,                                  /* dwBackBufferCount */                 \
    0,                                  /* dwZBufferBitDepth */                 \
    0,                                  /* dwAlphaBitDepth */                   \
    0,                                  /* dwReserved */                        \
    NULL,                               /* lpSurface */                         \
    { 0, 0 },                           /* ddckCKDestOverlay */                 \
    { 0, 0 },                           /* ddckCKDestBlt */                     \
    { 0, 0 },                           /* ddckCKSrcOverlay */                  \
    { 0, 0 },                           /* ddckCKSrcBlt */                      \
    {                                                                           \
      sizeof(DDPIXELFORMAT),            /* ddpfPixelFormat.dwSize */            \
      DDPF_LUMINANCE,                   /* ddpfPixelFormat.dwFlags */           \
      0,                                /* FOURCC code */                       \
      8,                                /* ddpfPixelFormat.dwRGBBitCount */     \
      0x000000FF,                       /* Red Component Mask */                \
      0x00000000,                       /* Green Component Mask */              \
      0x00000000,                       /* Blue Component Mask */               \
      0x00000000                        /* Alpha Component Mask */              \
    },                                                                          \
    DDSCAPS_TEXTURE,                    /* ddscaps.dwCaps */                    \
}

/*
 * Potentially need 4bpp and 8bpp palettized textures.
 */

/*
 * Define the texture formats available on NV3 hardware.
 * NV_TFMT_R5G6B5 texture format should be the last format in the
 * structure so it can be easily enabled/disabled at runtime.
 */
#define NV3_TEXTURE_FORMATS NV_TFMT_X1R5G5B5,   \
                            NV_TFMT_A1R5G5B5,   \
                            NV_TFMT_A4R4G4B4,   \
                            NV_TFMT_R5G6B5

/*
 * Define the texture formats available on NV4 hardware.
 */
#define NV_NUM_8BIT_TEXTURE_FORMATS             2
#define NV_NUM_8BIT_Y8ONLY_TEXTURE_FORMATS      (NV_NUM_8BIT_TEXTURE_FORMATS - 1)
#define NV4_TEXTURE_FORMATS NV_TFMT_X1R5G5B5,   \
                            NV_TFMT_A1R5G5B5,   \
                            NV_TFMT_A4R4G4B4,   \
                            NV_TFMT_R5G6B5,     \
                            NV_TFMT_X8R8G8B8,   \
                            NV_TFMT_A8R8G8B8,   \
                            NV_TFMT_Y8,         \
                            NV_TFMT_RGB8

/*
 * Based on the compile flag choose the texture formats available
 * for this driver build.
 */
#ifdef  NV3
#define NV_TEXTURE_FORMATS  NV3_TEXTURE_FORMATS
#endif  // NV3
#ifdef  NV4
#define NV_TEXTURE_FORMATS  NV4_TEXTURE_FORMATS
#endif  // NV4
#endif  // _NVTEXFMT_H_