#ifndef __WINCOMMON_H_
#define __WINCOMMON_H_

/*
 * Copyright (c) 1997, Nvidia Corporation.  All rights reserved.
 * Portions Copyright (c) 1996, 1997, Silicon Graphics Inc.
 */

/* Remember, this file is used by non-Win32 OpenGL drivers, too. */

// the high two bits are the severity code, see ntstatus.h and winerror.h
// the low 8 bits are unexplained in the windows docs, but don't indicate an error
#define IS_EVENT_ERROR(a) (((a & 0xC0000000) != 0) || (a >= 0x100))

typedef struct __WINNVpixelFormatFlagsRec {
    int unifiedBuffers;
    int color32depth16;
    int color16depth32;
    int multisample;
    int pbuffers;
    int flippingControl;
} __WINNVpixelFormatFlags;

#define WIN_PFD_SUPPORTS_COLOR32_DEPTH16    0x00000001 /* nv11 */
#define WIN_PFD_SUPPORTS_COLOR16_DEPTH32    0x00000002 /* nv20 */
#define WIN_PFD_SUPPORTS_OVERLAYS           0x00000004
#define WIN_PFD_SUPPORTS_SWAPCOPYONLY       0x00000008
#define WIN_PFD_SUPPORTS_SWAP_LAYER_BUFFERS 0x00000010
#define WIN_PFD_SUPPORTS_STEREO             0x00000020
#define WIN_PFD_SUPPORTS_PBUFFERS           0x00000040
#define WIN_PFD_SUPPORTS_MULTISAMPLE        0x00000080

// ---------- Internal Nvidia Pixel Format structure ---------------------

// bits for flags1
#define WINPF_DRAW_TO_WINDOW       0x00000001
#define WINPF_DRAW_TO_BITMAP       0x00000002
#define WINPF_DRAW_TO_PBUFFER      0x00000004
#define WINPF_NEED_PALETTE         0x00000008
#define WINPF_NEED_SYSTEM_PALETTE  0x00000010
#define WINPF_SWAP_LAYER_BUFFERS   0x00000020
#define WINPF_SUPPORT_GDI          0x00000040
#define WINPF_OPENGL_COMPLIANT     0x00000080
#define WINPF_DOUBLE_BUFFER        0x00000100
#define WINPF_STEREO               0x00000200
#define WINPF_SHARE_DEPTH          0x00000400
#define WINPF_SHARE_STENCIL        0x00000800
#define WINPF_SHARE_ACCUM          0x00001000
#define WINPF_TRANSPARENCY         0x00002000

typedef enum {
	WINPF_SWAP_UNDEFINED,
    WINPF_SWAP_COPY,
    WINPF_SWAP_EXCHANGE
} __WINswapMethod;

typedef enum {
    WINPF_PIXEL_RGBA,
    WINPF_PIXEL_COLOR_INDEX
} __WINpixelType;

typedef struct __WINpixelFormatRec {
    unsigned int     flags1;
    __WINswapMethod  swapMethod;
    __WINpixelType   pixelType;
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
    unsigned int     layerPlane;
} __WINpixelFormat;

// nvsharedpixelfmt.c
extern int STDCALL __winNVProcessWinFlags(__WINNVpixelFormatFlags *winPFFlags);
extern int __winDescribePixelFormat(struct __WINpixelFormatRec *winPixelFormat, int desktopBPP,
                                    int iLayerPlane, int pixelFormat, unsigned int flags,
                                    int *startNonVisible); 
#endif  /* __WINCOMMON_H_ */
