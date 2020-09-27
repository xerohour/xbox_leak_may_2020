#ifndef __gl_nvPixelFmt_h_
#define __gl_nvPixelFmt_h_

/*
 * Copyright (c) 1997, Nvidia Corporation.  All rights reserved.
 * Portions Copyright (c) 1996, 1997, Silicon Graphics Inc.
 */

int STDCALL __wglNVDescribePixelFormat(int bpp, int iPixelFormat, UINT nBytes,
                                       PIXELFORMATDESCRIPTOR *ppfd,
                                       int flags);

#define NUM_PIXELFORMATS_16 6
#define NUM_PIXELFORMATS_16_OVERLAY 12
#define NUM_PIXELFORMATS_32 10
#define NUM_PIXELFORMATS_32_OVERLAY 20
#define NUM_NV11_PIXELFORMATS_32 15

extern PIXELFORMATDESCRIPTOR *NV4PixelFormats16[NUM_PIXELFORMATS_16];
extern PIXELFORMATDESCRIPTOR *NV4PixelFormats32[NUM_PIXELFORMATS_32];
extern PIXELFORMATDESCRIPTOR *NV4PixelFormats32Overlay[NUM_PIXELFORMATS_32_OVERLAY];
extern PIXELFORMATDESCRIPTOR *NV11PixelFormats32[NUM_NV11_PIXELFORMATS_32];

#define NV_PFD_SUPPORTS_COLOR32_DEPTH16 0x00000001 /* nv11 */
#define NV_PFD_SUPPORTS_COLOR16_DEPTH32 0x00000002 /* hopefully, some day */
#define NV_PFD_SUPPORTS_OVERLAYS        0x00000004
#define NV_PFD_SUPPORTS_SWAPCOPYONLY    0x00000008

#endif  /* __gl_nvPixelFmt_h_ */
