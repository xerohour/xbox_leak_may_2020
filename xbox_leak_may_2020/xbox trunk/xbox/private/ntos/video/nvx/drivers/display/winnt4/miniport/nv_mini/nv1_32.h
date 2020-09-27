#ifndef NV32_INCLUDED
#define NV32_INCLUDED
/***************************************************************************

  Copyright (C) 1995 NVidia Corporation.

  <nv32.h> defines a 32-bit wide naming convention for the functionality
  of NVidia's Unified Media Architecture (TM).

  Other header files:
     <nvwin32.h>  Contains the function prototypes for the few client
                  functions implemented by the Windows Resource Manager.
     <nvmacros.h> Defines macros that assemble 32-bit quantities for
                  assigning to the hardware from 16-bit coordinates,
                  24-bit RGB triples, etc.  Use of this is optional.
     <nvutypes.h> Declares structures that expose elements smaller than
                  32 bits wide ("NVidia unpacked types").  Use of this
                  is optional.
    Copyright 1995,1996 NVidia Corporation. All Rights Reserved.

 ***************************************************************************/

/***************************************************************************
 *                          Constants                                      *
 ***************************************************************************/

/***** Miscellaneous Sizes *****/
#define NV_GUARANTEED_FIFO_SIZE    0x007C
#define NV_NVIDIA_NAME_LIMIT       0x00000FFF

/***** Notification Styles *****/
/* This simply fills the NvNotification structure */
#define NV_NOTIFY_WONT_SLEEP       0x00000000
/* Other notification styles are operating-system dependent */

/***** Notification Reports *****/
#define NV_BUFFER_GAP              0x00000001

/***** Well-known Objects (besides classes) *****/
#define NV_NULL_OBJECT             0
#define NV_THE_CHIP_ID             0x0FFE
#define NV_ERROR_NOTIFIER          0x0FFD

/***** Errors *****/
#define NV_ERROR_TYPE_CONFLICT           1
#define NV_ERROR_OUT_OF_RANGE            2
#define NV_ERROR_NO_CONNECTION           3
#define NV_ERROR_NO_SUCH_OBJECT          4
#define NV_ERROR_NAME_IN_USE             5
#define NV_ERROR_OUT_OF_RESOURCES        6
#define NV_ERROR_TRANSLATION_VIOLATION   7
#define NV_ERROR_PROTECTION_VIOLATION    8
#define NV_ERROR_BUFFER_BUSY             9
#define NV_ERROR_ILLEGAL_ACCESS          10
#define NV_ERROR_BAD_COLORMAP_FORMAT     11
#define NV_ERROR_BAD_COLOR_FORMAT        12
#define NV_ERROR_BAD_MONOCHROME_FORMAT   13
#define NV_ERROR_BAD_PATTERN_SHAPE       14
#define NV_ERROR_BAD_SUBDIVIDE           15
#define NV_ERROR_NO_CURRENT_POINT        16
#define NV_ERROR_BAD_AUDIO_FORMAT        17
#define NV_ERROR_NO_DMA_TRANSLATION      18
#define NV_ERROR_INCOMPLETE_METHOD       19
#define NV_ERROR_RESERVED_ADDRESS        20
#define NV_ERROR_UNIMPLEMENTED_PATCH     21
#define NV_ERROR_OS_NAME_ERROR           22
#define NV_ERROR_INCOMPLETE_PATCH        23
#define NV_ERROR_BAD_GAME_PORT_FORMAT    24

/***** SetMonochromeFormat values *****/
#define NV_MONO_LE_M1_P32          0x00000000
#define NV_MONO_CGA6_M1_P32        0x00000001

/***** SetColorFormat values ******/
/*
 The format names precisely specify how the bits are arranged
 in a 32-bit write of a color or colors in that format in a
 little-endian system.  For example NV_COLOR_FORMAT_LE_X17R5G5B5
 means the upper 17 bits are ignored, then 5 bits each of
 Red, Green, and Blue intensity:

   3                             1 1
   1                             6 5                             0
  |X X X X X X X X X X X X X X X X X R R R R R G G G G G B B B B B|

 */
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


/***************************************************************************
 *                          Typedefs                                       *
 ***************************************************************************/

/* The initial typedefs declare the C type information for
 * the NV structures later in the file.
 */

/***** The fundamental types *****/
#undef U008
#undef V008
#undef U016
#undef V016
#undef S032
#undef U032
#undef V032

typedef unsigned char    U008;
typedef unsigned char    V008;
typedef unsigned short   U016;
typedef unsigned short   V016; /* "void": more than one value or sign N/A */
typedef long             S032;
typedef unsigned long    U032;
typedef unsigned long    V032; /* "void": more than one value or sign N/A */

/*
 * NV objects return information to applications via notification
 * structures in main memory.
 */
typedef volatile struct {
 struct
  tagNvTimeStamp {
  U032 nanoseconds[2];           /* Nanoseconds since Jan. 1, 1970 */
 } timeStamp;                  /*                                 0000-0007*/
 V032 otherInfo;               /* info reported depends on method 0008-000b*/
 V016 errorCode;               /* Zero means no error             000c-000d*/
 V008 reserved01[0x001];
 V008 inProgress;              /* Application sets, NV zeroes     000f-000f*/
} NvNotification;

/*
 *  The first 256 bytes of each subchannel.
 */
typedef volatile struct {
 V032 object;                  /* current object register         0000-0003*/
 V032 reserved01[0x003];
 U016 free;                    /* free count, only readable reg.  0010-0011*/
 U016 reserved02[0x001];
 U032 reserved03[0x003];
 struct
  tagNvPassword {
  V032 token[4];               /* Any value written here must "unlock" the */
 } password;                   /* chip, matching its ChipToken.   0020-002f*/
 V032 push;                    /* Any write saves channel state   0030-0033*/
 V032 pop;                     /* Any write restores channel state0034-0037*/
 V032 reserved04[0x032];
} NvControl;

/***************************************************************************
 *                           Classes                                       *
 ***************************************************************************/

/*
 * These structures indicate the precise offsets of hardware
 * registers corresponding to the methods of each class.  Since the
 * first 256 bytes of each subchannel is the NvControl registers,
 * the hexadecimal offsets in comments start at 0x100.
 */

/* General and Utility Classes */

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
typedef volatile struct
 tagNvContextDmaFromMemory {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 struct
  tagNvSetDmaFromSpecifier {
  V032 address[2];               /* On X86, address[1] is the selector */
  U032 limit;                    /* size of data region in bytes - 1 */
 } SetDmaSpecifier;            /* memory region NV will read      0300-030b*/
 V032 reserved02[0x73d];
} NvContextDmaFromMemory;

/* class NV_CONTEXT_DMA_TO_MEMORY */
#define NV_CONTEXT_DMA_TO_MEMORY  3
typedef volatile struct
 tagNvContextDmaToMemory {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 struct
  tagNvSetDmaToSpecifier {
  V032 address[2];               /* On X86, address[1] is the selector */
  U032 limit;                    /* size of data region in bytes - 1   */
 } SetDmaSpecifier;            /* memory region NV will write     0300-030b*/
 V032 reserved02[0x73d];
} NvContextDmaToMemory;

/* class NV_TIMER */
#define NV_TIMER  4
typedef volatile struct
 tagNvTimer {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 struct
  tagNvSetAlarmTime {
  U032 nanoseconds[2];
 } SetAlarmTime;               /* nanoseconds since Jan. 1, 1970  0300-0307*/
 V032 SetAlarmNotifyCtxDma;    /* NV_CONTEXT_DMA_TO_MEMORY        0308-030b*/
 V032 SetAlarmNotify;          /* notification style, varies w/OS 030c-030f*/
 V032 reserved02[0x73c];
} NvTimer;

/* class NV_CHIP_ID */
#define NV_CHIP_ID  5
typedef volatile struct
 tagNvChipId {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 V032 SetChipTokenCtxDma;      /* DMA to memory for 64-bit token  0300-0303*/
 V032 reserved02[0x001];
 V032 GetChipToken;            /* any write triggers transfer     0308-030b*/
 V032 reserved03[0x73d];
} NvChipId;

/* class NV_CONTEXT_ORDINAL */
#define NV_CONTEXT_ORDINAL  6
typedef volatile struct
 tagNvContextOrdinal {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 U032 SetOrdinal;              /* zero or positive number         0300-0303*/
 V032 reserved02[0x73f];
} NvContextOrdinal;

/* class NV_NULL_CLASS */
#define NV_NULL_CLASS  48
typedef volatile struct
 tagNvNullClass {
 V032 reserved01[0x7c0];       /* has no methods */
} NvNullClass;

/***** Game Port Classes *****/

/* class NV_PATCHCORD_GAME_PORT */
#define NV_PATCHCORD_GAME_PORT  7
typedef volatile struct
 tagNvPatchcordGamePort {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 V032 Validate;                /* writing any value validates     0300-0303*/
 V032 reserved02[0x73f];
} NvPatchcordGamePort;

/* class NV_GAME_PORT_SOURCE */
#define NV_GAME_PORT_SOURCE  8
typedef volatile struct
 tagNvGamePortSource {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetGamePortOutput;       /* NV_PATCHCORD_GAME_PORT          0200-0203*/
 V032 reserved02[0x77f];
} NvGamePortSource;

/* class NV_GAME_PORT_TO_MEMORY */
#define NV_GAME_PORT_TO_MEMORY  9
typedef volatile struct
 tagNvGamePortToMemory {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetGamePortInput[56];    /* NV_GAME_PORT_PATCHCORD per chan 0200-02df*/
 V032 reserved02[0x008];
 struct
  tagNvSetStartTime {
  U032 nanoseconds[2];           /* nanoseconds since Jan. 1, 1970 */
 } SetStartTime;               /* 0 means immediate transfer      0300-0307*/
 V032 SetSampleRate;           /* U16.16 in Hertz (cycles/sec.)   0308-030b*/
 V032 SetGamePortFormat;       /* only valid is _LE_U016_LINEAR   030c-030f*/
 U032 SetNumberChannels;       /* 1 to 8 for NV game port on PC   0310-0313*/
 V032 reserved03[0x03b];
 V032 SetBufferCtxDma[2];      /* DMA to memory for the buffer(s) 0400-0407*/
 V032 reserved04[0x006];
 V032 SetBufferNotifyCtxDma[2];/* DMA to mem. for NvNotification  0420-0427*/
 V032 reserved05[0x006];
 U032 SetBufferStart[2];       /* offset within ctxdma in bytes   0440-0447*/
 V032 reserved06[0x006];
 U032 SetBufferLength[2];      /* in bytes                        0460-0467*/
 V032 reserved07[0x006];
 V032 SetBufferNotify[2];      /* indicates buff is ready to fill 0480-0487*/
 V032 reserved08[0x6de];
} NvGamePortToMemory;
/* values for SetGamePortFormat() */
#define NV_GAME_PORT_FORMAT_LE_U016_LINEAR 0x00000004

/***** Video Classes *****/

/* class NV_PATCHCORD_VIDEO */
#define NV_PATCHCORD_VIDEO  10
typedef volatile struct
 tagNvPatchcordVideo {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 V032 Validate;                /* writing any value validates     0300-0303*/
 V032 reserved02[0x73f];
} NvPatchcordVideo;

/* class NV_VIDEO_SINK */
#define NV_VIDEO_SINK  11
typedef volatile struct
 tagNvVideoSink {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetVideoInput[56];       /* NV_PATCHCORD_VIDEO              0200-02df*/
// V032 SetVideoInputNotifyCtxDma;/*
 V032 reserved02[0x748];
} NvVideoSink;

/* class NV_VIDEO_SWITCH */
#define NV_VIDEO_SWITCH  12
typedef volatile struct
 tagNvVideoSwitch {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetVideoOutput;          /* NV_PATCHCORD_VIDEO              0200-0203*/
 V032 SetVideoInput[56];       /* P_V's from I2V's or COLORMAPs   0204-02e3*/
 V032 reserved02[0x007];
 V032 SetCtxOrdinal;           /* NV_CONTEXT_ORDINAL              0300-0303*/
 V032 SetSwitchNotifyCtxDma[56];/* NvNotification for each switch 0304-03e3*/
 V032 reserved03[0x008];
 V032 SetSwitchNotify[56];      /* notifies when it switches away 0404-04e3*/
 V032 reserved04[0x6c7];
} NvVideoSwitch;

/* class NV_VIDEO_COLORMAP */
#define NV_VIDEO_COLORMAP  13
typedef volatile struct
 tagNvVideoColormap {
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
} NvVideoColormap;
/* values for NV_VIDEO_COLORMAP SetColormapFormat() */
#define NV_COLORMAP_X8R8G8B8       0x00000000
#define NV_COLORMAP_X2R10G10B10    0x00000001


/***** Image Classes *****/

/* class NV_PATCHCORD_IMAGE */
#define NV_PATCHCORD_IMAGE  14
typedef volatile struct
 tagNvPatchcordImage {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 V032 Validate;                /* validate connections downstream 0300-0303*/
 V032 reserved02[0x73f];
} NvPatchcordImage;

/* class NV_IMAGE_TO_VIDEO */
#define NV_IMAGE_TO_VIDEO  15
typedef volatile struct
 tagNvImageToVideo {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE to ROP/blend 0200-0203*/
 V032 SetVideoOutput;          /* NV_PATCHCORD_VIDEO              0204-0207*/
 V032 SetImageInput[56];       /* NV_PATCHCORD_IMAGEs             0208-02e7*/
 V032 reserved02[0x746];
} NvImageToVideo;

/* class NV_IMAGE_STENCIL */
#define NV_IMAGE_STENCIL  16
typedef volatile struct
 tagNvImageStencil {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 SetImageInput[2];        /* P_I, 1 is ImageBlackRectangle   0204-020b*/
 V032 reserved02[0x77d];
} NvImageStencil;

/* class NV_PATCHCORD_BETA */
#define NV_PATCHCORD_BETA  49
typedef volatile struct
 tagNvPatchcordBeta {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 V032 Validate;                /* writing any value validates     0300-0303*/
 V032 reserved02[0x73f];
} NvPatchcordBeta;

/* class NV_IMAGE_BLEND_AND */
#define NV_IMAGE_BLEND_AND  17
typedef volatile struct
 tagNvImageBlendAnd {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 SetBetaInput;            /* NV_PATCHCORD_BETA of Solid/QTMB 0204-0207*/
 V032 SetImageInput[2];        /* P_I, 1 is pattern or I2V        0208-020f*/
 V032 reserved02[0x77c];
} NvImageBlendAnd;

/* class NV_BETA_SOLID */
#define NV_BETA_SOLID  18
typedef volatile struct
 tagNvBetaSolid {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetBetaOutput;           /* NV_PATCHCORD_BETA               0200-0203*/
 V032 reserved02[0x03f];
 V032 SetBeta1d31;             /* S1.31 beta value                0300-0303*/
 V032 reserved03[0x73f];
} NvBetaSolid;

/* class NV_BETA_MAXIMUM */
#define NV_BETA_MAXIMUM  50
typedef volatile struct
 tagNvBetaMaximum {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetBetaOutput;           /* NV_PATCHCORD_BETA to blender    0200-0203*/
 V032 SetBetaInput[2];         /* NV_PATCHCORD_BETA, 1 from solid 0204-020b*/
 V032 reserved02[0x77d];
} NvBetaMaximum;

/* class NV_PATCHCORD_ROP */
#define NV_PATCHCORD_ROP  51
typedef volatile struct
 tagNvPatchcordRop {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 V032 Validate;                /* writing any value validates     0300-0303*/
 V032 reserved02[0x73f];
} NvPatchcordRop;

/* class NV_IMAGE_ROP_AND */
#define NV_IMAGE_ROP_AND  19
typedef volatile struct
 tagNvImageRopAnd {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 SetRopInput;             /* NV_PATCHCORD_ROP                0204-0207*/
 V032 SetImageInput[3];        /* 0 is I2V ImageOut, 2 is pattern 0208-0213*/
 V032 reserved02[0x77b];
} NvImageRopAnd;

/* class NV_ROP_SOLID */
#define NV_ROP_SOLID  20
typedef volatile struct
 tagRopSolid {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetRopOutput;            /* NV_PATCHCORD_ROP                0200-0203*/
 V032 reserved02[0x03f];
 V032 SetRop;                  /* 8-bit index to std. MS Win ROPs 0300-0303*/
 V032 reserved03[0x73f];
} NvRopSolid;

/* class NV_IMAGE_COLOR_KEY */
#define NV_IMAGE_COLOR_KEY  21
typedef volatile struct
 tagNvImageColorKey {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 SetImageInput[2];        /* P_I, 1 from ImageSolidColor     0204-020b*/
 V032 reserved02[0x77d];
} NvImageColorKey;

/* class NV_IMAGE_PLANE_SWITCH */
#define NV_IMAGE_PLANE_SWITCH  22
typedef volatile struct
 tagNvImagePlaneSwitch {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 SetImageInput[3];        /* PI, 1 from NV_IMAGE_SOLID ??    0204-020f*/
 V032 reserved02[0x77c];
} NvImagePlaneSwitch;

/***** Static Image Classes *****/

/* class NV_IMAGE_SOLID */
#define NV_IMAGE_SOLID  23
typedef volatile struct
 tagNvImageSolid {
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
typedef volatile struct
 tagNvImagePattern {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* must be an unpacked format      0300-0303*/
 V032 SetMonochromeFormat;     /* NV_MONO_{LE_M1_P32, CGA6_M1_P32}0304-0307*/
 V032 SetPatternShape;         /* NV_PATTERN_SHAPE_{8X8,64X1,1X64}0308-030b*/
 V032 reserved03[0x001];
 V032 SetColor0;               /* "background" color where pat=0  0310-0313*/
 V032 SetColor1;               /* "foreground" color where pat=1  0314-0317*/
 struct
  tagNvSetPattern {
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
typedef volatile struct
 tagNvImageBlackRectangle {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* P_I usually to NV_IMAGE_STENCIL 0200-0203*/
 V032 reserved02[0x03f];
 struct
  tagNvSetRectangle {
  V032 y_x;                      /* S16_S16 in pixels, 0 at top left 00-04*/
  V032 height_width;             /* U16_U16 in pixels                05-07*/
 } SetRectangle;               /* region in image where alpha=1   0300-0307*/
 V032 reserved03[0x73e];
} NvImageBlackRectangle;

/***** Solid Rendering Classes *****/

/* class NV_RENDER_SOLID_POINT */
#define NV_RENDER_SOLID_POINT  26
typedef volatile struct
 tagNvRenderSolidPoint {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* must be an unpacked format      0300-0303*/
 V032 Color;                   /*                                 0304-0307*/
 V032 reserved03[0x03e];
 V032 Point[32];               /* y_x S16_S16 in pixels           0400-047f*/
 struct
  tagNvPoint32 {
  S032 x;                        /* in pixels, 0 at left             00-03*/
  S032 y;                        /* in pixels, 0 at top              04-07*/
 } Point32[16];                /*                                 0480-04ff*/
 struct
  tagNvColorPoint {
  V032 color;                    /*                                  00-03*/
  V032 y_x;                      /* S16_S16 in pixels, 0 at top left 04-07*/
 } ColorPoint[16];             /*                                 0500-057f*/
 V032 reserved04[0x6a0];
} NvRenderSolidPoint;

/* class NV_RENDER_SOLID_LINE */
#define NV_RENDER_SOLID_LINE  27
typedef volatile struct
 tagNvRenderSolidLine {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* must be an unpacked format      0300-0303*/
 V032 Color;                   /*                                 0304-0307*/
 V032 reserved03[0x03e];
 struct
  tagNvLine {
  V032 y0_x0;                    /* S16_S16 in pixels, 0 at top left 00-03*/
  V032 y1_x1;                    /* S16_S16 in pixels, 0 at top left 04-07*/
 } Line[16];                   /*                                 0400-047f*/
 struct
  tagNvLine32 {
  S032 x0;                         /*                                 00-03*/
  S032 y0;                         /*                                 04-07*/
  S032 x1;                         /*                                 08-0b*/
  S032 y1;                         /*                                 0c-0f*/
 } Line32[8];                  /*                                 0480-04ff*/
 V032 PolyLine[32];            /*                                 0500-057f*/
 struct
  tagNvPolyLine32 {
  S032 x;                          /*                                 00-03*/
  S032 y;                          /*                                 04-07*/
 } PolyLine32[16];             /*                                 0580-05ff*/
 struct
  tagNvColorPolyLine {
  V032 color;                      /*                                 00-03*/
  V032 y_x;                      /* S16_S16 in pixels, 0 at top left 04-07*/
 } ColorPolyLine[16];          /*                                 0600-067f*/
 V032 reserved04[0x660];
} NvRenderSolidLine;

/* class NV_RENDER_SOLID_LIN */
#define NV_RENDER_SOLID_LIN  28
typedef volatile struct
 tagNvRenderSolidLin {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* must be an unpacked format      0300-0303*/
 V032 Color;                   /*                                 0304-0307*/
 V032 reserved03[0x03e];
 struct
  tagNvLin {
  V032 y0_x0;                    /* S16_S16 in pixels, 0 at top left 00-03*/
  V032 y1_x1;                    /* S16_S16 in pixels, 0 at top left 04-07*/
 } Lin[16];                    /*                                 0400-047f*/
 struct
  tagNvLin32 {
  S032 x0;                         /*                                 00-03*/
  S032 y0;                         /*                                 04-07*/
  S032 x1;                         /*                                 08-0b*/
  S032 y1;                         /*                                 0c-0f*/
 } Lin32[8];                   /*                                 0480-04ff*/
 V032 PolyLin[32];             /*                                 0500-057f*/
 struct
  tagNvPolyLin32 {
  S032 x;                          /*                                 00-03*/
  S032 y;                          /*                                 04-07*/
 } PolyLin32[16];              /*                                 0580-05ff*/
 struct
  tagNvColorPolyLin {
  V032 color;                      /*                                 00-03*/
  V032 y_x;                      /* S16_S16 in pixels, 0 at top left 04-07*/
 } ColorPolyLin[16];           /*                                 0600-067f*/
 V032 reserved04[0x660];
} NvRenderSolidLin;

/* class NV_RENDER_SOLID_TRIANGLE */
#define NV_RENDER_SOLID_TRIANGLE  29
typedef volatile struct
 tagNvRenderSolidTriangle {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /*                                 0300-0303*/
 V032 Color;                   /*                                 0304-0307*/
 V032 reserved03[0x002];
 struct
  tagNvTriangle {
  V032 y0_x0;                  /* S16_S16 in pixels, 0 = top left 0310-0313*/
  V032 y1_x1;                  /* S16_S16 in pixels, 0 = top left 0314-0317*/
  V032 y2_x2;                  /* S16_S16 in pixels, 0 = top left 0318-031b*/
 } Triangle;
 V032 reserved04[0x001];
 struct
  tagNvTriangle32 {
  S032 x0;                     /*                                 0320-0323*/
  S032 y0;                     /*                                 0324-0327*/
  S032 x1;                     /*                                 0328-032b*/
  S032 y1;                     /*                                 032c-032f*/
  S032 x2;                     /*                                 0330-0333*/
  S032 y2;                     /*                                 0334-0337*/
 } Triangle32;
 V032 reserved05[0x032];
 V032 Trimesh[32];             /*                                 0400-047f*/
 struct
  tagNvTrimesh32 {
  S032 x;                          /*                                 00-03*/
  S032 y;                          /*                                 04-07*/
 } Trimesh32[16];              /*                                 0480-04ff*/
 struct
  tagNvColorTriangle {
  V032 color;                    /*                                  00-03*/
  V032 y0_x0;                    /* S16_S16 in pixels, 0 at top left 04-07*/
  V032 y1_x1;                    /* S16_S16 in pixels, 0 at top left 08-0b*/
  V032 y2_x2;                    /* S16_S16 in pixels, 0 at top left 0c-0f*/
 } ColorTriangle[8];           /* Specify color and coordinates  0500-057f*/
 struct
  tagNvColorTrimesh {
  V032 color;                    /*                                 00-03*/
  V032 y_x;                      /* S16_S16 in pixels, 0 at top left 04-07*/
 } ColorTrimesh[16];           /* Specify color and next coordinate 0580-05ff*/
 V032 reserved06[0x680];
} NvRenderSolidTriangle;

/* class NV_RENDER_SOLID_RECTANGLE */
#define NV_RENDER_SOLID_RECTANGLE  30
typedef volatile struct
 tagNvRenderSolidRectangle {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /*                                 0300-0303*/
 V032 Color;                   /*                                 0304-0307*/
 V032 reserved03[0x03e];
 struct
  tagNvRectangle {
  V032 y_x;                      /* S16_S16 in pixels, 0 at top left 00-03*/
  V032 height_width;             /* U16_U16 in pixels                04-07*/
 } Rectangle[16];              /*                                 0400-047f*/
 V032 reserved04[0x6e0];
} NvRenderSolidRectangle;

/***** Image Rendering Classes *****/

/* class NV_IMAGE_BLIT */
#define NV_IMAGE_BLIT  31
typedef volatile struct
 tagNvImageBlit {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 SetImageInput;           /* NV_PATCHCORD_IMAGE              0204-0207*/
 V032 reserved02[0x03e];
 V032 ControlPointIn;          /* S16_S16 in pixels, u.r. of src  300-0303*/
 V032 ControlPointOut;         /* S16_16 in pixels, u.r. of dest  0304-0307*/
 V032 Size;                    /* U16_U16 in pixels               0308-030b*/
 V032 reserved03[0x73d];
} NvImageBlit;

/* class NV_IMAGE_FROM_MEMORY */
#define NV_IMAGE_FROM_MEMORY  32
typedef volatile struct
 tagNvImageFromMemory {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* cannot be an unpacked format    0300-0303*/
 V032 SetImageCtxDma;          /* NV_CONTEXT_DMA_FROM_MEMORY      0304-0307*/
 V032 Point;                   /* y_x S16_S16 in pixels           0308-030b*/
 V032 Size;                    /* height_width U16_U16 in pixels  030c-030f*/
 S032 ImagePitch;              /* gap in bytes between scan lines 0310-0313*/
 U032 ImageStart;              /* byte offset of top-left pixel   0314-0317*/
 V032 reserved03[0x73a];
} NvImageFromMemory;

/* class NV_IMAGE_FROM_CPU */
#define NV_IMAGE_FROM_CPU  33
typedef volatile struct
 tagNvImageFromCpu {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /*                                 0300-0303*/
 V032 Point;                   /* y_x S16_S16 in pixels           0304-0307*/
 V032 Size;                    /* height_width U16_U16 in pixels  0308-030b*/
 V032 SizeIn;                  /* height_width U16_U16 in pixels  030c-030f*/
 V032 reserved03[0x03c];
 V032 Color[32];               /*                                 0400-047f*/
 V032 reserved04[0x6e0];
} NvImageFromCpu;

/* class NV_IMAGE_MONOCHROME_FROM_CPU */
#define NV_IMAGE_MONOCHROME_FROM_CPU  34
typedef volatile struct
 tagNvImageMonochromeFromCpu {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /*                                 0300-0303*/
 V032 SetMonochromeFormat;     /*                                 0304-0307*/
 V032 Color0;                  /*                                 0308-030b*/
 V032 Color1;                  /*                                 030c-030f*/
 V032 Point;                   /*                                 0310-0313*/
 V032 Size;                    /*                                 0314-0317*/
 V032 SizeIn;                  /*                                 0318-031b*/
 V032 reserved03[0x039];
 V032 Monochrome[32];          /*                                 0400-047f*/
 V032 reserved04[0x6e0];
} NvImageMonochromeFromCpu;

/***** Texture Mapping Classes *****/

/* class NV_RENDER_TEXTURE_BILINEAR */
#define NV_RENDER_TEXTURE_BILINEAR  35
typedef volatile struct
 tagNvRenderTextureBilinear {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /*                                 0300-0303*/
 V032 SubdivideIn;             /*                                 0304-0307*/
 V032 reserved03[0x002];
 struct
  tagNvBiControlPointOut {
  V032 y0_x0;                  /* S16_S16 in pixels, 0 = top left 0310-0313*/
  V032 y1_x1;                  /* S16_S16 in pixels, 0 = top left 0314-0317*/
  V032 y2_x2;                  /* S16_S16 in pixels, 0 = top left 0318-031b*/
  V032 y3_x3;                  /* S16_S16 in pixels, 0 = top left 031c-031f*/
 } ControlPointOut;
 V032 reserved04[0x00c];
 struct
  tagNvBiControlPointOut12d4 {
  V032 y0_x0;                  /* S12.4_S1.d4 in pixels           0350-0353*/
  V032 y1_x1;                  /* S12.4_S12.4 in pixels           0354-0357*/
  V032 y2_x2;                  /* S12.4_S12.4 in pixels           0358-035b*/
  V032 y3_x3;                  /* S12.4_S12.4 in pixels           035c-035f*/
 } ControlPointOut12d4;
 V032 reserved05[0x028];
 V032 Color[32];               /*                                 0400-047f*/
 V032 reserved06[0x6e0];
} NvRenderTextureBilinear;
/*
 * Bit values for NV_RENDER_TEXTURE_BILINEAR and
 * NV_RENDER_TEXTURE_BILINEAR_BETA SubdivideIn()
 */
#define NV_BTM_SUBDIVIDE_HEIGHT_01_23_4    0x00000002
#define NV_BTM_SUBDIVIDE_HEIGHT_01_23_8    0x00000003
#define NV_BTM_SUBDIVIDE_HEIGHT_01_23_16   0x00000004
#define NV_BTM_SUBDIVIDE_HEIGHT_01_23_32   0x00000005
#define NV_BTM_SUBDIVIDE_HEIGHT_01_23_64   0x00000006
#define NV_BTM_SUBDIVIDE_HEIGHT_01_23_128  0x00000007
#define NV_BTM_SUBDIVIDE_HEIGHT_01_23_256  0x00000008
#define NV_BTM_SUBDIVIDE_WIDTH_02_13_4     0x00000020
#define NV_BTM_SUBDIVIDE_WIDTH_02_13_8     0x00000030
#define NV_BTM_SUBDIVIDE_WIDTH_02_13_16    0x00000040
#define NV_BTM_SUBDIVIDE_WIDTH_02_13_32    0x00000050
#define NV_BTM_SUBDIVIDE_WIDTH_02_13_64    0x00000060
#define NV_BTM_SUBDIVIDE_WIDTH_02_13_128   0x00000070
#define NV_BTM_SUBDIVIDE_WIDTH_02_13_256   0x00000080
#define NV_BTM_SUBDIVIDE_EDGE_01_1         0x00000000
#define NV_BTM_SUBDIVIDE_EDGE_01_2         0x00010000
#define NV_BTM_SUBDIVIDE_EDGE_01_4         0x00020000
#define NV_BTM_SUBDIVIDE_EDGE_01_8         0x00030000
#define NV_BTM_SUBDIVIDE_EDGE_01_16        0x00040000
#define NV_BTM_SUBDIVIDE_EDGE_01_32        0x00050000
#define NV_BTM_SUBDIVIDE_EDGE_01_64        0x00060000
#define NV_BTM_SUBDIVIDE_EDGE_01_128       0x00070000
#define NV_BTM_SUBDIVIDE_EDGE_01_256       0x00080000
#define NV_BTM_SUBDIVIDE_EDGE_23_1         0x00000000
#define NV_BTM_SUBDIVIDE_EDGE_23_2         0x00100000
#define NV_BTM_SUBDIVIDE_EDGE_23_4         0x00200000
#define NV_BTM_SUBDIVIDE_EDGE_23_8         0x00300000
#define NV_BTM_SUBDIVIDE_EDGE_23_16        0x00400000
#define NV_BTM_SUBDIVIDE_EDGE_23_32        0x00500000
#define NV_BTM_SUBDIVIDE_EDGE_23_64        0x00600000
#define NV_BTM_SUBDIVIDE_EDGE_23_128       0x00700000
#define NV_BTM_SUBDIVIDE_EDGE_23_256       0x00800000
#define NV_BTM_SUBDIVIDE_EDGE_02_1         0x00000000
#define NV_BTM_SUBDIVIDE_EDGE_02_2         0x01000000
#define NV_BTM_SUBDIVIDE_EDGE_02_4         0x02000000
#define NV_BTM_SUBDIVIDE_EDGE_02_8         0x03000000
#define NV_BTM_SUBDIVIDE_EDGE_02_16        0x04000000
#define NV_BTM_SUBDIVIDE_EDGE_02_32        0x05000000
#define NV_BTM_SUBDIVIDE_EDGE_02_64        0x06000000
#define NV_BTM_SUBDIVIDE_EDGE_02_128       0x07000000
#define NV_BTM_SUBDIVIDE_EDGE_02_256       0x08000000
#define NV_BTM_SUBDIVIDE_EDGE_13_1         0x00000000
#define NV_BTM_SUBDIVIDE_EDGE_13_2         0x10000000
#define NV_BTM_SUBDIVIDE_EDGE_13_4         0x20000000
#define NV_BTM_SUBDIVIDE_EDGE_13_8         0x30000000
#define NV_BTM_SUBDIVIDE_EDGE_13_16        0x40000000
#define NV_BTM_SUBDIVIDE_EDGE_13_32        0x50000000
#define NV_BTM_SUBDIVIDE_EDGE_13_64        0x60000000
#define NV_BTM_SUBDIVIDE_EDGE_13_128       0x70000000
#define NV_BTM_SUBDIVIDE_EDGE_13_256       0x80000000

/* class NV_RENDER_TEXTURE_QUADRATIC */
#define NV_RENDER_TEXTURE_QUADRATIC  36
typedef volatile struct
 tagNvRenderTextureQuadratic {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /*                                 0300-0303*/
 V032 SubdivideIn;             /*                                 0304-0307*/
 V032 reserved03[0x002];
 struct
  tagNvQuadControlPointOut {
  V032 y0_x0;                  /*                                 0310-0313*/
  V032 y1_x1;                  /*                                 0314-0317*/
  V032 y2_x2;                  /*                                 0318-031b*/
  V032 y3_x3;                  /*                                 031c-031f*/
  V032 y4_x4;                  /*                                 0320-0323*/
  V032 y5_x5;                  /*                                 0324-0327*/
  V032 y6_x6;                  /*                                 0328-032b*/
  V032 y7_x7;                  /*                                 032c-032f*/
  V032 y8_x8;                  /*                                 0330-0333*/
 } ControlPointOut;
 V032 reserved04[0x007];
 struct
  tagNvQuadControlPointOut12d4 {
  V032 y0_x0;                  /* S12.4_S12.4                     0350-0353*/
  V032 y1_x1;                  /* S12.4_S12.4                     0354-0357*/
  V032 y2_x2;                  /* S12.4_S12.4                     0358-035b*/
  V032 y3_x3;                  /* S12.4_S12.4                     035c-035f*/
  V032 y4_x4;                  /* S12.4_S12.4                     0360-0363*/
  V032 y5_x5;                  /* S12.4_S12.4                     0364-0367*/
  V032 y6_x6;                  /* S12.4_S12.4                     0368-036b*/
  V032 y7_x7;                  /* S12.4_S12.4                     036c-036f*/
  V032 y8_x8;                  /* S12.4_S12.4                     0370-0373*/
 } ControlPointOut12d4;
 V032 reserved05[0x023];
 V032 Color[32];               /*                                 0400-047f*/
 V032 reserved06[0x6e0];
} NvRenderTextureQuadratic;
/*
 * Bit values for NV_RENDER_TEXTURE_QUADRATIC and
 * NV_RENDER_TEXTURE_QUADRATIC_BETA SubdivideIn()
 */
#define NV_QTM_SUBDIVIDE_HEIGHT_02_68_4    0x00000002
#define NV_QTM_SUBDIVIDE_HEIGHT_02_68_8    0x00000003
#define NV_QTM_SUBDIVIDE_HEIGHT_02_68_16   0x00000004
#define NV_QTM_SUBDIVIDE_HEIGHT_02_68_32   0x00000005
#define NV_QTM_SUBDIVIDE_HEIGHT_02_68_64   0x00000006
#define NV_QTM_SUBDIVIDE_HEIGHT_02_68_128  0x00000007
#define NV_QTM_SUBDIVIDE_HEIGHT_02_68_256  0x00000008
#define NV_QTM_SUBDIVIDE_WIDTH_06_28_4     0x00000020
#define NV_QTM_SUBDIVIDE_WIDTH_06_28_8     0x00000030
#define NV_QTM_SUBDIVIDE_WIDTH_06_28_16    0x00000040
#define NV_QTM_SUBDIVIDE_WIDTH_06_28_32    0x00000050
#define NV_QTM_SUBDIVIDE_WIDTH_06_28_64    0x00000060
#define NV_QTM_SUBDIVIDE_WIDTH_06_28_128   0x00000070
#define NV_QTM_SUBDIVIDE_WIDTH_06_28_256   0x00000080
#define NV_QTM_SUBDIVIDE_EDGE_02_1         0x00000000
#define NV_QTM_SUBDIVIDE_EDGE_02_2         0x00010000
#define NV_QTM_SUBDIVIDE_EDGE_02_4         0x00020000
#define NV_QTM_SUBDIVIDE_EDGE_02_8         0x00030000
#define NV_QTM_SUBDIVIDE_EDGE_02_16        0x00040000
#define NV_QTM_SUBDIVIDE_EDGE_02_32        0x00050000
#define NV_QTM_SUBDIVIDE_EDGE_02_64        0x00060000
#define NV_QTM_SUBDIVIDE_EDGE_02_128       0x00070000
#define NV_QTM_SUBDIVIDE_EDGE_02_256       0x00080000
#define NV_QTM_SUBDIVIDE_EDGE_68_1         0x00000000
#define NV_QTM_SUBDIVIDE_EDGE_68_2         0x00100000
#define NV_QTM_SUBDIVIDE_EDGE_68_4         0x00200000
#define NV_QTM_SUBDIVIDE_EDGE_68_8         0x00300000
#define NV_QTM_SUBDIVIDE_EDGE_68_16        0x00400000
#define NV_QTM_SUBDIVIDE_EDGE_68_32        0x00500000
#define NV_QTM_SUBDIVIDE_EDGE_68_64        0x00600000
#define NV_QTM_SUBDIVIDE_EDGE_68_128       0x00700000
#define NV_QTM_SUBDIVIDE_EDGE_68_256       0x00800000
#define NV_QTM_SUBDIVIDE_EDGE_06_1         0x00000000
#define NV_QTM_SUBDIVIDE_EDGE_06_2         0x01000000
#define NV_QTM_SUBDIVIDE_EDGE_06_4         0x02000000
#define NV_QTM_SUBDIVIDE_EDGE_06_8         0x03000000
#define NV_QTM_SUBDIVIDE_EDGE_06_16        0x04000000
#define NV_QTM_SUBDIVIDE_EDGE_06_32        0x05000000
#define NV_QTM_SUBDIVIDE_EDGE_06_64        0x06000000
#define NV_QTM_SUBDIVIDE_EDGE_06_128       0x07000000
#define NV_QTM_SUBDIVIDE_EDGE_06_256       0x08000000
#define NV_QTM_SUBDIVIDE_EDGE_28_1         0x00000000
#define NV_QTM_SUBDIVIDE_EDGE_28_2         0x10000000
#define NV_QTM_SUBDIVIDE_EDGE_28_4         0x20000000
#define NV_QTM_SUBDIVIDE_EDGE_28_8         0x30000000
#define NV_QTM_SUBDIVIDE_EDGE_28_16        0x40000000
#define NV_QTM_SUBDIVIDE_EDGE_28_32        0x50000000
#define NV_QTM_SUBDIVIDE_EDGE_28_64        0x60000000
#define NV_QTM_SUBDIVIDE_EDGE_28_128       0x70000000
#define NV_QTM_SUBDIVIDE_EDGE_28_256       0x80000000

/* class NV_RENDER_TEXTURE_BILINEAR_BETA */
#define NV_RENDER_TEXTURE_BILINEAR_BETA  52
typedef volatile struct
 tagNvRenderTextureBilinearBeta {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 SetBetaOutput;           /* P_B to BetaMax or BlendAnd      0204-0207*/
 V032 reserved02[0x03e];
 V032 SetColorFormat;          /*                                 0300-0303*/
 V032 SubdivideIn;             /*                                 0304-0307*/
 V032 reserved03[0x002];
 struct
  tagNvBiBControlPointOut {
  V032 y0_x0;                  /*                                 0310-0313*/
  V032 y1_x1;                  /*                                 0314-0317*/
  V032 y2_x2;                  /*                                 0318-031b*/
  V032 y3_x3;                  /*                                 031c-031f*/
 } ControlPointOut;
 V032 reserved04[0x00c];
 struct
  tagNvBiBControlPointOut12d4 {
  V032 y0_x0;                  /* S12.4_S12.4                     0350-0353*/
  V032 y1_x1;                  /* S12.4_S12.4                     0354-0357*/
  V032 y2_x2;                  /* S12.4_S12.4                     0358-035b*/
  V032 y3_x3;                  /* S12.4_S12.4                     035c-035f*/
 } ControlPointOut12d4;
 V032 reserved05[0x008];
 struct
  tagNvBiBControlBetaOut {
  V032 beta1_beta0;            /* S1.15_S1.15 beta at CP1 and CP0 0380-0383*/
  V032 beta3_beta2;            /* S1.15_S1.15 beta at CP3 and CP2 0384-0387*/
 } ControlBetaOut;
 V032 reserved06[0x01e];
 V032 Color[32];               /*                                 0400-047f*/
 V032 reserved07[0x6e0];
} NvRenderTextureBilinearBeta;

/* class NV_RENDER_TEXTURE_QUADRATIC_BETA */
#define NV_RENDER_TEXTURE_QUADRATIC_BETA  53
typedef volatile struct
 tagNvRenderTextureQuadraticBeta {
 V032 SetNotifyCtxDma;         /*                                 0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageOutput;          /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 SetBetaOutput;           /* P_B to BetaMax or BlendAnd      0204-0207*/
 V032 reserved02[0x03e];
 V032 SetColorFormat;          /* NV_COLOR_FORMAT, must be packed 0300-0303*/
 V032 SubdivideIn;             /* NV_QTM_SUBDIVIDE OR'd values    0304-0307*/
 V032 reserved03[0x002];
 struct
  tagNvQuadBControlPointOut {
  V032 y0_x0;                  /*                                 0310-0313*/
  V032 y1_x1;                  /*                                 0314-0317*/
  V032 y2_x2;                  /*                                 0318-031b*/
  V032 y3_x3;                  /*                                 031c-031f*/
  V032 y4_x4;                  /*                                 0320-0323*/
  V032 y5_x5;                  /*                                 0324-0327*/
  V032 y6_x6;                  /*                                 0328-032b*/
  V032 y7_x7;                  /*                                 032c-032f*/
  V032 y8_x8;                  /*                                 0330-0333*/
 } ControlPointOut;
 V032 reserved04[0x007];
 struct
  tagNvQuadBControlPointOut12d4 {
  V032 y0_x0;                  /* S12.4_S12.4 in pixels           0350-0353*/
  V032 y1_x1;                  /* S12.4_S12.4 in pixels           0354-0357*/
  V032 y2_x2;                  /* S12.4_S12.4 in pixels           0358-035b*/
  V032 y3_x3;                  /* S12.4_S12.4 in pixels           035c-035f*/
  V032 y4_x4;                  /* S12.4_S12.4 in pixels           0360-0363*/
  V032 y5_x5;                  /* S12.4_S12.4 in pixels           0364-0367*/
  V032 y6_x6;                  /* S12.4_S12.4 in pixels           0368-036b*/
  V032 y7_x7;                  /* S12.4_S12.4 in pixels           036c-036f*/
  V032 y8_x8;                  /* S12.4_S12.4 in pixels           0370-0363*/
 } ControlPointOut12d4;
 V032 reserved05[0x003];
 struct
  tagNvQuadBControlBetaOut {
  V032 beta1_beta0;            /* S1.15_S1.15 beta at CP1 and CP0 0380-0383*/
  V032 beta3_beta2;            /* S1.15_S1.15 beta at CP3 and CP2 0384-0387*/
  V032 beta5_beta4;            /* S1.15_S1.15 beta at CP5 and CP4 0388-038b*/
  V032 beta7_beta6;            /* S1.15_S1.15 beta at CP7 and CP6 038c-038f*/
  V032 na_beta8;               /* <don't care>_S1.15 beta at CP9  0390-0393*/
 } ControlBetaOut;
 V032 reserved06[0x01b];
 V032 Color[32];               /* texel colors, must be packed    0400-047f*/
 V032 reserved07[0x6e0];
} NvRenderTextureQuadraticBeta;

/***** Image Readback Class *****/

/* class NV_IMAGE_TO_MEMORY */
#define NV_IMAGE_TO_MEMORY  37
typedef volatile struct
 tagNvImageToMemory {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetImageInput;           /* NV_PATCHCORD_IMAGE              0200-0203*/
 V032 reserved02[0x03f];
 V032 SetColorFormat;          /* cannot be an unpacked format    0300-0303*/
 V032 SetImageCtxDma;          /* NV_CONTEXT_DMA_TO_MEMORY        0304-0307*/
 V032 Point;                   /* y_x S16_S16 in pixels           0308-030b*/
 V032 Size;                    /* height_width U16_U16 in pixels  030c-030f*/
 S032 ImagePitch;              /* gap in bytes between scan lines 0310-0313*/
 U032 ImageStart;              /* byte offset of top-left pixel   0314-0317*/
 V032 reserved03[0x73a];
} NvImageToMemory;

/***** Audio Classes *****/

/* class NV_AUDIO_TO_MEMORY */
#define NV_AUDIO_TO_MEMORY  38
typedef volatile struct
 tagNvAudioToMemory {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetAudioInput[56];       /* NV_PATCHCORD_AUDIO per channel  0200-02df*/
 V032 reserved02[0x008];
 V032 SetStartCtxTime;         /* NV_CONTEXT_TIME, time 0=immed.  0300-0303*/
 V032 reserved03[0x001];
 V032 SetSampleRate;           /* U16.16 in Hertz (cycles/sec.)   0308-030b*/
 V032 SetAudioFormat;          /*                                 030c-030f*/
 U032 SetNumberChannels;       /* 2=stereo (L sample then R)      0310-0313*/
 V032 reserved04[0x03b];
 V032 SetBufferCtxDma[2];      /* DMA to memory for the buffer(s) 0400-0407*/
 V032 reserved05[0x006];
 V032 SetBufferNotifyCtxDma[2];/* DMA to mem. for NvNotification  0420-0427*/
 V032 reserved06[0x006];
 U032 SetBufferStart[2];       /* offset within ctxdma in bytes   0440-0447*/
 V032 reserved07[0x006];
 U032 SetBufferLength[2];      /* in bytes                        0460-0467*/
 V032 reserved08[0x006];
 V032 SetBufferNotify[2];      /* indicates buff is ready to fill 0480-0487*/
 V032 reserved09[0x6de];
} NvAudioToMemory;

/* values for SetAudioFormat() */
#define NV_AUDIO_FORMAT_LE_S016_LINEAR     0x00000000
#define NV_AUDIO_FORMAT_LE_V008_ULAW       0x00000001
#define NV_AUDIO_FORMAT_LE_V008_ALAW       0x00000002
#define NV_AUDIO_FORMAT_LE_U008_OFFSET     0x00000003

/* class NV_AUDIO_FROM_MEMORY */
#define NV_AUDIO_FROM_MEMORY  39
typedef volatile struct
 tagNvAudioFromMemory {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetAudioOutput[56];      /* NV_PATCHCORD_AUDIO per channel  0200-02df*/
 V032 reserved02[0x008];
 V032 SetStartCtxTime;         /* NV_CONTEXT_TIME, time 0=immed.  0300-0303*/
 V032 reserved03[0x001];
 V032 SetSampleRate;           /* U16.16 in Hertz (cycles/sec.)   0308-030b*/
 V032 SetAudioFormat;          /*                                 030c-030f*/
 U032 SetNumberChannels;       /* 2=stereo (L sample then R)      0310-0313*/
 V032 SetSampleRateAdjust;     /* U16.16 sample rate multiplier   0314-0317*/
 V032 reserved04[0x03a];
 V032 SetBufferCtxDma[2];      /* DMA to memory for the buffer(s) 0400-0407*/
 V032 reserved05[0x006];
 V032 SetBufferNotifyCtxDma[2];/* DMA to mem. for NvNotification  0420-0427*/
 V032 reserved06[0x006];
 U032 SetBufferStart[2];       /* offset within ctxdma in bytes   0440-0447*/
 V032 reserved07[0x006];
 U032 SetBufferLength[2];      /* in bytes                        0460-0467*/
 V032 reserved08[0x006];
 V032 SetBufferNotify[2];      /* indicates buf is ready to be read 0480-0487*/
 V032 reserved09[0x6de];
} NvAudioFromMemory;

/* class NV_AUDIO_NOTE_FROM_MEMORY */
#define NV_AUDIO_NOTE_FROM_MEMORY  40
typedef volatile struct
 tagNvAudioNoteFromMemory {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetAudioOutput[56];      /* NV_PATCHCORD_AUDIO per channel  0200-02df*/
 V032 reserved02[0x008];
 V032 SetStartCtxTime;         /* NV_CONTEXT_TIME, time 0=immed.  0300-0303*/
 V032 reserved03[0x001];
 V032 SetStopLoopingCtxTime;   /* no more looping after this time 0308-030b*/
 V032 reserved04[0x001];
 V032 SetSampleRate;           /* U16.16 in Hertz (cycles/sec.)   0310-0313*/
 V032 SetAudioFormat;          /*                                 0314-0317*/
 U032 SetNumberChannels;       /* 2=stereo (L sample then R)      0318-031b*/
 V032 SetSampleRateAdjust;     /* U16.16 sample rate multiplier   031c-031f*/
 V032 SetPortamento;           /* U16.16 10-90% ramp in millisec. 0320-0323*/
 U032 SetVibratoDepth;         /* U0.32 fraction                  0324-0327*/
 V032 SetVibratoFrequency;     /* U16.16 in Hertz (cycles/sec)    0328-032b*/
 V032 SetVibratoAttackCtxTime; /* NV_CONTEXT_TIME                 032c-032f*/
 V032 SetVibratoAttack;        /* U16.16 10-90% ramp in millisec. 0330-0333*/
 V032 reserved05[0x033];
 V032 SetBufferCtxDma;         /* NV_CONTEXT_DMA_FROM_MEMORY      0400-0403*/
 V032 SetBufferNotifyCtxDma;   /* NV_CONTEXT_DMA_TO_MEMORY        0404-0407*/
 U032 SetBufferStart;          /* offset within ctxdma in bytes   0408-040b*/
 U032 SetBufferLength;         /* in bytes                        040c-040f*/
 U032 SetBufferLoopStart;      /* offset in bytes of loop start   0410-0413*/
 U032 SetBufferLoopLength;     /* in bytes                        0414-0417*/
 V032 SetBufferNotify;         /* indicates buf is ready to read  0418-041b*/
 V032 reserved06[0x6f9];
} NvAudioNoteFromMemory;

/* class NV_AUDIO_MIXER */
#define NV_AUDIO_MIXER  41
typedef volatile struct
 tagNvAudioMixer {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetAudioOutput;          /* NV_PATCHCORD_AUDIO              0200-0203*/
 V032 SetAudioInput[56];       /* NV_PATCHCORD_AUDIO              0204-02e3*/
 V032 reserved02[0x007];
 V032 SetOutputGain;           /* U8.24 "master" volume           0300-0303*/
 V032 SetInputGain[56];        /* U8.24                           0303-03e3*/
 V032 reserved03[0x707];
} NvAudioMixer;

/* class NV_AUDIO_ENVELOPE */
#define NV_AUDIO_ENVELOPE  42
typedef volatile struct
 tagNvAudioEnvelope {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetAudioOutput;          /* NV_PATCHCORD_AUDIO              0200-0203*/
 V032 SetAudioInput;           /* NV_PATCHCORD_AUDIO              0204-0207*/
 V032 reserved02[0x03e];
 V032 SetAttack;               /* U16.16 10-90% ramp in millisec. 0300-0303*/
 V032 SetDecay;                /* U16.16 90-10% decay in msec.    0304-0307*/
 V032 SetSustain;              /* U16.16 90-10% decay in msec.    0308-030b*/
 V032 SetRelease;              /* U16.16 90-10% decay in msec.    030c-030f*/
 V032 SetAttackCtxTime;        /* NV_CONTEXT_TIME                 0310-0313*/
 V032 reserved03[0x001];
 V032 SetReleaseCtxTime;       /* NV_CONTEXT_TIME                 0318-031b*/
 V032 reserved04[0x001];
 U032 SetSustainLevel;         /* U0.32 fraction                  0320-0323*/
 V032 reserved05[0x737];
} NvAudioEnvelope;

/* class NV_AUDIO_TREMOLO */
#define NV_AUDIO_TREMOLO  43
typedef volatile struct
 tagNvAudioTremolo {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetAudioOutput;          /* NV_PATCHCORD_AUDIO              0200-0203*/
 V032 SetAudioInput;           /* NV_PATCHCORD_AUDIO              0204-0207*/
 V032 reserved02[0x03e];
 U032 SetTremoloDepth;         /* U0.32 proportion                0300-0303*/
 V032 SetTremoloFrequency;     /* U16.16 in Hertz (cycles/sec)    0304-0307*/
 V032 SetTremoloAttackCtxTime; /* NV_CONTEXT_TIME                 0308-030b*/
 V032 SetTremoloAttack;        /* U16.16 10-90% ramp in millisec. 030c-030f*/
 V032 reserved03[0x73c];
} NvAudioTremolo;

/* class NV_CONTEXT_TIME */
#define NV_CONTEXT_TIME  44
typedef volatile struct
 tagNvContextTime {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 struct
  tagNvSetTime {
  U032 nanoseconds[2];
 } SetTime;                    /* nanoseconds since Jan. 1, 1970  0300-0307*/
 V032 reserved02[0x73e];
} NvContextTime;

/* class NV_AUDIO_SINK */
#define NV_AUDIO_SINK  45
typedef volatile struct
 tagNvAudioSink {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetAudioInput[56];       /* NV_PATCHCORD_AUDIO              0200-02df*/
 V032 reserved02[0x748];
} NvAudioSink;

/* class NV_AUDIO_SOURCE */
#define NV_AUDIO_SOURCE  46
typedef volatile struct
 tagNvAudioSource {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x03e];
 V032 SetAudioOutput;          /* NV_PATCHCHORD_AUDIO             0200-0203*/
 V032 reserved02[0x77f];
} NvAudioSource;

/* class NV_PATCHCORD_AUDIO */
#define NV_PATCHCORD_AUDIO  47
typedef volatile struct
 tagNvPatchcordAudio {
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY        0100-0103*/
 V032 SetNotify;               /* notification style, varies w/OS 0104-0107*/
 V032 reserved01[0x07e];
 V032 Validate;                /* writing any value validates     0300-0303*/
 V032 reserved02[0x73f];
} NvPatchcordAudio;

/***************************************************************************
 *                   Subchannels within a Channel                          *
 ***************************************************************************/

/*
 * The first 256 bytes of each subchannel are the NvControl registers.
 * The interpretation of writes to offets in the rest of the
 * subchannel depends on the class of the object loaded in the
 * subchannel, so the rest of the NvSubchannel structure is a union
 * of the class structures.
 */
typedef struct
 tagNvSubchannel {
 NvControl                     control;
 union {
  NvClass                      nvClass;
  NvContextDmaFromMemory       contextDmaFromMemory;
  NvContextDmaToMemory         contextDmaToMemory;
  NvTimer                      timer;
  NvChipId                     chipId;
  NvContextOrdinal             contextOrdinal;
  NvNullClass                  nullClass;
  NvPatchcordGamePort          patchcordGamePort;
  NvGamePortSource             gamePortSource;
  NvGamePortToMemory           gamePortToMemory;
  NvPatchcordVideo             patchcordVideo;
  NvVideoSink                  videoSink;
  NvVideoSwitch                videoSwitch;
  NvVideoColormap              videoColormap;
  NvPatchcordImage             patchcordImage;
  NvImageToVideo               imageToVideo;
  NvImageStencil               imageStencil;
  NvPatchcordBeta              patchcordBeta;
  NvImageBlendAnd              imageBlendAnd;
  NvBetaSolid                  betaSolid;
  NvBetaMaximum                betaMaximum;
  NvPatchcordRop               patchcordRop;
  NvImageRopAnd                imageRopAnd;
  NvRopSolid                   ropSolid;
  NvImageColorKey              imageColorKey;
  NvImagePlaneSwitch           imagePlaneSwitch;
  NvImageSolid                 imageSolid;
  NvImagePattern               imagePattern;
  NvImageBlackRectangle        imageBlackRectangle;
  NvRenderSolidPoint           renderSolidPoint;
  NvRenderSolidLine            renderSolidLine;
  NvRenderSolidLin             renderSolidLin;
  NvRenderSolidTriangle        renderSolidTriangle;
  NvRenderSolidRectangle       renderSolidRectangle;
  NvImageBlit                  imageBlit;
  NvImageFromMemory            imageFromMemory;
  NvImageFromCpu               imageFromCpu;
  NvImageMonochromeFromCpu     imageMonochromeFromCpu;
  NvRenderTextureBilinear      renderTextureBilinear;
  NvRenderTextureQuadratic     renderTextureQuadratic;
  NvRenderTextureBilinearBeta  renderTextureBilinearBeta;
  NvRenderTextureQuadraticBeta renderTextureQuadraticBeta;
  NvImageToMemory              imageToMemory;
  NvAudioToMemory              audioToMemory;
  NvAudioFromMemory            audioFromMemory;
  NvAudioNoteFromMemory        audioNoteFromMemory;
  NvAudioMixer                 audioMixer;
  NvAudioEnvelope              audioEnvelope;
  NvAudioTremolo               audioTremolo;
  NvContextTime                contextTime;
  NvAudioSink                  audioSink;
  NvAudioSource                audioSource;
  NvPatchcordAudio             patchcordAudio;
 };
} NvSubchannel;

/*
 * There are 8 subchannels in one 64KByte channel.
 */
typedef struct
 tagNvChannel {
 NvSubchannel subchannel[8];
} NvChannel;

#endif /* NV32_INCLUDED */
