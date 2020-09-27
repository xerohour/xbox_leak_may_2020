
/***** Notification Status Values *****/
/*
 * NV sets NvNotification.status to NV_STATUS_DONE_OK if the operation
 * succeeded.
 */
#define NV_STATUS_DONE_OK          0



/* class NV_CONTEXT_ERROR_TO_MEMORY */
#define  NV_CONTEXT_ERROR_TO_MEMORY                      (0xFF5)
/* NvNotification[] elements */
#define NVFF5_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NVFF5_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NVFF5_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NVFF5_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NVFF5_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NVFF5_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NVFF5_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 SetNotify;               /* NVFF5_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0180-0183*/
 V032 Reserved01[0x05f];
 struct {                      /* start of method                   0300-    */
  V032 address[2];             /* address[1] is selector on X86        0-   7*/
  U032 limit;                  /* size of data region in bytes-1       8-   b*/
 } SetDmaSpecifier;            /* end of method                         -030b*/
 V032 Reserved02[0x73d];
} Nvff5Typedef, NvContextErrorToMemory;
#define NVFF5_TYPEDEF                                    NvContextErrorToMemory
/* dma method offsets, fields, and values */
#define NVFF5_SET_OBJECT                                 (0x00000000)
#define NVFF5_NO_OPERATION                               (0x00000100)
#define NVFF5_NOTIFY                                     (0x00000104)
#define NVFF5_NOTIFY_WRITE_ONLY                          (0x00000000)
#define NVFF5_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)
#define NVFF5_SET_CONTEXT_DMA_NOTIFIES                   (0x00000180)
#define NVFF5_SET_DMA_SPECIFIER                          (0x00000300)


/* class NV_VIDEO_COLOR_KEY */
#define  NV_VIDEO_COLOR_KEY                              (0xFF6)
/* NvNotification[] elements */
#define NVFF6_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NVFF6_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NVFF6_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NVFF6_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NVFF6_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NVFF6_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NVFF6_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 SetNotify;               /* NVFF6_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetNotifyCtxDma;         /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 Reserved01[0x01f];
 V032 SetVideoOutput;          /* NV1_PATCHCORD_VIDEO               0200-0203*/
 V032 SetVideoInput[3];        /* NV1_PATCHCORD_VIDEO               0204-020b*/
 V032 Reserved02[0x03c];
 V032 SetColorFormat;          /* NVFF6_SET_COLOR_FORMAT_*          0300-0303*/
 V032 SetColorKey;             /* color to compare with             0304-0307*/
 V032 SetPoint;                /* y_x, S16_S16 in pixels, top-left  0308-030b*/
 V032 SetSize;                 /* height_width U16_U16 in pixels    030c-030f*/
 V032 Reserved03[0x73c];
} Nvff6Typedef, NvVideoColorKey;
#define NVFF6_TYPEDEF                                    NvVideoColorKey
/* dma method offsets, fields, and values */
#define NVFF6_SET_OBJECT                                 (0x00000000)
#define NVFF6_NO_OPERATION                               (0x00000100)
#define NVFF6_NOTIFY                                     (0x00000104)
#define NVFF6_NOTIFY_WRITE_ONLY                          (0x00000000)
#define NVFF6_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)
#define NVFF6_SET_CONTEXT_DMA_NOTIFIES                   (0x00000180)
#define NVFF6_SET_VIDEO_OUTPUT                           (0x00000200)
#define NVFF6_SET_VIDEO_INPUT                            (0x00000204)
#define NVFF6_SET_COLOR_FORMAT                           (0x00000300)
#define NVFF6_SET_COLOR_FORMAT_LE_X16A8Y8                (0x00000001)
#define NVFF6_SET_COLOR_FORMAT_LE_X24Y8                  (0x00000002)
#define NVFF6_SET_COLOR_FORMAT_LE_X16A1R5G5B5            (0x00000003)
#define NVFF6_SET_COLOR_FORMAT_LE_X17R5G5B5              (0x00000004)
#define NVFF6_SET_COLOR_FORMAT_LE_A16R5G6B5              (0x00000005)
#define NVFF6_SET_COLOR_FORMAT_LE_A8R8G8B8               (0x00000006)
#define NVFF6_SET_COLOR_FORMAT_LE_X8R8G8B8               (0x00000007)
#define NVFF6_SET_COLOR_FORMAT_LE_A16Y16                 (0x00000008)
#define NVFF6_SET_COLOR_FORMAT_LE_X16Y16                 (0x00000009)
#define NVFF6_SET_COLOR_KEY                              (0x00000304)
#define NVFF6_SET_POINT                                  (0x00000308)
#define NVFF6_SET_POINT_X                                15:0
#define NVFF6_SET_POINT_Y                                31:16
#define NVFF6_SET_SIZE                                   (0x0000030C)
#define NVFF6_SET_SIZE_WIDTH                             15:0
#define NVFF6_SET_SIZE_HEIGHT                            31:16


/* class NV_VIDEO_SCALER */
#define  NV_VIDEO_SCALER                                 (0xFF7)
/* NvNotification[] elements */
#define NVFF7_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NVFF7_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NVFF7_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NVFF7_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NVFF7_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NVFF7_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NVFF7_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 SetNotify;               /* NVFF7_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetNotifyCtxDma;         /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 Reserved01[0x01f];
 V032 SetVideoOutput;          /* NV1_PATCHCORD_VIDEO               0200-0203*/
 V032 SetVideoInput;           /* NV1_PATCHCORD_VIDEO               0204-0207*/
 V032 Reserved02[0x03e];
 V032 SetDeltaDuDx;            /* S12d20 ratio du/dx                0300-0303*/
 V032 SetDeltaDvDy;            /* S12d20 ratio dv/dy                0304-0307*/
 V032 SetPoint;                /* y_x S16_S16 in pixels, top-left   0308-030b*/
 V032 Reserved03[0x73d];
} Nvff7Typedef, NvVideoScaler;
#define NVFF7_TYPEDEF                                    NvVideoScaler
/* dma method offsets, fields, and values */
#define NVFF7_SET_OBJECT                                 (0x00000000)
#define NVFF7_NO_OPERATION                               (0x00000100)
#define NVFF7_NOTIFY                                     (0x00000104)
#define NVFF7_NOTIFY_WRITE_ONLY                          (0x00000000)
#define NVFF7_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)
#define NVFF7_SET_CONTEXT_DMA_NOTIFIES                   (0x00000180)
#define NVFF7_SET_VIDEO_OUTPUT                           (0x00000200)
#define NVFF7_SET_VIDEO_INPUT                            (0x00000204)
#define NVFF7_SET_DELTA_DU_DX                            (0x00000300)
#define NVFF7_SET_DELTA_DV_DY                            (0x00000304)
#define NVFF7_SET_POINT                                  (0x00000308)
#define NVFF7_SET_POINT_X                                15:0
#define NVFF7_SET_POINT_Y                                31:16


/* class NV_VIDEO_FROM_MEMORY */
#define  NV_VIDEO_FROM_MEMORY                            (0xFF8)
/* NvNotification[] elements */
#define NVFF8_NOTIFIERS_NOTIFY                           (0)
#define NVFF8_NOTIFIERS_IMAGE_SCAN(b)                    (1+(b))
#define NVFF8_NOTIFIERS_GET_OFFSET_NOTIFY(b)             (3+(b))
/* NvNotification[] fields and values */
#define NVFF8_NOTIFICATION_INFO16_BUFFER_NOT_STARTED     (0x0001)
#define NVFF8_NOTIFICATION_INFO16_BUFFER_OFFSET_VALID    (0x0002)
#define NVFF8_NOTIFICATION_INFO16_BUFFER_DONE            (0x0003)
#define NVFF8_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NVFF8_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NVFF8_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NVFF8_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NVFF8_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NVFF8_NOTIFICATION_STATUS_ERROR_BAD_PATCH        (0x0400)
#define NVFF8_NOTIFICATION_STATUS_WARNING_INVALID_DATA   (0x0001)
#define NVFF8_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 SetNotify;               /* NVFF8_NOTIFY_*                    0104-0107*/
 V032 StopTransfer;            /* NVFF8_STOP_TRANSFER_VALUE         0108-010b*/
 V032 SetPatch;                /* NVFF8_SET_PATCH_*                 010c-010f*/
 V032 Reserved00[0x01c];
 V032 SetImageNotifyCtxDma;    /* NV1_CONTEXT_DMA_TO_MEMORY         0180-0183*/
 V032 SetImageCtxDma[2];       /* NV1_CONTEXT_DMA_FROM_MEMORY       0184-018b*/
 V032 Reserved01[0x01d];
 V032 SetVideoOutput;          /* NV1_PATCHCORD_VIDEO               0200-0203*/
 V032 Reserved02[0x03f];
 struct {                      /* start of methods in array         0300-    */
  U032 offset;                 /* byte offset of top-left pixel       00-  03*/
  U032 pitch;                  /* bytes, vertical pixel delta         04-  07*/
  V032 size;                   /* height_width U16_U16 in pixels      08-  0b*/
  V032 format;                 /* field_color V16_V16                 0c-  0f*/
  V032 notify;                 /* NVFF8_IMAGE_SCAN_NOTIFY_*           10-  13*/
 } ImageScan[2];               /* end of methods in array               -0327*/
 V032 GetOffsetNotify[2];      /* NVFF8_GET_OFFSET_NOTIFY_*         0328-032f*/
 V032 Reserved03[0x734];
} Nvff8Typedef, NvVideoFromMemory;
#define NVFF8_TYPEDEF                                    NvVideoFromMemory
/* dma method offsets, fields, and values */
#define NVFF8_SET_OBJECT                                 (0x00000000)
#define NVFF8_NO_OPERATION                               (0x00000100)
#define NVFF8_NOTIFY                                     (0x00000104)
#define NVFF8_NOTIFY_WRITE_ONLY                          (0x00000000)
#define NVFF8_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)
#define NVFF8_STOP_TRANSFER                              (0x00000108)
#define NVFF8_STOP_TRANSFER_VALUE                        (0x00000000)
#define NVFF8_SET_PATCH                                  (0x0000010C)
#define NVFF8_SET_PATCH_INVALIDATE                       (0x00000000)
#define NVFF8_SET_PATCH_VALIDATE                         (0x00000001)
#define NVFF8_SET_CONTEXT_DMA_NOTIFIES                   (0x00000180)
#define NVFF8_SET_CONTEXT_DMA_IMAGE                      (0x00000184)
#define NVFF8_SET_VIDEO_OUTPUT                           (0x00000200)
#define NVFF8_IMAGE_SCAN                                 (0x00000300)
#define NVFF8_IMAGE_SCAN_OFFSET                          (0x00000300)
#define NVFF8_IMAGE_SCAN_PITCH                           (0x00000304)
#define NVFF8_IMAGE_SCAN_SIZE                            (0x00000308)
#define NVFF8_IMAGE_SCAN_SIZE_WIDTH                      15:0
#define NVFF8_IMAGE_SCAN_SIZE_HEIGHT                     31:16
#define NVFF8_IMAGE_SCAN_FORMAT                          (0x0000030C)
#define NVFF8_IMAGE_SCAN_FORMAT_COLOR                    15:0
#define NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_Y8              (0x00000001)
#define NV_VFM_FORMAT_COLOR_LE_Y8_P4                      0x00000001
#define NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_V8YB8U8YA8      (0x00000002)
#define NV_VFM_FORMAT_COLOR_LE_V8Y8U8Y8                   0x00000002
#define NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_YB8V8YA8U8      (0x00000003)
#define NV_VFM_FORMAT_COLOR_LE_Y8V8Y8U8                   0x00000003
#define NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_GY1R5G5B5       (0x00000004)
#define NV_VFM_FORMAT_COLOR_LE_X1R5G5B5_P2                0x00000004
#define NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_R5G6B5          (0x00000005)
#define NV_VFM_FORMAT_COLOR_LE_R5G6B5_P2                  0x00000005
#define NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_GY1X7R8G8B8     (0x00000006)
#define NV_VFM_FORMAT_COLOR_LE_X8R8G8B8                   0x00000006
#define NVFF8_IMAGE_SCAN_FORMAT_FIELD                    31:16
#define NVFF8_IMAGE_SCAN_FORMAT_FIELD_PROGRESSIVE        (0x00000001)
#define NVFF8_IMAGE_SCAN_FORMAT_FIELD_EVEN_FIELD         (0x00000002)
#define NV_VFM_FORMAT_NEXT_FIELD_EVEN                     0x00000002
#define NVFF8_IMAGE_SCAN_FORMAT_FIELD_ODD_FIELD          (0x00000003)
#define NV_VFM_FORMAT_NEXT_FIELD_ODD                      0x00000003
#define NVFF8_IMAGE_SCAN_SET_NOTIFY                      (0x00000310)
#define NVFF8_IMAGE_SCAN_NOTIFY                          31:0
#define NVFF8_IMAGE_SCAN_NOTIFY_WRITE_ONLY               (0x00000000)
#define NVFF8_IMAGE_SCAN_NOTIFY_WRITE_THEN_AWAKEN        (0x00000001)
#define NVFF8_GET_OFFSET_NOTIFY                          (0x00000328)
#define NVFF8_GET_OFFSET_NOTIFY_WRITE_ONLY               (0x00000000)
#define NVFF8_GET_OFFSET_NOTIFY_WRITE_THEN_AWAKEN        (0x00000001)
#define NVFF8_SET_VIDEO_OUTPUT_PARAM(s)                  (0x00040200+8192*(s))
#define NVFF8_SET_CONTEXT_DMA_NOTIFIES_PARAM(s)          (0x00040180+8192*(s))
#define NVFF8_IMAGE_SCAN_PARAM(s,b)                      (0x00140300+8192*(s)+\
                                                                     20*(b))


/* class NV_VIDEO_COLORMAP */
#define  NV_VIDEO_COLORMAP                               (0xFF9)
/* NvNotification[] elements */
#define NVFF9_NOTIFIERS_NOTIFY                           (0)
#define NVFF9_NOTIFIERS_COLORMAP_DIRTY_NOTIFY            (1)
/* NvNotification[] fields and values */
#define NVFF9_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NVFF9_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NVFF9_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NVFF9_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NVFF9_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NVFF9_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 SetNotify;               /* NVFF9_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0180-0183*/
 V032 SetColormapCtxDma;       /* NV_CONTEXT_DMA_FROM_MEMORY        0184-0187*/
 V032 Reserved01[0x01e];
 V032 SetVideoOutput;          /* NV1_PATCHCORD_VIDEO               0200-0203*/
 V032 SetVideoInput;           /* NV1_PATCHCORD_VIDEO               0204-0207*/
 V032 Reserved02[0x040];
 U032 SetColormapStart;        /* offset in bytes                   0308-030b*/
 U032 SetColormapLength;       /* in bytes                          030c-030f*/
 U032 ColormapDirtyStart;      /* start of region to update         0310-0313*/
 U032 ColormapDirtyLength;     /* in bytes, starts transfer         0314-0317*/
 V032 Reserved03[0x73a];
} Nvff9Typedef, NvVideoColormap;
#define NVFF9_TYPEDEF                                    NvVideoColormap
/* dma method offsets, fields, and values */
#define NVFF9_SET_OBJECT                                 (0x00000000)
#define NVFF9_NO_OPERATION                               (0x00000100)
#define NVFF9_NOTIFY                                     (0x00000104)
#define NVFF9_NOTIFY_WRITE_ONLY                          (0x00000000)
#define NVFF9_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)
#define NVFF9_SET_CONTEXT_DMA_NOTIFIES                   (0x00000180)
#define NVFF9_SET_CONTEXT_DMA_COLORMAP                   (0x00000184)
#define NVFF9_SET_VIDEO_OUTPUT                           (0x00000200)
#define NVFF9_SET_VIDEO_INPUT                            (0x00000204)
#define NVFF9_SET_COLORMAP_START                         (0x00000308)
#define NVFF9_SET_COLORMAP_LENGTH                        (0x0000030C)
#define NVFF9_COLORMAP_DIRTY_START                       (0x00000310)
#define NVFF9_COLORMAP_DIRTY_LENGTH                      (0x00000314)
#define NVFF9_SET_VIDEO_OUTPUT_PARAM(s)                 (0x00040200+8192*(s))
#define NVFF9_SET_CONTEXT_DMA_NOTIFIES_PARAM(s)         (0x00040180+8192*(s))
#define NVFF9_SET_COLORMAP_START_PARAM(s)               (0x00040308+8192*(s))


/* class NV_VIDEO_SINK */
#define  NV_VIDEO_SINK                                   (0xFFA)
/* NvNotification[] elements */
#define NVFFA_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NVFFA_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NVFFA_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NVFFA_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NVFFA_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NVFFA_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NVFFA_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 SetNotify;               /* NVFFA_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0180-0183*/
 V032 Reserved01[0x01f];
 V032 SetVideoInput[64];       /* NV1_PATCHCORD_VIDEO               0200-02ff*/
 V032 Reserved02[0x740];
} NvffaTypedef, NvVideoSink;
#define NVFFA_TYPEDEF                                    NvVideoSink
/* dma method offsets, fields, and values */
#define NVFFA_SET_OBJECT                                 (0x00000000)
#define NVFFA_NO_OPERATION                               (0x00000100)
#define NVFFA_NOTIFY                                     (0x00000104)
#define NVFFA_NOTIFY_WRITE_ONLY                          (0x00000000)
#define NVFFA_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)
#define NVFFA_SET_CONTEXT_DMA_NOTIFIES                   (0x00000180)
#define NVFFA_SET_VIDEO_INPUT                            (0x00000200)


/* class NV_PATCHCORD_VIDEO */
#define  NV_PATCHCORD_VIDEO                              (0xFFB)
/* NvNotification[] entries, values, errors, warnings */
#define NVFFB_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
/* pio method data structure */
typedef volatile struct {
 V032 Reserved00[0x7c0];
} NvffbTypedef, NvPatchcordVideo;
#define NVFFB_TYPEDEF                                    NvPatchcordVideo


/* class NV_CONTEXT_DMA_IN_MEMORY */
#define  NV_CONTEXT_DMA_IN_MEMORY                        (0xFFC)
/* NvNotification[] elements */
#define NVFFC_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NVFFC_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NVFFC_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NVFFC_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NVFFC_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NVFFC_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NVFFC_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 SetNotify;               /* NVFFC_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0180-0183*/
 V032 Reserved01[0x05f];
 struct {                      /* start of method                   0300-    */
  V032 address[2];             /* On X86 address[1] is the selector    0-   7*/
  U032 limit;                  /* size of data region in bytes-1       8-   b*/
 } SetDmaSpecifier;            /* end of method                         -030b*/
 V032 Reserved02[0x73d];
} NvffcTypedef, NvContextDmaInMemory;
#define NVFFC_TYPEDEF                                    NvContextDmaInMemory
/* dma method offsets, fields, and values */
#define NVFFC_SET_OBJECT                                 (0x00000000)
#define NVFFC_NO_OPERATION                               (0x00000100)
#define NVFFC_NOTIFY                                     (0x00000104)
#define NVFFC_NOTIFY_WRITE_ONLY                          (0x00000000)
#define NVFFC_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)
#define NVFFC_SET_CONTEXT_DMA_NOTIFIES                   (0x00000180)
#define NVFFC_SET_DMA_SPECIFIER                          (0x00000300)


/* class NV_CONTEXT_DMA_TO_MEMORY */
#define  NV_CONTEXT_DMA_TO_MEMORY                        (0xFFD)
/* NvNotification[] elements */
#define NVFFD_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NVFFD_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NVFFD_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NVFFD_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NVFFD_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NVFFD_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NVFFD_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 SetNotify;               /* NVFFD_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0180-0183*/
 V032 Reserved01[0x05f];
 struct {                      /* start of method                   0300-    */
  V032 address[2];             /* address[1] is selector on X86        0-   7*/
  U032 limit;                  /* size of data region in bytes-1       8-   b*/
 } SetDmaSpecifier;            /* end of method                         -030b*/
 V032 Reserved02[0x73d];
} NvffdTypedef, NvContextDmaToMemory;
#define NVFFD_TYPEDEF                                    NvContextDmaToMemory
/* dma method offsets, fields, and values */
#define NVFFD_SET_OBJECT                                 (0x00000000)
#define NVFFD_NO_OPERATION                               (0x00000100)
#define NVFFD_NOTIFY                                     (0x00000104)
#define NVFFD_NOTIFY_WRITE_ONLY                          (0x00000000)
#define NVFFD_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)
#define NVFFD_SET_CONTEXT_DMA_NOTIFIES                   (0x00000180)
#define NVFFD_SET_DMA_SPECIFIER                          (0x00000300)


/* class NV_CONTEXT_DMA_FROM_MEMORY */
#define  NV_CONTEXT_DMA_FROM_MEMORY                      (0xFFE)
/* NvNotification[] elements */
#define NVFFE_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NVFFE_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NVFFE_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NVFFE_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NVFFE_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NVFFE_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NVFFE_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 SetNotify;               /* NVFFE_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetNotifyCtxDma;         /* NV_CONTEXT_DMA_TO_MEMORY          0180-0183*/
 V032 Reserved01[0x05f];
 struct {                      /* start of method                   0300-    */
  V032 address[2];             /* address[1] is selector on X86        0-   7*/
  U032 limit;                  /* size of data region in bytes-1       8-   b*/
 } SetDmaSpecifier;            /* end of method                         -030b*/
 V032 Reserved02[0x73d];
} NvffeTypedef, NvContextDmaFromMemory;
#define NVFFE_TYPEDEF                                    NvContextDmaFromMemory
/* dma method offsets, fields, and values */
#define NVFFE_SET_OBJECT                                 (0x00000000)
#define NVFFE_NO_OPERATION                               (0x00000100)
#define NVFFE_NOTIFY                                     (0x00000104)
#define NVFFE_NOTIFY_WRITE_ONLY                          (0x00000000)
#define NVFFE_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)
#define NVFFE_SET_CONTEXT_DMA_NOTIFIES                   (0x00000180)
#define NVFFE_SET_DMA_SPECIFIER                          (0x00000300)
#define NVFFE_SET_DMA_SPECIFIER_LIMIT                    (0x00000308)


/* class NV_CLASS */
#define  NV_CLASS                                        (0xFFF)
/* NvNotification[] elements */
#define NVFFF_NOTIFIERS_NOTIFY                           (0)
/* NvNotification[] fields and values */
#define NVFFF_NOTIFICATION_STATUS_IN_PROGRESS            (0x8000)
#define NVFFF_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT (0x4000)
#define NVFFF_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT     (0x2000)
#define NVFFF_NOTIFICATION_STATUS_ERROR_INVALID_STATE    (0x1000)
#define NVFFF_NOTIFICATION_STATUS_ERROR_STATE_IN_USE     (0x0800)
#define NVFFF_NOTIFICATION_STATUS_WARNING_UNAVAILABLE    (0x0002)
#define NVFFF_NOTIFICATION_STATUS_DONE_SUCCESS           (0x0000)
/* pio method data structure */
typedef volatile struct {
 V032 NoOperation;             /* ignored                           0100-0103*/
 V032 Notify;                  /* NVFFF_NOTIFY_*                    0104-0107*/
 V032 Reserved00[0x01e];
 V032 SetContextDmaNotifies;   /* NV_CONTEXT_DMA_TO_MEMORY          0180-0183*/
 V032 Reserved01[0x05f];
 V032 Create;                  /* 32-bit handle of new object       0300-0303*/
 V032 Destroy;                 /* 32-bit handle of object           0304-0307*/
 V032 Reserved02[0x73e];
} NvfffTypedef, NvClass;
#define NVFFF_TYPEDEF                                    NvClass
/* dma method offsets, fields, and values */
#define NVFFF_SET_OBJECT                                 (0x00000000)
#define NVFFF_NO_OPERATION                               (0x00000100)
#define NVFFF_NOTIFY                                     (0x00000104)
#define NVFFF_NOTIFY_WRITE_ONLY                          (0x00000000)
#define NVFFF_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)
#define NVFFF_SET_CONTEXT_DMA_NOTIFIES                   (0x00000180)
#define NVFFF_CREATE_OFFSET                              (0x00000300)
#define NVFFF_DESTROY_OFFSET                             (0x00000304)
#define NVFFF_CREATE(s)                                  (0x00000300+8192*(s))
#define NVFFF_CREATE_PARAM(s)                            (0x00040300+8192*(s))
#define NVFFF_DESTROY(s)                                 (0x00000304+8192*(s))

