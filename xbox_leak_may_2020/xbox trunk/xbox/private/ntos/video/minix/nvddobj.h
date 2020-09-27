;/*
;***************************************************************************
;                                                                           *
;   Copyright (C) 1995, 1998 NVidia Corporation. All Rights Reserved.       *
;                                                                           *
;***************************************************************************
;*/


#if defined NV10 || defined NV4
#define NV_IN_PROGRESS          0x8000
#else   /* !(NV10 || NV4) */
#define NV_IN_PROGRESS          0xFF
#endif  /* !(NV10 || NV4) */

#define NV_DD_COMMON_DMA_BUFFER_SIZE    32768
// win9x driver does not use these values anymore
#define NV_DD_DMA_PUSH_BUFFER_SIZE      (2*1024*1024)
#define NV_DD_DMA_PUSH_BUFFER_SIZE_LRG  (4*1024*1024)

#define NV_VIDEO_NOTIFIER_BUFFER_SIZE   32 * sizeof(NvNotification)

#define NV_STATUS_DONE_OK          0

#if defined NV10 || defined NV4

#define videoColorKey                  NVFF6_TYPEDEF
#define videoScaler                    NVFF7_TYPEDEF
#define videoFromMemory                NVFF8_TYPEDEF
#define videoColormap                  NVFF9_TYPEDEF
#define videoSink                      NVFFA_TYPEDEF
#define patchcordVideo                 NVFFB_TYPEDEF
#define contextDmaInMemory             NVFFC_TYPEDEF
#define contextDmaToMemory             NVFFD_TYPEDEF
#define contextDmaFromMemory           NVFFE_TYPEDEF

#undef  NVFF8_SET_CONTEXT_DMA_NOTIFIES
#define NVFF8_SET_CONTEXT_DMA_NOTIFIES                   (0x00000180)
#undef  NVFF8_SET_VIDEO_OUTPUT
#define NVFF8_SET_VIDEO_OUTPUT                           (0x00000200)
#undef  NVFF8_IMAGE_SCAN_OFFSET
#define NVFF8_IMAGE_SCAN_OFFSET                          (0x00000300)
#undef  NVFF9_SET_VIDEO_OUTPUT
#define NVFF9_SET_VIDEO_OUTPUT                           (0x00000200)
#undef  NVFFA_SET_VIDEO_INPUT
#define NVFFA_SET_VIDEO_INPUT                            (0x00000200)
#undef  NVFFF_SET_CONTEXT_DMA_NOTIFIES
#define NVFFF_SET_CONTEXT_DMA_NOTIFIES                   (0x00000180)
#undef  NVFFF_CREATE_OFFSET
#define NVFFF_CREATE_OFFSET                              (0x00000300)
#undef  NVFFF_DESTROY_OFFSET
#define NVFFF_DESTROY_OFFSET                             (0x00000304)

                                        // default object on this sub-channel
#define NV_DD_ROP                  0    // NV_DD_CONTEXT_ROP - NV03_CONTEXT_ROP (0x043)
#define NV_DD_SURFACES             1    // NV_DD_SURFACES_2D - NV4/10_CONTEXT_SURFACES_2D (0x042/0x062)
#define NV_DD_ROP_RECT_AND_TEXT    2    // NV_DD_ROP_GDI_RECT_AND_TEXT - NV4_GDI_RECTANGLE_TEXT (0x04a)
#define NV_DD_BLIT                 3    // NV_DD_IMAGE_BLIT  - NV4_IMAGE_BLIT (0x05f)
#define NV_DD_IMAGE                4    // NV_DD_IMAGE_FROM_CPU - NV4_IMAGE_FROM_CPU (0x061)
#define NV_DD_STRETCH              5    // NV_DD_STRETCHED_IMAGE_FROM_CPU/or one of many NV_DD_SCALED_IMAGE_FROM_MEMORY - NV04/05_STRETCHED/SCALED_IMAGE_FROM_CPU/MEMORY (0x066/0x076)
#define NV_DD_CELSIUS              6    // D3D_CELSIUS_PRIMITIVE - NV10_CELSIUS_PRIMITIVE (0x056)
#ifdef WINNT    // Remove this when DX7 port is complete
#define NV_DD_TRANSCOLOR           6
#endif // WINNT
#define NV_DD_SPARE                7

/* Default cached objects in Windows 95 Direct Draw patch */
#define dDrawRop                       subchannel[NV_DD_ROP].nv3ContextRop
#define dDrawSurfaces                  subchannel[NV_DD_SURFACES].nv4ContextSurfaces2D
#define dDrawRopRectAndText            subchannel[NV_DD_ROP_RECT_AND_TEXT].nv4GdiRectangleText
#define dDrawBlit                      subchannel[NV_DD_BLIT].nv1ImageBlit
#define dDrawImageFromCpu              subchannel[NV_DD_IMAGE].nv1ImageFromCpu
#define dDrawStretch                   subchannel[NV_DD_STRETCH]
#define dDrawTransColor                subchannel[NV_DD_TRANSCOLOR].nv4ContextColorKey
#define dDrawSpare                     subchannel[NV_DD_SPARE]

#define dDrawSubchannelOffset(I)       (I * 0x00002000)

#define dVideoControlHdr               subchannel[0]
#define dVideoControl                  subchannel[0].Nv10VideoLutCursorDac
#define dVideoOverlayHdr               subchannel[1]
#define dVideoOverlay                  subchannel[1].Nv10VideoOverlay
#define dVideoDVDSubPictureHdr         subchannel[2]
#define dVideoDVDSubPicture            subchannel[2].Nv10DvdSubpicture
#define dVideoImageBlitHdr             subchannel[3]
#define dVideoImageBlit                subchannel[3].Nv04ImageBlit
#define dVideoSurface2DHdr             subchannel[4]
#define dVideoSurface2D                subchannel[4].Nv10ContextSurfaces2d


#define NV_DMA_PUSHER_JUMP             0x20000000

/* DMA pusher offsets */
#define SET_ROP_OFFSET                                          NV043_SET_ROP5
#define SURFACES_2D_SET_COLOR_FORMAT_OFFSET                     NV042_SET_COLOR_FORMAT
#define SURFACES_2D_PITCH_OFFSET                                NV042_SET_PITCH
#define SURFACES_2D_DEST_OFFSET                                 NV042_SET_OFFSET_DESTIN
#define IMAGE_BLACK_RECTANGLE_POINT_OFFSET                      NV019_SET_POINT
#define RECT_AND_TEXT_NO_OPERATION_OFFSET                       NV04A_NO_OPERATION
#define RECT_AND_TEXT_NOTIFY_OFFSET                             NV04A_NOTIFY
#define RECT_AND_TEXT_COLOR1A_OFFSET                            NV04A_COLOR1_A
#define RECT_AND_TEXT_CLIPPOINT0B_OFFSET                        NV04A_CLIP_POINT0_B
#define RECT_AND_TEXT_CLIPPEDRECT_POINT0_OFFSET                 NV04A_CLIPPED_RECTANGLE(0)
#define BLIT_POINT_IN_OFFSET                                    NV05F_CONTROL_POINT_IN
#define IMAGE_FROM_CPU_POINT_OFFSET                             NV061_POINT
#define IMAGE_FROM_CPU_COLOR_OFFSET                             NV061_COLOR(0)
#define STRETCHED_IMAGE_SIZE_IN_OFFSET                          NV076_SIZE_IN
#define STRETCHED_IMAGE_COLOR_OFFSET                            NV076_COLOR(0)
#define SCALED_IMAGE_NOTIFY_OFFSET                              NV077_NOTIFY
#define SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET                   NV077_SET_CONTEXT_DMA_IMAGE
#define SCALED_IMAGE_SET_COLOR_FORMAT                           NV077_SET_COLOR_FORMAT
#define SCALED_IMAGE_CLIPPOINT_OFFSET                           NV077_CLIP_POINT
#define SCALED_IMAGE_DELTADUDX_OFFSET                           NV077_DELTA_DU_DX
#define SCALED_IMAGE_IMAGE_OUT_POINT_OFFSET                     NV077_IMAGE_OUT_POINT
#define SCALED_IMAGE_IMAGE_IN_SIZE_OFFSET                       NV077_IMAGE_IN_SIZE
#define SCALED_IMAGE_IMAGE_IN_POINT_OFFSET                      NV077_IMAGE_IN
#define SET_TRANSCOLOR_OFFSET                                   NV057_SET_COLOR
#define MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET                      NV039_SET_CONTEXT_DMA_BUFFER_IN
#define MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET                     NV039_SET_CONTEXT_DMA_BUFFER_OUT
#define MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET                      NV039_OFFSET_IN
#define BETA4_SET_BETA_FACTOR_OFFSET                            NV072_SET_BETA_FACTOR
#define VIDEO_FROM_MEM_OFFSET_OFFSET                            NVFF8_IMAGE_SCAN_OFFSET
#define CONTEXT_DMA_FROM_MEM_NOTIFY_OFFSET                      NVFFE_NOTIFY
#define CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_OFFSET           NVFFE_SET_DMA_SPECIFIER
#define CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_LIMIT_OFFSET     NVFFE_SET_DMA_SPECIFIER_LIMIT


#else   /* !(NV10 || NV4) */

#define NV_DD_ROP                      0
#define NV_DD_IMAGE_IN_MEMORY          1
#define NV_DD_ROP_RECT_AND_TEXT        2
#define NV_DD_BLIT                     3
#define NV_DD_IMAGE                    4
#define NV_DD_STRETCH                  5
#define NV_DD_TRANSCOLOR               6
#define NV_DD_SPARE                    7

/* Default cached objects in Windows 95 Direct Draw patch */
#define dDrawRop                       subchannel[NV_DD_ROP].rop5Solid
#define dDrawImageInMemory             subchannel[NV_DD_IMAGE_IN_MEMORY].imageInMemory
#define dDrawRopRectAndText            subchannel[NV_DD_ROP_RECT_AND_TEXT].winRectAndTextFromCpu
#define dDrawBlit                      subchannel[NV_DD_BLIT].imageBlit
#define dDrawImageFromCpu              subchannel[NV_DD_IMAGE].imageFromCpu
#define dDrawStretch                   subchannel[NV_DD_STRETCH]
#define dDrawTransColor                subchannel[NV_DD_TRANSCOLOR].imageSolid
#define dDrawSpare                     subchannel[NV_DD_SPARE]

#endif  /* !(NV10 || NV4) */
/*
      7 subchannels contain the default cached objects.  These objects
are expected to be loaded at all times except when one or more of them gets
temporarily replaced for an unusual rendering operation.  If a default cached
object does get temporarily replaced, it is expected to be restored immediately
after completion of the rendering operation which caused it's replacement.
There is a spare subchannel in the Direct Draw patch which is lazy evaluated
similarly to the one used in the display driver.

        In the Windows 95 Direct Draw patch description below outlined objects are
those expected to be cached in the 8 subchannels.  Objects with corners
marked only are objects which temporarily replace cached objects or which are
loaded into the eighth subchannel when used.  All other objects are created,
patched together, then never loaded again until the patch is destroyed.  Most of
those objects are in the interior of the patch and are referenced by row for
clear identification.  Patchcord object names are derived from the object
where their output originates (their source).




/* Windows 95 Double Buffered Direct Draw ROP3 Patch */


/*

   Objects

   Context DMA  Context DMA
    To Memory    To Memory
          . |    | . .-------. .               . .         . .---------------.
           MemToMem  |  Rop  |   Image Pattern      Image    |  Image Solid  |
            Format   | Solid |  (Alpha Enabled)     Black    |(Alpha Enabled)|
          '        ' '-------' '               '  Rectangle  '---------------'
                           |        |            '         '         |
                          P|I       |              |                 |
                           |  .-----'     .--------'    .------------'
                           | P|I         P|I           P|I
 .               .         |  |           |             |
    (Sync) Rect     P      |  |           |             |       P
     And Text     ------------------------------------------------->.
 '               '  I      |  |           |             |       I   |
                           |  |           |             |           |
                          P|IP|I         P|I           P|I          |
 .---------------.         V  V           |             V           |
 |   Rop Rect    |  P   Image Rop  P      V     P     Image     P   |
 |   And Text    |----->   And   ------------------>Color Key------>|       1
 '---------------'  I   ^          I      |     I               I   |
                       P|I |  |           |             |           |
                        | P|IP|I         P|I           P|I          |
.----------------.         V  V           V             V           |
| Image From CPU |  P   Image Rop  P    Image   P     Image     P   |
|(Alpha Disabled)|----->   And   ----->Stencil----->Color Key------>|       2
'----------------'  I   ^          I            I               I   |
                       P|I |  |           |             |           |
                        | P|IP|I         P|I           P|I          |
.                .         V  V           V             V           |
  Image From CPU    P   Image Rop  P    Image   P     Image     P   |
 (Alt Pxl Depth)  ----->   And   ----->Stencil----->Color Key------>|       3
'                '  I   ^          I            I               I   |
 CtxDmaFromSysMem      P|I |  |           |             |           |
    |                   | P|IP|I         P|I           P|I          |
.---|------------.         V  V           |             V           |
|  Scaled RGB    |  P   Image Rop  P      V     P     Image     P   |
|ImageFromSysMem |----->   And   ------------------>Color Key------>|       4
|(Alpna Disabled)|  I   ^          I      |     I               I   |
'----------------'     P|I |  |           |             |           |
 CtxDmaFromVidMem       | P|IP|I         P|I           P|I          |
.          |     .         V  V           |             V           |
   Scaled RGB       P   Image Rop  P      V     P     Image     P   |
 ImageFromVidMem  ----->   And   ------------------>Color Key------>|       5
 (Alpna Disabled)   I   ^          I      |     I               I   |
'                '     P|I |  |           |             |           |
    |                   | P|IP|I         P|I           P|I          |
.   |            .      |  |  |           |             |           |
   Scaled UYVY      P   |  V  V    P      V     P       V       P   |
 ImageFromSysMem  ------------------------------------------------->|       6
 (Alpna Disabled)   I   ^  |  |    I      |     I       |       I   |
'                '     P|I |  |           |             |           |
           |            | P|IP|I         P|I           P|I          |
.          |     .         |  |           |             |           |
   Scaled UYVY      P   |  V  V    P      V     P       V       P   |
 ImageFromVidMem  ------------------------------------------------->|       7
 (Alpna Disabled)   I   ^  |  |    I      |     I       |       I   |
'                '     P|I |  |           |             |           |
    |                   | P|IP|I         P|I           P|I          |
.   |            .      |  |  |           |             |           |
   Scaled YUYV      P   |  V  V    P      V     P       V       P   |
 ImageFromSysMem  ------------------------------------------------->|       8
 (Alpna Disabled)   I   ^  |  |    I      |     I       |       I   |
'                '     P|I |  |           |             |           |
           |            | P|IP|I         P|I           P|I          |
.          |     .         |  |           |             |           |
   Scaled YUYV      P   |  V  V    P      V     P       V       P   |
 ImageFromVidMem  ------------------------------------------------->|       9
 (Alpna Disabled)   I   ^  |  |    I      |     I       |       I   |
'                '     P|I |  |           |             |           |
 CtxDmaFromSysMem       | P|IP|I         P|I           P|I          |
.       |        .      |  |  |           |             |           |
    Floating 0      P   |  V  V    P      V     P       V       P   |
   Scaled UYVY    ------------------------------------------------->|       A
 ImageFromSysMem    I   ^  |  |    I      |     I       |       I   |
'                '     P|I |  |           |             |           |
 CtxDmaFromSysMem       | P|IP|I         P|I           P|I          |
.       |        .         |  |           |             |           |
    Floating 1      P   |  V  V    P      V     P       V       P   |
   Scaled UYVY    ------------------------------------------------->|       B
 ImageFromSysMem    I   ^  |  |    I      |     I       |       I   |
'                '     P|I |  |           |             |           |
 CtxDmaFromSysMem       | P|IP|I         P|I           P|I          |
.       |        .      |  |  |           |             |           |
    Floating 0      P   |  V  V    P      V     P       V       P   |
   Scaled YUYV    ------------------------------------------------->|       C
 ImageFromSysMem    I   ^  |  |    I      |     I       |       I   |
'                '     P|I |  |           |             |           |
 CtxDmaFromSysMem       | P|IP|I         P|I           P|I          |
.       |        .         |  |           |             |           |
    Floating 1      P   |  V  V    P      V     P       V       P   |
   Scaled YUYV    ------------------------------------------------->|       D
 ImageFromSysMem    I   ^  |  |    I      |     I       |       I   |
'                '     P|I |  |           |             |           |
 CtxDmaFromSysMem       | P|IP|I         P|I           P|I          |
.       |        .      |  |  |           |             |           |
    Floating 0      P   |  V  V    P      V     P       V       P   |
   Scaled Y420    ------------------------------------------------->|       AA
 ImageFromSysMem    I   ^  |  |    I      |     I       |       I   |
'                '     P|I |  |           |             |           |
 CtxDmaFromSysMem       | P|IP|I         P|I           P|I          |
.       |        .         |  |           |             |           |
    Floating 1      P   |  V  V    P      V     P       V       P   |
   Scaled Y420    ------------------------------------------------->|       BB
 ImageFromSysMem    I   ^  |  |    I      |     I       |       I   |
'                '     P|I |  |           |             |           |
                        | P|IP|I         P|I           P|I          |
.                .         V  V           |             |           |
  Stretched UV      P   Image Rop  P      V     P       V       P   |
      Image       ----->   And   ---------------------------------->|       CC
 (Alpna Disabled)   I   ^          I      |     I       |       I   |
'                '     P|I |  |           |             |           |
                        | P|IP|I         P|I           P|I          |
.                .         V  V           |             V           |
    Stretched       P   Image Rop  P      V     P     Image     P   |
      Image       ----->   And   ------------------>Color Key------>|       DD
 (Alpna Disabled)   I   ^          I      |     I       |       I   |
'                '     P|I |  |           |             |           |
                        | P|IP|I         P|I           P|I          |
.----------------.         V  V           V             V           |
|   Image Blit   |  P   Image Rop  P    Image   P     Image     P   |
|(Alpha Disabled)|----->   And   ----->Stencil----->Color Key------>|       E
'------------^---'  I   ^          I            I               I   |
            P|I        P|I                                          |
             |          |                                           |
             |          |                                           |
             |          '------------------------------------.      |
             |                                               |      |
             |                                               |      |
             |                                              P|I    P|I
             |                                               |      |
            P|I                                              |      |
             |                                               |      |
             |                                               |      V
 .             .               .           .              .-------------.
     Source     --------------- Context DMA --------------| Destination |  P
  ImageInMemory                  InMemory                 |ImageInMemory|<----.
 '             '               '           '              '-------------'  I  |
                                  |    |                                      |
                                  |    '-------------------------.            |
                                  |                              |            |
 .              .          P      |                 P     .                .  |
  Render Solid   ----------------------------------------> Destination Zeta   |
  Zeta Rectangle           I      |                 I       ImageInMemory     |
 '              '                 |                       '                '  |
                                  |                                           |
                                  '------------------------------.            |
                                                                 |            |
                                                          .                .  |
 .             .      P                               P    Destination Zeta   |
   Render D3D   ----------------------------------------->  BufferInMemory    |
  Triangle Zeta       Z         P   Image    P        Z   '      ^         '  |
 '             '------------------>Stencil----------------------------------->| F
                                I            I                   |            |
                                                                P|Z           |
 .             .      P                               P          |            |
   Render D3D   -------------------------------------------------'            |
  Triangle Zeta       Z         P   Image    P        Z                       |
   From Video   ------------------>Stencil----------------------------------->' FF
     Memory                     I            I
 '             '

                            .                 .
  Floating Format               Y, U, V, UV
  Context DMA From --------- System Memory To  ---------------.
   System Memory             Vid Memory Format                |
                            '                 '               |
                                                       Context DMA To
                                                        Video Memory
                            .                 .               |
                                Y, U, V, UV                   |
         .------------------  Video Memory To  ---------------'
         |                   Vid Memory Format
 .                .         '                 '
  Context DMA From
    Video Memory
 ,                ,               .          .
       |    |                       Primary
       |    '--------------------- Video From ------Flip Primary Notifier
       |                             Memory         Context DMA To Memory
       |                          '          '
       |                            |      |
       |                            |      |
       |                           P|V    P|V
       |                            |      |
       |                            |      |   P   .            .
  .          .                      V      V//<---- Shared Video
     YUV422     P   .      .    P  .         . V      Colormap
   Video From -----> Video  ------>   Video        '            '
     Memory     V    Scaler     V   Color Key
  '          '      '      '       '         '
        |                               |
        |                               |
  Flip Overlay Notifier                P|V
  Context DMA To Memory                 |
                                        V
                                   .         .
                                      Video
                                      Sink
                                   '         '

*/


/* Video channel shared Display Driver and DDraw Object IDs */

#define NV_VIDEO_NOTIFIER_CONTEXT_DMA_TO_MEMORY                 0x00008000
#define NV_VIDEO_LUT_CURSOR_DAC_NOTIFIER_CONTEXT_DMA_TO_MEMORY  0x00008004
#define NV_VIDEO_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY         0x00008008
#define NV_VIDEO_CONTEXT_DMA_IN_MEMORY                          0x00008010
#define NV_VIDEO_LUT_CURSOR_DAC                                 0x00008020
#define NV_VIDEO_OVERLAY                                        0x00008030
#define NV_VIDEO_DVD_SUBPICTURE                                 0x00008040
#define NV_VIDEO_SURFACES_2D_A8R8G8B8                           0x00008050
#define NV_VIDEO_CONTEXT_BETA4                                  0x00008060
#define NV_VIDEO_SRCCOPY_PREMULT_A8R8G8B8_IMAGE_BLIT            0x00008070

/* Windows 9X Direct Draw Object IDs */


#define NV_DD_PIO_CONTEXT_DMA_FROM_SYSTEM_MEMORY                0xDD001000
#define NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY                0xDD001000
#define NV_DD_PIO_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY 0xDD001001
#define NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY 0xDD001001
#define NV_DD_PIO_CONTEXT_DMA_FROM_VIDEO_MEMORY                 0xDD001002
#define NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY                 0xDD001002
#define NV_DD_PIO_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY 0xDD001003
#define NV_DD_DMA_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY 0xDD001003
#define NV_DD_PIO_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY 0xDD001004
#define NV_DD_DMA_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY 0xDD001004
#define NV_DD_PIO_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY 0xDD001005
#define NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY 0xDD001005
#define NV_DD_PIO_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY 0xDD001006
#define NV_DD_DMA_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY 0xDD001006
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_OVERLAY_SHADOW        0xDD001007
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_OVERLAY_SHADOW        0xDD001007
#define NV_DD_DMA_TEXTURE_CONTEXT_DMA_FROM_SYSTEM_MEMORY        0xDD001009
#define NV_DD_DMA_FLOATING_TEXTURE_CONTEXT_DMA_FROM_SYSTEM_MEMORY 0xDD00100A
#define NV_DD_DMA_TRIANGLE_CONTEXT_DMA_FROM_SYSTEM_MEMORY       0xDD00100B
#define NV_DD_PIO_CONTEXT_DMA_IN_VIDEO_MEMORY                   0xDD001010
#define NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY                   0xDD001010
#define NV_DD_PIO_COMBINER_EVENT_NOTIFIER                       0xDD001014
#define NV_DD_DMA_COMBINER_EVENT_NOTIFIER                       0xDD001014
#define NV_DD_PIO_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY         0xDD001015
#define NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY         0xDD001015
#define NV_DD_PIO_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY   0xDD001016
#define NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY   0xDD001016
#define NV_DD_PIO_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY   0xDD001017
#define NV_DD_DMA_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY   0xDD001017
#define NV_DD_PIO_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY_0 0xDD001017
#define NV_DD_PIO_CONTEXT_DMA_TO_SYSTEM_MEMORY                  0xDD001018
#define NV_DD_DMA_CONTEXT_DMA_TO_SYSTEM_MEMORY                  0xDD001018
#define NV_DD_PIO_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY       0xDD001019
#define NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY       0xDD001019
#define NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY    0xDD00101A
#define NV_DD_PIO_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY         0xDD00101B
#define NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY         0xDD00101B
#define NV_DD_PIO_CONTEXT_DMA_TO_VIDEO_MEMORY                   0xDD00101C
#define NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY                   0xDD00101C
#define NV_DD_PIO_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY           0xDD00101D
#define NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY           0xDD00101D
#define NV_DD_PIO_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY_1 0xDD00101E
#define NV_DD_PIO_COLOUR_CONTROL_EVENT_NOTIFIER                 0xDD00101F
#define NV_DD_DMA_COLOUR_CONTROL_EVENT_NOTIFIER                 0xDD00101F


#define NV_DD_VIDEO_MEMORY_TO_SYSTEM_MEMORY_FORMAT              0xDD001020
#define NV_DD_CONTEXT_ROP                                       0xDD001030
#define NV_DD_ROP5_SOLID                                        0xDD001030
#define NV_DD_P_I_ROP5_SOLID                                    0xDD001031
#define NV_DD_BETA_SOLID                                        0xDD001040
#define NV_DD_P_B_BETA_SOLID                                    0xDD001041
#define NV_DD_CONTEXT_PATTERN                                   0xDD001050
#define NV_DD_IMAGE_PATTERN                                     0xDD001050
#define NV_DD_P_I_IMAGE_PATTERN                                 0xDD001051
#define NV_DD_IMAGE_BLACK_RECTANGLE                             0xDD001060
#define NV_DD_P_I_IMAGE_BLACK_RECTANGLE                         0xDD001061
#define NV_DD_CONTEXT_COLOR_KEY                                 0xDD001070
#define NV_DD_COLOR_KEY_IMAGE_SOLID                             0xDD001070
#define NV_DD_P_I_COLOR_KEY_IMAGE_SOLID                         0xDD001071
#define NV_DD_CONTEXT_BETA4                                     0xDD001080

#define NV_DD_ROP_GDI_RECT_AND_TEXT                             0xDD001100
#define NV_DD_P_I_ROP_GDI_RECT_AND_TEXT                         0xDD001101
#define NV_DD_ABLEND_RECT_AND_TEXT                              0xDD001108
#define NV_DD_PUSHER_SYNC_RECT_AND_TEXT_FROM_CPU                0xDD00110A
#define NV_DD_P_I_PUSHER_SYNC_RECT_AND_TEXT_FROM_CPU            0xDD00110B
#define NV_DD_IMAGE_FROM_CPU                                    0xDD001200
#define NV_DD_P_I_IMAGE_FROM_CPU                                0xDD001201
#define NV_DD_SRCCOPY_IMAGE_FROM_CPU                            0xDD001208
#define NV_DD_IMAGE_BLEND_FROM_CPU                              0xDD001300
#define NV_DD_P_I_IMAGE_BLEND_FROM_CPU                          0xDD001301
#define NV_DD_ABLEND_NOSRCALPHA_IMAGE_FROM_CPU                  0xDD001308
#define NV_DD_ABLEND_IMAGE_FROM_CPU                             0xDD001309
#define NV_DD_STRETCHED_IMAGE_FROM_CPU                          0xDD001400
#define NV_DD_P_I_STRETCHED_IMAGE_FROM_CPU                      0xDD001401
#define NV_DD_SRCCOPY_STRETCHED_IMAGE_FROM_CPU                  0xDD001408
#define NV_DD_ABLEND_NOSRCALPHA_STRETCHED_IMAGE_FROM_CPU        0xDD001409
#define NV_DD_ABLEND_STRETCHED_IMAGE_FROM_CPU                   0xDD00140A
#define NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY               0xDD001500
#define NV_DD_P_I_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY           0xDD001501
#define NV_DD_SRCCOPY_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY       0xDD001508
#define NV_DD_ABLEND_NOSRCALPHA_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY 0xDD001509
#define NV_DD_ABLEND_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY        0xDD00150A
#define NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY                0xDD001510
#define NV_DD_P_I_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY            0xDD001511
#define NV_DD_SRCCOPY_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY        0xDD001518
#define NV_DD_ABLEND_NOSRCALPHA_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY 0xDD001519
#define NV_DD_ABLEND_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY         0xDD00151A
#define NV_DD_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY              0xDD001520
#define NV_DD_P_I_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY          0xDD001521
#define NV_DD_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY               0xDD001530
#define NV_DD_P_I_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY           0xDD001531
#define NV_DD_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY              0xDD001540
#define NV_DD_P_I_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY          0xDD001541
#define NV_DD_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY               0xDD001550
#define NV_DD_P_I_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY           0xDD001551
#define NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY    0xDD001560
#define NV_DD_P_I_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY 0xDD001561
#define NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY    0xDD001570
#define NV_DD_P_I_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY 0xDD001571
#define NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY    0xDD001580
#define NV_DD_P_I_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY 0xDD001581
#define NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY    0xDD001590
#define NV_DD_P_I_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY 0xDD001591
#define NV_DD_GENERIC_SCALED_IMAGE                              0xDD0015A0
#define NV_DD_STRETCHED_UV_IMAGE_FROM_CPU                       0xDD001600
#define NV_DD_P_I_STRETCHED_UV_IMAGE_FROM_CPU                   0xDD001601
#define NV_DD_DVD_SUBPICTURE                                    0xDD001700

#define NV_DD_IMAGE_BLIT                                        0xDD001800
#define NV_DD_P_I_IMAGE_BLIT                                    0xDD001801
#define NV_DD_SRCCOPY_IMAGE_BLIT                                0xDD001808
#define NV_DD_SRCCOPY_PREMULT_A8R8G8B8_IMAGE_BLIT               0xDD00180A
#define NV_DD_ALT_IMAGE_FROM_CPU                                0xDD001900
#define NV_DD_P_I_ALT_IMAGE_FROM_CPU                            0xDD001901
#define NV_DD_RENDER_SOLID_ZETA_RECTANGLE                       0xDD001A00
#define NV_DD_P_I_RENDER_SOLID_ZETA_RECTANGLE                   0xDD001A01

#define NV_D3D_RENDER_TRIANGLE_ZETA                             0xDD001B00
#define NV_D3D_P_I_RENDER_TRIANGLE_ZETA                         0xDD001B01
#define NV_D3D_P_Z_RENDER_TRIANGLE_ZETA                         0xDD001B02
#define NV_D3D_RENDER_TRIANGLE_ZETA_FROM_VIDEO_MEMORY           0xDD001B03
#define NV_D3D_P_I_RENDER_TRIANGLE_ZETA_FROM_VIDEO_MEMORY       0xDD001B04
#define NV_D3D_P_Z_RENDER_TRIANGLE_ZETA_FROM_VIDEO_MEMORY       0xDD001B05
#define NV_D3D_FLOATING_RENDER_TRIANGLE_ZETA                    0xDD001B06
#define NV_D3D_FLOATING_P_I_RENDER_TRIANGLE_ZETA                0xDD001B07
#define NV_D3D_FLOATING_P_Z_RENDER_TRIANGLE_ZETA                0xDD001B08

// floating dma contexts for DVD acceleration
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V            0xDD001C00
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V            0xDD001C00
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V0           0xDD001C00
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V0           0xDD001C00
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V1           0xDD001C01
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V1           0xDD001C01
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V2           0xDD001C02
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V2           0xDD001C02
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V3           0xDD001C03
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V3           0xDD001C03
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V4           0xDD001C04
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V4           0xDD001C04
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V5           0xDD001C05
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V5           0xDD001C05
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V6           0xDD001C06
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V6           0xDD001C06
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V7           0xDD001C07
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V7           0xDD001C07
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V8           0xDD001C08
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V8           0xDD001C08
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V9           0xDD001C09
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V9           0xDD001C09
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2VA           0xDD001C0A
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2VA           0xDD001C0A
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2VB           0xDD001C0B
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2VB           0xDD001C0B
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2VC           0xDD001C0C
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2VC           0xDD001C0C
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2VD           0xDD001C0D
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2VD           0xDD001C0D
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2VE           0xDD001C0E
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2VE           0xDD001C0E
#define NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2VF           0xDD001C0F
#define NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2VF           0xDD001C0F

#define NV_DD_DMA_FLOATING_MC_IDCT0_CONTEXT_DMA_FROM_MEMORY     0xDD001D00
#define NV_DD_DMA_FLOATING_MC_IDCT1_CONTEXT_DMA_FROM_MEMORY     0xDD001D01
#define NV_DD_DMA_FLOATING_MC_IDCT2_CONTEXT_DMA_FROM_MEMORY     0xDD001D02
#define NV_DD_DMA_FLOATING_MC_IDCT3_CONTEXT_DMA_FROM_MEMORY     0xDD001D03


/* Patch interior */

#define NV_DD_IMAGE_ROP_AND_1                                   0xDD002110
#define NV_DD_P_I_IMAGE_ROP_AND_1                               0xDD002111
#define NV_DD_IMAGE_COLOR_KEY_1                                 0xDD002130
#define NV_DD_P_I_IMAGE_COLOR_KEY_1                             0xDD002131

#define NV_DD_IMAGE_ROP_AND_2                                   0xDD002210
#define NV_DD_P_I_IMAGE_ROP_AND_2                               0xDD002211
#define NV_DD_IMAGE_STENCIL_2                                   0xDD002220
#define NV_DD_P_I_IMAGE_STENCIL_2                               0xDD002221
#define NV_DD_IMAGE_COLOR_KEY_2                                 0xDD002230
#define NV_DD_P_I_IMAGE_COLOR_KEY_2                             0xDD002231

#define NV_DD_IMAGE_ROP_AND_3                                   0xDD002310
#define NV_DD_P_I_IMAGE_ROP_AND_3                               0xDD002311
#define NV_DD_IMAGE_STENCIL_3                                   0xDD002320
#define NV_DD_P_I_IMAGE_STENCIL_3                               0xDD002321
#define NV_DD_IMAGE_COLOR_KEY_3                                 0xDD002330
#define NV_DD_P_I_IMAGE_COLOR_KEY_3                             0xDD002331

#define NV_DD_IMAGE_ROP_AND_4                                   0xDD002410
#define NV_DD_P_I_IMAGE_ROP_AND_4                               0xDD002411
#define NV_DD_IMAGE_COLOR_KEY_4                                 0xDD002430
#define NV_DD_P_I_IMAGE_COLOR_KEY_4                             0xDD002431

#define NV_DD_IMAGE_ROP_AND_5                                   0xDD002510
#define NV_DD_P_I_IMAGE_ROP_AND_5                               0xDD002511
#define NV_DD_IMAGE_COLOR_KEY_5                                 0xDD002530
#define NV_DD_P_I_IMAGE_COLOR_KEY_5                             0xDD002531

#define NV_DD_IMAGE_ROP_AND_C                                   0xDD002C10
#define NV_DD_P_I_IMAGE_ROP_AND_C                               0xDD002C11

#define NV_DD_IMAGE_ROP_AND_D                                   0xDD002D10
#define NV_DD_P_I_IMAGE_ROP_AND_D                               0xDD002D11
#define NV_DD_IMAGE_COLOR_KEY_D                                 0xDD002D30
#define NV_DD_P_I_IMAGE_COLOR_KEY_D                             0xDD002D31

#define NV_DD_IMAGE_ROP_AND_E                                   0xDD002E10
#define NV_DD_P_I_IMAGE_ROP_AND_E                               0xDD002E11
#define NV_DD_IMAGE_STENCIL_E                                   0xDD002E20
#define NV_DD_P_I_IMAGE_STENCIL_E                               0xDD002E21
#define NV_DD_IMAGE_COLOR_KEY_E                                 0xDD002E30
#define NV_DD_P_I_IMAGE_COLOR_KEY_E                             0xDD002E31

#define NV_DD_IMAGE_STENCIL_F                                   0xDD002F20
#define NV_DD_P_I_IMAGE_STENCIL_F                               0xDD002F21

#define NV_DD_IMAGE_STENCIL_F3                                  0xDD002F30
#define NV_DD_P_I_IMAGE_STENCIL_F3                              0xDD002F31

#define NV_DD_IMAGE_STENCIL_FF                                  0xDD002FF0
#define NV_DD_P_I_IMAGE_STENCIL_FF                              0xDD002FF1


/* Patch back end */

#define NV_DD_SURFACES_2D                                       0xDD003F00
#define NV_DD_DST_IMAGE_IN_MEMORY                               0xDD003F00
#define NV_DD_P_I_DST_IMAGE_IN_MEMORY                           0xDD003F01
#define NV_DD_SURFACES_2D_A8R8G8B8                              0xDD003F02
#define NV_DD_SURFACES_3D                                       0xDD003F03
#define NV_DD_SURFACES_SWIZZLED                                 0xDD003F04
#define NV_DD_SRC_IMAGE_IN_MEMORY                               0xDD003F10
#define NV_DD_P_I_SRC_IMAGE_IN_MEMORY                           0xDD003F11
#define NV_DD_ZETA_IMAGE_IN_MEMORY                              0xDD003F20
#define NV_D3D_ZETA_BUFFER_IN_MEMORY                            0xDD003F30
#define NV_D3D_P_Z_ZETA_BUFFER_IN_MEMORY                        0xDD003F31

#define NV_DD_PRIMARY_VIDEO_FROM_MEMORY                         0xDD003F80
#define NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY                     0xDD003F81
#define NV_DD_YUV422_VIDEO_FROM_MEMORY                          0xDD003F90
#define NV_DD_P_V_YUV422_VIDEO_FROM_MEMORY                      0xDD003F91
#define NV_DD_VIDEO_SCALER                                      0xDD003FA0
#define NV_DD_P_V_VIDEO_SCALER                                  0xDD003FA1
#define NV_DD_VIDEO_COLOR_KEY                                   0xDD003FB0
#define NV_DD_P_V_VIDEO_COLOR_KEY                               0xDD003FB1
#define NV_DD_SHARED_VIDEO_COLORMAP                             0xDD003FC0
#define NV_DD_P_V_SHARED_VIDEO_COLORMAP                         0xDD003FC1

#define NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT            0xDD003FD0
#define NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT            0xDD003FD1
#define NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT            0xDD003FD2
#define NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT           0xDD003FD3
#define NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT             0xDD003FE0
#define NV_DD_VIDEO_MEMORY_U_TO_VIDEO_MEMORY_FORMAT             0xDD003FE1
#define NV_DD_VIDEO_MEMORY_V_TO_VIDEO_MEMORY_FORMAT             0xDD003FE2
#define NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT            0xDD003FE3
#define NV_DD_VIDEO_MEMORY_TO_OVERLAY_SHADOW_FORMAT             0xDD003FE4
#define NV_DD_OVERLAY_SHADOW_TO_VIDEO_MEMORY_FORMAT             0xDD003FE5
#define NV_DD_DMABLT_TO_VID                                     0xDD003FE6
#define NV_DD_VIDEO_SINK                                        0xDD003FF0

#define NV_DD_COMMON_DMA_BUFFER                                 0xDD004000
#define NV_DD_PIO_CONTEXT_ERROR_TO_MEMORY                       0xDD004010
#define NV_DD_DEV_PIO                                           0xDD004020

#define NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY                0xDD005000
#define NV_DD_DMA_CONTEXT_ERROR_TO_MEMORY                       0xDD005010
#define NV_DD_DEV_DMA                                           0xDD005020

#define NV_DD_DEV_VIDEO                                         0xDD006020

#define NV_DD_DEV_VPE                                           0xDD007020

#define NV_DD_VIDEO_LUT_CURSOR_DAC                              0xDD008000

#define NV_DD_GLOBALDATA_MEMORY                                 0xDD008020
#define NV_DD_GLOBALDATA_MEMORY_NVDD32_CONTEXT                  0xDD008021
#define NV_DD_IPM                                               0xDD008022
#define NV_DD_CONTEXTDMA_MEMORY                                 0xDD008030

#define NV_DD_INDEXED_IMAGE_FROM_CPU                            0xDD00A000

#ifdef WINNT
// Pushbuf ID needed for Win2k RM AGP allocator.
#define NV_DD_AGP_PUSHBUF                                       0xDD00A001
#endif // WINNT

// VPP objects
#define NV_DD_SWIZZLE_BLIT                                      0xDD00B000
#define NV_DD_DMA_OVERLAY                                       0xDD00B010
#define NV_DD_V2V_FORMAT_Y                                      0xDD00B020
#define NV_DD_ALPHA_BLIT                                        0xDD00B030

// Events
#define NV_DD_EVENT_IDLE_SYNC                                   0xDD00C000
#define NV_DD_EVENT_CONVERT_SYNC                                0xDD00C001
#define NV_DD_EVENT_DVD_SUBPICTURE                              0xDD00C002
#define NV_DD_EVENT_ALPHA_BLIT_SYNC                             0xDD00C003
#define NV_DD_EVENT_VIDEO_TO_SYSTEM_SYNC                        0xDD00C004
#define NV_DD_EVENT_SYSTEM_TO_VIDEO_SYNC                        0xDD00C005
#define NV_DD_EVENT_COMBINER_SYNC                               0xDD00C006
#define NV_DD_EVENT_DMABLT_TO_VID                               0xDD00C007
#define NV_DD_EVENT_OVL_FLIP1                                   0xDD00C008
#define NV_DD_EVENT_OVL_FLIP2                                   0xDD00C009
#define NV_DD_EVENT_DFILTER_SYNC                                0xDD00C00A

// Floating context DMA's for system surfaces (reserve a block of 512 for now)
#define MAX_FLOATING_CONTEXT_DMA                                0x200
#define MASK_FLOATING_CONTEXT_DMA_ID                            0xFFFFF001
#define FLOATING_CONTEXT_DMA_ID                                 0xDD00D001
#define NV_DD_FLOATING_CONTEXT_DMA_BASE                         0xDD00D001
#define NV_DD_FLOATING_CONTEXT_DMA_0                            0xDD00D001
#define NV_DD_FLOATING_CONTEXT_DMA_7FF                          0xDD00DFFF


// D3D objects

// naming convention:
// D3Dxxxyy
// where xxx = object class number
//        yy = enumerator, in case we have more than one object of type xxx

// DMA contexts.

#ifdef WINNT  // Still needed by old DX6 Win2k driver!!
#define D3D_CONTEXT_LINEAR_TEXTURE_FROM_MEMORY                  0xD3D00201
#define D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY                0xD3D00202
#endif // Still needed by old DX6 win2k driver

#define D3D_CONTEXT_xxx_unused_xxx                              0xD3D00201
#define D3D_CONTEXT_DMA_HOST_MEMORY                             0xD3D00202  // pci or agp context (growable for surfaces)

// Surface contexts.
#define D3D_CONTEXT_SURFACE_SWIZZLED                            0xD3D05201
#define D3D_CONTEXT_SURFACES_ARGB_ZS                            0xD3D05301

// D3D 2D rendering objects.
#define D3D_RENDER_SOLID_RECTANGLE                              0xD3D05E01  // Currently used for Z and Buffer Clears
#define D3D_RENDER_SOLID_RECTANGLE_2                            0xD3D05E02  // pseudo texblit notifiers
#define D3D_SCALED_IMAGE_FROM_MEMORY                            0xD3D07701  // Used for swizzling textures.

// D3D Triangle objects.
#define D3D_DX5_TEXTURED_TRIANGLE                               0xD3D05401  // DX5 class triangle renderer.
#define D3D_DX6_MULTI_TEXTURE_TRIANGLE                          0xD3D05501  // Multi Texture/Stencil triangle renderer.

// Celcius primitive
#define D3D_CELSIUS_PRIMITIVE                                   0xD3D05601

// Kelvin primitive
#define D3D_KELVIN_PRIMITIVE                                    0xD3D09701

// IPM allocator
#define IPM_MEMORY_OBJECT_LO                                    0x10000000  // low
#define IPM_MEMORY_OBJECT_HI                                    0x1fffffff  // high

