#ifndef __gl_nvPixelFmt_h_
#define __gl_nvPixelFmt_h_

/*
 * Copyright (c) 1997, Nvidia Corporation.  All rights reserved.
 * Portions Copyright (c) 1996, 1997, Silicon Graphics Inc.
 */

#if defined(WIN32)
/* Remember, this file is used by non-Win32 OpenGL drivers, too. */

int STDCALL __wglNVDescribePixelFormat(int bpp, int iLayerPlane, int iPixelFormat,
                                       unsigned int nBytes,
                                       PIXELFORMATDESCRIPTOR *ppfd,
                                       LAYERPLANEDESCRIPTOR *plpd,
                                       int flags,
                                       int showExtended);

#define NUM_PIXELFORMATS_16 6
#define NUM_PIXELFORMATS_16_OVERLAY 10
#define NUM_PIXELFORMATS_16_STEREO  8
#define NUM_NV20_PIXELFORMATS_16 6
#define NUM_PIXELFORMATS_32 10
#define NUM_PIXELFORMATS_32_OVERLAY 16
#define NUM_PIXELFORMATS_32_STEREO  12
#define NUM_NV11_PIXELFORMATS_32 15

extern PIXELFORMATDESCRIPTOR *NV4PixelFormats16[NUM_PIXELFORMATS_16];
extern PIXELFORMATDESCRIPTOR *NV4PixelFormats16Stereo[NUM_PIXELFORMATS_16_STEREO];
extern PIXELFORMATDESCRIPTOR *NV20PixelFormats16[NUM_NV20_PIXELFORMATS_16];
extern PIXELFORMATDESCRIPTOR *NV4PixelFormats32[NUM_PIXELFORMATS_32];
extern PIXELFORMATDESCRIPTOR *NV4PixelFormats32Overlay[NUM_PIXELFORMATS_32_OVERLAY];
extern PIXELFORMATDESCRIPTOR *NV4PixelFormats32Stereo[NUM_PIXELFORMATS_32_STEREO];
extern PIXELFORMATDESCRIPTOR *NV11PixelFormats32[NUM_NV11_PIXELFORMATS_32];

#endif /* defined(WIN32) */

#define NV_PFD_SUPPORTS_COLOR32_DEPTH16    0x00000001 /* nv11 */
#define NV_PFD_SUPPORTS_COLOR16_DEPTH32    0x00000002 /* nv20 */
#define NV_PFD_SUPPORTS_OVERLAYS           0x00000004
#define NV_PFD_SUPPORTS_SWAPCOPYONLY       0x00000008
#define NV_PFD_SUPPORTS_SWAP_LAYER_BUFFERS 0x00000010
#define NV_PFD_SUPPORTS_STEREO             0x00000020
#define NV_PFD_SUPPORTS_PBUFFERS           0x00000040
#define NV_PFD_SUPPORTS_MULTISAMPLE        0x00000080

// ---------- Internal Nvidia Pixel Format structure ---------------------

// bits for flags1
#define NVPF_DRAW_TO_WINDOW       0x00000001
#define NVPF_DRAW_TO_BITMAP       0x00000002
#define NVPF_DRAW_TO_PBUFFER      0x00000004
#define NVPF_NEED_PALETTE         0x00000008
#define NVPF_NEED_SYSTEM_PALETTE  0x00000010
#define NVPF_SWAP_LAYER_BUFFERS   0x00000020
#define NVPF_SUPPORT_GDI          0x00000040
#define NVPF_SUPPORT_OPENGL       0x00000080
#define NVPF_DOUBLE_BUFFER        0x00000100
#define NVPF_STEREO               0x00000200
#define NVPF_SHARE_DEPTH          0x00000400
#define NVPF_SHARE_STENCIL        0x00000800
#define NVPF_SHARE_ACCUM          0x00001000
#define NVPF_TRANSPARENCY         0x00002000

typedef enum {
	NVPF_SWAP_UNDEFINED,
    NVPF_SWAP_COPY,
    NVPF_SWAP_EXCHANGE
} __GLNVswapMethod;

typedef enum {
    NVPF_PIXEL_RGBA,
    NVPF_PIXEL_COLOR_INDEX
} __GLNVpixelType;

typedef struct __GLNVpixelFormatRec {
    NvU32            flags1;
    __GLNVswapMethod swapMethod;
    __GLNVpixelType  pixelType;
    unsigned int     numOverlays;
    unsigned int     numUnderlays;
    unsigned int     colorBits;
    unsigned int     redBits;
    unsigned int     redShift;
    unsigned int     greenBits;
    unsigned int     greenShift;
    unsigned int     blueBits;
    unsigned int     blueShift;
    unsigned int     alphaBits;
    unsigned int     alphaShift;
    unsigned int     accumBits;
    unsigned int     accumRedBits;
    unsigned int     accumGreenBits;
    unsigned int     accumBlueBits;
    unsigned int     accumAlphaBits;
    unsigned int     depthBits;
    unsigned int     stencilBits;
    unsigned int     auxBuffers;
    unsigned int     transparentRed;
    unsigned int     transparentGreen;
    unsigned int     transparentBlue;
    unsigned int     transparentAlpha;
    unsigned int     transparentIndex;
    unsigned int     maxPbufferPixels;
    unsigned int     maxPbufferWidth;
    unsigned int     maxPbufferHeight;
    unsigned int     sampleBuffers;
    unsigned int     samples;
} __GLNVpixelFormat;

#endif  /* __gl_nvPixelFmt_h_ */
