 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1998 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1998  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

 

 /***************************************************************************\
|*                                                                           *|
|*                         NV Architecture Interface                         *|
|*                                                                           *|
|*  <nv32.h> defines a 32-bit wide naming convention  for the functionality  *|
|*  of NVIDIA's Unified Media Architecture (TM).                             *|
|*                                                                           *|
 \***************************************************************************/


#ifndef NV32_INCLUDED
#define NV32_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <nvtypes.h>

#include "nv332.h"

 /***************************************************************************\
|*                                NV Classes                                 *|
 \***************************************************************************/

/*
 * These structures indicate the offsets of hardware registers corresponding to
 * the methods of each class.  Since the first 256 bytes of each subchannel is 
 * the NvControlPio registers, the hexadecimal offsets in comments start at 
 * 0x100.
 */


/* class NV01_ROOT */
#define  NV01_ROOT                                                 (0x00000000)
/* NvNotification[] fields and values */
#define NV000_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv000Typedef, Nv01Root;
#define  NV000_TYPEDEF                                             nv01Root


/* class NV01_CLASS */
#define  NV01_CLASS                                                (0x00000001)
#define  NV1_CLASS                                                 (0x00000001)
/* NvNotification[] fields and values */
#define NV001_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv001Typedef, Nv01Class;
#define  Nv1Class                                                  Nv01Class
#define  nv1Class                                                  nv01Class
#define  NV001_TYPEDEF                                             nv01Class

/* class NV01_CONTEXT_DMA_FROM_MEMORY */
#define  NV01_CONTEXT_DMA_FROM_MEMORY                              (0x00000002)
#define  NV1_CONTEXT_DMA_FROM_MEMORY                               (0x00000002)
/* NvNotification[] fields and values */
#define NV002_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv002Typedef, Nv01ContextDmaFromMemory;
#define  Nv1ContextDmaFromMemory                       Nv01ContextDmaFromMemory
#define  nv1ContextDmaFromMemory                       nv01ContextDmaFromMemory
#define  NV002_TYPEDEF                                 nv01ContextDmaFromMemory


/* class NV01_CONTEXT_DMA_TO_MEMORY */
#define  NV01_CONTEXT_DMA_TO_MEMORY                                (0x00000003)
#define  NV1_CONTEXT_DMA_TO_MEMORY                                 (0x00000003)
/* NvNotification[] fields and values */
#define NV003_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv003Typedef, Nv01ContextDmaToMemory;
#define  Nv1ContextDmaToMemory                           Nv01ContextDmaToMemory
#define  nv1ContextDmaToMemory                           nv01ContextDmaToMemory
#define  NV003_TYPEDEF                                   nv01ContextDmaToMemory


/* class NV01_TIMER */
#define  NV01_TIMER                                                (0x00000004)
#define  NV1_TIMER                                                 (0x00000004)
/* NvNotification[] elements */
#define NV004_NOTIFIERS_NOTIFY                                     (0)
#define NV004_NOTIFIERS_SET_ALARM_NOTIFY                           (1)
/* NvNotification[] fields and values */
#define NV004_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV004_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV004_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV004_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV004_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV004_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV004_NOTIFY_*                    0104-0107*/
 V032 StopTransfer;            /* NV004_STOP_TRANSFER_VALUE         0108-010b*/
 V032 Reserved00[0x01d];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 Reserved01[0x05f];
 struct {                      /* start of method                   0300-    */
  U032 nanoseconds[2];         /* nsec since 1970-01-01 0=immediate    0-   7*/
 } SetAlarmTime;               /* end of method                         -0307*/
 V032 Reserved02[0x001];
 V032 SetAlarmNotify;          /* NV004_SET_ALARM_NOTIFY_*          030c-030f*/
 V032 Reserved03[0x73c];
} Nv004Typedef, Nv01Timer;
#define Nv1Timer                                                   Nv01Timer
#define nv1Timer                                                   nv01Timer
#define NV004_TYPEDEF                                              nv01Timer
/* dma method offsets, fields, and values */
#define NV004_SET_OBJECT                                           (0x00000000)
#define NV004_NO_OPERATION                                         (0x00000100)
#define NV004_NOTIFY                                               (0x00000104)
#define NV004_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV004_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV004_STOP_TRANSFER                                        (0x00000108)
#define NV004_STOP_TRANSFER_VALUE                                  (0x00000000)
#define NV004_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV004_SET_ALARM_TIME                                       (0x00000300)
#define NV004_SET_ALARM_NOTIFY                                     (0x0000030C)
#define NV004_SET_ALARM_NOTIFY_WRITE_ONLY                          (0x00000000)
#define NV004_SET_ALARM_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)

typedef V032 Nv005Typedef;

typedef V032 Nv006Typedef;

typedef V032 Nv007Typedef;

typedef V032 Nv008Typedef;

typedef V032 Nv009Typedef;

typedef V032 Nv00aTypedef;

typedef V032 Nv00bTypedef;

typedef V032 Nv00cTypedef;

typedef V032 Nv00dTypedef;

typedef V032 Nv00eTypedef;

typedef V032 Nv00fTypedef;

typedef V032 Nv010Typedef;

typedef V032 Nv011Typedef;


/* class NV01_BETA_SOLID */
#define  NV01_BETA_SOLID                                           (0x00000012)
#define  NV1_BETA_SOLID                                            (0x00000012)
/* NvNotification[] elements */
#define NV012_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV012_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV012_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV012_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV012_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV012_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV012_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV012_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 Reserved01[0x01f];
 V032 SetBetaOutput;           /* NV01_PATCHCORD_BETA               0200-0203*/
 V032 Reserved02[0x03f];
 V032 SetBeta1d31;             /* S1d31 beta value                  0300-0303*/
 V032 Reserved03[0x73f];
} Nv012Typedef, Nv01BetaSolid;
#define Nv1BetaSolid                                              Nv01BetaSolid
#define nv1BetaSolid                                              nv01BetaSolid
#define NV012_TYPEDEF                                             nv01BetaSolid
/* dma method offsets, fields, and values */
#define NV012_SET_OBJECT                                           (0x00000000)
#define NV012_NO_OPERATION                                         (0x00000100)
#define NV012_NOTIFY                                               (0x00000104)
#define NV012_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV012_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV012_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV012_SET_BETA_OUTPUT                                      (0x00000200)
#define NV012_SET_BETA_1D31                                        (0x00000300)

typedef V032 Nv013Typedef;

typedef V032 Nv014Typedef;

typedef V032 Nv015Typedef;

typedef V032 Nv016Typedef;


/* class NV01_IMAGE_SOLID */
#define  NV01_IMAGE_SOLID                                          (0x00000017)
#define  NV1_IMAGE_SOLID                                           (0x00000017)
/* NvNotification[] elements */
#define NV017_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV017_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV017_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV017_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV017_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV017_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV017_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV017_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 Reserved01[0x01f];
 V032 SetImageOutput;          /* NV01_PATCHCORD_IMAGE              0200-0203*/
 V032 Reserved02[0x03f];
 V032 SetColorFormat;          /* NV017_SET_COLOR_FORMAT_*          0300-0303*/
 V032 SetColor;                /* source color                      0304-0307*/
 V032 Reserved03[0x73e];
} Nv017Typedef, Nv01ImageSolid;
#define Nv1ImageSolid                                            Nv01ImageSolid
#define nv1ImageSolid                                            nv01ImageSolid
#define NV017_TYPEDEF                                            nv01ImageSolid
/* dma method offsets, fields, and values */
#define NV017_SET_OBJECT                                           (0x00000000)
#define NV017_NO_OPERATION                                         (0x00000100)
#define NV017_NOTIFY                                               (0x00000104)
#define NV017_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV017_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV017_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV017_SET_IMAGE_OUTPUT                                     (0x00000200)
#define NV017_SET_COLOR_FORMAT                                     (0x00000300)
#define NV017_SET_COLOR_FORMAT_LE_X16A8Y8                          (0x00000001)
#define NV017_SET_COLOR_FORMAT_LE_X16A1R5G5B5                      (0x00000002)
#define NV017_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000003)
#define NV017_SET_COLOR                                            (0x00000304)


/* class NV01_IMAGE_PATTERN */
#define  NV01_IMAGE_PATTERN                                        (0x00000018)
#define  NV1_IMAGE_PATTERN                                         (0x00000018)
/* NvNotification[] elements */
#define NV018_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV018_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV018_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV018_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV018_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV018_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV018_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV018_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 Reserved01[0x01f];
 V032 SetImageOutput;          /* NV01_PATCHCORD_IMAGE              0200-0203*/
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
#define Nv1ImagePattern                                        Nv01ImagePattern
#define nv1ImagePattern                                        nv01ImagePattern
#define NV018_TYPEDEF                                          nv01ImagePattern
/* dma method offsets, fields, and values */
#define NV018_SET_OBJECT                                           (0x00000000)
#define NV018_NO_OPERATION                                         (0x00000100)
#define NV018_NOTIFY                                               (0x00000104)
#define NV018_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV018_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV018_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV018_SET_IMAGE_OUTPUT                                     (0x00000200)
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
#define  NV01_IMAGE_BLACK_RECTANGLE                                (0x00000019)
#define  NV1_IMAGE_BLACK_RECTANGLE                                 (0x00000019)
/* NvNotification[] elements */
#define NV019_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV019_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV019_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV019_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV019_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV019_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV019_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV019_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 Reserved01[0x01f];
 V032 SetImageOutput;          /* NV01_PATCHCORD_IMAGE              0200-0203*/
 V032 Reserved02[0x03f];
 V032 SetPoint;                /* y_x S16_S16                       0300-0303*/
 V032 SetSize;                 /* height_width U16_U16              0304-0307*/
 V032 Reserved03[0x73e];
} Nv019Typedef, Nv01ImageBlackRectangle;
#define Nv1ImageBlackRectangle                          Nv01ImageBlackRectangle
#define nv1ImageBlackRectangle                          nv01ImageBlackRectangle
#define NV019_TYPEDEF                                   nv01ImageBlackRectangle
/* dma method offsets, fields, and values */
#define NV019_SET_OBJECT                                           (0x00000000)
#define NV019_NO_OPERATION                                         (0x00000100)
#define NV019_NOTIFY                                               (0x00000104)
#define NV019_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV019_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV019_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV019_SET_IMAGE_OUTPUT                                     (0x00000200)
#define NV019_SET_POINT                                            (0x00000300)
#define NV019_SET_POINT_X                                          15:0
#define NV019_SET_POINT_Y                                          31:16
#define NV019_SET_SIZE                                             (0x00000304)
#define NV019_SET_SIZE_WIDTH                                       15:0
#define NV019_SET_SIZE_HEIGHT                                      31:16

typedef V032 Nv01aTypedef;

typedef V032 Nv01bTypedef;


/* class NV01_RENDER_SOLID_LIN */
#define  NV01_RENDER_SOLID_LIN                                     (0x0000001C)
#define  NV1_RENDER_SOLID_LIN                                      (0x0000001C)
/* NvNotification[] elements */
#define NV01C_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV01C_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV01C_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV01C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV01C_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV01C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV01C_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV01C_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextClipRectangle; /* NV01_IMAGE_BLACK_RECTANGLE        0184-0187*/
 V032 SetContextPattern;       /* NV01_IMAGE_PATTERN                0188-018b*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  018c-018f*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0190-0193*/
 V032 SetContextSurface;       /* NV03_CONTEXT_SURFACE_0            0194-0197*/
 V032 Reserved01[0x059];
 V032 SetOperation;            /* NV01C_SET_OPERATION_*             02fc-02ff*/
 V032 SetColorFormat;          /* NV01C_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Color;                   /* source color                      0304-0307*/
 V032 Reserved02[0x03e];
 struct {                      /* start of aliased methods in array 0400-    */
  V032 point0;                 /* y_x S16_S16 in pixels                0-   3*/
  V032 point1;                 /* y_x S16_S16 in pixels                4-   7*/
 } Lin[16];                    /* end of aliased methods in array       -047f*/
 struct {                      /* start of aliased methods in array 0480-    */
  S032 point0X;                /* in pixels, 0 at left                 0-   3*/
  S032 point0Y;                /* in pixels, 0 at top                  4-   7*/
  S032 point1X;                /* in pixels, 0 at left                 8-   b*/
  S032 point1Y;                /* in pixels, 0 at top                  c-   f*/
 } Lin32[8];                   /* end of aliased methods in array       -04ff*/
 V032 PolyLin[32];             /* y_x S16_S16 in pixels             0500-057f*/
 struct {                      /* start of aliased methods in array 0580-    */
  S032 x;                      /* in pixels, 0 at left                 0-   3*/
  S032 y;                      /* in pixels, 0 at top                  4-   7*/
 } PolyLin32[16];              /* end of aliased methods in array       -05ff*/
 struct {                      /* start of aliased methods in array 0600-    */
  V032 color;                  /* source color                         0-   3*/
  V032 point;                  /* y_x S16_S16 in pixels                4-   7*/
 } ColorPolyLin[16];           /* end of aliased methods in array       -067f*/
 V032 Reserved03[0x660];
} Nv01cTypedef, Nv01RenderSolidLin;
#define Nv1RenderSolidLin                                    Nv01RenderSolidLin
#define nv1RenderSolidLin                                    nv01RenderSolidLin
#define NV01C_TYPEDEF                                        nv01RenderSolidLin
/* dma method offsets, fields, and values */
#define NV01C_SET_OBJECT                                           (0x00000000)
#define NV01C_NO_OPERATION                                         (0x00000100)
#define NV01C_NOTIFY                                               (0x00000104)
#define NV01C_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV01C_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV01C_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV01C_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000184)
#define NV01C_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV01C_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV01C_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV01C_SET_CONTEXT_SURFACE                                  (0x00000194)
#define NV01C_SET_OPERATION                                        (0x000002FC)
#define NV01C_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV01C_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV01C_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV01C_SET_COLOR_FORMAT                                     (0x00000300)
#define NV01C_SET_COLOR_FORMAT_LE_X24Y8                            (0x00000001)
#define NV01C_SET_COLOR_FORMAT_LE_X17R5G5B5                        (0x00000002)
#define NV01C_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000003)
#define NV01C_SET_COLOR_FORMAT_LE_X16Y16                           (0x00000004)
#define NV01C_COLOR                                                (0x00000304)
#define NV01C_LIN(a)                                               (0x00000400\
                                                                   +(a)*0x0008)
#define NV01C_LIN_POINT0(a)                                        (0x00000400\
                                                                   +(a)*0x0008)
#define NV01C_LIN_POINT0_X                                         15:0
#define NV01C_LIN_POINT0_Y                                         31:16
#define NV01C_LIN_POINT1(a)                                        (0x00000404\
                                                                   +(a)*0x0008)
#define NV01C_LIN_POINT1_X                                         15:0
#define NV01C_LIN_POINT1_Y                                         31:16
#define NV01C_LIN32(a)                                             (0x00000480\
                                                                   +(a)*0x0010)
#define NV01C_LIN32_POINT0_X(a)                                    (0x00000480\
                                                                   +(a)*0x0010)
#define NV01C_LIN32_POINT0_Y(a)                                    (0x00000484\
                                                                   +(a)*0x0010)
#define NV01C_LIN32_POINT1_X(a)                                    (0x00000488\
                                                                   +(a)*0x0010)
#define NV01C_LIN32_POINT1_Y(a)                                    (0x0000048C\
                                                                   +(a)*0x0010)
#define NV01C_POLY_LIN(a)                                          (0x00000500\
                                                                   +(a)*0x0004)
#define NV01C_POLY_LIN_X                                           15:0
#define NV01C_POLY_LIN_Y                                           31:16
#define NV01C_POLY_LIN32(a)                                        (0x00000580\
                                                                   +(a)*0x0008)
#define NV01C_POLY_LIN32_X(a)                                      (0x00000580\
                                                                   +(a)*0x0008)
#define NV01C_POLY_LIN32_Y(a)                                      (0x00000584\
                                                                   +(a)*0x0008)
#define NV01C_COLOR_POLY_LIN(a)                                    (0x00000600\
                                                                   +(a)*0x0008)
#define NV01C_COLOR_POLY_LIN_COLOR(a)                              (0x00000600\
                                                                   +(a)*0x0008)
#define NV01C_COLOR_POLY_LIN_POINT(a)                              (0x00000604\
                                                                   +(a)*0x0008)
#define NV01C_COLOR_POLY_LIN_POINT_X                               15:0
#define NV01C_COLOR_POLY_LIN_POINT_Y                               31:16


/* class NV01_RENDER_SOLID_TRIANGLE */
#define  NV01_RENDER_SOLID_TRIANGLE                                (0x0000001D)
#define  NV1_RENDER_SOLID_TRIANGLE                                 (0x0000001D)
/* NvNotification[] elements */
#define NV01D_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV01D_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV01D_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV01D_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV01D_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV01D_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV01D_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV01D_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextClipRectangle; /* NV01_IMAGE_BLACK_RECTANGLE        0184-0187*/
 V032 SetContextPattern;       /* NV01_IMAGE_PATTERN                0188-018b*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  018c-018f*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0190-0193*/
 V032 SetContextSurface;       /* NV03_CONTEXT_SURFACE_0            0194-0197*/
 V032 Reserved01[0x059];
 V032 SetOperation;            /* NV01D_SET_OPERATION_*             02fc-02ff*/
 V032 SetColorFormat;          /* NV01D_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Color;                   /* source color                      0304-0307*/
 V032 Reserved02[0x002];
 V032 TrianglePoint0;          /* y_x S16_S16 in pixels             0310-0313*/
 V032 TrianglePoint1;          /* y_x S16_S16 in pixels             0314-0317*/
 V032 TrianglePoint2;          /* y_x S16_S16 in pixels             0318-031b*/
 V032 Reserved03[0x001];
 S032 Triangle32Point0X;       /* in pixels, 0 at left              0320-0323*/
 S032 Triangle32Point0Y;       /* in pixels, 0 at top               0324-0327*/
 S032 Triangle32Point1X;       /* in pixels, 0 at left              0328-032b*/
 S032 Triangle32Point1Y;       /* in pixels, 0 at top               032c-032f*/
 S032 Triangle32Point2X;       /* in pixels, 0 at left              0330-0333*/
 S032 Triangle32Point2Y;       /* in pixels, 0 at top               0334-0337*/
 V032 Reserved04[0x032];
 V032 Trimesh[32];             /* y_x S16_S16 in pixels             0400-047f*/
 struct {                      /* start of aliased methods in array 0480-    */
  S032 x;                      /* in pixels, 0 at left                 0-   3*/
  S032 y;                      /* in pixels, 0 at top                  4-   7*/
 } Trimesh32[16];              /* end of aliased methods in array       -04ff*/
 struct {                      /* start of aliased methods in array 0500-    */
  V032 color;                  /* source color                         0-   3*/
  V032 y0_x0;                  /* S16_S16 in pixels, 0 at top left     4-   7*/
  V032 y1_x1;                  /* S16_S16 in pixels, 0 at top left     8-   b*/
  V032 y2_x2;                  /* S16_S16 in pixels, 0 at top left     c-   f*/
 } ColorTriangle[8];           /* end of aliased methods in array       -057f*/
 struct {                      /* start of aliased methods in array 0580-    */
  V032 color;                  /* source color                         0-   3*/
  V032 y_x;                    /* S16_S16 in pixels, 0 at top left     4-   7*/
 } ColorTrimesh[16];           /* end of aliased methods in array       -05ff*/
 V032 Reserved05[0x680];
} Nv01dTypedef, Nv01RenderSolidTriangle;
#define Nv1RenderSolidTriangle                          Nv01RenderSolidTriangle
#define nv1RenderSolidTriangle                          nv01RenderSolidTriangle
#define NV01D_TYPEDEF                                   nv01RenderSolidTriangle
/* dma method offsets, fields, and values */
#define NV01D_SET_OBJECT                                           (0x00000000)
#define NV01D_NO_OPERATION                                         (0x00000100)
#define NV01D_NOTIFY                                               (0x00000104)
#define NV01D_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV01D_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV01D_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV01D_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000184)
#define NV01D_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV01D_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV01D_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV01D_SET_CONTEXT_SURFACE                                  (0x00000194)
#define NV01D_SET_OPERATION                                        (0x000002FC)
#define NV01D_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV01D_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV01D_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV01D_SET_COLOR_FORMAT                                     (0x00000300)
#define NV01D_SET_COLOR_FORMAT_LE_X24Y8                            (0x00000001)
#define NV01D_SET_COLOR_FORMAT_LE_X17R5G5B5                        (0x00000002)
#define NV01D_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000003)
#define NV01D_SET_COLOR_FORMAT_LE_X16Y16                           (0x00000004)
#define NV01D_COLOR                                                (0x00000304)
#define NV01D_TRIANGLE_POINT0                                      (0x00000310)
#define NV01D_TRIANGLE_POINT0_X                                    15:0
#define NV01D_TRIANGLE_POINT0_Y                                    31:16
#define NV01D_TRIANGLE_POINT1                                      (0x00000314)
#define NV01D_TRIANGLE_POINT1_X                                    15:0
#define NV01D_TRIANGLE_POINT1_Y                                    31:16
#define NV01D_TRIANGLE_POINT2                                      (0x00000318)
#define NV01D_TRIANGLE_POINT2_X                                    15:0
#define NV01D_TRIANGLE_POINT2_Y                                    31:16
#define NV01D_TRIANGLE32_POINT0_X                                  (0x00000320)
#define NV01D_TRIANGLE32_POINT0_Y                                  (0x00000324)
#define NV01D_TRIANGLE32_POINT1_X                                  (0x00000328)
#define NV01D_TRIANGLE32_POINT1_Y                                  (0x0000032C)
#define NV01D_TRIANGLE32_POINT2_X                                  (0x00000330)
#define NV01D_TRIANGLE32_POINT2_Y                                  (0x00000334)
#define NV01D_TRIMESH(a)                                           (0x00000400\
                                                                   +(a)*0x0004)
#define NV01D_TRIMESH_X                                            15:0
#define NV01D_TRIMESH_Y                                            31:16
#define NV01D_TRIMESH32(a)                                         (0x00000480\
                                                                   +(a)*0x0008)
#define NV01D_TRIMESH32_X(a)                                       (0x00000480\
                                                                   +(a)*0x0008)
#define NV01D_TRIMESH32_Y(a)                                       (0x00000484\
                                                                   +(a)*0x0008)
#define NV01D_COLOR_TRIANGLE(a)                                    (0x00000500\
                                                                   +(a)*0x0010)
#define NV01D_COLOR_TRIANGLE_COLOR(a)                              (0x00000500\
                                                                   +(a)*0x0010)
#define NV01D_COLOR_TRIANGLE_POINT0(a)                             (0x00000504\
                                                                   +(a)*0x0010)
#define NV01D_COLOR_TRIANGLE_POINT0_X                              15:0
#define NV01D_COLOR_TRIANGLE_POINT0_Y                              31:16
#define NV01D_COLOR_TRIANGLE_POINT1(a)                             (0x00000508\
                                                                   +(a)*0x0010)
#define NV01D_COLOR_TRIANGLE_POINT1_X                              15:0
#define NV01D_COLOR_TRIANGLE_POINT1_Y                              31:16
#define NV01D_COLOR_TRIANGLE_POINT2(a)                             (0x0000050C\
                                                                   +(a)*0x0010)
#define NV01D_COLOR_TRIANGLE_POINT2_X                              15:0
#define NV01D_COLOR_TRIANGLE_POINT2_Y                              31:16
#define NV01D_COLOR_TRIMESH(a)                                     (0x00000580\
                                                                   +(a)*0x0008)
#define NV01D_COLOR_TRIMESH_COLOR(a)                               (0x00000580\
                                                                   +(a)*0x0008)
#define NV01D_COLOR_TRIMESH_POINT(a)                               (0x00000584\
                                                                   +(a)*0x0008)
#define NV01D_COLOR_TRIMESH_POINT_X                                15:0
#define NV01D_COLOR_TRIMESH_POINT_Y                                31:16


/* class NV01_RENDER_SOLID_RECTANGLE */
#define  NV01_RENDER_SOLID_RECTANGLE                               (0x0000001E)
#define  NV1_RENDER_SOLID_RECTANGLE                                (0x0000001E)
/* NvNotification[] elements */
#define NV01E_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV01E_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV01E_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV01E_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV01E_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV01E_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV01E_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV01E_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextClipRectangle; /* NV01_IMAGE_BLACK_RECTANGLE        0184-0187*/
 V032 SetContextPattern;       /* NV01_IMAGE_PATTERN                0188-018b*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  018c-018f*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0190-0193*/
 V032 SetContextSurface;       /* NV03_CONTEXT_SURFACE_0            0194-0197*/
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
#define Nv1RenderSolidRectangle                        Nv01RenderSolidRectangle
#define nv1RenderSolidRectangle                        nv01RenderSolidRectangle
#define NV01E_TYPEDEF                                  nv01RenderSolidRectangle
/* dma method offsets, fields, and values */
#define NV01E_SET_OBJECT                                           (0x00000000)
#define NV01E_NO_OPERATION                                         (0x00000100)
#define NV01E_NOTIFY                                               (0x00000104)
#define NV01E_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV01E_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
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
#define  NV01_IMAGE_BLIT                                           (0x0000001F)
#define  NV1_IMAGE_BLIT                                            (0x0000001F)
/* NvNotification[] elements */
#define NV01F_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV01F_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV01F_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV01F_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV01F_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV01F_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV01F_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV01F_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextColorKey;      /* NV01_IMAGE_SOLID                  0184-0187*/
 V032 SetContextClipRectangle; /* NV01_IMAGE_BLACK_RECTANGLE        0188-018b*/
 V032 SetContextPattern;       /* NV01_IMAGE_PATTERN                018c-018f*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  0190-0193*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0194-0197*/
 V032 SetContextSurfaceSource; /* NV03_CONTEXT_SURFACE_1            0198-019b*/
 V032 SetContextSurfaceDestin; /* NV03_CONTEXT_SURFACE_0            019c-019f*/
 V032 Reserved01[0x057];
 V032 SetOperation;            /* NV01F_SET_OPERATION_*             02fc-02ff*/
 V032 ControlPointIn;          /* y_x U16_U16, pixels, top left src 0300-0303*/
 V032 ControlPointOut;         /* y_x S16_S16, pixels, top left dst 0304-0307*/
 V032 Size;                    /* height_width U16_U16 in pixels    0308-030b*/
 V032 Reserved02[0x73d];
} Nv01fTypedef, Nv01ImageBlit;
#define Nv1ImageBlit                                              Nv01ImageBlit
#define nv1ImageBlit                                              nv01ImageBlit
#define NV01F_TYPEDEF                                             nv01ImageBlit
/* dma method offsets, fields, and values */
#define NV01F_SET_OBJECT                                           (0x00000000)
#define NV01F_NO_OPERATION                                         (0x00000100)
#define NV01F_NOTIFY                                               (0x00000104)
#define NV01F_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV01F_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV01F_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV01F_SET_CONTEXT_COLOR_KEY                                (0x00000184)
#define NV01F_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000188)
#define NV01F_SET_CONTEXT_PATTERN                                  (0x0000018C)
#define NV01F_SET_CONTEXT_ROP                                      (0x00000190)
#define NV01F_SET_CONTEXT_BETA1                                    (0x00000194)
#define NV01F_SET_CONTEXT_SURFACE_SOURCE                           (0x00000198)
#define NV01F_SET_CONTEXT_SURFACE_DESTIN                           (0x0000019C)
#define NV01F_SET_OPERATION(s)                                     (0x000002FC)
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

typedef V032 Nv020Typedef;


/* class NV01_IMAGE_FROM_CPU */
#define  NV01_IMAGE_FROM_CPU                                       (0x00000021)
#define  NV1_IMAGE_FROM_CPU                                        (0x00000021)
/* NvNotification[] elements */
#define NV021_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV021_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV021_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV021_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV021_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV021_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV021_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV021_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextColorKey;      /* NV01_IMAGE_SOLID                  0184-0187*/
 V032 SetContextClipRectangle; /* NV01_IMAGE_BLACK_RECTANGLE        0188-018b*/
 V032 SetContextPattern;       /* NV01_IMAGE_PATTERN                018c-018f*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  0190-0193*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0194-0197*/
 V032 SetContextSurface;       /* NV03_CONTEXT_SURFACE_0            0198-019b*/
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
#define Nv1ImageFromCpu                                        Nv01ImageFromCpu
#define nv1ImageFromCpu                                        nv01ImageFromCpu
#define NV021_TYPEDEF                                          nv01ImageFromCpu
/* dma method offsets, fields, and values */
#define NV021_SET_OBJECT                                           (0x00000000)
#define NV021_NO_OPERATION                                         (0x00000100)
#define NV021_NOTIFY                                               (0x00000104)
#define NV021_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV021_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
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

typedef V032 Nv022Typedef;

typedef V032 Nv023Typedef;

typedef V032 Nv024Typedef;

typedef V032 Nv025Typedef;

typedef V032 Nv026Typedef;

typedef V032 Nv027Typedef;

typedef V032 Nv028Typedef;

typedef V032 Nv029Typedef;

typedef V032 Nv02aTypedef;

typedef V032 Nv02bTypedef;

typedef V032 Nv02cTypedef;

typedef V032 Nv02dTypedef;

typedef V032 Nv02eTypedef;

typedef V032 Nv02fTypedef;


/* class NV01_NULL */
#define  NV01_NULL                                                 (0x00000030)
#define  NV1_NULL                                                  (0x00000030)
/* NvNotification[] fields and values */
#define NV030_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv030Typedef, Nv01Null;
#define  Nv1Null                                                   Nv01Null
#define  nv1Null                                                   nv01Null
#define  NV030_TYPEDEF                                             nv01Null

typedef V032 Nv031Typedef;

typedef V032 Nv032Typedef;

typedef V032 Nv033Typedef;

typedef V032 Nv034Typedef;

typedef V032 Nv035Typedef;



/* class NV03_STRETCHED_IMAGE_FROM_CPU */
#define  NV03_STRETCHED_IMAGE_FROM_CPU                             (0x00000036)
#define  NV3_STRETCHED_IMAGE_FROM_CPU                              (0x00000036)
/* NvNotification[] elements */
#define NV036_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV036_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV036_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV036_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV036_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV036_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV036_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV036_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextColorKey;      /* NV01_IMAGE_SOLID                  0184-0187*/
 V032 SetContextPattern;       /* NV01_IMAGE_PATTERN                0188-018b*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  018c-018f*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0190-0193*/
 V032 SetContextSurface;       /* NV03_CONTEXT_SURFACE_0            0194-0197*/
 V032 Reserved01[0x059];
 V032 SetOperation;            /* NV036_SET_OPERATION_*             02fc-02ff*/
 V032 SetColorFormat;          /* NV036_SET_COLOR_FORMAT_*          0300-0303*/
 V032 SizeIn;                  /* height_width U16_U16 in texels    0304-0307*/
 V032 DeltaDxDu;               /* S12d20 ratio dx/du                0308-030b*/
 V032 DeltaDyDv;               /* S12d20 ratio dy/dv                030c-030f*/
 V032 ClipPoint;               /* y_x S16_S16                       0310-0313*/
 V032 ClipSize;                /* height_width U16_U16              0314-0317*/
 V032 Point12d4;               /* y_x S12d4_S12d4 in pixels         0318-031b*/
 V032 Reserved02[0x039];
 V032 Color[1792];             /* source colors (packed texels)     0400-1fff*/
} Nv036Typedef, Nv03StretchedImageFromCpu;
#define Nv3StretchedImageFromCpu                      Nv03StretchedImageFromCpu
#define nv3StretchedImageFromCpu                      nv03StretchedImageFromCpu
#define NV036_TYPEDEF                                 nv03StretchedImageFromCpu
/* dma method offsets, fields, and values */
#define NV036_SET_OBJECT                                           (0x00000000)
#define NV036_NO_OPERATION                                         (0x00000100)
#define NV036_NOTIFY                                               (0x00000104)
#define NV036_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV036_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV036_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV036_SET_CONTEXT_COLOR_KEY                                (0x00000184)
#define NV036_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV036_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV036_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV036_SET_CONTEXT_SURFACE                                  (0x00000194)
#define NV036_SET_OPERATION                                        (0x000002FC)
#define NV036_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV036_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV036_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV036_SET_COLOR_FORMAT                                     (0x00000300)
#define NV036_SET_COLOR_FORMAT_LE_Y8                               (0x00000001)
#define NV036_SET_COLOR_FORMAT_LE_A1R5G5B5                         (0x00000002)
#define NV036_SET_COLOR_FORMAT_LE_X1R5G5B5                         (0x00000003)
#define NV036_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000004)
#define NV036_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000005)
#define NV036_SIZE_IN                                              (0x00000304)
#define NV036_SIZE_IN_WIDTH                                        15:0
#define NV036_SIZE_IN_HEIGHT                                       31:16
#define NV036_DELTA_DX_DU                                          (0x00000308)
#define NV036_DELTA_DY_DV                                          (0x0000030C)
#define NV036_CLIP_POINT                                           (0x00000310)
#define NV036_CLIP_POINT_X                                         15:0
#define NV036_CLIP_POINT_Y                                         31:16
#define NV036_CLIP_SIZE                                            (0x00000314)
#define NV036_CLIP_SIZE_WIDTH                                      15:0
#define NV036_CLIP_SIZE_HEIGHT                                     31:16
#define NV036_POINT_12D4                                           (0x00000318)
#define NV036_POINT_12D4_X                                         15:0
#define NV036_POINT_12D4_Y                                         31:16
#define NV036_COLOR(a)                                             (0x00000400\
                                                                   +(a)*0x0004)


/* class NV03_SCALED_IMAGE_FROM_MEMORY */
#define  NV03_SCALED_IMAGE_FROM_MEMORY                             (0x00000037)
#define  NV3_SCALED_IMAGE_FROM_MEMORY                              (0x00000037)
/* NvNotification[] elements */
#define NV037_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV037_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV037_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV037_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV037_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV037_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV037_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV037_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaImage;      /* NV01_CONTEXT_DMA_FROM_MEMORY      0184-0187*/
 V032 SetContextPattern;       /* NV01_IMAGE_PATTERN                0188-018b*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  018c-018f*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0190-0193*/
 V032 SetContextSurface;       /* NV03_CONTEXT_SURFACE_0            0194-0197*/
 V032 Reserved01[0x05A];
 V032 SetColorFormat;          /* NV037_SET_COLOR_FORMAT_*          0300-0303*/
 V032 SetOperation;            /* NV037_SET_OPERATION_*             0304-0307*/
 V032 ClipPoint;               /* y_x S16_S16                       0308-030b*/
 V032 ClipSize;                /* height_width U16_U16              030c-030f*/
 V032 ImageOutPoint;           /* y_x S16_S16                       0310-0313*/
 V032 ImageOutSize;            /* height_width U16_U16              0314-0317*/
 V032 DeltaDuDx;               /* S12d20 ratio du/dx                0318-031b*/
 V032 DeltaDvDy;               /* S12d20 ratio dv/dy                031c-031f*/
 V032 Reserved02[0x038];
 V032 ImageInSize;             /* height_width U16_U16              0400-0403*/
 U032 ImageInPitch;            /* bytes, vertical pixel delta       0404-0407*/
 U032 ImageInOffset;           /* byte offset of top-left texel     0408-040b*/
 V032 ImageInPoint;            /* v_u U12d4_U12d4                   040c-040f*/
 V032 Reserved03[0x6fc];
} Nv037Typedef, Nv03ScaledImageFromMemory;
#define Nv3ScaledImageFromMemory                      Nv03ScaledImageFromMemory
#define nv3ScaledImageFromMemory                      nv03ScaledImageFromMemory
#define NV037_TYPEDEF                                 nv03ScaledImageFromMemory
/* dma method offsets, fields, and values */
#define NV037_SET_OBJECT                                           (0x00000000)
#define NV037_NO_OPERATION                                         (0x00000100)
#define NV037_NOTIFY                                               (0x00000104)
#define NV037_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV037_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV037_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV037_SET_CONTEXT_DMA_IMAGE                                (0x00000184)
#define NV037_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV037_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV037_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV037_SET_CONTEXT_SURFACE                                  (0x00000194)
#define NV037_SET_COLOR_FORMAT                                     (0x00000300)
#define NV037_SET_COLOR_FORMAT_LE_A1R5G5B5                         (0x00000001)
#define NV037_SET_COLOR_FORMAT_LE_X1R5G5B5                         (0x00000002)
#define NV037_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000003)
#define NV037_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000004)
#define NV037_SET_COLOR_FORMAT_LE_V8YB8U8YA8                       (0x00000005)
#define NV037_SET_COLOR_FORMAT_LE_YB8V8YA8U8                       (0x00000006)
#define NV037_SET_OPERATION                                        (0x00000304)
#define NV037_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV037_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV037_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV037_CLIP_POINT                                           (0x00000308)
#define NV037_CLIP_POINT_X                                         15:0
#define NV037_CLIP_POINT_Y                                         31:16
#define NV037_CLIP_SIZE                                            (0x0000030C)
#define NV037_CLIP_SIZE_WIDTH                                      15:0
#define NV037_CLIP_SIZE_HEIGHT                                     31:16
#define NV037_IMAGE_OUT_POINT                                      (0x00000310)
#define NV037_IMAGE_OUT_POINT_X                                    15:0
#define NV037_IMAGE_OUT_POINT_Y                                    31:16
#define NV037_IMAGE_OUT_SIZE                                       (0x00000314)
#define NV037_IMAGE_OUT_SIZE_WIDTH                                 15:0
#define NV037_IMAGE_OUT_SIZE_HEIGHT                                31:16
#define NV037_DELTA_DU_DX                                          (0x00000318)
#define NV037_DELTA_DV_DY                                          (0x0000031C)
#define NV037_IMAGE_IN_SIZE                                        (0x00000400)
#define NV037_IMAGE_IN_SIZE_WIDTH                                  15:0
#define NV037_IMAGE_IN_SIZE_HEIGHT                                 31:16
#define NV037_IMAGE_IN_PITCH                                       (0x00000404)
#define NV037_IMAGE_IN_OFFSET                                      (0x00000408)
#define NV037_IMAGE_IN_POINT                                       (0x0000040C)
#define NV037_IMAGE_IN_POINT_U                                     15:0
#define NV037_IMAGE_IN_POINT_V                                     31:16


/* class NV04_DVD_SUBPICTURE */
#define  NV04_DVD_SUBPICTURE                                       (0x00000038)
#define  NV4_DVD_SUBPICTURE                                        (0x00000038)
/* NvNotification[] elements */
#define NV038_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV038_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV038_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV038_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV038_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV038_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV038_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV038_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaOverlay;    /* NV01_CONTEXT_DMA_FROM_MEMORY      0184-0187*/
 V032 SetContextDmaImageIn;    /* NV01_CONTEXT_DMA_FROM_MEMORY      0188-018b*/
 V032 SetContextDmaImageOut;   /* NV01_CONTEXT_DMA_TO_MEMORY        018c-018f*/
 V032 Reserved01[0x05c];
 V032 ImageOutPoint;           /* y_x S16_S16                       0300-0303*/
 V032 ImageOutSize;            /* height_width U16_U16              0304-0307*/
 U032 ImageOutFormat;          /* color_pitch V16_U16               0308-030b*/
 U032 ImageOutOffset;          /* byte offset of top-left pixel     030c-030f*/
 V032 ImageInDeltaDuDx;        /* S12d20 ratio du/dx                0310-0313*/
 V032 ImageInDeltaDvDy;        /* S12d20 ratio dv/dy                0314-0317*/
 V032 ImageInSize;             /* height_width U16_U16 in texels    0318-031b*/
 V032 ImageInFormat;           /* color_pitch V16_U16               031c-031f*/
 U032 ImageInOffset;           /* byte offset of top-left texel     0320-0323*/
 V032 ImageInPoint;            /* v_u U12d4_U12d4 in texels         0324-0317*/
 V032 OverlayDeltaDuDx;        /* S12d20 ratio du/dx                0328-032b*/
 V032 OverlayDeltaDvDy;        /* S12d20 ratio dv/dy                032c-032f*/
 V032 OverlaySize;             /* height_width U16_U16 in texels    0330-0333*/
 V032 OverlayFormat;           /* color_pitch V16_U16               0334-0337*/
 U032 OverlayOffset;           /* byte offset of top-left texel     0338-033b*/
 V032 OverlayPoint;            /* v_u U12d4_U12d4 in texels         033c-033f*/
 V032 Reserved02[0x730];
} Nv038Typedef, Nv04DvdSubpicture;
#define Nv4DvdSubpicture                                      Nv04DvdSubpicture
#define nv4DvdSubpicture                                      nv04DvdSubpicture
#define NV038_TYPEDEF                                         nv04DvdSubpicture
/* dma method offsets, fields, and values */
#define NV038_SET_OBJECT                                           (0x00000000)
#define NV038_NO_OPERATION                                         (0x00000100)
#define NV038_NOTIFY                                               (0x00000104)
#define NV038_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV038_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV038_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV038_SET_CONTEXT_DMA_OVERLAY                              (0x00000184)
#define NV038_SET_CONTEXT_DMA_IMAGE_IN                             (0x00000188)
#define NV038_SET_CONTEXT_DMA_IMAGE_OUT                            (0x0000018C)
#define NV038_IMAGE_OUT_POINT                                      (0x00000300)
#define NV038_IMAGE_OUT_POINT_X                                    15:0
#define NV038_IMAGE_OUT_POINT_Y                                    31:16
#define NV038_IMAGE_OUT_SIZE                                       (0x00000304)
#define NV038_IMAGE_OUT_SIZE_WIDTH                                 15:0
#define NV038_IMAGE_OUT_SIZE_HEIGHT                                31:16
#define NV038_IMAGE_OUT_FORMAT                                     (0x00000308)
#define NV038_IMAGE_OUT_FORMAT_PITCH                               15:0
#define NV038_IMAGE_OUT_FORMAT_COLOR                               31:16
#define NV038_IMAGE_OUT_FORMAT_COLOR_LE_V8YB8U8YA8                 (0x00000001)
#define NV038_IMAGE_OUT_FORMAT_COLOR_LE_YB8V8YA8U8                 (0x00000002)
#define NV038_IMAGE_OUT_OFFSET                                     (0x0000030C)
#define NV038_IMAGE_IN_DELTA_DU_DX                                 (0x00000310)
#define NV038_IMAGE_IN_DELTA_DV_DY                                 (0x00000314)
#define NV038_IMAGE_IN_SIZE                                        (0x00000318)
#define NV038_IMAGE_IN_SIZE_WIDTH                                  15:0
#define NV038_IMAGE_IN_SIZE_HEIGHT                                 31:16
#define NV038_IMAGE_IN_FORMAT                                      (0x0000031C)
#define NV038_IMAGE_IN_FORMAT_PITCH                                15:0
#define NV038_IMAGE_IN_FORMAT_COLOR                                31:16
#define NV038_IMAGE_IN_FORMAT_COLOR_LE_V8YB8U8YA8                  (0x00000001)
#define NV038_IMAGE_IN_FORMAT_COLOR_LE_YB8V8YA8U8                  (0x00000002)
#define NV038_IMAGE_IN_OFFSET                                      (0x00000320)
#define NV038_IMAGE_IN_POINT                                       (0x00000324)
#define NV038_IMAGE_IN_POINT_U                                     15:0
#define NV038_IMAGE_IN_POINT_V                                     31:16
#define NV038_OVERLAY_DELTA_DU_DX                                  (0x00000328)
#define NV038_OVERLAY_DELTA_DV_DY                                  (0x0000032C)
#define NV038_OVERLAY_SIZE                                         (0x00000330)
#define NV038_OVERLAY_SIZE_WIDTH                                   15:0
#define NV038_OVERLAY_SIZE_HEIGHT                                  31:16
#define NV038_OVERLAY_FORMAT                                       (0x00000334)
#define NV038_OVERLAY_FORMAT_PITCH                                 15:0
#define NV038_OVERLAY_FORMAT_COLOR                                 31:16
#define NV038_OVERLAY_FORMAT_COLOR_LE_A8V8U8Y8                     (0x00000001)
#define NV038_OVERLAY_FORMAT_COLOR_LE_A4V6YB6A4U6YA6               (0x00000002)
#define NV038_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT                  (0x00000003)
#define NV038_OVERLAY_OFFSET                                       (0x00000338)
#define NV038_OVERLAY_POINT                                        (0x0000033C)
#define NV038_OVERLAY_POINT_U                                      15:0
#define NV038_OVERLAY_POINT_V                                      31:16


/* class NV03_MEMORY_TO_MEMORY_FORMAT */
#define  NV03_MEMORY_TO_MEMORY_FORMAT                              (0x00000039)
#define  NV3_MEMORY_TO_MEMORY_FORMAT                               (0x00000039)
/* NvNotification[] elements */
#define NV039_NOTIFIERS_NOTIFY                                     (0)
#define NV039_NOTIFIERS_BUFFER_NOTIFY                              (1)
/* NvNotification[] fields and values */
#define NV039_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV039_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV039_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV039_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV039_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV039_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV039_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaBufferIn;   /* NV01_CONTEXT_DMA_FROM_MEMORY      0184-0187*/
 V032 SetContextDmaBufferOut;  /* NV01_CONTEXT_DMA_TO_MEMORY        0188-018b*/
 V032 Reserved01[0x060];
 U032 OffsetIn;                /* src offset in bytes               030c-030f*/
 U032 OffsetOut;               /* dst offset in bytes               0310-0313*/
 S032 PitchIn;                 /* delta in bytes, vert pixel delta  0314-0317*/
 S032 PitchOut;                /* delta in bytes, vert pixel delta  0318-031b*/
 U032 LineLengthIn;            /* in bytes                          031c-031f*/
 U032 LineCount;               /* in lines                          0320-0323*/
 V032 Format;                  /* out_in U24_U8                     0324-0327*/
 V032 BufferNotify;            /* NV039_BUFFER_NOTIFY_*             0328-032b*/
 V032 Reserved02[0x735];
} Nv039Typedef, Nv03MemoryToMemoryFormat;
#define Nv3MemoryToMemoryFormat                        Nv03MemoryToMemoryFormat
#define nv3MemoryToMemoryFormat                        nv03MemoryToMemoryFormat
#define NV039_TYPEDEF                                  nv03MemoryToMemoryFormat
/* dma method offsets, fields, and values */
#define NV039_SET_OBJECT                                           (0x00000000)
#define NV039_NO_OPERATION                                         (0x00000100)
#define NV039_NOTIFY                                               (0x00000104)
#define NV039_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV039_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV039_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV039_SET_CONTEXT_DMA_BUFFER_IN                            (0x00000184)
#define NV039_SET_CONTEXT_DMA_BUFFER_OUT                           (0x00000188)
#define NV039_OFFSET_IN                                            (0x0000030C)
#define NV039_OFFSET_OUT                                           (0x00000310)
#define NV039_PITCH_IN                                             (0x00000314)
#define NV039_PITCH_OUT                                            (0x00000318)
#define NV039_LINE_LENGTH_IN                                       (0x0000031C)
#define NV039_LINE_COUNT                                           (0x00000320)
#define NV039_FORMAT                                               (0x00000324)
#define NV039_FORMAT_IN                                            7:0
#define NV039_FORMAT_OUT                                           31:8
#define NV039_BUFFER_NOTIFY                                        (0x00000328)
#define NV039_BUFFER_NOTIFY_WRITE_ONLY                             (0x00000000)
#define NV039_BUFFER_NOTIFY_WRITE_THEN_AWAKEN                      (0x00000001)

typedef V032 Nv03aTypedef;

typedef V032 Nv03bTypedef;

typedef V032 Nv03cTypedef;



/* class NV01_CONTEXT_DMA_IN_MEMORY */
#define  NV01_CONTEXT_DMA_IN_MEMORY                                (0x0000003D)
#define  NV1_CONTEXT_DMA_IN_MEMORY                                 (0x0000003D)
/* NvNotification[] fields and values */
#define NV03D_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv03dTypedef, Nv01ContextDmaInMemory;
#define  Nv1ContextDmaInMemory                           Nv01ContextDmaInMemory
#define  nv1ContextDmaInMemory                           nv01ContextDmaInMemory
#define  NV03D_TYPEDEF                                   nv01ContextDmaInMemory


/* object NV01_CONTEXT_ERROR_TO_MEMORY */
#define   NV01_CONTEXT_ERROR_TO_MEMORY                             (0x0000003E)
#define   NV1_CONTEXT_ERROR_TO_MEMORY                              (0x0000003E)
/* NvNotification[] fields and values */
#define NV03E_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv03eTypedef, Nv01ContextErrorToMemory;
#define  Nv1ContextErrorToMemory                       Nv01ContextErrorToMemory
#define  nv1ContextErrorToMemory                       nv01ContextErrorToMemory
#define  NV03E_TYPEDEF                                 nv01ContextErrorToMemory


/* class NV01_MEMORY_PRIVILEGED */
#define  NV01_MEMORY_PRIVILEGED                                    (0x0000003F)
#define  NV1_MEMORY_PRIVILEGED                                     (0x0000003F)
/* NvNotification[] fields and values */
#define NV03F_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv03fTypedef, Nv01MemoryPrivileged;
#define  Nv1MemoryPrivileged                               Nv01MemoryPrivileged
#define  nv1MemoryPrivileged                               nv01MemoryPrivileged
#define  NV03F_TYPEDEF                                     nv01MemoryPrivileged


/* class NV01_MEMORY_USER */
#define  NV01_MEMORY_USER                                          (0x00000040)
#define  NV1_MEMORY_USER                                           (0x00000040)
/* NvNotification[] fields and values */
#define NV040_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv040Typedef, Nv01MemoryUser;
#define  Nv1MemoryUser                                           Nv01MemoryUser
#define  nv1MemoryUser                                           nv01MemoryUser
#define  NV040_TYPEDEF                                           nv01MemoryUser

typedef V032 Nv041Typedef;


/* class NV04_CONTEXT_SURFACES_2D */
#define  NV04_CONTEXT_SURFACES_2D                                  (0x00000042)
#define  NV4_CONTEXT_SURFACES_2D                                   (0x00000042)
/* NvNotification[] elements */
#define NV042_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV042_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV042_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV042_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV042_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV042_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV042_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV042_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaImageSource;/* NV01_CONTEXT_DMA_FROM_MEMORY      0184-0187*/
 V032 SetContextDmaImageDestin;/* NV01_CONTEXT_DMA_IN_MEMORY        0188-018b*/
 V032 Reserved01[0x05d];
 V032 SetColorFormat;          /* NV042_SET_COLOR_FORMAT_*          0300-0303*/
 U032 SetPitch;                /* destin_source U16_U16             0304-0307*/
 U032 SetOffsetSource;         /* byte offset of top-left pixel     0308-030b*/
 U032 SetOffsetDestin;         /* byte offset of top-left pixel     030c-030f*/
 V032 Reserved02[0x73c];
} Nv042Typedef, Nv04ContextSurfaces2d;
#define Nv4ContextSurfaces2D                              Nv04ContextSurfaces2D
#define nv4ContextSurfaces2D                              nv04ContextSurfaces2D
#define NV042_TYPEDEF                                     nv04ContextSurfaces2D
/* dma method offsets, fields, and values */
#define NV042_SET_OBJECT                                           (0x00000000)
#define NV042_NO_OPERATION                                         (0x00000100)
#define NV042_NOTIFY                                               (0x00000104)
#define NV042_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV042_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV042_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV042_SET_CONTEXT_DMA_IMAGE_SOURCE                         (0x00000184)
#define NV042_SET_CONTEXT_DMA_IMAGE_DESTIN                         (0x00000188)
#define NV042_SET_COLOR_FORMAT                                     (0x00000300)
#define NV042_SET_COLOR_FORMAT_LE_Y8                               (0x00000001)
#define NV042_SET_COLOR_FORMAT_LE_X1R5G5B5_Z1R5G5B5                (0x00000002)
#define NV042_SET_COLOR_FORMAT_LE_X1R5G5B5_O1R5G5B5                (0x00000003)
#define NV042_SET_COLOR_FORMAT_LE_R5G6B5                           (0x00000004)
#define NV042_SET_COLOR_FORMAT_LE_Y16                              (0x00000005)
#define NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8                (0x00000006)
#define NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_O8R8G8B8                (0x00000007)
#define NV042_SET_COLOR_FORMAT_LE_X1A7R8G8B8_Z1A7R8G8B8            (0x00000008)
#define NV042_SET_COLOR_FORMAT_LE_X1A7R8G8B8_O1A7R8G8B8            (0x00000009)
#define NV042_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x0000000A)
#define NV042_SET_COLOR_FORMAT_LE_Y32                              (0x0000000B)
#define NV042_SET_PITCH                                            (0x00000304)
#define NV042_SET_PITCH_SOURCE                                     15:0
#define NV042_SET_PITCH_DESTIN                                     31:16
#define NV042_SET_OFFSET_SOURCE                                    (0x00000308)
#define NV042_SET_OFFSET_DESTIN                                    (0x0000030C)


/* class NV03_CONTEXT_ROP */
#define  NV03_CONTEXT_ROP                                          (0x00000043)
#define  NV3_CONTEXT_ROP                                           (0x00000043)
/* NvNotification[] elements */
#define NV043_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV043_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV043_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV043_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV043_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV043_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV043_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV043_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 Reserved01[0x05f];
 V032 SetRop5;                 /* 32-bit ROP5                       0300-0303*/
 V032 Reserved02[0x73f];
} Nv043Typedef, Nv03ContextRop;
#define Nv3ContextRop                                            Nv03ContextRop
#define nv3ContextRop                                            nv03ContextRop
#define NV043_TYPEDEF                                            nv03ContextRop
/* dma method offsets, fields, and values */
#define NV043_SET_OBJECT                                           (0x00000000)
#define NV043_NO_OPERATION                                         (0x00000100)
#define NV043_NOTIFY                                               (0x00000104)
#define NV043_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV043_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV043_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV043_SET_ROP5                                             (0x00000300)


/* class NV04_CONTEXT_PATTERN */
#define  NV04_CONTEXT_PATTERN                                      (0x00000044)
#define  NV4_CONTEXT_PATTERN                                       (0x00000044)
/* NvNotification[] elements */
#define NV044_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV044_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV044_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV044_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV044_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV044_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV044_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV044_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 Reserved01[0x05f];
 V032 SetColorFormat;          /* NV044_SET_COLOR_FORMAT_*          0300-0303*/
 V032 SetMonochromeFormat;     /* NV044_SET_MONOCHROME_FORMAT_*     0304-0307*/
 V032 SetMonochromeShape;      /* NV044_SET_MONOCHROME_SHAPE_*      0308-030b*/
 V032 SetPatternSelect;        /* NV044_SET_PATTERN_SELECT_*        030c-030f*/
 V032 SetMonochromeColor0;     /* color of 0 pixels                 0310-0313*/
 V032 SetMonochromeColor1;     /* color of 1 pixels                 0314-0317*/
 V032 SetMonochromePattern0;   /* first  32 bits of pattern data    0318-031b*/
 V032 SetMonochromePattern1;   /* second 32 bits of pattern data    031c-031f*/
 V032 Reserved02[0x038];
 V032 SetPatternY8[16];        /* y3_y2_y1_y0 U8_U8_U8_U8           0400-043f*/
 V032 Reserved03[0x030];
 V032 SetPatternR5G6B5[32];    /* see text                          0500-057f*/
 V032 Reserved04[0x020];
 V032 SetPatternX1R5G5B5[32];  /* see text                          0600-067f*/
 V032 Reserved05[0x020];
 V032 SetPatternX8R8G8B8[64];  /* ignore_red_green_blue U8_U8_U8_U8 0700-07ff*/
 V032 Reserved06[0x600];
} Nv044Typedef, Nv04ImagePattern;
#define Nv4ImagePattern                                        Nv04ImagePattern
#define nv4ImagePattern                                        nv04ImagePattern
#define NV044_TYPEDEF                                          nv04ImagePattern
/* dma method offsets, fields, and values */
#define NV044_SET_OBJECT                                           (0x00000000)
#define NV044_NO_OPERATION                                         (0x00000100)
#define NV044_NOTIFY                                               (0x00000104)
#define NV044_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV044_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV044_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV044_SET_COLOR_FORMAT                                     (0x00000300)
#define NV044_SET_COLOR_FORMAT_LE_A16R5G6B5                        (0x00000001)
#define NV044_SET_COLOR_FORMAT_LE_X16A1R5G5B5                      (0x00000002)
#define NV044_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000003)
#define NV044_SET_MONOCHROME_FORMAT                                (0x00000304)
#define NV044_SET_MONOCHROME_FORMAT_CGA6_M1                        (0x00000001)
#define NV044_SET_MONOCHROME_FORMAT_LE_M1                          (0x00000002)
#define NV044_SET_MONOCHROME_SHAPE                                 (0x00000308)
#define NV044_SET_MONOCHROME_SHAPE_8X_8Y                           (0x00000000)
#define NV044_SET_MONOCHROME_SHAPE_64X_1Y                          (0x00000001)
#define NV044_SET_MONOCHROME_SHAPE_1X_64Y                          (0x00000002)
#define NV044_SET_PATTERN_SELECT                                   (0x0000030C)
#define NV044_SET_PATTERN_SELECT_MONOCHROME                        (0x00000001)
#define NV044_SET_PATTERN_SELECT_COLOR                             (0x00000002)
#define NV044_SET_MONOCHORME_COLOR0                                (0x00000310)
#define NV044_SET_MONOCHORME_COLOR1                                (0x00000314)
#define NV044_SET_MONOCHORME_PATTERN0                              (0x00000318)
#define NV044_SET_MONOCHORME_PATTERN1                              (0x0000031C)
#define NV044_SET_PATTERN_Y8(i)                                    (0x00000400\
                                                                   +(i)*0x0004)
#define NV044_SET_PATTERN_Y8_Y0                                    7:0
#define NV044_SET_PATTERN_Y8_Y1                                    15:8
#define NV044_SET_PATTERN_Y8_Y2                                    23:16
#define NV044_SET_PATTERN_Y8_Y3                                    31:24
#define NV044_SET_PATTERN_R5G6B5(i)                                (0x00000500\
                                                                   +(i)*0x0004)
#define NV044_SET_PATTERN_R5G6B5_BLUE0                             4:0
#define NV044_SET_PATTERN_R5G6B5_GREEN0                            10:5
#define NV044_SET_PATTERN_R5G6B5_RED0                              15:11
#define NV044_SET_PATTERN_R5G6B5_BLUE1                             20:16
#define NV044_SET_PATTERN_R5G6B5_GREEN1                            26:21
#define NV044_SET_PATTERN_R5G6B5_RED1                              31:27
#define NV044_SET_PATTERN_X1R5G5B5(i)                              (0x00000600\
                                                                   +(i)*0x0004)
#define NV044_SET_PATTERN_X1R5G5B5_BLUE0                           4:0
#define NV044_SET_PATTERN_X1R5G5B5_GREEN0                          9:5
#define NV044_SET_PATTERN_X1R5G5B5_RED0                            14:10
#define NV044_SET_PATTERN_X1R5G5B5_IGNORE0                         15:15
#define NV044_SET_PATTERN_X1R5G5B5_BLUE1                           20:16
#define NV044_SET_PATTERN_X1R5G5B5_GREEN1                          25:21
#define NV044_SET_PATTERN_X1R5G5B5_RED1                            30:26
#define NV044_SET_PATTERN_X1R5G5B5_IGNORE1                         31:31
#define NV044_SET_PATTERN_X8R8G8B8(i)                              (0x00000700\
                                                                   +(i)*0x0004)
#define NV044_SET_PATTERN_X8R8G8B8_BLUE                            7:0
#define NV044_SET_PATTERN_X8R8G8B8_GREEN                           15:8
#define NV044_SET_PATTERN_X8R8G8B8_RED                             23:16
#define NV044_SET_PATTERN_X8R8G8B8_IGNORE                          31:24

typedef V032 Nv045Typedef;


/* class NV03_VIDEO_LUT_CURSOR_DAC */
#define  NV03_VIDEO_LUT_CURSOR_DAC                                 (0x00000046)
/* NvNotification[] elements */
#define NV046_NOTIFIERS_NOTIFY                                     (0)
#define NV046_NOTIFIERS_SET_IMAGE(b)                               (1+(b))
#define NV046_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(b)                 (3+(b))
#define NV046_NOTIFIERS_SET_LUT(b)                                 (5+(b))
#define NV046_NOTIFIERS_SET_CURSOR_IMAGE(b)                        (7+(b))
#define NV046_NOTIFIERS_SET_CURSOR_POINT(b)                        (9+(b))
/* NvNotification[] fields and values */
#define NV046_NOTIFICATION_INFO16_OFFSET_VALID                     (0x0001)
#define NV046_NOTIFICATION_INFO16_OFFSET_INVALID                   (0x0002)
#define NV046_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV046_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV046_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV046_NOTIFICATION_STATUS_WARNING_INVALID_DATA             (0x0001)
#define NV046_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV046_NOTIFY_*                    0104-0107*/
 V032 StopTransfer;            /* NV046_STOP_TRANSFER_VALUE         0108-010b*/
 V032 Reserved00[0x01D];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaImage[2];   /* NV01_CONTEXT_DMA_FROM_MEMORY      0184-018b*/
 V032 SetContextDmaLut;        /* NV01_CONTEXT_DMA_FROM_MEMORY      018c-018f*/
 V032 SetContextDmaCursor;     /* NV01_CONTEXT_DMA_FROM_MEMORY      0190-0193*/
 V032 Reserved01[0x05B];
 struct {                      /* start of methods in array         0300-    */
  U032 offset;                 /* byte offset of top-left pixel        0-   3*/
  V032 format;                 /* pitch_color_field_notify             4-   7*/
 } SetImage[2];                /* end of methods in array               -030f*/
 V032 GetImageOffsetNotify[2]; /* NV046_GET_IMAGE_OFFSET_NOTIFY_*   0310-0317*/
 V032 SetLut[2];               /* notify_offset V02_U30             0318-031f*/
 V032 SetCursorImage[2];       /* notify_offset V02_U30             0320-0327*/
 V032 SetCursorPoint[2];       /* notify_y_x V02_S14_S16            0328-032f*/
 V032 SetDacImageSize;         /* height_width U16_U16 in pixels    0330-0333*/
 V032 SetDacHorizontalSync;    /* width_start U16_U16 in pixels     0334-0337*/
 V032 SetDacVerticalSync;      /* width_start U16_U16 in pixels     0338-033b*/
 V032 SetDacTotalSize;         /* height_width U16_U16 in pixels    033c-033f*/
 U032 SetDacPixelClock;        /* in Hertz                          0340-0343*/
 V032 Reserved02[0x72F];
} Nv046Typedef, nv03VideoLutCursorDac;
#define NV046_TYPEDEF                                     nv03VideoLutCursorDac
/* dma method offsets, fields, and values */
#define NV046_SET_OBJECT                                           (0x00000000)
#define NV046_NO_OPERATION                                         (0x00000100)
#define NV046_NOTIFY                                               (0x00000104)
#define NV046_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV046_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV046_STOP_TRANSFER                                        (0x00000108)
#define NV046_STOP_TRANSFER_VALUE                                  (0x00000000)
#define NV046_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV046_SET_CONTEXT_DMA_IMAGE(b)                             (0x00000184\
                                                                   +(b)*0x0004)
#define NV046_SET_CONTEXT_DMA_LUT                                  (0x0000018C)
#define NV046_SET_CONTEXT_DMA_CURSOR                               (0x00000190)
#define NV046_SET_IMAGE(b)                                         (0x00000300\
                                                                   +(b)*0x0008)
#define NV046_SET_IMAGE_OFFSET(b)                                  (0x00000300\
                                                                   +(b)*0x0008)
#define NV046_SET_IMAGE_FORMAT(b)                                  (0x00000304\
                                                                   +(b)*0x0008)
#define NV046_SET_IMAGE_FORMAT_PITCH                               15:0
#define NV046_SET_IMAGE_FORMAT_COLOR                               23:16
#define NV046_SET_IMAGE_FORMAT_COLOR_LUT_LE_Y8                     (0x00000000)
#define NV046_SET_IMAGE_FORMAT_COLOR_BYPASS_LE_R5G6B5              (0x00000001)
#define NV046_SET_IMAGE_FORMAT_COLOR_LE_BYPASS1R5G5B5              (0x00000002)
#define NV046_SET_IMAGE_FORMAT_COLOR_LE_BYPASS1X7R8G8B8            (0x00000003)
#define NV046_SET_IMAGE_FORMAT_FIELD                               29:24
#define NV046_SET_IMAGE_FORMAT_FIELD_PROGRESSIVE                   (0x00000000)
#define NV046_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD                    (0x00000001)
#define NV046_SET_IMAGE_FORMAT_FIELD_ODD_FIELD                     (0x00000002)
#define NV046_SET_IMAGE_FORMAT_NOTIFY                              31:30
#define NV046_SET_IMAGE_FORMAT_NOTIFY_WRITE_ONLY                   (0x00000000)
#define NV046_SET_IMAGE_FORMAT_NOTIFY_WRITE_THEN_AWAKEN            (0x00000001)
#define NV046_GET_IMAGE_OFFSET_NOTIFY(b)                           (0x00000310\
                                                                   +(b)*0x0004)
#define NV046_GET_IMAGE_OFFSET_NOTIFY_WRITE_ONLY                   (0x00000000)
#define NV046_GET_IMAGE_OFFSET_NOTIFY_WRITE_THEN_AWAKEN            (0x00000001)
#define NV046_SET_LUT(b)                                           (0x00000318\
                                                                   +(b)*0x0004)
#define NV046_SET_LUT_OFFSET                                       29:0
#define NV046_SET_LUT_NOTIFY                                       31:30
#define NV046_SET_LUT_NOTIFY_WRITE_ONLY                            (0x00000000)
#define NV046_SET_LUT_NOTIFY_WRITE_THEN_AWAKEN                     (0x00000001)
#define NV046_SET_CURSOR_IMAGE(b)                                  (0x00000320\
                                                                   +(b)*0x0004)
#define NV046_SET_CURSOR_IMAGE_OFFSET                              29:0
#define NV046_SET_CURSOR_IMAGE_NOTIFY                              31:30
#define NV046_SET_CURSOR_IMAGE_NOTIFY_WRITE_ONLY                   (0x00000000)
#define NV046_SET_CURSOR_IMAGE_NOTIFY_WRITE_THEN_AWAKEN            (0x00000001)
#define NV046_SET_CURSOR_POINT(b)                                  (0x00000328\
                                                                   +(b)*0x0004)
#define NV046_SET_CURSOR_POINT_X                                   15:0
#define NV046_SET_CURSOR_POINT_Y                                   29:16
#define NV046_SET_CURSOR_POINT_NOTIFY                              31:30
#define NV046_SET_CURSOR_POINT_NOTIFY_WRITE_ONLY                   (0x00000000)
#define NV046_SET_CURSOR_POINT_NOTIFY_WRITE_THEN_AWAKEN            (0x00000001)
#define NV046_SET_DAC_IMAGE_SIZE                                   (0x00000330)
#define NV046_SET_DAC_IMAGE_SIZE_WIDTH                             15:0
#define NV046_SET_DAC_IMAGE_SIZE_HEIGHT                            31:16
#define NV046_SET_DAC_HORIZONTAL_SYNC                              (0x00000334)
#define NV046_SET_DAC_HORIZONTAL_SYNC_START                        15:0
#define NV046_SET_DAC_HORIZONTAL_SYNC_WIDTH                        31:16
#define NV046_SET_DAC_VERTICAL_SYNC                                (0x00000338)
#define NV046_SET_DAC_VERTICAL_SYNC_START                          15:0
#define NV046_SET_DAC_VERTICAL_SYNC_WIDTH                          31:16
#define NV046_SET_DAC_TOTAL_SIZE                                   (0x0000033C)
#define NV046_SET_DAC_TOTAL_SIZE_WIDTH                             15:0
#define NV046_SET_DAC_TOTAL_SIZE_HEIGHT                            31:16
#define NV046_SET_DAC_PIXEL_CLOCK                                  (0x00000340)

typedef V032 Nv047Typedef;


/* class NV03_DX3_TEXTURED_TRIANGLE */
#define  NV03_DX3_TEXTURED_TRIANGLE                                (0x00000048)
#define  NV3_DX3_TEXTURED_TRIANGLE                                 (0x00000048)
/* NvNotification[] elements */
#define NV048_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV048_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV048_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV048_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV048_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV048_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV048_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV048_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaTexture;    /* NV01_CONTEXT_DMA_FROM_MEMORY      0184-0187*/
 V032 SetContextClipRectangle; /* NV01_IMAGE_BLACK_RECTANGLE        0188-018b*/
 V032 SetContextSurfaceColor;  /* NV03_CONTEXT_SURFACE_2            018c-018f*/
 V032 SetContextSurfaceZeta;   /* NV03_CONTEXT_SURFACE_3            0190-0193*/
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
#define Nv3Dx3TexturedTriangle                          Nv03Dx3TexturedTriangle
#define nv3Dx3TexturedTriangle                          nv03Dx3TexturedTriangle
#define NV048_TYPEDEF                                   nv03Dx3TexturedTriangle
/* dma method offsets, fields, and values */
#define NV048_SET_OBJECT                                           (0x00000000)
#define NV048_NO_OPERATION                                         (0x00000100)
#define NV048_NOTIFY                                               (0x00000104)
#define NV048_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV048_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
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

typedef V032 Nv049Typedef;


/* class NV04_GDI_RECTANGLE_TEXT */
#define  NV04_GDI_RECTANGLE_TEXT                                   (0x0000004A)
#define  NV4_GDI_RECTANGLE_TEXT                                    (0x0000004A)
/* NvNotification[] elements */
#define NV04A_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV04A_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV04A_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV04A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV04A_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV04A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV04A_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* memory data structures */
typedef struct {               /* start of data structure           0000-    */
   V032 size;                  /* height_width U16_U16 in pixels       0-   3*/
   V032 monochrome[1];         /* 32 monochrome pixels per write       4-   7*/
} Nv04aCharacter8[];           /* end of data structure                 -0007*/
typedef struct {               /* start of data structure           0000-    */
  V032 size;                   /* height_width U16_U16 in pixels       0-   3*/
  V032 monochrome[3];          /* 32 monochrome pixels per write       4-   f*/
} Nv04aCharacter16[];          /* end of data structure                 -000f*/
typedef struct {               /* start of data structure           0000-    */
  V032 size;                   /* height_width U16_U16 in pixels      00-  03*/
  V032 monochrome[7];          /* 32 monochrome pixels per write      04-  1f*/
} Nv04aCharacter32[];          /* end of data structure                 -001f*/
typedef struct {               /* start of data structure           0000-    */
  V032 size;                   /* height_width U16_U16 in pixels      00-  03*/
  V032 monochrome[15];         /* 32 monochrome pixels per write      04-  3f*/
} Nv04aCharacter64[];          /* end of data structure                 -003f*/
typedef struct {               /* start of data structure           0000-    */
  V032 size;                   /* height_width U16_U16 in pixels      00-  03*/
  V032 monochrome[31];         /* 32 monochrome pixels per write      04-  7f*/
} Nv04aCharacter128[];         /* end of data structure                 -007f*/
typedef struct {               /* start of data structure           0000-    */
  V032 size;                   /* height_width U16_U16 in pixels      00-  03*/
  V032 monochrome[63];         /* 32 monochrome pixels per write      04-  ff*/
} Nv04aCharacter256[];         /* end of data structure                 -00ff*/
typedef struct {               /* start of data structure           0000-    */
  V032 size;                   /* height_width U16_U16 in pixels     000- 003*/
  V032 monochrome[127];        /* 32 monochrome pixels per write     004- 1ff*/
} Nv04aCharacter512[];         /* end of data structure                 -01ff*/
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV04A_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaFonts;      /* NV01_CONTEXT_DMA_FROM_MEMORY      0184-0187*/
 V032 SetContextPattern;       /* NV04_CONTEXT_PATTERN              0188-018b*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  018c-018f*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0190-0193*/
 V032 SetContextBeta4;         /* NV04_CONTEXT_BETA                 0194-0197*/
 V032 SetContextSurface;       /* NV04_CONTEXT_SURFACES_2D          0198-019b*/
 V032 Reserved01[0x058];
 V032 SetOperation;            /* NV04A_SET_OPERATION_*             02fc-02ff*/
 V032 SetColorFormat;          /* NV04A_SET_COLOR_FORMAT_*          0300-0303*/
 V032 SetMonochromeFormat;     /* NV04A_SET_MONOCHROME_FORMAT_*     0304-0307*/
 V032 Reserved02[0x03D];
 V032 Color1A;                 /* rectangle color                   03fc-03ff*/
 struct {                      /* start of aliased methods in array 0400-    */
  V032 point;                  /* x_y S16_S16 in pixels                0-   3*/
  V032 size;                   /* width_height U16_U16 in pixels       4-   7*/
 } UnclippedRectangle[32];     /* end of aliased methods in array       -04ff*/
 V032 Reserved03[0x03D];
 V032 ClipPoint0B;             /* top_left S16_S16 in pixels        05f4-05f7*/
 V032 ClipPoint1B;             /* bottom_right S16_S16 in pixels    05f8-05fb*/
 V032 Color1B;                 /* rectangle color                   05fc-05ff*/
 struct {                      /* start of aliased methods in array 0600-    */
  V032 point0;                 /* top_left S16_S16 in pixels           0-   3*/
  V032 point1;                 /* bottom_right S16_S16 in pixels       4-   7*/
 } ClippedRectangle[32];       /* end of aliased methods in array       -06ff*/
 V032 Reserved04[0x03B];
 V032 ClipPoint0C;             /* top_left S16_S16 in pixels        07ec-07ef*/
 V032 ClipPoint1C;             /* bottom_right S16_S16 in pixe      07f0-07f3*/
 V032 Color1C;                 /* color of 1 pixels                 07f4-07f7*/
 V032 SizeC;                   /* height_width U16_U16 in pixels    07f8-07fb*/
 V032 PointC;                  /* y_x S16_S16 in pixels             07fc-07ff*/
 V032 MonochromeColor1C[128];  /* 32 monochrome pixels per write    0800-09ff*/
 V032 Reserved05[0x079];
 V032 ClipPoint0E;             /* top_left S16_S16 in pixels        0be4-0be7*/
 V032 ClipPoint1E;             /* bottom_right S16_S16 in pixels    0be8-0beb*/
 V032 Color0E;                 /* color of 0 pixels                 0bec-0bef*/
 V032 Color1E;                 /* color of 1 pixels                 0bf0-0bf3*/
 V032 SizeInE;                 /* height_width U16_U16 in pixels    0bf4-0bf7*/
 V032 SizeOutE;                /* height_width U16_U16 in pixels    0bf8-0bfb*/
 V032 PointE;                  /* y_x S16_S16 in pixels             0bfc-0bff*/
 V032 MonochromeColor01E[128]; /* 32 monochrome pixels per write    0c00-0dff*/
 V032 Reserved06[0x07C];
 V032 FontF;                   /* pitch_offset V4_U28               0ff0-0ff3*/
 V032 ClipPoint0F;             /* top_left S16_S16 in pixels        0ff4-0ff7*/
 V032 ClipPoint1F;             /* bottom_right S16_S16 in pixels    0ff8-0ffb*/
 V032 Color1F;                 /* color of 1 pixels                 0ffc-0fff*/
 V032 CharacterColor1F[256];   /* y_x_index S12_S12_U8              1000-13ff*/
 V032 Reserved07[0x0FC];
 V032 FontG;                   /* pitch_offset V4_U28               17f0-17f3*/
 V032 ClipPoint0G;             /* top_left S16_S16 in pixels        17f4-17f7*/
 V032 ClipPoint1G;             /* bottom_right S16_S16 in pixels    17f8-17fb*/
 V032 Color1G;                 /* color of 1 pixels                 17fc-17ff*/
 struct {                      /* start of aliased methods in array 1800-    */
  V032 point;                  /* y_x S16_S16 in pixels                0-   3*/
  U032 index;                  /* 0<=index<=65525                      4-   7*/
 } CharacterColor1G[256];      /* end of aliased methods in array       -1fff*/
} Nv04aTypedef, Nv04GdiRectangleText;
#define Nv4GdiRectangleText                                Nv04GdiRectangleText
#define nv4GdiRectangleText                                nv04GdiRectangleText
#define NV04A_TYPEDEF                                      nv04GdiRectangleText
/* dma method offsets, fields, and values */
#define NV04A_SET_OBJECT                                           (0x00000000)
#define NV04A_NO_OPERATION                                         (0x00000100)
#define NV04A_NOTIFY                                               (0x00000104)
#define NV04A_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV04A_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV04A_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV04A_SET_CONTEXT_DMA_FONTS                                (0x00000184)
#define NV04A_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV04A_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV04A_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV04A_SET_CONTEXT_BETA4                                    (0x00000194)
#define NV04A_SET_CONTEXT_SURFACE                                  (0x00000198)
#define NV04A_SET_OPERATION                                        (0x000002FC)
#define NV04A_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV04A_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV04A_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV04A_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV04A_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV04A_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV04A_SET_COLOR_FORMAT                                     (0x00000300)
#define NV04A_SET_COLOR_FORMAT_LE_X16R5G6B5                        (0x00000001)
#define NV04A_SET_COLOR_FORMAT_LE_X17R5G5B5                        (0x00000002)
#define NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000003)
#define NV04A_SET_MONOCHROME_FORMAT                                (0x00000304)
#define NV04A_SET_MONOCHROME_FORMAT_CGA6_M1                        (0x00000001)
#define NV04A_SET_MONOCHROME_FORMAT_LE_M1                          (0x00000002)
#define NV04A_COLOR1_A                                             (0x000003FC)
#define NV04A_UNCLIPPED_RECTANGLE(a)                               (0x00000400\
                                                                   +(a)*0x0008)
#define NV04A_UNCLIPPED_RECTANGLE_POINT(a)                         (0x00000400\
                                                                   +(a)*0x0008)
#define NV04A_UNCLIPPED_RECTANGLE_POINT_X                          15:0
#define NV04A_UNCLIPPED_RECTANGLE_POINT_Y                          31:16
#define NV04A_UNCLIPPED_RECTANGLE_SIZE(a)                          (0x00000404\
                                                                   +(a)*0x0008)
#define NV04A_UNCLIPPED_RECTANGLE_SIZE_WIDTH                       15:0
#define NV04A_UNCLIPPED_RECTANGLE_SIZE_HEIGHT                      31:16
#define NV04A_CLIP_POINT0_B                                        (0x000005F4)
#define NV04A_CLIP_POINT0_B_LEFT                                   15:0
#define NV04A_CLIP_POINT0_B_TOP                                    31:16
#define NV04A_CLIP_POINT1_B                                        (0x000005F8)
#define NV04A_CLIP_POINT1_B_RIGHT                                  15:0
#define NV04A_CLIP_POINT1_B_BOTTOM                                 31:16
#define NV04A_COLOR1_B                                             (0x000005FC)
#define NV04A_CLIPPED_RECTANGLE(a)                                 (0x00000600\
                                                                   +(a)*0x0008)
#define NV04A_CLIPPED_RECTANGLE_POINT_0                            (0x00000600\
                                                                   +(a)*0x0008)
#define NV04A_CLIPPED_RECTANGLE_POINT_0_LEFT                       15:0
#define NV04A_CLIPPED_RECTANGLE_POINT_0_TOP                        31:16
#define NV04A_CLIPPED_RECTANGLE_POINT_1                            (0x00000604\
                                                                   +(a)*0x0008)
#define NV04A_CLIPPED_RECTANGLE_POINT_1_RIGHT                      15:0
#define NV04A_CLIPPED_RECTANGLE_POINT_1_BOTTOM                     31:16
#define NV04A_CLIP_POINT0_C                                        (0x000007EC)
#define NV04A_CLIP_POINT0_C_LEFT                                   15:0
#define NV04A_CLIP_POINT0_C_TOP                                    31:16
#define NV04A_CLIP_POINT1_C                                        (0x000007F0)
#define NV04A_CLIP_POINT1_C_RIGHT                                  15:0
#define NV04A_CLIP_POINT1_C_BOTTOM                                 31:16
#define NV04A_COLOR1_C                                             (0x000007F4)
#define NV04A_SIZE_C                                               (0x000007F8)
#define NV04A_SIZE_C_WIDTH                                         15:0
#define NV04A_SIZE_C_HEIGHT                                        31:16
#define NV04A_POINT_C                                              (0x000007FC)
#define NV04A_POINT_C_X                                            15:0
#define NV04A_POINT_C_Y                                            31:16
#define NV04A_MONOCHROME_COLOR1_C(a)                               (0x00000800\
                                                                   +(a)*0x0004)
#define NV04A_CLIP_POINT0_E                                        (0x00000BE4)
#define NV04A_CLIP_POINT0_E_LEFT                                   15:0
#define NV04A_CLIP_POINT0_E_TOP                                    31:16
#define NV04A_CLIP_POINT1_E                                        (0x00000BE8)
#define NV04A_CLIP_POINT1_E_RIGHT                                  15:0
#define NV04A_CLIP_POINT1_E_BOTTOM                                 31:16
#define NV04A_COLOR0_E                                             (0x00000BEC)
#define NV04A_COLOR1_E                                             (0x00000BF0)
#define NV04A_SIZE_IN_E                                            (0x00000BF4)
#define NV04A_SIZE_IN_E_WIDTH                                      15:0
#define NV04A_SIZE_IN_E_HEIGHT                                     31:16
#define NV04A_SIZE_OUT_E                                           (0x00000BF8)
#define NV04A_SIZE_OUT_E_WIDTH                                     15:0
#define NV04A_SIZE_OUT_E_HEIGHT                                    31:16
#define NV04A_POINT_E                                              (0x00000BFC)
#define NV04A_POINT_E_X                                            15:0
#define NV04A_POINT_E_Y                                            31:16
#define NV04A_MONOCHROME_COLOR01_E(a)                              (0x00000C00\
                                                                   +(a)*0x0004
#define NV04A_FONT_F                                               (0x00000FF0)
#define NV04A_FONT_F_OFFSET                                        27:0
#define NV04A_FONT_F_PITCH                                         31:28
#define NV04A_FONT_F_PITCH_8                                       (0x00000003)
#define NV04A_FONT_F_PITCH_16                                      (0x00000004)
#define NV04A_FONT_F_PITCH_32                                      (0x00000005)
#define NV04A_FONT_F_PITCH_64                                      (0x00000006)
#define NV04A_FONT_F_PITCH_128                                     (0x00000007)
#define NV04A_FONT_F_PITCH_256                                     (0x00000008)
#define NV04A_FONT_F_PITCH_512                                     (0x00000009)
#define NV04A_CLIP_POINT0_F                                        (0x00000FF4)
#define NV04A_CLIP_POINT0_F_LEFT                                   15:0
#define NV04A_CLIP_POINT0_F_TOP                                    31:16
#define NV04A_CLIP_POINT1_F                                        (0x00000FF8)
#define NV04A_CLIP_POINT1_F_RIGHT                                  15:0
#define NV04A_CLIP_POINT1_F_BOTTOM                                 31:16
#define NV04A_COLOR1_F                                             (0x00000FFC)
#define NV04A_CHARACTER_COLOR1_F(a)                                (0x00001000\
                                                                   +(a)*0x0004)
#define NV04A_CHARACTER_COLOR1_F_INDEX                             7:0
#define NV04A_CHARACTER_COLOR1_F_X                                 19:8
#define NV04A_CHARACTER_COLOR1_F_Y                                 31:20
#define NV04A_FONT_G                                               (0x000017F0)
#define NV04A_FONT_G_OFFSET                                        27:0
#define NV04A_FONT_G_PITCH                                         31:28
#define NV04A_FONT_G_PITCH_8                                       (0x00000003)
#define NV04A_FONT_G_PITCH_16                                      (0x00000004)
#define NV04A_FONT_G_PITCH_32                                      (0x00000005)
#define NV04A_FONT_G_PITCH_64                                      (0x00000006)
#define NV04A_FONT_G_PITCH_128                                     (0x00000007)
#define NV04A_FONT_G_PITCH_256                                     (0x00000008)
#define NV04A_FONT_G_PITCH_512                                     (0x00000009)
#define NV04A_CLIP_POINT0_G                                        (0x000017F4)
#define NV04A_CLIP_POINT0_G_LEFT                                   15:0
#define NV04A_CLIP_POINT0_G_TOP                                    31:16
#define NV04A_CLIP_POINT1_G                                        (0x000017F8)
#define NV04A_CLIP_POINT1_G_RIGHT                                  15:0
#define NV04A_CLIP_POINT1_G_BOTTOM                                 31:16
#define NV04A_COLOR1_G                                             (0x000017FC)
#define NV04A_CHARACTER_COLOR1_G(a)                                (0x00001800\
                                                                   +(a)*0x0008)
#define NV04A_CHARACTER_COLOR1_G_POINT(a)                          (0x00001800\
                                                                   +(a)*0x0008)
#define NV04A_CHARACTER_COLOR1_G_POINT_X                           15:0
#define NV04A_CHARACTER_COLOR1_G_POINT_Y                           31:16
#define NV04A_CHARACTER_COLOR1_G_INDEX(a)                          (0x00001804\
                                                                   +(a)*0x0008)


/* class NV03_GDI_RECTANGLE_TEXT */
#define  NV03_GDI_RECTANGLE_TEXT                                   (0x0000004B)
#define  NV3_GDI_RECTANGLE_TEXT                                    (0x0000004B)
/* NvNotification[] elements */
#define NV04B_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV04B_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV04B_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV04B_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV04B_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV04B_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV04B_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV04B_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextPattern;       /* NV01_IMAGE_PATTERN                0184-0187*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  0188-018b*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   018c-018f*/
 V032 SetContextSurface;       /* NV03_CONTEXT_SURFACE_0            0190-0193*/
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
#define Nv3GdiRectangleText                                Nv03GdiRectangleText
#define nv3GdiRectangleText                                nv03GdiRectangleText
#define NV04B_TYPEDEF                                      nv03GdiRectangleText
/* dma method offsets, fields, and values */
#define NV04B_SET_OBJECT                                           (0x00000000)
#define NV04B_NO_OPERATION                                         (0x00000100)
#define NV04B_NOTIFY                                               (0x00000104)
#define NV04B_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV04B_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
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

typedef V032 Nv04cTypedef;


/* class NV03_EXTERNAL_VIDEO_DECODER */
#define  NV03_EXTERNAL_VIDEO_DECODER                               (0x0000004D)
/* NvNotification[] elements */
#define NV04D_NOTIFIERS_NOTIFY                                     (0)
#define NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(b)                   (1+(b))
#define NV04D_NOTIFIERS_GET_VBI_OFFSET_NOTIFY(b)                   (3+(b))
#define NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(b)                 (5+(b))
#define NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(b)                 (7+(b))
/* NvNotification[] fields and values */
#define NV04D_NOTIFICATION_INFO16_FIELD_NOT_STARTED                (0x0000)
#define NV04D_NOTIFICATION_INFO16_FIELD_VALID_OFFSET               (0x0001)
#define NV04D_NOTIFICATION_INFO16_FIELD_DONE                       (0x0002)
#define NV04D_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV04D_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV04D_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV04D_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV04D_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV04D_NOTIFICATION_STATUS_WARNING_INVALID_DATA             (0x0001)
#define NV04D_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 NvV32 NoOperation;             /* ignored                          0100-0103*/
 NvV32 Notify;                  /* NV04D_NOTIFY_*                   0104-0107*/
 NvV32 StopTransferVbi;         /* NV04D_STOP_TRANSFER_VBI_VALUE    0108-010b*/
 NvV32 StopTransferImage;       /* NV04D_STOP_TRANSFER_IMAGE_VALUE  010c-010f*/
 NvV32 Reserved00[0x01c];
 NvV32 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA                 0180-0183*/
 NvV32 SetContextDmaVbi[2];     /* NV01_CONTEXT_DMA                 0184-018b*/
 NvV32 SetContextDmaImage[2];   /* NV01_CONTEXT_DMA                 018c-0193*/
 NvV32 Reserved01[0x05a];
 NvU32 SetImageStartLine;       /* lines                            02fc-02ff*/
 struct {                       /* start of methods in array        0300-    */
  NvV32 size;                   /* height_firstLine U16_U16            0-   3*/
  NvU32 offset;                 /* byte offset of top-left pixel       4-   7*/
  NvV32 format;                 /* notify_field_pitch V8_V8_U16        8-   b*/
 } SetVbi[2];                   /* end of methods in array              -0317*/
 NvV32 GetVbiOffsetNotify[2];   /* NV04D_GET_VBI_OFFSET_NOTIFY_*    0318-031f*/
 struct {                       /* start of methods in array        0320-    */
  NvV32 sizeIn;                 /* height_width U16_U16 in pixels      0-   3*/
  NvV32 sizeOut;                /* height_width U16_U16 in pixels      4-   7*/
  NvU32 offset;                 /* byte offset of top-left pixel       8-   b*/
  NvV32 format;                 /* notify_field_pitch V8_V8_U16        c-   f*/
 } SetImage[2];                 /* end of methods in array              -033f*/
 NvV32 GetImageOffsetNotify[2]; /* NV04D_GET_IMAGE_OFFSET_NOTIFY_*  0340-0347*/
 NvV32 Reserved02[0x72e];
} Nv04dTypedef, Nv03ExternalVideoDecoder;
#define NV04D_TYPEDEF                                  nv03ExternalVideoDecoder
/* dma method offsets, fields, and values */
#define NV04D_SET_OBJECT                                           (0x00000000)
#define NV04D_NO_OPERATION                                         (0x00000100)
#define NV04D_NOTIFY                                               (0x00000104)
#define NV04D_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV04D_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV04D_STOP_TRANSFER_VBI                                    (0x00000108)
#define NV04D_STOP_TRANSFER_VBI_VALUE                              (0x00000000)
#define NV04D_STOP_TRANSFER_IMAGE                                  (0x0000010C)
#define NV04D_STOP_TRANSFER_IMAGE_VALUE                            (0x00000000)
#define NV04D_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV04D_SET_CONTEXT_DMA_VBI(b)                               (0x00000184\
                                                                   +(b)*0x0004)
#define NV04D_SET_CONTEXT_DMA_IMAGE(b)                             (0x0000018C\
                                                                   +(b)*0x0004)
#define NV04D_SET_IMAGE_START_LINE                                 (0x000002FC)
#define NV04D_SET_VBI(b)                                           (0x00000300\
                                                                   +(b)*0x000C)
#define NV04D_SET_VBI_SIZE(b)                                      (0x00000300\
                                                                   +(b)*0x000C)
#define NV04D_SET_VBI_SIZE_FIRST_LINE                              15:0
#define NV04D_SET_VBI_SIZE_HEIGHT                                  31:16
#define NV04D_SET_VBI_OFFSET(b)                                    (0x00000304\
                                                                   +(b)*0x000C)
#define NV04D_SET_VBI_FORMAT(b)                                    (0x00000308\
                                                                   +(b)*0x000C)
#define NV04D_SET_VBI_FORMAT_PITCH                                 15:0
#define NV04D_SET_VBI_FORMAT_FIELD                                 23:16
#define NV04D_SET_VBI_FORMAT_FIELD_PROGRESSIVE                     (0x00000000)
#define NV04D_SET_VBI_FORMAT_FIELD_EVEN_FIELD                      (0x00000001)
#define NV04D_SET_VBI_FORMAT_FIELD_ODD_FIELD                       (0x00000002)
#define NV04D_SET_VBI_FORMAT_NOTIFY                                31:24
#define NV04D_SET_VBI_FORMAT_NOTIFY_WRITE_ONLY                     (0x00000000)
#define NV04D_SET_VBI_FORMAT_NOTIFY_WRITE_THEN_AWAKEN              (0x00000001)
#define NV04D_GET_VBI_OFFSET_NOTIFY(b)                             (0x00000318\
                                                                   +(b)*0x0004)
#define NV04D_GET_VBI_OFFSET_NOTIFY_WRITE_ONLY                     (0x00000000)
#define NV04D_GET_VBI_OFFSET_NOTIFY_WRITE_THEN_AWAKEN              (0x00000001)
#define NV04D_SET_IMAGE(b)                                         (0x00000320\
                                                                   +(b)*0x0010)
#define NV04D_SET_IMAGE_SIZE_IN(b)                                 (0x00000320\
                                                                   +(b)*0x0010)
#define NV04D_SET_IMAGE_SIZE_IN_WIDTH                              15:0
#define NV04D_SET_IMAGE_SIZE_IN_HEIGHT                             31:16
#define NV04D_SET_IMAGE_SIZE_OUT(b)                                (0x00000324\
                                                                   +(b)*0x0010)
#define NV04D_SET_IMAGE_SIZE_OUT_WIDTH                             15:0
#define NV04D_SET_IMAGE_SIZE_OUT_HEIGHT                            31:16
#define NV04D_SET_IMAGE_OFFSET(b)                                  (0x00000328\
                                                                   +(b)*0x0010)
#define NV04D_SET_IMAGE_FORMAT(b)                                  (0x0000032C\
                                                                   +(b)*0x0010)
#define NV04D_SET_IMAGE_FORMAT_PITCH                               15:0
#define NV04D_SET_IMAGE_FORMAT_FIELD                               23:16
#define NV04D_SET_IMAGE_FORMAT_FIELD_PROGRESSIVE                   (0x00000000)
#define NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD                    (0x00000001)
#define NV04D_SET_IMAGE_FORMAT_FIELD_ODD_FIELD                     (0x00000002)
#define NV04D_SET_IMAGE_FORMAT_NOTIFY                              31:24
#define NV04D_SET_IMAGE_FORMAT_NOTIFY_WRITE_ONLY                   (0x00000000)
#define NV04D_SET_IMAGE_FORMAT_NOTIFY_WRITE_THEN_AWAKEN            (0x00000001)
#define NV04D_GET_IMAGE_OFFSET_NOTIFY(b)                           (0x00000340\
                                                                   +(b)*0x0004)
#define NV04D_GET_IMAGE_OFFSET_NOTIFY_WRITE_ONLY                   (0x00000000)
#define NV04D_GET_IMAGE_OFFSET_NOTIFY_WRITE_THEN_AWAKEN            (0x00000001)



/* class NV03_EXTERNAL_VIDEO_DECOMPRESSOR */
#define  NV03_EXTERNAL_VIDEO_DECOMPRESSOR                          (0x0000004E)
/* NvNotification[] elements */
#define NV04E_NOTIFIERS_NOTIFY                                     (0)
#define NV04E_NOTIFIERS_SET_DATA_NOTIFY(b)                         (1+(b))
#define NV04E_NOTIFIERS_SET_IMAGE_NOTIFY(b)                        (3+(b))
/* NvNotification[] fields and values */
#define NV04E_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV04E_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV04E_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV04E_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV04E_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV04E_NOTIFICATION_STATUS_WARNING_INVALID_DATA             (0x0001)
#define NV04E_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 NvV32 NoOperation;             /* ignored                          0100-0103*/
 NvV32 Notify;                  /* NV04E_NOTIFY_*                   0104-0107*/
 NvV32 StopTransfer;            /* NV04E_STOP_TRANSFER_VALUE        0108-010b*/
 NvV32 Reserved00[0x01d];
 NvV32 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA                 0180-0183*/
 NvV32 SetContextDmaData[2];    /* NV01_CONTEXT_DMA                 0184-018b*/
 NvV32 SetContextDmaImage[2];   /* NV01_CONTEXT_DMA                 018c-0193*/
 NvV32 Reserved01[0x05b];
 struct {                       /* start of methods in array        0300-    */
  NvU32 offset;                 /* in bytes                            0-   3*/
  NvU32 length;                 /* in bytes                            4-   7*/
  NvV32 notify;                 /* NV04E_SET_DATA_NOTIFY_*             8-   b*/
 } SetData[2];                  /* end of methods in array              -0317*/
 struct {                       /* start of methods in array        0318-    */
  NvV32 formatIn;               /* zero_field V16_V16                 00-  03*/
  NvV32 size;                   /* height_width U16_U16 in pixels     04-  07*/
  NvU32 offsetOut;              /* byte offset of top-left pixel      08-  0b*/
  NvV32 formatOut;              /* ignore_pitch V16_U16               0c-  0f*/
  NvV32 notify;                 /* NV04E_SET_IMAGE_NOTIFY_*           10-  13*/
 } SetImage[2];                 /* end of methods in array              -033f*/
 NvV32 Reserved02[0x730];
} Nv04eTypedef, Nv03ExternalVideoDecompressor;
#define NV04E_TYPEDEF                             nv03ExternalVideoDecompressor
/* dma method offsets, fields, and values */
#define NV04E_SET_OBJECT                                           (0x00000000)
#define NV04E_NO_OPERATION                                         (0x00000100)
#define NV04E_NOTIFY                                               (0x00000104)
#define NV04E_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV04E_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV04E_STOP_TRANSFER                                        (0x00000108)
#define NV04E_STOP_TRANSFER_VALUE                                  (0x00000000)
#define NV04E_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV04E_SET_CONTEXT_DMA_DATA(b)                              (0x00000184\
                                                                   +(b)*0x0004)
#define NV04E_SET_CONTEXT_DMA_IMAGE(b)                             (0x0000018C\
                                                                   +(b)*0x0004)
#define NV04E_SET_DATA(b)                                          (0x00000300\
                                                                   +(b)*0x000C)
#define NV04E_SET_DATA_OFFSET(b)                                   (0x00000300\
                                                                   +(b)*0x000C)
#define NV04E_SET_DATA_LENGTH(b)                                   (0x00000304\
                                                                   +(b)*0x000C)
#define NV04E_SET_DATA_NOTIFY(b)                                   (0x00000308\
                                                                   +(b)*0x000C)
#define NV04E_SET_DATA_NOTIFY_WRITE_ONLY                           (0x00000000)
#define NV04E_SET_DATA_NOTIFY_WRITE_THEN_AWAKEN                    (0x00000001)
#define NV04E_SET_IMAGE(b)                                         (0x00000318\
                                                                   +(b)*0x0014)
#define NV04E_SET_IMAGE_FORMAT_IN(b)                               (0x00000318\
                                                                   +(b)*0x0014)
#define NV04E_SET_IMAGE_FORMAT_IN_FIELD                            15:0
#define NV04E_SET_IMAGE_FORMAT_IN_FIELD_PROGRESSIVE                (0x00000001)
#define NV04E_SET_IMAGE_FORMAT_IN_FIELD_EVEN_FIELD                 (0x00000002)
#define NV04E_SET_IMAGE_FORMAT_IN_FIELD_ODD_FIELD                  (0x00000003)
#define NV04E_SET_IMAGE_FORMAT_IN_ZERO                             31:16
#define NV04E_SET_IMAGE_SIZE(b)                                    (0x0000031C\
                                                                   +(b)*0x0014)
#define NV04E_SET_IMAGE_SIZE_WIDTH                                 15:0
#define NV04E_SET_IMAGE_SIZE_HEIGHT                                31:16
#define NV04E_SET_IMAGE_OFFSET_OUT(b)                              (0x00000320\
                                                                   +(b)*0x0014)
#define NV04E_SET_IMAGE_FORMAT_OUT(b)                              (0x00000324\
                                                                   +(b)*0x0014)
#define NV04E_SET_IMAGE_FORMAT_OUT_PITCH                           15:0
#define NV04E_SET_IMAGE_FORMAT_OUT_IGNORE                          31:16
#define NV04E_SET_IMAGE_NOTIFY(b)                                  (0x00000324\
                                                                   +(b)*0x0014)
#define NV04E_SET_IMAGE_NOTIFY_WRITE_ONLY                          (0x00000000)
#define NV04E_SET_IMAGE_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)
/* obsolete stuff */
#define NV3_EXTERNAL_VIDEO_DECOMPRESSOR                            (0x0000004E)
#define Nv3ExternalVideoDecompressor              Nv03ExternalVideoDecompressor
#define nv3ExternalVideoDecompressor              nv03ExternalVideoDecompressor




/* class NV01_EXTERNAL_PARALLEL_BUS */
#define  NV01_EXTERNAL_PARALLEL_BUS                                (0x0000004F)
#define  NV1_EXTERNAL_PARALLEL_BUS                                 (0x0000004F)
/* NvNotification[] elements */
#define NV04F_NOTIFIERS_NOTIFY                                     (0)
#define NV04F_NOTIFICATION_READ_GET_DATA_NOTIFY                    (1)
#define NV04F_NOTIFICATION_SET_INTERRUPT_NOTIFY                    (2)
/* NvNotification[] fields and values */
#define NV04F_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV04F_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV04F_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV04F_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV04F_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV04F_NOTIFICATION_STATUS_WARNING_INVALID_DATA             (0x0001)
#define NV04F_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV04F_NOTIFY_*                    0104-0107*/
 V032 StopTransfer;            /* NV04F_STOP_TRANSFER_VALUE         0108-010b*/
 V032 Reserved00[0x01d];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 Reserved01[0x05f];
 V032 WriteSize;               /* address_data U16_U16              0300-0303*/
 U032 WriteAddress;            /* in bytes                          0304-0307*/
 V032 WriteData;               /* data                              0308-030b*/
 V032 ReadSize;                /* address_data U16_U16              030c-030f*/
 U032 ReadAddress;             /* in bytes                          0310-0313*/
 V032 ReadGetDataNotify;       /* NV04F_READ_GET_DATA_NOTIFY_*      0314-0317*/
 V032 SetInterruptNotify;      /* NV04F_SET_INTERRUPT_NOTIFY_*      0318-031b*/
 V032 Reserved02[0x739];
} Nv04fTypedef, Nv01ExternalParallelBus;
#define Nv1ExternalParallelBus                          Nv01ExternalParallelBus
#define nv1ExternalParallelBus                          nv01ExternalParallelBus
#define NV04F_TYPEDEF                                   nv01ExternalParallelBus
/* dma method offsets, fields, and values */
#define NV04F_SET_OBJECT                                           (0x00000000)
#define NV04F_NO_OPERATION                                         (0x00000100)
#define NV04F_NOTIFY                                               (0x00000104)
#define NV04F_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV04F_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV04F_STOP_TRANSFER                                        (0x00000108)
#define NV04F_STOP_TRANSFER_VALUE                                  (0x00000000)
#define NV04F_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV04F_WRITE                                                (0x00000300)
#define NV04F_WRITE_SIZE                                           (0x00000300)
#define NV04F_WRITE_SIZE_DATA                                      15:0
#define NV04F_WRITE_SIZE_ADDRESS                                   31:16
#define NV04F_WRITE_ADDRESS                                        (0x00000304)
#define NV04F_WRITE_DATA                                           (0x00000308)
#define NV04F_READ                                                 (0x0000030C)
#define NV04F_READ_SIZE                                            (0x0000030C)
#define NV04F_READ_SIZE_DATA                                       15:0
#define NV04F_READ_SIZE_ADDRESS                                    31:16
#define NV04F_READ_ADDRESS                                         (0x00000310)
#define NV04F_READ_GET_DATA_NOTIFY                                 (0x00000314)
#define NV04F_READ_GET_DATA_NOTIFY_WRITE_ONLY                      (0x00000000)
#define NV04F_READ_GET_DATA_NOTIFY_WRITE_THEN_AWAKEN               (0x00000001)
#define NV04F_SET_INTERRUPT_NOTIFY                                 (0x00000318)
#define NV04F_SET_INTERRUPT_NOTIFY_WRITE_ONLY                      (0x00000000)
#define NV04F_SET_INTERRUPT_NOTIFY_WRITE_THEN_AWAKEN               (0x00000001)


/* class NV03_EXTERNAL_MONITOR_BUS */
#define  NV03_EXTERNAL_MONITOR_BUS                                 (0x00000050)
#define  NV3_EXTERNAL_MONITOR_BUS                                  (0x00000050)
/* NvNotification[] elements */
#define NV050_NOTIFIERS_NOTIFY                                     (0)
#define NV050_NOTIFICATION_READ_GET_DATA_NOTIFY                    (1)
#define NV050_NOTIFICATION_SET_INTERRUPT_NOTIFY                    (2)
/* NvNotification[] fields and values */
#define NV050_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV050_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV050_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV050_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV050_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV050_NOTIFICATION_STATUS_WARNING_INVALID_DATA             (0x0001)
#define NV050_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV050_NOTIFY_*                    0104-0107*/
 V032 StopTransfer;            /* NV050_STOP_TRANSFER_VALUE         0108-010b*/
 V032 Reserved00[0x01d];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 Reserved01[0x05f];
 V032 WriteSize;               /* address_data U16_U16              0300-0303*/
 U032 WriteAddress;            /* in bytes                          0304-0307*/
 V032 WriteData;               /* data                              0308-030b*/
 V032 ReadSize;                /* address_data U16_U16              030c-030f*/
 U032 ReadAddress;             /* in bytes                          0310-0313*/
 V032 ReadGetDataNotify;       /* NV050_READ_GET_DATA_NOTIFY_*      0314-0317*/
 V032 SetInterruptNotify;      /* NV050_SET_INTERRUPT_NOTIFY_*      0318-031b*/
 V032 Reserved02[0x739];
} Nv050Typedef, Nv03ExternalMonitorBus;
#define Nv3ExternalMonitorBus                            Nv03ExternalMonitorBus
#define nv3ExternalMonitorBus                            nv03ExternalMonitorBus
#define NV050_TYPEDEF                                    nv03ExternalMonitorBus
/* dma method offsets, fields, and values */
#define NV050_SET_OBJECT                                           (0x00000000)
#define NV050_NO_OPERATION                                         (0x00000100)
#define NV050_NOTIFY                                               (0x00000104)
#define NV050_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV050_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV050_STOP_TRANSFER                                        (0x00000108)
#define NV050_STOP_TRANSFER_VALUE                                  (0x00000000)
#define NV050_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV050_WRITE                                                (0x00000300)
#define NV050_WRITE_SIZE                                           (0x00000300)
#define NV050_WRITE_SIZE_DATA                                      15:0
#define NV050_WRITE_SIZE_ADDRESS                                   31:16
#define NV050_WRITE_ADDRESS                                        (0x00000304)
#define NV050_WRITE_DATA                                           (0x00000308)
#define NV050_READ                                                 (0x0000030C)
#define NV050_READ_SIZE                                            (0x0000030C)
#define NV050_READ_SIZE_DATA                                       15:0
#define NV050_READ_SIZE_ADDRESS                                    31:16
#define NV050_READ_ADDRESS                                         (0x00000310)
#define NV050_READ_GET_DATA_NOTIFY                                 (0x00000314)
#define NV050_READ_GET_DATA_NOTIFY_WRITE_ONLY                      (0x00000000)
#define NV050_READ_GET_DATA_NOTIFY_WRITE_THEN_AWAKEN               (0x00000001)
#define NV050_SET_INTERRUPT_NOTIFY                                 (0x00000318)
#define NV050_SET_INTERRUPT_NOTIFY_WRITE_ONLY                      (0x00000000)
#define NV050_SET_INTERRUPT_NOTIFY_WRITE_THEN_AWAKEN               (0x00000001)


/* class NV03_EXTERNAL_SERIAL_BUS */
#define  NV03_EXTERNAL_SERIAL_BUS                                  (0x00000051)
#define  NV3_EXTERNAL_SERIAL_BUS                                   (0x00000051)
/* NvNotification[] elements */
#define NV051_NOTIFIERS_NOTIFY                                     (0)
#define NV051_NOTIFICATION_READ_GET_DATA_NOTIFY                    (1)
#define NV051_NOTIFICATION_SET_INTERRUPT_NOTIFY                    (2)
/* NvNotification[] fields and values */
#define NV051_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV051_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV051_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV051_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV051_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV051_NOTIFICATION_STATUS_WARNING_INVALID_DATA             (0x0001)
#define NV051_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV051_NOTIFY_*                    0104-0107*/
 V032 StopTransfer;            /* NV051_STOP_TRANSFER_VALUE         0108-010b*/
 V032 Reserved00[0x01d];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 Reserved01[0x05f];
 V032 WriteSize;               /* address_data U16_U16              0300-0303*/
 U032 WriteAddress;            /* in bytes                          0304-0307*/
 V032 WriteData;               /* data                              0308-030b*/
 V032 ReadSize;                /* address_data U16_U16              030c-030f*/
 U032 ReadAddress;             /* in bytes                          0310-0313*/
 V032 ReadGetDataNotify;       /* NV051_READ_GET_DATA_NOTIFY_*      0314-0317*/
 V032 SetInterruptNotify;      /* NV051_SET_INTERRUPT_NOTIFY_*      0318-031b*/
 V032 Reserved02[0x739];
} Nv051Typedef, Nv03ExternalSerialBus;
#define Nv3ExternalSerialBus                              Nv03ExternalSerialBus
#define nv3ExternalSerialBus                              nv03ExternalSerialBus
#define NV051_TYPEDEF                                     nv03ExternalSerialBus
/* dma method offsets, fields, and values */
#define NV051_SET_OBJECT                                           (0x00000000)
#define NV051_NO_OPERATION                                         (0x00000100)
#define NV051_NOTIFY                                               (0x00000104)
#define NV051_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV051_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV051_STOP_TRANSFER                                        (0x00000108)
#define NV051_STOP_TRANSFER_VALUE                                  (0x00000000)
#define NV051_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV051_WRITE                                                (0x00000300)
#define NV051_WRITE_SIZE                                           (0x00000300)
#define NV051_WRITE_SIZE_DATA                                      15:0
#define NV051_WRITE_SIZE_ADDRESS                                   31:16
#define NV051_WRITE_ADDRESS                                        (0x00000304)
#define NV051_WRITE_DATA                                           (0x00000308)
#define NV051_READ                                                 (0x0000030C)
#define NV051_READ_SIZE                                            (0x0000030C)
#define NV051_READ_SIZE_DATA                                       15:0
#define NV051_READ_SIZE_ADDRESS                                    31:16
#define NV051_READ_ADDRESS                                         (0x00000310)
#define NV051_READ_GET_DATA_NOTIFY                                 (0x00000314)
#define NV051_READ_GET_DATA_NOTIFY_WRITE_ONLY                      (0x00000000)
#define NV051_READ_GET_DATA_NOTIFY_WRITE_THEN_AWAKEN               (0x00000001)
#define NV051_SET_INTERRUPT_NOTIFY                                 (0x00000318)
#define NV051_SET_INTERRUPT_NOTIFY_WRITE_ONLY                      (0x00000000)
#define NV051_SET_INTERRUPT_NOTIFY_WRITE_THEN_AWAKEN               (0x00000001)


/* class NV04_CONTEXT_SURFACE_SWIZZLED */
#define  NV04_CONTEXT_SURFACE_SWIZZLED                             (0x00000052)
#define  NV4_CONTEXT_SURFACE_SWIZZLED                              (0x00000052)
/* NvNotification[] elements */
#define NV052_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV052_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV052_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV052_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV052_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV052_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV052_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV052_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaImage;      /* NV01_CONTEXT_DMA_IN_MEMORY        0184-0187*/
 V032 Reserved01[0x05e];
 V032 SetFormat;               /* height_width_color V8_V8_V16      0300-0303*/
 U032 SetOffset;               /* byte offset of top-left pixel     0304-0307*/
 V032 Reserved02[0x73e];
} Nv052Typedef, Nv04ContextSurfaceSwizzled;
#define Nv4ContextSurfaceSwizzled                    Nv04ContextSurfaceSwizzled
#define nv4ContextSurfaceSwizzled                    nv04ContextSurfaceSwizzled
#define NV052_TYPEDEF                                nv04ContextSurfaceSwizzled
/* dma method offsets, fields, and values */
#define NV052_SET_OBJECT                                           (0x00000000)
#define NV052_NO_OPERATION                                         (0x00000100)
#define NV052_NOTIFY                                               (0x00000104)
#define NV052_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV052_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV052_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV052_SET_CONTEXT_DMA_IMAGE                                (0x00000184)
#define NV052_SET_FORMAT                                           (0x00000300)
#define NV052_SET_FORMAT_COLOR                                     15:0
#define NV052_SET_FORMAT_COLOR_LE_Y8                               (0x00000001)
#define NV052_SET_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5                (0x00000002)
#define NV052_SET_FORMAT_COLOR_LE_X1R5G5B5_O1R5G5B5                (0x00000003)
#define NV052_SET_FORMAT_COLOR_LE_R5G6B5                           (0x00000004)
#define NV052_SET_FORMAT_COLOR_LE_Y16                              (0x00000005)
#define NV052_SET_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8                (0x00000006)
#define NV052_SET_FORMAT_COLOR_LE_X8R8G8B8_O8R8G8B8                (0x00000007)
#define NV052_SET_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8            (0x00000008)
#define NV052_SET_FORMAT_COLOR_LE_X1A7R8G8B8_O1A7R8G8B8            (0x00000009)
#define NV052_SET_FORMAT_COLOR_LE_A8R8G8B8                         (0x0000000A)
#define NV052_SET_FORMAT_COLOR_LE_Y32                              (0x0000000B)
#define NV052_SET_FORMAT_WIDTH                                     23:16
#define NV052_SET_FORMAT_WIDTH_1                                   (0x00000000)
#define NV052_SET_FORMAT_WIDTH_2                                   (0x00000001)
#define NV052_SET_FORMAT_WIDTH_4                                   (0x00000002)
#define NV052_SET_FORMAT_WIDTH_8                                   (0x00000003)
#define NV052_SET_FORMAT_WIDTH_16                                  (0x00000004)
#define NV052_SET_FORMAT_WIDTH_32                                  (0x00000005)
#define NV052_SET_FORMAT_WIDTH_64                                  (0x00000006)
#define NV052_SET_FORMAT_WIDTH_128                                 (0x00000007)
#define NV052_SET_FORMAT_WIDTH_256                                 (0x00000008)
#define NV052_SET_FORMAT_WIDTH_512                                 (0x00000009)
#define NV052_SET_FORMAT_WIDTH_1024                                (0x0000000A)
#define NV052_SET_FORMAT_WIDTH_2048                                (0x0000000B)
#define NV052_SET_FORMAT_HEIGHT                                    31:24
#define NV052_SET_FORMAT_HEIGHT_1                                  (0x00000000)
#define NV052_SET_FORMAT_HEIGHT_2                                  (0x00000001)
#define NV052_SET_FORMAT_HEIGHT_4                                  (0x00000002)
#define NV052_SET_FORMAT_HEIGHT_8                                  (0x00000003)
#define NV052_SET_FORMAT_HEIGHT_16                                 (0x00000004)
#define NV052_SET_FORMAT_HEIGHT_32                                 (0x00000005)
#define NV052_SET_FORMAT_HEIGHT_64                                 (0x00000006)
#define NV052_SET_FORMAT_HEIGHT_128                                (0x00000007)
#define NV052_SET_FORMAT_HEIGHT_256                                (0x00000008)
#define NV052_SET_FORMAT_HEIGHT_512                                (0x00000009)
#define NV052_SET_FORMAT_HEIGHT_1024                               (0x0000000A)
#define NV052_SET_FORMAT_HEIGHT_2048                               (0x0000000B)
#define NV052_SET_OFFSET                                           (0x00000304)


/* class NV04_CONTEXT_SURFACES_ARGB_ZS */
#define  NV04_CONTEXT_SURFACES_ARGB_ZS                             (0x00000053)
#define  NV4_CONTEXT_SURFACES_ARGB_ZS                              (0x00000053)
/* NvNotification[] elements */
#define NV053_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV053_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV053_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV053_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV053_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV053_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV053_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 NvV32 NoOperation;             /* ignored                          0100-0103*/
 NvV32 Notify;                  /* NV053_NOTIFY_*                   0104-0107*/
 NvV32 Reserved00[0x01e];
 NvV32 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA                 0180-0183*/
 NvV32 SetContextDmaColor;      /* NV01_CONTEXT_DMA                 0184-0187*/
 NvV32 SetContextDmaZeta;       /* NV01_CONTEXT_DMA                 0188-018b*/
 NvV32 Reserved01[0x05b];
 NvV32 SetClipHorizontal;       /* width_x U16_U16                  02F8-02FB*/
 NvV32 SetClipVertical;         /* height_y U16_U16                 02FC-02FF*/
 NvV32 SetFormat;               /* height_width_type_color all V8   0300-0303*/
 NvV32 SetClipSize;             /* height_width U16_U16             0304-0307*/
 NvU32 SetPitch;                /* zeta_color U16_U16               0308-030b*/
 NvU32 SetOffsetColor;          /* byte offset of top-left pixel    030c-030f*/
 NvU32 SetOffsetZeta;           /* byte offset of top-left pixel    0310-0313*/
 NvV32 Reserved02[0x73b];
} Nv053Typedef, Nv04ContextSurfacesArgbZs;
#define Nv4ContextSurfacesArgbZs                      Nv04ContextSurfacesArgbZs
#define nv4ContextSurfacesArgbZs                      nv04ContextSurfacesArgbZs
#define NV053_TYPEDEF                                 nv04ContextSurfacesArgbZs
/* dma method offsets, fields, and values */
#define NV053_SET_OBJECT                                           (0x00000000)
#define NV053_NO_OPERATION                                         (0x00000100)
#define NV053_NOTIFY                                               (0x00000104)
#define NV053_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV053_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV053_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV053_SET_CONTEXT_DMA_COLOR                                (0x00000184)
#define NV053_SET_CONTEXT_DMA_ZETA                                 (0x00000188)
#define NV053_SET_CLIP_HORIZONTAL                                  (0x000002F8)
#define NV053_SET_CLIP_HORIZONTAL_X                                15:0
#define NV053_SET_CLIP_HORIZONTAL_WIDTH                            31:16
#define NV053_SET_CLIP_VERTICAL                                    (0x000002FC)
#define NV053_SET_CLIP_VERTICAL_Y                                  15:0
#define NV053_SET_CLIP_VERTICAL_HEIGHT                             31:16
#define NV053_SET_FORMAT                                           (0x00000300)
#define NV053_SET_FORMAT_COLOR                                     7:0
#define NV053_SET_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5                (0x00000001)
#define NV053_SET_FORMAT_COLOR_LE_X1R5G5B5_O1R5G5B5                (0x00000002)
#define NV053_SET_FORMAT_COLOR_LE_R5G6B5                           (0x00000003)
#define NV053_SET_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8                (0x00000004)
#define NV053_SET_FORMAT_COLOR_LE_X8R8G8B8_O8R8G8B8                (0x00000005)
#define NV053_SET_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8            (0x00000006)
#define NV053_SET_FORMAT_COLOR_LE_X1A7R8G8B8_O1A7R8G8B8            (0x00000007)
#define NV053_SET_FORMAT_COLOR_LE_A8R8G8B8                         (0x00000008)
#define NV053_SET_FORMAT_TYPE                                      15:8
#define NV053_SET_FORMAT_TYPE_PITCH                                (0x00000001)
#define NV053_SET_FORMAT_TYPE_SWIZZLE                              (0x00000002)
#define NV053_SET_FORMAT_WIDTH                                     23:16
#define NV053_SET_FORMAT_HEIGHT                                    31:24
#define NV053_SET_CLIP_SIZE                                        (0x00000304)
#define NV053_SET_CLIP_SIZE_WIDTH                                  15:0
#define NV053_SET_CLIP_SIZE_HEIGHT                                 31:16
#define NV053_SET_PITCH                                            (0x00000308)
#define NV053_SET_PITCH_COLOR                                      15:0
#define NV053_SET_PITCH_ZETA                                       31:16
#define NV053_SET_OFFSET_COLOR                                     (0x0000030C)
#define NV053_SET_OFFSET_ZETA                                      (0x00000310)


/* class NV04_DX5_TEXTURED_TRIANGLE */
#define  NV04_DX5_TEXTURED_TRIANGLE                                (0x00000054)
#define  NV4_DX5_TEXTURED_TRIANGLE                                 (0x00000054)
/* NvNotification[] elements */
#define NV054_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV054_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV054_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV054_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV054_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV054_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV054_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV054_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaA;          /* NV01_CONTEXT_DMA_FROM_MEMORY      0184-0187*/
 V032 SetContextDmaB;          /* NV01_CONTEXT_DMA_FROM_MEMORY      0188-018b*/
 V032 SetContextSurfaces;      /* NV04_CONTEXT_SURFACES_ARGB_ZS     018c-018f*/
 V032 Reserved01[0x05c];
 V032 ColorKey;                /* texture color key value           0300-0303*/
 U032 Offset;                  /* texture offset in bytes           0304-0307*/
 V032 Format;                  /* see text                          0308-030b*/
 V032 Filter;                  /* see text                          030c-030f*/
 V032 Blend;                   /* see text                          0310-0313*/
 V032 Control;                 /* see text                          0314-0317*/
 V032 FogColor;                /* X8R8G8B8 fog color                0318-031b*/
 V032 Reserved02[0x039];
 struct {                      /* start of method in array          0400-    */
  F032 sx;                     /* screen x coordinate                 00-  03*/
  F032 sy;                     /* screen y coordinate                 04-  07*/
  F032 sz;                     /* screen z coordinate                 08-  0b*/
  F032 rhw;                    /* reciprocal homogeneous W (1/W)      0c-  0f*/
  V032 color;                  /* A8R8G8B8                            10-  13*/
  V032 specular;               /* F8R8G8B8                            14-  17*/
  F032 tu;                     /* texture u coordinate                18-  1b*/
  F032 tv;                     /* texture v coordinate                1c-  1f*/
 } Tlvertex[16];               /* end of method in array                -05ff*/
 V032 DrawPrimitive[64];       /* see text                          0600-06ff*/
 V032 Reserved03[0x640];
} Nv054Typedef, Nv04Dx5TexturedTriangle;
#define Nv4Dx5TexturedTriangle                          Nv04Dx5TexturedTriangle
#define nv4Dx5TexturedTriangle                          nv04Dx5TexturedTriangle
#define NV054_TYPEDEF                                   nv04Dx5TexturedTriangle
/* dma method offsets, fields, and values */
#define NV054_SET_OBJECT                                           (0x00000000)
#define NV054_NO_OPERATION                                         (0x00000100)
#define NV054_NOTIFY                                               (0x00000104)
#define NV054_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV054_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV054_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV054_SET_CONTEXT_DMA_A                                    (0x00000184)
#define NV054_SET_CONTEXT_DMA_B                                    (0x00000188)
#define NV054_SET_CONTEXT_SURFACES                                 (0x0000018C)
#define NV054_COLOR_KEY                                            (0x00000300)
#define NV054_OFFSET                                               (0x00000304)
#define NV054_FORMAT                                               (0x00000308)
#define NV054_FORMAT_CONTEXT_DMA                                   1:0
#define NV054_FORMAT_CONTEXT_DMA_A                                 (0x00000001)
#define NV054_FORMAT_CONTEXT_DMA_B                                 (0x00000002)
#define NV054_FORMAT_COLORKEYENABLE                                3:2
#define NV054_FORMAT_COLORKEYENABLE_FALSE                          (0x00000000)
#define NV054_FORMAT_COLORKEYENABLE_TRUE                           (0x00000001)
#define NV054_FORMAT_ORIGIN_ZOH                                    5:4
#define NV054_FORMAT_ORIGIN_ZOH_CENTER                             (0x00000001)
#define NV054_FORMAT_ORIGIN_ZOH_CORNER                             (0x00000002)
#define NV054_FORMAT_ORIGIN_FOH                                    7:6
#define NV054_FORMAT_ORIGIN_FOH_CENTER                             (0x00000001)
#define NV054_FORMAT_ORIGIN_FOH_CORNER                             (0x00000002)
#define NV054_FORMAT_COLOR                                         11:8
#define NV054_FORMAT_COLOR_LE_Y8                                   (0x00000001)
#define NV054_FORMAT_COLOR_LE_A1R5G5B5                             (0x00000002)
#define NV054_FORMAT_COLOR_LE_X1R5G5B5                             (0x00000003)
#define NV054_FORMAT_COLOR_LE_A4R4G4B4                             (0x00000004)
#define NV054_FORMAT_COLOR_LE_R5G6B5                               (0x00000005)
#define NV054_FORMAT_COLOR_LE_A8R8G8B8                             (0x00000006)
#define NV054_FORMAT_COLOR_LE_X8R8G8B8                             (0x00000007)
#define NV054_FORMAT_MIPMAP_LEVELS                                 15:12
#define NV054_FORMAT_BASE_SIZE_U                                   19:16
#define NV054_FORMAT_BASE_SIZE_U_1                                 (0x00000000)
#define NV054_FORMAT_BASE_SIZE_U_2                                 (0x00000001)
#define NV054_FORMAT_BASE_SIZE_U_4                                 (0x00000002)
#define NV054_FORMAT_BASE_SIZE_U_8                                 (0x00000003)
#define NV054_FORMAT_BASE_SIZE_U_16                                (0x00000004)
#define NV054_FORMAT_BASE_SIZE_U_32                                (0x00000005)
#define NV054_FORMAT_BASE_SIZE_U_64                                (0x00000006)
#define NV054_FORMAT_BASE_SIZE_U_128                               (0x00000007)
#define NV054_FORMAT_BASE_SIZE_U_256                               (0x00000008)
#define NV054_FORMAT_BASE_SIZE_U_512                               (0x00000009)
#define NV054_FORMAT_BASE_SIZE_U_1024                              (0x0000000A)
#define NV054_FORMAT_BASE_SIZE_U_2048                              (0x0000000B)
#define NV054_FORMAT_BASE_SIZE_V                                   23:20
#define NV054_FORMAT_BASE_SIZE_V_1                                 (0x00000000)
#define NV054_FORMAT_BASE_SIZE_V_2                                 (0x00000001)
#define NV054_FORMAT_BASE_SIZE_V_4                                 (0x00000002)
#define NV054_FORMAT_BASE_SIZE_V_8                                 (0x00000003)
#define NV054_FORMAT_BASE_SIZE_V_16                                (0x00000004)
#define NV054_FORMAT_BASE_SIZE_V_32                                (0x00000005)
#define NV054_FORMAT_BASE_SIZE_V_64                                (0x00000006)
#define NV054_FORMAT_BASE_SIZE_V_128                               (0x00000007)
#define NV054_FORMAT_BASE_SIZE_V_256                               (0x00000008)
#define NV054_FORMAT_BASE_SIZE_V_512                               (0x00000009)
#define NV054_FORMAT_BASE_SIZE_V_1024                              (0x0000000A)
#define NV054_FORMAT_BASE_SIZE_V_2048                              (0x0000000B)
#define NV054_FORMAT_TEXTUREADDRESSU                               26:24
#define NV054_FORMAT_TEXTUREADDRESSU_WRAP                          (0x00000001)
#define NV054_FORMAT_TEXTUREADDRESSU_MIRROR                        (0x00000002)
#define NV054_FORMAT_TEXTUREADDRESSU_CLAMP                         (0x00000003)
#define NV054_FORMAT_TEXTUREADDRESSU_BORDER                        (0x00000004)
#define NV054_FORMAT_WRAPU                                         27:27
#define NV054_FORMAT_WRAPU_FALSE                                   (0x00000000)
#define NV054_FORMAT_WRAPU_TRUE                                    (0x00000001)
#define NV054_FORMAT_TEXTUREADDRESSV                               30:28
#define NV054_FORMAT_TEXTUREADDRESSV_WRAP                          (0x00000001)
#define NV054_FORMAT_TEXTUREADDRESSV_MIRROR                        (0x00000002)
#define NV054_FORMAT_TEXTUREADDRESSV_CLAMP                         (0x00000003)
#define NV054_FORMAT_TEXTUREADDRESSV_BORDER                        (0x00000004)
#define NV054_FORMAT_WRAPV                                         31:31
#define NV054_FORMAT_WRAPV_FALSE                                   (0x00000000)
#define NV054_FORMAT_WRAPV_TRUE                                    (0x00000001)
#define NV054_FILTER                                               (0x0000030C)
#define NV054_FILTER_KERNEL_SIZE_X                                 7:0
#define NV054_FILTER_KERNEL_SIZE_Y                                 14:8
#define NV054_FILTER_MIPMAP_DITHER_ENABLE                          15:15
#define NV054_FILTER_MIPMAP_DITHER_ENABLE_FALSE                    (0x00000000)
#define NV054_FILTER_MIPMAP_DITHER_ENABLE_TRUE                     (0x00000001)
#define NV054_FILTER_MIPMAPLODBIAS                                 23:16
#define NV054_FILTER_TEXTUREMIN                                    26:24
#define NV054_FILTER_TEXTUREMIN_NEAREST                            (0x00000001)
#define NV054_FILTER_TEXTUREMIN_LINEAR                             (0x00000002)
#define NV054_FILTER_TEXTUREMIN_MIPNEAREST                         (0x00000003)
#define NV054_FILTER_TEXTUREMIN_MIPLINEAR                          (0x00000004)
#define NV054_FILTER_TEXTUREMIN_LINEARMIPNEAREST                   (0x00000005)
#define NV054_FILTER_TEXTUREMIN_LINEARMIPLINEAR                    (0x00000006)
#define NV054_FILTER_ANISOTROPIC_MINIFY_ENABLE                     27:27
#define NV054_FILTER_ANISOTROPIC_MINIFY_ENABLE_FALSE               (0x00000000)
#define NV054_FILTER_ANISOTROPIC_MINIFY_ENABLE_TRUE                (0x00000001)
#define NV054_FILTER_TEXTUREMAG                                    30:28
#define NV054_FILTER_TEXTUREMAG_NEAREST                            (0x00000001)
#define NV054_FILTER_TEXTUREMAG_LINEAR                             (0x00000002)
#define NV054_FILTER_TEXTUREMAG_MIPNEAREST                         (0x00000003)
#define NV054_FILTER_TEXTUREMAG_MIPLINEAR                          (0x00000004)
#define NV054_FILTER_TEXTUREMAG_LINEARMIPNEAREST                   (0x00000005)
#define NV054_FILTER_TEXTUREMAG_LINEARMIPLINEAR                    (0x00000006)
#define NV054_FILTER_ANISOTROPIC_MAGNIFY_ENABLE                    31:31
#define NV054_FILTER_ANISOTROPIC_MAGNIFY_ENABLE_FALSE              (0x00000000)
#define NV054_FILTER_ANISOTROPIC_MAGNIFY_ENABLE_TRUE               (0x00000001)
#define NV054_BLEND                                                (0x00000310)
#define NV054_BLEND_TEXTUREMAPBLEND                                3:0
#define NV054_BLEND_TEXTUREMAPBLEND_DECAL                          (0x00000001)
#define NV054_BLEND_TEXTUREMAPBLEND_MODULATE                       (0x00000002)
#define NV054_BLEND_TEXTUREMAPBLEND_DECALALPHA                     (0x00000003)
#define NV054_BLEND_TEXTUREMAPBLEND_MODULATEALPHA                  (0x00000004)
#define NV054_BLEND_TEXTUREMAPBLEND_DECALMASK                      (0x00000005)
#define NV054_BLEND_TEXTUREMAPBLEND_MODULATEMASK                   (0x00000006)
#define NV054_BLEND_TEXTUREMAPBLEND_COPY                           (0x00000007)
#define NV054_BLEND_TEXTUREMAPBLEND_ADD                            (0x00000008)
#define NV054_BLEND_MASK_BIT                                       5:4
#define NV054_BLEND_MASK_BIT_LSB                                   (0x00000001)
#define NV054_BLEND_MASK_BIT_MSB                                   (0x00000002)
#define NV054_BLEND_SHADEMODE                                      7:6
#define NV054_BLEND_SHADEMODE_FLAT                                 (0x00000001)
#define NV054_BLEND_SHADEMODE_GOURAUD                              (0x00000002)
#define NV054_BLEND_SHADEMODE_PHONG                                (0x00000003)
#define NV054_BLEND_TEXTUREPERSPECTIVE                             11:8
#define NV054_BLEND_TEXTUREPERSPECTIVE_FALSE                       (0x00000000)
#define NV054_BLEND_TEXTUREPERSPECTIVE_TRUE                        (0x00000001)
#define NV054_BLEND_SPECULARENABLE                                 15:12
#define NV054_BLEND_SPECULARENABLE_FALSE                           (0x00000000)
#define NV054_BLEND_SPECULARENABLE_TRUE                            (0x00000001)
#define NV054_BLEND_FOGENABLE                                      19:16
#define NV054_BLEND_FOGENABLE_FALSE                                (0x00000000)
#define NV054_BLEND_FOGENABLE_TRUE                                 (0x00000001)
#define NV054_BLEND_ALPHABLENDENABLE                               23:20
#define NV054_BLEND_ALPHABLENDENABLE_FALSE                         (0x00000000)
#define NV054_BLEND_ALPHABLENDENABLE_TRUE                          (0x00000001)
#define NV054_BLEND_SRCBLEND                                       27:24
#define NV054_BLEND_SRCBLEND_ZERO                                  (0x00000001)
#define NV054_BLEND_SRCBLEND_ONE                                   (0x00000002)
#define NV054_BLEND_SRCBLEND_SRCCOLOR                              (0x00000003)
#define NV054_BLEND_SRCBLEND_INVSRCCOLOR                           (0x00000004)
#define NV054_BLEND_SRCBLEND_SRCALPHA                              (0x00000005)
#define NV054_BLEND_SRCBLEND_INVSRCALPHA                           (0x00000006)
#define NV054_BLEND_SRCBLEND_DESTALPHA                             (0x00000007)
#define NV054_BLEND_SRCBLEND_INVDESTALPHA                          (0x00000008)
#define NV054_BLEND_SRCBLEND_DESTCOLOR                             (0x00000009)
#define NV054_BLEND_SRCBLEND_INVDESTCOLOR                          (0x0000000A)
#define NV054_BLEND_SRCBLEND_SRCALPHASAT                           (0x0000000B)
#define NV054_BLEND_DESTBLEND                                      31:28
#define NV054_BLEND_DESTBLEND_ZERO                                 (0x00000001)
#define NV054_BLEND_DESTBLEND_ONE                                  (0x00000002)
#define NV054_BLEND_DESTBLEND_SRCCOLOR                             (0x00000003)
#define NV054_BLEND_DESTBLEND_INVSRCCOLOR                          (0x00000004)
#define NV054_BLEND_DESTBLEND_SRCALPHA                             (0x00000005)
#define NV054_BLEND_DESTBLEND_INVSRCALPHA                          (0x00000006)
#define NV054_BLEND_DESTBLEND_DESTALPHA                            (0x00000007)
#define NV054_BLEND_DESTBLEND_INVDESTALPHA                         (0x00000008)
#define NV054_BLEND_DESTBLEND_DESTCOLOR                            (0x00000009)
#define NV054_BLEND_DESTBLEND_INVDESTCOLOR                         (0x0000000A)
#define NV054_BLEND_DESTBLEND_SRCALPHASAT                          (0x0000000B)
#define NV054_CONTROL                                              (0x00000314)
#define NV054_CONTROL_ALPHAREF                                     7:0
#define NV054_CONTROL_ALPHAFUNC                                    11:8
#define NV054_CONTROL_ALPHAFUNC_NEVER                              (0x00000001)
#define NV054_CONTROL_ALPHAFUNC_LESS                               (0x00000002)
#define NV054_CONTROL_ALPHAFUNC_EQUAL                              (0x00000003)
#define NV054_CONTROL_ALPHAFUNC_LESSEQUAL                          (0x00000004)
#define NV054_CONTROL_ALPHAFUNC_GREATER                            (0x00000005)
#define NV054_CONTROL_ALPHAFUNC_NOTEQUAL                           (0x00000006)
#define NV054_CONTROL_ALPHAFUNC_GREATEREQUAL                       (0x00000007)
#define NV054_CONTROL_ALPHAFUNC_ALWAYS                             (0x00000008)
#define NV054_CONTROL_ALPHATESTENABLE                              12:12
#define NV054_CONTROL_ALPHATESTENABLE_FALSE                        (0x00000000)
#define NV054_CONTROL_ALPHATESTENABLE_TRUE                         (0x00000001)
#define NV054_CONTROL_ORIGIN                                       13:13
#define NV054_CONTROL_ORIGIN_CENTER                                (0x00000000)
#define NV054_CONTROL_ORIGIN_CORNER                                (0x00000001)
#define NV054_CONTROL_ZENABLE                                      15:14
#define NV054_CONTROL_ZENABLE_FALSE                                (0x00000000)
#define NV054_CONTROL_ZENABLE_TRUE                                 (0x00000001)
#define NV054_CONTROL_ZFUNC                                        19:16
#define NV054_CONTROL_ZFUNC_NEVER                                  (0x00000001)
#define NV054_CONTROL_ZFUNC_LESS                                   (0x00000002)
#define NV054_CONTROL_ZFUNC_EQUAL                                  (0x00000003)
#define NV054_CONTROL_ZFUNC_LESSEQUAL                              (0x00000004)
#define NV054_CONTROL_ZFUNC_GREATER                                (0x00000005)
#define NV054_CONTROL_ZFUNC_NOTEQUAL                               (0x00000006)
#define NV054_CONTROL_ZFUNC_GREATEREQUAL                           (0x00000007)
#define NV054_CONTROL_ZFUNC_ALWAYS                                 (0x00000008)
#define NV054_CONTROL_CULLMODE                                     21:20
#define NV054_CONTROL_CULLMODE_NONE                                (0x00000001)
#define NV054_CONTROL_CULLMODE_CW                                  (0x00000002)
#define NV054_CONTROL_CULLMODE_CCW                                 (0x00000003)
#define NV054_CONTROL_DITHERENABLE                                 22:22
#define NV054_CONTROL_DITHERENABLE_FALSE                           (0x00000000)
#define NV054_CONTROL_DITHERENABLE_TRUE                            (0x00000001)
#define NV054_CONTROL_Z_PERSPECTIVE_ENABLE                         23:23
#define NV054_CONTROL_Z_PERSPECTIVE_ENABLE_FALSE                   (0x00000000)
#define NV054_CONTROL_Z_PERSPECTIVE_ENABLE_TRUE                    (0x00000001)
#define NV054_CONTROL_ZWRITEENABLE                                 29:24
#define NV054_CONTROL_ZWRITEENABLE_FALSE                           (0x00000000)
#define NV054_CONTROL_ZWRITEENABLE_TRUE                            (0x00000001)
#define NV054_CONTROL_Z_FORMAT                                     31:30
#define NV054_CONTROL_Z_FORMAT_FIXED                               (0x00000001)
#define NV054_CONTROL_Z_FORMAT_FLOAT                               (0x00000002)
#define NV054_FOG_COLOR                                            (0x00000318)
#define NV054_TLVERTEX(i)                                          (0x00000400\
                                                                   +(i)*0x0020)
#define NV054_TLVERTEX_SX(i)                                       (0x00000400\
                                                                   +(i)*0x0020)
#define NV054_TLVERTEX_SY(i)                                       (0x00000404\
                                                                   +(i)*0x0020)
#define NV054_TLVERTEX_SZ(i)                                       (0x00000408\
                                                                   +(i)*0x0020)
#define NV054_TLVERTEX_RHW(i)                                      (0x0000040C\
                                                                   +(i)*0x0020)
#define NV054_TLVERTEX_COLOR(i)                                    (0x00000410\
                                                                   +(i)*0x0020)
#define NV054_TLVERTEX_COLOR_BLUE                                  7:0
#define NV054_TLVERTEX_COLOR_GREEN                                 15:8
#define NV054_TLVERTEX_COLOR_RED                                   23:16
#define NV054_TLVERTEX_COLOR_ALPHA                                 31:24
#define NV054_TLVERTEX_SPECULAR(i)                                 (0x00000414\
                                                                   +(i)*0x0020)
#define NV054_TLVERTEX_SPECULAR_BLUE                               7:0
#define NV054_TLVERTEX_SPECULAR_GREEN                              15:8
#define NV054_TLVERTEX_SPECULAR_RED                                23:16
#define NV054_TLVERTEX_SPECULAR_FOG                                31:24
#define NV054_TLVERTEX_TU(i)                                       (0x00000418\
                                                                   +(i)*0x0020)
#define NV054_TLVERTEX_TV(i)                                       (0x0000041C\
                                                                   +(i)*0x0020)
#define NV054_DRAW_PRIMITIVE(a)                                    (0x00000600\
                                                                   +(a)*0x0004)
#define NV054_DRAW_PRIMITIVE_I0                                    3:0
#define NV054_DRAW_PRIMITIVE_I1                                    7:4
#define NV054_DRAW_PRIMITIVE_I2                                    11:8
#define NV054_DRAW_PRIMITIVE_I3                                    15:12
#define NV054_DRAW_PRIMITIVE_I4                                    19:16
#define NV054_DRAW_PRIMITIVE_I5                                    31:20


/* class NV04_DX6_MULTI_TEXTURE_TRIANGLE */
#define  NV04_DX6_MULTI_TEXTURE_TRIANGLE                           (0x00000055)
#define  NV4_DX6_MULTI_TEXTURE_TRIANGLE                            (0x00000055)
/* NvNotification[] elements */
#define NV055_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV055_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV055_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV055_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV055_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV055_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV055_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV055_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaA;          /* NV01_CONTEXT_DMA_FROM_MEMORY      0184-0187*/
 V032 SetContextDmaB;          /* NV01_CONTEXT_DMA_FROM_MEMORY      0188-018b*/
 V032 SetContextSurfaces;      /* NV01_CONTEXT_SURFACES_ARGB_ZS     018c-018f*/
 V032 Reserved01[0x05e];
 U032 Offset[2];               /* offset in bytes                   0308-030f*/
 V032 Format[2];               /* see text                          0310-0317*/
 V032 Filter[2];               /* see text                          0318-031f*/
 V032 Combine0Alpha;           /* see text                          0320-0323*/
 V032 Combine0Color;           /* see text                          0324-0327*/
 V032 Reserved02[0x001];
 V032 Combine1Alpha;           /* see text                          032c-032f*/
 V032 Combine1Color;           /* see text                          0330-0333*/
 V032 CombineFactor;           /* A8R8G8B8                          0334-0337*/
 V032 Blend;                   /* see text                          0338-033b*/
 V032 Control0;                /* see text                          033c-033f*/
 V032 Control1;                /* see text                          0340-0343*/
 V032 Control2;                /* see text                          0344-0347*/
 V032 FogColor;                /* X8R8G8B8 fog color                0348-034b*/
 V032 Reserved03[0x02D];
 struct {                      /* start of method in array          0400-    */
  F032 sx;                     /* screen x coordinate                 00-  03*/
  F032 sy;                     /* screen y coordinate                 04-  07*/
  F032 sz;                     /* screen z coordinate                 08-  0b*/
  F032 rhw;                    /* reciprocal homogeneous W (1/W)      0c-  0f*/
  V032 color;                  /* A8R8G8B8                            10-  13*/
  V032 specular;               /* F8R8G8B8                            14-  17*/
  F032 tu0;                    /* texture 0 u coordinate              18-  1b*/
  F032 tv0;                    /* texture 0 v coordinate              1c-  1f*/
  F032 tu1;                    /* texture 1 u coordinate              20-  23*/
  F032 tv1;                    /* texture 1 v coordinate              24-  27*/
 } Tlmtvertex[8];              /* end of method in array                -053f*/
 V032 DrawPrimitive[48];       /* see text                          0540-05ff*/
 V032 Reserved04[0x680];
} Nv055Typedef, Nv04Dx6MultiTextureTriangle;
#define Nv4Dx6MultiTextureTriangle                  Nv04Dx6MultiTextureTriangle
#define nv4Dx6MultiTextureTriangle                  nv04Dx6MultiTextureTriangle
#define NV055_TYPEDEF                               nv04Dx6MultiTextureTriangle
/* dma method offsets, fields, and values */
#define NV055_SET_OBJECT                                           (0x00000000)
#define NV055_NO_OPERATION                                         (0x00000100)
#define NV055_NOTIFY                                               (0x00000104)
#define NV055_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV055_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV055_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV055_SET_CONTEXT_DMA_A                                    (0x00000184)
#define NV055_SET_CONTEXT_DMA_B                                    (0x00000188)
#define NV055_SET_CONTEXT_SURFACES                                 (0x0000018C)
#define NV055_OFFSET(b)                                            (0x00000308\
                                                                   +(b)*0x0004)
#define NV055_FORMAT(b)                                            (0x00000310\
                                                                   +(b)*0x0004)
#define NV055_FORMAT_CONTEXT_DMA                                   3:0
#define NV055_FORMAT_CONTEXT_DMA_A                                 (0x00000001)
#define NV055_FORMAT_CONTEXT_DMA_B                                 (0x00000002)
#define NV055_FORMAT_ORIGIN_ZOH                                    5:4
#define NV055_FORMAT_ORIGIN_ZOH_CENTER                             (0x00000001)
#define NV055_FORMAT_ORIGIN_ZOH_CORNER                             (0x00000002)
#define NV055_FORMAT_ORIGIN_FOH                                    7:6
#define NV055_FORMAT_ORIGIN_FOH_CENTER                             (0x00000001)
#define NV055_FORMAT_ORIGIN_FOH_CORNER                             (0x00000002)
#define NV055_FORMAT_COLOR                                         11:8
#define NV055_FORMAT_COLOR_LE_AY8                                  (0x00000001)
#define NV055_FORMAT_COLOR_LE_A1R5G5B5                             (0x00000002)
#define NV055_FORMAT_COLOR_LE_X1R5G5B5                             (0x00000003)
#define NV055_FORMAT_COLOR_LE_A4R4G4B4                             (0x00000004)
#define NV055_FORMAT_COLOR_LE_R5G6B5                               (0x00000005)
#define NV055_FORMAT_COLOR_LE_A8R8G8B8                             (0x00000006)
#define NV055_FORMAT_COLOR_LE_X8R8G8B8                             (0x00000007)
#define NV055_FORMAT_MIPMAP_LEVELS                                 15:12
#define NV055_FORMAT_BASE_SIZE_U                                   19:16
#define NV055_FORMAT_BASE_SIZE_U_1                                 (0x00000000)
#define NV055_FORMAT_BASE_SIZE_U_2                                 (0x00000001)
#define NV055_FORMAT_BASE_SIZE_U_4                                 (0x00000002)
#define NV055_FORMAT_BASE_SIZE_U_8                                 (0x00000003)
#define NV055_FORMAT_BASE_SIZE_U_16                                (0x00000004)
#define NV055_FORMAT_BASE_SIZE_U_32                                (0x00000005)
#define NV055_FORMAT_BASE_SIZE_U_64                                (0x00000006)
#define NV055_FORMAT_BASE_SIZE_U_128                               (0x00000007)
#define NV055_FORMAT_BASE_SIZE_U_256                               (0x00000008)
#define NV055_FORMAT_BASE_SIZE_U_512                               (0x00000009)
#define NV055_FORMAT_BASE_SIZE_U_1024                              (0x0000000A)
#define NV055_FORMAT_BASE_SIZE_U_2048                              (0x0000000B)
#define NV055_FORMAT_BASE_SIZE_V                                   23:20
#define NV055_FORMAT_BASE_SIZE_V_1                                 (0x00000000)
#define NV055_FORMAT_BASE_SIZE_V_2                                 (0x00000001)
#define NV055_FORMAT_BASE_SIZE_V_4                                 (0x00000002)
#define NV055_FORMAT_BASE_SIZE_V_8                                 (0x00000003)
#define NV055_FORMAT_BASE_SIZE_V_16                                (0x00000004)
#define NV055_FORMAT_BASE_SIZE_V_32                                (0x00000005)
#define NV055_FORMAT_BASE_SIZE_V_64                                (0x00000006)
#define NV055_FORMAT_BASE_SIZE_V_128                               (0x00000007)
#define NV055_FORMAT_BASE_SIZE_V_256                               (0x00000008)
#define NV055_FORMAT_BASE_SIZE_V_512                               (0x00000009)
#define NV055_FORMAT_BASE_SIZE_V_1024                              (0x0000000A)
#define NV055_FORMAT_BASE_SIZE_V_2048                              (0x0000000B)
#define NV055_FORMAT_TEXTUREADDRESSU                               26:24
#define NV055_FORMAT_TEXTUREADDRESSU_WRAP                          (0x00000001)
#define NV055_FORMAT_TEXTUREADDRESSU_MIRROR                        (0x00000002)
#define NV055_FORMAT_TEXTUREADDRESSU_CLAMP                         (0x00000003)
#define NV055_FORMAT_TEXTUREADDRESSU_BORDER                        (0x00000004)
#define NV055_FORMAT_WRAPU                                         27:27
#define NV055_FORMAT_WRAPU_FALSE                                   (0x00000000)
#define NV055_FORMAT_WRAPU_TRUE                                    (0x00000001)
#define NV055_FORMAT_TEXTUREADDRESSV                               30:28
#define NV055_FORMAT_TEXTUREADDRESSV_WRAP                          (0x00000001)
#define NV055_FORMAT_TEXTUREADDRESSV_MIRROR                        (0x00000002)
#define NV055_FORMAT_TEXTUREADDRESSV_CLAMP                         (0x00000003)
#define NV055_FORMAT_TEXTUREADDRESSV_BORDER                        (0x00000004)
#define NV055_FORMAT_WRAPV                                         31:31
#define NV055_FORMAT_WRAPV_FALSE                                   (0x00000000)
#define NV055_FORMAT_WRAPV_TRUE                                    (0x00000001)
#define NV055_FILTER(b)                                            (0x00000318\
                                                                   +(b)*0x0004)
#define NV055_FILTER_KERNEL_SIZE_X                                 7:0
#define NV055_FILTER_KERNEL_SIZE_Y                                 14:8
#define NV055_FILTER_MIPMAP_DITHER_ENABLE                          15:15
#define NV055_FILTER_MIPMAP_DITHER_ENABLE_FALSE                    (0x00000000)
#define NV055_FILTER_MIPMAP_DITHER_ENABLE_TRUE                     (0x00000001)
#define NV055_FILTER_MIPMAPLODBIAS                                 23:16
#define NV055_FILTER_TEXTUREMIN                                    26:24
#define NV055_FILTER_TEXTUREMIN_NEAREST                            (0x00000001)
#define NV055_FILTER_TEXTUREMIN_LINEAR                             (0x00000002)
#define NV055_FILTER_TEXTUREMIN_MIPNEAREST                         (0x00000003)
#define NV055_FILTER_TEXTUREMIN_MIPLINEAR                          (0x00000004)
#define NV055_FILTER_TEXTUREMIN_LINEARMIPNEAREST                   (0x00000005)
#define NV055_FILTER_TEXTUREMIN_LINEARMIPLINEAR                    (0x00000006)
#define NV055_FILTER_ANISOTROPIC_MINIFY_ENABLE                     27:27
#define NV055_FILTER_ANISOTROPIC_MINIFY_ENABLE_FALSE               (0x00000000)
#define NV055_FILTER_ANISOTROPIC_MINIFY_ENABLE_TRUE                (0x00000001)
#define NV055_FILTER_TEXTUREMAG                                    30:28
#define NV055_FILTER_TEXTUREMAG_NEAREST                            (0x00000001)
#define NV055_FILTER_TEXTUREMAG_LINEAR                             (0x00000002)
#define NV055_FILTER_TEXTUREMAG_MIPNEAREST                         (0x00000003)
#define NV055_FILTER_TEXTUREMAG_MIPLINEAR                          (0x00000004)
#define NV055_FILTER_TEXTUREMAG_LINEARMIPNEAREST                   (0x00000005)
#define NV055_FILTER_TEXTUREMAG_LINEARMIPLINEAR                    (0x00000006)
#define NV055_FILTER_ANISOTROPIC_MAGNIFY_ENABLE                    31:31
#define NV055_FILTER_ANISOTROPIC_MAGNIFY_ENABLE_FALSE              (0x00000000)
#define NV055_FILTER_ANISOTROPIC_MAGNIFY_ENABLE_TRUE               (0x00000001)
#define NV055_COMBINE_0_ALPHA                                      (0x00000320)
#define NV055_COMBINE_0_ALPHA_INVERSE_0                            0:0
#define NV055_COMBINE_0_ALPHA_INVERSE_0_NORMAL                     (0x00000000)
#define NV055_COMBINE_0_ALPHA_INVERSE_0_INVERSE                    (0x00000001)
#define NV055_COMBINE_0_ALPHA_ALPHA_0                              1:1
#define NV055_COMBINE_0_ALPHA_ARGUMENT_0                           7:2
#define NV055_COMBINE_0_ALPHA_ARGUMENT_0_ZERO                      (0x00000001)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_0_FACTOR                    (0x00000002)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_0_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_0_INPUT                     (0x00000004)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_0_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_0_ALPHA_INVERSE_1                            8:8
#define NV055_COMBINE_0_ALPHA_INVERSE_1_NORMAL                     (0x00000000)
#define NV055_COMBINE_0_ALPHA_INVERSE_1_INVERSE                    (0x00000001)
#define NV055_COMBINE_0_ALPHA_ALPHA_1                              9:9
#define NV055_COMBINE_0_ALPHA_ARGUMENT_1                           15:10
#define NV055_COMBINE_0_ALPHA_ARGUMENT_1_ZERO                      (0x00000001)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_1_FACTOR                    (0x00000002)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_1_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_1_INPUT                     (0x00000004)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_1_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_1_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_1_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_0_ALPHA_INVERSE_2                            16:16
#define NV055_COMBINE_0_ALPHA_INVERSE_2_NORMAL                     (0x00000000)
#define NV055_COMBINE_0_ALPHA_INVERSE_2_INVERSE                    (0x00000001)
#define NV055_COMBINE_0_ALPHA_ALPHA_2                              17:17
#define NV055_COMBINE_0_ALPHA_ARGUMENT_2                           23:18
#define NV055_COMBINE_0_ALPHA_ARGUMENT_2_ZERO                      (0x00000001)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_2_FACTOR                    (0x00000002)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_2_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_2_INPUT                     (0x00000004)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_2_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_2_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_2_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_0_ALPHA_INVERSE_3                            24:24
#define NV055_COMBINE_0_ALPHA_INVERSE_3_NORMAL                     (0x00000000)
#define NV055_COMBINE_0_ALPHA_INVERSE_3_INVERSE                    (0x00000001)
#define NV055_COMBINE_0_ALPHA_ALPHA_3                              25:25
#define NV055_COMBINE_0_ALPHA_ARGUMENT_3                           28:26
#define NV055_COMBINE_0_ALPHA_ARGUMENT_3_ZERO                      (0x00000001)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_3_FACTOR                    (0x00000002)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_3_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_3_INPUT                     (0x00000004)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_3_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_3_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_0_ALPHA_ARGUMENT_3_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_0_ALPHA_OPERATION                            31:29
#define NV055_COMBINE_0_ALPHA_OPERATION_ADD                        (0x00000001)
#define NV055_COMBINE_0_ALPHA_OPERATION_ADD2                       (0x00000002)
#define NV055_COMBINE_0_ALPHA_OPERATION_ADD4                       (0x00000003)
#define NV055_COMBINE_0_ALPHA_OPERATION_ADDSIGNED                  (0x00000004)
#define NV055_COMBINE_0_ALPHA_OPERATION_MUX                        (0x00000005)
#define NV055_COMBINE_0_ALPHA_OPERATION_ADDCOMPLEMENT              (0x00000006)
#define NV055_COMBINE_0_ALPHA_OPERATION_ADDSIGNED2                 (0x00000007)
#define NV055_COMBINE_0_COLOR                                      (0x00000324)
#define NV055_COMBINE_0_COLOR_INVERSE_0                            0:0
#define NV055_COMBINE_0_COLOR_INVERSE_0_NORMAL                     (0x00000000)
#define NV055_COMBINE_0_COLOR_INVERSE_0_INVERSE                    (0x00000001)
#define NV055_COMBINE_0_COLOR_ALPHA_0                              1:1
#define NV055_COMBINE_0_COLOR_ALPHA_0_COLOR                        (0x00000000)
#define NV055_COMBINE_0_COLOR_ALPHA_0_ALPHA                        (0x00000001)
#define NV055_COMBINE_0_COLOR_ARGUMENT_0                           7:2
#define NV055_COMBINE_0_COLOR_ARGUMENT_0_ZERO                      (0x00000001)
#define NV055_COMBINE_0_COLOR_ARGUMENT_0_FACTOR                    (0x00000002)
#define NV055_COMBINE_0_COLOR_ARGUMENT_0_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_0_COLOR_ARGUMENT_0_INPUT                     (0x00000004)
#define NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_0_COLOR_ARGUMENT_0_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_0_COLOR_INVERSE_1                            8:8
#define NV055_COMBINE_0_COLOR_INVERSE_1_NORMAL                     (0x00000000)
#define NV055_COMBINE_0_COLOR_INVERSE_1_INVERSE                    (0x00000001)
#define NV055_COMBINE_0_COLOR_ALPHA_1                              9:9
#define NV055_COMBINE_0_COLOR_ALPHA_1_COLOR                        (0x00000000)
#define NV055_COMBINE_0_COLOR_ALPHA_1_ALPHA                        (0x00000001)
#define NV055_COMBINE_0_COLOR_ARGUMENT_1                           15:10
#define NV055_COMBINE_0_COLOR_ARGUMENT_1_ZERO                      (0x00000001)
#define NV055_COMBINE_0_COLOR_ARGUMENT_1_FACTOR                    (0x00000002)
#define NV055_COMBINE_0_COLOR_ARGUMENT_1_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_0_COLOR_ARGUMENT_1_INPUT                     (0x00000004)
#define NV055_COMBINE_0_COLOR_ARGUMENT_1_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_0_COLOR_ARGUMENT_1_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_0_COLOR_ARGUMENT_1_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_0_COLOR_INVERSE_2                            16:16
#define NV055_COMBINE_0_COLOR_INVERSE_2_NORMAL                     (0x00000000)
#define NV055_COMBINE_0_COLOR_INVERSE_2_INVERSE                    (0x00000001)
#define NV055_COMBINE_0_COLOR_ALPHA_2                              17:17
#define NV055_COMBINE_0_COLOR_ALPHA_2_COLOR                        (0x00000000)
#define NV055_COMBINE_0_COLOR_ALPHA_2_ALPHA                        (0x00000001)
#define NV055_COMBINE_0_COLOR_ARGUMENT_2                           23:18
#define NV055_COMBINE_0_COLOR_ARGUMENT_2_ZERO                      (0x00000001)
#define NV055_COMBINE_0_COLOR_ARGUMENT_2_FACTOR                    (0x00000002)
#define NV055_COMBINE_0_COLOR_ARGUMENT_2_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_0_COLOR_ARGUMENT_2_INPUT                     (0x00000004)
#define NV055_COMBINE_0_COLOR_ARGUMENT_2_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_0_COLOR_ARGUMENT_2_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_0_COLOR_ARGUMENT_2_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_0_COLOR_INVERSE_3                            24:24
#define NV055_COMBINE_0_COLOR_INVERSE_3_NORMAL                     (0x00000000)
#define NV055_COMBINE_0_COLOR_INVERSE_3_INVERSE                    (0x00000001)
#define NV055_COMBINE_0_COLOR_ALPHA_3                              25:25
#define NV055_COMBINE_0_COLOR_ALPHA_3_COLOR                        (0x00000000)
#define NV055_COMBINE_0_COLOR_ALPHA_3_ALPHA                        (0x00000001)
#define NV055_COMBINE_0_COLOR_ARGUMENT_3                           28:26
#define NV055_COMBINE_0_COLOR_ARGUMENT_3_ZERO                      (0x00000001)
#define NV055_COMBINE_0_COLOR_ARGUMENT_3_FACTOR                    (0x00000002)
#define NV055_COMBINE_0_COLOR_ARGUMENT_3_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_0_COLOR_ARGUMENT_3_INPUT                     (0x00000004)
#define NV055_COMBINE_0_COLOR_ARGUMENT_3_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_0_COLOR_ARGUMENT_3_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_0_COLOR_ARGUMENT_3_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_0_COLOR_OPERATION                            31:29
#define NV055_COMBINE_0_COLOR_OPERATION_ADD                        (0x00000001)
#define NV055_COMBINE_0_COLOR_OPERATION_ADD2                       (0x00000002)
#define NV055_COMBINE_0_COLOR_OPERATION_ADD4                       (0x00000003)
#define NV055_COMBINE_0_COLOR_OPERATION_ADDSIGNED                  (0x00000004)
#define NV055_COMBINE_0_COLOR_OPERATION_MUX                        (0x00000005)
#define NV055_COMBINE_0_COLOR_OPERATION_ADDCOMPLEMENT              (0x00000006)
#define NV055_COMBINE_0_COLOR_OPERATION_ADDSIGNED2                 (0x00000007)
#define NV055_COMBINE_1_ALPHA                                      (0x0000032C)
#define NV055_COMBINE_1_ALPHA_INVERSE_0                            0:0
#define NV055_COMBINE_1_ALPHA_INVERSE_0_NORMAL                     (0x00000000)
#define NV055_COMBINE_1_ALPHA_INVERSE_0_INVERSE                    (0x00000001)
#define NV055_COMBINE_1_ALPHA_ALPHA_0                              1:1
#define NV055_COMBINE_1_ALPHA_ARGUMENT_0                           7:2
#define NV055_COMBINE_1_ALPHA_ARGUMENT_0_ZERO                      (0x00000001)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_0_FACTOR                    (0x00000002)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_0_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_0_INPUT                     (0x00000004)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_0_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_0_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_0_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_1_ALPHA_INVERSE_1                            8:8
#define NV055_COMBINE_1_ALPHA_INVERSE_1_NORMAL                     (0x00000000)
#define NV055_COMBINE_1_ALPHA_INVERSE_1_INVERSE                    (0x00000001)
#define NV055_COMBINE_1_ALPHA_ALPHA_1                              9:9
#define NV055_COMBINE_1_ALPHA_ARGUMENT_1                           15:10
#define NV055_COMBINE_1_ALPHA_ARGUMENT_1_ZERO                      (0x00000001)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_1_FACTOR                    (0x00000002)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_1_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_1_INPUT                     (0x00000004)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_1_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_1_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_1_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_1_ALPHA_INVERSE_2                            16:16
#define NV055_COMBINE_1_ALPHA_INVERSE_2_NORMAL                     (0x00000000)
#define NV055_COMBINE_1_ALPHA_INVERSE_2_INVERSE                    (0x00000001)
#define NV055_COMBINE_1_ALPHA_ALPHA_2                              17:17
#define NV055_COMBINE_1_ALPHA_ARGUMENT_2                           23:18
#define NV055_COMBINE_1_ALPHA_ARGUMENT_2_ZERO                      (0x00000001)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_2_FACTOR                    (0x00000002)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_2_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_2_INPUT                     (0x00000004)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_2_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_2_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_2_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_1_ALPHA_INVERSE_3                            24:24
#define NV055_COMBINE_1_ALPHA_INVERSE_3_NORMAL                     (0x00000000)
#define NV055_COMBINE_1_ALPHA_INVERSE_3_INVERSE                    (0x00000001)
#define NV055_COMBINE_1_ALPHA_ALPHA_3                              25:25
#define NV055_COMBINE_1_ALPHA_ARGUMENT_3                           28:26
#define NV055_COMBINE_1_ALPHA_ARGUMENT_3_ZERO                      (0x00000001)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_3_FACTOR                    (0x00000002)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_3_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_3_INPUT                     (0x00000004)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_3_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_3_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_1_ALPHA_ARGUMENT_3_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_1_ALPHA_OPERATION                            31:29
#define NV055_COMBINE_1_ALPHA_OPERATION_ADD                        (0x00000001)
#define NV055_COMBINE_1_ALPHA_OPERATION_ADD2                       (0x00000002)
#define NV055_COMBINE_1_ALPHA_OPERATION_ADD4                       (0x00000003)
#define NV055_COMBINE_1_ALPHA_OPERATION_ADDSIGNED                  (0x00000004)
#define NV055_COMBINE_1_ALPHA_OPERATION_MUX                        (0x00000005)
#define NV055_COMBINE_1_ALPHA_OPERATION_ADDCOMPLEMENT              (0x00000006)
#define NV055_COMBINE_1_ALPHA_OPERATION_ADDSIGNED2                 (0x00000007)
#define NV055_COMBINE_1_COLOR                                      (0x00000330)
#define NV055_COMBINE_1_COLOR_INVERSE_0                            0:0
#define NV055_COMBINE_1_COLOR_INVERSE_0_NORMAL                     (0x00000000)
#define NV055_COMBINE_1_COLOR_INVERSE_0_INVERSE                    (0x00000001)
#define NV055_COMBINE_1_COLOR_ALPHA_0                              1:1
#define NV055_COMBINE_1_COLOR_ALPHA_0_COLOR                        (0x00000000)
#define NV055_COMBINE_1_COLOR_ALPHA_0_ALPHA                        (0x00000001)
#define NV055_COMBINE_1_COLOR_ARGUMENT_0                           7:2
#define NV055_COMBINE_1_COLOR_ARGUMENT_0_ZERO                      (0x00000001)
#define NV055_COMBINE_1_COLOR_ARGUMENT_0_FACTOR                    (0x00000002)
#define NV055_COMBINE_1_COLOR_ARGUMENT_0_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_1_COLOR_ARGUMENT_0_INPUT                     (0x00000004)
#define NV055_COMBINE_1_COLOR_ARGUMENT_0_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_1_COLOR_ARGUMENT_0_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_1_COLOR_ARGUMENT_0_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_1_COLOR_INVERSE_1                            8:8
#define NV055_COMBINE_1_COLOR_INVERSE_1_NORMAL                     (0x00000000)
#define NV055_COMBINE_1_COLOR_INVERSE_1_INVERSE                    (0x00000001)
#define NV055_COMBINE_1_COLOR_ALPHA_1                              9:9
#define NV055_COMBINE_1_COLOR_ALPHA_1_COLOR                        (0x00000000)
#define NV055_COMBINE_1_COLOR_ALPHA_1_ALPHA                        (0x00000001)
#define NV055_COMBINE_1_COLOR_ARGUMENT_1                           15:10
#define NV055_COMBINE_1_COLOR_ARGUMENT_1_ZERO                      (0x00000001)
#define NV055_COMBINE_1_COLOR_ARGUMENT_1_FACTOR                    (0x00000002)
#define NV055_COMBINE_1_COLOR_ARGUMENT_1_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_1_COLOR_ARGUMENT_1_INPUT                     (0x00000004)
#define NV055_COMBINE_1_COLOR_ARGUMENT_1_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_1_COLOR_ARGUMENT_1_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_1_COLOR_ARGUMENT_1_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_1_COLOR_INVERSE_2                            16:16
#define NV055_COMBINE_1_COLOR_INVERSE_2_NORMAL                     (0x00000000)
#define NV055_COMBINE_1_COLOR_INVERSE_2_INVERSE                    (0x00000001)
#define NV055_COMBINE_1_COLOR_ALPHA_2                              17:17
#define NV055_COMBINE_1_COLOR_ALPHA_2_COLOR                        (0x00000000)
#define NV055_COMBINE_1_COLOR_ALPHA_2_ALPHA                        (0x00000001)
#define NV055_COMBINE_1_COLOR_ARGUMENT_2                           23:18
#define NV055_COMBINE_1_COLOR_ARGUMENT_2_ZERO                      (0x00000001)
#define NV055_COMBINE_1_COLOR_ARGUMENT_2_FACTOR                    (0x00000002)
#define NV055_COMBINE_1_COLOR_ARGUMENT_2_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_1_COLOR_ARGUMENT_2_INPUT                     (0x00000004)
#define NV055_COMBINE_1_COLOR_ARGUMENT_2_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_1_COLOR_ARGUMENT_2_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_1_COLOR_ARGUMENT_2_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_1_COLOR_INVERSE_3                            24:24
#define NV055_COMBINE_1_COLOR_INVERSE_3_NORMAL                     (0x00000000)
#define NV055_COMBINE_1_COLOR_INVERSE_3_INVERSE                    (0x00000001)
#define NV055_COMBINE_1_COLOR_ALPHA_3                              25:25
#define NV055_COMBINE_1_COLOR_ALPHA_3_COLOR                        (0x00000000)
#define NV055_COMBINE_1_COLOR_ALPHA_3_ALPHA                        (0x00000001)
#define NV055_COMBINE_1_COLOR_ARGUMENT_3                           28:26
#define NV055_COMBINE_1_COLOR_ARGUMENT_3_ZERO                      (0x00000001)
#define NV055_COMBINE_1_COLOR_ARGUMENT_3_FACTOR                    (0x00000002)
#define NV055_COMBINE_1_COLOR_ARGUMENT_3_DIFFUSE                   (0x00000003)
#define NV055_COMBINE_1_COLOR_ARGUMENT_3_INPUT                     (0x00000004)
#define NV055_COMBINE_1_COLOR_ARGUMENT_3_TEXTURE0                  (0x00000005)
#define NV055_COMBINE_1_COLOR_ARGUMENT_3_TEXTURE1                  (0x00000006)
#define NV055_COMBINE_1_COLOR_ARGUMENT_3_TEXTURELOD                (0x00000007)
#define NV055_COMBINE_1_COLOR_OPERATION                            31:29
#define NV055_COMBINE_1_COLOR_OPERATION_ADD                        (0x00000001)
#define NV055_COMBINE_1_COLOR_OPERATION_ADD2                       (0x00000002)
#define NV055_COMBINE_1_COLOR_OPERATION_ADD4                       (0x00000003)
#define NV055_COMBINE_1_COLOR_OPERATION_ADDSIGNED                  (0x00000004)
#define NV055_COMBINE_1_COLOR_OPERATION_MUX                        (0x00000005)
#define NV055_COMBINE_1_COLOR_OPERATION_ADDCOMPLEMENT              (0x00000006)
#define NV055_COMBINE_1_COLOR_OPERATION_ADDSIGNED2                 (0x00000007)
#define NV055_COMBINE_FACTOR                                       (0x00000334)
#define NV055_COMBINE_FACTOR_BLUE                                  7:0
#define NV055_COMBINE_FACTOR_GREEN                                 15:8
#define NV055_COMBINE_FACTOR_RED                                   23:16
#define NV055_COMBINE_FACTOR_ALPHA                                 31:24
#define NV055_BLEND                                                (0x00000338)
#define NV055_BLEND_MASK_BIT                                       5:0
#define NV055_BLEND_MASK_BIT_LSB                                   (0x00000010)
#define NV055_BLEND_MASK_BIT_MSB                                   (0x00000020)
#define NV055_BLEND_SHADEMODE                                      7:6
#define NV055_BLEND_SHADEMODE_FLAT                                 (0x00000001)
#define NV055_BLEND_SHADEMODE_GOURAUD                              (0x00000002)
#define NV055_BLEND_SHADEMODE_PHONG                                (0x00000003)
#define NV055_BLEND_TEXTUREPERSPECTIVE                             11:8
#define NV055_BLEND_TEXTUREPERSPECTIVE_FALSE                       (0x00000000)
#define NV055_BLEND_TEXTUREPERSPECTIVE_TRUE                        (0x00000001)
#define NV055_BLEND_SPECULARENABLE                                 15:12
#define NV055_BLEND_SPECULARENABLE_FALSE                           (0x00000000)
#define NV055_BLEND_SPECULARENABLE_TRUE                            (0x00000001)
#define NV055_BLEND_FOGENABLE                                      19:16
#define NV055_BLEND_FOGENABLE_FALSE                                (0x00000000)
#define NV055_BLEND_FOGENABLE_TRUE                                 (0x00000001)
#define NV055_BLEND_ALPHABLENDENABLE                               23:20
#define NV055_BLEND_ALPHABLENDENABLE_FALSE                         (0x00000000)
#define NV055_BLEND_ALPHABLENDENABLE_TRUE                          (0x00000001)
#define NV055_BLEND_SRCBLEND                                       27:24
#define NV055_BLEND_SRCBLEND_ZERO                                  (0x00000001)
#define NV055_BLEND_SRCBLEND_ONE                                   (0x00000002)
#define NV055_BLEND_SRCBLEND_SRCCOLOR                              (0x00000003)
#define NV055_BLEND_SRCBLEND_INVSRCCOLOR                           (0x00000004)
#define NV055_BLEND_SRCBLEND_SRCALPHA                              (0x00000005)
#define NV055_BLEND_SRCBLEND_INVSRCALPHA                           (0x00000006)
#define NV055_BLEND_SRCBLEND_DESTALPHA                             (0x00000007)
#define NV055_BLEND_SRCBLEND_INVDESTALPHA                          (0x00000008)
#define NV055_BLEND_SRCBLEND_DESTCOLOR                             (0x00000009)
#define NV055_BLEND_SRCBLEND_INVDESTCOLOR                          (0x0000000A)
#define NV055_BLEND_SRCBLEND_SRCALPHASAT                           (0x0000000B)
#define NV055_BLEND_DESTBLEND                                      31:28
#define NV055_BLEND_DESTBLEND_ZERO                                 (0x00000001)
#define NV055_BLEND_DESTBLEND_ONE                                  (0x00000002)
#define NV055_BLEND_DESTBLEND_SRCCOLOR                             (0x00000003)
#define NV055_BLEND_DESTBLEND_INVSRCCOLOR                          (0x00000004)
#define NV055_BLEND_DESTBLEND_SRCALPHA                             (0x00000005)
#define NV055_BLEND_DESTBLEND_INVSRCALPHA                          (0x00000006)
#define NV055_BLEND_DESTBLEND_DESTALPHA                            (0x00000007)
#define NV055_BLEND_DESTBLEND_INVDESTALPHA                         (0x00000008)
#define NV055_BLEND_DESTBLEND_DESTCOLOR                            (0x00000009)
#define NV055_BLEND_DESTBLEND_INVDESTCOLOR                         (0x0000000A)
#define NV055_BLEND_DESTBLEND_SRCALPHASAT                          (0x0000000B)
#define NV055_CONTROL0                                             (0x0000033C)
#define NV055_CONTROL0_ALPHAREF                                    7:0
#define NV055_CONTROL0_ALPHAFUNC                                   11:8
#define NV055_CONTROL0_ALPHAFUNC_NEVER                             (0x00000001)
#define NV055_CONTROL0_ALPHAFUNC_LESS                              (0x00000002)
#define NV055_CONTROL0_ALPHAFUNC_EQUAL                             (0x00000003)
#define NV055_CONTROL0_ALPHAFUNC_LESSEQUAL                         (0x00000004)
#define NV055_CONTROL0_ALPHAFUNC_GREATER                           (0x00000005)
#define NV055_CONTROL0_ALPHAFUNC_NOTEQUAL                          (0x00000006)
#define NV055_CONTROL0_ALPHAFUNC_GREATEREQUAL                      (0x00000007)
#define NV055_CONTROL0_ALPHAFUNC_ALWAYS                            (0x00000008)
#define NV055_CONTROL0_ALPHATESTENABLE                             12:12
#define NV055_CONTROL0_ALPHATESTENABLE_FALSE                       (0x00000000)
#define NV055_CONTROL0_ALPHATESTENABLE_TRUE                        (0x00000001)
#define NV055_CONTROL0_ORIGIN                                      13:13
#define NV055_CONTROL0_ORIGIN_CENTER                               (0x00000000)
#define NV055_CONTROL0_ORIGIN_CORNER                               (0x00000001)
#define NV055_CONTROL0_ZENABLE                                     15:14
#define NV055_CONTROL0_ZENABLE_FALSE                               (0x00000000)
#define NV055_CONTROL0_ZENABLE_TRUE                                (0x00000001)
#define NV055_CONTROL0_ZFUNC                                       19:16
#define NV055_CONTROL0_ZFUNC_NEVER                                 (0x00000001)
#define NV055_CONTROL0_ZFUNC_LESS                                  (0x00000002)
#define NV055_CONTROL0_ZFUNC_EQUAL                                 (0x00000003)
#define NV055_CONTROL0_ZFUNC_LESSEQUAL                             (0x00000004)
#define NV055_CONTROL0_ZFUNC_GREATER                               (0x00000005)
#define NV055_CONTROL0_ZFUNC_NOTEQUAL                              (0x00000006)
#define NV055_CONTROL0_ZFUNC_GREATEREQUAL                          (0x00000007)
#define NV055_CONTROL0_ZFUNC_ALWAYS                                (0x00000008)
#define NV055_CONTROL0_CULLMODE                                    21:20
#define NV055_CONTROL0_CULLMODE_NONE                               (0x00000001)
#define NV055_CONTROL0_CULLMODE_CW                                 (0x00000002)
#define NV055_CONTROL0_CULLMODE_CCW                                (0x00000003)
#define NV055_CONTROL0_DITHERENABLE                                22:22
#define NV055_CONTROL0_DITHERENABLE_FALSE                          (0x00000000)
#define NV055_CONTROL0_DITHERENABLE_TRUE                           (0x00000001)
#define NV055_CONTROL0_Z_PERSPECTIVE_ENABLE                        23:23
#define NV055_CONTROL0_Z_PERSPECTIVE_ENABLE_FALSE                  (0x00000000)
#define NV055_CONTROL0_Z_PERSPECTIVE_ENABLE_TRUE                   (0x00000001)
#define NV055_CONTROL0_ZWRITEENABLE                                24:24
#define NV055_CONTROL0_ZWRITEENABLE_FALSE                          (0x00000000)
#define NV055_CONTROL0_ZWRITEENABLE_TRUE                           (0x00000001)
#define NV055_CONTROL0_STENCIL_WRITE_ENABLE                        25:25
#define NV055_CONTROL0_STENCIL_WRITE_ENABLE_FALSE                  (0x00000000)
#define NV055_CONTROL0_STENCIL_WRITE_ENABLE_TRUE                   (0x00000001)
#define NV055_CONTROL0_ALPHA_WRITE_ENABLE                          26:26
#define NV055_CONTROL0_ALPHA_WRITE_ENABLE_FALSE                    (0x00000000)
#define NV055_CONTROL0_ALPHA_WRITE_ENABLE_TRUE                     (0x00000001)
#define NV055_CONTROL0_RED_WRITE_ENABLE                            27:27
#define NV055_CONTROL0_RED_WRITE_ENABLE_FALSE                      (0x00000000)
#define NV055_CONTROL0_RED_WRITE_ENABLE_TRUE                       (0x00000001)
#define NV055_CONTROL0_GREEN_WRITE_ENABLE                          28:28
#define NV055_CONTROL0_GREEN_WRITE_ENABLE_FALSE                    (0x00000000)
#define NV055_CONTROL0_GREEN_WRITE_ENABLE_TRUE                     (0x00000001)
#define NV055_CONTROL0_BLUE_WRITE_ENABLE                           29:29
#define NV055_CONTROL0_BLUE_WRITE_ENABLE_FALSE                     (0x00000000)
#define NV055_CONTROL0_BLUE_WRITE_ENABLE_TRUE                      (0x00000001)
#define NV055_CONTROL0_Z_FORMAT                                    31:30
#define NV055_CONTROL0_Z_FORMAT_FIXED                              (0x00000001)
#define NV055_CONTROL0_Z_FORMAT_FLOAT                              (0x00000002)
#define NV055_CONTROL1                                             (0x00000340)
#define NV055_CONTROL1_STENCIL_TEST_ENABLE                         3:0
#define NV055_CONTROL1_STENCIL_TEST_ENABLE_FALSE                   (0x00000000)
#define NV055_CONTROL1_STENCIL_TEST_ENABLE_TRUE                    (0x00000001)
#define NV055_CONTROL1_STENCIL_FUNC                                7:4
#define NV055_CONTROL1_STENCIL_FUNC_NEVER                          (0x00000001)
#define NV055_CONTROL1_STENCIL_FUNC_LESS                           (0x00000002)
#define NV055_CONTROL1_STENCIL_FUNC_EQUAL                          (0x00000003)
#define NV055_CONTROL1_STENCIL_FUNC_LESSEQUAL                      (0x00000004)
#define NV055_CONTROL1_STENCIL_FUNC_GREATER                        (0x00000005)
#define NV055_CONTROL1_STENCIL_FUNC_NOTEQUAL                       (0x00000006)
#define NV055_CONTROL1_STENCIL_FUNC_GREATEREQUAL                   (0x00000007)
#define NV055_CONTROL1_STENCIL_FUNC_ALWAYS                         (0x00000008)
#define NV055_CONTROL1_STENCIL_REF                                 15:8
#define NV055_CONTROL1_STENCIL_MASK_READ                           23:16
#define NV055_CONTROL1_STENCIL_MASK_WRITE                          31:24
#define NV055_CONTROL2                                             (0x00000344)
#define NV055_CONTROL2_STENCIL_OP_FAIL                             3:0
#define NV055_CONTROL2_STENCIL_OP_FAIL_KEEP                        (0x00000001)
#define NV055_CONTROL2_STENCIL_OP_FAIL_ZERO                        (0x00000002)
#define NV055_CONTROL2_STENCIL_OP_FAIL_REPLACE                     (0x00000003)
#define NV055_CONTROL2_STENCIL_OP_FAIL_INCRSAT                     (0x00000004)
#define NV055_CONTROL2_STENCIL_OP_FAIL_DECRSAT                     (0x00000005)
#define NV055_CONTROL2_STENCIL_OP_FAIL_INVERT                      (0x00000006)
#define NV055_CONTROL2_STENCIL_OP_FAIL_INCR                        (0x00000007)
#define NV055_CONTROL2_STENCIL_OP_FAIL_DECR                        (0x00000008)
#define NV055_CONTROL2_STENCIL_OP_ZFAIL                            7:4
#define NV055_CONTROL2_STENCIL_OP_ZFAIL_KEEP                       (0x00000001)
#define NV055_CONTROL2_STENCIL_OP_ZFAIL_ZERO                       (0x00000002)
#define NV055_CONTROL2_STENCIL_OP_ZFAIL_REPLACE                    (0x00000003)
#define NV055_CONTROL2_STENCIL_OP_ZFAIL_INCRSAT                    (0x00000004)
#define NV055_CONTROL2_STENCIL_OP_ZFAIL_DECRSAT                    (0x00000005)
#define NV055_CONTROL2_STENCIL_OP_ZFAIL_INVERT                     (0x00000006)
#define NV055_CONTROL2_STENCIL_OP_ZFAIL_INCR                       (0x00000007)
#define NV055_CONTROL2_STENCIL_OP_ZFAIL_DECR                       (0x00000008)
#define NV055_CONTROL2_STENCIL_OP_ZPASS                            31:8
#define NV055_CONTROL2_STENCIL_OP_ZPASS_KEEP                       (0x00000001)
#define NV055_CONTROL2_STENCIL_OP_ZPASS_ZERO                       (0x00000002)
#define NV055_CONTROL2_STENCIL_OP_ZPASS_REPLACE                    (0x00000003)
#define NV055_CONTROL2_STENCIL_OP_ZPASS_INCRSAT                    (0x00000004)
#define NV055_CONTROL2_STENCIL_OP_ZPASS_DECRSAT                    (0x00000005)
#define NV055_CONTROL2_STENCIL_OP_ZPASS_INVERT                     (0x00000006)
#define NV055_CONTROL2_STENCIL_OP_ZPASS_INCR                       (0x00000007)
#define NV055_CONTROL2_STENCIL_OP_ZPASS_DECR                       (0x00000008)
#define NV055_FOG_COLOR                                            (0x00000348)
#define NV055_TLMTVERTEX(i)                                        (0x00000400\
                                                                   +(i)*0x0028)
#define NV055_TLMTVERTEX_SX(i)                                     (0x00000400\
                                                                   +(i)*0x0028)
#define NV055_TLMTVERTEX_SY(i)                                     (0x00000404\
                                                                   +(i)*0x0028)
#define NV055_TLMTVERTEX_SZ(i)                                     (0x00000408\
                                                                   +(i)*0x0028)
#define NV055_TLMTVERTEX_RHW(i)                                    (0x0000040C\
                                                                   +(i)*0x0028)
#define NV055_TLMTVERTEX_COLOR(i)                                  (0x00000410\
                                                                   +(i)*0x0028)
#define NV055_TLMTVERTEX_COLOR_BLUE                                7:0
#define NV055_TLMTVERTEX_COLOR_GREEN                               15:8
#define NV055_TLMTVERTEX_COLOR_RED                                 23:16
#define NV055_TLMTVERTEX_COLOR_ALPHA                               31:24
#define NV055_TLMTVERTEX_SPECULAR(i)                               (0x00000414\
                                                                   +(i)*0x0028)
#define NV055_TLMTVERTEX_SPECULAR_BLUE                             7:0
#define NV055_TLMTVERTEX_SPECULAR_GREEN                            15:8
#define NV055_TLMTVERTEX_SPECULAR_RED                              23:16
#define NV055_TLMTVERTEX_SPECULAR_FOG                              31:24
#define NV055_TLMTVERTEX_TU0(i)                                    (0x00000418\
                                                                   +(i)*0x0028)
#define NV055_TLMTVERTEX_TV0(i)                                    (0x0000041C\
                                                                   +(i)*0x0028)
#define NV055_TLMTVERTEX_TU1(i)                                    (0x00000420\
                                                                   +(i)*0x0028)
#define NV055_TLMTVERTEX_TV1(i)                                    (0x00000424\
                                                                   +(i)*0x0028)
#define NV055_DRAW_PRIMITIVE(a)                                    (0x00000540\
                                                                   +(a)*0x0004)
#define NV055_DRAW_PRIMITIVE_I0                                    3:0
#define NV055_DRAW_PRIMITIVE_I1                                    7:4
#define NV055_DRAW_PRIMITIVE_I2                                    11:8
#define NV055_DRAW_PRIMITIVE_I3                                    15:12
#define NV055_DRAW_PRIMITIVE_I4                                    19:16
#define NV055_DRAW_PRIMITIVE_I5                                    31:20

typedef V032 Nv056Typedef;


/* class NV04_CONTEXT_COLOR_KEY */
#define  NV04_CONTEXT_COLOR_KEY                                    (0x00000057)
#define  NV4_CONTEXT_COLOR_KEY                                     (0x00000057)
/* NvNotification[] elements */
#define NV057_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV057_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV057_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV057_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV057_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV057_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV057_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV057_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 Reserved01[0x05f];
 V032 SetColorFormat;          /* NV057_SET_COLOR_FORMAT_*          0300-0303*/
 V032 SetColor;                /* source color                      0304-0307*/
 V032 Reserved02[0x73e];
} Nv057Typedef, Nv04ContextColorKey;
#define Nv4ContextColorKey                                  Nv04ContextColorKey
#define nv4ContextColorKey                                  nv04ContextColorKey
#define NV057_TYPEDEF                                       nv04ContextColorKey
/* dma method offsets, fields, and values */
#define NV057_SET_OBJECT                                           (0x00000000)
#define NV057_NO_OPERATION                                         (0x00000100)
#define NV057_NOTIFY                                               (0x00000104)
#define NV057_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV057_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV057_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV057_SET_COLOR_FORMAT                                     (0x00000300)
#define NV057_SET_COLOR_FORMAT_LE_A16R5G6B5                        (0x00000001)
#define NV057_SET_COLOR_FORMAT_LE_X16A1R5G5B5                      (0x00000002)
#define NV057_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000003)
#define NV057_SET_COLOR                                            (0x00000304)


/* class NV03_CONTEXT_SURFACE_0 */
#define  NV03_CONTEXT_SURFACE_0                                    (0x00000058)
#define  NV3_CONTEXT_SURFACE_0                                     (0x00000058)
/* NvNotification[] elements */
#define NV058_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV058_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV058_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV058_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV058_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV058_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV058_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV058_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaImage;      /* NV01_CONTEXT_DMA_IN_MEMORY        0184-0187*/
 V032 Reserved01[0x05e];
 V032 SetColorFormat;          /* NV058_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Reserved02[0x001];
 U032 SetImagePitch;           /* bytes, vertical pixel delta       0308-030b*/
 U032 SetImageOffset;          /* byte offset of top-left pixel     030c-030f*/
 V032 Reserved03[0x73c];
} Nv058Typedef, Nv03ContextSurface0;
#define Nv3ContextSurface0                                  Nv03ContextSurface0
#define nv3ContextSurface0                                  nv03ContextSurface0
#define NV058_TYPEDEF                                       nv03ContextSurface0
/* dma method offsets, fields, and values */
#define NV058_SET_OBJECT                                           (0x00000000)
#define NV058_NO_OPERATION                                         (0x00000100)
#define NV058_NOTIFY                                               (0x00000104)
#define NV058_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV058_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
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
#define  NV03_CONTEXT_SURFACE_1                                    (0x00000059)
#define  NV3_CONTEXT_SURFACE_1                                     (0x00000059)
/* NvNotification[] elements */
#define NV059_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV059_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV059_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV059_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV059_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV059_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV059_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV059_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaImage;      /* NV01_CONTEXT_DMA_IN_MEMORY        0184-0187*/
 V032 Reserved01[0x05e];
 V032 SetColorFormat;          /* NV059_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Reserved02[0x001];
 U032 SetImagePitch;           /* bytes, vertical pixel delta       0308-030b*/
 U032 SetImageOffset;          /* byte offset of top-left pixel     030c-030f*/
 V032 Reserved03[0x73c];
} Nv059Typedef, Nv03ContextSurface1;
#define Nv3ContextSurface1                                  Nv03ContextSurface1
#define nv3ContextSurface1                                  nv03ContextSurface1
#define NV059_TYPEDEF                                       nv03ContextSurface1
/* dma method offsets, fields, and values */
#define NV059_SET_OBJECT                                           (0x00000000)
#define NV059_NO_OPERATION                                         (0x00000100)
#define NV059_NOTIFY                                               (0x00000104)
#define NV059_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV059_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV059_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV059_SET_CONTEXT_DMA_IMAGE                                (0x00000184)
#define NV059_SET_COLOR_FORMAT                                     (0x00000300)
#define NV059_SET_COLOR_FORMAT_LE_Y8                               (0x01010000)
#define NV059_SET_COLOR_FORMAT_LE_Y16                              (0x01010001)
#define NV059_SET_COLOR_FORMAT_LE_X1R5G5B5_Z1R5G5B5                (0x01000000)
#define NV059_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8                (0x00000001)
#define NV059_SET_IMAGE_PITCH                                      (0x00000308)
#define NV059_SET_IMAGE_OFFSET                                     (0x0000030C)


/* class NV03_CONTEXT_SURFACE_2 */
#define  NV03_CONTEXT_SURFACE_2                                    (0x0000005A)
#define  NV3_CONTEXT_SURFACE_2                                     (0x0000005A)
/* NvNotification[] elements */
#define NV05A_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV05A_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV05A_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV05A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV05A_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV05A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV05A_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV05A_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaImage;      /* NV01_CONTEXT_DMA_IN_MEMORY        0184-0187*/
 V032 Reserved01[0x05e];
 V032 SetColorFormat;          /* NV05A_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Reserved02[0x001];
 U032 SetImagePitch;           /* bytes, vertical pixel delta       0308-030b*/
 U032 SetImageOffset;          /* byte offset of top-left pixel     030c-030f*/
 V032 Reserved03[0x73c];
} Nv05aTypedef, Nv03ContextSurface2;
#define Nv3ContextSurface2                               Nv03ContextSurface2
#define nv3ContextSurface2                               nv03ContextSurface2
#define NV05A_TYPEDEF                                    nv03ContextSurface2
/* dma method offsets, fields, and values */
#define NV05A_SET_OBJECT                                           (0x00000000)
#define NV05A_NO_OPERATION                                         (0x00000100)
#define NV05A_NOTIFY                                               (0x00000104)
#define NV05A_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV05A_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV05A_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV05A_SET_CONTEXT_DMA_IMAGE                                (0x00000184)
#define NV05A_SET_COLOR_FORMAT                                     (0x00000300)
#define NV05A_SET_COLOR_FORMAT_DUMMY_0                             (0x01010000)
#define NV05A_SET_COLOR_FORMAT_LE_X1R5G5B5_Z1R5G5B5_1              (0x01010001)
#define NV05A_SET_COLOR_FORMAT_LE_X1R5G5B5_Z1R5G5B5_2              (0x01000000)
#define NV05A_SET_COLOR_FORMAT_DUMMY_3                             (0x00000001)
#define NV05A_SET_IMAGE_PITCH                                      (0x00000308)
#define NV05A_SET_IMAGE_OFFSET                                     (0x0000030C)


/* class NV03_CONTEXT_SURFACE_3 */
#define  NV03_CONTEXT_SURFACE_3                                    (0x0000005B)
#define  NV3_CONTEXT_SURFACE_3                                     (0x0000005B)
/* NvNotification[] elements */
#define NV05B_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV05B_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV05B_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV05B_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV05B_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV05B_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV05B_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV05B_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaImage;      /* NV01_CONTEXT_DMA_IN_MEMORY        0184-0187*/
 V032 Reserved01[0x05e];
 V032 SetColorFormat;          /* NV05B_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Reserved02[0x001];
 U032 SetImagePitch;           /* bytes, vertical pixel delta       0308-030b*/
 U032 SetImageOffset;          /* byte offset of top-left pixel     030c-030f*/
 V032 Reserved03[0x73c];
} Nv05bTypedef, Nv03ContextSurface3;
#define Nv3ContextSurface3                                  Nv03ContextSurface3
#define nv3ContextSurface3                                  nv03ContextSurface3
#define NV05B_TYPEDEF                                       nv03ContextSurface3
/* dma method offsets, fields, and values */
#define NV05B_SET_OBJECT                                           (0x00000000)
#define NV05B_NO_OPERATION                                         (0x00000100)
#define NV05B_NOTIFY                                               (0x00000104)
#define NV05B_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV05B_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV05B_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV05B_SET_CONTEXT_DMA_IMAGE                                (0x00000184)
#define NV05B_SET_COLOR_FORMAT                                     (0x00000300)
#define NV05B_SET_COLOR_FORMAT_DUMMY_0                             (0x01010000)
#define NV05B_SET_COLOR_FORMAT_DUMMY_1                             (0x01010001)
#define NV05B_SET_COLOR_FORMAT_DUMMY_2                             (0x01000000)
#define NV05B_SET_COLOR_FORMAT_DUMMY_3                             (0x00000001)
#define NV05B_SET_IMAGE_PITCH                                      (0x00000308)
#define NV05B_SET_IMAGE_OFFSET                                     (0x0000030C)


/* class NV04_RENDER_SOLID_LIN */
#define  NV04_RENDER_SOLID_LIN                                     (0x0000005C)
#define  NV4_RENDER_SOLID_LIN                                      (0x0000005C)
/* NvNotification[] elements */
#define NV05C_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV05C_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV05C_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV05C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV05C_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV05C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV05C_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV05C_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextClipRectangle; /* NV01_IMAGE_BLACK_RECTANGLE        0184-0187*/
 V032 SetContextPattern;       /* NV04_CONTEXT_PATTERN              0188-018b*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  018c-018f*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0190-0193*/
 V032 SetContextBeta4;         /* NV04_CONTEXT_BETA                 0194-0197*/
 V032 SetContextSurface;       /* NV04_CONTEXT_SURFACES_2D          0198-019b*/
 V032 Reserved01[0x058];
 V032 SetOperation;            /* NV05C_SET_OPERATION_*             02fc-02ff*/
 V032 SetColorFormat;          /* NV05C_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Color;                   /* source color                      0304-0307*/
 V032 Reserved02[0x03e];
 struct {                      /* start of aliased methods in array 0400-    */
  V032 point0;                 /* y_x S16_S16 in pixels             0400-0403*/
  V032 point1;                 /* y_x S16_S16 in pixels             0404-0407*/
 } Lin[16];                    /* end of aliased methods in array       -047f*/
 struct {                      /* start of aliased methods in array 0480-    */
  S032 point0X;                /* in pixels, 0 at left                 0-   3*/
  S032 point0Y;                /* in pixels, 0 at top                  4-   7*/
  S032 point1X;                /* in pixels, 0 at left                 8-   b*/
  S032 point1Y;                /* in pixels, 0 at top                  c-   f*/
 } Lin32[8];                   /* end of aliased methods in array       -04ff*/
 V032 PolyLin[32];             /* y_x S16_S16 in pixels             0500-057f*/
 struct {                      /* start of method in array          0580-    */
  S032 x;                      /* in pixels, 0 at left                 0-   3*/
  S032 y;                      /* in pixels, 0 at top                  4-   7*/
 } PolyLin32[16];              /* end of aliased methods in array       -05ff*/
 struct {                      /* start of aliased methods in array 0600-    */
  V032 color;                  /* source color                         0-   3*/
  V032 point;                  /* y_x S16_S16 in pixels                4-   7*/
 } ColorPolyLin[16];           /* end of aliased methods in array       -067f*/
 V032 Reserved03[0x660];
} Nv05cTypedef, Nv04RenderSolidLin;
#define Nv4RenderSolidLin                                    Nv04RenderSolidLin
#define nv4RenderSolidLin                                    nv04RenderSolidLin
#define NV05C_TYPEDEF                                        nv04RenderSolidLin
/* dma method offsets, fields, and values */
#define NV05C_SET_OBJECT                                           (0x00000000)
#define NV05C_NO_OPERATION                                         (0x00000100)
#define NV05C_NOTIFY                                               (0x00000104)
#define NV05C_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV05C_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV05C_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV05C_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000184)
#define NV05C_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV05C_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV05C_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV05C_SET_CONTEXT_BETA4                                    (0x00000194)
#define NV05C_SET_CONTEXT_SURFACE                                  (0x00000198)
#define NV05C_SET_OPERATION                                        (0x000002FC)
#define NV05C_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV05C_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV05C_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV05C_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV05C_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV05C_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV05C_SET_COLOR_FORMAT                                     (0x00000300)
#define NV05C_SET_COLOR_FORMAT_LE_X16R5G6B5                        (0x00000001)
#define NV05C_SET_COLOR_FORMAT_LE_X17R5G5B5                        (0x00000002)
#define NV05C_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000003)
#define NV05C_COLOR                                                (0x00000304)
#define NV05C_LIN(a)                                               (0x00000400\
                                                                   +(a)*0x0008)
#define NV05C_LIN_POINT0                                           (0x00000400\
                                                                   +(a)*0x0008)
#define NV05C_LIN_POINT0_X                                         15:0
#define NV05C_LIN_POINT0_Y                                         31:16
#define NV05C_LIN_POINT1                                           (0x00000404\
                                                                   +(a)*0x0008)
#define NV05C_LIN_POINT1_X                                         15:0
#define NV05C_LIN_POINT1_Y                                         31:16
#define NV05C_LIN32(a)                                             (0x00000480\
                                                                   +(a)*0x0010)
#define NV05C_LIN32_POINT0_X                                       (0x00000480\
                                                                   +(a)*0x0010)
#define NV05C_LIN32_POINT0_Y                                       (0x00000484\
                                                                   +(a)*0x0010)
#define NV05C_LIN32_POINT1_X                                       (0x00000488\
                                                                   +(a)*0x0010)
#define NV05C_LIN32_POINT1_Y                                       (0x0000048C\
                                                                   +(a)*0x0010)
#define NV05C_POLY_LIN(a)                                          (0x00000500\
                                                                   +(a)*0x0004)
#define NV05C_POLY_LIN_X                                           15:0
#define NV05C_POLY_LIN_Y                                           31:16
#define NV05C_POLY_LIN32(a)                                        (0x00000580\
                                                                   +(a)*0x0008)
#define NV05C_POLY_LIN32_X                                         (0x00000580\
                                                                   +(a)*0x0008)
#define NV05C_POLY_LIN32_Y                                         (0x00000584\
                                                                   +(a)*0x0008)
#define NV05C_COLOR_POLY_LIN(a)                                    (0x00000600\
                                                                   +(a)*0x0008)
#define NV05C_COLOR_POLY_LIN_COLOR                                 (0x00000600\
                                                                   +(a)*0x0008)
#define NV05C_COLOR_POLY_LIN_POINT                                 (0x00000604\
                                                                   +(a)*0x0008)
#define NV05C_COLOR_POLY_LIN_POINT_X                               15:0
#define NV05C_COLOR_POLY_LIN_POINT_Y                               31:16


/* class NV04_RENDER_SOLID_TRIANGLE */
#define  NV04_RENDER_SOLID_TRIANGLE                                (0x0000005D)
#define  NV4_RENDER_SOLID_TRIANGLE                                 (0x0000005D)
/* NvNotification[] elements */
#define NV05D_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV05D_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV05D_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV05D_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV05D_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV05D_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV05D_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV05D_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextClipRectangle; /* NV01_IMAGE_BLACK_RECTANGLE        0184-0187*/
 V032 SetContextPattern;       /* NV04_CONTEXT_PATTERN              0188-018b*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  018c-018f*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0190-0193*/
 V032 SetContextBeta4;         /* NV04_CONTEXT_BETA                 0194-0197*/
 V032 SetContextSurface;       /* NV04_CONTEXT_SURFACES_2D          0198-019b*/
 V032 Reserved01[0x058];
 V032 SetOperation;            /* NV05D_SET_OPERATION_*             02fc-02ff*/
 V032 SetColorFormat;          /* NV05D_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Color;                   /* source color                      0304-0307*/
 V032 Reserved02[0x002];
 V032 TrianglePoint0;          /* y_x S16_S16 in pixels             0310-0313*/
 V032 TrianglePoint1;          /* y_x S16_S16 in pixels             0314-0317*/
 V032 TrianglePoint2;          /* y_x S16_S16 in pixels             0318-031b*/
 V032 Reserved03[0x001];
 S032 Triangle32Point0X;       /* in pixels, 0 at left              0320-0323*/
 S032 Triangle32Point0Y;       /* in pixels, 0 at top               0324-0327*/
 S032 Triangle32Point1X;       /* in pixels, 0 at left              0328-032b*/
 S032 Triangle32Point1Y;       /* in pixels, 0 at top               032c-032f*/
 S032 Triangle32Point2X;       /* in pixels, 0 at left              0330-0333*/
 S032 Triangle32Point2Y;       /* in pixels, 0 at top               0334-0337*/
 V032 Reserved04[0x032];
 V032 Trimesh[32];             /* y_x S16_S16 in pixels             0400-047f*/
 struct {                      /* start of aliased methods in array 0480-    */
  S032 x;                      /* in pixels, 0 at left                 0-   3*/
  S032 y;                      /* in pixels, 0 at top                  4-   7*/
 } Trimesh32[16];              /* end of aliased methods in array       -04ff*/
 struct {                      /* start of aliased methods in array 0500-    */
  V032 color;                  /* source color                         0-   3*/
  V032 point0;                 /* y_x S16_S16 in pixels                4-   7*/
  V032 point1;                 /* y_x S16_S16 in pixels                8-   b*/
  V032 point2;                 /* y_x S16_S16 in pixels                c-   f*/
 } ColorTriangle[8];           /* end of aliased methods in array       -057f*/
 struct {                      /* start of aliased methods in array 0580-    */
  V032 color;                  /* source color                         0-   3*/
  V032 point;                  /* y_x S16_S16 in pixels                4-   7*/
 } ColorTrimesh[16];           /* end of aliased methods in array       -05ff*/
 V032 Reserved05[0x680];
} Nv05dTypedef, Nv04RenderSolidTriangle;
#define Nv4RenderSolidTriangle                          Nv04RenderSolidTriangle
#define nv4RenderSolidTriangle                          nv04RenderSolidTriangle
#define NV05D_TYPEDEF                                   nv04RenderSolidTriangle
/* dma method offsets, fields, and values */
#define NV05D_SET_OBJECT                                           (0x00000000)
#define NV05D_NO_OPERATION                                         (0x00000100)
#define NV05D_NOTIFY                                               (0x00000104)
#define NV05D_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV05D_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV05D_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV05D_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000184)
#define NV05D_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV05D_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV05D_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV05D_SET_CONTEXT_BETA4                                    (0x00000194)
#define NV05D_SET_CONTEXT_SURFACE                                  (0x00000198)
#define NV05D_SET_OPERATION                                        (0x000002FC)
#define NV05D_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV05D_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV05D_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV05D_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV05D_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV05D_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV05D_SET_COLOR_FORMAT                                     (0x00000300)
#define NV05D_SET_COLOR_FORMAT_LE_X16R5G6B5                        (0x00000001)
#define NV05D_SET_COLOR_FORMAT_LE_X17R5G5B5                        (0x00000002)
#define NV05D_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000003)
#define NV05D_COLOR                                                (0x00000304)
#define NV05D_TRIANGLE                                             (0x00000310)
#define NV05D_TRIANGLE_POINT0                                      (0x00000310)
#define NV05D_TRIANGLE_POINT0_X                                    15:0
#define NV05D_TRIANGLE_POINT0_Y                                    31:16
#define NV05D_TRIANGLE_POINT1                                      (0x00000314)
#define NV05D_TRIANGLE_POINT1_X                                    15:0
#define NV05D_TRIANGLE_POINT1_Y                                    31:16
#define NV05D_TRIANGLE_POINT2                                      (0x00000318)
#define NV05D_TRIANGLE_POINT2_X                                    15:0
#define NV05D_TRIANGLE_POINT2_Y                                    31:16
#define NV05D_TRIANGLE32_POINT0_X                                  (0x00000320)
#define NV05D_TRIANGLE32_POINT0_Y                                  (0x00000324)
#define NV05D_TRIANGLE32_POINT1_X                                  (0x00000328)
#define NV05D_TRIANGLE32_POINT1_Y                                  (0x0000032C)
#define NV05D_TRIANGLE32_POINT2_X                                  (0x00000330)
#define NV05D_TRIANGLE32_POINT2_Y                                  (0x00000334)
#define NV05D_TRIMESH(a)                                           (0x00000400\
                                                                   +(a)*0x0004)
#define NV05D_TRIMESH_X                                            15:0
#define NV05D_TRIMESH_Y                                            31:16
#define NV05D_TRIMESH32(a)                                         (0x00000480\
                                                                   +(a)*0x0008)
#define NV05D_TRIMESH32_X(a)                                       (0x00000480\
                                                                   +(a)*0x0008)
#define NV05D_TRIMESH32_Y(a)                                       (0x00000484\
                                                                   +(a)*0x0008)
#define NV05D_COLOR_TRIANGLE(a)                                    (0x00000500\
                                                                   +(a)*0x0010)
#define NV05D_COLOR_TRIANGLE_COLOR(a)                              (0x00000500\
                                                                   +(a)*0x0010)
#define NV05D_COLOR_TRIANGLE_POINT0(a)                             (0x00000504\
                                                                   +(a)*0x0010)
#define NV05D_COLOR_TRIANGLE_POINT0_X                              15:0
#define NV05D_COLOR_TRIANGLE_POINT0_Y                              31:16
#define NV05D_COLOR_TRIANGLE_POINT1(a)                             (0x00000508\
                                                                   +(a)*0x0010)
#define NV05D_COLOR_TRIANGLE_POINT1_X                              15:0
#define NV05D_COLOR_TRIANGLE_POINT1_Y                              31:16
#define NV05D_COLOR_TRIANGLE_POINT2(a)                             (0x0000050C\
                                                                   +(a)*0x0010)
#define NV05D_COLOR_TRIANGLE_POINT2_X                              15:0
#define NV05D_COLOR_TRIANGLE_POINT2_Y                              31:16
#define NV05D_COLOR_TRIMESH(a)                                     (0x00000580\
                                                                   +(a)*0x0008)
#define NV05D_COLOR_TRIMESH_COLOR(a)                               (0x00000580\
                                                                   +(a)*0x0008)
#define NV05D_COLOR_TRIMESH_POINT(a)                               (0x00000584\
                                                                   +(a)*0x0008)
#define NV05D_COLOR_TRIMESH_POINT_X                                15:0
#define NV05D_COLOR_TRIMESH_POINT_Y                                31:16


/* class NV04_RENDER_SOLID_RECTANGLE */
#define  NV04_RENDER_SOLID_RECTANGLE                               (0x0000005E)
#define  NV4_RENDER_SOLID_RECTANGLE                                (0x0000005E)
/* NvNotification[] elements */
#define NV05E_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV05E_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV05E_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV05E_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV05E_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV05E_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV05E_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV05E_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextClipRectangle; /* NV01_IMAGE_BLACK_RECTANGLE        0184-0187*/
 V032 SetContextPattern;       /* NV04_CONTEXT_PATTERN              0188-018b*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  018c-018f*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0190-0193*/
 V032 SetContextBeta4;         /* NV04_CONTEXT_BETA                 0194-0197*/
 V032 SetContextSurface;       /* NV04_CONTEXT_SURFACES_2D          0198-019b*/
 V032 Reserved01[0x058];
 V032 SetOperation;            /* NV05E_SET_OPERATION_*             02fc-02ff*/
 V032 SetColorFormat;          /* NV05E_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Color;                   /* source color                      0304-0307*/
 V032 Reserved02[0x03e];
 struct {                      /* start of aliased methods in array 0400-    */
  V032 point;                  /* y_x S16_S16                          0-   3*/
  V032 size;                   /* height_width U16_U16                 4-   7*/
 } Rectangle[16];              /* end of aliased methods in array       -047f*/
 V032 Reserved03[0x6e0];
} Nv05eTypedef, Nv04RenderSolidRectangle;
#define Nv4RenderSolidRectangle                        Nv04RenderSolidRectangle
#define nv4RenderSolidRectangle                        nv04RenderSolidRectangle
#define NV05E_TYPEDEF                                  nv04RenderSolidRectangle
/* dma method offsets, fields, and values */
#define NV05E_SET_OBJECT                                           (0x00000000)
#define NV05E_NO_OPERATION                                         (0x00000100)
#define NV05E_NOTIFY                                               (0x00000104)
#define NV05E_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV05E_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV05E_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV05E_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000184)
#define NV05E_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV05E_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV05E_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV05E_SET_CONTEXT_BETA4                                    (0x00000194)
#define NV05E_SET_CONTEXT_SURFACE                                  (0x00000198)
#define NV05E_SET_OPERATION                                        (0x000002FC)
#define NV05E_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV05E_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV05E_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV05E_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV05E_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV05E_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV05E_SET_COLOR_FORMAT                                     (0x00000300)
#define NV05E_SET_COLOR_FORMAT_LE_X16R5G6B5                        (0x00000001)
#define NV05E_SET_COLOR_FORMAT_LE_X17R5G5B5                        (0x00000002)
#define NV05E_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000003)
#define NV05E_COLOR                                                (0x00000304)
#define NV05E_RECTANGLE(a)                                         (0x00000400\
                                                                   +(a)*0x0008)
#define NV05E_RECTANGLE_POINT(a)                                   (0x00000400\
                                                                   +(a)*0x0008)
#define NV05E_RECTANGLE_POINT_X                                    15:0
#define NV05E_RECTANGLE_POINT_Y                                    31:16
#define NV05E_RECTANGLE_SIZE(a)                                    (0x00000404\
                                                                   +(a)*0x0008)
#define NV05E_RECTANGLE_SIZE_WIDTH                                 15:0
#define NV05E_RECTANGLE_SIZE_HEIGHT                                31:16


/* class NV04_IMAGE_BLIT */
#define  NV04_IMAGE_BLIT                                           (0x0000005F)
#define  NV4_IMAGE_BLIT                                            (0x0000005F)
/* NvNotification[] elements */
#define NV05F_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV05F_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV05F_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV05F_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV05F_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV05F_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV05F_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV05F_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextColorKey;      /* NV04_CONTEXT_COLOR_KEY            0184-0187*/
 V032 SetContextClipRectangle; /* NV01_IMAGE_BLACK_RECTANGLE        0188-018b*/
 V032 SetContextPattern;       /* NV04_CONTEXT_PATTERN              018c-018f*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  0190-0193*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0194-0197*/
 V032 SetContextBeta4;         /* NV04_CONTEXT_BETA                 0198-019b*/
 V032 SetContextSurfaces;      /* NV04_CONTEXT_SURFACES_2D          019c-019f*/
 V032 Reserved01[0x057];
 V032 SetOperation;            /* NV05F_SET_OPERATION_*             02fc-02ff*/
 V032 ControlPointIn;          /* y_x U16_U16, pixels, top left src 0300-0303*/
 V032 ControlPointOut;         /* y_x S16_S16, pixels, top left dst 0304-0307*/
 V032 Size;                    /* height_width U16_U16 in pixels    0308-030b*/
 V032 Reserved02[0x73d];
} Nv05fTypedef, Nv04ImageBlit;
#define Nv4ImageBlit                                              Nv04ImageBlit
#define nv4ImageBlit                                              nv04ImageBlit
#define NV05F_TYPEDEF                                             nv04ImageBlit
/* dma method offsets, fields, and values */
#define NV05F_SET_OBJECT                                           (0x00000000)
#define NV05F_NO_OPERATION                                         (0x00000100)
#define NV05F_NOTIFY                                               (0x00000104)
#define NV05F_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV05F_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV05F_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV05F_SET_CONTEXT_COLOR_KEY                                (0x00000184)
#define NV05F_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000188)
#define NV05F_SET_CONTEXT_PATTERN                                  (0x0000018C)
#define NV05F_SET_CONTEXT_ROP                                      (0x00000190)
#define NV05F_SET_CONTEXT_BETA1                                    (0x00000194)
#define NV05F_SET_CONTEXT_BETA4                                    (0x00000198)
#define NV05F_SET_CONTEXT_SURFACES                                 (0x0000019C)
#define NV05F_SET_OPERATION                                        (0x000002FC)
#define NV05F_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV05F_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV05F_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV05F_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV05F_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV05F_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV05F_CONTROL_POINT_IN                                     (0x00000300)
#define NV05F_CONTROL_POINT_IN_X                                   15:0
#define NV05F_CONTROL_POINT_IN_Y                                   31:16
#define NV05F_CONTROL_POINT_OUT                                    (0x00000304)
#define NV05F_CONTROL_POINT_OUT_X                                  15:0
#define NV05F_CONTROL_POINT_OUT_Y                                  31:16
#define NV05F_SIZE                                                 (0x00000308)
#define NV05F_SIZE_WIDTH                                           15:0
#define NV05F_SIZE_HEIGHT                                          31:16


/* class NV04_INDEXED_IMAGE_FROM_CPU */
#define  NV04_INDEXED_IMAGE_FROM_CPU                               (0x00000060)
#define  NV4_INDEXED_IMAGE_FROM_CPU                                (0x00000060)
/* NvNotification[] elements */
#define NV060_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV060_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV060_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV060_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV060_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV060_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV060_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV060_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaLut;        /* NV01_CONTEXT_DMA_FROM_MEMORY      0184-0187*/
 V032 SetContextColorKey;      /* NV04_CONTEXT_COLOR_KEY            0188-018b*/
 V032 SetContextClipRectangle; /* NV01_IMAGE_BLACK_RECTANGLE        018c-018f*/
 V032 SetContextPattern;       /* NV04_CONTEXT_PATTERN              0190-0193*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  0194-0197*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0198-019b*/
 V032 SetContextBeta4;         /* NV04_CONTEXT_BETA                 019c-019f*/
 V032 SetContextSurface;       /* NV04_CONTEXT_SURFACES_2D,SWIZZLED 01a0-01a3*/
 V032 Reserved01[0x090];
 V032 SetOperation;            /* NV060_SET_OPERATION_*             03e4-03e7*/
 V032 SetColorFormat;          /* NV060_SET_COLOR_FORMAT_*          03e8-03eb*/
 V032 IndexFormat;             /* NV060_INDEX_FORMAT_*              03ec-03ef*/
 U032 LutOffset;               /* offset in bytes                   03f0-03f3*/
 V032 Point;                   /* y_x S16_S16 in pixels             03f4-03f7*/
 V032 SizeOut;                 /* height_width U16_U16, pixels, dst 03f8-03fb*/
 V032 SizeIn;                  /* height_width U16_U16, pixels, src 03fc-03ff*/
 V032 Indices[1792];           /* source indices (packed texels)    0400-1fff*/
} Nv060Typedef, Nv04IndexedImageFromCpu;
#define Nv4IndexedImageFromCpu                          Nv04IndexedImageFromCpu
#define nv4IndexedImageFromCpu                          nv04IndexedImageFromCpu
#define NV060_TYPEDEF                                   nv04IndexedImageFromCpu
/* dma method offsets, fields, and values */
#define NV060_SET_OBJECT                                           (0x00000000)
#define NV060_NO_OPERATION                                         (0x00000100)
#define NV060_NOTIFY                                               (0x00000104)
#define NV060_NOTIFY_WRITE_ONLY                                   (0x00000000)
#define NV060_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV060_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV060_SET_CONTEXT_DMA_LUT                                  (0x00000184)
#define NV060_SET_CONTEXT_COLOR_KEY                                (0x00000188)
#define NV060_SET_CONTEXT_CLIP_RECTANGLE                           (0x0000018C)
#define NV060_SET_CONTEXT_PATTERN                                  (0x00000190)
#define NV060_SET_CONTEXT_ROP                                      (0x00000194)
#define NV060_SET_CONTEXT_BETA1                                    (0x00000198)
#define NV060_SET_CONTEXT_BETA4                                    (0x0000019C)
#define NV060_SET_CONTEXT_SURFACE                                  (0x000001A0)
#define NV060_SET_OPERATION                                        (0x000003E4)
#define NV060_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV060_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV060_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV060_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV060_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV060_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV060_INDEX_FORMAT                                         (0x000003EC)
#define NV060_INDEX_FORMAT_LE_I8                                   (0x00000000)
#define NV060_INDEX_FORMAT_SVGA65_I4                               (0x00000001)
#define NV060_SET_COLOR_FORMAT                                     (0x000003E8)
#define NV060_SET_COLOR_FORMAT_LE_R5G6B5                           (0x00000001)
#define NV060_SET_COLOR_FORMAT_LE_A1R5G5B5                         (0x00000002)
#define NV060_SET_COLOR_FORMAT_LE_X1R5G5B5                         (0x00000003)
#define NV060_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000004)
#define NV060_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000005)
#define NV060_LUT_OFFSET                                           (0x000003F0)
#define NV060_POINT                                                (0x000003F4)
#define NV060_POINT_X                                              15:0
#define NV060_POINT_Y                                              31:16
#define NV060_SIZE_OUT                                             (0x000003F8)
#define NV060_SIZE_OUT_WIDTH                                       15:0
#define NV060_SIZE_OUT_HEIGHT                                      31:16
#define NV060_SIZE_IN                                              (0x000003FC)
#define NV060_SIZE_IN_WIDTH                                        15:0
#define NV060_SIZE_IN_HEIGHT                                       31:16
#define NV060_INDICES(a)                                           (0x00000400\
                                                                   +(a)*0x0004)


/* class NV04_IMAGE_FROM_CPU */
#define  NV04_IMAGE_FROM_CPU                                       (0x00000061)
#define  NV4_IMAGE_FROM_CPU                                        (0x00000061)
/* NvNotification[] elements */
#define NV061_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV061_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV061_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV061_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV061_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV061_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV061_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV061_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextColorKey;      /* NV04_CONTEXT_COLOR_KEY            0184-0187*/
 V032 SetContextClipRectangle; /* NV01_IMAGE_BLACK_RECTANGLE        0188-018b*/
 V032 SetContextPattern;       /* NV04_CONTEXT_PATTERN              018c-018f*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  0190-0193*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0194-0197*/
 V032 SetContextBeta4;         /* NV04_CONTEXT_BETA                 0198-019b*/
 V032 SetContextSurface;       /* NV04_CONTEXT_SURFACES_2D          019c-019f*/
 V032 Reserved01[0x057];
 V032 SetOperation;            /* NV061_SET_OPERATION_*             02fc-02ff*/
 V032 SetColorFormat;          /* NV061_SET_COLOR_FORMAT_*          0300-0303*/
 V032 Point;                   /* y_x S16_S16 in pixels             0304-0307*/
 V032 SizeOut;                 /* height_width U16_U16, pixels, dst 0308-030b*/
 V032 SizeIn;                  /* height_width U16_U16, pixels, src 030c-030f*/
 V032 Reserved02[0x03c];
 V032 Color[1792];             /* source colors (packed texels)     0400-1fff*/
} Nv061Typedef, Nv04ImageFromCpu;
#define Nv4ImageFromCpu                                        Nv04ImageFromCpu
#define nv4ImageFromCpu                                        nv04ImageFromCpu
#define NV061_TYPEDEF                                          nv04ImageFromCpu
/* dma method offsets, fields, and values */
#define NV061_SET_OBJECT                                           (0x00000000)
#define NV061_NO_OPERATION                                         (0x00000100)
#define NV061_NOTIFY                                               (0x00000104)
#define NV061_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV061_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV061_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV061_SET_CONTEXT_COLOR_KEY                                (0x00000184)
#define NV061_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000188)
#define NV061_SET_CONTEXT_PATTERN                                  (0x0000018C)
#define NV061_SET_CONTEXT_ROP                                      (0x00000190)
#define NV061_SET_CONTEXT_BETA1                                    (0x00000194)
#define NV061_SET_CONTEXT_BETA4                                    (0x00000198)
#define NV061_SET_CONTEXT_SURFACE                                  (0x0000019C)
#define NV061_SET_OPERATION                                        (0x000002FC)
#define NV061_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV061_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV061_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV061_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV061_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV061_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV061_SET_COLOR_FORMAT                                     (0x00000300)
#define NV061_SET_COLOR_FORMAT_LE_R5G6B5                           (0x00000001)
#define NV061_SET_COLOR_FORMAT_LE_A1R5G5B5                         (0x00000002)
#define NV061_SET_COLOR_FORMAT_LE_X1R5G5B5                         (0x00000003)
#define NV061_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000004)
#define NV061_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000005)
#define NV061_POINT                                                (0x00000304)
#define NV061_POINT_X                                              15:0
#define NV061_POINT_Y                                              31:16
#define NV061_SIZE_OUT                                             (0x00000308)
#define NV061_SIZE_OUT_WIDTH                                       15:0
#define NV061_SIZE_OUT_HEIGHT                                      31:16
#define NV061_SIZE_IN                                              (0x0000030C)
#define NV061_SIZE_IN_WIDTH                                        15:0
#define NV061_SIZE_IN_HEIGHT                                       31:16
#define NV061_COLOR(a)                                             (0x00000400\
                                                                   +(a)*0x0004)

typedef V032 Nv062Typedef;


/* class NV05_SCALED_IMAGE_FROM_MEMORY */
#define  NV05_SCALED_IMAGE_FROM_MEMORY                             (0x00000063)
/* NvNotification[] elements */
#define NV063_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV063_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV063_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV063_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV063_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV063_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV063_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 NvV32 NoOperation;             /* ignored                          0100-0103*/
 NvV32 Notify;                  /* NV063_NOTIFY_*                   0104-0107*/
 NvV32 Reserved00[0x01e];
 NvV32 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA                 0180-0183*/
 NvV32 SetContextDmaImage;      /* NV01_CONTEXT_DMA                 0184-0187*/
 NvV32 SetContextPattern;       /* NV04_CONTEXT_PATTERN             0188-018b*/
 NvV32 SetContextRop;           /* NV03_CONTEXT_ROP                 018c-018f*/
 NvV32 SetContextBeta1;         /* NV01_CONTEXT_BETA                0190-0193*/
 NvV32 SetContextBeta4;         /* NV04_CONTEXT_BETA                0194-0197*/
 NvV32 SetContextSurface;       /* NV04_CONTEXT_SURFACES_2D,SWIZZLE 0198-019b*/
 NvV32 Reserved01[0x059];
 NvV32 SetColorFormat;          /* NV063_SET_COLOR_FORMAT_*         0300-0303*/
 NvV32 SetOperation;            /* NV063_SET_OPERATION_*            0304-0307*/
 NvV32 ClipPoint;               /* y_x S16_S16                      0308-030b*/
 NvV32 ClipSize;                /* height_width U16_U16             030c-030f*/
 NvV32 ImageOutPoint;           /* y_x S16_S16                      0310-0313*/
 NvV32 ImageOutSize;            /* height_width U16_U16             0314-0317*/
 NvV32 DsDx;                    /* S12d20 ds/dx                     0318-031b*/
 NvV32 DtDy;                    /* S12d20 dt/dy                     031c-031f*/
 NvV32 SetColorConversion;      /* NV064_SET_COLOR_CONVERSION_*     0320-0323*/
 NvV32 Reserved02[0x037];
 NvV32 ImageInSize;             /* height_width U16_U16             0400-0403*/
 NvV32 ImageInFormat;           /* interpolator_origin_pitch        0404-0407*/
 NvU32 ImageInOffset;           /* bytes                            0408-040b*/
 NvV32 ImageInPoint;            /* v_u U12d4_U12d4                  040c-040f*/
 NvV32 Reserved03[0x6fc];
} Nv063Typedef, Nv05ScaledImageFromMemory;
#define NV063_TYPEDEF                                 nv05ScaledImageFromMemory
/* dma method offsets, fields, and values */
#define NV063_SET_OBJECT                                           (0x00000000)
#define NV063_NO_OPERATION                                         (0x00000100)
#define NV063_NOTIFY                                               (0x00000104)
#define NV063_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV063_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV063_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV063_SET_CONTEXT_DMA_IMAGE                                (0x00000184)
#define NV063_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV063_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV063_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV063_SET_CONTEXT_BETA4                                    (0x00000194)
#define NV063_SET_CONTEXT_SURFACE                                  (0x00000198)
#define NV063_SET_COLOR_FORMAT                                     (0x00000300)
#define NV063_SET_COLOR_FORMAT_LE_A1R5G5B5                         (0x00000001)
#define NV063_SET_COLOR_FORMAT_LE_X1R5G5B5                         (0x00000002)
#define NV063_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000003)
#define NV063_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000004)
#define NV063_SET_COLOR_FORMAT_LE_CR8YB8CB8YA8                     (0x00000005)
#define NV063_SET_COLOR_FORMAT_LE_YB8CR8YA8CB8                     (0x00000006)
#define NV063_SET_COLOR_FORMAT_LE_R5G6B5                           (0x00000007)
#define NV063_SET_OPERATION                                        (0x00000304)
#define NV063_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV063_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV063_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV063_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV063_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV063_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV063_CLIP_POINT                                           (0x00000308)
#define NV063_CLIP_POINT_X                                         15:0
#define NV063_CLIP_POINT_Y                                         31:16
#define NV063_CLIP_SIZE                                            (0x0000030C)
#define NV063_CLIP_SIZE_WIDTH                                      15:0
#define NV063_CLIP_SIZE_HEIGHT                                     31:16
#define NV063_IMAGE_OUT_POINT                                      (0x00000310)
#define NV063_IMAGE_OUT_POINT_X                                    15:0
#define NV063_IMAGE_OUT_POINT_Y                                    31:16
#define NV063_IMAGE_OUT_SIZE                                       (0x00000314)
#define NV063_IMAGE_OUT_SIZE_WIDTH                                 15:0
#define NV063_IMAGE_OUT_SIZE_HEIGHT                                31:16
#define NV063_DS_DX                                                (0x00000318)
#define NV063_DT_DY                                                (0x0000031C)
#define NV063_SET_COLOR_CONVERSION                                 (0x00000320)
#define NV063_SET_COLOR_CONVERSION_DITHER                          (0x00000000)
#define NV063_SET_COLOR_CONVERSION_TRUNCATE                        (0x00000001)
#define NV063_SET_COLOR_CONVERSION_SUBTRACT_TRUNCATE               (0x00000002)
#define NV063_IMAGE_IN_SIZE                                        (0x00000400)
#define NV063_IMAGE_IN_SIZE_WIDTH                                  15:0
#define NV063_IMAGE_IN_SIZE_HEIGHT                                 31:16
#define NV063_IMAGE_IN_FORMAT                                      (0x00000404)
#define NV063_IMAGE_IN_FORMAT_PITCH                                15:0
#define NV063_IMAGE_IN_FORMAT_ORIGIN                               23:16
#define NV063_IMAGE_IN_FORMAT_ORIGIN_CENTER                        (0x00000001)
#define NV063_IMAGE_IN_FORMAT_ORIGIN_CORNER                        (0x00000002)
#define NV063_IMAGE_IN_FORMAT_INTERPOLATOR                         31:24
#define NV063_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH                     (0x00000000)
#define NV063_IMAGE_IN_FORMAT_INTERPOLATOR_FOH                     (0x00000001)
#define NV063_IMAGE_IN_OFFSET                                      (0x00000408)
#define NV063_IMAGE_IN                                             (0x0000040C)
#define NV063_IMAGE_IN_POINT_U                                     15:0
#define NV063_IMAGE_IN_POINT_V                                     31:16

/* class NV05_INDEXED_IMAGE_FROM_CPU */
#define  NV05_INDEXED_IMAGE_FROM_CPU                               (0x00000064)
#define NV064_NOTIFIERS_NOTIFY                                     (0)
#define NV064_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV064_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV064_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV064_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV064_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV064_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 NvV32 NoOperation;             /* ignored                          0100-0103*/
 NvV32 Notify;                  /* NV064_NOTIFY_*                   0104-0107*/
 NvV32 Reserved00[0x01e];
 NvV32 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA                 0180-0183*/
 NvV32 SetContextDmaLut;        /* NV01_CONTEXT_DMA                 0184-0187*/
 NvV32 SetContextColorKey;      /* NV04_CONTEXT_COLOR_KEY           0188-018b*/
 NvV32 SetContextClipRectangle; /* NV01_CONTEXT_CLIP_RECTANGLE      018c-018f*/
 NvV32 SetContextPattern;       /* NV04_CONTEXT_PATTERN             0190-0193*/
 NvV32 SetContextRop;           /* NV03_CONTEXT_ROP                 0194-0197*/
 NvV32 SetContextBeta1;         /* NV01_CONTEXT_BETA                0198-019b*/
 NvV32 SetContextBeta4;         /* NV04_CONTEXT_BETA                019c-019f*/
 NvV32 SetContextSurface;       /* NV04_CONTEXT_SURFACES_2D,SWIZZLE 01a0-01a3*/
 NvV32 Reserved01[0x08f];
 NvV32 SetColorConversion;      /* NV064_SET_COLOR_CONVERSION_*     03e0-03e3*/
 NvV32 SetOperation;            /* NV064_SET_OPERATION_*            03e4-03e7*/
 NvV32 SetColorFormat;          /* NV064_SET_COLOR_FORMAT_*         03e8-03eb*/
 NvV32 IndexFormat;             /* NV064_INDEX_FORMAT_*             03ec-03ef*/
 NvU32 LutOffset;               /* offset in bytes                  03f0-03f3*/
 NvV32 Point;                   /* y_x S16_S16 in pixels            03f4-03f7*/
 NvV32 SizeOut;                 /* height_width U16_U16, pixels     03f8-03fb*/
 NvV32 SizeIn;                  /* height_width U16_U16, pixels     03fc-03ff*/
 NvV32 Indices[1792];           /* source indices (packed texels)   0400-1fff*/
} Nv064Typedef, Nv05IndexedImageFromCpu;
#define NV064_TYPEDEF                                   nv05IndexedImageFromCpu
/* dma method offsets, fields, and values */
#define NV064_NO_OPERATION                                         (0x00000100)
#define NV064_NOTIFY                                               (0x00000104)
#define NV064_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV064_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV064_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV064_SET_CONTEXT_DMA_LUT                                  (0x00000184)
#define NV064_SET_CONTEXT_COLOR_KEY                                (0x00000188)
#define NV064_SET_CONTEXT_CLIP_RECTANGLE                           (0x0000018C)
#define NV064_SET_CONTEXT_PATTERN                                  (0x00000190)
#define NV064_SET_CONTEXT_ROP                                      (0x00000194)
#define NV064_SET_CONTEXT_BETA1                                    (0x00000198)
#define NV064_SET_CONTEXT_BETA4                                    (0x0000019C)
#define NV064_SET_CONTEXT_SURFACE                                  (0x000001A0)
#define NV064_SET_COLOR_CONVERSION                                 (0x000003E0)
#define NV064_SET_COLOR_CONVERSION_DITHER                          (0x00000000)
#define NV064_SET_COLOR_CONVERSION_TRUNCATE                        (0x00000001)
#define NV064_SET_COLOR_CONVERSION_SUBTRACT_TRUNCATE               (0x00000002)
#define NV064_SET_OPERATION                                        (0x000003E4)
#define NV064_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV064_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV064_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV064_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV064_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV064_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV064_SET_COLOR_FORMAT                                     (0x000003E8)
#define NV064_SET_COLOR_FORMAT_LE_R5G6B5                           (0x00000001)
#define NV064_SET_COLOR_FORMAT_LE_A1R5G5B5                         (0x00000002)
#define NV064_SET_COLOR_FORMAT_LE_X1R5G5B5                         (0x00000003)
#define NV064_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000004)
#define NV064_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000005)
#define NV064_INDEX_FORMAT                                         (0x000003EC)
#define NV064_INDEX_FORMAT_LE_I8                                   (0x00000000)
#define NV064_INDEX_FORMAT_SVGA65_I4                               (0x00000001)
#define NV064_LUT_OFFSET                                           (0x000003F0)
#define NV064_POINT                                                (0x000003F4)
#define NV064_POINT_X                                              15:0
#define NV064_POINT_Y                                              31:16
#define NV064_SIZE_OUT                                             (0x000003F8)
#define NV064_SIZE_OUT_WIDTH                                       15:0
#define NV064_SIZE_OUT_HEIGHT                                      31:16
#define NV064_SIZE_IN                                              (0x000003FC)
#define NV064_SIZE_IN_WIDTH                                        15:0
#define NV064_SIZE_IN_HEIGHT                                       31:16
#define NV064_INDICES(a)                                           (0x00000400\
                                                                   +(a)*0x0004)
/* class NV05_IMAGE_FROM_CPU */
#define  NV05_IMAGE_FROM_CPU                                       (0x00000065)
#define NV065_NOTIFIERS_NOTIFY                                     (0)
#define NV065_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV065_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV065_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV065_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV065_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV065_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 NvV32 NoOperation;             /* ignored                          0100-0103*/
 NvV32 Notify;                  /* NV065_NOTIFY_*                   0104-0107*/
 NvV32 Reserved00[0x01e];
 NvV32 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA                 0180-0183*/
 NvV32 SetContextColorKey;      /* NV04_CONTEXT_COLOR_KEY           0184-0187*/
 NvV32 SetContextClipRectangle; /* NV01_CONTEXT_CLIP_RECTANGLE      0188-018b*/
 NvV32 SetContextPattern;       /* NV04_CONTEXT_PATTERN             018c-018f*/
 NvV32 SetContextRop;           /* NV03_CONTEXT_ROP                 0190-0193*/
 NvV32 SetContextBeta1;         /* NV01_CONTEXT_BETA                0194-0197*/
 NvV32 SetContextBeta4;         /* NV04_CONTEXT_BETA                0198-019b*/
 NvV32 SetContextSurface;       /* NV04_CONTEXT_SURFACES_2D         019c-019f*/
 NvV32 Reserved01[0x056];
 NvV32 SetColorConversion;      /* NV065_SET_COLOR_CONVERSION_*     02f8-02fb*/
 NvV32 SetOperation;            /* NV065_SET_OPERATION_*            02fc-02ff*/
 NvV32 SetColorFormat;          /* NV065_SET_COLOR_FORMAT_*         0300-0303*/
 NvV32 Point;                   /* y_x S16_S16 in pixels            0304-0307*/
 NvV32 SizeOut;                 /* height_width U16_U16, pixels     0308-030b*/
 NvV32 SizeIn;                  /* height_width U16_U16, pixels     030c-030f*/
 NvV32 Reserved02[0x03c];
 NvV32 Color[1792];             /* source colors (packed texels)    0400-1fff*/
} Nv065Typedef, Nv05ImageFromCpu;
#define NV065_TYPEDEF                                          nv05ImageFromCpu
/* dma method offsets, fields, and values */
#define NV065_NO_OPERATION                                         (0x00000100)
#define NV065_NOTIFY                                               (0x00000104)
#define NV065_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV065_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV065_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV065_SET_CONTEXT_COLOR_KEY                                (0x00000184)
#define NV065_SET_CONTEXT_CLIP_RECTANGLE                           (0x00000188)
#define NV065_SET_CONTEXT_PATTERN                                  (0x0000018C)
#define NV065_SET_CONTEXT_ROP                                      (0x00000190)
#define NV065_SET_CONTEXT_BETA1                                    (0x00000194)
#define NV065_SET_CONTEXT_BETA4                                    (0x00000198)
#define NV065_SET_CONTEXT_SURFACE                                  (0x0000019C)
#define NV065_SET_COLOR_CONVERSION                                 (0x000002F8)
#define NV065_SET_COLOR_CONVERSION_DITHER                          (0x00000000)
#define NV065_SET_COLOR_CONVERSION_TRUNCATE                        (0x00000001)
#define NV065_SET_COLOR_CONVERSION_SUBTRACT_TRUNCATE               (0x00000002)
#define NV065_SET_OPERATION                                        (0x000002FC)
#define NV065_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV065_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV065_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV065_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV065_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV065_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV065_SET_COLOR_FORMAT                                     (0x00000300)
#define NV065_SET_COLOR_FORMAT_LE_R5G6B5                           (0x00000001)
#define NV065_SET_COLOR_FORMAT_LE_A1R5G5B5                         (0x00000002)
#define NV065_SET_COLOR_FORMAT_LE_X1R5G5B5                         (0x00000003)
#define NV065_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000004)
#define NV065_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000005)
#define NV065_POINT                                                (0x00000304)
#define NV065_POINT_X                                              15:0
#define NV065_POINT_Y                                              31:16
#define NV065_SIZE_OUT                                             (0x00000308)
#define NV065_SIZE_OUT_WIDTH                                       15:0
#define NV065_SIZE_OUT_HEIGHT                                      31:16
#define NV065_SIZE_IN                                              (0x0000030C)
#define NV065_SIZE_IN_WIDTH                                        15:0
#define NV065_SIZE_IN_HEIGHT                                       31:16
#define NV065_COLOR(a)                                             (0x00000400\
                                                                   +(a)*0x0004)
 /* class NV05_STRETCHED_IMAGE_FROM_CPU */
#define  NV05_STRETCHED_IMAGE_FROM_CPU                             (0x00000066)
#define NV066_NOTIFIERS_NOTIFY                                     (0)
#define NV066_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV066_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV066_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV066_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV066_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV066_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)

/* pio method data structure */
typedef volatile struct {
 NvV32 NoOperation;             /* ignored                          0100-0103*/
 NvV32 Notify;                  /* NV066_NOTIFY_*                   0104-0107*/
 NvV32 Reserved00[0x01e];
 NvV32 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA                 0180-0183*/
 NvV32 SetContextColorKey;      /* NV04_CONTEXT_COLOR_KEY           0184-0187*/
 NvV32 SetContextPattern;       /* NV04_CONTEXT_PATTERN             0188-018b*/
 NvV32 SetContextRop;           /* NV03_CONTEXT_ROP                 018c-018f*/
 NvV32 SetContextBeta1;         /* NV01_CONTEXT_BETA                0190-0193*/
 NvV32 SetContextBeta4;         /* NV04_CONTEXT_BETA                0194-0197*/
 NvV32 SetContextSurface;       /* NV04_CONTEXT_SURFACES_2D         0198-019b*/
 NvV32 Reserved01[0x057];
 NvV32 SetColorConversion;      /* NV066_SET_COLOR_CONVERSION_*     02f8-02fb*/
 NvV32 SetOperation;            /* NV066_SET_OPERATION_*            02fc-02ff*/
 NvV32 SetColorFormat;          /* NV066_SET_COLOR_FORMAT_*         0300-0303*/
 NvV32 SizeIn;                  /* height_width U16_U16 in texels   0304-0307*/
 NvV32 DxDs;                    /* S12d20 dx/ds                     0308-030b*/
 NvV32 DyDt;                    /* S12d20 dy/dt                     030c-030f*/
 NvV32 ClipPoint;               /* y_x S16_S16                      0310-0313*/
 NvV32 ClipSize;                /* height_width U16_U16             0314-0317*/
 NvV32 Point12d4;               /* y_x S12d4_S12d4 in pixels        0318-031b*/
 NvV32 Reserved02[0x039];
 NvV32 Color[1792];             /* source colors (packed texels)    0400-1fff*/
} Nv066Typedef, Nv05StretchedImageFromCpu;
#define NV066_TYPEDEF                                 nv05StretchedImageFromCpu
/* dma method offsets, fields, and values */
#define NV066_NO_OPERATION                                         (0x00000100)
#define NV066_NOTIFY                                               (0x00000104)
#define NV066_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV066_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV066_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV066_SET_CONTEXT_COLOR_KEY                                (0x00000184)
#define NV066_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV066_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV066_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV066_SET_CONTEXT_BETA4                                    (0x00000194)
#define NV066_SET_CONTEXT_SURFACE                                  (0x00000198)
#define NV066_SET_COLOR_CONVERSION                                 (0x000002F8)
#define NV066_SET_COLOR_CONVERSION_DITHER                          (0x00000000)
#define NV066_SET_COLOR_CONVERSION_TRUNCATE                        (0x00000001)
#define NV066_SET_COLOR_CONVERSION_SUBTRACT_TRUNCATE               (0x00000002)
#define NV066_SET_OPERATION                                        (0x000002FC)
#define NV066_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV066_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV066_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV066_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV066_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV066_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV066_SET_COLOR_FORMAT                                     (0x00000300)
#define NV066_SET_COLOR_FORMAT_LE_R5G6B5                           (0x00000001)
#define NV066_SET_COLOR_FORMAT_LE_A1R5G5B5                         (0x00000002)
#define NV066_SET_COLOR_FORMAT_LE_X1R5G5B5                         (0x00000003)
#define NV066_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000004)
#define NV066_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000005)
#define NV066_SIZE_IN                                              (0x00000304)
#define NV066_SIZE_IN_WIDTH                                        15:0
#define NV066_SIZE_IN_HEIGHT                                       31:16
#define NV066_DX_DS                                                (0x00000308)
#define NV066_DY_DT                                                (0x0000030C)
#define NV066_CLIP_POINT                                           (0x00000310)
#define NV066_CLIP_POINT_X                                         15:0
#define NV066_CLIP_POINT_Y                                         31:16
#define NV066_CLIP_SIZE                                            (0x00000314)
#define NV066_CLIP_SIZE_WIDTH                                      15:0
#define NV066_CLIP_SIZE_HEIGHT                                     31:16
#define NV066_POINT_12D4                                           (0x00000318)
#define NV066_POINT_12D4_X                                         15:0
#define NV066_POINT_12D4_Y                                         31:16
#define NV066_COLOR(a)                                             (0x00000400\


typedef V032 Nv067Typedef;

typedef V032 Nv06dTypedef;

typedef V032 Nv06eTypedef;

typedef V032 Nv06fTypedef;

typedef V032 Nv070Typedef;

typedef V032 Nv071Typedef;


/* class NV04_CONTEXT_BETA */
#define  NV04_CONTEXT_BETA                                         (0x00000072)
#define  NV4_CONTEXT_BETA                                          (0x00000072)
/* NvNotification[] elements */
#define NV072_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV072_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV072_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV072_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV072_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV072_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV072_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV072_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 Reserved01[0x05f];
 V032 SetBetaFactor;           /* A8R8G8B8 beta values              0300-0303*/
 V032 Reserved02[0x73f];
} Nv072Typedef, Nv04ContextBeta;
#define Nv4ContextBeta                                          Nv04ContextBeta
#define nv4ContextBeta                                          nv04ContextBeta
#define NV072_TYPEDEF                                           nv04ContextBeta
/* dma method offsets, fields, and values */
#define NV072_SET_OBJECT                                           (0x00000000)
#define NV072_NO_OPERATION                                         (0x00000100)
#define NV072_NOTIFY                                               (0x00000104)
#define NV072_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV072_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV072_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV072_SET_BETA_FACTOR                                      (0x00000300)
#define NV072_SET_BETA_FACTOR_BLUE                                 7:0
#define NV072_SET_BETA_FACTOR_GREEN                                15:8
#define NV072_SET_BETA_FACTOR_RED                                  23:16
#define NV072_SET_BETA_FACTOR_ALPHA                                31:24

typedef V032 Nv073Typedef;

typedef V032 Nv074Typedef;

typedef V032 Nv075Typedef;


/* class NV04_STRETCHED_IMAGE_FROM_CPU */
#define  NV04_STRETCHED_IMAGE_FROM_CPU                             (0x00000076)
#define  NV4_STRETCHED_IMAGE_FROM_CPU                              (0x00000076)
/* NvNotification[] elements */
#define NV076_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV076_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV076_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV076_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV076_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV076_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV076_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV076_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextColorKey;      /* NV04_CONTEXT_COLOR_KEY            0184-0187*/
 V032 SetContextPattern;       /* NV04_CONTEXT_PATTERN              0188-018b*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  018c-018f*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0190-0193*/
 V032 SetContextBeta4;         /* NV04_CONTEXT_BETA                 0194-0197*/
 V032 SetContextSurface;       /* NV04_CONTEXT_SURFACES_2D          0198-019b*/
 V032 Reserved01[0x058];
 V032 SetOperation;            /* NV076_SET_OPERATION_*             02fc-02ff*/
 V032 SetColorFormat;          /* NV076_SET_COLOR_FORMAT_*          0300-0303*/
 V032 SizeIn;                  /* height_width U16_U16 in texels    0304-0307*/
 V032 DeltaDxDu;               /* S12d20 ratio dx/du                0308-030b*/
 V032 DeltaDyDv;               /* S12d20 ratio dy/dv                030c-030f*/
 V032 ClipPoint;               /* y_x S16_S16                       0310-0313*/
 V032 ClipSize;                /* height_width U16_U16              0314-0317*/
 V032 Point12d4;               /* y_x S12d4_S12d4 in pixels         0318-031b*/
 V032 Reserved02[0x039];
 V032 Color[1792];             /* source colors (packed texels)     0400-1fff*/
} Nv076Typedef, Nv04StretchedImageFromCpu;
#define Nv4StretchedImageFromCpu                      Nv04StretchedImageFromCpu
#define nv4StretchedImageFromCpu                      nv04StretchedImageFromCpu
#define NV076_TYPEDEF                                 nv04StretchedImageFromCpu
/* dma method offsets, fields, and values */
#define NV076_SET_OBJECT                                           (0x00000000)
#define NV076_NO_OPERATION                                         (0x00000100)
#define NV076_NOTIFY                                               (0x00000104)
#define NV076_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV076_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV076_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV076_SET_CONTEXT_COLOR_KEY                                (0x00000184)
#define NV076_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV076_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV076_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV076_SET_CONTEXT_BETA4                                    (0x00000194)
#define NV076_SET_CONTEXT_SURFACE                                  (0x00000198)
#define NV076_SET_OPERATION                                        (0x000002FC)
#define NV076_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV076_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV076_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV076_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV076_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV076_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV076_SET_COLOR_FORMAT                                     (0x00000300)
#define NV076_SET_COLOR_FORMAT_LE_R5G6B5                           (0x00000001)
#define NV076_SET_COLOR_FORMAT_LE_A1R5G5B5                         (0x00000002)
#define NV076_SET_COLOR_FORMAT_LE_X1R5G5B5                         (0x00000003)
#define NV076_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000004)
#define NV076_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000005)
#define NV076_SIZE_IN                                              (0x00000304)
#define NV076_SIZE_IN_WIDTH                                        15:0
#define NV076_SIZE_IN_HEIGHT                                       31:16
#define NV076_DELTA_DX_DU                                          (0x00000308)
#define NV076_DELTA_DY_DV                                          (0x0000030C)
#define NV076_CLIP_POINT                                           (0x00000310)
#define NV076_CLIP_POINT_X                                         15:0
#define NV076_CLIP_POINT_Y                                         31:16
#define NV076_CLIP_SIZE                                            (0x00000314)
#define NV076_CLIP_SIZE_WIDTH                                      15:0
#define NV076_CLIP_SIZE_HEIGHT                                     31:16
#define NV076_POINT_12D4                                           (0x00000318)
#define NV076_POINT_12D4_X                                         15:0
#define NV076_POINT_12D4_Y                                         31:16
#define NV076_COLOR(a)                                             (0x00000400\
                                                                   +(a)*0x0004)


/* class NV04_SCALED_IMAGE_FROM_MEMORY */
#define  NV04_SCALED_IMAGE_FROM_MEMORY                             (0x00000077)
#define  NV4_SCALED_IMAGE_FROM_MEMORY                              (0x00000077)
/* NvNotification[] elements */
#define NV077_NOTIFIERS_NOTIFY                                     (0)
/* NvNotification[] fields and values */
#define NV077_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV077_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV077_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV077_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV077_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV077_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NV077_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA_TO_MEMORY        0180-0183*/
 V032 SetContextDmaImage;      /* NV01_CONTEXT_DMA_FROM_MEMORY      0184-0187*/
 V032 SetContextPattern;       /* NV04_CONTEXT_PATTERN              0188-018b*/
 V032 SetContextRop;           /* NV03_CONTEXT_ROP                  018c-018f*/
 V032 SetContextBeta1;         /* NV01_BETA_SOLID                   0190-0193*/
 V032 SetContextBeta4;         /* NV04_CONTEXT_BETA                 0194-0197*/
 V032 SetContextSurface;       /* NV04_CONTEXT_SURFACES_2D,SWIZZLED 0198-019b*/
 V032 Reserved01[0x059];
 V032 SetColorFormat;          /* NV077_SET_COLOR_FORMAT_*          0300-0303*/
 V032 SetOperation;            /* NV077_SET_OPERATION_*             0304-0307*/
 V032 ClipPoint;               /* y_x S16_S16                       0308-030b*/
 V032 ClipSize;                /* height_width U16_U16              030c-030f*/
 V032 ImageOutPoint;           /* y_x S16_S16                       0310-0313*/
 V032 ImageOutSize;            /* height_width U16_U16              0314-0317*/
 V032 DeltaDuDx;               /* S12d20 ratio du/dx                0318-031b*/
 V032 DeltaDvDy;               /* S12d20 ratio dv/dy                031c-031f*/
 V032 Reserved02[0x038];
 V032 ImageInSize;             /* height_width U16_U16              0400-0403*/
 U032 ImageInFormat;           /* interpolator_origin_pitch         0404-0407*/
 U032 ImageInOffset;           /* bytes                             0408-040b*/
 V032 ImageInPoint;            /* v_u U12d4_U12d4                   040c-040f*/
 V032 Reserved03[0x6fc];
} Nv077Typedef, Nv04ScaledImageFromMemory;
#define Nv4ScaledImageFromMemory                      Nv04ScaledImageFromMemory
#define nv4ScaledImageFromMemory                      nv04ScaledImageFromMemory
#define NV077_TYPEDEF                                 nv04ScaledImageFromMemory
/* dma method offsets, fields, and values */
#define NV077_SET_OBJECT                                           (0x00000000)
#define NV077_NO_OPERATION                                         (0x00000100)
#define NV077_NOTIFY                                               (0x00000104)
#define NV077_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV077_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV077_SET_CONTEXT_DMA_NOTIFIES                             (0x00000180)
#define NV077_SET_CONTEXT_DMA_IMAGE                                (0x00000184)
#define NV077_SET_CONTEXT_PATTERN                                  (0x00000188)
#define NV077_SET_CONTEXT_ROP                                      (0x0000018C)
#define NV077_SET_CONTEXT_BETA1                                    (0x00000190)
#define NV077_SET_CONTEXT_BETA4                                    (0x00000194)
#define NV077_SET_CONTEXT_SURFACE                                  (0x00000198)
#define NV077_SET_COLOR_FORMAT                                     (0x00000300)
#define NV077_SET_COLOR_FORMAT_LE_A1R5G5B5                         (0x00000001)
#define NV077_SET_COLOR_FORMAT_LE_X1R5G5B5                         (0x00000002)
#define NV077_SET_COLOR_FORMAT_LE_A8R8G8B8                         (0x00000003)
#define NV077_SET_COLOR_FORMAT_LE_X8R8G8B8                         (0x00000004)
#define NV077_SET_COLOR_FORMAT_LE_V8YB8U8YA8                       (0x00000005)
#define NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8                       (0x00000006)
#define NV077_SET_COLOR_FORMAT_LE_R5G6B5                           (0x00000007)
#define NV077_SET_OPERATION                                        (0x00000304)
#define NV077_SET_OPERATION_SRCCOPY_AND                            (0x00000000)
#define NV077_SET_OPERATION_ROP_AND                                (0x00000001)
#define NV077_SET_OPERATION_BLEND_AND                              (0x00000002)
#define NV077_SET_OPERATION_SRCCOPY                                (0x00000003)
#define NV077_SET_OPERATION_SRCCOPY_PREMULT                        (0x00000004)
#define NV077_SET_OPERATION_BLEND_PREMULT                          (0x00000005)
#define NV077_CLIP_POINT                                           (0x00000308)
#define NV077_CLIP_POINT_X                                         15:0
#define NV077_CLIP_POINT_Y                                         31:16
#define NV077_CLIP_SIZE                                            (0x0000030C)
#define NV077_CLIP_SIZE_WIDTH                                      15:0
#define NV077_CLIP_SIZE_HEIGHT                                     31:16
#define NV077_IMAGE_OUT_POINT                                      (0x00000310)
#define NV077_IMAGE_OUT_POINT_X                                    15:0
#define NV077_IMAGE_OUT_POINT_Y                                    31:16
#define NV077_IMAGE_OUT_SIZE                                       (0x00000314)
#define NV077_IMAGE_OUT_SIZE_WIDTH                                 15:0
#define NV077_IMAGE_OUT_SIZE_HEIGHT                                31:16
#define NV077_DELTA_DU_DX                                          (0x00000318)
#define NV077_DELTA_DV_DY                                          (0x0000031C)
#define NV077_IMAGE_IN_SIZE                                        (0x00000400)
#define NV077_IMAGE_IN_SIZE_WIDTH                                  15:0
#define NV077_IMAGE_IN_SIZE_HEIGHT                                 31:16
#define NV077_IMAGE_IN_FORMAT                                      (0x00000404)
#define NV077_IMAGE_IN_FORMAT_PITCH                                15:0
#define NV077_IMAGE_IN_FORMAT_ORIGIN                               23:16
#define NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER                        (0x00000001)
#define NV077_IMAGE_IN_FORMAT_ORIGIN_CORNER                        (0x00000002)
#define NV077_IMAGE_IN_FORMAT_INTERPOLATOR                         31:24
#define NV077_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH                     (0x00000000)
#define NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH                     (0x00000001)
#define NV077_IMAGE_IN_OFFSET                                      (0x00000408)
#define NV077_IMAGE_IN                                             (0x0000040C)
#define NV077_IMAGE_IN_POINT_U                                     15:0
#define NV077_IMAGE_IN_POINT_V                                     31:16

typedef V032 Nv078Typedef;

typedef V032 Nv079Typedef;

typedef V032 Nv07aTypedef;

typedef V032 Nv07bTypedef;

typedef V032 Nv07cTypedef;

typedef V032 Nv07dTypedef;

typedef V032 Nv07eTypedef;

typedef V032 Nv07fTypedef;


/* class NV01_DEVICE_0 */
#define  NV01_DEVICE_0                                             (0x00000080)
/* NvNotification[] fields and values */
#define NV080_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv080Typedef, Nv01Device0;
#define  NV080_TYPEDEF                                             nv01Device0


/* class NV01_DEVICE_1 */
#define  NV01_DEVICE_1                                             (0x00000081)
/* NvNotification[] fields and values */
#define NV081_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv081Typedef, Nv01Device1;
#define  NV081_TYPEDEF                                             nv01Device1


/* class NV01_DEVICE_2 */
#define  NV01_DEVICE_2                                             (0x00000082)
/* NvNotification[] fields and values */
#define NV082_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv082Typedef, Nv01Device2;
#define  NV082_TYPEDEF                                             nv01Device2


/* class NV01_DEVICE_3 */
#define  NV01_DEVICE_3                                             (0x00000083)
/* NvNotification[] fields and values */
#define NV083_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv083Typedef, Nv01Device3;
#define  NV083_TYPEDEF                                             nv01Device3


/* class NV01_DEVICE_4 */
#define  NV01_DEVICE_4                                             (0x00000084)
/* NvNotification[] fields and values */
#define NV084_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv084Typedef, Nv01Device4;
#define  NV084_TYPEDEF                                             nv01Device4


/* class NV01_DEVICE_5 */
#define  NV01_DEVICE_5                                             (0x00000085)
/* NvNotification[] fields and values */
#define NV085_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv085Typedef, Nv01Device5;
#define  NV085_TYPEDEF                                             nv01Device5


/* class NV01_DEVICE_6 */
#define  NV01_DEVICE_6                                             (0x00000086)
/* NvNotification[] fields and values */
#define NV086_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv086Typedef, Nv01Device6;
#define  NV086_TYPEDEF                                             nv01Device6


/* class NV01_DEVICE_7 */
#define  NV01_DEVICE_7                                             (0x00000087)
/* NvNotification[] fields and values */
#define NV087_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv087Typedef, Nv01Device7;
#define  NV087_TYPEDEF                                             nv01Device7



 /***************************************************************************\
|*                                 Channels                                  *|
 \***************************************************************************/

typedef V032 Nv068Typedef;

typedef V032 Nv069Typedef;


/* class NV03_CHANNEL_PIO */
#define  NV03_CHANNEL_PIO                                          (0x0000006A)
#define  NV3_CHANNEL_PIO                                           (0x0000006A)
/* NvNotification[] fields and values */
#define NV06A_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV06A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV06A_NOTIFICATION_STATUS_ERROR_FLOW_CONTROL               (0x0200)
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
  NvClass                       nvClass;
  NvContextDmaFromMemory        contextDmaFromMemory;
  NvContextDmaToMemory          contextDmaToMemory;
  NvContextDmaInMemory          contextDmaInMemory;
  NvPatchcordVideo              patchcordVideo;
  NvVideoSink                   videoSink;
  NvVideoColormap               videoColormap;
  NvVideoFromMemory             videoFromMemory;
  NvVideoScaler                 videoScaler;
  NvVideoColorKey               videoColorKey;
  Nv000Typedef NV000_TYPEDEF;
  Nv001Typedef NV001_TYPEDEF;
  Nv002Typedef NV002_TYPEDEF;
  Nv003Typedef NV003_TYPEDEF;
  Nv004Typedef NV004_TYPEDEF;
  Nv005Typedef NV005_TYPEDEF;
  Nv006Typedef NV006_TYPEDEF;
  Nv007Typedef NV007_TYPEDEF;
  Nv008Typedef NV008_TYPEDEF;
  Nv009Typedef NV009_TYPEDEF;
  Nv00aTypedef NV00A_TYPEDEF;
  Nv00bTypedef NV00B_TYPEDEF;
  Nv00cTypedef NV00C_TYPEDEF;
  Nv00dTypedef NV00D_TYPEDEF;
  Nv00eTypedef NV00E_TYPEDEF;
  Nv00fTypedef NV00F_TYPEDEF;
  Nv010Typedef NV010_TYPEDEF;
  Nv011Typedef NV011_TYPEDEF;
  Nv012Typedef NV012_TYPEDEF;
  Nv013Typedef NV013_TYPEDEF;
  Nv014Typedef NV014_TYPEDEF;
  Nv015Typedef NV015_TYPEDEF;
  Nv016Typedef NV016_TYPEDEF;
  Nv017Typedef NV017_TYPEDEF;
  Nv018Typedef NV018_TYPEDEF;
  Nv019Typedef NV019_TYPEDEF;
  Nv01aTypedef NV01A_TYPEDEF;
  Nv01bTypedef NV01B_TYPEDEF;
  Nv01cTypedef NV01C_TYPEDEF;
  Nv01dTypedef NV01D_TYPEDEF;
  Nv01eTypedef NV01E_TYPEDEF;
  Nv01fTypedef NV01F_TYPEDEF;
  Nv020Typedef NV020_TYPEDEF;
  Nv021Typedef NV021_TYPEDEF;
  Nv022Typedef NV022_TYPEDEF;
  Nv023Typedef NV023_TYPEDEF;
  Nv024Typedef NV024_TYPEDEF;
  Nv025Typedef NV025_TYPEDEF;
  Nv026Typedef NV026_TYPEDEF;
  Nv027Typedef NV027_TYPEDEF;
  Nv028Typedef NV028_TYPEDEF;
  Nv029Typedef NV029_TYPEDEF;
  Nv02aTypedef NV02A_TYPEDEF;
  Nv02bTypedef NV02B_TYPEDEF;
  Nv02cTypedef NV02C_TYPEDEF;
  Nv02dTypedef NV02D_TYPEDEF;
  Nv02eTypedef NV02E_TYPEDEF;
  Nv02fTypedef NV02F_TYPEDEF;
  Nv030Typedef NV030_TYPEDEF;
  Nv031Typedef NV031_TYPEDEF;
  Nv032Typedef NV032_TYPEDEF;
  Nv033Typedef NV033_TYPEDEF;
  Nv034Typedef NV034_TYPEDEF;
  Nv035Typedef NV035_TYPEDEF;
  Nv036Typedef NV036_TYPEDEF;
  Nv037Typedef NV037_TYPEDEF;
  Nv038Typedef NV038_TYPEDEF;
  Nv039Typedef NV039_TYPEDEF;
  Nv03aTypedef NV03A_TYPEDEF;
  Nv03bTypedef NV03B_TYPEDEF;
  Nv03cTypedef NV03C_TYPEDEF;
  Nv03dTypedef NV03D_TYPEDEF;
  Nv03eTypedef NV03E_TYPEDEF;
  Nv03fTypedef NV03F_TYPEDEF;
  Nv040Typedef NV040_TYPEDEF;
  Nv041Typedef NV041_TYPEDEF;
  Nv042Typedef NV042_TYPEDEF;
  Nv043Typedef NV043_TYPEDEF;
  Nv044Typedef NV044_TYPEDEF;
  Nv045Typedef NV045_TYPEDEF;
  Nv046Typedef NV046_TYPEDEF;
  Nv047Typedef NV047_TYPEDEF;
  Nv048Typedef NV048_TYPEDEF;
  Nv049Typedef NV049_TYPEDEF;
  Nv04aTypedef NV04A_TYPEDEF;
  Nv04bTypedef NV04B_TYPEDEF;
  Nv04cTypedef NV04C_TYPEDEF;
  Nv04dTypedef NV04D_TYPEDEF;
  Nv04eTypedef NV04E_TYPEDEF;
  Nv04fTypedef NV04F_TYPEDEF;
  Nv050Typedef NV050_TYPEDEF;
  Nv051Typedef NV051_TYPEDEF;
  Nv052Typedef NV052_TYPEDEF;
  Nv053Typedef NV053_TYPEDEF;
  Nv054Typedef NV054_TYPEDEF;
  Nv055Typedef NV055_TYPEDEF;
  Nv056Typedef NV056_TYPEDEF;
  Nv057Typedef NV057_TYPEDEF;
  Nv058Typedef NV058_TYPEDEF;
  Nv059Typedef NV059_TYPEDEF;
  Nv05aTypedef NV05A_TYPEDEF;
  Nv05bTypedef NV05B_TYPEDEF;
  Nv05cTypedef NV05C_TYPEDEF;
  Nv05dTypedef NV05D_TYPEDEF;
  Nv05eTypedef NV05E_TYPEDEF;
  Nv05fTypedef NV05F_TYPEDEF;
  Nv060Typedef NV060_TYPEDEF;
  Nv061Typedef NV061_TYPEDEF;
  Nv062Typedef NV062_TYPEDEF;
  Nv063Typedef NV063_TYPEDEF;
  Nv064Typedef NV064_TYPEDEF;
  Nv065Typedef NV065_TYPEDEF;
  Nv066Typedef NV066_TYPEDEF;
  Nv067Typedef NV067_TYPEDEF;
/*Nv068Typedef NV068_TYPEDEF;*/
/*Nv069Typedef NV069_TYPEDEF;*/
/*Nv06aTypedef NV06A_TYPEDEF;*/
/*Nv06bTypedef NV06B_TYPEDEF;*/
/*Nv06cTypedef NV06C_TYPEDEF;*/
  Nv06dTypedef NV06D_TYPEDEF;
  Nv06eTypedef NV06E_TYPEDEF;
  Nv06fTypedef NV06F_TYPEDEF;
  Nv070Typedef NV070_TYPEDEF;
  Nv071Typedef NV071_TYPEDEF;
  Nv072Typedef NV072_TYPEDEF;
  Nv073Typedef NV073_TYPEDEF;
  Nv074Typedef NV074_TYPEDEF;
  Nv075Typedef NV075_TYPEDEF;
  Nv076Typedef NV076_TYPEDEF;
  Nv077Typedef NV077_TYPEDEF;
  Nv078Typedef NV078_TYPEDEF;
  Nv079Typedef NV079_TYPEDEF;
  Nv07aTypedef NV07A_TYPEDEF;
  Nv07bTypedef NV07B_TYPEDEF;
  Nv07cTypedef NV07C_TYPEDEF;
  Nv07dTypedef NV07D_TYPEDEF;
  Nv07eTypedef NV07E_TYPEDEF;
  Nv07fTypedef NV07F_TYPEDEF;
  Nv080Typedef NV080_TYPEDEF;
  Nv081Typedef NV081_TYPEDEF;
  Nv082Typedef NV082_TYPEDEF;
  Nv083Typedef NV083_TYPEDEF;
  Nv084Typedef NV084_TYPEDEF;
  Nv085Typedef NV085_TYPEDEF;
  Nv086Typedef NV086_TYPEDEF;
  Nv087Typedef NV087_TYPEDEF;
 } /* cls */;                  /* end of class methods                  -1fff*/
} Nv03SubchannelPio;
#define Nv3SubchannelPio                                 Nv03SubchannelPio
/* pio channel */
typedef volatile struct {      /* start of array of subchannels     0000-    */
 Nv03SubchannelPio subchannel[8];/*subchannel                       0000-1fff*/
} Nv03ChannelPio;              /* end of array of subchannels           -ffff*/
#define Nv3ChannelPio                                            Nv03ChannelPio
/* fields and values */
#define NV06A_FIFO_GUARANTEED_SIZE                                 (0x007C)
#define NV06A_FIFO_EMPTY                                           (0x007C)


/* class NV03_CHANNEL_DMA */
#define  NV03_CHANNEL_DMA                                          (0x0000006B)
/* NvNotification[] fields and values */
#define NV06B_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv06bTypedef, Nv03ChannelDma;
#define  NV06B_TYPEDEF                                           nv03ChannelDma


/* class NV04_CHANNEL_DMA */
#define  NV04_CHANNEL_DMA                                          (0x0000006C)
#define  NV4_CHANNEL_DMA                                           (0x0000006C)
/* NvNotification[] fields and values */
#define NV06C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV06C_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} Nv06cTypedef, Nv04ChannelDma;
#define  Nv4ChannelDma                                           Nv04ChannelDma
#define  nv4ChannelDma                                           nv04ChannelDma
#define  NV06C_TYPEDEF                                           nv04ChannelDma
/* pio flow control data structure */
typedef volatile struct {
 V032 reserved00[0x010];
 U032 Put;                     /* put offset, write only            0040-0043*/
 U032 Get;                     /* get offset, read only             0044-0047*/
 V032 reserved01[0x7EE];
} Nv04ControlDma;
#define  Nv4ControlDma                                           Nv04ControlDma



 /***************************************************************************\
|*                            Well Known Objects                             *|
 \***************************************************************************/


/* object NV01_NULL_OBJECT */
#define   NV01_NULL_OBJECT                                         (0x00000000)
#define   NV1_NULL_OBJECT                                          (0x00000000)



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



#ifdef __cplusplus
};
#endif
#endif /* NV32_INCLUDED */

