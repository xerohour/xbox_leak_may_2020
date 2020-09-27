 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/


//******************************************************************************
// WARNING: !!!                                                     WARNING: !!!
//
//    In order to be able to use this same driver source with older nv3 chips and
//    resource manager, this file includes the following OBSOLETE structure
//    definitions. These structures are OBSOLETE and meant to be used with NV3 only.
//
//      NV_CLASS                            (Class 0x01)
//      NV_CONTEXT_DMA_FROM_MEMORY          (Class 0x02)
//      NV_CONTEXT_DMA_TO_MEMORY            (Class 0x03)
//      NV_PATCHCORD_VIDEO                  (Class 0x0a)
//      NV_VIDEO_SINK                       (Class 0x0b)
//      NV_VIDEO_COLOR_MAP                  (Class 0x0d)
//      NV_PATCHCORD_IMAGE                  (Class 0x0e)
//      NV_IMAGE_STENCIL                    (Class 0x10)
//      NV_IMAGE_ROP_AND                    (Class 0x13)
//      NV_IMAGE_COLOR_KEY                  (Class 0x15)
//      NV_IMAGE_SOLID                      (Class 0x17)
//      NV_IMAGE_PATTERN                    (Class 0x18)
//      NV_IMAGE_BLACK_RECTANGLE            (Class 0x19)
//      NV_RENDER_SOLID_LIN                 (Class 0x1c)
//      NV_RENDER_SOLID_RECTANGLE           (Class 0x1e)
//      NV_IMAGE_BLIT                       (Class 0x1f)
//      NV_IMAGE_FROM_CPU                   (Class 0x21)
//      NV_PATCHCORD_ROP                    (Class 0x33)
//      NV_MEMORY_TO_MEMORY_FORMAT          (Class 0x39)
//      NV_CONTEXT_DMA_IN_MEMORY            (Class 0x3d)
//      IMAGE_IN_MEMORY                     (Class 0x3e)
//      NV_VIDEO_FROM_MEMORY                (Class 0x3f)
//      NV_VIDEO_SCALER                     (Class 0x40)
//      NV_VIDEO_COLOR_KEY                  (Class 0x41)
//      NV_ROP5_SOLID                       (Class 0x43)
//      ZETA_BUFFER_IN_MEMORY               (Class 0x45)
//      NV_PATCHCORD_ZETA                   (Class 0x46)
//      NV_RENDER_D3D0_TRIANGLE_ZETA        (Class 0x48)
//      NV_RENDER_GDI0_RECTANGLE_AND_TEXT   (Class 0x4b)
//
//    The following NV3 VIDEO classes are still supported in NV4 but their
//    CLASS id numbers are DIFFERENT depending on which resman/chip you use !!!!
//    (resman.050 supports NV4 , resman.040 supports NV3)
//
//          RESMAN.050                                  RESMAN.040
//          ----------                                  ----------
//      NV4_VIDEO_COLOR_KEY (class 0xFF6) is same as NV_VIDEO_COLOR_KEY (class 0x41)
//      NV4_VIDEO_SCALER    (class 0xFF7) is same as NV_VIDEO_SCALER    (class 0x40)
//      NV4_VIDEO_COLORMAP  (class 0xFF9) is same as NV_VIDEO_COLORMAP  (class 0xd)
//
//
// WARNING: !!!                                                     WARNING: !!!
//******************************************************************************

#include <nvtypes.h>

//******************************************************************************
// The following classes are no longer supported beginning with NV4 !!!   |
//                                                                        |
// In order to be able to use this driver with the older nv3 chips      | | |
// and resource manager, we include the old obsolete structure           | |
// definitions in this separate header file.                              |
//
//******************************************************************************


/***** SetMonochromeFormat values *****/
#define NV_MONO_LE_M1_P32          0x00000000
#define NV_MONO_CGA6_M1_P32        0x00000001
#define NV_MONOCHROME_FORMAT_LE_M1_P32     0x00000000
#define NV_MONOCHROME_FORMAT_CGA6_M1_P32   0x00000001

//------------------------------------------------------------------------------
// SetColorFormat Values (old style global color format)
//
// The format names precisely specify how the bits are arranged
// in a 32-bit write of a color or colors in that format in a
// little-endian system.  For example NV_COLOR_FORMAT_LE_X17R5G5B5
// means the upper 17 bits are ignored, then 5 bits each of
// Red, Green, and Blue intensity:
//
//   3                             1 1
//   1                             6 5                             0
//  |X X X X X X X X X X X X X X X X X R R R R R G G G G G B B B B B|
//
//------------------------------------------------------------------------------

/* Unpacked color formats */
#define NV_COLOR_FORMAT_LE_X17R5G5B5       0x00000000
#define NV_COLOR_FORMAT_LE_X16A1R5G5B5     0x00100000
#define NV_COLOR_FORMAT_LE_X24Y8           0x00010000
/* Packed color formats -- more than one value per 32-bit write */
#define NV_COLOR_FORMAT_LE_X1R5G5B5_P2     0x01000000
#define NV_COLOR_FORMAT_LE_A1R5G5B5_P2     0x01100000
#define NV_COLOR_FORMAT_LE_Y8_P4           0x01010000
/* No difference between packed and unpacked */
#define NV_COLOR_FORMAT_LE_X8R8G8B8        0x00000001
#define NV_COLOR_FORMAT_LE_A8R8G8B8        0x00100001
#define NV_COLOR_FORMAT_LE_X2R10G10B10     0x00000002
#define NV_COLOR_FORMAT_LE_A2R10G10B10     0x00100002
#define NV_COLOR_FORMAT_LE_X16A8Y8         0x00110000
#define NV_COLOR_FORMAT_LE_X16Y16          0x00010001
#define NV_COLOR_FORMAT_LE_A16Y16          0x00110001

//------------------------------------------------------------------------------
// These defines are not present in the latest nv32.h file.
// So we'll include them here (so we can support old style pre-NV4 drivers).
//------------------------------------------------------------------------------

#define NV_IP_PATTERN_SHAPE_8X_8Y               (0x00000000)
#define NV_IP_PATTERN_SHAPE_64X_1Y              (0x00000001)
#define NV_IP_PATTERN_SHAPE_1X_64Y              (0x00000002)

//------------------------------------------------------------------------------
// Client sets NvNotification.status to NV_STATUS_IN_PROGRESS, NV
// sets to a different value when it notifies.
//------------------------------------------------------------------------------

#define NV_STATUS_IN_PROGRESS      255

//------------------------------------------------------------------------------
// The minimum depth of the NV chip's FIFO -- never wait for the free count
// to reach a value greater than NV_GUARANTEED_FIFO_SIZE (124 bytes)!
//------------------------------------------------------------------------------

#define NV_GUARANTEED_FIFO_SIZE                 (0x007C)

//------------------------------------------------------------------------------
// Obsolete classes (pre-NV4)
// We include them here so we can support old style pre-NV4 drivers, as we
// begin to transition to NV4.
//------------------------------------------------------------------------------

/* class NV_CLASS */
#define NV_CLASS  1
typedef volatile struct tagNvClass {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 V032 Create;                  /* 32-bit name of new object       0300-0303*/
 V032 Destroy;                 /* 32-bit name of object           0304-0307*/
 V032 SetNameCtxDma;           /* NV_CONTEXT_DMA_FROM_MEMORY      0308-030b*/
 U032 SetNameStart;            /* offset within ctxdma in bytes   030c-030f*/
 U032 SetNameLength;           /* ASCII O/S name length in bytes  0310-0313*/
 V032 SetEnumerateCtxDma;      /* NV_CONTEXT_DMA_TO_MEMORY        0314-0317*/
 U032 SetEnumerateStart;       /* offset within ctxdma in bytes   0318-031b*/
 U032 SetEnumerateLength;      /* in bytes                        031c-031f*/
 U032 Enumerate;               /* index into instance list        0320-0323*/
 V032 reserved02[0x737];
} NvClass;


/* class NV_CONTEXT_DMA_FROM_MEMORY */
#define NV_CONTEXT_DMA_FROM_MEMORY  2
typedef volatile struct tagNvContextDmaFromMemory {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 struct tagNvSetDmaFromSpecifier {
  V032 address[2];               /* On X86, address[1] is the selector */
  U032 limit;                    /* size of data region in bytes - 1 */
 } SetDmaSpecifier;            /* memory region NV will read      0300-030b*/
 V032 reserved02[0x73d];
} NvContextDmaFromMemory;

#define  NV01_CONTEXT_DMA             0x00000002


/* class NV_CONTEXT_DMA_TO_MEMORY */
#define NV_CONTEXT_DMA_TO_MEMORY  3
typedef volatile struct tagNvContextDmaToMemory {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 struct tagNvSetDmaToSpecifier {
  V032 address[2];               /* On X86, address[1] is the selector */
  U032 limit;                    /* size of data region in bytes - 1   */
 } SetDmaSpecifier;            /* memory region NV will write     0300-030b*/
 V032 reserved02[0x73d];
} NvContextDmaToMemory;


/* class NV_PATCHCORD_VIDEO */
#define NV_PATCHCORD_VIDEO  10
typedef volatile struct tagNvPatchcordVideo {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 V032 Validate;                /* writing any value validates     0300-0303*/
 V032 reserved02[0x73f];
} NvPatchcordVideo;


/* class NV_VIDEO_SINK */
#define NV_VIDEO_SINK  11
typedef volatile struct tagNvVideoSink {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetVideoInput[56];       /* NV_PATCHCORD_VIDEO              0200-02df*/
// V032 SetVideoInputNotifyCtxDma;/*
 V032 reserved02[0x748];
} NvVideoSink;

// Video Class supported in resman.040

/* class NV_VIDEO_COLORMAP */
#define NV_VIDEO_COLORMAP  13
typedef volatile struct tagNvVideoColorMap {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetVideoOutput;          /* NV_PATCHCORD_VIDEO              0200-0203*/
 V032 SetVideoInput;           /* NV_PATCHCORD_VIDEO              0204-0207*/
 V032 reserved02[0x03e];
 V032 SetColormapCtxDma;       /* NV_CONTEXT_DMA_FROM_MEMORY      0300-0303*/
 V032 SetColormapFormat;       /*                                 0304-0307*/
 U032 SetColormapStart;        /* offset in ctxdma in bytes       0308-030b*/
 U032 SetColormapLength;       /* in bytes                        030c-030f*/
 U032 ColormapDirtyStart;      /* start of region to update       0310-0313*/
 U032 ColormapDirtyLength;     /* in bytes, triggers asynch. xfer 0314-0317*/
 V032 reserved03[0x73a];
} NvVideoColorMap;
/* values for NV_VIDEO_COLORMAP SetColormapFormat() */
#define NV_COLORMAP_X8R8G8B8       0x00000000
#define NV_COLORMAP_X2R10G10B10    0x00000001


/* class NV_PATCHCORD_IMAGE */
#define NV_PATCHCORD_IMAGE  14
typedef volatile struct tagNvPatchcordImage {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 V032 Validate;                /* validate connections downstream 0300-0303*/
 V032 reserved02[0x73f];
} NvPatchcordImage;


/* class NV_IMAGE_STENCIL */
#define NV_IMAGE_STENCIL  16
typedef volatile struct tagNvImageStencil {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 SetImageInput[2];        /* P_I, 1 is ImageBlackRectangle   0204-020b*/
 V032 reserved02[0x77d];
} NvImageStencil;


/* class NV_IMAGE_ROP_AND */
#define NV_IMAGE_ROP_AND  19
typedef volatile struct tagNvImageRopAnd {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 SetRopInput;             /* NV_PATCHCORD_ROP                0204-0207*/
 V032 SetImageInput[3];        /* 0 is I2V ImageOut, 2 is pattern 0208-0213*/
 V032 reserved02[0x77b];
} NvImageRopAnd;


/* class NV_IMAGE_COLOR_KEY */
#define NV_IMAGE_COLOR_KEY  21
typedef volatile struct tagNvImageColorKey {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 SetImageInput[2];        /* P_I, 1 from ImageSolidColor     0204-020b*/
 V032 reserved02[0x77d];
} NvImageColorKey;


/* class NV_IMAGE_SOLID */
#define NV_IMAGE_SOLID  23
typedef volatile struct tagNvImageSolid {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* must be an unpacked format      0300-0303*/
 V032 SetColor;                /* color to fill the image         0304-0307*/
 V032 reserved03[0x73e];
} NvImageSolid;


/* class NV_IMAGE_PATTERN */
#define NV_IMAGE_PATTERN  24
typedef volatile struct tagNvImagePattern {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* Color 0/1, must be unpacked fmt 0300-0303*/
 V032 SetMonochromeFormat;     /* a NV_MONOCHROME_FORMAT_XX value 0304-0307*/
 V032 SetPatternShape;         /* NV_PATTERN_SHAPE_{8X8,64X1,1X64}0308-030b*/
 V032 reserved03[0x001];
 V032 SetColor0;               /* "background" color where pat=0  0310-0313*/
 V032 SetColor1;               /* "foreground" color where pat=1  0314-0317*/
 struct tagNvSetPattern {
  V032 monochrome[2];
 } SetPattern;                 /* 64 bits of pattern data         0318-031f*/
 V032 reserved04[0x738];
} NvImagePattern;
/* values for NV_IMAGE_PATTERN SetPatternShape() */
#define NV_PATTERN_SHAPE_8X8   0x00000000
#define NV_PATTERN_SHAPE_64X1  0x00000001
#define NV_PATTERN_SHAPE_1X64  0x00000002


/* class NV_IMAGE_BLACK_RECTANGLE */
#define NV_IMAGE_BLACK_RECTANGLE  25
typedef volatile struct tagNvImageBlackRectangle {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* P_I usually to NV_IMAGE_STENCIL 0200-0203*/
 V032 reserved02[0x03f];
 struct tagNvSetRectangle {
  V032 y_x;                      /* S16_S16 in pixels, 0 at top left 00-03*/
  V032 height_width;             /* U16_U16 in pixels                04-07*/
 } SetRectangle;               /* region in image where alpha=1   0300-0307*/
 V032 reserved03[0x73e];
} NvImageBlackRectangle;


/* class NV_RENDER_SOLID_LIN */
#define NV_RENDER_SOLID_LIN  28
typedef volatile struct tagNvRenderSolidLin {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* must be an unpacked format      0300-0303*/
 V032 Color;                   /*                                 0304-0307*/
 V032 reserved03[0x03e];
 struct tagNvLin {
  V032 y0_x0;                    /* S16_S16 in pixels, 0 at top left 00-03*/
  V032 y1_x1;                    /* S16_S16 in pixels, 0 at top left 04-07*/
 } Lin[16];                    /*                                 0400-047f*/
 struct tagNvLin32 {
  S032 x0;                         /*                                 00-03*/
  S032 y0;                         /*                                 04-07*/
  S032 x1;                         /*                                 08-0b*/
  S032 y1;                         /*                                 0c-0f*/
 } Lin32[8];                   /*                                 0480-04ff*/
 V032 PolyLin[32];             /*                                 0500-057f*/
 struct tagNvPolyLin32 {
  S032 x;                          /*                                 00-03*/
  S032 y;                          /*                                 04-07*/
 } PolyLin32[16];              /*                                 0580-05ff*/
 struct tagNvColorPolyLin {
  V032 color;                      /*                                 00-03*/
  V032 y_x;                      /* S16_S16 in pixels, 0 at top left 04-07*/
 } ColorPolyLin[16];           /*                                 0600-067f*/
 V032 reserved04[0x660];
} NvRenderSolidLin;


/* class NV_RENDER_SOLID_RECTANGLE */
#define NV_RENDER_SOLID_RECTANGLE  30
typedef volatile struct tagNvRenderSolidRectangle {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /*                                 0300-0303*/
 V032 Color;                   /*                                 0304-0307*/
 V032 reserved03[0x03e];
 struct tagNvRectangle {
  V032 y_x;                      /* S16_S16 in pixels, 0 at top left 00-03*/
  V032 height_width;             /* U16_U16 in pixels                04-07*/
 } Rectangle[16];              /*                                 0400-047f*/
 V032 reserved04[0x6e0];
} NvRenderSolidRectangle;


/* class NV_IMAGE_BLIT */
#define NV_IMAGE_BLIT  31
typedef volatile struct tagNvImageBlit {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 SetImageInput;           /* NV_PATCHCORD_IMAGE              0204-0207*/
 V032 reserved02[0x03e];
 V032 ControlPointIn;          /* S16_S16 in pixels, u.r. of src  0300-0303*/
 V032 ControlPointOut;         /* S16_16 in pixels, u.r. of dest  0304-0307*/
 V032 Size;                    /* U16_U16 in pixels               0308-030b*/
 V032 reserved03[0x73d];
} NvImageBlit;


/* class NV_IMAGE_FROM_CPU */
#define NV_IMAGE_FROM_CPU  33
typedef volatile struct tagNvImageFromCpu {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* must be a packed format         0300-0303*/
 V032 Point;                   /* y_x S16_S16 in pixels           0304-0307*/
 V032 Size;                    /* height_width U16_U16 in pixels  0308-030b*/
 V032 SizeIn;                  /* height_width U16_U16 in pixels  030c-030f*/
 V032 reserved03[0x03c];
 V032 Color[32];               /* packed pixel(s) to add to image 0400-047f*/
 V032 reserved04[0x6e0];
} NvImageFromCpu;


/* class NV_PATCHCORD_ROP */
#define NV_PATCHCORD_ROP  51
typedef volatile struct tagNvPatchcordRop {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 V032 Validate;                /* writing any value validates     0300-0303*/
 V032 reserved02[0x73f];
} NvPatchcordRop;

/* class NV_STRETCHED_IMAGE_FROM_CPU */
#define NV_STRETCHED_IMAGE_FROM_CPU  54
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS   0104-0107*/
 V032 StopTransfer;            /* writing any value stops transfer  0108-010b*/
 V032 reserved01[0x03d];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE                0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* must be a packed format           0300-0303*/
 V032 SizeIn;                  /* height_width U16_U16 in texels    0304-0307*/
 V032 DeltaDxDu;               /* dx/du S12.20                      0308-030b*/
 V032 DeltaDyDv;               /* dy/dv S12.20                      030c-030f*/
 struct {
  V032 y_x;                    /* S16_S16 in pixels, 0 at top left  0310-0313*/
  V032 height_width;           /* U16_U16 in pixels                 0314-0317*/
 } Clip;
 V032 Point12d4;               /* y_x S12.4_S12.4 in pixels         0318-031b*/
 V032 reserved03[0x039];
 V032 Color[1792];             /* packed pixel(s) of one scan line  0400-1fff*/
} NvStretchedImageFromCpu;

/* class NV_SCALED_IMAGE_FROM_MEMORY */
#define NV_SCALED_IMAGE_FROM_MEMORY  55
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS   0104-0107*/
 V032 StopTransfer;            /* writing any value stops transfer  0108-010b*/
 V032 reserved01[0x03d];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE                0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* must be a packed format           0300-0303*/
 V032 SetImageCtxDma;          /* NV_CONTEXT_DMA_FROM_MEMORY        0304-0307*/
 struct {
  V032 y_x;                    /* S16_S16 in pixels, 0 at top left  0308-030b*/
  V032 height_width;           /* U16_U16 in pixels                 030c-030f*/
 } Clip;
 struct {
  V032 y_x;                    /* S16_S16 in pixels, 0 at top left  0310-0313*/
  V032 height_width;           /* U16_U16 in pixels                 0314-0317*/
 } RectangleOut;
 V032 DeltaDuDx;               /* du/dx S12.20                      0318-031b*/
 V032 DeltaDvDy;               /* dv/dy S12.20                      031c-031f*/
 V032 reserved03[0x038];
 struct {
  V032 size;                   /* height_width U16_U16 in texels    0400-0403*/
  U032 pitch;                  /* bytes between lines               0404-0407*/
  U032 offset;                 /* byte offset of top-left texel     0408-040b*/
  V032 point12d4;              /* v_u U12.4_U12.4 in texels         040c-040f*/
 } ImageIn;
 V032 reserved04[0x6fc];
} NvScaledImageFromMemory;
/* additional values for SetColorFormat() */
#define NV_COLOR_FORMAT_LE_V8Y8U8Y8   0x00020000  /* YUYV, YUY2, YUV2 */
#define NV_COLOR_FORMAT_LE_Y8V8Y8U8   0x00020001  /* UYVY, CCIR601    */

/* class NV_SCALED_YUV40_FROM_MEMORY */
#define NV_SCALED_YUV420_FROM_MEMORY  56
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS   0104-0107*/
 V032 StopTransfer;            /* writing any value stops transfer  0108-010b*/
 V032 reserved01[0x03d];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE                0200-0203*/
 V032 reserved02[0x040];
 V032 SetImageCtxDma;          /* NV_CONTEXT_DMA_FROM_MEMORY        0304-0307*/
 struct {
  V032 y_x;                    /* S16_S16 in pixels, 0 at top left  0308-030b*/
  V032 height_width;           /* U16_U16 in pixels                 030c-030f*/
 } Clip;
 struct {
  V032 y_x;                    /* S16_S16 in pixels, 0 at top left  0310-0313*/
  V032 height_width;           /* U16_U16 in pixels                 0314-0317*/
 } RectangleOut;
 V032 DeltaDuDx;               /* du/dx S12.20                      0318-031b*/
 V032 DeltaDvDy;               /* dv/dy S12.20                      031c-031f*/
 V032 reserved03[0x038];
 struct {
  V032 size;                   /* height_width U16_U16 in texels    0400-0403*/
  V032 pitch;                  /* uv_y U16_U16 bytes between lines  0404-0407*/
  U032 offsety;                /* byte offset of top-left Y texel   0408-040b*/
  U032 offsetu;                /* byte offset of top-left U texel   040c-040f*/
  U032 offsetv;                /* byte offset of top-left V texel   0410-0413*/
  V032 point12d4;              /* v_u U12.4_U12.4 of Y in texels    0414-0417*/
 } ImageIn;
 V032 reserved04[0x6fa];
} NvScaledYuv420FromMemory;

/* class NV_MEMORY_TO_MEMORY_FORMAT */
#define NV_MEMORY_TO_MEMORY_FORMAT  57
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS   0104-0107*/
 V032 StopTransfer;            /* writing any value stops transfer  0108-010b*/
 V032 reserved01[0x07d];
 V032 SetBufferInCtxDma;       /* NV_CONTEXT_DMA_FROM_MEMORY        0300-0303*/
 V032 SetBufferOutCtxDma;      /* NV_CONTEXT_DMA_TO_MEMORY          0304-0307*/
 V032 SetBufferNotifyCtxDma;   /* NV_CONTEXT_DMA_TO_MEMORY          0308-030b*/
 struct {
  U032 offsetin;               /* input  ctxdma offset in bytes     030c-030f*/
  U032 offsetout;              /* output ctxdma offset in bytes     0310-0313*/
  S032 pitchin;                /* delta in bytes between scan lines 0314-0317*/
  S032 pitchout;               /* delta in bytes between scan lines 0318-031b*/
  U032 linelengthin;           /* in bytes                          031c-031f*/
  U032 linecount;              /* in lines                          0320-0323*/
  V032 format;                 /* ORed NV_MTMF_FORMAT_*             0324-0327*/
  V032 notify;                 /* notification style, varies w/OS   0328-032b*/
 } BufferTransfer;
 V032 reserved02[0x735];
} NvMemoryToMemoryFormat;
/* values for BufferTransfer.format() */
#define NV_MTMF_FORMAT_INPUT_INC_1     0x00000001
#define NV_MTMF_FORMAT_INPUT_INC_2     0x00000002
#define NV_MTMF_FORMAT_INPUT_INC_4     0x00000004
#define NV_MTMF_FORMAT_OUTPUT_INC_1    0x00000100
#define NV_MTMF_FORMAT_OUTPUT_INC_2    0x00000200
#define NV_MTMF_FORMAT_OUTPUT_INC_4    0x00000400


/* class NV_CONTEXT_DMA_IN_MEMORY */
#define NV_CONTEXT_DMA_IN_MEMORY  61
typedef volatile struct tagNvContextDmaInMemory {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS   0104-0107*/
 V032 reserved01[0x07e];
 struct {
  V032 address[2];             /* On X86 address[1] is the selector 0300-0307*/
  U032 limit;                  /* size of data region in bytes - 1  0308-030b*/
 } SetDmaSpecifier;
 V032 reserved02[0x73d];
} NvContextDmaInMemory;


/* class NV_IMAGE_IN_MEMORY */
#define NV_IMAGE_IN_MEMORY  62
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS   0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE                0200-0203*/
 V032 SetImageInput[63];       /* NV_PATCHCORD_IMAGEs               0204-02ff*/
 V032 SetColorFormat;          /* must be a packed format           0300-0303*/
 V032 SetImageCtxDma;          /* NV_CONTEXT_DMA_IN_MEMORY          0304-0307*/
 S032 SetImagePitch;           /* gap in bytes between scan lines   0308-030b*/
 U032 SetImageOffset;          /* byte offset of top-left pixel     030c-030f*/
 V032 reserved02[0x73c];
} NvImageInMemory;
/* additional values for SetColorFormat() */
#define NV_COLOR_FORMAT_LE_Y16_P2   0x01010001


/* class NV_VIDEO_FROM_MEMORY */
#define NV_VIDEO_FROM_MEMORY  63
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS   0104-0107*/
 V032 StopTransfer;            /* writing any value stops transfer  0108-010b*/
 V032 reserved01[0x03d];
 V032 SetVideoOutput;          /* NV_PATCHCORD_VIDEO                0200-0203*/
 V032 reserved02[0x03f];
 V032 SetImageCtxDma[2];       /* NV_CONTEXT_DMA_FROM_MEMORY        0300-0307*/
 V032 SetImageNotifyCtxDma[2]; /* NV_CONTEXT_DMA_TO_MEMORY          0308-030f*/
 struct {                      /*                                   0310-    */
  U032 offset;                 /* byte offset of top-left pixel       00-  03*/
  U032 pitch;                  /* bytes between lines                 04-  07*/
  V032 size;                   /* height_width U16_U16 in pixels      08-  0b*/
  V032 format;                 /* ORed NV_VFM_IMAGE_FORMAT_*          0c-  0f*/
  V032 notify;                 /* notification style, varies w/OS     10-  13*/
 } ImageScan[2];               /*                                       -0337*/
 V032 reserved03[0x732];
} NvVideoFromMemory;
/* values for ImageScan.format() */
#define NV_VFM_FORMAT_NEXT_FIELD_EVEN        0x00000000
#define NV_VFM_FORMAT_NEXT_FIELD_ODD         0x80000000
#define NV_VFM_FORMAT_COLOR_LE_Y8_P4         0x01010000
#define NV_VFM_FORMAT_COLOR_LE_X1R5G5B5_P2   0x01000000
#define NV_VFM_FORMAT_COLOR_LE_X8R8G8B8      0x00000001
#define NV_VFM_FORMAT_COLOR_LE_V8Y8U8Y8      0x00020000
#define NV_VFM_FORMAT_COLOR_LE_Y8V8Y8U8      0x00020001

// Video Class supported in resman.040

/* class NV_VIDEO_SCALER */
#define NV_VIDEO_SCALER  64
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS   0104-0107*/
 V032 reserved01[0x03e];
 V032 SetVideoOutput;          /* NV_PATCHCORD_VIDEO                0200-0203*/
 V032 SetVideoInput;           /* NV_PATCHCORD_VIDEO                0204-0207*/
 V032 reserved02[0x03e];
 V032 SetDeltaDuDx;            /* du/dx S12.20                      0300-0303*/
 V032 SetDeltaDvDy;            /* dv/dy S12.20                      0304-0307*/
 V032 SetPoint;                /* y_x S16_S16 in pixels of origin   0308-030b*/
 V032 reserved03[0x73d];
} NvVideoScaler;


// Video Class supported in resman.040

/* class NV_VIDEO_COLOR_KEY */
#define NV_VIDEO_COLOR_KEY  65
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS   0104-0107*/
 V032 reserved01[0x03e];
 V032 SetVideoOutput;          /* NV_PATCHCORD_VIDEO                0200-0203*/
 V032 SetVideoInput[3];        /* NV_PATCHCORD_VIDEO                0204-020f*/
 V032 reserved02[0x03c];
 V032 SetColorFormat;          /* must be an unpacked format        0300-0303*/
 V032 SetColorKey;             /* color to compare with             0304-0307*/
 V032 SetPoint;                /* y_x, S16_S16 in pixels, top left  0308-030b*/
 V032 SetSize;                 /* height_width, U16_U16 in pixels   030c-030f*/
 V032 reserved03[0x73c];
} NvVideoColorKey;

/* class NV_ROP5_SOLID */
#define NV_ROP5_SOLID  67
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS   0104-0107*/
 V032 reserved01[0x03e];
 V032 SetRopOutput;            /* NV_PATCHCORD_ROP                  0200-0203*/
 V032 reserved02[0x03f];
 V032 SetRop5;                 /* 32-bit ROP5                       0300-0303*/
 V032 reserved03[0x73f];
} NvRop5Solid;


/* class NV_ZETA_BUFFER_IN_MEMORY */
#define NV_ZETA_BUFFER_IN_MEMORY  69
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* NV_NOTIFY_* varies with OS        0104-0107*/
 V032 reserved01[0x03e];
 V032 SetZetaOutput;           /* NV_PATCHCORD_ZETA                 0200-0203*/
 V032 SetZetaInput[63];        /* NV_PATCHCORD_ZETAs                0204-02ff*/
 V032 SetZetaFormat;           /* NV_ZETA_FORMAT_* (packed)         0300-0303*/
 V032 SetZetaCtxDma;           /* NV_CONTEXT_DMA_IN_MEMORY          0304-0307*/
 S032 SetZetaPitch;            /* bytes, vertical zeta delta        0308-030b*/
 U032 SetZetaOffset;           /* byte offset of top-left zeta      030c-030f*/
 V032 reserved02[0x73c];
} NvZetaBufferInMemory;
/* additional values for SetColorFormat() */
#define NV_ZETA_FORMAT_LE_Z16_P2           0x00000000


/* class NV_PATCHCORD_ZETA */
#define NV_PATCHCORD_ZETA  70
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS   0104-0107*/
 V032 reserved01[0x07e];
 V032 Validate;                /* writing any value validates       0300-0303*/
 V032 reserved02[0x73f];
} NvPatchcordZeta;


/* class NV_RENDER_D3D0_TRIANGLE_ZETA */
#define NV_RENDER_D3D0_TRIANGLE_ZETA  72
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* NV_NOTIFY_* varies with OS        0104-0107*/
 V032 StopTransfer;            /* zero, stops transfer              0108-010b*/
 V032 InstantiatePatch;        /* zero, instantiates patch          010c-010f*/
 V032 reserved01[0x03c];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE                0200-0203*/
 V032 SetZetaOutput;           /* NV_PATCHCORD_ZETA                 0204-0207*/
 V032 SetZetaInput;            /* NV_PATCHCORD_ZETA                 0208-020b*/
 V032 reserved02[0x03d];
 V032 SetTextureCtxDma;        /* NV_CONTEXT_DMA_FROM_MEMORY        0300-0303*/
 U032 TextureOffset;           /* offset within ctxdma in bytes     0304-0307*/
 V032 TextureFormat;           /* NV_D3D0_TEXTURE_FORMAT_* OR'd     0308-030b*/
 V032 Filter;                  /* turbo_mipmap_spready_spreadx      030c-030f*/
 V032 FogColor;                /* X8R8G8B8 fog color                0310-0313*/
 V032 Control0;                /* NV_D3D0_CONTROL0_* OR'd           0314-0317*/
 V032 Control1;                /* NV_D3D0_CONTROL1_* OR'd           0318-031b*/
 V032 reserved03[0x339];
 struct {                      /* start of method in array          1000-    */
  V032 specular;               /* fog_i5-i0, U0d8_U004_..._U004       00-  03*/
  V032 color;                  /* X8R8G8B8 or A8R8G8B8, U0d8 chans    04-  07*/
  F032 x;                      /* screen x coordinate                 08-  0b*/
  F032 y;                      /* screen y coordinate                 0c-  0f*/
  F032 z;                      /* screen z coordinate                 10-  13*/
  F032 m;                      /* reciprocal homogenous W (m=1/W)     14-  17*/
  F032 u;                      /* texture u coordinate                18-  1b*/
  F032 v;                      /* texture v coordinate                1c-  1f*/
 } ControlPoint[128];          /* end of method in array                -1fff*/
} NvRenderD3D0TriangleZeta;
/* values for TextureFormat() */
#define NV_D3D0_TEXTURE_FORMAT_COLOR_KEY_COLOR_MASK   0x0000ffff
#define NV_D3D0_TEXTURE_FORMAT_COLOR_KEY_DISABLED     0x00000000
#define NV_D3D0_TEXTURE_FORMAT_COLOR_KEY_ENABLED      0x00010000
#define NV_D3D0_TEXTURE_FORMAT_COLOR_FORMAT_A1R5G5B5  0x00000000
#define NV_D3D0_TEXTURE_FORMAT_COLOR_FORMAT_X1R5G5B5  0x00100000
#define NV_D3D0_TEXTURE_FORMAT_COLOR_FORMAT_A4R4G4G4  0x00200000
#define NV_D3D0_TEXTURE_FORMAT_COLOR_FORMAT_R5G6B5    0x00300000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MIN_4X4           0x02000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MIN_8X8           0x03000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MIN_16X16         0x04000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MIN_32X32         0x05000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MIN_64X64         0x06000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MIN_128X128       0x07000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MIN_256X256       0x08000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MIN_512X512       0x09000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MIN_1024X1024     0x0a000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MIN_2048X2048     0x0b000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MAX_4X4           0x20000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MAX_8X8           0x30000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MAX_16X16         0x40000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MAX_32X32         0x50000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MAX_64X64         0x60000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MAX_128X128       0x70000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MAX_256X256       0x80000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MAX_512X512       0x90000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MAX_1024X1024     0xa0000000
#define NV_D3D0_TEXTURE_FORMAT_SIZE_MAX_2048X2048     0xb0000000
/* values for Control0() */
#define NV_D3D0_CONTROL0_INTERPOLATOR_ZOH             0x00000000
#define NV_D3D0_CONTROL0_INTERPOLATOR_ZOH_MS          0x00000001
#define NV_D3D0_CONTROL0_INTERPOLATOR_FOH             0x00000002
#define NV_D3D0_CONTROL0_WRAP_U_CYLINDRICAL           0x00000000
#define NV_D3D0_CONTROL0_WRAP_U_WRAP                  0x00000010
#define NV_D3D0_CONTROL0_WRAP_U_MIRROR                0x00000020
#define NV_D3D0_CONTROL0_WRAP_U_CLAMP                 0x00000030
#define NV_D3D0_CONTROL0_WRAP_V_CYLINDRICAL           0x00000000
#define NV_D3D0_CONTROL0_WRAP_V_WRAP                  0x00000040
#define NV_D3D0_CONTROL0_WRAP_V_MIRROR                0x00000080
#define NV_D3D0_CONTROL0_WRAP_V_CLAMP                 0x000000c0
#define NV_D3D0_CONTROL0_COLOR_FORMAT_LE_X8R8G8B8     0x00000000
#define NV_D3D0_CONTROL0_COLOR_FORMAT_LE_A8R8G8B8     0x00000100
#define NV_D3D0_CONTROL0_SRCCOLOR_NORMAL              0x00000000
#define NV_D3D0_CONTROL0_SRCCOLOR_COLOR_INVERSE       0x00000400
#define NV_D3D0_CONTROL0_SRCCOLOR_ALPHA_INVERSE       0x00000800
#define NV_D3D0_CONTROL0_SRCCOLOR_ALPHA_ONE           0x00000c00
#define NV_D3D0_CONTROL0_CULLING_NONE                 0x00001000
#define NV_D3D0_CONTROL0_CULLING_COUNTERCLOCKWISE     0x00002000
#define NV_D3D0_CONTROL0_CULLING_CLOCKWISE            0x00003000
#define NV_D3D0_CONTROL0_ZETA_BUFFER_SCREEN           0x00000000
#define NV_D3D0_CONTROL0_ZETA_BUFFER_LINEAR           0x00008000
#define NV_D3D0_CONTROL0_ZETA_COMPARE_FALSE           0x00010000
#define NV_D3D0_CONTROL0_ZETA_COMPARE_LT              0x00020000
#define NV_D3D0_CONTROL0_ZETA_COMPARE_EQ              0x00030000
#define NV_D3D0_CONTROL0_ZETA_COMPARE_LE              0x00040000
#define NV_D3D0_CONTROL0_ZETA_COMPARE_GT              0x00050000
#define NV_D3D0_CONTROL0_ZETA_COMPARE_NE              0x00060000
#define NV_D3D0_CONTROL0_ZETA_COMPARE_GE              0x00070000
#define NV_D3D0_CONTROL0_ZETA_COMPARE_TRUE            0x00080000
#define NV_D3D0_CONTROL0_ZETA_WRITE_NEVER             0x00000000
#define NV_D3D0_CONTROL0_ZETA_WRITE_ALPHA             0x00100000
#define NV_D3D0_CONTROL0_ZETA_WRITE_ALPHA_ZETA        0x00200000
#define NV_D3D0_CONTROL0_ZETA_WRITE_ZETA              0x00300000
#define NV_D3D0_CONTROL0_ZETA_WRITE_ALWAYS            0x00400000
#define NV_D3D0_CONTROL0_COLOR_WRITE_NEVER            0x00000000
#define NV_D3D0_CONTROL0_COLOR_WRITE_ALPHA            0x01000000
#define NV_D3D0_CONTROL0_COLOR_WRITE_ALPHA_ZETA       0x02000000
#define NV_D3D0_CONTROL0_COLOR_WRITE_ZETA             0x03000000
#define NV_D3D0_CONTROL0_COLOR_WRITE_ALWAYS           0x04000000
#define NV_D3D0_CONTROL0_ROP_BLEND_AND                0x00000000
#define NV_D3D0_CONTROL0_ROP_ADD_WITH_SATURATION      0x10000000
#define NV_D3D0_CONTROL0_BLEND_BETA_SRCALPHA          0x00000000
#define NV_D3D0_CONTROL0_BLEND_BETA_DESTCOLOR         0x20000000
#define NV_D3D0_CONTROL0_BLEND_INPUT0_DESTCOLOR       0x00000000
#define NV_D3D0_CONTROL0_BLEND_INPUT0_ZERO            0x40000000
#define NV_D3D0_CONTROL0_BLEND_INPUT1_SRCCOLOR        0x00000000
#define NV_D3D0_CONTROL0_BLEND_INPUT1_ZERO            0x80000000
/* values for Control1() */
#define NV_D3D0_CONTROL1_ALPHA_KEY_MASK               0x000000ff
#define NV_D3D0_CONTROL1_ALPHA_COMPARE_FALSE          0x00000100
#define NV_D3D0_CONTROL1_ALPHA_COMPARE_LT             0x00000200
#define NV_D3D0_CONTROL1_ALPHA_COMPARE_EQ             0x00000300
#define NV_D3D0_CONTROL1_ALPHA_COMPARE_LE             0x00000400
#define NV_D3D0_CONTROL1_ALPHA_COMPARE_GT             0x00000500
#define NV_D3D0_CONTROL1_ALPHA_COMPARE_NE             0x00000600
#define NV_D3D0_CONTROL1_ALPHA_COMPARE_GE             0x00000700
#define NV_D3D0_CONTROL1_ALPHA_COMPARE_TRUE           0x00000800


/* class NV_RENDER_GDI0_RECTANGLE_AND_TEXT */
#define NV_RENDER_GDI0_RECTANGLE_AND_TEXT  75
typedef volatile struct {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS   0104-0107*/
 V032 StopTransfer;            /* writing any value stops transfer  0108-010b*/
 V032 reserved01[0x03d];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE                0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* unpacked RGB or Y                 0300-0303*/
 V032 SetMonochromeFormat;     /* NV_MONOCHROME_FORMAT_             0304-0307*/
 V032 reserved03[(0x03e)-1];
 V032 Color1A;                 /* rectangle color                   03fc-03ff*/
 struct {                      /*                                   0400-    */
  V032 x_y;                    /* S16_S16 in pixels, 0 at top left     0-   3*/
  V032 width_height;           /* U16_U16 in pixels                    4-   7*/
 } UnclippedRectangle[64];     /*                                       -05ff*/
 V032 reserved04[(0x080)-3];
 struct {
  V032 top_left;               /* S16_S16 in pixels, inclusive      07f4-07f7*/
  V032 bottom_right;           /* S16_S16 in pixels, exclusive      07f8-07fb*/
 } ClipB;
 V032 Color1B;                 /* rectangle color                   07fc-07ff*/
 struct {                      /*                                   0800-    */
  V032 top_left;               /* S16_S16 in pixels, inclusive         0-   3*/
  V032 bottom_right;           /* S16_S16 in pixels, exclusive         4-   7*/
 } ClippedRectangle[64];       /*                                       -09ff*/
 V032 reserved05[(0x080)-5];
 struct {
  V032 top_left;               /* S16_S16 in pixels, inclusive      0bec-0bef*/
  V032 bottom_right;           /* S16_S16 in pixels, exclusive      0bf0-0bf3*/
 } ClipC;
 V032 Color1C;                 /* color of 1 pixels                 0bf4-0bf7*/
 V032 SizeC;                   /* height_width U16_U16 in pixels    0bf8-0bfb*/
 V032 PointC;                  /* y_x S16_S16 in pixels             0bfc-0bff*/
 V032 MonochromeColor1C[128];  /* 32 monochrome pixels per write    0c00-0dff*/
 V032 reserved06[(0x080)-6];
 struct {
  V032 top_left;               /* S16_S16 in pixels, inclusive      0fe8-0feb*/
  V032 bottom_right;           /* S16_S16 in pixels, exclusive      0fec-0fef*/
 } ClipD;
 V032 Color1D;                 /* color of 1 pixels                 0ff0-0ff3*/
 V032 SizeInD;                 /* height_width U16_U16 in pixels    0ff4-0ff7*/
 V032 SizeOutD;                /* height_width U16_U16 in pixels    0ff8-0ffb*/
 V032 PointD;                  /* y_x S16_S16 in pixels             0ffc-0fff*/
 V032 MonochromeColor1D[128];  /* 32 monochrome pixels per write    1000-11ff*/
 V032 reserved07[(0x080)-7];
 struct {
  V032 top_left;               /* S16_S16 in pixels, inclusive      13e4-13e7*/
  V032 bottom_right;           /* S16_S16 in pixels, exclusive      13e8-13eb*/
 } ClipE;
 V032 Color0E;                 /* color of 0 pixels                 13ec-13ef*/
 V032 Color1E;                 /* color of 1 pixels                 13f0-13f3*/
 V032 SizeInE;                 /* height_width U16_U16 in pixels    13f4-13f7*/
 V032 SizeOutE;                /* height_width U16_U16 in pixels    13f8-13fb*/
 V032 PointE;                  /* y_x S16_S16 in pixels             13fc-13ff*/
 V032 MonochromeColor01E[128]; /* 32 monochrome pixels per write    1400-15ff*/
 V032 reserved08[0x280];
} NvRenderGdi0RectangleAndText;




//******************************************************************************
// The above classes are no longer supported beginning with NV4 !!!       |
//                                                                       | |
// In order to be able to use this driver with the older chips          | | |
// and resource manager, we include the old obsolete structure            |
// definitions.  That way, we don't have to include the old               |
// NV1 NV32.H file.  We only want one NV32.H file.
//******************************************************************************


//******************************************************************************
// Additional classes required for NV3 follow below:
//******************************************************************************


/* class NV01_IMAGE_PATTERN */
#define  NV01_IMAGE_PATTERN                              (0x00000018)
#define  NV1_IMAGE_PATTERN                               (0x00000018)
/* NvNotification[] elements */
#define NV018_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NV018_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NV018_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NV018_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NV018_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NV018_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NV018_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV018_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 Reserved01[0x01f];
 V032 SetImageOutput;          /* NV1_PATCHCORD_IMAGE               0200-0203*/
 V032 Reserved02[0x03f];
 V032 SetColorFormat;          /* NV018_SET_COLOR_FORMAT_*          0300-0303*/
 V032 SetMonochromeFormat;     /* NV018_SET_MONOCHROME_FORMAT_*     0304-0307*/
 V032 SetMonochromeShape;      /* NV018_SET_MONOCHROME_SHAPE_*      0308-030b*/
 V032 Reserved03[0x001];
 V032 SetMonochromeColor0;     /* color of 0 pixels                 0310-0313*/
 V032 SetMonochromeColor1;     /* color of 1 pixels                 0314-0317*/
 V032 SetMonochromePattern0;   /* first  32 bits of pattern data    0318-031b*/
 V032 SetMonochromePattern1;   /* second 32 bits of pattern data    031c-031f*/
 V032 Reserved05[0x738];
} Nv018Typedef, Nv01ImagePattern;
#define Nv1ImagePattern                                  Nv01ImagePattern
#define nv1ImagePattern                                  nv01ImagePattern
#define NV018_TYPEDEF                                    nv01ImagePattern
/* dma method offsets, fields, and values */
#define NV018_SET_OBJECT                                           (0x00000000)
#define NV018_NO_OPERATION                                         (0x00000100)
#define NV018_NOTIFY                                               (0x00000104)
#define NV018_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_1                           (0x00000001)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_2                           (0x00000002)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_3                           (0x00000003)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_4                           (0x00000004)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_5                           (0x00000005)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_6                           (0x00000006)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_7                           (0x00000007)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_8                           (0x00000008)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_9                           (0x00000009)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_A                           (0x0000000A)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_B                           (0x0000000B)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_C                           (0x0000000C)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_D                           (0x0000000D)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_E                           (0x0000000E)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN_F                           (0x0000000F)
#define NV018_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV018_SET_COLOR_FORMAT                                     (0x00000300)
#define NV018_SET_COLOR_FORMAT_LE_X16A8Y8                          (0x00000001)
#define NV018_SET_COLOR_FORMAT_LE_X16A1R5G5B5                      (0x00000002)
#define NV018_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000003)
#define NV018_SET_MONOCHROME_FORMAT                                (0x00000304)
#define NV018_SET_MONOCHROME_FORMAT_CGA6_M1                        (0x00000001)
#define NV018_SET_MONOCHROME_FORMAT_LE_M1                          (0x00000002)
#define NV018_SET_MONOCHROME_SHAPE                                 (0x00000308)
#define NV018_SET_MONOCHROME_SHAPE_8X_8Y                           (0x00000000)
#define NV018_SET_MONOCHROME_SHAPE_64X_1Y                          (0x00000001)
#define NV018_SET_MONOCHROME_SHAPE_1X_64Y                          (0x00000002)
#define NV018_SET_MONOCHORME_COLOR0                                (0x00000310)
#define NV018_SET_MONOCHORME_COLOR1                                (0x00000314)
#define NV018_SET_MONOCHORME_PATTERN0                              (0x00000318)
#define NV018_SET_MONOCHORME_PATTERN1                              (0x0000031C)


/* class NV01_IMAGE_BLACK_RECTANGLE */
#define  NV01_IMAGE_BLACK_RECTANGLE                      (0x00000019)
#define  NV1_IMAGE_BLACK_RECTANGLE                       (0x00000019)
/* NvNotification[] elements */
#define NV019_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NV019_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NV019_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NV019_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NV019_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NV019_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NV019_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV019_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 Reserved01[0x01f];
 V032 SetImageOutput;          /* NV1_PATCHCORD_IMAGE               0200-0203*/
 V032 Reserved02[0x03f];
 V032 SetPoint;                /* y_x S16_S16                       0300-0303*/
 V032 SetSize;                 /* height_width U16_U16              0304-0307*/
 V032 Reserved03[0x73e];
} Nv019Typedef, Nv01ImageBlackRectangle;
#define Nv1ImageBlackRectangle                           Nv01ImageBlackRectangle
#define nv1ImageBlackRectangle                           nv01ImageBlackRectangle
#define NV019_TYPEDEF                                    nv01ImageBlackRectangle
/* dma method offsets, fields, and values */
#define NV019_SET_OBJECT                                           (0x00000000)
#define NV019_NO_OPERATION                                         (0x00000100)
#define NV019_NOTIFY                                               (0x00000104)
#define NV019_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_1                           (0x00000001)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_2                           (0x00000002)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_3                           (0x00000003)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_4                           (0x00000004)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_5                           (0x00000005)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_6                           (0x00000006)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_7                           (0x00000007)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_8                           (0x00000008)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_9                           (0x00000009)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_A                           (0x0000000A)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_B                           (0x0000000B)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_C                           (0x0000000C)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_D                           (0x0000000D)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_E                           (0x0000000E)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN_F                           (0x0000000F)
#define NV019_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV019_SET_POINT                                            (0x00000300)
#define NV019_SET_POINT_X                                          15:0
#define NV019_SET_POINT_Y                                          31:16
#define NV019_SET_SIZE                                             (0x00000304)
#define NV019_SET_SIZE_WIDTH                                       15:0
#define NV019_SET_SIZE_HEIGHT                                      31:16



/* class NV01_RENDER_SOLID_RECTANGLE */
#define  NV01_RENDER_SOLID_RECTANGLE                     (0x0000001E)
#define  NV1_RENDER_SOLID_RECTANGLE                      (0x0000001E)
/* NvNotification[] elements */
#define NV01E_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NV01E_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NV01E_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NV01E_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NV01E_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NV01E_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NV01E_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV01E_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 SetContextClipRectangle; /* NV1_IMAGE_BLACK_RECTANGLE         0184-0187*/
 V032 SetContextPattern;       /* NV1_IMAGE_PATTERN                 0188-018b*/
 V032 SetContextRop;           /* NV3_CONTEXT_ROP                   018c-018f*/
 V032 SetContextBeta1;         /* NV1_BETA_SOLID                    0190-0193*/
 V032 SetContextSurface;       /* NV3_CONTEXT_SURFACE_0             0194-0197*/
 V032 Reserved01[0x059];
 V032 SetOperation;            /* NV01E_SET_OPERATION_*             02fc-02ff*/
 V032 SetColorFormat;          /* NV01E_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Color;                   /* source color                      0304-0307*/
 V032 Reserved02[0x03e];
 struct {                      /* start of aliased methods in array 0400-    */
  V032 point;                  /* y_x S16_S16                          0-   3*/
  V032 size;                   /* height_width U16_U16                 4-   7*/
 } Rectangle[16];              /* end of method in array in array       -047f*/
 V032 Reserved03[0x6e0];
} Nv01eTypedef, Nv01RenderSolidRectangle;
#define Nv1RenderSolidRectangle                          Nv01RenderSolidRectangle
#define nv1RenderSolidRectangle                          nv01RenderSolidRectangle
#define NV01E_TYPEDEF                                    nv01RenderSolidRectangle
/* dma method offsets, fields, and values */
#define NV01E_SET_OBJECT                                           (0x00000000)
#define NV01E_NO_OPERATION                                         (0x00000100)
#define NV01E_NOTIFY                                               (0x00000104)
#define NV01E_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_1                           (0x00000001)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_2                           (0x00000002)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_3                           (0x00000003)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_4                           (0x00000004)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_5                           (0x00000005)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_6                           (0x00000006)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_7                           (0x00000007)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_8                           (0x00000008)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_9                           (0x00000009)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_A                           (0x0000000A)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_B                           (0x0000000B)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_C                           (0x0000000C)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_D                           (0x0000000D)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_E                           (0x0000000E)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN_F                           (0x0000000F)
#define NV01E_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV01E_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000184)
#define NV01E_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV01E_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV01E_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV01E_SET_CONTEXT_SURFACE                                  (0x00000194)
#define NV01E_SET_OPERATION                                        (0x000002FC)
#define NV01E_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV01E_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV01E_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV01E_SET_COLOR_FORMAT                                     (0x00000300)
#define NV01E_SET_COLOR_FORMAT_LE_X24Y8                            (0x00000001)
#define NV01E_SET_COLOR_FORMAT_LE_X17R5G5B5                        (0x00000002)
#define NV01E_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000003)
#define NV01E_SET_COLOR_FORMAT_LE_X16Y16                           (0x00000004)
#define NV01E_COLOR                                                (0x00000304)
#define NV01E_RECTANGLE(a)                                         (0x00000400\
                                                                   +(a)*0x0008)
#define NV01E_RECTANGLE_POINT(a)                                   (0x00000400\
                                                                   +(a)*0x0008)
#define NV01E_RECTANGLE_POINT_X                                    15:0
#define NV01E_RECTANGLE_POINT_Y                                    31:16
#define NV01E_RECTANGLE_SIZE(a)                                    (0x00000404\
                                                                   +(a)*0x0008)
#define NV01E_RECTANGLE_SIZE_WIDTH                                 15:0
#define NV01E_RECTANGLE_SIZE_HEIGHT                                31:16


/* class NV01_IMAGE_BLIT */
#define  NV01_IMAGE_BLIT                                 (0x0000001F)
#define  NV1_IMAGE_BLIT                                  (0x0000001F)
/* NvNotification[] elements */
#define NV01F_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NV01F_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NV01F_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NV01F_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NV01F_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NV01F_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NV01F_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV01F_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 SetContextColorKey;      /* NV1_IMAGE_SOLID                   0184-0187*/
 V032 SetContextClipRectangle; /* NV1_IMAGE_BLACK_RECTANGLE         0188-018b*/
 V032 SetContextPattern;       /* NV1_IMAGE_PATTERN                 018c-018f*/
 V032 SetContextRop;           /* NV3_CONTEXT_ROP                   0190-0193*/
 V032 SetContextBeta1;         /* NV1_BETA_SOLID                    0194-0197*/
 V032 SetContextSurfaceSource; /* NV3_CONTEXT_SURFACE_1             0198-019b*/
 V032 SetContextSurfaceDestin; /* NV3_CONTEXT_SURFACE_0             019c-019f*/
 V032 Reserved01[0x057];
 V032 SetOperation;            /* NV01F_SET_OPERATION_*             02fc-02ff*/
 V032 ControlPointIn;          /* y_x U16_U16, pixels, top left src 0300-0303*/
 V032 ControlPointOut;         /* y_x S16_S16, pixels, top left dst 0304-0307*/
 V032 Size;                    /* height_width U16_U16 in pixels    0308-030b*/
 V032 Reserved02[0x73d];
} Nv01fTypedef, Nv01ImageBlit;
#define Nv1ImageBlit                                     Nv01ImageBlit
#define nv1ImageBlit                                     nv01ImageBlit
#define NV01F_TYPEDEF                                    nv01ImageBlit
/* dma method offsets, fields, and values */
#define NV01F_SET_OBJECT                                           (0x00000000)
#define NV01F_NO_OPERATION                                         (0x00000100)
#define NV01F_NOTIFY                                               (0x00000104)
#define NV01F_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_1                           (0x00000001)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_2                           (0x00000002)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_3                           (0x00000003)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_4                           (0x00000004)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_5                           (0x00000005)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_6                           (0x00000006)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_7                           (0x00000007)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_8                           (0x00000008)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_9                           (0x00000009)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_A                           (0x0000000A)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_B                           (0x0000000B)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_C                           (0x0000000C)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_D                           (0x0000000D)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_E                           (0x0000000E)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN_F                           (0x0000000F)
#define NV01F_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV01F_SET_CONTEXT_COLOR_KEY                                (0x00000184)
#define NV01F_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000188)
#define NV01F_SET_CONTEXT_PATTERN                                  (0x0000018C)
#define NV01F_SET_CONTEXT_ROP                                      (0x00000190)
#define NV01F_SET_CONTEXT_BETA1                                    (0x00000194)
#define NV01F_SET_CONTEXT_SURFACE_SOURCE                           (0x00000198)
#define NV01F_SET_CONTEXT_SURFACE_DESTIN                           (0x0000019C)
#define NV01F_SET_OPERATION                                        (0x000002FC)
#define NV01F_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV01F_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV01F_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV01F_CONTROL_POINT_IN                                     (0x00000300)
#define NV01F_CONTROL_POINT_IN_X                                   15:0
#define NV01F_CONTROL_POINT_IN_Y                                   31:16
#define NV01F_CONTROL_POINT_OUT                                    (0x00000304)
#define NV01F_CONTROL_POINT_OUT_X                                  15:0
#define NV01F_CONTROL_POINT_OUT_Y                                  31:16
#define NV01F_SIZE                                                 (0x00000308)
#define NV01F_SIZE_WIDTH                                           15:0
#define NV01F_SIZE_HEIGHT                                          31:16


/* class NV01_IMAGE_FROM_CPU */
#define  NV01_IMAGE_FROM_CPU                             (0x00000021)
#define  NV1_IMAGE_FROM_CPU                              (0x00000021)
/* NvNotification[] elements */
#define NV021_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NV021_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NV021_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NV021_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NV021_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NV021_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NV021_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV021_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 SetContextColorKey;      /* NV1_IMAGE_SOLID                   0184-0187*/
 V032 SetContextClipRectangle; /* NV1_IMAGE_BLACK_RECTANGLE         0188-018b*/
 V032 SetContextPattern;       /* NV1_IMAGE_PATTERN                 018c-018f*/
 V032 SetContextRop;           /* NV3_CONTEXT_ROP                   0190-0193*/
 V032 SetContextBeta1;         /* NV1_BETA_SOLID                    0194-0197*/
 V032 SetContextSurface;       /* NV3_CONTEXT_SURFACE_0             0198-019b*/
 V032 Reserved01[0x058];
 V032 SetOperation;            /* NV021_SET_OPERATION_*             02fc-02ff*/
 V032 SetColorFormat;          /* NV021_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Point;                   /* y_x S16_S16 in pixels             0304-0307*/
 V032 SizeOut;                 /* height_width U16_U16, pixels, dst 0308-030b*/
 V032 SizeIn;                  /* height_width U16_U16, pixels, src 030c-030f*/
 V032 Reserved02[0x03c];
 V032 Color[32];               /* packed pixel(s) to add to image   0400-047f*/
 V032 Reserved03[0x6e0];
} Nv021Typedef, Nv01ImageFromCpu;
#define Nv1ImageFromCpu                                  Nv01ImageFromCpu
#define nv1ImageFromCpu                                  nv01ImageFromCpu
#define NV021_TYPEDEF                                    nv01ImageFromCpu
/* dma method offsets, fields, and values */
#define NV021_SET_OBJECT                                           (0x00000000)
#define NV021_NO_OPERATION                                         (0x00000100)
#define NV021_NOTIFY                                               (0x00000104)
#define NV021_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_1                           (0x00000001)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_2                           (0x00000002)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_3                           (0x00000003)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_4                           (0x00000004)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_5                           (0x00000005)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_6                           (0x00000006)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_7                           (0x00000007)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_8                           (0x00000008)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_9                           (0x00000009)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_A                           (0x0000000A)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_B                           (0x0000000B)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_C                           (0x0000000C)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_D                           (0x0000000D)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_E                           (0x0000000E)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN_F                           (0x0000000F)
#define NV021_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV021_SET_CONTEXT_COLOR_KEY                                (0x00000184)
#define NV021_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000188)
#define NV021_SET_CONTEXT_PATTERN                                  (0x0000018C)
#define NV021_SET_CONTEXT_ROP                                      (0x00000190)
#define NV021_SET_CONTEXT_BETA1                                    (0x00000194)
#define NV021_SET_CONTEXT_SURFACE                                  (0x00000198)
#define NV021_SET_OPERATION                                        (0x000002FC)
#define NV021_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV021_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV021_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV021_SET_COLOR_FORMAT                                     (0x00000300)
#define NV021_SET_COLOR_FORMAT_LE_Y8                               (0x00000001)
#define NV021_SET_COLOR_FORMAT_LE_A1R5G5B5                         (0x00000002)
#define NV021_SET_COLOR_FORMAT_LE_X1R5G5B5                         (0x00000003)
#define NV021_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000004)
#define NV021_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000005)
#define NV021_POINT                                                (0x00000304)
#define NV021_POINT_X                                              15:0
#define NV021_POINT_Y                                              31:16
#define NV021_SIZE_OUT                                             (0x00000308)
#define NV021_SIZE_OUT_WIDTH                                       15:0
#define NV021_SIZE_OUT_HEIGHT                                      31:16
#define NV021_SIZE_IN                                              (0x0000030C)
#define NV021_SIZE_IN_WIDTH                                        15:0
#define NV021_SIZE_IN_HEIGHT                                       31:16
#define NV021_COLOR(a)                                             (0x00000400\
                                                                   +(a)*0x0004)

/* class NV03_CONTEXT_ROP */
#define  NV03_CONTEXT_ROP                                (0x00000043)
#define  NV3_CONTEXT_ROP                                 (0x00000043)
/* NvNotification[] elements */
#define NV043_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NV043_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NV043_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NV043_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NV043_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NV043_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NV043_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV043_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 Reserved01[0x05f];
 V032 SetRop5;                 /* 32-bit ROP5                       0300-0303*/
 V032 Reserved02[0x73f];
} Nv043Typedef, Nv03ContextRop;
#define Nv3ContextRop                                    Nv03ContextRop
#define nv3ContextRop                                    nv03ContextRop
#define NV043_TYPEDEF                                    nv03ContextRop
/* dma method offsets, fields, and values */
#define NV043_SET_OBJECT                                           (0x00000000)
#define NV043_NO_OPERATION                                         (0x00000100)
#define NV043_NOTIFY                                               (0x00000104)
#define NV043_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_1                           (0x00000001)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_2                           (0x00000002)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_3                           (0x00000003)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_4                           (0x00000004)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_5                           (0x00000005)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_6                           (0x00000006)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_7                           (0x00000007)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_8                           (0x00000008)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_9                           (0x00000009)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_A                           (0x0000000A)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_B                           (0x0000000B)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_C                           (0x0000000C)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_D                           (0x0000000D)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_E                           (0x0000000E)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN_F                           (0x0000000F)
#define NV043_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV043_SET_ROP5                                             (0x00000300)



/* class NV03_DX3_TEXTURED_TRIANGLE */
#define  NV03_DX3_TEXTURED_TRIANGLE                      (0x00000048)
#define  NV3_DX3_TEXTURED_TRIANGLE                       (0x00000048)
/* NvNotification[] elements */
#define NV048_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NV048_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NV048_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NV048_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NV048_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NV048_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NV048_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV048_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 SetContextDmaTexture;    /* NV1_CONTEXT_DMA_FROM_MEMORY       0184-0187*/
 V032 SetContextClipRectangle; /* NV1_IMAGE_BLACK_RECTANGLE         0188-018b*/
 V032 SetContextSurfaceColor;  /* NV3_CONTEXT_SURFACE_2             018c-018f*/
 V032 SetContextSurfaceZeta;   /* NV3_CONTEXT_SURFACE_3             0190-0193*/
 V032 Reserved01[0x05c];
 U032 TextureOffset;           /* offset in bytes                   0304-0307*/
 V032 TextureFormat;           /* see text                          0308-030b*/
 V032 TextureFilter;           /* ignore_sizeAdjust_spreadY_spreadX 030c-030f*/
 V032 FogColor;                /* X8R8G8B8 fog color                0310-0313*/
 V032 Control0;                /* see text                          0314-0317*/
 V032 Control1;                /* alphafunc_alpharef V24_U8         0318-031b*/
 V032 Reserved02[0x339];
 struct {                      /* start of method in array          1000-    */
  V032 specular;               /* fog_i5-i0, U0d8_U4_U4_U4_U4_U4_U4   00-  03*/
  V032 color;                  /* X8R8G8B8 or A8R8G8B8                04-  07*/
  F032 sx;                     /* screen x coordinate                 08-  0b*/
  F032 sy;                     /* screen y coordinate                 0c-  0f*/
  F032 sz;                     /* screen z coordinate                 10-  13*/
  F032 rhw;                    /* reciprocal homogeneous W (1/W)      14-  17*/
  F032 tu;                     /* texture u coordinate                18-  1b*/
  F032 tv;                     /* texture v coordinate                1c-  1f*/
 } Tlvertex[128];              /* end of method in array                -1fff*/
} Nv048Typedef, Nv03Dx3TexturedTriangle;
#define Nv3Dx3TexturedTriangle                           Nv03Dx3TexturedTriangle
#define nv3Dx3TexturedTriangle                           nv03Dx3TexturedTriangle
#define NV048_TYPEDEF                                    nv03Dx3TexturedTriangle
/* dma method offsets, fields, and values */
#define NV048_SET_OBJECT                                           (0x00000000)
#define NV048_NO_OPERATION                                         (0x00000100)
#define NV048_NOTIFY                                               (0x00000104)
#define NV048_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_1                           (0x00000001)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_2                           (0x00000002)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_3                           (0x00000003)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_4                           (0x00000004)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_5                           (0x00000005)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_6                           (0x00000006)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_7                           (0x00000007)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_8                           (0x00000008)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_9                           (0x00000009)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_A                           (0x0000000A)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_B                           (0x0000000B)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_C                           (0x0000000C)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_D                           (0x0000000D)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_E                           (0x0000000E)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN_F                           (0x0000000F)
#define NV048_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV048_SET_CONTEXT_DMA_TEXTURE                              (0x00000184)
#define NV048_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000188)
#define NV048_SET_CONTEXT_SURFACE_COLOR                            (0x0000018C)
#define NV048_SET_CONTEXT_SURFACE_ZETA                             (0x00000190)
#define NV048_TEXTURE_OFFSET                                       (0x00000304)
#define NV048_TEXTURE_FORMAT                                       (0x00000308)
#define NV048_TEXTURE_FORMAT_VALUE                                 15:0
#define NV048_TEXTURE_FORMAT_KEY                                   19:16
#define NV048_TEXTURE_FORMAT_KEY_DISABLED                          (0x00000000)
#define NV048_TEXTURE_FORMAT_KEY_ENABLED                           (0x00000001)
#define NV048_TEXTURE_FORMAT_COLOR                                 23:20
#define NV048_TEXTURE_FORMAT_COLOR_LE_A1R5G5B5                     (0x00000000)
#define NV048_TEXTURE_FORMAT_COLOR_LE_X1R5G5B5                     (0x00000001)
#define NV048_TEXTURE_FORMAT_COLOR_LE_A4R4G4B4                     (0x00000002)
#define NV048_TEXTURE_FORMAT_COLOR_LE_R5G6B5                       (0x00000003)
#define NV048_TEXTURE_FORMAT_SIZE_MIN                              27:24
#define NV048_TEXTURE_FORMAT_SIZE_MIN_4X4                          (0x00000002)
#define NV048_TEXTURE_FORMAT_SIZE_MIN_8X8                          (0x00000003)
#define NV048_TEXTURE_FORMAT_SIZE_MIN_16X16                        (0x00000004)
#define NV048_TEXTURE_FORMAT_SIZE_MIN_32X32                        (0x00000005)
#define NV048_TEXTURE_FORMAT_SIZE_MIN_64X64                        (0x00000006)
#define NV048_TEXTURE_FORMAT_SIZE_MIN_128X128                      (0x00000007)
#define NV048_TEXTURE_FORMAT_SIZE_MIN_256X256                      (0x00000008)
#define NV048_TEXTURE_FORMAT_SIZE_MIN_512X512                      (0x00000009)
#define NV048_TEXTURE_FORMAT_SIZE_MIN_1024X1024                    (0x0000000A)
#define NV048_TEXTURE_FORMAT_SIZE_MIN_2048X2048                    (0x0000000B)
#define NV048_TEXTURE_FORMAT_SIZE_MAX                              31:28
#define NV048_TEXTURE_FORMAT_SIZE_MAX_4X4                          (0x00000002)
#define NV048_TEXTURE_FORMAT_SIZE_MAX_8X8                          (0x00000003)
#define NV048_TEXTURE_FORMAT_SIZE_MAX_16X16                        (0x00000004)
#define NV048_TEXTURE_FORMAT_SIZE_MAX_32X32                        (0x00000005)
#define NV048_TEXTURE_FORMAT_SIZE_MAX_64X64                        (0x00000006)
#define NV048_TEXTURE_FORMAT_SIZE_MAX_128X128                      (0x00000007)
#define NV048_TEXTURE_FORMAT_SIZE_MAX_256X256                      (0x00000008)
#define NV048_TEXTURE_FORMAT_SIZE_MAX_512X512                      (0x00000009)
#define NV048_TEXTURE_FORMAT_SIZE_MAX_1024X1024                    (0x0000000A)
#define NV048_TEXTURE_FORMAT_SIZE_MAX_2048X2048                    (0x0000000B)
#define NV048_TEXTURE_FILTER                                       (0x0000030C)
#define NV048_TEXTURE_FILTER_SPREAD_X                              4:0
#define NV048_TEXTURE_FILTER_IGNORE0                               7:5
#define NV048_TEXTURE_FILTER_SPREAD_Y                              12:8
#define NV048_TEXTURE_FILTER_IGNORE1                               15:13
#define NV048_TEXTURE_FILTER_SIZE_ADJUST                           23:16
#define NV048_TEXTURE_FILTER_IGNORE2                               31:24
#define NV048_FOG_COLOR                                            (0x00000310)
#define NV048_CONTROL0                                             (0x00000314)
#define NV048_CONTROL0_INTERPOLATOR                                3:0
#define NV048_CONTROL0_INTERPOLATOR_ZOH_CORNER                     (0x00000000)
#define NV048_CONTROL0_INTERPOLATOR_ZOH_CENTER                     (0x00000001)
#define NV048_CONTROL0_INTERPOLATOR_FOH_CENTER                     (0x00000002)
#define NV048_CONTROL0_WRAP_U                                      5:4
#define NV048_CONTROL0_WRAP_U_CYLINDRICAL                          (0x00000000)
#define NV048_CONTROL0_WRAP_U_WRAP                                 (0x00000001)
#define NV048_CONTROL0_WRAP_U_MIRROR                               (0x00000002)
#define NV048_CONTROL0_WRAP_U_CLAMP                                (0x00000003)
#define NV048_CONTROL0_WRAP_V                                      7:6
#define NV048_CONTROL0_WRAP_V_CYLINDRICAL                          (0x00000000)
#define NV048_CONTROL0_WRAP_V_WRAP                                 (0x00000001)
#define NV048_CONTROL0_WRAP_V_MIRROR                               (0x00000002)
#define NV048_CONTROL0_WRAP_V_CLAMP                                (0x00000003)
#define NV048_CONTROL0_COLOR_FORMAT                                9:8
#define NV048_CONTROL0_COLOR_FORMAT_LE_X8R8G8B8                    (0x00000000)
#define NV048_CONTROL0_COLOR_FORMAT_LE_A8R8G8B8                    (0x00000001)
#define NV048_CONTROL0_SOURCE_COLOR                                11:10
#define NV048_CONTROL0_SOURCE_COLOR_NORMAL                         (0x00000000)
#define NV048_CONTROL0_SOURCE_COLOR_COLOR_INVERSE                  (0x00000001)
#define NV048_CONTROL0_SOURCE_COLOR_ALPHA_INVERSE                  (0x00000002)
#define NV048_CONTROL0_SOURCE_COLOR_ALPHA_ONE                      (0x00000003)
#define NV048_CONTROL0_CULLING                                     14:12
#define NV048_CONTROL0_CULLING_NONE                                (0x00000001)
#define NV048_CONTROL0_CULLING_COUNTERCLOCKWISE                    (0x00000002)
#define NV048_CONTROL0_CULLING_CLOCKWISE                           (0x00000003)
#define NV048_CONTROL0_Z_PERSPECTIVE_ENABLE                        15:15
#define NV048_CONTROL0_Z_PERSPECTIVE_ENABLE_FALSE                  (0x00000000)
#define NV048_CONTROL0_Z_PERSPECTIVE_ENABLE_TRUE                   (0x00000001)
#define NV048_CONTROL0_ZFUNC                                       19:16
#define NV048_CONTROL0_ZFUNC_NEVER                                 (0x00000001)
#define NV048_CONTROL0_ZFUNC_LESS                                  (0x00000002)
#define NV048_CONTROL0_ZFUNC_EQUAL                                 (0x00000003)
#define NV048_CONTROL0_ZFUNC_LESSEQUAL                             (0x00000004)
#define NV048_CONTROL0_ZFUNC_GREATER                               (0x00000005)
#define NV048_CONTROL0_ZFUNC_NOTEQUAL                              (0x00000006)
#define NV048_CONTROL0_ZFUNC_GREATEREQUAL                          (0x00000007)
#define NV048_CONTROL0_ZFUNC_ALWAYS                                (0x00000008)
#define NV048_CONTROL0_ZETA_WRITE                                  23:20
#define NV048_CONTROL0_ZETA_WRITE_NEVER                            (0x00000000)
#define NV048_CONTROL0_ZETA_WRITE_ALPHA                            (0x00000001)
#define NV048_CONTROL0_ZETA_WRITE_ALPHA_ZETA                       (0x00000002)
#define NV048_CONTROL0_ZETA_WRITE_ZETA                             (0x00000003)
#define NV048_CONTROL0_ZETA_WRITE_ALWAYS                           (0x00000004)
#define NV048_CONTROL0_COLOR_WRITE                                 27:24
#define NV048_CONTROL0_COLOR_WRITE_NEVER                           (0x00000000)
#define NV048_CONTROL0_COLOR_WRITE_ALPHA                           (0x00000001)
#define NV048_CONTROL0_COLOR_WRITE_ALPHA_ZETA                      (0x00000002)
#define NV048_CONTROL0_COLOR_WRITE_ZETA                            (0x00000003)
#define NV048_CONTROL0_COLOR_WRITE_ALWAYS                          (0x00000004)
#define NV048_CONTROL0_ROP                                         28:28
#define NV048_CONTROL0_ROP_BLEND_AND                               (0x00000000)
#define NV048_CONTROL0_ROP_ADD_WITH_SATURATION                     (0x00000001)
#define NV048_CONTROL0_BLEND_BETA                                  29:29
#define NV048_CONTROL0_BLEND_BETA_SRCALPHA                         (0x00000000)
#define NV048_CONTROL0_BLEND_BETA_DESTCOLOR                        (0x00000001)
#define NV048_CONTROL0_BLEND_INPUT0                                30:30
#define NV048_CONTROL0_BLEND_INPUT0_DESTCOLOR                      (0x00000000)
#define NV048_CONTROL0_BLEND_INPUT0_ZERO                           (0x00000001)
#define NV048_CONTROL0_BLEND_INPUT1                                31:31
#define NV048_CONTROL0_BLEND_INPUT1_SRCCOLOR                       (0x00000000)
#define NV048_CONTROL0_BLEND_INPUT1_ZERO                           (0x00000001)
#define NV048_CONTROL1                                             (0x00000318)
#define NV048_CONTROL1_ALPHAREF                                    7:0
#define NV048_CONTROL1_ALPHAFUNC                                   31:8
#define NV048_CONTROL1_ALPHAFUNC_NEVER                             (0x00000001)
#define NV048_CONTROL1_ALPHAFUNC_LESS                              (0x00000002)
#define NV048_CONTROL1_ALPHAFUNC_EQUAL                             (0x00000003)
#define NV048_CONTROL1_ALPHAFUNC_LESSEQUAL                         (0x00000004)
#define NV048_CONTROL1_ALPHAFUNC_GREATER                           (0x00000005)
#define NV048_CONTROL1_ALPHAFUNC_NOTEQUAL                          (0x00000006)
#define NV048_CONTROL1_ALPHAFUNC_GREATEREQUAL                      (0x00000007)
#define NV048_CONTROL1_ALPHAFUNC_ALWAYS                            (0x00000008)

#define NV048_TLVERTEX(a)                                          (0x00001000\
                                                                   +(a)*0x0020)
#define NV048_TLVERTEX_SPECULAR(a)                                 (0x00001000\
                                                                   +(a)*0x0020)
#define NV048_TLVERTEX_SPECULAR_I0                                 3:0
#define NV048_TLVERTEX_SPECULAR_I1                                 7:4
#define NV048_TLVERTEX_SPECULAR_I2                                 11:8
#define NV048_TLVERTEX_SPECULAR_I3                                 15:12
#define NV048_TLVERTEX_SPECULAR_I4                                 19:16
#define NV048_TLVERTEX_SPECULAR_I5                                 23:20
#define NV048_TLVERTEX_SPECULAR_FOG                                31:24
#define NV048_TLVERTEX_COLOR(a)                                    (0x00001004\
                                                                   +(a)*0x0020)
#define NV048_TLVERTEX_SX(a)                                       (0x00001008\
                                                                   +(a)*0x0020)
#define NV048_TLVERTEX_SY(a)                                       (0x0000100C\
                                                                   +(a)*0x0020)
#define NV048_TLVERTEX_SZ(a)                                       (0x00001010\
                                                                   +(a)*0x0020)
#define NV048_TLVERTEX_RHW(a)                                      (0x00001014\
                                                                   +(a)*0x0020)
#define NV048_TLVERTEX_TU(a)                                       (0x00001018\
                                                                   +(a)*0x0020)
#define NV048_TLVERTEX_TV(a)                                       (0x0000101C\
                                                                   +(a)*0x0020)


/* class NV03_GDI_RECTANGLE_TEXT */
#define  NV03_GDI_RECTANGLE_TEXT                         (0x0000004B)
#define  NV3_GDI_RECTANGLE_TEXT                          (0x0000004B)
/* NvNotification[] elements */
#define NV04B_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NV04B_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NV04B_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NV04B_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NV04B_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NV04B_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NV04B_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV04B_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 SetContextPattern;       /* NV1_IMAGE_PATTERN                 0184-0187*/
 V032 SetContextRop;           /* NV3_CONTEXT_ROP                   0188-018b*/
 V032 SetContextBeta1;         /* NV1_BETA_SOLID                    018c-018f*/
 V032 SetContextSurface;       /* NV3_CONTEXT_SURFACE_0             0190-0193*/
 V032 Reserved01[0x05a];
 V032 SetOperation;            /* NV04B_SET_OPERATION_*             02fc-02ff*/
 V032 SetColorFormat;          /* NV04B_SET_COLOR_FORMAT_*          0300-0303*/
 V032 SetMonochromeFormat;     /* NV04B_SET_MONOCHROME_FORMAT_*     0304-0307*/
 V032 Reserved02[0x03d];
 V032 Color1A;                 /* rectangle color                   03fc-03ff*/
 struct {                      /* start of aliased methods in array 0400-    */
  V032 point;                  /* x_y S16_S16 in pixels                0-   3*/
  V032 size;                   /* width_height U16_U16 in pixels       4-   7*/
 } UnclippedRectangle[64];     /* end of aliased methods in array       -05ff*/
 V032 Reserved03[0x07D];
 V032 ClipPoint0B;             /* top_left S16_S16 in pixels        07f4-07f7*/
 V032 ClipPoint1B;             /* bottom_right S16_S16 in pixels    07f8-07fb*/
 V032 Color1B;                 /* rectangle color                   07fc-07ff*/
 struct {                      /* start of aliased methods in array 0800-    */
  V032 point0;                 /* top_left S16_S16 in pixels           0-   3*/
  V032 point1;                 /* bottom_right S16_S16 in pixels       4-   7*/
 } ClippedRectangle[64];       /* end of aliased methods in array       -09ff*/
 V032 Reserved04[0x07B];
 V032 ClipPoint0C;             /* top_left S16_S16 in pixels        0bec-0bef*/
 V032 ClipPoint1C;             /* bottom_right S16_S16 in pixels    0bf0-0bf3*/
 V032 Color1C;                 /* color of 1 pixels                 0bf4-0bf7*/
 V032 SizeC;                   /* height_width U16_U16 in pixels    0bf8-0bfb*/
 V032 PointC;                  /* y_x S16_S16 in pixels             0bfc-0bff*/
 V032 MonochromeColor1C[128];  /* 32 monochrome pixels per write    0c00-0dff*/
 V032 Reserved05[0x07A];
 V032 ClipPoint0D;             /* top_left S16_S16 in pixels        0fe8-0feb*/
 V032 ClipPoint1D;             /* bottom_right S16_S16 in pixels    0fec-0fef*/
 V032 Color1D;                 /* color of 1 pixels                 0ff0-0ff3*/
 V032 SizeInD;                 /* height_width U16_U16 in pixels    0ff4-0ff7*/
 V032 SizeOutD;                /* height_width U16_U16 in pixels    0ff8-0ffb*/
 V032 PointD;                  /* y_x S16_S16 in pixels             0ffc-0fff*/
 V032 MonochromeColor1D[128];  /* 32 monochrome pixels per write    1000-11ff*/
 V032 Reserved06[0x079];
 V032 ClipPoint0E;             /* top_left S16_S16 in pixels        13e4-13e7*/
 V032 ClipPoint1E;             /* bottom_right S16_S16 in pixels    13e8-13eb*/
 V032 Color0E;                 /* color of 0 pixels                 13ec-13ef*/
 V032 Color1E;                 /* color of 1 pixels                 13f0-13f3*/
 V032 SizeInE;                 /* height_width U16_U16 in pixels    13f4-13f7*/
 V032 SizeOutE;                /* height_width U16_U16 in pixels    13f8-13fb*/
 V032 PointE;                  /* y_x S16_S16 in pixels             13fc-13ff*/
 V032 MonochromeColor01E[128]; /* 32 monochrome pixels per write    1400-15ff*/
 V032 Reserved07[0x280];
} Nv04bTypedef, Nv03GdiRectangleText;
#define Nv3GdiRectangleText                              Nv03GdiRectangleText
#define nv3GdiRectangleText                              nv03GdiRectangleText
#define NV04B_TYPEDEF                                    nv03GdiRectangleText
/* dma method offsets, fields, and values */
#define NV04B_SET_OBJECT                                           (0x00000000)
#define NV04B_NO_OPERATION                                         (0x00000100)
#define NV04B_NOTIFY                                               (0x00000104)
#define NV04B_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_1                           (0x00000001)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_2                           (0x00000002)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_3                           (0x00000003)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_4                           (0x00000004)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_5                           (0x00000005)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_6                           (0x00000006)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_7                           (0x00000007)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_8                           (0x00000008)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_9                           (0x00000009)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_A                           (0x0000000A)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_B                           (0x0000000B)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_C                           (0x0000000C)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_D                           (0x0000000D)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_E                           (0x0000000E)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN_F                           (0x0000000F)
#define NV04B_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV04B_SET_CONTEXT_PATTERN                                  (0x00000184)
#define NV04B_SET_CONTEXT_ROP                                      (0x00000188)
#define NV04B_SET_CONTEXT_BETA1                                    (0x0000018C)
#define NV04B_SET_CONTEXT_SURFACE                                  (0x00000190)
#define NV04B_SET_OPERATION                                        (0x000002FC)
#define NV04B_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV04B_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV04B_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV04B_SET_COLOR_FORMAT                                     (0x00000300)
#define NV04B_SET_COLOR_FORMAT_LE_X24Y8                            (0x00000001)
#define NV04B_SET_COLOR_FORMAT_LE_X17R5G5B5                        (0x00000002)
#define NV04B_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000003)
#define NV04B_SET_MONOCHROME_FORMAT                                (0x00000304)
#define NV04B_SET_MONOCHROME_FORMAT_CGA6_M1                        (0x00000001)
#define NV04B_SET_MONOCHROME_FORMAT_LE_M1                          (0x00000002)
#define NV04B_COLOR1_A                                             (0x000003FC)
#define NV04B_UNCLIPPED_RECTANGLE(a)                               (0x00000400\
                                                                   +(a)*0x0008)
#define NV04B_UNCLIPPED_RECTANGLE_POINT(a)                         (0x00000400\
                                                                   +(a)*0x0008)
#define NV04B_UNCLIPPED_RECTANGLE_POINT_X                          15:0
#define NV04B_UNCLIPPED_RECTANGLE_POINT_Y                          31:16
#define NV04B_UNCLIPPED_RECTANGLE_SIZE(a)                          (0x00000404\
                                                                   +(a)*0x0008)
#define NV04B_UNCLIPPED_RECTANGLE_SIZE_WIDTH                       15:0
#define NV04B_UNCLIPPED_RECTANGLE_SIZE_HEIGHT                      31:16
#define NV04B_CLIP_POINT0_B                                        (0x000007F4)
#define NV04B_CLIP_POINT0_B_LEFT                                   15:0
#define NV04B_CLIP_POINT0_B_TOP                                    31:16
#define NV04B_CLIP_POINT1_B                                        (0x000007F8)
#define NV04B_CLIP_POINT1_B_RIGHT                                  15:0
#define NV04B_CLIP_POINT1_B_BOTTOM                                 31:16
#define NV04B_COLOR1_B                                             (0x000007FC)
#define NV04B_CLIPPED_RECTANGLE(a)                                 (0x00000800\
                                                                   +(a)*0x0008)
#define NV04B_CLIPPED_RECTANGLE_POINT0(a)                          (0x00000800\
                                                                   +(a)*0x0008)
#define NV04B_CLIPPED_RECTANGLE_POINT0_LEFT                        15:0
#define NV04B_CLIPPED_RECTANGLE_POINT0_TOP                         31:16
#define NV04B_CLIPPED_RECTANGLE_POINT1(a)                          (0x00000804\
                                                                   +(a)*0x0008)
#define NV04B_CLIPPED_RECTANGLE_POINT1_RIGHT                       15:0
#define NV04B_CLIPPED_RECTANGLE_POINT1_BOTTOM                      31:16
#define NV04B_CLIP_POINT0_C                                        (0x00000BEC)
#define NV04B_CLIP_POINT0_C_LEFT                                   15:0
#define NV04B_CLIP_POINT0_C_TOP                                    31:16
#define NV04B_CLIP_POINT1_C                                        (0x00000BF0)
#define NV04B_CLIP_POINT1_C_RIGHT                                  15:0
#define NV04B_CLIP_POINT1_C_BOTTOM                                 31:16
#define NV04B_COLOR1_C                                             (0x00000BF4)
#define NV04B_SIZE_C                                               (0x00000BF8)
#define NV04B_SIZE_C_WIDTH                                         15:0
#define NV04B_SIZE_C_HEIGHT                                        31:16
#define NV04B_POINT_C                                              (0x00000BFC)
#define NV04B_POINT_C_X                                            15:0
#define NV04B_POINT_C_Y                                            31:16
#define NV04B_MONOCHROME_COLOR1_C(a)                               (0x00000C00\
                                                                   +(a)*0x0004)
#define NV04B_CLIP_POINT0_D                                        (0x00000FE8)
#define NV04B_CLIP_POINT0_D_LEFT                                   15:0
#define NV04B_CLIP_POINT0_D_TOP                                    31:16
#define NV04B_CLIP_POINT1_D                                        (0x00000FEC)
#define NV04B_CLIP_POINT1_D_RIGHT                                  15:0
#define NV04B_CLIP_POINT1_D_BOTTOM                                 31:16
#define NV04B_COLOR1_D                                             (0x00000FF0)
#define NV04B_SIZE_IN_D                                            (0x00000FF4)
#define NV04B_SIZE_IN_D_WIDTH                                      15:0
#define NV04B_SIZE_IN_D_HEIGHT                                     31:16
#define NV04B_SIZE_OUT_D                                           (0x00000FF8)
#define NV04B_SIZE_OUT_D_WIDTH                                     15:0
#define NV04B_SIZE_OUT_D_HEIGHT                                    31:16
#define NV04B_POINT_D                                              (0x00000FFC)
#define NV04B_POINT_D_X                                            15:0
#define NV04B_POINT_D_Y                                            31:16
#define NV04B_MONOCHROME_COLOR1_D(a)                               (0x00001000\
                                                                   +(a)*0x0004)
#define NV04B_CLIP_POINT0_E                                        (0x000013E4)
#define NV04B_CLIP_POINT0_E_LEFT                                   15:0
#define NV04B_CLIP_POINT0_E_TOP                                    31:16
#define NV04B_CLIP_POINT1_E                                        (0x000013E8)
#define NV04B_CLIP_POINT1_E_RIGHT                                  15:0
#define NV04B_CLIP_POINT1_E_BOTTOM                                 31:16
#define NV04B_COLOR0_E                                             (0x000013EC)
#define NV04B_COLOR1_E                                             (0x000013F0)
#define NV04B_SIZE_IN_E                                            (0x000013F4)
#define NV04B_SIZE_IN_E_WIDTH                                      15:0
#define NV04B_SIZE_IN_E_HEIGHT                                     31:16
#define NV04B_SIZE_OUT_E                                           (0x000013F8)
#define NV04B_SIZE_OUT_E_WIDTH                                     15:0
#define NV04B_SIZE_OUT_E_HEIGHT                                    31:16
#define NV04B_POINT_E                                              (0x000013FC)
#define NV04B_POINT_E_X                                            15:0
#define NV04B_POINT_E_Y                                            31:16
#define NV04B_MONOCHROME_COLOR01_E(a)                              (0x00001400\
                                                                   +(a)*0x0004)

/* class NV03_CONTEXT_SURFACE_0 */
#define  NV03_CONTEXT_SURFACE_0                          (0x00000058)
#define  NV3_CONTEXT_SURFACE_0                           (0x00000058)
/* NvNotification[] elements */
#define NV058_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NV058_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NV058_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NV058_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NV058_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NV058_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NV058_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV058_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 SetContextDmaImage;      /* NV1_CONTEXT_DMA_IN_MEMORY         0184-0187*/
 V032 Reserved01[0x05e];
 V032 SetColorFormat;          /* NV058_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Reserved02[0x001];
 U032 SetImagePitch;           /* bytes, vertical pixel delta       0308-030b*/
 U032 SetImageOffset;          /* byte offset of top-left pixel     030c-030f*/
 V032 Reserved03[0x73c];
} Nv058Typedef, Nv03ContextSurface0;
#define Nv3ContextSurface0                               Nv03ContextSurface0
#define nv3ContextSurface0                               nv03ContextSurface0
#define NV058_TYPEDEF                                    nv03ContextSurface0
/* dma method offsets, fields, and values */
#define NV058_SET_OBJECT                                           (0x00000000)
#define NV058_NO_OPERATION                                         (0x00000100)
#define NV058_NOTIFY                                               (0x00000104)
#define NV058_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_1                           (0x00000001)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_2                           (0x00000002)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_3                           (0x00000003)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_4                           (0x00000004)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_5                           (0x00000005)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_6                           (0x00000006)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_7                           (0x00000007)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_8                           (0x00000008)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_9                           (0x00000009)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_A                           (0x0000000A)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_B                           (0x0000000B)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_C                           (0x0000000C)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_D                           (0x0000000D)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_E                           (0x0000000E)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN_F                           (0x0000000F)
#define NV058_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV058_SET_CONTEXT_DMA_IMAGE                                (0x00000184)
#define NV058_SET_COLOR_FORMAT                                     (0x00000300)
#define NV058_SET_COLOR_FORMAT_LE_Y8                               (0x01010000)
#define NV058_SET_COLOR_FORMAT_LE_Y16                              (0x01010001)
#define NV058_SET_COLOR_FORMAT_LE_X1R5G5B5_Z1R5G5B5                (0x01000000)
#define NV058_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8                (0x00000001)
#define NV058_SET_IMAGE_PITCH                                      (0x00000308)
#define NV058_SET_IMAGE_OFFSET                                     (0x0000030C)


/* class NV03_CONTEXT_SURFACE_1 */
#define  NV03_CONTEXT_SURFACE_1                          (0x00000059)
#define  NV3_CONTEXT_SURFACE_1                           (0x00000059)
/* NvNotification[] elements */
#define NV059_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NV059_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NV059_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NV059_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NV059_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NV059_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NV059_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV059_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 SetContextDmaImage;      /* NV1_CONTEXT_DMA_IN_MEMORY         0184-0187*/
 V032 Reserved01[0x05e];
 V032 SetColorFormat;          /* NV059_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Reserved02[0x001];
 U032 SetImagePitch;           /* bytes, vertical pixel delta       0308-030b*/
 U032 SetImageOffset;          /* byte offset of top-left pixel     030c-030f*/
 V032 Reserved03[0x73c];
} Nv059Typedef, Nv03ContextSurface1;
#define Nv3ContextSurface1                               Nv03ContextSurface1
#define nv3ContextSurface1                               nv03ContextSurface1
#define NV059_TYPEDEF                                    nv03ContextSurface1
/* dma method offsets, fields, and values */
#define NV059_SET_OBJECT                                           (0x00000000)
#define NV059_NO_OPERATION                                         (0x00000100)
#define NV059_NOTIFY                                               (0x00000104)
#define NV059_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_1                           (0x00000001)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_2                           (0x00000002)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_3                           (0x00000003)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_4                           (0x00000004)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_5                           (0x00000005)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_6                           (0x00000006)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_7                           (0x00000007)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_8                           (0x00000008)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_9                           (0x00000009)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_A                           (0x0000000A)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_B                           (0x0000000B)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_C                           (0x0000000C)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_D                           (0x0000000D)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_E                           (0x0000000E)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN_F                           (0x0000000F)
#define NV059_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV059_SET_CONTEXT_DMA_IMAGE                                (0x00000184)
#define NV059_SET_COLOR_FORMAT                                     (0x00000300)
#define NV059_SET_COLOR_FORMAT_LE_Y8                               (0x01010000)
#define NV059_SET_COLOR_FORMAT_LE_Y16                              (0x01010001)
#define NV059_SET_COLOR_FORMAT_LE_X1R5G5B5_Z1R5G5B5                (0x01000000)
#define NV059_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8                (0x00000001)
#define NV059_SET_IMAGE_PITCH                                      (0x00000308)
#define NV059_SET_IMAGE_OFFSET                                     (0x0000030C)


/* class NV03_CONTEXT_SURFACE_3 */
#define  NV03_CONTEXT_SURFACE_3                          (0x0000005B)
#define  NV3_CONTEXT_SURFACE_3                           (0x0000005B)
/* NvNotification[] elements */
#define NV05B_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NV05B_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NV05B_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NV05B_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NV05B_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NV05B_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NV05B_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV05B_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 SetContextDmaImage;      /* NV1_CONTEXT_DMA_IN_MEMORY         0184-0187*/
 V032 Reserved01[0x05e];
 V032 SetColorFormat;          /* NV05B_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Reserved02[0x001];
 U032 SetImagePitch;           /* bytes, vertical pixel delta       0308-030b*/
 U032 SetImageOffset;          /* byte offset of top-left pixel     030c-030f*/
 V032 Reserved03[0x73c];
} Nv05bTypedef, Nv03ContextSurface3;
#define Nv3ContextSurface3                               Nv03ContextSurface3
#define nv3ContextSurface3                               nv03ContextSurface3
#define NV05B_TYPEDEF                                    nv03ContextSurface3
/* dma method offsets, fields, and values */
#define NV05B_SET_OBJECT                                           (0x00000000)
#define NV05B_NO_OPERATION                                         (0x00000100)
#define NV05B_NOTIFY                                               (0x00000104)
#define NV05B_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_1                           (0x00000001)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_2                           (0x00000002)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_3                           (0x00000003)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_4                           (0x00000004)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_5                           (0x00000005)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_6                           (0x00000006)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_7                           (0x00000007)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_8                           (0x00000008)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_9                           (0x00000009)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_A                           (0x0000000A)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_B                           (0x0000000B)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_C                           (0x0000000C)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_D                           (0x0000000D)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_E                           (0x0000000E)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN_F                           (0x0000000F)
#define NV05B_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV05B_SET_CONTEXT_DMA_IMAGE                                (0x00000184)
#define NV05B_SET_COLOR_FORMAT                                     (0x00000300)
#define NV05B_SET_COLOR_FORMAT_LE_W16_0                            (0x01010000)
#define NV05B_SET_COLOR_FORMAT_LE_W16_1                            (0x01010001)
#define NV05B_SET_COLOR_FORMAT_LE_W16_2                            (0x01000000)
#define NV05B_SET_COLOR_FORMAT_LE_W16_3                            (0x00000001)
#define NV05B_DUMMY                                                (0x00000308)
#define NV05B_SET_IMAGE_OFFSET                                     (0x0000030C)


 /***************************************************************************\
|*                                 Channels                                  *|
 \***************************************************************************/

/* class NV03_CHANNEL_PIO */
#define  NV03_CHANNEL_PIO                                (0x0000006A)
#define  NV3_CHANNEL_PIO                                 (0x0000006A)
/* NvNotification[] fields and values */
#define NV06A_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NV06A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NV06A_NOTIFICATION_STATUS_ERROR_FLOW_CONTROL     (0x0200)
/* pio subchannel method data structure */
typedef volatile struct {
 V032 Reserved00[0x003];
 U016 Free;                    /* free count, read only             0010-0011*/
 U016 Zero[3];                 /* zeroes, read only                 0012-0017*/
 V032 Reserved01[0x03A];
} Nv03ControlPio;
#define Nv3ControlPio                                    Nv03ControlPio
typedef volatile struct {
 V032 SetObject;               /* handle of current object          0000-0003*/
 Nv03ControlPio control;       /* flow control                      0000-00ff*/
 union {                       /* start of class methods            0100-    */
  NvClass                   nvClass;
  NvContextDmaFromMemory    nvContextDmaFromMemory;
  NvContextDmaToMemory      nvContextDmaToMemory;
  NvPatchcordVideo          nvPatchcordVideo;
  NvVideoSink               nvVideoSink;
  NvVideoColorMap           nvVideoColorMap;            // Video Class supported in resman.040
  NvPatchcordImage          nvPatchcordImage;
  NvImageStencil            nvImageStencil;
  NvImageRopAnd             nvImageRopAnd;
  NvImageColorKey           nvImageColorKey;
  NvImageSolid              nvImageSolid;
  NvImagePattern            nvImagePattern;
  NvImageBlackRectangle     nvImageBlackRectangle;
  NvRenderSolidLin          nvRenderSolidLin;
  NvRenderSolidRectangle    nvRenderSolidRectangle;
  NvImageBlit               nvImageBlit;
  NvImageFromCpu            nvImageFromCpu;
  NvPatchcordRop            nvPatchcordRop;
  NvMemoryToMemoryFormat    nvMemoryToMemoryFormat;
  NvScaledImageFromMemory   nvScaledImageFromMemory;
  NvScaledYuv420FromMemory  nvScaledYuv420FromMemory;
  NvStretchedImageFromCpu   nvStretchedImageFromCpu;
  NvContextDmaInMemory      nvContextDmaInMemory;
  NvImageInMemory           nvImageInMemory;
  NvVideoFromMemory         nvVideoFromMemory;
  NvVideoScaler             nvVideoScaler;              // Video Class supported in resman.040
  NvVideoColorKey           nvVideoColorKey;            // Video Class supported in resman.040
  NvRop5Solid               nvRop5Solid;
  NvZetaBufferInMemory      nvZetaBufferInMemory;
  NvPatchcordZeta           nvPatchcordZeta;
  NvRenderD3D0TriangleZeta  nvRenderD3D0TriangleZeta;
  NvRenderGdi0RectangleAndText nvRenderGdi0RectangleAndText;

  Nv018Typedef NV018_TYPEDEF;
  Nv019Typedef NV019_TYPEDEF;
  Nv01eTypedef NV01E_TYPEDEF;
  Nv01fTypedef NV01F_TYPEDEF;
  Nv021Typedef NV021_TYPEDEF;
  Nv043Typedef NV043_TYPEDEF;
  Nv048Typedef NV048_TYPEDEF;
  Nv04bTypedef NV04B_TYPEDEF;
  Nv058Typedef NV058_TYPEDEF;
  Nv059Typedef NV059_TYPEDEF;
  Nv05bTypedef NV05B_TYPEDEF;


 } /* cls */;                  /* end of class methods                  -1fff*/
} Nv03SubchannelPio;
#define Nv3SubchannelPio                                 Nv03SubchannelPio
/* pio channel */
typedef volatile struct {      /* start of array of subchannels     0000-    */
 Nv03SubchannelPio subchannel[8];/*subchannel                       0000-1fff*/
} Nv03ChannelPio;              /* end of array of subchannels           -ffff*/
#define Nv3ChannelPio                                    Nv03ChannelPio
/* fields and values */
#define NV06A_FIFO_GUARANTEED_SIZE                       (0x007C)
#define NV06A_FIFO_EMPTY                                 (0x007C)


/* class NV03_CHANNEL_DMA */
#define  NV03_CHANNEL_DMA                                (0x0000006B)
/* NvNotification[] fields and values */
#define NV06B_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv06bTypedef, Nv03ChannelDma;
#define  NV06B_TYPEDEF                                   nv03ChannelDma



 /***************************************************************************\
|*                            Well Known Objects                             *|
 \***************************************************************************/


/* object NV01_NULL_OBJECT */
#define   NV01_NULL_OBJECT                               (0x00000000)
#define   NV1_NULL_OBJECT                                (0x00000000)



 /***************************************************************************\
|*                               Notification                                *|
 \***************************************************************************/


/***** NvNotification Structure *****/
/*
 * NV objects return information about method completion to clients via an
 * array of notification structures in main memory.
 *
 * The client sets the status field to NV???_NOTIFICATION_STATUS_IN_PROGRESS.
 * NV fills in the NvNotification[] data structure in the following order:
 * timeStamp, otherInfo32, otherInfo16, and then status.
 */


/* memory data structures */
typedef volatile struct {
 struct {                      /*                                   0000-    */
  U032 nanoseconds[2];         /* nanoseconds since Jan. 1, 1970       0-   7*/
 } timeStamp;                  /*                                       -0007*/
 V032 info32;                  /* info returned depends on method   0008-000b*/
 V016 info16;                  /* info returned depends on method   000c-000d*/
 V016 status;                  /* user sets bit 15, NV sets status  000e-000f*/
} NvNotification;
/* status values */

/************** WARNING Duplicated from nv32.h ******************************/

/* pio flow control data structure */
typedef volatile struct {
 NvV32 Ignored00[0x010];
 NvU32 Put;                     /* put offset, write only           0040-0043*/
 NvU32 Get;                     /* get offset, read only            0044-0047*/
 NvU32 Reference;               /* reference value, read only       0048-004b*/
 NvV32 Ignored01[0x7ed];
} Nv06eControl, Nv10ControlDma;




//****************************************************************************
// Macros relocated from DRIVER.H. They were moved here to avoid conflicts
// with D3d source files which had defined the same macros.
//****************************************************************************

#define NvGetFreeCount(NN, CH)\
                 (NN)->subchannel[CH].control.Free

